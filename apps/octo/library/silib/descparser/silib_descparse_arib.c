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
	@file     sxdescparse_arib.c
	@brief    sxdescparse_arib.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define goto_if(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	ARIB_DES_EOS				'\0'

static HCHAR *silib_desc_getTagString_Arib(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
	ENUM_TO_STR(eSIxDESCTAG_ARIB_BROADCASTER_ID);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_DTCP);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_AUDIO_COMPONENT);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_HYPER_LINK);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_TARGET_AREA);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_DATA_CONTENTS);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_CA_EMM_TS);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_CA_SERVICE);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_TS_INFORMATION);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_LOGO_TRANSMISSION);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_PARTIAL_RECEPTION);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_DATA_COMPONENT);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_SERIES);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_EVENT_GROUP);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_SI_PARAMETER);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_BROADCASTER_NAME);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_COMPONENT_GROUP);
	ENUM_TO_STR(eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY);
	default:
		break;
	}

	return NULL;
}

#if 0
/************************************************************************
 * eSIxDESCTAG_XXX					(Tag = 0xXX)
 ************************************************************************/
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



//XXX
#if 0
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxXXXDes_t));
	pstDes->pstNext = NULL;
}

static void initXXXInfo(SIxXXXInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxXXXInfo_t));
}

static SIxXXXDes_t *allocXXXDes(void)
{
	return (SIxXXXDes_t *)silib_desc_alloc(sizeof(SIxXXXDes_t));
}

static SIxXXXInfo_t *allocXXXInfo(void)
{
	SIxXXXInfo_t	*pstInfo;

	pstInfo = (SIxXXXInfo_t *)silib_desc_alloc(sizeof(SIxXXXInfo_t));
	if (pstInfo)
	{
		initXXXInfo(pstInfo);
	}

	return pstInfo;
}

static void freeXXXInfo(SIxXXXInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	// Free Pointer(s) if exists
	// silib_desc_free(pstInfo->pstYYY)
	// silib_desc_free(pstInfo->pszZZZ)

	silib_desc_free(pstInfo);
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{
err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;
	SIxXXXDes_t		*pstDesList = NULL;

	if (pstSource && pstDest)
	{
		SIxXXXDes_t	*pstSourceDes, *pstNewDes = NULL;

		pstDesList = pstDest;
		pstSourceDes = pstSource;
		while (pstSourceDes)
		{
			pstNewDes = (SIxXXXDes_t *)silib_desc_alloc(sizeof(SIxXXXDes_t));
			if (pstNewDes == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewDes, pstSourceDes, sizeof(SIxXXXDes_t));
			pstNewDes->pstNext = NULL;

			pstDesList->pstNext = pstNewDes;
			pstDesList = pstDesList->pstNext;

			pstSourceDes = pstSourceDes->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDest->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	return eSIxERR_FAIL;
}
#endif

//BroadcasterId
static void initBroadcasterIdDes(SIxBroadcasterIdDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxBroadcasterIdDes_t));
}

static SIxBroadcasterIdDes_t *allocBroadcasterIdDes(void)
{
	return (SIxBroadcasterIdDes_t *)silib_desc_alloc(sizeof(SIxBroadcasterIdDes_t));
}

static void freeBroadcasterIdDes(SIxBroadcasterIdDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseBroadcasterIdDes(HUINT8 *p, SIxBroadcasterIdDes_t *pstDes)
{
	HUINT8		 ucDesLen;

	ucDesLen = p[1];

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->usOriginalNetworkId		= HxMACRO_Get16Bit(p + 2);
	pstDes->usTransportStreamId		= HxMACRO_Get16Bit(p + 4);
	pstDes->nEventId				= HxMACRO_Get16Bit(p + 6);
	pstDes->ucBroadcasterId			= p[8];

	return eSIxERR_OK;
}

static SIxError_e copyBroadcasterIdDes(SIxBroadcasterIdDes_t *pstDest, const SIxBroadcasterIdDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxBroadcasterIdDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

//DvhsDTCP
static void initDvhsDTCPDes(SIxDvhsDTCPDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxDvhsDTCPDes_t));
}

static SIxDvhsDTCPDes_t *allocDvhsDTCPDes(void)
{
	return (SIxDvhsDTCPDes_t *)silib_desc_alloc(sizeof(SIxDvhsDTCPDes_t));
}

static void freeDvhsDTCPDes(SIxDvhsDTCPDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseDvhsDTCPDes(HUINT8 *p, SIxDvhsDTCPDes_t *pstDes)
{
	HUINT8		ucDesLen;

	ucDesLen = p[1];

	/* Revision 1.6
		Private_data_byte{
		   Reserved 				1	 bslbf
		   Retention_Move_mode		1	 bslbf
		   Retention_State			3	 bslbf
		   EPN						1	 bslbf
		   DTCP_CCI 				2	 bslbf

		   Reserved 				4	 bslbf
		   AST						1	 bslbf
		   Image_Constraint_Token	1	 bslbf
		   APS						2	 bslbf
		}
	*/
	pstDes->ucRetentionMoveMode 	= (HUINT8)((p[4] & 0x40) >> 6);
	pstDes->ucRetentionState		= (HUINT8)((p[4] & 0x38) >> 3);
	pstDes->ucEPN					= (HUINT8)((p[4] & 0x04) >> 2);
	pstDes->ucDTCPCCI				= (HUINT8)(p[4] & 0x03);

	pstDes->ucAST					= (HUINT8)((p[5] & 0x08) >> 3);
	pstDes->ucImageConstraintToken	= (HUINT8)((p[5] & 0x04) >> 2);
	pstDes->ucAPS					= (HUINT8)(p[5] & 0x03);

	return eSIxERR_OK;
}

static SIxError_e copyDvhsDTCPDes(SIxDvhsDTCPDes_t *pstDest, const SIxDvhsDTCPDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDvhsDTCPDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

//HierarchicalTransmission
static void initHierarchicalTransmissionDes(SIxHierarchicalTransmissionDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxHierarchicalTransmissionDes_t));
}

static SIxHierarchicalTransmissionDes_t *allocHierarchicalTransmissionDes(void)
{
	return (SIxHierarchicalTransmissionDes_t *)silib_desc_alloc(sizeof(SIxHierarchicalTransmissionDes_t));
}

static void freeHierarchicalTransmissionDes(SIxHierarchicalTransmissionDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseHierarchicalTransmissionDes(HUINT8 *p, SIxHierarchicalTransmissionDes_t *pstDes)
{
	HUINT8		 ucDesLen;

	p++;				// Tag
	ucDesLen	= *p++;

	pstDes->bQualityLevel		= (*p & 0x01); p++;
	pstDes->usReferencePID		= HxMACRO_Get13Bit(p);
//	pstDes->usReferencePID		= ((p[3]&0x1F)<<8)|p[4];

	return eSIxERR_OK;
}

static SIxError_e copyHierarchicalTransmissionDes(SIxHierarchicalTransmissionDes_t *pstDest, const SIxHierarchicalTransmissionDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxHierarchicalTransmissionDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

//DigitalCopyControl
static void initDigitalCopyControlDes(SIxDigitalCopyControlDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxDigitalCopyControlDes_t));
}

static SIxDigitalCopyControlDes_t *allocDigitalCopyControlDes(void)
{
	return (SIxDigitalCopyControlDes_t *)silib_desc_alloc(sizeof(SIxDigitalCopyControlDes_t));
}

static void freeDigitalCopyControlComponent(SIxDigitalCopyControlDesComponent_t *pstComponent)
{
	silib_desc_free(pstComponent);
}

static void freeDigitalCopyControlDes(SIxDigitalCopyControlDes_t *pstDes)
{
	if (pstDes)
	{
		freeDigitalCopyControlComponent(pstDes->pstComponent);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseDigitalCopyControlDes(HUINT8 *p, SIxDigitalCopyControlDes_t *pstDes)
{
	HUINT8				 				ucDesLen;
	SIxDigitalCopyControlDesComponent_t *pstComponent = NULL;
	HUINT8					*pucTemp;
	HINT8					ncComponentControlLen;
	HUINT8					ucNum;
	HUINT8					ucMaximumBitRateFlag;
	HUINT8					ucCopyControlType;

	ucDesLen = p[1];
	p += 2;

	if((ucDesLen == 0))
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucDigitalRecordingControlData = (*p & 0xC0) >> 6;
	pstDes->ucMaximumBitRateFlag = (*p & 0x20) >> 5;
	pstDes->ucComponentControlFlag = (*p & 0x10) >> 4;
	pstDes->ucCopyControlType = (*p & 0x0C) >> 2;		/* TR-B15/B14 volume 4 table 30-20 (not STD-B10) */
	pstDes->ucAPSControlData = (*p & 0x03);		/* TR-B15/B14 volume 4 table 30-20 (not STD-B10) */
	p += 1;

	if(pstDes->ucMaximumBitRateFlag == 1)
	{
		pstDes->ucMaximumBitRate = *p++;
	}

	if(pstDes->ucComponentControlFlag == 1)
	{
		pucTemp = p;
		ncComponentControlLen = *pucTemp++;
		ucNum = 0;

		while(ncComponentControlLen > 0)
		{
			if(ncComponentControlLen < 2)
			{
				HxLOG_Error("Componet control length(%d) is invalid. (%d)\n", ncComponentControlLen, __LINE__);
			}
			pucTemp++;	/* component_tag */
			ucMaximumBitRateFlag = (*pucTemp & 0x20) >> 5;
			ucCopyControlType = (*pucTemp & 0x0C) >> 2;
			pucTemp++;
			ncComponentControlLen -= 2;
			if(ucMaximumBitRateFlag == 1)
			{
				pucTemp++;
				ncComponentControlLen--;
			}
			ucNum++;
		}

		pstDes->ucNumComponents = ucNum;
		if(ucNum > 0)
		{
			pstDes->pstComponent = (SIxDigitalCopyControlDesComponent_t *)silib_desc_alloc(sizeof(SIxDigitalCopyControlDesComponent_t) * ucNum);
			if(pstDes->pstComponent != NULL)
			{
				HxSTD_MemSet(pstDes->pstComponent, 0, (sizeof(SIxDigitalCopyControlDesComponent_t) * ucNum));
			}
			else
			{
				return eSIxERR_INVALID;
			}
		}

		ncComponentControlLen = *p++;
		ucNum = 0;

		while(ncComponentControlLen > 0)
		{
			pstComponent = &pstDes->pstComponent[ucNum];
			if(ncComponentControlLen < 2)
			{
				HxLOG_Error("Componet control length(%d) is invalid. (%d)\n", ncComponentControlLen, __LINE__);
				break;
			}

			pstComponent->ucComponentTag = *p++;
			pstComponent->ucDigitalRecordingControlData = (*p & 0xC0) >> 6;
			pstComponent->ucMaximumBitRateFlag = (*p & 0x20) >> 5;
			pstComponent->ucCopyControlType = (*p & 0x0C) >> 2;
			pstComponent->ucAPSControlData = ((*p++) & 0x03);
			ncComponentControlLen -= 2;
			if(pstComponent->ucMaximumBitRateFlag == 1)
			{
				pstComponent->ucMaximumBitRate = *p++;
				ncComponentControlLen--;
			}
			ucNum++;
		}
	}

	return eSIxERR_OK;
}

static SIxError_e copyDigitalCopyControlDes(SIxDigitalCopyControlDes_t *pstDest, const SIxDigitalCopyControlDes_t *pstSource)
{
	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDigitalCopyControlDes_t));
		pstDest->pstComponent = NULL;
		pstDest->ucNumComponents = 0;

		if (0 < pstSource->ucNumComponents && NULL != pstSource->pstComponent)
		{
			HUINT32 ulSize = sizeof(SIxDigitalCopyControlDesComponent_t)*pstSource->ucNumComponents;

			pstDest->pstComponent = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pstComponent)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pstComponent, pstSource->pstComponent, ulSize);
			pstDest->ucNumComponents = pstSource->ucNumComponents;
		}
	}

	return eSIxERR_OK;

err_exit:
	if (pstDest)
	{
		freeDigitalCopyControlComponent(pstDest->pstComponent);
	}
	return eSIxERR_FAIL;
}


//NetworkIdentification
static void initNetworkIdentificationDes(SIxNetworkIdentificationDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxNetworkIdentificationDes_t));
}

static SIxNetworkIdentificationDes_t *allocNetworkIdentificationDes(void)
{
	return (SIxNetworkIdentificationDes_t *)silib_desc_alloc(sizeof(SIxNetworkIdentificationDes_t));
}

static void freeNetworkIdentificationDes(SIxNetworkIdentificationDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_util_freePrivateDataInfo(pstDes->pstPriavetDataInfo);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseNetworkIdentificationDes(HUINT8 *p, SIxNetworkIdentificationDes_t *pstDes)
{
	HUINT8	ucDesLen;

	//p[0];				// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ulCountryCode	= HxMACRO_Get24Bit(p + 2);
	pstDes->usMediaType		= HxMACRO_Get16Bit(p + 5);
	pstDes->usNetworkId		= HxMACRO_Get16Bit(p + 7);

	p += 14;
	ucDesLen -= 14;

	if (ucDesLen > 0)
	{
		SIxError_e	eErr;

		pstDes->pstPriavetDataInfo = silib_desc_util_allocPrivateDataInfo(ucDesLen);
		if (NULL == pstDes->pstPriavetDataInfo)
			return eSIxERR_FAIL;

		eErr = silib_desc_util_setPrivateDataInfo(pstDes->pstPriavetDataInfo, p, ucDesLen);
		if (eSIxERR_OK != eErr)
		{
			silib_desc_util_freePrivateDataInfo(pstDes->pstPriavetDataInfo);
			return eSIxERR_FAIL;
		}
	}

	return eSIxERR_OK;
}

static SIxError_e copyNetworkIdentificationDes(SIxNetworkIdentificationDes_t *pstDest, const SIxNetworkIdentificationDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNetworkIdentificationDes_t));
		eErr = eSIxERR_OK;

		if (pstSource->pstPriavetDataInfo)
		{
			pstDest->pstPriavetDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPriavetDataInfo);
			if (NULL == pstDest->pstPriavetDataInfo)
				eErr = eSIxERR_FAIL;
		}
	}

	return eErr;
}

//PartialTsTime
static void initPartialTsTimeDes(SIxPartialTsTimeDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxPartialTsTimeDes_t));
}

static SIxPartialTsTimeDes_t *allocPartialTsTimeDes(void)
{
	return (SIxPartialTsTimeDes_t *)silib_desc_alloc(sizeof(SIxPartialTsTimeDes_t));
}

static void freePartialTsTimeDes(SIxPartialTsTimeDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parsePartialTsTimeDes(HUINT8 *p, SIxPartialTsTimeDes_t *pstDes)
{
	HUINT8		 ucDesLen;
	UNIXTIME	ulTime;
	SIxError_e		ulRet;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucEventVersionNumber = p[2];
	ulRet = HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime((p + 3), &ulTime);
	if(ulRet == ERR_OK)
	{
		if( ulTime > 0 )
		{
			/*  EIT의 event_start_time이 Japan 은 시간이 Local Time (GMT +9.00) 으로 와서 GMT +0 으로 변경함으로 이것도 변경 */
			HINT32 	nHour = 0, nMin = 0;
			ulRet = HLIB_DATETIME_GetTimeOffsetByCountryIndex(eCountryID_JPN, &nHour, &nMin);
			if (ulRet == ERR_OK)
			{
				ulTime = ulTime - ((nHour * SECONDS_PER_HOUR) + (nMin * SECONDS_PER_MIN));
			}
		}
		pstDes->ulStartTime = ulTime;
	}
	else
	{
		pstDes->ulStartTime = 0xFFFFFFFF;
	}

	ulRet = HLIB_DATETIME_ConvertBcdTimeToUnixTime((p + 8), &ulTime);
	pstDes->ulDuration = (ulRet == ERR_OK) ? ulTime : 0xFFFFFFFF;

	ulRet = HLIB_DATETIME_ConvertBcdTimeToUnixTime((p + 11), &ulTime);
	pstDes->ulOffset = (ulRet == ERR_OK) ? ulTime : 0;

	pstDes->bOffsetFlag = (p[14] & 0x04)?1:0;
	pstDes->bOherDescriptorStatus = (p[14] & 0x02)?1:0;
	pstDes->bJSTTimeFlag = (p[14] & 0x01)?1:0;
	if(pstDes->bJSTTimeFlag == 1)
	{
		ulRet = HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime((p + 15), &ulTime);
		if(ulRet == ERR_OK)
		{
			pstDes->ulJSTTime = ulTime;
		}
		else
		{
			HxLOG_Error("HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime() return error. %d\n", __LINE__);
			return eSIxERR_INVALID;
		}
	}

	return eSIxERR_OK;
}

static SIxError_e copyPartialTsTimeDes(SIxPartialTsTimeDes_t *pstDest, const SIxPartialTsTimeDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxPartialTsTimeDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

//AudioComponent
static void initAudioComponentDes(SIxAudioComponentDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxAudioComponentDes_t));
}

static SIxAudioComponentDes_t *allocAudioComponentDes(void)
{
	return (SIxAudioComponentDes_t *)silib_desc_alloc(sizeof(SIxAudioComponentDes_t));
}

static void freeAudioComponentDes(SIxAudioComponentDes_t *pstDes)
{
	SIxAudioComponentDes_t	*pstTemp;

	if (NULL == pstDes)		return;

	while(pstDes != NULL)
	{
		silib_desc_free(pstDes->pucTextChar);
		pstDes->pucTextChar = NULL;

		pstTemp = pstDes->pstNext;
		silib_desc_free(pstDes);
		pstDes = pstTemp;
	}
}

static SIxError_e parseAudioComponentDes(HUINT8 *p, SIxAudioComponentDes_t *pstDes)
{
	HUINT8				 ucDesLen, ucTempLen;
	HUINT8				szIso639LangCode[4], *pszText = NULL;
	HUINT32				nTempTextLen;

	//	p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucStreamContent = p[2] & 0x0F;
	pstDes->ucComponentType = p[3];
	pstDes->ucComponentTag = p[4];
	pstDes->ucStreamType = p[5];
	pstDes->ucSimulcastGroupTag = p[6];
	pstDes->bESMultiLlingualFlag = ((p[7] & 0x80) >> 7) ? TRUE : FALSE;
	pstDes->bMainComponentFlag = ((p[7] & 0x40) >> 6) ? TRUE : FALSE;
	pstDes->ucQualityIndicator = (p[7] & 0x30) >> 4;
	pstDes->ucSamplingRate = (p[7] & 0x0E) >> 1;
	pstDes->ulISO639LanguageCode = HxMACRO_Get24Bit(p + 8);
	HxSTD_MemCopy(szIso639LangCode, (p + 8), 3);
	szIso639LangCode [3] = '\0';
	if(pstDes->bESMultiLlingualFlag == 1)
	{
		pstDes->ulISO639LanguageCode2 = HxMACRO_Get24Bit(p + 11);
		pstDes->ucTextCharLen = ucTempLen = ucDesLen - 12;
		p += 14;
	}
	else
	{
		pstDes->ulISO639LanguageCode2 = -1;
		pstDes->ucTextCharLen = ucTempLen = ucDesLen - 9;
		p += 11;
	}

	nTempTextLen = silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc, pstDes->ucTextCharLen, (const HUINT8 *)p, &pszText);
	if(pszText != NULL)
	{
		pstDes->pucTextChar = silib_desc_alloc(sizeof(HUINT8) * (nTempTextLen + 1));
		if (pstDes->pucTextChar)
		{
			HxSTD_MemSet(pstDes->pucTextChar, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
			HxSTD_MemCopy(pstDes->pucTextChar, pszText, silib_desc_util_dvbStrlen(pszText));
			pstDes->pucTextChar[silib_desc_util_dvbStrlen(pszText)] = ARIB_DES_EOS;
		}
		silib_desc_free(pszText);
	}

	return eSIxERR_OK;
}

static SIxError_e copyAudioComponentInfo(SIxAudioComponentDes_t *pstDest, const SIxAudioComponentDes_t *pstSource)
{
	SIxError_e	eErr = eSIxERR_FAIL;
	HUINT32 	ulSize;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAudioComponentDes_t));
		pstDest->pucTextChar = NULL;
		pstDest->pstNext = NULL;

		if (NULL != pstSource->pucTextChar)
		{
			ulSize = silib_desc_util_dvbStrlen(pstSource->pucTextChar);
			pstDest->pucTextChar = silib_desc_alloc(ulSize+1);
			if (NULL == pstDest->pucTextChar)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucTextChar, pstSource->pucTextChar, ulSize);
			pstDest->pucTextChar[ulSize] = '\0';
			pstDest->ucTextCharLen = (HUINT8)ulSize;
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pucTextChar);
		pstDest->pucTextChar = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyAudioComponentDes(SIxAudioComponentDes_t *pstDest, const SIxAudioComponentDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;
	SIxAudioComponentDes_t	*pstDesList = NULL;

	if (pstSource && pstDest)
	{
		SIxAudioComponentDes_t	*pstSourceDes, *pstNewDes = NULL;

		if (copyAudioComponentInfo(pstDest, pstSource) != eSIxERR_OK)
			goto err_exit;

		pstDesList = pstDest;
		pstSourceDes = pstSource->pstNext;
		while (pstSourceDes)
		{
			pstNewDes = allocAudioComponentDes();
			if (pstNewDes == NULL)
				goto err_exit;

			if (copyAudioComponentInfo(pstNewDes, pstSourceDes) != eSIxERR_OK)
			{
				silib_desc_free(pstNewDes);
				goto err_exit;
			}

			pstDesList->pstNext = pstNewDes;
			pstDesList = pstDesList->pstNext;

			pstSourceDes = pstSourceDes->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pucTextChar);
		freeAudioComponentDes(pstDest->pstNext);
	}
	return eSIxERR_FAIL;
}


//DataContent
static void initDataContentDes(SIxDataContentDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxDataContentDes_t));
}

static SIxDataContentDes_t *allocDataContentDes(void)
{
	SIxDataContentDes_t	*pstDes;

	pstDes = (SIxDataContentDes_t *)silib_desc_alloc(sizeof(SIxDataContentDes_t));
	if (pstDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocDataContentDes()\n");
		return NULL;
	}

	return pstDes;
}

static void freeDataContentBxmlInfo(SIxAribBxmlInfo_t *pstInfo)
{
	if (pstInfo)
	{
		if (pstInfo->pstCarouselInfo)
		{
			silib_desc_free(pstInfo->pstCarouselInfo->pstCarouselComponent);
			silib_desc_free(pstInfo->pstCarouselInfo);
		}
		if (pstInfo->pstStoredCarouselInfo)
		{
			silib_desc_free(pstInfo->pstStoredCarouselInfo->pstStoredCarouselComponent);
			silib_desc_free(pstInfo->pstStoredCarouselInfo);
		}
		silib_desc_free(pstInfo);
	}
}

static void freeDataContentCaptionInfo(SIxAribCaptionInfo_t *pstInfo)
{
	if (pstInfo)
	{
		if (pstInfo->pucLanguageTag)
		{
			silib_desc_free(pstInfo->pucLanguageTag);
		}
		if (pstInfo->pucDMF)
		{
			silib_desc_free(pstInfo->pucDMF);
		}
		if (pstInfo->pulISO639LanguageCode)
		{
			silib_desc_free(pstInfo->pulISO639LanguageCode);
		}
		silib_desc_free(pstInfo);
	}
}

static void freeDataContentInfo(SIxDataContentDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pucComponentRef);
		pstDes->pucComponentRef = NULL;

		silib_desc_free(pstDes->pucTextChar);
		pstDes->pucTextChar = NULL;

		freeDataContentBxmlInfo(pstDes->pstAribBxmlInfo);
		pstDes->pstAribBxmlInfo = NULL;

		freeDataContentCaptionInfo(pstDes->pstAribCaptionInfo);
		pstDes->pstAribCaptionInfo = NULL;
	}
}

static void freeDataContentDes(SIxDataContentDes_t *pstDes)
{
	if (pstDes)
	{
		freeDataContentInfo(pstDes);
		silib_desc_free(pstDes);
	}
}

static void freeDataContentDesList(SIxDataContentDes_t *pstDesHead)
{
	SIxDataContentDes_t		*pstDesList, *pstDesToFree = NULL;

	pstDesList = pstDesHead;
	while (pstDesList)
	{
		pstDesToFree = pstDesList;
		pstDesList = pstDesList->pstNext;

		freeDataContentDes(pstDesToFree);
	}
}

static SIxError_e parseDataContentDes(HUINT8 *p, SIxDataContentDes_t *pstDes)
{
	HUINT8						 	ucDesLen;
	HUINT32							i;
	HUINT8							*pucTemp = NULL;
	SIxAribBxmlInfo_t				*pstBxmlInfo = NULL;
	SIxAribCarouselInfo_t			*pstCarouselInfo = NULL;
	SIxAribCarouselComponent_t		*pstCarouselComponent = NULL;
	SIxAribStoredCarouselInfo_t		*pstStoredCarouselInfo = NULL;
	SIxAribStoredCarouselComponent_t *pstStoredCarouselComponent = NULL;
	SIxAribCaptionInfo_t				*pstCaptionInfo = NULL;
	HUINT8							 szIso639LangCode[4], *pszText = NULL;
	HUINT32							 nTempTextLen;
	HUINT32							 ulSize;

	// pucRaw[0];		// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->usDataComponentId = HxMACRO_Get16Bit(p + 2);
	pstDes->ucEntryComponent = p[4];
	pstDes->ucSelectorLen = p[5];
	p += 6;

	pstDes->pstAribBxmlInfo = NULL;
	pucTemp = p;
	if((pstDes->usDataComponentId == 0x0007) || (pstDes->usDataComponentId == 0x000C))
	{
		pstDes->pstAribBxmlInfo = (SIxAribBxmlInfo_t *)silib_desc_alloc(sizeof(SIxAribBxmlInfo_t));
		if (NULL == pstDes->pstAribBxmlInfo)
			goto	err_exit;
		HxSTD_MemSet(pstDes->pstAribBxmlInfo, 0, sizeof(SIxAribBxmlInfo_t));
		pstBxmlInfo = pstDes->pstAribBxmlInfo;

		pstBxmlInfo->ucTransmissionFormat		= (*pucTemp >> 6) & 0x03;		/* 2 bits */
		pstBxmlInfo->ucAutoStartFlag 			= (*pucTemp >> 4) & 0x01;		/* 1 bit */
		pstBxmlInfo->ucDocumentResolution 		= (*pucTemp++) & 0x0F;
		pstBxmlInfo->ucUseXml 					= (*pucTemp >> 7) & 0x01;
		pstBxmlInfo->ucDefaultVersionFlag 		= (*pucTemp >> 6) & 0x01;
		pstBxmlInfo->ucIndependentFlag 			= (*pucTemp >> 5) & 0x01;
		pstBxmlInfo->ucContentIdFlag 			= (*pucTemp >> 4) & 0x01;
		pstBxmlInfo->ucAssociatedContentsFlag 	= (*pucTemp >> 3) & 0x01;
		pstBxmlInfo->ucStyleForTvFlag 			= (*pucTemp >> 1) & 0x01;
		pstBxmlInfo->ucUpdateFlag 				= (*pucTemp++) & 0x01;
		pstBxmlInfo->ulISO639LanguageCode	 	= HxMACRO_Get24Bit(pucTemp);
		pucTemp	+= 3;

		if ( pstBxmlInfo->ucContentIdFlag == 1)
		{
			pstBxmlInfo->ulContentId				= HxMACRO_Get32Bit(pucTemp);
			pucTemp		+= 4;
			pstBxmlInfo->usContentVersion			= HxMACRO_Get16Bit(pucTemp);
			pucTemp		+= 2;
		}

		if ( pstBxmlInfo->ucDefaultVersionFlag == 0)
		{
			pstBxmlInfo->usBmlMajorVersion			= HxMACRO_Get16Bit(pucTemp);
			pucTemp		+= 2;
			pstBxmlInfo->usBmlMinorVersion			= HxMACRO_Get16Bit(pucTemp);
			pucTemp		+= 2;
			if ( pstBxmlInfo->ucUseXml == 1)
			{
				pstBxmlInfo->usBxmlMajorVersion		= HxMACRO_Get16Bit(pucTemp);
				pucTemp		+= 2;
				pstBxmlInfo->usBxmlMinorVersion		= HxMACRO_Get16Bit(pucTemp);
				pucTemp		+= 2;
			}
		}

		if(pstBxmlInfo->ucTransmissionFormat == 0)
		{
			ulSize = sizeof(SIxAribCarouselInfo_t);
			pstBxmlInfo->pstCarouselInfo = (SIxAribCarouselInfo_t *)silib_desc_alloc(ulSize);
			if (NULL == pstBxmlInfo->pstCarouselInfo)
				goto	err_exit;

			HxSTD_MemSet(pstBxmlInfo->pstCarouselInfo, 0, ulSize);
			pstCarouselInfo = pstBxmlInfo->pstCarouselInfo;
			pstCarouselInfo->ulNumOfCarousels = *pucTemp++;
			if(pstCarouselInfo->ulNumOfCarousels > 0)
			{
				ulSize = sizeof(SIxAribCarouselComponent_t) * pstCarouselInfo->ulNumOfCarousels;
				pstCarouselInfo->pstCarouselComponent = (SIxAribCarouselComponent_t *)silib_desc_alloc(ulSize);
				if (NULL == pstCarouselInfo->pstCarouselComponent)
					goto	err_exit;

				HxSTD_MemSet(pstCarouselInfo->pstCarouselComponent, 0, ulSize);

				for(i=0;i<pstCarouselInfo->ulNumOfCarousels;i++)
				{
					pstCarouselComponent = &pstCarouselInfo->pstCarouselComponent[i];
					pstCarouselComponent->ucComponentTag = pucTemp[0];
					pstCarouselComponent->ucEventSectionFlag = (pucTemp[1] >> 7) & 0x01;
					pstCarouselComponent->ucComponentSizeFlag = (pucTemp[1] >> 3) & 0x01;
					pstCarouselComponent->ucDefaultTransactionIdFlag = (pucTemp[1] >> 2) & 0x01;
					pstCarouselComponent->ucDefaultTimeoutDIIFlag = (pucTemp[1] >> 1) & 0x01;
					pstCarouselComponent->ucDefaultLeakRateFlag = pucTemp[1] & 0x01;
					pucTemp += 2;
					if(pstCarouselComponent->ucComponentSizeFlag == 1)
					{
						pstCarouselComponent->ulComponentSize = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstCarouselComponent->ucDefaultTransactionIdFlag == 1)
					{
						pstCarouselComponent->ulTimeoutValueDII = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstCarouselComponent->ucDefaultLeakRateFlag == 1)
					{
						pstCarouselComponent->ulLeakRate = (pucTemp[0] << 14) | (pucTemp[1] << 6) | ((pucTemp[2] >> 2) & 0x3F);
						pucTemp += 3;
					}
				}
			}
			pstBxmlInfo->ucOndemandRetrievalFlag = (pucTemp[0] >> 7) & 0x01;
			pstBxmlInfo->ucFileStorableFlag = (pucTemp[0] >> 6) & 0x01;
			pucTemp += 1;
		}
		else if(pstBxmlInfo->ucTransmissionFormat == 1)
		{
			ulSize = sizeof(SIxAribStoredCarouselInfo_t);
			pstBxmlInfo->pstStoredCarouselInfo = (SIxAribStoredCarouselInfo_t *)silib_desc_alloc(ulSize);
			if (NULL == pstBxmlInfo->pstStoredCarouselInfo)
				goto	err_exit;
			HxSTD_MemSet(pstBxmlInfo->pstStoredCarouselInfo, 0, ulSize);
			pstStoredCarouselInfo = pstBxmlInfo->pstStoredCarouselInfo;

			pstStoredCarouselInfo->ucNumOfCarousels = *pucTemp++;
			if(pstStoredCarouselInfo->ucNumOfCarousels > 0)
			{
				ulSize = sizeof(SIxAribStoredCarouselComponent_t) * pstStoredCarouselInfo->ucNumOfCarousels;
				pstStoredCarouselInfo->pstStoredCarouselComponent = (SIxAribStoredCarouselComponent_t *)silib_desc_alloc(ulSize);
				if (NULL == pstStoredCarouselInfo->pstStoredCarouselComponent)
					goto	err_exit;
				HxSTD_MemSet(pstStoredCarouselInfo->pstStoredCarouselComponent, 0, ulSize);

				for(i=0;i<pstStoredCarouselInfo->ucNumOfCarousels;i++)
				{
					pstStoredCarouselComponent = &pstStoredCarouselInfo->pstStoredCarouselComponent[i];
					pstStoredCarouselComponent->ucComponentTag = *pucTemp++;
					pstStoredCarouselComponent->ucNumDataEventFlag = (*pucTemp >> 7) & 0x01;
					pstStoredCarouselComponent->ucNumModulesFlag = (*pucTemp >> 6) & 0x01;
					pstStoredCarouselComponent->ucNumResourcesFlag = (*pucTemp >> 5) & 0x01;
					pstStoredCarouselComponent->ucCompressedComponentSizeFlag = (*pucTemp >> 4) & 0x01;
					pstStoredCarouselComponent->ucComponentSizeFlag = (*pucTemp >> 3) & 0x01;
					pstStoredCarouselComponent->ucDefaultTransactionIdFlag = (*pucTemp >> 2) & 0x01;
					pstStoredCarouselComponent->ucDefaultTimeoutDIIFlag = (*pucTemp >> 1) & 0x01;
					pstStoredCarouselComponent->ucDefaultLeakRateFlag = *pucTemp++ & 0x01;
					if(pstStoredCarouselComponent->ucNumDataEventFlag == 1)
					{
						pstStoredCarouselComponent->usNumDataEvent = HxMACRO_Get16Bit(pucTemp);
						pucTemp += 2;
					}
					if(pstStoredCarouselComponent->ucNumModulesFlag == 1)
					{
						pstStoredCarouselComponent->ulNumModules = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucNumResourcesFlag == 1)
					{
						pstStoredCarouselComponent->ulNumResoures = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucCompressedComponentSizeFlag == 1)
					{
 						pstStoredCarouselComponent->ulCompressedComponentSize = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucComponentSizeFlag == 1)
					{
						pstStoredCarouselComponent->ulComponentSize = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucDefaultTransactionIdFlag == 1)
					{
						pstStoredCarouselComponent->ulTransactionId = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucDefaultTimeoutDIIFlag == 1)
					{
						pstStoredCarouselComponent->ulTimeoutValueDII = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucDefaultLeakRateFlag == 1)
					{
						pstStoredCarouselComponent->ulLeakRate = (pucTemp[0] << 14) | (pucTemp[1] << 6) | ((pucTemp[2] >> 2) & 0x3F);
						pucTemp += 3;
					}
				}
			}
		}
	}
	else if(pstDes->usDataComponentId == 0x0008)
	{
		ulSize = sizeof(SIxAribCaptionInfo_t);
		pstDes->pstAribCaptionInfo = (SIxAribCaptionInfo_t *)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pstAribCaptionInfo)
			goto	err_exit;

		HxSTD_MemSet(pstDes->pstAribCaptionInfo, 0, ulSize);

		pstCaptionInfo = pstDes->pstAribCaptionInfo;

		pstCaptionInfo->ucNumLanguages = *pucTemp++;
		if(pstCaptionInfo->ucNumLanguages > 0)
		{
			ulSize = sizeof(HUINT8) * pstCaptionInfo->ucNumLanguages;
			pstCaptionInfo->pucLanguageTag = (HUINT8 *)silib_desc_alloc(ulSize);
			if (NULL == pstCaptionInfo->pucLanguageTag)
				goto	err_exit;

			HxSTD_MemSet(pstCaptionInfo->pucLanguageTag, 0, ulSize);

			pstCaptionInfo->pucDMF = (HUINT8 *)silib_desc_alloc(ulSize);
			if (NULL == pstCaptionInfo->pucDMF)
				goto	err_exit;

			HxSTD_MemSet(pstCaptionInfo->pucDMF, 0, ulSize);

			ulSize = sizeof(HUINT32) * pstCaptionInfo->ucNumLanguages;
			pstCaptionInfo->pulISO639LanguageCode = (HUINT32 *)silib_desc_alloc(ulSize);
			if (NULL == pstCaptionInfo->pulISO639LanguageCode)
				goto	err_exit;

			HxSTD_MemSet(pstCaptionInfo->pulISO639LanguageCode, 0, ulSize);

			for(i=0;i<pstCaptionInfo->ucNumLanguages;i++)
			{
				pstCaptionInfo->pucLanguageTag[i]		= (*pucTemp >> 5) & 0x07;
				pstCaptionInfo->pucDMF[i]				= (*pucTemp++) & 0x0F;
				pstCaptionInfo->pulISO639LanguageCode[i] =  HxMACRO_Get24Bit(pucTemp);
				pucTemp += 3;
			}
		}
	}

	p += pstDes->ucSelectorLen;
	pstDes->ucNumOfComponentRef = *p++;
	if(pstDes->ucNumOfComponentRef > 0)
	{
		ulSize = sizeof(HUINT8) * pstDes->ucNumOfComponentRef;
		pstDes->pucComponentRef = (HUINT8 *)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pucComponentRef)
			goto	err_exit;
		HxSTD_MemSet(pstDes->pucComponentRef, 0, ulSize);
		for(i=0;i<pstDes->ucNumOfComponentRef;i++)
		{
			pstDes->pucComponentRef[i] = *p++;
		}
	}

	pstDes->ulISO639LanguageCode = HxMACRO_Get24Bit(p);
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode [3] = '\0';
	p += 3;
	pstDes->ucTextLen = *p++;

	if(pstDes->ucTextLen > 0)
	{
		nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, pstDes->ucTextLen, (const HUINT8 *)p, &pszText);
		if(pszText != NULL)
		{
			ulSize = sizeof(HUINT8) * nTempTextLen;
			pstDes->pucTextChar = (HUINT8 *)silib_desc_alloc(ulSize+1);
			if (NULL == pstDes->pucTextChar)
				goto err_exit;

			HxSTD_MemSet(pstDes->pucTextChar, 0, ulSize);
			ulSize = silib_desc_util_dvbStrlen(pszText);
			HxSTD_MemCopy(pstDes->pucTextChar, pszText, ulSize);
			pstDes->pucTextChar[ulSize] = ARIB_DES_EOS;
			silib_desc_free(pszText);
			pszText = NULL;
		}
	}

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszText);
	freeDataContentInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyDataContentBxmlInfo(SIxAribBxmlInfo_t **ppstDest, const SIxAribBxmlInfo_t *pstSource)
{
	SIxError_e						 eErr;
	SIxAribBxmlInfo_t				*pstInfo;
	SIxAribCarouselInfo_t			*pstCarouselInfo;
	SIxAribStoredCarouselInfo_t		*pstStoredCarouselInfo;

	eErr = eSIxERR_FAIL;
	if (ppstDest && pstSource)
	{
		HUINT32		ulSize;

		*ppstDest = NULL;

		pstInfo = silib_desc_alloc(sizeof(SIxAribBxmlInfo_t));
		if (NULL == pstInfo)
			goto err_exit;

		HxSTD_MemCopy(pstInfo, pstSource, sizeof(SIxAribBxmlInfo_t));
		pstInfo->pstCarouselInfo = NULL;
		pstInfo->pstStoredCarouselInfo = NULL;

		// Copy Carousel Info
		if (NULL != pstSource->pstCarouselInfo && NULL != pstSource->pstCarouselInfo->pstCarouselComponent && 0 < pstSource->pstCarouselInfo->ulNumOfCarousels)
		{
			ulSize = sizeof(SIxAribCarouselInfo_t);
			pstInfo->pstCarouselInfo = (SIxAribCarouselInfo_t *)silib_desc_alloc(ulSize);
			if (NULL == pstInfo->pstCarouselInfo)
				goto err_exit;

			pstCarouselInfo = pstInfo->pstCarouselInfo;
			HxSTD_MemCopy(pstCarouselInfo, pstSource->pstCarouselInfo, ulSize);
			pstCarouselInfo->ulNumOfCarousels = 0;
			pstCarouselInfo->pstCarouselComponent = NULL;

			ulSize = sizeof(SIxAribCarouselComponent_t) * pstInfo->pstCarouselInfo->ulNumOfCarousels;
			pstCarouselInfo->pstCarouselComponent = silib_desc_alloc(ulSize);
			if (NULL == pstCarouselInfo->pstCarouselComponent)
				goto err_exit;

			pstCarouselInfo->ulNumOfCarousels = pstInfo->pstCarouselInfo->ulNumOfCarousels;
			HxSTD_MemCopy(pstCarouselInfo->pstCarouselComponent, pstInfo->pstCarouselInfo->pstCarouselComponent, ulSize);
		}

		// Copy Stored Carousel Info
		if (NULL != pstSource->pstStoredCarouselInfo && NULL!= pstSource->pstStoredCarouselInfo->pstStoredCarouselComponent && 0 < pstSource->pstStoredCarouselInfo->ucNumOfCarousels)
		{
			ulSize = sizeof(SIxAribStoredCarouselInfo_t);
			pstInfo->pstStoredCarouselInfo = (SIxAribStoredCarouselInfo_t *)silib_desc_alloc(ulSize);
			if (NULL == pstInfo->pstStoredCarouselInfo)
				goto err_exit;

			pstStoredCarouselInfo = pstInfo->pstStoredCarouselInfo;

			ulSize = sizeof(SIxAribStoredCarouselComponent_t)*pstSource->pstStoredCarouselInfo->ucNumOfCarousels;
			pstStoredCarouselInfo->pstStoredCarouselComponent = silib_desc_alloc(ulSize);
			if (NULL == pstStoredCarouselInfo->pstStoredCarouselComponent)
				goto err_exit;

			pstStoredCarouselInfo->ucNumOfCarousels = pstSource->pstStoredCarouselInfo->ucNumOfCarousels;
			HxSTD_MemCopy(pstStoredCarouselInfo->pstStoredCarouselComponent, pstSource->pstStoredCarouselInfo->pstStoredCarouselComponent, ulSize);
		}

		eErr = eSIxERR_OK;

		*ppstDest = pstInfo;
	}

	return eErr;

err_exit:
	freeDataContentBxmlInfo(pstInfo);
	return eSIxERR_FAIL;
}

static SIxError_e copyDataContentCaptionInfo(SIxAribCaptionInfo_t **ppstDest, const SIxAribCaptionInfo_t *pstSource)
{
	SIxError_e				 eErr;
	SIxAribCaptionInfo_t 	*pstInfo;

	eErr = eSIxERR_FAIL;
	if (ppstDest && pstSource && 0 < pstSource->ucNumLanguages)
	{
		HUINT32 ulSize;

		*ppstDest = NULL;

		pstInfo = (SIxAribCaptionInfo_t *)silib_desc_alloc(sizeof(SIxAribCaptionInfo_t));
		if (NULL == pstInfo)
			goto err_exit;

		HxSTD_MemCopy(pstInfo, pstSource, sizeof(SIxAribCaptionInfo_t));
		pstInfo->pucLanguageTag			= NULL;
		pstInfo->pucDMF					= NULL;
		pstInfo->pulISO639LanguageCode	= NULL;

		// Copy Languge Tags
		ulSize = sizeof(HUINT8)*pstSource->ucNumLanguages;
		pstInfo->pucLanguageTag = (HUINT8 *)silib_desc_alloc(ulSize);
		if (NULL == pstInfo->pucLanguageTag)
			goto err_exit;

		HxSTD_MemCopy(pstInfo->pucLanguageTag, pstSource->pucLanguageTag, ulSize);

		// Copy DMF
		ulSize = sizeof(HUINT8)*pstSource->ucNumLanguages;
		pstInfo->pucDMF = (HUINT8 *)silib_desc_alloc(ulSize);
		if (NULL == pstInfo->pucDMF)
			goto err_exit;

		HxSTD_MemCopy(pstInfo->pucDMF, pstSource->pucDMF, ulSize);

		// Copy ISO Language Code
		ulSize = sizeof(HUINT32)*pstSource->ucNumLanguages;
		pstInfo->pulISO639LanguageCode = (HUINT32 *)silib_desc_alloc(ulSize);
		if (NULL == pstInfo->pulISO639LanguageCode)
			goto err_exit;

		HxSTD_MemCopy(pstInfo->pulISO639LanguageCode, pstSource->pulISO639LanguageCode, ulSize);

		*ppstDest = pstInfo;
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	freeDataContentCaptionInfo(pstInfo);
	return eSIxERR_FAIL;
}

static SIxError_e copyDataContentComponentRef(HUINT8 *pucDestNum, HUINT8 **ppucDestComponentRef, HUINT8 ucNumOfComponentRef, const HUINT8 *pucComponentRef)
{
	SIxError_e		 eErr;
	HUINT8			*pucCompRef = NULL;

	if (NULL == pucDestNum || NULL == ppucDestComponentRef || 0 == ucNumOfComponentRef || NULL == pucComponentRef)
		return eSIxERR_FAIL;

	eErr = eSIxERR_FAIL;

	*pucDestNum = ucNumOfComponentRef;

	pucCompRef = silib_desc_alloc(sizeof(HUINT8)*ucNumOfComponentRef);
	if (NULL == pucCompRef)
		goto err_exit;

	HxSTD_MemCopy(pucCompRef, pucComponentRef, sizeof(HUINT8)*ucNumOfComponentRef);

	*ppucDestComponentRef = pucCompRef;

	return eErr;

err_exit:
	if (pucDestNum)				*pucDestNum = 0;
	if (ppucDestComponentRef)	*ppucDestComponentRef = NULL;
	silib_desc_free(pucCompRef);
	return eSIxERR_FAIL;
}

static SIxError_e copyDataContentTextChar(HUINT8 **ppucDest, const HUINT8 *pucSource)
{
	SIxError_e		eErr;

	eErr = eSIxERR_FAIL;
	if (ppucDest && pucSource)
	{
		HUINT8	*pucText = NULL;
		HUINT32  ulSize;

		ulSize = silib_desc_util_dvbStrlen(pucSource);

		pucText = (HUINT8 *)silib_desc_alloc(ulSize+1);
		HxSTD_MemCopy(pucText, pucSource, ulSize);
		pucText[ulSize] = '\0';

		*ppucDest = pucText;
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e copyDataContentInfo(SIxDataContentDes_t *pstDest, const SIxDataContentDes_t *pstSource)
{
	SIxError_e	eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDataContentDes_t));
		pstDest->pstAribBxmlInfo		= NULL;
		pstDest->pstAribCaptionInfo		= NULL;
		pstDest->ucNumOfComponentRef	= 0;
		pstDest->pucComponentRef		= NULL;
		pstDest->pucTextChar			= NULL;
		pstDest->pstNext				= NULL;

		if (pstSource->pstAribBxmlInfo)
		{
			eErr = copyDataContentBxmlInfo(&pstDest->pstAribBxmlInfo, (const SIxAribBxmlInfo_t *)pstSource->pstAribBxmlInfo);
			if (eSIxERR_OK != eErr)
			{
				pstDest->pstAribBxmlInfo = NULL;
				goto err_exit;
			}
		}

		if (pstSource->pstAribCaptionInfo)
		{
			eErr = copyDataContentCaptionInfo(&pstDest->pstAribCaptionInfo, (const SIxAribCaptionInfo_t *)pstSource->pstAribCaptionInfo);
			if (eSIxERR_OK != eErr)
			{
				pstDest->pstAribCaptionInfo = NULL;
				goto err_exit;
			}
		}

		if (0 < pstSource->ucNumOfComponentRef && NULL != pstSource->pucComponentRef)
		{
			eErr = copyDataContentComponentRef(&pstDest->ucNumOfComponentRef, &pstDest->pucComponentRef,
														pstSource->ucNumOfComponentRef, (const HUINT8 *)pstSource->pucComponentRef);
			if (eSIxERR_OK != eErr)
			{
				pstDest->ucNumOfComponentRef = 0;
				pstDest->pucComponentRef = NULL;
				goto err_exit;
			}
		}

		if (pstSource->pucTextChar)
		{
			eErr = copyDataContentTextChar(&pstDest->pucTextChar, (const HUINT8 *)pstSource->pucTextChar);
			if (eSIxERR_OK != eErr)
			{
				pstDest->pucTextChar = NULL;
				goto err_exit;
			}
		}

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeDataContentInfo(pstDest);
	return eSIxERR_FAIL;
}

static SIxError_e copyDataContentDes(SIxDataContentDes_t *pstDest, const SIxDataContentDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;
	SIxDataContentDes_t		*pstDesList = NULL;

	if (pstSource && pstDest)
	{
		SIxDataContentDes_t	*pstSourceDes, *pstNewDes = NULL;

		if (copyDataContentInfo(pstDest, pstSource) != eSIxERR_OK)
		{
			goto err_exit;
		}

		eErr = eSIxERR_OK;
		pstDesList = pstDest;
		pstSourceDes = (SIxDataContentDes_t *)pstSource->pstNext;
		while (pstSourceDes)
		{
			pstNewDes = (SIxDataContentDes_t *)silib_desc_alloc(sizeof(SIxDataContentDes_t));
			if (pstNewDes == NULL)
				goto err_exit;

			if (copyDataContentInfo(pstNewDes, pstSourceDes) != eSIxERR_OK)
			{
				silib_desc_free(pstNewDes);
				goto err_exit;
			}

			pstDesList->pstNext = pstNewDes;
			pstDesList = pstDesList->pstNext;

			pstSourceDes = pstSourceDes->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		freeDataContentInfo(pstDest);
		freeDataContentDesList(pstDest->pstNext);
	}
	return eSIxERR_FAIL;
}


//VideoDecodeControl
static void initVideoDecodeControlDes(SIxVideoDecodeControlDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxVideoDecodeControlDes_t));
}

static SIxVideoDecodeControlDes_t *allocVideoDecodeControlDes(void)
{
	return (SIxVideoDecodeControlDes_t *)silib_desc_alloc(sizeof(SIxVideoDecodeControlDes_t));
}

static void freeVideoDecodeControlDes(SIxVideoDecodeControlDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseVideoDecodeControlDes(HUINT8 *p, SIxVideoDecodeControlDes_t *pstDes)
{
	HUINT8				 	ucDesLen;

	//	p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->bStillPictureFlag = (p[2] & 0x80) >> 7;
	pstDes->bSequenceEndCodeFlag = (p[2] & 0x40) >> 6;
	pstDes->ucVideoEncodeFormat = (p[2] & 0x3C) >> 2;

	return eSIxERR_OK;

}

static SIxError_e copyVideoDecodeControlDes(SIxVideoDecodeControlDes_t *pstDest, const SIxVideoDecodeControlDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxVideoDecodeControlDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


//CaEmmTs
static void initCaEmmTsDes(SIxCaEmmTsDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxCaEmmTsDes_t));
	pstDes->pstNext = NULL;
}

static SIxCaEmmTsDes_t *allocCaEmmTsDes(void)
{
	return (SIxCaEmmTsDes_t *)silib_desc_alloc(sizeof(SIxCaEmmTsDes_t));
}

static void freeCaEmmTsDes(SIxCaEmmTsDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseCaEmmTsDes(HUINT8 *p, SIxCaEmmTsDes_t *pstDes)
{
	HUINT8			 ucDesLen;
	HUINT8			*pTemp;

	// p[0];				// Tag
	ucDesLen = p[1];		// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pTemp = p + 2;
	pstDes->usCaSystemId		= HxMACRO_Get16Bit(pTemp);
	pTemp = p + 4;
	pstDes->usTransportStreamId	= HxMACRO_Get16Bit(pTemp);
	pTemp = p + 6;
	pstDes->usOriginalNetworkId	= HxMACRO_Get16Bit(pTemp);
	pTemp = p + 8;
	pstDes->ucPowerSupplyPeriod	= HxMACRO_Get8Bit(pTemp);

	return eSIxERR_OK;

}

static SIxError_e copyCaEmmTsDes(SIxCaEmmTsDes_t *pstDest, const SIxCaEmmTsDes_t *pstSource)
{
	SIxError_e			 eErr = eSIxERR_FAIL;
	SIxCaEmmTsDes_t		*pstDesList = NULL;

	if (pstSource && pstDest)
	{
		SIxCaEmmTsDes_t	*pstSourceDes, *pstNewDes = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCaEmmTsDes_t));
		pstDest->pstNext = NULL;

		pstDesList = pstDest;
		pstSourceDes = pstSource->pstNext;
		while (pstSourceDes)
		{
			pstNewDes = (SIxCaEmmTsDes_t *)silib_desc_alloc(sizeof(SIxCaEmmTsDes_t));
			if (pstNewDes == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewDes, pstSourceDes, sizeof(SIxCaEmmTsDes_t));

			pstDesList->pstNext = pstNewDes;
			pstDesList = pstDesList->pstNext;

			pstSourceDes = pstSourceDes->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstNext, SIxCaEmmTsDes_t);
	}
	return eSIxERR_FAIL;
}


//CaContractInfo
static void initCaContractInfoDes(SIxCaContractInfoDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxCaContractInfoDes_t));
	pstDes->pstNext = NULL;
}

static SIxCaContractInfoDes_t *allocCaContractInfoDes(void)
{
	return (SIxCaContractInfoDes_t *)silib_desc_alloc(sizeof(SIxCaContractInfoDes_t));
}

static void freeCaContractInfo(SIxCaContractInfoDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pucComponentTag);
		pstDes->pucComponentTag = NULL;
		silib_desc_free(pstDes->pucContractVerificationInfo);
		pstDes->pucContractVerificationInfo = NULL;
		silib_desc_free(pstDes->pucFeeName);
		pstDes->pucFeeName = NULL;

		// Note!!!
		// DO NOT Free ''pstNext'' and ''pstDes''
	}
}

static void freeCaContractInfoAll(SIxCaContractInfoDes_t *pstDes)
{
	freeCaContractInfo(pstDes);
	silib_desc_free(pstDes);
}

static void freeCaContractInfoDes(SIxCaContractInfoDes_t *pstDes)
{
	FREE_INFO_LIST_EX(pstDes, SIxCaContractInfoDes_t, freeCaContractInfoAll);
}

static SIxError_e parseCaContractInfoDes(HUINT8 *p, SIxCaContractInfoDes_t *pstDes)
{
	HUINT8			ucDesLen;
	HINT32			i, ulSize;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->usCASystemId = HxMACRO_Get16Bit(p + 2);
	pstDes->ucCAUnitId = (p[4] >> 4) & 0x0F;
	pstDes->ucNumOfComponent = (p[4] & 0x0F);
	p += 5;
	if(pstDes->ucNumOfComponent > 0)
	{
		ulSize = sizeof(HUINT8) * pstDes->ucNumOfComponent;
		pstDes->pucComponentTag = (HUINT8 *)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pucComponentTag)
			goto err_exit;

		HxSTD_MemSet(pstDes->pucComponentTag, 0, ulSize);
		for(i=0;i<pstDes->ucNumOfComponent;i++)
		{
			pstDes->pucComponentTag[i] = HxMACRO_Get8Bit(p);
			p += 1;
		}
	}

	pstDes->ucContractVerificationInfoLength = HxMACRO_Get8Bit(p);
	p += 1;
	if(pstDes->ucContractVerificationInfoLength > 0)
	{
		ulSize = sizeof(HUINT8) * pstDes->ucContractVerificationInfoLength;
		pstDes->pucContractVerificationInfo = (HUINT8*)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pucContractVerificationInfo)
			goto err_exit;

		HxSTD_MemSet(pstDes->pucContractVerificationInfo, 0, ulSize);
		for(i=0;i<pstDes->ucContractVerificationInfoLength;i++)
		{
			pstDes->pucContractVerificationInfo[i] = HxMACRO_Get8Bit(p);
			p += 1;
		}
	}

	pstDes->ucFeeNameLength = HxMACRO_Get8Bit(p);
	p += 1;
	if(pstDes->ucFeeNameLength > 0)
	{
		ulSize = sizeof(HUINT8) * pstDes->ucFeeNameLength;
		pstDes->pucFeeName = (HUINT8*)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pucFeeName)
			goto err_exit;

		HxSTD_MemSet(pstDes->pucFeeName, 0, ulSize);
		for(i=0;i<pstDes->ucFeeNameLength;i++)
		{
			pstDes->pucFeeName[i] = HxMACRO_Get8Bit(p);
			p += 1;
		}
	}

	return eSIxERR_OK;

err_exit:
	freeCaContractInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyCaContractInfo(SIxCaContractInfoDes_t *pstDest, const SIxCaContractInfoDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32		ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCaContractInfoDes_t));
		pstDest->ucContractVerificationInfoLength = 0;
		pstDest->pucComponentTag = NULL;
		pstDest->ucContractVerificationInfoLength = 0;
		pstDest->pucContractVerificationInfo = NULL;
		pstDest->ucFeeNameLength = 0;
		pstDest->pucFeeName = NULL;
		pstDest->pstNext = NULL;

		if (0 < pstSource->ucNumOfComponent && NULL != pstSource->pucComponentTag)
		{
			ulSize = sizeof(HUINT8)*pstSource->ucNumOfComponent;
			pstDest->pucComponentTag = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pucComponentTag)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucComponentTag, pstSource->pucComponentTag, ulSize);
			pstDest->ucNumOfComponent = pstSource->ucNumOfComponent;
		}

		if (0 < pstSource->ucContractVerificationInfoLength && NULL != pstSource->pucContractVerificationInfo)
		{
			ulSize = sizeof(HUINT8)*pstSource->ucContractVerificationInfoLength;
			pstDest->pucContractVerificationInfo = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pucContractVerificationInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucContractVerificationInfo, pstSource->pucContractVerificationInfo, ulSize);
			pstDest->ucContractVerificationInfoLength = pstSource->ucContractVerificationInfoLength;
		}

		if (0 < pstSource->ucFeeNameLength && NULL != pstSource->pucFeeName)
		{
			ulSize = sizeof(HUINT8)*pstSource->ucFeeNameLength;
			pstDest->pucFeeName = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pucFeeName)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucFeeName, pstSource->pucFeeName, ulSize);
			pstDest->ucFeeNameLength = pstSource->ucFeeNameLength;
		}

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeCaContractInfo(pstDest);
	return eSIxERR_FAIL;
}

static SIxError_e copyCaContractInfoDes(SIxCaContractInfoDes_t *pstDest, const SIxCaContractInfoDes_t *pstSource)
{
	SIxError_e					 eErr = eSIxERR_FAIL;
	SIxCaContractInfoDes_t		*pstDesList = NULL;

	if (pstSource && pstDest)
	{
		SIxCaContractInfoDes_t	*pstSourceDes, *pstNewDes = NULL;

		if (copyCaContractInfo(pstDest, pstSource) != eSIxERR_OK)
			goto err_exit;

		pstDesList = pstDest;
		pstSourceDes = pstSource->pstNext;
		while (pstSourceDes)
		{
			pstNewDes = (SIxCaContractInfoDes_t *)silib_desc_alloc(sizeof(SIxCaContractInfoDes_t));
			if (pstNewDes == NULL)
				goto err_exit;

			if (copyCaContractInfo(pstNewDes, pstSourceDes) != eSIxERR_OK)
			{
				silib_desc_free(pstNewDes);
				goto err_exit;
			}

			pstDesList->pstNext = pstNewDes;
			pstDesList = pstDesList->pstNext;

			pstSourceDes = pstSourceDes->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		freeCaContractInfo(pstDest);
		FREE_INFO_LIST_EX(pstDest->pstNext, SIxCaContractInfoDes_t, freeCaContractInfoAll);
	}
	return eSIxERR_FAIL;
}


//TsInformation
static void initTsInformationDes(SIxTsInformationDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxTsInformationDes_t));
	pstDes->pucTsNameChar = NULL;
	pstDes->pstTransmissionTypeInfo = NULL;
}

static SIxTsInformationDes_t *allocTsInformationDes(void)
{
	return (SIxTsInformationDes_t *)silib_desc_alloc(sizeof(SIxTsInformationDes_t));
}

static void freeTsInformationDesInfo(SIxTsInformationDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pucTsNameChar);
		if (pstDes->pstTransmissionTypeInfo)
		{
			HUINT32 i;
			for (i=0 ; i<pstDes->ucTransmissionTypeCount; i++)
			{
				silib_desc_free(pstDes->pstTransmissionTypeInfo[i].pusServiceId);
			}
			silib_desc_free(pstDes->pstTransmissionTypeInfo);
		}
	}
}

static void freeTsInformationDes(SIxTsInformationDes_t *pstDes)
{
	if (pstDes)
	{
		freeTsInformationDesInfo(pstDes);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseTsInformationDes(HUINT8 *p, SIxTsInformationDes_t *pstDes)
{
	HUINT8									 ucDesLen;
	HUINT32									 i, j;
	SIxAribTsInfoDesTransmissionTypeInfo_t	*pstTransmissionTypeInfo;
	HUINT8				*pszText = NULL;
	HUINT32				nTempTextLen;

	// pucRaw[0];			// Tag
	ucDesLen = p[1];

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucRemoteControlKeyId = p[2];
	pstDes->ucLengthOfTsName = (p[3] & 0xFC) >> 2;
	pstDes->ucTransmissionTypeCount = p[3] & 0x03;
	p += 4;

	nTempTextLen = silib_desc_util_textToUTF8(NULL, pstDes->stHead.stParam.eTextEnc, pstDes->ucLengthOfTsName, (const HUINT8 *)p, &pszText);
	if(pszText != NULL)
	{
		pstDes->pucTsNameChar = silib_desc_alloc(sizeof(HUINT8) * (nTempTextLen + 1));
		if (NULL == pstDes->pucTsNameChar)
			goto err_exit;

		HxSTD_MemSet(pstDes->pucTsNameChar, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
		HxSTD_MemCopy(pstDes->pucTsNameChar, pszText, silib_desc_util_dvbStrlen(pszText));
		pstDes->pucTsNameChar[silib_desc_util_dvbStrlen(pszText)] = ARIB_DES_EOS;
		silib_desc_free(pszText);
	}
	p += pstDes->ucLengthOfTsName;

	if(pstDes->ucTransmissionTypeCount > 0)
	{
		pstDes->pstTransmissionTypeInfo = (SIxAribTsInfoDesTransmissionTypeInfo_t *)silib_desc_alloc(sizeof(SIxAribTsInfoDesTransmissionTypeInfo_t) * pstDes->ucTransmissionTypeCount);
		if (NULL == pstDes->pstTransmissionTypeInfo)
			goto err_exit;

		HxSTD_MemSet(pstDes->pstTransmissionTypeInfo, 0, (sizeof(SIxAribTsInfoDesTransmissionTypeInfo_t) * pstDes->ucTransmissionTypeCount));
		pstTransmissionTypeInfo = pstDes->pstTransmissionTypeInfo;
		for(i=0;i<pstDes->ucTransmissionTypeCount;i++)
		{
			pstTransmissionTypeInfo[i].ucTransmissionTypeInfo = *p++;
			pstTransmissionTypeInfo[i].ucNumOfService = *p++;
			if(pstTransmissionTypeInfo[i].ucNumOfService > 0)
			{
				pstTransmissionTypeInfo[i].pusServiceId = (HUINT16*)silib_desc_alloc(sizeof(HUINT16) * pstTransmissionTypeInfo[i].ucNumOfService);
				if (NULL == pstTransmissionTypeInfo[i].pusServiceId)
					goto err_exit;

				HxSTD_MemSet(pstTransmissionTypeInfo[i].pusServiceId, 0, (sizeof(HUINT16) * pstTransmissionTypeInfo[i].ucNumOfService));
				for(j=0;j<pstTransmissionTypeInfo[i].ucNumOfService;j++)
				{
					pstTransmissionTypeInfo[i].pusServiceId[j] = ((p[0] << 8) & 0xFF00) | p[1];
					p += 2;
				}
			}
		}
	}

	return eSIxERR_OK;

err_exit:
	freeTsInformationDes(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyTsInformationDes(SIxTsInformationDes_t *pstDest, const SIxTsInformationDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32		ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTsInformationDes_t));
		pstDest->ucLengthOfTsName = 0;
		pstDest->pucTsNameChar = NULL;
		pstDest->ucTransmissionTypeCount = 0;
		pstDest->pstTransmissionTypeInfo = NULL;

		if (0 < pstSource->ucLengthOfTsName && NULL != pstSource->pucTsNameChar)
		{
			ulSize = sizeof(HUINT8)*pstSource->ucLengthOfTsName;
			pstDest->pucTsNameChar = silib_desc_alloc(ulSize+1);
			if (NULL == pstDest->pucTsNameChar)
				goto err_exit;
			HxSTD_MemCopy(pstDest->pucTsNameChar, pstSource->pucTsNameChar, ulSize);
			pstDest->pucTsNameChar[ulSize] = '\0';
			pstDest->ucLengthOfTsName = pstSource->ucLengthOfTsName;
		}

		if (0 < pstSource->ucTransmissionTypeCount && NULL != pstSource->pstTransmissionTypeInfo)
		{
			HUINT32		i;
			SIxAribTsInfoDesTransmissionTypeInfo_t	*astSourceInfo, *astDestInfo;

			ulSize = sizeof(SIxAribTsInfoDesTransmissionTypeInfo_t)*pstSource->ucTransmissionTypeCount;

			pstDest->pstTransmissionTypeInfo = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pstTransmissionTypeInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pstTransmissionTypeInfo, pstSource->pstTransmissionTypeInfo, ulSize);
			pstDest->ucTransmissionTypeCount = pstSource->ucTransmissionTypeCount;

			astDestInfo = pstDest->pstTransmissionTypeInfo;
			astSourceInfo = pstSource->pstTransmissionTypeInfo;
			for (i=0; i<pstSource->ucTransmissionTypeCount; i++)
			{
				if (0 < astSourceInfo->ucNumOfService && NULL != astSourceInfo->pusServiceId)
				{
					ulSize = sizeof(HUINT16)*astSourceInfo->ucNumOfService;
					astDestInfo[i].pusServiceId = silib_desc_alloc(ulSize);
					if (NULL == astSourceInfo[i].pusServiceId)
						goto err_exit;

					HxSTD_MemCopy(astDestInfo[i].pusServiceId, astSourceInfo[i].pusServiceId, ulSize);
					astDestInfo[i].ucNumOfService = astSourceInfo->ucNumOfService;
				}
			}
		}

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeTsInformationDesInfo(pstDest);
	return eSIxERR_FAIL;
}


//ExtendedBroadcaster
static void initExtendedBroadcasterDes(SIxExtendedBroadcasterDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxExtendedBroadcasterDes_t));
}

static SIxExtendedBroadcasterDes_t *allocExtendedBroadcasterDes(void)
{
	return (SIxExtendedBroadcasterDes_t *)silib_desc_alloc(sizeof(SIxExtendedBroadcasterDes_t));
}

static void freeExtendedBroadcasterDesInfo(SIxExtendedBroadcasterDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pucAffiliationId);
		silib_desc_free(pstDes->pucBroadcasterId);
		silib_desc_free(pstDes->pusOriginalNetworkId);
	}
}

static void freeExtendedBroadcasterDes(SIxExtendedBroadcasterDes_t *pstDes)
{
	freeExtendedBroadcasterDesInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseExtendedBroadcasterDes(HUINT8 *p, SIxExtendedBroadcasterDes_t *pstDes)
{
	HUINT8		 	ucDesLen;
	HINT32			i;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucBroadcasterType = (p[2] & 0xF0) >> 4;
	if((pstDes->ucBroadcasterType == 0x01) || (pstDes->ucBroadcasterType == 0x02))
	{
		pstDes->usTerrestrialBroadcasterId = (p[3] << 8) | p[4];
		pstDes->ucNumberOfAffiliationIdLoop = (p[5] >> 4) & 0x0F;
		pstDes->ucNumberOfBroadcasterIdLoop = (p[5] & 0x0F);
		p += 6;

		if(pstDes->ucNumberOfAffiliationIdLoop > 0)
		{
			pstDes->pucAffiliationId = (HUINT8*)silib_desc_alloc(sizeof(HUINT8) * pstDes->ucNumberOfAffiliationIdLoop);
			if (NULL == pstDes->pucAffiliationId )
				goto err_exit;

			HxSTD_MemSet(pstDes->pucAffiliationId, 0, (sizeof(HUINT8) * pstDes->ucNumberOfAffiliationIdLoop));

			for(i=0;i<pstDes->ucNumberOfAffiliationIdLoop;i++)
			{
				pstDes->pucAffiliationId[i] = *p++;
			}
		}

		if(pstDes->ucNumberOfBroadcasterIdLoop > 0)
		{
			pstDes->pusOriginalNetworkId = (HUINT16*)silib_desc_alloc(sizeof(HUINT16) * pstDes->ucNumberOfBroadcasterIdLoop);
			if (NULL == pstDes->pusOriginalNetworkId )
				goto err_exit;

			HxSTD_MemSet(pstDes->pusOriginalNetworkId, 0, (sizeof(HUINT16) * pstDes->ucNumberOfBroadcasterIdLoop));

			pstDes->pucBroadcasterId = (HUINT8*)silib_desc_alloc(sizeof(HUINT8) * pstDes->ucNumberOfBroadcasterIdLoop);
			if (NULL == pstDes->pucBroadcasterId )
				goto err_exit;

			HxSTD_MemSet(pstDes->pucBroadcasterId, 0, (sizeof(HUINT8) * pstDes->ucNumberOfBroadcasterIdLoop));

			for(i=0;i<pstDes->ucNumberOfBroadcasterIdLoop;i++)
			{
				pstDes->pusOriginalNetworkId[i] = (p[0] << 8) | p[1];
				pstDes->pucBroadcasterId[i] = p[2];
				p += 3;
			}
		}
	}

	return eSIxERR_OK;

err_exit:
	freeExtendedBroadcasterDesInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyExtendedBroadcasterDes(SIxExtendedBroadcasterDes_t *pstDest, const SIxExtendedBroadcasterDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32	ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxExtendedBroadcasterDes_t));
		pstDest->ucNumberOfAffiliationIdLoop = 0;
		pstDest->ucNumberOfBroadcasterIdLoop = 0;
		pstDest->pucAffiliationId = NULL;
		pstDest->pucBroadcasterId = NULL;
		pstDest->pusOriginalNetworkId = NULL;

		if (0 < pstSource->ucNumberOfAffiliationIdLoop && NULL != pstSource->pucAffiliationId)
		{
			ulSize = sizeof(HUINT8)*pstSource->ucNumberOfAffiliationIdLoop;
			pstDest->pucAffiliationId = (HUINT8 *)silib_desc_alloc(ulSize);
			if (NULL == pstDest->pucAffiliationId)
				goto err_exit;
			HxSTD_MemCopy(pstDest->pucAffiliationId, pstSource->pucAffiliationId, ulSize);
			pstDest->ucNumberOfAffiliationIdLoop = pstSource->ucNumberOfAffiliationIdLoop;
		}

		pstDest->ucNumberOfBroadcasterIdLoop = pstSource->ucNumberOfBroadcasterIdLoop;
		if (0 < pstSource->ucNumberOfBroadcasterIdLoop && NULL != pstSource->pucBroadcasterId)
		{
			ulSize = sizeof(HUINT8)*pstSource->ucNumberOfBroadcasterIdLoop;
			pstDest->pucBroadcasterId = (HUINT8 *)silib_desc_alloc(ulSize);
			if (NULL == pstDest->pucBroadcasterId)
				goto err_exit;
			HxSTD_MemCopy(pstDest->pucBroadcasterId, pstSource->pucBroadcasterId, ulSize);
		}

		if (0 < pstSource->ucNumberOfBroadcasterIdLoop && NULL != pstSource->pusOriginalNetworkId)
		{
			ulSize = sizeof(HUINT16)*pstSource->ucNumberOfBroadcasterIdLoop;
			pstDest->pusOriginalNetworkId = (HUINT16 *)silib_desc_alloc(ulSize);
			if (NULL == pstDest->pusOriginalNetworkId)
				goto err_exit;
			HxSTD_MemCopy(pstDest->pusOriginalNetworkId, pstSource->pusOriginalNetworkId, ulSize);
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	freeExtendedBroadcasterDesInfo(pstDest);
	return eSIxERR_FAIL;
}


//LogoTransmission
static void initLogoTransmissionDes(SIxLogoTransmissionDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxLogoTransmissionDes_t));
}

static SIxLogoTransmissionDes_t *allocLogoTransmissionDes(void)
{
	return (SIxLogoTransmissionDes_t *)silib_desc_alloc(sizeof(SIxLogoTransmissionDes_t));
}

static void freeLogoTransmissionDes(SIxLogoTransmissionDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseLogoTransmissionDes(HUINT8 *p, SIxLogoTransmissionDes_t *pstDes)
{
	HUINT8		 	ucDesLen;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucLogoTransmissionType	= p[2];
	pstDes->usLogoId				= (p[3]&0x01 << 8) | p[4];

	return eSIxERR_OK;
}

static SIxError_e copyLogoTransmissionDes(SIxLogoTransmissionDes_t *pstDest, const SIxLogoTransmissionDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxLogoTransmissionDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

//CableTsDivisionSystem
static void initCableTsDivisionSystemDes(SIxCableTsDivisionSystemDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxCableTsDivisionSystemDes_t));
}

static SIxCableTsDivisionSystemDes_t *allocCableTsDivisionSystemDes(void)
{
	return (SIxCableTsDivisionSystemDes_t *)silib_desc_alloc(sizeof(SIxCableTsDivisionSystemDes_t));
}

static void freeCableTsDivisionSystemInfo(SIxCableTsDivisionSystemInfo_t *pstInfo)
{
	if (pstInfo)
	{
		silib_desc_free(pstInfo->pucFutureUseData);
		silib_desc_free(pstInfo->pusServiceId);
		silib_desc_free(pstInfo);
	}
}

static void freeCableTsDivisionSystemDes(SIxCableTsDivisionSystemDes_t *pstDes)
{
	if (pstDes)
	{
		freeCableTsDivisionSystemInfo(pstDes->pstInfo);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseCableTsDivisionSystemDes(HUINT8 *p, SIxCableTsDivisionSystemDes_t *pstDes)
{
	HUINT8					 		 ucDesLen;
	HINT32							 i, j;
	HUINT8							*temp;
	HUINT8							 ucFutureUseDataFlag;
	HUINT8							 ucNumLoops = 0;
	SIxCableTsDivisionSystemInfo_t	*pstInfo;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	temp = p += 2;
	do
	{
		ucFutureUseDataFlag = temp[4] & 0x01;
		ucDesLen -= 11;
		temp += 11;
		if(ucFutureUseDataFlag == 0x00)
		{
			ucDesLen -= (1 + temp[0]);
			temp += (1 + temp[0]);
		}
		ucDesLen -= (1 + temp[0] * 2);
		temp += (1 + temp[0] * 2);
		ucNumLoops++;
		if((ucDesLen < 12) && (ucDesLen != 0))
		{
			HxLOG_Error("ucDesLen(%d) is invalid.(%d)\n",ucDesLen, __LINE__);
		}
	}
	while(ucDesLen);

	if(ucNumLoops == 0)
	{
		return eSIxERR_FAIL;
	}

	pstDes->pstInfo = (SIxCableTsDivisionSystemInfo_t*)silib_desc_alloc(sizeof(SIxCableTsDivisionSystemInfo_t) * ucNumLoops);
	if (NULL == pstDes->pstInfo)
		goto err_exit;

	HxSTD_MemSet(pstDes->pstInfo, 0, (sizeof(SIxCableTsDivisionSystemInfo_t) * ucNumLoops));
	pstDes->ucNumLoops = ucNumLoops;
	for(i=0;i<ucNumLoops;i++)
	{
		pstInfo = &(pstDes->pstInfo[i]);
		/* [original] 4bits BCD 8행 ex. 733MHz -> 0x07330000 */
		pstInfo->ulFrequency = (((p[0]>>4)&0x0F)*1000000) + ((p[0]&0x0F)*100000)
								+ (((p[1]>>4)&0x0F)*10000) + ((p[1]&0x0F)*1000)
								+ (((p[2]>>4)&0x0F)*100)	+ ((p[2]&0x0F)*10)
								+ ((p[3]>>4)&0x0F);
		pstInfo->ucFutureUseDataFlag = p[4] & 0x01;
		pstInfo->ucFrameType = (p[5] & 0xF0) >> 4;
		pstInfo->ucFECOuter = p[5] & 0x0F;
		pstInfo->ucModulation = p[6];
		/* what's mean? */
		pstInfo->ulSymbolRate = (((p[7]>>4)&0x0F)*100000)	+ ((p[7]&0x0F)*10000)
								+ (((p[8]>>4)&0x0F)*1000) + ((p[8]&0x0F)*100)
								+ (((p[9]>>4)&0x0F)*10)	+ (p[9]&0x0F);
								/*+ (((p[10]&0xF0)>>4)*10);  */
		pstInfo->ucFECInner = p[10] & 0x0F;
		p += 11;
		if(pstInfo->ucFutureUseDataFlag == 0x00)
		{
			pstInfo->ucFutureUseDataLength = *p++;
			if(pstInfo->ucFutureUseDataLength > 0)
			{
				pstInfo->pucFutureUseData = (HUINT8*)silib_desc_alloc(sizeof(HUINT8) * pstInfo->ucFutureUseDataLength);
				if (NULL == pstInfo->pucFutureUseData)
					goto err_exit;
				HxSTD_MemSet(pstInfo->pucFutureUseData, 0, (sizeof(HUINT8) * pstInfo->ucFutureUseDataLength));
				for(j=0;j<pstInfo->ucFutureUseDataLength;j++)
				{
					pstInfo->pucFutureUseData[i] = *p++;
				}
			}
		}

		pstInfo->ucNumberOfServices = *p++;
		if(pstInfo->ucNumberOfServices > 0)
		{
			pstInfo->pusServiceId = (HUINT16*)silib_desc_alloc(sizeof(HUINT16) * pstInfo->ucNumberOfServices);
			if (NULL == pstInfo->pusServiceId)
				goto err_exit;

			HxSTD_MemSet(pstInfo->pusServiceId, 0, (sizeof(HUINT16) * pstInfo->ucNumberOfServices));
			for(j=0;j<pstInfo->ucNumberOfServices;j++)
			{
				pstInfo->pusServiceId[i] = ((p[0] << 8) & 0xFF00) | p[1];
				p += 2;
			}
		}
	}

	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyCableTsDivisionSystemDes(SIxCableTsDivisionSystemDes_t *pstDest, const SIxCableTsDivisionSystemDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCableTsDivisionSystemDes_t));
		pstDest->ucNumLoops = 0;
		pstDest->pstInfo = NULL;

		if (0 < pstSource->ucNumLoops && NULL != pstSource->pstInfo)
		{
			HUINT32									 ulSize;
			SIxCableTsDivisionSystemInfo_t			*pstInfo;
			const SIxCableTsDivisionSystemInfo_t		*pstSourceInfo;

			ulSize = sizeof(SIxCableTsDivisionSystemInfo_t)*pstSource->ucNumLoops;
			pstDest->pstInfo = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pstInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pstInfo, pstSource->pstInfo, ulSize);
			pstDest->ucNumLoops = pstSource->ucNumLoops;

			pstInfo = pstDest->pstInfo;

			pstSourceInfo = pstSource->pstInfo;
			if (0 < pstSourceInfo->ucFutureUseDataLength && NULL != pstSourceInfo->pucFutureUseData)
			{
				ulSize = sizeof(HUINT8)*pstSourceInfo->ucFutureUseDataLength;
				pstInfo->pucFutureUseData = silib_desc_alloc(ulSize);
				if (NULL == pstInfo->pucFutureUseData)
					goto err_exit;
				HxSTD_MemCopy(pstInfo->pucFutureUseData, pstSourceInfo->pucFutureUseData, ulSize);
			}

			if (0 < pstSourceInfo->ucNumberOfServices && NULL != pstSourceInfo->pusServiceId)
			{
				ulSize = sizeof(HUINT16)*pstSourceInfo->ucNumberOfServices;
				pstInfo->pusServiceId = silib_desc_alloc(ulSize);
				if (NULL == pstInfo->pusServiceId)
					goto err_exit;
				HxSTD_MemCopy(pstInfo->pusServiceId, pstSourceInfo->pusServiceId, ulSize);
			}
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		freeCableTsDivisionSystemInfo(pstDest->pstInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


//TerrestrialDeliverySystem
static void initTerrestrialDeliverySystemDes(SIxTerrestrialDeliverySystemDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxTerrestrialDeliverySystemDes_t));
	pstDes->pusFrequency = NULL;
}

static SIxTerrestrialDeliverySystemDes_t *allocTerrestrialDeliverySystemDes(void)
{
	return (SIxTerrestrialDeliverySystemDes_t *)silib_desc_alloc(sizeof(SIxTerrestrialDeliverySystemDes_t));
}

static void freeTerrestrialDeliverySystemDes(SIxTerrestrialDeliverySystemDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pusFrequency);
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseTerrestrialDeliverySystemDes(HUINT8 *p, SIxTerrestrialDeliverySystemDes_t *pstDes)
{
	HUINT8			ucDesLen;
	HINT32			i;
	HUINT32			ucNumFreq;

	// p[0];
	ucDesLen = p[1];

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->usAreaCode = ((p[2] << 4) & 0xFF0) | ((p[3] >> 4) & 0x0F);
	pstDes->ucGuardInterval = (p[3] >> 2) & 0x03;
	pstDes->ucTransmissionMode = p[3] & 0x03;
	ucNumFreq = (ucDesLen - 2) / 2;
	if((ucDesLen - 2)%2)
	{
		HxLOG_Error("Terrestrial Delivery Systerm Descriptor size erro.(%d)\n", __LINE__);
		return eSIxERR_FAIL;
	}
	pstDes->ucNumFreq = ucNumFreq;
	if(ucNumFreq > 0)
	{
		pstDes->pusFrequency = (HUINT16*)silib_desc_alloc(sizeof(HUINT16) * ucNumFreq);
		if (NULL == pstDes->pusFrequency)
			goto err_exit;

		HxSTD_MemSet(pstDes->pusFrequency, 0, (sizeof(HUINT16) * ucNumFreq));
		p += 4;
		for(i=0;i<ucNumFreq;i++)
		{
			pstDes->pusFrequency[i] = ((p[0] << 8) & 0xFF00) | (p[1] & 0x00FF);
			p += 2;
		}
	}

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pstDes->pusFrequency);
	return eSIxERR_FAIL;
}

static SIxError_e copyTerrestrialDeliverySystemDes(SIxTerrestrialDeliverySystemDes_t *pstDest, const SIxTerrestrialDeliverySystemDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTerrestrialDeliverySystemDes_t));
		pstDest->pusFrequency = NULL;
		pstDest->ucNumFreq = 0;

		if (0 < pstSource->ucNumFreq && NULL != pstSource->pusFrequency)
		{
			pstDest->pusFrequency = (HUINT16 *)silib_desc_alloc(sizeof(HUINT16)*pstSource->ucNumFreq);
			if (pstDest->pusFrequency == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pusFrequency, pstSource->pusFrequency, sizeof(HUINT16)*pstSource->ucNumFreq);
			pstDest->ucNumFreq = pstSource->ucNumFreq;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pusFrequency);
		pstDest->pusFrequency = NULL;
	}
	return eSIxERR_FAIL;
}


//PartialReception
static void initPartialReceptionDes(SIxPartialReceptionDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxPartialReceptionDes_t));
	pstDes->pusServiceId = NULL;
}

static SIxPartialReceptionDes_t *allocPartialReceptionDes(void)
{
	return (SIxPartialReceptionDes_t *)silib_desc_alloc(sizeof(SIxPartialReceptionDes_t));
}

static void freePartialReceptionDes(SIxPartialReceptionDes_t *pstDes)
{
	silib_desc_free(pstDes->pusServiceId);
	silib_desc_free(pstDes);
}

static SIxError_e parsePartialReceptionDes(HUINT8 *p, SIxPartialReceptionDes_t *pstDes)
{
	HUINT8			ucDesLen;
	HINT32			i;

	// p[0];
	ucDesLen = p[1];

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucNumLoops = p[1]/2;
	p += 2;
	if(pstDes->ucNumLoops > 0)
	{
		pstDes->pusServiceId = (HUINT16 *)silib_desc_alloc(sizeof(HUINT16) * pstDes->ucNumLoops);
		if (NULL == pstDes->pusServiceId)
			goto err_exit;
		HxSTD_MemSet(pstDes->pusServiceId, 0, (sizeof(HUINT16) * pstDes->ucNumLoops));
		for(i=0;i<pstDes->ucNumLoops;i++)
		{
			pstDes->pusServiceId[i] = (p[0] << 8) | p[1];
			p += 2;
		}
	}

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pstDes->pusServiceId);
	return eSIxERR_FAIL;
}

static SIxError_e copyPartialReceptionDes(SIxPartialReceptionDes_t *pstDest, const SIxPartialReceptionDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxPartialReceptionDes_t));
		pstDest->ucNumLoops = 0;
		pstDest->pusServiceId = NULL;

		if (0 < pstSource->ucNumLoops && NULL != pstSource->pusServiceId)
		{
			HUINT32 ulSize;
			ulSize = sizeof(HUINT16)*pstSource->ucNumLoops;
			pstDest->pusServiceId = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pusServiceId)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pusServiceId, pstSource->pusServiceId, ulSize);
			pstDest->ucNumLoops = pstSource->ucNumLoops;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pusServiceId);
		pstDest->pusServiceId = NULL;
	}
	return eSIxERR_FAIL;
}


//EmergencyInformation
static void initEmergencyInformationDes(SIxEmergencyInformationDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxEmergencyInformationDes_t));
}

static SIxEmergencyInformationDes_t *allocEmergencyInformationDes(void)
{
	return (SIxEmergencyInformationDes_t *)silib_desc_alloc(sizeof(SIxEmergencyInformationDes_t));
}

static void freeEmergencyInformationInfo(SIxEmergencyInformationDes_t *pstDes)
{
	if (pstDes)
	{
		HUINT32	i;
		silib_desc_free(pstDes->pusServiceId);
		pstDes->pusServiceId = NULL;
		silib_desc_free(pstDes->pbStartEndFlag);
		pstDes->pbStartEndFlag = NULL;
		silib_desc_free(pstDes->pbSignalLevel);
		pstDes->pbSignalLevel = NULL;
		silib_desc_free(pstDes->pucAreaCodeLen);
		pstDes->pucAreaCodeLen = NULL;
		for (i=0; i <pstDes->ucNumServices; i++)
		{
			silib_desc_free(pstDes->pausAreaCode[i]);
		}
		silib_desc_free(pstDes->pausAreaCode);
		pstDes->pausAreaCode = NULL;
	}
}

static void freeEmergencyInformationDes(SIxEmergencyInformationDes_t *pstDes)
{
	freeEmergencyInformationInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseEmergencyInformationDes(HUINT8 *p, SIxEmergencyInformationDes_t *pstDes)
{
	HUINT8				 	 ucDesLen;
	HINT32					 i, j;
	HUINT8					*temp;
	HUINT8					 ucNumService = 0, tempLen;

	// p[0];
	ucDesLen = p[1];

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	temp = p;
	temp += 2;
	while(ucDesLen > 0)
	{
		tempLen = temp[3];
		ucDesLen = ucDesLen - 4 - tempLen;
		temp = temp + 4 + tempLen;
		ucNumService++;
	}

	if(ucNumService > 0)
	{
		pstDes->pusServiceId = (HUINT16*)silib_desc_alloc(sizeof(HUINT16) * ucNumService);
		if (NULL == pstDes->pusServiceId)
			goto err_exit;
		HxSTD_MemSet(pstDes->pusServiceId, 0, (sizeof(HUINT16) * ucNumService));

		pstDes->pbStartEndFlag = (HBOOL*)silib_desc_alloc(sizeof(HBOOL) * ucNumService);
		if (NULL == pstDes->pbStartEndFlag)
			goto err_exit;
		HxSTD_MemSet(pstDes->pbStartEndFlag, 0, (sizeof(HBOOL) * ucNumService));

		pstDes->pbSignalLevel = (HBOOL*)silib_desc_alloc(sizeof(HBOOL) * ucNumService);
		if (NULL == pstDes->pbSignalLevel)
			goto err_exit;
		HxSTD_MemSet(pstDes->pbSignalLevel, 0, (sizeof(HBOOL) * ucNumService));

		pstDes->pucAreaCodeLen = (HUINT8*)silib_desc_alloc(sizeof(HUINT8) * ucNumService);
		if (NULL == pstDes->pucAreaCodeLen)
			goto err_exit;
		HxSTD_MemSet(pstDes->pucAreaCodeLen, 0, (sizeof(HBOOL) * ucNumService));

		pstDes->ucNumServices = ucNumService;
		p += 2;
		pstDes->pausAreaCode = (HUINT16 **)silib_desc_alloc(sizeof(HUINT16 *) * ucNumService);
		if (NULL == pstDes->pausAreaCode)
			goto err_exit;
		HxSTD_MemSet(pstDes->pausAreaCode, 0, (sizeof(HUINT16 *) * ucNumService));

		for(i=0;i<ucNumService;i++)
		{
			pstDes->pusServiceId[i] = (p[0] << 8) | p[1];
			pstDes->pbStartEndFlag[i] = (p[2] & 0x80) >> 7;
			pstDes->pbSignalLevel[i] = (p[2] & 0x40) >> 6;
			pstDes->pucAreaCodeLen[i] = tempLen = p[3]/2;
			pstDes->pausAreaCode[i] = NULL;
			p += 4;
			if(tempLen > 0)
			{
				pstDes->pausAreaCode[i] = (HUINT16 *)silib_desc_alloc(sizeof(HUINT16)*tempLen);
				if (NULL == pstDes->pausAreaCode[i])
					goto err_exit;

				for(j=0;j<tempLen;j++)
				{
					pstDes->pausAreaCode[i][j] = ((p[0] << 4) & 0xFF0) | ((p[1] >> 4) & 0x00F);
					p += 2;
				}
			}
		}
	 }

	return eSIxERR_OK;

err_exit:
	freeEmergencyInformationInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyEmergencyInformationDes(SIxEmergencyInformationDes_t *pstDest, const SIxEmergencyInformationDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32		ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxEmergencyInformationDes_t));
		pstDest->pusServiceId		= NULL;
		pstDest->pbStartEndFlag		= NULL;
		pstDest->pbSignalLevel		= NULL;
		pstDest->pucAreaCodeLen		= NULL;
		pstDest->pausAreaCode		= NULL;

		if (0 == pstSource->ucNumServices)
			goto err_exit;

		if (pstSource->pusServiceId)
		{
			ulSize = sizeof(HUINT16)*pstSource->ucNumServices;
			pstDest->pusServiceId = (HUINT16 *)silib_desc_alloc(ulSize);
			if (pstDest->pusServiceId == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pusServiceId, pstSource->pusServiceId, ulSize);
		}

		if (pstSource->pusServiceId)
		{
			ulSize = sizeof(HUINT16)*pstSource->ucNumServices;
			pstDest->pusServiceId = (HUINT16 *)silib_desc_alloc(ulSize);
			if (pstDest->pusServiceId == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pusServiceId, pstSource->pusServiceId, ulSize);
		}

		if (pstSource->pbStartEndFlag)
		{
			ulSize = sizeof(HBOOL)*pstSource->ucNumServices;
			pstDest->pbStartEndFlag = (HBOOL *)silib_desc_alloc(ulSize);
			if (pstDest->pbStartEndFlag == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pbStartEndFlag, pstSource->pbStartEndFlag, ulSize);
		}

		if (pstSource->pbSignalLevel)
		{
			ulSize = sizeof(HBOOL)*pstSource->ucNumServices;
			pstDest->pbSignalLevel = (HBOOL *)silib_desc_alloc(ulSize);
			if (pstDest->pbSignalLevel == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pbSignalLevel, pstSource->pbSignalLevel, ulSize);
		}

		if (pstSource->pucAreaCodeLen)
		{
			ulSize = sizeof(HUINT8)*pstSource->ucNumServices;
			pstDest->pucAreaCodeLen = (HUINT8 *)silib_desc_alloc(ulSize);
			if (pstDest->pucAreaCodeLen == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucAreaCodeLen, pstSource->pucAreaCodeLen, ulSize);
		}

		if (pstSource->pucAreaCodeLen && pstSource->pausAreaCode)
		{
			HUINT32		i;

			ulSize = sizeof(HUINT16*)*pstSource->ucNumServices;

			pstDest->pausAreaCode = (HUINT16 **)silib_desc_alloc(ulSize);
			if (NULL == pstSource->pausAreaCode)
				goto err_exit;

			HxSTD_MemSet(pstDest->pausAreaCode, 0, ulSize);

			for (i=0 ; i<pstSource->ucNumServices; i++)
			{
				ulSize = sizeof(HUINT16)*pstSource->pucAreaCodeLen[i];
				pstDest->pausAreaCode[i] = silib_desc_alloc(ulSize);
				if (NULL == pstDest->pausAreaCode[i])
					goto err_exit;

				HxSTD_MemCopy(pstDest->pausAreaCode[i], pstSource->pausAreaCode[i], ulSize);
			}
		}

		pstDest->ucNumServices = pstSource->ucNumServices;

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeEmergencyInformationInfo(pstDest);
	return eSIxERR_FAIL;
}


//DataComponent
static void initDataComponentDes(SIxDataComponentDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxDataComponentDes_t));
}

static SIxDataComponentDes_t *allocDataComponentDes(void)
{
	return (SIxDataComponentDes_t *)silib_desc_alloc(sizeof(SIxDataComponentDes_t));
}

static void freeDataComponentInfo(SIxDataComponentDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pstCaptionInfo);
		silib_desc_free(pstDes->pstBxmlInfo);
		pstDes->pstCaptionInfo = NULL;
		pstDes->pstBxmlInfo = NULL;
	}
}

static void freeDataComponentDes(SIxDataComponentDes_t *pstDes)
{
	freeDataComponentInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseDataComponentDes(HUINT8 *p, SIxDataComponentDes_t *pstDes)
{
	HUINT8						 ucDesLen;
	HINT32						 i;
	HUINT8						*temp;
	SIxAdditionalAribBxmlInfo_t	*pstBxmlInfo;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->usDataComponentId = ((p[2] << 8) & 0xFF00) | p[3];
	temp = p += 4;
	if(pstDes->usDataComponentId == 0x0008)	/* caption/super */
	{
		pstDes->ucNumCaptionInfo = ucDesLen - 2;
		if(pstDes->ucNumCaptionInfo > 0)
		{
			pstDes->pstCaptionInfo = (SIxAdditionalAribCaptionInfo_t *)silib_desc_alloc(sizeof(SIxAdditionalAribCaptionInfo_t) * pstDes->ucNumCaptionInfo);
			if (NULL == pstDes->pstCaptionInfo)
				goto err_exit;
			HxSTD_MemSet(pstDes->pstCaptionInfo, 0, (sizeof(SIxAdditionalAribCaptionInfo_t) * pstDes->ucNumCaptionInfo));
			for(i=0;i<pstDes->ucNumCaptionInfo;i++)
			{
				pstDes->pstCaptionInfo[i].ucDMF = (p[i] >> 4) & 0x0F;
				pstDes->pstCaptionInfo[i].ucTiming = p[i] & 0x03;
			}
		}
	}
	else if((pstDes->usDataComponentId == 0x0007) ||	/* for BXML of BS */
		(pstDes->usDataComponentId == 0x000B) ||		/* for CS110 */
		(pstDes->usDataComponentId == 0x000C))		/* for DT */
	{
		pstDes->ucNumBxmlInfo = 1;
		pstDes->pstBxmlInfo = (SIxAdditionalAribBxmlInfo_t *)silib_desc_alloc(sizeof(SIxAdditionalAribBxmlInfo_t));
		if (NULL == pstDes->pstBxmlInfo)
			goto err_exit;
		HxSTD_MemSet(pstDes->pstBxmlInfo, 0, (sizeof(SIxAdditionalAribBxmlInfo_t)));

		pstBxmlInfo = pstDes->pstBxmlInfo;
		pstBxmlInfo->ucTransmissionFormat = (*p >> 6) & 0x03;
		pstBxmlInfo->ucEntryPointFlag = (*p >> 5) & 0x01;
		if(pstBxmlInfo->ucEntryPointFlag == 0x01)
		{
			pstBxmlInfo->ucAutoStartFlag = (*p >> 4) & 0x01;
			pstBxmlInfo->ucDocumentResolution = (*p++) & 0x0F;
			pstBxmlInfo->ucUseXml = (*p >> 7) & 0x01;
			pstBxmlInfo->ucDefaultVersionFlag = (*p >> 6) & 0x01;
			pstBxmlInfo->ucIndependentFlag = (*p >> 5) & 0x01;
			pstBxmlInfo->ucStyleForTvFlag = (*p >> 4) & 0x01;
			p++;
			if(pstBxmlInfo->ucDefaultVersionFlag == 0x00)
			{
				pstBxmlInfo->usBmlMajorVersion = (p[0] << 8) | p[1];
				pstBxmlInfo->usBmlMinorVersion = (p[2] << 8) | p[3];
				p += 4;
				if(pstBxmlInfo->ucUseXml == 0x01)
				{
					pstBxmlInfo->usBxmlMajorVersion = (p[0] << 8) | p[1];
					pstBxmlInfo->usBxmlMinorVersion = (p[2] << 8) | p[3];
					p += 4;
				}
			}
		}
		else
		{
			p++;
		}
		if(pstBxmlInfo->ucTransmissionFormat == 0x00)
		{
			pstBxmlInfo->stCarouselInfo.ucDataEventId = (*p >> 4) & 0x0F;
			pstBxmlInfo->stCarouselInfo.ucEventSectionFlag = (*p >> 3) & 0x01;
			p++;
			pstBxmlInfo->ucOndemandRetrievalFlag = (*p >> 7) & 0x01;
			pstBxmlInfo->ucFileStorableFlag = (*p >> 6) & 0x01;
		}
	}

	return eSIxERR_OK;

err_exit:
	freeDataComponentInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyDataComponentDes(SIxDataComponentDes_t *pstDest, const SIxDataComponentDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32		ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDataComponentDes_t));
		pstDest->ucNumCaptionInfo = 0;
		pstDest->pstCaptionInfo = NULL;
		pstDest->ucNumBxmlInfo = 0;
		pstDest->pstBxmlInfo = NULL;

		if (0 < pstSource->ucNumCaptionInfo && NULL != pstSource->pstCaptionInfo)
		{
			ulSize = sizeof(SIxAdditionalAribCaptionInfo_t)*pstSource->ucNumCaptionInfo;
			pstDest->pstCaptionInfo = (SIxAdditionalAribCaptionInfo_t *)silib_desc_alloc(ulSize);
			if (pstDest->pstCaptionInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pstCaptionInfo, pstSource->pstCaptionInfo, ulSize);
			pstDest->ucNumCaptionInfo = pstSource->ucNumCaptionInfo;
		}

		if (0 < pstSource->ucNumBxmlInfo && NULL != pstSource->pstBxmlInfo)
		{
			ulSize = sizeof(SIxAdditionalAribBxmlInfo_t)*pstSource->ucNumBxmlInfo;
			pstDest->pstBxmlInfo = (SIxAdditionalAribBxmlInfo_t *)silib_desc_alloc(ulSize);
			if (pstDest->pstBxmlInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pstBxmlInfo, pstSource->pstBxmlInfo, ulSize);
			pstDest->ucNumBxmlInfo = pstSource->ucNumBxmlInfo;
		}

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeDataComponentInfo(pstDest);
	return eSIxERR_FAIL;
}


//SystemManagement
static void initSystemManagementDes(SIxSystemManagementDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSystemManagementDes_t));
}

static SIxSystemManagementDes_t *allocSystemManagementDes(void)
{
	return (SIxSystemManagementDes_t *)silib_desc_alloc(sizeof(SIxSystemManagementDes_t));
}

static void freeSystemManagementInfo(SIxSystemManagementDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pucAdditionalIdentificationInfo);
		pstDes->pucAdditionalIdentificationInfo = NULL;
	}
}

static void freeSystemManagementDes(SIxSystemManagementDes_t *pstDes)
{
	freeSystemManagementInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseSystemManagementDes(HUINT8 *p, SIxSystemManagementDes_t *pstDes)
{
	HUINT8				 ucDesLen;
	HINT32				i;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucBroadcastingFlag = (p[2] & 0xC0) >> 6;
	pstDes->ucBroadcastingIdentifier = p[2] & 0x3F;
	pstDes->ucAdditionalBroadcastingIdentification = p[3];
	pstDes->ucAdditionalIdentificationInfoLen = ucDesLen - 2;
	if(pstDes->ucAdditionalIdentificationInfoLen > 0)
	{
		pstDes->pucAdditionalIdentificationInfo = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * pstDes->ucAdditionalIdentificationInfoLen);
		if (NULL == pstDes->pucAdditionalIdentificationInfo)
			goto err_exit;

		HxSTD_MemSet(pstDes->pucAdditionalIdentificationInfo, 0, (sizeof(HUINT8) * pstDes->ucAdditionalIdentificationInfoLen));
		p += 4;
		for(i=0;i<pstDes->ucAdditionalIdentificationInfoLen;i++)
		{
			pstDes->pucAdditionalIdentificationInfo[i] = *p++;
		}
	}

	return eSIxERR_OK;

err_exit:
	freeSystemManagementInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copySystemManagementDes(SIxSystemManagementDes_t *pstDest, const SIxSystemManagementDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSystemManagementDes_t));
		pstDest->ucAdditionalIdentificationInfoLen = 0;
		pstDest->pucAdditionalIdentificationInfo = NULL;

		if (0 < pstSource->ucAdditionalIdentificationInfoLen && NULL != pstSource->pucAdditionalIdentificationInfo)
		{
			HUINT32 ulSize = sizeof(HUINT8)*pstSource->ucAdditionalIdentificationInfoLen;

			pstDest->pucAdditionalIdentificationInfo = (HUINT8 *)silib_desc_alloc(ulSize);
			if (NULL == pstDest->pucAdditionalIdentificationInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucAdditionalIdentificationInfo, pstSource->pucAdditionalIdentificationInfo, ulSize);
			pstDest->ucAdditionalIdentificationInfoLen = pstSource->ucAdditionalIdentificationInfoLen;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeSystemManagementInfo(pstDest);
	return eSIxERR_FAIL;
}

//Series
static void initSeriesDes(SIxSeriesDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSeriesDes_t));
}

static SIxSeriesDes_t *allocSeriesDes(void)
{
	return (SIxSeriesDes_t *)silib_desc_alloc(sizeof(SIxSeriesDes_t));
}

static void freeSeriesInfo(SIxSeriesDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pucSeriesNameChar);
		pstDes->pucSeriesNameChar = NULL;
	}
}

static void freeSeriesDes(SIxSeriesDes_t *pstDes)
{
	freeSeriesInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseSeriesDes(HUINT8 *p, SIxSeriesDes_t *pstDes)
{
	HUINT8		 ucDesLen;
	HUINT8		*pszText = NULL;
	HUINT32		 nTempTextLen;

	//  p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->usSeriesId = (p[2] << 8) | p[3];
	pstDes->ucRepeatLabel = (p[4] & 0xF0) >> 4;
	pstDes->ucProgramPattern = (p[4] & 0x0E) >> 1;
	pstDes->ucExpireDateValidFlag = p[4] & 0x01;
	pstDes->ucExpireDate = (p[5] << 8) | p[6];
	pstDes->ucEpisodeNumber = (p[7]<<4)|((p[8]&0xF0)>>4);
	pstDes->usLastEpisodeNumber = ((p[8]&0x0F)<<8)|p[9];
	pstDes->usSeriesNameCharLen = ucDesLen = ucDesLen - 8;

	nTempTextLen = silib_desc_util_textToUTF8(NULL, pstDes->stHead.stParam.eTextEnc, ucDesLen, (const HUINT8 *)p, &pszText);
	if (pszText != NULL)
	{
		pstDes->pucSeriesNameChar = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * (nTempTextLen + 1));
		if (NULL == pstDes->pucSeriesNameChar)
			goto err_exit;

		HxSTD_MemSet(pstDes->pucSeriesNameChar, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
		nTempTextLen = silib_desc_util_dvbStrlen(pszText);
		HxSTD_MemCopy(pstDes->pucSeriesNameChar, pszText, nTempTextLen);
		pstDes->pucSeriesNameChar[nTempTextLen] = ARIB_DES_EOS;
		silib_desc_free(pszText);
	}

	return eSIxERR_OK;

err_exit:
	freeSeriesInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copySeriesDes(SIxSeriesDes_t *pstDest, const SIxSeriesDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSeriesDes_t));
		pstDest->usSeriesNameCharLen = 0;
		pstDest->pucSeriesNameChar = NULL;

		if (NULL != pstSource->pucSeriesNameChar)
		{
			HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSource->pucSeriesNameChar);

			pstDest->pucSeriesNameChar = (HUINT8 *)silib_desc_alloc(ulSize+1);
			if (pstDest->pucSeriesNameChar == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucSeriesNameChar, pstSource->pucSeriesNameChar, ulSize);
			pstDest->pucSeriesNameChar[ulSize] = ARIB_DES_EOS;
			pstDest->usSeriesNameCharLen = ulSize;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeSeriesInfo(pstDest);
	return eSIxERR_FAIL;
}


//EventGroup
static void initEventGroupDes(SIxEventGroupDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxEventGroupDes_t));
}

static SIxEventGroupDes_t *allocEventGroupDes(void)
{
	return (SIxEventGroupDes_t *)silib_desc_alloc(sizeof(SIxEventGroupDes_t));
}

static void freeEventGroupInfo(SIxEventGroupDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pusServiceId);
		silib_desc_free(pstDes->pusEventId);
		silib_desc_free(pstDes->pstOtherNetworkInfo);
		silib_desc_free(pstDes->pucDataByte);
		pstDes->pusServiceId			= NULL;
		pstDes->pusEventId				= NULL;
		pstDes->pstOtherNetworkInfo		= NULL;
		pstDes->pucDataByte				= NULL;
	}
}

static void freeEventGroupDesList(SIxEventGroupDes_t *pstDes)
{
	SIxEventGroupDes_t			*pstDesList, *pstDesToFree;

	pstDesList = pstDes;
	while (pstDesList)
	{
		pstDesToFree = pstDesList;
		pstDesList = pstDesList->pstNext;

		freeEventGroupInfo(pstDesToFree);
		silib_desc_free(pstDesToFree);
	}
}

static void freeEventGroupDes(SIxEventGroupDes_t *pstDes)
{
	freeEventGroupDesList(pstDes);
}

static SIxError_e parseEventGroupDes(HUINT8 *p, SIxEventGroupDes_t *pstDes)
{
	HUINT8			 ucDesLen;
	HINT32			 i;
	HUINT8			*pTemp;
	HUINT8			 ucTempLen;
	HUINT32			 ulSize;
	SIxAribEventGroupOtherNetworkInfo_t *pstOtherNetInfo;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucGroupType = (p[2] & 0xF0) >> 4;
	pstDes->ucEventCount = (p[2] & 0x0F);
	if(pstDes->ucEventCount > 0)
	{
		ulSize = sizeof(HINT32) * pstDes->ucEventCount;
		pstDes->pusServiceId = (HUINT16*)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pusServiceId)
			goto err_exit;

		HxSTD_MemSet(pstDes->pusServiceId, 0, ulSize);
		pstDes->pusEventId = (HINT32*)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pusEventId)
			goto err_exit;

		HxSTD_MemSet(pstDes->pusEventId, 0, ulSize);

		pTemp = (p + 3);
		for(i=0;i<pstDes->ucEventCount;i++)
		{
			pstDes->pusServiceId[i] = HxMACRO_Get16Bit(pTemp);
			pTemp += 2;
			pstDes->pusEventId[i] = HxMACRO_Get16Bit(pTemp);
			pTemp += 2;
		}
	}

	ucTempLen = ucDesLen - 1 - (pstDes->ucEventCount * 4);
	if(ucTempLen > 0)
	{
		ulSize = sizeof(SIxAribEventGroupOtherNetworkInfo_t)*pstDes->ucNumLoop;

		if((pstDes->ucGroupType == 0x4) || (pstDes->ucGroupType == 0x05))
		{
			pstDes->ucNumLoop = ucTempLen / 8;
			if((ucTempLen % sizeof(SIxAribEventGroupOtherNetworkInfo_t)) != 0)
			{
				HxLOG_Error("The rest size is strange(right:%d, actual:%d). %d\n", sizeof(SIxAribEventGroupOtherNetworkInfo_t), ucTempLen, __LINE__);
			}

			if(pstDes->ucNumLoop > 0)
			{
				pstDes->pstOtherNetworkInfo = (SIxAribEventGroupOtherNetworkInfo_t *)silib_desc_alloc(ulSize);
				if (NULL == pstDes->pstOtherNetworkInfo)
					goto err_exit;

				HxSTD_MemSet(pstDes->pstOtherNetworkInfo, 0, ulSize);
				pTemp = (p + 3 + (pstDes->ucEventCount * 4));
				for(i=0;i<pstDes->ucNumLoop;i++)
				{
					pstOtherNetInfo = &(pstDes->pstOtherNetworkInfo[i]);
					pstOtherNetInfo->usOrigNetworkId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
					pstOtherNetInfo->usTransStreamId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
					pstOtherNetInfo->usServiceId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
					pstOtherNetInfo->nEventId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
				}
			}
		}
		else
		{
			pstDes->ucPrivateDataByteLen = ucTempLen;
			pstDes->pucDataByte = (HUINT8 *)silib_desc_alloc(ucTempLen);
			pTemp = (p + 3 + (pstDes->ucEventCount * 4));
			for(i=0;i<ucTempLen;i++)
			{
				pstDes->pucDataByte[i] = *pTemp++;
			}
		}
	}
	return eSIxERR_OK;

err_exit:
	freeEventGroupInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyEventGroupInfo(SIxEventGroupDes_t *pstDest, const SIxEventGroupDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32 ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxEventGroupDes_t));
		pstDest->pusServiceId = NULL;
		pstDest->pusEventId = NULL;
		pstDest->pstOtherNetworkInfo = NULL;
		pstDest->pucDataByte = NULL;
		pstDest->pstNext = NULL;

		if (pstSource->ucEventCount)
		{
			ulSize = sizeof(HINT32)*pstSource->ucEventCount;
			pstDest->pusServiceId	= (HUINT16 *)silib_desc_alloc(ulSize);
			if (NULL == pstDest->pusServiceId)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pusServiceId, pstSource->pusServiceId, ulSize);

			pstDest->pusEventId		= (HINT32 *)silib_desc_alloc(ulSize);
			if (NULL == pstDest->pusEventId)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pusEventId, pstSource->pusEventId, ulSize);
		}

		if (pstSource->pstOtherNetworkInfo)
		{
			pstDest->pstOtherNetworkInfo = silib_desc_alloc(sizeof(SIxAribEventGroupOtherNetworkInfo_t));
			if (NULL == pstDest->pstOtherNetworkInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pstOtherNetworkInfo, pstSource->pstOtherNetworkInfo, sizeof(SIxAribEventGroupOtherNetworkInfo_t));
		}

		if (0 < pstSource->ucPrivateDataByteLen && NULL != pstSource->pucDataByte)
		{
			ulSize = sizeof(HUINT8) * pstSource->ucPrivateDataByteLen;
			pstDest->pucDataByte = (HUINT8 *)silib_desc_alloc(ulSize);
			if (NULL == pstDest->pucDataByte)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucDataByte, pstSource->pucDataByte, ulSize);
			pstDest->ucPrivateDataByteLen = pstSource->ucPrivateDataByteLen;
		}
	}

	return eErr;

err_exit:
	freeEventGroupInfo(pstDest);
	pstDest->pusServiceId			= NULL;
	pstDest->pusEventId				= NULL;
	pstDest->pstOtherNetworkInfo	= NULL;
	pstDest->pucDataByte			= NULL;
	return eSIxERR_FAIL;
}

static SIxError_e copyEventGroupDes(SIxEventGroupDes_t *pstDest, const SIxEventGroupDes_t *pstSource)
{
	SIxError_e			 eErr = eSIxERR_FAIL;
	SIxEventGroupDes_t	*pstDesList = NULL;

	if (pstSource && pstDest)
	{
		SIxEventGroupDes_t	*pstSourceDes, *pstNewDes = NULL;

		if (copyEventGroupInfo(pstDest, pstSource) != eSIxERR_OK)
			goto err_exit;

		pstDesList = pstDest;
		pstSourceDes = pstSource->pstNext;
		while (pstSourceDes)
		{
			pstNewDes = (SIxEventGroupDes_t *)silib_desc_alloc(sizeof(SIxEventGroupDes_t));
			if (pstNewDes == NULL)
				goto err_exit;

			if (copyEventGroupInfo(pstNewDes, pstSourceDes) != eSIxERR_OK)
			{
				silib_desc_free(pstNewDes);
				goto err_exit;
			}

			pstNewDes->pstNext = NULL;

			pstDesList->pstNext = pstNewDes;
			pstDesList = pstDesList->pstNext;

			pstSourceDes = pstSourceDes->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		freeEventGroupInfo(pstDest);
		freeEventGroupDesList(pstDest->pstNext);
	}
	return eSIxERR_FAIL;
}


//SiParameter
static void initSiParameterDes(SIxSiParameterDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSiParameterDes_t));
}

static SIxSiParameterDes_t *allocSiParameterDes(void)
{
	return (SIxSiParameterDes_t *)silib_desc_alloc(sizeof(SIxSiParameterDes_t));
}

static void freeSiParameterInfo(SIxSiParameterDes_t *pstDes)
{
	if (pstDes)
	{
		if (pstDes->pstTableLoop)
		{
			HUINT32	i;

			for (i=0; i<pstDes->ucNumParameters; i++)
			{
				silib_desc_free(pstDes->pstTableLoop[i].pucTableDescriptionByte);
			}
			silib_desc_free(pstDes->pstTableLoop);
			pstDes->pstTableLoop = NULL;
		}
	}
}

static void freeSiParameterDes(SIxSiParameterDes_t *pstDes)
{
	freeSiParameterInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseSiParameterDes(HUINT8 *p, SIxSiParameterDes_t *pstDes)
{
	HUINT8			 ucDesLen;
	HINT32			 i, j;
	HUINT8			 tempLen1, tempLen2 = 0;
	HUINT8			*temp;
	HUINT32			 ulNumParam = 0;
	SIxAribSiParameterDesTableLoop_t *pstTableLoop;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucParameterVersion = p[2];
	pstDes->usUpdateTime = ((p[3] << 8) & 0xFF00) | p[4];

	tempLen1 = ucDesLen - 3;
	temp = p;
	temp += 5;
	while(tempLen1 > 0)
	{
		if(tempLen1 < 2)
		{
			HxLOG_Error("Table description length(%d) of SI parameter Descriptor is not valid.(%d)\n", tempLen1, __LINE__);
			break;
		}
		tempLen2 = temp[1];
		temp = temp + 2 + tempLen2;
		tempLen1 = tempLen1 - 2 -tempLen2;
		ulNumParam++;
	}

	p += 5;
	if(ulNumParam > 0)
	{
		HUINT32		ulSize;
		pstDes->ucNumParameters = ulNumParam;
		ulSize = sizeof(SIxAribSiParameterDesTableLoop_t) * ulNumParam;
		pstDes->pstTableLoop = (SIxAribSiParameterDesTableLoop_t *)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pstTableLoop)
			goto err_exit;

		HxSTD_MemSet(pstDes->pstTableLoop, 0, ulSize);
		pstTableLoop = pstDes->pstTableLoop;
		for(i=0;i<ulNumParam;i++)
		{
			pstTableLoop[i].ucTableId = *p++;
			pstTableLoop[i].ucTableDescriptionLength = *p++;
			ulSize = sizeof(HUINT8) * pstTableLoop[i].ucTableDescriptionLength;
			pstTableLoop[i].pucTableDescriptionByte = (HUINT8 *)silib_desc_alloc(ulSize);
			if (NULL == pstTableLoop[i].pucTableDescriptionByte)
				goto err_exit;

			HxSTD_MemSet(pstTableLoop[i].pucTableDescriptionByte, 0, ulSize);
			for(j=0;j<pstTableLoop[i].ucTableDescriptionLength;j++)
			{
				pstTableLoop[i].pucTableDescriptionByte[j] = *p++;
			}
		}
	}

	return eSIxERR_OK;

err_exit:
	freeSiParameterInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copySiParameterInfo(SIxSiParameterDes_t *pstDest, const SIxSiParameterDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSiParameterDes_t));
		pstDest->pstTableLoop = NULL;
		pstDest->ucNumParameters = 0;

		if (0 < pstSource->ucNumParameters && NULL != pstSource->pstTableLoop)
		{
			HUINT32	i;
			HUINT32	ulSize = sizeof(SIxAribSiParameterDesTableLoop_t)*pstSource->ucNumParameters;
			SIxAribSiParameterDesTableLoop_t	*pstInfo, *pstSourceInfo;

			pstDest->pstTableLoop = silib_desc_alloc(ulSize);
			if (NULL == pstDest->pstTableLoop)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pstTableLoop, pstSource->pstTableLoop, ulSize);
			pstDest->ucNumParameters = pstSource->ucNumParameters;

			for (i=0; i<pstSource->ucNumParameters; i++)
			{
				pstInfo = &pstDest->pstTableLoop[i];
				pstSourceInfo = &pstSource->pstTableLoop[i];

				pstInfo->pucTableDescriptionByte = NULL;
				if (0 < pstSourceInfo->ucTableDescriptionLength && NULL != pstSourceInfo->pucTableDescriptionByte)
				{
					ulSize = sizeof(HUINT8)*pstSourceInfo->ucTableDescriptionLength;
					pstInfo->pucTableDescriptionByte = silib_desc_alloc(ulSize);
					if (NULL == pstInfo->pucTableDescriptionByte)
						goto err_exit;

					HxSTD_MemCopy(pstInfo->pucTableDescriptionByte, pstSourceInfo->pucTableDescriptionByte, ulSize);
					pstInfo->ucTableDescriptionLength = pstSourceInfo->ucTableDescriptionLength;
				}
			}
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeSiParameterInfo(pstDest);
	return eSIxERR_FAIL;
}

static SIxError_e copySiParameterDes(SIxSiParameterDes_t *pstDest, const SIxSiParameterDes_t *pstSource)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		if (copySiParameterInfo(pstDest, pstSource) != eSIxERR_OK)
			goto err_exit;
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeSiParameterInfo(pstDest);
	return eSIxERR_FAIL;
}


//BroadcasterName
static void initBroadcasterNameDes(SIxBroadcasterNameDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxBroadcasterNameDes_t));
}

static SIxBroadcasterNameDes_t *allocBroadcasterNameDes(void)
{
	return (SIxBroadcasterNameDes_t *)silib_desc_alloc(sizeof(SIxBroadcasterNameDes_t));
}

static void freeBroadcasterNameInfo(SIxBroadcasterNameDes_t *pstDes)
{
	if (pstDes)
	{
		silib_desc_free(pstDes->pucBroadcasterName);
		pstDes->pucBroadcasterName = NULL;
	}
}

static void freeBroadcasterNameDes(SIxBroadcasterNameDes_t *pstDes)
{
	freeBroadcasterNameInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseBroadcasterNameDes(HUINT8 *p, SIxBroadcasterNameDes_t *pstDes)
{
	HUINT8			 ucDesLen;
	HUINT8			*pszText = NULL;
	HUINT32			 nTempTextLen;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucBroadcasterNameLen = ucDesLen;
	p += 2;

	nTempTextLen = silib_desc_util_textToUTF8(NULL, pstDes->stHead.stParam.eTextEnc, ucDesLen, (const HUINT8 *)p, &pszText);
	if (pszText != NULL)
	{
		HUINT32	ulSize = sizeof(HUINT8) * nTempTextLen;

		pstDes->pucBroadcasterName = silib_desc_alloc(ulSize + 1);
		if (NULL == pstDes->pucBroadcasterName)
			goto err_exit;

		HxSTD_MemSet(pstDes->pucBroadcasterName, 0, ulSize);
		ulSize = silib_desc_util_dvbStrlen(pszText);
		HxSTD_MemCopy(pstDes->pucBroadcasterName, pszText, ulSize);
		pstDes->pucBroadcasterName[ulSize] = ARIB_DES_EOS;
		silib_desc_free(pszText);
	}

	return eSIxERR_OK;

err_exit:
	freeBroadcasterNameInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyBroadcasterNameInfo(SIxBroadcasterNameDes_t *pstDest, const SIxBroadcasterNameDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxBroadcasterNameDes_t));
		pstDest->pucBroadcasterName = NULL;
		pstDest->ucBroadcasterNameLen = 0;

		if (NULL != pstSource->pucBroadcasterName)
		{
			HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSource->pucBroadcasterName);

			pstDest->pucBroadcasterName = silib_desc_alloc(ulSize+1);
			if (NULL == pstDest->pucBroadcasterName)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pucBroadcasterName, pstSource->pucBroadcasterName, ulSize);
			pstDest->pucBroadcasterName[ulSize] = ARIB_DES_EOS;
			pstDest->ucBroadcasterNameLen = ulSize;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeBroadcasterNameInfo(pstDest);
	return eSIxERR_FAIL;
}

static SIxError_e copyBroadcasterNameDes(SIxBroadcasterNameDes_t *pstDest, const SIxBroadcasterNameDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		eErr = copyBroadcasterNameInfo(pstDest, pstSource);
		if (eSIxERR_OK != eErr)
			goto err_exit;

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeBroadcasterNameInfo(pstDest);
	return eSIxERR_FAIL;
}


//ComponentGroup
static void initComponentGroupDes(SIxComponentGroupDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxComponentGroupDes_t));
}

static SIxComponentGroupDes_t *allocComponentGroupDes(void)
{
	return (SIxComponentGroupDes_t *)silib_desc_alloc(sizeof(SIxComponentGroupDes_t));
}

static void freeComponentGroupInfo(SIxComponentGroupDes_t *pstDes)
{
	if (pstDes)
	{
		if (pstDes->pstGroup)
		{
			if (pstDes->pstGroup->pstCaUnit)
			{
				silib_desc_free(pstDes->pstGroup->pstCaUnit->pucComponentTag);
				silib_desc_free(pstDes->pstGroup->pstCaUnit);
			}
			silib_desc_free(pstDes->pstGroup->pucTextChar);
			silib_desc_free(pstDes->pstGroup);
			pstDes->pstGroup = NULL;
		}
	}
}

static void freeComponentGroupDes(SIxComponentGroupDes_t *pstDes)
{
	freeComponentGroupInfo(pstDes);
	silib_desc_free(pstDes);
}

static SIxError_e parseComponentGroupDes(HUINT8 *p, SIxComponentGroupDes_t *pstDes)
{
	HUINT8			 ucDesLen;
	HINT32			 i, j, k;
	SIxAribComponentGroupDesGroup_t *pstGroup;
	SIxAribComponentGroupDesCaUnit_t	*pstCaUnit;
	HUINT8			*pszText = NULL;
	HUINT32			nTempTextLen;

	//  p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucComponentGroupType = (p[2] & 0xE0) >> 5;
	pstDes->ucTotalBitRateFlag = (p[2]& 0x10) >> 4;
	pstDes->ucNumOfGroup = p[2]&0x0F;
	if(pstDes->ucNumOfGroup > 0)
	{
		HUINT32	ulSize = sizeof(SIxAribComponentGroupDesGroup_t) * pstDes->ucNumOfGroup;
		pstDes->pstGroup = (SIxAribComponentGroupDesGroup_t *)silib_desc_alloc(ulSize);
		if (NULL == pstDes->pstGroup)
			goto err_exit;

		HxSTD_MemSet(pstDes->pstGroup, 0, (ulSize));
		p += 3;
		for(i=0;i<pstDes->ucNumOfGroup;i++)
		{
			pstGroup = &pstDes->pstGroup[i];
			pstGroup->ucComponentGroupId = (*p >> 4) & 0x0F;
			pstGroup->ucNumOfCaUnit = *p & 0x0F;
			p++;
			if(pstGroup->ucNumOfCaUnit > 0)
			{
				ulSize = sizeof(SIxAribComponentGroupDesCaUnit_t) * pstGroup->ucNumOfCaUnit;
				pstGroup->pstCaUnit = (SIxAribComponentGroupDesCaUnit_t *)silib_desc_alloc(ulSize);
				if (NULL == pstGroup->pstCaUnit)
					goto err_exit;

				HxSTD_MemSet(pstGroup->pstCaUnit, 0, ulSize);
				for(j=0;j<pstGroup->ucNumOfCaUnit;j++)
				{
					pstCaUnit = &pstGroup->pstCaUnit[j];
					pstCaUnit->ucCaUnitId = (*p >> 4) & 0x0F;
					pstCaUnit->ucNumOfComponent = *p & 0x0F;
					p++;
					if(pstCaUnit->ucNumOfComponent > 0)
					{
						ulSize = sizeof(HUINT8) * pstCaUnit->ucNumOfComponent;
						pstCaUnit->pucComponentTag = (HUINT8 *)silib_desc_alloc(ulSize);
						if (NULL == pstCaUnit->pucComponentTag)
							goto err_exit;

						HxSTD_MemSet(pstCaUnit->pucComponentTag, 0, ulSize);
						for(k=0;k<pstCaUnit->ucNumOfComponent;k++)
						{
							pstCaUnit->pucComponentTag[k] = *p++;
						}
					}
				}
			}
			if(pstDes->ucTotalBitRateFlag == 0x01)
			{
				pstGroup->ucTotalBitRate = *p++;
			}
			pstGroup->ucTextLen = *p++;
			nTempTextLen = silib_desc_util_textToUTF8(NULL, pstDes->stHead.stParam.eTextEnc, pstGroup->ucTextLen, (const HUINT8 *)p, &pszText);
			if(pszText != NULL)
			{
				ulSize = sizeof(HUINT8) * nTempTextLen;
				pstGroup->pucTextChar = silib_desc_alloc(ulSize+1);
				if (NULL == pstGroup->pucTextChar)
					goto err_exit;

				HxSTD_MemSet(pstGroup->pucTextChar, 0, ulSize + 1);
				ulSize = silib_desc_util_dvbStrlen(pszText);
				HxSTD_MemCopy(pstGroup->pucTextChar, pszText, ulSize);
				pstGroup->pucTextChar[ulSize] = ARIB_DES_EOS;
				silib_desc_free(pszText);
			}
		}
	}

	return eSIxERR_OK;

err_exit:
	freeComponentGroupInfo(pstDes);
	return eSIxERR_FAIL;
}

static SIxError_e copyComponentGroupInfo(SIxComponentGroupDes_t *pstDest, const SIxComponentGroupDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		if (0 < pstSource->ucNumOfGroup && NULL != pstSource->pstGroup)
		{
			HUINT32	i, j;
			HUINT32 ulSize = sizeof(SIxAribComponentGroupDesGroup_t)*pstSource->ucNumOfGroup;
			SIxAribComponentGroupDesGroup_t	*pstGroup, *pstSourceGroup;
			pstDest->pstGroup = silib_desc_alloc(ulSize);

			HxSTD_MemCopy(pstDest->pstGroup, pstSource->pstGroup, ulSize);
			pstDest->ucNumOfGroup = pstSource->ucNumOfGroup;

			for (i=0; i<pstSource->ucNumOfGroup; i++)
			{
				pstGroup = &pstDest->pstGroup[i];
				pstSourceGroup = &pstSource->pstGroup[i];

				pstGroup->ucNumOfCaUnit = 0;
				pstGroup->pstCaUnit = NULL;
				pstGroup->ucTextLen = 0;
				pstGroup->pucTextChar = NULL;

				if (0 < pstSourceGroup->ucNumOfCaUnit && NULL != pstSourceGroup->pstCaUnit)
				{
					SIxAribComponentGroupDesCaUnit_t	*pstUnit, *pstSourceUnit;

					ulSize = sizeof(SIxAribComponentGroupDesCaUnit_t)*pstSourceGroup->ucNumOfCaUnit;
					pstGroup->pstCaUnit = silib_desc_alloc(ulSize);
					if (NULL == pstGroup->pstCaUnit)
						goto err_exit;

					HxSTD_MemCopy(pstGroup->pstCaUnit, pstSourceGroup->pstCaUnit, ulSize);

					for (j=0; j<pstSourceGroup->ucNumOfCaUnit; j++)
					{
						pstUnit = &pstGroup->pstCaUnit[j];
						pstSourceUnit = &pstSourceGroup->pstCaUnit[j];
						pstUnit->pucComponentTag = NULL;
						pstUnit->ucNumOfComponent = 0;

						if (0 < pstSourceUnit->ucNumOfComponent && NULL != pstSourceUnit->pucComponentTag)
						{
							ulSize = sizeof(HUINT8)*pstSourceUnit->ucNumOfComponent;
							pstUnit->pucComponentTag = (HUINT8 *)silib_desc_alloc(ulSize);
							if (NULL == pstUnit->pucComponentTag)
								goto err_exit;

							HxSTD_MemCopy(pstUnit->pucComponentTag, pstSourceUnit->pucComponentTag, ulSize);
							pstUnit->ucNumOfComponent = pstSourceUnit->ucNumOfComponent;
						}
					}
				}

				if (pstSource->pstGroup[i].pucTextChar)
				{
					ulSize = silib_desc_util_dvbStrlen(pstSource->pstGroup[i].pucTextChar);
					pstDest->pstGroup[i].pucTextChar = silib_desc_alloc(ulSize+1);
					HxSTD_MemCopy(pstDest->pstGroup[i].pucTextChar, pstSource->pstGroup[i].pucTextChar, ulSize);
					pstDest->pstGroup[i].pucTextChar[ulSize] = ARIB_DES_EOS;
				}
			}

		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeComponentGroupInfo(pstDest);
	return eSIxERR_FAIL;
}


static SIxError_e copyComponentGroupDes(SIxComponentGroupDes_t *pstDest, const SIxComponentGroupDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxComponentGroupDes_t));
		pstDest->pstGroup = NULL;
		pstDest->ucNumOfGroup = 0;

		if (copyComponentGroupInfo(pstDest, pstSource) != eSIxERR_OK)
			goto err_exit;

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	freeComponentGroupInfo(pstDest);
	return eSIxERR_FAIL;
}


//ContentAvailability
static void initContentAvailabilityDes(SIxContentAvailabilityDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxContentAvailabilityDes_t));
}

static SIxContentAvailabilityDes_t *allocContentAvailabilityDes(void)
{
	return (SIxContentAvailabilityDes_t *)silib_desc_alloc(sizeof(SIxContentAvailabilityDes_t));
}

static void freeContentAvailabilityDes(SIxContentAvailabilityDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseContentAvailabilityDes(HUINT8 *p, SIxContentAvailabilityDes_t *pstDes)
{
	HUINT8			 ucDesLen;

	// p[0];			// Tag
	ucDesLen = p[1];	// Length

	if (ucDesLen == 0)
	{
		return eSIxERR_INVALID;
	}

	pstDes->ucCopyRestrictionMode = (p[2] & 0x40) >> 6;
	pstDes->ucImageContraintToken = (p[2] & 0x20) >> 5;
	pstDes->ucRetentionMode = (p[2] & 0x10) >> 4;
	pstDes->ucRetentionState = (p[2] & 0x0E) >>1 ;
	pstDes->ucEncryptionMode = (p[2] & 0x01) ;

	return eSIxERR_OK;
}

static SIxError_e copyContentAvailabilityDes(SIxContentAvailabilityDes_t *pstDest, const SIxContentAvailabilityDes_t *pstSource)
{
	SIxError_e		 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxContentAvailabilityDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

#if 0
eSIxDESCTAG_ARIB_BROADCASTER_ID					= 0x85
eSIxDESCTAG_ARIB_DTCP								= 0x88
eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION 		= 0xC0
eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL				= 0xC1
eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION			= 0xC2
eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME 	= 0xC3
eSIxDESCTAG_ARIB_AUDIO_COMPONENT					= 0xC4
eSIxDESCTAG_ARIB_HYPER_LINK						= 0xC5
eSIxDESCTAG_ARIB_TARGET_AREA						= 0xC6
eSIxDESCTAG_ARIB_DATA_CONTENTS 					= 0xC7
eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL				= 0xC8
eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT					= 0xC9
eSIxDESCTAG_ARIB_CA_EMM_TS 						= 0xCA
eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION			= 0xCB
eSIxDESCTAG_ARIB_CA_SERVICE						= 0xCC
eSIxDESCTAG_ARIB_TS_INFORMATION					= 0xCD
eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER				= 0xCE
eSIxDESCTAG_ARIB_LOGO_TRANSMISSION 				= 0xCF
eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM			= 0xF9
eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM		= 0xFA
eSIxDESCTAG_ARIB_PARTIAL_RECEPTION 				= 0xFB
eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION 			= 0xFC
eSIxDESCTAG_ARIB_DATA_COMPONENT					= 0xFD
eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT 				= 0xFE
eSIxDESCTAG_ARIB_SERIES							= 0xD5
eSIxDESCTAG_ARIB_EVENT_GROUP						= 0xD6
eSIxDESCTAG_ARIB_SI_PARAMETER						= 0xD7
eSIxDESCTAG_ARIB_BROADCASTER_NAME					= 0xD8
eSIxDESCTAG_ARIB_COMPONENT_GROUP					= 0xD9
eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY				= 0xDE
#endif


/************************************************************************
 * eSIxDESCTAG_ARIB_BROADCASTER_ID					(Tag = 0x85)
 ************************************************************************/
//BroadcasterId
static SIxDescriptor_t *silib_desc_allocBroadcasterIdDes(void)
{
	SIxBroadcasterIdDes_t	*pstDes;

	pstDes = allocBroadcasterIdDes();
	if (pstDes)
	{
		initBroadcasterIdDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeBroadcasterIdDes(SIxDescriptor_t *pstDes)
{
	freeBroadcasterIdDes((SIxBroadcasterIdDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseBroadcasterIdDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseBroadcasterIdDes((HUINT8 *)pucRawData, (SIxBroadcasterIdDes_t *)pstDes);
}

static SIxError_e silib_desc_copyBroadcasterIdDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyBroadcasterIdDes((SIxBroadcasterIdDes_t *)pstDest, (SIxBroadcasterIdDes_t *)pstSource);
}

static void silib_desc_printBroadcasterIdDes(const SIxDescriptor_t *pstDes)
{
	//(SIxBroadcasterIdDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_DTCP							(Tag = 0x88)
 ************************************************************************/
//DvhsDTCP
static SIxDescriptor_t *silib_desc_allocDvhsDTCPDes(void)
{
	SIxDvhsDTCPDes_t	*pstDes;

	pstDes = allocDvhsDTCPDes();
	if (pstDes)
	{
		initDvhsDTCPDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeDvhsDTCPDes(SIxDescriptor_t *pstDes)
{
	freeDvhsDTCPDes((SIxDvhsDTCPDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDvhsDTCPDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseDvhsDTCPDes((HUINT8 *)pucRawData, (SIxDvhsDTCPDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDvhsDTCPDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDvhsDTCPDes((SIxDvhsDTCPDes_t *)pstDest, (SIxDvhsDTCPDes_t *)pstSource);
}

static void silib_desc_printDvhsDTCPDes(const SIxDescriptor_t *pstDes)
{
	//(SIxDvhsDTCPDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION 		(Tag = 0xC0)
 ************************************************************************/
//HierarchicalTransmission
static SIxDescriptor_t *silib_desc_allocHierarchicalTransmissionDes(void)
{
	SIxHierarchicalTransmissionDes_t	*pstDes;

	pstDes = allocHierarchicalTransmissionDes();
	if (pstDes)
	{
		initHierarchicalTransmissionDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeHierarchicalTransmissionDes(SIxDescriptor_t *pstDes)
{
	freeHierarchicalTransmissionDes((SIxHierarchicalTransmissionDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseHierarchicalTransmissionDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseHierarchicalTransmissionDes((HUINT8 *)pucRawData, (SIxHierarchicalTransmissionDes_t *)pstDes);
}

static SIxError_e silib_desc_copyHierarchicalTransmissionDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyHierarchicalTransmissionDes((SIxHierarchicalTransmissionDes_t *)pstDest, (SIxHierarchicalTransmissionDes_t *)pstSource);
}

static void silib_desc_printHierarchicalTransmissionDes(const SIxDescriptor_t *pstDes)
{
	//(SIxHierarchicalTransmissionDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL			(Tag = 0xC1)
 ************************************************************************/
//DigitalCopyControl
static SIxDescriptor_t *silib_desc_allocDigitalCopyControlDes(void)
{
	SIxDigitalCopyControlDes_t	*pstDes;

	pstDes = allocDigitalCopyControlDes();
	if (pstDes)
	{
		initDigitalCopyControlDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeDigitalCopyControlDes(SIxDescriptor_t *pstDes)
{
	freeDigitalCopyControlDes((SIxDigitalCopyControlDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDigitalCopyControlDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseDigitalCopyControlDes((HUINT8 *)pucRawData, (SIxDigitalCopyControlDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDigitalCopyControlDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDigitalCopyControlDes((SIxDigitalCopyControlDes_t *)pstDest, (SIxDigitalCopyControlDes_t *)pstSource);
}

static void silib_desc_printDigitalCopyControlDes(const SIxDescriptor_t *pstDes)
{
	//(SIxDigitalCopyControlDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION			(Tag = 0xC2)
 ************************************************************************/
//NetworkIdentification
static SIxDescriptor_t *silib_desc_allocNetworkIdentificationDes(void)
{
	SIxNetworkIdentificationDes_t	*pstDes;

	pstDes = allocNetworkIdentificationDes();
	if (pstDes)
	{
		initNetworkIdentificationDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeNetworkIdentificationDes(SIxDescriptor_t *pstDes)
{
	freeNetworkIdentificationDes((SIxNetworkIdentificationDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseNetworkIdentificationDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseNetworkIdentificationDes((HUINT8 *)pucRawData, (SIxNetworkIdentificationDes_t *)pstDes);
}

static SIxError_e silib_desc_copyNetworkIdentificationDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyNetworkIdentificationDes((SIxNetworkIdentificationDes_t *)pstDest, (SIxNetworkIdentificationDes_t *)pstSource);
}

static void silib_desc_printNetworkIdentificationDes(const SIxDescriptor_t *pstDes)
{
	//(SIxNetworkIdentificationDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME 	(Tag = 0xC3)
 ************************************************************************/
//PartialTsTime
static SIxDescriptor_t *silib_desc_allocPartialTsTimeDes(void)
{
	SIxPartialTsTimeDes_t	*pstDes;

	pstDes = allocPartialTsTimeDes();
	if (pstDes)
	{
		initPartialTsTimeDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freePartialTsTimeDes(SIxDescriptor_t *pstDes)
{
	freePartialTsTimeDes((SIxPartialTsTimeDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parsePartialTsTimeDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parsePartialTsTimeDes((HUINT8 *)pucRawData, (SIxPartialTsTimeDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPartialTsTimeDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPartialTsTimeDes((SIxPartialTsTimeDes_t *)pstDest, (SIxPartialTsTimeDes_t *)pstSource);
}

static void silib_desc_printPartialTsTimeDes(const SIxDescriptor_t *pstDes)
{
	//(SIxPartialTsTimeDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_AUDIO_COMPONENT				(Tag = 0xC4)
 ************************************************************************/
//AudioComponent
static SIxDescriptor_t *silib_desc_allocAudioComponentDes(void)
{
	SIxAudioComponentDes_t	*pstDes;

	pstDes = allocAudioComponentDes();
	if (pstDes)
	{
		initAudioComponentDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeAudioComponentDes(SIxDescriptor_t *pstDes)
{
	freeAudioComponentDes((SIxAudioComponentDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseAudioComponentDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseAudioComponentDes((HUINT8 *)pucRawData, (SIxAudioComponentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAudioComponentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAudioComponentDes((SIxAudioComponentDes_t *)pstDest, (SIxAudioComponentDes_t *)pstSource);
}

static void silib_desc_printAudioComponentDes(const SIxDescriptor_t *pstDes)
{
	//(SIxAudioComponentDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_HYPER_LINK						(Tag = 0xC5)
 ************************************************************************/

/************************************************************************
 * eSIxDESCTAG_ARIB_TARGET_AREA					(Tag = 0xC6)
 ************************************************************************/

/************************************************************************
 * eSIxDESCTAG_ARIB_DATA_CONTENTS 					(Tag = 0xC7)
 ************************************************************************/
//DataContent
static SIxDescriptor_t *silib_desc_allocDataContentDes(void)
{
	SIxDataContentDes_t	*pstDes;

	pstDes = allocDataContentDes();
	if (pstDes)
	{
		initDataContentDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeDataContentDes(SIxDescriptor_t *pstDes)
{
	freeDataContentDes((SIxDataContentDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDataContentDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseDataContentDes((HUINT8 *)pucRawData, (SIxDataContentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDataContentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDataContentDes((SIxDataContentDes_t *)pstDest, (SIxDataContentDes_t *)pstSource);
}

static void silib_desc_printDataContentDes(const SIxDescriptor_t *pstDes)
{
	//(SIxDataContentDes_t *)pstDes;
}


/*************************************************************************
 * eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL			(Tag = 0xC8)
 ************************************************************************/
//VideoDecodeControl
static SIxDescriptor_t *silib_desc_allocVideoDecodeControlDes(void)
{
	SIxVideoDecodeControlDes_t	*pstDes;

	pstDes = allocVideoDecodeControlDes();
	if (pstDes)
	{
		initVideoDecodeControlDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeVideoDecodeControlDes(SIxDescriptor_t *pstDes)
{
	freeVideoDecodeControlDes((SIxVideoDecodeControlDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseVideoDecodeControlDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseVideoDecodeControlDes((HUINT8 *)pucRawData, (SIxVideoDecodeControlDes_t *)pstDes);
}

static SIxError_e silib_desc_copyVideoDecodeControlDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyVideoDecodeControlDes((SIxVideoDecodeControlDes_t *)pstDest, (SIxVideoDecodeControlDes_t *)pstSource);
}

static void silib_desc_printVideoDecodeControlDes(const SIxDescriptor_t *pstDes)
{
	//(SIxVideoDecodeControlDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT				(Tag = 0xC9)
 ************************************************************************/

/************************************************************************
 * eSIxDESCTAG_ARIB_CA_EMM_TS 						(Tag = 0xCA)
 ************************************************************************/
//CaEmmTs
static SIxDescriptor_t *silib_desc_allocCaEmmTsDes(void)
{
	SIxCaEmmTsDes_t	*pstDes;

	pstDes = allocCaEmmTsDes();
	if (pstDes)
	{
		initCaEmmTsDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeCaEmmTsDes(SIxDescriptor_t *pstDes)
{
	freeCaEmmTsDes((SIxCaEmmTsDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCaEmmTsDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseCaEmmTsDes((HUINT8 *)pucRawData, (SIxCaEmmTsDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCaEmmTsDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCaEmmTsDes((SIxCaEmmTsDes_t *)pstDest, (SIxCaEmmTsDes_t *)pstSource);
}

static void silib_desc_printCaEmmTsDes(const SIxDescriptor_t *pstDes)
{
	//(SIxCaEmmTsDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION			(Tag = 0xCB)
 ************************************************************************/
//CaContractInfo
static SIxDescriptor_t *silib_desc_allocCaContractInfoDes(void)
{
	SIxCaContractInfoDes_t	*pstDes;

	pstDes = allocCaContractInfoDes();
	if (pstDes)
	{
		initCaContractInfoDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeCaContractInfoDes(SIxDescriptor_t *pstDes)
{
	freeCaContractInfoDes((SIxCaContractInfoDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCaContractInfoDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseCaContractInfoDes((HUINT8 *)pucRawData, (SIxCaContractInfoDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCaContractInfoDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCaContractInfoDes((SIxCaContractInfoDes_t *)pstDest, (SIxCaContractInfoDes_t *)pstSource);
}

static void silib_desc_printCaContractInfoDes(const SIxDescriptor_t *pstDes)
{
	//(SIxCaContractInfoDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_CA_SERVICE						(Tag = 0xCC)
 ************************************************************************/
//TsInformation
static SIxDescriptor_t *silib_desc_allocTsInformationDes(void)
{
	SIxTsInformationDes_t	*pstDes;

	pstDes = allocTsInformationDes();
	if (pstDes)
	{
		initTsInformationDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeTsInformationDes(SIxDescriptor_t *pstDes)
{
	freeTsInformationDes((SIxTsInformationDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseTsInformationDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseTsInformationDes((HUINT8 *)pucRawData, (SIxTsInformationDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTsInformationDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTsInformationDes((SIxTsInformationDes_t *)pstDest, (SIxTsInformationDes_t *)pstSource);
}

static void silib_desc_printTsInformationDes(const SIxDescriptor_t *pstDes)
{
	//(SIxTsInformationDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_TS_INFORMATION					(Tag = 0xCD)
 ************************************************************************/

/************************************************************************
 * eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER			(Tag = 0xCE)
 ************************************************************************/
//ExtendedBroadcaster
static SIxDescriptor_t *silib_desc_allocExtendedBroadcasterDes(void)
{
	SIxExtendedBroadcasterDes_t	*pstDes;

	pstDes = allocExtendedBroadcasterDes();
	if (pstDes)
	{
		initExtendedBroadcasterDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeExtendedBroadcasterDes(SIxDescriptor_t *pstDes)
{
	freeExtendedBroadcasterDes((SIxExtendedBroadcasterDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseExtendedBroadcasterDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseExtendedBroadcasterDes((HUINT8 *)pucRawData, (SIxExtendedBroadcasterDes_t *)pstDes);
}

static SIxError_e silib_desc_copyExtendedBroadcasterDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyExtendedBroadcasterDes((SIxExtendedBroadcasterDes_t *)pstDest, (SIxExtendedBroadcasterDes_t *)pstSource);
}

static void silib_desc_printExtendedBroadcasterDes(const SIxDescriptor_t *pstDes)
{
	//(SIxExtendedBroadcasterDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_LOGO_TRANSMISSION 				(Tag = 0xCF)
 ************************************************************************/
//LogoTransmission
static SIxDescriptor_t *silib_desc_allocLogoTransmissionDes(void)
{
	SIxLogoTransmissionDes_t	*pstDes;

	pstDes = allocLogoTransmissionDes();
	if (pstDes)
	{
		initLogoTransmissionDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeLogoTransmissionDes(SIxDescriptor_t *pstDes)
{
	freeLogoTransmissionDes((SIxLogoTransmissionDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseLogoTransmissionDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseLogoTransmissionDes((HUINT8 *)pucRawData, (SIxLogoTransmissionDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLogoTransmissionDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLogoTransmissionDes((SIxLogoTransmissionDes_t *)pstDest, (SIxLogoTransmissionDes_t *)pstSource);
}

static void silib_desc_printLogoTransmissionDes(const SIxDescriptor_t *pstDes)
{
	//(SIxLogoTransmissionDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_ARIB_SERIES							(Tag = 0xD5)
 ************************************************************************/
//Series
static SIxDescriptor_t *silib_desc_allocSeriesDes(void)
{
	SIxSeriesDes_t	*pstDes;

	pstDes = allocSeriesDes();
	if (pstDes)
	{
		initSeriesDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSeriesDes(SIxDescriptor_t *pstDes)
{
	freeSeriesDes((SIxSeriesDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSeriesDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSeriesDes((HUINT8 *)pucRawData, (SIxSeriesDes_t *)pstDes);
}

static SIxError_e silib_desc_copySeriesDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySeriesDes((SIxSeriesDes_t *)pstDest, (SIxSeriesDes_t *)pstSource);
}

static void silib_desc_printSeriesDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSeriesDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_EVENT_GROUP					(Tag = 0xD6)
 ************************************************************************/
//EventGroup
static SIxDescriptor_t *silib_desc_allocEventGroupDes(void)
{
	SIxEventGroupDes_t	*pstDes;

	pstDes = allocEventGroupDes();
	if (pstDes)
	{
		initEventGroupDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeEventGroupDes(SIxDescriptor_t *pstDes)
{
	freeEventGroupDes((SIxEventGroupDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseEventGroupDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseEventGroupDes((HUINT8 *)pucRawData, (SIxEventGroupDes_t *)pstDes);
}

static SIxError_e silib_desc_copyEventGroupDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyEventGroupDes((SIxEventGroupDes_t *)pstDest, (SIxEventGroupDes_t *)pstSource);
}

static void silib_desc_printEventGroupDes(const SIxDescriptor_t *pstDes)
{
	//(SIxEventGroupDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_SI_PARAMETER					(Tag = 0xD7)
 ************************************************************************/
//SiParameter
static SIxDescriptor_t *silib_desc_allocSiParameterDes(void)
{
	SIxSiParameterDes_t	*pstDes;

	pstDes = allocSiParameterDes();
	if (pstDes)
	{
		initSiParameterDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSiParameterDes(SIxDescriptor_t *pstDes)
{
	freeSiParameterDes((SIxSiParameterDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSiParameterDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSiParameterDes((HUINT8 *)pucRawData, (SIxSiParameterDes_t *)pstDes);
}

static SIxError_e silib_desc_copySiParameterDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySiParameterDes((SIxSiParameterDes_t *)pstDest, (SIxSiParameterDes_t *)pstSource);
}

static void silib_desc_printSiParameterDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSiParameterDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_BROADCASTER_NAME				(Tag = 0xD8)
 ************************************************************************/
//BroadcasterName
static SIxDescriptor_t *silib_desc_allocBroadcasterNameDes(void)
{
	SIxBroadcasterNameDes_t	*pstDes;

	pstDes = allocBroadcasterNameDes();
	if (pstDes)
	{
		initBroadcasterNameDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeBroadcasterNameDes(SIxDescriptor_t *pstDes)
{
	freeBroadcasterNameDes((SIxBroadcasterNameDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseBroadcasterNameDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseBroadcasterNameDes((HUINT8 *)pucRawData, (SIxBroadcasterNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyBroadcasterNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyBroadcasterNameDes((SIxBroadcasterNameDes_t *)pstDest, (SIxBroadcasterNameDes_t *)pstSource);
}

static void silib_desc_printBroadcasterNameDes(const SIxDescriptor_t *pstDes)
{
	//(SIxBroadcasterNameDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_COMPONENT_GROUP				(Tag = 0xD9)
 ************************************************************************/
//ComponentGroup
static SIxDescriptor_t *silib_desc_allocComponentGroupDes(void)
{
	SIxComponentGroupDes_t	*pstDes;

	pstDes = allocComponentGroupDes();
	if (pstDes)
	{
		initComponentGroupDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeComponentGroupDes(SIxDescriptor_t *pstDes)
{
	freeComponentGroupDes((SIxComponentGroupDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseComponentGroupDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseComponentGroupDes((HUINT8 *)pucRawData, (SIxComponentGroupDes_t *)pstDes);
}

static SIxError_e silib_desc_copyComponentGroupDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyComponentGroupDes((SIxComponentGroupDes_t *)pstDest, (SIxComponentGroupDes_t *)pstSource);
}

static void silib_desc_printComponentGroupDes(const SIxDescriptor_t *pstDes)
{
	//(SIxComponentGroupDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY			(Tag = 0xDE)
 ************************************************************************/
//ContentAvailability
static SIxDescriptor_t *silib_desc_allocContentAvailabilityDes(void)
{
	SIxContentAvailabilityDes_t	*pstDes;

	pstDes = allocContentAvailabilityDes();
	if (pstDes)
	{
		initContentAvailabilityDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeContentAvailabilityDes(SIxDescriptor_t *pstDes)
{
	freeContentAvailabilityDes((SIxContentAvailabilityDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseContentAvailabilityDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseContentAvailabilityDes((HUINT8 *)pucRawData, (SIxContentAvailabilityDes_t *)pstDes);
}

static SIxError_e silib_desc_copyContentAvailabilityDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyContentAvailabilityDes((SIxContentAvailabilityDes_t *)pstDest, (SIxContentAvailabilityDes_t *)pstSource);
}

static void silib_desc_printContentAvailabilityDes(const SIxDescriptor_t *pstDes)
{
	//(SIxContentAvailabilityDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM		(Tag = 0xF9)
 ************************************************************************/
//CableTsDivisionSystem
static SIxDescriptor_t *silib_desc_allocCableTsDivisionSystemDes(void)
{
	SIxCableTsDivisionSystemDes_t	*pstDes;

	pstDes = allocCableTsDivisionSystemDes();
	if (pstDes)
	{
		initCableTsDivisionSystemDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeCableTsDivisionSystemDes(SIxDescriptor_t *pstDes)
{
	freeCableTsDivisionSystemDes((SIxCableTsDivisionSystemDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCableTsDivisionSystemDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseCableTsDivisionSystemDes((HUINT8 *)pucRawData, (SIxCableTsDivisionSystemDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCableTsDivisionSystemDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCableTsDivisionSystemDes((SIxCableTsDivisionSystemDes_t *)pstDest, (SIxCableTsDivisionSystemDes_t *)pstSource);
}

static void silib_desc_printCableTsDivisionSystemDes(const SIxDescriptor_t *pstDes)
{
	//(SIxCableTsDivisionSystemDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM	(Tag = 0xFA)
 ************************************************************************/
//TerrestrialDeliverySystem
static SIxDescriptor_t *silib_desc_allocTerrestrialDeliverySystemDes(void)
{
	SIxTerrestrialDeliverySystemDes_t	*pstDes;

	pstDes = allocTerrestrialDeliverySystemDes();
	if (pstDes)
	{
		initTerrestrialDeliverySystemDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeTerrestrialDeliverySystemDes(SIxDescriptor_t *pstDes)
{
	freeTerrestrialDeliverySystemDes((SIxTerrestrialDeliverySystemDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseTerrestrialDeliverySystemDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseTerrestrialDeliverySystemDes((HUINT8 *)pucRawData, (SIxTerrestrialDeliverySystemDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTerrestrialDeliverySystemDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTerrestrialDeliverySystemDes((SIxTerrestrialDeliverySystemDes_t *)pstDest, (SIxTerrestrialDeliverySystemDes_t *)pstSource);
}

static void silib_desc_printTerrestrialDeliverySystemDes(const SIxDescriptor_t *pstDes)
{
	//(SIxTerrestrialDeliverySystemDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_PARTIAL_RECEPTION 				(Tag = 0xFB)
 ************************************************************************/
//PartialReception
static SIxDescriptor_t *silib_desc_allocPartialReceptionDes(void)
{
	SIxPartialReceptionDes_t	*pstDes;

	pstDes = allocPartialReceptionDes();
	if (pstDes)
	{
		initPartialReceptionDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freePartialReceptionDes(SIxDescriptor_t *pstDes)
{
	freePartialReceptionDes((SIxPartialReceptionDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parsePartialReceptionDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parsePartialReceptionDes((HUINT8 *)pucRawData, (SIxPartialReceptionDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPartialReceptionDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPartialReceptionDes((SIxPartialReceptionDes_t *)pstDest, (SIxPartialReceptionDes_t *)pstSource);
}

static void silib_desc_printPartialReceptionDes(const SIxDescriptor_t *pstDes)
{
	//(SIxPartialReceptionDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION 			(Tag = 0xFC)
 ************************************************************************/
//EmergencyInformation
static SIxDescriptor_t *silib_desc_allocEmergencyInformationDes(void)
{
	SIxEmergencyInformationDes_t	*pstDes;

	pstDes = allocEmergencyInformationDes();
	if (pstDes)
	{
		initEmergencyInformationDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeEmergencyInformationDes(SIxDescriptor_t *pstDes)
{
	freeEmergencyInformationDes((SIxEmergencyInformationDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseEmergencyInformationDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseEmergencyInformationDes((HUINT8 *)pucRawData, (SIxEmergencyInformationDes_t *)pstDes);
}

static SIxError_e silib_desc_copyEmergencyInformationDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyEmergencyInformationDes((SIxEmergencyInformationDes_t *)pstDest, (SIxEmergencyInformationDes_t *)pstSource);
}

static void silib_desc_printEmergencyInformationDes(const SIxDescriptor_t *pstDes)
{
	//(SIxEmergencyInformationDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_DATA_COMPONENT					(Tag = 0xFD)
 ************************************************************************/
//DataComponent
static SIxDescriptor_t *silib_desc_allocDataComponentDes(void)
{
	SIxDataComponentDes_t	*pstDes;

	pstDes = allocDataComponentDes();
	if (pstDes)
	{
		initDataComponentDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeDataComponentDes(SIxDescriptor_t *pstDes)
{
	freeDataComponentDes((SIxDataComponentDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDataComponentDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseDataComponentDes((HUINT8 *)pucRawData, (SIxDataComponentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDataComponentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDataComponentDes((SIxDataComponentDes_t *)pstDest, (SIxDataComponentDes_t *)pstSource);
}

static void silib_desc_printDataComponentDes(const SIxDescriptor_t *pstDes)
{
	//(SIxDataComponentDes_t *)pstDes;
}


/************************************************************************
 * eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT 				(Tag = 0xFE)
 ************************************************************************/
//SystemManagement
static SIxDescriptor_t *silib_desc_allocSystemManagementDes(void)
{
	SIxSystemManagementDes_t	*pstDes;

	pstDes = allocSystemManagementDes();
	if (pstDes)
	{
		initSystemManagementDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSystemManagementDes(SIxDescriptor_t *pstDes)
{
	freeSystemManagementDes((SIxSystemManagementDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSystemManagementDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSystemManagementDes((HUINT8 *)pucRawData, (SIxSystemManagementDes_t *)pstDes);
}

static SIxError_e silib_desc_copySystemManagementDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySystemManagementDes((SIxSystemManagementDes_t *)pstDest, (SIxSystemManagementDes_t *)pstSource);
}

static void silib_desc_printSystemManagementDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSystemManagementDes_t *)pstDes;
}

/***********************************************************************************************************************
 * Spec APIs
 **********************************************************************************************************************/
SIxDescriptor_t *silib_desc_alloc_Arib(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
	case eSIxDESCTAG_ARIB_BROADCASTER_ID:					return silib_desc_allocBroadcasterIdDes();
	case eSIxDESCTAG_ARIB_DTCP:							return silib_desc_allocDvhsDTCPDes();
	case eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION:		return silib_desc_allocHierarchicalTransmissionDes();
	case eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL:			return silib_desc_allocDigitalCopyControlDes();
	case eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION:			return silib_desc_allocNetworkIdentificationDes();
	case eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME:	return silib_desc_allocPartialTsTimeDes();
	case eSIxDESCTAG_ARIB_AUDIO_COMPONENT:					return silib_desc_allocAudioComponentDes();
	case eSIxDESCTAG_ARIB_HYPER_LINK:						break;
	case eSIxDESCTAG_ARIB_TARGET_AREA:						break;
	case eSIxDESCTAG_ARIB_DATA_CONTENTS:					return silib_desc_allocDataContentDes();
	case eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL:			return silib_desc_allocVideoDecodeControlDes();
	case eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT:				break;
	case eSIxDESCTAG_ARIB_CA_EMM_TS:						return silib_desc_allocCaEmmTsDes();
	case eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION:			return silib_desc_allocCaContractInfoDes();
	case eSIxDESCTAG_ARIB_CA_SERVICE:						return silib_desc_allocTsInformationDes();
	case eSIxDESCTAG_ARIB_TS_INFORMATION:					break;
	case eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER:			return silib_desc_allocExtendedBroadcasterDes();
	case eSIxDESCTAG_ARIB_LOGO_TRANSMISSION:				return silib_desc_allocLogoTransmissionDes();
	case eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM:		return silib_desc_allocCableTsDivisionSystemDes();
	case eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM:		return silib_desc_allocTerrestrialDeliverySystemDes();
	case eSIxDESCTAG_ARIB_PARTIAL_RECEPTION:				return silib_desc_allocPartialReceptionDes();
	case eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION:			return silib_desc_allocEmergencyInformationDes();
	case eSIxDESCTAG_ARIB_DATA_COMPONENT:					return silib_desc_allocDataComponentDes();
	case eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT:				return silib_desc_allocSystemManagementDes();
	case eSIxDESCTAG_ARIB_SERIES:							return silib_desc_allocSeriesDes();
	case eSIxDESCTAG_ARIB_EVENT_GROUP:						return silib_desc_allocEventGroupDes();
	case eSIxDESCTAG_ARIB_SI_PARAMETER:					return silib_desc_allocSiParameterDes();
	case eSIxDESCTAG_ARIB_BROADCASTER_NAME:				return silib_desc_allocBroadcasterNameDes();
	case eSIxDESCTAG_ARIB_COMPONENT_GROUP:					return silib_desc_allocComponentGroupDes();
	case eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY:			return silib_desc_allocContentAvailabilityDes();
	default:											break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Arib(SIxDescriptor_t *pstDes)
{
	SIxError_e		 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_ARIB_BROADCASTER_ID:					eErr = silib_desc_freeBroadcasterIdDes(pstDes);				break;
	case eSIxDESCTAG_ARIB_DTCP:							eErr = silib_desc_freeDvhsDTCPDes(pstDes);					break;
	case eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION:		eErr = silib_desc_freeHierarchicalTransmissionDes(pstDes);	break;
	case eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL:			eErr = silib_desc_freeDigitalCopyControlDes(pstDes);		break;
	case eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION:			eErr = silib_desc_freeNetworkIdentificationDes(pstDes);		break;
	case eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME:	eErr = silib_desc_freePartialTsTimeDes(pstDes);				break;
	case eSIxDESCTAG_ARIB_AUDIO_COMPONENT: 				eErr = silib_desc_freeAudioComponentDes(pstDes);			break;
	case eSIxDESCTAG_ARIB_HYPER_LINK:						break;
	case eSIxDESCTAG_ARIB_TARGET_AREA: 					break;
	case eSIxDESCTAG_ARIB_DATA_CONTENTS:					eErr = silib_desc_freeDataContentDes(pstDes);				break;
	case eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL:			eErr = silib_desc_freeVideoDecodeControlDes(pstDes);		break;
	case eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT:				break;
	case eSIxDESCTAG_ARIB_CA_EMM_TS:						eErr = silib_desc_freeCaEmmTsDes(pstDes);					break;
	case eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION:			eErr = silib_desc_freeCaContractInfoDes(pstDes);			break;
	case eSIxDESCTAG_ARIB_CA_SERVICE:						eErr = silib_desc_freeTsInformationDes(pstDes);				break;
	case eSIxDESCTAG_ARIB_TS_INFORMATION:					break;
	case eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER:			eErr = silib_desc_freeExtendedBroadcasterDes(pstDes);		break;
	case eSIxDESCTAG_ARIB_LOGO_TRANSMISSION:				eErr = silib_desc_freeLogoTransmissionDes(pstDes);			break;
	case eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM:		eErr = silib_desc_freeCableTsDivisionSystemDes(pstDes);		break;
	case eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM: 	eErr = silib_desc_freeTerrestrialDeliverySystemDes(pstDes);	break;
	case eSIxDESCTAG_ARIB_PARTIAL_RECEPTION:				eErr = silib_desc_freePartialReceptionDes(pstDes);			break;
	case eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION:			eErr = silib_desc_freeEmergencyInformationDes(pstDes);		break;
	case eSIxDESCTAG_ARIB_DATA_COMPONENT:					eErr = silib_desc_freeDataComponentDes(pstDes);				break;
	case eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT:				eErr = silib_desc_freeSystemManagementDes(pstDes);			break;
	case eSIxDESCTAG_ARIB_SERIES:							eErr = silib_desc_freeSeriesDes(pstDes);					break;
	case eSIxDESCTAG_ARIB_EVENT_GROUP: 					eErr = silib_desc_freeEventGroupDes(pstDes);				break;
	case eSIxDESCTAG_ARIB_SI_PARAMETER:					eErr = silib_desc_freeSiParameterDes(pstDes);				break;
	case eSIxDESCTAG_ARIB_BROADCASTER_NAME:				eErr = silib_desc_freeBroadcasterNameDes(pstDes);			break;
	case eSIxDESCTAG_ARIB_COMPONENT_GROUP: 				eErr = silib_desc_freeComponentGroupDes(pstDes);			break;
	case eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY:			eErr = silib_desc_freeContentAvailabilityDes(pstDes);		break;
	default:											break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_Arib(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_ARIB_BROADCASTER_ID:					eErr = silib_desc_parseBroadcasterIdDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_DTCP:							eErr = silib_desc_parseDvhsDTCPDes(pucRawData, pstDes);						break;
	case eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION:		eErr = silib_desc_parseHierarchicalTransmissionDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL:			eErr = silib_desc_parseDigitalCopyControlDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION:			eErr = silib_desc_parseNetworkIdentificationDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME:	eErr = silib_desc_parsePartialTsTimeDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_AUDIO_COMPONENT: 				eErr = silib_desc_parseAudioComponentDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_HYPER_LINK:						break;
	case eSIxDESCTAG_ARIB_TARGET_AREA: 					break;
	case eSIxDESCTAG_ARIB_DATA_CONTENTS:					eErr = silib_desc_parseDataContentDes(pucRawData, pstDes);					break;
	case eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL:			eErr = silib_desc_parseVideoDecodeControlDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT:				break;
	case eSIxDESCTAG_ARIB_CA_EMM_TS:						eErr = silib_desc_parseCaEmmTsDes(pucRawData, pstDes);						break;
	case eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION:			eErr = silib_desc_parseCaContractInfoDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_CA_SERVICE:						eErr = silib_desc_parseTsInformationDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_TS_INFORMATION:					break;
	case eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER:			eErr = silib_desc_parseExtendedBroadcasterDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_ARIB_LOGO_TRANSMISSION:				eErr = silib_desc_parseLogoTransmissionDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM:		eErr = silib_desc_parseCableTsDivisionSystemDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM: 	eErr = silib_desc_parseTerrestrialDeliverySystemDes(pucRawData, pstDes);	break;
	case eSIxDESCTAG_ARIB_PARTIAL_RECEPTION:				eErr = silib_desc_parsePartialReceptionDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION:			eErr = silib_desc_parseEmergencyInformationDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_ARIB_DATA_COMPONENT:					eErr = silib_desc_parseDataComponentDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT:				eErr = silib_desc_parseSystemManagementDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_SERIES:							eErr = silib_desc_parseSeriesDes(pucRawData, pstDes);						break;
	case eSIxDESCTAG_ARIB_EVENT_GROUP: 					eErr = silib_desc_parseEventGroupDes(pucRawData, pstDes);					break;
	case eSIxDESCTAG_ARIB_SI_PARAMETER:					eErr = silib_desc_parseSiParameterDes(pucRawData, pstDes);					break;
	case eSIxDESCTAG_ARIB_BROADCASTER_NAME:				eErr = silib_desc_parseBroadcasterNameDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_COMPONENT_GROUP: 				eErr = silib_desc_parseComponentGroupDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY:			eErr = silib_desc_parseContentAvailabilityDes(pucRawData, pstDes);			break;
	default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Arib(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_ARIB_BROADCASTER_ID:					eErr = silib_desc_copyBroadcasterIdDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_ARIB_DTCP:							eErr = silib_desc_copyDvhsDTCPDes(pstDest, pstSource);						break;
	case eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION:		eErr = silib_desc_copyHierarchicalTransmissionDes(pstDest, pstSource);		break;
	case eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL:			eErr = silib_desc_copyDigitalCopyControlDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION:			eErr = silib_desc_copyNetworkIdentificationDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME:	eErr = silib_desc_copyPartialTsTimeDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_ARIB_AUDIO_COMPONENT: 				eErr = silib_desc_copyAudioComponentDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_ARIB_HYPER_LINK:						break;
	case eSIxDESCTAG_ARIB_TARGET_AREA: 					break;
	case eSIxDESCTAG_ARIB_DATA_CONTENTS:					eErr = silib_desc_copyDataContentDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL:			eErr = silib_desc_copyVideoDecodeControlDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT:				break;
	case eSIxDESCTAG_ARIB_CA_EMM_TS:						eErr = silib_desc_copyCaEmmTsDes(pstDest, pstSource);						break;
	case eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION:			eErr = silib_desc_copyCaContractInfoDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_ARIB_CA_SERVICE:						eErr = silib_desc_copyTsInformationDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_ARIB_TS_INFORMATION:					break;
	case eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER:			eErr = silib_desc_copyExtendedBroadcasterDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_ARIB_LOGO_TRANSMISSION:				eErr = silib_desc_copyLogoTransmissionDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM:		eErr = silib_desc_copyCableTsDivisionSystemDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM: 	eErr = silib_desc_copyTerrestrialDeliverySystemDes(pstDest, pstSource);		break;
	case eSIxDESCTAG_ARIB_PARTIAL_RECEPTION:				eErr = silib_desc_copyPartialReceptionDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION:			eErr = silib_desc_copyEmergencyInformationDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_ARIB_DATA_COMPONENT:					eErr = silib_desc_copyDataComponentDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT:				eErr = silib_desc_copySystemManagementDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_ARIB_SERIES:							eErr = silib_desc_copySeriesDes(pstDest, pstSource);						break;
	case eSIxDESCTAG_ARIB_EVENT_GROUP: 					eErr = silib_desc_copyEventGroupDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_ARIB_SI_PARAMETER:					eErr = silib_desc_copySiParameterDes(pstDest, pstSource);					break;
	case eSIxDESCTAG_ARIB_BROADCASTER_NAME:				eErr = silib_desc_copyBroadcasterNameDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_ARIB_COMPONENT_GROUP: 				eErr = silib_desc_copyComponentGroupDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY:			eErr = silib_desc_copyContentAvailabilityDes(pstDest, pstSource);			break;
	default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Arib(const SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Arib(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
	case eSIxDESCTAG_ARIB_BROADCASTER_ID:					silib_desc_printBroadcasterIdDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_DTCP:							silib_desc_printDvhsDTCPDes(pstDes);					eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION:		silib_desc_printHierarchicalTransmissionDes(pstDes);	eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL:			silib_desc_printDigitalCopyControlDes(pstDes);			eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION:			silib_desc_printNetworkIdentificationDes(pstDes);		eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME:	silib_desc_printPartialTsTimeDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_AUDIO_COMPONENT: 				silib_desc_printAudioComponentDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_HYPER_LINK:						break;
	case eSIxDESCTAG_ARIB_TARGET_AREA: 					break;
	case eSIxDESCTAG_ARIB_DATA_CONTENTS:					silib_desc_printDataContentDes(pstDes);					eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL:			silib_desc_printVideoDecodeControlDes(pstDes);			eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT:				break;
	case eSIxDESCTAG_ARIB_CA_EMM_TS:						silib_desc_printCaEmmTsDes(pstDes);						eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION:			silib_desc_printCaContractInfoDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_CA_SERVICE:						silib_desc_printTsInformationDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_TS_INFORMATION:					break;
	case eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER:			silib_desc_printExtendedBroadcasterDes(pstDes);			eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_LOGO_TRANSMISSION:				silib_desc_printLogoTransmissionDes(pstDes);			eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM:		silib_desc_printCableTsDivisionSystemDes(pstDes);		eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM: 	silib_desc_printTerrestrialDeliverySystemDes(pstDes);	eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_PARTIAL_RECEPTION:				silib_desc_printPartialReceptionDes(pstDes);			eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION:			silib_desc_printEmergencyInformationDes(pstDes);		eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_DATA_COMPONENT:					silib_desc_printDataComponentDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT:				silib_desc_printSystemManagementDes(pstDes);			eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_SERIES:							silib_desc_printSeriesDes(pstDes);						eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_EVENT_GROUP: 					silib_desc_printEventGroupDes(pstDes);					eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_SI_PARAMETER:					silib_desc_printSiParameterDes(pstDes);					eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_BROADCASTER_NAME:				silib_desc_printBroadcasterNameDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_COMPONENT_GROUP: 				silib_desc_printComponentGroupDes(pstDes);				eErr = eSIxERR_OK;		break;
	case eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY:			silib_desc_printContentAvailabilityDes(pstDes);			eErr = eSIxERR_OK;		break;
	default:											break;
	}

	return eErr;
}

