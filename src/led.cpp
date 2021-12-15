#include "led.h"
#include "log.h"

void initLed(void)
{
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_PWR, OUTPUT);
    turnLedOff();
    INFO("Internal led initialized");
}

void setLedColor(LED_COLOR color)
{
    digitalWrite(LED_RED, (color & 0b100) ? LOW : HIGH);
    digitalWrite(LED_GREEN, (color & 0b010) ? LOW : HIGH);
    digitalWrite(LED_BLUE, (color & 0b001) ? LOW : HIGH);
}