#include "LabContext.h"
#include "HAL/PlatformTime.h"

// Run with -Count=10000 -Reserve=0 -Shrink=0, then vary one parameter.
int32 RunExperiment(FLabContext& Lab)
{
    const int32 Count = Lab.Int(TEXT("Count"), 10000);
    const int32 Reserve = Lab.Int(TEXT("Reserve"), 0);
    const int32 Shrink = Lab.Int(TEXT("Shrink"), 0);
    const bool ValidCount = Lab.Require(Count >= 2 && Count <= 1000000,
        TEXT("Count must be in [2, 1000000]"));
    const bool ValidReserve = Lab.Require(Reserve == 0 || Reserve == 1,
        TEXT("Reserve must be 0 or 1"));
    const bool ValidShrink = Lab.Require(Shrink == 0 || Shrink == 1,
        TEXT("Shrink must be 0 or 1"));
    if (!ValidCount || !ValidReserve || !ValidShrink) return 1;

    struct FCapacityPoint { int32 Num; int32 Capacity; };
    TArray<FCapacityPoint> Trace;
    Trace.Reserve(128); // Record first, print later: logging would dominate timings.
    TArray<int32> Values;
    int32 PreviousCapacity = Values.Max();
    int32 CapacityChanges = 0;
    int32 PeakCapacity = PreviousCapacity;
    const double BuildStart = FPlatformTime::Seconds();
    if (Reserve != 0)
    {
        Values.Reserve(Count);
        if (Values.Max() != PreviousCapacity)
        {
            ++CapacityChanges;
            Trace.Add({Values.Num(), Values.Max()});
            PreviousCapacity = Values.Max();
            PeakCapacity = Values.Max();
        }
    }
    for (int32 Index = 0; Index < Count; ++Index)
    {
        Values.Add(Index);
        if (Values.Max() != PreviousCapacity)
        {
            ++CapacityChanges;
            Trace.Add({Values.Num(), Values.Max()});
            PreviousCapacity = Values.Max();
            PeakCapacity = FMath::Max(PeakCapacity, Values.Max());
        }
    }
    const double BuildMs = (FPlatformTime::Seconds() - BuildStart) * 1000.0;
    const SIZE_T BytesBeforeRemoval = Values.GetAllocatedSize();
    bool DataMatches = Values.Num() == Count;
    for (int32 Index = 0; Index < Values.Num(); ++Index)
        DataMatches = DataMatches && Values[Index] == Index;
    Lab.Require(DataMatches, TEXT("all generated values survived insertion"));

    // Removing one middle item moves the last value into its slot. Order changes.
    const int32 Hole = (Count - 1) / 2;
    const int32 OldTail = Values.Last();
    const int32 CapacityBeforeRemoval = Values.Max();
    Values.RemoveAtSwap(Hole, 1, EAllowShrinking::No);
    Lab.Require(Values.Num() == Count - 1 && Values[Hole] == OldTail,
        TEXT("RemoveAtSwap fills the hole with the old tail"));
    Lab.Require(Values.Max() == CapacityBeforeRemoval,
        TEXT("RemoveAtSwap with EAllowShrinking::No retains capacity"));
    const int64 ExpectedSum = int64(Count) * (Count - 1) / 2 - Hole;
    int64 ActualSum = 0;
    for (const int32 Value : Values) ActualSum += Value;
    Lab.Require(ActualSum == ExpectedSum, TEXT("only the selected value was removed"));

    // Discard most values to make retained capacity and explicit Shrink observable.
    const int32 KeptCount = FMath::Max(1, Values.Num() / 4);
    Values.SetNum(KeptCount, EAllowShrinking::No);
    const int32 CapacityBeforeShrink = Values.Max();
    const double ShrinkStart = FPlatformTime::Seconds();
    if (Shrink != 0) Values.Shrink();
    const double ShrinkMs = (FPlatformTime::Seconds() - ShrinkStart) * 1000.0;
    Lab.Require(Values.Max() >= Values.Num(), TEXT("capacity always covers live values"));
    Lab.Require(Shrink != 0 ? Values.Max() <= CapacityBeforeShrink
                            : Values.Max() == CapacityBeforeShrink,
        TEXT("explicit shrink policy matches the observed capacity"));

    for (const FCapacityPoint& Point : Trace)
        Lab.Note(FString::Printf(TEXT("capacity trace: Num=%d Max=%d"), Point.Num, Point.Capacity));
    Lab.Metric(TEXT("capacity_changes"), CapacityChanges);
    Lab.Metric(TEXT("peak_capacity"), PeakCapacity);
    Lab.Metric(TEXT("bytes_before_removal"), double(BytesBeforeRemoval));
    Lab.Metric(TEXT("bytes_after_policy"), double(Values.GetAllocatedSize()));
    Lab.Metric(TEXT("kept_count"), Values.Num());
    Lab.Metric(TEXT("build_ms"), BuildMs);
    Lab.Metric(TEXT("shrink_ms"), ShrinkMs);
    Lab.Note(TEXT("Capacity is measured from this real UE allocator. No fixed growth formula is assumed. Timings include trace bookkeeping; compare repeated runs."));
    return Lab.Failures == 0 ? 0 : 1;
}
