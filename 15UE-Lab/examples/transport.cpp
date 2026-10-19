// Real UE archives + packed integers + zlib. Local buffer round-trip, not a network protocol.
#include "LabContext.h"
#include "HAL/PlatformTime.h"
#include "Misc/Compression.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

namespace TransportLab
{
constexpr uint32 Magic = 0x55454C42;
constexpr uint16 Version = 1;
constexpr uint32 MaxRawBytes = 4 * 1024 * 1024;
constexpr uint32 MaxItems = 10000;
constexpr int32 EnvelopeBytes = 9; // uint8 flag + uint32 raw bytes + uint32 payload bytes

struct FSample
{
    uint32 Id = 0;
    int64 Resource = 0; // SLG values stay exact integers throughout this experiment.
    double Position = 0;
};

uint32 ZigZag(int32 Value)
{
    return Value < 0 ? static_cast<uint32>(-static_cast<int64>(Value) * 2 - 1)
                     : static_cast<uint32>(Value) * 2;
}

int32 UnZigZag(uint32 Value)
{
    return static_cast<int32>((Value & 1) ? -static_cast<int64>(Value / 2) - 1
                                         : static_cast<int64>(Value / 2));
}

// Match UE's writer format (continuation in the LOW bit), with explicit input bounds.
// The general FArchive reader is intended for trusted archives; this exercise accepts damaged input.
bool ReadPacked(FMemoryReader& Reader, uint32& Value)
{
    Value = 0;
    for (uint32 ByteIndex = 0; ByteIndex < 5; ++ByteIndex)
    {
        if (Reader.IsError() || Reader.Tell() >= Reader.TotalSize()) return false;
        uint8 Byte = 0;
        Reader << Byte;
        if (Reader.IsError() || (ByteIndex == 4 && (Byte & 0xE1) != 0)) return false;
        Value |= static_cast<uint32>(Byte >> 1) << (ByteIndex * 7);
        if ((Byte & 1) == 0) return true;
    }
    return false;
}

TArray<uint8> Encode(const TArray<FSample>& Samples, int32 Start, int32 Count, int32 Quantum)
{
    TArray<uint8> Bytes;
    FMemoryWriter Writer(Bytes, true);
    uint32 Tag = Magic;
    uint16 Schema = Version;
    uint32 Scale = static_cast<uint32>(Quantum);
    uint32 Items = static_cast<uint32>(Count);
    Writer << Tag << Schema;
    Writer.SerializeIntPacked(Scale);
    Writer.SerializeIntPacked(Items);
    uint32 PreviousId = 0;
    for (int32 Index = Start; Index < Start + Count; ++Index)
    {
        uint32 Delta = Samples[Index].Id - PreviousId;
        int64 Resource = Samples[Index].Resource;
        uint32 Position = ZigZag(FMath::RoundToInt(Samples[Index].Position * Quantum));
        Writer.SerializeIntPacked(Delta);
        Writer << Resource;
        Writer.SerializeIntPacked(Position);
        PreviousId = Samples[Index].Id;
    }
    return Bytes;
}

bool Wrap(const TArray<uint8>& Raw, bool AllowCompression, TArray<uint8>& Frame, int32& CandidateBytes, bool& Compressed)
{
    TArray<uint8> Candidate;
    CandidateBytes = Raw.Num();
    if (AllowCompression)
    {
        int32 Capacity = FCompression::CompressMemoryBound(NAME_Zlib, Raw.Num());
        if (Capacity <= 0) return false;
        Candidate.SetNumUninitialized(Capacity);
        if (!FCompression::CompressMemory(NAME_Zlib, Candidate.GetData(), Capacity, Raw.GetData(), Raw.Num())) return false;
        Candidate.SetNum(Capacity, EAllowShrinking::No);
        CandidateBytes = Capacity;
    }
    Compressed = AllowCompression && Candidate.Num() < Raw.Num();
    const TArray<uint8>& Payload = Compressed ? Candidate : Raw;
    FMemoryWriter Writer(Frame, true);
    uint8 Flag = Compressed ? 1 : 0;
    uint32 RawSize = static_cast<uint32>(Raw.Num());
    uint32 PayloadSize = static_cast<uint32>(Payload.Num());
    Writer << Flag << RawSize << PayloadSize;
    Writer.Serialize(const_cast<uint8*>(Payload.GetData()), Payload.Num());
    return !Writer.IsError();
}

bool Decode(const TArray<uint8>& Frame, TArray<FSample>& Out, int32& OutQuantum)
{
    // Validate lengths before allocating or decompressing untrusted data.
    if (Frame.Num() < EnvelopeBytes) return false;
    FMemoryReader Envelope(Frame, true);
    uint8 Flag = 0;
    uint32 RawSize = 0, PayloadSize = 0;
    Envelope << Flag << RawSize << PayloadSize;
    if (Flag > 1 || RawSize == 0 || RawSize > MaxRawBytes || PayloadSize == 0 ||
        PayloadSize > MaxRawBytes || PayloadSize != static_cast<uint32>(Frame.Num() - EnvelopeBytes)) return false;
    TArray<uint8> Raw;
    Raw.SetNumUninitialized(RawSize);
    const uint8* Payload = Frame.GetData() + EnvelopeBytes;
    if (Flag)
    {
        if (!FCompression::UncompressMemory(NAME_Zlib, Raw.GetData(), RawSize, Payload, PayloadSize)) return false;
    }
    else
    {
        if (PayloadSize != RawSize) return false;
        FMemory::Memcpy(Raw.GetData(), Payload, RawSize);
    }

    FMemoryReader Reader(Raw, true);
    uint32 Tag = 0, Scale = 0, Count = 0;
    uint16 Schema = 0;
    if (Raw.Num() < 8) return false;
    Reader << Tag << Schema;
    if (!ReadPacked(Reader, Scale) || !ReadPacked(Reader, Count)) return false;
    if (Reader.IsError() || Tag != Magic || Schema != Version || Scale < 1 || Scale > 1000 || Count > MaxItems) return false;
    // Each element requires at least one id byte, eight resource bytes, one position byte.
    if (static_cast<int64>(Count) * 10 > Reader.TotalSize() - Reader.Tell()) return false;
    Out.Reset();
    Out.Reserve(Count);
    uint32 PreviousId = 0;
    for (uint32 Index = 0; Index < Count; ++Index)
    {
        uint32 Delta = 0, Position = 0;
        int64 Resource = 0;
        if (!ReadPacked(Reader, Delta) || Reader.TotalSize() - Reader.Tell() < 8) return false;
        Reader << Resource;
        if (!ReadPacked(Reader, Position)) return false;
        if (Reader.IsError() || Delta == 0 || Delta > MAX_uint32 - PreviousId) return false;
        PreviousId += Delta;
        Out.Add({PreviousId, Resource, static_cast<double>(UnZigZag(Position)) / Scale});
    }
    if (Reader.Tell() != Reader.TotalSize()) return false;
    OutQuantum = static_cast<int32>(Scale);
    return true;
}
}

int32 RunExperiment(FLabContext& Lab)
{
    using namespace TransportLab;
    const int32 Count = Lab.Int(TEXT("Count"), 512);
    const int32 BatchSize = Lab.Int(TEXT("BatchSize"), 64);
    const int32 Quantum = Lab.Int(TEXT("Quantum"), 10);
    const int32 Compression = Lab.Int(TEXT("Compression"), 1);
    if (!Lab.Require(Count >= 1 && Count <= static_cast<int32>(MaxItems) && BatchSize >= 1 && BatchSize <= 4096 &&
        Quantum >= 1 && Quantum <= 1000 && (Compression == 0 || Compression == 1), TEXT("parameter bounds"))) return 1;

    TArray<FSample> Samples;
    Samples.Reserve(Count);
    for (int32 Index = 0; Index < Count; ++Index)
        Samples.Add({static_cast<uint32>(10000 + Index), 900000000000LL + (Index % 7) * 125LL, -100.0 + Index * 0.137});

    int64 RawBytes = 0, WireBytes = 0, CandidateBytes = 0, ResourceSum = 0;
    int32 Frames = 0, CompressedFrames = 0, Received = 0;
    double MaxError = 0;
    bool ExactValues = true;
    TArray<uint8> FirstFrame;
    const double Started = FPlatformTime::Seconds();
    for (int32 Start = 0; Start < Samples.Num(); Start += BatchSize)
    {
        const int32 Items = FMath::Min(BatchSize, Samples.Num() - Start);
        const TArray<uint8> Raw = Encode(Samples, Start, Items, Quantum);
        TArray<uint8> Frame;
        int32 Candidate = 0;
        bool Compressed = false;
        if (!Wrap(Raw, Compression != 0, Frame, Candidate, Compressed))
        {
            Lab.Require(false, TEXT("encode/compress frame failed"));
            return 1;
        }
        TArray<FSample> Decoded;
        int32 DecodedQuantum = 0;
        if (!Decode(Frame, Decoded, DecodedQuantum) || Decoded.Num() != Items || DecodedQuantum != Quantum)
        {
            Lab.Require(false, TEXT("bounded decode or schema round-trip failed"));
            return 1;
        }
        for (int32 Index = 0; Index < Items; ++Index)
        {
            const FSample& Original = Samples[Start + Index];
            const FSample& Restored = Decoded[Index];
            ExactValues &= Original.Id == Restored.Id && Original.Resource == Restored.Resource;
            MaxError = FMath::Max(MaxError, FMath::Abs(Original.Position - Restored.Position));
            ResourceSum += Restored.Resource;
        }
        Received += Decoded.Num();
        RawBytes += Raw.Num();
        WireBytes += Frame.Num();
        CandidateBytes += Candidate;
        ++Frames;
        CompressedFrames += Compressed ? 1 : 0;
        if (FirstFrame.IsEmpty()) FirstFrame = Frame;
    }
    const double CodecMs = (FPlatformTime::Seconds() - Started) * 1000.0;
    Lab.Require(Received == Count && ExactValues, TEXT("ids and SLG integer values survive every frame exactly"));
    Lab.Require(MaxError <= 0.5 / Quantum + 1e-8, TEXT("position error stays within chosen quantization budget"));
    Lab.Require(WireBytes <= RawBytes + Frames * EnvelopeBytes, TEXT("compression falls back when payload would grow"));

    TArray<FSample> Ignored;
    int32 IgnoredQuantum = 0;
    TArray<uint8> Truncated = FirstFrame;
    Truncated.Pop(EAllowShrinking::No);
    Lab.Require(!Decode(Truncated, Ignored, IgnoredQuantum), TEXT("truncated frame rejected"));
    TArray<uint8> Oversized;
    FMemoryWriter BadWriter(Oversized, true);
    uint8 Flag = 0;
    uint32 Huge = MaxRawBytes + 1, One = 1;
    BadWriter << Flag << Huge << One;
    uint8 Byte = 0;
    BadWriter << Byte;
    Lab.Require(!Decode(Oversized, Ignored, IgnoredQuantum), TEXT("oversized declaration rejected before allocation"));

    // Malformed packed values must stop before an out-of-range shift or an unbounded read.
    TArray<uint8> BadPacked = {1, 1, 1, 1, 1};
    FMemoryReader BadPackedReader(BadPacked, true);
    uint32 IgnoredPacked = 0;
    Lab.Require(!ReadPacked(BadPackedReader, IgnoredPacked), TEXT("unterminated packed integer rejected within five bytes"));
    TArray<uint8> ShortPacked = {1};
    FMemoryReader ShortPackedReader(ShortPacked, true);
    Lab.Require(!ReadPacked(ShortPackedReader, IgnoredPacked), TEXT("truncated packed integer rejected"));
    TArray<uint8> OverflowPacked = {1, 1, 1, 1, 32};
    FMemoryReader OverflowPackedReader(OverflowPacked, true);
    Lab.Require(!ReadPacked(OverflowPackedReader, IgnoredPacked), TEXT("packed integer overflow rejected"));

    Lab.Metric(TEXT("generated_count"), Count);
    Lab.Metric(TEXT("batch_size"), BatchSize);
    Lab.Metric(TEXT("quantum"), Quantum);
    Lab.Metric(TEXT("compression_enabled"), Compression);
    Lab.Metric(TEXT("received_count"), Received);
    Lab.Metric(TEXT("frames"), Frames);
    Lab.Metric(TEXT("raw_payload_bytes"), RawBytes);
    Lab.Metric(TEXT("candidate_compressed_bytes"), CandidateBytes);
    Lab.Metric(TEXT("wire_bytes"), WireBytes);
    Lab.Metric(TEXT("compressed_frames"), CompressedFrames);
    Lab.Metric(TEXT("max_position_error"), MaxError);
    Lab.Metric(TEXT("codec_ms"), CodecMs);
    Lab.Note(FString::Printf(TEXT("Exact resource sum = %lld"), ResourceSum));
    Lab.Note(TEXT("wire_bytes counts this local envelope only; no UDP/TCP/NetDriver, encryption or authentication is implemented."));
    Lab.Note(TEXT("FMemoryWriter layout is a same-build experiment; production protocols need explicit compatibility and endianness rules."));
    Lab.Note(TEXT("codec_ms includes first-use compressor initialization; use repeated runs and avoid treating one timing as a throughput benchmark."));
    return 0;
}
