#include "stm32f4xx.h"
#include "delay.h"
#include <stdint.h>

volatile int sw1_flag = 0;
volatile int sw2_flag = 0;

#define DEBOUNCE_TIME 30
volatile uint32_t last_sw1_time = 0;
volatile uint32_t last_sw2_time = 0;

void Button_Init(void) {
    // Enable clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB3, PB4 input mode, pull-up
    GPIOB->MODER &= ~(3 << (3 * 2)); // PB3 input
    GPIOB->MODER &= ~(3 << (4 * 2)); // PB4 input
    GPIOB->PUPDR &= ~(3 << (3 * 2));
    GPIOB->PUPDR |= (1 << (3 * 2));
    GPIOB->PUPDR &= ~(3 << (4 * 2));
    GPIOB->PUPDR |= (1 << (4 * 2));

    // Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // EXTI3 -> PB3, EXTI4 -> PB4
    SYSCFG->EXTICR[0] &= ~(0xF << 4*3); // EXTI3 clear
    SYSCFG->EXTICR[0] |= (1 << 4*3);    // 1 = GPIOB
    SYSCFG->EXTICR[1] &= ~(0xF << 0);   // EXTI4 clear
    SYSCFG->EXTICR[1] |= (1 << 0);      // 1 = GPIOB

    // Unmask EXTI3, EXTI4
    EXTI->IMR |= (1 << 3) | (1 << 4);
    EXTI->FTSR |= (1 << 3) | (1 << 4);

    // Enable NVIC interrupt EXTI3, EXTI4
    NVIC_SetPriority(EXTI3_IRQn, 1); // Ưu tiên cao (số nhỏ hơn)
    NVIC_SetPriority(EXTI4_IRQn, 2); // Ưu tiên thấp hơn (số lớn hơn)
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
}

void EXTI3_IRQHandler(void) {
    uint32_t now = msTicks;
    if (EXTI->PR & (1 << 3)) {
        EXTI->PR |= (1 << 3);
        if (now - last_sw1_time > DEBOUNCE_TIME) {
            if ((GPIOB->IDR & (1 << 3)) == 0) {
                sw1_flag = 1;
                last_sw1_time = now;
            }
        }
    }
}

void EXTI4_IRQHandler(void) {
    uint32_t now = msTicks;
    if (EXTI->PR & (1 << 4)) {
        EXTI->PR |= (1 << 4);
        if (now - last_sw2_time > DEBOUNCE_TIME) {
            if ((GPIOB->IDR & (1 << 4)) == 0) {
                sw2_flag = 1;
                last_sw2_time = now;
            }
        }
    }
}
