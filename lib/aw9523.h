#ifndef AW9523_H
#define AW9523_H

#define AW9523_ADDR         0x58
#define AW9523_OK           0
#define AW9523_ERR          1

#define REG_CONFIG_P0       0x04
#define REG_CONFIG_P1       0x05
#define REG_GCR             0x11
#define REG_LED_MODE_P0     0x12
#define REG_LED_MODE_P1     0x13
#define REG_RESET           0x7F

static const uint8_t dim_reg[16] = {
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 
    0x20, 0x21, 0x22, 0x23, 0x2C, 0x2D, 0x2E, 0x2F
};

uint8_t aw9523_init(void);
uint8_t set_led(uint8_t pin, uint8_t on);


#endif