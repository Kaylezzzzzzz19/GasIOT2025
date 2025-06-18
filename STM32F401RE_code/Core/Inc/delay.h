#ifndef __DELAY_H
#define __DELAY_H

#include <stdint.h>

extern volatile uint32_t msTicks;

void Delay_Init(void);
void Delay_ms(uint32_t ms);

#endif
