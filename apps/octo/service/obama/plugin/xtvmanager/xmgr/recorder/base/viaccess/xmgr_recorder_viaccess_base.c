/**
	@file     	ap_recorder_viaccess.c
	@brief    	recording control application.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#if defined(CONFIG_MW_CAS_VIACCESS)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <svc_si.h>
//#include <svc_pvr.h>
#include <svc_cas.h>
#include <svc_pipe.h>
#include <svc_epg.h>
#include <svc_sys.h>
#include <linkedlist.h>

#include <bus.h>

#include <mgr_copyrightcontrol.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include "../local_include/_xmgr_recorder_base.h"

#include <va_ui_adapt.h>
#include <xmgr_pvr.h>
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

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

#define _________PROTO_TYPE__________
STATIC BUS_Result_t xmgr_rec_EvtCasOK_Viaccess(xmgrRec_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_rec_EvtCasFail_Viaccess(xmgrRec_Context_t *pstContext, HUINT32 event, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC BUS_Result_t xmgr_rec_EvtCasDrmSignalled_Viaccess(xmgrRec_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_rec_EvtCasDrmNone_Viaccess_Viaccess(xmgrRec_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _________GLOBAL_FUNC__________

BUS_Result_t 	xproc_rec_Viaccess(xmgrRec_Context_t *pstContext,
									HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT8					ucRecIdx = ulActionId - eActionId_REC_FIRST;
	switch (message)
	{
		case eMEVT_BUS_DESTROY:
			if (pstContext->eRecType == eMgrRecType_TSR)
			{
#if 0
				BUS_SendMessage (NULL, MSG_APPC2U_RECORD_DISPLAY_MSG_INFO, eRecForbiddenCloseMsg, NULL, 0, 0);
#endif
			}

			break;

		case eSEVT_CAS_OK :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("MESSAGE : eSEVT_CAS_OK(error code : %d)\n", p1);
			eGwmResult = xmgr_rec_EvtCasOK_Viaccess(pstContext, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("MESSAGE : eSEVT_CAS_FAIL(error code : %d)\n", p1);
			eGwmResult = xmgr_rec_EvtCasFail_Viaccess(pstContext, message, p1, p2, p3);
			break;

		case eSEVT_CAS_CMD :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("MESSAGE : eSEVT_CAS_CMD(error code : %d)\n", p1);
			eGwmResult = xmgr_rec_EvtCasFail_Viaccess(pstContext, message, p1, p2, p3);
			break;

		/******************************************
			CAS DRM 도착 혹은 변경
		*******************************************/
		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[Viaccess]MESSAGE : eSEVT_CAS_DRM_SIGNALLED(DRM Type : %d)\n", p1);
			eGwmResult = xmgr_rec_EvtCasDrmSignalled_Viaccess(pstContext, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_NONE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[Viaccess]MESSAGE : eSEVT_CAS_DRM_NONE(DRM Type : %d)\n", p1);
			eGwmResult = xmgr_rec_EvtCasDrmNone_Viaccess(pstContext, p1, p2, p3);
			break;

		default :
			break;
	}

	// 향별 별도로 처리를 한 경우라면 ERR_BUS_NO_ERROR 가 아닌 값으로 처리해야 한다.
	if(eGwmResult != ERR_BUS_NO_ERROR)
	{
		return eGwmResult;
	}

	// 향별로 별도 처리하지 않는다면 공통으로 처리한다.
	return xproc_pvr_RecordProtoAction(pstContext, message, hAction, p1, p2, p3);
}

HBOOL	xmgr_rec_GetDrmCheckService_Viaccess(Handle_t hSvcHandle)
{
	HxLOG_Info("[Viaccess] : DRM Check Service - Always check...\n\t--> Ignore Start Record!!!\n");

	return TRUE;
}

#define _________LOCAL_FUNC__________

STATIC BUS_Result_t xmgr_rec_EvtCasOK_Viaccess(xmgrRec_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)p2;

	if(pstContext->bRecordStart == TRUE && pstContext->eRecState == eMgrRecState_BLOCKED)
	{
		xmgr_rec_ClearPauseFlag(pstContext, eRecPause_CAS);
	}

	NOT_USED_PARAM(p1);
//	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_rec_EvtCasFail_Viaccess(xmgrRec_Context_t *pstContext, HUINT32 event, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL bBlock = FALSE;
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)p2;

	if(pEvent != NULL && pstContext->bRecordStart == TRUE && pstContext->eRecState != eMgrRecState_BLOCKED)
	{
		// s/c 빼는 경우 뿐 아니라, fail되면 모두 녹화 정지.. (일부 경우는 제외)
		switch(pEvent->eEventType)
		{
		case VA_UI_NEW_OPERATOR_MESSAGE:
		case VA_UI_NEW_OPERATOR_DATA:
		case VA_UI_SMARTCARD_READY:
		case VA_UI_STOP_SECURITY_RESTRICTION:
		case VA_UI_CAMLOCK_NOT_ACTIVATED:
		case VA_UI_CAMLOCK_DISABLED:
		case VA_UI_CAMLOCK_ACTIVATED_OK:
#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
		case VA_UI_SHOW_FINGERPRINT:
		case VA_UI_HIDE_FINGERPRINT:
#endif
#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK
		case VA_UI_FREE_CHANNEL_BLOCK:
		case VA_UI_FREE_CHANNEL_BLOCK_STOP:
#endif
			bBlock = FALSE;
			break;

		default:
			bBlock = TRUE;
			break;
		}

		if (bBlock)
		{
			xmgr_rec_SetPauseFlag(pstContext, eRecPause_CAS);
		}
	}

	NOT_USED_PARAM(p1);
//	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

STATIC HERROR xmgr_rec_SetDrm_Viaccess(xmgrRec_Context_t *pstContext)
{
	HERROR			err;
	HBOOL			bSplit = FALSE;
	SvcMeta_Service_t	*pstSvcInfo = NULL;

	pstContext->stNewDrmInfo.eDrmType = eDxCopyrightControl_CasCcType_Viaccess;

	pstSvcInfo = (SvcMeta_Service_t *)OxAP_Malloc(sizeof(SvcMeta_Service_t));
	if(pstContext->eRecState == eMgrRecState_WAIT)	// recording start
	{
#if 0
		/* set drm for recorder */
		err = MW_Rec_SetDRMInfo(pstContext->hAction, &(pstContext->stNewDrmInfo), TRUE);
		HxLOG_Info("xmgr_rec_SetDrm_Viaccess(%d) : [eMgrRecState_WAIT] 1. MW_Rec_SetDRMInfo() eActionType=%x Handle_t=%x err=%d\n", pstContext->ucRecIdx, pstContext->eRecType, pstContext->hAction, err);
#endif

		// recording 불가인지 체크
		if (pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bRecordForbidden == TRUE)
		{
			if(pstContext->eRecType != eMgrRecType_TSR)
			{
				Handle_t hSvc;

				if (MGR_ACTION_GetMasterSvcHandle(MGR_ACTION_GetMainActionId(), &hSvc) == ERR_OK)
				{
					// 현재 화면과 같은 서비스인 경우만 화면에 메시지 표시
					if (pstContext->hMasterSvc == hSvc)
					{
#if 0
						BUS_SendMessage (NULL, MSG_APPC2U_RECORD_DISPLAY_MSG_INFO, eRecForbiddenRecordMsg, NULL, 0, 0);
#endif
					}
				}

				pstContext->bDestroybyDrm = TRUE;
				if((pstContext->eRecType == eMgrRecType_EBR) ||(pstContext->eRecType == eMgrRecType_TBR) || (pstContext->eRecType == eMgrRecType_SBR))
				{
					err = MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_UNKNOWN);
				}
				HxLOG_Info("[xmgr_rec_SetDrm_Viaccess] [eMgrRecState_WAIT] if(pstContext->eRecType != eMgrRecType_TSR) 2 BUS_MGR_Destroy \n");
				BUS_MGR_Destroy(0);
#if 0
				// endtime 창 닫기
				BUS_SendMessage (NULL, MSG_APPC2U_DESTROY_REC_EDIT_DURATION, 0, 0, 0, 0);
#endif
				return MESSAGE_BREAK;
			}
		}

		// TSR 불가인지 체크
		if (pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bTsrForbidden == TRUE)
		{
			if(pstContext->eRecType == eMgrRecType_TSR)
			{
#if 0
				BUS_SendMessage (NULL, MSG_APPC2U_RECORD_DISPLAY_MSG_INFO, eRecForbiddenTimeShiftMsg, NULL, 0, 0);
#endif
				/* start the recorder */
				if(pstContext->bRecordStart == FALSE)
				{
					err = MW_Rec_Start(pstContext->hAction);
					pstContext->bRecordStart = TRUE;
				}
				// TSR_REC 은 Pause
				xmgr_rec_SetPauseFlag(pstContext, eRecPause_DRM);
				return MESSAGE_BREAK;
			}
		}
		else
		{
			// 만약 Recording도 금지라면 그냥 두고, 아니면 혹시 열려있는 창이 있으면 닫아야 되겠다..
			if (pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bRecordForbidden == FALSE)
			{
#if 0
				BUS_SendMessage (NULL, MSG_APPC2U_RECORD_DISPLAY_MSG_INFO, eRecForbiddenCloseMsg, NULL, 0, 0);
#endif
			}
		}


		if((pstContext->eRecType == eMgrRecType_TSR)&&(pstContext->eRecState == eMgrRecState_BLOCKED))
		{
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
		}
		else
		{
			// 레코딩하는 채널과 동일하다면 tsr_rec 과 tsr_playback 을 Destroy 한다.
			xmgr_pvr_RecordDestroyCurrentSvcTsr(pstContext);

			/* start the recorder */
			err = MW_Rec_Start(pstContext->hAction);
			if (err)
			{
				HxLOG_Info("xmgr_rec_SetDrm_Viaccess(%d) : can't take MW_Rec_Start() eActionType=%x Handle_t=%x err=%d\n", pstContext->ucRecIdx, pstContext->eRecType, pstContext->hAction, err);
			}
		}
		pstContext->bRecordStart = TRUE;
		BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_RECORD_STARTED, pstContext->hAction, pstContext->eRecType, pstContext->ulContentId, pstContext->ulSlot);

		if(pstContext->eRecType != eMgrRecType_TSR)
		{
#if 0
			BUS_SendMessage (NULL, MSG_APPC2U_CREATE_REC_STATUS_WND,  0, 0, 0, 0);
#endif
		}
	}
	else											// split during record
	{
#if 0
		//if(pstContext->eRecType == eMgrRecType_TSR)
		{
			err = MW_Rec_SetDRMInfo(pstContext->hAction, &(pstContext->stNewDrmInfo), TRUE);
			HxLOG_Info("xmgr_rec_SetDrm_Viaccess(%d) : 2. MW_Rec_SetDRMInfo() eActionType=%x Handle_t=%x err=%d\n", pstContext->ucRecIdx, pstContext->eRecType, pstContext->hAction, err);
		}
#endif

		// recording 불가인지 체크
		if (pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bRecordForbidden == TRUE)
		{
			if(pstContext->eRecType != eMgrRecType_TSR)
			{
				Handle_t hSvc;

				if (MGR_ACTION_GetMasterSvcHandle(MGR_ACTION_GetMainActionId(), &hSvc) == ERR_OK)
				{

					// 현재 화면과 같은 서비스인 경우만 화면에 메시지 표시
					if (pstContext->hMasterSvc == hSvc)
					{
#if 0
						BUS_SendMessage (NULL, MSG_APPC2U_RECORD_DISPLAY_MSG_INFO, eRecForbiddenRecordMsg, NULL, 0, 0);
#endif
					}
				}

				pstContext->bDestroybyDrm = TRUE;
				if((pstContext->eRecType == eMgrRecType_EBR) ||(pstContext->eRecType == eMgrRecType_TBR) || (pstContext->eRecType == eMgrRecType_SBR))
				{
					err = MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_UNKNOWN);
				}
				HxLOG_Info("[xmgr_rec_SetDrm_Viaccess] [eMgrRecState_WAIT] if(pstContext->eRecType != eMgrRecType_TSR) 2 BUS_MGR_Destroy \n");
				BUS_MGR_Destroy(0);
#if 0
				// endtime 창 닫기
				BUS_SendMessage (NULL, MSG_APPC2U_DESTROY_REC_EDIT_DURATION, 0, 0, 0, 0);
#endif
				return MESSAGE_BREAK;
			}
		}

		// TSR 불가인지 체크
		if (pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bTsrForbidden == TRUE)
		{
			if(pstContext->eRecType == eMgrRecType_TSR)
			{
#if 0
				BUS_SendMessage (NULL, MSG_APPC2U_RECORD_DISPLAY_MSG_INFO, eRecForbiddenTimeShiftMsg, NULL, 0, 0);
#endif
				/* start the recorder */
				if(pstContext->bRecordStart == FALSE)
				{
					err = MW_Rec_Start(pstContext->hAction);
					pstContext->bRecordStart = TRUE;
				}
				// TSR_REC 은 Pause
				xmgr_rec_SetPauseFlag(pstContext, eRecPause_DRM);
				return MESSAGE_BREAK;
			}
		}
		else
		{
			// 만약 Recording도 금지라면 그냥 두고, 아니면 혹시 열려있는 창이 있으면 닫아야 되겠다..
			if (pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bRecordForbidden == FALSE)
			{
#if 0
				BUS_SendMessage (NULL, MSG_APPC2U_RECORD_DISPLAY_MSG_INFO, eRecForbiddenCloseMsg, NULL, 0, 0);
#endif
			}
		}

		if((pstContext->eRecType == eMgrRecType_TSR) &&(pstContext->eRecState == eMgrRecState_BLOCKED))
		{
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
		}
	}

	pstContext->stDrmInfo.eDrmType = eDxCopyrightControl_CasCcType_Viaccess;
	HxSTD_memcpy(&(pstContext->stDrmInfo.info.stViaccessDrmInfo), &(pstContext->stNewDrmInfo.info.stViaccessDrmInfo), sizeof(DxCopyrightControl_Viaccess_t));

	return TRUE;
}

/*
	Record forbidden
	TSR forbidden
*/

// eSEVT_CAS_DRM_SIGNALLED : CAS 로부터 DRM & Copy Protection 정보가 들어옴.
STATIC BUS_Result_t xmgr_rec_EvtCasDrmSignalled_Viaccess(xmgrRec_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->bTunningUnlock == TRUE)
	{
		return MESSAGE_PASS;
	}

	HLIB_STD_MemCpySafe(&(pstContext->stNewDrmInfo), sizeof(DxCopyrightControl_t), p3, sizeof(DxCopyrightControl_t));

	if(pstContext->stNewDrmInfo.eDrmType != eDxCopyrightControl_CasCcType_Viaccess)
	{
		return ERR_BUS_NO_ERROR;
	}



	HxLOG_Info("[xmgr_rec_EvtCasDrmSignalled_Viaccess] rec:%x tsr:%x\n",
			pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bRecordForbidden, pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bTsrForbidden);

	pstContext->bCasDrmValid = TRUE;
	pstContext->bCasDrmNone = FALSE;
	xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		HxLOG_Info("[xmgr_rec_EvtCasDrmSignalled_Viaccess] WAIT_DRM -> Normal Recording\n");
		xmgr_rec_SetDrm_Viaccess(pstContext);
	}
	else if(HxSTD_memcmp(&(pstContext->stDrmInfo.info.stViaccessDrmInfo), &(pstContext->stNewDrmInfo.info.stViaccessDrmInfo), sizeof(DxCopyrightControl_Viaccess_t)) != 0)
	{
		HxLOG_Info("[xmgr_rec_EvtCasDrmSignalled_Viaccess] Normal -> Normal Recording\n");
		xmgr_rec_SetDrm_Viaccess(pstContext);
	}
	else
	{
#if 0
		if(pstContext->eRecType == eMgrRecType_TSR)
		{
			MW_Rec_SetDRMInfo(pstContext->hAction, &(pstContext->stNewDrmInfo), TRUE);
		}
#endif
		HxLOG_Info("[xmgr_rec_EvtCasDrmSignalled_Viaccess] SAME DRM IGNORE !!! \n");
	}

	return MESSAGE_PASS;
}

// eSEVT_CAS_DRM_NONE : CAS 로부터 DRM & Copy Protection 정보가 없다는 메시지
STATIC BUS_Result_t xmgr_rec_EvtCasDrmNone_Viaccess(xmgrRec_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->bTunningUnlock == TRUE)
	{
		return MESSAGE_PASS;
	}

	if(p1 != eDxCopyrightControl_CasCcType_Viaccess)
	{
		// DRM Type 이 Viaccess 가 아니라면 Base 보내서 Base 에서 처리하도록 한다.
		return ERR_BUS_NO_ERROR;
	}

	pstContext->bCasDrmValid = FALSE;
	pstContext->bCasDrmNone = TRUE;

	HxLOG_Info("[xmgr_rec_EvtCasDrmNone_Viaccess] \n");

	// CAS 채널인 경우에만 CAS_DRM_NONE 이 온다.
	//MGR_COPYRIGHTCONTROL_InitCcInfo (&(pstContext->stNewDrmInfo), eDxCopyrightControl_CasCcType_Viaccess, eDxCopyrightControl_RestrictLevel_None);
	HxLOG_Info("[xmgr_rec_EvtCasDrmNone_Viaccess] rec:%x tsr:%x\n",
			pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bRecordForbidden, pstContext->stNewDrmInfo.info.stViaccessDrmInfo.bTsrForbidden);

	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		HxLOG_Info("[xmgr_rec_EvtCasDrmNone_Viaccess] WAIT_DRM -> Normal Recording\n");
		xmgr_rec_SetDrm_Viaccess(pstContext);
	}
	else if(HxSTD_memcmp(&(pstContext->stDrmInfo.info.stViaccessDrmInfo), &(pstContext->stNewDrmInfo.info.stViaccessDrmInfo), sizeof(DxCopyrightControl_Viaccess_t)) != 0)
	{
		HxLOG_Info("[xmgr_rec_EvtCasDrmNone_Viaccess] Normal -> Normal Recording\n");
		xmgr_rec_SetDrm_Viaccess(pstContext);
	}
	else
	{
		if(pstContext->eRecType == eMgrRecType_TSR)
		{
			MW_Rec_SetDRMInfo(pstContext->hAction, &(pstContext->stNewDrmInfo), TRUE);
		}
		HxLOG_Info("[xmgr_rec_EvtCasDrmNone_Viaccess] SAME DRM IGNORE !!! \n");
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}
#endif
// end of file

