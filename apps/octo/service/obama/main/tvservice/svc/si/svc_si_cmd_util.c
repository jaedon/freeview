/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  cmd_svc_si.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include <octo_common.h>
#include <svc_si.h>
#include <_svc_si.h>

#include "cutest.h"

#include "_svc_si_cmd.h"

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


#define ___LOCAL_FUNCTIONS___


#define ___GLOBAL_FUNCTIONS___

void svc_si_UtInitSelectedEs(SvcSi_SelectedEs_t *pstSelectedEs)
{
	if (pstSelectedEs)
	{
		HxSTD_MemSet(pstSelectedEs, 0x00, sizeof(SvcSi_SelectedEs_t));
	}
}

HUINT32 svc_si_UtSetTrackIdx(HUINT32 		*aulTrackList,
								HUINT32		 ulListNum,
								HUINT32		 ulMaxTrackIdx,
								va_list		 ap)
{
	HUINT32 ulTrackIndex;
	HUINT32 ulCount, ulNum;

	if (NULL == aulTrackList)
		return 0;

	if (0 == ulListNum)
		return 0;

	ulCount = 0;
    do {
        ulTrackIndex = va_arg(ap, HUINT32);
        if (ulTrackIndex >= 0) {
        	aulTrackList[ulCount++] = ulTrackIndex;
        }
    } while (ulCount < ulListNum && ulTrackIndex < ulMaxTrackIdx);
    ulNum = ulCount;

	HxLOG_Print("[TrackList:(%02d)] : ", ulNum);
    for (ulCount=0; ulCount<ulNum; ulCount++)
    {
    	HxLOG_Print("[%02d]", aulTrackList[ulCount]);
    }
    HxLOG_Print("\n");

    return ulCount;
}

const svcSi_PmtEsInfo_t *svc_si_UtSetEsListToPmtEsInfo(svcSi_PmtEsInfo_t	*pstPmtEsInfo,
														EsType_t			 eEsType,
														const HxList_t		*pstEsList)
{
	svcSi_PmtEsInfo_t	*pstInfo = NULL;
	HBOOL				 bAllocated = FALSE;

	if (NULL == pstEsList)
	{
		return NULL;
	}

	if (NULL == pstPmtEsInfo)
	{
		pstInfo = (svcSi_PmtEsInfo_t *)OxSI_Malloc(sizeof(svcSi_PmtEsInfo_t));
		if (NULL == pstInfo)
		{
			HxLOG_Warning("NULL == pstInfo\n");
			return NULL;
		}
		HxSTD_MemSet(pstInfo, 0x00, sizeof(svcSi_PmtEsInfo_t));
		bAllocated = TRUE;
	}
	else
	{
		pstInfo = pstPmtEsInfo;
	}

	switch (eEsType)
	{
	case eEsType_None:				if (bAllocated)	goto err_exit;						break;
	case eEsType_Video:				pstInfo->pstVideoTrack = (HxList_t *)pstEsList;		break;
	case eEsType_Audio:				pstInfo->pstAudioTrack = (HxList_t *)pstEsList;		break;
	case eEsType_PCR:				if (bAllocated)	goto err_exit;						break;
	case eEsType_Subtitle:			pstInfo->pstSubttlTrack = (HxList_t *)pstEsList;	break;
	case eEsType_Teletext:			pstInfo->pstTeltxtTrack = (HxList_t *)pstEsList;	break;
	case eEsType_ClosedCaption:		pstInfo->pstCaptionTrack = (HxList_t *)pstEsList;	break;
	case eEsType_SuperImpose:		pstInfo->pstCaptionTrack = (HxList_t *)pstEsList;	break;
	case eEsType_Data:				pstInfo->pstBmlTrack = (HxList_t *)pstEsList;		break;
	case eEsType_Reserved:
	case eEsType_Unknown:			if (bAllocated)	goto err_exit;						break;
	default:						if (bAllocated)	goto err_exit;						break;
	}

	return (const svcSi_PmtEsInfo_t *)pstInfo;

err_exit:
	if (pstInfo)
	{
		OxSI_Free(pstInfo);
	}
	return NULL;
}


void svc_si_UtFreeEsInfo(svcSi_PmtEsInfo_t *pstEsInfo)
{
	if (pstEsInfo)
	{
		svc_si_LibFreePmtEsInfo (pstEsInfo);
	}
}

void svc_si_UtFreeConfig(SvcSi_Config_t *pstConfig)
{
	if (pstConfig)
	{
		OxSI_Free((pstConfig));
	}
}

void svc_si_UtVeritySelectEsResult(Cutest_TestCase_t		*pstTc,
									const HCHAR			*pszFuncName,
									const HCHAR			*pszResultType,
									HINT32				 nActualResult,
									HINT32				 nExpectedResult)
{
	HUINT8	szPrintBuf[svcSi_CMD_STR_BUFF_SIZE] = {0, };

	HxLOG_Print("=======================================================\n");
	HxLOG_Print("= [Verify Result (\"%-12s\")] --------------------\n", (NULL == pszResultType) ? "HERROR" : pszResultType);
	HxLOG_Print("= -----------------------------------------------------\n");
	HxLOG_Print("= [Expected][%-20s] : [%d]\n", "nExptectedResult",		nExpectedResult);
	HxLOG_Print("= -----------------------------------------------------\n");
	HxLOG_Print("= [Actual  ][%-20s] : [%d]\n", "nActualResult",		nActualResult);
	HxLOG_Print("=======================================================\n");

	HxSTD_snprintf((char *)szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"Expect %s() return (%x) different value", pszFuncName, nExpectedResult);
	CutestAssertIntEquals_Msg(pstTc, (const char *)szPrintBuf, nExpectedResult, nActualResult);
}

void svc_si_UtVeritySelectAudioOutput(Cutest_TestCase_t		*pstTc,
										HERROR				 hErr,
										SvcSi_SelectedEs_t	*pstSelectedEs,
										HINT32				 nExpectedTrackIdx,
										const HCHAR			*pszErrMsg,
										HUINT32				 nParam1,
										HUINT32				 nParam2,
										HUINT32				 nParam3)
{
	HUINT8	szPrintBuf[svcSi_CMD_STR_BUFF_SIZE] = {0, };

	HxLOG_Print("=======================================================\n");
	HxLOG_Print("= [Verify Audio Info] ---------------------------------\n");
	HxLOG_Print("= -----------------------------------------------------\n");
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Idx",			nExpectedTrackIdx);
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Param1",		nParam1);
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Param2",		nParam2);
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Param3",		nParam3);
	HxLOG_Print("= -----------------------------------------------------\n");
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "Main Idx",		pstSelectedEs->nIndexMain);
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "DualMonoSel",	pstSelectedEs->ulParam1);
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "Result",		hErr);
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "Found",		pstSelectedEs->bFound);
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "AD Idx",		pstSelectedEs->nIndexAux);
	HxLOG_Print("=======================================================\n");

	if (ERR_OK == hErr)
	{
		HxSTD_snprintf((char *)szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"nIndexMain (%d) different value", pstSelectedEs->nIndexMain);
		CutestAssertIntEquals_Msg(pstTc, (const char *)szPrintBuf, nExpectedTrackIdx, pstSelectedEs->nIndexMain);

		HxSTD_snprintf((char *)szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"nIndexAux (%d) different value", pstSelectedEs->nIndexAux);
		CutestAssertIntEquals_Msg(pstTc, (const char *)szPrintBuf, -1, pstSelectedEs->nIndexAux);

		HxSTD_snprintf((char *)szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"Param1 (%d) different value", pstSelectedEs->ulParam1);
		CutestAssertIntEquals_Msg(pstTc, (const char *)szPrintBuf, nParam1, pstSelectedEs->ulParam1);
	}
	else
	{
		HxSTD_snprintf((char *)szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"%s : err[0x%x]", (NULL == pszErrMsg) ? "svc_si_LibSvcEsSelectAudio() fail" : pszErrMsg, hErr);
		CutestFail(pstTc, (const char *)szPrintBuf);
	}
}

void svc_si_UtVeritySelectSubtitleOutput(Cutest_TestCase_t		*pstTc,
											HERROR				 hErr,
											SvcSi_SelectedEs_t	*pstSelectedEs,
											HINT32				 nExpectedTrackIdx,
											const HCHAR			*pszErrMsg,
											HUINT32				 nParam1,
											HUINT32				 nParam2,
											HUINT32				 nParam3)
{
	HUINT8	szPrintBuf[svcSi_CMD_STR_BUFF_SIZE] = {0, };

	HxLOG_Print("=======================================================\n");
	HxLOG_Print("= [Verify Subtitle Info] ------------------------------\n");
	HxLOG_Print("= -----------------------------------------------------\n");
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Idx",			nExpectedTrackIdx);
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Param1",		nParam1);
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Param2",		nParam2);
	HxLOG_Print("= [Expected][%-11s] : [%d]\n", "Param3",		nParam3);
	HxLOG_Print("= -----------------------------------------------------\n");
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "Main Idx",		pstSelectedEs->nIndexMain);
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "Result",		hErr);
	HxLOG_Print("= [Actual  ][%-11s] : [%d]\n", "Found",		pstSelectedEs->bFound);
	HxLOG_Print("=======================================================\n");

	if (ERR_OK == hErr)
	{
		HxSTD_snprintf((char *)szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"nIndexMain (%d) different value", pstSelectedEs->nIndexMain);
		CutestAssertIntEquals_Msg(pstTc, (const char *)szPrintBuf, nExpectedTrackIdx, pstSelectedEs->nIndexMain);
	}
	else
	{
		HxSTD_snprintf((char *)szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"%s : err[0x%x]", (NULL == pszErrMsg) ? "svc_si_LibSvcEsSelectSubtitle() fail" : pszErrMsg, hErr);
		CutestFail(pstTc, (const char *)szPrintBuf);
	}
}

STATIC HCHAR *svc_si_UtPrintAspectRatio(DxAspectRatio_e eAr)
{
	switch (eAr)
	{
	ENUM_TO_STR(eDxASPECTRATIO_UNKNOWN);
	ENUM_TO_STR(eDxASPECTRATIO_4X3);
	ENUM_TO_STR(eDxASPECTRATIO_16X9);
	ENUM_TO_STR(eDxASPECTRATIO_14X9);
	ENUM_TO_STR(eDxASPECTRATIO_1X1);
	ENUM_TO_STR(eDxASPECTRATIO_MAX_NUM);
	default:	break;
	}

	return "";
}

void svc_si_UtPrintConfig(const SvcSi_Config_t	*pstConfig,
							HBOOL				 bAudio,
							HBOOL				 bSubtitle)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Print("================================================================\n");
	HxLOG_Print("= [Config] =====================================================\n");
	HxLOG_Print("================================================================\n");
	HxLOG_Print("= [System        ][%14s] : [%d]\n", "Operator", pstConfig->stSystem.eOperator);
	HxLOG_Print("= [System        ][%14s] : [%d]\n", "Country", pstConfig->stSystem.eCountry);
	HxLOG_Print("= [System        ][%14s] : [%s][0x%08x]\n", "OSD Language", svcSi_UtGetLang(pstConfig->stSystem.eOsdLang), svcSi_UtGetLang(pstConfig->stSystem.eOsdLang));
	HxLOG_Print("= [System        ][%14s] : [%s]\n", "Aspect Ratio", svc_si_UtPrintAspectRatio(pstConfig->stSystem.eAspectRatio));
	if (bAudio)
	{
		HxLOG_Print("= [Audio ][Lang  ][%14s] : [%s][0x%08x]\n", "User", svcSi_UtGetLang(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_User]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_User]);
		HxLOG_Print("= [Audio ][Lang  ][%14s] : [%s][0x%08x]\n", "Aud0", svcSi_UtGetLang(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_0]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_0]);
		HxLOG_Print("= [Audio ][Lang  ][%14s] : [%s][0x%08x]\n", "Aud1", svcSi_UtGetLang(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_1]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_1]);
		HxLOG_Print("= [Audio][Lang  ][%14s] : [%s][0x%08x]\n", "Audio Undefined", svcSi_UtGetLang(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Undefined]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Undefined]);
		HxLOG_Print("= [Audio][Lang  ][%14s] : [%s][0x%08x]\n", "Audio Original", svcSi_UtGetLang(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Original]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Original]);
		HxLOG_Print("= [Audio][Lang  ][%14s] : [%s][0x%08x]\n", "Audio Default", svcSi_UtGetLang(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Default]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Default]);
		HxLOG_Print("= [Audio ][Output][%14s] : [%d]\n", "StereoSelect", pstConfig->stAudioOutput.eStereoSelect);
		HxLOG_Print("= [Audio ][Output][%14s] : [%d]\n", "MultiChan User", pstConfig->stAudioOutput.abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_User]);
		HxLOG_Print("= [Audio ][Output][%14s] : [%d]\n", "MultiChan Menu", pstConfig->stAudioOutput.abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_Menu]);
		HxLOG_Print("= [Audio ][AudDes][%14s] : [%d]\n", "User", pstConfig->stAudioDesc.abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_User]);
		HxLOG_Print("= [Audio ][AudDes][%14s] : [%d]\n", "Menu", pstConfig->stAudioDesc.abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_Menu]);
	}
	if (bSubtitle)
	{
		HxLOG_Print("= [Subttl][Lang  ][%14s] : [%s][0x%08x]\n", "User", svcSi_UtGetLang(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_User]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_SUBTTL_LANG_User]);
		HxLOG_Print("= [Subttl][Lang  ][%14s] : [%s][0x%08x]\n", "Subtitle0", svcSi_UtGetLang(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_0]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_SUBTTL_LANG_0]);
		HxLOG_Print("= [Subttl][Lang  ][%14s] : [%s][0x%08x]\n", "Subtitle1", svcSi_UtGetLang(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_1]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_SUBTTL_LANG_1]);
		HxLOG_Print("= [Subttl][Lang  ][%14s] : [%s][0x%08x]\n", "Subtitle Undefined", svcSi_UtGetLang(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Undefined]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Undefined]);
		HxLOG_Print("= [Subttl][Lang  ][%14s] : [%s][0x%08x]\n", "Subtitle Original", svcSi_UtGetLang(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Original]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Original]);
		HxLOG_Print("= [Subttl][Lang  ][%14s] : [%s][0x%08x]\n", "Subtitle Default", svcSi_UtGetLang(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Default]), pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Default]);

		HxLOG_Print("= [Subttl][Type  ][%14s] : [%s]\n", "DVB(Menu)", pstConfig->stSubtitleType.abDvb[eSvcSi_CFG_SUBTTL_DVB_Menu] ? "TRUE" : "FALSE");
		HxLOG_Print("= [Subttl][Type  ][%14s] : [%s]\n", "DVB(User}", pstConfig->stSubtitleType.abDvb[eSvcSi_CFG_SUBTTL_DVB_User] ? "TRUE" : "FALSE");
		HxLOG_Print("= [Subttl][Type  ][%14s] : [%s]\n", "HOH(Menu)", pstConfig->stSubtitleType.abHoh[eSvcSi_CFG_SUBTTL_HOH_Menu] ? "TRUE" : "FALSE");
		HxLOG_Print("= [Subttl][Type  ][%14s] : [%s]\n", "HOH(User)", pstConfig->stSubtitleType.abHoh[eSvcSi_CFG_SUBTTL_HOH_User] ? "TRUE" : "FALSE");
		HxLOG_Print("= [Subttl][Type  ][%14s] : [%s]\n", "HD(Menu)", pstConfig->stSubtitleType.abHD[eSvcSi_CFG_SUBTTL_HD_Menu] ? "TRUE" : "FALSE");
		HxLOG_Print("= [Subttl][Type  ][%14s] : [%s]\n", "HD(User)", pstConfig->stSubtitleType.abHD[eSvcSi_CFG_SUBTTL_HD_User] ? "TRUE" : "FALSE");
	}
	HxLOG_Print("================================================================\n");
#endif
}

/* end of file */
