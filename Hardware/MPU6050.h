#ifndef MPU6050_H__
#define MPU6050_H__

void MPU6050_WriteData(uint8_t RegAddress,uint8_t Data);
uint8_t MPU6050_ReadData(uint8_t RegAddress);
void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(int16_t *ACCEL_XOUT,int16_t *ACCEL_YOUT,int16_t *ACCEL_ZOUT,
						int16_t *GYRO_XOUT,int16_t *GYRO_YOUT,int16_t *GYRO_ZOUT);
#endif
