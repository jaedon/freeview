/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSNETWORKINTERFACE__
#define __NATIVE_JLABSNETWORKINTERFACE__

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *jlabsNetworkInterfaceHandle;

typedef enum {
	JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CONNECT_FAIL= 0, 
	JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CONNECT_SUCCESS,
	JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CABLE_PLUGGED, 
	JLABS_NETWORK_INTERFACE_EVENT_LINK_STATUS_CABLE_UNPLUGGED,
} jlabsNetworkInterfaceEvent;
typedef void (jlabsNetworkInterfaceListener)(jlabsNetworkInterfaceHandle id, jlabsNetworkInterfaceEvent event);

typedef void (NetworkEventCallback)( int event, jlabsNetworkInterfaceHandle id, int nDevIndex );

OOIFReturnCode native_jlabsNetworkGetIndex(jlabsNetworkInterfaceHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsNetworkGetIpv6Type(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkInterfaceListener(jlabsNetworkInterfaceListener listener);
OOIFReturnCode native_jlabsNetworkGetIPAddress(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetMacAddress(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetInterfaceName(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetConnected(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkGetEnabled(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkSetEnabled(jlabsNetworkInterfaceHandle identifier, OOIFBoolean enabled);
OOIFReturnCode native_jlabsNetworkGetNetmask(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetGateway(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetDns1(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetDns2(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetDHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkGetDNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkGetInterfaceType(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkSetInterfaceType(jlabsNetworkInterfaceHandle identifier, const char *value);
OOIFReturnCode native_jlabsNetworkSetIPAddres(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetNetmask(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetGateway(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetDNS1(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetDNS2(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetDHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn );
OOIFReturnCode native_jlabsNetworkSetDNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn );

OOIFReturnCode native_jlabsNetworkSetIpv6Type(jlabsNetworkInterfaceHandle identifier, OOIFNumber value);
OOIFReturnCode native_jlabsNetworkGetIpv6Address(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetIpv6Prefix(jlabsNetworkInterfaceHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsNetworkGetIpv6Gateway(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetIpv6Dns1(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetIpv6Dns2(jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetIpv6DHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *value);
OOIFReturnCode native_jlabsNetworkGetIpv6DNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkSetIpv6Addres(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetIpv6Prefix(jlabsNetworkInterfaceHandle identifier, OOIFNumber value);
OOIFReturnCode native_jlabsNetworkSetIpv6Gateway(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetIpv6DNS1(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetIpv6DNS2(jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetIpv6DHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn );
OOIFReturnCode native_jlabsNetworkSetIpv6DNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn );
OOIFReturnCode native_jlabsNetworkSetIpv6StatelessMode(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn );
OOIFReturnCode native_jlabsNetworkGetIpv6StatelessMode(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *value);

OOIFReturnCode native_jlabsNetworkGetTunnelStartIpv4Address (jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkGetTunnelEndIpv4Address (jlabsNetworkInterfaceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkSetTunnelStartIpv4Address( jlabsNetworkInterfaceHandle identifier, const char * value);
OOIFReturnCode native_jlabsNetworkSetTunnelEndIpv4Address( jlabsNetworkInterfaceHandle identifier, const char * value);

OOIFReturnCode native_jlabsNetworkApplyConfigure(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkRegisterEventHandler(jlabsNetworkInterfaceHandle identifier, NetworkEventCallback cbFunc);
OOIFReturnCode native_jlabsNetworkUnRegisterEventHandler(jlabsNetworkInterfaceHandle identifier);
OOIFReturnCode native_jlabsNetworkRefreshAddressInfo(jlabsNetworkInterfaceHandle identifier);
OOIFReturnCode native_jlabsNetworkGet(jlabsNetworkInterfaceHandle identifier, const char *name, const char **retval);
OOIFReturnCode native_jlabsNetworkSet(jlabsNetworkInterfaceHandle identifier, const char *name, const char *value, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkRestart(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkIsDown(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkIsUp(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsNetworkReleaseHandle(jlabsNetworkInterfaceHandle identifier);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSNETWORKINTERFACE__ */
