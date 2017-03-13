#include "stm32f10x.h"
#include "Console.h"
#include "CommonConfig.h"
#include "CCan.h"
#include "iap.h"
#include "canIapDevice.h"
#include "Timer.h"
#include "stmflash.h"
CanTxMsg tempMsg;
extern const uint8_t FIRMWARE_VERSION = 0x10;
uint8_t test_buf[10] = {0,0,0,0,0,0,0,0,0,0};
// <<< Use Configuration Wizard in Context Menu >>>
// <e> RUN_IN_APP
// 	<i>Default: 0
#define RUN_IN_APP 0
// <<< end of configuration section >>>
int main()
{
#if RUN_IN_APP
	SCB->VTOR = FLASH_APP1_ADDR;
#else
	SCB->VTOR = 0x08000000;
#endif
	CommonConfig();
	BaseTimer::Instance()->initialize();
	
	CanRouter1.InitCan();
	CanRouter1.InitCanGpio(CCanRouter::GROUP_A11);
	
	iapDevice.open();
	BaseTimer::Instance()->delay_ms(50);
	CanRouter1.runReceiver();
	iapDevice.runReceiver();
#if !RUN_IN_APP
	if(iapDevice.data_in_read_buf() > 1)
	{
		iapDevice.runReceiver();
		iapDevice.read(test_buf, 2);
		if(test_buf[0] == 0x7F && test_buf[1] == 0x7F)
		{
			Console::Instance()->printf("Get into bootloader by emergency mode.\r\n");
			write_boot_parameter(BOOT_PARAM_IAP);
		}else if(BOOT_PARAM_IAP != read_boot_parameter())
		{
			iap_load_app(FLASH_APP1_ADDR);
		}
	}else if(BOOT_PARAM_IAP != read_boot_parameter())
	{
		iap_load_app(FLASH_APP1_ADDR);
	}
	
#endif

#if RUN_IN_APP
	BaseTimer::Instance()->delay_ms(1000);
	Console::Instance()->printf("\r\n Bootloader flasher start!\r\n");
#else	
	if(BOOT_PARAM_IAP == read_boot_parameter())
	{
		Console::Instance()->printf("\r\n Bootloader start!\r\n");
	}else
	{
		Console::Instance()->printf("\r\n Boot paramter error: 0x%X, keep at bootloader\r\n",
			read_boot_parameter());
	}
#endif
	Console::Instance()->printf("Firmware virsion: V%d.%d\r\n", FIRMWARE_VERSION>>4, FIRMWARE_VERSION&0xF);
	//InitWatchDog(500);
	while(1)
	{
		CanRouter1.runTransmitter();
		CanRouter1.runReceiver();
		Console::Instance()->runTransmitter();
		iapDevice.run();
		iap_run();
	}
	//return 0;
}

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	Timer loopTimer(1000,1000);
	
  /* Infinite loop */
  while (1)
  {
		Console::Instance()->runTransmitter();
		if(loopTimer.isAbsoluteTimeUp())
		{
			Console::Instance()->printf("Wrong parameters value: file %s on line %d\r\n", file, line);
		}
  }
}
