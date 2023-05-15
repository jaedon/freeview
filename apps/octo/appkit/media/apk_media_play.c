/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk2_mediaplay_base.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include <apk.h>
#include <_apk_int.h>


#define ___INTERNAL_STRUCTURES___
typedef struct
{
	HUINT32					 ulViewId;
	OxMediaPlay_Notifier_t	 pfListener;
	OxMediaPlay_EventFilter_t	pfEventFilter;
	void					*pvUserData;
} _apk_mp_Notifier_t;

#define ___INTERNAL_VARIABLES___
STATIC HxList_t				*s_pstApkMp_NotiList = NULL;
STATIC HUINT32				s_ulApkMpSemId = 0;
#define	ENTER_MP_CRITICAL		HxSEMT_Get(s_ulApkMpSemId)
#define	LEAVE_MP_CRITICAL		HxSEMT_Release(s_ulApkMpSemId)

#define ___PRIVATE_FUNCTION___
#define _____CONFLICT_CHECK_____

static HBOOL apk_media_play_checkAntennaTuningParamSame (DxAntInfo_t *pstAnt1, DxAntInfo_t *pstAnt2)
{
	if (pstAnt1->antType != pstAnt2->antType)
	{
		return FALSE;
	}

	switch (pstAnt1->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		{
			DxLnbAntenna_t			*pstLnb1 = &(pstAnt1->info.lnb);
			DxLnbAntenna_t			*pstLnb2 = &(pstAnt2->info.lnb);

			if ((pstLnb1->ulLnbFreq != pstLnb2->ulLnbFreq) ||
				(pstLnb1->lnbVoltage != pstLnb2->lnbVoltage))
			{
				return FALSE;
			}
		}

		break;

	case eDxANT_TYPE_DISEQC:
		{
			DxDiseqcAntenna_t		*pstDiseqc1 = &(pstAnt1->info.diseqc);
			DxDiseqcAntenna_t		*pstDiseqc2 = &(pstAnt2->info.diseqc);

			if ((pstDiseqc1->ulLnbFreq != pstDiseqc2->ulLnbFreq) ||
				(pstDiseqc1->lnbVoltage != pstDiseqc2->lnbVoltage))
			{
				return FALSE;
			}

			if ((pstDiseqc1->ulLnbFreq != OxMP_UNIVERSAL_LNB_FREQ) &&
				(pstDiseqc1->b22kTone != pstDiseqc2->b22kTone))
			{
				return FALSE;
			}

			if ((pstDiseqc1->diseqcVersion != pstDiseqc2->diseqcVersion) ||
				(pstDiseqc1->toneBurst != pstDiseqc2->toneBurst))
			{
				return FALSE;
			}

			switch (pstDiseqc1->diseqcVersion)
			{
			case eDxDISEQC_VER_1_2:
				if (pstDiseqc1->ucMotorPosition != pstDiseqc2->ucMotorPosition)
				{
					return FALSE;
				}

				break;

			case eDxDISEQC_VER_1_3:
				if (pstDiseqc1->sSatLonggitude != pstDiseqc2->sSatLonggitude)
				{
					return FALSE;
				}

				break;

			case eDxDISEQC_VER_1_0:
			case eDxDISEQC_VER_2_0:
			default:
				if (pstDiseqc1->diseqcInput != pstDiseqc2->diseqcInput)
				{
					return FALSE;
				}

				break;
			}
		}

		break;

	case eDxANT_TYPE_SCD:
		{
			DxScdAntenna_t			*pstScd1 = &(pstAnt1->info.scd);
			DxScdAntenna_t			*pstScd2 = &(pstAnt2->info.scd);

			if ((pstScd1->ulLnbFreq != pstScd2->ulLnbFreq) ||
				(pstScd1->lnbVoltage != pstScd2->lnbVoltage) ||
				(pstScd1->b22kTone != pstScd2->b22kTone))
			{
				return FALSE;
			}

			// except. scdInput
			if ((pstScd1->ulScdUserBandNum != pstScd2->ulScdUserBandNum) ||
				(pstScd1->ulScdUserBandFreq != pstScd2->ulScdUserBandFreq) ||
				(pstScd1->ulScdUserBandNum1 != pstScd2->ulScdUserBandNum1) ||
				(pstScd1->ulScdUserBandFreq1 != pstScd2->ulScdUserBandFreq1) ||
				(pstScd1->ulScdUserBandNum2 != pstScd2->ulScdUserBandNum2) ||
				(pstScd1->ulScdUserBandFreq2 != pstScd2->ulScdUserBandFreq2) ||
				(pstScd1->ulScdUserBandNum3 != pstScd2->ulScdUserBandNum3) ||
				(pstScd1->ulScdUserBandFreq3 != pstScd2->ulScdUserBandFreq3))
			{
				return FALSE;
			}
		}

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

static HUINT32 apk_media_play_getActualLnbFreq (HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return 9750;
	}
	else
	{
		return 10600;
	}
}

static HBOOL apk_media_play_getActual22kTone(HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

static HBOOL apk_media_play_checkRfInputAcceptable (DxTransponder_t *pstTp, DxTuneParam_t *pstRfTune)
{
	DxAntInfo_t					*pstRfAntInfo = &(pstRfTune->sat.antInfo);
	DxAntInfo_t					*pstTpAntInfo = &(pstTp->tuningParam.sat.antInfo);
	DxSat_TuningTPInfo_t		*pstRfTuneInfo = &(pstRfTune->sat.tuningInfo);
	DxSat_TuningTPInfo_t		*pstTpTuneInfo = &(pstTp->tuningParam.sat.tuningInfo);

	if (apk_media_play_checkAntennaTuningParamSame (pstRfAntInfo, pstTpAntInfo) != TRUE)
	{
		return FALSE;
	}

	// SCD는 Antenna 정보만 겹치지 않으면 RF-Input 단에서 충돌할 일이 없으므로 막는다.
	if (eDxANT_TYPE_SCD != pstRfAntInfo->antType)
	{
		if (pstRfTuneInfo->ePolarization != pstTpTuneInfo->ePolarization)
		{
			return FALSE;
		}

		if ((eDxANT_TYPE_DISEQC == pstRfAntInfo->antType) &&
			(OxMP_UNIVERSAL_LNB_FREQ == pstRfAntInfo->info.diseqc.ulLnbFreq))
		{
			HUINT32 		 ulRfLnbFreq	= apk_media_play_getActualLnbFreq (pstRfTuneInfo->ulFrequency);
			HUINT32 		 ulTpLnbFreq	= apk_media_play_getActualLnbFreq (pstTpTuneInfo->ulFrequency);
			HBOOL			 bRf22kTone 	= apk_media_play_getActual22kTone (pstRfTuneInfo->ulFrequency);
			HBOOL			 bTp22kTone 	= apk_media_play_getActual22kTone (pstTpTuneInfo->ulFrequency);

			if ((ulRfLnbFreq != ulTpLnbFreq) ||
				(bRf22kTone != bTp22kTone))
			{
				return FALSE;
			}
		}
		else if ((eDxANT_TYPE_LNB_ONLY == pstRfAntInfo->antType) &&
			(OxMP_UNIVERSAL_LNB_FREQ == pstRfAntInfo->info.lnb.ulLnbFreq))
		{
			HUINT32 		 ulRfLnbFreq	= apk_media_play_getActualLnbFreq (pstRfTuneInfo->ulFrequency);
			HUINT32 		 ulTpLnbFreq	= apk_media_play_getActualLnbFreq (pstTpTuneInfo->ulFrequency);
			HBOOL			 bRf22kTone 	= apk_media_play_getActual22kTone (pstRfTuneInfo->ulFrequency);
			HBOOL			 bTp22kTone 	= apk_media_play_getActual22kTone (pstTpTuneInfo->ulFrequency);

			if ((ulRfLnbFreq != ulTpLnbFreq) ||
				(bRf22kTone != bTp22kTone))
			{
				return FALSE;
			}
		}

	}

	return TRUE;
}

static HBOOL apk_media_play_checkSatTunerAcceptable (DxSatTuningInfo_t *pstTune1, DxSatTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;
	DxAntInfo_t					*pstAnt1 = &(pstTune1->antInfo);
	DxAntInfo_t					*pstAnt2 = &(pstTune2->antInfo);
	DxSat_TuningTPInfo_t		*pstTpInfo1 = &(pstTune1->tuningInfo);
	DxSat_TuningTPInfo_t		*pstTpInfo2 = &(pstTune2->tuningInfo);

	if (apk_media_play_checkAntennaTuningParamSame (pstAnt1, pstAnt2) != TRUE)
	{
		return FALSE;
	}

	if ((pstTpInfo1->ulSymbolRate != pstTpInfo2->ulSymbolRate) ||
		(pstTpInfo1->eTransSpec != pstTpInfo2->eTransSpec) ||
		(pstTpInfo1->eRollOff != pstTpInfo2->eRollOff))
	{
		return FALSE;
	}

	if ((eDxSAT_CODERATE_AUTO != pstTpInfo1->eFecCodeRate) && (eDxSAT_CODERATE_AUTO != pstTpInfo2->eFecCodeRate))
	{
		if (pstTpInfo1->eFecCodeRate != pstTpInfo2->eFecCodeRate)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_POLAR_AUTO != pstTpInfo1->ePolarization) && (eDxSAT_POLAR_AUTO != pstTpInfo2->ePolarization))
	{
		if (pstTpInfo1->eFecCodeRate != pstTpInfo2->eFecCodeRate)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_PSK_AUTO != pstTpInfo1->ePskMod) && (eDxSAT_PSK_AUTO != pstTpInfo2->ePskMod))
	{
		if (pstTpInfo1->ePskMod != pstTpInfo2->ePskMod)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_PILOT_AUTO != pstTpInfo1->ePilot) && (eDxSAT_PILOT_AUTO != pstTpInfo2->ePilot))
	{
		if (pstTpInfo1->ePilot != pstTpInfo2->ePilot)
		{
			return FALSE;
		}
	}

	//////////////// Frequency 비교 ////////////////
	//		45~18Ms/s	=	+/- (8MHz) search
	//		18 ~ 11Ms/s =	+/- (6MHz) search
	//		11 ~ 8Ms/s	=	+/- (5MHz) search
	//		8 ~ 4Ms/s	=	+/- (4MHz) search
	//		4 ~ 2Ms/s	=	+/- (3.8MHz) search
	//	=> 2003.09.09, 권기호 과장으로부터 입수
	nFreqDiff = (HINT32)pstTpInfo1->ulFrequency - (HINT32)pstTpInfo2->ulFrequency;

	if (pstTpInfo1->ulSymbolRate >= 18000)
	{
		if ((nFreqDiff >= 8) || (nFreqDiff <= -8))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 11000)
	{
		if ((nFreqDiff >= 6) || (nFreqDiff <= -6))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 8000)
	{
		if ((nFreqDiff >= 5) || (nFreqDiff <= -5))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 2000)
	{
		if ((nFreqDiff >= 4) || (nFreqDiff <= -4))
		{
			return FALSE;
		}
	}
	else
	{
		if (nFreqDiff != 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

static HBOOL apk_media_play_checkCabTunerAcceptable (DxCabTuningInfo_t *pstTune1, DxCabTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;

	if (pstTune1->ulSymbolRate != pstTune2->ulSymbolRate)
	{
		return FALSE;
	}

	if ((eDxCAB_CONSTELLATION_AUTO != pstTune1->constellation) && (eDxCAB_CONSTELLATION_AUTO != pstTune2->constellation))
	{
		if (pstTune1->constellation != pstTune2->constellation)
		{
			return FALSE;
		}
	}

	if ((eDxCAB_SPECTRUM_AUTO != pstTune1->spectrum) && (eDxCAB_SPECTRUM_AUTO != pstTune2->spectrum))
	{
		if (pstTune1->spectrum != pstTune2->spectrum)
		{
			return FALSE;
		}
	}

	nFreqDiff = (HINT32)pstTune1->ulFrequency - (HINT32)pstTune2->ulFrequency;
	if ((nFreqDiff > 32) || (nFreqDiff < -32))
	{
		return FALSE;
	}

	return TRUE;
}

static HBOOL apk_media_play_checkTerTunerAcceptable (DxTerTuningInfo_t *pstTune1, DxTerTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;

	if ((eDxTER_TRANSMODE_AUTO != pstTune1->eTransMode) && (eDxTER_TRANSMODE_AUTO != pstTune2->eTransMode))
	{
		if (pstTune1->eTransMode != pstTune2->eTransMode)
		{
			return FALSE;
		}
	}

	if ((eDxTER_GUARDINTERVAL_AUTO != pstTune1->eGuardInterval) && (eDxTER_GUARDINTERVAL_AUTO != pstTune2->eGuardInterval))
	{
		if (pstTune1->eGuardInterval != pstTune2->eGuardInterval)
		{
			return FALSE;
		}
	}

	if ((eDxTER_DVB_T2 == pstTune1->eSystem) && (0x0000FFFF != pstTune1->stT2Param.ulPlpId) &&
		(eDxTER_DVB_T2 == pstTune2->eSystem) && (0x0000FFFF != pstTune2->stT2Param.ulPlpId))
	{
		if (pstTune1->stT2Param.ulPlpId != pstTune2->stT2Param.ulPlpId)
		{
			return FALSE;
		}
	}

	nFreqDiff = (HINT32)pstTune1->ulFrequency - (HINT32)pstTune2->ulFrequency;
	if ((nFreqDiff > 3000) || (nFreqDiff < -3000))
	{
		return FALSE;
	}

	return TRUE;
}

static HBOOL apk_media_play_checkTunerAcceptable (DxTransponder_t *pstTp, DxTuneParam_t *pstTuner)
{
	switch (pstTp->deliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		return apk_media_play_checkSatTunerAcceptable (&(pstTp->tuningParam.sat), &(pstTuner->sat));

	case eDxDELIVERY_TYPE_CAB:
		return apk_media_play_checkCabTunerAcceptable (&(pstTp->tuningParam.cab), &(pstTuner->cab));

	case eDxDELIVERY_TYPE_TER:
		return apk_media_play_checkTerTunerAcceptable (&(pstTp->tuningParam.ter), &(pstTuner->ter));

	default:
		break;
	}

	return FALSE;
}


static HERROR apk_media_play_checkServiceTunable (OxMediaPlay_ConflictInfo_t *pstConflictInfo, HINT32 nSvcUid, OxMediaPlay_CheckTune_e *peResult)
{
	HBOOL				 bFound;
	HUINT32				 ulCount;
	DxService_t			*pstSvc = NULL;
	DxTransponder_t		*pstTp = NULL;
	DxAntInfo_t			*pstAnt = NULL;


	pstSvc = APK_META_SVC_GetService (nSvcUid);
	if (NULL == pstSvc)
	{
		HxLOG_Error ("APK_META_SVC_GetService err: nSvcUid=%d\n", nSvcUid);
		*peResult = eOxMP_CHECKTUNE_SvcNotExist;
		goto END_FUNC;
	}

	pstTp = APK_META_SVC_GetTransponder (pstSvc->tsuid);
	if (NULL == pstTp)
	{
		HxLOG_Error ("APK_METASVC_GetTransponder err: tsuid=%d\n", pstSvc->tsuid);
		*peResult = eOxMP_CHECKTUNE_SvcNotExist;
		goto END_FUNC;
	}

	if (eDxDELIVERY_TYPE_SAT == pstTp->deliType)
	{
		// Find the antenna Information
		pstAnt = APK_META_ANTENNA_GetInfo (pstSvc->antuid);
		if (NULL == pstAnt)
		{
			HxLOG_Error ("APK_METAANT_GetInfo err: antuid=%d\n", pstSvc->antuid);
			*peResult = eOxMP_CHECKTUNE_SvcNotExist;
			goto END_FUNC;
		}

		HxSTD_MemCopy (&(pstTp->tuningParam.sat.antInfo), pstAnt, sizeof(DxAntInfo_t));
		pstTp->tuningParam.sat.antennaType = pstAnt->antType;

		// 1. RF-Input: Only for the satellite
		for (ulCount = 0, bFound = FALSE; ulCount < pstConflictInfo->ulRfInfoNum; ulCount++)
		{
			OxMediaPlay_ConfRfInfo_t		*pstRfInfo = &(pstConflictInfo->astRfInfo[ulCount]);

			if (TRUE == pstRfInfo->bAvailable)
			{
				if ((TRUE != pstRfInfo->bAllocated) ||
					(TRUE == apk_media_play_checkRfInputAcceptable (pstTp, &(pstRfInfo->unTuneInfo))))
				{
					bFound = TRUE;
					break;
				}
			}
		}

		if (TRUE != bFound)
		{
			HxLOG_Error ("No available RF-Input...\n");
			*peResult = eOxMP_CHECKTUNE_NoTunerResource;
			goto END_FUNC;
		}
	}

#if 1
	// 2. Tuner:
	for (ulCount = 0, bFound = FALSE; ulCount < pstConflictInfo->ulTunerInfoNum; ulCount++)
	{
		OxMediaplay_ConfTunerInfo_t	*pstTunerInfo = &(pstConflictInfo->astTunerInfo[ulCount]);

		if ((TRUE == pstTunerInfo->bAvailable) &&
			(pstTp->deliType & pstTunerInfo->eTunerDeliType))
		{
			if ((TRUE != pstTunerInfo->bAllocated) ||
				(TRUE == apk_media_play_checkTunerAcceptable (pstTp, &(pstTunerInfo->unTuneInfo))))
			{
				bFound = TRUE;
				break;
			}
		}
	}

	if (TRUE != bFound)
	{
		HxLOG_Error ("No available Tuner...\n");
		*peResult = eOxMP_CHECKTUNE_NoTunerResource;
		goto END_FUNC;
	}
#endif

	*peResult = eOxMP_CHECKTUNE_NoProblem;

END_FUNC:
	if (NULL != pstSvc)					{ APK_META_SVC_Delete (pstSvc); }
	if (NULL != pstTp)					{ APK_META_SVC_Delete (pstTp); }
	if (NULL != pstAnt)					{ APK_Free (pstAnt); }
	return ERR_OK;
}


#define _____NOTIFIER_FUNCTION_____
STATIC void *apk_media_play_duplicateCbNotifier (void *pvSrc, OxMediaPlay_Event_e eEvent, HUINT32 *pulSize)
{
	void		*pvDup = NULL;

	if (NULL != pvSrc)
	{
		switch (eEvent)
		{
		case eOxMP_EVENT_RequestPincode:				// Argument Allocation 수정 필요 !!!
		case eOxMP_EVENT_SI_RCT_REQUEST_IMAEG_ICON:		// Argument Allocation 수정 필요 !!!
			*pulSize = sizeof(OxMediaPlay_EventNotify_t);
			pvDup = (void *)HLIB_STD_MemDup(pvSrc, sizeof(OxMediaPlay_EventNotify_t));
			break;

		case eOxMP_EVENT_STARTED:
			*pulSize = sizeof(OxMediaPlay_StartInfo_t);
			pvDup = (void *)HLIB_STD_MemDup(pvSrc, sizeof(OxMediaPlay_StartInfo_t));
			break;

		case eOxMP_EVENT_PLAYSTATE:
			*pulSize = sizeof(OxMediaPlay_PlayState_e);
			pvDup = (void *)HLIB_STD_MemDup(pvSrc, sizeof(OxMediaPlay_PlayState_e));
			break;

		case eOxMP_EVENT_PLAYERROR:
			*pulSize = sizeof(OxMediaPlay_PlayError_e);
			pvDup = (void *)HLIB_STD_MemDup(pvSrc, sizeof(OxMediaPlay_PlayError_e));
			break;

		case eOxMP_EVENT_DRM_BLOCK:
			*pulSize = sizeof(OxMediaPlay_DrmRightsError_t);
			pvDup = (void *)HLIB_STD_MemDup(pvSrc, sizeof(OxMediaPlay_DrmRightsError_t));
			break;

		case eOxMP_EVENT_VIDEOSTREAMINFO:
			*pulSize = sizeof(OxMediaPlay_VideoStreamInfo_t);
			pvDup = (void *)HLIB_STD_MemDup(pvSrc, sizeof(OxMediaPlay_VideoStreamInfo_t));
			break;

		case eOxMP_EVENT_LIVE_SvcStopped:				// Notifier 가 NULL인 항목은 다 따로 둔다.
			*pulSize = 0;
			pvDup = NULL;
			break;

		default:
			*pulSize = sizeof(OxMediaPlay_EventNotify_t);
			pvDup = (void *)HLIB_STD_MemDup(pvSrc, sizeof(OxMediaPlay_EventNotify_t));
			break;
		}
	}

	return pvDup;
}

STATIC void	apk_media_play_freeCbNotifier(HINT32 nArgc, void *apArgV[])
{
	OxMediaPlay_Event_e	 eEvent;
	void				*pvData;

	if (7 != nArgc)
	{
		HxLOG_Critical(HxANSI_COLOR_RED("!!!!! nArgc = %d, not 7 !!! You Must Check !!!"), nArgc);
		return;
	}

	eEvent = (OxMediaPlay_Event_e)apArgV[4];
	pvData = apArgV[5];
	if (pvData)
	{
		switch(eEvent)
		{
		default:
			APK_Free(pvData);
		}
	}
}

STATIC void apk_media_play_CbNotifier (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HxList_t		*pstListItem;

	ENTER_MP_CRITICAL;
	for (pstListItem = s_pstApkMp_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_mp_Notifier_t		*pstNotiItem = (_apk_mp_Notifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotiItem) &&
			(ulViewId == pstNotiItem->ulViewId) && (NULL != pstNotiItem->pfListener))
		{
			HBOOL	bNotify = TRUE;
			HBOOL	bUpdated = TRUE;

			if (pstNotiItem->pfEventFilter && pstNotiItem->pfEventFilter(ulViewId, eType, eEvent, &bUpdated))
			{
				bNotify = bUpdated;
			}

			if (bNotify == TRUE)
			{
				APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

				stApkEvtArgs.pfnFreeFunc = apk_media_play_freeCbNotifier;
				stApkEvtArgs.ulArgCount = 7;
				stApkEvtArgs.apArgV[0] = (void *)pstNotiItem->pfListener;
				stApkEvtArgs.apArgV[1] = (void *)ulViewId;
				stApkEvtArgs.apArgV[2] = (void *)ulSessionId;
				stApkEvtArgs.apArgV[3] = (void *)eType;
				stApkEvtArgs.apArgV[4] = (void *)eEvent;
				stApkEvtArgs.apArgV[5] = (void *)APK_MemDup(pstNotifyData, sizeof(OxMediaPlay_NotifierData_t));
				stApkEvtArgs.apArgV[6] = (void *)pstNotiItem->pvUserData;

				APK_EVENT_Emit(&stApkEvtArgs);
			}
		}
	}
	LEAVE_MP_CRITICAL;
}

STATIC void apk_media_play_CbInitNotified (void)
{
	HUINT32 			 ulViewId, ulViewNum;

	HxLOG_Info ("MediaPlay Activated: Now it can be used:\n");

	ulViewNum = OAPI_MEDIA_PLAY_GetViewNumber();
	for (ulViewId = 0; ulViewId < ulViewNum; ulViewId++)
	{
		(void)OAPI_MEDIA_PLAY_SetListener(ulViewId, apk_media_play_CbNotifier, NULL);
	}
}

STATIC _apk_mp_Notifier_t *apk_media_play_FindNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstApkMp_NotiList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		_apk_mp_Notifier_t		*pstNoti = (_apk_mp_Notifier_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstNoti) &&
			(pstNoti->ulViewId == ulViewId) &&
			(pstNoti->pfListener == pfNotifier) &&
			(pstNoti->pvUserData == pvUserData))
		{
			return pstNoti;
		}
	}

	return NULL;
}

#if defined(CONFIG_DEBUG)
HINT32 apk_media_play_CmdGetPlaySpeed(void *pvArg)
{
	HCHAR		*aucArg = NULL;
	HERROR		hErr = ERR_FAIL;
	HUINT32 	ulViewId = 0;
	HINT32 		nSpeed = 0;

	/* 1st argument - CRID Filter*/
	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if(aucArg != NULL)
	{
		if(sscanf(aucArg, "%d", &ulViewId) == 0)
		{
			HLIB_CMD_Printf("USAGE : getplayspeed [viewID]\n");
			return HxCMD_ERR;
		}
	}

	hErr = OAPI_MEDIA_PLAY_GetPlaySpeed (ulViewId, &nSpeed);

	HLIB_CMD_Printf("===============================================\n");
	HLIB_CMD_Printf("===== getplayspeed [%d] =====\n", ulViewId);
	HLIB_CMD_Printf("===============================================\n");
	HLIB_CMD_Printf("View[ %d ] - PlaySpeed[ %d ]\n", ulViewId, nSpeed);

	return HxCMD_OK;
}

void	apk_media_play_InitCommand(void)
{
#define	hCmdHandle		"APPKIT"
	HLIB_CMD_RegisterWord (hCmdHandle, apk_media_play_CmdGetPlaySpeed, "getplayspeed", "Get Play Speed", "getplayspeed [viewID]");
}
#endif

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___

#define ___BASE___
/********************************************************************************
 * APK_MEDIA_PLAY_Init
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_Init (void)
{
	HxLOG_Trace();

	HxSEMT_Create(&s_ulApkMpSemId, "apkmp", HxSEMT_FIFO);

	OAPI_MEDIA_PLAY_Init(apk_media_play_CbInitNotified);

#if defined(CONFIG_DEBUG)
	apk_media_play_InitCommand();
#endif

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetViewNumber
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetViewNumber (HUINT32 *pulViewNum)
{
	HxLOG_Trace();

	if (NULL == pulViewNum)
	{
		HxLOG_Error("Error.");
		return ERR_FAIL;
	}

	if (NULL != pulViewNum)
	{
		*pulViewNum = OAPI_MEDIA_PLAY_GetViewNumber();
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_Init
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetMainViewId (HUINT32 ulViewId)
{
	HxLOG_Trace();

	return OAPI_MEDIA_PLAY_SetMainViewId(ulViewId);
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetMainViewId
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetMainViewId (HUINT32 *pulViewId)
{
	HxLOG_Trace();

	if (NULL != pulViewId)
	{
		*pulViewId = OAPI_MEDIA_PLAY_GetMainViewId();
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetPlayType
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetPlayType (HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType)
{
	OxMediaPlay_MediaType_e 	 eOapiType;
	HERROR						 hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetPlayType (ulViewId, &eOapiType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetPlayType err:\n");
		return ERR_FAIL;
	}

	if (NULL != peType)
	{
		*peType = eOapiType;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetNeedRadioBg
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetNeedRadioBg (HUINT32 ulViewId, HBOOL *pbNeed)
{
	HERROR						 hErr;

	HxLOG_Trace();

	if( NULL == pbNeed )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetNeedRadioBg (ulViewId, pbNeed);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetNeedRadioBg err:\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("NeedRadigBG is (%s)\n", (*pbNeed == TRUE)?"TRUE":"FALSE" );

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_RegisterNotifier
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_RegisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, OxMediaPlay_EventFilter_t pfEventFilter, void *pvUserData)
{
	_apk_mp_Notifier_t	*pstNoti;

	HxLOG_Trace();

	if (NULL == pfNotifier)
	{
		HxLOG_Error("Notifier NULL \n");
		return ERR_FAIL;
	}

	ENTER_MP_CRITICAL;
	pstNoti = apk_media_play_FindNotifier(ulViewId, pfNotifier, pvUserData);
	if (NULL != pstNoti)
	{
		HxLOG_Error("Already Registered:\n");
		LEAVE_MP_CRITICAL;
		return ERR_OK;
	}

	pstNoti = (_apk_mp_Notifier_t *)HLIB_STD_MemCalloc(sizeof(_apk_mp_Notifier_t));
	if (NULL == pstNoti)
	{
		HxLOG_Error("Memory allocation failed:\n");
		LEAVE_MP_CRITICAL;
		return ERR_FAIL;
	}

	pstNoti->ulViewId	= ulViewId;
	pstNoti->pfListener	= pfNotifier;
	pstNoti->pfEventFilter		= pfEventFilter;
	pstNoti->pvUserData	= pvUserData;

	s_pstApkMp_NotiList = HLIB_LIST_Append (s_pstApkMp_NotiList, (void *)pstNoti);
	LEAVE_MP_CRITICAL;
	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_UnregisterNotifier
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_UnregisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData)
{
	_apk_mp_Notifier_t	*pstNoti;

	HxLOG_Trace();

	if (NULL == pfNotifier)
	{
		HxLOG_Error("Notifier NULL \n");
		return ERR_FAIL;
	}

	ENTER_MP_CRITICAL;
	pstNoti = apk_media_play_FindNotifier(ulViewId, pfNotifier, pvUserData);
	if (NULL == pstNoti)
	{
		HxLOG_Error("Not Registered:\n");
		LEAVE_MP_CRITICAL;
		return ERR_OK;
	}

	s_pstApkMp_NotiList = HLIB_LIST_Remove (s_pstApkMp_NotiList, (const void *)pstNoti);
	HLIB_STD_MemFree (pstNoti);
	LEAVE_MP_CRITICAL;
	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetSessionId
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetSessionId (HUINT32 ulViewId, HUINT32 *pulSessionId)
{
	HxLOG_Trace();

	return OAPI_MEDIA_PLAY_GetCurrentSessionId (ulViewId, pulSessionId);
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetRequestId
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetRequestId (ulViewId, eType, pulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetRequestId err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetStreamAspectRatio
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetStreamAspectRatio (HUINT32 ulViewId, DxAspectRatio_e *peAspectRatio)
{
	HERROR hErr;

	HxLOG_Trace();

	if (NULL == peAspectRatio)
	{
		HxLOG_Error ("Invalid Parameter!\n");
		return ERR_FAIL;
	}

	hErr = OAPI_MEDIA_PLAY_GetStreamAspectRatio (ulViewId, peAspectRatio);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetStreamAspectRatio err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetStreamVideoSize
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetStreamVideoSize(HUINT32 ulViewId, HUINT32 *ulHSize, HUINT32 *ulVSize)
{
	HERROR hErr;

	HxLOG_Trace();

	if ( (NULL == ulHSize) || (NULL == ulVSize) )
	{
		HxLOG_Error ("Invalid Parameter!\n");
		return ERR_FAIL;
	}

	hErr = OAPI_MEDIA_PLAY_GetStreamVideoSize (ulViewId, ulHSize, ulVSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("err return:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetOsdDisplayPoint
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetOsdDisplayPoint (HUINT32 ulViewId, HINT32 nRefWidth, HINT32 nRefHeight, HINT32 nVideoPointX, HINT32 nVideoPointY,HINT32 *pnScaledX, HINT32 *pnScaledY)
{
	HERROR hErr;

	HxLOG_Trace();

	if ( (NULL == pnScaledX) || (NULL == pnScaledY) )
	{
		HxLOG_Error ("Invalid Parameter!\n");
		return ERR_FAIL;
	}

	hErr = OAPI_MEDIA_PLAY_GetOsdDisplayPoint (ulViewId, nRefWidth, nRefHeight, nVideoPointX, nVideoPointY, pnScaledX, pnScaledY);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetOsdDisplayPoint err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____FOR_MHEG_____
HERROR APK_MEDIA_PLAY_CaptureStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulCaptureWidth, HUINT32 ulCaptureHeight, DxPixelFormat_e ePixelFormat, HUINT32 ulBufferSize, HUINT8 *pBuffer)
{
	HERROR hErr;

	HxLOG_Trace();

	if (NULL == pBuffer)
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_CaptureStillPicture (ulViewId, eVideoCodec, ulCaptureWidth, ulCaptureHeight, ePixelFormat, ulBufferSize, pBuffer);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_StartAudioClip err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_StartStillPicture (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT32 ulBufferSize, HUINT8 *pBuffer)
{
	HERROR hErr;

	HxLOG_Trace();

	if (NULL == pBuffer)
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_StartStillPicture (ulViewId, eVideoCodec, ulBufferSize, pBuffer);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_StartAudioClip err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_StopStillPicture (HUINT32 ulViewId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_StopStillPicture (ulViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_StartAudioClip err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_StartAudioClip (HUINT32 ulRequestId, HUINT8 *pucBuffer, HUINT32 ulBufferBytes, DxAudioCodec_e	eCodec, HUINT32 ulLoopCnt)
{
	HERROR hErr;

	HxLOG_Trace();

	if (NULL == pucBuffer)
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_StartAudioClip (ulRequestId, pucBuffer, ulBufferBytes, eCodec, ulLoopCnt);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_StartAudioClip err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_PauseAudioClip(HUINT32 ulRequestId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_PauseAudioClip (ulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_PauseAudioClip err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_ResumeAudioClip(HUINT32 ulRequestId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_ResumeAudioClip (ulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_ResumeAudioClip err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_StopAudioClip(HUINT32 ulRequestId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_StopAudioClip (ulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_StopAudioClip err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_SetVideoFreeze(HUINT32 ulViewId, HBOOL bFreeze)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetVideoFreeze (ulViewId, bFreeze);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_SetVideoFreeze err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_SetAudioFreeze(HUINT32 ulViewId, HBOOL bFreeze)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetAudioFreeze (ulViewId, bFreeze);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_SetAudioFreeze err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_SetMhegStatus (OxMediaPlay_MhegStatus_e eStatus)
{
	HxLOG_Trace();

	return OAPI_MEDIA_PLAY_SetMhegStatus(eStatus);
}

HERROR APK_MEDIA_PLAY_SetMhegInterruptibleFlag (HUINT32 ulViewId, HBOOL bMhegInterruptible)
{
	HxLOG_Trace();

	return OAPI_MEDIA_PLAY_SetMhegInterruptibleFlag( ulViewId,  bMhegInterruptible);
}

HERROR APK_MEDIA_PLAY_RequestPincode (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT8 *pszMsg)
{
	HxLOG_Trace();

	return OAPI_MEDIA_PLAY_RequestPincode(ulViewId, eMediaType, pszMsg);
}


HERROR APK_MEDIA_PLAY_ResultPincode (HBOOL bPermit)
{
	return OAPI_MEDIA_PLAY_ResultPincode(bPermit);
}

HERROR APK_MEDIA_PLAY_StopHbbtvMonitoring (HUINT32 ulViewId)
{
	return OAPI_MEDIA_PLAY_StopHbbtvMonitoring(ulViewId);
}

#define _____TRACK_____
/********************************************************************************
 * APK_MEDIA_PLAY_GetComponentNum
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetComponentNum (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 *pulCompNum)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulCompNum )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetComponentNum ( ulViewId, eCompType, pulCompNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetComponentNum err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetComponent
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetComponent (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, OxMediaPlay_Component_t *pstComponet)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pstComponet )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetComponent (ulViewId, eCompType, nCompIndex, pstComponet);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetComponent err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetComponentIndex
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 *pnCompIndex, HINT32 *pnCompSubIndex)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pnCompIndex )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetComponentIndex ( ulViewId, eCompType, pnCompIndex, pnCompSubIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetComponentIndex err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetComponentIndexEx
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetComponentIndexEx (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex, HUINT8 *pstrLang)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetComponentIndexEx (ulViewId, eCompType, nCompIndex, pstrLang);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetComponentIndexEx err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetComponentIndex
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetComponentIndex (ulViewId, eCompType, nCompIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetComponentIndex err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_UnsetActiveComponent
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_UnsetComponentIndex (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_UnsetComponentIndex (ulViewId, eCompType, nCompIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_UnsetComponentIndex err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____PLAYER_CONTROL_____
/********************************************************************************
 * APK_MEDIA_PLAY_Start
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_Start (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId)
{
	HUINT32		ulSessionId;
	HERROR		hErr;

	HxLOG_Trace();

	if (NULL == punStart)
	{
		return ERR_FAIL;
	}

	if ((eOxMP_MEDIATYPE_LIVE == punStart->eType) && (eOxMP_LIVETYPE_FCC == punStart->stLive.eLiveType))
	{
		ulViewId = 0;
	}

	HxLOG_Print("ulViewId = %d\n", ulViewId);
	hErr = OAPI_MEDIA_PLAY_Start (ulViewId, punStart, &ulSessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_Start err: ulViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	if (NULL != pulSessionId)
	{
		*pulSessionId = ulSessionId;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*** Media Play Start *** \n", __LINE__);
	HxLOG_Debug("* TYPE\t: %d\n", punStart->eType);
	HxLOG_Debug("* ulViewId\t: %d\n", ulViewId);

	switch(punStart->eType)
	{
		case eOxMP_MEDIATYPE_LIVE:			HxLOG_Debug("* MasterSvcUid\t: %d\n", punStart->stLive.ulMasterSvcUid);
											break;
		case eOxMP_MEDIATYPE_PVRPLAYBACK:	HxLOG_Debug("* ContentId\t: %d\n", punStart->stPvrPb.ulContentId);
											break;
		case eOxMP_MEDIATYPE_TSRPLAYBACK:	HxLOG_Debug("* MasterSvcUid\t: %d\n", punStart->stTsrPb.ulMasterSvcUid);
											break;
		case eOxMP_MEDIATYPE_MEDIAPLAY:		HxLOG_Debug("* URL\t: %s\n", punStart->stMediaPlay.szUrl);
											break;
		default:
			break;
	}
#endif
	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_Stop
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_Stop (HUINT32 ulViewId)
{
	HERROR	hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_Stop (ulViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_Start err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_StopWithParams (only for Netflix)
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_StopWithParams (HUINT32 ulViewId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulSessionId) /* (OxMediaPlay_PlayerType_e ePlayerType) */
{
	/* TODO : Netflix NRDP suspend-timeout 문제가 해결되면 APK_MEDIA_PLAY_Stop 으로 대체되어야 함 */
	HERROR	hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_StopWithParams(ulViewId, eMediaType, ulSessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error> OAPI_MEDIA_PLAY_Start err: ViewId=%d, MediaType:%d, SessionId:%d\n", ulViewId, eMediaType, ulSessionId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetPlaySpeed
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetPlaySpeed (HUINT32 ulViewId, HINT32 nSpeed)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetPlaySpeed (ulViewId, nSpeed);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetPlaySpeed err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetPlayPosition
 ********************************************************************************/
#if defined(CONFIG_TIMESTAMP_64)
HERROR APK_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT64 ullPosition)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetPlayPosition (ulViewId, ullPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetPlayPosition err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#else
HERROR APK_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetPlayPosition (ulViewId, ulPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetPlayPosition err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

/********************************************************************************
 * APK_MEDIA_PLAY_SetPlaySegment
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetPlaySegment (HUINT32 ulViewId, HUINT32 ulPosSeg)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetPlaySegment (ulViewId, ulPosSeg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetPlaySegment err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____PLAYER_GETTER_____
/********************************************************************************
 * APK_MEDIA_PLAY_GetPlaySpeed
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetPlaySpeed (HUINT32 ulViewId, HINT32 *pnSpeed)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pnSpeed )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetPlaySpeed (ulViewId, pnSpeed);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetPlaySpeed err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetPlayPosition
 ********************************************************************************/
#if defined(CONFIG_TIMESTAMP_64)
HERROR APK_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT64 *pullPosition)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pullPosition )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetPlayPosition (ulViewId, pullPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetPlayPosition err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#else
HERROR APK_MEDIA_PLAY_GetPlayPosition (HUINT32 ulViewId, HUINT32 *pulPosition)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulPosition )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetPlayPosition (ulViewId, pulPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetPlayPosition err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

/********************************************************************************
 * APK_MEDIA_PLAY_GetPlayState
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetPlayState (HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == peState )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetPlayState (ulViewId, peState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetPlayState err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetPlayError
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetPlayError (HUINT32 ulViewId, OxMediaPlay_PlayError_e *peError, OxMediaPlay_ErrorReason_e *peReason)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == peError )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetPlayError (ulViewId,  peError, peReason);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetPlayError err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetBufferedTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetBufferedTime (HUINT32 ulViewId, HUINT32 *pulBufferred)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulBufferred )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetBufferedTime (ulViewId, pulBufferred);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetBufferedTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}
	return ERR_FAIL;
}


/********************************************************************************
 * APK_MEDIA_PLAY_GetTsrStartTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetTsrStartTime (HUINT32 ulViewId, HUINT32 *pulTsrStartTime)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulTsrStartTime )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetTsrStartTime (ulViewId, pulTsrStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetTsrStartTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetTsrStartTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetTsrRecTime(HUINT32 ulViewId, HUINT32 *pulRecTime)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulRecTime )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetTsrRecTime (ulViewId, pulRecTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetTsrRecTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetDurationTime
 ********************************************************************************/
#if defined(CONFIG_TIMESTAMP_64)
HERROR APK_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT64 *pullDurationTime)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pullDurationTime )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetDurationTime (ulViewId, pullDurationTime );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetDurationTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	HxLOG_Debug("[view:%d, duration:%llu]\n", ulViewId, *pullDurationTime);
	return ERR_OK;
}
#else
HERROR APK_MEDIA_PLAY_GetDurationTime (HUINT32 ulViewId, HUINT32 *pulDurationTime)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulDurationTime )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetDurationTime (ulViewId, pulDurationTime );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetDurationTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	HxLOG_Debug("[view:%d, duration:%d]\n", ulViewId, *pulDurationTime);
	return ERR_OK;
}
#endif

/********************************************************************************
 * APK_MEDIA_PLAY_GetDurationTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetBitrate (HUINT32 ulViewId, HUINT32 *pulBitrate)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulBitrate )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetBitrate (ulViewId, pulBitrate );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetBitrate err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	HxLOG_Debug("[view:%d, bitrate:%d]\n", ulViewId, *pulBitrate);
	return ERR_OK;
}


/********************************************************************************
 * APK_MEDIA_PLAY_GetSupportedSpeeds
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetSupportedSpeeds (HUINT32 ulViewId, HUINT32 *pulNumSpeed, HUINT32 ulMaxNum, HINT32 *pnSpeeds)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pnSpeeds )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetSupportedSpeeds (ulViewId, pulNumSpeed, ulMaxNum, pnSpeeds );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSupportedSpeeds err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetTrickRestrictMode
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetTrickRestrictMode (HUINT32 ulViewId, HUINT32 *pulTrickRestrictMode)
{
	HERROR hErr;
	DxCopyrightControl_TrickRestrictMode_u uTrickRestrictMode;

	HxLOG_Trace();

	if( NULL == pulTrickRestrictMode )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetTrickRestrictMode (ulViewId, &uTrickRestrictMode );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSupportedSpeeds err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	HxSTD_MemCopy( pulTrickRestrictMode, &uTrickRestrictMode, sizeof(DxCopyrightControl_TrickRestrictMode_u));


	return ERR_OK;
}


/********************************************************************************
 * APK_MEDIA_PLAY_GetTsrPausedPoints
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetTsrPausedPoints (HUINT32 ulViewId, HUINT32 *pulNum, OxMediaPlay_Bookmark_t **ppstBookmarks)
{
	HERROR	hErr;
	HUINT32 ulNum;
	OxMediaPlay_Bookmark_t *pstBookmarks = NULL;

	HxLOG_Trace();

	if( NULL == pulNum || NULL == ppstBookmarks )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetTsrPausedPoints (ulViewId,&ulNum, &pstBookmarks );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetTsrPausedPoints err: ViewId=%d\n", ulViewId);
		*pulNum = 0;
		*ppstBookmarks = NULL;
		return ERR_FAIL;
	}

	*pulNum = ulNum;
	*ppstBookmarks = pstBookmarks;

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetTsrPaused
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetTsrPaused (HUINT32 ulViewId, HBOOL *pbIsPaused)
{
	HERROR  hErr;
	HBOOL	bIsPaused;

	HxLOG_Trace();

	if( NULL == pbIsPaused )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetTsrPaused (ulViewId, &bIsPaused );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetTsrPaused err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	*pbIsPaused = bIsPaused;

	return ERR_OK;
}


/********************************************************************************
 * APK_MEDIA_PLAY_GetVideoSize
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetVideoSize (HUINT32 ulViewId, HINT32 *pnStartX, HINT32 *pnStartY, HINT32 *pnWidth, HINT32 *pnHeight)
{
	HERROR hErr;
	OxMediaPlay_VideoSize_t stVideoSize;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetVideoSize (ulViewId, &stVideoSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetVideoSize err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	*pnStartX	=	stVideoSize.nStartX;
	*pnStartY	=	stVideoSize.nStartY;
	*pnWidth	=	stVideoSize.nWidth;
	*pnHeight	=	stVideoSize.nHeight;

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetAvailabilityStartTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetAvailabilityStartTime (HUINT32 ulViewId, HUINT32 pnSzBuffer, HUINT8 *pszTimeString)
{
	HERROR hErr;

	HxLOG_Trace();
	hErr = OAPI_MEDIA_PLAY_GetAvailabilityStartTime(ulViewId, pnSzBuffer, pszTimeString);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetAvailabilityStartTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____VIEW_CONTROL_____
/********************************************************************************
 * APK_MEDIA_PLAY_SetVideoSize
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetVideoSize (HUINT32 ulViewId, HINT32 nStartX, HINT32 nStartY, HINT32 nWidth, HINT32 nHeight)
{
	HERROR hErr = ERR_OK;
	OxMediaPlay_VideoSize_t	stVideoSize = {0, };

	HxLOG_Trace();

	HxSTD_MemSet (&stVideoSize, 0x0 ,sizeof(OxMediaPlay_VideoSize_t));
	stVideoSize.nStartX		=	nStartX;
	stVideoSize.nStartY		=	nStartY;
	stVideoSize.nWidth		=	nWidth;
	stVideoSize.nHeight		=	nHeight;

	stVideoSize.nMaxWidth	=	CONFIG_MWC_OSD_HORIZONTAL;
	stVideoSize.nMaxHeight	=	CONFIG_MWC_OSD_VERTICAL;

	stVideoSize.bRoundOff	=	FALSE; /* Default false; only for Netflix requirements */
	stVideoSize.bForceSourceBase =	FALSE; /* Default false; only for Netflix requirements */

#if defined(CONFIG_DEBUG)
{
	OxMediaPlay_MediaType_e eOapiType;
	OAPI_MEDIA_PLAY_GetPlayType (ulViewId, &eOapiType);

	HxLOG_Debug("MediaType(%d)\n", eOapiType);
	HxLOG_Debug("(X:%d,Y:%d,W:%d,H:%d) \n", nStartX, nStartY, nWidth, nHeight);
}
#endif

	hErr = OAPI_MEDIA_PLAY_SetVideoSize (ulViewId, &stVideoSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetVideoSize err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_ResizeVideoWindow
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_ResizeVideoWindow (HUINT32 ulViewId, OxMediaPlay_VideoSize_t *pstVideoSize)  /* only for Netflix */
{
	HERROR hErr = ERR_OK;
	OxMediaPlay_VideoSize_t	stVideoSize = {0, };

	HxLOG_Trace();

	if (pstVideoSize == NULL)
	{
		HxLOG_Error("Error> NULL arg\n");
		return ERR_FAIL;
	}
	HxLOG_Print("++[x:%d, y:%d, w:%d, h:%d, roundoff:%d, source:%d] -- ref[w:%d, h:%d]\n",
				pstVideoSize->nStartX, pstVideoSize->nStartY,
				pstVideoSize->nWidth, pstVideoSize->nHeight,
				pstVideoSize->bRoundOff, pstVideoSize->bForceSourceBase,
				pstVideoSize->nMaxWidth, pstVideoSize->nMaxHeight);

	HxSTD_MemSet (&stVideoSize, 0x0 ,sizeof(OxMediaPlay_VideoSize_t));
	/*
	 * [Netflix Video Resize Alignment requirements]
	 *   https://nrd.netflix.com/display/DOC41/Video+Resizing+Requirements
	 *   When aligning the resized video surface to the requested window size,
	 *   it is important to ensure that there is no gap between the resized video and the requested size.
	 *
	 * [Netflix UI over Video requirements]
	 *   https://nrd.netflix.com/display/DOC42/UI+Video
	 *   It must be at least as large as the requested size (both horizontally and vertically).
	 *   If it is larger than the requested size,
	 *   it must be as close as possible to the requested size in order to minimize clipping.
	 */
	stVideoSize.bRoundOff	=	pstVideoSize->bRoundOff;
	/*
	 * Under DASH streaming,
	 * in order to prevent clipBase/clipRect calcalation error by momentary (DASH) source-stream change
	 */
	stVideoSize.bForceSourceBase	=	pstVideoSize->bForceSourceBase;
	stVideoSize.nStartX		=	pstVideoSize->nStartX;
	stVideoSize.nStartY		=	pstVideoSize->nStartY;
	stVideoSize.nWidth		=	pstVideoSize->nWidth;
	stVideoSize.nHeight		=	pstVideoSize->nHeight;

	if (pstVideoSize->nMaxWidth) /* for Netflix NTS VIDEO-003-TC2 (Verify 2D Video Resize Before Starting Playback - 1080p) */
	{
		stVideoSize.nMaxWidth = pstVideoSize->nMaxWidth;
	}
	else
	{
		stVideoSize.nMaxWidth = CONFIG_MWC_OSD_HORIZONTAL;
	}

	if (pstVideoSize->nMaxHeight) /* for Netflix NTS VIDEO-003-TC2 (Verify 2D Video Resize Before Starting Playback - 1080p) */
	{
		stVideoSize.nMaxHeight = pstVideoSize->nMaxHeight;
	}
	else
	{
		stVideoSize.nMaxHeight = CONFIG_MWC_OSD_VERTICAL;
	}

	HxLOG_Print("[x:%d, y:%d, w:%d, h:%d, roundoff:%d, source:%d] -- ref[w:%d, h:%d]\n",
				stVideoSize.nStartX, stVideoSize.nStartY,
				stVideoSize.nWidth, stVideoSize.nHeight,
				stVideoSize.bRoundOff, stVideoSize.bForceSourceBase,
				stVideoSize.nMaxWidth, stVideoSize.nMaxHeight);

#if defined(CONFIG_DEBUG)
{
	OxMediaPlay_MediaType_e eOapiType;
	OAPI_MEDIA_PLAY_GetPlayType (ulViewId, &eOapiType);

	HxLOG_Debug("MediaType(%d)\n", eOapiType);
	HxLOG_Debug("(X:%d,Y:%d,W:%d,H:%d) \n", stVideoSize.nStartX, stVideoSize.nStartY, stVideoSize.nWidth, stVideoSize.nHeight);
}
#endif

	hErr = OAPI_MEDIA_PLAY_SetVideoSize (ulViewId, &stVideoSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error> OAPI_MEDIA_PLAY_SetVideoSize err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_ResetVideoSize
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_ResetVideoSize (HUINT32 ulViewId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_ResetVideoSize (ulViewId );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_ResetVideoSize err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetPigRect
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetPigRect (HUINT32 ulViewId, HUINT32 ulZOrder, HINT32 nInRect_StartX,
																	HINT32 nInRect_StartY,
																	HINT32 nInRect_Width,
																	HINT32 nInRect_Height,
																	HINT32 nInRect_RefWidth,
																	HINT32 nInRect_RefHeight,
																	HINT32 nOutRect_StartX,
																	HINT32 nOutRect_StartY,
																	HINT32 nOutRect_Width,
																	HINT32 nOutRect_Height,
																	HINT32 nOutRect_RefWidth,
																	HINT32 nOutRect_RefHeight,
									DxAspectRatio_e eOsdAspectRatio,
									DxDfcSelect_e	eDfc,
									DxAspectRatio_e ePigAspectRatio)
{
	HERROR hErr;
	OxMediaPlay_PigSize_t	stPigSize;

	HxLOG_Trace();

	stPigSize.ulZOrder				=	ulZOrder;

	stPigSize.nInRect_StartX		=	nInRect_StartX;
	stPigSize.nInRect_StartY		=	nInRect_StartY;
	stPigSize.nInRect_Width			=	nInRect_Width;
	stPigSize.nInRect_Height		=	nInRect_Height;
	stPigSize.nInRect_RefWidth		=	nInRect_RefWidth;
	stPigSize.nInRect_RefHeight		=	nInRect_RefHeight;
	stPigSize.nOutRect_StartX		=	nOutRect_StartX;
	stPigSize.nOutRect_StartY		=	nOutRect_StartY;
	stPigSize.nOutRect_Width		=	nOutRect_Width;
	stPigSize.nOutRect_Height		=	nOutRect_Height;
	stPigSize.nOutRect_RefWidth		=	nOutRect_RefWidth;
	stPigSize.nOutRect_RefHeight	=	nOutRect_RefHeight;

	stPigSize.eOsdAspectRatio	=	eOsdAspectRatio;
	stPigSize.eDfc				=	eDfc;
	stPigSize.ePigAspectRatio	=	ePigAspectRatio;

	hErr = OAPI_MEDIA_PLAY_SetPigRect ( ulViewId, &stPigSize);

	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_SetPigRect err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____FUNC_PROPERTY_____
/********************************************************************************
 * APK_MEDIA_PLAY_SetVideoHide
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetVideoHide (HUINT32 ulViewId, HBOOL bHide)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetVideoHideEnable (ulViewId, bHide );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetEnable err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}




/********************************************************************************
 * APK_MEDIA_PLAY_SetSubtitleEnable
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetSubtitleEnable(HUINT32 ulViewId, HBOOL bEnable)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetSubtitleEnable (ulViewId, bEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetSubtitleEnable err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_Init
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetTSREnable (HBOOL bEnable)
{
	HERROR hErr;
	HUINT32 ulViewId;

	HxLOG_Trace();

	hErr = APK_MEDIA_PLAY_GetMainViewId( &ulViewId );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetMainViewId err: \n");
		return ERR_FAIL;
	}

	hErr = OAPI_MEDIA_PLAY_SetTSREnable (ulViewId, bEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetTSREnable err: \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetSubtitleEnable
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetSubtitleEnable(HUINT32 ulViewId, HBOOL *pbEnable)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetSubtitleEnable (ulViewId, pbEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSubtitleEnable err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetTSREnable
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetTSREnable(HBOOL *pbEnable)
{
	HERROR hErr;
	HUINT32 ulViewId;

	HxLOG_Trace();

	hErr = APK_MEDIA_PLAY_GetMainViewId( &ulViewId );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetMainViewId err: \n");
		return ERR_FAIL;
	}

	hErr = OAPI_MEDIA_PLAY_GetTSREnable (ulViewId, pbEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetTSREnable err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


#define _____ETC_____
/********************************************************************************
 * APK_MEDIA_PLAY_GetEventInfo
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetEventInfo (HUINT32 ulViewId, DxEvent_t *pstEventInfo)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetEventInfo (ulViewId, pstEventInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_EventInfo err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SaveThumbnail
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SaveThumbnail (HUINT32 ulViewId, HCHAR *pszFilename, HUINT32 ulWidth, HUINT32 ulHeight)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SaveThumbnail (ulViewId, pszFilename, ulWidth, ulHeight);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SaveThumbnail err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____LIVE_ONLY_____
/********************************************************************************
 * APK_MEDIA_PLAY_GetMajorChannel
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetMajorChannel (HUINT32 ulViewId, HUINT32 * pulMajorCHNum)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == pulMajorCHNum )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetMajorChannel (ulViewId, pulMajorCHNum );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetMajorChannel err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}


/********************************************************************************
 * APK_MEDIA_PLAY_GetViewState
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetViewState (HUINT32 ulViewId, OxMediaPlay_LiveViewState_e *peViewState)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == peViewState )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetViewState (ulViewId, peViewState );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetViewState err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetLockState
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetLockState (HUINT32 ulViewId, OxMediaPlay_LiveLockState_e *peLockState)
{
	HERROR hErr;

	HxLOG_Trace();

	if( NULL == peLockState )
		return ERR_FAIL;

	hErr = OAPI_MEDIA_PLAY_GetLockState (ulViewId, peLockState );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetLockState err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_MemorizeMediaState (HUINT32 ulViewId, HBOOL bStopMedia)
{
#if 0
	HINT32						 nSvcUid;
	_apkMctrlViewContext_t		*pstContext = _apk_mctrlview_findContext (eDest);

	HxLOG_Trace();

	APPKIT_ENTER;

	if (NULL == pstContext)						{ APPKIT_LEAVE(ERR_FAIL); }

	if (FALSE != pstContext->stBookmark.bBookmarked)
	{
		HxLOG_Error ("[%s:%d] Already memorized\n", __FUNCTION__, __LINE__);
		APPKIT_LEAVE(ERR_OK);
	}

	// 원래는 현재 MediaType에 따라 동작이 다 달라야 하지만 여기서는 Live만 체크하기로 함.
	nSvcUid = APK_MEDIACTRL_LIVE_GetCurrentMasterServiceUid (eDest);
	if (nSvcUid <= 0)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		APPKIT_LEAVE(ERR_FAIL);
	}

	pstContext->stBookmark.bBookmarked		= TRUE;
	pstContext->stBookmark.eMediaType		= eMctrlBase_Live;
	pstContext->stBookmark.unData.stLiveBookmark.ulSvcUid = (HUINT32)nSvcUid;

	if (TRUE == bStopMedia)
	{
		APK_MEDIACTRL_LIVE_Stop (eDest);
	}

	APPKIT_LEAVE(ERR_OK);
#endif
	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_RestoreMediaState(HUINT32 ulViewId)
{

	HxLOG_Trace();
#if 0

	_apkMctrlViewContext_t		*pstContext = _apk_mctrlview_findContext (eDest);
	HERROR						 hErr;

	APPKIT_ENTER;

	if (NULL == pstContext)						{ APPKIT_LEAVE(ERR_FAIL); }

	if (TRUE != pstContext->stBookmark.bBookmarked)
	{
		HxLOG_Error ("[%s:%d] not memorized\n", __FUNCTION__, __LINE__);
		APPKIT_LEAVE(ERR_OK);
	}

	// 원래는 현재 MediaType에 따라 동작이 다 달라야 하지만 여기서는 Live만 체크하기로 함.
	switch (pstContext->stBookmark.eMediaType)
	{
	case eMctrlBase_Live:
		{
			DxService_t			 stService;

			stService.uid = pstContext->stBookmark.unData.stLiveBookmark.ulSvcUid;
			hErr = APK_MEDIACTRL_LIVE_Start (eDest, &stService);
		}
		break;

	default:
		break;
	}

	HxSTD_MemSet (&(pstContext->stBookmark), 0 ,sizeof(_apkMctrlBookmark_t));
	APPKIT_LEAVE(ERR_OK);
#endif
	return ERR_OK;
}

HCHAR *APK_MEDIA_PLAY_GetThumbnail (HUINT32 ulViewId, HUINT32 ulPos, HUINT32 *pulCaptureTime)
{

	HxLOG_Trace();
#if 0
	APK_HANDLE				 hApkHandle;
	APKS_MEDIADEC_PB_t		*pstPb;
	HUINT32					 ulAccessCode;
	HERROR					 hErr;

	hErr = _apk_mctrltsrpb_getDataFromDest (eDest, &hApkHandle, &pstPb);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	pstPb->stThumbnail.szFilename[0] = 0;
	pstPb->stThumbnail.ulTime = ulCaptureTime;
	hErr = APKI_MEDIACTRL_Cmd (hApkHandle, eMEDIADEC_ExtractThumbnail);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	ulAccessCode = APKI_MEDIACTRL_Query (hApkHandle, eMEDIADEC_GetThumbnail);
	if (0 == ulAccessCode)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	return &pstPb->stThumbnail;
#endif
	return NULL;
}


/********************************************************************************
 * APK_MEDIA_PLAY_CheckPlayChangable
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_CheckPlayChangable (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_StartInfo_t *punStart, HBOOL *pbChangable)
{
	HERROR hErr;

	HxLOG_Trace();

	if( (NULL == punStart) || (NULL == pbChangable) )
		return ERR_FAIL;

	switch( eType )
	{
		case eOxMP_MEDIATYPE_LIVE:
		{
			OxMediaPlay_CheckTune_e eResult	=	eOxMP_CHECKTUNE_NoProblem;
			OxMediaPlay_ConflictInfo_t stConflictInfo	=	{0,};

			hErr = OAPI_MEDIA_PLAY_GetConflictInfo (ulViewId, eType, &stConflictInfo);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("OAPI_MEDIA_PLAY_GetConflictInfo err: ViewId=%d\n", ulViewId);
				return ERR_FAIL;
			}

			hErr = apk_media_play_checkServiceTunable ( &stConflictInfo, punStart->stLive.ulMasterSvcUid, &eResult);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("apk_media_play_checkServiceTunable err: ViewId=%d\n", ulViewId);
				return ERR_FAIL;
			}

			*pbChangable	=	(eOxMP_CHECKTUNE_NoProblem == eResult)?TRUE:FALSE;

			break;
		}
		default:
			return ERR_FAIL;
	}

	return hErr;
}

HERROR APK_MEDIA_PLAY_CountAitSection (HUINT32 ulViewId, HUINT32 *pulAitNum)
{
	if (NULL == pulAitNum)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_CountAitSection(ulViewId, pulAitNum);
}

HERROR APK_MEDIA_PLAY_GetAitPid (HUINT32 ulViewId, HUINT16 *pusPrevAitPid, HUINT16 *pulCurrAitPid)
{
	if (NULL == pusPrevAitPid)	{ return ERR_FAIL; }
	if (NULL == pulCurrAitPid)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_GetAitPid(ulViewId, pusPrevAitPid, pulCurrAitPid);
}

HERROR APK_MEDIA_PLAY_GetAitSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucAitSec, HUINT32 *ulSecLen)
{
	if (NULL == ppucAitSec)	{ return ERR_FAIL; }
	if (NULL == ulSecLen)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_GetAitSection(ulViewId, ulIndex, ppucAitSec, ulSecLen);
}

HERROR APK_MEDIA_PLAY_FreeAitSection (HUINT8 *pucAitSec)
{
	if (NULL == pucAitSec)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_FreeAitSection(pucAitSec);
}

HERROR APK_MEDIA_PLAY_GetRctInfoCount (HUINT32 ulViewId, HUINT32 *pulTotalLinkCount , HUINT32 *pulImgDataByteSize)
{
	if (NULL == pulTotalLinkCount)	{ return ERR_FAIL; }
	if(NULL == pulImgDataByteSize)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_GetRctInfoCount(ulViewId, pulTotalLinkCount, pulImgDataByteSize);
}

HERROR APK_MEDIA_PLAY_GetRctInfo (HUINT32 ulViewId, DxService_RctInfo_t *pstRctInfo)
{
	if (NULL == pstRctInfo)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_GetRctInfo(ulViewId, pstRctInfo);
}

HERROR APK_MEDIA_PLAY_FreeRctInfo (DxService_RctInfo_t *pstRctInfo)
{
	if (NULL == pstRctInfo)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_FreeRctInfo(pstRctInfo);

}

HERROR APK_MEDIA_PLAY_CountMhegPatSection (HUINT32 ulViewId, HUINT32 *pulSecNum)
{
	if (NULL == pulSecNum)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_CountMhegPatSection(ulViewId, pulSecNum);
}

HERROR APK_MEDIA_PLAY_GetMhegPatSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucRawSec, HUINT32 *ulSecLen)
{
	if (NULL == ppucRawSec)	{ return ERR_FAIL; }
	if (NULL == ulSecLen)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_GetMhegPatSection(ulViewId, ulIndex, ppucRawSec, ulSecLen);
}

HERROR APK_MEDIA_PLAY_FreeMhegPatSection (HUINT8 *pucRawSec)
{
	if (NULL == pucRawSec)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_FreeMhegPatSection(pucRawSec);
}

HERROR APK_MEDIA_PLAY_CountMhegPmtSection (HUINT32 ulViewId, HUINT32 *pulSecNum)
{
	if (NULL == pulSecNum)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_CountMhegPmtSection(ulViewId, pulSecNum);
}

HERROR APK_MEDIA_PLAY_GetMhegPmtSection (HUINT32 ulViewId, HUINT32 ulIndex, HUINT8 **ppucRawSec, HUINT32 *ulSecLen)
{
	if (NULL == ppucRawSec)	{ return ERR_FAIL; }
	if (NULL == ulSecLen)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_GetMhegPmtSection(ulViewId, ulIndex, ppucRawSec, ulSecLen);
}

HERROR APK_MEDIA_PLAY_FreeMhegPmtSection (HUINT8 *pucRawSec)
{
	if (NULL == pucRawSec)	{ return ERR_FAIL; }

	return OAPI_MEDIA_PLAY_FreeMhegPmtSection(pucRawSec);
}

HERROR APK_MEDIA_PLAY_SetTempAvPid (HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid)
{
	return OAPI_MEDIA_PLAY_SetTempAvPid(ulViewId, eVideoCodec, usVideoPid, eAudioCodec, usAudioPid, usPcrPid);
}

HERROR APK_MEDIA_PLAY_GetSuppleType(HUINT32 ulViewId, OxMediaPlay_SuppleSvcType_e * peSuppSvcType)
{
	return OAPI_MEDIA_PLAY_GetSuppleType(ulViewId, peSuppSvcType);
}

// Obama에 AIT Section이 존재하는지 확인하고 존재하면 AIT다시 올려주도록 하는 Function

void APK_MEDIA_PLAY_SetAitListenerStatus(HUINT32 ulViewId, HBOOL bEnable)
{
	OAPI_MEDIA_PLAY_SetAitListenerStatus(ulViewId, bEnable);
}

HERROR APK_MEDIA_PLAY_CheckAitSection(HUINT32 ulViewId)
{
	return OAPI_MEDIA_PLAY_CheckAitSection(ulViewId);
}

#define _____MEDIAPB_ONLY_____
/********************************************************************************
 * APK_MEDIA_PLAY_GetURI
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetURI (HUINT32 ulViewId, HCHAR *pszURI)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetURI (ulViewId, pszURI);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetURI err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_Probe
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_Probe (HUINT32 ulViewId, HCHAR *pszURI)
{
	HERROR		hErr;

	HxLOG_Trace();

	if (NULL == pszURI)
	{
		return ERR_FAIL;
	}

	hErr = OAPI_MEDIA_PLAY_Probe (ulViewId, pszURI);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_Probe err:\n");
		return ERR_FAIL;
	}


	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetSubtitleSyncTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 nSubtitle_SyncTime)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_PLAY_SetSubtitleSyncTime (ulViewId, nSubtitle_SyncTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetSubtitleSyncTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetSubtitleFontSize
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetSubtitleFontSize (HUINT32 ulViewId, HUINT32 nSubtitle_FontSize)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_PLAY_SetSubtitleFontSize (ulViewId, nSubtitle_FontSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetSubtitleFontSize err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetSubtitleTextPosition
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 nSubtitle_TextPosition)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_PLAY_SetSubtitleTextPosition (ulViewId, nSubtitle_TextPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetSubtitleTextPosition err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetSubtitleSyncTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetSubtitleSyncTime (HUINT32 ulViewId, HUINT32 *pnSubtitle_SyncTime)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetSubtitleSyncTime (ulViewId, pnSubtitle_SyncTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSubtitleSyncTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetSubtitleFontSize
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetSubtitleFontSize (HUINT32 ulViewId, HUINT32 *pnSubtitle_FontSize)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetSubtitleFontSize (ulViewId, pnSubtitle_FontSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSubtitleFontSize err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetSubtitleTextPosition
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetSubtitleTextPosition (HUINT32 ulViewId, HUINT32 *pnSubtitle_TextPosition)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetSubtitleTextPosition (ulViewId, pnSubtitle_TextPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSubtitleTextPosition err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetSubtitleStatus
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetSubtitleStatus (HUINT32 ulViewId, OxMediaPlay_SubtitleStatus_t *pstStatus)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetSubtitleStatus (ulViewId, pstStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSubtitleStatus err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SeBufferingScenario
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SeBufferingScenario(HUINT32 ulViewId, HBOOL bAuto, HINT32 nThreshold)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetBufferingScenario (ulViewId, bAuto, nThreshold);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetBufferingScenario err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetBufferingChunkInfo
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetBufferingChunkInfo (HUINT32 ulViewId, HUINT32 *nChunks, HUINT32 **ppulStartOffsets, HUINT32 **ppulEndOffsets)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetBufferingChunkInfo (ulViewId, nChunks, ppulStartOffsets, ppulEndOffsets);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetBufferingChunkInfo err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR APK_MEDIA_PLAY_GetSeekableRangeInfo (HUINT32 ulViewId, HUINT32 *nChunks, HUINT64 **ppullSeekableStart, HUINT64 **ppullSeekableEnd)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetSeekableRangeInfo (ulViewId, nChunks, ppullSeekableStart, ppullSeekableEnd);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetSeekableRangeInfo err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetBufferingRemainedTime
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetBufferingRemainedTime (HUINT32 ulViewId, HUINT32 *pulBufferRemaindTime)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetBufferingRemainedTime (ulViewId, pulBufferRemaindTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetBufferingRemainedTime err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetBufferingPercent
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetBufferingPercent (HUINT32 ulViewId, HUINT32 *pulBufferPercent)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetBufferingPercent (ulViewId, pulBufferPercent);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetBufferingPercent err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetBufferState
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetBufferState (HUINT32 ulViewId, OxMediaPlay_BufferState_e *peBufferState)
{
	HERROR hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetBufferState (ulViewId, peBufferState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetBufferState err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetDrmLicenseData
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetDrmLicenseData (HUINT32 ulViewId, const HCHAR *pszData)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetDrmLicenseData (ulViewId, pszData);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetDrmLicenseData err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetBrowserUserAgentData
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetBrowserUserAgentData (HUINT32 ulViewId, HCHAR *pszUAData)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetBrowserUserAgentData (ulViewId, pszUAData);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetDrmLicenseData err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetResourceMap
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetResourceMap (OxMediaPlay_ResourceMap_t *pstResourceMap)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetResourceMap ( pstResourceMap );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetResourceMap err\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetPumpIndex
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetPumpIndex (HUINT32 ulViewId, HUINT32 *pulPumpId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetPumpIndex (ulViewId, pulPumpId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetPumpIndex err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}


/********************************************************************************
 * APK_MEDIA_PLAY_SetDuration
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetDuration (HUINT32 ulViewId, HUINT64 ullDuration)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetDuration ( ulViewId, ullDuration );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetDuration err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetPlayerType
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetPlayerType (HUINT32 ulViewId, OxMediaPlay_PlayerType_e eMediaPlayPlayerType)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetPlayerType ( ulViewId, eMediaPlayPlayerType );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetPlayerType err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_AudioFlush
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_AudioFlush(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_AudioFlush (ulViewId, unTryToChangeAudioTrack);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_AudioFlush err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		return ERR_FAIL;
	}
	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_AudioRestart
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_AudioRestart(HUINT32 ulViewId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_AudioRestart(ulViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_AudioRestart err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		return ERR_FAIL;
	}
	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_AudioCodecSwitch
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_AudioCodecSwitch(HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack, HINT32 nAudioCodec)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_AudioCodecSwitch(ulViewId, unTryToChangeAudioTrack, nAudioCodec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_AudioCodecSwitch err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		return ERR_FAIL;
	}
	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetFade
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetFade(HUINT32 ulViewId, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetFade(ulViewId, unStartVolume, unTargetVolume, nEase, nEaseTimeElapsed, nEaseTransitionFrames);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_SetFade err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		return ERR_FAIL;
	}
	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetAudioDecoderVolume
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetAudioDecoderVolume(HUINT32 ulViewId, HUINT32 *punVolume)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetAudioDecoderVolume(ulViewId, punVolume);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_GetAudioDecoderVolume err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____DRM_PLAYREADY______
/********************************************************************************
 * APK_MEDIA_PLAY_SetPlayReadyInfo
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetPlayReadyInfo (HUINT32 ulViewId, HCHAR *pszLaUrl, HCHAR *pszCustomData, HBOOL bControlFlag)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetPlayReadyInfo ( ulViewId, pszLaUrl, pszCustomData, bControlFlag );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetPlayReadyInfo err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____DRM_VERIMATRIX_____
/********************************************************************************
 * APK_MEDIA_PLAY_GetVmxIdentifier
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetVmxIdentifier (HUINT8 **pIdentifier)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetVmxIdentifier (pIdentifier);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetVmxIdentifier err: \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetVmxStatus
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetVmxStatus (HUINT32 ulViewId, HINT32 *eStatus)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetVmxStatus ( ulViewId, eStatus );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetVmxStatus err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_GetVmxError
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_GetVmxError (HUINT32 ulViewId, HINT32 *nErrorCode)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_GetVmxError ( ulViewId, nErrorCode );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_GetVmxError err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_SetVmxCas
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_SetVmxCas (HUINT32 ulViewId, HCHAR *pszVCASUrl, HCHAR *pszVendorName)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_SetVmxCas ( ulViewId, pszVCASUrl, pszVendorName );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_SetVmxCas err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/********************************************************************************
 * APK_MEDIA_PLAY_UnsetVmxCas
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_UnsetVmxCas (HUINT32 ulViewId)
{
	HERROR hErr;

	HxLOG_Trace();

	hErr = OAPI_MEDIA_PLAY_UnsetVmxCas ( ulViewId );
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_UnsetVmxCas err: ViewId=%d\n", ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}


#define _____BACKGROUND_____
/********************************************************************************
 * APK_MEDIA_PLAY_MhegBgStart
 ********************************************************************************/
HERROR APK_MEDIA_PLAY_MhegBgStart (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart, HUINT32 *pulSessionId)
{
	HERROR	hErr;

	hErr = OAPI_MEDIA_PLAY_MhegBgStart (ulViewId, punStart, pulSessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("OAPI_MEDIA_PLAY_Start err: ulViewId(%d)\n", ulViewId);
		return ERR_FAIL;
	}

	return hErr;
}

// @END_AUTOGEN_FUNCTION

/* end of file */
