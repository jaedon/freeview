/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_playback.h>
#include <pmgr_playback.h>
#include <pmgr_model.h>

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
/**************** Static Function Prototypes  **********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HERROR MGR_PLAYBACK_Init(void)
{
	return pmgr_playback_Init ();
}

HERROR MGR_PLAYBACK_GetPbSiSpec (SvcSi_PbSpec_t *pstSiSpec)
{
	return MGR_MODEL_PlaybackGetSiSpec (pstSiSpec);
}

HERROR MGR_PLAYBACK_GetPbCasTypes (HUINT32 *pulCasTypes)
{
	return MGR_MODEL_PlaybackGetCasTypes (pulCasTypes);
}

HERROR MGR_PLAYBACK_GetMediaPbSiSpec (SvcSi_MediaPbSpec_t *pstSiSpec)
{
	return MGR_MODEL_MediaplaybackGetSiSpec (pstSiSpec);
}

HERROR MGR_PLAYBACK_SetPlaySetup (HUINT32 ulActionId, MgrPb_PlaySetup_t *pstPbSetup)
{
 	return pmgr_playback_SetPlaySetup (ulActionId, pstPbSetup);
}

BUS_Callback_t	MGR_PLAYBACK_GetProcFunc(HUINT32 ulActionId)
{
 	return pmgr_playback_GetProcFunc (ulActionId);
}

HERROR MGR_PLAYBACK_SetPlaySpeed (HUINT32 ulActionId, HINT32 nPlaySpeed)
{
 	return pmgr_playback_SetPlaySpeed (ulActionId, nPlaySpeed);
}

HERROR MGR_PLAYBACK_SetJumpPlay(HUINT32 ulActionId, HUINT32 ulTimeToPlay)
{
 	return pmgr_playback_SetJumpPlay(ulActionId, ulTimeToPlay);
}

HERROR MGR_PLAYBACK_GetPlaybackInfo(HUINT32 ulActionId, MgrPb_Info_t *pstInfo)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_GetPlaybackInfo(ulActionId, pstInfo);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_PLAYBACK_GetPlaybackTimeInfo(HUINT32 ulActionId, MgrPb_Info_t *pstInfo)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_GetPlaybackTimeInfo(ulActionId, pstInfo);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_PLAYBACK_StopPlayback (HUINT32 ulActionId)
{
	return pmgr_playback_StopPlayback (ulActionId);
}

HERROR MGR_PLAYBACK_SetAudioInfo (HUINT32 ulActionId, HUINT32 ulIndex, DxStereoSelect_e eSoundMode)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_SetAudioInfo (ulActionId, ulIndex, eSoundMode);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_PLAYBACK_SetSubTitleDisplay (HUINT32 ulActionId, HBOOL bDisplay)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_SetSubTitleDisplay (ulActionId, bDisplay);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_PLAYBACK_SetSubtitleLanguage (HUINT32 ulActionId, HUINT32 ulIndex)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_SetSubtitleLanguage (ulActionId, ulIndex);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_PLAYBACK_GetSubtitleDisplay (HUINT32 ulActionId, HBOOL *pbDisplay)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_GetSubtitleDisplay (ulActionId, pbDisplay);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_PLAYBACK_GetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_GetServiceInfo (ulActionId, pstService);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_PLAYBACK_SetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService, HBOOL bUpdate)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_playback_SetServiceInfo (ulActionId, pstService, bUpdate);
#else
	return ERR_FAIL;
#endif
}

HUINT32	MGR_PLAYBACK_GetPlaybackParam (MgrPb_ParamType_e eType)
{
	return pmgr_playback_GetPlaybackParam (eType);
}

HBOOL MGR_PLAYBACK_IsPlaybackId(HUINT32 ulContentId, MgrPb_Info_t *pstInfo)
{
	return pmgr_playback_IsPlaybackId (ulContentId, pstInfo);
}

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)

STATIC HBOOL mgr_playback_CompareSvc(const void *pvInfo1, const void *pvInfo2, HUINT32 ulSize)
{
	if (NULL == pvInfo1 || NULL == pvInfo2)
		return FALSE;

	return (HxSTD_MemCmp (pvInfo1, pvInfo2, sizeof(SvcMeta_Service_t)) == 0) ? FALSE : TRUE;
}

STATIC HERROR mgr_playback_UpdateSvc(const SvcSi_PmtEsInfo_t *pstPmtEsInfo, void *pvSvcInfo)
{
	SvcMeta_Service_t	*pstSvcInfo = (SvcMeta_Service_t *)pvSvcInfo;

	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	// PCR
	pstSvcInfo->usPcrPid = pstPmtEsInfo->usPcrPid;

	// CAS type - PID는 아니지만 이것도 넣어줘야 cas type이 변경됐을 경우 처리할 수 있다
	pstSvcInfo->eCasType = (pstPmtEsInfo->ulCasType > eDxCAS_TYPE_FTA) ? (DxCasType_e)pstPmtEsInfo->ulCasType : eDxCAS_TYPE_FTA;

	return ERR_OK;
}

STATIC HERROR mgr_playback_UpdateVideoEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	SvcMeta_Service_t	*pstSvcInfo = NULL;

	pstSvcInfo = (SvcMeta_Service_t *)pvSvcInfo;

	pstSvcInfo->usVideoPid = PID_NULL;
	pstSvcInfo->eVideoCodec = eDxVIDEO_CODEC_UNKNOWN;

	if (TRUE == pstSelected->bFound)
	{
		HERROR			hErr;
		HUINT16			usPid = PID_NULL;
		DxVideoCodec_e	eCodec = eDxVIDEO_CODEC_UNKNOWN;

		hErr = SVC_SI_GetEsInfo(eES_UPDATE_Video, hEsInfoList, pstSelected->nIndexMain, &usPid, (HUINT32 *)&eCodec, NULL, NULL);
		if (ERR_OK == hErr)
		{
			pstSvcInfo->usVideoPid = usPid;
			pstSvcInfo->eVideoCodec = eCodec;
		}

		HxLOG_Debug("Index = %d, Pid = 0x%x \n", pstSelected->nIndexMain, pstSvcInfo->usVideoPid );
	}

	return ERR_OK;
}

STATIC void  mgr_playback_ResetAudioInfoFromSvc(SvcMeta_Service_t *pstSvcInfo)
{
	pstSvcInfo->bAudUserSetFlag = FALSE;

	pstSvcInfo->bDolbyFlag = FALSE;
	// TODO: Set Audio Description Flag
	// pstSvcInfo->b, = FALSE;
	// TODOL Set Audio Langauge
	// pstSvcInfo->sz, "";

	pstSvcInfo->usAudioPid		= PID_NULL;
	pstSvcInfo->usAudioAuxPid	= PID_NULL;
	pstSvcInfo->usDolbyPid		= PID_NULL;
	pstSvcInfo->eAudioCodec		= eDxAUDIO_CODEC_UNKNOWN;
	pstSvcInfo->eAudioAuxCodec	= eDxAUDIO_CODEC_UNKNOWN;
	pstSvcInfo->eDolbyCodec		= eDxAUDIO_CODEC_UNKNOWN;
}

STATIC HERROR mgr_playback_UpdateAudioEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	SvcMeta_Service_t	*pstSvcInfo = NULL;
	const HCHAR 	*pucMainLang = NULL;

	pstSvcInfo = (SvcMeta_Service_t *)pvSvcInfo;

	mgr_playback_ResetAudioInfoFromSvc(pstSvcInfo);

	if (TRUE == pstSelected->bFound)
	{
		HERROR			hErr;
		HUINT16 			usPid = PID_NULL;
		DxAudioCodec_e	eCodec = eDxAUDIO_CODEC_UNKNOWN;

		// Main
		if (pstSelected->nIndexMain >= 0)
		{
			usPid = PID_NULL;
			eCodec = eDxAUDIO_CODEC_UNKNOWN;

			hErr = SVC_SI_GetEsInfo(eES_UPDATE_Audio, hEsInfoList, pstSelected->nIndexMain, &usPid, (HUINT32 *)&eCodec, NULL, NULL);
			if(hErr == ERR_OK)
			{
				pstSvcInfo->usAudioPid		= usPid;
				pstSvcInfo->eAudioCodec 	= eCodec;
			}

			pucMainLang = HLIB_LANG_IdTo639(pstSelected->eLangIdMain);
			if(pucMainLang != NULL)
				HLIB_STD_StrNCpySafe(pstSvcInfo->audioLanguage, pucMainLang, DxLANG_CODE_LEN);

			pstSvcInfo->eDualMono		= (DxDualMonoSelect_e)pstSelected->ulParam1;
			pstSvcInfo->bDolbyFlag		= SvcSi_ES_AUDIO_IS_DOLBY_TYPE((DxAudioCodec_e)pstSelected->ulCodec);
		}

		// AD
		if (pstSelected->nIndexAux >= 0)
		{
			usPid = PID_NULL;
			eCodec = eDxAUDIO_CODEC_UNKNOWN;

			hErr = SVC_SI_GetEsInfo(eES_UPDATE_Audio, hEsInfoList, pstSelected->nIndexAux, &usPid, (HUINT32 *)&eCodec, NULL, NULL);
			if (ERR_OK == hErr)
			{
				pstSvcInfo->usAudioAuxPid	= usPid;
				pstSvcInfo->eAudioAuxCodec	= eCodec;

				pstSvcInfo->audioDescription = TRUE;
			}
		}

		HxLOG_Debug("Main(Index = %d, Pid = 0x%x) Aux(Index = %d, Pid = 0x%x) \n\n",
			pstSelected->nIndexMain, pstSvcInfo->usAudioPid, pstSelected->nIndexAux,  pstSvcInfo->usAudioAuxPid);
	}

	return ERR_OK;
}

STATIC HERROR mgr_playback_UpdateSubtitleEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	SvcMeta_Service_t	*pstSvcInfo = NULL;

	pstSvcInfo = (SvcMeta_Service_t *)pvSvcInfo;

	if (TRUE == pstSelected->bFound)
	{
		if (pstSelected->nIndexMain >= 0)
		{
			pstSvcInfo->ucSubttlIdx = (HUINT8)pstSelected->nIndexMain;
		}

		HxLOG_Debug("Index = %d \n", pstSvcInfo->ucSubttlIdx );
	}

	return ERR_OK;
}

STATIC HERROR mgr_playback_UpdateTeletextEs(Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	SvcMeta_Service_t	*pstSvcInfo = NULL;

	pstSvcInfo = (SvcMeta_Service_t *)pvSvcInfo;

	pstSvcInfo->usTtxPid = PID_NULL;

	if (TRUE == pstSelected->bFound)
	{
		if (pstSelected->nIndexMain >= 0)
		{
			HERROR			hErr;
			HUINT16 		usPid = PID_NULL;

			hErr = SVC_SI_GetEsInfo(eES_UPDATE_Teletext, hEsInfoList, pstSelected->nIndexMain, &usPid, NULL, NULL, NULL);
			if (ERR_OK == hErr)
			{
				pstSvcInfo->usTtxPid = usPid;
			}

			HxLOG_Debug("Index = %d, Pid = 0x%x \n", pstSelected->nIndexMain, pstSvcInfo->usTtxPid );
		}
	}

	return ERR_OK;
}

STATIC HERROR mgr_playback_UpdateEs(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	HERROR	hErr = ERR_FAIL;

	if (HANDLE_NULL == hEsInfoList)
		return ERR_FAIL;

	if (NULL == pvSvcInfo)
		return ERR_FAIL;

	if (NULL == pstSelected)
		return ERR_FAIL;

	switch (eType)
	{
	case eES_UPDATE_Video:			hErr = mgr_playback_UpdateVideoEs(hEsInfoList, pvSvcInfo, pstSelected);			break;
	case eES_UPDATE_Audio:			hErr = mgr_playback_UpdateAudioEs(hEsInfoList, pvSvcInfo, pstSelected);			break;
	case eES_UPDATE_Subtitle:		hErr = mgr_playback_UpdateSubtitleEs(hEsInfoList, pvSvcInfo, pstSelected);		break;
	case eES_UPDATE_Teletext:		hErr = mgr_playback_UpdateTeletextEs(hEsInfoList, pvSvcInfo, pstSelected);		break;
	default:
		break;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("mgr_playback_UpdateEs failed. eType(%d) \n",eType);
	}

	return ERR_FAIL;
}

HERROR MGR_PLAYBACK_UpdateEs(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected)
{
	if (HANDLE_NULL == hEsInfoList)
		return ERR_FAIL;

	if (NULL == pvSvcInfo)
		return ERR_FAIL;

	if (NULL == pstSelected)
		return ERR_FAIL;

	return mgr_playback_UpdateEs(eType, hEsInfoList, pvSvcInfo, pstSelected);
}

HBOOL MGR_PLAYBACK_CompareSvc(const void *pvInfo1, const void *pvInfo2, HUINT32 ulSize)
{
	if (NULL == pvInfo1 || NULL == pvInfo2)
		return FALSE;

	if (0 == ulSize)
		return FALSE;

	return mgr_playback_CompareSvc(pvInfo1, pvInfo2, ulSize);
}

HERROR MGR_PLAYBACK_UpdateSvc(const SvcSi_PmtEsInfo_t *pstPmtEsInfo, void *pvSvcInfo)
{
	if (NULL == pvSvcInfo)
		return ERR_FAIL;

	return mgr_playback_UpdateSvc(pstPmtEsInfo, pvSvcInfo);
}


STATIC HERROR mgr_playback_GetSiUserSettingsFromMetaInfo(const SvcMeta_Service_t *pstSvcInfo, SvcSi_UserSettings_t *pstUserSettings)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	if (NULL == pstUserSettings)
		return ERR_FAIL;

	HxSTD_MemSet(pstUserSettings, 0x00, sizeof(SvcSi_UserSettings_t));

#if defined(SVC_AUDIO_LANGUAGE_CODE) && defined(SVC_SUBTITLE_LANGUAGE_CODE)
	// Audio Settings : User Value
	pstUserSettings->stAudio.bUserSelected			= pstSvcInfo->bAudUserSetFlag;
	pstUserSettings->stAudio.bDolby					= pstSvcInfo->bDolbyFlag;
	if (pstSvcInfo->bAudUserSetFlag == TRUE)
	{
		pstUserSettings->stAudio.nPid = (HINT32)pstSvcInfo->usAudioPid;
	}
	// TODO: audio description

	// Subtitle Settings
	pstUserSettings->stSubtitle.bUserSelected		= pstSvcInfo->bSubttlUserFlag;
	pstUserSettings->stSubtitle.nTrack				= pstSvcInfo->ucSubttlIdx;
	pstUserSettings->stSubtitle.bHoh				= pstSvcInfo->bHardOfHearing;
	pstUserSettings->stSubtitle.bDvb				= FALSE;
	pstUserSettings->stSubtitle.bHd					= FALSE;

	if (pstUserSettings->stSubtitle.bUserSelected == TRUE && pstUserSettings->stSubtitle.nTrack == SUBTITLE_OFF_IDX)
	{
		pstUserSettings->stSubtitle.bUserSelectedOff = TRUE;
	}
#endif

	return ERR_OK;
}



STATIC HERROR mgr_playback_GetSiUpdate(SvcSi_UpdateEs_t *pstUpdate, SvcMeta_Service_t *pstOrgInfo, SvcMeta_Service_t *pstDestInfo)
{
	HxSTD_MemSet(pstUpdate, 0x00, sizeof(SvcSi_UpdateEs_t));

	pstUpdate->pvOrgInfo	= pstOrgInfo;
	pstUpdate->pvDstInfo	= pstDestInfo;
	pstUpdate->ulSize		= sizeof(SvcMeta_Service_t);

	(void)mgr_playback_GetSiUserSettingsFromMetaInfo((const SvcMeta_Service_t *)pstOrgInfo, &pstUpdate->stUserSettings);

	pstUpdate->fnCompareSvc	= MGR_PLAYBACK_CompareSvc;
	pstUpdate->fnUpdateSvc	= MGR_PLAYBACK_UpdateSvc;
	pstUpdate->fnUpdateEs	= MGR_PLAYBACK_UpdateEs;

	return ERR_OK;
}

HERROR MGR_PLAYBACK_GetSiUpdate(SvcSi_UpdateEs_t *pstUpdate, SvcMeta_Service_t *pstOrgInfo, SvcMeta_Service_t *pstDestInfo)
{
	if (NULL == pstUpdate)
		return ERR_FAIL;

	return mgr_playback_GetSiUpdate(pstUpdate, pstOrgInfo, pstDestInfo);
}
#endif

HERROR MGR_PLAYBACK_UpdateServiceInfo (Handle_t hAction, HUINT32 ulSvcId, SvcMeta_Service_t *pstSrcSvcInfo, SvcMeta_Service_t *pstDstSvcInfo, HBOOL *pbChanged)
{
	HERROR	hErr;

	if (NULL == pstSrcSvcInfo)
		return ERR_FAIL;

	if (NULL == pstDstSvcInfo)
		return ERR_FAIL;

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)
	{
		SvcSi_UpdateEs_t stUpdate;

		hErr = MGR_PLAYBACK_GetSiUpdate(&stUpdate, pstSrcSvcInfo, pstDstSvcInfo);
		if (ERR_OK == hErr)
		{
			hErr = SVC_SI_UpdateServiceInfo (hAction, ulSvcId, &stUpdate, pbChanged);
		}
	}
#else
	hErr = FALSE;
#endif

	return hErr;
}

HERROR MGR_PLAYBACK_GetMenuSubtitleLangIndex(DbParam_Item_e eSubtitleItem, SvcSi_SubtitleList_t *pstSubtitleList, HINT32 *pnIndex)
{
	return pmgr_playback_GetMenuSubtitleLangIndex(eSubtitleItem, pstSubtitleList, pnIndex);
}

