/*
 *   
 *      Program uses a ATMega328p MCU to query a HC-SR04 ultrasonic module. 
 *      Pin placement of ATMega328p:
 *      Pin PC4				HC-SR04 Trig
 *      Pin PC5				HC-SR04 Echo
 */                             
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*******************************************INITALIZE PORTS, TIMER, AND INTURRUPTS*******************************************/
void init() {

	DDRC |= (1<<DDC4);							// Port C all output. PC0: RW		PC1: RS		PC2: E;
	DDRC &= ~(1<<DDC5);						// Set Pin C5 as input to read Echo
	PORTC |= (1<<PORTC5);					// Enable pull up on C5
	PORTC &= ~(1<<PC4);						// Init C4 as low (trigger)

	PRR &= ~(1<<PRTIM1);			// To activate timer1 module
	TCNT1 = 0;								// Initial timer value
	TCCR1B |= (1<<CS10);			// Timer without prescaller. Since default clock for atmega328p is 1Mhz period is 1uS
	TCCR1B |= (1<<ICES1);			// First capture on rising edge

	PCICR = (1<<PCIE1);						// Enable PCINT[14:8] we use pin C5 which is PCINT13
	PCMSK1 = (1<<PCINT13);					// Enable C5 interrupt
	sei();									// Enable Global Interrupts
}

/*******************************************MAIN PROGRAM*******************************************/
int main() {
	init();
	while (1) {;
		_delay_ms(60); 							// To allow sufficient time between queries (60ms min)
		PORTC |= (1<<PC4);						// Set trigger high
		_delay_us(15);							// for 10uS
		PORTC &= ~(1<<PC4);						// to trigger the ultrasonic module
	}
}
/*******************************************INTURRUPT PCINT1 FOR PIN C5*******************************************/
ISR(PCINT1_vect) {
	if (bit_is_set(PINC,PC5)) {	// Checks if echo is high
		TCNT1 = 0;								// Reset Timer
	} else {
		uint16_t numuS = TCNT1;					// Save Timer value
		uint8_t oldSREG = SREG;
		cli();
        long distancia = numuS/58;
		SREG = oldSREG;							// Enable interrupts
        if (distancia < 100){
            PORTD = 0xFF;
        } else {
            PORTD = 0x00;
        }
	}
}
