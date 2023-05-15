/**
	@file     mgr_svclist.c
	@brief    ��ü Application�� �������� ����� �� �ִ� core utility API

	Description: MW�� ����ϴ� ����� �� application���� �������� ����� �� �ִ� �͵��� API�� ����� utilityȭ �Ѵ�.
	Module: AP/����			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <linkedlist.h>
#include <db_svc.h>
#include <isosvc.h>
#include <osd_font.h>
#include <osd_gfx.h>
#include <svc_si.h>

#include <mgr_svclist.h>
#include <pmgr_svclist.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/**************** Static Function Prototypes  **********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HINT32 MGR_SVCLIST_CountSvcInDB (DxSvcType_e eSvcType)
{
	DbSvc_Attr_t	stAttr;
	HINT32		nSvcCnt;
	HERROR		hErr;

	MGR_SVCLIST_InitSvcAttribute (&stAttr);
	stAttr.eSvcType = eSvcType;
	hErr = DB_SVC_GetServiceCount(eSvcGroup_All, &stAttr, &nSvcCnt);
	if(hErr == ERR_OK)
	{
		return nSvcCnt;
	}
	else
	{
		return 0;
	}
}

void MGR_SVCLIST_InitSvcAttribute(DbSvc_Attr_t * pstAttr)
{
	pmgr_svclist_InitSvcAttribute(pstAttr);
}

HERROR MGR_SVCLIST_GetSvcDbParamsFromGroup(ChListUiGroup_t eSvcListGroup, DbSvc_Group_e *peSvcGroup, DbSvc_Attr_t *pstAttr)
{
	return pmgr_svclist_GetSvcDbParamsFromGroup (eSvcListGroup, peSvcGroup, pstAttr);
}

HERROR MGR_SVCLIST_CheckValidSvcListGroup (ChListUiGroup_t eSvcListGroup)
{
	return pmgr_svclist_CheckValidGroup (eSvcListGroup);
}

DxSvcType_e MGR_SVCLIST_GetSvcTypeForListCount (void)
{
	return pmgr_svclist_GetSvcCountCheckType();
}

/*********************** End of File ******************************/

