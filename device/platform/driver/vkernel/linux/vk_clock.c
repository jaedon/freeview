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
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include "taskdef.h"
#include "vkernel.h"
#include "htype.h"

/********************************************************************
 Macro Definition
 ********************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_VK_CLOCK

#ifdef CLOCK_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else 
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)				
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)		
#endif /* CLOCK_DEBUG */


#define	VK_CLOCK_OFFSET_MAX_HOUR		(12)
#define	VK_CLOCK_OFFSET_MIN_HOUR		(-12)
#define	VK_CLOCK_OFFSET_MAX_MIN			(59)
#define	VK_CLOCK_OFFSET_MIN_MIN			(0)

#define VALID_UNIXTIME					(60 * 60 * 24 * 366)			// until 1971/01/02

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/
static VK_CLOCK_DST_MODE	s_eVk_Clock_DstMode = VK_CLOCK_DST_AUTO;

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
	if (!pulUnixTime)
	{
		return VK_ERROR;
	}

	*pulUnixTime = (unsigned long)time(NULL);
	if (*pulUnixTime < VALID_UNIXTIME) {
		*pulUnixTime = 0;
		return VK_INVALID_CLOCK;
	}
	
	return VK_OK;
}

static VK_CLOCK_SET_MODE s_clock_set_mode;

unsigned long VK_CLOCK_GetTimeSource(VK_CLOCK_SET_MODE *peSetMode)
{
	if (peSetMode == NULL)
	{
        PrintError("[VK_CLOCK_GetTimeSource] Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}
	
	*peSetMode = s_clock_set_mode;

	return VK_OK;
}

unsigned long VK_CLOCK_SetTime(VK_CLOCK_SET_MODE eSetMode, unsigned long ulUnixTime)
{
	int		 ulRet;

	if (ulUnixTime < VALID_UNIXTIME) 
	{
		ulUnixTime = VALID_UNIXTIME;
	}

	s_clock_set_mode = eSetMode;
	ulRet = stime ((time_t *)&ulUnixTime);
	if (ulRet != 0)
	{
		return VK_INVALID_CLOCK;
	}
	return VK_OK;
}

unsigned long VK_CLOCK_GetOffsetTime (int *pnHour, int *pnMin)
{
	struct timeval tv;
	struct timezone		 stTimeZone;
	int					 nRet;

	if (pnHour == NULL || pnMin == NULL)
	{
        PrintError("[VK_CLOCK_GetTimeSource] Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	nRet = gettimeofday(&tv, &stTimeZone);
	if (nRet != 0)
	{
		return VK_INVALID_CLOCK;
	}

	// Base unit : 15 min.
	*pnHour	= (stTimeZone.tz_minuteswest / 60);
	*pnMin	= ((stTimeZone.tz_minuteswest / 15) % 4) * 15;
	return VK_OK;
}

unsigned long VK_CLOCK_SetOffsetTime (int nHour, int nMin)
{
	struct timeval	tv;
	struct timezone	 stTimeZone;
	int				 nRet;

	if ((nHour > (int)VK_CLOCK_OFFSET_MAX_HOUR)
		|| (nHour < (int)VK_CLOCK_OFFSET_MIN_HOUR)
		|| (nMin > (int)VK_CLOCK_OFFSET_MAX_MIN)
		|| (nMin < (int)VK_CLOCK_OFFSET_MIN_MIN))
	{
		return VK_INVALID_CLOCK;
	}

	nRet = gettimeofday(&tv, &stTimeZone);
	// Base unit : 15 min.
	stTimeZone.tz_minuteswest = ((nHour * 60 + nMin) / 15) * 15;
	stTimeZone.tz_dsttime	  = 0;
	nRet = settimeofday(&tv, &stTimeZone);

	return (nRet == 0) ? VK_OK : VK_INVALID_CLOCK;
}

int VK_CLOCK_GetDstMode (VK_CLOCK_DST_MODE *peDstMode)
{
	if (peDstMode == NULL)
	{
        PrintError("[VK_CLOCK_GetDstMode] Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	*peDstMode = s_eVk_Clock_DstMode;
	return VK_OK;
}
int VK_CLOCK_SetDstMode (VK_CLOCK_DST_MODE eDstMode)
{
	s_eVk_Clock_DstMode = eDstMode;
	return VK_OK;
}

/* end of file */
