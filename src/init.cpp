#include <Arduino.h>
#include "log.h"
#include "led.h"
#include "imu.h"
#include "defs.h"
#include "macros.h"

extern MOVEMENT_STATE movState;
extern STILL_STATE stillState;
extern bool executionPaused, shouldQuit;
extern float pRotZ;
extern uint32_t nRotZ;

extern void pauseExecutionBtnCallback();
extern void terminateExecutionBtnCallback();

static void initSerial(uint32_t baudRate)
{
    Serial.begin(baudRate);
#ifdef __DEBUG__
    HANG(!Serial);
#endif
    delay(250uL);
    INFO("Serial initialized");
}

static void initPins()
{
    pinMode(TERMINATE_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(TERMINATE_PIN), terminateExecutionBtnCallback, FALLING);
    pinMode(RECALIBRATE_PIN, INPUT_PULLUP);
    pinMode(PAUSE_EXEC_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PAUSE_EXEC_PIN), pauseExecutionBtnCallback, FALLING);
    INFO("Pins initialized");
}

void setup(void)
{
    movState = MOV_STILL; stillState = STILL_STANDING;
    executionPaused = false; shouldQuit = false;
    pRotZ = 1.0f; nRotZ = 0;

    initSerial(9600uL);
    initPins();
    initLed();
    initIMU();
    INFO("Waiting calibration go ahead...");
    HANG(digitalRead(RECALIBRATE_PIN));
    delay(500uL);
    calibrateIMU();

    delay(1000uL);
}