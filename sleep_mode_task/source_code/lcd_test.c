/*
 * lcd_test.c
 * 
 * LCD Display and I2C Communication Driver
 * 
 * This module provides functions for:
 * - Timing delays (milliseconds and microseconds)
 * - I2C communication protocol
 * - LCD control via PCF8574 I/O Expander
 * - LCD initialization and display operations
 * 
 * Hardware:
 *  - I2C connected to AVR (SCL, SDA)
 *  - PCF8574 I/O Expander at address 0x20
 *  - 16x2 LCD display in 4-bit mode
 * 
 * Author: Eyob Bantayehu ( CTC-2790-26 )
 * Date: September, 2026
 */

#include "lcd_test.h"

/*
 * delay_ms(uint16_t ms)
 * 
 * Blocking delay in milliseconds using Timer0
 * 
 * Parameters:
 *  - ms: Number of milliseconds to delay
 * 
 * Implementation:
 *  - Uses Timer0 with prescaler (CS00=1, CS01=1)
 *  - Counts 250 cycles per millisecond at 16MHz
 *  - Blocking implementation (MCU does nothing)
 */
void delay_ms(uint16_t ms) {
    /* Reset Timer0 Control Register A */
    TCCR0A = 0x00;
    
    /* Loop for each millisecond */
    while (ms--) {
        /* Preload counter: 256 - 250 = 6 (250 cycles per ms) */
        TCNT0 = 256 - 250;
        
        /* Clear overflow flag */
        TIFR0 |= (1 << TOV0);
        
        /* Start timer with prescaler 64 (CS00=1, CS01=1) */
        TCCR0B |= (1 << CS00) | (1 << CS01);
        
        /* Wait for overflow flag to set */
        while (!(TIFR0 & (1 << TOV0)));
        
        /* Stop timer */
        TCCR0B = 0x00;
    }
}

/*
 * delay_us(uint16_t us)
 * 
 * Blocking delay in microseconds using Timer0
 * 
 * Parameters:
 *  - us: Number of microseconds to delay
 * 
 * Implementation:
 *  - Uses Timer0 with prescaler
 *  - Dynamically adjusts tick count based on requested delay
 *  - Blocking implementation
 */
void delay_us(uint16_t us) {
    /* Reset Timer0 Control Register A */
    TCCR0A = 0x00;
    
    /* Loop until delay complete */
    while (us >= 2) {
        /* Calculate number of ticks needed (max 240 ticks = 120 us) */
        uint8_t ticks = (us > 120) ? 240 : us * 2;
        
        /* Subtract completed delay from remaining */
        us -= (ticks / 2);
        
        /* Preload timer counter */
        TCNT0 = 256 - ticks;
        
        /* Clear overflow flag */
        TIFR0 |= (1 << TOV0);
        
        /* Start timer with prescaler 64 */
        TCCR0B |= (1 << CS00) | (1 << CS01);
        
        /* Wait for overflow */
        while (!(TIFR0 & (1 << TOV0)));
        
        /* Stop timer */
        TCCR0B = 0x00;
    }
}

/*
 * I2C_init()
 * 
 * Initialize I2C (TWI) communication
 * 
 * Configuration:
 *  - Baud rate set via TWBR = 72 (for 16MHz, ~100kHz clock)
 *  - Prescaler TWSR = 0x00 (1:1)
 *  - TWI enabled
 */
void I2C_init(void) {
    /* Set bit rate: TWBR = 72 gives ~100kHz at 16MHz */
    TWBR = 72;
    
    /* Set prescaler to 1:1 */
    TWSR = 0x00;
    
    /* Enable TWI peripheral */
    TWCR = (1 << TWEN);
}

/*
 * I2C_start()
 * 
 * Generate START condition on I2C bus
 * 
 * Behavior:
 *  - Sends START condition
 *  - Waits for TWINT flag (transfer complete)
 */
void I2C_start(void) {
    /* Generate START condition and wait for completion */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    /* Wait for START condition to complete */
    while (!(TWCR & (1 << TWINT)));
}

/*
 * I2C_stop()
 * 
 * Generate STOP condition on I2C bus
 * 
 * Behavior:
 *  - Sends STOP condition
 *  - Releases bus lines
 */
void I2C_stop(void) {
    /* Generate STOP condition */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/*
 * I2C_write(uint8_t data)
 * 
 * Transmit one byte on I2C bus
 * 
 * Parameters:
 *  - data: Byte to transmit
 * 
 * Behavior:
 *  - Loads data into TWDR register
 *  - Waits for transmission to complete
 */
void I2C_write(uint8_t data) {
    /* Load data into TWI Data Register */
    TWDR = data;
    
    /* Initiate transmission */
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    /* Wait for transmission to complete */
    while (!(TWCR & (1 << TWINT)));
}

/*
 * PCF8574_SendByte(uint8_t data)
 * 
 * Send one byte to PCF8574 I/O Expander via I2C
 * 
 * Parameters:
 *  - data: Byte to send (8 bits for LCD control)
 * 
 * I2C Transaction:
 *  1. START condition
 *  2. Send slave address (0x20 << 1 = 0x40 for write)
 *  3. Send data byte
 *  4. STOP condition
 */
void PCF8574_SendByte(uint8_t data) {
    /* Generate I2C START condition */
    I2C_start();
    
    /* Send PCF8574 address with write bit (address << 1) */
    I2C_write(PCF8574_ADDR << 1);
    
    /* Send data byte to I/O expander */
    I2C_write(data);
    
    /* Generate I2C STOP condition */
    I2C_stop();
}

/*
 * LCD_PulseEnable(uint8_t data)
 * 
 * Pulse LCD Enable line (creates rising edge for LCD to latch data)
 * 
 * Parameters:
 *  - data: Current I/O expander output state
 * 
 * Behavior:
 *  - Sets Enable high
 *  - Brief delay
 *  - Sets Enable low
 *  - Brief delay for setup/hold times
 */
void LCD_PulseEnable(uint8_t data) {
    /* Set Enable line high */
    PCF8574_SendByte(data | LCD_EN);
    delay_us(1);
    
    /* Set Enable line low */
    PCF8574_SendByte(data & ~LCD_EN);
    delay_us(10);
}

/*
 * LCD_SendNibble(uint8_t nibble, uint8_t mode)
 * 
 * Send 4-bit nibble to LCD via PCF8574
 * 
 * Parameters:
 *  - nibble: Upper 4 bits contain data to send
 *  - mode: 0 for command (RS=0), LCD_RS for data (RS=1)
 * 
 * Behavior:
 *  - Combines nibble with mode and backlight control
 *  - Sends to I/O expander
 *  - Pulses Enable line to latch data
 */
void LCD_SendNibble(uint8_t nibble, uint8_t mode) {
    /* Combine nibble with mode and backlight */
    uint8_t data = (nibble & 0xF0) | mode | LCD_BACKLIGHT;
    
    /* Send data to I/O expander */
    PCF8574_SendByte(data);
    
    /* Pulse Enable line to latch data into LCD */
    LCD_PulseEnable(data);
}

/*
 * LCD_SendCmd(uint8_t cmd)
 * 
 * Send 8-bit command to LCD in 4-bit mode
 * 
 * Parameters:
 *  - cmd: LCD command byte
 * 
 * Behavior:
 *  - Sends upper 4 bits first
 *  - Sends lower 4 bits second
 *  - RS line = 0 (command mode)
 */
void LCD_SendCmd(uint8_t cmd) {
    /* Send upper nibble */
    LCD_SendNibble(cmd & 0xF0, 0);
    
    /* Send lower nibble (shifted to upper nibble position) */
    LCD_SendNibble((cmd << 4) & 0xF0, 0);
}

/*
 * LCD_SendData(uint8_t data)
 * 
 * Send 8-bit data byte to LCD in 4-bit mode
 * 
 * Parameters:
 *  - data: ASCII character to display
 * 
 * Behavior:
 *  - Sends upper 4 bits first
 *  - Sends lower 4 bits second
 *  - RS line = 1 (data mode)
 */
void LCD_SendData(uint8_t data) {
    /* Send upper nibble with RS=1 (data mode) */
    LCD_SendNibble(data & 0xF0, LCD_RS);
    
    /* Send lower nibble with RS=1 (data mode) */
    LCD_SendNibble((data << 4) & 0xF0, LCD_RS);
}

/*
 * LCD_Init()
 * 
 * Initialize 16x2 LCD in 4-bit mode via PCF8574 I/O Expander
 * 
 * Sequence:
 *  1. Initialize I2C
 *  2. Perform LCD initialization sequence (as per HD44780 datasheet)
 *  3. Configure for 4-bit mode
 *  4. Set display options (display on, cursor off, auto-increment)
 */
void LCD_Init(void) {
    /* Initialize I2C communication */
    I2C_init();
    
    /* Wait for LCD to stabilize */
    delay_ms(50);
    
    /* LCD initialization sequence (4-bit mode) */
    
    /* First initialization nibble: 0x30 */
    LCD_SendNibble(0x30, 0);
    delay_ms(5);
    
    /* Second initialization nibble: 0x30 */
    LCD_SendNibble(0x30, 0);
    delay_us(150);
    
    /* Third initialization nibble: 0x30 */
    LCD_SendNibble(0x30, 0);
    
    /* Switch to 4-bit mode: 0x20 */
    LCD_SendNibble(0x20, 0);
    
    /* Function Set: 0x28 (4-bit mode, 2 lines, 5x8 font) */
    LCD_SendCmd(0x28);
    
    /* Display Control: 0x0C (display on, cursor off, blink off) */
    LCD_SendCmd(0x0C);
    
    /* Clear Display: 0x01 */
    LCD_SendCmd(0x01);
    delay_ms(2);
    
    /* Entry Mode Set: 0x06 (auto-increment, no shift) */
    LCD_SendCmd(0x06);
}

/*
 * LCD_On()
 * 
 * Turn on LCD display and cursor
 * 
 * Behavior:
 *  - Sends Display Control command to enable display
 *  - Brief delay for command execution
 */
void LCD_On(void) {
    /* Display Control: 0x0C (display on, cursor off, blink off) */
    LCD_SendCmd(0x0C);
    delay_ms(2);
}

/*
 * LCD_Off()
 * 
 * Turn off LCD display (reduces power consumption)
 * 
 * Behavior:
 *  - Sends Display Control command to disable display
 *  - Data remains in RAM (not erased)
 *  - Brief delay for command execution
 */
void LCD_Off(void) {
    /* Display Control: 0x08 (display off) */
    LCD_SendCmd(0x08);
    delay_ms(2);
}

/*
 * LCD_Clear()
 * 
 * Clear LCD display and return cursor to home position
 * 
 * Behavior:
 *  - Sends Clear Display command (0x01)
 *  - Clears all data in display RAM
 *  - Sets cursor to row 0, column 0
 */
void LCD_Clear(void) {
    /* Clear Display command: 0x01 */
    LCD_SendCmd(0x01);
    delay_us(20);
}

/*
 * LCD_SetCursor(uint8_t row, uint8_t col)
 * 
 * Set cursor position on LCD display
 * 
 * Parameters:
 *  - row: Row number (0 or 1 for 16x2 display)
 *  - col: Column number (0-15)
 * 
 * Behavior:
 *  - Calculates DDRAM address based on row and column
 *  - Row 0: address = 0x80 + col
 *  - Row 1: address = 0xC0 + col
 */
void LCD_SetCursor(uint8_t row, uint8_t col) {
    /* Calculate DDRAM address for cursor position */
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    
    /* Send Set DDRAM Address command */
    LCD_SendCmd(address);
}

/*
 * LCD_Print(const char* str)
 * 
 * Display a string on LCD starting at current cursor position
 * 
 * Parameters:
 *  - str: Pointer to null-terminated string
 * 
 * Behavior:
 *  - Iterates through string until null terminator
 *  - Sends each character as data to LCD
 */
void LCD_Print(const char* str) {
    /* Iterate through string characters */
    while (*str) {
        /* Send character as data to LCD */
        LCD_SendData((uint8_t)(*str++));
    }
}
