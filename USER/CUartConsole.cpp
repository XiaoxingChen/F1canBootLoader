/********************************************************************************
* @file    CUartConsole.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-04-21
* @brief   this file defines the Console function that can printf with uart.
*	   This is for STM32F4.
********************************************************************************/
#include "CUartConsole.h"
#include <stdarg.h>
//#include <stdio.h>
#include <string.h>


#if CONSOLE_TX_USE_DMA
char CUartConsole::TxDmaBuf_[TX_DMA_SIZE];	//for txDma
#endif //CONSOLE_TX_DMAST
char CUartConsole::TxBuf_[TXBUF_SIZE];		//buffer for None DMA Mode txQueue_
char CUartConsole::vsnprintfBuf_[TXBUF_SIZE];	//for sprintf

#if CONSOLE_RX_USE_DMA
char CUartConsole::RxBuf_[RXBUF_SIZE];			//for rxQueue_
char CUartConsole::RxDmaBuf_[RX_DMA_SIZE];	//for rxDma
#endif //CONSOLE_RX_USE_DMA

/**
  * @brief  Constructor
	* @param  None
  * @retval None
  */
CUartConsole::CUartConsole()
	:txQueue_(TxBuf_, TXBUF_SIZE),
	#ifdef CONSOLE_RX_DMACH
	rxQueue_(RxBuf_, RXBUF_SIZE),
	#endif
	overflowCounter_(0)
{
#if CONSOLE_SILENT
	return ;
#endif
	InitSciGpio();
	InitSci();
}

/**
  * @brief  Initialize the UART GPIO
  * @param  None
  * @retval None
  */
void CUartConsole::InitSciGpio()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	uint32_t RCC_APB2Periph_GPIOx;
	uint8_t GPIO_PinSource_BASE;
	GPIO_TypeDef *GPIOx;
	
	#if CONSOLE_IOGROUP_A2
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
		GPIOx = GPIOA;
		GPIO_PinSource_BASE = GPIO_PinSource2;
	#elif CONSOLE_IOGROUP_A9
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
		GPIOx = GPIOA;
		GPIO_PinSource_BASE = GPIO_PinSource9;
	#elif CONSOLE_IOGROUP_B10
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
		GPIOx = GPIOB;
		GPIO_PinSource_BASE = GPIO_PinSource10;
	#else 
		#error
	#endif

	/* open clock of gpio */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
	
	/* Config Pin: RXD TXD */

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 << GPIO_PinSource_BASE; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOx, &GPIO_InitStructure);	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 << GPIO_PinSource_BASE; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	GPIO_Init(GPIOx, &GPIO_InitStructure);
	
}

/**
  * @brief  Initialize the UART
  * @param  None
  * @retval None
  */
void CUartConsole::InitSci()
{
	/* init clock of USART */
	#if  CONSOLE_USE_UART1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	#elif  CONSOLE_USE_UART2 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	#elif  CONSOLE_USE_UART3  
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	#else 
		#error
	#endif
	USART_DeInit(CONSOLE_UART);
	
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(CONSOLE_UART, &USART_InitStructure); 

	USART_Cmd(CONSOLE_UART, ENABLE); 
	
	DMA_InitTypeDef DMA_InitStructure;
#if (defined CONSOLE_TX_DMACH) || (defined CONSOLE_RX_DMACH)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#endif //

#ifdef CONSOLE_TX_DMACH
	
	DMA_DeInit(CONSOLE_TX_DMACH);
	/* DMA Clock Config */
	uint32_t txRCC_AHB1Periph;
	DMA_TypeDef * txDMAx = (DMA_TypeDef *)((uint32_t)CONSOLE_TX_DMACH&0xFFFFFC00);
	if(txDMAx == DMA1) txRCC_AHB1Periph = RCC_AHBPeriph_DMA1;
	else if(txDMAx == DMA2) txRCC_AHB1Periph = RCC_AHBPeriph_DMA2;
	else while(1); //error
	RCC_AHBPeriphClockCmd(txRCC_AHB1Periph, ENABLE);
	
	USART_DMACmd(CONSOLE_UART, USART_DMAReq_Tx, ENABLE);
	
	/* TX DMA Config */	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(CONSOLE_UART->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxDmaBuf_;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 0;
	
	DMA_Cmd(CONSOLE_TX_DMACH, DISABLE);
	DMA_Init(CONSOLE_TX_DMACH, &DMA_InitStructure);
	
#endif //CONSOLE_TX_DMACH

#ifdef CONSOLE_RX_DMACH
	
	DMA_DeInit(CONSOLE_RX_DMACH);
	/* DMA Clock Config */
	DMA_TypeDef * rxDMAx = (DMA_TypeDef *)((uint32_t)CONSOLE_RX_DMACH&0xFFFFFC00);
	uint32_t rxRCC_AHB1Periph;
	
	if(rxDMAx == DMA1) rxRCC_AHB1Periph = RCC_AHBPeriph_DMA1;
	else if(rxDMAx == DMA2) rxRCC_AHB1Periph = RCC_AHBPeriph_DMA2;
	else while(1); //error
	RCC_AHBPeriphClockCmd(rxRCC_AHB1Periph, ENABLE);
	
	USART_DMACmd(CONSOLE_UART, USART_DMAReq_Rx, ENABLE);
	
	/* RX DMA Config */	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(CONSOLE_UART->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxDmaBuf_;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = RX_DMA_SIZE;
	
	DMA_Cmd(CONSOLE_RX_DMACH, DISABLE);
	DMA_Init(CONSOLE_RX_DMACH, &DMA_InitStructure);
	DMA_Cmd(CONSOLE_RX_DMACH, ENABLE);
	
#endif //CONSOLE_RX_DMACH
}

/**
  * @brief  do nothing to avoid the warning of compiler
  * @param  None
  * @retval always be 0
  */
#if CONSOLE_SILENT
int CUartConsole::printf(const char* fmt, ...)
{
	return 0;
}

#else // !CONSOLE_SILENT


int32_t vsnprintf(char       *buf,
									size_t   size,
									const char *fmt,
									va_list     args);
/**
  * @brief  printf a string without DMA controller.
	*					User should call the CUartConsole::run()
  * @param  None
  * @retval number of bytes were sent
  */
int CUartConsole::printf(const char* fmt, ...)
{
	va_list args;
	int n;
	int ret;
	
	//TODO lock vsnprintf mutex
	va_start(args, fmt);
	n = vsnprintf(vsnprintfBuf_, TXBUF_SIZE, fmt, args);
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
void CUartConsole::putc(const char c)
{
	//TODO add mutex lock here
	txQueue_.push(c);
	//TODO release mutex lock here
	transmitterRun();
}

/**
  * @brief  for None Dma Mode
  * @param  string to send
  * @retval None
  */
void CUartConsole::puts(const char* s)
{
	//TODO add mutex lock here
	txQueue_.push_array((char*)s, strlen(s));
	//TODO release mutex lock here
	transmitterRun();
}

#if CONSOLE_RX_USE_DMA
/**
  * @brief  for None Dma Mode
  * @param  None
  * @retval char get
  */
int CUartConsole::getc(void)
{
	//TODO add mutex lock here
	int ret = (uint8_t)rxQueue_.front();
	
	/* check if ret is valid */
	if(rxQueue_.pop())
		return ret;
	/* empty queue, ret is invalid */		
	else 
		return -1;
	//TODO release mutex lock here
}
#else
int CUartConsole::getc(void)
{
	return -1;
}
#endif

/**
  * @brief  send array
  * @param  None
  * @retval char get
  */
uint16_t CUartConsole::send_array(char* buf, uint16_t len)
{
	uint16_t res;
	//TODO add mutex lock here
	res = txQueue_.push_array(buf, len);
	//TODO release mutex lock here
	transmitterRun();
	return res;
}
#endif	// CONSOLE_SILENT

/**
  * @brief  wait until a char was read from UART
  * @param  None
  * @retval the value read from UART
  */
int CUartConsole::getch()
{
	while(USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_RXNE) == RESET);
	return CONSOLE_UART->DR;
}

/**
  * @brief  postErr() to cheat the IDE to show the macro function
	* @param  None
  * @retval None
  */
#undef postErr
void CUartConsole::postErr()
{}
#define postErr(msg) printf("Error: %s(%d)-%s(): %s\r\n", __FILE__, __LINE__, __FUNCTION__, msg)
	
/**
* @brief  get empty bytes in txQueue_
* @param  None
* @retval Number of empty bytes
*/
uint16_t CUartConsole::get_emptyBytesInTxQueue()
{
	return txQueue_.emptyBytesInQue();
}

/**
* @brief  Is Transmitter idel or not
* @param  None
* @retval If is idel
*/
bool CUartConsole::isTransmitterIdel()
{
	transmitterRun();
	return (0 == txQueue_.bytesInQue() 
		&& 0 == DMA_GetCurrDataCounter(CONSOLE_TX_DMACH)
		&& 1 == USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TXE));
}

/**
  * @brief  run UART transmitter, in another word TXD
  * @param  None
  * @retval None
  */
void CUartConsole::transmitterRun()
{
#if CONSOLE_TX_USE_DMA
	uint16_t bytesToSend;
	if((txQueue_.bytesInQue() != 0) && 0==DMA_GetCurrDataCounter(CONSOLE_TX_DMACH))
	{
		DMA_Cmd(CONSOLE_TX_DMACH, DISABLE);
		bytesToSend = (txQueue_.bytesInQue() < TX_DMA_SIZE) ? txQueue_.bytesInQue(): TX_DMA_SIZE;
		txQueue_.pop_array(TxDmaBuf_, bytesToSend);
		CONSOLE_TX_DMACH->CNDTR = bytesToSend;

		DMA_Cmd(CONSOLE_TX_DMACH, ENABLE);
	}
	
#else // !CONSOLE_TX_USE_DMA
	if((txQueue_.bytesInQue() != 0) && USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TXE))
	{
		CONSOLE_UART->DR = txQueue_.front();
		txQueue_.pop();
	}
#endif
}

/**
  * @brief  run UART receiver, in another word RXD
  * @param  None
  * @retval None
  */
void CUartConsole::receiverRun()
{
#if CONSOLE_RX_USE_DMA
	rxDMA_to_rxQueue();
#else // !CONSOLE_RX_USE_DMA
	
#endif
}

/**
  * @brief  run. Valid only in NoneDMA mode
  * @param  None
  * @retval None
  */
void CUartConsole::baseRun()
{
	transmitterRun();
	receiverRun();
}

/**
  * @brief  run. Valid only in NoneDMA mode
  * @param  None
  * @retval None
  */
#if CONSOLE_RX_USE_DMA
void CUartConsole::run()
{
	transmitterRun();
	receiverRun();
}
#endif
/**
  * @brief  move data from RX DMA buffer to rxQueue_
  * @param  None
  * @retval None
  */
#ifdef CONSOLE_RX_DMACH
void CUartConsole::rxDMA_to_rxQueue()
{
	uint16_t rxDataSize;	
	/* get rx data size without stop DMA */
	rxDataSize = RX_DMA_SIZE - CONSOLE_RX_DMACH->CNDTR;
	if(rxDataSize == 0) return;
	
	/* stop DMA */
	DMA_Cmd(CONSOLE_RX_DMACH, DISABLE);
	/* get rx data size again */
	rxDataSize = RX_DMA_SIZE - CONSOLE_RX_DMACH->CNDTR;
	
		/* check for overflow */
	if(rxDataSize > 10) overflowCounter_++;
	
	/* push data into rxQueue */
	rxQueue_.push_array(RxDmaBuf_, rxDataSize);
	
	CONSOLE_RX_DMACH->CNDTR = RX_DMA_SIZE;
	DMA_Cmd(CONSOLE_RX_DMACH, ENABLE);
	
	/* enable stream until it was enabled */
	while((CONSOLE_RX_DMACH->CCR&0x01) != 1) 
	{
		DMA_Cmd(CONSOLE_RX_DMACH, ENABLE);
	}
}
#endif
/********************************************************************************
* @brief   this part is for CRingQueue.
********************************************************************************/
/**
* @brief  Constructor
* @param  size of queue
* @retval invalid
*/
CUartConsole::CRingQueue::CRingQueue(char* array, uint16_t size) :
SIZE_(size),
static_array_(array),
back_ptr_(static_array_),
front_ptr_(static_array_)
{
	if (array == NULL) while (1);
	memset(static_array_, 0, SIZE_);
}

/**
* @brief  push a char into queue
* @param  value
* @retval Success or Not
*/
bool CUartConsole::CRingQueue::push(char value)
{
	if (emptyBytesInQue() == 0) return 0;
	*back_ptr_ = value;
	if(++back_ptr_ >= static_array_ + SIZE_) back_ptr_-= SIZE_;
	return 1;
}

/**
* @brief  pop a char from queue
* @param  None
* @retval return 0 if queue is empty
*/
bool CUartConsole::CRingQueue::pop()
{
	if (bytesInQue() == 0) return 0;
	if (++front_ptr_ >= static_array_ + SIZE_) front_ptr_ -= SIZE_;
	return 1;
}

/**
* @brief  get bytes in queue
* @param  None
* @retval number of bytes
*/
uint16_t CUartConsole::CRingQueue::bytesInQue()
{
	int32_t bytesInQue = int32_t((uint32_t)back_ptr_ - (uint32_t)front_ptr_);
	if (bytesInQue < 0) bytesInQue += SIZE_;

	return (uint16_t)bytesInQue;
}

/**
* @brief  get empty bytes in queue
* @param  None
* @retval number of empty bytes
*/
uint16_t CUartConsole::CRingQueue::emptyBytesInQue()
{
	return (SIZE_ - 1 - bytesInQue());
}

/**
* @brief  push array
* @param  str: pointer of the string
* @param  len: length of the string
* @retval the length actually pushed
*/
uint16_t CUartConsole::CRingQueue::push_array(char* str, uint16_t len)
{
	uint16_t emptyBytes_inQue = emptyBytesInQue();

	/* check if queue is full before push back */
	if (emptyBytes_inQue == 0) return 0;

	/* check if queue overflow */
	if (len > emptyBytes_inQue) 
		len = emptyBytes_inQue;

	/* front pointer is before back pointer */
	if ((uint32_t)front_ptr_ <= (uint32_t)back_ptr_)
	{
		uint16_t bytes_arrayEnd_To_queueBack = SIZE_ - ((uint32_t)back_ptr_ - (uint32_t)static_array_);//[1, TXBUF_SIZE]
		if (len >= bytes_arrayEnd_To_queueBack)
		{
			memcpy(back_ptr_, str, bytes_arrayEnd_To_queueBack);
			memcpy(static_array_, str + bytes_arrayEnd_To_queueBack, len - bytes_arrayEnd_To_queueBack);
			back_ptr_ = static_array_ + len - bytes_arrayEnd_To_queueBack;
		}
		else
		{
			memcpy(back_ptr_, str, len);
			back_ptr_ += len;
		}
	}
	/* back pointer is before front pointer */
	else
	{
		memcpy(back_ptr_, str, len);
		back_ptr_ += len;
	}
	return len;
}

/**
* @brief  pop array to queue
* @param  buf: pointer of target buffer
* @param  len: length of target length
* @retval the length actually popped
*/
uint16_t CUartConsole::CRingQueue::pop_array(char* buf, uint16_t len)
{
	uint16_t bytes_inQue = bytesInQue();

	/* check if queue is empty before pop front */
	if (bytes_inQue == 0) return 0;

	/* check if queue under flow */
	if (len > bytes_inQue) len = bytes_inQue;

	/* front pointer is before back pointer */
	if ((uint32_t)front_ptr_ <= (uint32_t)back_ptr_)
	{
		memcpy(buf, front_ptr_, len);
		front_ptr_ += len;
	}
	/* back pointer is before front pointer */
	else 
	{
		uint16_t bytes_arrayEnd_to_queueFront = SIZE_ - ((uint32_t)front_ptr_ - (uint32_t)static_array_);//[1, TXBUF_SIZE]
		/* front pointer needs to jump back */
		if (len > bytes_arrayEnd_to_queueFront)
		{
			memcpy(buf, front_ptr_, bytes_arrayEnd_to_queueFront);
			memcpy(buf + bytes_arrayEnd_to_queueFront, static_array_, len - bytes_arrayEnd_to_queueFront);
			front_ptr_ = static_array_ + len - bytes_arrayEnd_to_queueFront;
		}
		else /* do not need to jump back */
		{
			memcpy(buf, front_ptr_, len);
			front_ptr_ += len;
		}
	}
	return len;
}

/* private function */
#define isdigit(c)  ((unsigned)((c) - '0') < 10)


static inline int32_t divide(int32_t *n, int32_t base)
{
    int32_t res;

    /* optimized for processor which does not support divide instructions. */
    if (base == 10)
    {
        res = ((uint32_t)*n) % 10U;
        *n = ((uint32_t)*n) / 10U;
    }
    else
    {
        res = ((uint32_t)*n) % 16U;
        *n = ((uint32_t)*n) / 16U;
    }

    return res;
}

static inline int skip_atoi(const char **s)
{
    register int i=0;
    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';

    return i;
}

#define ZEROPAD     (1 << 0)    /* pad with zero */
#define SIGN        (1 << 1)    /* unsigned/signed long */
#define PLUS        (1 << 2)    /* show plus */
#define SPACE       (1 << 3)    /* space if plus */
#define LEFT        (1 << 4)    /* left justified */
#define SPECIAL     (1 << 5)    /* 0x */
#define LARGE       (1 << 6)    /* use 'ABCDEF' instead of 'abcdef' */

#ifdef RT_PRINTF_PRECISION
static char *print_number(char *buf,
                          char *end,
                          long  num,
                          int   base,
                          int   s,
                          int   precision,
                          int   type)
#else
static char *print_number(char *buf,
                          char *end,
                          long  num,
                          int   base,
                          int   s,
                          int   type)
#endif
{
    char c, sign;
#ifdef RT_PRINTF_LONGLONG
    char tmp[32];
#else
    char tmp[16];
#endif
    const char *digits;
    static const char small_digits[] = "0123456789abcdef";
    static const char large_digits[] = "0123456789ABCDEF";
    register int i;
    register int size;

    size = s;

    digits = (type & LARGE) ? large_digits : small_digits;
    if (type & LEFT)
        type &= ~ZEROPAD;

    c = (type & ZEROPAD) ? '0' : ' ';

    /* get sign */
    sign = 0;
    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
        }
        else if (type & PLUS)
            sign = '+';
        else if (type & SPACE)
            sign = ' ';
    }

#ifdef RT_PRINTF_SPECIAL
    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
#endif

    i = 0;
    if (num == 0)
        tmp[i++]='0';
    else
    {
        while (num != 0)
            tmp[i++] = digits[divide((int32_t*)&num, base)];
    }

#ifdef RT_PRINTF_PRECISION
    if (i > precision)
        precision = i;
    size -= precision;
#else
    size -= i;
#endif

    if (!(type&(ZEROPAD | LEFT)))
    {
        if ((sign)&&(size>0))
            size--;

        while (size-->0)
        {
            if (buf <= end)
                *buf = ' ';
            ++ buf;
        }
    }

    if (sign)
    {
        if (buf <= end)
        {
            *buf = sign;
            -- size;
        }
        ++ buf;
    }

#ifdef RT_PRINTF_SPECIAL
    if (type & SPECIAL)
    {
        if (base==8)
        {
            if (buf <= end)
                *buf = '0';
            ++ buf;
        }
        else if (base == 16)
        {
            if (buf <= end)
                *buf = '0';
            ++ buf;
            if (buf <= end)
            {
                *buf = type & LARGE? 'X' : 'x';
            }
            ++ buf;
        }
    }
#endif

    /* no align to the left */
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            if (buf <= end)
                *buf = c;
            ++ buf;
        }
    }

#ifdef RT_PRINTF_PRECISION
    while (i < precision--)
    {
        if (buf <= end)
            *buf = '0';
        ++ buf;
    }
#endif

    /* put number in the temporary buffer */
    while (i-- > 0)
    {
        if (buf <= end)
            *buf = tmp[i];
        ++ buf;
    }

    while (size-- > 0)
    {
        if (buf <= end)
            *buf = ' ';
        ++ buf;
    }

    return buf;
}


int32_t vsnprintf(char       *buf,
									size_t   size,
									const char *fmt,
									va_list     args)
{
#ifdef RT_PRINTF_LONGLONG
    unsigned long long num;
#else
    uint32_t num;
#endif
    int i, len;
    char *str, *end, c;
    const char *s;

    uint8_t base;            /* the base of number */
    uint8_t flags;           /* flags to print number */
    uint8_t qualifier;       /* 'h', 'l', or 'L' for integer fields */
    int32_t field_width;     /* width of output field */

#ifdef RT_PRINTF_PRECISION
    int precision;      /* min. # of digits for integers and max for a string */
#endif

    str = buf;
    end = buf + size - 1;

    /* Make sure end is always >= buf */
    if (end < buf)
    {
        end  = ((char *)-1);
        size = end - buf;
    }

    for (; *fmt ; ++fmt)
    {
        if (*fmt != '%')
        {
            if (str <= end)
                *str = *fmt;
            ++ str;
            continue;
        }

        /* process flags */
        flags = 0;

        while (1)
        {
            /* skips the first '%' also */
            ++ fmt;
            if (*fmt == '-') flags |= LEFT;
            else if (*fmt == '+') flags |= PLUS;
            else if (*fmt == ' ') flags |= SPACE;
            else if (*fmt == '#') flags |= SPECIAL;
            else if (*fmt == '0') flags |= ZEROPAD;
            else break;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt)) field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++ fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

#ifdef RT_PRINTF_PRECISION
        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++ fmt;
            if (isdigit(*fmt)) precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++ fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0) precision = 0;
        }
#endif
        /* get the conversion qualifier */
        qualifier = 0;
#ifdef RT_PRINTF_LONGLONG
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
#else
        if (*fmt == 'h' || *fmt == 'l')
#endif
        {
            qualifier = *fmt;
            ++ fmt;
#ifdef RT_PRINTF_LONGLONG
            if (qualifier == 'l' && *fmt == 'l')
            {
                qualifier = 'L';
                ++ fmt;
            }
#endif
        }

        /* the default base */
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    if (str <= end) *str = ' ';
                    ++ str;
                }
            }

            /* get character */
            c = (uint8_t)va_arg(args, int);
            if (str <= end) *str = c;
            ++ str;

            /* put width */
            while (--field_width > 0)
            {
                if (str <= end) *str = ' ';
                ++ str;
            }
            continue;

        case 's':
            s = va_arg(args, char *);
            if (!s) s = "(NULL)";

            len = strlen(s);
#ifdef RT_PRINTF_PRECISION
            if (precision > 0 && len > precision) len = precision;
#endif

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    if (str <= end) *str = ' ';
                    ++ str;
                }
            }

            for (i = 0; i < len; ++i)
            {
                if (str <= end) *str = *s;
                ++ str;
                ++ s;
            }

            while (len < field_width--)
            {
                if (str <= end) *str = ' ';
                ++ str;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = sizeof(void *) << 1;
                flags |= ZEROPAD;
            }
#ifdef RT_PRINTF_PRECISION
            str = print_number(str, end,
                               (long)va_arg(args, void *),
                               16, field_width, precision, flags);
#else
            str = print_number(str, end,
                               (long)va_arg(args, void *),
                               16, field_width, flags);
#endif
            continue;

        case '%':
            if (str <= end) *str = '%';
            ++ str;
            continue;

            /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            if (str <= end) *str = '%';
            ++ str;

            if (*fmt)
            {
                if (str <= end) *str = *fmt;
                ++ str;
            }
            else
            {
                -- fmt;
            }
            continue;
        }

#ifdef RT_PRINTF_LONGLONG
        if (qualifier == 'L') num = va_arg(args, long long);
        else if (qualifier == 'l')
#else
        if (qualifier == 'l')
#endif
        {
            num = va_arg(args, uint32_t);
            if (flags & SIGN) num = (int32_t)num;
        }
        else if (qualifier == 'h')
        {
            num = (uint16_t)va_arg(args, int32_t);
            if (flags & SIGN) num = (int16_t)num;
        }
        else
        {
            num = va_arg(args, uint32_t);
            if (flags & SIGN) num = (int32_t)num;
        }
#ifdef RT_PRINTF_PRECISION
        str = print_number(str, end, num, base, field_width, precision, flags);
#else
        str = print_number(str, end, num, base, field_width, flags);
#endif
    }

    if (str <= end) *str = '\0';
    else *end = '\0';

    /* the trailing null byte doesn't count towards the total
    * ++str;
    */
    return str - buf;
}
//end of file
