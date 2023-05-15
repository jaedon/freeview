/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tima.c
	@brief

	Description:  									\n
	Module: TiMa Main 								\n

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
#include	"tima_int.h"
#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static TIMA_t	*tima_getInstance(void);
static void	tima_releaseInstance(TIMA_t *pstTima);

static void		tima_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	TIMA_t	*pstTima = (TIMA_t*)userData;

	HLIB_RPC_ProcessRPC(pstTima->nRPCHandle, size, pvData, requestId, nSocketHandle);

}

static void		tima_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	TIMA_t	*pstTima = (TIMA_t*)userData;

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
	{
		pstTima = tima_getInstance();

		HLIB_RPC_RemoveClientHandle(pstTima->nRPCHandle, nClientHandle);
		tima_releaseInstance(pstTima);
	}
}

static void		tima_instance_EventListener(void *arg)
{
	TIMA_t	*pstTima = (TIMA_t*)arg;
	HLIB_SOCKET_Listen(pstTima->nServerHandle, TRUE, tima_onPacketReceived, tima_onStatusChanged, arg);
}

static TIMA_t	*tima_getInstance(void)
{
	static TIMA_t	*s_pstTima = NULL;

	if (s_pstTima == NULL)
	{
		unsigned long	tid;

		s_pstTima = (TIMA_t*)HLIB_STD_MemCalloc(sizeof(TIMA_t));

		HxSEMT_Create(&s_pstTima->ulSemId, "timasem", HxSEMT_FIFO);

		HxSEMT_Get(s_pstTima->ulSemId);
		s_pstTima->nServerHandle = HLIB_EXTRA_OpenServer("tima");
		s_pstTima->nRPCHandle    = HLIB_RPC_OpenEx(s_pstTima->nServerHandle, HLIB_STD_GetPID(), TRUE);

		VK_TASK_Create(tima_instance_EventListener, 70, SIZE_64K, "timaEventListener", (void*)s_pstTima, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
	}
	else
	{
		HxSEMT_Get(s_pstTima->ulSemId);
	}

	return s_pstTima;
}

static void	tima_releaseInstance(TIMA_t *pstTima)
{
	HxSEMT_Release(pstTima->ulSemId);
}

static HERROR	tima_rpc_SetNTP(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Debug("nArgc [%d] [%s]\n", nArgc, HxOBJECT_STR(apArgv[0]));
	HxLOG_Assert(nArgc == 1);
	TIMA_CreateNtp(HxOBJECT_STR(apArgv[0]));
	return ERR_OK;
}

//	i : source
//	i : unixtime
static HERROR	tima_rpc_SetTime(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	TIMA_t	*pstTima;
	HINT32	nSource;
	HUINT32	ulUnixTime;

	HxLOG_Debug("nArgc [%d]\n", nArgc);

	HxLOG_Assert(nArgc == 2);

	pstTima = tima_getInstance();

	nSource    = HxOBJECT_INT(apArgv[0]);
	ulUnixTime = (HUINT32)HxOBJECT_INT(apArgv[1]);
#ifdef	CONFIG_DEBUG
	{
		HxDATETIME_t	dateTime;

		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixTime, &dateTime);
		HxLOG_Debug("RPC setTime : [%d - %d] - [%d-%d-%d] [%d:%d:%d]\n", nSource, ulUnixTime, dateTime.stDate.usYear, dateTime.stDate.ucMonth, dateTime.stDate.ucDay,
			dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond);
	}
#endif
	HxLOG_Debug("SETAPI_SetTime : [%d]\n", ulUnixTime);
	pstTima->nSource = nSource;
	SETAPI_SetTime(ulUnixTime);

	HLIB_RPC_Notify(pstTima->nRPCHandle, "tima:onSourceChanged", "i", nSource);

	tima_releaseInstance(pstTima);

	return ERR_OK;
}

static int tima_cmd_startNtpClient(void *szArgs)
{
	HINT8	*szArg = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg == NULL )
	{
		return HxCMD_ERR;
	}

	if(TIMA_CreateNtp(szArg) != ERR_OK)
	{
		TIMA_t	*pstTima;
		pstTima = tima_getInstance();
		HLIB_CMD_Printf(" FAIL URL[%16s] client already started[%d]\n", pstTima->usNtpUrl, pstTima->ulNtpThreadId);
		tima_releaseInstance(pstTima);
	}
	else
	{
		HLIB_CMD_Printf(" Success URL[%16s] client maybe get the time info from server\n", szArg);
	}
	return ERR_OK;
}

void	tima_cmd_Init(void)
{
#define	cmdHandle		"timacmd"
	HLIB_CMD_RegisterWord(cmdHandle, tima_cmd_startNtpClient,
					 (HINT8 *)"startntp",
					 (HINT8 *)"tima ntp client start",
					 (HINT8 *)"ntpserver url");
}

TIMA_t	*Tima_GetInstance(void)
{
	return tima_getInstance();
}

void	Tima_ReleaseInstance(TIMA_t *pstTima)
{
	tima_releaseInstance(pstTima);
}

HINT32	tima_main(HINT32 argc, char **argv)
{
	TIMA_t	*pstTima;

	VK_Init();

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif
	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("tima");

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif

	pstTima = tima_getInstance();

	HLIB_RPC_RegisterCall(pstTima->nRPCHandle, "tima.settime", "ii", tima_rpc_SetTime, NULL, "tima.settime(int source, int unixtime)");
	HLIB_RPC_RegisterCall(pstTima->nRPCHandle, "tima.setNTP", "s", tima_rpc_SetNTP, NULL, "tima.settime(hchar * url)");
	HLIB_RPC_CreateNotification(pstTima->nRPCHandle, "tima:onSourceChanged", "i", "notify time source is changed.");
	tima_releaseInstance(pstTima);

#ifdef	CONFIG_DEBUG
	tima_cmd_Init();
#endif
	return 0;
}

#if defined(CONFIG_PROD_OS_EMULATOR)
#define TIMA_main	TIMA_EMU_Main
#else
#define TIMA_main	main
#endif

HINT32	TIMA_main(HINT32 argc, char ** argv)
{
	tima_main(argc, argv);

#if defined(CONFIG_PROD_OS_EMULATOR)
	VK_TASK_Sleep(100);
#else
	while (1)
		VK_TASK_Sleep(100000);
#endif
	return 0;
}

