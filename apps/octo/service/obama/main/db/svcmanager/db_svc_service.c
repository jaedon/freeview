/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc_service.c
	@brief

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다. 			\n
	Remarks :										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_svc.h>
#include <_db_svc_service.h>
#include <pdb_svc.h>

#include "_db_svc_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/

STATIC dbSvc_Service_t	*db_svc_SerivceCtor (dbSvc_Service_t *pstSelf, const DbSvc_Info_t *pstData)
{
	if (pstData)
		HxSTD_MemCopy(&pstSelf->stData, pstData, sizeof(DbSvc_Info_t));
	else
		(void) db_svc_ServiceInitData(&pstSelf->stData);
	return pstSelf;
}

STATIC void			db_svc_SerivceDtor (dbSvc_Service_t *pstSelf)
{
	// do nothing...
	(void)pstSelf;
}

STATIC HINT32		db_svc_SerivceGetKey (const dbSvc_Service_t *pstSelf)
{
	if (pstSelf)
	{
		return (HINT32)DbSvc_GetSvcIdx(&pstSelf->stData);
	}
	return 0;
}


STATIC void			db_svc_SerivceSetKey (dbSvc_Service_t *pstSelf, HINT32 nKey)
{
	if (pstSelf)
	{
		DbSvc_SetSvcIdx(&pstSelf->stData, nKey);
	}
}


STATIC HBOOL		db_svc_SerivceMatch (const dbSvc_Service_t *pstSvc, const dbSvc_ServiceFilter_t *pstFilter)
{
	if (NULL == pstSvc || NULL == pstFilter)
		return FALSE;

	return pdb_svc_Match(pstSvc, pstFilter);
}


STATIC HINT32	db_svc_SerivceSort_Number (const void *pvArg1, const void *pvArg2)
{
	const dbSvc_Service_t	*pstLhs = *(const dbSvc_Service_t **)pvArg1;
	const dbSvc_Service_t	*pstRhs = *(const dbSvc_Service_t **)pvArg2;

	return (HINT32)DbSvc_GetLcn(&pstLhs->stData) - (HINT32)DbSvc_GetLcn(&pstRhs->stData);
}

STATIC HINT32	db_svc_SerivceSort_Alphabet (const void *pvArg1, const void *pvArg2)
{
	const dbSvc_Service_t	*pstLhs = *(const dbSvc_Service_t **)pvArg1;
	const dbSvc_Service_t	*pstRhs = *(const dbSvc_Service_t **)pvArg2;

	HINT32	diff;

	diff = HxSTD_StrCmp(DbSvc_GetName(&pstLhs->stData), DbSvc_GetName(&pstRhs->stData));
	if (diff == 0)
	{
		return (HINT32)DbSvc_GetLcn(&pstLhs->stData) - (HINT32)DbSvc_GetLcn(&pstRhs->stData);
	}
	return diff;
}


STATIC HINT32	db_svc_SerivceSort_NumberAndSvcType (const void *pvArg1, const void *pvArg2)
{
	const dbSvc_Service_t	*pstLhs = *(const dbSvc_Service_t **)pvArg1;
	const dbSvc_Service_t	*pstRhs = *(const dbSvc_Service_t **)pvArg2;
	HINT32	nDiff;

	nDiff = (HINT32)DbSvc_GetSvcType(&pstLhs->stData) - (HINT32)DbSvc_GetSvcType(&pstRhs->stData);
	if (nDiff == 0)
	{
		return (HINT32)DbSvc_GetLcn(&pstLhs->stData) - (HINT32)DbSvc_GetLcn(&pstRhs->stData);
	}
	return nDiff;
}

STATIC HINT32	db_svc_SerivceSort_Index (const void *pvArg1, const void *pvArg2)
{
	const dbSvc_Service_t	*pstLhs = *(const dbSvc_Service_t **)pvArg1;
	const dbSvc_Service_t	*pstRhs = *(const dbSvc_Service_t **)pvArg2;

	return (HINT32)DbSvc_GetSvcIdx(&pstLhs->stData) - (HINT32)DbSvc_GetSvcIdx(&pstRhs->stData);
}

STATIC HxVector_t *	db_svc_SerivceSort (HxVector_t *pstList, DbSvc_SortType_e eType)
{
	if (pstList == NULL)
		return pstList;

	switch (eType)
	{
	// eOrgDeliType가 아닌, ucChListPriority로 ordering을 함. priority는 configuration되어야한다
	case eSortType_NUMBER:
	// LCN이 지정되어 있으면 LCN 우선, 아니라면 저장된 순서대로
	case eSortType_NUMBER_AND_INDEX:
		HLIB_VECTOR_Sort(pstList, db_svc_SerivceSort_Number);
		break;

	// szSvcName 우선이고, 이름 같으면, number 순으로
	case eSortType_ALPHABET:
		HLIB_VECTOR_Sort(pstList, db_svc_SerivceSort_Alphabet);
		break;

	// { 모든 TV 채널 } -> { 모든 Radio 채널 } 순서
	case eSortType_NUMBER_AND_SVCTYPE:
	// eSortType_NUMBER_AND_SVCTYPE에서 ucChListPriority 제한을 없앤 사양, ucChListPriority는 delivery 순서를 결정하는 기능이다
	case eSortType_NUMBER_AND_SVCTYPE_ONLY:
		HLIB_VECTOR_Sort(pstList, db_svc_SerivceSort_NumberAndSvcType);
		break;

	case eSortType_INDEX:
		HLIB_VECTOR_Sort(pstList, db_svc_SerivceSort_Index);
		break;

	// fav group에서, {Tv 전체} --> {Radio 전체} 로 보이기 위한 사양
	case eSortType_FAV_INDEX_AND_SVCTYPE:
	default:
		return pstList;
	}
	return pstList;
}

HINT32	db_svc_SerivcePrint(const HCHAR *pszMsg, const DbSvc_Info_t *pstInfo, FILE *pfStream)
{
	HINT32	nRet;

	nRet  = fprintf(pfStream, "%s: {\n", pszMsg);
	nRet += fprintf(pfStream, "    usSvcIdx    : %d\n", DbSvc_GetSvcIdx(pstInfo));
	nRet += fprintf(pfStream, "    usTsIdx     : %d\n", DbSvc_GetTsIdx(pstInfo));
	nRet += fprintf(pfStream, "    usSvcId     : %d\n", DbSvc_GetSvcId(pstInfo));
	nRet += fprintf(pfStream, "    usTsId      : %d\n", DbSvc_GetTsId(pstInfo));
	nRet += fprintf(pfStream, "    usOnId      : %d\n", DbSvc_GetOnId(pstInfo));
	nRet += fprintf(pfStream, "    szSvcName   : %s\n", DbSvc_GetName(pstInfo));
	nRet += fprintf(pfStream, "    usLcn       : %d\n", DbSvc_GetLcn(pstInfo));
	nRet += fprintf(pfStream, "    eSvcType    : %d\n", DbSvc_GetSvcType(pstInfo));
	nRet += fprintf(pfStream, "    eVideoType  : %d\n", DbSvc_GetVideoType(pstInfo));
	nRet += fprintf(pfStream, "    eCasType    : %d\n", DbSvc_GetCasType(pstInfo));
	nRet += fprintf(pfStream, "    eSvcOpType  : %d\n", DbSvc_GetOpType(pstInfo));
#if 0
#ifdef CONFIG_UK_FREESAT
	nRet += fprintf(pfStream, "    usFSatSvcIdentifier: %d\n", XdbSvc_FreeSatGetSvcIdentifierPtr(pstInfo));
	nRet += fprintf(pfStream, "    usFSatReginId      : %d\n", XdbSvc_FreeSatGetSatRegionIdPtr(pstInfo));
#endif
#endif
	nRet += fprintf(pfStream, "    eOrgDeliType: %d\n", DbSvc_GetDeliTypePtr(pstInfo));
	nRet += fprintf(pfStream, "    ucAntIdx    : %d\n", DbSvc_GetAntIdx(pstInfo));
	nRet += fprintf(pfStream, "    bLocked     : %d\n", DbSvc_GetLocked(pstInfo));
	nRet += fprintf(pfStream, "    usPmtPid    : %d\n", DbSvc_GetPmtPid(pstInfo));
	nRet += fprintf(pfStream, "    usPcrPid    : %d\n", DbSvc_GetPcrPid(pstInfo));
	nRet += fprintf(pfStream, "    usTtxPid    : %d\n", DbSvc_GetTtxPid(pstInfo));
	nRet += fprintf(pfStream, "    usVideoPid  : %d\n", DbSvc_GetVideoPid(pstInfo));
	nRet += fprintf(pfStream, "    usAudioPid  : %d\n", DbSvc_GetAudioPid(pstInfo));
	nRet += fprintf(pfStream, "    AudioAuxPid : %d\n", DbSvc_GetAudioAuxPid(pstInfo));
	nRet += fprintf(pfStream, "    usDolbyPid  : %d\n", DbSvc_GetDolbyPid(pstInfo));
	nRet += fprintf(pfStream, "    eUserFlag1  : %08X\n", DB_SVC_GetUserFlag1(pstInfo));
	nRet += fprintf(pfStream, "}\n");

	return nRet;
}

STATIC HINT32		db_svc_SerivceToString (const dbSvc_Service_t *pstSelf, FILE *pfStream)
{
	return db_svc_SerivcePrint("Service", &pstSelf->stData, pfStream);
}

STATIC dbSvc_TableClass_t *	db_svc_SerivceClass (void)
{
	STATIC dbSvc_TableClass_t	s_stClass = {
		  sizeof(dbSvc_Service_t)
		, sizeof(DbSvc_Info_t)
		, "OxService"
		, (dbSvc_Table_t *(*)(dbSvc_Table_t *, void *))db_svc_SerivceCtor
		, (             void (*) (dbSvc_Table_t *))db_svc_SerivceDtor
		, (      HINT32 (*)(const dbSvc_Table_t *))db_svc_SerivceGetKey
		, (      void (*)(dbSvc_Table_t *, HINT32))db_svc_SerivceSetKey
		, (HBOOL (*)(const dbSvc_Table_t *, const dbSvc_TableFilter_t *))db_svc_SerivceMatch
		, (HINT32 (*) (const dbSvc_Table_t *, FILE *))db_svc_SerivceToString
	};
	return &s_stClass;
}

/*
STATIC void	dump_map (HxHASH_t *map, const HCHAR *msg)
{
	STATIC HINT32	i;
	FILE *fp;
	HxHASH_TableIter_t	iter;
	HCHAR	buf[256];
	HUINT32	key;

	HxSTD_PrintToStrN(buf, 256, "map.%04d.%s.txt", i++, msg);

	fp = fopen(buf, "w");
	HLIB_HASH_IterInit(&iter, map);
	while (HLIB_HASH_IterNext(&iter, (void **)&key, NULL))
	{
		HINT32	tsid = key >> 16;
		HINT32	sid  = key & 0xFFFF;

		fprintf(fp, "tsid:%10d, sid:%10d\n", tsid, sid);
	}
	fclose(fp);
}
*/

STATIC HxHASH_t * db_svc_SerivceBuildTripletIndexMap (dbSvc_TableList_t *pstSelf)
{
	HxHASH_t		*pstMap;
	void			*pvIter;
	dbSvc_Service_t	*pstSvc;
	HxVector_t		*pstList;
	HINT32			 nKey;

	if (HLIB_TREE_Nnodes(pstSelf->pstTree) == 0)
		return NULL;

	pstMap = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, (_HxDestroyNotify)HLIB_VECTOR_Delete);
	if (pstMap == NULL)
		return NULL;

	for (pvIter = HLIB_TREE_Iterator(pstSelf->pstTree) ; pvIter ; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		pstSvc  = (dbSvc_Service_t *)HLIB_TREE_IteratorValue(pvIter);
		if (pstSvc == NULL)
			continue;

		nKey  = (HINT32)(DbSvc_GetTsId(&pstSvc->stData) << 16 | DbSvc_GetSvcId(&pstSvc->stData));
		pstList = (HxVector_t *)HLIB_HASH_Lookup(pstMap, (void *)nKey);
		if (pstList == NULL)
		{
			pstList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))db_svc_TableDecref, NULL);
			if (pstList == NULL)
				continue;

			HLIB_HASH_Insert(pstMap, (void *)nKey, (void *)pstList);
		}
		HLIB_VECTOR_Add(pstList, (void *)db_svc_TableIncref((dbSvc_Table_t *)pstSvc));
	}
	return pstMap;
}


STATIC HxVector_t *	db_svc_SerivceDoOptimizedFiltering (dbSvc_TableList_t *pstSelf, const dbSvc_ServiceFilter_t *pstFilter, HBOOL *pbPass)
{
	HxVector_t				*pstList;
	HxVector_t				*pstResult;
	dbSvc_Table_t			*pstTable;
	dbSvc_ServiceFilter_t	 stOptimized;

	*pbPass  = FALSE;
	pstResult = NULL;

	// index 0: key (tsid + svcid)
	if (pstFilter->nSvcId >= 0 && pstFilter->nTsId >= 0)
	{
		*pbPass = TRUE;

		HxSTD_MemCopy(&stOptimized, pstFilter, sizeof(dbSvc_ServiceFilter_t));
		stOptimized.nSvcId = -1;
		stOptimized.nTsId  = -1;

		if (pstSelf->apstIndexMap[0] == NULL)
		{
			pstSelf->apstIndexMap[0] = db_svc_SerivceBuildTripletIndexMap(pstSelf);
			if (pstSelf->apstIndexMap[0] == NULL)
				return NULL;
		}

		pstList = (HxVector_t *)HLIB_HASH_Lookup(pstSelf->apstIndexMap[0], (void *)(pstFilter->nTsId << 16 | pstFilter->nSvcId));
		if (pstList == NULL)
			return NULL;

		HxVECTOR_EACH(pstList, dbSvc_Table_t *, pstTable, {
			if (db_svc_TableMatch(pstTable, (dbSvc_TableFilter_t *)&stOptimized))
			{
				if (pstResult == NULL)
				{
					pstResult = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))db_svc_TableDecref, NULL);
					if (pstResult == NULL)
						break;
				}
				HLIB_VECTOR_Add(pstResult, (void *)db_svc_TableIncref(pstTable));
			}
		});
		return pstResult;
	}

	// Add more stOptimized filtering here...
	return NULL;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

DbSvc_Info_t	*	db_svc_ServiceInitData (DbSvc_Info_t *pstData)
{
	HxLOG_Assert(pstData);

	HxSTD_MemSet (pstData, 0, sizeof(DbSvc_Info_t));

	// PIDs
	DbSvc_SetPmtPid(pstData, PID_NULL);
	DbSvc_SetPcrPid(pstData, PID_NULL);
	DbSvc_SetVideoPid(pstData, PID_NULL);
	DbSvc_SetAudioPid(pstData, PID_NULL);
	DbSvc_SetDolbyPid(pstData, PID_NULL);
	DbSvc_SetAudioAuxPid(pstData, PID_NULL);
	DbSvc_SetTtxPid(pstData, PID_NULL);
	DbSvc_SetSubttlIdx(pstData, 0xff);		// SUBTITLE_INDEX_NULL;

	// Codec Information
	DbSvc_SetVideoCodec(pstData, eDxVIDEO_CODEC_UNKNOWN);
	DbSvc_SetAudioCodec(pstData, eDxAUDIO_CODEC_UNKNOWN);
	DbSvc_SetDolbyCodec(pstData, eDxAUDIO_CODEC_UNKNOWN);

	DbSvc_SetAudUserSelected(pstData, FALSE);
	DbSvc_SetDolbyFlag(pstData, FALSE);

	// Visible : TRUE가 기본
	DbSvc_SetVisibleFlag(pstData, TRUE);

	// OP-Type : NONE
	DbSvc_SetOpType(pstData, eDxOPERATOR_None);

	// Section : Non LCN 이 기본
	DbSvc_SetSvcSection(pstData, eDxSvcSection_NonLcn_Priority1);

	return ERR_OK;
}


dbSvc_Service_t *	db_svc_ServiceInit (dbSvc_Service_t *pstSelf, const DbSvc_Info_t *pstData)
{
	return (dbSvc_Service_t *)db_svc_TableInit(db_svc_SerivceClass(), (dbSvc_Table_t *)pstSelf, (void *)pstData);
}

dbSvc_Service_t	*	db_svc_ServiceNew (const DbSvc_Info_t *pstData)
{
	return (dbSvc_Service_t *)db_svc_TableNew(db_svc_SerivceClass(), (void *)pstData);
}


dbSvc_Service_t *	db_svc_ServiceDelete (dbSvc_Service_t *pstSelf)
{
	return (dbSvc_Service_t *)db_svc_TableDecref((dbSvc_Table_t *)pstSelf);
}


dbSvc_ServiceFilter_t *	db_svc_ServiceInitFilter (dbSvc_ServiceFilter_t *pstFilter)
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

dbSvc_ServiceFilter_t *	db_svc_ServiceConditionToFilter (dbSvc_ServiceFilter_t *pstFilter, const DbSvc_Condition_t *pstCond)
{
	if (pstFilter == NULL || pstCond == NULL)
		return NULL;

	(void)db_svc_ServiceInitFilter(pstFilter);

	pstFilter->nAntIdx      = pstCond->nAntIdx;
	pstFilter->nOnId        = pstCond->nOnId;
	pstFilter->nSvcId       = pstCond->nSvcId;
	pstFilter->nTsId        = pstCond->nTsId;
	pstFilter->nTsIdx       = pstCond->nTsIdx;
	pstFilter->nPrvIdx      = pstCond->nPrvIdx;
	pstFilter->nLcnFlag     = pstCond->nLcnFlag;
	pstFilter->nStartLcn    = pstCond->nStartLcn;
	pstFilter->nEndLcn      = pstCond->nEndLcn;
	pstFilter->eSvcType     = (DxSvcType_e)pstCond->nSvcType;
	pstFilter->eVideoType   = (DxVideoType_e)pstCond->nVideoType;
	pstFilter->eCasType     = (DxCasType_e)pstCond->nCasType;
	pstFilter->eOrgDeliType = (DxDeliveryType_e)pstCond->nOrgDeliType;
	pstFilter->eSection     = (DxSvcSection_e)pstCond->nSection;
	pstFilter->eSvcOpType   = (DxOperator_e)pstCond->eSvcOpType;
	pstFilter->eSatType     = (SVC_SatType_t)pstCond->eSatType;
	return pstFilter;
}

dbSvc_ServiceFilter_t *	db_svc_ServiceAttrToFilter (dbSvc_ServiceFilter_t *pstFilter, const DbSvc_Attr_t *pstAttr)
{
	if (pstFilter == NULL || pstAttr == NULL)
		return NULL;

	(void)db_svc_ServiceInitFilter(pstFilter);

	pstFilter->eSvcType     = pstAttr->eSvcType;
	pstFilter->eVideoType   = pstAttr->eVideoType;
	pstFilter->eSatType     = pstAttr->eSatType;
	pstFilter->eCasType     = pstAttr->eCasType;
	pstFilter->eOrgDeliType = pstAttr->eOrgDeliType;
	pstFilter->eSortType    = pstAttr->eSortType;
	pstFilter->eVisibleType = pstAttr->eVisibleType;
	pstFilter->nBouqIdx     = pstAttr->nBouqIdx;
	pstFilter->nGenreIdx    = pstAttr->nGenreIdx;
	pstFilter->eSection     = pstAttr->eSection;
	pstFilter->eSvcOpType   = pstAttr->eSvcOpType;
	pstFilter->bNewSvc      = pstAttr->bNewSvc;
	pstFilter->bwUserFlag1   = pstAttr->eUserFlag1;
	pstFilter->bwUserMask1   = pstAttr->eUserMask1;
	pstFilter->eJpNetType 	= pstAttr->eJpNetType;

	return pstFilter;
}


HxVector_t	*db_svc_ServiceDoFiltering (dbSvc_TableList_t *pstSelf, const dbSvc_ServiceFilter_t *pstFilter)
{
	HBOOL		 bOptimized;
	HxVector_t	*pstList;

	pstList = db_svc_SerivceDoOptimizedFiltering(pstSelf, pstFilter, &bOptimized);
	if (!bOptimized)
	{
		pstList = db_svc_TableListDoFiltering(pstSelf, (const dbSvc_TableFilter_t *)pstFilter);
	}

	return db_svc_SerivceSort(pstList, pstFilter->eSortType);
}

#if 0
HxVector_t	*db_svc_SerivceDoFiltering1 (dbSvc_TableList_t *pstSelf, const dbSvc_ServiceFilter_t *pstFilter)
{
	HxVector_t	*pstList;

	pstList = db_svc_TableListDoFiltering(pstSelf, (const dbSvc_TableFilter_t *)pstFilter);
	if (pstList == NULL)
		return pstList;

	return db_svc_SerivceSort(pstList, pstFilter->eSortType);
}


HxVector_t	*db_svc_ServiceDoFiltering (dbSvc_TableList_t *pstSelf, const dbSvc_ServiceFilter_t *pstFilter)
{
	HxVector_t		*pstList;
	HUINT32			tick;
	dbSvc_Table_t	*pstTable;

	HxLOG_Trace();

	if (pstFilter->nSvcId >= 0 && pstFilter->nTsId >= 0)
	{
		HLIB_CMD_Printf("\n\n\tjhlee4:svcid:%d, tsid:%d, onid:%d\n", pstFilter->nSvcId, pstFilter->nTsId, pstFilter->nOnId);
		tick = HLIB_STD_GetSystemTick();
		pstList = db_svc_SerivceDoFiltering1(pstSelf, pstFilter);
		HLIB_CMD_Printf("\n\n\tjhlee4:%s OLD(tick:%u), result:%d\n", __FUNCTION__, HLIB_STD_GetSystemTick() - tick, pstList ? pstList->count : 0);
		if (pstList)
		{
			HxVECTOR_EACH(pstList, dbSvc_Table_t *, pstTable, {
				db_svc_TableToString(pstTable, stdout);
			});
			HLIB_VECTOR_Delete(pstList);
		}

		tick = HLIB_STD_GetSystemTick();
		pstList = db_svc_SerivceDoFiltering2(pstSelf, pstFilter);
		HLIB_CMD_Printf("\n\n\tjhlee4:%s NEW(tick:%u), result:%d\n", __FUNCTION__, HLIB_STD_GetSystemTick() - tick, pstList ? pstList->count : 0);
		if (pstList)
		{
			HxVECTOR_EACH(pstList, dbSvc_Table_t *, pstTable, {
				db_svc_TableToString(pstTable, stdout);
			});
		}
		return pstList;
	}
	return db_svc_SerivceDoFiltering1(pstSelf, pstFilter);
}
#endif

