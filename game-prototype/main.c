#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "imu.h"
#include "uart.h"
 
#define F_CPU 16000000UL
 
static uint8_t s_reset_cause;
static volatile uint32_t s_ms = 0;
static volatile uint8_t s_shoot = 0;


ISR(TIMER1_COMPA_vect)
{
    s_ms += 1;
}

ISR(INT0_vect)
{
    s_shoot = 1;
}

static uint32_t millis(void)
{
    uint32_t t;
    cli();
    t = s_ms;
    sei();
    return t;
}

static void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC, prescaler 64
    OCR1A  = 249; // (16000000 / 64 / 10000) - 1 = 249 = 1ms tick
    TIMSK1 = (1 << OCIE1A);
}

void Initialize() {
    s_reset_cause = MCUSR;
    MCUSR = 0;
 
    uart_init();
 
    if (s_reset_cause & (1 << WDRF))  printf("RST:WDT\n");
    if (s_reset_cause & (1 << BORF))  printf("RST:BOD\n");
    if (s_reset_cause & (1 << EXTRF)) printf("RST:EXT\n");
    if (s_reset_cause & (1 << PORF))  printf("RST:PWR\n");

    timer1_init();
    i2c_init();
    
    if (i2c_writeRegister(MPU6050_ADDR, 0x6B, 0x00) != I2C_OK) {
        printf("MPU6050 init failed\n");
    }
 
    DDRD  &= ~(1 << DDD2);   // PD2 as input button, INT0
    PORTD |= (1 << PORTD2);  // enable internal pull-up
    
    EICRA |= (1 << ISC01);  // falling edge
    EICRA &= ~(1 << ISC00);

    EIMSK |=  (1 << INT0);  // enable INT
    
    sei();
    
}
 
int main(void)
{
    Initialize();
    uint32_t last_ms = millis();
    
    while (1) {
        if (s_shoot) {
            s_shoot = 0;
            printf("Shoot!!!\n");
        }

        uint32_t now = millis();
        uint32_t elapsed_ms = now - last_ms;
 
        if (elapsed_ms >= 10) {
            float dt = elapsed_ms / 1000.0f;
            last_ms = now;
 
            if (mpu6050_update(dt) == MPU6050_OK) {
                printf("P:%f R:%f Y:%f\n",
                       mpu6050_get_pitch(),
                       mpu6050_get_roll(),
                       mpu6050_get_yaw());
            } else {
                printf("update failed, g_i2c_last_error=0x%02X\n", get_error());
            }
        }
    }
    return 0;
}