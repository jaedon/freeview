/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_hotplug.c
	@brief

	Description:  									\n
	Module: UMMA File System				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding \uc744 \uc0ac\uc6a9\ud558\ub294 source file \uc785\ub2c8\ub2e4.
* MS949 encoding\u3092\u4f7f\u7528\u3057\u3066 source file\u3067\u3059\u3002
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
#include <hlib.h>
#include <dlib.h>
#include <uapi.h>
#include <oapi.h>
#include <hapi.h>
#include <thapi.h>
#include <papi.h>
#include	<vkernel.h>

#include <umma_int.h>

#include "umma_hotplug_int.h"

#include "se_uapi.h"
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
/********************      Static Variables     ********************/
/*******************************************************************/
static HxList_t		*s_pstHotplugList = NULL;
static UMMA_t		*s_pstUmmaHp = NULL;
static HULONG		s_ulUmmaHpMsgQId;

static HBOOL	s_bJobRunning=FALSE;
static HBOOL	s_bDevPower=FALSE;


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

static void		umma_hotplug_mainLoop(void *arg);

#define ____LOCAL__________
static UMMA_t*	umma_hotplug_getInstance(void)
{
	if (s_pstUmmaHp == NULL)
	{
		unsigned long	tid;
		HERROR hErr;

		s_pstUmmaHp = (UMMA_t*)UMMA_Calloc(sizeof(UMMA_t));

		HxSEMT_Create(&s_pstUmmaHp->ulSemId, "ummahpsem", 0);

		HxSEMT_Get(s_pstUmmaHp->ulSemId);
		hErr = UMMA_RpcProtocol_GetRpcHandle(&s_pstUmmaHp->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error!! (Get RPC Handle)\n");
			HxLOG_Assert(s_pstUmmaHp->nRpcHandle);
		}

		s_pstUmmaHp->pstRequestList= HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, UMMA_Free_CB);

		//	UMMA MainTask의 Wakeup을 위한 Event.
		HLIB_MSG_Create(128, sizeof(UMMA_EVT_t), "ummahpEvt", (unsigned long*)&s_pstUmmaHp->ulMsgQueue, eHLIB_SUSPENDTYPE_FIFO);

		s_ulUmmaHpMsgQId = s_pstUmmaHp->ulMsgQueue;


		VK_TASK_Create(umma_hotplug_mainLoop, 70, SIZE_64K, "ummahpMainLoop", (void*)s_pstUmmaHp, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
#ifdef	DUMP_STACK
		s_tid = tid;
		HxLOG_Print("---> tid : [%p]\n", tid);
#endif
	}
	else
	{
		HxSEMT_Get(s_pstUmmaHp->ulSemId);
	}

	return s_pstUmmaHp;
}

static UMMA_t*	umma_hotplug_getInstanceWO(void)
{
	return s_pstUmmaHp;
}

static void		umma_hotplug_releaseInstance(UMMA_t *pstUmma)
{
	HxSEMT_Release(pstUmma->ulSemId);
}

static void	umma_hotplug_sendEvent(HINT32 nEvt, HINT32 nP1, HINT32 nP2, HINT32 nP3)
{
	UMMA_EVT_t	stEvt;
	HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));

	stEvt.nEvent = nEvt;
	stEvt.nParam1 = nP1;
	stEvt.nParam2 = nP2;
	stEvt.nParam3 = nP3;

	HLIB_MSG_Send(s_ulUmmaHpMsgQId, &stEvt, sizeof(UMMA_EVT_t));
}


static HERROR	umma_hotplug_sendDummyEvent(UMMA_t *pstUmma)
{
	if (UMMA_OperationQueueEmpty(pstUmma))
	{
		UMMA_EVT_t	stEvt;
		HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));
		HLIB_MSG_Send(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t));
	}

	return ERR_OK;
}


static HERROR umma_hotplug_Callback(HUINT32 ulSlotIdx, HUINT32 eConnectType, void *pvInfo)
{
	DxHotplug_Info_t *pstHpInfo = NULL;
	DxHotplug_ConnectType_e	eDxConnType = eConnectType;
	UMMA_HOTPLUG_EVENT_e eEvent = eHOTPLUG_EVT_LAST;

	HxLOG_Debug("eDxConnType : %d\n", eDxConnType);

	pstHpInfo = (DxHotplug_Info_t *)HLIB_STD_MemDup(pvInfo, sizeof(DxHotplug_Info_t));

	if (eDxConnType == eDxHOTPLUG_CONN || eDxConnType == eDxHOTPLUG_CONN_BY_PWR)
	{
		eEvent = eHOTPLUG_EVT_CONNECTED;
	}
	else if (eDxConnType == eDxHOTPLUG_DISCONN || eDxConnType == eDxHOTPLUG_DISCONN_BY_PWR)
	{
		eEvent = eHOTPLUG_EVT_DISCONNECTED;
	}

	umma_hotplug_sendEvent(eEvent, (HINT32)pstHpInfo, ulSlotIdx, 0 );

	return ERR_OK;
}

static HERROR	umma_hotplug_Init(void)
{
	SEUAPI_Hotplug_Init();
	SEUAPI_Hotplug_RegisterCallback(umma_hotplug_Callback);

	return ERR_OK;
}


static HERROR	umma_hotplug_RemoveHotplugInfo(HUINT32 ulDeviceId)
{
	HxList_t	*list = s_pstHotplugList;
	DxHotplug_Info_t	*pstInfo;

	while (list)
	{
		pstInfo = (DxHotplug_Info_t*)HLIB_LIST_Data(list);
		if (pstInfo != NULL)
		{
			if (pstInfo->ulDeviceId == ulDeviceId)
			{
				s_pstHotplugList = HLIB_LIST_Remove(s_pstHotplugList, pstInfo);
				HLIB_MEM_Free((void *)pstInfo);
				break;
			}
		}
		list = list->next;
	}
	return ERR_OK;
}

static HERROR umma_hotplug_AddHotplugInfo(DxHotplug_Info_t * pstNew)
{
	if (!pstNew)
	{
		return ERR_FAIL;
	}

	s_pstHotplugList = HLIB_LIST_Append(s_pstHotplugList, (void *)pstNew);
	return ERR_OK;
}

STATIC HCHAR * umma_hotplug_getDevTypeStr(DxHotplug_DevType_e eType)
{
#define	NAME(type)	#type

	switch(eType)
	{
		case eDxHOTPLUG_DEVTYPE_HDD:
			return NAME(eDxHOTPLUG_DEVTYPE_HDD);
			break;

		case eDxHOTPLUG_DEVTYPE_USB:
			return NAME(eDxHOTPLUG_DEVTYPE_USB);

		case eDxHOTPLUG_DEVTYPE_USB_STOR:
			return NAME(eDxHOTPLUG_DEVTYPE_USB_STOR);

		case eDxHOTPLUG_DEVTYPE_USB_INPUT:
			return NAME(eDxHOTPLUG_DEVTYPE_USB_INPUT);

		case eDxHOTPLUG_DEVTYPE_USB_WIRELESS:
			return NAME(eDxHOTPLUG_DEVTYPE_USB_WIRELESS);

		case eDxHOTPLUG_DEVTYPE_SD:
			return NAME(eDxHOTPLUG_DEVTYPE_SD);

		case eDxHOTPLUG_DEVTYPE_MIC:
			return NAME(eDxHOTPLUG_DEVTYPE_MIC);

		case eDxHOTPLUG_DEVTYPE_HID:
			return NAME(eDxHOTPLUG_DEVTYPE_HID);

		default :
			return NAME(eDxHOTPLUG_DEVTYPE_NONE);
			break;
	}

	return NAME(eDxHOTPLUG_DEVTYPE_NONE);
}


#if defined(CONFIG_DEBUG)
STATIC int umma_hotplug_CmdShowHpList (void *szArgList)
{
	HxList_t	*list = s_pstHotplugList;
	DxHotplug_Info_t	*pstInfo;

	HLIB_CMD_Printf("===== Hotplug list (Num : %d)======\n", HLIB_LIST_Length(s_pstHotplugList));

	while (list)
	{
		pstInfo = (DxHotplug_Info_t*)HLIB_LIST_Data(list);
		if (pstInfo != NULL)
		{
			HLIB_CMD_Printf("ulDeviceId : %d\n", pstInfo->ulDeviceId);
			HLIB_CMD_Printf("eDevType : %s\n", umma_hotplug_getDevTypeStr(pstInfo->eDevType));
			HLIB_CMD_Printf("nPortNum : %d\n\n", pstInfo->nPortNum);
		}
		list = list->next;
	}

	return 0;
}

STATIC void umma_hotplug_svc_CmdInit(void)
{
#define	hCmdHandle		"HP"
	HLIB_CMD_RegisterWord (hCmdHandle, umma_hotplug_CmdShowHpList, "hplist", "Show hotplug list", "hplist");
}

#endif

#define _______CMD_______________

#define _______EVENT_CALLBACK_______
static void		umma_hotplug_onChangeDevice(UMMA_t*pstInstance)
{
	HINT32	nNum = HLIB_LIST_Length(s_pstHotplugList);
	DxHotplug_Info_t	*pstStorageInfo, *pstHead;

	if (nNum)
	{
		HxList_t	*list;
		pstStorageInfo = NULL;

		pstStorageInfo = UMMA_Malloc(sizeof(DxHotplug_Info_t) * nNum);

		if (pstStorageInfo == NULL)
		{
			HxLOG_Critical("UMMA_Malloc fail!!! \n");
			return ;
		}

		list = s_pstHotplugList;
		pstHead = pstStorageInfo;
		while (list)
		{
			HxSTD_MemCopy(pstStorageInfo++, HLIB_LIST_Data(list), sizeof(DxHotplug_Info_t));
			list = list->next;
		}
		HxLOG_Debug("HotplugNum : %d \n", nNum);

		HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_HOTPLUG_onChangeDevice,  "ib", nNum, (void*)pstHead, sizeof(DxHotplug_Info_t) * nNum);
		UMMA_Free((void *)pstHead);
	}
	else
	{
		HxLOG_Debug("HotplugNum : %d \n", nNum);
		HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_HOTPLUG_onChangeDevice,  "ib", nNum, "Empty", 5);
	}
}

static void		umma_hotplug_onDeviceConnected(HUINT32 ulDeviceId, HBOOL bConnected, DxHotplug_Info_t	*pstInfo, UMMA_t*pstInstance)
{
	HxLOG_Trace();
	HLIB_RPC_Notify(pstInstance->nRpcHandle, RPC_UMMA_HOTPLUG_onDeviceConnected, "iib", ulDeviceId, (bConnected ? 1 : 0), pstInfo, sizeof(DxHotplug_Info_t));
}


#define _____RPC_FUNCTION_____



//	nArgc == 0
static HERROR umma_hotplug_rpc_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	UMMA_t	*pstUmmaFs;
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	UMMA_ASSERT(nArgc == 0);

	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eHOTPLUG_SIGNAL_INIT;
	pstSignal->ulParam1 = 0;
	pstSignal->ulParam2 = 0;
	pstSignal->ulParam3 = 0;
	pstSignal->nReqId	= nRequestId;

	pstUmmaFs = umma_hotplug_getInstance();
	pstUmmaFs->pstSignalHandler = HLIB_LIST_Append(pstUmmaFs->pstSignalHandler, (void*)pstSignal);

	umma_hotplug_sendDummyEvent(pstUmmaFs);

	umma_hotplug_releaseInstance(pstUmmaFs);

	return ERR_OK;
}


static HERROR	umma_hotplug_rpc_MainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	UMMA_t	*pstUmmaHp;
	UMMA_SIGNAL_t	*pstSignal;

	HxLOG_Trace();
	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));
	pstSignal->ulSignal = eHOTPLUG_SIGNAL_APPREADY;

	pstUmmaHp = umma_hotplug_getInstance();
	pstUmmaHp->pstSignalHandler = HLIB_LIST_Append(pstUmmaHp->pstSignalHandler, (void*)pstSignal);

	umma_hotplug_sendDummyEvent(pstUmmaHp);

	umma_hotplug_releaseInstance(pstUmmaHp);
	return ERR_OK;
}

static HERROR	umma_hotplug_rpc_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	PxSTATUS_e			ePapiStatus;
	UMMA_t 		 *pstUmmaHp;
	UMMA_SIGNAL_t	 *pstSignal;

	HxLOG_Assert(nArgc == 1);

	ePapiStatus = HxOBJECT_INT(apArgv[0]);

	HxLOG_Trace();
	pstSignal = (UMMA_SIGNAL_t*)UMMA_Calloc(sizeof(UMMA_SIGNAL_t));

	if (ePapiStatus == ePAMA_STATUS_Shutdown)
	{
		pstSignal->ulSignal = eHOTPLUG_SIGNAL_SHUTDOWN;
	}
	else if (ePapiStatus == ePAMA_STATUS_Operation)
	{
		pstSignal->ulSignal = eHOTPLUG_SIGNAL_OPERATION;
	}
	else if (ePapiStatus == ePAMA_STATUS_ShutdownDone)
	{
		pstSignal->ulSignal = eHOTPLUG_SIGNAL_SHUTDOWNDONE;
	}
	else
	{
		UMMA_Free(pstSignal);
		return ERR_OK;
	}

	pstUmmaHp = umma_hotplug_getInstance();
	pstUmmaHp->pstSignalHandler = HLIB_LIST_Append(pstUmmaHp->pstSignalHandler, (void*)pstSignal);

	umma_hotplug_sendDummyEvent(pstUmmaHp);
	umma_hotplug_releaseInstance(pstUmmaHp);

	return ERR_OK;
}


#define ___________TASK___________________

static void		umma_hotplug_eventProcess(UMMA_t *pstUmma, UMMA_EVT_t *pstEvent)
{
	HUINT32		ulDeviceId;
	HINT32		nEvt, nP1;
//	HERROR		hErr;
	DxHotplug_Info_t	*pstHpInfo;

	if (!pstEvent)
	{
		HxLOG_Error("pstEvent is null.. \n");
		return;
	}

	nEvt = pstEvent->nEvent;
	nP1 = pstEvent->nParam1;
	ulDeviceId = pstEvent->nParam2;
	(void)pstEvent->nParam3;

	pstHpInfo = (DxHotplug_Info_t *)nP1;
	switch (nEvt)
	{
		case eHOTPLUG_EVT_CONNECTED:
			{
				DxHotplug_Info_t	*pstInfo;

				// Add device list
				// Assign device Id

				pstInfo = (DxHotplug_Info_t *)HLIB_MEM_MemDup(pstHpInfo, sizeof(DxHotplug_Info_t));

				umma_hotplug_AddHotplugInfo(pstInfo);

				HxLOG_Warning("eHOTPLUG_EVT_CONNECTED (ulDeviceId : %d, DevType : %d, PortNum : %d)\n",ulDeviceId, pstHpInfo->eDevType, pstHpInfo->nPortNum);

				umma_hotplug_onChangeDevice(pstUmma);
				umma_hotplug_onDeviceConnected(ulDeviceId, TRUE, pstInfo, pstUmma);
			}
			break;
		case eHOTPLUG_EVT_DISCONNECTED:
			{
				DxHotplug_Info_t	*pstInfo;

				// Add device list
				// Assign device Id
				umma_hotplug_RemoveHotplugInfo(ulDeviceId);

				pstInfo = (DxHotplug_Info_t *)HLIB_MEM_MemDup(pstHpInfo, sizeof(DxHotplug_Info_t));
				HxLOG_Warning("eHOTPLUG_EVT_DISCONNECTED (ulDeviceId : %d, DevType : %d, PortNum : %d)\n",ulDeviceId, pstHpInfo->eDevType, pstHpInfo->nPortNum);

				umma_hotplug_onChangeDevice(pstUmma);
				umma_hotplug_onDeviceConnected(ulDeviceId, FALSE, pstInfo, pstUmma);
				UMMA_Free(pstInfo);

			}
			break;

		default:
			HxLOG_Critical("Unknown event!!! Check ignored event!!!(%d) \n", nEvt);
			break;
	}

	if (pstHpInfo)
	{
		UMMA_Free(pstHpInfo);
	}

}

static void		umma_hotplug_signal_Process(UMMA_t *pstUmma, UMMA_SIGNAL_t *pstSignal)
{
	HCHAR	*pszSourceDir = NULL;
	HINT32	nRequestId;

	nRequestId = pstSignal->nReqId;

	switch (pstSignal->ulSignal)
	{
		case eHOTPLUG_SIGNAL_INIT:
			break;

		default:
			HxLOG_Debug("Unknown signal (%d) \n", pstSignal->ulSignal);
			break;
	}

	if (pszSourceDir)
		UMMA_Free((void *)pszSourceDir);

}


static void		umma_hotplug_mainLoop(void *arg)
{
	UMMA_t		*pstUmma;
	UMMA_EVT_t	stEvt;

	HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));

	while (1)
	{
		// Start JOB wait
		pstUmma = umma_hotplug_getInstanceWO();

		if (stEvt.nEvent == 0 && pstUmma->pstSignalHandler)	// signal
		{
			UMMA_SIGNAL_t	*pstSignal;

			pstSignal = (UMMA_SIGNAL_t*)HLIB_LIST_Data(pstUmma->pstSignalHandler);

			// UMMA_FS Job 시작과 관련된 signal
			if (pstSignal->ulSignal == eHOTPLUG_SIGNAL_APPREADY)
			{
				if (s_bJobRunning != TRUE)
				{
					s_bJobRunning = TRUE;
					HxLOG_Debug("############ UMMA Hotplug Job Init!!############\n");
					umma_hotplug_Init();

					s_bDevPower = TRUE;	// svc_init으로 전원이 인가됨
				}
			}
			else if (pstSignal->ulSignal == eHOTPLUG_SIGNAL_SHUTDOWN)
			{
				if (s_bJobRunning == FALSE && s_bDevPower == FALSE)
				{
					// booting시 바로 진입
					s_bJobRunning = TRUE;
					HxLOG_Debug("############ UMMA Hotplug Job Init!!############\n");
					umma_hotplug_Init();

					s_bDevPower = TRUE;	// svc_init으로 전원이 인가됨
				}
				else if (s_bJobRunning == TRUE && s_bDevPower == FALSE)
				{
					// booting시 바로 진입
					s_bJobRunning = TRUE;
					HxLOG_Debug("S3 mode booting ... On device power.\n");
					s_bDevPower = TRUE;	// svc_init으로 전원이 인가됨
				}

				// else : nothing...
			}
			else if (pstSignal->ulSignal == eHOTPLUG_SIGNAL_OPERATION)
			{
				if (s_bJobRunning == TRUE && s_bDevPower == FALSE)
				{
					// S3 mode booting --> On devices power
					HxLOG_Debug("S3 mode booting ... On device power.\n");
					s_bDevPower = TRUE;
				}
				else
				{
					//  booting시 바로 진입 등, eHOTPLUG_SIGNAL_APPREADY 받기 전엔 동작하지 않는다.
				}
			}
			else if (pstSignal->ulSignal == eHOTPLUG_SIGNAL_SHUTDOWNDONE)
			{
				// Off devices power
				HxLOG_Debug("Off device power.....\n");
				s_bDevPower = FALSE;
			}
			else if(s_bJobRunning == TRUE)
			{
				pstUmma = umma_hotplug_getInstance();

				// From RPC
				umma_hotplug_signal_Process(pstUmma, pstSignal);
				umma_hotplug_releaseInstance(pstUmma);
			}
			else
			{
				HxLOG_Debug("############ UMMA Hotplug job not yet started!!(signal : %d)############\n", pstSignal->ulSignal);
			}

			pstUmma->pstSignalHandler = HLIB_LIST_Remove (pstUmma->pstSignalHandler, (void*)pstSignal);
			UMMA_Free((void *)pstSignal);
		}
		else	if (stEvt.nEvent != 0)// Event
		{
			pstUmma->pstEventList = HLIB_LIST_Append(pstUmma->pstEventList, UMMA_MemDup((void *)&stEvt, sizeof(UMMA_EVT_t)));
		}

		if (s_bJobRunning == FALSE)
		{
			umma_hotplug_releaseInstance(pstUmma);

			if (stEvt.nEvent != 0) {
				HxLOG_Debug("UMMA Hotplug job not yet started...  keep event (%d)\n", stEvt.nEvent);
			}

			// wait msg (cmd, event)
			if (HLIB_MSG_Receive(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t)) == ERR_OK)
			{
				continue;
			}
		}
		else
		{
			// Event list 처리
			UMMA_EVT_t		*pstEvt;

			// From umma_hotplug
			pstUmma = umma_hotplug_getInstance();

			while(pstUmma->pstEventList)
			{
				pstEvt = (UMMA_EVT_t*)HLIB_LIST_Data(pstUmma->pstEventList);
				umma_hotplug_eventProcess(pstUmma, pstEvt);
				pstUmma->pstEventList = HLIB_LIST_Remove(pstUmma->pstEventList, (void *)pstEvt);
				UMMA_Free((void *)pstEvt);
			}

			umma_hotplug_releaseInstance(pstUmma);

			// wait msg (cmd, event)
			if (HLIB_MSG_ReceiveTimeout(pstUmma->ulMsgQueue, &stEvt, sizeof(UMMA_EVT_t), 1000)  == ERR_OK)
			{
				continue;
			}
			else
			{
				HxSTD_MemSet(&stEvt, 0, sizeof(UMMA_EVT_t));
			}
		}
	}
}

#define ___________API____________
HERROR	UMMA_HOTPLUG_Init(void)
{
	HERROR	err;
	UMMA_t		*pstUmma;

	HxLOG_Trace();
	pstUmma = umma_hotplug_getInstance();

	// API
	err  = HLIB_RPC_RegisterCall(pstUmma->nRpcHandle, RPC_UAPI_HOTPLUG_Init, NULL, umma_hotplug_rpc_Init, NULL,	"Activate umma hotplug module: (void)");

	// Event callback
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_HOTPLUG_onDeviceConnected, "iib", "Device is connected or disconnected(HUINT32 eDevType, HBOOL bConnected, DxHotplug_Info_t *pstStorageInfo)");
	err |= HLIB_RPC_CreateNotification(pstUmma->nRpcHandle, RPC_UMMA_HOTPLUG_onChangeDevice, "ib", "Device lists are changed (attached / detached) (HINT32 nDevNum, DxHotplug_Info_t *pstStorageInfo)");

	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, umma_hotplug_rpc_MainApplicationReady);
	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, umma_hotplug_rpc_onSystemStateChanged);

	umma_hotplug_releaseInstance(pstUmma);

#if defined(CONFIG_DEBUG)
	umma_hotplug_svc_CmdInit();
#endif

	return err;
}

