#ifndef _CSPICONSOLE_H_
#define _CSPICONSOLE_H_

#include "stm32f10x.h"
#include "Console.h"


// <o> Console on CAN: <1=>CAN1 <2=>CAN2
// 	<i>Default: 1
#define STM32_CONSOLE_SPI 1

/* Macros for Board Support */

#if STM32_CONSOLE_SPI == 1
	#define CONSOLE_SPI						SPI1
	#define CONSOLE_SPI_IOGROUP		CCanRouter::GROUP_A5
	#define CONSOLE_USE_SPI1 	1

#elif STM32_CONSOLE_SPI == 2
	#define CONSOLE_SPI						SPI2
	#define CONSOLE_SPI_IOGROUP		CCanRouter::GROUP_B13
	#define CONSOLE_USE_SPI2 	1
	
#else
	#error
#endif


class CSpiConsole
	:public CConsole::COstreamDev
{
public:
	CSpiConsole();
	virtual ~CSpiConsole(){close();};
	
	virtual uint16_t write(uint8_t*, uint16_t);
	virtual bool open();
	virtual void runTransmitter();
	virtual uint16_t getFreeSize();
	virtual bool isIdel();
	virtual bool isOpen(){return _inited;}
private:
	bool _inited;
	SPI_TypeDef * _SPI;
	int _usedSize;
	uint8_t _txBuf[100];
};


#endif
