/**
 * @file    HX8357.c
 * @brief   Driver for the HX8357D 3.5" TFT (320x480, RGB565) adapted from https://github.com/adafruit/Adafruit_HX8357_Library/tree/master
 *          Drop-in replacement for ST7735.c
 *
 *
 */

#include "HX8357.h"

/* ═══════════════════════════════════════════════════════════════════════════
   INTERNAL SPI PRIMITIVES
   All SPI on the 328PB uses the primary SPI peripheral (SPCR/SPSR/SPDR).
   The old ST7735 driver used SPCR0/SPSR0/SPDR0 — same registers, just the
   328PB headers expose both names.  We use the base names here for clarity.
   ═══════════════════════════════════════════════════════════════════════════ */

/* Send one byte and block until the transfer is complete. */
static inline void spi_tx(uint8_t b)
{
    SPDR = b;
    while (!(SPSR & (1u << SPIF)));
}

/* ═══════════════════════════════════════════════════════════════════════════
   PUBLIC SPI WRAPPERS
   Preserve the exact signatures from ST7735.h so LCD_GFX.c compiles
   without modification.
   ═══════════════════════════════════════════════════════════════════════════ */

void Delay_ms(unsigned int n)
{
    while (n--) _delay_ms(1);
}

/**
 * Send a single command byte (DC low) then release CS.
 * Not used directly by LCD_GFX but kept for completeness.
 */
void SPI_ControllerTx(uint8_t data)
{
    clear(LCD_DC_PORT, LCD_DC);         /* DC low = command */
    clear(LCD_PORT,    LCD_TFT_CS);
    spi_tx(data);
    set(LCD_PORT, LCD_TFT_CS);
    set(LCD_DC_PORT, LCD_DC);           /* DC high = data (idle state) */
}

/**
 * Send one raw byte without touching CS or DC.
 * Caller is responsible for setting DC and CS before/after.
 */
void SPI_ControllerTx_stream(uint8_t stream)
{
    spi_tx(stream);
}

/**
 * Send a 16-bit color word, managing CS.
 * Used by LCD_GFX.c after LCD_setAddr() to push pixel data.
 */
void SPI_ControllerTx_16bit(uint16_t data)
{
    /* DC stays high (data mode) — LCD_setAddr already left it there */
    clear(LCD_PORT, LCD_TFT_CS);
    spi_tx(data >> 8);
    spi_tx(data & 0xFFu);
    set(LCD_PORT, LCD_TFT_CS);
}

/**
 * Send a 16-bit color word without touching CS or DC.
 * Used inside burst loops where CS/DC are managed externally.
 */
void SPI_ControllerTx_16bit_stream(uint16_t data)
{
    spi_tx(data >> 8);
    spi_tx(data & 0xFFu);
}

/* ═══════════════════════════════════════════════════════════════════════════
   INTERNAL COMMAND HELPERS
   ═══════════════════════════════════════════════════════════════════════════ */

/* Send a command byte (DC low, CS managed). */
static void write_cmd(uint8_t cmd)
{
    clear(LCD_DC_PORT, LCD_DC);
    clear(LCD_PORT,    LCD_TFT_CS);
    spi_tx(cmd);
    set(LCD_PORT,    LCD_TFT_CS);
    set(LCD_DC_PORT, LCD_DC);
}

/* Send a command followed by N data bytes (from RAM). */
static void write_cmd_data(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    clear(LCD_DC_PORT, LCD_DC);         /* command */
    clear(LCD_PORT,    LCD_TFT_CS);
    spi_tx(cmd);
    set(LCD_DC_PORT, LCD_DC);           /* data */
    for (uint8_t i = 0; i < len; i++) spi_tx(data[i]);
    set(LCD_PORT, LCD_TFT_CS);
}

/* ═══════════════════════════════════════════════════════════════════════════
   LCD_setAddr
   Replaces the ST7735 version.  Sets column (CASET) and row (PASET) windows
   then issues RAMWR so the next 16-bit writes go straight to the framebuffer.
   Coordinates are uint8_t to match the original signature; the HX8357D
   accepts uint16_t internally but the 328PB screen area fits in a byte.
   ═══════════════════════════════════════════════════════════════════════════ */

void LCD_setAddr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    /* Column address set */
    {
        uint8_t d[] = { 0x00, x0, 0x00, x1 };
        write_cmd_data(HX8357_CASET, d, 4);
    }
    /* Row address set */
    {
        uint8_t d[] = { 0x00, y0, 0x00, y1 };
        write_cmd_data(HX8357_PASET, d, 4);
    }
    /* Write-to-RAM — leaves DC high so the caller can stream pixels */
    clear(LCD_DC_PORT, LCD_DC);
    clear(LCD_PORT,    LCD_TFT_CS);
    spi_tx(HX8357_RAMWR);
    set(LCD_PORT,    LCD_TFT_CS);
    set(LCD_DC_PORT, LCD_DC);
}

/* ═══════════════════════════════════════════════════════════════════════════
   LCD_rotate
   ═══════════════════════════════════════════════════════════════════════════ */

void LCD_rotate(uint8_t r)
{
    uint8_t madctl;
    switch (r % 4) {
        case 0:  madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB; break;
        case 1:  madctl = MADCTL_MV | MADCTL_MY | MADCTL_RGB; break;
        case 2:  madctl = MADCTL_RGB;                          break;
        default: madctl = MADCTL_MX | MADCTL_MV | MADCTL_RGB; break;
    }
    write_cmd_data(HX8357_MADCTL, &madctl, 1);
}

/* ═══════════════════════════════════════════════════════════════════════════
   lcd_init
   Ported from Adafruit's HX8357D initd[] sequence.
   Replaces ST7735's lcd_init() — same name, same call site in your main.c.
   ═══════════════════════════════════════════════════════════════════════════ */

void lcd_init(void)
{
    /* ── GPIO direction ─────────────────────────────────────────────────── */
    LCD_DDR    |= (1u << LCD_MOSI) | (1u << LCD_SCK) | (1u << LCD_TFT_CS);
    LCD_DC_DDR |= (1u << LCD_DC);
    LCD_RST_DDR|= (1u << LCD_RST);

    /* Idle states */
    set(LCD_PORT,     LCD_TFT_CS);
    set(LCD_DC_PORT,  LCD_DC);
    set(LCD_RST_PORT, LCD_RST);

    /* ── Hardware SPI: master, mode 0, MSB first, fosc/2 (~8 MHz) ───────── */
    SPCR = (1u << SPE) | (1u << MSTR);   /* enable, master */
    SPSR = (1u << SPI2X);                 /* double speed   */

    /* ── Hardware reset pulse ───────────────────────────────────────────── */
    clear(LCD_RST_PORT, LCD_RST);
    _delay_ms(10);
    set(LCD_RST_PORT, LCD_RST);
    _delay_ms(120);

    /* ── HX8357D init sequence ──────────────────────────────────────────── */

    /* Soft reset */
    write_cmd(HX8357_SWRESET);
    _delay_ms(100);

    /* Enable extension commands */
    { uint8_t d[] = {0xFF, 0x83, 0x57}; write_cmd_data(HX8357D_SETC,    d, 3); }
    _delay_ms(300);

    /* RGB interface: enable SDO pin */
    { uint8_t d[] = {0x80, 0x00, 0x06, 0x06}; write_cmd_data(HX8357_SETRGB,   d, 4); }

    /* VCOM = -1.52 V */
    { uint8_t d[] = {0x25}; write_cmd_data(HX8357D_SETCOM,  d, 1); }

    /* Oscillator: 70 Hz normal mode, 55 Hz idle */
    { uint8_t d[] = {0x68}; write_cmd_data(HX8357_SETOSC,   d, 1); }

    /* Panel: BGR order, gate direction swapped */
    { uint8_t d[] = {0x05}; write_cmd_data(HX8357_SETPANEL, d, 1); }

    /* Power control */
    { uint8_t d[] = {0x00, 0x15, 0x1C, 0x1C, 0x83, 0xAA}; write_cmd_data(HX8357_SETPWR1,  d, 6); }

    /* Source option (SETSTBA) */
    { uint8_t d[] = {0x50, 0x50, 0x01, 0x3C, 0x1E, 0x08}; write_cmd_data(HX8357D_SETSTBA, d, 6); }

    /* Display cycle (SETCYC) */
    { uint8_t d[] = {0x02, 0x40, 0x00, 0x2A, 0x2A, 0x0D, 0x78}; write_cmd_data(HX8357D_SETCYC, d, 7); }

    /* Gamma */
    {
        uint8_t d[] = {
            0x02,0x0A,0x11,0x1D,0x23,0x35,0x41,0x4B,
            0x4B,0x42,0x3A,0x27,0x1B,0x08,0x09,0x03,
            0x02,0x0A,0x11,0x1D,0x23,0x35,0x41,0x4B,
            0x4B,0x42,0x3A,0x27,0x1B,0x08,0x09,0x03,
            0x00,0x01
        };
        write_cmd_data(HX8357D_SETGAMMA, d, 34);
    }

    /* 16-bit color (RGB565) */
    { uint8_t d[] = {0x55}; write_cmd_data(HX8357_COLMOD, d, 1); }

    /* Memory access: MY=1, MX=1, RGB */
    { uint8_t d[] = {0xC0}; write_cmd_data(HX8357_MADCTL, d, 1); }

    /* Tear effect off */
    { uint8_t d[] = {0x00}; write_cmd_data(HX8357_TEON,     d, 1); }

    /* Tear scanline */
    { uint8_t d[] = {0x00, 0x02}; write_cmd_data(HX8357_TEARLINE, d, 2); }

    /* Exit sleep, then display on */
    write_cmd(HX8357_SLPOUT); _delay_ms(150);
    write_cmd(HX8357_DISPON); _delay_ms(50);
}