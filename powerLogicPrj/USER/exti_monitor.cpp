#include "exti_monitor.h"
#include "power_state_data.h"
#include "power_state_recoder_manager.h"
#include "Timer.h"
void EXTI_GPIO_Init(void) 			//powergood pc15
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void EXTI_Init(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);//开启中断线
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line15 ; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;        
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;    
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                  
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStructure); 
}


#ifdef __cplusplus
extern "C" {
#endif 
void EXTI15_10_IRQnHandler (void)
{
	if(PSGlobalData::Instance()->is_close_exti_interrupt)
	{
		if (EXTI_GetITStatus(EXTI_Line15) != RESET)
		{
			writeStateInPositionInFlash(PSGlobalData::Instance()->power_good_state_address_now, 0x01);
			PSGlobalData::Instance()->is_close_exti_interrupt = false;			//关闭外部中断，不检测power_good上升沿
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line15);
}

#ifdef __cplusplus
}
#endif 



