#include "uartIapDevice.h"
#include "Timer.h"

namespace
{
	const uint16_t UART_RX_DMA_BUF_LEN = 30;
	const uint16_t UART_TX_DMA_BUF_LEN = 150;
	uint8_t uartRxDmaBuff[UART_RX_DMA_BUF_LEN];
	uint8_t uartTxDmaBuff[UART_TX_DMA_BUF_LEN];
	CUsart iapUsart(USART2, uartRxDmaBuff, UART_RX_DMA_BUF_LEN);
};

CUartIapDevice iapDevice(iapUsart);
CCharDev& iap_device(iapDevice);

CUartIapDevice::CUartIapDevice(CUsart& refUsart)
	:CCharDev(300), refUsart_(refUsart)
{
	
}

int CUartIapDevice::open()
{
	refUsart_.InitSciGpio();
	refUsart_.InitSci();
	return 0;
}

int CUartIapDevice::close()
{
	return 0;
}

int CUartIapDevice::write(const uint8_t* buf, uint32_t len)
{
	return txBufQue_.push_array((uint8_t*)buf, len);
}

int CUartIapDevice::read(uint8_t* buf, uint32_t len)
{
	return rxBufQue_.pop_array(buf, len);
}
uint32_t gTemp = 0;
void CUartIapDevice::runReceiver()
{
	static Timer recvTimer(1, 1);
	
	if(refUsart_.get_BytesInRxFifo() == 0)
		return;
	
	update_data_break_flag();
	if(recvTimer.isAbsoluteTimeUp() || refUsart_.get_BytesInRxFifo() > UART_RX_DMA_BUF_LEN/2)
	{
		gTemp = refUsart_.read_RxFifo(rxBufQue_);
	}
}

void CUartIapDevice::runTransmitter()
{
	if(refUsart_.get_BytesInTxFifo() > 0)
		return;
	
	//refUsart_.get_BytesInTxFifo() == 0
	if(txBufQue_.elemsInQue() > 0)
	{
		uint16_t len = txBufQue_.pop_array(uartTxDmaBuff, UART_TX_DMA_BUF_LEN);
		refUsart_.send_Array(uartTxDmaBuff, len);
	}
}

uint32_t CUartIapDevice::data_in_read_buf()
{
	return rxBufQue_.elemsInQue();
}

void CUartIapDevice::clear_read_buf()
{
	refUsart_.clear_rxFifo();
	rxBufQue_.clear();
}

uint32_t CUartIapDevice::data_in_write_buf()
{
	return txBufQue_.elemsInQue();
}

uint32_t CUartIapDevice::freesize_in_write_buf()
{
	return txBufQue_.emptyElemsInQue();
}

//end of file
