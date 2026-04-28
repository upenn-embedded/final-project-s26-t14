///*
// * File:   sound.c
// * Author: amarischen
// *
// * Created on April 10, 2026, 1:37 AM
// */
//
//#include <avr/io.h>
//#include <util/delay.h>
//#include "sound.h"
//#define F_CPU 16000000UL
//
//void silence() {
//    TCCR2A &= ~(1 << COM2B0);
//}
//
//void tone(uint16_t freq) {
//    if (freq == REST || freq == 0) {
//        TCCR2A &= ~(1 << COM2B0); // Disconnect Buzzer
//        return;
//    }
//    
//    TCCR2A = (1 << WGM21) | (1 << COM2B0);  // Toggle OC2B on Compare Match, CTC Mode
//    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);  // Prescaler 1024
//    
//    // (F_CPU / (2 * Prescaler * Freq)) - 1
//    uint32_t ocr_val = (F_CPU / (2UL * 1024UL * freq)) - 1;
//
//    if (ocr_val > 255) ocr_val = 255;
//    OCR2A = (uint8_t) ocr_val;
//}
//
//void play_note(uint16_t freq, uint16_t duration_ms) {
//    if (freq == REST) {
//        silence();
//        for(uint16_t i=0; i<duration_ms; i++) _delay_ms(1);
//    } else {
//        tone(freq);
//        for(uint16_t i=0; i<(duration_ms * 9 / 10); i++) _delay_ms(1);
//        silence();
//        for(uint16_t i=0; i<(duration_ms / 10); i++) _delay_ms(1);
//    }
//}
//
//void pacman_theme() {
//    play_note(B4, E); play_note(B5, E); play_note(F5, E);  play_note(Ds5, E);
//    play_note(B5, S); play_note(F5, DQ); play_note(E5, E);
//    
//    play_note(C5, E); play_note(C6, E); play_note(G5, E); play_note(E5, E);
//    play_note(C6, S); play_note(G5, DQ);
//    
//    play_note(B4, E); play_note(B5, E); play_note(F5, E); play_note(Ds5, E);
//    play_note(B5, S); play_note(F5, E); play_note(E5, E);
//    
//    play_note(E5, S);  play_note(F5, S);  play_note(F5, E);
//    silence();
//}
//
//// 1. Shooting Sound: A quick "Pew!" descending pitch
//void play_shoot(void) {
//    // Starts high and drops fast
//    for (uint16_t f = 1000; f > 400; f -= 40) {
//        tone(f);
//        _delay_ms(10);
//    }
//    silence();
//}
//
//// 2. Enemy Dying: A stuttering, descending "crash" sound
//void play_enemy_death(void) {
//    // Descends with "stutter" gaps to sound like an explosion/crunch
//    for (uint16_t f = 600; f > 150; f -= 30) {
//        tone(f);
//        _delay_ms(20);
//        silence(); 
//        _delay_ms(5);
//    }
//}
//
//// 3. Player Damage: Two sharp, dissonant "Ouch" tones
//void play_player_damage(void) {
//    // Sharp high tone followed by a lower impact tone
//    tone(880); // A5
//    _delay_ms(100);
//    tone(440); // A4 (lower octave)
//    _delay_ms(200);
//    silence();
//}
//


#include "sound.h"

void sound_init(void) {
    // Set PD5 (OC0B) as output
    DDRD |= (1 << PD5);
}

void silence() {
    // Disconnect OC0B from the physical pin
    TCCR0A &= ~(1 << COM0B0);
}

void tone(uint16_t freq) {
    if (freq == REST || freq == 0) {
        silence();
        return;
    }

    // Ensure PD5 is output
    DDRD |= (1 << PD5);

    // TCCR0A: 
    // COM0B0 = 1 -> Toggle OC0B on Compare Match
    // WGM01 = 1  -> CTC Mode (Clear Timer on Compare)
    TCCR0A = (1 << COM0B0) | (1 << WGM01);

    // TCCR0B: 
    // CS02 and CS00 = 1 -> Prescaler 1024
    TCCR0B = (1 << CS02) | (1 << CS00);

    // Math for 8-bit Timer 0:
    // OCR0A defines the frequency (the "top" of the count)
    // Note: In WGM mode 2 (CTC), OCR0A is the top, even if we toggle OC0B
    uint32_t ocr_val = (16000000UL / (2UL * 1024UL * freq)) - 1;

    // Cap the value at 255 (8-bit limit)
    if (ocr_val > 255) ocr_val = 255;
    
    OCR0A = (uint8_t)ocr_val;
}

// Sound Effects (using the tone function above)
void play_shoot(void) {
    for (uint16_t f = 1000; f > 400; f -= 40) {
        tone(f);
        _delay_ms(10);
    }
    silence();
}

void play_enemy_death(void) {
    for (uint16_t f = 600; f > 150; f -= 30) {
        tone(f);
        _delay_ms(20);
        silence(); 
        _delay_ms(5);
    }
}

void play_player_damage(void) {
    tone(880); 
    _delay_ms(100);
    tone(440); 
    _delay_ms(200);
    silence();
}