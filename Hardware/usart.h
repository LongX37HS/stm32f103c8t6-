#ifndef __USART_H
#define __USART_H

#include "stdio.h"
void MyUSART_Init(void);

void MyUSART_Send(uint8_t Byte);

int fputc(int ch,FILE *f);

uint8_t MyUSART_GetRxFlag(void);


uint8_t MyUSART_GetRxData(void);
void MyUSART_SendArray(uint8_t *Array, uint16_t Length);
void MyUSART_SendString(char *String);
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
void MyUSART_SendNumber(uint32_t Number, uint8_t Length);
void MyUSART_Printf(char *format, ...);
void USART3_IRQHandler(void);

#endif
