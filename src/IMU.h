#pragma once

#include <Arduino_LSM9DS1.h>

extern double cx, cy, cz;
extern double crx, cry, crz;

void initIMU(void);
void calibrateIMU(void);
void pollIMU(double& ax, double& ay, double& az, double& rx, double& ry, double& rz);