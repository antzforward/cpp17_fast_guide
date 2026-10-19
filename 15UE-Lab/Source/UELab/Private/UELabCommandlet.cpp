#include "UELabCommandlet.h"
#include "LabContext.h"
#include "Misc/EngineVersion.h"
#include "HAL/PlatformTime.h"

DEFINE_LOG_CATEGORY(LogUELab);

UUELabCommandlet::UUELabCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
    ShowErrorCount = true;
    HelpDescription = TEXT("Execute one real UE C++ experiment.");
}

int32 UUELabCommandlet::Main(const FString& Params)
{
    FLabContext Lab;
    Lab.Params = Params;
    Lab.Level = Lab.Int(TEXT("Level"), 0);
    Lab.Note(FString::Printf(TEXT("Engine=%s GameThread=%d"),
        *FEngineVersion::Current().ToString(), IsInGameThread() ? 1 : 0));
    const double Started = FPlatformTime::Seconds();
    const int32 Code = RunExperiment(Lab);
    Lab.Metric(TEXT("experiment_ms"), (FPlatformTime::Seconds() - Started) * 1000.0);
    const bool Passed = Code == 0 && Lab.Failures == 0;
    UE_LOG(LogUELab, Display, TEXT("UELAB_RESULT status=%s failures=%d"),
        Passed ? TEXT("PASS") : TEXT("FAIL"), Lab.Failures);
    return Passed ? 0 : 1;
}
