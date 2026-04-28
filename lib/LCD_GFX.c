/*
 * LCD_GFX.c
 */

#include "LCD_GFX.h"
#include "HX8357.h"
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

static void LCD_fillSpan(uint16_t x0, uint16_t y, uint16_t x1, uint16_t color) {
    // Corrected boundary checks for HX8357 dimensions 
    if (y >= LCD_HEIGHT) return;
    if (x0 >= LCD_WIDTH) x0 = 0;
    if (x1 >= LCD_WIDTH) x1 = LCD_WIDTH - 1;
    if (x1 < x0) return;

    LCD_setAddr(x0, y, x1, y);
    
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

void LCD_drawCursor(uint16_t x, uint16_t y, uint16_t color) {
    // Define cursor size (5 pixels in each direction from center)
    uint16_t size = 5;

    // Horizontal line (left to right)
    // Start at x-size and end at x+size, centered on y
    LCD_drawLine(x - size, y, x + size, y, color);

    // Vertical line (top to bottom)
    // Start at y-size and end at y+size, centered on x
    LCD_drawLine(x, y - size, x, y + size, color);
}

//void LCD_drawSprite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* bitmap) {
//    // 1. Boundary Check
//    if ((x + w) > LCD_WIDTH || (y + h) > LCD_HEIGHT) return;
//
//    // 2. Set the address window to the size of the sprite
//    LCD_setAddr(x, y, x + w - 1, y + h - 1);
//
//    // 3. Prepare to send pixel data
//    clear(LCD_PORT, LCD_TFT_CS);
//    set(LCD_PORT, LCD_DC);
//
//    // 4. Stream the array from Flash
//    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
//        // MUST use &bitmap[i] to get the address in Flash
//        uint16_t color = pgm_read_word(&(bitmap[i])); 
//        
//        // HX8357 expects High Byte first
//        SPI_ControllerTx_stream((uint8_t)(color >> 8));   
//        SPI_ControllerTx_stream((uint8_t)(color & 0xFF)); 
//    }
//
//    set(LCD_PORT, LCD_TFT_CS);
//}

void LCD_drawSprite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* bitmap) {
    // 1. Calculate top-left origin based on center (x, y)
    // We use int32_t for a moment to prevent underflow if (x - w/2) is negative
    int32_t x_origin = (int32_t)x - (w / 2);
    int32_t y_origin = (int32_t)y - (h / 2);

    // 2. Boundary Check (Don't draw if the sprite is completely off-screen)
    if (x_origin < 0 || y_origin < 0 || 
       (x_origin + w) > LCD_WIDTH || (y_origin + h) > LCD_HEIGHT) return;

    // 3. Set the address window using the calculated origin
    LCD_setAddr((uint16_t)x_origin, (uint16_t)y_origin, 
                (uint16_t)x_origin + w - 1, (uint16_t)y_origin + h - 1);

    // 4. Prepare to send pixel data
    clear(LCD_PORT, LCD_TFT_CS);
    set(LCD_PORT, LCD_DC);

    // 5. Stream the array from Flash
    uint32_t total_pixels = (uint32_t)w * h;
    for (uint32_t i = 0; i < total_pixels; i++) {
        uint16_t color = pgm_read_word(&(bitmap[i])); 
        
        SPI_ControllerTx_stream((uint8_t)(color >> 8));   
        SPI_ControllerTx_stream((uint8_t)(color & 0xFF)); 
    }

    set(LCD_PORT, LCD_TFT_CS);
}
