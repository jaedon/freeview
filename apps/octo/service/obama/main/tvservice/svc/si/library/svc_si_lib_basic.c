/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_lib_basic.c
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
#include <sapi.h>
#include <octo_common.h>
#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <db_svc.h>
#include <db_param.h>
#include <isosvc.h>

//#include "../../si/local_include/si_service.h"
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_hbbtv.h>


#include <psvc_si.h>


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
STATIC DxService_RctLinkInfo_t	s_stInvalidRctLinkInfo;

/*******************************************************************/
/********************	   Static Variables 	********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

STATIC HERROR	svc_si_LibGetPfEventFromEit (HxList_t *pst2ndLoop, SIxTable_EitEvent_t *pstPresentEvt, SIxTable_EitEvent_t *pstFollowEvt, HBOOL *pbFoundPresent, HBOOL *pbFoundFollow)
{
	HxList_t			*pstElemItem = NULL;
	HBOOL				bPresentEvtRead, bFollowEvtRead;

	if ((NULL == pst2ndLoop)
		|| (NULL == pstPresentEvt) || (NULL == pstFollowEvt)
		|| (NULL == pbFoundPresent) || (NULL == pbFoundFollow))
	{
		return ERR_FAIL;
	}

	bPresentEvtRead = bFollowEvtRead = FALSE;
	HxSTD_MemSet (pstPresentEvt, 0, sizeof(SIxTable_EitEvent_t));
	HxSTD_MemSet (pstFollowEvt, 0, sizeof(SIxTable_EitEvent_t));

	for (pstElemItem = pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstElemItem);
		SIxTable_EitEvent_t		*pstEitElem = NULL;

		if (NULL == pst2ndItem)					{ continue; }

		pstEitElem = &(pst2ndItem->unInfo.stEit);
		if (bPresentEvtRead == FALSE)
		{
			// 첫번째 이벤트를 present로 처리
			HxSTD_MemCopy(pstPresentEvt, pstEitElem, sizeof(SIxTable_EitEvent_t));
			bPresentEvtRead = TRUE;
		}
		else if (bFollowEvtRead == FALSE)
		{
			// 두번째 이벤트가 follow가 맞는지 시간으로 비교해본다
			if (pstEitElem->utStartTime >= pstPresentEvt->utStartTime)
			{
				// follow가 맞다
				HxSTD_MemCopy(pstFollowEvt, pstEitElem, sizeof(SIxTable_EitEvent_t));
			}
			else
			{
				// 두번째 이벤트가 present다...!!
				HxSTD_MemCopy(pstFollowEvt, pstPresentEvt, sizeof(SIxTable_EitEvent_t));
				HxSTD_MemCopy(pstPresentEvt, pstEitElem, sizeof(SIxTable_EitEvent_t));
			}

			bFollowEvtRead = TRUE;
		}
	}

	*pbFoundPresent = bPresentEvtRead;
	*pbFoundFollow = bFollowEvtRead;

	return ERR_OK;
}

STATIC HERROR	svc_si_LibSetVideoComponentStatusByComponentType (SIxCompDes_t *pstCompDes, HUINT32 *pulStatus)
{
	HUINT8		 ucCompType;
	HUINT32		 ulStatus;

	if (NULL == pstCompDes)		{ return ERR_FAIL; }
	if (NULL == pulStatus)		{ return ERR_FAIL; }

	ulStatus = 0;

	switch (pstCompDes->ucStmContent)
	{
		case eEsStreamContent_MPEG2_Video:		// MPEG2 VIDEO
			if (pstCompDes->ucCompType >= 0x01 && pstCompDes->ucCompType <= 0x10)
			{
				ulStatus |= EVT_CPNT_VID_MPEG2;
				ucCompType = pstCompDes->ucCompType - 1;

				if ((ucCompType & 0x03) == 0x00)
				{
					ulStatus |= EVT_CPNT_VID_ASPECT_4_3;			// Component Type 0x01, 0x05, 0x09, 0x0D
				}
				else if ((ucCompType & 0x03) != 0x03)
				{
					ulStatus |= EVT_CPNT_VID_ASPECT_16_9;			// Component Type 0x02, 0x03, 0x06, 0x07, 0x0A, 0x0B, 0x0E, 0x0F
				}

				if ((ucCompType & 0x04) == 0x04)
				{
					ulStatus |= EVT_CPNT_VID_30HZ;				// Component Type 0x05, 0x06, 0x07, 0x08, 0x0D, 0x0E, 0x0F, 0x10
				}
				else
				{
					ulStatus |= EVT_CPNT_VID_25HZ;				// Component Type 0x01, 0x2, 0x03, 0x04, 0x09, 0x0A, 0x0B, 0x0C
				}

				if ((ucCompType & 0x08) == 0x08)
				{
					ulStatus |= EVT_CPNT_VID_HD;					// Component Type 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
				}
			}
			break;

		case eEsStreamContent_H264_Video:		// H.264 VIDEO
			if (pstCompDes->ucCompType >= 0x01 && pstCompDes->ucCompType <= 0x10)
			{
				ulStatus |= EVT_CPNT_VID_H264;
				ucCompType = pstCompDes->ucCompType - 1;

				if ((ucCompType & 0x03) == 0x00)
				{
					ulStatus |= EVT_CPNT_VID_ASPECT_4_3;			// Component Type 0x01, 0x05, 0x09, 0x0D
				}
				else if ((ucCompType & 0x03) != 0x03)
				{
					ulStatus |= EVT_CPNT_VID_ASPECT_16_9;			// Component Type 0x02, 0x03, 0x06, 0x07, 0x0A, 0x0B, 0x0E, 0x0F
				}

				if ((ucCompType & 0x04) == 0x04)
				{
					ulStatus |= EVT_CPNT_VID_30HZ;					// Component Type 0x05, 0x06, 0x07, 0x08, 0x0D, 0x0E, 0x0F, 0x10
				}
				else
				{
					ulStatus |= EVT_CPNT_VID_25HZ;					// Component Type 0x01, 0x2, 0x03, 0x04, 0x09, 0x0A, 0x0B, 0x0C
				}

				if ((ucCompType & 0x08) == 0x08)
				{
					ulStatus |= EVT_CPNT_VID_HD;					// Component Type 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
				}

				// Reset the status if the component type is reserved.
				ucCompType++;

				if (ucCompType == 0x02 || ucCompType == 0x06 || ucCompType == 0x09 ||
					ucCompType == 0x0A || ucCompType == 0x0D || ucCompType == 0x0E)
				{
					ulStatus = 0;
				}
			}
			break;

		default:
			// nothing to do
			break;
	}

	*pulStatus |= ulStatus;

	return ERR_OK;
}

STATIC HERROR	svc_si_LibSetAudioComponentStatusByComponentType (SIxCompDes_t *pstCompDes, HUINT32 *pulStatus)
{
	HUINT32		 ulStatus;

	if (NULL == pstCompDes)		{ return ERR_FAIL; }
	if (NULL == pulStatus)		{ return ERR_FAIL; }

	ulStatus = 0;

	switch (pstCompDes->ucStmContent)
	{
		case eEsStreamContent_MPEG1_Audio:		// MPEG1 Layer 2 Audio
			switch (pstCompDes->ucCompType)
			{
				case 0x01:
					ulStatus |= EVT_CPNT_AUD_SINGLEMONO;
					break;
				case 0x02:
					ulStatus |= EVT_CPNT_AUD_DUALMONO;
					break;
				case 0x03:
					ulStatus |= EVT_CPNT_AUD_STEREO;
					break;
				case 0x04:
					ulStatus |= EVT_CPNT_AUD_MULTILINGUAL;
					break;
				case 0x05:
					ulStatus |= EVT_CPNT_AUD_SURROUND;
					break;
			}
			break;

		case eEsStreamContent_AC3_Audio:		// AC3 Audio
			ulStatus |= EVT_CPNT_AUD_DOLBY;
			break;

		default:
			// nothing to do
			break;
	}

	if (*pulStatus != 0 && ulStatus != 0)
	{
		ulStatus |= EVT_CPNT_AUD_MULTI_TRACK;
	}

	*pulStatus |= ulStatus;

	return ERR_OK;
}

STATIC HERROR	svc_si_LibSetSubtitleComponentStatusByComponentType (SIxCompDes_t *pstCompDes, HUINT32 *pulStatus)
{
	HUINT32		 ulStatus;

	if (NULL == pstCompDes)		{ return ERR_FAIL; }
	if (NULL == pulStatus)		{ return ERR_FAIL; }

	ulStatus = 0;

	switch (pstCompDes->ucStmContent)
	{
		case eEsStreamContent_Subtitle:		// Subtitle & Teletext
			switch (pstCompDes->ucCompType)
			{
				case 0x01:
				case 0x10:
				case 0x11:
				case 0x12:
				case 0x13:
				case 0x20:
				case 0x21:
				case 0x22:
				case 0x23:
				case 0x24:
					ulStatus |= EVT_CPNT_SUBTITLE;
					break;
			}
			break;

		default:
			// nothing to do
			break;
	}

	*pulStatus |= ulStatus;

	return ERR_OK;
}

//< pszCountryCode 	- [IN] ISO3166 code
//< pucDesRaw 		- [IN] descriptor raw
//< pucRating 		- [OUT] parental rating
STATIC HERROR svc_si_LibGetParentalRatingFromDesRaw(HUINT32 ulActionId, const HUINT8 *pszCountryCode, const HUINT8 *pucDesRaw, HUINT8 *pucRating)
{
	HERROR			hErr = ERR_FAIL;
	SIxParentDes_t	*pstDesc;
	SIxParentInfo_t	*pstParentInfo = NULL;
	HUINT8			szTmpCountryCode[4];

	if ((pszCountryCode == NULL)
		|| (pucDesRaw == NULL)
		|| (pucRating == NULL))
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (pucDesRaw[0] != eSIxDESCTAG_PARENTAL_RATING)
	{
		HxLOG_Error ("Invalid des raw (0x%X) \n", pucDesRaw[0]);
		return ERR_FAIL;
	}

	*pucRating = 0x00;
	pstDesc = (SIxParentDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
	if (pstDesc != NULL)
	{
		pstParentInfo = pstDesc->pstInfo;
		while (pstParentInfo != NULL)
		{
			HxSTD_MemCopy (szTmpCountryCode, pstParentInfo->szCountryCode, sizeof(pstParentInfo->szCountryCode));
			HLIB_STD_StrLower(szTmpCountryCode);

			// country code가 맞는 rating을 우선
			if (HxSTD_MemCmp (pszCountryCode, szTmpCountryCode, sizeof(szTmpCountryCode)) == 0)
			{
				*pucRating = pstParentInfo->ucRate;
				hErr = ERR_OK;
				break;
			}

			pstParentInfo = pstParentInfo->pstNext;
		}

		// country code가 맞는 rating이 없다면 첫번째 rating으로..
		if ((hErr != ERR_OK) && (pstDesc->pstInfo != NULL))
		{
			*pucRating = pstDesc->pstInfo->ucRate;
			hErr = ERR_OK;
		}

		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
	}

	return hErr;
}

//< pucDesRaw 		- [IN] descriptor raw
//< pucContent 		- [OUT] content nibble
//< pucContent2 	- [OUT] user nibble
STATIC HERROR svc_si_LibGetContentFromDesRaw(HUINT32 ulActionId, const HUINT8 *pucDesRaw, HUINT8 *pucContent, HUINT8 *pucContent2)
{
	HERROR			hErr = ERR_FAIL;
	SIxContentDes_t	*pstDesc = NULL;

	if ((pucDesRaw == NULL)
		|| (pucContent == NULL)
		|| (pucContent2 == NULL))
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (pucDesRaw[0] != eSIxDESCTAG_CONTENT)
	{
		HxLOG_Error ("Invalid des raw (0x%X) \n", pucDesRaw[0]);
		return ERR_FAIL;
	}

	*pucContent = *pucContent2 = 0x00;
	pstDesc = (SIxContentDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
	if (pstDesc != NULL)
	{
		*pucContent = pstDesc->pstInfo->ucContentNibble;
		*pucContent2 = pstDesc->pstInfo->ucUserNibble;

		hErr = ERR_OK;
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
	}

	return hErr;
}

//< pszLangCode 	- [IN] ISO639 code
//< pucDesRaw 		- [IN] descriptor raw
//< pszEvtName 		- [OUT] event name
//< pszEvtText 		- [OUT] event text
// <pbMatched		- [OUT] is language matched
STATIC HERROR svc_si_LibGetShortEventFromDesRaw(HUINT32 ulActionId, const HUINT8 *pszLangCode, const HUINT8 *pucDesRaw, HUINT8 *pszEvtName, HUINT8 *pszEvtText, HBOOL *pbMatched)
{
	HERROR			hErr = ERR_FAIL;
	SIxShortEvtDes_t	*pstDesc = NULL;
	HUINT32			ulSize = 0;

	if ((pucDesRaw == NULL)
		|| (pszLangCode == NULL)
		|| (pszEvtName == NULL)
		|| (pszEvtText == NULL))
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (pucDesRaw[0] != eSIxDESCTAG_SHORT_EVENT)
	{
		HxLOG_Error ("Invalid des raw (0x%X) \n", pucDesRaw[0]);
		return ERR_FAIL;
	}

	pstDesc = (SIxShortEvtDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
	if (pstDesc != NULL)
	{
		if (HxSTD_MemCmp(pszLangCode, pstDesc->szIso639LangCode, sizeof(pstDesc->szIso639LangCode)) == 0)
		{
			*pbMatched = TRUE;
		}
		else
		{
			*pbMatched = FALSE;
		}

		ulSize = (pstDesc->ucEvtNameSize < SvcSi_MAX_SHORT_EVENT_NAME) ? pstDesc->ucEvtNameSize : SvcSi_MAX_SHORT_EVENT_NAME;
		HxSTD_MemCopy (pszEvtName, pstDesc->pszEvtName, ulSize);

		ulSize = (pstDesc->ucTextSize < SvcSi_MAX_EVENT_TEXT) ? pstDesc->ucTextSize : SvcSi_MAX_EVENT_TEXT;
		HxSTD_MemCopy (pszEvtText, pstDesc->pszText, ulSize);

		hErr = ERR_OK;
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
	}

	return hErr;
}


//< pszLangCode 	- [IN] ISO639 code
//< pucDesRaw 		- [IN] descriptor raw
//< pstExtEvent 		- [OUT] ext event
STATIC HERROR svc_si_LibGetExtEventFromDesRaw(HUINT32 ulActionId, const HUINT8 *pszLangCode, const HUINT8 *pucDesRaw, SvcSi_ExtEvent_t *pExtEvent)
{
	HERROR			hErr = ERR_FAIL;
	SIxExtEvtDes_t	*pstDesc = NULL;
	HUINT32			ulSize = 0;

	if ((pucDesRaw == NULL)
		|| (pszLangCode == NULL)
		|| (pExtEvent == NULL))
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (pucDesRaw[0] != eSIxDESCTAG_EXTENDED_EVENT)
	{
		HxLOG_Error ("Invalid des raw (0x%X) \n", pucDesRaw[0]);
		return ERR_FAIL;
	}

	pstDesc = (SIxExtEvtDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
	if (pstDesc != NULL)
	{
		pExtEvent->bAvailable	=	TRUE;
		pExtEvent->ucDesNum		=	pstDesc->ucDesNum;

		HxSTD_MemCopy (pExtEvent->aucLang, pstDesc->szIso639LangCode, sizeof(HUINT8) * 4);

		// pszChar는 UTF-8로 변환된 String이다.
		ulSize = HxSTD_StrLen(pstDesc->pszChar);

		pExtEvent->szTextChar	=	(HUINT8*)OxSI_Calloc( (ulSize+1) * sizeof(HUINT8) );

		HxSTD_MemCopy (pExtEvent->szTextChar, pstDesc->pszChar, ulSize);
		pExtEvent->szTextChar[ulSize] = '\0';

		hErr = ERR_OK;
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
	}

	return hErr;
}

//< pucDesRaw 		- [IN] descriptor raw
//< pstCompDes 	- [OUT] component descriptor
STATIC HERROR svc_si_LibGetComponentFromDesRaw(HUINT32 ulActionId, const HUINT8 *pucDesRaw, SIxCompDes_t *pstCompDes)
{
	HERROR			hErr = ERR_FAIL;
	SIxCompDes_t		*pstDesc = NULL;

	if ((pucDesRaw == NULL)
		|| (pstCompDes == NULL))
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (pucDesRaw[0] != eSIxDESCTAG_COMPONENT)
	{
		HxLOG_Error ("Invalid des raw (0x%X) \n", pucDesRaw[0]);
		return ERR_FAIL;
	}

	pstDesc = (SIxCompDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
	if (pstDesc != NULL)
	{
		HxSTD_MemCopy (pstCompDes, pstDesc, sizeof(SIxCompDes_t));

		hErr = ERR_OK;
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
	}

	return hErr;
}

//< pucDesRaw 		- [IN] descriptor raw
//< peCasType 		- [OUT] cas type
STATIC HERROR svc_si_LibGetCaIdentifierFromDesRaw(HUINT32 ulActionId, const HUINT8 *pucDesRaw, DxCasType_e *peCasType)
{
	HERROR			hErr = ERR_FAIL;
	SIxCaIdentDes_t	*pstDesc = NULL;
	SIxCaIdentInfo_t	*pstCaIdInfo = NULL;
	DxCasType_e			eCasType = 0;

	if ((pucDesRaw == NULL)
		|| (peCasType == NULL))
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (pucDesRaw[0] != eSIxDESCTAG_CA_IDENTIFIER)
	{
		HxLOG_Error ("Invalid des raw (0x%X) \n", pucDesRaw[0]);
		return ERR_FAIL;
	}

	pstDesc = (SIxCaIdentDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
	if (pstDesc != NULL)
	{
		*peCasType = 0;
		pstCaIdInfo = pstDesc->pstInfo;

		while (pstCaIdInfo != NULL)
		{
			eCasType = 0;
			svc_si_LibConvertSiCasIdToCasType((HUINT32)pstCaIdInfo->usCaStmId, &eCasType);
			*peCasType |= eCasType;

			pstCaIdInfo = pstCaIdInfo->pstNext;
		}

		hErr = ERR_OK;
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
	}

	return hErr;
}

STATIC HERROR svc_si_LibGetEvtComponentStatusFromPmtEsInfo (HUINT32 ulActionId, svcSi_PmtEsInfo_t *pstPmtEsInfo, SvcSi_Event_t *pstEvent)
{
	HxList_t			*pstItem = NULL;
	svcSi_VideoEs_t		*pstVideoEs = NULL;
	svcSi_AudioEs_t		*pstAudioEs = NULL;

	if ((NULL == pstPmtEsInfo) || (NULL == pstEvent))
	{
		return ERR_FAIL;
	}

	// 6-1. component status (video/audio/ttx/sbtl)
	if (pstPmtEsInfo->pstVideoTrack != NULL)
	{
		pstItem = pstPmtEsInfo->pstVideoTrack;
		while(pstItem != NULL)
		{
			pstVideoEs = (svcSi_VideoEs_t *)HLIB_LIST_Data (pstItem);

			if ((NULL != pstVideoEs)
				&& (PID_NULL != pstVideoEs->usPid)
				&& (pstVideoEs->eVideoCodec == eDxVIDEO_CODEC_H264))
			{
				pstEvent->ulVidComponentStatus |= EVT_CPNT_VID_H264;

				// 한번 체크하면 두번 할 필요가 없으니 break
				break;
			}
			pstItem = pstItem->next;
		}
	}

	if (pstPmtEsInfo->pstAudioTrack != NULL)
	{
		HxList_t		*pstItem;

		pstItem = pstPmtEsInfo->pstAudioTrack;
		while(pstItem != NULL)
		{
			pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (pstItem);

			if ((NULL != pstAudioEs) && (PID_NULL != pstAudioEs->usPid))
			{
				if ((pstAudioEs->eAudCodec == eDxAUDIO_CODEC_DOLBY_AC3) || (pstAudioEs->eAudCodec == eDxAUDIO_CODEC_DOLBY_AC3P))
				{
					pstEvent->ulAudComponentStatus |= EVT_CPNT_AUD_DOLBY;

					// 한번 체크하면 두번 할 필요가 없으니 break
					break;
				}
			}
			pstItem = pstItem->next;
		}
	}

	if (pstPmtEsInfo->pstTeltxtTrack != NULL)
	{
		pstEvent->ulAuxComponentStatus |= EVT_CPNT_TELETEXT;
	}

	if (pstPmtEsInfo->pstSubttlTrack != NULL)
	{
		pstEvent->ulAuxComponentStatus |= EVT_CPNT_SUBTITLE;
	}

	return ERR_OK;
}

//< pucDesRaw 			- [IN] descriptor raw
//< pstFtaContentMgmt 	- [OUT] fta content management descriptor
STATIC HERROR svc_si_LibGetFtaContentMgmtFromDesRaw(HUINT32 ulActionId, const HUINT8 *pucDesRaw, SIxFtaContentManagementDes_t *pstFtaContentMgmt)
{
	HERROR								 hErr = ERR_FAIL;
	SIxFtaContentManagementDes_t		*pstDesc = NULL;

	if ((pucDesRaw == NULL)
		|| (pstFtaContentMgmt == NULL))
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (pucDesRaw[0] != eSIxDESCTAG_FTA_CONTENT_MANAGEMENT)
	{
		HxLOG_Error ("Invalid des raw (0x%X) \n", pucDesRaw[0]);
		return ERR_FAIL;
	}

	pstDesc = (SIxFtaContentManagementDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucDesRaw);
	if (pstDesc != NULL)
	{
		HxSTD_MemCopy (pstFtaContentMgmt, pstDesc, sizeof(SIxFtaContentManagementDes_t));

		hErr = ERR_OK;
		PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
	}

	return hErr;
}

STATIC SIxDescriptor_t* svc_si_LibParseDesFromDesRaw(HUINT32 ulActionId, HUINT8 *pucDesRaw)
{
	HERROR						 hErr;
	SIxDescriptor_t 			*pstDes = NULL;
	SIxDescriptorHeaderParam_t	 stDesParam;
	SIxSpec_e					 eSpec;
	SIxTextEncoding_e			 eTextEnc;

	hErr = PAL_SIRAWPOOL_GetSiProperty(ulActionId, &eSpec, &eTextEnc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PAL_SIRAWPOOL_GetSiProperty() failed..!! \n");
		return NULL;
	}

	if ((eSpec == eSIxSPEC_DVB) && (pucDesRaw[0] >= eSIxDESCTAG_PRIVATE_START))
	{
		// private desc.는 dvb spec으로 parsing할 수 없으므로 예외처리
		return NULL;
	}

	if (pucDesRaw[0] < eSIxDESCTAG_PRIVATE_START)
	{
		// dvb에서 정의된 desc는  dvb spec으로 처리
		eSpec = eSIxSPEC_DVB;
	}

	stDesParam.eTextEnc = eTextEnc;
	stDesParam.pszArg = NULL;

	hErr = SILIB_DESC_New ((const SIxDescriptorTag_e)pucDesRaw[0], (const SIxSpec_e)eSpec, (const SIxDescriptorHeaderParam_t *)&stDesParam, pucDesRaw, &pstDes);
	if ((ERR_OK != hErr) || (NULL == pstDes))
	{
		HxLOG_Error ("SILIB_DESC_New err: DescTag=(0x%X)\n", pucDesRaw[0]);
		return NULL;
	}

	return pstDes;
}

STATIC void svc_si_LibFreeDesList (void *pvData)
{
	SIxDescriptor_t		*pstDes = (SIxDescriptor_t *)pvData;

	if (NULL != pstDes)
	{
		SILIB_DESC_Delete (pstDes);
	}

	return;
}

STATIC HERROR svc_si_LibSapiGetValidCrid (HUINT8 *szSrcCridStr, HUINT32 ulSrcCridLen, HUINT8 *szDefaultAuth, HUINT8 *szValidCridStr)
{
	return SAPI_GetValidCrid(szSrcCridStr,ulSrcCridLen, szDefaultAuth, szValidCridStr);
}

STATIC HERROR svc_si_LibCheckValidRctLinkInfo(Handle_t hSvc, SIxTable_RctLinkInfo_t *pstLinkInfo)
{
	HUINT32	ulLen, k;
	HBOOL	bChked;
	HUINT8	szDefAuth[DxMAX_DEFAULT_AUTHORITY_LEN];
	HUINT8	szValidCrId[DxRSV_CRID_LEN+1];

	if (pstLinkInfo == NULL)
	{
		HxLOG_Error("pstLinkInfo is NULL \n");
		return ERR_FAIL;
	}

	/* Only link_type 0x00 shall be used. (D-BOOK 6.2 8.12.8) */
	if(pstLinkInfo->ucLinkType != eDxSVC_RctLinkType_Url)
	{
		HxLOG_Error("Only Support eDxSVC_RctLinkType_Url (0x%x) \n", pstLinkInfo->ucLinkType);
		return ERR_FAIL;
	}

	/* Only how_related_classification_scheme_id 0x02 shall be used. (D-BOOK 6.2 8.12.10)*/
	if(pstLinkInfo->ucHowSchemeId!= eDxSVC_RctClassificationScheme_2007)
	{
		HxLOG_Error("Only support eDxSVC_RctClassificationScheme_2007 (0x%x)\n", pstLinkInfo->ucHowSchemeId);
		return ERR_FAIL;
	}

	if(pstLinkInfo->usTermId != eDxSVC_RctTermId_Programme && pstLinkInfo->usTermId != eDxSVC_RctTermId_Series)
	{
		HxLOG_Error("Invalid Term Id (0x%04x) \n", pstLinkInfo->usTermId );
		return ERR_FAIL;
	}

	if(pstLinkInfo->pucMediaUriByte == NULL)
	{
		HxLOG_Error("pszMediaUri Length NULL\n");
		return ERR_FAIL;
	}

	ulLen = pstLinkInfo->ucMediaUriLen;
	if(ulLen == 0)
	{
		HxLOG_Error("pszMediaUri Length Zero\n");
		return ERR_FAIL;
	}

	/* CRID 유효성 검사 */
	bChked = FALSE;
	MWC_UTIL_ConvertStringUpper(pstLinkInfo->pucMediaUriByte);
	for(k = 0; k < ulLen; ++k)
	{
		if ((pstLinkInfo->pucMediaUriByte[k] < 0x20)
			|| (pstLinkInfo->pucMediaUriByte[k] > 0x7F))
		{
			break;
		}

		/* CRID에 /는 반듯이 하나 이상 들어가야 한다 */
		if(pstLinkInfo->pucMediaUriByte[k] == '/')
		{
			bChked = TRUE;
		}
	}

	if(k < ulLen || bChked == FALSE)
	{
		HxLOG_Error("Invalid CRID\n");
		return ERR_FAIL;
	}

	/* Extract CRID */
	HxSTD_MemSet(szDefAuth, 0, sizeof(HUINT8) * (DxMAX_DEFAULT_AUTHORITY_LEN));
	DB_SVC_GetDefaultAuthority(hSvc, szDefAuth);
	if(ERR_OK  != svc_si_LibSapiGetValidCrid(pstLinkInfo->pucMediaUriByte, pstLinkInfo->ucMediaUriLen, szDefAuth,  szValidCrId))
	{
		HxLOG_Error("Invalid CRID structure\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_LibGetShortEvtInRct(HUINT32 ulActionId, HxList_t *pstDesLoop, HBOOL *pbFound, DxService_RctShortEvtInfo_t *pstDxShortEvtInfo)
{
	HxList_t				*pstCurrLoop = NULL, *pstShortEvtLoop = NULL;
	HxLANG_Id_e			eMainLangId = eLangID_MAX, eEventLangId =eLangID_MAX;
	HUINT32				ulPriority = 1, ulTopPriority = 0;
	HUINT32				ulSize = 0;

	if(pstDesLoop == NULL)
	{
		//HxLOG_Warning("pstDesLoop is NULL\n");
		return ERR_FAIL;
	}

	(void)DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMainLangId, 0);

	pstShortEvtLoop = pstDesLoop;
	for(pstCurrLoop = pstShortEvtLoop;pstCurrLoop != NULL; pstCurrLoop = pstCurrLoop->next)
	{
		HUINT8	*pucRawDes = (HUINT8*)HLIB_LIST_Data(pstCurrLoop);

		if(*pucRawDes == eSIxDESCTAG_SHORT_EVENT)
		{
			SIxShortEvtDes_t *pstShortEvtDes = (SIxShortEvtDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstShortEvtDes)
			{
				eEventLangId = HLIB_LANG_639ToId(pstShortEvtDes->szIso639LangCode);

				if(eEventLangId == eMainLangId)
				{
					ulPriority = 50;
				}
				else if(eEventLangId == eLangID_OriginalAudio || eEventLangId == eLangID_Undefined)
				{
					ulPriority = 30;
				}
				else if(eEventLangId == eLangID_English)
				{
					ulPriority = 10;

				}

				if(ulTopPriority<ulPriority)
				{
					ulTopPriority = ulPriority;

					HxSTD_StrNCpy(pstDxShortEvtInfo->szLangCode, pstShortEvtDes->szIso639LangCode, DxLANG_CODE_LEN);

					ulSize = (pstShortEvtDes->ucEvtNameSize < SvcSi_MAX_SHORT_EVENT_NAME) ? pstShortEvtDes->ucEvtNameSize : SvcSi_MAX_SHORT_EVENT_NAME;
					HxSTD_StrNCpy(pstDxShortEvtInfo->szEventName, pstShortEvtDes->pszEvtName, ulSize);

					ulSize = (pstShortEvtDes->ucTextSize < SvcSi_MAX_EVENT_TEXT) ? pstShortEvtDes->ucTextSize : SvcSi_MAX_EVENT_TEXT;
					HxSTD_StrNCpy(pstDxShortEvtInfo->szEventText, pstShortEvtDes->pszText, ulSize);

					*pbFound = TRUE;
				}

				HxLOG_Debug("LangCode (%s) EvtName(%s) EvtText(%s) \n", pstShortEvtDes->szIso639LangCode, pstShortEvtDes->pszEvtName, pstShortEvtDes->pszText);

				if(pstShortEvtDes != NULL)
					PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *) pstShortEvtDes);
			}
		}
	}

	return ERR_OK;
}
STATIC HERROR svc_si_LibGetImageIconInRct(HUINT32 ulActionId, HxList_t *pstDesLoop, HUINT8 ucIconIdInLink, HBOOL *pbFound, DxService_RctImageIconInfo_t *pstDxImageInfo)
{
	HxList_t	*pstCurrLoop = NULL, *pstImgDescLoop = NULL;
	HUINT32	ulLen = 0;

	*pbFound = FALSE;

	if(pstDesLoop == NULL)
	{
		//HxLOG_Warning("pstDesLoop is NULL\n");
		return ERR_FAIL;
	}

	pstImgDescLoop = pstDesLoop;
	for(pstCurrLoop = pstImgDescLoop;pstCurrLoop != NULL; pstCurrLoop = pstCurrLoop->next)
	{
		HUINT8	*pucRawDes = (HUINT8*)HLIB_LIST_Data(pstCurrLoop);

		if(*pucRawDes == eSIxDESCTAG_EXTENSION)
		{
			SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstExtensionDes)
			{
				if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_IMG_ICON)
				{
					SIxImageIconDes_t *pstImgIconDes = (SIxImageIconDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);

					if(NULL != pstImgIconDes)
					{
						if((pstImgIconDes->ucIconId == ucIconIdInLink)
							&& (pstImgIconDes->ucDesNum <= pstImgIconDes->ucLastDesNum))
						{
							pstDxImageInfo->ucIconId = ucIconIdInLink;

							*pbFound = TRUE;

							if(0x00 == pstImgIconDes->ucDesNum)
							{
								if(pstImgIconDes->pstImgHeadInfo)
								{
									pstDxImageInfo->ucTransPortMode = pstImgIconDes->pstImgHeadInfo->ucIconTransMode;
									pstDxImageInfo->ucPositionFlag = pstImgIconDes->pstImgHeadInfo->ucIconPosFlag;
									if(pstDxImageInfo->ucPositionFlag)
									{
										pstDxImageInfo->ucCoordinateSystem = pstImgIconDes->pstImgHeadInfo->ucCoordSystem;
										pstDxImageInfo->usHorizontalOrigin = pstImgIconDes->pstImgHeadInfo->usIconHorOrigin;
										pstDxImageInfo->usVerticalOrigin = pstImgIconDes->pstImgHeadInfo->usIconVerOrigin;
									}

									if(pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo->pucImgType)
									{
										if(pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo->usTypeLen > DxRCT_MAX_IMAGE_ICON_TYPE_CHART)
										{
											ulLen = DxRCT_MAX_IMAGE_ICON_TYPE_CHART;
										}
										else
										{
											ulLen = pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo->usTypeLen;
										}

										HxSTD_StrNCpy(pstDxImageInfo->szIconTypeChar,
													pstImgIconDes->pstImgHeadInfo->pstImgTypeInfo->pucImgType,
													ulLen);
									}

									//The icon is delivered in the icon_data_bytes
									if(pstDxImageInfo->ucTransPortMode == 0x00
										&& pstImgIconDes->pstImgHeadInfo->pstImgIconInfo
										&& pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->pucImgData
										&& pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->ucDataLen > 0)
									{
										HUINT8 *pstTemp = NULL;

										pstTemp = pstDxImageInfo->pucDatabyte;
										pstDxImageInfo->pucDatabyte = OxSI_Calloc(pstDxImageInfo->ulDataByteSize
																				+ pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->ucDataLen);
										if(pstDxImageInfo->pucDatabyte)
										{
											if(pstTemp && pstDxImageInfo->ulDataByteSize > 0)
											{
												HxSTD_MemCopy(pstDxImageInfo->pucDatabyte,
																pstTemp,
																pstDxImageInfo->ulDataByteSize);

											}

											HxSTD_MemCopy(pstDxImageInfo->pucDatabyte + pstDxImageInfo->ulDataByteSize,
															pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->pucImgData,
															pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->ucDataLen);
											pstDxImageInfo->ulDataByteSize += pstImgIconDes->pstImgHeadInfo->pstImgIconInfo->ucDataLen;
										}

										if(pstTemp)
										{
											OxSI_Free(pstTemp);
										}
									}
									//The location of the icon file is identified by URL carried in the url_char sequence of bytes.
									else if(pstDxImageInfo->ucTransPortMode == 0x01
											&& pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo
											&& pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo->pucImgUrl)
									{
										if(pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo->usUrlLen > DxRCT_MAX_IMAGE_ICON_URL_LEN)
										{
											ulLen = DxRCT_MAX_IMAGE_ICON_URL_LEN;
										}
										else
										{
											ulLen = pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo->usUrlLen ;
										}

										HxSTD_StrNCpy(pstDxImageInfo->szIconUrl,
													pstImgIconDes->pstImgHeadInfo->pstImgUrlInfo->pucImgUrl,
													ulLen);
									}
								}
							}
							else
							{
								if(pstImgIconDes->pstImgIconInfo
									&& pstImgIconDes->pstImgIconInfo->pucImgData
									&& pstImgIconDes->pstImgIconInfo->ucDataLen > 0)
								{
									HUINT8 *pstTemp = NULL;

									pstTemp = pstDxImageInfo->pucDatabyte;
									pstDxImageInfo->pucDatabyte = OxSI_Calloc(pstDxImageInfo->ulDataByteSize
																			+ pstImgIconDes->pstImgIconInfo->ucDataLen);
									if(pstDxImageInfo->pucDatabyte)
									{
										if(pstTemp && pstDxImageInfo->ulDataByteSize > 0)
										{
											HxSTD_MemCopy(pstDxImageInfo->pucDatabyte,
															pstTemp,
															pstDxImageInfo->ulDataByteSize);

										}

										HxSTD_MemCopy(pstDxImageInfo->pucDatabyte + pstDxImageInfo->ulDataByteSize,
														pstImgIconDes->pstImgIconInfo->pucImgData,
														pstImgIconDes->pstImgIconInfo->ucDataLen);
										pstDxImageInfo->ulDataByteSize += pstImgIconDes->pstImgIconInfo->ucDataLen;
									}

									if(pstTemp)
									{
										OxSI_Free(pstTemp);
									}
								}
							}
							PAL_SIRAWPOOL_FreeDes((SIxDescriptor_t *)pstImgIconDes);
						}
					}
				}
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);
			}

		}
	}

	HxLOG_Debug("pbFound (%d) \n", *pbFound);

	return ERR_OK;
}


STATIC void svc_si_LibCopyImageIconInfo(Handle_t hAction, DxService_RctImageIconInfo_t *pstSrcImg, DxService_RctImageIconInfo_t *pstDstImg)
{
	pstDstImg->ucIconId = pstSrcImg->ucIconId ;
	pstDstImg->ucTransPortMode = pstSrcImg->ucTransPortMode ;
	pstDstImg->ucPositionFlag = pstSrcImg->ucPositionFlag ;
	pstDstImg->ucCoordinateSystem = pstSrcImg->ucCoordinateSystem ;
	pstDstImg->usHorizontalOrigin = pstSrcImg->usHorizontalOrigin ;
	pstDstImg->usVerticalOrigin = pstSrcImg->usVerticalOrigin ;

	HxSTD_MemCopy(pstDstImg->szIconTypeChar, pstSrcImg->szIconTypeChar, DxRCT_MAX_IMAGE_ICON_TYPE_CHART + 1);

	pstDstImg->ulDataByteSize = pstSrcImg->ulDataByteSize ;
	pstDstImg->pucDatabyte = (HUINT8*)OxSI_Calloc(sizeof(HUINT8) * pstDstImg->ulDataByteSize);
	if(	pstDstImg->ucIconId != 0
		&&pstDstImg->ucTransPortMode == 0x00
		&&pstDstImg->ulDataByteSize > 0
		&& pstDstImg->pucDatabyte)
	{
		HCHAR	pszSrcPath[DxRCT_IMAGE_PATH_LEN];

		HxSTD_MemCopy(pstDstImg->pucDatabyte, pstSrcImg->pucDatabyte, sizeof(HUINT8) * pstDstImg->ulDataByteSize);

		HxSTD_memset(pszSrcPath, 0x0, DxRCT_IMAGE_PATH_LEN);
		HxSTD_snprintf(pszSrcPath, DxRCT_IMAGE_PATH_LEN,"%s", DxRCT_IMAGE_NAME);

		if (HLIB_FILE_Exist(pszSrcPath) == TRUE)
		{
			HLIB_FILE_Delete(pszSrcPath);
		}

		HxSTD_memset(pszSrcPath, 0x0,DxRCT_IMAGE_PATH_LEN);

		if(HLIB_STD_StrCaseStr(pstDstImg->szIconTypeChar, "png") != NULL)
		{
			HxSTD_snprintf(pszSrcPath, DxRCT_IMAGE_PATH_LEN,"%s.%s", DxRCT_IMAGE_NAME, "png");
			HLIB_FILE_WriteFile(pszSrcPath, (void *)pstDstImg->pucDatabyte, (HINT32)pstDstImg->ulDataByteSize);
		}
		else if(HLIB_STD_StrCaseStr(pstDstImg->szIconTypeChar, "jpeg") != NULL
			|| HLIB_STD_StrCaseStr(pstDstImg->szIconTypeChar, "jpg") != NULL)
		{
			HxSTD_snprintf(pszSrcPath, DxRCT_IMAGE_PATH_LEN,"%s.%s", DxRCT_IMAGE_NAME, "jpg");
			HLIB_FILE_WriteFile(pszSrcPath, (void *)pstDstImg->pucDatabyte, (HINT32)pstDstImg->ulDataByteSize);
		}
		else
		{
			//return;
		}

		HxLOG_Debug("pszSrcPath(%s) \n", pszSrcPath);
	}

	HxSTD_MemCopy(pstDstImg->szIconUrl, pstSrcImg->szIconUrl, DxRCT_MAX_IMAGE_ICON_URL_LEN + 1);

	// mheg에서 dsmcc로 image 받도록 noti 보냄.
	if((pstDstImg->ucIconId != 0)
		&& (pstDstImg->ucTransPortMode == 0x01))
	{
		if((HxSTD_StrLen(pstDstImg->szIconUrl) > 0) && (HLIB_STD_StrCaseStr(pstDstImg->szIconUrl, "dvb://") || HLIB_STD_StrCaseStr(pstDstImg->szIconUrl, "dvb:/") ))
		{
			//  webapp 과 sync 문제 때문에. Dsmcc 로 오는 RCT Image Icon을 mheg에서 file write 하기 전에 지우던 것을 Rct Info를 만들 때 지우도록 수정함.
			HCHAR	pszSrcPath[DxRCT_IMAGE_PATH_LEN];

			HxSTD_memset(pszSrcPath, 0x0, DxRCT_IMAGE_PATH_LEN);
			HxSTD_snprintf(pszSrcPath, DxRCT_IMAGE_PATH_LEN,"%s.%s", DxRCT_IMAGE_NAME, "dvb");

			if (HLIB_FILE_Exist(pszSrcPath) == TRUE)
			{
				HxLOG_Debug(" Delete pszSrcPath (%s) systemTick (%d)\n", pszSrcPath, HLIB_STD_GetSystemTick());
				HLIB_FILE_Delete(pszSrcPath);
			}

			svc_si_PostDataToAp (eSEVT_SI_REQUEST_RCT_IMAGE_BY_MHEG, hAction, 0, 0, (void *)pstDstImg->szIconUrl, DxRCT_MAX_IMAGE_ICON_URL_LEN + 1);
			HxLOG_Debug("Send Message to Mheg (eSEVT_SI_REQUEST_RCT_IMAGE_BY_MHEG) ,  IconUrl : %s  \n", pstDstImg->szIconUrl);
		}
	}
}

#define ___MEMBER_FUNCTIONS___
#define _____EVENT_RELATED_____

HERROR	svc_si_LibGetEventInfoFromEitPf (HUINT32 ulActionId, DbSvc_TripleId_t stTripleId, HBOOL bIsPresent, HBOOL bUsePmt, SvcSi_Event_t *pstEvent, SvcSi_ExtEvent_t *pstExtEvent)
{
	HERROR					hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstEitActPfTable = NULL;
	HxList_t				*pstElemItem = NULL;
	HxList_t				*pstDescList = NULL;
	HBOOL				 	bPresentEvtRead, bFollowEvtRead;
	HBOOL					bHaveMatched = FALSE;
	SIxTable_EitEvent_t 		stPresentEvt, stFollowEvt, *pstEitEvent;
	HUINT8					szCountryCode[4],szMenuLang[4];
	HxCountry_e				eCountryId;
	HxLANG_Id_e				eMainLang;
	HINT32					lMatchedCount = 0;

	SIxTable_PsiTable_t		*pstPmtTable = NULL;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = NULL;

	if (NULL == pstEvent)
	{
		return ERR_FAIL;
	}

	// clear pstEvent
	HxSTD_MemSet (pstEvent, 0, sizeof(SvcSi_Event_t));
	pstEvent->ucDoNotScramble = 0xff;
	pstEvent->ucCtrlRAccOverINet = 0xff;
	pstEvent->ucDoNotApplyRevocation = 0xff;

	// get EIT ACT PF table
	pstEitActPfTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_EitActPf, stTripleId.usSvcId);
	if (NULL == pstEitActPfTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, stTripleId.usSvcId);
		goto END_FUNC;
	}

	// check triple id
	if ((pstEitActPfTable->unInfo.stEit.usOrgNetId != stTripleId.usOnId)
		|| (pstEitActPfTable->unInfo.stEit.usTsId!= stTripleId.usTsId)
		|| (pstEitActPfTable->unInfo.stEit.usSvcId!= stTripleId.usSvcId))
	{
		HxLOG_Error ("triple id not matched.. (0x%X, 0x%X, 0x%X VS 0x%X, 0x%X, 0x%X \n",
						pstEitActPfTable->unInfo.stEit.usOrgNetId, pstEitActPfTable->unInfo.stEit.usTsId, pstEitActPfTable->unInfo.stEit.usSvcId,
						stTripleId.usOnId, stTripleId.usTsId, stTripleId.usSvcId);
		goto END_FUNC;
	}

	bPresentEvtRead = bFollowEvtRead = FALSE;
	HxSTD_MemSet (&stPresentEvt, 0, sizeof(SIxTable_EitEvent_t));
	HxSTD_MemSet (&stFollowEvt, 0, sizeof(SIxTable_EitEvent_t));

	//EIT event 정보를 가져옴
	hErr= svc_si_LibGetPfEventFromEit(pstEitActPfTable->pst2ndLoop, &stPresentEvt, &stFollowEvt, &bPresentEvtRead, &bFollowEvtRead);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_LibGetPfEventFromEit() failed.. \n");
		goto END_FUNC;
	}

	// 원하는 p/f event 정보가 없으면 err return
	if ( (bIsPresent == TRUE && bPresentEvtRead == FALSE) || (bIsPresent == FALSE && bFollowEvtRead == FALSE))
	{
		HxLOG_Error ("No Proper Evt Info found \n");
		goto END_FUNC;
	}

	// country code와 menuLang을 미리 가져온다 (desc. parse시에 사용)
//	eCountryId = MWC_PARAM_GetCountryId();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
	}

	if(FALSE == HLIB_COUNTRY_IsValidCountryId(eCountryId))
	{
		HxLOG_Error ("DB_PARAM_GetItem return (eCountryId:0x%X) \n", eCountryId);
		goto END_FUNC;
	}

#if 1
	{
		const HCHAR *pszIsoCountryCode = NULL;
		pszIsoCountryCode = HLIB_COUNTRY_IdTo3166_Alpha3(eCountryId);
		if (pszIsoCountryCode)
		{
			HxSTD_MemCopy(szCountryCode, pszIsoCountryCode, 4);
			hErr = ERR_OK;
		}
		else
		{
			hErr = ERR_FAIL;
		}
	}
#else
	hErr = MWC_UTIL_GetIso3166CodeByCountryId(eCountryId, szCountryCode);
#endif
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("ERROR in MWC_UTIL_GetIso3166CodeByCountryIndex \n");
		goto END_FUNC;
	}
	HLIB_STD_StrLower(szCountryCode);

//	eMainLang = MWC_PARAM_GetMainLanguage();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMainLang, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
	}

	hErr = MWC_UTIL_Get639CodeByLangIdx(eMainLang, &lMatchedCount, szMenuLang, NULL, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("[%s:%d] ERROR in MWC_UTIL_Get639CodeByLangIdx \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}
	HLIB_STD_StrLower(szMenuLang);

	// 1. GET basic EIT value
	if (bIsPresent)
	{
		pstEitEvent = &stPresentEvt;
	}
	else
	{
		pstEitEvent = &stFollowEvt;
	}
	pstEvent->nEvtId 		= pstEitEvent->usEvtId;
	pstEvent->ulStartTime 	= pstEitEvent->utStartTime;
	pstEvent->ulDuration 	= pstEitEvent->utDuration;
	pstEvent->ucRunStatus 	= pstEitEvent->ucRunStatus;

	for (pstElemItem = pstEitActPfTable->pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
	{
		SIxTable_2ndLoop_t		*pst2ndLoop = HLIB_LIST_Data (pstElemItem);

		if (NULL == pst2ndLoop)											{ continue; }
		if (pst2ndLoop->unInfo.stEit.usEvtId != pstEitEvent->usEvtId)	{ continue; }

		for (pstDescList = pst2ndLoop->pstDesLoop; NULL != pstDescList; pstDescList = pstDescList->next)
		{
			HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstDescList);

			switch (*pucDesRaw)
			{
				// 2. parental rating
				case eSIxDESCTAG_PARENTAL_RATING:
					{
						HUINT8 				 ucRating = 0;
						SIxDescriptor_t		*pstDes = NULL;

						// EIT 에 Running Status가 Not Running 이면 Rate는 없는 것으로 간주한다.
						if (pstEitEvent->ucRunStatus == eSVCSI_RunStatus_NotRunning)
						{
							HxLOG_Error("ucRunStatus is eSVCSI_RunStatus_NotRunning! get rating failed..!! \n");
							continue;
						}

						hErr = svc_si_LibGetParentalRatingFromDesRaw(ulActionId, szCountryCode, pucDesRaw, &ucRating);
						if (hErr != ERR_FAIL)
						{
							pstEvent->ucParentRating = ucRating;
						}

						// 모든 rating 정보를 올려달라는 요청이 있어 list로 만들어 추가
						pstDes = svc_si_LibParseDesFromDesRaw(ulActionId, pucDesRaw);
						if (pstDes == NULL)
						{
							HxLOG_Error("svc_si_LibParseDesFromDesRaw() failed..!! \n");
							continue;
						}
						pstEvent->pstRatingDescList = HLIB_LIST_Append(pstEvent->pstRatingDescList, (void *)pstDes);
					}
					break;

				// 3. content nibble
				case eSIxDESCTAG_CONTENT:
					{
						HUINT8 ucContent, ucContent2;
						hErr = svc_si_LibGetContentFromDesRaw(ulActionId, pucDesRaw, &ucContent, &ucContent2);
						if (hErr != ERR_FAIL)
						{
							pstEvent->ucContent = ucContent;
							pstEvent->ucContent2nd = ucContent2;
						}
					}
					break;

				// 4. short event
				case eSIxDESCTAG_SHORT_EVENT:
					{
						HUINT8			szName[SvcSi_MAX_SHORT_EVENT_NAME];
						HUINT8			szText[SvcSi_MAX_EVENT_TEXT];
						HBOOL			bMatched = FALSE;

						HxSTD_MemSet(szName, 0x00, sizeof(szName));
						HxSTD_MemSet(szText, 0x00, sizeof(szText));

						hErr = svc_si_LibGetShortEventFromDesRaw(ulActionId, szMenuLang, pucDesRaw, szName, szText, &bMatched);
						if (bMatched == TRUE)
						{
							bHaveMatched = TRUE;
						}

						if ( (bHaveMatched == TRUE) && (bMatched == FALSE))
						{
							// 2. matchedLang을 찾은 후(have matched)에는 덮어 씌우지 않는다.
							break;
						}

						// 1. default로 항상 덮어 씌운다.

						hErr = svc_si_LibGetShortEventFromDesRaw(ulActionId, szMenuLang, pucDesRaw, szName, szText, &bMatched);
						if (hErr != ERR_FAIL)
						{
							HxSTD_MemCopy(pstEvent->szName, szName, sizeof(pstEvent->szName));
							HxSTD_MemCopy(pstEvent->szText, szText, sizeof(pstEvent->szText));
						}
					}
					break;

				// 5. extended event - 존재 유무flag로 남김, SvcSi_ExtEvent_t 얻기
				case eSIxDESCTAG_EXTENDED_EVENT:
					pstEvent->bExtEvtInfo = TRUE;

					if (NULL != pstExtEvent)
					{
						if(NULL == pstExtEvent->szTextChar)
						{
							hErr = svc_si_LibGetExtEventFromDesRaw(ulActionId, szMenuLang, pucDesRaw, pstExtEvent);
							if(ERR_OK == hErr)
							{
								HxLOG_Debug("(%d) descriptor (%s)\n", pstExtEvent->ucDesNum, pstExtEvent->szTextChar);
							}
						}
						else
						{ /* 덧 붙이기 */
							SvcSi_ExtEvent_t 	stExtEvent;
							HINT32				nNewTextSize = 0;
							HUINT8				*pOldTextChar = NULL;

							HxSTD_MemSet(&stExtEvent, 0x00, sizeof(SvcSi_ExtEvent_t));

							hErr = svc_si_LibGetExtEventFromDesRaw(ulActionId, szMenuLang, pucDesRaw, &stExtEvent);
							if(ERR_OK == hErr)
							{
								pOldTextChar = pstExtEvent->szTextChar;
								nNewTextSize = HxSTD_StrLen(pOldTextChar) + HxSTD_StrLen(stExtEvent.szTextChar) + 1;
								pstExtEvent->szTextChar = (HUINT8 *)OxSI_Calloc( nNewTextSize * sizeof(HUINT8) );
								HxSTD_snprintf(pstExtEvent->szTextChar, nNewTextSize, "%s%s", pOldTextChar, stExtEvent.szTextChar);
								HxLOG_Debug("(%d) descriptor (%s)\n", stExtEvent.ucDesNum, pstExtEvent->szTextChar);
								OxSI_Free(pOldTextChar);
								svc_si_LibFreeExtEventInfo(&stExtEvent);
							}
						}
					}
					break;

				// 6. component - pmt를 안 보는 경우에만 여기서 처리
				case eSIxDESCTAG_COMPONENT:
					if (bUsePmt == FALSE)
					{
						SIxCompDes_t stCompDes;

						HxSTD_MemSet(&stCompDes, 0x00, sizeof(SIxCompDes_t));
						hErr = svc_si_LibGetComponentFromDesRaw(ulActionId, pucDesRaw, &stCompDes);
						if (hErr != ERR_FAIL)
						{
							svc_si_LibSetVideoComponentStatusByComponentType (&stCompDes, &pstEvent->ulVidComponentStatus);
							svc_si_LibSetAudioComponentStatusByComponentType (&stCompDes, &pstEvent->ulAudComponentStatus);
							svc_si_LibSetSubtitleComponentStatusByComponentType (&stCompDes, &pstEvent->ulAuxComponentStatus);
						}
					}
					break;

				// 7. CA identifier - pmt를 안 보는 경우에만 여기서 처리
				case eSIxDESCTAG_CA_IDENTIFIER:
					if (bUsePmt == FALSE)
					{
						DxCasType_e		eCasType = 0;

						hErr = svc_si_LibGetCaIdentifierFromDesRaw(ulActionId, pucDesRaw, &eCasType);
						if (hErr != ERR_FAIL)
						{
							pstEvent->ulCasType |= eCasType;
						}
					}
					break;

				// 8. fta content management
				case eSIxDESCTAG_FTA_CONTENT_MANAGEMENT:
					{
						SIxFtaContentManagementDes_t 	stFtaContentMgmt;

						HxSTD_MemSet(&stFtaContentMgmt, 0x00, sizeof(SIxFtaContentManagementDes_t));
						hErr = svc_si_LibGetFtaContentMgmtFromDesRaw(ulActionId, pucDesRaw, &stFtaContentMgmt);
						if (hErr != ERR_FAIL)
						{
							pstEvent->ucDoNotScramble 			= stFtaContentMgmt.ucDoNotScramble;
							pstEvent->ucCtrlRAccOverINet 		= stFtaContentMgmt.ucCtrlRAccOverINet;
							pstEvent->ucDoNotApplyRevocation 	= stFtaContentMgmt.ucDoNotApplyRevocation;
						}
					}
					break;

				default:

					// TODO: OP specific descriptor는 별도로 처리하도록 한다.

					// TODO: 여기서 private descriptor를 private data specifier 체크도 없이 parsing 하다보면 죽을 수도 있기 때문이다.

					// TODO: 현재 사용되는 곳은 xmgr_recorder_ukdtt.c 뿐이므로 CONFIG_OP_UK_DTT로 우선 막고, 수정 방향 정해지면 재정리 하도록 한다. (2015.07.10)

#ifdef CONFIG_OP_UK_DTT
					// 나머지 descriptor들은 pstEitDescList 에 넣고 xmgr에서 필요시 사용한다.
					{
						SIxDescriptor_t				*pstDes = NULL;

						pstDes = svc_si_LibParseDesFromDesRaw(ulActionId, pucDesRaw);
						if (pstDes == NULL)
						{
							HxLOG_Print("svc_si_LibParseDesFromDesRaw() failed..!! \n");
							continue;
						}

						pstEvent->pstEitDescList = HLIB_LIST_Append(pstEvent->pstEitDescList, (void *)pstDes);
					}
#endif
					break;
			}
		}
	}

	// PMT를 사용해서 event 정보를 채운다
	if (bUsePmt == TRUE)
	{
		pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, stTripleId.usSvcId);
		if (NULL == pstPmtTable)
		{
			HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, stTripleId.usSvcId);
			goto END_FUNC;
		}

		pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc(sizeof(svcSi_PmtEsInfo_t));
		if (NULL == pstPmtEsInfo)
		{
			HxLOG_Error ("mem alloc failed..\n", ulActionId, stTripleId.usSvcId);
			goto END_FUNC;
		}

		hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
			goto END_FUNC;
		}

		// 6-1. component status (video/audio/ttx/sbtl)
		hErr = svc_si_LibGetEvtComponentStatusFromPmtEsInfo(ulActionId,pstPmtEsInfo, pstEvent);
		if (hErr != ERR_OK)
		{
			HxLOG_Error ("svc_si_LibGetEvtComponentStatusFromPmtEsInfo err: ActionId(%d)\n", ulActionId);
		}

		// 7-1. CA identifier
		pstEvent->ulCasType = pstPmtEsInfo->ulCasType;
	}

END_FUNC:
	if (NULL != pstEitActPfTable)				{ PAL_SIRAWPOOL_FreeTable (pstEitActPfTable); }
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }


	if(ERR_OK != hErr)
	{
		svc_si_LibFreeExtEventInfo(pstExtEvent);
	}

	return hErr;
}

HERROR	svc_si_LibFreeEventInfo (SvcSi_Event_t *pstEvent)
{
	if (pstEvent == NULL)
	{
		return ERR_FAIL;
	}

	// parental rating desciptor list
	HLIB_LIST_RemoveAllFunc(pstEvent->pstRatingDescList, svc_si_LibFreeDesList);

	// another descriptor list
	HLIB_LIST_RemoveAllFunc(pstEvent->pstEitDescList, svc_si_LibFreeDesList);

	return ERR_OK;
}

HERROR	svc_si_LibFreeExtEventInfo (SvcSi_ExtEvent_t *pstExtEvent)
{
	if(NULL == pstExtEvent)
	{
		return ERR_FAIL;
	}

	if(NULL != pstExtEvent->szTextChar)
	{
		OxSI_Free(pstExtEvent->szTextChar);
		pstExtEvent->szTextChar = NULL;
	}

	return ERR_OK;
}

#define _____CAT_RELATED_____
HERROR svc_si_LibGetEmmInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EmmList_t *pstEmmList)
{
	HxList_t			*pstListItem;
	SIxTable_PsiTable_t *pstTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Cat, SILIB_TABLE_EXT_ID_NOT);
	HERROR				 hResult = ERR_OK;
	HINT32				 i=0;

	if (NULL == pstTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, CAT\n", ulActionId);
		return ERR_FAIL;
	}


	pstEmmList->nEmm = HLIB_LIST_Length (pstTable->pstDesLoop);
	pstEmmList->pstEmmInfo = (SvcSi_EmmInfo_t *)OxSI_Calloc(sizeof(SvcSi_EmmInfo_t) * pstEmmList->nEmm);

	for (pstListItem = pstTable->pstDesLoop; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		HUINT8 *pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstListItem);

		if (eSIxDESCTAG_CA == *pucRawDes)
		{
			SIxCaDes_t	*pstCaDes = (SIxCaDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
			if (NULL != pstCaDes)
			{
				pstEmmList->pstEmmInfo[i].usPid		= pstCaDes->usCaPid;
				pstEmmList->pstEmmInfo[i].usCaSysId	= pstCaDes->usCaSysId;
				i++;
				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t*)pstCaDes);
			}
		}
	}

	if (i != pstEmmList->nEmm)
	{
		HxLOG_Critical ("!!! Critical Error !!!\n");
		hResult = ERR_FAIL;
	}

	PAL_SIRAWPOOL_FreeTable (pstTable);
	return hResult;
}

HERROR svc_si_LibFreeEmmList (SvcSi_EmmList_t *pstEmmList)
{
	if (NULL == pstEmmList)
	{
		return ERR_FAIL;
	}

	if (pstEmmList->pstEmmInfo != NULL)
	{
		OxSI_Free (pstEmmList->pstEmmInfo);
	}

	pstEmmList->nEmm = 0;
	pstEmmList->pstEmmInfo = NULL;

	return ERR_OK;
}

#define _____RCT_RELATED_____	// tv anytime..

HERROR svc_si_LibGetRctInfo(Handle_t hAction, Handle_t hSvc, DxService_RctInfo_t *pstRctInfo, HBOOL bMhegInterruptibleFlag)
{
	HERROR	hError = ERR_FAIL;
	HUINT32	ulActionId, ulTotalLinkCnt = 0, ulIndex = 0, ulCntLink = 0 ;
	SIxTable_PsiTable_t		*pstRctTable = NULL, *pstCurrRct = NULL;
	SIxTable_RctSec_t		*pstRctSecInfo = NULL;
	DbSvc_Info_t		stSvcInfo;
	HxList_t			*pst2ndLoop = NULL, *pstPromoTextLoop = NULL, *pstDxLinkInfoList = NULL, *pstList = NULL;
	HxLANG_Id_e 		eCurLang = eLangID_MAX, eMenuLangId ;
	HBOOL			bFoundImgIcon = FALSE, bFoundEvt = FALSE;
	DxService_RctLinkInfo_t	*pstDxLinkInfo = NULL;
	DxService_RctImageIconInfo_t	stDxImageInfo;
	DxService_RctShortEvtInfo_t		stDxEvtInfo;

	HUINT8	ucIconId = 0;

	HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_MemSet(&stDxImageInfo, 0x00, sizeof(DxService_RctImageIconInfo_t));
	HxSTD_MemSet(&stDxEvtInfo, 0x00, sizeof(DxService_RctShortEvtInfo_t));

	hError = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error ("DB_SVC_GetServiceInfo hSvc(0x%08x) \n", hSvc);
		goto Error;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);

	pstCurrRct = pstRctTable = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_Rct, SILIB_TABLE_EXT_ID_ALL);
	if(pstRctTable == NULL)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, RCT \n", ulActionId);
		goto Error;
	}

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, &eMenuLangId, 0);
	if(hError != ERR_OK)
	{
		eMenuLangId = eLangID_MAX;
	}

	while(pstCurrRct!=NULL)
	{
		pstRctSecInfo = &(pstCurrRct->unInfo.stRct);

		// live service 와 다른 rct 정보는 처리하지 않는다..
		if((pstRctSecInfo->usSvcId != DbSvc_GetSvcId(&stSvcInfo)) && (pstRctSecInfo->ucTableIdExtFlag == 0))
		{
			HxLOG_Debug(C_CODE_F_GREEN" usSvcId is different(0x%04x : 0x%04x) , ucTableIdExtFlag (%d) "C_CODE_RESET"\n",
				pstRctSecInfo->usSvcId , DbSvc_GetSvcId(&stSvcInfo), pstRctSecInfo->ucTableIdExtFlag);

			pstCurrRct = pstCurrRct->pstNextTable;
			continue;
		}

		pstRctInfo->usServiceId = pstRctSecInfo->usSvcId;
		pstRctInfo->usYearOffset= pstRctSecInfo->usYearOffset;

		HxLOG_Debug(C_CODE_F_GREEN"link Info (%p) "C_CODE_RESET"\n", pstCurrRct->pst2ndLoop);

		// 2nd loop : link info
		for (pst2ndLoop = pstCurrRct->pst2ndLoop; NULL != pst2ndLoop; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t*)HLIB_LIST_Data (pst2ndLoop);
			SIxTable_RctLinkInfo_t		*pstLinkInfo = NULL ;
			HUINT8					szDefAuth[DxMAX_DEFAULT_AUTHORITY_LEN];
			HUINT8					szValidCrId[DxRSV_CRID_LEN+1];
			HBOOL					bFoundEvtInLink = FALSE;

			pstLinkInfo = &(pst2ndItem->unInfo.stRct);

			hError = svc_si_LibCheckValidRctLinkInfo(hSvc , pstLinkInfo);
			if(hError !=ERR_OK)
			{
				HxLOG_Debug(C_CODE_F_GREEN"svc_si_LibCheckValidRctLinkInfo hError (%d) "C_CODE_RESET"\n", hError);
				continue;
			}

			pstDxLinkInfo = (DxService_RctLinkInfo_t *)OxSI_Malloc (sizeof(DxService_RctLinkInfo_t));
			if(pstDxLinkInfo == NULL)	{continue;}

			HxSTD_MemSet(pstDxLinkInfo, 0x00 ,sizeof(DxService_RctLinkInfo_t));

			pstDxLinkInfo->usTermId = pstLinkInfo->usTermId ;
			pstDxLinkInfo->ucHowRelatedSchemeId = pstLinkInfo->ucHowSchemeId ;
			pstDxLinkInfo->ucGroupId = pstLinkInfo->ucGroupId;
			pstDxLinkInfo->ucPrecedence= pstLinkInfo->ucPrecedence;

			HxLOG_Debug(C_CODE_F_YELLOW"usTermId(%d) ucHowSchemeId(%d), ucGroupId(%d) ucPrecedence(%d) ucLinkType(%d) "C_CODE_RESET"\n",
				pstLinkInfo->usTermId, pstLinkInfo->ucHowSchemeId, pstLinkInfo->ucGroupId, pstLinkInfo->ucPrecedence, pstLinkInfo->ucLinkType);

			if(pstLinkInfo->ucLinkType == eDxSVC_RctLinkType_Url || pstLinkInfo->ucLinkType == eDxSVC_RctLinkType_Both)
			{
				if(pstLinkInfo->ucMediaUriLen >0 )
				{
					/* Extract CRID */
					HxSTD_MemSet(szDefAuth, 0, sizeof(HUINT8) * (DxMAX_DEFAULT_AUTHORITY_LEN));
					HxSTD_MemSet(szValidCrId, 0, sizeof(HUINT8) * (DxRSV_CRID_LEN+1));

					DB_SVC_GetDefaultAuthority(hSvc, szDefAuth);

					hError = svc_si_LibSapiGetValidCrid(pstLinkInfo->pucMediaUriByte, pstLinkInfo->ucMediaUriLen, szDefAuth,szValidCrId);
					if(hError != ERR_OK)
					{
						HxLOG_Warning(C_CODE_F_GREEN"svc_si_LibSapiGetValidCrid hError (%d) "C_CODE_RESET"\n", hError);
						OxSI_Free(pstDxLinkInfo);
						continue;
					}

					HxSTD_MemCopy(pstDxLinkInfo->stMediaUri.szMediaUri , szValidCrId, DxRSV_CRID_LEN+1);
				}
			}

			if(pstLinkInfo->ucLinkType == eDxSVC_RctLinkType_Binary|| pstLinkInfo->ucLinkType == eDxSVC_RctLinkType_Both)
			{
				if(pstLinkInfo->pstDvbBinLocator != NULL)
				{
					pstDxLinkInfo->stDvbBinLocator.ucIdentifierType = pstLinkInfo->pstDvbBinLocator->ucIdType;
				}
			}

			HxLOG_Debug(C_CODE_F_YELLOW"pstPromoTextLoop(%p)"C_CODE_RESET"\n", pstLinkInfo->pstPromoTextLoop);

			if(pstLinkInfo->pstPromoTextLoop != NULL)
			{
				for(pstPromoTextLoop = pstLinkInfo->pstPromoTextLoop;NULL != pstPromoTextLoop; pstPromoTextLoop = pstPromoTextLoop->next)
				{
					SIxTable_RctPromoText_t *pstPromoText = (SIxTable_RctPromoText_t*)HLIB_LIST_Data(pstPromoTextLoop);

					if(pstPromoText->ucTxtLen > 0)
					{
						eCurLang = HLIB_LANG_639ToId(pstPromoText->szIso639LangCode);

						if(eLangID_MAX != eMenuLangId && eMenuLangId == eCurLang ) // 같은 lang인 경우에 넣도록 함
						{
							if(pstPromoText->ucTxtLen > DxRCT_MAX_PROMOTIONAL_TEXT)
							{
								pstDxLinkInfo->stPromotionalText.ucPromotionalTextLen = (HUINT8)DxRCT_MAX_PROMOTIONAL_TEXT;
							}
							else
							{
								pstDxLinkInfo->stPromotionalText.ucPromotionalTextLen = pstPromoText->ucTxtLen;
							}

							HxSTD_StrNCpy(pstDxLinkInfo->stPromotionalText.szLangCode, pstPromoText->szIso639LangCode, DxLANG_CODE_LEN-1);
							HxSTD_StrNCpy(pstDxLinkInfo->stPromotionalText.szPromotionalText, pstPromoText->pszPromoText, pstDxLinkInfo->stPromotionalText.ucPromotionalTextLen);
							MWC_UTIL_EliminateShyAndControlCharacter(pstDxLinkInfo->stPromotionalText.szPromotionalText, HxSTD_StrLen(pstDxLinkInfo->stPromotionalText.szPromotionalText));
						}
					}
				}
			}

			pstDxLinkInfo->bDefaultIcon= pstLinkInfo->ucDefaultIconFlag;
			ucIconId = pstLinkInfo->ucIconId;
			pstDxLinkInfo->ucIconId = pstLinkInfo->ucIconId;

			HxLOG_Debug(C_CODE_F_YELLOW"ucDefaultIconFlag(%d) ucIconId(%d) "C_CODE_RESET"\n", pstLinkInfo->ucDefaultIconFlag, pstLinkInfo->ucIconId);

			if(bFoundImgIcon == FALSE)
			{
				(void)svc_si_LibGetImageIconInRct(ulActionId, pst2ndItem->pstDesLoop, ucIconId, &bFoundImgIcon, &stDxImageInfo);
				if(bFoundImgIcon == TRUE)
				{
					svc_si_LibCopyImageIconInfo(hAction, &stDxImageInfo, &pstRctInfo->stImageIconInfo);
				}
			}

			if(bFoundEvtInLink == FALSE)
			{
				(void)svc_si_LibGetShortEvtInRct(ulActionId, pst2ndItem->pstDesLoop, &bFoundEvtInLink, &stDxEvtInfo);
				if(bFoundEvtInLink == TRUE)
				{
					HxSTD_memcpy(&pstDxLinkInfo->stShortEvtInfoInLink, &stDxEvtInfo, sizeof(DxService_RctShortEvtInfo_t));
				}
			}

			if(pstDxLinkInfo!= NULL)
			{
				if(bMhegInterruptibleFlag == FALSE)
				{
					if(HxSTD_MemCmp(&s_stInvalidRctLinkInfo, pstDxLinkInfo, sizeof(DxService_RctLinkInfo_t)))
					{
						HxSTD_MemSet(&s_stInvalidRctLinkInfo, 0x00, sizeof(DxService_RctLinkInfo_t));
						HxSTD_MemCopy(&s_stInvalidRctLinkInfo, pstDxLinkInfo, sizeof(DxService_RctLinkInfo_t));
					}
				}
				else
				{
					if(HxSTD_MemCmp(&s_stInvalidRctLinkInfo, pstDxLinkInfo, sizeof(DxService_RctLinkInfo_t)) == 0)
					{
						OxSI_Free(pstDxLinkInfo);
						continue;
					}
				}
			}

			pstDxLinkInfoList = HLIB_LIST_Append (pstDxLinkInfoList, (void *)pstDxLinkInfo);
		}

		if(bFoundImgIcon == FALSE)
		{
			(void)svc_si_LibGetImageIconInRct(ulActionId, pstCurrRct->pstDesLoop, ucIconId, &bFoundImgIcon,&stDxImageInfo);
			if(bFoundImgIcon == TRUE)
			{
				svc_si_LibCopyImageIconInfo(hAction, &stDxImageInfo, &pstRctInfo->stImageIconInfo);
			}
		}

		if(bFoundEvt == FALSE)
		{
			(void)svc_si_LibGetShortEvtInRct(ulActionId, pstCurrRct->pstDesLoop, &bFoundEvt, &stDxEvtInfo);
			if(bFoundEvt == TRUE)
			{
				HxSTD_memcpy(&pstRctInfo->stShortEvtInfo, &stDxEvtInfo, sizeof(DxService_RctShortEvtInfo_t));
			}
		}

		pstCurrRct = pstCurrRct->pstNextTable;
	}

	if(pstDxLinkInfoList == NULL)
	{
		HxLOG_Warning("pstDxLinkInfoList is NULL \n");
		hError = ERR_FAIL;
		goto Error;
	}

	ulTotalLinkCnt = HLIB_LIST_Length(pstDxLinkInfoList);
	if(ulTotalLinkCnt == 0)
	{
		HxLOG_Warning ("ulTotalLinkCnt(%d)\n", ulTotalLinkCnt);
		hError = ERR_FAIL;
		goto Error;
	}

	pstRctInfo->ulTotalLinkCnt = ulTotalLinkCnt;
	pstRctInfo->pstLinkInfoList = (DxService_RctLinkInfo_t *)OxSI_Calloc(sizeof(DxService_RctLinkInfo_t) *pstRctInfo->ulTotalLinkCnt );
	if(pstRctInfo->pstLinkInfoList == NULL)
	{
		hError = ERR_FAIL;
		goto Error;
	}

	ulCntLink =0 ;

	for (ulIndex = 0; ulIndex < ulTotalLinkCnt; ulIndex++)
	{
		pstList = HLIB_LIST_GetListItem(pstDxLinkInfoList, ulIndex);
		if (pstList == NULL)
		{
			HxLOG_Warning ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", pstRctInfo->ulTotalLinkCnt, ulIndex);
			continue;
		}

		pstDxLinkInfo = (DxService_RctLinkInfo_t *)HLIB_LIST_Data(pstList);
		if (pstDxLinkInfo == NULL)
		{
			HxLOG_Warning ("HLIB_LIST_Data() failed.. curPos(%d)\n", ulIndex);
			continue;
		}

		HxSTD_MemCopy(&pstRctInfo->pstLinkInfoList[ulCntLink], pstDxLinkInfo, sizeof(DxService_RctLinkInfo_t));

		ulCntLink++;
	}

	pstRctInfo->ulTotalLinkCnt = ulCntLink;

	hError = ERR_OK;
Error:

	HxLOG_Debug("RCT Link Info Count (%d) hError (%d) \n", pstRctInfo->ulTotalLinkCnt, hError );

	if(pstDxLinkInfoList != NULL) {HLIB_LIST_RemoveAll(pstDxLinkInfoList);}
	if(pstRctTable != NULL)	{PAL_SIRAWPOOL_FreeTable(pstRctTable);}

	return hError;

}

HERROR svc_si_LibCheckRctInfo(Handle_t hAction, Handle_t hSvc)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bValidLink = FALSE;
	HUINT32			ulActionId ;
	SIxTable_PsiTable_t		*pstRctTable = NULL, *pstCurrRct = NULL;
	SIxTable_RctSec_t		*pstRctSecInfo = NULL;
	DbSvc_Info_t		stSvcInfo;
	HxList_t			*pst2ndLoop = NULL;

	HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));

	hError = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error ("DB_SVC_GetServiceInfo hSvc(0x%08x) \n", hSvc);
		goto Error;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);

	pstCurrRct = pstRctTable = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_Rct, SILIB_TABLE_EXT_ID_ALL);
	if(pstRctTable == NULL)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, RCT \n", ulActionId);
		goto Error;
	}

	while(pstCurrRct!=NULL)
	{
		pstRctSecInfo = &(pstCurrRct->unInfo.stRct);

		// live service 와 다른 rct 정보는 처리하지 않는다..
		if((pstRctSecInfo->usSvcId != DbSvc_GetSvcId(&stSvcInfo)) && (pstRctSecInfo->ucTableIdExtFlag == 0))
		{

			pstCurrRct = pstCurrRct->pstNextTable;
			continue;
		}

		// 2nd loop : link info
		for (pst2ndLoop = pstCurrRct->pst2ndLoop; NULL != pst2ndLoop; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t*)HLIB_LIST_Data (pst2ndLoop);
			SIxTable_RctLinkInfo_t		*pstLinkInfo = NULL ;

			pstLinkInfo = &(pst2ndItem->unInfo.stRct);

			hError = svc_si_LibCheckValidRctLinkInfo(hSvc , pstLinkInfo);
			if(hError == ERR_OK)
			{
				bValidLink = TRUE;
			}
		}

		pstCurrRct = pstCurrRct->pstNextTable;
	}

	if(bValidLink == TRUE)
		hError = ERR_OK;
	else
		hError = ERR_FAIL; //validLink가 아닌 경우에는 Error Fail 로 처리
Error:

	if(pstRctTable != NULL)	{PAL_SIRAWPOOL_FreeTable(pstRctTable);}

	return hError;

}


#define _____SERVICE_RELATED_____
HERROR svc_si_LibGetAllPmtProgramsFromPat (HUINT32 ulActionId, HUINT32 *pulNumPmt, SIxTable_PatProgram_t **ppstProgramArray)
{
	HUINT32					 ulNumPmt = 0;
	SIxTable_PsiTable_t		*pstTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
	SIxTable_PatProgram_t	*pstProgramArray = NULL;
	HxList_t				*pstListItem;
	HERROR					 hResult = ERR_FAIL;

	if (NULL == pstTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PAT\n", ulActionId);
		goto END_FUNC;
	}

	ulNumPmt = HLIB_LIST_Length (pstTable->pst2ndLoop);
	if (0 == ulNumPmt)
	{
		HxLOG_Error ("PAT has no PMT information!! ulActionId=%d\n", ulActionId);
		goto END_FUNC;
	}

	pstProgramArray = (SIxTable_PatProgram_t *)OxSI_Malloc (sizeof(SIxTable_PatProgram_t) * ulNumPmt);
	if (NULL == pstProgramArray)
	{
		HxLOG_Error ("Memory allocation failed!! ulActionId=%d\n", ulActionId);
		goto END_FUNC;
	}

	ulNumPmt = 0;
	for (pstListItem = pstTable->pst2ndLoop; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SIxTable_2ndLoop_t			*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pst2ndItem)
		{
			SIxTable_PatProgram_t	*pstProgram = &(pst2ndItem->unInfo.stPat);

			if (pstProgram->usProgNum != 0)	// ProgNum가 0이면 NIT다 (13818-1)
			{
				HxSTD_MemCopy (&(pstProgramArray[ulNumPmt]), pstProgram, sizeof(SIxTable_PatProgram_t));
				ulNumPmt ++;
			}
		}
	}

	if (NULL != pulNumPmt)
	{
		*pulNumPmt = ulNumPmt;
	}

	if (NULL != ppstProgramArray)
	{
		*ppstProgramArray = pstProgramArray;
		pstProgramArray = NULL;
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstProgramArray)					{ OxSI_Free (pstProgramArray); }
	PAL_SIRAWPOOL_FreeTable (pstTable);

	return hResult;
}

HERROR svc_si_LibCheckServiceNameBySvcDes (HINT32 nNameSizeOfSvcDes, HUINT8 *pucSvcDesName, HUINT8 *szSvcInfoName, HBOOL *pbNameUpdate)
{
	HINT32		nSrcPtr;
	HBOOL		bNonSpaceFound;

	HERROR		hErr = ERR_FAIL;
	HUINT8		*szUndefinedSvcName = NULL;

	if (NULL == szSvcInfoName)
	{ /* invalid param - do nothing */
		return ERR_FAIL;
	}

	if ((NULL == pucSvcDesName) || (0 == nNameSizeOfSvcDes))
	{
		goto set_to_unknown;
	}

	bNonSpaceFound = FALSE;

	for (nSrcPtr = 0; (nSrcPtr < /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()) && ('\0' != pucSvcDesName[nSrcPtr]); nSrcPtr++)
	{
		if ((' ' != pucSvcDesName[nSrcPtr]) && ('\t' != pucSvcDesName[nSrcPtr]))
		{
			bNonSpaceFound = TRUE;
			break;
		}
	}

	if (bNonSpaceFound == FALSE)
	{
		goto set_to_unknown;
	}

	if (pbNameUpdate != NULL)
	{
		*pbNameUpdate = (HxSTD_StrNCmp (pucSvcDesName, szSvcInfoName, /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()-1) != 0) ? TRUE : FALSE;
	}

	HLIB_STD_StrUtf8NCpy(szSvcInfoName, pucSvcDesName, /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()-1);
	return ERR_OK;

set_to_unknown:

	if (pbNameUpdate != NULL)
	{
		*pbNameUpdate = ('\0' != szSvcInfoName[0]) ? TRUE : FALSE;
	}

	hErr = svc_si_LibGetUndefinedServiceName(0, &szUndefinedSvcName); /* actionId를 param으로 주지만 함수안에서 사용되지 않음 */
	if ((hErr == ERR_OK) && (szUndefinedSvcName != NULL))
	{
		HLIB_STD_StrUtf8NCpy(szSvcInfoName, szUndefinedSvcName, /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()-1);
	}

	return ERR_OK;
}

HERROR svc_si_LibComposeServiceInfoInTp (HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HERROR			 hErr;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	hErr = psvc_si_ComposeServiceInfoInTp (ulActionId, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
	HxLOG_Print ("[%s:%d] (-) : hErr = 0x%08x\n", __FUNCTION__, __LINE__, hErr);

	return hErr;
}

HERROR svc_si_LibUpdateTimeByTdt (HUINT32 ulActionId, Handle_t hService, SIxTable_PsiTable_t *pstTdtTable)
{
	HERROR			 hErr;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	hErr = psvc_si_UpdateTimeByTdt (ulActionId, hService, pstTdtTable);
	HxLOG_Print ("[%s:%d] (-) : hErr = 0x%08x\n", __FUNCTION__, __LINE__, hErr);

	return hErr;
}

HERROR svc_si_LibUpdateTimeByTot (HUINT32 ulActionId)
{
	HERROR			 hErr;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	hErr = psvc_si_UpdateTimeByTot (ulActionId);
	HxLOG_Print ("[%s:%d] (-) : hErr = 0x%x \n", __FUNCTION__, __LINE__, hErr);

	return hErr;
}

HERROR svc_si_LibSvcEsComposeServiceInfoBySdt (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray)
{
	HERROR			 hErr;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	hErr = psvc_si_ComposeServiceInfoBySdt (ulActionId, pstSdtTable, pulSvcNum, ppstSvcArray, pulPrvNum, ppstPrvArray);
	HxLOG_Print ("[%s:%d] (-) : hErr = 0x%x \n", __FUNCTION__, __LINE__, hErr);

	return hErr;
}


#define _____MAKE_SECTION_____
HERROR svc_si_LibMakeSiSection (HUINT32 ulActionId, SvcSi_MakeSectionSpec_t *pstSpec, SvcSi_MakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen)
{
	HERROR			 hErr;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	hErr = psvc_si_MakeSiSection (ulActionId, pstSpec, punArg, pucRawSec, pulSecLen);
	HxLOG_Print ("[%s:%d] (-) : hErr = 0x%x \n", __FUNCTION__, __LINE__, hErr);

	return hErr;
}


#define _____UTILS_____

HERROR svc_si_LibAddBqInfoToBqInfoTable (DbSvc_BouqInfo_t *pstBqInfo, HBOOL bCheckBouqId, SvcSi_BouquetInfoTable_t * pstBqInfoTable, HUINT16 *pusBqIndex)
{
	HUINT16			 	usBqIdx;
	DbSvc_BouqInfo_t	*pstBqArray;

	if (NULL == pstBqInfo)		{ return ERR_FAIL; }
	if (NULL == pstBqInfoTable)	{ return ERR_FAIL; }
	if (NULL == pusBqIndex)		{ return ERR_FAIL; }

	// Bouquet ID로 체크하는 것이 아니라면 Name으로 동일 여부를 체크하도록 한다.
	if (bCheckBouqId == TRUE)
	{
		for (usBqIdx = 0; usBqIdx < pstBqInfoTable->ulBqNum; usBqIdx++ )
		{
			if (pstBqInfoTable->pstBqArray[usBqIdx].usBouqId == pstBqInfo->usBouqId)
			{
				if (pstBqInfoTable->pstBqArray[usBqIdx].szBouqName[0] == '\0' && pstBqInfo->szBouqName[0] != '\0')
				{
					HLIB_STD_StrUtf8NCpy (pstBqInfoTable->pstBqArray[usBqIdx].szBouqName, pstBqInfo->szBouqName, DxMAX_BOUQ_NAME_LEN );
				}

				*pusBqIndex = usBqIdx;
				return ERR_OK;
			}
		}
	}
	else
	{
		for (usBqIdx = 0; usBqIdx < pstBqInfoTable->ulBqNum; usBqIdx++ )
		{
			if (MWC_UTIL_DvbStrncmp (pstBqInfoTable->pstBqArray[usBqIdx].szBouqName, pstBqInfo->szBouqName, DxMAX_BOUQ_NAME_LEN) == 0)
			{
				*pusBqIndex = usBqIdx;
				return ERR_OK;
			}
		}
	}

	if (pstBqInfoTable->ulBqNum >= pstBqInfoTable->ulArrayLen)
	{
		pstBqArray = (DbSvc_BouqInfo_t *)OxSI_Malloc (sizeof(DbSvc_BouqInfo_t) * (pstBqInfoTable->ulArrayLen + SvcSi_MAX_NUM_BOUQUETS));
		if (pstBqArray == NULL)
		{
			return ERR_FAIL;
		}

		HxSTD_memset (pstBqArray, 0, sizeof(DbSvc_BouqInfo_t) * (pstBqInfoTable->ulArrayLen + SvcSi_MAX_NUM_BOUQUETS));

		if (pstBqInfoTable->ulArrayLen != 0)
		{
			HxSTD_memcpy (pstBqArray, pstBqInfoTable->pstBqArray, sizeof(DbSvc_BouqInfo_t) * pstBqInfoTable->ulArrayLen);
			OxSI_Free (pstBqInfoTable->pstBqArray);
		}

		pstBqInfoTable->pstBqArray = pstBqArray;
		pstBqInfoTable->ulArrayLen += SvcSi_MAX_NUM_BOUQUETS;
	}

	*pusBqIndex = usBqIdx;
	pstBqInfoTable->pstBqArray[usBqIdx] = *pstBqInfo;
	pstBqInfoTable->pstBqArray[usBqIdx].usBouqIdx = usBqIdx;
	pstBqInfoTable->ulBqNum++;

	return ERR_OK;
}

HERROR svc_si_LibAddSvcInfoToSvcInfoTable (DbSvc_Info_t *pstSvcInfo, HBOOL bIsSameTp, SvcSi_ServiceInfoTable_t *pstSvcInfoTable, HUINT32 *pulSvcIndex)
{
	HUINT32 		 ulSvcIdx;
	DbSvc_Info_t		*pstSvcArray;

	if (NULL == pstSvcInfo)			{ return ERR_FAIL; }
	if (NULL == pstSvcInfoTable)	{ return ERR_FAIL; }
	if (NULL == pulSvcIndex)		{ return ERR_FAIL; }

	// 같은 TP에서만 찾을 경우, 같은 Service ID는 있을 수 없으므로 제거해야 한다.
	if (bIsSameTp == TRUE)
	{
		for (ulSvcIdx = 0; ulSvcIdx < pstSvcInfoTable->ulSvcNum; ulSvcIdx++ )
		{
			if (DbSvc_GetSvcId(&pstSvcInfoTable->pstSvcArray[ulSvcIdx]) == DbSvc_GetSvcId(pstSvcInfo))
			{
				pstSvcInfoTable->pstSvcArray[ulSvcIdx] = *pstSvcInfo;
				*pulSvcIndex = ulSvcIdx;
				return ERR_OK;
			}
		}
	}

	ulSvcIdx = pstSvcInfoTable->ulSvcNum;

	if (pstSvcInfoTable->ulSvcNum >= pstSvcInfoTable->ulArrayLen)
	{
		pstSvcArray = (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t) * (pstSvcInfoTable->ulArrayLen + SvcSi_MAX_NUM_SVC));
		if (pstSvcArray == NULL)
		{
			return ERR_FAIL;
		}

		if (pstSvcInfoTable->ulArrayLen != 0)
		{
			HxSTD_memcpy (pstSvcArray, pstSvcInfoTable->pstSvcArray, sizeof(DbSvc_Info_t) * pstSvcInfoTable->ulArrayLen);
			OxSI_Free (pstSvcInfoTable->pstSvcArray);
		}

		pstSvcInfoTable->pstSvcArray = pstSvcArray;
		pstSvcInfoTable->ulArrayLen += SvcSi_MAX_NUM_SVC;
	}

	*pulSvcIndex = ulSvcIdx;
	pstSvcInfoTable->pstSvcArray[ulSvcIdx] = *pstSvcInfo;
	pstSvcInfoTable->ulSvcNum++;

	return ERR_OK;
}


#define _____DEBUG_FUNCTIONS_____

HUINT8* svc_si_LibGetTableTypeName(SIxTable_Type_e eTable)
{
#if defined(CONFIG_DEBUG)
	switch(eTable)
	{
		ENUM_TO_STR(eSIxTABLETYPE_Pat)
		ENUM_TO_STR(eSIxTABLETYPE_Pmt)
		ENUM_TO_STR(eSIxTABLETYPE_NitAct)
		ENUM_TO_STR(eSIxTABLETYPE_NitOth)
		ENUM_TO_STR(eSIxTABLETYPE_Bat)
		ENUM_TO_STR(eSIxTABLETYPE_SdtAct)
		ENUM_TO_STR(eSIxTABLETYPE_SdtOth)
		ENUM_TO_STR(eSIxTABLETYPE_EitActPf)
		ENUM_TO_STR(eSIxTABLETYPE_EitOthPf)
		ENUM_TO_STR(eSIxTABLETYPE_EitActSc)
		ENUM_TO_STR(eSIxTABLETYPE_EitOthSc)
		ENUM_TO_STR(eSIxTABLETYPE_Tdt)
		ENUM_TO_STR(eSIxTABLETYPE_Tot)
		ENUM_TO_STR(eSIxTABLETYPE_Cat)
		ENUM_TO_STR(eSIxTABLETYPE_Ait)

		ENUM_TO_STR(eSIxTABLETYPE_MAX)
		ENUM_TO_STR(eSIxTABLETYPE_ALL)

		default:			break;
	}

	HxLOG_Print("Unkown table type [%d]\n", eTable);
#endif

	return "Invalid table type";
}

/* end of file */

