#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

char BlueSerial_RxPacket[100];	// 蓝牙串口接收数据包缓冲
uint8_t BlueSerial_RxFlag;		// 数据包接收完成标志位

/**
  * 函    数：蓝牙串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void BlueSerial_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/*GPIO初始化*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*USART初始化*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	/*USART中断使能*/
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	/*NVIC中断配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	/*USART使能*/
	USART_Cmd(USART3, ENABLE);
}

/**
  * 函    数：蓝牙串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void BlueSerial_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：蓝牙串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void BlueSerial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)			//遍历数组
	{
		BlueSerial_SendByte(Array[i]);		//依次调用BlueSerial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：蓝牙串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void BlueSerial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)	//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		BlueSerial_SendByte(String[i]);		//依次调用BlueSerial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：蓝牙串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		BlueSerial_SendByte(Number / BlueSerial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用BlueSerial_SendByte发送每位数字
	}
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void BlueSerial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	BlueSerial_SendString(String);	//蓝牙串口发送字符数组（字符串）
}

/**
  * 函    数：USART3中断函数（蓝牙串口接收）
  * 数据包格式：0xA5 | DATA[0...N] | CHECKSUM | 0x5A
  *           CHECKSUM = (DATA所有字节之和) & 0xFF
  */
void USART3_IRQHandler(void)
{
	static uint8_t RxState = 0;		// 0=等待包头0xA5, 1=接收数据
	static uint8_t pRxPacket = 0;	// 当前缓冲写入位置
	static uint8_t RxSum = 0;		// 数据字节累加（含校验字节）

	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART3);

		if (RxState == 0)				// 状态0：等待包头 0xA5
		{
			if (RxData == 0xA5 && BlueSerial_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
				RxSum = 0;
			}
		}
		else if (RxState == 1)			// 状态1：接收数据
		{
			/* 收到可能的包尾 0x5A，验证校验和 */
			if (RxData == 0x5A && pRxPacket >= 2)
			{
				uint8_t checksum = BlueSerial_RxPacket[pRxPacket - 1];
				// (累计和 - 校验字节) & 0xFF 应等于校验字节
				if ((uint8_t)(RxSum - checksum) == checksum)
				{
					BlueSerial_RxPacket[pRxPacket - 1] = '\0';	// 覆盖校验字节，数据变字符串
					BlueSerial_RxFlag = 1;						// 接收完成
				}
				RxState = 0;			// 回到状态0（校验失败则丢弃该包）
			}
			else
			{
				/* 正常数据字节（含校验字节），存入缓冲区 */
				BlueSerial_RxPacket[pRxPacket] = RxData;
				RxSum += RxData;
				pRxPacket++;

				/* 缓冲区溢出保护 */
				if (pRxPacket >= sizeof(BlueSerial_RxPacket))
				{
					RxState = 0;		// 丢弃，重新等包头
				}
			}
		}

		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}
