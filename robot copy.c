#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL // Frequencia de clock do ATmega328P 16mHz

#define IN1 7 // pino de controle da ponte H (PD7)
#define IN2 6 // pino de controle da ponte H (PD6)
#define IN3 5 // pino de controle da ponte H (PD5)
#define IN4 4 // pino de controle da ponte H (PD4)

#define ENA 9 // Pinos de controle de velocidade usando o PWM para motor esquerdo (PB1 - OC1A)
#define ENB 10 // Pinos de controle de velocidade usando o PWM para motor direito (PB2 - OC1B)


volatile uint16_t pulse_width = 0; // variável volátil (pode ter seu valor alterado a qualquer momento no programa) para largura de pulso

int main(void) { // Configurações iniciais
	// Configuração dos pinos para a ponte H
	DDRD |= (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4); // Seta todos os 4 pinos de controle da ponte H como saída (output)
	DDRB |= (1 << ENA) | (1 << ENB); // Seta ambos os pinos de controle de velocidade como saída (output)

	init_PWM();
	set_PWM(120, 120);

	sei(); // Habilita interrupções globais
	init_timer1();

	while(1) {

	float dist_cm = calculate_distance(); // calcula distância em cm;

	while (dist_cm < 20) { // Enquanto a distancia for menor que 20cm, segue rotacionando e recalculando a distância.
		decisao();
		dist_cm = calculate_distance() // Pegar a distancia do sensor.c
	}

	robo_frente();
	}
}

// Funções auxiliares

void init_PWM() {
	// Configuração do Timer/Counter1 para PWM;
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10); // COM1A1 COM1B1 configurado para limpar OC1A e OC1B quando a comparação da um 'match' durante a contagem.
	TCCR1B |= (1 << CS11) | (1 << WGM12); // CS11 configura o prescaler para 1:8 // WGM10 E WGM12 configuram o modo de operação do Timer/counter como modo 5, fast PWM de 8 bits.
	// Configuração dos pinos de saída para os canais A e B;
}

void set_PWM(uint8_t duty_cycle_A, uint8_t duty_cycle_B) { // valor como (128, 128) indica 50% de duty cicle em ambos os canais
	// Configuração do ciclo de trabalho para os canais A e B;
	OCR1A = duty_cycle_A; // ciclo de trabalho de 0 a 255
	OCR1B = duty_cycle_B; // ciclo de trabalho de 0 a 255
}

void init_timer1() {
	// Configura o Timer1 para capturar o tempo de pulso;
	TCCR1B |= (1 << ICES1); // Configuração para captura na borda de subida (rising edge);
	TIMSK1 |= (1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A); // Ativa a interrupção quando ocorre a captura de pulso / Ativa interrupção por comparação de A e B.
}

void robo_frente() {
	PORTD |= (1 << IN1);
	PORTD &= ~(1 << IN2);
	PORTD |= (1 << IN3);
	PORTD &= ~(1 << IN4);
}

void robo_esquerda() {
	PORTD |= (1 << IN1);
	PORTD &= ~(1 << IN2);
	PORTD &= ~(1 << IN3);
	PORTD |= (1 << IN4);
}

void robo_parado() {
	PORTD &= ~(1 << IN1);
	PORTD &= ~(1 << IN2);
	PORTD &= ~(1 << IN3);
	PORTD &= ~(1 << IN4);
}

void decisao() {
	robo_parado();
	_delay_ms(500);

	robo_esquerda();
	_delay_ms(400); 	// Esse tempo precisa ser avaliado para que o robô vire por volta de 90 graus;

	robo_parado();
	_delay_ms(500);
}
