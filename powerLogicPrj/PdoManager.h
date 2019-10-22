#ifndef _PDO_MANAGER_H_
#define _PDO_MANAGER_H_

#include "stm32f10x.h"
#include "Singleton.h"

#define PDO0_IN_PORT   GPIOA
#define PDO0_IN_PIN    GPIO_Pin_4
#define PDO0_FILTER_PORT   GPIOA
#define PDO0_FILTER_PIN    GPIO_Pin_5      

#define PDO1_IN_PORT   GPIOC
#define PDO1_IN_PIN    GPIO_Pin_3
#define PDO1_FILTER_PORT   GPIOA
#define PDO1_FILTER_PIN    GPIO_Pin_3

#define PDO2_IN_PORT   GPIOA
#define PDO2_IN_PIN    GPIO_Pin_0
#define PDO2_FILTER_PORT   GPIOC
#define PDO2_FILTER_PIN    GPIO_Pin_2       

#define PDO3_IN_PORT   GPIOA
#define PDO3_IN_PIN    GPIO_Pin_1
#define PDO3_FILTER_PORT   GPIOC
#define PDO3_FILTER_PIN    GPIO_Pin_1

#define PDO4_IN_PORT   GPIOA
#define PDO4_IN_PIN    GPIO_Pin_2
#define PDO4_FILTER_PORT   GPIOC
#define PDO4_FILTER_PIN    GPIO_Pin_0

#define PDO5_IN_PORT   GPIOA
#define PDO5_IN_PIN    GPIO_Pin_7
#define PDO5_FILTER_PORT   GPIOC
#define PDO5_FILTER_PIN    GPIO_Pin_5

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

class PdoManager
{
private:
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    void open(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_SetBits(GPIOx, GPIO_Pin);}
    void close(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_ResetBits(GPIOx, GPIO_Pin);};
    volatile uint16_t ADCConvertedValue[50][6];     //50sample 6chanel
    volatile uint16_t m_adc_after_fiter[6];
    void adcDmaInit(void);
    uint16_t adcAverageFilter(uint8_t index){
        int sum = 0;
        for (int i=0;i<50;i++){
            sum += ADCConvertedValue[i][index];
        }
        return sum / 50;
    }
public:
    PdoManager(){
        
    }
    ~PdoManager(){}

    void pdoInit();

    void adcInit(void);

    uint16_t AdcGetVal(void);

    uint16_t getPDOADCAfterFilter(uint8_t index){return adcAverageFilter(index);}

    void openPDO0(){open(PDO0_IN_PORT, PDO0_IN_PIN);}
    void closePDO0(){close(PDO0_IN_PORT, PDO0_IN_PIN);}

    void openPDO1(){open(PDO1_IN_PORT, PDO1_IN_PIN);}
    void closePDO1(){close(PDO1_IN_PORT, PDO1_IN_PIN);}

    void openPDO2(){open(PDO2_IN_PORT, PDO2_IN_PIN);}
    void closePDO2(){close(PDO2_IN_PORT, PDO2_IN_PIN);}

    void openPDO3(){open(PDO3_IN_PORT, PDO3_IN_PIN);}
    void closePDO3(){close(PDO3_IN_PORT, PDO3_IN_PIN);}

    void openPDO4(){open(PDO4_IN_PORT, PDO4_IN_PIN);}
    void closePDO4(){close(PDO4_IN_PORT, PDO4_IN_PIN);}

    void openPDO5(){open(PDO5_IN_PORT, PDO5_IN_PIN);}
    void closePDO5(){close(PDO5_IN_PORT, PDO5_IN_PIN);}

};

typedef NormalSingleton<PdoManager>	Pdo_Manager;



#endif