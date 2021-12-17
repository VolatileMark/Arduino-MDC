#pragma once

#include <Arduino_LSM9DS1.h>

extern float cx, cy, cz;
extern float crx, cry, crz;

void initIMU(void);
void calibrateIMU(void);
void pollIMU(float& ax, float& ay, float& az, float& rx, float& ry, float& rz);