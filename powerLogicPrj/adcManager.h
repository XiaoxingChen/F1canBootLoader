#ifndef _ADC_MANAGER_H_
#define _ADC_MANAGER_H_
#include "stm32f10x.h"
#include "Singleton.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

#define PDO0_FILTER_PORT   GPIOA
#define PDO0_FILTER_PIN    GPIO_Pin_5 
#define PDO1_FILTER_PORT   GPIOA
#define PDO1_FILTER_PIN    GPIO_Pin_3
#define PDO2_FILTER_PORT   GPIOC
#define PDO2_FILTER_PIN    GPIO_Pin_2  
#define PDO3_FILTER_PORT   GPIOC
#define PDO3_FILTER_PIN    GPIO_Pin_1
#define PDO4_FILTER_PORT   GPIOC
#define PDO4_FILTER_PIN    GPIO_Pin_0
#define PDO5_FILTER_PORT   GPIOC
#define PDO5_FILTER_PIN    GPIO_Pin_5

#define Brake_FILTER_PORT   GPIOB
#define Brake_FILTER_PIN    GPIO_Pin_1

#define VBAT_PORT           GPIOB
#define VBAT_PIN            GPIO_Pin_0

class adcManager
{
private:
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    volatile uint16_t ADCConvertedValue[10][8];     //10sample 6chanel
    uint16_t adcAverageFilter(uint8_t index){
        int sum = 0;
        for (int i=0;i<10;i++){
            sum += ADCConvertedValue[i][index];
        }
        return sum / 10;
    }
    void adcDmaInit();
public:
    adcManager(){}
    ~adcManager(){}
    void adcInit();
    uint16_t getPDOADC(uint8_t index){return adcAverageFilter(index);}
    uint16_t getBrakeADC(){return adcAverageFilter(7);}
    uint16_t getVBatADC(){return adcAverageFilter(6);}
};

typedef NormalSingleton<adcManager>	adc_Manager;



#endif