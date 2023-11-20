/*
	projetoRobo.cpp
 
	Created: 02/11/2023 18:35:41
	Author : Ruan Cardoso, Mikelly Correia, Edilton Damasceno e Kelmer Passos
	Theme: Instrumentation of a differential robot
	Micro controller: ATMega328P
	Clock: 16Mhz
	
 */ 

#include <avr/io.h>
#include <util/delay.h> // lib for delays (we can use the timer maybe)

#define F_CPU 16000000UL // Clock frequency in Hz
#define SOUND_SPEED // Sound speed in m/s

void setup() 
{
	DDRB |= (1 << DDB1) // SETA O SEGUNDO BIT DO PORTB COMO SAIDA (TRIG)
	DDRD &= ~(1 << DDD2) // SETA O TERCEIRO BIT DO PORTD COMO ENTRADA (ECHO)
}

unsigned int measureDistance() // Função para medição de distância do sensor ultrassônico
{ 
	    PORTB |= (1 << PORTB1); // Gera pulso de 10 microssegundos no pino TRIG
	    _delay_us(10);
	    PORTB &= ~(1 << PORTB1);

	    while (!(PIND & (1 << PIND2))) // Aguarda o pulso de resposta no pino ECHO (! inverte a logica)
	    ;

	    // Inicia o Timer1
	    TCCR1B |= (1 << CS11); // Prescaler 8

	    // Aguarda a borda de subida no pino ECHO
	    while (PIND & (1 << PIND2))
	    ;

	    // Desliga o Timer1
	    TCCR1B = 0;

	    // Calcula a distância em centímetros
	    return (TCNT1 * SOUND_SPEED) / (2 * 8 * F_CPU / 1000000);
}

int main(void)
{
    /* Replace with your application code */
}

