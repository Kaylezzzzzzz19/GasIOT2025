#ifndef __LCD_DISPLAY_H
#define __LCD_DISPLAY_H

#include "system_state.h"
#include "gas_monitor.h"

void LCD_DisplayStatus(SystemState_t sysState, GasAlertLevel_t alertLevel, float ppm);

#endif
