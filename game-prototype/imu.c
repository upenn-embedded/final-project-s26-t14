#include "imu.h"
#include "i2c.h"
#include <math.h>
 
#define REG_SMPLRT_DIV      0x19
#define REG_CONFIG          0x1A
#define REG_GYRO_CONFIG     0x1B
#define REG_GYRO_XOUT_H     0x43
#define REG_ACCEL_XOUT_H    0x3B
#define REG_PWR_MGMT_1      0x6B
#define REG_WHO_AM_I        0x75
 
#define GYRO_SENSITIVITY    131.0f
#define ACCEL_SENSITIVITY    16384.0f
#define ALPHA                0.96f
 
static float s_pitch = 0.0f;
static float s_roll  = 0.0f;
static float s_yaw   = 0.0f;
 
static inline int16_t to_int16(uint8_t hi, uint8_t lo)
{
    return (int16_t)((uint16_t)hi << 8 | lo);
}
 
uint8_t imu_init(void)
{
    uint8_t who = 0;
    if (i2c_readCompleteStream(&who, MPU6050_ADDR, REG_WHO_AM_I, 1) != I2C_OK)
        return MPU6050_ERR;
    if (who != 0x68)
        return MPU6050_ERR;
 
    if (i2c_writeRegister(MPU6050_ADDR, REG_PWR_MGMT_1, 0x00) != I2C_OK)
        return MPU6050_ERR;
 
    if (i2c_writeRegister(MPU6050_ADDR, REG_SMPLRT_DIV, 0x07) != I2C_OK)
        return MPU6050_ERR;
 
    if (i2c_writeRegister(MPU6050_ADDR, REG_CONFIG, 0x03) != I2C_OK)
        return MPU6050_ERR;
 
    if (i2c_writeRegister(MPU6050_ADDR, REG_GYRO_CONFIG, 0x00) != I2C_OK)
        return MPU6050_ERR;
 
    return MPU6050_OK;
}
 
uint8_t mpu6050_update(float dt)
{
    uint8_t raw[14];
    if (i2c_readCompleteStream(raw, MPU6050_ADDR, REG_ACCEL_XOUT_H, 14) != I2C_OK)
        return MPU6050_ERR;
 
    float ax = (float)to_int16(raw[0],  raw[1])  / ACCEL_SENSITIVITY;
    float ay = (float)to_int16(raw[2],  raw[3])  / ACCEL_SENSITIVITY;
    float az = (float)to_int16(raw[4],  raw[5])  / ACCEL_SENSITIVITY;
 
    float gx = (float)to_int16(raw[8],  raw[9])  / GYRO_SENSITIVITY;
    float gy = (float)to_int16(raw[10], raw[11]) / GYRO_SENSITIVITY;
    float gz = (float)to_int16(raw[12], raw[13]) / GYRO_SENSITIVITY;
 
    float pitch_acc = atan2f(ax, sqrtf(ay * ay + az * az)) * (180.0f / (float)M_PI);
    float roll_acc  = atan2f(ay, sqrtf(ax * ax + az * az)) * (180.0f / (float)M_PI);
 
    s_pitch = ALPHA * (s_pitch + gy * dt) + (1.0f - ALPHA) * pitch_acc;
    s_roll  = ALPHA * (s_roll  + gx * dt) + (1.0f - ALPHA) * roll_acc;
    s_yaw  += gz * dt;
 
    if (s_yaw >  180.0f) s_yaw -= 360.0f;
    if (s_yaw < -180.0f) s_yaw += 360.0f;
 
    return MPU6050_OK;
}
 
float mpu6050_get_pitch(void) { return s_pitch; }
float mpu6050_get_roll(void)  { return s_roll;  }
float mpu6050_get_yaw(void)   { return s_yaw;   }