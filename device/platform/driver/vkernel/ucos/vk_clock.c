/**
 * vk_clock.c
*/

/**
 * @defgroup		VK_CLOCK
 * @author		Tae-Hyuk Kang
 * @note			Virsual Clock APIs
 * @file 			vk_clock.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

/********************************************************************
 Header Files
 ********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include "taskdef.h"
#include "vkernel.h"
#include "htype.h"

#include <linden_trace.h>

/********************************************************************
 Macro Definition
 ********************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_VK_CLOCK


/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/

/********************************************************************
 External Variable Declaration
 ********************************************************************/


/********************************************************************
 Static Function
 ********************************************************************/

/********************************************************************
 Global Function
 ********************************************************************/
int VK_CLOCK_Init (void)
{
	return VK_OK;
}

unsigned long VK_CLOCK_GetTime (unsigned long *pulUnixTime)
{
	HAPPY(pulUnixTime);

	return VK_OK;
}

unsigned long VK_CLOCK_GetTimeSource(VK_CLOCK_SET_MODE *peSetMode)
{
	HAPPY(peSetMode);

	return VK_OK;
}

unsigned long VK_CLOCK_SetTime(VK_CLOCK_SET_MODE eSetMode, unsigned long ulUnixTime)
{
	HAPPY(eSetMode);
	HAPPY(ulUnixTime);

	return VK_OK;
}

unsigned long VK_CLOCK_GetOffsetTime (int *pnHour, int *pnMin)
{
	HAPPY(pnHour);
	HAPPY(pnMin);

	return VK_OK;
}

unsigned long VK_CLOCK_SetOffsetTime (int nHour, int nMin)
{
	HAPPY(nHour);
	HAPPY(nMin);

	return VK_OK;
}

int VK_CLOCK_GetDstMode (VK_CLOCK_DST_MODE *peDstMode)
{
	HAPPY(peDstMode);

	return VK_OK;
}
int VK_CLOCK_SetDstMode (VK_CLOCK_DST_MODE eDstMode)
{
	HAPPY(eDstMode);
	return VK_OK;
}

/* end of file */
