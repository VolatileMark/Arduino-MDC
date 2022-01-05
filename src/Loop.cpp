#include "Common.h"

void loop(void)
{
    runMDC();
    
    switch (getCurrentMDCState())
    {
    case MDC_STANDING:
        setRgbLedColor(LED_COLOR_MAGENTA);
        break;
    case MDC_SITTING:
        setRgbLedColor(LED_COLOR_BLUE);
        break;
    case MDC_LAYING_B:
        setRgbLedColor(LED_COLOR_CYAN);
        break;
    case MDC_LAYING_F:
        setRgbLedColor(LED_COLOR_GREEN);
        break;
    case MDC_LAYING_L:
        setRgbLedColor(LED_COLOR_WHITE);
        break;
    case MDC_LAYING_R:
        setRgbLedColor(LED_COLOR_YELLOW);
        break;
    case MDC_WALKING:
        setRgbLedColor(LED_COLOR_RED);
        break;
    }

    if (!digitalRead(BTN_CALIBRATE_PIN))
    {
        info("Recalibrating IMU...");
        delay(500uL);
        calibrateIMU();
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