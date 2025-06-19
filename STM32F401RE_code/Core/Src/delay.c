#include "delay.h"
#include "stm32f4xx.h"

volatile uint32_t msTicks = 0;

// Hàm khởi tạo SysTick cho delay ms
void Delay_Init(void) {
    // Sử dụng SystemCoreClock để đảm bảo đúng mọi cấu hình clock
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    SysTick->VAL = 0;
    NVIC_SetPriority(SysTick_IRQn, 3);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Handler(void)
{
    msTicks++;
}

// Delay ms không bị trôi nhờ biến msTicks
void Delay_ms(uint32_t ms) {
    uint32_t startTicks = msTicks;
    while ((msTicks - startTicks) < ms);
}
