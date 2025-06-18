#include "gas_monitor.h"
#include "led.h"
#include "buzzer.h"
#include "relay.h"
#include "lcd_display.h"
#include "uart.h"

GasAlertLevel_t GasMonitor_GetAlertLevel(float ppm) {
    if (ppm < 100) return GAS_SAFE;
    else if (ppm < 300) return GAS_LOW;
    else if (ppm < 500) return GAS_HIGH;
    else return GAS_DANGEROUS;
}

void GasMonitor_UpdateState(SystemState_t sysState, float ppm) {
    GasAlertLevel_t alertLevel = GasMonitor_GetAlertLevel(ppm);

    if (sysState == SYSTEM_STOPPED) {
        LED_SetGreen();
        Buzzer_Off();
        Relay_On();
        LCD_DisplayStatus(sysState, alertLevel, ppm);
        return;
    }

    switch (alertLevel) {
        case GAS_SAFE:
            LED_SetBlue();
            Buzzer_Off();
            Relay_On();
            break;
        case GAS_LOW:
            LED_SetYellow();
            Buzzer_Off();
            Relay_On();
            break;
        case GAS_HIGH:
            LED_BlinkRed(1.0f, 1000); // 1Hz
            Buzzer_Off();
            Relay_On();
            break;
        case GAS_DANGEROUS:
        	float freq_now = Get_BlinkFreq_ByPPM(ppm,  500);
            LED_BlinkRed(freq_now, 1000);
            Buzzer_On();
            Relay_Off();
            Send_Alert_ToESP(ppm);
            break;
        default:
            LED_SetBlue();
            Buzzer_Off();
            Relay_On();
            break;
    }

    LCD_DisplayStatus(sysState, alertLevel, ppm);
}
