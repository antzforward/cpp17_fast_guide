#include "LabContext.h"
#include "ActiveLesson/ArrayParametersContract.h"

#ifndef UELAB_LEVEL
#define UELAB_LEVEL 0
#endif

#include "ActiveExercise.inl"

namespace ArrayParametersTests
{
#if UELAB_LEVEL == 0 || UELAB_LEVEL == 1
void TestBuild(FLabContext& Lab)
{
    const int32 Counts[] = {0, 1, 2, 31, 32, 257};
    const int32 FirstValues[] = {-13, 0, 701};
    for (int32 Count : Counts)
    {
        for (int32 First : FirstValues)
        {
            FArrayBuildResult Result = BuildSequentialBatch(First, Count);
            const FString Case = FString::Printf(TEXT("L1 First=%d Count=%d"), First, Count);
            Lab.Require(Result.NumImmediatelyAfterReserve == 0,
                Case + TEXT(": Reserve must not create elements"));
            Lab.Require(Result.CapacityImmediatelyAfterReserve >= Count,
                Case + TEXT(": reserved capacity must be at least Count"));
            Lab.Require(Result.Values.Num() == Count,
                Case + TEXT(": Num must equal Count"));
            Lab.Require(Result.Values.Max() == Result.CapacityImmediatelyAfterReserve,
                Case + TEXT(": filling the reserved range must not grow capacity"));
            bool bSequenceCorrect = Result.Values.Num() == Count;
            if (bSequenceCorrect)
            {
                for (int32 Index = 0; Index < Count; ++Index)
                {
                    bSequenceCorrect &= Result.Values[Index] == First + Index;
                }
            }
            Lab.Require(bSequenceCorrect, Case + TEXT(": sequence must match First + Index"));
        }
    }
    Lab.Note(TEXT("L1: Reserve changes storage capacity; Emplace creates the sequence."));
}
#endif

#if UELAB_LEVEL == 0 || UELAB_LEVEL == 2
void CheckRemoval(FLabContext& Lab, int32 Size, int32 Index, int32 Count, bool bKeepOrder)
{
    // Fixture 不调用 Level 1；单独实现 Level 2 即可验收。
    TArray<int32> Values;
    Values.Reserve(Size * 4 + 64);
    for (int32 Offset = 0; Offset < Size; ++Offset)
    {
        Values.Emplace(1000 + Offset);
    }
    const int32 CapacityBefore = Values.Max();
    TArray<int32> Expected;
    for (int32 Offset = 0; Offset < Size; ++Offset)
    {
        if (Offset < Index || Offset >= Index + Count)
        {
            Expected.Add(1000 + Offset);
        }
    }

    RemoveBatchEntries(Values, Index, Count, bKeepOrder);
    const FString Case = FString::Printf(TEXT("L2 Size=%d Index=%d Count=%d KeepOrder=%d"),
        Size, Index, Count, bKeepOrder ? 1 : 0);
    Lab.Require(Values.Num() == Size - Count, Case + TEXT(": resulting Num"));
    Lab.Require(Values.Max() == CapacityBefore, Case + TEXT(": shrinking must be disabled"));
    if (bKeepOrder)
    {
        Lab.Require(Values == Expected, Case + TEXT(": survivor order must be preserved"));
    }
    else
    {
        // 无序删除只承诺保留哪些值，不把某个版本的具体交换顺序当成契约。
        Values.Sort();
        Expected.Sort();
        Lab.Require(Values == Expected, Case + TEXT(": survivor multiset must match"));
    }
}

void TestRemoval(FLabContext& Lab)
{
    for (bool bKeepOrder : {true, false})
    {
        CheckRemoval(Lab, 12, 3, 6, bKeepOrder);
        CheckRemoval(Lab, 12, 0, 2, bKeepOrder);
        CheckRemoval(Lab, 12, 10, 2, bKeepOrder);
        CheckRemoval(Lab, 12, 5, 0, bKeepOrder);
        CheckRemoval(Lab, 12, 0, 12, bKeepOrder);
        CheckRemoval(Lab, 0, 0, 0, bKeepOrder);
        CheckRemoval(Lab, 2048, 8, 2010, bKeepOrder);
    }
    Lab.Note(TEXT("L2: ordered output and unordered membership are different contracts; capacity is retained."));
}
#endif

#if UELAB_LEVEL == 0 || UELAB_LEVEL == 3
int32 ValidateGrowth(FLabContext& Lab, const FArrayGrowthResult& Result, int32 Count, bool bReserved)
{
    const FString Case = FString::Printf(TEXT("L3 Count=%d Reserve=%d"), Count, bReserved ? 1 : 0);
    Lab.Require(Result.NumBeforeFill == 0, Case + TEXT(": array is empty before fill"));
    Lab.Require(bReserved ? Result.CapacityBeforeFill >= Count : Result.CapacityBeforeFill == 0,
        Case + TEXT(": initial capacity matches reservation mode"));
    Lab.Require(Result.Values.Num() == Count, Case + TEXT(": final Num"));
    Lab.Require(Result.CapacityAfterEachAdd.Num() == Count, Case + TEXT(": one capacity sample per add"));

    bool bValuesCorrect = Result.Values.Num() == Count;
    if (bValuesCorrect)
    {
        for (int32 Index = 0; Index < Count; ++Index)
        {
            bValuesCorrect &= Result.Values[Index] == Index;
        }
    }
    Lab.Require(bValuesCorrect, Case + TEXT(": payload must be 0..Count-1"));
    if (Result.CapacityAfterEachAdd.Num() != Count)
    {
        return INDEX_NONE;
    }

    int32 PreviousCapacity = Result.CapacityBeforeFill;
    int32 GrowthEvents = 0;
    bool bCapacityValid = true;
    for (int32 Index = 0; Index < Count; ++Index)
    {
        const int32 Capacity = Result.CapacityAfterEachAdd[Index];
        bCapacityValid &= Capacity >= Index + 1;
        bCapacityValid &= Capacity >= PreviousCapacity;
        if (Capacity > PreviousCapacity)
        {
            ++GrowthEvents;
        }
        PreviousCapacity = Capacity;
    }
    Lab.Require(bCapacityValid, Case + TEXT(": capacities must fit live elements and never decrease"));
    Lab.Require(Result.Values.Max() == PreviousCapacity, Case + TEXT(": final capacity matches trace"));
    if (bReserved)
    {
        Lab.Require(GrowthEvents == 0, Case + TEXT(": filling a reserved range must have zero growth events"));
    }
    else if (Count > 0)
    {
        Lab.Require(GrowthEvents > 0, Case + TEXT(": initially empty storage must grow"));
    }
    Lab.Metric(FString::Printf(TEXT("L3.Count%d.Reserve%d.GrowthDuringFill"), Count, bReserved ? 1 : 0), GrowthEvents);
    Lab.Metric(FString::Printf(TEXT("L3.Count%d.Reserve%d.FinalCapacity"), Count, bReserved ? 1 : 0), Result.Values.Max());
    Lab.Metric(FString::Printf(TEXT("L3.Count%d.Reserve%d.FinalSlack"), Count, bReserved ? 1 : 0),
        Result.Values.Max() - Result.Values.Num());
    return GrowthEvents;
}

void TestGrowth(FLabContext& Lab)
{
    const int32 RequestedCount = Lab.Int(TEXT("Count"), 2048);
    if (!Lab.Require(RequestedCount >= 64 && RequestedCount <= 1000000,
        TEXT("L3: Count must be between 64 and 1000000")))
    {
        return;
    }
    const int32 Counts[] = {0, 1, 64, 257, RequestedCount};
    for (int32 Count : Counts)
    {
        FArrayGrowthResult Reserved = FillBatchWithGrowthTrace(Count, true);
        FArrayGrowthResult Natural = FillBatchWithGrowthTrace(Count, false);
        const int32 ReservedGrowth = ValidateGrowth(Lab, Reserved, Count, true);
        const int32 NaturalGrowth = ValidateGrowth(Lab, Natural, Count, false);
        Lab.Require(Reserved.Values == Natural.Values,
            FString::Printf(TEXT("L3 Count=%d: reservation must not change payload"), Count));
        if (Count >= 64 && ReservedGrowth >= 0 && NaturalGrowth >= 0)
        {
            Lab.Require(NaturalGrowth > ReservedGrowth,
                FString::Printf(TEXT("L3 Count=%d: Reserve must reduce growth during fill"), Count));
        }
    }
    Lab.Note(TEXT("L3: growth counts describe capacity changes, not bytes copied, pointer moves, constructor calls, or elapsed time."));
}
#endif
} // namespace ArrayParametersTests

int32 RunExperiment(FLabContext& Lab)
{
#if UELAB_LEVEL == 0 || UELAB_LEVEL == 1
    ArrayParametersTests::TestBuild(Lab);
#endif
#if UELAB_LEVEL == 0 || UELAB_LEVEL == 2
    ArrayParametersTests::TestRemoval(Lab);
#endif
#if UELAB_LEVEL == 0 || UELAB_LEVEL == 3
    ArrayParametersTests::TestGrowth(Lab);
#endif
    return Lab.Failures == 0 ? 0 : 1;
}
