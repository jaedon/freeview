/**
	@file     svc_swup.c
	@brief    svc_swup.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>
#include <octo_common.h>

#include <napi_netconf.h>
#include <svc_swup.h>

#include "_svc_swup.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC PostMsgToMgrCb_t		s_pfnSvcSwupPostMsgToMgr = NULL;
STATIC PostDataToMgrCb_t	s_pfnSvcSwupPostDataToMgr = NULL;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HINT32 svc_swup_UtilPostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_pfnSvcSwupPostMsgToMgr)
	{
		return (*s_pfnSvcSwupPostMsgToMgr)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return ERR_OK;
}

HINT32 svc_swup_UtilPostDataToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize)
{
	if (s_pfnSvcSwupPostDataToMgr)
	{
		return (*s_pfnSvcSwupPostDataToMgr)(nMsg, hAction, nParam1, nParam2, pvData, ulSize);
	}

	return ERR_OK;
}

HERROR		SVC_SWUPDATE_Init(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData)
{
	s_pfnSvcSwupPostMsgToMgr = fnPostMsg;
	s_pfnSvcSwupPostDataToMgr = fnPostData;

	svc_swup_IpRegisterCommand();

	return ERR_OK;
}

