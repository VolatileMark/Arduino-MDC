#include "main.h"
#include "MDC.h"

void loop(void)
{
    runMDC();
    
    switch (getCurrentMDCState())
    {
    case MDC_STANDING:
        setRgbLedColor(LED_COLOR_BLUE);
        break;
    case MDC_SITTING:
        setRgbLedColor(LED_COLOR_MAGENTA);
        break;
    case MDC_LAYING:
        setRgbLedColor(LED_COLOR_RED);
        break;
    case MDC_WALKING:
        setRgbLedColor(LED_COLOR_CYAN);
        break;
    }

    if (isExecutionPaused())
    {
        turnRgbLedOff();
        turnBuiltinLedOn();
        info("Paused by user");
        hang(isExecutionPaused());
        turnBuiltinLedOff();
        info("Resuming...");
    }
    if (shouldTerminate())
    {
        turnRgbLedOff();
        turnPowerLedOn();
        info("Termination requested by user");
        Serial.flush();
        Serial.end();
        exit(0);
    }
}