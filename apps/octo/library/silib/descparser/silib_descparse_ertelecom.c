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
	@file     sxdescparse_ertelecom.c
	@brief    sxdescparse_ertelecom.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static HCHAR *silib_desc_getTagString_Ertelecom(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
 		ENUM_TO_STR(eSIxDESCTAG_LOGICAL_CHANNEL);

		default:
			break;
	}

	return NULL;
}

#define _eSIxDESCTAG_LOGICAL_CHANNEL_
/************************************************************************
 * eSIxDESCTAG_LOGICAL_CHANNEL (Tag = 0x83)
 ************************************************************************/
static void initLogicalChannelDes(SIxErtelecomLogicalChannelDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxErtelecomLogicalChannelDes_t));
}

static SIxErtelecomLogicalChannelInfo_t *allocLogicalChannelInfo(void)
{
	return (SIxErtelecomLogicalChannelInfo_t *)silib_desc_alloc(sizeof(SIxErtelecomLogicalChannelInfo_t));
}

static SIxErtelecomLogicalChannelDes_t *allocLogicalChannelDes(void)
{
	return (SIxErtelecomLogicalChannelDes_t *)silib_desc_alloc(sizeof(SIxErtelecomLogicalChannelDes_t));
}

static void freeLogicalChannelInfo(SIxErtelecomLogicalChannelInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeLogicalChannelDes(SIxErtelecomLogicalChannelDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxErtelecomLogicalChannelInfo_t, freeLogicalChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseLogicalChannelDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxErtelecomLogicalChannelDes_t *pstDes)
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
		HxLOG_Error("[WARN] Unknown parseLogicalChannelDes Format! \n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0)
	{
		SIxErtelecomLogicalChannelInfo_t 	*pstPrevInfo = NULL;
		SIxErtelecomLogicalChannelInfo_t		*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxErtelecomLogicalChannelInfo_t, pstDes);

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
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxErtelecomLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyLogicalChannelDes(SIxErtelecomLogicalChannelDes_t *pstDest, const SIxErtelecomLogicalChannelDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxErtelecomLogicalChannelDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxErtelecomLogicalChannelInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 单捞鸥 备己. */
				GET_PREV_INFO(pstPrevInfo, SIxErtelecomLogicalChannelInfo_t, pstDest);

				pstNewInfo = allocLogicalChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxErtelecomLogicalChannelInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxErtelecomLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

#define _eSIxDESCTAG_ERTELECOM_DTCP_
/************************************************************************
 * eSIxDESCTAG_ERTELECOM_DTCP							(Tag = 0x88)
 ************************************************************************/
static void initDtcpDes(SIxErtelecomDtcpDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxErtelecomDtcpDes_t));
}

static SIxErtelecomDtcpDataInfo_t *allocDtcpDataInfo(void)
{
	return (SIxErtelecomDtcpDataInfo_t *)silib_desc_alloc(sizeof(SIxErtelecomDtcpDataInfo_t));
}

static SIxErtelecomDtcpDes_t *allocDtcpDes(void)
{
	return (SIxErtelecomDtcpDes_t *)silib_desc_alloc(sizeof(SIxErtelecomDtcpDes_t));
}

static void freeDtcpDataInfo(SIxErtelecomDtcpDataInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeDtcpDes(SIxErtelecomDtcpDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxErtelecomDtcpDataInfo_t, freeDtcpDataInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseDtcpDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxErtelecomDtcpDes_t *pstDes)
{
	HINT16				usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseDtcpDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 2)
	{
		HxLOG_Error("[WARN] Unknown parseDtcpDes Format! \n");
		return eSIxERR_INVALID;
	}

	/** < ca system id (2byte) */
	pstDes->usCaSystemId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -= 2;

	while (usDesLen > 1)
	{
		SIxErtelecomDtcpDataInfo_t 		*pstPrevInfo = NULL;
		SIxErtelecomDtcpDataInfo_t		*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxErtelecomDtcpDataInfo_t, pstDes);

		pstNewInfo = allocDtcpDataInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocDtcpDataInfo() failed..! \n");
			goto err_exit;
		}

		/**
		  **	DTCP private data byte spec

		  	Private_data_byte{
			   Reserved 					1	 bslbf
			   Retention_Move_mode		1	 bslbf
			   Retention_State			3	 bslbf
			   EPN						1	 bslbf
			   DTCP_CCI 					2	 bslbf

			   Reserved 					3	 bslbf
			   DOT						1	 bslbf
			   AST						1	 bslbf
			   Image_Constraint_Token	1	 bslbf
			   APS						2	 bslbf
			}
		**/
		pstNewInfo->ucRetentionMoveMode 	= (HUINT8)((HxMACRO_Get8Bit(p) & 0x40) >> 6);
		pstNewInfo->ucRetentionState		= (HUINT8)((HxMACRO_Get8Bit(p) & 0x38) >> 3);
		pstNewInfo->ucEPN					= (HUINT8)((HxMACRO_Get8Bit(p) & 0x04) >> 2);
		pstNewInfo->ucDTCPCCI				= (HUINT8)(HxMACRO_Get8Bit(p) & 0x03);

		p++;
		usDesLen--;

		pstNewInfo->ucDOT					= (HUINT8)((HxMACRO_Get8Bit(p) & 0x10) >> 4);
		pstNewInfo->ucAST					= (HUINT8)((HxMACRO_Get8Bit(p) & 0x08) >> 3);
		pstNewInfo->ucImageConstraintToken	= (HUINT8)((HxMACRO_Get8Bit(p) & 0x04) >> 2);
		pstNewInfo->ucAPS					= (HUINT8)(HxMACRO_Get8Bit(p) & 0x03);

		p++;
		usDesLen--;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseDtcpDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxErtelecomDtcpDataInfo_t, freeDtcpDataInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyDtcpDes(SIxErtelecomDtcpDes_t *pstDest, const SIxErtelecomDtcpDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxErtelecomDtcpDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxErtelecomDtcpDataInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 单捞鸥 备己. */
				GET_PREV_INFO(pstPrevInfo, SIxErtelecomDtcpDataInfo_t, pstDest);

				pstNewInfo = allocDtcpDataInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxErtelecomDtcpDataInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxErtelecomDtcpDataInfo_t, freeDtcpDataInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}


#define __eSIxDESCTAG_LOGICAL_CHANNEL__

/* eSIxDESCTAG_LOGICAL_CHANNEL				: (0x83)	*/
//Logical channel Des
static SIxDescriptor_t *silib_desc_allocLogicalChannelDes(void)
{
	SIxErtelecomLogicalChannelDes_t	*pstDes;

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
		freeLogicalChannelDes((SIxErtelecomLogicalChannelDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseLogicalChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLogicalChannelDes((HUINT8 *)pucRawData, pstParam, (SIxErtelecomLogicalChannelDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLogicalChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLogicalChannelDes((SIxErtelecomLogicalChannelDes_t *)pstDest, (const SIxErtelecomLogicalChannelDes_t *)pstSource);
}

static void silib_desc_printLogicalChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxErtelecomLogicalChannelDes_t		*pstDesc;

	pstDesc = (SIxErtelecomLogicalChannelDes_t *)pstDes;
}

#define __eSIxDESCTAG_ERTELECOM_DTCP__
/************************************************************************
 * eSIxDESCTAG_ERTELECOM_DTCP							(Tag = 0x88)
 ************************************************************************/
//DvhsDTCP
static SIxDescriptor_t *silib_desc_allocDtcpDes(void)
{
	SIxErtelecomDtcpDes_t	*pstDes;

	pstDes = allocDtcpDes();
	if (pstDes)
	{
		initDtcpDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return (SIxDescriptor_t *)NULL;
}

static SIxError_e silib_desc_freeDtcpDes(SIxDescriptor_t *pstDes)
{
	freeDtcpDes((SIxErtelecomDtcpDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDtcpDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDtcpDes((HUINT8 *)pucRawData, pstParam, (SIxErtelecomDtcpDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDtcpDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDtcpDes((SIxErtelecomDtcpDes_t *)pstDest, (SIxErtelecomDtcpDes_t *)pstSource);
}

static void silib_desc_printDtcpDes(const SIxDescriptor_t *pstDes)
{
	SIxErtelecomDtcpDes_t		*pstDesc;

	pstDesc = (SIxErtelecomDtcpDes_t *)pstDes;
}


#define	___MODULE_APIs___

SIxDescriptor_t *silib_desc_alloc_Ertelecom(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
		case eSIxDESCTAG_LOGICAL_CHANNEL:						return silib_desc_allocLogicalChannelDes();
		case eSIxDESCTAG_ERTELECOM_DTCP:						return silib_desc_allocDtcpDes();
		default:												break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Ertelecom(SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_LOGICAL_CHANNEL:					return silib_desc_freeLogicalChannelDes(pstDes);
		case eSIxDESCTAG_ERTELECOM_DTCP:					return silib_desc_freeDtcpDes(pstDes);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_parse_Ertelecom(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_LOGICAL_CHANNEL:					return silib_desc_parseLogicalChannelDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_ERTELECOM_DTCP:					return silib_desc_parseDtcpDes(pucRawData, pstParam, pstDes);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Ertelecom(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_LOGICAL_CHANNEL:					return silib_desc_copyLogicalChannelDes(pstDest, pstSource);
		case eSIxDESCTAG_ERTELECOM_DTCP:					return silib_desc_copyDtcpDes(pstDest, pstSource);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Ertelecom(const SIxDescriptor_t *pstDes)
{
	SIxError_e		eErr;

//	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Ertelecom(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
		case eSIxDESCTAG_LOGICAL_CHANNEL:				silib_desc_printLogicalChannelDes(pstDes);			eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_ERTELECOM_DTCP:				silib_desc_printDtcpDes(pstDes);			eErr = eSIxERR_OK;	break;
		default:										eErr = eSIxERR_NOT_SUPPORT;	break;
	}

	return eErr;
}

/* end of file */
