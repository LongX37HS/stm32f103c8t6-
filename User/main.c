#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MPU6050.h"

int16_t Ax,Az,Ay,Gx,Gz,Gy;
uint8_t Ack=1;
int main(){
	MPU6050_Init();
	OLED_Init();
	while(1){
		MPU6050_GetData(&Ax,&Az,&Ay,&Gx,&Gz,&Gy);
		OLED_ShowSignedNum(1,1,Ax,5);
		OLED_ShowSignedNum(2,1,Az,5);
		OLED_ShowSignedNum(3,1,Ay,5);
		OLED_ShowSignedNum(1,8,Gx,5);
		OLED_ShowSignedNum(2,8,Gz,5);
		OLED_ShowSignedNum(3,8,Gy,5);
		
	}


}


//void TIM1_UP_IRQHandler(void){
//	if(TIM_GetFlagStatus(TIM1,TIM_FLAG_Update)==SET){
//		
//		TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
//	}
//}
