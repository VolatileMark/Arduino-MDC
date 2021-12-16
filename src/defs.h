#pragma once

#include <Arduino.h>
#include <math.h>

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

#define TERMINATE_PIN P1_11
#define RECALIBRATE_PIN P1_12
#define PAUSE_EXEC_PIN P1_13


#define STANDING_THS 0.10f
#define ROT_REC_THS 10.0f
#define PROT_THS 2000.0f
#define PROT_EXPECTED_VALUE_OFF 500.0f
#define NROT_THS 1500.0f
#define NROT_EXPECTED_VALUE_OFF 500.0f