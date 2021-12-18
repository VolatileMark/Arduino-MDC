#include "main.h"

void initSerial(uint32_t speed)
{
    Serial.begin(speed);
#ifdef __DEBUG__
    hang(!Serial);
#endif
    info("Serial initialized with speed %u", speed);
}

void initPins(void)
{
    pinMode(BTN_CALIBRATE_PIN, INPUT_PULLUP);
    pinMode(BTN_TERMINATE_PIN, INPUT_PULLUP);
    pinMode(BTN_PAUSEEXEC_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BTN_TERMINATE_PIN), terminateBtnCallback, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_PAUSEEXEC_PIN), pauseExecBtnCallback, FALLING);
}

void setup(void)
{
    initSerial(9600uL);
    initPins();
    initLeds();
    initIMU();
    initRecorder();


    info("Waiting for permission to calibrate");
    hang(digitalRead(BTN_CALIBRATE_PIN));
    delay(500uL);
    calibrateIMU();
}