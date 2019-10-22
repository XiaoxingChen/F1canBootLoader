#ifndef _LED_MANAGER_H_
#define _LED_MANAGER_H_
#include "stm32f10x.h"

#define LED_PORT   	GPIOC
#define LED_PIN		GPIO_Pin_13

class LedManager
{
private:
     GPIO_InitTypeDef GPIO_InitStructure;
    void open(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_ResetBits(GPIOx, GPIO_Pin);}
    void close(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_SetBits(GPIOx, GPIO_Pin);};		//low-trigger
public:
    LedManager(/* args */){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    }
    ~LedManager(){}

	void init();

    void openLED(){open(LED_PORT, LED_PIN);}
    void closeLED(){close(LED_PORT, LED_PIN);}
};

void LedManager::init()
{
    GPIO_InitStructure.GPIO_Pin = LED_PIN;			
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(LED_PORT,&GPIO_InitStructure);
}

#endif