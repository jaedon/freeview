 /**
	@file     apk_dsmcc.c
	@brief    dsmcc 에 관련된 state 처리 인터페이스

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
#include <dlib.h>
#include <oapi.h>
#include <apk.h>

/******************************************************************
	Static variables
******************************************************************/
STATIC void apk_dsmcc_onNotifyoccured(OAPI_DSMCC_Notifiler_t pfnNotiFunc, ApkEventFreeFunc_t pfnFreeFunc, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if(pfnNotiFunc != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = pfnFreeFunc;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)pfnNotiFunc;
		stApkEvtArgs.apArgV[1] = (void *)ulParam1;
		stApkEvtArgs.apArgV[2] = (void *)ulParam2;
		stApkEvtArgs.apArgV[3] = (void *)ulParam3;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#define __________PUBLIC_____FUNCTION__________________________________
/**************************************************************
	public-internal functions
**************************************************************/

void	APK_DSMCC_Init(void)
{
	HxLOG_Debug("[%s][%03d] ===== (+) \n", __FUNCTION__, __LINE__);

	OAPI_DSMCC_Init(apk_dsmcc_onNotifyoccured);
}

void	APK_DSMCC_DeInit(void)
{
	HxLOG_Debug("[%s][%03d] ===== (+) \n", __FUNCTION__, __LINE__);
	OAPI_DSMCC_DeInit();
}

HERROR	APK_DSMCC_Create(DxDSMCC_Type_e eType, Handle_t *phDsmcc, HCHAR **ppszCachePath)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (eType : %d) \n", __FUNCTION__, __LINE__, eType);

	hError = 	OAPI_DSMCC_Create(eType, phDsmcc, ppszCachePath);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d , phDsmcc : 0x%x) \n", __FUNCTION__, __LINE__, hError, *phDsmcc);

	return hError;
}

HERROR		APK_DSMCC_Destroy(Handle_t hDsmcc)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (hDsmcc : 0x%08x) \n", __FUNCTION__, __LINE__, hDsmcc);

	hError = OAPI_DSMCC_Destroy(hDsmcc);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

void	APK_DSMCC_FreePath(HCHAR *ppszCachePath)
{
	HxLOG_Debug("[%s][%03d] ===== (+) (ppszCachePath : %s)\n", __FUNCTION__, __LINE__, ppszCachePath);

	OAPI_DSMCC_FreePath(ppszCachePath);
}

void	APK_DSMCC_RegisterNotifier(Handle_t hDsmcc, APK_DSMCC_Notifiler_t fnNotifier)
{
	HxLOG_Debug("[%s][%03d] ===== (+) ( hDsmcc :0x%08x | fnNotifier : %p) \n", __FUNCTION__, __LINE__, hDsmcc, fnNotifier);

	OAPI_DSMCC_RegisterNotifier( hDsmcc,  (OAPI_DSMCC_Notifiler_t)fnNotifier);
}

void	APK_DSMCC_UnRegisterNotifier(Handle_t hDsmcc, APK_DSMCC_Notifiler_t fnNotifier)
{
	HxLOG_Debug("[%s][%03d] ===== (+) ( hDsmcc :0x%08x | pfnObjectHandler : %p) \n", __FUNCTION__, __LINE__, hDsmcc, fnNotifier);

	OAPI_DSMCC_UnRegisterNotifier(hDsmcc,  (OAPI_DSMCC_Notifiler_t)fnNotifier);
}

HERROR	APK_DSMCC_GetCarouselList(Handle_t hDsmcc, DxDSMCC_CarouselList_t *pstCarouselList)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) ( hDsmcc :0x%08x | pstCarouselList : %p)\n", __FUNCTION__, __LINE__, hDsmcc, pstCarouselList);

	hError = OAPI_DSMCC_GetCarouselList(hDsmcc,  pstCarouselList);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

void	APK_DSMCC_FreeCarouselInfo(DxDSMCC_CarouselInfo_t *pCarouselInfo)
{
	HxLOG_Debug("[%s][%03d] ===== (+) ( pCarouselInfo : %p)\n", __FUNCTION__, __LINE__, pCarouselInfo);

	OAPI_DSMCC_FreeCarouselInfo(pCarouselInfo);
}

HERROR	APK_DSMCC_Start(Handle_t hDsmcc, Handle_t hCarousel, const HCHAR *pszCachePath)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (hDsmcc :0x%08x | hCarousel : 0x%08x| pszCachePath : %s) \n", __FUNCTION__, __LINE__, hDsmcc, hCarousel, pszCachePath);

	hError = OAPI_DSMCC_Start(hDsmcc,  hCarousel, pszCachePath);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

HERROR	APK_DSMCC_Restart(Handle_t hDsmcc, Handle_t hCarousel)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (hDsmcc :0x%08x | hCarousel : 0x%08x) \n", __FUNCTION__, __LINE__, hDsmcc, hCarousel);

	hError = OAPI_DSMCC_ReStart(hDsmcc,  hCarousel);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

HERROR	APK_DSMCC_Stop(Handle_t hDsmcc, Handle_t hCarousel)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (hDsmcc :0x%08x | hCarousel : 0x%08x) \n", __FUNCTION__, __LINE__, hDsmcc, hCarousel);

	hError = OAPI_DSMCC_Stop(hDsmcc,  hCarousel);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

HERROR	APK_DSMCC_RequestEvent(Handle_t hDsmcc, HBOOL bAll)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (hDsmcc :0x%08x | bAll :%d) \n", __FUNCTION__, __LINE__, hDsmcc, bAll);

	hError = OAPI_DSMCC_RequestEvent(hDsmcc,  bAll);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

HERROR	APK_DSMCC_Open(Handle_t hDsmcc, Handle_t hSession)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (hDsmcc :0x%08x | hSession : 0x%08x) \n", __FUNCTION__, __LINE__, hDsmcc, hSession);

	hError = OAPI_DSMCC_Open(hDsmcc,  hSession);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

HERROR	APK_DSMCC_Close(Handle_t hDsmcc, Handle_t hSession)
{
	HERROR	hError;

	HxLOG_Debug("[%s][%03d] ===== (+) (hDsmcc :0x%08x | hSession : 0x%08x) \n", __FUNCTION__, __LINE__, hDsmcc, hSession);

	hError = OAPI_DSMCC_Close(hDsmcc,  hSession);

	HxLOG_Debug("[%s][%03d] ===== (-) (hError : %d) \n", __FUNCTION__,__LINE__, hError);

	return hError;
}

