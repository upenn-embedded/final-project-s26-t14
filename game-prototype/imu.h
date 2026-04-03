#ifndef imu_H
#define imu_H
 
#include <stdint.h>
 
#define MPU6050_ADDR        0x68
 
#define MPU6050_OK          0
#define MPU6050_ERR         1
 
uint8_t mpu6050_init(void);
uint8_t mpu6050_update(float dt);
float   mpu6050_get_pitch(void);
float   mpu6050_get_roll(void);
float   mpu6050_get_yaw(void);
 
#endif