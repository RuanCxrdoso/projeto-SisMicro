/*
 * File:   pwm.c
 * Author: kelmerpassos
 *
 * Created on November 15 , 2023, 9:13 PM
 */

#include <avr/io.h>
#define ENA 5 
#define ENB 6

int main(void) {
    
    DDRD |= (1 << ENA) | (1 << ENB);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00);
    OCR0A = 51;
    OCR0B = 51;
    while (1) {
        
    }
}
