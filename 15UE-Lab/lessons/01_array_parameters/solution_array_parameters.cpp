#include "ActiveLesson/ArrayParametersContract.h"

#ifndef UELAB_LEVEL
#define UELAB_LEVEL 0
#endif

#if UELAB_LEVEL == 0 || UELAB_LEVEL == 1
FArrayBuildResult BuildSequentialBatch(int32 First, int32 Count)
{
    FArrayBuildResult Result;
    Result.Values.Reserve(Count);
    Result.NumImmediatelyAfterReserve = Result.Values.Num();
    Result.CapacityImmediatelyAfterReserve = Result.Values.Max();
    for (int32 Offset = 0; Offset < Count; ++Offset)
    {
        Result.Values.Emplace(First + Offset);
    }
    return Result;
}
#endif

#if UELAB_LEVEL == 0 || UELAB_LEVEL == 2
void RemoveBatchEntries(TArray<int32>& Batch, int32 Index, int32 Count, bool bKeepOrder)
{
    if (bKeepOrder)
    {
        Batch.RemoveAt(Index, Count, EAllowShrinking::No);
    }
    else
    {
        Batch.RemoveAtSwap(Index, Count, EAllowShrinking::No);
    }
}
#endif

#if UELAB_LEVEL == 0 || UELAB_LEVEL == 3
FArrayGrowthResult FillBatchWithGrowthTrace(int32 Count, bool bReserve)
{
    FArrayGrowthResult Result;
    Result.CapacityAfterEachAdd.Reserve(Count);
    if (bReserve)
    {
        Result.Values.Reserve(Count);
    }
    Result.NumBeforeFill = Result.Values.Num();
    Result.CapacityBeforeFill = Result.Values.Max();
    for (int32 Value = 0; Value < Count; ++Value)
    {
        Result.Values.Emplace(Value);
        Result.CapacityAfterEachAdd.Add(Result.Values.Max());
    }
    return Result;
}
#endif
