/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   dash_main.c  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	Dash main module
 * Module:
 * Remarks:
 */

/**
 * @defgroup DASH_MAIN
 * @brief This manages handles for DASH data structures using master handle, and provide user event and basic controller interface
 */

/**
 * @author
 * @date 30 Jul 2012
 */

/**
 * @brief This manages handles for DASH data structures using master handle, and provide user event and basic controller interface
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
 * @file dash_main.c
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
#include "dash_main.h"
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
//#define DASH_MAIN_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DASH_MAIN_DEBUG
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


/****************************************************************
*						Maindata APIs
****************************************************************/

/**
 * @b Function @b Description <br>
 * Create master handle for DASH<br>
 *
 * @return	#  DASH master handle <br>
 */
DASH_MASTER_HANDLE_t DASH_MAIN_Create()
{
	HINT8 sem_name[128] = {0};
	HINT32 nRet = 0;

	PrintEnter;
	DASH_ESSENTIAL_t *pInstance = VK_MEM_Alloc(sizeof(DASH_ESSENTIAL_t));
	if(pInstance == NULL)
	{
		PrintError("%s(%d) Error : Not enough memory\n", __FUNCTION__, __LINE__);
		PrintExit;
		return NULL;
	}

	VK_MEM_Memset(pInstance, 0, sizeof(DASH_ESSENTIAL_t));
	nRet = VK_SEM_Create((unsigned long*)&pInstance->ulSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if(nRet != VK_OK)
	{
		PrintError("%s(%d) Create Semaphore Fail\n\r",__FUNCTION__,__LINE__, nRet);
		VK_MEM_Free(pInstance);
		pInstance = NULL;
		return NULL;
	}

	PrintDebug("%s(%d) Essential data has been allocated\n", __FUNCTION__, __LINE__);
	PrintExit;
	return (DASH_MASTER_HANDLE_t)pInstance;
}

/**
 * @b Function @b Description <br>
 * Destroy dash master handle <br>
 *
 * @param[in] pDASH : DASH master handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_Destroy(DASH_MASTER_HANDLE_t pDASH)
{
	PrintEnter;
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	VK_SEM_Destroy(pDASH->ulSema);
	VK_MEM_Free(pDASH);
	pDASH = NULL;
	PrintDebug("%s(%d) Essential data has been destroyed\n", __FUNCTION__, __LINE__);
	PrintExit;
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set heuristics handle to master handle <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] pHeuristics : Heuristics handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_SetHeuristicsHandle(DASH_MASTER_HANDLE_t pDASH, DASH_HEURISTICS_HANDLE_t pHeuristics)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Error : Heuristics Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	pDASH->handle.hHeuristics = pHeuristics;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set buffer manager handle to master handle <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] pBufferMgr : Buffer manager handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_SetBufferMgrHandle(DASH_MASTER_HANDLE_t pDASH, DASH_BUFFERMGR_HANDLE_t pBufferMgr)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error : Controller Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	pDASH->handle.hBuffermgr = pBufferMgr;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set controller handle to master handle <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] pController : DASH controller handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_SetControllerHandle(DASH_MASTER_HANDLE_t pDASH, DASH_CONTROLLER_HANDLE_t pController)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Controller Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	pDASH->handle.hController = pController;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set preconnector handle to master handle <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] pHeuristics : Preconnector handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_SetPreconnectorHandle(DASH_MASTER_HANDLE_t pDASH, DASH_PRECONNECTOR_HANDLE_t pPreconnector)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pPreconnector == NULL)
	{
		PrintError("%s(%d) Error : Preconnector Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(pDASH->ulSema);
	pDASH->handle.hPreconnector = pPreconnector;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set MPD parser handle to mster handle <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] pMPD : MPD parser handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_SetMPDParserHandle(DASH_MASTER_HANDLE_t pDASH, DASH_MPDPARSER_HANDLE_t pMPD)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pMPD == NULL)
	{
		PrintError("%s(%d) Error : Controller Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}

	VK_SEM_Get(pDASH->ulSema);
	pDASH->handle.hMPDParser = pMPD;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get handle for heuristics <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[out] pHeuristics : Heuristics handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_GetHeuristicsHandle(DASH_MASTER_HANDLE_t pDASH, DASH_HEURISTICS_HANDLE_t *pHeuristics)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pHeuristics == NULL)
	{
		PrintError("%s(%d) Error : Huristics Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	*pHeuristics = pDASH->handle.hHeuristics;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get handle for buffer manager <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[out] pBufferMgr : Buffer manager handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_GetBufferMgrHandle(DASH_MASTER_HANDLE_t pDASH, DASH_BUFFERMGR_HANDLE_t *pBufferMgr)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error : Controller Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	*pBufferMgr = pDASH->handle.hBuffermgr;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get handle for controller <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[out] pController : DASH controller handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_GetControllerHandle(DASH_MASTER_HANDLE_t pDASH, DASH_CONTROLLER_HANDLE_t* pController)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pController == NULL)
	{
		PrintError("%s(%d) Error : Controller Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	*pController = pDASH->handle.hController;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;

}

/**
 * @b Function @b Description <br>
 * Get handle for preconnector <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[out] pPreconnector : Preconnector handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_GetPreconnectorHandle(DASH_MASTER_HANDLE_t pDASH, DASH_PRECONNECTOR_HANDLE_t* pPreconnector)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pPreconnector == NULL)
	{
		PrintError("%s(%d) Error : Preconnector Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	*pPreconnector = pDASH->handle.hPreconnector;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get handle for MPD parser <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[out] pMPD : MPD parser handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_GetMPDParserHandle(DASH_MASTER_HANDLE_t pDASH, DASH_MPDPARSER_HANDLE_t* pMPD)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Error : Essential Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	if(pMPD == NULL)
	{
		PrintError("%s(%d) Error : Controller Handle is null\n", __FUNCTION__, __LINE__);
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	*pMPD = pDASH->handle.hMPDParser;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Set user event flag <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] eUserEvent : User event
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_SetOccuredUserEvent(DASH_MASTER_HANDLE_t pDASH, DASH_USER_EVENT_e eUserEvent)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	pDASH->eUserAction = eUserEvent;
	pDASH->eUserActionForRead = eUserEvent;
	VK_SEM_Release(pDASH->ulSema);
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Clear user event flag <br>
 *
 * @param[in] pDASH : DASH master handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_ClearUserEvent(DASH_MASTER_HANDLE_t pDASH)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	pDASH->eUserAction = DASH_USER_EVENT_NONE;
	VK_SEM_Release(pDASH->ulSema);
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Clear user event flag that is for blocking waiting IO in VFIO read <br>
 *
 * @param[in] pDASH : DASH master handle
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_ClearUserEventForRead(DASH_MASTER_HANDLE_t pDASH)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	pDASH->eUserActionForRead = DASH_USER_EVENT_NONE;
	VK_SEM_Release(pDASH->ulSema);
	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get user event flag <br>
 *
 * @param[in] pDASH : DASH master handle
 *
 * @return	#  User event <br>
 */
DASH_USER_EVENT_e DASH_MAIN_GetOccuredUserEvent(DASH_MASTER_HANDLE_t pDASH)
{
	DASH_USER_EVENT_e event;

	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	event = pDASH->eUserAction;

	return event;
}

/**
 * @b Function @b Description <br>
 * Get user event flag that is for blocking waiting IO in VFIO read <br>
 *
 * @param[in] pDASH : DASH master handle
 *
 * @return	#  User event <br>
 */
DASH_USER_EVENT_e DASH_MAIN_GetOccuredUserEventForRead(DASH_MASTER_HANDLE_t pDASH)
{
	DASH_USER_EVENT_e event;

	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	event = pDASH->eUserActionForRead;

	return event;
}

/**
 * @b Function @b Description <br>
 * Set seek position (in ms) <br>
 *
 * @param[in] pDASH : DASH master handle
 * @param[in] ullPosition : Seek position in ms
 *
 * @return	#  Error code <br>
 */
DASH_ERROR_e DASH_MAIN_SetSeekPosition(DASH_MASTER_HANDLE_t pDASH, HINT64 ullPosition)
{
	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}

	if(ullPosition < 0)
	{
		PrintError("%s(%d) Invalid Position value\n", __FUNCTION__, __LINE__);
		PrintExit;
		return DASH_ERR_ERROR;
	}
	VK_SEM_Get(pDASH->ulSema);
	pDASH->SeekPos = ullPosition;
	VK_SEM_Release(pDASH->ulSema);

	return DASH_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Get seek position (in ms) <br>
 *
 * @param[in] pDASH : DASH master handle
 *
 * @return	#  seek position in ms <br>
 */
HINT64 DASH_MAIN_GetSeekPosition(DASH_MASTER_HANDLE_t pDASH)
{
	HINT64 pos;

	if(pDASH == NULL)
	{
		PrintError("%s(%d) Invalid essential handle\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_SEM_Get(pDASH->ulSema);
	pos = pDASH->SeekPos;
	VK_SEM_Release(pDASH->ulSema);

	return pos;
}

