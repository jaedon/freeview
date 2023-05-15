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
	@file     silib_descparse_mbc.c
	@brief    silib_descparse_mbc.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		:
	Remarks		: 															\n

	Copyright (c) 2014 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static HCHAR *silib_desc_getTagString_Mbc(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
 		ENUM_TO_STR(eSIxDESCTAG_MBC_LOGICAL_CHANNEL);
		
		default:
			break;
	}

	return NULL;
}

#define _eSIxDESCTAG_MBC_LOGICAL_CHANNEL_
/************************************************************************
 * eSIxDESCTAG_MBC_LOGICAL_CHANNEL (Tag = 0x83)
 ************************************************************************/
static void initLogicalChannelDes(SIxMbcLogicalChannelDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxMbcLogicalChannelDes_t));
}

static SIxMbcLogicalChannelInfo_t *allocLogicalChannelInfo(void)
{
	return (SIxMbcLogicalChannelInfo_t *)silib_desc_alloc(sizeof(SIxMbcLogicalChannelInfo_t));
}

static SIxMbcLogicalChannelDes_t *allocLogicalChannelDes(void)
{
	return (SIxMbcLogicalChannelDes_t *)silib_desc_alloc(sizeof(SIxMbcLogicalChannelDes_t));
}

static void freeLogicalChannelInfo(SIxMbcLogicalChannelInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeLogicalChannelDes(SIxMbcLogicalChannelDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMbcLogicalChannelInfo_t, freeLogicalChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseLogicalChannelDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMbcLogicalChannelDes_t *pstDes)
{
	HUINT16				usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseLogicalChannelDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Info("\t(tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Info("\t(length = %d)\n", usDesLen);

	if (usDesLen < 4)
	{
		HxLOG_Error("[WARN] Unknown parseLogicalChannelDes Format! \n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0)
	{
		SIxMbcLogicalChannelInfo_t 	*pstPrevInfo = NULL;
		SIxMbcLogicalChannelInfo_t	*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxMbcLogicalChannelInfo_t, pstDes);

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

		/** < visible service flag	 */
		pstNewInfo->ucVisibleServiceFlag = (*p & 0x80) >> 7;

		/** < logical channel number	 */
		pstNewInfo->usLogicalChannelNumber = HxMACRO_Get14Bit(p);
		p += 2;
		usDesLen -= 2;

		HxLOG_Info("\t- service id = 0x%x, ucVisibleServiceFlag = %d, usLogicalChannelNumber = %d)\n"
			, pstNewInfo->usServiceId, pstNewInfo->ucVisibleServiceFlag, pstNewInfo->usLogicalChannelNumber);

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseLogicalChannelDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMbcLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyLogicalChannelDes(SIxMbcLogicalChannelDes_t *pstDest, const SIxMbcLogicalChannelDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMbcLogicalChannelDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxMbcLogicalChannelInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxMbcLogicalChannelInfo_t, pstDest);

				pstNewInfo = allocLogicalChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxMbcLogicalChannelInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxMbcLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}


#define __eSIxDESCTAG_MBC_LOGICAL_CHANNEL__
/************************************************************************
 * eSIxDESCTAG_MBC_LOGICAL_CHANNEL					(Tag = 0x83)
 ************************************************************************/

static SIxDescriptor_t *silib_desc_allocLogicalChannelDes(void)
{
	SIxMbcLogicalChannelDes_t	*pstDes;

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
		freeLogicalChannelDes((SIxMbcLogicalChannelDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseLogicalChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLogicalChannelDes((HUINT8 *)pucRawData, pstParam, (SIxMbcLogicalChannelDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLogicalChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLogicalChannelDes((SIxMbcLogicalChannelDes_t *)pstDest, (const SIxMbcLogicalChannelDes_t *)pstSource);
}

static void silib_desc_printLogicalChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxMbcLogicalChannelDes_t		*pstDesc;

	pstDesc = (SIxMbcLogicalChannelDes_t *)pstDes;
}


#define	___MODULE_APIs___

SIxDescriptor_t *silib_desc_alloc_Mbc(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
		case eSIxDESCTAG_MBC_LOGICAL_CHANNEL:				return silib_desc_allocLogicalChannelDes();
		default:											break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Mbc(SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_MBC_LOGICAL_CHANNEL:				return silib_desc_freeLogicalChannelDes(pstDes);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_parse_Mbc(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_MBC_LOGICAL_CHANNEL:				return silib_desc_parseLogicalChannelDes(pucRawData, pstParam, pstDes);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Mbc(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_MBC_LOGICAL_CHANNEL:				return silib_desc_copyLogicalChannelDes(pstDest, pstSource);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Mbc(const SIxDescriptor_t *pstDes)
{
	SIxError_e		eErr;

//	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	HxLOG_Info("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Mbc(pstDes->stHead.eTag) );
	HxLOG_Info("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
		case eSIxDESCTAG_MBC_LOGICAL_CHANNEL:				silib_desc_printLogicalChannelDes(pstDes);		eErr = eSIxERR_OK;	break;
		default:											eErr = eSIxERR_NOT_SUPPORT;	break;
	}

	return eErr;
}

/* end of file */
