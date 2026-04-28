///*
// * File:   atmega2code.c
// * Author: daniel
// *
// * Created on April 24, 2026, 6:29 AM
// */
//
//
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
//volatile int play_sound = 0;
//
//ISR(INT0_vect) {
//    play_sound = 1;
//}
//
//ISR(INT1_vect) {
//    play_sound = 2;
//}
//
//void interrupt_init(void) {
//    // Set PD2 as input
//    DDRD &= ~(1 << PD2);
////    DDRD &= ~(1 << PD3);
//    // Enable internal pull-up (This is why we connect button to GND)
//    PORTD |= (1 << PORTD2);
////    PORTD |= (1 << PORTD3);
//    
//    // Configure INT0 to trigger on Falling Edge (High to Low)
//    // ISC01 = 1, ISC00 = 0 in EICRA register
//    EICRA |= (1 << ISC01);
//    EICRA &= ~(1 << ISC00);
//    
////    EICRA |= (1 << ISC11);
////    EICRA |= (1 << ISC10);
//    
//    
//    
//    // Enable the INT0 external interrupt mask
//    EIMSK |= (1 << INT0);
////    EIMSK |= (1 << INT1);
//    
//    // TURN ON GLOBAL INTERRUPTS
//    sei(); 
//}
//
//int main(void) {
//    uart_init();
////    i2c_init();
////    imu_init();
//    interrupt_init();
//    
//    DDRD |= (1 << DDD5);
//    
//    DDRD &= ~(1 << PD3);    // Set PD3 as Input for Polling
//    PORTD |= (1 << PD3);    // Enable Pull-up on PD3
//    DDRD |= (1 << DDD5);    // Speaker Output
//    
//    
//    
//    
//    printf("Speaker Test Started\n");
//    
//    while (1) {
////        if (play_sound == 1) {
////            play_shoot();
////            play_sound = 0;
////        } else if (play_sound == 2) {
////            play_enemy_death();
////            play_sound = 0;
////        } else {
////            play_player_damage();
////            play_sound = 0;
////        }
//        if (play_sound == 1) {
//            play_shoot();
//            play_sound = 0;
//        }
//        
////        if (PIND & (1 << PD3)) { 
////            play_enemy_death();
////            // Wait for Board 1 to pull it LOW again
////            while(PIND & (1 << PD3)); 
////        }
//    }
//}
