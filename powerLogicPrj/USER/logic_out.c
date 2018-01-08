#include "logic_out.h"
#include "stm32f10x.h"

void logic_pin_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		
	
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA<< (((uint32_t)BATT_EN_GPIO-(uint32_t)GPIOA)/0x400), ENABLE);
	GPIO_InitStructure.GPIO_Pin  = BATT_EN_PIN;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_Init(BATT_EN_GPIO,&GPIO_InitStructure);	
	
	/* Board_EN should be Push pull type in V0.2Board */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA<< (((uint32_t)BRD_nEN_GPIO-(uint32_t)GPIOA)/0x400), ENABLE);
	GPIO_InitStructure.GPIO_Pin  = BRD_nEN_PIN;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BRD_nEN_GPIO,&GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA<< (((uint32_t)PC_EN_GPIO-(uint32_t)GPIOA)/0x400), ENABLE);
	GPIO_InitStructure.GPIO_Pin  = PC_EN_PIN;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(PC_EN_GPIO, &GPIO_InitStructure);
	
	pc_en_line_high();
}

//end of file
