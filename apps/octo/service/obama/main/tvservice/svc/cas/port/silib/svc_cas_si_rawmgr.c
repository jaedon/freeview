/**
	@file     mwc_rawmgr.c
	@brief    M/W common raw section manager

	Description: si table의 raw section을 관리하는 함수군을 제공한다. \n
	Module: MW - Common \n
	Remarks : Octo Base \n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <util.h>

#include <db_svc.h>

#include "../local_include/_svc_cas_si_api.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/





/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define PMT_ANY_PROGRAM	            0xffffffff

/* 다음 값을 16, 100 이렇게 줄여서도 테스트 했었음. */
/* 잠시 이동 silibapi.h
#define MAX_INDEX_HANDLE_NUM		128
#define	MAX_SI_RAWSEC_NUM			4000

#define	MACRO_MAKE_INDEX_HANDLE(usUniqueId, usIdx)	(HUINT32)(usUniqueId << 16 | usIdx)
#define	MACRO_GET_UNIQUEID_FROM_IDXHANDLE(hIdx)		(HUINT16)(hIdx >> 16 & 0xffff)
#define	MACRO_GET_INDEX_FROM_IDXHANDLE(hIdx)		(HUINT16)(hIdx & 0xffff)
*/

#define EXTID_USE		0
#define EXTID_UNUSE		1
#define EXTID_PRIVATE	2

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#if 0
/** @brief enummeration에 대한 간단한 설명. */
typedef enum
{
    eModule_DoWhat0=0, 				/**< Value 설명	*/
    eModule_DoWhat1 				/**< Value 설명	*/
}AV_DualMonoMode_;
#endif

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/******************* internal function prototypes ******************/
/*******************************************************************/


#define ___________________________________________________________

STATIC HERROR	svc_cas_RawPutSection(svcCas_RawPool_t *pstPool, HUINT8 *pucRawSec, HUINT16 *pusIndex);
STATIC HERROR	svc_cas_RawFreeSectionBySectionIdx(svcCas_RawPool_t *pstPool, HUINT16 usIndex, HBOOL bFreeRawData);
STATIC void		svc_cas_RawArrangeIndexHandle(svcCas_RawSecTable_t *pstTable);
STATIC HERROR	svc_cas_RawExtendIndexHandleBuffer(svcCas_RawSecTable_t *pstTable);
STATIC HUINT8	svc_cas_RawGetExtIdType(HUINT32 eSiTableType);
STATIC HUINT32	svc_cas_RawGetSectionNumAndListInternal(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusCnt, HUINT16 *pusSecIdxList);
STATIC void		svc_cas_RawRecalculateIndexStartPointInSecTable (svcCas_RawSecTable_t *pstSecTable);
STATIC void		svc_cas_RawFreeAllReceivingRawTableInSecTable (svcCas_RawSecTable_t *pstSecTable, HBOOL bRealFree);
STATIC void		svc_cas_RawFreeReceivingRawTableInSecTable (svcCas_RawSecTable_t *pstSecTable, HUINT16 usExtId);
STATIC HUINT8 **svc_cas_RawGetRawArraysInteral (svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													SiTableType_e eSiType,
													HBOOL bUseExtId,
													HUINT16 usExtId,
													HUINT32	*pulArrayLen);

STATIC svcCas_RawReceiving_t *svc_cas_RawAllocReceivingRawTable (svcCas_RawSecTable_t *pstSecTable);
STATIC HERROR	svc_cas_RawFindReceivingTableWithSecNum (svcCas_RawPool_t *pstPool,
																		HUINT16 usTsUniqId,
																		SiTableType_e eSiType,
																		HUINT16 usExtId,
																		HUINT8 ucSecNum,
																		svcCas_RawReceiving_t **ppstRcvTable);
STATIC HERROR	svc_cas_RawMakeReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8  ucLastSecNum,
														HUINT8	ucVer);
#if 0
STATIC HERROR	svc_cas_RawCheckRawSamenessInReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw);
#endif
STATIC HERROR	svc_cas_RawAddRawToReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw);
#if 0
STATIC HERROR	svc_cas_RawIsMultiSecFinishedInReceivingTable  (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId);
STATIC HUINT8**	svc_cas_RawGetRawArraysFromReceivingTable (svcCas_RawPool_t *pstPool,
																			HUINT16 usTsUniqId,
																			SiTableType_e eSiType,
																			HUINT16 usExtId,
																			HUINT32	*pulArrayLen);
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define ___________________________________________________________

HERROR svc_cas_RawInit(void)
{
	// To do :

	return ERR_OK;
}

svcCas_RawPool_t *svc_cas_RawAllocRawPool(HUINT16 usMaxRawSec, HUINT8 *szPoolName)
{
	svcCas_RawPool_t		*pstPool;

	if (usMaxRawSec == 0)
	{
		usMaxRawSec = MAX_SI_RAWSEC_NUM;
	}

	pstPool = (svcCas_RawPool_t *)OxCAS_Malloc (sizeof(svcCas_RawPool_t));
	if (pstPool == NULL)
	{
		return NULL;
	}

	HxSTD_MemSet(pstPool, 0, sizeof(svcCas_RawPool_t));

	pstPool->ppucRawSec = (HUINT8 **)OxCAS_Malloc (sizeof(HUINT8 *) * usMaxRawSec);
	if (pstPool->ppucRawSec == NULL)
	{
		OxCAS_Free (pstPool);
		return NULL;
	}

	HxSTD_MemSet(pstPool->ppucRawSec, 0, sizeof(HUINT8 *) * usMaxRawSec);
	pstPool->usMaxNumRawSec = usMaxRawSec;

	if (szPoolName)
	{
		HLIB_STD_StrUtf8NCpy (pstPool->szPoolName, szPoolName, RAW_POOL_NAME_LEN );
	}

	return pstPool;

}

void svc_cas_RawClearRawPool(svcCas_RawPool_t *pstPool)
{
	HUINT16			 usCount, ulIndexCnt;
	svcCas_RawSecTable_t	*pstSecTable;

	if (pstPool == NULL)			{ return; }

	for (usCount = 0; usCount < MAX_SI_TABLE_TYPE; usCount++)
	{
		pstSecTable = svc_cas_RawGetRawTable (pstPool, usCount);

		if (pstSecTable->phIdx)
		{
			for (ulIndexCnt = 0; ulIndexCnt < pstSecTable->usMaxSecNum; ulIndexCnt++)
			{
				pstSecTable->phIdx[ulIndexCnt] = INVALID_INDEX_HANDLE;
			}
		}

		// All receiving Table이 남아 있는 경우
		if (pstSecTable->pstReceivingTable)
		{
			svc_cas_RawFreeAllReceivingRawTableInSecTable (pstSecTable, FALSE);
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
				OxCAS_Free (pstPool->ppucRawSec[usCount]);
				pstPool->ppucRawSec[usCount] = NULL;
			}
		}
	}

	return;
}

void svc_cas_RawFreeRawPool(svcCas_RawPool_t *pstPool)
{
	HUINT16			 usCount;
	svcCas_RawSecTable_t	*pstSecTable;

	if (pstPool == NULL)			{ return; }

	// Free the raw data
	if (pstPool->ppucRawSec)
	{
		for (usCount = 0; usCount < pstPool->usMaxNumRawSec; usCount++)
		{
			if (pstPool->ppucRawSec[usCount])
			{
				OxCAS_Free (pstPool->ppucRawSec[usCount]);
				pstPool->ppucRawSec[usCount] = NULL;
			}
		}

		OxCAS_Free (pstPool->ppucRawSec);
	}

	// Free the raw data index array in the si table
	for (usCount = 0; usCount < MAX_SI_TABLE_TYPE; usCount++)
	{
		pstSecTable = svc_cas_RawGetRawTable (pstPool, usCount);

		if (pstSecTable->phIdx)
		{
			OxCAS_Free (pstSecTable->phIdx);
		}

		// All receiving Table이 남아 있는 경우
		if (pstSecTable->pstReceivingTable)
		{
			svc_cas_RawFreeAllReceivingRawTableInSecTable (pstSecTable, TRUE);
		}
	}

	OxCAS_Free (pstPool);
	return;
}

HUINT32 svc_cas_RawAddRawSection(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec)
{
	HUINT16			 usIndex;
	svcCas_RawSecTable_t	*pstSecTable;
	HUINT32			 ulRes;

	if (pstPool == NULL)							{ return ERR_FAIL; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	ulRes = svc_cas_RawPutSection (pstPool, pucRawSec, &usIndex);
	if(ulRes == ERR_OK)
	{
		pstSecTable = svc_cas_RawGetRawTable (pstPool, eSiTableType);

		if (pstSecTable->usInsertPos >= pstSecTable->usMaxSecNum)
		{
			svc_cas_RawArrangeIndexHandle (pstSecTable);

			if (pstSecTable->usInsertPos >= pstSecTable->usMaxSecNum)
			{
				ulRes = svc_cas_RawExtendIndexHandleBuffer (pstSecTable);
				if (ulRes != ERR_OK)
				{
					svc_cas_RawFreeSectionBySectionIdx(pstPool, usIndex, FALSE);
					return ERR_FAIL;
				}
			}
		}

		pstSecTable->phIdx[pstSecTable->usInsertPos] = MACRO_MAKE_INDEX_HANDLE (usUniqueId, usIndex);
		pstSecTable->usSecNum++;
		pstSecTable->usInsertPos++;

		return ERR_OK;
	}

	return ulRes;
}

void svc_cas_RawFreeAllSection(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType)
{
	HUINT16			 usCount;
	HUINT32			 ulIndex;
	svcCas_RawSecTable_t	*pstSecTable;
	HUINT32			 ulRes;

	if (pstPool == NULL)							{ return; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return; }

	pstSecTable = svc_cas_RawGetRawTable (pstPool, eSiTableType);

	for (usCount = pstSecTable->usStartPos; usCount < pstSecTable->usInsertPos; usCount++)
	{
		if (pstSecTable->phIdx[usCount] != INVALID_INDEX_HANDLE)
		{
			ulIndex = pstSecTable->phIdx[usCount];

			if (usUniqueId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(ulIndex))
			{
				ulRes = svc_cas_RawFreeSectionBySectionIdx(pstPool, MACRO_GET_INDEX_FROM_IDXHANDLE(ulIndex), TRUE);
				if (ulRes != ERR_OK)
				{
					HxLOG_Print ("[svc_cas_RawFreeAllSection] svc_cas_RawFreeSectionBySectionIdx (%08x)\n", ulRes);
				}

				pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
				pstSecTable->usSecNum--;
			}
		}
	}

	svc_cas_RawRecalculateIndexStartPointInSecTable (pstSecTable);
	return;
}

HUINT32 svc_cas_RawFreeSection(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId)
{
	HUINT8			 ucExtIdType;
	HUINT16			 usCount;
	HUINT16			 usSecIndex;
	HUINT16			 usExtIdFromRaw;
	svcCas_RawSecTable_t	*pstSecTable;

	if (pstPool == NULL)							{ return ERR_FAIL; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	ucExtIdType = svc_cas_RawGetExtIdType (eSiTableType);

#if 0
	if(ucExtIdType == EXTID_UNUSE)
	{
		HxLOG_Print ("USE svc_cas_SiFreeAllSection() to free the SI TABLE WITHOUT EXT_ID!!!\n");
		return ERR_FAIL;
	}
#endif

	pstSecTable = svc_cas_RawGetRawTable (pstPool, eSiTableType);
	for (usCount = pstSecTable->usStartPos; usCount < pstSecTable->usInsertPos; usCount++)
	{
		if (pstSecTable->phIdx[usCount] == INVALID_INDEX_HANDLE)								{ continue; }
		if (usUniqueId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstSecTable->phIdx[usCount]))		{ continue; }

		usSecIndex = MACRO_GET_INDEX_FROM_IDXHANDLE (pstSecTable->phIdx[usCount]);
		if (usSecIndex >= pstPool->usMaxNumRawSec)			{ continue; }
		if (pstPool->ppucRawSec[usSecIndex] == NULL)		{ continue; }

		if (bUseExtId == TRUE)
		{
			if (ucExtIdType == EXTID_USE)
			{
				usExtIdFromRaw = svc_cas_RawGetExtIdFromRawData (pstPool->ppucRawSec[usSecIndex]);
				if (usExtIdFromRaw == usExtId)
				{
					svc_cas_RawFreeSectionBySectionIdx(pstPool, usSecIndex, TRUE);
					pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
					pstSecTable->usSecNum--;
				}
			}
			else if (ucExtIdType == EXTID_UNUSE)
			{
				svc_cas_RawFreeSectionBySectionIdx(pstPool, usSecIndex, TRUE);
				pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
				pstSecTable->usSecNum--;
			}
			else /* ucExtIdType --> EXTID_PRIVATE */
			{
				HUINT8	*p;
				HUINT8	ucSecSyntaxIndicator;

				p = pstPool->ppucRawSec[usSecIndex];
				p++;
				ucSecSyntaxIndicator =(*p & 0x80) >> 7;
				if(ucSecSyntaxIndicator == 1)
				{
					usExtIdFromRaw = svc_cas_RawGetExtIdFromRawData (pstPool->ppucRawSec[usSecIndex]);
					if (usExtIdFromRaw == usExtId)
					{
						svc_cas_RawFreeSectionBySectionIdx(pstPool, usSecIndex, TRUE);
						pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
						pstSecTable->usSecNum--;
					}
				}
			}
		}
		else
		{
			svc_cas_RawFreeSectionBySectionIdx(pstPool, usSecIndex, TRUE);
			pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
			pstSecTable->usSecNum--;
		}
	}

	svc_cas_RawRecalculateIndexStartPointInSecTable (pstSecTable);
	return ERR_OK;
}

HUINT32 svc_cas_RawGetSectionNum(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusCnt)
{
	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pusCnt == NULL)		{ return ERR_FAIL; }

	return svc_cas_RawGetSectionNumAndListInternal(pstPool, usUniqueId, eSiTableType, pusCnt, NULL);
}

HUINT32 svc_cas_RawGetSectionList(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusSecIdxList)
{
	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pusSecIdxList == NULL)		{ return ERR_FAIL; }

	return svc_cas_RawGetSectionNumAndListInternal(pstPool, usUniqueId, eSiTableType, NULL, pusSecIdxList);
}

HUINT32 svc_cas_RawGetSectionNumAndList(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusCnt, HUINT16 **ppusSecIdxList)
{
	HUINT16		*pusIdxList;
	HUINT32		 ulRes;

	if (ppusSecIdxList)
	{
		pusIdxList = (HUINT16 *)OxCAS_Malloc(256 * sizeof(HUINT16));
		if (pusIdxList == NULL)			{ return ERR_FAIL; }

		HxSTD_MemSet(pusIdxList, 0, 256 * sizeof(HUINT16));
	}
	else
	{
		pusIdxList = NULL;
	}

	ulRes = svc_cas_RawGetSectionNumAndListInternal(pstPool, usUniqueId, eSiTableType, pusCnt, pusIdxList);
	if (ulRes != ERR_OK)
	{
		if (pusIdxList != NULL)			{ OxCAS_Free (pusIdxList); }
		return ulRes;
	}

	if (ppusSecIdxList)
	{
		*ppusSecIdxList = pusIdxList;
	}

	return ERR_OK;
}

HERROR	svc_cas_RawGetVerAndCrc(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiType, HBOOL bUseExtId, HUINT16 usExtId, HUINT8 *pucVer, HUINT32 *pulCrc32)
{
	HUINT8			*pucRaw;
	HUINT16			 usAllUniqueId;
	HUINT16			 usCount, usSecIndex;
	HUINT32			 ulPos;
	svcCas_RawSecTable_t	*pstTable;


	if (pstPool == NULL)						{ return ERR_FAIL; }
	if (eSiType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstPool, eSiType);
	if (pstTable == NULL)			{ return ERR_FAIL; }

	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

		if ((usUniqueId == usAllUniqueId) || (usUniqueId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))
		{
			usSecIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
			if (usSecIndex >= pstPool->usMaxNumRawSec)					{ continue; }

			pucRaw = svc_cas_RawGetRawSection (pstPool, (HUINT32)usSecIndex);
			if (pucRaw == NULL)			{ continue; }

			if (bUseExtId == TRUE)
			{
				if (svc_cas_RawGetExtIdFromRawData (pucRaw) != usExtId)
				{
					continue;
				}
			}

			ulPos = (HUINT32)get12bit (&(pucRaw[1]));
			if (pulCrc32)				{ *pulCrc32	= (HUINT32)get32bit(&(pucRaw[3]) + ulPos - 4); }
			if (pucVer)					{ *pucVer	= (HUINT8)((pucRaw[5] & 0x3e) >> 1); }

			return ERR_OK;
		}

	}

	return ERR_FAIL;
}




#if defined(CONFIG_DEBUG)

char *svc_cas_RawgetSiTypeStr(HUINT32 ulSiType)
{
	switch(ulSiType)
	{
		case eSI_TableType_PAT:
			return "eSI_TableType_PAT";
		case eSI_TableType_CAT:
			return "eSI_TableType_CAT";
		case eSI_TableType_PMT:
			return "eSI_TableType_PMT";
		case eSI_TableType_NIT_ACT:
			return "eSI_TableType_NIT_ACT";
		case eSI_TableType_NIT_OTH:
			return "eSI_TableType_NIT_OTH";
		case eSI_TableType_SDT_ACT:
			return "eSI_TableType_SDT_ACT";
		case eSI_TableType_SDT_OTH:
			return "eSI_TableType_SDT_OTH";
		case eSI_TableType_BAT:
			return "eSI_TableType_BAT";
		case eSI_TableType_TDT:
			return "eSI_TableType_TDT";
		case eSI_TableType_TOT:
			return "eSI_TableType_TOT";
		case eSI_TableType_DSI:
			return "eSI_TableType_DSI";
		case eSI_TableType_DDB:
			return "eSI_TableType_DDB";
		case eSI_TableType_EIT_ACTPF:
			return "eSI_TableType_EIT_ACTPF";
		case eSI_TableType_EIT_OTHPF:
			return "eSI_TableType_EIT_OTHPF";
		case eSI_TableType_EIT_ACTSC:
			return "eSI_TableType_EIT_ACTSC";
		case eSI_TableType_EIT_OTHSC:
			return "eSI_TableType_EIT_OTHSC";
		case eSI_TableType_UNT:
			return "eSI_TableType_UNT";
		case eSI_TableType_CIT:
			return "eSI_TableType_CIT";
		case eSI_TableType_CPT:
			return "eSI_TableType_CPT";
		case eSI_TableType_SGT:
			return "eSI_TableType_SGT";
		case eSI_TableType_AIT:
			return "eSI_TableType_AIT";
		case eSI_TableType_PMT_N:
			return "eSI_TableType_PMT_N";
		case eSI_TableType_POSTCODE_TREE:
			return "eSI_TableType_POSTCODE_TREE";
		case eSI_TableType_POSTCODE_REGION:
			return "eSI_TableType_POSTCODE_REGION";
		case eSI_TableType_ICT:
			return "eSI_TableType_ICT";
		case MAX_SI_TABLE_TYPE:
			return "MAX_SI_TABLE_TYPE";
		default:
			return "Undefined Type";
			break;
	}
}


void	svc_cas_RawPrintRawPool(svcCas_RawPool_t *pstPool)
{
/*
	HINT32		i, j, k, nFreeCnt, nTotalSecCnt;
	HUINT16		usSecIdx;
	HUINT8		*p;
*/
	HUINT16			 usCount, usSecIdx;
	HUINT32			 ulSiType, ulRawIdx;
	HUINT32			 ulTotalSecNum;
	HUINT8			*pucRaw;
	svcCas_RawSecTable_t	*pstTable;

	if (pstPool == NULL)			{ return; }

	ulTotalSecNum = 0;

	HLIB_CMD_Printf("\n##############################################################################\n");
	HLIB_CMD_Printf("\t\tPRINT RAW_POOL : [%s]\n", pstPool->szPoolName);
	HLIB_CMD_Printf("-----------------------------------------------------------------------------\n");

	for (ulSiType = 0; ulSiType < MAX_SI_TABLE_TYPE; ulSiType++)
	{
		pstTable = svc_cas_RawGetRawTable (pstPool, ulSiType);
		if (pstTable == NULL)		{ continue; }

		HLIB_CMD_Printf("\t==[%02d][%s] SI_TABLE(sec_cnt:%d/%d, insert_pos:%d)\n", ulSiType,svc_cas_RawgetSiTypeStr(ulSiType),
					pstTable->usSecNum,
					pstTable->usMaxSecNum,
					pstTable->usInsertPos);

		for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
		{
			if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

			usSecIdx = MACRO_GET_INDEX_FROM_IDXHANDLE (pstTable->phIdx[usCount]);
			HLIB_CMD_Printf("\t\t(0x%04X, 0x%04X)", MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]), usSecIdx);

			if (usSecIdx >= pstPool->usMaxNumRawSec)						{ continue; }
			pucRaw = pstPool->ppucRawSec[usSecIdx];

			if (pucRaw != NULL)
			{
				for (ulRawIdx = 0; ulRawIdx < 8; ulRawIdx++)
				{
					HLIB_CMD_Printf(" %02X", *(pucRaw + ulRawIdx));
				}

				HLIB_CMD_Printf(" - addr:0x%08x\n", (HUINT32)pucRaw);

				ulTotalSecNum++;
			}
			else
			{
				HLIB_CMD_Printf(" NULL Section...\n");
			}
		}

		HLIB_CMD_Printf("-----------------------------------------------------------------------------\n");
	}

	HLIB_CMD_Printf("## TOTAL_SECTION_COUNT : %d/%d\n", ulTotalSecNum, pstPool->usMaxNumRawSec);
	HLIB_CMD_Printf("-----------------------------------------------------------------------------\n");
}
#endif


#define ___________________________________________________________

svcCas_RawSecTable_t *svc_cas_RawGetRawTable(svcCas_RawPool_t *pstPool, SiTableType_e eTableType)
{
	if (pstPool == NULL)					{ return NULL; }
	if (eTableType >= MAX_SI_TABLE_TYPE)	{ return NULL; }

	return &(pstPool->astSiTable[eTableType]);
}

HUINT8 *svc_cas_RawGetRawSection(svcCas_RawPool_t *pstPool, HUINT32 ulSecIdx)
{
	if (pstPool == NULL)									{ return NULL; }
	if (pstPool->usMaxNumRawSec <= (HUINT16)ulSecIdx)		{ return NULL; }

	return pstPool->ppucRawSec[ulSecIdx];
}

HUINT8 **svc_cas_RawGetRawArrays(svcCas_RawPool_t *pstPool,
										HUINT16 usTsUniqId,
										SiTableType_e eSiType,
										HBOOL bUseExtId,
										HUINT16 usExtId,
										HUINT32 *pulArrayLen)
{
	return svc_cas_RawGetRawArraysInteral (pstPool, usTsUniqId, eSiType, bUseExtId, usExtId, pulArrayLen);
}

HERROR svc_cas_RawFreeRawArrays(HUINT8 **ppucRawArrays)
{
	if (ppucRawArrays == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }

	// raw array에 연결된 Raw Data는 RawPool에서 pointer만 가져온 것이기 때문에 raw data들까지 free해서는 안 된다.
	OxCAS_Free (ppucRawArrays);
	return ERR_CAS_SI_RESULT_OK;
}

HUINT16 svc_cas_RawGetExtId(svcCas_RawPool_t *pstPool, HUINT16 usSecIdx)
{
	if (pstPool == NULL)							{ return 0; }
	if (pstPool->usMaxNumRawSec <= usSecIdx)		{ return 0; }

	return svc_cas_RawGetExtIdFromRawData (pstPool->ppucRawSec[usSecIdx]);
}

HUINT16 svc_cas_RawGetExtIdFromRawData(HUINT8 *pucRaw)
{
	if (pucRaw != NULL)
	{
		return get16bit(pucRaw+3);
	}

	return 0;
}

#if 0
HERROR svc_cas_RawCheckRawSamenessInReceivingTable(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw)
{
  	return svc_cas_RawCheckRawSamenessInReceivingTable(pstPool, usTsUniqId, eSiType, usExtId, pucRaw);
}
#endif

HERROR svc_cas_RawAddRawSecToReceivingTable(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw)
{
	return svc_cas_RawAddRawToReceivingTable(pstPool, usTsUniqId, eSiType, usExtId, pucRaw);
}

#if 0
HERROR svc_cas_RawIsMultiSecFinishedInReceivingTable(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId)
{
	return svc_cas_RawIsMultiSecFinishedInReceivingTable(pstPool, usTsUniqId, eSiType, usExtId);
}

HUINT8 **svc_cas_RawGetRawArraysFromReceivingTable(svcCas_RawPool_t *pstPool,
																	HUINT16 usTsUniqId,
																	SiTableType_e eSiType,
																	HUINT16 usExtId,
																	HUINT32	*pulArrayLen)
{
	return svc_cas_RawGetRawArraysFromReceivingTable (pstPool, usTsUniqId, eSiType, usExtId, pulArrayLen);
}
#endif

HERROR svc_cas_RawFreeReceivingTables(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, SiTableType_e eSiType)
{
	svcCas_RawSecTable_t		*pstSecTable;

	if (pstPool == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (eSiType >= MAX_SI_TABLE_TYPE)			{ return ERR_CAS_SI_SOURCE_INVALID; }

	pstSecTable = &(pstPool->astSiTable[eSiType]);

	svc_cas_RawFreeAllReceivingRawTableInSecTable (pstSecTable, FALSE);
	return ERR_CAS_SI_RESULT_OK;
}

#define ___________________________________________________________

STATIC HERROR svc_cas_RawPutSection(svcCas_RawPool_t *pstPool, HUINT8 *pucRawSec, HUINT16 *pusIndex)
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

	ppucNewRawSec = (HUINT8 **)OxCAS_Malloc (sizeof(HUINT8 *) * ulNewSize);
	if (ppucNewRawSec == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(ppucNewRawSec, 0, sizeof(HUINT8 *) * ulNewSize);
	if (pstPool->ppucRawSec)
	{
		memcpy (ppucNewRawSec, pstPool->ppucRawSec, sizeof(HUINT8 *) * pstPool->usMaxNumRawSec);
		OxCAS_Free (pstPool->ppucRawSec);
	}

	pstPool->usMaxNumRawSec		 = (HUINT16)ulNewSize;
	pstPool->ppucRawSec			 = ppucNewRawSec;
	pstPool->ppucRawSec[usCount] = pucRawSec;
	pstPool->usLastEmptySecIdx	 = usCount;

	if (pusIndex)			{ *pusIndex = usCount; }
	return ERR_OK;
}

STATIC HERROR svc_cas_RawFreeSectionBySectionIdx(svcCas_RawPool_t *pstPool, HUINT16 usIndex, HBOOL bFreeRawData)
{
	if (pstPool == NULL)						{ return ERR_FAIL; }
	if (pstPool->usMaxNumRawSec <= usIndex)		{ return ERR_FAIL; }

	if (pstPool->ppucRawSec)
	{
		if (pstPool->ppucRawSec[usIndex])
		{
			if (bFreeRawData)
			{
				OxCAS_Free (pstPool->ppucRawSec[usIndex]);
			}
			pstPool->ppucRawSec[usIndex] = NULL;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC void svc_cas_RawArrangeIndexHandle (svcCas_RawSecTable_t *pstTable)
{
	HUINT16		 usOldIndex, usNewIndex;

	usOldIndex = pstTable->usStartPos;
	usNewIndex = 0;
	while (usOldIndex < pstTable->usInsertPos)
	{
		if (pstTable->phIdx[usOldIndex] == INVALID_INDEX_HANDLE)
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
		pstTable->phIdx[usNewIndex] = INVALID_INDEX_HANDLE;
		usNewIndex++;
	}

	return;
}

STATIC HERROR svc_cas_RawExtendIndexHandleBuffer(svcCas_RawSecTable_t *pstTable)
{
	HUINT32			 ulCount, ulNewMaxNum;
	HUINT32			*phNewIdx;

	if (pstTable->usMaxSecNum == 0xFFFF)
	{
		HxLOG_Print ("[svc_cas_RawExtendIndexHandleBuffer] Maximum Buffer : It cannot be extended\n");
		return ERR_FAIL;
	}

	ulNewMaxNum = (HUINT32)pstTable->usMaxSecNum + MAX_INDEX_HANDLE_NUM;
	if (ulNewMaxNum >= 0xFFFF)			{ ulNewMaxNum = 0xFFFF; }


	phNewIdx = (HUINT32 *)OxCAS_Malloc (ulNewMaxNum * sizeof(HUINT32));
	if (phNewIdx == NULL)
	{
		HxLOG_Print ("[svc_cas_RawExtendIndexHandleBuffer] Mem allocation fail\n");
		return ERR_FAIL;
	}

	if (pstTable->phIdx && pstTable->usMaxSecNum)
	{
		memcpy (phNewIdx, pstTable->phIdx, pstTable->usMaxSecNum * sizeof(HUINT32));
		OxCAS_Free (pstTable->phIdx);
	}

	for (ulCount = (HUINT32)pstTable->usMaxSecNum; ulCount < ulNewMaxNum; ulCount++)
	{
		phNewIdx[ulCount] = INVALID_INDEX_HANDLE;
	}

	pstTable->phIdx = phNewIdx;
	pstTable->usInsertPos = pstTable->usMaxSecNum;
	pstTable->usMaxSecNum = (HUINT16)ulNewMaxNum;

	return ERR_OK;
}

STATIC HUINT8 svc_cas_RawGetExtIdType (HUINT32 eSiTableType)
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
			return EXTID_USE;

		case eSI_TableType_PAT:
		case eSI_TableType_CAT:
		case eSI_TableType_TOT:
		case eSI_TableType_TDT:
			return EXTID_UNUSE;

		default:
			/* private section의 경우는 section_syntax_indicator로 판단하자. */
			return EXTID_PRIVATE;
	}
}

STATIC HUINT32 svc_cas_RawGetSectionNumAndListInternal(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusCnt, HUINT16 *pusSecIdxList)
{
	HUINT16			 usSecIdx, usSecNum;
	HUINT16			 usCount;
	svcCas_RawSecTable_t	*pstSecTable;

	if (pstPool == NULL)							{ return ERR_FAIL; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	pstSecTable = svc_cas_RawGetRawTable (pstPool, eSiTableType);
	usSecNum = 0;
	for (usCount = pstSecTable->usStartPos; usCount < pstSecTable->usInsertPos; usCount++)
	{
		if (pstSecTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }
		if (usUniqueId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstSecTable->phIdx[usCount]))		{ continue; }

		usSecIdx = MACRO_GET_INDEX_FROM_IDXHANDLE(pstSecTable->phIdx[usCount]);
		if (usSecIdx >= MAX_SI_RAWSEC_NUM)
		{
			HxLOG_Print("[svc_cas_RawGetSectionNumAndListInternal] invalid section index:%d\n", usSecIdx);
			continue;
		}

		if (pstPool->ppucRawSec[usSecIdx] == NULL)
		{
			HxLOG_Print("[svc_cas_RawGetSectionNumAndListInternal] abnormal.. section(%d) is null..\n", usSecIdx);
			continue;
		}

		if (pusSecIdxList)					{ pusSecIdxList[usSecNum] = usSecIdx; }
		usSecNum++;
	}

	if (pusCnt)			{ *pusCnt = usSecNum; }

	return ERR_OK;
}

STATIC void svc_cas_RawRecalculateIndexStartPointInSecTable (svcCas_RawSecTable_t *pstSecTable)
{
	HUINT16		usIndex;

	if (pstSecTable == NULL)					{ return; }

	for (usIndex = pstSecTable->usStartPos; usIndex < pstSecTable->usInsertPos; usIndex++)
	{
		if (pstSecTable->phIdx[usIndex] != INVALID_INDEX_HANDLE)
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

STATIC HUINT8 **svc_cas_RawGetRawArraysInteral (svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													SiTableType_e eSiType,
													HBOOL bUseExtId,
													HUINT16 usExtId,
													HUINT32	*pulArrayLen)
{
	HUINT16			 usAllUniqueId;
	HUINT16			 usSecExtId;
	HUINT16			 usCount, usTotalSecNum;
	HUINT32			 ulSecIdx;
	HUINT8			 *pucRaw;
	HUINT8			**ppucRawArray;
	svcCas_RawSecTable_t	 *pstSecTable;

	if (pstPool == NULL)				{ return NULL; }
	if (pulArrayLen == NULL)			{ return NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstSecTable = &(pstPool->astSiTable[eSiType]);

	usCount = 0;
	usTotalSecNum = 0;

	if (pstSecTable->usStartPos >= pstSecTable->usInsertPos)
	{
		return NULL;
	}

	ppucRawArray = (HUINT8 **)OxCAS_Malloc (sizeof(HUINT8 *) * (pstSecTable->usInsertPos - pstSecTable->usStartPos));
	if (ppucRawArray == NULL)
	{
		return NULL;
	}

	// Ext ID를 사용하려 해도 사용할 수 없는 SI Table들이 있다.
	if (bUseExtId == TRUE)
	{
		if (svc_cas_RawGetExtIdType (eSiType) == EXTID_UNUSE)
		{
			bUseExtId = FALSE;
		}
	}


	for (usCount = pstSecTable->usStartPos; usCount < pstSecTable->usInsertPos; usCount++)
	{
		if (pstSecTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstSecTable->phIdx[usCount])))				{ continue; }

		// Get Section Index
		ulSecIdx = (HUINT32)MACRO_GET_INDEX_FROM_IDXHANDLE(pstSecTable->phIdx[usCount]);
		pucRaw = svc_cas_RawGetRawSection (pstPool, ulSecIdx);
		if (pucRaw == NULL)				{ continue; }

		// Get EXT ID : offset 3 ~ 4
		if (bUseExtId == TRUE)
		{
			usSecExtId = get16bit (pucRaw + 3);
			if (usExtId != usSecExtId)		{ continue; }
		}

		ppucRawArray[usTotalSecNum] = pucRaw;
		usTotalSecNum++;
	}

	if (usTotalSecNum == 0)
	{
		OxCAS_Free (ppucRawArray);
		return NULL;
	}

	*pulArrayLen = (HUINT32)usTotalSecNum;
	return ppucRawArray;
}


// ===== for Receiving Raw Section Table :

STATIC void svc_cas_RawFreeAllReceivingRawTableInSecTable (svcCas_RawSecTable_t *pstSecTable, HBOOL bRealFree)
{
	HUINT32			 ulCntRecv;
	svcCas_RawReceiving_t	*pstReceiving, *pstRecvToFree;

	if (pstSecTable)
	{
		if (pstSecTable->pstReceivingTable)
		{
			pstReceiving = pstSecTable->pstReceivingTable;
			pstSecTable->pstReceivingTable = NULL;

			while (pstReceiving)
			{
				for (ulCntRecv = 0; ulCntRecv < NUM_SEC_FOR_RECEIVING_TABLE; ulCntRecv++)
				{
					if (pstReceiving->apucRawSec[ulCntRecv] != NULL)
					{
						OxCAS_Free (pstReceiving->apucRawSec[ulCntRecv]);
					}
				}

				pstRecvToFree = pstReceiving;
				pstReceiving = pstReceiving->pstNext;

				if (bRealFree == TRUE)
				{
					OxCAS_Free (pstRecvToFree);
				}
				else
				{
					HxSTD_MemSet(pstRecvToFree, 0, sizeof(svcCas_RawReceiving_t));
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
					for (ulCntRecv = 0; ulCntRecv < NUM_SEC_FOR_RECEIVING_TABLE; ulCntRecv++)
					{
						if (pstReceiving->apucRawSec[ulCntRecv] != NULL)
						{
							OxCAS_Free (pstReceiving->apucRawSec[ulCntRecv]);
						}
					}

					pstRecvToFree = pstReceiving;
					pstReceiving = pstReceiving->pstNext;

					OxCAS_Free (pstRecvToFree);
				}
			}
		}
	}
}

STATIC void svc_cas_RawFreeReceivingRawTableInSecTable (svcCas_RawSecTable_t *pstSecTable, HUINT16 usExtId)
{
	HUINT32			 ulCntRecv;
	svcCas_RawReceiving_t	*pstReceiving, *pstRecvToFree;
	svcCas_RawReceiving_t	*pstRecvFirst, *pstRecvLast;

	if (pstSecTable)
	{
		if (pstSecTable->pstReceivingTable)
		{
			pstReceiving = pstSecTable->pstReceivingTable;
			pstRecvFirst = NULL;
			pstRecvLast	 = NULL;

			while (pstReceiving)
			{
				if (pstReceiving->usExtId1 == usExtId)
				// Remove it !
				{
					for (ulCntRecv = 0; ulCntRecv < NUM_SEC_FOR_RECEIVING_TABLE; ulCntRecv++)
					{
						if (pstReceiving->apucRawSec[ulCntRecv] != NULL)
						{
							OxCAS_Free (pstReceiving->apucRawSec[ulCntRecv]);
						}
					}

					pstRecvToFree = pstReceiving;
					pstReceiving = pstReceiving->pstNext;

					HxSTD_MemSet(pstRecvToFree, 0, sizeof(svcCas_RawReceiving_t));
					pstRecvToFree->pstNext = pstSecTable->pstFreeRcvTable;
					pstSecTable->pstFreeRcvTable = pstRecvToFree;
				}
				else
				// Remain it !
				{
					if (pstRecvFirst == NULL)
					{
						pstRecvFirst = pstReceiving;
						pstRecvLast	 = pstReceiving;
					}
					else
					{
						pstRecvLast->pstNext = pstReceiving;
						pstRecvLast = pstReceiving;
					}

					pstReceiving = pstReceiving->pstNext;
					pstRecvLast->pstNext = NULL;
				}
			}

			pstSecTable->pstReceivingTable = pstRecvFirst;
		}
	}
}

STATIC svcCas_RawReceiving_t *svc_cas_RawAllocReceivingRawTable (svcCas_RawSecTable_t *pstSecTable)
{
	svcCas_RawReceiving_t	*pstTable;

	pstTable = NULL;

	if (pstSecTable != NULL)
	{
		if (pstSecTable->pstFreeRcvTable != NULL)
		{
			pstTable = pstSecTable->pstFreeRcvTable;
			pstSecTable->pstFreeRcvTable = pstTable->pstNext;
		}
	}

	if (pstTable == NULL)
	{
		pstTable = (svcCas_RawReceiving_t *)OxCAS_Malloc (sizeof(svcCas_RawReceiving_t));
		if (pstTable == NULL)
		{
			return NULL;
		}
	}

	HxSTD_MemSet(pstTable, 0, sizeof(svcCas_RawReceiving_t));
	return pstTable;
}

STATIC HERROR svc_cas_RawFindReceivingTableWithSecNum (svcCas_RawPool_t *pstPool,
																		HUINT16 usTsUniqId,
																		SiTableType_e eSiType,
																		HUINT16 usExtId,
																		HUINT8 ucSecNum,
																		svcCas_RawReceiving_t **ppstRcvTable)
{
	HUINT16			 usAllUniqueId;
	svcCas_RawSecTable_t	*pstSecTable;
	svcCas_RawReceiving_t	*pstRcvTable;

	if (pstPool == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppstRcvTable == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return ERR_CAS_SI_SOURCE_INVALID; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();

	pstSecTable = &(pstPool->astSiTable[eSiType]);
	pstRcvTable = pstSecTable->pstReceivingTable;

	while (pstRcvTable)
	{
		if (usTsUniqId == usAllUniqueId || usTsUniqId == pstRcvTable->usTsUniqId)
		{
			if (pstRcvTable->usExtId1 == usExtId)
			{
				if (pstRcvTable->ucStartSecNum <= ucSecNum && pstRcvTable->ucEndSecNum >= ucSecNum)
				{
					break;
				}
			}
		}

		pstRcvTable = pstRcvTable->pstNext;
	}

	*ppstRcvTable = pstRcvTable;
	return (pstRcvTable != NULL) ? ERR_CAS_SI_RESULT_OK : ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_RawMakeReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8  ucLastSecNum,
														HUINT8  ucVer)
{
	HUINT16			 usAllUniqueId;
	HUINT32			 ulTableIdx, ulTableNum;
	svcCas_RawSecTable_t	*pstSecTable;
	svcCas_RawReceiving_t	*pstRcvTable;
	svcCas_RawReceiving_t	*pstPrevRcvTable;

	if (pstPool == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return ERR_CAS_SI_SOURCE_INVALID; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();

	pstSecTable = &(pstPool->astSiTable[eSiType]);

	// Find a link position to the new receiving table, or check whether there is the same receiving table already.
	if (pstSecTable->pstReceivingTable == NULL)
	{
		pstPrevRcvTable = NULL;
	}
	else
	{
		pstPrevRcvTable = pstSecTable->pstReceivingTable;
		while (pstPrevRcvTable != NULL)
		{
			if (usTsUniqId == usAllUniqueId || usTsUniqId == pstPrevRcvTable->usTsUniqId)
			{
				if (pstPrevRcvTable->usExtId1 == usExtId)
				{
					return ERR_CAS_SI_RESULT_OK;
				}
			}

			if (pstPrevRcvTable->pstNext == NULL)			{ break; }

			pstPrevRcvTable = pstPrevRcvTable->pstNext;
		}
	}

	ulTableNum = ((HUINT32)ucLastSecNum / NUM_SEC_FOR_RECEIVING_TABLE) + 1;

	for (ulTableIdx = 0; ulTableIdx < ulTableNum; ulTableIdx++)
	{
		pstRcvTable = svc_cas_RawAllocReceivingRawTable (pstSecTable);
		if (pstRcvTable == NULL)
		{
			return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
		}

		pstRcvTable->usTsUniqId		= usTsUniqId;
		pstRcvTable->usExtId1		= usExtId;
		pstRcvTable->ucLastSecNum	= ucLastSecNum;
		pstRcvTable->ucVer			= ucVer;

		pstRcvTable->ucStartSecNum	= (HUINT8)(ulTableIdx * NUM_SEC_FOR_RECEIVING_TABLE);
		pstRcvTable->ucEndSecNum	= (ulTableIdx != ulTableNum - 1) ? pstRcvTable->ucStartSecNum + NUM_SEC_FOR_RECEIVING_TABLE - 1 :
																		ucLastSecNum;

		if (pstPrevRcvTable == NULL)
		{
			pstSecTable->pstReceivingTable = pstRcvTable;
		}
		else
		{
			pstPrevRcvTable->pstNext = pstRcvTable;
		}

		pstPrevRcvTable = pstRcvTable;
	}

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_RawCheckRawSamenessInReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw)
{
	HUINT8			 ucVer, ucSecNum, ucLastSecNum;
	HUINT8			 ucRealIdx;
	HUINT16 		 usSecLen, usOldSecLen;
	HUINT32 		 ulCrc32, ulOldCrc32;
	HUINT8			*pucOldRaw;
	svcCas_RawSecTable_t	*pstSecTable;
	svcCas_RawReceiving_t *pstRcvTable;
	HERROR			 hErr;

	if (pstPool == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return ERR_CAS_SI_SOURCE_INVALID; }

	pstSecTable = &(pstPool->astSiTable[eSiType]);

	ucVer		 = (pucRaw[5] >> 1) & 0x1F;
	ucSecNum	 = pucRaw[6];
	ucLastSecNum = pucRaw[7];
	usSecLen	 = get12bit (pucRaw + 1);
	ulCrc32 	 = get32bit (pucRaw + usSecLen - 1);

	hErr = svc_cas_RawFindReceivingTableWithSecNum (pstPool, usTsUniqId, eSiType, usExtId, ucSecNum, &pstRcvTable);
	if (hErr == ERR_CAS_SI_RESULT_OK && pstRcvTable != NULL)
	{
		if (pstRcvTable->ucVer == ucVer && pstRcvTable->ucLastSecNum == ucLastSecNum)
		{
			if (pstRcvTable->bSecCompleted == TRUE)
			{
				// SecNum = StartSecNum 인 경우에만 제대로 check 하고 그 외에는 동일하다고 정보를 보낸다.
				if (ucSecNum == pstRcvTable->ucStartSecNum)
				{
					if (pstRcvTable->ulCrc32 != ulCrc32)
					{
						return ERR_CAS_SI_RESULT_FAIL;
					}
				}

				return ERR_CAS_SI_ALREADY_EXIST;
			}

			// 처리가 덜 끝난 경우 : raw data가 남아 있으니 그것을 통해 비교하자.
			ucRealIdx = ucSecNum - pstRcvTable->ucStartSecNum;
			pucOldRaw = pstRcvTable->apucRawSec[ucRealIdx];
			if (pucOldRaw != NULL)
			{
				usOldSecLen  = get12bit (pucOldRaw + 1);
				ulOldCrc32 	 = get32bit (pucOldRaw + usSecLen - 1);
				if (ulCrc32 == ulOldCrc32)
				{
					return ERR_CAS_SI_ALREADY_EXIST;
				}
			}
		}
	}

	NOT_USED_PARAM(pstSecTable);
	NOT_USED_PARAM(usOldSecLen);

	// 비교 불가능 혹은 동일하지 않을 경우 :
	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_RawAddRawToReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw)
{
	HBOOL			 bRemakeTable;
	HUINT8			 ucVer, ucSecNum, ucLastSecNum;
	HUINT8			 ucRealIdx;
	HUINT16			 usSecLen, usOldSecLen;
	HUINT32			 ulCrc32, ulOldCrc32;
	HUINT8			*pucOldRaw;
	svcCas_RawSecTable_t	*pstSecTable;
	svcCas_RawReceiving_t	*pstRcvTable;
	HERROR			 hErr;

	if (pstPool == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return ERR_CAS_SI_SOURCE_INVALID; }

	pstSecTable = &(pstPool->astSiTable[eSiType]);

	ucVer		 = (pucRaw[5] >> 1) & 0x1F;
	ucSecNum	 = pucRaw[6];
	ucLastSecNum = pucRaw[7];
	usSecLen	 = get12bit (pucRaw + 1);
	ulCrc32 	 = get32bit (pucRaw + usSecLen - 1);

	bRemakeTable = TRUE;
	hErr = svc_cas_RawFindReceivingTableWithSecNum (pstPool, usTsUniqId, eSiType, usExtId, ucSecNum, &pstRcvTable);
	if (hErr == ERR_CAS_SI_RESULT_OK && pstRcvTable != NULL)
	{
		if (pstRcvTable->ucVer == ucVer && pstRcvTable->ucLastSecNum == ucLastSecNum)
		{
			if (pstRcvTable->bSecCompleted == FALSE)
			{
				bRemakeTable = FALSE;
			}
		}
	}

	if (bRemakeTable == TRUE)
	// Receiving Table shall be removed and remake.
	{
		svc_cas_RawFreeReceivingRawTableInSecTable (pstSecTable, usExtId);
		svc_cas_RawMakeReceivingTable (pstPool, usTsUniqId, eSiType, usExtId, ucLastSecNum, ucVer);
		hErr = svc_cas_RawFindReceivingTableWithSecNum (pstPool, usTsUniqId, eSiType, usExtId, ucSecNum, &pstRcvTable);
		if (hErr != ERR_CAS_SI_RESULT_OK && pstRcvTable == NULL)
		// 없거나 잘못되어 다시 만들었는데도 못 찾았다면, 에러 처리해야 한다.
		{
			return ERR_CAS_SI_RESULT_FAIL;
		}
	}

	// pstRcvTable : Raw Data 가 들어 갈 Receiving Table
	// 이 시점에서 pstRcvTable = NULL 일 수 없다. 하지만 안전 코드 로써 추가한다.
	// 그 외 값이 이상한 경우의 예외 처리도 추가한다.
	if (pstRcvTable == NULL)
	{
		return ERR_CAS_SI_RESULT_FAIL;
	}

	if (pstRcvTable->ucStartSecNum > ucSecNum || pstRcvTable->ucEndSecNum < ucSecNum)
	{
		return ERR_CAS_SI_RESULT_FAIL;
	}

	ucRealIdx = ucSecNum - pstRcvTable->ucStartSecNum;
	// 이미 동일한 section number 의 raw를 받았음. 중복 내지는 update 여부 체크한다.
	if (pstRcvTable->apucRawSec[ucRealIdx] != NULL)
	{
		pucOldRaw	 = pstRcvTable->apucRawSec[ucRealIdx];
		usOldSecLen  = get12bit (pucOldRaw + 1);
		ulOldCrc32	 = get32bit (pucOldRaw + usOldSecLen - 1);

		if (ulOldCrc32 == ulCrc32)
		// 동일한 raw section 이 들어 있으면 전혀 문제 없다.
		{
			return ERR_CAS_SI_ALREADY_EXIST;
		}
		else
		// 새 raw section이 들어 오고 내용이 달라짐.
		// 모든 것을 다시 지우고 다시 받아야 한다.
		{
			svc_cas_RawFreeReceivingRawTableInSecTable (pstSecTable, usExtId);
			svc_cas_RawMakeReceivingTable (pstPool, usTsUniqId, eSiType, usExtId, ucLastSecNum, ucVer);
			pstRcvTable = NULL;
			hErr = svc_cas_RawFindReceivingTableWithSecNum (pstPool, usTsUniqId, eSiType, usExtId, ucSecNum, &pstRcvTable);
			if (hErr != ERR_CAS_SI_RESULT_OK && pstRcvTable == NULL)
			// 없거나 잘못되어 다시 만들었는데도 못 찾았다면, 에러 처리해야 한다.
			{
				return ERR_CAS_SI_RESULT_FAIL;
			}
		}
	}

	pstRcvTable->apucRawSec[ucRealIdx] = pucRaw;
	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_RawIsMultiSecFinishedInReceivingTable  (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId)
{
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt, ulTotalSecCnt;
	HUINT32			 ulSecIdx, ulSecNum;
	svcCas_RawSecTable_t	*pstSecTable;
	svcCas_RawReceiving_t	*pstRcvTable;

	if (pstPool == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return ERR_CAS_SI_SOURCE_INVALID; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstSecTable = &(pstPool->astSiTable[eSiType]);
	pstRcvTable = pstSecTable->pstReceivingTable;

	ulTotalSecCnt = 0;
	ulSecCnt = 0;

	while (pstRcvTable)
	{
		if (usTsUniqId == usAllUniqueId || usTsUniqId == pstRcvTable->usTsUniqId)
		{
			if (pstRcvTable->usExtId1 == usExtId)
			{
				ulTotalSecCnt = (HUINT32)pstRcvTable->ucLastSecNum + 1;
				ulSecNum = (HUINT32)pstRcvTable->ucEndSecNum - (HUINT32)pstRcvTable->ucStartSecNum + 1;
				for (ulSecIdx = 0; ulSecIdx < ulSecNum; ulSecIdx ++)
				{
					if (pstRcvTable->apucRawSec[ulSecIdx] != NULL)
					{
						ulSecCnt ++;
					}
				}
			}
		}

		pstRcvTable = pstRcvTable->pstNext;
	}

	if (ulTotalSecCnt > 0 && ulSecCnt == ulTotalSecCnt)
	{
		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

HUINT8 **svc_cas_RawGetRawArraysFromReceivingTable(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT32	*pulArrayLen)
{
	HUINT16			 usAllUniqueId;
	HUINT16			 usSecLen;
	HUINT32			 ulSecCnt, ulTotalSecCnt;
	HUINT32			 ulSecIdx, ulSecNum;
	HUINT32			 ulCrc32;
	HUINT8			**ppucRawArray;
	svcCas_RawSecTable_t	*pstSecTable;
	svcCas_RawReceiving_t	*pstRcvTable;

	if (pstPool == NULL)				{ return NULL; }
	if (pulArrayLen == NULL)			{ return NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstSecTable = &(pstPool->astSiTable[eSiType]);
	pstRcvTable = pstSecTable->pstReceivingTable;

	ulTotalSecCnt = 0;
	ulSecCnt = 0;
	ppucRawArray = NULL;

	while (pstRcvTable)
	{
		if (usTsUniqId == usAllUniqueId || usTsUniqId == pstRcvTable->usTsUniqId)
		{
			if (pstRcvTable->usExtId1 == usExtId)
			{
				// Allocate the array at first.
				if (ppucRawArray == NULL)
				{
					ulTotalSecCnt = (HUINT32)pstRcvTable->ucLastSecNum + 1;
					ulSecCnt	  = 0;
					ppucRawArray = (HUINT8 **)OxCAS_Malloc (sizeof(HUINT8 *) * ulTotalSecCnt);
					if (ppucRawArray == NULL)			{ return NULL; }
					HxSTD_MemSet(ppucRawArray, 0, sizeof(HUINT8 *) * ulTotalSecCnt);
				}

				if (pstRcvTable->apucRawSec[0] != NULL)
				{
					// Set the complete flag and add the CRC32 value of the start section in the table
					usSecLen = get12bit (pstRcvTable->apucRawSec[0] + 1);
					ulCrc32	 = get32bit (pstRcvTable->apucRawSec[0] + usSecLen - 1);

					pstRcvTable->bSecCompleted	= TRUE;
					pstRcvTable->ulCrc32		= ulCrc32;

					ulSecNum = (HUINT32)pstRcvTable->ucEndSecNum - (HUINT32)pstRcvTable->ucStartSecNum + 1;
					for (ulSecIdx = 0; ulSecIdx < ulSecNum; ulSecIdx ++)
					{
						if (pstRcvTable->apucRawSec[ulSecIdx] != NULL)
						{
							ppucRawArray[ulSecCnt] = pstRcvTable->apucRawSec[ulSecIdx];
							pstRcvTable->apucRawSec[ulSecIdx] = NULL;
							ulSecCnt ++;
						}
					}
				}
			}
		}

		pstRcvTable = pstRcvTable->pstNext;
	}

	if (ulSecCnt > 0)
	{
		*pulArrayLen = ulSecCnt;
		return ppucRawArray;
	}

	if (ppucRawArray)
	{
		OxCAS_Free (ppucRawArray);
	}

	return NULL;
}

#define ___________________________________________________________
HERROR svc_cas_RawRegisterSectionMakeFunction (svcCas_RawPool_t *pstPool, svcCas_RawMakeSecCb_t pfSecMake)
{
	if (NULL == pstPool)					{ return ERR_FAIL; }

	pstPool->pfSectionMaker		= (void *)pfSecMake;
	return ERR_OK;
}

svcCas_RawMakeSecCb_t svc_cas_RawGetSectionMakeFunction (svcCas_RawPool_t *pstPool)
{
	if (NULL != pstPool)
	{
		return (svcCas_RawMakeSecCb_t)pstPool->pfSectionMaker;
	}

	return NULL;
}

#define ________________________NOT_USED___________________________________

#if 0
STATIC HERROR svc_cas_RawFindReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId1,
														HUINT16 usExtId2,
														svcCas_RawReceiving_t **ppstRcvTable)
{
	HUINT16			 usAllUniqueId;
	svcCas_RawSecTable_t	*pstSecTable;
	svcCas_RawReceiving_t	*pstRcvTable;
	HERROR			 hErr;

	if (pstPool == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppstRcvTable == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	if (eSiType >= MAX_SI_TABLE_TYPE)	{ return ERR_CAS_SI_SOURCE_INVALID; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();

	pstSecTable = &(pstPool->astSiTable[eSiType]);
	pstRcvTable = pstSecTable->pstReceivingTable;

	while (pstRcvTable)
	{
		if (usTsUniqId == usAllUniqueId || usTsUniqId == pstRcvTable->usTsUniqId)
		{
			if (pstRcvTable->usExtId1 == usExtId1 && pstRcvTable->usExtId2 == usExtId2)
			{
				break;
			}
		}

		pstRcvTable = pstRcvTable->pstNext;
	}

	*ppstRcvTable = pstRcvTable;
	return (pstRcvTable != NULL) ? ERR_CAS_SI_RESULT_OK : ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_RawAddRawSecToReceivingTable(svcCas_RawReceiving_t *pstRcvTable, HUINT8 *pucRaw)
{
	HUINT8			 ucVer, ucSecNum, ucLastSecNum;
	HUINT16			 usSecLen, usOldSecLen;
	HUINT32			 ulCrc32, ulOldCrc32;
	HUINT32			 ulIdx, ulNum;
	HUINT8			*pucOldRaw;
	HERROR			 hErr;

	if (pstRcvTable == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pucRaw == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }

	ucVer		 = (pucRaw[5] >> 1) & 0x1F;
	ucSecNum	 = pucRaw[6];
	ucLastSecNum = pucRaw[7];

	if (pstRcvTable->ucLastSecNum != ucLastSecNum || pstRcvTable->ucVer != ucVer)
	{
		// Table changed during the receiving : remove all and receive them from the first
		ulNum = (HUINT32)pstRcvTable->ucLastSecNum + 1;
		for (ulIdx = 0; ulIdx < ulNum; ulIdx++)
		{
			if (pstRcvTable->apucRawSec[ulIdx] != NULL)
			{
				OxCAS_Free (pstRcvTable->apucRawSec[ulIdx]);
			}
		}

		pstRcvTable->ucLastSecNum = ucLastSecNum;
		pstRcvTable->ucVer		  = ucVer;
		HxSTD_MemSet(pstRcvTable->apucRawSec, 0, sizeof(HUINT8 *) * NUM_SEC_FOR_RECEIVING_TABLE);
	}

	if (pstRcvTable->apucRawSec[ucSecNum] != NULL)
	{
		pucOldRaw = pstRcvTable->apucRawSec[ucSecNum];

		usSecLen	 = get12bit (pucRaw + 1);
		ulCrc32		 = get32bit (pucRaw + usSecLen - 1);

		usOldSecLen	 = get12bit (pucOldRaw + 1);
		ulOldCrc32	 = get32bit (pucOldRaw + usOldSecLen - 1);

		if (ulCrc32 != ulOldCrc32)
		// Something changed: Receive all sections in one table.
		{
			// Table changed during the receiving : remove all and receive them from the first
			ulNum = (HUINT32)pstRcvTable->ucLastSecNum + 1;
			for (ulIdx = 0; ulIdx < ulNum; ulIdx++)
			{
				if (pstRcvTable->apucRawSec[ulIdx] != NULL)
				{
					OxCAS_Free (pstRcvTable->apucRawSec[ulIdx]);
				}
			}

			pstRcvTable->ucLastSecNum = ucLastSecNum;
			pstRcvTable->ucVer		  = ucVer;
			HxSTD_MemSet(pstRcvTable->apucRawSec, 0, sizeof(HUINT8 *) * NUM_SEC_FOR_RECEIVING_TABLE);
		}
		else
		{
			// Nothing changed, we received the same thing.
			return ERR_CAS_SI_SOURCE_INVALID;
		}
	}

	// Anyway, the slot is empty in the phase.
	pstRcvTable->apucRawSec[ucSecNum] = pucRaw;
	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_RawIsMultiSecFinishedInReceivingTable_Old (svcCas_RawReceiving_t *pstRcvTable)
{
	HUINT32		 ulIdx, ulNum;

	if (pstRcvTable == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }

	ulNum = (HUINT32)pstRcvTable->ucLastSecNum + 1;
	for (ulIdx = 0; ulIdx < ulNum; ulIdx++)
	{
		if (pstRcvTable->apucRawSec[ulIdx] == NULL)
		{
			return ERR_CAS_SI_PARAM_NULL;
		}
	}

	return ERR_CAS_SI_RESULT_OK;
}
#endif


/*********************** End of File ******************************/
