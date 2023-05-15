/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _db_svc_table.h
	@brief

	Description:  \n
	Module: DB/ \n
	Remarks :										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef __DB_SM_TABLE_H__
#define __DB_SM_TABLE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <db_svc.h>


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define	DB_SVC_TABLE_MAX_EXT_FILTER		10
#define	DB_SVC_TABLE_MAX_INDEX_MAP		3

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eDbSvc_TableFlag_NULL		= 0,
	eDbSvc_TableFlag_SEARCH		= (1 << 0),
	eDbSvc_TableFlag_CHANGED	= (1 << 1),

	eDbSvc_TableFlag_ALL		= 0xFFFFFFFF
} dbSvc_TableFlags_e, dbSvc_TableFlags_b;

typedef struct _dbSvc_Table dbSvc_Table_t;

typedef struct
{
	HBOOL	(* fnMatch) (const dbSvc_Table_t *pstTable, void *pvUserData);
	void 	*pvUserData;
} dbSvc_TableFilterEx_t;

typedef struct
{
	HUINT32					ulSize;
	HUINT32					ulFlags;
	HINT32					nKey;
	HINT32					nCount;
	dbSvc_TableFilterEx_t	astExt[DB_SVC_TABLE_MAX_EXT_FILTER];
} dbSvc_TableFilter_t;

typedef struct
{
	//Initiated by db_svc_SerivceClass(), db_svc_TransponderClass(), db_svc_NetworkClass()...

	HUINT32			ulSize;
	HUINT32			ulDataSize;
	const HCHAR		*pszName;
	dbSvc_Table_t*	(* fnCtor) (dbSvc_Table_t *, void *);
	void			(* fnDtor) (dbSvc_Table_t *);
	HINT32			(* fnGetKey) (const dbSvc_Table_t *);
	void			(* fnSetKey) (dbSvc_Table_t *, HINT32);
	HBOOL			(* fnMatch) (const dbSvc_Table_t *, const dbSvc_TableFilter_t *);
	HINT32			(* fnToStr) (const dbSvc_Table_t *, FILE *pfStream);
} dbSvc_TableClass_t;

struct _dbSvc_Table
{
	const dbSvc_TableClass_t	*pstClazz;
	HINT32						 nRefCount;
	HUINT32						 ulFlags;
};

typedef struct _dbSvc_TableList dbSvc_TableList_t;		//p

typedef void	(* dbSvc_TableListenerCb_t) (dbSvc_TableList_t *, HINT32 nVariation, HINT32 nKey, void *pvUserData);

struct _dbSvc_TableList
{
	Handle_t				 hIdMgr;		///< ID Manager
	HxTREE_t				*pstTree;		///< table pstTree
	HxTREE_t				*pstTrash;		///< pstTrash pstTree
	HxHASH_t				*apstIndexMap[DB_SVC_TABLE_MAX_INDEX_MAP];

	dbSvc_TableListenerCb_t	 fnListener;	///< Table update fnListener
	void					*pvUserData;	///< pvUserData
};


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*	Table APIs
 */
dbSvc_Table_t*			db_svc_TableInit(const dbSvc_TableClass_t *pstClazz, dbSvc_Table_t *pstSelf, void *pvArg);
dbSvc_TableFilter_t*	db_svc_TableInitFilter(dbSvc_TableFilter_t *pstFilter);
dbSvc_TableFilter_t*	db_svc_TableAddFilterEx(dbSvc_TableFilter_t *pstFilter, HBOOL (* match)(const dbSvc_Table_t *, void *), void *pvUserData);	//p
dbSvc_Table_t*			db_svc_TableNew(const dbSvc_TableClass_t *pstClazz, void *pvArg);
void					db_svc_TableDelete(dbSvc_Table_t *pstSelf);
dbSvc_Table_t*			db_svc_TableClone(const dbSvc_Table_t *pstSelf);
dbSvc_Table_t*			db_svc_TableIncref(dbSvc_Table_t *pstSelf);
dbSvc_Table_t*			db_svc_TableDecref(dbSvc_Table_t *pstSelf);
HBOOL				 	db_svc_TableIsUpdated(dbSvc_Table_t *pstSelf, const void *pvData);
void*					db_svc_TableGetData(dbSvc_Table_t *pstSelf);
void*					db_svc_TableSetData(dbSvc_Table_t *pstSelf, void *pvData);
HINT32					db_svc_TableGetKey(const dbSvc_Table_t *pstSelf);	//p
void					db_svc_TableSetKey(dbSvc_Table_t *pstSelf, HINT32 nKey);
HBOOL					db_svc_TableHasFlag(const dbSvc_Table_t *pstTable, HUINT32 ulFlag);
dbSvc_Table_t*			db_svc_TableSetFlag(dbSvc_Table_t *pstTable, HUINT32 ulFlag, HBOOL bSet);	//p
HBOOL					db_svc_TableMatch(const dbSvc_Table_t *pstSelf, const dbSvc_TableFilter_t *pstFilter);
HINT32					db_svc_TableToString(const dbSvc_Table_t *pstSelf, FILE *pfStream);

/*	Table List APIs
 */
dbSvc_TableList_t*		db_svc_TableListNew(HUINT32 ulMaxId);	//p
void					db_svc_TableListDelete(dbSvc_TableList_t *pstSelf);
HERROR					db_svc_TableListPut(dbSvc_TableList_t *pstSelf, dbSvc_Table_t *pstData);
HERROR					db_svc_TableListAdd(dbSvc_TableList_t *pstSelf, dbSvc_Table_t *pstData);	//p
HERROR					db_svc_TableListRemove(dbSvc_TableList_t *pstSelf, HINT32 nKey);
HERROR					db_svc_TableListRemoveAll(dbSvc_TableList_t *pstSelf); //by Moon
HERROR					db_svc_TableListUpdate(dbSvc_TableList_t *pstSelf, HINT32 nKey, void *pvData);	//p
dbSvc_Table_t*			db_svc_TableListFind(dbSvc_TableList_t *pstSelf, HINT32 nKey);
void					db_svc_TableListClear(dbSvc_TableList_t *pstSelf);	//p
HINT32					db_svc_TableListCount(dbSvc_TableList_t *pstSelf);	//p
HBOOL					db_svc_TableListHasChanged(dbSvc_TableList_t *pstSelf);
HxVector_t*				db_svc_TableListGetChanged(dbSvc_TableList_t *pstSelf);
HxVector_t*				db_svc_TableListGetRemoved(dbSvc_TableList_t *pstSelf);
void					db_svc_TableListCommit(dbSvc_TableList_t *pstSelf);
HxVector_t*				db_svc_TableListDoFiltering(dbSvc_TableList_t *pstSelf, const dbSvc_TableFilter_t *pstFilter);	// p
HUINT32					db_svc_TableListRemoveIf(dbSvc_TableList_t *pstSelf, const dbSvc_TableFilter_t *pstFilter);
void					db_svc_TableListSetListener(dbSvc_TableList_t *pstSelf, dbSvc_TableListenerCb_t fnListener, void *pvUserData);
HxVector_t*				db_svc_TableListToVector(dbSvc_TableList_t *pstSelf);
HINT32					db_svc_TableListDump(dbSvc_TableList_t *pstSelf, FILE *pfStream);

#endif //__DB_SM_TABLE_H__
