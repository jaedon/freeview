/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_schedule.c
	@brief

	Description:  									\n
	Module: OBAMA / tvmanager / schedule	\n

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
#include <rlib.h>
#include <sapi.h>
#include <octo_common.h>
#include <db_param.h>
#include <svc_ch.h>
#include <svc_av.h>


#include <mgr_common.h>
#include <mgr_schedule.h>
#include <pmgr_schedule.h>

#include <mgr_pvr_action.h>
#include <mgr_recorder.h>

/*******************************************************************/
/********************      Local Definition         *************************/
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

#define ___LOCAL_FUNCTIONS___

#if defined(CONFIG_DEBUG)

STATIC HINT32 mgr_schedule_StartAso (void *pvArg)
{
	// case 2. Make NCD Schedule
	HUINT32 					 ulSlot;
	DxSchedule_t				 stSchedule;
	HERROR						hErr;
	UNIXTIME						utStartTime;

	HCHAR	*aucArg=NULL;

	/* 1st argument */
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
	{
		// default currtime+60
		utStartTime = (UNIXTIME)HLIB_STD_GetSystemTime() + 60;
		HLIB_CMD_Printf("use default. currtime+60 (%d)[%s] \n",utStartTime,HLIB_DATETIME_UnixTimeToStringWithOutOffset(utStartTime, NULL, 0));
	}
	else
	{
		 sscanf(aucArg, "%d", &utStartTime);
	}

	hErr = SAPI_GetAvailableSlot(&ulSlot);
	if (ERR_OK != hErr)
	{
		HLIB_CMD_Printf("SAPI_GetAvailableSlot err:\n");
		return HxCMD_OK;
	}

	HLIB_CMD_Printf("aso schedule. starttime(%d)[%s] \n",utStartTime,HLIB_DATETIME_UnixTimeToStringWithOutOffset(utStartTime, NULL, 0));

	HxSTD_MemSet(&stSchedule, 0, sizeof(DxSchedule_t));
	stSchedule.ulSlot		= ulSlot;
	stSchedule.eRsvType 	= DxRSVTYPE_ASO_NOTIFY;
	stSchedule.eRsvReady	= DxRSVREADY_30_SEC;
	stSchedule.eRepeat		= DxRSVREPEAT_ONCE;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(stSchedule.stStartTime));

	hErr = SAPI_MakeSchedule (ulSlot, &stSchedule, NULL);
	if (ERR_OK != hErr)
	{
		HLIB_CMD_Printf ("SAPI_MakeSchedule err:\n");
		return HxCMD_OK;
	}

	return HxCMD_OK;

}

STATIC void mgr_schedule_RegDebugCmd (void)
{
#define	hCmdHandle		"mgr_schedule"
	HLIB_CMD_RegisterWord (hCmdHandle,pmgr_schedule_base_get,
						"get",												/* keyword */
						"note"										,	 		/*   help  */
						"type");												/*  usage  */

	HLIB_CMD_RegisterWord (hCmdHandle,pmgr_schedule_base_start,
						"start",												/* keyword */
						"note"										,	 		/*   help  */
						"type");												/*  usage  */

	HLIB_CMD_RegisterWord (hCmdHandle,pmgr_schedule_base_stop,
						"stop",												/* keyword */
						"note"										,			/*	 help  */
						"type");												/*	usage  */

	HLIB_CMD_RegisterWord (hCmdHandle,pmgr_schedule_base_startMediaServer, "startms", " ", "monitoring ringbuf directly");

	HLIB_CMD_RegisterWord (hCmdHandle,pmgr_schedule_base_makeTBRSchedule,
						"makeTBR",												/* keyword */
						"note"										,			/*	 help  */
						"type");												/*	usage  */
	HLIB_CMD_RegisterWord (hCmdHandle,mgr_schedule_StartAso, "aso", " ", "aso start_time");

}
#endif

STATIC void	mgr_schedule_RemoveScheduleBGDescramble(void)
{
	HUINT32 		ulCnt = 0, ulNumSch = 0, ulNumSch2 = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	hErr = SAPI_CountScheduleByType(DxRSVTYPE_DESC_REC_STANDBY, &ulNumSch);
	if(hErr == ERR_FAIL || ulNumSch == 0)
	{
		HxLOG_Debug("Fail to Count Schedules. or ulNumSch == 0\n");
		return;
	}
	HxLOG_Debug("Schedule Count ... (%d) \n", ulNumSch);
	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (pstSchArray)
	{
		hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, DxRSVTYPE_DESC_REC_STANDBY);
		if(hErr == ERR_OK)
		{
			for (ulCnt = 0; ulCnt < ulNumSch2; ulCnt++)
			{
				DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
				if(pstSchedule != NULL && pstSchedule->eRsvType == DxRSVTYPE_DESC_REC_STANDBY)
				{
					HxLOG_Debug("Cancel Schedule Slot (%d) \n", pstSchedule->ulSlot);
					SAPI_CancelSchedule(pstSchedule->ulSlot);
				}
			}
		}
		HLIB_STD_MemFree(pstSchArray);
	}
}



// Notifier는 BUS Thread에서 돌지 않는다는 것을 반드시 암기해야 한다.
STATIC HERROR mgr_schedule_SapiNotifier (void *pvUserData, SAPI_NotiInfo_t *punNotify)
{
	DxSchedule_t			*pstSchedule = NULL;
	HERROR				hErr = ERR_OK;


	// 어차피 sys_sys를 control 할 목적이지만, bus thread를 태우지 않아도 되지만, 혹시 추후에 다른 기능들이 들어갈 경우 대비하여 bus로 태운다.
	BUS_PostMessage(NULL, eMEVT_SCHEDULE_EVENT_OCCURED, HANDLE_NULL, punNotify->eNotifyType, 0,  0);

	HxLOG_Debug("punNotify->eNotifyType: %d \n", punNotify->eNotifyType);

	switch (punNotify->eNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_SystemTimeChanged:
		if (punNotify->stSystemTimeChanged.eSystemTimeChangedType == eSAPI_SYSTEM_TIME_CHANGED_TYPE_First_Available)
		{
			HxLOG_Debug("Make Regular Ota Schedule By First valid System Time");
			MGR_SCHEDULE_MakeRegularOta();
		}
		break;
	case eSAPI_NOTIFY_TYPE_SchedulesLoaded:
		{
			mgr_schedule_RemoveScheduleBGDescramble();
		}
		break;
	case eSAPI_NOTIFY_TYPE_ScheduleAdded:
		{
			HUINT32 		ulMainActionId = MGR_ACTION_GetMainActionId();
			Handle_t		hMainAction = SVC_PIPE_GetActionHandle(ulMainActionId);
			DxSchedule_t	*pstSchedule = punNotify->stSchAdded.pstSchedule;

			if((pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR) || (pstSchedule->eRsvType == DxRSVTYPE_RECORD_TBR))
			{
				BUS_PostMessage(NULL, eMEVT_SCHEDULE_NEW_REC_ADDED, hMainAction, 0, 0, 0);
			}
		}
		break;
	case eSAPI_NOTIFY_TYPE_ScheduleChanged:
		{
#if defined(CONFIG_MW_MM_PVR)
			HUINT32		ulActionId;
			Handle_t	hSvcHandle;
			MgrRec_RecInfo_t stRecInfo;
#endif
			pstSchedule = (DxSchedule_t *)OxMGR_Calloc (sizeof(DxSchedule_t));
			if (NULL != pstSchedule)
			{

				HxSTD_MemCopy(pstSchedule, punNotify->stSchChanged.pstSchedule, sizeof(DxSchedule_t));

				switch(pstSchedule->eRsvType)
				{
#if defined(CONFIG_MW_MM_PVR)
					case DxRSVTYPE_RECORD_EBR:
					case DxRSVTYPE_RECORD_TBR:
					case DxRSVTYPE_RECORD_SERIES_EBR:
					case DxRSVTYPE_RECORD_SATRECORD_TBR:
					{
						hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstSchedule->ulSvcUid & 0xFFFF), &hSvcHandle);
						if (ERR_OK != hErr)
						{
							HxLOG_Error("Cannot find SvcHandle: ulSvcUid(%d)\n", pstSchedule->ulSvcUid);
							goto END_FUNC;
						}

						if (MGR_RECORDER_FindRecordingSvcInfo(hSvcHandle, &ulActionId, &stRecInfo) == ERR_OK)
						{
							UNIXTIME utStartTime = 0;
							UNIXTIME utEndTime = 0;
							HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &utStartTime);

							utEndTime = utStartTime + pstSchedule->ulDuration;

							if( utEndTime != stRecInfo.ulEndTime )
								BUS_PostMessage(NULL, eMEVT_PVR_RECORD_ENDTIME_UPDATE, stRecInfo.hAction, utEndTime, 0,  0);
						}
						break;
					}
#endif
#if defined(CONFIG_STREAMING_SATIP)
					case DxRSVTYPE_NETSTREAMING_LIVE:
					{

						BUS_PostData (NULL, eMEVT_SCHEDULE_SATIP_CHANGEPID, HANDLE_NULL,
						punNotify->stSchChanged.ulSlotId, punNotify->stSchChanged.eNotifyType, (void *)pstSchedule, sizeof(DxSchedule_t));
						break;
					}
#endif
#if defined(CONFIG_TRANSCODING_STREAMING)
					case DxRSVTYPE_NETSTREAMING_FILE:
					{
						BUS_PostData (NULL, eMEVT_SCHEDULE_HLS_CHANGE_PLAY_POSITION, HANDLE_NULL,
						punNotify->stSchChanged.ulSlotId, punNotify->stSchChanged.eNotifyType, (void *)pstSchedule, sizeof(DxSchedule_t));
						break;
					}
#endif
					default:
						break;
				}
			}
			else
			{
				HxLOG_Error("Calloc fail \n");
				hErr = ERR_FAIL;
			}
		}
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
		pstSchedule = (DxSchedule_t *)OxMGR_Calloc (sizeof(DxSchedule_t));
		if (NULL != pstSchedule)
		{
			HxLOG_Debug("slotId: %d, eRemovedReason %d \n", punNotify->stSchRemoved.ulSlotId, punNotify->stSchRemoved.pstSchedule->stRemovedInfo.eRemovedReason);
			HxSTD_MemCopy(pstSchedule, punNotify->stSchRemoved.pstSchedule, sizeof(DxSchedule_t));
#if 0
			if(punNotify->stSchRemoved.eRemoveReason == DxSchedule_RemovedReason_Expired)
			{
				BUS_PostData (NULL, eMEVT_SCHEDULE_EXPIRED, HANDLE_NULL,
								punNotify->stSchRemoved.ulSlotId, punNotify->stSchRemoved.nRequestPid, (void *)pstSchedule, sizeof(DxSchedule_t));
			}
			else
#endif
			{
				BUS_PostData (NULL, eMEVT_SCHEDULE_CANCELED, HANDLE_NULL,
								punNotify->stSchRemoved.ulSlotId, punNotify->stSchRemoved.nRequestPid, (void *)pstSchedule, sizeof(DxSchedule_t));
			}
		}
		else
		{
			HxLOG_Error("Calloc fail \n");
			hErr = ERR_FAIL;
		}

		break;

	case eSAPI_NOTIFY_TYPE_TimeUp:
		pstSchedule = (DxSchedule_t *)OxMGR_Calloc (sizeof(DxSchedule_t));
		if (NULL != pstSchedule)
		{
			if(punNotify->stTimeUp.pstSchedule != NULL)
			{
				HxLOG_Debug("slotId: %d, punNotify->stTimeUp.pstSchedule: 0x%p is not null. rsv Type: 0x%x \n", punNotify->stTimeUp.ulSlotId, punNotify->stTimeUp.pstSchedule, punNotify->stTimeUp.pstSchedule->eRsvType);
				HxSTD_MemCopy(pstSchedule, punNotify->stTimeUp.pstSchedule, sizeof(DxSchedule_t));

				BUS_PostData (NULL, eMEVT_SCHEDULE_TIME_UP, HANDLE_NULL,
								punNotify->stTimeUp.ulSlotId, punNotify->stTimeUp.eTimerType, (void *)pstSchedule, sizeof(DxSchedule_t));


			}
			else // NULL 이 오면 확인 필요함. NULL 올 경우가 없어야 한다.
			{
				HxLOG_Debug("punNotify->stTimeUp.pstSchedule is NULL \n");
				hErr = SAPI_GetSchedule (punNotify->stTimeUp.ulSlotId, pstSchedule);
				if (ERR_OK == hErr)
				{
					BUS_PostData (NULL, eMEVT_SCHEDULE_TIME_UP, HANDLE_NULL,
									punNotify->stTimeUp.ulSlotId, punNotify->stTimeUp.eTimerType, (void *)pstSchedule, sizeof(DxSchedule_t));
				}
				else
				{
					HxLOG_Error ("SAPI_GetSchedule err: ulSlotId(%d)\n", punNotify->stTimeUp.ulSlotId);
				}

			}
		}
		else
		{
			HxLOG_Error("Calloc fail \n");
			hErr = ERR_FAIL;
		}

		break;

	default:
		break;
	}

#if defined(CONFIG_MW_MM_PVR)
END_FUNC:
#endif
	if(pstSchedule != NULL)
	{
		OxMGR_Free (pstSchedule);
		pstSchedule = NULL;
	}

	return hErr;
}

#define _____RESOURCE_____
STATIC HERROR mgr_schedule_SetActionNumToRscInfo (SAPI_RscInfo_t *pstRscInfo)
{
	pstRscInfo->anActionNum[eSAPI_ACTION_TYPE_LiveView] = SVC_AV_GetNumberOfVideoDecoder();
	pstRscInfo->anActionNum[eSAPI_ACTION_TYPE_Record] = NUM_REC_ACTION;
	pstRscInfo->anMaxActionNum[eSAPI_ACTION_TYPE_LiveView] = NUM_VIEW_ACTION;
	pstRscInfo->anMaxActionNum[eSAPI_ACTION_TYPE_Record] = NUM_MAX_REC_ACTION;
#if defined(CONFIG_PROD_HGS1000S)
	pstRscInfo->anActionNum[eSAPI_ACTION_TYPE_NetStreaming] = 4; // PL_STREAMING_MAX_NUM와 동일하게
	pstRscInfo->anMaxActionNum[eSAPI_ACTION_TYPE_NetStreaming] = 4; // PL_STREAMING_MAX_NUM와 동일하게
#else
	pstRscInfo->anActionNum[eSAPI_ACTION_TYPE_NetStreaming] = 2; // PL_STREAMING_MAX_NUM와 동일하게
	pstRscInfo->anMaxActionNum[eSAPI_ACTION_TYPE_NetStreaming] = 2; // PL_STREAMING_MAX_NUM와 동일하게
#endif

	return ERR_OK;
}

STATIC HERROR mgr_schedule_SetRfInputToRscInfo (SAPI_RscInfo_t *pstRscInfo)
{
	HUINT32				 ulRfCnt = 0, ulRfInputNum = 0;
	RxRfInfoCapacity_t	*pstRxRfInfo = NULL;

#if defined(CONFIG_MW_CH_SATELLITE)
	ulRfInputNum = SVC_CH_CountSatelliteRfInput();
#endif
#if defined(CONFIG_MW_CH_CABLE)
	ulRfInputNum ++;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	ulRfInputNum ++;
#endif

	if (0 == ulRfInputNum)
	{
		HxLOG_Error ("Device Num = 0\n");
		return ERR_FAIL;
	}

	pstRscInfo->anRscNum[eRxRSCTYPE_RFINPUT] = (HINT32)ulRfInputNum;

#if defined(CONFIG_MW_CH_SATELLITE)
{
	HUINT32 				 ulSatRfInputNum = SVC_CH_CountSatelliteRfInput();
	CH_SatRfInput_t			 stRfInput;
	HERROR					 hErr;

	for (; ulRfCnt < ulSatRfInputNum; ulRfCnt++)
	{
		hErr = SVC_CH_GetSatelliteRfInput (ulRfCnt, &stRfInput);
		if (ERR_OK == hErr)
		{
			pstRxRfInfo = &(pstRscInfo->astRfInfoCap[ulRfCnt]);

			pstRxRfInfo->eSupportedDeliType = eDxDELIVERY_TYPE_SAT;
			pstRxRfInfo->ulConnectableTunerNum = (stRfInput.ulConnectableTunerNum > RxTUNER_NUM_MAX) ? RxTUNER_NUM_MAX : stRfInput.ulConnectableTunerNum;
			HxSTD_MemCopy (pstRxRfInfo->aulConnectableTunerId, stRfInput.aulConnectableTunerId, sizeof(HUINT32) * pstRxRfInfo->ulConnectableTunerNum);
		}
	}
}
#endif

#if defined(CONFIG_MW_CH_CABLE)
	pstRxRfInfo = &(pstRscInfo->astRfInfoCap[ulRfCnt]);

	pstRxRfInfo->eSupportedDeliType = eDxDELIVERY_TYPE_CAB;
	pstRxRfInfo->ulConnectableTunerNum = 0;

	ulRfCnt++;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	pstRxRfInfo = &(pstRscInfo->astRfInfoCap[ulRfCnt]);

	pstRxRfInfo->eSupportedDeliType = eDxDELIVERY_TYPE_TER;
	pstRxRfInfo->ulConnectableTunerNum = 0;

	ulRfCnt++;
#endif

	pstRscInfo->anRscNum[eRxRSCTYPE_RFINPUT] = (HINT32)ulRfCnt;
	return ERR_OK;
}

STATIC HERROR mgr_schedule_SetTunerToRscInfo (SAPI_RscInfo_t *pstRscInfo)
{
	HUINT32			 ulCnt, ulTunerNum = 0;
	HUINT32			 ulDxDeliType = eDxDELIVERY_TYPE_ALL;

	(void)SVC_CH_GetNumberOfTunerGroup (&ulTunerNum);
	if (0 == ulTunerNum)
	{
		HxLOG_Error ("Tuner Num 0\n");
		return ERR_FAIL;
	}

	if (ulTunerNum > RxTUNER_NUM_MAX)					{ ulTunerNum = RxTUNER_NUM_MAX; }

	pstRscInfo->anRscNum[eRxRSCTYPE_TUNER_GROUP] = (HINT32)ulTunerNum;

	for (ulCnt = 0; ulCnt < ulTunerNum; ulCnt++)
	{
		(void)SVC_CH_GetTunerType (ulCnt, &ulDxDeliType);
		pstRscInfo->aeTunerSupportDeliType[ulCnt] = (DxDeliveryType_e)ulDxDeliType;
	}

	return ERR_OK;
}

STATIC HERROR mgr_schedule_SetRfInputAbility (void)
{

#if defined(CONFIG_MW_CH_SATELLITE)

	HUINT32 				 ulSatRfInputNum = SVC_CH_CountSatelliteRfInput();
	DxAntennaConnection_e	 eAntConn;
	HERROR					 hErr;


	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32 *)&eAntConn, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
	}

	if (ulSatRfInputNum >= 1)
	{
		SAPI_SetRfInputAbility (0, TRUE);
	}

	if (ulSatRfInputNum >= 2)
	{
		switch (eAntConn)
		{
		case eDxANTCONN_TWOSAME:
			SAPI_SetRfInputAbility (1, TRUE);
			break;

		case eDxANTCONN_ONECABLE:
		default:
			SAPI_SetRfInputAbility (1, FALSE);
			break;
		}
	}

	if (ulSatRfInputNum >= 3)
	{
		HxLOG_Error ("\n\n\nSatellite RF Input Num (%d): Need Some Logic Here!!!\n\n\n", ulSatRfInputNum);
	}
#endif

	return ERR_OK;
}

STATIC HERROR mgr_schedule_SetRscInfo (SAPI_RscInfo_t *pstRscInfo)
{
	(void)mgr_schedule_SetActionNumToRscInfo (pstRscInfo);
	(void)mgr_schedule_SetRfInputToRscInfo (pstRscInfo);
	(void)mgr_schedule_SetTunerToRscInfo (pstRscInfo);


	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___

#define ___GLOBAL_FUNCTIONS___
HERROR MGR_SCHEDULE_Init (void)
{
	SAPI_RscInfo_t		*pstRscInfo = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	hErr = pmgr_schedule_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("pmgr_schedule_Init err:\n");
		goto END_FUNC;
	}

	// Notify 기본 연결:
	hErr = SAPI_RegisterNotifier (NULL, mgr_schedule_SapiNotifier);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAPI_RegisterNotifier err:\n");
		goto END_FUNC;
	}

	pstRscInfo = (SAPI_RscInfo_t *)OxMGR_Calloc (sizeof(SAPI_RscInfo_t));
	if (NULL == pstRscInfo)
	{
		HxLOG_Error ("Memory allocation err:\n");
		goto END_FUNC;
	}

	// Resource Setting
	(void)mgr_schedule_SetRscInfo (pstRscInfo);
	hErr = SAPI_SetResourceInfo (pstRscInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAPI_SetResourceInfo err:\n");
		goto END_FUNC;
	}

	mgr_schedule_SetRfInputAbility();

#if defined(CONFIG_DEBUG)
	mgr_schedule_RegDebugCmd ();
#endif

	hResult = ERR_OK;
END_FUNC:
	if (NULL != pstRscInfo)					{ OxMGR_Free (pstRscInfo); }
	return hResult;
}

void MGR_SCHEDULE_UpdateAntennaConnectionInfo(void)
{
	mgr_schedule_SetRfInputAbility();
}

HERROR MGR_SCHEDULE_SetScheduleInCancelDB(HUINT32 nRequestID, DxSchedule_t *pstSchedule)
{
	return pmgr_schedule_SetScheduleInCancelDB(nRequestID, pstSchedule);
}


HERROR MGR_SCHEDULE_CheckFrontPannelDisplay (void)
{
	return pmgr_schedule_CheckFrontPannelDisplay ();
}

HERROR MGR_SCHEDULE_StartDescRecordFinedItem(DxRecListData_t	*pstRecData)
{
	return pmgr_schedule_StartDescRecordFinedItem(pstRecData);
}

HERROR MGR_SCHEDULE_ReScheduleBurstUpdate(void)
{
	return pmgr_schedule_ReScheduletBurstUpdate();
}

HERROR MGR_SCHEDULE_CheckConflictSchedule(void)
{
	return pmgr_schedule_CheckConflictSchedule();
}

// // For Batch.
BUS_Callback_t	MGR_SCHEDULE_InitDescrambleRecordBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{
	HxLOG_Trace();
	return NULL;
}

HERROR			MGR_SCHEDULE_DeinitDescrambleRecordBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	HxLOG_Trace();


	return hErr;
}

HERROR			MGR_SCHEDULE_RescheduleDescrambleRecordBatch(DxBatchType_e eBatchType)
{
	HxLOG_Trace();
	return pmgr_schedule_RescheduleDescrambleRecordBatch ();
}

HERROR 			MGR_SCHEDULE_GetBatchDescrambleRecordCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HxLOG_Trace();

	fnBatchProcCBList->fnAppInit = NULL;
	fnBatchProcCBList->fnAppDeinit = NULL;
	fnBatchProcCBList->fnAppReSchedule = MGR_SCHEDULE_RescheduleDescrambleRecordBatch;

	return ERR_OK;
}

HERROR 			MGR_SCHEDULE_GetBatchDescrambleRecordTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	HxLOG_Trace();
	return ERR_OK;
}

Handle_t MGR_SCHEDULE_GetEarliestTrackingSvc(void)
{
	return pmgr_schedule_GetEarliestTrackingSvc();
}
HERROR	MGR_SCHEDULE_MakeRegularOta (void)
{
	return pmgr_schedule_MakeRegularOta();
}

HERROR	MGR_SCHEDULE_SetRecordFailReason (HUINT32 ulSlot, DxSchedule_RemovedReason_e eRemovedReason, HUINT32 eDetailedInfo)
{
	return pmgr_schedule_SetRecordFailReason(ulSlot, eRemovedReason, eDetailedInfo);
}

