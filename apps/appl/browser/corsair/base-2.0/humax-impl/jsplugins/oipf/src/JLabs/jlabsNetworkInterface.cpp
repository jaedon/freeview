#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsNetworkInterface.h"
#include "NativeJlabsNetworkInterface.h"
#include "jlabsNetworkInterfaceEventResult.h"

using namespace Ooif;

list<jlabsNetworkInterface*> jlabsNetworkInterface::instances;
jlabsNetworkInterfaceHandle jlabsNetworkInterface::m_sApplyIdentifier = NULL;

jlabsNetworkInterface::jlabsNetworkInterface(jlabsNetworkInterfaceHandle i) : EventTarget(), WrapperObject<jlabsNetworkInterface>(i)
{
	ENTRY;
	classType = CLASS_JLABS_NETWORK_INTERFACE;
}
jlabsNetworkInterface::jlabsNetworkInterface(const jlabsNetworkInterface &original) : EventTarget(), WrapperObject<jlabsNetworkInterface>(original.identifier)
{
	ENTRY;
	classType = CLASS_JLABS_NETWORK_INTERFACE;
}
jlabsNetworkInterface::~jlabsNetworkInterface()
{
	instances.remove(this);
	if ( 0 == instances.size() )
	{
		native_jlabsNetworkUnRegisterEventHandler( identifier );
	}
}

int jlabsNetworkInterface::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_BOOLEAN("ipv6Type", 					native_jlabsNetworkGetIpv6Type			(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("macAddress", 				native_jlabsNetworkGetMacAddress		(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("name", 						native_jlabsNetworkGetInterfaceName		(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("enable", 					native_jlabsNetworkGetEnabled			(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("dhcpEnable", 				native_jlabsNetworkGetDHCPStatus		(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("interfaceType", 			native_jlabsNetworkGetInterfaceType		(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("ipv4Address",				native_jlabsNetworkGetIPAddress			(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("subnetMask", 				native_jlabsNetworkGetNetmask			(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("defaultGateway", 			native_jlabsNetworkGetGateway			(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("primaryDNSServerAddress", 	native_jlabsNetworkGetDns1				(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("secondaryDNSServerAddress",	native_jlabsNetworkGetDns2				(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING ("ipv6Address", 			   	native_jlabsNetworkGetIpv6Address		(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER ("prefixLength",     			native_jlabsNetworkGetIpv6Prefix		(identifier, &RETVAL), JSP_GET_VALUE);

	/* humax ext. */
	NATIVE_GET_STRING ("ipv6defaultGateway",    		native_jlabsNetworkGetIpv6Gateway		(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("ipv6primaryDNSServerAddress",	native_jlabsNetworkGetIpv6Dns1			(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING ("ipv6secondaryDNSServerAddress", native_jlabsNetworkGetIpv6Dns2			(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("ipv6dhcpEnable", 			    native_jlabsNetworkGetIpv6DHCPStatus 	(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("ipv6dnsAuto", 	 				native_jlabsNetworkGetIpv6DNSAutoStatus(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("dnsAuto",						native_jlabsNetworkGetDNSAutoStatus		(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("StatelessIpv6Enable",			native_jlabsNetworkGetIpv6StatelessMode	(identifier, &RETVAL), JSP_GET_VALUE);

	/* Not implemented yet */
	NATIVE_GET_STRING("tunnelStartIpv4Address",	 native_jlabsNetworkGetTunnelStartIpv4Address(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("tunnelEndIpv4Address",	 native_jlabsNetworkGetTunnelEndIpv4Address(identifier, &RETVAL), JSP_GET_VALUE);
	
	INTRINSIC_EVENT_GET(LinkStatusChanged);
	INTRINSIC_EVENT_GET(linkStatusChanged);

	GET_FUNCTION(get, 		obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(set, 		obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(restart,	obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isDown, 	obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isUp, 		obj, "", JSP_GET_VALUE_CACHE);

	//GET_FUNCTION(addEventListener, 		obj, "sos", JSP_GET_VALUE_CACHE);
	//GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsNetworkInterface::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("ipv6Type", 				native_jlabsNetworkSetIpv6Type			(identifier, BOOLEAN_VALUE(value[0])));

	NATIVE_SET("interfaceType", 			native_jlabsNetworkSetInterfaceType				(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("tunnelStartIpv4Address",	native_jlabsNetworkSetTunnelStartIpv4Address	(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("tunnelEndIpv4Address", 		native_jlabsNetworkSetTunnelEndIpv4Address    	(identifier, STRING_VALUE(value[0])));

	NATIVE_SET("enable", 					native_jlabsNetworkSetEnabled				(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("ipv4Address", 				native_jlabsNetworkSetIPAddres				(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("subnetMask", 				native_jlabsNetworkSetNetmask				(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("defaultGateway", 			native_jlabsNetworkSetGateway				(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("primaryDNSServerAddress", 	native_jlabsNetworkSetDNS1					(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("secondaryDNSServerAddress",	native_jlabsNetworkSetDNS2					(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("dhcpEnable", 				native_jlabsNetworkSetDHCPStatus			(identifier, BOOLEAN_VALUE(value[0])));

	NATIVE_SET("ipv6Address", 				native_jlabsNetworkSetIpv6Addres			(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("prefixLength", 				native_jlabsNetworkSetIpv6Prefix			(identifier, NUMBER_VALUE(value[0])));

	/* humax ext. */
	NATIVE_SET("ipv6defaultGateway", 			native_jlabsNetworkSetIpv6Gateway 		(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("ipv6primaryDNSServerAddress",  	native_jlabsNetworkSetIpv6DNS1			(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("ipv6secondaryDNSServerAddress",	native_jlabsNetworkSetIpv6DNS2			(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("ipv6dhcpEnable",  				native_jlabsNetworkSetIpv6DHCPStatus	(identifier, BOOLEAN_VALUE(value[0])));

	NATIVE_SET("dnsAuto",						native_jlabsNetworkSetDNSAutoStatus		(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("ipv6dnsAuto", 					native_jlabsNetworkSetIpv6DNSAutoStatus	(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("StatelessIpv6Enable",			native_jlabsNetworkSetIpv6StatelessMode	(identifier, BOOLEAN_VALUE(value[0])));

	if ( !strcmp(name, "onLinkStatusChanged" ) || !strcmp(name, "onlinkStatusChanged" ) )
	{
		if ( 0 == instances.size() )
		{
			native_jlabsNetworkRegisterEventHandler( identifier, jlabsNetworkInterface::callbackEventHandler );
		}
		instances.push_back(this);
		INTRINSIC_EVENT(LinkStatusChanged);
		INTRINSIC_EVENT(linkStatusChanged);
	}
	return JSP_PUT_NOTFOUND;
}

void jlabsNetworkInterface::releaseNative(jlabsNetworkInterfaceHandle id)
{
	NATIVE_WARN_NOK(native_jlabsNetworkReleaseHandle(id));
}

OOIFReturnCode jlabsNetworkInterface::checkListenerAccessAllowed(const char *eventName)
{
#ifdef HBBTV_SECURITY
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("LinkStatusChanged", TRUE);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("linkStatusChanged", TRUE);
#endif // HBBTV_SECURITY
	return OOIF_RETURN_OK;
}
jlabsNetworkInterface * jlabsNetworkInterface::GetInstanceById( jlabsNetworkInterfaceHandle id )
{
	for ( list<jlabsNetworkInterface *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		if ( (*it)->getIdentifier() == id )
			return *it;
	}
	return NULL;
}

void jlabsNetworkInterface::callbackEventHandler( int event, jlabsNetworkInterfaceHandle id, int nDevIndex)
{
	jlabsNetworkInterface *netInterface = NULL;
	jsplugin_obj *context  = NULL;
	Event *e = NULL;

	OOIFNumber	nInsanceIndex = 0; 
	jlabsNetworkInterfaceHandle hInstanceId = NULL;
	for ( list<jlabsNetworkInterface *>::iterator it = instances.begin(); it != instances.end(); it++ )
	{
		hInstanceId = (*it)->getIdentifier();
		OOIF_LOG_DEBUG("\n(*) [%s,%d] Network Event(%d) - TargetIndex(%d), instance count(%d)\n", __FUNCTION__,__LINE__, event, nDevIndex, instances.size() );
		native_jlabsNetworkGetIndex( hInstanceId, &nInsanceIndex);

		OOIF_LOG_DEBUG("\n(*) [%s,%d] NetworkDevice(%ld), Event - TargetIndex(%d)\n", __FUNCTION__,__LINE__, nInsanceIndex, nDevIndex );
		if ( nDevIndex == nInsanceIndex )
		{
			netInterface = (*it);
			if( NULL == netInterface )
			{
				continue;
			}

			context = netInterface->getHost();
			if( NULL == context )
			{
				continue;
			}

			switch (event) {
				case JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CONNECT_FAIL:
					e = new jlabsNetworkInterfaceEventResult(context, "NetworkConnectFail");
					break;
				case JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CONNECT_SUCCESS:
					e = new jlabsNetworkInterfaceEventResult(context, "NetworkConnectSuccess");
					break;
				case JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CABLE_PLUGGED:
					e = new jlabsNetworkInterfaceEventResult(context, "NetworkCableUnplugged");
					break;
				case JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CABLE_UNPLUGGED:
					e = new jlabsNetworkInterfaceEventResult(context, "NetworkCablePlugged");
					break;
				default:
					OOIF_LOG_WARNING("Unknown event received");
					break;
			}

			if(e != NULL)
			{
				netInterface->dispatchEvent(e);
			}
		}
	}
}

int jlabsNetworkInterface::restart(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	OOIFBoolean retval;

	jlabsNetworkInterface *instance = EXTRACT_OBJECT_OR_THROW( instance, this_obj, jlabsNetworkInterface );
	m_sApplyIdentifier = instance->getIdentifier();
	NATIVE_CALL( native_jlabsNetworkRestart(instance->getIdentifier(), &retval) );
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int jlabsNetworkInterface::get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	
	jlabsNetworkInterface	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkInterface);
	const char				*retval;

	NATIVE_CALL(native_jlabsNetworkGet(instance->getIdentifier(), STRING_VALUE(argv[0]), &retval));

	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int jlabsNetworkInterface::set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_STRING);

	jlabsNetworkInterface	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkInterface);
	OOIFBoolean				retval;
	
	NATIVE_CALL(native_jlabsNetworkSet(instance->getIdentifier(), STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));

	return OOIF_RETURN_OK;
}

int jlabsNetworkInterface::isDown(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	
	jlabsNetworkInterface	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkInterface);
	OOIFBoolean				retval;
	
	NATIVE_CALL(native_jlabsNetworkIsDown(instance->getIdentifier(), &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
int jlabsNetworkInterface::isUp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	
	jlabsNetworkInterface	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsNetworkInterface);
	OOIFBoolean				retval;
	
	NATIVE_CALL(native_jlabsNetworkIsUp(instance->getIdentifier(), &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

