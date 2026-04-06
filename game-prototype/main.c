#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "i2c.h"
#include "imu.h"
#include "uart.h"
 
#define DT_MS   10
#define DT_S    (DT_MS / 1000.0f)
 
static uint8_t s_reset_cause;
 
int main(void)
{
    s_reset_cause = MCUSR;
    MCUSR = 0;
    wdt_disable();
 
    uart_init();
 
    if (s_reset_cause & (1 << WDRF))  printf("RST:WDT\n");
    if (s_reset_cause & (1 << BORF))  printf("RST:BOD\n");
    if (s_reset_cause & (1 << EXTRF)) printf("RST:EXT\n");
    if (s_reset_cause & (1 << PORF))  printf("RST:PWR\n");
 
    i2c_init();
 
    while (imu_init() != MPU6050_OK) {
        printf("imu_init failed, g_i2c_last_error=0x%02X\n",  get_error());
        _delay_ms(100);
    }
 
    while (1) {
        _delay_ms(DT_MS);
 
        if (mpu6050_update(DT_S) == MPU6050_OK) {
            printf("P:%f R:%f Y:%f\n",
                   mpu6050_get_pitch(),
                   mpu6050_get_roll(),
                   mpu6050_get_yaw());
        } else {
            printf("update failed, g_i2c_last_error=0x%02X\n", get_error());
        }
    }
 
    return 0;
}