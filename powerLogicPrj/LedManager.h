#ifndef _LED_MANAGER_H_
#define _LED_MANAGER_H_
#include "stm32f10x.h"
#include "Singleton.h"

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
		
    }
    ~LedManager(){}

	void init();

    void openLED(){open(LED_PORT, LED_PIN);}
    void closeLED(){close(LED_PORT, LED_PIN);}
};

typedef NormalSingleton<LedManager>	Led_Manager;




#endif