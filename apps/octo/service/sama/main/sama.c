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
#include <vkernel.h>
#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <uapi.h>
#include <oapi.h>

#include <sapi.h>

#include "sama_int.h"
#include <sama_lib.h>

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
#define SAMA_MSGQ_SIZE			1024
#define SAMA_MSG_QUE_NAME		"SamaMsgQ"
#define SAMA_MAIN_TASK_NAME		"samaMainTask"
#define SAMA_TASK_STACK_SIZE 				SIZE_64K

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32			 ulNumSvcUid;
	HUID			*pulSvcUidList;

	HUINT32			 ulNumSch;
	DxSchedule_t	*pstSchArray;
} sama_DbSvcUpdated_t;

#if 0
typedef struct
{
	HxQueue_t		*pJobQueue; 	///< job queue
	struct
	{
		HxQueue_t	*pJob;
	} stPool;


} sama_MainTaskManager_t;
#endif

typedef struct
{
	HUINT32				 ulSemId;
	HINT32				 nSocket;
	HINT32				 nRpcHandle;

	HxQueue_t		*pJobQueue;		///< job queue
}Sama_RpcProtocol_t;


STATIC HULONG					s_ulSamaMainMsgQId;
STATIC HULONG					s_ulSamaMainTaskId;


STATIC void sama_serverListener (void *pvArg);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC SAMA_Context_t	 s_stSama_Context;

#define ___LOCAL_FUNCTIONS___
STATIC HERROR sama_loadSpec (SAMA_Context_t *pstContext)
{
	//ToDo: 추후 File을 읽어서 Spec을 정하도록 수정한다.

	pstContext->stSpecOption.eSpec = SAMALIB_SPEC_Get();
	pstContext->stSpecOption.ulShutdownWaitTime = SAMALIB_SPEC_GetExtraBootTime();

	return ERR_OK;
}

STATIC Sama_RpcProtocol_t* sama_RpcProtocol_GetInstance(void)
{
	static Sama_RpcProtocol_t *s_pSamaRpc = NULL;

	if(s_pSamaRpc == NULL)
	{
		unsigned long		 ulTaskId;

		s_pSamaRpc = (Sama_RpcProtocol_t *)HLIB_STD_MemCalloc(sizeof(Sama_RpcProtocol_t));
		HxLOG_Assert(s_pSamaRpc);

		(void)HxSEMT_Create (&(s_pSamaRpc->ulSemId), "sama_sem", 0);
		s_pSamaRpc->nSocket 	= HLIB_EXTRA_OpenServer("sama");//HLIB_SOCKET_OpenServer (HxSOCKET_UDS, SAMA_SIGNAL_UDS);
		s_pSamaRpc->nRpcHandle	= HLIB_RPC_Open (s_pSamaRpc->nSocket, HLIB_STD_GetPID());

		s_pSamaRpc->pJobQueue = HLIB_QUEUE_New(NULL);
		HxLOG_Assert(s_pSamaRpc->pJobQueue);

		// Listener Thread Create:
		(void)VK_TASK_Create (sama_serverListener, 65, (64 * 1024), "sama_listen", (void *)s_pSamaRpc, &ulTaskId, 0);
		(void)VK_TASK_Start (ulTaskId);

	}

	return s_pSamaRpc;
}

STATIC HERROR sama_initContext (void)
{
	HUINT32				 ulCnt;
	SAMA_Context_t		*pstContext = SAMA_GetContext();

	SAMA_MemSet (pstContext, 0, sizeof(SAMA_Context_t));

	pstContext->nSamaPid = HLIB_STD_GetPID();


	for (ulCnt = 0; ulCnt < RxRFINPUT_NUM_MAX; ulCnt++)
	{
		pstContext->stSamaConflictCheck.anRfInputAbility[ulCnt] = -1;
	}

	(void)sama_loadSpec (pstContext);

	return ERR_OK;
}

STATIC void sama_schedule_CbFreeSeriesRecordEventlist (void *pvData)
{
	Sama_SeriesRecordEventInfo_t		*pstSeriesRecordEventInfo = (Sama_SeriesRecordEventInfo_t*)pvData;

	if(pstSeriesRecordEventInfo != NULL)
	{
		SAMA_Free(pstSeriesRecordEventInfo);
		pstSeriesRecordEventInfo = NULL;
	}

	return;
}

#define _____SERVER_LISTENER_____
STATIC HINT32	sama_RpcProtocol_ReadAndDispatch (HINT32 nCount)
{
	Sama_RpcProtocolJob_t		*pJob;
	HINT32					nDispatched = 0;
	Sama_RpcProtocol_t		*pstSamaRpc = sama_RpcProtocol_GetInstance();

	if(pstSamaRpc == NULL)
	{
		goto END_FUNC;
	}


	if(pstSamaRpc->pJobQueue == NULL)
	{
		goto END_FUNC;
	}


	while (nCount--)
	{
		HxSEMT_Get(pstSamaRpc->ulSemId);

		pJob = (Sama_RpcProtocolJob_t *)HLIB_QUEUE_Pop(pstSamaRpc->pJobQueue);

		if (pJob == NULL)
		{
			HxSEMT_Release(pstSamaRpc->ulSemId);
			continue;
		}

		(void)HLIB_RPC_ProcessRPC (pstSamaRpc->nRpcHandle, pJob->ulSize, pJob->pvData, pJob->ulRequestId, pJob->nClientSock);

		if (pJob->pvData)
		{
			HLIB_STD_MemFree(pJob->pvData);
		}
		HLIB_STD_MemFree(pJob);

		nDispatched++;

		HxSEMT_Release(pstSamaRpc->ulSemId);
	}

END_FUNC:
	return nDispatched;


}

STATIC void sama_onDataReceived (void *pvUserdata, HINT32 nClientSock, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	Sama_RpcProtocol_t		*pSamaRPC = (Sama_RpcProtocol_t *)pvUserdata;

	Sama_RpcProtocolJob_t	*pJob;

	HxSEMT_Get(pSamaRPC->ulSemId);

	pJob = (Sama_RpcProtocolJob_t *)HLIB_STD_MemCalloc(sizeof(Sama_RpcProtocolJob_t));
	if (pJob == NULL)
	{
		HxLOG_Error("cannot alloc the job! \n");

		HxSEMT_Release(pSamaRPC->ulSemId);
		return;
	}

	pJob->ulSize	 = nSize;
	if(nSize > 0)
	{
		pJob->pvData = HLIB_STD_MemCalloc(nSize);
		if(pvData != NULL)
		{
			HxSTD_MemCopy(pJob->pvData, pvData, nSize);
		}
	}
	pJob->ulRequestId	  = ulRequestId;
	pJob->nClientSock  = nClientSock;


	HLIB_QUEUE_Push(pSamaRPC->pJobQueue, pJob);

	HxSEMT_Release(pSamaRPC->ulSemId);

	SAMA_PostMessage(SAMAMESSAGE_RPCCALLRECEIVED, 0, 0, 0, 0, 0, 0);

	// 절대 여기서 처리하지 말것.
//	(void)HLIB_RPC_ProcessRPC (pstContext->nRpcHandle, (HUINT32)nSize, pvData, ulRequestId, nClientSock);
}

STATIC void		sama_RpcProtocol_ClientDisconnected (Sama_RpcProtocol_t *pSamaRPC, HINT32 nSock)
{
	HxQueue_t					*pTempQ;
	Sama_RpcProtocolJob_t 		*pJob;

	HxLOG_Debug("%s(0x%X)\n", __FUNCTION__, nSock);

	HxSEMT_Get(pSamaRPC->ulSemId);

	pTempQ = HLIB_QUEUE_New(NULL);
	if (pTempQ)
	{
		while ((pJob = (Sama_RpcProtocolJob_t *)HLIB_QUEUE_Pop(pSamaRPC->pJobQueue)) != NULL)
		{
			if (pJob->nClientSock != nSock)
			{
				HLIB_QUEUE_Push(pTempQ, pJob);
				continue;
			}

			if (pJob->pvData)
			{
				HLIB_STD_MemFree(pJob->pvData);
			}
			HLIB_STD_MemFree(pJob);
		}
		HLIB_QUEUE_Delete(pSamaRPC->pJobQueue);
		pSamaRPC->pJobQueue = pTempQ;
	}

	HxSEMT_Release(pSamaRPC->ulSemId);

}

STATIC void sama_onStatusChanged (void *pvUserdata, HxSocket_Status_e eStatus, HINT32 nClientSock)
{
	Sama_RpcProtocol_t		*pSamaRPC = (Sama_RpcProtocol_t *)pvUserdata;

	switch (eStatus)
	{
	case HxSOCKET_REMOVE_CLIENT:
		sama_RpcProtocol_ClientDisconnected(pSamaRPC, nClientSock);
		HLIB_RPC_RemoveClientHandle (pSamaRPC->nRpcHandle, nClientSock);
		break;

	default:
		break;
	}
}

STATIC void sama_serverListener (void *pvArg)
{
	Sama_RpcProtocol_t		*pSamaRPC = (Sama_RpcProtocol_t *)pvArg;

	(void)HLIB_SOCKET_Listen (pSamaRPC->nSocket, TRUE, sama_onDataReceived, sama_onStatusChanged, (void *)pSamaRPC);
}





#define ___SAMA_MAIN_TASK___

STATIC void		sama_MainTask_ProcessMessage(Sama_InternalMsg_t *msg)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	DxSchedule_t	*pstSchedule = NULL;

	switch(msg->message)
	{
		case SAMAMESSAGE_RPCCALLRECEIVED:
			sama_RpcProtocol_ReadAndDispatch(1);
			break;

		case SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL:
			pstSchedule = (DxSchedule_t*)(msg->p1);
			if(pstSchedule != NULL)
			{
				SAMA_SCHEDULE_ReleaseSlotForSapi(pstSchedule->ulSlot);
				SAMA_Free(pstSchedule);
				pstSchedule = NULL;
			}
			break;
		case SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE:
			pstSchedule = (DxSchedule_t*)(msg->p1);
			if(pstSchedule != NULL)
			{
				SAMA_Free(pstSchedule);
				pstSchedule = NULL;
			}
			break;

		case SAMAMESSAGE_CHECK_SERIES_RECORD:
			HxLOG_Debug("receive SAMAMESSAGE_CHECK_SERIES_RECORD \n");
			pstSchedule = (DxSchedule_t*)(msg->p1);
			if(pstSchedule != NULL)
			{
				HxLOG_Debug("find current series record info ulSlot: %d \n", pstSchedule->ulSlot);
				SAMA_SCHEDULE_FindCurrentSeriesRecordInfo(pstSchedule, msg->p2);
			}
			else
			{
				HxLOG_Error("pstSchedule is NULL \n");
			}
			break;

#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST
		case SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED:
			HxLOG_Debug("receive SAMAMESSAGE_CHECKSAMESERIESRECORDEXIST \n");
			pstSchedule = (DxSchedule_t*)(msg->p1);
			if(pstSchedule != NULL)
			{
				SAMA_SCHEDULE_CheckSameSeriesRecordExist(pstSchedule, msg->p2, msg->p3, msg->p4);
			}
			else
			{
				HxLOG_Error("pstSchedule is NULL \n");
			}
			break;

		case SAMAMESSAGE_CHECK_SAME_SERIES_RECORD_EXIST_RESULT:
			HxLOG_Debug("receive SAMAMESSAGE_CHECK_SAME_SERIES_RECORD_EXIST_RESULT, ulSeason: %d, ulEpisodeNum: %d, ulTotalNum: %d, bExistSameSeriesItem: %d \n", msg->p2, msg->p3, msg->p4, msg->p5);
			pstSchedule = (DxSchedule_t*)(msg->p1);
			if(pstSchedule != NULL)
			{
				HUINT32 ulSeriesCnt;

				if(msg->p5 == TRUE)
				{
					DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

					stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_SameSeriesRecItemExist;
					SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
				}

				ulSeriesCnt = SAMA_SCHEDULE_CountSeries(pstSchedule);
				HxLOG_Debug("series count: %d \n", ulSeriesCnt);
				if (ulSeriesCnt == 0)
				{
					SAMA_SCHEDULE_GetNextSeriesRecord(pstSchedule, msg->p2, msg->p3, msg->p4, msg->p5);
				}
				else
				{
					HxLOG_Debug("send SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \n");
					SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
				}


			}
			else
			{
				HxLOG_Error("pstSchedule is NULL \n");
			}
			break;
#else
		case SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED:
			HxLOG_Debug("receive SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED \n");
			pstSchedule = (DxSchedule_t*)(msg->p1);
			if(pstSchedule != NULL)
			{
				SAMA_SCHEDULE_GetNextSeriesRecord(pstSchedule, msg->p2, msg->p3, msg->p4, FALSE);
			}
			else
			{
				HxLOG_Error("pstSchedule is NULL \n");
			}
			break;
#endif

		case SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM:
			HxLOG_Debug("receive SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM, bExistSameSeriesItem: %d \n", msg->p3);
			pstSchedule = (DxSchedule_t*)(msg->p1);
			if(pstSchedule != NULL)
			{
				HUINT32 ulSeriesCnt = SAMA_SCHEDULE_CountSeries(pstSchedule);
				HxLOG_Debug("series count: %d \n", ulSeriesCnt);
				if (ulSeriesCnt == 0)
				{
					SAMA_SCHEDULE_MakeNextSeriesRecord(pstSchedule, (HxList_t*)msg->p2, msg->p3);
				}
				else
				{
					HxList_t	*pstSeriesEventList = (HxList_t *)msg->p2;
					if (pstSeriesEventList != NULL)
					{
						HLIB_LIST_RemoveAllFunc(pstSeriesEventList, sama_schedule_CbFreeSeriesRecordEventlist);
					}
					SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
				}
			}
			else
			{
				HxLOG_Error("pstSchedule is NULL \n");
			}
			break;

		case SAMAMESSAGE_CHECK_SERIES_DONE:
			HxLOG_Debug("receive SAMAMESSAGE_CHECK_SERIES_DONE \n");
			{
				// 영국향에서는 current series event를 받아오면, 이 event에 대한 service 정보도 필요해서 메세지를 따로 돌아야 한다.
				HUINT32 			ulSeriesIndex = (HUINT32)(msg->p2);
				DxEvent_t			*pstCurrentSeriesEvent = (DxEvent_t *)(msg->p3);
				pstSchedule 			  = (DxSchedule_t*)(msg->p1);

				(void)SAMA_SCHEDULE_AddCurrentSeriesEvent(pstSchedule, ulSeriesIndex, pstCurrentSeriesEvent);
			}
			break;

		case SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED_DONE:
			HxLOG_Debug("receive SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED_DONE \n");
			{
				// 영국향에서는 series event들을 받아오면, 이 event 들에 대한 service 정보도 필요해서 메세지를 따로 돌아야 한다.
				HUINT32 			ulSeriesIndex = (HUINT32)(msg->p2);
				SERIES_SET			*pstSeriesSet = (SERIES_SET *)(msg->p3);
				HxList_t			*pstSeriesEventList = (HxList_t *)(msg->p4);
				pstSchedule 			  = (DxSchedule_t*)(msg->p1);


				(void)SAMA_SCHEDULE_AddSeriesEvents(pstSchedule, ulSeriesIndex, pstSeriesSet,pstSeriesEventList);

			}
			break;

		case SAMAMESSAGE_CHECKPVRREADYSTATUS:
			HxLOG_Debug("receive SAMAMESSAGE_CHECKPVRREADYSTATUS \n");
			{
				// device의 연결/해제, 혹은 Format으로 인해 PVR device 정보가 바뀌었을때 발생
				HBOOL	bPvr = (HBOOL)msg->p1;	//TRUE : PVR storage, FALSE : Not a PVR storage (disable or unpaired)
				//HINT32	eDevIdx = (HINT32)msg->p2;
				//HUINT32	ulPvrIdx =(HUINT32)msg->p3;
				HUINT32 			 ulSchCnt = 0, ulSchNum = 0;
				DxSchedule_t		*pstSchArray = NULL;
				HERROR				 hErr;
				DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
				UNIXTIME		ulCurrTime = 0;
				UNIXTIME		ulStartTime = 0;
				HCHAR			*szTimeStr;

				pstContext->bSama_PvrDeviceMounted = bPvr;
				HxLOG_Warning("PvrReady : %d \n", bPvr);

				if(pstContext->bSama_PvrDeviceMounted == TRUE)
				{
					hErr = SAMA_SCHEDULE_GetAllSchedules(&ulSchNum, &pstSchArray);
					if ((ERR_OK != hErr) || (0 == ulSchNum) || (NULL == pstSchArray))
					{
						if (NULL != pstSchArray)					{ SAMA_Free(pstSchArray); }
						return;
					}

					HxLOG_Warning("All keeping removed schedules shall be deleted...\n");

					for (ulSchCnt = 0; ulSchCnt < ulSchNum; ulSchCnt++)
					{
						DxSchedule_t		*pstSchedule = (DxSchedule_t *)&(pstSchArray[ulSchCnt]);

						if ((pstSchedule->bRemoved != TRUE) && (DxRSVREPEAT_ONCE == pstSchedule->eRepeat))
						{
							// check whether schedule time is valid
							ulCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

							(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &ulStartTime);

							if (ulCurrTime > (ulStartTime + 15))
							{
								HxLOG_Warning("Schedule(Slot:%d) expired for the time.\n", pstSchedule->ulSlot);

								szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(ulStartTime);
								HxLOG_Warning("Schedule Start Time : %s\n", szTimeStr);

								szTimeStr = SAMA_SCHEDULE_GetStringOfUnixTime(ulCurrTime);
								HxLOG_Warning("Current Time 	   : %s\n", szTimeStr);

								pstSchedule->bRemoved = TRUE;
							}
							else
							{
								HxLOG_Info("this schedule's time is valid (%d)\n", pstSchedule->ulSlot);
							}
						}

						if(pstSchedule->bRemoved == TRUE)
						{
							stRemovedInfo.eRemovedReason = pstSchedule->stRemovedInfo.eRemovedReason;
							(void)SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, TRUE, SAMA_GetVirtualRequestId(), TRUE);
						}
					}

				}

				if (NULL != pstSchArray)					{ SAMA_Free(pstSchArray); }

			}
			break;

		case SAMAMESSAGE_PROCESSONEWAYTIMER:
			HxLOG_Debug("receive SAMAMESSAGE_PROCESSONEWAYTIMER \n");
			SAMA_SCHEDULE_BASE_ProcessOneWayTimer(msg->p1, msg->p2, msg->p3);
			break;

		case SAMAMESSAGE_PROCESSTWOWAYTIMER:
			HxLOG_Debug("receive SAMAMESSAGE_PROCESSTWOWAYTIMER \n");
			SAMA_SCHEDULE_ProcessTwoWayTimer(msg->p1, msg->p2, msg->p3);
			break;

		case SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE:
			HxLOG_Error("receive SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE, msg->p1: %d, msg->p2 : %d , msg->p3 : %d \n", msg->p1, msg->p2, msg->p3);
			(void)SAMA_TIMER_ProcessAfterSystemTimeAvailable(msg->p1, msg->p2, msg->p3);
			break;

		case SAMAMESSAGE_PROCESSHAPISYSTEMSTATECHANGED:
			HxLOG_Debug("receive SAMAMESSAGE_PROCESSHAPISYSTEMSTATECHANGED \n");
			(void)SAMA_TIMER_ProcessHapiSystemStateChanged (msg->p1);
			break;
		case SAMAMESSAGE_PROCESSOAPIRECCHANGED:
			HxLOG_Debug("receive SAMAMESSAGE_PROCESSOAPIRECCHANGED \n");
			{
				// 영국향에서는 자동으로 series update가 일어난다. update 시에 이미 녹화한 event는 제외할 수 있도록, record된 crid를 받아온다.
				HUINT32				ulContentId = (HUINT32)msg->p1;
				HBOOL				bIsLoadAll = (HBOOL)msg->p2;

				(void)SAMA_SCHEDULE_UpdateRecordedProgramme(ulContentId, bIsLoadAll);

			}
			break;

		case SAMAMESSAGE_PROCESSDAPIEITPFCHANGED:
			HxLOG_Debug("receive SAMAMESSAGE_PROCESSDAPIEITPFCHANGED \n");
			{
				// 영국향에서는 tracking mode일 때 rec 시작,끝을 schedule start,end가 아닌 eit pf running status transition으로 잡는다.
				HUID				*uidList = (HUID*)msg->p1;
				HUINT32				size = (HUINT32)msg->p2;

				(void)SAMA_SCHEDULE_UpdateAndTrackEitPf(uidList,size);

				if ( uidList != NULL )
				{
					SAMA_Free(uidList);
				}
			}
			break;

		case SAMAMESSAGE_PROCESSDAPIEITSCCHANGED:
			HxLOG_Debug("receive SAMAMESSAGE_PROCESSDAPIEITSCCHANGED \n");
			{
				HUID				*uidList = (HUID*)msg->p1;
				HUINT32				size = (HUINT32)msg->p2;

				(void)SAMA_SCHEDULE_UpdateEitSc(uidList,size);

				if ( uidList != NULL )
				{
					SAMA_Free(uidList);
				}
			}
			break;


		case SAMAMESSAGE_DEFAULTHDDINFOCHANGED:
			HxLOG_Debug("receive SAMAMESSAGE_DEFAULTHDDINFOCHANGED \n");
#if defined(CONFIG_TEMP_REMOVE_SCHEDULE_BY_REC_INSTANCE)
			{
				HCHAR	*szOldUuid = (HCHAR *)msg->p1;
				HCHAR	*szNewUuid = (HCHAR *)msg->p2;  // /var/lib/humaxtv/hdd_default_info uuid
				HUINT32	ulOldRec = (HUINT32)msg->p3;
				HUINT32	ulNewRec = (HUINT32)msg->p4;	// /var/lib/humaxtv/hdd_default_info max rec instance
				HERROR	hErr;
				HUINT32	ulSchNum, ulSchCnt;
				DxSchedule_t		*pstSchArray = NULL;
				DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

				if (ulOldRec > ulNewRec)
				{
					HxLOG_Warning("recording capacity got smaller. delete scheduled recordings. \n");

					hErr = SAMA_SCHEDULE_GetAllSchedules(&ulSchNum, &pstSchArray);
					if ((ERR_OK != hErr) || (0 == ulSchNum) || (NULL == pstSchArray))
					{
						if (NULL != pstSchArray)					{ SAMA_Free(pstSchArray); }
						return;
					}

					for (ulSchCnt = 0; ulSchCnt < ulSchNum; ulSchCnt++)
					{
						DxSchedule_t		*pstSchedule = (DxSchedule_t *)&(pstSchArray[ulSchCnt]);

						if ((pstSchedule->eRsvType == DxRSVTYPE_RECORD_TBR) ||
							(pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR) ||
							(pstSchedule->eRsvType == DxRSVTYPE_RECORD_INSTANT) ||
							(pstSchedule->eRsvType == DxRSVTYPE_RECORD_DELAYED) ||
							(pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR))
						{
							stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Canceled;
							(void)SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, TRUE, SAMA_GetVirtualRequestId(), TRUE);
						}
					}
				}

				if (pstSchArray != NULL)
				{
					SAMA_Free(pstSchArray);
				}
				if (szOldUuid != NULL)
				{
					SAMA_Free(szOldUuid);
				}
				if (szNewUuid != NULL)
				{
					SAMA_Free(szNewUuid);
				}
			}
#endif
			break;

		case SAMAMESSAGE_ANTENA_UPDATED:
			SAMA_RSC_RefreshResource(SAMA_GetVirtualRequestId());
			break;

		default:
			break;
	}

}

STATIC void				sama_MainTask(void *pvArg)
{
	HINT32				err = 0;
	unsigned long		timeout = 0;
	Sama_InternalMsg_t 	msg;
	Sama_RpcProtocol_t		*pSamaRpc;

	pSamaRpc = sama_RpcProtocol_GetInstance();
	SAMA_PROTOCOL_RegisterRpcProtocols(pSamaRpc->nRpcHandle);

	while (1)
	{
		if(err == VK_TIMEOUT)
		{
			timeout = 10;
		}
		else
		{
			timeout = 0;
		}

		err = VK_MSG_ReceiveTimeout (s_ulSamaMainMsgQId, &msg, sizeof(Sama_InternalMsg_t), timeout);

		if(err == VK_OK)
		{
			sama_MainTask_ProcessMessage(&msg);
		}

		// TODO: 여기서 sleep 시간을, sama timer에서 남은 시간 만큼 하면 더 성능 향상.
		VK_TASK_Sleep (100);

	}

}


STATIC void sama_MainTaskInit(void)
{
	HINT32		err;

	/* create & init message que */
	err = VK_MSG_Create(SAMA_MSGQ_SIZE, sizeof(Sama_InternalMsg_t), SAMA_MSG_QUE_NAME, &s_ulSamaMainMsgQId, VK_SUSPENDTYPE_FIFO);
	if (err != VK_OK)
	{
		HxLOG_Error("GWM msgQ creation fail err = 0x%x\n", err);
		return;
	}

	/* create and start GWM task */
	err = VK_TASK_Create(sama_MainTask, 80, SAMA_TASK_STACK_SIZE,
						SAMA_MAIN_TASK_NAME, NULL, &s_ulSamaMainTaskId, 0);
	if (err != VK_OK)
	{
		HxLOG_Error("Sama task creation fail err = 0x%x\n", err);
		return;
	}

	err = VK_TASK_Start(s_ulSamaMainTaskId);
	if (err != VK_OK)
	{
		HxLOG_Error("Sama task start fail err = 0x%x\n", err);
		return;
	}

}


#define ___CHANNEL_DB_NOTIFIER___
STATIC void sama_cbDamaServiceGetter (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
	{
	HUINT32				 ulCnt, ulSchCnt;
	sama_DbSvcUpdated_t	*pstSvcUpdated = (sama_DbSvcUpdated_t *)pvUser;
	DAxIterator_t		 stIter;

	if ((NULL == pstSvcUpdated) ||
		(0 == pstSvcUpdated->ulNumSvcUid) || (NULL == pstSvcUpdated->pulSvcUidList) ||
		(0 == pstSvcUpdated->ulNumSch) || (NULL == pstSvcUpdated->pstSchArray))
	{
		goto END_FUNC;
	}

	DAPI_InitIterator(&stIter, pvData, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxService_t 	*pstService = (DxService_t *)stIter.data;

		if (NULL != pstService)
		{
			for (ulCnt = 0; ulCnt < pstSvcUpdated->ulNumSvcUid; ulCnt++)
			{
				HUID		*pulSvcUid = &(pstSvcUpdated->pulSvcUidList[ulCnt]);

				if (*pulSvcUid == (HUID)pstService->uid)
				{
					*pulSvcUid = (HUID)0;
				}
			}
		}
	}

	// Check the No-Serviced UID Lists:
	for (ulCnt = 0; ulCnt < pstSvcUpdated->ulNumSvcUid; ulCnt++)
	{
		HUINT32			 ulSvcUid = (HUINT32)(pstSvcUpdated->pulSvcUidList[ulCnt]);
		DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

		if (0 == ulSvcUid)			{ continue; }

		HxLOG_Debug("Removed SVC UID (%d)\n", ulSvcUid);

		for (ulSchCnt = 0; ulSchCnt < pstSvcUpdated->ulNumSch; ulSchCnt++)
		{
			DxSchedule_t		*pstSchedule = &(pstSvcUpdated->pstSchArray[ulSchCnt]);

			if (pstSchedule->ulSvcUid == ulSvcUid)
			{
				HxLOG_Debug("Removed Schedule Slot (%d)\n", pstSchedule->ulSlot);
				stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_ServiceDeleted;
				SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
			}
		}
	}

END_FUNC:
	if (NULL != pstSvcUpdated)
	{
		if (NULL != pstSvcUpdated->pulSvcUidList)				{ SAMA_Free(pstSvcUpdated->pulSvcUidList); }
		if (NULL != pstSvcUpdated->pstSchArray)					{ SAMA_Free(pstSvcUpdated->pstSchArray); }
		SAMA_Free(pstSvcUpdated);
	}

	return;
}

STATIC void sama_cbDamaOnServiceUpdated (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HUINT32				 ulCnt, ulSchCnt, ulCheckSvcNum = 0, ulSchNum = 0;
	HUINT32				 ulNumberOfSvc = (HUINT32)(size / sizeof(HUID));
	DxSchedule_t		*pstSchArray = NULL;
	HUID				*pulUidList = (HUID *)pvData;
	HUID				*pulReqUidList = NULL;
	sama_DbSvcUpdated_t	*pstSvcUpdated = NULL;
	HERROR				 hErr;

	if ((0 == ulNumberOfSvc) || (NULL == pulUidList))
	{
		return;
	}

#if 0
	// Reset 되어있는지 확인
	if (ulNumberOfSvc == 1)
	{
		ulUid = *pulUidList;
		if (ulUid == -1)
		{
			// Delete all list
			HxLOG_Warning("Remove ALL info\n");
			return;
		}
	}
#endif
	hErr = SAMA_SCHEDULE_GetAllSchedules(&ulSchNum, &pstSchArray);
	if ((ERR_OK != hErr) || (0 == ulSchNum) || (NULL == pstSchArray))
	{
		goto END_FUNC;
	}

	// Reset 되어있는지 확인
	if (ulNumberOfSvc == 1)
	{
		HUINT32 	ulSvcUid = (HUINT32)*pulUidList;
		DxSchedule_RemovedInfo_t stRemovedInfo = {0, };

		if ((HUINT32)-1 == ulSvcUid)
		{
			HxLOG_Warning("All services deleted. All Schedules shall be deleted...\n");

			for (ulSchCnt = 0; ulSchCnt < ulSchNum; ulSchCnt++)
			{
				DxSchedule_t		*pstSchedule = (DxSchedule_t *)&(pstSchArray[ulSchCnt]);

				stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_ServiceDeleted;
				(void)SAMA_SCHEDULE_RemoveSchedule(pstSchedule->ulSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
			}

			goto END_FUNC;
		}
	}

	for (ulCnt = 0; ulCnt < ulNumberOfSvc; ulCnt++)
	{
		HUINT32			 ulSvcUid = (HUINT32)pulUidList[ulCnt];

		for (ulSchCnt = 0; ulSchCnt < ulSchNum; ulSchCnt++)
		{
			DxSchedule_t		*pstSchedule = (DxSchedule_t *)&(pstSchArray[ulSchCnt]);

			if (pstSchedule->ulSvcUid == ulSvcUid)
			{
				ulCheckSvcNum ++;
				break;
			}
		}
	}

	if (ulCheckSvcNum == 0)
	{
		goto END_FUNC;
	}

	pulReqUidList = (HUID *)SAMA_Calloc(sizeof(HUID) * ulCheckSvcNum);
	if (NULL == pulReqUidList)
	{
		goto END_FUNC;
	}

	for (ulCnt = 0, ulCheckSvcNum = 0; ulCnt < ulNumberOfSvc; ulCnt++)
	{
		HUINT32			 ulSvcUid = (HUINT32)pulUidList[ulCnt];

		for (ulSchCnt = 0; ulSchCnt < ulSchNum; ulSchCnt++)
		{
			DxSchedule_t		*pstSchedule = (DxSchedule_t *)&(pstSchArray[ulSchCnt]);

			if (pstSchedule->ulSvcUid == ulSvcUid)
			{
				pulReqUidList[ulCheckSvcNum] = pulUidList[ulCnt];
				ulCheckSvcNum ++;
				break;
			}
		}
	}

	pstSvcUpdated = (sama_DbSvcUpdated_t *)SAMA_Calloc(sizeof(sama_DbSvcUpdated_t));
	if (NULL == pstSvcUpdated)
	{
		goto END_FUNC;
	}

	pstSvcUpdated->ulNumSvcUid		= ulCheckSvcNum;
	pstSvcUpdated->pulSvcUidList	= pulReqUidList;
	pstSvcUpdated->ulNumSch			= ulSchNum;
	pstSvcUpdated->pstSchArray		= pstSchArray;

	DAPI_GetUIDInfos(DxNAMEOF(DxService_t), pulReqUidList, ulCheckSvcNum, sama_cbDamaServiceGetter, (void *)pstSvcUpdated);

	pulReqUidList = NULL;
	pstSchArray = NULL;

END_FUNC:
	if (NULL != pstSchArray)					{ SAMA_Free(pstSchArray); }
	if (NULL != pulReqUidList)					{ SAMA_Free(pulReqUidList); }
	return;
}




STATIC HERROR sama_initSvcDbNotifier (void)
{
	STATIC HUINT32		 s_ulNotiId = 0;

	if (0 == s_ulNotiId)
	{
		s_ulNotiId = DAPI_AddTableNotifier(DxNAMEOF(DxService_t), sama_cbDamaOnServiceUpdated, NULL);
	}

	return ERR_OK;
}
#define ___ANTENA_DB_NOTIFIER___

STATIC void sama_cbDamaOnAntenaUpdated (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	SAMA_PostMessage(SAMAMESSAGE_ANTENA_UPDATED, 0, 0, 0, 0, 0, 0);

	return;
}

STATIC HERROR sama_initAntDbNotifier (void)
{
	STATIC HUINT32		 s_ulNotiId = 0;

	if (0 == s_ulNotiId)
	{
		s_ulNotiId = DAPI_AddTableNotifier(DxNAMEOF(DxAntInfo_t), sama_cbDamaOnAntenaUpdated, NULL);
	}

	return ERR_OK;
}



#define ___UAPI_NOTIFIER___

void	sama_cbUdapiOnStatusChanged(DxStorage_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HxLOG_Trace();

	HxLOG_Debug("eType: %d \n", eType);

	if (eDxSTORAGE_NOTI_DevChange == eType)
	{
		// nothing to do
	}
	else if (eDxSTORAGE_NOTI_PvrReady == eType)
	{
		SAMA_PostMessage(SAMAMESSAGE_CHECKPVRREADYSTATUS, nParam1, nParam2, nParam3, 0, 0, 0);

	}
	else if(eDxSTORAGE_NOTI_FormatState == eType)
	{
		// nothing to do
	}
	else if (eDxSTORAGE_NOTI_Result == eType)
	{
		// nothing to do
	}
	else if (eDxSTORAGE_NOTI_DefaultHddInfoChanged == eType)
	{
		HCHAR *szOldUuid;
		HCHAR *szNewUuid;
		HUINT32 ulOldRec;
		HUINT32 ulNewRec;
		HUINT32 ulPackedRec;

		szOldUuid = (HCHAR *)SAMA_StrDup((const HCHAR *)nParam1);
		szNewUuid = (HCHAR *)SAMA_StrDup((const HCHAR *)nParam2);
		ulPackedRec = (HUINT32)nParam3;
		ulOldRec = ulPackedRec>>16;
		ulNewRec = ulPackedRec& 0xFFFF;
		if (szOldUuid != NULL && szNewUuid != NULL)
		{
			SAMA_PostMessage(SAMAMESSAGE_DEFAULTHDDINFOCHANGED, (HINT32)szOldUuid, (HINT32)szNewUuid, (HINT32)ulOldRec, (HINT32)ulNewRec, 0, 0);
		}
		else
		{
			if (szOldUuid != NULL) {SAMA_Free(szOldUuid);}
			if (szNewUuid != NULL) {SAMA_Free(szNewUuid);}
		}
	}
	else
	{
		HxLOG_Error("Unknown Type : %d \n", eType);
	}

}

STATIC HERROR sama_initUapiNotifier (void)
{
	STATIC	HERROR		hErrUapiNotifierSet = ERR_FAIL;

	if (ERR_FAIL == hErrUapiNotifierSet)
	{
		(void)UAPI_FS_Init();
		hErrUapiNotifierSet = UAPI_FS_SetNotifier(sama_cbUdapiOnStatusChanged);
	}

	return ERR_OK;
}


#define ___OAPI_NOTIFIER___

static void sama_cbOapiOnRecStatusChanged (
						OxMetaRecord_Event_e evt
						, HINT32 count
						, HUINT32 cid
						, const DxRecListData_t *rec
						, void *unused)
{
	HBOOL			bLoadAll = TRUE;
	HUINT32			ulContentId = 0;

	HxLOG_Debug("evt:%d, count:%d, cid:%u \n", evt, count, cid);

	switch (evt)
	{
		case eOxMETA_RECORD_ADDED:
			if (rec == NULL)
			{
				bLoadAll = TRUE;
			}
			else
			{
				bLoadAll = FALSE;
				ulContentId = rec->ulContentId;
			}
			SAMA_PostMessage(SAMAMESSAGE_PROCESSOAPIRECCHANGED, (HINT32)ulContentId, (HINT32)bLoadAll, 0, 0, 0, 0);
			break;
		default:
			break;
	}
}

STATIC HERROR sama_initOapiNotifier (void)
{
	STATIC	HERROR		hErrOapiNotifierSet = ERR_FAIL;

	if (ERR_FAIL == hErrOapiNotifierSet)
	{
		OAPI_META_RECORD_SetListener(sama_cbOapiOnRecStatusChanged, NULL);
		hErrOapiNotifierSet = ERR_OK;
	}

	return ERR_OK;
}

#define ___EPGPF_NOTIFIER___

STATIC void sama_cbDamaOnEitPfUpdated (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	const HUID			*uidlist = (const HUID*)pvData;
	HUID				*uidListClone;

 	if ( size == 0 )
 	{
		HxLOG_Debug("invalid param. size=0\n");
		return;
 	}

	uidListClone = (HUID *)SAMA_MemDup(uidlist, size);
	if ( uidListClone == NULL )
	{
		HxLOG_Debug("SAMA_MemDup failed. \n");
		return;
	}

	SAMA_PostMessage(SAMAMESSAGE_PROCESSDAPIEITPFCHANGED, (HINT32)uidListClone, (HINT32)size, 0, 0, 0, 0);
}

STATIC HERROR sama_initEpgPfNotifier (void)
{
	STATIC HUINT32		 s_ulNotiId = 0;

	if (0 == s_ulNotiId)
	{
		s_ulNotiId = DAPI_AddTableNotifier(DxNAMEOF(eDxEPG_TYPE_PF), sama_cbDamaOnEitPfUpdated, NULL);
	}

	return ERR_OK;
}

STATIC void sama_cbDamaOnEitScUpdated (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	const HUID			*uidlist = (const HUID*)pvData;
	HUID				*uidListClone;

 	if ( size == 0 )
 	{
		HxLOG_Debug("invalid param. size=0\n");
		return;
 	}

	uidListClone = (HUID *)SAMA_MemDup(uidlist, size);
	if ( uidListClone == NULL )
	{
		HxLOG_Debug("SAMA_MemDup failed. \n");
		return;
	}

	SAMA_PostMessage(SAMAMESSAGE_PROCESSDAPIEITSCCHANGED, (HINT32)uidListClone, (HINT32)size, 0, 0, 0, 0);

}

STATIC HERROR sama_initEpgScNotifier (void)
{
	STATIC HUINT32		 s_ulNotiId = 0;

	if (0 == s_ulNotiId)
	{
		s_ulNotiId = DAPI_AddTableNotifier(DxNAMEOF(eDxEPG_TYPE_SC), sama_cbDamaOnEitScUpdated, NULL);

	}

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR		SAMA_GetRpcHandle (HINT32 *pnRpcHandle)
{
	Sama_RpcProtocol_t *pSamaRPC;

	HxLOG_Trace();
	pSamaRPC = sama_RpcProtocol_GetInstance();
	if (pSamaRPC)
	{
		*pnRpcHandle = pSamaRPC->nRpcHandle;
		HxLOG_Debug("*pnRpcHandle: %d \n", *pnRpcHandle);
		return ERR_OK;
	}

	*pnRpcHandle = 0;
	HxLOG_Error("pSamaRPC is NULL, *pnRpcHandle = 0 \n");
	return ERR_FAIL;

}

SAMA_Context_t *SAMA_GetContext (void)
{
	SAMA_Context_t* pstContext;

	//HxSEMT_Get(s_stSama_Context->ulSemId);
	pstContext = &s_stSama_Context;
	//HxSEMT_Release(s_stSama_Context->ulSemId);

	return pstContext;
}

// SAMA내에서 직접 수행한 것을 알려주기 위함. 즉 특정 client 의 요청이 아님.
HINT32	SAMA_GetVirtualRequestId(void)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	HUINT16	nSamaPid = (HINT32)eSAPI_UNIQUE_PROCESS_Indicator_Sama;

#else
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	HUINT16				nSamaPid = 0;

	nSamaPid = pstContext->nSamaPid;
#endif

	return ((nSamaPid << 16) | (0x0000));

}

HERROR SAMA_PostMessage(HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3, HINT32 p4, HINT32 p5, HINT32 p6)
{
	Sama_InternalMsg_t		msg;
	HINT32				err;
	HERROR				hErr = ERR_FAIL;

	msg.message = message;
	msg.p1 = p1;
	msg.p2 = p2;
	msg.p3 = p3;
	msg.p4 = p4;
	msg.p5 = p5;
	msg.p6 = p6;

	err = VK_MSG_SendTimeout(s_ulSamaMainMsgQId, &msg, sizeof(Sama_InternalMsg_t), 0);

	if(err == VK_OK)
	{
		hErr = ERR_OK;
	}

	return	hErr;
}


#define ___API_FUNCTIONS___
int	sama_main(int argc, char *argv[])
{
	VK_Init();

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("sama");

	(void)DAPI_Init();

	(void)sama_initContext();
	sama_MainTaskInit();


	(void)SAMA_TIMER_InitTimer();
	(void)sama_initSvcDbNotifier();
	(void)sama_initAntDbNotifier();
	(void)sama_initUapiNotifier();
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	(void)sama_initOapiNotifier();

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME_ACCURATE_RECORD)
	(void)sama_initEpgPfNotifier();
#endif

#if defined(CONFIG_OP_NORDIG)
	(void)sama_initEpgPfNotifier();
	(void)sama_initEpgScNotifier();
#endif

#endif

#if defined(CONFIG_DEBUG)
	(void)SAMA_DBG_Init();
#endif

	return 0;
}

#if defined(CONFIG_PROD_OS_EMULATOR)
#define SAMA_main	SAMA_EMU_Main
#else
#define SAMA_main	main
#endif

int SAMA_main (int argc, char *argv[])
{
	sama_main(argc, argv);

#if defined(CONFIG_PROD_OS_EMULATOR)
	VK_TASK_Sleep (100);
#else
	while (1)
	{
		VK_TASK_Sleep (1000000);
	}
#endif
	return 0;
}

