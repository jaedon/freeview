/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc_network.c
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

STATIC dbSvc_Network_t	*db_svc_NetworkCtor (dbSvc_Network_t *pstSelf, const DbSvc_NetInfo_t *pstData)
{
	if (pstData)
		HxSTD_MemCopy(&pstSelf->stData, pstData, sizeof(DbSvc_NetInfo_t));
	else
		(void) db_svc_NetworkInitData(&pstSelf->stData);
	return pstSelf;
}

STATIC void			db_svc_NetworkDtor (dbSvc_Network_t *pstSelf)
{
	// do nothing...
	(void)pstSelf;
}

STATIC HINT32		db_svc_NetworkGetKey (const dbSvc_Network_t *pstSelf)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		return (HINT32)pstSelf->stData.usNetIdx;
	}
	return 0;
}


STATIC void			db_svc_NetworkSetKey (dbSvc_Network_t *pstSelf, HINT32 nKey)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		pstSelf->stData.usNetIdx = (HUINT16)nKey;
	}
}


STATIC HINT32		db_svc_NetworkToString (const dbSvc_Network_t *pstSelf, FILE *pfStream)
{
	HINT32	ret;
	HUINT8	*aucDefAuth = NULL;
	HERROR	hErr;

	hErr = DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)&pstSelf->stData, &aucDefAuth);
	if (ERR_OK != hErr || NULL == aucDefAuth)
	{
		ret = fprintf(pfStream
				, "Network: {\n"
				  " 			usNetIdx: %d\n"
				  " 		eOrgDeliType: %d\n"
				  " 			 usNetId: %d\n"
				  " 		   szNetName: %s\n"
				  "}\n"
				, pstSelf->stData.usNetIdx
				, pstSelf->stData.eOrgDeliType
				, pstSelf->stData.usNetId
				, pstSelf->stData.szNetName
			);
	}
	else
	{
		ret = fprintf(pfStream
				, "Network: {\n"
				  " 			usNetIdx: %d\n"
				  " 		eOrgDeliType: %d\n"
				  " 			 usNetId: %d\n"
				  " 		   szNetName: %s\n"
				  "    default authority: %s\n"
				  "}\n"
				, pstSelf->stData.usNetIdx
				, pstSelf->stData.eOrgDeliType
				, pstSelf->stData.usNetId
				, pstSelf->stData.szNetName
				, aucDefAuth
			);
	}

	return ret;
}


STATIC dbSvc_TableClass_t *	db_svc_NetworkClass (void)
{
	STATIC dbSvc_TableClass_t	s_stClass = {
		sizeof(dbSvc_Network_t)
		, sizeof(DbSvc_NetInfo_t)
		, "OxNetwork"
		, (dbSvc_Table_t *(*)(dbSvc_Table_t *, void *))db_svc_NetworkCtor
		, (             void (*) (dbSvc_Table_t *))db_svc_NetworkDtor
		, (      HINT32 (*)(const dbSvc_Table_t *))db_svc_NetworkGetKey
		, (      void (*)(dbSvc_Table_t *, HINT32))db_svc_NetworkSetKey
		, NULL
		, (HINT32 (*) (const dbSvc_Table_t *, FILE *))db_svc_NetworkToString
	};
	return &s_stClass;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

DbSvc_NetInfo_t *	db_svc_NetworkInitData (DbSvc_NetInfo_t *pstData)
{
	HxLOG_Trace();
	HxLOG_Assert(pstData);

	HxSTD_MemSet(pstData, 0, sizeof(DbSvc_NetInfo_t));

	return ERR_OK;
}

dbSvc_Network_t *	db_svc_NetworkInit (dbSvc_Network_t *pstSelf, const DbSvc_NetInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Network_t *)db_svc_TableInit(db_svc_NetworkClass(), (dbSvc_Table_t *)pstSelf, (void *)pstData);
}

dbSvc_Network_t	*	db_svc_NetworkNew (const DbSvc_NetInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Network_t *)db_svc_TableNew(db_svc_NetworkClass(), (void *)pstData);
}

dbSvc_Network_t *	db_svc_NetworkDelete (dbSvc_Network_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Network_t *)db_svc_TableDecref((dbSvc_Table_t *)pstData);
}




