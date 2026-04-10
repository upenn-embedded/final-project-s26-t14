#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ST7735.h"
#include "LCD_GFX.h"
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
    int vx;
    int vy;
    int size;
    int isAlive;
    uint16_t color;
} Enemy;

/* Definitions */
#define xp PC0
#define yp PC1
#define xdim 160 // change xdim & ydim based on the size of the screen
#define ydim 128

Player player;
Enemy enemy;
int cursorX;
int cursorY;

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
    player.size = 12;
    player.color = 0x0000;
    player.health = 3;

    LCD_drawCircle(player.xpos, player.ypos, player.size, player.color);
}

void cursor_init() {
    cursorX = xdim/2;
    cursorY = ydim/2;
    LCD_drawChar_Transparent(cursorX, cursorY, '+', RED);
}

void enemy_init() {
    enemy.xpos = 50;
    enemy.ypos = 50;
    enemy.size = 10;
    enemy.isAlive = 1;
    enemy.color = BLUE;
    LCD_drawBlock(enemy.xpos - (enemy.size/2), enemy.ypos - (enemy.size/2), enemy.xpos + (enemy.size/2), enemy.ypos + (enemy.size/2), enemy.color);
}

void Initialize() {
    ADC_Init();

    lcd_init();
    LCD_setScreen(0xFFFF);
    player_init();
    cursor_init();
    enemy_init();
}

int ADC_Read(char channel)
{
    int Ain, AinLow;
    
    // Clear the channel selection bits (0-3) before setting the new channel
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    ADCSRA |= (1<<ADSC);        /* Start conversion */
    while((ADCSRA&(1<<ADIF))==0);    /* Monitor end of conversion interrupt */
    
    _delay_us(10);
    AinLow = (int)ADCL;        /* Read lower byte*/
    Ain = (int)ADCH*256;        /* Read higher byte */
    Ain = Ain + AinLow;                
    return(Ain);            /* Return digital value*/
}

void update_player() {
    // TODO
    LCD_drawCircle(player.xpos, player.ypos, player.size, player.color);
}

void update_enemy() {
    // TODO
    int half = enemy.size / 2;

    // Erase current position
    LCD_drawBlock(enemy.xpos - half, enemy.ypos - half, 
                  enemy.xpos + half, enemy.ypos + half, 0xFFFF);

    // Occasionally change direction 
    if (rand() % 20 == 0) {
        enemy.vx = (rand() % 5) - 2; 
        enemy.vy = (rand() % 5) - 2;
    }

    // Update position
    int nextX = enemy.xpos + enemy.vx;
    int nextY = enemy.ypos + enemy.vy;

    // 4. Boundary Check
    if (nextX < half || nextX > (xdim - half)) {
        enemy.vx = -enemy.vx; // Reverse direction
    } else {
        enemy.xpos = nextX;
    }

    if (nextY < half || nextY > (ydim - half)) {
        enemy.vy = -enemy.vy; // Reverse direction
    } else {
        enemy.ypos = nextY;
    }

    // Draw new position
    LCD_drawBlock(enemy.xpos - half, enemy.ypos - half, 
                  enemy.xpos + half, enemy.ypos + half, enemy.color);
}

void update_cursor() {
    // TODO
    // 1. Erase the OLD cursor first using the background color
    LCD_drawChar_Transparent(cursorX, cursorY, '+', 0xFFFF);

    // 2. Read ADC and update coordinates
    int xval = ADC_Read(0);
    int yval = ADC_Read(1);
    
    
    if (xval >= 800) {
        cursorX += 5;
    } else if (xval <= 400) {
        cursorX -= 5;
    }

    if (yval >= 800) {
        cursorY += 5;
    } else if (yval <= 400) {
        cursorY -= 5;
    }

    // Keep cursor within screen bounds (Screen is 160x128)
    if (cursorX < 0) {
        cursorX = 0;
    }
    if (cursorX > 150) {
        cursorX = 150;
    } // Leave room for char width
    if (cursorY < 0) {
        cursorY = 0;
    }
    if (cursorY > 120) {
        cursorY = 120;
    } // Leave room for char height

    // 4. Draw the NEW cursor
    LCD_drawChar_Transparent(cursorX, cursorY, '+', RED);
}

int main(void) {
    // TODO
    Initialize();
    while(1) {
        update_player();
        update_cursor();
        update_enemy();
        _delay_ms(20);
    }
}