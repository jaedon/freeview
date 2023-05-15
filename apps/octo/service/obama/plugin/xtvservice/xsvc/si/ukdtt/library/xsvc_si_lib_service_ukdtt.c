/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_service_ukdtt.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIBRARY		\n

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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <tapi.h>
#include <hapi.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_output.h>

#include <db_param.h>

#include <svc_si.h>

#include <_xsvc_si_lib.h>
#include <_xsvc_si_lib_service_ukdtt.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define UKDTT_SBTL_PRIORITY_NONE					0x00000000
#define UKDTT_SBTL_PRIORITY_EBU					0x00000001
#define UKDTT_SBTL_PRIORITY_DVB					0x00000002
#define UKDTT_SBTL_PRIORITY_HOH_O_ON				0x00000004		// Hard of hearing Menu: On         && PMT: On
#define UKDTT_SBTL_PRIORITY_LANG					0x00000008
#define UKDTT_SBTL_PRIORITY_SBTL_LANG			0x00000010
#define UKDTT_SBTL_PRIORITY_USER					0x00000020		/** < Opt select */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

STATIC HBOOL xsvc_si_LibSvcEsIsAudioTrackDolby_Ukdtt (svcSi_AudioEs_t *pstAudioEs)
{
	if ((eDxAUDIO_CODEC_DOLBY_AC3 == pstAudioEs->eAudCodec) ||
		(eDxAUDIO_CODEC_DOLBY_AC3P == pstAudioEs->eAudCodec))
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL xsvc_si_LibSvcEsCheckAudioTrackLanguage_Ukdtt (svcSi_AudioEs_t *pstAudioEs, HBOOL *pbLang1, HBOOL *pbLang2)
{
	HBOOL			 bLang1 = FALSE, bLang2 = FALSE;
	HxLANG_Id_e	 eAudioLangId		= eLangID_MAX;
	HxLANG_Id_e	 eLangId1			= eLangID_MAX;
	HxLANG_Id_e	 eLangId2			= eLangID_MAX;
	HERROR			 hErr;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0) Error!!\n");
	}

	// Check the language number:
	hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioEs->astIsoLangInfo[0].szIso639LangCode, &eLangId1);
	if ( hErr != ERR_OK )
	{
		eLangId1 = eLangID_MAX;
	}

	hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioEs->astIsoLangInfo[1].szIso639LangCode, &eLangId2);
	if ( hErr != ERR_OK )
	{
		eLangId2 = eLangID_MAX;
	}

	bLang1 = (eLangId1 == eAudioLangId) ? TRUE : FALSE;
	bLang2 = (eLangId2 == eAudioLangId) ? TRUE : FALSE;

	if (NULL != pbLang1)					{ *pbLang1 = bLang1; }
	if (NULL != pbLang2)					{ *pbLang2 = bLang2; }

	return ((TRUE == bLang1) || (TRUE == bLang2)) ? TRUE : FALSE;
}


#define ___MEMBER_FUNCTIONS___
/****************************************************************************
Description	:	(1) HD 또는 SD 비디오가 존재하는 경우 HD를 지원하면 HD 비디오를 선택한다.
				You have to sync code with local_MHEG_GetAudioPriority() and local_MHEG_GetAudioPriority().
				If you are not sync these code, your DTG test suite testing will be fail.
Parameters	:
Output		:
Return		:
****************************************************************************/
HERROR xsvc_si_LibSvcEsSelectVideo_Ukdtt (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedVideo)
{
	HINT32			 nCount;
	HUINT32			 ulCurrPriority;
	HUINT32			 ulLastPriority;
	HxList_t		*pstItem;						/** < svcSi_VideoEs_t */

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedVideo))
	{
		return ERR_FAIL;
	}

	pstSelectedVideo->bFound = FALSE;
	pstSelectedVideo->nEsIdx = -1;

	ulLastPriority = SVCSI_VIDEO_PRIORITY_NONE;

	for (pstItem = pstPmtEsInfo->pstVideoTrack, nCount = 0; NULL != pstItem; pstItem = pstItem->next, nCount++)
	{
		svcSi_VideoEs_t			*pstVideoEs = (svcSi_VideoEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL != pstVideoEs) && (PID_NULL != pstVideoEs->usPid))
		{
			ulCurrPriority = SVCSI_VIDEO_PRIORITY_NORMAL;
			if (pstVideoEs->bIsHd)
			{
				ulCurrPriority = SVCSI_VIDEO_PRIORITY_HD;
			}

			if (ulCurrPriority > ulLastPriority)
			{
				pstSelectedVideo->bFound	= TRUE;
				pstSelectedVideo->nEsIdx	= nCount;
				ulLastPriority				= ulCurrPriority;
			}
		}
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsSelectAudio_Ukdtt (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedAudio_t *pstSelectedAudio)
{
	HBOOL					 bUserFound = FALSE;
	HINT32					 nAudioCnt;
	HINT32					 nSameLangNum = 0;
	HINT32					 nMpegIndex = -1, nDolbyIndex = -1, nAdIndex = -1;
//	HBOOL					 bChange = FALSE;
	HBOOL					 bAdOn = FALSE;
	DxStereoSelect_e		 eSoundMode = eDxSTEREO_SELECT_UNKNOWN;
	HxLANG_Id_e			 	 eSelectedLangId = eLangID_MAX;
	svcSi_AudioEs_t			*pstAudioEs, *pstSelectedEs = NULL;
	HxList_t				*pstItem;
	HERROR					 hErr;

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedAudio))
	{
		return ERR_FAIL;
	}

	pstSelectedAudio->bFound		= FALSE;
	pstSelectedAudio->nMpegAudioIdx	= -1;
	pstSelectedAudio->nDolbyIdx		= -1;
	pstSelectedAudio->nAdIdx		= -1;
	pstSelectedAudio->eSoundMode	= eDxSTEREO_SELECT_UNKNOWN;


	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&bAdOn, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION) Error!!\n");
	}

	for (pstItem = pstPmtEsInfo->pstAudioTrack, nAudioCnt = 0; NULL != pstItem; pstItem = pstItem->next, nAudioCnt++)
	{
		pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (pstItem);
		if ((NULL == pstAudioEs) || (PID_NULL == pstAudioEs->usPid))		{ continue; }

		if (TRUE == DbSvc_GetAudUserSelected(pstSvcInfo))
		{
			if (TRUE == DbSvc_GetDolbyFlag(pstSvcInfo))
			{
				if ((xsvc_si_LibSvcEsIsAudioTrackDolby_Ukdtt (pstAudioEs) == TRUE) &&
					(DbSvc_GetDolbyPid(pstSvcInfo) == pstAudioEs->usPid))
				{
					bUserFound = TRUE;
					break;
				}
			}
			else
			{
				if ((xsvc_si_LibSvcEsIsAudioTrackDolby_Ukdtt (pstAudioEs) != TRUE) &&
					(DbSvc_GetAudioPid(pstSvcInfo) == pstAudioEs->usPid))
				{
					bUserFound = TRUE;
					break;
				}
			}
		}

		if (xsvc_si_LibSvcEsCheckAudioTrackLanguage_Ukdtt (pstAudioEs, NULL, NULL) == TRUE)
		{
			nSameLangNum ++;
		}
	}

	if ((TRUE == bUserFound) && (NULL != pstItem))				// User Selected 된 것이 발견 : 그대로
	{
		pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (pstItem);

		if (TRUE == DbSvc_GetDolbyFlag(pstSvcInfo))
		{
			nDolbyIndex = nAudioCnt;
		}
		else
		{
			nMpegIndex = nAudioCnt;
		}

		eSelectedLangId = eLangID_MAX;
		hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioEs->astIsoLangInfo[0].szIso639LangCode, &eSelectedLangId);
		if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
		{
			hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioEs->astIsoLangInfo[1].szIso639LangCode, &eSelectedLangId);
			if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
			{
				eSelectedLangId = eLangID_MAX;
			}
		}

		eSoundMode = DbSvc_GetStereoSelect(pstSvcInfo);
	}
	else
	{
//		HBOOL						 bAudioSelected = FALSE;
		HBOOL						 bDolby = FALSE, bMpeg = FALSE, bTrackDolby = FALSE;
		HBOOL						 bDolbySelected = FALSE;
		HINT32						 nSelectedAudio;
		HINT32						 nSelectedMode = eDxSTEREO_SELECT_STEREO;
		DxDigitalAudioFormat_e			 eHdmiAudioOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
		DxDigitalAudioFormat_e			 eSpdifAudioOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;

		PAL_OUT_GetHdmiAudioFormat (&eHdmiAudioOut);
		PAL_OUT_GetSpdifAudioFormat (&eSpdifAudioOut);

		if ((eDxDIGITALAUDIO_FORMAT_MULTICHANNEL == eHdmiAudioOut) || (eDxDIGITALAUDIO_FORMAT_MULTICHANNEL == eSpdifAudioOut))
		{
			bDolby = TRUE;
		}
		else if ((eDxDIGITALAUDIO_FORMAT_PCM == eHdmiAudioOut) && (eDxDIGITALAUDIO_FORMAT_PCM == eSpdifAudioOut))
		{
			bMpeg = TRUE;
		}

		nSelectedAudio = -1;
		pstSelectedEs = NULL;

		for (pstItem = pstPmtEsInfo->pstAudioTrack, nAudioCnt = 0; NULL != pstItem; pstItem = pstItem->next, nAudioCnt++)
		{
			HBOOL			 bLang1 = FALSE, bLang2 = FALSE;

			pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (pstItem);
			if ((NULL == pstAudioEs) || (PID_NULL == pstAudioEs->usPid))		{ continue; }

			if ((nSameLangNum > 0) && (xsvc_si_LibSvcEsCheckAudioTrackLanguage_Ukdtt (pstAudioEs, &bLang1, &bLang2) != TRUE))
			{
				continue;
			}

			bTrackDolby = xsvc_si_LibSvcEsIsAudioTrackDolby_Ukdtt (pstAudioEs);

			if (nSelectedAudio < 0)
			{
				bDolbySelected = bTrackDolby;
				nSelectedAudio = nAudioCnt;
				pstSelectedEs = pstAudioEs;

				if (TRUE != bLang1) 		{ nSelectedMode = eDxSTEREO_SELECT_MONORIGHT; }
				else if (TRUE != bLang2)	{ nSelectedMode = eDxSTEREO_SELECT_MONOLEFT; }
				else						{ nSelectedMode = eDxSTEREO_SELECT_STEREO; }
			}

			if (((TRUE == bDolby) && (TRUE == bTrackDolby)) ||
				((TRUE == bMpeg) && (TRUE != bTrackDolby)))
			{
				bDolbySelected = bTrackDolby;
				nSelectedAudio = nAudioCnt;
				pstSelectedEs = pstAudioEs;

				if (TRUE != bLang1) 		{ nSelectedMode = eDxSTEREO_SELECT_MONORIGHT; }
				else if (TRUE != bLang2)	{ nSelectedMode = eDxSTEREO_SELECT_MONOLEFT; }
				else						{ nSelectedMode = eDxSTEREO_SELECT_STEREO; }

				break;
			}
		}

		if (NULL != pstSelectedEs)
		{
			if (TRUE == bDolbySelected)
			{
				nDolbyIndex = nSelectedAudio;
			}
			else
			{
				nMpegIndex = nSelectedAudio;
			}

			eSelectedLangId = eLangID_MAX;
			hErr = MWC_UTIL_GetLangIDBy639Code (pstSelectedEs->astIsoLangInfo[0].szIso639LangCode, &eSelectedLangId);
			if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
			{
				hErr = MWC_UTIL_GetLangIDBy639Code (pstSelectedEs->astIsoLangInfo[1].szIso639LangCode, &eSelectedLangId);
				if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
				{
					eSelectedLangId = eLangID_MAX;
				}
			}

			eSoundMode = nSelectedMode;
		}
	}

	/* Check AD */
	nAdIndex = -1;

	if (TRUE == bAdOn)
	{
		for (pstItem = pstPmtEsInfo->pstAudioTrack, nAudioCnt = 0; NULL != pstItem; pstItem = pstItem->next, nAudioCnt++)
		{
			HINT32			 nSubCnt;
//			HBOOL			 bLang1 = FALSE, bLang2 = FALSE;

			pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (pstItem);
			if ((NULL == pstAudioEs) || (PID_NULL == pstAudioEs->usPid))		{ continue; }

			/* AD(Audio Description)이 아닌 경우 skip */
			if (pstAudioEs->eSuppAudioEditorial == eAUDIO_TYPE_SUPP_MAIN)
			{
				continue;
			}

			// ISO Language Info Check:
			for (nSubCnt = 0; nSubCnt < 2; nSubCnt++)
			{
				svcSi_Iso639_t			*pstLangInfo = &(pstAudioEs->astIsoLangInfo[nSubCnt]);
				HxLANG_Id_e			 eAdLangId = eLangID_MAX;

				/* AD(Audio Description)이 아닌 경우 skip */
				if (pstLangInfo->eAudType == eAUDIO_TYPE_UNDEFINED)			{ continue; }

				hErr = MWC_UTIL_GetLangIDBy639Code (pstLangInfo->szIso639LangCode, &eAdLangId);
				if ( hErr != ERR_OK )
				{
					eAdLangId = eLangID_MAX;
				}

				if (eAdLangId == eSelectedLangId)
				{
					nAdIndex = nSubCnt;
					break;
				}
			}

			if (nAdIndex >= 0)
			{
				break;
			}
		}
	}

	if ((bUserFound == TRUE)  /* user selection을 찾았거나 기본 설정값에서 바뀐 idx가 있으면 found TRUE */
		|| ((pstSelectedAudio->nMpegAudioIdx != nMpegIndex) || (pstSelectedAudio->nDolbyIdx != nDolbyIndex) || (pstSelectedAudio->nAdIdx != nAdIndex) || (pstSelectedAudio->eSoundMode != eSoundMode)))
	{
		pstSelectedAudio->bFound		= TRUE;
		pstSelectedAudio->nMpegAudioIdx	= nMpegIndex;
		pstSelectedAudio->nDolbyIdx		= nDolbyIndex;
		pstSelectedAudio->nAdIdx		= nAdIndex;
		pstSelectedAudio->eSoundMode	= eSoundMode;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsSelectSubtitle_Ukdtt (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedSubttl)
{
	HINT32			 nCount, nEsCount = -1;
	HINT32			 nSubttlNum;
	HUINT32			 ulLastPriority;
	HxLANG_Id_e	 eSetLangId = eLangID_MAX, eSiLangId = eLangID_MAX;
	HBOOL			 bHardOfHearing;
	HxList_t		*pstItem;						/** < svcSi_VideoEs_t */
	HERROR			 hErr;
	HBOOL		bSubtitleEnable = FALSE;

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedSubttl))
	{
		HxLOG_Error(" Invalid Parameter \n");
		return ERR_FAIL;
	}

	pstSelectedSubttl->bFound = FALSE;
	pstSelectedSubttl->nEsIdx = -1;

	// eDB_PARAM_ITEM_SUBTITLE_SETTING 값이 FALSE이면 처리하지 않는다..
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SUBTITLE_SETTING, (HUINT32 *)&bSubtitleEnable, 0);
	if((hErr != ERR_OK) || bSubtitleEnable == FALSE)
	{
		HxLOG_Error("eDB_PARAM_ITEM_SUBTITLE_SETTING  is FALSE\n");
		return ERR_FAIL;
	}

	// Subtitle Es Num 이 0이 처리하지 않는다..
	nSubttlNum = (HINT32)HLIB_LIST_Length (pstPmtEsInfo->pstSubttlTrack);
	if(nSubttlNum == 0)
	{
		HxLOG_Error("nSubttlNum is Zero\n");
		return ERR_FAIL;
	}

	// 사용자가 선택을 했는데, index가 없으면 처리하지 않는다..
	if ((TRUE == DbSvc_GetSubttlFlag(pstSvcInfo)) && (SvcSi_SUBTITLE_INDEX_NULL == DbSvc_GetSubttlIdx(pstSvcInfo)))
	{
		HxLOG_Error("DbSvc_GetSubttlFlag is TRUE & DbSvc_GetSubttlIdx is SvcSi_SUBTITLE_INDEX_NULL \n");
		return ERR_FAIL;
	}

/*
	if (TRUE == DbSvc_GetSubttlFlag(pstSvcInfo))
	{
		if ((HINT32)DbSvc_GetSubttlIdx(pstSvcInfo) < nSubttlNum)
		{
			pstSelectedSubttl->bFound = TRUE;
			pstSelectedSubttl->nEsIdx = (HINT32)DbSvc_GetSubttlIdx(pstSvcInfo);
		}

		HxLOG_Debug("[%s] Found Subtitle Es Info ( index : %d)  \n", __FUNCTION__, pstSelectedSubttl->nEsIdx);

		return ERR_OK;
	}
*/
	// TODO: Preferred Language로 변경해야 된다..우선은 SubTitleLanguage로 사용한다..
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0, (HUINT32 *)&eSetLangId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0) Error!!(eSetLangId : 0x%08x) \n", eSetLangId);
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING, (HUINT32 *)&bHardOfHearing, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING) Error!!(bHardOfHearing : %d)\n", bHardOfHearing);
	}

	HxLOG_Debug("[%s] SubTitle LangId (0x%08x)	bHardOfHearing(%d) \n", __FUNCTION__, eSetLangId, bHardOfHearing);

	ulLastPriority = UKDTT_SBTL_PRIORITY_NONE;

	for (pstItem = pstPmtEsInfo->pstSubttlTrack, nCount = 0; NULL != pstItem; pstItem = pstItem->next, nCount++)
	{
		HUINT32 		 		 ulCurrPriority = 0;
		svcSi_SubtitleEs_t		*pstSbtlEs = (svcSi_SubtitleEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL == pstSbtlEs) || (PID_NULL == pstSbtlEs->usPid))
		{
			//HxLOG_Error ("No subtitle es data : count(%d) \n", nCount);
			continue;
		}

		if (DbSvc_GetSubttlFlag(pstSvcInfo) && (nCount == (HINT32)DbSvc_GetSubttlIdx(pstSvcInfo)))
		{
			pstSelectedSubttl->bFound = TRUE;
			pstSelectedSubttl->nEsIdx = (HINT32)nCount;

			HxLOG_Debug("[%s] Found Subtitle Es Info by selecting of user( index : %d)	\n", __FUNCTION__, pstSelectedSubttl->nEsIdx);

			return ERR_OK;
		}

		pstSbtlEs->aucLangCode[3] = '\0';
		HLIB_STD_StrLower (pstSbtlEs->aucLangCode);

		eSiLangId = HLIB_LANG_639ToId (pstSbtlEs->aucLangCode);

#if 0
		if (eSiLangId == eSetLangId)
		{
			ulCurrPriority |= UKDTT_SBTL_PRIORITY_SBTL_LANG;
		}
		else
		{
			if(eSiLangId == eLangID_Undefined)
			{
				ulCurrPriority |= UKDTT_SBTL_PRIORITY_SBTL_LANG;
			}
			else
			{
				if(eSiLangId == eLangID_OriginalAudio)
				{
					ulCurrPriority |= UKDTT_SBTL_PRIORITY_SBTL_LANG;
				}
				else
				{
					if(eSiLangId == eLangID_English)
					{

					}
				}
			}
		}
#endif
		if (ulCurrPriority > ulLastPriority)
		{
			ulLastPriority = ulCurrPriority;
			nEsCount = nCount;
		}
	}

	if (ulLastPriority >= UKDTT_SBTL_PRIORITY_LANG)
	{
		pstSelectedSubttl->bFound	= TRUE;
		pstSelectedSubttl->nEsIdx	= nEsCount;
	}

	return ERR_OK;
}


HERROR xsvc_si_LibSvcEsUpdateTimeByTdt_Ukdtt (HUINT32 ulActionId, SIxTable_PsiTable_t *pstTdtTable)
{
	HBOOL					 bUpdate = FALSE;
	UNIXTIME				 utCurrTime = 0, utSiTime;
	HERROR					 hErr;

	if (NULL == pstTdtTable)					{ return ERR_FAIL; }

	utSiTime = pstTdtTable->unInfo.stTdt.utUtcTime;

	hErr = VK_CLOCK_GetTime ((unsigned long *)&utCurrTime);
	if ((VK_OK == hErr) && (utCurrTime > (60 * 60 * 24 * 365)))
	{
		HUINT32			 ulDiff = (utSiTime > utCurrTime) ? (utSiTime - utCurrTime) : (utCurrTime - utSiTime);

		if (ulDiff > 30)
		{
			bUpdate = TRUE;
		}
	}
	else
	{
		bUpdate = TRUE;
	}

	if (TRUE != bUpdate)
	{
		return ERR_FAIL;
	}

	if (TAPI_SetTime(TxSOURCE_SET_BY_SI, utSiTime) != ERR_OK)
		HxLOG_Error("[TAPI_SetTime is Failed - time %u]\n", utSiTime);

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsUpdateTimeByTot_Ukdtt (HUINT32 ulActionId)
{
	SIxTable_PsiTable_t *pstTotTable = NULL;
	UNIXTIME			 utSiTime;
	HERROR				 hErr;
	HxList_t			*pstDesLoop = NULL;
	SIxTable_PsiTable_t	*pstCurrTot = NULL;
	SIxLocalTimeDes_t	*pstLocalTimeDesc = NULL;
	HxCountry_e 		 eCountryId;
	HCHAR				*pucIso3166Code;
	HINT32				 nTimeRegion;
	HBOOL				 bOffsetFound = FALSE, bTimeFound = FALSE;
	HUINT8				 aucBcdTime[3];
	HINT32				 nOffsetHour;
	HINT32				 nLocalTimeOffset = 0, nNextLocalTimeOffset=0;	// minutes
	UNIXTIME			 ulTimeOfChange = 0, ulTempTime = 0;
	HxDATETIME_Time_t	 stDatetime;
	HCHAR				 szTzDstString[MAX_TIMEZONE_STR_LEN];

	// Country
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if ((ERR_OK != hErr) || (FALSE == HLIB_COUNTRY_IsValidCountryId(eCountryId)))
	{
		HxLOG_Error("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) :0x%X (-) \n", eCountryId);
		return ERR_FAIL;
	}

	pucIso3166Code = (HCHAR *)HLIB_COUNTRY_IdTo3166_Alpha3(eCountryId);

	// Region
	hErr = HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID, &nTimeRegion);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID) Error!!\n");
		nTimeRegion = 0;
	}

	// TOT table
	pstTotTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Tot, SILIB_TABLE_EXT_ID_NOT);
	if (NULL == pstTotTable)
	{
		HxLOG_Error("PAL_SIRAWPOOL_ParseTable() failed..!!  \n");
		return ERR_FAIL;
	}

	for (pstCurrTot = pstTotTable; pstCurrTot != NULL; pstCurrTot = pstCurrTot->pstNextTable)
	{
		bTimeFound = TRUE;

		// parse 1st des loop
		for (pstDesLoop = pstCurrTot->pstDesLoop; pstDesLoop != NULL; pstDesLoop = pstDesLoop->next)
		{
			HUINT8		*pucRawDesc = HLIB_LIST_Data (pstDesLoop);

			if (pucRawDesc == NULL)
			{
				HxLOG_Print("pucRawDesc == NULL, continue \n");
				continue;
			}

			if (*pucRawDesc == eSIxDESCTAG_LOCAL_TIME_OFFSET)
			{
				pstLocalTimeDesc = (SIxLocalTimeDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDesc);
				if(pstLocalTimeDesc != NULL)
				{
					SIxLocalTimeInfo_t	*pstLocalTimeInfo = NULL;

					pstLocalTimeInfo = pstLocalTimeDesc->pstInfo;

					for (pstLocalTimeInfo = pstLocalTimeDesc->pstInfo; pstLocalTimeInfo != NULL; pstLocalTimeInfo = pstLocalTimeInfo->pstNext)
					{
						if (HxSTD_StrCmp (pstLocalTimeInfo->szCountryCode, pucIso3166Code) == 0)
						{
							if (nTimeRegion == 0 ||
								pstLocalTimeInfo->ucRegionId == 0 ||
								nTimeRegion == (HINT32)pstLocalTimeInfo->ucRegionId)
							{
								bOffsetFound = TRUE;

								// Time Of Change
								ulTimeOfChange = pstLocalTimeInfo->timeOfChange;

								// Local Time Offset
								HxSTD_memcpy(aucBcdTime, pstLocalTimeInfo->pucLocalTimeOff, 2);
								aucBcdTime[2] = 0;
								HLIB_DATETIME_ConvertBcdTimeToUnixTime(aucBcdTime, &ulTempTime);
								ulTempTime = (ulTempTime / SECONDS_PER_MIN);	// sec -> min
								nLocalTimeOffset = (pstLocalTimeInfo->ucLocalTimeOffPolar) ? -(ulTempTime) : ulTempTime;

								// Next Time Offset
								HxSTD_memcpy(aucBcdTime, pstLocalTimeInfo->pucNextTimeOffset, 2);
								aucBcdTime[2] = 0;
								HLIB_DATETIME_ConvertBcdTimeToUnixTime(aucBcdTime, &ulTempTime);
								ulTempTime = (ulTempTime / SECONDS_PER_MIN);	// sec -> min
								nNextLocalTimeOffset = (pstLocalTimeInfo->ucLocalTimeOffPolar) ? -(ulTempTime) : ulTempTime;

								HLIB_DATETIME_ConvertBcdTimeToDecimalTime(pstLocalTimeInfo->pucLocalTimeOff, 2, &stDatetime);
								HxLOG_Print("\t LocalTimeOff's Conversion -> [%d:%d:%d:%d]\n",
									stDatetime.ucHour,stDatetime.ucMinute,stDatetime.ucSecond,stDatetime.usMillisecond);

								if (pstLocalTimeInfo->ucLocalTimeOffPolar == 1)
								{
									nOffsetHour = -(stDatetime.ucHour);
								}
								else
								{
									nOffsetHour = stDatetime.ucHour;
								}

								// change time 이 current time 보다 early 라면 next time offset 을 적용해야 한다.
								if (pstLocalTimeInfo->timeOfChange < pstCurrTot->unInfo.stTot.utUtcTime)
								{
									HLIB_DATETIME_ConvertBcdTimeToDecimalTime(pstLocalTimeInfo->pucNextTimeOffset, 2, &stDatetime);
									if (pstLocalTimeInfo->ucLocalTimeOffPolar == 1)
									{
										nOffsetHour = -(stDatetime.ucHour);
									}
									else
									{
										nOffsetHour = stDatetime.ucHour;
									}
								}
							}
						}

						if(bOffsetFound == TRUE)
						{
							break;
						}
					}

					if(pstLocalTimeDesc != NULL)
					{
						PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t*)pstLocalTimeDesc);
						pstLocalTimeDesc = NULL;
					}
				}

				if (bOffsetFound == TRUE)
				{
					break;
				}
			}
		}
	}

	if(bOffsetFound == TRUE)
	{
		HCHAR	 szTzFileString[TZ_NAME_LENGTH], szSystemCommand[TZ_NAME_LENGTH + 5];
		FILE	*pfTzFile = fopen(TZ_NAME_PATH, "r");
		HBOOL	 bMakeTzFile = FALSE;



		// BASIC SI Test Suite 3.1.0의 SI04가 다 통과되려면 이부분 적용 하여야 한다. (필요한지 확인 후 적용.)
		// TOT에 변경될 시간을 고려하여 Event Duration을 늘리고 줄이는 기능.
		if (bOffsetFound == TRUE)
		{
//			SI_TIME_SetTimeOffsetParam(ulTimeOfChange, nLocalTimeOffset, nNextLocalTimeOffset);
		}

		if (pfTzFile != NULL)
		{
			/**
			  *
			  *	TZ file 이 존재한다. 방금 만든 tz string이랑 비교해보고 다르면 업데이트 하자
			  *
			**/
			// TZ flie string을 읽고
			fgets(szTzFileString, TZ_NAME_LENGTH, pfTzFile);
			szTzFileString[HxSTD_StrLen(szTzFileString)-1] = '\0';
			fclose(pfTzFile);

			// 현재 tot로 만든 tz string을 읽어서 tz file에서 읽은 것과 비교
			HLIB_DATETIME_MakeTzFileFromTot(pstTotTable->unInfo.stTot.utUtcTime, eCountryId, ulTimeOfChange, nLocalTimeOffset, nNextLocalTimeOffset, szTzDstString);
			if (HxSTD_StrCmp(szTzDstString, szTzFileString) == 0)
			{
				// 이미 존재하는 TZ 와 같다. 더 이상 할 것 없음~
				bMakeTzFile = FALSE;
			}
			else
			{
				// TZ file과 다르네?? 새로 만들어 주자
				bMakeTzFile = TRUE;
			}
		}
		else
		{
			// TZ file이 없으면 안 되는데..
			bMakeTzFile = TRUE;
			HxLOG_Error("NO TZ FILE NOW !!!! \n");
		}

		if (bMakeTzFile == TRUE)
		{
			HxSTD_snprintf(szSystemCommand, 128, "%s%c", szTzDstString, 0x0a);
			HLIB_FILE_WriteFile(TZ_NAME_PATH, szSystemCommand, HxSTD_StrLen(szSystemCommand));
			HLIB_FILE_Delete(TZ_NAME_PATH_TMP);
			HLIB_FILE_Copy(TZ_NAME_PATH, TZ_NAME_PATH_TMP);
		}
	}

	utSiTime = pstTotTable->unInfo.stTot.utUtcTime;

	if (TAPI_SetTime(TxSOURCE_SET_BY_SI, utSiTime) != ERR_OK)
	{
		HxLOG_Error("[TAPI_SetTime is Failed - time %u]\n", utSiTime);
	}

	if (bOffsetFound == TRUE)
	{
		HINT32	nTime;

		//HxLOG_Print("[local_sitime_UpdateTimeAsTimeScheme] New Time Offset [%02d:%02d]\n", pstParam->nOffsetHour, pstParam->nOffsetMin);
		nTime = (nLocalTimeOffset * 60) ;
		DB_PARAM_SetItem(eDB_PARAM_ITEM_GMT_OFFSET_MINUTES, (HUINT32)nTime, (HUINT32)0);
	}

	if (pstTotTable != NULL)
	{
		PAL_SIRAWPOOL_FreeTable (pstTotTable);
	}

	return ERR_OK;
}

STATIC HINT32 xsvc_si_GetAudioLangPriority_UkDtt(HxLANG_Id_e eLangId)
{
	HINT32	nPriority = 0;

	switch (eLangId)
	{
	case eLangID_Undefined: 		nPriority = 50;		break;
	case eLangID_OriginalAudio: 		nPriority = 40;		break;
	case eLangID_English:			nPriority = 30;		break;
	default:
		break;
	}

	HxLOG_Debug("eLangId(0x%08x) nPriority(%d) \n", eLangId, nPriority);

	return nPriority;
}

STATIC HINT32 xsvc_si_GetAudioTypePriority_UkDtt(AUDIO_Type_t eAudType)
{
	HINT32	nPriority = 0;

	switch (eAudType)
	{
		case eAUDIO_TYPE_UNDEFINED:  			nPriority = 20;		break;
		case eAUDIO_TYPE_CLEAN_EFFECTS:		nPriority = 10;		break;
		default:
			break;
	}

	HxLOG_Debug("eAudType(%d) nPriority(%d) \n", eAudType, nPriority);

	return nPriority;
}

HERROR xsvc_si_GetAlternativeAudioPriority_UkDtt(const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect)
{
	HINT32				nPriority;
	HBOOL				bDualMono = FALSE;
	HxLANG_Id_e			eLangId;
	DxDualMonoSelect_e	eDualMonoSelect;

	nPriority = 0;
	eLangId = eLangID_MAX;
	eDualMonoSelect = eDxDUALMONO_SELECT_UNKNOWN;

	HxLOG_Info("bLangMatched (%d) \n", bLangMatched);

	if(bLangMatched == TRUE)
	{
		HxLANG_Id_e 	eLang1, eLang2;
		HINT32	nPriority1=0, nPriority2=0;

		// Get Language Priority
		eLang1 = eLang2 = eLangID_MAX;
		bDualMono = svc_si_LibSvcEsGetLangInfo(pstAudioEs, &eLang1, &eLang2, NULL);

		HxLOG_Info("bDualMono (%d) \n", bDualMono);

		if(bDualMono == TRUE)
		{
			nPriority1 = xsvc_si_GetAudioLangPriority_UkDtt(eLang1);
			nPriority2 = xsvc_si_GetAudioLangPriority_UkDtt(eLang2);

			// Get Audio Type Priority
			nPriority1 += xsvc_si_GetAudioTypePriority_UkDtt(pstAudioEs->astIsoLangInfo[0].eAudType);
			nPriority2 += xsvc_si_GetAudioTypePriority_UkDtt(pstAudioEs->astIsoLangInfo[1].eAudType);

			if (nPriority1 >= nPriority2)
			{
				eDualMonoSelect = eDxDUALMONO_SELECT_LEFT;
				nPriority = nPriority1;
				eLangId = eLang1;
			}
			else
			{
				eDualMonoSelect = eDxDUALMONO_SELECT_RIGHT;
				nPriority = nPriority2;
				eLangId = eLang2;
			}
		}

		HxLOG_Debug("nPriority1(%d) nPriority2(%d) eDualMonoSelect(0x%x) \n", nPriority1, nPriority2, eDualMonoSelect);
	}
	else //(FALSE == bLangMatched)
	{
		HxLANG_Id_e 	eLang1, eLang2;

		// Get Language Priority
		eLang1 = eLang2 = eLangID_MAX;
		bDualMono = svc_si_LibSvcEsGetLangInfo(pstAudioEs, &eLang1, &eLang2, NULL);

		if (FALSE == bDualMono)
		{
			eDualMonoSelect = eDxDUALMONO_SELECT_UNKNOWN;
			nPriority = xsvc_si_GetAudioLangPriority_UkDtt(eLang1);
			eLangId = eLang1;

			// Get Audio Type Priority
			nPriority += xsvc_si_GetAudioTypePriority_UkDtt(pstAudioEs->astIsoLangInfo[0].eAudType);
		}
		else
		{
			HINT32	nPriority1=0, nPriority2=0;

			nPriority1 = xsvc_si_GetAudioLangPriority_UkDtt(eLang1);
			nPriority2 = xsvc_si_GetAudioLangPriority_UkDtt(eLang2);

			// Get Audio Type Priority
			nPriority1 += xsvc_si_GetAudioTypePriority_UkDtt(pstAudioEs->astIsoLangInfo[0].eAudType);
			nPriority2 += xsvc_si_GetAudioTypePriority_UkDtt(pstAudioEs->astIsoLangInfo[1].eAudType);

			if (nPriority1 >= nPriority2)
			{
				eDualMonoSelect = eDxDUALMONO_SELECT_LEFT;
				nPriority = nPriority1;
				eLangId = eLang1;
			}
			else
			{
				eDualMonoSelect = eDxDUALMONO_SELECT_RIGHT;
				nPriority = nPriority2;
				eLangId = eLang2;
			}

			HxLOG_Debug("nPriority1(%d) nPriority2(%d) eDualMonoSelect(0x%x) \n", nPriority1, nPriority2, eDualMonoSelect);

		}
	}

	HxLOG_Debug("bDualMono(%d) nPriority(%d) eLangId(0x%08x) eDualMonoSelect(0x%x) \n", bDualMono, nPriority, eLangId, eDualMonoSelect);

	*pnUserPriority = nPriority;

	if (peLangId)
	{
		*peLangId = eLangId;
	}

	if (peDualMonoSelect)
	{
		*peDualMonoSelect = eDualMonoSelect;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsGetSvcAndVideoTypeFromDvbSvcType_Ukdtt (HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType)
{
	if ((NULL == peSvcType) || (NULL == peVideoType))
	{
		return ERR_FAIL;
	}

	switch (ulDvbSvcType)
	{
	case SERVICE_TYPE_DATA_BROADCAST:
		*peSvcType = eDxSVC_TYPE_TV;
		return ERR_OK;
	default:
		break;
	}

	return ERR_FAIL;

}
/* end of file */

