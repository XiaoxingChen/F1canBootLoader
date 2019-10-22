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
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    }
    ~PdoManager(){}

    void pdoInit();

    void adcInit(void);

    uint16_t AdcGetVal(void);

    //uint16_t getPDOADC(uint8_t index){return ADCConvertedValue[index];}

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

void PdoManager::pdoInit()
{
    GPIO_InitStructure.GPIO_Pin = PDO5_IN_PIN | PDO4_IN_PIN | PDO3_IN_PIN | PDO2_IN_PIN | PDO0_IN_PIN;			
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin=PDO1_IN_PIN;			
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

void PdoManager::adcDmaInit(void)
{
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 6*50;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void PdoManager::adcInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
	/* ADCCLK = PCLK2/2 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div2); 

	GPIO_InitStructure.GPIO_Pin = PDO2_FILTER_PIN | PDO3_FILTER_PIN | PDO4_FILTER_PIN |PDO5_FILTER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PDO0_FILTER_PIN | PDO1_FILTER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;/*ADC工作在独立模式*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;/*软件触发*/
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 6;
	ADC_Init(ADC1, &ADC_InitStructure);	
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 5, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_239Cycles5);

	adcDmaInit();/*DMA配置*/	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE); 
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Enable ADC1 reset calibration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/*开始校准*/
	ADC_StartCalibration(ADC1);
	/* 等待校准结束*/
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);/*开启转换*/     
}

uint16_t PdoManager::AdcGetVal(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);/*开启转换*/
	return ADC_GetConversionValue(ADC1);
}

typedef NormalSingleton<PdoManager>	Pdo_Manager;

#endif