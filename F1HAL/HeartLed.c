#include "stm32f10x.h"
#include "HeartLed_def.h"
#include "Timer.h"

Timer heart_frequency(500,500);

void Initial_HeartLED(void)
{
	uint32_t RCC_AHB1Periphx;
	GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA<< (((uint32_t)HEART_LED_GPIO-(uint32_t)GPIOA)/0x400), ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = HEART_LED_PIN;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_Init(HEART_LED_GPIO, &GPIO_InitStructure);
}

void HeartLed_Run()
{
	static bool LedOn = true;
	if(heart_frequency.isAbsoluteTimeUp())
	{
		LedOn = !LedOn;
	}
	
	if(LedOn)
		GPIO_SetBits(HEART_LED_GPIO, HEART_LED_PIN);
	else
		GPIO_ResetBits(HEART_LED_GPIO, HEART_LED_PIN);
}
//end of file
