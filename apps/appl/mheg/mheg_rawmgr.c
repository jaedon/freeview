/**
	@file     mheg_rawmgr.c
	@brief    mheg__rawmgr.c (MHEG Service)

	Description:  \n
	Module: MW/ \n
	Remarks :\n

	Copyright (c) 2008 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <apk.h>
#include <silib.h>
#include <vkernel.h>

#include "mheg_int.h"
#include "mheg_rawmgr.h"
#include "mheg_si.h"
#include "mheg_param.h"
#include "mheg_sef.h"

#include "itk_redkey.h"
#include "itk_service.h"
#include "port_itk_service.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
//semaphore
STATIC HULONG						s_ulMhegRawMgrSemId;
STATIC HULONG						s_ulMhegRawMakeListSemId;

STATIC MhegRawPool					*s_pstMhegSiRawDataPool[eMHEG_SI_RAW_DATA_TYPE_MAX];
STATIC MHEG_SiRawDataList_t			*s_pstMHEGSiRawList[eMHEG_SI_RAW_DATA_TYPE_MAX];

#if defined(CONFIG_DEBUG)
//#define _MHEGRAWMAKE_SEM_DEBUG_
//#define _MHEGMGR_SEM_DEBUG_
#endif
#ifdef _MHEGMGR_SEM_DEBUG_
#define	ENTER_CRITICAL_MHEGRAW	{HxLOG_Debug("++++ ENTER_MHEG_MGR_CRITICAL(%d)\n", __LINE__);VK_SEM_Get(s_ulMhegRawMgrSemId);}
#define	LEAVE_CRITICAL_MHEGRAW	{VK_SEM_Release(s_ulMhegRawMgrSemId);HxLOG_Debug("---- LEAVE_MHEG_MGR_CRITICAL(%d)\n", __LINE__);}
#else
#define	ENTER_CRITICAL_MHEGRAW		VK_SEM_Get(s_ulMhegRawMgrSemId)
#define	LEAVE_CRITICAL_MHEGRAW		VK_SEM_Release(s_ulMhegRawMgrSemId)
#endif

#ifdef _MHEGRAWMAKE_SEM_DEBUG_
#define	ENTER_CRITICAL_MHEG_RAWLISTS	{HxLOG_Debug("++++ ENTER_MHEG_RAWLIST_CRITICAL(%d)\n", __LINE__);VK_SEM_Get(s_ulMhegRawMakeListSemId);}
#define	LEAVE_CRITICAL_MHEG_RAWLISTS	{VK_SEM_Release(s_ulMhegRawMakeListSemId);HxLOG_Debug("---- LEAVE_MHEG_RAWLIST_CRITICAL(%d)\n", __LINE__);}
#else
#define	ENTER_CRITICAL_MHEG_RAWLISTS		VK_SEM_Get(s_ulMhegRawMakeListSemId)
#define	LEAVE_CRITICAL_MHEG_RAWLISTS		VK_SEM_Release(s_ulMhegRawMakeListSemId)
#endif

/* Audio Priority. */
#define MHEG_AUDIO_PRIORITY_NONE				0x00000000			/** < Nothing! */
#define MHEG_AUDIO_PRIORITY_NORMAL			0x00000001			/** < Normal */
#define MHEG_AUDIO_PRIORITY_UNDEFINE_LANG		0x00000002			/** < For Audio, Undefine Language. */
#define MHEG_AUDIO_PRIORITY_MENU_LANG			0x00000004			/** < Menu Language */
#define MHEG_AUDIO_PRIORITY_AUDIO_LANG		0x00000008			/** < For Audio */
#define MHEG_AUDIO_PRIORITY_DOLBY				0x00000010			/** < For Audio [Dolby or Dolby Plus] */
#define MHEG_AUDIO_PRIORITY_USER				0x00000020			/** < Opt select */

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define	___MHEG_RAW_DATA____

static void local_MHEG_RAW_FreeAllReceivingRawTableInSecTable (MhegRawSecTable *pstSecTable, HBOOL bRealFree)
{
	HUINT32			 	ulCntRecv;
	MhegReceivingRaw	*pstReceiving, *pstRecvToFree;

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
						HLIB_STD_MemFree (pstReceiving->apucRawSec[ulCntRecv]);
					}
				}

				pstRecvToFree = pstReceiving;
				pstReceiving = pstReceiving->pstNext;

				if (bRealFree == TRUE)
				{
					HLIB_STD_MemFree (pstRecvToFree);
				}
				else
				{
					HxSTD_memset (pstRecvToFree, 0, sizeof(MhegReceivingRaw));
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
							HLIB_STD_MemFree (pstReceiving->apucRawSec[ulCntRecv]);
						}
					}

					pstRecvToFree = pstReceiving;
					pstReceiving = pstReceiving->pstNext;

					HLIB_STD_MemFree (pstRecvToFree);
				}
			}
		}
	}
}

static HERROR local_MHEG_RAW_PutSection (MhegRawPool *pstPool, HUINT8 *pucRawSec, HUINT16 *pusIndex)
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

	ppucNewRawSec = (HUINT8 **)HLIB_STD_MemAlloc (sizeof(HUINT8 *) * ulNewSize);
	if (ppucNewRawSec == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset (ppucNewRawSec, 0, sizeof(HUINT8 *) * ulNewSize);
	if (pstPool->ppucRawSec)
	{
		HxSTD_memcpy (ppucNewRawSec, pstPool->ppucRawSec, sizeof(HUINT8 *) * pstPool->usMaxNumRawSec);
		HLIB_STD_MemFree (pstPool->ppucRawSec);
	}

	pstPool->usMaxNumRawSec		 = (HUINT16)ulNewSize;
	pstPool->ppucRawSec			 = ppucNewRawSec;
	pstPool->ppucRawSec[usCount] = pucRawSec;
	pstPool->usLastEmptySecIdx	 = usCount;

	if (pusIndex)			{ *pusIndex = usCount; }
	return ERR_OK;
}

static void local_MHEG_RAW_ArrangeIndexHandle (MhegRawSecTable *pstTable)
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

static HERROR local_MHEG_RAW_ExtendIndexHandleBuffer(MhegRawSecTable *pstTable)
{
	HUINT32			 ulCount, ulNewMaxNum;
	HUINT32			*phNewIdx;

	if (pstTable->usMaxSecNum == 0xFFFF)
	{
		HxLOG_Error("[local_mwcraw_ExtendIndexHandleBuffer] Maximum Buffer : It cannot be extended\n");
		return ERR_FAIL;
	}

	ulNewMaxNum = (HUINT32)pstTable->usMaxSecNum + MAX_INDEX_HANDLE_NUM;
	if (ulNewMaxNum >= 0xFFFF)			{ ulNewMaxNum = 0xFFFF; }


	phNewIdx = (HUINT32 *)HLIB_STD_MemAlloc (ulNewMaxNum * sizeof(HUINT32));
	if (phNewIdx == NULL)
	{
		HxLOG_Error("[local_mwcraw_ExtendIndexHandleBuffer] Mem allocation fail\n");
		return ERR_FAIL;
	}

	if (pstTable->phIdx && pstTable->usMaxSecNum)
	{
		HxSTD_memcpy (phNewIdx, pstTable->phIdx, pstTable->usMaxSecNum * sizeof(HUINT32));
		HLIB_STD_MemFree (pstTable->phIdx);
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

static HERROR local_MHEG_RAW_FreeSection (MhegRawPool *pstPool, HUINT16 usIndex, HBOOL bFreeRawData)
{
	if (pstPool == NULL)						{ return ERR_FAIL; }
	if (pstPool->usMaxNumRawSec <= usIndex)		{ return ERR_FAIL; }

	if (pstPool->ppucRawSec)
	{
		if (pstPool->ppucRawSec[usIndex])
		{
			if (bFreeRawData)
			{
				HLIB_STD_MemFree (pstPool->ppucRawSec[usIndex]);
			}
			pstPool->ppucRawSec[usIndex] = NULL;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static void local_MHEG_RAW_RecalculateIndexStartPointInSecTable (MhegRawSecTable *pstSecTable)
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

static HUINT8 local_MHEG_RAW_GetExtIdType (HUINT32 eSiTableType)
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


HERROR MHEG_AppendSiTblRawDataToList(MHEG_SiRawDataList_t* pstRawData, MhegSiRawDataType eType)
{
	MHEG_SiRawDataList_t*		pstRawList = NULL, *pPrevList = NULL;

	if(pstRawData == NULL)
	{
		HxLOG_Error("pstRawData : NULL");
		return ERR_FAIL;
	}

	HxLOG_Debug(" Append Si Table -> RawData(0x%x) \n", (HUINT32)pstRawData);
	pstRawData->pNext = NULL;

	ENTER_CRITICAL_MHEG_RAWLISTS;

	if(s_pstMHEGSiRawList[eType] == NULL)
	{
		s_pstMHEGSiRawList[eType] = pstRawData;
		LEAVE_CRITICAL_MHEG_RAWLISTS;
		return ERR_OK;
	}

	pstRawList = s_pstMHEGSiRawList[eType];
	while(pstRawList)
	{
		if(((pstRawList->tableId == PMT_TABLE_ID) || (pstRawList->pid == pstRawData->pid) )&& pstRawList->tableId == pstRawData->tableId &&
			pstRawList->tableIdExtn == pstRawData->tableIdExtn && pstRawList->uniqeId == pstRawData->uniqeId)
		{
			/* in case of same table id in same TS */
			if(pstRawList->savedRawData)
				HLIB_STD_MemFree(pstRawList->savedRawData);

			//Let's Update.
			pstRawList->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc( pstRawData->savedRawDataSize);
			HxSTD_memcpy(pstRawList->savedRawData,pstRawData->savedRawData, pstRawData->savedRawDataSize);
			pstRawList->savedRawDataSize	= pstRawData->savedRawDataSize;
			pstRawList->pid					= pstRawData->pid;

			HLIB_STD_MemFree(pstRawData->savedRawData);
			pstRawData->savedRawData = NULL;
			HLIB_STD_MemFree(pstRawData);
			pstRawData = NULL;
			break;
		}

		pPrevList = pstRawList;
		pstRawList = pstRawList->pNext;
	}

	if(pstRawList == NULL)
	{
		if(pPrevList == NULL)
		{
			HxLOG_Error("pPrevList : NULL");
			return ERR_FAIL;
		}
		pPrevList->pNext = pstRawData;
	}

	LEAVE_CRITICAL_MHEG_RAWLISTS;
	return ERR_OK;
}

/* REMARK :
    이 함수는 Caller에서 반드시 SI table Semaphore ( s_ulSiTblSema )를 Lock 한 상태로  호출 되어야 한다. */
HERROR MHEG_RemoveSiTblRawData(MHEG_SiRawDataList_t* pstRawData, MhegSiRawDataType eType)
{
	MHEG_SiRawDataList_t*		pstRawList =NULL, *pstPrevList = NULL;
	HERROR						ulErr = ERR_FAIL;

	if(pstRawData == NULL)
	{
		HxLOG_Error("pstRawData : NULL");
		return ERR_FAIL;
	}

	if((pstRawList = s_pstMHEGSiRawList[eType]) == NULL)
	{
		return ERR_OK;
	}

	HxLOG_Debug(" Remove Si RawData(%p) \n", pstRawData);
	pstPrevList = NULL;
	while(pstRawList)
	{
		if(pstRawList == pstRawData)
		{
			if(pstPrevList)
				pstPrevList->pNext = pstRawList->pNext;
			else
			{
				/* case of header list */
				s_pstMHEGSiRawList[eType] = pstRawList->pNext;
			}

			if(pstRawList->savedRawData)
			{
				HLIB_STD_MemFree(pstRawList->savedRawData);
				pstRawList->savedRawData = NULL;
			}

			HLIB_STD_MemFree(pstRawList);
			pstRawList = NULL;

			ulErr = ERR_OK;
			break;
		}

		pstPrevList = pstRawList;
		pstRawList = pstRawList->pNext;
	}
	return ulErr;
}


/* REMARK :
    이 함수는 Caller에서 반드시 MHEG-SI table Semaphore ( s_ulSiTblSema )를 Lock 한 상태로  호출 되어야 한다. */
MHEG_SiRawDataList_t *MHEG_GetSiRawData(HUINT16 usUniqId, HUINT16 usPid, HUINT16 usTableId, HUINT16 usExtTableId, MhegSiRawDataType eType)
{
	MHEG_SiRawDataList_t*		pstRawList =NULL;

	if((pstRawList = s_pstMHEGSiRawList[eType]) == NULL)
	{
		return NULL;
	}

	while(pstRawList)
	{
		//HxLOG_Debug(" uniqId(%d), tableId(0x%x), tableidExt(0x%x), Pid(0x%x) \n", pstRawList->uniqeId, pstRawList->tableId, pstRawList->tableIdExtn, pstRawList->pid);
		if(((usPid == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->pid == usPid)) &&
		   ((usTableId == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->tableId == usTableId)) &&
		   ((usExtTableId == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->tableIdExtn == usExtTableId)) &&
		   ((usUniqId == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->uniqeId == usUniqId)))
		{
			return pstRawList;
		}

		pstRawList = pstRawList->pNext;
	}
	return NULL;

}

HERROR MHEG_RemoveSiTblPMTDataExceptCurSvc(HUINT16 usUniqId, HUINT16 usPmtPid, MhegSiRawDataType eType)
{
	MHEG_SiRawDataList_t*		pstRawList =NULL, *pstPrevList = NULL,*tmpstRawList;
	HERROR						ulErr = ERR_FAIL;

	ENTER_CRITICAL_MHEG_RAWLISTS;

	if((pstRawList = s_pstMHEGSiRawList[eType]) == NULL)
	{
		LEAVE_CRITICAL_MHEG_RAWLISTS;
		return ERR_OK;
	}

	pstPrevList = NULL;
	while(pstRawList)
	{
		if( pstRawList->pid && (((pstRawList->pid != usPmtPid) && (pstRawList->uniqeId == usUniqId)) ||  (pstRawList->uniqeId != usUniqId)))
		{
			if(pstPrevList)
				pstPrevList->pNext = pstRawList->pNext;
			else
			{
				/* case of header list */
				s_pstMHEGSiRawList[eType] = pstRawList->pNext;
			}

			tmpstRawList = pstRawList;
			pstRawList = pstRawList->pNext;

			if(tmpstRawList->savedRawData)
			{
				HLIB_STD_MemFree(tmpstRawList->savedRawData);
				tmpstRawList->savedRawData = NULL;
			}

			HLIB_STD_MemFree(tmpstRawList);
			tmpstRawList = NULL;
		}
		else {
			pstPrevList = pstRawList;
			pstRawList = pstRawList->pNext;
		}
	}

	LEAVE_CRITICAL_MHEG_RAWLISTS;

	return ulErr;
}

MhegRawSecTable *MHEG_RAW_GetRawTable (MhegRawPool *pstPool, SiTableType_t eTableType)
{
	if (pstPool == NULL)					{ return NULL; }
	if (eTableType >= MAX_SI_TABLE_TYPE)	{ return NULL; }

	return &(pstPool->astSiTable[eTableType]);
}

HUINT8 *MHEG_RAW_GetRawSection (MhegRawPool *pstPool, HUINT32 ulSecIdx)
{
	if (pstPool == NULL)									{ return NULL; }
	if (pstPool->usMaxNumRawSec <= (HUINT16)ulSecIdx)		{ return NULL; }

	return pstPool->ppucRawSec[ulSecIdx];
}

HUINT16 MHEG_RAW_GetExtIdFromRawData (HUINT8 *pucRaw)
{
	if (pucRaw != NULL)
	{
		return HxMACRO_Get16Bit(pucRaw+3);
	}

	return 0;
}

MhegRawPool *MHEG_Raw_GetRawPool (MhegSiRawDataType eType)
{
	return s_pstMhegSiRawDataPool[eType];
}

void MHEG_Raw_ClearRawPool (MhegSiRawDataType eType)
{
	HUINT16			 	usCount, ulIndexCnt;
	MhegRawSecTable*	pstSecTable;
	MhegRawPool*		pstPool = s_pstMhegSiRawDataPool[eType];

	HxLOG_Debug(" \r\n");

	ENTER_CRITICAL_MHEGRAW;
	for (usCount = 0; usCount < MAX_SI_TABLE_TYPE; usCount++)
	{
		pstSecTable = MHEG_RAW_GetRawTable (pstPool, usCount);

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
			local_MHEG_RAW_FreeAllReceivingRawTableInSecTable (pstSecTable, FALSE);
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
				HLIB_STD_MemFree(pstPool->ppucRawSec[usCount]);
				pstPool->ppucRawSec[usCount] = NULL;
			}
		}
	}
	LEAVE_CRITICAL_MHEGRAW;

	return;
}

HUINT32 MHEG_RAW_AddRawSection(HUINT16 usUniqueId, SiTableType_t eSiTableType, HUINT8 *pucRawSec, MhegSiRawDataType eType)
{
	HUINT32		 	ulRes;
	HUINT16			usIndex;
	MhegRawSecTable	*pstSecTable = NULL;
	MhegRawPool		*pstPool = s_pstMhegSiRawDataPool[eType];

	if (pstPool == NULL)							{ return ERR_FAIL; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	ENTER_CRITICAL_MHEGRAW;
	ulRes = local_MHEG_RAW_PutSection (pstPool, pucRawSec, &usIndex);
	if(ulRes == ERR_OK)
	{
		pstSecTable = MHEG_RAW_GetRawTable (pstPool, eSiTableType);

		if (pstSecTable->usInsertPos >= pstSecTable->usMaxSecNum)
		{
			local_MHEG_RAW_ArrangeIndexHandle (pstSecTable);

			if (pstSecTable->usInsertPos >= pstSecTable->usMaxSecNum)
			{
				ulRes = local_MHEG_RAW_ExtendIndexHandleBuffer (pstSecTable);
				if (ulRes != ERR_OK)
				{
					local_MHEG_RAW_FreeSection (pstPool, usIndex, FALSE);
					LEAVE_CRITICAL_MHEGRAW;
					return ERR_FAIL;
				}
			}
		}

		pstSecTable->phIdx[pstSecTable->usInsertPos] = MACRO_MAKE_INDEX_HANDLE (usUniqueId, usIndex);
		pstSecTable->usSecNum++;
		pstSecTable->usInsertPos++;
	}

	LEAVE_CRITICAL_MHEGRAW;

	return ulRes;
}

void MHEG_RAW_FreeAllSection(HUINT16 usUniqueId, SiTableType_t eSiTableType, MhegSiRawDataType eType)
{
	HUINT16			 usCount;
	HUINT32			 ulIndex;
	MhegRawSecTable	*pstSecTable;
	MhegRawPool 	*pstPool = s_pstMhegSiRawDataPool[eType];
	HUINT32			 ulRes;

	if (pstPool == NULL)							{ return; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return; }

	ENTER_CRITICAL_MHEGRAW;

	pstSecTable = MHEG_RAW_GetRawTable (pstPool, eSiTableType);

	for (usCount = pstSecTable->usStartPos; usCount < pstSecTable->usInsertPos; usCount++)
	{
		if (pstSecTable->phIdx[usCount] != INVALID_INDEX_HANDLE)
		{
			ulIndex = pstSecTable->phIdx[usCount];

			if (usUniqueId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(ulIndex))
			{
				ulRes = local_MHEG_RAW_FreeSection (pstPool, MACRO_GET_INDEX_FROM_IDXHANDLE(ulIndex), TRUE);
				if (ulRes != ERR_OK)
				{
					HxLOG_Error("ulRes = (%08x)\n", ulRes);
				}

				pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
				pstSecTable->usSecNum--;
			}
		}
	}

	local_MHEG_RAW_RecalculateIndexStartPointInSecTable (pstSecTable);
	LEAVE_CRITICAL_MHEGRAW;
}

HERROR MHEG_RAW_FreeSection(HUINT16 usUniqueId, SiTableType_t eSiTableType, HBOOL bUseExtId, HUINT32 ulExtId, MhegSiRawDataType eType)
{
	HUINT8			 ucExtIdType;
	HUINT16			 usCount;
	HUINT16			 usSecIndex;
	HUINT16			 usExtIdFromRaw;
	MhegRawSecTable	*pstSecTable;
	MhegRawPool 	*pstPool = s_pstMhegSiRawDataPool[eType];

	if (pstPool == NULL)							{ return ERR_FAIL; }
	if (eSiTableType >= MAX_SI_TABLE_TYPE)			{ return ERR_FAIL; }

	ENTER_CRITICAL_MHEGRAW;
	ucExtIdType = local_MHEG_RAW_GetExtIdType (eSiTableType);
	pstSecTable = MHEG_RAW_GetRawTable (pstPool, eSiTableType);
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
				usExtIdFromRaw = MHEG_RAW_GetExtIdFromRawData (pstPool->ppucRawSec[usSecIndex]);
				if (usExtIdFromRaw == (HUINT16)ulExtId)
				{
					local_MHEG_RAW_FreeSection (pstPool, usSecIndex, TRUE);
					pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
					pstSecTable->usSecNum--;
				}
			}
			else if (ucExtIdType == EXTID_UNUSE)
			{
				local_MHEG_RAW_FreeSection (pstPool, usSecIndex, TRUE);
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
					usExtIdFromRaw = MHEG_RAW_GetExtIdFromRawData (pstPool->ppucRawSec[usSecIndex]);
					if (usExtIdFromRaw == (HUINT16)ulExtId)
					{
						local_MHEG_RAW_FreeSection (pstPool, usSecIndex, TRUE);
						pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
						pstSecTable->usSecNum--;
					}
				}
			}
		}
		else
		{
			local_MHEG_RAW_FreeSection (pstPool, usSecIndex, TRUE);
			pstSecTable->phIdx[usCount] = INVALID_INDEX_HANDLE;
			pstSecTable->usSecNum--;
		}
	}

	local_MHEG_RAW_RecalculateIndexStartPointInSecTable (pstSecTable);

	LEAVE_CRITICAL_MHEGRAW;

	return ERR_OK;
}


#define	___TABLE_PARING_FOR_MHEG____




HERROR MHEG_SI_GetFromMHEGRawData(HUINT16 usTsUniqId, HUINT16 usPid, HUINT16 usTableId, HUINT16 usExtTableId, HUINT16* pusDataLen, HUINT8** ppucSection)
{
	MHEG_SiRawDataList_t		*pstRawList =NULL;
	HERROR						ulErr = ERR_FAIL;
	HUINT8*						pucTmpSection = NULL;

	/* initialize output variables */
	*pusDataLen = 0;
	*ppucSection = NULL;

	ENTER_CRITICAL_MHEG_RAWLISTS;

	if((pstRawList = s_pstMHEGSiRawList[eMHEG_SI_RAW_DATA_TYPE_LIVE]) == NULL)
	{
		LEAVE_CRITICAL_MHEG_RAWLISTS;
		return ulErr;
	}

	//HxLOG_Debug("[GetFromMHEGRawData()] uniqId(%d), usPid(0x%x), usTableId(0x%x), usExtTableId(0x%x) \n", usTsUniqId, usPid, usTableId, usExtTableId);

	if(usTableId == PMT_TABLE_ID)
		usPid = MHEG_DEMUX_VALUE_DONT_CARE;

	while(pstRawList)
	{
		//HxLOG_Debug(" uniqId(%d), pid(0x%x) tableId(0x%x), tableIdExtn(0x%x) \n", pstRawList->uniqeId, pstRawList->pid, pstRawList->tableId, pstRawList->tableIdExtn);
		if(((usPid == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->pid == usPid)) &&
		   ((usTableId == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->tableId == usTableId)) &&
		   ((usExtTableId == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->tableIdExtn == usExtTableId)) &&
		   ((usTsUniqId == MHEG_DEMUX_VALUE_DONT_CARE) || (pstRawList->uniqeId == usTsUniqId)))
		{
			pucTmpSection = (HUINT8*)HLIB_STD_MemAlloc(pstRawList->savedRawDataSize);
			if(pucTmpSection != NULL)
			{
				HxSTD_memcpy(pucTmpSection, pstRawList->savedRawData, pstRawList->savedRawDataSize);
				*ppucSection = pucTmpSection;
				*pusDataLen = pstRawList->savedRawDataSize;

				ulErr = ERR_OK;
				break;
			}
		}

		pstRawList = pstRawList->pNext;
	}

	LEAVE_CRITICAL_MHEG_RAWLISTS;

	return ulErr;

}

#if defined(CONFIG_DEBUG)
HINT32	MHEG_RAW_PrintRawPool (void *pvArg)
{
	HUINT16			 usCount, usSecIdx;
	HUINT32			 ulSiRawDataType, ulSiType, ulRawIdx;
	HUINT32			 ulTotalSecNum;
	HUINT8			*pucRaw;
	MhegRawSecTable	*pstTable = NULL;
	MhegRawPool		*pstPool = NULL;

	for (ulSiRawDataType = 0; ulSiRawDataType < eMHEG_SI_RAW_DATA_TYPE_MAX; ulSiRawDataType++)
	{
		pstPool = s_pstMhegSiRawDataPool[ulSiRawDataType];
		if (pstPool == NULL)	continue;

		ulTotalSecNum = 0;

		HLIB_CMD_Printf("\n##############################################################################\n");
		HLIB_CMD_Printf("\t\tPRINT MHEG RAW_POOL : [%d][%s]\n", ulSiRawDataType, pstPool->szPoolName);
		HLIB_CMD_Printf("-----------------------------------------------------------------------------\n");

		for (ulSiType = 0; ulSiType < MAX_SI_TABLE_TYPE; ulSiType++)
		{
			pstTable = MHEG_RAW_GetRawTable (pstPool, (SiTableType_t)ulSiType);
			if (pstTable == NULL)		{ continue; }

			HLIB_CMD_Printf("\t==[%02d] SI_TABLE(type:%02d, sec_cnt:%d/%d, insert_pos:%d)\n", ulSiType,
						pstTable->eSiTableType,
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
	}

	HLIB_CMD_Printf("-----------------------------------------------------------------------------\n");

	return 0;
}
#endif


#define __STATIC_FUNCTION__

STATIC HERROR mheg_raw_GetComponentTag(HUINT8* pRawData, HUINT32 ulIndex, HUINT8 ucElementType, HUINT8* pucCompTag)
{
	HUINT32		ulDesCnt = 0, ulDesIndex = 0;
	HERROR		hErr = ERR_FAIL;
	HUINT8		*pDesc = NULL;

	if(pRawData == NULL || pucCompTag == NULL)
	{
		HxLOG_Error("pRawData || pucCompTag : NULL");
		return ERR_FAIL;
	}

	hErr = MHEG_SI_CountPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_STREAM_IDENTIFIER, &ulDesCnt);
	if(hErr !=ERR_OK || ulDesCnt == 0)
	{
		HxLOG_Error("MHEG_SI_CountPmtElementDescriptorFromRaw Error(0x%x)  ulCnt(%d) \n", hErr, ulDesCnt);
		return ERR_FAIL;
	}

	for(ulDesIndex = 0; ulDesIndex < ulDesCnt; ulDesIndex++)
	{
		hErr = MHEG_SI_GetPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_STREAM_IDENTIFIER, ulDesIndex, (void**)&pDesc);
		if( hErr == ERR_OK && pDesc != NULL)
		{
			SIxStmIdentDes_t* pstStmIdDesc = NULL;

			pstStmIdDesc = (SIxStmIdentDes_t*)pDesc;
			*pucCompTag = pstStmIdDesc->ucCompTag;

			SILIB_DESC_Delete((SIxDescriptor_t*)pstStmIdDesc);

			HxLOG_Debug("elementType(0x%x) --> CompTag(%d) \n", ucElementType, *pucCompTag);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR mheg_raw_GetIso639LanguageCode(HUINT8* pRawData, HUINT32 ulIndex, HUINT32 ulMaxLangNum, MhegIso639LangInfo_t *pstIsoLangInfo)
{
	HUINT32					ulLangDesCnt = 0, ulDesIndex = 0,ulTargetIdx = 0;
	HERROR					hErr = ERR_OK;
	HUINT8					*pDesc = NULL;
	SIxIso639LangDes_t		*pstIso639LangDes = NULL;
	SIxLangInfo_t				*pstLangInfo = NULL;
	MhegIso639LangInfo_t		*pstCurLangInfo = NULL;

	if(pRawData == NULL || pstIsoLangInfo == NULL)
	{
		HxLOG_Error("pRawData || pstIsoLangInfo : NULL");
		return ERR_FAIL;
	}

	HxSTD_memset(pstIsoLangInfo , 0 , sizeof(MhegIso639LangInfo_t)*ulMaxLangNum);

	hErr = MHEG_SI_CountPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_ISO_639_LANGUAGE, &ulLangDesCnt);
	if(hErr != ERR_OK || ulLangDesCnt == 0)
	{
		HxLOG_Error("MHEG_SI_CountPmtElementDescriptorFromRaw Error(0x%x)  : ulLangDesCnt(%d) \n", hErr, ulLangDesCnt);
		return ERR_FAIL;
	}

	HxLOG_Debug("eSIxDESCTAG_ISO_639_LANGUAGE Count(%d)\n", ulLangDesCnt);

	for (ulDesIndex = 0 ; ulDesIndex < ulLangDesCnt; ulDesIndex++)
	{
		hErr = MHEG_SI_GetPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_ISO_639_LANGUAGE, ulDesIndex, (void**)&pDesc);
		if ( ERR_OK == hErr && pDesc != NULL)
		{
			pstIso639LangDes  = (SIxIso639LangDes_t *)pDesc;

			pstLangInfo = pstIso639LangDes->pstInfo;
			while(NULL != pstLangInfo)
			{
				pstCurLangInfo = &pstIsoLangInfo[ulTargetIdx];
				pstCurLangInfo->eAudType = pstLangInfo->ucAudioType;
				HxSTD_memcpy(pstCurLangInfo->szIso639LangCode, pstLangInfo->szIso639LangCode, 4);
				HxLOG_Debug("[%02d][AudioType:0x%X][Audio Language code : %s]\n",
					ulTargetIdx,pstCurLangInfo->eAudType, (char *)pstCurLangInfo->szIso639LangCode);
				ulTargetIdx++;
				pstLangInfo = pstLangInfo->pstNext;
			}

			SILIB_DESC_Delete((SIxDescriptor_t *)pstIso639LangDes);
		}
	}

#if defined(CONFIG_DEBUG)
{
	for (ulDesIndex = 0; ulDesIndex < 4; ulDesIndex++)
	{
		HxLOG_Debug(" [%02d] AudioType:0x%X LangCode:%s\n\r",
			ulDesIndex,pstIsoLangInfo->eAudType,pstIsoLangInfo->szIso639LangCode);
	}
}
#endif

	return hErr;
}

STATIC HUINT32 mheg_raw_GetAudioPriority(DxAudioCodec_e audCodecType, AUDIO_Type_t AudioType, SUPPLEMENTARY_AUDIO_Type_t suppAudioType, HxLANG_Id_e curLangId, HUINT8 menuPriority, HxLANG_Id_e menuLangId)
{
	HUINT32 	PriorityCurAudio = (20 * 1000);
	HUINT32 	PriorityAudioDesc = 0;
	HUINT32 	PriorityDolbyAudio = 0;
	HUINT32 	PriorityMenuAudioLang = 0;
	HERROR		hErr;
	HINT32		bAudioDescription = FALSE;
	HBOOL		bAutoDolby = FALSE;

	PriorityMenuAudioLang = (10 * 100 * menuPriority);

	hErr = APK_OUTPUT_AUDIO_GetAudioDescription(&bAudioDescription);
//	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&bAudioDescription, 0);
	//hErr = DB_SETUP_GetMenuItemValue(MENUCFG_ITEMNAME_AUDIO_DESCRIPTION, &val);
	if ( (hErr == ERR_OK) && ((HBOOL)bAudioDescription == TRUE) )
	{
		PriorityAudioDesc = (100 * 100 * 100);
	}

#if 0
	//hErr = DB_SETUP_GetMenuItemValue(MENUCFG_ITEMNAME_SOUND_DIGITALOUTPUT, &val);
	if ((hErr == ERR_OK) && (val == eDxAUDIO_CODEC_DOLBY_AC3))
	{
		bAutoDolby = TRUE;
	}
#endif

	bAutoDolby = TRUE;	/* Freesat은 메뉴 값 상관없이 Dolby 우선 이다.  Same as FreeView*/
	switch (audCodecType)
	{
		case eDxAUDIO_CODEC_DOLBY_AC3:
			if (bAutoDolby)
				PriorityDolbyAudio = (1 * 100 * 100 + 1);
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			if (bAutoDolby)
				PriorityDolbyAudio = (1 * 100 * 100);
			break;
		case eDxAUDIO_CODEC_MPEG:
			PriorityDolbyAudio = 0;
			break;
		default:
			return 1;	/* AAC 등은 어떻게 될지 모르겠음. 최 하위 값 리턴 */
			break;
	}

	if (suppAudioType == eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED)
	{
		if (curLangId == menuLangId)
		{
			PriorityCurAudio += PriorityDolbyAudio + PriorityAudioDesc + (10) ;
		}
		else if (curLangId == eLangID_Undefined)
		{
			PriorityCurAudio += PriorityDolbyAudio + PriorityAudioDesc + (5) ;
		}
		else if (curLangId == eLangID_OriginalAudio)
		{
			PriorityCurAudio += PriorityDolbyAudio + PriorityAudioDesc + (4) ;
		}
		else if (curLangId == eLangID_English)	/* Default is English */
		{
			PriorityCurAudio += PriorityDolbyAudio + PriorityAudioDesc + (3) ;
		}
		else
		{
			PriorityCurAudio += 1;
		}
	}
	else
	{
		/*	Mono, Stereo 우선 순위 구별은 못함.  */
		if (curLangId == menuLangId)
		{
			if (AudioType == eAUDIO_TYPE_UNDEFINED) 			{ PriorityCurAudio += PriorityDolbyAudio + (900 + 0) + PriorityMenuAudioLang; }
			else if (AudioType == eAUDIO_TYPE_CLEAN_EFFECTS)	{ PriorityCurAudio += PriorityDolbyAudio + (500 + 0) + PriorityMenuAudioLang; }
			else if (AudioType == eAUDIO_TYPE_HEARING_IMPAIRED) { PriorityCurAudio += PriorityDolbyAudio + (9); }
			else if (AudioType == eAUDIO_TYPE_VISUAL_IMPAIRED)	{ PriorityCurAudio += PriorityDolbyAudio + (9); }
			else												{ PriorityCurAudio += PriorityDolbyAudio + (9); }
		}
		else if (curLangId == eLangID_Undefined)
		{
			if (AudioType == eAUDIO_TYPE_UNDEFINED) 			{ PriorityCurAudio += PriorityDolbyAudio + (800 + 0); }
			else if (AudioType == eAUDIO_TYPE_CLEAN_EFFECTS)	{ PriorityCurAudio += PriorityDolbyAudio + (400 + 0); }
			else if (AudioType == eAUDIO_TYPE_HEARING_IMPAIRED) { PriorityCurAudio += PriorityDolbyAudio + (8); }
			else if (AudioType == eAUDIO_TYPE_VISUAL_IMPAIRED)	{ PriorityCurAudio += PriorityDolbyAudio + (8); }
			else												{ PriorityCurAudio += PriorityDolbyAudio + (8); }
		}
		else if (curLangId == eLangID_OriginalAudio)
		{
			if (AudioType == eAUDIO_TYPE_UNDEFINED) 			{ PriorityCurAudio += PriorityDolbyAudio + (700 + 0); }
			else if (AudioType == eAUDIO_TYPE_CLEAN_EFFECTS)	{ PriorityCurAudio += PriorityDolbyAudio + (300 + 0); }
			else if (AudioType == eAUDIO_TYPE_HEARING_IMPAIRED) { PriorityCurAudio += PriorityDolbyAudio + (7); }
			else if (AudioType == eAUDIO_TYPE_VISUAL_IMPAIRED)	{ PriorityCurAudio += PriorityDolbyAudio + (7); }
			else												{ PriorityCurAudio += PriorityDolbyAudio + (7); }
		}
		else if (curLangId == eLangID_English)	/* Default is English */
		{
			if (AudioType == eAUDIO_TYPE_UNDEFINED) 			{ PriorityCurAudio += PriorityDolbyAudio + (600 + 0); }
			else if (AudioType == eAUDIO_TYPE_CLEAN_EFFECTS)	{ PriorityCurAudio += PriorityDolbyAudio + (200 + 0); }
			else if (AudioType == eAUDIO_TYPE_HEARING_IMPAIRED) { PriorityCurAudio += PriorityDolbyAudio + (6); }
			else if (AudioType == eAUDIO_TYPE_VISUAL_IMPAIRED)	{ PriorityCurAudio += PriorityDolbyAudio + (6); }
			else												{ PriorityCurAudio += PriorityDolbyAudio + (6); }
		}
		else if (curLangId == eLangID_Narration)
		{
			PriorityCurAudio += PriorityDolbyAudio + PriorityAudioDesc + (9) ;
		}
		else
		{
			if (AudioType == eAUDIO_TYPE_UNDEFINED) 			{ PriorityCurAudio += PriorityDolbyAudio + (100 + 1); }
			else if (AudioType == eAUDIO_TYPE_CLEAN_EFFECTS)	{ PriorityCurAudio += PriorityDolbyAudio + (100 + 0); }
			else if (AudioType == eAUDIO_TYPE_HEARING_IMPAIRED) { PriorityCurAudio += PriorityDolbyAudio + (5); }
			else if (AudioType == eAUDIO_TYPE_VISUAL_IMPAIRED)	{ PriorityCurAudio += PriorityDolbyAudio + (5); }
			else												{ PriorityCurAudio += PriorityDolbyAudio + (5); }
		}
	}

	return PriorityCurAudio;
}

/* NOTICE :
    1. note that you must lock valid semaphore for PMT Rawdata before call this at caller side.
    2. You have to sync code with local_ukter_SelectAudioIndex() , local_ukter_SelectVideoIndex() and local_ukter_GetAudioPriority().
       If you are not sync these code, your DTG test suite testing will be fail.
*/


STATIC HERROR mheg_raw_GetDefaultPesInfo(HUINT8* pRawData, HUINT16 usLen, HUINT8 ucElementType, HINT32* pnPid, HUINT8* pucCodecType, DxStereoSelect_e* pSoundMode)
{
	HERROR		ulErr = ERR_OK;
	HUINT32 		ulCntStream = 0, ulIndex = 0;
	HUINT16 		usPid = PID_NULL;
	HUINT32 		lastPriority = MHEG_AUDIO_PRIORITY_NONE, curPriority=MHEG_AUDIO_PRIORITY_NONE;
	HUINT32		lastDolbyPriority = MHEG_AUDIO_PRIORITY_NONE, lastMpegPriority = MHEG_AUDIO_PRIORITY_NONE;
	HUINT8		ucCodec,ucLastCodec=eDxAUDIO_CODEC_UNKNOWN;
	MhegIso639LangInfo_t	stIsoLangInfo[4];
	HxLANG_Id_e			eLangId1 = eLangID_MAX;
	HxLANG_Id_e			eLangId2 = eLangID_MAX;

	PmtElementInfo_t	stElementInfo;
	PmtProgramInfo_t	stProgramInfo;
	HxLANG_Id_e			eAudLangId = eLangID_MAX;
	DxStereoSelect_e	mode = eDxSTEREO_SELECT_STEREO;
	HUINT8		szAudioLang[4] = {0,};
	HBOOL		bAD = FALSE;


	/* for Dolby */
	HUINT16 		usDolbyPid = PID_NULL;	/* Dolby audio가 mpeg audio보다 우선 */
	HUINT8		unDolbyType = eDxAUDIO_CODEC_UNKNOWN;


	if(pnPid == NULL || pucCodecType == NULL)
	{
		HxLOG_Error("pstPmtTable || pusPid || pucCodecType : NULL");
		return ERR_FAIL;
	}

	/* initiate value */
	*pnPid = PID_NULL;
	*pucCodecType = 0;
	*pSoundMode = eDxSTEREO_SELECT_STEREO;

	if(ucElementType == ITK_ELEMENT_INDEX_PCR)
	{
		if((ulErr = MHEG_SI_GetPmtProgramInfoFromRaw(pRawData, &stProgramInfo)) == ERR_OK)
			*pnPid = stProgramInfo.usPCRPid;

		return ulErr;
	}
	else if(ucElementType == ITK_ELEMENT_INDEX_VIDEO)
	{
		MHEG_SI_CountPmtElementFromRaw(pRawData, &ulCntStream);
		if(ulCntStream > 0)
		{
			for(ulIndex = 0; ulIndex < ulCntStream; ulIndex++)
			{
				curPriority = 1;
				ulErr = MHEG_SI_GetPmtElementInfoFromRaw(pRawData, ulIndex, &stElementInfo);
				if(ERR_OK != ulErr)
					continue;

				HxLOG_Debug("\tstreamType(0x%x) PID(0x%x)\n", stElementInfo.ucStreamType, stElementInfo.usElemPid);
				/* we assume that valid video pid in a service is just one */
				if(stElementInfo.ucStreamType == ITK_ISO_TYPE_VIDEO_11172 || stElementInfo.ucStreamType == ITK_ISO_TYPE_VIDEO_13818 ||
					stElementInfo.ucStreamType == ITK_ISO_VIDEO_H_264)
				{
#if defined(CONFIG_DB_SVC_TYPE_HD)
					// when we found HD video, we will use it , in HD BOX
					if ( ITK_ISO_VIDEO_H_264 == stElementInfo.ucStreamType)
						curPriority = 2;
					else
						curPriority = 1;
#else
					// when we found first Video PID, we will use it, in SD BOX.
					curPriority = 1;
#endif
					if (PID_NULL == stElementInfo.usElemPid)
						continue;

					if (curPriority > lastPriority)
					{
						DxVideoCodec_e	eVCodec;

						MHEG_SI_FindVideoCodecFromPSI (stElementInfo.ucStreamType, &eVCodec);
						*pucCodecType = eVCodec;
						usPid = stElementInfo.usElemPid;
						lastPriority = curPriority;
					}

				}
			}

			if(usPid == PID_NULL)
			{
				HxLOG_Debug(" fail to find default Pes info !!! \n");
				ulErr = ERR_FAIL;
			}
			else
			{
				*pnPid = usPid;
				*pSoundMode = mode;
				ulErr = ERR_OK;
			}
		}
		HxLOG_Debug("*pusPid(0x%x), *pSoundMode(0x%x), ulErr (0x%x)\n", *pnPid, *pSoundMode, ulErr);
	}
	else if(ucElementType == ITK_ELEMENT_INDEX_AUDIO)
	{
		/*
		 *	[시나리오]
		 *	audio 선택은 아래의 priority를 따른다.
		 *	단, Dolby와 AD에 관해 필요시에 추가구현해야한다.
		 *
		 *	<PCM - MPEG>
		 *	1. Audio Language
		 *	2. Menu Language
		 *	3. Undefine Language.(Only UK)
		 */

		/* Audio Language ID 값을 얻어 온다. */
		ulErr = APK_SCENARIO_GetAudioLanguage(0, szAudioLang);
		if ( ulErr != ERR_OK )
		{
			eAudLangId = eLangID_MAX;
		}
		else
		{
			eAudLangId = HLIB_LANG_639ToId(szAudioLang);
		}

		/* Menu에서 AD(Audio Description) on/off 항목 value를 가져 온다. */
		ulErr = APK_OUTPUT_AUDIO_GetAudioDescription(&bAD);
		if ( ulErr != ERR_OK )
		{
			bAD = FALSE;
		}

		MHEG_SI_CountPmtElementFromRaw(pRawData, &ulCntStream);
		for(ulIndex = 0; ulIndex < ulCntStream; ulIndex++)
		{
			ulErr = MHEG_SI_GetPmtElementInfoFromRaw(pRawData, ulIndex, &stElementInfo);
			if (ERR_OK != ulErr)
				continue;

			HxLOG_Debug("\tstreamType(0x%x) PID(0x%x)\n", stElementInfo.ucStreamType, stElementInfo.usElemPid);
			if ((stElementInfo.ucStreamType ==  ITK_ISO_PRIVDATA_2) || ( ITK_ISO_PRIVDATA_1 == stElementInfo.ucStreamType))
			{
				SIxDolbyAC3Des_t		*pAC3Des = NULL;
				SIxEnhancedAC3Des_t		*pE_AC3Des = NULL;
				HBOOL					bAC3 = FALSE;

				/* 왜 항상 첫번째만 가정하지???? */
				ulErr = MHEG_SI_GetPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_DOLBY_AC3, 0, (void**)&pAC3Des);
				if ((ulErr == ERR_OK) && (pAC3Des != NULL))
				{
					/* AC3 audio */
					bAC3 = TRUE;
					unDolbyType = eDxAUDIO_CODEC_DOLBY_AC3;
				}
				else
				{
					/* 왜 항상 첫번째만 가정하지???? */
					ulErr = MHEG_SI_GetPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_ENHANCED_AC3, 0, (void**)&pE_AC3Des);
					if ((ulErr == ERR_OK) && (pE_AC3Des != NULL))
					{
						/* E-AC3 audio */
						bAC3 = TRUE;
						unDolbyType = eDxAUDIO_CODEC_DOLBY_AC3P;
					}
				}

				if (bAC3 == TRUE)
				{
					// 1. get ISO 639 language
					ulErr = mheg_raw_GetIso639LanguageCode(pRawData, ulIndex, 4, stIsoLangInfo);
					if (ulErr == ERR_OK)
					{
						eLangId1 = HLIB_LANG_639ToId(stIsoLangInfo[0].szIso639LangCode);
						if ( eLangId1== eLangID_NONE )
						{
							eLangId1 = eLangID_MAX;
						}
#if 0
						ulErr = MWC_UTIL_GetLangIDBy639Code(stIsoLangInfo[0].szIso639LangCode, &eLangId1);
						if ( ulErr != ERR_OK )
						{
							eLangId1 = eLangID_MAX;
							if (0 == HxSTD_StrLen((const HUINT8 *)stIsoLangInfo[0].szIso639LangCode))
								eLangId1 = eLangID_NONE;

						}
#endif
					}
					else
					{
						eLangId1 = eLangID_MAX;
					}

					// PMT에 TAG_EXT_SUPPLEMENTARY_AUDIO가 존재 할 때만 eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED라는 define가능.
					curPriority = mheg_raw_GetAudioPriority(unDolbyType,stIsoLangInfo[0].eAudType,eAUDIO_TYPE_SUPP_MAIN,eLangId1,0,eAudLangId);
					if ( curPriority > lastDolbyPriority )
					{
						lastDolbyPriority = curPriority;
						usDolbyPid = stElementInfo.usElemPid;
					}

					HxLOG_Debug(" >> AC3 << lastPriority(0x%x), usPid(0x%x), mode(0x%x)\n", lastPriority, usPid, mode);
				}
				SILIB_DESC_Delete((SIxDescriptor_t *)pAC3Des);
				SILIB_DESC_Delete((SIxDescriptor_t *)pE_AC3Des);
			}
		}

		curPriority = MHEG_AUDIO_PRIORITY_NONE;
		lastMpegPriority = MHEG_AUDIO_PRIORITY_NONE;

		for(ulIndex = 0; ulIndex < ulCntStream; ulIndex++)
		{
			ulErr = MHEG_SI_GetPmtElementInfoFromRaw(pRawData, ulIndex, &stElementInfo);
			if (ERR_OK != ulErr)
				continue;

			HxLOG_Debug("\tstreamType(0x%x) PID(0x%x)\n", stElementInfo.ucStreamType, stElementInfo.usElemPid);
			if(stElementInfo.ucStreamType == ITK_ISO_TYPE_AUDIO_13818 || stElementInfo.ucStreamType == ITK_ISO_TYPE_AUDIO_11172 ||
			  stElementInfo.ucStreamType == ITK_ISO_AUDIO_H_264 || stElementInfo.ucStreamType == ITK_ISO_AUDIO_13818_7)
			{
				DxAudioCodec_e	eACodec;

				MHEG_SI_FindAudioCodecFromPSI (stElementInfo.ucStreamType, &eACodec);
				ucCodec = eACodec;
				ulErr = mheg_raw_GetIso639LanguageCode(pRawData, ulIndex, 4, stIsoLangInfo);
				if (ulErr == ERR_OK)
				{
					eLangId1 = HLIB_LANG_639ToId(stIsoLangInfo[0].szIso639LangCode);
					if ( eLangId1== eLangID_NONE )
					{
						eLangId1 = eLangID_MAX;
					}


					eLangId2 = HLIB_LANG_639ToId(stIsoLangInfo[1].szIso639LangCode);
					if ( eLangId2== eLangID_NONE )
					{
						eLangId2 = eLangID_MAX;
					}

#if 0
					ulErr = MWC_UTIL_GetLangIDBy639Code(stIsoLangInfo[0].szIso639LangCode, &eLangId1);
					if ( ulErr != ERR_OK )
					{
						eLangId1 = eLangID_MAX;
						if (0 == HxSTD_StrLen((const HUINT8 *)stIsoLangInfo[0].szIso639LangCode))
							eLangId1 = eLangID_NONE;

					}

					ulErr = MWC_UTIL_GetLangIDBy639Code(stIsoLangInfo[1].szIso639LangCode, &eLangId2);
					if ( ulErr != ERR_OK )
					{
						eLangId2 = eLangID_MAX;
						if (0 == HxSTD_StrLen((const HUINT8 *)stIsoLangInfo[1].szIso639LangCode))
							eLangId2 = eLangID_NONE;
					}
#endif
				}
				else
				{
					eLangId1 = eLangID_MAX;
					eLangId2 = eLangID_MAX;
				}

				if (eLangId2 == eLangID_NONE)
				{
					curPriority = mheg_raw_GetAudioPriority(ucCodec, stIsoLangInfo[0].eAudType, eAUDIO_TYPE_SUPP_MAIN, eLangId1, 0, eAudLangId);
					if ( curPriority > lastMpegPriority )
					{
						usPid = stElementInfo.usElemPid;
						// If There is only one ISO639 Language in PMT, We have to select Mono Left
						// we have to refer document in D-BOOK 6.1 7.4.2.2 Dual mono audio.
						mode = eDxSTEREO_SELECT_STEREO;	//changed mono--> stereo 2010.03.17
						lastMpegPriority = curPriority;
						ucLastCodec = ucCodec;
					}
				}
				else
				{
					curPriority = mheg_raw_GetAudioPriority(ucCodec, stIsoLangInfo[0].eAudType, eAUDIO_TYPE_SUPP_MAIN, eLangId1, 0, eAudLangId);
					if ( curPriority > lastMpegPriority )
					{
						usPid = stElementInfo.usElemPid;
						mode = eDxSTEREO_SELECT_MONOLEFT;
						lastMpegPriority = curPriority;
						ucLastCodec = ucCodec;
					}

					curPriority = mheg_raw_GetAudioPriority(ucCodec, stIsoLangInfo[1].eAudType, eAUDIO_TYPE_SUPP_MAIN, eLangId2, 0, eAudLangId);
					if ( curPriority > lastMpegPriority )
					{
						usPid = stElementInfo.usElemPid;
						mode = eDxSTEREO_SELECT_MONORIGHT;
						lastMpegPriority = curPriority;
						ucLastCodec = ucCodec;
					}
					if ((eLangId1 == eLangId2) && (stIsoLangInfo[0].eAudType == stIsoLangInfo[1].eAudType))
					{
						usPid = stElementInfo.usElemPid;
						mode = eDxSTEREO_SELECT_STEREO;
						ucLastCodec = ucCodec;
						//lastMpegPriority = curPriority;
					}
				}

				HxLOG_Debug("ulErr(0x%x) eLangId1(%d) eLangId2(%d) curPriority(0x%x) lastPriority(0x%x) usPid(0x%x) mode(0x%x)\n", __FUNCTION__, __LINE__, ulErr, eLangId1, eLangId2, curPriority, lastPriority, usPid, mode);
			}
		}

		if (lastMpegPriority < lastDolbyPriority)
		{
			*pnPid = usDolbyPid;
			*pucCodecType = unDolbyType;
			ulErr = ERR_OK;
		}
		else
		{
			*pnPid = usPid;
			*pSoundMode = mode;
			*pucCodecType = ucLastCodec;
			ulErr = ERR_OK;
		}

		HxLOG_Debug("\033[30;46m Audio pid : %x, Audio Codec : %x, Soundmode : %x  \033[0m\n", *pnPid, *pSoundMode, *pucCodecType);
		HxLOG_Debug("*pusPid(0x%x), *pSoundMode(0x%x), ulErr (0x%x)\n", *pnPid, *pSoundMode, ulErr);
	}
	else
	{
		ulErr = ERR_FAIL;
		HxLOG_Debug(" do not have elementary stream !! \n");
	}

	return ulErr;
}

/*
   NOTICE :
      1. Before call below function, you must be careful of if you locks valid semaphore for raw data or not
      2. Avaliable elementary type for this function is one of both MHEG_ELEMENT_INDEX_VIDEO or MHEG_ELEMENT_INDEX_AUDIO,
         PCR stream type is not available in this function.

*/
STATIC HERROR mheg_raw_GetPesInfo(HUINT8* pRawData, HUINT16 usLen, HINT32 nComponent, HUINT8 ucElementType, HINT32* pnCompPid, HUINT8* pucCodecType, HUINT8 *pucStreamType, DxStereoSelect_e* pSoundMode)
{
	HERROR					ulErr = ERR_OK;
	HUINT32 				ulCntStream = 0, ulIndex = 0;
	PmtElementInfo_t		stElementInfo;
	HUINT8					ucTmpComponentTag;

	if(pRawData == NULL || pnCompPid == NULL || pucCodecType == NULL || pucStreamType == NULL || pSoundMode == NULL)
	{
		HxLOG_Error("pRawData | pusCompPid | pucCodecType | pucStreamType | pSoundMode : NULL");
		return ERR_FAIL;
	}

	/* initiate default Component values */
	*pnCompPid = PID_NULL;
	*pucCodecType = 0;
	*pucStreamType = 0;

	if(nComponent == -1) 	/* -1 menas Default component type */
	{
		ulErr = mheg_raw_GetDefaultPesInfo(pRawData, usLen, ucElementType, pnCompPid, pucCodecType, pSoundMode);
	}
	else
	{
		MHEG_SI_CountPmtElementFromRaw(pRawData, &ulCntStream);
		if(ulCntStream > 0)
		{
			for(ulIndex = 0; ulIndex < ulCntStream; ulIndex++)
			{
				if(mheg_raw_GetComponentTag(pRawData, ulIndex, ucElementType, &ucTmpComponentTag) == ERR_OK)
				{
					if(ucTmpComponentTag == (HUINT8)nComponent)
						break;
				}
			}

			if(ulIndex < ulCntStream)
			{
				/* in case there is a valid component we want to */
				if(MHEG_SI_GetPmtElementInfoFromRaw(pRawData, ulIndex, &stElementInfo) == ERR_OK)
				{
					/* component tag만 맞다면 PID는 바로 취하고, Stream Type만 Rule에 따라 결정한다 */
					*pnCompPid = stElementInfo.usElemPid;

					/* check whether selected component is valid or not again */
					/*
						VIDEO
					*/
					if((ucElementType == ITK_ELEMENT_INDEX_VIDEO) &&
						(stElementInfo.ucStreamType == ITK_ISO_TYPE_VIDEO_11172 || stElementInfo.ucStreamType == ITK_ISO_TYPE_VIDEO_13818 ||
						 stElementInfo.ucStreamType == ITK_ISO_VIDEO_H_264 || stElementInfo.ucStreamType == ITK_ISO_PRIVDATA_2))
					{
						*pucStreamType = stElementInfo.ucStreamType;

						/* UK의 경우 video일때 ISO_PRIVDATA_2이면 H.264 or Mpeg2이다 */
						if (stElementInfo.ucStreamType == ITK_ISO_PRIVDATA_2)
						{
							/*
								SDT의 Stream Type을 보고 HD이면 H.264, SD이면 MPEG2로 결정한다
								채널 search시 service info를 구성할때 SDT를 보고 video type을 결정하므로, StreamType만 결정하여 올려주고, ap에서 DB service info를 보고 codec type을 결정한다
							*/
							*pucCodecType = eDxVIDEO_CODEC_UNKNOWN;
						}
						else
						{
							DxVideoCodec_e eCodec;

							if(MHEG_SI_FindVideoCodecFromPSI(stElementInfo.ucStreamType, &eCodec) != ERR_OK)
							{
								/* if error happens, we make it default value */
								*pucCodecType = eDxVIDEO_CODEC_MPEG2;
								HxLOG_Debug(" WARNING: no valid Video Codec from streamType(%d) --> set default \n", stElementInfo.ucStreamType);
							}
							else
							{
								*pucCodecType = eCodec;
							}
						}
					}
					/*
						AUDIO
					*/
					else if ((ucElementType == ITK_ELEMENT_INDEX_AUDIO)	 &&
									 (stElementInfo.ucStreamType == ITK_ISO_TYPE_AUDIO_13818 || stElementInfo.ucStreamType == ITK_ISO_TYPE_AUDIO_11172 ||
									 stElementInfo.ucStreamType == ITK_ISO_AUDIO_H_264 || stElementInfo.ucStreamType == ITK_ISO_AUDIO_13818_7 || stElementInfo.ucStreamType == ITK_ISO_PRIVDATA_2))
					{
						*pucStreamType = stElementInfo.ucStreamType;

						 /* AC3 or E-AC3 */
						if (stElementInfo.ucStreamType == ITK_ISO_PRIVDATA_2)
						{
							SIxDolbyAC3Des_t		*pAC3Des = NULL;
							SIxEnhancedAC3Des_t		*pE_AC3Des = NULL;

							ulErr = MHEG_SI_GetPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_DOLBY_AC3, 0, (void**)&pAC3Des);
							if ((ulErr == ERR_OK) && (pAC3Des != NULL))
							{
								/* AC3 audio */
								*pucCodecType = eDxAUDIO_CODEC_DOLBY_AC3;
								SILIB_DESC_Delete((SIxDescriptor_t *)pAC3Des);
							}
							else
							{
								ulErr = MHEG_SI_GetPmtElementDescriptorFromRaw(pRawData, ulIndex, eSIxDESCTAG_ENHANCED_AC3, 0, (void**)&pE_AC3Des);
								if ((ulErr == ERR_OK) && (pE_AC3Des != NULL))
								{
									/* E-AC3 audio */
									*pucCodecType = eDxAUDIO_CODEC_DOLBY_AC3P;
									SILIB_DESC_Delete((SIxDescriptor_t *)pE_AC3Des);
								}
								else
								{
									/* we make it default value */
									*pucCodecType = eDxAUDIO_CODEC_MPEG;
									ulErr = ERR_OK;
								}
							}
						}
						else
						{
							DxAudioCodec_e	eCodec;

							if(MHEG_SI_FindAudioCodecFromPSI(stElementInfo.ucStreamType, &eCodec) != ERR_OK)
							{
								/* if error happens, we make it default value */
								*pucCodecType = eDxAUDIO_CODEC_MPEG;
								HxLOG_Debug(" WARNING: no valid Audio Codec from streamType(%d) --> set default \n", stElementInfo.ucStreamType);
							}
							else
							{
								*pucCodecType = eCodec;
							}
						}
					}
					else
					{
						HxLOG_Error(" assert \n");
					}
				}
				else
				{
					HxLOG_Debug(" ERROR : fail to get elementary infor from RawData \n");
					ulErr = ERR_FAIL;
				}
			}
			else
			{
				HxLOG_Debug(" ERROR : can not find component Tag(%d) \n", nComponent);
				ulErr = ERR_FAIL;
			}
		}
		else
		{
			HxLOG_Debug(" WARNING : amount of stream count is 0 !!! \n");
			ulErr = ERR_FAIL;
		}
	}

	HxLOG_Debug(" Codec type(0x%x) pid(0x%x)  \n", *pucCodecType, *pnCompPid);
	return ulErr;

}

STATIC MhegRawPool* mheg_raw_AllocRawPool(HUINT16 usMaxRawSec, HUINT8 *szPoolName)
{
	MhegRawPool		*pstPool = NULL;

	if (usMaxRawSec == 0)
	{
		usMaxRawSec = MAX_SI_RAWSEC_NUM;
	}

	pstPool = (MhegRawPool *)HLIB_STD_MemAlloc (sizeof(MhegRawPool));
	if (pstPool == NULL)
	{
		HxLOG_Error("MhegRawPool MemAlloc Error \n");
		return NULL;
	}

	HxSTD_memset (pstPool, 0, sizeof(MhegRawPool));

	pstPool->ppucRawSec = (HUINT8 **)HLIB_STD_MemAlloc (sizeof(HUINT8 *) * usMaxRawSec);
	if (pstPool->ppucRawSec == NULL)
	{
		HxLOG_Error("MhegRawPool->ppucRawSec MemAlloc Error \n");
		HLIB_STD_MemFree (pstPool);
		return NULL;
	}

	HxSTD_memset (pstPool->ppucRawSec, 0, sizeof(HUINT8 *) * usMaxRawSec);
	pstPool->usMaxNumRawSec = usMaxRawSec;

	if (szPoolName)
	{
		HxSTD_StrNCpy (pstPool->szPoolName, szPoolName, RAW_POOL_NAME_LEN - 1);
		pstPool->szPoolName[RAW_POOL_NAME_LEN - 1] = '\0';
	}

	return pstPool;
}

#define __GLOBAL_FUNCTION__

HERROR MHEG_RAW_GetComponentInfo(HUINT16 usTSUniqId, HUINT16 usSvcId, HINT32 nComponent, HUINT8 ucElementType, HINT32 *pnCompPid, HUINT8 *pucCodecType, HUINT8 *pucStreamType, DxStereoSelect_e *pSoundMode)
{
	HUINT16 			usRawDataLen = 0;
	HUINT8			*pRawData = NULL;
	HERROR			hErr = ERR_OK;


	hErr = MHEG_SI_GetFromMHEGRawData(usTSUniqId, MHEG_DEMUX_VALUE_DONT_CARE, PMT_TABLE_ID, usSvcId, &usRawDataLen, &pRawData);
	if( hErr == ERR_OK)
	{
		if(ucElementType == ITK_ELEMENT_INDEX_PCR)
		{
			PmtProgramInfo_t	stProgramInfo;

			HxSTD_MemSet(&stProgramInfo, 0x00, sizeof(PmtProgramInfo_t));

			hErr = MHEG_SI_GetPmtProgramInfoFromRaw(pRawData, &stProgramInfo);
			if(hErr == ERR_OK)
			{
				*pnCompPid = stProgramInfo.usPCRPid;
			}
		}
		else
		{
			hErr = mheg_raw_GetPesInfo(pRawData, usRawDataLen, nComponent, ucElementType, pnCompPid, pucCodecType, pucStreamType, pSoundMode);
		}

		if(pRawData)
			HLIB_STD_MemFree(pRawData);

		if(hErr != ERR_OK)
		{
			*pnCompPid = PID_NULL;
		//	eDxVIDEO_CODEC_UNKNOWN
			*pucCodecType = eDxAUDIO_CODEC_UNKNOWN;
		}
	}

	HxLOG_Debug(" ucElementType(%d) Codec Type(0x%x), pid(0x%x) \n", ucElementType, *pucCodecType, *pnCompPid);

	return hErr;
}

HINT32	MHEG_RawMakeList_GetSemaphore(void)
{
	return VK_SEM_Get(s_ulMhegRawMakeListSemId);
}

HINT32	MHEG_RawMakeList_ReleaseSemaphore(void)
{
	return VK_SEM_Release(s_ulMhegRawMakeListSemId);
}

HINT32	MHEG_Raw_GetSemaphore(void)
{
	return VK_SEM_Get(s_ulMhegRawMgrSemId);
}

HINT32	MHEG_Raw_ReleaseSemaphore(void)
{
	return VK_SEM_Release(s_ulMhegRawMgrSemId);
}

HERROR MHEG_Raw_InitModule(void)
{
	HINT32 	nResult = 0;
	HUINT32	ulSiRawDataType;
	HCHAR	szPoolName[16];

	nResult = VK_SEM_Create(&s_ulMhegRawMgrSemId, "MhegRawMgr", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}
	nResult = VK_SEM_Create(&s_ulMhegRawMakeListSemId, "MhegRawMakeListMgr", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	for (ulSiRawDataType = 0; ulSiRawDataType < eMHEG_SI_RAW_DATA_TYPE_MAX; ulSiRawDataType++)
	{
		snprintf(szPoolName, 16, "MhegRawMgr(%d)", ulSiRawDataType);
		s_pstMhegSiRawDataPool[ulSiRawDataType] = mheg_raw_AllocRawPool (200, (HUINT8 *)szPoolName);
		if(s_pstMhegSiRawDataPool[ulSiRawDataType] == NULL)
		{
			HxLOG_Error("Failure in SI RawPool Creation \r\n");
			return ERR_FAIL;
		}

		s_pstMHEGSiRawList[ulSiRawDataType] = NULL;
	}

#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord("rawpool", MHEG_RAW_PrintRawPool,
						"printrawpool",
						"show raw pool",
						"printrawpool");
#endif

	return ERR_OK;
}


/*********************** End of File ******************************/
