// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "jlabsNetworkManager.h"

#include "EmptyEvent.h"
#include "Collection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

typedef enum {
	eNET_CONNECT_TRY_FAIL = 0,
	eNET_CONNECT_TRY_SUCCESS,

	eNET_CABLE_LINK_DICONNECTED,
	eNET_CABLE_LINK_CONNECTD,

	eNET_WIFI_USB_EXTRACTED,
	eNET_WIFI_USB_INSERTED,
} eNetStatusEvent;


typedef enum {
	eNET_WIFI_SCANNING_OUTPUT = 0,
	eNET_WIFI_SCANNING_END,
	eNET_WIFI_SCANNING_FAIL,
	eNET_WIFI_CONNECT_SUCCESS,
	eNET_WIFI_CONNECT_FAIL
} eNetWifiEvent;

using namespace Ooif;


/***********************************************************************************
 *  jlabsNetworkManager Class
 ***********************************************************************************
 */
void * jlabsNetworkManager::m_sNetManagerIdentifier = NULL;
list<jlabsNetworkManager *> jlabsNetworkManager::instances;
jlabsNetworkManager::jlabsNetworkManager() :  EventTarget()
{
	ENTRY;
	classType = CLASS_NETWORKMANAGER;
	jlabsNetworkManager::instances.push_back(this);
}

jlabsNetworkManager::~jlabsNetworkManager( )
{
	jlabsNetworkManager::instances.remove(this);
	if ( 0  == jlabsNetworkManager::instances.size() )
	{
		m_sNetManagerIdentifier = NULL;
		native_HNetworkManagerUnRegisterEventHandler();
	}
}

int jlabsNetworkManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	INTRINSIC_EVENT_GET(NetworkStatusChanged);

	NATIVE_GET_NUMBER("defaultDevIndex", native_HNetworkManagerDefaultDevIndex( &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("linkStatus", native_HNetworkManagerLinkStatus( &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getWifiConfig, obj, "n", JSP_GET_VALUE_CACHE);

//	GET_FUNCTION(getLinkStatus, obj, "", JSP_GET_VALUE_CACHE);
//	GET_FUNCTION(getDefaultDevIndex, obj, "", JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int jlabsNetworkManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	if ( !strcmp("onNetworkStatusChanged",name ) )
	{
		m_sNetManagerIdentifier = this;
		native_HNetworkManagerRegisterEventHandler( jlabsNetworkManager::callbackEventHandler );
		INTRINSIC_EVENT(NetworkStatusChanged);
	}
	return JSP_PUT_NOTFOUND;
}

//static
int	jlabsNetworkManager::getWifiConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int devIndex = NUMBER_VALUE(argv[0]);
	//TEMP
	if ( devIndex == 0 )
		return JSP_GET_NOTFOUND;

	RETURN_OBJECT(this_obj, new jlabsNetworkWiFiConfig( devIndex ) ,result, JSP_GET_VALUE);
}

//static
int	jlabsNetworkManager::getLinkStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
void jlabsNetworkManager::callbackEventHandler( int state, HNetworkHandle id, int nDevIndex )
{
	Event *e =  NULL;
	jlabsNetworkManager *netManager = NULL;
	jsplugin_obj *context = NULL;

	if ( NULL == m_sNetManagerIdentifier )
		return ;

#if 0
	netManager = (jlabsNetworkManager *)m_sNetManagerIdentifier;
	context = netManager->getHost();
	e = new jlabsNetworkLinkStatusEvent(context, "NetworkStatusChanged", state);

	netManager->dispatchEvent(e);
#else
	for ( list<jlabsNetworkManager *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		netManager = (jlabsNetworkManager *)(*it);
		context = netManager->getHost();
		e = new jlabsNetworkLinkStatusEvent(context, "NetworkStatusChanged", state);
		netManager->dispatchEvent(e);
	}
#endif
}


/***********************************************************************************
 *  jlabsNetworkWiFiConfig Class
 ***********************************************************************************
 */

int		jlabsNetworkWiFiConfig::m_nWifiDevIndex = 0;
char *	jlabsNetworkWiFiConfig::m_szKey = NULL;
list<jlabsNetworkWiFiConfig *> jlabsNetworkWiFiConfig::instances;
jlabsNetworkWiFiConfig::jlabsNetworkWiFiConfig( ) : EventTarget()
{
	ENTRY;
	classType = CLASS_NETWORKWIFICONFIG;
}

void * jlabsNetworkWiFiConfig::m_sApScanningIdentifier = NULL;

jlabsNetworkWiFiConfig::jlabsNetworkWiFiConfig( int devIndex ) : EventTarget()
{
	ENTRY;
//	OOIF_LOG_PRINT("\n######################################\n");
//	OOIF_LOG_PRINT("\n### jlabsNetworkWiFiConfig() create ###\n");
//	OOIF_LOG_PRINT("\n######################################\n");
	classType = CLASS_NETWORKWIFICONFIG;
	m_nWifiDevIndex = devIndex;
	m_sApScanningIdentifier = this;
	jlabsNetworkWiFiConfig::instances.push_back(this);

	native_HNetworkAPScanningResultListener ( jlabsNetworkWiFiConfig::callbackEventHandler );
}

jlabsNetworkWiFiConfig::~jlabsNetworkWiFiConfig()
{
	if( NULL != m_szKey )
	{
		delete m_szKey;
		m_szKey = NULL;
	}

	jlabsNetworkWiFiConfig::instances.remove(this);
	if ( 0  == jlabsNetworkWiFiConfig::instances.size() )
	{	
		m_sApScanningIdentifier = NULL;
		native_HNetworkAPScanningResultListenerRelease ( jlabsNetworkWiFiConfig::callbackEventHandler );
	}
}

int jlabsNetworkWiFiConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{

	INTRINSIC_EVENT_GET(AccessPointUpdated);
	GET_FUNCTION(isConnectedAP, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(apScanning, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setKey, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(applyConnectToAP, obj, "os", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getWaveStrength, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getConnectedAPInfo, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsNetworkWiFiConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	INTRINSIC_EVENT(AccessPointUpdated);
	return JSP_PUT_NOTFOUND;
}

//static
void jlabsNetworkWiFiConfig::releaseNative( HNetworkHandle id )
{
}
//static
int	jlabsNetworkWiFiConfig::isConnectedAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int	jlabsNetworkWiFiConfig::apScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;

	native_HNetworkWiFiConfigApScanning( m_nWifiDevIndex, &retval );

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
#define KEY_LEN_MAX 64
//static
int	jlabsNetworkWiFiConfig::setKey(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	if( NULL != m_szKey )
	{
		delete m_szKey;
		m_szKey = NULL;
	}
	m_szKey	= new char[KEY_LEN_MAX];
	memset(m_szKey, 0x00, sizeof(char)*KEY_LEN_MAX);
	strncpy( m_szKey, STRING_VALUE(argv[0]), KEY_LEN_MAX-1 );

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int	jlabsNetworkWiFiConfig::applyConnectToAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	OOIFBoolean retval = TRUE;
	void * apIdentifier = NULL;
	const char * apKey = NULL;

	jlabsNetworkAccessPoint * ap = OBJECT_VALUE( argv[0], jlabsNetworkAccessPoint );

	if ( NULL != ap )
		apIdentifier =  ap->getIdentifier();
	else
	{
		retval = FALSE;
		RETURN_NUMBER(retval, JSP_CALL_VALUE);
	}

	apKey = STRING_VALUE(argv[1]);

	native_HNetworkWiFiConfigApplyConnectToAP( m_nWifiDevIndex, apIdentifier, apKey, &retval );

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int	jlabsNetworkWiFiConfig::getWaveStrength(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;


	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int	jlabsNetworkWiFiConfig::getConnectedAPInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	jlabsNetworkAccessPoint *apInfo = NULL;
	NetworkApHandle identifier = NULL;
	native_HNetworkWiFiGetConnectedApInfo( m_nWifiDevIndex, &identifier );
	if ( NULL == identifier )
	{
		RETURN_NULL(JSP_GET_VALUE);
	} 
	
	apInfo = new jlabsNetworkAccessPoint(identifier);

	RETURN_OBJECT(this_obj, apInfo ,result, JSP_GET_VALUE);
}

//static
void jlabsNetworkWiFiConfig::callbackEventHandler( int event, HNetworkHandle id, int nDevCount)
{
//	ENTRY;
	jlabsNetworkWiFiConfig *wifiConfig = NULL;
	jlabsNetworkWiFiResultEvent *e = NULL;
	jsplugin_obj *context = NULL;

	if ( NULL == m_sApScanningIdentifier )
	{
		return ;
	}
#if 0
	wifiConfig = (jlabsNetworkWiFiConfig *)m_sApScanningIdentifier;
	context = wifiConfig->getHost();

	jlabsNetworkWiFiResultEvent *e = new jlabsNetworkWiFiResultEvent(context, id, event);
	wifiConfig->dispatchEvent(e);
#else
	for ( list<jlabsNetworkWiFiConfig *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		wifiConfig = (jlabsNetworkWiFiConfig *)(*it);
		context = wifiConfig->getHost();
		e = new jlabsNetworkWiFiResultEvent(context, id, event);
		wifiConfig->dispatchEvent(e);
	}
#endif
}

/***********************************************************************************
 *  jlabsNetworkAccessPoint Class
 ***********************************************************************************
 */

jlabsNetworkAccessPoint::jlabsNetworkAccessPoint( NetworkApHandle id ) : WrapperObject<jlabsNetworkAccessPoint>(id)
{
	classType = CLASS_NETWORKACCESSPOINT;
}

jlabsNetworkAccessPoint::jlabsNetworkAccessPoint( const jlabsNetworkAccessPoint &original) : WrapperObject<jlabsNetworkAccessPoint>(original.identifier)
{
	classType = CLASS_NETWORKACCESSPOINT;

}
jlabsNetworkAccessPoint::~jlabsNetworkAccessPoint()
{
	ENTRY;
}

int jlabsNetworkAccessPoint::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	NATIVE_GET_STRING("essid", native_HNetworkAPGetEssid(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("secure", native_HNetworkAPGetSecureType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("authen", native_HNetworkAPGetAuthenType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("encrypt", native_HNetworkAPGetEncryptType(identifier, &RETVAL), JSP_GET_VALUE);

	/* read only */
	NATIVE_GET_NUMBER("waveStrength", native_HNetworkAPGetWaveStrength(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("connectSpeed", native_HNetworkAPGetConnectSpeed(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int jlabsNetworkAccessPoint::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	NATIVE_SET("essid", native_HNetworkAPSetEssid(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("secure", native_HNetworkAPSetSecureType(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("authen", native_HNetworkAPSetAuthenType(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("encrypt", native_HNetworkAPSetEncryptType(identifier, NUMBER_VALUE(value[0])));
	return JSP_PUT_NOTFOUND;
}

//static
void jlabsNetworkAccessPoint::releaseNative( NetworkApHandle id )
{
	NATIVE_WARN_NOK(native_HNetworkAccessPointReleaseHandle(id));
}

/***********************************************************************************
 *  jlabsNetworkLinkStatusEvent Class
 ***********************************************************************************
 */
jlabsNetworkLinkStatusEvent::jlabsNetworkLinkStatusEvent( jsplugin_obj *c, const char *eventName, int state ) : Event( c, eventName), m_nState( state )
{
	ENTRY;
}
jlabsNetworkLinkStatusEvent::~jlabsNetworkLinkStatusEvent()
{
	ENTRY;
}

int jlabsNetworkLinkStatusEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	return JSP_GET_NOTFOUND;
}

int jlabsNetworkLinkStatusEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

//virtual
int jlabsNetworkLinkStatusEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nState;

	return 1;
}
/***********************************************************************************
 *  jlabsNetworkWiFiResultEvent Class
 ***********************************************************************************
 */
jlabsNetworkWiFiResultEvent::jlabsNetworkWiFiResultEvent( jsplugin_obj *c, NetworkApHandle handleId, int state ) : Event( c, "AccessPointUpdated"), ap(handleId), m_nState( state )
{
	ENTRY;
}

jlabsNetworkWiFiResultEvent::~jlabsNetworkWiFiResultEvent()
{
	ENTRY;
}

int jlabsNetworkWiFiResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	return JSP_GET_NOTFOUND;
}

int jlabsNetworkWiFiResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

//virtual
int jlabsNetworkWiFiResultEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nState;

	SET_SERIALIZED_OBJECT(new jlabsNetworkAccessPoint(ap), 1);
	return 2;
}

#endif // OIPF
