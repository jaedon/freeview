// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "HNetworkInterface.h"

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

	eNET_CONNECT_AUTOIP_FAIL,
	eNET_CONNECT_AUTOIP_SUCCESS,

	eNET_ACCESS_GW_FAIL,
	eNET_ACCESS_GW_SUCCESS,
	eNET_ACCESS_DNS_FAIL,
	eNET_ACCESS_DNS_SUCCESS,
	eNET_ACCESS_INTERNET_FAIL,
	eNET_ACCESS_INTERNET_SUCCESS,

	/* reference OplNetLinkStatus_e */
} eNetStatusEvent;


typedef enum {
	eNET_WIFI_SCANNING_OUTPUT = 0,
	eNET_WIFI_SCANNING_END,
	eNET_WIFI_SCANNING_FAIL,
	eNET_WIFI_CONNECT_SUCCESS,
	eNET_WIFI_CONNECT_FAIL,
	eNET_WIFI_KEY_INVALID,
	eNET_WIFI_WPS_SUCCESS,
	eNET_WIFI_WPS_FAIL,
} eNetWifiEvent;

using namespace Ooif;

list<HNetworkInterface *> HNetworkInterface::instances;

HNetworkInterface::HNetworkInterface(HNetworkInterfaceHandle i) : EventTarget(), WrapperObject<HNetworkInterface>(i)
{
	ENTRY;
	classType = CLASS_NETWORKINTERFACE;
  	instances.push_back(this);
    
	if ( 1 == instances.size() )
	{
		native_HNetworkInterfaceRegisterEventHandler( identifier, HNetworkInterface::callbackEventHandler );
	}

}

HNetworkInterface::HNetworkInterface(const HNetworkInterface &original) : EventTarget(), WrapperObject<HNetworkInterface>(original.identifier)
{
	ENTRY;
	classType = CLASS_NETWORKINTERFACE;
  	instances.push_back(this);
}

HNetworkInterface::~HNetworkInterface()
{
	instances.remove(this);
	if ( 0 == instances.size() )
	{
		native_HNetworkInterfaceUnRegisterEventHandler( identifier );
	}
}

int HNetworkInterface::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("ipAddress", native_HNetworkInterfaceGetIPAddress(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("macAddress", native_HNetworkInterfaceGetMacAddress(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("connected", native_HNetworkInterfaceGetConnected(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("enabled", native_HNetworkInterfaceGetEnabled(identifier, &RETVAL), JSP_GET_VALUE);

	// Humax Extension
	NATIVE_GET_STRING("netmask", native_HNetworkInterfaceGetNetmask(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("gateway", native_HNetworkInterfaceGetGateway(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("dns1st", native_HNetworkInterfaceGetDns1(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("dns2nd", native_HNetworkInterfaceGetDns2(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("networkType", native_HNetworkInterfaceGetNetworkType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("dhcpOn", native_HNetworkInterfaceGetDHCPStatus(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("dnsAuto", native_HNetworkInterfaceGetDNSAutoStatus(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING("ipv6Address",	native_HNetworkInterfaceGetIpv6Address(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("ipv6Prefix",		native_HNetworkInterfaceGetIpv6Prefix(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("ipv6Gateway",	native_HNetworkInterfaceGetIpv6Gateway(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("ipv6dns1st",		native_HNetworkInterfaceGetIpv6Dns1(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("ipv6dns2nd",		native_HNetworkInterfaceGetIpv6Dns2(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("ipv6dhcpOn",	native_HNetworkInterfaceGetIpv6DHCPStatus(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("ipv6dnsAuto",	native_HNetworkInterfaceGetIpv6DNSAutoStatus(identifier, &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(applyConfigure, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(stopConnecting, obj, "", JSP_GET_VALUE);

	GET_FUNCTION(checkGatewayAccess, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(checkDnsAccess, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(checkInternetConnection, obj, "", JSP_GET_VALUE);

	//onLinkStatusChanged
	INTRINSIC_EVENT_GET(LinkStatusChanged);
	INTRINSIC_EVENT_GET(AccessStatusUpdated);
	GET_FUNCTION(addEventListener, obj, "sos", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HNetworkInterface::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("enabled", native_HNetworkInterfaceSetEnabled(identifier, BOOLEAN_VALUE(value[0])));

#if 1	// Humax Extension
	NATIVE_SET("ipAddress", native_HNetworkInterfaceSetIPAddres(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("netmask", native_HNetworkInterfaceSetNetmask(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("gateway", native_HNetworkInterfaceSetGateway(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("dns1st", native_HNetworkInterfaceSetDNS1(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("dns2nd", native_HNetworkInterfaceSetDNS2(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("dhcpOn", native_HNetworkInterfaceSetDHCPStatus(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("dnsAuto", native_HNetworkInterfaceSetDNSAutoStatus(identifier, BOOLEAN_VALUE(value[0])));

	NATIVE_SET("ipv6Address", native_HNetworkInterfaceSetIpv6Addres(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("ipv6Prefix",  native_HNetworkInterfaceSetIpv6Prefix(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("ipv6gateway", native_HNetworkInterfaceSetIpv6Gateway (identifier, STRING_VALUE(value[0])));
	NATIVE_SET("ipv6dns1st",  native_HNetworkInterfaceSetIpv6DNS1(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("ipv6dns2nd",  native_HNetworkInterfaceSetIpv6DNS2(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("ipv6dhcpOn",  native_HNetworkInterfaceSetIpv6DHCPStatus(identifier,		BOOLEAN_VALUE(value[0])));
	NATIVE_SET("ipv6dnsAuto", native_HNetworkInterfaceSetIpv6DNSAutoStatus(identifier,	BOOLEAN_VALUE(value[0])));

	INTRINSIC_EVENT(LinkStatusChanged);
	INTRINSIC_EVENT(AccessStatusUpdated);
#endif

	return JSP_PUT_NOTFOUND;
}

void HNetworkInterface::releaseNative(HNetworkInterfaceHandle id)
{
	NATIVE_WARN_NOK(native_HNetworkInterfaceReleaseHandle(id));
}

OOIFReturnCode HNetworkInterface::checkListenerAccessAllowed(const char *eventName)
{

#ifdef HBBTV_SECURITY
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("LinkStatusChanged", TRUE);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("AccessStatusUpdated", TRUE);
#endif // HBBTV_SECURITY
	return OOIF_RETURN_OK;
}

HNetworkInterface * HNetworkInterface::GetInstanceById( HNetworkInterfaceHandle id )
{
	for ( list<HNetworkInterface *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		if ( (*it)->getIdentifier() == id )
			return *it;
	}
	return NULL;
}

int	HNetworkInterface::applyConfigure(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval;

	HNetworkInterface *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, HNetworkInterface );
	NATIVE_CALL( native_HNetworkInterfaceApplyConfigure(instance->getIdentifier(), &retval) );

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HNetworkInterface::stopConnecting(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval;

	HNetworkInterface *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, HNetworkInterface );
	NATIVE_CALL( native_HNetworkInterfaceStopConnecting(instance->getIdentifier(), &retval) );

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HNetworkInterface::checkGatewayAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval;

	HNetworkInterface *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, HNetworkInterface );
	NATIVE_CALL( native_HNetworkInterfaceCheckGatewayAccess(instance->getIdentifier(), &retval) );
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HNetworkInterface::checkDnsAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval;

	HNetworkInterface *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, HNetworkInterface );
	NATIVE_CALL( native_HNetworkInterfaceCheckDnsAccess(instance->getIdentifier(), &retval) );
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HNetworkInterface::checkInternetConnection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval;

	HNetworkInterface *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, HNetworkInterface );
	NATIVE_CALL( native_HNetworkInterfaceCheckInternetConnection(instance->getIdentifier(), &retval) );
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

#define OPL_CONNECT_GW_FAIL 8
void HNetworkInterface::callbackEventHandler( int event, HNetworkHandle id, int nDevIndex)
{
	HNetworkInterface *netInterface = NULL;
	jsplugin_obj *context  = NULL;
	Event *e = NULL;
	OOIFNumber	nInsanceIndex = 0;
	HNetworkInterfaceHandle hInstanceId = NULL;

	for ( list<HNetworkInterface *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		e = NULL;
		hInstanceId = (*it)->getIdentifier();
		nInsanceIndex = -1;
		native_HNetworkInterfaceGetIndex( hInstanceId, &nInsanceIndex);
		OOIF_LOG_DEBUG("\n(*) [%s,%d] Network Event(%d) - EventIndex(%d), instanceIndex(%d)\n", __FUNCTION__,__LINE__, event, nDevIndex, nInsanceIndex );
		if ( nDevIndex == nInsanceIndex )
		{
			netInterface = (*it);
			if( NULL == netInterface )
			{
				continue;
			}

			context = netInterface->getHost();
			if ( NULL == context )
			{
				continue;
			}

			switch ( event )
			{
				case eNET_ACCESS_GW_FAIL :
				case eNET_ACCESS_GW_SUCCESS :
				case eNET_ACCESS_DNS_FAIL :
				case eNET_ACCESS_DNS_SUCCESS :
				case eNET_ACCESS_INTERNET_FAIL :
				case eNET_ACCESS_INTERNET_SUCCESS :
					e =  new HMX_NetworkLinkStatusEvent(context, "AccessStatusUpdated", event-eNET_ACCESS_GW_FAIL);
					break;
				default :
					e =  new HMX_NetworkLinkStatusEvent(context, "LinkStatusChanged", event);
					break;
			}

			if ( NULL != e )
			{
				netInterface->dispatchEvent(e);
			}
		}
	}
}


/***********************************************************************************
 *  HMX_NetworkManager Class
 ***********************************************************************************
 */
list<HMX_NetworkManager *> HMX_NetworkManager::instances;
HMX_NetworkManager::HMX_NetworkManager() :  EventTarget()
{
	ENTRY;
	classType = CLASS_NETWORKMANAGER;
	if ( 0 == HMX_NetworkManager::instances.size() )
	{
		native_HNetworkManagerRegisterEventHandler( HMX_NetworkManager::callbackEventHandler );
	}
	HMX_NetworkManager::instances.push_back(this);
}

HMX_NetworkManager::~HMX_NetworkManager( )
{
	HMX_NetworkManager::instances.remove(this);
	if ( 0  == HMX_NetworkManager::instances.size() )
	{
		native_HNetworkManagerUnRegisterEventHandler();
	}
}

int HMX_NetworkManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	INTRINSIC_EVENT_GET(NetworkStatusChanged);

	NATIVE_GET_NUMBER("defaultDevIndex", native_HNetworkManagerDefaultDevIndex( &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("linkStatus", native_HNetworkManagerLinkStatus( &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getWifiConfig, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getDMSConfig, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getFTPConfig, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getPPPoEConfig, obj, "n", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMX_NetworkManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	INTRINSIC_EVENT(NetworkStatusChanged);
	return JSP_PUT_NOTFOUND;
}

void HMX_NetworkManager::releaseNative( HNetworkHandle id )
{
}

int	HMX_NetworkManager::getWifiConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	int devIndex = NUMBER_VALUE(argv[0]);
	//TEMP
	if ( devIndex == 0 )
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	RETURN_OBJECT(this_obj, new HMX_NetworkWiFiConfig( devIndex ) ,result, JSP_GET_VALUE);
}

int	HMX_NetworkManager::getLinkStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HMX_NetworkManager::getFTPConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	RETURN_OBJECT(this_obj, new HMX_NetworkFTPConfig() ,result, JSP_GET_VALUE);
}

int	HMX_NetworkManager::getDMSConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	RETURN_OBJECT(this_obj, new HMX_NetworkDMSConfig() ,result, JSP_GET_VALUE);
}

int	HMX_NetworkManager::getPPPoEConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	int devIndex = NUMBER_VALUE(argv[0]);
	if ( devIndex == 0 )
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	RETURN_OBJECT(this_obj, new HMX_NetworkPPPoEConfig(devIndex) ,result, JSP_GET_VALUE);
}

void HMX_NetworkManager::callbackEventHandler( int state, HNetworkHandle id, int nDevIndex )
{
	Event *e =  NULL;
	HMX_NetworkManager *netManager = NULL;
	jsplugin_obj *context = NULL;

	for ( list<HMX_NetworkManager *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		netManager = (HMX_NetworkManager *)(*it);
		context = netManager->getHost();
		if ( NULL == context )
		{
			continue;
		}
		e = new HMX_NetworkLinkStatusEvent(context, "NetworkStatusChanged", state);
		if ( NULL != e )
		{
			netManager->dispatchEvent(e);
		}
	}
}

/***********************************************************************************
 *  HMX_NetworkFTPConfig Class
 ***********************************************************************************
 */

HMX_NetworkFTPConfig::HMX_NetworkFTPConfig()
{
	ENTRY;
	classType = CLASS_NETWORKFTPCONFIG;
}

HMX_NetworkFTPConfig::~HMX_NetworkFTPConfig( )
{
}

int HMX_NetworkFTPConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	GET_FUNCTION(FTPServerOn, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(FTPServerOff, obj, "", JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("ftpStatus", native_HNetworkManagerGetFTPServer( &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HMX_NetworkFTPConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	return JSP_PUT_NOTFOUND;
}
void HMX_NetworkFTPConfig::releaseNative( void *id )
{
}

int	HMX_NetworkFTPConfig::FTPServerOn(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	native_HNetworkManagerSetFTPServer(TRUE);
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HMX_NetworkFTPConfig::FTPServerOff(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	native_HNetworkManagerSetFTPServer(FALSE);
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}


/***********************************************************************************
 *  HMX_NetworkDMSConfig Class
 ***********************************************************************************
 */
HMX_NetworkDMSConfig::HMX_NetworkDMSConfig()
{
	ENTRY;
	classType = CLASS_NETWORKDMSCONFIG;
}

HMX_NetworkDMSConfig::~HMX_NetworkDMSConfig( )
{
}

int HMX_NetworkDMSConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	GET_FUNCTION(start, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, obj, "", JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("actionStatus", native_HNetworkDMSGetStatus( &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HMX_NetworkDMSConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	return JSP_PUT_NOTFOUND;
}

int	HMX_NetworkDMSConfig::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	native_HNetworkDMSSetAcionMode(TRUE);
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HMX_NetworkDMSConfig::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	native_HNetworkDMSSetAcionMode(FALSE);
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

/***********************************************************************************
 *  HMX_NetworkPPPoEConfig Class
 ***********************************************************************************
 */
#define ID_LEN_MAX 64
#define PW_LEN_MAX 128
list<HMX_NetworkPPPoEConfig *> HMX_NetworkPPPoEConfig::instances;
int		HMX_NetworkPPPoEConfig::m_nPPPoEDevIndex = 0;
char *	HMX_NetworkPPPoEConfig::m_szID = NULL;
char *	HMX_NetworkPPPoEConfig::m_szPassword = NULL;
HMX_NetworkPPPoEConfig::HMX_NetworkPPPoEConfig(int devIndex) : EventTarget()
{
	ENTRY;
	classType = CLASS_NETWORKPPPOECONFIG;
	m_nPPPoEDevIndex = devIndex;
	if ( NULL == m_szID )
	{
		m_szID = new char[ID_LEN_MAX];
	}
	if ( NULL == m_szPassword )
	{
		m_szPassword = new char[PW_LEN_MAX];
	}
	native_HNetworkPPPoEConfigGetInfo(m_nPPPoEDevIndex, &m_szID, &m_szPassword);

	if ( 0 == instances.size() )
	{
		native_HNetworkPPPoEConfigRegisterEventHandler ( HMX_NetworkPPPoEConfig::callbackEventHandler );
	}
	HMX_NetworkPPPoEConfig::instances.push_back(this);
}

HMX_NetworkPPPoEConfig::~HMX_NetworkPPPoEConfig()
{
	HMX_NetworkPPPoEConfig::instances.remove(this);
	if ( 0  == instances.size() )
	{
		native_HNetworkPPPoEConfigUnRegisterEventHandler ();
	}

	if ( NULL != m_szID )
	{
		delete []m_szID;
		m_szID = NULL;
	}
	if ( NULL != m_szPassword )
	{
		delete []m_szPassword;
		m_szPassword = NULL;
	}
}

int HMX_NetworkPPPoEConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	INTRINSIC_EVENT_GET(PPPoEStatusChanged);

	GET_FUNCTION(getOn, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getID, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getPassword, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setOn, obj, "b", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setID, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPassword, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(connect, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(disconnect, obj, "", JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HMX_NetworkPPPoEConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	INTRINSIC_EVENT(PPPoEStatusChanged);
	return JSP_PUT_NOTFOUND;
}

int HMX_NetworkPPPoEConfig::getOn(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;
	native_HNetworkPPPoEConfigGetOnOff(m_nPPPoEDevIndex, &retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int HMX_NetworkPPPoEConfig::getID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	RETURN_STRING((const char *)m_szID, JSP_CALL_VALUE);
}

int HMX_NetworkPPPoEConfig::getPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	RETURN_STRING((const char *)m_szPassword, JSP_CALL_VALUE);
}

int HMX_NetworkPPPoEConfig::setOn(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	NATIVE_CALL(native_HNetworkPPPoEConfigSetOnOff(m_nPPPoEDevIndex, BOOLEAN_VALUE(argv[0])));
	return JSP_CALL_NO_VALUE;
}

int HMX_NetworkPPPoEConfig::setID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	strncpy( m_szID, STRING_VALUE(argv[0]), ID_LEN_MAX-1 );
	NATIVE_CALL(native_HNetworkPPPoEConfigSetInfo(m_nPPPoEDevIndex, m_szID, m_szPassword));
	return JSP_CALL_NO_VALUE;
}

int HMX_NetworkPPPoEConfig::setPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	strncpy( m_szPassword, STRING_VALUE(argv[0]), PW_LEN_MAX-1 );
	NATIVE_CALL(native_HNetworkPPPoEConfigSetInfo(m_nPPPoEDevIndex, m_szID, m_szPassword));
	return JSP_CALL_NO_VALUE;
}

int	HMX_NetworkPPPoEConfig::connect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	native_HNetworkPPPoEConfigConnect(m_nPPPoEDevIndex, m_szID, m_szPassword, &retval);
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HMX_NetworkPPPoEConfig::disconnect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	native_HNetworkPPPoEConfigDisconnect(m_nPPPoEDevIndex, &retval);
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

void HMX_NetworkPPPoEConfig::callbackEventHandler(int event, HNetworkHandle id, int nDevCount)
{
	Event *e =  NULL;
	HMX_NetworkPPPoEConfig *pppoeConfig = NULL;
	jsplugin_obj *context = NULL;

	for ( list<HMX_NetworkPPPoEConfig *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		pppoeConfig = (HMX_NetworkPPPoEConfig *)(*it);
		context = pppoeConfig->getHost();
		if ( NULL == context )
		{
			continue;
		}

		e = new HMX_NetworkLinkStatusEvent(context, "PPPoEStatusChanged", event);
		if ( NULL != e )
		{
			pppoeConfig->dispatchEvent(e);
		}
	}
}

/***********************************************************************************
 *  HMX_NetworkWiFiConfig Class
 ***********************************************************************************
 */
int		HMX_NetworkWiFiConfig::m_nWifiDevIndex = 0;
char *	HMX_NetworkWiFiConfig::m_szKey = NULL;
HMX_NetworkWiFiConfig *	HMX_NetworkWiFiConfig::m_sScanningInstance = NULL;
HMX_NetworkWiFiConfig *	HMX_NetworkWiFiConfig::m_sConnectInstance = NULL;
list<HMX_NetworkWiFiConfig *> HMX_NetworkWiFiConfig::instances;
HMX_NetworkWiFiConfig::HMX_NetworkWiFiConfig( int devIndex ) : EventTarget(), m_bStopScan(FALSE)
{
	ENTRY;
	classType = CLASS_NETWORKWIFICONFIG;
	m_nWifiDevIndex = devIndex;
	if ( 0 == instances.size() )
	{
		native_HNetworkAPScanningResultListener ( HMX_NetworkWiFiConfig::callbackEventHandler );
	}

	HMX_NetworkWiFiConfig::instances.push_back(this);
}

HMX_NetworkWiFiConfig::~HMX_NetworkWiFiConfig()
{
	if( NULL != m_szKey )
	{
		delete []m_szKey;
		m_szKey = NULL;
	}

	HMX_NetworkWiFiConfig::instances.remove(this);
	if ( 0  == instances.size() )
	{
		native_HNetworkAPScanningResultListenerRelease ( HMX_NetworkWiFiConfig::callbackEventHandler );
	}
}

int HMX_NetworkWiFiConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	INTRINSIC_EVENT_GET(AccessPointUpdated);

        NATIVE_GET_STRING("FirmwareVersion", native_HNetworkWiFiGetFWVersion(m_nWifiDevIndex, &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(isConnectedAP, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(apScanning, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopApScanning, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setKey, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(applyConnectToAP, obj, "os", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(applyConnectToAPbyWPS, obj, "ns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getWaveStrength, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getConnectedAPInfo, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getUserAP, obj, "", JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int HMX_NetworkWiFiConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	INTRINSIC_EVENT(AccessPointUpdated);
	return JSP_PUT_NOTFOUND;
}

void HMX_NetworkWiFiConfig::releaseNative( HNetworkHandle id )
{
}

int	HMX_NetworkWiFiConfig::isConnectedAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int HMX_NetworkWiFiConfig::stopApScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	m_sScanningInstance = EXTRACT_OBJECT_OR_THROW( m_sScanningInstance, this_obj, HMX_NetworkWiFiConfig );
	RETURN_BOOLEAN(m_sScanningInstance->m_bStopScan = TRUE, JSP_CALL_VALUE);
}

int	HMX_NetworkWiFiConfig::apScanning(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;

	native_HNetworkWiFiConfigApScanning( m_nWifiDevIndex, &retval );

	m_sScanningInstance = EXTRACT_OBJECT_OR_THROW( m_sScanningInstance, this_obj, HMX_NetworkWiFiConfig );
	m_sScanningInstance->m_bStopScan = FALSE;

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
#define KEY_LEN_MAX 64
int	HMX_NetworkWiFiConfig::setKey(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFNumber retval = TRUE;
	if( NULL != m_szKey )
	{
		delete []m_szKey;
		m_szKey = NULL;
	}
	m_szKey	= new char[KEY_LEN_MAX];
	strncpy( m_szKey, STRING_VALUE(argv[0]), KEY_LEN_MAX-1 );

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HMX_NetworkWiFiConfig::applyConnectToAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	OOIFBoolean retval = TRUE;
	void * apIdentifier = NULL;
	const char * apKey = NULL;

	HMX_NetworkAccessPoint * ap = OBJECT_VALUE( argv[0], HMX_NetworkAccessPoint );

	if ( NULL != ap )
		apIdentifier =  ap->getIdentifier();
	else
	{
		retval = FALSE;
		RETURN_NUMBER(retval, JSP_CALL_VALUE);
	}

	apKey = STRING_VALUE(argv[1]);

	m_sConnectInstance = EXTRACT_OBJECT_OR_THROW( m_sConnectInstance, this_obj, HMX_NetworkWiFiConfig );

	native_HNetworkWiFiConfigApplyConnectToAP( m_nWifiDevIndex, apIdentifier, apKey, &retval );
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HMX_NetworkWiFiConfig::applyConnectToAPbyWPS(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	OOIFBoolean retval = TRUE;
	const char * pincode = NULL;
	int connectionType = -1;

	connectionType = NUMBER_VALUE( argv[0] );

	if ( connectionType < 0 )
	{
		retval = FALSE;
		RETURN_NUMBER(retval, JSP_CALL_VALUE);
	}

	pincode = STRING_VALUE(argv[1]);

	m_sConnectInstance = EXTRACT_OBJECT_OR_THROW( m_sConnectInstance, this_obj, HMX_NetworkWiFiConfig );

	native_HNetworkWiFiConfigApplyConnectToAPbyWPS( m_nWifiDevIndex, connectionType, pincode, &retval );
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}


int	HMX_NetworkWiFiConfig::getWaveStrength(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval = TRUE;


	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HMX_NetworkWiFiConfig::getConnectedAPInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	HMX_NetworkAccessPoint *apInfo = NULL;
	NetworkApHandle identifier = NULL;
	native_HNetworkWiFiGetConnectedApInfo( m_nWifiDevIndex, &identifier );
	if ( NULL == identifier )
	{
		RETURN_NULL(JSP_GET_VALUE);
	}

	apInfo = new HMX_NetworkAccessPoint(identifier);

	RETURN_OBJECT(this_obj, apInfo ,result, JSP_GET_VALUE);
}

int	HMX_NetworkWiFiConfig::getUserAP(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	HMX_NetworkAccessPoint *apInfo = NULL;
	NetworkApHandle identifier = NULL;
	native_HNetworkWiFiGetUserAp( m_nWifiDevIndex, &identifier );
	if ( NULL == identifier )
	{
		RETURN_NULL(JSP_GET_VALUE);
	}

	apInfo = new HMX_NetworkAccessPoint(identifier);

	RETURN_OBJECT(this_obj, apInfo ,result, JSP_GET_VALUE);
}
void HMX_NetworkWiFiConfig::callbackEventHandler( int event, HNetworkHandle id, int nDevCount)
{
	HMX_NetworkWiFiConfig *wifiConfig = NULL;
	HMX_NetworkWiFiConfig *targetInstance = NULL;
	HMX_NetworkAPScanningResultEvent *e = NULL;
	jsplugin_obj *context = NULL;

	switch ( event )
	{
		case eNET_WIFI_SCANNING_OUTPUT :
		case eNET_WIFI_SCANNING_END :
		case eNET_WIFI_SCANNING_FAIL :
			targetInstance = m_sScanningInstance;
			break;

		case eNET_WIFI_CONNECT_SUCCESS :
		case eNET_WIFI_CONNECT_FAIL :
		case eNET_WIFI_WPS_SUCCESS :
		case eNET_WIFI_WPS_FAIL :
		case eNET_WIFI_KEY_INVALID :
			targetInstance = m_sConnectInstance;
			break;

		default :
			return;
	}

	if ( NULL == targetInstance )
	{
		return ;
	}

	for ( list<HMX_NetworkWiFiConfig *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		wifiConfig = (HMX_NetworkWiFiConfig *)(*it);

		if ( targetInstance != wifiConfig )
		{
			continue;
		}
		if(TRUE == targetInstance->m_bStopScan)
		{
			OOIF_LOG_PRINT("[%s] stop ap-scanning !!!!! \n", __FUNCTION__);
			break;
		}
		context = wifiConfig->getHost();
		if ( NULL == context )
		{
			continue;
		}
		e = new HMX_NetworkAPScanningResultEvent(context, id, event);
		if ( NULL != e )
		{
			wifiConfig->dispatchEvent(e);
		}
	}
}

/***********************************************************************************
 *  HMX_NetworkAccessPoint Class
 ***********************************************************************************
 */

HMX_NetworkAccessPoint::HMX_NetworkAccessPoint( NetworkApHandle id ) : WrapperObject<HMX_NetworkAccessPoint>(id)
{
	classType = CLASS_NETWORKACCESSPOINT;
}

HMX_NetworkAccessPoint::HMX_NetworkAccessPoint( const HMX_NetworkAccessPoint &original) : WrapperObject<HMX_NetworkAccessPoint>(original.identifier)
{
	classType = CLASS_NETWORKACCESSPOINT;

}
HMX_NetworkAccessPoint::~HMX_NetworkAccessPoint()
{
	ENTRY;
}

int HMX_NetworkAccessPoint::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	NATIVE_GET_STRING("essid", native_HNetworkAPGetEssid(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("secure", native_HNetworkAPGetSecureType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("authen", native_HNetworkAPGetAuthenType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("encrypt", native_HNetworkAPGetEncryptType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("macaddr", native_HNetworkAPGetMacAddr(identifier, &RETVAL), JSP_GET_VALUE);

	/* read only */
	NATIVE_GET_NUMBER("waveStrength", native_HNetworkAPGetWaveStrength(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("connectSpeed", native_HNetworkAPGetConnectSpeed(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("isConnected", native_HNetworkAPGetIsConnected(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HMX_NetworkAccessPoint::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	NATIVE_SET("essid", native_HNetworkAPSetEssid(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("secure", native_HNetworkAPSetSecureType(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("authen", native_HNetworkAPSetAuthenType(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("encrypt", native_HNetworkAPSetEncryptType(identifier, NUMBER_VALUE(value[0])));
	return JSP_PUT_NOTFOUND;
}

void HMX_NetworkAccessPoint::releaseNative( NetworkApHandle id )
{
	NATIVE_WARN_NOK(native_HNetworkAccessPointReleaseHandle(id));
}

/***********************************************************************************
 *  HMX_NetworkLinkStatusEvent Class
 ***********************************************************************************
 */
HMX_NetworkLinkStatusEvent::HMX_NetworkLinkStatusEvent( jsplugin_obj *c, const char *eventName, int state ) : Event( c, eventName), m_nState( state )
{
	ENTRY;
}
HMX_NetworkLinkStatusEvent::~HMX_NetworkLinkStatusEvent()
{
	ENTRY;
}

int HMX_NetworkLinkStatusEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	return JSP_GET_NOTFOUND;
}

int HMX_NetworkLinkStatusEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int HMX_NetworkLinkStatusEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nState;

	return 1;
}
/***********************************************************************************
 *  HMX_NetworkAPScanningResultEvent Class
 ***********************************************************************************
 */
HMX_NetworkAPScanningResultEvent::HMX_NetworkAPScanningResultEvent( jsplugin_obj *c, NetworkApHandle handleId, int state ) : Event( c, "AccessPointUpdated"), ap(handleId), m_nState( state )
{
	ENTRY;
}

HMX_NetworkAPScanningResultEvent::~HMX_NetworkAPScanningResultEvent()
{
	ENTRY;
}

int HMX_NetworkAPScanningResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	return JSP_GET_NOTFOUND;
}

int HMX_NetworkAPScanningResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int HMX_NetworkAPScanningResultEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nState;

	SET_SERIALIZED_OBJECT(new HMX_NetworkAccessPoint(ap), 1);
	return 2;
}

#endif // OIPF
