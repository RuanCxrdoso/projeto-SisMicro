/*
 * File:   pwm.c
 * Author: kelmerpassos
 *
 * Created on December 5, 2023, 9:13 PM
 */

#include <avr/io.h>
#define ENA 5 // Pinos de controle de velocidade usando o PWM para motor esquerdo (PB1 - OC1A)
#define ENB 6

int main(void) {
    
    DDRD |= (1 << ENA) | (1 << ENB);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00);
    OCR0A = 128;
    OCR0B = 128;
    while (1) {
        
    }
}
