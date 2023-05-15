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

	/* - Description : Option �޴���� DB���� PID�� subtitle index�� �������� ��� Master action���� DB ������ �ݿ��϶�� �뺸�ϴ� �޽���.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_SVCLIST_SVC_PID_CHANGED,

	/*	- Description : Radiobackground ���°� ����Ǿ����� �˸��� �޽���
		- Parameters Usage :
			handle : Action handle. Session version�� 0
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

