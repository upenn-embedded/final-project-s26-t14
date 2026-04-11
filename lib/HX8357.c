/**************************************************************************//**
* @file        ST7735.c
* @brief       Backward-compatible LCD driver source retargeted to HX8357D
*
* @details     Keeps the old file name so the rest of the project does not
*              need include-path changes.
*****************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "HX8357.h"

static void lcd_pin_init(void)
{
    // Control + SPI output pins
    LCD_DDR |= (1 << LCD_DC) | (1 << LCD_RST) | (1 << LCD_TFT_CS) |
               (1 << LCD_MOSI) | (1 << LCD_SCK);
    LCD_LITE_DDR |= (1 << LCD_LITE);

    // Idle states
    set(LCD_PORT, LCD_TFT_CS);
    set(LCD_PORT, LCD_DC);
    set(LCD_PORT, LCD_RST);

    // Backlight PWM on OC0A / PD6
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS02);    // clk / 256 => ~244 Hz at 16 MHz
    OCR0A = 200;             // brighter default

    // Hardware reset pulse
    _delay_ms(10);
    clear(LCD_PORT, LCD_RST);
    _delay_ms(20);
    set(LCD_PORT, LCD_RST);
    _delay_ms(150);
}

static void SPI_Controller_Init(void)
{
    // SPI master, fosc/4 with 2X => fosc/2 on AVR when possible.
    SPCR0 = (1 << SPE) | (1 << MSTR);
    SPSR0 = (1 << SPI2X);
}

static inline void LCD_writeCommand(uint8_t cmd)
{
    clear(LCD_PORT, LCD_TFT_CS);
    clear(LCD_PORT, LCD_DC);
    SPI_ControllerTx_stream(cmd);
    set(LCD_PORT, LCD_TFT_CS);
}

static inline void LCD_writeData(uint8_t data)
{
    clear(LCD_PORT, LCD_TFT_CS);
    set(LCD_PORT, LCD_DC);
    SPI_ControllerTx_stream(data);
    set(LCD_PORT, LCD_TFT_CS);
}

void Delay_ms(unsigned int n)
{
    while (n--) {
        _delay_ms(1);
    }
}

void SPI_ControllerTx(uint8_t data)
{
    clear(LCD_PORT, LCD_TFT_CS);
    SPI_ControllerTx_stream(data);
    set(LCD_PORT, LCD_TFT_CS);
}

void SPI_ControllerTx_stream(uint8_t stream)
{
    SPDR0 = stream;
    // Wait for the transmission to be 100% finished
    while (!(SPSR0 & (1 << SPIF)));
}

void SPI_ControllerTx_16bit(uint16_t data)
{
    clear(LCD_PORT, LCD_TFT_CS);
    set(LCD_PORT, LCD_DC);
    SPI_ControllerTx_stream((uint8_t)(data >> 8));
    SPI_ControllerTx_stream((uint8_t)data);
    set(LCD_PORT, LCD_TFT_CS);
}

void SPI_ControllerTx_16bit_stream(uint16_t data)
{
    SPI_ControllerTx_stream((uint8_t)(data >> 8));
    SPI_ControllerTx_stream((uint8_t)data);
}

void sendCommands(const uint8_t *cmds, uint8_t length)
{
    uint8_t numCommands = length;
    uint8_t numData;
    uint8_t waitTime;

    clear(LCD_PORT, LCD_TFT_CS);

    while (numCommands--) {
        clear(LCD_PORT, LCD_DC);
        SPI_ControllerTx_stream(*cmds++);

        numData = *cmds++;

        set(LCD_PORT, LCD_DC);
        while (numData--) {
            SPI_ControllerTx_stream(*cmds++);
        }

        waitTime = *cmds++;
        if (waitTime != 0U) {
            Delay_ms((waitTime == 255U) ? 500U : waitTime);
        }
    }

    set(LCD_PORT, LCD_TFT_CS);
}

void lcd_init(void)
{
    lcd_pin_init();
    SPI_Controller_Init();
    _delay_ms(10);

    // HX8357D SPI init sequence adapted from the HX8357D command set and the
    // widely used Adafruit HX8357D bring-up sequence.
    static const uint8_t HX8357_cmds[] = {
        HX8357_SWRESET,   0,                              150,
        HX8357D_SETC,     3, 0xFF, 0x83, 0x57,           10,
        HX8357_SETCOM,    1, 0x25,                        0,
        HX8357_SETOSC,    1, 0x68,                        0,
        HX8357_SETPANEL,  1, 0x05,                        0,
        HX8357_SETPWR1,   6, 0x00, 0x15, 0x1C, 0x1C, 0x83, 0xAA, 0,
        HX8357D_SETSTBA,  6, 0x50, 0x50, 0x01, 0x3C, 0x1E, 0x08, 0,
        HX8357_SETCYC,    7, 0x02, 0x40, 0x00, 0x2A, 0x2A, 0x0D, 0x78, 0,
        HX8357D_SETGAMMA, 34,
            0x02, 0x0A, 0x11, 0x1D, 0x23, 0x35, 0x41, 0x4B,
            0x4B, 0x42, 0x3A, 0x27, 0x1B, 0x08, 0x09, 0x03,
            0x02, 0x0A, 0x11, 0x1D, 0x23, 0x35, 0x41, 0x4B,
            0x4B, 0x42, 0x3A, 0x27, 0x1B, 0x08, 0x09, 0x03,
            0x00, 0x01,
            0,
        HX8357_COLMOD,    1, 0x55,                        10,
        HX8357_MADCTL,    1, 0xC0,                        0,
        HX8357_TEON,      1, 0x00,                        0,
        HX8357_TEARLINE,  2, 0x00, 0x02,                  0,
        HX8357_SLPOUT,    0,                              150,
        HX8357_NORON,     0,                              10,
        HX8357_DISPON,    0,                              50
    };

    sendCommands(HX8357_cmds, 16);
    LCD_rotate(0);
}

void LCD_setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    if (x0 >= LCD_WIDTH || x1 >= LCD_WIDTH || y0 >= LCD_HEIGHT || y1 >= LCD_HEIGHT) return;

    LCD_writeCommand(HX8357_CASET);
    LCD_writeData(x0 >> 8); LCD_writeData(x0 & 0xFF);
    LCD_writeData(x1 >> 8); LCD_writeData(x1 & 0xFF);

    LCD_writeCommand(HX8357_PASET);
    LCD_writeData(y0 >> 8); LCD_writeData(y0 & 0xFF);
    LCD_writeData(y1 >> 8); LCD_writeData(y1 & 0xFF);

    LCD_writeCommand(HX8357_RAMWR);
    // CS is set HIGH by LCD_writeCommand, which is good. 
    // It creates a 'break' before the bulk data starts.
}

void LCD_brightness(uint8_t intensity)
{
    OCR0A = intensity;
}

void LCD_rotate(uint8_t r)
{
    uint8_t madctl;

    switch (r & 3U) {
        case 0:
            madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB;
            break;
        case 1:
            madctl = MADCTL_MV | MADCTL_MY | MADCTL_RGB;
            break;
        case 2:
            madctl = MADCTL_RGB;
            break;
        default:
            madctl = MADCTL_MX | MADCTL_MV | MADCTL_RGB;
            break;
    }

    LCD_writeCommand(HX8357_MADCTL);
    LCD_writeData(madctl);
}
