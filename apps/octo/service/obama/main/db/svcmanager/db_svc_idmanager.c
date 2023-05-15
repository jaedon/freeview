/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc_idmanager.h
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

#include <_db_svc_table.h>

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
typedef struct
{
	HUINT32		nMaxId;
	HUINT32		nLastId;
	HBOOL		(* pfnHasId) (HUINT32, void *);
	void *		pvData;
} dbSvc_IdManager_t;

/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

Handle_t	db_svc_IdmanagerNew (HUINT32 nMaxId, HUINT32 nLastId, HBOOL (* pfnHasId) (HUINT32, void *), void *pvData)
{
	dbSvc_IdManager_t	stSelf;

	HxLOG_Trace();
	HxLOG_Assert(nMaxId && pfnHasId);

	stSelf.nMaxId  = nMaxId;
	stSelf.nLastId = nLastId;
	stSelf.pfnHasId  = pfnHasId;
	stSelf.pvData  = pvData;

	return (Handle_t)HLIB_STD_MemDup(&stSelf, sizeof(dbSvc_IdManager_t));
}

void		db_svc_IdmanagerDelete (Handle_t hMgr)
{
	dbSvc_IdManager_t *pstSelf = (dbSvc_IdManager_t *)hMgr;

	HxLOG_Trace();
	if (pstSelf)
	{
		HLIB_STD_MemFree(pstSelf);
	}
}

HUINT32		db_svc_IdmanagerGetLastId (Handle_t hMgr)
{
	dbSvc_IdManager_t *pstSelf   = (dbSvc_IdManager_t *)hMgr;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	return pstSelf->nLastId;
}

void		db_svc_IdmanagerSetLastId (Handle_t hMgr, HUINT32 nLastId)
{
	dbSvc_IdManager_t *pstSelf   = (dbSvc_IdManager_t *)hMgr;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	pstSelf->nLastId = nLastId;
}

HUINT32		db_svc_IdmanagerNextId (Handle_t hMgr)
{
	dbSvc_IdManager_t *pstSelf   = (dbSvc_IdManager_t *)hMgr;
	HUINT32        randId;

	HxLOG_Trace();
	HxLOG_Assert(pstSelf);

	if (pstSelf->nLastId < pstSelf->nMaxId)
	{
		return ++pstSelf->nLastId;
	}

	if (pstSelf->pfnHasId == NULL)
		return 0;

	for (;;)
	{
		randId = HLIB_MATH_Random() % pstSelf->nMaxId + 1;
		if (!pstSelf->pfnHasId(randId, pstSelf->pvData))
			break;
	}
	return randId;
}

