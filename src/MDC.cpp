#include "MDC.h"
#include "IMU.h"
#include "MacroUtils.h"
#include <math.h>
#include <arduinoFFT.h>

static double accx, accy, accz;
static double rotx, roty, rotz;
static double prevT, graceTimer;
static uint16_t samplesRecorded;
static SamplesBuffer axdata, aydata, azdata;
static SamplesBuffer rxdata, rydata, rzdata;
static SamplesBuffer dtdata;
static arduinoFFT FFT;
static MDC_STATE currentState;

static void classifyMovement(void)
{
    SamplesBuffer saxdata, saydata, sazdata;
    SamplesBuffer srxdata, srydata, srzdata;
    uint32_t i;
    double ymin, ymax;
    uint32_t ymini, ymaxi;
    double standing, sitting;

    for (ymin = 99.0, ymax = 0.0, i = 0; i < samplesRecorded; i++)
    {
        if (i < 4) 
        {
            saxdata[i] = axdata[i]; saydata[i] = aydata[i]; sazdata[i] = azdata[i];
            srxdata[i] = rxdata[i]; srydata[i] = rydata[i]; srzdata[i] = rzdata[i];
        }
        else
        {
            saxdata[i] = (axdata[i - 4] + axdata[i - 3] + axdata[i - 2] + axdata[i - 1] + axdata[i - 0] + axdata[i - 1] + axdata[i + 2] + axdata[i + 3] + axdata[i + 4]) / 9.0;
            saydata[i] = (aydata[i - 4] + aydata[i - 3] + aydata[i - 2] + aydata[i - 1] + aydata[i - 0] + aydata[i - 1] + aydata[i + 2] + aydata[i + 3] + aydata[i + 4]) / 9.0;
            sazdata[i] = (azdata[i - 4] + azdata[i - 3] + azdata[i - 2] + azdata[i - 1] + azdata[i - 0] + azdata[i - 1] + azdata[i + 2] + azdata[i + 3] + azdata[i + 4]) / 9.0;
            
            srxdata[i] = (rxdata[i - 4] + rxdata[i - 3] + rxdata[i - 2] + rxdata[i - 1] + rxdata[i - 0] + rxdata[i - 1] + rxdata[i + 2] + rxdata[i + 3] + rxdata[i + 4]) / 9.0;
            srydata[i] = (rydata[i - 4] + rydata[i - 3] + rydata[i - 2] + rydata[i - 1] + rydata[i - 0] + rydata[i - 1] + rydata[i + 2] + rydata[i + 3] + rydata[i + 4]) / 9.0;
            srzdata[i] = (rzdata[i - 4] + rzdata[i - 3] + rzdata[i - 2] + rzdata[i - 1] + rzdata[i - 0] + rzdata[i - 1] + rzdata[i + 2] + rzdata[i + 3] + rzdata[i + 4]) / 9.0;
        }
        
        if (ymin > saydata[i]) { ymin = saydata[i]; ymini = i; }
        if (ymax < saydata[i]) { ymax = saydata[i]; ymaxi = i; }
    
        //Serial.print(srxdata[i]); Serial.print(", "); Serial.print(srydata[i]); Serial.print(", "); Serial.print(srzdata[i]); Serial.println();
    }

    standing = (ymini < ymaxi);
    sitting = (ymini > ymaxi);

    currentState = (standing > sitting) ? MDC_STANDING : MDC_SITTING;

    //Serial.println("--------------- NEW PACKET ---------------");

    //info("%s / ymin: %f, ymini: %d, ymax: %f, ymaxi: %d", (standing) ? "Standing" : "Sitting", ymin, ymini, ymax, ymaxi);
}

static void checkMovement(void)
{
    double nowT;

    if 
    (
        (
            sqrt(accx * accx + accy * accy + accz * accz) < ACC_REC_THS &&
            sqrt(rotx * rotx + roty * roty + rotz * rotz) < GYR_REC_THS
        )
        || samplesRecorded >= MAX_SAMPLES
    )
    {
        if ((graceTimer > 0 && seconds() - graceTimer > GRACE_TIMER_DELAY) || samplesRecorded >= MAX_SAMPLES)
        {
            if (samplesRecorded >= MIN_SAMPLES) { classifyMovement(); }
            graceTimer = 0;
            samplesRecorded = 0;
            if (samplesRecorded < MAX_SAMPLES) { delay(COOLDOWN_TIMER_DELAY); }
        }
        return;
    }

    if (graceTimer == 0)
    {
        graceTimer = seconds();
        samplesRecorded = 0;
    }

    nowT = seconds();
    if (samplesRecorded < MAX_SAMPLES && (dtdata[samplesRecorded] = (nowT - prevT)) > (1.0 / SAMPLE_RATE))
    {
        axdata[samplesRecorded] = accx; aydata[samplesRecorded] = accy; azdata[samplesRecorded] = accz;
        rxdata[samplesRecorded] = rotx; rydata[samplesRecorded] = roty; rzdata[samplesRecorded] = rotz;
        ++samplesRecorded;
        prevT = nowT;
    }
}

void initRecorder(void)
{
    accx = 0.0; accy = 0.0; accz = 0.0;
    rotx = 0.0; roty = 0.0; rotz = 0.0;
    prevT = 0; graceTimer = 0;
    samplesRecorded = 0;
    currentState = MDC_SITTING;
    FFT = arduinoFFT();
}

void runMDC(void)
{
    double approxAccX, approxAccY, approxAccZ;
    pollIMU(accx, accy, accz, rotx, roty, rotz);

    approxAccX = round(accx / G_IN_MS2); approxAccY = round(accy / G_IN_MS2); approxAccZ = round(accz / G_IN_MS2);

    if (approxAccX == 0.0 && approxAccY == 1.0 && abs(approxAccZ) == 1.0) { currentState = (approxAccZ > 0.0) ? MDC_LAYING_B : MDC_LAYING_F; }
    else if (abs(approxAccX) == 1.0 && approxAccY == 1.0 && approxAccZ == 0.0) { currentState = (approxAccX > 0.0) ? MDC_LAYING_R : MDC_LAYING_L; }
    else { checkMovement(); }
}

MDC_STATE getCurrentMDCState(void)
{
    return currentState;
}