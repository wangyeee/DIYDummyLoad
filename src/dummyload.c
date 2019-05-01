#include "dummyload.h"
#include "dac.h"
#include "ILI9163C.h"
#include "string.h"

static loadStatus_s status;

void loadInit(void) {
    memset(&status, 0, sizeof(loadStatus_s));
    status.mode = MODE_STOP;
    status.voltageSource = INTERNAL;
    status.maxCurrentReached = 0;
    status.currentSet = 0.0f;
    status.resistanceSet = 1000.0f;
    status.powerSet = 0.0f;
    dacSetValue(0.0f, CHANNEL_INT);
}

static char* floatToString(float f) {
    int a = (int) f;
    int b = FLOAT_SCALE * (f - a);
    int size = snprintf(status.floatDispBuf, FLOAT_CHAR_BUF_SIZE, FLOAT_INT_MASK, a, b);
    status.floatDispBuf[size < FLOAT_CHAR_BUF_SIZE ? size : FLOAT_CHAR_BUF_SIZE - 1] = 0;
    return status.floatDispBuf;
}

void showCurrentStatus(void) {
    setCursor(0, 5);
    lcd_println(" Electronic Load");
    lcd_println("");
    switch (status.mode) {
    case MODE_STOP:
        lcd_println(" Disarmed");
        lcd_println("                          ");
        break;
    case MODE_CC:
        lcd_println(" Mode: CC");
        lcd_printf(" Current Set: %sA     \n", floatToString(status.currentSet));
        break;
    case MODE_CR:
        lcd_println(" Mode: CR");
        lcd_printf(" Resistance Set: %sOhm\n", floatToString(status.resistanceSet));
        break;
    case MODE_CP:
        lcd_println(" Mode: CP");
        lcd_printf(" Power Set: %sW     \n", floatToString(status.powerSet));
        break;
    default:
        break;
    }
    lcd_printf(" Voltage:%sV(%s)   \n",
                floatToString(adcGetVoltage(status.voltageSource)),
                (status.voltageSource == INTERNAL ? "INT" : "EXT"));
    lcd_printf(" Current: %sA   \n", floatToString(status.current));
    lcd_printf(" Power: %sW   \n", floatToString(status.power));
    lcd_printf(" Resistance: %s   \n", floatToString(status.resistance));
    if (status.maxCurrentReached) {
        lcd_printf(" Current Limit: %sA   \n", floatToString(MAX_AMP));
    } else {
        lcd_println("                          ");
    }
}

void setConstantCurrentAbs(float amp) {
    if (amp >= 0.0f) {
        float volt = adcGetVoltage(status.voltageSource);
        if (amp >= MAX_AMP) {
            status.maxCurrentReached = 1;
            amp = MAX_AMP;
        } else {
            status.maxCurrentReached = 0;
        }
        status.currentSet = amp;
        if (status.mode != MODE_STOP) {
            dacSetValue(amp * SHUNT_RESISTOR / DAC_DIVIDER_LOW * (DAC_DIVIDER_LOW + DAC_DIVIDER_UP), CHANNEL_INT);
        } else {
            dacSetValue(0.0f, CHANNEL_INT);
        }
        status.current = adcGetCurrent();
        status.power = status.current * volt;
        status.resistance = volt / status.current;
        showCurrentStatus();
    }
}

void setConstantResistanceAbs(float ohm) {
    if (ohm >= 0.0f) {
        float volt = adcGetVoltage(status.voltageSource);
        float amp = volt / ohm;
        if (amp >= MAX_AMP) {
            status.maxCurrentReached = 1;
            amp = MAX_AMP;
            status.resistanceSet = volt / amp;
        } else {
            status.maxCurrentReached = 0;
            status.resistanceSet = ohm;
        }
        if (status.mode != MODE_STOP) {
            dacSetValue(amp * SHUNT_RESISTOR / DAC_DIVIDER_LOW * (DAC_DIVIDER_LOW + DAC_DIVIDER_UP), CHANNEL_INT);
        } else {
            dacSetValue(0.0f, CHANNEL_INT);
        }
        status.current = adcGetCurrent();
        status.power = status.current * volt;
        status.resistance = volt / status.current;
        showCurrentStatus();
    }
}

void setConstantPowerAbs(float watt) {
    if (watt >= 0.0f) {
        float volt = adcGetVoltage(status.voltageSource);
        float amp = watt / volt;
        if (amp >= MAX_AMP) {
            status.maxCurrentReached = 1;
            amp = MAX_AMP;
            status.powerSet = volt * amp;
        } else {
            status.maxCurrentReached = 0;
            status.powerSet = watt;
        }
        if (status.mode != MODE_STOP) {
            dacSetValue(amp * SHUNT_RESISTOR / DAC_DIVIDER_LOW * (DAC_DIVIDER_LOW + DAC_DIVIDER_UP), CHANNEL_INT);
        } else {
            dacSetValue(0.0f, CHANNEL_INT);
        }
        status.current = adcGetCurrent();
        status.power = status.current * volt;
        status.resistance = volt / status.current;
        showCurrentStatus();
    }
}

void setMode(loadModeType newMode) {
    status.mode = newMode;
    if (newMode == MODE_STOP) {
        dacSetValue(0.0f, CHANNEL_INT);
    }
    showCurrentStatus();
}

loadModeType getMode(void) {
    return status.mode;
}

float getCurrent(void) {
    return status.currentSet;
}

float getResistance(void) {
    return status.resistanceSet;
}

float getPower(void) {
    return status.powerSet;
}

adcVoltageType getVoltageSource(void) {
    return status.voltageSource;
}

void setVoltageSource(adcVoltageType source) {
    status.voltageSource = source;
}
