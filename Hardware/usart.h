#ifndef __USART_H
#define __USART_H

#include "stdio.h"
void MyUSART_Init(void);

void MyUSART_Send(uint8_t Byte);

int fputc(int ch,FILE *f);


#endif
