#include "stm32f10x.h"
#include "CommonConfig.h"
#include "core_cm3.h"

void CommonConfig()
{
		__set_PRIMASK(0);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

void __set_PRIMASK(uint32_t priMask)
{
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) );
}
