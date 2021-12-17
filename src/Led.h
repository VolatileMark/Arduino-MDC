#pragma once

#include <Arduino.h>

typedef enum
{
    LED_COLOR_BLACK = 0b000,
    LED_COLOR_BLUE = 0b001,
    LED_COLOR_GREEN = 0b010,
    LED_COLOR_CYAN = 0b011,
    LED_COLOR_RED = 0b100,
    LED_COLOR_MAGENTA = 0b101,
    LED_COLOR_YELLOW = 0b110,
    LED_COLOR_WHITE = 0b111
} LED_COLOR;


#define turnRgbLedOff() setRgbLedColor(LED_COLOR_BLACK)

void initLeds(void);
void setRgbLedColor(LED_COLOR color);
void turnPowerLedOn(void);
void turnBuiltinLedOn(void);
void turnPowerLedOff(void);
void turnBuiltinLedOff(void);