#include "LabContext.h"
#include "UObject/NameTypes.h"

int32 RunExperiment(FLabContext& Lab)
{
    TArray<int32> Values = {10, 20, 30};
    const FName Upper(TEXT("LabActor"));
    const FName Lower(TEXT("labactor"));
    Lab.Require(Values.Num() == 3 && Values[1] == 20, TEXT("real TArray works"));
    Lab.Require(Upper == Lower, TEXT("real FName comparison is case insensitive"));
    Lab.Metric(TEXT("fname_bytes"), sizeof(FName));
    Lab.Metric(TEXT("array_capacity"), Values.Max());
    Lab.Note(TEXT("Sizes and capacity describe this UE build, not every UE configuration."));
    return 0;
}
