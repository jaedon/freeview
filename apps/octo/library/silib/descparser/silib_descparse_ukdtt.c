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
	@file     sxdescparse_ukdtt.c
	@brief    sxdescparse_ukdtt.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/




#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static HCHAR *silib_desc_getTagString_Ukdtt(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
 		ENUM_TO_STR(eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL);
		ENUM_TO_STR(eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST);
		ENUM_TO_STR(eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER);
		ENUM_TO_STR(eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE);
		ENUM_TO_STR(eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL);
		ENUM_TO_STR(eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME);
		ENUM_TO_STR(eSIxDESCTAG_UKDTT_GUIDANCE);

		default:
			break;
	}

	return NULL;
}

#define ______LOGICAL_CHANNEL_DESCRIPTOR______

/************************************************************************
 * eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL (Tag = 0x83)
 ************************************************************************/
static void initLogicalChannelDes(SIxUkdttLogicalChannelDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxUkdttLogicalChannelDes_t));
}

static SIxUkdttLogicalChannelInfo_t *allocLogicalChannelInfo(void)
{
	return (SIxUkdttLogicalChannelInfo_t *)silib_desc_alloc(sizeof(SIxUkdttLogicalChannelInfo_t));
}

static SIxUkdttLogicalChannelDes_t *allocLogicalChannelDes(void)
{
	return (SIxUkdttLogicalChannelDes_t *)silib_desc_alloc(sizeof(SIxUkdttLogicalChannelDes_t));
}

static void freeLogicalChannelInfo(SIxUkdttLogicalChannelInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeLogicalChannelDes(SIxUkdttLogicalChannelDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxUkdttLogicalChannelInfo_t, freeLogicalChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseLogicalChannelDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxUkdttLogicalChannelDes_t *pstDes)
{
	HUINT16				usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseLogicalChannelDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4)
	{
		HxLOG_Error("[WARN] Unknown parseVirtualServiceIdDes Format! \n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0)
	{
		SIxUkdttLogicalChannelInfo_t 	*pstPrevInfo = NULL;
		SIxUkdttLogicalChannelInfo_t		*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxUkdttLogicalChannelInfo_t, pstDes);

		pstNewInfo = allocLogicalChannelInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocLogicalChannelInfo() failed..! \n");
			goto err_exit;
		}

		/** < service id	 */
		pstNewInfo->usServiceId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(service id = 0x%x)\n", pstNewInfo->usServiceId);

		/** < visible service flag	 */
		pstNewInfo->ucVisibleServiceFlag = (*p & 0x80) >> 7;
		HxLOG_Print("\t\t(ucVisibleServiceFlag = 0x%x)\n", pstNewInfo->ucVisibleServiceFlag);

		/** < logical channel number	 */
		pstNewInfo->usLogicalChannelNumber = HxMACRO_Get10Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(usLogicalChannelNumber = 0x%x)\n", pstNewInfo->usLogicalChannelNumber);

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseLogicalChannelDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxUkdttLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyLogicalChannelDes(SIxUkdttLogicalChannelDes_t *pstDest, const SIxUkdttLogicalChannelDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxUkdttLogicalChannelDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxUkdttLogicalChannelInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxUkdttLogicalChannelInfo_t, pstDest);

				pstNewInfo = allocLogicalChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxUkdttLogicalChannelInfo_t));
				pstNewInfo->pstNext = NULL;

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:

	if (pstDest != NULL)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxUkdttLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

/* eSIxDESCTAG_LOGICAL_CHANNEL				: (0x83)	*/
//Logical channel Des
static SIxDescriptor_t *silib_desc_allocLogicalChannelDes(void)
{
	SIxUkdttLogicalChannelDes_t	*pstDes;

	pstDes = allocLogicalChannelDes();
	if (pstDes)
	{
		initLogicalChannelDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeLogicalChannelDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeLogicalChannelDes((SIxUkdttLogicalChannelDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseLogicalChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLogicalChannelDes((HUINT8 *)pucRawData, pstParam, (SIxUkdttLogicalChannelDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLogicalChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLogicalChannelDes((SIxUkdttLogicalChannelDes_t *)pstDest, (const SIxUkdttLogicalChannelDes_t *)pstSource);
}

static void silib_desc_printLogicalChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxUkdttLogicalChannelDes_t		*pstDesc;

	pstDesc = (SIxUkdttLogicalChannelDes_t *)pstDes;
}

#define ______PREFERRED_NAME_LIST_DESCRIPTOR______
/************************************************************************
 * eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE (Tag = 0x84)
 ************************************************************************/

static void initPreferredNameListDes(SIxUkdttPreferredNameListDesc_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxUkdttPreferredNameListDesc_t));
}

static SIxUkdttPreferredNameListDesc_t *allocPreferredNameListDes(void)
{
	return (SIxUkdttPreferredNameListDesc_t *)silib_desc_alloc(sizeof(SIxUkdttPreferredNameListDesc_t));
}

static SIxUkdttPreferredNameListInfo_t *allocPreferredNameListInfo(void)
{
	return (SIxUkdttPreferredNameListInfo_t *)silib_desc_alloc(sizeof(SIxUkdttPreferredNameListInfo_t));
}

static void freePreferredNameListDes(SIxUkdttPreferredNameListDesc_t *pstDes)
{
	// TODO: 구현 필요.
}

static void setPreferredNameInfo(SIxUkdttPreferredNameListDesc_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszGuidance)
{
	// TODO: 구현 필요.
}

static SIxError_e parsePreferredNameListDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxUkdttPreferredNameListDesc_t *pstDes)
{
	// TODO: 구현 필요.

	return eSIxERR_OK;
}

static SIxError_e copyPreferredNameListDes(SIxUkdttPreferredNameListDesc_t *pstDest, const SIxUkdttPreferredNameListDesc_t *pstSource)
{
	// TODO: 구현 필요.
	
	return eSIxERR_OK;
}

static SIxDescriptor_t *silib_desc_allocPreferredNameListDes(void)
{
	SIxUkdttPreferredNameListDesc_t	*pstDes = NULL;

	pstDes = allocPreferredNameListDes();
	if (pstDes)
	{
		initPreferredNameListDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freePreferredNameListDes(SIxDescriptor_t *pstDes)
{
	freePreferredNameListDes((SIxUkdttPreferredNameListDesc_t *)pstDes);
	
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parsePreferredNameListDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parsePreferredNameListDes((HUINT8 *)pucRawData, pstParam, (SIxUkdttPreferredNameListDesc_t *)pstDes);
}

static SIxError_e silib_desc_copyPreferredNameListDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPreferredNameListDes((SIxUkdttPreferredNameListDesc_t *)pstDest, (SIxUkdttPreferredNameListDesc_t *)pstSource);
}

static void silib_desc_printPreferredNameListDes(const SIxDescriptor_t *pstDes)
{
	// TODO: 구현 필요.
}

#define ______PREFERRED_NAME_IDENTIFIER_DESCRIPTOR______
/************************************************************************
 * eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER (Tag = 0x85)
 ************************************************************************/
static void initPreferredNameIdentifierDes(SIxUkdttPrefNameIdenDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxUkdttPrefNameIdenDes_t));
}

static SIxUkdttPrefNameIdenDes_t *allocPreferredNameIdentifierDes(void)
{
	return (SIxUkdttPrefNameIdenDes_t *)silib_desc_alloc(sizeof(SIxUkdttPrefNameIdenDes_t));
}

static void freePreferredNameIdentifierDes(SIxUkdttPrefNameIdenDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes);
}

static SIxError_e parsePreferredNameIdentifierDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam,SIxUkdttPrefNameIdenDes_t *pstDes)
{
	HUINT16				usDesLen;

	// TODO: 확인 필요.

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parsePreferredNameIdentifierDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen == 0)					{ return eSIxERR_INVALID; }
	
	pstDes->ucNameId = *p;
	
	HxLOG_Print("\t\t(nameId = 0x%x)\n", pstDes->ucNameId);
	HxLOG_Info(" parsePreferredNameIdentifierDes () Leaved!");

	return eSIxERR_OK;

}

static SIxError_e copyPreferredNameIdentifierDes(SIxUkdttPrefNameIdenDes_t *pstDest, const SIxUkdttPrefNameIdenDes_t *pstSource)
{
	if (pstSource && pstDest)
	{
		pstDest->ucNameId = pstSource->ucNameId;
	}
	
	return eSIxERR_OK;
}

static SIxDescriptor_t *silib_desc_allocPreferredNameIdentifierDes(void)
{
	SIxUkdttPrefNameIdenDes_t	*pstDes = NULL;

	pstDes = allocPreferredNameIdentifierDes();
	if (pstDes)
	{
		initPreferredNameIdentifierDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freePreferredNameIdentifierDes(SIxDescriptor_t *pstDes)
{
	freePreferredNameIdentifierDes((SIxUkdttPrefNameIdenDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parsePreferredNameIdentifierDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parsePreferredNameIdentifierDes((HUINT8 *)pucRawData, pstParam, (SIxUkdttPrefNameIdenDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPreferredNameIdentifierDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPreferredNameIdentifierDes((SIxUkdttPrefNameIdenDes_t *)pstDest, (SIxUkdttPrefNameIdenDes_t *)pstSource);
}

static void silib_desc_printPreferredNameIdentifierDes(const SIxDescriptor_t *pstDes)
{

}

#define ______SERVICE_ATTRIBUTE_DESCRIPTOR______

/************************************************************************
 * eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE (Tag = 0x86)
 ************************************************************************/
static void initServiceAttributeDes(SIxUkdttSvcAttrDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxUkdttSvcAttrDes_t));
}

static SIxUkdttSvcAttrInfo_t *allocServiceAttributeInfo(void)
{
	return (SIxUkdttSvcAttrInfo_t *)silib_desc_alloc(sizeof(SIxUkdttSvcAttrInfo_t));
}

static SIxUkdttSvcAttrDes_t *allocServiceAttributeDes(void)
{
	return (SIxUkdttSvcAttrDes_t *)silib_desc_alloc(sizeof(SIxUkdttSvcAttrDes_t));
}

static void freeServiceAttributeInfo(SIxUkdttSvcAttrInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeServiceAttributeDes(SIxUkdttSvcAttrDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxUkdttSvcAttrInfo_t, freeServiceAttributeInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseServiceAttributeDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxUkdttSvcAttrDes_t *pstDes)
{
	HUINT16				usDesLen;
	HUINT32				ulInfoNum;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseServiceAttributeDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 3)
	{
		HxLOG_Error("[WARN] Unknown parseServiceAttributeDes Format! \n");
		return eSIxERR_INVALID;
	}

	
	// 1. count the total service attribute info :
	// service attribute info size : 3 bytes
	ulInfoNum = (HUINT16)usDesLen / 3;
	if(ulInfoNum == 0)
	{
		HxLOG_Error("[WARN] Unknown parseVirtualServiceIdDes Format! \n");
		return eSIxERR_INVALID;

	}

	while (usDesLen > 0)
	{
		SIxUkdttSvcAttrInfo_t 	*pstPrevInfo = NULL;
		SIxUkdttSvcAttrInfo_t	*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxUkdttSvcAttrInfo_t, pstDes);

		pstNewInfo = allocServiceAttributeInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocServiceAttributeInfo() failed..! \n");
			goto err_exit;
		}

		/** < service id	 */
		pstNewInfo->usServiceId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(service id = 0x%x)\n", pstNewInfo->usServiceId);

		/** < nemeric_selection_flag (1bit)	 */
		pstNewInfo->bNumericSelectFlag = (HBOOL)((*p & 0x02) ? TRUE : FALSE);
		HxLOG_Print("\t\t(bNumericSelectFlag = 0x%x)\n", pstNewInfo->bNumericSelectFlag);
		/** < visible_service_flag (1bit)	 */
		pstNewInfo->bVisibleSvcFlag= (HBOOL)((*p & 0x01) ? TRUE : FALSE);
		HxLOG_Print("\t\t(bVisibleSvcFlag = 0x%x)\n", pstNewInfo->bVisibleSvcFlag);

		p += 1;
		usDesLen -= 1;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	pstDes->ulNumAttrInfo = ulInfoNum;

	HxLOG_Info(" parseServiceAttributeDes () Leaved\n");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxUkdttSvcAttrInfo_t, freeServiceAttributeInfo);
		pstDes->ulNumAttrInfo=0;
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyServiceAttributeDes(SIxUkdttSvcAttrDes_t *pstDest, const SIxUkdttSvcAttrDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxUkdttSvcAttrDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxUkdttSvcAttrInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxUkdttSvcAttrInfo_t, pstDest);

				pstNewInfo = allocServiceAttributeInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxUkdttSvcAttrInfo_t));
				pstNewInfo->pstNext = NULL;

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:

	if (pstDest != NULL)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxUkdttSvcAttrInfo_t, freeServiceAttributeInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

static SIxDescriptor_t *silib_desc_allocServiceAttributeDes(void)
{
	SIxUkdttSvcAttrDes_t	*pstDes;

	pstDes = allocServiceAttributeDes();
	if (pstDes)
	{
		initServiceAttributeDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeServiceAttributeDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeServiceAttributeDes((SIxUkdttSvcAttrDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseServiceAttributeDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseServiceAttributeDes((HUINT8 *)pucRawData, pstParam, (SIxUkdttSvcAttrDes_t *)pstDes);
}

static SIxError_e silib_desc_copyServiceAttributeDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyServiceAttributeDes((SIxUkdttSvcAttrDes_t *)pstDest, (const SIxUkdttSvcAttrDes_t *)pstSource);
}

static void silib_desc_printServiceAttributeDes(const SIxDescriptor_t *pstDes)
{
	SIxUkdttSvcAttrDes_t		*pstDesc;

	pstDesc = (SIxUkdttSvcAttrDes_t *)pstDes;
}

#define ______HD_SIMULCAST_LCN_DESCRIPTOR______
/************************************************************************
 * eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL (Tag = 0x87)
 ************************************************************************/
static void initHdSimulLogicalChannelDes(SIxUkdttHDSimulcastLcnDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxUkdttHDSimulcastLcnDes_t));
}

static SIxUkdttHDSimulcastLcnDes_t *allocHdSimulLogicalChannelDes(void)
{
	return (SIxUkdttHDSimulcastLcnDes_t *)silib_desc_alloc(sizeof(SIxUkdttHDSimulcastLcnDes_t));
}

static SIxUkdttHDSimulLcnInfo_t *allocHdSimulLogicalChannelInfo(void)
{
	return  (SIxUkdttHDSimulLcnInfo_t *)silib_desc_alloc(sizeof(SIxUkdttHDSimulLcnInfo_t));
}

static void freeHdSimulLogicalChannelInfo(SIxUkdttHDSimulLcnInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeHdSimulLogicalChannelDes(SIxUkdttHDSimulcastLcnDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxUkdttHDSimulLcnInfo_t, freeHdSimulLogicalChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);


}

static SIxError_e parseHdSimulLogicalChannelDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxUkdttHDSimulcastLcnDes_t *pstDes)
{
	HUINT16 					usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseHdSimulLogicalChannelDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4)
	{
		HxLOG_Error("[WARN] Unknown parseHdSimulLogicalChannelDes Format! \n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0)
	{
		SIxUkdttHDSimulLcnInfo_t	*pstPrevInfo = NULL;
		SIxUkdttHDSimulLcnInfo_t	*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxUkdttHDSimulLcnInfo_t, pstDes);

		pstNewInfo = allocHdSimulLogicalChannelInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocLogicalChannelInfo() failed..! \n");
			goto err_exit;
		}


		/** < service id	 */
		pstNewInfo->usSvcId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;

		/** < visible service flag	 */
		pstNewInfo->bVisibleSvcFlag = (*p & 0x80) >> 7;

		/** < logical channel number	 */
		pstNewInfo->usLcnNumber = HxMACRO_Get10Bit(p);
		p += 2;
		usDesLen -= 2;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
		pstDes->ulNumLcn++;
	}

	HxLOG_Info(" parseLogicalChannelDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxUkdttHDSimulLcnInfo_t, freeHdSimulLogicalChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;


}

static SIxError_e copyHdSimulLogicalChannelDes(SIxUkdttHDSimulcastLcnDes_t *pstDest, const SIxUkdttHDSimulcastLcnDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxUkdttHDSimulcastLcnDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxUkdttHDSimulLcnInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxUkdttHDSimulLcnInfo_t, pstDest);

				pstNewInfo = allocHdSimulLogicalChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxUkdttHDSimulLcnInfo_t));
				pstNewInfo->pstNext = NULL;

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:

	if (pstDest != NULL)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxUkdttHDSimulLcnInfo_t, freeHdSimulLogicalChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;

}

static SIxDescriptor_t *silib_desc_allocHdSimulLogicalChannelDes(void)
{
	SIxUkdttHDSimulcastLcnDes_t	*pstDes;

	pstDes = allocHdSimulLogicalChannelDes();
	if (pstDes)
	{
		initHdSimulLogicalChannelDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeHdSimulLogicalChannelDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeHdSimulLogicalChannelDes((SIxUkdttHDSimulcastLcnDes_t *) pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;

}

static SIxError_e silib_desc_parseHdSimulLogicalChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseHdSimulLogicalChannelDes((HUINT8 *)pucRawData, pstParam, (SIxUkdttHDSimulcastLcnDes_t *)pstDes);
}

static SIxError_e silib_desc_copyHdSimulLogicalChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyHdSimulLogicalChannelDes((SIxUkdttHDSimulcastLcnDes_t *)pstDest, (SIxUkdttHDSimulcastLcnDes_t *)pstSource);
}

static void silib_desc_printHdSimulLogicalChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxUkdttHDSimulcastLcnDes_t		*pstDesc;

	pstDesc = (SIxUkdttHDSimulcastLcnDes_t *)pstDes;
}

#define ______SHORT_SERVICE_NAME_DESCRIPTOR______
/************************************************************************
 * eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME (Tag = 0x88)
 ************************************************************************/
static void initShortSvcNameDes(SIxUkdttShortSvcNameDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxUkdttShortSvcNameDes_t));
}

static SIxUkdttShortSvcNameDes_t *allocShortSvcNameDes(void)
{
	return (SIxUkdttShortSvcNameDes_t *)silib_desc_alloc(sizeof(SIxUkdttShortSvcNameDes_t));
}

static void freeShortSvcNameDes(SIxUkdttShortSvcNameDes_t *pstDes)
{
	if (pstDes)
	{
		if(pstDes->pszSvcName !=NULL)
			silib_desc_free(pstDes->pszSvcName);
		
		silib_desc_free(pstDes);
	}
}
static void setShortServiceNameDes(SIxUkdttShortSvcNameDes_t *pstDes, HUINT8 *pszShortSvcName)
{
    if(pszShortSvcName != NULL)
	    HxSTD_MemCopy(pstDes->pszSvcName, pszShortSvcName, silib_desc_util_dvbStrlen(pszShortSvcName));
	pstDes->pszSvcName[silib_desc_util_dvbStrlen(pszShortSvcName)] = '\0';
}

static SIxError_e parseShortSvcNameDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxUkdttShortSvcNameDes_t *pstDes)
{

	HUINT8			*pszShortSvcName = NULL;
	HINT16			usDesLen;
	HUINT32 		nTempShortSvcNameLen;

	HxLOG_Info("parseShortSvcNameDes() Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = (HINT16)*p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen == 0)
	{
		HxLOG_Print("[WARN] Unknown parseShortSvcNameDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < Max allocation limit & allocation */
#ifdef SHORTSVCNAME_DEBUG
	{
		HUINT32 i,j;
		HxLOG_Print("Short Service name(raw data -len %d\)\n",usDesLen);
		HLIB_LOG_Dump(p,usDesLen, 0, FALSE);
		HxLOG_Info("\n\t");
		for ( i = 0; i < usDesLen; i++ ) {
			HxLOG_Print("%c",*(p+i));
			HxLOG_Print("-");
			HxLOG_Print("*");
		}
		HxLOG_Info("\n\n");
	}
#endif
	/** < short service name */
	nTempShortSvcNameLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)usDesLen, (const HUINT8 *)p, &pszShortSvcName);
	p += usDesLen;

	/** < Descriptor 데이타 구성. */
	pstDes->pszSvcName = silib_desc_util_allocStr(nTempShortSvcNameLen+1);
	if (pstDes->pszSvcName == NULL)
		goto err_exit;
	setShortServiceNameDes(pstDes, pszShortSvcName);
	silib_desc_free(pszShortSvcName);

	HxLOG_Info(" parseShortSvcNameDes() Leaved!\n");

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszShortSvcName);
	silib_desc_free(pstDes->pszSvcName);
	return eSIxERR_NOT_ENOUGH_MEMORY;

}

static SIxError_e copyShortSvcNameDes(SIxUkdttShortSvcNameDes_t *pstDest, const SIxUkdttShortSvcNameDes_t *pstSource)
{
	HERROR eErr = eSIxERR_FAIL;
	HUINT32	ulSvcNameSize = 0;

	if(pstDest && pstSource)
	{
		eErr = eSIxERR_FAIL;
		ulSvcNameSize = silib_desc_util_dvbStrlen(pstSource->pszSvcName);
		if(ulSvcNameSize == 0)
		{
			goto err_exit;
		}
		pstDest->pszSvcName = silib_desc_util_allocStr(ulSvcNameSize+1);
		if (pstDest->pszSvcName)
		{
			HxSTD_MemCopy(pstDest->pszSvcName, pstSource->pszSvcName, ulSvcNameSize);
			pstDest->pszSvcName[ulSvcNameSize] = '\0';
			eErr = eSIxERR_OK;
		}
		else
			goto err_exit;
		

		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pszSvcName);
		pstDest->pszSvcName = NULL;
	}
	
	return eSIxERR_FAIL;
	
}

static SIxDescriptor_t *silib_desc_allocShortSvcNameDes(void)
{
	SIxUkdttShortSvcNameDes_t	*pstDes = NULL;

	pstDes = allocShortSvcNameDes();
	if (pstDes)
	{
		initShortSvcNameDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeShortSvcNameDes(SIxDescriptor_t *pstDes)
{
	freeShortSvcNameDes((SIxUkdttShortSvcNameDes_t *)pstDes);
	
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseShortSvcNameDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseShortSvcNameDes((HUINT8 *)pucRawData, pstParam, (SIxUkdttShortSvcNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyShortSvcNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyShortSvcNameDes((SIxUkdttShortSvcNameDes_t *)pstDest, (SIxUkdttShortSvcNameDes_t *)pstSource);
}

static void silib_desc_printShortSvcNameDes(const SIxDescriptor_t *pstDes)
{
	// TODO: 구현 필요.
}

#define ______GUIDANCE_DESCRIPTOR______
/************************************************************************
 * eSIxDESCTAG_UKDTT_GUIDANCE (Tag = 0x89)
 ************************************************************************/
static void initGuidanceDes(SIxUkdttGuidanceDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxUkdttGuidanceDes_t));
	pstDes->ucGuidanceType = 0xFF;
	pstDes->unInfo.pUkDttGuidanceInfo = NULL;
	pstDes->unInfo.pPrivateData = NULL;
}

static SIxUkdttGuidanceDes_t *allocGuidanceDes(void)
{
	return (SIxUkdttGuidanceDes_t *)silib_desc_alloc(sizeof(SIxUkdttGuidanceDes_t));
}

static SIxUkdttGuidanceInfo_t *allocGuidanceInfo(void)
{
	SIxUkdttGuidanceInfo_t *pstInfo;

	pstInfo = (SIxUkdttGuidanceInfo_t *)silib_desc_alloc(sizeof(SIxUkdttGuidanceInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	HxSTD_MemSet(pstInfo->szIso639LangCode, 0, 4);
	pstInfo->pszGuidance = NULL;

	return pstInfo;
}

static void freeGuidanceDes(SIxUkdttGuidanceDes_t *pstDes)
{
	if (pstDes)
	{
		if(NULL!= pstDes->unInfo.pUkDttGuidanceInfo)
		{
			silib_desc_free(pstDes->unInfo.pUkDttGuidanceInfo);
			pstDes->unInfo.pUkDttGuidanceInfo= NULL;
		}
		if( NULL != pstDes->unInfo.pPrivateData)
		{
			silib_desc_free(pstDes->unInfo.pPrivateData);
			pstDes->unInfo.pPrivateData = NULL;
		}
		silib_desc_free(pstDes);
	}

}

static void setGuidanceInfo(SIxUkdttGuidanceInfo_t *pstUkDttGuidanceInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszGuidance, HUINT8 ucGuidanceMode)
{
	HINT32 nStrLen;
		
	if (NULL != pstUkDttGuidanceInfo)
	{

		pstUkDttGuidanceInfo->ucGuidanceMode = ucGuidanceMode;
		HxSTD_MemCopy(pstUkDttGuidanceInfo->szIso639LangCode, pszIso639LangCode, 3);
		pstUkDttGuidanceInfo->szIso639LangCode[3] = '\0';
		nStrLen = silib_desc_util_dvbStrlen(pszGuidance);
		HxSTD_MemCopy(pstUkDttGuidanceInfo->pszGuidance, pszGuidance, nStrLen);
		pstUkDttGuidanceInfo->pszGuidance[nStrLen] = '\0';

	}

}

static SIxError_e parseGuidanceDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxUkdttGuidanceDes_t *pstDes)
{
	
	SIxUkdttGuidanceInfo_t		*pstUkDttGuidanceInfo = NULL;
	SIxUkdttPrivateDataInfo_t	*pNewPrivateDataInfo = NULL;
	HUINT8				ucGuidanceMode;
	HUINT8				 ucDesTag,ucDesLen;
	
	HUINT8				 ucGuidanceType,ucStrLen,ucAssignedGuidanceLen;
	HUINT8				 szIso639LangCode[4],*pszGuidance = NULL;
	HUINT16 				usTmpNameLen;

	HxLOG_Info("parseGuidanceDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	ucDesTag = *p++;

	// descriptor length (1byte)
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor len = 0x%x)\n", ucDesLen);

	if (ucDesLen < 1)					{ return eSIxERR_FAIL; }

	// Guidance Type (2 bit)
	ucGuidanceType = (((HUINT8)HxMACRO_Get8Bit(p)) & 0x3);
	p += 1;
	ucDesLen -= 1;
	pstDes->ucGuidanceType = ucGuidanceType;

	if (0 == ucGuidanceType)
	{
		pstUkDttGuidanceInfo = allocGuidanceInfo();
		if (NULL == pstUkDttGuidanceInfo)
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
		usTmpNameLen = (HUINT16)silib_desc_util_textToUTF8(szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucAssignedGuidanceLen, (const HUINT8 *)p, &pszGuidance);
		if (NULL == pszGuidance)
		{
			silib_desc_free(pstUkDttGuidanceInfo);
			goto err_exit;
		}

		pstUkDttGuidanceInfo->pszGuidance = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * (usTmpNameLen+1));
		if (NULL == pstUkDttGuidanceInfo->pszGuidance)
		{
			silib_desc_free(pstUkDttGuidanceInfo);
			goto err_exit;
		}

		setGuidanceInfo(pstUkDttGuidanceInfo, szIso639LangCode, pszGuidance, 0xff);
		if (pszGuidance)
		{
			silib_desc_free(pszGuidance);
			pszGuidance = NULL;
		}

		pstDes->unInfo.pUkDttGuidanceInfo = pstUkDttGuidanceInfo;
	}
	else if(1 == ucGuidanceType)
	{
		pstUkDttGuidanceInfo = allocGuidanceInfo();
		if (NULL != pstUkDttGuidanceInfo)
		{
			ucGuidanceMode = *p++; 
			ucDesLen--;
			ucGuidanceMode &= 1;

			// ISO 639 language code (3byte)
			HxSTD_MemCopy(szIso639LangCode, p, 3);
			p += 3;
			ucDesLen -= 3;
			ucStrLen = ucDesLen;
			ucAssignedGuidanceLen = (ucStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucStrLen);

			// Guidance string.
			usTmpNameLen = (HINT32)silib_desc_util_textToUTF8( szIso639LangCode,pstDes->stHead.stParam.eTextEnc, (HUINT32) ucAssignedGuidanceLen, (const HUINT8 *)p, &pszGuidance);

			pstUkDttGuidanceInfo->pszGuidance = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8) * (usTmpNameLen+1));
			if (NULL == pstUkDttGuidanceInfo->pszGuidance)
			{
				goto err_exit;
			}

			setGuidanceInfo(pstUkDttGuidanceInfo, szIso639LangCode, pszGuidance, ucGuidanceMode);
			if (pszGuidance)
			{
				silib_desc_free (pszGuidance);
				pszGuidance = NULL;
			}

			pstDes->unInfo.pUkDttGuidanceInfo = pstUkDttGuidanceInfo;

		}
	}
	else
	{
		pNewPrivateDataInfo = (SIxUkdttPrivateDataInfo_t *)silib_desc_alloc (sizeof(SIxUkdttPrivateDataInfo_t));
		if (pNewPrivateDataInfo == NULL)
			goto err_exit;

		pNewPrivateDataInfo->ucPrivateDataLen	= (HUINT8) ucDesLen;
		pNewPrivateDataInfo->pPrivateData = silib_desc_alloc(ucDesLen+1);

		if(pNewPrivateDataInfo->pPrivateData != NULL)
		{
			HxSTD_memcpy(pNewPrivateDataInfo->pPrivateData, p, ucDesLen);
		}

		pstDes->unInfo.pPrivateData = pNewPrivateDataInfo;
	}
	return eSIxERR_OK;

err_exit:
	if (pszGuidance)
	{
		silib_desc_free(pszGuidance);
	}
	return eSIxERR_FAIL;

}


static SIxError_e copyGuidanceDes(SIxUkdttGuidanceDes_t *pstDest, const SIxUkdttGuidanceDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxGuidanceDes_t));
		pstDest->unInfo.pUkDttGuidanceInfo = NULL;
		if (pstSource->unInfo.pUkDttGuidanceInfo)
		{
			pstDest->unInfo.pUkDttGuidanceInfo = allocGuidanceInfo();
			if (NULL == pstDest->unInfo.pUkDttGuidanceInfo)
				goto err_exit;

			HxSTD_MemCopy(pstDest->unInfo.pUkDttGuidanceInfo, pstSource->unInfo.pUkDttGuidanceInfo, sizeof(SIxUkdttGuidanceInfo_t));
			pstDest->unInfo.pUkDttGuidanceInfo->pszGuidance = NULL;

			if (pstSource->unInfo.pUkDttGuidanceInfo->pszGuidance)
			{
				HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSource->unInfo.pUkDttGuidanceInfo->pszGuidance);
				pstDest->unInfo.pUkDttGuidanceInfo->pszGuidance = silib_desc_alloc(ulSize+1);
				if (NULL == pstDest->unInfo.pUkDttGuidanceInfo->pszGuidance)
					goto err_exit;

				HxSTD_MemCopy(pstDest->unInfo.pUkDttGuidanceInfo->pszGuidance, pstSource->unInfo.pUkDttGuidanceInfo->pszGuidance, ulSize);
				pstDest->unInfo.pUkDttGuidanceInfo->pszGuidance[ulSize] = '\0';
			}
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest->unInfo.pUkDttGuidanceInfo)
	{
		silib_desc_free(pstDest->unInfo.pUkDttGuidanceInfo->pszGuidance);
		silib_desc_free(pstDest->unInfo.pUkDttGuidanceInfo);
		pstDest->unInfo.pUkDttGuidanceInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxDescriptor_t *silib_desc_allocGuidanceDes(void)
{
	SIxUkdttGuidanceDes_t	*pstDes = NULL;

	pstDes = allocGuidanceDes();
	if (pstDes)
	{
		initGuidanceDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeGuidanceDes(SIxDescriptor_t *pstDes)
{
	freeGuidanceDes((SIxUkdttGuidanceDes_t *)pstDes);
	
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseGuidanceDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseGuidanceDes((HUINT8 *)pucRawData, pstParam, (SIxUkdttGuidanceDes_t *)pstDes);
}

static SIxError_e silib_desc_copyGuidanceDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyGuidanceDes((SIxUkdttGuidanceDes_t *)pstDest, (SIxUkdttGuidanceDes_t *)pstSource);
}

static void silib_desc_printGuidanceDes(const SIxDescriptor_t *pstDes)
{
	// TODO: 구현 필요.
}

#define	___MODULE_APIs___

SIxDescriptor_t *silib_desc_alloc_Ukdtt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
		case eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL:					return silib_desc_allocLogicalChannelDes();
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST:				return silib_desc_allocPreferredNameListDes();
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER: 			return silib_desc_allocPreferredNameIdentifierDes();
		case eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE:					return silib_desc_allocServiceAttributeDes();
		case eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_allocHdSimulLogicalChannelDes();
		case eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME: 			return silib_desc_allocShortSvcNameDes();
		case eSIxDESCTAG_UKDTT_GUIDANCE:							return silib_desc_allocGuidanceDes();
		default:													break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Ukdtt(SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL:					return silib_desc_freeLogicalChannelDes(pstDes);
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST:				return silib_desc_freePreferredNameListDes(pstDes);
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER: 			return silib_desc_freePreferredNameIdentifierDes(pstDes);
		case eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE:					return silib_desc_freeServiceAttributeDes(pstDes);
		case eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_freeHdSimulLogicalChannelDes(pstDes);
		case eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME: 			return silib_desc_freeShortSvcNameDes(pstDes);
		case eSIxDESCTAG_UKDTT_GUIDANCE:							return silib_desc_freeGuidanceDes(pstDes);
		default:													break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_Ukdtt(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL:					return silib_desc_parseLogicalChannelDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST:				return silib_desc_parsePreferredNameListDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER: 			return silib_desc_parsePreferredNameIdentifierDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE:					return silib_desc_parseServiceAttributeDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_parseHdSimulLogicalChannelDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME: 			return silib_desc_parseShortSvcNameDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_UKDTT_GUIDANCE:							return silib_desc_parseGuidanceDes(pucRawData, pstParam, pstDes);
		default:													break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Ukdtt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL:					return silib_desc_copyLogicalChannelDes(pstDest, pstSource);
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST:				return silib_desc_copyPreferredNameListDes(pstDest, pstSource);
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER: 			return silib_desc_copyPreferredNameIdentifierDes(pstDest, pstSource);
		case eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE:					return silib_desc_copyServiceAttributeDes(pstDest, pstSource);
		case eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_copyHdSimulLogicalChannelDes(pstDest, pstSource);
		case eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME: 			return silib_desc_copyShortSvcNameDes(pstDest, pstSource);
		case eSIxDESCTAG_UKDTT_GUIDANCE:							return silib_desc_copyGuidanceDes(pstDest, pstSource);
		default:													break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Ukdtt(const SIxDescriptor_t *pstDes)
{
	SIxError_e		eErr;

//	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Ukdtt(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
		case eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL:					silib_desc_printLogicalChannelDes(pstDes);		eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST:				 silib_desc_printPreferredNameListDes(pstDes);	eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER: 			 silib_desc_printPreferredNameIdentifierDes(pstDes);eErr = eSIxERR_OK;break;
		case eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE:					silib_desc_printServiceAttributeDes(pstDes);		eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL:		 silib_desc_printHdSimulLogicalChannelDes(pstDes);	eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME: 			 silib_desc_printShortSvcNameDes(pstDes);		eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_UKDTT_GUIDANCE:							 silib_desc_printGuidanceDes(pstDes);			eErr = eSIxERR_OK;	break;
		default:													eErr = eSIxERR_NOT_SUPPORT;	break;
	}

	return eErr;
}


/* end of file */
