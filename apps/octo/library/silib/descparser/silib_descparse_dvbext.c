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

#if 0
static HUINT8 *silib_desc_allocInfo_DvbExt(const SIxDescriptorTag_e eTag, HUINT16 usSubTag);
static void silib_desc_freeInfo_DvbExt(const SIxDescriptorTag_e eTag, HUINT16 usSubTag, HUINT8 *pucDesInfo);
#endif

static HCHAR *silib_desc_getTagString_DvbExt(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
	ENUM_TO_STR(eSIxDESCTAG_EXT_IMG_ICON);
 	ENUM_TO_STR(eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING);
 	ENUM_TO_STR(eSIxDESCTAG_EXT_CP);
	ENUM_TO_STR(eSIxDESCTAG_EXT_CP_IDENTIFIER);
	ENUM_TO_STR(eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM);
	ENUM_TO_STR(eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM);
	ENUM_TO_STR(eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO);
	ENUM_TO_STR(eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY);
	ENUM_TO_STR(eSIxDESCTAG_EXT_MESSAGE);
	ENUM_TO_STR(eSIxDESCTAG_EXT_TARGET_REG);
	ENUM_TO_STR(eSIxDESCTAG_EXT_TARGET_REG_NAME);
	ENUM_TO_STR(eSIxDESCTAG_EXT_SERVICE_RELOCATED);
	ENUM_TO_STR(eSIxDESCTAG_EXT_XAIT_PID);
	ENUM_TO_STR(eSIxDESCTAG_EXT_C2_DELIVERY_SYSTEM);
	ENUM_TO_STR(eSIxDESCTAG_EXT_DTS_HD_AUDIO_STREAM	);
	ENUM_TO_STR(eSIxDESCTAG_EXT_DTS_NEURAL);
	ENUM_TO_STR(eSIxDESCTAG_EXT_VIDEO_DEPTH_RANGE);
	ENUM_TO_STR(eSIxDESCTAG_EXT_T2MI);
	ENUM_TO_STR(eSIxDESCTAG_EXT_URI_LINKAGE);
	ENUM_TO_STR(eSIxDESCTAG_EXT_BCI_ANCILLARY_DATA);

	default:
		break;
	}

	return NULL;
}

/************************************************************************
 * eSIxDESCTAG_EXT_IMG_ICON (TAG = 0x00)
 ************************************************************************/
//ImageIcon
static void initImageIconDes(SIxImageIconDes_t *pImageIconDes)
{
	pImageIconDes->ucDesNum = 0xff;
	pImageIconDes->ucLastDesNum = 0xff;
	pImageIconDes->ucIconId = 0xff;
	pImageIconDes->pstImgHeadInfo = NULL;
	pImageIconDes->pstImgIconInfo = NULL;
}

static SIxImageIconDes_t *allocImageIconDes(void)
{
	return (SIxImageIconDes_t *)silib_desc_alloc(sizeof(SIxImageIconDes_t));
}

static void freeImageIconDes(SIxImageIconDes_t *pstDes)
{
	if (pstDes)
	{
		if (pstDes->pstImgHeadInfo)
		{
			SIxImageIconHeadInfo_t	*pImgHeadInfo = pstDes->pstImgHeadInfo;
			if (pImgHeadInfo->pstImgTypeInfo)
			{
				SIxImageIconTypeInfo_t	*pImgTypeInfo = pImgHeadInfo->pstImgTypeInfo;
				silib_desc_free(pImgTypeInfo->pucImgType);
				silib_desc_free(pImgTypeInfo);
			}
			if (pImgHeadInfo->pstImgUrlInfo)
			{
				SIxImageIconURLInfo_t	*pImgUrlInfo = pImgHeadInfo->pstImgUrlInfo;
				silib_desc_free(pImgUrlInfo->pucImgUrl);
				silib_desc_free(pImgUrlInfo);
			}
			if (pImgHeadInfo->pstImgIconInfo)
			{
				SIxImageIconDataInfo_t	*pImgIconInfo = pImgHeadInfo->pstImgIconInfo;
				silib_desc_free(pImgIconInfo->pucImgData);
				silib_desc_free(pImgIconInfo);
			}
			silib_desc_free(pImgHeadInfo);
		}

		if (pstDes->pstImgIconInfo)
		{
			SIxImageIconDataInfo_t	*pImgIconInfo = pstDes->pstImgIconInfo;
			silib_desc_free(pImgIconInfo->pucImgData );
			silib_desc_free(pImgIconInfo);
		}
		silib_desc_free(pstDes);
	}
}

static SIxError_e parseImageIconDes(HUINT8 *p, SIxImageIconDes_t *pstDes)
{
	HUINT8					ucTmpValue;
	HUINT16 				usTmpValue;
	HUINT8					ucImgIconDesLen;
	SIxError_e				eErr = eSIxERR_OK;
	SIxImageIconDataInfo_t	*pstImgIcoDataInfo;
	SIxImageIconDataInfo_t	*pstImgIcoDataInfo2;
	SIxImageIconHeadInfo_t	*pstImgHeadInfo;
	SIxImageIconTypeInfo_t	*pstImgTypeInfo;
	SIxImageIconURLInfo_t	*pstImgUrlInfo;

	HxLOG_Info("parseExtImgIconDes () Entered!");

	*p++;						// Tag
	ucImgIconDesLen = *p++;		// Length

	ucTmpValue = *p++; ucImgIconDesLen--;

	/** < Descriptor Number (4 bit) > */
	pstDes->ucDesNum = ((ucTmpValue >> 4) & 0x0F);

	/** < Last Descriptor Number (4 bit) > */
	pstDes->ucLastDesNum = (ucTmpValue & 0x0F);

	ucTmpValue = *p++; ucImgIconDesLen--;
	/** < Icon Id (3 bit) > */
	pstDes->ucIconId = (ucTmpValue & 0x07);

	HxLOG_Print("\t\t(DesNum:%d) (LastDesNum:%d) (IconId:0x%X)\n",
			pstDes->ucDesNum, pstDes->ucLastDesNum, pstDes->ucIconId);

	pstImgIcoDataInfo2	= NULL;
	pstImgIcoDataInfo	= NULL;
	pstImgHeadInfo		= NULL;
	pstImgTypeInfo		= NULL;
	pstImgUrlInfo		= NULL;
	if ( 0 != pstDes->ucDesNum )
	{
		pstDes->pstImgHeadInfo = NULL;
		pstDes->pstImgIconInfo = NULL;
		pstImgIcoDataInfo = silib_desc_alloc(sizeof(SIxImageIconDataInfo_t));
		if (NULL == pstImgIcoDataInfo)
		{
			eErr = eSIxERR_NOT_ENOUGH_MEMORY;
			goto err_exit;
		}

		ucTmpValue = *p++; ucImgIconDesLen--;
		pstImgIcoDataInfo->ucDataLen = ucTmpValue;
		pstImgIcoDataInfo->pucImgData = silib_desc_alloc(ucTmpValue+1);
		if (NULL == pstImgIcoDataInfo->pucImgData)
		{
			eErr = eSIxERR_NOT_ENOUGH_MEMORY;
			goto err_exit;
		}

		pstImgIcoDataInfo->pucImgData[ucTmpValue] = 0;
		HxLOG_Print("\t\tIcon data exist!, Len:%d\n",ucTmpValue);

		HxSTD_MemCopy(pstImgIcoDataInfo->pucImgData, p, ucTmpValue);
		p += ucTmpValue;
		ucImgIconDesLen -= ucTmpValue;
		pstDes->pstImgIconInfo = pstImgIcoDataInfo;
	}
	else
	{
		pstDes->pstImgHeadInfo = NULL;
		pstDes->pstImgIconInfo = NULL;
		pstImgHeadInfo = silib_desc_alloc(sizeof(SIxImageIconHeadInfo_t));
		if (NULL == pstImgHeadInfo)
		{
			eErr = eSIxERR_NOT_ENOUGH_MEMORY;
			goto err_exit;
		}
		HxSTD_MemSet(pstImgHeadInfo, 0 , sizeof(SIxImageIconHeadInfo_t));
		ucTmpValue = *p++; ucImgIconDesLen--;
		pstImgHeadInfo->ucIconTransMode = ((ucTmpValue >> 6) & 0x3);
		pstImgHeadInfo->ucIconPosFlag = ((ucTmpValue >> 5) & 0x1);

		HxLOG_Print("\t\t(Icon Transport Mode : %d) (Position Flag : %d)\n",
				pstImgHeadInfo->ucIconTransMode, pstImgHeadInfo->ucIconPosFlag);

		if ( 0x01 == pstImgHeadInfo->ucIconPosFlag)
		{
			pstImgHeadInfo->ucCoordSystem = ((ucTmpValue >> 2) & 0x7);
			usTmpValue = *p++; ucImgIconDesLen--;
			usTmpValue <<= 4;
			usTmpValue |= ((*p) & 0x0F);
			pstImgHeadInfo->usIconHorOrigin = usTmpValue;
			pstImgHeadInfo->usIconVerOrigin = HxMACRO_Get12Bit(p);
			p += 2; ucImgIconDesLen -= 3;

			HxLOG_Print("\t\t(coordinate system : %d) (icon hor origin : %d) (icon ver origin : %d)\n",
					pstImgHeadInfo->ucCoordSystem, pstImgHeadInfo->usIconHorOrigin, pstImgHeadInfo->usIconVerOrigin);
		}

		ucTmpValue = *p++; ucImgIconDesLen--;
		if (0 != ucTmpValue)
		{
			pstImgTypeInfo = silib_desc_alloc(sizeof(SIxImageIconTypeInfo_t));
			if (NULL == pstImgTypeInfo)
			{
				eErr = eSIxERR_NOT_ENOUGH_MEMORY;
				goto err_exit;
			}
			HxSTD_MemSet(pstImgTypeInfo, 0 , sizeof(SIxImageIconTypeInfo_t));
			pstImgTypeInfo->usTypeLen = ucTmpValue;
			pstImgTypeInfo->pucImgType = silib_desc_alloc(ucTmpValue+1);
			if (NULL == pstImgTypeInfo->pucImgType)
			{
				eErr = eSIxERR_NOT_ENOUGH_MEMORY;
				goto err_exit;
			}

			pstImgTypeInfo->pucImgType[ucTmpValue] = 0;
			HxLOG_Print("\t\tIcon Type info exist!, Len:%d\n",ucTmpValue);

			HxSTD_MemCopy(pstImgTypeInfo->pucImgType, p , ucTmpValue);
			p += ucTmpValue; ucImgIconDesLen -= ucTmpValue;
			pstImgHeadInfo->pstImgTypeInfo = pstImgTypeInfo;
		}

		if ( 0x0 == pstImgHeadInfo->ucIconTransMode )
		{
			// icon data length
			ucTmpValue = *p++; ucImgIconDesLen--;
			if (0 != ucTmpValue)
			{
				pstImgIcoDataInfo2 = silib_desc_alloc(sizeof(SIxImageIconDataInfo_t));
				if (NULL == pstImgIcoDataInfo2)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}
				HxSTD_MemSet(pstImgIcoDataInfo2, 0 , sizeof(SIxImageIconDataInfo_t));
				pstImgIcoDataInfo2->ucDataLen = ucTmpValue;
				pstImgIcoDataInfo2->pucImgData = silib_desc_alloc(ucTmpValue+1);
				if (NULL == pstImgIcoDataInfo2->pucImgData)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}

				pstImgIcoDataInfo2->pucImgData[ucTmpValue] = 0;
				HxLOG_Print("\t\tIcon data exist!, Len:%d\n",ucTmpValue);

				HxSTD_MemCopy(pstImgIcoDataInfo2->pucImgData, p, ucTmpValue);
				p += ucTmpValue;
				ucImgIconDesLen -= ucTmpValue;
				pstImgHeadInfo->pstImgIconInfo = pstImgIcoDataInfo2;
			}
		}
		else if (0x01 == pstImgHeadInfo->ucIconTransMode)
		{
			// URL length
			ucTmpValue = *p++; ucImgIconDesLen--;
			if (0 != ucTmpValue)
			{
				pstImgUrlInfo = silib_desc_alloc(sizeof(SIxImageIconURLInfo_t));
				if (NULL == pstImgUrlInfo)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}
				HxSTD_MemSet(pstImgUrlInfo, 0 , sizeof(SIxImageIconURLInfo_t));
				pstImgUrlInfo->usUrlLen = ucTmpValue;
				pstImgUrlInfo->pucImgUrl = silib_desc_alloc(ucTmpValue+1);
				if (NULL == pstImgUrlInfo->pucImgUrl)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}

				HxSTD_MemCopy(pstImgUrlInfo->pucImgUrl, p, ucTmpValue);
				pstImgUrlInfo->pucImgUrl[ucTmpValue] = 0;
				HxLOG_Print("\t\tIcon URL exist!, Len:%d\n",ucTmpValue);

				p += ucTmpValue;
				ucImgIconDesLen -= ucTmpValue;
				pstImgHeadInfo->pstImgUrlInfo= pstImgUrlInfo;
			}
		}

		pstDes->pstImgHeadInfo = pstImgHeadInfo;
	}
	return eSIxERR_OK;

err_exit:
	if (pstImgTypeInfo)
	{
		silib_desc_free(pstImgTypeInfo->pucImgType);
		silib_desc_free(pstImgTypeInfo);
	}

	if (pstImgIcoDataInfo)
	{
		silib_desc_free(pstImgIcoDataInfo->pucImgData);
		silib_desc_free(pstImgIcoDataInfo);
	}

	if (pstImgIcoDataInfo2)
	{
		silib_desc_free(pstImgIcoDataInfo2->pucImgData);
		silib_desc_free(pstImgIcoDataInfo2);
	}

	if (pstImgUrlInfo)
	{
		silib_desc_free(pstImgUrlInfo->pucImgUrl);
		silib_desc_free(pstImgUrlInfo);
	}

	silib_desc_free(pstImgHeadInfo);

	return eErr;
}

static SIxError_e copyImageIconDes(SIxImageIconDes_t *pstDest, const SIxImageIconDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;
	SIxImageIconDataInfo_t	*pstImgIcoDataInfo = NULL, *pstImgIcoDataInfo2 = NULL;
	SIxImageIconHeadInfo_t	*pstImgHeadInfo = NULL;
	SIxImageIconTypeInfo_t	*pstImgTypeInfo = NULL;
	SIxImageIconURLInfo_t	*pstImgUrlInfo = NULL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxImageIconDes_t));
		pstDest->pstImgHeadInfo	= NULL;
		pstDest->pstImgIconInfo	= NULL;

		if (pstSource->pstImgHeadInfo)
		{
			pstImgHeadInfo = silib_desc_alloc(sizeof(SIxImageIconHeadInfo_t));
			if (NULL == pstImgHeadInfo)
			{
				eErr = eSIxERR_NOT_ENOUGH_MEMORY;
				goto err_exit;
			}

			HxSTD_MemCopy(pstImgHeadInfo, pstSource->pstImgHeadInfo, sizeof(SIxImageIconHeadInfo_t));
			pstImgHeadInfo->pstImgTypeInfo	= NULL;
			pstImgHeadInfo->pstImgUrlInfo	= NULL;
			pstImgHeadInfo->pstImgIconInfo	= NULL;
			if (pstSource->pstImgHeadInfo->pstImgTypeInfo)
			{
				pstImgTypeInfo = silib_desc_alloc(sizeof(SIxImageIconTypeInfo_t));
				if (NULL == pstImgTypeInfo)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}

				HxSTD_MemCopy(pstImgTypeInfo, pstSource->pstImgHeadInfo->pstImgTypeInfo, sizeof(SIxImageIconTypeInfo_t));
				pstImgTypeInfo->pucImgType = NULL;

				pstImgTypeInfo->pucImgType = silib_desc_alloc(sizeof(HUINT8)*pstImgTypeInfo->usTypeLen);
				if (NULL == pstImgTypeInfo->pucImgType)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}
				HxSTD_MemCopy(pstImgTypeInfo->pucImgType, pstSource->pstImgHeadInfo->pstImgTypeInfo->pucImgType, sizeof(HUINT8)*pstImgTypeInfo->usTypeLen);
				pstImgHeadInfo->pstImgTypeInfo = pstImgTypeInfo;
			}

			if (pstSource->pstImgHeadInfo->pstImgIconInfo)
			{
				pstImgIcoDataInfo = silib_desc_alloc(sizeof(SIxImageIconDataInfo_t));
				if (NULL == pstImgIcoDataInfo)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}

				HxSTD_MemCopy(pstImgIcoDataInfo, pstSource->pstImgHeadInfo->pstImgIconInfo, sizeof(SIxImageIconDataInfo_t));
				pstImgIcoDataInfo->pucImgData = NULL;

				pstImgIcoDataInfo->pucImgData = silib_desc_alloc(sizeof(HUINT8)*pstImgIcoDataInfo->ucDataLen);
				if (NULL == pstImgIcoDataInfo->pucImgData)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}
				HxSTD_MemCopy(pstImgIcoDataInfo->pucImgData, pstSource->pstImgHeadInfo->pstImgIconInfo->pucImgData, sizeof(HUINT8)*pstImgIcoDataInfo->ucDataLen);
				pstImgHeadInfo->pstImgIconInfo = pstImgIcoDataInfo;
			}

			if (pstSource->pstImgHeadInfo->pstImgUrlInfo)
			{
				pstImgUrlInfo = silib_desc_alloc(sizeof(SIxImageIconURLInfo_t));
				if (NULL == pstImgUrlInfo)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}

				HxSTD_MemCopy(pstImgUrlInfo, pstSource->pstImgHeadInfo->pstImgUrlInfo, sizeof(SIxImageIconURLInfo_t));
				pstImgUrlInfo->pucImgUrl = NULL;

				pstImgUrlInfo->pucImgUrl = silib_desc_alloc(sizeof(HUINT8)*pstImgUrlInfo->usUrlLen);
				if (NULL == pstImgUrlInfo->pucImgUrl)
				{
					eErr = eSIxERR_NOT_ENOUGH_MEMORY;
					goto err_exit;
				}
				HxSTD_MemCopy(pstImgUrlInfo->pucImgUrl, pstSource->pstImgHeadInfo->pstImgUrlInfo->pucImgUrl, sizeof(HUINT8)*pstImgUrlInfo->usUrlLen);

				pstImgHeadInfo->pstImgUrlInfo = pstImgUrlInfo;
			}
			eErr = eSIxERR_OK;
		}
		pstDest->pstImgHeadInfo = pstImgHeadInfo;

		if (pstSource->pstImgIconInfo)
		{
			eErr = eSIxERR_FAIL;
			pstImgIcoDataInfo2 = silib_desc_alloc(sizeof(SIxImageIconDataInfo_t));
			if (NULL == pstImgIcoDataInfo2)
			{
				eErr = eSIxERR_NOT_ENOUGH_MEMORY;
				goto err_exit;
			}
			HxSTD_MemCopy(pstImgIcoDataInfo2, pstSource->pstImgIconInfo, sizeof(SIxImageIconDataInfo_t));
			pstImgIcoDataInfo2->pucImgData = NULL;

			pstImgIcoDataInfo2->pucImgData = silib_desc_alloc(sizeof(HUINT8)*pstImgIcoDataInfo2->ucDataLen);
			if (NULL == pstImgIcoDataInfo2->pucImgData)
			{
				eErr = eSIxERR_NOT_ENOUGH_MEMORY;
				goto err_exit;
			}
			HxSTD_MemCopy(pstImgIcoDataInfo2->pucImgData, pstSource->pstImgIconInfo->pucImgData, sizeof(HUINT8)*pstImgIcoDataInfo2->ucDataLen);
			eErr = eSIxERR_OK;
		}
		pstDest->pstImgIconInfo = pstImgIcoDataInfo2;
	}

	return eErr;

err_exit:
	if (pstImgTypeInfo)
	{
		silib_desc_free(pstImgTypeInfo->pucImgType);
		silib_desc_free(pstImgTypeInfo);
	}

	if (pstImgIcoDataInfo)
	{
		silib_desc_free(pstImgIcoDataInfo->pucImgData);
		silib_desc_free(pstImgIcoDataInfo);
	}

	if (pstImgUrlInfo)
	{
		silib_desc_free(pstImgUrlInfo->pucImgUrl);
		silib_desc_free(pstImgUrlInfo);
	}

	if (pstImgIcoDataInfo2)
	{
		silib_desc_free(pstImgIcoDataInfo2->pucImgData);
		silib_desc_free(pstImgIcoDataInfo2);
	}

	if (pstImgHeadInfo)
	{
		silib_desc_free(pstImgHeadInfo);
	}

	pstDest->pstImgHeadInfo		= NULL;
	pstDest->pstImgIconInfo		= NULL;

	return eErr;
}



/************************************************************************
 * eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING (TAG = 0x01)
 ************************************************************************/
#if 0
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	pstDes->pLangInfo = NULL;
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
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocXXXInfo()\n");
		return NULL;
	}
	initXXXInfo(pstInfo);

	return ((SIxXXXInfo_t *)pstInfo);
}


static void freeXXXInfo(SIxXXXInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	silib_desc_free(pstDes);
}

static void setXXXInfo(SIxXXXInfo_t *pstInfo, )
{
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{

}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxXXXInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxXXXDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxXXXInfo_t, pstDest);
			pstNewInfo = allocXXXInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			setXXXInfo(pstNewInfo, pstSourceInfo->szXXXCode, pstSourceInfo->ucAudioType);
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}
#endif


/************************************************************************
 * eSIxDESCTAG_EXT_CP (TAG = 0x02)
 ************************************************************************/
#if 0
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	pstDes->pLangInfo = NULL;
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
	SIxXXXInfo_t *pstInfo;

	pstInfo = (SIxXXXInfo_t *)silib_desc_alloc(sizeof(SIxXXXInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocXXXInfo()\n");
		return NULL;
	}
	initXXXInfo(pstInfo);

	return ((SIxXXXInfo_t *)pstInfo);
}


static void freeXXXInfo(SIxXXXInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (NULL == pstDes) 	return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	silib_desc_free(pstDes);
}

static void setXXXInfo(SIxXXXInfo_t *pstInfo, )
{
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{

}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

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
			pstNewInfo = allocXXXInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			setXXXInfo(pstNewInfo, pstSourceInfo->szXXXCode, pstSourceInfo->ucAudioType);
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}
#endif

/************************************************************************
 * eSIxDESCTAG_EXT_CP_IDENTIFIER (TAG = 0x03)
 ************************************************************************/
#if 0
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	pstDes->pLangInfo = NULL;
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
	SIxXXXInfo_t *pstInfo;

	pstInfo = (SIxXXXInfo_t *)silib_desc_alloc(sizeof(SIxXXXInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocXXXInfo()\n");
		return NULL;
	}
	initXXXInfo(pstInfo);

	return ((SIxXXXInfo_t *)pstInfo);
}


static void freeXXXInfo(SIxXXXInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (NULL == pstDes) 	return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	silib_desc_free(pstDes);
}

static void setXXXInfo(SIxXXXInfo_t *pstInfo, )
{
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{

}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

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
			pstNewInfo = allocXXXInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			setXXXInfo(pstNewInfo, pstSourceInfo->szXXXCode, pstSourceInfo->ucAudioType);
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}
#endif

/************************************************************************
 * eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM (TAG = 0x04)
 ************************************************************************/
//T2DeliverySystem
static void initT2DeliverySystemDes(SIxT2DeliverySystemDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxT2DeliverySystemDes_t));
	pstDes->ucPlpId			 = 0xFF;
	pstDes->usT2SystemId	 = 0xFFFF;
	pstDes->pstDetailInfo	 = NULL;
}

static void initT2DeliverySystemInfo(SIxT2DeliveryCellInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxT2DeliveryCellInfo_t));
}

static SIxT2DeliverySystemDes_t *allocT2DeliverySystemDes(void)
{
	return (SIxT2DeliverySystemDes_t *)silib_desc_alloc(sizeof(SIxT2DeliverySystemDes_t));
}

static SIxT2DeliveryCellInfo_t *allocT2DeliveryCellInfo(void)
{
	SIxT2DeliveryCellInfo_t	*pstInfo;

	pstInfo = (SIxT2DeliveryCellInfo_t *)silib_desc_alloc(sizeof(SIxT2DeliveryCellInfo_t));
	if (pstInfo)
	{
		initT2DeliverySystemInfo(pstInfo);
	}

	return pstInfo;
}

static void freeT2DeliveryCellInfo(SIxT2DeliveryCellInfo_t *pstInfo)
{
	silib_desc_free(pstInfo->pstInfo);
	silib_desc_free(pstInfo->pulCentreFreq);
	silib_desc_free(pstInfo);
}

static void freeT2DeliverySystemDes(SIxT2DeliverySystemDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	if (pstDes->pstDetailInfo)
	{
		SIxT2DeliveryCellInfo_t *pstInfo = pstDes->pstDetailInfo->pstInfo;
		if (pstInfo)
		{
			FREE_INFO_LIST_EX(pstInfo, SIxT2DeliveryCellInfo_t, freeT2DeliveryCellInfo);
		}
		silib_desc_free(pstDes->pstDetailInfo);
	}

	silib_desc_free(pstDes);
}

static void setT2DeliveryCellInfo(SIxT2DeliveryCellInfo_t *pstInfo, HUINT16 usCellId, HUINT8 ucFreqLoopCnt, HUINT32 *pulCentreFreq, HUINT8 ucSubCellInfoLoopCnt, SIxT2DeliverySubCellInfo_t *pSubCellInfo)
{
	pstInfo->usCellId				= usCellId;
	pstInfo->ucFreqLoopCnt			= ucFreqLoopCnt;
	pstInfo->pulCentreFreq			= pulCentreFreq;
	pstInfo->ucSubCellInfoLoopCnt	= ucSubCellInfoLoopCnt;
	pstInfo->pstInfo				= pSubCellInfo;
}

static SIxError_e parseT2DeliverySystemDes(HUINT8 *p, SIxT2DeliverySystemDes_t *pstDes)
{
	HUINT8						 ucTmpValue;
	HUINT8						 ucT2DelSysLen;
	HUINT16 					 usTmpValue;
	HUINT32 					 i,ulCnt;
	SIxT2DeliveryDetailInfo_t	*pstDetailInfo = NULL;
	SIxT2DeliveryCellInfo_t		*pstNewInfo = NULL, *pstPrevInfo = NULL;

	HxLOG_Info("parseExtT2DeliverySystemDes () Entered!");

	p++;						// Tag
	ucT2DelSysLen = *p++;		// Length

	/** < plp id (1byte) */
	pstDes->ucPlpId = *p++; ucT2DelSysLen--;
	HxLOG_Print("\t\t(PLP Id = 0x%x)\n", pstDes->ucPlpId);

	/** < T2 system id (2byte) */
	pstDes->usT2SystemId = HxMACRO_Get16Bit(p); p += 2; ucT2DelSysLen -= 2;
	HxLOG_Print("\t\t(T2 System Id = 0x%x)\n", pstDes->usT2SystemId);

	pstDes->pstDetailInfo = NULL;
	if ( ucT2DelSysLen < 1)
	{
		HxLOG_Print("\t\t(There are no T2 detail information)\n");
		goto err_exit;
	}
	else
	{
		pstDetailInfo = silib_desc_alloc(sizeof(SIxT2DeliveryDetailInfo_t));
		if (NULL == pstDetailInfo)
		{
			HxLOG_Info(" parseExtT2DeliverySystemDes () Error Leaved!");
			goto err_exit;
		}
		usTmpValue = HxMACRO_Get16Bit(p); p += 2; ucT2DelSysLen -= 2;

		pstDetailInfo->ucSMISO = (HUINT8)((usTmpValue >> 14) & 0x03);
		pstDetailInfo->ucBandWidth = (HUINT8)((usTmpValue >> 10) & 0x0F);
		pstDetailInfo->ucGuardInterval = (HUINT8)((usTmpValue >> 5) & 0x07);
		pstDetailInfo->ucTransmissionMode = (HUINT8)((usTmpValue >> 2) & 0x07);
		pstDetailInfo->ucOtherFreqFlag = (HUINT8)((usTmpValue >> 1) & 0x01);
		pstDetailInfo->ucTfsFlag = (HUINT8)(usTmpValue & 0x01);
		pstDetailInfo->pstInfo = NULL;

		pstDes->pstDetailInfo = pstDetailInfo;

		while (ucT2DelSysLen > 0 && ucT2DelSysLen < MAX_DESCRIPTOR_LIMIT)
		{
			/* remain size check */
			if (ucT2DelSysLen < 7)
			{
				p += ucT2DelSysLen;
				break;
			}

			/** < alloc Info */
			pstNewInfo = allocT2DeliveryCellInfo();
			if (NULL == pstNewInfo)
			{
				HxLOG_Print("[WARN] parseExtT2DeliverySystemDes Alloc error\n");
				goto err_exit;
			}

			pstNewInfo->usCellId = HxMACRO_Get16Bit(p); p += 2; ucT2DelSysLen -= 2;

			if (1 == pstDetailInfo->ucTfsFlag)
			{
				ucTmpValue = *p++; ucT2DelSysLen--;
				ulCnt = (HUINT32)(ucTmpValue >> 2); 		// ulCnt = ucTmpValue / 4
				pstNewInfo->ucFreqLoopCnt = (HUINT8)ulCnt;
				pstNewInfo->pulCentreFreq = silib_desc_alloc(sizeof(HUINT32)*ulCnt);
				for (i=0; i < ulCnt; i++)
				{
					pstNewInfo->pulCentreFreq[i] = HxMACRO_Get32Bit(p); p += 4; ucT2DelSysLen -= 4;
				}
			}
			else
			{
				pstNewInfo->pulCentreFreq = silib_desc_alloc(sizeof(HUINT32));
				pstNewInfo->ucFreqLoopCnt = 1;
				*pstNewInfo->pulCentreFreq = HxMACRO_Get32Bit(p); p += 4; ucT2DelSysLen -= 4;
			}
			ucTmpValue = *p++; ucT2DelSysLen--;
			ulCnt = (HUINT32)(ucTmpValue/5);
			pstNewInfo->ucSubCellInfoLoopCnt = (HUINT8)ulCnt;
			pstNewInfo->pstInfo = (SIxT2DeliverySubCellInfo_t *)silib_desc_alloc(sizeof(SIxT2DeliverySubCellInfo_t)*ulCnt);
			for (i=0; i < ulCnt; i++)
			{
				pstNewInfo->pstInfo[i].ucCellIdExtension = *p++; ucT2DelSysLen -= 1;
				pstNewInfo->pstInfo[i].ulTransponderFreq = HxMACRO_Get32Bit(p); p += 4; ucT2DelSysLen -= 4;
			}

			LINK_DES_NODE(pstDes->pstDetailInfo->pstInfo, pstPrevInfo, pstNewInfo);
		}
	}

err_exit:
	if (pstDes->pstDetailInfo)
	{
		FREE_INFO_LIST_EX(pstDes->pstDetailInfo->pstInfo, SIxT2DeliveryCellInfo_t, freeT2DeliveryCellInfo);
		silib_desc_free(pstDes->pstDetailInfo);
		pstDes->pstDetailInfo = NULL;
	}
	HxLOG_Info(" parseExtT2DeliverySystemDes () Leaved!");
	return eSIxERR_FAIL;
}

static SIxError_e copyT2DeliverySystemDes(SIxT2DeliverySystemDes_t *pstDest, const SIxT2DeliverySystemDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxT2DeliveryCellInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxT2DeliverySystemDes_t));
		pstDest->pstDetailInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstDetailInfo)
		{
			pstDest->pstDetailInfo = silib_desc_alloc(sizeof(SIxT2DeliveryDetailInfo_t));
			if (pstDest->pstDetailInfo)
			{
				HUINT32							*aulCentreFreq = NULL;
				HUINT8							 ucFreqLoopCnt = 0;
				SIxT2DeliverySubCellInfo_t		*astSubCellInfo = NULL;
				HUINT8							 ucCellInfoCnt = 0;

				HxSTD_MemCopy(pstDest->pstDetailInfo, pstSource->pstDetailInfo, sizeof(SIxT2DeliveryDetailInfo_t));
				pstDest->pstDetailInfo->pstInfo = NULL;

				pstSourceInfo = pstSource->pstDetailInfo->pstInfo;
				while (pstSourceInfo)
				{
					/** < Descriptor 데이타 구성. */
					GET_PREV_INFO(pstPrevInfo, SIxT2DeliveryCellInfo_t, pstDest->pstDetailInfo);
					pstNewInfo = allocT2DeliveryCellInfo();
					if (pstNewInfo == NULL)
						goto err_exit;

					if (0 < pstSourceInfo->ucFreqLoopCnt && NULL != pstSourceInfo->pulCentreFreq)
					{
						ucFreqLoopCnt = pstSourceInfo->ucFreqLoopCnt;
						aulCentreFreq = silib_desc_alloc(sizeof(HUINT32)*pstSourceInfo->ucFreqLoopCnt);
						if (aulCentreFreq)
						{
							HxSTD_MemCopy(aulCentreFreq, pstSourceInfo->pulCentreFreq, sizeof(HUINT32)*pstSourceInfo->ucFreqLoopCnt);
						}
					}

					if (0 < pstSourceInfo->ucSubCellInfoLoopCnt && NULL != pstSourceInfo->pstInfo)
					{
						ucCellInfoCnt = pstSourceInfo->ucSubCellInfoLoopCnt;
						astSubCellInfo = silib_desc_alloc(sizeof(SIxT2DeliverySubCellInfo_t)*pstSourceInfo->ucSubCellInfoLoopCnt);
						if (aulCentreFreq)
						{
							HxSTD_MemCopy(astSubCellInfo, pstSourceInfo->pstInfo, sizeof(HUINT32)*pstSourceInfo->ucSubCellInfoLoopCnt);
						}
					}

					setT2DeliveryCellInfo(pstNewInfo, pstSourceInfo->usCellId, ucFreqLoopCnt, aulCentreFreq, ucCellInfoCnt, astSubCellInfo);
					LINK_INFO_NODE(pstDest->pstDetailInfo->pstInfo, pstNewInfo, NULL, pstPrevInfo);

					pstSourceInfo = pstSourceInfo->pstNext;
				}
				eErr = eSIxERR_OK;
			}
		}
	}

	return eErr;

err_exit:
	if (pstDest->pstDetailInfo)
	{
		FREE_INFO_LIST_EX(pstDest->pstDetailInfo->pstInfo, SIxT2DeliveryCellInfo_t, freeT2DeliveryCellInfo);
		silib_desc_free(pstDest->pstDetailInfo);
		pstDest->pstDetailInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM (TAG = 0x05)
 ************************************************************************/
#if 0
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	pstDes->pLangInfo = NULL;
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
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocXXXInfo()\n");
		return NULL;
	}
	initXXXInfo(pstInfo);

	return ((SIxXXXInfo_t *)pstInfo);
}


static void freeXXXInfo(SIxXXXInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	silib_desc_free(pstDes);
}

static void setXXXInfo(SIxXXXInfo_t *pstInfo, )
{
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{

}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxXXXInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxXXXDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxXXXInfo_t, pstDest);
			pstNewInfo = allocXXXInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			setXXXInfo(pstNewInfo, pstSourceInfo->szXXXCode, pstSourceInfo->ucAudioType);
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}
#endif

/************************************************************************
 * eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO (TAG = 0x06)
 ************************************************************************/
//SupplementaryAudio

static void initSupplementaryAudioDes(SIxSupplementaryAudioDes_t *pSupplementaryAudioDes)
{
	HxSTD_MemSet(pSupplementaryAudioDes, 0, sizeof(SIxSupplementaryAudioDes_t));
}

static SIxSupplementaryAudioDes_t *allocSupplementaryAudioDes(void)
{
	return (SIxSupplementaryAudioDes_t *)silib_desc_alloc(sizeof(SIxSupplementaryAudioDes_t));
}


static void freeSupplementaryAudioDes(SIxSupplementaryAudioDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}


static SIxError_e parseSupplementaryAudioDes(HUINT8 *p, SIxSupplementaryAudioDes_t *pstDes)
{
	HUINT32		nLen;
	SIxError_e	eErr;

	p++;						// Tag
	nLen = (HINT32)*p++;		// Length

	pstDes->ucMixType = (*p & 0x80) >> 7;
	pstDes->ucEditorialClassification = (*p & 0x7C) >> 2;
	pstDes->ucLanguageCodePresent = (*p & 0x01);

	p++;
	nLen--;
	if ((pstDes->ucLanguageCodePresent) && (nLen >=3 ))
	{
		pstDes->ISO_639[0] = p[0];
		pstDes->ISO_639[1] = p[1];
		pstDes->ISO_639[2] = p[2];
		pstDes->ISO_639[3] = 0;
		p += 3;
		nLen -= 3;
	}

	eErr = eSIxERR_OK;
	if (0 < nLen)
	{
		pstDes->pstPrivateDataInfo = silib_desc_util_allocPrivateDataInfo((HUINT8)nLen);
		if (NULL == pstDes->pstPrivateDataInfo)
			return eSIxERR_FAIL;

		eErr = silib_desc_util_setPrivateDataInfo(pstDes->pstPrivateDataInfo, p, (HUINT8)nLen);
	}

	return eErr;
}

static SIxError_e copySupplementaryAudioDes(SIxSupplementaryAudioDes_t *pstDest, const SIxSupplementaryAudioDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSupplementaryAudioDes_t));
		pstDest->pstPrivateDataInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstPrivateDataInfo)
		{
			pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
			if (NULL == pstDest->pstPrivateDataInfo)
				eErr = eSIxERR_FAIL;
		}
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY (TAG = 0x07)
 ************************************************************************/
//NetChangeNotify
static void initNetChangeNotifyDes(SIxNetChangeNotifyDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxNetChangeNotifyDes_t));
}

static void initNetChangeNotifyInfo(SIxNetChangeNotifyCellInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxNetChangeNotifyCellInfo_t));
	pstInfo->usCellId = 0xFFFF;
}

static void initNetChangeNotifySubInfo(SIxNetChangeNotifySubInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxNetChangeNotifySubInfo_t));
}

static SIxNetChangeNotifyDes_t *allocNetChangeNotifyDes(void)
{
	return (SIxNetChangeNotifyDes_t *)silib_desc_alloc(sizeof(SIxNetChangeNotifyDes_t));
}

static SIxNetChangeNotifyCellInfo_t *allocNetChangeNotifyInfo(void)
{
	SIxNetChangeNotifyCellInfo_t	*pstInfo;

	pstInfo = (SIxNetChangeNotifyCellInfo_t *)silib_desc_alloc(sizeof(SIxNetChangeNotifyCellInfo_t));
	if (pstInfo)
	{
		initNetChangeNotifyInfo(pstInfo);
	}

	return pstInfo;
}

static SIxNetChangeNotifySubInfo_t *allocNetChangeNotifySubInfo(void)
{
	SIxNetChangeNotifySubInfo_t	*pstSubInfo;

	pstSubInfo = (SIxNetChangeNotifySubInfo_t *)silib_desc_alloc(sizeof(SIxNetChangeNotifySubInfo_t));
	if (pstSubInfo)
	{
		initNetChangeNotifySubInfo(pstSubInfo);
	}

	return pstSubInfo;
}

static void freeNetChangeNotifyInfo(SIxNetChangeNotifyCellInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	FREE_INFO_LIST(pstInfo->pstInfo, SIxNetChangeNotifySubInfo_t);
	silib_desc_free(pstInfo);
}

static void freeNetChangeNotifyDes(SIxNetChangeNotifyDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNetChangeNotifyCellInfo_t, freeNetChangeNotifyInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseNetChangeNotifyDes(HUINT8 *p, SIxNetChangeNotifyDes_t *pstDes)
{
	HUINT8				ucLoopLen;
	HUINT8				ucNetChNotifyLen;
	HUINT16				usTmpValue;
	SIxNetChangeNotifyCellInfo_t	*pstNewInfo = NULL, *pstPrevInfo = NULL;
	SIxNetChangeNotifySubInfo_t	*pstNewSubInfo = NULL, *pstPrevSubInfo = NULL;


	HxLOG_Info("parseExtNetworkChangeNotifyDes () Entered!\n");

	p++;			// Tag
	ucNetChNotifyLen = *p++;

	if (ucNetChNotifyLen < 3)
	{
		HxLOG_Print("[WARN] parseExtNetworkChangeNotifyDes Invalid Length error(Len:%d)\n",ucNetChNotifyLen);
		HxLOG_Info(" parseExtNetworkChangeNotifyDes () Leaved!\n");
		return eSIxERR_INVALID;
	}

	while (ucNetChNotifyLen > 0 && ucNetChNotifyLen < MAX_DESCRIPTOR_LIMIT)
	{
		/* remain size check */
		if (ucNetChNotifyLen < 3)
		{
			p += ucNetChNotifyLen;
			break;
		}

		/** < alloc Info */
		pstNewInfo = allocNetChangeNotifyInfo();
		if (NULL == pstNewInfo)
		{
			HxLOG_Print("[WARN] parseExtNetworkChangeNotifyDes Alloc error\n");
			HxLOG_Info(" parseExtNetworkChangeNotifyDes () Leaved!\n");
			return eSIxERR_NOT_ENOUGH_MEMORY;
		}
		pstNewInfo->usCellId = HxMACRO_Get16Bit(p); p += 2; ucNetChNotifyLen -= 2;
		HxLOG_Print("\t\t(cell id = 0x%X)\n", pstNewInfo->usCellId);
		ucLoopLen = *p++; ucNetChNotifyLen--;
		pstNewInfo->pstInfo = NULL;
		pstNewInfo->pstNext = NULL;
		pstPrevSubInfo = NULL;

		while (ucLoopLen > 0 && ucLoopLen < MAX_DESCRIPTOR_LIMIT)
		{
			/* remain size check */
			if (ucLoopLen < 12)
			{
				p += ucLoopLen; ucNetChNotifyLen -= ucLoopLen;
				break;
			}

			/** < alloc Info */
			pstNewSubInfo = allocNetChangeNotifySubInfo();
			if (NULL == pstNewSubInfo)
			{
				freeNetChangeNotifyInfo(pstNewInfo);
				pstNewInfo = NULL;
				HxLOG_Print("[WARN] parseExtT2DeliverySystemDes Alloc error\n");
				HxLOG_Info(" parseExtT2DeliverySystemDes () Leaved!\n");
				return eSIxERR_NOT_ENOUGH_MEMORY;
			}
			pstNewSubInfo->pstNext = NULL;
			pstNewSubInfo->ucNetChangeId = *p++;
			pstNewSubInfo->ucNetChangeVer = *p++;
			HxSTD_MemCopy(pstNewSubInfo->ucStartTimeOfChange, p, 5); p += 5;
			pstNewSubInfo->ulBcdChangeDur = HxMACRO_Get24Bit(p); p += 3;

			usTmpValue = HxMACRO_Get16Bit(p); p += 2;
			pstNewSubInfo->ucReceiverCategory = (HUINT8)((usTmpValue >> 13) & 0x7);
			pstNewSubInfo->ucInvariantTsPresent = (HUINT8)((usTmpValue >> 12) & 0x1);
			pstNewSubInfo->ucChangeType = (HUINT8)((usTmpValue >> 8) & 0xF);
			pstNewSubInfo->ucMessageId = (HUINT8)(usTmpValue & 0xFF);
			ucLoopLen -= 12;ucNetChNotifyLen -= 12;

			HxLOG_Print("\t\t\t(NetChangeId = 0x%X NetChangeVer:0x%X)\n", pstNewSubInfo->ucNetChangeId, pstNewSubInfo->ucNetChangeVer);
			HxLOG_Print("\t\t\t(StartTime = 0x%X 0x%X 0x%X 0x%X 0x%X)(Dur BCD:0x%X)\n",
				pstNewSubInfo->ucStartTimeOfChange[0],pstNewSubInfo->ucStartTimeOfChange[1],pstNewSubInfo->ucStartTimeOfChange[2],pstNewSubInfo->ucStartTimeOfChange[3],pstNewSubInfo->ucStartTimeOfChange[4],pstNewSubInfo->ulBcdChangeDur);
			HxLOG_Print("\t\t\t(ReceiverCategory = 0x%X InvariantTsPresent:0x%X ChangeType:0x%X MessageId:0x%X)\n",
				pstNewSubInfo->ucReceiverCategory,pstNewSubInfo->ucInvariantTsPresent,pstNewSubInfo->ucChangeType,pstNewSubInfo->ucMessageId);

			if (1 == pstNewSubInfo->ucInvariantTsPresent)
			{
				pstNewSubInfo->usInvariantTsId = HxMACRO_Get16Bit(p); p += 2;
				pstNewSubInfo->usInvariantOnId = HxMACRO_Get16Bit(p); p += 2;
				ucLoopLen -= 4;ucNetChNotifyLen -= 4;
				HxLOG_Print("\t\t\t(Invariant TsId = 0x%X OnId:0x%X)\n",
					pstNewSubInfo->usInvariantTsId,pstNewSubInfo->usInvariantOnId);
			}
			LINK_DES_NODE(pstNewInfo->pstInfo, pstPrevSubInfo, pstNewSubInfo);
		}
		LINK_DES_NODE(pstDes->pstInfo, pstPrevInfo, pstNewInfo);
	}

	HxLOG_Info(" parseExtNetworkChangeNotifyDes () Leaved!\n");
	return eSIxERR_OK;
}

static SIxError_e copyNetChangeNotifyDes(SIxNetChangeNotifyDes_t *pstDest, const SIxNetChangeNotifyDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxNetChangeNotifyCellInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNetChangeNotifyDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			SIxNetChangeNotifySubInfo_t	*pstSourceSubInfo, *pstPrevSubInfo, *pstNewSubInfo = NULL;

			/** < Descriptor 데이타 구성. */
			GET_PREV_INFO(pstPrevInfo, SIxNetChangeNotifyCellInfo_t, pstDest);
			pstNewInfo = allocNetChangeNotifyInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemSet(pstNewInfo, 0, sizeof(SIxNetChangeNotifyCellInfo_t));
			pstSourceSubInfo = pstSourceInfo->pstInfo;
			while (pstSourceSubInfo)
			{
				GET_PREV_INFO(pstPrevSubInfo, SIxNetChangeNotifySubInfo_t, pstNewInfo);

				pstNewSubInfo = allocNetChangeNotifySubInfo();
				if (NULL == pstNewSubInfo)
				{
					freeNetChangeNotifyInfo(pstNewInfo);
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewSubInfo, pstSourceSubInfo, sizeof(SIxNetChangeNotifySubInfo_t));
				pstNewSubInfo->pstNext = NULL;
				LINK_INFO_NODE(pstNewInfo->pstInfo, pstNewSubInfo, NULL, pstPrevSubInfo);

				pstSourceSubInfo = pstSourceSubInfo->pstNext;
			}

			pstNewInfo->usCellId = pstSourceInfo->usCellId;
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDest->pstInfo, SIxNetChangeNotifyCellInfo_t, freeNetChangeNotifyInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_EXT_MESSAGE (TAG = 0x08)
 ************************************************************************/
//Message
static void initMessageDes(SIxMessageDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxMessageDes_t));
}

static SIxMessageDes_t *allocMessageDes(void)
{
	return (SIxMessageDes_t *)silib_desc_alloc(sizeof(SIxMessageDes_t));
}

static void freeMessageDes(SIxMessageDes_t *pstDes)
{
	silib_desc_free(pstDes->pszMessageText);
	silib_desc_free(pstDes);
}

static void setMessageDes(SIxMessageDes_t *pstDes, const HUINT8 *pszIso639LangCode, const HUINT8 *pszText)
{
	HxSTD_MemCopy(pstDes->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstDes->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';

	if(pszText != NULL)
	{
		HxSTD_MemCopy(pstDes->pszMessageText, pszText, silib_desc_util_dvbStrlen(pszText));
		pstDes->pszMessageText[silib_desc_util_dvbStrlen(pszText)] = '\0';
	}
	else
	{
		pstDes->pszMessageText = NULL;
	}
}

static SIxError_e parseMessageDes(HUINT8 *p, SIxMessageDes_t *pstDes)
{
	HUINT32				nDesLen;
	HUINT8				szIso639LangCode[4], *pszText = NULL;
	HUINT32				nTempNameLen;

	HxLOG_Info("parseExtMessageDes () Entered!\n");

	p++;							// Tag
	nDesLen = (HUINT32)*p++;		// Length

	pstDes->ucMessageId = *p++; nDesLen--;

	HxLOG_Print("\t\t(Message Id = 0x%X)\n", pstDes->ucMessageId);

	/** < ISO 639 language code (3byte) */
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode[3] = '\0';
	silib_desc_util_makeCorrct639Code(szIso639LangCode);
	p += 3;
	nDesLen -= 3;
	HxLOG_Print("\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

	/** < network name */
	nTempNameLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nDesLen, (const HUINT8 *)p, (HUINT8 **)&pszText);
	if (NULL == pszText)
		goto err_exit;

	p += nTempNameLen;
	nDesLen -= nTempNameLen;

	HxLOG_Print("\t\t(Message = %s)\n", pszText);

	pstDes->pszMessageText = silib_desc_alloc(nTempNameLen+1);
	if (NULL == pstDes->pszMessageText)
		goto err_exit;

	setMessageDes(pstDes,szIso639LangCode, pszText);
	silib_desc_free( pszText );

	HxLOG_Info(" parseExtMessageDes () Leaved!\n");

	return eSIxERR_OK;

err_exit:
	silib_desc_free( pszText );
	HxLOG_Info(" parseExtMessageDes () Error Leaved!\n");

	return eSIxERR_FAIL;

}

static SIxError_e copyMessageDes(SIxMessageDes_t *pstDest, const SIxMessageDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMessageDes_t));
		pstDest->pszMessageText = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pszMessageText)
		{
			HUINT32	ulSize = silib_desc_util_dvbStrlen(pstSource->pszMessageText);

			eErr = eSIxERR_FAIL;
			pstDest->pszMessageText = (HUINT8 *)silib_desc_alloc(ulSize+1);
			if (pstDest->pszMessageText)
			{
				setMessageDes(pstDest, pstSource->szIso639LangCode, pstSource->pszMessageText);
				eErr = eSIxERR_OK;
			}
		}
		else
		{
			setMessageDes(pstDest, pstSource->szIso639LangCode, NULL);
		}
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_EXT_TARGET_REG (TAG = 0x09)
 ************************************************************************/
//TargetRegion
static void initTargetRegionDes(SIxTargetRegionDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxTargetRegionDes_t));
}

static void initTargetRegionInfo(SIxTargetRegionInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxTargetRegionInfo_t));
}

static SIxTargetRegionDes_t *allocTargetRegionDes(void)
{
	return (SIxTargetRegionDes_t *)silib_desc_alloc(sizeof(SIxTargetRegionDes_t));
}

static SIxTargetRegionInfo_t *allocTargetRegionInfo(void)
{
	SIxTargetRegionInfo_t	*pstInfo;

	pstInfo = (SIxTargetRegionInfo_t *)silib_desc_alloc(sizeof(SIxTargetRegionInfo_t));
	if (pstInfo)
	{
		initTargetRegionInfo(pstInfo);
	}

	return pstInfo;
}

static void freeTargetRegionInfo(SIxTargetRegionInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void freeTargetRegionDes(SIxTargetRegionDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxTargetRegionInfo_t, freeTargetRegionInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseTargetRegionDes(HUINT8 *p, SIxTargetRegionDes_t *pstDes)
{
	HUINT8						ucTmpValue;
	HUINT8						ucTargetRegionLen;
	SIxTargetRegionInfo_t		*pstNewInfo, *pstPrevInfo = NULL;

	HxLOG_Info("parseExtTargetRegionDes () Entered!\n");

	p++;						// Tag
	ucTargetRegionLen = *p++;	// Length

	if (ucTargetRegionLen < 3)
	{
		HxLOG_Print("[WARN] parseExtTargetRegionDes Invalid Length error(Len:%d)\n",ucTargetRegionLen);
		HxLOG_Info(" parseExtTargetRegionDes () Leaved!\n");
		return eSIxERR_INVALID;
	}

	HxSTD_MemCopy(pstDes->ucCountryCode, p, 3);
	pstDes->ucCountryCode[3] = '\0'; p += 3; ucTargetRegionLen -=3;
	pstDes->pstInfo = NULL;

	while (ucTargetRegionLen > 0)
	{
		/* remain size check */
		if (ucTargetRegionLen < 1)
		{
			p += ucTargetRegionLen;
			break;
		}

		/** < alloc Info */
		pstNewInfo = allocTargetRegionInfo();
		if (NULL == pstNewInfo)
		{
			HxLOG_Print("[WARN] parseExtTargetRegionDes Alloc error\n");
			HxLOG_Info(" parseExtTargetRegionDes () Leaved\n");
			return eSIxERR_NOT_ENOUGH_MEMORY;
		}
		ucTmpValue = HxMACRO_Get8Bit(p); p++; ucTargetRegionLen--;

		pstNewInfo->ucCountryCodeFlag = (HUINT8)( ( ucTmpValue >> 2 ) & 0x01);
		pstNewInfo->ucRegionDepth = (HUINT8)( ( ucTmpValue ) & 0x03);

		HxLOG_Print("\tCountryCodeFlag:%d RegionDepth:%d\n",pstNewInfo->ucCountryCodeFlag,pstNewInfo->ucRegionDepth);

		if ( 1 ==  pstNewInfo->ucCountryCodeFlag )
		{
			if (ucTargetRegionLen < 3)
			{
				p += ucTargetRegionLen;
				break;
			}
			HxSTD_MemCopy(pstNewInfo->ucCountryCode, p, 3);
			pstNewInfo->ucCountryCode[3] = '\0'; p += 3; ucTargetRegionLen -=3;
			HxLOG_Print("\t\tCountryCode : %s\n",pstNewInfo->ucCountryCode);
		}

		if ( 1 <=  pstNewInfo->ucRegionDepth)
		{
			pstNewInfo->stTargetRegionInfo.ucPrimaryRegionCode = HxMACRO_Get8Bit(p); p++; ucTargetRegionLen--;
			HxLOG_Print("\t\tPrimaryRegionCode : 0x%X\n",pstNewInfo->stTargetRegionInfo.ucPrimaryRegionCode);
			if ( 2 <=  pstNewInfo->ucRegionDepth)
			{
				pstNewInfo->stTargetRegionInfo.ucSecondaryRegionCode = HxMACRO_Get8Bit(p); p++; ucTargetRegionLen--;
				HxLOG_Print("\t\tucSecondaryRegionCode : 0x%X\n",pstNewInfo->stTargetRegionInfo.ucSecondaryRegionCode);
			}
			if ( 3 <=  pstNewInfo->ucRegionDepth)
			{
				pstNewInfo->stTargetRegionInfo.usTertiaryRegionCode = HxMACRO_Get16Bit(p); p += 2; ucTargetRegionLen -= 2;
				HxLOG_Print("\t\tusTertiaryRegionCode : 0x%X\n",pstNewInfo->stTargetRegionInfo.usTertiaryRegionCode);
			}
		}

		LINK_DES_NODE(pstDes->pstInfo, pstPrevInfo, pstNewInfo);
	}

	HxLOG_Info(" parseExtTargetRegionDes () Leaved\n");

	return eSIxERR_OK;
}

static SIxError_e copyTargetRegionDes(SIxTargetRegionDes_t *pstDest, const SIxTargetRegionDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxTargetRegionInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTargetRegionDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			/** < Descriptor 데이타 구성. */
			GET_PREV_INFO(pstPrevInfo, SIxTargetRegionInfo_t, pstDest);
			pstNewInfo = allocTargetRegionInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxTargetRegionInfo_t));
			pstNewInfo->pstNext = NULL;

			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDest->pstInfo, SIxTargetRegionInfo_t, freeTargetRegionInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_EXT_TARGET_REG_NAME (TAG = 0x0A)
 ************************************************************************/
//TargetRegionName
static void initTargetRegionNameDes(SIxTargetRegionNameDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxTargetRegionNameDes_t ));
}

static void initTargetRegionNameInfo(SIxTargetRegionNameInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxTargetRegionNameInfo_t ));
}

static SIxTargetRegionNameDes_t *allocTargetRegionNameDes(void)
{
	return (SIxTargetRegionNameDes_t *)silib_desc_alloc(sizeof(SIxTargetRegionNameDes_t));
}

static SIxTargetRegionNameInfo_t *allocTargetRegionNameInfo(void)
{
	SIxTargetRegionNameInfo_t	*pstInfo;

	pstInfo = (SIxTargetRegionNameInfo_t *)silib_desc_alloc(sizeof(SIxTargetRegionNameInfo_t));
	if (pstInfo)
	{
		initTargetRegionNameInfo(pstInfo);
	}

	return ((SIxTargetRegionNameInfo_t *)pstInfo);
}

static void freeTargetRegionNameInfo(SIxTargetRegionNameInfo_t *pstInfo)
{
	silib_desc_free(pstInfo->pszRegionName);
	silib_desc_free(pstInfo);
}

static void freeTargetRegionNameDes(SIxTargetRegionNameDes_t *pstDes)
{
	if (NULL == pstDes)		return;
	
	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxTargetRegionNameInfo_t, freeTargetRegionNameInfo);
	silib_desc_free(pstDes);
}

static void setTargetRegionNameInfo(SIxTargetRegionNameInfo_t *pstInfo,
			HUINT8 ucRegionDepth, HUINT8 ucRegionNameLen, HUINT8 *pszRegionName, SIxTargetRegionCode_t *pstTargetRegionInfo)
{
	pstInfo->ucRegionDepth		= ucRegionDepth;
	pstInfo->ucRegionNameLen	= ucRegionNameLen;
	if (0 < ucRegionNameLen && NULL != pszRegionName)
	{
		HxSTD_MemCopy(pstInfo->pszRegionName, pszRegionName, sizeof(HUINT8)*ucRegionNameLen);
		pstInfo->pszRegionName[pstInfo->ucRegionNameLen] = '\0';
	}

	if (pstTargetRegionInfo)
	{
		HxSTD_MemCopy(&pstInfo->stTargetRegionInfo, pstTargetRegionInfo, sizeof(SIxTargetRegionCode_t));
	}
	else
	{
		HxSTD_MemSet(&pstInfo->stTargetRegionInfo, 0, sizeof(SIxTargetRegionCode_t));
	}
}

static SIxError_e parseTargetRegionNameDes(HUINT8 *p, SIxTargetRegionNameDes_t *pstDes)
{
	SIxError_e					 eErr;
	HUINT8						 ucTmpValue;
	HUINT8						 ucTargetRegNameLen;
	HUINT32						 nTempTextLen;
	SIxTargetRegionNameInfo_t	*pstNewInfo,*pstPrevInfo = NULL;

	HxLOG_Info("parseExtTargetRegionNameDes () Entered\n");

	p++;						// Tag
	ucTargetRegNameLen = *p++;	// Length

	if (ucTargetRegNameLen < 6)
	{
		HxLOG_Print("[WARN] parseExtTargetRegionNameDes Invalid Length error(Len:%d)\n",ucTargetRegNameLen);
		HxLOG_Info(" parseExtTargetRegionNameDes () Leaved\n");
		return eSIxERR_INVALID;
	}

	HxSTD_MemCopy(pstDes->ucCountryCode,p,3);
	pstDes->ucCountryCode[3] = '\0'; p += 3; ucTargetRegNameLen -=3;

	HxSTD_MemCopy(pstDes->ucIso639LangCode,p,3);
	pstDes->ucIso639LangCode[3] = '\0'; p += 3; ucTargetRegNameLen -=3;
	pstDes->pstInfo = NULL;

	eErr = eSIxERR_OK;
	while (ucTargetRegNameLen > 0)
	{
		/* remain size check */
		if (ucTargetRegNameLen < 3)
		{
			p += ucTargetRegNameLen;
			break;
		}

		/** < alloc Info */
		pstNewInfo = allocTargetRegionNameInfo();
		if (NULL == pstNewInfo)
		{
			HxLOG_Print("[WARN] parseExtTargetRegionNameDes Alloc error\n");
			HxLOG_Info(" parseExtTargetRegionNameDes () Leaved\n");
			goto err_exit;
		}
		ucTmpValue = HxMACRO_Get8Bit(p); p++; ucTargetRegNameLen--;

		pstNewInfo->ucRegionDepth = (HUINT8)( ( ucTmpValue >> 6 ) & 0x03 );
		pstNewInfo->ucRegionNameLen = (HUINT8)( ucTmpValue & 0x3F );

		HxLOG_Print("\tRegionDepth:%d RegionNameLen:%d\n",pstNewInfo->ucRegionDepth,pstNewInfo->ucRegionNameLen);
		nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)pstDes->ucIso639LangCode, pstDes->stHead.stParam.eTextEnc, pstNewInfo->ucRegionNameLen, (const HUINT8 *)p, (HUINT8 **)&pstNewInfo->pszRegionName);
		if (NULL == pstNewInfo->pszRegionName)
		{
			goto err_exit;
		}

		p += pstNewInfo->ucRegionNameLen;
		ucTargetRegNameLen -= pstNewInfo->ucRegionNameLen;

		// 실제 스트링 길이로 변환.
		pstNewInfo->ucRegionNameLen = nTempTextLen;
		HxLOG_Print("\t--Region Name : %s\n",pstNewInfo->pszRegionName);

		pstNewInfo->stTargetRegionInfo.ucPrimaryRegionCode = HxMACRO_Get8Bit(p); p++; ucTargetRegNameLen--;
		HxLOG_Print("\t\tucPrimaryRegionCode : 0x%X\n",pstNewInfo->stTargetRegionInfo.ucPrimaryRegionCode);
		if ( 2 <=  pstNewInfo->ucRegionDepth)
		{
			pstNewInfo->stTargetRegionInfo.ucSecondaryRegionCode = HxMACRO_Get8Bit(p); p++; ucTargetRegNameLen--;
			HxLOG_Print("\t\tucSecondaryRegionCode : 0x%X\n",pstNewInfo->stTargetRegionInfo.ucSecondaryRegionCode);
			if ( 3 <=  pstNewInfo->ucRegionDepth)
			{
				pstNewInfo->stTargetRegionInfo.usTertiaryRegionCode = HxMACRO_Get16Bit(p); p += 2; ucTargetRegNameLen -= 2;
				HxLOG_Print("\t\tusTertiaryRegionCode : 0x%X\n",pstNewInfo->stTargetRegionInfo.usTertiaryRegionCode);
			}
		}

		LINK_DES_NODE(pstDes->pstInfo, pstPrevInfo, pstNewInfo);
	}

	HxLOG_Info(" parseExtTargetRegionNameDes () Leaved\n");
	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxTargetRegionNameInfo_t, freeTargetRegionNameInfo);
	pstDes->pstInfo = NULL;
	return eSIxERR_FAIL;
}

static SIxError_e copyTargetRegionNameDes(SIxTargetRegionNameDes_t *pstDest, const SIxTargetRegionNameDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxTargetRegionNameInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTargetRegionNameDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			/** < Descriptor 데이타 구성. */
			GET_PREV_INFO(pstPrevInfo, SIxTargetRegionNameInfo_t, pstDest);
			pstNewInfo = allocTargetRegionNameInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			if (0 < pstSourceInfo->ucRegionNameLen && NULL != pstSourceInfo->pszRegionName)
			{
				pstNewInfo->pszRegionName = silib_desc_alloc(pstSourceInfo->ucRegionNameLen+1);
			}
			else
			{
				pstNewInfo->pszRegionName = NULL;
			}
			setTargetRegionNameInfo(pstNewInfo, pstSourceInfo->ucRegionDepth
											, pstSourceInfo->ucRegionNameLen
											, pstSourceInfo->pszRegionName
											, &pstSourceInfo->stTargetRegionInfo);
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	FREE_INFO_LIST_EX(pstDest->pstInfo, SIxTargetRegionNameInfo_t, freeTargetRegionNameInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_EXT_URI_LINKAGE (TAG = 0x13)
 ************************************************************************/
//URI_LINKAGE_DESC
static void initUriLinkageDes(SIxUriLinkageDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxUriLinkageDes_t));
}

static SIxUriLinkageDes_t *allocUriLinkageDes(void)
{
	return (SIxUriLinkageDes_t *)silib_desc_alloc(sizeof(SIxUriLinkageDes_t));
}

static void freeUriLinkageDes(SIxUriLinkageDes_t *pstDes)
{
	if (NULL == pstDes)		return;
	
	silib_desc_free(pstDes->pszUriChar);
	silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}

static void setUriLinkageDes(SIxUriLinkageDes_t *pstDes, const HUINT8 *pszText)
{
	if(pszText != NULL)
	{
		HxSTD_MemCopy(pstDes->pszUriChar, pszText, silib_desc_util_dvbStrlen(pszText));
		pstDes->pszUriChar[silib_desc_util_dvbStrlen(pszText)] = '\0';
		HxLOG_Print("[setUriLinkageDes] pstDes->pszUriChar : %s\n",pstDes->pszUriChar);
	}
	else
	{
		pstDes->pszUriChar = NULL;
	}
}

static SIxError_e parseUriLinkageDes(HUINT8 *p, SIxUriLinkageDes_t *pstDes)
{
	HUINT32				nDesLen;
	HUINT8				*pszText = NULL;
	HUINT32				nTempNameLen,ulTempUriCharLen;
	HERROR				eErr = ERR_FAIL;

	HxLOG_Print("parseUriLinkageDes () Entered!");

	p++;							// Tag
	nDesLen = (HUINT32)*p++;		// Length

	pstDes->ucUri_linkage_type = *p++; nDesLen--;	//
	HxLOG_Print("\t\t(ucUri_linkage_type = 0x%X)\n", pstDes->ucUri_linkage_type);
	
	pstDes->ucUriCharLen = *p++; nDesLen--;	//
	
	nTempNameLen = pstDes->ucUriCharLen;
	HxLOG_Print("\t\t(ucUriCharLen = 0x%X)\n", pstDes->ucUriCharLen);

	/* remain size check */
	if (nTempNameLen < 3)
	{
		p += nTempNameLen;
		goto err_exit;
	}

	ulTempUriCharLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, nTempNameLen, (const HUINT8 *)p, &pszText);
	/** < Descriptor 데이타 구성. */
	pstDes->pszUriChar = silib_desc_util_allocStr(ulTempUriCharLen+1);
	if (NULL == pstDes->pszUriChar)
		goto err_exit;

	setUriLinkageDes(pstDes,pszText);
	silib_desc_free( pszText );
	
	p += nTempNameLen;
	nDesLen -= nTempNameLen;


	if ( (0x00 ==  pstDes->ucUri_linkage_type) || (0x01 ==  pstDes->ucUri_linkage_type))
	{
		pstDes->ucMin_polling_interval = HxMACRO_Get16Bit(p); p += 2; nDesLen -= 2;
		HxLOG_Print("\t\tpstDes->ucMin_polling_interval : 0x%X\n",pstDes->ucMin_polling_interval);
	}

	eErr = ERR_OK;

	HxLOG_Print("\t\nDesLen : %d\n",nDesLen);
	if (0 < nDesLen)
	{
		pstDes->pstPrivateDataInfo = silib_desc_util_allocPrivateDataInfo((HUINT8)nDesLen);
		if (NULL == pstDes->pstPrivateDataInfo)
			return eSIxERR_FAIL;

		eErr = silib_desc_util_setPrivateDataInfo(pstDes->pstPrivateDataInfo, p, (HUINT8)nDesLen);
	}

	HxLOG_Print(" parseUriLinkageDes () Leaved!");
	return eErr;

err_exit:
	silib_desc_free( pszText );
	HxLOG_Print(" parseUriLinkageDes () Error Leaved!");

	return eSIxERR_FAIL;

}

static SIxError_e copyUriLinkageDes(SIxUriLinkageDes_t *pstDest, const SIxUriLinkageDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxUriLinkageDes_t));
		pstDest->pszUriChar = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pszUriChar)
		{
			HUINT32	ulSize = silib_desc_util_dvbStrlen(pstSource->pszUriChar);

			eErr = eSIxERR_FAIL;
			pstDest->pszUriChar = (HUINT8 *)silib_desc_alloc(ulSize+1);
			if (pstDest->pszUriChar)
			{
				setUriLinkageDes(pstDest, pstSource->pszUriChar);

				eErr = eSIxERR_OK;
			}
			
		}
		else
		{
			setUriLinkageDes(pstDest, NULL);
		}
		
		if (pstSource->pstPrivateDataInfo)
		{
			pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
			if (NULL == pstDest->pstPrivateDataInfo)
				eErr = eSIxERR_FAIL;
		}
	}

	return eErr;
}

#if 0
static HUINT8 *silib_desc_allocInfo_DvbExt(const SIxDescriptorTag_e eTag, HUINT16 usSubTag)
{
	HUINT8	*pstNewInfo = NULL;

	switch (eTag)
	{
	case eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM:			/** < tag = 0x04 */
		pstNewInfo = (HUINT8 *)allocT2DeliveryCellInfo();
		break;
	case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:		/** < tag = 0x07 */
		if (0 == usSubTag)
			pstNewInfo = (HUINT8 *)allocNetChangeNotifyInfo();
		else if (1 == usSubTag)
			pstNewInfo = (HUINT8 *)allocNetChangeNotifySubInfo();
		break;
	case eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO:			/** < tag = 0x06 */
		pstNewInfo = NULL;
		break;
	case eSIxDESCTAG_EXT_TARGET_REG:					/** < tag = 0x09 */
		pstNewInfo = (HUINT8 *)allocTargetRegionInfo();
		break;
	case eSIxDESCTAG_EXT_TARGET_REG_NAME:				/** < tag = 0x0A */
		pstNewInfo = (HUINT8 *)allocTargetRegionNameInfo();
		break;
	default :
		HxLOG_Print("[ERROR] alloc fail Tag = 0x%x, SubTag = 0x%x\n", eTag, usSubTag);
		break;
	}

	return ((HUINT8 *)pstNewInfo);
}

static void silib_desc_freeInfo_DvbExt(const SIxDescriptorTag_e eTag, HUINT16 usSubTag, HUINT8 *pucDesInfo)
{
	if (NULL == pucDesInfo)
	{
		return;
	}

	switch (eTag)
	{
	case eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM:
		freeT2DeliveryCellInfo((SIxT2DeliveryCellInfo_t *)pucDesInfo);
		break;
	case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:
		if (0 == usSubTag) {
			freeNetChangeNotifyInfo((SIxNetChangeNotifyCellInfo_t *)pucDesInfo);
		} else if (1 == usSubTag) {
			FREE_INFO_LIST((SIxNetChangeNotifySubInfo_t *)pucDesInfo, SIxNetChangeNotifySubInfo_t);
		}
		break;
#if 0
	case eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO:
		break;
	case eSIxDESCTAG_EXT_MESSAGE:
		break;
#endif
	case eSIxDESCTAG_EXT_TARGET_REG:					/** < tag = 0x09 */
		freeTargetRegionInfo((SIxTargetRegionInfo_t *)pucDesInfo);
		break;
	case eSIxDESCTAG_EXT_TARGET_REG_NAME:				/** < tag = 0x0A */
		freeTargetRegionNameInfo((SIxTargetRegionNameInfo_t *)pucDesInfo);
		break;
	default :
		HxLOG_Print("[ERROR] Free fail Tag = 0x%x\n", eTag);
		break;
	}
}
#endif

/* eSIxDESCTAG_EXT_IMG_ICON							:(0x00)	*/
//ImageIcon
static SIxDescriptor_t *silib_desc_allocImageIconDes(void)
{
	SIxImageIconDes_t	*pstDes;

	pstDes = allocImageIconDes();
	if (pstDes)
	{
		initImageIconDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeImageIconDes(SIxDescriptor_t *pstDes)
{
	freeImageIconDes((SIxImageIconDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseImageIconDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseImageIconDes((HUINT8 *)pucRawData, (SIxImageIconDes_t *)pstDes);
}

static SIxError_e silib_desc_copyImageIconDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyImageIconDes((SIxImageIconDes_t *)pstDest, (SIxImageIconDes_t *)pstSource);
}

static void silib_desc_printImageIconDes(const SIxDescriptor_t *pstDes)
{
	//(SIxImageIconDes_t *)pstDes;
}

/* eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM					:(0x04)	*/
//T2DeliverySystem
static SIxDescriptor_t *silib_desc_allocT2DeliverySystemDes(void)
{
	SIxT2DeliverySystemDes_t	*pstDes;

	pstDes = allocT2DeliverySystemDes();
	if (pstDes)
	{
		initT2DeliverySystemDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeT2DeliverySystemDes(SIxDescriptor_t *pstDes)
{
	freeT2DeliverySystemDes((SIxT2DeliverySystemDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseT2DeliverySystemDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseT2DeliverySystemDes((HUINT8 *)pucRawData, (SIxT2DeliverySystemDes_t *)pstDes);
}

static SIxError_e silib_desc_copyT2DeliverySystemDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyT2DeliverySystemDes((SIxT2DeliverySystemDes_t *)pstDest, (SIxT2DeliverySystemDes_t *)pstSource);
}

static void silib_desc_printT2DeliverySystemDes(const SIxDescriptor_t *pstDes)
{
//	(SIxT2DeliverySystemDes_t *)pstDes;
}


/* eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO					:(0x06)	*/
//SupplementaryAudio
static SIxDescriptor_t *silib_desc_allocSupplementaryAudioDes(void)
{
	SIxSupplementaryAudioDes_t	*pstDes;

	pstDes = allocSupplementaryAudioDes();
	if (pstDes)
	{
		initSupplementaryAudioDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeSupplementaryAudioDes(SIxDescriptor_t *pstDes)
{
	freeSupplementaryAudioDes((SIxSupplementaryAudioDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSupplementaryAudioDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseSupplementaryAudioDes((HUINT8 *)pucRawData, (SIxSupplementaryAudioDes_t *)pstDes);
}

static SIxError_e silib_desc_copySupplementaryAudioDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySupplementaryAudioDes((SIxSupplementaryAudioDes_t *)pstDest, (SIxSupplementaryAudioDes_t *)pstSource);
}

static void silib_desc_printSupplementaryAudioDes(const SIxDescriptor_t *pstDes)
{
	//(SIxSupplementaryAudioDes_t *)pstDes;
}


/* eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY					:(0x07)	*/
//NetChangeNotify
static SIxDescriptor_t *silib_desc_allocNetChangeNotifyDes(void)
{
	SIxNetChangeNotifyDes_t	*pstDes;

	pstDes = allocNetChangeNotifyDes();
	if (pstDes)
	{
		initNetChangeNotifyDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeNetChangeNotifyDes(SIxDescriptor_t *pstDes)
{
	freeNetChangeNotifyDes((SIxNetChangeNotifyDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseNetChangeNotifyDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseNetChangeNotifyDes((HUINT8 *)pucRawData, (SIxNetChangeNotifyDes_t *)pstDes);
}

static SIxError_e silib_desc_copyNetChangeNotifyDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyNetChangeNotifyDes((SIxNetChangeNotifyDes_t *)pstDest, (SIxNetChangeNotifyDes_t *)pstSource);
}

static void silib_desc_printNetChangeNotifyDes(const SIxDescriptor_t *pstDes)
{
	//(SIxNetChangeNotifyDes_t *)pstDes;
}

/* eSIxDESCTAG_EXT_MESSAGE							:(0x08)	*/
//Message
static SIxDescriptor_t *silib_desc_allocMessageDes(void)
{
	SIxMessageDes_t	*pstDes;

	pstDes = allocMessageDes();
	if (pstDes)
	{
		initMessageDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeMessageDes(SIxDescriptor_t *pstDes)
{
	freeMessageDes((SIxMessageDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseMessageDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseMessageDes((HUINT8 *)pucRawData, (SIxMessageDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMessageDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMessageDes((SIxMessageDes_t *)pstDest, (SIxMessageDes_t *)pstSource);
}

static void silib_desc_printMessageDes(const SIxDescriptor_t *pstDes)
{
	//(SIxMessageDes_t *)pstDes;
}

/* eSIxDESCTAG_EXT_TARGET_REG							:(0x09)	*/
//TargetRegion
static SIxDescriptor_t *silib_desc_allocTargetRegionDes(void)
{
	SIxTargetRegionDes_t	*pstDes;

	pstDes = allocTargetRegionDes();
	if (pstDes)
	{
		initTargetRegionDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeTargetRegionDes(SIxDescriptor_t *pstDes)
{
	freeTargetRegionDes((SIxTargetRegionDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseTargetRegionDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseTargetRegionDes((HUINT8 *)pucRawData, (SIxTargetRegionDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTargetRegionDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTargetRegionDes((SIxTargetRegionDes_t *)pstDest, (SIxTargetRegionDes_t *)pstSource);
}

static void silib_desc_printTargetRegionDes(const SIxDescriptor_t *pstDes)
{
	//(SIxTargetRegionDes_t *)pstDes;
}

/* eSIxDESCTAG_EXT_TARGET_REG_NAME							:(0x0A)	*/
//TargetRegionName
static SIxDescriptor_t *silib_desc_allocTargetRegionNameDes(void)
{
	SIxTargetRegionNameDes_t	*pstDes;

	pstDes = allocTargetRegionNameDes();
	if (pstDes)
	{
		initTargetRegionNameDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeTargetRegionNameDes(SIxDescriptor_t *pstDes)
{
	freeTargetRegionNameDes((SIxTargetRegionNameDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseTargetRegionNameDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseTargetRegionNameDes((HUINT8 *)pucRawData, (SIxTargetRegionNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTargetRegionNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTargetRegionNameDes((SIxTargetRegionNameDes_t *)pstDest, (SIxTargetRegionNameDes_t *)pstSource);
}

static void silib_desc_printTargetRegionNameDes(const SIxDescriptor_t *pstDes)
{
	//(SIxTargetRegionNameDes_t *)pstDes;
}
/* eSIxDESCTAG_EXT_URI_LINKAGE_DESC							:(0x13)	*/
//UriLinkageDesc
static SIxDescriptor_t *silib_desc_allocUriLinkageDes(void)
{
	SIxUriLinkageDes_t	*pstDes;

	pstDes = allocUriLinkageDes();
	if (pstDes)
	{
		initUriLinkageDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeUriLinkageDes(SIxDescriptor_t *pstDes)
{
	freeUriLinkageDes((SIxUriLinkageDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseUriLinkageDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseUriLinkageDes((HUINT8 *)pucRawData, (SIxUriLinkageDes_t *)pstDes);
}

static SIxError_e silib_desc_copyUriLinkageDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyUriLinkageDes((SIxUriLinkageDes_t *)pstDest, (SIxUriLinkageDes_t *)pstSource);
}

static void silib_desc_printUriLinkageDes(const SIxDescriptor_t *pstDes)
{
	//(SIxUriLinkageDes_t *)pstDes;
}
SIxDescriptor_t *silib_desc_alloc_DvbExt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
	case eSIxDESCTAG_EXT_IMG_ICON:					return silib_desc_allocImageIconDes();
	case eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING:		break;
	case eSIxDESCTAG_EXT_CP:						break;
	case eSIxDESCTAG_EXT_CP_IDENTIFIER:			break;
	case eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM:		return silib_desc_allocT2DeliverySystemDes();
	case eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM:		break;
	case eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO:		return silib_desc_allocSupplementaryAudioDes();
	case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:	return silib_desc_allocNetChangeNotifyDes();
	case eSIxDESCTAG_EXT_MESSAGE:					return silib_desc_allocMessageDes();
	case eSIxDESCTAG_EXT_TARGET_REG:				return silib_desc_allocTargetRegionDes();
	case eSIxDESCTAG_EXT_TARGET_REG_NAME:			return silib_desc_allocTargetRegionNameDes();
	case eSIxDESCTAG_EXT_URI_LINKAGE:				return silib_desc_allocUriLinkageDes();
	default:									break;
	}

	return NULL;
}

SIxError_e silib_desc_free_DvbExt(SIxDescriptor_t *pstDes)
{
	SIxError_e		 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_EXT_IMG_ICON:					eErr = silib_desc_freeImageIconDes(pstDes);				break;
	case eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING:		break;
	case eSIxDESCTAG_EXT_CP:						break;
	case eSIxDESCTAG_EXT_CP_IDENTIFIER:			break;
	case eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM:		eErr = silib_desc_freeT2DeliverySystemDes(pstDes);		break;
	case eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM:		break;
	case eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO:		eErr = silib_desc_freeSupplementaryAudioDes(pstDes);	break;
	case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:	eErr = silib_desc_freeNetChangeNotifyDes(pstDes);		break;
	case eSIxDESCTAG_EXT_MESSAGE:					eErr = silib_desc_freeMessageDes(pstDes);				break;
	case eSIxDESCTAG_EXT_TARGET_REG:				eErr = silib_desc_freeTargetRegionDes(pstDes);			break;
	case eSIxDESCTAG_EXT_TARGET_REG_NAME:			eErr = silib_desc_freeTargetRegionNameDes(pstDes);		break;
	case eSIxDESCTAG_EXT_URI_LINKAGE:				eErr = silib_desc_freeUriLinkageDes(pstDes);			break;
	default:
		break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_DvbExt(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_EXT_IMG_ICON:					eErr = silib_desc_parseImageIconDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING:		break;
	case eSIxDESCTAG_EXT_CP:						break;
	case eSIxDESCTAG_EXT_CP_IDENTIFIER:			break;
	case eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM:		eErr = silib_desc_parseT2DeliverySystemDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM:		break;
	case eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO:		eErr = silib_desc_parseSupplementaryAudioDes(pucRawData, pstDes);	break;
	case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:	eErr = silib_desc_parseNetChangeNotifyDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_EXT_MESSAGE:					eErr = silib_desc_parseMessageDes(pucRawData, pstDes);				break;
	case eSIxDESCTAG_EXT_TARGET_REG:				eErr = silib_desc_parseTargetRegionDes(pucRawData, pstDes);			break;
	case eSIxDESCTAG_EXT_TARGET_REG_NAME:			eErr = silib_desc_parseTargetRegionNameDes(pucRawData, pstDes);		break;
	case eSIxDESCTAG_EXT_URI_LINKAGE:				eErr = silib_desc_parseUriLinkageDes(pucRawData, pstDes);			break;
	default:
		break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_DvbExt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e		 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;
	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_EXT_IMG_ICON:					eErr = silib_desc_copyImageIconDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING:		break;
	case eSIxDESCTAG_EXT_CP:						break;
	case eSIxDESCTAG_EXT_CP_IDENTIFIER:			break;
	case eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM:		eErr = silib_desc_copyT2DeliverySystemDes(pstDest, pstSource);		break;
	case eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM:		break;
	case eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO:		eErr = silib_desc_copySupplementaryAudioDes(pstDest, pstSource);	break;
	case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:	eErr = silib_desc_copyNetChangeNotifyDes(pstDest, pstSource);		break;
	case eSIxDESCTAG_EXT_MESSAGE:					eErr = silib_desc_copyMessageDes(pstDest, pstSource);				break;
	case eSIxDESCTAG_EXT_TARGET_REG:				eErr = silib_desc_copyTargetRegionDes(pstDest, pstSource);			break;
	case eSIxDESCTAG_EXT_TARGET_REG_NAME:			eErr = silib_desc_copyTargetRegionNameDes(pstDest, pstSource);		break;
	case eSIxDESCTAG_EXT_URI_LINKAGE:				eErr = silib_desc_copyUriLinkageDes(pstDest, pstSource);			break;
	default:									break;
	}

	return eErr;
}

SIxError_e silib_desc_print_DvbExt(const SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_DvbExt(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
	case eSIxDESCTAG_EXT_IMG_ICON:					silib_desc_printImageIconDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING:		break;
	case eSIxDESCTAG_EXT_CP:						break;
	case eSIxDESCTAG_EXT_CP_IDENTIFIER:			break;
	case eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM:		silib_desc_printT2DeliverySystemDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM:		break;
	case eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO:		silib_desc_printSupplementaryAudioDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY:	silib_desc_printNetChangeNotifyDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXT_MESSAGE:					silib_desc_printMessageDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXT_TARGET_REG:				silib_desc_printTargetRegionDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXT_TARGET_REG_NAME:			silib_desc_printTargetRegionNameDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXT_URI_LINKAGE:				silib_desc_printUriLinkageDes(pstDes);			eErr = eSIxERR_OK;	break;
	default:									break;
	}

	return eErr;
}

