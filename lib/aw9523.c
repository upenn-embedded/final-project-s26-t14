/*
 * File:   aw9523.c
 * Author: amarischen
 *
 * Created on April 23, 2026, 1:21 AM
 */

#include <xc.h>
#include "i2c.h"
#include "aw9523.h"

uint8_t aw9523_init(void)
{
    uint8_t id = 0;
    if (i2c_readCompleteStream(&id, AW9523_ADDR, 0x10, 1) != I2C_OK)
        return 1;
    
    if (id != 0x23)
        return 2;
 
    if (i2c_writeRegister(AW9523_ADDR, REG_RESET, 0x00) != I2C_OK)
        return 3;
 
    if (i2c_writeRegister(AW9523_ADDR, REG_GCR, 0x10) != I2C_OK)
        return 4;
 
    if (i2c_writeRegister(AW9523_ADDR, REG_LED_MODE_P0, 0x00) != I2C_OK)
        return 5;

    if (i2c_writeRegister(AW9523_ADDR, REG_LED_MODE_P1, 0x00) != I2C_OK)
        return 6;
 
    if (i2c_writeRegister(AW9523_ADDR, REG_CONFIG_P0, 0x00) != I2C_OK)
        return 7;
 
    if (i2c_writeRegister(AW9523_ADDR, REG_CONFIG_P1, 0x00) != I2C_OK)
        return 8;
 
    return AW9523_OK;
}


uint8_t set_led(uint8_t pin, uint8_t on) {
    // only pin between 0 and 15
    uint8_t level = 0;
    if (on) {
        level = 0xFF;
    }
    return (i2c_writeRegister(AW9523_ADDR, dim_reg[pin], level) == I2C_OK)
           ? AW9523_OK : AW9523_ERR;
}
