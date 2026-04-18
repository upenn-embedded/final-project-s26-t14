#include "HX8357.h"
#include <util/delay.h>

static void SPI_Controller_Init(void) {
    // Setup SPI pins: MOSI, SCK, CS, DC, RST as output
    LCD_DDR |= (1<<LCD_MOSI)|(1<<LCD_SCK)|(1<<LCD_TFT_CS)|(1<<LCD_DC)|(1<<LCD_RST);
    
    SPCR0 = (1<<SPE) | (1<<MSTR);
    SPSR0 = (1<<SPI2X);
}

void SPI_ControllerTx_stream(uint8_t data) {
    SPDR0 = data;
    while(!(SPSR0 & (1<<SPIF)));
}

static void sendCommand(uint8_t cmd) {
    clear(LCD_PORT, LCD_DC);
    clear(LCD_PORT, LCD_TFT_CS);
    SPI_ControllerTx_stream(cmd);
    set(LCD_PORT, LCD_TFT_CS);
}

static void sendData(uint8_t data) {
    set(LCD_PORT, LCD_DC);
    clear(LCD_PORT, LCD_TFT_CS);
    SPI_ControllerTx_stream(data);
    set(LCD_PORT, LCD_TFT_CS);
}

void LCD_rotate(uint8_t rotation) {
    sendCommand(HX8357_MADCTL);
    uint8_t madctl_val = 0;

    switch (rotation % 4) {
        case 0: // Portrait
            madctl_val = MADCTL_BGR;
            break;
        case 1: // Landscape (Top-Left Origin, Y increases DOWN)
            madctl_val = MADCTL_MV | MADCTL_BGR; // Total: 0x28
            break;
        case 2: // Portrait (Upside Down)
            madctl_val = MADCTL_MY | MADCTL_MX | MADCTL_BGR;
            break;
        case 3: // Landscape 
            madctl_val = MADCTL_MV | MADCTL_MY | MADCTL_MX | MADCTL_BGR;
            break;
    }
    sendData(madctl_val);
}

void LCD_init(void) {
    SPI_Controller_Init();

    // Hardware Reset
    clear(LCD_PORT, LCD_RST);
    _delay_ms(50);
    set(LCD_PORT, LCD_RST);
    _delay_ms(150);

    sendCommand(HX8357_SWRESET);
    _delay_ms(150);

    sendCommand(HX8357_SLPOUT); // Exit Sleep 
    _delay_ms(150);

    sendCommand(HX8357_COLMOD); // Set Pixel Format 
    sendData(0x55);             // 16-bit color 

    sendCommand(HX8357_MADCTL); // Memory Access Control 
//    sendData(0x00);             // Default orientation
    sendData(0x08);

    sendCommand(HX8357_DISPON); // Display On 
    _delay_ms(50);

    // Set to Landscape mode immediately after turning display on
    LCD_rotate(1);
}

void LCD_setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    sendCommand(HX8357_CASET); // Column Address Set 
    sendData(x0 >> 8);
    sendData(x0 & 0xFF);
    sendData(x1 >> 8);
    sendData(x1 & 0xFF);

    sendCommand(HX8357_PASET); // Page Address Set 
    sendData(y0 >> 8);
    sendData(y0 & 0xFF);
    sendData(y1 >> 8);
    sendData(y1 & 0xFF);

    sendCommand(HX8357_RAMWR); // Memory Write 
}