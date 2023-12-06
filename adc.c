ISR(TIMER0_COMPA_vect) {
    count++;
    if (count == 100) { 
        PORTD ^= (1 << PORTD0); 
        count = 0;
    }
}

int main(void) {
    DDRD |= (1 << DDD6); 

    // ADC Configuration - Using ADC0 (Pin A0)
    ADMUX = 0; // Use ADC0
    ADMUX |= (1 << REFS0); // AVCC as voltage reference
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Liga o ADC e põe prescaler de 1:128

    // Timer0 Configuration for Fast PWM
    TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01); 
    TCCR0B = (1 << CS00); 

    while (1) {
        ADCSRA |= (1 << ADSC); // Inicia a conversão
        while (ADCSRA & (1 << ADSC)); // Aguarda a conversão finalizar
        
        OCR0A = ADC >> 2; 
        PORTB = ADC;
 
 
        _delay_ms(100); 
    }

    return 0;
