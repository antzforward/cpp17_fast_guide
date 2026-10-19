#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "UELabCommandlet.generated.h"

UCLASS()
class UUELabCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UUELabCommandlet();
    virtual int32 Main(const FString& Params) override;
};
