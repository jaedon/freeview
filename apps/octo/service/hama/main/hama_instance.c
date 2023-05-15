/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama_instance.c
	@brief

	Description:  									\n
	Module: HaMa Instance manager					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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

#include	<directfb.h>
#include	<sawman.h>
#include	<sys/wait.h>

#include	"hama_int.h"

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
static HAMA_t	*hama_getInstance(void);
static void		hama_releaseInstance(HAMA_t *pstHama);
static HERROR	hama_addEvent(HAMA_t *pstHama, HUINT32 ulEventId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4);

#if 0
static void	hama_process_child_handler(int signum, siginfo_t *siginfo, void *context)
{
	int		st;
	pid_t	pid;

	HAMA_t	*pstHama;

	while ((pid = waitpid(-1, &st, WNOHANG)) > 0)
	{
		pstHama = HAMA_GetInstance();

		HAMA_SCENARIO_ProcessStoppedSignal(pid, st);
		HAMA_AddEvent(pstHama, eHSIGSYS_APP_EXITED, 0, 0, 0, 0);
		HAMA_ReleaseInstance(pstHama);
	}
}
#endif

static void	hama_system_InitializeSignal(void)
{
#if 0
	static struct	sigaction	action;

	action.sa_sigaction = hama_process_child_handler;
	action.sa_flags = SA_SIGINFO;
	sigfillset(&(action.sa_mask));
	sigaction(SIGCHLD, &action, NULL);
#endif
}

static void		hama_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	HAMA_t	*pstHama = (HAMA_t*)userData;

	HLIB_RPC_ProcessRPC(pstHama->nRPCHandle, size, pvData, requestId, nSocketHandle);

}

static void		hama_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	HAMA_t	*pstHama = (HAMA_t*)userData;

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
	{
		pstHama = hama_getInstance();

		HLIB_RPC_RemoveClientHandle(pstHama->nRPCHandle, nClientHandle);
		hama_addEvent(pstHama, eHSIGSYS_INTERNAL_SOCKETREMOVE, nClientHandle, 0, 0, 0);

		hama_releaseInstance(pstHama);
	}
}

static void		hama_instance_EventListener(void *arg)
{
	HAMA_t	*pstHama = (HAMA_t*)arg;

	HLIB_SOCKET_Listen(pstHama->nServerHandle, TRUE, hama_onPacketReceived, hama_onStatusChanged, arg);
}

static HINT32	hama_event_CompareEvent(void *pvData1, void *pvData2)
{
	return 0;
}

static HERROR	hama_addEvent(HAMA_t *pstHama, HUINT32 ulEventId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4)
{
	HamaMsg_t	stHamaMsg;

	stHamaMsg.ulMsg = ulEventId;
	stHamaMsg.ulParam1 = ulParam1;
	stHamaMsg.ulParam2 = ulParam2;
	stHamaMsg.ulParam3 = ulParam3;
	stHamaMsg.ulParam4 = ulParam4;

	pstHama->pstEventList = HLIB_LIST_AppendSortEx(pstHama->pstEventList, HAMA_MemDup(&stHamaMsg, sizeof(HamaMsg_t)), hama_event_CompareEvent);

	HAMA_TOOL_SendWakeup(pstHama);

	return ERR_OK;
}

static HERROR	hama_getEvent(HAMA_t *pstHama, HamaMsg_t *pstMsg)
{
	HamaMsg_t	*pstMessage;

	pstMessage = (HamaMsg_t*)HLIB_LIST_Data(pstHama->pstEventList);
	if (pstMessage == NULL)
		return ERR_FAIL;

	if (pstMsg)
	{
		HxSTD_MemCopy(pstMsg, pstMessage, sizeof(HamaMsg_t));
		pstHama->pstEventList = HLIB_LIST_Remove(pstHama->pstEventList, (void*)pstMessage);
		HAMA_Free(pstMessage);

		return ERR_OK;
	}

	return ERR_FAIL;
}

static HAMA_t	*s_pstHama = NULL;

static HAMA_t	*hama_getInstance(void)
{
	if (s_pstHama->bInit == FALSE)
	{
		unsigned long	tid;
#if 0
		s_pstHama = (HAMA_t*)HAMA_Calloc(sizeof(HAMA_t));
		hama_system_InitializeSignal();
#endif
		//	Register Launch scenario from HAMA_SCENARIO_FILE
//		HAMA_SCENARIO_Init("/etc/octo_system.ini");

		HxSEMT_Create(&s_pstHama->ulSemId, "hamasem", HxSEMT_FIFO);
		HxSEMT_Get(s_pstHama->ulSemId);
		s_pstHama->bInit = TRUE;
		s_pstHama->nServerHandle = HLIB_EXTRA_OpenServer("hama");//HLIB_SOCKET_OpenServer(HxSOCKET_UDS, HAMA_SIGNAL_UDS);
		s_pstHama->nRPCHandle    = HLIB_RPC_OpenEx(s_pstHama->nServerHandle, HLIB_STD_GetPID(), TRUE);
		s_pstHama->nCBuf         = HLIB_CBUF_New(0x4000);

		s_pstHama->pstBufferedSignal = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, HAMA_Free_CB);

#ifdef	CONFIG_DEBUG
		s_pstHama->stPool.pstObj = HLIB_SERIALIZER_OpenUnpackCache();
		s_pstHama->stPool.pstJob = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)HAMA_Free_CB);
#endif

		HAMA_RPC_Register(s_pstHama);

		//	HAMA MainTask의 Wakeup을 위한 Event.
		HLIB_MSG_Create(128, sizeof(HAMA_EVT_t), "hamaEvt", (unsigned long*)&s_pstHama->ulHamaEvtQId, eHLIB_SUSPENDTYPE_FIFO);

		VK_TASK_Create(hama_instance_EventListener, 70, SIZE_64K, "hamaEventListener", (void*)s_pstHama, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);

		VK_TASK_Create(HAMA_HANDLER_MainLoop, 70, SIZE_64K, "hamaMainHandler", (void*)s_pstHama, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
	}
	else
	{
		HxSEMT_Get(s_pstHama->ulSemId);
	}

	return s_pstHama;
}

static void		hama_releaseInstance(HAMA_t *pstHama)
{
	HxSEMT_Release(pstHama->ulSemId);
}

#define	____HAMA_INSTANCE_MAIN_____
HAMA_t*		HAMA_InitInstance(void)
{
	s_pstHama = (HAMA_t*)HAMA_Calloc(sizeof(HAMA_t));
	hama_system_InitializeSignal();
	s_pstHama->bInit = FALSE;

	return s_pstHama;
}
HERROR		HAMA_AddEvent(HAMA_t *pstHama, HUINT32 ulEventId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4)
{
	return hama_addEvent(pstHama, ulEventId, ulParam1, ulParam2, ulParam3, ulParam4);
}

HERROR		HAMA_GetEvent(HAMA_t *pstHama, HamaMsg_t *pstMsg)
{
	return hama_getEvent(pstHama, pstMsg);
}

HAMA_t*		HAMA_GetInstance(void)
{
	return hama_getInstance();
}

void		HAMA_ReleaseInstance(HAMA_t *pstHama)
{
	hama_releaseInstance(pstHama);
}


