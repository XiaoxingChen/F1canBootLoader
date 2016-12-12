#ifndef __PRINTF_H__
#define __PRINTF_H__
#include "CUartConsole.h"
//#include "Console.h"

#define printf(fmt, args...) Console::Instance()->printf(fmt, ##args)
#define is_printf_idel()	Console::Instance()->isTransmitterIdel()

#endif
//end of file
