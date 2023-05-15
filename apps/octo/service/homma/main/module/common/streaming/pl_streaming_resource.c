/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


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
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "int_streaming.h"

#include <sapi.h>


/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/


//#define __DEBUG_CONFIRMSEM__
#ifdef __DEBUG_CONFIRMSEM__
STATIC HINT32		s_nConfirmSemCnt = 0;
#define ENTER_CONFIRMSEM		{ s_nConfirmSemCnt++; if(s_nConfirmSemCnt > 1) HxLOG_Error("++ ENTER_CONFIRMSEM (%d, %d) ++\n\n", s_nConfirmSemCnt, HLIB_STD_GetSystemTick()); else HxLOG_Warning("++ ENTER_CONFIRMSEM (%d, %d) ++\n", s_nConfirmSemCnt); VK_SEM_Get (s_ulPLStreamRes_ConfirmSemId); }
#define LEAVE_CONFIRMSEM		{ VK_SEM_Release (s_ulPLStreamRes_ConfirmSemId); s_nConfirmSemCnt--; if(s_nConfirmSemCnt > 0) HxLOG_Error("-- LEAVE_CONFIRMSEM (%d, %d) --\n\n", s_nConfirmSemCnt, HLIB_STD_GetSystemTick()); else HxLOG_Warning("-- LEAVE_CONFIRMSEM (%d, %d) --\n", s_nConfirmSemCnt, HLIB_STD_GetSystemTick()); }
#else
#define ENTER_CONFIRMSEM		VK_SEM_Get (s_ulPLStreamRes_ConfirmSemId)
#define LEAVE_CONFIRMSEM		VK_SEM_Release (s_ulPLStreamRes_ConfirmSemId)
#endif

//#define __DEBUG_GSEM__
#ifdef __DEBUG_GSEM__
STATIC HINT32		s_nGsemCnt = 0;
#define ENTER_GSEM		{ s_nGsemCnt++; if(s_nGsemCnt > 1) HxLOG_Error("++ ENTER_GSEM (%d, %d) ++\n\n", s_nGsemCnt, HLIB_STD_GetSystemTick()); else HxLOG_Warning("++ ENTER_GSEM (%d, %d) ++\n", s_nGsemCnt, HLIB_STD_GetSystemTick()); VK_SEM_Get (s_ulPLStreamRes_SemId); }
#define LEAVE_GSEM		{ VK_SEM_Release (s_ulPLStreamRes_SemId); s_nGsemCnt--; if(s_nGsemCnt > 0) HxLOG_Error("-- LEAVE_GSEM (%d, %d) --\n\n", s_nGsemCnt, HLIB_STD_GetSystemTick()); else HxLOG_Warning("-- LEAVE_GSEM (%d, %d) --\n", s_nGsemCnt, HLIB_STD_GetSystemTick()); }
#else
#define ENTER_GSEM		VK_SEM_Get (s_ulPLStreamRes_SemId)
#define LEAVE_GSEM		VK_SEM_Release (s_ulPLStreamRes_SemId)
#endif

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/



typedef struct	__plStreamResMsg_t
{
	HINT32				nRequestPid;
	Handle_t			hStreamingId;
	SAPI_NotifyType_e	eNotifyType;
} _plStreamResMsg_t;




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/


STATIC HxList_t				*s_pWillRemoveList;
STATIC _plStreamResMgr_t	s_stStreamResMgr;
STATIC	unsigned long		s_ulPLStreamRes_SemId;
STATIC	unsigned long		s_ulPLStreamRes_ConfirmSemId;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC HOM_Result_e _plstreaming_Resource_CheckConflictWithLive(DxTuneParam_t *pstTuningInfo);
STATIC HCHAR*       _plstreaming_getProcessName(HINT32 nRequestPid);

STATIC HBOOL	_plstreaming_Resource_IsLive (DxRsvStreamType_e eStreamType)
{
	switch(eStreamType)
	{
	case DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO:
	case DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO:
	case DxRSVSTREAM_TYPE_SATIP_LIVE:
	case DxRSVSTREAM_TYPE_SATIP_LIVE_AUTOPID:
	case DxRSVSTREAM_TYPE_NTLS_LIVE_VIDEO:
	case DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO:
		return TRUE;

	default:
		break;
	}

	return FALSE;
}



STATIC _plStreamResInst_t *_plstreaming_Resource_GetEmptyInstance (void)
{
	STATIC Handle_t			s_hLastHandle = 0x00001225;
	Handle_t				hResHandle = (s_hLastHandle < 0xFFFFFFF0) ? s_hLastHandle + 1 : 0x00001225;
	HUINT32					ulIndex = 0;
	_plStreamResInst_t		*parrStreamRes = s_stStreamResMgr.astInstance;

	for (ulIndex = 0; ulIndex < PL_STREAMRES_INST_NUM; ulIndex++)
	{
		if (TRUE != parrStreamRes[ulIndex].bExecuting)
		{
			parrStreamRes[ulIndex].hResHandle = (hResHandle + ulIndex);
			return &(parrStreamRes[ulIndex]);
		}
	}

	return NULL;
}


STATIC _plStreamResInst_t *_plstreaming_Resource_GetInstance (Handle_t hResHandle)
{
	HUINT32					 ulIndex;
	_plStreamResInst_t		*parrStreamRes = s_stStreamResMgr.astInstance;

	for (ulIndex = 0; ulIndex < PL_STREAMRES_INST_NUM; ulIndex++)
	{
		if (TRUE == parrStreamRes[ulIndex].bExecuting)
		{
			if ( hResHandle == parrStreamRes[ulIndex].hResHandle)
			{
				return &(parrStreamRes[ulIndex]);
			}
		}
	}

	return NULL;
}

STATIC _plStreamResInst_t *_plstreaming_Resource_GetInstanceByRingBuf (Handle_t hRingBuf)
{
	HUINT32					 ulIndex;
	_plStreamResInst_t		*parrStreamRes = s_stStreamResMgr.astInstance;

	for (ulIndex = 0; ulIndex < PL_STREAMRES_INST_NUM; ulIndex++)
	{
		if ( hRingBuf == parrStreamRes[ulIndex].hRingbuf)
		{
			return &(parrStreamRes[ulIndex]);
		}
	}

	return NULL;
}

STATIC _plStreamResInst_t *_plstreaming_Resource_GetInstanceBySlot (HUINT32 ulSamaSlot)
{
	HUINT32					 ulIndex;
	_plStreamResInst_t		*parrStreamRes = s_stStreamResMgr.astInstance;

	for (ulIndex = 0; ulIndex < PL_STREAMRES_INST_NUM; ulIndex++)
	{
		if (TRUE == parrStreamRes[ulIndex].bExecuting)
		{
			if ( ulSamaSlot == parrStreamRes[ulIndex].ulSamaSlot)
			{
				return &(parrStreamRes[ulIndex]);
			}
		}
	}

	return NULL;
}





STATIC _plStreamResInst_t *_plstreaming_Resource_NewInstance(void)
{
	_plStreamResInst_t 	*pInst = NULL;

	pInst = _plstreaming_Resource_GetEmptyInstance();
	if(NULL == pInst)
	{
		HxLOG_Error("_plstreaming_Resource_GetEmptyInstance error\r\n");
		return NULL;
	}

	pInst->bExecuting	= TRUE;
	pInst->ulSamaSlot	= SAMASLOT_INITVAL;
	pInst->utMakeTime	= HLIB_STD_GetSystemTime();
	pInst->bUseRingBuf  = TRUE;

	return pInst;
}


STATIC void _plstreaming_Resource_DeleteInstance(_plStreamResInst_t *pInst)
{
	if(NULL == pInst)
	{
		HxLOG_Error("invalid args(pInst is null)\n");
		return;
	}

	HxSTD_memset(pInst, 0, sizeof(_plStreamResInst_t));
	pInst->ulSamaSlot	= SAMASLOT_INITVAL;
}

STATIC int _plstreaming_Resource_GetDuration(DxRsvStreamType_e eStreamType)
{

	switch(eStreamType)
	{
	case DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO:
	case DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO:
		return PL_STREAMING_SCHEDULE_HLS_DURATION;
	case DxRSVSTREAM_TYPE_SATIP_LIVE:
	case DxRSVSTREAM_TYPE_SATIP_LIVE_AUTOPID:
		return PL_STREAMING_SCHEDULE_SATIP_DURATION;
	case DxRSVSTREAM_TYPE_NTLS_LIVE_VIDEO:
	case DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO:
		return PL_STREAMING_SCHEDULE_NTLS_DURATION;
	default:
		return PL_STREAMING_SCHEDULE_DEFAULT_DURATION;
	}

	return PL_STREAMING_SCHEDULE_DEFAULT_DURATION;
}

// 2015/2/1 mhkang PlayPosition : PVR 재생할 때 재생시작 위치. 단위는 초.
STATIC HOM_Result_e _plstreaming_Resource_AddSchedule(PLStreaming_ModeInst_t *pInst, DxSchedule_t *pstSchedule, UNIXTIME  utScheduleStartTime, HUINT32 PlayPosition)
{
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	HBOOL				bIsLive = FALSE;
	HUID 				uUID = 0;
	Handle_t			hRingBuf = HANDLE_NULL;
	DxRsvType_e 		eRsvType = DxRSVTYPE_NETSTREAMING_LIVE;
	HBOOL				bPcrTsPacketInsert = FALSE;
	HINT32				nPIDsCount = 0;
	HUINT16 			*pusPIDs = NULL;
	HCHAR				*pszStreamingKey = NULL;
	HUINT32 			ulSlot = SAMASLOT_INITVAL;

	PLStreaming_SetupInfo_t		*pstSetupInfo;
	PLMediaProfile_Item_t		*pstProfileItem = NULL;

	ENTER_FUNCTION;

	pstSetupInfo 		= &pInst->stSetupInfo;
	pszStreamingKey		= PL_StreamingKey_GetBuffer(pInst->pstStreamingKey);
	pstProfileItem		= pInst->pstMediaProfileItem;
	hRingBuf			= pInst->hRingbuf;
	uUID 				= pstSetupInfo->uUID;
	pusPIDs				= pInst->usPIDs;
	nPIDsCount			= pInst->nPIDsCount;

	bIsLive  = _plstreaming_Resource_IsLive(pstSetupInfo->eStreamType);
	eRsvType = (TRUE == bIsLive) ? DxRSVTYPE_NETSTREAMING_LIVE : DxRSVTYPE_NETSTREAMING_FILE;

#if 0
#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
	/* 2014/4/22 njkim2 :
		Notification Message Box는 conflict여부 상관없이 SAT>IP 재생 시도시 나타나도록 한다.
		그러기 위해, Live와의 conflict 체크도 막아 놓고, User에게 SAT>IP mode로의 진입을 유도한다.
		추후, ntls 등과 같이 추가되는 기능이 있을 시 시나리오(혹은 스펙)에 맞춰 다시 검토가 필요할 것이다.
	*/
#else
	if(TRUE == bIsLive)
	{
        /* 2014/10/21 mhkang:
         eStreamType이 SAT>IP 이라면 Conflict Check는 하지 않는다.
         => Homma가 SAT>IP Streaming 요청을 받았을 때 TP Conflict 유무와 상관없이 STB TV화면에 Notification MessageBox를 띄우도록 한다.
        */
        if ((pstSetupInfo->eStreamType & DxRSVSTREAM_TYPE_SATIP_MASK) == 0)
        {
            eHomResult = _plstreaming_Resource_CheckConflictWithLive(&pstSetupInfo->stTuningParam); /* Supported SAT */
            if(eHOM_Result_Ok != eHomResult)
            {
                HxLOG_Error("Error>  _plstreaming_Resource_CheckConflictWithLive Conflict, eHomResult(%s)\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));
                goto END_LOOP;
            }
        }
	}
#endif
#endif

	if(NULL == pstSetupInfo->pstMediaProfile)
	{
		bPcrTsPacketInsert = FALSE;
	}
	else
	{
		PL_MediaProfile_GetIsInsertPCR(pstSetupInfo->pstMediaProfile, &bPcrTsPacketInsert);
	}


	{ // fill schedule
		HERROR				hErr = ERR_OK;
		HxDATETIME_t		stStartTime;
		DxRSV_Streaming_t	*pstDxStreaming = NULL;
		SAPI_Conflict_t 	stConfilct;


		HLIB_DATETIME_ConvertUnixTimeToDateTime(utScheduleStartTime, &stStartTime);

		pstSchedule->eScheduleSpec		= PL_Streaming_Spec_GetSpec();
		pstSchedule->ulSvcUid			= uUID;
		pstSchedule->eRsvType 			= eRsvType;
		pstSchedule->eRsvReady			= DxRSVREADY_0_SEC;
		pstSchedule->stStartTime		= stStartTime;
		pstSchedule->ulDuration 		= _plstreaming_Resource_GetDuration(pstSetupInfo->eStreamType);
		pstSchedule->eRepeat			= DxRSVREPEAT_ONCE;
		pstSchedule->ulRepeatDays		= 0;
		pstSchedule->eStatus			= DxRSVSTATUS_WAITING;
		pstSchedule->bExceptConflict	= (bIsLive == TRUE) ? FALSE : TRUE;
		pstSchedule->eEpgType			= eDxEPG_TYPE_NONE;

		pstDxStreaming 	= &pstSchedule->uExtInfo.stStrm;
		pstDxStreaming->ulMsgId				= hRingBuf;
		pstDxStreaming->eUpdate				= DxRsvStream_Update_None;

#ifdef SUPPORT_PVR_HLS_SEEK  // 2015/4/6 mhkang: PVR SEEK관련 hlib, obama쪽이 merge되지 않은 상태임.
		pstDxStreaming->ulStartTime         = PlayPosition;
#endif
		pstDxStreaming->eStreamType			= pstSetupInfo->eStreamType;
		pstDxStreaming->bTranscoding 		= (pstProfileItem == NULL) ? FALSE : pstProfileItem->bUseTranscoder;
		pstDxStreaming->bPcrTsPacketInsert	= bPcrTsPacketInsert;
		pstDxStreaming->nPIDsCount			= nPIDsCount;
		HxSTD_MemCopy(pstDxStreaming->usPIDs, pusPIDs, sizeof(HUINT16) * DxRSV_PIDTABLE_COUNT);
		HxSTD_MemCopy(&pstDxStreaming->stTuningParam, &pstSetupInfo->stTuningParam, sizeof(DxTuneParam_t));
		if(TRUE == bIsLive)
		{
			pstDxStreaming->ulSvcUId		= uUID;
			pstDxStreaming->ulContentUId	= 0;
		}
		else
		{
			pstDxStreaming->ulSvcUId		= 0;
			pstDxStreaming->ulContentUId	= uUID;
		}

		HxSTD_StrNCpy(pstDxStreaming->szStreamingKey, pszStreamingKey, (DxRSV_STREAMKEY_LEN-1));
		if(NULL != pstProfileItem)
		{
			HxSTD_StrNCpy(pstDxStreaming->szProfileString, pstProfileItem->pszString, (DxRSV_PROFILESTRING_LEN-1));
		}
		if(NULL != pstSetupInfo->pszDisplayString)
		{
			HxSTD_StrNCpy(pstDxStreaming->szAppDisplayString, pstSetupInfo->pszDisplayString, (DxRSV_APPDISPLAYSTRING_LEN-1));
		}

		HxLOG_Info("Add Schedule Info(Start:%d hRingBuf:0x%x eStreamType:0x%x)\n", utScheduleStartTime, hRingBuf, pstSetupInfo->eStreamType);

		hErr = SAPI_GetAvailableSlot(&ulSlot);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("Critical/Error> SAPI_GetAvailableSlot failed(hErr:%d)\r\n", hErr);
			eHomResult = eHOM_Result_Error_Conflict;
			goto END_LOOP;
		}

		pstSchedule->ulSlot			= ulSlot;

		// too many prints... Activate it when you need this print.
		//PL_Streaming_Debug_PrintScheduleInfo(pstSchedule, __FUNCTION__, __LINE__);

#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S)
		/* 2014/4/1 mhkang:
		Conflict발생하지 않을때만 MakeSchedule하게되면,
		STB Live채널 시청 중에 SAT>IP Client에서 conflict발생하는 채널을 재생시도하면 Notification Message Box가 출력되지 않는 문제가 있다.
		물론 SAT>IP Client에서 conflict발생하지 않는 채널을 재생하면 Notification Message Box가 표시된다.
		일단, Notification Message Box는  conflict여부 상관없이 SAT>IP 재생 시도시 나타나도록 한다.
		*/
#else
// conflict popup 테스트 시 이부분을 막고 테스트
		/* 2014/3/14 mhkang:
		SAPI_MakeSchedule 함수만 호출했을 때, conflict이 발생하는 상황이라면,
		SAPI_MakeSchedule이 실패하는데 이때 STB화면에서 Conflict창이 나타난다.
		STB화면에서 필요없는 Conflict창이 나타나지 않게 하기 위해서
		SAPI_CheckScheduleConflict 함수를 사용해서 Conflict발생할 수 있는지 체크하여 문제가 없는 경우에만 AddSchedule하도록 수정함.
		*/
		hErr = SAPI_CheckScheduleConflict(ulSlot, pstSchedule, &stConfilct);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("Error> SAPI_CheckScheduleConflict failed(hErr:%d)\n", hErr);
			eHomResult = eHOM_Result_Error_Conflict;
			goto END_LOOP;
		}

		if (stConfilct.eType != eSAPI_CONFLICT_TYPE_None)
		{
			HxLOG_Error("Error> Conflict occurred(stConfilct.eType:%d)\n", stConfilct.eType);
			eHomResult = eHOM_Result_Error_Conflict;
			goto END_LOOP;
		}
#endif

		HxLOG_Print(HxANSI_COLOR_PURPLE("stSchedule.uExtInfo.stStrm.eUpdate: 0x%x\n"), pstSchedule->uExtInfo.stStrm.eUpdate);
		HxLOG_Print(HxANSI_COLOR_PURPLE("call SAPI_MakeSchedule : ulSlot(%d), hRingBuf(0x%x)\n"), ulSlot, hRingBuf);
		hErr = SAPI_MakeSchedule(ulSlot, pstSchedule, &stConfilct);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("Error> SAPI_MakeSchedule failed(stConflict.eTyp:%d, hErr:%d)\n", stConfilct.eType, hErr);
			eHomResult = eHOM_Result_Error_Conflict;
			goto END_LOOP;
		}
	}


END_LOOP:

	LEAVE_FUNCTION;

	HxLOG_Info("(-)\n");

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Resource_StopSchedule(HUINT32 ulSlot)
{
	HOM_Result_e eHomResult = eHOM_Result_Ok;
	HERROR		 hErr;

	HxLOG_Info("Slot:%d, Tick:%d(+)\n", ulSlot, HLIB_STD_GetSystemTick());

	if(SAMASLOT_INITVAL != ulSlot)
	{
		hErr = SAPI_StopSchedule(ulSlot);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("SAPI_StopSchedule failed(hErr:%d)\n", hErr);
			eHomResult = eHOM_Result_Error;
			goto END_LOOP;
		}
	}
	else
	{
		HxLOG_Error("[WRN] Slot is SAMASLOT_INITVAL(%08x)\n", SAMASLOT_INITVAL);
	}

END_LOOP:

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Resource_WillCancelSchedule(HUINT32 ulSlot)
{
	HOM_Result_e eHomResult = eHOM_Result_Error;
	HERROR		 hErr;

	HxLOG_Info("Slot:%d, Tick:%d(+)\n", ulSlot, HLIB_STD_GetSystemTick());

	/* 2014/06/06 mhkang:
	SAPI_CancelSchedule 호출이 반환되기 전에 _plstreaming_Resource_SapiNotifier event가 호출되어 _plstreaming_Resource_CancelConfirmSchedule 함수에서 5초 대기하는 경우가 발생함.
	  => ENTER_CONFIRMSEM 위치를 SAPI_CancelSchedule 앞으로 옮김
	*/
	ENTER_CONFIRMSEM;

	if(SAMASLOT_INITVAL != ulSlot)
	{
		hErr = SAPI_CancelSchedule(ulSlot);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("SAPI_CancelSchedule fail(hErr:%d)\n", hErr);
		}
		else
		{
			s_pWillRemoveList = HLIB_LIST_Append(s_pWillRemoveList, (void *)ulSlot);
			HxLOG_Print(HxANSI_COLOR_PURPLE("ulSlot(%d) is appended to s_pWillRemoveList\n"), ulSlot);
			eHomResult = eHOM_Result_Ok;
		}
	}
	else
	{
		HxLOG_Warning("Slot is SAMASLOT_INITVAL(%08x)\n", SAMASLOT_INITVAL);
	}

	LEAVE_CONFIRMSEM;
	LEAVE_FUNCTION;

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Resource_CancelConfirmSchedule(HUINT32 ulSlot)
{
	HINT32	ulCount = 0;
	HxList_t *pList = NULL;
	HOM_Result_e eHomResult = eHOM_Result_Ok;

	HxLOG_Info("Slot:%d, Tick:%d(+)\n", ulSlot, HLIB_STD_GetSystemTick());

	while(ulCount < 1000)
	{
		ENTER_CONFIRMSEM;
		pList = HLIB_LIST_Find(s_pWillRemoveList, (void *)ulSlot);
		if(NULL == pList){
			HxLOG_Print(HxANSI_COLOR_PURPLE("ulSlot(%d) is NULL\n"), ulSlot);
			LEAVE_CONFIRMSEM;
			goto END_LOOP;
		}
		LEAVE_CONFIRMSEM;
		VK_TASK_Sleep(50);
		ulCount++;
	}

	if(ulCount >= 1000)
	{
		eHomResult = eHOM_Result_Error;
		HxLOG_Error("============================================================\r\n");
		HxLOG_Error("not receive sama noti, ulSlot(%d)!!!!\n",ulSlot);
		HxLOG_Error("============================================================\r\n");
	}

END_LOOP:
	LEAVE_FUNCTION;

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Resource_UpdateScheduleTime(const Handle_t hResHandle, HUINT32 ulSlot, UNIXTIME utUpdateDuration)
{
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	HERROR				hErr = ERR_OK;
	DxSchedule_t		stSchedule;
	SAPI_Conflict_t 	stConfilct;

	ENTER_FUNCTION;

	hErr = SAPI_GetSchedule(ulSlot, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetSchedule eHOM_Result_Error\r\n");
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}

	switch(stSchedule.eRsvType)
	{
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_NETSTREAMING_FILE:
		break;

	default:
		HxLOG_Error("is live streaming reservation? slot(%d)\r\n", ulSlot);
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}

	stSchedule.ulDuration = utUpdateDuration;
	stSchedule.uExtInfo.stStrm.eUpdate	= DxRsvStream_Update_Time;

#if defined(CONFIG_DEBUG)
// too many prints... Activate it when you need this print.
#if 0
{
	HCHAR		szDateTime[128];
	UNIXTIME	ulUnixtime = 0;

	HxLOG_Print("\r\n========= Schedule Time update ==========\r\n");
	HxLOG_Print("============================================================================\r\n");
	HxLOG_DecIntDump(stSchedule.ulSlot);
	HxLOG_HexIntDump(stSchedule.eRsvType);
	HxLOG_DecIntDump(stSchedule.stStartTime);

	HLIB_DATETIME_GetStringByDateTime(&(stSchedule.stStartTime), szDateTime, 128); // 2014/4/21 mhkang: datetime 정보 로그 출력
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stSchedule.stStartTime, &ulUnixtime);
	HxLOG_Print("szDateTime: %s (ulUnixtime = %d)\n", szDateTime, ulUnixtime);

	HxLOG_DecIntDump(stSchedule.ulDuration);
	{
		DxRSV_Streaming_t	*pstDxStreaming = NULL;

		pstDxStreaming	= &stSchedule.uExtInfo.stStrm;
		HxLOG_HexIntDump(pstDxStreaming->ulMsgId);
		HxLOG_StringDump(pstDxStreaming->szStreamingKey);
	}
	HxLOG_Print("============================================================================\r\n");
}
#endif
#endif

#if 1
	HxLOG_Print(HxANSI_COLOR_PURPLE("call SAPI_UpdateSchedule : ulSlot(%d)\n"), ulSlot);
	hErr = SAPI_UpdateSchedule(ulSlot, &stSchedule, &stConfilct, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_UpdateSchedule eHOM_Result_Error, ulSlot = %d\r\n", ulSlot);
			eHomResult = eHOM_Result_Error;
			goto END_LOOP;
	}
#else
	HxSTD_memset(&stConfilct, 0, sizeof(SAPI_Conflict_t));

	hErr = SAPI_UpdateSchedule(ulSlot, &stSchedule, &stConfilct, FALSE, TRUE);
	if (ERR_OK != hErr)
	{
		HINT32					i;
		SAPI_ConflictItem_t		*pstConflictItem = NULL;
		DxSchedule_t			stConflictSch;
		UNIXTIME				utConflictUnixtime = 0;
		_plStreamResInst_t		*pInst = NULL;

		HxLOG_Print(HxANSI_COLOR_PURPLE("SAPI_UpdateSchedule Fail!!\n"));

		pInst 	= _plstreaming_Resource_GetInstance(hResHandle);
		if(NULL != pInst)
		{
			HxLOG_Print("[CONFLICT] stConfilct.ulNumConflict = %d\n", stConfilct.ulNumConflict);

			if ((0 < stConfilct.ulNumConflict) && (NULL != stConfilct.pstConflictArray))
			{

				for (i = 0; i < stConfilct.ulNumConflict; i++)
				{
					pstConflictItem = &(stConfilct.pstConflictArray[i]);
					if (NULL == pstConflictItem)
						continue;

					hErr = SAPI_GetSchedule(pstConflictItem->ulSlot, &stConflictSch);
					if (ERR_OK != hErr)
						continue;

					hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime(&stConflictSch.stStartTime, &utConflictUnixtime);
					if (ERR_OK == hErr)
					{
						HxLOG_Print("[CONFLICT] ulSlot = %d, pInst->utEndTime = %d, utConflictUnixtime = %d\n", pstConflictItem->ulSlot, pInst->utEndTime, utConflictUnixtime);
						if(pInst->utEndTime >= utConflictUnixtime)
						{
							/* loop 돌면서 가장 앞선 conflict time으로 설정 될 것임 */
							pInst->utEndTime = utConflictUnixtime - 1;
							stSchedule.ulDuration = pInst->utEndTime - pInst->utStartTime;
							HxLOG_Print("[CONFLICT] pInst->utEndTime = %d, utUpdateDuration = %d\n", pInst->utEndTime, stSchedule.ulDuration);
						}
					}
				}

				hErr = SAPI_UpdateSchedule(ulSlot, &stSchedule, &stConfilct, FALSE, FALSE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SAPI_UpdateSchedule eHOM_Result_Error\r\n");
					eHomResult = eHOM_Result_Error;
					goto END_LOOP;
				}
			}
		}
	}
	else
	{
		HxLOG_Print(HxANSI_COLOR_CYAN("SAPI_UpdateSchedule OK!!\n"));
	}

#endif

END_LOOP:

	LEAVE_FUNCTION;

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Resource_UpdateSchedulePids(DxRsvStreamType_e eStreamType, HUINT32 ulSlot, UNIXTIME utUpdateEndTime, HINT32 nPIDsCount, HUINT16 usPIDs[], Handle_t hMsgId)
{
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	HERROR				hErr = ERR_OK;
	DxSchedule_t		stSchedule;
	SAPI_Conflict_t 	stConfilct;

	ENTER_FUNCTION;

	hErr = SAPI_GetSchedule(ulSlot, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetSchedule eHOM_Result_Error\r\n");
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}

	switch(stSchedule.eRsvType)
	{
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_NETSTREAMING_FILE:
		break;

	default:
		HxLOG_Error("is live streaming reservation? slot(%d)\r\n", ulSlot);
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}

	stSchedule.uExtInfo.stStrm.eUpdate = 0;
	if(0 < utUpdateEndTime)
	{
		stSchedule.ulDuration = utUpdateEndTime;
		stSchedule.uExtInfo.stStrm.eUpdate	|= DxRsvStream_Update_Time;
	}
	stSchedule.uExtInfo.stStrm.ulMsgId		= (HUINT32)hMsgId;
	stSchedule.uExtInfo.stStrm.nPIDsCount 	= nPIDsCount;
	stSchedule.uExtInfo.stStrm.eUpdate		|= DxRsvStream_Update_Pids;

	// 2015/3/26 mhkang: if pids=all , update eStreamType as valid eStreamType(DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID).
	stSchedule.uExtInfo.stStrm.eStreamType = eStreamType;
	HxLOG_Info("eStreamType: %d\n", eStreamType);
	HxLOG_Print(HxANSI_COLOR_PURPLE("stSchedule.uExtInfo.stStrm.eUpdate: 0x%x\n"), stSchedule.uExtInfo.stStrm.eUpdate);

	HxSTD_MemCopy(stSchedule.uExtInfo.stStrm.usPIDs, usPIDs, sizeof(HUINT16) * DxRSV_PIDTABLE_COUNT);

#if defined(CONFIG_DEBUG)
// too many prints... Activate it when you need this print.
#if 0
	HxLOG_Print("\r\n\r\n========= Schedule pid update ==========\r\n");
	HxLOG_DecIntDump(stSchedule.ulSlot);
	HxLOG_HexIntDump(stSchedule.eRsvType);
	HxLOG_DecIntDump(stSchedule.stStartTime);
	HxLOG_DecIntDump(stSchedule.ulDuration);

	HxLOG_Print("pstDxStreaming = &stSchedule.uExtInfo.stStrm");
	{
		DxRSV_Streaming_t	*pstDxStreaming = NULL;
		pstDxStreaming	= &stSchedule.uExtInfo.stStrm;
		HxLOG_HexIntDump(pstDxStreaming->ulMsgId);
		HxLOG_DecIntDump(pstDxStreaming->nPIDsCount);
		PL_Streaming_Debug_PrintPids(pstDxStreaming->nPIDsCount, pstDxStreaming->usPIDs, __FUNCTION__, __LINE__);
		HxLOG_StringDump(pstDxStreaming->szStreamingKey);
	}
	HxLOG_Print("============================================================================\r\n");
#endif
#endif

	HxLOG_Print(HxANSI_COLOR_PURPLE("call SAPI_UpdateSchedule : ulSlot(%d)\n"), ulSlot);
	hErr = SAPI_UpdateSchedule(ulSlot, &stSchedule, &stConfilct, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_UpdateSchedule eHOM_Result_Error\r\n");
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}


END_LOOP:

	LEAVE_FUNCTION;

	return eHomResult;
}


STATIC HOM_Result_e _plstreaming_Resource_UpdateSchedulePosition(HUINT32 ulSlot, UNIXTIME utUpdateEndTime, HUINT32 ulPosition, Handle_t hMsgId)
{
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	HERROR				hErr = ERR_OK;
	DxSchedule_t		stSchedule;
	SAPI_Conflict_t 	stConfilct;

	ENTER_FUNCTION;

	hErr = SAPI_GetSchedule(ulSlot, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetSchedule eHOM_Result_Error\r\n");
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}

	switch(stSchedule.eRsvType)
	{
	// file 에 대해서만 position 변경 가능
	case DxRSVTYPE_NETSTREAMING_FILE:
		break;

	default:
		HxLOG_Error("is live streaming reservation? slot(%d)\r\n", ulSlot);
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}

	stSchedule.uExtInfo.stStrm.eUpdate = 0;
	if(0 < utUpdateEndTime)
	{
		stSchedule.ulDuration = utUpdateEndTime;
		stSchedule.uExtInfo.stStrm.eUpdate	|= DxRsvStream_Update_Time;
	}
	stSchedule.uExtInfo.stStrm.ulMsgId			= (HUINT32)hMsgId;
	stSchedule.uExtInfo.stStrm.ulPlayerPosition	= ulPosition;
	stSchedule.uExtInfo.stStrm.eUpdate			|= DxRsvStream_Update_PlayPosition;

#if defined(CONFIG_DEBUG)
// too many prints... Activate it when you need this print.
#if 0
	HxLOG_Print("\r\n\r\n========= Schedule pid update ==========\r\n");
	HxLOG_DecIntDump(stSchedule.ulSlot);
	HxLOG_HexIntDump(stSchedule.eRsvType);
	HxLOG_DecIntDump(stSchedule.stStartTime);
	HxLOG_DecIntDump(stSchedule.ulDuration);

	HxLOG_Print("pstDxStreaming = &stSchedule.uExtInfo.stStrm");
	{
		DxRSV_Streaming_t	*pstDxStreaming = NULL;
		pstDxStreaming	= &stSchedule.uExtInfo.stStrm;
		HxLOG_HexIntDump(pstDxStreaming->ulMsgId);
		HxLOG_HexIntDump(pstDxStreaming->ulPlayerPosition);
		HxLOG_StringDump(pstDxStreaming->szStreamingKey);
	}
	HxLOG_Print("============================================================================\r\n");
#endif
#endif

	HxLOG_Print(HxANSI_COLOR_PURPLE("call SAPI_UpdateSchedule : ulSlot(%d)\n"), ulSlot);
	hErr = SAPI_UpdateSchedule(ulSlot, &stSchedule, &stConfilct, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_UpdateSchedule eHOM_Result_Error\r\n");
		eHomResult = eHOM_Result_Error;
		goto END_LOOP;
	}


END_LOOP:

	LEAVE_FUNCTION;

	return eHomResult;
}

STATIC HERROR _plstreaming_Resource_SapiNotifier (void *pvUserData, SAPI_NotiInfo_t *punNotify)
{
	DxSchedule_t			*pstSchedule = NULL;
	HERROR					hErr = ERR_OK;
	HINT32					ulSlot = SAMASLOT_INITVAL;
	HINT32					nRequestPid = 0;
	_plStreamResInst_t		*pInst = NULL;
	plStreamingInst_t		*pOwnerInst= NULL;

	if (punNotify == NULL)
	{
		HxLOG_Error( "[ERR] Invalid arg(punNotify is null) \n" );
		return ERR_FAIL;
	}

	switch (punNotify->eNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_SchedulesLoaded:
		if(NULL != punNotify->stSchLoaded.pstSchArray)
		{
			ulSlot = punNotify->stSchLoaded.pstSchArray->ulSlot;
		}
		pstSchedule = punNotify->stSchLoaded.pstSchArray;
		nRequestPid = punNotify->stSchLoaded.nRequestPid;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleAdded:
		ulSlot = punNotify->stSchAdded.ulSlotId;
		pstSchedule = punNotify->stSchAdded.pstSchedule;
		nRequestPid = punNotify->stSchAdded.nRequestPid;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleChanged:
		ulSlot = punNotify->stSchChanged.ulSlotId;
		pstSchedule = punNotify->stSchChanged.pstSchedule;
		nRequestPid = punNotify->stSchChanged.nRequestPid;

#ifdef CONFIG_DEBUG
		SAPI_DbgPrintSchedule(pstSchedule); // 2014/4/21 mhkang: add SAMA debugging log.
#endif
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
		ulSlot = punNotify->stSchRemoved.ulSlotId;
		pstSchedule = punNotify->stSchRemoved.pstSchedule;
		nRequestPid = punNotify->stSchRemoved.nRequestPid;
		break;

	case eSAPI_NOTIFY_TYPE_TimeUp:
		ulSlot = punNotify->stTimeUp.ulSlotId;
		pstSchedule = punNotify->stTimeUp.pstSchedule;
		nRequestPid = 0;
		break;

	case eSAPI_NOTIFY_TYPE_ConflictScheduleAdded:
		ulSlot		= punNotify->stConfSchAdded.stConflicted.stTriedSchedule.ulSlot;
		nRequestPid = 0;
		break;

	default:
		break;
	}

	if(SAMASLOT_INITVAL == ulSlot)
	{
		HxLOG_Warning("ulSlot(%d) punNotify->eNotifyType(%d) from(%s) \r\n",
				ulSlot, punNotify->eNotifyType,
				((0 == nRequestPid) ? "unknown" : HLIB_STD_GetProcessName(nRequestPid)));
		return ERR_OK;
	}


	switch(punNotify->eNotifyType)
	{
		case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
			ENTER_CONFIRMSEM;
			s_pWillRemoveList = HLIB_LIST_Remove(s_pWillRemoveList, (void *)ulSlot);
			HxLOG_Print(HxANSI_COLOR_PURPLE("ulSlot(%d) is removed from s_pWillRemoveList\n"), ulSlot);
			LEAVE_CONFIRMSEM;
			break;

		default:
			break;
	}


	ENTER_GSEM;
	pInst = _plstreaming_Resource_GetInstanceBySlot(ulSlot);
	if(NULL != pInst)
	{
		_plStreamResMgr_t	*pstMgr = &s_stStreamResMgr;
		_plStreamResMsg_t	stResMsg;
		HCHAR				*pszProcessName = ((0 == nRequestPid) ? "unknown" : _plstreaming_getProcessName(nRequestPid));

		hErr = ERR_OK;
		switch(punNotify->eNotifyType)
		{
		case eSAPI_NOTIFY_TYPE_SchedulesLoaded:
			HxLOG_Debug("eSAPI_NOTIFY_TYPE_SchedulesLoaded(slot:%d), Process(%s) Tick(%d)\n",ulSlot , pszProcessName, HLIB_STD_GetSystemTick());
			pInst->eSetupStatus = ePLStreaming_SetupStatus_Loaded;
			break;

		case eSAPI_NOTIFY_TYPE_ScheduleAdded:
			HxLOG_Info("eSAPI_NOTIFY_TYPE_ScheduleAdded(slot:%d), Process(%s) Tick(%d)\n",ulSlot , pszProcessName, HLIB_STD_GetSystemTick());
			pInst->eSetupStatus = ePLStreaming_SetupStatus_Running;
			break;

		case eSAPI_NOTIFY_TYPE_ScheduleChanged:
			HxLOG_Debug("eSAPI_NOTIFY_TYPE_ScheduleChanged(slot:%d), Process(%s) Tick(%d)\n",ulSlot , pszProcessName, HLIB_STD_GetSystemTick());
			pOwnerInst = (plStreamingInst_t*)PL_Streaming_Context_getInstance(pInst->hStreamingId);

			/* 2015/5/12 mhkang: Semaphore lock 이 필요한가?
				eSAPI_NOTIFY_TYPE_ScheduleRemoved event가 발생할 때까지 pOwnerInst->pstModeInst 메모리가 삭제될 일이 없다.
				고로 Lock없이 사용가능할 것으로 보임.
			*/
			if (pOwnerInst != NULL)
			{
				if (pOwnerInst->pstModeInst != NULL)
				{
					pOwnerInst->pstModeInst->tunerid = pstSchedule->uExtInfo.stStrm.stTuningParam.sat.ulTunerGroupId;
				}
			}
			else
			{
				// pOwnerInst 가 왜 없지... SAT>IP Streaming하게 되면  반드시 있어야 하는데, pInst->hStreamingId 값이 셋팅이 안되어 있나?
				// Tunerid 를 갱신 못하는데..
				HxLOG_Error("pOwnerInst was NULL \n");
			}

			break;

		case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
			HxLOG_Info("eSAPI_NOTIFY_TYPE_ScheduleRemoved(slot:%d), Process(%s) Tick(%d)\n",ulSlot , pszProcessName, HLIB_STD_GetSystemTick());
			pInst->eSetupStatus = ePLStreaming_SetupStatus_Conflict;
			break;

		case eSAPI_NOTIFY_TYPE_TimeUp:
			HxLOG_Debug("eSAPI_NOTIFY_TYPE_TimeUp(slot:%d), Process(%s) Tick(%d)\n",ulSlot , pszProcessName, HLIB_STD_GetSystemTick());
			break;

		case eSAPI_NOTIFY_TYPE_ConflictScheduleAdded:
			HxLOG_Info("eSAPI_NOTIFY_TYPE_ConflictScheduleAdded(slot:%d), Process(%s) Tick(%d)\n",ulSlot , pszProcessName, HLIB_STD_GetSystemTick());
			pInst->eSetupStatus = ePLStreaming_SetupStatus_Conflict;
			break;

		default:
			HxLOG_Debug("default(slot:%d) punNotify->eNotifyType(%d) Process(%s) Tick(%d)\n",ulSlot , punNotify->eNotifyType, pszProcessName, HLIB_STD_GetSystemTick());
			break;
		}

		HxSTD_memset(&stResMsg, 0, sizeof(_plStreamResMsg_t));

		stResMsg.eNotifyType	= punNotify->eNotifyType;
		stResMsg.hStreamingId	= pInst->hStreamingId;
		stResMsg.nRequestPid	= nRequestPid;
		hErr = VK_MSG_SendTimeout(pstMgr->ulMsgId, &stResMsg, sizeof(_plStreamResMsg_t), 10); // _plstreaming_Resource_Task => PL_Streaming_Context_ResConflict 함수 호출됨.
		if(ERR_OK != hErr)
		{
			HxLOG_Error("VK_MSG_SendTimeout error!!(hErr=%d)\n", hErr);
		}
	}
	else
	{
		HxLOG_Debug("Resource not found, ulSlot(%d) \n", ulSlot);
		hErr = ERR_OK;
	}
	LEAVE_GSEM;

	return hErr;
}

/**
  @brief LXC를 사용하는 경우, 다른 Container의 Process name을 알 수 없다.
		따라서 SAPI_UniqueProcessIndicator_e에 정의된 값으로 Process를 구분하도록 한다.
  @param LXC를 사용하지 않으면 process pid값이 오고 LXC를 사용하면 SAPI_UniqueProcessIndicator_e 정의된 값이 넘어온다.

  @return Process name
*/
STATIC HCHAR* _plstreaming_getProcessName(HINT32 nRequestPid)
{
#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
	static char szProcessName[128];

	switch(nRequestPid)
	{
	case eSAPI_UNIQUE_PROCESS_Indicator_None:
		snprintf(szProcessName,128,"%s(%d)","None", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Sama:
		snprintf(szProcessName,128,"%s(%d)","Sama", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Obama:
		snprintf(szProcessName,128,"%s(%d)","Obama", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Homma:
		snprintf(szProcessName,128,"%s(%d)","Homma", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Corsair:
		snprintf(szProcessName,128,"%s(%d)","Corsair", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Ipepg:
		snprintf(szProcessName,128,"%s(%d)","Ipepg", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Unknown:
	default:
		snprintf(szProcessName,128,"%s(%d)","Unknown", nRequestPid);
		break;
	}
	return szProcessName;
#else
	return HLIB_STD_GetProcessName(nRequestPid);
#endif
}

/* check recording conflict reservations... */
STATIC void _plstreaming_Resource_AdjustDuration(HUINT32 ulSlot, _plStreamResInst_t *pInst, UNIXTIME *putUpdateDuration)
{
	HERROR					hErr = ERR_FAIL;
	HINT32					i;
	HUINT32					ulTempSlot = 0xffffffff;
	SAPI_Conflict_t			stSapiConf;
	SAPI_ConflictItem_t		*pstConflictItem = NULL;
	DxSchedule_t			stSchedule, stConflictSch;
	UNIXTIME				utConflictUnixtime = 0;

	ENTER_FUNCTION;

	if ((NULL == putUpdateDuration) || (NULL == pInst))
	{
		HxLOG_Error("putUpdateDuration = 0x%x, pInst = 0x%x\r\n", putUpdateDuration, pInst);
		LEAVE_FUNCTION;
		return;
	}

	hErr = SAPI_GetSchedule(ulSlot, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetSchedule eHOM_Result_Error\r\n");
		LEAVE_FUNCTION;
		return;
	}

	hErr = SAPI_GetAvailableSlot(&ulTempSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Can't SAPI_GetAvailableSlot\r\n");
		LEAVE_FUNCTION;
		return;
	}

	HxLOG_Print("ulSlot = %d, ulTempSlot = %d\n", ulSlot, ulTempSlot);

	stSchedule.ulSlot = ulTempSlot;
	hErr = SAPI_CheckScheduleConflict(ulTempSlot, &stSchedule, &stSapiConf);
	if (ERR_OK == hErr)
	{
		if ((0 < stSapiConf.ulNumConflict) && (NULL != stSapiConf.pstConflictArray))
		{
			HxLOG_Print("[CONFLICT] stSapiConf.ulNumConflict = %d\n", stSapiConf.ulNumConflict);

			for (i = 0; i < stSapiConf.ulNumConflict; i++)
			{
				pstConflictItem = &(stSapiConf.pstConflictArray[i]);
				if (NULL == pstConflictItem)
					continue;

				hErr = SAPI_GetSchedule(pstConflictItem->ulSlot, &stConflictSch);
				if (ERR_OK != hErr)
					continue;

				hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime(&stConflictSch.stStartTime, &utConflictUnixtime);
				if (ERR_OK == hErr)
				{
					HxLOG_Print("[CONFLICT] ulSlot = %d, pInst->utEndTime = %d, utConflictUnixtime = %d\n", pstConflictItem->ulSlot, pInst->utEndTime, utConflictUnixtime);
					if(pInst->utEndTime >= utConflictUnixtime)
					{
						/* loop 돌면서 가장 앞선 conflict time으로 설정 될 것임 */
						pInst->utEndTime = utConflictUnixtime - 1;
						*putUpdateDuration = pInst->utEndTime - pInst->utStartTime;
						HxLOG_Print("[CONFLICT] pInst->utEndTime = %d, utUpdateDuration = %d\n", pInst->utEndTime, *putUpdateDuration);
					}
				}
			}
		}
	}
	else
	{
		HxLOG_Print("[CONFLICT] hErr = %d, ulSlot = %d\n", hErr, ulSlot);
	}

	LEAVE_FUNCTION;
}


#define __SAT_CONFLICT__

#define PL_CH_UNIVERSAL_LNB_FREQ		0xFFFFFFFF
#define PL_CH_MDU1_LNB_FREQ				(PL_CH_UNIVERSAL_LNB_FREQ - 1)			// for turkish LNB types MDU1 using in digitiruk OP
#define PL_CH_MDU2_LNB_FREQ				(PL_CH_UNIVERSAL_LNB_FREQ - 2)			// for turkish LNB types MDU2 using in digitiruk OP
#define PL_CH_MDU3_LNB_FREQ				(PL_CH_UNIVERSAL_LNB_FREQ - 3)			// for turkish LNB types MDU3 using in digitiruk OP
#define PL_CH_MDU4_LNB_FREQ				(PL_CH_UNIVERSAL_LNB_FREQ - 4)			// for turkish LNB types MDU4 using in digitiruk OP
#define PL_CH_MDU_USER_DEFINE			(PL_CH_UNIVERSAL_LNB_FREQ - 5)			// for turkish LNB types User define using in digitiruk OP


STATIC HUINT32 _plstreaming_Resource_CH_LNB_GetActualLnbFreq(HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return 9750;
	}
	else
	{
		return 10600;
	}
}


STATIC HBOOL _plstreaming_Resource_CH_LNB_GetActual22KTone(HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

STATIC HUINT32 _plstreaming_Resource_CH_LNB_GetActualMduLnbFreq(HUINT32 ulLnbFreq, DxSat_Polarization_e ePol, HUINT32 ulFreq)
{
HUINT32		ulActLnbFreq = 0;

	switch(ulLnbFreq)
	{
		case PL_CH_MDU1_LNB_FREQ:
			if(ePol == eDxSAT_POLAR_HOR)
				ulActLnbFreq = 10450;
			else
				ulActLnbFreq = 10000;
			break;

		case PL_CH_MDU2_LNB_FREQ:
			if (ulFreq >= 10780 && ulFreq <= 10950)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 9830;
				else
					ulActLnbFreq = 9550;
			}
			else if (ulFreq >= 11450 && ulFreq <= 11700)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 9925;
				else
					ulActLnbFreq = 9550;
			}
			else
				ulActLnbFreq = 0;
			break;

		case PL_CH_MDU3_LNB_FREQ:
			if(ulFreq >= 10780 && ulFreq <= 10950)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 12930;
				else
					ulActLnbFreq = 9830;
			}
			else if(ulFreq >= 11450 && ulFreq <= 11700)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 9830;
				else
					ulActLnbFreq = 12930;
			}
			else
				ulActLnbFreq = 0;

			break;

		case PL_CH_MDU4_LNB_FREQ:
			if(ulFreq >= 10780 && ulFreq <= 10950)
			{
				if(ePol == eDxSAT_POLAR_HOR)
					ulActLnbFreq = 12930;
				else
					ulActLnbFreq = 12650;
			}
			else if(ulFreq >= 11450 && ulFreq <= 11700 && ePol == eDxSAT_POLAR_HOR)
				ulActLnbFreq = 13035;
			else if(ulFreq >= 11429 && ulFreq <= 11700 && ePol == eDxSAT_POLAR_VER)
				ulActLnbFreq = 12650;
			else
				ulActLnbFreq = 0;

			break;

		default:
			HxLOG_Error("[_plstreaming_Resource_CH_LNB_GetActualMduLnbFreq] unsupported MDU Freq (%d) \n", ulLnbFreq);
			ulActLnbFreq = 0;
			break;

	}

	if(ulActLnbFreq == 0)
		HxLOG_Error("ERROR : out of range freq : %d \n", ulFreq);
	else
		HxLOG_Info("LnbFreq for cur MDU -> %d \n", ulActLnbFreq);


	return ulActLnbFreq;

}

STATIC HOM_Result_e _plstreaming_Resource_CH_CheckLnbOnlyLoopThrough(DxSatTuningInfo_t *pstTuningInfo1, DxSatTuningInfo_t *pstTuningInfo2)
{
	HUINT32		ulLnbFreq1, ulLnbFreq2;


	if(pstTuningInfo1->antInfo.info.lnb.ulLnbFreq == PL_CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq1 = _plstreaming_Resource_CH_LNB_GetActualLnbFreq(pstTuningInfo1->tuningInfo.ulFrequency);
	}
	else
	{
		ulLnbFreq1 = pstTuningInfo1->antInfo.info.lnb.ulLnbFreq;
	}

	if(pstTuningInfo2->antInfo.info.lnb.ulLnbFreq == PL_CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq2 = _plstreaming_Resource_CH_LNB_GetActualLnbFreq(pstTuningInfo2->tuningInfo.ulFrequency);
	}
	else
	{
		ulLnbFreq2 = pstTuningInfo2->antInfo.info.lnb.ulLnbFreq;
	}

	if(	(ulLnbFreq1 == ulLnbFreq2) && /* LNB frequency 같고... */
		(pstTuningInfo1->tuningInfo.ePolarization == pstTuningInfo2->tuningInfo.ePolarization) /* polar가 같으면 diseqc version별로 더 체크해 보자. */
	)
	{
		return eHOM_Result_Ok;
	}
	else
	{
		HxLOG_Print("\t<LNB Only> Loop through conflict ! ulLnbFreq[%d/%d], polar[%d/%d]\n",
					ulLnbFreq1, ulLnbFreq2, pstTuningInfo1->tuningInfo.ePolarization, pstTuningInfo2->tuningInfo.ePolarization);

		return eHOM_Result_Error_Conflict;
	}
}


STATIC HOM_Result_e _plstreaming_Resource_CH_CheckDiseqcLoopThrough(DxSatTuningInfo_t *pstTuningInfo1, DxSatTuningInfo_t *pstTuningInfo2)
{
	HUINT32		ulLnbFreq1, ulLnbFreq2;
	HBOOL		b22kTone1, b22kTone2;

	if(pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq == PL_CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq1 = _plstreaming_Resource_CH_LNB_GetActualLnbFreq(pstTuningInfo1->tuningInfo.ulFrequency);
		b22kTone1 = _plstreaming_Resource_CH_LNB_GetActual22KTone(pstTuningInfo1->tuningInfo.ulFrequency);
	}
	else if(pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq >= (HUINT32)PL_CH_MDU4_LNB_FREQ && pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq <= (HUINT32)PL_CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq1 = _plstreaming_Resource_CH_LNB_GetActualMduLnbFreq(pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq, pstTuningInfo1->tuningInfo.ePolarization, pstTuningInfo1->tuningInfo.ulFrequency);
		b22kTone1 = pstTuningInfo1->antInfo.info.diseqc.b22kTone;
	}
	else
	{
		ulLnbFreq1 = pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq;
		b22kTone1 = pstTuningInfo1->antInfo.info.diseqc.b22kTone;
	}

	if(pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq == PL_CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq2 = _plstreaming_Resource_CH_LNB_GetActualLnbFreq(pstTuningInfo2->tuningInfo.ulFrequency);
		b22kTone2 = _plstreaming_Resource_CH_LNB_GetActual22KTone(pstTuningInfo2->tuningInfo.ulFrequency);
	}
	else if(pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq >= (HUINT32)PL_CH_MDU4_LNB_FREQ && pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq <= (HUINT32)PL_CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq2 = _plstreaming_Resource_CH_LNB_GetActualMduLnbFreq(pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq, pstTuningInfo2->tuningInfo.ePolarization, pstTuningInfo2->tuningInfo.ulFrequency);
		b22kTone2 = pstTuningInfo2->antInfo.info.diseqc.b22kTone;
	}
	else
	{
		ulLnbFreq2 = pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq;
		b22kTone2 = pstTuningInfo2->antInfo.info.diseqc.b22kTone;
	}

	if( (pstTuningInfo1->antInfo.info.diseqc.diseqcVersion == pstTuningInfo2->antInfo.info.diseqc.diseqcVersion) && /* Diseqc version이 같고... */
		(ulLnbFreq1 == ulLnbFreq2) && /* LNB frequency 같고... */
		(b22kTone1 == b22kTone2) && /* 22kTone 같고... */
		(pstTuningInfo1->tuningInfo.ePolarization == pstTuningInfo2->tuningInfo.ePolarization) /* polar가 같으면 diseqc version별로 더 체크해 보자. */
	)
	{
		switch(pstTuningInfo1->antInfo.info.diseqc.diseqcVersion)
		{
			case eDxDISEQC_VER_1_0 :
			case eDxDISEQC_VER_2_0 :
				if(pstTuningInfo1->antInfo.info.diseqc.diseqcInput != pstTuningInfo2->antInfo.info.diseqc.diseqcInput)
				{
					HxLOG_Print("\t<DiSEqC> Loop through conflict ! Diseqc input[%d/%d]\n",
								pstTuningInfo1->antInfo.info.diseqc.diseqcInput, pstTuningInfo2->antInfo.info.diseqc.diseqcInput);
					return eHOM_Result_Error_Conflict;
				}
				break;

			case eDxDISEQC_VER_1_2 :
				if(pstTuningInfo1->antInfo.info.diseqc.ucMotorPosition != pstTuningInfo2->antInfo.info.diseqc.ucMotorPosition)
				{
					HxLOG_Print("\t<DiSEqC> Loop through conflict ! Motor position[%d/%d]\n",
								pstTuningInfo1->antInfo.info.diseqc.ucMotorPosition, pstTuningInfo2->antInfo.info.diseqc.ucMotorPosition);
					return eHOM_Result_Error_Conflict;
				}
				break;

			case eDxDISEQC_VER_1_3 :
				if(pstTuningInfo1->antInfo.info.diseqc.sSatLonggitude != pstTuningInfo2->antInfo.info.diseqc.sSatLonggitude)
				{
					HxLOG_Print("\t<DiSEqC> Loop through conflict ! Longgitude[%d/%d]\n",
								pstTuningInfo1->antInfo.info.diseqc.sSatLonggitude, pstTuningInfo2->antInfo.info.diseqc.sSatLonggitude);
					return eHOM_Result_Error_Conflict;
				}
				break;

			default :
				HxLOG_Critical("\n\n");
				return eHOM_Result_Error_Conflict;
		}
	}
	else
	{ /* Diseqc version 더 볼 필요없이 FAIL */
		HxLOG_Print("\t<Diseqc> Loop through conflict ! Diseqc ver[%d/%d], ulLnbFreq[%d/%d], 22k[%d/%d], polar[%d/%d]\n",
					pstTuningInfo1->antInfo.info.diseqc.diseqcVersion, pstTuningInfo2->antInfo.info.diseqc.diseqcVersion,
					ulLnbFreq1, ulLnbFreq2,
					b22kTone1, b22kTone2,
					pstTuningInfo1->tuningInfo.ePolarization, pstTuningInfo2->tuningInfo.ePolarization);

		return eHOM_Result_Error_Conflict;
	}

	return eHOM_Result_Ok;
}


STATIC HOM_Result_e _plstreaming_Resource_CheckConflictWithLive(DxTuneParam_t *pstTuningInfo)
{
	CO_CHANNEL_t			*pCurCoChannel = NULL;
	HERROR					hErr = eHOM_Result_Ok;
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;
	DxAntennaConnection_e 	eConnectionType = eDxANTCONN_MAX;

	hErr = PL_COMMON_GetCurrentServiceInfo(&pCurCoChannel, NULL);
	if( eHOM_Result_Ok != hErr || NULL == pCurCoChannel || NULL == pCurCoChannel->ts)
	{
		HxLOG_Print("live unlocked!!\r\n");
		eHomResult = eHOM_Result_Ok;
		goto FUNC_END;
	}

	// 2014/10/17 mhkang: 위성이 아닌경우 conflict체크가 불필요하다.
	if (eDxDELIVERY_TYPE_SAT != pCurCoChannel->ts->tuningParam.eDeliType)
	{
		if (eDxDELIVERY_TYPE_TER == pCurCoChannel->ts->tuningParam.eDeliType || eDxDELIVERY_TYPE_CAB == pCurCoChannel->ts->tuningParam.eDeliType)
		{
			HxLOG_Debug("No need to check conflict of tuner(eDeliType:0x%x)\n", pCurCoChannel->ts->tuningParam.eDeliType);
			eHomResult = eHOM_Result_Ok;
		}
		else // unknown eDeliType
		{
			HxLOG_Error("Unsupported tuner type(0x%x)\n", pCurCoChannel->ts->tuningParam.eDeliType);
			eHomResult = eHOM_Result_Error_InvalidValue;
		}
		goto FUNC_END;
	}

	if (eDxDELIVERY_TYPE_SAT == pCurCoChannel->ts->tuningParam.eDeliType)
	{
		hErr = PL_COMMON_GetAntennaConnection(&eConnectionType);
		if(eHOM_Result_Ok != hErr)
		{
			HxLOG_Print("GetAntennaConnection error!\r\n");
			eHomResult = eHOM_Result_Ok;
			goto FUNC_END;
		}

		if(eDxANTCONN_ONECABLE != eConnectionType)
		{
			HxLOG_Print("eDxANTCONN_ONECABLE != eConnectionType\r\n");
			eHomResult = eHOM_Result_Ok;
			goto FUNC_END;
		}

		{
			DxTuneParam_t 		*pstTuningInfo1 = NULL;
			DxTuneParam_t 		*pstTuningInfo2 = NULL;

			pstTuningInfo1 = &pCurCoChannel->ts->tuningParam;
			pstTuningInfo2 = pstTuningInfo;

			if( pstTuningInfo1->sat.antennaType == eDxANT_TYPE_LNB_ONLY )
			{
				eHomResult = _plstreaming_Resource_CH_CheckLnbOnlyLoopThrough(&(pstTuningInfo1->sat), &(pstTuningInfo2->sat));
				HxLOG_Print("eDxANT_TYPE_LNB_ONLY eHomResult(%s)\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));
			}
			else if ( pstTuningInfo1->sat.antennaType == eDxANT_TYPE_DISEQC )
			{
				eHomResult = _plstreaming_Resource_CH_CheckDiseqcLoopThrough(&(pstTuningInfo1->sat), &(pstTuningInfo2->sat));
				HxLOG_Print("eDxANT_TYPE_DISEQC eHomResult(%s)\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));
			}
			else
			{
				/* SCD, SMATV 는 항상 valid함. */
				HxLOG_Print("SCD or SMATV\r\n");
				eHomResult = eHOM_Result_Ok;
			}
		}
	}

FUNC_END:

	if(NULL != pCurCoChannel)
	{
		PL_COMMON_ReleaseCurrentServiceInfo(pCurCoChannel);
	}

	return eHomResult;
}

STATIC void	_plstreaming_Resource_Task(void *vpMgr)
{
	HERROR				hErr = ERR_OK;
	HCHAR				*pszRequstTaskName = NULL;
	HCHAR				*pszRequstDupTaskName = NULL;

	_plStreamResMgr_t	*pstMgr = (_plStreamResMgr_t *)vpMgr;
	_plStreamResMsg_t	stResMsg;

	while(1)
	{
		HxSTD_memset(&stResMsg, 0, sizeof(_plStreamResMsg_t));

		/* 2014/12/22 mhkang:
		timeout 1000 => 10 변경한 이유:  RTSP SETUP+TP 이후 RTSP PLAY+SID+PID 형태로 오는 경우,
		RTSP PLAY 명령이 Background thread(PL_Streaming_Conte해xt_Task)에서 가능한 빨리 처리되기 위해서 10 msec로 줄였음.
		http://svn:3000/issues/95517 참고

		2015/1/14 mhkang: timeout값을 1로 했을 때는 SAT>IP 재생화면이 깨지는 경향이 높아지는 것 같음.
		timeout값을 높일 수록 TP정보 갱신 및 signal locking check timing이 늦어질 수 있음.
		*/
		hErr = VK_MSG_ReceiveTimeout(pstMgr->ulMsgId, &stResMsg, sizeof(_plStreamResMsg_t), 10);

		if (VK_TIMEOUT == hErr) // case timeout
		{
			PL_Streaming_Context_Task();
			continue;
		}
		else if(ERR_OK != hErr) // other error
		{
			HxLOG_Error("VK_MSG_Receive error(hErr:%d)!!\n", hErr);
			continue;
		}


		pszRequstTaskName = HLIB_STD_GetProcessName(stResMsg.nRequestPid);
		if(NULL != pszRequstTaskName)
		{
			pszRequstDupTaskName = HLIB_STD_StrDup(pszRequstTaskName);
		}
		else
		{
			pszRequstDupTaskName = "unknown";
		}

		if(NULL != pstMgr->pConflictCallback)
		{
			pstMgr->pConflictCallback(stResMsg.hStreamingId, (HINT32)stResMsg.eNotifyType, stResMsg.nRequestPid, pszRequstDupTaskName);
		}

		if(NULL != pszRequstDupTaskName)
		{
			HLIB_STD_MemFree(pszRequstDupTaskName);
			pszRequstDupTaskName = NULL;
		}
	}
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Resource_Init(void)
{
	HERROR				hErr = ERR_OK;
	HOM_Result_e 		eHomResult = eHOM_Result_Ok;
	_plStreamResMgr_t	*pstMgr = &s_stStreamResMgr;

	ENTER_FUNCTION;

	s_pWillRemoveList = NULL;

	// SAPI Notify 기본 연결:
	hErr = SAPI_RegisterNotifier (NULL, _plstreaming_Resource_SapiNotifier);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAPI_RegisterNotifier err: eHOM_Result_Error_InitFail\n");
		return eHOM_Result_Error_InitFail;
	}

	hErr = VK_SEM_Create (&s_ulPLStreamRes_SemId, "StRes_GSem", VK_SUSPENDTYPE_PRIORITY);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("VK_SEM_Create failed!!!\n");
		return eHOM_Result_Error_InitFail;
	}

	hErr = VK_SEM_Create (&s_ulPLStreamRes_ConfirmSemId, "StRes_CtSem", VK_SUSPENDTYPE_PRIORITY);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("VK_SEM_Create failed!!!\n");
		return eHOM_Result_Error_InitFail;
	}

	hErr = VK_MSG_Create(32, sizeof(_plStreamResMsg_t), "StRes_Msg", &pstMgr->ulMsgId, VK_SUSPENDTYPE_FIFO);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("VK_MSG_Create failed!!!\n");
		return eHOM_Result_Error_InitFail;
	}

	hErr = VK_TASK_Create(_plstreaming_Resource_Task, 40, SIZE_32K, "StRes_Task", pstMgr, &pstMgr->ulTaskId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Create failed!!!\n");
		return eHOM_Result_Error_InitFail;
	}

	VK_TASK_Start(pstMgr->ulTaskId);

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_Resource_DeInit(void)
{
	ENTER_FUNCTION;

	LEAVE_FUNCTION;

	return ;
}


/**
 *
 *
 *
 * @param
 * @return
 */
Handle_t PL_Streaming_Resource_New(Handle_t hStremaingId)
{
	_plStreamResInst_t	*pResInst = NULL;
	Handle_t			hResHandle = HANDLE_NULL;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pResInst = _plstreaming_Resource_NewInstance();
	if( NULL != pResInst)
	{
		hResHandle = pResInst->hResHandle;
		pResInst->hStreamingId = hStremaingId;
	}
	LEAVE_GSEM;

	LEAVE_FUNCTION;

	return hResHandle;
}

HOM_Result_e PL_Streaming_Resource_WaitForCancelSchedule(HUINT32 SamaSlot)
{
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;

	eHomResult = _plstreaming_Resource_CancelConfirmSchedule(SamaSlot);
	if (eHomResult != eHOM_Result_Ok)
	{
		HxLOG_Error("_plstreaming_Resource_CancelConfirmSchedule failed(ulSlot:%d, eHomResult:%d)\n",SamaSlot, eHomResult);
	}

	return eHomResult;
}

/**
 *
 *
 *
 * @param hResHandle
 * @param bWaiting SAPI_CancelSchedule 호출후 Sama로부터 eSAPI_NOTIFY_TYPE_ScheduleRemoved event받을 때까지 waiting하도록 합니다. default : TRUE.
 * @return 성공하면 Sama Schedule Slot을 리턴하고, 에러발생시 SAMASLOT_INITVAL 을 반환합니다.
 */
HUINT32 PL_Streaming_Resource_Delete(Handle_t hResHandle, HBOOL bWaiting)
{
	HUINT32				ulSlot = SAMASLOT_INITVAL;
	_plStreamResInst_t	*pResInst = NULL;
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pResInst = _plstreaming_Resource_GetInstance(hResHandle);
	if(NULL == pResInst)
	{
		HxLOG_Error("_plstreaming_Resource_GetInstance(hResHandle:%d) returns null\n", hResHandle);
		goto ErrBlock;
	}

	pResInst->bUseRingBuf = FALSE;

	ulSlot = pResInst->ulSamaSlot;

	if(SAMASLOT_INITVAL == ulSlot)
	{
		HxLOG_Error("invalid slot(%d)\n");
		goto ErrBlock;
	}

	// append to list
	eHomResult = _plstreaming_Resource_WillCancelSchedule(ulSlot);
	if (eHomResult != eHOM_Result_Ok)
	{
		HxLOG_Error("_plstreaming_Resource_WillCancelSchedule failed(ulSlot:%d, eHomResult:%d)\n",ulSlot, eHomResult);
		goto ErrBlock;
	}
	LEAVE_GSEM;

	// waiting for removed
	/* s_pWillRemoveList에 제대로 append 된 경우에만, 기다리는 게 의미 있음 */
	if (bWaiting == TRUE)
	{
		eHomResult = _plstreaming_Resource_CancelConfirmSchedule(ulSlot);
		if (eHomResult != eHOM_Result_Ok)
		{
			HxLOG_Error("_plstreaming_Resource_CancelConfirmSchedule failed(ulSlot:%d, eHomResult:%d)\n",ulSlot, eHomResult);
		}
	}

	ENTER_GSEM;
	_plstreaming_Resource_DeleteInstance(pResInst);
	LEAVE_GSEM;

	LEAVE_FUNCTION;
	return ulSlot;

ErrBlock:
	_plstreaming_Resource_DeleteInstance(pResInst);
	LEAVE_GSEM;
	LEAVE_FUNCTION;
	return SAMASLOT_INITVAL;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Resource_Start(const Handle_t hResHandle, PLStreaming_ModeInst_t *pModeInst, HUINT32 PlayPosition)
{
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	DxSchedule_t		stSchedule;
	UNIXTIME			utStartTime = (HLIB_STD_GetSystemTime());

	_plStreamResInst_t	*pResInst = NULL;

	HxLOG_Info("(+): PlayPosition=%d Tick=%d\n", PlayPosition, HLIB_STD_GetSystemTick());

	HxSTD_memset(&stSchedule, 0, sizeof(DxSchedule_t));

	ENTER_GSEM;

	pModeInst->eSetupStatus = ePLStreaming_SetupStatus_Start;
	eHomResult = _plstreaming_Resource_AddSchedule(pModeInst, &stSchedule, utStartTime, PlayPosition);
	if(eHOM_Result_Ok == eHomResult)
	{
		pResInst = _plstreaming_Resource_GetInstance(hResHandle);
		if(NULL != pResInst)
		{
			pResInst->ulSamaSlot 	= stSchedule.ulSlot;
			pResInst->utStartTime	= utStartTime;
			pResInst->utEndTime     = (utStartTime + stSchedule.ulDuration);
			pResInst->hRingbuf		= pModeInst->hRingbuf;
			pResInst->eSetupStatus  = ePLStreaming_SetupStatus_Start;
		}
		else
		{
			HxLOG_Error("Cannot find a instance of _plStreamResInst_t(hResHandle:%08x)\n",hResHandle);
			eHomResult = eHOM_Result_Error_Resource_Notfound;
			goto ErrBlock;
		}
	}
	else
	{
		HxLOG_Error("_plstreaming_Resource_AddSchedule fail! eHomResult(%s)\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));
	}

	LEAVE_GSEM;
	LEAVE_FUNCTION;

	return eHomResult;

ErrBlock:
	pModeInst->eSetupStatus = ePLStreaming_SetupStatus_Error;

	LEAVE_GSEM;
	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 *
 * @param hResHandle	Resource handle
 * @param SchDuration   SAMA schedule dur (sec)s
 * @param PeriodSec		SAMA Schedule를 업데이트하는 주기(단위:초)
 * @return
 */
void PL_Streaming_Resource_UpdateTime(const Handle_t hResHandle, int SchDuration, int PeriodSec)
{
	HUINT32				ulSlot = SAMASLOT_INITVAL;
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	UNIXTIME			utCur = 0, utEnd = 0, utUpdateDuration = 0;
	_plStreamResInst_t	*pInst = NULL;

	//HxLOG_Info("(+): PeriodSec:%d %d\n", PeriodSec, HLIB_STD_GetSystemTick());

	if (hResHandle == HANDLE_NULL)
	{
		HxLOG_Error("invalid args(hResHandle:%p)\n", hResHandle);
		return;
	}

	utCur = HLIB_STD_GetSystemTime();

	ENTER_GSEM;
	pInst 	= _plstreaming_Resource_GetInstance(hResHandle);
	if(NULL != pInst)
	{
		ulSlot = pInst->ulSamaSlot;
		utEnd  = pInst->utEndTime;

		if((utEnd > utCur)	&& ((utEnd - utCur) <  SchDuration - PeriodSec)) // TODO: 10초마다 (60 - 50) 갱신하지 말고, 10초 남았을 때 갱신하자
		{
			pInst->utEndTime = (utCur + SchDuration);
			utUpdateDuration = pInst->utEndTime - pInst->utStartTime;
		}
	}


	if(SAMASLOT_INITVAL == ulSlot)
	{
		HxLOG_Error("SAMASLOT_INITVAL == ulSlot error slot(hResHandle:%x, pInst:%x)\n", hResHandle, pInst);
		goto LOOP_END;
	}

	if(0 == utUpdateDuration)
	{
		goto LOOP_END;
	}

	HxLOG_Debug("left time (%d)\n", (utEnd-utCur));
	HxLOG_Debug("Update Schedule Time (Slot:%d, utCur:%d)\n", ulSlot, utCur);

	eHomResult = _plstreaming_Resource_UpdateScheduleTime(hResHandle, ulSlot, utUpdateDuration);
	if (eHomResult != eHOM_Result_Ok)
	{
		HxLOG_Error("Update Schedule Time error(eHomResult:%d)\n", eHomResult);
	}

LOOP_END:
	LEAVE_GSEM;

	LEAVE_FUNCTION;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Resource_GetStatus(const Handle_t hResHandle, PLStreaming_SetupStatus_e *peSetupStatus)
{
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	_plStreamResInst_t	*pResInst = NULL;

	//ENTER_FUNCTION;

	ENTER_GSEM;
	pResInst 	= _plstreaming_Resource_GetInstance(hResHandle);
	if(NULL != pResInst)
	{
		*peSetupStatus = pResInst->eSetupStatus;
	}
	LEAVE_GSEM;

	//LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 * for SAT>IP
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Resource_UpdateSchedulePids(PLStreaming_ModeInst_t* pstInstMode, HINT32 nPIDsCount, HUINT16 usPIDs[], Handle_t hMsgId)
{
	HUINT32				ulSlot = SAMASLOT_INITVAL;
	_plStreamResInst_t	*pInstRes = NULL;
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	UNIXTIME			utCur = 0, utEnd = 0, utUpdateDuration = 0;

	ENTER_FUNCTION;

	if (pstInstMode == NULL)
	{
		HxLOG_Error("Invalid args(pstInstMode is null)\n");
		return eHOM_Result_Error_Invalid_Arguments;
	}

	utCur = HLIB_STD_GetSystemTime();

	ENTER_GSEM;
	pInstRes 	= _plstreaming_Resource_GetInstance(pstInstMode->hResHandle);
	if (NULL == pInstRes)
	{
		HxLOG_Error("Not found pInstRes (hResHandle:%d)\n", pstInstMode->hResHandle);
		goto LOOP_END;
	}

	ulSlot = pInstRes->ulSamaSlot;
	utEnd  = pInstRes->utEndTime;
	pInstRes->hRingbuf = hMsgId; // 2015/4/6 mhkang: update handle of ringbuf.

	if((utEnd > utCur)	&& ((utEnd-utCur) < PL_STREAMING_SCHEDULE_LOWER_BOUND))
	{
		pInstRes->utEndTime = (utCur + PL_STREAMING_SCHEDULE_SATIP_DURATION);
		utUpdateDuration = pInstRes->utEndTime - pInstRes->utStartTime;
		//_plstreaming_Resource_AdjustDuration(ulSlot, pInst, &utUpdateDuration);
	}

	if(SAMASLOT_INITVAL == ulSlot)
	{
		HxLOG_Error("SAMASLOT_INITVAL == ulSlot error slot(hResHandle:%x, pInst:%x)\n", pstInstMode->hResHandle, pInstRes);
		eHomResult = eHOM_Result_Error;
		goto LOOP_END;
	}

	eHomResult = _plstreaming_Resource_UpdateSchedulePids(pstInstMode->stSetupInfo.eStreamType, ulSlot, utUpdateDuration, nPIDsCount, usPIDs, hMsgId);

LOOP_END:
	LEAVE_GSEM;

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 * for HLS
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Resource_UpdateSchedulePosition(const Handle_t hResHandle, HUINT32 ulPosition, Handle_t hMsgId)
{
	HUINT32				ulSlot = SAMASLOT_INITVAL;
	_plStreamResInst_t	*pResInst = NULL;
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	UNIXTIME			utCur = 0, utEnd = 0, utUpdateDuration = 0;

	ENTER_FUNCTION;

	utCur = HLIB_STD_GetSystemTime();

	ENTER_GSEM;
	pResInst 	= _plstreaming_Resource_GetInstance(hResHandle);
	if(NULL != pResInst)
	{
		pResInst->hRingbuf = hMsgId; // Handle of RingBuf

		ulSlot = pResInst->ulSamaSlot;
		utEnd  = pResInst->utEndTime;

		if((utEnd > utCur)	&& ((utEnd-utCur) < PL_STREAMING_SCHEDULE_LOWER_BOUND))
		{
			pResInst->utEndTime = (utCur + PL_STREAMING_SCHEDULE_SATIP_DURATION);
			utUpdateDuration = pResInst->utEndTime - pResInst->utStartTime;
			//_plstreaming_Resource_AdjustDuration(ulSlot, pInst, &utUpdateDuration);
		}
	}


	if(SAMASLOT_INITVAL == ulSlot)
	{
		HxLOG_Error("SAMASLOT_INITVAL == ulSlot error slot(hResHandle:%x, pResInst:%x)\n", hResHandle, pResInst);
		eHomResult = eHOM_Result_Error;
		goto LOOP_END;
	}

	eHomResult = _plstreaming_Resource_UpdateSchedulePosition(ulSlot, utUpdateDuration, ulPosition, hMsgId);

LOOP_END:
	LEAVE_GSEM;

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HINT32 PL_Streaming_Resource_GetRemaining (void)
{
	HUINT32 		ulLiveNumSch = 0;
	HUINT32 		ulFileNumSch = 0;
	HUINT32 		ulTotalNumSch = 0;
	HERROR			hErr = ERR_OK;

	ENTER_FUNCTION;
	ENTER_GSEM;

	ulLiveNumSch = -1;
	hErr = SAPI_CountScheduleByType(DxRSVTYPE_NETSTREAMING_LIVE, &ulLiveNumSch); // 2014/3/7 mhkang:
	if (ERR_OK != hErr || ulLiveNumSch == -1)
	{
		HxLOG_Error("Error> SAPI_CountScheduleByType(DxRSVTYPE_NETSTREAMING_LIVE,..) failed hErr(%x), live(%d)\n", hErr, ulLiveNumSch);
		goto ErrBlock;
	}
	ulTotalNumSch += ulLiveNumSch;

	ulFileNumSch = -1;
	hErr = SAPI_CountScheduleByType(DxRSVTYPE_NETSTREAMING_FILE, &ulFileNumSch);
	if (ERR_OK != hErr || ulFileNumSch == -1)
	{
		HxLOG_Error("Error> SAPI_CountScheduleByType(DxRSVTYPE_NETSTREAMING_FILE,..) failed hErr(%x), file(%d)\n", hErr, ulFileNumSch);
		goto ErrBlock;
	}
	ulTotalNumSch += ulFileNumSch;

	HxLOG_Warning("LiveStreamingSchedule:%d FileStreamingSchedule:%d\n", ulLiveNumSch, ulFileNumSch);

	if ((ulLiveNumSch > PL_STREAMING_MAX_NUM) ||
		(ulFileNumSch > PL_STREAMING_MAX_NUM) ||
		(ulTotalNumSch > PL_STREAMING_MAX_NUM))
	{
		HxLOG_Error("Error> invalid sama resource(live:%d, file:%d, ttl:%d)\n",
				ulLiveNumSch, ulFileNumSch, ulTotalNumSch);
		goto ErrBlock;
	}
	HxLOG_Debug("sama slot(live:%d, file:%d, total:%d)\n", ulLiveNumSch, ulFileNumSch, ulTotalNumSch);
	LEAVE_GSEM;
	LEAVE_FUNCTION;
	return (PL_STREAMING_MAX_NUM - (HINT32)ulTotalNumSch);

ErrBlock:
	LEAVE_GSEM;
	LEAVE_FUNCTION;
	return -1;
}

/**
 *
 *
 *
 * @param
 * @return
 */
void PL_Streaming_Resource_SetConflictCallback (cbPLConflictSchedule cbCallback)
{
	_plStreamResMgr_t		*pstMgr = &s_stStreamResMgr;

	ENTER_FUNCTION;

	ENTER_GSEM;

	pstMgr->pConflictCallback = cbCallback;

	LEAVE_GSEM;

	LEAVE_FUNCTION;
}


_plStreamResInst_t *PL_Streaming_Resource_GetInstanceByRingBuf (Handle_t hRingBuf)
{
	return _plstreaming_Resource_GetInstanceByRingBuf(hRingBuf);
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/


void	PL_Streaming_Resource_NotUsedFunc_RemoveWarning (void)
{
	(void)_plstreaming_Resource_GetInstanceBySlot;
}

