/**************************************************************************************/
/**
 * @file oploperator.c
 *
 * Defines the query structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplplayready_freesat.h"
#include <hlib.h>
#include <appkit.h>
#include <appkit_freesat.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"playready"


#define	____OPL_PLAYREADY_PRIVATE_____________________________________________________________________

#define	____OPL_PLAYREADY_PUBLIC______________________________________________________________________
typedef void (*opl_fsat_pr_setlicenseurl_callback)(const char *value);

static opl_fsat_pr_setlicenseurl_callback pr_license_callback = NULL;

static void opl_fsat_pr_notifyFunc (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HUINT32							 ulSessionHandle;
	HxJSON_t						 jsParam = (HxJSON_t)ulParam1;
	APKE_MediaCtrl_Event_e			 eEvent;
	APKE_MediaCtrl_Destination_e	 eDest;
	HERROR							 hErr;
	char 						 licenseURL[2048] = {0,};

	hErr = APK_MEDIACTRL_GetNotifyArgs (jsParam, &eDest, &ulSessionHandle, &eEvent, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIACTRL_GetNotifyArgs Err\n");
		return;
	}

	switch (eEvent)
	{

	case eMctrlEvent_MEDIA_PR_NeedLicense:
		/* get url */
		APK_MEDIACTRL_PR_GetLicenseURL(eDest, licenseURL);
		HxLOG_Print("[%s:%d] %s\n", __FUNCTION__, __LINE__, licenseURL);

		(void)pr_license_callback((const char*)licenseURL);

		break;

	default:
		break;
	}
}


OPL_BOOL	OPL_FSAT_PR_SetNeedLicenceURL_RegisterCallback(void (*cbFunc)(const char *value))
{
	APKE_MediaCtrl_Destination_e	 eDest = APK_MEDIACTRL_GetMainViewDestination();
	APK_HANDLE						 hMediaPbHandle;

	HxLOG_Error("[%s:%d] %x\n", __FUNCTION__, __LINE__, cbFunc);

	pr_license_callback = cbFunc;

	// Media PB APPKIT
	hMediaPbHandle = APK_MEDIA_GetHandle (eMctrlBase_MediaPlay, eDest);

	APK_MEDIACTRL_RegisterNotifier (hMediaPbHandle, opl_fsat_pr_notifyFunc);


	APK_MEDIACTRL_PR_NeedCallback(eDest, TRUE);

	return OPL_TRUE;
}

OPL_BOOL	OPL_FSAT_PR_SetLicenceURL(const char *licenseURL)
{
	APKE_MediaCtrl_Destination_e	 eDest = APK_MEDIACTRL_GetMainViewDestination();

	(void)APK_MEDIACTRL_PR_SetLicenseURL(eDest, licenseURL);

	return OPL_TRUE;
}


