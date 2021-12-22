#include "MDC.h"
#include "IMU.h"
#include "MacroUtils.h"
#include "DoubleBuffer.h"
#include <arduinoFFT.h>

static double ryReal[SAMPLES], ryImag[SAMPLES];
static DoubleBuffer ryDataBuffer = DoubleBuffer(SAMPLES);
static DoubleBuffer yDataBuffer = DoubleBuffer(SAMPLES);
static arduinoFFT FFT = arduinoFFT(ryReal, ryImag, (uint16_t) SAMPLES, SAMPLING_RATE);
static MDC_STATE currentState;
static double accx, accy, accz;
static double rotx, roty, rotz;
static uint32_t prevT, prevMovT;

static void checkMovement(void)
{
    MDC_STATE newState = currentState;

    /* Stationary detection */
    double yMin, yMax;
    uint16_t yMinIdx, yMaxIdx;
    double* bufferWindow;
    bufferWindow = yDataBuffer.getBuffer() + BUFFER_WINDOW_OFFSET;
    for 
    (
        uint16_t i = 0, yMin = bufferWindow[0], yMax = bufferWindow[0], yMinIdx = 0, yMaxIdx = 0; 
        i < BUFFER_WINDOW_SIZE; 
        i++
    )
    {
        if (bufferWindow[i] > yMax) { yMax = bufferWindow[i]; yMinIdx = i; }
        if (bufferWindow[i] < yMin) { yMin = bufferWindow[i]; yMaxIdx = i; }
    }
    if (yMax - yMin > DETECTION_THS) {  }

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
    //srprintf("Major peak: %f %d -- Peak value: %f\n", approxPeak, peakIndex, ryReal[peakIndex]);
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
}

void runMDC(void)
{
    pollIMU(accx, accy, accz, rotx, roty, rotz);

    if (millis() - prevT < (1000.0 / SAMPLING_RATE))
        return;

    /* Record data */
    ryDataBuffer.push(roty);
    yDataBuffer.push(accy);

    parseData();

    prevT = millis();
}

MDC_STATE getCurrentMDCState(void)
{
    return currentState;
}