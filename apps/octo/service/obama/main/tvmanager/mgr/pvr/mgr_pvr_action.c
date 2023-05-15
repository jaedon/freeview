#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_fs.h>
#include <svc_rec.h>


#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>

#include <mgr_pvr_action.h>
#include <pmgr_pvr_action.h>


#define ___DEBUG_FUNCTIONS___

#if defined(CONFIG_DEBUG)
STATIC int mgr_pvr_printPvrStatus (void *szArgList)
{
	pmgr_pvr_PrintPvrStatus();

	NOT_USED_PARAM (szArgList);
	return HxCMD_OK;
}

STATIC int mgr_pvr_cmdStartExternalTsr (void *szArgList)
{
	HINT32			 nSvcNo;
	Handle_t		 hSvc = HANDLE_NULL;
	HCHAR			*szSvcNum;
	HERROR			 hErr;
	
	szSvcNum = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if ((NULL == szSvcNum) || ('\0' == *szSvcNum))		{ return HxCMD_ERR; }

	nSvcNo = HLIB_STD_StrToINT32((const HCHAR *)szSvcNum);

	hSvc = HANDLE_NULL;
	hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, nSvcNo, &hSvc);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hSvc))
	{
		hSvc = HANDLE_NULL;
		hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_CAB, eDxSVC_TYPE_TV, nSvcNo, &hSvc);
		if ((ERR_OK != hErr) || (HANDLE_NULL == hSvc))
		{
			hSvc = HANDLE_NULL;
			hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_TER, eDxSVC_TYPE_TV, nSvcNo, &hSvc);
			if ((ERR_OK != hErr) || (HANDLE_NULL == hSvc))
			{
				HLIB_CMD_Printf(">>> Svc No [%d] Wrong:\n", nSvcNo);
				return HxCMD_ERR;
			}
		}
	}

	hErr = MGR_PVR_StartTsrRecording(HANDLE_NULL, hSvc, hSvc, 0, eTsrType_ExternalRequest, 0);
	if (ERR_OK != hErr)
	{
		HLIB_CMD_Printf(">>> MGR_PVR_StartTsrRecording failed:\n");
		return HxCMD_ERR;
	}
	
 	NOT_USED_PARAM (szArgList);
	return HxCMD_OK;
}

STATIC int mgr_pvr_cmdStopExternalTsr (void *szArgList)
{
	HINT32			 nSvcNo;
	Handle_t		 hSvc = HANDLE_NULL;
	HCHAR			*szSvcNum;
	HERROR			 hErr;
	
	szSvcNum = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if ((NULL == szSvcNum) || ('\0' == *szSvcNum))		{ return HxCMD_ERR; }

	nSvcNo = HLIB_STD_StrToINT32((const HCHAR *)szSvcNum);

	hSvc = HANDLE_NULL;
	hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eDxSVC_TYPE_TV, nSvcNo, &hSvc);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hSvc))
	{
		hSvc = HANDLE_NULL;
		hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_CAB, eDxSVC_TYPE_TV, nSvcNo, &hSvc);
		if ((ERR_OK != hErr) || (HANDLE_NULL == hSvc))
		{
			hSvc = HANDLE_NULL;
			hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_TER, eDxSVC_TYPE_TV, nSvcNo, &hSvc);
			if ((ERR_OK != hErr) || (HANDLE_NULL == hSvc))
			{
				HLIB_CMD_Printf(">>> Svc No [%d] Wrong:\n", nSvcNo);
				return HxCMD_ERR;
			}
		}
	}

	hErr = MGR_PVR_StopTsrRecording(hSvc, hSvc, 0);
	if (ERR_OK != hErr)
	{
		HLIB_CMD_Printf(">>> MGR_PVR_StopTsrRecording failed:\n");
		return HxCMD_ERR;
	}
	
 	NOT_USED_PARAM (szArgList);
	return HxCMD_OK;
}

STATIC void mgr_pvr_registerCmd (void)
{
#define	hCmdHandle		"mgr_pvr"
	HLIB_CMD_RegisterWord(hCmdHandle,mgr_pvr_printPvrStatus,
						"status",											/* keyword */
						"View the current PVR module Status (Manager)",	 		/*   help  */
						"status");											/*  usage  */

	HLIB_CMD_RegisterWord(hCmdHandle, mgr_pvr_cmdStartExternalTsr,
						"start_tsr_ext",											/* keyword */
						"Start the external TSR", 		/*	 help  */
						"start_tsr_ext [tv_channel_no]");											/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,mgr_pvr_cmdStopExternalTsr,
						"stop_tsr_ext",											/* keyword */
						"Stop the external TSR",		/*	 help  */
						"stop_tsr_ext [tv_channel_no]");											/*	usage  */
}
#endif



#define ___PVR_ACTION___
HERROR MGR_PVR_Init (void)
{
	pmgr_pvr_Create();

#if defined(CONFIG_DEBUG)
	mgr_pvr_registerCmd();
#endif

	return ERR_OK;
}

HERROR MGR_PVR_CheckHddEnable(void)
{
	HERROR	hErr;
	HBOOL	bIsAvailable = FALSE;

	hErr = pmgr_pvr_CheckPvrDevStatus(&bIsAvailable);
	if (ERR_OK == hErr)
	{
		hErr = (bIsAvailable) ? ERR_OK : ERR_FAIL;
	}

	return hErr;
}

HERROR MGR_PVR_CheckPVRDeivceUsed(HUINT32 ulPvrIdx)
{
	return pmgr_pvr_CheckPVRDeivceUsed(ulPvrIdx);
}
// 해당 PvrIdx HDD에 대한 Rec/Pb 동작을 정지하는 API
HERROR MGR_PVR_StopPvrFunction(HUINT32 ulPvrIdx)
{
	return pmgr_pvr_StopPvrFunction(ulPvrIdx);
}

HERROR MGR_PVR_MakeThumbnail (HUINT32 ulContentId)
{
	return pmgr_pvr_MakeThumbnail (ulContentId);
}

#define ___PVR_PLAYBACK___
HERROR MGR_PVR_StartPlayback (HUINT32 ulContentId, HUINT32 ulStartTime)
{
	return pmgr_pvr_StartPlayback (ulContentId, ulStartTime);
}

HERROR MGR_PVR_StartTsrChasePlayback (HUINT32 ulActionId, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPos, HUINT32 ulOffset, MgrPb_Option_e ePbOption)
{
	return pmgr_pvr_StartTsrChasePlayback (ulActionId, eTrick, ePbPos, ulOffset, ePbOption);
}

HBOOL MGR_PVR_GetTsrPlaybackEnable(void)
{
#if defined(CONFIG_MW_MM_PVR)
	return TRUE;
#else
	return FALSE;
#endif
}

HBOOL MGR_PVR_GetChasePlaySurferEnable(void)
{
#if defined(CONFIG_MW_MM_PVR)
	return TRUE;
#else
	return FALSE;
#endif
}

#define ___PVR_RECORDER___
HERROR MGR_PVR_CheckRecordPossible (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason)
{
	if (pstRecSetup == NULL)			{ return ERR_FAIL; }

	return pmgr_pvr_CheckRecordPossible (pstRecSetup, pulActionId, pulFailReason);
}

HERROR MGR_PVR_GetCurrentRecSvcHandles (HUINT32 *pulRecNum, Handle_t *phSvcArray)
{
	return pmgr_pvr_GetCurrentRecSvcHandles (pulRecNum, phSvcArray);
}

MgrRec_EndReasonType_e MGR_PVR_StartRecording (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId)
{
	MgrRec_EndReasonType_e	eFailReason;
	if (pstRecSetup == NULL)			{ return eMgrRecFailed_UNKNOWN; }

	switch (pstRecSetup->eRecDst)
	{
	case eMgrRecOutput_EXT_TSLINE:	// N/A
	case eMgrRecOutput_MEM_RINGBUF: // N/A
	case eMgrRecOutput_HDD:
	default:
		eFailReason = pmgr_pvr_StartRecording (pstRecSetup, pulActionId);
		break;
	}

	return eFailReason;
}

HERROR MGR_PVR_GetTsrAutoTracking (HUINT32 ulViewId, HBOOL *pbAutoTrack)
{
	return pmgr_pvr_GetTsrAutoTracking(ulViewId, pbAutoTrack);
}

HERROR MGR_PVR_SetTsrAutoTracking (HUINT32 ulViewId, HBOOL bAutoTrack)
{
	return pmgr_pvr_SetTsrAutoTracking(ulViewId, bAutoTrack);
}

HERROR MGR_PVR_GetTsrRecActionId (Handle_t hSvcHandle, HUINT32 *pulActionId)
{
	return pmgr_pvr_GetTsrRecActionId (hSvcHandle, pulActionId);
}

HERROR MGR_PVR_StartTsrRecording (HUINT32 ulRelatedViewId, Handle_t hMasterSvc, Handle_t hSuppleSvc, HINT32 eSessionType, MgrPvr_TsrType_e eTsrType, HUINT32 ulStartTime)
{
	return pmgr_pvr_StartTsrRecording(ulRelatedViewId, hMasterSvc, hSuppleSvc, eSessionType, eTsrType, ulStartTime);
}

HERROR MGR_PVR_StopTsrRecording (Handle_t hMasterSvc, Handle_t hSuppleSvc, MgrPvr_TsrType_e eTsrType)
{
	return pmgr_pvr_StopTsrRecording(hMasterSvc, hSuppleSvc, eTsrType);
}

HERROR MGR_PVR_StartLiveStreaming (Handle_t hSvcHandle, HUINT32 *pulActionId, Handle_t *phLiveStream)
{
	return pmgr_pvr_StartLiveStreaming (hSvcHandle, pulActionId, phLiveStream);
}

HERROR MGR_PVR_StopLiveStreaming (Handle_t hLiveStream)
{
	return pmgr_pvr_StopLiveStreaming (hLiveStream);
}

#define ___PVR_TRANSCODER___
HERROR	MGR_PVR_GetProfileFromJSON(const HCHAR *pProfileString, MgrTrcd_StreamProfile *pstProfile)
{
	return pmgr_pvr_GetProfileFromJSON (pProfileString, pstProfile);
}

HERROR MGR_PVR_StartPlaybackTranscodeToRingBuf (MgrPvr_PbTrcdToMemSetup_t *pstSetup, HUINT32 *pulActionId)
{
	return pmgr_pvr_StartPlaybackTranscodeToRingBuf (pstSetup, pulActionId);
}

HERROR MGR_PVR_StopPlaybackTranscodeToRingBuf (HUINT32 ulActionId)
{
	return pmgr_pvr_StopPlaybackTranscodeToRingBuf (ulActionId);
}

HERROR MGR_PVR_StartTrRecording (HUINT32 ulContentId, HUINT32 ulRecMode, HUINT32 ulPvrIdx, HUINT32 *pulActionId)
{
	return pmgr_pvr_StartTrRecording (ulContentId, ulRecMode, ulPvrIdx, pulActionId);
}
