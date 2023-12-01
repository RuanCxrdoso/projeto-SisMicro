/* 
 * File:   mainSensor.c
 * Author: ruanz
 *
 * Created on 24 de Novembro de 2023, 21:07
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define TRIG_PIN PB0 // PINO 8 ARDUINO
#define ECHO_PIN PB1 // PINO 9 ARDUINO

long tempo, distancia;

int main(void) {
  DDRB |= (1 << TRIG_PIN); // PINO TRIG COMO OUTPUT
  DDRB &= ~(1 << ECHO_PIN); // PINO ECHO COMO INPUT

  PORTB &= ~((1 << TRIG_PIN) | (1 << ECHO_PIN)); // Inicia ECHO e TRIG como nível lógico baixo

  TCCR1A = 0x00;
  TCCR1B |= ((1 << CS11) & (1 << CS10)); // Ajusta o prescaler em 1:64 -> (64 / 16mHz) * 1m = 4ms
  
  while(1) {
	// Envio do pulso de 10ms no pino TRIG
	PORTB |= (1 << TRIG_PIN);
	_delay_us(10);
	PORTB &= ~(1 << TRIG_PIN);
	
	while((PINB & (1 << ECHO_PIN)) == 0); // Não avança enquanto o ECHO estiver em nível lógico baixo
	TCNT1 = 0; // Inicia a contagem quando o ECHO sobre para nível lógico alto
	while(PINB & (1 << ECHO_PIN)); // Não avança enquanto o ECHO estiver em nível lógico alto
	
	tempo = TCNT1 * 4.0; // Converte em microssegundos
	distancia = tempo / 58.0; // Converte o tempo em distância em centímetros
  }
}


