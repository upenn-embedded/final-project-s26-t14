/**************************************************************************//**
* @file        ST7735.h
* @brief       Backward-compatible LCD driver header retargeted to HX8357D
*
* @details     Keeps the old project include name (ST7735.h) so existing
*              application code still builds, but the register set and screen
*              geometry now target the HX8357-D controller.
*****************************************************************************/

#include <avr/io.h>
#include <stdint.h>

#ifndef ST7735_H_
#define ST7735_H_

#define LCD_PORT        PORTB
#define LCD_DDR         DDRB
#define LCD_DC          PORTB0
#define LCD_RST         PORTB1
#define LCD_TFT_CS      PORTB2
#define LCD_MOSI        PORTB3
#define LCD_SCK         PORTB5

// LCD_LITE must be connected to PD6 / OC0A for PWM brightness control.
#define LCD_LITE_PORT   PORTD
#define LCD_LITE_DDR    DDRD
#define LCD_LITE        PORTD6

#define LCD_WIDTH       320U
#define LCD_HEIGHT      480U
#define LCD_SIZE        ((uint32_t)LCD_WIDTH * (uint32_t)LCD_HEIGHT)

// HX8357 / MIPI-DCS commands used by this driver
#define HX8357_NOP      0x00
#define HX8357_SWRESET  0x01
#define HX8357_SLPOUT   0x11
#define HX8357_NORON    0x13
#define HX8357_INVOFF   0x20
#define HX8357_INVON    0x21
#define HX8357_DISPOFF  0x28
#define HX8357_DISPON   0x29
#define HX8357_CASET    0x2A
#define HX8357_PASET    0x2B
#define HX8357_RAMWR    0x2C
#define HX8357_MADCTL   0x36
#define HX8357_COLMOD   0x3A
#define HX8357_TEON     0x35
#define HX8357_TEARLINE 0x44

#define HX8357_SETOSC   0xB0
#define HX8357_SETPWR1  0xB1
#define HX8357_SETDISP  0xB2
#define HX8357_SETRGB   0xB3
#define HX8357_SETCYC   0xB4
#define HX8357_SETCOM   0xB6
#define HX8357D_SETC    0xB9
#define HX8357D_SETSTBA 0xC0
#define HX8357_SETPANEL 0xCC
#define HX8357D_SETGAMMA 0xE0

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

// Macro functions
#define set(reg,bit)    ((reg) |= (1U << (bit)))
#define clear(reg,bit)  ((reg) &= ~(1U << (bit)))
#define toggle(reg,bit) ((reg) ^= (1U << (bit)))

void Delay_ms(unsigned int n);
void lcd_init(void);
void sendCommands(const uint8_t *cmds, uint8_t length);
void LCD_setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void SPI_ControllerTx(uint8_t data);
void SPI_ControllerTx_stream(uint8_t stream);
void SPI_ControllerTx_16bit(uint16_t data);
void SPI_ControllerTx_16bit_stream(uint16_t data);
void LCD_brightness(uint8_t intensity);
void LCD_rotate(uint8_t r);

#endif /* ST7735_H_ */
