#pragma once

#include <Arduino.h>

#define BTN_CALIBRATE_PIN P1_11
#define BTN_TERMINATE_PIN P1_12
#define BTN_PAUSEEXEC_PIN P1_15

#define G_IN_MS2 9.8066

#define SAMPLING_RATE 20
#define SAMPLES 64
#define IMPULSE_SAMPLING_RATE 200
#define IMPULSE_SAMPLES 200

#define ACC_DET_THS (0.175 * G_IN_MS2)
#define GRACE_TIME 1000

typedef enum
{
    MDC_WALKING,
    MDC_STANDING,
    MDC_SITTING,
    MDC_LAYING_B,
    MDC_LAYING_F,
    MDC_LAYING_L,
    MDC_LAYING_R
} MDC_STATE;