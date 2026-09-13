#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd_test.h"
#include <stdint.h>

volatile uint8_t button_flag = 1;
ISR(INT0_vect){
    button_flag = 1;
}

void INT0_Init(void) {
    DDRD &= ~(1 << DDD2);
    PORTD |= (1 << PORTD2);

    EICRA &= ~((1 << ISC01) | (1 << ISC00));
    EIMSK |= (1 << INT0);
    sei();
}
void go_to_sleep() {
    ADCSRA &= ~(1 << ADEN);
    SMCR = (1 << SE) | (1 << SM1);
    sei();
    __asm__ __volatile__("sleep");
    SMCR &= ~(1 << SE);
}

int main(void){
    DDRB |= (1 << PB0);
    PORTB &= ~(1 << PB0);
    INT0_Init();
    LCD_Init();
    LCD_SetCursor(0, 0);
    
    while(1){
        if (button_flag == 1){
            button_flag = 0;
            PORTB |= (1 << PB0);
            LCD_Print("LCD ACTIVE!!");
            delay_ms(500);
            LCD_Clear();
            LCD_Print("GOING TO SLEEP!!");
            delay_us(30000);
            LCD_Clear();
            PORTB &= ~(1 << PB0);
            LCD_Off();
            LCD_SetCursor(0, 0);
        }
        
            
    
        go_to_sleep();
        LCD_On();
        LCD_SetCursor(0, 0);
        LCD_Print("WOKE UP!!");
        delay_us(500);
        LCD_Clear();
        
    }
}

