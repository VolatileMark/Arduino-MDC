#include <Arduino.h>
#include <math.h>
#include "log.h"
#include "led.h"
#include "imu.h"
#include "defs.h"
#include "macros.h"

MOVEMENT_STATE movState;
STILL_STATE stillState;

bool executionPaused, shouldQuit;
float pRotZ, py = 0;
uint32_t nRotZ;

static float x, y, z;
static float rx, ry, rz;

extern float cx, cy, cz;
extern float crx, cry, crz;

static void updateMovementState(void)
{

}

static void updateStillState(void)
{
    float standing, sitting, laying;
    float approxX, approxY, approxZ;
    //float adjX, adjY, adjZ;
    //float absy;
    float commonWeight;
    STILL_STATE newState;

    approxX = roundf(x); approxY = roundf(y); approxZ = roundf(z);
    //adjX = AVG(x, approxX); adjY = AVG(y, approxY); adjZ = AVG(z, approxZ);
    //absy = abs(y);

    /*
    if (rz < -ROT_REC_THS) { nRotZ -= rz; pRotZ = 0; }
    else if (rz > ROT_REC_THS) { pRotZ += rz; nRotZ = 0; }

    commonWeight = (approxX == 1.0f) + ((approxY == -1.0f) + (adjX > adjY)) / 2.0f;

    standing = (2.0f - sqrtf(approxX * approxX + approxY * approxY + approxZ * approxZ)) + (sqrtf(adjX * adjX + adjY * adjY + adjZ * adjZ) < STANDING_THS) / 2.0f + STATE_WEIGHT(stillState, STILL_STANDING);
    sitting = commonWeight + GET_ROT_WEIGHT(pRotZ, PROT_THS, PROT_EXPECTED_VALUE_OFF) + STATE_WEIGHT(stillState, STILL_SITTING);
    laying = commonWeight + GET_ROT_WEIGHT(nRotZ, NROT_THS, NROT_EXPECTED_VALUE_OFF)  - ((standing > sitting) / 2.0f) + STATE_WEIGHT(stillState, STILL_LAYING);
    */

    if (abs(y) > Y_ACC_LO_THS)
    {
        if (y * pRotZ <= 0)
        {
            if (nRotZ > 0 && millis() - nRotZ > ACTION_MILLIS) { pRotZ = y; nRotZ = millis(); INFO("TRIGGERED"); }
            else if (py * y <= 0) { nRotZ = millis(); }
        }
        else if (abs(pRotZ) < abs(y)) { pRotZ = y; }
        py = y;
    }
    else if (y * py <= 0) { nRotZ = 0; }

    commonWeight = (roundf(sqrtf(approxX * approxX + approxY * approxY + approxZ * approxZ)) == 0);
    standing = commonWeight + (pRotZ > +Y_ACC_LO_THS || pRotZ < -Y_ACC_HI_THS) + STATE_WEIGHT(stillState, STILL_STANDING);
    sitting = commonWeight + (pRotZ < -Y_ACC_LO_THS || pRotZ > +Y_ACC_HI_THS) + STATE_WEIGHT(stillState, STILL_SITTING);
    laying = (abs(approxX) == 0.0f && abs(approxY) == 1.0f && abs(approxZ) == 1.0f) * 2.0f + STATE_WEIGHT(stillState, STILL_LAYING);

    INFO("standing: %f, sitting: %f, laying: %f, millis: %d, highest: %f, current: %f, delta: %d", standing, sitting, laying, nRotZ, pRotZ, y, millis() - nRotZ);
    
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
        Serial.flush();
        Serial.end();
        exit(0);
    }

    delay(10uL);
    //delay(100uL);
}