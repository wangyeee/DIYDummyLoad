#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"

typedef enum {
    INTERNAL = 0,
    EXTERNAL = 1
} adcVoltageType;

#define ADC_RESOLUTION (4095.0f / 3.3f)

#define SHUNT_RESISTOR 0.2f

#define ADC_VOLT_DIVIDER_LOW 10.0
#define ADC_VOLT_DIVIDER_UP 100.0

#define ADC_EXT_VOLT_DIVIDER_LOW 10.0
#define ADC_EXT_VOLT_DIVIDER_UP 100.0

/*
CH0 -> CURR
CH1 -> VOLT_EXT
CH2 -> VOLT
CH3 -> TEMP
*/
typedef enum {
    ADC_CURRENT = 0,
    ADC_VOLTAGE_EXT = 1,
    ADC_VOLTAGE = 2,
    ADC_TEMPERATURE = 3,
    ADC_IN_NUM = 4
} adcChannelType;

typedef struct {
    __IO uint16_t adcRawData[ADC_IN_NUM];
    float voltage;
    float current;
    float extVoltage;
    float temperature;

    float scale[ADC_IN_NUM];
    float offset[ADC_IN_NUM];
} adcStruct_s;

extern void adcInit(void);
extern float adcGetVoltage(adcVoltageType source);
extern float adcGetCurrent(void);
extern float adcGetTemperature(void);
extern uint8_t adcDecode(void);

#endif /* __ADC_H */
