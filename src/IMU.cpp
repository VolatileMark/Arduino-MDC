#include "IMU.h"
#include "Led.h"
#include "Log.h"
#include "MacroUtils.h"
#include "Defs.h"

double cx, cy, cz;
double crx, cry, crz;

void calibrateIMU(void)
{
    info("Starting IMU software calibration...");

    uint8_t times;
    float px, py, pz;
    float prx, pry, prz;

    cx = 0.0; cy = 0.0; cz = 0.0;
    crx = 0.0; cry = 0.0; crz = 0.0;

    for (times = 0; times < (uint8_t)-2; times++)
    {
        if (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable())
        {
            --times;
            continue;
        }

        IMU.readAcceleration(px, py, pz);
        cx = (cx + px) / 2.0;
        cy = (cy + py) / 2.0;
        cz = (cz + pz) / 2.0;

        IMU.readGyroscope(prx, pry, prz);
        crx = (crx + prx) / 2.0;
        cry = (cry + pry) / 2.0;
        crz = (crz + prz) / 2.0;

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

void pollIMU(double& ax, double& ay, double& az, double& rx, double& ry, double& rz)
{
    float nax, nay, naz;
    float nrx, nry, nrz;

    hang(!IMU.accelerationAvailable());
    IMU.readAcceleration(nax, nay, naz);
    ax = nax; ay = nay; az = naz;
    ax -= cx; ay -= cy; az -= cz;
    ax *= G_IN_MS2; ay *= G_IN_MS2; az *= G_IN_MS2;

    hang(!IMU.gyroscopeAvailable());
    IMU.readGyroscope(nrx, nry, nrz);
    rx = nrx; ry = nry; rz = nrz;
    rx -= crx; ry -= cry; rz -= crz;
}