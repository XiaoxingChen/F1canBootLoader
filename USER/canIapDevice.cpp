#include "canIapDevice.h"
#include "CUartConsole.h"

const uint8_t RX_MAILBOX_QUE_SIZE  = 5;
const uint16_t RX_BUFFER_QUE_SIZE = 300;
CanRxMsg rxMailboxBuf[RX_MAILBOX_QUE_SIZE];
uint8_t rxQueueBuffer[RX_BUFFER_QUE_SIZE];
const uint32_t IAP_REMOTE_ID = 0x19205;
const uint32_t IAP_NATIVE_ID = 0x19205;

/**
  * @brief  Constructor
	* @param  None
  * @retval bytes it reads
  */
CCanIapDevice::CCanIapDevice(CCanRouter& canBaseRouter,
														uint32_t txNodeId, 
														uint8_t txNodeIde, 
														uint32_t rxNodeId, 
														uint8_t rxNodeIde)
	:canBaseRouter_(canBaseRouter),
	rxBufQue_(rxQueueBuffer, RX_BUFFER_QUE_SIZE),
	rxMailbox_(rxMailboxBuf, RX_MAILBOX_QUE_SIZE),
	txNodeId_(txNodeId),txNodeIde_(txNodeIde)
{
	if(rxNodeIde == CAN_Id_Standard)
		rxMailbox_.setStdId(rxNodeId);
	else
		rxMailbox_.setExtId(rxNodeId);
}

/**
* @brief  open
* @param  None
* @retval state
*/
uint32_t CCanIapDevice::open()
{
	rxMailbox_.attachToRouter(canBaseRouter_);
	return 0;
	//TODO switch socket return type into CDevice return type
}

/**
* @brief  close
* @param  None
* @retval 
*/
uint32_t CCanIapDevice::close()
{
	return 0;
}

/**
  * @brief  write
	* @param  databuf: the data pointer wants to send
	* @param  len: data length
  * @retval actual send length
  */
uint32_t CCanIapDevice::write(const uint8_t* databuf, uint32_t len)
{
	CanTxMsg tempMsg;
	tempMsg.IDE = txNodeIde_;
	tempMsg.StdId = tempMsg.ExtId = txNodeId_;
	tempMsg.DLC = 8;
	
	uint8_t* endptr = (uint8_t*)databuf + len; //the byte at endptr should not be used
	
	while(databuf + 8 < endptr)
	{
		memcpy(tempMsg.Data, databuf, 8);
		databuf += 8;
		canBaseRouter_.putMsg(tempMsg);
	}
	tempMsg.DLC = endptr - databuf;
	memcpy(tempMsg.Data, databuf, tempMsg.DLC);
	canBaseRouter_.putMsg(tempMsg);
	Console::Instance()->printf("can send: ");
	for(int i = 0; i < tempMsg.DLC; i++)
	{
		Console::Instance()->printf(" 0x%X", tempMsg.Data[i]);
	}
	Console::Instance()->printf("\r\n");
	return len;
}

/**
  * @brief  read
	* @param  databuf: the data pointer wants to read
	* @param  len: data length
  * @retval actual read length
  */
uint32_t CCanIapDevice::read(uint8_t* databuf, uint32_t len)
{
	uint8_t bytesInBuf = rxBufQue_.elemsInQue();
	uint32_t ret = (len < bytesInBuf ? len : bytesInBuf);
	
	rxBufQue_.pop_array(databuf, ret);
	return ret;
}

/**
  * @brief  data_in_read_buf
	* @param  None
  * @retval bytes
  */
uint32_t CCanIapDevice::data_in_read_buf()
{
	return rxBufQue_.elemsInQue();
}

/**
  * @brief  data in write buffer
	* @param  None
  * @retval number of bytes
  */
uint32_t CCanIapDevice::data_in_write_buf()
{
	return 0;
}

/**
  * @brief  data in write buffer
	* @param  None
  * @retval number of bytes
  */
uint32_t CCanIapDevice::freesize_in_write_buf()
{
	return canBaseRouter_.freeSizeInTxQue();
}

/**
  * @brief  runReceiver
	* @param  None
  * @retval None
  */
void CCanIapDevice::runReceiver()
{
	CanRxMsg tempMsg;
	while(rxMailbox_.msgsInQue() > 0)
	{
		rxMailbox_.getMsg(&tempMsg);
		rxBufQue_.push_array(tempMsg.Data, tempMsg.DLC);
	}
}

#define IAP_NATIVE_ID	0x19204
#define IAP_REMOTE_ID	0x19205
CCanIapDevice iapDevice(CanRouter250k, IAP_REMOTE_ID, CAN_Id_Extended, IAP_NATIVE_ID, CAN_Id_Extended);
//end of file
