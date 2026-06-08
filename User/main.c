#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "Timer.h"

uint16_t num=0;

int main(){
	Timer_Init();
	OLED_Init();
	OLED_ShowString(1,1,"WTF");
	OLED_ShowNum(2,1,num,5);
	while(1){
		OLED_ShowNum(2,1,num,5);
	}


}


void TIM1_UP_IRQHandler(void){
	if(TIM_GetFlagStatus(TIM1,TIM_FLAG_Update)==SET){
		num++;
		TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
	}
}
