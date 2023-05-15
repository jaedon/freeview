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
	@file     silib_descparse_nordig.c
	@brief    silib_descparse_nordig.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		:
	Remarks		: 															\n

	Copyright (c) 2014 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static HCHAR *silib_desc_getTagString_Nordig(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
 		ENUM_TO_STR(eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL);
		ENUM_TO_STR(eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2);
 		ENUM_TO_STR(eSIxDESCTAG_NORDIG_DTCP);
		ENUM_TO_STR(eSIxDESCTAG_NORDIG_CONTENT_PROTECTION);
		ENUM_TO_STR(eSIxDESCTAG_TDC_CHANNEL);

		default:
			break;
	}

	return NULL;
}

#define _eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_
/************************************************************************
 * eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL (Tag = 0x83)
 ************************************************************************/
static void initLogicalChannelDes(SIxNordigLogicalChannelDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxNordigLogicalChannelDes_t));
}

static SIxNordigLogicalChannelInfo_t *allocLogicalChannelInfo(void)
{
	return (SIxNordigLogicalChannelInfo_t *)silib_desc_alloc(sizeof(SIxNordigLogicalChannelInfo_t));
}

static SIxNordigLogicalChannelDes_t *allocLogicalChannelDes(void)
{
	return (SIxNordigLogicalChannelDes_t *)silib_desc_alloc(sizeof(SIxNordigLogicalChannelDes_t));
}

static void freeLogicalChannelInfo(SIxNordigLogicalChannelInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeLogicalChannelDes(SIxNordigLogicalChannelDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNordigLogicalChannelInfo_t, freeLogicalChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseLogicalChannelDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxNordigLogicalChannelDes_t *pstDes)
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
		SIxNordigLogicalChannelInfo_t 	*pstPrevInfo = NULL;
		SIxNordigLogicalChannelInfo_t	*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxNordigLogicalChannelInfo_t, pstDes);

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
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNordigLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyLogicalChannelDes(SIxNordigLogicalChannelDes_t *pstDest, const SIxNordigLogicalChannelDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNordigLogicalChannelDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxNordigLogicalChannelInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxNordigLogicalChannelInfo_t, pstDest);

				pstNewInfo = allocLogicalChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxNordigLogicalChannelInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxNordigLogicalChannelInfo_t, freeLogicalChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

#define _eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2_
/************************************************************************
 * eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2				(Tag = 0x87)
 ************************************************************************/
static void initLogicalChannelv2Des(SIxNordigLogicalChannelv2Des_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxNordigLogicalChannelv2Des_t));
}

static SIxNordigLogicalChannelv2SvcInfo_t *allocLogicalChannelv2SvcInfo(void)
{
	return (SIxNordigLogicalChannelv2SvcInfo_t *)silib_desc_alloc(sizeof(SIxNordigLogicalChannelv2SvcInfo_t));
}

static SIxNordigLogicalChannelv2Info_t *allocLogicalChannelv2Info(void)
{
	return (SIxNordigLogicalChannelv2Info_t *)silib_desc_alloc(sizeof(SIxNordigLogicalChannelv2Info_t));
}

static SIxNordigLogicalChannelv2Des_t *allocLogicalChannelv2Des(void)
{
	return (SIxNordigLogicalChannelv2Des_t *)silib_desc_alloc(sizeof(SIxNordigLogicalChannelv2Des_t));
}

static void freeLogicalChannelv2SvcInfo(SIxNordigLogicalChannelv2SvcInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeLogicalChannelv2Info(SIxNordigLogicalChannelv2Info_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	FREE_INFO_LIST_EX(pstInfo->pstInfo, SIxNordigLogicalChannelv2SvcInfo_t, freeLogicalChannelv2SvcInfo);
	pstInfo->pstInfo = NULL;

	silib_desc_free(pstInfo);
}

static void freeLogicalChannelv2Des(SIxNordigLogicalChannelv2Des_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNordigLogicalChannelv2Info_t, freeLogicalChannelv2Info);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseLogicalChannelv2Des (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxNordigLogicalChannelv2Des_t *pstDes)
{
	HUINT16			 usDesLen;
	HUINT32			 ulNameLen, ulTempLen, ulSvcInfoLen;
	HUINT8			*pszChListName = NULL;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseLogicalChannelv2Des () Entered! \n");

#if 0
	// 구현은 했으나 test stream이 없어 test하지 못함.
	// 필요시 열고 테스트해서 사용하세요~
	return eSIxERR_NOT_SUPPORT;
#else
	/** < descriptor tag (1byte) */
	HxLOG_Info("\t(tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Info("\t(length = %d)\n", usDesLen);

	if (usDesLen < 6)
	{
		HxLOG_Error("[WARN] Unknown parseLogicalChannelv2Des Format! \n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0)
	{
		SIxNordigLogicalChannelv2Info_t 	*pstPrevInfo = NULL;
		SIxNordigLogicalChannelv2Info_t		*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxNordigLogicalChannelv2Info_t, pstDes);

		pstNewInfo = allocLogicalChannelv2Info();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocLogicalChannelInfo() failed..! \n");
			goto err_exit;
		}

		HxSTD_MemSet(pstNewInfo, 0, sizeof(SIxNordigLogicalChannelv2Info_t));
		/** < channel list id	 */
		pstNewInfo->ucChannelListId = HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;
		HxLOG_Info("\t\t(service id = 0x%x)\n", pstNewInfo->ucChannelListId);

		/** < channel list name	 */
		ulNameLen = HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		if (ulNameLen > 0)
		{
			pszChListName = NULL;
			ulTempLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, ulNameLen, (const HUINT8 *)p, &pszChListName);
			HxLOG_Info("\t\t(ch list name = %s)\n", (char *)pszChListName);

			pstNewInfo->pszChannelListName = silib_desc_util_allocStr(ulTempLen+1);
			if (pstNewInfo->pszChannelListName == NULL)
			{
				goto err_exit;
			}

			if (pszChListName != NULL)
			{
				ulTempLen = silib_desc_util_dvbStrlen(pszChListName);
				HxSTD_MemCopy(pstNewInfo->pszChannelListName, pszChListName, ulTempLen);
				pstNewInfo->pszChannelListName[ulTempLen] = '\0';
			}

			p += ulNameLen;
			usDesLen -= ulNameLen;
		}

		/** < country code 	*/
		HxSTD_MemCopy(pstNewInfo->szCountryCode, p, 3);
		pstNewInfo->szCountryCode[3] = '\0';
		p += 3;
		usDesLen -= 3;
		HxLOG_Info("\t\t(country code = %s)\n", pstNewInfo->szCountryCode);

		/** < service info 	*/
		ulSvcInfoLen = HxMACRO_Get8Bit(p);
		p += 1;
		usDesLen -= 1;

		while (ulSvcInfoLen > 0)
		{
			SIxNordigLogicalChannelv2SvcInfo_t 	*pstPrevSvcInfo = NULL;
			SIxNordigLogicalChannelv2SvcInfo_t 	*pstNewSvcInfo = NULL;

			// make svc info data
			GET_PREV_INFO(pstPrevSvcInfo, SIxNordigLogicalChannelv2SvcInfo_t, pstNewInfo);

			pstNewSvcInfo = allocLogicalChannelv2SvcInfo();
			if (pstNewSvcInfo == NULL)
			{
				HxLOG_Error("[WARN] allocLogicalChannelv2SvcInfo() failed..! \n");
				goto err_exit;
			}

			/*** < service id	 */
			pstNewSvcInfo->usServiceId = HxMACRO_Get16Bit(p);
			p += 2;
			usDesLen -= 2;
			ulSvcInfoLen -= 2;
			HxLOG_Info("\t\t(service id = 0x%x)\n", pstNewSvcInfo->usServiceId);

			/*** < visible service flag	 */
			pstNewSvcInfo->ucVisibleServiceFlag = (*p & 0x80) >> 7;
			HxLOG_Info("\t\t(ucVisibleServiceFlag = 0x%x)\n", pstNewSvcInfo->ucVisibleServiceFlag);

			/*** < logical channel number	 */
			pstNewSvcInfo->usLogicalChannelNumber = HxMACRO_Get10Bit(p);
			p += 2;
			usDesLen -= 2;
			ulSvcInfoLen -= 2;
			HxLOG_Info("\t\t(usLogicalChannelNumber = 0x%x)\n", pstNewSvcInfo->usLogicalChannelNumber);

			LINK_INFO_NODE(pstNewInfo->pstInfo, pstNewSvcInfo, NULL, pstPrevSvcInfo);
		}

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseLogicalChannelv2Des () Leaved! \n");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNordigLogicalChannelv2Info_t, freeLogicalChannelv2Info);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
#endif
}

static SIxError_e copyLogicalChannelv2Des(SIxNordigLogicalChannelv2Des_t *pstDest, const SIxNordigLogicalChannelv2Des_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNordigLogicalChannelv2Des_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxNordigLogicalChannelv2Info_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxNordigLogicalChannelv2Info_t, pstDest);

				pstNewInfo = allocLogicalChannelv2Info();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxNordigLogicalChannelInfo_t));
				pstNewInfo->pstNext = NULL;

				if (pstSourceInfo->pstInfo != NULL)
				{
					SIxNordigLogicalChannelv2SvcInfo_t	*pstPrevSvcInfo = NULL, *pstNewSvcInfo = NULL, *pstSourceSvcInfo = NULL;

					pstSourceSvcInfo = pstSourceInfo->pstInfo;
					while (pstSourceSvcInfo)
					{
						/** < Descriptor 데이타 구성. */
						GET_PREV_INFO(pstPrevSvcInfo, SIxNordigLogicalChannelv2SvcInfo_t, pstNewInfo);

						pstNewSvcInfo = allocLogicalChannelv2SvcInfo();
						if (pstNewSvcInfo == NULL)
						{
							goto err_exit;
						}

						HxSTD_MemCopy(pstNewSvcInfo, pstSourceSvcInfo, sizeof(SIxNordigLogicalChannelv2SvcInfo_t));
						pstNewSvcInfo->pstNext = NULL;

						LINK_INFO_NODE(pstNewInfo->pstInfo, pstNewSvcInfo, NULL, pstPrevSvcInfo);
						pstSourceInfo = pstSourceInfo->pstNext;
					}
				}

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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxNordigLogicalChannelv2Info_t, freeLogicalChannelv2Info);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}


#define _eSIxDESCTAG_NORDIG_DTCP_
/************************************************************************
 * eSIxDESCTAG_NORDIG_DTCP							(Tag = 0x88)
 ************************************************************************/
static void initDtcpDes(SIxNordigDtcpDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxNordigDtcpDes_t));
}

static SIxNordigDtcpDataInfo_t *allocDtcpDataInfo(void)
{
	return (SIxNordigDtcpDataInfo_t *)silib_desc_alloc(sizeof(SIxNordigDtcpDataInfo_t));
}

static SIxNordigDtcpDes_t *allocDtcpDes(void)
{
	return (SIxNordigDtcpDes_t *)silib_desc_alloc(sizeof(SIxNordigDtcpDes_t));
}

static void freeDtcpDataInfo(SIxNordigDtcpDataInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeDtcpDes(SIxNordigDtcpDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNordigDtcpDataInfo_t, freeDtcpDataInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseDtcpDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxNordigDtcpDes_t *pstDes)
{
	HINT16				usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseDtcpDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Info("\t(tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Info("\t(length = %d)\n", usDesLen);

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
		SIxNordigDtcpDataInfo_t 	*pstPrevInfo = NULL;
		SIxNordigDtcpDataInfo_t		*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxNordigDtcpDataInfo_t, pstDes);

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
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNordigDtcpDataInfo_t, freeDtcpDataInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyDtcpDes(SIxNordigDtcpDes_t *pstDest, const SIxNordigDtcpDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNordigDtcpDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxNordigDtcpDataInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxNordigDtcpDataInfo_t, pstDest);

				pstNewInfo = allocDtcpDataInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxNordigDtcpDataInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxNordigDtcpDataInfo_t, freeDtcpDataInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

#define _eSIxDESCTAG_NORDIG_CONTENT_PROTECTION_
/************************************************************************
 * eSIxDESCTAG_NORDIG_CONTENT_PROTECTION			(Tag = 0xa0)
 ************************************************************************/
static void initContentProtectionDes(SIxNordigContentProtectionDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxNordigContentProtectionDes_t));
}

static SIxNordigContentProtectionDes_t *allocContentProtectionDes(void)
{
	return (SIxNordigContentProtectionDes_t *)silib_desc_alloc(sizeof(SIxNordigContentProtectionDes_t));
}

static void freeContentProtectionDes(SIxNordigContentProtectionDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }
	silib_desc_free(pstDes);
}

static SIxError_e parseContentProtectionDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxNordigContentProtectionDes_t *pstDes)
{
	HINT16				usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseContentProtectionDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Info("\t(tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Info("\t(length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Error("[WARN] Unknown parseContentProtectionDes Format! \n");
		return eSIxERR_INVALID;
	}

	pstDes->ucContentProtectionLevel = HxMACRO_Get8Bit(p);

	HxLOG_Info(" parseContentProtectionDes () Leaved!");
	return eSIxERR_OK;
}

static SIxError_e copyContentProtectionDes(SIxNordigContentProtectionDes_t *pstDest, const SIxNordigContentProtectionDes_t *pstSource)
{
	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNordigContentProtectionDes_t));
		return eSIxERR_OK;
	}

	return eSIxERR_FAIL;
}


#ifdef CONFIG_OP_TDC
#define _eSIxDESCTAG_TDC_CHANNEL_
/************************************************************************
 * eSIxDESCTAG_TDC_CHANNEL (Tag = 0x82)
 ************************************************************************/
static void initTdcChannelDes(SIxTdcChannelDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxTdcChannelDes_t));
}

static SIxTdcChannelInfo_t *allocTdcChannelInfo(void)
{
	return (SIxTdcChannelInfo_t *)silib_desc_alloc(sizeof(SIxTdcChannelInfo_t));
}

static SIxTdcChannelDes_t *allocTdcChannelDes(void)
{
	return (SIxTdcChannelDes_t *)silib_desc_alloc(sizeof(SIxTdcChannelDes_t));
}

static void freeTdcChannelInfo(SIxTdcChannelInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeTdcChannelDes(SIxTdcChannelDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxTdcChannelInfo_t, freeTdcChannelInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseTdcChannelDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxTdcChannelDes_t *pstDes)
{
	HUINT16				usDesLen;

	if (pstDes == NULL)
	{
		HxLOG_Error("[WARN] NULL descriptor param !!  \n");
		return eSIxERR_INVALID;
	}

	HxLOG_Info("parseTdcChannelDes () Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Info("\t(tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Info("\t(length = %d)\n", usDesLen);

	/** descriptor length should be a multiple of number four.
		refer the TDC channel descriptor syntax:

		Channel_descriptor() {
			descriptor_tag			8 Uimsbf
			descriptor_length		8 Uimsbf
			for (i=0;i<N;i++) {
				service_id			16 Uimsbf
				Channel_number		16 uimsbf
			}
		}
	*/
	if ((usDesLen < 4) || (0 != (usDesLen % 4)))
	{
		HxLOG_Error("[WARN] Unknown parseTdcChannelDes Format! - usDesLen = %d, (usDesLen %% 4) = %d \n", usDesLen, (usDesLen % 4));
		return eSIxERR_INVALID;
	}

	while (4 <= usDesLen)
	{
		SIxTdcChannelInfo_t 	*pstPrevInfo = NULL;
		SIxTdcChannelInfo_t	*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxTdcChannelInfo_t, pstDes);

		pstNewInfo = allocTdcChannelInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocTdcChannelInfo() failed..! \n");
			goto err_exit;
		}

		/** < service id	 */
		pstNewInfo->usServiceId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Info("\t\t(service id = 0x%x)\n", pstNewInfo->usServiceId);

		/** < channel number	 */
		pstNewInfo->usChannelNumber = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Info("\t\t(usChannelNumber = 0x%x)\n", pstNewInfo->usChannelNumber);

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseLogicalChannelDes () Leaved!\n");
	return eSIxERR_OK;

err_exit:

	if (pstDes != NULL)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxTdcChannelInfo_t, freeTdcChannelInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyTdcChannelDes(SIxTdcChannelDes_t *pstDest, const SIxTdcChannelDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTdcChannelDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxTdcChannelInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxTdcChannelInfo_t, pstDest);

				pstNewInfo = allocTdcChannelInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxTdcChannelInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxTdcChannelInfo_t, freeTdcChannelInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

/* _eSIxDESCTAG_TDC_CHANNEL_				: (0x82)	*/
//Channel Des
static SIxDescriptor_t *silib_desc_allocTdcChannelDes(void)
{
	SIxTdcChannelDes_t	*pstDes;

	pstDes = allocTdcChannelDes();
	if (pstDes)
	{
		initTdcChannelDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeTdcChannelDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeTdcChannelDes((SIxTdcChannelDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseTdcChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseTdcChannelDes((HUINT8 *)pucRawData, pstParam, (SIxTdcChannelDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTdcChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTdcChannelDes((SIxTdcChannelDes_t *)pstDest, (const SIxTdcChannelDes_t *)pstSource);
}

static void silib_desc_printTdcChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxTdcChannelDes_t		*pstDesc;

	pstDesc = (SIxTdcChannelDes_t *)pstDes;
}

#endif

#define __eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL__
/************************************************************************
 * eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL					(Tag = 0x83)
 ************************************************************************/

static SIxDescriptor_t *silib_desc_allocLogicalChannelDes(void)
{
	SIxNordigLogicalChannelDes_t	*pstDes;

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
		freeLogicalChannelDes((SIxNordigLogicalChannelDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseLogicalChannelDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLogicalChannelDes((HUINT8 *)pucRawData, pstParam, (SIxNordigLogicalChannelDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLogicalChannelDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLogicalChannelDes((SIxNordigLogicalChannelDes_t *)pstDest, (const SIxNordigLogicalChannelDes_t *)pstSource);
}

static void silib_desc_printLogicalChannelDes(const SIxDescriptor_t *pstDes)
{
	SIxNordigLogicalChannelDes_t		*pstDesc;

	pstDesc = (SIxNordigLogicalChannelDes_t *)pstDes;
}

#define __eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2__
/************************************************************************
 * eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2				(Tag = 0x87)
 ************************************************************************/
static SIxDescriptor_t *silib_desc_allocLogicalChannelv2Des(void)
{
	SIxNordigLogicalChannelv2Des_t	*pstDes;

	pstDes = allocLogicalChannelv2Des();
	if (pstDes)
	{
		initLogicalChannelv2Des(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeLogicalChannelv2Des(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeLogicalChannelv2Des((SIxNordigLogicalChannelv2Des_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseLogicalChannelv2Des(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLogicalChannelv2Des((HUINT8 *)pucRawData, pstParam, (SIxNordigLogicalChannelv2Des_t *)pstDes);
}

static SIxError_e silib_desc_copyLogicalChannelv2Des(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLogicalChannelv2Des((SIxNordigLogicalChannelv2Des_t *)pstDest, (const SIxNordigLogicalChannelv2Des_t *)pstSource);
}

static void silib_desc_printLogicalChannelv2Des(const SIxDescriptor_t *pstDes)
{
	SIxNordigLogicalChannelv2Des_t		*pstDesc;

	pstDesc = (SIxNordigLogicalChannelv2Des_t *)pstDes;
}


#define __eSIxDESCTAG_NORDIG_DTCP__
/************************************************************************
 * eSIxDESCTAG_NORDIG_DTCP							(Tag = 0x88)
 ************************************************************************/
//DvhsDTCP
static SIxDescriptor_t *silib_desc_allocDtcpDes(void)
{
	SIxNordigDtcpDes_t	*pstDes;

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
	freeDtcpDes((SIxNordigDtcpDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDtcpDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDtcpDes((HUINT8 *)pucRawData, pstParam, (SIxNordigDtcpDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDtcpDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDtcpDes((SIxNordigDtcpDes_t *)pstDest, (SIxNordigDtcpDes_t *)pstSource);
}

static void silib_desc_printDtcpDes(const SIxDescriptor_t *pstDes)
{
	SIxNordigDtcpDes_t		*pstDesc;

	pstDesc = (SIxNordigDtcpDes_t *)pstDes;
}

#define __eSIxDESCTAG_NORDIG_CONTENT_PROTECTION__
/************************************************************************
 * eSIxDESCTAG_NORDIG_CONTENT_PROTECTION				(Tag = 0xA0)
 ************************************************************************/
//DvhsDTCP
static SIxDescriptor_t *silib_desc_allocContentProtectionDes(void)
{
	SIxNordigContentProtectionDes_t	*pstDes;

	pstDes = allocContentProtectionDes();
	if (pstDes)
	{
		initContentProtectionDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return (SIxDescriptor_t *)NULL;
}

static SIxError_e silib_desc_freeContentProtectionDes(SIxDescriptor_t *pstDes)
{
	freeContentProtectionDes((SIxNordigContentProtectionDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseContentProtectionDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseContentProtectionDes((HUINT8 *)pucRawData, pstParam, (SIxNordigContentProtectionDes_t *)pstDes);
}

static SIxError_e silib_desc_copyContentProtectionDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyContentProtectionDes((SIxNordigContentProtectionDes_t *)pstDest, (SIxNordigContentProtectionDes_t *)pstSource);
}

static void silib_desc_printContentProtectionDes(const SIxDescriptor_t *pstDes)
{
	SIxNordigContentProtectionDes_t		*pstDesc;

	pstDesc = (SIxNordigContentProtectionDes_t *)pstDes;
}


#define	___MODULE_APIs___

SIxDescriptor_t *silib_desc_alloc_Nordig(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL:			return silib_desc_allocLogicalChannelDes();
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2:			return silib_desc_allocLogicalChannelv2Des();
		case eSIxDESCTAG_NORDIG_DTCP:						return silib_desc_allocDtcpDes();
		case eSIxDESCTAG_NORDIG_CONTENT_PROTECTION:			return silib_desc_allocContentProtectionDes();
#ifdef CONFIG_OP_TDC
		case eSIxDESCTAG_TDC_CHANNEL:						return silib_desc_allocTdcChannelDes();
#endif
		default:											break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Nordig(SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL:			return silib_desc_freeLogicalChannelDes(pstDes);
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2:			return silib_desc_freeLogicalChannelv2Des(pstDes);
		case eSIxDESCTAG_NORDIG_DTCP:						return silib_desc_freeDtcpDes(pstDes);
		case eSIxDESCTAG_NORDIG_CONTENT_PROTECTION:			return silib_desc_freeContentProtectionDes(pstDes);
#ifdef CONFIG_OP_TDC
		case eSIxDESCTAG_TDC_CHANNEL:						return silib_desc_freeTdcChannelDes(pstDes);
#endif
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_parse_Nordig(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL:			return silib_desc_parseLogicalChannelDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2:			return silib_desc_parseLogicalChannelv2Des(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_NORDIG_DTCP:						return silib_desc_parseDtcpDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_NORDIG_CONTENT_PROTECTION:			return silib_desc_parseContentProtectionDes(pucRawData, pstParam, pstDes);
#ifdef CONFIG_OP_TDC
		case eSIxDESCTAG_TDC_CHANNEL:						return silib_desc_parseTdcChannelDes(pucRawData, pstParam, pstDes);
#endif
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Nordig(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL:			return silib_desc_copyLogicalChannelDes(pstDest, pstSource);
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2:			return silib_desc_copyLogicalChannelv2Des(pstDest, pstSource);
		case eSIxDESCTAG_NORDIG_DTCP:						return silib_desc_copyDtcpDes(pstDest, pstSource);
		case eSIxDESCTAG_NORDIG_CONTENT_PROTECTION:			return silib_desc_copyContentProtectionDes(pstDest, pstSource);
#ifdef CONFIG_OP_TDC
		case eSIxDESCTAG_TDC_CHANNEL:						return silib_desc_copyTdcChannelDes(pstDest, pstSource);
#endif
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Nordig(const SIxDescriptor_t *pstDes)
{
	SIxError_e		eErr;

//	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	HxLOG_Info("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Nordig(pstDes->stHead.eTag) );
	HxLOG_Info("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL:			silib_desc_printLogicalChannelDes(pstDes);		eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2:			silib_desc_printLogicalChannelv2Des(pstDes);	eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_NORDIG_DTCP:						silib_desc_printDtcpDes(pstDes);				eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_NORDIG_CONTENT_PROTECTION:			silib_desc_printContentProtectionDes(pstDes);				eErr = eSIxERR_OK;	break;
#ifdef CONFIG_OP_TDC
		case eSIxDESCTAG_TDC_CHANNEL:						silib_desc_printTdcChannelDes(pstDes);			eErr = eSIxERR_OK;	break;
#endif
		default:											eErr = eSIxERR_NOT_SUPPORT;	break;
	}

	return eErr;
}

/* end of file */
