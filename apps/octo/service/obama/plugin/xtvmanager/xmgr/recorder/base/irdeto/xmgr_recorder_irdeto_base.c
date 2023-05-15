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
//#include <ir_evt.h>

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
// TODO: [JHLEE] IRDETO UC Softcell Multi descramble �� �������� �ʾƼ� ��Ȯ�� �� �� ����. ���߿� ������ �Ѵ�.
#define CONFIG_MW_CAS_IRDETO_UCAS_RECORD_LIMIT
#endif

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define	__________________BODY______________________
/*
	Irdeto Digital MacroVision�� ���� ������.
	DRM�ʵ带 �׳� ����ϱ�� ��.
*/

STATIC HERROR xmgr_rec_SetDrm(xmgrRec_Context_t *pstContext)
{
	/* eMgrRecState_WAIT ������ ���(record start point)���� ���� �� */

	HERROR	hErr = ERR_FAIL;
	pstContext->stNewDrmInfo.stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_Irdeto;

	if(pstContext->eRecState == eMgrRecState_WAIT)	// recording start
	{
		/* set drm for recorder */
		hErr = SVC_REC_SetDRMInfo(pstContext->hAction, &pstContext->stNewDrmInfo, TRUE);

		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_DRM);

		/* start the recorder */
		xmgr_rec_StartRec(pstContext);
	}
	else
	{
		/* �� �Լ�(xmgr_rec_SetDrm)�� eDxCopyrightControl_CasCcType_CiPlus & eMgrRecState_WAIT ��쿡�� ��� ��. Ȥ�� ���Ŀ� �ٸ� ������ �� �Լ��� �θ��ٸ� ���� ���� �ʿ�.*/
		return ERR_FAIL;
	}
	return ERR_OK;
}


////////////////////////////////////////////////////////////////////////////////////
/* FTA����� ���, Irdeto�� ������ �ʰ� Play�Ǵ� ����.	*/
/////////////////////////////////////////////////////////////////////////////////////
STATIC BUS_Result_t xmgr_rec_EvtCasDigitalMVNone(xmgrRec_Context_t *context, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Info("[XMGR:Irdeto] : EvtCasDigitalMVNone\n");
#if	defined(CONFIG_MW_CAS_IRDETO_PVR)
	// TODO: Update Function
	if (context->bTunningUnlock == TRUE)
		return MESSAGE_PASS;

	context->bCasDrmValid = FALSE;
	context->bCasDrmNone = TRUE;
	xmgr_rec_ClearWaitFlag(pstContext,eRecPause_DRM);

	if (context->eRecState == eMgrRecState_WAIT)
		local_Record_Irdeto_StartRecord(context);

#endif

	return MESSAGE_PASS;
}

//////////////////////////////////////////////////////////////////////////////////////
/*		Irdeto�� �� �ļ� ������ ����..					*/
//////////////////////////////////////////////////////////////////////////////////////
STATIC BUS_Result_t xmgr_rec_EvtCasDigitalMVSignaled(xmgrRec_Context_t *context, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if	defined(CONFIG_MW_CAS_IRDETO_PVR)
	// TODO: Update Fuction
	MgrAction_Type_e			eActionType;

	HxLOG_Info("[XMGR:Irdeto] : EvtCasDigitalMVSignaled\n");

	if (context->bTunningUnlock == TRUE)
		return MESSAGE_PASS;

	context->bCasDrmValid	= TRUE;
	context->bCasDrmNone	= FALSE;
	xmgr_rec_ClearWaitFlag(pstContext,eRecPause_DRM);

	if (context->eRecState == eMgrRecState_WAIT)
	{
		switch (p1)
		{
			case	IRMV_STATUS_FTA:
			case	IRMV_STATUS_CLEAR:
				HxLOG_Info("[XMGR:Irdeto] : Recording is allowed without encryption!!\n");
				local_Record_Irdeto_StartRecord(context);
				break;

			case	IRMV_STATUS_ENCRYPTION:
				HxLOG_Info("[XMGR:Irdeto] : Recording is allowed with encryption!!\n");
				//	Encryption is done in Irdeto CAS module. so Just Start Recording...
				local_Record_Irdeto_StartRecord(context);
				break;

			case	IRMV_STATUS_NOMSK:
				HxLOG_Info("[XMGR:Irdeto] : Recording is not allowed due to NO_MSK!!\n");
				eActionType = MGR_ACTION_GetType(SVC_PIPE_GetActionId(context->hAction));
				if (eActionType == eRecType_TSR_REC)
				{
					HxLOG_Print("[XMGR:Irdeto] : TSR recording - Ignore NO MSK Message!!!\n");
					HxLOG_Print("         : TSR is in Pausing Mode!!!\n");
					break;
				}

				local_Record_Irdeto_StopRecord(context);
				break;

			//	forbidden message�� application���� ó�� �� �־�� ��. Irdeto Spec.�� �ƴ�.
			//	Ziggo���� ����ϴ°���. (Ziggo + Irdeto (beta??))
			case	IRMV_STATUS_FORBIDDEN:
				HxLOG_Info("[XMGR:Irdeto] : Recording is not allowed due to MACROVISION!!!\n");
				eActionType = MGR_ACTION_GetType(SVC_PIPE_GetActionId(context->hAction));
				if (eActionType == eRecType_TSR_REC)
				{
					HxLOG_Print("[XMGR:Irdeto)] : TSR recording - Ignore NO MSK Message!!!\n");
					HxLOG_Print("         : TSR is in Pausing Mode!!!\n");
					break;
				}

				//1 To Do : �ʿ��� UI�� �׸����� �� ���� �߰� ����.
				//xmgr_cas_IrCreateUI("IR Decoder PVR", APP_PVR_PRIORITY, eCasIr_UI_DecoderPVR, (HINT32)context->hAction, p1, IR_EVT_Forbidden_Rec, 0);
				local_Record_Irdeto_StopRecord(context);
				break;
		}
	} else
	{
		//	�̹� Recording�� ������ �Ǿ��ٸ�??
		//	����� FORBIDDEN�� NOMSK������ �ü� �ִ�.
		//	�ٸ����� �׳� Resume�� ����.
		switch (p1)
		{
			case	IRMV_STATUS_FTA:
			case	IRMV_STATUS_CLEAR:
				HxLOG_Info("[XMGR:Irdeto] : Encryption mode is changed --> FTA/CLEAR\n");
				xmgr_rec_ClearPauseFlag(context, eRecPause_CAS);
				break;

			case	IRMV_STATUS_ENCRYPTION:
				HxLOG_Info("[XMGR:Irdeto] : Encryption mode is changed --> ENCRYPTION\n");
				xmgr_rec_ClearPauseFlag(context, eRecPause_CAS);
				break;

			case	IRMV_STATUS_NOMSK:
				HxLOG_Info("[XMGR:Irdeto] : Encryption mode is changed --> NO_MSK\n");
				xmgr_rec_SetPauseFlag(context, eRecPause_CAS);
				break;

			case	IRMV_STATUS_FORBIDDEN:
				HxLOG_Info("[XMGR:Irdeto] : Encryption mode is changed --> FORBIDDEN\n");
				xmgr_rec_SetPauseFlag(context, eRecPause_CAS);
				break;
		}
	}
#endif
	return MESSAGE_PASS;
}


STATIC BUS_Result_t xmgr_rec_IrdetoMsgEvtCasDrmNone_IrdetoPvr(xmgrRec_Context_t *pstContext)
{
	HxLOG_Info("[XMGR:Irdeto] : EvtCasDigitalMVNone - Irdeto PVR\n");
#if defined(CONFIG_MW_CAS_IRDETO_PVR)
	// TODO : �߰� ���� �ʿ�.
	if (pstContext->bTunningUnlock == TRUE)
		return MESSAGE_PASS;

	pstContext->bCasDrmValid = FALSE;
	pstContext->bCasDrmNone = TRUE;
	xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

	if (pstContext->eRecState == eMgrRecState_WAIT)
		local_Record_Irdeto_StartRecord(pstContext);

	return MESSAGE_PASS;
#else
	return ERR_BUS_NO_ERROR;
#endif
}

STATIC BUS_Result_t xmgr_rec_IrdetoMsgEvtCasDrmNone_IrdetoHMXPvr(xmgrRec_Context_t *pstContext)
{
	HxLOG_Info("[XMGR:Irdeto] : EvtCasDigitalMVNone - Irdeto HUMAX PVR\n");

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

	// DRM ������ �����Ƿ� recording �����Ѵ�.
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

		/* start the recorder */
		xmgr_rec_StartRec(pstContext);
		return MESSAGE_BREAK;
	}

	return ERR_BUS_NO_ERROR;
}


#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

BUS_Result_t xmgr_rec_IrdetoMsgEvtCasDrmNone_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;

	HxLOG_Info("[XMGR:Irdeto] : EvtCasDigitalMVNone\n");

#if defined(CONFIG_MW_CAS_IRDETO_PVR)
	eBusResult = xmgr_rec_IrdetoMsgEvtCasDrmNone_IrdetoPvr(pstContext);
#else
	eBusResult = xmgr_rec_IrdetoMsgEvtCasDrmNone_IrdetoHMXPvr(pstContext);
#endif
	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return eBusResult;
}


BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasDrmUnknown_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t ulResult = ERR_BUS_NO_ERROR;

	HxLOG_Debug("+\n");

#if defined(CONFIG_IRDETO_HMX_PVR)
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

	// DRM �� �� �� �����Ƿ� scramble recording �����Ѵ�.
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

		/* start the recorder */
		xmgr_rec_StartRec(pstContext);
		return MESSAGE_BREAK;
	}
#endif
	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ulResult;
}


BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasOK_Base ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("+\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	/* context update */
	pstContext->bCasOk = TRUE;
/*
	if( 0 != p1 ) {
		pstContext->nCasErrCode = p1;
	} else {
		pstContext->nCasErrCode = 0;
	}
*/
#if 0//defined(CONFIG_RECORD_PAUSE)
	// CI Only ���� �ƴ� ���, CAS �� Descramble ���� ���ϴ� ��� Record Pause �� �� ����̹Ƿ� Record Resume ���� ����� �Ѵ�.
	if(pstContext->eRecState == eMgrRecState_BLOCKED)
	{
		xmgr_rec_ClearPauseFlag(pstContext,eRecPause_CAS);
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}


BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasFail_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
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

	//pstContext->nCasErrCode = p1;
	pstContext->bCasOk = FALSE;

#if 0//defined(CONFIG_RECORD_PAUSE)
	/* CI Only ���� �ƴ� ���, CAS �� Descramble ���� ���ϴ� ��� Record Pause*/
	if(pstContext->eRecState == eMgrRecState_RECORDING)
	{
		if(pstContext->bRecordStart == FALSE)
		{
			xmgr_rec_StartRec(pstContext);
			pstContext->bRecordStart = TRUE;
		}
		xmgr_rec_SetPauseFlag(pstContext, eRecPause_CAS);
	}
#endif

	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}


BUS_Result_t	xmgr_rec_IrdetoMsgEvtCasCmd_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_CAS_IRDETO_UCAS) && defined(CONFIG_MW_CAS_IRDETO_UCAS_RECORD_LIMIT)
	HERROR				hErr = ERR_FAIL;
	DxCAS_GroupId_e		eCasId = eDxCAS_GROUPID_NONE;
	SvcCas_ShowType_e	eCasShowType = eCasShowType_None;

	HxLOG_Debug("+\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	eCasId = (DxCAS_GroupId_e)p1;
	if (eCasId != eDxCAS_GROUPID_IR)
	{
		HxLOG_Error("Error!! Irdeto IR Only eCasId:[0x%x]\n", eCasId);
		return MESSAGE_PASS;
	}
	eCasShowType = (SvcCas_ShowType_e)p3;
	if (eCasShowType != eCasShowType_RecStop)
	{
		HxLOG_Error("Error!! It is not eCasShowType_RecStop!! eCasShowType_RecStop:[%d]\n", eCasShowType);
		return MESSAGE_PASS;
	}

	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);

		HxLOG_Error("xmgr_rec_StartRec() CHECK this log!!!\n");
		xmgr_rec_StartRec(pstContext);
	}

	if(pstContext->eRecState == eMgrRecState_RECORDING)
	{
		if(pstContext->stSetup.eRecType != eMgrRecType_TSR)
		{
			HxLOG_Error("Recording Fail!!! eRecType[%d]\n", pstContext->stSetup.eRecType);
		/* flag update */
		MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_NORIGHT_SCR);
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_NORIGHT_SCR, pstContext->stSetup.ulSlot, 0);
		BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
		}

		if(pstContext->stSetup.eRecType == eMgrRecType_TSR)
		{
			HxLOG_Error("TSR Fail!!! eRecType[%d]\n", pstContext->stSetup.eRecType);
		}
	}
#endif
	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}


BUS_Result_t xmgr_rec_IrdetoProtoProc_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_DRM);
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_PMT);
			break;

		case eSEVT_CAS_OK :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_rec_IrdetoMsgEvtCasOK_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_rec_IrdetoMsgEvtCasFail_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CMD:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_CMD\n", ulActionId);
			eBusResult = xmgr_rec_IrdetoMsgEvtCasCmd_Base(pstContext, hAction, p1, p2, p3);
			break;

		//	MW_CAS_MAIN���� FTA��� �Ǵ��ϰ� Digital MacroVision�� ���� �ʴ� ������.
		case eSEVT_CAS_DMV_NONE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_DMV_NONE\n", ulActionId);
			eBusResult = xmgr_rec_EvtCasDigitalMVNone(pstContext, p1, p2, p3);
			break;

		case eSEVT_CAS_DMV_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_DMV_SIGNALLED\n", ulActionId);
			eBusResult = xmgr_rec_EvtCasDigitalMVSignaled(pstContext, p1, p2, p3);
			break;

		case eSEVT_CAS_DMV_UNKNOWN:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_DMV_UNKNOWN\n", ulActionId);
			break;

		case eSEVT_CAS_DRM_UNKNOWN:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_DRM_UNKNOWN\n", ulActionId);
			eBusResult = xmgr_rec_IrdetoMsgEvtCasDrmUnknown_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_NONE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_CAS_DRM_NONE\n", ulActionId);
			eBusResult = xmgr_rec_IrdetoMsgEvtCasDrmNone_Base (pstContext, hAction, p1, p2, p3);
			break;
		/*EVT-SI*/
		case eSEVT_SI_PMT:
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			//eBusResult = xmgr_rec_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

#if	defined(CONFIG_MW_CAS_IRDETO_SDT_RUNNING_STATUS_BLOCK)
		//	SDT�� ���� ����. Watch TV���� ���� ��������� �ҵ�...
		case eSEVT_SI_SDT_RUNNING_STATUS_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_SI_SDT_RUNNING_STATUS_CHANGED\n", ulActionId);
			break;

		case eSEVT_SI_SDT_TIMEOUT :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Irdeto(%d)] MESSAGE : eSEVT_SI_SDT_TIMEOUT\n", ulActionId);
			break;
#endif
		default :
			break;
	}

	return eBusResult;
}


#define _________PLUGIN_FUNC__________
HBOOL	xmgr_recorder_IrdetoGetEncryptionMode_Base (MgrRec_RecSetup_t *pstSetup)
{
	HBOOL	bEncryption = FALSE;

	bEncryption = FALSE;

	/* TSR,Delayed Recording is always	Encryption Enable */
	HxLOG_Info("pstContext->stSetup.eRecType: %d\n", pstSetup->eRecType);
	if(eMgrRecType_TSR == pstSetup->eRecType || pstSetup->eRecType == eMgrRecType_DELAYED)
	{
		bEncryption = TRUE;
	}
	else
	{
		HERROR	hErr = ERR_FAIL;
		DbSvc_Info_t	stSvcInfo;

		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));

		/* FTA ä���̸� Encryption False, scramble ä���̸� Encryption True */
		hErr = DB_SVC_GetServiceInfo(pstSetup->hCurrSvc, &stSvcInfo);
		HxLOG_Info("hErr: %d, stSvcInfo.eCasType: %d \n", hErr, DbSvc_GetCasType(&stSvcInfo));
		if (ERR_OK == hErr)
		{
			bEncryption = (DbSvc_GetCasType(&stSvcInfo) == eDxCAS_TYPE_FTA) ? FALSE : TRUE;
		}
	}

	HxLOG_Info("bEncryption: %d \n", bEncryption);

	return bEncryption;
}



#if 0



#define _________LOCAL_FUNC__________
STATIC BUS_Result_t	local_Record_Irdeto_StartRecord(xmgrRec_Context_t *context)
{
	//	RecState�� DRM���� DigitalMacroVision Waiting�� �Ǵ��Ѵ�...
	if (context->eRecState == eMgrRecState_WAIT)
	{
		xmgr_pvr_RecordDestroyCurrentSvcTsr(context);
		MW_Rec_Start(context->hAction);
		context->bRecordStart = TRUE;
		BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_RECORD_STARTED, context->hAction, context->eRecType, context->ulContentId, context->ulSlot);

		if(context->eRecType != eMgrRecType_TSR)
		{
#if 0
			BUS_SendMessage (NULL, MSG_APPC2U_CREATE_REC_STATUS_WND,  0, 0, 0, 0);
#endif
		}
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t	local_Record_Irdeto_StopRecord(xmgrRec_Context_t *context)
{
	if (context->eRecType != eMgrRecType_TSR)
	{
		//	�Ϲ� record�� stop�ϰ� TSR����.
		//	Message�� ap_cas_ir���� ��� �ٰ��̰�...
		context->bDestroybyDrm = TRUE;

		//	Recording���� �϶� ���� ���� �Ƿ� End Condition�� �������� �ʴ´�.
		//	������ ��� History�м������� ȭ���� �������� �ʴ´�.
		//if((context->eRecType == eMgrRecType_EBR) ||(context->eRecType == eMgrRecType_TBR) || (context->eRecType == eMgrRecType_SBR))
		//	xmgr_pvr_SetRecordFailReason(context->ulContentId, eMgrRecFailed_UNKNOWN);

		HxLOG_Info("[local_Record_Irdeto_StopRecord] [eMgrRecState_WAIT] if(context->eRecType != eMgrRecType_TSR) 2 BUS_MGR_Destroy \n");

		BUS_MGR_Destroy(0);

#if 0
		// endtime â �ݱ�
		BUS_SendMessage (NULL, MSG_APPC2U_DESTROY_REC_EDIT_DURATION, 0, 0, 0, 0);
#endif
		return MESSAGE_BREAK;
	}
	else
	{
		//	TSR�̸� �� ����.
		return local_Record_Irdeto_StartRecord(context);
	}

	return MESSAGE_PASS;
}
#endif
