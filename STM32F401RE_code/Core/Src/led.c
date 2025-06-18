#include "led.h"
#include "stm32f4xx.h"
#include "delay.h"

// LED Đỏ   : PA5
// LED Vàng : PA6
// LED Xanh Dương: PA7
// LED Xanh Lá: PA8  (đã chuyển từ PA9 sang PA8)

void LED_Init(void) {
    // Bật clock cho GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // PA5, PA6, PA7, PA8 output (01)
    GPIOA->MODER |= (1 << (5 * 2)) | (1 << (6 * 2)) | (1 << (7 * 2)) | (1 << (8 * 2));
    // Tắt toàn bộ các LED khi khởi động
    GPIOA->ODR &= ~((1 << 5) | (1 << 6) | (1 << 7) | (1 << 8));
}

void LED_SetBlue(void) {
    GPIOA->ODR |= (1 << 7);    // Bật LED xanh dương (PA7)
    GPIOA->ODR &= ~((1 << 5) | (1 << 6) | (1 << 8)); // Tắt đỏ, vàng, xanh lá
}

void LED_SetYellow(void) {
    GPIOA->ODR |= (1 << 6);    // Bật LED vàng (PA6)
    GPIOA->ODR &= ~((1 << 5) | (1 << 7) | (1 << 8)); // Tắt đỏ, xanh dương, xanh lá
}

void LED_SetGreen(void) {
    GPIOA->ODR |= (1 << 8);    // Bật LED xanh lá (PA8)
    GPIOA->ODR &= ~((1 << 5) | (1 << 6) | (1 << 7)); // Tắt các LED còn lại
}

void LED_BlinkRed(float freq_hz, int duration_ms) {
    int total_blinks = (int)(freq_hz * duration_ms / 1000.0f);
    int delay_half = (int)(500.0f / freq_hz); // ms mỗi nửa chu kỳ

    // Tắt các LED khác
    GPIOA->ODR &= ~((1 << 6) | (1 << 7) | (1 << 8));

    for (int i = 0; i < total_blinks; i++) {
        GPIOA->ODR ^= (1 << 5); // Đảo trạng thái led đỏ (PA5)
        Delay_ms(delay_half);
        GPIOA->ODR ^= (1 << 5);
        Delay_ms(delay_half);
    }
    GPIOA->ODR &= ~(1 << 5); // Đảm bảo tắt led đỏ khi xong
}

// Hàm tính tần số nháy LED đỏ dựa vào ppm khí gas so với ngưỡng nguy hiểm
float Get_BlinkFreq_ByPPM(float ppm, float ppm_danger) {
    // Vượt nguy hiểm: scale từ 2Hz (ngưỡng nguy hiểm) đến 10Hz (ppm rất cao)
    float k = (ppm - ppm_danger) / (ppm_danger);  // 0 ~ tăng dần
    float freq = 2.0f + k * 8.0f;                 // 2Hz đến 10Hz
    if (freq > 10.0f) freq = 10.0f;
    if (freq < 2.0f) freq = 2.0f;
    return freq;
}
