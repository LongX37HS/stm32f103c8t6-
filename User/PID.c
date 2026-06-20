#include "stm32f10x.h"                  // Device header
#include "PID.h"

/**
  * 函    数：PID计算更新（带积分抗饱和）
  * 参    数：p  PID结构体指针
  * 返 回 值：无
  * 说    明：采用条件积分法 — 输出饱和时停止同向积分，防止积分饱和（Windup）
  */
void PID_Update(PID_t *p)
{
	/* 更新误差 */
	p->Error1 = p->Error0;
	p->Error0 = p->Target - p->Actual;

	/* 条件积分：只在输出未饱和、或者积分正在"退饱和"时才累加 */
	if ((p->Out < p->OutMax && p->Out > p->OutMin)			// 输出未饱和
		|| (p->Out >= p->OutMax && p->Error0 < 0)			// 已到上限，但误差为负（在往回拉）
		|| (p->Out <= p->OutMin && p->Error0 > 0))			// 已到下限，但误差为正（在往回拉）
	{
		p->ErrorInt += p->Error0;
	}

	/* 积分限幅（双保险，防止极端情况下积分值过大） */
	if (p->ErrorInt > p->IntegralMax)
		p->ErrorInt = p->IntegralMax;
	if (p->ErrorInt < p->IntegralMin)
		p->ErrorInt = p->IntegralMin;

	/* PID输出 */
	p->Out = p->Kp * p->Error0
		   + p->Ki * p->ErrorInt
		   + p->Kd * (p->Error0 - p->Error1);

	if(p->Out>0){
		p->Out+=p->Outoffset;
	}else if(p->Out<0){
		p->Out-=p->Outoffset;
	}
	
	
	/* 输出限幅 */
	
	
	
	
	if (p->Out > p->OutMax)
		p->Out = p->OutMax;
	if (p->Out < p->OutMin)
		p->Out = p->OutMin;
}

/**
  * 函    数：PID状态复位
  * 参    数：p  PID结构体指针
  * 返 回 值：无
  * 说    明：清零积分项和历史误差，用于翻车恢复、模式切换等场景
  */
void PID_Reset(PID_t *p)
{
	p->Error0   = 0;
	p->Error1   = 0;
	p->ErrorInt = 0;
	p->Out      = 0;
}
