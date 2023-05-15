/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_rsv.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_RSV_H___
#define	___PMGR_RSV_H___

#if 0
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/
HERROR	pmgr_rsv_PrintInstance(void);
HERROR	pmgr_rsv_Start(void);
HERROR	pmgr_rsv_EventChaseGetNumOfChaseList(HUINT32 *pulNumOfList);
HBOOL	pmgr_rsv_CheckConflictTypeForReservation(MgrAction_ConflictType_e eType);
HERROR	pmgr_rsv_GetRecExtInfoFromSupplementarySvcInfo(RsvRecord_t *pstRec, DbSvc_Info_t *pstSupplementarySvcInfo);
#endif
#endif /* !___PMGR_RSV_H___ */

