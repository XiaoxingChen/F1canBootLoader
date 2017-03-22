/********************************************************************************
* @file    CConsole.h
* @author  Chenxx
* @version V1.0
* @date    2016-04-21
* @brief   this file defines the Console function. Only for output logs. 
*	   			No receive function. Do not need to care about hardware platform.
* release information: 
*	2016/07/01: chenxx add noneDMA mode.
* 2016/07/24: chenxx fixed the bug in printf: stop sending when overflow
* 2016/08/26: chenxx completed the TX DMA mode.
********************************************************************************/
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "stm32f10x.h"
#include "Singleton.h"
#include "ringque.h"

class CConsole
{
public:
	CConsole();
	~CConsole();
	uint16_t send_array(char*, uint16_t);
	int printf(const char* fmt, ...) __attribute__((format(printf,2,3)));
	void putc(const char c);
	void puts(const char* s);
	enum{
		TXBUF_SIZE = 512,
	};
	void postErr();
	void runTransmitter();
	void run();
	bool isIdel();

public:
	class COstreamDev
	{
		public:
			virtual uint16_t write(uint8_t*, uint16_t) = 0;
			virtual bool open() = 0;
			virtual bool close() {return true;}
			virtual void runTransmitter() {}
			virtual uint16_t getFreeSize() = 0;
			virtual bool isIdel() = 0;
	};
	enum OstreamDevEnum
	{
		UART_DEV = 0,
		CAN_DEV,
		RTT_DEV,
		ETH_DEV,
		NUM_OF_DEV
	};
	
private:
	static char TxBuf_[TXBUF_SIZE]; //for txQueue_.
	ringque<char> txQueue_;
	static char vsnprintfBuf_[TXBUF_SIZE]; //for sprintf
	uint16_t overflowCounter_;
	static COstreamDev* ConsoleDevTab_[NUM_OF_DEV];
};

typedef NormalSingleton<CConsole> Console;
#define postErr(msg) printf("Error: %s(%d)-%s(): %s\r\n", __FILE__, __LINE__, __FUNCTION__, msg)

#endif
//end of file
