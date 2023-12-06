/*
 *   
 *      Program uses a ATMega328p MCU to query a HC-SR04 ultrasonic module. 
 *      Pin placement of ATMega328p:
 *      Pin PC4				HC-SR04 Trig
 *      Pin PC5				HC-SR04 Echo
 */                             
#define F_CPU 16000000UL
#define ENA 5 // Pinos de controle de velocidade usando o PWM para motor esquerdo (PB1 - OC1A)
#define ENB 6
#define IN1 1 // pino de controle da ponte H (PD1)
#define IN2 2 // pino de controle da ponte H (PD2)
#define IN3 3 // pino de controle da ponte H (PD3)
#define IN4 4 // pino de controle da ponte H (PD4)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


/*******************************************INICIALIZAÇÃO DAS PORTAS, DOS TIMERS E DOS REGISTRADORES*******************************************/
void init() {
    DDRC |= (1<<DDC4);							// Todos os pinos do Port C configurados como saída. PC0: RW   PC1: RS   PC2: E;
	DDRC &= ~(1<<DDC5);						// Define o Pino C5 como entrada para leitura do Echo
	PORTC |= (1<<PORTC5);					       // Ativa o pull-up em C5
	PORTC &= ~(1<<PC4);					       // Inicializa C4 em nível baixo (trigger)

	PRR &= ~(1<<PRTIM1);					//  Para ativar o módulo do timer1;
	TCNT1 = 0;						// Valor inicial do timer
	TCCR1B |= (1<<CS11);					// Timer sem prescaler. Como o clock padrão para o atmega328p é 1Mhz, o período é 1uS
	TCCR1B |= (1<<ICES1);					// Captura inicial na borda de subida

	PCICR = (1<<PCIE1);						// Habilita PCINT[14:8], usamos o pino C5 que é PCINT13
	PCMSK1 = (1<<PCINT13);					//Habilita a interrupção em C5
	
    DDRD |= (1 << ENA) | (1 << ENB);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00);
    robot_forward();
    sei();									// Habilita a interrupção global
    
}

void set_PWM(int dutyCycleA, int dutyCycleB) {
    OCR0A = dutyCycleA;                    //Define o valor do registrador OCR0A (Output Compare Register A) com o valor de "dutyCycleA"
    OCR0B = dutyCycleB;                   //Define o valor do registrador OCR0B (Output Compare Register B) com o valor de "dutyCycleB"
}

void robot_left() {
	set_PWM(51, 51); // 20% de PWM
	PORTD &= ~(1 << IN1);  //Coloca em zero o bit associado ao pino IN1 no registrador PORTD
	PORTD |= (1 << IN2);  //Coloca em um o bit associado ao pino IN2 no registrador PORTD
	PORTD |= (1 << IN3);  //Coloca em um o bit associado ao pino IN3 no registrador PORTD
	PORTD &= ~(1 << IN4); //Coloca em zero o bit associado ao pino IN4 no registrador PORTD
}

void robot_forward() {
	set_PWM(128, 128);
	PORTD |= (1 << IN1);
	PORTD &= ~(1 << IN2);
	PORTD |= (1 << IN3);
	PORTD &= ~(1 << IN4);
}

/*******************************************MAIN PROGRAM*******************************************/
int main() {
	init();
	while (1) {;
		_delay_ms(60); 							// To allow sufficient time between queries (60ms min)
		PORTC |= (1<<PC4);						// Set trigger high
		_delay_us(10);							// for 10uS
		PORTC &= ~(1<<PC4);						// to trigger the ultrasonic module
	}
}
/*******************************************INTURRUPT PCINT1 FOR PIN C5*******************************************/
ISR(PCINT1_vect) {
	if (bit_is_set(PINC,PC5)) {					// Checks if echo is high
		TCNT1 = 0;								// Reset Timer
	} else {
		long numuS = TCNT1/2;					// Save Timer value
		uint8_t oldSREG = SREG;
		cli();
        long distancia = numuS/58;
		SREG = oldSREG;							// Enable interrupts
        if (distancia < 30){
            robot_left();
        } else {
            robot_forward();
        }
	}
}
