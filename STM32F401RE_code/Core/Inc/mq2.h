#ifndef __MQ2_H
#define __MQ2_H

#include <stdint.h>

// Khởi tạo phần cứng cho cảm biến MQ-2 (bật ADC, cấu hình chân)
void MQ2_Init(void);

// Hiệu chuẩn cảm biến MQ-2 ngoài không khí sạch, trả về giá trị Ro
float MQ2_Calibrate(void);

// Đọc giá trị ADC thô từ cảm biến MQ-2
uint16_t MQ2_ReadRaw(void);

// Đọc trung bình điện trở Rs từ MQ-2 (ít dùng, chủ yếu để debug/test)
float MQ2_ReadRs(void);

// Tính toán và trả về nồng độ khí gas (ppm) từ giá trị ADC
float MQ2_Calculate_PPM(uint16_t adc_value);

#endif // __MQ2_H
