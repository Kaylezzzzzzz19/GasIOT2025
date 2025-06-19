#include "lcd_i2c.h"
#include <stdint.h>
#include "stm32f401xe.h"
#include "delay.h"

// Địa chỉ I2C của LCD (có thể khác tuỳ module)
#define LCD_I2C_ADDR 0x27
// Các bit điều khiển LCD
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RW        0x02
#define LCD_RS        0x01

// Khởi tạo I2C1 để giao tiếp với LCD
void I2C1_Init(void) {
    // Bật clock cho port B
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // PB8 - SCL, PB9 - SDA: chế độ alternate function (AF4)
    GPIOB->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB->MODER |= (2 << (8 * 2)) | (2 << (9 * 2));
    // Kiểu output open-drain
    GPIOB->OTYPER |= (1 << 8) | (1 << 9);
    // Kéo lên (pull-up)
    GPIOB->PUPDR &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB->PUPDR |= (1 << (8 * 2)) | (1 << (9 * 2));
    // Tốc độ cao
    GPIOB->OSPEEDR |= (3 << (8 * 2)) | (3 << (9 * 2));
    // Chọn chức năng AF4 cho chân PB8, PB9
    GPIOB->AFR[1] &= ~((0xF << ((8 - 8) * 4)) | (0xF << ((9 - 8) * 4)));
    GPIOB->AFR[1] |= (4 << ((8 - 8) * 4)) | (4 << ((9 - 8) * 4));
    // Bật clock cho I2C1
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    // Reset I2C1
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    // Cấu hình tốc độ cho I2C1 (chuẩn 100kHz, clock 16MHz)
    I2C1->CR2 = 16;
    I2C1->CCR = 80;
    I2C1->TRISE = 17;
    // Cho phép I2C1 hoạt động
    I2C1->CR1 |= I2C_CR1_PE;
}

// Gửi 1 byte dữ liệu qua I2C1 đến thiết bị có địa chỉ dev_addr
int I2C1_WriteByte(uint8_t dev_addr, uint8_t data) {
    I2C1->CR1 |= I2C_CR1_START;                 // Gửi tín hiệu START
    while (!(I2C1->SR1 & I2C_SR1_SB));          // Chờ bit START được set
    I2C1->DR = (dev_addr << 1) | 0;             // Gửi địa chỉ thiết bị + bit ghi (0)
    while (!(I2C1->SR1 & I2C_SR1_ADDR));        // Chờ phản hồi địa chỉ
    (void)I2C1->SR2;                            // Đọc SR2 để thoát khỏi trạng thái BUSY
    I2C1->DR = data;                            // Gửi dữ liệu
    while (!(I2C1->SR1 & I2C_SR1_TXE));         // Chờ gửi xong
    I2C1->CR1 |= I2C_CR1_STOP;                  // Gửi tín hiệu STOP
    return 0;
}

// Khai báo trước các hàm static nội bộ
static void LCD_ExpanderWrite(uint8_t data);
static void LCD_PulseEnable(uint8_t data);
static void LCD_Write4Bits(uint8_t data);
static void LCD_Send(uint8_t value, uint8_t mode);
static void LCD_SendCommand(uint8_t cmd);
static void LCD_SendData(uint8_t data);

// Hàm khởi tạo LCD (giao tiếp I2C)
void LCD_Init(void) {
    I2C1_Init();         // Khởi tạo I2C1
    Delay_ms(50);        // Đợi LCD sẵn sàng
    LCD_Write4Bits(0x30); Delay_ms(5);
    LCD_Write4Bits(0x30); Delay_ms(1);
    LCD_Write4Bits(0x30); Delay_ms(10);
    LCD_Write4Bits(0x20); // Chuyển về chế độ 4-bit

    LCD_SendCommand(0x28); // Giao diện 2 dòng, font 5x8
    LCD_SendCommand(0x08); // Tắt hiển thị
    LCD_Clear();           // Xoá màn hình
    LCD_SendCommand(0x06); // Tự động tăng con trỏ
    LCD_SendCommand(0x0C); // Bật hiển thị, tắt con trỏ
}

// Xoá màn hình LCD
void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    Delay_ms(2); // Cần delay vì lệnh này mất thời gian
}

// Đặt vị trí con trỏ (row: dòng, col: cột)
void LCD_SetCursor(uint8_t row, uint8_t col) {
    const uint8_t row_offsets[] = {0x00, 0x40};
    if (row > 1) row = 1;
    LCD_SendCommand(0x80 | (col + row_offsets[row]));
}

// In chuỗi ký tự ra LCD
void LCD_Print(char* str) {
    while(*str) {
        LCD_SendData((uint8_t)(*str));
        str++;
    }
}

// Gửi lệnh điều khiển tới LCD
static void LCD_SendCommand(uint8_t cmd) {
    LCD_Send(cmd, 0);
}

// Gửi dữ liệu hiển thị tới LCD
static void LCD_SendData(uint8_t data) {
    LCD_Send(data, LCD_RS);
}

// Gửi giá trị 8-bit (2 lần 4-bit) tới LCD, mode=0: lệnh, mode=LCD_RS: dữ liệu
static void LCD_Send(uint8_t value, uint8_t mode) {
    uint8_t high_nibble = value & 0xF0;
    uint8_t low_nibble = (value << 4) & 0xF0;
    LCD_Write4Bits(high_nibble | mode | LCD_BACKLIGHT);
    LCD_Write4Bits(low_nibble | mode | LCD_BACKLIGHT);
}

// Gửi 4-bit dữ liệu tới LCD
static void LCD_Write4Bits(uint8_t data) {
    LCD_ExpanderWrite(data);   // Gửi dữ liệu ra expander
    LCD_PulseEnable(data);     // Tạo xung enable
}

// Gửi dữ liệu tới chip mở rộng I2C LCD
static void LCD_ExpanderWrite(uint8_t data) {
    I2C1_WriteByte(LCD_I2C_ADDR, data);
}

// Tạo xung ENABLE cho LCD để nhận dữ liệu
static void LCD_PulseEnable(uint8_t data) {
    LCD_ExpanderWrite(data | LCD_ENABLE);
    Delay_ms(1);
    LCD_ExpanderWrite(data & ~LCD_ENABLE);
    Delay_ms(1);
}
