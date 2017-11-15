#include "uart5IapDevice.h"
#include "Timer.h"
#include "printf.h"

namespace
{
};

#define IAP_UART5 UART5

CUart5IapDevice iapDevice;
CCharDev& iap_device(iapDevice);

CUart5IapDevice::CUart5IapDevice()
	:CCharDev(200)
{
	
}

/**
* @brief  open
* @param  None
* @retval state
*/
int CUart5IapDevice::open()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint32_t RCC_APB2Periph_GPIOx;
	uint8_t GPIO_PinSource_Tx;
	uint8_t GPIO_PinSource_Rx;
//	uint8_t GPIO_AF_USARTx;
	GPIO_TypeDef *GPIOx_Tx;
	GPIO_TypeDef *GPIOx_Rx;

	if(IAP_UART5 == UART5)
	{
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC;
		GPIOx_Tx = GPIOC;
		GPIOx_Rx = GPIOD;
		GPIO_PinSource_Tx = GPIO_PinSource12;
		GPIO_PinSource_Rx = GPIO_PinSource2;

		/* open clock of GPIO */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);

		/* Config Pin: TXD RXD*/
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0<< GPIO_PinSource_Tx;
		GPIO_Init(GPIOx_Tx, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0<< GPIO_PinSource_Rx;
		GPIO_Init(GPIOx_Rx, &GPIO_InitStructure);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	}
	else
	{
		while(1);
	}
	/* Deinitializes the USARTx */
	USART_DeInit(IAP_UART5);

	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	NVIC_InitTypeDef NVIC_InitStructure;
	if(IAP_UART5 == UART5)
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	else
		while(1);
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(IAP_UART5,USART_IT_TC,ENABLE);
	USART_ITConfig(IAP_UART5,USART_IT_RXNE,ENABLE);
//	USART_ITConfig(IAP_UART5,USART_IT_IDLE,ENABLE);

	USART_Cmd(IAP_UART5, DISABLE);
	USART_Init(IAP_UART5, &USART_InitStructure);
	USART_Cmd(IAP_UART5, ENABLE);
	return 0;
}

/**
* @brief  close
* @param  None
* @retval 
*/
int CUart5IapDevice::close()
{
	return 0;
}

/**
  * @brief  write
	* @param  databuf: the data pointer wants to send
	* @param  len: data length
  * @retval actual send length
  */
int CUart5IapDevice::write(const uint8_t* buf, uint32_t len)
{
	return txBufQue_.push_array((uint8_t*)buf, len);
}

/**
  * @brief  read
	* @param  databuf: the data pointer wants to read
	* @param  len: data length
  * @retval actual read length
  */
int CUart5IapDevice::read(uint8_t* buf, uint32_t len)
{
	return rxBufQue_.pop_array(buf, len);
}

/**
  * @brief  runReceiver
	* @param  None
  * @retval None
  */
void CUart5IapDevice::runReceiver()
{
	if(USART_GetFlagStatus(IAP_UART5, USART_FLAG_ORE) == SET)
	{
		uint16_t tmp;
		tmp = IAP_UART5->SR;
		tmp += IAP_UART5->DR;
	}
	
	update_data_break_flag();
}

void CUart5IapDevice::runTransmitter()
{
	//give up if TX buff is not empty
	if(USART_GetFlagStatus(IAP_UART5, USART_FLAG_TXE) != SET)
		return;
	
	if(txBufQue_.elemsInQue() > 0)
	{
		USART_SendData(IAP_UART5, txBufQue_.front());
		txBufQue_.pop();
	}
}

/**
  * @brief  data_in_read_buf
	* @param  None
  * @retval bytes
  */
uint32_t CUart5IapDevice::data_in_read_buf()
{
	return rxBufQue_.elemsInQue();
}

/**
  * @brief  clear read buffer
	* @param  None
  * @retval None
  */
void CUart5IapDevice::clear_read_buf()
{
	rxBufQue_.clear();
}

/**
  * @brief  data in write buffer
	* @param  None
  * @retval number of bytes
  */
uint32_t CUart5IapDevice::data_in_write_buf()
{
	return txBufQue_.elemsInQue();
}

/**
  * @brief  data in write buffer
	* @param  None
  * @retval number of bytes
  */
uint32_t CUart5IapDevice::freesize_in_write_buf()
{
	return txBufQue_.emptyElemsInQue();
}

bool CUart5IapDevice::isTransmitterIdle()
{ 
	runTransmitter();
	return (txBufQue_.elemsInQue() == 0 && USART_GetFlagStatus(IAP_UART5, USART_FLAG_TXE) == SET);
}

/**
	* @brief  run modbus slaver for 1 time
	* @param  None
	* @retval None
	*/

void CUart5IapDevice::doIrqRun()
{
	if(USART_GetITStatus(IAP_UART5, USART_IT_RXNE) == SET)
	{
		rxBufQue_.push(IAP_UART5->DR);
		USART_ClearITPendingBit(IAP_UART5, USART_IT_RXNE);
	}
	else if(USART_GetITStatus(IAP_UART5, USART_IT_TC) == SET)
	{
		USART_ClearITPendingBit(IAP_UART5, USART_IT_TC);
		if(txBufQue_.elemsInQue() > 0)
		{
			USART_SendData(IAP_UART5, txBufQue_.front());
			txBufQue_.pop();
		}
	}
}

extern "C" {

	void UART5_IRQHandler(void)
	{
		iapDevice.doIrqRun();
	}

}

//end of file
