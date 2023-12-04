#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void USART_init() {
    // Set baud rate to 9600
    UBRR0H = (unsigned char)(103 >> 8);
    UBRR0L = (unsigned char)103;
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_send(char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    // Put data into buffer, sends the data
    UDR0 = data;
}

void printString(const char myString[]) {
    for (int i = 0; myString[i] != 0; i++)
        USART_send(myString[i]);
}

uint16_t analogRead(uint8_t channel) {
    ADMUX = (1 << REFS0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
        ;
    return ADC;
}

int main(void) {
    // Set A0 as input (analog)
    DDRC &= ~(1 << DDC0);

    // Initialize USART
    USART_init();

    while (1) {
        // Read analog values from A0
        uint16_t AnalogSensorValue = analogRead(0);

        // Print values to the serial monitor
        printString("Analog Sensor: ");
        char buffer[10];
        sprintf(buffer, "%d", AnalogSensorValue);
        printString(buffer);

        // Pause for 100 milliseconds
        _delay_ms(100);
    }

    return 0;
}
