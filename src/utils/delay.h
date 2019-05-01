#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

extern void init_delay(void);
extern void delay_ms(uint32_t ms);

#endif /* __DELAY_H */
