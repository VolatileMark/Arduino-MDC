#include "ISR.h"

static bool terminationRequested = false, executionPaused = false;

void terminateBtnCallback(void)
{
    terminationRequested = true;
    executionPaused = false;
}

void pauseExecBtnCallback(void)
{
    executionPaused = !executionPaused;
}

bool isExecutionPaused(void)
{
    return executionPaused;
}

bool shouldTerminate(void)
{
    return terminationRequested;
}