
#if defined(CONFIG_MW_CAS_IRDETO)
/**
	@file     	ap_player_irdeto.c
	@brief    	humax recorded file & single program ts file playback control application.	\n

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <osd_gfx.h>
#include <osd_font.h>
#include <bus.h>
#include <db_svc.h>
#include <db_param.h>
#include <svc_si.h>
#include <svc_fs.h>
#include <svc_pvr.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_pipe.h>
#include <mgr_output.h>
#include <mgr_action.h>
#include <mgr_cas.h>

#include <ir_api.h>

#include "../local_include/_xmgr_playback_base.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	IR_PBERROR_STOP_TIMER_ID				10
#define	IR_PBERROR_STOP_TIMER_DUR				3000

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
STATIC BUS_Result_t xmgr_playback_IrdetoEvtAvDecodingStarted_Base(xmgrPbContext_t *pstContext, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
STATIC BUS_Result_t xmgr_playback_IrdetoStartPlayback_Base(xmgrPbContext_t *pstContext, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);

#define _________PROTO_TYPE__________


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _________GLOBAL_FUNC__________


BUS_Result_t 	xproc_playback_Irdeto_Base(xmgrPbContext_t *pstContext,
									HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT8					ucPlayIdx = ulActionId - eActionId_VIEW_0;

	switch (nMessage)
	{
		case eSEVT_AV_SESSION_STARTED:
			CHECK_PB_ACTION_HANDLE();
			HxLOG_Print("MESSAGE : eSEVT_AV_SESSION_STARTED %d\n", nParam1);
			eGwmResult = xmgr_playback_IrdetoEvtAvDecodingStarted_Base(pstContext, nParam1, nParam2, nParam3);

			if (eGwmResult == MESSAGE_PASS)
				return xproc_pvr_PbProto (pstContext, nMessage, hAction, nParam1, nParam2, nParam3);
			break;

		case eSEVT_CAS_DMV_NONE:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DMV_NONE %d\n", nParam1);
			xmgr_playback_IrdetoStartPlayback_Base(pstContext, nParam1, nParam2, nParam3);
			break;

		case eSEVT_CAS_DMV_SIGNALLED:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DMV_SIGNALLED %d\n", nParam1);
			eGwmResult = xmgr_playback_IrdetoStartPlayback_Base(pstContext, nParam1, nParam2, nParam3);
			break;

		case eMEVT_BUS_TIMER:
			if (nParam1 == IR_PBERROR_STOP_TIMER_ID)
			{
				BUS_MGR_Destroy (NULL);
				BUS_KillTimer(IR_PBERROR_STOP_TIMER_ID);
			}
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
	return xproc_pvr_PbProto(pstContext, nMessage, hAction, nParam1, nParam2, nParam3);
}

STATIC BUS_Result_t xmgr_playback_IrdetoStartPlayback_Base(xmgrPbContext_t *pstContext, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (pstContext->ePbState == eXmgrPbState_WAIT_AV_STARTED)
	{
		switch (nParam1)
		{
			case	IRMV_STATUS_CLEAR:
			case	IRMV_STATUS_FTA:
			case	IRMV_STATUS_ENCRYPTION:
				HxLOG_Info("Received Event CLEAR/FTA/ENCRYPTION(%x) --> Start PB\n", nParam1);
				pstContext->ePbState = eXmgrPbState_WAIT_PMT;
				SvcPb_Start(pstContext->hAction);
				break;

			case	IRMV_STATUS_NOMSK:
				//	Playback ERROR
				HxLOG_Info("Received Event NO_MSK --> not start PB\n");
				BUS_SetTimer(IR_PBERROR_STOP_TIMER_ID, IR_PBERROR_STOP_TIMER_DUR);
				break;

			case	IRMV_STATUS_FORBIDDEN:
				//	Playback ERROR
				HxLOG_Info("Received Event FORBIDDEN --> not start PB\n");
				BUS_SetTimer(IR_PBERROR_STOP_TIMER_ID, IR_PBERROR_STOP_TIMER_DUR);
				break;
		}
	} else
	{
		//	in Playing : Show/Hide Only
		switch (nParam1)
		{
			case	IRMV_STATUS_CLEAR:
			case	IRMV_STATUS_FTA:
			case	IRMV_STATUS_ENCRYPTION:
				HxLOG_Info("on running : Received Event CLEAR/FTA/ENCRYPTION(%x) --> need SHOW AV\n", nParam1);
				if ((pstContext->bAVHide == TRUE) && (pstContext->ePbState == eXmgrPbState_AV))
				{
					SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_Cas, FALSE);
					SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_Cas, FALSE);
					pstContext->bAVHide = FALSE;
				}
				break;

			case	IRMV_STATUS_NOMSK:
				//	Playback ERROR
				HxLOG_Info("on running : Received Event NO_MSK --> need HIDE AV\n");
				if (pstContext->bAVHide != TRUE)
				{
					SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE);
					SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_Cas, TRUE);
					pstContext->bAVHide = TRUE;
				}
				break;

			case	IRMV_STATUS_FORBIDDEN:
				//	Playback ERROR
				HxLOG_Info("on running : Received Event FORBIDDEN --> nothing\n");
				break;
		}
	}

	return ERR_OK;
}

STATIC BUS_Result_t xmgr_playback_IrdetoEvtAvDecodingStarted_Base(xmgrPbContext_t *pstContext, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	HERROR			err;

	if (pstContext->ePbState == eXmgrPbState_WAIT_AV_STARTED)
	{
		if (nParam1 == ERR_OK)
		{
			//	CAS Start for Playback!
#if	defined(CONFIG_MW_CAS_IRDETO_PVR)
			HxLOG_Print("Start Irdeto CAS service for checking encryption\n");
			switch (pstContext->ePbType & 0x0000000F)
			{
				case	eMgrPb_PlayType_Tsr:
				case	eMgrPb_PlayType_Chase:
					MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, /*pstContext->hSvc, */eCAS_ActionType_Tsr_Play, eDxCAS_GROUPID_IR, NULL);
					break;
				default:
					MGR_CAS_StartService(pstContext->hAction, HANDLE_NULL, /*pstContext->hSvc, */eCAS_ActionType_Playback, eDxCAS_GROUPID_IR, NULL);
					break;
			}
			pstContext->bCasStarted = TRUE;
#endif
			//	이 경우 외에는 기본 루틴을 따라간다.
			return MESSAGE_BREAK;
		} else
			return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

#endif