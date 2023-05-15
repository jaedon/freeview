/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _db_svc_service.h
	@brief

	Description:  \n
	Module: DB/ \n
	Remarks :										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef __DB_SM_SERVICE_H__
#define __DB_SM_SERVICE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <_db_svc_table.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	dbSvc_Table_t			stTable;
	DbSvc_Info_t			stData;
} dbSvc_Service_t;

typedef struct
{
	dbSvc_TableFilter_t	stFilter;
	dbSvc_TableFlags_e	bwSvcFlag;

	DxSvcType_e			eSvcType;
	DxVideoType_e		eVideoType;
	SVC_SatType_t		eSatType;
	DxCasType_e			eCasType;
	DxDeliveryType_e	eOrgDeliType;
	DxSvcSection_e		eSection;
	DxOperator_e		eSvcOpType;
	DbSvc_SortType_e	eSortType;
	DbSvc_VisibleType_e	eVisibleType;

	HINT32				nBouqIdx;
	HINT32				nGenreIdx;
	HINT32				nAntIdx;
	HINT32				nOnId;
	HINT32				nTsId;
	HINT32				nSvcId;
	HINT32				nTsIdx;
	HINT32				nPrvIdx;
	HINT32				nLcnFlag;
	HINT32				nStartLcn;
	HINT32				nEndLcn;
	HUINT8				ucAlphabetIdx;

	HBOOL				bNewSvc;
	DbSvc_UserFlag1_b	bwUserFlag1;
	DbSvc_UserFlag1_b	bwUserMask1;

	HINT32				eJpNetType;
} dbSvc_ServiceFilter_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

dbSvc_ServiceFilter_t *	db_svc_ServiceInitFilter (dbSvc_ServiceFilter_t *pstFilter);
dbSvc_ServiceFilter_t *	db_svc_ServiceConditionToFilter (dbSvc_ServiceFilter_t *pstFilter, const DbSvc_Condition_t *pstCond);
dbSvc_ServiceFilter_t *	db_svc_ServiceAttrToFilter (dbSvc_ServiceFilter_t *pstFilter, const DbSvc_Attr_t *pstAttr);
HxVector_t	*db_svc_ServiceDoFiltering (dbSvc_TableList_t *pstSelf, const dbSvc_ServiceFilter_t *pstFilter);


#endif //__DB_SM_SERVICE_H__
