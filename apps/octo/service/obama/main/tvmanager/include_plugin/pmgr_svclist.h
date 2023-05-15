/*******************************************************************
	File Description
********************************************************************/
/**
	@file     pmgr_svclist.h
	@brief

	Description:					\n
	Module :						\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_SVCLIST_H___
#define	___PMGR_SVCLIST_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_svc.h>

#include <mgr_svclist.h>

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

void		pmgr_svclist_InitSvcAttribute (DbSvc_Attr_t * pstAttr);

HERROR		pmgr_svclist_GetSvcDbParamsFromGroup (ChListUiGroup_t eSvcListGroup, DbSvc_Group_e *peSvcGroup, DbSvc_Attr_t *pstAttr);

HERROR		pmgr_svclist_CheckValidGroup (ChListUiGroup_t eSvcListroup);

DxSvcType_e	pmgr_svclist_GetSvcCountCheckType (void);


#endif /* !___PMGR_SVCLIST_H___ */

