#pragma once

#include "CoreMinimal.h"
#include "Misc/Parse.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUELab, Log, All);

// The host owns initialization. Experiments own their data, checks and measurements.
struct FLabContext
{
    FString Params;
    int32 Level = 0;
    int32 Failures = 0;

    bool Require(bool Condition, const FString& Message)
    {
        if (Condition)
        {
            UE_LOG(LogUELab, Display, TEXT("UELAB_CHECK PASS %s"), *Message);
        }
        else
        {
            ++Failures;
            UE_LOG(LogUELab, Error, TEXT("UELAB_CHECK FAIL %s"), *Message);
        }
        return Condition;
    }

    void Note(const FString& Message)
    {
        UE_LOG(LogUELab, Display, TEXT("UELAB_NOTE %s"), *Message);
    }

    void Metric(const FString& Name, double Value)
    {
        UE_LOG(LogUELab, Display, TEXT("UELAB_METRIC %s=%.9f"), *Name, Value);
    }

    int32 Int(const TCHAR* Name, int32 Default) const
    {
        int32 Value = Default;
        const FString Key = TEXT("-") + FString(Name) + TEXT("=");
        FParse::Value(*Params, *Key, Value);
        return Value;
    }

    double Number(const TCHAR* Name, double Default) const
    {
        double Value = Default;
        const FString Key = TEXT("-") + FString(Name) + TEXT("=");
        FParse::Value(*Params, *Key, Value);
        return Value;
    }
};

int32 RunExperiment(FLabContext& Lab);
