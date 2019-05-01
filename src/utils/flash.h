#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "adc.h"
#include "dac.h"

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
#define FLASH_PAGE_SIZE ((uint16_t)0x800)
#else
#define FLASH_PAGE_SIZE ((uint16_t)0x400)
#endif

#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08008000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0800C000)

#ifdef STM32F10X_XL
#define BANK2_WRITE_START_ADDR   ((uint32_t)0x08088000)
#define BANK2_WRITE_END_ADDR     ((uint32_t)0x0808C000)
#endif /* STM32F10X_XL */

// Page 127, size 2kB (STM32F103RCT6, 256k flash)
// 2kB / sizeof(configItemStruct_s) = 256
// 256 parameters can be stored per page
#define CONFIG_FLASH_PAGE_ADDR 0x0803F800
//#define CONFIG_FLASH_PAGE_ADDR 0x0801F800
#define CONFIG_MAGIC_VALUE 0x030934    // magic

typedef enum {
    CFG_MAGIC = 0,
    CFG_VERSION,
    CFG_ADC_SCALE_BASE,//2
    CFG_ADC_OFFSET_BASE = CFG_ADC_SCALE_BASE + ADC_IN_NUM,//6
    CFG_DAC_SCALE_BASE = CFG_ADC_OFFSET_BASE + ADC_IN_NUM,//10
    CFG_DAC_OFFSET_BASE = CFG_DAC_SCALE_BASE + DAC_CHANNELS,//12
    CFG_WIFI_EN = CFG_DAC_OFFSET_BASE + DAC_CHANNELS,//14
    CFG_TEST,
    CFG_COUNT//16
} configIndexType;

typedef struct {
    uint32_t index;
    union {
        uint32_t ui32;
        int32_t i32;
        float f32;
        uint16_t ui16[2];
        int16_t i16[2];
        uint8_t raw[4];
    } data;
    //char name[24];
} configItemStruct_s;

extern void flashInit(void);
extern void loadDefaultConfig(void);
extern void loadConfig(void);
extern uint8_t saveConfig(uint8_t verify);
extern uint8_t getConfig(configIndexType index, configItemStruct_s** config);
extern uint8_t setConfig(configItemStruct_s* config);

#endif /* __FLASH_H */
