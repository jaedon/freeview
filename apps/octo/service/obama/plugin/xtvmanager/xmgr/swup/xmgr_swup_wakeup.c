/**
	@file     	xmgr_sw_update_wakeup.c
	@brief    	UI for the cold boot processes of Nothing
	It does nothing, but destroys itself and finish.

	Description:  			\n
	Module: 		app/ui/surfer			 					\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_param.h>
#include <svc_pipe.h>
#include <svc_si.h>

#include <bus.h>
#include <mgr_action.h>
#include <mgr_common.h>
#include <mgr_swup.h>

#include <mgr_appmgr.h>

#include "local_include/_xmgr_swup_wakeup.h"


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


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#if 0
// OTA Action에서 호출하는 함수...
STATIC HERROR xmgr_swup_WakeupOnCreateHandler(HUINT32 ulActionId, DxSwUpdate_Spec_e eSwUpdateSpec)
{
	HERROR hErr;

	hErr = ERR_FAIL;

	switch (eSwUpdateSpec)
	{
	default:
		break;
	}

	return hErr;
}

STATIC HERROR xmgr_swup_WakeupOnSuccessHandler(HUINT32 ulActionId, DxSwUpdate_Spec_e eSwUpdateSpec)
{
	HERROR hErr;

	hErr = ERR_FAIL;

	switch (eSwUpdateSpec)
	{
	default:
		break;
	}

	return hErr;
}

// eMEVT_BUS_CREATE
STATIC BUS_Result_t	xmgr_swup_WakeupOnCreate (HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					hError = ERR_FAIL;
	DxSwUpdate_Spec_e		eSwUpdateSpec = eDxSwUpdate_SPEC_NONE;

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	hError = MGR_SWUP_GetSpec (&eSwUpdateSpec);
	if (hError != ERR_OK)
	{
		eSwUpdateSpec = eDxSwUpdate_SPEC_NONE;
	}

	xmgr_swup_WakeupOnCreateHandler(ulActionId, eSwUpdateSpec);

	return	MESSAGE_BREAK;
}


// eMEVT_BUS_DESTROY
STATIC BUS_Result_t	xmgr_swup_WakeupOnDestroy (HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return	MESSAGE_BREAK;
}

#define _________________GLOBAL_FUNCTION__________________

STATIC BUS_Result_t xproc_swup_Wakeup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eRes = ERR_BUS_INITIAL_FAILED;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_swup_Wakeup");
		eRes = xmgr_swup_WakeupOnCreate (ulActionId, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		eRes = xmgr_swup_WakeupOnDestroy (ulActionId, p1, p2, p3);
		break;

	default:
		eRes = ERR_BUS_NO_ERROR;
		break;
	}

	if (eRes == MESSAGE_BREAK)
	{
		return MESSAGE_BREAK;
	}

	return	BUS_ProcessMessageDefault (message, hAction, p1, p2, p3);
}

HERROR xmgr_swup_WakeupStart(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("xproc_swup_Wakeup", APP_DEFAULT_PRIORITY, xproc_swup_Wakeup, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR xmgr_swup_WakeupStop(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Destroy(xproc_swup_Wakeup);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}
#endif

/*********************** End of File ******************************/
