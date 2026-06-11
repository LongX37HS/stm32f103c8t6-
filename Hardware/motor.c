#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void motor_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	PWM_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
}

void motor_SetSpeed(uint8_t witchPWM,int8_t Speed)//控制两个电机的IN1,IN2
{
	if(witchPWM==1){//右电机
		if (Speed >= 0)	//正转						//如果设置正转的速度值
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_15);	//PA4置高电平
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);	//PA5置低电平，设置方向为正转
			PWM_SetCompare1(Speed);				//PWM设置为速度值
		}
		else									//否则，即设置反转的速度值
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_15);	//PA4置低电平
			GPIO_SetBits(GPIOB, GPIO_Pin_14);	//PA5置高电平，设置方向为反转
			PWM_SetCompare1(-Speed);			//PWM设置为负的速度值，因为此时速度值为负数，而PWM只能给正数
		}
		}else if(witchPWM==0){//左电机
		if (Speed >= 0)		//正转					//如果设置正转的速度值
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_12);	//PA4置高电平
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);	//PA5置低电平，设置方向为正转
			PWM_SetCompare4(Speed);				//PWM设置为速度值
		}
		else									//否则，即设置反转的速度值
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);	//PA4置低电平
			GPIO_SetBits(GPIOB, GPIO_Pin_13);	//PA5置高电平，设置方向为反转
			PWM_SetCompare4(-Speed);			//PWM设置为负的速度值，因为此时速度值为负数，而PWM只能给正数
		}		
	}
	
}


