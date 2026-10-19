#include "LabContext.h"
#include "Delegates/Delegate.h"
#include "HAL/Event.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformTLS.h"
#include "HAL/PlatformTime.h"
#include "Tasks/Task.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FValueFeedback, int32, int64);

int32 RunExperiment(FLabContext& Lab)
{
    const int32 Count = Lab.Int(TEXT("Count"), 10000);
    const int32 Scale = Lab.Int(TEXT("Scale"), 3);
    const bool ValidCount = Lab.Require(Count >= 1 && Count <= 1000000,
        TEXT("Count must be in [1, 1000000]"));
    const bool ValidScale = Lab.Require(Scale >= 1 && Scale <= 1000,
        TEXT("Scale must be in [1, 1000]"));
    const bool OnGameThread = Lab.Require(IsInGameThread(), TEXT("generation and consumption start on the game thread"));
    const bool HasWorkers = Lab.Require(FPlatformProcess::SupportsMultithreading(),
        TEXT("this flow requires a multithreaded UE process"));
    if (!ValidCount || !ValidScale || !OnGameThread || !HasWorkers) return 1;

    const uint32 MainThreadId = FPlatformTLS::GetCurrentThreadId();
    TArray<int64> Input;
    Input.Reserve(Count);
    for (int32 Index = 0; Index < Count; ++Index) Input.Add(Index);
    const int64 GeneratedChecksum = int64(Count) * (Count - 1) / 2;
    struct FProcessedValues
    {
        TArray<int64> Values;
        uint32 ThreadId = 0;
        int64 Checksum = 0;
    };

    FEvent* ComputationFinished = FPlatformProcess::GetSynchEventFromPool(true);
    if (!Lab.Require(ComputationFinished != nullptr, TEXT("completion event was allocated"))) return 1;
    const double Start = FPlatformTime::Seconds();
    auto Task = UE::Tasks::Launch(UE_SOURCE_LOCATION,
        [Values = MoveTemp(Input), Scale, ComputationFinished]() mutable
        {
            FProcessedValues Result;
            Result.ThreadId = FPlatformTLS::GetCurrentThreadId();
            for (int64& Value : Values)
            {
                Value = Value * Scale + 7;
                Result.Checksum += Value;
            }
            Result.Values = MoveTemp(Values);
            ComputationFinished->Trigger();
            return Result;
        });

    // A plain Task.Wait() may execute a queued task on the waiting thread.
    // The event first lets a worker do the calculation; Task.Wait then joins it.
    ComputationFinished->Wait();
    Task.Wait();
    FProcessedValues Result = MoveTemp(Task.GetResult());
    FPlatformProcess::ReturnSynchEventToPool(ComputationFinished);
    const double PipelineMs = (FPlatformTime::Seconds() - Start) * 1000.0;

    Lab.Require(Result.ThreadId != MainThreadId, TEXT("value transformation actually ran on a worker thread"));
    Lab.Require(IsInGameThread() && FPlatformTLS::GetCurrentThreadId() == MainThreadId,
        TEXT("consumption resumes on the original game thread"));
    int64 ConsumedChecksum = 0;
    bool ValuesMatch = Result.Values.Num() == Count;
    for (int32 Index = 0; Index < Result.Values.Num(); ++Index)
    {
        ConsumedChecksum += Result.Values[Index];
        ValuesMatch = ValuesMatch && Result.Values[Index] == int64(Index) * Scale + 7;
    }
    const int64 ExpectedChecksum = GeneratedChecksum * Scale + int64(Count) * 7;
    Lab.Require(ValuesMatch && ConsumedChecksum == ExpectedChecksum
        && Result.Checksum == ExpectedChecksum, TEXT("all processed values and checksums agree"));

    int32 FeedbackCount = 0;
    int32 AcknowledgedValues = 0;
    int64 FeedbackChecksum = 0;
    uint32 FeedbackThreadId = 0;
    FValueFeedback Feedback;
    Feedback.AddLambda([&](int32 Num, int64 Checksum)
    {
        ++FeedbackCount;
        AcknowledgedValues = Num;
        FeedbackChecksum = Checksum;
        FeedbackThreadId = FPlatformTLS::GetCurrentThreadId();
    });
    Feedback.Broadcast(Result.Values.Num(), ConsumedChecksum);
    Lab.Require(FeedbackCount == 1 && AcknowledgedValues == Count
        && FeedbackChecksum == ExpectedChecksum && FeedbackThreadId == MainThreadId,
        TEXT("synchronous delegate reports one complete result on the game thread"));

    Lab.Metric(TEXT("generated_count"), Count);
    Lab.Metric(TEXT("generated_checksum"), double(GeneratedChecksum));
    Lab.Metric(TEXT("processed_count"), Result.Values.Num());
    Lab.Metric(TEXT("processed_checksum"), double(Result.Checksum));
    Lab.Metric(TEXT("consumed_count"), Result.Values.Num());
    Lab.Metric(TEXT("consumed_checksum"), double(ConsumedChecksum));
    Lab.Metric(TEXT("feedback_count"), FeedbackCount);
    Lab.Metric(TEXT("feedback_checksum"), double(FeedbackChecksum));
    Lab.Metric(TEXT("generation_thread_id"), MainThreadId);
    Lab.Metric(TEXT("processing_thread_id"), Result.ThreadId);
    Lab.Metric(TEXT("consumption_thread_id"), FPlatformTLS::GetCurrentThreadId());
    Lab.Metric(TEXT("feedback_thread_id"), FeedbackThreadId);
    Lab.Metric(TEXT("pipeline_ms"), PipelineMs);
    Lab.Note(TEXT("This is a local CPU value-data flow: generate -> UE task -> join -> consume -> ordinary delegate. It is not a network or rendering pipeline. Blocking join makes ownership and ordering visible; a running game should schedule continuations instead."));
    return Lab.Failures == 0 ? 0 : 1;
}
