#ifndef HX8357_H_
#define HX8357_H_

#include <avr/io.h>

// Screen Dimensions
#define LCD_WIDTH  480
#define LCD_HEIGHT 320

// MADCTL Bits for Orientation
#define MADCTL_MY  0x80 // Row Address Order (Y-mirror)
#define MADCTL_MX  0x40 // Column Address Order (X-mirror)
#define MADCTL_MV  0x20 // Row/Column Exchange (Rotation)
#define MADCTL_BGR 0x08 // BGR color filter panel

// Pin Definitions
#define LCD_PORT    PORTB
#define LCD_DDR     DDRB
#define LCD_DC      PORTB0  // Data/Command
#define LCD_RST     PORTB1  // Reset
#define LCD_TFT_CS  PORTB2  // Chip Select
#define LCD_MOSI    PORTB3  // SPI MOSI
#define LCD_SCK     PORTB5  // SPI Clock

// HX8357 Commands
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