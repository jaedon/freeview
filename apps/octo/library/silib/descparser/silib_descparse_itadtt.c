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
	@file     sxdescparse_itadtt.c
	@brief    sxdescparse_itadtt.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static HCHAR *silib_desc_getTagString_Itadtt(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
 		ENUM_TO_STR(eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL);
		ENUM_TO_STR(eSIxDESCTAG_ITADTT_PREFERRED_NAME_LIST);
		ENUM_TO_STR(eSIxDESCTAG_ITADTT_EACEM_STREAM_IDENTIFIER);
		ENUM_TO_STR(eSIxDESCTAG_ITADTT_PREFERRED_NAME_IDENTIFIER);
		ENUM_TO_STR(eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL);

		default:
			break;
	}

	return NULL;
}

#define ______LOGICAL_CHANNEL_DESCRIPTOR______

/************************************************************************
 * eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL (Tag = 0x83)
 ************************************************************************/
static void initLogicalChannelDes(SIxItadttLogicalChannelDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxItadttLogicalChannelDes_t));
}

static SIxItadttLogicalChannelInfo_t *allocLogicalChannelInfo(void)
{
	return (SIxItadttLogicalChannelInfo_t *)silib_desc_alloc(sizeof(SIxItadttLogicalChannelInfo_t));
}

static SIxItadttLogicalChannelDes_t *allocLogicalChannelDes(void)
{
	return (SIxItadttLogicalChannelDes_t *)silib_desc_alloc(sizeof(SIxItadttLogicalChannelDes_t));
}

static void freeLogicalChannelInfo(SIxItadttLogicalChannelInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeLogicalChannelDes(SIxItadttLogicalChannelDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxItadttLogicalChannelInfo_t, freeLogicalChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseLogicalChannelDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxItadttLogicalChannelDes_t *pstDes)
{
	HUINT16				usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseVirtualServiceIdDes () Entered!");

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
		SIxItadttLogicalChannelInfo_t 	*pstPrevInfo = NULL;
		SIxItadttLogicalChannelInfo_t		*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxItadttLogicalChannelInfo_t, pstDes);

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
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxItadttLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyLogicalChannelDes(SIxItadttLogicalChannelDes_t *pstDest, const SIxItadttLogicalChannelDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxItadttLogicalChannelDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxItadttLogicalChannelInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 单捞鸥 备己. */
				GET_PREV_INFO(pstPrevInfo, SIxItadttLogicalChannelInfo_t, pstDest);

				pstNewInfo = allocLogicalChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxItadttLogicalChannelInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxItadttLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

/* eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL				: (0x83)	*/
//Logical channel Des
static SIxDescriptor_t *silib_desc_allocLogicalChannelDes(void)
{
	SIxItadttLogicalChannelDes_t	*pstDes;

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
		freeLogicalChannelDes((SIxItadttLogicalChannelDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseLogicalChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLogicalChannelDes((HUINT8 *)pucRawData, pstParam, (SIxItadttLogicalChannelDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLogicalChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLogicalChannelDes((SIxItadttLogicalChannelDes_t *)pstDest, (const SIxItadttLogicalChannelDes_t *)pstSource);
}

static void silib_desc_printLogicalChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxItadttLogicalChannelDes_t		*pstDesc;

	pstDesc = (SIxItadttLogicalChannelDes_t *)pstDes;
}

#define ______HD_SIMULCAST_LCN_DESCRIPTOR______
/************************************************************************
 * eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL (Tag = 0x87)
 ************************************************************************/
static void initHdSimulLogicalChannelDes(SIxItadttHDSimulcastLcnDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxItadttHDSimulcastLcnDes_t));
}

static SIxItadttHDSimulcastLcnDes_t *allocHdSimulLogicalChannelDes(void)
{
	return (SIxItadttHDSimulcastLcnDes_t *)silib_desc_alloc(sizeof(SIxItadttHDSimulcastLcnDes_t));
}

static SIxItadttHDSimulLcnInfo_t *allocHdSimulLogicalChannelInfo(void)
{
	return	(SIxItadttHDSimulLcnInfo_t *)silib_desc_alloc(sizeof(SIxItadttHDSimulLcnInfo_t));
}

static void freeHdSimulLogicalChannelInfo(SIxItadttHDSimulLcnInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeHdSimulLogicalChannelDes(SIxItadttHDSimulcastLcnDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxItadttHDSimulLcnInfo_t, freeHdSimulLogicalChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);


}

static SIxError_e parseHdSimulLogicalChannelDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxItadttHDSimulcastLcnDes_t *pstDes)
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
		SIxItadttHDSimulLcnInfo_t	*pstPrevInfo = NULL;
		SIxItadttHDSimulLcnInfo_t	*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxItadttHDSimulLcnInfo_t, pstDes);

		pstNewInfo = allocHdSimulLogicalChannelInfo();
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
		pstNewInfo->usLogicalChannelNumber = HxMACRO_Get10Bit(p);
		p += 2;
		usDesLen -= 2;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseLogicalChannelDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxItadttHDSimulLcnInfo_t, freeHdSimulLogicalChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;


}

static SIxError_e copyHdSimulLogicalChannelDes(SIxItadttHDSimulcastLcnDes_t *pstDest, const SIxItadttHDSimulcastLcnDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxItadttHDSimulcastLcnDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxItadttHDSimulLcnInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 单捞鸥 备己. */
				GET_PREV_INFO(pstPrevInfo, SIxItadttHDSimulLcnInfo_t, pstDest);

				pstNewInfo = allocHdSimulLogicalChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxItadttHDSimulLcnInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxItadttHDSimulLcnInfo_t, freeHdSimulLogicalChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;

}

static SIxDescriptor_t *silib_desc_allocHdSimulLogicalChannelDes(void)
{
	SIxItadttHDSimulcastLcnDes_t *pstDes;

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
		freeHdSimulLogicalChannelDes((SIxItadttHDSimulcastLcnDes_t *) pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;

}

static SIxError_e silib_desc_parseHdSimulLogicalChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseHdSimulLogicalChannelDes((HUINT8 *)pucRawData, pstParam, (SIxItadttHDSimulcastLcnDes_t *)pstDes);
}

static SIxError_e silib_desc_copyHdSimulLogicalChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyHdSimulLogicalChannelDes((SIxItadttHDSimulcastLcnDes_t *)pstDest, (SIxItadttHDSimulcastLcnDes_t *)pstSource);
}

static void silib_desc_printHdSimulLogicalChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxItadttHDSimulcastLcnDes_t 	*pstDesc;

	pstDesc = (SIxItadttHDSimulcastLcnDes_t *)pstDes;
}


#define	___MODULE_APIs___

SIxDescriptor_t *silib_desc_alloc_Itadtt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
		case eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL:					return silib_desc_allocLogicalChannelDes();
		case eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_allocHdSimulLogicalChannelDes();
		default:											break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Itadtt(SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL:					return silib_desc_freeLogicalChannelDes(pstDes);
		case eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_freeHdSimulLogicalChannelDes(pstDes);
		default:											break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_Itadtt(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL:					return silib_desc_parseLogicalChannelDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_parseHdSimulLogicalChannelDes(pucRawData, pstParam, pstDes);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Itadtt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL:					return silib_desc_copyLogicalChannelDes(pstDest, pstSource);
		case eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL:		return silib_desc_copyHdSimulLogicalChannelDes(pstDest, pstSource);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Itadtt(const SIxDescriptor_t *pstDes)
{
	SIxError_e		eErr;

//	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Itadtt(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
		case eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL:				silib_desc_printLogicalChannelDes(pstDes);			eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL:	silib_desc_printHdSimulLogicalChannelDes(pstDes);	eErr = eSIxERR_OK;	break;
		default:										eErr = eSIxERR_NOT_SUPPORT;	break;
	}

	return eErr;
}

/* end of file */
