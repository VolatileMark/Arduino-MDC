#include <Arduino.h>
#include <math.h>
#include "log.h"
#include "led.h"
#include "imu.h"
#include "defs.h"

MOVEMENT_STATE movState;
STILL_STATE stillState;

bool executionPaused, shouldQuit;
float pRotZ, nRotZ;

static float x, y, z;
static float rx, ry, rz;

extern float cx, cy, cz;
extern float crx, cry, crz;

static void updateMovementState(void)
{
}

static void updateStillState(void)
{
    float approxX, approxY, approxZ;
    float adjX, adjY, adjZ;
    float standing, sitting, laying;
    int commonWeight;

    approxX = roundf(x); approxY = roundf(y); approxZ = roundf(z);
    adjX = AVG(x, approxX); adjY = AVG(y, approxY); adjZ = AVG(z, approxZ);

    if (rz < -ROT_REC_THS) { nRotZ -= rz; pRotZ = 0; }
    else if (rz > ROT_REC_THS) { pRotZ += rz; nRotZ = 0; }

    commonWeight = (approxX == 1.0f) + ((approxY == -1.0f) + (adjX > adjY)) / 2.0f;

    standing = (2.0f - sqrtf(approxX * approxX + approxY * approxY + approxZ * approxZ)) + (sqrtf(adjX * adjX + adjY * adjY + adjZ * adjZ) < STANDING_THS) / 2.0f + STATE_WEIGHT(stillState, STILL_STANDING);
    sitting = commonWeight + GET_ROT_WEIGHT(pRotZ, PROT_THS, PROT_EXPECTED_VALUE_OFF) + STATE_WEIGHT(stillState, STILL_SITTING);
    laying = commonWeight + GET_ROT_WEIGHT(nRotZ, NROT_THS, NROT_EXPECTED_VALUE_OFF)  - ((standing > sitting) / 2.0f) + STATE_WEIGHT(stillState, STILL_LAYING);

    //INFO("standing: %f, sitting: %f, laying: %f, nrot: %f, prot: %f", standing, sitting, laying, nRotZ, pRotZ);
    
    stillState = (standing > sitting && standing > laying) ? STILL_STANDING : (sitting >= laying) ? STILL_SITTING : STILL_LAYING;
}

void loop(void)
{
    if (IMU.accelerationAvailable())
    {
        // X = Forward/Backward
        // Y = Right/Left
        // Z = Up/Down
        IMU.readAcceleration(x, y, z);
        x -= cx; y -= cy; z -= cz;
        //INFO("Accelerometer data:");
        //LOG_AXIS(x, y, z);
    }

    if (IMU.gyroscopeAvailable())
    {
        // RX = Roll
        // RY = Pitch
        // RZ = Yaw
        IMU.readGyroscope(rx, ry, rz);
        rx -= crx; ry -= cry; rz -= crz;
        //INFO("Gyroscope data:");
        //LOG_AXIS(rx, ry, rz);
    }

    updateMovementState();
    switch (movState)
    {
    case MOV_WALKING:
        setLedColor(LED_COLOR_BLUE);
        break;
    case MOV_STILL:
        updateStillState();
        switch (stillState)
        {
        case STILL_STANDING:
            setLedColor(LED_COLOR_WHITE);
            break;
        case STILL_SITTING:
            setLedColor(LED_COLOR_MAGENTA);
            break;
        case STILL_LAYING:
            setLedColor(LED_COLOR_RED);
            break;
        }
        break;
    }

    if (!digitalRead(RECALIBRATE_PIN))
    {
        INFO("Recalibrating IMU...");
        delay(500uL);
        calibrateIMU();
    }
    else if (executionPaused)
    {
        INFO("Execution paused");
        HANG(executionPaused);
    }
    if (shouldQuit)
    {
        IMU.end();
        setLedColor(LED_COLOR_GREEN);
        INFO("Execution terminated");
        exit(0);
    }

    delay(10uL);
}