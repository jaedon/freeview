#include "bpldb.h"
#include "opldial.h"
#include "oapi.h"
#include "hapi.h"
#include <hlib.h>
#include "apk.h"

#define ________________Static_Variables___________________________
static OPL_DIAL_EventCallback	s_pf_dial_eventlistener = NULL;

#define ________________Internal_Functions_________________________
void  	__dial_apk_listener( HUINT32 p1, HUINT32 p2, HUINT32 p3 )
{
	switch ( p1 )
	{
		case eDLNA_DIAL_ReqAppLaunch:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_ReqAppLaunch - %s / %s \n", (char *)p2, (char *)p3);
			HAPI_BroadcastSignal("signal:onDialStarted", 0, "s", (HINT32)p3);
			//return;
			break;

		case eDLNA_DIAL_ReqAppStop:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_ReqAppStop- %s\n", (char *)p2);
		//	HAPI_BroadcastSignal("signal:onDialStopped", 0, "s", (HINT32)p1);
			//return;
			break;

		case eDLNA_DIAL_ReqAppStatus:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_ReqAppStatus- %s\n", (char *)p2);
			break;

		case eDLNA_DIAL_Started:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_Started\n");
			break;

		case eDLNA_DIAL_Stopped:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_Stopped\n");
			break;

		default:
			break;
	}

	if( NULL != s_pf_dial_eventlistener )
	{
		s_pf_dial_eventlistener( p1, p2, p3);
	}
}

#define ________________Public_Interfaces__________________________
BPLStatus	OPL_DIAL_Start( void )
{
	HERROR 	hErr = ERR_OK;
	HCHAR 	szFriendlyName[ 128 ] = { 0, };
	HCHAR	szDefaultFriendlyName[128];
	HBOOL bAddDlnaServiceName = TRUE;

	HxSTD_MemSet( szFriendlyName, 0x00, 128 );
	hErr = APK_DB_GetStr( USERDB_DLNA_DIAL_FRIENDLYNAME, szFriendlyName, 128 );
	if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
	{
		if (APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 128) == ERR_OK)
		{
			HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName)-1, "%s", szDefaultFriendlyName);
			if (bAddDlnaServiceName == TRUE)
				HxSTD_StrNCat(szFriendlyName, " DIAL Server", sizeof(szFriendlyName)-1);

			if (APK_DB_SetStr(USERDB_DLNA_DIAL_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
				HxLOG_Error("[OPL:DIAL] Error> DB_SetStr(DIAL) failed\n");
		}
	}

	APK_DIAL_Start(szFriendlyName);
	APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HUINT32)TRUE );
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DIAL_Stop( void )
{
	APK_DIAL_Stop();
	APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HUINT32)FALSE );
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DIAL_RegisterListener(OPL_DIAL_EventCallback listener )
{
	s_pf_dial_eventlistener = listener;
	APK_DIAL_RegisterNotifier((APKS_DLNA_CbHandler) __dial_apk_listener);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DIAL_UnRegisterListener( void )
{
	APK_DIAL_UnRegisterNotifier((APKS_DLNA_CbHandler)__dial_apk_listener);
	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DIAL_IsActivated(HBOOL *bEnable)
{
	HERROR hErr = ERR_FAIL;
	HBOOL bStatus = FALSE;

	hErr = APK_DIAL_GetDialStatus(&bStatus);
	if(hErr == ERR_OK)
	{
		*bEnable = bStatus;
	}
	else
	{
		*bEnable = FALSE;
	}

	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DIAL_SetFriendlyName( const HCHAR *friendlyName )
{
	APK_DIAL_SetFriendlyName( friendlyName );
	APK_DB_SetStr( USERDB_DLNA_DIAL_FRIENDLYNAME, friendlyName );
	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DIAL_GetFriendlyName(HCHAR *friendlyName, int nLen)
{
	HERROR 	hErr = ERR_OK;
	HCHAR	szDefaultFriendlyName[128];
	HBOOL bAddDlnaServiceName = TRUE;

	hErr = APK_DB_GetStr(USERDB_DLNA_DIAL_FRIENDLYNAME, friendlyName, nLen );
	if ((hErr != ERR_OK) || HxSTD_StrEmpty(friendlyName))
	{
		HxLOG_Error("[OPL:DIAL] Error> DB_GetStr(DIAL) failed\n");
		if (APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 128) == ERR_OK)
		{
			HxSTD_snprintf( friendlyName, sizeof(friendlyName)-1, "%s", szDefaultFriendlyName);
			if (bAddDlnaServiceName == TRUE)
				HxSTD_StrNCat(friendlyName, " DIAL Server", sizeof(friendlyName)-1);

			if (APK_DB_SetStr(USERDB_DLNA_DIAL_FRIENDLYNAME, friendlyName)  != ERR_OK)
				HxLOG_Error("[OPL:DIAL] Error> DB_SetStr(DIAL) failed\n");
		}
	}
	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DIAL_ApplicationRegist(const HCHAR *AppName)
{
	if(APK_DIAL_AppRegist(AppName) != ERR_OK)
		return BPL_STATUS_ERROR;

	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DIAL_ApplicationUnregist(const HCHAR *AppName)
{
	if(APK_DIAL_AppUnregist(AppName) != ERR_OK)
		return BPL_STATUS_ERROR;

	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DIAL_ApplicationStateUpdate(const HCHAR *AppName, HUINT32 state)
{
	if(APK_DIAL_SetAppState(AppName, state) != ERR_OK)
		return BPL_STATUS_ERROR;

	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DIAL_SetCookie( const HCHAR * cookie ) {
	APK_DIAL_SetCookie( cookie );
	return BPL_STATUS_OK;
}
