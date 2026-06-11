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

void MPU6050_ReadDatas(uint8_t RegAddress,uint8_t* Array,uint8_t count){
	uint8_t i=0;
	MyIIC_Start();
	MyIIC_SendByte(MPU6050_ADDRESS);
	MyIIC_ReceiveAck();
	MyIIC_SendByte(RegAddress);
	MyIIC_ReceiveAck();
	
	MyIIC_Start();
	MyIIC_SendByte(MPU6050_ADDRESS|0x01);
	MyIIC_ReceiveAck();
	for (i=0;i<count;i++){
		Array[i]=MyIIC_ReceiveByte();
		if(i<count-1){
			MyIIC_SendAck(0);
		}else{
			MyIIC_SendAck(1);
		}
		
	}
	MyIIC_Stop();
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

//uint8_t MPU6050_GetID(void){
//	return MPU6050_ReadDatas(MPU6050_WHO_AM_I,1);
//}

void MPU6050_GetData(int16_t *ACCEL_XOUT,int16_t *ACCEL_YOUT,int16_t *ACCEL_ZOUT,
						int16_t *GYRO_XOUT,int16_t *GYRO_YOUT,int16_t *GYRO_ZOUT){
	
	uint8_t Data[14];
	MPU6050_ReadDatas(MPU6050_ACCEL_XOUT_H,Data,14);	//读取从ACCEL_XOUT开始的连续14个字节的数据					

	*ACCEL_XOUT = (Data[0]<<8)|Data[1];			
	
	*ACCEL_YOUT = (Data[2]<<8)|Data[3];				
	
	*ACCEL_ZOUT = (Data[4]<<8)|Data[5];				

	*GYRO_XOUT = (Data[8]<<8)|Data[9];			

	*GYRO_YOUT = (Data[10]<<8)|Data[11];			

	*GYRO_ZOUT = (Data[12]<<8)|Data[13];	
}

