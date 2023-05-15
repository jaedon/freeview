/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_epg.h>

#include "../local_include/_svc_epg_common.h"
#include "../local_include/_svc_epg_common_lib.h"
#include "../local_include/_svc_epg_raw_mgr.h"
#include "_svc_epg_raw_pool.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(hIdx)				(HUINT16)(hIdx >> 16 & 0xffff)
#define	SvcEpg_MACRO_GET_INDEX_FROM_IDXHANDLE(hIdx)				(HUINT16)(hIdx & 0xffff)
#define	SvcEpg_MACRO_MAKE_INDEX_HANDLE(usUniqueId, usIdx)		(HUINT32)(usUniqueId << 16 | usIdx)

#define SvcEpg_EXTID_USE		0
#define SvcEpg_EXTID_UNUSE		1
#define SvcEpg_EXTID_PRIVATE	2

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


#define ____LOCAL_FUNC____

STATIC HUINT8 svc_epg_RawPoolGetExtIdType(HUINT32 eSiTableType)
{
	switch(eSiTableType)
	{
		case eSI_TableType_PMT:
		case eSI_TableType_NIT_ACT:
		case eSI_TableType_NIT_OTH:
		case eSI_TableType_SDT_ACT:
		case eSI_TableType_SDT_OTH:
		case eSI_TableType_BAT:
		case eSI_TableType_UNT:
		case eSI_TableType_EIT_ACTPF:
		case eSI_TableType_EIT_OTHPF:
		case eSI_TableType_EIT_ACTSC:
		case eSI_TableType_EIT_OTHSC:
			return SvcEpg_EXTID_USE;

		case eSI_TableType_PAT:
		case eSI_TableType_CAT:
		case eSI_TableType_TOT:
		case eSI_TableType_TDT:
			return SvcEpg_EXTID_UNUSE;

		default:
			/* private section의 경우는 section_syntax_indicator로 판단하자. */
			return SvcEpg_EXTID_PRIVATE;
	}
}

STATIC svcEpg_RawSecTable_t *svc_epg_RawPoolGetRawTable(svcEpg_RawPool_t *pstPool, SiTableType_e eTableType)
{
	if (pstPool == NULL)					{ return NULL; }
	if (eTableType >= MAX_SI_TABLE_TYPE)	{ return NULL; }

	return &(pstPool->astSiTable[eTableType]);
}

STATIC HUINT8 *svc_epg_RawPoolGetRawSection(const svcEpg_RawPool_t *pstPool, HUINT32 ulSecIdx)
{
	if (pstPool == NULL)								{ return NULL; }
	if (pstPool->usMaxNumRawSec <= (HUINT16)ulSecIdx)	{ return NULL; }

	return pstPool->ppucRawSec[ulSecIdx];
}

STATIC HUINT16 svc_epg_RawPoolGetExtIdFromRawData(HUINT8 *pucRaw)
{
	if (pucRaw != NULL)
	{
		return HxMACRO_Get16Bit(pucRaw+3);
	}

	return 0;
}

STATIC HERROR svc_epg_RawPoolFreeSectionData(svcEpg_RawPool_t *pstPool, HUINT16 usIndex, HBOOL bFreeRawData)
{
	if (pstPool == NULL)						{ return ERR_FAIL; }
	if (pstPool->usMaxNumRawSec <= usIndex)		{ return ERR_FAIL; }

	if (pstPool->ppucRawSec)
	{
		if (pstPool->ppucRawSec[usIndex])
		{
			if (bFreeRawData)
			{
				OxEPG_Free (pstPool->ppucRawSec[usIndex]);
			}
			pstPool->ppucRawSec[usIndex] = NULL;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC void svc_epg_RawPoolRecalculateIndexStartPointInSecTable(svcEpg_RawSecTable_t *pstSecTable)
{
	HUINT16		usIndex;

	if (pstSecTable == NULL)					{ return; }

	for (usIndex = pstSecTable->usStartPos; usIndex < pstSecTable->usInsertPos; usIndex++)
	{
		if (pstSecTable->phIdx[usIndex] != SvcEpg_INVALID_INDEX_HANDLE)
		{
			// New Start Position :
			pstSecTable->usStartPos = usIndex;
			return;
		}
	}

	// All is clean, no section are inside:
	pstSecTable->usStartPos = 0;
	pstSecTable->usInsertPos = 0;

	return;
}

STATIC void svc_epg_RawPoolFreeAllReceivingRawTableInSecTable(svcEpg_RawSecTable_t *pstSecTable, HBOOL bRealFree)
{
	HUINT32					 ulCntRecv;
	svcEpg_ReceivingRaw_t	*pstReceiving, *pstRecvToFree;

	if (pstSecTable)
	{
		if (pstSecTable->pstReceivingTable)
		{
			pstReceiving = pstSecTable->pstReceivingTable;
			pstSecTable->pstReceivingTable = NULL;

			while (pstReceiving)
			{
				for (ulCntRecv = 0; ulCntRecv < SvcEpg_NUM_SEC_FOR_RECEIVING_TABLE; ulCntRecv++)
				{
					if (pstReceiving->apucRawSec[ulCntRecv] != NULL)
					{
						OxEPG_Free(pstReceiving->apucRawSec[ulCntRecv]);
					}
				}

				pstRecvToFree = pstReceiving;
				pstReceiving = pstReceiving->pstNext;

				if (bRealFree == TRUE)
				{
					OxEPG_Free (pstRecvToFree);
				}
				else
				{
					HxSTD_MemSet(pstRecvToFree, 0, sizeof(svcEpg_ReceivingRaw_t));
					pstRecvToFree->pstNext = pstSecTable->pstFreeRcvTable;
					pstSecTable->pstFreeRcvTable = pstRecvToFree;
				}
			}

			// if it shall really be freed, then free table list shall be freed all
			if (bRealFree == TRUE)
			{
				pstReceiving = pstSecTable->pstFreeRcvTable;
				pstSecTable->pstFreeRcvTable = NULL;

				while (pstReceiving)
				{
					for (ulCntRecv = 0; ulCntRecv < SvcEpg_NUM_SEC_FOR_RECEIVING_TABLE; ulCntRecv++)
					{
						if (pstReceiving->apucRawSec[ulCntRecv] != NULL)
						{
							OxEPG_Free (pstReceiving->apucRawSec[ulCntRecv]);
						}
					}

					pstRecvToFree = pstReceiving;
					pstReceiving = pstReceiving->pstNext;

					OxEPG_Free (pstRecvToFree);
				}
			}
		}
	}
}

STATIC HERROR svc_epg_RawPoolPutSection(svcEpg_RawPool_t *pstPool, HUINT8 *pucRawSec, HUINT16 *pusIndex)
{
	HUINT16		  usCount;
	HUINT32		  ulNewSize;
	HUINT8		**ppucNewRawSec;

	if (pstPool->ppucRawSec)
	{
		for (usCount = pstPool->usLastEmptySecIdx; usCount < pstPool->usMaxNumRawSec; usCount++)
		{
			if (pstPool->ppucRawSec[usCount] == NULL)
			{
				pstPool->ppucRawSec[usCount] = pucRawSec;
				pstPool->usLastEmptySecIdx = usCount;

				if (pusIndex)			{ *pusIndex = usCount; }
				return ERR_OK;
			}
		}

		for (usCount = 0; usCount < pstPool->usLastEmptySecIdx; usCount++)
		{
			if (pstPool->ppucRawSec[usCount] == NULL)
			{
				pstPool->ppucRawSec[usCount] = pucRawSec;
				pstPool->usLastEmptySecIdx = usCount;

				if (pusIndex)			{ *pusIndex = usCount; }
				return ERR_OK;
			}
		}
	}

	// No empty section slot : enlarge the buffer
	usCount = pstPool->usMaxNumRawSec;
	ulNewSize = (HUINT32)pstPool->usMaxNumRawSec + MAX_SI_RAWSEC_NUM;
	if (ulNewSize >= 0xFFFF)		{ ulNewSize = 0xFFFF; }

	// Already the buffer exceed the maximum size :
	if ((HUINT32)usCount >= ulNewSize)
	{
		return ERR_FAIL;
	}

	ppucNewRawSec = (HUINT8 **)OxEPG_Malloc (sizeof(HUINT8 *) * ulNewSize);
	if (ppucNewRawSec == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(ppucNewRawSec, 0, sizeof(HUINT8 *) * ulNewSize);
	if (pstPool->ppucRawSec)
	{
		HxSTD_MemCopy(ppucNewRawSec, pstPool->ppucRawSec, sizeof(HUINT8 *) * pstPool->usMaxNumRawSec);
		OxEPG_Free (pstPool->ppucRawSec);
	}

	pstPool->usMaxNumRawSec		 = (HUINT16)ulNewSize;
	pstPool->ppucRawSec			 = ppucNewRawSec;
	pstPool->ppucRawSec[usCount] = pucRawSec;
	pstPool->usLastEmptySecIdx	 = usCount;

	if (pusIndex)			{ *pusIndex = usCount; }
	return ERR_OK;
}

STATIC void svc_epg_RawPoolArrangeIndexHandle(svcEpg_RawSecTable_t *pstTable)
{
	HUINT16		 usOldIndex, usNewIndex;

	usOldIndex = pstTable->usStartPos;
	usNewIndex = 0;
	while (usOldIndex < pstTable->usInsertPos)
	{
		if (pstTable->phIdx[usOldIndex] == SvcEpg_INVALID_INDEX_HANDLE)
		{
			// Skip to the next index
			usOldIndex++;
		}
		else
		{
			pstTable->phIdx[usNewIndex] = pstTable->phIdx[usOldIndex];
			usOldIndex++;
			usNewIndex++;
		}
	}

	pstTable->usInsertPos = usNewIndex;
	pstTable->usSecNum	  = usNewIndex;
	pstTable->usStartPos = 0;

	while (usNewIndex < pstTable->usMaxSecNum)
	{
		pstTable->phIdx[usNewIndex] = SvcEpg_INVALID_INDEX_HANDLE;
		usNewIndex++;
	}

	return;
}

STATIC HERROR svc_epg_RawPoolExtendIndexHandleBuffer(svcEpg_RawSecTable_t *pstTable)
{
	HUINT32			 ulCount, ulNewMaxNum;
	HUINT32			*phNewIdx;

	if (pstTable->usMaxSecNum == 0xFFFF)
	{
		HxLOG_Print ("Maximum Buffer : It cannot be extended\n");
		return ERR_FAIL;
	}

	ulNewMaxNum = (HUINT32)pstTable->usMaxSecNum + SvcEpg_MAX_INDEX_HANDLE_NUM;
	if (ulNewMaxNum >= 0xFFFF)			{ ulNewMaxNum = 0xFFFF; }


	phNewIdx = (HUINT32 *)OxEPG_Malloc (ulNewMaxNum * sizeof(HUINT32));
	if (phNewIdx == NULL)
	{
		HxLOG_Print ("Mem allocation fail\n");
		return ERR_FAIL;
	}

	if (pstTable->phIdx && pstTable->usMaxSecNum)
	{
		HxSTD_MemCopy(phNewIdx, pstTable->phIdx, pstTable->usMaxSecNum * sizeof(HUINT32));
		OxEPG_Free (pstTable->phIdx);
	}

	for (ulCount = (HUINT32)pstTable->usMaxSecNum; ulCount < ulNewMaxNum; ulCount++)
	{
		phNewIdx[ulCount] = SvcEpg_INVALID_INDEX_HANDLE;
	}

	pstTable->phIdx = phNewIdx;
	pstTable->usInsertPos = pstTable->usMaxSecNum;
	pstTable->usMaxSecNum = (HUINT16)ulNewMaxNum;

	return ERR_OK;
}

#define ____GLOBAL_FUNC____

svcEpg_RawPool_t *svc_epg_RawPoolAlloc(HUINT16 usMaxRawSec, HUINT8 *szPoolName)
{
	svcEpg_RawPool_t		*pstPool;

	if (usMaxRawSec == 0)
	{
		usMaxRawSec = SvcEpg_MAX_SI_RAWSEC_NUM;
	}

	pstPool = (svcEpg_RawPool_t *)OxEPG_Malloc (sizeof(svcEpg_RawPool_t));
	if (pstPool == NULL)
	{
		return NULL;
	}

	HxSTD_MemSet(pstPool, 0, sizeof(svcEpg_RawPool_t));

	pstPool->ppucRawSec = (HUINT8 **)OxEPG_Malloc (sizeof(HUINT8 *) * usMaxRawSec);
	if (pstPool->ppucRawSec == NULL)
	{
		OxEPG_Free (pstPool);
		return NULL;
	}

	HxSTD_MemSet(pstPool->ppucRawSec, 0, sizeof(HUINT8 *) * usMaxRawSec);
	pstPool->usMaxNumRawSec = usMaxRawSec;

	if (szPoolName)
	{
		HLIB_STD_StrUtf8NCpy (pstPool->szPoolName, szPoolName, SvcEpg_RAW_POOL_NAME_LEN );
	}

	return pstPool;

}

HUINT32 svc_epg_RawPoolFreeSection(svcEpg_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId)
{
	HUINT8			 ucExtIdType;
	HUINT16			 usCount;
	HUINT16			 usSecIndex;
	HUINT16			 usExtIdFromRaw;
	svcEpg_RawSecTable_t	*pstSecTable;

	if (pstPool == NULL)							{ return ERR_FAIL; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	ucExtIdType = svc_epg_RawPoolGetExtIdType(eSiTableType);

#if 0
	if(ucExtIdType == SvcEpg_EXTID_UNUSE)
	{
		HxLOG_Print ("USE MWC_SI_FreeAllSection() to free the SI TABLE WITHOUT EXT_ID!!!\n");
		return ERR_FAIL;
	}
#endif

	pstSecTable = svc_epg_RawPoolGetRawTable(pstPool, eSiTableType);
	for (usCount = pstSecTable->usStartPos; usCount < pstSecTable->usInsertPos; usCount++)
	{
		if (pstSecTable->phIdx[usCount] == SvcEpg_INVALID_INDEX_HANDLE)								{ continue; }
		if (usUniqueId != SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(pstSecTable->phIdx[usCount]))		{ continue; }

		usSecIndex = SvcEpg_MACRO_GET_INDEX_FROM_IDXHANDLE (pstSecTable->phIdx[usCount]);
		if (usSecIndex >= pstPool->usMaxNumRawSec)			{ continue; }
		if (pstPool->ppucRawSec[usSecIndex] == NULL)		{ continue; }

		if (bUseExtId == TRUE)
		{
			if (ucExtIdType == SvcEpg_EXTID_USE)
			{
				usExtIdFromRaw = svc_epg_RawPoolGetExtIdFromRawData(pstPool->ppucRawSec[usSecIndex]);
				if (usExtIdFromRaw == usExtId)
				{
					svc_epg_RawPoolFreeSectionData(pstPool, usSecIndex, TRUE);
					pstSecTable->phIdx[usCount] = SvcEpg_INVALID_INDEX_HANDLE;
					pstSecTable->usSecNum--;
				}
			}
			else if (ucExtIdType == SvcEpg_EXTID_UNUSE)
			{
				svc_epg_RawPoolFreeSectionData(pstPool, usSecIndex, TRUE);
				pstSecTable->phIdx[usCount] = SvcEpg_INVALID_INDEX_HANDLE;
				pstSecTable->usSecNum--;
			}
			else /* ucExtIdType --> SvcEpg_EXTID_PRIVATE */
			{
				HUINT8	*p;
				HUINT8	ucSecSyntaxIndicator;

				p = pstPool->ppucRawSec[usSecIndex];
				p++;
				ucSecSyntaxIndicator =(*p & 0x80) >> 7;
				if(ucSecSyntaxIndicator == 1)
				{
					usExtIdFromRaw = svc_epg_RawPoolGetExtIdFromRawData(pstPool->ppucRawSec[usSecIndex]);
					if (usExtIdFromRaw == usExtId)
					{
						svc_epg_RawPoolFreeSectionData(pstPool, usSecIndex, TRUE);
						pstSecTable->phIdx[usCount] = SvcEpg_INVALID_INDEX_HANDLE;
						pstSecTable->usSecNum--;
					}
				}
			}
		}
		else
		{
			svc_epg_RawPoolFreeSectionData(pstPool, usSecIndex, TRUE);
			pstSecTable->phIdx[usCount] = SvcEpg_INVALID_INDEX_HANDLE;
			pstSecTable->usSecNum--;
		}
	}

	svc_epg_RawPoolRecalculateIndexStartPointInSecTable (pstSecTable);
	return ERR_OK;
}

void svc_epg_RawPoolFreeAllSection(svcEpg_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType)
{
	HUINT16			 usCount;
	HUINT32			 ulIndex;
	svcEpg_RawSecTable_t	*pstSecTable;
	HUINT32			 ulRes;

	if (pstPool == NULL)							{ return; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return; }

	pstSecTable = svc_epg_RawPoolGetRawTable (pstPool, eSiTableType);

	for (usCount = pstSecTable->usStartPos; usCount < pstSecTable->usInsertPos; usCount++)
	{
		if (pstSecTable->phIdx[usCount] != SvcEpg_INVALID_INDEX_HANDLE)
		{
			ulIndex = pstSecTable->phIdx[usCount];

			if (usUniqueId == SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(ulIndex))
			{
				ulRes = svc_epg_RawPoolFreeSectionData(pstPool, SvcEpg_MACRO_GET_INDEX_FROM_IDXHANDLE(ulIndex), TRUE);
				if (ulRes != ERR_OK)
				{
					HxLOG_Print ("svc_epg_RawPoolFreeSectionData (%08x)\n", ulRes);
				}

				pstSecTable->phIdx[usCount] = SvcEpg_INVALID_INDEX_HANDLE;
				pstSecTable->usSecNum--;
			}
		}
	}

	svc_epg_RawPoolRecalculateIndexStartPointInSecTable (pstSecTable);
	return;
}

void svc_epg_RawPoolClear(svcEpg_RawPool_t *pstPool)
{
	HUINT16			 usCount, ulIndexCnt;
	svcEpg_RawSecTable_t	*pstSecTable;

	if (pstPool == NULL)			{ return; }

	for (usCount = 0; usCount < MAX_SI_TABLE_TYPE; usCount++)
	{
		pstSecTable = svc_epg_RawPoolGetRawTable (pstPool, usCount);

		if (pstSecTable->phIdx)
		{
			for (ulIndexCnt = 0; ulIndexCnt < pstSecTable->usMaxSecNum; ulIndexCnt++)
			{
				pstSecTable->phIdx[ulIndexCnt] = SvcEpg_INVALID_INDEX_HANDLE;
			}
		}

		// All receiving Table이 남아 있는 경우
		if (pstSecTable->pstReceivingTable)
		{
			svc_epg_RawPoolFreeAllReceivingRawTableInSecTable (pstSecTable, FALSE);
		}

		pstSecTable->usSecNum = 0;
		pstSecTable->usStartPos  = 0;
		pstSecTable->usInsertPos = 0;
	}

	if (pstPool->ppucRawSec)
	{
		for (usCount = 0; usCount < pstPool->usMaxNumRawSec; usCount++)
		{
			if (pstPool->ppucRawSec[usCount])
			{
				OxEPG_Free (pstPool->ppucRawSec[usCount]);
				pstPool->ppucRawSec[usCount] = NULL;
			}
		}
	}

	return;
}

HUINT32 svc_epg_RawPoolAddSection(svcEpg_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec)
{
	HUINT16			 usIndex;
	svcEpg_RawSecTable_t	*pstSecTable;
	HUINT32			 ulRes;

	if (pstPool == NULL)							{ return ERR_FAIL; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	ulRes = svc_epg_RawPoolPutSection(pstPool, pucRawSec, &usIndex);
	if(ulRes == ERR_OK)
	{
		pstSecTable = svc_epg_RawPoolGetRawTable(pstPool, eSiTableType);

		if (pstSecTable->usInsertPos >= pstSecTable->usMaxSecNum)
		{
			svc_epg_RawPoolArrangeIndexHandle(pstSecTable);

			if (pstSecTable->usInsertPos >= pstSecTable->usMaxSecNum)
			{
				ulRes = svc_epg_RawPoolExtendIndexHandleBuffer(pstSecTable);
				if (ulRes != ERR_OK)
				{
					svc_epg_RawPoolFreeSectionData(pstPool, usIndex, FALSE);
					return ERR_FAIL;
				}
			}
		}

		pstSecTable->phIdx[pstSecTable->usInsertPos] = SvcEpg_MACRO_MAKE_INDEX_HANDLE(usUniqueId, usIndex);
		pstSecTable->usSecNum++;
		pstSecTable->usInsertPos++;

		return ERR_OK;
	}

	return ulRes;
}

svcEpg_RawSecTable_t *svc_epg_RawPoolGetTable(svcEpg_RawPool_t *pstPool, SiTableType_e eTableType)
{
	return svc_epg_RawPoolGetRawTable(pstPool, eTableType);
}

HUINT8 *svc_epg_RawPoolGetSection(const svcEpg_RawPool_t *pstPool, HUINT32 ulSecIdx)
{
	return svc_epg_RawPoolGetRawSection(pstPool, ulSecIdx);
}


/*********************** End of File ******************************/
