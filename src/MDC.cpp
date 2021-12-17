#include "MDC.h"
#include "IMU.h"
#include "MacroUtils.h"

static float accx, accy, accz;
static float rotx, roty, rotz;
static float prevT, graceTimer;
static uint32_t samplesRecorded;
static SamplesBuffer axdata, aydata, azdata;
static SamplesBuffer rxdata, rydata, rzdata;
static SamplesBuffer dtdata;
static MDC_STATE currentState;

void initRecorder()
{
    accx = 0; accy = 0; accz = 0;
    rotx = 0; roty = 0; rotz = 0;
    prevT = 0; graceTimer = 0;
    samplesRecorded = 0;
    currentState = MDC_SITTING;
}

static void classifyMovement(void)
{
    SamplesBuffer saxdata, saydata, sazdata;
    SamplesBuffer srxdata, srydata, srzdata;
    float ax, ay, az;
    uint32_t i;
    float ymin, ymax;
    uint32_t ymini, ymaxi;
    float standing, sitting;

    for (ymini = 99.0f, ymaxi = 0.0f, i = 0; i < samplesRecorded; i++)
    {
        if (i < 4) 
        {
            saxdata[i] = axdata[i]; saydata[i] = aydata[i]; sazdata[i] = azdata[i];
            srxdata[i] = rxdata[i]; srydata[i] = rydata[i]; srzdata[i] = rzdata[i];
        }
        else
        {
            saxdata[i] = (axdata[i - 4] + axdata[i - 3] + axdata[i - 2] + axdata[i - 1] + axdata[i - 0] + axdata[i - 1] + axdata[i + 2] + axdata[i + 3] + axdata[i + 4]) / 9.0f;
            saydata[i] = (aydata[i - 4] + aydata[i - 3] + aydata[i - 2] + aydata[i - 1] + aydata[i - 0] + aydata[i - 1] + aydata[i + 2] + aydata[i + 3] + aydata[i + 4]) / 9.0f;
            sazdata[i] = (azdata[i - 4] + azdata[i - 3] + azdata[i - 2] + azdata[i - 1] + azdata[i - 0] + azdata[i - 1] + azdata[i + 2] + azdata[i + 3] + azdata[i + 4]) / 9.0f;
            
            srxdata[i] = (rxdata[i - 4] + rxdata[i - 3] + rxdata[i - 2] + rxdata[i - 1] + rxdata[i - 0] + rxdata[i - 1] + rxdata[i + 2] + rxdata[i + 3] + rxdata[i + 4]) / 9.0f;
            srydata[i] = (rydata[i - 4] + rydata[i - 3] + rydata[i - 2] + rydata[i - 1] + rydata[i - 0] + rydata[i - 1] + rydata[i + 2] + rydata[i + 3] + rydata[i + 4]) / 9.0f;
            srzdata[i] = (rzdata[i - 4] + rzdata[i - 3] + rzdata[i - 2] + rzdata[i - 1] + rzdata[i - 0] + rzdata[i - 1] + rzdata[i + 2] + rzdata[i + 3] + rzdata[i + 4]) / 9.0f;
        }
        
        if (ymin > saydata[i]) { ymin = saydata[i]; ymini = i; }
        if (ymax < saydata[i]) { ymax = saydata[i]; ymaxi = i; }
    
        Serial.print(srxdata[i]); Serial.print(", "); Serial.print(srydata[i]); Serial.print(", "); Serial.print(srzdata[i]); Serial.println();
    }

    standing = (ymini < ymaxi);
    sitting = (ymini > ymaxi);

    currentState = (standing > sitting) ? MDC_STANDING : MDC_SITTING;

    //Serial.println("--------------- NEW PACKET ---------------");

    //info("%s / ymin: %f, ymini: %d, ymax: %f, ymaxi: %d", (standing) ? "Standing" : "Sitting", ymin, ymini, ymax, ymaxi);
}

static void checkMovement(void)
{
    float nowT;
    //info("%d", samplesRecorded);
    if 
    (
        (
            sqrtf(accx * accx + accy * accy + accz * accz) < ACC_REC_THS &&
            sqrtf(rotx * rotx + roty * roty + rotz * rotz) < GYR_REC_THS
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
    if (samplesRecorded < MAX_SAMPLES && (dtdata[samplesRecorded] = (nowT - prevT)) > (1.0f / SAMPLE_RATE))
    {
        axdata[samplesRecorded] = accx; aydata[samplesRecorded] = accy; azdata[samplesRecorded] = accz;
        rxdata[samplesRecorded] = rotx; rydata[samplesRecorded] = roty; rzdata[samplesRecorded] = rotz;
        ++samplesRecorded;
        prevT = nowT;
    }
}

void runMDC(void)
{
    pollIMU(accx, accy, accz, rotx, roty, rotz);

    checkMovement();

    if (abs(roundf(accx / G_IN_MS2)) == 0.0f && abs(roundf(accy  / G_IN_MS2)) == 1.0f && abs(roundf(accz / G_IN_MS2)) == 1.0f) { currentState = MDC_LAYING; }
}

MDC_STATE getCurrentMDCState(void)
{
    return currentState;
}