 /**
	@file     apk_swupdate.c
	@brief    s/w update 에 관련된 state 처리 인터페이스

	Description: Software Update Interface ( OPL - APPKIT - OAPI ) In Appkit\n
	Module: Appkit(Layer), Software Update (module) \n
	Remarks : \n

	Date : 2012.05.30			Modification: Add 			Name : mykim  \n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <oapi.h>
#include <apk.h>

typedef void (*pvApkSwupdateEventListener)(DxSwUpdate_EventData_t *psSwUpdateEvent);

/******************************************************************
	Static variables
******************************************************************/

STATIC void apk_swupdate_onNotifyoccured(OxSwUpdate_Notifier_t pfnNotiFunc, ApkEventFreeFunc_t pfnFreeFunc, DxSwUpdate_EventData_t *psSwUpdateEvent)
{
	if(pfnNotiFunc != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = pfnFreeFunc;
		stApkEvtArgs.ulArgCount = 2;
		stApkEvtArgs.apArgV[0] = (void *)pfnNotiFunc;
		stApkEvtArgs.apArgV[1] = (void *)psSwUpdateEvent;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}


#define __________PUBLIC_____FUNCTION__________________________________
/**************************************************************
	public-internal functions
**************************************************************/
void	 APK_SWUPDATE_Init(void)
{
	OAPI_SWUPDATE_Init(apk_swupdate_onNotifyoccured);
}

void	 APK_SWUPDATE_DeInit(void)
{
	OAPI_SWUPDATE_UnInit();
}

HERROR  APK_SWUPDATE_Create(DxSwUpdate_Spec_e eSwUpdateSpec, Handle_t *phSwupdate, void *pUserData)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_CreateHandle(eSwUpdateSpec, phSwupdate, FALSE, pUserData);

	return hError;
}

HERROR  APK_SWUPDATE_CreateManualConnect(DxSwUpdate_Spec_e eSwUpdateSpec, Handle_t *phSwupdate, DxTransponder_t *pTunningInfo)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_CreateHandle(eSwUpdateSpec, phSwupdate, TRUE, (void*)pTunningInfo);

	return hError;
}

HERROR 	APK_SWUPDATE_SwupdateInit(APK_SWUPDATE_Notifier_t fnNotifier)
{
	HERROR hError;

	hError = OAPI_SWUPDATE_SwupdateInit(fnNotifier);

	return hError;
}

HERROR	APK_SWUPDATE_StartDetect(APK_SWUPDATE_Notifier_t fnNotifier, Handle_t hSwUpdate)
{
	HERROR hError;

	hError = OAPI_SWUPDATE_DetectStart(hSwUpdate, (OxSwUpdate_Notifier_t)fnNotifier);

	return hError;
}

HERROR	APK_SWUPDATE_StopDetect(Handle_t hSwUpdate)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_DetectStop(hSwUpdate);

	return hError;
}

HERROR	APK_SWUPDATE_StartDownload(APK_SWUPDATE_Notifier_t fnNotifier, Handle_t hSwUpdate)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_DownloadStart(hSwUpdate, (OxSwUpdate_Notifier_t)fnNotifier);

	return hError;
}

HERROR	APK_SWUPDATE_StopDownload(Handle_t hSwUpdate)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_DownloadStop(hSwUpdate);

	return hError;
}

HERROR	APK_SWUPDATE_StartInstall(APK_SWUPDATE_Notifier_t fnNotifier, Handle_t hSwUpdate)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_InstallStart(hSwUpdate, (OxSwUpdate_Notifier_t)fnNotifier);

	return hError;
}

HERROR	APK_SWUPDATE_StopInstall(Handle_t hSwUpdate)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_InstallStop(hSwUpdate);

	return hError;
}

HERROR	APK_SWUPDATE_Cancel(Handle_t hSwUpdate)
{
	HERROR	hError;

	hError = OAPI_SWUPDATE_Cancel(hSwUpdate);

	return hError;
}

