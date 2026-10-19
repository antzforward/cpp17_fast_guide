#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LabObject.generated.h"

// A concrete reflected type. UE 5.6's UObject base is abstract.
UCLASS()
class ULabObject : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    int32 SampleValue = 42;
};
