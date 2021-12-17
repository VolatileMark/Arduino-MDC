#include "Led.h"
#include "Log.h"

void initLeds(void)
{
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_PWR, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    turnRgbLedOff();
    turnPowerLedOff();
    turnBuiltinLedOff();
    INFO("Internal led initialized");
}

void setRgbLedColor(LED_COLOR color)
{
    digitalWrite(LED_RED, (color & 0b100) ? LOW : HIGH);
    digitalWrite(LED_GREEN, (color & 0b010) ? LOW : HIGH);
    digitalWrite(LED_BLUE, (color & 0b001) ? LOW : HIGH);
}

void turnPowerLedOn(void)
{
    digitalWrite(LED_PWR, HIGH);
}

void turnBuiltinLedOn(void)
{
    digitalWrite(LED_BUILTIN, HIGH);
}

void turnPowerLedOff(void)
{
    digitalWrite(LED_PWR, LOW);
}

void turnBuiltinLedOff(void)
{
    digitalWrite(LED_BUILTIN, LOW);
}