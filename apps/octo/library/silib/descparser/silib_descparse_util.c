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
	@file     sxdescparse_dvbext.c
	@brief    sxdescparse_dvbext.c (Descriptor Parser)

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

static void silib_desc_util_initStr(HUINT8 *pStr, HUINT32 nStrLen)
{
	HxSTD_MemSet(pStr, '\0', nStrLen);
}


HUINT32	silib_desc_util_uriStrlen(HUINT8 ucSrcLen, HUINT8 *pucStr)
{
	HINT32	i, j = 0;

	for (i = 0; i < ucSrcLen; i++)
	{
		if (pucStr[i] >= 0x20 && pucStr[i] <= 0x7F)
			j++;
	}

	return j;
}

HUINT32	silib_desc_util_uriStrcpy(HUINT8 *pucDestUri, HUINT8 *pucSrcStr, HUINT8 ucStrLen)
{
	HINT32	i, j = 0;

	for (i = 0; i < ucStrLen; i++)
	{
		if (pucSrcStr[i] >= 0x20 && pucSrcStr[i] <= 0x7F)
			pucDestUri[j++] = pucSrcStr[i];
	}

	return j;
}


HULONG silib_desc_util_bcdToBinary(HULONG ulVal)
{
	HULONG	ulRetVal, ulIndex;
	HLONG	i;

	ulRetVal	= 0;
	ulIndex		= 1;

	for (i = 0; i < 8; i++)
	{
		ulRetVal += (ulVal & 0x0f) * ulIndex;
		ulVal  = ulVal >> 4;
		ulIndex *= 10;
	}

	return (ulRetVal);
}


HERROR silib_desc_util_makeCorrct639Code(HUINT8 *p)
{
	HUINT32		i;

	for (i = 0; i < 3; i++)
	{
		// DVB string 의 language indicator charactor 와 conflict 를 방지한다.
		// SI data 가 이상해하게 들어와 hangup 되는 issue 가 있었음
		if (*p < 0x20)
		{
			*p = '\0';
		}
		p++;
	}

	return ERR_OK;
}

HUINT32 silib_desc_util_dvbStrlen(const HUINT8 *pucStr)
{
//	HINT32			ulCount = 0;
//	HINT32			ulStrlen = 0;
//	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pucStr == NULL) return 0;

	//MWC_UTIL_CodePage_GetEncodingWithLenByLocaleCode(pcStr, &eEncoding, &ulCount, SETTING_DEFAULT_ENCODING);
	//ulStrlen = local_mwc_util_DvbStrlen(pcStr+ulCount, eEncoding);

	return HxSTD_StrLen(pucStr);

//	return ulStrlen + ulCount;
}

HUINT8 *silib_desc_util_allocStr(HUINT32 nStrLen)
{
	HUINT8*	pStr;

	if (nStrLen > 0)
	{
		pStr = (HUINT8 *)silib_desc_alloc(sizeof(HUINT8)*nStrLen);
	}
	else
	{
		pStr = NULL;
	}

	if (pStr == NULL)
	{
		HxLOG_Print("[ERROR] String allocation error!\n");
		return NULL;
	}

	silib_desc_util_initStr(pStr, nStrLen);

	return ((HUINT8 *)pStr);
}

static SIxTextEncoding_e 	s_eDefaultEnc = eSIxTEXT_ENC_300468_8859_9_BASE;
SIxTextEncoding_e silib_desc_util_getTextEnc(void)
{
	return s_eDefaultEnc;
}
void silib_desc_util_setTextEnc(SIxTextEncoding_e eEnc)
{
	s_eDefaultEnc = eEnc;
}

HUINT32 silib_desc_util_textToUTF8(const HCHAR *szLangCode, SIxTextEncoding_e eEnc, HUINT32 ucStrLen, const HUINT8 *pucSiText, HUINT8 **ppucDestStr)
{
	if (NULL == pucSiText)			return 0;
	if (NULL == ppucDestStr)		return 0;

	*ppucDestStr = SILIB_TEXT_MakeUtf8(eEnc, szLangCode, pucSiText, ucStrLen);

	return (*ppucDestStr) ? HxSTD_StrLen(*ppucDestStr) : 0;
}


/************************************************************************
 * Private Data 관련...
 * 	(ps) private data 구조는 MPEG or DVD 규정의 구조체가 아님.
 *		 임의의 크기의 data를 처리하기 위하여 define한 구조체.
 ************************************************************************/
SIxPrivateDataInfo_t *silib_desc_util_allocPrivateDataInfo(HUINT8 ucLength)
{
	HUINT32					 ulSize;
	SIxPrivateDataInfo_t		*pstInfo;

	ulSize = sizeof(SIxPrivateDataInfo_t) + sizeof(HUINT8)*ucLength;
	pstInfo = (SIxPrivateDataInfo_t *)silib_desc_alloc(ulSize);
	if (pstInfo)
	{
		HxSTD_MemSet(pstInfo, 0, sizeof(SIxPrivateDataInfo_t));
		pstInfo->ucPrivateDataLen = ucLength;
	}

	return pstInfo;
}

void silib_desc_util_freePrivateDataInfo(SIxPrivateDataInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

SIxError_e silib_desc_util_setPrivateDataInfo(SIxPrivateDataInfo_t *pstInfo, HUINT8 *pucData, HUINT8 ucLength)
{
	HUINT32		ulSize;

	if (NULL == pstInfo)						return eSIxERR_TARGET_NULL;
	if (NULL == pstInfo->pucPrivateData)		return eSIxERR_TARGET_NULL;
	if (NULL == pucData)						return eSIxERR_SOURCE_NULL;
	if (0 == ucLength)							return eSIxERR_INVALID;
	if (pstInfo->ucPrivateDataLen < ucLength)	return eSIxERR_INVALID;

	ulSize = sizeof(HUINT8)*pstInfo->ucPrivateDataLen;
	HxSTD_MemSet(pstInfo->pucPrivateData, 0, ulSize);
	pstInfo->ucPrivateDataLen = ucLength;
	HxSTD_MemCopy(pstInfo->pucPrivateData, pucData, ulSize);

	return eSIxERR_OK;
}

SIxError_e silib_desc_util_parsePrivateDataInfo(HUINT8 *p, SIxPrivateDataInfo_t *pstDes)
{
	SIxError_e		eErr;

	if (NULL == pstDes)	return eSIxERR_FAIL;

	p++;	// Tag
	pstDes->ucPrivateDataLen = *p++;

	eErr = eSIxERR_FAIL;
	if (pstDes->ucPrivateDataLen > 0)
	{
		HxSTD_MemCopy(pstDes->pucPrivateData, p, pstDes->ucPrivateDataLen);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

SIxError_e silib_desc_util_copyPrivateDataInfo(SIxPrivateDataInfo_t *pstDest, const SIxPrivateDataInfo_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;
	HUINT32			ulSize;

	if (pstDest && pstSource)
	{
		if (pstDest->ucPrivateDataLen < pstSource->ucPrivateDataLen)
			goto err_exit;

		ulSize = sizeof(SIxPrivateDataInfo_t) + pstSource->ucPrivateDataLen;
		HxSTD_MemCopy(pstDest, pstSource, ulSize);
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		pstDest->ucPrivateDataLen = 0;
	}
	return eSIxERR_FAIL;
}

SIxPrivateDataInfo_t *silib_desc_util_dumpPrivateDataInfo(const SIxPrivateDataInfo_t *pstSource)
{
	SIxError_e				 eErr;
	SIxPrivateDataInfo_t		*pstInfo;

	if (NULL == pstSource)						return NULL;
	if (0 == pstSource->ucPrivateDataLen)		return NULL;
	if (NULL == pstSource->pucPrivateData)		return NULL;

	pstInfo = silib_desc_util_allocPrivateDataInfo(pstSource->ucPrivateDataLen);
	if (NULL == pstInfo)	return NULL;

	eErr = silib_desc_util_copyPrivateDataInfo(pstInfo, pstSource);
	if (eSIxERR_OK != eErr)
	{
		silib_desc_util_freePrivateDataInfo(pstInfo);
		return NULL;
	}

	return pstInfo;
}

