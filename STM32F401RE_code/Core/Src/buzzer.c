#include "buzzer.h"
#include "stm32f4xx.h"

void Buzzer_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (1 << (10 * 2));
}

void Buzzer_On(void) {
    GPIOB->ODR |= (1 << 10);
}

void Buzzer_Off(void) {
    GPIOB->ODR &= ~(1 << 10);
}

