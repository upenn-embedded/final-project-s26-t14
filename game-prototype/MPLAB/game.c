#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include "ST7735.h"
#include "HX8357.h"
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
#define xdim LCD_WIDTH // change xdim & ydim based on the size of the screen
#define ydim LCD_HEIGHT
#define maxEnemy 5
#define ammoCount 3

Player player;
Enemy enemies[maxEnemy];
int cursorX;
int cursorY;
volatile int game_started = 0;
volatile int shoot_pressed = 0;
uint8_t ammo_pins[ammoCount] = {PORTD3, PORTD4, PORTD5};
int ammo;


ISR(INT0_vect) {
    if (!game_started) {
        game_started = 1; // Set flag when button is pressed
    } else {
        shoot_pressed = 1;
    }
}

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
//    LCD_drawChar_Transparent(cursorX, cursorY, '+', RED);
    LCD_drawCursor(cursorX, cursorY, RED);
}

void enemy_init() {
//    enemy.xpos = 50;
//    enemy.ypos = 50;
//    enemy.size = 20;
//    enemy.isAlive = 1;
//    enemy.color = BLUE;
//    LCD_drawBlock(enemy.xpos - (enemy.size/2), enemy.ypos - (enemy.size/2), enemy.xpos + (enemy.size/2), enemy.ypos + (enemy.size/2), enemy.color);
      for (int i = 0; i < maxEnemy; i++) {
//        enemies[i].isAlive = 0;
        enemies[i].xpos = 50;
        enemies[i].ypos = 50;
        enemies[i].size = 20;
        enemies[i].isAlive = 1;
        enemies[i].color = BLUE;
        LCD_drawBlock(enemies[i].xpos - (enemies[i].size/2), enemies[i].ypos - (enemies[i].size/2), enemies[i].xpos + (enemies[i].size/2), enemies[i].ypos + (enemies[i].size/2), enemies[i].color);
      }
}

void button_init(void) {
    // Set PD2 as input
    DDRD &= ~(1 << DDD2);
    // Enable internal pull-up (This is why we connect button to GND)
    PORTD |= (1 << PORTD2);
    
    // Configure INT0 to trigger on Falling Edge (High to Low)
    // ISC01 = 1, ISC00 = 0 in EICRA register
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    
    // Enable the INT0 external interrupt mask
    EIMSK |= (1 << INT0);
    
    // TURN ON GLOBAL INTERRUPTS
    sei(); 
}

//void spawn_enemy(int playerX, int playerY) {
//    for (int i = 0; i < maxEnemy; i++) {
//        if (!enemies[i].isAlive) {
//            enemies[i].isAlive = 1;
//            enemies[i].size = 20;
//            enemies[i].color = BLUE;
//
//            // Pick a random side: 0=Top, 1=Bottom, 2=Left, 3=Right
//            int side = rand() % 4;
//            if (side == 0) { enemies[i].xpos = rand() % LCD_WIDTH; enemies[i].ypos = 0; }
//            else if (side == 1) { enemies[i].xpos = rand() % LCD_WIDTH; enemies[i].ypos = LCD_HEIGHT; }
//            else if (side == 2) { enemies[i].xpos = 0; enemies[i].ypos = rand() % LCD_HEIGHT; }
//            else { enemies[i].xpos = LCD_WIDTH; enemies[i].ypos = rand() % LCD_HEIGHT; }
//            
//            // Calculate simple velocity toward player
//            enemies[i].vx = (enemies[i].xpos < playerX) ? 3 : -3;
//            enemies[i].vy = (enemies[i].ypos < playerY) ? 3 : -3;
//            
//            return; // Only spawn one at a time
//        }
//    }
//}

void led_init(void) {
    // Set ammo pins as outputs
    for (int i = 0; i < ammoCount; i++) {
        DDRD |= (1 << ammo_pins[i]);
    }
}

void game_init() {
    LCD_setScreen(WHITE);
    ADC_Init();
    ammo = ammoCount;

    led_init();
    player_init();
    cursor_init();
    enemy_init();
//    spawn_enemy(player.xpos, player.ypos);
}

void start_init() {
    LCD_setScreen(BLACK);
    
    LCD_drawString(160, 150, "doomsDAY", WHITE, BLACK);
    LCD_drawString(160, 250, "PRESS BUTTON TO START", WHITE, BLACK);
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

//void update_enemy() {
//    int half = enemy.size / 2;
//    
//    if (enemy.isAlive) {
//        // 1. Erase the OLD position BEFORE updating coordinates
//        // Use a slightly larger block (half + 1) to ensure no "streaks" are left
//        LCD_drawBlock(enemy.xpos - half, enemy.ypos - half, 
//                      enemy.xpos + half, enemy.ypos + half, 0xFFFF);
//
//        // 2. Update movement logic
//        if (rand() % 20 == 0) {
//            enemy.vx = (rand() % 7) - 3; 
//            enemy.vy = (rand() % 7) - 3;
//        }
//
//        enemy.xpos += enemy.vx;
//        enemy.ypos += enemy.vy;
//
//        // 3. Boundary Check (using the new 320x480 dimensions)
//        if (enemy.xpos < half) { enemy.xpos = half; enemy.vx = -enemy.vx; }
//        if (enemy.xpos > (LCD_WIDTH - half)) { enemy.xpos = LCD_WIDTH - half; enemy.vx = -enemy.vx; }
//        if (enemy.ypos < half) { enemy.ypos = half; enemy.vy = -enemy.vy; }
//        if (enemy.ypos > (LCD_HEIGHT - half)) { enemy.ypos = LCD_HEIGHT - half; enemy.vy = -enemy.vy; }
//
//        // 4. Draw new position
//        LCD_drawBlock(enemy.xpos - half, enemy.ypos - half, 
//                      enemy.xpos + half, enemy.ypos + half, enemy.color);
//    }
//}

void update_enemies() {
    for (int i = 0; i < maxEnemy; i++) {
        int half = enemies[i].size / 2;
    
        if (enemies[i].isAlive) {
            // 1. Erase the OLD position BEFORE updating coordinates
            // Use a slightly larger block (half + 1) to ensure no "streaks" are left
            LCD_drawBlock(enemies[i].xpos - half, enemies[i].ypos - half, 
                          enemies[i].xpos + half, enemies[i].ypos + half, 0xFFFF);

            // 2. Update movement logic
            if (rand() % 20 == 0) {
                enemies[i].vx = (rand() % 7) - 3; 
                enemies[i].vy = (rand() % 7) - 3;
            }

            enemies[i].xpos += enemies[i].vx;
            enemies[i].ypos += enemies[i].vy;

            // 3. Boundary Check (using the new 320x480 dimensions)
            if (enemies[i].xpos < half) { enemies[i].xpos = half; enemies[i].vx = -enemies[i].vx; }
            if (enemies[i].xpos > (LCD_WIDTH - half)) { enemies[i].xpos = LCD_WIDTH - half; enemies[i].vx = -enemies[i].vx; }
            if (enemies[i].ypos < half) { enemies[i].ypos = half; enemies[i].vy = -enemies[i].vy; }
            if (enemies[i].ypos > (LCD_HEIGHT - half)) { enemies[i].ypos = LCD_HEIGHT - half; enemies[i].vy = -enemies[i].vy; }

            // 4. Draw new position
            LCD_drawBlock(enemies[i].xpos - half, enemies[i].ypos - half, 
                          enemies[i].xpos + half, enemies[i].ypos + half, enemies[i].color);
        }
    }
}

//void update_enemies(int pX, int pY) {
//    for (int i = 0; i < maxEnemy; i++) {
//        if (enemies[i].isAlive) {
//            int half = enemies[i].size / 2;
//            
//            // 1. Erase
//            LCD_drawBlock(enemies[i].xpos - half, enemies[i].ypos - half, 
//                          enemies[i].xpos + half, enemies[i].ypos + half, WHITE);
//
//            // 2. Move toward player (Simple tracking)
//            enemies[i].xpos += enemies[i].vx;
//            enemies[i].ypos += enemies[i].vy;
//
//            // 3. Draw
//            LCD_drawBlock(enemies[i].xpos - half, enemies[i].ypos - half, 
//                          enemies[i].xpos + half, enemies[i].ypos + half, enemies[i].color);
//            
//            // 4. Check for "Game Over" (Enemy touches player)
//            if (abs(enemies[i].xpos - pX) < 10 && abs(enemies[i].ypos - pY) < 10) {
//                game_started = 0; // Trigger reset/game over
//            }
//        }
//    }
//}

void kill_enemy(Enemy *en){
    en->isAlive = 0;
    int half = en->size / 2;
    
    // Turn Red to indicate death
    LCD_drawBlock(en->xpos - half, en->ypos - half, 
                      en->xpos + half, en->ypos + half, RED);
    
    // Erase
    
    LCD_drawBlock(en->xpos - half, en->ypos - half, 
                      en->xpos + half, en->ypos + half, 0xFFFF);
    
}

void update_cursor() {
    // 1. Erase using the background color (WHITE)
    LCD_drawCursor(cursorX, cursorY, 0xFFFF);

    int xval = ADC_Read(0);
    int yval = ADC_Read(1);
    
    // Increased speed for the larger screen
    if (xval >= 800) cursorX += 8; 
    else if (xval <= 400) cursorX -= 8;

    if (yval >= 800) cursorY += 8;
    else if (yval <= 400) cursorY -= 8;

    // Corrected bounds for 320x480 screen
    if (cursorX < 0) cursorX = 0;
    if (cursorX > (xdim - 6)) cursorX = xdim - 6; 
    if (cursorY < 0) cursorY = 0;
    if (cursorY > (ydim - 8)) cursorY = ydim - 8;

    // 4. Draw the NEW cursor
    LCD_drawCursor(cursorX, cursorY, RED);
}

void update_ammo_leds() {
    for (int i = 1; i < ammoCount + 1; i++) {
        if (i <= ammo) {
            // Turn ON LED (High)
            PORTD |= (1 << ammo_pins[i]);
        } else {
            // Turn OFF LED (Low)
            PORTD &= ~(1 << ammo_pins[i]);
        }
    }
}

uint8_t check_hit(uint16_t curX, uint16_t curY, uint16_t enX, uint16_t enY, uint16_t enSize) {
    int half = enSize / 2;
    // Check if cursor is within the bounds centered around enX, enY
    if ((curX >= (enX - half)) && (curX <= (enX + half)) && 
        (curY >= (enY - half)) && (curY <= (enY + half))) {
        return 1;
    }
    return 0;
}

int main(void) {
    // TODO
    LCD_init();
    button_init();
    
    while(1) {
        start_init();
    
        while (!game_started) {}
        game_init();
        
        while (game_started) {
            update_player();
            update_cursor();
//            update_enemy();
            update_enemies(player.xpos, player.ypos);
            if (shoot_pressed) {
                for (int i = 0; i < maxEnemy; i++) {
                    if (enemies[i].isAlive && check_hit(cursorX, cursorY, enemies[i].xpos, enemies[i].ypos, enemies[i].size)) {
                        kill_enemy(&enemies[i]); // Pass pointer to specific enemy
                        break; // One shot, one kill
                    }
                }
//                ammo--;
                shoot_pressed = 0;
            }
            update_ammo_leds();
            _delay_ms(20);
        }
    }
}