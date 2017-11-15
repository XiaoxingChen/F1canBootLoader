#ifndef UART5_IAP_DEVICE_H
#define UART5_IAP_DEVICE_H
#include "chardev.h"
#include "ringque.h"

class CUart5IapDevice
	:public CCharDev
{
	public:
		CUart5IapDevice();	
		virtual int open();
		virtual int close();
	
		virtual int write(const uint8_t*, uint32_t);
		virtual int read(uint8_t*, uint32_t);
	
		virtual uint32_t data_in_write_buf();
		virtual uint32_t freesize_in_write_buf();
		virtual uint32_t data_in_read_buf();
		virtual void clear_read_buf();
		virtual bool isTransmitterIdle();
	
		virtual void runTransmitter();
		virtual void runReceiver();
	
		void doIrqRun();
			
		enum
		{
			RX_QUE_LEN = 300,
			TX_QUE_LEN = 150
		};
			
	private:
		ringque<uint8_t, RX_QUE_LEN> rxBufQue_;
		ringque<uint8_t, TX_QUE_LEN> txBufQue_;
};

extern CUart5IapDevice iapDevice;

#endif
//end of file
