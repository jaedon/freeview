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
	@file     sxdescparse.c
	@brief    sxdescparse.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static void *silib_desc_memAlloc(HLONG ulSize)
{
	return HLIB_STD_MemAlloc(ulSize);
}

static HINT32 silib_desc_memFree(void *p)
{
	if (p) HLIB_STD_MemFree(p);
	return 0;
}

typedef void *(*_malloc)(HLONG);
typedef HINT32 (*_free)(void *);

static _malloc	s_malloc = silib_desc_memAlloc;
static _free	s_free	 = silib_desc_memFree;

void *silib_desc_alloc(HLONG ulSize)
{
	return (ulSize>0) ? (s_malloc)(ulSize) : NULL;
}

HINT32 silib_desc_free(void *p)
{
	if (p) (s_free)(p);
	return 0;
}

static SIxDescriptor_t *silib_desc_allocDes(const SIxSpec_e eSpec, const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, HUINT32 ulRawLen)
{
	SIxDescriptor_t		*pstDes;

	pstDes = NULL;
	switch (eSpec)
	{
	case eSIxSPEC_DVB:			pstDes = silib_desc_alloc_Dvb(eTag, ulRawLen);			break;
	case eSIxSPEC_DVBEXT:		pstDes = silib_desc_alloc_DvbExt(eTag, ulRawLen);		break;
	case eSIxSPEC_SSUUNT:		pstDes = silib_desc_alloc_Unt(eTag, ulRawLen);			break;
	case eSIxSPEC_ARIB:			pstDes = silib_desc_alloc_Arib(eTag, ulRawLen);			break;
	case eSIxSPEC_FSAT:			pstDes = silib_desc_alloc_Fsat(eTag, ulRawLen);			break;
	case eSIxSPEC_SES:			pstDes = silib_desc_alloc_Ses(eTag, ulRawLen);			break;
	case eSIxSPEC_ERTELECOM:	pstDes = silib_desc_alloc_Ertelecom(eTag, ulRawLen);	break;
	case eSIxSPEC_AUSDTT:		pstDes = silib_desc_alloc_Ausdtt(eTag, ulRawLen);		break;
	case eSIxSPEC_ITADTT:		pstDes = silib_desc_alloc_Itadtt(eTag, ulRawLen);		break;

//	case eSIxSPEC_FsatGen2:		pstDes = silib_desc_alloc_Fsat(eTag, ulRawLen);			break;
	case eSIxSPEC_UKDTT:		pstDes = silib_desc_alloc_Ukdtt(eTag, ulRawLen);		break;
//	case eSIxSPEC_SKAPA:		pstDes = silib_desc_alloc_Skapa(eTag, ulRawLen);		break;
	case eSIxSPEC_ASTRA:		break;
	case eSIxSPEC_NORDIG:		pstDes = silib_desc_alloc_Nordig(eTag, ulRawLen);		break;
	case eSIxSPEC_ZIGGO:		pstDes = silib_desc_alloc_Ziggo(eTag, ulRawLen);		break;
	case eSIxSPEC_BOXER:		break;
	case eSIxSPEC_MBC:			pstDes = silib_desc_alloc_Mbc(eTag, ulRawLen);			break;
	default:
		break;
	}

	if (pstDes)
	{
		pstDes->stHead.eSpec		= eSpec;
		pstDes->stHead.eTag			= eTag;
		pstDes->stHead.ulLength		= ulRawLen;
		HxSTD_MemCopy(&(pstDes->stHead.stParam), pstParam, sizeof(SIxDescriptorHeaderParam_t));
	}

	return pstDes;
}

static SIxError_e silib_desc_freeDes(SIxDescriptor_t *pstDes)
{
	SIxError_e		 eErr;
	SIxSpec_e		 eSpec;

	eSpec = pstDes->stHead.eSpec;

	eErr = eSIxERR_FAIL;
	switch (eSpec)
	{
	case eSIxSPEC_DVB:			eErr = silib_desc_free_Dvb(pstDes);			break;
	case eSIxSPEC_DVBEXT: 		eErr = silib_desc_free_DvbExt(pstDes);		break;
	case eSIxSPEC_SSUUNT:		eErr = silib_desc_free_Unt(pstDes); 		break;
	case eSIxSPEC_ARIB:			eErr = silib_desc_free_Arib(pstDes); 		break;
	case eSIxSPEC_FSAT:			eErr = silib_desc_free_Fsat(pstDes); 		break;
	case eSIxSPEC_SES:			eErr = silib_desc_free_Ses(pstDes); 		break;
	case eSIxSPEC_ERTELECOM:	eErr = silib_desc_free_Ertelecom(pstDes); 	break;
	case eSIxSPEC_AUSDTT:		eErr = silib_desc_free_Ausdtt(pstDes); 		break;
	case eSIxSPEC_ITADTT:		eErr = silib_desc_free_Itadtt(pstDes);		break;

//	case eSIxSPEC_FsatGen2:		eErr = silib_desc_free_Fsat(pstDes); 		break;
	case eSIxSPEC_UKDTT:		eErr = silib_desc_free_Ukdtt(pstDes); 		break;
//	case eSIxSPEC_SKAPA: 		eErr = silib_desc_free_Skapa(pstDes);		break;
	case eSIxSPEC_ASTRA:		break;
	case eSIxSPEC_NORDIG:		eErr = silib_desc_free_Nordig(pstDes);		break;
	case eSIxSPEC_ZIGGO: 		eErr = silib_desc_free_Ziggo(pstDes);		break;
	case eSIxSPEC_BOXER:		break;
	case eSIxSPEC_MBC:			eErr = silib_desc_free_Mbc(pstDes);			break;
	default:
		break;
	}

	return eErr;
}


static SIxError_e silib_desc_parseDes(const SIxSpec_e eSpec, const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e	eErr;

	eErr = eSIxERR_FAIL;
	switch (eSpec)
	{
	case eSIxSPEC_DVB:			eErr = silib_desc_parse_Dvb(eTag, pstParam, pucRawData, pstDes);		break;
	case eSIxSPEC_DVBEXT:		eErr = silib_desc_parse_DvbExt(eTag, pucRawData, pstDes);				break;
	case eSIxSPEC_SSUUNT:		eErr = silib_desc_parse_Unt(eTag, pucRawData, pstDes);					break;
	case eSIxSPEC_ARIB:			eErr = silib_desc_parse_Arib(eTag, pucRawData, pstDes);					break;
	case eSIxSPEC_FSAT:			eErr = silib_desc_parse_Fsat(eTag, pucRawData, pstDes);					break;
	case eSIxSPEC_SES:			eErr = silib_desc_parse_Ses(eTag, pstParam, pucRawData, pstDes);		break;
	case eSIxSPEC_ERTELECOM:	eErr = silib_desc_parse_Ertelecom(eTag, pstParam, pucRawData, pstDes);	break;
	case eSIxSPEC_AUSDTT:		eErr = silib_desc_parse_Ausdtt(eTag, pstParam, pucRawData, pstDes);		break;
	case eSIxSPEC_ITADTT:		eErr = silib_desc_parse_Itadtt(eTag, pstParam, pucRawData, pstDes);		break;

//	case eSIxSPEC_FsatGen2:		eErr = silib_desc_parse_Fsat(eTag, pstParam, pucRawData, pstDes);		break;
	case eSIxSPEC_UKDTT:		eErr = silib_desc_parse_Ukdtt(eTag, pstParam, pucRawData, pstDes);		break;
//	case eSIxSPEC_SKAPA:		eErr = silib_desc_parse_Skapa(eTag, pstParam, pucRawData, pstDes);		break;
	case eSIxSPEC_ASTRA:		break;
	case eSIxSPEC_NORDIG:		eErr = silib_desc_parse_Nordig(eTag, pstParam, pucRawData, pstDes);		break;
	case eSIxSPEC_ZIGGO:		eErr = silib_desc_parse_Ziggo(eTag, pstParam, pucRawData, pstDes);		break;
	case eSIxSPEC_BOXER:		break;
	case eSIxSPEC_MBC:			eErr = silib_desc_parse_Mbc(eTag, pstParam, pucRawData, pstDes);		break;
	default:
		break;
	}

	if (eSIxERR_OK == eErr)
	{
		pstDes->stHead.eTag = eTag;
		pstDes->stHead.eSpec = eSpec;
		HxSTD_MemCopy(&(pstDes->stHead.stParam), pstParam, sizeof(SIxDescriptorHeaderParam_t));
		pstDes->stHead.ulLength = (HUINT32)pucRawData[1];
	}

	return eErr;
}

static SIxError_e silib_desc_copyDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e	 eErr;
	SIxSpec_e	 eSpec;

	eSpec = pstSource->stHead.eSpec;

	eErr = eSIxERR_FAIL;
	switch (eSpec)
	{
	case eSIxSPEC_DVB:			eErr = silib_desc_copy_Dvb(pstDest, pstSource);			break;
	case eSIxSPEC_DVBEXT:		eErr = silib_desc_copy_DvbExt(pstDest, pstSource);		break;
	case eSIxSPEC_ARIB:			eErr = silib_desc_copy_Arib(pstDest, pstSource);		break;
	case eSIxSPEC_SSUUNT:		eErr = silib_desc_copy_Unt(pstDest, pstSource);			break;
	case eSIxSPEC_FSAT:			eErr = silib_desc_copy_Fsat(pstDest, pstSource);		break;
	case eSIxSPEC_SES:			eErr = silib_desc_copy_Ses(pstDest, pstSource);			break;
	case eSIxSPEC_ERTELECOM:	eErr = silib_desc_copy_Ertelecom(pstDest, pstSource);	break;
	case eSIxSPEC_AUSDTT:		eErr = silib_desc_copy_Ausdtt(pstDest, pstSource);		break;
	case eSIxSPEC_ITADTT:		eErr = silib_desc_copy_Itadtt(pstDest, pstSource);		break;

//	case eSIxSPEC_FsatGen2:		eErr = silib_desc_copy_Fsat(pstDest, pstSource);		break;
	case eSIxSPEC_UKDTT:		eErr = silib_desc_copy_Ukdtt(pstDest, pstSource);		break;
//	case eSIxSPEC_SKAPA:		eErr = silib_desc_copy_Skapa(pstDest, pstSource);		break;
	case eSIxSPEC_ASTRA:		break;
	case eSIxSPEC_NORDIG:		eErr = silib_desc_copy_Nordig(pstDest, pstSource);		break;
	case eSIxSPEC_ZIGGO:		eErr = silib_desc_copy_Ziggo(pstDest, pstSource);		break;
	case eSIxSPEC_BOXER:		break;
	case eSIxSPEC_MBC:			eErr = silib_desc_copy_Mbc(pstDest, pstSource);			break;
	default:
		break;
	}

	// Copy Head
	if (eSIxERR_OK == eErr)
	{
		HxSTD_MemCopy(&pstDest->stHead, &pstSource->stHead, sizeof(SIxDescriptorHeader_t));
	}

	return eErr;
}

static SIxError_e silib_desc_printDes(const SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr;
	SIxSpec_e	 eSpec;

	eSpec = pstDes->stHead.eSpec;

	eErr = eSIxERR_FAIL;
	switch (eSpec)
	{
	case eSIxSPEC_DVB:			eErr = silib_desc_print_Dvb(pstDes);		break;
	case eSIxSPEC_DVBEXT:		eErr = silib_desc_print_DvbExt(pstDes);		break;
	case eSIxSPEC_ARIB:			eErr = silib_desc_print_Arib(pstDes);		break;
	case eSIxSPEC_SSUUNT:		eErr = silib_desc_print_Unt(pstDes);		break;
	case eSIxSPEC_FSAT:			eErr = silib_desc_print_Fsat(pstDes);		break;
	case eSIxSPEC_SES:			eErr = silib_desc_print_Ses(pstDes);		break;
	case eSIxSPEC_ERTELECOM:	eErr = silib_desc_print_Ertelecom(pstDes);	break;
	case eSIxSPEC_AUSDTT:		eErr = silib_desc_print_Ausdtt(pstDes);		break;
	case eSIxSPEC_ITADTT:		eErr = silib_desc_print_Itadtt(pstDes);		break;

//	case eSIxSPEC_FsatGen2:		eErr = silib_desc_print_Fsat(pstDes);		break;
	case eSIxSPEC_UKDTT:		eErr = silib_desc_print_Ukdtt(pstDes);		break;
//	case eSIxSPEC_SKAPA:		eErr = silib_desc_print_Skapa(pstDes);		break;
	case eSIxSPEC_ASTRA:		break;
	case eSIxSPEC_NORDIG:		eErr = silib_desc_print_Nordig(pstDes);		break;
	case eSIxSPEC_ZIGGO:		eErr = silib_desc_print_Ziggo(pstDes);		break;
	case eSIxSPEC_BOXER:		break;
	case eSIxSPEC_MBC:			eErr = silib_desc_print_Mbc(pstDes);		break;
	default:
		break;
	}

	return eErr;
}


SIxError_e SILIB_DESC_Init (void *(*_malloc)(HLONG), HINT32 (*_free)(void *))
{
	if (_malloc && _free)
	{
		s_malloc	= _malloc;
		s_free		= _free;
	}

	return eSIxERR_OK;
}

SIxError_e SILIB_DESC_New (const SIxDescriptorTag_e eTag, const SIxSpec_e eSpec, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t **ppstDes)
{
	SIxError_e	 eErr;
	HUINT32		 ulSize;
	HUINT8		*p;
	SIxDescriptor_t	*pstDes = NULL;

	if (eSIxSPEC_MAX <= eSpec)		{ return eSIxERR_FAIL; }
	if (NULL == pucRawData)			{ return eSIxERR_FAIL; }
	if (NULL == ppstDes)			{ return eSIxERR_FAIL; }

	p = (HUINT8 *)pucRawData;

	if (eTag != *p++)				{ return eSIxERR_FAIL; }	// Check Tag

	ulSize = (HUINT32)*p++;

	/* DTG test 같은 경우 desc length가 0 이라도 해당 desc.가 들어왔는 지 확인이 되어야 한다고 한다
	   따라서 length zero라도 parsing은 진행한다
	if (ulSize == 0)				{ return eSIxERR_FAIL; }
	*/

	pstDes = *ppstDes = NULL;

	// 1. Alloc
	pstDes = silib_desc_allocDes(eSpec, eTag, pstParam, ulSize);
	if (NULL == pstDes)
	{
		HxLOG_Critical("silib_desc_allocDes(eSpec:%d, eTag:0x%X) error : Return NULL\n", eSpec, eTag);
		goto err_exit;
	}

	// 2. Parse
	eErr = silib_desc_parseDes(eSpec, eTag, pstParam, pucRawData, pstDes);
	if (eSIxERR_OK != eErr)
	{
		HxLOG_Critical("silib_desc_parseDes(eSpec:%d, eTag:0x%X) error : eErr [0x%x]\n", eSpec, eTag, eErr);
		goto err_exit;
	}

	*ppstDes = pstDes;

	return eErr;

err_exit:
	if (pstDes)
	{
		eErr = silib_desc_freeDes(pstDes);
	}

	return eSIxERR_FAIL;
}

SIxError_e SILIB_DESC_Delete (SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr;

	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	eErr = silib_desc_freeDes(pstDes);
	if (eSIxERR_OK != eErr)
		HxLOG_Critical("silib_desc_freeDes() error : eErr [0x%x]\n", eErr);

	return eErr;
}

SIxError_e SILIB_DESC_Clone (SIxDescriptor_t **ppstTarget, const SIxDescriptor_t *pstSource)
{
	SIxDescriptor_t		*pstOut;
	SIxDescriptorTag_e	 eTag;
	SIxSpec_e		 eSpec;
	HUINT32			 ulLength;
	SIxError_e		 eErr;

	if (NULL == pstSource)		{ return eSIxERR_FAIL; }
	if (NULL == ppstTarget)		{ return eSIxERR_FAIL; }

	eTag = pstSource->stHead.eTag;
	eSpec = pstSource->stHead.eSpec;
	ulLength = pstSource->stHead.ulLength;

	// Step.1 : Alloc
	*ppstTarget = pstOut = NULL;
	pstOut = silib_desc_allocDes(eSpec, eTag, &(pstSource->stHead.stParam), ulLength);
	if (NULL == pstOut)
	{
		HxLOG_Critical("silib_desc_allocDes(eSpec:%d, eTag:%d) error : Reture NULL\n", eSpec, eTag);
		goto err_exit;
	}

	// Step.2 : Copy Contents
	// Deep Copy : Not only structure memcopy but also pointer member field memory allocation and memcopy
	eErr = silib_desc_copyDes(pstOut, pstSource);
	if (eSIxERR_OK != eErr || NULL == pstOut)
	{
		HxLOG_Critical("silib_desc_copyDes(eSpec:%d, eTag:%d) error : eErr [0x%x]\n", pstSource->stHead.eSpec, pstSource->stHead.eTag, eErr);
		goto err_exit;
	}

	*ppstTarget = pstOut;

	return eErr;

err_exit:
	if (pstOut)
	{
		eErr = silib_desc_freeDes(pstOut);
		if (eSIxERR_OK != eErr)
			HxLOG_Critical("silib_desc_freeDes() error : eErr [0x%x]\n", eErr);
	}
	return eSIxERR_FAIL;
}

SIxError_e SILIB_DESC_Print (const SIxDescriptor_t *pstDes)
{
	SIxError_e		 eErr;

	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	eErr = silib_desc_printDes(pstDes);

	return eErr;
}

HCHAR* SILIB_DESC_GetStrOfSIxSpec_e(SIxSpec_e eSpec)
{
	switch(eSpec)
	{
		ENUM_TO_STR(eSIxSPEC_DVB);
		ENUM_TO_STR(eSIxSPEC_DVBEXT);
		ENUM_TO_STR(eSIxSPEC_SSUUNT);
		ENUM_TO_STR(eSIxSPEC_ARIB);
		ENUM_TO_STR(eSIxSPEC_FSAT);
		ENUM_TO_STR(eSIxSPEC_UKDTT);
		ENUM_TO_STR(eSIxSPEC_SKAPA);
		ENUM_TO_STR(eSIxSPEC_ASTRA);
		ENUM_TO_STR(eSIxSPEC_NORDIG);
		ENUM_TO_STR(eSIxSPEC_ZIGGO);
		ENUM_TO_STR(eSIxSPEC_BOXER);
		ENUM_TO_STR(eSIxSPEC_SES);
		ENUM_TO_STR(eSIxSPEC_ERTELECOM);
		ENUM_TO_STR(eSIxSPEC_AUSDTT);
		ENUM_TO_STR(eSIxSPEC_ITADTT);
		ENUM_TO_STR(eSIxSPEC_MBC);
		
		ENUM_TO_STR(eSIxSPEC_MAX);
		default:
			return "??";
	}
}

/* end of file */

