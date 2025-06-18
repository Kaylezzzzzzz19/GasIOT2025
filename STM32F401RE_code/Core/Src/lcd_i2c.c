#include "lcd_i2c.h"
#include <stdint.h>
#include "stm32f401xe.h"

#define LCD_I2C_ADDR 0x27
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RW        0x02
#define LCD_RS        0x01

static void delay_ms(uint32_t ms) {
    volatile uint32_t i;
    while(ms--) {
        for(i=0; i<4000; i++) {
            __NOP();
        }
    }
}

void I2C1_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB->MODER |= (2 << (8 * 2)) | (2 << (9 * 2));
    GPIOB->OTYPER |= (1 << 8) | (1 << 9);
    GPIOB->PUPDR &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB->PUPDR |= (1 << (8 * 2)) | (1 << (9 * 2));
    GPIOB->OSPEEDR |= (3 << (8 * 2)) | (3 << (9 * 2));
    GPIOB->AFR[1] &= ~((0xF << ((8 - 8) * 4)) | (0xF << ((9 - 8) * 4)));
    GPIOB->AFR[1] |= (4 << ((8 - 8) * 4)) | (4 << ((9 - 8) * 4));
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    I2C1->CR2 = 16;
    I2C1->CCR = 80;
    I2C1->TRISE = 17;
    I2C1->CR1 |= I2C_CR1_PE;
}

int I2C1_WriteByte(uint8_t dev_addr, uint8_t data) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = (dev_addr << 1) | 0;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->CR1 |= I2C_CR1_STOP;
    return 0;
}

static void LCD_ExpanderWrite(uint8_t data);
static void LCD_PulseEnable(uint8_t data);
static void LCD_Write4Bits(uint8_t data);
static void LCD_Send(uint8_t value, uint8_t mode);
static void LCD_SendCommand(uint8_t cmd);
static void LCD_SendData(uint8_t data);

void LCD_Init(void) {
    I2C1_Init();
    delay_ms(50);
    LCD_Write4Bits(0x30);
    delay_ms(5);
    LCD_Write4Bits(0x30);
    delay_ms(1);
    LCD_Write4Bits(0x30);
    delay_ms(10);
    LCD_Write4Bits(0x20);
    LCD_SendCommand(0x28);
    LCD_SendCommand(0x08);
    LCD_Clear();
    LCD_SendCommand(0x06);
    LCD_SendCommand(0x0C);
}

void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    delay_ms(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    const uint8_t row_offsets[] = {0x00, 0x40};
    if (row > 1) row = 1;
    LCD_SendCommand(0x80 | (col + row_offsets[row]));
}

void LCD_Print(char* str) {
    while(*str) {
        LCD_SendData((uint8_t)(*str));
        str++;
    }
}

static void LCD_SendCommand(uint8_t cmd) {
    LCD_Send(cmd, 0);
}

static void LCD_SendData(uint8_t data) {
    LCD_Send(data, LCD_RS);
}

static void LCD_Send(uint8_t value, uint8_t mode) {
    uint8_t high_nibble = value & 0xF0;
    uint8_t low_nibble = (value << 4) & 0xF0;
    LCD_Write4Bits(high_nibble | mode | LCD_BACKLIGHT);
    LCD_Write4Bits(low_nibble | mode | LCD_BACKLIGHT);
}

static void LCD_Write4Bits(uint8_t data) {
    LCD_ExpanderWrite(data);
    LCD_PulseEnable(data);
}

static void LCD_ExpanderWrite(uint8_t data) {
    I2C1_WriteByte(LCD_I2C_ADDR, data);
}

static void LCD_PulseEnable(uint8_t data) {
    LCD_ExpanderWrite(data | LCD_ENABLE);
    delay_ms(1);
    LCD_ExpanderWrite(data & ~LCD_ENABLE);
    delay_ms(1);
}
