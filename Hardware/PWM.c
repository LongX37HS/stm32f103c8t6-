#include "stm32f10x.h"                  // Device header

void PWM_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler=36-1;
	TIM_TimeBaseInitStructure.TIM_Period=100-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);
	
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}
//AM1/AM2 是 TB6612 输出的功率驱动信号，电压=电池电压，电流=驱动电机的大电流。它们是 H 桥的输出端，直接给电机供电的
void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM1, Compare);		//设置CCR1的值
}

void PWM_SetCompare4(uint16_t Compare)
{
	TIM_SetCompare4(TIM1, Compare);		//设置CCR4的值
}
