#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"

#define F_CPU 16000000UL

void Initialize() {

    uart_init();
    
    // ADC init for potentiometer
    ADMUX = (1 << REFS0);  
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler = 128

    sei();
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int main(void) {
    Initialize();

    printf("Test starting...\n");

    while (1) {
        uint16_t pot1 = adc_read(0);
        uint16_t pot2 = adc_read(1);
        printf("pot1: %u  pot2: %u\n", pot1, pot2);
        _delay_ms(1000);
    }
    return 0;
}