//#ifndef SOUND_H
//#define SOUND_H
//
//#define B4   494
//#define C5   523
//#define D5   587
//#define Ds5  622
//#define E5   659
//#define F5   698
//#define G5   784
//#define A5   880
//#define B5   988
//#define C6   1047
//#define REST 0
//
//#define E  1000
//#define S  E / 2
//#define DQ S * 6
//
//void silence();
//void tone(uint16_t freq);
//void play_note(uint16_t freq, uint16_t duration_ms);
//void pacman_theme();
//
//void play_shoot(void);
//void play_enemy_death(void);
//void play_player_damage(void);
//
//#endif

#ifndef SOUND_H
#define SOUND_H

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define REST 0

void sound_init(void);
void tone(uint16_t freq);
void silence(void);
void play_shoot(void);
void play_enemy_death(void);
void play_player_damage(void);

#endif