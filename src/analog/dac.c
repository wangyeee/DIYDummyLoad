#include "dac.h"
#include "stm32f10x_gpio.h"
#include "string.h"

dacStruct_s dacData;

/*
TODO setup DMA for DAC
DAC_CH1 -> DMA2_CH3
DAC_CH2 -> DMA2_CH4
*/
void dacInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
       connected to the DAC converter. In order to avoid parasitic consumption, 
       the GPIO pin should be configured in analog */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  // clock already enabled in adcInit

    /* DAC channel1 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_2047;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);

    /* Set DAC dual channel DHR12RD register */
    DAC_SetDualChannelData(DAC_Align_12b_R, 0x00, 0x00);

    memset(&dacData, 0, sizeof(dacStruct_s));
    {
    // TODO load parameters from flash
    uint8_t i;
    for (i = 0; i < DAC_CHANNELS; i++) {
        dacData.scale[i] = 1.0f;
        dacData.offset[i] = 0.0f;
    }
    dacData.reference = 3.3f;
    dacData.resolution = 4095.0f;  // 12-bit
    }
}

void dacSetValue(float volt, dacChannelType channel) {
    if (volt < dacGetStep()) {
        // anything less than what the dac could handle is zero
        volt = 0.0f;
    }
    if (channel == CHANNEL_EXT || channel == CHANNEL_INT) {
        int val = (int) (dacData.resolution * volt / dacData.reference);
        dacData.setVoltage[channel] = volt;
        if (channel == CHANNEL_EXT) {
            DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t) val);
        } else if (channel == CHANNEL_INT) {
            DAC_SetChannel2Data(DAC_Align_12b_R, (uint16_t) val);
        }
    }
}

float dacGetValue(dacChannelType channel) {
    if (channel == CHANNEL_EXT || channel == CHANNEL_INT) {
        return dacData.setVoltage[channel];
    }
    return 0.0f;
}

float dacGetStep(void) {
    return dacData.reference / dacData.resolution;
}
