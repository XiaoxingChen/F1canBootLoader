/********************************************************************************
* @file    CUartConsole.h
* @author  Chenxx
* @version V1.0
* @date    2016-05-13
* @brief   this file defines the Console function that can printf with uart.
*	   This is for STM32F1.
* release information:
*	2016/07/21 chenxx: add postErr() macro function
* 2016/09/17 chenxx: add baseRun() and terminalRun()
* 2016/09/17 chenxx: fix data type bug in .cpp rxDMA_to_rxQueue() rxDataSize
********************************************************************************/
#ifndef _CUARTCONSOLE_H_
#define _CUARTCONSOLE_H_
#include "stm32f10x.h"
#include "Singleton.h"

/****** UART IOGROUP DMAST reference ******/
//	UARTx		TX_DMACH

//	UART1		DMA1_Channel4

//	UART2		DMA1_Channel7

//	UART3		DMA1_Channel2

//	UART4		DMA2_Channel5

//BSP MACROS
#define CONSOLE_SILENT				0
#define CONSOLE_TX_USE_DMA		1	//not finished yet
#define CONSOLE_RX_USE_DMA		1

/* Macros for Board Support */
//#define CONSOLE_USE_UART2			1//change here
//#define CONSOLE_IOGROUP_A2		1//change here

#define CONSOLE_USE_UART1			1//change here
#define CONSOLE_IOGROUP_A9		1//change here

#ifdef 	CONSOLE_USE_UART1 
	#define 	CONSOLE_UART	USART1	
#elif 	CONSOLE_USE_UART2
	#define 	CONSOLE_UART	USART2	
#elif 	CONSOLE_USE_UART3
	#define 	CONSOLE_UART	USART3
#elif 	CONSOLE_USE_UART4
	#define 	CONSOLE_UART	UART4
#elif 	CONSOLE_USE_UART5
	#define 	CONSOLE_UART	UART5
#endif

/**
  * @brief  Macros for Console send 
  * @note  uncomment the CONSOLE_TX_DMACH macro can
	*		make the Console send port work at DMA mode
  */
#if CONSOLE_TX_USE_DMA
#ifdef 	CONSOLE_USE_UART1 
	#define 	CONSOLE_TX_DMACH	DMA1_Channel4	
#elif 	CONSOLE_USE_UART2
	#define 	CONSOLE_TX_DMACH	DMA1_Channel7	
#elif 	CONSOLE_USE_UART3
	#define 	CONSOLE_TX_DMACH	DMA1_Channel12
#elif 	CONSOLE_USE_UART4
	#define 	CONSOLE_TX_DMACH	DMA2_Channel15
#endif
#endif //CONSOLE_TX_USE_DMA

/**
  * @brief  Macros for Console receive 
  * @note  uncomment the CONSOLE_RX_DMACH macro can
	*		make the Console send port work at DMA mode
  */
#if CONSOLE_RX_USE_DMA
#ifdef 	CONSOLE_USE_UART1 
	#define 	CONSOLE_RX_DMACH	DMA1_Channel5	
#elif 	CONSOLE_USE_UART2
	#define 	CONSOLE_RX_DMACH	DMA1_Channel6	
#elif 	CONSOLE_USE_UART3
	#define 	CONSOLE_RX_DMACH	DMA1_Channel3
#elif 	CONSOLE_USE_UART4
	#define 	CONSOLE_RX_DMACH	DMA2_Channel3
#endif
#endif //CONSOLE_RX_USE_DMA

#define CONSOLE_DMA				((DMA_TypeDef *)((uint32_t)CONSOLE_TX_DMACH&0xFFFFFC00))

class CUartConsole
{
public:
public:
	CUartConsole();
	~CUartConsole();
	uint16_t send_array(char*, uint16_t);
	uint16_t read_array(char*, uint16_t);
	int printf(const char* fmt, ...) __attribute__((format(printf,2,3)));
	void putc(const char c);
	void puts(const char* s);
	int getc(void);
	int getch(void);
	enum{
		TXBUF_SIZE = 300,
		TX_DMA_SIZE = 100,
		RXBUF_SIZE = 200,
		RX_DMA_SIZE = 20
	};
	void postErr();
	uint16_t get_emptyBytesInTxQueue();
	bool isTransmitterIdel();
	void transmitterRun();
	void receiverRun();
	void run();

#if CONSOLE_RX_USE_DMA
	void terminalRun();
#endif
	void baseRun();
	
	//enhanced char ring queue
	class CRingQueue
	{
		public:
			CRingQueue(char*, uint16_t);
		
			uint16_t size() {return SIZE_;}
			char front() {return *front_ptr_;}
			char back() {return *back_ptr_;}
			bool push(char);
			bool pop();
			
			uint16_t bytesInQue();
			uint16_t emptyBytesInQue();
			uint16_t push_array(char* ,uint16_t);
			uint16_t pop_array(char* ,uint16_t);
		
		private:
			const uint16_t SIZE_;
		
			char* const static_array_;
			char* back_ptr_; //[front, back)
			char* front_ptr_;
	};
	static char vsnprintfBuf_[TXBUF_SIZE]; //for sprintf

private:
	void InitSciGpio();
	void InitSci();

	static char TxBuf_[TXBUF_SIZE]; //for txQueue_
	CRingQueue txQueue_;
#if CONSOLE_TX_USE_DMA
	static char TxDmaBuf_[TX_DMA_SIZE]; //for txDma
#endif

#if CONSOLE_RX_USE_DMA
	static char RxBuf_[RXBUF_SIZE];	//for rxQueue_.
	CRingQueue rxQueue_;
	static char RxDmaBuf_[RX_DMA_SIZE]; //for rxDma
#endif
	
	/* interrupt flag clear register */
#ifdef CONSOLE_TX_DMACH
#endif

#ifdef CONSOLE_RX_DMACH
	void rxDMA_to_rxQueue();
#endif

	uint16_t overflowCounter_;
};
typedef NormalSingleton<CUartConsole> Console;
#define postErr(msg) printf("Error: %s(%d)-%s(): %s\r\n", __FILE__, __LINE__, __FUNCTION__, msg)
#endif
//end of file
