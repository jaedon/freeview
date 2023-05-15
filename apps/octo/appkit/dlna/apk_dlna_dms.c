/*******************************************************************
    File Description
********************************************************************/
/**
    @file     apk_dlna_dms.c
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
#include	"apk_sysinfo_stb.h"
#include	<homapi.h>
#include	<homapi_dms.h>

#if defined(CONFIG_OP_JAPAN)
#define DMS_DEFAULT_FRIENDLYNAME	"WA-7000"
#else
#define DMS_DEFAULT_FRIENDLYNAME	"HMS-1000SPH2"
#endif

#define ________________Typedef____________________________________
static HxList_t	*s_pst_dms_cblist;

#define ________________Internal_Functions_________________________
STATIC void	apk_dlnadms_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];

	if (pvData)
		APK_Free(pvData);
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	void			*pResultData = NULL;
	HOMxDMS_Event_e	 cmd = p1;

	APKE_DLNA_DMS_Property_e eType = eDLNA_DMS_UnknownType;

	switch(cmd)
	{
		case eHOMAPI_DMS_Event_DmsInfo:
			{
				HOMxDMS_Event_DmsInfo_t *evt = (HOMxDMS_Event_DmsInfo_t *)p2;
				APKS_DLNA_DMS_DmsInfo_t *ret = (APKS_DLNA_DMS_DmsInfo_t*)APK_Calloc( sizeof( APKS_DLNA_DMS_DmsInfo_t ) );

				HxSTD_snprintf(ret->szFriendlyName, sizeof(ret->szFriendlyName), "%s", evt->szFriendlyName);
				HxSTD_snprintf(ret->szUDN, sizeof(ret->szUDN), "%s", evt->szUDN);
				HxSTD_snprintf(ret->szModelName, sizeof(ret->szModelName), "%s", evt->szModelName);
				HxSTD_snprintf(ret->szModelDescription, sizeof(ret->szModelDescription), "%s", evt->szModelDescription);
				HxSTD_snprintf(ret->szModelURL, sizeof(ret->szModelURL), "%s", evt->szModelURL);
				HxSTD_snprintf(ret->szModelNumber, sizeof(ret->szModelNumber), "%s", evt->szModelNumber);

				ret->upnp_port = evt->upnp_port;
				ret->webserver_port = evt->webserver_port;
				ret->upnp_dtcp_port = evt->upnp_dtcp_port;
				ret->uploadserver_port = evt->uploadserver_port;

				pResultData = ret;
				eType = eDLNA_DMS_GetDmsInfo;
			}
			break;

		case eHOMAPI_DMS_Event_DmsRunningState:
			{
				HOMxDMS_Event_RunState_t *evt = (HOMxDMS_Event_RunState_t *)p2;
				APKS_DLNA_DMS_RunState_t *pState = ( APKS_DLNA_DMS_RunState_t* )APK_Calloc( sizeof( APKS_DLNA_DMS_RunState_t ));

				eType = eDLNA_DMS_NotiRunState;

				HxSTD_snprintf(pState->szRunState, sizeof(pState->szRunState), "%s", evt->szRunState);
				pState->nErrCode = evt->nErrCode;

				pResultData = pState;

				if( pResultData )
				{
					{
						/* for debugging */
						APKS_DLNA_DMS_RunState_t *pState = ( APKS_DLNA_DMS_RunState_t * )pResultData;
						HxLOG_Print( "[ APK_DMS ] =============================================\n" );
						HxLOG_Print( "[ APK_DMS ] DMS Notify Run State\n" );
						HxLOG_Print( "[ APK_DMS ] State : %s\n", pState->szRunState );
						HxLOG_Print( "[ APK_DMS ] err_code : %d\n", pState->nErrCode );
						HxLOG_Print( "[ APK_DMS ] =============================================\n" );
					}
				}
			}
			break;

		case eHOMAPI_DMS_Event_SetupStreaming :
		case eHOMAPI_DMS_Event_StartStreaming :
		case eHOMAPI_DMS_Event_StopStreaming :
		case eHOMAPI_DMS_Event_ReleaseStreaming :
		default :
			HxLOG_Error( "[ APK_DMS ] Error> Unsupported cmd(%d)!\n", cmd);
			break;
	}

	if ( s_pst_dms_cblist )
	{
		HxList_t *item;
		APKS_DLNA_CbHandler pfnObjectHandler;
		APK_EVENT_EMIT_ARGS	stApkEvtArgs;

		item = s_pst_dms_cblist;
		while ( item )
		{
			pfnObjectHandler = (APKS_DLNA_CbHandler)HLIB_LIST_Data(item);
			if ( pfnObjectHandler )
			{
				HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
				switch (eType)
				{
					case eDLNA_DMS_GetDmsInfo:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadms_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMS_DmsInfo_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMS_NotiRunState:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadms_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMS_RunState_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					default:
						HxLOG_Error("[%s:%d] not defined type [%d]\n", __FUNCTION__, __LINE__, eType);
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
void	APK_DLNADMS_Init( void )
{
	HOMAPI_DMS_RegisterNotifier(___event_cb);
}

void	APK_DLNADMS_DeInit( void )
{
	HOMAPI_DMS_UnRegisterNotifier();
	HOMAPI_DMS_DeInit();
}

void	APK_DLNADMS_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t	*list = HLIB_LIST_Find( s_pst_dms_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_dms_cblist = HLIB_LIST_Append( s_pst_dms_cblist, (void *)pfnObjectHandler );
}

void	APK_DLNADMS_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t	*list = HLIB_LIST_Find( s_pst_dms_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_dms_cblist = HLIB_LIST_Remove( s_pst_dms_cblist, pfnObjectHandler );
}

HERROR	APK_DLNADMS_Start( HCHAR *pFriendlyName, HBOOL bBootTimeStart )
{
#if defined(CONFIG_HOMMA_SATIP)
	HCHAR szDefaultFriendlyName[64] = {0, };
	HCHAR szFriendlyName[128] = {0, };

	APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 64);
	HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName), "%s SAT>IP Server", szDefaultFriendlyName);
	APK_SATIP_Start(szFriendlyName);
#endif

	return HOMAPI_DMS_Start(pFriendlyName, bBootTimeStart);
}

HERROR	APK_DLNADMS_Stop( void )
{
#if defined(CONFIG_HOMMA_SATIP)
	APK_SATIP_Stop();
#endif

	return HOMAPI_DMS_Stop();
}

HERROR	APK_DLNADMS_GetDefaultFriendlyName( HCHAR* pFriendlyName, HINT32 nLen )
{
	HxSTD_snprintf( pFriendlyName, nLen, "%s", DMS_DEFAULT_FRIENDLYNAME );
	/* Dont save to DB; This will be decided by BPL */
	return ERR_OK;
}

HERROR	APK_DLNADMS_SetFriendlyName( const HCHAR *pFriendlyName )
{
	return HOMAPI_DMS_SetFriendlyName(pFriendlyName);
}

HERROR	APK_DLNADMS_GetLiveStreamInfo( HINT32 hSvc, HUINT32 *pCount, APKS_DLNA_DMS_LiveStreamInfo_t **pResultData ) {
	if (sizeof(APKS_DLNA_DMS_LiveStreamInfo_t) != sizeof(HOMxDMS_LiveStreamInfo_t))
	{
		return ERR_FAIL;
	}
	return HOMAPI_DMS_GetLiveStreamInfo(hSvc, pCount, (HOMxDMS_LiveStreamInfo_t**) pResultData);
}

HERROR	APK_DLNADMS_FreeLiveStreamInfo(HUINT32 nCount, APKS_DLNA_DMS_LiveStreamInfo_t *pResultData ) {
	if (sizeof(APKS_DLNA_DMS_LiveStreamInfo_t) != sizeof(HOMxDMS_LiveStreamInfo_t))
	{
		return ERR_FAIL;
	}
	return HOMAPI_DMS_FreeLiveStreamInfo(nCount, (HOMxDMS_LiveStreamInfo_t*) pResultData);
}
