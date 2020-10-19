#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "stdio.h"
#include "Picture.h"
#include "Timer.h"
#include "key.h"
#include "Exti.h"
#include "DMA.h"

extern u8 i;  //按键标志位
extern u8 j;  //按键标志位
extern u8 k;       
extern u8  Capture_State;//输入捕获状态                            
extern u16 Capture_Value;//输入捕获值 
extern u16 ADC_ConvertVal[6000];

u8 tmp[10];

int main(void)
{
	u32 temp=0;
	double temp1=0;
	u32 temp2=0;
	u32 posicnt=0;
	double duty_dma=0;
	delay_init();
	LCD_Init();
	PA4_EXTI4_Configuration();
	PA5_EXTI5_Configuration();
	PA10_EXTI10_Configuration();
	LCD_BLK = 0;
	LCD_Clear(WHITE);
	LCD_DrawFont_GBK16(24, 0, BLUE, WHITE, "数字频率计");
	LCD_DrawFont_GBK16(10, 30, BLUE, WHITE, "1.测量频率");
	LCD_DrawFont_GBK16(10, 50, BLUE, WHITE, "2.测量时间间隔");
	LCD_DrawFont_GBK16(10, 70, BLUE, WHITE, "3.测量占空比");
	while(1)
	{
		if(i==1)
		{
			delay_ms(900);
			temp=TIM2->CNT+TIM3->CNT*65535-1;
			sprintf(tmp,"%d",temp);
	 		if(k==0)
			{
				LCD_Clear(WHITE);
				LCD_DrawFont_GBK16(0, 40, BLUE, WHITE, "频率:");
				LCD_DrawFont_GBK16(110, 60, BLUE, WHITE, "Hz");
				LCD_DrawFont_GBK16(10, 60, RED, WHITE, tmp);
				k=1;
			}
		}
		if(i==2)
		{
			delay_ms(500);
			if(Capture_State&0X80)//成功捕获到了一次高电平
      {
        temp=Capture_State&0X3F;
        temp*=65536;//溢出时间总和
        temp+=Capture_Value;//得到总的高电平时间
      }
			temp1=(double)temp/10;
			temp1=0.9732*temp1-0.0046;
			sprintf(tmp,"%.1f",temp1);
			if(k==0)
			{
				LCD_Clear(WHITE);
				LCD_DrawFont_GBK16(0, 40, BLUE, WHITE, "时间间隔:");
				LCD_DrawFont_GBK16(110, 60, BLUE, WHITE, "us");
				LCD_DrawFont_GBK16(10, 60, RED, WHITE, tmp);
				k=1;
			}
		}
		if(i==3)
		{
			delay_ms(600);
			DMA_Configuration();
			if(Capture_State&0X80)//成功捕获到了一次高电平
      {
					temp2=Capture_State&0X3F;
					temp2*=65536;//溢出时间总和
					temp2+=Capture_Value;//得到总的高电平时间
      }
			Timer_Configuration();
			PA1_EXIT1_Configuration();
			delay_ms(600);
			temp=TIM2->CNT+TIM3->CNT*65535-1;
			if(temp>=30000)
				temp=temp-3;
			if(temp>=1000000)
				temp=temp-300;
			if(k==0)
			{
				for(int a=0;a<6000;a++)
				{
					if(ADC_ConvertVal[a]>0x01)
						posicnt++;
				}
				duty_dma=(double)posicnt/60;
			}
			temp1=(double)temp2*(double)temp/10000;
			if(temp>=4000)
			{
				temp1=duty_dma;
			}
			sprintf(tmp,"%.1f",temp1); 
			if(k==0)
			{
				LCD_Clear(WHITE);
				LCD_DrawFont_GBK16(0, 40, BLUE, WHITE, "占空比:");
				LCD_DrawFont_GBK16(110, 40, RED, WHITE, "%");
				LCD_DrawFont_GBK16(70, 40, RED, WHITE, tmp);
				k=1;
			}
		}
		LCD_DrawFont_GBK16(24, 0, BLUE, WHITE, "数字频率计");
	}
}
