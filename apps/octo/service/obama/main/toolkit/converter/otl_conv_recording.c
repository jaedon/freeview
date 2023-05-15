
#include	<otl.h>
#include	<octo_common.h>
#include	<db_param.h>



DxRecListData_t *	OTL_CONV_MW_PvrRecord2DxRecListData(DxRecListData_t *d, const SvcMeta_Record_t *rec)
{
	HLIB_STD_StrNCpySafe(d->stMeta.szPath, rec->aucPath, DxRECLIST_PATH);
	HLIB_STD_StrNCpySafe(d->stMeta.szFileName, rec->aucFileName, DxRECLIST_PATH);

	d->ulRecStartTime	= rec->ulStartTime;
	d->stMeta.ulEndTime	= rec->ulEndTime;
	d->ulRecDuration	= rec->ulRecTime;
#if defined(CONFIG_OP_FREESAT)
	d->ulRecOrgDuration = rec->ulOrgDuration;
	if (d->ulRecOrgDuration <= 0)
	{	// set by start time & end time
		d->ulRecOrgDuration = rec->ulEndTime - rec->ulStartTime;
	}
#else
	d->ulRecOrgDuration = rec->ulEndTime - rec->ulStartTime;
#endif

	d->eRecStatus		= rec->ucRecordStatus;
	d->bEventBased		= rec->u2cEventBase;
	d->nEventId		= rec->nEventId;
	d->bBlocked 	= rec->u1cLocked;
	d->bPlayed			= rec->u1cWatched;
	d->bScrambled		= rec->u1cScrambled;
	d->bEncrypted		= rec->u1cEncryption;
	d->bAudoDeleted = rec->u1cAutoDeleted;
	d->eRecEndReason	= rec->ucEndCondition;
	d->ulLastViewTime	= rec->ulLastWatchtime;
	d->bDoNotDelete		= rec->bDoNotDelete;
	d->ulDoNotDeleteSetTime		= rec->ulDoNotDeleteSetTime;
	d->bUserLock		= rec->bUserLocked;
	d->bWillBeDeleted	= rec->bWillBeDeleted;
	d->usNumOfBookmark	= rec->usNumOfBookmark;
	d->usNumOfPausedPoints	= rec->usNumOfPausedPoints;
#if defined (CONFIG_OP_SES) && defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	d->ucas.ucSCPinCodeFlag 	= rec->ucSCPinCodeFlag;
#endif
	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(d->astBookmarks, rec->astBookmarks
		, sizeof(DxRecList_Bookmark_t) * d->usNumOfBookmark);
	HxSTD_MemCopy(d->astPausedPoints, rec->astPausedPoints
		, sizeof(DxRecList_Bookmark_t) * d->usNumOfPausedPoints);

	HLIB_STD_StrNCpySafe(d->stMeta.aucDisplayEvtName, rec->aucDisplayEvtName, DxRECLIST_NAME_LEN);

	if (!HxSTD_StrEmpty(d->stMeta.aucDisplayEvtName))
		HLIB_STD_StrNCpySafe(d->szName, d->stMeta.aucDisplayEvtName, DxRECLIST_NAME_LEN);
	else
		HLIB_STD_StrNCpySafe(d->szName, d->stEvt.aucEventName, DxRECLIST_NAME_LEN);

#if 0//defined(CONFIG_PROD_HMS1000S) // 기존 hms ph1 의 잔재로 남아있는 코드.
	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->stMeta.stDrm, &rec->stDrmInfo, sizeof(DxDrmInfo_t));
#else
	HxSTD_MemCopy(&d->stMeta.stCcInfo, &rec->stDrmInfo, sizeof(DxCopyrightControl_t));
#endif

	d->bSeriesRec		= rec->bSeriesRec;
	HLIB_STD_StrNCpySafe(d->stEvt.aucSeriesId, rec->szSeriesId, DxPVR_TVANYTIME_CRID_TOTAL_LEN);
	d->stEvt.ulEpisodeNum = rec->ulEpisodeNum;
	d->stEvt.ulSeason = rec->ulSeason;
	// Model Specific Data
#if defined(CONFIG_OP_FREESAT)
	d->extension.freesat.ulFtaCmVal 		= rec->ulFtaCmVal ;
	d->extension.freesat.ucCopyTime 	= rec->ucCopyTime;
	d->extension.freesat.ulTimeOffsetAtRec = rec->ulTimeOffsetAtRec;
	d->extension.freesat.stEvt.astGuidance.eType = rec->guidanceType;
	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->extension.freesat.stEvt.astGuidance.stData, &rec->stGuidanceData, DxFREESAT_PVR_GUIDANCE_DATA_MAX);
#endif

#if defined(CONFIG_OP_UK_DTT)
	d->extension.ukdtt.ulFtaCmVal 		= rec->ulFtaCmVal ;
	d->extension.ukdtt.ucCopyTime 	= rec->ucCopyTime;
	d->extension.ukdtt.ulTimeOffsetAtRec = rec->ulTimeOffsetAtRec;
#endif

#if defined(CONFIG_MW_CI)
	d->ucas.cas.ulInstType = rec->ulInstType;
	d->ucas.cas.ulSlotId = rec->usSlotId;
  #if defined(CONFIG_MW_CI_PLUS_V_1_3)
	HxSTD_MemCopy(d->ucas.cas.ciplus.aucBindedCamId, rec->aucBindedCamId, SVC_META_CAMID_LEN);
	d->ucas.cas.ciplus.bCamNeedContent = rec->bCamNeedContent;
  #endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // defined(CONFIG_APS_HD)
	d->ucas.ucSCPinCodeFlag = rec->ucSCPinCodeFlag;
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	HLIB_STD_StrNCpySafe(d->ucas.conax.szConaxMetaName, rec->szConaxMetaName, DxRECLIST_PATH);
#endif

	d->bTranscoded	= rec->bTranscoded;
	d->ulRecMode	= rec->ulRecMode;
	d->stMeta.usTrRecState = rec->usTrRecState;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HxSTD_MemCopy(d->stMeta.astTvaCrids,rec->stCrid, sizeof(DxPVR_TvaCrid_t)*DxMAX_CRID_DATA_NUM);

#if defined(CONFIG_OP_TDC)
	HxSTD_MemCopy(d->stMeta.szSeriesTitle,rec->aucSeriesTitle, DxMAX_SERIES_TITLE_LEN);
#endif

#endif

	return d;
}


DxRecListData_t *	OTL_CONV_MW_PvrService2DxRecListData(DxRecListData_t *d, const SvcMeta_Service_t *svc)
{
	d->nSvcNo = svc->nChNumber;
	HLIB_STD_StrNCpySafe(d->szSvcName, svc->aucServiceName, DxPVR_MAX_SERVICE_NAME_LEN);
	d->stMeta.usSvcId	= svc->usSvcId;
	d->stMeta.usTsId	= svc->usTsId;
	d->stMeta.usOnId	= svc->usOnId;

	d->stMeta.usPmtPid	= svc->usPmtPid;
	d->stMeta.usPcrPid = svc->usPcrPid;
	d->stMeta.usTtxPid	= svc->usTtxPid;
	d->stMeta.usCaptionPid	= svc->usCaptionPid;

	d->stMeta.usVideoPid	= svc->usVideoPid;
	d->stMeta.eVideoCodec	= svc->eVideoCodec;

	d->stMeta.usAudioPid	= svc->usAudioPid;
	d->stMeta.eAudioCodec	= svc->eAudioCodec;

	d->stMeta.usAudioAuxPid = svc->usAudioAuxPid;
	d->stMeta.eAudioAuxCodec	= svc->eAudioAuxCodec;

	d->stMeta.usDolbyPid	= svc->usDolbyPid;
	d->stMeta.eDolbyCodec	= svc->eDolbyCodec;
	d->stMeta.bDolbyFlag	= svc->bDolbyFlag;

	d->eSvcType 		= svc->eSvcType;
	d->eVideoType			= svc->eVideoType;
	d->eCasType 		= svc->eCasType;

	d->stMeta.ucSubttlIdx		= svc->ucSubttlIdx;
	d->stMeta.bSubttlUserFlag	= svc->bSubttlUserFlag;
	d->stMeta.eSoundMode		= svc->eSoundMode;
	d->stMeta.eDualMono 	= svc->eDualMono;
	d->stMeta.bAudUserSetFlag	= svc->bAudUserSetFlag;

	// TODO: 왜 이딴식으로 구현해 놓은 거야!!!
	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->stAudioInfoList, &svc->stAudioInfoList, sizeof(DxMETA_PvrAudioInfoList_t));
	HxSTD_MemCopy(&d->stSbtlInfoList, &svc->stSbtlInfoList, sizeof(DxMETA_PvrSbtlInfoList_t));


#if defined(CONFIG_OP_FREESAT)
	d->extension.freesat.nMasterChNumber	= svc->nMasterChNumber ;
	HLIB_STD_StrNCpySafe(d->extension.freesat.aucMasterServiceName, svc->aucMasterServiceName, DxPVR_MAX_SERVICE_NAME_LEN);
	d->extension.freesat.usMasterSvcId		= svc->usMasterSvcId;
	d->extension.freesat.usMasterTsId		= svc->usMasterTsId;
	d->extension.freesat.usMasterOnId		= svc->usMasterOnId;
#endif

	return d;
}


DxRecListData_t *	OTL_CONV_MW_PvrEvent2DxRecListData(DxRecListData_t *d, const SvcMeta_Event_t *evt)
{
	HUINT32 n, i, j;
	HCHAR				*szTmp;
	SvcMeta_ExtEvt_t 		*pstExtEvt;
	DxRecList_ExtEvt_t	*pstDxExtEvt;

	d->stEvt.ucRunStatus	=	evt->u3RunnigStatus;
	d->stEvt.ucFreeCaMode	=	evt->u1FreeCaMode;
	d->nEventId			=	evt->nEventId;
	d->stEvt.ucPRNum		=	evt->ucParentalRatingNum;
	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(d->stEvt.astParentalRating, evt->astParentalRating
					, d->stEvt.ucPRNum * sizeof(DxPvr_ParentalRating_t) );
	d->stEvt.ucContentNibble=	evt->ucContentNibble;
	d->stEvt.ucUserNibble	=	evt->ucUserNibble;
	HLIB_STD_StrNCpySafe(d->stEvt.aucEventName, evt->aucEventName, DxPVR_SHORT_EVENT_NAME_LEN);
	HLIB_STD_StrNCpySafe(d->stEvt.aucText, evt->aucEventText, DxPVR_EVENT_TEXT);

	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->stEvt.stComponent, evt->stComponent
			, sizeof(DxPvr_EvtComponent_t) * DxEPG_COMPONENT_DESC_MAX);

#if defined(CONFIG_OP_FREESAT)
	// TODO: 변경.... OTL ㅠㅠ
	if (evt->bUseFtaCntMgmt)
		HxSTD_MemCopy(&d->extension.freesat.stEvt.stFtaCntMgmt, &evt->ftaCntMgmt, sizeof(DxFreesat_SvcMeta_FtaCntMgmt_t));
	if (evt->bUseGuidance)
		HxSTD_MemCopy(&d->extension.freesat.stEvt.astGuidance, &evt->guidance, sizeof(DxFreesat_Pvr_Guidance_t));
#endif
#if defined(CONFIG_OP_UK_DTT)
	if (evt->bUseFtaCntMgmt)
		HxSTD_MemCopy(&d->extension.ukdtt.stEvt.stFtaCntMgmt, &evt->stFtaCntMgmt, sizeof(DxUkdtt_Pvr_FtaCntMgmt_t));
	if (evt->bUseGuidance)
		HxSTD_MemCopy(&d->extension.ukdtt.stEvt.stGuidance, &evt->stGuidance, sizeof(DxUkdtt_Pvr_Guidance_t));
#endif
	/* Extended Event Descriptor의 갯수를 파악 */
	pstExtEvt = evt->pstExtEvt;
	d->ulExtEvtCnt = 0;
	while (pstExtEvt)
	{
		d->ulExtEvtCnt++;
		pstExtEvt = pstExtEvt->pNext;
	}

	n = d->ulExtEvtCnt;
	if (0 == n)
	{
		d->pstExtEvts = NULL;
		return d;
	}

	pstExtEvt = evt->pstExtEvt;
	d->pstExtEvts	= (DxRecList_ExtEvt_t*)HLIB_STD_MemCalloc( sizeof(DxRecList_ExtEvt_t) * n);
	if(d->pstExtEvts == NULL)
	{
		return NULL;
	}

	for (i = 0; i < n; i++)
	{
		pstDxExtEvt = &d->pstExtEvts[i];
		pstDxExtEvt->ucDesNum		= pstExtEvt->ucDesNum;
		pstDxExtEvt->ucLastDesNum	= pstExtEvt->ucLastDesNum ;
		HxSTD_MemCopy(pstDxExtEvt->szIso639LangCode, pstExtEvt->szIso639LangCode, 4);
		pstDxExtEvt->ucItemNum		= pstExtEvt->ucItemNum;

		for (j = 0; j < pstDxExtEvt->ucItemNum; j++)
		{
			if (pstExtEvt->astItem[j].pszDescription)
				HLIB_STD_StrNCpySafe(pstDxExtEvt->astItem[j].aucDescription, pstExtEvt->astItem[j].pszDescription, DxPVR_EVENT_TEXT);
			if (pstExtEvt->astItem[j].pszItem)
				HLIB_STD_StrNCpySafe(pstDxExtEvt->astItem[j].aucItem, pstExtEvt->astItem[j].pszItem, DxPVR_EVENT_TEXT);
		}

		pstExtEvt = pstExtEvt->pNext;
	}

	pstExtEvt = evt->pstExtEvt;
	szTmp     = d->aucExtText;
	n         = DxRECLIST_EXT_TEXT_BUFSIZE;
	while (pstExtEvt)
	{
		if (pstExtEvt->pszChar)
		{
			HUINT32	len = strlen(pstExtEvt->pszChar);
			if (n < len)
				break;
			memcpy(szTmp, pstExtEvt->pszChar, len);
			szTmp += len;
			n     -= len;
		}
		pstExtEvt = pstExtEvt->pNext;
	}
	*szTmp = '\0';
	return d;
}

HERROR OTL_CONV_MW_PvrMeta2DxRecListData(DxRecListData_t *d, const SvcMeta_Record_t *rec, const SvcMeta_Service_t *svc, const SvcMeta_Event_t *evt)
{
	memset(d, 0, sizeof(DxRecListData_t));

	if (rec && OTL_CONV_MW_PvrRecord2DxRecListData(d, rec) == NULL)
		return ERR_FAIL;

	if (svc && OTL_CONV_MW_PvrService2DxRecListData(d, svc) == NULL)
		return ERR_FAIL;

	if (evt && OTL_CONV_MW_PvrEvent2DxRecListData(d, evt) == NULL)
		return ERR_FAIL;

	return ERR_OK;
}

SvcMeta_Record_t *	OTL_CONV_DxRecListData2MW_PvrRecord(SvcMeta_Record_t *d, const DxRecListData_t *s)
{

	HLIB_STD_StrNCpySafe(d->aucPath, s->stMeta.szPath, FS_MAX_FILE_NAME_LEN);
	HLIB_STD_StrNCpySafe(d->aucFileName, s->stMeta.szFileName, FS_MAX_FILE_NAME_LEN);
	d->ulStartTime = s->ulRecStartTime;
	d->ulEndTime = s->stMeta.ulEndTime;
	d->ulRecTime = s->ulRecDuration;
#if defined(CONFIG_OP_FREESAT)
	d->ulOrgDuration = s->ulRecOrgDuration;
#endif

	d->ucRecordStatus = s->eRecStatus;
	d->u2cEventBase 	= s->bEventBased;
	d->nEventId		= s->nEventId;
	d->u1cLocked		= s->bBlocked;
	d->u1cWatched	= s->bPlayed;
	d->u1cScrambled 	= s->bScrambled;
	d->u1cEncryption	= s->bEncrypted;
	d->u1cAutoDeleted = s->bAudoDeleted;
	d->ucEndCondition = s->eRecEndReason;
	d->ulLastWatchtime= s->ulLastViewTime;
	d->bUserLocked	= s->bUserLock;
	d->bDoNotDelete		= s->bDoNotDelete;
	d->ulDoNotDeleteSetTime		= s->ulDoNotDeleteSetTime;
	d->bWillBeDeleted	= s->bWillBeDeleted;
	d->usNumOfBookmark= s->usNumOfBookmark;
	d->usNumOfPausedPoints= s->usNumOfPausedPoints;
#if defined (CONFIG_OP_SES) && defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	d->ucSCPinCodeFlag	= s->ucas.ucSCPinCodeFlag;
#endif
		// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(d->astBookmarks, s->astBookmarks
		, sizeof(SvcMeta_Bookmark_t) * s->usNumOfBookmark);
	HxSTD_MemCopy(d->astPausedPoints, s->astPausedPoints
		, sizeof(SvcMeta_Bookmark_t) * s->usNumOfPausedPoints);
	HLIB_STD_StrNCpySafe(d->aucDisplayEvtName, s->stMeta.aucDisplayEvtName, PVR_MAX_TITLE_LEN);

#if 0//defined(CONFIG_PROD_HMS1000S) // 기존 hms ph1 의 잔재로 남아있는 코드.
		// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->stDrmInfo, &s->stMeta.stDrm, sizeof(DxCopyrightControl_t));
#else
	HxSTD_MemCopy(&d->stDrmInfo, &s->stMeta.stCcInfo, sizeof(DxCopyrightControl_t));
#endif

	d->bSeriesRec = s->bSeriesRec;
	HLIB_STD_StrNCpySafe(d->szSeriesId, s->stEvt.aucSeriesId, EPG_TVA_CRID_TOTAL_LEN);
	d->ulEpisodeNum = s->stEvt.ulEpisodeNum;
	d->ulSeason = s->stEvt.ulSeason;

	// Model Specific Data
#if defined(CONFIG_OP_FREESAT)
	d->ulFtaCmVal = s->extension.freesat.ulFtaCmVal;
	d->ucCopyTime = s->extension.freesat.ucCopyTime;
	d->ulTimeOffsetAtRec = s->extension.freesat.ulTimeOffsetAtRec;
	d->guidanceType = s->extension.freesat.stEvt.astGuidance.eType;
		// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->stGuidanceData, &s->extension.freesat.stEvt.astGuidance.stData, DxMAX_GUIDANCE_DATA_LEN);
#endif

#if defined(CONFIG_OP_UK_DTT)
	d->ulFtaCmVal = s->extension.ukdtt.ulFtaCmVal;
	d->ucCopyTime = s->extension.ukdtt.ucCopyTime;
	d->ulTimeOffsetAtRec = s->extension.ukdtt.ulTimeOffsetAtRec;
#endif

#if defined(CONFIG_MW_CI)
	d->ulInstType = s->ucas.cas.ulInstType;
	d->usSlotId = s->ucas.cas.ulSlotId;
  #if defined(CONFIG_MW_CI_PLUS_V_1_3)
	d->bCamNeedContent = s->ucas.cas.ciplus.bCamNeedContent;
	HLIB_STD_StrNCpySafe(d->aucBindedCamId, s->ucas.cas.ciplus.aucBindedCamId, SVC_META_CAMID_LEN);
  #endif	// CONFIG_MW_CI_PLUS_V_1_3
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // defined(CONFIG_APS_HD)
	d->ucSCPinCodeFlag = s->ucas.ucSCPinCodeFlag;
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	HLIB_STD_StrNCpySafe(d->szConaxMetaName, s->ucas.conax.szConaxMetaName, FS_MAX_FILE_NAME_LEN);
#endif

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HxSTD_MemCopy(d->stCrid,s->stMeta.astTvaCrids, sizeof(DxPVR_TvaCrid_t)*DxMAX_CRID_DATA_NUM);

#if defined(CONFIG_OP_TDC)
	HxSTD_MemCopy(d->aucSeriesTitle,s->stMeta.szSeriesTitle, DxMAX_SERIES_TITLE_LEN);
#endif

#endif
	d->bTranscoded = s->bTranscoded;
	d->ulRecMode	 = s->ulRecMode;
	d->usTrRecState = s->stMeta.usTrRecState;

	return d;
}

SvcMeta_Service_t *	OTL_CONV_DxRecListData2MW_PvrService(SvcMeta_Service_t *d, const DxRecListData_t *s)
{

	d->nChNumber = s->nSvcNo;
	HLIB_STD_StrNCpySafe(d->aucServiceName, s->szSvcName, PVR_MAX_SERVICE_NAME_LEN);
	d->usSvcId = s->stMeta.usSvcId;
	d->usTsId	= s->stMeta.usTsId;
	d->usOnId	= s->stMeta.usOnId;

	d->usPmtPid 	= s->stMeta.usPmtPid;
	d->usPcrPid 	= s->stMeta.usPcrPid;
	d->usTtxPid 	= s->stMeta.usTtxPid;
	d->usCaptionPid= s->stMeta.usCaptionPid;

	d->usVideoPid	= s->stMeta.usVideoPid;
	d->eVideoCodec	= s->stMeta.eVideoCodec;

	d->usAudioPid	= s->stMeta.usAudioPid;
	d->eAudioCodec	= s->stMeta.eAudioCodec;

	d->usAudioAuxPid	= s->stMeta.usAudioAuxPid;
	d->eAudioAuxCodec	= s->stMeta.eAudioAuxCodec;

	d->usDolbyPid	= s->stMeta.usDolbyPid;
	d->eDolbyCodec	= s->stMeta.eDolbyCodec;
	d->bDolbyFlag	= s->stMeta.bDolbyFlag;

	d->eSvcType 	= s->eSvcType;
	d->eVideoType	= s->eVideoType;
	d->eCasType = s->eCasType;

	d->ucSubttlIdx	= s->stMeta.ucSubttlIdx;
	d->bSubttlUserFlag	= s->stMeta.bSubttlUserFlag;
	d->eSoundMode		= s->stMeta.eSoundMode;
	d->eDualMono		= s->stMeta.eDualMono;
	d->bAudUserSetFlag	= s->stMeta.bAudUserSetFlag;

	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->stAudioInfoList, &s->stAudioInfoList, sizeof(SvcMeta_AudioInfoList_t));
	HxSTD_MemCopy(&d->stSbtlInfoList,	&s->stSbtlInfoList, sizeof(SvcMeta_SbtlInfoList_t));


#if defined(CONFIG_OP_FREESAT)
	d->nMasterChNumber = s->extension.freesat.nMasterChNumber;
	HLIB_STD_StrNCpySafe(d->aucMasterServiceName, s->extension.freesat.aucMasterServiceName, PVR_MAX_SERVICE_NAME_LEN);
	d->usMasterSvcId = s->extension.freesat.usMasterSvcId;
	d->usMasterTsId = s->extension.freesat.usMasterTsId;
	d->usMasterOnId = s->extension.freesat.usMasterOnId;
#endif

	return d;
}

SvcMeta_Event_t *		OTL_CONV_DxRecListData2MW_PvrEvent(SvcMeta_Event_t *d, const DxRecListData_t *s)
{
	HUINT32 ulLength = 0;
	HUINT32 ulExtEvtNum = 0, i, j;

	SvcMeta_ExtEvt_t *pstExtEvt = NULL, *pstPvrExtHead = NULL;
	DxRecList_ExtEvt_t	*pstDxExtEvt = NULL;


	d->ulStartTime 		= s->ulRecStartTime;
	d->ulDuration 		= s->ulRecDuration;
	d->u3RunnigStatus 	= s->stEvt.ucRunStatus;
	d->u1FreeCaMode 		= s->stEvt.ucFreeCaMode;
	d->nEventId 			= s->nEventId;
	d->ucParentalRatingNum	= s->stEvt.ucPRNum;
	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy( d->astParentalRating, s->stEvt.astParentalRating,
				d->ucParentalRatingNum* sizeof(SvcMeta_Rating_t) );
	d->ucContentNibble 	= s->stEvt.ucContentNibble;
	d->ucUserNibble 		= s->stEvt.ucUserNibble;
	HLIB_STD_StrNCpySafe( d->aucEventName, s->stEvt.aucEventName, PVR_SHORT_EVENT_NAME_LEN );
	HLIB_STD_StrNCpySafe( d->aucEventText, s->stEvt.aucText, PVR_EVENT_TEXT );

	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy( d->stComponent, &s->stEvt.stComponent
		, sizeof(SvcMeta_Component_t) * EPG_COMPONENT_DESC_MAX);

	d->bUseFtaCntMgmt = FALSE;
	d->bUseGuidance = FALSE;
#if defined(CONFIG_OP_FREESAT)
	// TODO: 변경.... OTL ㅠㅠ
	HxSTD_MemCopy(&d->ftaCntMgmt, &s->extension.freesat.stEvt.stFtaCntMgmt, sizeof(SvcMeta_FtaCntMgmt_t));
	d->bUseFtaCntMgmt = TRUE;

	HxSTD_MemCopy(&d->guidance, &s->extension.freesat.stEvt.astGuidance, sizeof(SvcMeta_Guidance_t));
	d->bUseGuidance = TRUE;
#endif

#if defined(CONFIG_OP_UK_DTT)
	HxSTD_MemCopy(&d->stFtaCntMgmt, &s->extension.ukdtt.stEvt.stFtaCntMgmt, sizeof(DxUkdtt_Pvr_FtaCntMgmt_t));
	d->bUseFtaCntMgmt = TRUE;

	HxSTD_MemCopy(&d->stGuidance, &s->extension.ukdtt.stEvt.stGuidance, sizeof(DxUkdtt_Pvr_Guidance_t));
	d->bUseGuidance = TRUE;
#endif
	ulExtEvtNum = s->ulExtEvtCnt;
	if (0 == ulExtEvtNum)
	{
		d->pstExtEvt = NULL;
		return d;
	}

	pstPvrExtHead = NULL;
	for( i=0; i<ulExtEvtNum; i++ )
	{
		if( pstPvrExtHead == NULL )
		{
			pstExtEvt = (SvcMeta_ExtEvt_t *)HLIB_STD_MemCalloc( sizeof(SvcMeta_ExtEvt_t) );
			if( pstExtEvt == NULL ) break;

			pstPvrExtHead = pstExtEvt;
		} else {
			pstExtEvt->pNext = (SvcMeta_ExtEvt_t *)HLIB_STD_MemCalloc( sizeof(SvcMeta_ExtEvt_t) );
			pstExtEvt = pstExtEvt->pNext;
			if( pstExtEvt == NULL ) break;
		}

		pstDxExtEvt = &s->pstExtEvts[i];

		pstExtEvt->ucDesNum 	= pstDxExtEvt->ucDesNum;
		pstExtEvt->ucLastDesNum = pstDxExtEvt->ucLastDesNum;
		HxSTD_MemCopy( pstExtEvt->szIso639LangCode, pstDxExtEvt->szIso639LangCode, 4 );
		pstExtEvt->ucItemNum = pstDxExtEvt->ucItemNum;
		for (j = 0; j < pstExtEvt->ucItemNum; j++)
		{
			ulLength = HxSTD_StrLen(pstDxExtEvt->astItem[j].aucDescription);
			if (ulLength)
			{
				pstExtEvt->astItem[j].pszDescription = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength + 1));
				if (NULL != pstExtEvt->astItem[j].pszDescription)
				{
					HxSTD_MemCopy( pstExtEvt->astItem[j].pszDescription, pstDxExtEvt->astItem[j].aucDescription, ulLength );
					pstExtEvt->astItem[j].pszDescription[ulLength] = '\0';
				}
			}
			else
			{
				pstExtEvt->astItem[j].pszDescription = NULL;
			}

			ulLength = HxSTD_StrLen(pstDxExtEvt->astItem[j].aucItem);
			if (ulLength)
			{
				pstExtEvt->astItem[j].pszItem = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength + 1));
				if (NULL != pstExtEvt->astItem[j].pszItem)
				{
					HxSTD_MemCopy( pstExtEvt->astItem[j].pszItem, pstDxExtEvt->astItem[j].aucItem, ulLength);
					pstExtEvt->astItem[j].pszItem[ulLength] = '\0';
				}
			}
			else
			{
				pstExtEvt->astItem[j].pszItem = NULL;
			}
		}
	}
	d->pstExtEvt = pstPvrExtHead;

	ulLength = HxSTD_StrLen(s->aucExtText);
	if ((0 <  ulLength) && (d->pstExtEvt))
	{
		d->pstExtEvt->pszChar = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength + 1));
		if( d->pstExtEvt->pszChar != NULL )
		{
			HxSTD_MemCopy( d->pstExtEvt->pszChar, s->aucExtText, ulLength );
			d->pstExtEvt->pszChar[ulLength] = '\0';
		}
	}
	return d;
}

HERROR				OTL_CONV_DxRecListData2MW_PvrMeta(SvcMeta_Record_t *rec, SvcMeta_Service_t *svc, SvcMeta_Event_t *evt, const DxRecListData_t *s)
{
	if (rec && OTL_CONV_DxRecListData2MW_PvrRecord(rec, s) == NULL)
		return ERR_FAIL;

	if (svc && OTL_CONV_DxRecListData2MW_PvrService(svc, s) == NULL)
		return ERR_FAIL;

	if (evt && OTL_CONV_DxRecListData2MW_PvrEvent(evt, s) == NULL)
		return ERR_FAIL;
	return ERR_OK;
}



