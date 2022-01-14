#include "Common.h"

void initSerial(uint32_t speed)
{
    if (!digitalRead(DEBUG_PIN))
    {
        printOnLCD("Waiting for\nserial...");
        turnPowerLedOn();
        Serial.begin(speed);
        hang(!Serial);
        delay(250uL);
        turnPowerLedOff();
        info("Serial initialized with baud rate %u", speed);
        warn("Debug mode is enabled!");
    }
}

void initPins(void)
{
    pinMode(BTN_CALIBRATE_PIN, INPUT_PULLUP);
    pinMode(BTN_TERMINATE_PIN, INPUT_PULLUP);
    pinMode(BTN_PAUSEEXEC_PIN, INPUT_PULLUP);
    pinMode(DEBUG_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BTN_TERMINATE_PIN), terminateBtnCallback, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_PAUSEEXEC_PIN), pauseExecBtnCallback, FALLING);
}

void setup(void)
{
    initLCD();
    initRecorder();
    initPins();
    initLeds();
    initSerial(115200uL);
    initIMU();

    setRgbLedColor(LED_COLOR_BLUE);
    printOnLCD("Waiting for\ncalibration...");
    info("Waiting for permission to calibrate");
    hang(digitalRead(BTN_CALIBRATE_PIN));
    delay(500uL);
    printOnLCD("Calibrating\nIMU...");
    calibrateIMU();
}