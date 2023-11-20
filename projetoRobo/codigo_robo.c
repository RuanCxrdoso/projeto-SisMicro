#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL // Frequencia de clock do ATmega328P 16mHz
#define TRIG_PIN 3 // Trigger do sensor - PD3 (emissor do pulso)
#define ECHO_PIN 2 // Echo do sensor - PD2 (receptor do pulso)

#define IN1 7 // pino de controle da ponte H (PD7)
#define IN2 6 // pino de controle da ponte H (PD6)
#define IN3 5 // pino de controle da ponte H (PD5)
#define IN4 4 // pino de controle da ponte H (PD4)

#define ENA 9 // Pinos de controle de velocidade usando o PWM para motor esquerdo (PB1)
#define ENB 10 // Pinos de controle de velocidade usando o PWM para motor direito (PB2)

// #define TRIG_PIN TRIGGER_PIN 
// #define ECHO_PIN ECHO_PIN 

volatile uint16_t pulse_width = 0; // variável volátil (pode ter seu valor alterado a qualquer momento no programa) para largura de pulso

void setup() { // Configurações iniciais
	// Configuração dos pinos dos motores
	DDRD |= (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4); // Seta todos os 4 pinos de controle da ponte H como saída (output)
	// Configuração dos pinos de controle de velocidade
	DDRD |= (1 << ENA) | (1 << ENB); // Seta ambos os pinos como saída (output)

	init_PWM();
	set_PWM(120, 120);

	// Configuração dos pinos de trigger (saída) e echo (entrada)
	DDRD |= (1 << TRIG_PIN);
	DDRD &= ~(1 << ECHO_PIN);

	init_timer1();
	sei(); // Habilita interrupções globais
}

void loop() {
	// Robô inicia andando para frente;
	robo_frente();

	float dist_cm = calculate_distance(); // Declara variável que armazena a distância do obstáculo;

	if (dist_cm < 20) // distância menor que 20cm?;
	{
		decisao();
	}

	_delay_ms(100);
}

// Funções auxiliares

void init_PWM() {
	// Configuração do Timer/Counter1 para PWM;
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10); // COM1A1 COM1B1 configurado para limpar OC1A e OC1B quando a comparação da um 'match' durante a contagem.
	TCCR1B |= (1 << CS11) | (1 << WGM12); // CS11 configura o prescaler para 1:8 // WGM10 E WGM12 configuram o modo de operação do Timer/counter como modo 5, fast PWM de 8 bits.
	// Configuração dos pinos de saída para os canais A e B;
	DDRB |= (1 << ENA) | (1 << ENB);
}

void set_PWM(uint8_t duty_cycle_A, uint8_t duty_cycle_B) { // valor como (128, 128) indica 50% de duty cicle em ambos os canais
	// Configuração do ciclo de trabalho para os canais A e B;
	OCR1A = duty_cycle_A; // ciclo de trabalho (clock em nível alto) de 0 a 255
	OCR1B = duty_cycle_B; // ciclo de trabalho (clock em nível alto) de 0 a 255
}

void init_timer1() {
	// Configura o Timer1 para capturar o tempo de pulso;
	TCCR1B |= (1 << ICES1); // Configuração para captura na borda de subida (rising edge);
	TIMSK1 |= (1 << ICIE1); // Ativa a interrupção quando ocorre a captura de pulso;
}

void send_pulse() {
	// Gera um pulso de 10 microssegundos no pino de trigger do sensor ultrassônico;
	PORTD |= (1 << TRIG_PIN);
	_delay_us(10);
	PORTD &= ~(1 << TRIG_PIN);
}

float calculate_distance() {
	// Velocidade do som no ar a 20 graus Celsius (em metros por segundo);
	float speed_of_sound = 343.0;
	// Fator de conversão para transformar a largura do pulso em centímetros;
	float conversion_factor = 100.0 / 2.0;  // Supondo que a largura do pulso está em microssegundos;
	// Calcula a distância usando a fórmula;
	return (pulse_width / 2.0) * (speed_of_sound / 1000000.0) * conversion_factor;
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

void robo_direita() {
	PORTD &= (1 << IN1);
	PORTD |= ~(1 << IN2);
	PORTD |= ~(1 << IN3);
	PORTD &= (1 << IN4);
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
	// Esse tempo precisa ser avaliado para que o robô vire por volta de 90 graus;
	_delay_ms(400);
	robo_parado();
	_delay_ms(500);
}

ISR(TIMER1_CAPT_vect) {
	// Interrupção é acionada quando o pulso é capturado;
	static uint16_t start_time = 0;

	if (TCCR1B & (1 << ICES1)) {
		// borda de subida detectado;
		start_time = ICR1; // Salva o tempo de início do pulso;
		TCCR1B &= ~(1 << ICES1); // Muda para captura no flanco de descida;
		} else {
		// Flanco de descida detectado;
		uint16_t end_time = ICR1; // Salva o tempo de término do pulso;
		pulse_width = end_time - start_time; // Calcula a largura do pulso em microssegundos;
		TCCR1B |= (1 << ICES1); // Muda de volta para captura no flanco de subida;
	}
}
