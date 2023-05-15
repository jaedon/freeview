/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _db_svc_group.h
	@brief

	Description:  \n
	Module: DB/ \n
	Remarks :										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef __OxGROUP_H__
#define __OxGROUP_H__

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
	dbSvc_Table_t		stTable;
	DbSvc_GroupInfo_t	stData;
} dbSvc_Group_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

dbSvc_Group_t	*db_svc_GroupNew (const DbSvc_GroupInfo_t *data);
dbSvc_Group_t	*db_svc_GroupDelete (dbSvc_Group_t *svc);

#endif //__OxGROUP_H__