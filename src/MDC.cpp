#include "MDC.h"
#include "IMU.h"
#include "MacroUtils.h"
#include "DoubleBuffer.h"
#include <math.h>
#include <arduinoFFT.h>

static double yReal[SAMPLES], yImag[SAMPLES];
static DoubleBuffer yDataBuffer = DoubleBuffer(SAMPLES);
static arduinoFFT FFT = arduinoFFT(yReal, yImag, (uint16_t) SAMPLES, SAMPLING_RATE);

static MDC_STATE currentState;
static double accx, accy, accz;
static double rotx, roty, rotz;
static double prevT;
static uint16_t recordedSamples;

static void checkMovement(void)
{
    memcpy(yReal, yDataBuffer.getBuffer(), sizeof(double) * SAMPLES);
    FFT.Windowing(FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    Serial.println("--- NEW PKT ---");
    for (int i = 0; i < SAMPLES; i++) { Serial.print(yReal[i]); Serial.print(", "); Serial.print(yDataBuffer.getBuffer()[i]); Serial.println(); }
}

static void parseData(void)
{
    double approxAccX, approxAccY, approxAccZ;
    approxAccX = round(accx / G_IN_MS2); approxAccY = round(accy / G_IN_MS2); approxAccZ = round(accz / G_IN_MS2);
    if (approxAccX == 0.0 && approxAccY == 1.0 && abs(approxAccZ) == 1.0) { currentState = (approxAccZ > 0.0) ? MDC_LAYING_B : MDC_LAYING_F; }
    else if (abs(approxAccX) == 1.0 && approxAccY == 1.0 && approxAccZ == 0.0) { currentState = (approxAccX > 0.0) ? MDC_LAYING_R : MDC_LAYING_L; }
    else { checkMovement(); }
}

void initRecorder(void)
{
    accx = 0.0; accy = 0.0; accz = 0.0;
    rotx = 0.0; roty = 0.0; rotz = 0.0;
    currentState = MDC_SITTING;
    recordedSamples = 0;
    memset(yReal, 0, sizeof(double) * SAMPLES);
    memset(yImag, 0, sizeof(double) * SAMPLES);
}

void runMDC(void)
{
    pollIMU(accx, accy, accz, rotx, roty, rotz);

    if (seconds() - prevT < (1.0 / SAMPLING_RATE))
        return;

    yDataBuffer.push(accy);
    parseData();

    prevT = seconds();
}

MDC_STATE getCurrentMDCState(void)
{
    return currentState;
}