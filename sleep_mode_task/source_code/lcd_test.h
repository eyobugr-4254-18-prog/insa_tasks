#ifndef LCD_TEST_H_
#define LCD_TEST_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define PCF8574_ADDR 0x20

#define LCD_RS (1 << 0)
#define LCD_RW (1 << 1)
#define LCD_EN (1 << 2)
#define LCD_BACKLIGHT (1 << 3)
void delay_ms(uint16_t ms);
void delay_us(uint16_t us);
void I2C_init(void);
void I2C_start(void);
void I2C_stop(void);
void I2C_write(uint8_t data);
void PCF8574_SendByte(uint8_t data);
void LCD_PulseEnable(uint8_t data); 
void LCD_SendNibble(uint8_t nibble, uint8_t mode); 
void LCD_SendCmd(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(void);
void LCD_On(void);
void LCD_Off(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(const char* str);

#endif /* LCD_TEST_H_ */