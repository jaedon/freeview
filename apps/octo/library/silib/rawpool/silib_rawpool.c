/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  scrawpool.c
	@brief

	Description:  									\n
	Module: SILIB/RAWPOOL					\n

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
#include <silib.h>

#include "_silib.h"
//#include "_silib_descparse.h"

/*******************************************************************/
/********************      Local Macro Definition     ********************/
/*******************************************************************/
#define RAW_PER_BLOCK							32
#define RAW_DEFRAG_THRESHOLD					(RAW_PER_BLOCK * 3)			// (3 - 1) block 이상의 Deflag가 있으면 정리한다.

/*******************************************************************/
/********************      Local Enumerations         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Data Type         *************************/
/*******************************************************************/
typedef enum
{
	eRAW_EXTID_USABLE,
	eRAW_EXTID_NOTUSABLE,
	eRAW_EXTID_PRIVATE
} rawExtIdType_e;

typedef struct
{
	HBOOL				 bAvailable;

	HBOOL				 bOptionAvail;
	SIxRawOption_t		 stOption;

	HBOOL				 bIncompleteSec;	// filter mode가 continue일 때 모든 section을 받기 전에는 TRUE임, default FALSE

	HUINT32				 ulRawSize;
	HUINT8				*pucRaw;
} rawItem_t;

typedef struct _raw_block_
{
	rawItem_t				 astRawBlock[RAW_PER_BLOCK];
	struct _raw_block_	*pstPrev, *pstNext;
} rawBlock_t;

typedef struct
{
//	SIxTable_Type_e		 eSiTableType;
	HINT32				 nRawNum;			// 실제 들어간 Raw 개수

	rawBlock_t		*pstBlockList;
	HINT32				 nBlockNum;			// List 내의 Block 개수 : Block 내의 Raw들은 유효하지 않은 것들이 있을 수 있다.

	HINT32				 nLastAddPos;
	rawBlock_t		*pstLastBlock;		// Pointer of the Last Raw block in the list "pstRawList"
} rawTable_t;

typedef struct
{
	SIxRawPoolProperty_t	 stProperty;

	rawTable_t		 astSiTable[eSIxTABLETYPE_MAX];
} rawPool_t;

typedef struct
{
	rawBlock_t		*pstCurrBlock;
	HINT32				 nCurrIndex;			// RawItem
} rawBookmark_t;

typedef struct
{
	HUINT32				 ulExtId;
	HUINT32				 ulLastVerNum;
	HUINT32				 ulSecNum;
} rawSecCount_t;

typedef struct
{
	HxList_t			*pstSecCntList;

} rawSecInfo_t;

/*******************************************************************/
/********************      Local Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
#define _____RAW_FUNCTIONS_____
STATIC HUINT8 *silib_raw_DuplicateRaw (HUINT8 *pucRaw, HUINT16 usRawSize)
{
	HUINT8		*pucDupRaw = (HUINT8 *)HLIB_STD_MemAlloc (usRawSize);

	if (NULL != pucDupRaw)
	{
		HxSTD_MemCopy (pucDupRaw, pucRaw, usRawSize);
	}

	return pucDupRaw;
}

#define _____RAWPOOL_DATATYPE_FUNCTIONS_____
STATIC HERROR silib_raw_ClearItem (rawItem_t *pstItem)
{
	if (NULL == pstItem)
	{
		return ERR_FAIL;
	}

	if (NULL != pstItem->pucRaw)
	{
		HLIB_STD_MemFree (pstItem->pucRaw);
		pstItem->pucRaw = NULL;
	}

	HxSTD_MemSet (pstItem, 0, sizeof(rawItem_t));
	return ERR_OK;
}

STATIC HERROR silib_raw_ClearTable (rawTable_t *pstTable)
{
	HUINT32				 ulBlockCnt;
	rawBlock_t		*pstRawList, *pstNextList;

	pstRawList = pstTable->pstBlockList;
	while (pstRawList)
	{
		pstNextList = pstRawList->pstNext;
		for (ulBlockCnt = 0; ulBlockCnt < RAW_PER_BLOCK; ulBlockCnt++)
		{
			rawItem_t			*pstRawItem = &(pstRawList->astRawBlock[ulBlockCnt]);

			if ((NULL != pstRawItem)
				&& (NULL != pstRawItem->pucRaw))
			{
				HLIB_STD_MemFree (pstRawItem->pucRaw);
				pstRawItem->pucRaw = NULL;
			}
		}

		HLIB_STD_MemFree (pstRawList);
		pstRawList = pstNextList;
	}

	HxSTD_MemSet (pstTable, 0, sizeof(rawTable_t));
	pstTable->nLastAddPos = -1;

	return ERR_OK;
}

STATIC HERROR silib_raw_ClearPool (rawPool_t *pstPool)
{
	HUINT32			 ulCnt;
	HERROR			 hErr, hResult = ERR_OK;

	if (pstPool == NULL)
	{
		HxLOG_Error ("null pointer \n");
		return ERR_FAIL;
	}

	for (ulCnt = 0; ulCnt < eSIxTABLETYPE_MAX; ulCnt++)
	{
		hErr = silib_raw_ClearTable (&(pstPool->astSiTable[ulCnt]));
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("silib_raw_ClearTable err in TableType(%d)\n", ulCnt);
			hResult = ERR_FAIL;
		}
	}

	return hResult;
}

STATIC HERROR silib_raw_AppendNewBlockTable (rawTable_t *pstTable)
{
	rawBlock_t		*pstBlock = (rawBlock_t *)HLIB_STD_MemCalloc (sizeof(rawBlock_t));

	if (NULL == pstBlock)
	{
		return ERR_FAIL;
	}

	if (NULL == pstTable->pstLastBlock)
	{
		pstTable->pstBlockList = pstBlock;
	}
	else if (pstTable->nLastAddPos == (RAW_PER_BLOCK - 1))
	{
		rawBlock_t		*pstPrevBlock = pstTable->pstBlockList;

		while (pstPrevBlock->pstNext)
		{
			pstPrevBlock = pstPrevBlock->pstNext;
		}

		pstPrevBlock->pstNext = pstBlock;
		pstBlock->pstPrev = pstPrevBlock;
	}
	else
	{
		HxLOG_Error ("It shall not append the new block!!!\n");
		HLIB_STD_MemFree (pstBlock);

		return ERR_FAIL;
	}

	pstTable->pstLastBlock = pstBlock;
	pstTable->nLastAddPos = -1;
	pstTable->nBlockNum ++;

	return ERR_OK;
}

#define _____RAWITEM_COMPARE_FUNCTIONS_____
STATIC HBOOL silib_raw_CompareRawItemExtId(SIxTable_Type_e eSiType, rawItem_t *pstRawItem, HUINT32 ulExtId)
{
	HUINT8				*pucRaw = pstRawItem->pucRaw;
	HUINT32				 ulRawExtId;
	rawExtIdType_e	 eExtIdType = eRAW_EXTID_USABLE;

	if (ulExtId > 0xFFFF)
	{
		eExtIdType = eRAW_EXTID_NOTUSABLE;
	}
	else if (eRAW_EXTID_PRIVATE == eExtIdType)
	{
		HUINT8		 ucSecSyntaxIndicator = (pucRaw[1] & 0x80) >> 1;

		if (1 == ucSecSyntaxIndicator)
		{
			eExtIdType = eRAW_EXTID_USABLE;
		}
		else
		{
			return FALSE;
		}
	}

	switch (eExtIdType)
	{
	case eRAW_EXTID_USABLE:
		ulRawExtId = HxMACRO_Get16Bit (pucRaw + 3);
		return (ulExtId == ulRawExtId);

	case eRAW_EXTID_NOTUSABLE:
		return TRUE;

	default:
		break;
	}

	return FALSE;
}

STATIC HINT32 silib_raw_CompVersion (HUINT32 ulVer1, HUINT32 ulVer2)
{
	HINT32		 nDiff = (HINT32)ulVer1 - (HINT32)ulVer2;

	// Version이 5비트이기 때문에 31 -> 1 로 가는 경우엔 1이 더 크다고 판단하도록 해야 한다.

	if (nDiff > 28)
	{
		return (-1 * (32 - nDiff));
	}
	else if (nDiff < -28)
	{
		return (32 + nDiff);
	}

	return nDiff;
}

STATIC HUINT32 silib_raw_CreateSectionVersionHandle (void)
{
	rawSecInfo_t			*pstSecInfo;

	pstSecInfo = (rawSecInfo_t *)HLIB_STD_MemCalloc(sizeof(rawSecInfo_t));
	return (HUINT32)pstSecInfo;
}

STATIC void silib_raw_DestroySectionVersionHandle (HUINT32 ulSecVerHandle)
{
	rawSecInfo_t			*pstSecInfo = (rawSecInfo_t *)ulSecVerHandle;

	if (NULL != pstSecInfo)
	{
		pstSecInfo->pstSecCntList = HLIB_LIST_RemoveAllFunc(pstSecInfo->pstSecCntList, HLIB_STD_MemFree_CB);
		HLIB_STD_MemFree(pstSecInfo);
	}
}

STATIC HERROR silib_raw_RegisterSectionVersion (HUINT32 ulSecVerHandle, HUINT32 ulSecExtId, HUINT32 ulCurSecVer, HUINT32 ulLastSecNum)
{
	HxList_t				*pstItem;
	rawSecInfo_t			*pstSecInfo = (rawSecInfo_t *)ulSecVerHandle;
	rawSecCount_t			*pstSecCnt;

	if (NULL == pstSecInfo)				{ return ERR_FAIL; }

	for (pstItem = pstSecInfo->pstSecCntList; NULL != pstItem; pstItem = pstItem->next)
	{
		pstSecCnt = (rawSecCount_t *)HLIB_LIST_Data(pstItem);
		if ((NULL != pstSecCnt) && (ulSecExtId == pstSecCnt->ulExtId))
		{
			if (silib_raw_CompVersion(ulCurSecVer, pstSecCnt->ulLastVerNum) > 0)
			{
				pstSecCnt->ulLastVerNum = ulCurSecVer;
				pstSecCnt->ulSecNum = ulLastSecNum;
			}

			return ERR_OK;
		}
	}

	pstSecCnt = (rawSecCount_t *)HLIB_STD_MemCalloc(sizeof(rawSecCount_t));
	if (NULL == pstSecCnt)
	{
		return ERR_FAIL;
	}

	pstSecCnt->ulExtId = ulSecExtId;
	pstSecCnt->ulLastVerNum = ulCurSecVer;
	pstSecCnt->ulSecNum = ulLastSecNum;

	pstSecInfo->pstSecCntList = HLIB_LIST_Append(pstSecInfo->pstSecCntList, (void *)pstSecCnt);
	return ERR_OK;
}

STATIC HERROR silib_raw_CompareSectionVersion (HUINT32 ulSecVerHandle, HUINT32 ulSecExtId, HUINT32 ulCurSecVer, HINT32 *pnCompVal)
{
	HxList_t				*pstItem;
	rawSecInfo_t			*pstSecInfo = (rawSecInfo_t *)ulSecVerHandle;
	rawSecCount_t			*pstSecCnt;

	if (NULL == pstSecInfo) 			{ return ERR_FAIL; }

	for (pstItem = pstSecInfo->pstSecCntList; NULL != pstItem; pstItem = pstItem->next)
	{
		pstSecCnt = (rawSecCount_t *)HLIB_LIST_Data(pstItem);
		if ((NULL != pstSecCnt) && (ulSecExtId == pstSecCnt->ulExtId))
		{
			if (NULL != pnCompVal)
			{
				*pnCompVal = silib_raw_CompVersion(ulCurSecVer, pstSecCnt->ulLastVerNum);
			}

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HUINT32 silib_raw_CountTotalSectionNumInVersionHandle (HUINT32 ulSecVerHandle, HUINT32 ulSecExtId)
{
	HUINT32					 ulSecNum = 0;
	HxList_t				*pstItem;
	rawSecInfo_t			*pstSecInfo = (rawSecInfo_t *)ulSecVerHandle;
	rawSecCount_t			*pstSecCnt;

	if (NULL == pstSecInfo) 			{ return ulSecNum; }

	for (pstItem = pstSecInfo->pstSecCntList; NULL != pstItem; pstItem = pstItem->next)
	{
		pstSecCnt = (rawSecCount_t *)HLIB_LIST_Data(pstItem);
		if (NULL != pstSecCnt)
		{
			if ((ulSecExtId >= 0xFFFF) || (ulSecExtId == pstSecCnt->ulExtId))
			{
				ulSecNum += pstSecCnt->ulSecNum;
			}
		}
	}

	return ulSecNum;
}

STATIC SIxTable_PsiTable_t *silib_raw_GetLastPsiTable (SIxTable_PsiTable_t *pstTable)
{
	while (NULL != pstTable)
	{
		if (NULL == pstTable->pstNextTable)
		{
			return pstTable;
		}

		pstTable = pstTable->pstNextTable;
	}

	return NULL;
}

STATIC SIxTable_PsiTable_t *silib_raw_FindPsiTableWithSameTypeAndExtId (SIxTable_PsiTable_t *pstTable, SIxTable_Type_e eTableType, HUINT16 usExtId)
{
	while (NULL != pstTable)
	{
		if ((pstTable->eTableType == eTableType) &&
			(pstTable->unInfo.stTemplate.usExtId == usExtId))
		{
			return pstTable;
		}

		pstTable = pstTable->pstNextTable;
	}

	return NULL;
}

#define _____RAWPOOL_BOOKMARK_FUNCTIONS_____
STATIC HERROR silib_raw_InitBookmark (rawTable_t *pstTable, rawBookmark_t *pstBookmark)
{
	/* [참고] pstTable->pstBlockList 는 늘 맨 앞이로, list의 맨 앞으로 돌려줄 필요는 없다 */

	pstBookmark->pstCurrBlock = pstTable->pstBlockList;
	pstBookmark->nCurrIndex = -1;

	return ERR_OK;
}

STATIC rawItem_t *silib_raw_GetNextRawItem (rawBookmark_t *pstBookmark)
{
	rawItem_t			*pstRawItem;

	while (NULL != pstBookmark->pstCurrBlock)
	{
		pstBookmark->nCurrIndex ++;
		if (RAW_PER_BLOCK == pstBookmark->nCurrIndex)
		{
			pstBookmark->pstCurrBlock = pstBookmark->pstCurrBlock->pstNext;
			pstBookmark->nCurrIndex = 0;

			if (NULL == pstBookmark->pstCurrBlock)
			{
				return NULL;
			}
		}

		pstRawItem = &(pstBookmark->pstCurrBlock->astRawBlock[pstBookmark->nCurrIndex]);
		if (TRUE == pstRawItem->bAvailable)
		{
			return pstRawItem;
		}
	}

	return NULL;
}



#define _____RAWPOOL_OPERATION_FUNCTIONS_____
STATIC HERROR silib_raw_AddRawToTable (rawTable_t *pstTable, HUINT8 *pucRaw, HUINT16 usRawSize, SIxRawOption_t *pstOption, HBOOL bIncompleteSec)
{
	HINT32				 nNextAddPos;
	rawItem_t			*pstRawItem;
	HERROR				 hErr;

	if ((NULL == pstTable->pstLastBlock) ||
		(pstTable->nLastAddPos == (RAW_PER_BLOCK - 1)))
	{
		hErr = silib_raw_AppendNewBlockTable (pstTable);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("silib_raw_AppendNewBlockTable err:\n");
			return ERR_FAIL;
		}
	}

	nNextAddPos = pstTable->nLastAddPos + 1;
	if ((nNextAddPos < 0) || (nNextAddPos >= RAW_PER_BLOCK))
	{
		HxLOG_Error ("Next Add Position (%d)... something wrong;\n", nNextAddPos);
		return ERR_FAIL;
	}

	pstRawItem = &(pstTable->pstLastBlock->astRawBlock[nNextAddPos]);

	pstRawItem->bAvailable	= TRUE;
	pstRawItem->pucRaw		= pucRaw;
	pstRawItem->ulRawSize	= (HUINT32)usRawSize;

	if (NULL != pstOption)
	{
		pstRawItem->bOptionAvail	= TRUE;
		HxSTD_MemCopy (&(pstRawItem->stOption), pstOption, sizeof(SIxRawOption_t));
	}
	else
	{
		pstRawItem->bOptionAvail	= FALSE;
	}

	pstRawItem->bIncompleteSec = FALSE;
	if (TRUE == bIncompleteSec)
	{
		pstRawItem->bIncompleteSec = TRUE;
	}

	pstTable->nLastAddPos = nNextAddPos;
	pstTable->nRawNum ++;

	HxLOG_Print("pstTable->nLastAddPos = %d, nRawNum = %d, nBlockNum = %d, pstBlockList(0x%x), pstLastBlock(0x%x)\n", pstTable->nLastAddPos, pstTable->nRawNum, pstTable->nBlockNum, pstTable->pstBlockList, pstTable->pstLastBlock);

	return ERR_OK;
}

STATIC HERROR silib_raw_FreeRawFromTable (rawTable_t *pstTable, rawItem_t *pstRawItem)
{
	if (TRUE == pstRawItem->bAvailable)
	{
		pstTable->nRawNum --;
	}

	silib_raw_ClearItem (pstRawItem);
	return ERR_OK;
}

STATIC HBOOL silib_raw_CheckTheNeedOfDefrag (rawTable_t *pstTable)
{
	HINT32				 nTotalRawSlot = pstTable->nBlockNum * RAW_PER_BLOCK;

	if ((nTotalRawSlot - pstTable->nRawNum) > RAW_DEFRAG_THRESHOLD)
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("nTotalRawSlot = %d, pstTable->nRawNum = %d\n"), nTotalRawSlot, pstTable->nRawNum);
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR silib_raw_DefragTable (rawTable_t *pstTable)
{
	HUINT32				 ulSrcIdx = 0, ulDstIdx = 0;
	HUINT32				 ulRawNum = 0, ulBlockNum = 1;
	rawBlock_t		*pstSrcBlock, *pstDstBlock;
	rawItem_t			*pstSrcItem, *pstDstItem;

	if (NULL == pstTable->pstBlockList)
	{
		HxLOG_Error ("No Raw Data at all\n");
		return ERR_OK;
	}

	pstSrcBlock = pstDstBlock = pstTable->pstBlockList;

	pstTable->pstLastBlock	= NULL;
	pstTable->nLastAddPos	= -1;

	while ((NULL != pstSrcBlock) && (NULL != pstDstBlock))
	{
		pstSrcItem = &(pstSrcBlock->astRawBlock[ulSrcIdx]);
		pstDstItem = &(pstDstBlock->astRawBlock[ulDstIdx]);

		if (TRUE == pstSrcItem->bAvailable)
		{
			if (pstDstItem != pstSrcItem)
			{
				HxSTD_MemCopy (pstDstItem, pstSrcItem, sizeof(rawItem_t));
			}

			pstTable->pstLastBlock	= pstDstBlock;
			pstTable->nLastAddPos	= (HINT32)ulDstIdx;

			// Next DstItem
			ulDstIdx ++;
			ulRawNum ++;
			if (ulDstIdx == RAW_PER_BLOCK)
			{
				pstDstBlock = pstDstBlock->pstNext;
				ulDstIdx = 0;
				ulBlockNum ++;
			}
		}

		// Next SrcItem
		ulSrcIdx ++;
		if (ulSrcIdx == RAW_PER_BLOCK)
		{
			pstSrcBlock = pstSrcBlock->pstNext;
			ulSrcIdx = 0;
		}
	}

	if (NULL != pstDstBlock)
	{
		// pstDstBlock 뒤에 담은 구조체들은 이미 copy해서 내용을 옮긴 것이기 때문에
		// 정식 clear를 시키면 오히려 raw data들이 free 되는 위험이 크다.
		// 여기는 Block만 Free 시키는 것이어야 한다.
		for (; ulDstIdx < RAW_PER_BLOCK; ulDstIdx++)
		{
			HxSTD_MemSet (&(pstDstBlock->astRawBlock[ulDstIdx]), 0, sizeof(rawItem_t));
		}

		if (NULL != pstDstBlock->pstNext)
		{
			rawBlock_t		*pstBlock = pstDstBlock->pstNext;
			rawBlock_t		*pstNextBlock;

			while (NULL != pstBlock)
			{
				pstNextBlock = pstBlock->pstNext;
				HLIB_STD_MemFree (pstBlock);
				pstBlock = pstNextBlock;
			}

			pstDstBlock->pstNext = NULL;
		}
	}

	pstTable->nRawNum = (HINT32)ulRawNum;
	pstTable->nBlockNum = (HINT32)ulBlockNum;

	HxLOG_Print("pstTable->pstBlockList(0x%x), ulRawNum = %d, ulBlockNum = %d\n", pstTable->pstBlockList, ulRawNum, ulBlockNum);

	return ERR_OK;
}



/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
#define ___API_FUNCTIONS___
HUINT8 *SILIB_RAW_DuplicateRaw (HUINT8 *pucRaw)
{
	HUINT16					 usRawSize;

	if (NULL == pucRaw)
	{
		return NULL;
	}

	usRawSize = HxMACRO_Get12Bit (pucRaw + 1) + 3;
	if (usRawSize <= 7)
	{
		HxLOG_Error ("Raw Size is wrong: pucRaw is not PSI Section format\n");
		return NULL;
	}

	return silib_raw_DuplicateRaw (pucRaw, usRawSize);
}

SIxRawPoolHandle_t SILIB_RAW_CreatePool (SIxRawPoolProperty_t *pstProperty)
{
	rawPool_t		*pstPool;

	if (NULL != pstProperty)
	{
		pstPool = (rawPool_t *)HLIB_STD_MemCalloc (sizeof(rawPool_t));
		if (NULL != pstPool)
		{
			HxSTD_MemCopy (&(pstPool->stProperty), pstProperty, sizeof(SIxRawPoolProperty_t));

			return (SIxRawPoolHandle_t)pstPool;
		}
	}

	return (SIxRawPoolHandle_t)NULL;
}

HERROR SILIB_RAW_DestroyPool (SIxRawPoolHandle_t hPool)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	HERROR			 hErr;

	if (NULL == pstPool)					{ return ERR_FAIL; }

	hErr = silib_raw_ClearPool (pstPool);
	if (ERR_OK == hErr)
	{
		HLIB_STD_MemFree (pstPool);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR SILIB_RAW_ClearPool (SIxRawPoolHandle_t hPool)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;

	if (NULL == pstPool)					{ return ERR_FAIL; }

	return silib_raw_ClearPool (pstPool);
}

HERROR SILIB_RAW_GetProperty (SIxRawPoolHandle_t hPool, SIxRawPoolProperty_t *pstProperty)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;

	if (NULL == pstPool)					{ return ERR_FAIL; }

	if (NULL != pstProperty)
	{
		HxSTD_MemCopy (pstProperty, &(pstPool->stProperty), sizeof(SIxRawPoolProperty_t));
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR SILIB_RAW_ChangeProperty (SIxRawPoolHandle_t hPool, SIxRawPoolProperty_t *pstProperty)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;

	if (NULL == pstPool)					{ return ERR_FAIL; }

	if (NULL != pstProperty)
	{
		HxSTD_MemCopy (&(pstPool->stProperty), pstProperty, sizeof(SIxRawPoolProperty_t));
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR SILIB_RAW_GetIncompleteSectionStatus (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId, HBOOL *pbIncomplete)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	rawItem_t		*pstRawItem;
	rawBookmark_t	 stBookmark;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX) || (NULL == pbIncomplete))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d) \n", hPool, eSiType);
		return ERR_FAIL;
	}

	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			// section 중 하나라도 incomplete라고 표시된 게 있다면 incomplete다
			if (pstRawItem->bIncompleteSec == TRUE)
			{
				*pbIncomplete = TRUE;
				return ERR_OK;
			}
		}
	}

	*pbIncomplete = FALSE;
	return ERR_OK;
}

HERROR SILIB_RAW_FinalizeIncompleteSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	rawItem_t		*pstRawItem;
	rawBookmark_t	 stBookmark;
	HINT32			 nCompVal = 0;
	HUINT32			 ulSecExtId;
	HUINT32			 ulSecCount = 0;
	HUINT32			 ulCurSecVer, ulLastSecNum;
	HUINT32			 ulSecVerHandle = 0;
	HERROR			 hErr, hResult = ERR_FAIL;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d) \n", hPool, eSiType);
		goto END_FUNC;
	}

	ulSecVerHandle = silib_raw_CreateSectionVersionHandle();
	if (0 == ulSecVerHandle)
	{
		HxLOG_Error("silib_raw_CreateSectionVersionHandle err\n");
		goto END_FUNC;
	}

	// old version section들을 지우고 위해 최신 section version을 찾는다
	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			ulSecExtId = (HUINT32)HxMACRO_Get16Bit(pstRawItem->pucRaw + 3);
			ulCurSecVer = (HUINT32)(HxMACRO_Get8Bit(pstRawItem->pucRaw + 5) & 0x3E) >> 1;
			ulLastSecNum = (HUINT32)HxMACRO_Get8Bit(pstRawItem->pucRaw + 7) + 1;

			(void)silib_raw_RegisterSectionVersion(ulSecVerHandle, ulSecExtId, ulCurSecVer, ulLastSecNum);
		}
	}

	// old version section은 지우고, 현재 version section은 몇 개인지 세어보자
	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			ulSecExtId = (HUINT32)HxMACRO_Get16Bit(pstRawItem->pucRaw + 3);
			ulCurSecVer = (HUINT32)(HxMACRO_Get8Bit(pstRawItem->pucRaw + 5) & 0x3E) >> 1;

			hErr = silib_raw_CompareSectionVersion(ulSecVerHandle, ulSecExtId, ulCurSecVer, &nCompVal);
			if (ERR_OK == hErr)
			{
				if (nCompVal < 0)
				{
					// old version section이다. 지워버리자
					HxLOG_Error("Remove the section curSecVer(%d)\n", ulExtId, ulCurSecVer);
					silib_raw_FreeRawFromTable (&(pstPool->astSiTable[eSiType]), pstRawItem);
				}
				else if (nCompVal == 0)
				{
					// same version section이다.
					ulSecCount ++;
				}
				else
				{
					// last section version보다 current section version이 크네..? 이러면 안 되는데..
						HxLOG_Error("ERROR ! invalid current section version !! - extId(0x%X) curSecVer(%d)\n",
										ulExtId, ulCurSecVer);
				}
			}
			else
			{
				HxLOG_Error("ERROR ! silib_raw_CompareSectionVersion !! - extId(0x%X) curSecVer(%d)\n",
								ulExtId, ulCurSecVer);
			}
		}
	}

	ulLastSecNum = silib_raw_CountTotalSectionNumInVersionHandle(ulSecVerHandle, ulExtId);

	if (ulSecCount < ulLastSecNum)
	{
		// 아직 못 받은 section이 있다. 종료
		goto END_FUNC;
	}
	else if (ulSecCount > ulLastSecNum)
	{
		// last sec number보다 count sec num가 크다면 count가 잘못된 것이다.. error !
		HxLOG_Error("ERROR ! invalid current section counter !! - extId(0x%X),  ulCntSec(%d) > ulLastSecNum(%d) \n",
								ulExtId, ulSecCount, ulLastSecNum);
		goto END_FUNC;
	}

	// section을 전부 다 받았다. 완료표시 해주자
	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{

			HxLOG_Print("Set complete section - bIncompleteSec(%d) extId(0x%4X) section(%2d/%2d) \n", pstRawItem->bIncompleteSec,
																									HxMACRO_Get16Bit(pstRawItem->pucRaw + 3),
																									HxMACRO_Get8Bit(pstRawItem->pucRaw + 6),
																									HxMACRO_Get8Bit(pstRawItem->pucRaw + 7));

			pstRawItem->bIncompleteSec = FALSE;
		}
	}

	hResult = ERR_OK;

END_FUNC:
	silib_raw_DestroySectionVersionHandle(ulSecVerHandle);
	return hResult;
}


HERROR SILIB_RAW_AddRawSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT8 *pucRaw, SIxRawOption_t *pstOption, HBOOL bIncompleteSec)
{
	HUINT16			 usRawSize;
	HUINT8			*pucDupRaw = NULL;
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	HERROR			 hErr;

	if ((NULL == pstPool) || (NULL == pucRaw) || (eSiType >= eSIxTABLETYPE_MAX))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d), pucRaw(0x%08x)\n", hPool, eSiType, pucRaw);
		goto ERROR;
	}

	usRawSize = HxMACRO_Get12Bit (pucRaw + 1) + 3;
	if (usRawSize <= 7)
	{
		HxLOG_Error ("Raw Size is wrong: pucRaw is not PSI Section format\n");
		goto ERROR;
	}

	pucDupRaw = silib_raw_DuplicateRaw (pucRaw, usRawSize);
	if (NULL == pucDupRaw)
	{
		HxLOG_Error ("silib_raw_DuplicateRaw err:\n");
		goto ERROR;
	}

	hErr = silib_raw_AddRawToTable (&(pstPool->astSiTable[eSiType]), pucDupRaw, usRawSize, pstOption, bIncompleteSec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("silib_raw_DuplicateRaw err:\n");
		goto ERROR;
	}

	if (silib_raw_CheckTheNeedOfDefrag (&(pstPool->astSiTable[eSiType])) == TRUE)
	{
		silib_raw_DefragTable (&(pstPool->astSiTable[eSiType]));
	}

	return ERR_OK;
ERROR:
	if (NULL != pucDupRaw)					{ HLIB_STD_MemFree (pucDupRaw); }
	return ERR_FAIL;
}

HERROR SILIB_RAW_FindSameRawSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId, HUINT8 *pucRaw)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	rawItem_t		*pstRawItem;
	rawBookmark_t	 stBookmark;
	HUINT16			 usDstRawSize;
	HUINT32			 ulSrcCrc, ulDstCrc;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d)\n", hPool, eSiType);
		return ERR_FAIL;
	}

	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			HxLOG_Print("pstRawItem is NULL\n");
			break;
		}

		HxLOG_Print("[%d - 0x%x] Compare => %d (ulRawExtId = 0x%x)\n"
									, stBookmark.nCurrIndex
									, stBookmark.pstCurrBlock
									, silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId), HxMACRO_Get16Bit(pstRawItem->pucRaw + 3));

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			ulSrcCrc = HxMACRO_Get32Bit (pstRawItem->pucRaw + pstRawItem->ulRawSize - 4);

			usDstRawSize = HxMACRO_Get12Bit (pucRaw + 1) + 3;
			ulDstCrc = HxMACRO_Get32Bit (pucRaw + usDstRawSize - 4);

			HxLOG_Print(HxANSI_COLOR_CYAN("\t - ulSrcCrc = 0x%x, ulDstCrc = 0x%x \n"), ulSrcCrc, ulDstCrc);
			if (ulSrcCrc == ulDstCrc)	// crc로 동일한 section인지 확인한다
			{
				// 동일한 raw section이 raw pool에 존재한다
				return ERR_OK;
			}
		}
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("[%d] no same item, ulExtId = 0x%x\n"), stBookmark.nCurrIndex, ulExtId);

	return ERR_FAIL;
}

HERROR SILIB_RAW_FreeRawSectionByExtId (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	rawItem_t 	*pstRawItem;
	rawBookmark_t	 stBookmark;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d)\n", hPool, eSiType);
		return ERR_FAIL;
	}

	if (TRUE != bUseExtId)
	{
		ulExtId = SILIB_TABLE_EXT_ID_ALL;
	}

	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			silib_raw_FreeRawFromTable (&(pstPool->astSiTable[eSiType]), pstRawItem);
		}
	}

	return ERR_OK;
}

HERROR SILIB_RAW_FreeRawSectionByUniqueId (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulUniqueId)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	rawItem_t 	*pstRawItem;
	rawBookmark_t	 stBookmark;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d)\n", hPool, eSiType);
		return ERR_FAIL;
	}

	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);

	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			/* free 시점에 nLastAddPos 값 등이 초기 값으로 돌아와야 다음 번 add 시 문제 안 생김 */
			if(0 == pstPool->astSiTable[eSiType].nRawNum)
			{
				silib_raw_DefragTable(&pstPool->astSiTable[eSiType]);
			}
			break;
		}

		if ((TRUE == pstRawItem->bOptionAvail) &&
			(pstRawItem->stOption.ulUniqueId == ulUniqueId))
		{
			silib_raw_FreeRawFromTable (&(pstPool->astSiTable[eSiType]), pstRawItem);
		}

		HxLOG_Print("[eSiType(%d)], nRawNum = %d\n", eSiType, pstPool->astSiTable[eSiType].nRawNum);
	}

	return ERR_OK;
}

HERROR SILIB_RAW_FreeAllRawSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType)
{
	rawPool_t		*pstPool = (rawPool_t *)hPool;
	HERROR			 hErr, hResult = ERR_OK;

	if (NULL == pstPool)
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x)\n", hPool);
		return ERR_FAIL;
	}

	if (eSiType < eSIxTABLETYPE_MAX)
	{
		hResult = silib_raw_ClearTable (&(pstPool->astSiTable[eSiType]));
	}
	else if (eSIxTABLETYPE_ALL == eSiType)
	{
		HUINT32				 ulCnt;

		for (ulCnt = 0; ulCnt < eSIxTABLETYPE_MAX; ulCnt++)
		{
			hErr = silib_raw_ClearTable (&(pstPool->astSiTable[ulCnt]));
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("silib_raw_ClearTable failed in SiType(%d)\n", ulCnt);
				hResult = ERR_FAIL;
			}
		}
	}
	else
	{
		HxLOG_Error ("Wrong Arg: eSiType(%d)\n", eSiType);
		return ERR_FAIL;
	}

	return hResult;
}

SIxTable_PsiTable_t *SILIB_RAW_Parse (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId)
{
	HBOOL				 bFound = FALSE;
	rawPool_t			*pstPool = (rawPool_t *)hPool;
	SIxTable_PsiTable_t	*pstTable = NULL, *pstNextTable = NULL;
	rawItem_t			*pstRawItem;
	rawBookmark_t		 stBookmark;
	HERROR				 hErr;
	HBOOL				 bIncompleteSec = FALSE;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d)\n", hPool, eSiType);
		goto ERROR;
	}

	pstTable = (SIxTable_PsiTable_t *)HLIB_STD_MemCalloc (sizeof(SIxTable_PsiTable_t));
	if (NULL == pstTable)
	{
		HxLOG_Error ("memory allocation failed\n");
		goto ERROR;
	}

	pstTable->bRawDup = TRUE;

	if (ulExtId > 0xFFFF)				// 16진수 이상을 올리면 EXT ID ALL을 의미하게 한다.
	{
		ulExtId = SILIB_TABLE_EXT_ID_ALL;
	}

	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			hErr = SILIB_RAW_GetIncompleteSectionStatus(hPool, eSiType, ulExtId, &bIncompleteSec);
			if ((ERR_OK == hErr) && (TRUE == bIncompleteSec))
			{
				// 아직 모든 section을 받지 못했다..
				HxLOG_Error ("can't parse incomplete section : eSiType(%d) ulExtId(0x%X)\n", eSiType, ulExtId);
				continue;
			}

			if (NULL == pstNextTable)
			{
				pstNextTable = (SIxTable_PsiTable_t *)HLIB_STD_MemCalloc (sizeof(SIxTable_PsiTable_t));
				if (NULL == pstNextTable)
				{
					HxLOG_Error ("memory allocation failed\n");
					goto ERROR;
				}

				pstNextTable->bRawDup = TRUE;
			}

			if (TRUE != bFound)
			{
				hErr = silib_table_ParseRawSecToTable (pstTable, pstPool->stProperty.eTextEnc, pstRawItem->pucRaw);
				if (ERR_OK == hErr)
				{
					bFound = TRUE;
				}
				else
				{
					HxLOG_Error ("silib_table_ParseRawSecToTable err: Raw[%02x][%02x][%02x][%02x][%02x]...\n",
									pstRawItem->pucRaw[0], pstRawItem->pucRaw[1], pstRawItem->pucRaw[2], pstRawItem->pucRaw[3], pstRawItem->pucRaw[4]);
					goto ERROR;
				}
			}
			else
			{
				hErr = silib_table_ParseRawSecToTable (pstNextTable, pstPool->stProperty.eTextEnc, pstRawItem->pucRaw);
				if (ERR_OK == hErr)
				{
					SIxTable_PsiTable_t	*pstSameTable = silib_raw_FindPsiTableWithSameTypeAndExtId (pstTable, pstNextTable->eTableType, pstNextTable->unInfo.stTemplate.usExtId);

					if (NULL != pstSameTable)
					{
						HxList_t		*pstLastDesLoop = HLIB_LIST_Last (pstSameTable->pstDesLoop);
						HxList_t		*pstLast2ndLoop = HLIB_LIST_Last (pstSameTable->pst2ndLoop);

						if (NULL != pstLastDesLoop)
						{
							pstLastDesLoop->next = pstNextTable->pstDesLoop;

							if (pstNextTable->pstDesLoop != NULL)
							{
								pstNextTable->pstDesLoop->prev = pstLastDesLoop;
							}
						}
						else
						{
							pstSameTable->pstDesLoop = pstNextTable->pstDesLoop;
						}

						pstNextTable->pstDesLoop = NULL;

						if (NULL != pstLast2ndLoop)
						{
							pstLast2ndLoop->next = pstNextTable->pst2ndLoop;

							if (pstNextTable->pst2ndLoop != NULL)
							{
								pstNextTable->pst2ndLoop->prev = pstLast2ndLoop;
							}
						}
						else
						{
							pstSameTable->pst2ndLoop = pstNextTable->pst2ndLoop;
						}

						pstNextTable->pst2ndLoop = NULL;
					}
					else
					{
						SIxTable_PsiTable_t	*pstLastTable = silib_raw_GetLastPsiTable (pstTable);

						pstLastTable->pstNextTable = pstNextTable;
						pstNextTable = NULL;
					}
				}
				else
				{
					HxLOG_Error ("silib_table_ParseRawSecToTable err: Raw[%02x][%02x][%02x][%02x][%02x]...\n",
									pstRawItem->pucRaw[0], pstRawItem->pucRaw[1], pstRawItem->pucRaw[2], pstRawItem->pucRaw[3], pstRawItem->pucRaw[4]);
					goto ERROR;
				}
			}
		}
	}

	if (TRUE != bFound)
	{
		HxLOG_Debug ("No ExtID Raw Found: eSiType(%d)\n", eSiType);
		goto ERROR;
	}

	if (NULL != pstNextTable)
	{
		silib_table_ClearTable (pstNextTable);
		HLIB_STD_MemFree (pstNextTable);
	}

	return pstTable;

ERROR:
	if (NULL != pstTable)
	{
		silib_table_ClearTable (pstTable);
		HLIB_STD_MemFree (pstTable);
	}

	if (NULL != pstNextTable)
	{
		silib_table_ClearTable (pstNextTable);
		HLIB_STD_MemFree (pstNextTable);
	}

	return NULL;
}

HUINT32 SILIB_RAW_CountRaw (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId)
{
	HUINT32				 ulCount = 0;
	rawPool_t 		*pstPool = (rawPool_t *)hPool;
	rawItem_t 		*pstRawItem;
	rawBookmark_t 	 stBookmark;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d)\n", hPool, eSiType);
		return 0;
	}

	if (TRUE != bUseExtId)
	{
		ulExtId = SILIB_TABLE_EXT_ID_ALL;
	}

	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			ulCount ++;
		}
	}

	return ulCount;
}

HERROR SILIB_RAW_GetRawArrays (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulNumRaw)
{
	HUINT32				 ulCount = 0;
	rawPool_t 		*pstPool = (rawPool_t *)hPool;
	rawItem_t 		*pstRawItem;
	rawBookmark_t 	 stBookmark;

	if ((NULL == pstPool) || (eSiType >= eSIxTABLETYPE_MAX) ||
		(NULL == ppucRawArray) || (0 == ulRawArrayMax))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), eSiType(%d)\n", hPool, eSiType);
		return ERR_FAIL;
	}

	if (TRUE != bUseExtId)
	{
		ulExtId = SILIB_TABLE_EXT_ID_ALL;
	}

	silib_raw_InitBookmark (&(pstPool->astSiTable[eSiType]), &stBookmark);
	while (1)
	{
		pstRawItem = silib_raw_GetNextRawItem (&stBookmark);
		if (NULL == pstRawItem)
		{
			break;
		}

		if (silib_raw_CompareRawItemExtId (eSiType, pstRawItem, ulExtId))
		{
			if (ulCount < ulRawArrayMax)
			{
				ppucRawArray[ulCount] = pstRawItem->pucRaw;
				ulCount++;
			}
		}
	}

	if (NULL != pulNumRaw)					{ *pulNumRaw = ulCount; }
	return (ulCount > 0) ? ERR_OK : ERR_FAIL;
}

SIxDescriptor_t *SILIB_RAW_ParseDes (SIxRawPoolHandle_t hPool, const HUINT8 *pucRawData)
{
	SIxDescriptor_t				*pstDes = NULL;
	rawPool_t 					*pstPool = (rawPool_t *)hPool;
	SIxDescriptorHeaderParam_t	 stDesParam;
	HERROR				 		 hErr;
	SIxSpec_e					 eSpec = eSIxSPEC_DVB;

	if ((NULL == pstPool) || (NULL == pucRawData))
	{
		HxLOG_Error ("Wrong Arg: hPool(0x%08x), pucRawData(0x%08x)\n", hPool, pucRawData);
		goto ERROR;
	}

	stDesParam.eTextEnc = pstPool->stProperty.eTextEnc;
	stDesParam.pszArg = NULL;

	// private descriptor region 에 해당하는 tag 이거나
	// si pool spec이 dvbext, ssuunt 이면 여기서 spec을 할당해 준다
	if ((pucRawData[0] >= 0x80)
		|| ((pstPool->stProperty.eSpec == eSIxSPEC_DVBEXT) || (pstPool->stProperty.eSpec == eSIxSPEC_SSUUNT)))
	{
		eSpec = pstPool->stProperty.eSpec;
	}

	hErr = SILIB_DESC_New ((const SIxDescriptorTag_e)pucRawData[0], (const SIxSpec_e)eSpec, (const SIxDescriptorHeaderParam_t *)&stDesParam, pucRawData, &pstDes);
	if ((ERR_OK != hErr) || (NULL == pstDes))
	{
		HxLOG_Error ("SILIB_DESC_New err: DescTag=(0x%X)\n", pucRawData[0]);
		goto ERROR;
	}

	return pstDes;

ERROR:
	if (NULL != pstDes)
	{
		SILIB_DESC_Delete (pstDes);
	}

	return NULL;
}

