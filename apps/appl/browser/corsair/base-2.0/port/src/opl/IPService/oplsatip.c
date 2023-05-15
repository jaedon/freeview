#include "bpldb.h"
#include "oplsatip.h"
#include "oapi.h"
#include "hapi.h"
#include <hlib.h>
#include "apk.h"

#define ________________Static_Variables___________________________
static OPL_SATIP_EventCallback	s_pf_satip_eventlistener = NULL;

#define ________________Internal_Functions_________________________ 
void  	__satip_apk_listener( HUINT32 p1, HUINT32 p2, HUINT32 p3 )
{
	switch ( p1 )
	{
		case eSATIP_Started:
			HxLOG_Error("[OPL:SATIP] eDLNA_SATIP_Started\n");
			break;

		case eSATIP_Stopped:
			HxLOG_Error("[OPL:SATIP] eDLNA_SATIP_Stopped\n");
			break;

		default:
			break;
	}

	if( NULL != s_pf_satip_eventlistener )
	{
		s_pf_satip_eventlistener( p1, p2, p3);
	}
}

#define ________________Public_Interfaces__________________________
BPLStatus	OPL_SATIP_Start( void )
{
	HCHAR 	szFriendlyName[ 128 ] = { 0, };
	HERROR 	hErr = ERR_OK;
	HCHAR	szDefaultFriendlyName[128];
	HBOOL bAddDlnaServiceName = TRUE;
	
	HxSTD_MemSet( szFriendlyName, 0x00, 128 );
	hErr = APK_DB_GetStr( USERDB_DLNA_SATIP_FRIENDLYNAME, szFriendlyName, 128 );
	if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
	{
		if (APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 128) == ERR_OK)
		{
			HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName)-1, "%s", szDefaultFriendlyName);
			if (bAddDlnaServiceName == TRUE)
				HxSTD_StrNCat(szFriendlyName, " SAT>IP Server", sizeof(szFriendlyName)-1);

			if (APK_DB_SetStr(USERDB_DLNA_SATIP_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
				HxLOG_Error("BPL_INIT: Error> DB_SetStr(SATIP) failed\n");
		}
	}

	APK_SATIP_Start(szFriendlyName);
	APK_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, (HUINT32)TRUE );
	return BPL_STATUS_OK;
}

BPLStatus	OPL_SATIP_Stop( void )
{
	APK_SATIP_Stop();
	APK_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, (HUINT32)FALSE );
	return BPL_STATUS_OK;
}

BPLStatus	OPL_SATIP_RegisterListener(OPL_SATIP_EventCallback listener )
{
	s_pf_satip_eventlistener = listener;
	APK_SATIP_RegisterNotifier((APKS_DLNA_CbHandler) __satip_apk_listener);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_SATIP_UnRegisterListener( void )
{
	APK_SATIP_UnRegisterNotifier((APKS_DLNA_CbHandler)__satip_apk_listener);
	return BPL_STATUS_OK;
}

BPLStatus 	OPL_SATIP_IsActivated(HBOOL *bEnable)
{
	HERROR hErr;
	hErr = APK_DB_GetInt(USERDB_DLNA_SATIP_ACTIVATION, (HINT32*)bEnable);
	if (hErr != ERR_OK) 
	{
		HxLOG_Error("[OPL:SATIP] Error> DB_GetInt(SATIP) failed\n");
		APK_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, (HUINT32)FALSE );
		*bEnable = FALSE;
	}
	return BPL_STATUS_OK;
}

BPLStatus 	OPL_SATIP_SetFriendlyName( const HCHAR *friendlyName )
{
	APK_SATIP_SetFriendlyName( friendlyName );
	APK_DB_SetStr( USERDB_DLNA_SATIP_FRIENDLYNAME, friendlyName );
	return BPL_STATUS_OK;
}

BPLStatus 	OPL_SATIP_GetFriendlyName(HCHAR *friendlyName, int nLen)
{
	HERROR 	hErr = ERR_OK;
	HCHAR	szDefaultFriendlyName[128];
	HBOOL bAddDlnaServiceName = TRUE;
	
	hErr = APK_DB_GetStr(USERDB_DLNA_SATIP_FRIENDLYNAME, friendlyName, nLen );
	if ((hErr != ERR_OK) || HxSTD_StrEmpty(friendlyName))
	{
		HxLOG_Error("[OPL:SATIP] Error> DB_GetStr(SATIP) failed\n");
		if (APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 128) == ERR_OK)
		{
			HxSTD_snprintf( friendlyName, sizeof(friendlyName)-1, "%s", szDefaultFriendlyName);
			if (bAddDlnaServiceName == TRUE)
				HxSTD_StrNCat(friendlyName, " SAT>IP Server", sizeof(friendlyName)-1);
				
			if (APK_DB_SetStr(USERDB_DLNA_SATIP_FRIENDLYNAME, friendlyName)  != ERR_OK)
				HxLOG_Error("[OPL:SATIP] Error> DB_SetStr(SATIP) failed\n");
		}
	}
	
	return BPL_STATUS_OK;
}
