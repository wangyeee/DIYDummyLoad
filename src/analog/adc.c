#include "adc.h"
#include "stm32f10x_gpio.h"
#include "string.h"

static adcStruct_s adcData;

float adcGetVoltage(adcVoltageType source) {
    switch (source) {
    case INTERNAL:
        return adcData.voltage;
    case EXTERNAL:
        return adcData.extVoltage;
    default:
        return 0.0f;
    }
}

float adcGetCurrent(void) {
    return adcData.current;
}

float adcGetTemperature(void) {
    return adcData.temperature;
}

uint8_t adcDecode(void) {
    adcData.current = ((float) adcData.adcRawData[ADC_CURRENT]) / SHUNT_RESISTOR / ADC_RESOLUTION * adcData.scale[ADC_CURRENT] + adcData.offset[ADC_CURRENT];
    adcData.extVoltage = ((float) adcData.adcRawData[ADC_VOLTAGE_EXT]) / ADC_EXT_VOLT_DIVIDER_LOW / ADC_RESOLUTION * (ADC_EXT_VOLT_DIVIDER_LOW + ADC_EXT_VOLT_DIVIDER_UP) * adcData.scale[ADC_VOLTAGE_EXT] + adcData.offset[ADC_VOLTAGE_EXT];
    adcData.voltage = ((float) adcData.adcRawData[ADC_VOLTAGE]) / ADC_VOLT_DIVIDER_LOW / ADC_RESOLUTION * (ADC_VOLT_DIVIDER_LOW + ADC_VOLT_DIVIDER_UP) * adcData.scale[ADC_VOLTAGE] + adcData.offset[ADC_VOLTAGE];
    adcData.temperature = ((float) adcData.adcRawData[ADC_TEMPERATURE]) * adcData.scale[ADC_TEMPERATURE] + adcData.offset[ADC_TEMPERATURE];
    return 0;
}

static void adc1LowLevelInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* Enable GPIOs and ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1, ENABLE);

    /* Configure PA0~3 (ADC Channel11~4) as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* DMA1 channel1 configuration */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &(adcData.adcRawData);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_IN_NUM;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 configuration */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_IN_NUM;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channel1 configuration */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Enable ADC1 reset calibration register */
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while (ADC_GetResetCalibrationStatus(ADC1));

    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC1));

    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void adcInit(void) {
    char i;
    memset(&adcData, 0, sizeof(adcStruct_s));
    for (i = 0; i < ADC_IN_NUM; i++) {
        // TODO load parameters from flash
        adcData.scale[i] = 1.0f;
        adcData.offset[i] = 0.0f;
    }
    adc1LowLevelInit();
}
