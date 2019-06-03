#ifndef _POWER_STATE_RECODER_MANAGER_H
#define _POWER_STATE_RECODER_MANAGER_H

#include "stmflashV2.h"
#include "stm32f10x.h"

void traverseLastPostionAndStateInFlash(const uint32_t block_first_address, const uint32_t block_end_address, uint32_t & address_now);
void transferStateIfLastError(bool is_error);
void writeStateInPositionInFlash(uint32_t & address_now, uint8_t state);
void traverseLastPowerArrary(const uint32_t block_first_address, const uint32_t address_now, uint32_t & array_head_address);
void readLastPowerArrary(uint32_t arrary_head, uint32_t arrary_tail, uint8_t* arrary);
void readLastSwitchTimes(uint32_t & address_now, uint8_t & switch_times_h, uint8_t & switch_times_l);
void readLastPowerGoodState(uint32_t & address_now, uint8_t & last_power_good_state);

#endif
