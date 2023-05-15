/**
	@file     db_svcmanager.c
	@brief     관련 API

	Description: db별, 모델 별 달라지는 default 값을 처리하는 API 모음 \n
	Module: DB/default \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include	<octo_common.h>

#include	<db_svc.h>


#include	<_db_svc_service.h>
#include	<_db_svc_transponder.h>
#include	<_db_svc_group.h>
#include	<_db_svc.h>
#include	<pdb_svc.h>

#include "_db_svc_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macros          *************************/
/*******************************************************************/

#if	(DxBOUQUET_LIST_BUF_LEN > DxGROUP_LIST_BUF_LEN)
#define	IDXLIST_LEN		(DxBOUQUET_LIST_BUF_LEN / 3)
#else
#define	IDXLIST_LEN		(DxGROUP_LIST_BUF_LEN / 3)
#endif


//#define	AUTO_DYNAMIC_UPDATE_CHECK

// exception handling...
#define	__THROW_LOG(expr,log)	\
	do {\
		if (!(expr)) {\
			HxLOG_Debug("[%s] failed to operation(%s). %s\n", __FUNCTION__, #expr, log);\
			goto __CATCH;\
		}\
	} while (0)
#undef	__THROW
#define	__THROW(expr)	__THROW_LOG(expr,"")
#define	__TRY
#define	__RETURN_IF(expr,err)	\
	do {\
		if (expr) {\
			HxLOG_Debug("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return (err);\
		}\
	} while (0)
#define __RETURN_IF_VOID(expr)	\
	do {\
		if (expr) {\
			HxLOG_Debug("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return;\
		}\
	} while (0)

#define C_RETURN_IF(expr,err)	\
	do {\
		if (expr) {\
			HxLOG_Critical("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return (err);\
		}\
	} while (0)
#define C_RETURN_IF_VOID(expr)	\
	do {\
		if (expr) {\
			HxLOG_Critical("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return;\
		}\
	} while (0)

/*******************************************************************/
/********************	   Typedef		   *************************/
/*******************************************************************/

typedef void	(* dbSvc_UpdateListenerCb_t)(HINT32 nRevision, HINT32 nCount, void *pvUserData);

typedef struct
{
	HINT32						nType;

	DbSvc_Table_e				stTable;
	HINT32						nVariation;	///< -1 (deleted), 0 (updated), 1 (added)
	HINT32						nUid;		///< db index

	dbSvc_UpdateListenerCb_t	fnListener;
	void						*pvUserData;
} dbSvc_MgsMsg_t;

/*******************************************************************/
/********************	   Static Variables 	********************/
/*******************************************************************/

STATIC dbSvc_TableList_t *		s_apstList[eSvcTable_MAX];
#ifdef	AUTO_DYNAMIC_UPDATE_CHECK
STATIC HUINT32				s_ulMsgQID;
#endif

/*******************************************************************/
/**************    local function prototypes	********************/
/*******************************************************************/

/*******************************************************************/
/********************	   Functions	   *************************/
/*******************************************************************/
STATIC INLINE
HUINT32		db_svc_MgrCountBit (HUINT32 ulWord)
{
	HUINT32	i, nCount;

	if (ulWord == 0)
		return 0;

	for (i = 0, nCount = 0 ; i < 32 ; i++)
	{
		if (ulWord & (1 << i))
			nCount++;
	}
	return nCount;
}

STATIC INLINE
Handle_t	db_svc_MgrToHandle (HUINT16 usTsIdx, HUINT16 usSvcIdx)
{
	return (Handle_t)(((usTsIdx & 0xFFFF)<<16) | (usSvcIdx & 0xFFFF));
}

STATIC INLINE
HUINT16		db_svc_MgrToTsIdx (Handle_t hSvc)
{
	return (HUINT16)((hSvc >> 16) & 0xFFFF);
}

STATIC INLINE
HUINT16		db_svc_MgrToSvcIdx (Handle_t hSvc)
{
	return (HUINT16)(hSvc & 0xFFFF);
}

STATIC INLINE
dbSvc_TableList_t *	db_svc_MgrGetListItem (DbSvc_Table_e eTableIdx)
{
	if (eTableIdx < eSvcTable_MAX)
	{
		return s_apstList[eTableIdx];
	}

	return NULL;
}

STATIC INLINE
HERROR	db_svc_MgrSetListItem (DbSvc_Table_e eTableIdx, dbSvc_TableList_t *pstItem)
{
	if (eTableIdx >= eSvcTable_MAX)
	{
		return ERR_FAIL;
	}

	s_apstList[eTableIdx] = pstItem;

	return ERR_OK;
}

STATIC
Handle_t *	db_svc_MgrMakeServiceListInt (HxVector_t *pstVec, HUINT32 *pulCount)
{
	HINT32				 i, n, nCount;
	dbSvc_Service_t		*pstInfo;
	Handle_t			*ahList;

	n    = HLIB_VECTOR_Length(pstVec);
	if (n <= 0)
		return NULL;

	ahList = (Handle_t *)OxDB_Malloc(sizeof(Handle_t) * n);
	if (ahList == NULL)
		return NULL;

	for (i = 0, nCount = 0 ; i < n ; i++)
	{
		pstInfo = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstVec, i);
		if (pstInfo == NULL)
			continue;

		ahList[nCount++] = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstInfo->stData), DbSvc_GetSvcIdx(&pstInfo->stData));
	}

	*pulCount = (HUINT32)nCount;
	if (nCount == 0)
	{
		OxDB_Free(ahList);
		return NULL;
	}
	return ahList;
}


STATIC
void *	db_svc_MgrMakeDataList (HxTREE_t *pstTree, HUINT32 ulSize, HUINT32 *pulCount)
{
	HINT32			 n, nCount;
	dbSvc_Table_t	*pstInfo;
	HUINT8			*aucList;
	void			*pvIter;

	n = HLIB_TREE_Nnodes(pstTree);
	if (n <= 0)
		return NULL;

	aucList = (HUINT8 *)OxDB_Malloc(ulSize * n);
	if (aucList == NULL)
		return NULL;

	nCount = 0;
	HxTREE_EACH(pstTree, pvIter, {
		pstInfo = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstInfo == NULL)
			continue;
		HxSTD_MemCopy(aucList + (nCount * ulSize), db_svc_TableGetData(pstInfo), ulSize);
		nCount++;
	});

	if (pulCount)
		*pulCount = (HUINT32)nCount;
	if (nCount == 0)
	{
		OxDB_Free(aucList);
		return NULL;
	}
	return (void *)aucList;
}

STATIC
void *	db_svc_MgrMakeDataListFromVector (HxVector_t *pstVec, HUINT32 ulSize, HUINT32 *pulCount)
{
	HINT32			 i, n, nCount;
	dbSvc_Table_t	*pstInfo;
	HUINT8			*aucList;

	n    = HLIB_VECTOR_Length(pstVec);
	if (n <= 0)
		return NULL;

	aucList = (HUINT8 *)OxDB_Malloc(ulSize * n);
	if (aucList == NULL)
		return NULL;

	for (i = 0, nCount = 0 ; i < n ; i++)
	{
		pstInfo = (dbSvc_Table_t *)HLIB_VECTOR_ItemAt(pstVec, i);
		if (pstInfo == NULL)
			continue;

		HxSTD_MemCopy(aucList + (nCount * ulSize), db_svc_TableGetData(pstInfo), ulSize);
		nCount++;
	}

	if (pulCount)
		*pulCount = (HUINT32)nCount;
	if (nCount == 0)
	{
		OxDB_Free(aucList);
		return NULL;
	}
	return (void *)aucList;
}


STATIC
HINT32		db_svc_MgrMakeServiceListWithInfo (HxVector_t *pstVec, Handle_t **aphList, DbSvc_Info_t **apstInfoList)
{
	HUINT32	ulHandleCount;
	HUINT32	ulInfoCount;

	*aphList = db_svc_MgrMakeServiceListInt(pstVec, &ulHandleCount);
	if (*aphList == NULL)
		return 0;

	*apstInfoList = (DbSvc_Info_t *)db_svc_MgrMakeDataListFromVector(pstVec, sizeof(DbSvc_Info_t), &ulInfoCount);
	if (*apstInfoList == NULL)
	{
		OxDB_Free(*aphList);
		*aphList = NULL;
		return 0;
	}
	// if (ulHandleCount != ulInfoCount) ...
	return (HINT32)ulHandleCount;
}


STATIC
HBOOL	db_svc_MgrCompareSerivceHandle (dbSvc_Service_t *pstItem, Handle_t hSvc)
{
	Handle_t hLhs = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstItem->stData), DbSvc_GetSvcIdx(&pstItem->stData));

	if (hLhs == hSvc)
		return TRUE;
	return FALSE;
}

STATIC
HINT32	db_svc_MgrCompareTuningInfo (DxDeliveryType_e eDeliType, const DbSvc_TuningInfo_t *pstLhs, const DbSvc_TuningInfo_t *pstRhs)
{
	HINT32	diff = 0;
	HINT32	boundary = 0;

	switch (eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		diff = (HINT32)pstLhs->sat.ulSymbolRate - (HINT32)pstRhs->sat.ulSymbolRate;
		if (diff != 0)
			break;
		diff = (HINT32)pstLhs->sat.ePolarization - (HINT32)pstRhs->sat.ePolarization;
		if (diff != 0)
			break;
		diff = (HINT32)pstLhs->sat.eFecCodeRate - (HINT32)pstRhs->sat.eFecCodeRate;
		if (diff != 0)
			break;

		if (pstLhs->sat.ulSymbolRate > 18000)
			boundary = 8;
		else if (pstLhs->sat.ulSymbolRate > 11000)
			boundary = 6;
		else if (pstLhs->sat.ulSymbolRate > 8000)
			boundary = 5;
		else if (pstLhs->sat.ulSymbolRate > 4000)
			boundary = 4;
		else if (pstLhs->sat.ulSymbolRate > 2000)
			boundary = 3;
		else
			boundary = 8;

		diff = (HINT32)pstLhs->sat.ulFrequency - (HINT32)pstRhs->sat.ulFrequency;
		if (boundary >= HxMACRO_ABS(diff))
			diff = 0;
		break;

	case eDxDELIVERY_TYPE_CAB:
		diff = (HINT32)pstLhs->cab.ulFreq - (HINT32)pstRhs->cab.ulFreq;
		break;

	case eDxDELIVERY_TYPE_TER:
		diff = (HINT32)pstLhs->ter.ulFreq - (HINT32)pstRhs->ter.ulFreq;
		break;

	default:
		break;
	}
	return diff;
}

#ifdef AUTO_DYNAMIC_UPDATE_CHECK

STATIC
void 		db_svc_MgrSendTransaction (DbSvc_Table_e stTable, HINT32 nVariation, HINT32 uid)
{
	dbSvc_MgsMsg_t	msg;

	msg.nType      = 0;
	msg.stTable    = stTable;
	msg.nVariation = nVariation;
	msg.nUid       = uid;

	VK_MSG_SendTimeout((unsigned long)s_ulMsgQID, &msg, sizeof(dbSvc_MgsMsg_t), 10);
}

STATIC
void		db_svc_MgrSendListener (dbSvc_UpdateListenerCb_t fnListener, void *pvUserData)
{
	dbSvc_MgsMsg_t	msg;

	msg.nType      = 1;
	msg.fnListener  = fnListener;
	msg.pvUserData  = pvUserData;

	VK_MSG_SendTimeout((unsigned long)s_ulMsgQID, &msg, sizeof(dbSvc_MgsMsg_t), 10);
}

STATIC
void		db_svc_MgrUpdateTask (void *pvArg)
{
	struct _dbSvc_SmListener_t
	{
		dbSvc_UpdateListenerCb_t	 fnCallback;
		void		 				*pvUserData;
	};
	struct _dbSvc_SmContext_t
	{
		HINT32						nRevision;
		HINT32						anTransaction[eSvcTable_MAX];
		struct _dbSvc_SmListener_t	stSend;
		struct _dbSvc_SmListener_t	stRecv;
	} stContext;

	dbSvc_MgsMsg_t 		stMsg;
	HINT32				i, nCount;
	int					nRet;
	unsigned long		ulSemaphore;

	HxLOG_Trace();

	ulSemaphore = (unsigned long)pvArg;

	HxSTD_MemSet(&stContext, 0, sizeof(struct _dbSvc_SmContext_t));
	stContext.nRevision = 1;

	for (;;)
	{
		nRet = VK_MSG_ReceiveTimeout((unsigned long)s_ulMsgQID, &stMsg, sizeof(dbSvc_MgsMsg_t), 300);// 300은 그냥 내맘대로..
		if (nRet == VK_OK)
		{
			if (stMsg.nType == 0)
			{
				stContext.anTransaction[stMsg.stTable % eSvcTable_MAX]++;
				/**
				 * timeout의 타이밍 때문에 이 시점의 listener가 정확한 것임
				 * 그래서 stSend/stRecv fnListener 구분
				 */
 				stContext.stSend = stContext.stRecv;
			}
			else if (stMsg.nType == 1)
			{
				stContext.stRecv.fnCallback = stMsg.fnListener;
				stContext.stRecv.pvUserData = stMsg.pvUserData;

				HxLOG_Info("\n\tDYNAMIC(%s:%d): fnListener:0x%X\n", __FUNCTION__, __LINE__, (int)stMsg.fnListener);
			}
		}
		else if (nRet == VK_TIMEOUT)
		{
			// COMMIT!!
			VK_SEM_Get(ulSemaphore);
			for (i = 0, nCount = 0 ; i < eSvcTable_MAX ; i++)
			{
				if (stContext.anTransaction[i] == 0)
					continue;
				#if 0
				if (OxTABLELIST_CommitToRevision(db_svc_MgrGetListItem(i), stContext.nRevision) == ERR_OK)
					nCount += stContext.anTransaction[i];
				#endif
				stContext.anTransaction[i] = 0;
			}
			VK_SEM_Release(ulSemaphore);

			if (nCount > 0)
			{
				HxLOG_Critical("\n\tDYNAMIC(%s:%d): emit(rev:%d, count:%d)\n", __FUNCTION__, __LINE__, stContext.nRevision, nCount);
				if (stContext.stSend.fnCallback)
				{
					stContext.stSend.fnCallback(stContext.nRevision, nCount, stContext.stSend.pvUserData);
				}
				stContext.nRevision++;
			}
		}
	}
}
#endif

// ========================
// Table List Mapping
// ========================
STATIC HERROR		db_svc_MgrTableSetListener (DbSvc_Table_e eTableIdx, HUINT32 ulMaxId, dbSvc_TableListenerCb_t fnListener, void *pvUserData)
{
	dbSvc_TableList_t	*pstTableList = NULL;

	if (eTableIdx >= eSvcTable_MAX)
	{
		HxLOG_Error("[%s : %04d] Invalid Table Index [%d]\n", "Network", __FUNCTION__, __LINE__, eTableIdx);
		return ERR_FAIL;
	}

	pstTableList = db_svc_TableListNew(ulMaxId);
	if (NULL == pstTableList)
	{
		return ERR_FAIL;
	}

	pstTableList->fnListener = fnListener;
	pstTableList->pvUserData = pvUserData;

	return db_svc_MgrSetListItem(eTableIdx, pstTableList);
}

STATIC dbSvc_Table_t *db_svc_MgrTableFindItemByKey(dbSvc_TableList_t *pstTableList, HINT32 nKey)
{
	return db_svc_TableListFind(pstTableList, nKey);
}

STATIC HERROR		db_svc_MgrTableAddItem(dbSvc_TableList_t *pstList, dbSvc_Table_t *pstData)
{
	return db_svc_TableListAdd(pstList, pstData);
}

STATIC HERROR		db_svc_MgrTablePutItem(dbSvc_TableList_t *pstList, dbSvc_Table_t *pstData)
{
	return db_svc_TableListPut (pstList, pstData);
}

STATIC HUINT32		db_svc_MgrTableRemoveItemByFilter(dbSvc_TableList_t *pstList, const dbSvc_TableFilter_t *pstFilter)
{
	if (NULL == pstFilter)
	{
		return db_svc_TableListRemoveAll(pstList);
	}

	return db_svc_TableListRemoveIf(pstList, pstFilter);
}

STATIC HERROR		db_svc_MgrTableRemoveItemByKey(dbSvc_TableList_t *pstList, HINT32 nKey)
{
	return db_svc_TableListRemove(pstList, nKey);
}

STATIC HxVector_t	*db_svc_MgrTableFilterList(dbSvc_TableList_t *pstList, const dbSvc_TableFilter_t *pstFilter)
{
	return db_svc_TableListDoFiltering(pstList, pstFilter);
}

STATIC HERROR		db_svc_MgrTableUpdateItemByKey(dbSvc_TableList_t *pstList, HINT32 nKey, void *pvData)
{
	return db_svc_TableListUpdate(pstList, nKey, pvData);
}

STATIC HINT32		db_svc_MgrTableCountItem(dbSvc_TableList_t *pstList)
{
	return db_svc_TableListCount(pstList);
}

STATIC dbSvc_Table_t *	db_svc_MgrTableSetFlag(dbSvc_Table_t *pstTable, HUINT32 ulFlag, HBOOL bSet)
{
	if (pstTable)
	{
		if (bSet)
			pstTable->ulFlags |= ulFlag;
		else
			pstTable->ulFlags &= ~ulFlag;
	}
	return pstTable;
}

STATIC void		db_svc_MgrTableClearList(dbSvc_TableList_t *pstList)
{
	db_svc_TableListClear(pstList);
}

// ========================
// Service List Mapping
// ========================
STATIC dbSvc_Service_t	*db_svc_MgrServiceNew(const DbSvc_Info_t *pstSvcInfo)
{
	return db_svc_ServiceNew(pstSvcInfo);
}

STATIC dbSvc_Service_t	*db_svc_MgrServiceDelete(dbSvc_Service_t *pstService)
{
	return db_svc_ServiceDelete(pstService);
}

STATIC HxVector_t		*db_svc_MgrServiceFilterList(dbSvc_TableList_t *pstList, const dbSvc_ServiceFilter_t *pstFilter)
{
	return db_svc_ServiceDoFiltering(pstList, pstFilter);
}

STATIC dbSvc_ServiceFilter_t *db_svc_MgrServiceInitFilter(dbSvc_ServiceFilter_t *pstFilter)
{
	if (pstFilter)
	{
		HxSTD_MemSet(pstFilter, 0, sizeof(dbSvc_ServiceFilter_t));

		(void)db_svc_TableInitFilter(&pstFilter->stFilter);

		pstFilter->stFilter.ulSize = sizeof(dbSvc_Service_t);
		pstFilter->nAntIdx = -1;
		pstFilter->nOnId   = -1;
		pstFilter->nTsId   = -1;
		pstFilter->nSvcId  = -1;
		pstFilter->nTsIdx  = -1;
		pstFilter->nPrvIdx = -1;
		pstFilter->nLcnFlag = -1;
		pstFilter->nStartLcn = -1;
		pstFilter->nEndLcn = -1;
		pstFilter->nGenreIdx = -1;
		pstFilter->eSvcOpType = -1;
	}

	return pstFilter;
}

STATIC dbSvc_ServiceFilter_t *db_svc_MgrServiceConditionToFilter(dbSvc_ServiceFilter_t *pstFilter, const DbSvc_Condition_t *pstCond)
{
	if (NULL == pstFilter || NULL == pstCond)
		return NULL;

	(void)db_svc_MgrServiceInitFilter(pstFilter);

	pstFilter->nAntIdx		= pstCond->nAntIdx;
	pstFilter->nOnId		= pstCond->nOnId;
	pstFilter->nSvcId		= pstCond->nSvcId;
	pstFilter->nTsId		= pstCond->nTsId;
	pstFilter->nTsIdx		= pstCond->nTsIdx;
	pstFilter->nPrvIdx		= pstCond->nPrvIdx;
	pstFilter->nLcnFlag 	= pstCond->nLcnFlag;
	pstFilter->nStartLcn	= pstCond->nStartLcn;
	pstFilter->nEndLcn		= pstCond->nEndLcn;
	pstFilter->eSvcType 	= (DxSvcType_e)pstCond->nSvcType;
	pstFilter->eVideoType	= (DxVideoType_e)pstCond->nVideoType;
	pstFilter->eCasType 	= (DxCasType_e)pstCond->nCasType;
	pstFilter->eOrgDeliType = (DxDeliveryType_e)pstCond->nOrgDeliType;
	pstFilter->eSection 	= (DxSvcSection_e)pstCond->nSection;
	pstFilter->eSvcOpType	= (DxOperator_e)pstCond->eSvcOpType;
	pstFilter->eSatType 	= (SVC_SatType_t)pstCond->eSatType;
	pstFilter->bwUserMask1 = pstCond->eUserMask1;
	return pstFilter;
}

STATIC dbSvc_ServiceFilter_t *db_svc_MgrServiceAttrToFilter(dbSvc_ServiceFilter_t *pstFilter, const DbSvc_Attr_t *pstAttr)
{
	if (pstFilter == NULL || pstAttr == NULL)
		return NULL;

	(void)db_svc_MgrServiceInitFilter(pstFilter);

	pstFilter->eSvcType 	= pstAttr->eSvcType;
	pstFilter->eVideoType	= pstAttr->eVideoType;
	pstFilter->eSatType 	= pstAttr->eSatType;
	pstFilter->eCasType 	= pstAttr->eCasType;
	pstFilter->eOrgDeliType = pstAttr->eOrgDeliType;
	pstFilter->eSortType	= pstAttr->eSortType;
	pstFilter->eVisibleType = pstAttr->eVisibleType;
	pstFilter->nBouqIdx 	= pstAttr->nBouqIdx;
	pstFilter->nGenreIdx	= pstAttr->nGenreIdx;
	pstFilter->eSection 	= pstAttr->eSection;
	pstFilter->eSvcOpType	= pstAttr->eSvcOpType;
	pstFilter->bNewSvc		= pstAttr->bNewSvc;
	pstFilter->bwUserFlag1	= pstAttr->eUserFlag1;
	pstFilter->bwUserMask1	= pstAttr->eUserMask1;
	pstFilter->eJpNetType	= pstAttr->eJpNetType;

	return pstFilter;
}

STATIC
void		db_svc_MgrSvcUpdateListener (dbSvc_TableList_t *pstList, HINT32 nVariation, HINT32 nKey, void *pvUserData)
{
	dbSvc_Service_t	*pstService;

	HxLOG_Debug("\tSM  %15s[%4d] uid:%d\n", "Service", nVariation, nKey);

	if (nVariation == 0)	// updated!!
	{
		pstService = (dbSvc_Service_t *)db_svc_MgrTableFindItemByKey(pstList, nKey);
		if (pstService)
		{
			pdb_svc_DamaAdaptorUpdateSvcInfo(&(pstService->stData));
		}
	}
#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	db_svc_MgrSendTransaction(eSvcTable_Service, nVariation, nKey);
#endif
	(void)pvUserData;
}

STATIC
void		db_svc_MgrTsUpdateListener (dbSvc_TableList_t *pstList, HINT32 nVariation, HINT32 nKey, void *pvUserData)
{
	HxLOG_Debug("\tSM  %15s[%4d] uid:%d\n", "Transponder", nVariation, nKey);
	(void)pvUserData;

#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	db_svc_MgrSendTransaction(eSvcTable_Transponder, nVariation, nKey);
#endif

	 //TS에 해당하는 서비스를 모두 지운다
	if (nVariation < 0)
	{
		dbSvc_TableList_t *pstSvclist = db_svc_MgrGetListItem(eSvcTable_Service);
		if (pstSvclist)
		{
			dbSvc_ServiceFilter_t	stFilter;

			db_svc_MgrServiceInitFilter(&stFilter);
			stFilter.nTsIdx = (HINT32)nKey;
			(void) db_svc_MgrTableRemoveItemByFilter(pstSvclist, (dbSvc_TableFilter_t *)&stFilter);
		}
	}
}

STATIC
void		db_svc_MgrPrvUpdateListener (dbSvc_TableList_t *pstList, HINT32 nVariation, HINT32 nKey, void *pvUserData)
{
	HxLOG_Debug("\tSM  %15s[%4d] uid:%d\n", "Provider", nVariation, nKey);

	(void)pvUserData;
#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	db_svc_MgrSendTransaction(eSvcTable_Provider, nVariation, nKey);
#endif

	//Provider에 해당하는 모든 서비스를 지운다
	if (nVariation < 0)
	{
		dbSvc_TableList_t *pstSvclist = db_svc_MgrGetListItem(eSvcTable_Service);
		if (pstSvclist)
		{
			dbSvc_ServiceFilter_t	stFilter;

			db_svc_MgrServiceInitFilter(&stFilter);
			stFilter.nPrvIdx = (HINT32)nKey;
			(void) db_svc_MgrTableRemoveItemByFilter(pstSvclist, (dbSvc_TableFilter_t *)&stFilter);
		}
	}
}

STATIC
void		db_svc_MgrNetUpdateListener (dbSvc_TableList_t *pstList, HINT32 nVariation, HINT32 nKey, void *pvUserData)
{
	HxLOG_Debug("\tSM  %15s[%4d] uid:%d\n", "Network", nVariation, nKey);

	(void)pvUserData;
#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	db_svc_MgrSendTransaction(eSvcTable_Network, nVariation, nKey);
#endif

	if (nVariation < 0)
	{
		dbSvc_TableList_t *tslist = db_svc_MgrGetListItem(eSvcTable_Transponder);
		if (tslist)
		{
			dbSvc_TransponderFilter_t stFilter;

			db_svc_TransponderInitFilter(&stFilter, NULL);
			stFilter.stCond.nNetIdx = (HINT32)nKey;
			(void) db_svc_MgrTableRemoveItemByFilter(tslist, (dbSvc_TableFilter_t *)&stFilter);
		}
	}
}

STATIC
void		db_svc_MgrGrpUpdateListener (dbSvc_TableList_t *pstList, HINT32 nVariation, HINT32 nKey, void *pvUserData)
{
	HxLOG_Debug("\tSM  %15s[%4d] uid:%d\n", "Group", nVariation, nKey);

	(void)pvUserData;
#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	db_svc_MgrSendTransaction(eSvcTable_Group, nVariation, nKey);
#endif
}

STATIC
void		db_svc_MgrBqtUpdateListener (dbSvc_TableList_t *pstList, HINT32 nVariation, HINT32 nKey, void *pvUserData)
{
	HxLOG_Debug("\tSM  %15s[%4d] uid:%d\n", "Bouquet", nVariation, nKey);

	(void)pvUserData;
#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	db_svc_MgrSendTransaction(eSvcTable_Bouquet, nVariation, nKey);
#endif
}

STATIC
void		db_svc_MgrTableInit(HUINT32 ulSemId)
{
#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	HUINT32	ulTaskId;

	VK_MSG_Create(OCTO_MSGQ_LEVEL05, sizeof(dbSvc_MgsMsg_t), "SM_MSGQ", (unsigned long *)&s_ulMsgQID, VK_SUSPENDTYPE_FIFO);

	VK_TASK_Create(
		db_svc_MgrUpdateTask
		, DB_SVC_TASK_PRIORITY
		, DB_SVC_TASK_STACK_SIZE
		, "SM_TASK"
		, (void *)ulSemId
		, (unsigned long *)&ulTaskId
		, VK_SUSPENDTYPE_FIFO
	);
#endif

#if 1
	db_svc_MgrTableSetListener(eSvcTable_Service,		0x0000FFFF, (dbSvc_TableListenerCb_t)db_svc_MgrSvcUpdateListener,	NULL);
	db_svc_MgrTableSetListener(eSvcTable_Transponder,	0x0000FFFF, (dbSvc_TableListenerCb_t)db_svc_MgrTsUpdateListener,	NULL);
	db_svc_MgrTableSetListener(eSvcTable_Provider,		0x0000FFFF, (dbSvc_TableListenerCb_t)db_svc_MgrPrvUpdateListener,	NULL);
	db_svc_MgrTableSetListener(eSvcTable_Network,		0x0000FFFF, (dbSvc_TableListenerCb_t)db_svc_MgrNetUpdateListener,	NULL);
	db_svc_MgrTableSetListener(eSvcTable_Antenna,		0x000000FF, (dbSvc_TableListenerCb_t)NULL,							NULL);
	db_svc_MgrTableSetListener(eSvcTable_Group,			0x0000FFFF, (dbSvc_TableListenerCb_t)db_svc_MgrGrpUpdateListener,	NULL);
	db_svc_MgrTableSetListener(eSvcTable_Bouquet,		0x0000FFFF, (dbSvc_TableListenerCb_t)db_svc_MgrBqtUpdateListener,	NULL);
#else
	s_apstList[eSvcTable_Service]     = db_svc_TableListNew(0xFFFF);
	s_apstList[eSvcTable_Transponder] = db_svc_TableListNew(0xFFFF);
	s_apstList[eSvcTable_Provider]    = db_svc_TableListNew(0xFFFF);
	s_apstList[eSvcTable_Network]     = db_svc_TableListNew(0xFFFF);
	s_apstList[eSvcTable_Antenna]     = db_svc_TableListNew(0xFF);
	s_apstList[eSvcTable_Group]       = db_svc_TableListNew(0xFFFF);
	s_apstList[eSvcTable_Bouquet]     = db_svc_TableListNew(0xFFFF);

	db_svc_TableListSetListener(s_apstList[eSvcTable_Service]    , (dbSvc_TableListenerCb_t)db_svc_MgrSvcUpdateListener, NULL);
	db_svc_TableListSetListener(s_apstList[eSvcTable_Transponder], (dbSvc_TableListenerCb_t)db_svc_MgrTsUpdateListener, NULL);
	db_svc_TableListSetListener(s_apstList[eSvcTable_Provider]   , (dbSvc_TableListenerCb_t)db_svc_MgrPrvUpdateListener, NULL);
	db_svc_TableListSetListener(s_apstList[eSvcTable_Network]    , (dbSvc_TableListenerCb_t)db_svc_MgrNetUpdateListener, NULL);
	db_svc_TableListSetListener(s_apstList[eSvcTable_Group]      , (dbSvc_TableListenerCb_t)db_svc_MgrGrpUpdateListener, NULL);
	db_svc_TableListSetListener(s_apstList[eSvcTable_Bouquet]    , (dbSvc_TableListenerCb_t)db_svc_MgrBqtUpdateListener, NULL);
#endif

#ifdef AUTO_DYNAMIC_UPDATE_CHECK
	VK_TASK_Start((unsigned long)ulTaskId);
#endif
}


#define	____db_svc_APIs____________________________________________________

HERROR		db_svc_MgrInit(HUINT32 ulSemId)
{
	HERROR		hErr;

	db_svc_MgrTableInit(ulSemId);

	hErr = pdb_svc_Init();
	if (hErr != ERR_OK)
	{
		HxLOG_Error("pdb_svc_Init failed. \n");
	}

	return ERR_OK;
}

#ifdef	AUTO_DYNAMIC_UPDATE_CHECK
void		db_svc_SetUpdateListener (dbSvc_UpdateListenerCb_t fnListener, void *pvUserData)
{
	HxLOG_Trace();

	db_svc_MgrSendListener(fnListener, pvUserData);
}
#endif

void		db_svc_MgrCommit (HINT32 n, ...)
{
	dbSvc_TableList_t	*pstList;
	va_list				 ap;
	DbSvc_Table_e		 eTableIdx;

	HxLOG_Trace();

	va_start(ap, n);
	while (n--)
	{
		eTableIdx = va_arg(ap, HINT32);
		pstList = db_svc_MgrGetListItem(eTableIdx);
		if(pstList)
			db_svc_TableListCommit(pstList);
	}
	va_end(ap);
}


HBOOL		db_svc_MgrHasChanged (HBOOL bCheckItemCount, HINT32 n, ...)
{
	dbSvc_TableList_t	*pstList;
	va_list				 ap;
	DbSvc_Table_e		 eTableIdx;
	HUINT32				 ulTableCount = 0;

	HxLOG_Trace();

	va_start(ap, n);
	while (n--)
	{
		eTableIdx = va_arg(ap, HINT32);
		pstList = db_svc_MgrGetListItem(eTableIdx);
		if(pstList)
		{
			ulTableCount++;
			if (db_svc_TableListHasChanged(pstList))
			{
				va_end(ap);
				return TRUE;
			}
		}
	}
	va_end(ap);

	if (bCheckItemCount)
	{
		if(ulTableCount == 0)
		{
			/* network, ts, service, provider 모든 정보가 delete된 경우에 meta_service 적용을 위해 update를 발생시켜 줘야 한다. */
			return TRUE;
		}
	}

	return FALSE;
}


HERROR		db_svc_MgrGetChanged (DbSvc_Table_e eTable, void **apvUpdateList, HINT32 *pnUpdateNum, void **apvRemoveList, HINT32 *pnRemoveNum)
{
	dbSvc_TableList_t		*pstList;
	HxVector_t				*pstRev;
	STATIC const HUINT32	s_ulSizeOf[] = {
		sizeof(DbSvc_Info_t),
		sizeof(DbSvc_TsInfo_t),
		sizeof(DbSvc_PrvInfo_t),
		sizeof(DbSvc_NetInfo_t),
		sizeof(DbSvc_GroupInfo_t),
		sizeof(DbSvc_BouqInfo_t),
		sizeof(DbSvc_AntInfo_t)
	};

	HxLOG_Trace();


	pstList = db_svc_MgrGetListItem(eTable);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	if (apvUpdateList && pnUpdateNum)
	{
		pstRev   = db_svc_TableListGetChanged(pstList);
		if (pstRev)
		{
			*apvUpdateList = db_svc_MgrMakeDataListFromVector(pstRev, s_ulSizeOf[eTable], pnUpdateNum);
			HLIB_VECTOR_Delete(pstRev);
		}
		else
		{
			*apvUpdateList	= NULL;
			*pnUpdateNum	= 0;
		}
	}
	if (apvRemoveList && pnRemoveNum)
	{
		pstRev   = db_svc_TableListGetRemoved(pstList);
		if (pstRev)
		{
			*apvRemoveList = db_svc_MgrMakeDataListFromVector(pstRev, s_ulSizeOf[eTable], pnRemoveNum);
			HLIB_VECTOR_Delete(pstRev);
		}
		else
		{
			*apvRemoveList	= NULL;
			*pnRemoveNum	= 0;
		}
	}
	return ERR_OK;
}

void		db_svc_MgrClearAllInfo(void)
{
	HINT32				 n;
	dbSvc_TableList_t	*pstList;

	n = eSvcTable_MAX;
	while (n--)
	{
		if (n == eSvcTable_Antenna)
			continue;

		pstList = db_svc_MgrGetListItem(n);
		if (pstList == NULL)
			continue;

		db_svc_MgrTableClearList(pstList);
	}
}

HERROR		db_svc_MgrAddServiceInfo(DbSvc_Info_t *pstSvcInfo, Handle_t *phSvc)
{
	dbSvc_Service_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	__TRY
	{
		//	db_svc_AddServiceInfo에서 무조건 eDbSvc_TableFlag_SEARCH Flag를 세우는 것으로 봐서는 해당 함수가 Add New Service에 해당 하는 경우에만 이쪽으로 오는것 같음.
		//	다시 말하면 OCTO내에서 Add Service가 이루어 졌을때 이루어 지는 것으로 보임.
		//	우선 해당 함수에서 eUserFlag1의 Searched Flag를 세워주고 추후에 1:1로 세워주는 식으로 변경이 필요함
		DbSvc_SetSvcUpdated(pstSvcInfo, TRUE);		//		pstSvcInfo->eUserFlag1 |= eSvcUserFlag_32_NewService;

		__THROW( (pstInfo = db_svc_MgrServiceNew(pstSvcInfo)) != NULL );
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );

		(void) db_svc_MgrTableSetFlag((dbSvc_Table_t *)pstInfo, eDbSvc_TableFlag_SEARCH, TRUE);

		__THROW( db_svc_MgrTableAddItem(pstList, (dbSvc_Table_t *)pstInfo) == ERR_OK );

		if (phSvc)
			*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstInfo->stData), DbSvc_GetSvcIdx(&pstInfo->stData));
		return ERR_OK;
	}
	__CATCH:
	if (phSvc) *phSvc = HANDLE_NULL;
	if (pstInfo) db_svc_MgrServiceDelete(pstInfo);
	return ERR_FAIL;
}

HERROR		db_svc_MgrAddNetworkInfo(DbSvc_NetInfo_t *pstNetworkInfo, HUINT16 *pusNetIdx)
{
	dbSvc_Network_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	__TRY
	{
		__THROW( (pstInfo = db_svc_NetworkNew(pstNetworkInfo)) != NULL );
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Network)) != NULL );

		(void) db_svc_MgrTableSetFlag((dbSvc_Table_t *)pstInfo, eDbSvc_TableFlag_SEARCH, TRUE);

		__THROW( db_svc_MgrTableAddItem(pstList, (dbSvc_Table_t *)pstInfo) == ERR_OK );

		if (pusNetIdx)
			*pusNetIdx = pstInfo->stData.usNetIdx;
		return ERR_OK;
	}
	__CATCH:
	if (pusNetIdx) *pusNetIdx = 0;
	if (pstInfo) db_svc_NetworkDelete(pstInfo);
	return ERR_FAIL;
}

HERROR		db_svc_MgrAddTsInfo(DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusTsIdx)
{
	dbSvc_Transponder_t	*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pstTsInfo);
	__TRY
	{
		__THROW( (pstInfo = db_svc_TransponderNew(pstTsInfo)) != NULL );
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Transponder)) != NULL );

		(void) db_svc_MgrTableSetFlag((dbSvc_Table_t *)pstInfo, eDbSvc_TableFlag_SEARCH, TRUE);

		__THROW( db_svc_MgrTableAddItem(pstList, (dbSvc_Table_t *)pstInfo) == ERR_OK );

		if (pusTsIdx)
			*pusTsIdx = pstInfo->stData.usTsIdx;
		pstInfo->stData.usUniqueId = pstInfo->stData.usTsIdx;	// 이거.. 일단 여기에 넣자
		return ERR_OK;
	}
	__CATCH:
	if (pusTsIdx) *pusTsIdx = 0;
	if (pstInfo) db_svc_TransponderDelete(pstInfo);
	return ERR_FAIL;
}

HERROR		db_svc_MgrAddProviderInfo(DbSvc_PrvInfo_t *pstPrvInfo, HUINT16 *pusPrvIdx)
{
	dbSvc_Provider_t	*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pstPrvInfo);
	__TRY
	{
		__THROW( (pstInfo = db_svc_ProviderNew(pstPrvInfo)) != NULL );
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Provider)) != NULL );

		(void) db_svc_MgrTableSetFlag((dbSvc_Table_t *)pstInfo, eDbSvc_TableFlag_SEARCH, TRUE);

		__THROW( db_svc_MgrTableAddItem(pstList, (dbSvc_Table_t *)pstInfo) == ERR_OK );

		if (pusPrvIdx)
			*pusPrvIdx = pstInfo->stData.usPrvIdx;
		return ERR_OK;
	}
	__CATCH:
	if (pusPrvIdx) *pusPrvIdx = 0;
	if (pstInfo) db_svc_ProviderDelete(pstInfo);
	return ERR_FAIL;
}


HUINT32		db_svc_MgrPutServiceInfoList (const DbSvc_Info_t *pstList, HUINT32 nListNum)
{
	dbSvc_Service_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstTablelist = NULL;
	HUINT32				 nCount = 0;

	HxLOG_Assert(pstList && nListNum);

	pstTablelist = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstTablelist)
	{
		for ( ; nListNum-- ; pstList++)
		{
			pstInfo = db_svc_MgrServiceNew(pstList);
			if (pstInfo == NULL)
				continue;

			if (db_svc_MgrTablePutItem(pstTablelist, (dbSvc_Table_t *)pstInfo) == ERR_OK)
				nCount++;
		}
	}
	return nCount;
}

HUINT32		db_svc_MgrPutNetworkInfoList (const DbSvc_NetInfo_t *pstList, HUINT32 nListNum)
{
	dbSvc_Network_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstTablelist = NULL;
	HUINT32				 nCount = 0;

	HxLOG_Assert(pstList && nListNum);

	pstTablelist = db_svc_MgrGetListItem(eSvcTable_Network);
	if (pstTablelist)
	{
		for ( ; nListNum-- ; pstList++)
		{
			pstInfo = db_svc_NetworkNew(pstList);
			if (pstInfo == NULL)
				continue;

			if (db_svc_MgrTablePutItem(pstTablelist, (dbSvc_Table_t *)pstInfo) == ERR_OK)
				nCount++;
		}
	}
	return nCount;
}

HUINT32		db_svc_MgrPutTsInfoList (const DbSvc_TsInfo_t *pstList, HUINT32 nListNum)
{
	dbSvc_Transponder_t *pstInfo = NULL;
	dbSvc_TableList_t	*pstTablelist = NULL;
	HUINT32 			 nCount = 0;

	HxLOG_Assert(pstList && nListNum);

	pstTablelist = db_svc_MgrGetListItem(eSvcTable_Transponder);
	if (pstTablelist)
	{
		for ( ; nListNum-- ; pstList++)
		{
			pstInfo = db_svc_TransponderNew(pstList);
			if (pstInfo == NULL)
				continue;

			if (db_svc_MgrTablePutItem(pstTablelist, (dbSvc_Table_t *)pstInfo) == ERR_OK)
			{
				nCount++;
				pstInfo->stData.usUniqueId = pstInfo->stData.usTsIdx;
			}
		}
	}
	return nCount;
}

HUINT32		db_svc_MgrPutProviderInfoList (const DbSvc_PrvInfo_t *pstList, HUINT32 nListNum)
{
	dbSvc_Provider_t 	*pstInfo = NULL;
	dbSvc_TableList_t	*pstTablelist = NULL;
	HUINT32 			 nCount = 0;

	HxLOG_Assert(pstList && nListNum);

	pstTablelist = db_svc_MgrGetListItem(eSvcTable_Provider);
	if (pstTablelist)
	{
		for ( ; nListNum-- ; pstList++)
		{
			pstInfo = db_svc_ProviderNew(pstList);
			if (pstInfo == NULL)
				continue;

			if (db_svc_MgrTablePutItem(pstTablelist, (dbSvc_Table_t *)pstInfo) == ERR_OK)
				nCount++;
		}
	}
	return nCount;
}


HUINT32		db_svc_MgrPutGroupInfoList (const DbSvc_GroupInfo_t *pstList, HUINT32 nListNum)
{
	dbSvc_Group_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstTablelist = NULL;
	HUINT32 			 nCount = 0;

	HxLOG_Assert(pstList && nListNum);

	pstTablelist = db_svc_MgrGetListItem(eSvcTable_Group);
	if (pstTablelist)
	{
		for ( ; nListNum-- ; pstList++)
		{
			pstInfo = db_svc_GroupNew(pstList);
			if (pstInfo == NULL)
				continue;

			if (db_svc_MgrTablePutItem(pstTablelist, (dbSvc_Table_t *)pstInfo) == ERR_OK)
				nCount++;
		}
	}
	return nCount;
}

HUINT32		db_svc_MgrPutBouquetInfoList (const DbSvc_BouqInfo_t *pstList, HUINT32 nListNum)
{
	dbSvc_Bouquet_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstTablelist = NULL;
	HUINT32 			 nCount = 0;

	HxLOG_Assert(pstList && nListNum);

	pstTablelist = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	if (pstTablelist)
	{
		for ( ; nListNum-- ; pstList++)
		{
			pstInfo = db_svc_BouquetNew(pstList);
			if (pstInfo == NULL)
				continue;

			if (db_svc_MgrTablePutItem(pstTablelist, (dbSvc_Table_t *)pstInfo) == ERR_OK)
				nCount++;
		}
	}
	return nCount;
}

void       db_svc_MgrDeleteServiceList(DbSvc_Condition_t *pstCond)
{
	dbSvc_ServiceFilter_t	stFilter;
	dbSvc_TableList_t		*pstList;

	HxLOG_Assert(pstCond);

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return;
	}

	(void) db_svc_MgrTableRemoveItemByFilter(
			  pstList
			, (dbSvc_TableFilter_t *)db_svc_MgrServiceConditionToFilter(&stFilter, pstCond)
		);
}

void       db_svc_MgrDeleteAllSearchingService(void)
{
	HINT32				 i;
	dbSvc_TableList_t	*pstTabls;
	dbSvc_TableFilter_t	 stFilter;

	(void)db_svc_TableInitFilter(&stFilter);
	stFilter.ulFlags |= eDbSvc_TableFlag_SEARCH;

	for (i = 0 ; i < eSvcTable_MAX ; i++)
	{
		pstTabls = db_svc_MgrGetListItem(i);
		if (pstTabls == NULL)
			continue;

		(void) db_svc_MgrTableRemoveItemByFilter(pstTabls, &stFilter);
	}
}

void       db_svc_MgrRemoveSearchFlag(void)
{
	HINT32				 i;
	dbSvc_Table_t		*pstItem;
	dbSvc_TableList_t	*pstTabls;
	void				*pvIter;

	for (i = 0 ; i < eSvcTable_MAX ; i++)
	{
		pstTabls = db_svc_MgrGetListItem(i);
		if (pstTabls == NULL)
			continue;

		HxTREE_EACH(pstTabls->pstTree, pvIter, {
			pstItem = (dbSvc_Table_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem)
				db_svc_MgrTableSetFlag(pstItem, eDbSvc_TableFlag_SEARCH, FALSE);
		});
	}
}

HERROR     db_svc_MgrDeleteAllNetworkService(DxDeliveryType_e eOrgDeliType)
{
	dbSvc_TableList_t		*pstTabls;
	dbSvc_ServiceFilter_t	 stFilter;

	(void)db_svc_MgrServiceInitFilter(&stFilter);
	stFilter.eOrgDeliType = eOrgDeliType;

	pstTabls = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstTabls == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	(void) db_svc_MgrTableRemoveItemByFilter(pstTabls, (dbSvc_TableFilter_t *)&stFilter);

	return ERR_OK;
}

HERROR		db_svc_MgrDeleteServiceInfoOnly(Handle_t hSvc)
{
	dbSvc_TableList_t	*pstList;

	if (hSvc == (Handle_t)NULL || hSvc == HANDLE_NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	return db_svc_MgrTableRemoveItemByKey(pstList, (HINT32)db_svc_MgrToSvcIdx(hSvc));
}

HERROR		db_svc_MgrDeleteServiceInfo(Handle_t hSvc)
{
	HERROR					 hErr;
	dbSvc_ServiceFilter_t	 stFilter;
	HxVector_t				*pstSameTs;
	dbSvc_TableList_t		*pstList;
	dbSvc_Service_t			*pstSelf = NULL;
	HxVector_t				*pstSamePrv;
	DbSvc_Info_t			 stInfo;
	HUINT16					 usPrvIdx = (-1);

	if (hSvc == (Handle_t)NULL || hSvc == HANDLE_NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Service_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)db_svc_MgrToSvcIdx(hSvc));
	if (pstSelf)
	{
		HxSTD_MemCopy(&stInfo, &pstSelf->stData, sizeof(DbSvc_Info_t));
		usPrvIdx = DbSvc_GetPrvIdx(&stInfo);
	}

	hErr = db_svc_MgrDeleteServiceInfoOnly(hSvc);
	if (hErr != ERR_OK)
		return hErr;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	if(usPrvIdx != (HUINT16)(-1))
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.nPrvIdx = (HINT32)usPrvIdx;
		pstSamePrv = db_svc_MgrTableFilterList(pstList, (const dbSvc_TableFilter_t *)&stFilter);
		if (pstSamePrv)
			HLIB_VECTOR_Delete(pstSamePrv);

		if (pstSamePrv == NULL)
		{
			hErr |= db_svc_MgrDeleteProviderInfo((HUINT16)stFilter.nPrvIdx);
		}
	}

	db_svc_MgrServiceInitFilter(&stFilter);
	stFilter.nTsIdx = (HINT32)db_svc_MgrToTsIdx(hSvc);

	pstSameTs = db_svc_MgrTableFilterList(pstList, (const dbSvc_TableFilter_t *)&stFilter);
	if (pstSameTs)
		HLIB_VECTOR_Delete(pstSameTs);

	if (pstSameTs == NULL)
	{
		hErr |= db_svc_MgrDeleteTsInfo((HUINT16)stFilter.nTsIdx);
	}
	return hErr;
}

HERROR		db_svc_MgrDeleteAllServiceInfo(void) // by Moon
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	return db_svc_MgrTableRemoveItemByFilter(pstList, NULL);
}

HERROR		db_svc_MgrDeleteNetworkInfoOnly(HUINT16 usNetIdx)
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Network);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Network).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	return db_svc_MgrTableRemoveItemByKey(pstList, (HINT32)usNetIdx);
}

HERROR		db_svc_MgrDeleteAllNetworkInfo(void) // by Moon
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Network);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	return db_svc_MgrTableRemoveItemByFilter(pstList, NULL);
}

HERROR		db_svc_MgrDeleteTsInfo(HUINT16 usTsIdx)
{
	dbSvc_Transponder_t			*pstTs;
	dbSvc_TableList_t			*pstList;
	HUINT16						 usNetIdx;
	HxVector_t					*pstSameNet;
	dbSvc_TransponderFilter_t	 stFilter;

	pstList = db_svc_MgrGetListItem(eSvcTable_Transponder);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Transponder).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstTs = (dbSvc_Transponder_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usTsIdx);
	if (pstTs == NULL)
	{
		HxLOG_Debug("[%s:%d] There is no transponder(idx:%d).\n", __HxFILE__, __HxLINE__, usTsIdx);
		return ERR_FAIL;
	}

	usNetIdx = pstTs->stData.usNetIdx;

	(void) db_svc_MgrTableRemoveItemByKey(pstList, (HINT32)usTsIdx);
	(void) db_svc_TransponderInitFilter(&stFilter, NULL);
	stFilter.stCond.nNetIdx = (HINT32)usNetIdx;

	pstSameNet = db_svc_MgrTableFilterList(pstList, (const dbSvc_TableFilter_t *)&stFilter);
	if (pstSameNet)
	{
		usNetIdx = 0;
		HLIB_VECTOR_Delete(pstSameNet);
	}

	if (usNetIdx)// 같은 usNetIdx를 갖는 ts가 하나도 없으면, network도 지운다.
	{
		(void)db_svc_MgrDeleteNetworkInfoOnly(usNetIdx);
	}
	return ERR_OK;
}

HERROR		db_svc_MgrDeleteAllTsInfo(void) // by Moon
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Transponder);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	return db_svc_MgrTableRemoveItemByFilter(pstList, NULL);
}

HERROR		db_svc_MgrDeleteProviderInfo(HUINT16 usPrvIdx)
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Provider);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Provider).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	return db_svc_MgrTableRemoveItemByKey(pstList, (HINT32)usPrvIdx);
}

HERROR		db_svc_MgrDeleteAllProviderInfo(void) // by Moon
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Provider);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	return db_svc_MgrTableRemoveItemByFilter(pstList, NULL);
}

HERROR     db_svc_MgrUpdateServiceInfo(Handle_t hSvc, DbSvc_Info_t *pstSvcInfo)
{
	dbSvc_TableList_t	*pstList;

	if (hSvc == (Handle_t)NULL || hSvc == HANDLE_NULL || pstSvcInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	DbSvc_SetTsIdx(pstSvcInfo, db_svc_MgrToTsIdx(hSvc));
	DbSvc_SetSvcIdx(pstSvcInfo, db_svc_MgrToSvcIdx(hSvc));

	return db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)db_svc_MgrToSvcIdx(hSvc), pstSvcInfo);
}

HERROR     db_svc_MgrUpdateNetworkInfo(HUINT16 usNetIdx, DbSvc_NetInfo_t *pstNetworkInfo)
{
	dbSvc_TableList_t	*pstList;

	if (pstNetworkInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Network);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Network).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstNetworkInfo->usNetIdx = usNetIdx;

	return db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)usNetIdx, pstNetworkInfo);
}

HERROR     db_svc_MgrUpdateTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo)
{
	dbSvc_TableList_t	*pstList;

	if (pstTsInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Transponder);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Transponder).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstTsInfo->usTsIdx = usTsIdx;

	return db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)usTsIdx, pstTsInfo);
}

HERROR     db_svc_MgrUpdateProviderInfo(HUINT16 usPrvIdx, DbSvc_PrvInfo_t *pstPrvInfo)
{
	dbSvc_TableList_t	*pstList;

	if (pstPrvInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Provider);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Provider).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstPrvInfo->usPrvIdx = usPrvIdx;

	return db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)usPrvIdx, pstPrvInfo);
}

HERROR     db_svc_MgrUpdateServiceInfoEx(Handle_t hSvc, DbSvc_FieldList_b bwSvcField, DbSvc_FieldListExt_b bwSvcFieldExt, DbSvc_Info_t *pstSvcInfo)
{
	dbSvc_Service_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr  = ERR_FAIL;

	if (pstSvcInfo == NULL)
		return ERR_FAIL;

	if (bwSvcField == eSvcField_All)
	{
		return db_svc_MgrUpdateServiceInfo(hSvc, pstSvcInfo);
	}

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Service_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)db_svc_MgrToSvcIdx(hSvc));
	if (pstSelf)
	{
		DbSvc_Info_t	stInfo;

		HxSTD_MemCopy(&stInfo, &pstSelf->stData, sizeof(DbSvc_Info_t));
		if(bwSvcField & eSvcField_01_usLcn)					DbSvc_SetLcn(&stInfo, DbSvc_GetLcn(pstSvcInfo));
		if(bwSvcField & eSvcField_02_szSvcName)				DbSvc_SetName(&stInfo, DbSvc_GetName(pstSvcInfo));
		if(bwSvcField & eSvcField_03_bModifiedName)			DbSvc_SetRenamed(&stInfo, DbSvc_GetRenamed(pstSvcInfo));
		if(bwSvcField & eSvcField_04_bLocked)				DbSvc_SetLocked(&stInfo, DbSvc_GetLocked(pstSvcInfo));
		if(bwSvcField & eSvcField_05_eUserFlag1)			(void)DB_SVC_ServiceCopyUserFlag(&stInfo, (const DbSvc_Info_t *)pstSvcInfo);
		if(bwSvcField & eSvcField_06_ulRatingPassedEventId)	DbSvc_SetRatingPassedEvent(&stInfo, DbSvc_GetRatingPassedEvent(pstSvcInfo));
		if(bwSvcField & eSvcField_07_bAudUserSetFlag)		DbSvc_SetAudUserSelected(&stInfo, DbSvc_GetAudUserSelected(pstSvcInfo));
		if(bwSvcField & eSvcField_08_bSubttlUserFlag)		DbSvc_SetSubttlFlag(&stInfo, DbSvc_GetSubttlFlag(pstSvcInfo));

		hErr = db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)db_svc_MgrToSvcIdx(hSvc), &stInfo);
	}
	return hErr;
}

HERROR     db_svc_MgrGetServiceInfo(Handle_t hSvc, DbSvc_Info_t *pstSvcInfo)
{
	dbSvc_Service_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr = ERR_FAIL;

	if (pstSvcInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Service_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)db_svc_MgrToSvcIdx(hSvc));
	if (pstSelf)
	{
		HxSTD_MemCopy(pstSvcInfo, &pstSelf->stData, sizeof(DbSvc_Info_t));
		hErr = ERR_OK;
	}
	return hErr;
}


HERROR     db_svc_MgrGetTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo)
{
	dbSvc_Transponder_t	*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr = ERR_FAIL;

	if (pstTsInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Transponder);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Transponder).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Transponder_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usTsIdx);
	if (pstSelf)
	{
		HxSTD_MemCopy(pstTsInfo, &pstSelf->stData, sizeof(DbSvc_TsInfo_t));
		hErr = ERR_OK;
	}
	return hErr;
}


HERROR     db_svc_MgrFindTsInfoList(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt, DbSvc_TsInfo_t **ppstInfoList)
{
	dbSvc_TableList_t			*pstList = NULL;
	HxVector_t					*pstResult = NULL;
	DbSvc_TsInfo_t				*pstTsList = NULL;
	HINT32						 nCount;
	dbSvc_TransponderFilter_t	 stFilter;

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Transponder)) != NULL );

		(void) db_svc_TransponderInitFilter(&stFilter, pstCond);

		__THROW( (pstResult = db_svc_MgrTableFilterList(pstList, (const dbSvc_TableFilter_t *)&stFilter)) != NULL );
		__THROW( (pstTsList = (DbSvc_TsInfo_t *)db_svc_MgrMakeDataListFromVector(pstResult, sizeof(DbSvc_TsInfo_t), &nCount)) != NULL);
		HLIB_VECTOR_Delete(pstResult);

		if (pnCnt)
			*pnCnt = nCount;
		if (ppstInfoList)
			*ppstInfoList = pstTsList;
		else
			OxDB_Free(pstTsList);

		return ERR_OK;
	}
	__CATCH:
	if (pnCnt) *pnCnt = 0;
	if (ppstInfoList) *ppstInfoList = NULL;
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	if (pstTsList) OxDB_Free(pstTsList);
	return ERR_FAIL;
}


HERROR     db_svc_MgrGetNetworkInfo(HUINT16 usNetIdx, DbSvc_NetInfo_t *pstNetworkInfo)
{
	dbSvc_Network_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr = ERR_FAIL;

	if (pstNetworkInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Network);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Network).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Network_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usNetIdx);
	if (pstSelf)
	{
		HxSTD_MemCopy(pstNetworkInfo, &pstSelf->stData, sizeof(DbSvc_NetInfo_t));
		hErr = ERR_OK;
	}
	return hErr;
}


HERROR     db_svc_MgrGetPrvInfo(HUINT16 usPrvIdx, DbSvc_PrvInfo_t *pstPrvInfo)
{
	dbSvc_Provider_t	*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr = ERR_FAIL;

	if (pstPrvInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Provider);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Provider).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Provider_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usPrvIdx);
	if (pstSelf)
	{
		HxSTD_MemCopy(pstPrvInfo, &pstSelf->stData, sizeof(DbSvc_PrvInfo_t));
		hErr = ERR_OK;
	}
	return hErr;
}

HERROR     db_svc_MgrGetPrvInfoByServiceHandle(Handle_t hSvc, DbSvc_PrvInfo_t *pstPrvInfo)
{
	DbSvc_Info_t	stInfo;

	if (db_svc_MgrGetServiceInfo(hSvc, &stInfo) == ERR_OK)
	{
		return db_svc_MgrGetPrvInfo(DbSvc_GetPrvIdx(&stInfo), pstPrvInfo);
	}
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetServiceCount(
								DbSvc_Group_e 	 eSvcGroup, 	/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 	*pstAttr, 	/**< [in]  filtering 조건 2 - 서비스 속성 */
								HINT32 			*pnCnt)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList = NULL;
	HxVector_t				*pstResult;

	HxLOG_Assert(pstAttr && pnCnt);

	__TRY
	{
		__THROW( eSvcGroup == eSvcGroup_All );
		__THROW( db_svc_MgrServiceAttrToFilter(&stFilter, pstAttr) );
		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrTableFilterList(pstList, (const dbSvc_TableFilter_t *)&stFilter)) != NULL );

		*pnCnt = HLIB_VECTOR_Length(pstResult);
		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	*pnCnt = 0;
	return ERR_FAIL;
}


HERROR	   db_svc_MgrGetServiceHandleBySvcIdx(HUINT16 usSvcIdx, Handle_t *phSvc)
{
	dbSvc_Service_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr = ERR_FAIL;

	if (phSvc == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Service).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Service_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usSvcIdx);
	if (pstSelf)
	{
		*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstSelf->stData), usSvcIdx);
		hErr = ERR_OK;
	}
	return hErr;
}

HERROR     db_svc_MgrGetServiceHandleByNumber(DxDeliveryType_e eOrgDeliType,
							DxSvcType_e eSvcType, HINT32 nChNum, Handle_t *phSvc)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_Service_t 		*pstInfo;
	dbSvc_TableList_t		*pstList   = NULL;
	HxVector_t				*pstResult = NULL;

	HxLOG_Assert(phSvc);

	*phSvc = HANDLE_NULL;

	__TRY
	{
		// base level에서 막기 전 APS에서 확인 중. 0인 채널넘버가 존재할 수 있고, 튜닝 불가의 경우, 별도 처리 필요
		// channel number가 0인 경우는 에러 처리함
		__THROW( nChNum != 0 );

		// 하나의 delivery type에 대해서만 지원함
		__THROW( db_svc_MgrCountBit((HUINT32)eOrgDeliType) == 1 );

		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.eSvcType     = eSvcType;
		stFilter.nStartLcn    = nChNum;
		stFilter.nEndLcn      = nChNum;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (pstInfo   = (dbSvc_Service_t *)HLIB_VECTOR_First(pstResult)) != NULL );

		*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstInfo->stData), DbSvc_GetSvcIdx(&pstInfo->stData));

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetServiceIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusSvcId)
{
	DbSvc_Info_t	stInfo;

	if (db_svc_MgrGetServiceInfo(hSvc, &stInfo) == ERR_OK && pusSvcId)
	{
		*pusSvcId = DbSvc_GetSvcId(&stInfo);
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetServiceHandleByOnIdAndSvcId(DxSvcSection_e eSvcSection, HUINT16 usOnId, HUINT16 usSvcId, Handle_t *phSvc)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_Service_t			*pstItem;

	HxLOG_Assert(phSvc);

	*phSvc = HANDLE_NULL;

	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eSection 	= (DxSvcSection_e)eSvcSection;
		stFilter.nOnId	= (HINT32)usOnId;
		stFilter.nSvcId	= (HINT32)usSvcId;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (pstItem   = HLIB_VECTOR_First(pstResult)) != NULL);

		*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstItem->stData), DbSvc_GetSvcIdx(&pstItem->stData));

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetUniqueIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusUniqueId)
{
	if (pusUniqueId == NULL)
		return ERR_FAIL;

	*pusUniqueId = db_svc_MgrToTsIdx(hSvc);
	return ERR_OK;
}

HERROR     db_svc_MgrGetServiceHandle(DxSvcSection_e eSvcSection, HUINT16 usTsIdx, HUINT16 usSvcId, Handle_t *phSvc)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_Service_t 		*pstInfo;
	dbSvc_TableList_t		*pstList   = NULL;
	HxVector_t				*pstResult = NULL;

	HxLOG_Assert(phSvc);

	*phSvc = HANDLE_NULL;

	db_svc_MgrServiceInitFilter(&stFilter);
	stFilter.eSection = eSvcSection;
	stFilter.nTsIdx   = (HINT32)usTsIdx;
	stFilter.nSvcId   = (HINT32)usSvcId;

	__TRY
	{
		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (pstInfo   = (dbSvc_Service_t *)HLIB_VECTOR_First(pstResult)) != NULL );

		*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstInfo->stData), (HUINT16)DbSvc_GetSvcIdx(&pstInfo->stData));

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}


HERROR     db_svc_MgrFindServiceListByNumber(DxDeliveryType_e 		  eOrgDeliType,	/**< [in]  filtering 조건 1 - delivery type: original delivery type, all 포함 sat/ter/.. 등 OR 가능 */
											DxSvcType_e 			  eSvcType, 	/**< [in]  filtering 조건 2 - service type */
											DbSvc_VisibleType_e 	  eVisibleType, /**< [in]  filtering 조건 3 - visible type */
											HINT32 					  nChNum, 		/**< [in]  filtering 조건 4 - 검색할 channel number */
											HUINT32 				 *pulSvcCount, 	/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
											Handle_t 				**pphSvcList)	/**< [out] service handle list를 담아올 포인터. 받드시 db_svc_FreeServiceList()로 free 해줘야함 */
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList   = NULL;
	HxVector_t				*pstResult = NULL;

	HxLOG_Assert(pulSvcCount && pphSvcList);

	*pulSvcCount = 0;
	*pphSvcList  = NULL;

	__TRY
	{
	// base level에서 막기 전 APS에서 확인 중. 0인 채널넘버가 존재할 수 있고, 튜닝 불가의 경우, 별도 처리 필요
	// channel number가 0인 경우는 에러 처리함
		__THROW( nChNum != 0 );

		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.eSvcType 	= eSvcType;
		stFilter.eVisibleType = eVisibleType;
		stFilter.nStartLcn	= nChNum;
		stFilter.nEndLcn		= nChNum;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (*pphSvcList = db_svc_MgrMakeServiceListInt(pstResult, pulSvcCount)) != NULL );

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult)
		HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetTsCount(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt)
{
	dbSvc_TransponderFilter_t	 stFilter;
	dbSvc_TableList_t			*pstList   = NULL;
	HxVector_t					*pstResult = NULL;

	HxLOG_Assert(pstCond && pnCnt);

	*pnCnt = 0;
	db_svc_TransponderInitFilter(&stFilter, pstCond);

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Transponder)) != NULL );

		pstResult = db_svc_MgrTableFilterList(pstList, (const dbSvc_TableFilter_t *)&stFilter);
		if (pstResult)
		{
			*pnCnt = HLIB_VECTOR_Length(pstResult);
			HLIB_VECTOR_Delete(pstResult);
		}
		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetNetworkCount(HINT32 *pnCnt)
{
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pnCnt);

	*pnCnt = 0;
	pstList = db_svc_MgrGetListItem(eSvcTable_Network);
	if (pstList)
	{
		*pnCnt = db_svc_MgrTableCountItem(pstList);
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetProviderCount(HINT32 *pnCnt)
{
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pnCnt);

	*pnCnt = 0;
	pstList = db_svc_MgrGetListItem(eSvcTable_Provider);
	if (pstList)
	{
		*pnCnt = db_svc_MgrTableCountItem(pstList);
		return ERR_OK;
	}
	return ERR_FAIL;
}


HERROR     db_svc_MgrFindServiceHandle(
								DbSvc_Group_e 			 eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			*pstAttr, 			/**< [in]  filtering 조건 2 - 서비스 속성 */
								Handle_t 				 hOffsetSvc, 		/**< [in]  filtering 조건 3 - 기준되는 service handle. 만약 next를 찾고자하면 current service에 해당됨. */
								DbSvc_FindDirection_e	 eDirection, 		/**< [in]  filtering 조건 4 - 방향에 관한 조건 */
								Handle_t 				*phSvc)				/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_Service_t			*pstItem;
	HINT32					 i;

	HxLOG_Assert(pstAttr && phSvc);

	*phSvc = HANDLE_NULL;

	__TRY
	{
		__THROW( eSvcGroup == eSvcGroup_All );
		__THROW( db_svc_MgrServiceAttrToFilter(&stFilter, pstAttr) != NULL );
		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );

		switch (eDirection)
		{
		case eSvcFindDirection_Current:
			__THROW( hOffsetSvc != HANDLE_NULL );
			i = HLIB_VECTOR_ForEach(pstResult, (HBOOL(*)(void *,void *))db_svc_MgrCompareSerivceHandle, (void *)hOffsetSvc);
			if (i < 0)
				pstItem = NULL;
			else
				pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			break;

		case eSvcFindDirection_Prev:
			__THROW( hOffsetSvc != HANDLE_NULL );
			i = HLIB_VECTOR_ForEach(pstResult, (HBOOL(*)(void *,void *))db_svc_MgrCompareSerivceHandle, (void *)hOffsetSvc);
			if (i < 0)
				pstItem = NULL;
			else
			{
				i--;
				if (i < 0)
					pstItem = (dbSvc_Service_t *)HLIB_VECTOR_Last(pstResult);
				else
					pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			}
			break;

		case eSvcFindDirection_Next:
			__THROW( hOffsetSvc != HANDLE_NULL );
			i = HLIB_VECTOR_ForEach(pstResult, (HBOOL(*)(void *,void *))db_svc_MgrCompareSerivceHandle, (void *)hOffsetSvc);
			if (i < 0)
				pstItem = NULL;
			else
			{
				i++;
				if (i == HLIB_VECTOR_Length(pstResult))
					pstItem = (dbSvc_Service_t *)HLIB_VECTOR_First(pstResult);
				else
					pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			}
			break;

		case eSvcFindDirection_First:
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_First(pstResult);
			break;

		case eSvcFindDirection_Last:
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_Last(pstResult);
			break;

		default:
			pstItem = NULL;
			break;
		}
		__THROW( pstItem != NULL);

		*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstItem->stData), DbSvc_GetSvcIdx(&pstItem->stData));

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}


HERROR		db_svc_MgrFindServiceHandleByCondition (
								const DbSvc_Condition_t *pstCond,
								Handle_t *phSvc
							)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_Service_t			*pstItem;

	HxLOG_Assert(pstCond && phSvc);

	__TRY
	{
		__THROW( db_svc_MgrServiceConditionToFilter(&stFilter, pstCond) != NULL );
		stFilter.stFilter.nCount = 1;

		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );

		pstItem = (dbSvc_Service_t *)HLIB_VECTOR_First(pstResult);

		*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstItem->stData), DbSvc_GetSvcIdx(&pstItem->stData));

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	*phSvc = HANDLE_NULL;
	return ERR_FAIL;
}


HERROR     db_svc_MgrFindServiceHandleByTripleId(
								DxDeliveryType_e 	 eOrgDeliType, 	/**< [in]  서치 시 할당된 orginal delivery type */
								DbSvc_TripleId_t 	 stTripleId, 		/**< [in]  triple id (onid, tsid, svcid) */
								Handle_t 			*phSvc)		/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */
{
	dbSvc_ServiceFilter_t	stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_Service_t			*pstItem;

	HxLOG_Assert(phSvc);

	*phSvc = HANDLE_NULL;

	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nOnId	= (HINT32)stTripleId.usOnId;
		stFilter.nTsId	= (HINT32)stTripleId.usTsId;
		stFilter.nSvcId	= (HINT32)stTripleId.usSvcId;
		stFilter.stFilter.nCount = 1;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (pstItem   = HLIB_VECTOR_First(pstResult)) != NULL);

		*phSvc = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstItem->stData), DbSvc_GetSvcIdx(&pstItem->stData));

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindSvcHandleListWhereSearchFlagTrue(HINT32 *pnCnt, Handle_t **aphSvcList)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	Handle_t				*phList  = NULL;
	HINT32					 nCount;

	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.stFilter.ulFlags |= eDbSvc_TableFlag_SEARCH;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (phList  = db_svc_MgrMakeServiceListInt(pstResult, &nCount)) != NULL );

		if (pnCnt)
			*pnCnt = nCount;
		if (aphSvcList)
			*aphSvcList = phList;
		else
			OxDB_Free(phList);

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pnCnt)  *pnCnt = 0;
	if (aphSvcList) *aphSvcList = NULL;
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindServiceList(
								DbSvc_Group_e 			  eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			 *pstAttr, 			/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32					  ulMaxFindNum, 	/**< [in]  filtering 조건 3 - 가져올 최대 서비스 개수, 0이면 LIMIT이 없다 */
								Handle_t				  hOffsetSvc, 		/**< [in]  filtering 조건 4 - (Find 시작) 기준되는 hSvc */
								DbSvc_FindDirection_e	  eDirection,		/**< [in]  filtering 조건 5 - 방향에 관한 조건 */
								HUINT32 				 *pulCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 				**aphSvcList)		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_Service_t 		*pstItem;
	HINT32					 nOffset;
	HUINT32					 i, ulLen;
	Handle_t				*phList  = NULL;

	HxLOG_Assert(pstAttr && pulCount && aphSvcList);

	*pulCount   = 0;
	*aphSvcList = NULL;

	__TRY
	{
		__THROW( eSvcGroup == eSvcGroup_All );
		__THROW( db_svc_MgrServiceAttrToFilter(&stFilter, pstAttr) != NULL );
		stFilter.eSortType = eSortType_NUMBER;

		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );

		ulLen = HLIB_VECTOR_Length(pstResult);
		switch (eDirection)
		{
		case eSvcFindDirection_Backward:
			__THROW( hOffsetSvc != HANDLE_NULL );
			HLIB_VECTOR_Reverse(pstResult);
			nOffset = HLIB_VECTOR_ForEach(pstResult, (HBOOL (*)(void *, void *))db_svc_MgrCompareSerivceHandle, (void *)hOffsetSvc);
			nOffset = (nOffset == (HINT32)ulLen) ? -1 : nOffset;
			break;

		case eSvcFindDirection_BackwardFromLast:
			HLIB_VECTOR_Reverse(pstResult);
			nOffset = 0;
			break;

		case eSvcFindDirection_Foward:
			__THROW( hOffsetSvc != HANDLE_NULL );
			nOffset = HLIB_VECTOR_ForEach(pstResult, (HBOOL (*)(void *, void *))db_svc_MgrCompareSerivceHandle, (void *)hOffsetSvc);
			nOffset = (nOffset == (HINT32)ulLen) ? -1 : nOffset;
			break;

		case eSvcFindDirection_FowardFromStart:
		case eSvcFindDirection_SearchedOrder:
			nOffset = 0;
			break;

		default:
			nOffset = -1;
			break;
		}
		__THROW( nOffset >= 0 );

		ulLen = (ulMaxFindNum == 0 || ulMaxFindNum >= ulLen) ? ulLen : ulMaxFindNum;

		__THROW( (phList = (Handle_t *)OxDB_Malloc(sizeof(Handle_t) * ulLen)) != NULL );

		for (i = 0, *pulCount = 0 ; i < ulLen ; i++)
		{
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, ((HUINT32)nOffset + i) % ulLen);
			if (pstItem == NULL)
				continue;

			phList[(*pulCount)++] = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstItem->stData), DbSvc_GetSvcIdx(&pstItem->stData));
		}
		__THROW( *pulCount > 0 );

		*aphSvcList = phList;

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	if (phList)  OxDB_Free(phList);
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindServiceListBySvcId(
								DxDeliveryType_e 	  eOrgDeliType, 	/**< [in]  filtering 조건 1 - original delivery type */
								HUINT16 			  usSvcId, 		/**< [in]  filtering 조건 2 - service id */
								HINT32 				 *pnSvcCnt, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**aphSvcList)	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;

	HxLOG_Assert(pnSvcCnt && aphSvcList);

	*pnSvcCnt   = 0;
	*aphSvcList = NULL;

	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nSvcId = (HINT32)usSvcId;

		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (*aphSvcList = db_svc_MgrMakeServiceListInt(pstResult, pnSvcCnt)) != NULL);

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}


HERROR     db_svc_MgrFindServiceListByTripleId(
								DxDeliveryType_e 	  eOrgDeliType, /**< [in]  filtering 조건 1 - original delivery type */
								DbSvc_TripleId_t 	  stTripleId, 	/**< [in]  filtering 조건 2 - triple id */
								HINT32 				 *pnSvcCnt, 	/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**aphSvcList)	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;

	HxLOG_Assert(pnSvcCnt && aphSvcList);

	*pnSvcCnt   = 0;
	*aphSvcList = NULL;

	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nOnId	= (HINT32)stTripleId.usOnId;
		stFilter.nTsId	= (HINT32)stTripleId.usTsId;
		stFilter.nSvcId	= (HINT32)stTripleId.usSvcId;

		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (*aphSvcList = db_svc_MgrMakeServiceListInt(pstResult, pnSvcCnt)) != NULL);

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindServiceListWithInfo(
								DbSvc_Group_e 		  eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 		 *pstAttr, 			/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32 			 *pulCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**aphSvcList,		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
								DbSvc_Info_t		**ppstSvcInfoList)	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;

	HxLOG_Assert(pstAttr && pulCount && aphSvcList && ppstSvcInfoList);

	*pulCount   = 0;
	*aphSvcList = NULL;
	*ppstSvcInfoList = NULL;

	__TRY
	{
		__THROW( eSvcGroup == eSvcGroup_All );
		__THROW( db_svc_MgrServiceAttrToFilter(&stFilter, pstAttr) != NULL );
		stFilter.eSortType = eSortType_NUMBER;

		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (*pulCount = db_svc_MgrMakeServiceListWithInfo(pstResult, aphSvcList, ppstSvcInfoList)) > 0 );

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindSvcInfoList(DbSvc_Condition_t *pstCond, HINT32 *pnCnt, DbSvc_Info_t **ppstInfoList)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	DbSvc_Info_t			*pstSvclist = NULL;
	HUINT32					 ulCount;

	__TRY
	{
		__THROW( db_svc_MgrServiceConditionToFilter(&stFilter, pstCond) != NULL );
		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (pstSvclist = (DbSvc_Info_t *)db_svc_MgrMakeDataListFromVector(pstResult, sizeof(DbSvc_Info_t), &ulCount)) != NULL );

		if (pnCnt)
			*pnCnt = ulCount;
		if (ppstInfoList)
			*ppstInfoList = pstSvclist;
		else
			OxDB_Free(pstSvclist);

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pnCnt)      *pnCnt = 0;
	if (ppstInfoList) *ppstInfoList = NULL;
	if (pstSvclist)    OxDB_Free(pstSvclist);
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}

HINT32     db_svc_MgrFindSvcUidList (const DbSvc_Condition_t *pstCond, HINT32 *pnUids, HINT32 n)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_Table_t			*pstItem;
	HINT32					 nCount;

	HxLOG_Assert(pstCond && pnUids && n);

	__TRY
	{
		__THROW( db_svc_MgrServiceConditionToFilter(&stFilter, pstCond) != NULL );
		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );

		nCount = 0;
		HxVECTOR_EACH(pstResult, dbSvc_Table_t *, pstItem, {
			if (nCount >= n)
				break;
			pnUids[nCount++] = db_svc_TableGetKey(pstItem);
		});
		HLIB_VECTOR_Delete(pstResult);
		return nCount;
	}
	__CATCH:
	return 0;
}

HERROR     db_svc_MgrFindSvcHandleList(DbSvc_Condition_t *pstCond, HINT32 *pnCnt, Handle_t **aphSvcList)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;

	HxLOG_Assert(pstCond && pnCnt && aphSvcList);

	*pnCnt   = 0;
	*aphSvcList = NULL;

	__TRY
	{
		__THROW( db_svc_MgrServiceConditionToFilter(&stFilter, pstCond) != NULL );
		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );
		__THROW( (*aphSvcList = db_svc_MgrMakeServiceListInt(pstResult, (HUINT32 *)pnCnt)) != NULL );

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}


HERROR     db_svc_MgrFindTripleIdAndSvcTypeList(
					  DxDeliveryType_e		  eOrgDeliType
					, HUINT8				  ucAntIdx
					, HINT32				 *pnCnt
					, DbSvc_TripleId_t		**apstTripleIdList
					, DxSvcType_e			**apeSvcTypeList
					, Handle_t				**aphSvcList
				)
{
	dbSvc_ServiceFilter_t	 stFilter;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_Service_t			*pstItem;
	HINT32					 i, n, nCount;
	DbSvc_TripleId_t		 stTriplet;
	DxSvcType_e				 eSvcType;
	Handle_t				 hService;

	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nAntIdx = (HINT32)ucAntIdx;

		__THROW( (pstList	 = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL );

		n = HLIB_VECTOR_Length(pstResult);

		if (apstTripleIdList)
			__THROW( (*apstTripleIdList = (DbSvc_TripleId_t *)OxDB_Malloc(sizeof(DbSvc_TripleId_t) * n)) != NULL );
		if (apeSvcTypeList)
			__THROW( (*apeSvcTypeList = (DxSvcType_e *)OxDB_Malloc(sizeof(DxSvcType_e) * n)) != NULL );
		if (aphSvcList)
			__THROW( (*aphSvcList = (Handle_t *)OxDB_Malloc(sizeof(Handle_t) * n)) != NULL );

		for (i = 0, nCount = 0 ; i < n ; i++)
		{
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			if (pstItem == NULL)
				continue;

			stTriplet.usOnId = DbSvc_GetOnId(&pstItem->stData);
			stTriplet.usTsId = DbSvc_GetTsId(&pstItem->stData);
			stTriplet.usSvcId = DbSvc_GetSvcId(&pstItem->stData);
			eSvcType = DbSvc_GetSvcType(&pstItem->stData);
			hService = db_svc_MgrToHandle(DbSvc_GetTsIdx(&pstItem->stData), DbSvc_GetSvcIdx(&pstItem->stData));

			if (apstTripleIdList)
				(*apstTripleIdList)[nCount] = stTriplet;
			if (apeSvcTypeList)
				(*apeSvcTypeList)[nCount] = eSvcType;
			if (aphSvcList)
				(*aphSvcList)[nCount] = hService;
			nCount++;
		}
		if (pnCnt)
			*pnCnt = nCount;

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	if (apstTripleIdList && *apstTripleIdList) OxDB_Free(*apstTripleIdList);
	if (apeSvcTypeList && *apeSvcTypeList) OxDB_Free(*apeSvcTypeList);
	if (aphSvcList     && *aphSvcList)     OxDB_Free(*aphSvcList);

	if (pnCnt)          *pnCnt = 0;
	if (apstTripleIdList) *apstTripleIdList = NULL;
	if (apeSvcTypeList) *apeSvcTypeList = NULL;
	if (aphSvcList)     *aphSvcList = NULL;

	if (pstResult) HLIB_VECTOR_Delete(pstResult);
	return ERR_FAIL;
}


HERROR     db_svc_MgrCheckValidServiceHandle(Handle_t hSvc)
{
	dbSvc_TableList_t	*pstList	= NULL;
	__TRY
	{
		__THROW( hSvc != (Handle_t)NULL && hSvc != HANDLE_NULL );
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		__THROW( db_svc_MgrTableFindItemByKey(pstList, (HINT32)db_svc_MgrToSvcIdx(hSvc)) );

		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}


void     db_svc_MgrResetAudioUserFlag(void)
{
	dbSvc_TableList_t	*pstList;
	dbSvc_Service_t 	*pstItem;
	void				*pvIter;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList)
	{
		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Service_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem == NULL)
				continue;
			if (DbSvc_GetAudUserSelected(&pstItem->stData))
			{
				DbSvc_SetAudUserSelected(&pstItem->stData, FALSE);
				db_svc_MgrSvcUpdateListener(pstList, 0, (HINT32)DbSvc_GetSvcIdx(&pstItem->stData), NULL);
			}
		});
	}
}

void     db_svc_MgrResetSubtitleUserFlag(void)
{
	dbSvc_TableList_t	*pstList;
	dbSvc_Service_t 	*pstItem;
	void				*pvIter;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList)
	{
		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Service_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem == NULL)
				continue;
			if (DbSvc_GetSubttlFlag(&pstItem->stData))
			{
				DbSvc_SetSubttlFlag(&pstItem->stData, FALSE);
				db_svc_MgrSvcUpdateListener(pstList, 0, (HINT32)DbSvc_GetSvcIdx(&pstItem->stData), NULL);
			}
		});
	}
}


void	db_svc_MgrResetRatingPassedEventId(void)
{
	dbSvc_TableList_t	*pstList;
	dbSvc_Service_t 	*pstItem;
	void				*pvIter;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList)
	{
		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Service_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem == NULL)
				continue;
			if (DbSvc_GetRatingPassedEvent(&pstItem->stData) != (HUINT32)0xFFFFFFFF)
			{
				DbSvc_SetRatingPassedEvent(&pstItem->stData, 0xFFFFFFFF);
				db_svc_MgrSvcUpdateListener(pstList, 0, (HINT32)DbSvc_GetSvcIdx(&pstItem->stData), NULL);
			}
		});
	}
}

HERROR     db_svc_MgrFindProviderIndexByName(HUINT8 *pszName, HUINT16 *pusPrvIdx)
{
	dbSvc_TableList_t	*pstList	= NULL;
	dbSvc_Provider_t	*pstItem;
	void				*pvIter;

	HxLOG_Assert(pszName && pusPrvIdx);

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Provider)) != NULL);

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Provider_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem && strcmp(pstItem->stData.szPrvName, pszName) == 0)
			{
				*pusPrvIdx = pstItem->stData.usPrvIdx;
				return ERR_OK;
			}
		});
	}
	__CATCH:
	return ERR_FAIL;
}


HERROR     db_svc_MgrFindNetworkIndexById(DxDeliveryType_e eOrgDeliType, HUINT16 usNetId, HUINT16 *pusNetworkIdx)
{
	dbSvc_TableList_t	*pstList	= NULL;
	dbSvc_Network_t		*pstItem;
	void				*pvIter;

	HxLOG_Assert(pusNetworkIdx);

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Network)) != NULL);

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Network_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem == NULL)
				continue;
			if (pstItem->stData.eOrgDeliType == eOrgDeliType && pstItem->stData.usNetId == usNetId)
			{
				*pusNetworkIdx = pstItem->stData.usNetIdx;
				return ERR_OK;
			}
		});
	}
	__CATCH:
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindNetworkIndexByName(HUINT16 usNetId, HUINT8 *pszName, HUINT16 *pusNetworkIdx)
{
	dbSvc_TableList_t	*pstList	= NULL;
	dbSvc_Network_t		*pstItem;
	void				*pvIter;

	HxLOG_Assert(pszName && pusNetworkIdx);

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Network)) != NULL);

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Network_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem == NULL)
				continue;
			if (pstItem->stData.usNetId == usNetId && strcmp(pstItem->stData.szNetName, pszName) == 0)
			{
				*pusNetworkIdx = pstItem->stData.usNetIdx;
				return ERR_OK;
			}
		});
	}
	__CATCH:
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindNetworkInfoByDeliType(DxDeliveryType_e eOrgDeliType, HINT32 *pnCnt, DbSvc_NetInfo_t **ppstNetworkInfo)
{
	dbSvc_TableList_t	*pstList = NULL;
	dbSvc_Network_t		*pstItem;
	DbSvc_NetInfo_t		*pstNetlist = NULL;
	void				*pvIter;
	HINT32				 n, nCount = 0;

	HxLOG_Assert(pnCnt && ppstNetworkInfo);

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Network)) != NULL);

		n = db_svc_MgrTableCountItem(pstList);
		__THROW( n > 0 );
		__THROW( (pstNetlist = (DbSvc_NetInfo_t *)OxDB_Malloc(sizeof(DbSvc_NetInfo_t) * n)) != NULL );

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Network_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem == NULL)
				continue;

			if (pstItem->stData.eOrgDeliType != eOrgDeliType)
				continue;

			HxSTD_MemCopy(pstNetlist + nCount++, &pstItem->stData, sizeof(DbSvc_NetInfo_t));
		});
		__THROW( nCount > 0 );

		*pnCnt	   = nCount;
		*ppstNetworkInfo = pstNetlist;
		return ERR_OK;
	}
	__CATCH:
	*pnCnt = 0;
	*ppstNetworkInfo = NULL;
	if (pstNetlist) OxDB_Free(pstNetlist);
	return ERR_FAIL;

}



HERROR     db_svc_MgrGetAllCasType(DxSvcType_e eSvcType, DxVideoType_e eVideoType, DxCasType_e *pbwCasType)
{
	HINT32					 i, n;
	dbSvc_Service_t			*pstItem;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_ServiceFilter_t	 stFilter;
	DxCasType_b				 bwCasType = eDxCAS_TYPE_All;

	HxLOG_Assert(pbwCasType);

	*pbwCasType = eDxCAS_TYPE_All;
	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eSvcType = eSvcType;
		stFilter.eVideoType = eVideoType;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL);
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL);

		n = HLIB_VECTOR_Length(pstResult);
		for (i = 0 ; i < n ; i++)
		{
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			if (pstItem == NULL)
				continue;

			bwCasType |= DbSvc_GetCasType(&(pstItem->stData));
		}
		HLIB_VECTOR_Delete(pstResult);

		*pbwCasType = bwCasType;
		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}


HERROR     db_svc_MgrIsLcnNetwork(DxDeliveryType_e eOrgDeliType, HBOOL *pbIsLcnNetwork)
{
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_ServiceFilter_t	 stFilter;

	HxLOG_Assert(pbIsLcnNetwork);

	*pbIsLcnNetwork = FALSE;
	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nLcnFlag = TRUE;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL);
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL);
		HLIB_VECTOR_Delete(pstResult);

		*pbIsLcnNetwork = TRUE;
		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetMaxLcnInLcnServices (DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 *pnLcn)
{
	HUINT16					 usLcn;
	HINT32					 i, n;
	dbSvc_Service_t			*pstItem;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_ServiceFilter_t	 stFilter;

	HxLOG_Assert(pnLcn);

	*pnLcn = 0;
	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eSvcType = eSvcType;
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nLcnFlag = TRUE;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL);
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL);

		n = HLIB_VECTOR_Length(pstResult);
		for (i = 0, usLcn = 0 ; i < n ; i++)
		{
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			if (pstItem == NULL)
				continue;

			if (usLcn < DbSvc_GetLcn(&pstItem->stData))
				usLcn = DbSvc_GetLcn(&pstItem->stData);
		}
		*pnLcn = (HINT32)usLcn;

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}

HERROR     db_svc_MgrGetMaxLcnInInterval(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nStartNum, HINT32 nEndNum, HINT32 *pnLcn)
{
	HUINT16 				 usLcn;
	HINT32					 i, n;
	dbSvc_Service_t 		*pstItem;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_ServiceFilter_t	 stFilter;

	HxLOG_Assert(pnLcn);

	*pnLcn = 0;
	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eSvcType = eSvcType;
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nLcnFlag = TRUE;

		__THROW( (pstList	= db_svc_MgrGetListItem(eSvcTable_Service)) != NULL);
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL);

		n = HLIB_VECTOR_Length(pstResult);
		for (i = 0, usLcn = 0 ; i < n ; i++)
		{
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			if (pstItem == NULL)
				continue;

			if (usLcn < DbSvc_GetLcn(&pstItem->stData))
			{
				if(usLcn >= nStartNum  && usLcn <= nEndNum )
				{
					usLcn = DbSvc_GetLcn(&pstItem->stData);
				}
			}
		}
		*pnLcn = (HINT32)usLcn;

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}


HERROR     db_svc_MgrRenumberSavingOrderToChNum(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nBaseStartChNum)
{
	HINT32					 i, n;
	dbSvc_Service_t			*pstItem;
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_ServiceFilter_t	 stFilter;
	DbSvc_Info_t			 stInfo;

	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eSvcType = eSvcType;
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nLcnFlag = FALSE;	// TODO: 이걸 넣어야 하나??
		stFilter.eSortType = eSortType_INDEX;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL);
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL);

		n = HLIB_VECTOR_Length(pstResult);
		for (i = 0 ; i < n ; i++)
		{
			pstItem = (dbSvc_Service_t *)HLIB_VECTOR_ItemAt(pstResult, i);
			if (pstItem == NULL)
				continue;
			HxSTD_MemCopy(&stInfo, &pstItem->stData, sizeof(DbSvc_Info_t));
			DbSvc_SetLcn(&stInfo, nBaseStartChNum++);

			(void)db_svc_MgrTableUpdateItemByKey(pstList, db_svc_TableGetKey((dbSvc_Table_t *)pstItem), &stInfo);
		}
		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}


HERROR db_svc_MgrGetLcnOrderNumber(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HUINT16 *pusLcn)
{
	dbSvc_TableList_t		*pstList	= NULL;
	HxVector_t				*pstResult = NULL;
	dbSvc_ServiceFilter_t	 stFilter;

	HxLOG_Assert(pusLcn);

	*pusLcn = 0;
	__TRY
	{
		db_svc_MgrServiceInitFilter(&stFilter);
		stFilter.eSvcType     = eSvcType;
		stFilter.eOrgDeliType = eOrgDeliType;
		stFilter.nLcnFlag     = FALSE;
		stFilter.eSortType    = eSortType_INDEX;

		__THROW( (pstList   = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL);
		__THROW( (pstResult = db_svc_MgrServiceFilterList(pstList, &stFilter)) != NULL);

		*pusLcn = (HUINT16)HLIB_VECTOR_Length(pstResult);

		HLIB_VECTOR_Delete(pstResult);
		return ERR_OK;
	}
	__CATCH:
	return ERR_FAIL;
}


#if defined(CONFIG_DB_SVC_SVCBOUQUET)
HERROR     db_svc_MgrAddBouquetInfo(DbSvc_BouqInfo_t *pstBouquetInfo, HUINT16 *pusBouqIdx)
{
	dbSvc_Bouquet_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pstBouquetInfo && pusBouqIdx);
	*pusBouqIdx = 0;

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	pstInfo = db_svc_BouquetNew(pstBouquetInfo);
	__RETURN_IF(pstInfo == NULL, ERR_FAIL);

	if (db_svc_MgrTableAddItem(pstList, (dbSvc_Table_t *)pstInfo) != ERR_OK)
	{
		db_svc_BouquetDelete(pstInfo);
		return ERR_FAIL;
	}
	*pusBouqIdx = pstInfo->stData.usBouqIdx;
	HxLOG_Info("db_svc_MgrAddBouquetInfo *pusBouqIdx = %d\n", *pusBouqIdx);
	return ERR_OK;
}

HERROR     db_svc_MgrDeleteBouquetInfo(HUINT16 usBouqIdx)
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	return db_svc_MgrTableRemoveItemByKey(pstList, (HINT32)usBouqIdx);
}

HERROR     db_svc_MgrDeleteAllBouquetInfo(void)
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	db_svc_MgrTableClearList(pstList);
	return ERR_OK;

}

HERROR     db_svc_MgrUpdateBouquetInfo(HUINT16 usBouqIdx, DbSvc_BouqInfo_t *pstBouquetInfo)
{
	dbSvc_TableList_t	*pstList;

	__RETURN_IF(pstBouquetInfo == NULL, ERR_FAIL);

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	pstBouquetInfo->usBouqIdx = usBouqIdx;
	return db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)usBouqIdx, pstBouquetInfo);
}

HERROR     db_svc_MgrGetBouquetCount(HINT32 *pnCnt)
{
	dbSvc_TableList_t	*pstList;

	__RETURN_IF(pnCnt == NULL, ERR_FAIL);

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	*pnCnt = db_svc_MgrTableCountItem(pstList);
	return ERR_OK;
}

HERROR     db_svc_MgrGetBouquetInfo(HUINT16 usBouqIdx, DbSvc_BouqInfo_t *pstBouquetInfo)
{
	dbSvc_Bouquet_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Assert(pstBouquetInfo);

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	pstSelf = (dbSvc_Bouquet_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usBouqIdx);
	if (pstSelf)
	{
		HxSTD_MemCopy(pstBouquetInfo, &pstSelf->stData, sizeof(DbSvc_BouqInfo_t));
		hErr = ERR_OK;
	}
	return hErr;
}

HERROR     db_svc_MgrFindBouquetIndexById(HUINT16 usBouqId, HUINT16 *pusBouqIdx)
{
	dbSvc_Bouquet_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	void				*pvIter;

	HxLOG_Assert(pusBouqIdx);

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	HxTREE_EACH(pstList->pstTree, pvIter, {
		pstSelf = (dbSvc_Bouquet_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstSelf && pstSelf->stData.usBouqId == usBouqId)
		{
			*pusBouqIdx = pstSelf->stData.usBouqIdx;
			return ERR_OK;
		}
	});
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindBouquetIndexByName(const HCHAR *pszName, HUINT16 *pusBouqIdx)
{
	dbSvc_Bouquet_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	void				*pvIter;

	HxLOG_Assert(pszName && pusBouqIdx);

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	HxTREE_EACH(pstList->pstTree, pvIter, {
		pstSelf = (dbSvc_Bouquet_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstSelf && HxSTD_StrCmp((char *)pstSelf->stData.szBouqName, (const char *)pszName) == 0)
		{
			*pusBouqIdx = pstSelf->stData.usBouqIdx;
			return ERR_OK;
		}
	});
	return ERR_FAIL;
}

HERROR     db_svc_MgrFindBouquetIndexList(HINT32 *pnBouqNum, HUINT16 **ppusBouqIdxList)
{
	dbSvc_Bouquet_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	HINT32				 nCount;
	void				*pvIter;

	__RETURN_IF( pnBouqNum == NULL || ppusBouqIdxList == NULL, ERR_FAIL );
	__RETURN_IF( db_svc_MgrGetBouquetCount(pnBouqNum) == ERR_FAIL, ERR_FAIL );
	__RETURN_IF( *pnBouqNum == 0, ERR_FAIL );

	pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet);
	__RETURN_IF(pstList == NULL, ERR_FAIL);

	*ppusBouqIdxList = (HUINT16 *)OxDB_Malloc(sizeof(HUINT16) * (*pnBouqNum));
	__RETURN_IF(*ppusBouqIdxList == NULL, ERR_FAIL);

	nCount = 0;
	HxTREE_EACH(pstList->pstTree, pvIter, {
		pstSelf = (dbSvc_Bouquet_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstSelf)
			(*ppusBouqIdxList)[nCount++] = pstSelf->stData.usBouqIdx;
	});
	if (nCount == 0)
	{
		OxDB_Free(*ppusBouqIdxList);
		return ERR_FAIL;
	}
	*pnBouqNum = nCount;
	return ERR_OK;
}


HERROR     db_svc_MgrAddBouquetIndexToSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usBqIdx)
{
	HUINT32			 ulIndex, ulBqNum = 0;

	__RETURN_IF(pstSvcInfo == NULL, ERR_FAIL);

	(void)db_svc_MgrCountBouquetInSvcInfo(pstSvcInfo, &ulBqNum);

	for (ulIndex = 0; ulIndex < ulBqNum; ulIndex++)
	{
		if (DbSvc_GetBouquetId(pstSvcInfo, ulIndex) == (HINT32)usBqIdx)
		{
			return ERR_OK;
		}
	}

	if (ulBqNum >= DxBOUQUET_LEN)
	{
		HxLOG_Error("MAX Bouquet Index (%d) exceeded:\n", DxBOUQUET_LEN);
		return ERR_FAIL;
	}

	DbSvc_SetBouquetId(pstSvcInfo, ulIndex, usBqIdx);
	return ERR_OK;
}

HERROR     db_svc_MgrCountBouquetInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 *pulBqNum)
{
	HUINT32			 ulIndex;

	__RETURN_IF(pstSvcInfo == NULL || pulBqNum == NULL, ERR_FAIL);

	for (ulIndex = 0; ulIndex < DxBOUQUET_LEN; ulIndex++)
	{
		if (DbSvc_GetBouquetId(pstSvcInfo, ulIndex) <= 0)
		{
			break;
		}
	}

	*pulBqNum = ulIndex;
	return ERR_OK;
}

HERROR     db_svc_MgrGetBouquetIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulIndex, HUINT16 *pusBqIdx)
{
	HUINT32			 ulBqNum = 0;

	__RETURN_IF(pstSvcInfo == NULL || pusBqIdx == NULL, ERR_FAIL);

	(void)db_svc_MgrCountBouquetInSvcInfo(pstSvcInfo, &ulBqNum);
	if (ulIndex >= ulBqNum)
	{
		HxLOG_Error("Bouquet Number (%d), Index (%d)\n", ulBqNum, ulIndex);
		return ERR_FAIL;
	}

	*pusBqIdx = (HUINT16)DbSvc_GetBouquetId(pstSvcInfo, ulIndex);
	return ERR_OK;
}

HERROR     db_svc_MgrGetBouquetIndexArrayInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulBqArrayMaxNum, HUINT32 *pulBqIdxNum, HUINT16 *pusBqIdxArray)
{
	HUINT32			 ulIndex, ulBqNum = 0;

	__RETURN_IF(pstSvcInfo == NULL, ERR_FAIL);

	(void)db_svc_MgrCountBouquetInSvcInfo(pstSvcInfo, &ulBqNum);
	ulBqNum = (ulBqNum > ulBqArrayMaxNum) ? ulBqArrayMaxNum : ulBqNum;

	if (pulBqIdxNum)
	{
		*pulBqIdxNum = ulBqNum;
	}

	if (pusBqIdxArray && ulBqNum)
	{
		for (ulIndex = 0; ulIndex < ulBqNum; ulIndex++)
		{
			pusBqIdxArray[ulIndex] = (HUINT16)DbSvc_GetBouquetId(pstSvcInfo, ulIndex);
		}
	}

	return (ulBqNum == 0) ? ERR_FAIL : ERR_OK;
}

HERROR     db_svc_MgrClearBouquetInfoInSvcInfo (DbSvc_Info_t *pstSvcInfo)
{
	HUINT32			 ulIndex;

	__RETURN_IF(pstSvcInfo == NULL, ERR_FAIL);

	for (ulIndex = 0; ulIndex < DxBOUQUET_LEN; ulIndex++)
	{
		DbSvc_SetBouquetId(pstSvcInfo, ulIndex, 0);
	}

	return ERR_OK;
}
#endif

HERROR		db_svc_MgrAddGroupInfo (DbSvc_GroupInfo_t *pstGroupInfo, HUINT16 *pusGroupIdx)
{
	dbSvc_Group_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pstGroupInfo && pusGroupIdx);
	*pusGroupIdx = 0;

	__TRY
	{
		__THROW( (pstInfo = db_svc_GroupNew(pstGroupInfo)) != NULL );
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Group)) != NULL );

		(void) db_svc_MgrTableSetFlag((dbSvc_Table_t *)pstInfo, eDbSvc_TableFlag_SEARCH, TRUE);

		__THROW( db_svc_MgrTableAddItem(pstList, (dbSvc_Table_t *)pstInfo) == ERR_OK );

		*pusGroupIdx = pstInfo->stData.usGroupIdx;
		HxLOG_Info("db_svc_MgrAddGroupInfo *pusGroupIdx = %d\n", *pusGroupIdx);
		return ERR_OK;
	}
	__CATCH:
	if (pstInfo) db_svc_GroupDelete(pstInfo);
	return ERR_FAIL;
}

HERROR		db_svc_MgrDeleteGroupInfo (HUINT16 usGroupIdx)
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Group);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Group).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}
	return db_svc_MgrTableRemoveItemByKey(pstList, (HINT32)usGroupIdx);
}

HERROR		db_svc_MgrDeleteAllGroupInfo(void)
{
	dbSvc_TableList_t	*pstList;

	pstList = db_svc_MgrGetListItem(eSvcTable_Group);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Group).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	db_svc_MgrTableClearList(pstList);
	return ERR_OK;
}

HERROR		db_svc_MgrUpdateGroupInfo (HUINT16 usGroupIdx, DbSvc_GroupInfo_t *pstGroupInfo)
{
	dbSvc_TableList_t	*pstList;

	if (pstGroupInfo == NULL)
		return ERR_FAIL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Group);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Group).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstGroupInfo->usGroupIdx = usGroupIdx;
	return db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)usGroupIdx, pstGroupInfo);
}

HERROR		db_svc_MgrCountGroup (HINT32 *pnCnt)
{
	dbSvc_TableList_t	*pstList;

	HxLOG_Assert(pnCnt);

	*pnCnt = 0;

	pstList = db_svc_MgrGetListItem(eSvcTable_Group);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Group).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	*pnCnt = db_svc_MgrTableCountItem(pstList);
	return ERR_OK;
}

HERROR		db_svc_MgrGetGroupInfo (HUINT16 usGroupIdx, DbSvc_GroupInfo_t *pstGroupInfo)
{
	dbSvc_Group_t		*pstSelf;
	dbSvc_TableList_t	*pstList;
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Assert(pstGroupInfo);

	pstList = db_svc_MgrGetListItem(eSvcTable_Group);
	if (pstList == NULL)
	{
		HxLOG_Error("[%s:%d] Failed to db_svc_MgrGetListItem(eSvcTable_Group).\n", __HxFILE__, __HxLINE__);
		return ERR_FAIL;
	}

	pstSelf = (dbSvc_Group_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usGroupIdx);
	if (pstSelf)
	{
		HxSTD_MemCopy(pstGroupInfo, &pstSelf->stData, sizeof(DbSvc_GroupInfo_t));
		hErr = ERR_OK;
	}
	return hErr;
}

HERROR		db_svc_MgrGetGroupInfoByGroupId (HUINT32 ulGroupId, DbSvc_GroupInfo_t *pstGroupInfo)
{
	dbSvc_Group_t		*pstItem;
	dbSvc_TableList_t	*pstList = NULL;
	void				*pvIter;

	HxLOG_Assert(pstGroupInfo);

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Group)) != NULL );

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Group_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem && pstItem->stData.ulGroupId == ulGroupId)
			{
				HxSTD_MemCopy(pstGroupInfo, &pstItem->stData, sizeof(DbSvc_GroupInfo_t));
				return ERR_OK;
			}
		});
	}
	__CATCH:
	return ERR_FAIL;
}


HERROR		db_svc_MgrFindGroupIndexByName (HUINT8 *pszName, HUINT16 *pusGroupIdx)
{
	dbSvc_Group_t		*pstItem;
	dbSvc_TableList_t	*pstList = NULL;
	void				*pvIter;

	HxLOG_Assert(pszName && pusGroupIdx);

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Group)) != NULL );

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Group_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem && strcmp(pstItem->stData.szGroupName, pszName) == 0)
			{
				*pusGroupIdx = pstItem->stData.usGroupIdx;
				return ERR_OK;
			}
		});
	}
	__CATCH:
	return ERR_FAIL;
}

HERROR		db_svc_MgrGetAllGroupIndexList (HINT32 *pnGroupNum, HUINT16 **ppusGroupIdxList)
{
	dbSvc_Group_t		*pstItem;
	dbSvc_TableList_t	*pstList = NULL;
	HUINT16				*ausGroupList = NULL;

	HxLOG_Assert(pnGroupNum && ppusGroupIdxList);

	__TRY
	{
		HINT32	 n, nCount = 0;
		void	*pvIter;

		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Group)) != NULL );

		n = db_svc_MgrTableCountItem(pstList);

		__THROW( n > 0 );
		__THROW( (ausGroupList = (HUINT16 *)OxDB_Malloc(sizeof(HUINT16) * n)) != NULL );

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Group_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem)
				ausGroupList[nCount++] = pstItem->stData.usGroupIdx;
		});
		__THROW( nCount > 0 );

		*pnGroupNum = nCount;
		*ppusGroupIdxList = ausGroupList;
		return ERR_OK;
	}
	__CATCH:
	*pnGroupNum = 0;
	*ppusGroupIdxList = NULL;
	if (ausGroupList)
		OxDB_Free(ausGroupList);
	return ERR_FAIL;
}

HERROR     	db_svc_MgrGetAllGroupInfoList (HINT32 *pnGroupNum, DbSvc_GroupInfo_t **pstGroupInfoList)
{
	dbSvc_Group_t		*pstItem;
	dbSvc_TableList_t	*pstList = NULL;
	DbSvc_GroupInfo_t	*ausGroupList = NULL;

	HxLOG_Assert(pnGroupNum && pstGroupInfoList);

	__TRY
	{
		void	*pvIter;
		HINT32	 n, nCount = 0;

		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Group)) != NULL );

		n = db_svc_MgrTableCountItem(pstList);

		__THROW( n > 0 );
		__THROW( (ausGroupList = (DbSvc_GroupInfo_t *)OxDB_Malloc(sizeof(DbSvc_GroupInfo_t) * n)) != NULL );

		HxTREE_EACH(pstList->pstTree, pvIter, {
			pstItem = (dbSvc_Group_t *)HLIB_TREE_IteratorValue(pvIter);
			if (pstItem)
				HxSTD_MemCopy(ausGroupList + nCount++, &pstItem->stData, sizeof(DbSvc_GroupInfo_t));
		});
		__THROW( nCount > 0 );

		*pnGroupNum = nCount;
		*pstGroupInfoList = ausGroupList;
		return ERR_OK;
	}
	__CATCH:
	*pnGroupNum = 0;
	*pstGroupInfoList = NULL;
	if (ausGroupList)
		OxDB_Free(ausGroupList);
	return ERR_FAIL;
}

#if defined(CONFIG_DB_SVC_SVCGROUP)
HERROR     db_svc_MgrCountGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 *pulGroupNum)
{
	HUINT32			 ulIndex;

	__RETURN_IF(pstSvcInfo == NULL || pulGroupNum == NULL, ERR_FAIL);

	for (ulIndex = 0; ulIndex < DxGROUP_LEN; ulIndex++)
	{
		if (DbSvc_GetGroupId(pstSvcInfo, ulIndex) <= 0)
		{
			break;
		}
	}

	*pulGroupNum = ulIndex;
	return ERR_OK;
}

HERROR		db_svc_MgrAddGroupIndexToSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usGroupIdx)
{
	HUINT32			 ulIndex, ulGroupNum = 0;

	__RETURN_IF(pstSvcInfo == NULL, ERR_FAIL);

	(void)db_svc_MgrCountGroupIndexInSvcInfo(pstSvcInfo, &ulGroupNum);

	for (ulIndex = 0; ulIndex < ulGroupNum; ulIndex++)
	{
		if (DbSvc_GetGroupId(pstSvcInfo, ulIndex) == (HINT32)usGroupIdx)
		{
			return ERR_OK;
		}
	}

	if (ulGroupNum >= DxGROUP_LEN)
	{
		HxLOG_Error("GroupNum(%d) exceed the limit (%d)\n", ulGroupNum, DxGROUP_LEN);
		return ERR_FAIL;
	}

	DbSvc_SetGroupId(pstSvcInfo, ulGroupNum, usGroupIdx);
	return ERR_OK;
}

HERROR		db_svc_MgrGetGroupIndexArrayInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulGroupArrayMaxNum, HUINT32 *pulGroupIdxNum, HUINT16 *pusGroupIdxArray)
{
	HUINT32			 ulIndex, ulGroupNum = 0;

	__RETURN_IF(pstSvcInfo == NULL, ERR_FAIL);

	(void)db_svc_MgrCountGroupIndexInSvcInfo(pstSvcInfo, &ulGroupNum);
	ulGroupNum = (ulGroupNum > ulGroupArrayMaxNum) ? ulGroupArrayMaxNum : ulGroupNum;

	if (pulGroupIdxNum)
	{
		*pulGroupIdxNum = ulGroupNum;
	}

	if (pusGroupIdxArray && ulGroupNum)
	{
		for (ulIndex = 0; ulIndex < ulGroupNum; ulIndex++)
		{
			pusGroupIdxArray[ulIndex] = (HUINT16)DbSvc_GetGroupId(pstSvcInfo, ulIndex);
		}
	}

	return (ulGroupNum == 0) ? ERR_FAIL : ERR_OK;
}

HERROR		db_svc_MgrGetGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulIndex, HUINT16 *pusGroupIdx)
{
	HUINT32			 ulGroupNum = 0;

	__RETURN_IF(pstSvcInfo == NULL || pusGroupIdx == NULL, ERR_FAIL);

	(void)db_svc_MgrCountGroupIndexInSvcInfo(pstSvcInfo, &ulGroupNum);
	if (ulIndex >= ulGroupNum)
	{
		HxLOG_Error("ulIndex(%d), ulGroupNum(%d)\n", ulIndex, ulGroupNum);
		return ERR_FAIL;
	}

	*pusGroupIdx = (HUINT16)DbSvc_GetGroupId(pstSvcInfo, ulIndex);
	return ERR_OK;
}

HERROR		db_svc_MgrDeleteGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usGroupIdx)
{
	HUINT32			 ulIndex, ulSubIndex, ulGroupNum = 0;

	__RETURN_IF(pstSvcInfo == NULL, ERR_FAIL);

	(void)db_svc_MgrCountGroupIndexInSvcInfo(pstSvcInfo, &ulGroupNum);

	for (ulIndex = 0; ulIndex < ulGroupNum; ulIndex++)
	{
		if (DbSvc_GetGroupId(pstSvcInfo, ulIndex) == (HINT32)usGroupIdx)
		{
			for (ulSubIndex = ulIndex + 1; ulSubIndex < DxGROUP_LEN; ulSubIndex++)
			{
				DbSvc_SetGroupId(pstSvcInfo, (ulSubIndex - 1), DbSvc_GetGroupId(pstSvcInfo, ulSubIndex));
			}

			DbSvc_SetGroupId(pstSvcInfo, (DxGROUP_LEN - 1), 0);
			return ERR_OK;
		}
	}

	return ERR_OK;
}

HERROR		db_svc_MgrClearGroupInSvcInfo (DbSvc_Info_t *pstSvcInfo)
{
	HUINT32			 ulIndex;

	__RETURN_IF(pstSvcInfo == NULL, ERR_FAIL);

	for (ulIndex = 0; ulIndex < DxGROUP_LEN; ulIndex++)
	{
		DbSvc_SetGroupId(pstSvcInfo, ulIndex, 0);
	}

	return ERR_OK;
}
#endif


DbSvc_TsInfo_t *	db_svc_MgrGetAllTsInfo (HINT32 *pulCount)
{
	dbSvc_TableList_t	*pstList = NULL;
	DbSvc_TsInfo_t		*astInfoList;

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Transponder)) != NULL );
		(void) db_svc_MgrTableFindItemByKey(pstList, 0);

		__THROW( (astInfoList = (DbSvc_TsInfo_t *)db_svc_MgrMakeDataList(pstList->pstTree, sizeof(DbSvc_TsInfo_t), pulCount)) != NULL );
		return astInfoList;
	}
	__CATCH:
	return NULL;
}

DbSvc_NetInfo_t *	db_svc_MgrGetAllNetInfo (HINT32 *pulCount)
{
	dbSvc_TableList_t	*pstList = NULL;
	DbSvc_NetInfo_t		*astInfoList;

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Network)) != NULL );
		(void) db_svc_MgrTableFindItemByKey(pstList, 0);
		__THROW( (astInfoList = (DbSvc_NetInfo_t *)db_svc_MgrMakeDataList(pstList->pstTree, sizeof(DbSvc_NetInfo_t), pulCount)) != NULL );
		return astInfoList;
	}
	__CATCH:
	return NULL;
}

DbSvc_PrvInfo_t *	db_svc_MgrGetAllPrvInfo (HINT32 *pulCount)
{
	dbSvc_TableList_t	*pstList = NULL;
	DbSvc_PrvInfo_t		*astInfoList;

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Provider)) != NULL );
		(void) db_svc_MgrTableFindItemByKey(pstList, 0);
		__THROW( (astInfoList = (DbSvc_PrvInfo_t *)db_svc_MgrMakeDataList(pstList->pstTree, sizeof(DbSvc_PrvInfo_t), pulCount)) != NULL );
		return astInfoList;
	}
	__CATCH:
	return NULL;
}

DbSvc_Info_t *	db_svc_MgrGetAllSvcInfo (HINT32 *pulCount)
{
	dbSvc_TableList_t	*pstList = NULL;
	DbSvc_Info_t		*astInfoList;

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Service)) != NULL );
		(void) db_svc_MgrTableFindItemByKey(pstList, 0);

		__THROW( (astInfoList = (DbSvc_Info_t *)db_svc_MgrMakeDataList(pstList->pstTree, sizeof(DbSvc_Info_t), pulCount)) != NULL );
		return astInfoList;
	}
	__CATCH:
	return NULL;
}


DbSvc_BouqInfo_t *	db_svc_MgrGetAllBouqInfo (HINT32 *pulCount)
{
	dbSvc_TableList_t	*pstList = NULL;
	DbSvc_BouqInfo_t	*astInfoList;

	__TRY
	{
		__THROW( (pstList = db_svc_MgrGetListItem(eSvcTable_Bouquet)) != NULL );
		(void) db_svc_MgrTableFindItemByKey(pstList, 0);
		__THROW( (astInfoList = (DbSvc_BouqInfo_t *)db_svc_MgrMakeDataList(pstList->pstTree, sizeof(DbSvc_BouqInfo_t), pulCount)) != NULL );
		return astInfoList;
	}
	__CATCH:
	return NULL;
}

void		db_svc_MgrDumpSvcInfo (const DbSvc_Info_t *pstInfo)
{
	dbSvc_TableList_t	*pstList	= NULL;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList)
	{
		dbSvc_Table_t *t = db_svc_MgrTableFindItemByKey(pstList, (HINT32)DbSvc_GetSvcIdx(pstInfo));
		if (t)
			db_svc_TableToString(t, stdout);
	}
}

void		db_svc_MgrDumpAllSvcInfo (const HCHAR *pszFileName)
{

	dbSvc_TableList_t	*pstList	= NULL;
	FILE				*fp;

	fp = fopen(pszFileName, "w");
	if (fp == NULL)
		return;

	pstList = db_svc_MgrGetListItem(eSvcTable_Service);
	if (pstList)
	{
		db_svc_TableListDump(pstList, fp);
	}
	fclose(fp);
}

dbSvc_TableList_t *	db_svc_MgrGetList (DbSvc_Table_e eTableIdx)
{
	return db_svc_MgrGetListItem(eTableIdx);
}

Handle_t *	db_svc_MgrMakeServiceList (HxVector_t *pstVec, HUINT32 *pulCount)
{
	return db_svc_MgrMakeServiceListInt (pstVec, pulCount);
}


//#undef	__RETURN_IF
//#define	__RETURN_IF	C_RETURN_IF
//#undef	__RETURN_IF_VOID
//#define	__RETURN_IF_VOID	C_RETURN_IF_VOID

HERROR	db_svc_MgrPutAntInfo (HUINT16 usAntIdx, const DbSvc_AntInfo_t *pstAntInfo)
{
	dbSvc_Antenna_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pstAntInfo);

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);
	__RETURN_IF((pstInfo = db_svc_AntennaNew(pstAntInfo)) == NULL, ERR_FAIL);
	pstInfo->nKey = (HINT32)usAntIdx;

	return db_svc_MgrTablePutItem(pstList, (dbSvc_Table_t *)pstInfo);
}


HERROR db_svc_MgrAddAntInfo(
					DbSvc_AntInfo_t 	*pstAntInfo, 	/**< 추가할 antenna info */
					HUINT16 			*pusAntIdx		/**< db에 추가 후 자동 설정된 index */
					)
{
	dbSvc_Antenna_t		*pstInfo = NULL;
	dbSvc_TableList_t	*pstList = NULL;

	HxLOG_Assert(pstAntInfo);

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);
	__RETURN_IF((pstInfo = db_svc_AntennaNew(pstAntInfo)) == NULL, ERR_FAIL);

	if (db_svc_MgrTableAddItem(pstList, (dbSvc_Table_t *)pstInfo) == ERR_OK)
	{
		if (pusAntIdx)
			*pusAntIdx = (HUINT16)pstInfo->nKey;
		return ERR_OK;
	}
	db_svc_AntennaDelete(pstInfo);
	HxLOG_Warning("[%s] failed\n", __FUNCTION__);
	return ERR_FAIL;
}

HERROR db_svc_MgrDeleteAntInfo(
					HUINT16 	usAntIdx		/**< 지우고자 하는 antenna index */
					)
{
	dbSvc_TableList_t	*pstList;

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);
	return db_svc_MgrTableRemoveItemByKey(pstList, (HINT32)usAntIdx);
}

HERROR db_svc_MgrDeleteAllAntInfo(void)
{
	dbSvc_TableList_t	*pstList;

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);
	db_svc_MgrTableClearList(pstList);
	return ERR_OK;
}

HERROR db_svc_MgrUpdateAntInfo(
					HUINT16 			 usAntIdx,		/**< 업데이트 할 antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< 업데이트 내용 */
					)
{
	dbSvc_TableList_t	*pstList;

	if (pstAntInfo == NULL)
		return ERR_FAIL;

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);

	return db_svc_MgrTableUpdateItemByKey(pstList, (HINT32)usAntIdx, pstAntInfo);
}

HERROR db_svc_MgrGetAntInfo(
					HUINT16 			 usAntIdx,		/**< 읽고자 하는 antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< 담아올 구조체 */
					)
{
	dbSvc_Antenna_t		*pstSelf;
	dbSvc_TableList_t	*pstList;

	HxLOG_Assert(pstAntInfo);

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);
	__RETURN_IF((pstSelf = (dbSvc_Antenna_t *)db_svc_MgrTableFindItemByKey(pstList, (HINT32)usAntIdx)) == NULL, ERR_FAIL);

	HxSTD_MemCopy(pstAntInfo, &pstSelf->stData, sizeof(DbSvc_AntInfo_t));
	return ERR_OK;
}

HERROR db_svc_MgrGetAntCount(HINT32 *pnCnt)
{
	dbSvc_TableList_t	*pstList;

	HxLOG_Assert(pnCnt);

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);

	*pnCnt = db_svc_MgrTableCountItem(pstList);
	return ERR_OK;

}

HERROR db_svc_MgrFindAllAntIdxList(HINT32 *pnCnt, HINT32 **panAntIdxList)
{
	dbSvc_TableList_t	*pstList = NULL;
	HINT32				*anAntList = NULL;
	dbSvc_Antenna_t		*pstItem;
	void				*pvIter;
	HINT32				 n, nCount = 0;

	__RETURN_IF((pstList = db_svc_MgrGetListItem(eSvcTable_Antenna)) == NULL, ERR_FAIL);
	__RETURN_IF((n = db_svc_MgrTableCountItem(pstList)) < 0, ERR_FAIL);

	anAntList = (HINT32 *)OxDB_Malloc(sizeof(HINT32) * n);
	if (anAntList == NULL)
	{
		HxLOG_Error("[%s] Out of memory!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	HxTREE_EACH(pstList->pstTree, pvIter, {
		pstItem = (dbSvc_Antenna_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstItem)
			anAntList[nCount++] = pstItem->nKey;
	});

	if (nCount == 0)
	{
		OxDB_Free(anAntList);
		anAntList = NULL;
		HxLOG_Error("[%s] There is no antenna.\n", __FUNCTION__);
	}
	if (pnCnt)
		*pnCnt = nCount;
	if (panAntIdxList)
		*panAntIdxList = anAntList;
	else
		OxDB_Free(anAntList);

	return (nCount == 0) ? ERR_FAIL : ERR_OK;
}


