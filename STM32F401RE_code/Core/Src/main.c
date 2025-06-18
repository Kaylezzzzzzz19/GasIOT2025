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
    int prev_sysState = SYSTEM_RUNNING; // lưu trạng thái trước đó

    Delay_Init();
    MQ2_Init();
    Button_Init();
    LED_Init();
    LCD_Init();
    Buzzer_Init();
    UART1_Init();
    Relay_Init();

    // Hiệu chuẩn cảm biến MQ2 trước khi vào vòng lặp chính
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Dang hieu chuan");
    LCD_SetCursor(1, 0);
    LCD_Print("Cam bien MQ-2...");

    float ro = MQ2_Calibrate();
    int ro_int = (int)ro;
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Hieu chuan xong!");
    LCD_SetCursor(1, 0);
    snprintf(buff, sizeof(buff), "Ro=%d Ohm", ro_int);
    LCD_Print(buff);
    Delay_ms(1500);
    LCD_Clear();

    while (1) {
        // Xử lý chuyển đổi RUN/STOP
        if (sw1_flag) {
            prev_sysState = sysState;
            sysState = (sysState == SYSTEM_RUNNING) ? SYSTEM_STOPPED : SYSTEM_RUNNING;
            sw1_flag = 0;

            // Nếu chuyển từ RUNNING sang STOPPED, cập nhật trạng thái lên LCD, giữ nguyên ppm cũ
            if (prev_sysState == SYSTEM_RUNNING && sysState == SYSTEM_STOPPED) {
                GasMonitor_UpdateState(sysState, ppm);
            }
            // Nếu chuyển từ STOPPED sang RUNNING, không cập nhật LCD tại đây (vòng lặp RUN sẽ update)
        }

        // Xử lý RESET: luôn về ppm=0, LCD báo "SAFE" (ppm=0), giữ nguyên trạng thái hệ thống
        if (sw2_flag) {
            ppm = 0.0f;
            GasMonitor_UpdateState(sysState, ppm); // Hiển thị ppm=0, trạng thái safe
            sw2_flag = 0;
        }

        if (sysState == SYSTEM_RUNNING) {
            uint16_t adc_raw = MQ2_ReadRaw();
            ppm = MQ2_Calculate_PPM(adc_raw);
            GasMonitor_UpdateState(sysState, ppm);
            Delay_ms(100); // delay 100ms giữa các lần đọc
        } else {
            // Ở STOPPED: Không cập nhật LCD liên tục, chỉ update khi chuyển trạng thái hoặc reset (xử lý ở trên)
            Delay_ms(100);
        }
    }
}
