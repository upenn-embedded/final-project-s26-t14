//#define F_CPU 16000000UL
//#include <xc.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include "i2c.h"
//#include "uart.h"
//#include "imu.h"
//
//// CRITICAL: Must be volatile so the compiler knows it changes in an ISR
//volatile int shaking = 0;
//
//ISR(PCINT1_vect) {
//    // Only trigger if the pin is currently HIGH (the start of the pulse)
//    if (PINC & (1 << PC3)) {
//        shaking = 1;
//    }
//}
//
//void imu_init() {
//    // 1. WAKE UP: Register 0x6B (PWR_MGMT_1) -> 0x00
////    if (i2c_writeRegister(MPU6050_ADDR, 0x6B, 0x01) != I2C_OK) return;
////    _delay_ms(10);
//    
//    i2c_writeRegister(MPU6050_ADDR, 0x6B, 0x01); // 0x01 instead of 0x00
//
//    // 2. CONFIG MOTION: Register 0x1F (Threshold), 0x20 (Duration)
//    i2c_writeRegister(MPU6050_ADDR, 0x1F, 20); // Lower threshold to 40mg for easier testing
//    i2c_writeRegister(MPU6050_ADDR, 0x20, 1);  // 1ms duration
//
//    // 3. INT PIN CFG: Register 0x37 
//    // Set to 0x00: Active High, Push-Pull, 50us Pulse (No latching)
//    // This is better for PCINT because it returns to 0 automatically.
//    i2c_writeRegister(MPU6050_ADDR, 0x37, 0x00); 
//
//    // 4. ENABLE INT: Register 0x38 -> 0x40 (Motion Enable)
//    i2c_writeRegister(MPU6050_ADDR, 0x38, 0x40);
//
//    // 5. AVR PCINT SETUP (PC3 / PCINT11)
//    DDRC &= ~(1 << PC3);    // Input
//    PORTC &= ~(1 << PC3);   // No pull-up (MPU6050 drives it)
//    
//    PCMSK1 |= (1 << PCINT11);
//    PCICR  |= (1 << PCIE1);
//    
//    sei(); 
//}
//
//
//int main(void) {
//    uart_init();
//    i2c_init();
//    imu_init();
//    
//    printf("IMU Motion Test Started\n");
//    
//    while (1) {
//        if (shaking) {
//            printf("SHAKEN!!!\n");
//            
//            // 6. CLEAR THE INTERRUPT on the MPU6050
//            // You MUST read register 0x3A to reset the INT pin
//            uint8_t dummy;
//            i2c_readCompleteStream(&dummy, MPU6050_ADDR, 0x3A, 1);
//            
//            shaking = 0; // Reset flag
//        } else {
//            printf("nothing rn... \n");
//        }
//        _delay_ms(500);
//    }
//}

//#define F_CPU 16000000UL
//#include <xc.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include "i2c.h"
//#include "uart.h"
//#include "imu.h"
//
//// CRITICAL: Must be volatile so the compiler knows it changes in an ISR
//volatile int shaking = 0;
//
//ISR(PCINT1_vect) {
//    // Only trigger if the pin is currently HIGH (the start of the pulse)
//    if (PINC & (1 << PC3)) {
//        shaking = 1;
//    }
//}
//
//void imu_init() {
//    // 1. WAKE UP: Register 0x6B (PWR_MGMT_1) -> 0x00
////    if (i2c_writeRegister(MPU6050_ADDR, 0x6B, 0x01) != I2C_OK) return;
////    _delay_ms(10);
//    
//    i2c_writeRegister(MPU6050_ADDR, 0x6B, 0x01); // 0x01 instead of 0x00
//
//    // 2. CONFIG MOTION: Register 0x1F (Threshold), 0x20 (Duration)
//    i2c_writeRegister(MPU6050_ADDR, 0x1F, 20); // Lower threshold to 40mg for easier testing
//    i2c_writeRegister(MPU6050_ADDR, 0x20, 5);  // 5ms duration
//    
//    // 2.5 ACCEL CONFIG (The "Tilt" Secret)
//    // 0x02 sets a 1.25Hz High Pass Filter - better for detecting slower swaying
//    i2c_writeRegister(MPU6050_ADDR, 0x1C, 0x02);
//
//    // 3. INT PIN CFG: Register 0x37 
//    // Set to 0x00: Active High, Push-Pull, 50us Pulse (No latching)
//    // This is better for PCINT because it returns to 0 automatically.
//    i2c_writeRegister(MPU6050_ADDR, 0x37, 0x00); 
//
//    // 4. ENABLE INT: Register 0x38 -> 0x40 (Motion Enable)
//    i2c_writeRegister(MPU6050_ADDR, 0x38, 0x40);
//
//    // 5. AVR PCINT SETUP (PC3 / PCINT11)
//    DDRC &= ~(1 << PC3);    // Input
//    PORTC &= ~(1 << PC3);   // No pull-up (MPU6050 drives it)
//    
//    PCMSK1 |= (1 << PCINT11);
//    PCICR  |= (1 << PCIE1);
//    
//    sei(); 
//}
//
//
//int main(void) {
//    uart_init();
//    i2c_init();
//    imu_init();
//    
//    printf("IMU Motion Test Started\n");
//    
//    while (1) {
//        if (shaking) {
//            printf("SHAKEN!!!\n");
//            
//            // 6. CLEAR THE INTERRUPT on the MPU6050
//            // You MUST read register 0x3A to reset the INT pin
//            uint8_t dummy;
//            i2c_readCompleteStream(&dummy, MPU6050_ADDR, 0x3A, 1);
//            
//            shaking = 0; // Reset flag
//        } else {
//            printf("nothing rn... \n");
//        }
//        _delay_ms(500);
//    }
//}

//#define F_CPU 16000000UL
//#include <xc.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include "i2c.h"
//#include "uart.h"
//#include "imu.h"
//#include "sound.h"
//
//
//
//
//int main(void) {
//    uart_init();
////    i2c_init();
////    imu_init();
//    
//    DDRD |= (1 << DDD5);
//    
//    
//    printf("Speaker Test Started\n");
//    
//    while (1) {
//        play_shoot();
//        _delay_ms(1000);
//        play_enemy_death();
//        _delay_ms(1000);
//        play_player_damage();
//        _delay_ms(1000);
//    }
//}