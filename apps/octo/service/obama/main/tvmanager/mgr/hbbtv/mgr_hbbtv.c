/**
	@file     mgr_dynamicupdate.c
	@brief    file_name & simple comment.

	Description:  \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>

#include <svc_pipe.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_hbbtv.h>
#include <svc_si.h>

#include "local_include/_mgr_hbbtv.h"

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


#define	___INTERNAL_FUNCTIONS___
/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
STATIC BUS_Result_t mgr_hbbtv_MsgMevtActionSetupSucceeded (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId = SVC_PIPE_GetActionId(hAction);

	if ((MGR_ACTION_GetMainActionId() == ulActionId) && (eViewType_WTV == p1))
	{
		mgr_hbbtv_StartAction();
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtv_MsgMevtHbbtvStopMonitoring (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	mgr_hbbtv_StopAction();

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_hbbtv_proc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		 eRet = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("MESSAGE : eMEVT_BUS_CREATE, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HUINT8*)__FUNCTION__);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Debug("MESSAGE : eMEVT_BUS_DESTROY, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		break;

	case eMEVT_ACTION_SETUP_SUCCEEDED:
		HxLOG_Debug("MESSAGE : eMEVT_ACTION_SETUP_SUCCEEDED, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtv_MsgMevtActionSetupSucceeded(handle, p1, p2, p3);
		break;

	case eMEVT_HBBTV_STOP_MONITORING:
		HxLOG_Debug("MESSAGE : eMEVT_HBBTV_STOP_MONITORING, handle(0x%08x), p1(%d), p2(%d), p3(%d)\n", handle, p1, p2, p3);
		eRet = mgr_hbbtv_MsgMevtHbbtvStopMonitoring(handle, p1, p2, p3);
		break;

	default:
		break;
	}

	return eRet;
}

#define	___MODULE_FUNCTIONS___
/*******************************************************************/
/********************      	Module Function		 	********************/
/*******************************************************************/

#define	___API_FUNCTIONS___
/*******************************************************************/
/********************      	API Function    ********************/
/*******************************************************************/
HERROR MGR_HBBTV_Init (void)
{
	HERROR		 hErr = ERR_OK;

	if (BUS_MGR_Get(mgr_hbbtv_proc) == NULL)
	{
		// Before the development is completed, it should be blocked.
		hErr = BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, mgr_hbbtv_proc, HANDLE_NULL, 0, 0, 0);
	}

	return hErr;
}


