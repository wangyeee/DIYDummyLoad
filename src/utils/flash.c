#include "flash.h"
#include "string.h"

configItemStruct_s allConfigItems[CFG_COUNT];

void flashInit(void) {
    memset(&allConfigItems, 0, sizeof(configItemStruct_s) * CFG_COUNT);
    loadConfig();
}

void loadDefaultConfig(void) {
    uint8_t i;

    allConfigItems[CFG_MAGIC].index = CFG_MAGIC;
    allConfigItems[CFG_MAGIC].data.ui32 = CONFIG_MAGIC_VALUE;
    allConfigItems[CFG_VERSION].index = CFG_VERSION;
    allConfigItems[CFG_VERSION].data.f32 = 1.0f;

    for (i = 0; i < ADC_IN_NUM; i++) {
        allConfigItems[CFG_ADC_SCALE_BASE + i].index = CFG_ADC_SCALE_BASE + i;
        allConfigItems[CFG_ADC_SCALE_BASE + i].data.f32 = 1.0f;
        allConfigItems[CFG_ADC_OFFSET_BASE + i].index = CFG_ADC_OFFSET_BASE + i;
        allConfigItems[CFG_ADC_OFFSET_BASE + i].data.f32 = 0.0f;
    }

    for (i = 0; i < DAC_CHANNELS; i++) {
        allConfigItems[CFG_DAC_SCALE_BASE + i].index = CFG_DAC_SCALE_BASE + i;
        allConfigItems[CFG_DAC_SCALE_BASE + i].data.f32 = 1.0f;
        allConfigItems[CFG_DAC_OFFSET_BASE + i].index = CFG_DAC_OFFSET_BASE + i;
        allConfigItems[CFG_DAC_OFFSET_BASE + i].data.f32 = 0.0f;
    }

    allConfigItems[CFG_WIFI_EN].index = CFG_WIFI_EN;
    allConfigItems[CFG_WIFI_EN].data.ui32 = 0;  // disable WiFi by default

    allConfigItems[CFG_TEST].index = CFG_TEST;
    allConfigItems[CFG_TEST].data.ui32 = 0;  // test only
}

void loadConfig(void) {
    uint32_t t;

    memcpy((uint8_t*) allConfigItems, (uint8_t*) CONFIG_FLASH_PAGE_ADDR, sizeof(configItemStruct_s) * CFG_COUNT);
    t = allConfigItems[CFG_MAGIC].data.ui32;
    if (t != CONFIG_MAGIC_VALUE) {
        loadDefaultConfig();
    }
}

uint8_t saveConfig(uint8_t verify) {
    FLASH_Status status;
    uint32_t addr;
    uint8_t flashBreak = 0;
    uint32_t i;

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    status = FLASH_ErasePage(CONFIG_FLASH_PAGE_ADDR);
    if (status != FLASH_COMPLETE) {
        FLASH_Lock();
        return 3;
    }
    addr = CONFIG_FLASH_PAGE_ADDR;

    for (i = 0; i < CFG_COUNT; i++) {
        if (FLASH_ProgramWord(addr, allConfigItems[i].index) != FLASH_COMPLETE) {
            flashBreak = 1;
            break;
        }
        addr += sizeof(uint32_t);
        if (FLASH_ProgramWord(addr, allConfigItems[i].data.ui32) != FLASH_COMPLETE) {
            flashBreak = 1;
            break;
        }
        addr += sizeof(uint32_t);
    }
    FLASH_Lock();
    if (flashBreak) {
        return 1;
    }
    if (verify) {
        uint8_t* cfgBase = (uint8_t*) allConfigItems;
        uint8_t* flashBase = (uint8_t*) CONFIG_FLASH_PAGE_ADDR;

        for (i = 0; i < sizeof(configItemStruct_s) * CFG_COUNT; i++) {
            if (cfgBase[i] != flashBase[i]) {
                return 2;
            }
        }
    }
    return 0;
}

uint8_t getConfig(configIndexType index, configItemStruct_s** config) {
    if (index < CFG_COUNT) {
        *config = &(allConfigItems[index]);
        return 0;
    }
    return 1;
}

uint8_t setConfig(configItemStruct_s* config) {
    // not needed for now
    return 0;
}
