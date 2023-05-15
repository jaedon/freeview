/**
	@file     ap_ota_detect_action.c
	@brief   OTA download application \n

	Description: sw update detect \n
	Module: Application \n
	Remarks: \n
	 Date			Modification						Name \n
	-----------	-------------------------------------	------------------ \n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/


/******************************************************************************/
/******************************* Header Files *********************************/
/******************************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>


#include <mgr_swup.h>
#include "../local_include/_mgr_swup.h"

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/********************************** Defines ***********************************/
/******************************************************************************/



/******************************************************************************/
/********************************** Typedef ***********************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Static Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/************************ Static Functions Declaration ************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global Functions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Static Functions ******************************/
/******************************************************************************/
HERROR			MGR_SWUP_DetectCreate(HINT32 nTunerId, HUINT32 ulTag)
{
	HUINT32 			ulActionId;
	Handle_t				hAction;
	BUS_Mgr_t			mgr;
	BUS_Callback_t		pfnSwupDetectProc;
	HERROR			hErr;


	HxLOG_Trace();
	pfnSwupDetectProc	= mgr_swup_DetectGetAction(eDxSWUPDATE_SOURCE_Rf);
	HxLOG_Trace();

	mgr = BUS_MGR_Get(pfnSwupDetectProc);
	if (mgr)
	{
		HxLOG_Trace();
		BUS_MGR_Destroy(pfnSwupDetectProc);
	}
	HxLOG_Trace();

	hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("cannot get action id \n");
		MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0, eSWUP_FAILREASON_Action);
		return ERR_FAIL;
	}
	HxLOG_Trace();

	hAction = SVC_PIPE_GetActionHandle(ulActionId);
	HxLOG_Print("pfnSwupDetectProc[%x]", pfnSwupDetectProc);

	BUS_MGR_Create("SWUP_Detect_Proc", APP_DEFAULT_PRIORITY, pfnSwupDetectProc, hAction, nTunerId, (HINT32)ulTag, 0);
	HxLOG_Trace();

	return ERR_OK;
}


HERROR			MGR_SWUP_DetectDestroy(void)
{
	BUS_Mgr_t			mgr;
	BUS_Callback_t		pfnSwupDetectProc;


	pfnSwupDetectProc	= mgr_swup_DetectGetAction(eDxSWUPDATE_SOURCE_Rf);

	mgr = BUS_MGR_Get(pfnSwupDetectProc);
	if (mgr)
	{
		BUS_MGR_Destroy(pfnSwupDetectProc);
	}

	return ERR_OK;
}
/******************************** End of File *********************************/

