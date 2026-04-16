/*
 * LCD_GFX.c
 */

#include "LCD_GFX.h"
#include "HX8357.h"
#include <string.h>
#include <stdlib.h>

static void LCD_fillSpan(uint16_t x0, uint16_t y, uint16_t x1, uint16_t color) {
    // Corrected boundary checks for HX8357 dimensions 
    if (y >= LCD_HEIGHT) return;
    if (x0 >= LCD_WIDTH) x0 = 0;
    if (x1 >= LCD_WIDTH) x1 = LCD_WIDTH - 1;
    if (x1 < x0) return;

    LCD_setAddr(x0, y, x1, y);
    
    // Low CS once for the entire stream for efficiency
    clear(LCD_PORT, LCD_TFT_CS);
    set(LCD_PORT, LCD_DC);
    
    for (uint32_t count = 0; count <= (uint32_t)(x1 - x0); count++) {
        SPI_ControllerTx_stream((uint8_t)(color >> 8)); // High Byte
        SPI_ControllerTx_stream((uint8_t)(color & 0xFF)); // Low Byte
    }
    set(LCD_PORT, LCD_TFT_CS);
}

void LCD_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    LCD_setAddr(x, y, x, y);
    
    clear(LCD_PORT, LCD_TFT_CS);
    set(LCD_PORT, LCD_DC);
    SPI_ControllerTx_stream((uint8_t)(color >> 8));
    SPI_ControllerTx_stream((uint8_t)(color & 0xFF));
    set(LCD_PORT, LCD_TFT_CS);
}

void LCD_drawChar(uint16_t x, uint16_t y, uint8_t character, uint16_t fColor, uint16_t bColor)
{
    uint8_t row;
    uint8_t i;
    uint8_t j;

    if (character < 0x20 || character > 0x7F) {
        character = '?';
    }
    row = (uint8_t)(character - 0x20);

    for (i = 0; i < 5; i++) {
        uint8_t pixels = (uint8_t)ASCII[row][i];
        for (j = 0; j < 8; j++) {
            if (((pixels >> j) & 0x01U) != 0U) {
                LCD_drawPixel(x + i, y + j, fColor);
            } else {
                LCD_drawPixel(x + i, y + j, bColor);
            }
        }
    }

    for (j = 0; j < 8; j++) {
        LCD_drawPixel(x + 5, y + j, bColor);
    }
}

void LCD_drawChar_Transparent(uint16_t x, uint16_t y, uint8_t character, uint16_t fColor)
{
    uint8_t row;
    uint8_t i;
    uint8_t j;

    if (character < 0x20 || character > 0x7F) {
        character = '?';
    }
    row = (uint8_t)(character - 0x20);

    for (i = 0; i < 5; i++) {
        uint8_t pixels = (uint8_t)ASCII[row][i];
        for (j = 0; j < 8; j++) {
            if (((pixels >> j) & 0x01U) != 0U) {
                LCD_drawPixel(x + i, y + j, fColor);
            }
        }
    }
}

void LCD_drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
{
    int16_t x = 0;
    int16_t y = (int16_t)radius;
    int16_t d = 3 - 2 * (int16_t)radius;

    while (y >= x) {
        // We use signed math for the offset, then cast to uint16_t for the function
        LCD_fillSpan((uint16_t)(x0 - x), (uint16_t)(y0 + y), (uint16_t)(x0 + x), color);
        LCD_fillSpan((uint16_t)(x0 - x), (uint16_t)(y0 - y), (uint16_t)(x0 + x), color);
        LCD_fillSpan((uint16_t)(x0 - y), (uint16_t)(y0 + x), (uint16_t)(x0 + y), color);
        LCD_fillSpan((uint16_t)(x0 - y), (uint16_t)(y0 - x), (uint16_t)(x0 + y), color);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c)
{
    int16_t dx = abs(x1 - x0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t dy = -abs(y1 - y0);
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;
    int16_t e2;

    while (1) {
        if (x0 >= 0 && y0 >= 0 && x0 < (int16_t)LCD_WIDTH && y0 < (int16_t)LCD_HEIGHT) {
            LCD_drawPixel((uint16_t)x0, (uint16_t)y0, c);
        }
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void LCD_drawBlock(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t y;

    if (x0 >= LCD_WIDTH || y0 >= LCD_HEIGHT) return;

    if (x1 >= LCD_WIDTH)  x1 = LCD_WIDTH - 1;
    if (y1 >= LCD_HEIGHT) y1 = LCD_HEIGHT - 1;
    if (x1 < x0 || y1 < y0) return;

    for (y = y0; y <= y1; y++) {
        LCD_fillSpan(x0, y, x1, color);
    }
}

void LCD_setScreen(uint16_t color)
{
    uint16_t y;

    for (y = 0; y < LCD_HEIGHT; y++) {
        LCD_fillSpan(0, y, LCD_WIDTH - 1, color);
    }
}

void LCD_drawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg)
{
    uint16_t i;
    size_t len = strlen(str);

    for (i = 0; i < len; i++) {
        LCD_drawChar(x + (uint16_t)(i * 6U), y, (uint8_t)str[i], fg, bg);
    }
}
