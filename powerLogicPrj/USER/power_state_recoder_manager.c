#include "power_state_recoder_manager.h"
#include "spi.h"
#include "string.h"
#include "SEGGER_RTT.h"

void traverseLastPostionAndStateInFlash(const uint32_t block_first_address, const uint32_t block_end_address, uint32_t & address_now)
{
    uint32_t address_temp = block_first_address;
    uint8_t state_temp[2];
	address_now = address_temp;
    while(address_temp <= (block_end_address - 1))                         //end the cycle while at the end of block 
    {
        flasher::Instance()->Read(address_temp, (u16*)state_temp, 1);
        if ((state_temp[1] != 0xFF)) 				//不能判断自增位，如开关机次数，会增加到0xFF
		{	
			if(address_temp == (block_end_address - 1))	address_now = address_temp;
			address_temp += 2;      //go on traverse	
        }
		else
        {
			if(address_temp == block_first_address)	address_now = block_first_address;
			else address_now = address_temp - 2;
            break;
        } 
    }
}

void traverseLastPowerArrary(const uint32_t block_first_address, const uint32_t address_now, uint32_t & array_head_address)
{
	uint32_t address_arrary_temp = address_now;
    uint8_t state_temp[2];
	array_head_address = address_arrary_temp;
	while(address_arrary_temp >= block_first_address)
	{
		flasher::Instance()->Read(address_arrary_temp, (u16*)state_temp, 1);
		
		if((state_temp[0] == 0x01) && (state_temp[1] == 0xAA))
		{
			array_head_address = address_arrary_temp;
			break;
		}
		else
			address_arrary_temp -= 2;
	}
}

void readLastPowerArrary(uint32_t arrary_head, uint32_t arrary_tail, uint8_t* arrary)
{
	uint8_t state_temp[2];
	uint8_t i = 1;
	for(uint32_t address_temp = arrary_head;address_temp <= arrary_tail;address_temp+=2)
	{
		if(arrary_head != arrary_tail )
		{
			flasher::Instance()->Read(address_temp, (u16*)state_temp, 1);
			arrary[i] = state_temp[0];
			i++;
		}
	}
}

void writeStateInPositionInFlash(uint32_t & address_now, uint8_t state)
{
	__set_PRIMASK(1); 	//关中断
	uint8_t check_first_temp[2];
	u8 write_state[]={state,0xAA};
	if((0x01 == state) && ((BOARD_POWER_STATE_END_ADDRESS-1-address_now) < 10) && (address_now != BOARD_POWER_STATE_FIRST_ADDRESS))
	{
		FLASH_Unlock();	
		FLASH_ErasePage(BOARD_POWER_STATE_FIRST_ADDRESS);
		FLASH_ErasePage(BOARD_POWER_STATE_END_ADDRESS);
		FLASH_Lock();
		address_now = BOARD_POWER_STATE_FIRST_ADDRESS;
	}
	if(((SWITCH_TIMES_END_ADDRESS-1-address_now) <= 0) && (address_now != SWITCH_TIMES_FIRST_ADDRESS))
	{
		FLASH_Unlock();
		FLASH_ErasePage(SWITCH_TIMES_FIRST_ADDRESS);		
		FLASH_Lock();
		address_now = SWITCH_TIMES_FIRST_ADDRESS;
	}
	if((address_now == BOARD_POWER_STATE_FIRST_ADDRESS) || (address_now == SWITCH_TIMES_FIRST_ADDRESS) || (address_now == POWERGOOD_STATE_FIRST_ADDRESS))
	{
		flasher::Instance()->Read(address_now, (u16*)check_first_temp, 1);
		if((check_first_temp[0] != 0xFF) && (check_first_temp[1] != 0xFF))
		{
			address_now += 2;
		}
		flasher::Instance()->Write(address_now, (u16*)write_state, 1);
	}
	else
	{
		address_now += 2;
		flasher::Instance()->Write(address_now, (u16*)write_state, 1);
	}
	__set_PRIMASK(0); 	//开中断	
}

void readLastSwitchTimes(uint32_t & address_now, uint8_t & switch_times_h, uint8_t & switch_times_l)
{
	uint8_t state_temp[2];
	flasher::Instance()->Read(address_now, (u16*)state_temp, 1);
	switch_times_l = state_temp[0];
	flasher::Instance()->Read(address_now - 2, (u16*)state_temp, 1);
	switch_times_h = state_temp[0];
}

void readLastPowerGoodState(uint32_t & address_now, uint8_t & last_power_good_state)
{
	uint8_t state_temp[2];
	flasher::Instance()->Read(address_now, (u16*)state_temp, 1);
	if(address_now == POWERGOOD_STATE_FIRST_ADDRESS)
	{
		if((state_temp[0] != 0xFF) && (state_temp[1] != 0xFF))	
		{
			last_power_good_state = state_temp[0];
		}
		else last_power_good_state = 0x00;
	}
	else	last_power_good_state = state_temp[0];
}