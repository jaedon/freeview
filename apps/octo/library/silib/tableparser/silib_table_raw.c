/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sxtable_raw.c
	@brief

	Description:  									\n
	Module: SILIB/TABLEPARSER					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include "../include/silib.h"

/*******************************************************************/
/********************      Local Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Enumerations         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Data Type         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32			 ulSecLen;
	HUINT32			 ul1stDesLoopPos, ul1stDesLoopLen;
	HUINT32			 ul2ndLoopPos, ul2ndLoopLen;
} silib_table_posInfo_t;

/*******************************************************************/
/********************      Local Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
// Descriptors:
STATIC HERROR silib_table_InsertRawDescriptors (HxList_t **ppstDesList, HUINT8 *pucDesRaw, HUINT32 ulLength, HBOOL bRawDup)
{
	HUINT32				 ulDesLen;
	HINT32				 nRemainLen = (HINT32)ulLength;

	while (nRemainLen > 0)
	{
		if (nRemainLen < 2)
		{
			HxLOG_Error("Error in Raw Descriptors...\n");
			return ERR_FAIL;
		}

		ulDesLen = (HUINT32)HxMACRO_Get8Bit(pucDesRaw + 1) + 2;

		if (ulDesLen > nRemainLen)
		{
			HxLOG_Error("Error in Raw Descriptors Length(ulDesLen=%d,nRemainLen=%d)...\n",ulDesLen,nRemainLen);
			return ERR_FAIL;
		}

		if (TRUE == bRawDup)
		{
			HUINT8			*pucDupRaw = (HUINT8 *)HLIB_STD_MemAlloc(ulDesLen);

			if (NULL == pucDupRaw)
			{
				HxLOG_Error("Duplicating the raw failed\n");
				return ERR_FAIL;
			}

			HxSTD_MemCopy(pucDupRaw, pucDesRaw, ulDesLen);
			*ppstDesList = HLIB_LIST_Append(*ppstDesList, (void *)pucDupRaw);
		}
		else
		{
			*ppstDesList = HLIB_LIST_Append(*ppstDesList, (void *)pucDesRaw);
		}

		pucDesRaw += ulDesLen;
		nRemainLen -= ulDesLen;
	}

	if (nRemainLen < 0)
	{
		HxLOG_Critical("Descriptors Error !!!! \n");
	}

	return ERR_OK;
}

STATIC HERROR silib_table_InsertRawDescTo1stLoop (SIxTable_PsiTable_t *pstTable, HUINT8 *pucDesRaw, HUINT32 ulLength, HBOOL bRawDup)
{
	return silib_table_InsertRawDescriptors(&(pstTable->pstDesLoop), pucDesRaw, ulLength, bRawDup);
}

STATIC HERROR silib_table_InsertRawDescTo2ndLoop (SIxTable_2ndLoop_t *pstLoop, HUINT8 *pucDesRaw, HUINT32 ulLength, HBOOL bRawDup)
{
	return silib_table_InsertRawDescriptors(&(pstLoop->pstDesLoop), pucDesRaw, ulLength, bRawDup);
}

// refer. silib_desc_util_makeCorrct639Code()
STATIC HERROR silib_table_makeCorrect639Code(HUINT8 *p)
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

// refer. silib_desc_util_textToUTF8()
STATIC HUINT32 silib_table_textToUTF8(const HCHAR *szLangCode, SIxTextEncoding_e eEnc, HUINT32 ucStrLen, const HUINT8 *pucSiText, HUINT8 **ppucDestStr)
{
	if (NULL == pucSiText)			return 0;
	if (NULL == ppucDestStr)		return 0;

	*ppucDestStr = SILIB_TEXT_MakeUtf8(eEnc, szLangCode, pucSiText, ucStrLen);

	return (*ppucDestStr) ? HxSTD_StrLen(*ppucDestStr) : 0;
}


#define _____PAT_____
STATIC HERROR silib_table_ParsePatSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_PatSec_t	*pstPat = &(pstTable->unInfo.stPat);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Pat;
	pstPat->eTableType			= eSIxTABLETYPE_Pat;
	pstPat->usTsId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstPat->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstPat->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 0;
	pstPos->ul1stDesLoopLen		= 0;		// PAT에 1st Descriptor Loop는 없음.
	pstPos->ul2ndLoopPos		= 8;		// First Program Position: Offset + 8
	pstPos->ul2ndLoopLen		= (ulSecLen >= 8) ? (ulSecLen - 8) : 0;

	return ERR_OK;
}


STATIC HERROR silib_table_ParsePatProgram (SIxTable_2ndLoop_t *pstSubItem, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	SIxTable_PatProgram_t			*pstProgram = &(pstSubItem->unInfo.stPat);

	pstProgram->eTableType		= eSIxTABLETYPE_Pat;
	pstProgram->usProgNum		= HxMACRO_Get16Bit(pucRaw);
	pstProgram->usProgMapPid	= HxMACRO_Get13Bit(pucRaw + 2);

	*pulNextPos		= 4;
	*pulDesLoopLen	= 0;				// PAT의 Program Info에 Descriptor는 없다.

	return ERR_OK;
}

#define _____PMT_____
STATIC HERROR silib_table_ParsePmtSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_PmtSec_t	*pstPmt = &(pstTable->unInfo.stPmt);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Pmt;
	pstPmt->eTableType			= eSIxTABLETYPE_Pmt;
	pstPmt->usProgNum			= HxMACRO_Get16Bit(pucRaw + 3);
	pstPmt->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstPmt->bCurNextInd			= pucRaw[5] & 0x01;
	pstPmt->usPcrPid			= HxMACRO_Get13Bit(pucRaw + 8);
	pstPmt->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 12;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + 10);
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen;
	pstPos->ul2ndLoopLen		= (ulSecLen >= pstPos->ul2ndLoopPos) ? (ulSecLen - pstPos->ul2ndLoopPos) : 0;

	return ERR_OK;
}

STATIC HERROR silib_table_ParsePmtElement (SIxTable_2ndLoop_t *pstSubItem, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	SIxTable_PmtElement_t			*pstElement = &(pstSubItem->unInfo.stPmt);

	pstElement->eTableType		= eSIxTABLETYPE_Pmt;
	pstElement->ucStreamType	= HxMACRO_Get8Bit(pucRaw);
	pstElement->usElemPid		= HxMACRO_Get13Bit(pucRaw + 1);

	*pulNextPos		= 5;
	*pulDesLoopLen	= HxMACRO_Get12Bit(pucRaw + 3);				// PMT Element Descriptor Length는 Offset 3~4 임.

	return ERR_OK;
}

#define _____NIT_____
STATIC HERROR silib_table_ParseNitSecInfo (SIxTable_PsiTable_t *pstTable, SIxTable_Type_e eSiType, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_NitSec_t	*pstNit = &(pstTable->unInfo.stNit);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSiType;
	pstNit->eTableType			= eSiType;
	pstNit->usNetId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstNit->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstNit->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 10;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + 8);
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen + 2;
	pstPos->ul2ndLoopLen		= HxMACRO_Get12Bit(pucRaw + pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen);

	return ERR_OK;
}

STATIC HERROR silib_table_ParseNitNetwork (SIxTable_2ndLoop_t *pstSubItem, SIxTable_Type_e eSiType, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	SIxTable_NitTs_t		*pstNitTs = &(pstSubItem->unInfo.stNit);

	pstNitTs->eTableType		= eSiType;
	pstNitTs->usTsId			= HxMACRO_Get16Bit(pucRaw);
	pstNitTs->usOrgNetId		= HxMACRO_Get16Bit(pucRaw + 2);

	*pulNextPos					= 6;
	*pulDesLoopLen 				= HxMACRO_Get12Bit(pucRaw + 4);

	return ERR_OK;
}

#define _____BAT_____
STATIC HERROR silib_table_ParseBatSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_BatSec_t	*pstBat = &(pstTable->unInfo.stBat);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Bat;
	pstBat->eTableType			= eSIxTABLETYPE_Bat;
	pstBat->usBqId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstBat->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstBat->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 10;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + 8);
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen + 2;
	pstPos->ul2ndLoopLen		= HxMACRO_Get12Bit(pucRaw + pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen);

	return ERR_OK;
}

STATIC HERROR silib_table_ParseBatTs (SIxTable_2ndLoop_t *pstSubItem, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	SIxTable_BatTs_t		*pstBatTs = &(pstSubItem->unInfo.stBat);

	pstBatTs->eTableType		= eSIxTABLETYPE_Bat;
	pstBatTs->usTsId			= HxMACRO_Get16Bit(pucRaw);
	pstBatTs->usOrgNetId		= HxMACRO_Get16Bit(pucRaw + 2);

	*pulNextPos					= 6;
	*pulDesLoopLen 				= HxMACRO_Get12Bit(pucRaw + 4);

	return ERR_OK;
}

#define _____SDT_____
STATIC HERROR silib_table_ParseSdtSecInfo (SIxTable_PsiTable_t *pstTable, SIxTable_Type_e eSiType, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_SdtSec_t	*pstSdt = &(pstTable->unInfo.stSdt);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSiType;
	pstSdt->eTableType			= eSiType;
	pstSdt->usTsId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstSdt->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstSdt->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstSdt->usOrgNetId			= HxMACRO_Get16Bit(pucRaw + 8);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 11;
	pstPos->ul1stDesLoopLen		= 0;
	pstPos->ul2ndLoopPos		= 11;
	pstPos->ul2ndLoopLen		= (ulSecLen >= pstPos->ul2ndLoopPos) ? (ulSecLen - pstPos->ul2ndLoopPos) : 0;

	return ERR_OK;
}

STATIC HERROR silib_table_ParseSdtService (SIxTable_2ndLoop_t *pstSubItem, SIxTable_Type_e eSiType, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	SIxTable_SdtService_t			*pstSdtSvc = &(pstSubItem->unInfo.stSdt);

	pstSdtSvc->eTableType		= eSiType;
	pstSdtSvc->usSvcId			= HxMACRO_Get16Bit(pucRaw);
	pstSdtSvc->bEitSched		= (pucRaw[2] & 0x02) >> 1;
	pstSdtSvc->bEitPreFollow	= (pucRaw[2] & 0x01);
	pstSdtSvc->ucRunningStatus	= (pucRaw[3] & 0xe0) >> 5;
	pstSdtSvc->bCaFree			= (pucRaw[3] & 0x10) >> 4;

	*pulNextPos		= 5;
	*pulDesLoopLen	= HxMACRO_Get12Bit(pucRaw + 3);				// PMT Element Descriptor Length는 Offset 3~4 임.

	return ERR_OK;
}

#define _____EIT_____
STATIC HERROR silib_table_ParseEitSecInfo (SIxTable_PsiTable_t *pstTable, SIxTable_Type_e eSiType, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_EitSec_t	*pstEit = &(pstTable->unInfo.stEit);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSiType;
	pstEit->eTableType			= eSiType;
	pstEit->usSvcId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstEit->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstEit->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstEit->usTsId				= HxMACRO_Get16Bit(pucRaw + 8);
	pstEit->usOrgNetId			= HxMACRO_Get16Bit(pucRaw + 10);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 14;
	pstPos->ul1stDesLoopLen		= 0;
	pstPos->ul2ndLoopPos		= 14;
	pstPos->ul2ndLoopLen		= (ulSecLen >= pstPos->ul2ndLoopPos) ? (ulSecLen - pstPos->ul2ndLoopPos) : 0;

	return ERR_OK;
}

STATIC HERROR silib_table_ParseEitEvent (SIxTable_2ndLoop_t *pstSubItem, SIxTable_Type_e eSiType, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	HUINT8						 aucUtcTime[5];
	SIxTable_EitEvent_t			*pstEitEvt = &(pstSubItem->unInfo.stEit);

	pstEitEvt->eTableType		= eSiType;
	pstEitEvt->usEvtId			= HxMACRO_Get16Bit(pucRaw);

	// Start Time
	HxSTD_MemCopy(aucUtcTime, (pucRaw + 2), 5);
	HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(aucUtcTime, &(pstEitEvt->utStartTime));

	// Duration
	HxSTD_MemCopy(aucUtcTime, (pucRaw + 7), 3);
	HLIB_DATETIME_ConvertBcdTimeToUnixTime(aucUtcTime, &(pstEitEvt->utDuration));

	pstEitEvt->ucRunStatus = (pucRaw[10] & 0xE0) >> 5;
	pstEitEvt->bCaFree = (pucRaw[10] & 0x10) >> 4;

	*pulNextPos		= 12;
	*pulDesLoopLen	= HxMACRO_Get12Bit(pucRaw + 10);				// PMT Element Descriptor Length는 Offset 3~4 임.

	return ERR_OK;
}

#define _____TDT_____
STATIC HERROR silib_table_ParseTdtSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT8				 aucUtcTime[5];
	HUINT32				 ulSecLen;
	SIxTable_TdtSec_t	*pstTdt = &(pstTable->unInfo.stTdt);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Tdt;
	pstTdt->eTableType			= eSIxTABLETYPE_Tdt;
	pstTdt->usExtId				= 0x00;				// N/A
	pstTdt->ucVer				= 0x00;				// N/A
	pstTdt->ulCrc32				= 0x00;				// N/A

	HxSTD_MemCopy(aucUtcTime, (pucRaw + 3), 5);
	HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(aucUtcTime, &(pstTdt->utUtcTime));

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 8;
	pstPos->ul1stDesLoopLen		= 0;
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen;
	pstPos->ul2ndLoopLen		= 0;

	return ERR_OK;
}

#define _____TOT_____
STATIC HERROR silib_table_ParseTotSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT8				 aucUtcTime[5];
	HUINT32				 ulSecLen;
	SIxTable_TotSec_t	*pstTot = &(pstTable->unInfo.stTot);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Tot;
	pstTot->eTableType			= eSIxTABLETYPE_Tot;
	pstTot->usExtId				= 0x00;				// N/A
	pstTot->ucVer				= 0x00;				// N/A
	pstTot->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	HxSTD_MemCopy(aucUtcTime, (pucRaw + 3), 5);
	HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(aucUtcTime, &(pstTot->utUtcTime));

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 10;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + 8);
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen;
	pstPos->ul2ndLoopLen		= 0;

	return ERR_OK;
}

#define _____DSI_____
STATIC INLINE void silib_table_GetDsmccMsgHeaderPos (HUINT8 *pucSection, HUINT8 **ppHeader)
{
	*ppHeader = pucSection + 8;
}

STATIC INLINE void silib_table_GetDsmccMsgHeaderLength (HUINT8 *pucHeader, HUINT32 *pulLength)
{
	HUINT8		 ucAdaptationLen;

	ucAdaptationLen = pucHeader[9];
	*pulLength = 12 + (HUINT32)ucAdaptationLen;
}

STATIC HERROR silib_table_ParseDsiSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen, ulLocalSecLen;
	HUINT32				 ulCnt, ulCurrPos, ulHeaderLen;
	HUINT32				 ulCompDesPos, ulCompDesLen;
	HUINT32				 ulGroupNum, ulGroupLoopPos, ulGroupLoopLen;
	HUINT8				*pucPos;
	SIxTable_DsiSec_t	*pstDsi = &(pstTable->unInfo.stDsi);

	ulCurrPos = 0;
	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;
	ulLocalSecLen = ulSecLen;

	pstTable->eTableType		= eSIxTABLETYPE_Dsi;
	pstDsi->eTableType			= eSIxTABLETYPE_Dsi;
	pstDsi->usExtId				= HxMACRO_Get16Bit(pucRaw + 3);

	// Skip the Header
	silib_table_GetDsmccMsgHeaderPos(pucRaw, &pucPos);
	ulCurrPos += 8;
	ulLocalSecLen -= 8;

	silib_table_GetDsmccMsgHeaderLength(pucPos, &ulHeaderLen);
	pucPos += ulHeaderLen;
	ulCurrPos += ulHeaderLen;
	ulLocalSecLen -= ulHeaderLen;

	// Server ID : 20byte
	if (ulLocalSecLen < 20)
	{
		HxLOG_Error("SecLen(%d)... parsing error\n", ulLocalSecLen);
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstDsi->aucServerId, pucPos, 20);
	pucPos += 20;
	ulCurrPos += 20;
	ulLocalSecLen -= 20;

	// Compotibility Descriptor
	if (ulLocalSecLen < 2)
	{
		HxLOG_Error("SecLen(%d)... parsing error\n", ulLocalSecLen);
		return ERR_FAIL;
	}

	ulCompDesPos = (ulCurrPos + 2);
	ulCompDesLen = HxMACRO_Get8Bit(pucPos+1);

	pucPos += (2 + ulCompDesLen);
	ulCurrPos += (2 + ulCompDesLen);
	ulLocalSecLen -= (2 + ulCompDesLen);

	// Private Data Length: Skip
	pucPos += 2;
	ulCurrPos += 2;
	ulLocalSecLen -= 2;

	// Number of Group
	ulGroupNum = HxMACRO_Get16Bit(pucPos);
	pucPos += 2;
	ulCurrPos += 2;
	ulLocalSecLen -= 2;

	ulGroupLoopPos = ulCurrPos;

	for (ulCnt = 0, ulGroupLoopLen = 0; ulCnt < ulGroupNum; ulCnt++)
	{
		HUINT16				 usGroupCompLen =0 , usGroupInfoLen= 0;

		// Skip Group ID(4byte) & Size(4byte)
		pucPos += 8;
		ulGroupLoopLen += 8;

		// Group Compatibility
		usGroupCompLen = HxMACRO_Get16Bit(pucPos);
		pucPos += (2 + usGroupCompLen);
		ulGroupLoopLen += (2 + usGroupCompLen);

		// Group Info Length
		usGroupInfoLen = HxMACRO_Get16Bit(pucPos);
		pucPos += (2 + usGroupInfoLen);
		ulGroupLoopLen += (2 + usGroupInfoLen);
	}

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= ulCompDesPos;
	pstPos->ul1stDesLoopLen		= ulCompDesLen;
	pstPos->ul2ndLoopPos		= ulGroupLoopPos;
	pstPos->ul2ndLoopLen		= ulGroupLoopLen;

	return ERR_OK;
}

STATIC HERROR silib_table_ParseDsiGroup (SIxTable_2ndLoop_t *pstSubItem, SIxTable_Type_e eSiType, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	HUINT32						 ulPos = 0, ulGrpCompLen, ulGrpCompNum;
	HUINT8						*pucPos = pucRaw;
	SIxTable_DsiGroup_t			*pstDsiGrp = &(pstSubItem->unInfo.stDsi);

	pstDsiGrp->eTableType		= eSiType;
	pstDsiGrp->ulGroupId		= HxMACRO_Get32Bit(pucPos);				pucPos += 4;	ulPos += 4;
	pstDsiGrp->ulGroupSize		= HxMACRO_Get32Bit(pucPos);				pucPos += 4;	ulPos += 4;

	ulGrpCompLen				= HxMACRO_Get16Bit(pucPos);				pucPos += 2;	ulPos += 2;
	ulGrpCompNum			= HxMACRO_Get16Bit(pucPos);				pucPos += 2;	ulPos += 2;
	ulGrpCompLen				-=2;

	(void)silib_table_InsertRawDescriptors (&(pstDsiGrp->pstGroupCompList), pucPos, ulGrpCompLen, TRUE);
	pucPos += ulGrpCompLen;		ulPos += ulGrpCompLen;

	*pulNextPos		= ulPos + 2;
	*pulDesLoopLen	= HxMACRO_Get16Bit(pucPos);

	return ERR_OK;
}

STATIC void silib_table_ClearDsiSection (SIxTable_PsiTable_t *pstTable)
{
	HxList_t			*pstItem, *pstDesItem;

	// 1. Section Info 내에 Memory free 해야 할 항목

	// 2. 2nd Loop 안에 Memory free 해야 할 항목
	for (pstItem = pstTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		SIxTable_2ndLoop_t		*pstSubInfo = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pstItem);

		if (NULL != pstSubInfo)
		{
			SIxTable_DsiGroup_t 	*pstDsiGrp = &(pstSubInfo->unInfo.stDsi);

			for (pstDesItem = pstDsiGrp->pstGroupCompList; NULL != pstDesItem; pstDesItem = pstDesItem->next)
			{
				HUINT8			*pucRaw = (HUINT8 *)HLIB_LIST_Data(pstDesItem);

				if (NULL != pucRaw) 			{ HLIB_STD_MemFree(pucRaw); }
			}

			pstDsiGrp->pstGroupCompList = HLIB_LIST_RemoveAll(pstDsiGrp->pstGroupCompList);
		}
	}
}

#define _____AIT_____
STATIC HERROR silib_table_ParseAitSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_AitSec_t	*pstAit = &(pstTable->unInfo.stAit);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Ait;
	pstAit->eTableType			= eSIxTABLETYPE_Ait;
	pstAit->usExtId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstAit->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstAit->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstAit->ucTestAppFlag		= (pucRaw[3] & 0x80) >> 7 ;
	pstAit->usAppicationType	= HxMACRO_Get15Bit(pucRaw + 3);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 10;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + 8);
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen + 2;
	pstPos->ul2ndLoopLen		= HxMACRO_Get12Bit(pucRaw + pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen);

	return ERR_OK;
}

STATIC HERROR silib_table_ParseAitApplication (SIxTable_2ndLoop_t *pstSubItem, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	SIxTable_AitApplication_t			*pstElement = &(pstSubItem->unInfo.stAit);

	pstElement->eTableType			= eSIxTABLETYPE_Ait;
	pstElement->ulOrganizaionId		= HxMACRO_Get32Bit(pucRaw);
	pstElement->ulApplicationId		= HxMACRO_Get16Bit(pucRaw + 4);
	pstElement->ucApplControlCode	= HxMACRO_Get8Bit(pucRaw + 6);

	*pulNextPos						= 9;
	*pulDesLoopLen					= HxMACRO_Get12Bit(pucRaw + 7);

	return ERR_OK;
}

#define _____ICT_____
// spec이 없어서 local_si_GetIctSectionInfoFromRaw()을 보고 parsing 구현
// spec 찾으면 재검토 해야 함 !!
STATIC HERROR silib_table_ParseIctSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen, ulLeftLen, i;
	HUINT8				*pucRawPos;
	SIxTable_IctSec_t	*pstIct = &(pstTable->unInfo.stIct);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Ict;
	pstIct->eTableType			= eSIxTABLETYPE_Ict;
	pstIct->usExtId				= HxMACRO_Get16Bit(pucRaw + 3);		// 실제 이런 field는 없으나 general psi section form에 맞추기 위해..

	pstIct->ucManufacturerCode 	= HxMACRO_Get8Bit(pucRaw + 3);
	pstIct->ucHardwareCode 		= HxMACRO_Get8Bit(pucRaw + 4);

	HxLOG_Print("[%s][%d] pstIct->ucManufacturerCode : %02x, pstIct->ucHardwareCode: %02x \n", __FUNCTION__, __LINE__, pstIct->ucManufacturerCode, pstIct->ucHardwareCode);

	i 			= 0;
	ulLeftLen 	= ulSecLen - 8;
	pucRawPos 	= pucRaw + 8;
	while(ulLeftLen > 0)
	{
		pstIct->aucSeqNumber[i] 	= HxMACRO_Get8Bit(pucRawPos++);
		pstIct->aucTableId[i] 		= HxMACRO_Get8Bit(pucRawPos++);
		pstIct->ausDownloadTime[i] 	= HxMACRO_Get16Bit(pucRawPos);
		pucRawPos += 2;

		ulLeftLen -= 4;

		HxLOG_Print("[%s][%d]  i = %d, ulLeftLen = %d, SeqNum (0x%x), TableId (0x%x), DownTime (0x%x) \n",
				__FUNCTION__, __LINE__, i, ulLeftLen, pstIct->aucSeqNumber[i], pstIct->aucTableId[i], pstIct->ausDownloadTime[i]);

		i++;
	}

	pstIct->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstIct->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 8;
	pstPos->ul1stDesLoopLen		= 0;
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen;
	pstPos->ul2ndLoopLen		= 0;

	return ERR_OK;
}

#define _____SGT_____
STATIC HERROR silib_table_ParseSgtSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_SgtSec_t	*pstSgt = &(pstTable->unInfo.stSgt);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Sgt;
	pstSgt->eTableType			= eSIxTABLETYPE_Sgt;
	pstSgt->usServiceListId		= HxMACRO_Get16Bit(pucRaw + 3);
	pstSgt->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstSgt->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 12;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + 10);
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen + 2;
	pstPos->ul2ndLoopLen		= HxMACRO_Get12Bit(pucRaw + pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen);

	return ERR_OK;
}

STATIC HERROR silib_table_ParseSgtService (SIxTable_2ndLoop_t *pstSubItem, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	SIxTable_SgtService_t			*pstSgtService = &(pstSubItem->unInfo.stSgt);

	pstSgtService->eTableType		= eSIxTABLETYPE_Sgt;
	pstSgtService->usServiceId		= HxMACRO_Get16Bit(pucRaw);
	pstSgtService->usTsId			= HxMACRO_Get16Bit(pucRaw + 2);
	pstSgtService->usOnId			= HxMACRO_Get16Bit(pucRaw + 4);
	pstSgtService->usLcn			= HxMACRO_Get16Bit(pucRaw + 6) >> 2;
	pstSgtService->ucVisibleSvcFlag	= (pucRaw[7] & 0x02) >> 1 ;
	pstSgtService->ucNewSvcFlag		= (pucRaw[7] & 0x01);
	pstSgtService->usGenreCode		= HxMACRO_Get16Bit(pucRaw + 8);

	*pulNextPos						= 12;
	*pulDesLoopLen					= HxMACRO_Get12Bit(pucRaw + 10);

	return ERR_OK;
}

#define _____UNT_____
STATIC HERROR silib_table_ParseUntSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_UntSec_t	*pstUnt = &(pstTable->unInfo.stUnt);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Unt;
	pstUnt->eTableType			= eSIxTABLETYPE_Unt;
	pstUnt->usExtId				= HxMACRO_Get16Bit(pucRaw + 3);	// for generic psi section format, action type + oui hash
	pstUnt->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstUnt->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstUnt->ucActionType		= HxMACRO_Get8Bit(pucRaw + 3);
	pstUnt->ucOuiHash			= HxMACRO_Get8Bit(pucRaw + 4);
	pstUnt->ulOui				= HxMACRO_Get24Bit(pucRaw + 8);
	pstUnt->ucProcessingOrder	= HxMACRO_Get8Bit(pucRaw + 11);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 14;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + 12);
	pstPos->ul2ndLoopPos		= pstPos->ul1stDesLoopPos + pstPos->ul1stDesLoopLen;
	pstPos->ul2ndLoopLen		= (ulSecLen >= pstPos->ul2ndLoopPos) ? (ulSecLen - pstPos->ul2ndLoopPos) : 0;

	return ERR_OK;
}

STATIC HERROR silib_table_ParseUntInfo (SIxTable_2ndLoop_t *pstSubItem, SIxTable_Type_e eSiType, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	HUINT32						 ulPos = 0;
	SIxTable_UntInfo_t			*pstUntInfo = &(pstSubItem->unInfo.stUnt);

	HUINT32						 ulCompDesLen, ulCompDesNum;
//	HUINT32						 ulDesLen;
	HUINT32						 ulPlatLoopLen, ulDesLoopLen;

	pstUntInfo->eTableType		= eSiType;

	/**
	  *	compatiblity descriptor
	**/
	ulCompDesLen 		= HxMACRO_Get16Bit(pucRaw);
	ulCompDesNum 		= HxMACRO_Get16Bit(pucRaw + 2);
	ulPos += 4;

#if 1
	/* desc parsing하지 않고 raw를 그대로 넣어준다 - svc level에서 parsing해서 사용해야 함 */
	ulCompDesLen -= 2;

	(void)silib_table_InsertRawDescriptors(&(pstUntInfo->pstCompDesLoop), (pucRaw + ulPos), ulCompDesLen, TRUE);
	ulPos += ulCompDesLen;
#else
	/* desc parsing해서 넣어준다 - svc level에서 parsing 없이 사용 가능 */
	pstUntInfo->pstCompDesLoop = NULL;
	while (ulCompDesNum > 0)
	{
		SIxTable_UntCompDes_t	*pstCompDes = (SIxTable_UntCompDes_t *)HLIB_STD_MemAlloc(sizeof(SIxTable_UntCompDes_t));

		if (pstCompDes == NULL)
		{
			HxLOG_Error("UNT compatibility desc. alloc failed...!! \n");
			return ERR_FAIL;
		}

		ulDesLen 						= HxMACRO_Get8Bit(pucRaw + ulPos + 1) + 2;

		pstCompDes->ucDescType 			= HxMACRO_Get8Bit (pucRaw + ulPos);
		pstCompDes->ucSpecifierType 	= HxMACRO_Get8Bit (pucRaw + ulPos + 2);
		pstCompDes->ulSpecifierData 	= HxMACRO_Get24Bit(pucRaw + ulPos + 3);
		pstCompDes->usModel 			= HxMACRO_Get16Bit(pucRaw + ulPos + 6);
		pstCompDes->usVersion 			= HxMACRO_Get16Bit(pucRaw + ulPos + 8);
		pstCompDes->pstSubDesc 			= NULL;		// sub descriptor는 skip~

		pstUntInfo->pstCompDesLoop = HLIB_LIST_Append(pstUntInfo->pstCompDesLoop, (void *)pstCompDes);

		ulPos += ulDesLen;
		ulCompDesNum--;
	}
#endif

	/**
	  *	platform loop
	**/
	ulPlatLoopLen = HxMACRO_Get16Bit(pucRaw + ulPos);
	ulPos += 2;

	// target des loop는 skip~
	ulDesLoopLen = HxMACRO_Get12Bit(pucRaw + ulPos);
	ulPos += (ulDesLoopLen + 2);
	pstUntInfo->pstTargetDesLoop = NULL;

	// operational des loop
	ulDesLoopLen = HxMACRO_Get12Bit(pucRaw + ulPos);
	ulPos += 2;
	(void)silib_table_InsertRawDescriptors(&(pstUntInfo->pstOperationalDesLoop), (pucRaw + ulPos), ulDesLoopLen, TRUE);

	/**
	  *	2nd loop > desc. loop는 null 처리한다
	**/
	*pulNextPos		= ulPos + ulDesLoopLen;
	*pulDesLoopLen	= 0;

	return ERR_OK;
}

STATIC void silib_table_ClearUntSection (SIxTable_PsiTable_t *pstTable)
{
	HxList_t			*pstItem, *pstDesItem;

	for (pstItem = pstTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		SIxTable_2ndLoop_t		*pstSubInfo = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pstItem);

		if (NULL != pstSubInfo)
		{
			SIxTable_UntInfo_t			*pstUntInfo = &(pstSubInfo->unInfo.stUnt);

			// 1. compatibility desc. 처리
			for (pstDesItem = pstUntInfo->pstCompDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
			{
#if 1
				/* raw des 사용하는 경우 */
				HUINT8	*pstCompDes = (HUINT8 *)HLIB_LIST_Data(pstDesItem);
#else
				/* des parsing 해서 사용하는 경우 */
				SIxTable_UntCompDes_t	*pstCompDes = (SIxTable_UntCompDes_t *)HLIB_LIST_Data(pstDesItem);
#endif

				if (NULL != pstCompDes)
				{
					HLIB_STD_MemFree(pstCompDes);
				}
			}
			pstUntInfo->pstCompDesLoop = HLIB_LIST_RemoveAll(pstUntInfo->pstCompDesLoop);

			// 2. target desc 처리 -> 현재 사용하지 않으므로 생략

			// 3. operational desc 처리
			for (pstDesItem = pstUntInfo->pstOperationalDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
			{
				HUINT8			*pucRaw = (HUINT8 *)HLIB_LIST_Data(pstDesItem);

				if (NULL != pucRaw)
				{
					HLIB_STD_MemFree(pucRaw);
				}
			}
			pstUntInfo->pstOperationalDesLoop = HLIB_LIST_RemoveAll(pstUntInfo->pstOperationalDesLoop);
		}
	}
}

#define _____RCT_____
STATIC HERROR silib_table_ParseRctSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	HUINT32				 ulLinkInfoNum, ulLinkInfoLen, ulPos, ulTmpLinkInfoLen;
	SIxTable_RctSec_t	*pstRct = &(pstTable->unInfo.stRct);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Rct;
	pstRct->eTableType			= eSIxTABLETYPE_Rct;
	pstRct->usSvcId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstRct->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstRct->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstRct->ucTableIdExtFlag	= (pucRaw[1] & 0x40) >> 6;
	pstRct->usYearOffset		= HxMACRO_Get16Bit(pucRaw + 8);

	//get link info len
	ulLinkInfoNum 				= HxMACRO_Get8Bit(pucRaw + 10);
	pstRct->ucLinkCount			= ulLinkInfoNum;
	ulLinkInfoLen				= 0;
	ulPos						= 11;
	while (ulLinkInfoNum > 0)
	{
#if 1
		ulTmpLinkInfoLen 		= (HxMACRO_Get12Bit(pucRaw + ulPos) + 2);
		ulLinkInfoLen 			+= ulTmpLinkInfoLen;
		ulPos				+= ulTmpLinkInfoLen;
#else
		ulLinkInfoLen 			+= (HxMACRO_Get12Bit(pucRaw + ulPos) + 2);
		ulPos					+= (ulLinkInfoLen);
#endif
		ulLinkInfoNum--;
	}

	// generic psi table 구조를 유지하기 위해 1st loop에 있는 link info를 2nd loop에 넣고
	// 2nd loop에 있는 desc loop를 1st loop에 넣는다.
	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul2ndLoopPos		= 11;
	pstPos->ul2ndLoopLen		= ulLinkInfoLen;
	pstPos->ul1stDesLoopPos		= pstPos->ul2ndLoopPos + ulLinkInfoLen + 2;
	pstPos->ul1stDesLoopLen		= HxMACRO_Get12Bit(pucRaw + pstPos->ul2ndLoopPos + ulLinkInfoLen);

	return ERR_OK;
}

STATIC void silib_table_FreeRctPromoText (void *pvData)
{
	SIxTable_RctPromoText_t		*pstPromoText = (SIxTable_RctPromoText_t *)pvData;

	if (NULL != pstPromoText)
	{
		if (pstPromoText->pszPromoText != NULL)
		{
			HLIB_STD_MemFree (pstPromoText->pszPromoText);
		}

		HLIB_STD_MemFree (pstPromoText);
	}

	return;
}

STATIC HERROR silib_table_ParseRctInfo (SIxTable_2ndLoop_t *pstSubItem, SIxTable_Type_e eSiType, SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	HUINT32						 ulPos = 0;
	SIxTable_RctLinkInfo_t		*pstRctInfo = &(pstSubItem->unInfo.stRct);

	HUINT32						 ulLinkInfoLen;
	HUINT32						 ulPromoTxtNum;

	pstRctInfo->eTableType		= eSiType;
	ulLinkInfoLen				= HxMACRO_Get12Bit(pucRaw);
	ulPos += 2;

	pstRctInfo->ucLinkType		= (pucRaw[ulPos] & 0xf0) >> 4;
	pstRctInfo->ucHowSchemeId	= ((pucRaw[ulPos] & 0x03) << 4) | ((pucRaw[ulPos+1] & 0xf0) >> 4);
	pstRctInfo->usTermId		= ((pucRaw[ulPos+1] & 0x0f) << 4) | pucRaw[ulPos+2];
	ulPos += 3;

	pstRctInfo->ucGroupId		= (pucRaw[ulPos] & 0xf0) >> 4;
	pstRctInfo->ucPrecedence	= pucRaw[ulPos] & 0x0f;
	ulPos++;

	// media uri
	if ((pstRctInfo->ucLinkType == 0x00) || (pstRctInfo->ucLinkType == 0x02))
	{
		pstRctInfo->ucMediaUriLen	= pucRaw[ulPos];
		pstRctInfo->pucMediaUriByte	= NULL;
		ulPos++;

		if (pstRctInfo->ucMediaUriLen > 0)
		{
			HUINT8		*pucMediaUri = (HUINT8 *)HLIB_STD_MemCalloc(pstRctInfo->ucMediaUriLen + 1);
			if (pucMediaUri == NULL)
			{
				HxLOG_Error("Media uri byte alloc failed...!! \n");
				goto err_exit;
			}

			HxSTD_MemCopy(pucMediaUri, &pucRaw[ulPos], sizeof(HUINT8) * pstRctInfo->ucMediaUriLen);

			pstRctInfo->pucMediaUriByte = pucMediaUri;
			ulPos += pstRctInfo->ucMediaUriLen;
		}
	}

	pstRctInfo->pstDvbBinLocator = NULL;

	// dvb binary locator
	if ((pstRctInfo->ucLinkType == 0x01) || (pstRctInfo->ucLinkType == 0x02))
	{
		SIxTable_RctDvbBinLocator_t		*pstLocator = (SIxTable_RctDvbBinLocator_t *)HLIB_STD_MemCalloc(sizeof(SIxTable_RctDvbBinLocator_t));
		if (pstLocator == NULL)
		{
			HxLOG_Error("Dvb binary locator alloc failed...!! \n");
			goto err_exit;
		}

		pstLocator->eTableType 				= eSiType;
		pstLocator->ucIdType 				= (pucRaw[ulPos] & 0xc0) >> 6;
		pstLocator->ucSchTimeReliability 	= (pucRaw[ulPos] & 0x20) >> 5;
		pstLocator->ucInlineService 		= (pucRaw[ulPos] & 0x10) >> 4;
		pstLocator->usStartDate 			= ((pucRaw[ulPos] & 0x07) << 6) | ((pucRaw[ulPos+1] & 0xfc) >> 2);

		if (pstLocator->ucInlineService == 0)
		{
			pstLocator->usDvbSvcTripletId	= ((pucRaw[ulPos+1] & 0x03) << 8) | pucRaw[ulPos+2];
			ulPos += 3;
		}
		else
		{
			ulPos += 2;
			pstLocator->usTsId				= HxMACRO_Get16Bit(pucRaw + ulPos);
			pstLocator->usOnId				= HxMACRO_Get16Bit(pucRaw + ulPos + 2);
			pstLocator->usSvcId				= HxMACRO_Get16Bit(pucRaw + ulPos + 4);
			ulPos += 6;
		}

		pstLocator->usStartTime 			= HxMACRO_Get16Bit(pucRaw + ulPos);
		pstLocator->usDuration 				= HxMACRO_Get16Bit(pucRaw + ulPos + 2);
		ulPos += 4;

		switch(pstLocator->ucIdType)
		{
			case 0x01:
				pstLocator->usEventId 		= HxMACRO_Get16Bit(pucRaw + ulPos);
				ulPos += 2;
				break;

			case 0x02:
				pstLocator->usTvaId 		= HxMACRO_Get16Bit(pucRaw + ulPos);
				ulPos += 2;
				break;

			case 0x03:
				pstLocator->usTvaId 		= HxMACRO_Get16Bit(pucRaw + ulPos);
				pstLocator->ucComponent		= HxMACRO_Get8Bit(pucRaw + ulPos + 2);
				ulPos += 3;
				break;

			case 0x00:
				if (pstLocator->ucSchTimeReliability == 0x01)
				{
					pstLocator->ucEarlyStartWindow 	= (pucRaw[ulPos] & 0xe0) >> 5;
					pstLocator->ucLateEndWindow		= pucRaw[ulPos] & 0x1f;
					ulPos++;
				}
				break;

			default:
				HxLOG_Error("invalid identifier type (0x%X) \n", pstLocator->ucIdType);
				break;
		}

		pstRctInfo->pstDvbBinLocator = pstLocator;
	}

	// promotional text
	pstRctInfo->pstPromoTextLoop	= NULL;
	ulPromoTxtNum					= pucRaw[ulPos] & 0x3f;
	ulPos++;

	while (ulPromoTxtNum > 0)
	{
		//HUINT8						 ucTxtLen = 0;
		HUINT32						 ulUtf8Len = 0;
		SIxTable_RctPromoText_t		*pstPromoText = (SIxTable_RctPromoText_t *)HLIB_STD_MemCalloc(sizeof(SIxTable_RctPromoText_t));

		if (pstPromoText == NULL)
		{
			HxLOG_Error("Promotional text structure alloc failed...!! \n");
			goto err_exit;
		}

		pstPromoText->eTableType = eSiType;

		// iso 639
		HxSTD_MemCopy(pstPromoText->szIso639LangCode, &pucRaw[ulPos], 3);
		pstPromoText->szIso639LangCode[3] = '\0';
		silib_table_makeCorrect639Code(pstPromoText->szIso639LangCode);
		ulPos += 3;

		// text len
		pstPromoText->ucTxtLen = pucRaw[ulPos];
		ulPos++;

		if (pstPromoText->ucTxtLen > 0)
		{
			HUINT8 *pszText = NULL;

			// promotional text
			ulUtf8Len = silib_table_textToUTF8((const HCHAR *)pstPromoText->szIso639LangCode, eTextEnc, (HUINT32)pstPromoText->ucTxtLen, (const HUINT8 *)&pucRaw[ulPos], &pszText);
			if ((pszText == NULL) || (ulUtf8Len < 1))
			{
				HxLOG_Error("silib_table_textToUTF8() failed..!! \n");
				HLIB_STD_MemFree(pstPromoText);
				if (pszText)
					HLIB_STD_MemFree(pszText);
				goto err_exit;
			}

			ulPos += pstPromoText->ucTxtLen;

			pstPromoText->ucTxtLen = ulUtf8Len;
			pstPromoText->pszPromoText = (HUINT8 *)HLIB_STD_MemAlloc(ulUtf8Len + 1);
			if (pstPromoText->pszPromoText == NULL)
			{
				HxLOG_Error("Promo text alloc failed..!! \n");
				HLIB_STD_MemFree(pszText);
				HLIB_STD_MemFree(pstPromoText);
				goto err_exit;
			}

			HxSTD_MemCopy(pstPromoText->pszPromoText, pszText, ulUtf8Len);
			pstPromoText->pszPromoText[ulUtf8Len] = '\0';

			HLIB_STD_MemFree(pszText);
		}

		pstRctInfo->pstPromoTextLoop = HLIB_LIST_Append(pstRctInfo->pstPromoTextLoop, (void *)pstPromoText);
		ulPromoTxtNum--;
	}

	pstRctInfo->ucDefaultIconFlag	= (pucRaw[ulPos] & 0x80) >> 7;
	pstRctInfo->ucIconId			= (pucRaw[ulPos] & 0x70) >> 4;

 	*pulNextPos		= ulPos + 2;
	*pulDesLoopLen	= HxMACRO_Get12Bit(pucRaw + ulPos);

	return ERR_OK;

err_exit:

	if(pstRctInfo->pucMediaUriByte)		HLIB_STD_MemFree(pstRctInfo->pucMediaUriByte);
	if(pstRctInfo->pstDvbBinLocator)		HLIB_STD_MemFree(pstRctInfo->pstDvbBinLocator);
 	if(pstRctInfo->pstPromoTextLoop)		HLIB_LIST_RemoveAllFunc(pstRctInfo->pstPromoTextLoop, silib_table_FreeRctPromoText);

	return ERR_FAIL;
}

STATIC void silib_table_ClearRctSection (SIxTable_PsiTable_t *pstTable)
{
	HxList_t			*pstItem;

	for (pstItem = pstTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		SIxTable_2ndLoop_t		*pstSubInfo = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pstItem);

		if (NULL != pstSubInfo)
		{
			SIxTable_RctLinkInfo_t			*pstRctInfo = &(pstSubInfo->unInfo.stRct);

			// media uri byte
			if (pstRctInfo->pucMediaUriByte != NULL)
			{
				HLIB_STD_MemFree(pstRctInfo->pucMediaUriByte);
			}

			// dvb binary locator
			if (pstRctInfo->pstDvbBinLocator != NULL)
			{
				HLIB_STD_MemFree(pstRctInfo->pstDvbBinLocator);
			}

			// promotional text
			if (pstRctInfo->pstPromoTextLoop != NULL)
			{
				HLIB_LIST_RemoveAllFunc(pstRctInfo->pstPromoTextLoop, silib_table_FreeRctPromoText);
			}
		}
	}

	return;
}

#define _____CAT_____
STATIC HERROR silib_table_ParseCatSecInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HUINT32				 ulSecLen;
	SIxTable_CatSec_t	*pstCat = &(pstTable->unInfo.stCat);

	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) - 1;

	pstTable->eTableType		= eSIxTABLETYPE_Cat;
	pstCat->eTableType			= eSIxTABLETYPE_Cat;
	pstCat->usExtId				= HxMACRO_Get16Bit(pucRaw + 3);
	pstCat->ucVer				= (pucRaw[5] & 0x3E) >> 1;
	pstCat->ulCrc32				= HxMACRO_Get32Bit(pucRaw + ulSecLen);

	pstPos->ulSecLen 			= ulSecLen;
	pstPos->ul1stDesLoopPos		= 8;
	pstPos->ul1stDesLoopLen		= (ulSecLen >= 8) ? (ulSecLen - 8) : 0;
	pstPos->ul2ndLoopPos		= 0;
	pstPos->ul2ndLoopLen		= 0;

	return ERR_OK;
}

#define _____GENERAL_PSI_SECTION_____
STATIC HERROR silib_table_ParseSectionInfo (SIxTable_PsiTable_t *pstTable, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos)
{
	HERROR					 hErr = ERR_FAIL;

	switch (*pucRaw)
	{
	case eSIxTABLEID_PAT:
		hErr = silib_table_ParsePatSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_PMT:
		hErr = silib_table_ParsePmtSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_NIT_ACT:
		hErr = silib_table_ParseNitSecInfo(pstTable, eSIxTABLETYPE_NitAct, pucRaw, pstPos);
		break;

	case eSIxTABLEID_NIT_OTH:
		hErr = silib_table_ParseNitSecInfo(pstTable, eSIxTABLETYPE_NitOth, pucRaw, pstPos);
		break;

	case eSIxTABLEID_BAT:
		hErr = silib_table_ParseBatSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_SDT_ACT:
		hErr = silib_table_ParseSdtSecInfo(pstTable, eSIxTABLETYPE_SdtAct, pucRaw, pstPos);
		break;

	case eSIxTABLEID_SDT_OTH:
		hErr = silib_table_ParseSdtSecInfo(pstTable, eSIxTABLETYPE_SdtOth, pucRaw, pstPos);
		break;

	case eSIxTABLEID_EIT_ACT_PF:
		hErr = silib_table_ParseEitSecInfo(pstTable, eSIxTABLETYPE_EitActPf, pucRaw, pstPos);
		break;

	case eSIxTABLEID_EIT_OTH_PF:
		hErr = silib_table_ParseEitSecInfo(pstTable, eSIxTABLETYPE_EitOthPf, pucRaw, pstPos);
		break;

	case eSIxTABLEID_TDT:
		hErr = silib_table_ParseTdtSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_TOT:
		hErr = silib_table_ParseTotSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_DSI:
		hErr = silib_table_ParseDsiSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_AIT:
		hErr = silib_table_ParseAitSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_ICT:
		hErr = silib_table_ParseIctSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_SGT:
		hErr = silib_table_ParseSgtSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_UNT:
		hErr = silib_table_ParseUntSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_RCT:
		hErr = silib_table_ParseRctSecInfo(pstTable, pucRaw, pstPos);
		break;

	case eSIxTABLEID_CAT:
		hErr = silib_table_ParseCatSecInfo(pstTable, pucRaw, pstPos);
		break;

	default:
		// EIT Other는 너무 많으니 여기서 처리한다.
		if ((*pucRaw >= eSIxTABLEID_EIT_ACT_SC_0) && (*pucRaw <= eSIxTABLEID_EIT_ACT_SC_15))
		{
			hErr = silib_table_ParseEitSecInfo(pstTable, eSIxTABLETYPE_EitActSc, pucRaw, pstPos);
			break;
		}
		else if ((*pucRaw >= eSIxTABLEID_EIT_OTH_SC_0) && (*pucRaw <= eSIxTABLEID_EIT_OTH_SC_15))
		{
			hErr = silib_table_ParseEitSecInfo(pstTable, eSIxTABLETYPE_EitOthSc, pucRaw, pstPos);
			break;
		}

		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

STATIC HERROR silib_table_ParseSubItem (HUINT8 ucTableId, SIxTable_2ndLoop_t *pstSubItem, SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw, HUINT32 *pulNextPos, HUINT32 *pulDesLoopLen)
{
	HERROR					 hErr = ERR_FAIL;

	switch (ucTableId)
	{
	case eSIxTABLEID_PAT:
		hErr = silib_table_ParsePatProgram(pstSubItem, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_PMT:
		hErr = silib_table_ParsePmtElement(pstSubItem, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_NIT_ACT:
		hErr = silib_table_ParseNitNetwork(pstSubItem, eSIxTABLETYPE_NitAct, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_NIT_OTH:
		hErr = silib_table_ParseNitNetwork(pstSubItem, eSIxTABLETYPE_NitOth, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_BAT:
		hErr = silib_table_ParseBatTs(pstSubItem, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_SDT_ACT:
		hErr = silib_table_ParseSdtService(pstSubItem, eSIxTABLETYPE_SdtAct, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_SDT_OTH:
		hErr = silib_table_ParseSdtService(pstSubItem, eSIxTABLETYPE_SdtOth, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_EIT_ACT_PF:
		hErr = silib_table_ParseEitEvent(pstSubItem, eSIxTABLETYPE_EitActPf, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_EIT_OTH_PF:
		hErr = silib_table_ParseEitEvent(pstSubItem, eSIxTABLETYPE_EitOthPf, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_DSI:
		hErr = silib_table_ParseDsiGroup(pstSubItem, eSIxTABLETYPE_Dsi, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_UNT:
		hErr = silib_table_ParseUntInfo(pstSubItem, eSIxTABLETYPE_Unt, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_RCT:
		hErr = silib_table_ParseRctInfo(pstSubItem, eSIxTABLETYPE_Rct, eTextEnc, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_AIT:
		hErr = silib_table_ParseAitApplication(pstSubItem, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	case eSIxTABLEID_SGT:
		hErr = silib_table_ParseSgtService(pstSubItem, pucRaw, pulNextPos, pulDesLoopLen);
		break;

	default:
		// EIT Other는 너무 많으니 여기서 처리한다.
		if ((*pucRaw >= eSIxTABLEID_EIT_ACT_SC_0) && (*pucRaw <= eSIxTABLEID_EIT_ACT_SC_15))
		{
			hErr = silib_table_ParseEitEvent(pstSubItem, eSIxTABLETYPE_EitActSc, pucRaw, pulNextPos, pulDesLoopLen);
			break;
		}
		else if ((*pucRaw >= eSIxTABLEID_EIT_OTH_SC_0) && (*pucRaw <= eSIxTABLEID_EIT_OTH_SC_15))
		{
			hErr = silib_table_ParseEitEvent(pstSubItem, eSIxTABLETYPE_EitOthSc, pucRaw, pulNextPos, pulDesLoopLen);
			break;
		}

		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

STATIC HERROR silib_table_Parse2ndLoop (SIxTable_PsiTable_t *pstTable, SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw, silib_table_posInfo_t *pstPos, HBOOL bRawDup)
{
	HUINT8					 ucTableTag = *pucRaw;
	HINT32					 nLeftLen = (HINT32)pstPos->ul2ndLoopLen;
	HERROR					 hErr = ERR_FAIL;

	pucRaw += pstPos->ul2ndLoopPos;

	while (nLeftLen > 0)
	{
		HUINT32					 ulNextPos = 0, ulDesLoopLen = 0;
		SIxTable_2ndLoop_t		*pstSubItem = NULL;

		pstSubItem = (SIxTable_2ndLoop_t *)HLIB_STD_MemCalloc(sizeof(SIxTable_2ndLoop_t));
		if (NULL == pstSubItem)
		{
			HxLOG_Critical("memory allocation failed !!!\n");
			return ERR_FAIL;
		}

		pstTable->pst2ndLoop = HLIB_LIST_Append(pstTable->pst2ndLoop, (void *)pstSubItem);

		hErr = silib_table_ParseSubItem(ucTableTag, pstSubItem, eTextEnc, pucRaw, &ulNextPos, &ulDesLoopLen);
		if (ERR_OK != hErr)
		{
			HxLOG_Critical("Sub Item parsing failed!!! tableID(%02x)\n", ucTableTag);
			return ERR_FAIL;
		}

		pucRaw += ulNextPos;
		nLeftLen -= ulNextPos;

		if (ulDesLoopLen > 0)
		{
			if (ulDesLoopLen > nLeftLen)
			{
				// left length보다 des loop length가 큰 경우는 없을 것 같은데...
				HxLOG_Critical("invalid desLoopLen(%d) : leftLen(%d)\n", ulDesLoopLen, nLeftLen);
				return ERR_OK;
			}

			hErr = silib_table_InsertRawDescTo2ndLoop(pstSubItem, pucRaw, ulDesLoopLen, bRawDup);
			if (hErr != ERR_OK)
			{
				HxLOG_Critical("silib_table_InsertRawDescTo2ndLoop() failed.. hErr(%d) \n", hErr);
			}

			pucRaw += ulDesLoopLen;
			nLeftLen -= ulDesLoopLen;
		}
	}

	return ERR_OK;
}

STATIC HERROR silib_table_ParseDefaultPsiSection (SIxTable_PsiTable_t *pstTable, SIxTextEncoding_e eTextEnc, HUINT8 *pucRawSec)
{
	silib_table_posInfo_t			 stPos;
	HERROR						 hErr;

	// Parse the PSI section info:
	hErr = silib_table_ParseSectionInfo(pstTable, pucRawSec, &stPos);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("silib_table_ParseSectionInfo err:\n");
		return ERR_FAIL;
	}

	// Parse the 1st Descriptor Loop
	if (stPos.ul1stDesLoopLen > 0)
	{
		hErr = silib_table_InsertRawDescTo1stLoop(pstTable, pucRawSec + stPos.ul1stDesLoopPos, stPos.ul1stDesLoopLen, pstTable->bRawDup);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("silib_table_InsertRawDescTo1stLoop err:\n");
			return ERR_FAIL;
		}
	}

	// Parse the 2nd Info Loop
	if (stPos.ul2ndLoopLen > 0)
	{
		hErr = silib_table_Parse2ndLoop(pstTable, eTextEnc, pucRawSec, &stPos, pstTable->bRawDup);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("silib_table_Parse2ndLoop err:\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

STATIC void silib_table_ClearDefaultRawSection (SIxTable_PsiTable_t *pstTable)
{
	HBOOL				 bRawDup = pstTable->bRawDup;
	HxList_t			*pstItem, *pstDesItem;

	if (TRUE == bRawDup)
	{
		for (pstDesItem = pstTable->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
		{
			HUINT8			*pucRaw = (HUINT8 *)HLIB_LIST_Data(pstDesItem);

			if (NULL != pucRaw)				{ HLIB_STD_MemFree(pucRaw); }
		}
	}

	for (pstItem = pstTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		SIxTable_2ndLoop_t		*pstSubInfo = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pstItem);

		if (NULL != pstSubInfo)
		{
			if (TRUE == bRawDup)
			{
				for (pstDesItem = pstSubInfo->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
				{
					HUINT8			*pucRaw = (HUINT8 *)HLIB_LIST_Data(pstDesItem);

					if (NULL != pucRaw) 			{ HLIB_STD_MemFree(pucRaw); }
				}
			}

			pstSubInfo->pstDesLoop = HLIB_LIST_RemoveAll(pstSubInfo->pstDesLoop);
			HLIB_STD_MemFree(pstSubInfo);
		}
	}

	pstTable->pst2ndLoop = HLIB_LIST_RemoveAll(pstTable->pst2ndLoop);
	pstTable->pstDesLoop = HLIB_LIST_RemoveAll(pstTable->pstDesLoop);

	HxSTD_MemSet(pstTable, 0, sizeof(SIxTable_PsiTable_t));
	pstTable->bRawDup = bRawDup;
}


STATIC void silib_table_ClearRawSection (SIxTable_PsiTable_t *pstTable)
{
	switch (pstTable->eTableType)
	{
	case eSIxTABLETYPE_Dsi:
		silib_table_ClearDsiSection(pstTable);
		silib_table_ClearDefaultRawSection(pstTable);
		break;

	case eSIxTABLETYPE_Unt:
		silib_table_ClearUntSection(pstTable);
		silib_table_ClearDefaultRawSection(pstTable);
		break;

	case eSIxTABLETYPE_Rct:
		silib_table_ClearRctSection(pstTable);
		silib_table_ClearDefaultRawSection(pstTable);
		break;

	default:
		silib_table_ClearDefaultRawSection(pstTable);
		break;
	}
}

STATIC HERROR silib_table_DuplicateTable (SIxTable_PsiTable_t *pstDstTable, SIxTable_PsiTable_t *pstSrcTable)
{
	HxList_t			*pstItem, *pstDesItem;

	pstDstTable->eTableType	= pstSrcTable->eTableType;
	pstDstTable->bRawDup	= TRUE;

	HxSTD_MemCopy(&(pstDstTable->unInfo), &(pstSrcTable->unInfo), sizeof(SIxTable_Info_t));

	for (pstDesItem = pstSrcTable->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
	{
		HUINT8				*pucDesRaw = (HUINT8 *)HLIB_LIST_Data(pstDesItem);

		if (NULL != pucDesRaw)
		{
			HUINT32			 ulDesLen = (HUINT32)pucDesRaw[1] + 2;
			HUINT8			*pucDupRaw = (HUINT8 *)HLIB_STD_MemAlloc(ulDesLen);

			if (NULL == pucDupRaw)			{ return ERR_FAIL; }
			HxSTD_MemCopy(pucDupRaw, pucDesRaw, ulDesLen);

			pstDstTable->pstDesLoop = HLIB_LIST_Append(pstDstTable->pstDesLoop, (void *)pucDupRaw);
		}
	}

	for (pstItem = pstDstTable->pst2ndLoop; NULL != pstItem; pstItem = pstItem->next)
	{
		SIxTable_2ndLoop_t	*pstSrc2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pstItem);

		if (NULL != pstSrc2ndItem)
		{
			SIxTable_2ndLoop_t	*pstDst2ndItem = (SIxTable_2ndLoop_t *)HLIB_STD_MemCalloc(sizeof(SIxTable_2ndLoop_t));

			if (NULL == pstDst2ndItem)		{ return ERR_FAIL; }
			HxSTD_MemCopy(&(pstDst2ndItem->unInfo), &(pstSrc2ndItem->unInfo), sizeof(SIxTable_SubInfo_t));

			pstDstTable->pst2ndLoop = HLIB_LIST_Append(pstDstTable->pst2ndLoop, (void *)pstDst2ndItem);

			for (pstDesItem = pstSrc2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
			{
				HUINT8				*pucDesRaw = (HUINT8 *)HLIB_LIST_Data(pstDesItem);

				if (NULL != pucDesRaw)
				{
					HUINT32 		 ulDesLen = (HUINT32)pucDesRaw[1] + 2;
					HUINT8			*pucDupRaw = (HUINT8 *)HLIB_STD_MemAlloc(ulDesLen);

					if (NULL == pucDupRaw)			{ return ERR_FAIL; }
					HxSTD_MemCopy(pucDupRaw, pucDesRaw, ulDesLen);

					pstDst2ndItem->pstDesLoop = HLIB_LIST_Append(pstDst2ndItem->pstDesLoop, (void *)pucDupRaw);
				}
			}
		}
	}

	return ERR_OK;
}

STATIC void silib_table_FreeRawSection (SIxTable_PsiTable_t *pstTable)
{
	silib_table_ClearRawSection(pstTable);
	HLIB_STD_MemFree(pstTable);
}

STATIC HERROR silib_table_ParseRawSection (SIxTable_PsiTable_t *pstTable, SIxTextEncoding_e eTextEnc, HUINT8 *pucRawSec)
{
	HERROR						 hErr = ERR_FAIL;

	switch (*pucRawSec)
	{
	default:
		// 아주 특별한 경우를 제외하고는 다 default 처리한다.
		hErr = silib_table_ParseDefaultPsiSection(pstTable, eTextEnc, pucRawSec);
		break;
	}

	return hErr;
}

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/
#define ___MEMBER_FUNCTIONS___
HERROR silib_table_ParseRawSecToTable (SIxTable_PsiTable_t *pstTable, SIxTextEncoding_e eTextEnc, HUINT8 *pucRawSec)
{
	if ((NULL == pstTable) || (NULL == pucRawSec))
	{
		HxLOG_Error("Wrong Arg (pstTable(0x%08x), pucRawSec(0x%08x))\n", pstTable, pucRawSec);
		return ERR_FAIL;
	}

	return silib_table_ParseRawSection(pstTable, eTextEnc, pucRawSec);
}

HERROR silib_table_ClearTable (SIxTable_PsiTable_t *pstTable)
{
	if (NULL == pstTable)
	{
		HxLOG_Error("Wrong Arg\n");
		return ERR_FAIL;
	}

	silib_table_ClearRawSection(pstTable);
	return ERR_OK;
}

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
#define ___API_FUNCTIONS___
HUINT8 SILIB_TABLE_GetTableIdByTableType (SIxTable_Type_e eTable)
{
	switch (eTable)
	{
	case eSIxTABLETYPE_Pat:
		return eSIxTABLEID_PAT;

	case eSIxTABLETYPE_Pmt:
		return eSIxTABLEID_PMT;

	case eSIxTABLETYPE_NitAct:
		return eSIxTABLEID_NIT_ACT;

	case eSIxTABLETYPE_NitOth:
		return eSIxTABLEID_NIT_OTH;

	case eSIxTABLETYPE_Bat:
		return eSIxTABLEID_BAT;

	case eSIxTABLETYPE_SdtAct:
		return eSIxTABLEID_SDT_ACT;

	case eSIxTABLETYPE_SdtOth:
		return eSIxTABLEID_SDT_OTH;

	case eSIxTABLETYPE_EitActPf:
		return eSIxTABLEID_EIT_ACT_PF;

	case eSIxTABLETYPE_EitOthPf:
		return eSIxTABLEID_EIT_OTH_PF;

	case eSIxTABLETYPE_EitActSc:
		return eSIxTABLEID_EIT_ACT_SC_0;

	case eSIxTABLETYPE_EitOthSc:
		return eSIxTABLEID_EIT_OTH_SC_0;

	case eSIxTABLETYPE_Tdt:
		return eSIxTABLEID_TDT;

	case eSIxTABLETYPE_Tot:
		return eSIxTABLEID_TOT;

	case eSIxTABLETYPE_Cat:
		return eSIxTABLEID_CAT;

	case eSIxTABLETYPE_Dsi:
		return eSIxTABLEID_DSI;

	case eSIxTABLETYPE_Unt:
		return eSIxTABLEID_UNT;

	case eSIxTABLETYPE_Rct:
		return eSIxTABLEID_RCT;

	case eSIxTABLETYPE_Ait:
		return eSIxTABLEID_AIT;

	case eSIxTABLETYPE_Ict:
		return eSIxTABLEID_ICT;

	case eSIxTABLETYPE_Sgt:
		return eSIxTABLEID_SGT;

	default:
		break;
	}

	return 0xFF;
}

SIxTable_Type_e SILIB_TABLE_GetTableTypeByTableId (HUINT8 ucTableId)
{
	switch (ucTableId)
	{
	case eSIxTABLEID_PAT:
		return eSIxTABLETYPE_Pat;

	case eSIxTABLEID_CAT:
		return eSIxTABLETYPE_Cat;

	case eSIxTABLEID_PMT:
		return eSIxTABLETYPE_Pmt;

	case eSIxTABLEID_TSDT:
		// 추후 구현한다.
		break;

	case eSIxTABLEID_NIT_ACT:
		return eSIxTABLETYPE_NitAct;

	case eSIxTABLEID_NIT_OTH:
		return eSIxTABLETYPE_NitOth;

	case eSIxTABLEID_SDT_ACT:
		return eSIxTABLETYPE_SdtAct;

	case eSIxTABLEID_SDT_OTH:
		return eSIxTABLETYPE_SdtOth;

	case eSIxTABLEID_EIT_ACT_PF:
		return eSIxTABLETYPE_EitActPf;

	case eSIxTABLEID_EIT_OTH_PF:
		return eSIxTABLETYPE_EitOthPf;

	case eSIxTABLEID_EIT_ACT_SC_0:
	case eSIxTABLEID_EIT_ACT_SC_1:
	case eSIxTABLEID_EIT_ACT_SC_2:
	case eSIxTABLEID_EIT_ACT_SC_3:
	case eSIxTABLEID_EIT_ACT_SC_4:
	case eSIxTABLEID_EIT_ACT_SC_5:
	case eSIxTABLEID_EIT_ACT_SC_6:
	case eSIxTABLEID_EIT_ACT_SC_7:
	case eSIxTABLEID_EIT_ACT_SC_8:
	case eSIxTABLEID_EIT_ACT_SC_9:
	case eSIxTABLEID_EIT_ACT_SC_10:
	case eSIxTABLEID_EIT_ACT_SC_11:
	case eSIxTABLEID_EIT_ACT_SC_12:
	case eSIxTABLEID_EIT_ACT_SC_13:
	case eSIxTABLEID_EIT_ACT_SC_14:
	case eSIxTABLEID_EIT_ACT_SC_15:
		return eSIxTABLETYPE_EitActSc;

	case eSIxTABLEID_EIT_OTH_SC_0:
	case eSIxTABLEID_EIT_OTH_SC_1:
	case eSIxTABLEID_EIT_OTH_SC_2:
	case eSIxTABLEID_EIT_OTH_SC_3:
	case eSIxTABLEID_EIT_OTH_SC_4:
	case eSIxTABLEID_EIT_OTH_SC_5:
	case eSIxTABLEID_EIT_OTH_SC_6:
	case eSIxTABLEID_EIT_OTH_SC_7:
	case eSIxTABLEID_EIT_OTH_SC_8:
	case eSIxTABLEID_EIT_OTH_SC_9:
	case eSIxTABLEID_EIT_OTH_SC_10:
	case eSIxTABLEID_EIT_OTH_SC_11:
	case eSIxTABLEID_EIT_OTH_SC_12:
	case eSIxTABLEID_EIT_OTH_SC_13:
	case eSIxTABLEID_EIT_OTH_SC_14:
	case eSIxTABLEID_EIT_OTH_SC_15:
		return eSIxTABLETYPE_EitOthSc;

	case eSIxTABLEID_BAT:
		return eSIxTABLETYPE_Bat;

	case eSIxTABLEID_RST:
		// 추후 구현한다.
		break;

	case eSIxTABLEID_TDT:
		return eSIxTABLETYPE_Tdt;

	case eSIxTABLEID_TOT:
		return eSIxTABLETYPE_Tot;

	default:
		break;

	}

	return eSIxTABLETYPE_MAX;
}

SIxTable_PsiTable_t *SILIB_TABLE_ParseRawSection (HUINT8 *pucRawSec, SIxTextEncoding_e eTextEnc, HBOOL bRawDup)
{
	SIxTable_PsiTable_t	*pstTable = NULL;
	HERROR				 hErr;

	if (NULL == pucRawSec)					{ return NULL; }

	pstTable = (SIxTable_PsiTable_t *)HLIB_STD_MemCalloc(sizeof(SIxTable_PsiTable_t));
	if (NULL == pstTable)
	{
		HxLOG_Error("Memory allocation failed:\n");
		return NULL;
	}

	pstTable->bRawDup = bRawDup;

	hErr = silib_table_ParseRawSection(pstTable, eTextEnc, pucRawSec);
	if (ERR_OK != hErr)
	{
		silib_table_FreeRawSection(pstTable);
		pstTable = NULL;
	}

	return pstTable;
}

SIxTable_PsiTable_t *SILIB_TABLE_Duplicate (SIxTable_PsiTable_t *pstSrcTable)
{
	SIxTable_PsiTable_t			*pstDstTable = NULL;
	HERROR						 hErr;

	if (NULL == pstSrcTable)					{ return NULL; }

	pstDstTable = (SIxTable_PsiTable_t *)HLIB_STD_MemCalloc(sizeof(SIxTable_PsiTable_t));
	if (NULL == pstDstTable)					{ return NULL; }

	pstDstTable->bRawDup = TRUE;
	hErr = silib_table_DuplicateTable(pstDstTable, pstSrcTable);
	if (ERR_OK != hErr)
	{
		silib_table_FreeRawSection(pstDstTable);
		pstDstTable = NULL;
	}

	return pstDstTable;
}

void SILIB_TABLE_Free (SIxTable_PsiTable_t *pstTable)
{
	SIxTable_PsiTable_t		*pstNextTable;

	while (NULL != pstTable)
	{
		pstNextTable = pstTable->pstNextTable;

		switch (pstTable->eTableType)
		{
		default:
			// 아주 특별한 경우를 제외하고는 다 default 처리한다.
			silib_table_FreeRawSection(pstTable);
			break;
		}

		pstTable = pstNextTable;
	}

	return;
}

HUINT8 *SILIB_TABLE_Get1stLoopDescriptor (SIxTable_PsiTable_t *pstTable, HUINT32 ulDesTag, HUINT32 ulIndex)
{
	HUINT32			 ulCount = 0;

	if (NULL != pstTable)
	{
		HxList_t	*pstItem;

		for (pstItem = pstTable->pstDesLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data(pstItem);

			if ((SILIB_TABLE_DESTAG_ALL != ulDesTag) && ((HUINT8)ulDesTag != pucRawDes[0]))
			{
				continue;
			}

			if (ulCount == ulIndex)
			{
				return pucRawDes;
			}

			ulCount++;
		}
	}

	return NULL;
}

HUINT8 *SILIB_TABLE_Get2ndLoopDescriptor (SIxTable_2ndLoop_t *pst2ndLoop, HUINT32 ulDesTag, HUINT32 ulIndex)
{
	HUINT32			 ulCount = 0;

	if (NULL != pst2ndLoop)
	{
		HxList_t	*pstItem;

		for (pstItem = pst2ndLoop->pstDesLoop; NULL != pstItem; pstItem = pstItem->next)
		{
			HUINT8			*pucRawDes = (HUINT8 *)HLIB_LIST_Data(pstItem);

			if ((SILIB_TABLE_DESTAG_ALL != ulDesTag) && ((HUINT8)ulDesTag != pucRawDes[0]))
			{
				continue;
			}

			if (ulCount == ulIndex)
			{
				return pucRawDes;
			}

			ulCount++;
		}
	}

	return NULL;
}



