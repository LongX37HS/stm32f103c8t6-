#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MPU6050.h"
#include "Timer.h"

int16_t Ax,Az,Ay,Gx,Gz,Gy;
uint8_t Ack=1;
uint16_t TimerCount=0;
int main(){
	Timer_Init();
	MPU6050_Init();
	OLED_Init();
	OLED_ShowNum(4,3,TimerCount,5);
	while(1){
		OLED_ShowSignedNum(1,1,Ax,5);
		OLED_ShowSignedNum(2,1,Az,5);
		OLED_ShowSignedNum(3,1,Ay,5);
		OLED_ShowSignedNum(1,8,Gx,5);
		OLED_ShowSignedNum(2,8,Gz,5);
		OLED_ShowSignedNum(3,8,Gy,5);
		
	}


}


void TIM1_UP_IRQHandler(void){
	
	if(TIM_GetFlagStatus(TIM1,TIM_FLAG_Update)==SET){
		MPU6050_GetData(&Ax,&Az,&Ay,&Gx,&Gz,&Gy);
		TimerCount=TIM_GetCounter(TIM1);
		TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
		
	}
}
