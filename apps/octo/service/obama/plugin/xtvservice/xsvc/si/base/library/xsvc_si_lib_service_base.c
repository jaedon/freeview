/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_service_base.c
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
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_live.h>
#include <_xsvc_si_lib.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define BASE_SBTL_PRIORITY_NONE					0x00000000
#define BASE_SBTL_PRIORITY_EBU					0x00000001
#define BASE_SBTL_PRIORITY_DVB					0x00000002
#define BASE_SBTL_PRIORITY_HOH_O_ON				0x00000004		// Hard of hearing Menu: On         && PMT: On
#define BASE_SBTL_PRIORITY_LANG					0x00000008
#define BASE_SBTL_PRIORITY_SBTL_LANG			0x00000010
#define BASE_SBTL_PRIORITY_USER					0x00000020		/** < Opt select */

#if defined(CONFIG_MW_SI_LOCALTIME) && defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
#define	TZH_TIME_DATA_NUM						2
#define TZH_TIME_CHAR_NUM						11
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if defined(CONFIG_MW_SI_LOCALTIME) && defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
typedef struct
{
	HUINT8					 ucTzh_magic[4];
	HUINT8					 ucTzh_version;
	HUINT8					 ucTzh_reserved[15];
	HUINT32 				 ulTzh_ttisgmtcnt;
	HUINT32 				 ulTzh_ttisstdcnt;
	HUINT32	 				 ulTzh_leapcnt;
	HUINT32 				 ulTzh_timecnt;
	HUINT32 				 ulTzh_typecnt;
	HUINT32 				 ulTzh_charcnt;
	// 추가되거나 삭제 되면 안됨. 32bit의 배수
} TzDbFileHeader_t;

typedef struct
{
	// types of local time starting at ulTzh_time
	HUINT8					 ucTzh_timeType1;
	HUINT8					 ucTzh_timeType2;
	HUINT8					 ulTzh_timeOffset1[4];		// HUINT32
	HUINT8					 ucTzh_timeOffsetType1;
	HUINT8					 ucTzh_timeCharOffset1;
	HUINT8					 ulTzh_timeOffset2[4];		// HUINT32
	HUINT8					 ucTzh_timeOffsetType2;
	HUINT8					 ucTzh_timeCharOffset2;

	HUINT8					 ucTzh_timeChar[TZH_TIME_CHAR_NUM];		// Time Char
	HUINT8					 ucTzh_ttisstd1;
	HUINT8					 ucTzh_ttisstd2;
	HUINT8					 ucTzh_ttisgmt;
	// 추가되거나 삭제 되면 안됨. 32bit의 배수
} TzDbFileTimeInfo_t;

typedef struct
{
	//1st-Loop
	TzDbFileHeader_t		 stTzh_header;
	UNIXTIME				 ulTzh_time32_1;		// coded transition times
	UNIXTIME				 ulTzh_time32_2;
	TzDbFileTimeInfo_t		 stTzh_timeInfo;

	// 2nd-Loop
	TzDbFileHeader_t		 stTzh_header2;
	UNIXTIME				 ulTzh_time64_1h;		// coded transition times
	UNIXTIME				 ulTzh_time64_1l;		// coded transition times
	UNIXTIME				 ulTzh_time64_2h;		// coded transition times
	UNIXTIME				 ulTzh_time64_2l;		// coded transition times

	TzDbFileTimeInfo_t		 stTzh_timeInfo2;
	HCHAR					 szTzString[MAX_TIMEZONE_STR_LEN+1];
} TzDbFileData_t;
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC HERROR xsvc_si_LibSvcEsSetCasTypeToPmtEsInfoByDesc_Base (HUINT32 ulActionId, HUINT8 *pucRawDes, svcSi_PmtEsInfo_t *pstPmtEsInfo)
{
	DxCasType_e			 eCasType	=	0;
	SIxCaDes_t			*pstCaDes	=	NULL;
	SIxDescriptor_t		*pstDes		=	NULL;
	svcSi_EcmEs_t		*pstEcmEs	= 	NULL;

	if ((NULL == pucRawDes) || (eSIxDESCTAG_CA != pucRawDes[0]))		{ return ERR_FAIL; }

	pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
	if (NULL == pstDes)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseDes NULL\n");
		return ERR_FAIL;
	}

	pstCaDes = (SIxCaDes_t *)pstDes;
	svc_si_LibConvertSiCasIdToCasType ((HUINT32)pstCaDes->usCaSysId, &eCasType);

	pstEcmEs = (svcSi_EcmEs_t *)OxSI_Calloc (sizeof(svcSi_EcmEs_t));
	pstEcmEs->usPid		= pstCaDes->usCaPid;
	pstEcmEs->usCaSysId	= pstCaDes->usCaSysId;

	PAL_SIRAWPOOL_FreeDes (pstDes);

	pstPmtEsInfo->ulCasType |= eCasType;
	pstPmtEsInfo->pstEcmTrack = HLIB_LIST_Append (pstPmtEsInfo->pstEcmTrack, (void *)pstEcmEs);

	return ERR_OK;
}

STATIC HERROR xsvc_si_LibSvcEsGetComponentTagFromDesc_Base (HUINT32 ulActionId, HUINT8 *pucRawDes, HUINT8 *pucComponentTag)
{
	SIxStmIdentDes_t 	*pstStreamIdDes;
	SIxDescriptor_t			*pstDes;

	if ((NULL == pucRawDes) || (eSIxDESCTAG_STREAM_IDENTIFIER != pucRawDes[0]))		{ return ERR_FAIL; }

	pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
	if (NULL == pstDes)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseDes\n");
		return ERR_FAIL;
	}

	pstStreamIdDes = (SIxStmIdentDes_t *)pstDes;
	*pucComponentTag = pstStreamIdDes->ucCompTag;

	PAL_SIRAWPOOL_FreeDes (pstDes);

	return ERR_OK;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtEsFromVideoStreamType_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);
	svcSi_VideoEs_t			*pstVideoEs = (svcSi_VideoEs_t *)OxSI_Calloc (sizeof(svcSi_VideoEs_t));

	if (NULL == pstVideoEs)			{ return ERR_FAIL; }

	pstVideoEs->usPid = pstPmtElem->usElemPid;
	if (nComponentTag >= 0)
	{
		pstVideoEs->ucComponentTag = (HUINT8)nComponentTag;
	}

	pstVideoEs->eVideoCodec = eDxVIDEO_CODEC_MPEG2; // Default:
	(void)svc_si_LibConvertSiStreamTypeToVideoCodec ((HUINT32)pstPmtElem->ucStreamType, &(pstVideoEs->eVideoCodec));
	pstVideoEs->bCaDesFound = bCaDesFound;

	pstVideoEs->bIsHd = FALSE;
	if (eDxVIDEO_CODEC_H264 == pstVideoEs->eVideoCodec || eDxVIDEO_CODEC_H265 == pstVideoEs->eVideoCodec)
	{
		HUINT8			*pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndItem, eSIxDESCTAG_AVC_VIDEO, 0);

		if (NULL != pucRawDes)
		{
			SIxDescriptor_t		*pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

			if (NULL != pstDes)
			{

#if 0
				SIxAvcVideoDes_t	*pstAvcVideoDes = (SIxAvcVideoDes_t *)pstDes;
				/**
				 *	13818-1, 14496-10을 확인해 보았으나 아래 0x4d, 0x1e가 무엇을 의미하는 지 명확히 확인되지 않아 일단 막아둠
				 *	아래 코드는 항상 TRUE가 나오는 구조이므로 일단 기존처럼 항상 HD인 것으로 처리함
				 *	- 2009년 OCTO1에 최초 commit될 때부터 이 상태인 것으로 보아 코딩 실수로 보임
				**/

				if ((pstAvcVideoDes->ucProfile != 0x4D) || (pstAvcVideoDes->ucProfile != 0x1E))
				{
					pstVideoEs->bIsHd = TRUE;
				}
#endif

#if defined(CONFIG_OP_NORDIG)
				SIxAvcVideoDes_t	*pstAvcVideoDes = (SIxAvcVideoDes_t *)pstDes;

				// AVC HD Video
				if (pstAvcVideoDes->ucLevel >= 0x28)
				{
					pstVideoEs->bIsHd = TRUE;
				}
#else
				pstVideoEs->bIsHd = TRUE;
#endif

				PAL_SIRAWPOOL_FreeDes (pstDes);
			}
		}
	}

	pstPmtEsInfo->pstVideoTrack = HLIB_LIST_Append (pstPmtEsInfo->pstVideoTrack, (void *)pstVideoEs);
	return ERR_OK;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtEsFromPrivateRegdescFormatIdent_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, SIxDescriptor_t *pstDes, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	HERROR					 hResult = ERR_FAIL;

	if (eSIxDESCTAG_REGISTRATION == pstDes->stHead.eTag)
	{
		SIxRegDes_t 	*pstRegDes = (SIxRegDes_t *)pstDes;

		if(pstRegDes->ulFormatIdent == ISO_REGD_FORMAT_VIDEO_HEVC) // HEVC
		{
			SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);
			svcSi_VideoEs_t 		*pstVideoEs;

			if (NULL == pstPmtElem)
			{
				goto END_FUNC;
			}

			pstVideoEs = (svcSi_VideoEs_t *)OxSI_Calloc (sizeof(svcSi_VideoEs_t));
			if (NULL == pstVideoEs)
			{
				goto END_FUNC;
			}

			pstVideoEs->usPid = pstPmtElem->usElemPid;
			if (nComponentTag >= 0)
			{
				pstVideoEs->ucComponentTag = (HUINT8)nComponentTag;
			}

			pstVideoEs->eVideoCodec = eDxVIDEO_CODEC_H265; // Default:
			pstVideoEs->bCaDesFound = bCaDesFound;
			pstVideoEs->bIsHd = TRUE;

			pstPmtEsInfo->pstVideoTrack = HLIB_LIST_Append (pstPmtEsInfo->pstVideoTrack, (void *)pstVideoEs);
			hResult = ERR_OK;
		}
	}

END_FUNC :

	return hResult;
}

STATIC HERROR xsvc_si_LibSvcEsIsPrivateVideo_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem)
{
	HERROR					 hResult = ERR_FAIL;
	HUINT8					*pucRawDes = NULL;
	SIxDescriptor_t 		*pstDes = NULL;

	if ((pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndItem, eSIxDESCTAG_REGISTRATION, 0)) != NULL)
	{
		pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

		if (NULL != pstDes)
		{
			if (eSIxDESCTAG_REGISTRATION == pstDes->stHead.eTag)
			{
				SIxRegDes_t		*pstRegDes = (SIxRegDes_t *)pstDes;

				if(pstRegDes->ulFormatIdent == ISO_REGD_FORMAT_VIDEO_HEVC) // HEVC
				{
					hResult = ERR_OK;
				}
			}

			PAL_SIRAWPOOL_FreeDes (pstDes);
		}
	}

	return hResult;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtEsFromAudioStreamType_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);
	svcSi_AudioEs_t			*pstAudioEs = (svcSi_AudioEs_t *)OxSI_Calloc (sizeof(svcSi_AudioEs_t));

	if (NULL == pstAudioEs)			{ return ERR_FAIL; }

	// Initailization
	pstAudioEs->ucMixType = 0xff;
	pstAudioEs->eSuppAudioEditorial = eAUDIO_TYPE_SUPP_UNKNOWN;


	pstAudioEs->usPid = pstPmtElem->usElemPid;
	if (nComponentTag >= 0)
	{
		pstAudioEs->ucComponentTag = (HUINT8)nComponentTag;
	}

	pstAudioEs->eAudCodec = eDxAUDIO_CODEC_MPEG; // Default:
	(void)svc_si_LibConvertSiStreamTypeToAudioCodec ((HUINT32)pstPmtElem->ucStreamType, &(pstAudioEs->eAudCodec));
	pstAudioEs->bCaDesFound = bCaDesFound;

	(void)svc_si_LibGetAudioLanguageCodeFromPmt (ulActionId, pst2ndItem, MAX_LANG_PER_ONE_TRACK, pstAudioEs->astIsoLangInfo, NULL);

	(void)svc_si_LibGetSupplementaryAudioLanguageCodeFromPmt(ulActionId, pst2ndItem, MAX_LANG_PER_ONE_TRACK, pstAudioEs, NULL);

	if ((ISO_AUDIO_AAC == pstPmtElem->ucStreamType) || (ISO_AUDIO_4 == pstPmtElem->ucStreamType))
	{
		HUINT8		*pucDesRaw = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndItem,  eSIxDESCTAG_AAC, 0);
		SIxAACDes_t	*pstAacDes = (SIxAACDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

		if (NULL != pstAacDes)
		{
			if (TRUE == pstAacDes->bAACTypeFlag)
			{
				(void)svc_si_LibConvertSiAacTypeToAudioCodec (pstAacDes->ucAACType, &(pstAudioEs->eAudCodec));
				(void)svc_si_LibConvertSiAacProfileLevelToChannelNum (pstAacDes->ucProfileAndLevel, &(pstAudioEs->ucChannelNum));
			}

			PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstAacDes);
		}
	}

	pstPmtEsInfo->pstAudioTrack = HLIB_LIST_Append (pstPmtEsInfo->pstAudioTrack, (void *)pstAudioEs);
	return ERR_OK;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtDolbyEsFromPrivateStreamType_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, SIxDescriptor_t *pstDes, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	HUINT8					 ucComponentType;
	DxAudioCodec_e			 eAudioCodec;
	SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);
	svcSi_AudioEs_t			*pstAudioEs = NULL;

	if (eSIxDESCTAG_DOLBY_AC3 == pstDes->stHead.eTag)
	{
		SIxDolbyAC3Des_t		*pstAc3Des = (SIxDolbyAC3Des_t *)pstDes;

		eAudioCodec		= eDxAUDIO_CODEC_DOLBY_AC3;
		ucComponentType	= pstAc3Des->ucComponentType;

	}
	else if (eSIxDESCTAG_ENHANCED_AC3 == pstDes->stHead.eTag)
	{
		SIxEnhancedAC3Des_t	*pstEnAc3Des = (SIxEnhancedAC3Des_t *)pstDes;

		eAudioCodec		= eDxAUDIO_CODEC_DOLBY_AC3P;
		ucComponentType	= pstEnAc3Des->ucComponentType;
	}
	else
	{
		return ERR_FAIL;
	}

	pstAudioEs = (svcSi_AudioEs_t *)OxSI_Calloc (sizeof(svcSi_AudioEs_t));
	if (NULL == pstAudioEs)
	{
		return ERR_FAIL;
	}

	pstAudioEs->ucMixType = 0xff;
	pstAudioEs->eSuppAudioEditorial = eAUDIO_TYPE_SUPP_UNKNOWN;

	pstAudioEs->usPid = pstPmtElem->usElemPid;
	if (nComponentTag >= 0)
	{
		pstAudioEs->ucComponentTag = (HUINT8)nComponentTag;
	}


	pstAudioEs->eAudCodec = eAudioCodec;
	pstAudioEs->bCaDesFound = bCaDesFound;

	(void)svc_si_LibGetAudioLanguageCodeFromPmt (ulActionId, pst2ndItem, MAX_LANG_PER_ONE_TRACK, pstAudioEs->astIsoLangInfo, NULL);

	(void)svc_si_LibGetSupplementaryAudioLanguageCodeFromPmt(ulActionId, pst2ndItem, MAX_LANG_PER_ONE_TRACK, pstAudioEs, NULL);

	(void)svc_si_LibConvertSiAc3ComponentTypeToChannelNum (ucComponentType, &(pstAudioEs->ucChannelNum));

	pstPmtEsInfo->pstAudioTrack = HLIB_LIST_Append (pstPmtEsInfo->pstAudioTrack, (void *)pstAudioEs);
	return ERR_OK;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtSubtitleEsFromSubtitleDesInfo_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, SIxSubtitleInfo_t *pstSbtlInfo, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	svcSi_SubtitleEs_t		*pstSbtlEs;
	SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);

	switch (pstSbtlInfo->ucSubtitleType)
	{
	/* Normal */
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14: 	/* Freesat HD stream에서 여기로 들어 옴. */

	/* For the hard of hearing */
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24: 	/* 300.468 Latest Version */
		pstSbtlEs = (svcSi_SubtitleEs_t *)OxSI_Calloc (sizeof(svcSi_SubtitleEs_t));
		if (NULL != pstSbtlEs)
		{
			pstSbtlEs->usPid				= pstPmtElem->usElemPid;
			if (nComponentTag >= 0)
			{
				pstSbtlEs->ucComponentTag = (HUINT8)nComponentTag;
			}

			pstSbtlEs->ucType				= pstSbtlInfo->ucSubtitleType;
			pstSbtlEs->usCompositionPageId	= pstSbtlInfo->usCompositionPageId;
			pstSbtlEs->usAncillaryPageId	= pstSbtlInfo->usAncillaryPageId;
			pstSbtlEs->bCaDesFound			= bCaDesFound;
			HxSTD_MemCopy (pstSbtlEs->aucLangCode, pstSbtlInfo->szIso639LangCode, 4);

			pstPmtEsInfo->pstSubttlTrack = HLIB_LIST_Append (pstPmtEsInfo->pstSubttlTrack, (void *)pstSbtlEs);
		}

		return ERR_OK;

	default:
		HxLOG_Error ("Check Subtitle Type (0x%02x)\n", pstSbtlInfo->ucSubtitleType);
		break;
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtSubtitleEsFromPrivateStreamType_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	HBOOL				 bSubtitleFound = FALSE;
	HxList_t			*pstDesItem;
	HERROR				 hErr;

	for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
	{
		HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstDesItem);

		if ((NULL != pucRawDes) && (eSIxDESCTAG_SUBTITLING == pucRawDes[0]))
		{
			SIxSubtitleInfo_t	*pstSbtlInfo;
			SIxSubtitleDes_t		*pstSbtlDes = (SIxSubtitleDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

			if (NULL != pstSbtlDes)
			{
				for (pstSbtlInfo = pstSbtlDes->pstInfo; NULL != pstSbtlInfo; pstSbtlInfo = pstSbtlInfo->pstNext)
				{
					hErr = xsvc_si_LibSvcEsGetPmtSubtitleEsFromSubtitleDesInfo_Base (ulActionId, pst2ndItem, pstSbtlInfo, pstPmtEsInfo, bCaDesFound, nComponentTag);
					if (ERR_OK == hErr)
					{
						bSubtitleFound = TRUE;
					}
				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSbtlDes);
			}
		}
	}

	return (TRUE == bSubtitleFound) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtTeletextEsFromTeletextDesInfo_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, SIxTeletextInfo_t *pstTtxInfo, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	svcSi_TeletextEs_t		*pstTtxEs;
	SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);

	pstTtxEs = (svcSi_TeletextEs_t *)OxSI_Calloc (sizeof(svcSi_TeletextEs_t));
	if (NULL == pstTtxEs)					{ return ERR_FAIL; }

	pstTtxEs->usPid = pstPmtElem->usElemPid;
	if (nComponentTag >= 0)
	{
		pstTtxEs->ucComponentTag	= (HUINT8)nComponentTag;
	}
	pstTtxEs->ucTeltxtType		= pstTtxInfo->ucType;
	pstTtxEs->ucMagazineNumber	= pstTtxInfo->ucMagazineNum;
	pstTtxEs->ucPageNumber		= pstTtxInfo->ucPageNum;
	HxSTD_MemCopy (pstTtxEs->aucLangCode, pstTtxInfo->szIso639LangCode, 4);
	pstTtxEs->bCaDesFound		= bCaDesFound;

	pstPmtEsInfo->pstTeltxtTrack = HLIB_LIST_Append (pstPmtEsInfo->pstTeltxtTrack, (void *)pstTtxEs);
	return ERR_OK;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtSubtitleEsFromTeletextDesInfo_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, SIxTeletextInfo_t *pstTtxInfo, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	svcSi_SubtitleEs_t		*pstSbtlEs;
	SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);

	pstSbtlEs = (svcSi_SubtitleEs_t *)OxSI_Calloc (sizeof(svcSi_SubtitleEs_t));
	if (NULL == pstSbtlEs)					{ return ERR_FAIL; }

	pstSbtlEs->usPid			= pstPmtElem->usElemPid;
	if (nComponentTag > 0)
	{
		pstSbtlEs->ucComponentTag	= (HUINT8)nComponentTag;
	}

	pstSbtlEs->ucType			= pstTtxInfo->ucType;
	pstSbtlEs->ucMagazineNumber	= pstTtxInfo->ucMagazineNum;
	pstSbtlEs->ucPageNumber		= pstTtxInfo->ucPageNum;
	pstSbtlEs->bCaDesFound		= bCaDesFound;
	HxSTD_MemCopy (pstSbtlEs->aucLangCode, pstTtxInfo->szIso639LangCode, 4);

	pstPmtEsInfo->pstSubttlTrack = HLIB_LIST_Append (pstPmtEsInfo->pstSubttlTrack, (void *)pstSbtlEs);
	return ERR_OK;
}

STATIC HERROR xsvc_si_LibSvcEsGetPmtTeletextEsFromPrivateStreamType_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	HBOOL				 bFound = FALSE;
	HxList_t			*pstDesItem;
	HERROR				 hErr;

	for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
	{
		HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstDesItem);

		if ((NULL != pucRawDes) && (eSIxDESCTAG_TELETEXT == pucRawDes[0]))
		{
			SIxTeletextInfo_t	*pstTtxInfo;
			SIxTeletextDes_t		*pstTtxDes = (SIxTeletextDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

			if (NULL != pstTtxDes)
			{
				for (pstTtxInfo = pstTtxDes->pstInfo; NULL != pstTtxInfo; pstTtxInfo = pstTtxInfo->pstNext)
				{
					switch (pstTtxInfo->ucType)
					{
					case TTX_DESTYPE_INITIAL_TELETEXT:
					case TTX_DESTYPE_ADDITIONAL_INFO_PAGE:
					case TTX_DESTYPE_PROG_SCHEDULE_PAGE:
						hErr = xsvc_si_LibSvcEsGetPmtTeletextEsFromTeletextDesInfo_Base (ulActionId, pst2ndItem, pstTtxInfo, pstPmtEsInfo, bCaDesFound, nComponentTag);
						if (ERR_OK == hErr)
						{
							bFound = TRUE;
						}

						break;

					case TTX_DESTYPE_TELETEXT_SUBTITLE:
					case TTX_DESTYPE_TTX_SBTL_PAGE_HEARING_IMPAIRED:
						hErr = xsvc_si_LibSvcEsGetPmtSubtitleEsFromTeletextDesInfo_Base (ulActionId, pst2ndItem, pstTtxInfo, pstPmtEsInfo, bCaDesFound, nComponentTag);
						if (ERR_OK == hErr)
						{
							bFound = TRUE;
						}

						break;

					default:
						break;
					}

				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstTtxDes);
			}
		}
	}

	return (TRUE == bFound) ? ERR_OK : ERR_FAIL;
}

/******************************************
  *	Close caption + Superimpose for JAPAN
*******************************************/
STATIC HERROR xsvc_si_LibSvcEsGetPmtSuperCcEsFromPrivateStreamType_Base (HUINT32 ulActionId, SIxTable_2ndLoop_t *pst2ndItem, svcSi_PmtEsInfo_t *pstPmtEsInfo, HBOOL bCaDesFound, HINT32 nComponentTag)
{
	HBOOL					bFoundSuperCc = FALSE;

	if (((nComponentTag >= 0x30) && (nComponentTag <= 0x37))	/* Closedcaption */
		|| (nComponentTag >= 0x38))								/* SuperImpose */
	{
		SIxTable_PmtElement_t	*pstPmtElem = &(pst2ndItem->unInfo.stPmt);
		svcSi_CaptionEs_t		*pstCaptionEs = NULL;

		pstCaptionEs = (svcSi_CaptionEs_t *)OxSI_Calloc (sizeof(svcSi_CaptionEs_t));
		if (pstCaptionEs != NULL)
		{
			pstCaptionEs->usPid				= pstPmtElem->usElemPid;
			pstCaptionEs->ucComponentTag 	= (HUINT8)nComponentTag;
			bFoundSuperCc = TRUE;

			pstPmtEsInfo->pstCaptionTrack = HLIB_LIST_Append (pstPmtEsInfo->pstCaptionTrack, (void *)pstCaptionEs);
		}
	}

	return (TRUE == bFoundSuperCc) ? ERR_OK : ERR_FAIL;
}

STATIC HBOOL xsvc_si_LibSvcEsIsAudioTrackDolby_Base (svcSi_AudioEs_t *pstAudioEs)
{
	if ((eDxAUDIO_CODEC_DOLBY_AC3 == pstAudioEs->eAudCodec) ||
		(eDxAUDIO_CODEC_DOLBY_AC3P == pstAudioEs->eAudCodec))
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL xsvc_si_LibSvcEsCheckAudioTrackLanguage_Base (svcSi_AudioEs_t *pstAudioEs, HBOOL *pbLang1, HBOOL *pbLang2)
{
	HBOOL			 bLang1 = FALSE, bLang2 = FALSE;
	HxLANG_Id_e	 eAudioLangId		= eLangID_MAX;
	HxLANG_Id_e	 eLangId1			= eLangID_MAX;
	HxLANG_Id_e	 eLangId2			= eLangID_MAX;
	HERROR			 hErr;

//	eAudioLangId = MWC_PARAM_GetAudioLanguage(0);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0) Error!!\n");
	}

	// Check the language number:
/*
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
*/
	eLangId1 = HLIB_LANG_639ToId((const HCHAR *)pstAudioEs->astIsoLangInfo[0].szIso639LangCode);
	eLangId2 = HLIB_LANG_639ToId((const HCHAR *)pstAudioEs->astIsoLangInfo[1].szIso639LangCode);

	bLang1 = (eLangId1 == eAudioLangId) ? TRUE : FALSE;
	bLang2 = (eLangId2 == eAudioLangId) ? TRUE : FALSE;

	if (NULL != pbLang1)					{ *pbLang1 = bLang1; }
	if (NULL != pbLang2)					{ *pbLang2 = bLang2; }

	return ((TRUE == bLang1) || (TRUE == bLang2)) ? TRUE : FALSE;
}


#if defined(CONFIG_MW_SI_LOCALTIME)
#if defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
HERROR xsvc_si_LibSvcEsUpdateTime_localtimeByTot(HCHAR *szTzDstString, UNIXTIME ulNextTimeOfChange, HINT32 nNowLocalTimeOffset, HINT32 nNextLocalTimeOffset)
{
	/*
			1. TOT를 기반으로 tz Database를 만들어서 사용한다.
			2. 현재 사용되고 있는 Offset이 언제 바뀐 것인지 정확한 기록을 할 수 없다는 한계가 존재 한다.
			3. 사용되고 있는 Time은 두개의 값이 존재 한다.
			        - 현재의 Time Offset 적용 시점 - 0x00 : 1970년 1월 1일 0시
			        - 다음 Time Offset 적용 시점 - TOT로 들어오는 값
	*/

	TzDbFileData_t				*pstTzDbFileData;

	// Header
	const HUINT8				 ucTzh_magic[] = {'T', 'Z', 'i', 'f'};
	const HUINT8				 ucTzh_version = '2';
	const HUINT32				 ulTzh_ttisgmtcnt = TZH_TIME_DATA_NUM;
	const HUINT32				 ulTzh_ttisstdcnt = TZH_TIME_DATA_NUM;
	const HUINT32				 ulTzh_leapcnt = 0;
	const HUINT32				 ulTzh_timecnt = TZH_TIME_DATA_NUM;
	const HUINT32				 ulTzh_typecnt = TZH_TIME_DATA_NUM;
	const HUINT32				 ulTzh_charcnt = TZH_TIME_CHAR_NUM-1;
	const HUINT8				 ucTzh_timeChar[TZH_TIME_CHAR_NUM] = {'G', 'M', 'T', '\0', 'D', 'S', 'T', '\0', '\0', '\0', '0'};
	const UNIXTIME 				 ulNowTimeOfChange = 0x00000000;	// 현재 시간에 대한 Time Offset은 무조건 먼 과거


	// tz database file structure
	pstTzDbFileData = (TzDbFileData_t *)HLIB_STD_MemAlloc(sizeof(TzDbFileData_t));
	if (NULL == pstTzDbFileData)
	{
		HxLOG_Error("Memory allocation failed\n");
		goto END_FUNC;
	}

	// Header
	HxSTD_memcpy(pstTzDbFileData->stTzh_header.ucTzh_magic, ucTzh_magic, sizeof(ucTzh_magic));
	pstTzDbFileData->stTzh_header.ucTzh_version = ucTzh_version;
	pstTzDbFileData->stTzh_header.ulTzh_ttisgmtcnt = HxMACRO_SWAP32(ulTzh_ttisgmtcnt);
	pstTzDbFileData->stTzh_header.ulTzh_ttisstdcnt = HxMACRO_SWAP32(ulTzh_ttisstdcnt);
	pstTzDbFileData->stTzh_header.ulTzh_leapcnt = HxMACRO_SWAP32(ulTzh_leapcnt);
	pstTzDbFileData->stTzh_header.ulTzh_timecnt = HxMACRO_SWAP32(ulTzh_timecnt);
	pstTzDbFileData->stTzh_header.ulTzh_typecnt = HxMACRO_SWAP32(ulTzh_typecnt);
	pstTzDbFileData->stTzh_header.ulTzh_charcnt = HxMACRO_SWAP32(ulTzh_charcnt);


	// Time
	pstTzDbFileData->ulTzh_time32_1 = HxMACRO_SWAP32(ulNowTimeOfChange);
	pstTzDbFileData->ulTzh_time32_2 = HxMACRO_SWAP32(ulNextTimeOfChange);

	// Type of Time
	pstTzDbFileData->stTzh_timeInfo.ucTzh_timeType1 = 0;	// Type of Local Time - 아래 Time Offset 중에서 어떤 것을 사용할지 결정.
	pstTzDbFileData->stTzh_timeInfo.ucTzh_timeType2 = 1;

	// Time offset
	*((HUINT32*)pstTzDbFileData->stTzh_timeInfo.ulTzh_timeOffset1) = HxMACRO_SWAP32(nNowLocalTimeOffset);
	*((HUINT32*)pstTzDbFileData->stTzh_timeInfo.ulTzh_timeOffset2) = HxMACRO_SWAP32(nNextLocalTimeOffset);

	// DST
	if(nNowLocalTimeOffset < nNextLocalTimeOffset)
	{
		// Current is no DST
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeOffsetType1 = 0; // is no DST
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeCharOffset1 = 0; // GMT - Index of ucTzh_timeChar
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeOffsetType2 = 1; // is DST
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeCharOffset2 = 4; // DST - Index of ucTzh_timeChar
	}
	else
	{
		// Current is DST
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeOffsetType1 = 1; // is DST
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeCharOffset1 = 4; // DST - Index of ucTzh_timeChar
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeOffsetType2 = 0; // is no DST
		pstTzDbFileData->stTzh_timeInfo.ucTzh_timeCharOffset2 = 0; // GMT - Index of ucTzh_timeChar
	}

	// Copy Time Char
	HxSTD_memcpy(pstTzDbFileData->stTzh_timeInfo.ucTzh_timeChar, ucTzh_timeChar, TZH_TIME_CHAR_NUM);

	// Time info
	pstTzDbFileData->stTzh_timeInfo.ucTzh_ttisstd1 = 1;		// if 1, transition time is standard time, if 0, transition time is wall clock time
	pstTzDbFileData->stTzh_timeInfo.ucTzh_ttisstd2 = 1;		//
	pstTzDbFileData->stTzh_timeInfo.ucTzh_ttisgmt = 0;		// if 1, transition time is UT, if 0, transition time is local time

	// 2nd-Loop
	HxSTD_memcpy(&pstTzDbFileData->stTzh_header2, &pstTzDbFileData->stTzh_header, sizeof(TzDbFileHeader_t));
	pstTzDbFileData->ulTzh_time64_1h = 0x00000000;
	pstTzDbFileData->ulTzh_time64_1l = pstTzDbFileData->ulTzh_time32_1;
	pstTzDbFileData->ulTzh_time64_2h = 0x00000000;
	pstTzDbFileData->ulTzh_time64_2l = pstTzDbFileData->ulTzh_time32_2;
	HxSTD_memcpy(&pstTzDbFileData->stTzh_timeInfo2, &pstTzDbFileData->stTzh_timeInfo, sizeof(TzDbFileTimeInfo_t));
	HxSTD_snprintf(pstTzDbFileData->szTzString, MAX_TIMEZONE_STR_LEN, "%c%s%c", 0x0a, szTzDstString, 0x0a);

END_FUNC:

	// write file
	HLIB_FILE_Delete(LOCALTIME_NAME_PATH);

	if(pstTzDbFileData != NULL)
	{
		HLIB_FILE_WriteFile(LOCALTIME_NAME_PATH, (void*)pstTzDbFileData, sizeof(TzDbFileData_t));
		HLIB_STD_MemFree(pstTzDbFileData);
	}

	return ERR_OK;
}
#endif
HERROR xsvc_si_LibSvcEsUpdateLocaltime_linkTzDB(HxCountry_e eCountryId, HINT32 nTimeRegion)
{
	/*
			1. A localtime file is used as a timezone by linking individual city tz file in /usr/share/zoneinfo.
			2. If linking a localtime file in zoneinfo directory, there is no reason to use timezone by TOT.
			3. It can link a localtime file by only Country Code and Time Zone Code.
	*/

	HCHAR	szSystemCommand[LOCALTIME_NAME_LENGTH+1];
	HCHAR	*szLocalTimeFilePath;
	HERROR 	hErr = ERR_FAIL;

	hErr = HLIB_DATETIME_GetLocalTimeZoneInfoFile(eCountryId, (HUINT8)nTimeRegion, &szLocalTimeFilePath);

	HLIB_FILE_Delete(LOCALTIME_NAME_PATH);

	if(hErr == ERR_OK)
	{
		HxSTD_snprintf(szSystemCommand, LOCALTIME_NAME_LENGTH, "ln -s %s%s %s", LOCALTIME_ZONEINFO_PATH, szLocalTimeFilePath, LOCALTIME_NAME_PATH);
		VK_SYSTEM_Command(szSystemCommand);
	}

	return ERR_OK;
}
#endif

STATIC HBOOL xsvc_si_LibSvcEsUpdateTime_tz(SIxTable_PsiTable_t *pstTotTable, HxCountry_e eCountryId, HINT32 nTimeRegion, UNIXTIME ulTimeOfChange, HINT32 nLocalTimeOffset, HINT32 nNextLocalTimeOffset)
{
	HCHAR	 szTzDstString[MAX_TIMEZONE_STR_LEN];
	HCHAR	 szTzFileString[TZ_NAME_LENGTH], szSystemCommand[TZ_NAME_LENGTH + 5];
	FILE	*pfTzFile = fopen(TZ_NAME_PATH, "r");
	HBOOL	 bMakeTzFile = FALSE;

	if (pfTzFile != NULL)
	{
		/**
		  *
		  * TZ file 이 존재한다. 방금 만든 tz string이랑 비교해보고 다르면 업데이트 하자
		  *
		**/
		// TZ flie string을 읽고
		fgets(szTzFileString, TZ_NAME_LENGTH, pfTzFile);
		szTzFileString[HxSTD_StrLen(szTzFileString)-1] = '\0';
		fclose(pfTzFile);

		// 현재 tot로 만든 tz string을 읽어서 tz file에서 읽은 것과 비교
		HLIB_DATETIME_MakeTzFileFromTot(pstTotTable->unInfo.stTot.utUtcTime, eCountryId, ulTimeOfChange, nLocalTimeOffset, nNextLocalTimeOffset, szTzDstString);

		HxLOG_Print("szTzFileString = %s\n", szTzFileString);
		HxLOG_Print("szTzDstString  = %s\n", szTzDstString);


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

	HxLOG_Print("bMakeTzFile = %d\n", bMakeTzFile);
	if (bMakeTzFile == TRUE)
	{
		HxSTD_snprintf(szSystemCommand, 128, "%s%c", szTzDstString, 0x0a);
		HxLOG_Print("szTzDstString: %s\n", szTzDstString);

		HLIB_FILE_WriteFile((const HUINT8 *)TZ_NAME_PATH, szSystemCommand, HxSTD_StrLen(szSystemCommand));
		HLIB_FILE_Delete((const HUINT8 *)TZ_NAME_PATH_TMP);
		HLIB_FILE_Copy((const HUINT8 *)TZ_NAME_PATH, (const HUINT8 *)TZ_NAME_PATH_TMP);

		HxLOG_Print("copy: %s -> %s\n", TZ_NAME_PATH, TZ_NAME_PATH_TMP);


#if defined(CONFIG_MW_SI_LOCALTIME) && defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
		xsvc_si_LibSvcEsUpdateTime_localtimeByTot(szTzDstString, ulTimeOfChange, nLocalTimeOffset*60, nNextLocalTimeOffset*60);
#elif defined(CONFIG_MW_SI_LOCALTIME)
		xsvc_si_LibSvcEsUpdateLocaltime_linkTzDB(eCountryId, nTimeRegion);
#endif
	}
	return TRUE;
}

#define _____SERVICE_SEARCH_COMPOSING______
HERROR xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HUINT16					 usPmtPid = PID_NULL;
	HUINT32					 ulSvcNum = 0, ulPrvNum = 0;
	HxList_t				*pstSdtSvcLoop, *pstSdtSvcDesLoop;
	SIxTable_PsiTable_t		*pstCurrSdt = NULL;
	DbSvc_Info_t			*pstSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;
	DxOperator_e			 eSvcOpType = eDxOPERATOR_None;

	if (pstSdtTable == NULL) 		{ return ERR_FAIL; }

	for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);

				if (eSIxDESCTAG_SERVICE == *pucRawDes)
				{
					ulSvcNum ++;
				}
			}
		}
	}

	if (0 == ulSvcNum)
	{
		HxLOG_Error ("No Service found in SDT\n");
		goto END_FUNC;
	}

	pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	pstPrvArray = (DbSvc_PrvInfo_t *)OxSI_Calloc (sizeof(DbSvc_PrvInfo_t) * ulSvcNum);
	if ((NULL == pstSvcArray) || (NULL == pstPrvArray))
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto END_FUNC;
	}

	ulSvcNum = 0;
	for (pstCurrSdt = pstSdtTable; NULL != pstCurrSdt; pstCurrSdt = pstCurrSdt->pstNextTable)
	{
		for (pstSdtSvcLoop = pstCurrSdt->pst2ndLoop; NULL != pstSdtSvcLoop; pstSdtSvcLoop = pstSdtSvcLoop->next)
		{
			SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstSdtSvcLoop);

			for (pstSdtSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSdtSvcDesLoop; pstSdtSvcDesLoop = pstSdtSvcDesLoop->next)
			{
				HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSdtSvcDesLoop);
				DbSvc_Info_t	*pstSvcInfo = &(pstSvcArray[ulSvcNum]);

				if (eSIxDESCTAG_SERVICE == *pucRawDes)
				{
					SIxSvcDes_t	*pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (NULL != pstSvcDes)
					{
						DxSvcType_e				eSvcType;
						DxVideoType_e			eVideoType;
						SvcSi_UserSettings_t	stUserSettings = { { 0, }, };

						DB_SVC_InitServiceInfo (pstSvcInfo);

						DbSvc_SetOnId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usOrgNetId);
						DbSvc_SetTsId(pstSvcInfo, pstCurrSdt->unInfo.stSdt.usTsId);
						DbSvc_SetSvcId(pstSvcInfo, pstSdtSvcItem->unInfo.stSdt.usSvcId);
						svc_si_LibCheckServiceNameBySvcDes (pstSvcDes->ucSvcName, pstSvcDes->pszServName, (HUINT8 *)DbSvc_GetName(pstSvcInfo), NULL);

						// Provider
						if ((pstSvcDes->ucPrvName > 0) && (NULL != pstSvcDes->pszProviderName))
						{
							HUINT32			 ulPrvCnt, ulPrvLen = (pstSvcDes->ucPrvName < DxMAX_PRV_NAME_LEN) ? (HUINT32)pstSvcDes->ucPrvName : DxMAX_PRV_NAME_LEN;
							HCHAR			 szPrvName[DxMAX_PRV_NAME_LEN + 1];

							HxSTD_StrNCpy (szPrvName, (const char *)pstSvcDes->pszProviderName, ulPrvLen);
							szPrvName[ulPrvLen] = '\0';

							for (ulPrvCnt = 0; ulPrvCnt < ulPrvNum; ulPrvCnt++)
							{
								if (HxSTD_StrNCmp ((HCHAR*)pstPrvArray[ulPrvCnt].szPrvName, szPrvName, DxMAX_PRV_NAME_LEN) == 0)
								{
									break;
								}
							}

							if (ulPrvCnt < ulPrvNum)
							{
								DbSvc_SetPrvIdx(pstSvcInfo, ulPrvCnt);
							}
							else
							{
								DbSvc_SetPrvIdx(pstSvcInfo, ulPrvNum);
								pstPrvArray[ulPrvNum].usPrvIdx = (HUINT16)ulPrvNum;
								HxSTD_StrNCpy ((HCHAR*)pstPrvArray[ulPrvNum].szPrvName, szPrvName, DxMAX_PRV_NAME_LEN);
								pstPrvArray[ulPrvNum].szPrvName[DxMAX_PRV_NAME_LEN - 1] = '\0';

								ulPrvNum ++;
							}
						}

						// PIDs
						hErr = svc_si_LibGetPmtPidFromPat (ulActionId, DbSvc_GetSvcId(pstSvcInfo), &usPmtPid);
						DbSvc_SetPmtPid(pstSvcInfo, (ERR_OK == hErr) ? usPmtPid : PID_NULL);

						(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pstSvcInfo, &stUserSettings);

						hErr = svc_si_LibGetServiceInfoPid (ulActionId, DbSvc_GetSvcId(pstSvcInfo), pstSvcInfo, pstSvcInfo, svc_si_GetSysConfig(&stUserSettings), NULL);
						if (ERR_OK != hErr)
						{
							HxLOG_Debug ("svc_si_LibGetServiceInfoPid err: SvcId(0x%04x)\n", DbSvc_GetSvcId(pstSvcInfo));
						}

						// Service type, Video Type
						eSvcType = eDxSVC_TYPE_All;
						eVideoType = eDxVIDEO_TYPE_ALL;
						hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, pstSvcDes->ucServType, &eSvcType, &eVideoType);
						if (ERR_OK != hErr)
						{
							HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: SvcType(%d)\n", pstSvcDes->ucServType);
						}

						DbSvc_SetSvcType(pstSvcInfo, eSvcType);
						DbSvc_SetVideoType(pstSvcInfo, eVideoType);

						// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
						//DbSvc_SetOrgSvcType(pstSvcInfo, DbSvc_GetSvcType(pstSvcInfo));
						DbSvc_SetOrgSvcType(pstSvcInfo, pstSvcDes->ucServType); //??

						// op type
						hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
						if (hErr != ERR_OK)
						{
							HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
							eSvcOpType = eDxOPERATOR_None;
						}
						DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

						ulSvcNum++;
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
					}
				}

#if defined(CONFIG_OP_NORDIG)
				if (eSIxDESCTAG_LINKAGE == *pucRawDes)
				{
					SIxLinkDes_t *pstLinkDes = (SIxLinkDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (pstLinkDes != NULL)
					{
						if (pstLinkDes->ucLinkType == LINKTYPE_NORDIG_SIMULCAST)
						{
							DbSvc_SetSimulcastOnId(pstSvcArray, pstLinkDes->usOrgNetId);
							DbSvc_SetSimulcastTsId(pstSvcArray, pstLinkDes->usTsId);
							DbSvc_SetSimulcastSvcId(pstSvcArray, pstLinkDes->usSvcId);
						}
					}

					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstLinkDes);
				}
#endif
			}
		}
	}

	if (NULL != pulSvcNum)						{ *pulSvcNum = ulSvcNum; }
	if (NULL != ppstSvcArray)
	{
		if (ulSvcNum > 0)
		{
			*ppstSvcArray = pstSvcArray;
			pstSvcArray = NULL;
		}
		else
		{
			*ppstSvcArray = NULL;
		}
	}

	if (NULL != pulPrvNum)						{ *pulPrvNum = ulPrvNum; }
	if (NULL != ppstPrvArray)
	{
		if (ulPrvNum > 0)
		{
			*ppstPrvArray = pstPrvArray;
			pstPrvArray = NULL;
		}
		else
		{
			*ppstPrvArray = NULL;
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }
	if (NULL != pstPrvArray)					{ OxSI_Free (pstPrvArray); }

	return hResult;
}

HERROR xsvc_si_LibSvcEsComposeServiceInfoByPmt_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstPmtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HERROR						 hResult = ERR_FAIL;
	HERROR						 hErr = ERR_OK;
	HUINT32					 	 ulSvcNum = 0;
	DbSvc_Info_t				*pstSvcArray = NULL;
	SIxTable_PsiTable_t			*pstCurrPmt = NULL;
	HUINT16					 	 usPmtPid = PID_NULL;
	HUINT32						 ulDvbSvcType;
	HxList_t					*pst2ndLoop = NULL;
	DxOperator_e			 	 eSvcOpType = eDxOPERATOR_None;

	if (pstPmtTable == NULL) 		{ return ERR_FAIL; }

	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		ulSvcNum++;
	}

	if (0 == ulSvcNum)
	{
		HxLOG_Error ("no pmt found...!! \n");
		goto END_FUNC;
	}

	pstSvcArray = (DbSvc_Info_t *)OxSI_Calloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	if (NULL == pstSvcArray)
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto END_FUNC;
	}

	ulSvcNum = 0;
	for (pstCurrPmt = pstPmtTable; pstCurrPmt != NULL; pstCurrPmt = pstCurrPmt->pstNextTable)
	{
		DbSvc_Info_t			*pstSvcInfo = &(pstSvcArray[ulSvcNum]);
		HUINT8					*pszUndefinedSvcName = NULL;
		DxSvcType_e 			eSvcType;
		DxVideoType_e			eVideoType;
		SvcSi_UserSettings_t	stUserSettings = { { 0, }, };

		DB_SVC_InitServiceInfo (pstSvcInfo);

		// Service id
		DbSvc_SetSvcId(pstSvcInfo, pstCurrPmt->unInfo.stPmt.usProgNum);

		// Service name
		hErr = svc_si_LibGetUndefinedServiceName(ulActionId, &pszUndefinedSvcName);
		if ((hErr == ERR_OK) && (pszUndefinedSvcName != NULL))
		{
			//HLIB_STD_StrUtf8NCpy(pstSvcInfo->szSvcName, pszUndefinedSvcName, CONFIG_DB_SVC_MAX_NAME_BYTES-1);
			DbSvc_SetName(pstSvcInfo, pszUndefinedSvcName);
		}

		// PIDs
		hErr = svc_si_LibGetPmtPidFromPat (ulActionId, DbSvc_GetSvcId(pstSvcInfo), &usPmtPid);
		DbSvc_SetPmtPid(pstSvcInfo, (ERR_OK == hErr) ? usPmtPid : PID_NULL);

		(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pstSvcInfo, &stUserSettings);

		hErr = svc_si_LibGetServiceInfoPid (ulActionId, DbSvc_GetSvcId(pstSvcInfo), pstSvcInfo, pstSvcInfo, svc_si_GetSysConfig(&stUserSettings), NULL);
		if (ERR_OK != hErr)
		{
			HxLOG_Debug ("svc_si_LibGetServiceInfoPid err: SvcId(0x%04x)\n", DbSvc_GetSvcId(pstSvcInfo));
		}

		// guess service type - tv or radio
		ulDvbSvcType = SERVICE_TYPE_DIGITAL_RADIO;
		for (pst2ndLoop = pstCurrPmt->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data(pst2ndLoop);
			SIxTable_PmtElement_t	*pstPmtElem;
			HERROR					 isPrivateVideo = ERR_FAIL;

			if (NULL == pst2ndItem)					{ continue; }
			pstPmtElem = &(pst2ndItem->unInfo.stPmt);

			if ((pstPmtElem->ucStreamType == ISO_VIDEO_1) || (pstPmtElem->ucStreamType == ISO_VIDEO_2) || (pstPmtElem->ucStreamType == ISO_VIDEO_4))
			{
				// video가 있으므로 tv service type으로 간주
				ulDvbSvcType = SERVICE_TYPE_DIGITAL_TV;
				break;
			}

			isPrivateVideo = xsvc_si_LibSvcEsIsPrivateVideo_Base(ulActionId, pst2ndItem);
			if(isPrivateVideo == ERR_OK)
			{
				ulDvbSvcType = SERVICE_TYPE_DIGITAL_TV;
			}
		}

		// Service type, Video Type
		eSvcType = eDxSVC_TYPE_All;
		eVideoType = eDxVIDEO_TYPE_ALL;
		hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, ulDvbSvcType, &eSvcType, &eVideoType);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: fixed SvcType(%d)\n", SERVICE_TYPE_DIGITAL_TV);
		}

		DbSvc_SetSvcType(pstSvcInfo, eSvcType);
		DbSvc_SetVideoType(pstSvcInfo, eVideoType);

		// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
		//DbSvc_SetOrgSvcType(pstSvcInfo, DbSvc_GetSvcType(pstSvcInfo));
		DbSvc_SetOrgSvcType(pstSvcInfo, ulDvbSvcType); //??

		// op type
		hErr = xsvc_si_LibSrchGetServiceOptype(ulActionId, &eSvcOpType);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("xsvc_si_LibSrchGetServiceOptype() return failed..!! ulActionId(%d) \n", ulActionId);
			eSvcOpType = eDxOPERATOR_None;
		}
		DbSvc_SetOpType(pstSvcInfo, eSvcOpType);

		ulSvcNum++;
	}

	if (NULL != pulSvcNum)						{ *pulSvcNum = ulSvcNum; }
	if (NULL != ppstSvcArray)
	{
		// fix prevent issue #142185
		// 여기까지 오면 pmtTable이 1개 이상이라 항상 ulSvcNum > 0이 되므로 ulSvcNum를 검사하지 않는다
		// pmtTable이 없다면 어래 END_FUNC으로 뛰도록 위에 구현되어 있네요.
		*ppstSvcArray = pstSvcArray;
		pstSvcArray = NULL;
	}

	// pmt로는 provider info를 채울 수가 없다..
	if (NULL != pulPrvNum)						{ *pulPrvNum = 0; }
	if (NULL != ppstPrvArray)					{ *ppstPrvArray = NULL; }

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstSvcArray)					{ OxSI_Free (pstSvcArray); }

	return hResult;
}



#define ___MEMBER_FUNCTIONS___
HERROR xsvc_si_LibSvcEsGetPmtEsInfo_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSiTable, svcSi_PmtEsInfo_t *pstPmtEsInfo)
{
	HUINT8				*pucRawDes;
	SIxDescriptor_t			*pstDes;
	SIxTable_PmtSec_t	*pstPmtSec = &(pstSiTable->unInfo.stPmt);
	HxList_t			*pstElemItem, *pstDesItem;
	HERROR				 hErr;

	if ((NULL == pstSiTable) || (NULL == pstPmtEsInfo) || (eSIxTABLETYPE_Pmt != pstSiTable->eTableType))
	{
		return ERR_FAIL;
	}

	// Service ID
	pstPmtEsInfo->usServiceId = pstPmtSec->usProgNum;

	// CAS system ID
	pstPmtEsInfo->ulCasType = 0;

	for (pstDesItem = pstSiTable->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
	{
		pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstDesItem);
		(void)xsvc_si_LibSvcEsSetCasTypeToPmtEsInfoByDesc_Base (ulActionId, pucRawDes, pstPmtEsInfo);
	}

	// PCR PID
	pstPmtEsInfo->usPcrPid = pstPmtSec->usPcrPid;

	// CRC
	pstPmtEsInfo->ulPmtCRC = pstPmtSec->ulCrc32;

	for (pstElemItem = pstSiTable->pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		HBOOL					 bCaDesFound = FALSE;
		HINT32					 nComponentTag = -1;
		SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstElemItem);
		SIxTable_PmtElement_t	*pstPmtElem;

		if (NULL == pst2ndItem)					{ continue; }
		pstPmtElem = &(pst2ndItem->unInfo.stPmt);

		// CAS & Component Tag:
		bCaDesFound = FALSE;
		for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
		{
			HUINT8			 ucComponentTag = 0;

			pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstDesItem);
			hErr = xsvc_si_LibSvcEsSetCasTypeToPmtEsInfoByDesc_Base (ulActionId, pucRawDes, pstPmtEsInfo);
			if (ERR_OK == hErr)
			{
				bCaDesFound = TRUE;
			}

			hErr = xsvc_si_LibSvcEsGetComponentTagFromDesc_Base (ulActionId, pucRawDes, &ucComponentTag);
			if ((ERR_OK == hErr) && (nComponentTag < 0))
			{
				nComponentTag = (HINT32)ucComponentTag;
			}
		}

		switch (pstPmtElem->ucStreamType)
		{
		case ISO_VIDEO_1:
		case ISO_VIDEO_2:
		case ISO_VIDEO_4:
		case ISO_VIDEO_5:
			(void)xsvc_si_LibSvcEsGetPmtEsFromVideoStreamType_Base (ulActionId, pst2ndItem, pstPmtEsInfo, bCaDesFound, nComponentTag);
			break;

		case ISO_AUDIO_1:
		case ISO_AUDIO_2:
		case ISO_AUDIO_AAC:
		case ISO_AUDIO_4:
			(void)xsvc_si_LibSvcEsGetPmtEsFromAudioStreamType_Base (ulActionId, pst2ndItem, pstPmtEsInfo, bCaDesFound, nComponentTag);
			break;

		case ISO_PRIVDATA_1:
		case ISO_PRIVDATA_2:
			/*
			 * Audio가 ISO_PRIVDATA_2으로 들어 올 수도 있다. 보통 Dolby가 Private Data로 들어 오는 경우가
			 * 있다. 이 경우에 Audio 인지 Teletext 인지 판단 여부는 TAG_DOLBY_AC3 or TAG_ENHANCED_AC3가
			 * 있으면 AUDIO이고 Subtitle or teletext Descriptor가 있으면 Subtitle or Teletext가 되는 것으로
			 * 판단 할 수 밖에 없다.
			 */
			 if ((pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndItem, eSIxDESCTAG_DOLBY_AC3, 0)) != NULL)
			 {
			 	pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
				if (NULL != pstDes)
				{
					(void)xsvc_si_LibSvcEsGetPmtDolbyEsFromPrivateStreamType_Base (ulActionId, pst2ndItem, pstDes, pstPmtEsInfo, bCaDesFound, nComponentTag);
					PAL_SIRAWPOOL_FreeDes (pstDes);
				}
			 }
			 else if ((pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndItem, eSIxDESCTAG_ENHANCED_AC3, 0)) != NULL)
			 {
				 pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
				 if (NULL != pstDes)
				 {
					 (void)xsvc_si_LibSvcEsGetPmtDolbyEsFromPrivateStreamType_Base (ulActionId, pst2ndItem, pstDes, pstPmtEsInfo, bCaDesFound, nComponentTag);
					 PAL_SIRAWPOOL_FreeDes (pstDes);
				 }
			 }
			else if ((pucRawDes = SILIB_TABLE_Get2ndLoopDescriptor (pst2ndItem, eSIxDESCTAG_REGISTRATION, 0)) != NULL)
			{
				pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

				if (NULL != pstDes)
				{
					(void)xsvc_si_LibSvcEsGetPmtEsFromPrivateRegdescFormatIdent_Base (ulActionId, pst2ndItem, pstDes, pstPmtEsInfo, bCaDesFound, nComponentTag);
					PAL_SIRAWPOOL_FreeDes (pstDes);
				}
			}
			 else
			 {
			 	// Subtitle 확인해서 있으면 추가.
				hErr = xsvc_si_LibSvcEsGetPmtSubtitleEsFromPrivateStreamType_Base (ulActionId, pst2ndItem, pstPmtEsInfo, bCaDesFound, nComponentTag);
				if (ERR_OK == hErr)				 { break; }

				// Teletext 확인해서 있으면 추가.
			 	hErr = xsvc_si_LibSvcEsGetPmtTeletextEsFromPrivateStreamType_Base (ulActionId, pst2ndItem, pstPmtEsInfo, bCaDesFound, nComponentTag);
			 	if (ERR_OK == hErr)				 { break; }

				// CC, SuperImpose 확인해서 있으면 추가
				hErr = xsvc_si_LibSvcEsGetPmtSuperCcEsFromPrivateStreamType_Base (ulActionId, pst2ndItem, pstPmtEsInfo, bCaDesFound, nComponentTag);
			 	if (ERR_OK == hErr)				 { break; }
			 }

			 break;

		default:
			break;

		}
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsSelectVideo_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedVideo)
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

HERROR xsvc_si_LibSvcEsSelectAudio_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedAudio_t *pstSelectedAudio)
{
	HBOOL					 bUserFound = FALSE,  bAdOn = FALSE;
	HINT32					 nAudioCnt;
	HINT32					 nSameLangNum = 0;
	HINT32					 nMpegIndex = -1, nDolbyIndex = -1, nAdIndex = -1;
	DxStereoSelect_e		 eSoundMode = eDxSTEREO_SELECT_UNKNOWN;
	HxLANG_Id_e			 	 eSelectedLangId = eLangID_MAX;
	svcSi_AudioEs_t			*pstAudioEs = NULL, *pstSelectedEs = NULL;
	HxList_t					*pstItem;
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
	pstSelectedAudio->eDualMonoSelect = eDxDUALMONO_SELECT_UNKNOWN;

	/*
	 *  [UX 시나리오 2013/02/27: 이광일 제안]

	 1. 특정 채널로 튜닝할 때
		-> User가 해당 채널에서 설정한 Audio Language가 있으면 해당 Audio 선택하여 출력한다.

	 2. User 설정 값 없으면 Settings > General Settings > Language > Audio Language 값 참고

		2.1) Settings > General Settings > Language > Audio Language와 매칭되는 language가 있는 경우
			2.1.1) 매칭되는 Language가 1개 존재할 경우 해당 언어를 선택하여 출력한다.
			2.1.2) 매칭되는 Language가 2개 이상 존재할 경우, 매칭되는 언어에 한해서,
	 -> Settings > General Settings > Audio > S/P DIF와 HDMI 설정 값 중에 하나라도 Multi-channel이 설정되어 있는 경우 Dolby Audio를 선택하여 출력한다. 없으면 맨 상위의 Audio Language를 선택한다.
	 -> Settings > General Settings > Audio > S/P DIF와 HDMI 설정 값이 모두 PCM으로 설정되어 있는 경우 MPEG Audio를 선택하여 출력한다. 없으면 맨 상위의 Audio Language를 선택한다.
				  -> 매칭되는 2개의 Language가 동일한 Auto Format일 경우에는 맨 상위의 Audio Language를 선택한다.

	 2.2) Settings > General Settings > Language > Audio Language와 동일한 Audio Language가 없는 경우, 매칭되는 언어에 상관없이,
	 -> Settings > General Settings > Audio > S/P DIF와 HDMI 설정 값 중에 하나라도 Multi-channel이 설정되어 있는 경우 Dolby Audio를 선택하여 출력한다. 없으면 맨 상위의 Audio Language를 선택한다.
	 -> Settings > General Settings > Audio > S/P DIF와 HDMI 설정 값이 모두 PCM으로 설정되어 있는 경우 MPEG Audio를 선택하여 출력한다. 없으면 맨 상위의 Audio Language를 선택한다.
	 -> 매칭되는 2개의 Language가 동일한 Auto Format일 경우에는 맨 상위의 Audio Language를 선택한다.

	 3. Soundtrack 자체가 없으면 Audio 출력하지 않는다.

	 }
	 */

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
				if ((xsvc_si_LibSvcEsIsAudioTrackDolby_Base (pstAudioEs) == TRUE) &&
					(DbSvc_GetDolbyPid(pstSvcInfo) == pstAudioEs->usPid))
				{
					bUserFound = TRUE;
					break;
				}
			}
			else
			{
				if ((xsvc_si_LibSvcEsIsAudioTrackDolby_Base (pstAudioEs) != TRUE) &&
					(DbSvc_GetAudioPid(pstSvcInfo) == pstAudioEs->usPid))
				{
					bUserFound = TRUE;
					break;
				}
			}
		}

		if (xsvc_si_LibSvcEsCheckAudioTrackLanguage_Base (pstAudioEs, NULL, NULL) == TRUE)
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

		eSelectedLangId = HLIB_LANG_639ToId((const HCHAR *)pstAudioEs->astIsoLangInfo[0].szIso639LangCode);
		if(eSelectedLangId == eLangID_NONE)
		{
			eSelectedLangId = HLIB_LANG_639ToId((const HCHAR *)pstAudioEs->astIsoLangInfo[1].szIso639LangCode);
		}
/*
		hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioEs->astIsoLangInfo[0].szIso639LangCode, &eSelectedLangId);
		if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
		{
			hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioEs->astIsoLangInfo[1].szIso639LangCode, &eSelectedLangId);
			if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
			{
				eSelectedLangId = eLangID_MAX;
			}
		}
*/
		eSoundMode = DbSvc_GetStereoSelect(pstSvcInfo);
	}
	else
	{
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

			if ((nSameLangNum > 0) && (xsvc_si_LibSvcEsCheckAudioTrackLanguage_Base (pstAudioEs, &bLang1, &bLang2) != TRUE))
			{
				continue;
			}

			bTrackDolby = xsvc_si_LibSvcEsIsAudioTrackDolby_Base (pstAudioEs);

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
			eSelectedLangId = HLIB_LANG_639ToId((const HCHAR *)pstSelectedEs->astIsoLangInfo[0].szIso639LangCode);
			if(eSelectedLangId == eLangID_NONE)
			{
				eSelectedLangId = HLIB_LANG_639ToId((const HCHAR *)pstSelectedEs->astIsoLangInfo[1].szIso639LangCode);
			}
/*
			hErr = MWC_UTIL_GetLangIDBy639Code (pstSelectedEs->astIsoLangInfo[0].szIso639LangCode, &eSelectedLangId);
			if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
			{
				hErr = MWC_UTIL_GetLangIDBy639Code (pstSelectedEs->astIsoLangInfo[1].szIso639LangCode, &eSelectedLangId);
				if ((ERR_OK != hErr) || (eSelectedLangId == eLangID_MAX))
				{
					eSelectedLangId = eLangID_MAX;
				}
			}
*/
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

				eAdLangId = eLangID_MAX;
				eAdLangId = HLIB_LANG_639ToId((const HCHAR *)pstLangInfo->szIso639LangCode);
				/*if(eAdLangId == eLangID_NONE)
				hErr = MWC_UTIL_GetLangIDBy639Code (pstLangInfo->szIso639LangCode, &eAdLangId);
				if ( hErr != ERR_OK )
				{
					eAdLangId = eLangID_MAX;
				}
				*/
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

HERROR xsvc_si_LibSvcEsSelectSubtitle_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedSubttl)
{
	HINT32			 nCount, nEsCount = -1;
	HINT32			 nSubttlNum;
	HUINT32			 ulLastPriority;
	HxLANG_Id_e	 eSetLangId = eLangID_MAX, eSiLangId = eLangID_MAX;
	HxList_t		*pstItem;						/** < svcSi_VideoEs_t */
	HERROR			 hErr;
	HBOOL			bSubtitleEnable = FALSE,  bHardOfHearing = FALSE;

	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedSubttl))
	{
		HxLOG_Error("eDB_PARAM_ITEM_SUBTITLE_SETTING  is FALSE\n");
		return ERR_FAIL;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SUBTITLE_SETTING, (HUINT32 *)&bSubtitleEnable, 0);

	if(hErr != ERR_OK)
	{
		HxLOG_Warning("eDB_PARAM_ITEM_SUBTITLE_SETTING  is FALSE\n");
		return ERR_FAIL;
	}

	if(bSubtitleEnable == FALSE)
	{
		HxLOG_Message("eDB_PARAM_ITEM_SUBTITLE_SETTING  is FALSE By DB Item \n");
		return ERR_FAIL;
	}

	pstSelectedSubttl->bFound = FALSE;
	pstSelectedSubttl->nEsIdx = -1;

	nSubttlNum = (HINT32)HLIB_LIST_Length (pstPmtEsInfo->pstSubttlTrack);

	if ((TRUE == DbSvc_GetSubttlFlag(pstSvcInfo)) && (SvcSi_SUBTITLE_INDEX_NULL == DbSvc_GetSubttlIdx(pstSvcInfo)))
	{
		HxLOG_Error("Subtitle User Flag set, but index is invalid \n");
		return ERR_OK;
	}

	if (TRUE == DbSvc_GetSubttlFlag(pstSvcInfo))
	{
		if ((HINT32)DbSvc_GetSubttlIdx(pstSvcInfo) < nSubttlNum)
		{
			pstSelectedSubttl->bFound = TRUE;
			pstSelectedSubttl->nEsIdx = (HINT32)DbSvc_GetSubttlIdx(pstSvcInfo);
		}

		return ERR_OK;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0, (HUINT32 *)&eSetLangId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0) Error!!\n");
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING, (HUINT32 *)&bHardOfHearing, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING) Error!!\n");
	}

	ulLastPriority = BASE_SBTL_PRIORITY_NONE;

	for (pstItem = pstPmtEsInfo->pstSubttlTrack, nCount = 0; NULL != pstItem; pstItem = pstItem->next, nCount++)
	{
		HUINT32 		 		 ulCurrPriority = 0;
		svcSi_SubtitleEs_t		*pstSbtlEs = (svcSi_SubtitleEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL == pstSbtlEs) || (PID_NULL == pstSbtlEs->usPid))
		{
			//HxLOG_Error ("No subtitle es data : count(%d) \n", nCount);
			continue;
		}

		ulCurrPriority |= (pstSbtlEs->ucType <= 0x5) ? BASE_SBTL_PRIORITY_EBU : BASE_SBTL_PRIORITY_DVB;
		if ((DbSvc_GetSubttlFlag(pstSvcInfo)) && (nCount == (HINT32)DbSvc_GetSubttlIdx(pstSvcInfo)))
		{
			ulCurrPriority |= BASE_SBTL_PRIORITY_USER;
		}

		if (IsHardOfHearingSubtitle(pstSbtlEs->ucType))
		{
			if (!bHardOfHearing)			{ continue; }
			ulCurrPriority |= BASE_SBTL_PRIORITY_HOH_O_ON;
		}

		pstSbtlEs->aucLangCode[3] = '\0';
		HLIB_STD_StrLower ((HCHAR *)pstSbtlEs->aucLangCode);
		eSiLangId = HLIB_LANG_639ToId ((const HCHAR *)pstSbtlEs->aucLangCode);

		if (eSiLangId == eSetLangId)
		{
			ulCurrPriority |= BASE_SBTL_PRIORITY_SBTL_LANG;
		}

		if (ulCurrPriority > ulLastPriority)
		{
			ulLastPriority = ulCurrPriority;
			nEsCount = nCount;
		}
	}

	if (ulLastPriority >= BASE_SBTL_PRIORITY_LANG)
	{
		pstSelectedSubttl->bFound	= TRUE;
		pstSelectedSubttl->nEsIdx	= nEsCount;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsSelectTeletext_Base (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedTtx)
{
	if ((NULL == pstPmtEsInfo) || (NULL == pstSvcInfo) || (NULL == pstSelectedTtx))
	{
		return ERR_FAIL;
	}

	// Base 에는 Teletext 관련 시나리오가 없다.

	pstSelectedTtx->bFound = FALSE;
	pstSelectedTtx->nEsIdx = -1;

	if (NULL != pstPmtEsInfo->pstTeltxtTrack)
	{
		pstSelectedTtx->bFound = TRUE;
		pstSelectedTtx->nEsIdx = 0;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibSvcEsComposeServiceInfoInTp_Base (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	SIxTable_PsiTable_t		*pstSdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PsiTable_t		*pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	HERROR					 hErr;

	if (NULL != pstSdtTable)
	{
		HxLOG_Info ("SDT Found: Composite the Services by SDT \n");
		hErr = xsvc_si_LibSvcEsComposeServiceInfoBySdt_Base (ulActionId, pstSdtTable, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
	}
	else if (NULL != pstPmtTable)
	{
		HxLOG_Info ("PMT Found: Composite the Services by PMT \n");
		hErr = xsvc_si_LibSvcEsComposeServiceInfoByPmt_Base (ulActionId, pstPmtTable, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
	}
	else
	{
		HxLOG_Error ("No SDT & PMT\n");
		hErr = ERR_FAIL;
	}

	if (NULL != pstSdtTable)					{ PAL_SIRAWPOOL_FreeTable (pstSdtTable); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }

	return hErr;
}

HERROR xsvc_si_LibSvcEsGetSvcAndVideoTypeFromDvbSvcType_Base (HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType)
{
	if ((NULL == peSvcType) || (NULL == peVideoType))
	{
		return ERR_FAIL;
	}

	*peVideoType = eDxVIDEO_TYPE_ALL;

	switch (ulDvbSvcType)
	{
	case SERVICE_TYPE_DIGITAL_TV :
#if defined(CONFIG_DB_SVC_TYPE_HD)
	case SERVICE_TYPE_AVC_SD_DIGITAL_TV :
#endif
		*peSvcType = eDxSVC_TYPE_TV;
		*peVideoType = eDxVIDEO_TYPE_SD;
		return ERR_OK;

	case SERVICE_TYPE_DIGITAL_RADIO :
		*peSvcType = eDxSVC_TYPE_RADIO;
		return ERR_OK;

#if defined(CONFIG_DB_SVC_TYPE_HD)
	case SERVICE_TYPE_MPEG2_HD :
	case SERVICE_TYPE_AVC_HD_DIGITAL_TV:
	case SERVICE_TYPE_AVC_HD_DIGITAL_TV2:
	case SERVICE_TYPE_HEVC_DIGITAL_TV:
		*peSvcType = eDxSVC_TYPE_TV;
		*peVideoType = eDxVIDEO_TYPE_HD;
		return ERR_OK;
#endif

#if defined(CONFIG_DB_SVC_TYPE_HD)
	case SERVICE_TYPE_FM_RADIO :
		*peSvcType = eDxSVC_TYPE_RADIO;
		return ERR_OK;
#endif

#if defined(CONFIG_DB_SVC_TYPE_DATA) || defined(CONFIG_MW_EPG_TVTV)
	case SERVICE_TYPE_DATA_BROADCAST:
		*peSvcType = eDxSVC_TYPE_DATA;
		return ERR_OK;
#endif

	case SERVICE_TYPE_DVB_MHP:
		// You must convert eSvcType_MHPTV to other valid service type right after parsing this information
		// please, refer to local_itatersrch_CollectServiceDataFromSI function
		*peSvcType = eDxSVC_TYPE_MHPTV;
		*peVideoType = eDxVIDEO_TYPE_ALL;
		return ERR_OK;

	default :
		*peSvcType = eDxSVC_TYPE_All;
		break;
	}

	return ERR_FAIL;
}

HERROR xsvc_si_LibSvcEsUpdateTimeByTdt_Base (HUINT32 ulActionId, SIxTable_PsiTable_t *pstTdtTable)
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

HERROR xsvc_si_LibSvcEsUpdateTimeByTot_Base (HUINT32 ulActionId)
{
	SIxTable_PsiTable_t *pstTotTable = NULL;
	UNIXTIME			 utCurrTime = 0, utSiTime;
	HERROR				 hErr;
	HxList_t			*pstDesLoop = NULL;
	SIxTable_PsiTable_t *pstCurrTot = NULL;
	SIxLocalTimeDes_t	*pstLocalTimeDesc = NULL;
	HxCountry_e 		 eCountryId;
	HCHAR				*pucIso3166Code;
	HINT32				 nTimeRegion;
	HBOOL				 bOffsetFound = FALSE, bTimeDiff = FALSE;
	HUINT8				 aucBcdTime[3];
	HINT32				 nOffsetHour;
	HINT32				 nLocalTimeOffset = 0, nNextLocalTimeOffset=0;	// minutes
	UNIXTIME			 ulTimeOfChange = 0, ulTempTime = 0;
	HxDATETIME_Time_t	 stDatetime;

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
		HxLOG_Error("PAL_SIRAWPOOL_ParseTable() failed..!!	\n");
		return ERR_FAIL;
	}

	for (pstCurrTot = pstTotTable; pstCurrTot != NULL; pstCurrTot = pstCurrTot->pstNextTable)
	{
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
				HxLOG_Print("eSIxDESCTAG_LOCAL_TIME_OFFSET found!!\n");
				pstLocalTimeDesc = (SIxLocalTimeDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDesc);
				if(pstLocalTimeDesc != NULL)
				{
					SIxLocalTimeInfo_t	*pstLocalTimeInfo = NULL;

					pstLocalTimeInfo = pstLocalTimeDesc->pstInfo;

					for (pstLocalTimeInfo = pstLocalTimeDesc->pstInfo; pstLocalTimeInfo != NULL; pstLocalTimeInfo = pstLocalTimeInfo->pstNext)
					{
						HxLOG_Print("pstLocalTimeInfo->szCountryCode = %s, pucIso3166Code = %s\n", pstLocalTimeInfo->szCountryCode, pucIso3166Code);

						if (HxSTD_StrCmp ((char *)pstLocalTimeInfo->szCountryCode, pucIso3166Code) == 0)
						{
							HxLOG_Print("nTimeRegion = %d, pstLocalTimeInfo->ucRegionId = %d\n", nTimeRegion, pstLocalTimeInfo->ucRegionId);

							if (nTimeRegion == 0 ||
								pstLocalTimeInfo->ucRegionId == 0 ||
								nTimeRegion == (HINT32)pstLocalTimeInfo->ucRegionId)
							{
								bOffsetFound = TRUE;
								HxLOG_Print("bOffsetFound = %d\n", bOffsetFound);

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
		HxLOG_Print("call xsvc_si_LibSvcEsUpdateTime_tz\n");
		xsvc_si_LibSvcEsUpdateTime_tz(pstTotTable, eCountryId, nTimeRegion, ulTimeOfChange, nLocalTimeOffset, nNextLocalTimeOffset);
	}

	utSiTime = pstTotTable->unInfo.stTot.utUtcTime;

	hErr = VK_CLOCK_GetTime ((unsigned long *)&utCurrTime);
	if ((VK_OK == hErr) && (utCurrTime > (60 * 60 * 24 * 365)))
	{
		HUINT32 		 ulDiff = (utSiTime > utCurrTime) ? (utSiTime - utCurrTime) : (utCurrTime - utSiTime);

		if (ulDiff > 30)
		{
			bTimeDiff = TRUE;
		}
	}
	else
	{
		bTimeDiff = TRUE;
	}

	if (bTimeDiff)
	{
		HxLOG_Print("call TAPI_SetTime: %u\n", utSiTime);
		if (TAPI_SetTime(TxSOURCE_SET_BY_SI, utSiTime) != ERR_OK)
		{
			HxLOG_Error("[TAPI_SetTime is Failed - time %u]\n", utSiTime);
		}
	}

	if (bOffsetFound == TRUE)
	{
		HINT32	nTime;

		//HxLOG_Print("[local_sitime_UpdateTimeAsTimeScheme] New Time Offset [%02d:%02d]\n", pstParam->nOffsetHour, pstParam->nOffsetMin);
		nTime = (nLocalTimeOffset * 60) ;
		HxLOG_Print("nTime = %d, nTime = %d\n", nTime, nLocalTimeOffset);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_GMT_OFFSET_MINUTES, (HUINT32)nTime, (HUINT32)0);
	}

	if (pstTotTable != NULL)
	{
		PAL_SIRAWPOOL_FreeTable (pstTotTable);
	}

	NOT_USED_PARAM(nOffsetHour);

	return ERR_OK;
}


HERROR xsvc_si_GetAlternativeAudioPriority_Base(const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect)
{
	return ERR_FAIL;
}


/* end of file */

