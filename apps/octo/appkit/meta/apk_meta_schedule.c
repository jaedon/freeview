/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_schedule.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#include <hlib.h>
#include <rlib.h>
#include <sapi.h>
#include <apk.h>
#include <_apk_int.h>
#include <_apk_resource.h>

#include <oapi_resource.h>

#define ___DECLARATION___

#define ENTER_CRITICAL(pstInst)			do { HxSEMT_Get(pstInst->ulSemId); } while (0)
#define LEAVE_CRITICAL(pstInst)			do { HxSEMT_Release(pstInst->ulSemId); } while (0)

#define UID2SLOT(uid)				((HUINT32)(((uid) == 0) ? 0xFFFF : (uid)-1))

#define ___INTERNAL_STRUCTURES___
typedef void (*apk_meta_sch_LsFn_t) (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule);

typedef struct
{
	void				*pvUserData;
	apk_meta_sch_LsFn_t  pfCbFunc;
} apk_meta_sch_Notifier_t;

typedef struct
{
	HUINT32					 ulSemId;

	void				*pvUserData;
	apk_meta_sch_LsFn_t  pfCbFunc;
	HxList_t			*pstNotiList;			// apk_meta_sch_Notifier_t
} apk_meta_sch_Inst_t;

typedef struct _META_SCHEDULER_t_
{
	HUINT32				accessCode;
	HUINT32				notifierId;
} _META_SCHEDULER_t;



STATIC  HERROR	apk_meta_schedule_RegistNotifier (apk_meta_sch_LsFn_t pfListener, void *pvUserData);
STATIC HERROR apk_meta_schedule_UnregistNotifier (apk_meta_sch_LsFn_t pfListener, void *pvUserData);

#define ___INTERNAL_VARIABLES___
STATIC HxList_t		*s_pstApkMetaSch_NotifierList = NULL;

#define ___PRIVATE_FUNCTION___
STATIC apk_meta_sch_Inst_t *apk_meta_schedule_getInstance (void)
{
	STATIC apk_meta_sch_Inst_t		 s_stInst;
	return &s_stInst;
}


STATIC APK_META_SCH_Conflict_e apk_meta_schedule_convertSapiConfType2ApkConfType (SAPI_ConflictType_e eType)
{
	switch (eType)
	{
	case eSAPI_CONFLICT_TYPE_None:				return eAPK_METASCH_CONFTYPE_None;
	case eSAPI_CONFLICT_TYPE_Tp:				return eAPK_METASCH_CONFTYPE_Tp;
	case eSAPI_CONFLICT_TYPE_OverCapacity:		return eAPK_METASCH_CONFTYPE_OverCapacity;
	case eSAPI_CONFLICT_TYPE_View:				return eAPK_METASCH_CONFTYPE_View;
	case eSAPI_CONFLICT_TYPE_Duplicated:		return eAPK_METASCH_CONFTYPE_Duplicated;
	case eSAPI_CONFLICT_TYPE_PaddingTime:		return eAPK_METASCH_CONFTYPE_PaddingTime;
	case eSAPI_CONFLICT_TYPE_Alternative:		return eAPK_METASCH_CONFTYPE_Alternative;
	case eSAPI_CONFLICT_TYPE_RfOta:				return eAPK_METASCH_CONFTYPE_RfOta;
	case eSAPI_CONFLICT_TYPE_DataOnly:			return eAPK_METASCH_CONFTYPE_DataOnly;
	case eSAPI_CONFLICT_TYPE_Unknown:			return eAPK_METASCH_CONFTYPE_Unknown;
	default:									break;
	}

	return eAPK_METASCH_CONFTYPE_Unknown;
}

STATIC HERROR apk_meta_schedule_convertSapiNotiInfo2ApkNotiInfo (SAPI_NotiInfo_t *punSapiNoti, APK_META_SCH_MsgType_e *peApkMsgType, HUINT32 *pulSlot)
{
	HxLOG_Debug("punSapiNoti->eNotifyType: %d \n", punSapiNoti->eNotifyType);
	switch (punSapiNoti->eNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_SchedulesLoaded:
		*peApkMsgType = eAPK_METASCH_MSG_SchedulesUpdated;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleAdded:
		*peApkMsgType	= eAPK_METASCH_MSG_ScheduleAdded;
		*pulSlot		= punSapiNoti->stSchAdded.ulSlotId;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleChanged:
		*peApkMsgType	= eAPK_METASCH_MSG_ScheduleChanged;
		*pulSlot		= punSapiNoti->stSchChanged.ulSlotId;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
		*peApkMsgType	= eAPK_METASCH_MSG_ScheduleRemoved;
		*pulSlot		= punSapiNoti->stSchRemoved.ulSlotId;
		break;

	case eSAPI_NOTIFY_TYPE_TimeUp:
		HxLOG_Debug("punSapiNoti->stTimeUp.eTimerType: %d \n", punSapiNoti->stTimeUp.eTimerType);
		switch (punSapiNoti->stTimeUp.eTimerType)
		{
		case eSAPI_TIMER_TYPE_Ready:
			*peApkMsgType = eAPK_METASCH_MSG_TimeUpReady;
			break;

		case eSAPI_TIMER_TYPE_Start:
			*peApkMsgType = eAPK_METASCH_MSG_TimeUpStart;
			break;

		case eSAPI_TIMER_TYPE_Stop:
			*peApkMsgType = eAPK_METASCH_MSG_TimeUpStop;
			break;

		case eSAPI_TIMER_TYPE_ReadyTracking:
		case eSAPI_TIMER_TYPE_UpdateDelayed:
			return ERR_FAIL;

		default:
			HxLOG_Error("not defined timer type \n");
			return ERR_FAIL;
		}

		*pulSlot		= punSapiNoti->stTimeUp.ulSlotId;
		break;

	case eSAPI_NOTIFY_TYPE_ConflictScheduleAdded:
		*peApkMsgType	= eAPK_METASCH_MSG_ConflictScheduleAdded;
		*pulSlot		= punSapiNoti->stConfSchAdded.stConflicted.stTriedSchedule.ulSlot;
		break;
	case eSAPI_NOTIFY_TYPE_AlternativeScheduleAdded:
		*peApkMsgType	= eAPK_METASCH_MSG_AlternativeScheduleAdded;
		*pulSlot		= punSapiNoti->stAltSchAdded.stAlternative.stSapiConflict.stTriedSchedule.ulSlot;
		break;

#if 0
	case eAPK_METASCH_MSG_ConflictInReady:
		*peApkMsgType = eSCHEDULE_EVENT_START_ERROR;
		*pulSlot		= punSapiNoti->stTimeUp.ulSlotId;
		break;
#endif

	default:
		HxLOG_Error("not defined notrify type \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HBOOL apk_meta_schedule_checkRecRscConflict (DxSchedule_t *pstScheduleArray, HUINT32 ulNumSchedule)
{

	HUINT32				 *pulActionIdArray = NULL;
	HBOOL				 bConflict = TRUE;
	RxRscReqHandle_t			 hRecReq = (RxRscReqHandle_t)NULL;
	DxService_t 		*pstRecSvc = NULL;
	DxTransponder_t		*pstRecTp = NULL;
	DxAntInfo_t			*pstRecAnt = NULL;
	OxRsc_InitialInfo_t	 stInitInfo;
	RxTrialArg_t		 unTrialArg;
	HERROR				 hErr;
	HUINT32				ulCnt = 0;
//	apk_meta_sch_Inst_t		*pstInst = apk_meta_schedule_getInstance();

//	ENTER_CRITICAL(pstInst);

	HxLOG_Trace();

	if(pstScheduleArray == NULL || ulNumSchedule == 0)
	{
		HxLOG_Error("invalid parameter \n");
		goto END_FUNC;
	}

	hErr = OAPI_RSC_GetInitialInfo(&stInitInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_RSC_GetInitialInfo err:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	hErr = OAPI_RSC_ResetVirtualHandleInfo();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_RSC_ClearVirtualHandleInfo err:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	pulActionIdArray = (HUINT32 *)HLIB_STD_MemCalloc(sizeof(HUINT32) * ulNumSchedule);
	if (NULL == pulActionIdArray)
	{
		HxLOG_Error("[%s:%d] memory allocation failed\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	// 어차피 apk_meta_schedule_checkRecRscConflict() time up ready 시에 check 하는 함수이기 때문에 꼭 record action id를 가져올 필요는 없다.
	// 단순히 비어 있기만 한 action id 라면 rlib 에서 사용하는데 문제가 없다.
	// 오히려 record action id 만 가져온다면 문제가 생길 수 있다. time up ready 시간에 record action 이 꽉 차 있지만, 곧 끝날 아이들일 수 있으므로.
	// 즉, apk_meta_schedule_checkRecRscConflict() 는 simulation 성 함수 이기 때문에 OAPI_RSC_GetEmptyRecordAndBgActionIdArray() 를 사용한다.
	hErr = OAPI_RSC_GetEmptyRecordAndBgActionIdArray(pulActionIdArray, ulNumSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_RSC_GetEmptyRecordAndBgActionIdArray err:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	for (ulCnt = 0; ulCnt < ulNumSchedule; ulCnt++)
	{
		DxSchedule_t		*pstSchedule = &(pstScheduleArray[ulCnt]);

		HxLOG_Debug("check %d-th schedule, slot: %d \n", ulCnt, pstSchedule->ulSlot);

		hRecReq = OAPI_RSC_MakeResourceRequestHandle(pulActionIdArray[ulCnt], stInitInfo.ulRecActPriority, TRUE);
		if ((RxRscReqHandle_t)NULL == hRecReq)
		{
			HxLOG_Error("[%s:%d] OAPI_RSC_MakeResourceRequestHandle err:\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		pstRecSvc = APK_META_SVC_GetService(pstSchedule->ulSvcUid);
		if (NULL == pstRecSvc)
		{
			HxLOG_Error("[%s:%d] APK_META_SVC_GetService err: svcUid(0x%08x)\n", __FUNCTION__, __LINE__, pstSchedule->ulSvcUid);
			goto END_FUNC;
		}

		pstRecTp = APK_META_SVC_GetTransponder(pstRecSvc->tsuid);
		if (NULL == pstRecTp)
		{
			HxLOG_Error("[%s:%d] APK_META_SVC_GetTransponder err: tsuid(%d)\n", __FUNCTION__, __LINE__, pstRecSvc->tsuid);
			goto END_FUNC;
		}

		if (eDxDELIVERY_TYPE_SAT == pstRecTp->deliType)
		{
			// Find the antenna Information
			pstRecAnt = APK_META_ANTENNA_GetInfo(pstRecTp->tuningParam.sat.antInfo.uid);
			if (NULL == pstRecAnt)
			{
				HxLOG_Error("[%s:%d] APK_META_ANTENNA_GetInfo err: antuid(%d)\n", __FUNCTION__, __LINE__, pstRecTp->tuningParam.sat.antInfo.uid);
				goto END_FUNC;
			}

			HxSTD_MemCopy (&(pstRecTp->tuningParam.sat.antInfo), pstRecAnt, sizeof(DxAntInfo_t));
			pstRecTp->tuningParam.sat.antennaType = pstRecAnt->antType;
		}

		unTrialArg.eArgType = eRxARGTYPE_TUNEPARAM;
		unTrialArg.stTuneParam.stTuneParam = pstRecTp->tuningParam;

		// RF-Input Request:
		if (eDxDELIVERY_TYPE_SAT == pstRecTp->deliType)
		{
			hErr = OAPI_RSC_AddRequiredResourceToRequestHandle(hRecReq, eRxRSCTYPE_RFINPUT, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("[%s:%d] OAPI_RSC_AddRequiredResourceToRequestHandle err: eRxRSCTYPE_RFINPUT\n", __FUNCTION__, __LINE__);
				goto END_FUNC;
			}
		}

		// Tuner Request:
		hErr = OAPI_RSC_AddRequiredResourceToRequestHandle(hRecReq, eRxRSCTYPE_TUNER_GROUP, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("[%s:%d] OAPI_RSC_AddRequiredResourceToRequestHandle err: eRxRSCTYPE_TUNER_GROUP\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		hErr = OAPI_RSC_AddArgumentToRequestHandle(hRecReq, &unTrialArg);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("[%s:%d] OAPI_RSC_AddArgumentToRequestHandle err:\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		// 현재 상태에서 Background로 동작하는 것들도 체크해야 한다.
		hErr = OAPI_RSC_TryResourceRequest(hRecReq);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("[%s:%d] Resource conflict will occur::\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		(void)OAPI_RSC_SetResourcesByTrialResults(hRecReq);
		if ((RxRscReqHandle_t)NULL != hRecReq)
		{
			OAPI_RSC_DestroyResourceRequestHandle(hRecReq);
		}

		if (NULL != pstRecSvc)					{ APK_META_SVC_Delete((void*)pstRecSvc); pstRecSvc = NULL;}
		if (NULL != pstRecTp)					{ APK_META_SVC_Delete((void*)pstRecTp); pstRecTp = NULL;}
		if (NULL != pstRecAnt)					{ APK_Free (pstRecAnt); pstRecAnt = NULL;}

	}

	bConflict = FALSE;

END_FUNC:
	if (NULL != pulActionIdArray)				{ HLIB_STD_MemFree(pulActionIdArray); pulActionIdArray = NULL;}
	if (NULL != pstRecSvc)					{ APK_META_SVC_Delete((void*)pstRecSvc); pstRecSvc = NULL;}
	if (NULL != pstRecTp)					{ APK_META_SVC_Delete((void*)pstRecTp); pstRecTp = NULL;}
	if (NULL != pstRecAnt)					{ APK_Free (pstRecAnt); pstRecAnt = NULL;}

	HxLOG_Debug("\n bConflict = %d \n", bConflict);

//	LEAVE_CRITICAL (pstInst);

	return bConflict;

}

STATIC void apk_meta_schedule_CallNotifier (apk_meta_sch_Inst_t *pstInst, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	HxList_t				*pstListItem;

	for (pstListItem = pstInst->pstNotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		apk_meta_sch_Notifier_t		*pstNotifier = (apk_meta_sch_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotifier) && (NULL != pstNotifier->pfCbFunc))
		{
			pstNotifier->pfCbFunc(pstNotifier->pvUserData, eMsgType, ulSlot, pstSchedule);
		}
	}
}

STATIC HERROR apk_meta_schedule_CheckConflictForReadyTimer (apk_meta_sch_Inst_t *pstInst, DxSchedule_t *pstSchedule, HUINT32 ulSlot)
{
	HERROR							hErr = ERR_FAIL;
	DxSchedule_t 					stScheduleForConflictCheck;
	DxSchedule_FindFilterAttr_t 	*pstFilter = NULL;
	DxSchedule_t				*pstFilteredSchArray = NULL;
	DxSchedule_t				*pstFilteredAndConflictTypeSchArray = NULL;
	UNIXTIME					utEndTime = 0;
	HUINT32 				ulNumFilteredSch = 0;
	HUINT32 				ulNumFilteredAndConflict = 0;
	HUINT32 				ulCnt = 0;
	HBOOL					bIsConflictInReady = FALSE;


	hErr = SAPI_GetSchedule(ulSlot, &stScheduleForConflictCheck);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetSchedule failed. slot=(%d) \n");
		goto END_FUNC;
	}

	if ( !( stScheduleForConflictCheck.eRsvType == DxRSVTYPE_RECORD_EBR
		|| stScheduleForConflictCheck.eRsvType == DxRSVTYPE_RECORD_TBR
		|| stScheduleForConflictCheck.eRsvType == DxRSVTYPE_RECORD_SERIES_EBR
		|| stScheduleForConflictCheck.eRsvType == DxRSVTYPE_RECORD_SATRECORD_TBR ))
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("Doesn't need ready conflict check . \n"));
		hErr = ERR_OK;
		goto END_FUNC;
	}

	pstFilter = (DxSchedule_FindFilterAttr_t*)HLIB_STD_MemCalloc(sizeof(DxSchedule_FindFilterAttr_t));
	if (pstFilter == NULL)
	{
		HxLOG_Error("alloc fail \n");
		goto END_FUNC;
	}

	SAPI_FindScheduleFilterInit(pstFilter);

	hErr = SAPI_GetPaddingStartEndTime(pstSchedule, &(pstFilter->utStartTime), &utEndTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SAPI_GetPaddingStartEndTime failed. slot=(%d) \n",pstSchedule->ulSlot);
		goto END_FUNC;
	}

	hErr = SAPI_FindScheduleUsingFilter(pstFilter, &ulNumFilteredSch, &pstFilteredSchArray);
	HxLOG_Debug("ulNumFilteredSch: %d \n", ulNumFilteredSch);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SAPI_FindScheduleUsingFilter failed. \n");
		goto END_FUNC;
	}

	pstFilteredAndConflictTypeSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumFilteredSch);
	if (NULL == pstFilteredAndConflictTypeSchArray)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}


	for (ulCnt = 0; ulCnt < ulNumFilteredSch; ulCnt++)
	{
		DxSchedule_t		*pstScheduleTemp = &(pstFilteredSchArray[ulCnt]);

#ifdef CONFIG_DEBUG
		SAPI_DbgPrintSchedule(&(pstFilteredSchArray[ulCnt]));
#endif
		if(pstScheduleTemp->eRsvType == DxRSVTYPE_RECORD_EBR
			||pstScheduleTemp->eRsvType == DxRSVTYPE_RECORD_TBR
			||pstScheduleTemp->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR
			||pstScheduleTemp->eRsvType == DxRSVTYPE_RECORD_SATRECORD_TBR
			||pstScheduleTemp->eRsvType == DxRSVTYPE_RECORD_INSTANT
			||pstScheduleTemp->eRsvType == DxRSVTYPE_RECORD_DELAYED
			)
		{
			HxSTD_memcpy(&(pstFilteredAndConflictTypeSchArray[ulNumFilteredAndConflict])
				, pstScheduleTemp, sizeof(DxSchedule_t));

			ulNumFilteredAndConflict++;
		}
	}

	if (ulNumFilteredAndConflict == 0 )
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("There is no overlapped & conflict type schedules exists. \n"));
		hErr = ERR_OK;
		goto END_FUNC;
	}

	// stScheduleForConflictCheck 도 당연히 pstFilteredAndConflictTypeSchArray 에 포함된다.
	bIsConflictInReady = apk_meta_schedule_checkRecRscConflict(pstFilteredAndConflictTypeSchArray, ulNumFilteredAndConflict);
	if (bIsConflictInReady == TRUE)
	{
		apk_meta_schedule_CallNotifier(pstInst, eAPK_METASCH_MSG_ConflictInReady, ulSlot, &stScheduleForConflictCheck);
	}
	else
	{
		(void)APK_META_SCHEDULE_ResumeSchedule(stScheduleForConflictCheck.ulSlot);
	}


END_FUNC:
	SAPI_FindScheduleUsingFilterFreeList(pstFilteredSchArray);

	if (pstFilter != NULL)
	{
		HLIB_STD_MemFree(pstFilter);
		pstFilter = NULL;
	}

	if (pstFilteredAndConflictTypeSchArray != NULL)
	{
		HLIB_STD_MemFree(pstFilteredAndConflictTypeSchArray);
		pstFilteredAndConflictTypeSchArray = NULL;
	}

	return ERR_OK;
}

// TODO: 논의 되어야 할 사항.
//apk 단에서 conflict in ready 를 check 할 수 밖에 없는 근본적인 원인은 sama에서 live에 대한 정확한 정보를 알 수 없기 때문이다.
//이것에 대한 히스토리는,
//애초에 rlib 이 설계될 당시 pal -> rlib 을 부르는데 action 을 set 하면서 action type에 대한 것은 pal 에서 알 필요가 없다는 의견이 있었다.
//(pal은 단순히 action에 대한 set만 해주고, action type에 대한 것들까지 알게되면 안된다는 원칙..action type에 관한 것은 mgr에서 관리)
//이것이 시초가 되어 만약 sama나 기타 다른 process들이 rlib을 통하여 resource 상태를 알 수 있게 된다 하더라도,
//정작 어떤 이유로(즉 어떤 action이) tuner가 점유되었는지 알수는 없게 되어버리는 상황이 생기고, priority에 의한 여러 check 가 힘들어지게 된다.
//이를 수정하기 위해서는,
//1. rlib으로 action type에 대한 정보도 같이 주고 (여전히 pal단으로 action type을 내리기 싫다면 skip...)
//2. obama가 resource를 관리하는 process이므로, obama-> oapi resource 로 cache update 시에도 action type을 같이 주고,
//   - 2.1   1번을 했을 경우는 oapi resource 에서 rlib으로 set 시 action type을 같이
//   - 2.1   1번을 skip 한 경우는 그냥 oapi resource 내부 cache 에만 보관.
//3. sama에서 oapi resource 를 보게 한 후에, sama단에서 conflict in ready 를 체크할 수 있다.

STATIC HERROR apk_meta_schedule_cbNotifier (void *pvUserData, SAPI_NotiInfo_t *punNotify)
{
	HUINT32					 ulSlot = 0;
	APK_META_SCH_MsgType_e	 eMsgType;
	apk_meta_sch_Inst_t		*pstInst = (apk_meta_sch_Inst_t *)pvUserData;
	HERROR					hErr = ERR_FAIL;
	DxSchedule_t	*pstSchedule = NULL;

	if (NULL == punNotify)			{ return ERR_FAIL; }

/*
	if(pstInst->pfCbFunc == NULL) // No Listeners:
	{
		return ERR_OK;
	}
*/
	if (NULL == pstInst->pstNotiList)  // No Listeners:
	{
		return ERR_OK;
	}


	hErr = apk_meta_schedule_convertSapiNotiInfo2ApkNotiInfo(punNotify, &eMsgType, &ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug("[%s:%d] apk_meta_schedule_convertSapiNotiInfo2ApkNotiInfo err:\n", __FUNCTION__, __LINE__);
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	HxLOG_Debug("eMsgType: %d, ulSlot: %d \n", eMsgType, ulSlot);

	if(eMsgType == eAPK_METASCH_MSG_TimeUpReady
		|| eMsgType == eAPK_METASCH_MSG_TimeUpStart
		|| eMsgType == eAPK_METASCH_MSG_TimeUpStop)
	{
		pstSchedule = (DxSchedule_t *)APK_Malloc(sizeof(DxSchedule_t));
		if(pstSchedule != NULL)
		{
			HxSTD_memcpy(pstSchedule, punNotify->stTimeUp.pstSchedule, sizeof(DxSchedule_t));
		}
	}
	else if(eMsgType == eAPK_METASCH_MSG_ScheduleRemoved)
	{
		pstSchedule = (DxSchedule_t *)APK_Malloc(sizeof(DxSchedule_t));
		if(pstSchedule != NULL)
		{
			HxSTD_memcpy(pstSchedule, punNotify->stSchRemoved.pstSchedule, sizeof(DxSchedule_t));
		}
	}
	else if(eMsgType == eAPK_METASCH_MSG_ScheduleAdded)
	{
		pstSchedule = (DxSchedule_t *)APK_Malloc(sizeof(DxSchedule_t));
		if(pstSchedule != NULL)
		{
			HxSTD_memcpy(pstSchedule, punNotify->stSchAdded.pstSchedule, sizeof(DxSchedule_t));
		}
	}
	else if(eMsgType == eAPK_METASCH_MSG_ScheduleChanged)
	{
		pstSchedule = (DxSchedule_t *)APK_Malloc(sizeof(DxSchedule_t));
		if(pstSchedule != NULL)
		{
			HxSTD_memcpy(pstSchedule, punNotify->stSchChanged.pstSchedule, sizeof(DxSchedule_t));
		}
	}

/*
	if (NULL != pstInst->pfCbFunc)
	{
		pstInst->pfCbFunc(pstInst->pvUserData, eMsgType, ulSlot, pstSchedule);
	}
*/
	apk_meta_schedule_CallNotifier(pstInst, eMsgType, ulSlot, pstSchedule);

	if (eAPK_METASCH_MSG_TimeUpReady == eMsgType)
	{
		apk_meta_schedule_CheckConflictForReadyTimer(pstInst, pstSchedule, ulSlot);
	}

	hErr = ERR_OK;

END_FUNC:
	if(pstSchedule != NULL)
	{
		APK_Free(pstSchedule);
		pstSchedule = NULL;
	}

	return hErr;
}

//////////////////////////////////////////////
//
// 구 appkit
//
///////////////////////////////////////////////
static _META_SCHEDULER_t	*	apk_meta_schedule_GetManager (void);

#define ____CONVERT_FUNCTIONS_____

STATIC void	apk_meta_schedule_CbFreeEvtEmitter (HINT32 nArgc, void *apArgV[])
{
	void	*pvDataSchedule;


	HxLOG_Assert(nArgc == 5);

	pvDataSchedule = apArgV[4];

	if (pvDataSchedule)
	{
		HLIB_STD_MemFree(pvDataSchedule);
		pvDataSchedule = NULL;
	}
}

static void apk_meta_schedule_OnNotifiedUsingDxSchedule (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	HxList_t					*pstListItem;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs;

	for (pstListItem = s_pstApkMetaSch_NotifierList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		apk_meta_sch_Notifier_t			*pstNotifier = (apk_meta_sch_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotifier) && (NULL != pstNotifier->pfCbFunc))
		{
			DxSchedule_t		*pstDstSchedule = NULL;

			if (NULL != pstSchedule)
			{
				pstDstSchedule = (DxSchedule_t *)HLIB_STD_MemDup(pstSchedule, sizeof(DxSchedule_t));
			}


			HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
			stApkEvtArgs.pfnFreeFunc = apk_meta_schedule_CbFreeEvtEmitter;
			stApkEvtArgs.ulArgCount = 5;
			stApkEvtArgs.apArgV[0] = (void *)pstNotifier->pfCbFunc;
			stApkEvtArgs.apArgV[1] = (void *)pstNotifier->pvUserData;
			stApkEvtArgs.apArgV[2] = (void *)eMsgType;
			stApkEvtArgs.apArgV[3] = (void *)ulSlot;
			stApkEvtArgs.apArgV[4] = (void *)pstDstSchedule;
			APK_EVENT_Emit(&stApkEvtArgs);
		}
	}
}

STATIC HERROR apk_meta_schedule_Init (void)
{
	apk_meta_sch_Inst_t	*pstInst = apk_meta_schedule_getInstance();
	HERROR				 hErr;

	HxSTD_MemSet (pstInst, 0, sizeof(apk_meta_sch_Inst_t));

	hErr = HxSEMT_Create(&(pstInst->ulSemId), "apk_metasch", 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] HxSEMT_Create err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	hErr = SAPI_RegisterNotifier((void *)pstInst, apk_meta_schedule_cbNotifier);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_RegisterNotifier err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	hErr = OAPI_RSC_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_RSC_Init err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static _META_SCHEDULER_t *	apk_meta_schedule_GetManager (void)
{
	static _META_SCHEDULER_t s_manager;

	{
		STATIC HBOOL			 s_bInited = FALSE;

		if (TRUE != s_bInited)
		{
			(void)apk_meta_schedule_Init();
			(void)apk_meta_schedule_RegistNotifier (apk_meta_schedule_OnNotifiedUsingDxSchedule, (void *)&s_manager);
			s_bInited = TRUE;
		}
	}

	return &s_manager;
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
STATIC HBOOL apk_meta_schedule_cmpNotifier (void *pvUserData, void *pvListItem)
{
	apk_meta_sch_Notifier_t		*pstUserNoti = (apk_meta_sch_Notifier_t *)pvUserData;
	apk_meta_sch_Notifier_t		*pstListNoti = (apk_meta_sch_Notifier_t *)pvListItem;

	if ((NULL != pstUserNoti) && (NULL != pstListNoti))
	{
		if ((pstUserNoti->pfCbFunc == pstListNoti->pfCbFunc) &&
			(pstUserNoti->pvUserData == pstListNoti->pvUserData))
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HERROR apk_meta_schedule_RegistNotifier (apk_meta_sch_LsFn_t pfListener, void *pvUserData)
{
	apk_meta_sch_Inst_t		*pstInst = apk_meta_schedule_getInstance();
	HERROR					 hResult = ERR_FAIL;

	if (NULL == pfListener)
	{
		HxLOG_Error("[%s:%d] Listener NULL\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	ENTER_CRITICAL(pstInst);

	pstInst->pfCbFunc		= pfListener;
	pstInst->pvUserData		= pvUserData;

	{
		apk_meta_sch_Notifier_t  stNotifier;
		apk_meta_sch_Notifier_t *pstNotifier;

		stNotifier.pfCbFunc		= pfListener;
		stNotifier.pvUserData	= pvUserData;

		if (HLIB_LIST_FindEx(pstInst->pstNotiList, (const void *)&stNotifier, apk_meta_schedule_cmpNotifier) == NULL)
		{
			pstNotifier = (apk_meta_sch_Notifier_t *)APK_Malloc(sizeof(apk_meta_sch_Notifier_t));

			if (NULL != pstNotifier)
			{
				HxSTD_MemCopy(pstNotifier, &stNotifier, sizeof(apk_meta_sch_Notifier_t));
				pstInst->pstNotiList = HLIB_LIST_Append(pstInst->pstNotiList, (void *)pstNotifier);
			}
		}
	}

	hResult = ERR_OK;

//END_FUNC:
	LEAVE_CRITICAL (pstInst);

	return hResult;
}

STATIC HERROR apk_meta_schedule_UnregistNotifier (apk_meta_sch_LsFn_t pfListener, void *pvUserData)
{
	apk_meta_sch_Inst_t		*pstInst = apk_meta_schedule_getInstance();

	HERROR					 hResult = ERR_FAIL;

	if (NULL == pfListener)
	{
		HxLOG_Error("[%s:%d] Listener NULL\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	ENTER_CRITICAL(pstInst);

	pstInst->pfCbFunc		= NULL;
	pstInst->pvUserData	= NULL;

	{
		apk_meta_sch_Notifier_t  stNotifier;
		apk_meta_sch_Notifier_t *pstNotifier;
		HxList_t				*pstListItem;

		stNotifier.pfCbFunc		= pfListener;
		stNotifier.pvUserData	= pvUserData;

		pstListItem = HLIB_LIST_FindEx(pstInst->pstNotiList, (const void *)&stNotifier, apk_meta_schedule_cmpNotifier);
		if (NULL != pstListItem)
		{
			pstNotifier = (apk_meta_sch_Notifier_t *)HLIB_LIST_Data(pstListItem);

			if (NULL != pstNotifier)
			{
				pstInst->pstNotiList = HLIB_LIST_Remove(pstInst->pstNotiList, (const void *)pstNotifier);
				APK_Free(pstNotifier);
			}
		}
	}

	LEAVE_CRITICAL(pstInst);
	return hResult;
}

HERROR APK_META_SCHEDULE_RegisterNotifier (APK_META_SCH_NotiFunc_t pfNotifier, void *pvUserData)
{
	apk_meta_sch_Notifier_t  stNotifier;
	apk_meta_sch_Notifier_t *pstNotifier;

	stNotifier.pfCbFunc 	= pfNotifier;
	stNotifier.pvUserData	= pvUserData;

	if (HLIB_LIST_FindEx(s_pstApkMetaSch_NotifierList, (const void *)&stNotifier, apk_meta_schedule_cmpNotifier) == NULL)
	{
		pstNotifier = (apk_meta_sch_Notifier_t *)APK_Malloc(sizeof(apk_meta_sch_Notifier_t));

		if (NULL != pstNotifier)
		{
			HxSTD_MemCopy(pstNotifier, &stNotifier, sizeof(apk_meta_sch_Notifier_t));
			s_pstApkMetaSch_NotifierList = HLIB_LIST_Append(s_pstApkMetaSch_NotifierList, (void *)pstNotifier);
		}
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_UnregisterNotifier (APK_META_SCH_NotiFunc_t pfNotifier, void *pvUserData)
{
	apk_meta_sch_Notifier_t  stNotifier;
	apk_meta_sch_Notifier_t *pstNotifier;
	HxList_t				*pstListItem;

	stNotifier.pfCbFunc 	= pfNotifier;
	stNotifier.pvUserData	= pvUserData;

	pstListItem = HLIB_LIST_FindEx(s_pstApkMetaSch_NotifierList, (const void *)&stNotifier, apk_meta_schedule_cmpNotifier);
	if (NULL != pstListItem)
	{
		pstNotifier = (apk_meta_sch_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstNotifier)
		{
			s_pstApkMetaSch_NotifierList = HLIB_LIST_Remove(s_pstApkMetaSch_NotifierList, (const void *)pstNotifier);
			APK_Free(pstNotifier);
		}
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_GetSchedule (HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	HERROR			 hErr;

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	hErr = SAPI_GetSchedule(ulSlot, pstSchedule);

	return hErr;
}

HERROR APK_META_SCHEDULE_GetScheduleByIndex (HUINT32 ulIndex, DxSchedule_t *pstSchedule)
{
	HERROR			 hErr;

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	hErr = SAPI_GetScheduleByIndex(ulIndex, pstSchedule);

	return hErr;
}

HERROR APK_META_SCHEDULE_GetScheduleByType (DxRsvType_e eType, HUINT32 ulIndex, DxSchedule_t *pstSchedule)
{
	HERROR			 hErr;

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	hErr = SAPI_GetScheduleByType(eType, ulIndex, pstSchedule);

	return hErr;
}

HERROR APK_META_SCHEDULE_CountSchedule (HUINT32 *pulNumSch)
{
	HERROR			 hErr;

	if (NULL == pulNumSch)						{ return ERR_FAIL; }

	hErr = SAPI_CountSchedule(pulNumSch);
	return hErr;
}

HERROR APK_META_SCHEDULE_CountScheduleByType (DxRsvType_e eType, HUINT32 *pulNumSch)
{
	HERROR			 hErr;

	if (NULL == pulNumSch)						{ return ERR_FAIL; }

	hErr = SAPI_CountScheduleByType(eType, pulNumSch);

	return hErr;
}

HERROR APK_META_SCHEDULE_AddSchedule (DxSchedule_t *pstSchedule, HUINT32 *pulSlot)
{
	HUINT32			 ulSlot;
	SAPI_Conflict_t	 stConflict;
	HERROR			 hErr;

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	hErr = SAPI_GetAvailableSlot(&ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_GetAvailableSlot err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Debug("[%s:%d] Available Slot = %d\n", __FUNCTION__, __LINE__, ulSlot);
	if (NULL != pulSlot)				{ *pulSlot = ulSlot; }

	hErr = SAPI_MakeSchedule(ulSlot, pstSchedule, &stConflict);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_MakeSchedule err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_UpdateSchedule (HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	SAPI_Conflict_t	 stConflict;
	HERROR			 hErr;

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	hErr = SAPI_UpdateSchedule(ulSlot, pstSchedule, &stConflict, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_UpdateSchedule err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_UpdateSeries (void)
{
	HERROR			 hErr;

	hErr = SAPI_UpdateSeries();
	if (ERR_OK != hErr)
	{
		HxLOG_Error(" SAPI_UpdateSeries err \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_CancelSchedule (HUINT32 ulSlot)
{
	HERROR			 hErr;

	hErr = SAPI_CancelSchedule(ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_CancelSchedule err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_StopSchedule (HUINT32 ulSlot)
{
	HERROR			 hErr;

	hErr = SAPI_StopSchedule(ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_StopSchedule err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_PauseSchedule (HUINT32 ulSlot)
{
	HERROR			 hErr;

	hErr = SAPI_PauseSchedule(ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_PauseSchedule err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_META_SCHEDULE_ResumeSchedule (HUINT32 ulSlot)
{
	HERROR			 hErr;

	hErr = SAPI_ResumeSchedule(ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_ResumeSchedule err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

APK_META_SCH_Conflict_t *APK_META_SCHEDULE_GetConflictResult (HUINT32 ulSlot)
{
	HUINT32					 ulCnt;
	SAPI_Conflict_t			 stSapiConf;
	APK_META_SCH_Conflict_t	*pstApkConf = NULL, *pstConfResult = NULL;
	HERROR					 hErr;

	hErr = SAPI_GetConflictResult(ulSlot, &stSapiConf);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_GetConflictResult err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		goto END_FUNC;
	}

	pstApkConf = (APK_META_SCH_Conflict_t *)APK_Calloc(sizeof(APK_META_SCH_Conflict_t));
	if (NULL == pstApkConf)
	{
		HxLOG_Error("[%s:%d] Memory allocation failed:\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	pstApkConf->eType = apk_meta_schedule_convertSapiConfType2ApkConfType(stSapiConf.eType);
	HxSTD_MemCopy(&(pstApkConf->stTriedSchedule), &(stSapiConf.stTriedSchedule), sizeof(DxSchedule_t));

	if ((stSapiConf.ulNumConflict > 0) && (NULL != stSapiConf.pstConflictArray))
	{
		pstApkConf->ulNumConflict = stSapiConf.ulNumConflict;
		pstApkConf->pstConfArray = (APK_META_SCH_ConfItem_t *)APK_Calloc(sizeof(APK_META_SCH_ConfItem_t) * stSapiConf.ulNumConflict);

		if (NULL == pstApkConf->pstConfArray)
		{
			HxLOG_Error("[%s:%d] Memory allocation failed:\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		pstApkConf->ulNumConflict = stSapiConf.ulNumConflict;
		for (ulCnt = 0; ulCnt < stSapiConf.ulNumConflict; ulCnt++)
		{
			SAPI_ConflictItem_t			*pstSapiItem = &(stSapiConf.pstConflictArray[ulCnt]);
			APK_META_SCH_ConfItem_t		*pstApkItem = &(pstApkConf->pstConfArray[ulCnt]);

			pstApkItem->ulSlot		= pstSapiItem->ulSlot;
		}
	}

	pstConfResult = pstApkConf;
	pstApkConf = NULL;

END_FUNC:
	if (NULL != pstApkConf)					{ APK_META_SCHEDULE_FreeConflictResult(pstApkConf); }
	return pstConfResult;
}

APK_META_SCH_Alternative_t *APK_META_SCHEDULE_GetAlternativeResult (HUINT32 ulSlot)
{
	SAPI_Alternative_t			stSapiAlt;
	APK_META_SCH_Conflict_t		*pstApkConf = NULL;
	APK_META_SCH_Alternative_t	*pstApkAlt = NULL;
	APK_META_SCH_Alternative_t	*pstAltResult = NULL;
	HERROR						hErr;
	HUINT32						ulCnt;

	hErr = SAPI_GetAlternativeResult(ulSlot, &stSapiAlt);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] SAPI_GetAlternativeResult err: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
		return NULL;
	}

	pstApkAlt = (APK_META_SCH_Alternative_t *)APK_Calloc(sizeof(APK_META_SCH_Alternative_t));
	if (NULL == pstApkAlt)
	{
		HxLOG_Error("[%s:%d] Memory allocation failed:\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	pstApkConf			= &(pstApkAlt->stConfResult);
	pstApkConf->eType	= apk_meta_schedule_convertSapiConfType2ApkConfType(stSapiAlt.stSapiConflict.eType);
	HxSTD_MemCopy(&(pstApkConf->stTriedSchedule), &(stSapiAlt.stSapiConflict.stTriedSchedule), sizeof(DxSchedule_t));

	if ((stSapiAlt.stSapiConflict.ulNumConflict > 0) && (NULL != stSapiAlt.stSapiConflict.pstConflictArray))
	{
		pstApkConf->ulNumConflict = stSapiAlt.stSapiConflict.ulNumConflict;
		pstApkConf->pstConfArray = (APK_META_SCH_ConfItem_t *)APK_Calloc(sizeof(APK_META_SCH_ConfItem_t) * stSapiAlt.stSapiConflict.ulNumConflict);

		if (NULL == pstApkConf->pstConfArray)
		{
			HxLOG_Error("[%s:%d] Memory allocation failed:\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		pstApkConf->ulNumConflict = stSapiAlt.stSapiConflict.ulNumConflict;
		for (ulCnt = 0; ulCnt < stSapiAlt.stSapiConflict.ulNumConflict; ulCnt++)
		{
			SAPI_ConflictItem_t			*pstSapiItem = &(stSapiAlt.stSapiConflict.pstConflictArray[ulCnt]);
			APK_META_SCH_ConfItem_t		*pstApkItem = &(pstApkConf->pstConfArray[ulCnt]);

			pstApkItem->ulSlot		= pstSapiItem->ulSlot;
		}
	}

	pstApkAlt->stAltItems.ulNumAlternatives	= stSapiAlt.stAltItems.ulNumAlternatives;
	for ( ulCnt = 0; ulCnt < stSapiAlt.stAltItems.ulNumAlternatives; ulCnt++)
	{
		pstApkAlt->stAltItems.astAltItemArray[ulCnt].ulSlot			= stSapiAlt.stAltItems.astAltItemArray[ulCnt].ulSlot;
		pstApkAlt->stAltItems.astAltItemArray[ulCnt].ulAltEvtId		= stSapiAlt.stAltItems.astAltItemArray[ulCnt].ulAltEvtId;
		pstApkAlt->stAltItems.astAltItemArray[ulCnt].ulAltSvcUid	= stSapiAlt.stAltItems.astAltItemArray[ulCnt].ulAltSvcUid;
	}

	pstAltResult = pstApkAlt;
	pstApkAlt = NULL;

END_FUNC:
	if (NULL != pstApkAlt) 				{ APK_META_SCHEDULE_FreeAlternativeResult(pstApkAlt); }
	return pstAltResult;

}
void APK_META_SCHEDULE_FreeConflictResult (APK_META_SCH_Conflict_t *pstConflict)
{
	if (NULL != pstConflict)
	{
		if (NULL != pstConflict->pstConfArray)
		{
			APK_Free(pstConflict->pstConfArray);
		}

		APK_Free(pstConflict);
	}
}

void APK_META_SCHEDULE_FreeAlternativeResult (APK_META_SCH_Alternative_t *pstAlternative)
{
	if (NULL != pstAlternative)
	{
		if (NULL != pstAlternative->stConfResult.pstConfArray)
		{
			APK_Free(pstAlternative->stConfResult.pstConfArray);
		}

		APK_Free(pstAlternative);
	}
}

HERROR		APK_META_SCHEDULE_Refresh (ApkMetaSched_Handle_t hMeta)
{
	SAPI_RefreshScheduleData();
	return ERR_OK;
}

HBOOL		APK_META_IsEnableTBRPadding(void)
{
	return SAPI_IsEnableTBRPadding();
}

DxSchedule_Spec_e	APK_META_SCHEDULE_GetSpec(void)
{
	HERROR		hErr;
	SAPI_SpecOption_t	stSpec;

	hErr = SAPI_GetSpecOptions(&stSpec);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SAPI_GetSpecOptions failed. \n");
		return eDxSchedule_Spec_None;
	}

	return stSpec.eSpec;
}


#define _____OLD_APPKIT_____
///////////////////////////////////////////////
//
//  구 Appkit
//
/////////////////////////////////////////////
ApkMetaSched_Handle_t	APK_META_SCHEDULE_GetInstance (void)
{
	HxLOG_Trace();

	return (ApkMetaSched_Handle_t)apk_meta_schedule_GetManager();
}

HERROR		APK_META_SCHEDULE_Confirm (ApkMetaSched_Handle_t hMeta, HINT32 uid, HINT32 type, HINT32 check)
#if 1 // 임시 코드.
{
	HUINT32				 ulSlot = UID2SLOT (uid);
	HERROR				 hErr;
//	HUINT32						 ulCnt, ulNumSch = 0;
	DxSchedule_t				*pstSchArray = NULL;

	HxLOG_Debug("ulSlot: %d, uid: %d, type: %d, check: %d \n", ulSlot, uid, type, check);

	// web ap에서 내려주는 정확한 type/check 값 확인 필요.
	if(type == 0)//record
	{
		if(check == 1) // select yes.
		{
			// select yes 선택시 윗단에서 zapping 할 채널이 tunable 한지 check 해서 tunable 하면 zapping
			// 지금 당장 tunable 하지 않으면 record 시작할 때 까지 기다린 후에, record가 시작되었다고 noti가 가면
			// 현재 live 가 tunable 한지 check 해서 tunable 하면 그냥 있음.
			// tunable 하지 않으면 record 가 시작된 채널로 zapping.
#if 0
			(void)SAPI_CountSchedule(&ulNumSch);
			if (ulNumSch > 0)
			{
				pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
				if (NULL == pstSchArray)
				{
					HxLOG_Error("[%s:%d] memory allocation failed\n", __FUNCTION__, __LINE__);
					goto END_FUNC;
				}

				// apk 에서 conflict 난 schedule 을 알 수가 없어 일단 running 인 아이들을 모두 stop.
				hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("[%s:%d] SAPI_GetAllSchedules err:\n", __FUNCTION__, __LINE__);
					goto END_FUNC;
				}
				for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
				{
					DxSchedule_t		*pstSchedule = &(pstSchArray[ulCnt]);

					switch (pstSchedule->eRsvType)
					{
					case DxRSVTYPE_RECORD_EBR:
					case DxRSVTYPE_RECORD_TBR:
					case DxRSVTYPE_RECORD_INSTANT:
					case DxRSVTYPE_RECORD_DELAYED:
					case DxRSVTYPE_RECORD_SERIES_EBR:
					case DxRSVTYPE_RECORD_SATRECORD_TBR:
						if (DxRSVSTATUS_RUNNING == pstSchedule->eStatus)
						{
							hErr = SAPI_StopSchedule(pstSchedule->ulSlot);
							if (ERR_OK != hErr)
							{
								HxLOG_Error ("[%s:%d] SAPI_CancelSchedule failed: ulSlot(%d)\n", __FUNCTION__, __LINE__, pstSchedule->ulSlot);
							}

						}

						break;

					default:
						break;
					}
				}

			}
#endif
		}
		else if(check == 0)// select no.
		{
			hErr = SAPI_StopSchedule(ulSlot);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("[%s:%d] SAPI_CancelSchedule failed: ulSlot(%d)\n", __FUNCTION__, __LINE__, ulSlot);
			}

		}
	}
#if 0
	// should be checked.
	else if(type == ) // reminder.
	{
		if(check == ) // yes
		{

		}
		else if(check == ) // no
		{

		}
	}
#endif
	else
	{
		//should be checked. // reminder, HLS, power off, cancel tsr etc...
	}


//END_FUNC:

	hErr = APK_META_SCHEDULE_ResumeSchedule(ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_META_SCHEDULE_ResumeSchedule err:\n", __FUNCTION__, __LINE__);
	}

	if (pstSchArray)
		HLIB_STD_MemFree(pstSchArray);

	return hErr;
}

#else
{
	HUINT32				 ulSlot = UID2SLOT (uid);
	HERROR				 hErr;

	hErr = APK_META_SCHEDULE_ResumeSchedule(ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_META_SCHEDULE_ResumeSchedule err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR		APK_META_SCHEDULE_RemoveWholeSeries (ApkMetaSched_Handle_t hMeta, HINT32 uid)
{
	// ToDo: Series Recording 관련은 나중에 구현
	HUINT32				 ulSlot = UID2SLOT (uid);
	HERROR				 hErr;

	hErr = SAPI_CancelWholeSeries (ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_META_SCHEDULE_CancelSchedule err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}


// Series Recording 의 경우 User가 원하면 현재 Conflict나는 이벤트만 삭제하고 나머지 시리즈는 녹화 가능해야 한다.
HERROR		APK_META_SCHEDULE_RemoveFirstItemOnSeries (ApkMetaSched_Handle_t hMeta, HINT32 uid)
{
	// ToDo: Series Recording 관련은 나중에 구현
	HUINT32				 ulSlot = UID2SLOT (uid);
	HERROR				 hErr;

	hErr = SAPI_ChangeSeriesScheduleToNext (ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_META_SCHEDULE_CancelSchedule err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL	APK_META_SCHEDULE_SetName (HINT32 sched_uid, HCHAR *name)
{
	HUINT32				 ulSlot = UID2SLOT (sched_uid);
	DxSchedule_t		 stDxItemSch;
	HERROR				 hErr;

	if (NULL != name)
	{
		hErr = APK_META_SCHEDULE_GetSchedule (ulSlot, &stDxItemSch);
		if (ERR_OK == hErr)
		{
			switch (stDxItemSch.eRsvType)
			{
			case DxRSVTYPE_WATCHING_EBR:
			case DxRSVTYPE_WATCHING_TBR:
			case DxRSVTYPE_WATCHING_SERIES_EBR:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stWch.szEvtName, name, DxRSV_MAX_EVENT_NAME);
				break;

			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_TBR:
			case DxRSVTYPE_RECORD_INSTANT:
			case DxRSVTYPE_RECORD_DELAYED:
			case DxRSVTYPE_DUBBING:
			case DxRSVTYPE_RECORD_SERIES_EBR:
			case DxRSVTYPE_RECORD_SATRECORD_TBR:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stRec.szEventName, name, DxRSV_MAX_EVENT_NAME);
				break;

			case DxRSVTYPE_NETSTREAMING_LIVE:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stStrm.szAppDisplayString, name, DxRSV_APPDISPLAYSTRING_LEN);
				break;

			case DxRSVTYPE_NETSTREAMING_FILE:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stStrm.szAppDisplayString, name, DxRSV_APPDISPLAYSTRING_LEN);
				break;

			default:
				return FALSE;
			}

			hErr = APK_META_SCHEDULE_UpdateSchedule (ulSlot, &stDxItemSch);
			if (ERR_OK == hErr)
			{
				return TRUE;
			}
		}

	}

	return FALSE;
}

/* end of file */
