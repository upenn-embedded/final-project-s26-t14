/*
 * File:   sound.c
 * Author: amarischen
 *
 * Created on April 10, 2026, 1:37 AM
 */

#include <avr/io.h>
#include <util/delay.h>
#include "sound.h"
#define F_CPU 16000000UL

void silence() {
    TCCR2A &= ~(1 << COM2B0);
}

void tone(uint16_t freq) {
    if (freq == REST || freq == 0) {
        TCCR2A &= ~(1 << COM2B0); // Disconnect Buzzer
        return;
    }
    
    TCCR2A = (1 << WGM21) | (1 << COM2B0);  // Toggle OC2B on Compare Match, CTC Mode
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);  // Prescaler 1024
    
    // (F_CPU / (2 * Prescaler * Freq)) - 1
    uint32_t ocr_val = (F_CPU / (2UL * 1024UL * freq)) - 1;

    if (ocr_val > 255) ocr_val = 255;
    OCR2A = (uint8_t) ocr_val;
}

void play_note(uint16_t freq, uint16_t duration_ms) {
    if (freq == REST) {
        silence();
        for(uint16_t i=0; i<duration_ms; i++) _delay_ms(1);
    } else {
        tone(freq);
        for(uint16_t i=0; i<(duration_ms * 9 / 10); i++) _delay_ms(1);
        silence();
        for(uint16_t i=0; i<(duration_ms / 10); i++) _delay_ms(1);
    }
}

void pacman_theme() {
    play_note(B4, E); play_note(B5, E); play_note(F5, E);  play_note(Ds5, E);
    play_note(B5, S); play_note(F5, DQ); play_note(E5, E);
    
    play_note(C5, E); play_note(C6, E); play_note(G5, E); play_note(E5, E);
    play_note(C6, S); play_note(G5, DQ);
    
    play_note(B4, E); play_note(B5, E); play_note(F5, E); play_note(Ds5, E);
    play_note(B5, S); play_note(F5, E); play_note(E5, E);
    
    play_note(E5, S);  play_note(F5, S);  play_note(F5, E);
    silence();
}

