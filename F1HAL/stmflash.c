#include "stmflash.h"
#include "stm32f10x.h"
#include <string.h>
#include "printf.h"
 
#define STM32_FLASH_SIZE		((*(volatile uint16_t*)(0x1FFFF7E0))*1024)
#define STM_SECTOR_SIZE			(STM32_FLASH_SIZE < (256*1024) ? 1024ul : 2048ul)

/**
  * @brief  get the flash page head address according to an address
	* @param  uint32_t address
	* @retval uint32_t head address
  */
uint32_t STMFLASH_GetFlashSector(u32 addr)
{
	uint32_t head_addr = 0;
	uint8_t sect_idx = 0;
	if(addr - 0x08000000 + 1> STM32_FLASH_SIZE) return 0xFFFFFFFF;
	
	sect_idx = (addr - 0x08000000)/STM_SECTOR_SIZE;
	head_addr = 0x08000000 + sect_idx * STM_SECTOR_SIZE;
	
	return head_addr;
}


/**
  * @brief  get sector index from sector address
	* @param  uint32_t sector head address
	* @retval sector index
  */
uint8_t FLASH_Sector_to_index(uint32_t FLASH_Sector)
{
	return (FLASH_Sector - 0x08000000)/STM_SECTOR_SIZE;
}


static uint32_t flash_sector_H = 0x00000000;
static uint32_t flash_sector_L = 0x00000000;
/**
  * @brief  Check if sector is erased
	* @param  FLASH_Sector
	* @retval 0xFF: erased
	* @retval 0x00: not erased
  */
uint8_t is_sector_erased(uint8_t sector_idx)
{
	uint32_t ret;
	
	if(sector_idx > 63) return 0x00;
	else if(sector_idx > 31)
		ret = (flash_sector_H & (1ul << (sector_idx - 32)));
	else
		ret = (flash_sector_L & (1ul << sector_idx));
	
	if(ret > 0) return 0xFF;
	else return 0;
}

void mark_sector_erased(uint8_t sector_idx)
{
	if(sector_idx > 63) return;
	else if(sector_idx > 31)
		flash_sector_H |= (1ul << (sector_idx - 32));
	else
		flash_sector_L |= (1ul << sector_idx);
}

void rst_addr_erase_mark(uint32_t addr)
{
	uint8_t sector_idx = FLASH_Sector_to_index(STMFLASH_GetFlashSector(addr));
	if(sector_idx > 63) return;
	else if(sector_idx > 31)
		flash_sector_H &= ~(1ul << (sector_idx - 32));
	else
		flash_sector_L &= ~(1ul << sector_idx);
}

#define RESET_SECTOR_ERASE_MARK() \
	flash_sector_H = 0;flash_sector_L = 0

/**
  * @brief  Check before erasing. If erased, do nothing
	* @param  FLASH_Sector
	* @retval None
  */
FLASH_Status erase_sector_once(uint32_t FLASH_Sector)
{
	FLASH_Status ret = FLASH_COMPLETE;
	uint8_t idx = FLASH_Sector_to_index(FLASH_Sector);
	if(!is_sector_erased(idx))
	{
		printf("Erasing sector %d\r\n", FLASH_Sector_to_index(FLASH_Sector));
		ret = FLASH_ErasePage(FLASH_Sector);
		mark_sector_erased(idx);
	}
	return ret;
}

int STMFLASH_write_bytes_noerase(uint32_t appxaddr,uint8_t *buf,uint16_t len)
{
	uint8_t idx = FLASH_Sector_to_index(STMFLASH_GetFlashSector(appxaddr));
	mark_sector_erased(idx);
	return STMFLASH_write_bytes(appxaddr, buf, len);
}

/**
  * @brief  write byte array to flash
	* @param  FLASH_Sector
	* @retval -1 means error
	* @Note   Also active for OTP. OTP address: 0X1FFF7800~0X1FFF7A0F
  */
int STMFLASH_write_bytes(uint32_t appxaddr,uint8_t *buf,uint16_t len)
{
	int i;
	int ret = 0;
	FLASH_Status flash_ret;
	// pack_length must be a multiple of 4
	// pack_length is less than 256
	// that means it will never write cross sector
	if((len & 0x3) != 0 || len > 256 || appxaddr<STM32_FLASH_BASE)
	{
		printf("buffer length check failed\r\n");
		return -1;
	}	
	
	FLASH_Unlock();							
	
	if(appxaddr < 0X1FFF0000)			//do not erase OTP sector
	{
		//printf("Erasing sector %d\r\n", FLASH_Sector_to_index(STMFLASH_GetFlashSector(appxaddr)));
		FLASH_ClearFlag( FLASH_FLAG_EOP |  FLASH_FLAG_PGERR);
		if((flash_ret = erase_sector_once(STMFLASH_GetFlashSector(appxaddr))) != FLASH_COMPLETE)
		{
			printf("erase sector failed, error code: %d\r\n", flash_ret);
			RESET_SECTOR_ERASE_MARK();
			ret = -1;
		}
	}
//	printf("Erase finished...\r\nStart to write\r\n");
	if(ret == 0) //did not get any error
	{
		for(i = 0; i < len; i+= 4)
		{
			if((flash_ret = FLASH_ProgramWord(appxaddr + i, *(uint32_t*)(buf + i))) != FLASH_COMPLETE)
			{ 
				printf("flash program failed, error code: %d\r\n", flash_ret);
				RESET_SECTOR_ERASE_MARK();
				ret = -1;
				break;
			}
		}
	}
//	printf("Flash programming finished...\r\n");
	FLASH_Lock();
		
	return ret;
}


#include "stm32f10x_flash.h"

#define StartServerManageFlashAddress    ((u32)0x08036000)//读写起始地址（内部flash的主存储块地址从0x08036000开始）

//从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{    
    uint32_t offsetAddress=startAddress - FLASH_BASE;               //计算去掉0X08000000后的实际偏移地址
  uint32_t sectorPosition=offsetAddress/STM_SECTOR_SIZE;            //计算扇区地址，对于STM32F103VET6为0~255
  uint32_t sectorStartAddress=sectorPosition*STM_SECTOR_SIZE+FLASH_BASE;    //对应扇区的首地址
    uint16_t dataIndex;
    
  if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE + STM_SECTOR_SIZE * STM32_FLASH_SIZE)))
  {
    return;//非法地址
  }
  FLASH_Unlock();         //解锁写保护

  FLASH_ErasePage(sectorStartAddress);//擦除这个扇区
  
  for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
  {
    FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
  }
  
  FLASH_Lock();//上锁写保护
}

//读取指定地址的半字(16位数据)
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}

//从指定地址开始读取多个数据
void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead)
{
  uint16_t dataIndex;
  for(dataIndex=0;dataIndex<countToRead;dataIndex++)
  {
    readData[dataIndex]=FLASH_ReadHalfWord(startAddress+dataIndex*2);
  }
}

void write_to_flash(void)
{
    u16 buff[1200];
    u16 count_len = 2272 / 2;
    FLASH_WriteMoreData(StartServerManageFlashAddress,buff,count_len);
}

void read_from_flash(void)
{
    u16 buff[1200];
    u16 count_len = 2272 / 2;
    FLASH_WriteMoreData(StartServerManageFlashAddress,buff,count_len);

 }



