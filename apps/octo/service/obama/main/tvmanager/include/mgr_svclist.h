/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_svclist.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_SVCLIST_H___
#define	___MGR_SVCLIST_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_SVCLIST			= eMEVT_SVCLIST_START,

	/* - Description : Option 메뉴등에서 DB상의 PID나 subtitle index를 변경했을 경우 Master action에게 DB 변경을 반영하라고 통보하는 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_SVCLIST_SVC_PID_CHANGED,

	/*	- Description : Radiobackground 상태가 변경되었음을 알리는 메시지
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : bRadioBackground
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_SVCLIST_RADIOBG_CHANGED,

};

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HINT32		MGR_SVCLIST_CountSvcInDB(DxSvcType_e eSvcType);

void		MGR_SVCLIST_InitSvcAttribute(DbSvc_Attr_t * pstAttr);

HERROR		MGR_SVCLIST_GetSvcDbParamsFromGroup(ChListUiGroup_t eSvcListGroup, DbSvc_Group_e *peSvcGroup, DbSvc_Attr_t *pstAttr);
HERROR		MGR_SVCLIST_CheckValidSvcListGroup (ChListUiGroup_t eSvcListGroup);

DxSvcType_e	MGR_SVCLIST_GetSvcTypeForListCount (void);


#endif /* !___MGR_SVCLIST_H___ */

