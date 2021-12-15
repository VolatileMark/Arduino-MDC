#include <Arduino.h>
#include <math.h>
#include "log.h"
#include "led.h"
#include "imu.h"
#include "utils.h"

#define STANDING_THS 0.10f
#define ROT_REC_THS 10.0f
#define PROT_THS 2000.0f
#define PROT_EXPECTED_VALUE_OFF 500.0f
#define NROT_THS 1500.0f
#define NROT_EXPECTED_VALUE_OFF 500.0f

#define STATE_WEIGHT(state) ((stillState == state) * 0.25f)
#define GET_ROT_WEIGHT(rot, ths, off) ((rot >= ths) * ((rot - ths) / off))
#define AVG(x, y) ((x + y) / 2.0f)

MOVEMENT_STATE movState;
STILL_STATE stillState;

bool executionPaused;

static float x, y, z;
static float rx, ry, rz;
static float px, py, pz;
static float pRotZ = 0, nRotZ = 0;

extern float cx, cy, cz;
extern float crx, cry, crz;

static STILL_STATE getStillState(void)
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

    standing = (2.0f - sqrtf(approxX * approxX + approxY * approxY + approxZ * approxZ)) + (sqrtf(adjX * adjX + adjY * adjY + adjZ * adjZ) < STANDING_THS) / 2.0f + STATE_WEIGHT(STILL_STANDING);
    //sitting = (approxX == 1.0f) + ((approxY == -1.0f) + (adjX > adjY)) / 2.0f + (rz < -SITTING_ROT_THS) + STATE_WEIGHT(STILL_SITTING);
    //sitting = commonWeight + ((pRotZ > PROT_THS) * ((pRotZ - PROT_THS) / PROT_EXPECTED_VALUE_OFF)) + STATE_WEIGHT(STILL_SITTING);
    sitting = commonWeight + GET_ROT_WEIGHT(pRotZ, PROT_THS, PROT_EXPECTED_VALUE_OFF) + STATE_WEIGHT(STILL_SITTING);
    //laying = (approxX == 1.0f) + ((approxY == -1.0f) + (adjX > adjY)) / 2.0f + (rz > SITTING_ROT_THS && stillState == STILL_SITTING) + STATE_WEIGHT(STILL_LAYING);
    //laying = commonWeight + ((nRotZ > NROT_THS) * ((nRotZ - NROT_THS) / NROT_EXPECTED_VALUE_OFF)) + STATE_WEIGHT(STILL_LAYING);
    laying = commonWeight + GET_ROT_WEIGHT(nRotZ, NROT_THS, NROT_EXPECTED_VALUE_OFF) + STATE_WEIGHT(STILL_LAYING) - (standing > sitting);

    INFO("standing: %f, sitting: %f, laying: %f, nrot: %f, prot: %f", standing, sitting, laying, nRotZ, pRotZ);
    LOG_AXIS(rx, ry, rz);
    //LOG_AXIS(x, y, z);

    return (standing > sitting && standing > laying) ? STILL_STANDING : (sitting >= laying) ? STILL_SITTING : STILL_LAYING;
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

    switch (movState)
    {
    case MOV_WALKING:
        setLedColor(LED_COLOR_BLUE);
        break;
    case MOV_STILL:
        stillState = getStillState();
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

    px = x; py = y; pz = z;
    
    if (!digitalRead(TERMINATE_PIN))
    {
        IMU.end();
        setLedColor(LED_COLOR_GREEN);
        INFO("Execution terminated");
        exit(0);
    }

    if (!digitalRead(RECALIBRATE_PIN))
    {
        INFO("Recalibrating IMU...");
        delay(500);
        calibrateIMU();
    }

    HANG(executionPaused);

    delay(10);
}