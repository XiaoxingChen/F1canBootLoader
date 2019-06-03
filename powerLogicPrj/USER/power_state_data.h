#ifndef _POWER_STATE_DATA_H
#define _POWER_STATE_DATA_H

#include "stm32f10x.h"
#include "Singleton.h"

namespace
{
	const uint8_t TX_BUFFER_SIZE = 10;
}

class PowerStateGlobalData
{
private:
    /* data */
public:
    PowerStateGlobalData(/* args */){};
    ~PowerStateGlobalData(){};

    uint32_t board_power_state_address_now;
	uint32_t switch_times_address_now;
	uint32_t arrary_head_address_now;
	uint32_t power_good_state_address_now;

	uint8_t switch_times_h;
	uint8_t switch_times_l;
	uint16_t switch_times;
		
	uint8_t last_power_good_state;
		
	uint8_t arrary[TX_BUFFER_SIZE];
	/************************************************
	1.[0]为帧头
	2.[1:5]为开关机过程标记码
	3.[6]为power_good状态位
	4.[7:8]为开关机次数，最大65536
	5.[9]为帧尾
	***********************************************/
	bool is_close_exti_interrupt;
};


typedef NormalSingleton<PowerStateGlobalData> PSGlobalData;

#endif