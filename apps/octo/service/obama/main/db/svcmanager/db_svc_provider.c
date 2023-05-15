/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc_provider.c
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

STATIC dbSvc_Provider_t	*db_svc_ProviderCtor (dbSvc_Provider_t *pstSelf, const DbSvc_PrvInfo_t *pstData)
{
	if (pstData)
		HxSTD_MemCopy(&pstSelf->stData, pstData, sizeof(DbSvc_PrvInfo_t));
	else
		(void) db_svc_ProviderInitData(&pstSelf->stData);
	return pstSelf;
}

STATIC void			db_svc_ProviderDtor (dbSvc_Provider_t *pstSelf)
{
	// do nothing...
	(void)pstSelf;
}

STATIC HINT32		db_svc_ProviderGetKey (const dbSvc_Provider_t *pstSelf)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		return (HINT32)pstSelf->stData.usPrvIdx;
	}
	return 0;
}

STATIC void			db_svc_ProviderSetKey (dbSvc_Provider_t *pstSelf, HINT32 nKey)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		pstSelf->stData.usPrvIdx = (HUINT16)nKey;
	}
}

STATIC HINT32		db_svc_ProviderToString (const dbSvc_Provider_t *pstSelf, FILE *pfStream)
{
	HINT32	nRet;

	nRet = fprintf(pfStream
			, "Provider: {\n"
			  "     usPrvIdx: %d\n"
			  "    szPrvName: %s\n"
			  "}\n"
			, pstSelf->stData.usPrvIdx
			, pstSelf->stData.szPrvName
		);

	return nRet;
}


STATIC dbSvc_TableClass_t *	db_svc_ProviderClass (void)
{
	STATIC dbSvc_TableClass_t	s_stClass = {
		sizeof(dbSvc_Provider_t)
		, sizeof(DbSvc_PrvInfo_t)
		, "OxProvider"
		, (dbSvc_Table_t *(*)(dbSvc_Table_t *, void *))db_svc_ProviderCtor
		, (             void (*) (dbSvc_Table_t *))db_svc_ProviderDtor
		, (      HINT32 (*)(const dbSvc_Table_t *))db_svc_ProviderGetKey
		, (      void (*)(dbSvc_Table_t *, HINT32))db_svc_ProviderSetKey
		, NULL
		, (HINT32 (*) (const dbSvc_Table_t *, FILE *))db_svc_ProviderToString
	};
	return &s_stClass;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

DbSvc_PrvInfo_t *	db_svc_ProviderInitData (DbSvc_PrvInfo_t *pstData)
{
	HxLOG_Trace();
	HxLOG_Assert(pstData);

	HxSTD_MemSet (pstData, 0, sizeof(DbSvc_PrvInfo_t));

	return ERR_OK;
}


dbSvc_Provider_t *	db_svc_ProviderInit (dbSvc_Provider_t *pstSelf, const DbSvc_PrvInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Provider_t *)db_svc_TableInit(db_svc_ProviderClass(), (dbSvc_Table_t *)pstSelf, (void *)pstData);
}

dbSvc_Provider_t *	db_svc_ProviderNew (const DbSvc_PrvInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Provider_t *)db_svc_TableNew(db_svc_ProviderClass(), (void *)pstData);
}


dbSvc_Provider_t *	db_svc_ProviderDelete (dbSvc_Provider_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Provider_t *)db_svc_TableDecref((dbSvc_Table_t *)pstData);
}

