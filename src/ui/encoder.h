#ifndef __ENCODE_H
#define __ENCODE_H

#include "timer.h"

#define encoderGetRawCount() timerGetEncoderCount()

#ifndef UINT16_MAX
// should be defined in "stdint.h"
#define UINT16_MAX 65536
#endif /* UINT16_MAX */

#define ENCODER_OVERFLOW_THRESHOLD 10000

typedef struct {
    int32_t count;
    uint8_t revert;
    uint8_t buttonStatus;
} encoderStruct_s;

extern void encoderInit(void);
extern uint8_t encoderGetButton(void);
extern int32_t encoderGetCount(void);
extern void encoderResetButton(void);
extern void encoderSetRevert(uint8_t rev);

#endif /* __ENCODE_H */
