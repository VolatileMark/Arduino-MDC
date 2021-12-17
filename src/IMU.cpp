#include "IMU.h"
#include "Led.h"
#include "Log.h"
#include "MacroUtils.h"
#include "Defs.h"

float cx, cy, cz;
float crx, cry, crz;

void calibrateIMU(void)
{
    info("Starting IMU software calibration...");

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

    info("IMU software calibration complete");

    info("Accelerometer calibration:");
    logaxis(cx, cy, cz);
    info("Gyroscope calibration:");
    logaxis(crx, cry, crz);
}

void initIMU(void)
{
    if (!IMU.begin())
    {
        err("Could not initialize IMU");
        setRgbLedColor(LED_COLOR_RED);
        exit(1);
    }
    IMU.setContinuousMode();
    info("IMU initialized");
}

void pollIMU(float& ax, float& ay, float& az, float& rx, float& ry, float& rz)
{
    hang(!IMU.accelerationAvailable());
    IMU.readAcceleration(ax, ay, az);
    ax -= cx; ay -= cy; az -= cz;
    ax *= G_IN_MS2; ay *= G_IN_MS2; az *= G_IN_MS2;
    
    hang(!IMU.gyroscopeAvailable());
    IMU.readGyroscope(rx, ry, rz);
    rx -= crx; ry -= cry; rz -= crz;
}