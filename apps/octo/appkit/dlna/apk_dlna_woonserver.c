/*******************************************************************
    File Description
********************************************************************/
/**
    @file     apk_dlna_woonserver.c
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
#include	<homapi.h>
#include	<homapi_woonserver.h>


#if defined(CONFIG_OP_JAPAN)
#define WOONSERVER_DEFAULT_FRIENDLYNAME	"WA-7000"
#else
#define WOONSERVER_DEFAULT_FRIENDLYNAME	"HMS-1000SPH2"
#endif

#define ________________Static_Variables___________________________
static HxList_t	*s_pst_woons_cblist;

#define ________________Internal_Functions_________________________
STATIC void	apk_woonserver_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];
	if (pvData)
		APK_Free(pvData);
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR					hErr = ERR_FAIL;
	void					*pResultData = NULL;
	HOMxWoonServer_Event_e 	cmd = p1;

	APKE_WOONSERVER_Property_e eType = eWOONSERVER_UnknownType;

	switch ( cmd )
	{
		case eHOMAPI_WoonServer_Event_Start:
			{
				HOMxWoonServer_Event_Started_t *evt = (HOMxWoonServer_Event_Started_t *)p2;
				APKS_WOONSERVER_Started_t *result = APK_Calloc(sizeof(APKS_WOONSERVER_Started_t));

				if(result != NULL)
				{
					eType = eWOONSERVER_StartCmd;

					result->isStarted = evt->isStarted;
					HxLOG_Debug("############# Woon Started(%d) ############## \n", result->isStarted);

					pResultData = result;
				}
			}
			break;
		case eHOMAPI_WoonServer_Event_Stop:
			{
				HOMxWoonServer_Event_Stopped_t *evt = (HOMxWoonServer_Event_Stopped_t *)p2;
				APKS_WOONSERVER_Stopped_t *result = APK_Calloc(sizeof(APKS_WOONSERVER_Stopped_t));

				if(result != NULL)
				{
					eType = eWOONSERVER_StopCmd;

					result->isStopped = evt->isStopped;
					HxLOG_Debug("############# Woon Stoped(%d) ############## \n", result->isStopped);

					pResultData = result;
				}
			}
			break;
		case eHOMAPI_WoonServer_Event_Renamed:
			{
				HOMxWoonServer_Event_Renamed_t *evt = (HOMxWoonServer_Event_Renamed_t *)p2;
				APKS_WOONSERVER_Renamed_t *result = APK_Calloc(sizeof(APKS_WOONSERVER_Renamed_t));

				if(result != NULL)
				{
					eType = eWOONSERVER_SetFriendlyName;

					result->isRenamed = evt->isRenamed;
					HxLOG_Debug("############# Woon Renamed(%d) ############## \n", result->isRenamed);

					pResultData = result;
				}
			}
			break;

		case eHOMAPI_WoonServer_Event_TestConnection:
			{
				HOMxWoonServer_Event_ServerInfo_t *evt = (HOMxWoonServer_Event_ServerInfo_t*)p2;
				APKS_WOONSERVER_Info_t *result = APK_Calloc( sizeof(APKS_WOONSERVER_Info_t));

				HxLOG_Debug("############# Woon TestConnection ############## \n");

				if(result != NULL)
				{
					eType = eWOONSERVER_TestConnection;

					result->nState = evt->nState;
					result->nPubPort = evt->nPubPort;
					result->nExtPort = evt->nExtPort;

					HxSTD_snprintf(result->szLocalIp, sizeof(result->szLocalIp), "%s", evt->szLocalIp);
					HxSTD_snprintf(result->szExtIp, sizeof(result->szExtIp), "%s", evt->szExtIp);
					HxSTD_snprintf(result->szPubIp, sizeof(result->szPubIp), "%s", evt->szPubIp);

					HxLOG_Debug("## state(%d) public port(%d) ext port(%d) \n", result->nState, result->nPubPort, result->nExtPort);
					HxLOG_Debug("## local(%s) ext(%s) pub(%s) \n", result->szLocalIp, result->szExtIp, result->szPubIp);

					pResultData = result;
				}
			}
			break;

		case eHOMAPI_WoonServer_Event_PinCode:
			{
				HOMxWoonServer_Event_PinCode_t *evt = (HOMxWoonServer_Event_PinCode_t *)p2;
				APKS_WOONSERVER_PinCode_t *result = APK_Calloc(sizeof(APKS_WOONSERVER_PinCode_t));

				HxLOG_Error("############# Woon Pin Code ############## \n");

				if(result != NULL)
				{
					eType = eWOONSERVER_NotifyPinCode;

					result->bOpen = evt->bOpen;
					HxSTD_snprintf(result->szPinCode, sizeof(result->szPinCode), "%s", evt->szPinCode);
					HxLOG_Error("## Open(%d) Pin(%s) \n", result->bOpen, result->szPinCode);

					pResultData = result;
				}
			}
			break;

		default:
			HxLOG_Error("[%s:%d] invalid woonserver event [%d] \n", __func__, __LINE__, cmd);
			return ERR_OK;
	}

	if ( s_pst_woons_cblist )
	{
		HxList_t *item;
		APKS_DLNA_CbHandler pfnObjectHandler;
		APK_EVENT_EMIT_ARGS	stApkEvtArgs;

		item = s_pst_woons_cblist;
		while ( item )
		{
			pfnObjectHandler = (APKS_DLNA_CbHandler)HLIB_LIST_Data(item);

			if ( pfnObjectHandler )
			{
				HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
				switch (eType)
				{
					case eWOONSERVER_StartCmd:
						stApkEvtArgs.pfnFreeFunc = apk_woonserver_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_WOONSERVER_Started_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eWOONSERVER_StopCmd:
						stApkEvtArgs.pfnFreeFunc = apk_woonserver_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_WOONSERVER_Stopped_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eWOONSERVER_SetFriendlyName:
						stApkEvtArgs.pfnFreeFunc = apk_woonserver_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_WOONSERVER_Renamed_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eWOONSERVER_TestConnection:
						stApkEvtArgs.pfnFreeFunc = apk_woonserver_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_WOONSERVER_Info_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eWOONSERVER_NotifyPinCode:
						stApkEvtArgs.pfnFreeFunc = apk_woonserver_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_WOONSERVER_PinCode_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					default:
						HxLOG_Error("[%s:%d] undefined eType [%d]\n", __FUNCTION__, __LINE__, eType);
						break;
				}
			}

			item = item->next;
		}
	}

	if ( pResultData )
		APK_Free( pResultData );

	return hErr;
}

#define ________________Public_Interfaces__________________________
void	APKI_WOONSERVER_Init( void )
{
	HOMAPI_WOONSERVER_RegisterNotifier(___event_cb);
}

void	APKI_WOONSERVER_DeInit( void )
{
	HOMAPI_WOONSERVER_UnRegisterNotifier();
	HOMAPI_WOONSERVER_DeInit();
}

void	APK_WOONSERVER_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t				*list = HLIB_LIST_Find(s_pst_woons_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_woons_cblist = HLIB_LIST_Append(s_pst_woons_cblist, (void *)pfnObjectHandler );
}

void	APK_WOONSERVER_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t			 *list = HLIB_LIST_Find( s_pst_woons_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_woons_cblist = HLIB_LIST_Remove( s_pst_woons_cblist, pfnObjectHandler );
}

HERROR	APK_WOONSERVER_Start( HCHAR *pFriendlyName )
{
	return HOMAPI_WOONSERVER_Start(pFriendlyName);
}

HERROR	APK_WOONSERVER_Stop( void )
{
	return HOMAPI_WOONSERVER_Stop();
}

HERROR	APK_WOONSERVER_GetDefaultFriendlyName( HCHAR* pFriendlyName, HINT32 nLen )
{
	HxSTD_snprintf( pFriendlyName, nLen, "%s", WOONSERVER_DEFAULT_FRIENDLYNAME );
	/* Dont save to DB; This will be decided by BPL */
	return ERR_OK;
}

HERROR	APK_WOONSERVER_SetFriendlyName( const HCHAR *pFriendlyName )
{
	return HOMAPI_WOONSERVER_SetFriendlyName(pFriendlyName);
}

HERROR	APK_WOONSERVER_TestConnection(void )
{
	return HOMAPI_WOONSERVER_TestConnection();
}
