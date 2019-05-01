#ifndef __DAC_H
#define __DAC_H

#include "stm32f10x.h"
#include "stm32f10x_dac.h"

typedef enum {
    CHANNEL_EXT = 0,
    CHANNEL_INT = 1,
    DAC_CHANNELS
} dacChannelType;

typedef struct {
    float setVoltage[DAC_CHANNELS];
    float reference;
    float resolution;
    float scale[DAC_CHANNELS];
    float offset[DAC_CHANNELS];
} dacStruct_s;

extern void dacInit(void);
extern void dacSetValue(float volt, dacChannelType channel);
extern float dacGetValue(dacChannelType channel);
extern float dacGetStep(void);

#endif /* __DAC_H */
