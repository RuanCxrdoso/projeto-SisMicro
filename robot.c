#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL // Frequencia de clock do ATmega328P 16mHz

#define IN1 1 // pino de controle da ponte H (PD1)
#define IN2 2 // pino de controle da ponte H (PD2)
#define IN3 3 // pino de controle da ponte H (PD3)
#define IN4 4 // pino de controle da ponte H (PD4)

#define ENA 5 // Pinos de controle de velocidade usando o PWM para motor esquerdo (PD5)
#define ENB 6 // Pinos de controle de velocidade usando o PWM para motor direito (PD6)

#define TRIG_PIN 4 // PC4
#define ECHO_PIN 5 // PC5

long tempo, distancia, dist_cm;

void robot_forward(void);
void robot_left(void);
void robot_stop(void);

void init() { // Configurações iniciais
	// Sensor ultrassônico
	DDRC |= (1 << TRIG_PIN);	// Pino TRIG como saída
	DDRC &= ~(1 << ECHO_PIN);	// Pino de input para leitura do ECHO
	PORTC |= (1 << ECHO_PIN);		// Habilita pull up no pino PC5/ECHO
	PORTC &= ~(1 << TRIG_PIN);// Inicia o TRIG como baixo

	TCNT1 = 0;								// Valor inicial do Timer1
	TCCR1B |= (1<<CS11);			// Prescaller em 1:8
	TCCR1B |= (1<<ICES1);			// Primeira captura na borda de subida

	PCICR = (1 << PCIE1);			// Habilita interrupção por mudança
	PCMSK1 = (1 << PCINT13);	// Habilita a interrupção por mudança no PC5
	sei(); // Habilita interrupções globais

	// Pinos de saída para a ponte H
	DDRD |= (1 << ENA) | (1 << ENB) | (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4);

	// PWM
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler	
	OCR0A = 0; // PWM PARA O PINO ENA DA PONTE H
	OCR0B = 0; // PWM PARA O PINO ENB DA PONTE H
}

int main(void) { 
	init();
	robot_stop();

	while(1) {
		dist_cm = distance();

		while (dist_cm < 20) { // Enquanto a distancia for menor que 20cm, segue rotacionando e recalculando a distância.
			robot_left();
			_delay_ms(100);
			robot_stop();
			dist_cm = distance();
		}

		robot_forward();
	}
}

void set_PWM(int dutyCycleA, int dutyCycleB) {
		OCR0A = dutyCycleA;
		OCR0B = dutyCycleB;
}

long distance() {
	// Envio do pulso no pino TRIG
	PORTB |= (1 << TRIG_PIN);
	_delay_us(10);
	PORTB &= ~(1 << TRIG_PIN);
	
	// while((PINB & (1 << ECHO_PIN)) == 0); // Aguarda o sinal do ECHO ir para 1
	// TCNT1 = 0; // Inicia a contagem quando o ECHO sobe para 1
	// while(PINB & (1 << ECHO_PIN)); // Aguarda o sinal do ECHO ir para 0
	// tempo = TCNT1 * 4.0;
	// distancia = tempo / 58.0;
	return distancia;
}

void robot_forward() {
	set_PWM(128, 128);
	PORTD |= (1 << IN1);
	PORTD &= ~(1 << IN2);
	PORTD |= (1 << IN3);
	PORTD &= ~(1 << IN4);
}

void robot_left() {
	set_PWM(51, 51); // 20% de PWM
	PORTD &= ~(1 << IN1);
	PORTD |= (1 << IN2);
	PORTD |= (1 << IN3);
	PORTD &= ~(1 << IN4);
}

void robot_stop() {
	set_PWM(0, 0);
	PORTD &= ~(1 << IN1);
	PORTD &= ~(1 << IN2);
	PORTD &= ~(1 << IN3);
	PORTD &= ~(1 << IN4);
}

ISR(PCINT1_vect) {
	if (bit_is_set(PINC,PC5)) {	// Se o ECHO está em 1
		TCNT1 = 0;								// Reseta o timer
	} else { 										// Se o ECHO está em 0
		long numuS = TCNT1/2;			// Salva o valor do timer
		uint8_t oldSREG = SREG;
		cli();										// Limpa a flag de interrupção globais
		distancia = numuS/58;
		SREG = oldSREG;
	}
}

// DDRB |= (1 << TRIG_PIN); // PINO TRIG COMO OUTPUT
// DDRB &= ~(1 << ECHO_PIN); // PINO ECHO COMO INPUT
// PORTB &= ~((1 << TRIG_PIN) | (1 << ECHO_PIN)); // ECHO & TRIG == 0
// TCCR1B |= ((1 << CS11) & (1 << CS10)); // Ajusta o prescaler em 1:64 -> (64 / 16mHz) * 1m = 4ms
