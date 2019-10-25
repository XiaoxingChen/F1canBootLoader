#ifndef _PDO_MANAGER_H_
#define _PDO_MANAGER_H_

#include "stm32f10x.h"
#include "Singleton.h"
#include "adcManager.h"

#define PDO0_IN_PORT   GPIOA
#define PDO0_IN_PIN    GPIO_Pin_4

#define PDO1_IN_PORT   GPIOC
#define PDO1_IN_PIN    GPIO_Pin_3


#define PDO2_IN_PORT   GPIOA
#define PDO2_IN_PIN    GPIO_Pin_0
      

#define PDO3_IN_PORT   GPIOA
#define PDO3_IN_PIN    GPIO_Pin_1


#define PDO4_IN_PORT   GPIOA
#define PDO4_IN_PIN    GPIO_Pin_2

#define PDO5_IN_PORT	GPIOA
#define PDO5_IN_PIN    GPIO_Pin_7


class PdoManager
{
private:
    GPIO_InitTypeDef GPIO_InitStructure;
    void open(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_SetBits(GPIOx, GPIO_Pin);}
    void close(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){GPIO_ResetBits(GPIOx, GPIO_Pin);};
public:
    PdoManager(){
        
    }
    ~PdoManager(){}

    void pdoInit();

    float getPDOCurrent(uint8_t index){
		return (adc_Manager::Instance()->getPDOADC(index)/4096*3.3f*500.0f)/750.0f;
	}

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