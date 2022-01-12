#include "MDC.h"
#include "IMU.h"
#include "MacroUtils.h"
#include "DoubleBuffer.h"
#include "Defs.h"
#include <arduinoFFT.h>

static double ryReal[SAMPLES], ryImag[SAMPLES], yAvg[IMPULSE_SAMPLES];
static DoubleBuffer ryDataBuffer = DoubleBuffer(SAMPLES);
static DoubleBuffer yDataBuffer = DoubleBuffer(IMPULSE_SAMPLES);
static arduinoFFT FFT = arduinoFFT(ryReal, ryImag, (uint16_t) SAMPLES, SAMPLING_RATE);
static MDC_STATE currentState;
static double accx, accy, accz;
static double rotx, roty, rotz;
static bool walking, standing, sitting, layingF, layingB, layingR, layingL;
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
    walking = (approxPeak >= MIN_WALK_FRQ && approxPeak <= MAX_WALK_FRQ && ryReal[peakIndex] >= MIN_WALK_AMP && ryReal[peakIndex] <= MAX_WALK_AMP && (currentState == MDC_WALKING || currentState == MDC_STANDING));
    //srprintf("Major peak: %fHz -- Peak index: %d -- Peak value: %f -- Walking: %d\n", approxPeak, peakIndex, ryReal[peakIndex], walking);
}

static void checkIfLaying(void)
{
    bool commonX, commonZ;
    double approxAccX, approxAccY, approxAccZ;
    approxAccX = round(accx / G_IN_MS2); approxAccY = round(accy / G_IN_MS2); approxAccZ = round(accz / G_IN_MS2);
    commonX = (approxAccX == 0.0 && approxAccY == 1.0); commonZ = (approxAccZ == 0.0 && approxAccY == 1.0);
    layingF = (commonX && approxAccZ == -1.0);
    layingB = (commonX && approxAccZ == +1.0);
    layingL = (commonZ && approxAccX == -1.0);
    layingR = (commonZ && approxAccX == +1.0);
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
    
    standing = (yMinIdx < yMaxIdx && !walking);
    sitting = (yMaxIdx < yMinIdx && currentState != MDC_WALKING);
}

static void checkIfStandingOrSitting(void)
{
    uint32_t nowT;
    double acc;

    acc = sqrt(sq(accx) + sq(accy) + sq(accz));

    if 
    (
        acc < ACC_DETECT_THS_LO || acc > ACC_DETECT_THS_HI /* Add high accelerometer ths and replace or with and */
    )
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
    if (graceT > 0) { return; }
    //standing |= (!walking && currentState == walking);
    standing |= !(layingB || layingF || layingL || layingR || sitting || walking) && (currentState == MDC_WALKING);
    if (layingB) { currentState = MDC_LAYING_B; }
    else if (layingF) { currentState = MDC_LAYING_F; }
    else if (layingL) { currentState = MDC_LAYING_L; }
    else if (layingR) { currentState = MDC_LAYING_R; }
    else if (walking) { currentState = MDC_WALKING; }
    else if (sitting) { currentState = MDC_SITTING; }
    else if (standing) { currentState = MDC_STANDING; }
}

void initRecorder(void)
{
    accx = 0.0; accy = 0.0; accz = 0.0;
    rotx = 0.0; roty = 0.0; rotz = 0.0;
    currentState = MDC_SITTING;
    sampleT20 = 0; sampleT200 = 0; graceT = 0;
    walking = false; standing = false; sitting = true;
    layingF = false; layingB = false; layingR = false; layingL = false;
}

void runMDC(void)
{
    pollIMU(accx, accy, accz, rotx, roty, rotz);

    checkIfLaying();
    checkIfWalking();
    checkIfStandingOrSitting();
    setState();

    //srprintf("Walking: %d / Standing: %d / Sitting: %d / LayingF: %d / LayingB: %d / LayingR: %d / LayingL: %d\n", walking, standing, sitting, layingF, layingB, layingR, layingL);
}

MDC_STATE getCurrentMDCState(void)
{
    return currentState;
}