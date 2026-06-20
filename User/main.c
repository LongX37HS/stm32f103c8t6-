#include "stm32f10x.h"                  // Device header
#include "stdlib.h"
#include "OLED.h"
#include "MPU6050.h"
#include "Timer.h"
#include "usart.h"
#include "motor.h"
#include "Encoder.h"
#include "Delay.h"
#include "math.h"
#include "PID.h"
#include "BlueSerial.h"

#define RIGHT 1
#define LEFT 0

//MPU6050测试
int16_t Ax,Az,Ay,Gx,Gz,Gy;
//速度环参数
float Leftspeed;
float Rightspeed;
float Avespeed;
float Diffspeed;

float Angleacc=0,AngleGyro=0,Angle=0;
float Alpha=0.01;//俯仰角互补滤波参数

uint8_t RunFlag=0;

PID_t Angle_PID={

	.Kp=3.34,
	.Ki=0.28,
	.Kd=7.04,


	.OutMax=100,
	.OutMin=-100,
	.IntegralMax=150,
	.IntegralMin=-150,
	
	.Outoffset=0
};

PID_t Speed_PID={

	.Kp=5.51,
	.Ki=0.027,
	.Kd=0,


	.OutMax=20,
	.OutMin=-20,
	.IntegralMax=30,
	.IntegralMin=-30,
};

//左右轮PWM，平均PWM，查分PWM
int8_t RIGHT_PWM,LEFT_PWM,AVE_PWM,DIF_PWM;

int main (void){
	MPU6050_Init();
	Timer_Init();
	motor_Init();
	Encoder_Init();
	OLED_Init();
	BlueSerial_Init();
	while(1){
		if (BlueSerial_RxFlag == 1)
		{
			//角度环
			float *pfloat=(float *)BlueSerial_RxPacket;
			OLED_ShowSignedNum(2,1,pfloat[0],2);
			OLED_ShowString(2,4,".");
			OLED_ShowNum(2,5,(int)(pfloat[0]*100)%100,2);
			Angle_PID.Kp=pfloat[0];

			OLED_ShowSignedNum(3,1,pfloat[1],2);
			OLED_ShowString(3,4,".");
			OLED_ShowNum(3,5,(int)(pfloat[1]*100)%100,2);
			Angle_PID.Ki=pfloat[1];
			
			OLED_ShowSignedNum(4,1,pfloat[2],2);
			OLED_ShowString(4,4,".");
			OLED_ShowNum(4,5,(int)(pfloat[2]*100)%100,2);
			Angle_PID.Kd=pfloat[2];

			//速度环
			OLED_ShowSignedNum(2,8,pfloat[3],2);
			OLED_ShowString(2,11,".");
			OLED_ShowNum(2,12,(int)(pfloat[3]*100)%100,2);
			Speed_PID.Kp=pfloat[3];

			OLED_ShowSignedNum(3,8,pfloat[4],2);
			OLED_ShowString(3,11,".");
			OLED_ShowNum(3,12,(int)(pfloat[4]*100)%100,2);
			Speed_PID.Ki=pfloat[4];
			
			OLED_ShowSignedNum(4,8,pfloat[5],2);
			OLED_ShowString(4,11,".");
			OLED_ShowNum(4,12,(int)(pfloat[5]*100)%100,2);
			Speed_PID.Kd=pfloat[5];
			
			Speed_PID.Target=pfloat[6]/25.0;
			DIF_PWM=pfloat[7]/2.0;
			
			
			
			BlueSerial_RxFlag = 0;
		}
		
		OLED_ShowSignedNum(1,1,Angle_PID.Actual,2);
		OLED_ShowString(1,4,".");
		OLED_ShowNum(1,5,(int)(Angle_PID.Actual*100)%100,2);
		OLED_ShowSignedNum(1,8,Speed_PID.Out,2);
		OLED_ShowString(1,11,".");
		OLED_ShowNum(1,12,(int)(Speed_PID.Out*100)%100,2);
//		MyUSART_Printf("%.2f %.2f %.2f\r\n", Angle_PID.Actual, Angle_PID.Target, Angle_PID.Out);
	}
}

void TIM3_IRQHandler(void){
	static uint8_t anglecount=0,speedcount=0;
	
	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)==SET){
		anglecount++,speedcount++;	
		if(anglecount>=10){
			anglecount=0;
			MPU6050_GetData(&Ax,&Az,&Ay,&Gx,&Gz,&Gy);
			Gx+=125;
			Angleacc=-atan2(Az,Ay)/3.14*180;
			Angleacc+=0;
			AngleGyro=Angle-Gx/32768.0*2000*0.01;
			Angle=Alpha*Angleacc+(1-Alpha)*AngleGyro;
			if(Angle>50||Angle<-50){
				RunFlag=1;
			}else{
				RunFlag=0;
			}
			/* 翻车/恢复瞬间复位 PID，清除积分残留 */
			static uint8_t lastRunFlag=0;
			if(RunFlag!=lastRunFlag){
				PID_Reset(&Angle_PID);
				PID_Reset(&Speed_PID);
				lastRunFlag=RunFlag;
			}
			if(!RunFlag){
				Angle_PID.Actual=Angle;
				PID_Update(&Angle_PID);
				AVE_PWM=-Angle_PID.Out;
				
				LEFT_PWM=AVE_PWM+DIF_PWM/2;
				RIGHT_PWM=AVE_PWM-DIF_PWM/2;
				
				if(LEFT_PWM>100){LEFT_PWM=100;}else if(LEFT_PWM<-100){LEFT_PWM=-100;}
				if(RIGHT_PWM>100){RIGHT_PWM=100;}else if(RIGHT_PWM<-100){RIGHT_PWM=-100;}
				
				motor_SetPWM(RIGHT,RIGHT_PWM);
				motor_SetPWM(LEFT,LEFT_PWM);
			}else{
				motor_SetPWM(RIGHT,0);
				motor_SetPWM(LEFT,0);
			}
		}
		if(speedcount>=30){
			speedcount=0;
			Leftspeed=Encoder_GetCounterL()/44.0/0.03/21.3;
			Rightspeed=Encoder_GetCounterR()/44.0/0.03/21.3;
			
			Avespeed=(Leftspeed+Rightspeed)/2.0;
			Diffspeed=Leftspeed-Rightspeed;
			
			if(!RunFlag){
				Speed_PID.Actual=Avespeed;
				PID_Update(&Speed_PID);
				Angle_PID.Target=Speed_PID.Out;
			}
		}
		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
		
	}
}

//#define RIGHT 1
//#define LEFT 2
////串口接收
//uint8_t USART_receive;
////编码器定时器计数
//uint8_t Encoder_delay;
////左右轮速度
//float SpeedR,SpeedL;
////PID控制
//float Target ,Actual,Out;
//float Kp=35.3, Ki=15.9, Kd=5;
//float Error0,Error1,ErrorInt;

//int main(void){
//	OLED_Init();
//	Timer_Init();
//	motor_Init();
//	Encoder_Init();
//	MyUSART_Init();
//	MyUSART_Printf("TEST");
//	while(1){
//		if (MyUSART_GetRxFlag()) {
//			USART_receive = MyUSART_GetRxData();
//			if(USART_receive == 0x45){
//				Target+=0.5;
////				if(Target>=4.5){Target=4.5;}
//			}else if(USART_receive == 0x46){
//				Target-=0.5;
////				if(Target<-4.5){Target=-4.5;}
//			}
//		}
////        MyUSART_Printf("SpeedL=%d.%02d L  SpeedR=%d.%02d R  Target=%d.%02d T  Out=%d.%02d O\r\n",
////			(int)SpeedL, abs((int)(SpeedL*100)%100),
////			(int)SpeedR, abs((int)(SpeedR*100)%100),     //实际速度
////			(int)Target, abs((int)(Target*100)%100),      //目标速度
////			(int)Out, abs((int)(Out*100)%100));           //输出占空比
//		MyUSART_Printf("%.2f %.2f %.2f\r\n", SpeedR, Target, Out);
//	}
//	
//}

//void TIM3_IRQHandler(void){
//	
//	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)==SET){
//		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
//	}
//	Encoder_delay++;
//	if(Encoder_delay>=40){
//		Encoder_delay=0;
//		SpeedL=Encoder_GetCounterL()/44.0/0.04/21.3;
//		SpeedR=Encoder_GetCounterR()/44.0/0.04/21.3;
//		Error1=Error0;
//		Error0=Target-SpeedR;
//		
//		ErrorInt+=Error0;
//		
//		Out=Kp*Error0+Ki*ErrorInt+Kd*(Error0-Error1);
//		
//		//输出限幅
//		if(Out>100){
//			Out=100;
////			if(Error0>0){
////				ErrorInt-=Error0;
////			}
//		}
//		if(Out<-100){
//			Out=-100;
////			if(Error0<0){
////				ErrorInt-=Error0;
////			}
//		}
//		motor_SetPWM(RIGHT,Out);
//	}
//}



////MPU6050测试
//int16_t Ax,Az,Ay,Gx,Gz,Gy;

////串口通讯
//uint8_t receive;

//int8_t speedL=0,speedR=0;

////编码器测速
//float count1;
//float count2;

//uint8_t Encoder_delay=0;
////PID控制
//float Target ,Actual,Out;
//float Kp=0.2, Ki=0.2, Kd=0;
//float Error0,Error1,ErrorInt;

//int main(){
//	Timer_Init();
//	motor_Init();
//	MPU6050_Init();
//	MyUSART_Init();
//	OLED_Init();	
//	MyUSART_Printf("U");
//	Encoder_Init();
//	while(1){
//		if (MyUSART_GetRxFlag()) {
//			receive = MyUSART_GetRxData();
//			//开环控制
////			if (receive == 0x41) {
////				speedR+=20;
////				if(speedR>100){
////					speedR=0;
////				}
////				motor_SetSpeed(1,speedR);
////			}else if(receive == 0x42){
////				speedR-=20;
////				if(speedR<-100){
////					speedR=0;
////				}
////				motor_SetSpeed(1,speedR);
////			}else if(receive == 0x43){
////				speedL+=20;
////				if(speedL>100){
////					speedL=0;
////				}
////				motor_SetSpeed(0,speedL);
////			}else if(receive == 0x44){
////				speedL-=20;
////				if(speedL<-100){
////					speedL=0;
////				}
////				motor_SetSpeed(0,speedL);
////			}
////			OLED_ShowSignedNum(4, 1, speedL,3);
////			OLED_ShowSignedNum(4, 5, speedR,3);
//			//闭环控制
//			if(receive == 0x45){
//				Target+=0.5;
//			}else if(receive == 0x46){
//				Target-=0.5;
//			}
//		}
//		//目标速度
//		OLED_ShowSignedNum(2,1,Target,1);
//		OLED_ShowString(2,3,".");
//		OLED_ShowNum(2,4,(int)(Target*100)%100,2);
////		OLED_ShowSignedNum(1,1,Ax,5);
////		OLED_ShowSignedNum(2,1,Az,5);
////		OLED_ShowSignedNum(3,1,Ay,5);
////		OLED_ShowSignedNum(1,8,Gx,5);
////		OLED_ShowSignedNum(2,8,Gz,5);
////		OLED_ShowSignedNum(3,8,Gy,5);
//		//实际速度
//		OLED_ShowSignedNum(1,1,count1,1);
//		OLED_ShowString(1,3,".");
//		OLED_ShowNum(1,4,(int)(count1*100)%100,2);
//		OLED_ShowSignedNum(1,7,count2,1);
//		OLED_ShowString(1,9,".");
//		OLED_ShowNum(1,10,(int)(count2*100)%100,2);
//		
//		//占空比
//		OLED_ShowSignedNum(3,1,Out,1);
//		OLED_ShowString(3,3,".");
//		OLED_ShowNum(3,4,(int)(Out*100)%100,2);
////		MyUSART_Printf("c1=%d.%02d  c2=%d.%02d\r\n",
////      (int32_t)count1, (int)(count1*100)%100,
////      (int32_t)count2, (int)(count2*100)%100);
//	}

//}


//void TIM3_IRQHandler(void){
//	
//	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update)==SET){
////		MPU6050_GetData(&Ax,&Az,&Ay,&Gx,&Gz,&Gy);
//		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
//		
//	}
//	Encoder_delay++;
//	if(Encoder_delay>=50){
//		Encoder_delay=0;
//		count1=Encoder_GetCounter1()/44.0/0.05/21.3;
//		count2=Encoder_GetCounter2()/44.0/0.05/21.3;
//		
//		Error1=Error0;
//		Error0=Target-count2;
//		
//		ErrorInt+=Error0;
//		
//		Out=Kp*Error0+Ki*ErrorInt+Kd*(Error0-Error1);
//		
//		//输出限幅
//		if(Out>100){
//			Out=100;
//		}
//		if(Out<-100){
//			Out=-100;
//		}
//		motor_SetSpeed(1,Out);
//		
//		
//	}
//}
////编码器测试
////int16_t count1;
////int16_t count2;

////int main(){
////	Encoder_Init();
////	motor_Init();
////	MPU6050_Init();
////	OLED_Init();
////	OLED_Clear();
////	while(1){
////		count1=Encoder_GetCounter1();
////		count2=Encoder_GetCounter2();
////		Delay_ms(100);
////		OLED_ShowSignedNum(1,1,count1,5);
////		OLED_ShowSignedNum(1,8,count2,5);
////	}
////}
