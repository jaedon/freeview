/**************************************************************************************/
/**
 * @file opldms.cpp
 *
 * OplDMS
 *
 * @author  Sung-Su Yun(ssyun@humaxdigital.com)
 * @date    2013/02/23
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include "bpldb.h"
#include "opldms.h"
#include "oapi.h"
#include <hlib.h>
#include "apk.h"

static OPL_DMS_EventCallback	dms_eventlistener = NULL;

void  	_OPL_DMS_ApkListener( HUINT32 p1, HUINT32 p2, HUINT32 p3 )
{
	HUINT32	event = 0;
	HUINT32	result = 0;
	switch ( p1 )
	{
		case eDLNA_DMS_NotiRunState:
			{
				APKS_DLNA_DMS_RunState_t *pApkData = ( APKS_DLNA_DMS_RunState_t * )p2;
				HxLOG_Print( "[ OPL_DMS ] DMS Notify Run State\n" );
				HxLOG_Print( "[ OPL_DMS ] State : %s\n", pApkData->szRunState );
				HxLOG_Print( "[ OPL_DMS ] err_code : %d\n", pApkData->nErrCode );
			}
			break;

		case eDLNA_DMS_UnknownType:
		default:
			break;
	}

	if( NULL != dms_eventlistener )
	{
		dms_eventlistener( event, result, NULL );
	}

}

#define ____________OPL_DMS_______________
BPLStatus	OPL_DMS_Start( void )
{
	HCHAR szFriendlyName[ 128 ] = { 0, };
	HERROR hErr = ERR_OK;
	HxSTD_MemSet( szFriendlyName, 0x00, 128 );
	hErr = APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szFriendlyName, 128 );

	APK_DLNADMS_Start( szFriendlyName, FALSE );
	APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)TRUE );

	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMS_Stop( void )
{
	APK_DLNADMS_Stop();
	APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)FALSE );

	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMS_GetActionStatus(HUINT32 *status )
{
	APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, (HINT32*)status);

	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DMS_IsActivated(HBOOL *bEnable)
{
	APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, (HINT32*)bEnable);

	return BPL_STATUS_OK;
}

BPLStatus OPL_DMS_GetDmsFriendlyName( HCHAR *friendlyName, int nLen )
{
	APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, friendlyName, nLen );
	return BPL_STATUS_OK;
}

BPLStatus OPL_DMS_SetDmsFriendlyName( const HCHAR *friendlyName )
{
	(void)APK_DLNADMS_SetFriendlyName( friendlyName );
	APK_DB_SetStr( USERDB_DLNA_DMS_FRIENDLYNAME, friendlyName ); /* Store UDB regardless of setFriendlyName() failure */
	return BPL_STATUS_OK;
}


BPLStatus	OPL_DMS_RegisterListener( OPL_DMS_EventCallback listener )
{
	dms_eventlistener = listener ;

	APK_DLNADMS_RegisterNotifier((APKS_DLNA_CbHandler) _OPL_DMS_ApkListener );
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMS_UnRegisterListener( void )
{
	APK_DLNADMS_UnRegisterNotifier( ( APKS_DLNA_CbHandler)_OPL_DMS_ApkListener);
	return BPL_STATUS_OK;
}


