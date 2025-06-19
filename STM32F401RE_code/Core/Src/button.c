#include "stm32f4xx.h"
#include "delay.h"
#include <stdint.h>

volatile int sw1_flag = 0;
volatile int sw2_flag = 0;

#define DEBOUNCE_TIME 30
volatile uint32_t last_sw1_time = 0;
volatile uint32_t last_sw2_time = 0;

extern volatile uint32_t msTicks;

void Button_Init(void) {
    // Enable clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB3, PB4 input mode, pull-up
    GPIOB->MODER &= ~(3 << (3 * 2)); // PB3 input
    GPIOB->MODER &= ~(3 << (4 * 2)); // PB4 input
    GPIOB->PUPDR &= ~(3 << (3 * 2));
    GPIOB->PUPDR |= (1 << (3 * 2));  // PB3 pull-up
    GPIOB->PUPDR &= ~(3 << (4 * 2));
    GPIOB->PUPDR |= (1 << (4 * 2));  // PB4 pull-up

    // Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // EXTI3 -> PB3, EXTI4 -> PB4
    SYSCFG->EXTICR[0] &= ~(0xF << (4 * 3)); // Clear EXTI3 (bits 12..15)
    SYSCFG->EXTICR[0] |=  (1 << (4 * 3));   // Set PB3 (1 = Port B) for EXTI3
    SYSCFG->EXTICR[1] &= ~(0xF << 0);       // Clear EXTI4 (bits 0..3)
    SYSCFG->EXTICR[1] |=  (1 << 0);         // Set PB4 (1 = Port B) for EXTI4

    // Unmask EXTI3, EXTI4
    EXTI->IMR  |= (1 << 3) | (1 << 4);
    EXTI->FTSR |= (1 << 3) | (1 << 4); // Falling edge trigger

    // Enable NVIC interrupt EXTI3, EXTI4
    NVIC_SetPriority(EXTI3_IRQn, 1);
    NVIC_SetPriority(EXTI4_IRQn, 2);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
}

void EXTI3_IRQHandler(void) {
    if (EXTI->PR & (1 << 3)) {
        EXTI->PR |= (1 << 3); // Clear pending bit
        uint32_t now = msTicks;
        if (now - last_sw1_time > DEBOUNCE_TIME) {
            if ((GPIOB->IDR & (1 << 3)) == 0) { // Button pressed (active low)
                sw1_flag = 1;
                last_sw1_time = now;
            }
        }
    }
}

void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1 << 4)) {
        EXTI->PR |= (1 << 4); // Clear pending bit
        uint32_t now = msTicks;
        if (now - last_sw2_time > DEBOUNCE_TIME) {
            if ((GPIOB->IDR & (1 << 4)) == 0) { // Button pressed (active low)
                sw2_flag = 1;
                last_sw2_time = now;
            }
        }
    }
}
