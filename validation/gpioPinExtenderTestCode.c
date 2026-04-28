#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "i2c.h"
#include "uart.h"
#include "aw9523.h"

#define F_CPU 16000000UL

void Initialize() {

    uart_init();
    i2c_init();
    
    // pin extender init
    if (aw9523_init() != AW9523_OK) {
        printf("AW9523 FAIL\n");
    }
    sei();
}


int main(void) {
    Initialize();

    printf("Test starting...\n");

    while (1) {
        
        for (uint32_t i = 0; i < 16; i++) {
            set_led(i, 1);
            _delay_ms(1000);
            set_led(i, 0);
            _delay_ms(1000);
        }

    }
    return 0;
}