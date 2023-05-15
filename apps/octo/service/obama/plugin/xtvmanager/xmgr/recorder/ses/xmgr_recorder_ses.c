/**
	@file     	xmgr_recorder_ses.c
	@brief    	recording control application for SES.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>

#include <octo_common.h>

#include <db_svc.h>

#include <isosvc.h>

#include <svc_pipe.h>
#include <svc_rec.h>

#include <bus.h>

#include <mgr_action.h>
#include <mgr_storage.h>
#include <mgr_schedule.h>
#include "../local_include/_xmgr_recorder.h"
#include "../base/local_include/_xmgr_recorder_base.h"
#include <xmgr_cas.h>
#include <mgr_copyrightcontrol.h>
#include <pmgr_copyrightcontrol.h>
#include <svc_sys.h>
#include <svc_cas.h>

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
/********************      Functions       *************************/
/*******************************************************************/

#define CRITICAL_TMP		// 꼭 적용해서 반영해야함..현재는 임시로 막음.
STATIC BUS_Result_t xproc_rec_Proto_Ses (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR xmgr_rec_NagraProcessSetDrmInfo_Ses(xmgrRec_Context_t *pstContext, HBOOL bMakeTimeStamp);

#if defined (CONFIG_DEBUG)
HUINT8 *xmgr_recorder_GetMessageName_Ses(HINT32 message);
void xmgr_rec_PrintKey(HUINT8 *pucKey);
void xmgr_rec_PrintDrmInfo_Ses( xmgrRec_Context_t *pstContext, HINT32 message);
void xmgr_rec_PrintDrm_Ses(DxCopyrightControl_t *pstDrmInfo);

HUINT8 *xmgr_recorder_GetMessageName_Ses(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_BUS_READY_SHUTDOWN);
		ENUM_TO_STR(eMEVT_BUS_CREATE);
		ENUM_TO_STR(eMEVT_BUS_DESTROY);
		//ENUM_TO_STR(eMEVT_BUS_TIMER);
		ENUM_TO_STR(eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD);
		ENUM_TO_STR(eMEVT_PVR_RECORD_ENDTIME_UPDATE);
		ENUM_TO_STR(eSEVT_SI_PMT);
		ENUM_TO_STR(eSEVT_SI_PAT_TIMEOUT);
		ENUM_TO_STR(eSEVT_SI_PMT_TIMEOUT);
		ENUM_TO_STR(eSEVT_SI_PMT_PIDREMOVED);
		ENUM_TO_STR(eSEVT_SI_PMT_PIDCHANGED);
		ENUM_TO_STR(eSEVT_SI_EIT);
		ENUM_TO_STR(eSEVT_SI_EIT_TIMEOUT);
		ENUM_TO_STR(eSEVT_SI_MULTIFEED_DETECTED);
		ENUM_TO_STR(eSEVT_SI_SDT_FREE_CA_MODE_CHANGED);
		ENUM_TO_STR(eSEVT_SI_SDT_RUNNING_STATUS_CHANGED);
		ENUM_TO_STR(eSEVT_CH_NO_SIGNAL);
		ENUM_TO_STR(eSEVT_CH_MOVING_MOTOR);
		ENUM_TO_STR(eSEVT_CH_LOCKED);
		ENUM_TO_STR(eMEVT_STORAGE_NOTIFY_UNPLUGGED);
		ENUM_TO_STR(eSEVT_DB_CHECK_SVC_DB);
		ENUM_TO_STR(eSEVT_REC_PID_CHANGED);
		//ENUM_TO_STR(eSEVT_REC_TIMECODE_CHANGE);
		ENUM_TO_STR(eSEVT_REC_SCRAMBLED);
		ENUM_TO_STR(eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE);
		ENUM_TO_STR(eSEVT_REC_REQ_COMMAND);
		ENUM_TO_STR(eSEVT_CAS_DRM_SIGNALLED);
		ENUM_TO_STR(eSEVT_SI_DRM_SIGNALLED);
		ENUM_TO_STR(eSEVT_CAS_OK);
		ENUM_TO_STR(eSEVT_CAS_FAIL);
		ENUM_TO_STR(eSEVT_CAS_DRM_NONE);
		default:
			return NULL;
	}
}

void xmgr_rec_PrintKey(HUINT8 *pucKey)
{
	HUINT8 k = 0;
	HxLOG_Print("[%s:%d] aucKey= [ ", __FUNCTION__, __LINE__);
	for(k=0;k<16;k++)
	{
		HxLOG_Print("%02x ", pucKey[k]);
	}
	HxLOG_Print(" ]\n");
}

static HERROR xmgr_rec_PrintDrmParseInfo (HUINT8 ucCtrlByteA, HUINT8 ucCtrlByteB)
{
	MgrCopyrightControl_MediaControlInfo_t stDetailDrmInfo;

	HxSTD_MemSet (&stDetailDrmInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));

	// Byte A
	stDetailDrmInfo.eTrickModeCtrl.eTrickRestrictMode_Ses = (HUINT8)((ucCtrlByteA & 0x38) >> 3);
	stDetailDrmInfo.bSvcBlock		 = (ucCtrlByteA & 0x04) ? TRUE : FALSE;
	stDetailDrmInfo.bDigiCpProtect	 = (ucCtrlByteA & 0x02) ? TRUE : FALSE;
	stDetailDrmInfo.bAnaCpProtect	 = (ucCtrlByteA & 0x01) ? TRUE : FALSE;

	// Byte B
	stDetailDrmInfo.eStorageRight	 = (HUINT8)((ucCtrlByteB & 0xE0) >> 5);
	stDetailDrmInfo.bStorageEncrypt = (ucCtrlByteB & 0x10) ? TRUE : FALSE;
	stDetailDrmInfo.eCompVidCtrl	 = (HUINT8)((ucCtrlByteB & 0x0C) >> 2);
	stDetailDrmInfo.bTransCtrl		 =  (ucCtrlByteB & 0x02) ? TRUE : FALSE;
	stDetailDrmInfo.bBlucomCtrl	 =  (ucCtrlByteB & 0x01) ? TRUE : FALSE;

	HxLOG_Debug("+ [METADATA] eTrickModeCtrl(%x)SvcBlock(%x) DigiCpProtect(%x)AnaCpProtect(%x) eStorageRight(%x) bStorageEncrypt(%x) eCompVidCtrl(%x)bTransCtrl(%x) bBlucomCtrl(%x)\n\n",
		stDetailDrmInfo.eTrickModeCtrl.eTrickRestrictMode_Ses, stDetailDrmInfo.bSvcBlock, stDetailDrmInfo.bDigiCpProtect, stDetailDrmInfo.bAnaCpProtect,
		stDetailDrmInfo.eStorageRight, stDetailDrmInfo.bStorageEncrypt, stDetailDrmInfo.eCompVidCtrl, stDetailDrmInfo.bTransCtrl,
		stDetailDrmInfo.bBlucomCtrl);

	return ERR_OK;
}
void xmgr_rec_PrintDrm_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	if(pstDrmInfo == NULL)
	{
		return;
	}

	if(pstDrmInfo->bCasCc == TRUE)
	{
		HxLOG_Debug("+ DRM(CAS) TRUE\n");
		if(pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
		{
			HxLOG_Debug("+ ----------------- cas drm type[%d] : No NagraSes -------------------\n", pstDrmInfo->stDrmCcInfo.eDrmCcType);
			return;
		}
		xmgr_rec_PrintDrmParseInfo(pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA, pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB);
	}

	if(pstDrmInfo->bSiCc == TRUE)
	{
		HxLOG_Debug("+ DRM(SI) TRUE\n");
		if(pstDrmInfo->stSiCcInfo.eSiCcType != eDxCopyrightControl_SiCcType_Ses)
		{
			HxLOG_Debug("+ ----------------- si drm type[%d] : No NagraSes -------------------\n", pstDrmInfo->stDrmCcInfo.eDrmCcType);
			return;
		}
		xmgr_rec_PrintDrmParseInfo(pstDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteA, pstDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteB);
	}

}

STATIC void xmgr_rec_PrintMetaInfo_Ses(xmgrRec_Context_t *pstContext)
{
	SvcMeta_Record_t stMetaInfo;
	if(pstContext== NULL)
	{
		return;
	}
	HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcMeta_Record_t));
	SVC_META_GetRecordInfo(pstContext->ulContentId, &stMetaInfo);
#if 0
	HxLOG_Debug("++++++++++++++ pstContext->stMetaInfo ++++++++++++++\n");
	HxLOG_Debug("+ path = %s\n",  pstContext->stMetaInfo.aucPath);
	HxLOG_Debug("+ fileName = %s\n",  pstContext->stMetaInfo.aucFileName);
	HxLOG_Debug("+ ulStartTime(%d), ulEndTime(%d), ulRecTime(%d), ulOrgDuration(%d\n", pstContext->stMetaInfo.ulStartTime, pstContext->stMetaInfo.ulEndTime, pstContext->stMetaInfo.ulRecTime,  pstContext->stMetaInfo.ulOrgDuration);
	HxLOG_Debug("+ ucRecordStatus(%d)\n", pstContext->stMetaInfo.ucRecordStatus);
	HxLOG_Debug("+ u2cEventBase(%d)\n", pstContext->stMetaInfo.u2cEventBase);
	HxLOG_Debug("+ nEventId(%d)\n", pstContext->stMetaInfo.nEventId);
	HxLOG_Debug("+ u1cLocked(%d)\n", pstContext->stMetaInfo.u1cLocked);
	HxLOG_Debug("+ u1cWatched(%d)\n", pstContext->stMetaInfo.u1cWatched);
	HxLOG_Debug("+ u1cScrambled(%d)\n", pstContext->stMetaInfo.u1cScrambled); // exist any scramble position in record file
	HxLOG_Debug("+ u1cEncryption(%d)\n", pstContext->stMetaInfo.u1cEncryption);
	HxLOG_Debug("+ u1cAutoDeleted(%d)\n", pstContext->stMetaInfo.u1cAutoDeleted);
	HxLOG_Debug("+ ucEndCondition(%d)\n", pstContext->stMetaInfo.ucEndCondition);			// reason to Record Fail, Recocrd Split
	HxLOG_Debug("+ ulLastWatchtime(%d)\n", pstContext->stMetaInfo.ulLastWatchtime);
	HxLOG_Debug("+ bUserLocked(%d)\n", pstContext->stMetaInfo.bUserLocked);			// true means locked at the record menu (or record list)
											// u1cLocked means service lock or parental lock or watch limited
	HxLOG_Debug("+ bWillBeDeleted(%d)\n", pstContext->stMetaInfo.bWillBeDeleted);
//	HUINT16			usNumOfBookmark;
//	SvcMeta_Bookmark_t	astBookmarks[DxPVR_NUM_BOOKMARK];
	HxLOG_Debug("+ aucDisplayEvtName = %s\n", pstContext->stMetaInfo.aucDisplayEvtName);
	HxLOG_Debug("+ bSeriesRec(%d)\n", pstContext->stMetaInfo.bSeriesRec);
	HxLOG_Debug("+ szSeriesId(%s)\n", pstContext->stMetaInfo.szSeriesId);
	HxLOG_Debug("+ ulEpisodeNum(%d)\n", pstContext->stMetaInfo.ulEpisodeNum);
	HxLOG_Debug("+ ulSeason(%d)\n", pstContext->stMetaInfo.ulSeason);
#endif
	HxLOG_Debug("++++++++++++++ SVC_META_GetRecordInfo() -> MetaInfo ++++++++++++++\n");
	HxLOG_Debug("+ path = %s\n",  stMetaInfo.aucPath);
	HxLOG_Debug("+ fileName = %s\n",  stMetaInfo.aucFileName);
	HxLOG_Debug("+ ulStartTime(%d), ulEndTime(%d), ulRecTime(%d), ulOrgDuration(%d\n", stMetaInfo.ulStartTime, stMetaInfo.ulEndTime, stMetaInfo.ulRecTime,  stMetaInfo.ulOrgDuration);
#if 0
	HxLOG_Debug("+ ucRecordStatus(%d)\n", stMetaInfo.ucRecordStatus);
	HxLOG_Debug("+ u2cEventBase(%d)\n", stMetaInfo.u2cEventBase);
	HxLOG_Debug("+ nEventId(%d)\n", stMetaInfo.nEventId);
	HxLOG_Debug("+ u1cLocked(%d)\n", stMetaInfo.u1cLocked);
	HxLOG_Debug("+ u1cWatched(%d)\n", stMetaInfo.u1cWatched);
	HxLOG_Debug("+ u1cScrambled(%d)\n", stMetaInfo.u1cScrambled); // exist any scramble position in record file
	HxLOG_Debug("+ u1cEncryption(%d)\n", stMetaInfo.u1cEncryption);
	HxLOG_Debug("+ u1cAutoDeleted(%d)\n", stMetaInfo.u1cAutoDeleted);
	HxLOG_Debug("+ ucEndCondition(%d)\n", stMetaInfo.ucEndCondition);			// reason to Record Fail, Recocrd Split
	HxLOG_Debug("+ ulLastWatchtime(%d)\n", stMetaInfo.ulLastWatchtime);
	HxLOG_Debug("+ bUserLocked(%d)\n", stMetaInfo.bUserLocked);			// true means locked at the record menu (or record list)
											// u1cLocked means service lock or parental lock or watch limited
	HxLOG_Debug("+ bWillBeDeleted(%d)\n", stMetaInfo.bWillBeDeleted);
//	HUINT16			usNumOfBookmark;
//	SvcMeta_Bookmark_t	astBookmarks[DxPVR_NUM_BOOKMARK];
	HxLOG_Debug("+ aucDisplayEvtName = %s\n", stMetaInfo.aucDisplayEvtName);
	HxLOG_Debug("+ bSeriesRec(%d)\n", stMetaInfo.bSeriesRec);
	HxLOG_Debug("+ szSeriesId(%s)\n", stMetaInfo.szSeriesId);
	HxLOG_Debug("+ ulEpisodeNum(%d)\n", stMetaInfo.ulEpisodeNum);
	HxLOG_Debug("+ ulSeason(%d)\n", stMetaInfo.ulSeason);
#endif
}

STATIC void xmgr_rec_PrintRecInfo_Ses(xmgrRec_Context_t *pstContext)
{
	if(pstContext == NULL)
	{
		return;
	}

	HxLOG_Debug("+ ulActionId(%d)\n", pstContext->ulActionId);
	HxLOG_Debug("+ hAction(%d)\n", pstContext->hAction);
	HxLOG_Debug("+ eRecState(%d)\n", pstContext->eRecState);
//	SvcRec_PIDList_t	stRecPidInfo;
	HxLOG_Debug("+ ulContentId(%d)\n", pstContext->ulContentId);
//	MgrRec_RecSetup_t	stSetup;
//	SvcMeta_Record_t 	stMetaInfo;
	HxLOG_Debug("+ bCasOk(%d)\n", pstContext->bCasOk);				// CAS에게 시청 허가
	HxLOG_Debug("+ nCasErrCode(%d)\n", pstContext->nCasErrCode);		// CAS error code
	HxLOG_Debug("+ bPMTValid(%d)\n", pstContext->bPMTValid);
	HxLOG_Debug("+ bCasDrmValid(%d)\n", pstContext->bCasDrmValid);		// CAS로부터 Drm&Copy protection 정보가 detect 됨. SI보다는 CAS가 우선 순위가 있음.
	HxLOG_Debug("+ bCasDrmNone(%d)\n", pstContext->bCasDrmNone);		// cas로부터 Drm 정보가 없다는 메시지를 받음
// 	CopyrightControl_t			stNewDrmInfo;		// SI, CAS중 최종 젹용할 drm info, Split 후 drm
	HxLOG_Debug("+ usBaseEventId(%d)\n", pstContext->usBaseEventId);		//	recording name에 대한 Base event id.
	HxLOG_Debug("+ ulStartTimeFromTimeCode(%d)\n", pstContext->ulStartTimeFromTimeCode);
	HxLOG_Debug("+ ulTsrtoDelayTime(%d)\n", pstContext->ulTsrtoDelayTime);
	HxLOG_Debug("+ nRecvEventNum(%d)\n", pstContext->nRecvEventNum);
	HxLOG_Debug("+ szDelayCopyFileName(%s)\n", pstContext->szDelayCopyFileName);
	HxLOG_Debug("+ bDelayedCopy(%d)\n", pstContext->bDelayedCopy);
//	xmgrRec_RecvEventInfo_t astRecvEventInfo[PVR_MAX_RECV_EVENT_NUM];
}

void xmgr_rec_PrintDrmInfo_Ses( xmgrRec_Context_t *pstContext, HINT32 message)
{
	HUINT8 buf[100];

	if(pstContext == NULL)
	{
		return;
	}

	HxSTD_memset(&buf, 0, sizeof(buf));
	switch(message)
	{
		case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD:
			sprintf(buf, "%s", "eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD");
			break;
		case eSEVT_REC_SCRAMBLED:
			sprintf(buf, "%s", "eSEVT_REC_SCRAMBLED");
			break;
		case eSEVT_CAS_DRM_SIGNALLED:
			sprintf(buf, "%s", "eSEVT_CAS_DRM_SIGNALLED");
			break;
		case eSEVT_CAS_OK:
			sprintf(buf, "%s", "eSEVT_CAS_OK");
			break;
		case eSEVT_CAS_FAIL:
			sprintf(buf, "%s", "eSEVT_CAS_FAIL");
			break;
		case eSEVT_CAS_DRM_NONE:
			sprintf(buf, "%s", "eSEVT_CAS_DRM_NONE");
			break;
		case eSEVT_SI_DRM_SIGNALLED:
			sprintf(buf, "%s", "eSEVT_SI_DRM_SIGNALLED");
			break;
		default:
			sprintf(buf, "%s", "null");
			break;
	}
	HxLOG_Debug("*************************************** PRINT START ***************************************\n");
	HxLOG_Debug("*************************************** REC Info < %s > ***************************************\n", buf);
//	xmgr_rec_PrintRecInfo_Ses(pstContext);
	HxLOG_Debug("*************************************** New DrmInfo ***************************************\n");
	xmgr_rec_PrintDrm_Ses(&pstContext->stNewDrmInfo);
//	HxLOG_Debug("*************************************** Meta DrmInfo ***************************************\n");
//	xmgr_rec_PrintDrm_Ses(&pstContext->stMetaInfo.stDrmInfo);
//	HxLOG_Debug("*************************************** Meta Info ***************************************\n");
	xmgr_rec_PrintMetaInfo_Ses(pstContext);
	HxLOG_Debug("*************************************** PRINT END ***************************************\n");
}
#endif

STATIC HERROR xmgr_rec_NagraSendFailReason_Ses(xmgrRec_Context_t *pstContext, eDxCopyrightControl_SesStorageRight_e eStorageRight)
{
	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("[%s:%d] eStorageRight(%d)\n", __FUNCTION__, __LINE__, eStorageRight);

	switch(eStorageRight)
	{
		case eDxCopyrightControl_SesStorageRight_NoRecording:
		case eDxCopyrightControl_SesStorageRight_NoMore90Min:
			MGR_SCHEDULE_SetRecordFailReason(pstContext->stSetup.ulSlot,DxSchedule_RemovedReason_CasCopyrightControlFail, eStorageRight);
			break;
		default:
			break;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_rec_NagraGetDrmInfo_Ses(DxCopyrightControl_t *pstDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDetailDrmInfo)
{
	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstDrmInfo == NULL || pstDetailDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo or pstDetailDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstDetailDrmInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	return MGR_COPYRIGHTCONTROL_GetMediaControlInfo(FALSE, pstDrmInfo, pstDetailDrmInfo);
}

STATIC HERROR xmgr_rec_NagraIsValidCasDrmInfo_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	if(pstDrmInfo->bCasCc == TRUE)
	{
		if(pstDrmInfo->stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_NagraSes)
		{
			HxLOG_Error("eCasCcType is not SES\n");
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xmgr_rec_NagraIsValidDrmInfo_Ses(DxCopyrightControl_t *pstDrmInfo)
{
	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo is NULL\n");
		return ERR_FAIL;
	}

	if(pstDrmInfo->bSiCc == TRUE)
	{
		if(pstDrmInfo->stSiCcInfo.eSiCcType != eDxCopyrightControl_SiCcType_Ses)
		{
			HxLOG_Error("eSiCcType is not SES\n");
			return ERR_FAIL;
		}
	}

	if(pstDrmInfo->bCasCc == TRUE)
	{
		if(pstDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
		{
			HxLOG_Error("eCasCcType is not SES\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

STATIC HBOOL xmgr_rec_IsHDplusService_Ses(HUINT32 ulActionId)
{
	HERROR			hErr = ERR_OK;
	HBOOL			eSame = FALSE;

	HUINT32 		ulPMTCaIdCnt = 0;
	HUINT16			ausPMTCaId[64];
	HUINT32			ulCnt = 0;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	VK_memset(ausPMTCaId, 0, sizeof(ausPMTCaId));
	hErr = SVC_CAS_GetCaSystemIdByActionId(ulActionId, &ulPMTCaIdCnt, ausPMTCaId);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("_mgr_LiveLock_base_IsHDplusService() - not found CA System ID from PMT \n");
		return FALSE;
	}

	for(ulCnt = 0; ulCnt < ulPMTCaIdCnt ; ulCnt++)
	{
		hErr = XMGR_CAS_NaIsSameCasIdInSmartCard(0, ausPMTCaId[ulCnt], &eSame);
		if(hErr == ERR_OK && eSame == TRUE)
		{
			HxLOG_Debug("HD PLUS SERVICE !!! \n");
			return TRUE;
		}
	}

	HxLOG_Debug("NOT HD PLUS SERVICE !!! \n");

	return FALSE;
}

STATIC HBOOL xmgr_rec_SetSCPinCodeFlag_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction)
{
	SvcMeta_Record_t 	stRecInfo;
	HUINT32 ulActionId = SVC_PIPE_GetActionId(hAction);

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		return ERR_FAIL;
	}

	pstContext->stMetaInfo.ucSCPinCodeFlag = FALSE;

	if(xmgr_rec_IsHDplusService_Ses(ulActionId) == TRUE)
	{
		if(SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA) == TRUE)
		{
			HxLOG_Debug("Rec : Current channel is APS HD\n");
			pstContext->stMetaInfo.ucSCPinCodeFlag = TRUE;
		}
	}

	HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcMeta_Record_t));

	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) == ERR_OK)
	{
		HxLOG_Debug("Set -> ucSCPinCodeFlag(%d)\n", pstContext->stMetaInfo.ucSCPinCodeFlag);
		if(stRecInfo.ucSCPinCodeFlag != TRUE)
		{
			stRecInfo.ucSCPinCodeFlag = pstContext->stMetaInfo.ucSCPinCodeFlag;
			if(SVC_META_SetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
			{
				HxLOG_Debug("SVC_META_SetRecordInfo is error \n");
			}
		}
	}

	return ERR_OK;
}

// 오직 APS HD+ Service 일 경우에만 DRM 정보를 확인한다.
STATIC DxOperator_e xmgr_rec_NagraGetServiceOptype_Ses(Handle_t  hAction)
{
#if 0
	DxOperator_e eSvcOpType = eDxOPERATOR_None;
	HUINT32 ulActionId = SVC_PIPE_GetActionId(hAction);

	if(xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType) != ERR_OK)
	{
		HxLOG_Debug("xsvc_si_LibSrchGetServiceOptype() is error\n");
	}

	HxLOG_Debug("svcOpType is Ses(%s)\n", eSvcOpType==eDxOPERATOR_Ses ? "Yes":"No");

	return 	eSvcOpType;
#endif
	return eDxOPERATOR_Ses; // 임시로 무조건 ses로 올린다.
}

STATIC HBOOL xmgr_rec_NagraIsReceivedNewSiDrmInfo_Ses(xmgrRec_Context_t *pstContext)
{
	HBOOL bChangedDrm = FALSE;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return FALSE;
	}

#if 1 // TODO : 추후 multiscreen이 drm 정보로 들어올수 있으므로 2개의 byte만 비교한다.
	if( (pstContext->stNewDrmInfo.stSiCcInfo.info.stSesDrmInfo.ucControlByteA != pstContext->stMetaInfo.stDrmInfo.stSiCcInfo.info.stSesDrmInfo.ucControlByteA) ||
	    (pstContext->stNewDrmInfo.stSiCcInfo.info.stSesDrmInfo.ucControlByteB != pstContext->stMetaInfo.stDrmInfo.stSiCcInfo.info.stSesDrmInfo.ucControlByteB) )
#else
	if(HxSTD_MemCmp(&(pstContext->stNewDrmInfo.stSiCcInfo), &(pstContext->stMetaInfo.stDrmInfo.stSiCcInfo), sizeof(DxCopyrightControl_SiCcInfo_t)) != 0)
#endif
	{
		bChangedDrm = TRUE;
	}

	HxLOG_Debug("bChanged(%d)\n", bChangedDrm);

	return bChangedDrm;
}

STATIC HERROR xmgr_rec_MsgMgrIsRestrictionDelayedRecord_Ses(xmgrRec_Context_t *pstContext, HUINT32 nSlot)
{
	HERROR rst = ERR_FAIL;
	MgrCopyrightControl_MediaControlInfo_t stDetailDrmInfo;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	if(xmgr_rec_NagraIsValidDrmInfo_Ses(&pstContext->stNewDrmInfo) != ERR_OK)
	{
		HxLOG_Error("DRMInfo is not valid\n");
		return ERR_FAIL;
	}

	if(xmgr_rec_NagraGetDrmInfo_Ses(&pstContext->stNewDrmInfo, &stDetailDrmInfo) != ERR_OK)
	{
		HxLOG_Error("xmgr_rec_NagraGetDrmInfo_Ses is error\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("eRecState(%d), eStorageRight(%d)\n", pstContext->eRecState, stDetailDrmInfo.eStorageRight);

	switch(stDetailDrmInfo.eStorageRight)
	{
		case eDxCopyrightControl_SesStorageRight_NoMore90Min:
		case eDxCopyrightControl_SesStorageRight_NoRecording:
			pstContext->stSetup.ulSlot = nSlot;
			xmgr_rec_NagraSendFailReason_Ses(pstContext, stDetailDrmInfo.eStorageRight);
			rst = ERR_OK;
			break;
		case eDxCopyrightControl_SesStorageRight_NoRestriction:
		case eDxCopyrightControl_SesStorageRight_ExpireSameDay:
		case eDxCopyrightControl_SesStorageRight_Expire3Day:
		case eDxCopyrightControl_SesStorageRight_Expire7Day:
		case eDxCopyrightControl_SesStorageRight_Expire60Day:
		default:
			break;
	}

	return rst;
}

STATIC HERROR	xmgr_rec_SetSiDrmInfoByNewDrmInfo_Ses( xmgrRec_Context_t *pstContext, HINT32 p3, HBOOL *pbChangedDrm)
{
	DxCopyrightControl_t			stDrmInfo;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	if(stDrmInfo.bSiCc == FALSE || stDrmInfo.stSiCcInfo.eSiCcType != eDxCopyrightControl_SiCcType_Ses)
	{
		HxLOG_Error("eSiCcType is not ses\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(&(pstContext->stNewDrmInfo.stSiCcInfo), 0, sizeof(DxCopyrightControl_SiCcInfo_t));
	//HxSTD_MemSet(&(pstContext->stMetaInfo.stDrmInfo.stSiCcInfo), 0, sizeof(DxCopyrightControl_SiCcInfo_t));

	pstContext->stNewDrmInfo.bSiCc = TRUE;
	pstContext->stMetaInfo.stDrmInfo.bSiCc = TRUE;

	HxSTD_MemCopy(&(pstContext->stNewDrmInfo.stSiCcInfo), &stDrmInfo.stSiCcInfo, sizeof(DxCopyrightControl_SiCcInfo_t));
	*pbChangedDrm = xmgr_rec_NagraIsReceivedNewSiDrmInfo_Ses(pstContext);
	HxSTD_MemCopy(&(pstContext->stMetaInfo.stDrmInfo.stSiCcInfo), &stDrmInfo.stSiCcInfo, sizeof(DxCopyrightControl_SiCcInfo_t));

	HxLOG_Debug("pbChangedDrm(%d)\n", *pbChangedDrm);

	return ERR_OK;
}

STATIC HERROR xmgr_rec_NagraProcessDrmStorageEncrypt_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction)
{
	HUINT32 ulContentID;
	SvcMeta_Record_t stRecInfo;
	MgrCopyrightControl_MediaControlInfo_t stDetailDrmInfo;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	if(xmgr_rec_NagraGetDrmInfo_Ses(&pstContext->stNewDrmInfo, &stDetailDrmInfo) != ERR_OK)
	{
		HxLOG_Error("xmgr_rec_NagraGetDrmInfo_Ses is error\n");
		return ERR_FAIL;
	}

	/* APS TSR인 경우, 무조건 encryption 하게 수정 */
	if(pstContext->stSetup.eRecType == eMgrRecType_TSR)
	{
		stDetailDrmInfo.bStorageEncrypt = TRUE;
	}

	ulContentID = pstContext->ulContentId;

	HxLOG_Debug("eRecType(%d), bStorageEncrypt(%d)\n", pstContext->stSetup.eRecType, stDetailDrmInfo.bStorageEncrypt);

	if(stDetailDrmInfo.bStorageEncrypt == TRUE)
	{
		if(SVC_META_GetRecordInfo(ulContentID, &stRecInfo) == ERR_OK)
		{
			HxLOG_Debug("u1cEncryption(%d)\n", stRecInfo.u1cEncryption);
			if(stRecInfo.u1cEncryption == FALSE)
			{
				stRecInfo.u1cEncryption = TRUE;
				HxLOG_Debug("Set u1cEncryption(%d)\n", stRecInfo.u1cEncryption);
				if(SVC_META_SetRecordInfo(ulContentID, &stRecInfo) != ERR_OK)
				{
					HxLOG_Debug("SVC_META_SetRecordInfo is error \n");
				}
				pstContext->stMetaInfo.u1cEncryption = TRUE;
			}
		}
	}
// TODO :  bStorageEncrypt == TRUE -> bStorageEncrypt == FALSE로 들어오는경우도 생각해 봐야함.
#if 0
	else
	{
		if(SVC_META_GetRecordInfo(ulContentID, &stRecInfo) == ERR_OK)
		{
			HxLOG_Debug("u1cEncryption(%d)\n", stRecInfo.u1cEncryption);
			if(stRecInfo.u1cEncryption == TRUE)
			{
				stRecInfo.u1cEncryption = FALSE;
				HxLOG_Debug("Set u1cEncryption(%d)\n", stRecInfo.u1cEncryption);
				if(SVC_META_SetRecordInfo(ulContentID, &stRecInfo) != ERR_OK)
				{
					HxLOG_Debug("SVC_META_SetRecordInfo is error \n");
				}
				pstContext->stMetaInfo.u1cEncryption = FALSE;
			}
		}

	}
#endif

	return ERR_OK;
}

STATIC HERROR xmgr_rec_NagraProcessDrmStorageRight_Ses(xmgrRec_Context_t *pstContext, HBOOL bChangedDrm)
{
	HUINT32 ulContentID;
	SvcMeta_Record_t stRecInfo;
	MgrCopyrightControl_MediaControlInfo_t stDetailDrmInfo;
	eDxCopyrightControl_SesStorageRight_e eStorageRight;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	if(xmgr_rec_NagraIsValidDrmInfo_Ses(&pstContext->stNewDrmInfo) != ERR_OK)
	{
		HxLOG_Error("DRMInfo is not valid\n");
		return ERR_FAIL;
	}

	if(xmgr_rec_NagraGetDrmInfo_Ses(&pstContext->stNewDrmInfo, &stDetailDrmInfo) != ERR_OK)
	{
		HxLOG_Error("xmgr_rec_NagraGetDrmInfo_Ses is error\n");
		return ERR_FAIL;
	}

	ulContentID = pstContext->ulContentId;
	eStorageRight = stDetailDrmInfo.eStorageRight;

	HxLOG_Debug("eRecState(%d), eStorageRight(%d), eRecType(%d)\n", pstContext->eRecState, eStorageRight, pstContext->stSetup.eRecType);

	switch(eStorageRight)
	{
		case eDxCopyrightControl_SesStorageRight_NoRestriction:	/* no restrictions */
			break;
		case eDxCopyrightControl_SesStorageRight_NoMore90Min:		/* no recording apart from temporary buffering up to 90 minutes */
			if(pstContext->stSetup.eRecType != eMgrRecType_TSR)
			{
				xmgr_rec_NagraSendFailReason_Ses(pstContext, eDxCopyrightControl_SesStorageRight_NoMore90Min);
				if((pstContext->stSetup.eRecType == eMgrRecType_EBR) ||(pstContext->stSetup.eRecType == eMgrRecType_TBR) || (pstContext->stSetup.eRecType == eMgrRecType_SBR))
				{
					HxSTD_memset(&stRecInfo, 0, sizeof(SvcMeta_Record_t));
					if(SVC_META_GetRecordInfo(ulContentID, &stRecInfo) == ERR_OK)
					{
						// TODO: sub param으로 향마다 특정한 값 저장할수 있도록. ex) eDxCopyrightControl_SesStorageRight_NoMore90Min
						MGR_RECORDER_SetRecordFailReason(ulContentID, eMgrRecFailed_DRM); // eMgrRecFailed_UNKNOWN
					}
				}
				BUS_PostMessage(xproc_rec_Proto_Ses, eMEVT_BUS_DESTROY, pstContext->hAction, 0, 0, 0);
				return MESSAGE_BREAK;
			}
			break;
		case eDxCopyrightControl_SesStorageRight_NoRecording:		/* no recording at all */ // tsr도 안되야하는데 icordmini에는 tsr은 동작하도록 됨.
			if(pstContext->stSetup.eRecType != eMgrRecType_TSR)
			{
				xmgr_rec_NagraSendFailReason_Ses(pstContext, eDxCopyrightControl_SesStorageRight_NoRecording);
				if((pstContext->stSetup.eRecType == eMgrRecType_EBR) ||(pstContext->stSetup.eRecType == eMgrRecType_TBR) || (pstContext->stSetup.eRecType == eMgrRecType_SBR))
				{
					HxSTD_memset(&stRecInfo, 0, sizeof(SvcMeta_Record_t));
					if(SVC_META_GetRecordInfo(ulContentID, &stRecInfo) == ERR_OK)
					{
						// TODO: sub param으로 향마다 특정한 값 저장할수 있도록. ex) eDxCopyrightControl_SesStorageRight_NoRecording
						MGR_RECORDER_SetRecordFailReason(ulContentID, eMgrRecFailed_DRM); //eMgrRecFailed_UNKNOWN
					}
				}
				BUS_PostMessage(xproc_rec_Proto_Ses, eMEVT_BUS_DESTROY, pstContext->hAction, 0, 0, 0);
				return MESSAGE_BREAK;
			}
			else
			{
				xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);
				xmgr_rec_NagraProcessSetDrmInfo_Ses(pstContext, TRUE); /* set mata data & timestame */
				HxLOG_Debug("[%s:%d] xmgr_rec_StartRec\n", __FUNCTION__, __LINE__);
				//xmgr_rec_StartRec(pstContext);
				xmgr_rec_SetPauseFlag(pstContext, eRecPause_DRM);
				return MESSAGE_BREAK;
			}
			break;
		case eDxCopyrightControl_SesStorageRight_ExpireSameDay:
		case eDxCopyrightControl_SesStorageRight_Expire3Day:
		case eDxCopyrightControl_SesStorageRight_Expire7Day:
		case eDxCopyrightControl_SesStorageRight_Expire60Day:
		default:		/* no restrictions 으로 처리한다. expiration 은 playback 에서의 제한이다. */
			break;
	}

	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		if((pstContext->stSetup.eRecType == eMgrRecType_TSR) && (pstContext->eRecState == eMgrRecState_BLOCKED))
		{
			HxLOG_Debug("RecResume\n");
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
		}
		else
		{
			xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);
			xmgr_rec_NagraProcessSetDrmInfo_Ses(pstContext, TRUE); /* set mata data & timestame */
			HxLOG_Debug("[%s:%d] xmgr_rec_StartRec\n", __FUNCTION__, __LINE__);
		}
	}
	else if(pstContext->eRecState == eMgrRecState_RECORDING)
	{
		SvcRec_RecInfo	stRecInfo;
		Handle_t			hPlayback = HANDLE_NULL;
		HUINT32 		ulRecTime;

		HxLOG_Debug("Recording...Set DrmInfo....bChangedDrm(%d)\n", bChangedDrm);

		if(bChangedDrm)
		{
			HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));

			if(SVC_REC_GetInfo (pstContext->hAction, &stRecInfo) == ERR_OK)
			{
				ulRecTime = stRecInfo.ulDuration;

				if (0 == ulRecTime)
				{
					ulRecTime = 1;
				}

				HxLOG_Debug("ulRecTime(%d)\n", ulRecTime);

				if(SVC_META_SetDrmTimeStamp(pstContext->ulContentId, ulRecTime, &pstContext->stNewDrmInfo ) == ERR_OK)
				{
					if(SVC_META_GetPlaybackActionByContentId(pstContext->ulContentId, &hPlayback) == ERR_OK)
					{
						BUS_SendMessage(NULL, eMEVT_PVR_RECORD_TIMESTAMP_UPDATE, pstContext->hAction, 0, 0, 0); // TODO : 확인해봐야함.
					}
					else
					{
						HxLOG_Debug("SVC_META_GetPlaybackActionByContentId is error\n");
					}
				}
				else
				{
					HxLOG_Debug("SVC_META_SetDrmTimeStamp is error\n");
				}
			}
			else
			{
				HxLOG_Debug("SVC_REC_GetInfo is error\n");
			}
		}
	}
	else
	{
		if((pstContext->stSetup.eRecType == eMgrRecType_TSR) && (pstContext->eRecState == eMgrRecState_BLOCKED))
		{
			HxLOG_Debug("RecResume\n");
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_DRM);
		}
	}

	return ERR_OK;
}

STATIC HERROR xmgr_rec_NagraProcessSetDrmInfo_Ses(xmgrRec_Context_t *pstContext, HBOOL bMakeTimeStamp)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32			ulRecTime;
	SvcRec_RecInfo 	stRecInfo;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return hErr;
	}

	if(SVC_META_SetDRMInfo (pstContext->ulContentId, &pstContext->stNewDrmInfo, TRUE) == ERR_OK)
	{
		if(bMakeTimeStamp == TRUE)
		{
			HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));

			if(SVC_REC_GetInfo (pstContext->hAction, &stRecInfo) == ERR_OK)
			{
				ulRecTime = stRecInfo.ulDuration;

				if (0 == ulRecTime)
				{
					ulRecTime = 1;
				}

				if(SVC_META_SetDrmTimeStamp(pstContext->ulContentId, ulRecTime, &pstContext->stNewDrmInfo ) == ERR_OK)
				{
					hErr = ERR_OK;
					BUS_SendMessage(NULL, eMEVT_PVR_RECORD_TIMESTAMP_UPDATE, pstContext->hAction, pstContext->ulContentId, 0, 0);
				}
				else
				{
					HxLOG_Debug("SVC_META_SetDrmTimeStamp is error\n");
				}
			}
			else
			{
				HxLOG_Debug("SVC_REC_GetInfo is error\n");
			}
		}
	}
	else
	{
		HxLOG_Debug("SVC_META_SetDRMInfo is error\n");
	}

	return hErr;
}

STATIC HERROR xmgr_rec_NagraProcessDrmInfo_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction, HBOOL bChangedDrm)
{
	HERROR	hErr = ERR_FAIL;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return hErr;
	}

	if(xmgr_rec_NagraIsValidDrmInfo_Ses(&pstContext->stNewDrmInfo) != ERR_OK)
	{
		HxLOG_Error("xmgr_rec_NagraIsValidDrmInfo_Ses is not valid\n");
		return ERR_FAIL;
	}

	xmgr_rec_SetSCPinCodeFlag_Ses(pstContext, hAction);
	/* set drm for recorder */
	xmgr_rec_NagraProcessDrmStorageEncrypt_Ses(pstContext, hAction);
	/* set rec condition */
	xmgr_rec_NagraProcessDrmStorageRight_Ses(pstContext, bChangedDrm);
	/* set mata data & timestame */
	//xmgr_rec_NagraProcessSetDrmInfo_Ses(pstContext, TRUE);

	return ERR_OK;
}

HERROR xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction, HBOOL bChangedDrm)
{
	HERROR 	hErr = ERR_FAIL;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext or pstDrmInfo is NULL\n");
		return hErr;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("eRecState = %d, bChangedDrm = %d\n", pstContext->eRecState, bChangedDrm);
	xmgr_rec_PrintDrmInfo_Ses(pstContext, eSEVT_CAS_DRM_SIGNALLED);
#endif

	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		//if((pstContext->bCasDrmValid == TRUE) && (pstContext->bCasDrmNone == FALSE))
		{
			hErr = xmgr_rec_NagraProcessDrmInfo_Ses(pstContext, hAction, bChangedDrm);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_rec_NaProcessDrm_Ses() ERROR.\n");
				return hErr;
			}
		}
	}
	else
	{
		if(bChangedDrm == TRUE)
		{
			hErr = xmgr_rec_NagraProcessDrmInfo_Ses(pstContext, hAction, bChangedDrm);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_rec_NaProcessDrm_Ses() ERROR.\n");
				return hErr;
			}
		}
		else
		{
			if(pstContext->stSetup.eRecType == eMgrRecType_TSR)
			{
				/* set drm for recorder */
				xmgr_rec_NagraProcessDrmStorageEncrypt_Ses(pstContext, hAction);
#if defined(CRITICAL_TMP)
	#if 0
				xmgr_rec_NagraProcessSetDrmInfo_Ses(pstContext, TRUE); /* set mata data & timestame */
	#endif
#endif
			}
		}
	}

	return hErr;
}

HERROR xmgr_rec_NagraMsgEvtCasFail_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction)
{
	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	if (pstContext->eRecState == eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return ERR_FAIL;
	}

	if(xmgr_rec_NagraGetServiceOptype_Ses(hAction) != eDxOPERATOR_Ses)
	{
		HxLOG_Error("Optype is not  eDxOPERATOR_Ses\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("eRecState(%d)\n", pstContext->eRecState);

	if(pstContext->stSetup.eRecType == eMgrRecType_TSR)
	{
		if (pstContext->eRecState == eMgrRecState_BLOCKED)
		{
			HxLOG_Debug("eRecState == eMgrRecState_BLOCKED\n");
		}
		else
		{
			HxLOG_Debug("eRecState(%d) not blocked\n", pstContext->eRecState);
			// ci tsr 안되는 문제 수정
			if(pstContext->eRecState == eMgrRecState_RECORDING)
			{
				xmgr_rec_SetPauseFlag(pstContext, eRecPause_CAS);
			}
		}
	}

	return ERR_OK;
}

HERROR xmgr_rec_NagraMsgEvtCasOK_Ses(xmgrRec_Context_t *pstContext, Handle_t hAction)
{
	DxCopyrightControl_CasCcType_e eCasCcType;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_FAIL;
	}

	if(xmgr_rec_NagraGetServiceOptype_Ses(hAction) != eDxOPERATOR_Ses)
	{
		HxLOG_Error("Optype is not  eDxOPERATOR_Ses\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("eRecState(%d)\n", pstContext->eRecState);
	if(pstContext->stSetup.eRecType == eMgrRecType_TSR)
	{
		if(pstContext->eRecState == eMgrRecState_BLOCKED)
		{
			xmgr_rec_ClearPauseFlag(pstContext, eRecPause_CAS);

			eCasCcType = pstContext->stNewDrmInfo.stCasCcInfo.eCasCcType;

			if((eCasCcType != eDxCopyrightControl_CasCcType_None) && (eCasCcType != eDxCopyrightControl_CasCcType_Max))
			{
				// 기존 코드 의미 해석 필요(resume rec(=clean pause flag) 이후에 wait drm state로 가서 무엇을 기대하고 있는 것인지)
				xmgr_rec_SetPauseFlag(pstContext, eRecPause_DRM);
			}
		}
	}

	return ERR_OK;
}

// TODO : DrmNono과 DrmUnKnow의 차이점을 비교해서 작업을 다시 해주어야함. DrmUnknown 인경우 처리를 어떻게 ?
BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmUnknown_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL bChangedDrm = FALSE;
	MgrCopyrightControl_MediaControlInfo_t stDrmInfo;
	HERROR hErr = ERR_FAIL;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxLOG_Debug("eRecState(%d)\n", pstContext->eRecState);
	if (pstContext->eRecState == eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return MESSAGE_PASS;
	}

	pstContext->bCasDrmValid = FALSE;
	pstContext->bCasDrmNone = TRUE;

	// CAS 채널인 경우에만 CAS_DRM_NONE 이 온다.
	// CAS_DRM_NONE 역시 ECM 을 통해 들어 오므로, EIT DRM 보다 우선한다.
	hErr = pmgr_copyrightcontrol_InitCasCopyProtectInfo(&pstContext->stNewDrmInfo, &stDrmInfo);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_copyrightcontrol_InitCasCopyProtectInfo Error...\n");
		return MESSAGE_PASS;
	}

	bChangedDrm = xmgr_rec_NagraIsReceivedNewCasDrmInfo(pstContext);

	xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(pstContext, hAction, bChangedDrm);

	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		return MESSAGE_BREAK;
	}

	return ERR_BUS_NO_ERROR;
}

// TODO : DrmNono과 DrmUnKnow의 차이점을 비교해서 작업을 다시 해주어야함.
BUS_Result_t	xmgr_rec_NagraMsgEvtCasDrmNone_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL bChangedDrm = FALSE;
	MgrCopyrightControl_MediaControlInfo_t stDrmInfo;
	HERROR hErr = ERR_FAIL;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxLOG_Debug("eRecState(%d)\n", pstContext->eRecState);
	if (pstContext->eRecState == eMgrRecState_NO_SIGNAL)
	{
		HxLOG_Error("eRecState is eMgrRecState_NO_SIGNAL\n");
		return MESSAGE_PASS;
	}

	pstContext->bCasDrmValid = FALSE;
	pstContext->bCasDrmNone = TRUE;

	// CAS 채널인 경우에만 CAS_DRM_NONE 이 온다.
	// CAS_DRM_NONE 역시 ECM 을 통해 들어 오므로, EIT DRM 보다 우선한다.
	hErr = pmgr_copyrightcontrol_InitCasCopyProtectInfo(&pstContext->stNewDrmInfo, &stDrmInfo);
	if (hErr == ERR_FAIL)
	{
		// eDxCopyrightControl_CasCcType_NagraSes이 아닌경우 Fail로 리턴하며 xmgr_rec_NagraProcessDrmInfo_Ses 함수로 진입 말아야 함.
		// SES code에서 시퀀스가 꼬이게 만드는 부분이 많아 원작자인 안광천님께 의도를 파악하고, 근본적인 정리작업이 필요 함.

		// DRM_NONE 이 호출되면 정상적인 DRM을 획득하기 전이라도 Recording Start가 되어야 함. (ex. scrambled recording 에서는 DRM_NONE일 때 Rec start.)
		xmgr_rec_ClearWaitFlag(pstContext, eRecWait_DRM);

		HxLOG_Error("pmgr_copyrightcontrol_InitCasCopyProtectInfo Error...\n");
		return MESSAGE_PASS;
	}

	bChangedDrm = xmgr_rec_NagraIsReceivedNewCasDrmInfo(pstContext);

	// TODO : fta channel에서 먼저 ciplus에서  rec가 걸려 encryption rec fail이 난다. 그러나 playback인경우 encryption 으로 play함으로 play가 안되는
	// 문제가 발생하여 임시로 fta channel인경우 encryption rec을 걸지 않게 만든다.
    	// tsr도 aps 채널인경우에는 tsr도 encryption을 걸어주어야한다.
	if(pstContext->eRecState == eMgrRecState_WAIT)
	{
		//if(svc_cas_MgrGetWriteTimestampFlagForDrmNone(hAction) == TRUE)
		{
			if(xmgr_rec_NagraProcessDrmInfo_Ses(pstContext, hAction, bChangedDrm) == ERR_FAIL)
			{
				xmgr_rec_ClearWaitFlag(pstContext,eRecWait_DRM);
				HxLOG_Debug("[%s:%d] xmgr_rec_StartRec\n", __FUNCTION__, __LINE__);
				//xmgr_rec_StartRec(pstContext);
			}
		}
		return MESSAGE_BREAK;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_rec_MsgEvtSiPmtReceived_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s][%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return MESSAGE_PASS;
	}

	// cipluse인지 base or nagra인지 확인 필요.
	//eBusResult = xmgr_rec_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
#if defined(CONFIG_MW_CI_PLUS)
	eBusResult = xmgr_rec_CiPlusMsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
#else
	eBusResult = xmgr_rec_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
#endif

	return eBusResult;
}

STATIC BUS_Result_t xmgr_rec_MsgEvtCasDrmSignalled_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	DxCopyrightControl_t	stDrmInfo;
	DxCopyrightControl_CasCcType_e			eCasCcType;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	eCasCcType = stDrmInfo.stCasCcInfo.eCasCcType;

	HxLOG_Debug("eDrmType(%d)\n", eCasCcType);

	switch(eCasCcType)
	{
		case eDxCopyrightControl_CasCcType_NagraSes:
			eBusResult = xmgr_rec_NagraMsgEvtCasDrmSignalled_Base (pstContext, hAction, p1, p2, p3);
			break;
		case eDxCopyrightControl_CasCcType_CiPlus:
#if defined(CONFIG_MW_CI_PLUS)
			eBusResult = xmgr_rec_CiPlusMsgEvtCasDrmSignalled_Base (pstContext, hAction, p1, p2, p3);
#endif
			break;
		default:
			eBusResult = ERR_BUS_NO_ERROR;
			break;
	}

	return eBusResult;
}

STATIC BUS_Result_t xmgr_rec_MsgEvtSiDrmSignalled_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL 							bChangedDrm = FALSE;
	SvcCas_ChannelType_e 			eCasType = 0;
	SvcMeta_Record_t				stRecInfo;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

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

#if defined(CONFIG_MW_CI_PLUS)
	// CI+ DRM을 획득한 Recording File은 CI+CAM에 의하여 Contents Control이 이뤄지므로, SES 사양의 DRM은 혼란을 주므로 적용하지 않도록 함.
	if(pstContext->stNewDrmInfo.bCasCc == TRUE)
	{
		if(pstContext->stNewDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
		{
			HxLOG_Info("CI+ DRM is already affected... SI DRM info is going to be ignored...\n");
			return MESSAGE_PASS;
		}
	}
#endif

	if(xmgr_rec_NagraGetServiceOptype_Ses(hAction) != eDxOPERATOR_Ses)
	{
		HxLOG_Error("Optype is not  eDxOPERATOR_Ses\n");
		return ERR_BUS_NO_ERROR;
	}

	if(xmgr_rec_SetSiDrmInfoByNewDrmInfo_Ses(pstContext, p3, &bChangedDrm) != ERR_OK)
	{
		return ERR_BUS_NO_ERROR;
	}

	if(bChangedDrm != TRUE)
	{
		return ERR_BUS_NO_ERROR;
	}

	HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcMeta_Record_t));
	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) == ERR_OK)
	{
		HxSTD_MemCopy(&stRecInfo.stDrmInfo.stSiCcInfo, &pstContext->stMetaInfo.stDrmInfo.stSiCcInfo, sizeof(DxCopyrightControl_SiCcInfo_t));
		if(SVC_META_SetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
		{
			HxLOG_Error("SVC_META_SetRecordInfo : Error\n");
		}
	}

	if(pstContext->bPMTValid == TRUE)
	{
		if(SVC_CAS_GetChannelType(hAction, &eCasType) == ERR_OK)
		{
			HxLOG_Debug("eCasType = %d\n", eCasType);
#if defined (CONFIG_DEBUG)
			xmgr_rec_PrintDrmInfo_Ses(pstContext, eSEVT_SI_DRM_SIGNALLED);
#endif
			if(eCasType == eCasChannel_FTA)
			{
				xmgr_rec_SetSCPinCodeFlag_Ses(pstContext, hAction);

#ifdef CRITICAL_TMP // TODO : fta channel에서 먼저 ciplus에서  rec가 걸려 encryption rec fail이 난다. 그러나 playback인경우 encryption 으로 play함으로 play가 안되는
      				// 문제가 발생하여 임시로 fta channel인경우 encryption rec을 걸지 않게 만든다.
      				// tsr도 aps 채널인경우에는 tsr도 encryption을 걸어주어야한다.
      				// cas drm 정보가 우선순위가 높으므로 cas drm이 true인경우에는 si drm을 실행하지 않는다.
				#if 0
	      			if(xmgr_rec_NagraIsValidCasDrmInfo_Ses(&pstContext->stMetaInfo.stDrmInfo) == ERR_FAIL)
	      			{
						// si로 drm을 setting하는경우
						if(pstContext->eRecState == eMgrRecState_WAIT)
						{
							xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(pstContext, hAction, bChangedDrm);
						}
						else
						{
							if(bChangedDrm == TRUE)
							{
								xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(pstContext, hAction, bChangedDrm);
							}
							else
							{
								xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(pstContext, hAction, bChangedDrm);
							}
						}

	      			}
				#endif
#else
				xmgr_rec_NagraProcessDrmInfoByDrmSignalled_Ses(pstContext, hAction, bChangedDrm);
#endif
			}
		}
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t	xmgr_rec_MsgEvtCasOK_Ses ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

#if defined(CONFIG_DEBUG)
	xmgr_rec_PrintDrmInfo_Ses( pstContext, eSEVT_CAS_OK);
#endif

#if 0
	/* ci plus와 nagra를 구분해서 정리 필요. 아니면 같이 사용할 것인지. */
	if(p1 == eDxCAS_GROUPID_NA)
	{
		eBusResult = xmgr_rec_NagraMsgEvtCasOK_Base (pstContext, hAction, p1, p2, p3);
	}
	else if(p1 == eDxCAS_GROUPID_CAM)
	{
		eBusResult = xmgr_rec_CiPlusMsgEvtCasOK_Base (pstContext, hAction, p1, p2, p3);
	}
	else
	{
		eBusResult = xmgr_rec_CiPlusMsgEvtCasOK_Base (pstContext, hAction, p1, p2, p3);
	}
#else
	// ci와 card가 insert되지 않은경우 tsr인경우에 record pause를 한다. 그러나 ci 삽입시 이걸 다시 풀어주는 코드가 ci에는 없어
	// nagra소스로 대체한다.
	eBusResult = xmgr_rec_NagraMsgEvtCasOK_Base (pstContext, hAction, p1, p2, p3);
#endif

	return eBusResult;
}

BUS_Result_t	xmgr_rec_MsgEvtCasFail_Ses ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

#if defined(CONFIG_DEBUG)
	xmgr_rec_PrintDrmInfo_Ses( pstContext, eSEVT_CAS_FAIL);
#endif

	/* ci plus와 nagra를 구분해서 정리 필요. 아니면 같이 사용할 것인지. */
	if(p1 == eDxCAS_GROUPID_NA)
	{
		eBusResult = xmgr_rec_NagraMsgEvtCasFail_Base (pstContext, hAction, p1, p2, p3);
	}
	else
	{
#if defined(CONFIG_MW_CI_PLUS)
		eBusResult = xmgr_rec_CiPlusMsgEvtCasFail_Base (pstContext, hAction, p1, p2, p3);
#endif
	}

	return eBusResult;
}

BUS_Result_t	xmgr_rec_MsgEvtCasDrmNone_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

#if defined(CONFIG_DEBUG)
	xmgr_rec_PrintDrmInfo_Ses( pstContext, eSEVT_CAS_DRM_NONE);
#endif

	/* ci plus와 nagra를 구분해서 정리 필요. 아니면 같이 사용할 것인지. */
	// 1. case : Nagra + Ses
	eBusResult = xmgr_rec_NagraMsgEvtCasDrmNone_Base(pstContext, hAction, p1, p2, p3);
	// 2. case : CiPlus
	//eBusResult = xmgr_rec_CiPlusMsgEvtCasDrmNone_Base (pstContext, hAction, p1, p2, p3);

	return eBusResult;
}

BUS_Result_t	xmgr_rec_MsgEvtCasDrmUnknown_Ses( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;

	HxLOG_Debug("[%s:%d]\n", __FUNCTION__, __LINE__);

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	if(p1 == eDxCopyrightControl_CasCcType_CiPlus)
	{
#if defined(CONFIG_MW_CI_PLUS)
		eBusResult = xmgr_rec_CiPlusMsgEvtCasDrmUnknown_Base (pstContext, hAction, p1, p2, p3);
#endif
	}

	return eBusResult;
}

STATIC BUS_Result_t xproc_rec_Proto_Ses (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_recorder_GetMessageName_Ses(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("********************************************************************************\n");
		HxLOG_Print("*\n");
		HxLOG_Print("* [XMGR:RecSes] MESSAGE (%s), ulActionId(%x), hAction(%x), p1(%x), p2(%x), p3(%x), eWaitFlag(0x%08x)\n", pszMsg, ulActionId, hAction, p1, p2, p3, pstContext->eWaitFlag);
		HxLOG_Print("*\n");
		HxLOG_Print("********************************************************************************\n");
	}
#endif

	switch (message)
	{
		/*MSG*/
		case eMEVT_BUS_READY_SHUTDOWN :
			eBusResult = xmgr_rec_MsgBusGoStandby_Base(pstContext, hAction, p1, p2, p3);
			return MESSAGE_PASS;

		case eMEVT_BUS_CREATE:
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_DRM);
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_PMT);

			eBusResult = xmgr_rec_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);

			/* update status to svc_meta */
			{
				HERROR hErr;
				hErr = SVC_META_RegisterPvrAction(pstContext->hAction, pstContext->ulContentId, eSvcMeta_UsingActType_Record);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SVC_META_RegisterPvrAction. Error\n");
				}
			}
			break;

		case eMEVT_BUS_DESTROY:
			eBusResult = xmgr_rec_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgBusTimer_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD:
			CHECK_REC_ACTION_HANDLE();
#if defined(CONFIG_DEBUG)
			xmgr_rec_PrintDrmInfo_Ses(pstContext, message);
#endif
			if(xmgr_rec_MsgMgrIsRestrictionDelayedRecord_Ses(pstContext, p1) == ERR_OK)
			{
				eBusResult = MESSAGE_PASS;
			}
			else
			{
				eBusResult = xmgr_rec_MsgMgrRecordChangeTsrRecToDelayedRecord_Base(pstContext, hAction, p1, p2, p3);
			}
			break;

		case eMEVT_PVR_RECORD_ENDTIME_UPDATE :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgMgrRecordEndtimeUpdate_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-SI*/
		case eSEVT_SI_PMT:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiPmtReceived_Ses(pstContext, hAction, p1, p2, p3);
			xmgr_rec_ClearWaitFlag(pstContext,eRecWait_PMT);
			break;

		case eSEVT_SI_PAT_TIMEOUT :
		case eSEVT_SI_PMT_TIMEOUT :
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiPmtTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiEitReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiEitTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_MULTIFEED_DETECTED :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiMultifeedDetected_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_FREE_CA_MODE_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiSdtFreeCaModeChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_RUNNING_STATUS_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiSdtRunningStatusChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-CH*/
		case eSEVT_CH_NO_SIGNAL :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtChNoSignal_Base(pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CH_MOVING_MOTOR :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtChMovingMotor_Base(pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CH_LOCKED :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtChLocked_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_STORAGE_NOTIFY_UNPLUGGED:
			if (p2 == FALSE)	// Ext Hdd
			{
				eBusResult = xmgr_rec_MsgEvtExtStorageDisconnected_Base (pstContext, hAction, p1, p2, p3);
			}
			break;

		/* EVT-DB */
		case eSEVT_DB_CHECK_SVC_DB :
			eBusResult = xmgr_rec_MsgEvtDbCheckSvcDb_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-SVC_Rec */
		case eSEVT_REC_PID_CHANGED:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtRecPidChanged_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_TIMECODE_CHANGE :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtRecTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_SCRAMBLED:
			CHECK_REC_ACTION_HANDLE();
#if defined(CONFIG_DEBUG)
			xmgr_rec_PrintDrmInfo_Ses(pstContext, eSEVT_REC_SCRAMBLED);
#endif
			eBusResult = xmgr_rec_MsgEvtRecScrambled_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtRecDelayedRecStatusChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-Xmgr_Rec */
		case eSEVT_REC_REQ_COMMAND:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtReqCommand_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtCasDrmSignalled_Ses(pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_SI_DRM_SIGNALLED:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtSiDrmSignalled_Ses (pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CAS_OK :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtCasOK_Ses (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL :
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtCasFail_Ses (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_NONE:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtCasDrmNone_Ses (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_UNKNOWN:
			CHECK_REC_ACTION_HANDLE();
			eBusResult = xmgr_rec_MsgEvtCasDrmUnknown_Ses (pstContext, hAction, p1, p2, p3);
			break;

		default :
			break;
	}

	xmgr_rec_RunState(pstContext);

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


STATIC BUS_Result_t xproc_rec_Action0_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (0);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action1_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (1);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action2_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (2);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action3_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (3);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action4_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (4);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action5_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (5);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action6_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (6);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action7_Ses (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (7);

	return xproc_rec_Proto_Ses (pstContext, message, hAction, p1, p2, p3);
}

#define _____MSG_PROCESSING_FUNCTIONS_____
BUS_Callback_t xmgr_recorder_GetProcFunc_Ses (HUINT32 ulActionId)
{
	// Code 내에서 Record 갯수가 최대 몇개 인지 알기 힘드므로
	// switch 함수 위에 ulActionID 영역을 확인하여 실제로 넘치는 값이어도 logic상 들어오지 않게 만든다.
	if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		switch (ulActionId)
		{
		case (eActionId_REC_FIRST + 0):
			return xproc_rec_Action0_Ses;

		case (eActionId_REC_FIRST + 1):
			return xproc_rec_Action1_Ses;

		case (eActionId_REC_FIRST + 2):
			return xproc_rec_Action2_Ses;

		case (eActionId_REC_FIRST + 3):
			return xproc_rec_Action3_Ses;

		case (eActionId_REC_FIRST + 4):
			return xproc_rec_Action4_Ses;

		case (eActionId_REC_FIRST + 5):
			return xproc_rec_Action5_Ses;

		case (eActionId_REC_FIRST + 6):
			return xproc_rec_Action6_Ses;

		case (eActionId_REC_FIRST + 7):
			return xproc_rec_Action7_Ses;

		default:
			break;
		}
	}

	return NULL;
}
