#include "LabContext.h"

// Copy to examples/my_experiment.cpp, then: ue.bat run examples/my_experiment.cpp
int32 RunExperiment(FLabContext& Lab)
{
    // 1. Read and validate parameters with Lab.Int / Lab.Number / Lab.Require.
    // 2. Generate input and invoke real UE APIs. Make ownership and order explicit.
    // 3. Compare output against an independent expectation with Lab.Require.
    // 4. Save measurable facts through Lab.Metric; explain boundaries through Lab.Note.
    Lab.Note(TEXT("Write your experiment here."));
    return Lab.Failures == 0 ? 0 : 1;
}
