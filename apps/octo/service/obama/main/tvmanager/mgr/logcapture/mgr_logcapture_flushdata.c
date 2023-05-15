/**
	@file     	mgr_logcapture_flushdata.c
	@brief    	logcapture action manager

	Description: 	\n
	Module: 		app/ap_core/action/logcapture			\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <isosvc.h>
#include <linkedlist.h>

#include <svc_fs.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Proto Type define     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC BUS_Result_t mgr_LogcaptureFlushData_MsgGwmCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	HLIB_CMD_CaptureLog("ShutDown");
	HLIB_CMD_SyncLog();

	NOT_USED_PARAM(hAction);

	BUS_MGR_Destroy (NULL);

	return MESSAGE_BREAK;
}
STATIC BUS_Result_t	mgr_LogcaptureFlushData_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);
	return MESSAGE_BREAK;
}

#define _____GLOBAL_FUNCTIONS_____

STATIC BUS_Result_t proc_Logcapture_FlushData (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Critical("[proc_Logcapture_FlushData]MESSAGE : eMEVT_BUS_CREATE\n");
			eGwmResult = mgr_LogcaptureFlushData_MsgGwmCreate(hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Critical("[proc_Logcapture_FlushData]MESSAGE : eMEVT_BUS_DESTROY\n");
			eGwmResult = mgr_LogcaptureFlushData_MsgGwmDestroy(p1, p2, p3);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Critical("[proc_Logcapture_FlushData]MESSAGE : eMEVT_BUS_GO_SHUTDOWN\n");
			return MESSAGE_PASS;
	}

	if(eGwmResult != ERR_BUS_NO_ERROR)
	{
		return eGwmResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

BUS_Callback_t	MGR_Logcapture_FlushDataGetProc (void)
{
	return proc_Logcapture_FlushData;
}

HERROR			MGR_Logcapture_FlushDataStart (void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("proc_Logcapture_FlushData", APP_ACTION_PRIORITY,
								proc_Logcapture_FlushData,
								HANDLE_NULL,
								0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR			MGR_Logcapture_FlushDataStop (void)
{
	BUS_Result_t	eRes = ERR_BUS_NO_OBJECT;

	if (BUS_MGR_Get(proc_Logcapture_FlushData) != NULL)
	{
		eRes = BUS_MGR_Destroy (proc_Logcapture_FlushData);
	}

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

