/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  se_tapi.c
	@brief

	Description:  									\n
	Module: TiMa Secure Handler API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>

#include	<vkernel.h>
#include	<hlib.h>

#include	<se_tapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32	nClientHandle;
	HINT32	nRPCHandle;
} SETAPI_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void		setapi_event_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	SETAPI_t	*setapi = (SETAPI_t*)userData;

	HLIB_RPC_ProcessRPC(setapi->nRPCHandle, size, pvData, requestId, nSocketHandle);
}

static void		setapi_event_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	(void)userData;
	(void)eStatus;
	(void)nClientHandle;
}

static void		setapi_event_listener(void *arg)
{
	SETAPI_t	*setapi = (SETAPI_t*)arg;

	HLIB_SOCKET_Listen(setapi->nClientHandle, TRUE, setapi_event_onPacketReceived, setapi_event_onStatusChanged, arg);
}

static SETAPI_t*	setapi_getInstance(void)
{
	static SETAPI_t	*s_setapi = NULL;

	if (s_setapi == NULL)
	{
		unsigned int	tid;

		s_setapi = (SETAPI_t*)HLIB_STD_MemCalloc(sizeof(SETAPI_t));
		s_setapi->nClientHandle = HLIB_SOCKET_OpenClientEx(HxSOCKET_UDS, NULL, (void*)SE_TIMA_SERVER, 0);
		s_setapi->nRPCHandle    = HLIB_RPC_OpenEx(s_setapi->nClientHandle, HLIB_STD_GetPID(), TRUE);

		VK_TASK_Create(setapi_event_listener, 70, SIZE_64K, "setapi_event_listener", (void*)s_setapi, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
	}

	return s_setapi;
}

static void	setapi_releaseInstance(SETAPI_t *setapi)
{
	(void)(setapi);
}

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
HERROR	SETAPI_SetTime(HUINT32 ulUnixTime)
{
#ifdef	CONFIG_ENHANCED_SECURITY
	SETAPI_t	*setapi = setapi_getInstance();

	HLIB_RPC_Call(setapi->nRPCHandle, "setima.settime", "i", ulUnixTime);

	setapi_releaseInstance(setapi);
#else
	stime ((time_t *)&ulUnixTime);
#endif

	return ERR_OK;
}

