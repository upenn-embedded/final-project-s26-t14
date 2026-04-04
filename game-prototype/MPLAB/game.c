#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lib/ST7735.h"
#include "lib/LCD_GFX.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/* Structs */
typedef struct Player {
    int xpos;
    int ypos;
    int size;
    int health;
    uint16_t color;
} Player;

typedef struct Enemy {
    int xpos;
    int ypos;
    int size;
    int isAlive;
    uint16_t color;
} Enemy;

/* Definitions */
#define xp PC0
#define yp PC1
#define xdim 180 // change xdim & ydim based on the size of the screen
#define ydim 128

Player player;

/* Functions */
void ADC_Init()
{
    DDRC &= ~(1<<xp);
    DDRC &= ~(1<<yp);

    ADCSRA = 0x87;			/* Enable ADC, fr/128  */
	ADMUX = 0x40;			/* Vref: Avcc, ADC channel: 0 */
	
}

void player_init() {
    player.xpos = xdim / 2;
    player.ypos = ydim / 2;
    player.size = 20;
    player.color = 0x0000;
    player.health = 3;

    LCD_drawCircle(player.xpos, player.ypos, player.size, player.color);
}

void Initialize() {
    ADC_Init();

    lcd_init();
    LCD_setScreen(0xFFFF);
    player_init();
}

int ADC_Read(char channel)
{
	int Ain,AinLow;
	
	ADMUX=ADMUX|(channel & 0x0f);	/* Set input channel to read */

	ADCSRA |= (1<<ADSC);		/* Start conversion */
	while((ADCSRA&(1<<ADIF))==0);	/* Monitor end of conversion interrupt */
	
	_delay_us(10);
	AinLow = (int)ADCL;		/* Read lower byte*/
	Ain = (int)ADCH*256;		/* Read higher 2 bits and 
					Multiply with weight */
	Ain = Ain + AinLow;				
	return(Ain);			/* Return digital value*/
}

void update_player() {
    // TODO
}

void update_enemy() {
    // TODO
}

int main(void) {
    // TODO
}