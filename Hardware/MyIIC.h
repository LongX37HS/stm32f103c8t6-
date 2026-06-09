#ifndef __MYIIC_H__
#define __MYIIC_H__

void MyIIC_W_SCL(uint8_t BitValue);
void MyIIC_W_SDA(uint8_t BitValue);
uint8_t MyIIC_R_SDA(void);
void MyIIC_Init(void);
void MyIIC_Start(void);
void MyIIC_Stop(void);
void MyIIC_SendByte(uint8_t Byte);
uint8_t MyIIC_ReceiveByte(void);
void MyIIC_SendAck(uint8_t Ack);
uint8_t MyIIC_ReceiveAck(void);


#endif
