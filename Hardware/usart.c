#include "stm32f10x.h"                  // Device header
#include "stdio.h"

void MyUSART_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USAT_InitSturcture;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//开启usart3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//GPIOB时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//tx设为复用推挽
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//rx为上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USAT_InitSturcture.USART_BaudRate=9600;//波特率
	USAT_InitSturcture.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无流控制
	USAT_InitSturcture.USART_Mode=USART_Mode_Tx;//只发送功能
	USAT_InitSturcture.USART_Parity=USART_Parity_No;//无奇偶校验
	USAT_InitSturcture.USART_StopBits=USART_StopBits_1;//一位停止位
	USAT_InitSturcture.USART_WordLength=USART_WordLength_8b;//8比特字长
	
	USART_Init(USART3,&USAT_InitSturcture);
	USART_Cmd(USART3,ENABLE);
}

void MyUSART_Send(uint8_t Byte){
	USART_SendData(USART3,Byte);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
}

int fputc(int ch,FILE *f){
	MyUSART_Send(ch);
	return ch;
}
