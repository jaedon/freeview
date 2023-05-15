/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tapi.h
	@brief

	Description:  									\n
	Module: TiMa Control Client API					\n

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
#include	<vkernel.h>
#include	<hlib.h>

#include	<tapi.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define VALID_UNIXTIME					(60 * 60 * 24 * 366)			// until 1971/01/02

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32		ulSemId;
	HINT32		nClientHandle;
	HINT32		nRPCHandle;

	TxSOURCE_e	eSource;
} TAPI_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static TAPI_t*	tapi_getInstance(void);
static void		tapi_releaseInstance(TAPI_t *tapi);

static HERROR	tapi_onSourceChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	TAPI_t	*tapi;

	HxLOG_Assert(nArgc == 1);

	tapi = tapi_getInstance();

	HxLOG_Debug("Time source is changed : [%d --> %d]\n", tapi->eSource, HxOBJECT_INT(apArgv[0]));
	tapi->eSource = HxOBJECT_INT(apArgv[0]);

	tapi_releaseInstance(tapi);

	return ERR_OK;
}

static void		tapi_event_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	TAPI_t	*tapi = (TAPI_t*)userData;

	HLIB_RPC_ProcessRPC(tapi->nRPCHandle, size, pvData, requestId, nSocketHandle);
}

static void		tapi_event_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	(void)userData;
	(void)eStatus;
	(void)nClientHandle;
}

static void		tapi_event_listener(void *arg)
{
	TAPI_t	*tapi = (TAPI_t*)arg;

	HLIB_SOCKET_Listen(tapi->nClientHandle, TRUE, tapi_event_onPacketReceived, tapi_event_onStatusChanged, arg);
}

static TAPI_t*	tapi_getInstance(void)
{
	static TAPI_t	*s_tapi = NULL;

	if (s_tapi == NULL)
	{
		unsigned int	tid;

		s_tapi = (TAPI_t*)HLIB_STD_MemCalloc(sizeof(TAPI_t));
		HxSEMT_Create(&s_tapi->ulSemId, "tapisem", 0);
		HxSEMT_Get(s_tapi->ulSemId);
		s_tapi->nClientHandle = HLIB_EXTRA_OpenClient("tima");
		s_tapi->nRPCHandle    = HLIB_RPC_OpenEx(s_tapi->nClientHandle, HLIB_STD_GetPID(), TRUE);
		s_tapi->eSource       = TxSOURCE_SET_BY_UNKNOWN;

		VK_TASK_Create(tapi_event_listener, 70, SIZE_64K, "tapi_event_listener", (void*)s_tapi, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);

		HLIB_RPC_RegisterNotifyCallback(s_tapi->nRPCHandle, "tima:onSourceChanged", "i", (void*)s_tapi, tapi_onSourceChanged);
	}
	else
	{
		HxSEMT_Get(s_tapi->ulSemId);
	}

	return s_tapi;
}

static void		tapi_releaseInstance(TAPI_t *tapi)
{
	HxSEMT_Release(tapi->ulSemId);
}

HERROR	TAPI_Init(void)
{
	tapi_releaseInstance(tapi_getInstance());

	return ERR_OK;
}

HERROR	TAPI_SetTime(TxSOURCE_e eSource, HUINT32 ulUnixTime)
{
	TAPI_t	*tapi;

	if (ulUnixTime < VALID_UNIXTIME)
		return ERR_FAIL;

	tapi = tapi_getInstance();

	HLIB_RPC_Call(tapi->nRPCHandle, "tima.settime", "ii", eSource, ulUnixTime);

	tapi_releaseInstance(tapi);

	return ERR_OK;
}

HERROR	TAPI_SetNTP(HCHAR *usURL)
{
	TAPI_t	*tapi;
	HxLOG_Critical("");
	if(!usURL)
		return ERR_FAIL;
	
	tapi = tapi_getInstance();

	HLIB_RPC_Call(tapi->nRPCHandle, "tima.setNTP", "s", usURL);

	tapi_releaseInstance(tapi);

	return ERR_OK;
}

HERROR	TAPI_GetTimeSource(TxSOURCE_e *peSource)
{
	TAPI_t	*tapi;

	tapi = tapi_getInstance();

	*peSource = tapi->eSource;

	tapi_releaseInstance(tapi);

	return ERR_OK;
}

