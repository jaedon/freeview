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
#include <mgr_schedule.h>

#include <_xmgr_recorder.h>
#include "../local_include/_xmgr_recorder_base.h"

#include <xmgr_cas.h>


#define _________LOCAL_FUNC__________

#define _________MSG_EVENT_LOCAL_FUNC__________

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtCasDrmSignalled_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3) // ok
{
	DxCopyrightControl_t		stDrmInfo;

	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_NO_ERROR;
	}

	if(pstContext->eRecState== eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return ERR_BUS_NO_ERROR;
	}

#if 0	// 여기서 cas drm만 처리해도 된다면 enable할 것
	// vmx cas cc만 처리한다
	if (stDrmInfo.stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Verimatrix)
	{
		HxLOG_Error("Invalid cas cc info ! eCasCcType(%d)\n", stDrmInfo.stCasCcInfo.eCasCcType);
		return ERR_BUS_NO_ERROR;
	}
#endif

	HxSTD_MemSet(&stDrmInfo, 0x00, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&(stDrmInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	HxLOG_Info("//++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	HxLOG_Info("stDrmInfo.bDrmCc : %s\r\n", stDrmInfo.bDrmCc ? "TRUE" : "FALSE");
	HxLOG_Info("\tstDrmInfo.stDrmCcInfo.eDrmCcType : %d\r\n", stDrmInfo.stDrmCcInfo.eDrmCcType);

	HxLOG_Info("stDrmInfo.bCasCc : %s\r\n", stDrmInfo.bCasCc ? "TRUE" : "FALSE");
	HxLOG_Info("\tstDrmInfo.stCasCcInfo.eCasCcType : %d\r\n", stDrmInfo.stCasCcInfo.eCasCcType);
	HxLOG_Info("\t\tstDrmInfo.stCasCcInfo.info.stVmxDrmInfo.usVmxType : %d\r\n", stDrmInfo.stCasCcInfo.info.stVmxDrmInfo.usVmxType);
	HxLOG_Info("\t\tstDrmInfo.stCasCcInfo.info.stVmxDrmInfo.bHdcp : %d\r\n", stDrmInfo.stCasCcInfo.info.stVmxDrmInfo.bHdcp);
	HxLOG_Info("\t\tstDrmInfo.stCasCcInfo.info.stVmxDrmInfo.bDC : %d\r\n", stDrmInfo.stCasCcInfo.info.stVmxDrmInfo.bDC);
	HxLOG_Info("\t\tstDrmInfo.stCasCcInfo.info.stVmxDrmInfo.eApsCopyCtrl : 0x%X\r\n", stDrmInfo.stCasCcInfo.info.stVmxDrmInfo.eApsCopyCtrl);
	HxLOG_Info("\t\tstDrmInfo.stCasCcInfo.info.stVmxDrmInfo.eEmiCopyCtrl : 0x%X\r\n", stDrmInfo.stCasCcInfo.info.stVmxDrmInfo.eEmiCopyCtrl);

	HxLOG_Info("stDrmInfo.bSiCc : %s\r\n", stDrmInfo.bSiCc ? "TRUE" : "FALSE");
	HxLOG_Info("\tstDrmInfo.stSiCcInfo.eSiCcType : %d\r\n", stDrmInfo.stSiCcInfo.eSiCcType);
	//HxLOG_Info("\tstDrmInfo.stSiCcInfo.info.stNordigDrmInfo.bHD : %s\r\n", stDrmInfo.stSiCcInfo.info.stNordigDrmInfo.bHD ? "TRUE" : "FALSE");
	//HxLOG_Info("\tstDrmInfo.stSiCcInfo.info.stNordigDrmInfo.eContentProtectionLevel : 0x%08x\r\n", stDrmInfo.stSiCcInfo.info.stNordigDrmInfo.eContentProtectionLevel);
	HxLOG_Info("//--------------------------------------------------------\r\n");
	pstContext->bCasDrmValid = TRUE;
	pstContext->bCasDrmNone = FALSE;

	stDrmInfo.bCasCc = TRUE;
	pstContext->stNewDrmInfo.bCasCc = stDrmInfo.bCasCc;
	pstContext->stMetaInfo.stDrmInfo.bCasCc = stDrmInfo.bCasCc;

	HLIB_STD_MemCpySafe(&(pstContext->stNewDrmInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t), &stDrmInfo.stCasCcInfo, sizeof(DxCopyrightControl_CasCcInfo_t));

	if (SVC_META_SetDRMInfo(pstContext->ulContentId, &pstContext->stNewDrmInfo, TRUE) != ERR_OK)
	{
		HxLOG_Error("SVC_META_SetDRMInfo() failed! \n");
	}

	if (pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_DRM);
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtCasOk_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtCasFail_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtCasCheck_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtCasDrmUnknown_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_NO_ERROR;
	}

	if(pstContext->eRecState== eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return ERR_BUS_NO_ERROR;
	}

	if (pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_DRM);
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtCasDrmNone_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_NO_ERROR;
	}

	if(pstContext->eRecState== eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return ERR_BUS_NO_ERROR;
	}

	if (pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_DRM);
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtRecCreate_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtRecDestroy_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtSiPmtReceived_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3) // ok
{
 	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_VerimatrixMsgEvtRecScrambled_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

#define _____PROTO_PROCEDURE_____

STATIC BUS_Result_t xproc_rec_VerimatrixProto_Base(xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print ("********************************************************************************\n");
			HxLOG_Print ("*  XMGR : VERIMATRIX : RECORDING : CREATE (%d)\n", ulActionId);
			HxLOG_Print ("********************************************************************************\n");

			HxLOG_Info("[XMGR:RecVmx(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtRecCreate_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtRecDestroy_Base(pstContext, hAction, p1, p2, p3);

			HxLOG_Print ("********************************************************************************\n");
			HxLOG_Print ("*  XMGR : VERIMATRIX : RECORDING : DESTROY (%d)\n", ulActionId);
			HxLOG_Print ("********************************************************************************\n");
			break;

		case eSEVT_SI_PMT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_SCRAMBLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_REC_SCRAMBLED\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtRecScrambled_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_OK:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtCasOk_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtCasFail_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CHECK:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtCasCheck_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtCasDrmSignalled_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_UNKNOWN:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_CAS_DRM_UNKNOWN\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtCasDrmUnknown_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_NONE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[RecVmx(%d)] MESSAGE : eSEVT_CAS_DRM_NONE\n", ulActionId);
			eBusResult = xmgr_rec_VerimatrixMsgEvtCasDrmNone_Base(pstContext, hAction, p1, p2, p3);
			break;

		default :
			break;
	}

	return eBusResult;
}

#define _____MSG_PROCESSING_FUNCTIONS_____
BUS_Result_t xmgr_rec_VerimatrixProto_Base(xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return xproc_rec_VerimatrixProto_Base(pstContext, message, hAction, p1, p2, p3);
}

#define _________PLUGIN_FUNC__________

#define __________DEBUG_FUNC_________
#if defined(CONFIG_DEBUG)
#if 0
STATIC HINT32 xmgr_recorder_ConaxDebugPauseTsr(void *szArgList)
{
	HUINT32 ulMainActionId = 0;
	HUINT32	ulTsrActionId = 0;
	xmgrRec_Context_t *pstContext = NULL;
	Handle_t			hSvcHandle;
	HERROR				hErr;


	ulMainActionId = MGR_ACTION_GetMainActionId();
	hErr = MGR_ACTION_GetActualSvcHandle(ulMainActionId, &hSvcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_GetActualSvcHandle failed");
		return HxCMD_ERR;
	}

	hErr = MGR_PVR_GetTsrRecActionId(hSvcHandle, &ulTsrActionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_PVR_GetTsrRecActionId failed");
		return HxCMD_ERR;
	}

	pstContext = xmgr_rec_GetContext(ulTsrActionId-eActionId_REC_FIRST);
	if (pstContext == NULL)
	{
		HxLOG_Error("xmgr_rec_GetContext failed");
		return HxCMD_ERR;
	}

	xmgr_rec_SetPauseFlag(pstContext,eRecPause_DRM);

	BUS_PostData(NULL, eMEVT_PVR_RECORD_TSR_PAUSED, pstContext->hAction, eMgrRec_TsrPausedReason_ByDrm, pstContext->stMetaInfo.ulRecTime, DRM_POINT_NAME, HxSTD_StrLen(DRM_POINT_NAME));
	xmgr_rec_VerimatrixSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);

	return ERR_OK;
}

STATIC HINT32 xmgr_recorder_ConaxDebugResumeTsr(void *szArgList)
{
	HUINT32 ulMainActionId = 0;
	HUINT32	ulTsrActionId = 0;
	xmgrRec_Context_t *pstContext = NULL;
	Handle_t			hSvcHandle;
	HERROR				hErr;


	ulMainActionId = MGR_ACTION_GetMainActionId();
	hErr = MGR_ACTION_GetActualSvcHandle(ulMainActionId, &hSvcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_GetActualSvcHandle failed");
		return HxCMD_ERR;
	}

	hErr = MGR_PVR_GetTsrRecActionId(hSvcHandle, &ulTsrActionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_PVR_GetTsrRecActionId failed");
		return HxCMD_ERR;
	}

	pstContext = xmgr_rec_GetContext(ulTsrActionId-eActionId_REC_FIRST);
	if (pstContext == NULL)
	{
		HxLOG_Error("xmgr_rec_GetContext failed");
		return HxCMD_ERR;
	}

	// TSR REC Resume
	xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);

	BUS_PostMessage(NULL, eMEVT_PVR_RECORD_TSR_RESUMED, pstContext->hAction, eMgrRec_TsrPausedReason_ByDrm, 0, 0);

	return ERR_OK;
}

STATIC HINT32 xmgr_recorder_ConaxDebugSetPausedPoint(void *szArgList)
{
	HCHAR	*pucArg=NULL;
	HUINT32	ulActionId = 0;
	xmgrRec_Context_t *pstContext = NULL;

	/* 1st argument */
	pucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if (NULL == pucArg)
		return HxCMD_ERR;

	if( sscanf(pucArg, "%d", &ulActionId) == 0 )
		return HxCMD_ERR;


	pstContext = xmgr_rec_GetContext(ulActionId-eActionId_REC_FIRST);
	if (pstContext == NULL)
	{
		HxLOG_Error("xmgr_rec_GetContext failed");
		return HxCMD_ERR;
	}

	xmgr_rec_VerimatrixSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);

	return ERR_OK;
}

STATIC HINT32 xmgr_recorder_ConaxDebugStopByDrm(void *szArgList)
{
	HCHAR	*pucArg=NULL;
	HUINT32	ulActionId = 0;
	xmgrRec_Context_t *pstContext = NULL;

	/* 1st argument */
	pucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if (NULL == pucArg)
		return HxCMD_ERR;

	if( sscanf(pucArg, "%d", &ulActionId) == 0 )
		return HxCMD_ERR;


	pstContext = xmgr_rec_GetContext(ulActionId-eActionId_REC_FIRST);
	if (pstContext == NULL)
	{
		HxLOG_Error("xmgr_rec_GetContext failed");
		return HxCMD_ERR;
	}

	MGR_SCHEDULE_SetRecordFailReason(pstContext->stSetup.ulSlot, DxSchedule_RemovedReason_CasCopyrightControlFail, 0);
	MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_DRM);
	BUS_PostMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, pstContext->hAction, 0, 0, 0);

	return ERR_OK;
}


STATIC void xmgr_recorder_ConaxDebugRegCmd (void)
{
#define	hCmdHandle		"mgr_rec_cx"
	HLIB_CMD_RegisterWord (hCmdHandle,xmgr_recorder_ConaxDebugPauseTsr,
						"pause_tsr",										/* keyword */
						"pause_tsr",										/*   help  */
						"pause_tsr");										/*  usage  */

	HLIB_CMD_RegisterWord (hCmdHandle,xmgr_recorder_ConaxDebugResumeTsr,
						"resume_tsr",										/* keyword */
						"resume_tsr",										/*	 help  */
						"resume_tsr");										/*	usage  */

	HLIB_CMD_RegisterWord (hCmdHandle,xmgr_recorder_ConaxDebugSetPausedPoint,
						"set_paused_point",										/* keyword */
						"set_paused_point actionId",							/*	 help  */
						"set_paused_point actionId");							/*	usage  */

	HLIB_CMD_RegisterWord (hCmdHandle,xmgr_recorder_ConaxDebugStopByDrm,
						"stop", 									/* keyword */
						"stop actionId",							/*	 help  */
						"stop actionId");							/*	usage  */

}
#endif
#endif

