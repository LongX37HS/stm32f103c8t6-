#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MPU6050.h"
#include "Timer.h"
#include "usart.h"
#include "motor.h"
#include "MPU6050.h"
#include "Encoder.h"
#include "Delay.h"
//MPU6050测试
int16_t Ax,Az,Ay,Gx,Gz,Gy;
uint8_t Ack=1;
uint16_t TimerCount=0;
uint8_t receive;;
int8_t speedL=0,speedR=0;
uint8_t ID;
int16_t count1;
int16_t count2;
int main(){
	Timer_Init();
	motor_Init();
	MPU6050_Init();
	MyUSART_Init();
	OLED_Init();	
	MyUSART_Printf("U");
	Encoder_Init();
	while(1){
		if (MyUSART_GetRxFlag()) {
			receive = MyUSART_GetRxData();
			if (receive == 0x41) {
				speedR+=20;
				if(speedR>100){
					speedR=0;
				}
				motor_SetSpeed(1,speedR);
			}else if(receive == 0x42){
				speedR-=20;
				if(speedR<-100){
					speedR=0;
				}
				motor_SetSpeed(1,speedR);
			}else if(receive == 0x43){
				speedL+=20;
				if(speedL>100){
					speedL=0;
				}
				motor_SetSpeed(0,speedL);
			}else if(receive == 0x44){
				speedL-=20;
				if(speedL<-100){
					speedL=0;
				}
				motor_SetSpeed(0,speedL);
			}
			OLED_ShowSignedNum(4, 1, speedL,3);
			OLED_ShowSignedNum(4, 5, speedR,3);
			
		}
//		OLED_ShowSignedNum(1,1,Ax,5);
//		OLED_ShowSignedNum(2,1,Az,5);
//		OLED_ShowSignedNum(3,1,Ay,5);
//		OLED_ShowSignedNum(1,8,Gx,5);
//		OLED_ShowSignedNum(2,8,Gz,5);
//		OLED_ShowSignedNum(3,8,Gy,5);
        count1=Encoder_GetCounter1();
		count2=Encoder_GetCounter2();
		Delay_ms(100);
		OLED_ShowSignedNum(1,1,count1,5);
		OLED_ShowSignedNum(1,8,count2,5);
	}

}


void TIM3_IRQHandler(void){
	
	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)==SET){
//		MPU6050_GetData(&Ax,&Az,&Ay,&Gx,&Gz,&Gy);
		TimerCount=TIM_GetCounter(TIM3);
		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
		
	}
}
//编码器测试
//int16_t count1;
//int16_t count2;

//int main(){
//	Encoder_Init();
//	motor_Init();
//	MPU6050_Init();
//	OLED_Init();
//	OLED_Clear();
//	while(1){
//		count1=Encoder_GetCounter1();
//		count2=Encoder_GetCounter2();
//		Delay_ms(100);
//		OLED_ShowSignedNum(1,1,count1,5);
//		OLED_ShowSignedNum(1,8,count2,5);
//	}
//}
