#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "i2c.h"
#include "imu.h"
#include "uart.h"
#include "sound.h"

#define F_CPU 16000000UL

static uint8_t s_reset_cause;
static volatile uint32_t s_ms = 0;
static volatile uint8_t s_shoot = 0;

ISR(TIMER1_COMPA_vect) {
    s_ms++;
}

static void timer1_init(void) {
    TCCR1A = 0;
    // CTC mode, Prescaler 64
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); 
    OCR1A  = 249; // (16MHz / 64 / 1000Hz) - 1 = 249 for 1ms
    TIMSK1 = (1 << OCIE1A);
}

uint32_t millis(void) {
    uint32_t t;
    cli();
    t = s_ms;
    sei();
    return t;
}

ISR(INT0_vect) {
    static uint32_t last_interrupt_time = 0;
    uint32_t interrupt_time = s_ms;
    
    // software debounce
    if (interrupt_time - last_interrupt_time > 150) {
        s_shoot = 1;
    }
    last_interrupt_time = interrupt_time;
}

void Initialize() {
    s_reset_cause = MCUSR;
    MCUSR = 0;

    uart_init();
    timer1_init();
    i2c_init();
    
    // MPU6050 Wakeup
    if (i2c_writeRegister(MPU6050_ADDR, 0x6B, 0x00) != I2C_OK) {
        printf("IMU FAIL\n");
    }

    // Button Setup
    DDRD  &= ~(1 << DDD2);   
    PORTD |= (1 << PORTD2);  // Pull-up
    EICRA |= (1 << ISC01);   // Falling edge
    EIMSK |= (1 << INT0);  
    
    // Buzzer Setup (PD3 as output)
    DDRD |= (1 << DDD3);

    sei();
}

int main(void) {
    Initialize();
    
    printf("Game Starting...\n");
    pacman_theme(); 

    uint32_t last_ms = millis();
    
    while (1) {
        if (s_shoot) {
            s_shoot = 0;
            printf("Shoot!!!\n");
        }

        uint32_t now = millis();
        uint32_t elapsed_ms = now - last_ms;

        // 100Hz Update Loop
        if (elapsed_ms >= 10) {
            float dt = elapsed_ms / 1000.0f;
            last_ms = now;

            if (mpu6050_update(dt) == MPU6050_OK) {
                printf("ax:%.1f ay:%.1f az:%.1f\n",
                       get_ax(), get_ay(), get_az());
            }
        }
    }
    return 0;
}