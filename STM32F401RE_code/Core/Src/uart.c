#include "stm32f4xx.h"
#include "uart.h"
#include <stdio.h>

void UART1_Init(void) {
    // Bật clock GPIOA và USART1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // PA9 (TX), PA10 (RX): Alternate function
    GPIOA->MODER &= ~((3 << (2 * 9)) | (3 << (2 * 10)));
    GPIOA->MODER |= (2 << (2 * 9)) | (2 << (2 * 10)); // AF mode

    // AF7 cho USART1 trên PA9, PA10
    GPIOA->AFR[1] &= ~((0xF << (4 * (9 - 8))) | (0xF << (4 * (10 - 8))));
    GPIOA->AFR[1] |= (7 << (4 * (9 - 8))) | (7 << (4 * (10 - 8)));

    // Baudrate 115200, giả sử APB2 clock là 84MHz (theo CubeMX mặc định cho F401RE)
    USART1->BRR = (uint32_t)(16000000 / 9600);

    USART1->CR1 = USART_CR1_TE | USART_CR1_UE; // Enable TX, USART
}

void UART1_SendChar(char c) {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void UART1_SendString(const char* str) {
    while (*str) {
        UART1_SendChar(*str++);
    }
}

// Gửi thông báo ALERT lên ESP32
void Send_Alert_ToESP(float ppm) {
    char buff[64];
    int ppm_int = (int)ppm;
    snprintf(buff, sizeof(buff), "ALERT! PPM=%d\n", ppm_int); // Chỉ dùng '\n' (ESP32 nhận đến ký tự này mới publish)
    UART1_SendString(buff);
}

/*
// Gửi giá trị ppm thông thường định kỳ (nếu cần push liên tục ppm)
void Send_PPM_ToESP(float ppm) {
    char buff[64];
    snprintf(buff, sizeof(buff), "PPM=%.1f\n", ppm); // Định dạng đơn giản, có thể đổi tùy topic
    UART1_SendString(buff);
}
*/


