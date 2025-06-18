#include "mq2.h"
#include "stm32f4xx.h"
#include <math.h>
#include "delay.h"

#define MQ2_RL_VALUE        1000.0f      // Điện trở tải kΩ
#define MQ2_RO_CLEAN_AIR    9.8f      // Hệ số Ro không khí sạch theo datasheet
#define MQ2_PPM_A           574.25f
#define MQ2_PPM_B           -2.222

static float MQ2_Ro = 10.0f;     // Giá trị Ro, sẽ hiệu chuẩn

void MQ2_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    GPIOA->MODER |= (3 << (0 * 2));
    GPIOA->PUPDR &= ~(3 << (0 * 2));
    ADC1->CR2 = 0;
    ADC1->SQR1 = 0;
    ADC1->SQR3 = 0;
    ADC1->SMPR2 &= ~(7 << 0);
    ADC1->SMPR2 |= 5 << 0;
    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t MQ2_ReadRaw(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}

static float MQ2_Calculate_Rs(uint16_t adc_val) {
    float v_adc = ((float)adc_val / 4095.0f) * 3.3f;
    if (v_adc < 0.001f) v_adc = 0.001f;
    return MQ2_RL_VALUE * (5.0f - v_adc) / v_adc;
}

float MQ2_Calibrate(void) {
    float rs_sum = 0.0f;
    for (int i = 0; i < 50; i++) {
        rs_sum += MQ2_Calculate_Rs(MQ2_ReadRaw());
        Delay_ms(200);
    }
    MQ2_Ro = rs_sum / 50.0f / MQ2_RO_CLEAN_AIR;
    return MQ2_Ro;
}

float MQ2_ReadRs(void) {
    float rs_sum = 0.0f;
    for (int i = 0; i < 10; i++) {
        rs_sum += MQ2_Calculate_Rs(MQ2_ReadRaw());
        Delay_ms(10);
    }
    return rs_sum / 10.0f;
}

float MQ2_Calculate_PPM(uint16_t adc_val) {
    if (MQ2_Ro < 0.01f) return 0.0f;
    float rs = MQ2_Calculate_Rs(adc_val);
    float ratio = rs / MQ2_Ro;
    if (ratio <= 0.0f) return 0.0f;
    float ppm = MQ2_PPM_A * powf(ratio, MQ2_PPM_B);
    return (ppm < 0.0f) ? 0.0f : ppm;
}
