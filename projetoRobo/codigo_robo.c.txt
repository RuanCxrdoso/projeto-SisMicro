#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define TRIGGER_PIN 3
#define ECHO_PIN 2

#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4

#define ENA 9
#define ENB 10

#define TRIG_PIN PD2
#define ECHO_PIN PD3

volatile uint16_t pulse_width = 0;

void setup() {
	// Configuração dos pinos dos motores
	DDRD |= (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4);
	// Configuração dos pinos de controle de velocidade
	DDRD |= (1 << ENA) | (1 << ENB);

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

void init_PWM() {
	// Configuração do Timer/Counter1 para PWM;
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
	TCCR1B |= (1 << CS11) | (1 << WGM12);
	// Configuração dos pinos de saída para os canais A e B;
	DDRB |= (1 << ENA) | (1 << ENB);
}

void set_PWM(uint8_t duty_cycle_A, uint8_t duty_cycle_B) {
	// Configurao do ciclo de trabalho para os canais A e B;
	OCR1A = duty_cycle_A;
	OCR1B = duty_cycle_B;
}

void init_timer1() {
	// Configura o Timer1 para capturar o tempo de pulso;
	TCCR1B |= (1 << ICES1); // Configura para captura no flanco de subida (rising edge);
	TIMSK1 |= (1 << ICIE1); // Ativa a interrupção de captura;
}

void send_pulse() {
	// Gera um pulso de 10 microssegundos no pino de trigger;
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
		// Flanco de subida detectado;
		start_time = ICR1; // Salva o tempo de início do pulso;
		TCCR1B &= ~(1 << ICES1); // Muda para captura no flanco de descida;
		} else {
		// Flanco de descida detectado;
		uint16_t end_time = ICR1; // Salva o tempo de término do pulso;
		pulse_width = end_time - start_time; // Calcula a largura do pulso em microssegundos;
		TCCR1B |= (1 << ICES1); // Muda de volta para captura no flanco de subida;
	}
}
