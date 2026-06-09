#include "stm32f10x.h"                  // Device header
#include "MyIIC.h"
#include "MPU6050_REG.h"

#define MPU6050_ADDRESS 0xD0

void MPU6050_WriteData(uint8_t RegAddress,uint8_t Data){
	MyIIC_Start();
	MyIIC_SendByte(MPU6050_ADDRESS);
	MyIIC_ReceiveAck();
	MyIIC_SendByte(RegAddress);
	MyIIC_ReceiveAck();
	MyIIC_SendByte(Data);
	MyIIC_ReceiveAck();
	MyIIC_Stop();
}

uint8_t MPU6050_ReadData(uint8_t RegAddress){
	uint8_t Byte;
	MyIIC_Start();
	MyIIC_SendByte(MPU6050_ADDRESS);
	MyIIC_ReceiveAck();
	MyIIC_SendByte(RegAddress);
	MyIIC_ReceiveAck();
	
	MyIIC_Start();
	MyIIC_SendByte(MPU6050_ADDRESS|0x01);
	MyIIC_ReceiveAck();
	Byte=MyIIC_ReceiveByte();
	MyIIC_SendAck(1);
	MyIIC_Stop();
	return Byte;
}

void MPU6050_Init(void){
	MyIIC_Init();
	MPU6050_WriteData(MPU6050_PWR_MGMT_1,0x01);
	MPU6050_WriteData(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteData(MPU6050_SMPLRT_DIV,0x09);
	MPU6050_WriteData(MPU6050_CONFIG,0x06);
	MPU6050_WriteData(MPU6050_GYRO_CONFIG,0x18);
	MPU6050_WriteData(MPU6050_ACCEL_CONFIG,0x18);
}

uint8_t MPU6050_GetID(void){
	return MPU6050_ReadData(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *ACCEL_XOUT,int16_t *ACCEL_YOUT,int16_t *ACCEL_ZOUT,
						int16_t *GYRO_XOUT,int16_t *GYRO_YOUT,int16_t *GYRO_ZOUT){
	uint8_t DataL;
	uint8_t DataH;
	DataH = MPU6050_ReadData(MPU6050_ACCEL_XOUT_H);		//??????X???8???
	DataL = MPU6050_ReadData(MPU6050_ACCEL_XOUT_L);		//??????X???8???
	*ACCEL_XOUT = (DataH << 8) | DataL;						//????,????????
	
	DataH = MPU6050_ReadData(MPU6050_ACCEL_YOUT_H);		//??????Y???8???
	DataL = MPU6050_ReadData(MPU6050_ACCEL_YOUT_L);		//??????Y???8???
	*ACCEL_YOUT = (DataH << 8) | DataL;						//????,????????
	
	DataH = MPU6050_ReadData(MPU6050_ACCEL_ZOUT_H);		//??????Z???8???
	DataL = MPU6050_ReadData(MPU6050_ACCEL_ZOUT_L);		//??????Z???8???
	*ACCEL_ZOUT = (DataH << 8) | DataL;						//????,????????
	
	DataH = MPU6050_ReadData(MPU6050_GYRO_XOUT_H);		//?????X???8???
	DataL = MPU6050_ReadData(MPU6050_GYRO_XOUT_L);		//?????X???8???
	*GYRO_XOUT = (DataH << 8) | DataL;						//????,????????
	
	DataH = MPU6050_ReadData(MPU6050_GYRO_YOUT_H);		//?????Y???8???
	DataL = MPU6050_ReadData(MPU6050_GYRO_YOUT_L);		//?????Y???8???
	*GYRO_YOUT = (DataH << 8) | DataL;						//????,????????
	
	DataH = MPU6050_ReadData(MPU6050_GYRO_ZOUT_H);		//?????Z???8???
	DataL = MPU6050_ReadData(MPU6050_GYRO_ZOUT_L);		//?????Z???8???
	*GYRO_ZOUT = (DataH << 8) | DataL;		
}

