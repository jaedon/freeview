/**
	@file     ap_ota_dwnld.c
	@brief   OTA download application \n

	Description: OTA download \n
	Module: Application \n
	Remarks: \n
	 Date			Modification						Name \n
	-----------	-------------------------------------	------------------ \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
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

HERROR MGR_SWUP_DownloadCreate(Handle_t hAction, DxSwUpdate_Source_e eSource, HUINT32 ulSignalVer, HUINT16 usPid, Handle_t hSwUpdate, HBOOL bStartByMenu)
{
	MgrSwup_DownloadArg_t	stDownloadArgument;
	BUS_Callback_t		pfnOTADownloadProc;
	DxSwUpdate_Spec_e	eSwUpdateSpec;

	MGR_SWUP_GetSpec (&eSwUpdateSpec);

	pfnOTADownloadProc = mgr_swup_DownloadGetAction(eSource);
	if (NULL == pfnOTADownloadProc)
		return ERR_FAIL;

	if (BUS_MGR_Get(pfnOTADownloadProc) != NULL)
	{
		BUS_MGR_Destroy(pfnOTADownloadProc);
	}

	stDownloadArgument.eSwUpdateSpec = eSwUpdateSpec;
	stDownloadArgument.ulSignalVer = ulSignalVer;
	stDownloadArgument.usPid = usPid;
	stDownloadArgument.hSwUpdate = hSwUpdate;

	BUS_MGR_Create("MGR_SWUP_DownloadCreate", APP_DEFAULT_PRIORITY, pfnOTADownloadProc, (HINT32)hAction, (HINT32)&stDownloadArgument, (HINT32)bStartByMenu, 0);
	HxLOG_Info("\tcreated download application. (hAction: 0x%x, eSwUpdateSpec: %d, sig ver: 0x%x, pid: 0x%x)\n", hAction, eSwUpdateSpec, ulSignalVer, usPid);

	return ERR_OK;
}

HERROR MGR_SWUP_DownloadDestroy(DxSwUpdate_Source_e eSource)
{
	BUS_Callback_t		pfnOTADownloadProc;

	pfnOTADownloadProc = mgr_swup_DownloadGetAction(eSource);
	if (NULL == pfnOTADownloadProc)
		return ERR_FAIL;

	if (BUS_MGR_Get(pfnOTADownloadProc) != NULL)
	{
		BUS_MGR_Destroy(pfnOTADownloadProc);
	}

	return ERR_OK;
}

/******************************** End of File *********************************/
