/*
 *   
 *      Programa usando a ATMega328p MCU para consulta ao sensor ultrassonico HC-SR04. 
 *      Escolha dos pinos do ATMega328p:
 *      Pino PC4				HC-SR04 Trig
 *      Pino PC5				HC-SR04 Echo
 */                             
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*******************************************INICIALIZANDO PORTAS, TEMPORIZADOR E INTERRUPÇÕES*******************************************/
void init() {
	DDRD |= (1<<DDD5);					// Define o pino D5 Como Saida
	DDRC |= (1<<DDC4);					// Define o pino C4 Como Saida
	DDRC &= ~(1<<DDC5);					// Define o pino C5 como entrada para ler o sinal do echo.
	PORTC |= (1<<PORTC5);					// Habilitando o pull-up no pino C5.
	PORTC &= ~(1<<PC4);					// Iniciando o pino C4 como baixo (Trig).

	PRR &= ~(1<<PRTIM1);					// Ativando o timer1;
	TCNT1 = 0;						// Iniciando o valor do timer1
	TCCR1B |= (1<<CS11);					// Habilitando o prescaller.
	TCCR1B |= (1<<ICES1);					// Captura da borda de subida.

	PCICR = (1<<PCIE1);					// Habilitando o PCINT[14:8] usando o pino C5 que é o PCINY13.
	PCMSK1 = (1<<PCINT13);					// Habilitando interrupção C5
	sei();							// Habilitando interrupções globais
}

/*******************************************PROGRAMA PRINCIPAL*******************************************/
int main() {
	init();
	while (1) {;
		_delay_ms(60); 							// Para criar um atraso entre as consultas.
		PORTC |= (1<<PC4);						// Definindo o Trig como alto.
		_delay_us(10);							// Espera 10 microsegundos.
		PORTC &= ~(1<<PC4);						// Volta o pino Trig para nivel baixo.
	}
}
/*******************************************INTURRUPÇÃO PCINT1 PARA O PINO C5*******************************************/
ISR(PCINT1_vect) {
	if (bit_is_set(PINC,PC5)) {	  					// Checando se o pino echo está em estado alto.
		TCNT1 = 0;							// Zerando o timer.
	} else {
		long numuS = TCNT1/2;						// Salvando o valor do timer.
		uint8_t oldSREG = SREG;
		cli();
		long distancia = numuS/58;
		SREG = oldSREG;							// Habilitando interrupções.
		if (distancia < 30){
				PORTD |= (1 << PORTD5);
		} else {
				PORTD &= ~(1 << PORTD5);
		}
	}
}
