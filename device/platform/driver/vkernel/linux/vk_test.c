/**
 * vk_test.c
*/

/**
 * @defgroup		VK_TEST
 * @author		Tae-Hyuk Kang
 * @note			Virsual Test APIs
 * @file 			vk_test.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>	/*NULL*/
#include "string.h"	/*memcpy*/
#include "vkernel.h"
#include "taskdef.h"
#include "time.h"
#include "htype.h"

/********************************************************************
 Macro Definition
 ********************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_VK

/************** Debuging option *******************/
//#define FUNCTION_CALL_DEBUG  //function Enter Exit¸¦ À§ÇÑ option
/************** Debuging option *******************/


int TEST_VK(void)
{
	return 0;
}

int CMD_SwTest_Vkernel(void *szArgs)
{
	HAPPY(szArgs);
	return 0;
}
/*END OF FILE*/

