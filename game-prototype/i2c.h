#include <avr/io.h>
#include <util/twi.h>
#include <stdint.h>

/**
 * @brief Two Wire Control Register masks
 * TWINT: Interrupt Flag
 * TWSTA: Start Condition Bit
 * TWSTO: Stop Condition Bit
 * TWEN: Enable Bit
 * TWEA: Enable Acknowledge Bit
 */
#define TWCR_START  (1 << TWINT) | (1 << TWEN) | (1 << TWSTA)
#define TWCR_STOP   (1 << TWINT) | (1 << TWEN) | (1 << TWSTO)
#define TWCR_SEND   (1 << TWINT) | (1 << TWEN)
#define TWCR_ACK    (1 << TWINT) | (1 << TWEN) | (1 << TWEA)

#define SCL_PORT  PORTC5
#define SDA_PORT  PORTC4

#define I2C_OK  0x00

uint8_t get_error();
void i2c_init(void);
void i2c_stop(void);
uint8_t i2c_start(void);
uint8_t i2c_repStart(void);
uint8_t i2c_writeBegin(uint8_t addr);
uint8_t i2c_readBegin(uint8_t addr);
uint8_t i2c_writeRegister(uint8_t addr, uint8_t reg, uint8_t data);
uint8_t i2c_readStream(uint8_t *buf, uint16_t len);
uint8_t i2c_readCompleteStream(uint8_t *buf, uint8_t addr, uint8_t reg, uint16_t len);
