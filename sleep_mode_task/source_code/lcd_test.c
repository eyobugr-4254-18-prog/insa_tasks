#include "lcd_test.h"
void delay_ms(uint16_t ms){
    TCCR0A = 0x00;
    
    while(ms--){
        TCNT0 = 256 - 250;
        TIFR0 |= (1 << TOV0);
        TCCR0B |= (1 << CS00) | (1 << CS01);
        while(!(TIFR0 & (1 << TOV0)));
        TCCR0B = 0x00;
    }
}
void delay_us(uint16_t us){
    TCCR0A = 0x00;
    
    while(us >= 2){
        uint8_t ticks = (us > 120) ? 240 : us * 2;
        us -= (ticks / 2);
        TCNT0 = 256 - ticks;
        TIFR0 |= (1 << TOV0);
        TCCR0B |= (1 << CS00) | (1 << CS01);
        while(!(TIFR0 & (1 << TOV0)));
        TCCR0B = 0x00;
    }
}
void I2C_init(void) {
    TWBR = 72;
    TWSR = 0x00;
    TWCR = (1 << TWEN);
}
void I2C_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while(!(TWCR & (1 << TWINT)));
}
void I2C_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}
void I2C_write(uint8_t data) {
    TWDR  = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while(!(TWCR & (1 << TWINT)));
}

void PCF8574_SendByte(uint8_t data) {
    I2C_start();
    I2C_write(PCF8574_ADDR << 1);
    I2C_write(data);
    I2C_stop();

}
void LCD_PulseEnable(uint8_t data) {
    PCF8574_SendByte(data | LCD_EN);
    delay_us(1);
    PCF8574_SendByte(data & ~LCD_EN);
    delay_us(10);

}
void LCD_SendNibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = (nibble & 0xF0) | mode | LCD_BACKLIGHT;
    PCF8574_SendByte(data);
    LCD_PulseEnable(data);

}
void LCD_SendCmd(uint8_t cmd){
    LCD_SendNibble(cmd & 0xF0, 0);
    LCD_SendNibble((cmd << 4) & 0xF0, 0);

}

void LCD_SendData(uint8_t data) {
    LCD_SendNibble(data & 0xF0, LCD_RS);
    LCD_SendNibble((data << 4) & 0xF0, LCD_RS);


}
void LCD_Init(void) {
    I2C_init();
    delay_ms(50);
    LCD_SendNibble(0X30, 0);
    delay_ms(5);
    LCD_SendNibble(0X30, 0);
    delay_us(150);
    LCD_SendNibble(0X30, 0);
    LCD_SendNibble(0X20, 0);
    LCD_SendCmd(0x28);
    LCD_SendCmd(0x0C);
    LCD_SendCmd(0x01);
    delay_ms(2);
    LCD_SendCmd(0x06);
}
void LCD_On(void){
    LCD_SendCmd(0x0C);
    delay_ms(2);

}
void LCD_Off(void){
    LCD_SendCmd(0x08);
    delay_ms(2);

}
void LCD_Clear(void){
    LCD_SendCmd(0x01);
    delay_us(20);

}


void LCD_SetCursor(uint8_t row, uint8_t col){
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCmd(address);
}
void LCD_Print(const char* str) {
    while(*str) {
        LCD_SendData((uint8_t)(*str++));
    }
}

