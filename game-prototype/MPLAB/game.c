#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "HX8357.h"
#include "LCD_GFX.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sprites.h"
#include "aw9523.h"
#include "i2c.h"
#include "uart.h"
#include "imu.h"
#include "sound.h"


/* Structs */
typedef struct Player {
    int xpos;
    int ypos;
    int size;
    int health;
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
#define maxEnemy 8 // 5 * 10 round
#define ammoCount 10
#define healthCount 6

Player player;
Enemy enemies[maxEnemy];
int cursorX;
int cursorY;
volatile int game_started = 0;
volatile int shoot_pressed = 0;
volatile int shaken = 0; 
//uint8_t ammo_pins[ammoCount] = {PORTD5, PORTD4, PORTD3};
//uint8_t health_pins[healthCount] = {PORTD5, PORTD4, PORTD3};
volatile int ammo;
int gameRound = 1;
int enemiesDead = 0;
int changeRound = 0;
volatile int blinded = 0;


ISR(INT0_vect) {
    if (!game_started) {
        game_started = 1; // Set flag when button is pressed
    } else {
        shoot_pressed = 1;
    }
}

ISR(INT1_vect) {
    if (game_started) {
        ammo = ammoCount;
    }
}

ISR(PCINT1_vect) {
    // Only trigger if the pin is currently HIGH (the start of the pulse)
    if (PINC & (1 << PC3)) {
        if (blinded) {
            shaken = 1;
            printf("SHAKEN!!!");
        }
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
    player.size = 30;
    player.health = healthCount;

    LCD_drawSprite(player.xpos, player.ypos, player.size, player.size, player_sprite);
}

void cursor_init() {
    cursorX = xdim/2;
    cursorY = ydim/2;

    LCD_drawCursor(cursorX, cursorY, RED);
}


void enemy_init() {
      for (int i = 0; i < maxEnemy; i++) {
        enemies[i].xpos = (rand() % xdim - 60 + 1) + 30;
        enemies[i].ypos = (rand() % ydim - 60 + 1) + 30;
        while (enemies[i].xpos >= (xdim / 2) - 30 && enemies[i].xpos <= (xdim / 2) + 30 && enemies[i].ypos >= (ydim / 2) - 30 && enemies[i].ypos <= (ydim / 2) + 30) {
            enemies[i].xpos = (rand() % xdim - 60 + 1) + 30;
            enemies[i].ypos = (rand() % ydim - 60 + 1) + 30;
        }
        enemies[i].size = 30;
        enemies[i].vx = 0;
        enemies[i].vy = 0;
        enemies[i].isAlive = 1;
        enemies[i].color = BLUE;

        LCD_drawSprite(enemies[i].xpos, enemies[i].ypos, enemies[i].size, enemies[i].size, enemy_sprite);
      }
}

void button_init(void) {
    // Set PD2 as input
    DDRD &= ~(1 << PD2);
    DDRD &= ~(1 << PD3);

    // Enable internal pull-up
    PORTD |= (1 << PORTD2);
    PORTD |= (1 << PORTD3);
    
    // Configure INT0 to trigger on Falling Edge (High to Low)
    // ISC01 = 1, ISC00 = 0 in EICRA register
    EICRA |= (1 << ISC01);
    EICRA |= (1 << ISC11);
    EICRA &= ~(1 << ISC00);
    EICRA &= ~(1 << ISC10);
    
    // Enable the INT0 external interrupt mask
    EIMSK |= (1 << INT0);
    EIMSK |= (1 << INT1);
    
    // TURN ON GLOBAL INTERRUPTS
    sei(); 
}

void imu_init() {
    // 1. WAKE UP: Register 0x6B (PWR_MGMT_1) -> 0x00
    
    i2c_writeRegister(MPU6050_ADDR, 0x6B, 0x01); // 0x01 instead of 0x00

    // 2. CONFIG MOTION: Register 0x1F (Threshold), 0x20 (Duration)
    i2c_writeRegister(MPU6050_ADDR, 0x1F, 35);
    i2c_writeRegister(MPU6050_ADDR, 0x20, 5);  // 5ms duration
    
    // 2.5 ACCEL CONFIG (The "Tilt" Secret)
    // 0x02 sets a 1.25Hz High Pass Filter - better for detecting slower swaying
    i2c_writeRegister(MPU6050_ADDR, 0x1C, 0x02);

    // 3. INT PIN CFG: Register 0x37 
    // Set to 0x00: Active High, Push-Pull, 50us Pulse (No latching)
    // This is better for PCINT because it returns to 0 automatically.
    i2c_writeRegister(MPU6050_ADDR, 0x37, 0x00); 

    // 4. ENABLE INT: Register 0x38 -> 0x40 (Motion Enable)
    i2c_writeRegister(MPU6050_ADDR, 0x38, 0x40);

    // 5. AVR PCINT SETUP (PC3 / PCINT11)
    DDRC &= ~(1 << PC3);    // Input
    PORTC &= ~(1 << PC3);   
    
    PCMSK1 |= (1 << PCINT11);
    PCICR  |= (1 << PCIE1);
    
    sei(); 
}

void game_init() {
    LCD_setScreen(WHITE);
    ammo = ammoCount;

    player_init();
    cursor_init();
    enemy_init();
}

void restart_round() {
    LCD_setScreen(WHITE);
    ammo = ammoCount;

    player_init();
    cursor_init();
    enemy_init();
}

void start_init() {
    LCD_setScreen(BLACK);
    
    LCD_drawString(160, 150, "doomsDAY", WHITE, BLACK);
    LCD_drawString(160, 250, "PRESS BUTTON TO START", WHITE, BLACK);
}

void end_screen() {
    LCD_setScreen(BLACK);
    
    LCD_drawString(240, 160, "You lost :(", WHITE, BLACK);
}

void change_round_screen(char text[]) {
    LCD_setScreen(BLACK);
    
    LCD_drawString(230, 160, text, WHITE, BLACK);
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
    if (!blinded) {
        LCD_drawSprite(player.xpos, player.ypos, player.size, player.size, player_sprite);
    }
    if (player.health == 0 && game_started) {
        game_started = 0;
    }
}

void play_shoot_sound() {
    PORTD &= ~(1 << PD4);
    _delay_ms(20);
    PORTD |= (1 << PD4);
}

void play_enemy_death_sound() {
    PORTD |= (1 << PD7);
    _delay_ms(20);
    PORTD &= ~(1 << PD7);
}



void update_enemies() {
    int count = 0;
    for (int i = 0; i < maxEnemy; i++) {
        int half = enemies[i].size / 2;
    
        if (enemies[i].isAlive) {
            int oldX = enemies[i].xpos;
            int oldY = enemies[i].ypos;
            
            // Update movement logic
            if (rand() % 20 == 0) {
                enemies[i].vx = ((rand() % 7) - 3) * gameRound; 
                enemies[i].vy = ((rand() % 7) - 3) * gameRound;
            }

            enemies[i].xpos += enemies[i].vx;
            enemies[i].ypos += enemies[i].vy;
            
            if ((enemies[i].xpos - half >= player.xpos - (player.size / 2)  && 
                 enemies[i].xpos - half <= player.xpos + (player.size / 2)  &&
                 enemies[i].ypos - half >= player.ypos - (player.size / 2)  &&
                 enemies[i].ypos - half <= player.ypos + (player.size / 2)) ||
                (enemies[i].xpos + half >= player.xpos - (player.size / 2)  && 
                 enemies[i].xpos + half <= player.xpos + (player.size / 2)  &&
                 enemies[i].ypos - half >= player.ypos - (player.size / 2)  &&
                 enemies[i].ypos - half <= player.ypos + (player.size / 2)) ||
                (enemies[i].xpos - half >= player.xpos - (player.size / 2)  && 
                 enemies[i].xpos - half <= player.xpos + (player.size / 2)  &&
                 enemies[i].ypos + half >= player.ypos - (player.size / 2)  &&
                 enemies[i].ypos + half <= player.ypos + (player.size / 2)) ||
                (enemies[i].xpos + half >= player.xpos - (player.size / 2)  && 
                 enemies[i].xpos + half <= player.xpos + (player.size / 2)  &&
                 enemies[i].ypos + half >= player.ypos - (player.size / 2)  &&
                 enemies[i].ypos + half <= player.ypos + (player.size / 2))) 
            {
                // Nudge the enemy back to the old position so they don't get stuck "inside" the player
                enemies[i].xpos = oldX;
                enemies[i].ypos = oldY;
                
                // BOUNCE: Reverse the enemy's velocity
                enemies[i].vx = -enemies[i].vx;
                enemies[i].vy = -enemies[i].vy;
                
                player.health--;
            }

            // Boundary Checks
            
            // Left and Right walls
            if (enemies[i].xpos < half) { 
                enemies[i].xpos = half; 
                enemies[i].vx = -enemies[i].vx; 
            }
            if (enemies[i].xpos + half > LCD_WIDTH) { 
                // Bounce when the RIGHT edge hits the wall
                enemies[i].xpos = LCD_WIDTH - half; 
                enemies[i].vx = -enemies[i].vx; 
            }

            // Top and Bottom walls
            if (enemies[i].ypos < half) { 
                enemies[i].ypos = half; 
                enemies[i].vy = -enemies[i].vy; 
            }
            if (enemies[i].ypos + half > LCD_HEIGHT) { 
                // Bounce when the BOTTOM edge hits the wall (LCD_HEIGHT - 30)
                enemies[i].ypos = LCD_HEIGHT - half; 
                enemies[i].vy = -enemies[i].vy; 
            }
            
            if (!blinded) {
                // Erase old position
                LCD_drawSprite(oldX, oldY, enemies[i].size, enemies[i].size, white_sprite);

                // Draw new position
                LCD_drawSprite(enemies[i].xpos, enemies[i].ypos, enemies[i].size, enemies[i].size, enemy_sprite);
            }
        } else {
            count++;
        }
    }
    
    enemiesDead = count;
    
    if (enemiesDead == maxEnemy) {
        changeRound = 1;
    }
}

void kill_enemy(Enemy *en){
    en->isAlive = 0;
    int half = en->size / 2;
    int blindChance = (rand() % 10) + 1;
    
    // Turn Red to indicate death
    LCD_drawBlock(en->xpos - half, en->ypos - half, 
                      en->xpos + half, en->ypos + half, RED);
    
    // Erase
    LCD_drawBlock(en->xpos - half, en->ypos - half, 
                      en->xpos + half, en->ypos + half, 0xFFFF);
    
    if (blindChance == 6) {
        blinded = 1;
    }
    
}

// Cursor controlled by joystick
//void update_cursor() {
//    // Erase using the background color (WHITE)
//    if (!blinded) {
//        LCD_drawCursor(cursorX, cursorY, 0xFFFF);
//    }
//
//    int xval = ADC_Read(0);
//    int yval = ADC_Read(1);
//    
//    // Increased speed for the larger screen
//    if (xval >= 800) cursorX += 8; 
//    else if (xval <= 400) cursorX -= 8;
//
//    if (yval >= 800) cursorY += 8;
//    else if (yval <= 400) cursorY -= 8;
//
//    // Bounds checking
//    if (cursorX < 0) cursorX = 0;
//    if (cursorX > (xdim - 6)) cursorX = xdim - 6; 
//    if (cursorY < 0) cursorY = 0;
//    if (cursorY > (ydim - 8)) cursorY = ydim - 8;
//
//    // Draw the NEW cursor
//    if (!blinded) {
//        LCD_drawCursor(cursorX, cursorY, RED);
//    }
//}


//// Cursor Controlled by Pot
void update_cursor() {
    // Erase the old cursor using the background color
    LCD_drawCursor(cursorX, cursorY, 0xFFFF);

    // Read Potentiometers
    uint16_t xval = ADC_Read(0); 
    uint16_t yval = ADC_Read(1); 
    printf("x val: %d", xval);
    printf("y val: %d", yval);

    // Map 0-1023 to Screen Dimensions
    cursorX = (uint32_t) (xval - 230) * (LCD_WIDTH - 1) / (530 - 230);
    cursorY = (uint32_t) (yval - 380) * (LCD_HEIGHT - 1) / (625-380);

    // Boundary Safety
    if (cursorX < 5) cursorX = 5;
    if (cursorX > (LCD_WIDTH - 6)) cursorX = LCD_WIDTH - 6; 
    if (cursorY < 5) cursorY = 5;
    if (cursorY > (LCD_HEIGHT - 6)) cursorY = LCD_HEIGHT - 6;

    // Draw the NEW cursor
    LCD_drawCursor(cursorX, cursorY, RED);
}

void update_health_leds() {
    for (int i = 0; i < healthCount; i++) {
        if (i + 1 <= player.health) {
            set_led(i, 1);
        } else {
            set_led(i, 0);
        }
    }
}

void update_ammo_leds() {
    for (int i = 0; i < ammoCount; i++) {
        if (i + 1 <= ammo) {

            set_led(i + 6, 1);
        } else {
            set_led(i + 6, 0);
        }
    }
}

void update_blinded_screen() {
    if (blinded) {
        LCD_setScreen(GREEN);
    
        LCD_drawString(160, 160, "YOU HAVE BEEN BLINDED! SHAKE TO SEE AGAIN!!", WHITE, BLACK);
    }
    if (shaken) {
        blinded = 0;
        shaken = 0;
        
        uint8_t dummy;
        i2c_readCompleteStream(&dummy, MPU6050_ADDR, 0x3A, 1);
        
        LCD_setScreen(WHITE);
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
    LCD_init();
    ADC_Init();
    button_init();
    uart_init();
    i2c_init();
    printf("Status: %d", aw9523_init());
    imu_init();
//    DDRD |= (1 << DDD4); // setup up PD4 as output (for speaker)
    DDRD |= (1 << DDD0); // second sound

    PORTD &= ~(1 << PD7);
    DDRC |= (1 << DDC2);
    PORTC &= ~(1 << PC2);
    
    
    while(1) {
        start_init();
    
        while (!game_started) {}
        game_init();
        
        while (game_started) {
            
            update_player();
            update_health_leds();
            update_cursor();
            update_enemies(player.xpos, player.ypos);
            if (shoot_pressed && ammo > 0) {
                PORTC |= (1 << PC2);
                for (int i = 0; i < maxEnemy; i++) {
                    if (enemies[i].isAlive && check_hit(cursorX, cursorY, enemies[i].xpos, enemies[i].ypos, enemies[i].size)) {
                        kill_enemy(&enemies[i]); // Pass pointer to specific enemy
                        break; // One shot, one kill
                    }
                }

                PORTC &= ~(1 << PC2);
                ammo--;
                shoot_pressed = 0;
            }
            update_ammo_leds();
            
            update_blinded_screen();
            
            if (changeRound) {
                gameRound++;
                char roundStringResult[30];
                snprintf(roundStringResult, sizeof(roundStringResult), "Round %d", gameRound);

                
                change_round_screen(roundStringResult);
                _delay_ms(1000);
                
                changeRound = 0;
                LCD_setScreen(WHITE);
                cursor_init();
                player_init();
                enemy_init();
            }
            
            _delay_ms(20);
        }
        
        end_screen();
        _delay_ms(1000); 
        game_started = 0;
        gameRound = 1;
    }
}