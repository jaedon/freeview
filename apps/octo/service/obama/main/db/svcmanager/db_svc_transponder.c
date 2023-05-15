/**
	@file     db_svc_transponder.c
	@brief    default setting 관련 API

	Description: db별, 모델 별 달라지는 default 값을 처리하는 API 모음 \n
	Module: DB/default \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <_db_svc_transponder.h>

#include <pdb_svc.h>

#include "_db_svc_common.h"

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

/*******************************************************************/
/**************    local function prototypes    ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC dbSvc_Transponder_t	*db_svc_TransponderCtor (dbSvc_Transponder_t *pstSelf, const DbSvc_TsInfo_t *pstData)
{
	if (pstData)
		HxSTD_MemCopy(&pstSelf->stData, pstData, sizeof(DbSvc_TsInfo_t));
	else
		(void) db_svc_TransponderInitData(&pstSelf->stData);
	return pstSelf;
}

STATIC void			db_svc_TransponderDtor (dbSvc_Transponder_t *pstSelf)
{
	// do nothing...
	(void)pstSelf;
}

STATIC HINT32		db_svc_TransponderGetKey (const dbSvc_Transponder_t *pstSelf)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		return (HINT32)pstSelf->stData.usTsIdx;
	}
	return 0;
}

STATIC void			db_svc_TransponderSetKey (dbSvc_Transponder_t *pstSelf, HINT32 nKey)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		pstSelf->stData.usTsIdx = (HUINT16)nKey;
		pstSelf->stData.usUniqueId = (HUINT16)nKey;
	}
}


STATIC HBOOL		db_svc_TransponderMatch (const dbSvc_Transponder_t *pstTs, const dbSvc_TransponderFilter_t *pstFilter)
{
	const DbSvc_TsInfo_t 		*pstData;
	const DbSvc_TsCondition_t	*pstCond;

	HxLOG_Trace();

	if (pstTs == NULL || pstFilter == NULL)
		return FALSE;

	pstCond = &pstFilter->stCond;
	pstData = &pstTs->stData;

	if (pstCond->nUniqueid >= 0)
	{
		if (pstCond->nUniqueid != (HINT32)pstData->usUniqueId)
			return FALSE;
	}
	if (pstCond->nNetIdx >= 0)
	{
		if (pstCond->nNetIdx != (HINT32)pstData->usNetIdx)
			return FALSE;
	}
	if (pstCond->nTsId >= 0)
	{
		if (pstCond->nTsId != (HINT32)pstData->usTsId)
			return FALSE;
	}
	if (pstCond->nOnId >= 0)
	{
		if (pstCond->nOnId != (HINT32)pstData->usOnId)
			return FALSE;
	}
	if (pstCond->nTunerId >= 0)
	{
		if (pstCond->nTunerId != (HINT32)pstData->ucTunerId)
			return FALSE;
	}
	if (pstCond->nDeliType != eDxDELIVERY_TYPE_ALL)
	{
		if (!(pstCond->nDeliType & (HINT32)pstData->eDeliType))
			return FALSE;
	}
	return TRUE;
}


STATIC HINT32		db_svc_TransponderToString (const dbSvc_Transponder_t *pstSelf, FILE *pfStream)
{
	HINT32					 nRet;
	const DbSvc_TsInfo_t	*pstInfo;
	HERROR					 hErr;
	HUINT8					*aucDefAuth = NULL;

	pstInfo = &pstSelf->stData;
	nRet  = fprintf(pfStream, "Transponder: {\n");
	nRet += fprintf(pfStream, "    usTsIdx	 : %d\n", pstInfo->usTsIdx);
	nRet += fprintf(pfStream, "    usUniqueId: %d\n", pstInfo->usUniqueId);
	nRet += fprintf(pfStream, "    usNetIdx  : %d\n", pstInfo->usNetIdx);
	nRet += fprintf(pfStream, "    usTsId	 : %d\n", pstInfo->usTsId);
	nRet += fprintf(pfStream, "    usOnId	 : %d\n", pstInfo->usOnId);
	nRet += fprintf(pfStream, "    ucTunerId : %d\n", pstInfo->ucTunerId);
	nRet += fprintf(pfStream, "    eDeliType : %d\n", pstInfo->eDeliType);

	hErr = DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)&pstSelf->stData, &aucDefAuth);
	if (ERR_OK == hErr && NULL != aucDefAuth)
	{
		nRet += fprintf(pfStream, "    aucDefaultAuthority: %s\n", aucDefAuth);
	}

	switch (pstInfo->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
	{
		const DbSvc_SatTuningInfo_t *pstSat = &pstInfo->stTuningParam.sat;
		nRet += fprintf(pfStream, "    Satellite :{\n");
		nRet += fprintf(pfStream, "          ulFrequency: %d\n", pstSat->ulFrequency);
		nRet += fprintf(pfStream, "         ulSymbolRate: %d\n", pstSat->ulSymbolRate);
		nRet += fprintf(pfStream, "        ePolarization: %d\n", pstSat->ePolarization);
		nRet += fprintf(pfStream, "         eFecCodeRate: %d\n", pstSat->eFecCodeRate);
		nRet += fprintf(pfStream, "           eTransSpec: %d\n", pstSat->eTransSpec);
		nRet += fprintf(pfStream, "              ePskMod: %d\n", pstSat->ePskMod);
		nRet += fprintf(pfStream, "               ePilot: %d\n", pstSat->ePilot);
		nRet += fprintf(pfStream, "             eRollOff: %d\n", pstSat->eRollOff);
		nRet += fprintf(pfStream, "             eSatType: %d\n", pstSat->eSatType);
		nRet += fprintf(pfStream, "              ucAntId: %d\n", pstSat->ucAntId);
		nRet += fprintf(pfStream, "    }\n");
		break;
	}
	case eDxDELIVERY_TYPE_TER:
	{
		const DbSvc_TerTuningInfo_t *pstTer = &pstInfo->stTuningParam.ter;
		nRet += fprintf(pfStream, "    Terrestrial :{\n");
		nRet += fprintf(pfStream, "                 ulFreq: %d\n", pstTer->ulFreq);
		nRet += fprintf(pfStream, "            ucBandwidth: %d\n", pstTer->ucBandwidth);
		nRet += fprintf(pfStream, "             ucHierachy: %d\n", pstTer->ucHierachy);
		nRet += fprintf(pfStream, "               ucStream: %d\n", pstTer->ucStream);
		nRet += fprintf(pfStream, "             ucCodeRate: %d\n", pstTer->ucCodeRate);
		nRet += fprintf(pfStream, "        ucGuardInterval: %d\n", pstTer->ucGuardInterval);
		nRet += fprintf(pfStream, "            ucTransMode: %d\n", pstTer->ucTransMode);
		nRet += fprintf(pfStream, "               ucOffset: %d\n", pstTer->ucOffset);
		nRet += fprintf(pfStream, "              ucChanNum: %d\n", pstTer->ucChanNum);
		nRet += fprintf(pfStream, "                ucLevel: %d\n", pstTer->ucLevel);
		nRet += fprintf(pfStream, "              ucQuality: %d\n", pstTer->ucQuality);
		nRet += fprintf(pfStream, "    }\n");
		break;
	}
	case eDxDELIVERY_TYPE_CAB:
	{
		const DbSvc_CabTuningInfo_t *pstCab = &pstInfo->stTuningParam.cab;
		nRet += fprintf(pfStream, "    Cable :{\n");
		nRet += fprintf(pfStream, "             ulFreq: %d\n", pstCab->ulFreq);
		nRet += fprintf(pfStream, "               usSR: %d\n", pstCab->usSR);
		nRet += fprintf(pfStream, "        usNetworkId: %d\n", pstCab->usNetworkId);
		nRet += fprintf(pfStream, "              ucCon: %d\n", pstCab->ucCon);
		nRet += fprintf(pfStream, "             ucSpec: %d\n", pstCab->ucSpec);
		nRet += fprintf(pfStream, "    }\n");
		break;
	}
	default:
		break;
	}
	nRet += fprintf(pfStream, "}\n");

	return nRet;
}

STATIC dbSvc_TableClass_t *	db_svc_TransponderClass (void)
{
	STATIC dbSvc_TableClass_t	s_stClass = {
		sizeof(dbSvc_Transponder_t)
		, sizeof(DbSvc_TsInfo_t)
		, "OxTransponder"
		, (dbSvc_Table_t *(*)(dbSvc_Table_t *, void *))db_svc_TransponderCtor
		, (             void (*) (dbSvc_Table_t *))db_svc_TransponderDtor
		, (      HINT32 (*)(const dbSvc_Table_t *))db_svc_TransponderGetKey
		, (      void (*)(dbSvc_Table_t *, HINT32))db_svc_TransponderSetKey
		, (HBOOL (*)(const dbSvc_Table_t *, const dbSvc_TableFilter_t *))db_svc_TransponderMatch
		, (HINT32 (*) (const dbSvc_Table_t *, FILE *))db_svc_TransponderToString
	};
	return &s_stClass;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

DbSvc_TsInfo_t *	db_svc_TransponderInitData (DbSvc_TsInfo_t *pstData)
{
	HxLOG_Trace();
	HxLOG_Assert(pstData);

	HxSTD_MemSet (pstData, 0, sizeof(DbSvc_TsInfo_t));

	return ERR_OK;
}

dbSvc_Transponder_t *	db_svc_TransponderInit (dbSvc_Transponder_t *pstSelf, const DbSvc_TsInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Transponder_t *)db_svc_TableInit(db_svc_TransponderClass(), (dbSvc_Table_t *)pstSelf, (void *)pstData);
}

dbSvc_Transponder_t *	db_svc_TransponderNew (const DbSvc_TsInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Transponder_t *)db_svc_TableNew(db_svc_TransponderClass(), (void *)pstData);
}

dbSvc_Transponder_t *	db_svc_TransponderDelete (dbSvc_Transponder_t *pstSelf)
{
	HxLOG_Trace();

	return (dbSvc_Transponder_t *)db_svc_TableDecref((dbSvc_Table_t *)pstSelf);
}

dbSvc_TransponderFilter_t *	db_svc_TransponderInitFilter (dbSvc_TransponderFilter_t *pstFilter, DbSvc_TsCondition_t *pstCond)
{
	if (pstFilter)
	{
		HxSTD_MemSet(pstFilter, 0, sizeof(dbSvc_TransponderFilter_t));

		(void) db_svc_TableInitFilter(&pstFilter->stFilter);
		pstFilter->stFilter.ulSize = sizeof(dbSvc_Transponder_t);

		if (pstCond)
		{
			HxSTD_MemCopy(&pstFilter->stCond, pstCond, sizeof(DbSvc_TsCondition_t));
			pstCond = NULL;
		}
		else
		{
			pstCond = &pstFilter->stCond;
		}
	}

	if (pstCond)
	{
		pstCond->nUniqueid = -1;
		pstCond->nNetIdx = -1;
		pstCond->nTsId = -1;
		pstCond->nOnId = -1;
		pstCond->nTunerId = -1;
		pstCond->nDeliType = (HINT32)eDxDELIVERY_TYPE_ALL;
	}

	return pstFilter;
}



