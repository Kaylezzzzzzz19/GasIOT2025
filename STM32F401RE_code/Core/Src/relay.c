#include "relay.h"
#include "stm32f4xx.h"

void Relay_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (1 << (12 * 2));
}

void Relay_On(void) {
    GPIOB->ODR |= (1 << 12);
}

void Relay_Off(void) {
    GPIOB->ODR &= ~(1 << 12);
}
