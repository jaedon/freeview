/*******************************************************************
    File Description
********************************************************************/
/**
    @file     apk_dlna_dial.c
    @brief

    Description:                                    \n
    Module: APPKITv2            \n

    Copyright (c) 2013 HUMAX Co., Ltd.              \n
    All rights reserved.                            \n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#define	________________Header_Files_______________________________
#include	"_apk_int.h"
#include	"apk_dlna.h"
#include	"apk_event.h"
#include    <homapi_dial.h>

#define ________________Static_Variables___________________________
static HxList_t	* s_pst_dial_cblist = NULL;

#define ________________Internal_Functions_________________________
STATIC void	apk_dlnadial_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	HCHAR	*pszApplName, *pszUrl;

	HxLOG_Assert(nArgc == 4);

	pszApplName = (HCHAR*)apArgV[2];
	pszUrl = (HCHAR*)apArgV[3];

	if (pszApplName)
		APK_Free(pszApplName);
	if (pszUrl)
		APK_Free(pszUrl);
}

static HCHAR * __safe_strdup(const HCHAR * data) {
	if (data) {
		return APK_StrDup(data);
	}
	return NULL;
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	APKE_DIAL_Property_e 	eType = eDLNA_DIAL_CmdEnd;
	HOMxDIAL_Event_e		cmd = p1;
	HCHAR					*pApp = NULL;
	HCHAR					*pData = NULL;

	switch ( cmd )
	{
		case eHOMAPI_DIAL_Event_AppLaunch:
			{
				HOMxDIAL_Event_AppLaunch_t * evt = (HOMxDIAL_Event_AppLaunch_t *)p2;
				HxLOG_Error("[%s:%d] APP LAUNCH :: %s - %s \n", __func__, __LINE__, evt->szApplicationName, evt->szUrl);
				pApp = evt->szApplicationName;
				pData = evt->szUrl;
				eType = eDLNA_DIAL_ReqAppLaunch;

				// Probably this message will be thrown to "$appl/browser/corsair/base-2.0/port/src/opl/IPService/opldial.c"
			}
			break;

		case eHOMAPI_DIAL_Event_AppStop:
			{
				HOMxDIAL_Event_AppLaunch_t * evt = (HOMxDIAL_Event_AppLaunch_t *)p2;
				HxLOG_Error("[%s:%d] APP STOP :: %s\n", __func__, __LINE__, evt->szApplicationName);
				pApp = evt->szApplicationName;
				eType = eDLNA_DIAL_ReqAppStop;
			}
			break;

		case eHOMAPI_DIAL_Event_AppStatus:
			{
				eType = eDLNA_DIAL_ReqAppStatus;
			}
			break;

		case eHOMAPI_DIAL_Event_Started:
			{
				eType = eDLNA_DIAL_Started;
			}
			break;

		case eHOMAPI_DIAL_Event_Stopped:
			{
				eType = eDLNA_DIAL_Stopped;
			}
			break;

		default:
			HxLOG_Warning("[%s:%d] can't handle : dial Property (%d) \n", __HxFILE__, __HxLINE__, eType);
			return ERR_FAIL;
	}

	if ( s_pst_dial_cblist )
	{
		HxList_t 		*item =  s_pst_dial_cblist;
		APKS_DLNA_CbHandler	pfnObjectHandler = NULL;

		while ( item )
		{
			pfnObjectHandler = (APKS_DLNA_CbHandler)HLIB_LIST_Data(item);
			if ( pfnObjectHandler )
			{
				APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

				stApkEvtArgs.pfnFreeFunc = apk_dlnadial_freeEventCb;
				stApkEvtArgs.ulArgCount = 4;
				stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
				stApkEvtArgs.apArgV[1] = (void *)eType;
				stApkEvtArgs.apArgV[2] = (void *)__safe_strdup(pApp);
				stApkEvtArgs.apArgV[3] = (void *)__safe_strdup(pData);

				APK_EVENT_Emit(&stApkEvtArgs);
			}

			item = item->next;
		}
	}

	return ERR_OK;
}

#define ________________Public_Interfaces__________________________
void	APK_DIAL_Init( void )
{
	HOMAPI_DIAL_RegisterNotifier(___event_cb);
}

void	APK_DIAL_DeInit( void )
{
	HOMAPI_DIAL_UnRegisterNotifier();
	HOMAPI_DIAL_DeInit();
}

void	APK_DIAL_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t		*list = HLIB_LIST_Find( s_pst_dial_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_dial_cblist = HLIB_LIST_Append( s_pst_dial_cblist, (void *)pfnObjectHandler );
}

void	APK_DIAL_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t		*list =  HLIB_LIST_Find( s_pst_dial_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_dial_cblist = HLIB_LIST_Remove( s_pst_dial_cblist, pfnObjectHandler );
}

HERROR	APK_DIAL_Start( HCHAR *pFriendlyName )
{
	return HOMAPI_DIAL_Start(pFriendlyName);
}

HERROR	APK_DIAL_Stop( void )
{
	return HOMAPI_DIAL_Stop();
}

HERROR	APK_DIAL_SetFriendlyName(const HCHAR *pFriendlyName )
{
	return HOMAPI_DIAL_SetFriendlyName(pFriendlyName);
}

HERROR	APK_DIAL_AppRegist(const HCHAR *app)
{
	return HOMAPI_DIAL_AppRegist(app);
}

HERROR	APK_DIAL_AppUnregist(const HCHAR *app)
{
	return HOMAPI_DIAL_AppUnregist(app);
}

HERROR	APK_DIAL_SetAppState(const HCHAR *app, HINT32 state)
{
	return HOMAPI_DIAL_SetAppStatus(app, state);
}

HERROR	APK_DIAL_SetCookie(const HCHAR * cookie) {
	return HOMAPI_DIAL_SetCookie(cookie);
}

HERROR	APK_DIAL_GetDialStatus(HBOOL * pDialActivated) {
	return HOMAPI_DIAL_GetDialStatus(pDialActivated);
}

HERROR	APK_DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size) {
	return HOMAPI_DIAL_GetFriendlyName(pFriendlyName, size);
}

HERROR	APK_DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * pAppName) {
	return HOMAPI_DIAL_GetAdditionalDataUrl(pAdditionalDataUrl, size, pAppName);
}
