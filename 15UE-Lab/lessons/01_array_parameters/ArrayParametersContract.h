#pragma once

#include "CoreMinimal.h"

// 公共观测契约；答题和验收共用。这里没有题目实现。
struct FArrayBuildResult
{
    TArray<int32> Values;
    int32 NumImmediatelyAfterReserve = INDEX_NONE;
    int32 CapacityImmediatelyAfterReserve = 0;
};

struct FArrayGrowthResult
{
    TArray<int32> Values;
    TArray<int32> CapacityAfterEachAdd;
    int32 NumBeforeFill = INDEX_NONE;
    int32 CapacityBeforeFill = 0;
};
