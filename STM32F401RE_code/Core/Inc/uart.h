#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"

void UART1_Init(void);
void UART1_SendChar(char c);
void UART1_SendString(const char* str);
void Send_Alert_ToESP(float ppm);
//void Send_PPM_ToESP(float ppm);

#endif
