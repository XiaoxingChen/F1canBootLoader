#include "logic_out.h"
#include "string.h"
#include "power_process.h"
#include "power_state_data.h"
#include "power_state_recoder_manager.h"
#include "SEGGER_RTT.h"

Timer openpc(500,500);
Timer close_pc_key_timer(2000,2000);
Timer closepc(500,500);
Timer force_close_pc_timer(40000,40000);
Timer wait_pc_power_timer(200, 200);

bool is_openpc_ready = true;
bool is_closepc_ready = false;
bool is_open_process_timer_open = true;
bool is_close_key_timer_open = true;
bool is_close_process_timer_open = true;
bool is_close_key_time_enough = false;
bool is_can_close_pc_process = false;
bool is_can_open_pc_process = false;
bool is_force_close_pc = false;
bool is_over_time_monitor = false;
bool is_open_key_finished = false;
bool is_close_key_finished = false;
bool is_can_wait_pc_power = false;
bool is_wait_pc_power_timer_open = true;


void powerProcess()
{
	if (is_openpc_ready)              //按了下开机按键，开机开始按下后是高
	{
		PSGlobalData::Instance()->is_close_exti_interrupt = true;			//打开外部中断，检测power_good上升沿
		if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 1) && !is_open_key_finished)
		{
			pcEnableKeyConfig(2);			//开机键配置为浮空输入，防止上电冲击
			launch_board();
			enable_battery();
			is_can_wait_pc_power = true;
			/**********************************************开机读取上次状态***************************************************************************************************************************/
			traverseLastPowerArrary(BOARD_POWER_STATE_FIRST_ADDRESS, PSGlobalData::Instance()->board_power_state_address_now, PSGlobalData::Instance()->arrary_head_address_now);	//遍历最后一组标记码
			memset(PSGlobalData::Instance()->arrary, 0, TX_BUFFER_SIZE);		//清空标记码，重新发送	
			readLastPowerArrary(PSGlobalData::Instance()->arrary_head_address_now, PSGlobalData::Instance()->board_power_state_address_now, PSGlobalData::Instance()->arrary);		//读出最后一组标记码
			readLastPowerGoodState(PSGlobalData::Instance()->power_good_state_address_now, PSGlobalData::Instance()->last_power_good_state);										//读出上一次的power_good状态
			/********************************************************************************************************************************************************************************************/
			PSGlobalData::Instance()->arrary[6] = PSGlobalData::Instance()->last_power_good_state;
			is_open_key_finished = true;
		}
		
		if(is_can_wait_pc_power)			//等待PC上电后
		{
			if(is_wait_pc_power_timer_open)
			{
				writeStateInPositionInFlash(PSGlobalData::Instance()->board_power_state_address_now, 0x01);		//写标记码,写操作后，最新位置更新为当前标记码位置
				wait_pc_power_timer.reset();
				is_wait_pc_power_timer_open = false;
			}
			if(wait_pc_power_timer.isAbsoluteTimeUp())		
			{
				pcEnableKeyConfig(1);						//上电200ms后才开机键配置为开漏输出，并拉高开机键
				pc_en_line_high();
				is_can_wait_pc_power = false;
				is_can_open_pc_process = true;
			}
		}
		
		if(is_can_open_pc_process)
		{	
			/*****************************************open pc process************************************/
			pc_en_line_low();
			if (is_open_process_timer_open)             
			{
				openpc.reset();
				is_open_process_timer_open = false;
			}
			if (openpc.isAbsoluteTimeUp())
			{
				judgeAndErrasePowerGoodPartition();		//单独提出来做，当写到分区最后一字节时，不能放在写函数里面，Flash擦写一页20ms,断电9ms
				/*****************************************************************开机记录动作*******************************************************************************/
				PSGlobalData::Instance()->switch_times++;																					//每开机一次，自增一次
				PSGlobalData::Instance()->switch_times_h = (uint8_t)((PSGlobalData::Instance()->switch_times & 0xFF00) >> 8);
				PSGlobalData::Instance()->switch_times_l = (uint8_t)(PSGlobalData::Instance()->switch_times & 0x00FF);
				PSGlobalData::Instance()->arrary[7] = (uint8_t)PSGlobalData::Instance()->switch_times_h;									//填进发送序列中
				PSGlobalData::Instance()->arrary[8] = (uint8_t)PSGlobalData::Instance()->switch_times_l;
				writeStateInPositionInFlash(PSGlobalData::Instance()->power_good_state_address_now, 0x00);						//开机正常记一下，power_good正常写入0x00
				writeStateInPositionInFlash(PSGlobalData::Instance()->switch_times_address_now, PSGlobalData::Instance()->switch_times_h);			//把开机次数写进flash
				writeStateInPositionInFlash(PSGlobalData::Instance()->switch_times_address_now, PSGlobalData::Instance()->switch_times_l);
				/**************************************************************************************************************************************************************/
				pc_en_line_high();
				is_open_process_timer_open = true;
				is_can_open_pc_process = false;
				is_openpc_ready = false;
				is_closepc_ready = true;
				writeStateInPositionInFlash(PSGlobalData::Instance()->board_power_state_address_now, 0x02);
			}
			/*********************************************************************************************/
		}
	}

	if(is_closepc_ready)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)                   //PC已经关机了，所以直接断开电源GPIOA, GPIO_Pin_0		关机是高
		{
			PSGlobalData::Instance()->is_close_exti_interrupt = false;			//关闭外部中断，不检测power_good上升沿
			writeStateInPositionInFlash(PSGlobalData::Instance()->board_power_state_address_now, 0x05);
			BaseTimer::Instance()->delay_ms(1000);
			pcEnableKeyConfig(2);
			shutdown_board();
			disable_battery();
			BaseTimer::Instance()->delay_ms(500);					//防止开机键在24V光耦衰减时的高检测，误开机
			/********************************************/
			
			resetAllStatusBool();
		}
		
		if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 1) && !is_close_key_time_enough)
		{
			if (is_close_key_timer_open)
			{
				close_pc_key_timer.reset();
				is_close_key_timer_open = false;
			} 
			if(close_pc_key_timer.isAbsoluteTimeUp())
			{
				is_close_key_timer_open = true;
				is_over_time_monitor = true;
			}
		}
		else if ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0))				//如果按下就松掉，那么时钟也要重新开
		{
			is_close_key_timer_open = true;
			is_close_key_time_enough = false;
		}
		
		if(is_over_time_monitor)												//超过2s后松掉才执行后面的关机流程
		{
			if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) && !is_close_key_finished)
			{
				is_over_time_monitor = true;
				/****enter in normal close pc process********/
				is_close_key_time_enough = true;
				is_can_close_pc_process = true;
				/*******************************************/
				/****start force close pc process****/
				is_force_close_pc = true;
				force_close_pc_timer.reset();
				/***************************************/
				is_close_key_finished = true;
				PSGlobalData::Instance()->is_close_exti_interrupt = false;			//关闭外部中断，不检测power_good上升沿
				writeStateInPositionInFlash(PSGlobalData::Instance()->board_power_state_address_now, 0x03);
			}
		}

		if(is_can_close_pc_process)                     //正常关机流程，关机按住500ms
		{
			
			pc_en_line_low();
			if (is_close_process_timer_open)
			{
				closepc.reset();
				is_close_process_timer_open = false;
			}
			if (closepc.isAbsoluteTimeUp())
			{
				pc_en_line_high();
				is_close_process_timer_open = true;
				is_can_close_pc_process = false;
				writeStateInPositionInFlash(PSGlobalData::Instance()->board_power_state_address_now, 0x04);
			}
		}

		if(is_force_close_pc && force_close_pc_timer.isAbsoluteTimeUp())            //强制关机流程，40s后关机
		{
			writeStateInPositionInFlash(PSGlobalData::Instance()->board_power_state_address_now, 0x06);
			BaseTimer::Instance()->delay_ms(1000);
			pcEnableKeyConfig(2);
			shutdown_board();
			disable_battery();
			BaseTimer::Instance()->delay_ms(500);
			/********************************************/
			resetAllStatusBool();
		}
	}
}

void resetAllStatusBool()
{
	is_openpc_ready = true;
	is_closepc_ready = false;
	is_open_process_timer_open = true;
	is_close_key_timer_open = true;
	is_close_process_timer_open = true;
	is_close_key_time_enough = false;
	is_can_close_pc_process = false;
	is_can_open_pc_process = false;
	is_force_close_pc = false;
	is_over_time_monitor = false;
	is_open_key_finished = false;
	is_close_key_finished = false;
	is_can_wait_pc_power =false;
	is_wait_pc_power_timer_open = true;
}

void judgeAndErrasePowerGoodPartition()
{
	if(((POWERGOOD_STATE_END_ADDRESS-1-PSGlobalData::Instance()->power_good_state_address_now) <= 4) && (PSGlobalData::Instance()->power_good_state_address_now != POWERGOOD_STATE_FIRST_ADDRESS))
	{
		FLASH_Unlock();	
		FLASH_ErasePage(POWERGOOD_STATE_FIRST_ADDRESS);
		FLASH_Lock();
		PSGlobalData::Instance()->power_good_state_address_now = POWERGOOD_STATE_FIRST_ADDRESS;
	}	
}

