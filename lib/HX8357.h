#ifndef HX8357_H_
#define HX8357_H_

#include <avr/io.h>

// Screen Dimensions
#define LCD_WIDTH  320
#define LCD_HEIGHT 480

// Pin Definitions (Modify based on your specific wiring)
#define LCD_PORT    PORTB
#define LCD_DDR     DDRB
#define LCD_DC      PORTB0  // Data/Command
#define LCD_RST     PORTB1  // Reset
#define LCD_TFT_CS  PORTB2  // Chip Select
#define LCD_MOSI    PORTB3  // SPI MOSI
#define LCD_SCK     PORTB5  // SPI Clock

// HX8357 Commands [cite: 23, 31]
#define HX8357_SWRESET 0x01
#define HX8357_SLPOUT  0x11
#define HX8357_COLMOD  0x3A
#define HX8357_MADCTL  0x36
#define HX8357_CASET   0x2A
#define HX8357_PASET   0x2B
#define HX8357_RAMWR   0x2C
#define HX8357_DISPON  0x29

// Function Prototypes
void LCD_init(void);
void LCD_setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void SPI_ControllerTx_stream(uint8_t data);

// Helper Macros
#define set(port, pin)    (port |= (1 << pin))
#define clear(port, pin)  (port &= ~(1 << pin))

#endif