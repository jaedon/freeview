/**
	@file     xmgr_pvr_action_base.c
	@brief
	Description:
	Module:

	Copyright (c) 2013 HUMAX Co., Ltd.
	All rights reserved.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <sapi.h>
#include <otl.h>
#include <octo_common.h>

#include <db_svc.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_epg.h>
#include <svc_sys.h>
#include <svc_output.h>

#include <bus.h>


#include <isosvc.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#include <mgr_storage.h>
#include <mgr_pvr_action.h>
#include <mgr_recorder.h>
#include <mgr_playback.h>
#include <mgr_transcoder.h>

#include <uapi.h>

#include "../local_include/_xmgr_pvr_action.h"
#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
#include <sapi.h>
#endif

#define CMD_PRINT			HLIB_CMD_Printf

#define PVR_MGR_DELAY_TSRREC_TIMERID			0x00000100
#define PVR_MGR_DELAY_TSRREC_TIMERID_MASK		0xFFFFFF00
#define PVR_MGR_DELAY_TSRREC_TIMERID_ACTIDMASK	0x000000FF

#define SPACE_PER_MIN_FOR_SD					(15 * 1024 / 10)	// 1.5GB per hour
#define SPACE_PER_MIN_FOR_HD					(4 * 1024)			// 4GB per hour

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ePbTrRecMode_none,				// not releated to transcoder
	ePbTrRecMode_SectionGathering,	// For section gathering (PMT, SIT...) -> first step
	ePbTrRecMode_TranscodeRecording,	// For Transcode recording  -> second step
} Pb_TrRecMode_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

STATIC HERROR			xmgr_pvrbase_preprocessForDestroyingPvrAction(XmgrPvr_Action_t *pstAction);
STATIC HERROR			xmgr_pvrbase_postprocessForDestroyingPvrAction(XmgrPvr_Action_t *pstAction);

STATIC HERROR			xmgr_pvrbase_stopAllPvrFunction(void);
STATIC HERROR 			xmgr_pvrbase_stopPvrFunction(HUINT32 ulPvrIdx);

STATIC XmgrPvr_Action_t*	xmgr_pvrbase_getPvrActionForPb (XmgrPvr_Context_t *pstContext, HUINT32 ulActionId);

STATIC HBOOL			xmgr_pvrbase_isTsrAccepted(HUINT32 ulActionId);
STATIC XmgrPvr_Action_t*	xmgr_pvrbase_getPvrActionForTsrNormal(XmgrPvr_Context_t *pstContext, HUINT32 ulWtvActId, HUINT32 *pulActionId);
STATIC HERROR			xmgr_pvrbase_changeTsrNormalSvc(HUINT32 ulWtvActionId, Handle_t hMasterSvc, Handle_t hSuppleSvc, HINT32 eSessionType, HUINT32 ulStartDelayTime);
STATIC HERROR			xmgr_pvrbase_tryTsrNormal(HUINT32 ulTsrStartDelayTime);
STATIC XmgrPvr_Action_t*	xmgr_pvrbase_getPvrActionForTsrLiveStream (XmgrPvr_Context_t *pstContext, HUINT32 *pulActionId);
STATIC HERROR			xmgr_pvrbase_startTsrLiveStreaming(Handle_t hService, HUINT32 *pulActionId, Handle_t *phLsHandle);
STATIC HERROR			xmgr_pvrbase_stopTsrLiveStreaming(Handle_t hLsHandle);

STATIC XmgrPvr_Action_t*	xmgr_pvrbase_getPvrActionForRec(XmgrPvr_Context_t *pstContext, HUINT32 *pulActionId);
STATIC HERROR			xmgr_pvrbase_startChasePlayback(HUINT32 ulActionId, Handle_t hCurrSvc, Handle_t hMasterSvc, ChListUiGroup_t eSvcListGroup, MgrRec_RecInfo_t *pstRecInfo, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPosOrigin, HUINT32 ulOffset, MgrPb_Option_e ePbOption);
STATIC HERROR			xmgr_pvrbase_startTsrPlayback(HUINT32 ulActionId, Handle_t hCurrSvc, Handle_t hMasterSvc, ChListUiGroup_t eSvcListGroup, MgrRec_RecInfo_t *pstRecInfo, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPosOrigin, HUINT32 ulOffset, MgrPb_Option_e ePbOption);
STATIC HERROR			xmgr_pvrbase_startNormalPlayback(HUINT32 ulActionId, HUINT32 ulContentId, HUINT32 ulStartTime, HBOOL bMakeThumbnail, Pb_TrRecMode_t eTrRecMode);
STATIC HERROR 			xmgr_pvrbase_getPvrIdxbyContentId(HUINT32 ulContentId, HUINT32 *pulPvrIdx);

STATIC XmgrPvr_Action_t*	xmgr_pvrbase_checkRecordPossible(MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason);

STATIC BUS_Result_t		xmgr_pvrbase_msgBusCreate(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgBusDestroy(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgBusTimer(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgWtvSvcChanged(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgAmActionSetup (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgAmActionTaken(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgAmActionReleased(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgAmActionTypeChanged(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_pvrbase_msgAppRebootStb (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC BUS_Result_t xmgr_pvrbase_evtStoragePvrEnabled(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_pvrbase_evtStoragePvrDisabled(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#if defined(CONFIG_MW_MM_DELAYED_REC)
STATIC BUS_Result_t		xmgr_pvrbase_evtRecDelayedRecordStatusChange(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif



#if defined(CONFIG_DEBUG)
STATIC HUINT8			*xmgr_PVR_BASE_GetPvrActionTypeStr(XmgrPvr_ActionType_e ePvrActType);
STATIC HUINT8			*xmgr_PVR_BASE_GetPvrActionStateStr(XmgrPvr_ActionState_e ePvrActState);
#endif

/*******************************************************************/
/************************ Static Function *******************************/
/*******************************************************************/

#define _____PVR_ACTION_SCENARIO_FUNCTIONS_____
// 특정 Action이 죽거나 살면 여기서 처리한다, 라는 시나리오를 여기에 모아 놓도록 한다.
// 여기는 시나리오에 맞게 특정 PVR Action을 없애거나 생성할 경우, 연달아 혹은 그 직후에 해야 할 행동을 나열하기
// 때문에 코드 성격이 recursive 하게 갈 수 밖에 없다.

/*
	특정 PVR Action이 죽을 경우, 그 직전에 처리해 줘야 할 경우에는 여기에 처리.
		EX) TSR Action이 죽기 전에 TSR PB 를 먼저 닫아야 한다.

	주의 : Pre-processor로 그에 관련된 PVR Action을 '죽일' 경우엔 XmgrPvr_Action_t 내의 pfActFunc 를 직접 Kill 하지 말고
		   반드시 xmgr_pvr_DestroyPvrAction 명령을 사용하도록 한다.
		   그 Proc 역시 죽을 때 반드시 해 줘야 할 일이 있을 수 있기 때문이다.
*/
STATIC HERROR xmgr_pvrbase_preprocessForDestroyingPvrAction(XmgrPvr_Action_t *pstAction)
{
	XmgrPvr_Action_t 		*pstRelAction;

	if (pstAction == NULL)						{ return ERR_FAIL; }

	// 특정 PVR Action의 Destroy 이전에 따라 와야 항목은 여기에 추가하도록 한다.
	// Preprocess Action Start :
	switch (pstAction->eType)
	{
	case ePvrActType_TSR_Normal:
		// TSR과 관련 있는 TSR Playback Action이 있으면 TSR Playback을 먼저 죽여야 한다.
		pstRelAction = xmgr_pvr_FindPvrActionByRelatedActionId(ePvrActType_PB_Tsr, pstAction->ulActionId);
		if (pstRelAction != NULL)
		{
			xmgr_pvr_DestroyPvrAction(pstRelAction);
		}

		break;

	default:
		break;
	}

	// Preprocess Action End :
	return ERR_OK;
}

/*
	특정 PVR Action이 죽을 경우, 그 직후에 처리해야 할 작업을 여기에 모은다.
		EX) TSR Action이 죽기 전에 TSR PB 를 먼저 닫아야 한다.

	주의 : Pre-processor로 그에 관련된 PVR Action을 '죽일' 경우엔 XmgrPvr_Action_t 내의 pfActFunc 를 직접 Kill 하지 말고
		   반드시 xmgr_pvr_DestroyPvrAction 명령을 사용하도록 한다.
		   그 Proc 역시 죽을 때 반드시 해 줘야 할 일이 있을 수 있기 때문이다.
*/
STATIC HERROR xmgr_pvrbase_postprocessForDestroyingPvrAction(XmgrPvr_Action_t *pstAction)
{
//	XmgrPvr_Action_t 		*pstRelAction;

	if (pstAction == NULL)						{ return ERR_FAIL; }

	// 특정 PVR Action의 Destroy 이후에 따라 와야 항목은 여기에 추가하도록 한다.
	// Post Action Start :


	// Post Action End :
	return ERR_OK;
}

#define _____ACTION_FUNCTIONS_____
STATIC HERROR xmgr_pvrbase_stopPvrFunction(HUINT32 ulPvrIdx)
{
	HUINT32 				 ulIndex;
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t			*pstAction;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL!!\n");
		return ERR_FAIL;
	}

	// 1. Play back
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);

		if(pstAction->ulPvrIdx == ulPvrIdx)
		{
			xmgr_pvr_DestroyPvrAction(pstAction);
		}
	}

	// 2. Record / TSR Function
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if(pstAction->ulPvrIdx == ulPvrIdx)
		{
			xmgr_pvr_DestroyPvrAction(pstAction);
		}
	}

	// 3. TrRec
	BUS_SendMessage(NULL, eMEVT_PVR_TRREC_STOP, HANDLE_NULL, (HINT32)TRUE, (HINT32)ulPvrIdx, 0);

	return ERR_OK;
}

STATIC HERROR xmgr_pvrbase_stopAllPvrFunction(void)
{
	HUINT32 				 ulIndex;
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t			*pstAction;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL!!\n");
		return ERR_FAIL;
	}

	// 1. Play back
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);
		xmgr_pvr_DestroyPvrAction(pstAction);
	}

	// 2. Record / TSR Function
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		xmgr_pvr_DestroyPvrAction(pstAction);
	}

	// 3. BG Function
	for (ulIndex = 0; ulIndex < NUM_BG_ACTION; ulIndex++)
	{
		pstAction = &(pstContext->astTrPbAction[ulIndex]);
		xmgr_pvr_DestroyPvrAction(pstAction);
	}

	return ERR_OK;
}

#define _____ACTION_FIND_FUNCTIONS_____
STATIC HERROR xmgr_pvrbase_setPvrActionByOuterInfo(XmgrPvr_Action_t *pstAction, HUINT32 ulActionId, XmgrPvr_ActionType_e ePvrActType)
{
	MgrPb_Info_t			 stPbInfo;
	MgrRec_RecInfo_t		 stRecInfo;
	XmgrPvr_Action_t		*pstRelAction;
	HERROR					 hErr;

	// Action change가 일어나는 경우 : PrevPvrActType이 Destroy되는 것과 동일한 개념이므로
	// 그 PvrActType이 Destroy될 때 같이 작업해야 하는 것을 처리해야 한다.
	if (pstAction->eType != ePvrActType)
	{
		xmgr_pvrbase_preprocessForDestroyingPvrAction(pstAction);
		xmgr_pvrbase_postprocessForDestroyingPvrAction(pstAction);
	}

	// 새 Action Type으로 Create된 것과 동일함.
	switch (ePvrActType)
	{
	case ePvrActType_TSR:
	case ePvrActType_TSR_Normal:
		pstAction->eType = ePvrActType_TSR_Normal;
		pstAction->ulRelatedActId = MGR_ACTION_GetMainActionId();
		(void)MGR_ACTION_GetMasterSvcHandle(ulActionId, &pstAction->hMasterSvcHandle);
		(void)MGR_ACTION_GetActualSvcHandle(ulActionId, &pstAction->hCurrSvcHandle);
		break;

	case ePvrActType_REC:
	case ePvrActType_REC_Normal:
		hErr = MGR_RECORDER_GetRecordingSvcInfo(ulActionId, &stRecInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_RECORDER_GetRecordingSvcInfo ret:0x%X\n",hErr);
			return ERR_FAIL;
		}

		switch (stRecInfo.eRecType)
		{
		case eMgrRecType_DELAYED:
			pstAction->eType = ePvrActType_REC_Delayed;
			break;

		case eMgrRecType_EBR:
		case eMgrRecType_SBR:
			pstAction->eType = ePvrActType_REC_EvtReserved;
			break;

		case eMgrRecType_INSTANT:
		case eMgrRecType_TBR:
		case eMgrRecType_FULLTSREC:
		default:
			pstAction->eType = ePvrActType_REC_Normal;
			break;
		}

		pstAction->ulRelatedActId	= HANDLE_NULL;
		pstAction->hMasterSvcHandle	= stRecInfo.hMasterSvc;
		pstAction->hCurrSvcHandle	= stRecInfo.hSvc;
		break;

	case ePvrActType_PB:
	case ePvrActType_PB_Normal:
		hErr = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stPbInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_PLAYBACK_GetPlaybackInfo ret:0x%X\n",hErr);
			return ERR_FAIL;
		}

		pstAction->eType			= ePvrActType_PB_Normal;
		pstAction->ulRelatedActId	= HANDLE_NULL;
		pstAction->hMasterSvcHandle	= stPbInfo.hMasterSvc;
		pstAction->hCurrSvcHandle	= stPbInfo.hSvc;
		break;

	case ePvrActType_PB_Chase:
		hErr = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stPbInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_PLAYBACK_GetPlaybackInfo ret:0x%X\n",hErr);
			return ERR_FAIL;
		}

		pstRelAction = xmgr_pvr_FindPvrActionByServiceHandle(ePvrActType, eLiveViewSession_REPLACE, stPbInfo.hMasterSvc, stPbInfo.hSvc);
		if (pstRelAction != NULL)
		{
			pstAction->ulRelatedActId	= pstRelAction->ulActionId;
		}
		else
		{
			HxLOG_Error("[xmgr_pvrbase_setPvrActionByOuterInfo] Chase PB, but no related Record. ulActionId(%d), MasterSvc(%08x), Svc(%08x)\n", ulActionId, stPbInfo.hMasterSvc, stPbInfo.hSvc);
			pstAction->ulRelatedActId	= HANDLE_NULL;
		}

		pstAction->eType			= ePvrActType_PB_Chase;
		pstAction->hMasterSvcHandle	= stPbInfo.hMasterSvc;
		pstAction->hCurrSvcHandle	= stPbInfo.hSvc;
		break;

	case ePvrActType_PB_Tsr:
		hErr = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stPbInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_PLAYBACK_GetPlaybackInfo ret:0x%X\n",hErr);
			return ERR_FAIL;
		}

		pstAction->eType			= ePvrActType_PB_Tsr;
		xmgr_pvr_GetTsrRecActionId_Base(stPbInfo.hSvc, &(pstAction->ulRelatedActId));
		pstAction->hMasterSvcHandle	= stPbInfo.hMasterSvc;
		pstAction->hCurrSvcHandle	= stPbInfo.hSvc;
		break;

	case ePvrActType_REC_ExtTsLine:
		pstAction->eType = ePvrActType_REC_ExtTsLine;
		pstAction->ulRelatedActId	= HANDLE_NULL;
		pstAction->hMasterSvcHandle	= HANDLE_NULL;
		pstAction->hCurrSvcHandle	= HANDLE_NULL;
		break;

	default:
		HxLOG_Error("ePvrActType is unknown:0x%X\n",ePvrActType);
		return ERR_FAIL;
	}

	pstAction->eState			= ePvrActState_Working;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulTimerId		= 0;
	pstAction->pfActFunc		= (BUS_Callback_t)MGR_ACTION_GetActionProc(ulActionId);

	return ERR_OK;
}


#define _____REC_CONTROL_FUNCTIONS_____

STATIC XmgrPvr_Action_t *xmgr_pvrbase_getPvrActionForPb (XmgrPvr_Context_t *pstContext, HUINT32 ulActionId)
{
	HUINT32				 ulIndex;
	XmgrPvr_Action_t		*pstAction;

	if (pstContext == NULL)						{ return NULL; }

	// 현재 사용하는 PVR Action 중 Action ID가 같은 Action이 있으면 일단 그것을 없애고 시작한다.
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);
		if ((pstAction->eState != ePvrActState_None) && (pstAction->ulActionId == ulActionId))
		{
			xmgr_pvr_DestroyPvrAction(pstAction);

			return pstAction;
		}
	}

	// 비어 있는 Action을 잡도록 한다.
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);
		if (pstAction->eState == ePvrActState_None)
		{
			return pstAction;
		}
	}

	return NULL;
}

STATIC HERROR xmgr_pvrbase_findTargetPvr(HUINT32 ulReqSize, HUINT32 *pulPvrIdx)
{
	HUINT32			ulPvrIdx, ulCandidatePvrIdx = 0xFFFFFFFF;
	HUINT64			ullReqSizeMb = 0, ullAvailablePvrKbSize = 0, ullAvailSizeMb = 0, ullReservedSizeMb = 0, ullTsrSizeMb = 0;
	HUINT64			ullReservedSizeKb = 0;
	HERROR			hErr;
	SvcFs_DevIdx_e	eDevIdx;
	SvcFs_DevInfo_t	stMwDevInfo;
	HCHAR			szTsrPath[SVC_FS_PVR_DIR_PATH_LEN];

	// internal/Detachable HDD만 검색한다...
	for (eDevIdx = eSVC_FS_DEV_sda; eDevIdx < eSVC_FS_DEV_MAX; eDevIdx++)
	{
		hErr = SVC_FS_GetDeviceInfo(eDevIdx, &stMwDevInfo);
		if(hErr == ERR_OK)
		{
			// Internal/Detachable HDD 등만 검색한다.
			if ((eSVC_FS_DevType_IntHdd != stMwDevInfo.eDevType) &&
				(eSVC_FS_DevType_Detachable != stMwDevInfo.eDevType))
			{
				HxLOG_Debug("Device(%d) is not proper device : devType(%d)\n", eDevIdx, stMwDevInfo.eDevType);
				continue;
			}

			// 정상적인 Device냐....
			if(stMwDevInfo.bMounted == FALSE
				|| stMwDevInfo.eDevStatus != eDevStatus_OK)
			{
				HxLOG_Debug("Device(%d) is not available for PVR\n", eDevIdx);
				continue;
			}
			// PVR 가능한 Device냐....
			if(SVC_FS_IsDevicePvrPossible(eDevIdx)==FALSE)
			{
				HxLOG_Debug("Device(%d) is not available for PVR\n", eDevIdx);
				continue;
			}
			// PVR Index 가져오고....
			hErr = SVC_FS_GetPvrIdxByDevIdx(&ulPvrIdx, eDevIdx);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("Device(%d) is not available for PVR\n", eDevIdx);
				continue;
			}
			// Device 의 Hdd 용량을 가져오고...
			hErr = SVC_FS_GetSize(eDevIdx, NULL, NULL, NULL, NULL, &ullAvailablePvrKbSize);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("Device(%d) is not available for PVR\n", eDevIdx);
				continue;
			}

			// Get Reserved size
			hErr = SVC_FS_GetPvrReservedSize(eDevIdx, &ullReservedSizeKb);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("Device(%d) is not available for PVR\n", eDevIdx);
				continue;
			}

			// TSR 로 Reserved 된 공간 체크해두고...
			ullReservedSizeMb = (ullReservedSizeKb + DxSVC_FS_PVR_SAFE_AREA_SIZE_KB) / SIZE_1K;
			HxSTD_MemSet(szTsrPath, 0, SVC_FS_PVR_DIR_PATH_LEN);

			// 실제 TSR 파일의 크기를 구하여 ullReservedSizeMb에서 뺀다 (중복 계산 방지)
			hErr = SVC_FS_GetPvrPath (ulPvrIdx, eSVC_FS_UsageType_Tsr, szTsrPath);
			if (ERR_OK == hErr)
			{
				hErr = HLIB_STORAGE_GetPathSize(szTsrPath, &ullTsrSizeMb);
				if (hErr == ERR_OK && ullTsrSizeMb > 0)
				{
					ullTsrSizeMb /= (SIZE_1K*SIZE_1K);
					ullReservedSizeMb -= ullTsrSizeMb;
				}
			}
			ullReqSizeMb = ulReqSize;
			ullAvailSizeMb = ullAvailablePvrKbSize / SIZE_1K;

			// Available HDD Size에서 Reserved 된 만큼은 제외....
			if (ullAvailSizeMb > ullReservedSizeMb)
			{
				ullAvailSizeMb -= ullReservedSizeMb;
			}
			else
			{
				ullAvailSizeMb = 0;
			}

			// 그래도 여유가 요구되는 Size이상으로 존재할 경우라면 그 Device 당첨...아니면 다음 Index로....
			if(ullAvailSizeMb > 0 && ullAvailSizeMb >= ullReqSizeMb)
			{
				if(stMwDevInfo.eDevType== eSVC_FS_DevType_IntHdd)
				{
					*pulPvrIdx = ulPvrIdx;
					// 에러는 아니지만...not available 로그만 나오면 트집잡히므로...
					HxLOG_Error("Device(%d) is available for PVR\n", ulPvrIdx);
					return ERR_OK;
				}
				else
				{
					ulCandidatePvrIdx = ulPvrIdx;
				}
			}
		}
		else
		{
			// 첫 부팅시에는 Detachable HDD에 대해 보장 못함...
			HxLOG_Debug("Device(%d) is not available for PVR\n", eDevIdx);
		}
	}

	if (ulCandidatePvrIdx != 0xFFFFFFFF)
	{
		// 여기까지 왔으면 keep해둔걸 쓰자.
		*pulPvrIdx = ulCandidatePvrIdx;
		// 에러는 아니지만...not available 로그만 나오면 트집잡히므로...
		HxLOG_Error("Device(%d) is available for PVR\n", ulCandidatePvrIdx);
		return ERR_OK;
	}
	else
	{
		*pulPvrIdx = SVC_FS_GetDefaultPvrIdx();
		HxLOG_Error("All devices are not available for PVR\n" );
		return ERR_FAIL;
	}
}

STATIC HUINT32 xmgr_pvrbase_getAvailablePvrIdx(Handle_t hCurSvc, MgrRec_RecordType_e	eRecType, HUINT32	ulSlot)
{
	HERROR			hErr;
	HUINT32 		ulPvrIdx = 0xffff, ulTargetPvrIdx = 0xffff; /* fix prevent #181366 (uninitialize var.) */
	DbSvc_Info_t	stSvcInfo;
	HUINT32 		ulRecTimePerMin = 0;
	HUINT32 		ulReqFreeSize;
	DxSchedule_t	stSchedule;

	ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
	if(ulPvrIdx >= MAX_CONNECTABLE_PVR_STORAGE_NUM)
	{
		HxLOG_Error("Cannot found Default Pvr Idx\n");
		return ulPvrIdx;
	}

	switch(eRecType)
	{
		case eMgrRecType_TSR:
			// TSR 은 Slot을 받을 필요없다.
			ulRecTimePerMin = MGR_RECORDER_GetRecorderParam(eMgrRec_DEFAULT_DURATION_TSR_RECORD)/60;
			break;
		case eMgrRecType_DELAYED:
			ulRecTimePerMin = 0;
			break;
		case eMgrRecType_TBR:
		case eMgrRecType_EBR:
		case eMgrRecType_SBR:
		case eMgrRecType_INSTANT:
		case eMgrRecType_SCR2DESC:
		case eMgrRecType_FULLTSREC:
			if(SAPI_GetSchedule(ulSlot, &stSchedule)==ERR_OK)
			{
				ulRecTimePerMin = (stSchedule.ulDuration + stSchedule.uExtInfo.stRec.ulPostOffsetTime+ stSchedule.uExtInfo.stRec.ulPreOffsetTime)/60 + 1;
			}
			break;
		default:
			return ulPvrIdx;
	}

	hErr = DB_SVC_GetServiceInfo(hCurSvc, &stSvcInfo);
	if(hErr == ERR_OK)
	{
		if(DbSvc_GetSvcType(&stSvcInfo) == eDxSVC_TYPE_TV && DbSvc_GetVideoType(&stSvcInfo) == eDxVIDEO_TYPE_HD)
		{
			// HD
			ulReqFreeSize = ulRecTimePerMin * SPACE_PER_MIN_FOR_HD / 60;
		}
		else
		{
			// SD
			ulReqFreeSize = ulRecTimePerMin * SPACE_PER_MIN_FOR_SD / 60;
		}

		ulTargetPvrIdx = ulPvrIdx;

		hErr = xmgr_pvrbase_findTargetPvr(ulReqFreeSize, &ulTargetPvrIdx);
		if(hErr == ERR_OK)
		{
			if(ulPvrIdx != ulTargetPvrIdx)
			{
				ulPvrIdx = ulTargetPvrIdx;
			}
		}
	}
	HxLOG_Debug("ulPvrIdx: 0x%x \n", ulPvrIdx);
	return ulPvrIdx;
}

#define _____TSR_CONTROL_FUNCTIONS_____
STATIC HBOOL xmgr_pvrbase_isTsrInUsbmem (HUINT32 ulPvrIdx)
{
	SvcFs_DevIdx_e eDevIdx;
	HUINT32 ulPartIdx;
	SvcFs_DevInfo_t stMwDevInfo;
	HERROR err;

	err = SVC_FS_GetPvrDevIdPartIdx (ulPvrIdx, &eDevIdx, &ulPartIdx);
	if (err != ERR_OK)
	{
		return FALSE;
	}

	err = SVC_FS_GetDeviceInfo(eDevIdx, &stMwDevInfo);

	if (err == ERR_OK && eSVC_FS_DevType_UsbMem == stMwDevInfo.eDevType)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL xmgr_pvrbase_isTsrInUsbmemByDevIdx (HUINT32 ulDevIdx)
{
	SvcFs_DevInfo_t stMwDevInfo;
	HERROR hErr;

	hErr = SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)ulDevIdx, &stMwDevInfo);
	if (hErr == ERR_OK && eSVC_FS_DevType_UsbMem == stMwDevInfo.eDevType)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC XmgrPvr_Action_t *xmgr_pvrbase_getPvrActionForTsrLiveStream (XmgrPvr_Context_t *pstContext, HUINT32 *pulActionId)
{
	HUINT32				 ulIndex, ulActionId;
	XmgrPvr_Action_t		*pstAction;

	if (pstContext == NULL || pulActionId == NULL)					{ return NULL; }

	// 비어 있는 Record Action을 할당해서 현재 사용하지 않는 TSR용 Action ID를 할당해 준다.
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if (pstAction->eState != ePvrActState_None)				{ continue; }

		for (ulActionId =  eActionId_REC_FIRST; ulActionId <=  eActionId_REC_LAST; ulActionId++)
		{
			if (MGR_ACTION_GetType(ulActionId) == eActionType_NONE)
			{
				*pulActionId = ulActionId;
				return pstAction;
			}
		}

		return NULL;
	}

	return NULL;
}

STATIC HERROR xmgr_pvrbase_startTsrLiveStreaming(Handle_t hService, HUINT32 *pulActionId, Handle_t *phLsHandle)
{
	HUINT32					 ulActionId;
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t			*pstAction;
	MgrRec_RecSetup_t			 stRecSetup;
	HERROR					 hErr;
	HUINT32				ulPvrIdx = 0;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("xmgr_pvr_GetContext ret :0x%X\n",pstContext);
		return ERR_FAIL;
	}

	// if SVC_FS_UseTsrOnlyDevice() is TRUE then ulPvrIdx is not used
	if( SVC_FS_UseTsrOnlyDevice() == FALSE )
	{
		ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
		if(0xffff == ulPvrIdx || MAX_CONNECTABLE_PVR_STORAGE_NUM <= ulPvrIdx)
		{
			return ERR_FAIL;
		}

		// PVR device가 문제 없을 경우에만 동작한다.
		if (pstContext->ePvrDevState[ulPvrIdx] != ePvrDevState_OK)
		{
			HxLOG_Error("[xmgr_pvrbase_changeTsrNormalSvc] PVR Device is not ready.\n");
			return ERR_FAIL;
		}
	}

	HxSTD_MemSet(&stRecSetup, 0, sizeof(stRecSetup));

	stRecSetup.eRecDst = eMgrRecOutput_HDD;
	stRecSetup.eRecType = eMgrRecType_TSR;
	stRecSetup.eSessionType = eLiveViewSession_NORMAL;
	stRecSetup.ulLiveActionId = HANDLE_NULL;
	stRecSetup.ulSlot = DxRSV_SLOT_NULL;
	stRecSetup.ulSourceContentId = 0xffffffff;

	stRecSetup.hCurrSvc = hService;
	stRecSetup.hMasterSvc = hService;

	// TSR Live Stream을 위해 비는 Action을 가져온다.
	pstAction = xmgr_pvrbase_getPvrActionForTsrLiveStream (pstContext, &ulActionId);
	if (pstAction == NULL)
	{
		HxLOG_Error("No empty recording action found\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_SetupRecord(ulActionId, eRecType_TSR_REC, stRecSetup.hCurrSvc, stRecSetup.eRecType, ulPvrIdx, eAmPriority_RECORD);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_SetupRecord err(0x%08x), hCurrSvc(0x%08x)\n", hErr, stRecSetup.hCurrSvc);
		return ERR_FAIL;
	}

	// Context에 등록한다.
	pstAction->eState			= ePvrActState_Working;
	pstAction->eType			= ePvrActType_TSR_LiveStream;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulRelatedActId	= HANDLE_NULL;
	pstAction->hMasterSvcHandle	= stRecSetup.hMasterSvc;
	pstAction->hCurrSvcHandle	= stRecSetup.hCurrSvc;
	pstAction->ulTimerId		= 0;
	pstAction->ulPvrIdx			= ulPvrIdx;
	pstAction->pfActFunc		= (BUS_Callback_t)MGR_RECORDER_GetProcFunc(ulActionId);

	// TSR_REC 시작
	MGR_RECORDER_SetRecSetup (ulActionId - eActionId_REC_FIRST, &stRecSetup);
	BUS_MGR_Create(MGR_RECORDER_GetProcName(ulActionId), APP_PVR_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	*pulActionId = ulActionId;

	// Live Stream Handle은 후에 하나의 TSR Action에 다수의 Request가 엮일 경우를 따지면 ActionID와 독립적이어야 한다.
	// 현재 상태에서는 ActionID로 대신해도 되므로 사용하기로 한다.
	*phLsHandle = (Handle_t)ulActionId;
	return ERR_OK;
}

STATIC HERROR xmgr_pvrbase_stopTsrLiveStreaming(Handle_t hLsHandle)
{
	HUINT32				 ulActionId = (HUINT32)hLsHandle;
	XmgrPvr_Action_t		*pstAction;

	pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_TSR_LiveStream, ulActionId);
	if (NULL == pstAction)
	{
		HxLOG_Error("LiveStreamHandle(0x%08x) No TSR LiveStream Action\n", hLsHandle);
		return ERR_FAIL;
	}

	xmgr_pvr_DestroyPvrAction(pstAction);
	return ERR_OK;
}

#define _____PB_CONTROL_FUNCTIONS_____

STATIC XmgrPvr_Action_t *xmgr_pvrbase_getPvrActionForRec(XmgrPvr_Context_t *pstContext, HUINT32 *pulActionId)
{
	HUINT32				 ulIndex, ulActionId;
	HUINT32				 ulCount;
	XmgrPvr_Action_t		*pstAction;

	if (pstContext == NULL || pulActionId == NULL)					{ return NULL; }

	// 1. CONFIG로 정의한 최대 Non TSR Record 수를 넘었는가 체크
	ulCount = 0;
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if ((pstAction->eState != ePvrActState_None) && ((pstAction->eType & ePvrActType_MASK) == ePvrActType_REC))
		{
			ulCount++;
		}
	}

	if (ulCount >= PVRMGR_MAX_NONTSR_REC)
	{
		HxLOG_Error("[xmgr_pvrbase_getPvrActionForRec] Already REC Num Exceeded\n");
		return NULL;
	}

	// 비어 있는 Record Action 을 찾는다.
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if (pstAction->eState == ePvrActState_None)
		{
			for (ulActionId = eActionId_REC_FIRST; ulActionId <=  eActionId_REC_LAST; ulActionId++)
			{
				if (MGR_ACTION_GetType(ulActionId) == eActionType_NONE)
				{
					*pulActionId = ulActionId;
					return pstAction;
				}
			}

			return NULL;
		}
	}

	// 혹시 TSR이 있으면 TSR을 뺏도록 한다.
	// 이 경우는 PVRMGR_ACTION_REC_NUM < PVRMGR_MAX_TSR + PVRMGR_MAX_NONTSR_REC 일 경우 일어난다.
	// ex) PVRMGR_MAX_NONTSR_REC = 2, PVRMGR_MAX_TSR = 1, PVRMGR_ACTION_REC_NUM = 2
	// 1 Rec 1 TSR 중 Rec 추가되면 TSR이 없어지고 2 Rec으로 되어야 한다.
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if ((pstAction->eState != ePvrActState_None) &&
			(pstAction->eType == ePvrActType_TSR_Normal))
		{
			*pulActionId = pstAction->ulActionId;
			return pstAction;
		}
	}

	return NULL;
}

STATIC HERROR xmgr_pvrbase_startChasePlayback(HUINT32 ulActionId, Handle_t hCurrSvc, Handle_t hMasterSvc, ChListUiGroup_t eSvcListGroup, MgrRec_RecInfo_t *pstRecInfo, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPosOrigin, HUINT32 ulOffset, MgrPb_Option_e ePbOption)
{
	HINT32						 nStartTime;
	MgrPb_PlaySetup_t				 stPbSetup;
	XmgrPvr_Action_t				*pstAction;
	XmgrPvr_Context_t			*pstContext;
	HERROR						 hErr;

	pstContext = xmgr_pvr_GetContext();
	if ((pstContext == NULL) || (pstContext->ePvrDevState[pstRecInfo->ulPvrIdx] != ePvrDevState_OK))
	{
		HxLOG_Error("[xmgr_pvrbase_startChasePlayback] PVR Device not ready\n");
		return ERR_FAIL;
	}

	pstAction = xmgr_pvrbase_getPvrActionForPb (pstContext, ulActionId);
	if (pstAction == NULL)
	{
		HxLOG_Error("[xmgr_pvrbase_startChasePlayback] Cannot Get PVR Playback Action\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_MM_DELAYED_REC)
	// Delayed Recording 중이고, 파일 copy 가 진행중이라면 playbar 를 disable 로 표시하고, trick 을 금지한다.
	if (MGR_RECORDER_IsDuringDelayedRecCopy(pstRecInfo->hAction) == TRUE)
	{
		HxLOG_Error("[xmgr_pvrbase_startChasePlayback] Chase Playback is forbidden during delayed record copy.\n");
		return ERR_FAIL;
	}
#endif

	ePbPosOrigin &= eMgrPb_PlayType_PosMask;

	switch (ePbPosOrigin)
	{
	case eMgrPb_PlayType_EndPos:
		nStartTime = (pstRecInfo->ulRecTime > ulOffset) ? ((HINT32)pstRecInfo->ulRecTime - (HINT32)ulOffset) : 0;
		break;

	case eMgrPb_PlayType_FirstPos:
	case eMgrPb_PlayType_RecentPos:
	default:
		nStartTime = (HINT32)ulOffset;
		break;
	}

	if (nStartTime < 0)
	{
		nStartTime = 0;
	}
	else if (nStartTime >= pstRecInfo->ulRecTime - 1)
	{
		nStartTime = pstRecInfo->ulRecTime - 2;
	}

	HxSTD_MemSet(&stPbSetup, 0, sizeof(MgrPb_PlaySetup_t));
	stPbSetup.ePlayType 		= eMgrPb_PlayType_Chase | eMgrPb_PlayType_NoNeedCasCi | ePbPosOrigin;
	stPbSetup.hCurrSvc			= hCurrSvc;
	stPbSetup.hMasterSvc		= hMasterSvc;
	stPbSetup.eSvcListGroup 	= eSvcListGroup;
	stPbSetup.eTrickState		= eTrick;
	stPbSetup.ulStartTime		= (HUINT32)nStartTime;
	stPbSetup.ulContentId		= pstRecInfo->ulContentId;
	stPbSetup.bMakeThumbnail	= FALSE;
	stPbSetup.bStartPaused 		= (ePbOption & eMgrPb_Option_StartPaused) ? TRUE : FALSE;

#if defined(CONFIG_TSR_IMMEDIATELY_PAUSE)
	// For immediately pause
	SVC_AV_SetChannelChangeMode(eSvcAv_ChChangeMode_HoldUntilFirstPicture);

	// If you set flag to false then YOU MUST RESTORE FLAG to TRUE
	SVC_AV_SetShowHideControl(FALSE);
	if( ePbOption & eMgrPb_Option_UseLatestPbTime )
	{
		stPbSetup.ulStartTime = pstRecInfo->ulLastTime;
	}
#else
	// Not support immediately pause
	stPbSetup.bStartPaused = FALSE;
#endif

	hErr = MGR_ACTION_SetupPlayback(ulActionId, eViewType_CHASE_PB, stPbSetup.ePlayType, eAmPriority_VIEW_ACTION);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_SetupPlayback err(0x%08x), ulActionId(%d)\n", hErr, ulActionId);

#if defined(CONFIG_TSR_IMMEDIATELY_PAUSE)
		//Restore channel-change mode
		SVC_AV_SetDefaultChannelChangeMode();
		SVC_AV_SetShowHideControl(TRUE);
#endif

		return ERR_FAIL;
	}

	// Context에 등록한다.
	pstAction->eState			= ePvrActState_Working;
	pstAction->eType			= ePvrActType_PB_Chase;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulRelatedActId	= SVC_PIPE_GetActionId(pstRecInfo->hAction);
	pstAction->hMasterSvcHandle	= hMasterSvc;
	pstAction->hCurrSvcHandle	= hCurrSvc;
	pstAction->ulTimerId		= 0;
	pstAction->ulPvrIdx			= pstRecInfo->ulPvrIdx;
	pstAction->pfActFunc		= (BUS_Callback_t)MGR_PLAYBACK_GetProcFunc(ulActionId);

	MGR_PLAYBACK_SetPlaySetup(ulActionId, &stPbSetup);
	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	return ERR_OK;
}

STATIC HERROR xmgr_pvrbase_startTsrPlayback(HUINT32 ulActionId, Handle_t hCurrSvc, Handle_t hMasterSvc, ChListUiGroup_t eSvcListGroup, MgrRec_RecInfo_t *pstRecInfo, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPosOrigin, HUINT32 ulOffset, MgrPb_Option_e ePbOption)
{
	HINT32						 nStartTime;
	MgrPb_PlaySetup_t				 stPbSetup;
	XmgrPvr_Action_t				*pstAction;
	XmgrPvr_Context_t			*pstContext;
	HERROR						 hErr;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("[xmgr_pvrbase_startTsrPlayback] pstContext is NULL\n");
		return ERR_FAIL;
	}
	if( SVC_FS_UseTsrOnlyDevice() == FALSE && (pstContext->ePvrDevState[pstRecInfo->ulPvrIdx] != ePvrDevState_OK))
	{
		HxLOG_Error("[xmgr_pvrbase_startTsrPlayback] PVR Device not ready\n");
		return ERR_FAIL;
	}

	pstAction = xmgr_pvrbase_getPvrActionForPb (pstContext, ulActionId);
	if (pstAction == NULL)
	{
		HxLOG_Error("[xmgr_pvrbase_startTsrPlayback] Cannot Get PVR Record Action\n");
		return ERR_FAIL;
	}

	ePbPosOrigin &= eMgrPb_PlayType_PosMask;

	switch (ePbPosOrigin)
	{
	case eMgrPb_PlayType_EndPos:
		nStartTime = (pstRecInfo->ulRecTime > ulOffset) ? ((HINT32)pstRecInfo->ulEndTime - (HINT32)ulOffset) : ((HINT32)pstRecInfo->ulStartTime + 1);
		break;

	case eMgrPb_PlayType_FirstPos:
	case eMgrPb_PlayType_RecentPos:
	default:
		nStartTime = (HINT32)(pstRecInfo->ulStartTime + ulOffset);
		break;
	}

	if (nStartTime < 0)
	{
		nStartTime = 0;
	}
	else if (nStartTime >= pstRecInfo->ulEndTime - 1)
	{
		nStartTime = pstRecInfo->ulEndTime - 2;
	}

	HxSTD_MemSet(&stPbSetup, 0, sizeof(MgrPb_PlaySetup_t));
	stPbSetup.ePlayType 		= eMgrPb_PlayType_Tsr| eMgrPb_PlayType_NoNeedCasCi | ePbPosOrigin;
	stPbSetup.hCurrSvc			= hCurrSvc;
	stPbSetup.hMasterSvc		= hMasterSvc;
	stPbSetup.eSvcListGroup 	= eSvcListGroup;
	stPbSetup.eTrickState		= eTrick;
	stPbSetup.ulStartTime		= (HUINT32)nStartTime;
	stPbSetup.ulContentId		= pstRecInfo->ulContentId;
	stPbSetup.bMakeThumbnail	= FALSE;
	if( SVC_FS_UseTsrOnlyDevice() == TRUE )
	{
		stPbSetup.bIsUsbFlashRecord = xmgr_pvrbase_isTsrInUsbmemByDevIdx(SVC_FS_GetTsrOnlyDeviceIdx());
	}
	else
	{
		stPbSetup.bIsUsbFlashRecord	= xmgr_pvrbase_isTsrInUsbmem (pstRecInfo->ulPvrIdx);
	}

	// Context에 setup 직전으로 등록한다.
	pstAction->eState			= ePvrActState_SettingUp;
	pstAction->eType			= ePvrActType_PB_Tsr;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulRelatedActId	= SVC_PIPE_GetActionId(pstRecInfo->hAction);
	pstAction->hMasterSvcHandle	= hMasterSvc;
	pstAction->hCurrSvcHandle	= hCurrSvc;
	stPbSetup.bStartPaused 		= (ePbOption & eMgrPb_Option_StartPaused) ? TRUE : FALSE;

#if defined(CONFIG_TSR_IMMEDIATELY_PAUSE)
	// For immediately pause
	SVC_AV_SetChannelChangeMode(eSvcAv_ChChangeMode_HoldUntilFirstPicture);

	// If you set flag to false then YOU MUST RESTORE FLAG to TRUE
	SVC_AV_SetShowHideControl(FALSE);
	if( ePbOption & eMgrPb_Option_UseLatestPbTime )
	{
		stPbSetup.ulStartTime = pstRecInfo->ulLastTime;
	}
#else
	// Not support immediately pause
	stPbSetup.bStartPaused = FALSE;
#endif

	hErr = MGR_ACTION_SetupPlayback(ulActionId, eViewType_TSR_PB, stPbSetup.ePlayType, eAmPriority_VIEW_ACTION);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_SetupPlayback err(0x%08x), ulActionId(%d)\n", hErr, ulActionId);
		HxSTD_MemSet(pstAction, 0, sizeof(XmgrPvr_Action_t));

#if defined(CONFIG_TSR_IMMEDIATELY_PAUSE)
		//Restore channel-change mode
		SVC_AV_SetDefaultChannelChangeMode();
		SVC_AV_SetShowHideControl(TRUE);
#endif

		return ERR_FAIL;
	}

	// Context에 등록한다.
	pstAction->eState			= ePvrActState_Working;
	pstAction->eType			= ePvrActType_PB_Tsr;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulRelatedActId	= SVC_PIPE_GetActionId(pstRecInfo->hAction);
	pstAction->hMasterSvcHandle	= hMasterSvc;
	pstAction->hCurrSvcHandle	= hCurrSvc;
	pstAction->ulTimerId		= 0;
	pstAction->ulPvrIdx			= pstRecInfo->ulPvrIdx;
	pstAction->pfActFunc		= (BUS_Callback_t)MGR_PLAYBACK_GetProcFunc(ulActionId);

	MGR_PLAYBACK_SetPlaySetup(ulActionId, &stPbSetup);
	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	return ERR_OK;
}

// ContentId에 대한 PvrIdx 얻어오는 함수
STATIC HERROR xmgr_pvrbase_getPvrIdxbyContentId(HUINT32 ulContentId, HUINT32 *pulPvrIdx)
{
	SvcMeta_Record_t	 stRecInfo;
	HCHAR		 szPath[SVC_FS_PVR_DIR_PATH_LEN];
	HERROR		 hErr;
	HUINT32		i;

	HxSTD_MemSet(szPath, 0, SVC_FS_PVR_DIR_PATH_LEN);

	hErr = SVC_META_GetRecordInfo(ulContentId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[xmgr_pvrbase_getPvrIdxbyContentId] SVC_META_GetRecordInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	for (i=0 ; i<MAX_CONNECTABLE_PVR_STORAGE_NUM ; i++)
	{
		hErr = SVC_FS_GetPvrPath (i, eSVC_FS_UsageType_Record, szPath);
		if (ERR_OK != hErr)
		{
			continue;
		}

		// ContentId로 얻은 path와 비교해서 같은지 확인
		if(HxSTD_StrCmp(stRecInfo.aucPath, szPath) == 0)
		{
			*pulPvrIdx = i;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}

STATIC HERROR xmgr_pvrbase_getPlaybackIdxbyContentId (HUINT32 ulContentId, HUINT32 *pulPlaybackIdx)
{
	SvcMeta_Record_t	 stRecInfo;
	HCHAR		 szPath[SVC_FS_PVR_DIR_PATH_LEN];
	HERROR		 hErr;
	HUINT32		i;

	HxSTD_MemSet(szPath, 0, SVC_FS_PVR_DIR_PATH_LEN);

	hErr = SVC_META_GetRecordInfo(ulContentId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[xmgr_pvrbase_getPvrIdxbyContentId] SVC_META_GetRecordInfo err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	for (i=0 ; i<MAX_CONNECTABLE_PLAYBACK_STORAGE_NUM ; i++)
	{
		hErr = SVC_FS_GetPlaybackPath (i, eSVC_FS_UsageType_Record, szPath);
		if (ERR_OK != hErr)
		{
			continue;
		}

		// ContentId로 얻은 path와 비교해서 같은지 확인
		if(HxSTD_StrCmp(stRecInfo.aucPath, szPath) == 0)
		{
			if (pulPlaybackIdx != NULL)
			{
				*pulPlaybackIdx = i;
			}
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}

STATIC HERROR xmgr_pvrbase_startNormalPlayback(HUINT32 ulActionId, HUINT32 ulContentId, HUINT32 ulStartTime, HBOOL bMakeThumbnail, Pb_TrRecMode_t eTrRecMode)
{
	MgrPb_PlaySetup_t				 stPbSetup;
	XmgrPvr_Action_t				*pstAction;
	XmgrPvr_Context_t			*pstContext;
	HERROR						 hErr;
	HUINT32 					ulPvrIdx = 0xffff;
	SvcMeta_Record_t	 			stRecInfo;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("Null context. actionId=(%d), contentId=(%d) \n",ulActionId, ulContentId);
		return ERR_FAIL;
	}

	// ContentId로 PvrIdx 얻어오기
	hErr = xmgr_pvrbase_getPvrIdxbyContentId(ulContentId, &ulPvrIdx);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug(" Can't get PvrIdx\n");
		hErr = xmgr_pvrbase_getPlaybackIdxbyContentId(ulContentId, NULL);
		if (hErr != ERR_OK)
		{
			HxLOG_Error(" Can't get recording storage \n");
			return ERR_FAIL;
		}

		HxLOG_Debug("Got recording storage \n");
	}

	if (ulPvrIdx != 0xffff)
	{
		// got pvridx -> pvr
		if (pstContext->ePvrDevState[ulPvrIdx] != ePvrDevState_OK)
		{
			HxLOG_Error("[xmgr_pvrbase_startNormalPlayback] PVR Device not ready\n");
			return ERR_FAIL;
		}
	}
	else
	{
		// rec storage
		HxLOG_Debug("allow playback on recording storage. \n");
	}

	pstAction = xmgr_pvrbase_getPvrActionForPb (pstContext, ulActionId);
	if (pstAction == NULL)
	{
		HxLOG_Error("[xmgr_pvrbase_startNormalPlayback] Cannot Get PVR Playback Action\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stPbSetup, 0, sizeof(MgrPb_PlaySetup_t));

	if(ulStartTime == 0)
	{
		stPbSetup.ePlayType = eMgrPb_PlayType_Normal | eMgrPb_PlayType_NoNeedCasCi | eMgrPb_PlayType_FirstPos;
	}
	else
	{
		stPbSetup.ePlayType = eMgrPb_PlayType_Normal | eMgrPb_PlayType_NoNeedCasCi | eMgrPb_PlayType_RecentPos;
	}

	// For transcoder
	if(ePbTrRecMode_SectionGathering == eTrRecMode)
	{
		stPbSetup.ePlayType |= eMgrPb_PlayType_Transcoder_SectionGathering;
	}
	else if(ePbTrRecMode_TranscodeRecording == eTrRecMode)
	{
		stPbSetup.ePlayType |= eMgrPb_PlayType_Transcoder;
	}

	stPbSetup.hCurrSvc = HANDLE_NULL;
	stPbSetup.hMasterSvc = HANDLE_NULL;
	stPbSetup.eSvcListGroup = 0;
	stPbSetup.eTrickState = eMgrPb_TickState_PLAY;
	stPbSetup.ulStartTime = ulStartTime;
	stPbSetup.ulContentId = ulContentId;
	stPbSetup.bMakeThumbnail = bMakeThumbnail;
	stPbSetup.bShowMediaList = TRUE;

	hErr = MGR_ACTION_SetupPlayback(ulActionId, eViewType_PB, stPbSetup.ePlayType, eAmPriority_VIEW_ACTION);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_SetupPlayback err(0x%08x)\n", hErr);
		return hErr;
	}

	// Context에 등록한다.
	pstAction->eState			= ePvrActState_Working;
	pstAction->eType			= ePvrActType_PB_Normal;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulRelatedActId	= HANDLE_NULL;
	pstAction->hMasterSvcHandle	= HANDLE_NULL;
	pstAction->hCurrSvcHandle	= HANDLE_NULL;
	pstAction->ulTimerId		= 0;
	pstAction->ulPvrIdx			= ulPvrIdx;
	pstAction->pfActFunc		= (BUS_Callback_t)MGR_PLAYBACK_GetProcFunc(ulActionId);

	// 재생될 동안 ContentList 에 유지하도록 Skip Flag 를 TRUE 로 한다.
	// VideoList 를 Show 시 기존 ContentList 를 SVC_PVR_UnloadContentsInfo 로 해제하는데, Skip Flag 를 TRUE 로 하면 해재되지 않고 유지된다. FALSE 이면 해제
	hErr = SVC_META_SetAsDeleteSkipContent(ulContentId, TRUE);
	if (hErr)
		HxLOG_Error("SVC_META_SetAsDeleteSkipContent ret:0x%X\n",hErr);

	hErr = MGR_PLAYBACK_SetPlaySetup (ulActionId, &stPbSetup);
	if (hErr)
		HxLOG_Error("xmgt_pvr_SetPlaySetup ret:0x%X\n",hErr);

	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	HxSTD_MemSet(&stRecInfo, 0, sizeof(SvcMeta_Record_t));
	hErr = SVC_META_GetRecordInfo(ulContentId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] xmgr_pvrbase_startNormalPlayback() error, hErr:0x%X, ulContentId:0x%X\n"
			, __FUNCTION__, __LINE__, hErr, ulContentId);
		return ERR_OK;
	}
	HLIB_CMD_CaptureLog("PB-Content,File Name=%s",stRecInfo.aucFileName);

	return ERR_OK;
}

STATIC HERROR xmgr_pvrbase_startLivestreamPlayback(HUINT32 ulActionId, HUINT32 ulContentId, HUINT32 ulStartTime, HBOOL bMakeThumbnail)
{
	MgrPb_PlaySetup_t				 stPbSetup;
	XmgrPvr_Action_t				*pstAction;
	XmgrPvr_Context_t			*pstContext;
	MgrAction_Priority_e		 eActPriority = eAmPriority_RECORD;
	DxLiveStreamPriority_e		 eLsPriority = eDxLsPriority_EQUAL_REC;
	HERROR						 hErr;
	HUINT32 					 ulPvrIdx;


	// ContentId로 PvrIdx 얻어오기
	hErr = xmgr_pvrbase_getPvrIdxbyContentId(ulContentId, &ulPvrIdx);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Can't get PvrIdx, ulContentId(%d)\n", ulContentId);
		return ERR_FAIL;
	}

	pstContext = xmgr_pvr_GetContext();
	if ((pstContext == NULL) || (pstContext->ePvrDevState[ulPvrIdx] != ePvrDevState_OK))
	{
		HxLOG_Error("PVR Device not ready\n");
		return ERR_FAIL;
	}

	pstAction = xmgr_pvrbase_getPvrActionForPb (pstContext, ulActionId);
	if (NULL == pstAction)
	{
		HxLOG_Error("[Cannot Get PVR Playback Action\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stPbSetup, 0, sizeof(MgrPb_PlaySetup_t));

	if(ulStartTime == 0)
	{
		stPbSetup.ePlayType = eMgrPb_PlayType_Normal | eMgrPb_PlayType_NoNeedCasCi | eMgrPb_PlayType_FirstPos;
	}
	else
	{
		stPbSetup.ePlayType = eMgrPb_PlayType_Normal | eMgrPb_PlayType_NoNeedCasCi | eMgrPb_PlayType_RecentPos;
	}

	// For transcoder
	stPbSetup.ePlayType |= eMgrPb_PlayType_Transcoder;

	stPbSetup.hCurrSvc = HANDLE_NULL;
	stPbSetup.hMasterSvc = HANDLE_NULL;
	stPbSetup.eSvcListGroup = 0;
	stPbSetup.eTrickState = eMgrPb_TickState_PLAY;
	stPbSetup.ulStartTime = ulStartTime;
	stPbSetup.ulContentId = ulContentId;
	stPbSetup.bMakeThumbnail = bMakeThumbnail;
	stPbSetup.bShowMediaList = TRUE;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, (HUINT32 *)&eLsPriority, 0);
	if (ERR_OK == hErr)
	{
		switch (eLsPriority)
		{
		case eDxLsPriority_EQUAL_REC:
			eActPriority = eAmPriority_RECORD;
			break;

		case eDxLsPriority_EQUAL_VIEW:
			eActPriority = eAmPriority_VIEW_ACTION;
			break;

		case eDxLsPriority_BELOW_VIEW:
			eActPriority = (eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		case eDxLsPriority_UPPER_REC:
			eActPriority = (eAmPriority_RECORD + (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		default:
			break;
		}
	}

	hErr = MGR_ACTION_SetupLiveStream (ulActionId, HANDLE_NULL, eActPriority, eLiveStreamMode_ViaPvrPb);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_SetupPlayback err(0x%08x)\n", hErr);
		return hErr;
	}

	// Context에 등록한다.
	pstAction->eState			= ePvrActState_Working;
	pstAction->eType			= ePvrActType_PB_Normal;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulRelatedActId	= HANDLE_NULL;
	pstAction->hMasterSvcHandle	= HANDLE_NULL;
	pstAction->hCurrSvcHandle	= HANDLE_NULL;
	pstAction->ulTimerId		= 0;
	pstAction->ulPvrIdx			= ulPvrIdx;
	pstAction->pfActFunc		= (BUS_Callback_t)MGR_PLAYBACK_GetProcFunc(ulActionId);

	// 재생될 동안 ContentList 에 유지하도록 Skip Flag 를 TRUE 로 한다.
	// VideoList 를 Show 시 기존 ContentList 를 SVC_PVR_UnloadContentsInfo 로 해제하는데, Skip Flag 를 TRUE 로 하면 해재되지 않고 유지된다. FALSE 이면 해제
	hErr = SVC_META_SetAsDeleteSkipContent(ulContentId, TRUE);
	if (hErr)
		HxLOG_Error("SVC_META_SetAsDeleteSkipContent ret:0x%X\n",hErr);

	hErr = MGR_PLAYBACK_SetPlaySetup (ulActionId, &stPbSetup);
	if (hErr)
		HxLOG_Error("xmgt_pvr_SetPlaySetup ret:0x%X\n",hErr);

	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	return ERR_OK;
}


#define _____RECORD_CONFLICT_CHECK_____

STATIC XmgrPvr_Action_t *xmgr_pvrbase_checkRecordPossible(MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason)
{
	HUINT32					 ulActionId = 0xFFFF;
	HUINT32					 ulFailReason = eMgrRecFailed_UNKNOWN;
	SvcFs_DevIdx_e			 ePvrDevIdx;
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t		*pstRecAction = NULL;
	HERROR					 hErr;

	pstContext = xmgr_pvr_GetContext();
	if (NULL == pstContext)
	{
		HxLOG_Error("pstContext is NULL\n");
		ulFailReason = eMgrRecFailed_UNKNOWN;
		goto END_FUNC;
	}

	if (eMgrRecOutput_HDD == pstRecSetup->eRecDst)
	{
		if(pstRecSetup->ulPvrIdx >= MAX_CONNECTABLE_PVR_STORAGE_NUM)
		{
			HxLOG_Error("ulPvrIdx is not available.\n");
			ulFailReason = eMgrRecFailed_NOUSBHDD;
			goto END_FUNC;
		}

		// PVR device가 문제 없을 경우에만 동작한다.
		if (ePvrDevState_OK != pstContext->ePvrDevState[pstRecSetup->ulPvrIdx])
		{
			HxLOG_Error("PVR Device is not ready.\n");
			ulFailReason = eMgrRecFailed_NOHDD;
			goto END_FUNC;
		}

		hErr = SVC_FS_GetPvrDevIdPartIdx (pstRecSetup->ulPvrIdx, &ePvrDevIdx, NULL);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("FS_PVR_FindPvrDevIdPartIdx failed.\n");
			ulFailReason = eMgrRecFailed_NOHDD;
			goto END_FUNC;
		}

		if (TRUE != SVC_FS_IsDevicePvrPossible(ePvrDevIdx))
		{
			HxLOG_Error("SVC_FS_IsDevicePvrPossible failed.\n");
			ulFailReason = eMgrRecFailed_HDDFULL;
			goto END_FUNC;
		}
		
#if defined(CONFIG_OP_UK_DTT)		
		// check Recording Maxium number. 
		{
			HUINT32 		*pulContentId = NULL;
			HUINT32 		ulContentNum = 0; 

			if(pstRecSetup->eRecType != eMgrRecType_TSR)
			{
				(void)SVC_META_GetContentsIdList(&ulContentNum, &pulContentId);
		
				if(ulContentNum >=MAX_RECORD_LIST)
				{					
					ulFailReason = eMgrRecFailed_MAXRECORDLISTNUM;
					SVC_META_FreeContentsIdList(pulContentId);
					goto END_FUNC;
				}
				
				SVC_META_FreeContentsIdList(pulContentId);
			}			
		}
#endif
	}

	// Record용 Action Item을 잡는다.
	pstRecAction = xmgr_pvrbase_getPvrActionForRec(pstContext, &ulActionId);
	if ((NULL == pstRecAction) || (ulActionId > NUM_MAX_ACTION))
	{
		HxLOG_Error("Cannot Get PVR Record Action\n", pstRecSetup->hMasterSvc, pstRecSetup->hCurrSvc);

		pstRecAction = NULL;
		ulFailReason = eMgrRecFailed_MAXNUM;
		goto END_FUNC;
	}

	ulFailReason = eMgrRecFailed_NOFAIL;

END_FUNC:
	*pulActionId	= ulActionId;
	*pulFailReason	= ulFailReason;

	return pstRecAction;
}

#define _____RECORD_START_FUNCTIONS_____
STATIC MgrRec_EndReasonType_e xmgr_pvrbase_setupRecording (MgrAction_Type_e eActType, MgrAction_Priority_e ePriority, MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, XmgrPvr_Action_t **ppstRecAction)
{
	HUINT32				 ulActionId = HANDLE_NULL, ulFailReason = eMgrRecFailed_UNKNOWN;
	XmgrPvr_Action_t	*pstAction;
	HERROR				 hErr;

	*ppstRecAction = NULL;

	pstAction = xmgr_pvrbase_checkRecordPossible(pstRecSetup, &ulActionId, &ulFailReason);
	if ((NULL == pstAction) || (ulActionId >= NUM_MAX_ACTION))
	{
		HxLOG_Error("Record Impossible. failReason=(%d) \n", ulFailReason);
		return ulFailReason;
	}

	hErr = MGR_ACTION_SetupRecord(ulActionId, eActType, pstRecSetup->hCurrSvc, pstRecSetup->eRecType, pstRecSetup->ulPvrIdx, ePriority);
	ulFailReason = (MgrRec_EndReasonType_e)hErr;
	if (ulFailReason != eMgrRecFailed_NOFAIL)
	{
		HxLOG_Error("can't take MGR_ACTION_SetupRecord() Handle_t=%x ulFailReason=%d\n", pstRecSetup->hCurrSvc, hErr);
		return ulFailReason;
	}

	*ppstRecAction	= pstAction;
	*pulActionId	= ulActionId;

	// Context에 등록한다.
	pstAction->eState			= ePvrActState_Working;
	pstAction->ulActionId		= ulActionId;
	pstAction->ulRelatedActId	= HANDLE_NULL;
	pstAction->hMasterSvcHandle	= pstRecSetup->hMasterSvc;
	pstAction->hCurrSvcHandle	= pstRecSetup->hCurrSvc;
	pstAction->ulTimerId		= 0;
	pstAction->ulPvrIdx			= pstRecSetup->ulPvrIdx;
	pstAction->pfActFunc		= (BUS_Callback_t)MGR_RECORDER_GetProcFunc(ulActionId);

	switch (pstRecSetup->eRecType)
	{
	case eMgrRecType_EBR:
	case eMgrRecType_SBR:
		pstAction->eType			= ePvrActType_REC_EvtReserved;
		break;

	case eMgrRecType_DELAYED:
		pstAction->eType			= ePvrActType_REC_Delayed;
		break;

	case eMgrRecType_INSTANT:
	case eMgrRecType_TBR:
	case eMgrRecType_FULLTSREC:
	default:
		pstAction->eType			= ePvrActType_REC_Normal;
		break;
	}

	MGR_RECORDER_SetRecSetup (ulActionId - eActionId_REC_FIRST, pstRecSetup);

	return eMgrRecFailed_NOFAIL;
}

STATIC MgrRec_EndReasonType_e xmgr_PVR_BASE_StartHddRecording(MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId)
{
	HUINT32					 ulActionId = HANDLE_NULL;
	MgrRec_RecSetup_t		 stRecSetup;
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t		*pstAction = NULL, *pstTsrAction = NULL;
	MgrRec_EndReasonType_e	eFailReason = eMgrRecFailed_UNKNOWN;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return eMgrRecFailed_UNKNOWN;
	}

	pstRecSetup->ulPvrIdx = xmgr_pvrbase_getAvailablePvrIdx(pstRecSetup->hCurrSvc, pstRecSetup->eRecType, pstRecSetup->ulSlot);
	if (pstRecSetup->ulPvrIdx >= MAX_CONNECTABLE_PVR_STORAGE_NUM)
	{
		HxLOG_Error("pstRecSetup->ulPvrIdx: 0x%x, pstRecSetup->ulPvrIdx: %d \n", pstRecSetup->ulPvrIdx, pstRecSetup->ulPvrIdx);

		return eMgrRecFailed_NOHDD;
	}

	HxSTD_memcpy(&stRecSetup, pstRecSetup, sizeof(MgrRec_RecSetup_t));
	stRecSetup.eRecDst = eMgrRecOutput_HDD;

	eFailReason = xmgr_pvrbase_setupRecording(eRecType_REC, eAmPriority_RECORD, &stRecSetup, &ulActionId, &pstAction);
	if (eMgrRecFailed_NOFAIL != eFailReason)
	{
		HxLOG_Error("xmgr_pvrbase_setupRecording err: failReason=(%d) \n",eFailReason);
		return eFailReason;
	}

	// 현재 TSR 중인 채널이 아닌지 확인한다. 현재 TSR 중이면 그 TSR은 중지 시킨다.
	pstTsrAction = xmgr_pvr_FindPvrActionByServiceHandle(ePvrActType_TSR_Normal, stRecSetup.eSessionType, stRecSetup.hMasterSvc, stRecSetup.hCurrSvc);
	if (pstTsrAction != NULL)
	{
		xmgr_pvr_DestroyPvrAction(pstTsrAction);
	}

	BUS_MGR_Create(MGR_RECORDER_GetProcName(ulActionId), APP_PVR_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	if (NULL != pulActionId)
	{
		*pulActionId = ulActionId;
	}

	return eMgrRecFailed_NOFAIL;
}

STATIC MgrRec_EndReasonType_e xmgr_pvrbase_startExtRecording(MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId)
{
	HUINT32					 ulActionId = HANDLE_NULL;
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t		*pstAction = NULL, *pstTsrAction = NULL;
	MgrRec_EndReasonType_e	eFailReason = eMgrRecFailed_UNKNOWN;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return eMgrRecFailed_UNKNOWN;
	}

	pstRecSetup->eRecDst = eMgrRecOutput_EXT_TSLINE;

	// PVR device가 문제 없을 경우에만 동작한다.
	if (pstContext->ePvrDevState[pstRecSetup->ulPvrIdx]!= ePvrDevState_OK)
	{
		HxLOG_Error("PVR Device is not ready.\n");
		return eMgrRecFailed_NOHDD;
	}

	// 현재 동일한 서비스를 Recording 중이면 Recording을 막는다.
	pstAction = xmgr_pvr_FindPvrActionByServiceHandle(ePvrActType_REC, pstRecSetup->eSessionType, pstRecSetup->hMasterSvc, pstRecSetup->hCurrSvc);
	if (pstAction != NULL)
	{
		HxLOG_Error("Already has the other REC hMasterSvc=%x hSuppleSvc=%x\n", pstRecSetup->hMasterSvc, pstRecSetup->hCurrSvc);
		return eMgrRecFailed_SAME_SVC_RECORDING;
	}

	eFailReason = xmgr_pvrbase_setupRecording(eRecType_REC, eAmPriority_RECORD, pstRecSetup, &ulActionId, &pstAction);
	if (eMgrRecFailed_NOFAIL != eFailReason)
	{
		HxLOG_Error("xmgr_pvrbase_setupRecording err:\n");
		return eFailReason;
	}

	// 현재 TSR 중인 채널이 아닌지 확인한다. 현재 TSR 중이면 그 TSR은 중지 시킨다.
	pstTsrAction = xmgr_pvr_FindPvrActionByServiceHandle(ePvrActType_TSR_Normal, pstRecSetup->eSessionType, pstRecSetup->hMasterSvc, pstRecSetup->hCurrSvc);
	if (pstTsrAction != NULL)
	{
		xmgr_pvr_DestroyPvrAction(pstTsrAction);
	}

	pstAction->eType			= ePvrActType_REC_ExtTsLine;
	BUS_MGR_Create(MGR_RECORDER_GetProcName(ulActionId), APP_PVR_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	if (NULL != pulActionId)
	{
		*pulActionId = ulActionId;
	}

	return eMgrRecFailed_NOFAIL;
}

STATIC MgrRec_EndReasonType_e xmgr_PVR_BASE_StartMembufRecording(MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId)
{
	HUINT32					 ulActionId = HANDLE_NULL;
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t		*pstAction = NULL, *pstTsrAction = NULL;
	MgrRec_EndReasonType_e	eFailReason = eMgrRecFailed_UNKNOWN;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return eMgrRecFailed_UNKNOWN;
	}

	pstRecSetup->eRecDst = eMgrRecOutput_MEM_RINGBUF;

	eFailReason = xmgr_pvrbase_setupRecording(eRecType_REC, eAmPriority_RECORD, pstRecSetup, &ulActionId, &pstAction);
	if (eMgrRecFailed_NOFAIL != eFailReason)
	{
		HxLOG_Error("xmgr_pvrbase_setupRecording err:\n");
		return eFailReason;
	}

	// 현재 TSR 중인 채널이 아닌지 확인한다. 현재 TSR 중이면 그 TSR은 중지 시킨다.
	pstTsrAction = xmgr_pvr_FindPvrActionByServiceHandle(ePvrActType_TSR_Normal, pstRecSetup->eSessionType, pstRecSetup->hMasterSvc, pstRecSetup->hCurrSvc);
	if (pstTsrAction != NULL)
	{
		xmgr_pvr_DestroyPvrAction(pstTsrAction);
	}

	pstAction->eType			= ePvrActType_REC_MemBuffer;
	BUS_MGR_Create(MGR_RECORDER_GetProcName(ulActionId), APP_PVR_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);

	if (NULL != pulActionId)
	{
		*pulActionId = ulActionId;
	}

	return eMgrRecFailed_NOFAIL;
}


#define _____INTERNAL_MSG_FUNCTIONS_____
STATIC BUS_Result_t xmgr_pvrbase_msgBusCreate(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SvcFs_DevIdx_e		 ePvrDevIdx = eSVC_FS_DEV_NULL;
	HUINT32				 ulPvrPartIdx;
	SvcFs_DevInfo_t		 stDevInfo;
	XmgrPvr_Context_t	*pstContext;
	HERROR				 hErr;
	HUINT32				ulPvrIdx;
	HUINT32				i;

	if( SVC_FS_UseTsrOnlyDevice() == FALSE )
	{
		ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
		if(0xffff == ulPvrIdx || MAX_CONNECTABLE_PVR_STORAGE_NUM <= ulPvrIdx)
		{
			return MESSAGE_BREAK;
		}
	}

	pstContext = xmgr_pvr_GetContext();
	if (pstContext != NULL)
	{
		for (i=0; i<MAX_CONNECTABLE_PVR_STORAGE_NUM; i++)
		{
			// PVR Manager가 뜨기 전에 이미 Internal HDD 등이 세팅되었을 수도 있다.
			hErr = SVC_FS_GetPvrDevIdPartIdx (i, &ePvrDevIdx, &ulPvrPartIdx);
			if (hErr == ERR_OK && ePvrDevIdx != eSVC_FS_DEV_NULL)
			{
				hErr = SVC_FS_GetDeviceInfo(ePvrDevIdx, &stDevInfo);
				if (hErr == ERR_OK && stDevInfo.eDevStatus == eDevStatus_OK)
				{
					pstContext->ePvrDevState[i]	= ePvrDevState_OK;
					pstContext->ePvrDevIdx	[i]	= ePvrDevIdx;
					pstContext->ulPvrPartIdx[i]	= ulPvrPartIdx;
				}
			}
		}
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_pvrbase_msgBusDestroy(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_pvrbase_msgBusTimer(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulTimerId = (HUINT32)p1;
	HERROR			 hErr;
	HUINT32			ulPvrIdx = 0;

	// if SVC_FS_UseTsrOnlyDevice() is TRUE then ulPvrIdx is not used
	if(SVC_FS_UseTsrOnlyDevice() == FALSE)
	{
		ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
		if(0xffff == ulPvrIdx || MAX_CONNECTABLE_PVR_STORAGE_NUM <= ulPvrIdx)
		{
			return MESSAGE_BREAK;
		}
	}

	if ((ulTimerId & PVR_MGR_DELAY_TSRREC_TIMERID_MASK) == PVR_MGR_DELAY_TSRREC_TIMERID)
	{
		XmgrPvr_Action_t		*pstAction = xmgr_pvr_FindPvrActionByTimerId(ePvrActType_TSR_Normal, ulTimerId);

		if ((NULL != pstAction) && (ePvrActState_WaitingByTimer == pstAction->eState))
		{
			hErr = MGR_ACTION_SetupRecord(pstAction->ulActionId, eRecType_TSR_REC, pstAction->hCurrSvcHandle, eMgrRecType_TSR, ulPvrIdx, eAmPriority_TSR);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("MGR_ACTION_SetupRecord err(0x%08x), hCurrSvc(0x%08x)\n", hErr, pstAction->hCurrSvcHandle);
				return ERR_FAIL;
			}

			if (ePvrActState_WaitingByTimer == pstAction->eState)	// 그 사이에 cancel되는 경우 있어서 한번 더 체크!
			{
				pstAction->eState			= ePvrActState_Working;
				pstAction->ulTimerId		= 0;
				BUS_MGR_Create(NULL, APP_PVR_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(pstAction->ulActionId), 0, 0, 0);
			}
			else
			{
				// action이 cancel되서 없어진 상황.. 아무것도 하지 않는다.
			}
		}

		BUS_KillTimer(ulTimerId);
		return MESSAGE_BREAK;
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_pvrbase_msgAppPlaybackStopPb (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId;
	XmgrPvr_Action_t		*pstAction;
	XmgrPvr_Context_t	*pstContext;

	ulActionId = (hAction == HANDLE_NULL) ? MGR_ACTION_GetMainActionId() : SVC_PIPE_GetActionId(hAction);

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_PB, ulActionId);
	if (pstAction == NULL)
	{
		HxLOG_Error("pstAction is NULL\n");
		return MESSAGE_PASS;
	}

	// Action이 TSR PB 나 Chase PB면 Destroy
	if ((pstAction->eType == ePvrActType_PB_Normal) ||
		(pstAction->eType == ePvrActType_PB_Tsr) ||
		(pstAction->eType == ePvrActType_PB_Chase))
	{
#if defined(CONFIG_TSR_IMMEDIATELY_PAUSE)
		//Restore channel-change mode
		SVC_AV_SetDefaultChannelChangeMode();
		SVC_AV_SetShowHideControl(TRUE);
#endif

		xmgr_pvr_DestroyPvrAction(pstAction);
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_msgAppRecordStartRec(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrRec_RecSetup_t		*pstRecSetup = (MgrRec_RecSetup_t *)p1;
	HUINT32					*pulActionId = (HUINT32 *)p2;
	MgrRec_EndReasonType_e	*peFailReason = (MgrRec_EndReasonType_e *)p3;

	switch (pstRecSetup->eRecDst)
	{
	case eMgrRecOutput_EXT_TSLINE:
		*peFailReason = xmgr_pvrbase_startExtRecording(pstRecSetup, pulActionId);
		break;

	case eMgrRecOutput_MEM_RINGBUF:
		*peFailReason = xmgr_PVR_BASE_StartMembufRecording(pstRecSetup, pulActionId);
		break;

	case eMgrRecOutput_HDD:
	default:
		*peFailReason = xmgr_PVR_BASE_StartHddRecording(pstRecSetup, pulActionId);
		break;

	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_msgAppRecordStopRec(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	XmgrPvr_Context_t		*pstContext;
	XmgrPvr_Action_t			*pstAction;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	// Action ID에 맞는 Record Action이 현재 존재하는가?
	pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_REC, ulActionId);
	if (pstAction == NULL)
	{
		HxLOG_Error("[xmgr_pvrbase_msgAppRecordStopRec] No Record Action in ActionID(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}
#if defined(CONFIG_MW_MM_DELAYED_REC)
	if(pstAction->eType == ePvrActType_REC_Delayed)
	{
		if(pstAction->eCopyStatus != ePvrCopyStatus_Complete)
		{
			pstAction->eCopyStatus = ePvrCopyStatus_WaitingForStop;
			return MESSAGE_PASS;
		}
	}
#endif
	xmgr_pvr_DestroyPvrAction(pstAction);

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_msgAppTsrStartTsr(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulTsrActionId, ulRelatedActionId;
	HUINT32					 ulPvrIdx = 0;
	MgrAction_Type_e		 eRelatedActType;
	XmgrPvr_TsrRequest_t	*pstTsrReq = (XmgrPvr_TsrRequest_t *)p3;
	XmgrPvr_Action_t		*pstAction;
	MgrRec_RecSetup_t		 stRecSetup;
	HERROR					 hErr;

	pstAction = xmgr_pvr_FindPvrActionByServiceHandle(pstTsrReq->eTsrActionType, (MgrLiveView_SessionType_t)pstTsrReq->eSessionType, pstTsrReq->hMasterSvc, pstTsrReq->hSuppleSvc);
	if (NULL != pstAction)
	{
		HxLOG_Message("Already the same service TSR running...\n");

		pstAction->nRefCount ++;
		pstTsrReq->hResult = ERR_OK;
		return MESSAGE_PASS;
	}

	// if SVC_FS_UseTsrOnlyDevice() is TRUE then ulPvrIdx is not used
	if( SVC_FS_UseTsrOnlyDevice() == FALSE )
	{
		ulPvrIdx = xmgr_pvrbase_getAvailablePvrIdx(pstTsrReq->hSuppleSvc, eMgrRecType_TSR, (HUINT32)-1);
		if ((0xffff == ulPvrIdx) || (MAX_CONNECTABLE_PVR_STORAGE_NUM <= ulPvrIdx))
		{
			HxLOG_Error("ulPvrIdx: %d \n", ulPvrIdx);
			return MESSAGE_PASS;
		}
	}

	pstAction = xmgr_pvr_GetEmptyPvrAction(pstTsrReq->eTsrActionType);
	if (NULL == pstAction)
	{
		HxLOG_Error("No Empty PVR Action...\n");
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetEmptyRecActionId(&ulTsrActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("No Empty Record Action Id...\n");
		return MESSAGE_PASS;
	}

	if (pstTsrReq->ulRelatedViewId < NUM_VIEW_ACTION)
	{
		hErr = MGR_ACTION_GetActionIdByViewId(pstTsrReq->ulRelatedViewId, &ulRelatedActionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("No related action ID in the viewID(%d)\n", pstTsrReq->ulRelatedViewId);
			return MESSAGE_PASS;
		}

		eRelatedActType = MGR_ACTION_GetType(ulRelatedActionId);
	}
	else
	{
		ulRelatedActionId = HANDLE_NULL;
		eRelatedActType = eActionType_NONE;
	}


	HxSTD_MemSet (&stRecSetup, 0, sizeof(stRecSetup));

	stRecSetup.eRecDst = eMgrRecOutput_HDD;
	stRecSetup.eRecType = eMgrRecType_TSR;
	stRecSetup.eSessionType = pstTsrReq->eSessionType;
	stRecSetup.ulLiveActionId = ulRelatedActionId;
	stRecSetup.ulSlot = DxRSV_SLOT_NULL;
	stRecSetup.ulSourceContentId = 0xffffffff;
	stRecSetup.ulPvrIdx = ulPvrIdx;
	stRecSetup.stUsbFlash.bIsUsbFlashRecord = xmgr_pvrbase_isTsrInUsbmem(ulPvrIdx);

	switch((MgrLiveView_SessionType_t)pstTsrReq->eSessionType)
	{
	case eLiveViewSession_NORMAL:
		stRecSetup.hCurrSvc = pstTsrReq->hMasterSvc;
		stRecSetup.hMasterSvc = pstTsrReq->hMasterSvc;
		break;
	case eLiveViewSession_MHEG_QUIET:	// TODO: /**< MHEG, MHP등 data 방송에서 요청한 silent tuning , 확인 필요 */
		stRecSetup.hCurrSvc = pstTsrReq->hSuppleSvc;
		stRecSetup.hMasterSvc = pstTsrReq->hMasterSvc;
		break;
	case eLiveViewSession_REPLACE:		// TODO:	/**< Free CA mode/running status change에 의한 replacement service tuning , 확인 필요 */
		stRecSetup.hCurrSvc = pstTsrReq->hSuppleSvc;
		stRecSetup.hMasterSvc = pstTsrReq->hMasterSvc;
		break;
	case eLiveViewSession_SUBFEED:		/**< Subfeed */
		stRecSetup.hCurrSvc = pstTsrReq->hSuppleSvc;
		stRecSetup.hMasterSvc = pstTsrReq->hMasterSvc;
		break;
	default:
		HxLOG_Error("SessionType(%d) is Something wrong.\n", pstTsrReq->eSessionType);
		return MESSAGE_PASS;
	}

	if (pstTsrReq->ulStartTime > 0)
	{
		// Context에 등록한다.
		pstAction->eState			= ePvrActState_WaitingByTimer;
		pstAction->eType			= pstTsrReq->eTsrActionType;
		pstAction->ulActionId		= ulTsrActionId;
		pstAction->ulRelatedActId	= ulRelatedActionId;
		pstAction->ulRelatedViewId	= pstTsrReq->ulRelatedViewId;
		pstAction->eRelatedActType	= eRelatedActType;
		pstAction->hMasterSvcHandle = stRecSetup.hMasterSvc;
		pstAction->hCurrSvcHandle	= stRecSetup.hCurrSvc;
		pstAction->ulTimerId		= PVR_MGR_DELAY_TSRREC_TIMERID + ulTsrActionId;
		pstAction->ulPvrIdx 		= ulPvrIdx;
		pstAction->pfActFunc		= (BUS_Callback_t)MGR_RECORDER_GetProcFunc(ulTsrActionId);

		// TSR_REC 시작
		MGR_RECORDER_SetRecSetup(ulTsrActionId - eActionId_REC_FIRST, &stRecSetup);
		BUS_SetTimer (pstAction->ulTimerId, pstTsrReq->ulStartTime);
	}
	else
	{
		hErr = MGR_ACTION_SetupRecord(ulTsrActionId, eRecType_TSR_REC, stRecSetup.hCurrSvc, stRecSetup.eRecType, ulPvrIdx, eAmPriority_TSR);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_ACTION_SetupRecord err(0x%08x), hCurrSvc(0x%08x)\n", hErr, stRecSetup.hCurrSvc);
			return MESSAGE_PASS;
		}

		// Context에 등록한다.
		pstAction->eState			= ePvrActState_Working;
		pstAction->eType			= pstTsrReq->eTsrActionType;
		pstAction->ulActionId		= ulTsrActionId;
		pstAction->ulRelatedActId	= ulRelatedActionId;
		pstAction->ulRelatedViewId	= pstTsrReq->ulRelatedViewId;
		pstAction->eRelatedActType	= eRelatedActType;
		pstAction->hMasterSvcHandle = stRecSetup.hMasterSvc;
		pstAction->hCurrSvcHandle	= stRecSetup.hCurrSvc;
		pstAction->ulTimerId		= 0;
		pstAction->ulPvrIdx 		= ulPvrIdx;
		pstAction->pfActFunc		= (BUS_Callback_t)MGR_RECORDER_GetProcFunc(ulTsrActionId);

		// TSR_REC 시작
		MGR_RECORDER_SetRecSetup (ulTsrActionId - eActionId_REC_FIRST, &stRecSetup);
		BUS_MGR_Create(MGR_RECORDER_GetProcName(ulTsrActionId), APP_PVR_PRIORITY, pstAction->pfActFunc, SVC_PIPE_GetActionHandle(ulTsrActionId), 0, 0, 0);

		if (BUS_MGR_Get(pstAction->pfActFunc) == NULL)
		{
			MGR_ACTION_Release(ulTsrActionId, eRecType_TSR_REC);
			HxLOG_Error("\n\n\n\n\n\n\n\n\n\n\n\n Something Wrong !!!!!!! \n\n\n\n\n\n\n\n\n\n\n");

			return MESSAGE_PASS;
		}
	}

	pstTsrReq->hResult = ERR_OK;
	return MESSAGE_PASS;
}
STATIC BUS_Result_t xmgr_pvrbase_msgAmActionSetup (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_msgAmActionTaken(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrAction_Type_e		 eActType = (MgrAction_Type_e)p1;
	MgrAction_Type_e		 eConvActType = eActionType_NONE;
	XmgrPvr_ActionType_e	 ePvrActType;
	XmgrPvr_Action_t		*pstAction;
	HERROR					 hErr;

	hErr = xmgr_pvr_ConvertAmActTypeToPvrActType(eActType, &ePvrActType);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("[xmgr_pvrbase_msgAmActionStarted] No PVR Action Type.\n");
		return MESSAGE_PASS;
	}

	pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_ALL, ulActionId);
	if (pstAction != NULL)
	{
		// 동일한 PVR Action Type이다:
		hErr = xmgr_pvr_ConvertPvrActTypeToAmActType(pstAction->eType, &eConvActType);
		if ((ERR_OK == hErr) && (eConvActType == eActType))
		{
			HxLOG_Info("PVR manager already managed the action.\n");
			return MESSAGE_PASS;
		}

		// 다른 Action : 이전에 잡은 PVR Action이Release 없이 다른 Setup으로 들어온 경우임.
		// 밑의 경우와 일괄처리하기 위해 일단 이 Action은 제거처리 해 준다.
#if defined(CONFIG_DEBUG)
		HxLOG_Print("[xmgr_pvrbase_msgAmActionStarted] ActionID(%d) : PVR Action Type [%s]->[%s]\n", ulActionId,
																			(char *)xmgr_PVR_BASE_GetPvrActionTypeStr(pstAction->eType),
																			(char *)xmgr_PVR_BASE_GetPvrActionTypeStr(ePvrActType));
#endif
		(void)xmgr_pvr_ResetPvrAction(pstAction);
	}

	// PVR Manager 이외의 곳에서 SetupAction이 일어나서 제멋대로 PVR Action을 잡은 경우.
	pstAction = xmgr_pvr_GetEmptyPvrAction(ePvrActType);
	if (pstAction == NULL)
	{
		HxLOG_Print("[xmgr_pvrbase_msgAmActionStarted] No empty PVR Action Slot for the PvrActType(%d)\n", ePvrActType);
		return MESSAGE_PASS;
	}

	xmgr_pvrbase_setPvrActionByOuterInfo(pstAction, ulActionId, ePvrActType);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t	xmgr_pvrbase_msgAmActionReleased(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrAction_Type_e		 eActType = (MgrAction_Type_e)p1;
	XmgrPvr_Action_t		*pstAction;

	switch (eActType)
	{
	case eRecType_REC:
	case eRecType_EXT_REC:
		pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_REC, ulActionId);
		if (pstAction != NULL)
		{
			(void)xmgr_pvr_ResetPvrAction(pstAction);
		}

		break;

	case eRecType_TSR_REC:
		pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_TSR, ulActionId);
		if (pstAction != NULL)
		{
			(void)xmgr_pvr_ResetPvrAction(pstAction);
		}

		break;

	case eViewType_PB:
	case eViewType_TSR_PB:
	case eViewType_CHASE_PB:
		pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_PB, ulActionId);
		if (pstAction != NULL)
		{
			(void)xmgr_pvr_ResetPvrAction(pstAction);
		}

		break;

	default:
		break;
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t	xmgr_pvrbase_msgAmActionTypeChanged(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrAction_Type_e		 eActType = (MgrAction_Type_e)p1;
	MgrAction_Type_e		 eConvActType = eActionType_NONE;
	XmgrPvr_ActionType_e	 ePvrActType;
	XmgrPvr_Action_t		*pstAction;
	HERROR					 hErr;

	hErr = xmgr_pvr_ConvertAmActTypeToPvrActType(eActType, &ePvrActType);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("[xmgr_pvrbase_msgAmActionTypeChanged] No PVR Action Type.\n");
		return MESSAGE_PASS;
	}

	pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_ALL, ulActionId);
	if (pstAction != NULL)
	{
		// 동일한 PVR Action Type이다:
		hErr = xmgr_pvr_ConvertPvrActTypeToAmActType(pstAction->eType, &eConvActType);
		if ((ERR_OK == hErr) && (eConvActType == eActType))
		{
			HxLOG_Error("PVR manager already managed the action.\n");
			return MESSAGE_PASS;
		}

		// 다른 Action : 이전에 잡은 PVR Action이Release 없이 다른 Setup으로 들어온 경우임.
#if defined(CONFIG_DEBUG)
		HxLOG_Print("[xmgr_pvrbase_msgAmActionTypeChanged] ActionID(%d) : PVR Action Type [%s]->[%s]\n", ulActionId,
																			(char *)xmgr_PVR_BASE_GetPvrActionTypeStr(pstAction->eType),
																			(char *)xmgr_PVR_BASE_GetPvrActionTypeStr(ePvrActType));
#endif
	}
	else
	{
		// PVR Manager 이외의 곳에서 SetupAction이 일어나서 제멋대로 PVR Action을 잡은 경우.
		pstAction = xmgr_pvr_GetEmptyPvrAction(ePvrActType);
		if (pstAction == NULL)
		{
			HxLOG_Print("[xmgr_pvrbase_msgAmActionTypeChanged] No empty PVR Action Slot for the PvrActType(%d)\n", ePvrActType);
			return MESSAGE_PASS;
		}
	}

	if (pstAction != NULL)
	{
		xmgr_pvrbase_setPvrActionByOuterInfo(pstAction, ulActionId, ePvrActType);
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}


STATIC BUS_Result_t	xmgr_pvrbase_msgAppRebootStb (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgr_pvrbase_stopAllPvrFunction();

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_evtStoragePvrEnabled(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				 ulPartIdx, ulPartNum;
	SvcFs_DevIdx_e			 eDevIdx = (SvcFs_DevIdx_e)p1;
	SvcFs_DevInfo_t 		 stDevInfo;
	SvcFs_PartitionInfo_t	*pstPartInfo, *pstPartArray = NULL;
	XmgrPvr_Context_t		*pstContext;
	HERROR					 hErr;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	hErr = SVC_FS_GetDeviceInfo (eDevIdx, &stDevInfo);
	if (hErr == ERR_OK)
	{
		// HDD는 존재한다, 하지만 Partition이 있는가, Format이 되어 있는가 확인한다.
		hErr = SVC_FS_GetPartitionInfoArray (eDevIdx, &ulPartNum, &pstPartArray);
		if (hErr == ERR_OK && ulPartNum > 0 && pstPartArray != NULL)
		{
			for (ulPartIdx = 0; ulPartIdx < ulPartNum; ulPartIdx++)
			{
				pstPartInfo = &(pstPartArray[ulPartIdx]);

				if (pstPartInfo->bAvailable == TRUE && pstPartInfo->bMounted == TRUE && pstPartInfo->bPvrFlag == TRUE)
				{
					pstContext->ePvrDevState[pstPartInfo->ulPvrIdx]= ePvrDevState_OK;
					pstContext->ePvrDevIdx	[pstPartInfo->ulPvrIdx] = eDevIdx;
					pstContext->ulPvrPartIdx[pstPartInfo->ulPvrIdx] = ulPartIdx;
					break;
				}
			}
		}
	}

	if (pstPartArray != NULL)
	{
		SVC_FS_FreePartitionInfoArray (pstPartArray);
	}
	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_evtStoragePvrDisabled(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulMainActionId;
	SvcFs_DevIdx_e			 eDevIdx = (SvcFs_DevIdx_e)p1;
	XmgrPvr_Context_t		*pstContext;
	MgrPb_Info_t	 stInfo;
	HERROR					 hErr;
	HUINT32				ulPvrIdx = (HUINT32)p3;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if(ulPvrIdx == 0xff)
	{
		HxLOG_Error("Not PVR HDD\n");
		return MESSAGE_PASS;
	}

	switch (pstContext->ePvrDevState[ulPvrIdx])
	{
	case ePvrDevState_OK:
		if (eDevIdx == pstContext->ePvrDevIdx[ulPvrIdx])
		{
			ulMainActionId = MGR_ACTION_GetMainActionId();
			if (MGR_ACTION_GetType(ulMainActionId) == eViewType_PB)
			{
				hErr = MGR_PLAYBACK_GetPlaybackInfo(ulMainActionId, &stInfo);
				if (hErr == ERR_OK )
				{
					SVC_META_SetAsDeleteSkipContent(stInfo.ulContentId, FALSE);
				}
			}
			xmgr_pvrbase_stopPvrFunction(ulPvrIdx);
		}

		pstContext->ePvrDevIdx[ulPvrIdx] = eSVC_FS_DEV_NULL;
		pstContext->ulPvrPartIdx[ulPvrIdx] = 0;
		pstContext->ePvrDevState[ulPvrIdx] = ePvrDevState_NoDevice;

		break;

	default:
		HxLOG_Error("pstContext->ePvrDevState is unknown:0x%X\n",pstContext->ePvrDevState[ulPvrIdx]);
		break;
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;


}

STATIC BUS_Result_t xmgr_pvrbase_evtRecDelayedRecordStatusChange(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrAction_State_e		 eAmState;
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	XmgrPvr_Action_t		*pstAction;

	// Action ID에 맞는 Record Action이 현재 존재하는가?
	pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_REC, ulActionId);
	if (pstAction == NULL)
	{
		HxLOG_Error("[xmgr_pvrbase_msgAppRecordStopRec] No Record Action in ActionID(%d)\n", ulActionId);
		return MESSAGE_PASS;
	}

	eAmState = MGR_ACTION_GetState();
	if (eAmState == eAmState_ACTIVE_STANDBY)		// Active Standby Action Manager
	{
		if (p1 == eMgrRec_DelayedCopy_Completed || p1 == eMgrRec_DelayedCopy_Error)
		{
			// Standby 중에 Record Stop이 올라오면 Standby 로 간다.

			if(pstAction->eCopyStatus == ePvrCopyStatus_WaitingForStop)
			{
				xmgr_pvr_DestroyPvrAction(pstAction);
			}
			else
			{
				pstAction->eCopyStatus = ePvrCopyStatus_Complete;
			}

			HxLOG_Debug("\nSend MSG_ACTION_MANUAL_GO_MANUAL_ACTION. because Not bRecFound\n");
			//BUS_SendMessage(NULL, MSG_ACTION_MANUAL_GO_MANUAL_ACTION, hAction, TRUE, TRUE, 0);
		}
	}
	else
	{
		if (p1 == eMgrRec_DelayedCopy_Completed || p1 == eMgrRec_DelayedCopy_Error)
		{
			if(pstAction->eCopyStatus == ePvrCopyStatus_WaitingForStop)
			{
				xmgr_pvr_DestroyPvrAction(pstAction);
			}
			else
			{
				pstAction->eCopyStatus = ePvrCopyStatus_Complete;
			}
		}
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_evtPvrRecordSeriesLockFound (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	HUINT32				 ulContentId = (HUINT32)p1;
	HINT32				 nLockedContent = p2;
	SvcMeta_Record_t	 stRecInfo;
	HERROR				 hErr;

	hErr = SVC_META_GetRecordInfo(ulContentId, &stRecInfo);
	if ((ERR_OK == hErr) && (TRUE == stRecInfo.bSeriesRec))
	{
		HBOOL				 bUserLocked = (nLockedContent > 0) ? TRUE : FALSE;

		if (stRecInfo.bUserLocked != bUserLocked)
		{
			stRecInfo.bUserLocked = bUserLocked;

			hErr = SVC_META_SetRecordInfo(ulContentId, &stRecInfo);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_META_SetRecordInfo err: contentsId (%d)\n", ulContentId);
			}
		}
	}
#endif

	return MESSAGE_PASS;
}

static HBOOL	xmgr_pvrbase_ComparePath (const DxRecListData_t *rec, void *path)
{
	return HLIB_STD_StrStartWith(rec->stMeta.szPath, path);
}

STATIC BUS_Result_t xmgr_pvrbase_msgAppFsFormatDevice(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SvcFs_DevIdx_e		 eDevIdx = (SvcFs_DevIdx_e)p1;
	XmgrPvr_Context_t	*pstContext = xmgr_pvr_GetContext();
	HERROR				 hErr;
	HBOOL				bFlag=FALSE;
	HUINT32				ulPvrIdx=0;
	HCHAR				szPath[SVC_FS_PVR_DIR_PATH_LEN];

	if (pstContext == NULL)					{ return MESSAGE_PASS; }
	HxLOG_Print("[%s:%d]eDevIdx[%d]\n",__FUNCTION__,__LINE__,eDevIdx);
	// Format 하려는 Device가 현재 PVR로 동작 중인지를 확인한다.
	bFlag = SVC_FS_IsDevicePvrUsed(eDevIdx);
	if(bFlag == TRUE)
	{
		hErr = SVC_FS_GetPvrIdxByDevIdx(&ulPvrIdx, eDevIdx);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return MESSAGE_PASS;
		}

		pstContext->ePvrDevState[ulPvrIdx]	= ePvrDevState_Formatting;
		pstContext->ePvrDevIdx[ulPvrIdx]		= eDevIdx;

		xmgr_pvrbase_stopPvrFunction(ulPvrIdx);

		HxSTD_MemSet(szPath, 0, SVC_FS_PVR_DIR_PATH_LEN);

		// 해당 HDD playlist unload
		hErr = SVC_FS_GetPvrPath(ulPvrIdx, eSVC_FS_UsageType_Record, szPath);
		if(ERR_OK == hErr)
		{
			SVC_META_UnloadContentsInfoByPath(szPath);
			OTL_HJM_RemoveRecordingList(xmgr_pvrbase_ComparePath, (void *)szPath, FALSE);
		}
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_evtSiTdtTot(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_pvrbase_msgAppRecordStopTsr(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulActionId = SVC_PIPE_GetActionId(hAction);
	XmgrPvr_Context_t		*pstContext = NULL;
	XmgrPvr_Action_t		*pstAction = NULL;

	ulActionId = SVC_PIPE_GetActionId(hAction);
	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL) {
		HxLOG_Error("xmgr_pvr_GetContext() Is NULL \n");
		return MESSAGE_PASS;
	}

	if (TRUE == pstContext->bTimeAvailable)
	{
		pstAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_TSR_Normal, ulActionId);
		if (NULL == pstAction)
		{
			HxLOG_Error("No TSR Action\n");
			return ERR_FAIL;
		}
		xmgr_pvr_DestroyPvrAction(pstAction);
	}

	return MESSAGE_PASS;
}

HERROR xmgr_pvrbase_GetPvrIdxByPath(HUINT8 *pszPath, HUINT32 *pulPvrIdx)
{
	HUINT32		ulIdx = 0;
	HUINT8		*pszTemp = NULL;

	if(pszPath == NULL || pulPvrIdx == NULL)
	{
		return ERR_FAIL;
	}

	for (ulIdx = 0 ; ulIdx < MAX_CONNECTABLE_PVR_STORAGE_NUM ; ulIdx++)
	{
		pszTemp = MGR_RECORDER_GetRecordPath(ulIdx);
		if(HxSTD_StrNCmp(pszTemp, pszPath, HxSTD_StrLen(pszPath)) == 0)
		{
			*pulPvrIdx = ulIdx;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#define _____GLOBAL_FUNCTIONS_____
STATIC BUS_Result_t xproc_PVR_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		 eGwmResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
	case eMEVT_BUS_CREATE :
		HxLOG_Print("MESSAGE : eMEVT_BUS_CREATE [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgBusCreate(hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("MESSAGE : eMEVT_BUS_DESTROY [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgBusDestroy(hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("MESSAGE : eMEVT_BUS_TIMER [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgBusTimer(hAction, p1, p2, p3);
		break;

/*
	Playback 명령
*/

	case eMEVT_PVR_PLAYBACK_STOP:
		HxLOG_Print("MESSAGE : eMEVT_PVR_PLAYBACK_STOP [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAppPlaybackStopPb (hAction, p1, p2, p3);
		break;

/*
	Record 명령
*/
	case eMEVT_PVR_RECORD_START_REC:
		HxLOG_Print("MESSAGE : eMEVT_PVR_RECORD_START_REC [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAppRecordStartRec(hAction, p1, p2, p3);
		break;

	case eMEVT_PVR_RECORD_STOP_REC:
		HxLOG_Print("MESSAGE : eMEVT_PVR_RECORD_STOP_REC [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAppRecordStopRec(hAction, p1, p2, p3);
		break;

/*
	TSR Record 명령
*/
	case eMEVT_PVR_RECORD_START_TSR:
		HxLOG_Print("MESSAGE : eMEVT_PVR_RECORD_START_TSR [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAppTsrStartTsr(hAction, p1, p2, p3);
		break;

	case eMEVT_PVR_RECORD_STOP_TSR:
		HxLOG_Print("MESSAGE : eMEVT_PVR_RECORD_STOP_TSR [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		//eGwmResult = xmgr_pvrbase_msgAppTsrStopTsr(hAction, p1, p2, p3);
		break;

/*
	Action Message
*/
	case eMEVT_ACTION_SETUP_REQUESTED:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_SETUP_REQUESTED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAmActionSetup (hAction, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_TAKEN:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_NOTIFY_TAKEN [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAmActionTaken(hAction, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_RELEASED:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_NOTIFY_RELEASED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAmActionReleased(hAction, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_TYPE_CHANGED:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_NOTIFY_TYPE_CHANGED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAmActionTypeChanged(hAction, p1, p2, p3);
		break;


/*
	System Power Off / Reset
*/
	case eMEVT_ACTION_REBOOT_STB:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_REBOOT_STB [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAppRebootStb (hAction, p1, p2, p3);
		break;

	case eMEVT_STORAGE_NOTIFY_PVR_ENABLED:
		HxLOG_Print("MESSAGE : eMEVT_STORAGE_NOTIFY_PVR_ENABLED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_evtStoragePvrEnabled(hAction, p1, p2, p3);
		break;

	case eMEVT_STORAGE_NOTIFY_PVR_DISABLED:
		HxLOG_Print("MESSAGE : eMEVT_STORAGE_NOTIFY_UNPLUGGED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_evtStoragePvrDisabled(hAction, p1, p2, p3);
		break;

	// Normal Action Manager에 구현되어 있던 PVR 항목
	case eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE:
		HxLOG_Print("MESSAGE : eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_evtRecDelayedRecordStatusChange(hAction, p1, p2, p3);
		break;

/*
	File System Functions
*/
	case eMEVT_STORAGE_FORMAT_DEVICE:
		HxLOG_Print("MESSAGE : eMEVT_STORAGE_FORMAT_DEVICE [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_pvrbase_msgAppFsFormatDevice(hAction, p1, p2, p3);
		break;

/*
	시간이 Enable되어야 동작 허용
*/
	case eSEVT_SI_TDT:
	case eSEVT_SI_TOT:
		eGwmResult = xmgr_pvrbase_evtSiTdtTot(hAction, p1, p2, p3);
		break;
#if 0
#if defined(CONFIG_MW_MM_TRANSCODING)
	case eMEVT_PVR_TRREC_MGR_START:
		eGwmResult = xmgr_pvrbase_msgAppTrRecMgrStart(hAction, p1, p2, p3);
		break;
#endif
#endif
	case eMEVT_PVR_RECORD_SERIES_LOCK_FOUND:
		eGwmResult = xmgr_pvrbase_evtPvrRecordSeriesLockFound(hAction, p1, p2, p3);
		break;

	default:
		eGwmResult = ERR_BUS_NO_ERROR;
		break;
	}

	return(eGwmResult == MESSAGE_BREAK) ? MESSAGE_BREAK : MESSAGE_PASS;
}

HERROR xmgr_pvr_Create_Base(void)
{
	BUS_Result_t 	eRes;

	eRes = BUS_MGR_Create("proc_PVR", APP_PVR_MGR_PRIORITY, xproc_PVR_Base, 0, 0, 0, 0);

	return(eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

// Default HDD의 상태만 확인
HERROR xmgr_pvr_CheckPvrDevStatus_Base(HBOOL *pbIsAvailable)
{
	XmgrPvr_Context_t	*pstContext;
	HUINT32 			 ulPvrIdx;

	if (NULL == pbIsAvailable)
		return ERR_FAIL;

	*pbIsAvailable = FALSE;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
	if(MAX_CONNECTABLE_PVR_STORAGE_NUM <= ulPvrIdx)
	{
		return ERR_FAIL;
	}

	*pbIsAvailable = (pstContext->ePvrDevState[ulPvrIdx] == ePvrDevState_OK) ? TRUE : FALSE;

	return ERR_OK;
}

HERROR xmgr_pvr_DestroyPvrAction_BASE (XmgrPvr_Action_t *pstAction)
{
	XmgrPvr_Action_t			 stAction;

	if (pstAction == NULL)						{ return ERR_FAIL; }

	// pstAction에 연결된 reference가 다 끊길 때  동작한다.
	pstAction->nRefCount--;
	if (pstAction->nRefCount > 0)
	{
		return ERR_OK;
	}

	switch (pstAction->eState)
	{
	case ePvrActState_Working:
		// Destroy 이후 Action Item 값이 바뀔테니 일단 여기에 복사해두고 이 값으로 Preprocess, Postprocess로 쓴다.
		stAction = *pstAction;

		// Action Destroy 이전에 해 주어야 하는 것들
		xmgr_pvrbase_preprocessForDestroyingPvrAction(&stAction);

		if (pstAction->pfActFunc != NULL)
		{
			// Action에 대한 Reset은 해당 proc이 action release를 하면 ACTION_STOPPED 메시지에서 처리된다.
			// 그러므로 여기서 일일이 해줄 필요가 전혀 없다.
			BUS_MGR_Destroy(pstAction->pfActFunc);
		}

		xmgr_pvrbase_postprocessForDestroyingPvrAction(&stAction);

		break;

	case ePvrActState_WaitingByTimer:
		// Timer로 기다린 것이며, 실제 Action이 실행된 것이 아니므로 Kill 할 필요도 pre- post-도 처리할 필요도 없다.
		if (pstAction->ulTimerId != 0)
		{
			BUS_KillTimer(pstAction->ulTimerId);
		}

		// Reset the Action:
		(void)xmgr_pvr_ResetPvrAction(pstAction);
		break;

	default:
		break;
	}

	return ERR_OK;
}
#define _____PLAYBACK_FUNCTION_____
HERROR xmgr_pvr_StartTsrChasePlayback_Base(HUINT32 ulActionId, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPosOrigin, HUINT32 ulOffset, MgrPb_Option_e ePbOption)
{
	HBOOL						 bChasePlayEnable = MGR_PVR_GetChasePlaySurferEnable();
	HBOOL						 bTsrPlayEnable = MGR_PVR_GetTsrPlaybackEnable();
	Handle_t					 hMasterSvc, hCurrSvc;
	ChListUiGroup_t				 eSvcListGroup;
	MgrRec_RecInfo_t		 stRecInfo;
	HERROR						 hErr;

	// WatchTV 의 Master, Actual Service 및 Group 정보를 얻어온다.
	hErr = MGR_LIVE_GetServiceHandle(ulActionId, eLiveViewPlaySvc_MASTER, &hMasterSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error : ulActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	hErr = MGR_LIVE_GetServiceHandle(ulActionId, eLiveViewPlaySvc_ACTUAL, &hCurrSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error : ulActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	eSvcListGroup = eChListUiGroup_TV;

	// Check the chase record
	hErr = MGR_RECORDER_FindRecordingSvcInfo(hCurrSvc, NULL, &stRecInfo);
	if (hErr == ERR_OK && bChasePlayEnable == TRUE)
	{
		if (stRecInfo.ulRecTime >= MGR_PLAYBACK_GetPlaybackParam(eMgrPb_MIN_REQUIRED_PLAYBACK_TIME))
		{
			hErr = xmgr_pvrbase_startChasePlayback(ulActionId, hCurrSvc, hMasterSvc, eSvcListGroup, &stRecInfo, eTrick, ePbPosOrigin, ulOffset, ePbOption);
		}
		else
		{
			HxLOG_Error("stRecInfo.ulRecTime:%u\n",stRecInfo.ulRecTime);
			hErr = ERR_FAIL;
		}

		return hErr;
	}

	// Check the TSR
	hErr = MGR_RECORDER_FindTsrRecordingSvcInfo(hCurrSvc, NULL, &stRecInfo);
	if (hErr == ERR_OK && bTsrPlayEnable == TRUE)
	{
		if (stRecInfo.ulRecTime >= MGR_PLAYBACK_GetPlaybackParam(eMgrPb_MIN_REQUIRED_PLAYBACK_TIME))
		{
			hErr = xmgr_pvrbase_startTsrPlayback(ulActionId, hCurrSvc, hMasterSvc, eSvcListGroup, &stRecInfo, eTrick, ePbPosOrigin, ulOffset, ePbOption);
		}
		else
		{
			HxLOG_Error("stRecInfo.ulRecTime:%u\n",stRecInfo.ulRecTime);
			hErr = ERR_FAIL;
		}

		return hErr;
	}

	HxLOG_Error("return ERR_FAIL\n");
	return ERR_FAIL;
}

HERROR xmgr_pvr_StartPlayback_Base(HUINT32 ulContentId, HUINT32 ulStartTime)
{
	HBOOL					bMakeThumbnail = TRUE;
	Handle_t				hSvc = HANDLE_NULL;
	MgrPb_PlayType_e	ePbPosOrigin;
	HUINT32					ulActionId = MGR_ACTION_GetMainActionId();
	SvcMeta_Record_t 		stSvcRecInfo;
	MgrRec_RecInfo_t	 	stApRecInfo;
	MgrAction_Type_e		eAmActType = eViewType_PB;
	HERROR					hErr;

	hErr = SVC_META_GetRecordInfo(ulContentId, &stSvcRecInfo);
	if (hErr == ERR_OK)
	{
		// 현재 녹화중인 파일이라면 Normal Playback이 아니라 Chase PB로 처리해야 한다.
		if (	eMgrRec_STS_RECORDING    == stSvcRecInfo.ucRecordStatus
			 || eMgrRec_STS_RECORD_START == stSvcRecInfo.ucRecordStatus)
		{	// Recording -> Check Chase PB
			hErr = MGR_RECORDER_GetRecordSvcHandleByContentId(ulContentId, &hSvc);
			if (ERR_OK == hErr)
			{
				eAmActType = eViewType_CHASE_PB;
			}
			else
			{	// RECORDING & Not Recording in the Recording Context -> caused by exception(eg-PowerOff) in the recording
				HxLOG_Error("Recording, But Can't find(%d) CID in the RECCnxt\n", ulContentId);

				stSvcRecInfo.ucRecordStatus = eMgrRec_STS_INCOMPLETED;
                if (eMgrRecFailed_NOFAIL == stSvcRecInfo.ucEndCondition)
				    stSvcRecInfo.ucEndCondition = eMgrRecFailed_UNKNOWN;

				hErr = SVC_META_SetRecordInfo(ulContentId, &stSvcRecInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SVC_META_SetRecordInfo. Error\n");
					return hErr;
				}
			}
		}
	}

	switch (eAmActType)
	{
		case eViewType_CHASE_PB:

			hErr = MGR_RECORDER_GetRecordSvcHandleByContentId(ulContentId, &hSvc);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("hErr:0x%X\n",hErr);
				return ERR_FAIL;
			}

			hErr = MGR_RECORDER_FindRecordingSvcInfo(hSvc, NULL, &stApRecInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("MGR_RECORDER_FindRecordingSvcInfo ret:0x%X\n",hErr);
				return ERR_FAIL;
			}

			ePbPosOrigin = (ulStartTime == 0) ?  eMgrPb_PlayType_FirstPos : eMgrPb_PlayType_RecentPos;
			hErr = xmgr_pvrbase_startChasePlayback(ulActionId, hSvc, hSvc, eChListUiGroup_CURRENT, &stApRecInfo, eMgrPb_TickState_PLAY, ePbPosOrigin, ulStartTime, FALSE);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_pvrbase_startChasePlayback ret:0x%X\n",hErr);
				return ERR_FAIL;
			}

			break;

		case eViewType_PB:
		default:
			hErr = xmgr_pvrbase_startNormalPlayback(ulActionId, ulContentId, ulStartTime, bMakeThumbnail, ePbTrRecMode_none);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_pvrbase_startNormalPlayback ret:0x%X\n",hErr);
				return ERR_FAIL;
			}

			break;
	}

	// 한번 본 것이므로 보았다는 Flag를 반드시 붙인다.
	// 밑의 Make Thumbnail은 보는 것이 아니니까 그렇게 해서는 안된다.
	HxSTD_MemSet(&stSvcRecInfo, 0, sizeof(SvcMeta_Record_t));
	hErr = SVC_META_GetRecordInfo(ulContentId, &stSvcRecInfo);
	if ( (hErr == ERR_OK) && (stSvcRecInfo.u1cWatched == 0) )
	{
		stSvcRecInfo.u1cWatched = 1;
		hErr = SVC_META_SetRecordInfo(ulContentId, &stSvcRecInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_META_SetRecordInfo. Error\n");
			return hErr;
		}
	}

	return ERR_OK;
}

HERROR xmgr_pvr_MakeThumbnail_Base(HUINT32 ulContentId)
{
	HUINT32			 ulActionId = MGR_ACTION_GetMainActionId();
	HERROR			 hErr;

	hErr = xmgr_pvrbase_startNormalPlayback(ulActionId, ulContentId, 0, TRUE, ePbTrRecMode_none);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____TRANSCODING_FUNCTION_____
STATIC HCHAR *xmgr_pvr_ProfileCvtVideoCodecToAscii_Base(DxVideoCodec_e codec)
{
	switch (codec)
	{
	case eDxVIDEO_CODEC_H264:	return "h264";
/*
		ENUM_TO_STR(eDxVIDEO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG1);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG2);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG4_PART2);
		ENUM_TO_STR(eDxVIDEO_CODEC_H263);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_SVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_MVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1_SIMPLEMAIN);
		ENUM_TO_STR(eDxVIDEO_CODEC_AVS);
		ENUM_TO_STR(eDxVIDEO_CODEC_RV40);
		ENUM_TO_STR(eDxVIDEO_CODEC_VP6);
		ENUM_TO_STR(eDxVIDEO_CODEC_DIVX311);
		ENUM_TO_STR(eDxVIDEO_CODEC_FromLIVE);
*/
	default:
		break;
	}
	return NULL;
}

STATIC DxVideoCodec_e xmgr_pvr_ProfileCvtAsciiToVideoCodec_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_CODEC_MAX_NUM;
	HCHAR		*text = NULL;

	for (i=0; i < max; i++)
	{
		text = xmgr_pvr_ProfileCvtVideoCodecToAscii_Base((DxVideoCodec_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return(DxVideoCodec_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}

STATIC HCHAR *xmgr_pvr_ProfileCvtAudioCodecToAscii_Base(DxAudioCodec_e codec)
{
	switch (codec)
	{
	case eDxAUDIO_CODEC_MP3: return "mp3";
	case eDxAUDIO_CODEC_AAC: return "aac";
/*
		ENUM_TO_STR(eDxAUDIO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxAUDIO_CODEC_MPEG);
		ENUM_TO_STR(eDxAUDIO_CODEC_MP3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3P);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCM);

		ENUM_TO_STR(eDxAUDIO_CODEC_AAC);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_LOAS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS_ADTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS_HD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_HD_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_BLUERAY);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_STD);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_PRO);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCMWAVE);
		ENUM_TO_STR(eDxAUDIO_CODEC_DRA);

		ENUM_TO_STR(eDxAUDIO_CODEC_AIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_RIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_FromLIVE);
*/
	default:
		break;
	}
	return NULL;
}

STATIC HBOOL xmgr_pvr_ProfileCvtAsciiToCheckAsterisk_Base(const HCHAR *ascii)
{
	/* if property is '*', bUsingLiveXXX is TRUE */
	if(HxSTD_StrCmp("*", ascii) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC DxAudioCodec_e xmgr_pvr_ProfileCvtAsciiToAudioCodec_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxAUDIO_CODEC_MAX_NUM;
	HCHAR		*text = NULL;

	for (i=0; i < max; i++)
	{
		text = xmgr_pvr_ProfileCvtAudioCodecToAscii_Base((DxAudioCodec_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return(DxAudioCodec_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}

STATIC HCHAR *xmgr_pvr_ProfileCvtVideoAspectRatioToAscii_Base(DxAspectRatio_e eAspectRatio)
{
	switch (eAspectRatio)
	{
	case eDxASPECTRATIO_4X3:	return "4:3";
	case eDxASPECTRATIO_14X9:	return "14:9";
	case eDxASPECTRATIO_16X9:	return "16:9";
/*
		ENUM_TO_STR(eDxASPECTRATIO_UNKNOWN);
		ENUM_TO_STR(eDxASPECTRATIO_4X3);
		ENUM_TO_STR(eDxASPECTRATIO_16X9);
		ENUM_TO_STR(eDxASPECTRATIO_1X1);
		ENUM_TO_STR(eDxASPECTRATIO_14X9);
		ENUM_TO_STR(eHxASPECTRATIO_FromLIVE);
		ENUM_TO_STR(eDxASPECTRATIO_MAX_NUM);
*/
		default: break;
	}

	return NULL;
}

STATIC DxAspectRatio_e xmgr_pvr_ProfileCvtAsciiToVideoAspectRatio_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxASPECTRATIO_MAX_NUM;
	HCHAR		*text = NULL;

	for (i=0; i < max; i++)
	{
		text = xmgr_pvr_ProfileCvtVideoAspectRatioToAscii_Base((DxAspectRatio_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return(DxAspectRatio_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}


STATIC HCHAR *xmgr_pvr_ProfileCvtVidoeEncCodecLevelToAscii_Base(DxVideoEncodeLevel_e level)
{
	switch (level)
	{
	case eDxVIDEO_ENC_LEVEL_30:	return "30";
	case eDxVIDEO_ENC_LEVEL_31:	return "31";
/*
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_00);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_10);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_1B);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_11);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_12);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_13);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_20);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_21);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_22);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_30);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_31);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_32);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_40);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_41);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_42);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_50);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_51);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_60);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_62);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_LOW);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_MAIN);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_HIGH);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_HIGH1440);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_AUTO);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_NONE);
*/
		default: break;
	}

	return NULL;
}


STATIC DxVideoEncodeLevel_e xmgr_pvr_ProfileCvtAsciiToVidoeEncCodecLevel_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_ENC_LEVEL_MAX;
	HCHAR		*text = NULL;

	for (i=0; i < max; i++)
	{
		text = xmgr_pvr_ProfileCvtVidoeEncCodecLevelToAscii_Base((DxVideoEncodeLevel_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return(DxVideoEncodeLevel_e)i;
		}
	}

	return eDxVIDEO_ENC_LEVEL_30;
}



STATIC HCHAR *xmgr_pvr_ProfileCvtVidoeEncCodecProfileToAscii_Base(DxVideoEncodeProfile_e level)
{
	switch (level)
	{
		case eDxVIDEO_ENC_PROFILE_MAIN:		return "main";
		case eDxVIDEO_ENC_PROFILE_BASELINE:	return "baseline";
/*
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SIMPLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_MAIN);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_HIGH);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_ADVANCED);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_JIZHUN);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SNRSCALABLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SPATIALLYSCALABLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_ADVANCEDSIMPLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_BASELINE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_AUTO);
*/
		default: break;
	}

	return NULL;
}


STATIC DxVideoEncodeProfile_e xmgr_pvr_ProfileCvtAsciiToVidoeEncCodecProfile_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_ENC_PROFILE_MAX_NUM;
	HCHAR		*text = NULL;

	for (i=0; i < max; i++)
	{
		text = xmgr_pvr_ProfileCvtVidoeEncCodecProfileToAscii_Base((DxVideoEncodeProfile_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return(DxVideoEncodeProfile_e)i;
		}
	}

	return eDxVIDEO_ENC_PROFILE_BASELINE;
}


HERROR	xmgr_pvr_GetProfileFromJSON_Base(const HCHAR *pProfileString, MgrTrcd_StreamProfile *pstProfile)
{
	if ((NULL == pProfileString) || (NULL == pstProfile))
		return ERR_FAIL;

	/* parsing */
	{
		const HINT32	DEFAULT_VIDEO_BITRATE	=	1120;
		const HINT32	DEFAULT_SCREEN_WIDTH	=	640;
		const HINT32	DEFAULT_SCREEN_HEIGHT	=	360;
		const HINT32	DEFAULT_AUDIO_BITRATE	=	64;
		const HINT32	DEFAULT_FRAMERATE		=	50;

		HxJSON_t pJsonRoot = NULL;
		pJsonRoot = HLIB_JSON_Decode(pProfileString);

		if (NULL != pJsonRoot)
		{
			HINT32	nVideoBitrateKbps		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "video_bitrate_kbps", DEFAULT_VIDEO_BITRATE);
//			HINT32	nSdVideoBitrateKbps 	= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "sd_video_bitrate_kbps", nVideoBitrateKbps);
			HINT32	nHdVideoBitrateKbps 	= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "hd_video_bitrate_kbps", nVideoBitrateKbps);
			HINT32	nScreenWidth			= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "screen_width", DEFAULT_SCREEN_WIDTH);
			HINT32	nScreenHeight			= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "screen_height", DEFAULT_SCREEN_HEIGHT);

			HBOOL	bInterlaceMode			= HLIB_JSON_Object_GetBoolean(pJsonRoot, "interlace_mode");

			HINT32	nAudioBitrateKbps	= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "audio_bitrate_kbps", DEFAULT_AUDIO_BITRATE);
			HINT32	nFrameRate			= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "frame_rate", DEFAULT_FRAMERATE);
//			HBOOL	bUseTranscoder		= HLIB_JSON_Object_GetBoolean(pJsonRoot, "use_transcoding");

			DxAspectRatio_e eVideoAspectRatio = 0;
			DxVideoCodec_e	eVideoCodec = 0;
			DxAudioCodec_e	eAudioCodec = 0;
			HBOOL bUsingLiveAudioCodec = 0;
			DxVideoEncodeLevel_e eVideoCodecLevel = 0;
			DxVideoEncodeProfile_e eVideoCodecProfile = 0;
			HBOOL	bUsingLiveAspectRatio = 0;

			/* string to value */
			{
				const HCHAR *json_str = NULL;

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "aspect_ratio");
				HxLOG_Assert(json_str);
				eVideoAspectRatio		= xmgr_pvr_ProfileCvtAsciiToVideoAspectRatio_Base(json_str);
				bUsingLiveAspectRatio	= xmgr_pvr_ProfileCvtAsciiToCheckAsterisk_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec");
				HxLOG_Assert(json_str);
				eVideoCodec = xmgr_pvr_ProfileCvtAsciiToVideoCodec_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "audio_enc_codec");
				HxLOG_Assert(json_str);
				eAudioCodec 		= xmgr_pvr_ProfileCvtAsciiToAudioCodec_Base(json_str);
				bUsingLiveAudioCodec	= xmgr_pvr_ProfileCvtAsciiToCheckAsterisk_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec_level");
				HxLOG_Assert(json_str);
				eVideoCodecLevel		= xmgr_pvr_ProfileCvtAsciiToVidoeEncCodecLevel_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec_profile");
				HxLOG_Assert(json_str);
				eVideoCodecProfile	= xmgr_pvr_ProfileCvtAsciiToVidoeEncCodecProfile_Base(json_str);
			}

			pstProfile->eVideoCodec			=	eVideoCodec;
			pstProfile->eAudioCodec			=	eAudioCodec;
			pstProfile->nAudioBitrateKbps 	=	nAudioBitrateKbps;
			pstProfile->nVideoBitrateKbps 	=	nVideoBitrateKbps;
			pstProfile->nHdVideoBitrateKbps	=	nHdVideoBitrateKbps;
			pstProfile->nFrameRate			=	nFrameRate;
			pstProfile->nScreenWidth		=	nScreenWidth;
			pstProfile->nScreenHeight 		=	nScreenHeight;
			pstProfile->bInterlaceMode		=	bInterlaceMode;
			pstProfile->eVideoAspectRatio 	=	eVideoAspectRatio;
			pstProfile->eVideoCodecProfile	=	eVideoCodecProfile;
			pstProfile->eVideoCodecLevel		=	eVideoCodecLevel;
			pstProfile->bUsingLiveAudioCodec	=	bUsingLiveAudioCodec;
			pstProfile->bUsingLiveAspectRatio 	=	bUsingLiveAspectRatio;
		}
		else
		{
			HxLOG_Error("Error. \n");
			return ERR_FAIL;
		}

		if (NULL != pJsonRoot)
		{
			HLIB_JSON_Delete(pJsonRoot);
			pJsonRoot = NULL;
		}
	}

	return ERR_OK;
}

HERROR xmgr_pvr_StartPlaybackTranscodeToRingBuf_Base(MgrPvr_PbTrcdToMemSetup_t *pstSetup, HUINT32 *pulActionId)
{
#if defined(CONFIG_MW_MM_TRANSCODING)
	HUINT32 				 ulActionId = HANDLE_NULL;
	MgrTrcd_Start_t		 stTrcdStart;
	SvcMeta_Record_t			 stRecInfo;
	HERROR					 hErr;

	if ( (NULL == pstSetup) || (NULL == pulActionId) )
		return ERR_FAIL;

	hErr = SVC_META_GetRecordInfo(pstSetup->ulContentId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_META_GetRecordInfo err: contentID(%d)\n", pstSetup->ulContentId);
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId failed\n");
		return ERR_FAIL;
	}

	// start playback
	hErr = xmgr_pvrbase_startLivestreamPlayback(ulActionId, pstSetup->ulContentId, pstSetup->ulStartTime, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_pvrbase_startLivestreamPlayback failed\n");
		return ERR_FAIL;
	}

	stTrcdStart.eSrcType					= eMgrTrcd_SrcType_Playback;
	stTrcdStart.unSrc.stPb.ulContentId		= pstSetup->ulContentId;
	stTrcdStart.eDstType					= eMgrTrcd_DstType_RingBuf;
	stTrcdStart.unDst.stRingBuf.hRingBuf	= pstSetup->hRingBuf;
	stTrcdStart.bPcrTsPacketInsert			= pstSetup->bPcrTsPacketInsert;

	HxSTD_MemCopy(&(stTrcdStart.stProfile), &(pstSetup->stProfile), sizeof(MgrTrcd_StreamProfile));

	hErr = MGR_TRANSCODER_Start(ulActionId, &stTrcdStart);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_TRANSCODER_Start err: ulActionId=%d\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL != pulActionId)					{ *pulActionId = ulActionId; }
#endif
	return ERR_OK;
}

HERROR xmgr_pvr_StopPlaybackTranscodeToRingBuf_Base(HUINT32 ulActionId)
{
#if defined(CONFIG_MW_MM_TRANSCODING)
	HERROR hErr;

	hErr = MGR_TRANSCODER_Stop (ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_TRANSCODER_Stop err: ulActionId=%d\n", ulActionId);
		return ERR_FAIL;
	}
#endif
	return ERR_OK;
}

HERROR xmgr_pvr_StartTrRecording_Base(HUINT32 ulContentId, HUINT32 ulRecMode, HUINT32 ulPvrIdx, HUINT32 *pulActionId)
{
#if defined(CONFIG_MW_MM_TRANSCODING)
	HUINT32					 ulActionId = HANDLE_NULL;
	HERROR					 hErr;
	SvcMeta_Record_t			 stRecInfo;

	hErr = SVC_META_GetRecordInfo(ulContentId, &stRecInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_META_GetRecordInfo err: contentID(%d)\n", ulContentId);
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_ACTION_GetEmptyBgActionId failed\n");
		return ERR_FAIL;
	}

	// start playback
	hErr = xmgr_pvrbase_startLivestreamPlayback(ulActionId, ulContentId, 0, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_pvrbase_startLivestreamPlayback failed\n");
		return ERR_FAIL;
	}

	{
		MgrTrcd_Start_t		 stTrcdStart;

		stTrcdStart.eSrcType				= eMgrTrcd_SrcType_Playback;
		stTrcdStart.unSrc.stPb.ulContentId	= ulContentId;
		stTrcdStart.eDstType				= eMgrTrcd_DstType_File;
		HxSTD_PrintToStrN (stTrcdStart.unDst.stFile.szFileUrl, TRANSCODING_FILE_LEN, "%s%s_tr", stRecInfo.aucPath, stRecInfo.aucFileName);

		// Test...
		//stTrcdStart.nProfileId				= 1;
		stTrcdStart.bPcrTsPacketInsert		= TRUE;

		hErr = MGR_TRANSCODER_Start(ulActionId, &stTrcdStart);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("MGR_TRANSCODER_Start err: ulActionId=%d\n", ulActionId);
			return ERR_FAIL;
		}
	}

	if (NULL != pulActionId)					{ *pulActionId = ulActionId; }

#endif
	return ERR_OK;

}

#define _____RECORD_FUNCTION_____
HERROR xmgr_pvr_CheckRecordPossible_Base(MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason)
{
	HUINT32					 ulActionId = HANDLE_NULL, ulFailReason = eMgrRecFailed_UNKNOWN;
	XmgrPvr_Action_t			*pstRecAction;

	pstRecAction = xmgr_pvrbase_checkRecordPossible(pstRecSetup, &ulActionId, &ulFailReason);

	if ((NULL != pstRecAction) && (ulActionId < NUM_MAX_ACTION))
	{
		if (NULL != pulActionId)			{ *pulActionId = ulActionId; }
		if (NULL != pulFailReason)			{ *pulFailReason = ulFailReason; }

		return ERR_OK;
	}

	if (NULL != pulActionId)			{ *pulActionId = HANDLE_NULL; }
	if (NULL != pulFailReason)			{ *pulFailReason = ulFailReason; }

	return ERR_OK;
}

HERROR xmgr_pvr_GetCurrentRecSvcHandles_Base(HUINT32 *pulRecNum, Handle_t *phSvcArray)
{
	HUINT32					 ulIndex, ulRecNum = 0;
	XmgrPvr_Action_t			*pstAction;
	XmgrPvr_Context_t		*pstContext;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)					{ return ERR_FAIL; }

	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if (pstAction->eState != ePvrActState_None)
		{
			switch (pstAction->eType)
			{
			case ePvrActType_REC_Normal:
			case ePvrActType_REC_EvtReserved:
			case ePvrActType_REC_Delayed:
				if (NULL != phSvcArray)				{ phSvcArray[ulRecNum] = pstAction->hCurrSvcHandle; }
				ulRecNum ++;
				break;

			default:
				break;
			}
		}
	}

	if (NULL != pulRecNum)					{ *pulRecNum = ulRecNum; }
	return ERR_OK;
}

MgrRec_EndReasonType_e xmgr_pvr_StartRecording_Base(MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId)
{
	HUINT32					 ulActionId = 0;
	MgrRec_EndReasonType_e	 eFailReason = eMgrRecFailed_UNKNOWN;

	if (pstRecSetup == NULL)			{ return eMgrRecFailed_UNKNOWN; }

	// TODO: 이 아래 code 너무 위험해요... sync message 라서 동작은 함.... Post message로만 바꿔도 동작 안할거임...원작하신분 수정해주세요 ㅠ
	BUS_SendMessage(NULL, eMEVT_PVR_RECORD_START_REC, HANDLE_NULL, (HINT32)pstRecSetup, (HINT32)&ulActionId, (HINT32)&eFailReason);

	HxLOG_Debug("hErr(fail reason): %d \n", eFailReason);

	if (NULL != pulActionId)			{ *pulActionId = ulActionId; }
	return eFailReason;
}


#define _____TSR_NORMAL_FUNCTION_____
HERROR xmgr_pvr_GetTsrAutoTracking(HUINT32 ulViewId, HBOOL *pbTsrAutoTrack)
{
	XmgrPvr_Context_t		*pstContext = xmgr_pvr_GetContext();

	if (NULL == pbTsrAutoTrack)
	{
		return ERR_FAIL;
	}

	if (ulViewId >= NUM_VIEW_ACTION)
	{
		return ERR_FAIL;
	}

	*pbTsrAutoTrack = pstContext->bTsrAutoTrack[ulViewId];

	return ERR_OK;
}

HERROR xmgr_pvr_SetTsrAutoTracking(HUINT32 ulViewId, HBOOL bTsrAutoTrack)
{
	HUINT32					 ulIdx, ulAutoTrackNum;
	XmgrPvr_Context_t		*pstContext = xmgr_pvr_GetContext();

	if (ulViewId >= NUM_VIEW_ACTION)
	{
		return ERR_FAIL;
	}

	if (pstContext->bTsrAutoTrack[ulViewId] == bTsrAutoTrack)
	{
		return ERR_OK;
	}

	for (ulIdx = 0, ulAutoTrackNum = 0; ulIdx < NUM_VIEW_ACTION; ulIdx++)
	{
		if (TRUE == pstContext->bTsrAutoTrack[ulViewId])
		{
			ulAutoTrackNum ++;
		}
	}

	if ((TRUE == bTsrAutoTrack) && (0 == ulAutoTrackNum))
	{
		// Create the TSR Auto Tracking Procedure
		(void)xmgr_pvr_CreateAutoTrackProc_Base();
	}

	BUS_SendMessage(NULL, eMEVT_PVR_TSR_SET_LIVE_TSR, HANDLE_NULL, (HINT32)ulViewId, (HINT32)bTsrAutoTrack, 0);

	if ((TRUE != bTsrAutoTrack) && (1 == ulAutoTrackNum))
	{
		// Destroy the TSR Auto Tracking Procedure
		(void)xmgr_pvr_DestroyAutoTrackProc_Base();
	}

	return ERR_OK;
}

HERROR xmgr_pvr_StartTsrRecording_Base (HUINT32 ulRelatedViewId, Handle_t hMasterSvc, Handle_t hSuppleSvc, HINT32 eSessionType, MgrPvr_TsrType_e eTsrType, HUINT32 ulStartTime)
{
	XmgrPvr_ActionType_e	 ePvrActionType;
	XmgrPvr_TsrRequest_t	 stTsrRecRequest;

	if ((HANDLE_NULL != hMasterSvc) && (HANDLE_NULL == hSuppleSvc))
	{
		hSuppleSvc = hMasterSvc;
	}
	else if ((HANDLE_NULL == hMasterSvc) && (HANDLE_NULL != hSuppleSvc))
	{
		hMasterSvc = hSuppleSvc;
	}
	else if ((HANDLE_NULL == hMasterSvc) && (HANDLE_NULL == hSuppleSvc))
	{
		HxLOG_Error("Master SVC and Supplementary SVC NULL\n");
		return ERR_FAIL;
	}

	switch (eTsrType)
	{
	case eTsrType_LiveTracing:
		ePvrActionType = ePvrActType_TSR_Normal;
		break;

	case eTsrType_Streaming:
		ePvrActionType = ePvrActType_TSR_LiveStream;
		break;

	case eTsrType_ExternalRequest:
		ePvrActionType = ePvrActType_TSR_Extern;
		break;

	default:
		HxLOG_Error("TSR Type wrong... (%d)\n", eTsrType);
		return ERR_FAIL;
	}

	if ((ePvrActType_TSR_Normal != ePvrActionType) || (ulRelatedViewId >= NUM_VIEW_ACTION))
	{
		ulRelatedViewId = HANDLE_NULL;
	}


	HxSTD_MemSet(&stTsrRecRequest, 0, sizeof(XmgrPvr_TsrRequest_t));
	stTsrRecRequest.ulRelatedViewId		= ulRelatedViewId;
	stTsrRecRequest.hMasterSvc			= hMasterSvc;
	stTsrRecRequest.hSuppleSvc			= hSuppleSvc;
	stTsrRecRequest.eSessionType		= eSessionType;
	stTsrRecRequest.eTsrActionType		= ePvrActionType;
	stTsrRecRequest.ulStartTime			= ulStartTime;
	stTsrRecRequest.hResult				= ERR_FAIL;

	BUS_SendData(NULL, eMEVT_PVR_RECORD_START_TSR, HANDLE_NULL, HANDLE_NULL, (HINT32)hMasterSvc, (void *)&stTsrRecRequest, sizeof(XmgrPvr_TsrRequest_t));

	return stTsrRecRequest.hResult;
}

HERROR xmgr_pvr_StopTsrRecording_Base (Handle_t hMasterSvc, Handle_t hSuppleSvc, MgrPvr_TsrType_e eTsrType)
{
	XmgrPvr_ActionType_e	 ePvrActionType;
	XmgrPvr_TsrRequest_t	 stTsrRecRequest;

	if ((HANDLE_NULL != hMasterSvc) && (HANDLE_NULL == hSuppleSvc))
	{
		hSuppleSvc = hMasterSvc;
	}
	else if ((HANDLE_NULL == hMasterSvc) && (HANDLE_NULL != hSuppleSvc))
	{
		hMasterSvc = hSuppleSvc;
	}
	else if ((HANDLE_NULL == hMasterSvc) && (HANDLE_NULL == hSuppleSvc))
	{
		HxLOG_Error("Master SVC and Supplementary SVC NULL\n");
		return ERR_FAIL;
	}

	switch (eTsrType)
	{
	case eTsrType_LiveTracing:
		ePvrActionType = ePvrActType_TSR_Normal;
		break;

	case eTsrType_Streaming:
		ePvrActionType = ePvrActType_TSR_LiveStream;
		break;

	case eTsrType_ExternalRequest:
		ePvrActionType = ePvrActType_TSR_Extern;
		break;

	default:
		HxLOG_Error("TSR Type wrong... (%d)\n", eTsrType);
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stTsrRecRequest, 0, sizeof(XmgrPvr_TsrRequest_t));
	stTsrRecRequest.hMasterSvc			= hMasterSvc;
	stTsrRecRequest.hSuppleSvc			= hSuppleSvc;
	stTsrRecRequest.eTsrActionType		= ePvrActionType;
	stTsrRecRequest.hResult				= ERR_FAIL;

	BUS_SendData(NULL, eMEVT_PVR_RECORD_STOP_TSR, HANDLE_NULL, (HINT32)ePvrActionType, (HINT32)hMasterSvc, (void *)&stTsrRecRequest, sizeof(XmgrPvr_TsrRequest_t));

	return stTsrRecRequest.hResult;
}

HERROR xmgr_pvr_GetTsrRecActionId_Base(Handle_t hSvcHandle, HUINT32 *pulActionId)
{
	HUINT32					 ulIndex;
	XmgrPvr_Action_t		*pstAction;
	XmgrPvr_Context_t		*pstContext;

	if (pulActionId != NULL)				{ *pulActionId = (HUINT32)-1; }

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)					{ return ERR_FAIL; }

	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if (((pstAction->eType & ePvrActType_MASK) == ePvrActType_TSR) &&
			((HANDLE_NULL == hSvcHandle) || (pstAction->hCurrSvcHandle == hSvcHandle)) &&
			(pstAction->eState == ePvrActState_Working))
		{
			if (pulActionId != NULL)				{ *pulActionId = pstAction->ulActionId; }
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#define _____TSR_LIVE_STREAMING_FUNCTION_____
HERROR xmgr_pvr_StartLiveStreaming_Base(Handle_t hSvcHandle, HUINT32 *pulActionId, Handle_t *phLiveStream)
{
	HUINT32				 ulActionId = HANDLE_NULL;
	Handle_t			 hLiveStream = HANDLE_NULL;
	HERROR				 hErr;

	hErr = xmgr_pvrbase_startTsrLiveStreaming(hSvcHandle, &ulActionId, &hLiveStream);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_pvrbase_startTsrLiveStreaming err(0x%08x)\n", hErr);

		if (NULL != pulActionId)					{ *pulActionId = HANDLE_NULL; }
		if (NULL != phLiveStream)					{ *phLiveStream = HANDLE_NULL; }
		return ERR_FAIL;
	}

	if (NULL != pulActionId)					{ *pulActionId = ulActionId; }
	if (NULL != phLiveStream)					{ *phLiveStream = hLiveStream; }
	return ERR_OK;
}

HERROR xmgr_pvr_StopLiveStreaming_Base(Handle_t hLiveStream)
{
	HERROR				 hErr;

	hErr = xmgr_pvrbase_stopTsrLiveStreaming(hLiveStream);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_pvrbase_startTsrLiveStreaming err(0x%08x)\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}


#define _____HDD_RELATED_FUNCTION_____



HBOOL xmgr_pvr_CheckPVRDeivceUsed_Base(HUINT32 ulPvrIdx)
{
	HUINT32					 ulIndex;
	XmgrPvr_Action_t			*pstAction;
	XmgrPvr_Context_t		*pstContext;

	pstContext = xmgr_pvr_GetContext();

	// Check Playback
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);

		if(pstAction->ulPvrIdx == ulPvrIdx && pstAction->eState != ePvrActState_None)
		{
			return TRUE;
		}
	}

	// Check Record
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if((pstAction->ulPvrIdx == ulPvrIdx ) &&
		 	((pstAction->eType & ePvrActType_MASK) == ePvrActType_REC) &&
			(pstAction->eState != ePvrActState_None)	)
		{
			return TRUE;
		}

	}

	return FALSE;
}


HERROR xmgr_pvr_StopPvrFunction_Base(HUINT32 ulPvrIdx)
{
	return xmgr_pvrbase_stopPvrFunction(ulPvrIdx);
}

#define _____DEBUG_FUNCTION_____

#if defined(CONFIG_DEBUG)
STATIC HUINT8 *xmgr_PVR_BASE_GetPvrActionTypeStr(XmgrPvr_ActionType_e ePvrActType)
{
	switch (ePvrActType)
	{
	case ePvrActType_None:
		return "NONE";

	case ePvrActType_PB:
		return "Playback(NO Proper)";

	case ePvrActType_PB_Normal:
		return "Playback(Normal)";

	case ePvrActType_PB_Chase:
		return "Playback(Chase)";

	case ePvrActType_PB_Tsr:
		return "Playback(TSR)";

	case ePvrActType_REC:
		return "Record(NO Proper)";

	case ePvrActType_REC_Normal:
		return "Record(Normal)";

	case ePvrActType_REC_EvtReserved:
		return "Record(Event/Reserved)";

	case ePvrActType_REC_Delayed:
		return "Record(Delayed Rec)";

	case ePvrActType_TSR:
		return "TSR (NO Proper)";

	case ePvrActType_TSR_Normal:
		return "TSR (Normal)";

	case ePvrActType_TSR_LiveStream:
		return "TSR (Live Stream)";

	case ePvrActType_TSR_Extern:
		return "TSR (Extern)";

	default:
		break;
	}

	return "Unknown";
}

STATIC HUINT8 *xmgr_PVR_BASE_GetPvrActionStateStr(XmgrPvr_ActionState_e ePvrActState)
{
	switch (ePvrActState)
	{
	case ePvrActState_None:
		return "NONE";

	case ePvrActState_WaitingByTimer:
		return "TIMER WAIT";

	case ePvrActState_Working:
		return "EXECUTING";

	default:
		break;
	}

	return "Unknown";
}

STATIC HUINT8 *xmgr_PVR_BASE_GetPvrDevStateString(XmgrPvr_DeviceState_e eDevState)
{
	switch (eDevState)
	{
	case ePvrDevState_NoDevice:
		return "No Device";

	case ePvrDevState_Malfunction:
		return "Malfunction";

	case ePvrDevState_NeedToFormat:
		return "Need to Format";

	case ePvrDevState_Formatting:
		return "Formatting";

	case ePvrDevState_Switching:
		return "Switching";

	case ePvrDevState_OK:
		return "No Problem";

	default:
		break;
	}

	return "Unknown Error";
}


HERROR xmgr_pvr_PrintPvrStatus_Base(void)
{
	HUINT32					 ulIndex;
	XmgrPvr_Action_t			*pstAction;
	XmgrPvr_Context_t		*pstContext;
	DbSvc_Info_t				 stSvcInfo;
	HERROR					 hErr;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)					{ return ERR_FAIL; }

	CMD_PRINT("=====================================================================\n");
	CMD_PRINT(" 	PVR Manager \n");

	for (ulIndex = 0 ; ulIndex < MAX_CONNECTABLE_PVR_STORAGE_NUM ; ulIndex++)
	{
//		CMD_PRINT("[%d]\n", ulIndex);
		CMD_PRINT("[%d]  PVR Device Status    : %s\n", ulIndex, xmgr_PVR_BASE_GetPvrDevStateString(pstContext->ePvrDevState[ulIndex]));
		CMD_PRINT("    PVR Device Index     : %d\n", pstContext->ePvrDevIdx[ulIndex]);
		CMD_PRINT("    PVR Partition Index  : %d\n\n", pstContext->ulPvrPartIdx[ulIndex]);
	}

	CMD_PRINT("---------------------------------------------------------------------\n");
	CMD_PRINT("Playback:\n");

	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);
		if (pstAction->eState != ePvrActState_None)
		{
			CMD_PRINT("\tPVR Action Type [%s] State [%s]\n", xmgr_PVR_BASE_GetPvrActionTypeStr(pstAction->eType),
															xmgr_PVR_BASE_GetPvrActionStateStr(pstAction->eState));
			CMD_PRINT("\tAction ID (0x%08x), Related Action ID (0x%08x) \n", pstAction->ulActionId, pstAction->ulRelatedActId);
			CMD_PRINT("\tRelated View ID (0x%08x)\n", pstAction->ulRelatedViewId);

			hErr = DB_SVC_GetServiceInfo(pstAction->hMasterSvcHandle, &stSvcInfo);
			if (hErr == ERR_OK)
			{
				CMD_PRINT("\tMaster Svc  [%04d:%s]\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
			}
			else
			{
				CMD_PRINT("\tMaster Svc  [XXXX:Unknown(0x%08x)]\n", pstAction->hMasterSvcHandle);
			}

			hErr = DB_SVC_GetServiceInfo(pstAction->hCurrSvcHandle, &stSvcInfo);
			if (hErr == ERR_OK)
			{
				CMD_PRINT("\tCurrent Svc [%04d:%s]\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
			}
			else
			{
				CMD_PRINT("\tCurrent Svc [XXXX:Unknown(0x%08x)]\n", pstAction->hCurrSvcHandle);
			}
		}
	}

	CMD_PRINT("\n");
	CMD_PRINT("Record:\n");

	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if (((pstAction->eType & ePvrActType_MASK) != ePvrActType_REC) ||
			(pstAction->eState == ePvrActState_None))
		{
			continue;
		}

		CMD_PRINT("\tPVR Action Type [%s] State [%s]\n", xmgr_PVR_BASE_GetPvrActionTypeStr(pstAction->eType),
														xmgr_PVR_BASE_GetPvrActionStateStr(pstAction->eState));
		CMD_PRINT("\tAction ID (0x%08x), Related Action ID (0x%08x) \n", pstAction->ulActionId, pstAction->ulRelatedActId);
		CMD_PRINT("\tRelated View ID (0x%08x)\n", pstAction->ulRelatedViewId);

		hErr = DB_SVC_GetServiceInfo(pstAction->hMasterSvcHandle, &stSvcInfo);
		if (hErr == ERR_OK)
		{
			CMD_PRINT("\tMaster Svc  [%04d:%s]\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
		}
		else
		{
			CMD_PRINT("\tMaster Svc  [XXXX:Unknown(0x%08x)]\n", pstAction->hMasterSvcHandle);
		}

		hErr = DB_SVC_GetServiceInfo(pstAction->hCurrSvcHandle, &stSvcInfo);
		if (hErr == ERR_OK)
		{
			CMD_PRINT("\tCurrent Svc [%04d:%s]\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
		}
		else
		{
			CMD_PRINT("\tCurrent Svc [XXXX:Unknown(0x%08x)]\n", pstAction->hCurrSvcHandle);
		}
	}

	CMD_PRINT("\n");
	CMD_PRINT("TSR:\n");

	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astRecActItem[ulIndex]);
		if (((pstAction->eType & ePvrActType_MASK) != ePvrActType_TSR) ||
			(pstAction->eState == ePvrActState_None))
		{
			continue;
		}

		CMD_PRINT("\tPVR Action Type [%s] State [%s]\n", xmgr_PVR_BASE_GetPvrActionTypeStr(pstAction->eType),
														xmgr_PVR_BASE_GetPvrActionStateStr(pstAction->eState));
		CMD_PRINT("\tAction ID (0x%08x), Related Action ID (0x%08x) \n", pstAction->ulActionId, pstAction->ulRelatedActId);
		CMD_PRINT("\tRelated View ID (0x%08x)\n", pstAction->ulRelatedViewId);

		hErr = DB_SVC_GetServiceInfo(pstAction->hMasterSvcHandle, &stSvcInfo);
		if (hErr == ERR_OK)
		{
			CMD_PRINT("\tMaster Svc  [%04d:%s]\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
		}
		else
		{
			CMD_PRINT("\tMaster Svc  [XXXX:Unknown(0x%08x)]\n", pstAction->hMasterSvcHandle);
		}

		hErr = DB_SVC_GetServiceInfo(pstAction->hCurrSvcHandle, &stSvcInfo);
		if (hErr == ERR_OK)
		{
			CMD_PRINT("\tCurrent Svc [%04d:%s]\n", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
		}
		else
		{
			CMD_PRINT("\tCurrent Svc [XXXX:Unknown(0x%08x)]\n", pstAction->hCurrSvcHandle);
		}
	}

	CMD_PRINT("=====================================================================\n");

	return ERR_OK;
}
#endif

