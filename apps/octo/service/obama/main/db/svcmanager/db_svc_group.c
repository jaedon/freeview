/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc_group.c
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

#include <_db_svc_group.h>

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

STATIC dbSvc_Group_t	*db_svc_GroupCtor (dbSvc_Group_t *pstSelf, const DbSvc_GroupInfo_t *pstData)
{
	if (pstData)
		HxSTD_MemCopy(&pstSelf->stData, pstData, sizeof(DbSvc_GroupInfo_t));
	else
		(void) db_svc_GroupInitData(&pstSelf->stData);
	return pstSelf;
}

STATIC void			db_svc_GroupDtor (dbSvc_Group_t *pstSelf)
{
	// do nothing...
	(void)pstSelf;
}

STATIC HINT32		db_svc_GroupGetKey (const dbSvc_Group_t *pstSelf)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		return (HINT32)pstSelf->stData.usGroupIdx;
	}
	return 0;
}

STATIC void			db_svc_GroupSetKey (dbSvc_Group_t *pstSelf, HINT32 key)
{
	HxLOG_Trace();
	if (pstSelf)
	{
		pstSelf->stData.usGroupIdx = (HUINT16)key;
	}
}


STATIC HINT32		db_svc_GroupToString (const dbSvc_Group_t *pstSelf, FILE *pfStream)
{
	HINT32	nRet;

	nRet = fprintf(pfStream
			, "Group: {\n"
			  "     usGroupIdx: %d\n"
			  "      ulGroupId: %d\n"
		#if 0		//#ifdef CONFIG_UK_FREESAT
			  "    ucGroupType: %d\n"
			  "      tune flag: %d\n"
			  "    access flag: %d\n"
			  "      lang code: %s\n"
		#endif
			  "     group name: %s\n"
			  "}\n"
			, pstSelf->stData.usGroupIdx
			, pstSelf->stData.ulGroupId
		#if 0		//#ifdef CONFIG_UK_FREESAT
			, XdbSvc_FreeSatGroupGetGroupType(pstSelf->stData)
			, XdbSvc_FreeSatGroupGetNondestructiveTuneFlag(pstSelf->stData)
			, XdbSvc_FreeSatGroupGetReturnChannelAccessFlag(pstSelf->stData)
			, XdbSvc_FreeSatGroupGetIso639LangCode(pstSelf->stData)
		#endif
			, pstSelf->stData.szGroupName
		);

	return nRet;
}


STATIC dbSvc_TableClass_t *	db_svc_GroupClass (void)
{
	STATIC dbSvc_TableClass_t	s_stClass = {
		sizeof(dbSvc_Group_t)
		, sizeof(DbSvc_GroupInfo_t)
		, "OxGroup"
		, (dbSvc_Table_t *(*)(dbSvc_Table_t *, void *))db_svc_GroupCtor
		, (             void (*) (dbSvc_Table_t *))db_svc_GroupDtor
		, (      HINT32 (*)(const dbSvc_Table_t *))db_svc_GroupGetKey
		, (      void (*)(dbSvc_Table_t *, HINT32))db_svc_GroupSetKey
		, NULL
		, (HINT32 (*) (const dbSvc_Table_t *, FILE *))db_svc_GroupToString
	};
	return &s_stClass;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

DbSvc_GroupInfo_t *	db_svc_GroupInitData (DbSvc_GroupInfo_t *pstData)
{
	HxLOG_Trace();
	HxLOG_Assert(pstData);

	HxSTD_MemSet(pstData, 0, sizeof(DbSvc_GroupInfo_t));

	return ERR_OK;
}

dbSvc_Group_t *	db_svc_GroupInit (dbSvc_Group_t *pstSelf, const DbSvc_GroupInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Group_t *)db_svc_TableInit(db_svc_GroupClass(), (dbSvc_Table_t *)pstSelf, (void *)pstData);
}

dbSvc_Group_t *	db_svc_GroupNew (const DbSvc_GroupInfo_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Group_t *)db_svc_TableNew(db_svc_GroupClass(), (void *)pstData);
}


dbSvc_Group_t *	db_svc_GroupDelete (dbSvc_Group_t *pstData)
{
	HxLOG_Trace();

	return (dbSvc_Group_t *)db_svc_TableDecref((dbSvc_Table_t *)pstData);
}

