#ifndef __PID_H
#define __PID_H

typedef struct {
	float Target;
	float Actual;
	float Out;

	float Kp;
	float Ki;
	float Kd;

	float Error0;
	float Error1;
	float ErrorInt;

	float OutMax;
	float OutMin;
	float IntegralMax;	// 积分限幅上限（通常设为 OutMax）
	float IntegralMin;	// 积分限幅下限（通常设为 OutMin）
	
	float Outoffset; 
}PID_t;

void PID_Update(PID_t *p);
void PID_Reset(PID_t *p);	// 清零积分和历史误差，用于翻车恢复

#endif
