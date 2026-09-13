/*
 * sleep_mode.c
 * 
 * ATmega328P Sleep Mode Demonstration
 * 
 * This program demonstrates the ATmega328P sleep functionality:
 * - Normal operation with LED and LCD display
 * - Entry into Power-down sleep mode
 * - Wake-up via external interrupt (INT0)
 * 
 * Hardware Configuration:
 *  - LED: PB0 (Port B, Pin 0)
 *  - Button: PD2 (INT0) for wake-up interrupt
 *  - LCD: Connected via I2C (PCF8574 I/O Expander)
 * 
 * Author: Eyob Bantayehu ( CTC-2790-26 )
 * Date: September, 2026
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd_test.h"
#include <stdint.h>

/* Global flag to track button press for wake-up event */
volatile uint8_t button_flag = 1;

/*
 * ISR(INT0_vect)
 * 
 * External Interrupt Service Routine for INT0 (PD2)
 * Triggered when the button is pressed (falling edge)
 * 
 * Purpose: Set flag to indicate MCU has awakened from sleep mode
 */
ISR(INT0_vect) {
    button_flag = 1;
}

/*
 * INT0_Init()
 * 
 * Initialize external interrupt INT0 for wake-up button
 * 
 * Configuration:
 *  - PD2 configured as input with pull-up enabled
 *  - INT0 triggered on falling edge (ISC01:ISC00 = 00)
 *  - Interrupt enabled and global interrupts enabled
 */
void INT0_Init(void) {
    /* Configure PD2 as input */
    DDRD &= ~(1 << DDD2);
    
    /* Enable internal pull-up resistor on PD2 */
    PORTD |= (1 << PORTD2);

    /* Set interrupt trigger on falling edge (ISC01=0, ISC00=0) */
    EICRA &= ~((1 << ISC01) | (1 << ISC00));
    
    /* Enable external interrupt INT0 */
    EIMSK |= (1 << INT0);
    
    /* Enable global interrupts */
    sei();
}

/*
 * go_to_sleep()
 * 
 * Configures and executes Power-down sleep mode
 * 
 * Behavior:
 *  - Disables ADC to reduce power consumption
 *  - Enables sleep mode (Power-down: SM1=1, SM0=0)
 *  - Executes sleep instruction
 *  - Disables sleep after wake-up
 * 
 * Power-down Mode Effects:
 *  - Stopped: CPU, system clock, I/O clocks, timers
 *  - Active: External interrupts, watchdog (if enabled)
 * 
 * Wake-up: External interrupt (INT0) from button press
 * Execution: Continues from instruction after sleep
 */
void go_to_sleep(void) {
    /* Disable ADC to reduce power consumption in sleep mode */
    ADCSRA &= ~(1 << ADEN);
    
    /* Configure sleep mode: Power-down mode (SM1=1, SM0=0) */
    /* Set Sleep Enable (SE) bit */
    SMCR = (1 << SE) | (1 << SM1);
    
    /* Ensure interrupts are enabled for wake-up */
    sei();
    
    /* Execute sleep instruction */
    __asm__ __volatile__("sleep");
    
    /* Clear Sleep Enable bit after wake-up */
    SMCR &= ~(1 << SE);
}

/*
 * main()
 * 
 * Main program loop
 * 
 * Sequence:
 * 1. Initialize LED (PB0)
 * 2. Configure external interrupt for wake-up button
 * 3. Initialize LCD display
 * 4. Loop:
 *    - If button pressed: Display "LCD ACTIVE", brief delay, then sleep
 *    - Enter sleep mode and wait for interrupt
 *    - On wake-up: Display "WOKE UP", continue loop
 */
int main(void) {
    /* Configure PB0 (LED) as output */
    DDRB |= (1 << PB0);
    
    /* Initialize LED to OFF state */
    PORTB &= ~(1 << PB0);
    
    /* Initialize external interrupt for wake-up */
    INT0_Init();
    
    /* Initialize LCD display */
    LCD_Init();
    LCD_SetCursor(0, 0);
    
    /* Main infinite loop */
    while (1) {
        /* Check if button was pressed (wake-up event) */
        if (button_flag == 1) {
            /* Clear flag for next interrupt */
            button_flag = 0;
            
            /* Turn on LED */
            PORTB |= (1 << PB0);
            
            /* Display active status on LCD */
            LCD_Print("LCD ACTIVE!!");
            delay_ms(500);
            
            /* Clear display and show sleep message */
            LCD_Clear();
            LCD_Print("GOING TO SLEEP!!");
            delay_us(30000);
            
            /* Prepare for sleep */
            LCD_Clear();
            PORTB &= ~(1 << PB0);  /* Turn off LED before sleep */
            LCD_Off();              /* Turn off LCD before sleep */
            LCD_SetCursor(0, 0);
        }
        
        /* Enter sleep mode and wait for wake-up interrupt */
        go_to_sleep();
        
        /* Execution resumes here after wake-up interrupt */
        LCD_On();
        LCD_SetCursor(0, 0);
        LCD_Print("WOKE UP!!");
        delay_us(500);
        LCD_Clear();
    }
    
    return 0;  /* Never reached */
}
