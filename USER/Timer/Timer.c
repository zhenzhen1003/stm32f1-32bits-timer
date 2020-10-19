/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#include "Timer.h"                  // Device header
#include "stm32f10x.h"                  // Device header


//设置测量频率和周期的函数
void Timer_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* PA0   TIM2脉冲输入*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   //PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//设置RCC
	RCC_ClocksTypeDef RCC_ClockFreq;
	SystemInit();//源自system_stm32f10x.c文件,只需要调用此函数,则可完成RCC的配置.
	RCC_GetClocksFreq(&RCC_ClockFreq);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    //使能TIM2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //使能TIM2时钟
	TIM_ITRxExternalClockConfig(TIM2,TIM_TS_ETRF); //timer的驱动时钟源默认是RCC，需要更改为外部ETR输入才行
	TIM_ITRxExternalClockConfig(TIM3,TIM_TS_ITR1); //更改TIM3的驱动时钟为外部ITR1，其实就是TIM2的溢出端
	//设置定时器
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;

   //配置TIMER2作为计数器
  TIM_DeInit(TIM2);

  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;   //计数周期为65535
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00;  //不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  //不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); // Time base configuration

	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);  //配置外部输入参数，不分频，上升沿触发


	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//使用更新事件为触发输入
	//选择TIM2为主定时器
	TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
	//关闭TIM定时器
	
	TIM_DeInit(TIM3);
	//配置TIM3定时器
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
	TIM_TimeBaseStructure.TIM_Prescaler = 0; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//设置TIM从模式为外部触发
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1);
	//触发方式为TIM2的ITR1线
	TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	//开启TIM3的从模式
	TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);
	
	TIM_SetCounter(TIM2, 0);
	TIM_Cmd(TIM2, ENABLE);
	TIM_SetCounter(TIM3, 0);
	TIM_Cmd(TIM3, ENABLE);
}

//定时器2通道1和通道2输入捕获配置
void TIM2_Cap_Init(u16 arr,u16 psc)
{     
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM2_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    //使能定时器和GPIO的时钟 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    //使能TIM2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟
    //初始化IO口，模式为输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  //PA0 清除之前设置  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 输入  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_0);   //PA0 下拉    
	
	  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;  //PA0 清除之前设置  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 输入  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_1);   //PA0 下拉    
    //初始化定时器2 TIM2   
    TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
    TIM_TimeBaseStructure.TIM_Prescaler =psc;   //预分频器   
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);   
    //初始化TIM2输入捕获参数
    TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01     选择输入端 IC1映射到TI1上
    TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;    //上升沿捕获
    TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
    TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;   //配置输入分频,不分频 
    TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM2, &TIM2_ICInitStructure);    
		
    TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01     选择输入端 IC1映射到TI1上
    TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;    //上升沿捕获
    TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
    TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;   //配置输入分频,不分频 
    TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM2, &TIM2_ICInitStructure);    
    //中断分组初始化
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器  
    TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC1IE捕获中断        
    TIM_Cmd(TIM2,ENABLE );     //使能定时器2 
}

u8  Capture_State=0;//输入捕获状态
u16 Capture_Value;//输入捕获值

//定时器2中断服务程序  
void TIM2_IRQHandler(void)
{ 
     if((Capture_State&0X80)==0)//还未成功捕获 1000 0000   
    {
        if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)       
        {       
            if(Capture_State&0X40)//已经捕获到高电平了0100 0000
            {
                if((Capture_State&0X3F)==0X3F)//高电平太长了 0011 1111
                {
                    Capture_State|=0X80;//标记成功捕获了一次 1000 0000   
                    Capture_Value=0XFFFF; //1111 1111
                }
                else 
                   Capture_State++;
            }    
        }
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
        {   
            if(Capture_State&0X40)//捕获到一个下降沿 0100 0000
            {               
                Capture_State|=0X80;//标记成功捕获到一次上升沿1000 0000 
                Capture_Value=TIM_GetCapture1(TIM2);
                TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
            }else //还未开始,第一次捕获上升沿
            {
                Capture_State=0;//清空
                Capture_Value=0;
                TIM_SetCounter(TIM2,0);
                Capture_State|=0X40;//标记捕获到了上升沿0100 0000
                TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling); //CC1P=1 设置为下降沿捕获
            }           
        }                                              
     } 
		if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
		{
			TIM_Cmd(TIM2,DISABLE );
		}
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2|TIM_IT_CC1|TIM_IT_Update); //清除中断标志位 
}
