#pragma once

#include <Arduino.h>

typedef enum
{
  MOV_STILL = 0,
  MOV_WALKING = 1
} MOVEMENT_STATE;

typedef enum
{
  STILL_STANDING = 0,
  STILL_SITTING = 1,
  STILL_LAYING = 2
} STILL_STATE;

#define HANG(x) while (x) delay(10)

#define TERMINATE_PIN P1_11
#define RECALIBRATE_PIN P1_12
#define PAUSE_EXEC_PIN P1_13