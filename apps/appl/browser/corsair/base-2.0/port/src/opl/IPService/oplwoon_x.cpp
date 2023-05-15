/**************************************************************************************/
/**
 * @file oplnetwork.cpp
 *
 * OplNetworkInterface
 *
 * @author  Mun-ho Park(mhpark3@humaxdigital.com)
 * @date    2011/10/24
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include "bpldb.h"
#include "oplwoon.h"

#include "oapi.h"
#include "apk.h"
#include "hapi.h"

#define ____________OPL_WOON_SERVER_______________
static HUINT32							g_nLastEventType;
static WoonServerOpResult		 	 	g_stOperated = {0,};
static WoonServerInfo 					g_stWoonServerInfo = {0,};
static WoonServerPinCode				g_stWoonServerPinCode = {0,};
static OPL_WOONSERVER_EventCallback 	s_pfn_woonServer_callback;

BPLStatus	OPL_WOONSERVER_Start( const HCHAR *szFriendlyName )
{
	HxLOG_Debug("Start WoonServer with [%s] \n", szFriendlyName);
	APK_WOONSERVER_Start((HCHAR*)szFriendlyName);
	APK_DB_SetInt(USERDB_WOONSERVER_ACTIVATION, (HUINT32)TRUE );

	return BPL_STATUS_OK;
}

BPLStatus	OPL_WOONSERVER_Stop( void )
{
	APK_WOONSERVER_Stop();
	APK_DB_SetInt(USERDB_WOONSERVER_ACTIVATION, (HUINT32)FALSE );

	return BPL_STATUS_OK;
}

BPLStatus OPL_WOONSERVER_TestConnection()
{
	APK_WOONSERVER_TestConnection();
	return BPL_STATUS_OK;
}

BPLStatus OPL_WOONSERVER_GetServerName(HCHAR *szFriendlyName, int nLen)
{
	BPLStatus ret = BPL_STATUS_OK;
	HERROR hErr;

	hErr = APK_DB_GetStr( USERDB_WOONSERVER_FRIENDLYNAME, szFriendlyName, nLen );
	HxLOG_Debug("Get WoonServer Name : %s \n", szFriendlyName);
	if(ERR_FAIL == hErr)
	{
		ret = BPL_STATUS_ERROR;
	}
	return ret;
}

BPLStatus OPL_WOONSERVER_SetServerName(const HCHAR *szFriendlyName)
{
	BPLStatus ret = BPL_STATUS_OK;
	HERROR hErr = APK_WOONSERVER_SetFriendlyName(szFriendlyName);
	HxLOG_Debug("Set WoonServer Name : %s \n", szFriendlyName);
	if(ERR_FAIL == hErr )
	{
		ret = BPL_STATUS_ERROR;
	}

	APK_DB_SetStr( USERDB_WOONSERVER_FRIENDLYNAME, szFriendlyName );

	return ret;
}
BPLStatus OPL_WOONSERVER_IsActivated(HBOOL *bActivated)
{
	APK_DB_GetInt(USERDB_WOONSERVER_ACTIVATION, (HINT32*)bActivated);
	return BPL_STATUS_OK;
}

static	void	OPL_WOONSERVER_EventNotifier( HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	HxLOG_Error("Woon event(%d, last:%d)\n", ulParam1, g_nLastEventType);
	g_nLastEventType 	= ulParam1;

	switch(ulParam1){
		case eWOONSERVER_StartCmd :
		case eWOONSERVER_StopCmd :
		case eWOONSERVER_SetFriendlyName:
			{
				HxSTD_MemCopy(&g_stOperated, (WoonServerOpResult*)ulParam2, sizeof(WoonServerOpResult));
				s_pfn_woonServer_callback(ulParam1, g_stOperated.isOperated, NULL);
			}
			break;
		case eWOONSERVER_TestConnection:
			{
				HxSTD_MemCopy(&g_stWoonServerInfo, (WoonServerInfo*)ulParam2, sizeof(WoonServerInfo));
				s_pfn_woonServer_callback(ulParam1, g_stWoonServerInfo.nState, &g_stWoonServerInfo);
			}
			break;
		case eWOONSERVER_NotifyPinCode:
			{
				HxSTD_MemCopy(&g_stWoonServerPinCode, (WoonServerPinCode*)ulParam2, sizeof(WoonServerPinCode));
				HxLOG_Error("############## bShow : %d, PIN_CODE : %s\n", g_stWoonServerPinCode.bOpen, g_stWoonServerPinCode.szPinCode);
				s_pfn_woonServer_callback(ulParam1, g_stWoonServerPinCode.bOpen, g_stWoonServerPinCode.szPinCode);
			}
			break;
		default:
			break;
	}
}
BPLStatus OPL_WOONSERVER_RegEventListener(OPL_WOONSERVER_EventCallback listener)
{
	s_pfn_woonServer_callback = listener;
	APK_WOONSERVER_RegisterNotifier(OPL_WOONSERVER_EventNotifier);
	return BPL_STATUS_OK;
}
BPLStatus OPL_WOONSERVER_UnRegEventListener()
{
	APK_WOONSERVER_UnRegisterNotifier(OPL_WOONSERVER_EventNotifier);
	return BPL_STATUS_OK;
}


