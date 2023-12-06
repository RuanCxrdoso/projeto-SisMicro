#include <avr/io.h>
#include <util/delay.h>

int rawsensorValue = 0; // variável para armazenar o valor do sensor
int sensorcount0 = 0;
int sensorcount1 = 0;
long count = 0;

void setup() {
    int i;
    for (i = 5; i <= 8; i++) {
        // Configurar os pinos 5 a 8 como saídas
        DDRC |= (1 << i);
    }
}

void loop() {
    // Configurar a saída PWM no pino 10 com um valor de 255
    OCR1A = 255;
    // Configurar o pino 12 como LOW (0 volts)
    PORTB &= ~(1 << PB4);
    // Configurar a saída PWM no pino 11 com um valor de 255
    OCR1B = 255;
    // Configurar o pino 13 como LOW (0 volts)
    PORTB &= ~(1 << PB5);
    _delay_ms(20);

    // Ler o valor analógico do pino A0 (supostamente conectado a um sensor)
    rawsensorValue = ADC_read(0);

    // Se o valor lido do sensor for inferior a 600, sensorcount1 é definido como 1; caso contrário, é definido como 0.
    sensorcount1 = (rawsensorValue < 600) ? 1 : 0;

    // Se houver uma mudança no estado do sensor em comparação com o estado anterior, incrementa o contador count.
    if (sensorcount1 != sensorcount0) {
        count++;
    }

    // Atualiza o estado anterior do sensor para o estado atual.
    sensorcount0 = sensorcount1;
}

// Função para ler um canal analógico usando o conversor analógico-digital (ADC)
uint16_t ADC_read(uint8_t channel) {
    // Configurar o canal do ADC e outras configurações
    ADMUX = (1 << REFS0) | (channel & 0x0F);
    // Iniciar a conversão ADC
    ADCSRA |= (1 << ADSC);
    // Esperar até que a conversão seja concluída
    while (ADCSRA & (1 << ADSC))
        ;
    // Retornar o valor convertido
    return ADC;
}
