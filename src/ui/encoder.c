#include "encoder.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "string.h"

static encoderStruct_s encoderData;

void encoderInit(void) {
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // config PC13 as encoder button
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Connect EXTI13 Line to PC13 pin
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);

    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    memset(&encoderData, 0, sizeof(encoderStruct_s));
}

void encoderResetButton(void) {
    encoderData.buttonStatus = 0;
}

uint8_t encoderGetButton(void) {
    return encoderData.buttonStatus;
}

int32_t encoderGetCount(void) {
    int32_t tmp = encoderData.count;
    encoderData.count = encoderGetRawCount();
    tmp = encoderData.count - tmp;
    if (tmp > 0) {
        tmp = tmp > ENCODER_OVERFLOW_THRESHOLD ? UINT16_MAX - tmp : tmp;
    } else {
        tmp = tmp < 0 - ENCODER_OVERFLOW_THRESHOLD ? UINT16_MAX + tmp : tmp;
    }
    return encoderData.revert ? -tmp : tmp;
}

void encoderSetRevert(uint8_t rev) {
    encoderData.revert = rev;
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        encoderData.buttonStatus = 1;
        /* Clear the  EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}
