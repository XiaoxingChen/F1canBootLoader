#include "stm32f10x.h"
#include "CUartConsole.h"
#include "CommonConfig.h"
#include "CCan.h"
#include "iap.h"
#include "canIapDevice.h"
#include "Timer.h"
CanTxMsg tempMsg;
int main()
{
	CommonConfig();
	BaseTimer::Instance()->initialize();
	BaseTimer::Instance()->delay_ms(500);
	
	CanRouter250k.InitCan();
	CanRouter250k.InitCanGpio(CCanRouter::GROUP_B8);
	
	iapDevice.open();
	
	tempMsg.StdId = 0x01;
	Console::Instance()->printf("\r\nStart..\r\n");
	
	while(1)
	{
		CanRouter250k.runTransmitter();
		CanRouter250k.runReceiver();
		Console::Instance()->run();
		iapDevice.runReceiver();
		iap_run();
	}
	//return 0;
}
