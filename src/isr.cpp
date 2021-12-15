extern bool executionPaused;

void pauseExecutionBtnCallback(void)
{
    executionPaused = !executionPaused;
}