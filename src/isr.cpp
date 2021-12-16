extern bool executionPaused;
extern bool shouldQuit;

void pauseExecutionBtnCallback(void)
{
    executionPaused = !executionPaused;
}

void terminateExecutionBtnCallback(void)
{
    shouldQuit = true;
    executionPaused = false;
}