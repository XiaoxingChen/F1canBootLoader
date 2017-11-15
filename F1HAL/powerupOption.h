#ifndef POWERUP_OPTION_H
#define POWERUP_OPTION_H

#include <string.h>
#include <stdint.h>
namespace pvf //powerup variables field 
{
	#include "powerupDefaultValue.h"
	
	bool verifyPowerupField();
	
	uint32_t read(powerupVarEnum idx);
	
	void write(powerupVarEnum idx, uint32_t val);
	
	void initInApp();
	
	void printField();
	
	uint32_t getSuitVarNum();
}



#endif
//end of file
