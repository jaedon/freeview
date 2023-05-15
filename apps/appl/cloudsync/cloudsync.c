/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax - Europe/CIS team, hcyoo@humaxdigital.com
**************************************************************/
/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <signal.h>
#include <hlib.h>
#include <vkernel.h>
#include <apk.h>
#include "profile.h"
#include "appverify.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
static void _cloudSync_MainThread( void *arg1 );

#define ZIGGO_ALPHA_BASE_DATA_URL			"https://wsl-ot.ziggo.nl/base_data"

enum
{
	LOOK_SERVER_URL = 0,
	HEARTBEAT_SERVER_URL,
	OAUTH_GET_REQUEST_TOKEN_URL,
	OAUTH_GET_ACCESS_TOKEN_URL,
	OAUTH_LOGOUT_URL,
	OAUTH_LOGIN_URL,
	OAUTH_GET_VERIFIER_URL,
	OAUTH_SUN_QUERY_PARAM_STRING,
	LOGIN_URL,
	APP_VERIFIER_TOKEN_URL,
	LOOK_HEARTBEAT_INTERVAL,
	CHANNEL_STILL_REFRESH_INTERVAL,
	LOOK_ENVIRONMENT,
	TEXT_HELP_XBOX,
	MAX
};

static const HCHAR* s_sbaseDataString [] =
{
	"LookServerURL",
	"HeartBeatServerURL",
	"oAuthGetRequestTokenURL",
	"oAuthGetAccessTokenURL",
	"oAuthLogoutURL",
	"oAuthLoginURL",
	"oAuthGetVerifierURL",
	"oAuthSunQueryParamsString",
	"LoginURL",
	"AppVerifierTokenURL",
	"LookHeartbeatInterval",
	"ChannelStillRefreshInterval",
	"LookEnvironment",
	"TextHelpXbox",
};

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/**@brief Cloud Sync Content structure. */
typedef struct tagCLOUDSYNC_CONTENT
{
	APKE_NET_STATUS_e 	eNetStatus;				/**< Network Status. */
	HBOOL				bConnectedZiggoServer;	/**< BOOL Degraded Mode. */
	HLONG				ulMainThreadId;			/**< Main Thread ID. */
	HCHAR				*pToken;
} CS_Content_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static CS_Content_t		s_stCSContent;	/**< CS Main Content Structure. */

#if defined(CONFIG_DEBUG)
static HUINT8 * _cloudSync_dbg_GetNetworkNotifyString( HINT32 eEvent )
{
	switch (eEvent)
	{
		ENUM_TO_STR(eNETWORK_STATUS_CON_SUCCESS);
		ENUM_TO_STR(eNETWORK_STATUS_CON_FAIL);
		ENUM_TO_STR(eNETWORK_STATUS_CON_AUTOIP_SUCCESS);
		ENUM_TO_STR(eNETWORK_STATUS_CON_AUTOIP_FAIL);
		ENUM_TO_STR(eNETWORK_STATUS_CABLE_LINK_CONNECTED);
		ENUM_TO_STR(eNETWORK_STATUS_CABLE_LINK_DISCONNECTED);

		default :
			break;
	}

	return (HUINT8*)NULL;
}
#endif

static HBOOL _cloudSync_IsNetworkActive( void )
{
	HERROR		hErr;
	HUINT32		index;
	APKS_NETDEVINFO_t	apkNetDevInfo;

	hErr = APK_NETWORK_CONF_GetDefaultDev( &index );
	if ( hErr != ERR_OK )
	{
		HxLOG_Error( "[%s] GetDefaultDev fail = %d\n", __FUNCTION__, hErr );
		return FALSE;
	}

	hErr = APK_NETWORK_CONF_GetDevInfo( index, &apkNetDevInfo );
	if ( hErr != ERR_OK )
	{
		HxLOG_Error( "[%s] GetDevInfo fail = %d\n", __FUNCTION__, hErr );
		return FALSE;
	}

	HxLOG_Debug( "[%s] Max = %s\n", __FUNCTION__, apkNetDevInfo.szMacAddr );
	HxLOG_Debug( "[%s] szDev_name = %s\n", __FUNCTION__, apkNetDevInfo.szDev_name );

	return apkNetDevInfo.bNetActive;
}

static void _cloudSync_DegradedMode( void )
{

}

#if 0
static HERROR _cloudSync_ParseXmlData( HUINT32 ulSize, HINT8 *data )
{
	HERROR  	hErr;
	HxXMLDOC	xmlDoc = NULL;
	HxXMLELEMENT *phRootHandle = NULL;
	HxList_t	*pList = NULL;
	HxXMLELEMENT *element;
	HINT8	*contents = NULL;

	HxLOG_Trace();

	hErr = HLIB_XML_InitParser();
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug( "[%s] Init fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	hErr = HLIB_XML_ParseBuffer( data, ulSize, &xmlDoc );
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug( "[%s] Parse fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	hErr = HLIB_XML_GetRootElement( xmlDoc, phRootHandle );
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug( "[%s] Get Root Element fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	hErr = HLIB_XML_FindChildElementByName( phRootHandle, s_sbaseDataString[LOOK_SERVER_URL], &pList );
	element = (HxXMLELEMENT) HLIB_LIST_Data( pList );
	HLIB_XML_GetElementContents( phRootHandle, element, &contents );

	HxLOG_Debug( "[%s] Contents - %s\n", __FUNCTION__, contents );

	return ERR_OK;
}
#endif

static HERROR _cloudSync_ParseJanssonData( HCHAR *pJsonData )
{
	HxJSON_t	pJsonRoot = NULL;
	HCHAR		*p;

	HxLOG_Trace();

	pJsonRoot = HLIB_JSON_Decode( pJsonData );
	if( pJsonRoot != NULL )
	{
		HxJSON_t configuration;

		configuration = HxJSON_Object_Get( pJsonRoot, "Configuration");

		/* Get LOOK_SERVER_URL. */
		p = (HCHAR *)HLIB_JSON_Object_GetString( configuration, s_sbaseDataString[LOOK_SERVER_URL] );
		HxLOG_Debug( "[%s] LOOK_SERVER_URL : %s\n", __FUNCTION__, p );
		/* Save UDB. */

		/* Get HEARTBEAT_SERVER_URL. */
		p = (HCHAR *)HLIB_JSON_Object_GetString( configuration, s_sbaseDataString[HEARTBEAT_SERVER_URL] );
		HxLOG_Debug( "[%s] HEARTBEAT_SERVER_URL : %s\n", __FUNCTION__, p );
		/* Save UDB. */

		/* Get HEARTBEAT_SERVER_URL. */
		p = (HCHAR *)HLIB_JSON_Object_GetString( configuration, s_sbaseDataString[OAUTH_GET_REQUEST_TOKEN_URL] );
		HxLOG_Debug( "[%s] OAUTH_GET_REQUEST_TOKEN_URL : %s\n", __FUNCTION__, p );
		/* Save UDB. */

		// TODO:
	}
	else
	{
		HxLOG_Debug( "[%s] Json loading error.\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR _cloudSync_GetBaseDataFromZiggoServer( void )
{
	HERROR  hErr;
	HxTRANSPORT_Http_t*	hCurl;
	HINT8 *buf = NULL;
	HFLOAT64 size;
	HxTRANSPORT_HttpResponse_e eResponse;

	HxLOG_Trace();

	/* Open curl. */
	hCurl = HLIB_TRANSPORT_HttpOpen();
	if ( hCurl == NULL )
	{
		HxLOG_Error( "[%s] Open curl fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

 	/* HTTPS를 위해 설정,, 아직 정확한 이유는 모름. */
	hErr = HLIB_TRANSPORT_HttpSetPeerVeryfication( hCurl, FALSE );
	if ( hErr != ERR_OK )
	{
		HxLOG_Error( "[%s] SetPeerVeryfication fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	/* Get data. */
	hErr = HLIB_TRANSPORT_HttpGet( hCurl, ZIGGO_ALPHA_BASE_DATA_URL, &buf, &size );
	if ( hErr != ERR_OK )
	{
		HLIB_TRANSPORT_HttpFreeBuffer( buf );
		HxLOG_Error( "[%s] GetWithCallback fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	/* Get ResponseCode. */
	hErr = HLIB_TRANSPORT_HttpGetHttpResponseCode( hCurl, &eResponse );
	if( eResponse != eHxTP_HTTP_RESCODE_OK )
	{
		HxLOG_Error( "[%s] GetHttpResponseCode %d\n", __FUNCTION__, eResponse );

		HLIB_TRANSPORT_HttpFreeBuffer( hCurl );
		HLIB_TRANSPORT_HttpClose( hCurl );

		return ERR_FAIL;
	}

	/* Debug Print. */
	if ( buf != NULL )
	{
		HxLOG_Debug( "[%s]\nsize - %d\nstring - \n%s\n", __FUNCTION__, (HUINT32)HxSTD_StrLen( buf ), buf );
	}

	if ( HLIB_JSON_IsJSON( buf, (HUINT32)HxSTD_StrLen( buf ) ) == TRUE )
	{
		_cloudSync_ParseJanssonData( buf );
	}

	/* Close. */
	HLIB_TRANSPORT_HttpFreeBuffer( buf );
	HLIB_TRANSPORT_HttpClose( hCurl );

	return ERR_OK;
}

static void _cloudSync_ConnectStatusSuccess( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	HERROR  hErr;
	HBOOL 	bNetwork = FALSE;

	HxLOG_Trace();

	/* Check Network Activate. */
	bNetwork = _cloudSync_IsNetworkActive();
	if ( bNetwork != TRUE )
	{
		HxLOG_Error( "[%s] bNetwork = %d\n", __FUNCTION__, bNetwork );
		return;
	}

	if ( s_stCSContent.eNetStatus == eEvent )
	{
		HxLOG_Debug( "[%s] - It isn't changed. (%s:%s)\n", __FUNCTION__, _cloudSync_dbg_GetNetworkNotifyString( s_stCSContent.eNetStatus ), _cloudSync_dbg_GetNetworkNotifyString( eEvent ) );
		return;
	}
	else
	{
		HxLOG_Debug( "[%s] - It is changed. (%s:%s)\n", __FUNCTION__, _cloudSync_dbg_GetNetworkNotifyString( s_stCSContent.eNetStatus ), _cloudSync_dbg_GetNetworkNotifyString( eEvent ) );

		/* Set NetStatus. */
		s_stCSContent.eNetStatus = eEvent;
	}

	/* Get BaseData from Ziggo cloud Server. */
	hErr = _cloudSync_GetBaseDataFromZiggoServer();
	if ( hErr != ERR_OK )
	{
		/* Goto Degraded mode. */
	}

	hErr = AV_GetAppVerifyToken( &s_stCSContent.pToken );
	if ( hErr != ERR_OK )
	{
		HxLOG_Error( "[%s] Get Token fail\n", __FUNCTION__ );
	}
	else
	{
		HxLOG_Debug( "[%s] Token : %s\n", __FUNCTION__, s_stCSContent.pToken );
	}

	/* Set NetStatus -> Profile. */
	PF_SetNetworkStatus( eEvent );
}

static void _cloudSync_NetworkNotify( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	HxLOG_Debug( "[%s] : %s\n", __FUNCTION__, _cloudSync_dbg_GetNetworkNotifyString( eEvent ) );

	switch ( eEvent )
	{
		/* CONNECT_STATUS SUCCESS*/
		case eNETWORK_STATUS_CON_SUCCESS :
		case eNETWORK_STATUS_CON_AUTOIP_SUCCESS :
			_cloudSync_ConnectStatusSuccess( eEvent, ulParam1, ulParam2, ulParam3 );
			break;

		case eNETWORK_STATUS_CON_FAIL :
		case eNETWORK_STATUS_CON_AUTOIP_FAIL :
			break;

		/* CABLE LINK_STATUS */
		case eNETWORK_STATUS_CABLE_LINK_CONNECTED :
		case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED :
			break;

		// TODO: Add case
		default :
			HxLOG_Debug( "[%s] other[%d]\n", __FUNCTION__, eEvent );
			return;
	}
}

static void _cloudSync_ResetCSContent( void )
{
	if ( s_stCSContent.pToken != NULL )
	{
		HLIB_MEM_Free( s_stCSContent.pToken );
	}

	HxSTD_memset( &s_stCSContent, 0, sizeof( CS_Content_t ) );
	s_stCSContent.eNetStatus = eNETWORK_STATUS_UNKNOWN;
}

static void _cloudSync_InitializeAppkit( void )
{
	HxLOG_Trace();

	/* Init APK DB. */
	APK_DB_Init();

	/* Init APK Event. */
	APK_EVENT_Init();

	/* Init APK Network. */
	APK_NETWORK_Init();
	APK_NETWORK_RegisterNotifier( _cloudSync_NetworkNotify );
}

static void _cloudSync_CreateMainThread( void )
{
	HERROR  hErr;

	HxLOG_Trace();

	/* Dispatch Event from Appkit & Manage Degraded Mode. */
	hErr = VK_TASK_Create( _cloudSync_MainThread, VK_TASK_PRIORITY_MW_REF, SIZE_1K, (const char*)"_cloudSync_MainThread", (void*)NULL, (unsigned long *)&s_stCSContent.ulMainThreadId, 0 );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "[%s] thread creation fail hErr = 0x%x\n", __FUNCTION__, hErr );
		return;
	}

	hErr = VK_TASK_Start( s_stCSContent.ulMainThreadId );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "[%s] thread start fail hErr = 0x%x\n", __FUNCTION__, hErr );
		return;
	}
}

static void _cloudSync_DestroyMainThread( void )
{
	HERROR  hErr;

	HxLOG_Trace();

	hErr = VK_TASK_Stop( s_stCSContent.ulMainThreadId );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "[%s] thread stop fail hErr = 0x%x\n", __FUNCTION__, hErr );
		return;
	}

	hErr = VK_TASK_Destroy( s_stCSContent.ulMainThreadId );
	if ( hErr != VK_OK )
	{
		HxLOG_Error( "[%s] thread destroy fail hErr = 0x%x\n", __FUNCTION__, hErr );
		return;
	}
}

static void _cloudSync_MainThread( void *arg1 )
{
	HxLOG_Trace();

	while (1)	/* Polling. */
	{
		/* Event Dispatch */
		APK_EVENT_Dispatch();

		/* Degraded Mode. */
		_cloudSync_DegradedMode();

		HLIB_STD_TaskSleep(300);
	}
}

HINT32 main( HINT32 argc, char ** argv )
{
	HLIB_EXTRA_DropRootPrivileges( "cloudsync" );

#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
	HLIB_DBG_Init();
#endif
	HxLOG_Trace();

	VK_Init();
	signal( SIGPIPE, SIG_IGN );

	/* Init Context. */
	_cloudSync_ResetCSContent();

	/* Init Curl. */
	HLIB_TRANSPORT_HttpInit();

	/* Init Jansson. */
	HLIB_JSON_Init( eHLIB_JSON_Conf_Minimum, NULL, NULL );

	/* Initialize APKs. */
	_cloudSync_InitializeAppkit();

	/* Initialize Profile. */
	PF_InitProfile();

	/* Create CloudSync MainThread. */
	_cloudSync_CreateMainThread();

	while( 1 )
	{
		/* Main Task has fallen asleep... */
		HLIB_STD_TaskSleep( 100000 );
	}

	/* All Destroy. */
	_cloudSync_DestroyMainThread();
	HLIB_TRANSPORT_HttpDestory();
	PF_TermProfile();

	return 0;
}

