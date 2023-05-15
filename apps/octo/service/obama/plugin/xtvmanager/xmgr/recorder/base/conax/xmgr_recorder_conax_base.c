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

#include <conax.h>
#include <conax_ctrl.h>

#define CONFIG_RECORD_PAUSE

#define MATURITY_RATING_UPDATE_TIME					(5)


STATIC void xmgr_recorder_ConaxDebugRegCmd (void);

#define _________LOCAL_FUNC__________
#define	DRM_POINT_NAME						"STR_MESG_5718_ID"

STATIC HERROR xmgr_rec_ConaxGetCachedPin(HUINT8 *pucPin)
{
	SvcCas_CtrlParam_t			stParam;
	CX_GetCachedPinOutParam_t	stOutParam;

	stOutParam.pucPin = pucPin;

	stParam.ulControlType = eCACTRL_CX_GetCachedPin;
	stParam.pvIn = NULL;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CX, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eCACTRL_CX_GetCachedPin)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_rec_ConaxSetCaPin(Handle_t hAction, HUINT8 *pucPin, CxUiMsgType_t eUiMsgType)
{
	SvcCas_CtrlParam_t 		stParam;
	CX_SetScPinInParam_t	stInParam;
	CX_SetScPinOutParam_t	stOutParam;

	stInParam.hHandle = hAction;
	stInParam.pucPin = pucPin;
	stInParam.eMsgType = eUiMsgType;

	stParam.ulControlType = eCACTRL_CX_SetScPin;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eCACTRL_CX_SetScPin) error.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_rec_ConaxUpdateSessionKey(xmgrRec_Context_t *pstContext)
{
#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
	DbSvc_Info_t stSvcInfo;
#endif
	SvcRec_Setup_t		stRecSetup;
	SvcMeta_Record_t	stRecInfo;

	if(CXAPI_PVR_Rec_GetSessionKey(pstContext->hAction, stRecSetup.aucKey) != ERR_OK)
	{
		HxLOG_Error("CXAPI_PVR_Rec_GetSessionKey() error\n");
		return ERR_FAIL;
	}

	if(CXAPI_PVR_Rec_UpdateSessionKeyToMetaFile(pstContext->hAction, 0, stRecSetup.aucKey) != ERR_OK)
	{
		HxLOG_Error("CXAPI_PVR_Rec_UpdateSessionKeyToMetaFile() error\n");
		return ERR_FAIL;
	}

	pstContext->stSetup.bUseUserEncKey = TRUE;
	HxSTD_memcpy(pstContext->stSetup.aucKey, stRecSetup.aucKey, PAL_PVR_CRYPTO_KEY_LEN);

#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
	if(DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo) != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo() error\n");
	}

	if(DbSvc_GetCasType(&stSvcInfo) == eDxCAS_TYPE_All)
	{
		HxLOG_Print("Service DB will be update...\n");
		return ERR_OK;
	}

	if((DbSvc_GetCasType(&stSvcInfo) & eDxCAS_TYPE_CONAX) == 0)
	{
		HxLOG_Print("The service is not scrambled by CONAX CAS...\n");
		return ERR_OK;
	}
#endif

	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
	}

	if(stRecInfo.u1cEncryption)
	{
		if(pstContext->eRecState > eMgrRecState_WAIT)
		{
			stRecSetup.bEncryption = TRUE;
			stRecSetup.bUseUserEncKey = TRUE;

			if(SVC_REC_SetEncryption(pstContext->hAction, &stRecSetup, TRUE) != ERR_OK)
			{
				HxLOG_Error("SVC_REC_SetEncryption failed. hAction=(0x%x) \n",pstContext->hAction);
			}
		}
		else
		{
			if(pstContext->bCasOk)
			{
				xmgr_rec_ClearWaitFlag(pstContext, eRecWait_CAS);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR	xmgr_rec_ConaxSetPausedPoint (HUINT32 ulContentId, HUINT32 ulPosition)
{
	HERROR hErr;
	HUINT16 i;
	HUINT16	usNumOfPausedPoints = 0;
	SvcMeta_Bookmark_t *pstPausedPoints = NULL;
	SvcMeta_Bookmark_t *pstPausedPoint = NULL;
	HCHAR	szName[SVC_PVR_BOOKMARK_NAME_LEN] = {0,};

	hErr = SVC_META_GetPausedPoints(ulContentId, &usNumOfPausedPoints, &pstPausedPoints);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetPausedPoints failed. cid=(%d) ulPosition=(%d) \n",ulContentId, ulPosition);
		goto END_FUNC;
	}

	for (i=0;i<usNumOfPausedPoints; i++)
	{
		pstPausedPoint = &(pstPausedPoints[i]);
		if (ulPosition == pstPausedPoint->ulPos)
		{
			HxLOG_Debug("Drm point already exists. cid=(%d) ulPosition=(%d) \n",ulContentId, ulPosition);
			goto END_FUNC;
		}
	}

	HxLOG_Debug("Add new drm point. cid=(%d) position=(%d) \n",ulContentId, ulPosition);

	HxSTD_snprintf(szName, SVC_PVR_BOOKMARK_NAME_LEN, "%s", DRM_POINT_NAME);
	hErr = SVC_META_AddPausedPoint(ulContentId, szName, ulPosition, FALSE);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_AddPausedPoint failed. cid=(%d) ulPosition=(%d) \n",ulContentId, ulPosition);
		goto END_FUNC;
	}

END_FUNC:
	if (pstPausedPoints != NULL)
	{
		SVC_META_FreeBookmarks(pstPausedPoints);
	}

	return ERR_OK;
}

#define _________MSG_EVENT_LOCAL_FUNC__________
STATIC BUS_Result_t xmgr_rec_ConaxSetDrm_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t		stCcInfo = {0, };

	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_NO_ERROR;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	HLIB_STD_MemCpySafe(&(stCcInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	HxLOG_Info("//++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	HxLOG_Info("stCcInfo.bDrmCc : %s\r\n", stCcInfo.bDrmCc ? "TRUE" : "FALSE");
	HxLOG_Info("\tstCcInfo.stDrmCcInfo.eDrmCcType : %d\r\n", stCcInfo.stDrmCcInfo.eDrmCcType);
	HxLOG_Info("stCcInfo.bCasCc : %s\r\n", stCcInfo.bCasCc ? "TRUE" : "FALSE");
	HxLOG_Info("\tstCcInfo.stCasCcInfo.eCasCcType : %d\r\n", stCcInfo.stCasCcInfo.eCasCcType);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eApsCopyCtrl : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eApsCopyCtrl);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eEmiCopyCtrl : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eEmiCopyCtrl);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eICT : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eICT);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eRedistributionCtrl : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eRedistributionCtrl);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eTrickPlayCtrl : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eTrickPlayCtrl);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eMR : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eMR);
	HxLOG_Info("\tstCcInfo.stCasCcInfo.info.stConaxDrmInfo.eDisableAnalOutput : 0x%08x\r\n", stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eDisableAnalOutput);
	HxLOG_Info("stCcInfo.bSiCc : %s\r\n", stCcInfo.bSiCc ? "TRUE" : "FALSE");
	HxLOG_Info("\tstCcInfo.stSiCcInfo.eSiCcType : %d\r\n", stCcInfo.stSiCcInfo.eSiCcType);
	HxLOG_Info("\tstCcInfo.stSiCcInfo.info.stNordigDrmInfo.bHD : %s\r\n", stCcInfo.stSiCcInfo.info.stNordigDrmInfo.bHD ? "TRUE" : "FALSE");
	HxLOG_Info("\tstCcInfo.stSiCcInfo.info.stNordigDrmInfo.eContentProtectionLevel : 0x%08x\r\n", stCcInfo.stSiCcInfo.info.stNordigDrmInfo.eContentProtectionLevel);
	HxLOG_Info("//--------------------------------------------------------\r\n");

	if (stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eEmiCopyCtrl == eDxCopyrightControl_ConaxEmiCopyCtrl_Never)
	{
		// emi_copy_control = '11'
		if (pstContext->stSetup.eRecType == eMgrRecType_TSR)
		{
			if (stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eTrickPlayCtrl == eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpPauseFF)
			{
				HxLOG_Warning("TSR forbidden by NoJumpPauseFF...\n");

				BUS_PostData(NULL, eMEVT_PVR_RECORD_TSR_PAUSED, pstContext->hAction, eMgrRec_TsrPausedReason_ByDrm, pstContext->stMetaInfo.ulRecTime, DRM_POINT_NAME, HxSTD_StrLen(DRM_POINT_NAME));
				xmgr_rec_ConaxSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);

				xmgr_rec_SetPauseFlag(pstContext,eRecPause_DRM);
			}
			else
			{
				BUS_PostMessage(NULL, eMEVT_PVR_RECORD_TSR_RESUMED, pstContext->hAction, eMgrRec_TsrPausedReason_ByDrm, 0, 0);

				xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
			}
		}
		else
		{
			// long-term recording
			pstContext->bDestroybyDrm = TRUE;
			if(pstContext->stSetup.eRecType == eMgrRecType_INSTANT)
			{
				MGR_SCHEDULE_SetRecordFailReason(pstContext->stSetup.ulSlot, DxSchedule_RemovedReason_CasCopyrightControlFail, 0);
				MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_DRM);
				BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, pstContext->hAction, 0, 0, 0);

				return ERR_BUS_NO_ERROR;
			}
			else if((pstContext->stSetup.eRecType == eMgrRecType_EBR) ||(pstContext->stSetup.eRecType == eMgrRecType_TBR) || (pstContext->stSetup.eRecType == eMgrRecType_SBR))
			{
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
				MGR_SCHEDULE_SetRecordFailReason(pstContext->stSetup.ulSlot, DxSchedule_RemovedReason_CasCopyrightControlFail, 0);
				BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, pstContext->hAction, 0, 0, 0);

				return ERR_BUS_NO_ERROR;
#else
				// MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_DRM);
				if(pstContext->eRecState == eMgrRecState_WAIT)
				{
					if(pstContext->eWaitFlag & eRecWait_DRM)
					{
						HxLOG_Info("Rec Pause...\n");
						xmgr_rec_SetPauseFlag(pstContext,eRecPause_DRM);
						xmgr_rec_ConaxSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);
					}
				}
				else if(pstContext->eRecState == eMgrRecState_RECORDING)
				{
					HxLOG_Info("Rec Pause...\n");
					xmgr_rec_SetPauseFlag(pstContext,eRecPause_DRM);
					xmgr_rec_ConaxSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);
				}
#endif
			}
			HxLOG_Info("Copy Never Long-term recording prohibited\n");
		}
	}
	else if (stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eEmiCopyCtrl == eDxCopyrightControl_ConaxEmiCopyCtrl_Once)
	{
		// 직전까지 Recording 하는 구간이 Copy Never 구간이면 Recording을 수행할 수 없고 pause가 걸려있는 상태 일 때 Resume 수행
		if(pstContext->eRecState == eMgrRecState_BLOCKED)
		{
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
		}

		// svc_meta에 현재 녹화되는 content에 대한 cas drm 정보를 저장해야 하는데, copy once 일 경우 retention limit 값을 따져봐야 하고
		// 하나의 recording 파일 안에 여러개의 retention limit 값을 가지고 있는 구간이 있을 경우 보여줄 수 있는 content는 최대한 user에게 제공해야한다.
		// 따라서 retention limit 값은 가장 긴 시간을 기준으로 저장한다.
		if(pstContext->bCasDrmValid == TRUE) 		// 기존에 DRM 정보를 받은적 있는 경우에만 수행
		{
			if(pstContext->stNewDrmInfo.stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit < stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit)
			{
				stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit = pstContext->stNewDrmInfo.stCasCcInfo.info.stConaxDrmInfo.eRetentionLimit;
			}
		}

		// emi_copy_control = '10'
		if (pstContext->stSetup.eRecType == eMgrRecType_TSR)
		{
			// time-shift buffer
			if (stCcInfo.stCasCcInfo.info.stConaxDrmInfo.eTrickPlayCtrl == eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpPauseFF)
			{
				BUS_PostData(NULL, eMEVT_PVR_RECORD_TSR_PAUSED, pstContext->hAction, eMgrRec_TsrPausedReason_ByDrm, pstContext->stMetaInfo.ulRecTime, DRM_POINT_NAME, HxSTD_StrLen(DRM_POINT_NAME));
				xmgr_rec_ConaxSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);

				// TSR_REC Pause
				xmgr_rec_SetPauseFlag(pstContext,eRecPause_DRM);
			}
			else
			{
				BUS_PostMessage(NULL, eMEVT_PVR_RECORD_TSR_RESUMED, pstContext->hAction, eMgrRec_TsrPausedReason_ByDrm, 0, 0);

				// TSR REC Resume
				xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
			}
		}
	}
	else
	{
		if(pstContext->eRecState == eMgrRecState_BLOCKED)
		{
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
		}
	}

	pstContext->bCasDrmValid = TRUE;
	HLIB_STD_MemCpySafe(&pstContext->stNewDrmInfo, sizeof(DxCopyrightControl_t), &(stCcInfo), sizeof(DxCopyrightControl_t));
	SVC_META_SetDRMInfo(pstContext->ulContentId, &pstContext->stNewDrmInfo, TRUE);

	//--------------------------------------------------------

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtCasDrmSignalled_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3) // ok
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_NO_ERROR;
	}

	if (pstContext->eRecState == eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return ERR_BUS_NO_ERROR;
	}

	xmgr_rec_ConaxSetDrm_Base(pstContext,hAction, p1,p2,p3);

	if (pstContext->eRecState == eMgrRecState_WAIT)
	{
		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_DRM);
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtCasOk_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(p1 != eDxCAS_GROUPID_CX)
	{
		HxLOG_Error("p1 != eDxCAS_GROUPID_CX\n");
		return MESSAGE_PASS;
	}

	pstContext->nCasErrCode = p2;
	pstContext->bCasOk = TRUE;

	switch(pstContext->eRecState)
	{
		case eMgrRecState_WAIT:
			if(pstContext->stSetup.bUseUserEncKey)
			{
				xmgr_rec_ClearWaitFlag(pstContext, eRecWait_CAS);
			}
			break;

#if defined(CONFIG_RECORD_PAUSE)
		case eMgrRecState_BLOCKED:
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_CAS);
			break;
#endif
		default:
			break;
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtCasFail_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	CxUiMsgInfo_t *pstUiMsg = NULL;

	if(p1 != eDxCAS_GROUPID_CX)
	{
		HxLOG_Error("p1 != eDxCAS_GROUPID_CX\n");
		return MESSAGE_PASS;
	}

	if(pstContext->eRecState == eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return MESSAGE_PASS;
	}

	pstContext->nCasErrCode = p2;
	pstContext->bCasOk = FALSE;

	switch(pstContext->eRecState)
	{
		case eMgrRecState_WAIT:
			if(pstContext->stSetup.bUseUserEncKey)
			{
				xmgr_rec_ClearWaitFlag(pstContext, eRecWait_CAS);
			}
			break;

#if defined(CONFIG_RECORD_PAUSE)
		case eMgrRecState_RECORDING:
			xmgr_rec_SetPauseFlag(pstContext, eRecPause_CAS);
			break;
#endif
		default:
			break;
	}

	if(pstContext->stMetaInfo.ucEndCondition != eMgrRecFailed_NORIGHT_SCR)
	{
		switch(pstContext->stSetup.eRecType)
		{
			case eMgrRecType_TBR:
			case eMgrRecType_EBR:
			case eMgrRecType_SBR:
				if(p2 != NULL)
				{
					pstUiMsg = (CxUiMsgInfo_t *)p2;
					switch(pstUiMsg->eUiMsgType)
					{
						case eCxUi_GroupBasic_NoAccess:
							MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_NORIGHT_SCR);
							break;

						default:
							break;
					}
				}
				break;

			default:
				break;
		}
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtCasCheck_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL			bCacheCaPin = FALSE;
	HUINT8			aucCaPin[CONAX_CA_PIN_SIZE] = {0,};
	CxUiMsgInfo_t	*pstUiMsg;
	HERROR			hErr = ERR_OK;

	if(p1 != eDxCAS_GROUPID_CX)
	{
		HxLOG_Error("p1 != eDxCAS_GROUPID_CX\n");
		return MESSAGE_PASS;
	}

	pstUiMsg = (CxUiMsgInfo_t*)p2;
	if(pstUiMsg == NULL)
	{
		return ERR_BUS_NO_ERROR;
	}

	switch(MGR_ACTION_GetType(SVC_PIPE_GetActionId(hAction)))
	{
		case eRecType_REC:
		case eRecType_TSR_REC:
			switch(pstUiMsg->eUiMsgType)
			{
				//case eCxUi_GroupPpv_AcceptViewing:
				//case eCxUi_GroupPpv_TokenPpv:
				case eCxUi_GroupMr_Ask:
					bCacheCaPin = TRUE;
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	if(bCacheCaPin == TRUE)
	{
		switch(pstContext->stSetup.eRecType)
		{
			case eMgrRecType_TSR:
			case eMgrRecType_INSTANT:
				hErr = xmgr_rec_ConaxGetCachedPin(aucCaPin);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("ERROR - xmgr_rec_ConaxGetCachedPin()...\n");
				}
				break;

			default:
				hErr = xmgr_cas_CxOperateCachedPin(NULL);
				if(hErr == ERR_OK)
				{
					hErr = xmgr_rec_ConaxGetCachedPin(aucCaPin);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("ERROR - xmgr_rec_ConaxGetCachedPin()...\n");
					}
				}
				break;
		}

		if(hErr == ERR_OK)
		{
			hErr = xmgr_rec_ConaxSetCaPin(hAction, aucCaPin, pstUiMsg->eUiMsgType);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("ERROR - xmgr_rec_ConaxSetCaPin()...\n");
			}
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtCasDrmUnknown_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
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

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtCasDrmNone_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
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

#if 0	// EVT_CAS_OK & EVT_CAS_FAIL로 대체하도록 하자...
/*	conax 6.0 - 5.5 ECM processing during recording
				5.5.2 Acquire control words (ref.)
				5.5.3 Access granted?
	p1: record type / p2: check if drmtype is conax / p3: access_granted or not (access_granted: FALSE)
*/
STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtCheckAccessGranted(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(!(((eCAS_ActionType_Tsr_Rec == p1) && (pstContext->stSetup.eRecType == eMgrRecType_TSR))
		||((eCAS_ActionType_Recording == p1) && (pstContext->stSetup.eRecType != eMgrRecType_TSR))))
	{
		return ERR_BUS_MESSAGE_BREAK;
	}

	if(p3 == FALSE) // access_granted
	{
		if(pstContext->stSetup.eRecType == eMgrRecType_TSR && pstContext->bNotAccessGranted == TRUE)
		{
			pstContext->bNotAccessGranted = FALSE;
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
		}
	}
	else // not access_granted
	{
		if(pstContext->bNotAccessGranted == FALSE)
		{
			pstContext->bNotAccessGranted = TRUE;

			if(pstContext->stSetup.eRecType == eMgrRecType_TSR)
			{
				xmgr_rec_SetPauseFlag(pstContext, eRecPause_DRM);
			}
			else
			{
				MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_DRM);
				BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, pstContext->hAction, 0, 0, 0);
			}
		}
	}

	return	ERR_BUS_NO_ERROR;
}
#endif

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtRecCreate_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrRec_RecSetup_t	*pstSetup =	&pstContext->stSetup;
	SvcCas_ActionType_e	eActionType;
	BUS_Result_t eBusResult;
#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
	DbSvc_Info_t stSvcInfo;
#endif
	HBOOL bApplySpec = TRUE;

	eBusResult = xmgr_rec_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);

	/* update status to svc_meta */
	if(SVC_META_RegisterPvrAction(pstContext->hAction, pstContext->ulContentId, eSvcMeta_UsingActType_Record) != ERR_OK)
	{
		HxLOG_Error("SVC_META_RegisterPvrAction. Error\n");
	}

	eActionType = (pstSetup->eRecType == eMgrRecType_TSR) ? eCAS_ActionType_Tsr_Rec : eCAS_ActionType_Recording;

	if(CXAPI_PVR_Rec_Open(pstContext->hAction, pstContext->ulContentId, pstContext->stSetup.hCurrSvc, eActionType) != ERR_OK)
	{
		HxLOG_Error("CXAPI_PVR_Rec_Open() error\n");
		return eBusResult;
	}

	xmgr_rec_SetWaitFlag(pstContext, eRecWait_CAS);

#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
	if(DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo) != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo() error\n");
	}

	if(DbSvc_GetCasType(&stSvcInfo) == eDxCAS_TYPE_All)
	{
		return eBusResult;
	}

	bApplySpec = (DbSvc_GetCasType(&stSvcInfo) & eDxCAS_TYPE_CONAX) ? TRUE : FALSE;
#endif

	if(bApplySpec != TRUE)
	{
		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_CAS);
	}

	return eBusResult;
}

#if defined(CONFIG_MW_MM_DELAYED_REC)
STATIC BUS_Result_t xmgr_rec_ConaxMsgMgrRecordChangeTsrRecToDelayedRecord_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusResult;
	SvcMeta_Record_t stRecInfo;

	if (pstContext->stNewDrmInfo.stCasCcInfo.info.stConaxDrmInfo.eEmiCopyCtrl == eDxCopyrightControl_ConaxEmiCopyCtrl_Never)
	{
		// MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_DRM);
		BUS_PostMessage(NULL, eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT, pstContext->hAction, (HINT32)p1, (HINT32)ERR_FAIL, (HINT32)eMgrRecFailed_DRM);
		return MESSAGE_PASS;
	}

	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
	}

	eBusResult = xmgr_rec_MsgMgrRecordChangeTsrRecToDelayedRecord_Base(pstContext, hAction, p1, p2, p3);

	if(CXAPI_PVR_Rec_StartDelayed(pstContext->hAction, pstContext->ulContentId, stRecInfo.aucPath) != ERR_OK)
	{
		HxLOG_Error("CXAPI_PVR_Rec_StartDelayed() error\n");
		goto _RTN;
	}

_RTN:
	return eBusResult;
}
#endif

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtRecDestroy_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusResult;

	if(pstContext->hAction != hAction)
	{
		HxLOG_Warning("pstContext->hAction (%x), hAction (%x)\n", pstContext->hAction, hAction);
		hAction = pstContext->hAction;
	}

	eBusResult = xmgr_rec_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);

	if(CXAPI_PVR_Rec_Close(pstContext->hAction) != ERR_OK)
	{
		HxLOG_Error("CXAPI_PVR_Rec_Close() error\n");
		goto _RTN;
	}

_RTN:
	return eBusResult;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtSiPmtReceived_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3) // ok
{
#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
	DbSvc_Info_t stOldSvcInfo, stNewSvcInfo;
	HUINT16 usSvcId;
	HBOOL bChangedPid = FALSE;
#endif
	HBOOL bApplySpec = TRUE;

#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
	if(DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stOldSvcInfo) != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo() error\n");
		goto _RTN;
	}

	if(DB_SVC_GetServiceIdFromSvcHandle(pstContext->stSetup.hCurrSvc, &usSvcId) != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() error\n");
		goto _RTN;
	}

	if(SVC_SI_GetLiveServiceInfoPid(pstContext->hAction, (HUINT32)usSvcId, &stOldSvcInfo, &stNewSvcInfo, &bChangedPid) != ERR_OK)
	{
		HxLOG_Error("SVC_SI_GetLiveServiceInfoPid() error\n");
		goto _RTN;
	}

	if(DbSvc_GetCasType(&stNewSvcInfo) == eDxCAS_TYPE_All)
	{
		HxLOG_Print("DbSvc_GetCasType(eDxCAS_TYPE_All)\n");
		goto _RTN;
	}

	bApplySpec = (DbSvc_GetCasType(&stNewSvcInfo) & eDxCAS_TYPE_CONAX) ? TRUE : FALSE;
#endif

	if(bApplySpec)
	{
		if(pstContext->bCasOk)
		{
			if(pstContext->stSetup.bUseUserEncKey)
			{
				xmgr_rec_ClearWaitFlag(pstContext, eRecWait_CAS);
			}
		}
	}
	else
	{
		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_CAS);
	}

_RTN:
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtRecTimecodeChange_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3) // ok
{
	DbSvc_Info_t stSvcInfo;
	HUINT32	ulCurrentTime	= (HUINT32)p1;
	HBOOL bUpdateKey = FALSE;
	HBOOL bApplySpec = TRUE;
	Handle_t hCurrSvc = HANDLE_NULL;
	HERROR	hErr = ERR_OK;

#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
	hCurrSvc = pstContext->stSetup.hCurrSvc;
	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	if (DB_SVC_GetServiceInfo (hCurrSvc, &stSvcInfo) == ERR_OK)
	{
		bApplySpec = (DbSvc_GetCasType(&stSvcInfo) & eDxCAS_TYPE_CONAX) ? TRUE : FALSE;
	}
#endif

	if(bApplySpec == TRUE)
	{
		if(CXAPI_PVR_Rec_ChangeTimecode(hAction, ulCurrentTime, &bUpdateKey) != ERR_OK)
		{
			HxLOG_Error("CXAPI_PVR_Rec_ChangeTimecode() error\n");
			goto _RTN;
		}
	}
_RTN:
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtRecScrambled_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_CI)
	return ERR_BUS_NO_ERROR;
#else
	return MESSAGE_PASS;
#endif
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtRecUpdateKey_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(p2 != eDxCAS_GROUPID_CX)
	{
		HxLOG_Print("p2 != eDxCAS_GROUPID_CX\n");
		return MESSAGE_PASS;
	}

#if defined(CONAX_PVR_SPEC_ENABLE)
	if(xmgr_rec_ConaxUpdateSessionKey(pstContext) != ERR_OK)
	{
		HxLOG_Error("xmgr_rec_ConaxUpdateSessionKey() error\n");
		goto _RTN;
	}
#endif

_RTN:
	return MESSAGE_BREAK;
}

#if defined(CONFIG_MW_CAS_CONAX_PRETEST)

#define CX_REC_CHECK_TIMER_ID	1
#define CX_REC_CHECK_TIMER_TIME	100

STATIC HERROR xmgr_rec_ConaxUpdateRecTime(xmgrRec_Context_t *pstContext)
{
	SvcMeta_Record_t stRecInfo;
	HULONG ulTime;

	VK_CLOCK_GetTime(&ulTime);

	SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo);

	if(stRecInfo.ulStartTime >= 100)
		return ERR_OK;

	stRecInfo.ulStartTime = ulTime - pstContext->ulPlaytime;

	SVC_META_SetRecordInfo(pstContext->ulContentId, &stRecInfo);

	return ERR_OK;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtSiTdtReceived_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusResult = ERR_BUS_NO_ERROR;
	HULONG ulTime;

	if(VK_CLOCK_GetTime(&ulTime) != VK_OK)
	{
		BUS_KillTimer(CX_REC_CHECK_TIMER_ID);
		BUS_SetTimer(CX_REC_CHECK_TIMER_ID, CX_REC_CHECK_TIMER_TIME);
	}
	else
	{
		xmgr_rec_ConaxUpdateRecTime(pstContext);
	}

	return eBusResult;
}

STATIC BUS_Result_t xmgr_rec_ConaxMsgEvtBusTimer_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1)
	{
		case CX_REC_CHECK_TIMER_ID:
			BUS_KillTimer(p1);
			xmgr_rec_ConaxUpdateRecTime(pstContext);
			return MESSAGE_BREAK;

		default:
			return MESSAGE_PASS;
	}
}
#endif

#define _____PROTO_PROCEDURE_____

STATIC BUS_Result_t xproc_rec_ConaxProto_Base(xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Warning ("********************************************************************************\n");
			HxLOG_Warning ("*  XMGR : CONAX : RECORDING : CREATE (%d)\n", ulActionId);
			HxLOG_Warning ("********************************************************************************\n");

			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtRecCreate_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtRecDestroy_Base(pstContext, hAction, p1, p2, p3);

			HxLOG_Warning ("********************************************************************************\n");
			HxLOG_Warning ("*  XMGR : CONAX : RECORDING : DESTROY (%d)\n", ulActionId);
			HxLOG_Warning ("********************************************************************************\n");
			break;

#if defined(CONFIG_MW_MM_DELAYED_REC)
		case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:RecConax(%d)] MESSAGE : eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgMgrRecordChangeTsrRecToDelayedRecord_Base(pstContext, hAction, p1, p2, p3);
			break;
#endif

		case eSEVT_SI_PMT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_TIMECODE_CHANGE :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_REC_TIMECODE_CHANGE\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtRecTimecodeChange_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_SCRAMBLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_REC_SCRAMBLED\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtRecScrambled_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_UPDATE_KEYINFO:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_UPDATE_KEYINFO\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtRecUpdateKey_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_OK:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_OK\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtCasOk_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtCasFail_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CHECK:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_FAIL\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtCasCheck_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtCasDrmSignalled_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_UNKNOWN:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_DRM_UNKNOWN\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtCasDrmUnknown_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_NONE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_DRM_NONE\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtCasDrmNone_Base(pstContext, hAction, p1, p2, p3);
			break;

#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
		case eSEVT_SI_TDT:
			eBusResult = xmgr_rec_ConaxMsgEvtSiTdtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER:
			eBusResult = xmgr_rec_ConaxMsgEvtBusTimer_Base(pstContext, hAction, p1, p2, p3);
			break;
#endif

#if 0
		case eSEVT_CAS_CX_CHECK_ACCESSGRANTED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:RecConax(%d)] MESSAGE : eSEVT_CAS_DRM_NONE\n", ulActionId);
			eBusResult = xmgr_rec_ConaxMsgEvtCheckAccessGranted(pstContext, hAction, p1, p2, p3);
			break;
#endif

		default :
			break;
	}

	return eBusResult;
}

#define _____MSG_PROCESSING_FUNCTIONS_____
BUS_Result_t xmgr_rec_ConaxProto_Base(xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return xproc_rec_ConaxProto_Base(pstContext, message, hAction, p1, p2, p3);
}


#define _________PLUGIN_FUNC__________
HERROR xmgr_recorder_ConaxInit_Base(void)
{
#if defined(CONFIG_DEBUG)
	xmgr_recorder_ConaxDebugRegCmd();
#endif
	xmgr_recorder_Init_Base();

	return ERR_OK;
}

HERROR xmgr_recorder_ConaxSetRecordSetup_Base (MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo, Handle_t hAction)
{
	DbSvc_Info_t	stOldSvcInfo, stNewSvcInfo, *pstSvcInfo;
	HBOOL			bApplySpec = TRUE, bChangedPid = FALSE;
	HUINT16			usSvcId;

	if (MGR_RECORDER_GetEncryptionMode(pstSetup) == TRUE)
	{
#if defined(CONAX_PVR_SPEC_ENABLE_ONLY_CONAX_SERVICE)
		bApplySpec = FALSE;
		pstSvcInfo = &stOldSvcInfo;

		if(DB_SVC_GetServiceInfo(pstSetup->hCurrSvc, pstSvcInfo) != ERR_OK)
		{
			HxLOG_Error("DB_SVC_GetServiceInfo() error\n");
			goto _BASE_SETUP;
		}

		if(DbSvc_GetCasType(pstSvcInfo) == eDxCAS_TYPE_All)
		{
			pstSvcInfo = &stNewSvcInfo;

			if(DB_SVC_GetServiceIdFromSvcHandle(pstSetup->hCurrSvc, &usSvcId) != ERR_OK)
			{
				HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() error\n");
				goto _BASE_SETUP;
			}

			if(SVC_SI_GetLiveServiceInfoPid(hAction, (HUINT32)usSvcId, &stOldSvcInfo, &stNewSvcInfo, &bChangedPid) != ERR_OK)
			{
				HxLOG_Error("SVC_SI_GetLiveServiceInfoPid() error\n");
				goto _BASE_SETUP;
			}
		}

		bApplySpec = (DbSvc_GetCasType(pstSvcInfo) & eDxCAS_TYPE_CONAX) ? TRUE : FALSE;

_BASE_SETUP:
#endif
		if(bApplySpec == TRUE)
		{
			if(pstSetup->bUseUserEncKey == TRUE)
			{
				pstSvcRecSetup->bUseUserEncKey = TRUE;
				HxSTD_MemCopy(pstSvcRecSetup->aucKey, pstSetup->aucKey, PAL_PVR_CRYPTO_KEY_LEN);
			}
		}
		else
		{
			HxLOG_Print("USE default record key...\n");
		}
	}

	return xmgr_recorder_SetRecordSetup_Base(pstSetup, pstSvcRecSetup,pstMetaInfo, hAction);
}

#define __________DEBUG_FUNC_________
#if defined(CONFIG_DEBUG)
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
	xmgr_rec_ConaxSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);

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

	xmgr_rec_ConaxSetPausedPoint(pstContext->ulContentId, pstContext->stMetaInfo.ulRecTime);

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

