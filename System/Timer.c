#include "stm32f10x.h"                  // Device header

void Timer_Init(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler=100-1;	//72000000/100=720khz
	TIM_TimeBaseInitStructure.TIM_Period=720-1;//720k/720=1000hz=1ms

	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	
	NVIC_Init(&NVIC_InitStructure);
	
}

//void TIM3_IRQHandler(void){
//	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)==SET){
//		
//		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
//	}
//}
