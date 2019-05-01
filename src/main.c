#include "stm32f10x.h"
#include "ILI9163C.h"
#include "delay.h"
#include "adc.h"
#include "dac.h"
#include "timer.h"
#include "encoder.h"
#include "flash.h"
#include "dummyload.h"

int main(void) {
    init_delay();
    flashInit();
    timerInit();
    encoderInit();  // must be after timer init
    ILI9163CInit();
    adcInit();
    encoderSetRevert(1);
    dacInit();

    // initilize dummy load after all peripherals are initialized
    loadInit();

    for (;;) {
        int delta;
        loadModeType mode;
        adcDecode();
        delta = encoderGetCount();
        mode = getMode();
        switch (mode) {
        case MODE_CC:
            setConstantCurrent(delta);
            break;
        case MODE_CP:
            setConstantPower(delta);
            break;
        case MODE_CR:
            setConstantResistance(delta);
            break;
        default:
            showCurrentStatus();
            break;
        }
        if (encoderGetButton()) {
            int newMode = ((int) mode + 1) % ALL_MODES;
            setMode((loadModeType) newMode);
            encoderResetButton();
        }
        delay_ms(20);
    }
}
