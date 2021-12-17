#include "IMU.h"
#include "Led.h"
#include "Log.h"

float crx, cry, crz;
float cx, cy, cz;

void calibrateIMU(void)
{
    INFO("Starting IMU software calibration...");

    uint8_t times;
    float px, py, pz;
    float prx, pry, prz;

    cx = 0;
    cy = 0;
    cz = 0;
    crx = 0;
    cry = 0;
    crz = 0;

    for (times = 0; times < (uint8_t)-2; times++)
    {
        if (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable())
        {
            --times;
            continue;
        }

        IMU.readAcceleration(px, py, pz);
        cx = (cx + px) / 2.0f;
        cy = (cy + py) / 2.0f;
        cz = (cz + pz) / 2.0f;

        IMU.readGyroscope(prx, pry, prz);
        crx = (crx + prx) / 2.0f;
        cry = (cry + pry) / 2.0f;
        crz = (crz + prz) / 2.0f;

        delay(10uL);
    }

    INFO("IMU software calibration complete");

    INFO("Accelerometer calibration:");
    LOG_AXIS(cx, cy, cz);
    INFO("Gyroscope calibration:");
    LOG_AXIS(crx, cry, crz);
}

void initIMU(void)
{
    if (!IMU.begin())
    {
        ERR("Could not initialize IMU");
        setRgbLedColor(LED_COLOR_RED);
        exit(1);
    }
    IMU.setContinuousMode();
    INFO("IMU initialized");
}