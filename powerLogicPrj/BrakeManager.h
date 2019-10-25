#ifndef _BRAKE_MANAGER_H_
#define _BRAKE_MANAGER_H_
#include "stm32f10x.h"
#include "Singleton.h"
#include "adcManager.h"

class BrakeManager
{
private:
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    void open(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_SetBits(GPIOx, GPIO_Pin);}
    void close(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_ResetBits(GPIOx, GPIO_Pin);}
public:
    BrakeManager(/* args */){}
    ~BrakeManager(){}

    void outInit();
    
    void inInit();

    float getCurrent(){
		return (adc_Manager::Instance()->getBrakeADC()/4096*3.3f*500.0f)/750.0f;
    }

    void openBrake(){open(GPIOC, GPIO_Pin_7);}
    void closeBrake(){close(GPIOC, GPIO_Pin_7);}
    
    bool getBrakeState(){
        return (bool)GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
    }
};

typedef NormalSingleton<BrakeManager>	Brake_Manager;


#endif //_BRAKE_MANAGER_H_