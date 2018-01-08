//release information:
//V1.1: open interrupt at CommonConfig(), extended TX buffer size for iapDevice.
//			bootloader before version V1.1 do not support code read back
#include "stm32f10x.h"
#include "Console.h"
#include "CommonConfig.h"
#include "CCan.h"
#include "uart5IapDevice.h"
#include "Timer.h"
#include "stmflash.h"
#include "powerupOption.h"
#include "HeartLed.h"
#include "logic_out.h"
#include "key_monitor.h"
#include "power_manager.h"

extern const uint8_t FIRMWARE_VERSION = 0x10;
uint8_t test_buf[10] = {0,0,0,0,0,0,0,0,0,0};

#define FLASH_APP1_ADDR		0x08000000

int main()
{
	CommonConfig();
	BaseTimer::Instance()->initialize();

	key_init();
	logic_pin_config();
	Console::Instance()->printf("PowerLogic virsion: V%d.%d\r\n", FIRMWARE_VERSION>>4, FIRMWARE_VERSION&0xF);
	confirm_wake_type();
	Initial_HeartLED();
	enable_battery();

//	while(1);
//	InitWatchDog(5000);
	while(1)
	{
		key_monitor_run();
		power_manager_run();
		
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
