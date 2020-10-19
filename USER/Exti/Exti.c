#include "Exti.h"
#include "stm32f10x.h"                  // Device header
#include "Delay.h"


void PA1_EXIT1_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  //开启 PA1和复用功能时钟
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);     //设置优先级分组
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //EXTI1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能向量中断控制器
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;     //设置PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //下拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	
	
	EXTI_InitTypeDef EXTI_InitStructure;      //设置中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line1; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) == 	SET)
	{
		EXTI->IMR &= ~(EXTI_Line1);  //失能外部中断1
		delay_ms(1000);
		TIM_Cmd(TIM2,DISABLE);
		TIM_Cmd(TIM3,DISABLE);
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
