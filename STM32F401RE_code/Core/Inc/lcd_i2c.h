#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include <stdint.h>

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char* str);

#endif
