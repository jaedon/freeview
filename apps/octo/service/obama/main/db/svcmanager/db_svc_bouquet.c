/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc_bouquet.h
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

STATIC dbSvc_Bouquet_t	*db_svc_BouquetCtor (dbSvc_Bouquet_t *pstSelf, const DbSvc_BouqInfo_t *pstData)
{
	if (pstData)
		HxSTD_MemCopy(&pstSelf->stData, pstData, sizeof(DbSvc_BouqInfo_t));
	else
		(void) db_svc_BouquetInitData(&pstSelf->stData);
	return pstSelf;
}

STATIC void			db_svc_BouquetDtor (dbSvc_Bouquet_t *pstSelf)
{
	// do nothing...
	(void)pstSelf;
}

STATIC HINT32		db_svc_BouquetGetKey (const dbSvc_Bouquet_t *pstSelf)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		return (HINT32)pstSelf->stData.usBouqIdx;
	}
	return 0;
}

STATIC void			db_svc_BouquetSetKey (dbSvc_Bouquet_t *pstSelf, HINT32 nKey)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		pstSelf->stData.usBouqIdx = (HUINT16)nKey;
	}
}


STATIC HINT32		db_svc_BouquetToString (const dbSvc_Bouquet_t *pstSelf, FILE *pfStream)
{
	HINT32	ret;

	ret = fprintf(pfStream
			, "Bouquet: {\n"
			  "      usBouqIdx: %d\n"
			  "       usBouqId: %d\n"
			  "   bouquet name: %s\n"
			  "}\n"
			, pstSelf->stData.usBouqIdx
			, pstSelf->stData.usBouqId
			, pstSelf->stData.szBouqName
		);

	return ret;
}


STATIC dbSvc_TableClass_t *	db_svc_BouquetClass (void)
{
	STATIC dbSvc_TableClass_t	s_stClass = {
		sizeof(dbSvc_Bouquet_t)
		, sizeof(DbSvc_BouqInfo_t)
		, "OxBouquet"
		, (dbSvc_Table_t *(*)(dbSvc_Table_t *, void *))db_svc_BouquetCtor
		, (             void (*) (dbSvc_Table_t *))db_svc_BouquetDtor
		, (      HINT32 (*)(const dbSvc_Table_t *))db_svc_BouquetGetKey
		, (      void (*)(dbSvc_Table_t *, HINT32))db_svc_BouquetSetKey
		, NULL
		, (HINT32 (*) (const dbSvc_Table_t *, FILE *))db_svc_BouquetToString
	};
	return &s_stClass;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

DbSvc_BouqInfo_t *	db_svc_BouquetInitData (DbSvc_BouqInfo_t *pstData)
{
	HxLOG_Trace();
	HxLOG_Assert(pstData);

	HxSTD_MemSet(pstData, 0, sizeof(DbSvc_BouqInfo_t));

	return ERR_OK;
}


dbSvc_Bouquet_t *	db_svc_BouquetInit (dbSvc_Bouquet_t *pstSelf, const DbSvc_BouqInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Bouquet_t *)db_svc_TableInit(db_svc_BouquetClass(), (dbSvc_Table_t *)pstSelf, (void *)pstData);
}

dbSvc_Bouquet_t *	db_svc_BouquetNew (const DbSvc_BouqInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Bouquet_t *)db_svc_TableNew(db_svc_BouquetClass(), (void *)pstData);
}


dbSvc_Bouquet_t *	db_svc_BouquetDelete (dbSvc_Bouquet_t *pstSelf)
{
	HxLOG_Trace();

	return (dbSvc_Bouquet_t *)db_svc_TableDecref((dbSvc_Table_t *)pstSelf);
}

