#include <Arduino.h>
#include "log.h"
#include "led.h"
#include "imu.h"
#include "utils.h"

extern MOVEMENT_STATE movState;
extern STILL_STATE stillState;
extern bool executionPaused;

extern void pauseExecutionBtnCallback();

static void initSerial(uint64_t baudRate)
{
    Serial.begin(baudRate);
#ifdef __DEBUG__
    HANG(!Serial);
#endif
    delay(250);
    INFO("Serial initialized");
}

static void initPins()
{
    pinMode(TERMINATE_PIN, INPUT_PULLUP);
    pinMode(RECALIBRATE_PIN, INPUT_PULLUP);
    pinMode(PAUSE_EXEC_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PAUSE_EXEC_PIN), pauseExecutionBtnCallback, FALLING);
    INFO("Pins initialized");
}

void setup(void)
{
    movState = MOV_STILL;
    stillState = STILL_SITTING;
    executionPaused = false;

    initSerial(9600);
    initPins();
    initLed();
    initIMU();
    INFO("Waiting calibration go ahead...");
    HANG(digitalRead(RECALIBRATE_PIN));
    delay(500);
    calibrateIMU();

    delay(1000);
}