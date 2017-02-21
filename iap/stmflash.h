#ifndef __STMFLASH_H__
#define __STMFLASH_H__ 

#include <stdint.h>

//FLASH起始地址
#define STM32_FLASH_BASE 	0x08000000 	//STM32 FLASH的起始地址
#define BOOT_PARAM_NORM 	0x5555AAAA
#define BOOT_PARAM_IAP		0x2b2b6666
#define FLASH_APP1_ADDR		0x08008000 
 

int STMFLASH_write_bytes(uint32_t appxaddr,uint8_t *buf,uint16_t len);
uint32_t read_boot_parameter();
int write_boot_parameter(uint32_t);
						   
#endif

















