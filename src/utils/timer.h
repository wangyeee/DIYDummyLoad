#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

extern void timerInit(void);
extern uint16_t timerGetEncoderCount(void) ;

#endif /* __TIMER_H */
