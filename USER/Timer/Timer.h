#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f10x.h"                  // Device header


void Timer_Configuration(void);
void TIM2_Cap_Init(u16 arr,u16 psc);

#endif
