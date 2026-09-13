/*
 * lcd_test.h
 * 
 * LCD Display and I2C Communication Header
 * 
 * This header defines macros, constants, and function prototypes for:
 * - I2C communication with PCF8574 I/O Expander
 * - LCD control and display operations
 * - Timing delays
 * 
 * Hardware Configuration:
 *  - MCU: ATmega328P at 16MHz
 *  - I2C: For communication with PCF8574
 *  - PCF8574: I/O Expander at address 0x20
 *  - LCD: 16x2 LCD in 4-bit mode
 * 
 * Author: Student
 * Date: 2026
 */

#ifndef LCD_TEST_H_
#define LCD_TEST_H_

/* CPU frequency for delay calculations */
#define F_CPU 16000000UL

/* Standard AVR headers */
#include <avr/io.h>
#include <util/delay.h>

/* PCF8574 I/O Expander I2C address */
#define PCF8574_ADDR 0x20

/* LCD control line bit positions on PCF8574 output */
#define LCD_RS        (1 << 0)  /* Register Select: 0=Command, 1=Data */
#define LCD_RW        (1 << 1)  /* Read/Write: 0=Write, 1=Read (not used) */
#define LCD_EN        (1 << 2)  /* Enable: Latches data on falling edge */
#define LCD_BACKLIGHT (1 << 3)  /* Backlight: 1=On, 0=Off */

/*
 * Timing Functions
 * 
 * These functions provide blocking delays for timing-critical operations
 */
void delay_ms(uint16_t ms);    /* Delay in milliseconds */
void delay_us(uint16_t us);    /* Delay in microseconds */

/*
 * I2C Communication Functions
 * 
 * Low-level TWI (I2C) communication primitives
 */
void I2C_init(void);           /* Initialize I2C peripheral */
void I2C_start(void);          /* Generate START condition */
void I2C_stop(void);           /* Generate STOP condition */
void I2C_write(uint8_t data);  /* Transmit one byte on I2C bus */

/*
 * PCF8574 I/O Expander Functions
 * 
 * Interface for sending data to the PCF8574 I/O expander via I2C
 */
void PCF8574_SendByte(uint8_t data);  /* Send byte to PCF8574 */

/*
 * LCD Control Functions
 * 
 * Low-level LCD control operations
 */
void LCD_PulseEnable(uint8_t data);           /* Pulse LCD Enable line */
void LCD_SendNibble(uint8_t nibble, uint8_t mode);  /* Send 4-bit nibble */
void LCD_SendCmd(uint8_t cmd);                /* Send command to LCD */
void LCD_SendData(uint8_t data);              /* Send data/character to LCD */

/*
 * LCD User-Level Functions
 * 
 * High-level LCD display operations
 */
void LCD_Init(void);                     /* Initialize LCD display */
void LCD_On(void);                       /* Turn on LCD display */
void LCD_Off(void);                      /* Turn off LCD display */
void LCD_Clear(void);                    /* Clear display and home cursor */
void LCD_SetCursor(uint8_t row, uint8_t col);  /* Set cursor position */
void LCD_Print(const char* str);         /* Print string on LCD */

#endif /* LCD_TEST_H_ */
