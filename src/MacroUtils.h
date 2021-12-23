#pragma once

#include <Arduino.h>
#include "Log.h"

#define hang(condition) while (condition) delay(10uL)
#define avg(x, y) ((x + y) / 2.0)
#define seconds() ((double) millis() / 1000.0)
#define max(x, y) ((x > y) ? x : y);
#define min(x, y) ((x < y) ? x : y);