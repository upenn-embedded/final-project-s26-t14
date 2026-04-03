#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lib/ST7735.h"
#include "lib/LCD_GFX.h"
#include <stdio.h>
#include <stdlib.h>


/* Structs */
typedef struct Player {
    Vector2 position;
    int size;
    int health;
    uint16_t color;
} Player;

typedef struct Enemy {
    Vector2 position;
    int size;
    int isAlive;
    uint16_t color;
} Enemy;

/* Definitions */
#define yp PC1
#define xp PC2

Player player;

/* Functions */
void ADC_Init()
{
    DDRC &= ~(1<<y_p);

    ADCSRA = 0x87;			/* Enable ADC, fr/128  */
	ADMUX = 0x40;			/* Vref: Avcc, ADC channel: 0 */
	
}

void player_init()

void Initialize() {
    ADC_Init();

    lcd_init();
    LCD_setScreen(0xFFFF);
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

void player() {
    // TODO
}

void enemy() {
    // TODO
}

int main(void) {
    // TODO
}