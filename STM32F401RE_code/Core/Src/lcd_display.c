#include "lcd_display.h"
#include "lcd_i2c.h"
#include <stdio.h>

void LCD_DisplayStatus(SystemState_t sysState, GasAlertLevel_t alertLevel, float ppm) {
    static int last_ppm = -1;
    static SystemState_t last_sysState = -1;
    static GasAlertLevel_t last_alertLevel = -1;

    int ppm_int = (int)(ppm + 0.5f);

    // Chỉ cập nhật khi có thay đổi dữ liệu
    if (ppm_int != last_ppm || sysState != last_sysState || alertLevel != last_alertLevel) {
        char line1[17], line2[17];

        sprintf(line1, "Sys: %d %s",
            (sysState == SYSTEM_RUNNING) ? 1 : 0,
            (sysState == SYSTEM_RUNNING) ? "RUN " : "STOP");

        const char* alertStr = "Safe";
        switch (alertLevel) {
            case GAS_LOW:        alertStr = "Low ";     break;
            case GAS_HIGH:       alertStr = "High";     break;
            case GAS_DANGEROUS:  alertStr = "DANGER";   break;
            default:             alertStr = "Safe";     break;
        }
        sprintf(line2, "%s PPM: %d      ", alertStr, ppm_int); // Thêm khoảng trắng xóa ký tự thừa nếu có

        LCD_SetCursor(0, 0);
        LCD_Print(line1);
        LCD_SetCursor(1, 0);
        LCD_Print(line2);

        // Lưu lại dữ liệu cũ
        last_ppm = ppm_int;
        last_sysState = sysState;
        last_alertLevel = alertLevel;
    }
}
