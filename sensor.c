#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init() {
    DDRD |= (1<<DDD5);
    DDRC |= (1<<DDC4);
	DDRC &= ~(1<<DDC5);
	PORTC |= (1<<PORTC5);
	PORTC &= ~(1<<PC4);

	PRR &= ~(1<<PRTIM1);
	TCNT1 = 0;								
	TCCR1B |= (1<<CS11);
	TCCR1B |= (1<<ICES1);

	PCICR = (1<<PCIE1);	
	PCMSK1 = (1<<PCINT13);
	sei();
}

int main() {
	init();
	while (1) {;
		_delay_ms(60);						
		PORTC |= (1<<PC4);
		_delay_us(10);					
		PORTC &= ~(1<<PC4);	
	}
}
ISR(PCINT1_vect) {
	if (bit_is_set(PINC,PC5)) {
		TCNT1 = 0;		
	} else {
		long numuS = TCNT1/2;				
		uint8_t oldSREG = SREG;
		cli();
        long distancia = numuS/58;
		SREG = oldSREG;						
        if (distancia < 30){
            PORTD |= (1 << PORTD5);
        } else {
            PORTD &= ~(1 << PORTD5);
        }
	}
}
