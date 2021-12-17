#pragma once

#include <Arduino.h>

#define BTN_CALIBRATE_PIN P1_11
#define BTN_TERMINATE_PIN P1_12
#define BTN_PAUSEEXEC_PIN P1_15

#define G_IN_MS2 9.8066f

#define SAMPLE_RATE 64.0f
#define MAX_SAMPLES 128
#define MIN_SAMPLES 32
#define ACC_REC_THS (0.17f * G_IN_MS2)
#define GYR_REC_THS 17.0f
#define GRACE_TIMER_DELAY ((MIN_SAMPLES / SAMPLE_RATE) * 2.0f)
#define COOLDOWN_TIMER_DELAY 500uL

typedef float SamplesBuffer[MAX_SAMPLES];

typedef enum
{
    MDC_WALKING,
    MDC_STANDING,
    MDC_SITTING,
    MDC_LAYING
} MDC_STATE;