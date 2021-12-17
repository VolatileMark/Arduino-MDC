#include "main.h"

void loop(void)
{
    if (isExecutionPaused())
    {
        turnRgbLedOff();
        turnBuiltinLedOn();
        INFO("Paused by user");
        HANG(isExecutionPaused());
        turnBuiltinLedOff();
        INFO("Resuming...");
    }
    if (shouldTerminate())
    {
        turnRgbLedOff();
        turnPowerLedOn();
        INFO("Termination requested by user");
        Serial.flush();
        Serial.end();
        exit(0);
    }
}