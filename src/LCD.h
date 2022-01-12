#pragma once

#include "Defs.h"

void initLCD(void);
void printOnLCD(const char* str, ...);
void updateState(MDC_STATE state);
void printState(MDC_STATE state);
void clearLCD(void);