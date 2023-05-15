/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**
	@file     sxdescparse_fsat.c
	@brief    sxdescparse_fsat.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"


static HCHAR *silib_desc_getTagString_Fsat(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
	ENUM_TO_STR(eSIxDESCTAG_FSAT_TUNNELLED_DATA);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_LINKAGE);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_REGION_NAME);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_SERVICE_GROUP);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_INFO_LOCATION);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_SERVICEGROUP_NAME);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_GUIDANCE);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_PREFIX);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_MEDIA_LOCATOR);
	ENUM_TO_STR(eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL);
	default:
		break;
	}

	return NULL;
}

// ==================================================================================
// = Internal Function Template
// = DO NOT Delete!!!
// ==================================================================================
#if 0
//XXX
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxXXXDes_t));
}

static SIxXXXDes_t *allocXXXDes(void)
{
	return (SIxXXXDes_t *)silib_desc_alloc(sizeof(SIxXXXDes_t));
}

static void freeXXXInfo(SIxXXXInfo_t *pstInfo)
{
	if (pstInfo)
	{
		// silib_desc_free(pstInfo->pszYYY);
		// silib_desc_free(pstInfo->pstZZZ);
		silib_desc_free(pstInfo);
	}
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDest->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{
	SIxXXXInfo_t		*pstPrevInfo = NULL, *pstNewInfo;
	HUINT8				 ucXXXLen, ucAssignedXXXLen;
	HUINT16 			 usTempXXXLen;
	HUINT8				 szIso639LangCode[4], *pszXXX = NULL;
	HUINT16				 usDesLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	if (usDesLen < 6)
	{
		return eSIxERR_FAIL;
	}

	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxXXXInfo_t, pstDes);
		pstNewInfo = allocXXXInfo();

		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		// ISO 639 language code (3byte)
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		p += 3;
		usDesLen -= 3;

		// name length (1 byte)
		ucXXXLen = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		if (usDesLen < ucXXXLen)
		{
			goto err_exit;
		}
		ucAssignedXXXLen = ucXXXLen;

		// event name
		usTempXXXLen = (HUINT16)silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc, ucAssignedXXXLen, (const HUINT8 *)p, &pszXXX);
		if (NULL == pszXXX)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		p += ucXXXLen;
		usDesLen -= ucXXXLen;

		HxSTD_MemSet(pszXXX, '\0', sizeof(HUINT8) * (usTempXXXLen+1));

#if 0
		setXXXInfo(pstNewInfo, usRegionId, szIso639LangCode, pszXXX);
		pstNewInfo->pszXXX = pszXXX;
#endif

		LINK_INFO_NODE (pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	return eSIxERR_OK;

err_exit:
	if (pszXXX)
	{
		silib_desc_free(pszXXX);
	}

	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxXXXInfo_t *pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxXXXDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxXXXInfo_t, pstDest);
			pstNewInfo = (SIxXXXInfo_t *)allocXXXInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxXXXInfo_t));
			pstNewInfo->pstNext = NULL;
			// TODO: Init Pointer
			pstNewInfo->pszYYY = NULL;
			pstNewInfo->pstZZZ = NULL;
			pstNewInfo->aucUUU = NULL;
			pstNewInfo->ucNum = 0;
			if (copyXXXInfo(pstNewInfo, pstSourceInfo) != eSIxERR_OK)
			{
				silib_desc_free(pstNewInfo);
				goto err_exit;
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxXXXInfo_t, freeXXXInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}
#endif

#if 0
eSIxDESCTAG_FSAT_TUNNELLED_DATA
eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA
eSIxDESCTAG_FSAT_LINKAGE
eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL
eSIxDESCTAG_FSAT_REGION_NAME
eSIxDESCTAG_FSAT_SERVICE_GROUP
eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE
eSIxDESCTAG_FSAT_INFO_LOCATION
eSIxDESCTAG_FSAT_SERVICEGROUP_NAME
eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME
eSIxDESCTAG_FSAT_GUIDANCE
eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION
eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT
eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY
eSIxDESCTAG_FSAT_PREFIX
eSIxDESCTAG_FSAT_MEDIA_LOCATOR
eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL

SIxTunnelledDataDes_t
SIxAltTunnelledDataDes_t
SIxFSatLinkageDes_t
SIxRegionalisedLcnDes_t
SIxRegionNameDes_t
SIxSvcGroupDes_t
SIxInteractiveStorageDes_t
SIxInfoLocationDes_t
SIxSvcGroupNameDes_t
SIxShortSvcNameDes_t
SIxGuidanceDes_t
SIxInteractiveRestrictionDes_t
SIxContentManagementDes_t
SIxOnlineAvailabilityDes_t
SIxPrefixDes_t
SIxMediaLocatorDes_t
SIxShortEventPromotionalDes_t

//TunnelledData
//AltTunnelledData
//FSatLinkage
//RegionalisedLcn
//RegionName
//SvcGroup
//InteractiveStorage
//InfoLocation
//SvcGroupName
//ShortSvcName
//Guidance
//InteractiveRestriction
//ContentManagement
//OnlineAvailability
//Prefix
//MediaLocator
//ShortEventPromotional

#endif


/************************************************************************
 * eSIxDESCTAG_FSAT_TUNNELLED_DATA					 (TAG = 0xD0)
 ************************************************************************/
//TunnelledData
static void initTunnelledDataDes(SIxTunnelledDataDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxTunnelledDataDes_t));
}

static SIxTunnelledDataDes_t *allocTunnelledDataDes(void)
{
	return (SIxTunnelledDataDes_t *)silib_desc_alloc(sizeof(SIxTunnelledDataDes_t));
}

static void freeTunnelledDataDes(SIxTunnelledDataDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->astInfo);
		silib_desc_free(pstDes);
	}
}

static SIxTunnelledDataInfo_t *allocTunnelledDataInfo(HUINT32 ulNumInfo)
{
	SIxTunnelledDataInfo_t	*pstInfo = NULL;

	if (ulNumInfo > 0)
	{
		pstInfo = (SIxTunnelledDataInfo_t *) silib_desc_alloc(sizeof(SIxTunnelledDataInfo_t) * ulNumInfo);
		if (pstInfo != NULL)
		{
			HxSTD_MemSet(pstInfo, 0, sizeof(SIxTunnelledDataInfo_t) * ulNumInfo);
		}
	}

	return pstInfo;
}

#define	SIZE_OF_UKFSAT_TUNNELLED_DATA_DES		(2)

static SIxError_e parseTunnelledDataDes(HUINT8 *p, SIxTunnelledDataDes_t *pstDes)
{
	HUINT8					 ucDesLen;
	HUINT32					 ulNumInfo, ulIndex;
	SIxTunnelledDataInfo_t	*astInfo = NULL;

	ucDesLen = p[1];
	p += 2;

	if ((ucDesLen == 0) || (ucDesLen % 2 != 0))
	{
		return eSIxERR_INVALID;
	}

	ulNumInfo = (HUINT32) ucDesLen / SIZE_OF_UKFSAT_TUNNELLED_DATA_DES;

	astInfo	= allocTunnelledDataInfo(ulNumInfo);
	if (NULL == astInfo)
		goto err_exit;

	pstDes->ulNumInfo   = ulNumInfo;
	pstDes->astInfo		= astInfo;

	for (ulIndex = 0; ulIndex < ulNumInfo; ulIndex++)
	{
		astInfo[ulIndex].ucTunnelledDataType	= HxMACRO_Get8Bit(p);
		astInfo[ulIndex].ucComponentTag			= HxMACRO_Get8Bit(p + 1);
		p += 2;
	}

	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyTunnelledDataDes(SIxTunnelledDataDes_t *pstDest, const SIxTunnelledDataDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTunnelledDataDes_t));
		pstDest->astInfo = NULL;
		pstDest->ulNumInfo = 0;

		if (0 < pstSource->ulNumInfo && NULL != pstSource->astInfo)
		{
			pstDest->astInfo = allocTunnelledDataInfo(pstSource->ulNumInfo);
			if (NULL == pstDest->astInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->astInfo, pstSource->astInfo, sizeof(SIxTunnelledDataInfo_t)*pstSource->ulNumInfo);
			pstDest->ulNumInfo = pstSource->ulNumInfo;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->astInfo);
		pstDest->astInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA				 (TAG = 0xD1)
 ************************************************************************/
//AltTunnelledData
static void initAltTunnelledDataDes(SIxAltTunnelledDataDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxAltTunnelledDataDes_t));
}

static SIxAltTunnelledDataDes_t *allocAltTunnelledDataDes(void)
{
	return (SIxAltTunnelledDataDes_t *)silib_desc_alloc(sizeof(SIxAltTunnelledDataDes_t));
}

static void freeAltTunnelledDataDes(SIxAltTunnelledDataDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->astInfo);
		silib_desc_free(pstDes);
	}
}

static SIxAltTunnelledDataInfo_t *allocAltTunnelledDataInfo(HUINT32 ulNumInfo)
{
	SIxAltTunnelledDataInfo_t	*pstInfo = NULL;

	if (ulNumInfo > 0)
	{
		pstInfo = (SIxAltTunnelledDataInfo_t *)silib_desc_alloc(sizeof(SIxAltTunnelledDataInfo_t) * ulNumInfo);
		if (pstInfo != NULL)
		{
			HxSTD_MemSet(pstInfo, 0, sizeof(SIxAltTunnelledDataInfo_t) * ulNumInfo);
		}
	}

	return pstInfo;
}

static SIxError_e parseAltTunnelledDataDes(HUINT8 *p, SIxAltTunnelledDataDes_t *pstDes)
{
	HUINT8				 		 ucDesLen;
	HUINT32						 ulNumInfo, ulIndex;
	SIxAltTunnelledDataInfo_t	*astInfo;

	ucDesLen = HxMACRO_Get8Bit(p+1);
	p += 2;

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	ulNumInfo = (HUINT32) ucDesLen;

	astInfo	= allocAltTunnelledDataInfo(ulNumInfo);
	if (NULL == astInfo)
		goto err_exit;

	pstDes->ulNumInfo   = ulNumInfo;
	pstDes->astInfo		= astInfo;

	for (ulIndex = 0; ulIndex < ulNumInfo; ulIndex++)
	{
		astInfo[ulIndex].ucTunnelledDataType = HxMACRO_Get8Bit(p);
		p += 1;
	}

	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyAltTunnelledDataDes(SIxAltTunnelledDataDes_t *pstDest, const SIxAltTunnelledDataDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAltTunnelledDataDes_t));
		pstDest->astInfo = NULL;
		pstDest->ulNumInfo = 0;

		if (0 < pstSource->ulNumInfo && NULL != pstSource->astInfo)
		{
			pstDest->astInfo = allocAltTunnelledDataInfo(pstSource->ulNumInfo);
			if (NULL == pstDest->astInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->astInfo, pstSource->astInfo, sizeof(SIxAltTunnelledDataInfo_t)*pstSource->ulNumInfo);
			pstDest->ulNumInfo = pstSource->ulNumInfo;
		}

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->astInfo);
		pstDest->astInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_LINKAGE						 (TAG = 0xD2)
 ************************************************************************/
//FSatLinkage
static void initFSatLinkageDes(SIxFSatLinkageDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxFSatLinkageDes_t));
}

static SIxFSatLinkageDes_t *allocFSatLinkageDes(void)
{
	return (SIxFSatLinkageDes_t *)silib_desc_alloc(sizeof(SIxFSatLinkageDes_t));
}

static SIxFSatLinkageInfo_t *allocFSatLinkageInfo(void)
{
	SIxFSatLinkageInfo_t *pstInfo;

	pstInfo = (SIxFSatLinkageInfo_t *)silib_desc_alloc(sizeof(SIxFSatLinkageInfo_t));
	if (pstInfo==NULL)	return NULL;

	pstInfo->usTsId = 0xFFFF;
	pstInfo->usOrgNetId = 0xFFFF;
	pstInfo->usSvcId = 0xFFFF;
	pstInfo->ucStandardTypeNum = 0;
	pstInfo->ucTunnelledTypeNum = 0;
	pstInfo->aucStandardLinkageType = NULL;
	pstInfo->aucTunnelledLinkageType = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}


static void freeFSatLinkageInfo(SIxFSatLinkageInfo_t	*pstInfo)
{
	if (pstInfo)
	{
		silib_desc_free(pstInfo->aucStandardLinkageType);
		silib_desc_free(pstInfo->aucTunnelledLinkageType);
		silib_desc_free(pstInfo);
	}
}

static void freeFSatLinkageDes(SIxFSatLinkageDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxFSatLinkageInfo_t, freeFSatLinkageInfo);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseFSatLinkageDes(HUINT8 *p, SIxFSatLinkageDes_t *pstDes)
{
	SIxFSatLinkageInfo_t	*pstPrevInfo = NULL, *pstNewInfo;
	HUINT8				ucTypeLoopLen;
	HUINT16				usDesLen;

	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	if (usDesLen < 8)
	{
		return eSIxERR_INVALID;
	}

 	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxFSatLinkageInfo_t, pstDes);
		pstNewInfo = allocFSatLinkageInfo();

		if (NULL == pstNewInfo)
			goto err_exit;

		// Ts id (2byte)
		pstNewInfo->usTsId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;

		// Original Network id (2byte)
		pstNewInfo->usOrgNetId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;

		// standard type loop length
		pstNewInfo->ucStandardTypeNum = ucTypeLoopLen = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		if (0 != ucTypeLoopLen)
		{
			pstNewInfo->aucStandardLinkageType = silib_desc_alloc(sizeof(HUINT8)*ucTypeLoopLen);
			if (NULL == pstNewInfo->aucStandardLinkageType)
			{
				goto err_exit;
			}
			HxSTD_MemCopy(pstNewInfo->aucStandardLinkageType, p, sizeof(HUINT8)*ucTypeLoopLen);
		}
		else
		{
			pstNewInfo->aucStandardLinkageType = NULL;
		}
		p += ucTypeLoopLen;
		usDesLen -= ucTypeLoopLen;

		// Service id (2byte)
		pstNewInfo->usSvcId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;

		// tunnelled type loop length
		pstNewInfo->ucTunnelledTypeNum = ucTypeLoopLen = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		if (0 != ucTypeLoopLen)
		{
			pstNewInfo->aucTunnelledLinkageType = silib_desc_alloc(sizeof(HUINT8)*ucTypeLoopLen);
			if (NULL == pstNewInfo->aucTunnelledLinkageType)
			{
				goto err_exit;
			}
			HxSTD_MemCopy(pstNewInfo->aucTunnelledLinkageType, p, sizeof(HUINT8)*ucTypeLoopLen);
		}
		else
			pstNewInfo->aucTunnelledLinkageType = NULL;
		p += ucTypeLoopLen;
		usDesLen -= ucTypeLoopLen;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}
	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxFSatLinkageInfo_t, freeFSatLinkageInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;

}

static SIxError_e copyFSatLinkageInfo(SIxFSatLinkageInfo_t *pstDest, const SIxFSatLinkageInfo_t *pstSource)
{
	SIxError_e	eErr = eSIxERR_FAIL;
	HUINT32		ulSize;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxFSatLinkageInfo_t));

		pstDest->aucStandardLinkageType = NULL;
		pstDest->aucTunnelledLinkageType = NULL;
		pstDest->pstNext = NULL;

		if (0 < pstSource->ucStandardTypeNum && NULL != pstSource->aucStandardLinkageType)
		{
			ulSize = sizeof(HUINT8)*pstDest->ucStandardTypeNum;
			pstDest->aucStandardLinkageType = silib_desc_alloc(ulSize);
			if (NULL == pstDest->aucStandardLinkageType)
				goto err_exit;

			HxSTD_MemCopy(pstDest->aucStandardLinkageType, pstSource->aucStandardLinkageType, ulSize);
			pstDest->ucStandardTypeNum = pstSource->ucStandardTypeNum;
		}

		if (0 < pstSource->ucTunnelledTypeNum && NULL != pstSource->aucTunnelledLinkageType)
		{
			ulSize = sizeof(HUINT8)*pstDest->ucTunnelledTypeNum;
			pstDest->aucTunnelledLinkageType = silib_desc_alloc(ulSize);
			if (NULL == pstDest->aucTunnelledLinkageType)
				goto err_exit;

			HxSTD_MemCopy(pstDest->aucTunnelledLinkageType, pstSource->aucTunnelledLinkageType, ulSize);
			pstDest->ucTunnelledTypeNum = pstSource->ucTunnelledTypeNum;
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->aucStandardLinkageType);
		pstDest->aucStandardLinkageType = NULL;
		pstDest->ucStandardTypeNum = 0;
		silib_desc_free(pstDest->aucTunnelledLinkageType);
		pstDest->aucTunnelledLinkageType = NULL;
		pstDest->ucTunnelledTypeNum = 0;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyFSatLinkageDes(SIxFSatLinkageDes_t *pstDest, const SIxFSatLinkageDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxFSatLinkageInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxFSatLinkageDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxFSatLinkageInfo_t, pstDest);
			pstNewInfo = (SIxFSatLinkageInfo_t *)allocFSatLinkageInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxFSatLinkageInfo_t));
			pstNewInfo->pstNext = NULL;
			pstNewInfo->aucStandardLinkageType = NULL;
			pstNewInfo->aucTunnelledLinkageType = NULL;
			pstNewInfo->ucStandardTypeNum = 0;
			pstNewInfo->ucTunnelledTypeNum = 0;
			if (copyFSatLinkageInfo(pstNewInfo, pstSourceInfo) != eSIxERR_OK)
			{
				silib_desc_free(pstNewInfo);
				goto err_exit;
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxFSatLinkageInfo_t, freeFSatLinkageInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL 	 (TAG = 0xD3)
 ************************************************************************/
//RegionalisedLcn
static void initRegionalisedLcnDes(SIxRegionalisedLcnDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxRegionalisedLcnDes_t));
}

static SIxRegionalisedLcnDes_t *allocRegionalisedLcnDes(void)
{
	return (SIxRegionalisedLcnDes_t *)silib_desc_alloc(sizeof(SIxRegionalisedLcnDes_t));
}

static SIxFSatLcnSvcInfo_t *allocFSatLcnSvcInfo(void)
{
	SIxFSatLcnSvcInfo_t *pstInfo;

	pstInfo = (SIxFSatLcnSvcInfo_t *)silib_desc_alloc(sizeof(SIxFSatLcnSvcInfo_t));
	if (NULL == pstInfo)	return NULL;

	pstInfo->usSvcId = 0xffff;
	pstInfo->usSvcIdentifier = 0xffff;
	pstInfo->pstInfo = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static SIxFSatLcnInfo_t *allocFSatLcnInfo(void)
{
	SIxFSatLcnInfo_t *pstInfo;

	pstInfo = (SIxFSatLcnInfo_t *)silib_desc_alloc(sizeof(SIxFSatLcnInfo_t));
	if (NULL == pstInfo)	return NULL;

	HxSTD_MemSet(pstInfo,0,sizeof(SIxFSatLcnInfo_t));
	pstInfo->logical_channel_number = 0x0fff;
	pstInfo->region_id = 0xffff;

	return pstInfo;
}

static void freeFSatLcnSvcInfo(SIxFSatLcnSvcInfo_t *pstInfo)
{
	if (pstInfo)
	{
		FREE_INFO_LIST(pstInfo->pstInfo, SIxFSatLcnInfo_t);
		silib_desc_free(pstInfo);
	}
}

static void freeRegionalisedLcnDes(SIxRegionalisedLcnDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxFSatLcnSvcInfo_t, freeFSatLcnSvcInfo);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseRegionalisedLcnDes(HUINT8 *p, SIxRegionalisedLcnDes_t *pstDes)
{
	SIxFSatLcnSvcInfo_t		*pstPrevInfo = NULL, *pstNewInfo;
	SIxFSatLcnInfo_t			*pstPrevLcnInfo = NULL, *pstNewLcnInfo;
	HUINT8					 ucTmp8bit;
	HUINT16					 usLcnLoopLen;
	HUINT16					 usDesLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	if (usDesLen < 5)
	{
		return eSIxERR_INVALID;
	}

 	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxFSatLcnSvcInfo_t, pstDes);
		pstNewInfo = allocFSatLcnSvcInfo();
		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		// Service Id (2byte)
		pstNewInfo->usSvcId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;

		// Original Network id (2byte)
		pstNewInfo->usSvcIdentifier = (((HUINT16)HxMACRO_Get16Bit(p)) & 0x7FFF);
		p += 2;
		usDesLen -= 2;

		// LCN loop length
		usLcnLoopLen = (HUINT16)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		while (usLcnLoopLen > 0 && usLcnLoopLen < (HUINT16) MAX_DESCRIPTOR_LIMIT)
		{
			GET_PREV_INFO(pstPrevLcnInfo, SIxFSatLcnInfo_t, pstNewInfo);
			pstNewLcnInfo = allocFSatLcnInfo();

			if (NULL == pstNewLcnInfo)
			{
				goto err_exit;
			}

			ucTmp8bit = ((((HUINT8)HxMACRO_Get8Bit (p)) >> 5) & 0x07);
			pstNewLcnInfo->numeric_selection_flag = (ucTmp8bit >> 2);
			pstNewLcnInfo->visible_selection_flag = (ucTmp8bit >> 1);
			pstNewLcnInfo->user_customisable = (ucTmp8bit);

			pstNewLcnInfo->logical_channel_number = (HUINT16)HxMACRO_Get12Bit (p);
			p += 2;
			usDesLen -= 2; usLcnLoopLen -= 2;

			pstNewLcnInfo->region_id = (HUINT16)HxMACRO_Get16Bit (p);
			p += 2;
			usDesLen -= 2; usLcnLoopLen -= 2;

			LINK_INFO_NODE(pstNewInfo->pstInfo, pstNewLcnInfo, NULL, pstPrevLcnInfo);
		}

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxFSatLcnSvcInfo_t, freeFSatLcnSvcInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyFSatLcnSvcInfo(SIxFSatLcnSvcInfo_t *pstDest, const SIxFSatLcnSvcInfo_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxFSatLcnInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxFSatLcnSvcInfo_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxFSatLcnInfo_t, pstDest);
			pstNewInfo = (SIxFSatLcnInfo_t *)allocFSatLcnInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxFSatLcnInfo_t));
			pstNewInfo->pstNext = NULL;

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstInfo, SIxFSatLcnInfo_t);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyRegionalisedLcnDes(SIxRegionalisedLcnDes_t *pstDest, const SIxRegionalisedLcnDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxFSatLcnSvcInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxRegionalisedLcnDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxFSatLcnSvcInfo_t, pstDest);
			pstNewInfo = (SIxFSatLcnSvcInfo_t *)allocFSatLcnSvcInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxFSatLcnSvcInfo_t));
			pstNewInfo->pstNext = NULL;
			if (copyFSatLcnSvcInfo(pstNewInfo, pstSourceInfo) != eSIxERR_OK)
			{
				silib_desc_free(pstNewInfo);
				goto err_exit;
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxFSatLcnSvcInfo_t, freeFSatLcnSvcInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_REGION_NAME					 (TAG = 0xD4)
 ************************************************************************/
//RegionName
static void initRegionNameDes(SIxRegionNameDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxRegionNameDes_t));
}

static SIxRegionNameDes_t *allocRegionNameDes(void)
{
	return (SIxRegionNameDes_t *)silib_desc_alloc(sizeof(SIxRegionNameDes_t));
}

static SIxRegionNameInfo_t *allocRegionNameInfo(void)
{
	SIxRegionNameInfo_t *pstInfo;

	pstInfo = (SIxRegionNameInfo_t *)silib_desc_alloc(sizeof(SIxRegionNameInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	pstInfo->usRegion_id = 0xffff;
	HxSTD_MemSet(pstInfo->szIso639LangCode,0,4);
	pstInfo->pszRegionName = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeRegionNameInfo(SIxRegionNameInfo_t *pstInfo)
{
	if (pstInfo)
	{
		silib_desc_free(pstInfo->pszRegionName);
		silib_desc_free(pstInfo);
	}
}

static void freeRegionNameDes(SIxRegionNameDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxRegionNameInfo_t, freeRegionNameInfo);
		silib_desc_free(pstDes);
	}
}

static void setRegionNameInfo(SIxRegionNameInfo_t *pstInfo, HUINT16 usRegionId, HUINT8 *pszIso639LangCode, HUINT8 *pszRegionName)
{
	HINT32 nStrLen;

	if (NULL == pstInfo || NULL == pszIso639LangCode || NULL == pszRegionName)
	{
		HxLOG_Error("[%s] invalid param (NULL) \n",__FUNCTION__);
		return;
	}

	pstInfo->usRegion_id = usRegionId;
	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, 3);
	pstInfo->szIso639LangCode[3] = '\0';
	nStrLen = silib_desc_util_dvbStrlen(pszRegionName);
	HxSTD_MemCopy(pstInfo->pszRegionName, pszRegionName, nStrLen);
	pstInfo->pszRegionName[nStrLen] = '\0';
}


static SIxError_e parseRegionNameDes(HUINT8 *p, SIxRegionNameDes_t *pstDes)
{
	SIxRegionNameInfo_t	*pstPrevInfo = NULL, *pstNewInfo;
	HUINT8				 ucRegionNameLen,ucAssignedRegionNameLen;
	HUINT16				 usRegionId,usTempRegionNameLen;
	HUINT8				 szIso639LangCode[4],*pszRegionName = NULL;
	HUINT16				 usDesLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	if (usDesLen < 6)
	{
		return eSIxERR_FAIL;
	}

	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxRegionNameInfo_t, pstDes);
		pstNewInfo = allocRegionNameInfo();

		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		// Region Id (2byte)
		usRegionId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;

		// ISO 639 language code (3byte)
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		p += 3;
		usDesLen -= 3;

		// Region name length (1 byte)
		ucRegionNameLen = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		if(usDesLen < ucRegionNameLen)
		{
			goto err_exit;
		}
		ucAssignedRegionNameLen = ucRegionNameLen;

		// event name
		usTempRegionNameLen = (HUINT16)silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc,ucAssignedRegionNameLen, (const HUINT8 *)p, &pszRegionName);
		if (NULL == pszRegionName)
			goto err_exit;

		p += ucRegionNameLen;
		usDesLen -= ucRegionNameLen;

		pstNewInfo->pszRegionName = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * (usTempRegionNameLen+1));
		if (NULL == pstNewInfo->pszRegionName)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}
		HxSTD_MemSet(pstNewInfo->pszRegionName, '\0', sizeof(HUINT8) * (usTempRegionNameLen+1));


		setRegionNameInfo(pstNewInfo, usRegionId, szIso639LangCode, pszRegionName);
		if (pszRegionName)
		{
			silib_desc_free(pszRegionName);
			pszRegionName = NULL;
		}

		LINK_INFO_NODE (pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	return eSIxERR_OK;

err_exit:
	if (pszRegionName)
	{
		silib_desc_free(pszRegionName);
	}

//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxRegionNameInfo_t, freeRegionNameInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;

}

static SIxError_e copyRegionNameDes(SIxRegionNameDes_t *pstDest, const SIxRegionNameDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxRegionNameInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxRegionNameDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxRegionNameInfo_t, pstDest);
			pstNewInfo = (SIxRegionNameInfo_t *)allocRegionNameInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxRegionNameInfo_t));
			pstNewInfo->pszRegionName = NULL;
			pstNewInfo->pstNext = NULL;

			if (pstSourceInfo->pszRegionName)
			{
				HUINT32		ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszRegionName);

				pstNewInfo->pszRegionName = silib_desc_alloc(ulSize+1);
				if (NULL == pstNewInfo->pszRegionName)
				{
					silib_desc_free(pstNewInfo);
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo->pszRegionName, pstSourceInfo->pszRegionName, ulSize);
				pstNewInfo->pszRegionName[ulSize] = '\0';
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxRegionNameInfo_t, freeRegionNameInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_SERVICE_GROUP					 (TAG = 0xD5)
 ************************************************************************/
//SvcGroup
static void initSvcGroupDes(SIxSvcGroupDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSvcGroupDes_t));
}

static SIxSvcGroupDes_t *allocSvcGroupDes(void)
{
	return (SIxSvcGroupDes_t *)silib_desc_alloc(sizeof(SIxSvcGroupDes_t));
}

static SIxSvcGroupInfo_t *allocSvcGroupInfo(void)
{
	SIxSvcGroupInfo_t *pstInfo;

	pstInfo = (SIxSvcGroupInfo_t *)silib_desc_alloc(sizeof(SIxSvcGroupInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	pstInfo->ucNondestructiveTuneFlag = 0xff;
	pstInfo->ucReturnChannelAccessFlag = 0xff;
	pstInfo->usSvcGroupId = 0xffff;
	pstInfo->pstInfo = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static SIxFSatSvcIdentifierInfo_t *allocSvcIdentifierInfo(void)
{
	SIxFSatSvcIdentifierInfo_t *pstInfo;

	pstInfo = (SIxFSatSvcIdentifierInfo_t *)silib_desc_alloc(sizeof(SIxFSatSvcIdentifierInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	pstInfo->usSvcIdentifier = 0xffff;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeSvcGroupInfo(SIxSvcGroupInfo_t *pstInfo)
{
	if (pstInfo)
	{
		FREE_INFO_LIST(pstInfo->pstInfo, SIxFSatSvcIdentifierInfo_t);
		silib_desc_free(pstInfo);
	}
}

static void freeSvcGroupDes(SIxSvcGroupDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSvcGroupInfo_t, freeSvcGroupInfo);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseSvcGroupDes(HUINT8 *p, SIxSvcGroupDes_t *pstDes)
{
	SIxSvcGroupInfo_t			*pstPrevInfo = NULL, *pstNewInfo;
	SIxFSatSvcIdentifierInfo_t 	*pstPrevSvcInfo = NULL,*pstNewSvcInfo;
	HUINT8				ucNonDestructiveTuneFlag, ucRtnChannelAccessFlag;
	HUINT16 			usFreeSatSvcGrpId;
	HUINT16 			usTmp;
	HUINT16				usDesLen, usGrpMemberLen;
	HUINT8				ucG2ExtensionFlag;
	HUINT8				ucTmp8bit;
	HxLOG_Print(" [%s] (+)\n",__FUNCTION__);

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	if (usDesLen < 3)
	{
		return eSIxERR_FAIL;
	}

	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		usTmp = (HUINT16)HxMACRO_Get16Bit(p);
		ucNonDestructiveTuneFlag = ((usTmp >> 15) & 0x1);
		ucRtnChannelAccessFlag = ((usTmp >> 14) & 0x1);
		ucG2ExtensionFlag = ((usTmp >> 13) & 0x1);
		usFreeSatSvcGrpId = (usTmp & 0x7FF);

		p += 2;
		usDesLen -= 2;
		usGrpMemberLen = (HUINT16)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxSvcGroupInfo_t, pstDes);
		pstNewInfo = allocSvcGroupInfo();

		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		pstNewInfo->ucNondestructiveTuneFlag = ucNonDestructiveTuneFlag;
		pstNewInfo->ucReturnChannelAccessFlag = ucRtnChannelAccessFlag;
		pstNewInfo->ucG2ExtensionFlag = ucG2ExtensionFlag;
		pstNewInfo->usSvcGroupId = usFreeSatSvcGrpId;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);

		while (usGrpMemberLen > 0 && usGrpMemberLen < (HUINT16) MAX_DESCRIPTOR_LIMIT)
		{
			GET_PREV_INFO(pstPrevSvcInfo, SIxFSatSvcIdentifierInfo_t, pstNewInfo);
			pstNewSvcInfo = allocSvcIdentifierInfo();
			if (NULL == pstNewSvcInfo)
			{
				goto err_exit;
			}

			pstNewSvcInfo->usSvcIdentifier = ((HUINT16)HxMACRO_Get16Bit(p)) & 0x7FFF;

			p += 2;
			usDesLen -= 2;

			LINK_INFO_NODE(pstNewInfo->pstInfo, pstNewSvcInfo, NULL, pstPrevSvcInfo);
			usGrpMemberLen -= 2;
		}

		if (1 == pstNewInfo->ucG2ExtensionFlag)
		{
			ucTmp8bit = (HUINT8)((((HUINT8)HxMACRO_Get8Bit (p)) & 0xF0) >> 4);
			pstNewInfo->stFlagInfo.default_visable_flag = (ucTmp8bit >> 3);
			pstNewInfo->stFlagInfo.adult_channels_group_flag = (ucTmp8bit >> 2);
			pstNewInfo->stFlagInfo.ip_channels_group_flag = (ucTmp8bit >> 1);
			pstNewInfo->stFlagInfo.ca_channels_group_flag = (ucTmp8bit);

			p += 1;
			usDesLen -= 1;
		}
	}

	HxLOG_Print(" [%s] (-)\n",__FUNCTION__);

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSvcGroupInfo_t, freeSvcGroupInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copySvcGroupInfo(SIxSvcGroupInfo_t *pstDest, const SIxSvcGroupInfo_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxFSatSvcIdentifierInfo_t *pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcGroupDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxFSatSvcIdentifierInfo_t, pstDest);
			pstNewInfo = allocSvcIdentifierInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxFSatSvcIdentifierInfo_t));
			pstNewInfo->pstNext = NULL;

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstInfo, SIxFSatSvcIdentifierInfo_t);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copySvcGroupDes(SIxSvcGroupDes_t *pstDest, const SIxSvcGroupDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxSvcGroupInfo_t *pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcGroupDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxSvcGroupInfo_t, pstDest);
			pstNewInfo = (SIxSvcGroupInfo_t *)allocSvcGroupInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxSvcGroupInfo_t));
			pstNewInfo->pstNext = NULL;
			if (copySvcGroupInfo(pstNewInfo, pstSourceInfo) != eSIxERR_OK)
			{
				silib_desc_free(pstNewInfo);
				goto err_exit;
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxSvcGroupInfo_t, freeSvcGroupInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE			 (TAG = 0xD6)
 ************************************************************************/
//InteractiveStorage
static void initInteractiveStorageDes(SIxInteractiveStorageDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxInteractiveStorageDes_t));
}

static SIxInteractiveStorageDes_t *allocInteractiveStorageDes(void)
{
	return (SIxInteractiveStorageDes_t *)silib_desc_alloc(sizeof(SIxInteractiveStorageDes_t));
}

static SIxInteractiveStorageInfo_t *allocInteractiveStorageInfo(void)
{
	SIxInteractiveStorageInfo_t *pstInfo;

	pstInfo = (SIxInteractiveStorageInfo_t *)silib_desc_alloc(sizeof(SIxInteractiveStorageInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	HxSTD_MemSet(pstInfo, 0, sizeof(SIxInteractiveStorageInfo_t));
	pstInfo->ucGroupFlag = 0xff;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeInteractiveStorageDes(SIxInteractiveStorageDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST(pstDes->pstInfo, SIxInteractiveStorageInfo_t);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseInteractiveStorageDes(HUINT8 *p, SIxInteractiveStorageDes_t *pstDes)
{
	SIxInteractiveStorageInfo_t		*pstPrevInfo = NULL, *pstNewInfo;
	HUINT8							 ucAllocUnit, ucNumBlocks, ucGroupFlag;
	HUINT16 						 usTmpValue, usFSatSvcGrpId = 0, usSvcIdentifier = 0;
	HUINT16							 usDesLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;
	if (usDesLen < 1)
	{
		return eSIxERR_FAIL;
	}

	/*
		- allocation unit (5 bit) -
		This 5-bit field specifies the log to the base 2 of the size of a single block of non-volatile
		memory. The size of the block in bytes is 2 	to the power of the allocation_unit value. For
		example an allocation_unit value of 10 indicates a block size of 1024 bytes.
	*/
	ucAllocUnit = (((HUINT8)HxMACRO_Get8Bit(p)) & 0x1F);
	pstDes->ucAllocationUnit = ucAllocUnit;
	p += 1;
	usDesLen -= 1;

	// Private Data
	while (usDesLen > 0 && usDesLen < (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		usTmpValue = (HUINT16)HxMACRO_Get16Bit(p);
		/*
			- group_flag -
			This 1-bit field is set to ‘0’ to indicate that the NVM allocation is assigned to a service, or
			'1' to indicate that the allocation is assigned to a group of services.

		 */
		ucGroupFlag = (HUINT8)((HUINT16)(usTmpValue >> 15) & 0x7FFF);
		if (1 == ucGroupFlag)
		{
			/*
				- free_satellite_servicegroup_id -
				This 11-bit field identifies a grouping of free satellite services that have access to this
				allocation non-volatile storage.
			 */
			usFSatSvcGrpId = (HUINT16)(usTmpValue & 0x7FF);
		}
		else
		{
			/*
				- free_satellite_service_identifier -
				This 15-bit field is the free satellite service identifier uniquely assigned to a service by
				the regionalised logical channel descriptor.
				The service assigned this free_satellite_service_identifier has access
				to this allocation of non-volatile storage.
			 */
			usSvcIdentifier = (HUINT16)(usTmpValue & 0x7FFF);
		}
		p += 2;
		usDesLen -= 2;

		/*
			- num_blocks -
			This 8-bit field specifies the number of blocks of non-volatile memory in this allocation.
		*/
		ucNumBlocks = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxInteractiveStorageInfo_t, pstDes);
		pstNewInfo = allocInteractiveStorageInfo();

		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		pstNewInfo->ucGroupFlag = ucGroupFlag;
		if (1 == ucGroupFlag)
		{
			pstNewInfo->unInfo.usSvcGroupId = usFSatSvcGrpId;
		}
		else
		{
			pstNewInfo->unInfo.usSvcIdentifier = usSvcIdentifier;
		}
		pstNewInfo->ucNumBlocks = ucNumBlocks;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}
	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST(pstDes->pstInfo, SIxInteractiveStorageInfo_t);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyInteractiveStorageDes(SIxInteractiveStorageDes_t *pstDest, const SIxInteractiveStorageDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxInteractiveStorageInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxInteractiveStorageDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxInteractiveStorageInfo_t, pstDest);
			pstNewInfo = (SIxInteractiveStorageInfo_t *)allocInteractiveStorageInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxInteractiveStorageInfo_t));
			pstNewInfo->pstNext = NULL;

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstInfo, SIxInteractiveStorageInfo_t);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_INFO_LOCATION					 (TAG = 0xD7)
 ************************************************************************/
//InfoLocation
static void initInfoLocationDes(SIxInfoLocationDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxInfoLocationDes_t));
}

static SIxInfoLocationDes_t *allocInfoLocationDes(void)
{
	return (SIxInfoLocationDes_t *)silib_desc_alloc(sizeof(SIxInfoLocationDes_t));
}

static SIxFSatHomeTpInfo_t *allocHomeTpInfo(void)
{
	SIxFSatHomeTpInfo_t *pstInfo;

	pstInfo = (SIxFSatHomeTpInfo_t *)silib_desc_alloc(sizeof(SIxFSatHomeTpInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	pstInfo->usTsId = 0xffff;
	pstInfo->usOrgNetId = 0xffff;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static SIxTunnelledSvcInfo_t *allocTunnelledSvcInfo(void)
{
	SIxTunnelledSvcInfo_t *pstInfo;

	pstInfo = (SIxTunnelledSvcInfo_t *)silib_desc_alloc(sizeof(SIxTunnelledSvcInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	pstInfo->usOrgNetId = 0xffff;
	pstInfo->usTsId = 0xffff;
	pstInfo->usSvcId = 0xffff;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeInfoLocationDes(SIxInfoLocationDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST(pstDes->pstHomeTpInfo, SIxFSatHomeTpInfo_t);
		FREE_INFO_LIST(pstDes->pstTunnelledSvcInfo, SIxTunnelledSvcInfo_t);
		silib_desc_free(pstDes);
	}
}


static SIxError_e parseInfoLocationDes(HUINT8 *p, SIxInfoLocationDes_t *pstDes)
{
	SIxTunnelledSvcInfo_t		*pstPrevTunnelledSvcInfo, *pstNewTunnelledSvcInfo;
	SIxFSatHomeTpInfo_t			*pstPrevHomeTpInfo, *pstNewHomeTpInfo;
	HUINT16 					 usTsId, usOnId, usSvcId;
	HUINT16						 usDesLen, usTunnelledLoopLen, usHomeLoopLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

#if 0
	if (ucDesLen < 2)
	{
		PrintDebug ("[WARN] Unknown parseFreeSatInfoLocationDes Format!\n");
		return ERR_SI_INVALID_DESCRIPTOR;
	}
#endif

	usHomeLoopLen = (HUINT16)HxMACRO_Get8Bit(p);
	p += 1;
	usDesLen -= 1;

	// Private Data
	while (usHomeLoopLen > 0 && usHomeLoopLen < (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		usTsId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2; usHomeLoopLen -= 2;

		usOnId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2; usHomeLoopLen -= 2;

		// Descriptor 单捞鸥 备己.
		GET_PREV_LINK(pstPrevHomeTpInfo, SIxFSatHomeTpInfo_t, pstDes->pstHomeTpInfo);
		pstNewHomeTpInfo = allocHomeTpInfo();

		if (pstNewHomeTpInfo == NULL)
		{
			goto err_exit;
		}

		pstNewHomeTpInfo->usTsId = usTsId;
		pstNewHomeTpInfo->usOrgNetId = usOnId;

		LINK_INFO_NODE(pstDes->pstHomeTpInfo, pstNewHomeTpInfo, NULL, pstPrevHomeTpInfo);
	}

	usTunnelledLoopLen = (HUINT16)HxMACRO_Get8Bit(p);
	p += 1;
	usDesLen -= 1;

	// Private Data
	while (usTunnelledLoopLen > 0 && usTunnelledLoopLen < (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		usTsId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2; usTunnelledLoopLen -= 2;

		usOnId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2; usTunnelledLoopLen -= 2;

		usSvcId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2; usTunnelledLoopLen -= 2;

		// Descriptor 单捞鸥 备己.
		GET_PREV_LINK(pstPrevTunnelledSvcInfo, SIxTunnelledSvcInfo_t, pstDes->pstTunnelledSvcInfo);
		pstNewTunnelledSvcInfo = allocTunnelledSvcInfo();

		if (pstNewTunnelledSvcInfo == NULL)
		{
			goto err_exit;
		}

		pstNewTunnelledSvcInfo->usTsId = usTsId;
		pstNewTunnelledSvcInfo->usOrgNetId = usOnId;
		pstNewTunnelledSvcInfo->usSvcId = usSvcId;

		LINK_INFO_NODE(pstDes->pstTunnelledSvcInfo, pstNewTunnelledSvcInfo, NULL, pstPrevTunnelledSvcInfo);
	}
	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST(pstDes->pstHomeTpInfo, SIxFSatHomeTpInfo_t);
		FREE_INFO_LIST(pstDes->pstTunnelledSvcInfo, SIxTunnelledSvcInfo_t);
		pstDes->pstHomeTpInfo = NULL;
		pstDes->pstTunnelledSvcInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyInfoLocationDes(SIxInfoLocationDes_t *pstDest, const SIxInfoLocationDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxInfoLocationDes_t));
		pstDest->pstHomeTpInfo = NULL;
		pstDest->pstTunnelledSvcInfo = NULL;

		if (pstSource->pstHomeTpInfo)
		{
			SIxFSatHomeTpInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

			pstSourceInfo = pstSource->pstHomeTpInfo;
			while (pstSourceInfo)
			{
				GET_PREV_LINK(pstPrevInfo, SIxFSatHomeTpInfo_t, pstDest->pstHomeTpInfo);
				pstNewInfo = (SIxFSatHomeTpInfo_t *)allocHomeTpInfo();
				if (pstNewInfo == NULL)
					goto err_exit;

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxFSatHomeTpInfo_t));
				pstNewInfo->pstNext = NULL;

				LINK_INFO_NODE(pstDest->pstHomeTpInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
		}

		if (pstSource->pstTunnelledSvcInfo)
		{
			SIxTunnelledSvcInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

			pstSourceInfo = pstSource->pstTunnelledSvcInfo;
			while (pstSourceInfo)
			{
				GET_PREV_LINK(pstPrevInfo, SIxTunnelledSvcInfo_t, pstDest->pstTunnelledSvcInfo);
				pstNewInfo = (SIxTunnelledSvcInfo_t *)allocTunnelledSvcInfo();
				if (pstNewInfo == NULL)
					goto err_exit;

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxTunnelledSvcInfo_t));
				pstNewInfo->pstNext = NULL;

				LINK_INFO_NODE(pstDest->pstTunnelledSvcInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
		}

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstHomeTpInfo, SIxFSatHomeTpInfo_t);
		pstDest->pstHomeTpInfo = NULL;
		FREE_INFO_LIST(pstDest->pstTunnelledSvcInfo, SIxTunnelledSvcInfo_t);
		pstDest->pstTunnelledSvcInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_SERVICEGROUP_NAME				 (TAG = 0xD8)
 ************************************************************************/
//SvcGroupName
static void initSvcGroupNameDes(SIxSvcGroupNameDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSvcGroupNameDes_t));
}

static SIxSvcGroupNameDes_t *allocSvcGroupNameDes(void)
{
	return (SIxSvcGroupNameDes_t *)silib_desc_alloc(sizeof(SIxSvcGroupNameDes_t));
}

static SIxSvcGroupNameInfo_t *allocSvcGroupNameInfo(void)
{
	SIxSvcGroupNameInfo_t *pstInfo;

	pstInfo = (SIxSvcGroupNameInfo_t *)silib_desc_alloc(sizeof(SIxSvcGroupNameInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	pstInfo->ucGroupType = 0xff;
	pstInfo->usSvcGroupId = 0xffff;
	pstInfo->pstSvcGroupData = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static SIxSvcGroupNameData_t *allocSvcGroupNameData(void)
{
	SIxSvcGroupNameData_t *pstInfo;

	pstInfo = (SIxSvcGroupNameData_t *)silib_desc_alloc(sizeof(SIxSvcGroupNameData_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	HxSTD_MemSet(pstInfo->szIso639LangCode, 0, 4);
	pstInfo->pszSvcGroupName = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeSvcGroupNameData(SIxSvcGroupNameData_t *pstData)
{
	if (pstData)
	{
		silib_desc_free(pstData->pszSvcGroupName);
		silib_desc_free(pstData);
	}
}

static void freeSvcGroupNameInfo(SIxSvcGroupNameInfo_t *pstInfo)
{
	if (pstInfo)
	{
		FREE_INFO_LIST_EX(pstInfo->pstSvcGroupData, SIxSvcGroupNameData_t, freeSvcGroupNameData);
		silib_desc_free(pstInfo);
	}
}

static void freeSvcGroupNameDes(SIxSvcGroupNameDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSvcGroupNameInfo_t, freeSvcGroupNameInfo);
		silib_desc_free(pstDes);
	}
}

static void setSvcGroupNameData(SIxSvcGroupNameData_t *pstData, HUINT8 *pszIso639LangCode, HUINT8 *pszSvcGroupName)
{
	HINT32 nStrLen;

	if (NULL == pstData || NULL == pszIso639LangCode || NULL == pszSvcGroupName)
	{
		HxLOG_Error("[%s] invalid param (NULL) \n",__FUNCTION__);
		return;
	}

	HxSTD_MemCopy(pstData->szIso639LangCode, pszIso639LangCode, 3);
	pstData->szIso639LangCode[3] = '\0';
	nStrLen = silib_desc_util_dvbStrlen(pszSvcGroupName);
	HxSTD_MemCopy(pstData->pszSvcGroupName, pszSvcGroupName, nStrLen);
	pstData->pszSvcGroupName[nStrLen] = '\0';
}


static SIxError_e parseSvcGroupNameDes(HUINT8 *p, SIxSvcGroupNameDes_t *pstDes)
{
	SIxSvcGroupNameInfo_t	*pstPrevInfo = NULL, *pstNewInfo;
	SIxSvcGroupNameData_t	*pstPrevData = NULL, *pstNewData;
	HUINT8					 ucGroupType;
	HUINT16 				 usSvcGroupId, usTmp, usAssignedNameLen, usTmpNameLen;
	HUINT8					 szIso639LangCode[4], *pszGroupName = NULL;
	HUINT16					 usDesLen, usNameLoopLen, usGrpNameLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	if (usDesLen < 3)
	{
		return eSIxERR_FAIL;
	}

	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		usTmp = (HUINT16)HxMACRO_Get16Bit(p);
		ucGroupType = ((usTmp >> 11) & 0x1f);
		usSvcGroupId = (usTmp & 0x7ff);
		p += 2;
		usDesLen -= 2;

		usNameLoopLen = (HUINT16)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxSvcGroupNameInfo_t, pstDes);
		pstNewInfo = allocSvcGroupNameInfo();

		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		pstNewInfo->ucGroupType = ucGroupType;
		pstNewInfo->usSvcGroupId = usSvcGroupId;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);

		while (usNameLoopLen > 0 && usNameLoopLen < (HUINT16) MAX_DESCRIPTOR_LIMIT)
		{
			GET_PREV_LINK(pstPrevData, SIxSvcGroupNameData_t, pstNewInfo->pstSvcGroupData);
			pstNewData = allocSvcGroupNameData();

			if (NULL == pstNewData)
			{
				goto err_exit;
			}

			// ISO 639 language code (3byte)
			HxSTD_MemCopy(szIso639LangCode, p, 3);
			p += 3;
			usDesLen -= 3; usNameLoopLen -= 3;

			usGrpNameLen = (HUINT16)HxMACRO_Get8Bit(p);
			p += 1;
			usDesLen -= 1; usNameLoopLen -= 1;

			if(usDesLen < usGrpNameLen)
			{
				goto err_exit;
			}
			usAssignedNameLen = (usGrpNameLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usGrpNameLen);

			// event name
			usTmpNameLen = (HUINT16)silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc, usAssignedNameLen, (const HUINT8 *)p, &pszGroupName);
			if (NULL == pszGroupName)
			{
				silib_desc_free(pstNewData);
				goto err_exit;
			}

			p += usGrpNameLen;
			usDesLen -= usGrpNameLen;
			usNameLoopLen -= usGrpNameLen;

			pstNewData->pszSvcGroupName = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * (usTmpNameLen+1));
			if (NULL == pstNewData->pszSvcGroupName)
			{
				silib_desc_free(pstNewData);
				goto err_exit;
			}
			HxSTD_MemSet(pstNewData->pszSvcGroupName, '\0', sizeof(HUINT8) * (usTmpNameLen+1));

			setSvcGroupNameData(pstNewData, szIso639LangCode, pszGroupName);

			if (pszGroupName)
			{
				silib_desc_free(pszGroupName);
				pszGroupName= NULL;
			}

			LINK_INFO_NODE(pstNewInfo->pstSvcGroupData, pstNewData, NULL, pstPrevData);
		}
	}
	return eSIxERR_OK;

err_exit:
	if (pszGroupName)
	{
		silib_desc_free(pszGroupName);
	}
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSvcGroupNameInfo_t, freeSvcGroupNameInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copySvcGroupNameData(SIxSvcGroupNameInfo_t *pstDest, const SIxSvcGroupNameInfo_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxSvcGroupNameData_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcGroupNameInfo_t));
		pstDest->pstSvcGroupData = NULL;
		pstDest->pstNext = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstSvcGroupData;
		while (pstSourceInfo)
		{
			GET_PREV_LINK(pstPrevInfo, SIxSvcGroupNameData_t, pstDest->pstSvcGroupData);
			pstNewInfo = allocSvcGroupNameData();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxSvcGroupNameData_t));
			pstNewInfo->pstNext = NULL;
			pstNewInfo->pszSvcGroupName = NULL;

			if (pstSourceInfo->pszSvcGroupName)
			{
				HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszSvcGroupName);

				pstNewInfo->pszSvcGroupName = silib_desc_alloc(ulSize+1);
				if (NULL == pstNewInfo->pszSvcGroupName)
				{
					silib_desc_free(pstNewInfo);
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo->pszSvcGroupName, pstSourceInfo->pszSvcGroupName, ulSize);
				pstNewInfo->pszSvcGroupName[ulSize] = '\0';
			}

			LINK_INFO_NODE(pstDest->pstSvcGroupData, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstSvcGroupData, SIxSvcGroupNameData_t, freeSvcGroupNameData);
		pstDest->pstSvcGroupData = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copySvcGroupNameDes(SIxSvcGroupNameDes_t *pstDest, const SIxSvcGroupNameDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxSvcGroupNameInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcGroupNameDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxSvcGroupNameInfo_t, pstDest);
			pstNewInfo = (SIxSvcGroupNameInfo_t *)allocSvcGroupNameInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxSvcGroupNameInfo_t));
			pstNewInfo->pstNext = NULL;
			pstNewInfo->pstSvcGroupData = NULL;

			if (pstSourceInfo->pstSvcGroupData)
			{
				pstNewInfo->pstSvcGroupData = allocSvcGroupNameData();
				if (NULL == pstNewInfo->pstSvcGroupData)
				{
					silib_desc_free(pstNewInfo);
					goto err_exit;
				}

				if (copySvcGroupNameData(pstNewInfo, pstSourceInfo) != eSIxERR_OK)
				{
					silib_desc_free(pstNewInfo->pstSvcGroupData);
					silib_desc_free(pstNewInfo);
					goto err_exit;
				}
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxSvcGroupNameInfo_t, freeSvcGroupNameInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME				 (TAG = 0xD9)
 ************************************************************************/
//ShortSvcName
static void initShortSvcNameDes(SIxShortSvcNameDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxShortSvcNameDes_t));
}

static SIxShortSvcNameDes_t *allocShortSvcNameDes(void)
{
	return (SIxShortSvcNameDes_t *)silib_desc_alloc(sizeof(SIxShortSvcNameDes_t));
}

static SIxShortSvcNameInfo_t *allocShortSvcNameInfo(void)
{
	SIxShortSvcNameInfo_t *pstInfo;

	pstInfo = (SIxShortSvcNameInfo_t *)silib_desc_alloc(sizeof(SIxShortSvcNameInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	// Init
	HxSTD_MemSet(pstInfo->szIso639LangCode, 0, 4);
	pstInfo->pszShortSvcName = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeShortSvcNameInfo(SIxShortSvcNameInfo_t *pstInfo)
{
	if (pstInfo)
	{
		silib_desc_free(pstInfo->pszShortSvcName);
		silib_desc_free(pstInfo);
	}
}

static void freeShortSvcNameDes(SIxShortSvcNameDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxShortSvcNameInfo_t, freeShortSvcNameInfo);
		silib_desc_free(pstDes);
	}
}

void	setShortSvcNameInfo(SIxShortSvcNameInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszShortSvcName)
{
	HINT32 nStrLen;

	if (NULL == pstInfo || NULL == pszIso639LangCode || NULL == pszShortSvcName)
	{
		HxLOG_Error("[%s] invalid param (NULL) \n",__FUNCTION__);
		return;
	}

	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, 3);
	pstInfo->szIso639LangCode[3] = '\0';
	nStrLen = silib_desc_util_dvbStrlen(pszShortSvcName);
	HxSTD_MemCopy(pstInfo->pszShortSvcName, pszShortSvcName, nStrLen);
	pstInfo->pszShortSvcName[nStrLen] = '\0';
}


static SIxError_e parseShortSvcNameDes(HUINT8 *p, SIxShortSvcNameDes_t *pstDes)
{
	SIxShortSvcNameInfo_t			*pstPrevInfo = NULL, *pstNewInfo;
	HUINT8							ucStrLen,ucAssignedShortSvcNameLen;
	HUINT8							szIso639LangCode[4],*pszShortSvcName = NULL;
	HUINT16 						usTmpShortSvcNameLen;
	HUINT16							usDesLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

#if 0
	if (ucDesLen < 2)
	{
		PrintDebug ("[WARN] Unknown parseFSatShortSvcNameDes Format!\n");
		return ERR_SI_INVALID_DESCRIPTOR;
	}
#endif

	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		GET_PREV_INFO(pstPrevInfo, SIxShortSvcNameInfo_t, pstDes);
		pstNewInfo = allocShortSvcNameInfo();
		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		// ISO 639 language code (3byte)
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		p += 3;
		usDesLen -= 3;

		ucStrLen = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;
		ucAssignedShortSvcNameLen = (ucStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucStrLen);

		// Guidance string.
		usTmpShortSvcNameLen = (HUINT16)silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc, ucAssignedShortSvcNameLen, (const HUINT8 *)p, &pszShortSvcName);
		if (NULL == pszShortSvcName)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		pstNewInfo->pszShortSvcName = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * (usTmpShortSvcNameLen+1));
		if (NULL == pstNewInfo->pszShortSvcName)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		HxSTD_MemSet(pstNewInfo->pszShortSvcName, '\0', sizeof(HUINT8) * (usTmpShortSvcNameLen+1));

		setShortSvcNameInfo(pstNewInfo, szIso639LangCode, pszShortSvcName);

		if (pszShortSvcName)
		{
			silib_desc_free(pszShortSvcName);
			pszShortSvcName = NULL;
		}

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);

		p += ucStrLen;
		usDesLen -= ucStrLen;
	}
	return eSIxERR_OK;

err_exit:
	if (pszShortSvcName)
	{
		silib_desc_free(pszShortSvcName);
	}
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxShortSvcNameInfo_t, freeShortSvcNameInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyShortSvcNameDes(SIxShortSvcNameDes_t *pstDest, const SIxShortSvcNameDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxShortSvcNameInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxShortSvcNameDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxShortSvcNameInfo_t, pstDest);
			pstNewInfo = (SIxShortSvcNameInfo_t *)allocShortSvcNameInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxShortSvcNameInfo_t));
			pstNewInfo->pstNext = NULL;
			pstNewInfo->pszShortSvcName = NULL;

			if (pstSourceInfo->pszShortSvcName)
			{
				HUINT32	ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszShortSvcName);

				pstNewInfo->pszShortSvcName = silib_desc_alloc(ulSize+1);
				if (NULL == pstNewInfo->pszShortSvcName)
				{
					silib_desc_free(pstNewInfo);
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo->pszShortSvcName, pstSourceInfo->pszShortSvcName, ulSize);
				pstNewInfo->pszShortSvcName[ulSize] = '\0';
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxShortSvcNameInfo_t, freeShortSvcNameInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_GUIDANCE 						 (TAG = 0xDA)
 ************************************************************************/
//Guidance
static void initGuidanceDes(SIxGuidanceDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxGuidanceDes_t));
	pstDes->unInfo.pstInfo = NULL;
}

static SIxGuidanceDes_t *allocGuidanceDes(void)
{
	return (SIxGuidanceDes_t *)silib_desc_alloc(sizeof(SIxGuidanceDes_t));
}

static SIxGuidanceInfo_t *allocGuidanceInfo(void)
{
	SIxGuidanceInfo_t *pstInfo;

	pstInfo = (SIxGuidanceInfo_t *)silib_desc_alloc(sizeof(SIxGuidanceInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	HxSTD_MemSet(pstInfo->szIso639LangCode, 0, 4);
	pstInfo->pszGuidance = NULL;

	return pstInfo;
}

static void freeGuidanceDes(SIxGuidanceDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->unInfo.pstInfo);
		silib_desc_free(pstDes);
	}
}

static void setGuidanceInfo(SIxGuidanceInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszGuidance)
{
	HINT32 nStrLen;

	if (NULL == pstInfo || NULL == pszIso639LangCode || NULL == pszGuidance)
	{
		HxLOG_Error("[%s] invalid param (NULL) \n",__FUNCTION__);
		return;
	}

	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, 3);
	pstInfo->szIso639LangCode[3] = '\0';
	nStrLen = silib_desc_util_dvbStrlen(pszGuidance);
	HxSTD_MemCopy(pstInfo->pszGuidance, pszGuidance, nStrLen);
	pstInfo->pszGuidance[nStrLen] = '\0';
}


static SIxError_e parseGuidanceDes(HUINT8 *p, SIxGuidanceDes_t *pstDes)
{
	SIxGuidanceInfo_t	*pstInfo;
	HUINT8				 ucDesTag,ucDesLen;
	HUINT8				 ucGuidanceType,ucStrLen,ucAssignedGuidanceLen;
	HUINT8				 szIso639LangCode[4],*pszGuidance = NULL;
	HUINT16 			 usTmpNameLen;

	// descriptor tag (1byte)
	ucDesTag = *p++;

	// descriptor length (1byte)
	ucDesLen = *p++;

#if 0
	if (ucDesLen < 2)
	{
		PrintDebug ("[WARN] Unknown parseFSatGuidanceDes Format!\n");
		return ERR_SI_INVALID_DESCRIPTOR;
	}
#endif

	// Guidance Type (2 bit)
	ucGuidanceType = (((HUINT8)HxMACRO_Get8Bit(p)) & 0x3);
	p += 1;
	ucDesLen -= 1;
	pstDes->ucGuidanceType = ucGuidanceType;

	if (0 == ucGuidanceType)
	{
		pstInfo = allocGuidanceInfo();
		if (NULL == pstInfo)
		{
			goto err_exit;
		}

		// ISO 639 language code (3byte)
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		p += 3;
		ucDesLen -= 3;
		ucStrLen = ucDesLen;
		ucAssignedGuidanceLen = (ucStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucStrLen);

		// Guidance string.
		usTmpNameLen = (HUINT16)silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT8)ucAssignedGuidanceLen, (const HUINT8 *)p, &pszGuidance);
		if (NULL == pszGuidance)
		{
			silib_desc_free(pstInfo);
			goto err_exit;
		}

		pstInfo->pszGuidance = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * (usTmpNameLen+1));
		if (NULL == pstInfo->pszGuidance)
		{
			silib_desc_free(pstInfo);
			goto err_exit;
		}
		HxSTD_MemSet(pstInfo->pszGuidance, '\0', sizeof(HUINT8) * (usTmpNameLen+1));

		setGuidanceInfo(pstInfo, szIso639LangCode, pszGuidance);
		if (pszGuidance)
		{
			silib_desc_free(pszGuidance);
			pszGuidance = NULL;
		}

		pstDes->unInfo.pstInfo = pstInfo;
	}
	else
	{
		/* Nothing */
	}
	return eSIxERR_OK;

err_exit:
	if (pszGuidance)
	{
		silib_desc_free(pszGuidance);
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyGuidanceDes(SIxGuidanceDes_t *pstDest, const SIxGuidanceDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxGuidanceDes_t));
		pstDest->unInfo.pstInfo = NULL;
		if (pstSource->unInfo.pstInfo)
		{
			pstDest->unInfo.pstInfo = allocGuidanceInfo();
			if (NULL == pstDest->unInfo.pstInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->unInfo.pstInfo, pstSource->unInfo.pstInfo, sizeof(SIxGuidanceInfo_t));
			pstDest->unInfo.pstInfo->pszGuidance = NULL;

			if (pstSource->unInfo.pstInfo->pszGuidance)
			{
				HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSource->unInfo.pstInfo->pszGuidance);
				pstDest->unInfo.pstInfo->pszGuidance = silib_desc_alloc(ulSize+1);
				if (NULL == pstDest->unInfo.pstInfo->pszGuidance)
					goto err_exit;

				HxSTD_MemCopy(pstDest->unInfo.pstInfo->pszGuidance, pstSource->unInfo.pstInfo->pszGuidance, ulSize);
				pstDest->unInfo.pstInfo->pszGuidance[ulSize] = '\0';
			}
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest->unInfo.pstInfo)
	{
		silib_desc_free(pstDest->unInfo.pstInfo->pszGuidance);
		silib_desc_free(pstDest->unInfo.pstInfo);
		pstDest->unInfo.pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION		 (TAG = 0xDB)
 ************************************************************************/
//InteractiveRestriction
static void initInteractiveRestrictionDes(SIxInteractiveRestrictionDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxInteractiveRestrictionDes_t));
}

static SIxInteractiveRestrictionDes_t *allocInteractiveRestrictionDes(void)
{
	return (SIxInteractiveRestrictionDes_t *)silib_desc_alloc(sizeof(SIxInteractiveRestrictionDes_t));
}

static SIxInteractiveRestrictionInfo_t *allocInteractiveRestrictionInfo(void)
{
	SIxInteractiveRestrictionInfo_t *pstInfo;

	pstInfo = (SIxInteractiveRestrictionInfo_t *)silib_desc_alloc(sizeof(SIxInteractiveRestrictionInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}
	pstInfo->usSvcId = 0xFFFF;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeInteractiveRestrictionDes(SIxInteractiveRestrictionDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST(pstDes->pstInfo, SIxInteractiveRestrictionInfo_t);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseInteractiveRestrictionDes(HUINT8 *p, SIxInteractiveRestrictionDes_t *pstDes)
{
	SIxInteractiveRestrictionInfo_t		*pstPrevInfo = NULL, *pstNewInfo;
	HUINT16 			usSvcId;
	HUINT16				usDesLen;
	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;
	if (usDesLen < 2)
	{
		return eSIxERR_FAIL;
	}

	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		// CA system id (2byte)
		usSvcId = (HUINT16)HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;

		// Descriptor 单捞鸥 备己.
		GET_PREV_INFO(pstPrevInfo, SIxInteractiveRestrictionInfo_t, pstDes);
		pstNewInfo = allocInteractiveRestrictionInfo();

		if (pstNewInfo == NULL)
		{
			goto err_exit;
		}

		pstNewInfo->usSvcId = usSvcId;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}
	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST(pstDes->pstInfo, SIxInteractiveRestrictionInfo_t);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyInteractiveRestrictionDes(SIxInteractiveRestrictionDes_t *pstDest, const SIxInteractiveRestrictionDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxInteractiveRestrictionInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxInteractiveRestrictionDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxInteractiveRestrictionInfo_t, pstDest);
			pstNewInfo = (SIxInteractiveRestrictionInfo_t *)allocInteractiveRestrictionInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxInteractiveRestrictionInfo_t));
			pstNewInfo->pstNext = NULL;

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstInfo, SIxInteractiveRestrictionInfo_t);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT				 (TAG = 0xDC)
 ************************************************************************/
//ContentManagement
static void initContentManagementDes(SIxContentManagementDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxContentManagementDes_t));
}

static SIxContentManagementDes_t *allocContentManagementDes(void)
{
	return (SIxContentManagementDes_t *)silib_desc_alloc(sizeof(SIxContentManagementDes_t));
}

static void freeContentManagementDes(SIxContentManagementDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseContentManagementDes(HUINT8 *p, SIxContentManagementDes_t *pstDes)
{
	HUINT8				ucDesLen, ucTmpValue;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;

	if (ucDesLen < 1)
	{
		return eSIxERR_FAIL;
	}

	// descriptor whole contents (1byte)
	ucTmpValue = *p++;

	pstDes->ucDoNotScramble = ((ucTmpValue >> 3) & 0x01);
	pstDes->ucNotRedistributeOverInternet = ((ucTmpValue >> 1) & 0x3);
	pstDes->ucDoNotApplyRevocation = (ucTmpValue & 0x01);
	return eSIxERR_OK;
}

static SIxError_e copyContentManagementDes(SIxContentManagementDes_t *pstDest, const SIxContentManagementDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxContentManagementDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY			 (TAG = 0xDE)
 ************************************************************************/
//OnlineAvailability
static void initOnlineAvailabilityDes(SIxOnlineAvailabilityDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxOnlineAvailabilityDes_t));
}

static SIxOnlineAvailabilityDes_t *allocOnlineAvailabilityDes(void)
{
	return (SIxOnlineAvailabilityDes_t *)silib_desc_alloc(sizeof(SIxOnlineAvailabilityDes_t));
}

static void freeOnlineAvailabilityDes(SIxOnlineAvailabilityDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseOnlineAvailabilityDes(HUINT8 *p, SIxOnlineAvailabilityDes_t *pstDes)
{
	HUINT8				ucDesLen, ucTmpValue;
	HUINT8				ucUtcTime[5];
	UNIXTIME			ulStartDateTime, ulStopDateTime;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;

	if (ucDesLen < 1)
	{
		return eSIxERR_FAIL;
	}

	// descriptor media_usage (1byte)
	ucTmpValue = *p++;

	pstDes->ucMediaUsage = ucTmpValue;

	// descriptor available_start_datetime (5byte)
	HxSTD_MemCopy(ucUtcTime, p, 5);
	p += 5;
	ucDesLen -= 5;
	HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime (ucUtcTime, &ulStartDateTime);

	pstDes->StartDateTime = ulStartDateTime;

	// descriptor available_stop_datetime (5byte)
	HxSTD_MemCopy(ucUtcTime, p, 5);
	p += 5;
	ucDesLen -= 5;
	HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime (ucUtcTime, &ulStopDateTime);

	pstDes->StopDateTime = ulStopDateTime;

	// descriptor available_now (1 bit)
	ucTmpValue = (HUINT8)((((HUINT8)HxMACRO_Get8Bit(p)) & 0x80) >> 7);
	p += 1;
	ucDesLen -= 1;

	pstDes->bAvailableNow = (HBOOL)ucTmpValue;

	return eSIxERR_OK;
}

static SIxError_e copyOnlineAvailabilityDes(SIxOnlineAvailabilityDes_t *pstDest, const SIxOnlineAvailabilityDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxOnlineAvailabilityDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_PREFIX							 (TAG = 0xDF)
 ************************************************************************/
//Prefix
static void initPrefixDes(SIxPrefixDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxPrefixDes_t));
}

static SIxPrefixDes_t *allocPrefixDes(void)
{
	return (SIxPrefixDes_t *)silib_desc_alloc(sizeof(SIxPrefixDes_t));
}

static SIxPrefixInfo_t *allocPrefixInfo(void)
{
	SIxPrefixInfo_t *pstInfo;

	pstInfo = (SIxPrefixInfo_t *)silib_desc_alloc(sizeof(SIxPrefixInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	// Init
	pstInfo->ucMediaURIPrefixIdx = 0xFF;
	pstInfo->pszURIPrefix = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freePrefixInfo(SIxPrefixInfo_t *pstInfo)
{
	if (pstInfo)
	{
		silib_desc_free(pstInfo->pszURIPrefix);
		silib_desc_free(pstInfo);
	}
}

static void freePrefixDes(SIxPrefixDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxPrefixInfo_t, freePrefixInfo);
		silib_desc_free(pstDes);
	}
}

static void setPrefixInfo(SIxPrefixInfo_t *pstInfo, HUINT8 *pszPrefix)
{
	HINT32 nStrLen;

	nStrLen = silib_desc_util_dvbStrlen(pszPrefix);
	if(pszPrefix != NULL)
	{
		HxSTD_MemCopy(pstInfo->pszURIPrefix, pszPrefix, nStrLen);
	}
	pstInfo->pszURIPrefix[nStrLen] = '\0';
}

static SIxError_e parsePrefixDes(HUINT8 *p, SIxPrefixDes_t *pstDes)
{
	SIxPrefixInfo_t	*pstNewInfo = NULL, *pstPrevInfo = NULL;
	HUINT8			*pszUriPrefix = NULL;
	HUINT16 		 usDesLen, usStrLen, usAssignedUriPrefixLen, usTmpUriPrefixLen;
	HUINT8			 ucUriPrefixIdx;

	HxLOG_Info("parsePrefixDes () Entered!");
	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		//descriptor UriPrefixIdx (7 bit)
		ucUriPrefixIdx = (HUINT8)(((HUINT8)HxMACRO_Get8Bit(p)) & 0x7F);
		p += 1;
		usDesLen -= 2;

		//descriptor UriPrefixname (1 Byte)
		usStrLen = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		GET_PREV_INFO(pstPrevInfo, SIxPrefixInfo_t, pstDes);
		pstNewInfo = allocPrefixInfo();

		if (pstNewInfo == NULL)
		{
			goto err_exit;
		}

		pstNewInfo->ucMediaURIPrefixIdx = ucUriPrefixIdx;

		usAssignedUriPrefixLen = (usStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usStrLen);
		usTmpUriPrefixLen = (HUINT16)silib_desc_util_textToUTF8((HUINT8 *)NULL, pstDes->stHead.stParam.eTextEnc, usAssignedUriPrefixLen, (const HUINT8 *)p, &pszUriPrefix);
		if (NULL == pszUriPrefix)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		pstNewInfo->pszURIPrefix = silib_desc_alloc(usTmpUriPrefixLen+1);
		if (NULL == pstNewInfo->pszURIPrefix)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		setPrefixInfo(pstNewInfo, pszUriPrefix);

		if (pszUriPrefix)
		{
			silib_desc_free(pszUriPrefix);
			pszUriPrefix = NULL;
		}

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);

		p += usStrLen;
		usDesLen -= usStrLen;
	}

	HxLOG_Info("parsePrefixDes () Leaved!");
	return eSIxERR_OK;

err_exit:
	if (pszUriPrefix)
	{
		silib_desc_free(pszUriPrefix);
	}
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxPrefixInfo_t, freePrefixInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyPrefixDes(SIxPrefixDes_t *pstDest, const SIxPrefixDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxPrefixInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxPrefixDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxPrefixInfo_t, pstDest);
			pstNewInfo = (SIxPrefixInfo_t *)allocPrefixInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxPrefixInfo_t));
			pstNewInfo->pstNext = NULL;

			if (pstSourceInfo->pszURIPrefix)
			{
				HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszURIPrefix);

				pstNewInfo->pszURIPrefix = silib_desc_alloc(ulSize+1);
				if (NULL == pstNewInfo->pszURIPrefix)
				{
					silib_desc_free(pstNewInfo);
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo->pszURIPrefix, pstSourceInfo->pszURIPrefix, ulSize);
				pstNewInfo->pszURIPrefix[ulSize] = '\0';
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxPrefixInfo_t, freePrefixInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_FSAT_MEDIA_LOCATOR					 (TAG = 0xE0)
 ************************************************************************/
//MediaLocator
static void initMediaLocatorDes(SIxMediaLocatorDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxMediaLocatorDes_t));
}

static SIxMediaLocatorDes_t *allocMediaLocatorDes(void)
{
	return (SIxMediaLocatorDes_t *)silib_desc_alloc(sizeof(SIxMediaLocatorDes_t));
}

static void freeMediaLocatorDes(SIxMediaLocatorDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pszURIChar);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseMediaLocatorDes(HUINT8 *p, SIxMediaLocatorDes_t *pstDes)
{
	HUINT8				 ucDesLen, ucTmpValue;
	HUINT8				 ucAssignedUriPrefixLen, ucStrLen;
	HUINT32 			 ulTmpUriPrefixLen;
	HBOOL				 bMediaPrefixLocated;
	HUINT8				 ucMediaPrefixIdx;
	HUINT8				*pszUriChar;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	ucDesLen = *p++;

	if (ucDesLen < 1)
	{
		return eSIxERR_FAIL;
	}

	// descriptor media_type (1byte)
	ucTmpValue = *p++;

	pstDes->ucMediaType = ucTmpValue;

	// descriptor media_usage (1byte)
	ucTmpValue = *p++;

	pstDes->ucMediaUsage = ucTmpValue;


	// descriptor media_prefix_location (1 bit)
	bMediaPrefixLocated = (HBOOL)((((HUINT8)HxMACRO_Get8Bit(p)) & 0x80) >> 7);


	// descriptor media_URI_prefix_idx (7 bit)
	ucMediaPrefixIdx = (HUINT8)(((HUINT8)HxMACRO_Get8Bit(p)) & 0x7F);

	p += 1;
	ucDesLen -= 1;

	pstDes->bMediaPrefixLocated = bMediaPrefixLocated;
	pstDes->ucUriPrefixIdx = ucMediaPrefixIdx;

	//descriptor URI_length (1 Byte)
	ucStrLen = (HUINT8)HxMACRO_Get8Bit(p);
	p += 1;
	ucDesLen -= 1;

	//descriptor URI_prefix char
	pszUriChar = NULL;
	ucAssignedUriPrefixLen = (ucStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucStrLen);
	ulTmpUriPrefixLen = silib_desc_util_textToUTF8((HUINT8 *) NULL, pstDes->stHead.stParam.eTextEnc, ucAssignedUriPrefixLen, (const HUINT8 *)p, &pszUriChar);
	if (NULL == pszUriChar)
	{
		goto err_exit;
	}

	p += ucStrLen;
	ucDesLen -= ucStrLen;

	pstDes->pszURIChar = silib_desc_alloc(ulTmpUriPrefixLen+1);
	if (NULL == pstDes->pszURIChar)
	{
		goto err_exit;
	}

	ucStrLen = silib_desc_util_dvbStrlen(pszUriChar);
	if(pszUriChar != NULL)
		HxSTD_MemCopy(pstDes->pszURIChar, pszUriChar, ucStrLen);
	pstDes->pszURIChar[ucStrLen] = '\0';

	if (pszUriChar)
	{
		silib_desc_free(pszUriChar);
		pszUriChar = NULL;
	}
	return eSIxERR_OK;

err_exit:
	if (pszUriChar)
	{
		silib_desc_free(pszUriChar);
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyMediaLocatorDes(SIxMediaLocatorDes_t *pstDest, const SIxMediaLocatorDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMediaLocatorDes_t));
		pstDest->pszURIChar = NULL;

		if (pstSource->pszURIChar)
		{
			HUINT32	ulSize = silib_desc_util_dvbStrlen(pstSource->pszURIChar);

			pstDest->pszURIChar = silib_desc_alloc(ulSize+1);
			if (NULL == pstDest->pszURIChar)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pszURIChar, pstSource->pszURIChar, ulSize);
			pstDest->pszURIChar[ulSize] = '\0';
		}

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pszURIChar);
		pstDest->pszURIChar = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL		 (TAG = 0xE1)
 ************************************************************************/
//ShortEventPromotional
static void initShortEventPromotionalDes(SIxShortEventPromotionalDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxShortEventPromotionalDes_t));
}

static SIxShortEventPromotionalDes_t *allocShortEventPromotionalDes(void)
{
	return (SIxShortEventPromotionalDes_t *)silib_desc_alloc(sizeof(SIxShortEventPromotionalDes_t));
}

static SIxShortEventPromotionalInfo_t *allocShortEventPromotionalInfo(void)
{
	SIxShortEventPromotionalInfo_t *pstInfo;

	pstInfo = (SIxShortEventPromotionalInfo_t *)silib_desc_alloc(sizeof(SIxShortEventPromotionalInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	// Init
	HxSTD_MemSet(pstInfo->szIso639LangCode,0,4);
	pstInfo->pszShortEventPromotional = NULL;
	pstInfo->pstNext = NULL;

	return pstInfo;
}

static void freeShortEventPromotionalInfo(SIxShortEventPromotionalInfo_t	*pstInfo)
{
	if (pstInfo)
	{
		silib_desc_free(pstInfo->pszShortEventPromotional);
		silib_desc_free(pstInfo);
	}
}

static void freeShortEventPromotionalDes(SIxShortEventPromotionalDes_t *pstDes)
{
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxShortEventPromotionalInfo_t, freeShortEventPromotionalInfo);
		silib_desc_free(pstDes);
	}
}

static void setShortEventPromotionalInfo(SIxShortEventPromotionalInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszShortEventPromotional)
{
	HINT32 nStrLen;

	nStrLen = silib_desc_util_dvbStrlen(pszIso639LangCode);
	if(pszIso639LangCode != NULL)
		HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, nStrLen);
	pstInfo->szIso639LangCode[nStrLen] = '\0';

	nStrLen = silib_desc_util_dvbStrlen(pszShortEventPromotional);
	if(pszShortEventPromotional != NULL)
		HxSTD_MemCopy(pstInfo->pszShortEventPromotional, pszShortEventPromotional, nStrLen);
	pstInfo->pszShortEventPromotional[nStrLen] = '\0';
}

static SIxError_e parseShortEventPromotionalDes(HUINT8 *p, SIxShortEventPromotionalDes_t *pstDes)
{
	SIxShortEventPromotionalInfo_t	*pstPrevInfo = NULL, *pstNewInfo;
	HUINT8							 ucStrLen, ucAssignedShortEventPromotionalLen;
	HUINT8							 szIso639LangCode[4], *pszShortEventPromotional = NULL;
	HUINT16 						 usTmpShortEventPromotionalLen;
	HUINT16							 usDesLen;

	// descriptor tag (1byte)
	p++;

	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	// Private Data
	while (usDesLen > 0 && usDesLen <= (HUINT16) MAX_DESCRIPTOR_LIMIT)
	{
		GET_PREV_INFO(pstPrevInfo, SIxShortEventPromotionalInfo_t, pstDes);
		pstNewInfo = allocShortEventPromotionalInfo();
		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		// ISO 639 language code (3byte)
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		p += 3;
		usDesLen -= 3;

		ucStrLen = (HUINT8)HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;
		ucAssignedShortEventPromotionalLen = (ucStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucStrLen);

		// short event promotional info string.
		usTmpShortEventPromotionalLen = (HUINT16)silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc, ucAssignedShortEventPromotionalLen, (const HUINT8 *)p, &pszShortEventPromotional);
		if (NULL == pszShortEventPromotional)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		pstNewInfo->pszShortEventPromotional = silib_desc_alloc(usTmpShortEventPromotionalLen+1);
		if (NULL == pstNewInfo->pszShortEventPromotional)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		setShortEventPromotionalInfo(pstNewInfo, szIso639LangCode, pszShortEventPromotional);

		if (pszShortEventPromotional)
		{
			silib_desc_free(pszShortEventPromotional);
			pszShortEventPromotional = NULL;
		}

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);

		p += ucStrLen;
		usDesLen -= ucStrLen;
	}
	return eSIxERR_OK;

err_exit:
	if (pszShortEventPromotional)
	{
		silib_desc_free ( pszShortEventPromotional );
	}

//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxShortEventPromotionalInfo_t, freeShortEventPromotionalInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyShortEventPromotionalDes(SIxShortEventPromotionalDes_t *pstDest, const SIxShortEventPromotionalDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxShortEventPromotionalInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxShortEventPromotionalDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxShortEventPromotionalInfo_t, pstDest);
			pstNewInfo = (SIxShortEventPromotionalInfo_t *)allocShortEventPromotionalInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxShortEventPromotionalInfo_t));
			pstNewInfo->pstNext = NULL;
			pstNewInfo->pszShortEventPromotional = NULL;

			if (pstSourceInfo->pszShortEventPromotional)
			{
				HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszShortEventPromotional);

				pstNewInfo->pszShortEventPromotional = silib_desc_alloc(ulSize+1);
				if (NULL == pstNewInfo->pszShortEventPromotional)
				{
					silib_desc_free(pstNewInfo);
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo->pszShortEventPromotional, pstSourceInfo->pszShortEventPromotional, ulSize);
				pstNewInfo->pszShortEventPromotional[ulSize] = '\0';
			}

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxShortEventPromotionalInfo_t, freeShortEventPromotionalInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}



// ==================================================================================
// = Internal Function Template
// = DO NOT Delete!!!
// ==================================================================================
#if 0
//XXX
static SIxDescriptor_t *silib_desc_allocXXXDes(void)
{
	SIxXXXDes_t	*pstDes;

	pstDes = allocXXXDes();
	if (pstDes)
	{
		initXXXDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeXXXDes(SIxDescriptor_t *pstDes)
{
	freeXXXDes((SIxXXXDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseXXXDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseXXXDes((HUINT8 *)pucRawData, (SIxXXXDes_t *)pstDes);
}

static SIxError_e silib_desc_copyXXXDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyXXXDes((SIxXXXDes_t *)pstDest, (SIxXXXDes_t *)pstSource);
}

static void silib_desc_printXXXDes(const SIxDescriptor_t *pstDes)
{
	//(SIxXXXDes_t *)pstDes;
}
#endif


/************************************************************************
 * eSIxDESCTAG_FSAT_TUNNELLED_DATA					 (TAG = 0xD0)
 ************************************************************************/
//TunnelledData
static SIxDescriptor_t *silib_desc_allocTunnelledDataDes(void)
{
	SIxTunnelledDataDes_t	*pstDes;

	pstDes = allocTunnelledDataDes();
	if (pstDes)
	{
		initTunnelledDataDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeTunnelledDataDes(SIxDescriptor_t *pstDes)
{
	freeTunnelledDataDes((SIxTunnelledDataDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseTunnelledDataDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseTunnelledDataDes((HUINT8 *)pucRawData, (SIxTunnelledDataDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTunnelledDataDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTunnelledDataDes((SIxTunnelledDataDes_t *)pstDest, (SIxTunnelledDataDes_t *)pstSource);
}

static void silib_desc_printTunnelledDataDes(const SIxDescriptor_t *pstDes)
{
	//(SIxTunnelledDataDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA				 (TAG = 0xD1)
 ************************************************************************/
//AltTunnelledData
static SIxDescriptor_t *silib_desc_allocAltTunnelledDataDes(void)
{
	SIxAltTunnelledDataDes_t	*pstDes;

	pstDes = allocAltTunnelledDataDes();
	if (pstDes)
	{
		initAltTunnelledDataDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeAltTunnelledDataDes(SIxDescriptor_t *pstDes)
{
	freeAltTunnelledDataDes((SIxAltTunnelledDataDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseAltTunnelledDataDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseAltTunnelledDataDes((HUINT8 *)pucRawData, (SIxAltTunnelledDataDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAltTunnelledDataDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAltTunnelledDataDes((SIxAltTunnelledDataDes_t *)pstDest, (SIxAltTunnelledDataDes_t *)pstSource);
}

static void silib_desc_printAltTunnelledDataDes(const SIxDescriptor_t *pstDes)
{
	//(SIxAltTunnelledDataDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_LINKAGE						 (TAG = 0xD2)
 ************************************************************************/
//FSatLinkage
static SIxDescriptor_t *silib_desc_allocFSatLinkageDes(void)
{
	SIxFSatLinkageDes_t	*pstDes;

	pstDes = allocFSatLinkageDes();
	if (pstDes)
	{
		initFSatLinkageDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeFSatLinkageDes(SIxDescriptor_t *pstDes)
{
	freeFSatLinkageDes((SIxFSatLinkageDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseFSatLinkageDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseFSatLinkageDes((HUINT8 *)pucRawData, (SIxFSatLinkageDes_t *)pstDes);
}

static SIxError_e silib_desc_copyFSatLinkageDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyFSatLinkageDes((SIxFSatLinkageDes_t *)pstDest, (SIxFSatLinkageDes_t *)pstSource);
}

static void silib_desc_printFSatLinkageDes(const SIxDescriptor_t *pstDes)
{
	//(SIxFSatLinkageDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL 	 (TAG = 0xD3)
 ************************************************************************/
//RegionalisedLcn
static SIxDescriptor_t *silib_desc_allocRegionalisedLcnDes(void)
{
	SIxRegionalisedLcnDes_t	*pstDes;

	pstDes = allocRegionalisedLcnDes();
	if (pstDes)
	{
		initRegionalisedLcnDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeRegionalisedLcnDes(SIxDescriptor_t *pstDes)
{
	freeRegionalisedLcnDes((SIxRegionalisedLcnDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseRegionalisedLcnDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseRegionalisedLcnDes((HUINT8 *)pucRawData, (SIxRegionalisedLcnDes_t *)pstDes);
}

static SIxError_e silib_desc_copyRegionalisedLcnDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyRegionalisedLcnDes((SIxRegionalisedLcnDes_t *)pstDest, (SIxRegionalisedLcnDes_t *)pstSource);
}

static void silib_desc_printRegionalisedLcnDes(const SIxDescriptor_t *pstDes)
{
	//(SIxRegionalisedLcnDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_REGION_NAME					 (TAG = 0xD4)
 ************************************************************************/
//RegionName
static SIxDescriptor_t *silib_desc_allocRegionNameDes(void)
{
	SIxRegionNameDes_t	*pstDes;

	pstDes = allocRegionNameDes();
	if (pstDes)
	{
		initRegionNameDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeRegionNameDes(SIxDescriptor_t *pstDes)
{
	freeRegionNameDes((SIxRegionNameDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseRegionNameDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseRegionNameDes((HUINT8 *)pucRawData, (SIxRegionNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyRegionNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyRegionNameDes((SIxRegionNameDes_t *)pstDest, (SIxRegionNameDes_t *)pstSource);
}

static void silib_desc_printRegionNameDes(const SIxDescriptor_t *pstDes)
{
	//(SIxRegionNameDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_SERVICE_GROUP					 (TAG = 0xD5)
 ************************************************************************/
//SvcGroup
static SIxDescriptor_t *silib_desc_allocSvcGroupDes(void)
{
	SIxSvcGroupDes_t	*pstDes;

	pstDes = allocSvcGroupDes();
	if (pstDes)
	{
		initSvcGroupDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSvcGroupDes(SIxDescriptor_t *pstDes)
{
	freeSvcGroupDes((SIxSvcGroupDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSvcGroupDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSvcGroupDes((HUINT8 *)pucRawData, (SIxSvcGroupDes_t *)pstDes);
}

static SIxError_e silib_desc_copySvcGroupDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySvcGroupDes((SIxSvcGroupDes_t *)pstDest, (SIxSvcGroupDes_t *)pstSource);
}

static void silib_desc_printSvcGroupDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSvcGroupDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE			 (TAG = 0xD6)
 ************************************************************************/
//InteractiveStorage
static SIxDescriptor_t *silib_desc_allocInteractiveStorageDes(void)
{
	SIxInteractiveStorageDes_t	*pstDes;

	pstDes = allocInteractiveStorageDes();
	if (pstDes)
	{
		initInteractiveStorageDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeInteractiveStorageDes(SIxDescriptor_t *pstDes)
{
	freeInteractiveStorageDes((SIxInteractiveStorageDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseInteractiveStorageDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseInteractiveStorageDes((HUINT8 *)pucRawData, (SIxInteractiveStorageDes_t *)pstDes);
}

static SIxError_e silib_desc_copyInteractiveStorageDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyInteractiveStorageDes((SIxInteractiveStorageDes_t *)pstDest, (SIxInteractiveStorageDes_t *)pstSource);
}

static void silib_desc_printInteractiveStorageDes(const SIxDescriptor_t *pstDes)
{
	//(SIxInteractiveStorageDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_INFO_LOCATION					 (TAG = 0xD7)
 ************************************************************************/
//InfoLocation
static SIxDescriptor_t *silib_desc_allocInfoLocationDes(void)
{
	SIxInfoLocationDes_t	*pstDes;

	pstDes = allocInfoLocationDes();
	if (pstDes)
	{
		initInfoLocationDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeInfoLocationDes(SIxDescriptor_t *pstDes)
{
	freeInfoLocationDes((SIxInfoLocationDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseInfoLocationDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseInfoLocationDes((HUINT8 *)pucRawData, (SIxInfoLocationDes_t *)pstDes);
}

static SIxError_e silib_desc_copyInfoLocationDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyInfoLocationDes((SIxInfoLocationDes_t *)pstDest, (SIxInfoLocationDes_t *)pstSource);
}

static void silib_desc_printInfoLocationDes(const SIxDescriptor_t *pstDes)
{
	//(SIxInfoLocationDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_SERVICEGROUP_NAME				 (TAG = 0xD8)
 ************************************************************************/
//SvcGroupName
static SIxDescriptor_t *silib_desc_allocSvcGroupNameDes(void)
{
	SIxSvcGroupNameDes_t	*pstDes;

	pstDes = allocSvcGroupNameDes();
	if (pstDes)
	{
		initSvcGroupNameDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSvcGroupNameDes(SIxDescriptor_t *pstDes)
{
	freeSvcGroupNameDes((SIxSvcGroupNameDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSvcGroupNameDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSvcGroupNameDes((HUINT8 *)pucRawData, (SIxSvcGroupNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copySvcGroupNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySvcGroupNameDes((SIxSvcGroupNameDes_t *)pstDest, (SIxSvcGroupNameDes_t *)pstSource);
}

static void silib_desc_printSvcGroupNameDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSvcGroupNameDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME				 (TAG = 0xD9)
 ************************************************************************/
//ShortSvcName
static SIxDescriptor_t *silib_desc_allocShortSvcNameDes(void)
{
	SIxShortSvcNameDes_t	*pstDes;

	pstDes = allocShortSvcNameDes();
	if (pstDes)
	{
		initShortSvcNameDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeShortSvcNameDes(SIxDescriptor_t *pstDes)
{
	freeShortSvcNameDes((SIxShortSvcNameDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseShortSvcNameDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseShortSvcNameDes((HUINT8 *)pucRawData, (SIxShortSvcNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyShortSvcNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyShortSvcNameDes((SIxShortSvcNameDes_t *)pstDest, (SIxShortSvcNameDes_t *)pstSource);
}

static void silib_desc_printShortSvcNameDes(const SIxDescriptor_t *pstDes)
{
	//(SIxShortSvcNameDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_GUIDANCE 						 (TAG = 0xDA)
 ************************************************************************/
//Guidance
static SIxDescriptor_t *silib_desc_allocGuidanceDes(void)
{
	SIxGuidanceDes_t	*pstDes;

	pstDes = allocGuidanceDes();
	if (pstDes)
	{
		initGuidanceDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeGuidanceDes(SIxDescriptor_t *pstDes)
{
	freeGuidanceDes((SIxGuidanceDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseGuidanceDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseGuidanceDes((HUINT8 *)pucRawData, (SIxGuidanceDes_t *)pstDes);
}

static SIxError_e silib_desc_copyGuidanceDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyGuidanceDes((SIxGuidanceDes_t *)pstDest, (SIxGuidanceDes_t *)pstSource);
}

static void silib_desc_printGuidanceDes(const SIxDescriptor_t *pstDes)
{
	//(SIxGuidanceDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION		 (TAG = 0xDB)
 ************************************************************************/
//InteractiveRestriction
static SIxDescriptor_t *silib_desc_allocInteractiveRestrictionDes(void)
{
	SIxInteractiveRestrictionDes_t	*pstDes;

	pstDes = allocInteractiveRestrictionDes();
	if (pstDes)
	{
		initInteractiveRestrictionDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeInteractiveRestrictionDes(SIxDescriptor_t *pstDes)
{
	freeInteractiveRestrictionDes((SIxInteractiveRestrictionDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseInteractiveRestrictionDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseInteractiveRestrictionDes((HUINT8 *)pucRawData, (SIxInteractiveRestrictionDes_t *)pstDes);
}

static SIxError_e silib_desc_copyInteractiveRestrictionDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyInteractiveRestrictionDes((SIxInteractiveRestrictionDes_t *)pstDest, (SIxInteractiveRestrictionDes_t *)pstSource);
}

static void silib_desc_printInteractiveRestrictionDes(const SIxDescriptor_t *pstDes)
{
	//(SIxInteractiveRestrictionDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT				 (TAG = 0xDC)
 ************************************************************************/
//ContentManagement
static SIxDescriptor_t *silib_desc_allocContentManagementDes(void)
{
	SIxContentManagementDes_t	*pstDes;

	pstDes = allocContentManagementDes();
	if (pstDes)
	{
		initContentManagementDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeContentManagementDes(SIxDescriptor_t *pstDes)
{
	freeContentManagementDes((SIxContentManagementDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseContentManagementDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseContentManagementDes((HUINT8 *)pucRawData, (SIxContentManagementDes_t *)pstDes);
}

static SIxError_e silib_desc_copyContentManagementDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyContentManagementDes((SIxContentManagementDes_t *)pstDest, (SIxContentManagementDes_t *)pstSource);
}

static void silib_desc_printContentManagementDes(const SIxDescriptor_t *pstDes)
{
	//(SIxContentManagementDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY			 (TAG = 0xDE)
 ************************************************************************/
//OnlineAvailability
static SIxDescriptor_t *silib_desc_allocOnlineAvailabilityDes(void)
{
	SIxOnlineAvailabilityDes_t	*pstDes;

	pstDes = allocOnlineAvailabilityDes();
	if (pstDes)
	{
		initOnlineAvailabilityDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeOnlineAvailabilityDes(SIxDescriptor_t *pstDes)
{
	freeOnlineAvailabilityDes((SIxOnlineAvailabilityDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseOnlineAvailabilityDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseOnlineAvailabilityDes((HUINT8 *)pucRawData, (SIxOnlineAvailabilityDes_t *)pstDes);
}

static SIxError_e silib_desc_copyOnlineAvailabilityDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyOnlineAvailabilityDes((SIxOnlineAvailabilityDes_t *)pstDest, (SIxOnlineAvailabilityDes_t *)pstSource);
}

static void silib_desc_printOnlineAvailabilityDes(const SIxDescriptor_t *pstDes)
{
	//(SIxOnlineAvailabilityDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_PREFIX							 (TAG = 0xDF)
 ************************************************************************/
//Prefix
static SIxDescriptor_t *silib_desc_allocPrefixDes(void)
{
	SIxPrefixDes_t	*pstDes;

	pstDes = allocPrefixDes();
	if (pstDes)
	{
		initPrefixDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freePrefixDes(SIxDescriptor_t *pstDes)
{
	freePrefixDes((SIxPrefixDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parsePrefixDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parsePrefixDes((HUINT8 *)pucRawData, (SIxPrefixDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPrefixDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPrefixDes((SIxPrefixDes_t *)pstDest, (SIxPrefixDes_t *)pstSource);
}

static void silib_desc_printPrefixDes(const SIxDescriptor_t *pstDes)
{
	//(SIxPrefixDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_MEDIA_LOCATOR					 (TAG = 0xE0)
 ************************************************************************/
//MediaLocator
static SIxDescriptor_t *silib_desc_allocMediaLocatorDes(void)
{
	SIxMediaLocatorDes_t	*pstDes;

	pstDes = allocMediaLocatorDes();
	if (pstDes)
	{
		initMediaLocatorDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeMediaLocatorDes(SIxDescriptor_t *pstDes)
{
	freeMediaLocatorDes((SIxMediaLocatorDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseMediaLocatorDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseMediaLocatorDes((HUINT8 *)pucRawData, (SIxMediaLocatorDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMediaLocatorDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMediaLocatorDes((SIxMediaLocatorDes_t *)pstDest, (SIxMediaLocatorDes_t *)pstSource);
}

static void silib_desc_printMediaLocatorDes(const SIxDescriptor_t *pstDes)
{
	//(SIxMediaLocatorDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL		 (TAG = 0xE1)
 ************************************************************************/
//ShortEventPromotional
static SIxDescriptor_t *silib_desc_allocShortEventPromotionalDes(void)
{
	SIxShortEventPromotionalDes_t	*pstDes;

	pstDes = allocShortEventPromotionalDes();
	if (pstDes)
	{
		initShortEventPromotionalDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeShortEventPromotionalDes(SIxDescriptor_t *pstDes)
{
	freeShortEventPromotionalDes((SIxShortEventPromotionalDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseShortEventPromotionalDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseShortEventPromotionalDes((HUINT8 *)pucRawData, (SIxShortEventPromotionalDes_t *)pstDes);
}

static SIxError_e silib_desc_copyShortEventPromotionalDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyShortEventPromotionalDes((SIxShortEventPromotionalDes_t *)pstDest, (SIxShortEventPromotionalDes_t *)pstSource);
}

static void silib_desc_printShortEventPromotionalDes(const SIxDescriptor_t *pstDes)
{
	//(SIxShortEventPromotionalDes_t *)pstDes;
}


SIxDescriptor_t *silib_desc_alloc_Fsat(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
	case eSIxDESCTAG_FSAT_TUNNELLED_DATA:					return silib_desc_allocTunnelledDataDes();				break;
	case eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA:				return silib_desc_allocAltTunnelledDataDes();			break;
	case eSIxDESCTAG_FSAT_LINKAGE:							return silib_desc_allocFSatLinkageDes();				break;
	case eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL:	return silib_desc_allocRegionalisedLcnDes();			break;
	case eSIxDESCTAG_FSAT_REGION_NAME:						return silib_desc_allocRegionNameDes();					break;
	case eSIxDESCTAG_FSAT_SERVICE_GROUP: 					return silib_desc_allocSvcGroupDes();					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE:				return silib_desc_allocInteractiveStorageDes();			break;
	case eSIxDESCTAG_FSAT_INFO_LOCATION: 					return silib_desc_allocInfoLocationDes();				break;
	case eSIxDESCTAG_FSAT_SERVICEGROUP_NAME: 				return silib_desc_allocSvcGroupNameDes();				break;
	case eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME:				return silib_desc_allocShortSvcNameDes();				break;
	case eSIxDESCTAG_FSAT_GUIDANCE:						return silib_desc_allocGuidanceDes();					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION:			return silib_desc_allocInteractiveRestrictionDes();		break;
	case eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT:				return silib_desc_allocContentManagementDes();			break;
	case eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY:				return silib_desc_allocOnlineAvailabilityDes();			break;
	case eSIxDESCTAG_FSAT_PREFIX:							return silib_desc_allocPrefixDes();						break;
	case eSIxDESCTAG_FSAT_MEDIA_LOCATOR: 					return silib_desc_allocMediaLocatorDes();				break;
	case eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL:			return silib_desc_allocShortEventPromotionalDes();		break;
	default:											break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Fsat(SIxDescriptor_t *pstDes)
{
	SIxError_e		 eErr;
	SIxDescriptorTag_e	 eTag;

	if (NULL == pstDes)		return eSIxERR_SOURCE_NULL;

	eTag = pstDes->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_FSAT_TUNNELLED_DATA:					eErr = silib_desc_freeTunnelledDataDes(pstDes);				break;
	case eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA:				eErr = silib_desc_freeAltTunnelledDataDes(pstDes);			break;
	case eSIxDESCTAG_FSAT_LINKAGE:							eErr = silib_desc_freeFSatLinkageDes(pstDes);				break;
	case eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL:	eErr = silib_desc_freeRegionalisedLcnDes(pstDes);			break;
	case eSIxDESCTAG_FSAT_REGION_NAME:						eErr = silib_desc_freeRegionNameDes(pstDes);				break;
	case eSIxDESCTAG_FSAT_SERVICE_GROUP: 					eErr = silib_desc_freeSvcGroupDes(pstDes);					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE:				eErr = silib_desc_freeInteractiveStorageDes(pstDes);		break;
	case eSIxDESCTAG_FSAT_INFO_LOCATION: 					eErr = silib_desc_freeInfoLocationDes(pstDes);				break;
	case eSIxDESCTAG_FSAT_SERVICEGROUP_NAME: 				eErr = silib_desc_freeSvcGroupNameDes(pstDes);				break;
	case eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME:				eErr = silib_desc_freeShortSvcNameDes(pstDes);				break;
	case eSIxDESCTAG_FSAT_GUIDANCE:						eErr = silib_desc_freeGuidanceDes(pstDes);					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION:			eErr = silib_desc_freeInteractiveRestrictionDes(pstDes);	break;
	case eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT:				eErr = silib_desc_freeContentManagementDes(pstDes);			break;
	case eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY:				eErr = silib_desc_freeOnlineAvailabilityDes(pstDes);		break;
	case eSIxDESCTAG_FSAT_PREFIX:							eErr = silib_desc_freePrefixDes(pstDes);					break;
	case eSIxDESCTAG_FSAT_MEDIA_LOCATOR: 					eErr = silib_desc_freeMediaLocatorDes(pstDes);				break;
	case eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL:			eErr = silib_desc_freeShortEventPromotionalDes(pstDes);		break;
	default:											break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_Fsat(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr;

	if (NULL == pucRawData)		return eSIxERR_SOURCE_NULL;
	if (NULL == pstDes)			return eSIxERR_TARGET_NULL;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_FSAT_TUNNELLED_DATA:					eErr = silib_desc_parseTunnelledDataDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA:				eErr = silib_desc_parseAltTunnelledDataDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_FSAT_LINKAGE:							eErr = silib_desc_parseFSatLinkageDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL:	eErr = silib_desc_parseRegionalisedLcnDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_FSAT_REGION_NAME:						eErr = silib_desc_parseRegionNameDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_FSAT_SERVICE_GROUP: 					eErr = silib_desc_parseSvcGroupDes(pucRawData, pstDes);					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE:				eErr = silib_desc_parseInteractiveStorageDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_FSAT_INFO_LOCATION: 					eErr = silib_desc_parseInfoLocationDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_FSAT_SERVICEGROUP_NAME: 				eErr = silib_desc_parseSvcGroupNameDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME:				eErr = silib_desc_parseShortSvcNameDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_FSAT_GUIDANCE:						eErr = silib_desc_parseGuidanceDes(pucRawData, pstDes);					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION:			eErr = silib_desc_parseInteractiveRestrictionDes(pucRawData, pstDes);	break;
	case eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT:				eErr = silib_desc_parseContentManagementDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY:				eErr = silib_desc_parseOnlineAvailabilityDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_FSAT_PREFIX:							eErr = silib_desc_parsePrefixDes(pucRawData, pstDes);					break;
	case eSIxDESCTAG_FSAT_MEDIA_LOCATOR: 					eErr = silib_desc_parseMediaLocatorDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL:			eErr = silib_desc_parseShortEventPromotionalDes(pucRawData, pstDes);	break;
	default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Fsat(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e		 eErr;
	SIxDescriptorTag_e	 eTag;

	if (NULL == pstSource)		return eSIxERR_SOURCE_NULL;
	if (NULL == pstDest)		return eSIxERR_TARGET_NULL;

	eTag = pstSource->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_FSAT_TUNNELLED_DATA:					eErr = silib_desc_copyTunnelledDataDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA:				eErr = silib_desc_copyAltTunnelledDataDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_FSAT_LINKAGE:							eErr = silib_desc_copyFSatLinkageDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL:	eErr = silib_desc_copyRegionalisedLcnDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_FSAT_REGION_NAME:						eErr = silib_desc_copyRegionNameDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_FSAT_SERVICE_GROUP: 					eErr = silib_desc_copySvcGroupDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE:				eErr = silib_desc_copyInteractiveStorageDes(pstDest, pstSource);		break;
	case eSIxDESCTAG_FSAT_INFO_LOCATION: 					eErr = silib_desc_copyInfoLocationDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_FSAT_SERVICEGROUP_NAME: 				eErr = silib_desc_copySvcGroupNameDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME:				eErr = silib_desc_copyShortSvcNameDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_FSAT_GUIDANCE:						eErr = silib_desc_copyGuidanceDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION:			eErr = silib_desc_copyInteractiveRestrictionDes(pstDest, pstSource);	break;
	case eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT:				eErr = silib_desc_copyContentManagementDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY:				eErr = silib_desc_copyOnlineAvailabilityDes(pstDest, pstSource);		break;
	case eSIxDESCTAG_FSAT_PREFIX:							eErr = silib_desc_copyPrefixDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_FSAT_MEDIA_LOCATOR: 					eErr = silib_desc_copyMediaLocatorDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL:			eErr = silib_desc_copyShortEventPromotionalDes(pstDest, pstSource);		break;
	default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Fsat(const SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	if (NULL == pstDes)		return eSIxERR_SOURCE_NULL;

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Fsat(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
	case eSIxDESCTAG_FSAT_TUNNELLED_DATA:					silib_desc_printTunnelledDataDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA:				silib_desc_printAltTunnelledDataDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_LINKAGE:							silib_desc_printFSatLinkageDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL:	silib_desc_printRegionalisedLcnDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_REGION_NAME:						silib_desc_printRegionNameDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_SERVICE_GROUP: 					silib_desc_printSvcGroupDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE:				silib_desc_printInteractiveStorageDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_INFO_LOCATION: 					silib_desc_printInfoLocationDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_SERVICEGROUP_NAME: 				silib_desc_printSvcGroupNameDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME:				silib_desc_printShortSvcNameDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_GUIDANCE:						silib_desc_printGuidanceDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION:			silib_desc_printInteractiveRestrictionDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT:				silib_desc_printContentManagementDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY:				silib_desc_printOnlineAvailabilityDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_PREFIX:							silib_desc_printPrefixDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_MEDIA_LOCATOR: 					silib_desc_printMediaLocatorDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL:			silib_desc_printShortEventPromotionalDes(pstDes);	eErr = eSIxERR_OK;	break;
	default:											break;
	}

	return eErr;
}


