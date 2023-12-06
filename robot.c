/*
 *      Program uses a ATMega328p MCU to query a HC-SR04 ultrasonic module. 
 *      Pin placement of ATMega328p:
 *      Pin PC4				HC-SR04 Trig
 *      Pin PC5				HC-SR04 Echo
 */            
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

#define ENA 5 // Pinos de controle de velocidade usando o PWM para motor esquerdo (PB1 - OC1A)
#define ENB 6 // Pinos de controle de velocidade usando o PWM para motor esquerdo (PB2 - OC1B)
#define IN1 1 // pino de controle da ponte H (PD1)
#define IN2 2 // pino de controle da ponte H (PD2)
#define IN3 3 // pino de controle da ponte H (PD3)
#define IN4 4 // pino de controle da ponte H (PD4)

#define TRIG_PIN 4 // PC4
#define ECHO_PIN 5 // PC5

void init() {
	// Sensor Ultrassônico
	DDRC |= (1 << TRIG_PIN);		// Define o TRIG como saída
	DDRC &= ~(1 << ECHO_PIN);		// Define ECHO como entrada
	PORTC |= (1 << ECHO_PIN);		// Habilita o pull up no pino ECHO
	PORTC &= ~(1 << TRIG_PIN);	// Iniciar o TRIG como 0

	TCNT1 = 0;									// Valor inicial do timer
	TCCR1B |= (1 << CS11);			// Define o prescaller em 1:8
	TCCR1B |= (1 << ICES1);			// Captura em borda de subida

	PCICR = (1 << PCIE1);				// Altera a interrupção no PC5/ECHO
	PCMSK1 = (1 << PCINT13);		// Altera a interrupção no PC5/ECHO

	// PWM
	DDRD |= (1 << ENA) | (1 << ENB) | (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4); // Pinos de saída para a ponte H
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00); // Limpa OC0A e OC0B no 'match' da comparação / Fast PWM
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaller 1:64 

	robot_forward();
	sei();										// Enable Global Interrupts
}

// Define os valores de PWM para os canais A e B (0 a 255)
void set_PWM(int dutyCycleA, int dutyCycleB) {
    OCR0A = dutyCycleA;
    OCR0B = dutyCycleB;
}

// Rotaciona o robô para a esquerda, invertendo o sentido da rotação dos motores, com o PWM em 20%
void robot_left() {
	set_PWM(51, 51); // 20% de PWM
	PORTD &= ~(1 << IN1);
	PORTD |= (1 << IN2);
	PORTD |= (1 << IN3);
	PORTD &= ~(1 << IN4);
}
// Move o robô à frente com PWM em 50%
void robot_forward() {
	set_PWM(128, 128);
	PORTD |= (1 << IN1);
	PORTD &= ~(1 << IN2);
	PORTD |= (1 << IN3);
	PORTD &= ~(1 << IN4);
}

int main() {
	init(); // Define todas as configurações padrões de pinos e timers
	while (1) {;
		_delay_ms(60); 							
		// Envia um pulso de 10us no pino TRIG do sensor
		PORTC |= (1 << TRIG_PIN); 	// 1
		_delay_us(10);							
		PORTC &= ~(1 << TRIG_PIN);	// 0
	}
}

ISR(PCINT1_vect) {
	if (bit_is_set(PINC,PC5)) {				// Se ECHO sobe para 1
		TCNT1 = 0;											// inicia o timer em 0
	} else {													// Se ECHO desce para 0
		long numuS = TCNT1/2;						// Salva o tempo do timer
		uint8_t oldSREG = SREG;
		cli();													// Limpa a flag de interrupção
		long distancia = numuS/58;
		SREG = oldSREG;									// Habilita a interrupção
		if (distancia < 30) {           // Se houver obstáculo a frente, rotaciona o robô
				robot_left();
		} else {                        // Se não, o robô segue em frente
				robot_forward();       
		}
	}
}
