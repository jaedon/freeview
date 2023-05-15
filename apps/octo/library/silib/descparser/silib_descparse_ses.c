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
	@file     sxdescparse_ses.c
	@brief    sxdescparse_ses.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

static HCHAR *silib_desc_getTagString_Ses(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
 	ENUM_TO_STR(eSIxDESCTAG_SES_SERVICE_LIST_NAME);
 	ENUM_TO_STR(eSIxDESCTAG_SES_BOUQUET_LIST);
 	ENUM_TO_STR(eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID);

	default:
		break;
	}

	return NULL;
}

#define _eSIxDESCTAG_SES_SERVICE_LIST_NAME_
/************************************************************************
 * eSIxDESCTAG_SES_SERVICE_LIST_NAME (Tag = 0x88)
 ************************************************************************/
static void initServiceListNameDes(SIxServiceListNameDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxServiceListNameDes_t));
	HxSTD_MemSet(pstDes->szIso639LangCode, '\0', 4);
	pstDes->pszSvcListName 	= NULL;
}

static SIxServiceListNameDes_t *allocServiceListNameDes(void)
{
	return (SIxServiceListNameDes_t *)silib_desc_alloc(sizeof(SIxServiceListNameDes_t));
}

static void freeServiceListNameDes(SIxServiceListNameDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	silib_desc_free(pstDes->pszSvcListName);
	silib_desc_free(pstDes);
}

static void setServiceListNameDes(SIxServiceListNameDes_t *pstDes, HUINT8 *pszIso639LangCode, HUINT8 *pszSvcListName)
{
    if(pszIso639LangCode != NULL)
    {
    	HxSTD_MemCopy(pstDes->szIso639LangCode, pszIso639LangCode, 3);
    }
	pstDes->szIso639LangCode[3] = '\0';

    if(pszSvcListName != NULL)
    {
	    HxSTD_MemCopy(pstDes->pszSvcListName, pszSvcListName, silib_desc_util_dvbStrlen(pszSvcListName));
    }
	pstDes->pszSvcListName[silib_desc_util_dvbStrlen(pszSvcListName)] = '\0';
}

static SIxError_e parseServiceListNameDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxServiceListNameDes_t *pstDes)
{
	HUINT16				usDesLen;
	HUINT8				szIso639LangCode[4];
	HUINT8				*pszSvcListName = NULL;
	HUINT32				ulSvcListNameLen = 0;

	HxLOG_Info("parseServiceListNameDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 3)
	{
		HxLOG_Error("[WARN] Unknown parseServiceListNameDes Format!\n");
		return eSIxERR_INVALID;
	}

	/**< ISO 639-2 language code (3byte)*/
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode[3] = '\0';
	silib_desc_util_makeCorrct639Code(szIso639LangCode);
	HxLOG_Print("\t\t(szIso639LangCode = %s)\n", szIso639LangCode);

	/**< char */
	p += 3;
	usDesLen -= 3;
	ulSvcListNameLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)usDesLen, (const HUINT8 *)p, &pszSvcListName);
	HxLOG_Print("\t\t(char = %s, len(%d))\n", pszSvcListName, ulSvcListNameLen);

	/** < Descriptor 데이타 구성. */
	pstDes->pszSvcListName = silib_desc_util_allocStr(ulSvcListNameLen + 1);
	if (pstDes->pszSvcListName == NULL)
	{
		goto err_exit;
	}
	setServiceListNameDes(pstDes, szIso639LangCode, pszSvcListName);
	silib_desc_free(pszSvcListName);

	HxLOG_Info(" parseServiceListNameDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	silib_desc_free(pszSvcListName);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyServiceListNameDes(SIxServiceListNameDes_t *pstDest, const SIxServiceListNameDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxServiceListNameDes_t));
		pstDest->pszSvcListName = NULL;

		if (pstSource->pszSvcListName != NULL)
		{
			HUINT32 ulStrLen = silib_desc_util_dvbStrlen(pstSource->pszSvcListName);
			if (ulStrLen == 0)
			{
				goto err_exit;
			}

			pstDest->pszSvcListName = silib_desc_util_allocStr(ulStrLen + 1);
			if (pstDest->pszSvcListName == NULL)
			{
				goto err_exit;
			}

			HxSTD_MemCopy(pstDest->pszSvcListName, pstSource->pszSvcListName, ulStrLen);
			pstDest->pszSvcListName[ulStrLen] = '\0';

			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	silib_desc_free(pstDest->pszSvcListName);
	pstDest->pszSvcListName = NULL;

	return eSIxERR_FAIL;
}

#define _eSIxDESCTAG_SES_BOUQUET_LIST_
/************************************************************************
 * eSIxDESCTAG_SES_BOUQUET_LIST (Tag = 0x93)
 ************************************************************************/
static void initBouquetListDes(SIxBouquetListDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxBouquetListDes_t));
	pstDes->pstInfo = NULL;
}

static SIxBouquetListDes_t *allocBouquetListDes(void)
{
	return (SIxBouquetListDes_t *)silib_desc_alloc(sizeof(SIxBouquetListDes_t));
}

static SIxBouquetListInfo_t *allocBouquetListInfo(void)
{
	return (SIxBouquetListInfo_t *)silib_desc_alloc(sizeof(SIxBouquetListInfo_t));
}

static void freeBouquetListInfo(SIxBouquetListInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo->pszBouquetName);
	silib_desc_free(pstInfo);
}

static void freeBouquetListDes(SIxBouquetListDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxBouquetListInfo_t, freeBouquetListInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static void setBouquetListInfo(SIxBouquetListInfo_t *pstInfo, HUINT8 *pszName, HUINT8 ucLen)
{
	if(pszName != NULL)
	{
		HxSTD_MemCopy(pstInfo->pszBouquetName, pszName, ucLen);
	}
	pstInfo->pszBouquetName[ucLen] = '\0';
	pstInfo->ucBouquetNameLength = ucLen;
}

static SIxError_e parseBouquetListDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxBouquetListDes_t *pstDes)
{
	HINT32					lDesLen = 0;
	SIxBouquetListInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8					ucAssignedNameLen = 0, ucNameLen = 0;
	HUINT8					*pszName = NULL;

	HxLOG_Info("parseBouquetListDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	lDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", lDesLen);

	if (lDesLen < 2)
	{
		HxLOG_Error("[WARN] Unknown parseBouquetListDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < bouquet name   */
	while (lDesLen > 0)
	{
		ucNameLen = *p++;
		lDesLen--;

		// set max name len
		ucAssignedNameLen = (ucNameLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucNameLen);

		if (lDesLen < ucAssignedNameLen)
		{
			// 간혹 desc. len보다 name len가 크게 들어 있는 경우가 있다
			ucAssignedNameLen = lDesLen;
		}

		// get bouquet name string
		ucNameLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucAssignedNameLen, (const HUINT8 *)p, &pszName);

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxBouquetListInfo_t, pstDes);

		pstNewInfo = allocBouquetListInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocBouquetListInfo() failed..! \n");
			goto err_exit;
		}

		pstNewInfo->pszBouquetName = silib_desc_util_allocStr(ucNameLen + 1);
		if (pstNewInfo->pszBouquetName == NULL)
		{
			HxLOG_Error("[WARN] silib_desc_util_allocStr() failed..! \n");
			goto err_exit;
		}

		setBouquetListInfo(pstNewInfo, pszName, ucNameLen);
		silib_desc_free(pszName);
		pszName = NULL;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);

		p += ucAssignedNameLen;
		lDesLen -= ucAssignedNameLen;

		HxLOG_Print("\t\t(bouquet name = %s)\n", pstNewInfo->pszBouquetName);
	}


	HxLOG_Info(" parseBouquetListDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	silib_desc_free(pszName);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyBouquetListDes(SIxBouquetListDes_t *pstDest, const SIxBouquetListDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxBouquetListDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxBouquetListInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxBouquetListInfo_t, pstDest);

				pstNewInfo = allocBouquetListInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				pstNewInfo->pszBouquetName = silib_desc_util_allocStr(pstSourceInfo->ucBouquetNameLength +1);
				if (pstNewInfo->pszBouquetName == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo->pszBouquetName, pstSourceInfo->pszBouquetName, pstSourceInfo->ucBouquetNameLength);
				pstNewInfo->pszBouquetName[pstSourceInfo->ucBouquetNameLength] = '\0';
				pstNewInfo->ucBouquetNameLength = pstSourceInfo->ucBouquetNameLength;

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}

			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxBouquetListInfo_t, freeBouquetListInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
	}

	return eSIxERR_FAIL;
}

#define _eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID_
/************************************************************************
 * eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID (Tag = 0xd1)
 ************************************************************************/
static void initVirtualServiceIdDes(SIxVirtualServiceIdDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxVirtualServiceIdDes_t));
}

static SIxVirtualServiceIdDes_t *allocVirtualServiceIdDes(void)
{
	return (SIxVirtualServiceIdDes_t *)silib_desc_alloc(sizeof(SIxVirtualServiceIdDes_t));
}

static void freeVirtualServiceIdDes(SIxVirtualServiceIdDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	silib_desc_free(pstDes);
}

static SIxError_e parseVirtualServiceIdDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxVirtualServiceIdDes_t *pstDes)
{
	HUINT16				usDesLen;

	HxLOG_Info("parseVirtualServiceIdDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 2)
	{
		HxLOG_Error("[WARN] Unknown parseVirtualServiceIdDes Format! \n");
		return eSIxERR_INVALID;
	}

	/**< virtual service id */
	pstDes->usVirtualServiceId = HxMACRO_Get16Bit(p);

	HxLOG_Info(" parseVirtualServiceIdDes () Leaved!");
	return eSIxERR_OK;
}

static SIxError_e copyVirtualServiceIdDes(SIxVirtualServiceIdDes_t *pstDest, const SIxVirtualServiceIdDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxVirtualServiceIdDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

#define _eSIxDESCTAG_SES_COPY_CONTROL_
/************************************************************************
 * eSIxDESCTAG_SES_COPY_CONTROL (Tag = 0xde)
 ************************************************************************/
static void initCopyControlDes(SIxCopyControlDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxCopyControlDes_t));
	pstDes->pstInfo = NULL;
}

static SIxCopyControlInfo_t *allocCopyControlInfo(void)
{
	return (SIxCopyControlInfo_t *)silib_desc_alloc(sizeof(SIxCopyControlInfo_t));
}

static SIxCopyControlDes_t *allocCopyControlDes(void)
{
	return (SIxCopyControlDes_t *)silib_desc_alloc(sizeof(SIxCopyControlDes_t));
}

static void freeCopyControlInfo(SIxCopyControlInfo_t *pstInfo)
{
	if (pstInfo == NULL) 	{ return; }

	silib_desc_free(pstInfo);
}

static void freeCopyControlDes(SIxCopyControlDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxCopyControlInfo_t, freeCopyControlInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseCopyControlDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCopyControlDes_t *pstDes)
{
	HUINT16					usDesLen;
	SIxCopyControlInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;

	HxLOG_Info("parseCopyControlDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Error("[WARN] Unknown parseCopyControlDes Format! \n");
		return eSIxERR_INVALID;
	}

	/**< version */
	pstDes->ucVersion= HxMACRO_Get8Bit(p);
	p++;
	usDesLen--;

	while (usDesLen > 0)
	{
		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxCopyControlInfo_t, pstDes);

		pstNewInfo = allocCopyControlInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Error("[WARN] allocBouquetListInfo() failed..! \n");
			goto err_exit;
		}

		pstNewInfo->ucCopyControl = HxMACRO_Get8Bit(p);

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);

		p++;
		usDesLen--;

		HxLOG_Print("\t\t(copy control byte = 0x%X)\n", pstNewInfo->ucCopyControl);
	}

	HxLOG_Info(" parseCopyControlDes () Leaved!");
	return eSIxERR_OK;

err_exit:

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyCopyControlDes(SIxCopyControlDes_t *pstDest, const SIxCopyControlDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCopyControlDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxCopyControlInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;

			while (pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxCopyControlInfo_t, pstDest);

				pstNewInfo = allocCopyControlInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				pstNewInfo->ucCopyControl = pstSourceInfo->ucCopyControl;

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}

			eErr = eSIxERR_OK;
		}
	}

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxCopyControlInfo_t, freeCopyControlInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
	}

	return eSIxERR_FAIL;
}

#define _eSIxDESCTAG_SES_DTCP_
/************************************************************************
 * eSIxDESCTAG_SES_DTCP							(Tag = 0x88)
 ************************************************************************/
static void initDtcpDes(SIxSesDtcpDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0x00, sizeof(SIxSesDtcpDes_t));
}

static SIxSesDtcpDataInfo_t *allocDtcpDataInfo(void)
{
	return (SIxSesDtcpDataInfo_t *)silib_desc_alloc(sizeof(SIxSesDtcpDataInfo_t));
}

static SIxSesDtcpDes_t *allocDtcpDes(void)
{
	return (SIxSesDtcpDes_t *)silib_desc_alloc(sizeof(SIxSesDtcpDes_t));
}

static void freeDtcpDataInfo(SIxSesDtcpDataInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo);
}

static void freeDtcpDes(SIxSesDtcpDes_t *pstDes)
{
	if (pstDes == NULL) 	{ return; }

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSesDtcpDataInfo_t, freeDtcpDataInfo);
	pstDes->pstInfo = NULL;

	silib_desc_free(pstDes);
}

static SIxError_e parseDtcpDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSesDtcpDes_t *pstDes)
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
		SIxSesDtcpDataInfo_t 		*pstPrevInfo = NULL;
		SIxSesDtcpDataInfo_t		*pstNewInfo = NULL;

		// make descriptor data
		GET_PREV_INFO(pstPrevInfo, SIxSesDtcpDataInfo_t, pstDes);

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
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSesDtcpDataInfo_t, freeDtcpDataInfo);
		pstDes->pstInfo = NULL;
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyDtcpDes(SIxSesDtcpDes_t *pstDest, const SIxSesDtcpDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSesDtcpDes_t));
		pstDest->pstInfo = NULL;

		if (pstSource->pstInfo != NULL)
		{
			SIxSesDtcpDataInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL, *pstSourceInfo = NULL;

			pstSourceInfo = pstSource->pstInfo;
			while(pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxSesDtcpDataInfo_t, pstDest);

				pstNewInfo = allocDtcpDataInfo();
				if (pstNewInfo == NULL)
				{
					goto err_exit;
				}

				HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxSesDtcpDataInfo_t));
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
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxSesDtcpDataInfo_t, freeDtcpDataInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest);
		pstDest = NULL;
	}

	return eSIxERR_FAIL;
}

#define __eSIxDESCTAG_SES_SERVICE_LIST_NAME__
/* eSIxDESCTAG_SES_SERVICE_LIST_NAME				: (0x88)	*/
//ServiceListNameDes
static SIxDescriptor_t *silib_desc_allocServiceListNameDes(void)
{
	SIxServiceListNameDes_t	*pstDes;

	pstDes = allocServiceListNameDes();
	if (pstDes)
	{
		initServiceListNameDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeServiceListNameDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeServiceListNameDes((SIxServiceListNameDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseServiceListNameDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseServiceListNameDes((HUINT8 *)pucRawData, pstParam, (SIxServiceListNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyServiceListNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyServiceListNameDes((SIxServiceListNameDes_t *)pstDest, (const SIxServiceListNameDes_t *)pstSource);
}

static void silib_desc_printServiceListNameDes(const SIxDescriptor_t *pstDes)
{
	SIxServiceListNameDes_t		*pstDesc;

	pstDesc = (SIxServiceListNameDes_t *)pstDes;
}

#define __eSIxDESCTAG_SES_BOUQUET_LIST__
/* eSIxDESCTAG_SES_BOUQUET_LIST				: (0x93)	*/
//BouquetListDes
static SIxDescriptor_t *silib_desc_allocBouquetListDes(void)
{
	SIxBouquetListDes_t	*pstDes;

	pstDes = allocBouquetListDes();
	if (pstDes)
	{
		initBouquetListDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeBouquetListDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeBouquetListDes((SIxBouquetListDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseBouquetListDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseBouquetListDes((HUINT8 *)pucRawData, pstParam, (SIxBouquetListDes_t *)pstDes);
}

static SIxError_e silib_desc_copyBouquetListDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyBouquetListDes((SIxBouquetListDes_t *)pstDest, (const SIxBouquetListDes_t *)pstSource);
}

static void silib_desc_printBouquetListDes(const SIxDescriptor_t *pstDes)
{
	SIxBouquetListDes_t		*pstDesc;

	pstDesc = (SIxBouquetListDes_t *)pstDes;
}

#define __eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID__
/* eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID				: (0xd1)	*/
//VirtualServiceIdDes
static SIxDescriptor_t *silib_desc_allocVirtualServiceIdDes(void)
{
	SIxVirtualServiceIdDes_t	*pstDes;

	pstDes = allocVirtualServiceIdDes();
	if (pstDes)
	{
		initVirtualServiceIdDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeVirtualServiceIdDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeVirtualServiceIdDes((SIxVirtualServiceIdDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseVirtualServiceIdDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseVirtualServiceIdDes((HUINT8 *)pucRawData, pstParam, (SIxVirtualServiceIdDes_t *)pstDes);
}

static SIxError_e silib_desc_copyVirtualServiceIdDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyVirtualServiceIdDes((SIxVirtualServiceIdDes_t *)pstDest, (const SIxVirtualServiceIdDes_t *)pstSource);
}

static void silib_desc_printVirtualServiceIdDes(const SIxDescriptor_t *pstDes)
{
	SIxVirtualServiceIdDes_t		*pstDesc;

	pstDesc = (SIxVirtualServiceIdDes_t *)pstDes;
}

#define __eSIxDESCTAG_SES_COPY_CONTROL__
/* eSIxDESCTAG_SES_COPY_CONTROL				: (0xde)	*/
//SIxCopyControlDes_t
static SIxDescriptor_t *silib_desc_allocCopyControlDes(void)
{
	SIxCopyControlDes_t	*pstDes;

	pstDes = allocCopyControlDes();
	if (pstDes)
	{
		initCopyControlDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCopyControlDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeCopyControlDes((SIxCopyControlDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseCopyControlDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCopyControlDes((HUINT8 *)pucRawData, pstParam, (SIxCopyControlDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCopyControlDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCopyControlDes((SIxCopyControlDes_t *)pstDest, (const SIxCopyControlDes_t *)pstSource);
}

static void silib_desc_printCopyControlDes(const SIxDescriptor_t *pstDes)
{
	SIxCopyControlDes_t		*pstDesc;

	pstDesc = (SIxCopyControlDes_t *)pstDes;
}

/************************************************************************
 * eSIxDESCTAG_SES_DTCP							(Tag = 0x88)
 ************************************************************************/
//DvhsDTCP
static SIxDescriptor_t *silib_desc_allocDtcpDes(void)
{
	SIxSesDtcpDes_t	*pstDes;

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
	freeDtcpDes((SIxSesDtcpDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDtcpDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDtcpDes((HUINT8 *)pucRawData, pstParam, (SIxSesDtcpDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDtcpDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDtcpDes((SIxSesDtcpDes_t *)pstDest, (SIxSesDtcpDes_t *)pstSource);
}

static void silib_desc_printDtcpDes(const SIxDescriptor_t *pstDes)
{
	SIxSesDtcpDes_t		*pstDesc;

	pstDesc = (SIxSesDtcpDes_t *)pstDes;
}


#define	___MODULE_APIs___

SIxDescriptor_t *silib_desc_alloc_Ses(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
#if defined(CONFIG_SUPPORT_ROVI)
		case eSIxDESCTAG_SES_DTCP:								return silib_desc_allocDtcpDes();
#else
		case eSIxDESCTAG_SES_SERVICE_LIST_NAME: 				return silib_desc_allocServiceListNameDes();
#endif
		case eSIxDESCTAG_SES_BOUQUET_LIST:						return silib_desc_allocBouquetListDes();
		case eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID:				return silib_desc_allocVirtualServiceIdDes();
		case eSIxDESCTAG_SES_COPY_CONTROL:						return silib_desc_allocCopyControlDes();
		default:													break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Ses(SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
#if defined(CONFIG_SUPPORT_ROVI)
		case eSIxDESCTAG_SES_DTCP:							return silib_desc_freeDtcpDes(pstDes);
#else
		case eSIxDESCTAG_SES_SERVICE_LIST_NAME: 			return silib_desc_freeServiceListNameDes(pstDes);
#endif
		case eSIxDESCTAG_SES_BOUQUET_LIST:					return silib_desc_freeBouquetListDes(pstDes);
		case eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID:			return silib_desc_freeVirtualServiceIdDes(pstDes);
		case eSIxDESCTAG_SES_COPY_CONTROL:					return silib_desc_freeCopyControlDes(pstDes);
		default:												break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_Ses(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
#if defined(CONFIG_SUPPORT_ROVI)
		case eSIxDESCTAG_SES_DTCP:							return silib_desc_parseDtcpDes(pucRawData, pstParam, pstDes);
#else
		case eSIxDESCTAG_SES_SERVICE_LIST_NAME: 			return silib_desc_parseServiceListNameDes(pucRawData, pstParam, pstDes);
#endif
		case eSIxDESCTAG_SES_BOUQUET_LIST:					return silib_desc_parseBouquetListDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID:			return silib_desc_parseVirtualServiceIdDes(pucRawData, pstParam, pstDes);
		case eSIxDESCTAG_SES_COPY_CONTROL:					return silib_desc_parseCopyControlDes(pucRawData, pstParam, pstDes);
		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Ses(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
#if defined(CONFIG_SUPPORT_ROVI)
		case eSIxDESCTAG_SES_DTCP:							return silib_desc_copyDtcpDes(pstDest, pstSource);
#else
		case eSIxDESCTAG_SES_SERVICE_LIST_NAME: 			return silib_desc_copyServiceListNameDes(pstDest, pstSource);
#endif
		case eSIxDESCTAG_SES_BOUQUET_LIST:					return silib_desc_copyBouquetListDes(pstDest, pstSource);
		case eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID:			return silib_desc_copyVirtualServiceIdDes(pstDest, pstSource);
		case eSIxDESCTAG_SES_COPY_CONTROL:					return silib_desc_copyCopyControlDes(pstDest, pstSource);

		default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Ses(const SIxDescriptor_t *pstDes)
{
	SIxError_e		eErr;

//	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Ses(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
#if defined(CONFIG_SUPPORT_ROVI)
		case eSIxDESCTAG_SES_DTCP:						silib_desc_printDtcpDes(pstDes);					eErr = eSIxERR_OK;	break;
#else
		case eSIxDESCTAG_SES_SERVICE_LIST_NAME: 		silib_desc_printServiceListNameDes(pstDes); 		eErr = eSIxERR_OK;	break;
#endif
		case eSIxDESCTAG_SES_BOUQUET_LIST:				silib_desc_printBouquetListDes(pstDes);				eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID:		silib_desc_printVirtualServiceIdDes(pstDes);		eErr = eSIxERR_OK;	break;
		case eSIxDESCTAG_SES_COPY_CONTROL:				silib_desc_printCopyControlDes(pstDes);				eErr = eSIxERR_OK;	break;

		default:										eErr = eSIxERR_NOT_SUPPORT;	break;
	}

	return eErr;
}

/* end of file */
