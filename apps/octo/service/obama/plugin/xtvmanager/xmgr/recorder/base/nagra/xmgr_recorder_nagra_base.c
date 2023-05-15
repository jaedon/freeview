
#if defined(CONFIG_MW_CAS_NAGRA)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <svc_si.h>
#include <svc_fs.h>
#include <svc_cas.h>
#include <svc_pipe.h>
#include <svc_sys.h>
#include <linkedlist.h>
#include <bus.h>

#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_recorder.h>
#include <mgr_storage.h>

#include <_xmgr_recorder.h>
#include "../local_include/_xmgr_recorder_base.h"

#include <xmgr_cas.h>

#include <namgr_main.h>

#define CONFIG_RECORD_PAUSE	// 나중에 KCONFIG로 추가하던지 하자.

HBOOL xmgr_rec_NagraIsReceivedNewCasDrmInfo(xmgrRec_Context_t *pstContext)
{
	HBOOL bChangedDrm = FALSE;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return FALSE;
	}

#if 1 // TODO : ucControlByteC가 CHANGE되어 bChangedDrm이 TRUE도 진행된다. 추후 ucControlByteC를 제거한후 아래로 다시 변경하자.
	if( (pstContext->stNewDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA != pstContext->stMetaInfo.stDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA) ||
	    (pstContext->stNewDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB != pstContext->stMetaInfo.stDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB) )
#else
	// recording 도중 drm 정보가 변경된 경우
	if(HxSTD_MemCmp(&(pstContext->stNewDrmInfo.stCasCcInfo), &(pstContext->stMetaInfo.stDrmInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t)) != 0)
#endif
	{
		bChangedDrm = TRUE;
	}

	return bChangedDrm;
}

BUS_Result_t xmgr_rec_NagraMsgEvtCasDrmSignalled_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3) // ok
{
	HBOOL 					bChangedDrm = FALSE;
	DxCopyrightControl_t	stDrmInfo;
	DxCopyrightControl_CasCcType_e			eCasCcType;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if (pstContext->eRecState == eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	eCasCcType = stDrmInfo.stCasCcInfo.eCasCcType;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
	{
		HxLOG_Error("eCasCcType(%d) is error\n", eCasCcType);
		return ERR_BUS_NO_ERROR;
	}
#else
	if(eCasCcType != eDxCopyrightControl_CasCcType_Nagra)
	{
		HxLOG_Error("eCasCcType(%d) is error\n", eCasCcType);
		return ERR_BUS_NO_ERROR;
	}
#endif

	pstContext->bCasDrmValid = TRUE;
	pstContext->bCasDrmNone = FALSE;
	xmgr_rec_ClearWaitFlag(pstContext,eRecPause_DRM);

	stDrmInfo.bCasCc = TRUE;
	pstContext->stNewDrmInfo.bCasCc = stDrmInfo.bCasCc;
	pstContext->stMetaInfo.stDrmInfo.bCasCc = stDrmInfo.bCasCc;

	HxSTD_MemSet(&(pstContext->stNewDrmInfo.stCasCcInfo), 0, sizeof(DxCopyrightControl_CasCcInfo_t));
#if defined (CONFIG_OP_SES)
//	HxSTD_MemSet(&(pstContext->stMetaInfo.stDrmInfo.stCasCcInfo), 0, sizeof(DxCopyrightControl_CasCcInfo_t));
	HxSTD_MemCopy(&(pstContext->stNewDrmInfo.stCasCcInfo), &stDrmInfo.stCasCcInfo, sizeof(DxCopyrightControl_CasCcInfo_t));
	HxLOG_Debug("eCasCcType(%d, %d), A(0x%x, 0x%x), B(0x%x, 0x%x)\n",
		pstContext->stNewDrmInfo.stCasCcInfo.eCasCcType, pstContext->stMetaInfo.stDrmInfo.stCasCcInfo.eCasCcType,
		pstContext->stNewDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA, pstContext->stMetaInfo.stDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA,
		pstContext->stNewDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB, pstContext->stMetaInfo.stDrmInfo.stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB);
#else
	HxSTD_MemSet(&(pstContext->stMetaInfo.stDrmInfo.stCasCcInfo), 0, sizeof(DxCopyrightControl_CasCcInfo_t));

	HxSTD_MemCopy(&(pstContext->stNewDrmInfo.stCasCcInfo), &stDrmInfo.stCasCcInfo, sizeof(DxCopyrightControl_CasCcInfo_t));
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
	// DRM 정보가 변경이 된경우.
	bChangedDrm = xmgr_rec_NagraIsReceivedNewCasDrmInfo(pstContext);

	HxSTD_MemCopy(&(pstContext->stMetaInfo.stDrmInfo.stCasCcInfo), &stDrmInfo.stCasCcInfo, sizeof(DxCopyrightControl_CasCcInfo_t)); // TODO  다시 확인 필요.

	HxLOG_Debug("eCasCcType(%d)\n", eCasCcType);

	switch(eCasCcType)
	{
		case eDxCopyrightControl_CasCcType_NagraSes:
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(pstContext, hAction, bChangedDrm);
#endif
			break;
		case eDxCopyrightControl_CasCcType_Nagra:
			// TODO
			break;
		default:
			break;
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_NagraMsgEvtCasOK_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("+\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

#if 0 // TODO : 확인후 적용해야함.
	if(pstContext->stSetup.eRecType == eMgrRecType_SCR2DESC)
	{
		if( 0 != p1 )	pstContext->nCasErrCode = p1;
		else			pstContext->nCasErrCode = 0;

		HxLOG_Debug("eRecType == eMgrRecType_SCR2DESC\n");
		return ERR_BUS_NO_ERROR;
	}
#endif

	/* context update */
	pstContext->bCasOk = TRUE;

	if( 0 != p1 )	pstContext->nCasErrCode = p1;
	else			pstContext->nCasErrCode = 0;

	// Code 상으로 보면 OP SES 사양때문에 xmgr_rec_NagraMsgEvtCasOK_Ses(pstContext, hAction); 로 가는 걸로 되어 있는데
	// 현재 HUMAX Scenario로 따지면 아닌 경우가 맞다.
	// 이에 대해 정확히 spec을 알아 오기 전까지는 시나리오에 맞추기로 하자.
#if 0
//#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	xmgr_rec_NagraMsgEvtCasOK_Ses(pstContext, hAction);
#else
	#if defined(CONFIG_RECORD_PAUSE)
	// CI Only 모델이 아닌 경우, CAS 로 Descramble 하지 못하는 경우 Record Pause 를 한 경우이므로 Record Resume 으로 살려야 한다.
	if(pstContext->eRecState == eMgrRecState_BLOCKED)
	{
		xmgr_rec_ClearPauseFlag(pstContext,eRecPause_CAS);
	}
	#endif
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_NagraMsgEvtCasFail_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NaAlarmData_t *pstNaAlarm = (NaAlarmData_t *)p2;

	HxLOG_Debug("+\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if(pstContext->eRecState == eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return MESSAGE_PASS;
	}

#if 0 // TODO : 확인후 적용해야함.
	if(pstContext->stSetup.eRecType == eMgrRecType_SCR2DESC)
	{
		pstContext->nCasErrCode = p1;
		pstContext->bCasOk = FALSE;

		HxLOG_Debug("eRecType == eMgrRecType_SCR2DESC\n");
		return MESSAGE_PASS;
	}
#endif

	pstContext->nCasErrCode = p1;
	pstContext->bCasOk = FALSE;

	// Code 상으로 보면 OP SES 사양때문에 xmgr_rec_NagraMsgEvtCasFail_Ses(pstContext, hAction); 로 가는 걸로 되어 있는데
	// 현재 HUMAX Scenario로 따지면 아닌 경우가 맞다.
	// 이에 대해 정확히 spec을 알아 오기 전까지는 시나리오에 맞추기로 하자.
#if 0
//#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	xmgr_rec_NagraMsgEvtCasFail_Ses(pstContext, hAction);
#else
	#if defined(CONFIG_RECORD_PAUSE)
	#if defined(CONFIG_MW_CI)
	/* CI 를 지원하는 모델인 경우 Descramble 하지 못하더라도 Scrambled Recording을 지원하기 때문에 Pause를하지 않고, Scramble 된 상태로 녹화 */
	#else
	/* CI 모델이 아닌 경우, CAS 로 Descramble 하지 못하는 경우 Record Pause*/
	if(pstContext->eRecState == eMgrRecState_RECORDING)
	{
		xmgr_rec_SetPauseFlag(pstContext, eRecPause_CAS);
	}
	#endif
	#endif
#endif

	if(pstNaAlarm != NULL)
	{
		if(pstNaAlarm->pstScAlarmItems != NULL)
		{
			;// TODO:
		}

		if(pstNaAlarm->pstAccessAlarmItems != NULL)
		{
			switch(pstNaAlarm->pstAccessAlarmItems->eAlarm)
			{
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
				case CA_ACCESS_DENIED_PARENTAL_CONTROL:
					pstContext->stMetaInfo.u1cLocked = TRUE;
					SVC_META_SetRecordInfo(pstContext->ulContentId, &pstContext->stMetaInfo);
					break;
#endif
				default:
					break;
			}
		}
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmUnknown_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t rst = ERR_BUS_NO_ERROR;

	HxLOG_Debug("+\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if(pstContext->eRecState== eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return MESSAGE_PASS;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	rst = xmgr_rec_NagraMsgEvtCasDrmUnknown_Ses(pstContext, hAction, p1, p2, p3);
#else
	// DRM 을 알 수 없으므로 scramble recording 시작한다.
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

		/* start the recorder */
		xmgr_rec_StartRec(pstContext);
		return MESSAGE_BREAK;
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return rst;
}

BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmNone_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t rst = MESSAGE_PASS;

	HxLOG_Debug("eRecState(%d)\n", pstContext->eRecState);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if(pstContext->eRecState== eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return MESSAGE_PASS;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	rst = xmgr_rec_NagraMsgEvtCasDrmNone_Ses(pstContext, hAction, p1, p2, p3);
#else
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);
		xmgr_rec_StartRec(pstContext);
		return MESSAGE_BREAK;
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return rst;
}

STATIC BUS_Result_t xproc_rec_NagraProto_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
		// BASE RECODER PROC 을 이용하고, CAS & DRM EVENT만 처리하자.
#else
		/*MSG*/
		case eMEVT_BUS_READY_SHUTDOWN :
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eMEVT_BUS_READY_SHUTDOWN\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusGoStandby_Base(pstContext, hAction, p1, p2, p3);
			return MESSAGE_PASS;

		case eMEVT_BUS_CREATE:
			HxLOG_Warning ("********************************************************************************\n");
			HxLOG_Warning ("*  XMGR : RECORDING : CREATE \n");
			HxLOG_Warning ("********************************************************************************\n");

			xmgr_rec_SetWaitFlag(pstContext,eRecWait_DRM);
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_PMT);

			HxLOG_Debug ("[XMGR:RecNagra(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);

			HxLOG_Warning ("********************************************************************************\n");
			HxLOG_Warning ("*  XMGR : RECORDING : DESTROY \n");
			HxLOG_Warning ("********************************************************************************\n");
			break;

		case eMEVT_BUS_TIMER:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eMEVT_BUS_TIMER\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusTimer_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD\n", ulActionId);
			eBusResult = xmgr_rec_MsgMgrRecordChangeTsrRecToDelayedRecord_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_ENDTIME_UPDATE :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eMEVT_PVR_RECORD_ENDTIME_UPDATE\n", ulActionId);
			eBusResult = xmgr_rec_MsgMgrRecordEndtimeUpdate_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-SI*/
		case eSEVT_SI_PMT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PAT_TIMEOUT :
		case eSEVT_SI_PMT_TIMEOUT :
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_PAT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_PMT_PIDCHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_EIT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiEitReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_EIT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiEitTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_MULTIFEED_DETECTED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_MULTIFEED_DETECTED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiMultifeedDetected_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_FREE_CA_MODE_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_SDT_FREE_CA_MODE_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiSdtFreeCaModeChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_RUNNING_STATUS_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_SI_SDT_RUNNING_STATUS_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiSdtRunningStatusChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-CH*/
		case eSEVT_CH_NO_SIGNAL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CH_NO_SIGNAL\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChNoSignal_Base(pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CH_MOVING_MOTOR :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CH_MOVING_MOTOR\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChMovingMotor_Base(pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CH_LOCKED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CH_LOCKED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChLocked_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_STORAGE_NOTIFY_UNPLUGGED:
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : EVT_FS_NOTIFY_USB_DISCONNECTED\n", ulActionId);
			if (p2 == FALSE)	// Ext Hdd
			{
				eBusResult = xmgr_rec_MsgEvtExtStorageDisconnected_Base (pstContext, hAction, p1, p2, p3);
			}
			break;

		/* EVT-DB */
		case eSEVT_DB_CHECK_SVC_DB :
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_DB_CHECK_SVC_DB\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtDbCheckSvcDb_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-SVC_Rec */
		case eSEVT_REC_PID_CHANGED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_REC_PID_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecPidChanged_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_TIMECODE_CHANGE :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtRecTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_SCRAMBLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_REC_SCRAMBLED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecScrambled_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecDelayedRecStatusChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-Xmgr_Rec */
		case eSEVT_REC_REQ_COMMAND:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_REC_REQ_COMMAND\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtReqCommand_Base (pstContext, hAction, p1, p2, p3);
			break;
#endif
		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
			eBusResult = xmgr_rec_NagraMsgEvtCasDrmSignalled_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_OK :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_rec_NagraMsgEvtCasOK_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_rec_NagraMsgEvtCasFail_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_UNKNOWN:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CAS_DRM_UNKNOWN\n", ulActionId);
			eBusResult = xmgr_rec_NagraMsgEvtCasDrmUnknown_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_NONE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecNagra(%d)] MESSAGE : eSEVT_CAS_DRM_NONE\n", ulActionId);
			eBusResult = xmgr_rec_NagraMsgEvtCasDrmNone_Base(pstContext, hAction, p1, p2, p3);
			break;

		default :
			break;
	}

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	return eBusResult;
#else
	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}
	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
#endif
}


STATIC BUS_Result_t xproc_rec_NagraAction0_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (0);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_NagraAction1_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (1);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_NagraAction2_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (2);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_NagraAction3_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (3);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_NagraAction4_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (4);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_NagraAction5_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (5);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_NagraAction6_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (6);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_NagraAction7_Base (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (7);

	return xproc_rec_NagraProto_Base (pstContext, message, hAction, p1, p2, p3);
}

#define _____MSG_PROCESSING_FUNCTIONS_____
BUS_Callback_t xmgr_recorder_GetNagraProc_Base (HUINT32 ulActionId)
{
	// Code 내에서 Record 갯수가 최대 몇개 인지 알기 힘드므로
	// switch 함수 위에 ulActionID 영역을 확인하여 실제로 넘치는 값이어도 logic상 들어오지 않게 만든다.
	if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		switch (ulActionId)
		{
		case (eActionId_REC_FIRST + 0):
			return xproc_rec_NagraAction0_Base;

		case (eActionId_REC_FIRST + 1):
			return xproc_rec_NagraAction1_Base;

		case (eActionId_REC_FIRST + 2):
			return xproc_rec_NagraAction2_Base;

		case (eActionId_REC_FIRST + 3):
			return xproc_rec_NagraAction3_Base;

		case (eActionId_REC_FIRST + 4):
			return xproc_rec_NagraAction4_Base;

		case (eActionId_REC_FIRST + 5):
			return xproc_rec_NagraAction5_Base;

		case (eActionId_REC_FIRST + 6):
			return xproc_rec_NagraAction6_Base;

		case (eActionId_REC_FIRST + 7):
			return xproc_rec_NagraAction7_Base;

		default:
			break;
		}
	}

	return NULL;
}

BUS_Result_t xmgr_rec_NagraProto_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return xproc_rec_NagraProto_Base(pstContext, message, hAction, p1, p2, p3);
}
#endif
