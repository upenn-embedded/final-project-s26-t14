/**
 * @file    HX8357.h
 * @brief   Bare-metal C driver for the HX8357D 3.5" TFT (320x480, RGB565)
 *          adapated from https://github.com/adafruit/Adafruit_HX8357_Library/tree/master
 *          to work with current LCD_GFX.c code
 *
 *
 * 
 */

#ifndef HX8357D_H_
#define HX8357D_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>

/* ── Pin definitions ────────────────────────────────────────────────────── */

#define LCD_PORT        PORTB
#define LCD_DDR         DDRB
#define LCD_MOSI        PB3
#define LCD_SCK         PB5
#define LCD_TFT_CS      PB2

#define LCD_DC_PORT     PORTB
#define LCD_DC_DDR      DDRB
#define LCD_DC          PB0

#define LCD_RST_PORT    PORTB
#define LCD_RST_DDR     DDRB
#define LCD_RST         PB1

/* ── Pin toggle macros (same names LCD_GFX / game code may use) ─────────── */

#define set(reg, bit)    ((reg) |=  (1u << (bit)))
#define clear(reg, bit)  ((reg) &= ~(1u << (bit)))
#define toggle(reg, bit) ((reg) ^=  (1u << (bit)))

/* ── Display dimensions ─────────────────────────────────────────────────── */
/*    Replaces the ST7735 160×128 values that LCD_GFX.c uses for clipping.  */

#define LCD_WIDTH   320
#define LCD_HEIGHT  480
#define LCD_SIZE    (LCD_WIDTH * LCD_HEIGHT)

/* ── HX8357D register addresses ─────────────────────────────────────────── */

#define HX8357_SWRESET   0x01u
#define HX8357_SLPOUT    0x11u
#define HX8357_INVOFF    0x20u
#define HX8357_INVON     0x21u
#define HX8357_DISPOFF   0x28u
#define HX8357_DISPON    0x29u
#define HX8357_CASET     0x2Au
#define HX8357_PASET     0x2Bu
#define HX8357_RAMWR     0x2Cu
#define HX8357_MADCTL    0x36u
#define HX8357_COLMOD    0x3Au
#define HX8357_TEON      0x35u
#define HX8357_TEARLINE  0x44u
#define HX8357_SETOSC    0xB0u
#define HX8357_SETPWR1   0xB1u
#define HX8357_SETRGB    0xB3u
#define HX8357D_SETCOM   0xB6u
#define HX8357D_SETCYC   0xB4u
#define HX8357D_SETC     0xB9u
#define HX8357D_SETSTBA  0xC0u
#define HX8357_SETPANEL  0xCCu
#define HX8357D_SETGAMMA 0xE0u

/* MADCTL bits */
#define MADCTL_MY  0x80u
#define MADCTL_MX  0x40u
#define MADCTL_MV  0x20u
#define MADCTL_ML  0x10u
#define MADCTL_RGB 0x00u
#define MADCTL_BGR 0x08u
#define MADCTL_MH  0x04u

/* ── Public API ─────────────────────────────────────────────────────────── */

/* Initialise hardware SPI + display controller. Call once at startup. */
void lcd_init(void);

/* Set the pixel address window. Next SPI_ControllerTx_16bit calls fill it. */
void LCD_setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/* Set display rotation: 0–3 */
void LCD_rotate(uint8_t r);

/* SPI transmit helpers — same signatures as ST7735.h */
void SPI_ControllerTx(uint8_t data);
void SPI_ControllerTx_stream(uint8_t stream);
void SPI_ControllerTx_16bit(uint16_t data);
void SPI_ControllerTx_16bit_stream(uint16_t data);

/* Millisecond delay */
void Delay_ms(unsigned int n);

#endif /* HX8357D_H_ */