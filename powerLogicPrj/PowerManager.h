#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_
#include "stm32f10x.h"
#include "Singleton.h"
#define IMX8_PORT   GPIOB
#define IMX8_PIN    GPIO_Pin_8

class PowerManager
{
private:
    GPIO_InitTypeDef GPIO_InitStructure;
    void open(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_SetBits(GPIOx, GPIO_Pin);}
    void close(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_ResetBits(GPIOx, GPIO_Pin);};
public:
    PowerManager(/* args */){
		
	}
    ~PowerManager(){}
    void init();
		
    void openiMX8(){open(GPIOB, GPIO_Pin_8);}
    void closeiMX8(){close(GPIOB, GPIO_Pin_8);}
	
    void openLte(){open(GPIOB, GPIO_Pin_5);}
    void closeLte(){close(GPIOB, GPIO_Pin_5);}
	
    void openSubIG(){open(GPIOB, GPIO_Pin_4);}
    void closeSub1G(){close(GPIOB, GPIO_Pin_4);}
	
    void openSwitch(){open(GPIOD,GPIO_Pin_2);}
    void closeSwitch(){close(GPIOD,GPIO_Pin_2);}
	
    void openWifi(){open(GPIOC,GPIO_Pin_11);}
    void closeWifi(){close(GPIOC,GPIO_Pin_11);}
};

typedef NormalSingleton<PowerManager>	Power_Manager;




#endif


