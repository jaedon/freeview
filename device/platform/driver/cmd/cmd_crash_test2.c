/*********************************************************************
*	author :
*
*	file descriptions....
*		:
*
*	Copyright (c) 2004 by Humax Co., Ltd.
*	All rights reserved
********************************************************************/

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <ctype.h>

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"

HUINT8 CMD_IS_BOOL_Func(HUINT8 bool8)
{
	return bool8;
}

void CMD_FLOAT_Func(float inFloat)
{
	DI_UART_Print("CMD_FLOAT_Func in (%f)\n", inFloat);
}
