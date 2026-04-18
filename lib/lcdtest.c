//#define F_CPU 16000000UL
//
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include "HX8357.h"
//#include "LCD_GFX.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//
//
//void Initialize() {
//    LCD_init();
//    LCD_setScreen(CYAN);
//}
//
//int main (void) {
//    Initialize();
////    LCD_drawBlock(0, 0, 319, 479, RED);
//    LCD_drawBlock(0, 0, 10, 10, RED);
//    LCD_drawBlock(0, 100, 10, 110, YELLOW);
//    LCD_drawBlock(100, 0, 110, 10, BLUE);
//    LCD_drawBlock(100, 100, 110, 110, GREEN);
////    LCD_drawBlock(0, 0, 319, 479, RED);
//    // Test 1: use LCD_fillSpan path (known working via drawCircle)
////    LCD_setAddr(0, 0, 319, 479);
////    clear(LCD_PORT, LCD_TFT_CS);
////    set(LCD_PORT, LCD_DC);
////    for (uint32_t i = 0; i < 153600UL; i++) {
////        SPI_ControllerTx_16bit_stream(RED);
////    }
////    set(LCD_PORT, LCD_TFT_CS);
//    
//    while (1);
//}