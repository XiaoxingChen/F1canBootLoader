#include "stm32f10x.h"
#include "Console.h"
#include "CommonConfig.h"
#include "iap.h"
#include "uartIapDevice.h"
#include "Timer.h"
#include "stmflash.h"
#include "powerupOption.h"
#include "HeartLed.h"

extern const uint8_t FIRMWARE_VERSION = 0x11;
uint8_t test_buf[10] = {0,0,0,0,0,0,0,0,0,0};
// <<< Use Configuration Wizard in Context Menu >>>
// <e> RUN_IN_APP
// 	<i>Default: 0
#define RUN_IN_APP 0
// <<< end of configuration section >>>

//#define FLASH_APP1_ADDR		0x08005800 
#define FLASH_APP1_ADDR		0x08000000 

int main()
{
	SCB->VTOR = FLASH_APP1_ADDR;
	CommonConfig();
	BaseTimer::Instance()->initialize();
	Console::Instance()->printf("program start\r\n");	
	Initial_HeartLed();
	//InitWatchDog(500);
	while(1)
	{
		Console::Instance()->runTransmitter();
		HeartLed_Run();
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
//end of file

