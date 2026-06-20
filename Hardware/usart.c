#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdarg.h"

uint8_t MyUSART_RxData;		//定义串口接收的数据变量
uint8_t MyUSART_RxFlag;		//定义串口接收的标志位变量

void MyUSART_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USAT_InitSturcture;
	NVIC_InitTypeDef NVIC_InitStructure;
	
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
	USAT_InitSturcture.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//只发送功能
	USAT_InitSturcture.USART_Parity=USART_Parity_No;//无奇偶校验
	USAT_InitSturcture.USART_StopBits=USART_StopBits_1;//一位停止位
	USAT_InitSturcture.USART_WordLength=USART_WordLength_8b;//8比特字长
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
    NVIC_InitStructure.NVIC_IRQChannel    = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority       = 1;
	
	NVIC_Init(&NVIC_InitStructure);
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

uint8_t MyUSART_GetRxFlag(void)
{
	if (MyUSART_RxFlag == 1)			//如果标志位为1
	{
		MyUSART_RxFlag = 0;
		return 1;					//则返回1，并自动清零标志位
	}
	return 0;						//如果标志位为0，则返回0
}


uint8_t MyUSART_GetRxData(void)
{
	return MyUSART_RxData;			//返回接收的数据变量
}

void MyUSART_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		MyUSART_Send(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void MyUSART_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		MyUSART_Send(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void MyUSART_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		MyUSART_Send(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}
}


/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void MyUSART_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	MyUSART_SendString(String);		//串口发送字符数组（字符串）
}

//void USART3_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)		//判断是否是USART1的接收事件触发的中断
//	{
//		MyUSART_RxData = USART_ReceiveData(USART3);				//读取数据寄存器，存放在接收的数据变量
//		MyUSART_RxFlag = 1;										//置接收标志位变量为1
//		USART_ClearITPendingBit(USART3, USART_IT_RXNE);			//清除USART1的RXNE标志位
//																//读取数据寄存器会自动清除此标志位
//																	//如果已经读取了数据寄存器，也可以不执行此代码
//	}
//}
