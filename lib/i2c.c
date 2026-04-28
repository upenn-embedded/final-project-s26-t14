#include "i2c.h"
#include <avr/io.h>
#include <util/twi.h>

#define I2C_TIMEOUT 1

// Globaal error code
volatile uint8_t g_i2c_last_error = I2C_OK;

// Inline removes function call overhead, for frequently used function
static inline uint8_t twi_wait(void)
{
    while (!(TWCR0 & (1 << TWINT)));
}

static inline uint8_t twi_status(void)
{
    return TWSR0 & 0xF8; // Mask prescaler bits (1111 1000)
}

static inline uint8_t twi_error(uint8_t status)
{
    g_i2c_last_error = status;
    return status;
}

uint8_t get_error() {
    return g_i2c_last_error;
}

// I2C functions
void i2c_init(void) {
    // input pins (TWI peripheral drives open-drain)
    DDRC  &= ~((1 << SDA_PORT) | (1 << SCL_PORT));

    // Disable internal pull-ups
    // Internal pull up resistors in MPU6050
    // If not MPU6050, need exterminal pull up resistors
    PORTC &= ~((1 << SDA_PORT) | (1 << SCL_PORT));;

    TWSR0 = 0x00;  // prescaler = 1
    TWBR0 = 0x0C;  // 400kHz at 16MHz: TWBR = ((16000000/400000) - 16) / 2 = 12 = 0x0C
    TWCR0 = (1 << TWEN); 
}

void i2c_stop(void) {
    TWCR0 = TWCR_STOP;
}

uint8_t i2c_start(void) {
    g_i2c_last_error = I2C_OK;  // clear previous error
 
    TWCR0 = TWCR_START;
    twi_wait();
 
    uint8_t st = twi_status();
    if (st != TW_START)
        return twi_error(st);
 
    return I2C_OK;
}

uint8_t i2c_repStart(void)
{
    g_i2c_last_error = I2C_OK;
 
    TWCR0 = TWCR_START;  // same bits as START; hardware knows context
    twi_wait();
 
    uint8_t st = twi_status();
    if (st != TW_REP_START)
        return twi_error(st);
 
    return I2C_OK;
}

uint8_t i2c_writeBegin(uint8_t addr)
{
    TWDR0 = addr << 1;  // SLA+W: address + write bit (0)
    TWCR0 = TWCR_SEND;
    twi_wait();
 
    uint8_t st = twi_status();
    if (st != TW_MT_SLA_ACK)
        return twi_error(st);  // device absent
 
    return I2C_OK;
}

uint8_t i2c_readBegin(uint8_t addr)
{
    TWDR0 = (addr << 1) | 1; // SLA+R: address + read bit (1)
    TWCR0 = TWCR_SEND;
    twi_wait();
 
    uint8_t st = twi_status();
    if (st != TW_MR_SLA_ACK)
        return twi_error(st);  // device absent
 
    return I2C_OK;
}

uint8_t i2c_writeRegister(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t err;
 
    if ((err = i2c_start()) != I2C_OK)
        return err;
 
    if ((err = i2c_writeBegin(addr)) != I2C_OK) {
        i2c_stop();
        return err;
    }
 
    TWDR0 = reg;
    TWCR0 = TWCR_SEND;
    twi_wait();
    uint8_t st = twi_status();
    if (st != TW_MT_DATA_ACK) {
        i2c_stop();
        return twi_error(st);
    }
 
    TWDR0 = data;
    TWCR0 = TWCR_SEND;
    twi_wait();
    st = twi_status();
    if (st != TW_MT_DATA_ACK) {
        i2c_stop();
        return twi_error(st);
    }
 
    i2c_stop();
    return I2C_OK;
}
 
uint8_t i2c_readStream(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        uint8_t is_last = (i == len - 1);
 
        TWCR0 = is_last ? TWCR_SEND : TWCR_ACK;
        twi_wait();
  
        uint8_t st = twi_status();
        uint8_t expected = is_last ? TW_MR_DATA_NACK : TW_MR_DATA_ACK;
        if (st != expected) {
            twi_error(st);
            return;
        }
 
        buf[i] = TWDR0;
    }
    return I2C_OK;
}
 
uint8_t i2c_readCompleteStream(uint8_t *buf, uint8_t addr, uint8_t reg, uint16_t len)
{
    uint8_t err;
 
    if ((err = i2c_start()) != I2C_OK)
        return err;
 
    if ((err = i2c_writeBegin(addr)) != I2C_OK) {
        i2c_stop();
        return err;
    }
 
    TWDR0 = reg;
    TWCR0 = TWCR_SEND;
    twi_wait();
    uint8_t st = twi_status();
    if (st != TW_MT_DATA_ACK) {
        i2c_stop();
        return twi_error(st);
    }
 
    if ((err = i2c_repStart()) != I2C_OK) {
        i2c_stop();
        return err;
    }
 
    if ((err = i2c_readBegin(addr)) != I2C_OK) {
        i2c_stop();
        return err;
    }
 
    i2c_readStream(buf, len);
    i2c_stop();
 
    return g_i2c_last_error;
}
