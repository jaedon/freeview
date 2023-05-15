/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   dash_controller.c  $
 * Version:			$Revision:   1.0 $
 * Original Author:
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     Dash controller module
 * Module:
 * Remarks:
 */

/**
 * @defgroup DASH_CONTROLLER
 * @brief DASH 의 구성요소들을 Control 한다.
 */

/**
 * @author
 * @date 30 Jul 2012
 */

/**
 * @brief DASH 내에서 구성요소들을 Control 한다.
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file dash_controller.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "di_iptuner_config.h"
#include "di_uart.h"
#include "dash_controller.h"
#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "util_url.h"

//#define DASH_MEMORY_DEBUG

#ifdef DASH_MEMORY_DEBUG
#include "util_memtool.h"
#define VK_MEM_Alloc(X)		EIKI_MEM_Alloc((HUINT32)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#define VK_MEM_Free(X)		EIKI_MEM_Free((void*)X,(HINT8*)__FUNCTION__,(HUINT32)__LINE__)
#endif


/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
//#define DASH_CONTROLLER_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DASH_CONTROLLER_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif

#ifdef DI_IPTUNER_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/

/*
 *  find the number of occurrence of [needle] in [src]
 */
static HINT32 __find_number_occurrence(const HINT8 *needle, HINT8 *src)
{
	HINT32 count = 0;
	HINT8 *ptr = NULL;
	HINT32  size_needle = 0;

	if( needle == NULL || src == NULL || *needle == '\0' || *src == '\0' )
	{
		return 0;
	}

	size_needle = VK_strlen(needle);
	ptr = (HINT8*)VK_strstr((const char*)src, (const char*)needle);
	while ( ptr != NULL )
	{
		count++;
		ptr += size_needle;
		ptr = (HINT8*)VK_strstr((const char*)ptr, (const char*)needle);
	}

	return count;
}


/*
 *  replace string [from] to string [to] in [src]
 *  return the result string [char *]
 *  NULL means "there are no changes"
 *  if return value is not NULL, then you have to free it later
 */
static HINT8 *__str_replace(const HINT8 *from, const HINT8 *to, HINT8 *src)
{
	HINT32 size_from = VK_strlen(from);
	HINT32 size_to = VK_strlen(to);
	HINT32 size_src = VK_strlen(src);
	HINT8 *result = NULL;
	HINT32 num_occur = 0;
	HINT8 *ptr = NULL;
	HINT8 *prev = NULL;
	HINT32  size_interval = 0;

	if(size_src == 0) return NULL;
	if(size_from == 0) return NULL;

	num_occur = __find_number_occurrence(from, src);
	if(num_occur == 0) return NULL;

	result = (HINT8 *)VK_MEM_Alloc(sizeof(HINT8) * (size_src - num_occur*(size_from - size_to)) + 1) ;
	if( result == NULL )
	{
		PrintError("%s(%d) Not enough memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	result[0] = '\0';
	prev = src;
	ptr = (HINT8*)VK_strstr((const char*)src, (const char*)from);
	while ( ptr != NULL )
	{
		//copy interval
		size_interval = ptr - prev;
		VK_strncat(result, prev, size_interval);
		//copy 'to'
		VK_strncat(result, to, size_to);
		ptr += size_from;
		prev = ptr;
		ptr = (HINT8*)VK_strstr((const char*)ptr, (const char*)from);
	}
	//copy remained string
	VK_strcat(result, prev);

	return result;
}

static HUINT32 __parsing_width(char *nptr)
{
	char *s = (char *)nptr;
	unsigned long long acc = 0;

	while(isspace((int)*s))
	{
		s++;
	}
	while(!VK_isdigit((int)*s))
	{
		s++;
	}
	while(VK_isdigit((int)*s))
	{
		acc = 10 * acc + ((int)*s - '0');
		s++;
	}
	return acc;
}

HINT32	P_DASH_CTRL_GetMaxBandwidth(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_BASE_e tbase)
{
	DASH_MPDPARSER_HANDLE_t mpdparser = pController->mpdparser;
	DASH_AdaptationSet_t* adaptation = NULL;
	DASH_Representation_t* representation = NULL;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_MPD_ERROR_e submerr = DASH_MPD_ERR_OK;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_BANDWIDTH_t maxBandwidth = 0;
	HUINT32 szList = 0;
	HUINT32 loopcnt = 0;
	HUINT32 time;


	if(tbase == DASH_TIME_BASE_CONNECTING)
	{
		time = pController->CurVideoConnectTime;
	}
	else if(tbase == DASH_TIME_BASE_BUFFERING)
	{
		time = pController->CurrentBufferingTime;
	}
	//get adaptationset which has video stream or av stream
	merr = DASH_MPD_GetAdaptationSetInfo(mpdparser, &adaptation, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY, DASH_LOCALE_TYPE_NONE, time);
	if(merr == DASH_MPD_ERR_ERROR)
	{
		PrintError("%s(%d) Failed to get adaptationset, time(%d)\n", __FUNCTION__, __LINE__, pController->CurVideoConnectTime);
		VK_SEM_Release(pController->ulSema);
		return -1;
	}
	else if(merr == DASH_MPD_ERR_EOF)
	{
		PrintError("%s(%d) Failed to get adaptationset, time(%d) => over duration\n", __FUNCTION__, __LINE__, pController->CurVideoConnectTime);
		VK_SEM_Release(pController->ulSema);
		return -2;
	}
	else if(merr == DASH_MPD_ERR_ADAPTATION_NOTFOUND)
	{
		PrintError("%s(%d) Failed to get given adaptationset, try to find AV stream\n", __FUNCTION__, __LINE__);
		submerr = DASH_MPD_GetAdaptationSetInfo(mpdparser, &adaptation, DASH_ADAPTATIONSET_TYPE_AV, DASH_LOCALE_TYPE_NONE, time);
		if(submerr == DASH_MPD_ERR_ADAPTATION_NOTFOUND)
		{
			PrintError("%s(%d) Failed to get alternative adaptationset\n", __FUNCTION__, __LINE__);
			VK_SEM_Release(pController->ulSema);
			return -1;
		}
	}

	szList = UTIL_DLLIST_Size(&adaptation->Representation);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&adaptation->Representation, loopcnt);
		representation = dllnode->pData;
		if(maxBandwidth < representation->bandwidth)
		{
			maxBandwidth = representation->bandwidth;
		}
	}

	return maxBandwidth;
}


HINT32 P_DASH_CTRL_GetMinBandwidth(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_BASE_e tbase)
{
	DASH_MPDPARSER_HANDLE_t mpdparser = pController->mpdparser;
	DASH_AdaptationSet_t* adaptation = NULL;
	DASH_Representation_t* representation = NULL;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_MPD_ERROR_e submerr = DASH_MPD_ERR_OK;
	UTIL_DLLNode_t* dllnode = NULL;
	DASH_BANDWIDTH_t minBandwidth = 0x7FFFFFFF;
	HUINT32 szList = 0;
	HUINT32 loopcnt = 0;
	HUINT32 time;

	if(tbase == DASH_TIME_BASE_CONNECTING)
	{
		time = pController->CurVideoConnectTime;
	}
	else if(tbase == DASH_TIME_BASE_BUFFERING)
	{
		time = pController->CurVideoConnectTime;
	}
	merr = DASH_MPD_GetAdaptationSetInfo(mpdparser, &adaptation, DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY, DASH_LOCALE_TYPE_NONE, time);
	if(merr == DASH_MPD_ERR_ERROR)
	{
		PrintError("%s(%d) Failed to get adaptationset, time(%d)\n", __FUNCTION__, __LINE__, pController->CurVideoConnectTime);
		return -1;
	}
	else if(merr == DASH_MPD_ERR_EOF)
	{
		PrintError("%s(%d) Failed to get adaptationset, time(%d) => over duration\n", __FUNCTION__, __LINE__, pController->CurVideoConnectTime);
		return -2;
	}
	else if(merr == DASH_MPD_ERR_ADAPTATION_NOTFOUND)
	{
		PrintError("%s(%d) Failed to get given adaptationset, try to find AV stream\n", __FUNCTION__, __LINE__);
		submerr = DASH_MPD_GetAdaptationSetInfo(mpdparser, &adaptation, DASH_ADAPTATIONSET_TYPE_AV, DASH_LOCALE_TYPE_NONE, time);
		if(submerr == DASH_MPD_ERR_ADAPTATION_NOTFOUND)
		{
			PrintError("%s(%d) Failed to get alternative adaptationset\n", __FUNCTION__, __LINE__);
			VK_SEM_Release(pController->ulSema);
			return -1;
		}
	}
	szList = UTIL_DLLIST_Size(&adaptation->Representation);
	for(loopcnt = 0; loopcnt < szList; loopcnt++)
	{
		dllnode = UTIL_DLLIST_Get_n_Node(&adaptation->Representation, loopcnt);
		representation = dllnode->pData;
		if(minBandwidth > representation->bandwidth)
		{
			minBandwidth = representation->bandwidth;
		}
	}
	return minBandwidth;
}

/****************************************************************
*                      Controller APIs
****************************************************************/

/**
 * @b Function @b Description <br>
 * Create DASH controller <br>
 *
 * @return  #  DASH controller handle <br>
 */
DASH_CONTROLLER_HANDLE_t DASH_CTRL_Create(void)
{
	HINT32 nRet = 0;
	HINT8 sem_name[128]={0};

	PrintEnter;
	DASH_CONTROLLER_t *pInstance = VK_MEM_Alloc(sizeof(DASH_CONTROLLER_t));
	if(pInstance == NULL)
	{
		PrintError("%s(%d) Error : Not enough memory\n", __FUNCTION__, __LINE__);
		PrintExit;
		return NULL;
	}
	VK_MEM_Memset(pInstance, 0, sizeof(DASH_CONTROLLER_t));
	nRet = VK_SEM_Create((unsigned long*)&pInstance->ulSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if(nRet != VK_OK)
	{
		PrintError("%s(%d) Create Semaphore Fail\n\r",__FUNCTION__,__LINE__, nRet);
		VK_MEM_Free(pInstance);
		pInstance = NULL;
		return NULL;
	}
	nRet = VK_SEM_Create((unsigned long*)&pInstance->ulCmdlock, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if(nRet != VK_OK)
	{
		PrintError("%s(%d) Create Semaphore Fail\n\r",__FUNCTION__,__LINE__, nRet);
		VK_MEM_Free(pInstance);
		pInstance = NULL;
		return NULL;
	}

	PrintDebug("%s(%d) Controller has been allocated\n", __FUNCTION__, __LINE__);
	PrintExit;
	return (DASH_CONTROLLER_HANDLE_t)pInstance;
}

/**
 * @b Function @b Description <br>
 * Destroy controller <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_Destroy(DASH_CONTROLLER_HANDLE_t pController)
{
	PrintEnter;
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(pController->ulSema);
	if(pController->MPDUrl != NULL)
	{
		VK_MEM_Free(pController->MPDUrl);
		pController->MPDUrl = NULL;
	}
	VK_SEM_Release(pController->ulSema);
	VK_SEM_Destroy(pController->ulSema);
	VK_SEM_Destroy(pController->ulCmdlock);
	VK_MEM_Free(pController);
	pController = NULL;
	PrintDebug("%s(%d) Controller has been destroyed\n", __FUNCTION__, __LINE__);
	PrintExit;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set MPD Url <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] stURL	: URL string
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetMPDUrl(DASH_CONTROLLER_HANDLE_t pController, HINT8* stURL)
{
	PrintEnter;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(pController->ulSema);
	if(pController->MPDUrl != NULL)
	{
		VK_MEM_Free(pController->MPDUrl);
		pController->MPDUrl = NULL;
	}
	pController->MPDUrl = VK_MEM_Alloc(VK_strlen(stURL)+1);
	if(pController->MPDUrl == NULL)
	{
		PrintError("%s(%d) Not enough memory!\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_MEM_Memset(pController->MPDUrl, 0, VK_strlen(stURL)+1);
	VK_MEM_Memcpy(pController->MPDUrl, stURL, VK_strlen(stURL)+1);
	VK_SEM_Release(pController->ulSema);

	PrintExit;

	return DASH_ERR_OK;

}

/**
 * @b Function @b Description <br>
 * Set probing method (this is needed by TS probing) <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eMethod	: probing method value
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetProbeMethod(DASH_CONTROLLER_HANDLE_t pController, DASH_PROBE_METHOD_e eMethod)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->probemethod = eMethod;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get probing method (this is needed by TS probing) <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  probing method value <br>
 */
DASH_PROBE_METHOD_e	DASH_CTRL_GetProbeMethod(DASH_CONTROLLER_HANDLE_t pController)
{
	DASH_PROBE_METHOD_e probe;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_PROBE_NONE;
	}
	VK_SEM_Get(pController->ulSema);
	probe = pController->probemethod;
	VK_SEM_Release(pController->ulSema);

	return probe;
}

/**
 * @b Function @b Description <br>
 * Set MPD Parser <br>
 * Controller always refers to connected MPD Parser when looking for segment or MPD value <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] mpd : mpd parser handle
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetMPDParser(DASH_CONTROLLER_HANDLE_t pController, DASH_MPDPARSER_HANDLE_t mpd)
{
	PrintEnter;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	if(mpd == NULL)
	{
		PrintError("%s(%d) Invalid MPD handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->mpdparser = mpd;
	VK_SEM_Release(pController->ulSema);

	PrintExit;

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get minimum update period <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  update period in MS <br>
 */
HINT32 DASH_CTRL_GetUpdatePeriod(DASH_CONTROLLER_HANDLE_t pController)
{
	HINT32 updatePeriod = 0;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	updatePeriod = DASH_MPD_GetMPDUpdatePeriod(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);

	return updatePeriod;
}

/**
 * @b Function @b Description <br>
 * Check whether MPD is updatable or not <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  bool <br>
 */
HBOOL DASH_CTRL_GetIsLiveUpdate(DASH_CONTROLLER_HANDLE_t pController)
{
	HBOOL isLive;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(pController->ulSema);
	isLive = DASH_MPD_Get_IsLiveUpdate(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);

	return isLive;
}

/**
 * @b Function @b Description <br>
 * Get segment format in MPD (MP4 or TS) <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  Segment type value <br>
 */
DASH_SEGMENT_FORMAT_e DASH_CTRL_GetSegmentFormat(DASH_CONTROLLER_HANDLE_t pController)
{
	DASH_SEGMENT_FORMAT_e segform;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(pController->ulSema);
	segform = DASH_MPD_GetSegmentFormat(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);

	return segform;
}

/**
 * @b Function @b Description <br>
 * Get current connecting time <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eTimebase : timebase (Audio or Video)
 *
 * @return  #  time <br>
 */
DASH_TIME_t DASH_CTRL_GetCurrentConnectingTime(DASH_CONTROLLER_HANDLE_t pController, DASH_CONNECTTIME_e eTimebase)
{
	DASH_TIME_t time;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pController->ulSema);
	if(eTimebase == DASH_CONNECTTIME_VIDEO)
	{
		time = pController->CurVideoConnectTime;
	}
	else if(eTimebase == DASH_CONNECTTIME_AUDIO)
	{
		time = pController->CurAudioConnectTime;
	}
	else
	{
		VK_SEM_Release(pController->ulSema);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Release(pController->ulSema);

	return time;
}

/**
 * @b Function @b Description <br>
 * Set current connecting time <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] tTime : time
 * @param[in] eTimebase : timebase (Audio or Video)
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetCurrentConnectingTime(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_t tTime, DASH_CONNECTTIME_e eTimebase)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	if(eTimebase == DASH_CONNECTTIME_VIDEO)
	{
		pController->CurVideoConnectTime = tTime;
	}
	else if(eTimebase == DASH_CONNECTTIME_AUDIO)
	{
		pController->CurAudioConnectTime = tTime;
	}
	else
	{
		VK_SEM_Release(pController->ulSema);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get current buffering time <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  time <br>
 */
DASH_TIME_t DASH_CTRL_GetCurrentBufferingTime(DASH_CONTROLLER_HANDLE_t pController)
{
	DASH_TIME_t time;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pController->ulSema);
	time = pController->CurrentBufferingTime;
	VK_SEM_Release(pController->ulSema);

	return time;
}

/**
 * @b Function @b Description <br>
 * Set current buffering time <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eTime : time
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetCurrentBufferingTime(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_t etime)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->CurrentBufferingTime = etime;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set locale information for audio <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eLocale : locale value
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetCurrentLocale(DASH_CONTROLLER_HANDLE_t pController, DASH_LOCALE_TYPE_e eLocale)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->CurrentLocale = eLocale;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set current bandwidth <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] tBandwidth : bandwidth
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetCurrentBandwidth(DASH_CONTROLLER_HANDLE_t pController, DASH_BANDWIDTH_t tBandwidth)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(tBandwidth < 0)
	{
		PrintError("%s(%d) Invalid bandwidth\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->CurrentBandwidth = tBandwidth;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get minimum buffer time in MS  <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  time <br>
 */
HUINT32 DASH_CTRL_GetMinBufferTime(DASH_CONTROLLER_HANDLE_t pController)
{
	DASH_MPD_t* mpd = NULL;
	HUINT32 minBufferTime = 0;

	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	mpd = pController->mpdparser->parsedMPD;
	minBufferTime = mpd->minBufferTime;
	VK_SEM_Release(pController->ulSema);

	return minBufferTime;
}

/**
 * @b Function @b Description <br>
 * Get maximum bandwidth <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eTimebase : timebase (Audio or Video)
 *
 * @return  #  bandwidth <br>
 */
HINT32	DASH_CTRL_GetMaxBandwidth(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_BASE_e eTimebase)
{
	DASH_BANDWIDTH_t bandwidth = 0;
	VK_SEM_Get(pController->ulSema);
	bandwidth = P_DASH_CTRL_GetMaxBandwidth(pController, eTimebase);
	VK_SEM_Release(pController->ulSema);

	return bandwidth;
}

/**
 * @b Function @b Description <br>
 * Get current bandwidth <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  bandwidth <br>
 */
HUINT32	DASH_CTRL_GetCurrentBandwidth(DASH_CONTROLLER_HANDLE_t pController)
{
	HINT32 curBandwidth = 0;

	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pController->ulSema);
	curBandwidth = pController->CurrentBandwidth;
	VK_SEM_Release(pController->ulSema);

	return curBandwidth;
}

/**
 * @b Function @b Description <br>
 * Get media duration <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  duration in ms <br>
 */
HINT64 DASH_CTRL_GetMediaDuration(DASH_CONTROLLER_HANDLE_t pController)
{
	HINT64 mediaDuration = 0LL;

	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pController->ulSema);
	mediaDuration = DASH_MPD_GetMediaDuration(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);

	return DASH_CTRL_ConvertNonScaledTimetoMs(pController, mediaDuration);
}

/**
 * @b Function @b Description <br>
 * Get the number of tracks in MPD (based on adaptation set) <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  the number of tracks <br>
 */
HINT32 DASH_CTRL_GetTotalTracks(DASH_CONTROLLER_HANDLE_t pController)
{
	HINT32 nTracks = 0;

	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pController->ulSema);
	nTracks = DASH_MPD_GetTotalTracks(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);

	return nTracks;
}

/**
 * @b Function @b Description <br>
 * Get stream type of (n)th track <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] nTrack : (n)th track
 *
 * @return  #  stream type value <br>
 */
DASH_ADAPTATIONSET_TYPE_e DASH_CTRL_GetTrackType(DASH_CONTROLLER_HANDLE_t pController, HUINT32 nTrack)
{
	DASH_ADAPTATIONSET_TYPE_e nTracks = 0;

	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pController->ulSema);
	nTracks = DASH_MPD_GetTrackType(pController->mpdparser, nTrack);
	VK_SEM_Release(pController->ulSema);

	return nTracks;
}

/**
 * @b Function @b Description <br>
 * Get codec of (n)th track <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] nTrack : (n)th track
 *
 * @return  #  Error code <br>
 */
DASH_CODEC_e DASH_CTRL_GetTrackCodec(DASH_CONTROLLER_HANDLE_t pController, HUINT32 nTrack)
{
	DASH_CODEC_e codec = 0;

	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pController->ulSema);
	codec = DASH_MPD_GetTrackCodec(pController->mpdparser, nTrack);
	VK_SEM_Release(pController->ulSema);

	return codec;
}

/**
 * @b Function @b Description <br>
 * Get language of track (for audio) <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] nTrack : (n)th track
 *
 * @return  #  language string <br>
 */
DASH_STRING_t DASH_CTRL_GetTrackLang(DASH_CONTROLLER_HANDLE_t pController, HUINT32 nTrack)
{
	DASH_STRING_t str;
	str.str = NULL;
	str.length = 0;

	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return str;
	}
	VK_SEM_Get(pController->ulSema);
	str = DASH_MPD_GetTrackLang(pController->mpdparser, nTrack);
	VK_SEM_Release(pController->ulSema);
	return str;
}

/**
 * @b Function @b Description <br>
 * Set current buffering mode : prebuffering or normal<br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eBufmode : buffering mode value
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetBufferingMode(DASH_CONTROLLER_HANDLE_t pController, DASH_BUFFERING_MODE_e eBufmode)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->BufferingMode = eBufmode;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get current buffering mode <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  buffering mode value <br>
 */
DASH_BUFFERING_MODE_e DASH_CTRL_GetBufferingMode(DASH_CONTROLLER_HANDLE_t pController)
{
	DASH_BUFFERING_MODE_e bufferingmode;
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	bufferingmode = pController->BufferingMode;
	VK_SEM_Release(pController->ulSema);

	return bufferingmode;
}

/**
 * @b Function @b Description <br>
 * Set current buffering item (preconnector item) <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] pItem : preconnector item which is in buffering
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetCurrentBufferingItem(DASH_CONTROLLER_HANDLE_t pController, DASH_PRECONNECTOR_ITEM_t* pItem)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pItem == NULL)
	{
		PrintError("%s(%d) Error : pItem is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->inBufferingItem = pItem;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get current buffering item (preconnector item)  <br>
 *
 * @param[in] pController	: controller handle
 * @param[out] pTime : buffering item info
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_GetCurrentBufferingItem(DASH_CONTROLLER_HANDLE_t pController, DASH_PRECONNECTOR_ITEM_t* pItem)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pItem == NULL)
	{
		PrintError("%s(%d) Error : pItem is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	VK_MEM_Memcpy(pItem, pController->inBufferingItem, sizeof(DASH_PRECONNECTOR_ITEM_t));
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Check that currently End of Stream is reached or not <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  true or false <br>
 */
HBOOL DASH_CTRL_GetIsEndOfStream(DASH_CONTROLLER_HANDLE_t pController)
{
	HBOOL isEndofStream = FALSE;
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(pController->ulSema);
	isEndofStream = pController->bIsEndofStream;
	VK_SEM_Release(pController->ulSema);

	return isEndofStream;
}

/**
 * @b Function @b Description <br>
 * Set EOF flag <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] bVal : bool value
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetIsEndOfStream(DASH_CONTROLLER_HANDLE_t pController, HBOOL bVal)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->bIsEndofStream = bVal;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Wait for read completion by external buffer reader  <br>
 * this is for synchronization between DASH module and external module <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_WaitForReadCompletion(DASH_CONTROLLER_HANDLE_t pController)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	while(DASH_CTRL_GetIsReadCompletion(pController) != TRUE)
	{
		VK_TASK_Sleep(DASH_MINIMUM_TASK_SLEEP);
	}
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get read completion flag  <br>
 * this is for synchronization between DASH module and external module <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  true or false <br>
 */
HBOOL DASH_CTRL_GetIsReadCompletion(DASH_CONTROLLER_HANDLE_t pController)
{
	HBOOL isReadCompletion = FALSE;
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(pController->ulSema);
	isReadCompletion = pController->bIsReadCompletion;
	VK_SEM_Release(pController->ulSema);

	return isReadCompletion;

}

/**
 * @b Function @b Description <br>
 * Set read completion flag <br>
 * this is for synchronization between DASH module and external module <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] bVal	: true or false
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetIsReadCompletion(DASH_CONTROLLER_HANDLE_t pController, HBOOL bVal)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->bIsReadCompletion = bVal;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get time scale of MPD <br>
 * warn) this could be different than time scale of segment <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  Time scale value <br>
 */
HINT64 DASH_CTRL_GetTimeScale(DASH_CONTROLLER_HANDLE_t pController)
{
	HINT64 timeScale = 0LL;
	VK_SEM_Get(pController->ulSema);
	timeScale = DASH_MPD_GetTimeScale(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);

	return timeScale;
}

/**
 * @b Function @b Description <br>
 * Convert milisecond to non-scaled time based on timescale of MPD <br>
 * non-scaled time means original time in MPD, that is, not converted by timescale value <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] val : milisecond time
 *
 * @return  #  non-scaled time <br>
 */
HINT64 DASH_CTRL_ConvertMStoNonScaledTime(DASH_CONTROLLER_HANDLE_t pController, HINT64 val)
{
	HINT64 timeScale = 0LL;
	VK_SEM_Get(pController->ulSema);
	timeScale = DASH_MPD_GetTimeScale(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);
	return val / 1000 * timeScale;
}

/**
 * @b Function @b Description <br>
 * Convert non-scaled time to milisecond based on timescale of MPD<br>
 * non-scaled time means original time in MPD, that is, not converted by timescale value <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] val : non-scaled time
 *
 * @return  #  milisecond time <br>
 */
HINT64 DASH_CTRL_ConvertNonScaledTimetoMs(DASH_CONTROLLER_HANDLE_t pController, HINT64 val)
{
	HINT64 timeScale = 0LL;
	VK_SEM_Get(pController->ulSema);
	timeScale = DASH_MPD_GetTimeScale(pController->mpdparser);
	VK_SEM_Release(pController->ulSema);
	return val / timeScale * 1000;
}

DASH_ERROR_e P_DASH_CTRL_AppendBaseURL(DASH_BaseURLSet_t* urlset, HINT8* dest)
{
	UTIL_DLLNode_t *dllist = NULL;
	DASH_BaseURL_t* url = NULL;
	if( UTIL_DLLIST_Size(urlset) != 0 )
	{
		dllist = UTIL_DLLIST_GetHeadNode(urlset);
		url = dllist->pData;

		if(url->serviceLocation.str != NULL)
		{
			VK_strncat(dest, url->serviceLocation.str, VK_strlen(url->serviceLocation.str)+1);
			return DASH_MPD_ERR_OK;
		}
		else
		{
			PrintError("%s(%d) URL string is NULL\n",__FUNCTION__,__LINE__);
			return DASH_MPD_ERR_ERROR;
		}
	}
	else
	{
		return DASH_MPD_ERR_ERROR;
	}
}

DASH_ERROR_e P_DASH_CTRL_Append_n_BaseURL(DASH_BaseURLSet_t* urlset, HINT8* dest, HUINT32 nth)
{
	UTIL_DLLNode_t *dllist = NULL;
	DASH_BaseURL_t* url = NULL;
	if( UTIL_DLLIST_Size(urlset) != 0 )
	{
		dllist = UTIL_DLLIST_Get_n_Node(urlset, nth);
		url = dllist->pData;

		if(url->serviceLocation.str != NULL)
		{
			VK_strncat(dest, url->serviceLocation.str, VK_strlen(url->serviceLocation.str)+1);
			return DASH_MPD_ERR_OK;
		}
		else
		{
			PrintError("%s(%d) URL string is NULL\n",__FUNCTION__,__LINE__);
			return DASH_MPD_ERR_ERROR;
		}
	}
	else
	{
		return DASH_MPD_ERR_ERROR;
	}
}

/**
 * @b Function @b Description <br>
 * Find immediate elements based on stream type, locale, time and bandwidth  <br>
 * This is a part of mechanism that looks for current segment <br>
 *
 * @param[in] pController	: controller handle
 * @param[out] element	: immediate element structure
 * @param[in] type : stream(adaptationset) type
 * @param[in] locale : locale information(this should provide when type is audio)
 * @param[in] time : base time
 * @param[in] bandwidth : base bandwidth
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e P_DASH_CTRL_GetImmediateElement(DASH_CONTROLLER_HANDLE_t pController, DASH_IMMEDIATE_ELEMENT_t *element, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time, DASH_BANDWIDTH_t bandwidth)
{
	DASH_MPDPARSER_HANDLE_t mpdparser = pController->mpdparser;
	DASH_MPD_t* mpd = pController->mpdparser->parsedMPD;
	DASH_MPD_ERROR_e merr = DASH_MPD_ERR_OK;
	DASH_MPD_ERROR_e submerr = DASH_MPD_ERR_OK;
	DASH_Period_t* period = NULL;
	DASH_AdaptationSet_t* adaptation = NULL;
	DASH_Representation_t* representation = NULL;
	HUINT32 nBaseURLIdx = 0;
	HUINT32 szList = 0;
	HUINT32 szSegduration = 0;
	HUINT8 url_backup[DASH_MAX_URL_LEN];
	VK_MEM_Memset(url_backup, 0, DASH_MAX_URL_LEN);

	element->mpd = mpd;
	element->selectedElement = DASH_LAST_ELEMENT_MPD;
	// Check Base URL in MPD
	merr = P_DASH_CTRL_AppendBaseURL(&mpd->BaseURL, element->derivedURL);
	if (merr == DASH_MPD_ERR_ERROR)
	{
		//PrintError("%s(%d) There are not BaseURLs in mpd\n",__FUNCTION__,__LINE__);
	}

	// Get Period info
	merr = DASH_MPD_GetPeriodInfo(mpdparser, &period, time);
	if (merr == DASH_MPD_ERR_PERIOD_NOTFOUND)
	{
		return DASH_ERR_ERROR;
	}
	else if (merr == DASH_MPD_ERR_EOF)
	{
		return DASH_ERR_EOF;
	}

	element->period = period;
	element->selectedElement = DASH_LAST_ELEMENT_PERIOD;

	// set segment list info
	if(period->isSegmentList == TRUE)
	{
		element->segmentlist = &period->SegmentList;
		element->whereSegmentlist = DASH_LAST_ELEMENT_PERIOD;
	}
	if(period->isSegmentTemplate == TRUE)
	{
		element->segmenttemplate = &period->SegmentTemplate;
		element->whereSegmenttemplate = DASH_LAST_ELEMENT_PERIOD;
	}
	if(period->isSegmentBase == TRUE)
	{
		element->segmentbase = &period->SegmentBase;
		element->whereSegmentbase = DASH_LAST_ELEMENT_PERIOD;
	}

	//check Base URL in Period
	merr = P_DASH_CTRL_AppendBaseURL(&period->BaseURL, element->derivedURL);
	if (merr == DASH_MPD_ERR_ERROR)
	{
		//PrintError("%s(%d) There are BaseURLs in period\n",__FUNCTION__,__LINE__);
	}

	//check Adaptationset if it exists
	if( UTIL_DLLIST_Size(&period->AdaptationSet) == 0)
	{
		/* if enter this area, it means period is last element */
		return DASH_MPD_ERR_OK;
	}

	// Get Adaptation info
	merr = DASH_MPD_GetAdaptationSetInfo(mpdparser, &adaptation, type, locale, time);
	if (merr == DASH_MPD_ERR_ADAPTATION_NOTFOUND)
	{
		if(type == DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY)
		{
			type = DASH_ADAPTATIONSET_TYPE_AV;
		}
		else if(type == DASH_ADAPTATIONSET_TYPE_AV)
		{
			type = DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY;
		}
		else
		{
			return DASH_ERR_ERROR;
		}
		submerr = DASH_MPD_GetAdaptationSetInfo(mpdparser, &adaptation, type, locale, time);
		if(submerr != DASH_MPD_ERR_OK)
		{
			return DASH_ERR_OK;
//			DASH_CTRL_SetCurrentAVType(pController, type);
		}
	}
	else if (merr == DASH_MPD_ERR_EOF)
	{
		return DASH_ERR_EOF;
	}

	element->adaptation = adaptation;
	element->selectedElement = DASH_LAST_ELEMENT_ADAPTATION;

	// set segment list info
	if(adaptation->isSegmentList == TRUE)
	{
		element->segmentlist = &adaptation->SegmentList;
		element->whereSegmentlist = DASH_LAST_ELEMENT_ADAPTATION;
	}
	if(adaptation->isSegmentTemplate == TRUE)
	{
		element->segmenttemplate = &adaptation->SegmentTemplate;
		element->whereSegmenttemplate = DASH_LAST_ELEMENT_ADAPTATION;
	}
	if(adaptation->isSegmentBase == TRUE)
	{
		element->segmentbase = &adaptation->SegmentBase;
		element->whereSegmentbase = DASH_LAST_ELEMENT_ADAPTATION;
	}

	//check Base URL in Adaptation
	merr = P_DASH_CTRL_AppendBaseURL(&adaptation->BaseURL, element->derivedURL);
	if (merr == DASH_MPD_ERR_ERROR)
	{
		//PrintError("%s(%d) There are not BaseURLs in adaptation\n",__FUNCTION__,__LINE__);
	}
	else if (merr == DASH_MPD_ERR_EOF)
	{
		return DASH_ERR_EOF;
	}

	//check Representation if it exists
	if( UTIL_DLLIST_Size(&adaptation->Representation) == 0)
	{
		/* if enter this area, it means period is last element */
		return DASH_ERR_OK;
	}

	// Get Representation info
	merr = DASH_MPD_GetRepresentationSetInfo(mpdparser, &representation, type, locale, time, bandwidth);
	if (merr == DASH_MPD_ERR_REPRESENTATION_NOTFOUND || representation == NULL)
	{
		return DASH_ERR_OK;
	}

	element->representation = representation;
	element->selectedElement = DASH_LAST_ELEMENT_REPRESENTATION;

	// set segment list info
	if(representation->isSegmentList == TRUE)
	{
		element->segmentlist = &representation->SegmentList;
		element->whereSegmentlist = DASH_LAST_ELEMENT_REPRESENTATION;
	}
	if(representation->isSegmentTemplate == TRUE)
	{
		element->segmenttemplate = &representation->SegmentTemplate;
		element->whereSegmenttemplate = DASH_LAST_ELEMENT_REPRESENTATION;
	}
	if(representation->isSegmentBase == TRUE)
	{
		element->segmentbase = &representation->SegmentBase;
		element->whereSegmentbase = DASH_LAST_ELEMENT_REPRESENTATION;
	}

	/* If Segment duration doest not exist */
	if(adaptation->isSegmentList == FALSE
		&& adaptation->isSegmentTemplate == FALSE
		&& adaptation->isSegmentBase == FALSE)
	{
		szList = UTIL_DLLIST_Size(&representation->BaseURL);
		if(szList > 1)
		{
			szSegduration = period->duration / szList;
			nBaseURLIdx = time / szSegduration;
		}
		if(szList == 1)
		{
			szSegduration = period->duration / szList;
			nBaseURLIdx = 0;
		}

		element->segment_starttime = szSegduration * nBaseURLIdx;
		element->segment_duration = szSegduration;
	}
	//check Base URL in Representation
	merr = P_DASH_CTRL_Append_n_BaseURL(&representation->BaseURL, element->derivedURL, nBaseURLIdx);
	if (merr == DASH_MPD_ERR_ERROR)
	{
		//PrintError("%s(%d) There are not BaseURLs in representation\n",__FUNCTION__,__LINE__);
	}
	return DASH_ERR_OK;
}

#if 0
DASH_ERROR_e P_DASH_CTRL_Derive_SegmentTemplate(DASH_IMMEDIATE_ELEMENT_t* element, UTIL_DLL_t* ret, DASH_TIME_t curTime)
{
	UTIL_DLLNode_t *dllist = NULL;
	DASH_SegmentTimeline_t* timeline;
	HBOOL isTime = FALSE;
	HBOOL isNumber = FALSE;
	HBOOL isBandwidth = FALSE;
	HBOOL isRepresentationId = FALSE;
	HINT32 number_width = 1;
	HINT32 bandwidth_width = 1;
	HINT32 time_width = 1;
	HUINT32 loopcnt = 0;
	HUINT32 subloopcnt = 0;
	HUINT32 elementcnt = 0;
	HUINT32 elementtempcnt = 0;
	HINT8 zeroform[255] = {0,};
	HINT8 compare_str[255] = {0,};
	HINT8 replace_str[255] = {0,};
	HINT8 src[DASH_MAX_URL_LEN];
	HINT8* dest = NULL;

	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Time$"))
	{
		isTime = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "Time%0"))
	{
		time_width = __parsing_width(VK_strstr((const char*)element->segmenttemplate->media.str, "Time%0"));
		isTime = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Number$"))
	{
		isNumber = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Number%0"))
	{
		number_width = __parsing_width(VK_strstr((const char*)element->segmenttemplate->media.str, "$Number%0"));
		isNumber = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Bandwidth$"))
	{
		isBandwidth = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Bandwidth%0"))
	{
		bandwidth_width = __parsing_width(VK_strstr((const char*)element->segmenttemplate->media.str, "$Bandwidth%0"));
		isBandwidth = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$RepresentationID$"))
	{
		isRepresentationId = TRUE;
	}

#if 0
	PrintDebug("%s(%d) media str : %s\n", __FUNCTION__, __LINE__, element->segmenttemplate->media.str);
	PrintDebug("%s(%d) isTime : %d\n", __FUNCTION__, __LINE__,isTime);
	PrintDebug("%s(%d) isNumber : %d\n", __FUNCTION__, __LINE__,isNumber);
	PrintDebug("%s(%d) isBandwidth : %d\n", __FUNCTION__, __LINE__,isBandwidth);
	PrintDebug("%s(%d) isRepresentationId : %d\n", __FUNCTION__, __LINE__,isRepresentationId);
#endif

	/* invalid template media */
	if(isTime == FALSE && isNumber == FALSE && isBandwidth == FALSE && isRepresentationId == FALSE)
	{
		return DASH_ERR_ERROR;
	}

	/* get count of element */
	if(isTime == TRUE)
	{
		for(loopcnt = 0; ; loopcnt++)
		{
			dllist = UTIL_DLLIST_Get_n_Node(&element->segmenttemplate->MultipleSegmentBaseInformation.SegmentTimeline, loopcnt);
			timeline = dllist->pData;
			if(timeline->t != -1)
			{
				elementtempcnt++;
			}
			if(elementtempcnt > elementcnt)
			{
				elementcnt = elementtempcnt;
			}
		}
	}
	if(isNumber == TRUE)
	{
		elementtempcnt =
			curTime / (( element->segmenttemplate->MultipleSegmentBaseInformation.duration /
			element->segmenttemplate->MultipleSegmentBaseInformation.SegmentBaseInformation.timescale ) * 1000) + 1;
		if(elementtempcnt > elementcnt)
		{
			elementcnt = elementtempcnt;
		}
	}
	if(isBandwidth == TRUE || isRepresentationId == TRUE)
	{
		elementtempcnt = UTIL_DLLIST_Size(&element->adaptation->Representation);
		if(elementtempcnt > elementcnt)
		{
			elementcnt = elementtempcnt;
		}
	}

#if 0
	PrintDebug("%s(%d) Total derived url (%d)\n", __FUNCTION__, __LINE__, elementcnt);
#endif

	for(loopcnt = 0; loopcnt < elementcnt; loopcnt++)
	{
		VK_strcpy(src, element->segmenttemplate->media.str);

		if(isTime == TRUE)
		{
			for(subloopcnt = 0; subloopcnt < UTIL_DLLIST_Size(&element->segmenttemplate->MultipleSegmentBaseInformation.SegmentTimeline); subloopcnt++)
			{
				dllist = UTIL_DLLIST_Get_n_Node(&element->segmenttemplate->MultipleSegmentBaseInformation.SegmentTimeline, subloopcnt);
				timeline = dllist->pData;

				if(timeline->t != -1)
				{
					VK_MEM_Memset(compare_str, 0x0, 255);
					if(time_width > 1)
					{
						VK_sprintf((char*)compare_str, "%s%d%s", "$Time%0", time_width, "d$");
					}
					else
					{
						VK_strncpy(compare_str, "$Time$", 6);
					}
					VK_sprintf((char*)replace_str, "%"PRId64"", timeline->t);
					dest = __str_replace(compare_str, replace_str, src);
				}
			}
		}

		if(isNumber == TRUE)
		{
			if(dest != NULL)
			{
				VK_strcpy(src,dest);
				VK_MEM_Free(dest);
				dest = NULL;
			}
			VK_MEM_Memset(compare_str, 0x0, 255);
			VK_sprintf((char *)zeroform, "%%0%dd", number_width);

			if(number_width > 1)
			{
				VK_sprintf((char *)compare_str, "%s%d%s", "$Number%0", number_width, "d$");
			}
			else
			{
				VK_strncpy(compare_str, "$Number$", 8);
			}
			VK_sprintf((char *)replace_str, (char *)zeroform, loopcnt + element->segmenttemplate->MultipleSegmentBaseInformation.startNumber);
			dest = __str_replace(compare_str, replace_str, src);
		}

		if(isBandwidth == TRUE || isRepresentationId == TRUE)
		{
			if(isBandwidth == TRUE)
			{
				if(dest != NULL)
				{
					VK_strcpy(src,dest);
					VK_MEM_Free(dest);
					dest = NULL;
				}
				VK_MEM_Memset(compare_str, 0x0, 255);
				VK_sprintf((char *)zeroform, "%%0%dd", bandwidth_width);

				if(number_width > 1)
				{
					VK_sprintf((char *)compare_str, "%s%d%s", "$Bandwidth%0", number_width, "d$");
				}
				else
				{
					VK_strncpy(compare_str, "$Bandwidth$", 11);
				}
				VK_sprintf((char *)replace_str, (char *)zeroform, element->representation->bandwidth);
				dest = __str_replace(compare_str, replace_str, src);
			}

			if(isRepresentationId == TRUE)
			{
				if(dest != NULL)
				{
					VK_strcpy(src,dest);
					VK_MEM_Free(dest);
					dest = NULL;
				}
				VK_MEM_Memset(compare_str, 0x0, 255);
				VK_strncpy(compare_str, "$RepresentationID$", 18);
				VK_sprintf((char *)replace_str, "%s", element->representation->id.str);
				dest = __str_replace(compare_str, replace_str, src);
			}
		}
		UTIL_DLLIST_Append(ret, dest);
		dest = NULL;
	}
	return DASH_ERR_OK;
}
#endif

DASH_ERROR_e P_DASH_CTRL_GetSegmentTimeInfo(DASH_IMMEDIATE_ELEMENT_t* pElement, DASH_SEGMENTINFO_t* ret, DASH_TIME_t curTime, HUINT32 *nRetIdx)
{
	HUINT32 nSegmentIdx = 0;
	HINT64 llSegmentStarttime = 0LL;
	HINT64 llBaseDuration = 0LL;
	HINT64 llBaseTimeScale = 0LL;
	HINT64 llMPDDuration = 0LL;
	HINT64 llLastSegmentDuration = 0LL;
	HINT32 loopcnt = 0;
	DASH_SegmentTimelineSet_t* tTimeLineSet = NULL;
	DASH_SegmentTimeline_t* tTimeLine = NULL;
	UTIL_DLLNode_t *dllnode = NULL;

	/* get base time method based on type of segment definition */
	if( pElement->whereSegmentlist != DASH_LAST_ELEMENT_NONE )
	{
		if(pElement->segmentlist->MultipleSegmentBaseInformation.isSegmentTimeline == TRUE)
		{
			tTimeLineSet = &pElement->segmentlist->MultipleSegmentBaseInformation.SegmentTimeline;
		}
		else
		{
			llBaseTimeScale =pElement->segmentlist->MultipleSegmentBaseInformation.SegmentBaseInformation.timescale;
			llBaseDuration = pElement->segmentlist->MultipleSegmentBaseInformation.duration;
		}
	}
	if( pElement->whereSegmenttemplate != DASH_LAST_ELEMENT_NONE )
	{
		if(pElement->segmenttemplate->MultipleSegmentBaseInformation.isSegmentTimeline == TRUE)
		{
			tTimeLineSet = &pElement->segmenttemplate->MultipleSegmentBaseInformation.SegmentTimeline;
		}
		else
		{
			llBaseTimeScale =pElement->segmenttemplate->MultipleSegmentBaseInformation.SegmentBaseInformation.timescale;
			llBaseDuration = pElement->segmenttemplate->MultipleSegmentBaseInformation.duration;
		}
	}

	/* get segment index in list */
	llSegmentStarttime = (HUINT32)pElement->period->start;
	if(pElement->mpd->type == DASH_MPD_TYPE_STATIC)
	{
		llMPDDuration = pElement->mpd->mediaPresentationDuration;
	}
	else
	{
		llMPDDuration = 0x7FFFFFFFLL;
	}
	if(tTimeLineSet != NULL)
	{
		/*
		curtime이 period의 마지막에 도달하더라도, 남은 period가 존재할 경우, pElement의 period는
		다음 period 정보를 가지므로 EOS 체크에 걸리지 않는다
		*/

		/* getting segment on EOS */
		if(curTime == pElement->period->start + pElement->period->duration)
		{
			nSegmentIdx = UTIL_DLLIST_Size(tTimeLineSet)-1;
			dllnode = UTIL_DLLIST_GetTailNode(tTimeLineSet);
			tTimeLine = dllnode->pData;
			llSegmentStarttime = (pElement->period->start + pElement->period->duration) - tTimeLine->d;
			return DASH_ERR_EOF;
		}
		else
		{
			dllnode = UTIL_DLLIST_GetHeadNode(tTimeLineSet);
			tTimeLine = dllnode->pData;
			llSegmentStarttime = tTimeLine->t;

			for(loopcnt = 0; loopcnt < (HINT32)UTIL_DLLIST_Size(tTimeLineSet); loopcnt++)
			{
				dllnode = UTIL_DLLIST_Get_n_Node(tTimeLineSet, loopcnt);
				tTimeLine = dllnode->pData;

				if(curTime >= llSegmentStarttime && curTime < llSegmentStarttime + tTimeLine->d)
				{
					llBaseDuration = tTimeLine->d;
					break;
				}
				else
				{
					nSegmentIdx++;
					llSegmentStarttime = llSegmentStarttime + tTimeLine->d;
				}
			}
		}
	}
	else if(tTimeLineSet == NULL)
	{
		/* get segment index using duration and time scale */
		if(llBaseTimeScale != 0 && llBaseDuration != 0)
		{
			/* getting segment on EOS */
			if(curTime == pElement->period->start + pElement->period->duration)
			{
				nSegmentIdx = pElement->period->duration / llBaseDuration -1;
				llLastSegmentDuration = pElement->period->duration % llBaseDuration;
				if(llLastSegmentDuration == 0LL)
				{
					llLastSegmentDuration = llBaseDuration;
				}
				llSegmentStarttime = (pElement->period->start + pElement->period->duration) - llLastSegmentDuration;
				return DASH_ERR_EOF;
			}
			else
			{
				for(loopcnt = 1; ; loopcnt++)
				{
					if(curTime >= llSegmentStarttime && curTime < llSegmentStarttime + llBaseDuration)
					{
							break;
					}
					else
					{
						nSegmentIdx++;
						llSegmentStarttime += llBaseDuration;
					}
				}
			}
		}
		else
		{
			PrintError("%s(%d) Cannot get segment (No timeline and No Base duration, timescale)\n", __FUNCTION__,__LINE__);
			return DASH_ERR_ERROR;
		}
	}

	ret->llSegmentStarttime = llSegmentStarttime;
	ret->llSegmentDuration = llBaseDuration;
	*nRetIdx = nSegmentIdx;

	return DASH_ERR_OK;
}

DASH_ERROR_e P_DASH_CTRL_GetSegInfofromTemplate(DASH_IMMEDIATE_ELEMENT_t* element, DASH_SEGMENTINFO_t* ret, DASH_TIME_t curTime)
{
	UTIL_DLLNode_t *dllist = NULL;
	DASH_SegmentTimeline_t* timeline;
	HBOOL isTime = FALSE;
	HBOOL isNumber = FALSE;
	HBOOL isBandwidth = FALSE;
	HBOOL isRepresentationId = FALSE;
	HINT32 number_width = 1;
	HINT32 bandwidth_width = 1;
	HINT32 time_width = 1;
	HUINT32 loopcnt = 0;
	//HUINT32 subloopcnt = 0;
	HUINT32 nTimeIdx = 0;
	HUINT32 nNumberIdx = 0;
	HINT64	timescale = 0LL;
	HINT64	prevtime = 0LL;
	HINT8 zeroform[255] = {0,};
	HINT8 compare_str[255] = {0,};
	HINT8 replace_str[255] = {0,};
	HINT8 src[DASH_MAX_URL_LEN];
	HINT8* dest = NULL;
	HUINT32 nReserved;
	DASH_ERROR_e derr;


	/* Get time info */
	derr = P_DASH_CTRL_GetSegmentTimeInfo(element, ret, curTime, &nReserved);
	if(derr != DASH_ERR_OK)
	{
		return derr;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Time$"))
	{
		isTime = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "Time%0"))
	{
		time_width = __parsing_width(VK_strstr((const char*)element->segmenttemplate->media.str, "Time%0"));
		isTime = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Number$"))
	{
		isNumber = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Number%0"))
	{
		number_width = __parsing_width(VK_strstr((const char*)element->segmenttemplate->media.str, "$Number%0"));
		isNumber = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Bandwidth$"))
	{
		isBandwidth = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$Bandwidth%0"))
	{
		bandwidth_width = __parsing_width(VK_strstr((const char*)element->segmenttemplate->media.str, "$Bandwidth%0"));
		isBandwidth = TRUE;
	}
	if(NULL != VK_strstr((const char*)element->segmenttemplate->media.str, "$RepresentationID$"))
	{
		isRepresentationId = TRUE;
	}

	/* invalid template media */
	if(isTime == FALSE && isNumber == FALSE && isBandwidth == FALSE && isRepresentationId == FALSE)
	{
		return DASH_ERR_ERROR;
	}

	/* get count of element */
	if(isTime == TRUE)
	{
		/* get timescale from template attribute */
		timescale = element->segmenttemplate->MultipleSegmentBaseInformation.SegmentBaseInformation.timescale;
		for(loopcnt = 0; ; loopcnt++)
		{
			dllist = UTIL_DLLIST_Get_n_Node(&element->segmenttemplate->MultipleSegmentBaseInformation.SegmentTimeline, loopcnt);
			timeline = dllist->pData;
			VK_printf("timeline! : %"PRId64"\n", timeline->t);
			if(timeline->t != -1)
			{
				if(curTime >= prevtime)
				{
					nTimeIdx++;
				}
				if(curTime < timeline->t)
				{
					break;
				}
				else
				{
					prevtime = timeline->t;
				}
			}
		}
	}
	if(isNumber == TRUE)
	{
		nNumberIdx = curTime / element->segmenttemplate->MultipleSegmentBaseInformation.duration;
	}

	VK_strcpy(src, element->segmenttemplate->media.str);

	if(isTime == TRUE)
	{
		dllist = UTIL_DLLIST_Get_n_Node(&element->segmenttemplate->MultipleSegmentBaseInformation.SegmentTimeline, nTimeIdx);
		timeline = dllist->pData;

		VK_MEM_Memset(compare_str, 0x0, 255);
		if(time_width > 1)
		{
			VK_sprintf((char*)compare_str, "%s%d%s", "$Time%0", time_width, "d$");
		}
		else
		{
			VK_strncpy(compare_str, "$Time$", 6);
		}
		VK_sprintf((char*)replace_str, "%"PRId64"", timeline->t);
		dest = __str_replace(compare_str, replace_str, src);
	}

	if(isNumber == TRUE)
	{
		if(dest != NULL)
		{
			VK_strcpy(src,dest);
			VK_MEM_Free(dest);
			dest = NULL;
		}
		VK_MEM_Memset(compare_str, 0x0, 255);
		VK_sprintf((char *)zeroform, "%%0%dd", number_width);

		if(number_width > 1)
		{
			VK_sprintf((char *)compare_str, "%s%d%s", "$Number%0", number_width, "d$");
		}
		else
		{
			VK_strncpy(compare_str, "$Number$", 8);
		}
		VK_sprintf((char *)replace_str, (char *)zeroform, nNumberIdx + element->segmenttemplate->MultipleSegmentBaseInformation.startNumber);
		dest = __str_replace(compare_str, replace_str, src);
	}

	if(isBandwidth == TRUE || isRepresentationId == TRUE)
	{
		if(isBandwidth == TRUE)
		{
			if(dest != NULL)
			{
				VK_strcpy(src,dest);
				VK_MEM_Free(dest);
				dest = NULL;
			}
			VK_MEM_Memset(compare_str, 0x0, 255);
			VK_sprintf((char *)zeroform, "%%0%dd", bandwidth_width);

			if(number_width > 1)
			{
				VK_sprintf((char *)compare_str, "%s%d%s", "$Bandwidth%0", number_width, "d$");
			}
			else
			{
				VK_strncpy(compare_str, "$Bandwidth$", 11);
			}
			VK_sprintf((char *)replace_str, (char *)zeroform, element->representation->bandwidth);
			dest = __str_replace(compare_str, replace_str, src);
		}

		if(isRepresentationId == TRUE)
		{
			if(dest != NULL)
			{
				VK_strcpy(src,dest);
				VK_MEM_Free(dest);
				dest = NULL;
			}
			VK_MEM_Memset(compare_str, 0x0, 255);
			VK_strncpy(compare_str, "$RepresentationID$", 18);
			VK_sprintf((char *)replace_str, "%s", element->representation->id.str);
			dest = __str_replace(compare_str, replace_str, src);
		}
	}

	VK_MEM_Memcpy(ret->pucSegmentURL, element->derivedURL, VK_strlen(element->derivedURL));
	VK_strcat(ret->pucSegmentURL, dest);
	VK_MEM_Free(dest);

	return DASH_ERR_OK;
}

DASH_ERROR_e P_DASH_CTRL_GetSegInfofromList(DASH_IMMEDIATE_ELEMENT_t* pElement, DASH_SEGMENTINFO_t* ret, DASH_TIME_t curTime)
{
	HUINT32 nSegmentIdx = 0;
	UTIL_DLLNode_t *dllnode = NULL;
	//HUINT32 loopcnt = 0;
	DASH_URLTYPE_t *segurl = NULL;
	DASH_ERROR_e derr;

	/* Get time info */
	derr = P_DASH_CTRL_GetSegmentTimeInfo(pElement, ret, curTime, &nSegmentIdx);
	if(derr != DASH_ERR_OK)
	{
		return derr;
	}
	dllnode = UTIL_DLLIST_Get_n_Node(&pElement->segmentlist->SegmentUrl, nSegmentIdx);
	if(dllnode == NULL)
	{
		PrintDebug("%s(%d) Segment template is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
		//DASH_ERR_EOF?
	}
	segurl = dllnode->pData;
	VK_MEM_Memcpy(ret->pucSegmentURL, pElement->derivedURL, VK_strlen(pElement->derivedURL));
	VK_strcat(ret->pucSegmentURL, segurl->sourceURL.str);
	ret->startoffset = segurl->byteRange.start;
	ret->endoffset = segurl->byteRange.end;
	return DASH_ERR_OK;
}

DASH_ERROR_e P_DASH_CTRL_GetInitSegInfo(DASH_CONTROLLER_HANDLE_t pController, DASH_IMMEDIATE_ELEMENT_t* pElement, DASH_SEGMENTINFO_t* seg)
{
	UNUSED_PARAM(pController);
	seg->llSegmentDuration = 0LL;
	seg->llSegmentStarttime = 0LL;
	/* if initialisation segment is in segment list */
	if( pElement->whereSegmentlist != DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmenttemplate == DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmentbase == DASH_LAST_ELEMENT_NONE)
	{
		if(pElement->segmentlist->MultipleSegmentBaseInformation.SegmentBaseInformation.Initialisation.sourceURL.str == NULL)
		{
			return DASH_ERR_INITSEGMENT_NONE;
		}
		seg->startoffset = pElement->segmentlist->MultipleSegmentBaseInformation.SegmentBaseInformation.Initialisation.byteRange.start;
		seg->endoffset = pElement->segmentlist->MultipleSegmentBaseInformation.SegmentBaseInformation.Initialisation.byteRange.end;
		VK_strncpy(seg->pucSegmentURL, pElement->derivedURL, VK_strlen(pElement->derivedURL));
		VK_strncpy(seg->pucSegmentURL+VK_strlen(pElement->derivedURL),
			pElement->segmentlist->MultipleSegmentBaseInformation.SegmentBaseInformation.Initialisation.sourceURL.str,
			VK_strlen(pElement->segmentlist->MultipleSegmentBaseInformation.SegmentBaseInformation.Initialisation.sourceURL.str)+1);
	}
	/* if initialisation segment is in segment template */
	else if( pElement->whereSegmentlist == DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmenttemplate != DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmentbase == DASH_LAST_ELEMENT_NONE )
	{
		if(pElement->segmenttemplate->initialisation.str == NULL)
		{
			return DASH_ERR_INITSEGMENT_NONE;
		}
		seg->startoffset = 0;
		seg->endoffset = 0;
		VK_strncpy(seg->pucSegmentURL, pElement->derivedURL, VK_strlen(pElement->derivedURL));
		VK_strncpy(seg->pucSegmentURL+VK_strlen(pElement->derivedURL),
			pElement->segmenttemplate->initialisation.str,
			VK_strlen(pElement->segmenttemplate->initialisation.str)+1);
	}
	/* if initialisation segment is in segment base */
	else if( pElement->whereSegmentlist == DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmenttemplate == DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmentbase != DASH_LAST_ELEMENT_NONE )
	{
		if(pElement->segmentbase->Initialisation.sourceURL.str == NULL)
		{
			return DASH_ERR_INITSEGMENT_NONE;
		}
		seg->startoffset = pElement->segmentbase->Initialisation.byteRange.start;
		seg->endoffset = pElement->segmentbase->Initialisation.byteRange.end;
		VK_strncpy(seg->pucSegmentURL, pElement->derivedURL, VK_strlen(pElement->derivedURL));
		VK_strncpy(seg->pucSegmentURL+VK_strlen(pElement->derivedURL),
			pElement->segmentbase->Initialisation.sourceURL.str,
			VK_strlen(pElement->segmentbase->Initialisation.sourceURL.str)+1);
	}
	/* if initialisation segment is in segment base and segment list exist */
	else if( pElement->whereSegmentlist != DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmenttemplate == DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmentbase != DASH_LAST_ELEMENT_NONE )
	{
		if(pElement->segmentbase->Initialisation.sourceURL.str == NULL)
		{
			return DASH_ERR_INITSEGMENT_NONE;
		}
		seg->startoffset = pElement->segmentbase->Initialisation.byteRange.start;
		seg->endoffset = pElement->segmentbase->Initialisation.byteRange.end;
		VK_strncpy(seg->pucSegmentURL, pElement->derivedURL, VK_strlen(pElement->derivedURL));
		VK_strncpy(seg->pucSegmentURL+VK_strlen(pElement->derivedURL),
			pElement->segmentbase->Initialisation.sourceURL.str,
			VK_strlen(pElement->segmentbase->Initialisation.sourceURL.str)+1);
	}
	/* if initialisation segment does not exist */
	else
	{
		return DASH_ERR_INITSEGMENT_NONE;
	}

	return DASH_ERR_OK;
}

DASH_ERROR_e P_DASH_CTRL_GetIndexSegInfo(DASH_CONTROLLER_HANDLE_t pController, DASH_IMMEDIATE_ELEMENT_t* pElement, DASH_SEGMENTINFO_t* seg)
{
	UNUSED_PARAM(pController);
	seg->llSegmentDuration = 0LL;
	seg->llSegmentStarttime = 0LL;
	if( pElement->whereSegmentlist != DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmenttemplate == DASH_LAST_ELEMENT_NONE)
	{
		seg->startoffset = pElement->segmentlist->index.byteRange.start;
		seg->endoffset = pElement->segmentlist->index.byteRange.end;
	}
	else if( pElement->whereSegmentlist == DASH_LAST_ELEMENT_NONE &&
		pElement->whereSegmenttemplate != DASH_LAST_ELEMENT_NONE)
	{

	}
	return DASH_ERR_INITSEGMENT_NONE;
}

/**
 * @b Function @b Description <br>
 * Find Segment information based on segment type, stream type, bandwidth and time   <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eSegType : segment type (initialisation, index, media)
 * @param[in] eAdaptationType : stream type
 * @param[in] nBandwidth : base bandwidth
 * @param[in] tTime : base time
 * @param[out] pSeg	: segment info structure
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_FindSegmentInfo (
 DASH_CONTROLLER_HANDLE_t pController,
 DASH_SEGMENT_e eSegType,
 DASH_ADAPTATIONSET_TYPE_e eAdaptaionType,
 DASH_BANDWIDTH_t nBandwidth,
 DASH_TIME_t tTime,
 DASH_SEGMENTINFO_t* pSeg
 )

{
	DASH_ERROR_e derr;
	DASH_IMMEDIATE_ELEMENT_t tElement;

	VK_MEM_Memset(&tElement, 0x0, sizeof(DASH_IMMEDIATE_ELEMENT_t));
	pSeg->eSegmentType = eSegType;

   	/* Get immediate elements from MPD to representation */
	if(eAdaptaionType == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
	{
		derr = P_DASH_CTRL_GetImmediateElement(pController, &tElement, eAdaptaionType, pController->CurrentLocale, tTime, nBandwidth);
	}
	else
	{
		derr = P_DASH_CTRL_GetImmediateElement(pController, &tElement, eAdaptaionType, pController->CurrentLocale, tTime, nBandwidth);
	}
	if(derr == DASH_ERR_ERROR)
	{
		PrintError("%s(%d) Getting segment failed\n",__FUNCTION__,__LINE__);
		return derr;
	}
	else if(derr == DASH_ERR_EOF)
	{
		/* this area will not be entered because of blocking of DI_MEDIA_SetPosition */
		PrintError("%s(%d) the requested time exeeds duration\n",__FUNCTION__,__LINE__);
		return derr;
	}

	/* Fill bandwidth, codec, tracktype information */
	if(tElement.representation != NULL)
	{
		pSeg->nBandwidth = tElement.representation->bandwidth;
	}
	else
	{
		pSeg->nBandwidth = 0;
	}
	if(tElement.adaptation != NULL)
	{
		pSeg->eTrackType = tElement.adaptation->trackType;
		pSeg->eCodec = tElement.adaptation->CommonAttributesElements.eCodec;
	}
	else
	{
		pSeg->eTrackType = DASH_ADAPTATIONSET_TYPE_NONE;
		pSeg->eCodec = tElement.adaptation->CommonAttributesElements.eCodec;
	}

	/* Get Index or Initialisation segment */
	if( eSegType == DASH_SEGMENT_INDEX || eSegType == DASH_SEGMENT_INITIALISATION )
	{
		if(eSegType == DASH_SEGMENT_INDEX)
		{
			derr = P_DASH_CTRL_GetIndexSegInfo(pController, &tElement, pSeg);
		}
		else if(eSegType == DASH_SEGMENT_INITIALISATION)
		{
			derr = P_DASH_CTRL_GetInitSegInfo(pController, &tElement, pSeg);
		}
		return derr;
	}

	/* If There are only "baseurl" element, return the segment directly */
	if( tElement.whereSegmentlist == DASH_LAST_ELEMENT_NONE &&
		tElement.whereSegmenttemplate == DASH_LAST_ELEMENT_NONE)
	{
		pSeg->llSegmentStarttime = tElement.segment_starttime;
		pSeg->llSegmentDuration = tElement.segment_duration;
		VK_MEM_Memcpy(pSeg->pucSegmentURL, tElement.derivedURL, VK_strlen(tElement.derivedURL)+1);
		return DASH_ERR_OK;
	}
	/* Get Segment info if this is "segment list" */
	if(tElement.whereSegmentlist != DASH_LAST_ELEMENT_NONE &&
		tElement.whereSegmenttemplate == DASH_LAST_ELEMENT_NONE)
	{
		if(eAdaptaionType == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
		{
			derr = P_DASH_CTRL_GetSegInfofromList(&tElement, pSeg, tTime);
		}
		else
		{
			derr = P_DASH_CTRL_GetSegInfofromList(&tElement, pSeg, tTime);
		}
		if(derr != DASH_ERR_OK)
		{
			/* this is only method to check EOF */
			VK_printf("derr : %d\n", derr);
			return derr;
		}
		return DASH_ERR_OK;
	}
	/* Get Segment info if this is "segment template" */
	if(tElement.whereSegmenttemplate != DASH_LAST_ELEMENT_NONE &&
		tElement.whereSegmentlist == DASH_LAST_ELEMENT_NONE)
	{
		if(eAdaptaionType == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
		{
			derr = P_DASH_CTRL_GetSegInfofromTemplate(&tElement, pSeg, tTime);
		}
		else
		{
			derr = P_DASH_CTRL_GetSegInfofromTemplate(&tElement, pSeg, tTime);
		}
		if(derr != DASH_ERR_OK)
		{
			return derr;
		}
		return DASH_ERR_OK;
	}
	return DASH_ERR_ERROR;
}

/**
 * @b Function @b Description <br>
 * Find Segment information based on segment type, stream type, bandwidth and time   <br>
 * The difference betweed FindSegmentInfo() and this func. is that this function is automatically increased current connecting time <br>
 * Also no need to provide bandwidth, time information, because this function will be use controller's current value <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] eSegType : segment type (initialisation, index, media)
 * @param[in] eAdaptationType : stream type
 * @param[out] pSeg	: segment info structure
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_GetSegmentInfo(DASH_CONTROLLER_HANDLE_t pController, DASH_SEGMENT_e eSegType, DASH_ADAPTATIONSET_TYPE_e eAdaptaionType, DASH_SEGMENTINFO_t* pSeg)
{
	DASH_ERROR_e derr;
	DASH_IMMEDIATE_ELEMENT_t tElement;

	VK_MEM_Memset(&tElement, 0x0, sizeof(DASH_IMMEDIATE_ELEMENT_t));
	pSeg->eSegmentType = eSegType;

   	/* Get immediate elements from MPD to representation */
	if(eAdaptaionType == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
	{
		derr = P_DASH_CTRL_GetImmediateElement(pController, &tElement, eAdaptaionType, pController->CurrentLocale, pController->CurAudioConnectTime, pController->CurrentBandwidth);
	}
	else
	{
		derr = P_DASH_CTRL_GetImmediateElement(pController, &tElement, eAdaptaionType, pController->CurrentLocale, pController->CurVideoConnectTime, pController->CurrentBandwidth);
	}

	if(derr == DASH_ERR_ERROR)
	{
		PrintError("%s(%d) Getting segment failed\n",__FUNCTION__,__LINE__);
		return derr;
	}
	else if(derr == DASH_ERR_EOF)
	{
		/* this area will not be entered because of blocking of DI_MEDIA_SetPosition */
		PrintError("%s(%d) the requested time exeeds duration\n",__FUNCTION__,__LINE__);
		return derr;
	}

	/* Fill bandwidth, codec, tracktype information */
	if(tElement.representation != NULL)
	{
		pSeg->nBandwidth = tElement.representation->bandwidth;
	}
	else
	{
		pSeg->nBandwidth = 0;
	}

	if(tElement.adaptation != NULL)
	{
		pSeg->eTrackType = tElement.adaptation->trackType;
		pSeg->eCodec = tElement.adaptation->CommonAttributesElements.eCodec;
	}
	else
	{
		pSeg->eTrackType = DASH_ADAPTATIONSET_TYPE_NONE;
		pSeg->eCodec = tElement.adaptation->CommonAttributesElements.eCodec;
	}

	/* Get Index or Initialisation segment */
	if( eSegType == DASH_SEGMENT_INDEX || eSegType == DASH_SEGMENT_INITIALISATION )
	{
		if(eSegType == DASH_SEGMENT_INDEX)
		{
			derr = P_DASH_CTRL_GetIndexSegInfo(pController, &tElement, pSeg);
		}
		else if(eSegType == DASH_SEGMENT_INITIALISATION)
		{
			derr = P_DASH_CTRL_GetInitSegInfo(pController, &tElement, pSeg);
		}
		return derr;
	}

	/* If There are only "baseurl" element, return the segment directly */
	if( tElement.whereSegmentlist == DASH_LAST_ELEMENT_NONE &&
		tElement.whereSegmenttemplate == DASH_LAST_ELEMENT_NONE)
	{
		pSeg->llSegmentStarttime = tElement.segment_starttime;
		pSeg->llSegmentDuration = tElement.segment_duration;
		VK_MEM_Memcpy(pSeg->pucSegmentURL, tElement.derivedURL, VK_strlen(tElement.derivedURL)+1);
		switch(eAdaptaionType)
		{
			case DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY:
				DASH_CTRL_SetCurrentConnectingTime(pController, tElement.segment_starttime + tElement.segment_duration, DASH_CONNECTTIME_VIDEO);
				break;
			case DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY:
				DASH_CTRL_SetCurrentConnectingTime(pController, tElement.segment_starttime + tElement.segment_duration, DASH_CONNECTTIME_AUDIO);
				break;
			case DASH_ADAPTATIONSET_TYPE_AV:
				DASH_CTRL_SetCurrentConnectingTime(pController, tElement.segment_starttime + tElement.segment_duration, DASH_CONNECTTIME_VIDEO);
				DASH_CTRL_SetCurrentConnectingTime(pController, tElement.segment_starttime + tElement.segment_duration, DASH_CONNECTTIME_AUDIO);
				break;
			case DASH_ADAPTATIONSET_TYPE_TEXT:
			default:
				break;
		}
		return DASH_ERR_OK;
	}
	/* Get Segment info if this is "segment list" */
	if(tElement.whereSegmentlist != DASH_LAST_ELEMENT_NONE &&
		tElement.whereSegmenttemplate == DASH_LAST_ELEMENT_NONE)
	{
		if(eAdaptaionType == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
		{
			derr = P_DASH_CTRL_GetSegInfofromList(&tElement, pSeg, pController->CurAudioConnectTime);
		}
		else
		{
			derr = P_DASH_CTRL_GetSegInfofromList(&tElement, pSeg, pController->CurVideoConnectTime);
		}
		if(derr != DASH_ERR_OK)
		{
			/* this is only method to check EOF */
			return derr;
		}
		switch(eAdaptaionType)
		{
			case DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY:
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_VIDEO);
				break;
			case DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY:
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_AUDIO);
				break;
			case DASH_ADAPTATIONSET_TYPE_AV:
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_VIDEO);
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_AUDIO);
				break;
			case DASH_ADAPTATIONSET_TYPE_TEXT:
			default:
				break;
		}
		return DASH_ERR_OK;
	}
	/* Get Segment info if this is "segment template" */
	if(tElement.whereSegmenttemplate != DASH_LAST_ELEMENT_NONE &&
		tElement.whereSegmentlist == DASH_LAST_ELEMENT_NONE)
	{
		if(eAdaptaionType == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
		{
			derr = P_DASH_CTRL_GetSegInfofromTemplate(&tElement, pSeg, pController->CurAudioConnectTime);
		}
		else
		{
			derr = P_DASH_CTRL_GetSegInfofromTemplate(&tElement, pSeg, pController->CurVideoConnectTime);
		}
		if(derr != DASH_ERR_OK)
		{
			return derr;
		}
		switch(eAdaptaionType)
		{
			case DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY:
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_VIDEO);
				break;
			case DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY:
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_AUDIO);
				break;
			case DASH_ADAPTATIONSET_TYPE_AV:
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_VIDEO);
				DASH_CTRL_SetCurrentConnectingTime(pController, pSeg->llSegmentStarttime + pSeg->llSegmentDuration, DASH_CONNECTTIME_AUDIO);
				break;
			case DASH_ADAPTATIONSET_TYPE_TEXT:
			default:
				break;
		}
		return DASH_ERR_OK;
	}
	return DASH_ERR_ERROR;
}

/**
 * @b Function @b Description <br>
 * Set current stream type (Audio or Video) <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] type : stream type value
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetCurrentAVType(DASH_CONTROLLER_HANDLE_t pController, DASH_ADAPTATIONSET_TYPE_e type)
{
	if(pController == NULL)
	{
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->CurrentSelectedAV = type;
	VK_SEM_Release(pController->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get current stream type (Audio or Video) <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  stream type value <br>
 */
DASH_ADAPTATIONSET_TYPE_e DASH_CTRL_GetCurrentAVType(DASH_CONTROLLER_HANDLE_t pController)
{
	DASH_ADAPTATIONSET_TYPE_e ret;

	VK_SEM_Get(pController->ulSema);
	ret = pController->CurrentSelectedAV;
	VK_SEM_Release(pController->ulSema);

	return ret;
}

/**
 * @b Function @b Description <br>
 * Toggle current stream type between Audio and Video <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_ToggleCurrentAVType(DASH_CONTROLLER_HANDLE_t pController)
{
	if(pController == NULL)
	{
		return DASH_ERR_ERROR;
	}
	PrintDebug("%s(%d) AVStream is toggled ( %d -> ", __FUNCTION__, __LINE__, pController->CurrentSelectedAV);
	VK_SEM_Get(pController->ulSema);
	if(pController->CurrentSelectedAV == DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY)
	{
		pController->CurrentSelectedAV = DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY;
	}
	else if(pController->CurrentSelectedAV == DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY)
	{
		pController->CurrentSelectedAV = DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY;
	}
	VK_SEM_Release(pController->ulSema);
	PrintDebug("%d )\n", pController->CurrentSelectedAV);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * set switch for enabling heuristics <br>
 *
 * @param[in] pController	: controller handle
 * @param[in] onoff : true or false
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_CTRL_SetHeuristicsSwitch(DASH_CONTROLLER_HANDLE_t pController, HBOOL onoff)
{
	if(pController == NULL)
	{
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pController->ulSema);
	pController->bHeuristicsOn = onoff;
	VK_SEM_Release(pController->ulSema);
	PrintDebug("Heuristics switch : %d\n", onoff);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * get current heuristics switch <br>
 *
 * @param[in] pController	: controller handle
 *
 * @return  #  true or false <br>
 */
HBOOL DASH_CTRL_GetHeuristicsSwitch(DASH_CONTROLLER_HANDLE_t pController)
{
	HBOOL onoff;

	if(pController == NULL)
	{
		return FALSE;
	}
	VK_SEM_Get(pController->ulSema);
	onoff = pController->bHeuristicsOn;
	VK_SEM_Release(pController->ulSema);

	return onoff;
}

HUINT32 _CB_DASH_CTRL_GetCurDuration(void* pController)
{
	DASH_PRECONNECTOR_ITEM_t item;
	VK_MEM_Memset(&item, 0x0, sizeof(DASH_PRECONNECTOR_ITEM_t));
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	DASH_CTRL_GetCurrentBufferingItem(pController, &item);
	//PrintDebug("current buffering duration : %"PRId64"\n", item.ullDuration);
	return DASH_CTRL_ConvertNonScaledTimetoMs(pController, item.ullDuration)/1000;
}
HUINT32 _CB_DASH_CTRL_GetCurBitrate(void* pController)
{
	HUINT32 curbitrate = 0;
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);
	curbitrate = ((DASH_CONTROLLER_HANDLE_t)pController)->CurrentBandwidth;
	VK_SEM_Release(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);

	//PrintDebug("Current bitrate : %d\n", curbitrate);
	return curbitrate;
}
HBOOL _CB_DASH_CTRL_GetIsProtected(void* pController)
{
	UNUSED_PARAM(pController);
	return FALSE;
}
HBOOL _CB_DASH_CTRL_SetBandwidthStepDown(void* pController)
{
	DASH_BANDWIDTH_t current;
	DASH_BANDWIDTH_t nextstep;

	current = DASH_CTRL_GetCurrentBandwidth(pController);
	if(current == -1)
	{
		return FALSE;
	}

	nextstep = DASH_MPD_GetPrevBandwidth(
		((DASH_CONTROLLER_HANDLE_t)pController)->mpdparser,
		DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY,
		((DASH_CONTROLLER_HANDLE_t)pController)->CurrentLocale,
		((DASH_CONTROLLER_HANDLE_t)pController)->CurVideoConnectTime,
		((DASH_CONTROLLER_HANDLE_t)pController)->CurrentBandwidth
		);

	if(nextstep == 0)
	{
		nextstep = P_DASH_CTRL_GetMinBandwidth(pController, DASH_TIME_BASE_CONNECTING);
	}

	DASH_CTRL_SetCurrentBandwidth(pController, nextstep);
	//PrintDebug("CB_Bandwidth down(%d)\n\n", nextstep);
	/* DASH gets segment information from MPD based on current bandwidth */
	return TRUE;
}
HBOOL _CB_DASH_CTRL_SetBandwidthStepUp(void* pController)
{
#if 1
	DASH_BANDWIDTH_t current;
	DASH_BANDWIDTH_t nextstep;

	current = DASH_CTRL_GetCurrentBandwidth(pController);
	if(current == -1)
	{
		return FALSE;
	}

	nextstep = DASH_MPD_GetNextBandwidth(
		((DASH_CONTROLLER_HANDLE_t)pController)->mpdparser,
		DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY,
		((DASH_CONTROLLER_HANDLE_t)pController)->CurrentLocale,
		((DASH_CONTROLLER_HANDLE_t)pController)->CurVideoConnectTime,
		((DASH_CONTROLLER_HANDLE_t)pController)->CurrentBandwidth
		);
	if(nextstep == (DASH_BANDWIDTH_t)0x7FFFFFFF)
	{
		nextstep = P_DASH_CTRL_GetMaxBandwidth(pController, DASH_TIME_BASE_CONNECTING);
	}
	DASH_CTRL_SetCurrentBandwidth(pController, nextstep);
	//PrintDebug("CB_Bandwidth up(%d)\n\n", nextstep);
	/* DASH gets segment information from MPD based on current bandwidth */
#endif

	return TRUE;
}
HBOOL _CB_DASH_CTRL_SetManualBandwidth(void* pController, HUINT32 ulBitrate)
{
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);
	((DASH_CONTROLLER_HANDLE_t)pController)->CurrentBandwidth = ulBitrate;
	VK_SEM_Release(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);
	//PrintDebug("CB_Bandwidth set(%d)\n\n", ulBitrate);

	return TRUE;
}
HBOOL _CB_DASH_CTRL_SetLowestBandwidth(void* pController)
{
	DASH_BANDWIDTH_t bandwidth;
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);
	bandwidth = P_DASH_CTRL_GetMinBandwidth(pController, DASH_TIME_BASE_BUFFERING);
	((DASH_CONTROLLER_HANDLE_t)pController)->CurrentBandwidth = bandwidth;
	VK_SEM_Release(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);

	//PrintDebug("\n (%0x)Lowest bandwidth called (%d)\n",pController, bandwidth);
	return TRUE;
}
HBOOL _CB_DASH_CTRL_SetHighestBandwidth(void* pController)
{
	DASH_BANDWIDTH_t bandwidth;
	if(pController == NULL)
	{
		PrintError("%s(%d) Invalid pController handle\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	VK_SEM_Get(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);
	bandwidth = P_DASH_CTRL_GetMaxBandwidth(pController, DASH_TIME_BASE_BUFFERING);
	((DASH_CONTROLLER_HANDLE_t)pController)->CurrentBandwidth = bandwidth;
	VK_SEM_Release(((DASH_CONTROLLER_HANDLE_t)pController)->ulSema);

	//PrintDebug("\nHighest bandwidth called (%d) \n",bandwidth);
	return TRUE;
}




/****************************************************************
*                      Preconnector APIs
****************************************************************/

/**
 * @b Function @b Description <br>
 * Create preconnector structure <br>
 *
 * @return  #  preconnector handle <br>
 */
DASH_PRECONNECTOR_HANDLE_t DASH_PRECON_Create(void)
{
	DASH_PRECONNECTOR_t *pInstance;
	HINT8 sem_name[128]={0};
	HINT32 nRet = 0;

	PrintEnter;
	pInstance = VK_MEM_Alloc(sizeof(DASH_PRECONNECTOR_t));
	if(pInstance == NULL)
	{
		PrintError("%s(%d) Error : Not enough memory\n", __FUNCTION__, __LINE__);
		PrintExit;
		return NULL;
	}
	VK_MEM_Memset(pInstance, 0, sizeof(DASH_PRECONNECTOR_t));

	nRet = VK_SEM_Create((unsigned long*)&pInstance->ulSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if(nRet != VK_OK)
	{
		PrintError("%s(%d) Create Semaphore Fail\n\r",__FUNCTION__,__LINE__, nRet);
		VK_MEM_Free(pInstance);
		pInstance = NULL;
		return NULL;
	}

	PrintDebug("%s(%d) Preconnector has been allocated\n", __FUNCTION__, __LINE__);
	PrintExit;
	return (DASH_PRECONNECTOR_HANDLE_t)pInstance;
}

/**
 * @b Function @b Description <br>
 * Destroy preconnector structure
 *
 * @param[in] hPrecon	: preconnector handle
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_Destroy(DASH_PRECONNECTOR_HANDLE_t hPrecon)
{
	PrintEnter;
	UTIL_DLLNode_t* node = NULL;
	DASH_PRECONNECTOR_ITEM_t* item = NULL;
	HUINT32 cnt = 0;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Invalid preconnector handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	DASH_PRECON_Flush(hPrecon);
	VK_SEM_Get(hPrecon->ulSema);
	node = UTIL_DLLIST_GetHeadNode(&(hPrecon->Pool));
	while(node != NULL)
	{
		item = node->pData;
		if(item != NULL)
		{
			if(item->pucURL != NULL)
			{
				VK_MEM_Free(item->pucURL);
				item->pucURL = NULL;
			}
			if(item->pSegBuffer != NULL)
			{
				VK_MEM_Free(item->pSegBuffer);
				item->pSegBuffer = NULL;
			}
			VK_MEM_Free(item);
			item = NULL;
		}
		node = UTIL_DLLIST_NextNode(node);
		cnt++;
	}
	UTIL_DLLIST_Reset(&(hPrecon->Pool));
	VK_SEM_Release(hPrecon->ulSema);
	VK_SEM_Destroy(hPrecon->ulSema);
	VK_MEM_Free(hPrecon);
	hPrecon = NULL;
	PrintDebug("%s(%d) preconnector has been destroyed\n", __FUNCTION__, __LINE__);
	PrintExit;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Initialize pool in preconnector
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[in] nMaxElement : the number of pool element
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_InitPool(DASH_PRECONNECTOR_HANDLE_t hPrecon, HUINT32 nMaxElement)
{
	HUINT32 uidx;
	DASH_PRECONNECTOR_ITEM_t *prefared_item;
	DLL_ERRCODE nRet;
	PrintEnter;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	hPrecon->nMaxPoolElements = nMaxElement;

	for(uidx = 0; uidx < nMaxElement; uidx++)
	{
		prefared_item = VK_MEM_Alloc(sizeof(DASH_PRECONNECTOR_ITEM_t));
		if(prefared_item == NULL)
		{
			PrintError("%s(%d) Error : Not enough memory!!\n", __FUNCTION__, __LINE__);
			return DASH_ERR_ERROR;
		}
		VK_MEM_Memset(prefared_item, 0, sizeof(DASH_PRECONNECTOR_ITEM_t));
		prefared_item->nID = uidx;
		VK_SEM_Get(hPrecon->ulSema);
		nRet = UTIL_DLLIST_Append(&hPrecon->Pool, (void*)prefared_item);
		VK_SEM_Release(hPrecon->ulSema);
		if(nRet != DLL_ERR_OK)
		{
			PrintError("%s(%d) Error : Appending node was failed\n", __FUNCTION__, __LINE__);
			PrintExit;
			return DASH_ERR_ERROR;
		}
	}
	PrintDebug("%s(%d) Success to set up pool\n", __FUNCTION__, __LINE__);

	PrintExit;

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get pool element from pool
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[out] node : pool element
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_GetPool(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_POOL_t* node)
{
	DASH_PRECONNECTOR_POOL_t getNode;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(hPrecon->ulSema);
	if(UTIL_DLLIST_Size(&hPrecon->Pool) <= 0)
	{
		VK_SEM_Release(hPrecon->ulSema);
		return DASH_ERR_POOL_EMPTY;
	}

	getNode = UTIL_DLLIST_ExtractHead(&hPrecon->Pool);
	PrintDebug("%s(%d) Notconnectedlist size (%d)\n",__FUNCTION__, __LINE__, UTIL_DLLIST_Size(&hPrecon->NotConnectedList));
	VK_SEM_Release(hPrecon->ulSema);

	if(getNode == NULL)
	{
		PrintError("%s(%d) Error : List is empty\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	if(getNode -> pData == NULL)
	{
		PrintError("%s(%d) Error : Data in list is corrupt(null)\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	*node = getNode;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get the number of remaining pool element
 *
 * @param[in] hPrecon	: preconnector handle
 *
 * @return  #  the number of remaining pool element <br>
 */
HINT32 DASH_PRECON_GetCntPool(DASH_PRECONNECTOR_HANDLE_t hPrecon)
{
	HUINT32 nRet = 0;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(hPrecon->ulSema);
	nRet = UTIL_DLLIST_Size(&hPrecon->Pool);
	VK_SEM_Release(hPrecon->ulSema);

	return nRet;
}

/**
 * @b Function @b Description <br>
 * Get connected element from connected list <br>
 * this is dequeue method of not connected <br>
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[out] node : item
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_GetConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_CONNECTED_t* node)
{
	DASH_PRECONNECTOR_CONNECTED_t getNode;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(hPrecon->ulSema);
	getNode = UTIL_DLLIST_ExtractHead(&(hPrecon->ConnectedList));
	PrintDebug("%s(%d) Connectedlist size (%d)\n", __FUNCTION__, __LINE__, UTIL_DLLIST_Size(&hPrecon->ConnectedList));
	VK_SEM_Release(hPrecon->ulSema);

	if(getNode == NULL)
	{
		PrintError("%s(%d) Error : List is empty\n", __FUNCTION__, __LINE__);
		return DASH_ERR_LIST_EMPTY;
	}

	if(getNode -> pData == NULL)
	{
		PrintError("%s(%d) Error : Data in list is corrupt(null)\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	if(node == NULL)
	{
		PrintError("%s(%d) Error : Destination is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	*node = getNode;

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Free item and append the empty element to pool <br>
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[in] node : item
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_Free(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_CONNECTED_t node)
{
	DLL_ERRCODE nRet;
	DASH_PRECONNECTOR_ITEM_t* item = NULL;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	if(node == NULL)
	{
		PrintError("%s(%d) Error : Node is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	//Release resource
	VK_SEM_Get(hPrecon->ulSema);
	item = node->pData;
	if(item->pucURL != NULL)
	{
		VK_MEM_Free(item->pucURL);
		item->pucURL = NULL;
	}
	if(item->pSegBuffer != NULL)
	{
		VK_MEM_Free(item->pSegBuffer);
		item->pSegBuffer = NULL;
	}
	VK_MEM_Memset(item, 0x0, sizeof(DASH_PRECONNECTOR_ITEM_t));

	//Return node to pool
	nRet = UTIL_DLLIST_AppendNode(&hPrecon->Pool, node);
	VK_SEM_Release(hPrecon->ulSema);

	if(nRet != DLL_ERR_OK)
	{
		PrintError("%s(%d) Error : Appending node was failed\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	PrintDebug("%s(%d) POOL size (%d)\n", __FUNCTION__, __LINE__, UTIL_DLLIST_Size(&hPrecon->Pool));

	return DASH_ERR_OK;

}

/**
 * @b Function @b Description <br>
 * Flush preconnector lists <br>
 *
 * @param[in] hPrecon	: preconnector handle
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_Flush(DASH_PRECONNECTOR_HANDLE_t hPrecon)
{
	HUINT32 lsize;
	HUINT32 lidx;
	UTIL_DLLNode_t* node = NULL;
	DASH_ERROR_e nRet;

	lsize = UTIL_DLLIST_Size(&hPrecon->ConnectedList);
	for(lidx = 0; lidx < lsize; lidx++)
	{
		VK_SEM_Get(hPrecon->ulSema);
		node = UTIL_DLLIST_ExtractHead(&(hPrecon->ConnectedList));
		VK_SEM_Release(hPrecon->ulSema);
		if(node != NULL)
		{
			nRet = DASH_PRECON_Free(hPrecon, node);
		}
		if(nRet != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error : list  free was failed\n", __FUNCTION__, __LINE__);
			return DASH_ERR_ERROR;
		}
	}

	lsize = UTIL_DLLIST_Size(&hPrecon->NotConnectedList);
	for(lidx = 0; lidx < lsize; lidx++)
	{
		VK_SEM_Get(hPrecon->ulSema);
		node = UTIL_DLLIST_ExtractHead(&(hPrecon->NotConnectedList));
		VK_SEM_Release(hPrecon->ulSema);
		if(node != NULL)
		{
			nRet = DASH_PRECON_Free(hPrecon, node);
		}
		if(nRet != DASH_ERR_OK)
		{
			PrintError("%s(%d) Error : list free was failed\n", __FUNCTION__, __LINE__);
			return DASH_ERR_ERROR;
		}
	}

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get the number of connected element <br>
 *
 * @param[in] hPrecon	: preconnector handle
 *
 * @return  #  the number of connected element <br>
 */
HINT32 DASH_PRECON_GetCntConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon)
{
	HUINT32 nRet = 0;
	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return -1;
	}

	VK_SEM_Get(hPrecon->ulSema);
	nRet = UTIL_DLLIST_Size(&hPrecon->ConnectedList);
	VK_SEM_Release(hPrecon->ulSema);

	return nRet;
}

/**
 * @b Function @b Description <br>
 * Insert item to not connected list <br>
 * this is enqueue method <br>
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[in] pItem : item
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_Insert(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_ITEM_t* pItem)
{
	DLL_ERRCODE nRet;
	DASH_PRECONNECTOR_POOL_t node = NULL;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(hPrecon->ulSema);
	if( UTIL_DLLIST_Size(&(hPrecon->Pool)) <= 0 )
	{
		VK_SEM_Release(hPrecon->ulSema);
		return DASH_ERR_POOL_EMPTY;
	}
	node = UTIL_DLLIST_ExtractHead(&(hPrecon->Pool));
	if(node == NULL)
	{
		PrintError("%s(%d) Error : List is empty\n", __FUNCTION__, __LINE__);
		VK_SEM_Release(hPrecon->ulSema);
		return DASH_ERR_ERROR;
	}
	if(node -> pData == NULL)
	{
		PrintError("%s(%d) Error : Data in list is corrupt(null)\n", __FUNCTION__, __LINE__);
		VK_SEM_Release(hPrecon->ulSema);
		return DASH_ERR_ERROR;
	}
	/* pItem->pucURL is allocated in Scheduler and destroyed in Free */
	VK_MEM_Memcpy(node->pData, pItem, sizeof(DASH_PRECONNECTOR_ITEM_t));
	nRet = UTIL_DLLIST_AppendNode(&hPrecon->NotConnectedList, node);

	//nRet = UTIL_DLLIST_Append(&hPrecon->NotConnectedList, pItem);
	if(nRet == DLL_ERR_FAIL)
	{
		PrintError("%s(%d) Error : Appending node was failed\n", __FUNCTION__, __LINE__);
		VK_SEM_Release(hPrecon->ulSema);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Release(hPrecon->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get the number of not connected element <br>
 *
 * @param[in] hPrecon	: preconnector handle
 *
 * @return  #  the number of not connected element <br>
 */
HINT32 DASH_PRECON_GetCntNotConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon)
{
	HUINT32 nRet = 0;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		PrintExit;
		return -1;
	}

	VK_SEM_Get(hPrecon->ulSema);
	nRet = UTIL_DLLIST_Size(&hPrecon->NotConnectedList);
	VK_SEM_Release(hPrecon->ulSema);

	return nRet;
}

/**
 * @b Function @b Description <br>
 * Get the element from not connected list <br>
 * this is dequeue method of not connected <br>
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[out] node : not connected element
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_GetNotConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_NOTCONNECTED_t* node)
{
	DASH_PRECONNECTOR_POOL_t getNode;

	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(hPrecon->ulSema);
	getNode = UTIL_DLLIST_ExtractHead(&hPrecon->NotConnectedList);
	PrintDebug("%s(%d) Notconnectedlist size (%d)\n",__FUNCTION__, __LINE__, UTIL_DLLIST_Size(&hPrecon->NotConnectedList));
	VK_SEM_Release(hPrecon->ulSema);

	if(getNode == NULL)
	{
		PrintError("%s(%d) Error : List is empty\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	if(getNode -> pData == NULL)
	{
		PrintError("%s(%d) Error : Data in list is corrupt(null)\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	*node = getNode;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Move the element to not connected list <br>
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[out] node : element
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_MovetoNotConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_POOL_t node)
{
	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	if(node == NULL)
	{
		PrintError("%s(%d) Error : Node is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(hPrecon->ulSema);
	UTIL_DLLIST_AppendNode(&hPrecon->NotConnectedList, node);
	VK_SEM_Release(hPrecon->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Move the element to connected list <br>
 *
 * @param[in] hPrecon	: preconnector handle
 * @param[out] node : element
 *
 * @return  #  Error code <br>
 */
DASH_ERROR_e DASH_PRECON_MovetoConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_NOTCONNECTED_t node)
{
	if(hPrecon == NULL)
	{
		PrintError("%s(%d) Error : Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	if(node == NULL)
	{
		PrintError("%s(%d) Error : Node is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(hPrecon->ulSema);
	UTIL_DLLIST_AppendNode(&hPrecon->ConnectedList, node);
	VK_SEM_Release(hPrecon->ulSema);

	return DASH_ERR_OK;
}

