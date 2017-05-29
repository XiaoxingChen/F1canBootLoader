/********************************************************************************
* @file    CConsole.h
* @author  Chenxx
* @version V1.0
* @date    2016-04-21
* @brief   this file defines the Console function. Adjusted from CConsole
*	   This is for STM32F4.
* release information: 
*	2016/07/01: chenxx add noneDMA mode.
* 2016/07/24: chenxx fixed the bug in printf: stop sending when overflow
* 2016/08/26: chenxx completed the TX DMA mode.
********************************************************************************/
#include "Console.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef USE_MINI_PRINT
#	include <stdio.h>
#else
#	include "rtt_vsnprintf.h"
#endif

// <<< Use Configuration Wizard in Context Menu >>>

// <e>Enable UartConsole
#define ENABLE_UART_CONSOLE		0 
// </e>

// <e>Enable CanConsole
#define ENABLE_CAN_CONSOLE		1 
// </e>

// <e>Enable RttConsole
#define ENABLE_RTT_CONSOLE		0 
// </e>

// <e>Enable EthConsole
#define ENABLE_ETH_CONSOLE		0 
// </e>

// <<< end of configuration section >>>

#if ENABLE_UART_CONSOLE
	#include "Console.h"
#endif
	
#if ENABLE_CAN_CONSOLE
	#include "CCanConsole.h"
#endif
	
#if ENABLE_RTT_CONSOLE	
	#include "CRttConsole.h"
#endif
	
#if ENABLE_ETH_CONSOLE	
	#include "CEthConsole.h"
#endif

#if ENABLE_UART_CONSOLE || ENABLE_CAN_CONSOLE || ENABLE_RTT_CONSOLE || ENABLE_ETH_CONSOLE
#	define CONSOLE_UNSILENT
#endif

char CConsole::TxBuf_[TXBUF_SIZE];		//buffer for None DMA Mode txQueue_
char CConsole::vsnprintfBuf_[TXBUF_SIZE];	//for sprintf
CConsole::COstreamDev* CConsole::ConsoleDevTab_[NUM_OF_DEV];
/**
  * @brief  Constructor
	* @param  None
  * @retval None
  */
CConsole::CConsole()
	:txQueue_(TxBuf_, TXBUF_SIZE),
	overflowCounter_(0)
{
#if ENABLE_UART_CONSOLE
	ConsoleDevTab_[UART_DEV] = new CUartConsole;
#else 
	ConsoleDevTab_[UART_DEV] = NULL;
#endif
	
#if ENABLE_CAN_CONSOLE
	ConsoleDevTab_[CAN_DEV] = new CCanConsole;
#else
	ConsoleDevTab_[CAN_DEV] = NULL;
#endif
	
#if ENABLE_RTT_CONSOLE	
	ConsoleDevTab_[RTT_DEV] = new CRttConsole;
#else
	ConsoleDevTab_[RTT_DEV] = NULL;
#endif
	
#if ENABLE_ETH_CONSOLE	
	ConsoleDevTab_[ETH_DEV] = new CEthConsole;
#else
	ConsoleDevTab_[ETH_DEV] = NULL;
#endif
	
	for(int i = 0; i < NUM_OF_DEV; i++)
	{
		if(ConsoleDevTab_[i] != NULL)
		{
			ConsoleDevTab_[i]->open();
		}
	}
	
}

/**
  * @brief  send array
  * @param  None
  * @retval char get
  */
uint16_t CConsole::send_array(char* buf, uint16_t len)
{
	uint16_t res;
	//TODO add mutex lock here
	res = txQueue_.push_array(buf, len);
	//TODO release mutex lock here
	runTransmitter();
	return res;
}

/**
  * @brief  printf a string without DMA controller.
	*					User should call the CConsole::run()
  * @param  None
  * @retval number of bytes were sent
  */
int CConsole::printf(const char* fmt, ...)
{
	va_list args;
	int n;
	int ret;
	
	//TODO lock vsnprintf mutex
	va_start(args, fmt);
#ifndef USE_MINI_PRINT
	n = vsnprintf(vsnprintfBuf_, TXBUF_SIZE, fmt, args);
#else
	n = SEGGER_RTT_vsnprintf(vsnprintfBuf_, TXBUF_SIZE, fmt, &args);
#endif
	va_end(args);
	if(n > TXBUF_SIZE) n = TXBUF_SIZE;
	
	ret = send_array(vsnprintfBuf_, n);
	//TODO release vsnprintf mutex
	return ret;
}

/**
  * @brief  for None Dma Mode
  * @param  char to send
  * @retval None
  */
void CConsole::putc(const char c)
{
	//TODO add mutex lock here
	txQueue_.push(c);
	//TODO release mutex lock here
	runTransmitter();
}

/**
  * @brief  for None Dma Mode
  * @param  string to send
  * @retval None
  */
void CConsole::puts(const char* s)
{
	//TODO add mutex lock here
	txQueue_.push_array((char*)s, strlen(s));
	//TODO release mutex lock here
	runTransmitter();
}

/**
  * @brief  run UART transmitter
  * @param  None
  * @retval None
  */
void CConsole::runTransmitter()
{
	const uint8_t BUFF_SIZE = 32;
	uint8_t tempBuff[BUFF_SIZE];
	uint8_t copyLen = txQueue_.elemsInQue();
	
	//find the minium between: txQue.elems, ConsoleDev.freesize, BUFF_SIZE
	for(int i = 0; i < NUM_OF_DEV; i++)
	{
		if(ConsoleDevTab_[i] != NULL)
		{
			ConsoleDevTab_[i]->runTransmitter();
			if(ConsoleDevTab_[i]->getFreeSize() < copyLen)
				copyLen = ConsoleDevTab_[i]->getFreeSize();
		}
	}
	if(0 == copyLen) return;
	if(copyLen > BUFF_SIZE) copyLen = BUFF_SIZE;

	txQueue_.pop_array((char*)tempBuff, copyLen);
	//write data into dev
	for(int i = 0; i < NUM_OF_DEV; i++)
	{
		if(ConsoleDevTab_[i] != NULL)
		{
			ConsoleDevTab_[i]->write(tempBuff, copyLen);
			ConsoleDevTab_[i]->runTransmitter();
		}
	}
}

/**
  * @brief  confirm if all transmitter is Idel.
  * @param  None
  * @retval None
  */
bool CConsole::isIdel()
{
	for(int i = 0; i < NUM_OF_DEV; i++)
	{
		if(ConsoleDevTab_[i] != NULL && !ConsoleDevTab_[i]->isIdel())
		{
			//any device busy means Console busy
			return false;
		}
	}
	//all device idel means Console idel
	return true;
}

//end of file
