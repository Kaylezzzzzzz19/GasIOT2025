#include "stm32f4xx.h"
#include "mq2.h"
#include "gas_monitor.h"
#include "buzzer.h"
#include "led.h"
#include "relay.h"
#include "lcd_i2c.h"
#include "button.h"
#include "delay.h"
#include "lcd_display.h"
#include "uart.h"
#include <stdio.h>

extern volatile int sw1_flag;
extern volatile int sw2_flag;

char buff[128];

int main(void) {
    SystemState_t sysState = SYSTEM_RUNNING;
    float ppm = 0;
    int stopped_shown = 0; // Đánh dấu đã cập nhật LCD khi vào STOPPED
    int reset_request = 0; // Flag cho biết vừa reset xong

    Delay_Init();
    MQ2_Init();
    Button_Init();
    LED_Init();
    LCD_Init();
    Buzzer_Init();
    UART1_Init();
    Relay_Init();

    // Thông báo hiệu chuẩn trên màn hình LCD
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Dang hieu chuan");  // Hiển thị dòng 1: Đang hiệu chuẩn
    LCD_SetCursor(1, 0);
    LCD_Print("Cam bien MQ-2..."); // Hiển thị dòng 2

    float ro = MQ2_Calibrate();
    int ro_int = (int)ro;
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Hieu chuan xong!");
    LCD_SetCursor(1, 0);
    snprintf(buff, sizeof(buff), "Ro=%d Ohm", ro_int);
    LCD_Print(buff);
    Delay_ms(1500); // Đợi 1.5s để người dùng kịp đọc

    LCD_Clear();

    while (1) {
        // Chuyển đổi RUN/STOP khi nhấn SW1
        if (sw1_flag) {
            sysState = (sysState == SYSTEM_RUNNING) ? SYSTEM_STOPPED : SYSTEM_RUNNING;
            sw1_flag = 0;
            stopped_shown = 0;
        }

        // Xử lý RESET khi nhấn SW2: ppm = 0, báo SAFE
        if (sw2_flag) {
            ppm = 0.0f;
            GasMonitor_UpdateState(sysState, ppm);
            stopped_shown = 0;
            reset_request = 1;
            sw2_flag = 0;
        }

        if (sysState == SYSTEM_RUNNING) {
            if (reset_request) {
                reset_request = 0;
                Delay_ms(150);
                continue;
            }
            stopped_shown = 0;
            uint16_t adc_raw = MQ2_ReadRaw();
            ppm = MQ2_Calculate_PPM(adc_raw);
            GasMonitor_UpdateState(sysState, ppm);
        } else {
            // Nếu ở chế độ STOPPED, chỉ update LCD 1 lần
            if (!stopped_shown || reset_request) {
                GasMonitor_UpdateState(sysState, 0.0f); // Luôn báo SAFE khi STOPPED
                stopped_shown = 1;
                reset_request = 0;
            }
        }
        Delay_ms(100);
    }
}
