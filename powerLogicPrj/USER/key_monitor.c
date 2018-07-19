#include "key_monitor.h"
#include "Timer.h"
#include "stm32f10x.h"
#include "Console.h"

struct key_struct
{
	GPIO_TypeDef * KEY_GPIO_X_;
	const uint16_t KEY_GPIO_PIN_;
	const uint8_t IS_PRESSED; 		//is 0 means pressed or 1 mean pressed. 
																//It depends on hardware connection
	key_state key_state_;
	const char* name_;
	Timer timer_;
	
};

key_type key_array[TOTAL_KEY_NUM] = 
{
	{GPIOA, GPIO_Pin_12, 1, RELEASED, "IS_PC_LAUNCH"}, //KEY_IS_PC_LAUNCH
	{GPIOA, GPIO_Pin_2, 1, RELEASED, "MODE1_LAUNCH"}, //KEY_LAUNCH
	{GPIOA, GPIO_Pin_0, 1, RELEASED, "BRAKE"}, //KEY_BRAKE
	{GPIOA, GPIO_Pin_11, 0, RELEASED, "DCIN"}, //KEY_DCIN
	{GPIOA, GPIO_Pin_10, 1, RELEASED, "MODE2_ON"}, //KEY_HY_ON
	{GPIOA, GPIO_Pin_9, 0, RELEASED, "MODE2_OFF"}, //KEY_HY_OFF
	{GPIOA, GPIO_Pin_3, 0, RELEASED, "MODE0_ON_BRD"}  //KEY_ON_BRD
}; 

void key_init()
{
	int i;
	GPIO_InitTypeDef	GPIO_InitStructure;		
	
	for(i = 0; i < TOTAL_KEY_NUM; i++)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA<< (((uint32_t)key_array[i].KEY_GPIO_X_-(uint32_t)GPIOA)/0x400), ENABLE);
		
		/* key bsp gpio init */
		GPIO_InitStructure.GPIO_Pin  = key_array[i].KEY_GPIO_PIN_;		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
		GPIO_Init(key_array[i].KEY_GPIO_X_, &GPIO_InitStructure);
	}	
}

/**
  * @brief  Get pin raw data without filter
  * @param  key_index 
  * @retval pressed or released
  */
key_state key_get_raw_data(int key_index)
{
	GPIO_TypeDef * key_gpio = key_array[key_index].KEY_GPIO_X_;
	uint16_t key_pin = key_array[key_index].KEY_GPIO_PIN_;

	if(key_index >= TOTAL_KEY_NUM)
		return NO_SUCH_KEY;

	if(GPIO_ReadInputDataBit(key_gpio, key_pin) == key_array[key_index].IS_PRESSED)
	{
		return PRESSED;
	}
	else
	{
		return RELEASED;
	}
}

/**
  * @brief  Get pin raw data after filterd
  * @param  key_index 
  * @retval 
  */
key_state key_get_data(int key_index)
{
	if(key_index >= TOTAL_KEY_NUM)
		return NO_SUCH_KEY;

	return key_array[key_index].key_state_;
}

/**
  * @brief  judge if key is pressed for a definited time
  * @param  key_index 
  * @retval 
  */
uint8_t key_is_long_pressed(int key_index)
{
//	uint8_t ret;

//	if(key_index >= TOTAL_KEY_NUM)
//		return 0;

//	ret = (key_array[key_index].key_state_ == PRESSED 
//			&& timer_get_time(&key_array[key_index].timer_) > 10000);
//	return (ret);
	return 0;
}

/**
  * @brief  run key monitor for one time, update all keys' state
  * @param  None
  * @retval None
  */
void key_monitor_run()
{
	int i;
	uint8_t temp_state = 0;
	for(i = 0; i < TOTAL_KEY_NUM; i++)
	{
		/* RELEASED ===> PRESSING */
		if(RELEASED == key_array[i].key_state_ 
			&& PRESSED == key_get_raw_data(i))
		{
//			timer_set_period(&key_array[i].timer_, 10);
//			timer_reset(&key_array[i].timer_);
			
			key_array[i].timer_ = Timer(10, 10);
			key_array[i].timer_.reset();
			
			key_array[i].key_state_ = PRESSING;
//			Console::Instance()->printf("key %d pressing\r\n", i);
		}
		
		/* RELEASED <== PRESSING ==> PRESSED */
		else if(PRESSING == key_array[i].key_state_
			&& key_array[i].timer_.isAbsoluteTimeUp())
		{
			if(PRESSED == key_get_raw_data(i))
			{
				key_array[i].key_state_ = PRESSED;
				
//				timer_set_period(&key_array[i].timer_, 60000);
//				timer_reset(&key_array[i].timer_);
				Console::Instance()->printf("[Key]: %s pressed\r\n", key_array[i].name_);
			}
			else 
			{
				key_array[i].key_state_ = RELEASED;
			}			
		}
		
		/* PRESSED ==> RELEASING */
		else if(PRESSED == key_array[i].key_state_
			&& RELEASED == key_get_raw_data(i))
		{
//			timer_set_period(&key_array[i].timer_, 10);
//			timer_reset(&key_array[i].timer_);
			
			key_array[i].timer_ = Timer(10, 10);
			key_array[i].timer_.reset();
			
			key_array[i].key_state_ = RELEASING;
//			Console::Instance()->printf("key %d releasing\r\n", i);
		}
		
		/* PRESSED <== RELEASING ==> RELEASED */
		else if(RELEASING == key_array[i].key_state_
			&& key_array[i].timer_.isAbsoluteTimeUp())
		{
			if(PRESSED == key_get_raw_data(i))
			{
				key_array[i].key_state_ = PRESSED;
			}
			else 
			{
				key_array[i].key_state_ = RELEASED;
				Console::Instance()->printf("[Key]: %s released\r\n", key_array[i].name_);
			}
		}
		
	}	
}

//end of file
