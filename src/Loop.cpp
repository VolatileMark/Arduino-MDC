#include "Common.h"

MDC_STATE currentState;

void loop(void)
{
    runMDC();

    updateState(getCurrentMDCState());

    if (!digitalRead(BTN_CALIBRATE_PIN))
    {
        info("Recalibrating IMU...");
        printOnLCD("Recalibrating\nIMU...");
        delay(500uL);
        calibrateIMU();
    }
    if (isExecutionPaused())
    {
        turnRgbLedOff();
        turnBuiltinLedOn();
        info("Paused by user");
        printOnLCD("Paused by user");
        hang(isExecutionPaused());
        turnBuiltinLedOff();
        info("Resuming...");
        printState(getCurrentMDCState());
    }
    if (shouldTerminate())
    {
        turnRgbLedOff();
        turnPowerLedOn();
        info("Termination requested by user");
        Serial.flush();
        Serial.end();
        printOnLCD("Terminated\nby user");
        exit(0);
    }
}