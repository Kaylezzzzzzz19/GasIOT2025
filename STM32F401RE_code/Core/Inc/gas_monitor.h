#ifndef __GAS_MONITOR_H
#define __GAS_MONITOR_H

#include "system_state.h"

// Các mức cảnh báo khí gas
// Hàm xác định mức cảnh báo dựa trên giá trị ppm
GasAlertLevel_t GasMonitor_GetAlertLevel(float ppm);

// Hàm cập nhật trạng thái hệ thống và điều khiển LED, còi, relay, LCD
void GasMonitor_UpdateState(SystemState_t sysState, float ppm);

#endif
