#include "MDC.h"
#include "IMU.h"
#include "MacroUtils.h"
#include "DoubleBuffer.h"
#include <arduinoFFT.h>

static double ryReal[SAMPLES], ryImag[SAMPLES], yAvg[IMPULSE_SAMPLES];
static DoubleBuffer ryDataBuffer = DoubleBuffer(SAMPLES);
static DoubleBuffer yDataBuffer = DoubleBuffer(IMPULSE_SAMPLES);
static arduinoFFT FFT = arduinoFFT(ryReal, ryImag, (uint16_t) SAMPLES, SAMPLING_RATE);
static MDC_STATE currentState;
static double accx, accy, accz;
static double rotx, roty, rotz;
static char walking, standing, sitting, layingF, layingB, layingR, layingL;
static uint32_t sampleT20, sampleT200, graceT;

static void averageArray(double* src, double* output, int arrSize, int perSideWindowSize)
{
    int start, end;
    double sum;
    for (int i = 0; i < arrSize; i++)
    {
        sum = 0.0;
        start = max(i - perSideWindowSize, 0); end = min(i + perSideWindowSize, arrSize - 1);
        for (uint16_t j = start; j <= end; j++) { sum += src[j]; }
        output[i] = sum / ((double)((end - start) + 1));
    }
}

static void checkIfWalking(void)
{
    if (millis() - sampleT20 < (1000 / SAMPLING_RATE)) { return; }

    ryDataBuffer.push(roty);
    sampleT20 = millis();

    /* Walking detection */
    double maxPeak, approxPeak;
    uint16_t peakIndex;
    memcpy(ryReal, ryDataBuffer.getBuffer(), sizeof(double) * SAMPLES);
    memset(ryImag, 0, sizeof(double) * SAMPLES);
    FFT.Windowing(FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
    maxPeak = FFT.MajorPeak();
    approxPeak = round(maxPeak * 10.0) / 10.0;
    peakIndex = (uint16_t) round((maxPeak * SAMPLES) / SAMPLING_RATE);
    srprintf("Major peak: %fHz %d -- Peak value: %f\n", approxPeak, peakIndex, ryReal[peakIndex]);
    walking = (approxPeak >= 0.55 && approxPeak <= 2.25 && ryReal[peakIndex] > 150.0) + (currentState == MDC_WALKING || currentState == MDC_STANDING);
}

static void checkIfLaying(void)
{
    double approxAccX, approxAccY, approxAccZ;
    approxAccX = round(accx / G_IN_MS2); approxAccY = round(accy / G_IN_MS2); approxAccZ = round(accz / G_IN_MS2);
    //if (approxAccX == 0.0 && approxAccY == 1.0 && abs(approxAccZ) == 1.0) { currentState = (approxAccZ > 0.0) ? MDC_LAYING_B : MDC_LAYING_F; }
    //else if (abs(approxAccX) == 1.0 && approxAccY == 1.0 && approxAccZ == 0.0) { currentState = (approxAccX > 0.0) ? MDC_LAYING_R : MDC_LAYING_L; }
    layingF = (approxAccX == 0.0 && approxAccY == 1.0) + (approxAccZ == -1.0);
    layingB = (approxAccX == 0.0 && approxAccY == 1.0) + (approxAccZ == +1.0);
    layingL = (approxAccZ == 0.0 && approxAccY == 1.0) + (approxAccX == -1.0);
    layingR = (approxAccZ == 0.0 && approxAccY == 1.0) + (approxAccX == +1.0);
}

static void parseImpulse(void)
{
    double yMin, yMax;
    uint16_t yMinIdx, yMaxIdx, i;
    averageArray(yDataBuffer.getBuffer(), yAvg, IMPULSE_SAMPLES, 4);
    for 
    (
        i = 0, yMaxIdx = 0, yMinIdx = 0, yMin = yAvg[0], yMax = yAvg[0];
        i < IMPULSE_SAMPLES;
        i++
    )
    {
        if (yAvg[i] > yMax) { yMax = yAvg[i]; yMaxIdx = i; }
        if (yAvg[i] < yMin) { yMin = yAvg[i]; yMinIdx = i; }
    }
    yDataBuffer.clear();

    if (round(yMax - yMin) < MIN_VALID_THS) { return; }
    //if (yMinIdx < yMaxIdx) { srprintf("Stood up: %f\n", yMax - yMin); }
    //else if (yMaxIdx < yMinIdx) { srprintf("Stood down: %f\n", yMax - yMin); }
    standing = (yMinIdx < yMaxIdx) + (currentState != MDC_STANDING && currentState != MDC_WALKING);
    sitting = (yMaxIdx < yMinIdx) + (currentState != MDC_SITTING && currentState != MDC_WALKING) - (currentState == MDC_WALKING);
}

static void checkIfStandingOrSitting(void)
{
    uint32_t nowT;

    if (sqrt(sq(accx) + sq(accy) + sq(accz)) < ACC_DET_THS)
    {
        if (graceT > 0 && millis() - graceT > GRACE_TIME) { parseImpulse(); graceT = 0; }
        return;
    }
    if (graceT == 0) { graceT = millis(); }

    nowT = millis();
    if (nowT - sampleT200 >= (1000 / IMPULSE_SAMPLING_RATE))
    {
        yDataBuffer.put(accy);
        sampleT200 = nowT;
    }
}

static void setState()
{
    currentState = (walking > standing && walking > sitting && walking > layingF && walking > layingB && walking > layingR && walking > layingL) ? MDC_WALKING : \
                   (standing > sitting && standing > layingF && standing > layingB && standing > layingR && standing > layingL) ? MDC_STANDING : \
                   (sitting > layingF && sitting > layingB && sitting > layingR && sitting > layingL) ? MDC_SITTING : \
                   (layingF > layingB && layingF > layingR && layingF > layingL) ? MDC_LAYING_F : \
                   (layingB > layingR && layingB > layingF) ? MDC_LAYING_B : \
                   (layingR > layingL) ? MDC_LAYING_R : MDC_LAYING_L;
}

void initRecorder(void)
{
    accx = 0.0; accy = 0.0; accz = 0.0;
    rotx = 0.0; roty = 0.0; rotz = 0.0;
    currentState = MDC_SITTING;
    sampleT20 = 0; sampleT200 = 0; graceT = 0;
    walking = 0.0; standing = 0.0; sitting = 1.0; layingF = 0.0; layingB = 0.0; layingR = 0.0; layingL = 0.0;
}

void runMDC(void)
{
    pollIMU(accx, accy, accz, rotx, roty, rotz);

    srprintf("%f %f %f\n", rotx, roty, rotz);
    delay(50);

    //checkIfLaying();
    //checkIfWalking();
    //checkIfStandingOrSitting();
    //setState();

    //srprintf("Walking: %d / Standing: %d / Sitting: %d / LayingF: %d / LayingB: %d / LayingR: %d / LayingL: %d\n", walking, standing, sitting, layingF, layingB, layingR, layingL);
}

MDC_STATE getCurrentMDCState(void)
{
    return currentState;
}