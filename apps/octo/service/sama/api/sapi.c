/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama.c
	@brief

	Description:  									\n
	Module: SAMA		\n

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
#include <htype.h>
#include <hlib.h>
#include <vkernel.h>
#include <sapi.h>

#include <sapi_int.h>
#include <sama_lib.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
#define	SAPI_CALLBACK_NAME_LEN				256
#define	SAPI_CALLBACK_MAX					12

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


STATIC sapi_Inst_t		 s_stSapi_Inst;

#define ___PRIVATE_FUNCTIONS_DECLARATION___
/*******************************************************************/
/********************      Private Functions Declaration    ********************/
/*******************************************************************/
STATIC void				sapi_onPacketReceived (void *pvUserData, HINT32 nSocket, HUINT32 ulReqestId, HINT32 nSize, void *pvData);
STATIC void				sapi_onStatusChanged (void *pvUserData, HxSocket_Status_e eStatus, HINT32 nSocket);
STATIC void				sapi_socketListener (void *arg);

#define ___PRIVATE_FUNCTIONS___
/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/

#define _____SAPI_INITIALIZATION_____
STATIC HERROR sapi_initComm (sapi_Inst_t *pstInst)
{
	HERROR				 hErr = ERR_OK;

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr  = sapi_base_CmdInit(pstInst);
		hErr |= sapi_base_CmdRefreshScheduleData(pstInst);
		break;
	}

	return hErr;
}


#define _____RPC_CALLBACKS_____				// SAPI <- SAMA
STATIC HERROR sapi_registerRpcCallback (sapi_Inst_t *pstInst)
{
	if (NULL == pstInst)
	{
		HxLOG_Error("NULL == pstInst\n");
		return ERR_FAIL;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_TvAnyTime:
		return sapi_tva_RegisterRpcCallback(pstInst);
	case eDxSchedule_Spec_Base:
	default:
		return sapi_base_RegisterRpcCallback(pstInst);
	}
}


#define ___SAPI_SAMA_CONNECT___

// sem으로 잘 control 해줘야 해요.
STATIC void sapi_Disconnect(void)
{
	HxLOG_Error(" + \n");
	s_stSapi_Inst.nSocket = 0;
	HLIB_RPC_Close(s_stSapi_Inst.nRpcHandle);

}

// sem으로 잘 control 해줘야 해요.
STATIC void sapi_Connect(void)
{
	HxLOG_Warning(" + \n");
	s_stSapi_Inst.nSocket			= HLIB_EXTRA_OpenClient("sama");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS, SAMA_SIGNAL_UDS, 0);
	s_stSapi_Inst.nRpcHandle		= HLIB_RPC_Open(s_stSapi_Inst.nSocket, s_stSapi_Inst.nPid);
	(void)sapi_registerRpcCallback(&s_stSapi_Inst);
}


STATIC HERROR sapi_loadSpec (sapi_Inst_t *pstInst)
{
	//ToDo: 추후 File을 읽어서 Spec을 정하도록 수정한다.

	pstInst->stSamaConflictCheck.stSpecOption.eSpec = SAMALIB_SPEC_Get();

	return ERR_OK;
}

#define _____DATA_RECEPTION_____
STATIC void sapi_onPacketReceived (void *pvUserData, HINT32 nSocket, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	sapi_Inst_t		*pstInst = (sapi_Inst_t*)pvUserData;


	HxSEMT_Get(pstInst->ulSemId);
	HLIB_RPC_ProcessRPC(pstInst->nRpcHandle, (HUINT32)nSize, pvData, ulRequestId, nSocket);
	HxSEMT_Release(pstInst->ulSemId);

}

STATIC void sapi_onStatusChanged (void *pvUserData, HxSocket_Status_e eStatus, HINT32 nSocket)
{
//	sapi_Inst_t		*pstInst = (sapi_Inst_t*)pvUserData;

	HxLOG_Debug("(+) eStatus(%d)\n", eStatus);

	switch (eStatus)
	{
	case HxSOCKET_DISCONNECT_CLIENT:
		HxLOG_Critical("HxSOCKET_DISCONNECT_CLIENT!!!\n");
		HxSEMT_Get(s_stSapi_Inst.ulSemId);
		sapi_Disconnect();
		HxSEMT_Release(s_stSapi_Inst.ulSemId);

		HLIB_SOCKET_Close(nSocket);
		break;

	default:
		break;
	}

	HxLOG_Debug("(-)\n");
}

STATIC void sapi_socketListener (void *arg)
{
	sapi_Inst_t 	*pstInst = (sapi_Inst_t *)arg;

	while(1)
	{
		if(pstInst->nSocket)
		{
			HLIB_SOCKET_Listen(pstInst->nSocket, FALSE, sapi_onPacketReceived, sapi_onStatusChanged, (void *)pstInst);
			HLIB_STD_TaskSleep(10);
		}
		else
		{
			HUINT32 nCnt = 0;

			HLIB_STD_TaskSleep(100);
			HxSEMT_Get(s_stSapi_Inst.ulSemId);
			sapi_Connect();
			(void)sapi_initComm(&s_stSapi_Inst);


			if(pstInst->stSamaConflictCheck.bRscSet == TRUE)
			{
				(void)sapi_base_CmdSetRscInfo(pstInst, &(pstInst->stSamaConflictCheck.stRscInfo));
			}

			for(nCnt = 0; nCnt < RxRFINPUT_NUM_MAX; nCnt++)
			{
				if(pstInst->stSamaConflictCheck.bRfInputAbilitySet[nCnt] == TRUE)
				{
					(void)sapi_base_CmdSetRfInputAbility(pstInst, nCnt, pstInst->stSamaConflictCheck.anRfInputAbility[nCnt]);
				}

			}

			HxSEMT_Release(s_stSapi_Inst.ulSemId);
		}
	}
}

#define _____NOTIFIER_____				// SAPI -> SAMA
STATIC sapi_NotifyItem_t *sapi_findNotifier (sapi_Inst_t *pstInst, void *pvUserData, SAPI_NotiFunc_t pfNotifier)
{
	HxList_t			*pstListItem;

	for (pstListItem = pstInst->pstNotifierList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		sapi_NotifyItem_t		*pstNotiItem = (sapi_NotifyItem_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotiItem) &&
			(pvUserData == pstNotiItem->pvUserData) &&
			(pfNotifier == pstNotiItem->pfNotifier))
		{
			return pstNotiItem;
		}
	}

	return NULL;
}


#define _____SCHEDULE_CACHE_____
STATIC HBOOL sapi_cbCmpScheduleBySlotId (void *pvUser, void *pvListData)
{
	DxSchedule_t			*pstSchedule = (DxSchedule_t *)pvListData;
	HUINT32					 ulSlotId = (HUINT32)pvUser;

	if ((NULL != pstSchedule) && (ulSlotId == pstSchedule->ulSlot))
	{
		return TRUE;
	}

	return FALSE;
}


#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/

// SAMA_SCHEDULE_RemovePadding 와 동일한 함수이다.. 정리해야함.
STATIC HBOOL sapi_RemovePadding(sapi_Inst_t *pstInst)
{
	DxSchedule_t		 stSchedule;
	HxList_t				*pstListItem = NULL;
	sama_PadRemoval_t		*pstPadRemoval;
	HERROR					 hErr;
	HBOOL				bUpdate = FALSE;
	HUINT32				ulUpdateCnt = 0;

	sama_CnftTimeEvt_t *pstStartTimeEvt;

	if ((NULL == pstInst) || (NULL == pstInst->stSamaConflictCheck.pstCheckStartEvt) || (NULL == pstInst->stSamaConflictCheck.pstCheckEndEvt) )
	{
		return FALSE;
	}

	pstStartTimeEvt = pstInst->stSamaConflictCheck.pstCheckStartEvt;

	if (eSAPI_CONFLICT_TYPE_None == pstInst->stSamaConflictCheck.stConflictResult.eType)
	{
		HxLOG_Debug("No Conflict... now remove the padding time required\n");

		for (pstListItem = pstInst->stSamaConflictCheck.pstPadRemoveInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			bUpdate = FALSE;

			pstPadRemoval = (sama_PadRemoval_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstPadRemoval)
			{
				hErr = SAPI_GetSchedule(pstPadRemoval->ulSlot, &stSchedule);
				if (ERR_OK == hErr)
				{
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
					if (pstPadRemoval->ulSlot == pstStartTimeEvt->pstSchedule->ulSlot)
#endif
					{
						HxLOG_Debug("pstPadRemoval->ulSlot: %d, pstPadRemoval->ePadRemoval: 0x%x \n", pstPadRemoval->ulSlot, pstPadRemoval->ePadRemoval);
						if (pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StartPad)
						{
							if(stSchedule.uExtInfo.stRec.ulPreOffsetTime != SAMA_TVA_REMOVED_PADDING_TIME)
							{
								stSchedule.uExtInfo.stRec.ulPreOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
								bUpdate = TRUE;
							}
						}

						if (pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StopPad)
						{
							if(stSchedule.uExtInfo.stRec.ulPostOffsetTime != SAMA_TVA_REMOVED_PADDING_TIME)
							{
								stSchedule.uExtInfo.stRec.ulPostOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
								bUpdate = TRUE;
							}
						}

						if(bUpdate == TRUE)
						{
							ulUpdateCnt++;
							SAPI_UpdateSchedule(pstPadRemoval->ulSlot, &stSchedule, NULL, TRUE, TRUE);
						}
					}
				}
				else if (pstPadRemoval->ulSlot == pstStartTimeEvt->pstSchedule->ulSlot)
				{
					if (pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StartPad)
					{
						pstStartTimeEvt->pstSchedule->uExtInfo.stRec.ulPreOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
					}

					if (pstPadRemoval->ePadRemoval & eSAPI_PADDINGRMV_StopPad)
					{
						pstStartTimeEvt->pstSchedule->uExtInfo.stRec.ulPostOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
					}
				}
			}
		}
	}
	else
	{
		HxLOG_Debug("Conflict Occurred... \n");
	}

	// 여기서 free 하지 않는 것은 의도된 동작. 다른 곳에서 사용할수도 있기 때문에, make conflict time line 할때만 새로 만들어주는 형태임.
	//pstInst->pstPadRemoveInfoList = HLIB_LIST_RemoveAllFunc(pstInst->pstPadRemoveInfoList, SAMA_Free_CB);
	if(ulUpdateCnt > 0)
	{
		bUpdate = TRUE;
	}
	else
	{
		bUpdate = FALSE;
	}
	return bUpdate;
}

#if defined(CONFIG_ENHANCED_SECURITY)
STATIC SAPI_UniqueProcessIndicator_e sapi_getProcessUniqueId (HCHAR* pszProcessName)
{
	SAPI_UniqueProcessIndicator_e		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Unknown;

	if(HLIB_STD_StrNCaseCmp( pszProcessName, "sama", 4 ) == 0 )
	{
		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Sama;
	}
	else 	if(HLIB_STD_StrNCaseCmp( pszProcessName, "obama", 5 ) == 0 )
	{
		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Obama;
	}
	else 	if(HLIB_STD_StrNCaseCmp( pszProcessName, "homma", 5 ) == 0 )
	{
		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Homma;
	}
	else 	if(HLIB_STD_StrNCaseCmp( pszProcessName, "corsair", 7 ) == 0 )
	{
		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Corsair;
	}
	else 	if(HLIB_STD_StrNCaseCmp( pszProcessName, "ipepg", 5 ) == 0 )
	{
		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Ipepg;
	}
	else 	if(HLIB_STD_StrNCaseCmp( pszProcessName, "nova", 4 ) == 0 )
	{
		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Nova;
	}
	else
	{
		eUniqueProcessIndicator = eSAPI_UNIQUE_PROCESS_Indicator_Unknown;
	}

	return eUniqueProcessIndicator;
}
#endif


sapi_Inst_t *sapi_GetInstance (void)
{
	if (TRUE != s_stSapi_Inst.bSapi_Inited)
	{
		HUINT32					 ulCount;
		HUINT32				nPid;

		HxLOG_Debug("Initialize the Instance\n");

		SAMA_MemSet(&s_stSapi_Inst, 0, sizeof(sapi_Inst_t));

		(void)HxSEMT_Create(&(s_stSapi_Inst.ulSemId), "sapi_sem", 0);

		HxSEMT_Get(s_stSapi_Inst.ulSemId);
		for (ulCount = 0; ulCount < SAMA_EMPTY_SLOT_PER_PROCESS; ulCount++)
		{
			s_stSapi_Inst.aulAvailSlots[ulCount] = SAPI_SLOTID_NULL;
		}

		// Load the spec info from the setting file:
		(void)sapi_loadSpec(&s_stSapi_Inst);

#if defined(CONFIG_ENHANCED_SECURITY)
		nPid = (HUINT32)sapi_getProcessUniqueId(HLIB_STD_GetCurProcessName());
#else
		nPid = HLIB_STD_GetPID();
#endif
		s_stSapi_Inst.nPid				= nPid;

		// connect.
		sapi_Connect();

		HxLOG_Warning("** (%s) is connected to SAMA\n", HLIB_STD_GetCurProcessName());

		(void)VK_TASK_Create(sapi_socketListener, 64, SIZE_512K, "sapi_listener", (void *)&s_stSapi_Inst, (unsigned long*)&(s_stSapi_Inst.ulTaskId), 0);
		(void)VK_TASK_Start(s_stSapi_Inst.ulTaskId);

		// init comm.
		(void)sapi_initComm(&s_stSapi_Inst);
		s_stSapi_Inst.bSapi_Inited = TRUE;
	}
	else
	{
		HxSEMT_Get(s_stSapi_Inst.ulSemId);
	}

	return &s_stSapi_Inst;
}

HERROR sapi_GetConflictResult (sapi_Inst_t *pstInst, HUINT32 ulSlotId, SAPI_Conflict_t *pstConflict)
{
	SAPI_Conflict_t	*pstSrcConflict = &(pstInst->stSamaConflictCheck.stConflictResult);
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (ulSlotId != pstSrcConflict->stTriedSchedule.ulSlot)
	{
		HxLOG_Error("Conflict Slot ID not matching... (%d:%d)\n", ulSlotId, pstInst->stSamaConflictCheck.stConflictResult.stTriedSchedule);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	HxSTD_MemCopy(pstConflict, pstSrcConflict, sizeof(SAPI_Conflict_t));

#ifdef CONFIG_DEBUG
	{
		HUINT32 		i=0, ulNumConflict = 0;

		HxLOG_Debug("pstConflict->eType: %d \n", pstConflict->eType);
		HxLOG_Debug("print pstConflict->stTriedSchedule \n");
		sapi_PrintSchedule(&(pstConflict->stTriedSchedule));

		HxLOG_Debug("print pstConflict->ulNumConflict: %d, pstConflict->pstConflictArray[i]->ulSlot \n", pstConflict->ulNumConflict);
		ulNumConflict = pstConflict->ulNumConflict;
		for(i=0; i<ulNumConflict; i++)
		{
			SAPI_ConflictItem_t 		*pstConflictArray;
			pstConflictArray = &(pstConflict->pstConflictArray[i]);
			HxLOG_Debug("ulSlot: %d, ", pstConflictArray->ulSlot);
		}
		HxLOG_Debug("\n");
	}
#endif

	hErr = ERR_OK;
END_FUNC:

	return hErr;
}

void sapi_ReleaseInstance (sapi_Inst_t *pstInst)
{
	if(pstInst != NULL)
	{
		HxSEMT_Release(pstInst->ulSemId);
	}
}

void sapi_ExecuteNotifiers (sapi_Inst_t *pstInst, SAPI_NotiInfo_t *pstNotify)
{
	HxList_t			*pstListItem;

	for (pstListItem = pstInst->pstNotifierList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		sapi_NotifyItem_t		*pstNotiItem = (sapi_NotifyItem_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotiItem) &&
			(NULL != pstNotiItem->pfNotifier))
		{
			HxSEMT_Release(pstInst->ulSemId);
			(void)pstNotiItem->pfNotifier(pstNotiItem->pvUserData, pstNotify);
			HxSEMT_Get(pstInst->ulSemId);
		}
	}
}

HERROR sapi_AppendScheduleToCache (sapi_Inst_t *pstInst, DxSchedule_t *pstSchedule)
{
	DxSchedule_t		*pstDstSch;

	if ((NULL == pstInst) || (NULL == pstSchedule))
	{
		HxLOG_Error("Source NULL\n");
		return ERR_FAIL;
	}

	if (sapi_FindScheduleFromCache(pstInst, pstSchedule->ulSlot) != NULL)
	{
		HxLOG_Error("Already the slot is in the cache:\n");
		return ERR_FAIL;
	}

	pstDstSch = (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t));
	if (NULL == pstDstSch)
	{
		HxLOG_Error("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	SAMA_MemCpy(pstDstSch, pstSchedule, sizeof(DxSchedule_t));
	pstInst->stSamaConflictCheck.pstScheduleList = HLIB_LIST_Append(pstInst->stSamaConflictCheck.pstScheduleList, (void *)pstDstSch);

	return ERR_OK;
}

DxSchedule_t *sapi_FindScheduleFromCache (sapi_Inst_t *pstInst, HUINT32 ulSlot)
{
	HxList_t		*pstListItem;
	DxSchedule_t	*pstSrcSchedule;

	if (NULL == pstInst)
	{
		HxLOG_Error("Source NULL\n");
		return NULL;
	}

	pstListItem = HLIB_LIST_FindEx(pstInst->stSamaConflictCheck.pstScheduleList, (const void *)ulSlot, sapi_cbCmpScheduleBySlotId);
	pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Debug("No Schedule with the slot id (%d)\n", ulSlot);
		return NULL;
	}

	return pstSrcSchedule;
}

STATIC DxSchedule_t *sapi_findEarliestSchedule (sama_ConflictCheck_t *pstInst)
{
	UNIXTIME		 utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	UNIXTIME		 utStartTime, utEarliestTime = 0xFFFFFFFF;
	DxSchedule_t	*pstEarliest = NULL;
	HxList_t				*pstListItem;

	HLIB_DATETIME_PrintDateTime(utCurrTime);

	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return NULL;
	}

	for (pstListItem = pstInst->pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

#if defined(CONFIG_OP_UK_DTT)
		if (NULL != pstSchedule
			&& pstSchedule->bRemoved != TRUE
			&& pstSchedule->eRsvType != DxRSVTYPE_ASO_NOTIFY
			&& pstSchedule->eRsvType != DxRSVTYPE_IP_EPG_GATHERING)	// EPG Gather마다 깨어나는 것은 humax ipepg 사양. UK FVC 사양은 안 이렇다.
#else
		if (NULL != pstSchedule
			&& pstSchedule->bRemoved != TRUE
			&& pstSchedule->eRsvType != DxRSVTYPE_ASO_NOTIFY)
#endif
		{
			sapi_PrintSchedule(pstSchedule);

			utStartTime = 0;

			(void)SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
			if (utStartTime > 0)
			{
				if ((NULL == pstEarliest) ||
					(utStartTime < utEarliestTime))
				{
					pstEarliest = pstSchedule;
					utEarliestTime = utStartTime;
				}
			}
		}
	}

	return pstEarliest;
}

STATIC DxSchedule_t *sapi_findEarliestScheduleWithPaddingAndReadyTime (sama_ConflictCheck_t *pstInst, UNIXTIME *putStartTimeWithPaddingAndReady)
{
	UNIXTIME		 utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	UNIXTIME		 utStartTime, utEarliestTime = 0xFFFFFFFF;
	DxSchedule_t	*pstEarliest = NULL;
	HxList_t				*pstListItem;
	UNIXTIME		 utStartTimeWithPaddingAndReady;


	HLIB_DATETIME_PrintDateTime(utCurrTime);

	if (utCurrTime < SAMA_TIME_AVAIL_MIN)
	{
		HxLOG_Error("HLIB_STD_GetSystemTime err:\n");
		return NULL;
	}

	for (pstListItem = pstInst->pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t		*pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

#if defined(CONFIG_OP_UK_DTT)
		if (NULL != pstSchedule
			&& pstSchedule->bRemoved != TRUE
			&& pstSchedule->eRsvType != DxRSVTYPE_ASO_NOTIFY
			&& pstSchedule->eRsvType != DxRSVTYPE_IP_EPG_GATHERING)
#else
		if (NULL != pstSchedule
			&& pstSchedule->bRemoved != TRUE
			&& pstSchedule->eRsvType != DxRSVTYPE_ASO_NOTIFY)
#endif
		{
			sapi_PrintSchedule(pstSchedule);

			utStartTime = 0;
			utStartTimeWithPaddingAndReady = 0;

			(void)SAMALIB_TIME_GetRepeatedScheduleStartTime(pstSchedule, utCurrTime, &utStartTime);
			(void)SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utStartTimeWithPaddingAndReady, NULL);
			utStartTimeWithPaddingAndReady -= pstSchedule->eRsvReady;

			if (utStartTimeWithPaddingAndReady > 0)
			{
				if ((NULL == pstEarliest) ||
					(utStartTimeWithPaddingAndReady < utEarliestTime))
				{
					pstEarliest = pstSchedule;
					utEarliestTime = utStartTimeWithPaddingAndReady;
				}
			}
		}
	}

	if (NULL != putStartTimeWithPaddingAndReady)
	{
		*putStartTimeWithPaddingAndReady = utEarliestTime;
	}

	return pstEarliest;
}

STATIC HBOOL	sapi_checkScheduledWithSvcUidAndStartEndTime (sama_ConflictCheck_t *pstInst, HUINT32 ulSvcUid, HUINT32 ulStartTime, HUINT32 ulEndTime)
{
	DxSchedule_FindFilterAttr_t stFilter;
	HxList_t			*pstFoundList = NULL;
	HxList_t			*pstListItem = NULL;
	HBOOL				bScheduled = FALSE;
	HUINT32				ulListLength = 0;

	SAMALIB_CFLT_FindScheduleListFilterInit(&stFilter);

	stFilter.ulSvcUid = ulSvcUid;
	stFilter.utStartTime = ulStartTime;
	stFilter.utEndTime = ulEndTime;

	(void)SAMALIB_CFLT_FindScheduleListUsingFilter(pstInst->pstScheduleList, &stFilter, &pstFoundList);

	if (pstFoundList == NULL)
	{
		HxLOG_Debug("there is no schedule list satisfing filter \n");
		bScheduled = FALSE;
		goto END_FUNC;
	}

	ulListLength = HLIB_LIST_Length(pstFoundList);
	if (ulListLength == 0)
	{
		HxLOG_Error("list length is 0 although the list exists! \n");
		bScheduled = FALSE;
		goto END_FUNC;
	}

	bScheduled = TRUE;

	for (pstListItem = pstFoundList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t	*pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstSrcSchedule)
		{
			HxLOG_Debug("found schedule. \n");
			sapi_PrintSchedule(pstSrcSchedule);
		}
	}
END_FUNC:

	if (pstFoundList != NULL)
	{
		HLIB_LIST_RemoveAll(pstFoundList);
	}

	return bScheduled;
}

HERROR sapi_RemoveScheduleFromCache (sapi_Inst_t *pstInst, HUINT32 ulSlot)
{
	DxSchedule_t	*pstSrcSchedule;

	if (NULL == pstInst)
	{
		HxLOG_Error("Source NULL\n");
		return ERR_FAIL;
	}

	pstSrcSchedule = sapi_FindScheduleFromCache(pstInst, ulSlot);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("No Schedule with the slot(%d) exists:\n", ulSlot);
		return ERR_FAIL;
	}

	pstInst->stSamaConflictCheck.pstScheduleList = HLIB_LIST_Remove(pstInst->stSamaConflictCheck.pstScheduleList, (const void *)pstSrcSchedule);
	SAMA_Free(pstSrcSchedule);

	return ERR_OK;
}

HERROR sapi_PrintSchedule (DxSchedule_t *pstSchedule)
{
	HCHAR		 szDateTime[128];

	if (NULL == pstSchedule)
	{
		HxLOG_Error("NULL == pstSchedule \n");
		return ERR_FAIL;
	}
	HxLOG_Debug("Print Schedule ! \n");

	HxLOG_Debug("Schedule SlotID     : %d\n", pstSchedule->ulSlot);
	HxLOG_Debug("         RsvType    : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
	HxLOG_Debug("         SvcUID     : 0x%08x\n", pstSchedule->ulSvcUid);
	HxLOG_Debug("         OnID: 0x%04x, TsID: 0x%04x, SvcID: 0x%04x\n", pstSchedule->stTripleId.usOnId, pstSchedule->stTripleId.usTsId, pstSchedule->stTripleId.usSvcId);
	HxLOG_Debug("         RsvReady   : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvReady_e), pstSchedule->eRsvReady));
	HLIB_DATETIME_GetStringByDateTime(&(pstSchedule->stStartTime), szDateTime, 128);
	HxLOG_Debug("         StartTime  : %s\n", szDateTime);
	HxLOG_Debug("         Duration   : %d sec\n", pstSchedule->ulDuration);
	if(pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR || pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
	{
		HxLOG_Debug("         Event ID  : %d \n", pstSchedule->uExtInfo.stRec.nEvtId);
		HxLOG_Debug("         Start Padding  : %d sec\n", pstSchedule->uExtInfo.stRec.ulPreOffsetTime);
		HxLOG_Debug("         End Padding   : %d sec\n", pstSchedule->uExtInfo.stRec.ulPostOffsetTime);
		HxLOG_Debug("         Series ID   : %d\n", pstSchedule->uExtInfo.stRec.ulSeriesId);
	}
	HxLOG_Debug("         RepeatType : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvRepeat_e), pstSchedule->eRepeat));
	HxLOG_Debug("         RepeatDays : 0x%08x\n", pstSchedule->ulRepeatDays);
	HxLOG_Debug("         Status     : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvStatus_e), pstSchedule->eStatus));
	HxLOG_Debug("         Epg Type     : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), pstSchedule->eEpgType));
	HxLOG_Debug("	Schedule Spec	   : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxSchedule_Spec_e), pstSchedule->eScheduleSpec));
	HxLOG_Debug("	bPaused		   : %d\n", pstSchedule->bPaused);
	HxLOG_Debug("	bRemoved	   : %d\n", pstSchedule->bRemoved);
	HxLOG_Debug("	Removed reason	   : %s\n", DLIB_ConvertEnum2Str(DxNAMEOF(DxSchedule_RemovedReason_e), pstSchedule->stRemovedInfo.eRemovedReason));

	return ERR_OK;
}

HERROR sapi_PrintAllSchedules (void)
{
	HUINT32 					 ulCnt, ulNumSch = 0;
	DxSchedule_t				*pstSchArray = NULL;
	HERROR					hErr = ERR_FAIL;


	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("No Schedule\n");
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)SAMA_Calloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("memory allocation failed\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetAllSchedules err:\n");
		goto END_FUNC;
	}

	HxLOG_Debug("Print all scheduels!\n");
	for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
	{
		DxSchedule_t		*pstSchedule = &(pstSchArray[ulCnt]);
		sapi_PrintSchedule(pstSchedule);
	}

END_FUNC:
	if(pstSchArray != NULL)
	{
		SAMA_Free(pstSchArray);
		pstSchArray = NULL;
	}

	return hErr;

}










#define ___API_FUNCTIONS___
/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR SAPI_RegisterNotifier (void *pvUserData, SAPI_NotiFunc_t pfNotifier)
{
	sapi_Inst_t			*pstInst = sapi_GetInstance();
	sapi_NotifyItem_t	*pstNotiItem;
	SAPI_NotiInfo_t		 unNotiInfo;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pfNotifier)
	{
		HxLOG_Error("pfNotifier NULL\n");
		hErr = ERR_FAIL;
		goto END_FUNC;

	}

	pstNotiItem = sapi_findNotifier(pstInst, pvUserData, pfNotifier);
	if (NULL != pstNotiItem)
	{
		HxLOG_Error("Already the same notifier registered:\n");
		hErr = ERR_OK;
		goto END_FUNC;

	}

	pstNotiItem = (sapi_NotifyItem_t *)SAMA_Malloc(sizeof(sapi_NotifyItem_t));
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("Memory allocation failed\n");
		hErr = ERR_FAIL;
		goto END_FUNC;

	}

	pstNotiItem->pvUserData	= pvUserData;
	pstNotiItem->pfNotifier	= pfNotifier;

	pstInst->pstNotifierList = HLIB_LIST_Append(pstInst->pstNotifierList, (void *)pstNotiItem);

	if (TRUE == pstInst->bScheduleLoaded)
	{
		HUINT32			 ulCount = 0, ulSchNum = HLIB_LIST_Length(pstInst->stSamaConflictCheck.pstScheduleList);
		DxSchedule_t	*pstSchArray = (ulSchNum > 0) ? SAMA_Malloc(sizeof(DxSchedule_t) * ulSchNum) : NULL;

		if ((ulSchNum > 0) && (NULL != pstSchArray))
		{
			HxList_t		*pstListItem;

			for (pstListItem = pstInst->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
			{
				DxSchedule_t		*pstSchedule = HLIB_LIST_Data(pstListItem);

				if (NULL != pstSchedule)
				{
					SAMA_MemCpy(&(pstSchArray[ulCount]), pstSchedule, sizeof(DxSchedule_t));
					ulCount ++;
				}
			}
		}

		unNotiInfo.stSchLoaded.eNotifyType	= eSAPI_NOTIFY_TYPE_SchedulesLoaded;
		unNotiInfo.stSchLoaded.ulSchNum		= ulCount;
		unNotiInfo.stSchLoaded.pstSchArray	= pstSchArray;

		HxSEMT_Release(pstInst->ulSemId);
		(void)pfNotifier(pvUserData, &unNotiInfo);
		HxSEMT_Get(pstInst->ulSemId);
		if (NULL != pstSchArray)
		{
			SAMA_Free(pstSchArray);
		}

		hErr = ERR_OK;
		goto END_FUNC;
	}
	else
	{
		hErr = sapi_base_CmdRefreshScheduleData(pstInst);
		goto END_FUNC;

	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}

HERROR SAPI_UnregisterNotifier (void *pvUserData, SAPI_NotiFunc_t pfNotifier)
{
	sapi_Inst_t			*pstInst = sapi_GetInstance();
	sapi_NotifyItem_t	*pstNotiItem;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pfNotifier)
	{
		HxLOG_Error("pfNotifier NULL\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstNotiItem = sapi_findNotifier(pstInst, pvUserData, pfNotifier);
	if (NULL == pstNotiItem)
	{
		HxLOG_Error("No same notifier registered:\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstInst->pstNotifierList = HLIB_LIST_Remove(pstInst->pstNotifierList, (const void *)pstNotiItem);
	SAMA_Free(pstNotiItem);


	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_GetResourceInfo (SAPI_RscInfo_t *pstRscInfo)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstRscInfo)
	{
		HxLOG_Error("pstRscInfo NULL\n");
		hErr = ERR_FAIL;
		goto END_FUNC;

	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;

	}

	if (TRUE != pstInst->stSamaConflictCheck.bRscSet)
	{
		HxLOG_Error("Resource not set:\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMA_MemCpy(pstRscInfo, &(pstInst->stSamaConflictCheck.stRscInfo), sizeof(SAPI_RscInfo_t));

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_SetResourceInfo (SAPI_RscInfo_t *pstRscInfo)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();


	if (NULL == pstRscInfo)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	hErr = sapi_base_CmdSetRscInfo(pstInst, pstRscInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("sapi_base_CmdSetRscInfo err:\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstInst->stSamaConflictCheck.bRscSet = TRUE;
	SAMA_MemCpy(&(pstInst->stSamaConflictCheck.stRscInfo), pstRscInfo, sizeof(SAPI_RscInfo_t));

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();


	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (ulRfInputId >= RxRFINPUT_NUM_MAX)
	{
		HxLOG_Error("ulRfInputId(%d), RxRFINPUT_NUM_MAX(%d)\n", ulRfInputId, RxRFINPUT_NUM_MAX);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	hErr = sapi_base_CmdSetRfInputAbility(pstInst, ulRfInputId, bAbility);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("sapi_base_CmdSetRfInputAbility err:\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstInst->stSamaConflictCheck.anRfInputAbility[ulRfInputId] = (HINT32)bAbility;
	pstInst->stSamaConflictCheck.bRfInputAbilitySet[ulRfInputId] = TRUE;


	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_GetSpecOptions (SAPI_SpecOption_t *pstSpec)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();


	if (NULL == pstSpec)
	{
		HxLOG_Error("NULL == pstSpec\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMA_MemCpy(pstSpec, &(pstInst->stSamaConflictCheck.stSpecOption), sizeof(SAPI_SpecOption_t));

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_GetSchedule (HUINT32 ulSlotId, DxSchedule_t *pstSchedule)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	DxSchedule_t	*pstSrcSchedule;
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();


	if (NULL == pstSchedule)
	{
		HxLOG_Error("NULL == pstSchedule\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSrcSchedule = sapi_FindScheduleFromCache(pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("No Schedule with the slot id (%d)\n", ulSlotId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMA_MemCpy(pstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}

HERROR SAPI_GetScheduleByIndex (HUINT32 ulIndex, DxSchedule_t *pstSchedule)
{
	HUINT32			 ulCount = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	DxSchedule_t	*pstSrcSchedule;
	HxList_t		*pstListItem;
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();


	if (NULL == pstSchedule)
	{
		HxLOG_Error("NULL == pstSchedule\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	ulCount = 0;	pstListItem = pstInst->stSamaConflictCheck.pstScheduleList;
	while ((NULL != pstListItem) && (ulCount < ulIndex))
	{
		pstListItem = pstListItem->next;
		ulCount ++;
	}

	pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("Wrong Index(%d)\n", ulIndex);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMA_MemCpy(pstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_GetScheduleByType (DxRsvType_e eType, HUINT32 ulIndex, DxSchedule_t *pstSchedule)
{
	HUINT32			 ulCnt;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HxList_t		*pstListItem;
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();


	if (NULL == pstSchedule)
	{
		HxLOG_Error("NULL == pstSchedule\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("NULL == pstInst\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	for (ulCnt = 0, pstListItem = pstInst->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t	*pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstSrcSchedule) && (eType == pstSrcSchedule->eRsvType))
		{
			if (ulCnt >= ulIndex)
			{
				SAMA_MemCpy(pstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));
				hErr = ERR_OK;
				goto END_FUNC;
			}

			ulCnt ++;
		}
	}

	HxLOG_Error("Wrong Index(%d), Total Count(%d)\n", ulIndex, ulCnt);

	hErr = ERR_FAIL;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_CountSchedule (HUINT32 *pulNumSch)
{
	HUINT32 		 ulCnt = 0;
	sapi_Inst_t 	*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstInst)
	{
		HxLOG_Error("NULL == pstInst\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pulNumSch)
	{
		HxLOG_Error("NULL == pulNumSch\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	ulCnt = HLIB_LIST_Length(pstInst->stSamaConflictCheck.pstScheduleList);

	*pulNumSch = ulCnt;

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}


HERROR SAPI_CountScheduleByType (DxRsvType_e eType, HUINT32 *pulNumSch)
{
	HUINT32			 ulCnt = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HxList_t		*pstListItem;
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstInst)
	{
		HxLOG_Error("NULL == pstInst\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pulNumSch)
	{
		HxLOG_Error("NULL == pulNumSch\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	for (ulCnt = 0, pstListItem = pstInst->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t	*pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstSrcSchedule) && (eType == pstSrcSchedule->eRsvType))
		{
			ulCnt ++;
		}
	}


	*pulNumSch = ulCnt;

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_GetAllSchedulesByRsvType (HUINT32 ulArrayMax, HUINT32 *pulNumSch, DxSchedule_t *pstSchArray, DxRsvType_e eRsvType)
{
	HUINT32			 ulNumSch = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HxList_t		*pstListItem;
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();

	if ((NULL == pstSchArray) || (0 == ulArrayMax))
	{
		HxLOG_Error("(NULL == pstSchArray) || (0 == ulArrayMax)\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	HxLOG_Debug("find eRsvType: %d[%s] \n", eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e),eRsvType));
	for (pstListItem = pstInst->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t	*pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);
		if(NULL != pstSrcSchedule)
		{
			sapi_PrintSchedule(pstSrcSchedule);

			if (eRsvType == pstSrcSchedule->eRsvType)
			{
				HxLOG_Debug("found eRsvType:%d[%s] \n", eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e),eRsvType));
				SAMA_MemCpy(&(pstSchArray[ulNumSch]), pstSrcSchedule, sizeof(DxSchedule_t));

				ulNumSch ++;
				if (ulNumSch >= ulArrayMax)
				{
					break;
				}
			}
		}
	}

	if (NULL != pulNumSch)				{ *pulNumSch = ulNumSch; }

	if(ulNumSch > 0)
	{
		hErr = ERR_OK;
	}
	else
	{
		hErr = ERR_FAIL;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}

// SAMA_SCHEDULE_GetEarliestSchedule 와 병합해야함.
HERROR	SAPI_GetEarliestSchedule (DxSchedule_t *pstSchedule)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	DxSchedule_t		*pstSrcSchedule;
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();


	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL.\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}
	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSrcSchedule = sapi_findEarliestSchedule(&(pstInst->stSamaConflictCheck));

	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("There is No Schedule \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMA_MemCpy(pstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));

	sapi_PrintSchedule(pstSchedule);

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;


}

HERROR	SAPI_GetEarliestScheduleWithPaddingAndReadyTime (DxSchedule_t *pstSchedule, UNIXTIME *putStartTimeWithPaddingAndReady)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	DxSchedule_t		*pstSrcSchedule;
	HERROR			 hErr = ERR_FAIL;
	UNIXTIME		 utStartTimeWithPaddingAndReady = 0;

	HxLOG_Trace();


	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL.\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}
	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSrcSchedule = sapi_findEarliestScheduleWithPaddingAndReadyTime(&(pstInst->stSamaConflictCheck), &utStartTimeWithPaddingAndReady);

	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("There is No Schedule \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMA_MemCpy(pstSchedule, pstSrcSchedule, sizeof(DxSchedule_t));

	sapi_PrintSchedule(pstSchedule);

	hErr = ERR_OK;

END_FUNC:

	if (NULL != putStartTimeWithPaddingAndReady)
	{
		*putStartTimeWithPaddingAndReady = utStartTimeWithPaddingAndReady;
	}

	sapi_ReleaseInstance(pstInst);

	return hErr;


}


HERROR SAPI_GetAllSchedules (HUINT32 ulArrayMax, HUINT32 *pulNumSch, DxSchedule_t *pstSchArray)
{
	HUINT32			 ulNumSch = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HxList_t		*pstListItem;
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();

	if ((NULL == pstSchArray) || (0 == ulArrayMax))
	{
		HxLOG_Error("(NULL == pstSchArray) || (0 == ulArrayMax)\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	for (pstListItem = pstInst->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t	*pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstSrcSchedule)
		{
			SAMA_MemCpy(&(pstSchArray[ulNumSch]), pstSrcSchedule, sizeof(DxSchedule_t));

			ulNumSch ++;
			if (ulNumSch >= ulArrayMax)
			{
				break;
			}
		}
	}

	if (NULL != pulNumSch)				{ *pulNumSch = ulNumSch; }

	if(ulNumSch > 0)
	{
		hErr = ERR_OK;
	}
	else
	{
		hErr = ERR_FAIL;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}



HERROR SAPI_GetAvailableSlot (HUINT32 *pulSlotId)
{
	HUINT32			 ulCount, ulSlotId = SAPI_SLOTID_NULL;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pulSlotId)
	{
		HxLOG_Error("NULL == pulSlotId \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	for (ulCount = 0; ulCount < SAMA_EMPTY_SLOT_PER_PROCESS; ulCount++)
	{
		if (SAPI_SLOTID_NULL != pstInst->aulAvailSlots[ulCount])
		{
			ulSlotId = pstInst->aulAvailSlots[ulCount];
			break;
		}
	}

	*pulSlotId = ulSlotId;

	hErr = (SAPI_SLOTID_NULL != ulSlotId) ? ERR_OK : ERR_FAIL;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;


}

HERROR SAPI_MakeSchedule (HUINT32 ulSlotId, DxSchedule_t *pstSchedule, SAPI_Conflict_t *pstConflict)
{
	HINT32			 nRequestId = 0;
	HUINT32			 ulCount;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;


	HxLOG_Trace();

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstSchedule)
	{
		HxLOG_Error("Input Argument NULL:\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMALIB_SEPC_CheckScheduleSpec(pstSchedule);

	pstSchedule->ulSlot = ulSlotId;

	HxLOG_Debug("Add Schedule:\n");
	sapi_PrintSchedule(pstSchedule);

#if 0
	{
		UNIXTIME		 utCurrTime = HLIB_STD_GetSystemTime();

		HLIB_DATETIME_ConvertUnixTimeToDateTime((utCurrTime + 3 * 60), &(pstSchedule->stStartTime));
	}
#endif

	for (ulCount = 0; ulCount < SAMA_EMPTY_SLOT_PER_PROCESS; ulCount++)
	{
		if (ulSlotId == pstInst->aulAvailSlots[ulCount])
		{
			break;
		}
	}

	if (ulCount >= SAMA_EMPTY_SLOT_PER_PROCESS)
	{
		HxLOG_Error("ulSlotId(%d) is not available\n", ulSlotId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_TvAnyTime:
		{
			HBOOL bExceptConflictCheck = FALSE;
			// alternative를 찾기 위해 sama 안에서 conflict check를 수행한다.
			if ( MACRO_IS_TVA_SCHEDULE_TYPE(pstSchedule->eRsvType))
			{
				bExceptConflictCheck = TRUE;
			}
			if ( bExceptConflictCheck == TRUE )
			{
				UNIXTIME	utStartTime, utPaddingStartTime,utPaddingEndTime;
				HBOOL		bIsScheduled;
				HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);
				(void)SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utPaddingStartTime, &utPaddingEndTime);

				HxLOG_Debug("make schedule without conflict check. \n");

				// tva에서는 sama 내부에서 alternative를 찾기 위해, async call이 된다. (base는 sync)
				// 동일한 event를 예약시에만 예외적으로 아무처리도 하지 않기 위해 체크 한다.
				bIsScheduled = sapi_checkScheduledWithSvcUidAndStartEndTime(&(pstInst->stSamaConflictCheck), pstSchedule->ulSvcUid, utPaddingStartTime, utPaddingEndTime);
				if (bIsScheduled == FALSE){
					hErr = sapi_base_CmdMakeSchedule(pstInst, ulSlotId, pstSchedule, &nRequestId);
					pstInst->aulAvailSlots[ulCount] = SAPI_SLOTID_NULL; 		// 이제 해당 SlotID는 사용하지 못한다.
				}
				else
				{
					HxLOG_Error("Already scheduled. \n");
				}
				goto END_FUNC;
			}
			break;
		}
	default:
		break;
	}

	// Conflict Result를 일단 비워야 한다.
	(void)SAMALIB_CFLT_ClearConflictResult(&(pstInst->stSamaConflictCheck.stConflictResult));

	// Check Conflict:
	pstInst->stSamaConflictCheck.bScheduleChange = (sapi_FindScheduleFromCache(pstInst, pstSchedule->ulSlot) != NULL) ? TRUE : FALSE;
	(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstInst->stSamaConflictCheck), pstSchedule);
#ifndef SAMA_LAZY_PADDING_CONFLICT_REMOVE
	(void)sapi_RemovePadding(pstInst);
#endif

	if (NULL != pstConflict)
	{
		SAMA_MemCpy(pstConflict, &(pstInst->stSamaConflictCheck.stConflictResult), sizeof(SAPI_Conflict_t));
	}

	if (eSAPI_CONFLICT_TYPE_None != pstInst->stSamaConflictCheck.stConflictResult.eType)
	{
		HxLOG_Error("Conflict occurred!!!eType(%d) CftNum(%d)\n", pstInst->stSamaConflictCheck.stConflictResult.eType, pstInst->stSamaConflictCheck.stConflictResult.ulNumConflict);

		switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
		{
		case eDxSchedule_Spec_Base:
		default:
			hErr = sapi_base_CmdInformConflictAddTried(pstInst, &(pstInst->stSamaConflictCheck.stConflictResult), &nRequestId);
			break;
		}

		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	// SAMA에 예약을 넣도록 한다.
	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdMakeSchedule(pstInst, ulSlotId, pstSchedule, &nRequestId);
		break;
	}

	if (ERR_OK == hErr)
	{
		pstInst->aulAvailSlots[ulCount] = SAPI_SLOTID_NULL;			// 이제 해당 SlotID는 사용하지 못한다.
		sapi_PrintSchedule(pstSchedule);
	}


END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;


}

HERROR SAPI_CheckScheduleConflict (HUINT32 ulSlotId, DxSchedule_t *pstSchedule, SAPI_Conflict_t *pstConflict)
{
	HUINT32			 ulCount;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;


	HxLOG_Trace();

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if (NULL == pstSchedule)
	{
		HxLOG_Error("Input Argument NULL:\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	SAMALIB_SEPC_CheckScheduleSpec(pstSchedule);

	pstSchedule->ulSlot = ulSlotId;

	HxLOG_Debug("Check Schedule Conflict:\n");
	sapi_PrintSchedule(pstSchedule);


	// Conflict Result를 일단 비워야 한다.
	(void)SAMALIB_CFLT_ClearConflictResult(&(pstInst->stSamaConflictCheck.stConflictResult));

	for (ulCount = 0; ulCount < SAMA_EMPTY_SLOT_PER_PROCESS; ulCount++)
	{
		if (ulSlotId == pstInst->aulAvailSlots[ulCount])
		{
			break;
		}
	}

	if (ulCount >= SAMA_EMPTY_SLOT_PER_PROCESS)
	{
		HxLOG_Error("ulSlotId(%d) is not available\n", ulSlotId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	// Check Conflict:
	pstInst->stSamaConflictCheck.bScheduleChange = (sapi_FindScheduleFromCache(pstInst, pstSchedule->ulSlot) != NULL) ? TRUE : FALSE;
	(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstInst->stSamaConflictCheck), pstSchedule);

	if (eSAPI_CONFLICT_TYPE_None != pstInst->stSamaConflictCheck.stConflictResult.eType)
	{
		HxLOG_Error("Conflict occurred!!!eType(%d) CftNum(%d)\n", pstInst->stSamaConflictCheck.stConflictResult.eType, pstInst->stSamaConflictCheck.stConflictResult.ulNumConflict);
	}
	else
	{
		HxLOG_Debug("no conflict \n");
	}


	if (NULL != pstConflict)
	{
		SAMA_MemCpy(pstConflict, &(pstInst->stSamaConflictCheck.stConflictResult), sizeof(SAPI_Conflict_t));
	}

	hErr = ERR_OK;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;


}

HERROR SAPI_UpdateSchedule (HUINT32 ulSlotId, DxSchedule_t *pstSchedule, SAPI_Conflict_t *pstConflict, HBOOL bDisableConflictCheck, HBOOL bDisableConflictNotify)
{
	HINT32			 nRequestId = 0;
	HxList_t		*pstListItem;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();

	sapi_PrintSchedule(pstSchedule);

	//SAMALIB_SEPC_CheckScheduleSpec(pstSchedule);// schedule load 시 이미 처리.

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstListItem = HLIB_LIST_FindEx(pstInst->stSamaConflictCheck.pstScheduleList, (const void *)ulSlotId, sapi_cbCmpScheduleBySlotId);
	if (NULL == pstListItem)
	{
		HxLOG_Error("No Schedule with the slot id (%d)\n", ulSlotId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	if(!bDisableConflictCheck)
	{
		// Conflict Result를 일단 비워야 한다.
		(void)SAMALIB_CFLT_ClearConflictResult(&(pstInst->stSamaConflictCheck.stConflictResult));

		// Check Conflict:
		pstSchedule->ulSlot = ulSlotId;

		pstInst->stSamaConflictCheck.bScheduleChange = (sapi_FindScheduleFromCache(pstInst, pstSchedule->ulSlot) != NULL) ? TRUE : FALSE;
		(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstInst->stSamaConflictCheck), pstSchedule);

		// SAPI_UpdateSchedule 안에서 SAPI_UpdateSchedule를 call 하는 좋지 않은 구조.
#ifndef SAMA_LAZY_PADDING_CONFLICT_REMOVE
		(void)sapi_RemovePadding(pstInst);
#endif

		if (NULL != pstConflict)				{ SAMA_MemCpy(pstConflict, &(pstInst->stSamaConflictCheck.stConflictResult), sizeof(SAPI_Conflict_t)); }

		if (eSAPI_CONFLICT_TYPE_None != pstInst->stSamaConflictCheck.stConflictResult.eType)
		{
			HxLOG_Error("Conflict occurred!!!eType(%d) CftNum(%d)\n", pstInst->stSamaConflictCheck.stConflictResult.eType, pstInst->stSamaConflictCheck.stConflictResult.ulNumConflict);

			if(!bDisableConflictNotify)
			{
				switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
				{
				case eDxSchedule_Spec_Base:
				default:
					hErr = sapi_base_CmdInformConflictAddTried(pstInst, &(pstInst->stSamaConflictCheck.stConflictResult), &nRequestId);
					if (hErr != ERR_OK)
					{
						HxLOG_Debug("sapi_base_CmdInformConflictAddTried failed. \n");
					}
					break;
				}

			}
			hErr = ERR_FAIL;
			goto END_FUNC;
		}
	}

	// SAMA에 예약을 넣도록 한다.
	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdUpdateSchedule(pstInst, ulSlotId, pstSchedule, &nRequestId);
		break;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}

HERROR SAPI_CancelSchedule (HUINT32 ulSlotId)
{
	HINT32			 nRequestId = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;
	DxSchedule_t	*pstSrcSchedule;

	HxLOG_Debug("ulSlotId: %d \n", ulSlotId);

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSrcSchedule = sapi_FindScheduleFromCache(pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("No Schedule with the slot(%d) exists:\n", ulSlotId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdCancelSchedule(pstInst, ulSlotId, &nRequestId);
		break;
	}

	pstSrcSchedule->bRemoving = TRUE;

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

/*
* This API SHOULD BE used only when resolving conflict with series schedule.
*/
HERROR SAPI_ChangeSeriesScheduleToNext (HUINT32 ulSlotId)
{
	HINT32			 nRequestId = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;
	DxSchedule_t	*pstSrcSchedule;

	HxLOG_Debug("ulSlotId: %d \n", ulSlotId);

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSrcSchedule = sapi_FindScheduleFromCache(pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("No Schedule with the slot(%d) exists:\n", ulSlotId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_TvAnyTime:
		hErr = sapi_base_CmdCancelSchedule(pstInst, ulSlotId, &nRequestId);
		break;
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdChangeSeriesScheduleToNext(pstInst, ulSlotId, &nRequestId);
		break;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}

HERROR SAPI_CancelWholeSeries (HUINT32 ulSlotId)
{
	HINT32			 nRequestId = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;
	DxSchedule_t	*pstSrcSchedule;

	HxLOG_Debug("ulSlotId: %d \n", ulSlotId);

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSrcSchedule = sapi_FindScheduleFromCache(pstInst, ulSlotId);
	if (NULL == pstSrcSchedule)
	{
		HxLOG_Error("No Schedule with the slot(%d) exists:\n", ulSlotId);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_TvAnyTime:
		hErr = sapi_tva_CmdCancelWholeSeries(pstInst, ulSlotId, &nRequestId);
		break;

	default:
		hErr = sapi_base_CmdCancelSchedule(pstInst, ulSlotId, &nRequestId);
		break;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}


HERROR SAPI_StopSchedule (HUINT32 ulSlotId)
{
	HINT32			 nRequestId = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Debug("ulSlotId: %d \n", ulSlotId);

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdStopSchedule(pstInst, ulSlotId, &nRequestId);
		break;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;
}

HERROR SAPI_PauseSchedule (HUINT32 ulSlotId)
{
	HINT32			 nRequestId = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Debug("ulSlotId: %d \n", ulSlotId);

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdPauseSchedule(pstInst, ulSlotId, &nRequestId);
		break;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_ResumeSchedule (HUINT32 ulSlotId)
{
	HINT32			 nRequestId = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Debug("ulSlotId: %d \n", ulSlotId);

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdResumeSchedule(pstInst, ulSlotId, &nRequestId);
		break;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_IncompleteSchedule (HUINT32 ulSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo)
{
	HINT32			 nRequestId = 0;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_Base:
	default:
		hErr = sapi_base_CmdIncompleteSchedule(pstInst, ulSlotId, pstRemovedInfo, &nRequestId);
		break;
	}

END_FUNC:
	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_GetConflictResult (HUINT32 ulSlotId, SAPI_Conflict_t *pstConflict)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HERROR			 hErr = ERR_FAIL;

	hErr = sapi_GetConflictResult(pstInst, ulSlotId, pstConflict);

	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_ChangePaddingTime(HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
{
	HINT32					nRequestId = 0;
	HERROR					hErr = ERR_FAIL;
	sapi_Inst_t 				*pstInst = sapi_GetInstance();
#ifdef SAMA_CHANGE_PADDING_TIME_IN_SAMA
#else
	HxList_t			*pstListItem;
	DxSchedule_t		*pstSchedule;
	HBOOL			bEnableTBRPadding = FALSE;
#endif


	HxLOG_Info("ulOffsetTime: %d, bIsPreOffset: %d \n", ulOffsetTime, bIsPreOffset);

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

#ifdef SAMA_CHANGE_PADDING_TIME_IN_SAMA
	hErr=sapi_base_CmdChangePaddingTime (pstInst, ulOffsetTime, bIsPreOffset, &nRequestId);
#else
	if(SAPI_IsEnableTBRPadding()==TRUE)
	{
		bEnableTBRPadding = TRUE;
	}

	for (pstListItem = pstInst->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);
		if (NULL == pstSchedule)
		{
			HxLOG_Error("null schedule \n");
			continue;
		}
		switch(pstSchedule->eRsvType)
		{
			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_SERIES_EBR:
			case DxRSVTYPE_RECORD_TBR:
				if(pstSchedule->eRsvType == DxRSVTYPE_RECORD_TBR && bEnableTBRPadding == FALSE)
				{
					// 일반적인 경우에는 TBR은 SKIP...
					break;
				}

				if(bIsPreOffset == TRUE)
				{
					pstSchedule->uExtInfo.stRec.ulPreOffsetTime = ulOffsetTime;
				}
				else
				{
					pstSchedule->uExtInfo.stRec.ulPostOffsetTime = ulOffsetTime;
				}

#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
				switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
				{
					case eDxSchedule_Spec_Base:
					default:
						hErr = sapi_base_CmdUpdateSchedule(pstInst, pstSchedule->ulSlot, pstSchedule, &nRequestId);
						break;
				}
#else
				pstInst->stSamaConflictCheck.bScheduleChange = TRUE;
				(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstInst->stSamaConflictCheck), pstSchedule);

				HxLOG_Debug("stSamaConflictCheck.stConflictResult.eType: %d \n", pstInst->stSamaConflictCheck.stConflictResult.eType);

				if (eSAPI_CONFLICT_TYPE_None == pstInst->stSamaConflictCheck.stConflictResult.eType)
				{
					switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
					{
					case eDxSchedule_Spec_Base:
					default:
						hErr = sapi_base_CmdUpdateSchedule(pstInst, pstSchedule->ulSlot, pstSchedule, &nRequestId);
						break;
					}

					// SAPI_ChangePaddingTime 안에서 SAPI_UpdateSchedule를 call 하는 좋지 않은 구조.
					(void)sapi_RemovePadding(pstInst);
				}
				break;
#endif
			default:
				break;
		}
	}
#endif
END_FUNC:

	sapi_ReleaseInstance(pstInst);

	return hErr;

}

HERROR SAPI_GetAlternativeResult (HUINT32 ulSlotId, SAPI_Alternative_t *pstAlternative)
{
	sapi_Inst_t				*pstInst;
	SAPI_AlternativeItems_t	*pstSrcAltItems;
	HERROR					hErr;

	if ( pstAlternative == NULL )
	{
		HxLOG_Error("Invalid Argument. NULL Alternative. Slot=(%d)\n",ulSlotId);
		return ERR_FAIL;
	}

	pstInst = sapi_GetInstance();

	hErr = sapi_GetConflictResult(pstInst, ulSlotId, &(pstAlternative->stSapiConflict));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("GetConflictResult failed. slot=(%d)\n",ulSlotId);
	}

	pstSrcAltItems	= &(pstInst->stSamaConflictCheck.stAltItems);
	HxSTD_MemCopy(&(pstAlternative->stAltItems), pstSrcAltItems, sizeof(SAPI_AlternativeItems_t));

	sapi_ReleaseInstance(pstInst);

	return hErr;
}

HERROR SAPI_UpdateSeries (void)
{
	sapi_Inst_t 				*pstInst = sapi_GetInstance();
	HERROR						hErr = ERR_OK;
	HINT32			 			nRequestId = 0;

	if (NULL == pstInst)
	{
		HxLOG_Error("sapiInst NULL: sapi_GetInstance err\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	switch (pstInst->stSamaConflictCheck.stSpecOption.eSpec)
	{
	case eDxSchedule_Spec_TvAnyTime:
		hErr = sapi_tva_CmdUpdateSeries(pstInst,&nRequestId);
        break;
	case eDxSchedule_Spec_Base:
	default:
		break;
	}

END_FUNC:

	sapi_ReleaseInstance(pstInst);
	return hErr;
}

HERROR SAPI_GetValidCrid (HUINT8 *szSrcCridStr, HUINT32 ulSrcCridLen, HUINT8 *szDefaultAuth, HUINT8 *szValidCridStr)
{
	TVA_CONTENTID_T stCrid;
	HERROR			hErr;

	if ( szSrcCridStr == NULL
		|| szValidCridStr == NULL )
	{
		HxLOG_Error("szValidCridStr must be mem alloced. \n");
		return ERR_FAIL;
	}

	hErr = SAMALIB_TVA_ExtractEachPartCRID(szSrcCridStr,ulSrcCridLen,&stCrid,(int*)szDefaultAuth);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("extract crid failed. cridstr=[%s], auth=[%s]\n", szSrcCridStr, szDefaultAuth);
		return ERR_FAIL;
	}

	if(strlen((char *)stCrid.auth) <= 0 || strlen((char *)stCrid.data) <= 0)
	{
		HxLOG_Error("Invalid CRID structure. cridstr=[%s], auth=[%s]\n",szSrcCridStr, szDefaultAuth);
		return ERR_FAIL;
	}

	if ( stCrid.imi != 0 )
	{
		HxSTD_snprintf((HCHAR*)szValidCridStr, DxRSV_CRID_LEN+1, "%s%s%s", (char*)stCrid.auth, (char*)stCrid.data,(char*)&stCrid.imi);
	}
	else
	{
		HxSTD_snprintf((HCHAR*)szValidCridStr, DxRSV_CRID_LEN+1, "%s%s", (char*)stCrid.auth, (char*)stCrid.data);
	}
	HxLOG_Debug("valid crid:[%s]\n",szValidCridStr);
	return ERR_OK;
}

HUINT32 SAPI_GetExtraBootTime(void)
{
	return SAMALIB_SPEC_GetExtraBootTime();
}

HBOOL	SAPI_IsEnableTBRPadding(void)
{
	return SAMALIB_SPEC_IsEnableTBRPadding();
}

void	SAPI_FindScheduleFilterInit(DxSchedule_FindFilterAttr_t	*pstFilter)
{
	SAMALIB_CFLT_FindScheduleListFilterInit(pstFilter);
}

// 이 함수 call 하면 SAPI_FindScheduleUsingFilterFreeList()로 free 해줘야 함.!!
HERROR	SAPI_FindScheduleUsingFilter(DxSchedule_FindFilterAttr_t *pstFilter, HUINT32 *pulNumSch, DxSchedule_t **ppstSchArray)
{
	HxList_t			*pstFoundList = NULL;
	HUINT32 		 ulNumSch = 0;
	HxList_t		*pstListItem;
	HUINT32		ulListLength = 0;
	HERROR		hErr = ERR_FAIL;
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	DxSchedule_t *pstSchArray = NULL;

	HxLOG_Trace();


	if (pstFilter == NULL || NULL == pulNumSch || NULL == ppstSchArray)
	{
		HxLOG_Error("invalid parameter \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}


	(void)SAMALIB_CFLT_FindScheduleListUsingFilter(pstInst->stSamaConflictCheck.pstScheduleList, pstFilter, &pstFoundList);

	if(pstFoundList == NULL)
	{
		HxLOG_Debug("there is no schedule list satisfing filter \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	ulListLength = HLIB_LIST_Length(pstFoundList);
	if(ulListLength == 0)
	{
		HxLOG_Error("list length is 0 although the list exists! \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)SAMA_Calloc(sizeof(DxSchedule_t) * ulListLength);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("memory allocation failed\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}


	for (pstListItem = pstFoundList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxSchedule_t	*pstSrcSchedule = (DxSchedule_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstSrcSchedule)
		{
			sapi_PrintSchedule(pstSrcSchedule);

			SAMA_MemCpy(&(pstSchArray[ulNumSch]), pstSrcSchedule, sizeof(DxSchedule_t));

			ulNumSch ++;
		}
	}

	*pulNumSch = ulNumSch;
	*ppstSchArray = pstSchArray;

	hErr = (ulNumSch > 0) ? ERR_OK : ERR_FAIL;

END_FUNC:

	sapi_ReleaseInstance(pstInst);

	if(pstFoundList != NULL)
	{
		HLIB_LIST_RemoveAll(pstFoundList);
	}

	return hErr;


}

void	SAPI_FindScheduleUsingFilterFreeList(DxSchedule_t *pstSchArray)
{
	if(pstSchArray != NULL)
	{
		SAMA_Free(pstSchArray);
		pstSchArray = NULL;
	}
}

HERROR SAPI_GetPaddingStartEndTime(DxSchedule_t *pstSchedule, UNIXTIME *putStartTime, UNIXTIME *putEndTime)
{
	UNIXTIME utStartTime;

	if(pstSchedule == NULL || putStartTime == NULL || putEndTime == NULL)
	{
		HxLOG_Error("invalid parameter \n");
		return ERR_FAIL;
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);

	return SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, putStartTime, putEndTime);
}

SAPI_ActionType_e SAPI_GetSapiActionTypeBySchedule (DxSchedule_t *pstSchedule)
{
	return SAMALIB_CFLT_GetSapiActionTypeBySchedule(pstSchedule);

}

HBOOL SAPI_IsScheduleLoaded(void)
{
	sapi_Inst_t		*pstInst = sapi_GetInstance();
	HBOOL					 bScheduleLoaded;

	HxLOG_Trace();

	bScheduleLoaded = pstInst->bScheduleLoaded;

	sapi_ReleaseInstance(pstInst);

	return bScheduleLoaded;
}

void SAPI_RefreshScheduleData (void)
{
	sapi_Inst_t			*pstInst = sapi_GetInstance();
	SAPI_NotiInfo_t		 unNotiInfo;

	HxLOG_Trace();

	if(pstInst->bScheduleLoaded == TRUE)
	{
		HUINT32			 ulCount = 0, ulSchNum = HLIB_LIST_Length(pstInst->stSamaConflictCheck.pstScheduleList);
		DxSchedule_t	*pstSchArray = (ulSchNum > 0) ? SAMA_Malloc(sizeof(DxSchedule_t) * ulSchNum) : NULL;

		if ((ulSchNum > 0) && (NULL != pstSchArray))
		{
			HxList_t		*pstListItem;

			for (pstListItem = pstInst->stSamaConflictCheck.pstScheduleList; NULL != pstListItem; pstListItem = pstListItem->next)
			{
				DxSchedule_t		*pstSchedule = HLIB_LIST_Data(pstListItem);

				if (NULL != pstSchedule)
				{
					SAMA_MemCpy(&(pstSchArray[ulCount]), pstSchedule, sizeof(DxSchedule_t));
					ulCount ++;
				}
			}
		}

		unNotiInfo.stSchLoaded.eNotifyType	= eSAPI_NOTIFY_TYPE_SchedulesLoaded;
		unNotiInfo.stSchLoaded.ulSchNum		= ulCount;
		unNotiInfo.stSchLoaded.pstSchArray	= pstSchArray;
		unNotiInfo.stSchLoaded.nRequestPid	= HLIB_STD_GetPID();

		sapi_ExecuteNotifiers (pstInst, &unNotiInfo);

		if (NULL != pstSchArray)
		{
			SAMA_Free(pstSchArray);
		}

	}
	else
	{
		(void)sapi_base_CmdRefreshScheduleData(pstInst);
	}


	sapi_ReleaseInstance(pstInst);

	return ;
}

/*
* timeout : millisecond (3000 -> 3 seconds)
*/
HERROR	SAPI_Wait (HUINT32 ulSlotId, HUINT32 ulTimeout)
{
	sapi_Inst_t			*pstInst = NULL;
	HUINT32				ultick = 0;
	HUINT32 			ulElapsedTime = 0;
	DxSchedule_t		*pstSchedule = NULL;

	HxLOG_Debug("(ulSlotId: %d, ulTimeout:%u)\n", ulSlotId, ulTimeout);

	ultick = HLIB_STD_GetSystemTick();
	for (;;)
	{
		pstInst = sapi_GetInstance();
		if (pstInst != NULL)
		{
			pstSchedule = sapi_FindScheduleFromCache(pstInst, ulSlotId);
			if (pstSchedule != NULL)
			{
				sapi_ReleaseInstance(pstInst);

				ulElapsedTime = HLIB_STD_GetSystemTick() - ultick;
				if (ulElapsedTime < 1000)
				{
					HxLOG_Debug("ulElapsedTime: (%u)milliseconds done!!\n",  ulElapsedTime);
				}
				else if (ulElapsedTime < 3000)
				{
					HxLOG_Warning("ulElapsedTime: (%u)milliseconds done!!\n",  ulElapsedTime);
				}
				else
				{
					HxLOG_Error("ulElapsedTime: (%u)milliseconds done!!\n",  ulElapsedTime);
				}
				HxLOG_Debug("(ulSlotId: %d, ulTimeout:%u) success. \n",ulSlotId ,ulTimeout);

				return ERR_OK;
			}
			sapi_ReleaseInstance(pstInst);
		}
		else
		{
			return ERR_FAIL;
		}

		if (HLIB_STD_GetSystemTick() - ultick > ulTimeout)
		{
			break;
		}

		HLIB_STD_TaskSleep(10);
	}
	HxLOG_Error("(ulSlotId: %d ulTimeout:%u) Timeout !!\n",ulSlotId ,ulTimeout);

	return ERR_FAIL;
}
#ifdef CONFIG_DEBUG
HERROR SAPI_DbgPrintSchedule(DxSchedule_t *pstSchedule)
{
	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule is NULL \n");
		return ERR_FAIL;
	}
	sapi_PrintSchedule(pstSchedule);
	return ERR_OK;
}
#endif


