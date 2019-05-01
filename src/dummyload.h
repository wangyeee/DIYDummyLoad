#ifndef __DUMMYLOAD_H
#define __DUMMYLOAD_H

#include "stm32f10x.h"
#include "adc.h"

#define DAC_DIVIDER_UP 15.0
#define DAC_DIVIDER_LOW 20.0

/* 2A for test only */
#define MAX_AMP 2.0f

#define FLOAT_CHAR_BUF_SIZE 10
#define FLOAT_SCALE 100.0f
#define FLOAT_INT_MASK "%d.%02d"

typedef enum {
    MODE_STOP = 0,
    MODE_CC, /* constant current */
    MODE_CR, /* constant resistance */
    MODE_CP, /* constant power */
    ALL_MODES
} loadModeType;

typedef struct {
    loadModeType mode;
    adcVoltageType voltageSource;
    uint8_t maxCurrentReached;
    float currentSet;
    float resistanceSet;
    float powerSet;
    float current;
    float resistance;
    float power;
    char floatDispBuf[FLOAT_CHAR_BUF_SIZE];
} loadStatus_s;

extern void loadInit(void);
extern void setConstantCurrentAbs(float value);
extern void setConstantResistanceAbs(float value);
extern void setConstantPowerAbs(float value);

extern void setMode(loadModeType newMode);
extern loadModeType getMode(void);
extern float getCurrent(void);
extern float getResistance(void);
extern float getPower(void);

extern adcVoltageType getVoltageSource(void);
extern void setVoltageSource(adcVoltageType source);
extern void showCurrentStatus(void);

#define setConstantCurrent(d) setConstantCurrentAbs(getCurrent() + (dacGetStep() * d))
#define setConstantResistance(d) setConstantResistanceAbs(getResistance() + (dacGetStep() * 100.0f * d))
#define setConstantPower(d) setConstantPowerAbs(getPower() + (dacGetStep() * 10.0f * d))

#define toggleVoltageSource() if (getVoltageSource() == INTERNAL) \
                                {setVoltageSource(EXTERNAL);} else \
                                {setVoltageSource(INTERNAL);}

#endif /* __DUMMYLOAD_H */
