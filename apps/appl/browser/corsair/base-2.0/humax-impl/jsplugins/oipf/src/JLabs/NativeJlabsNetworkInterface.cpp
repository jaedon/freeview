/**************************************************************************************/
/**
 * @file NativeJlabsNetworkInterface.cpp
 *
 * jlabsDTTChanne module
 *
 * @author 
 * @date    
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "NativeJlabsNetworkInterface.h"
#include "macros.h"

#include "opllocalsystem.h"
#include "oplnetwork.h"
#include "oplsignalinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hlib.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


#ifdef OIPF
OOIFReturnCode native_jlabsNetworkGetIndex(jlabsNetworkInterfaceHandle identifier, OOIFNumber *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	int ret;
	ret  = networkinterfaceObj->getIndex();
	*retval = (OOIFNumber) ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkInterfaceListener(jlabsNetworkInterfaceListener listener)
{
    return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6Type(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getIpv6Type();
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsNetworkGetIPAddress(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpAddress();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetMacAddress(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getMacAddress();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetInterfaceName(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getInterfaceName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetConnected(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getConnected();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetEnabled(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getEnabled();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetNetmask(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getNetmask();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetGateway(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getGateway();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetDns1(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getDNS1();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetDns2(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getDNS2();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetDHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getDHCPStatus();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetDNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getDNSAutoStatus();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6StatelessMode(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getIpv6StatelessMode();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetInterfaceType(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	int nType = networkinterfaceObj->getNetworkType();
	switch(nType)
	{
		case 0 : *retval = "LAN"; break;
		case 1 : *retval = "WLAN"; break;
		case 2 : *retval = "CABLE"; break;
		case 3 : *retval = "WAN"; break;
		default :  *retval =  "LAN"; break;
	}
	//*retval = networkinterfaceObj->getInterfaceName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetInterfaceType(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
#if 0
	OOIFBoolean retval = networkinterfaceObj->setInterfaceName( value );
	if ( TRUE != retval )
	{
		return OOIF_RETURN_ERROR;
	}
#endif
	(void )(networkinterfaceObj);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetEnabled(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bEnabled)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = (OOIFBoolean)networkinterfaceObj->setEnabled(bEnabled);
	if ( TRUE != retval )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIPAddres(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpAddress( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetNetmask(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setNetmask( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetGateway(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setGateway( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetDNS1(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDNS1( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetDNS2(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDNS2( value );

	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetDHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDHCPStatus( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetDNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setDNSAutoStatus( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

#define ____Support_IPv6_start____

OOIFReturnCode native_jlabsNetworkGetIpv6Address(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6Address();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6Prefix(jlabsNetworkInterfaceHandle identifier, OOIFNumber *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);

	int ret;
	ret  = networkinterfaceObj->getIpv6Prefix();
	*retval = (OOIFNumber) ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6Gateway(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6Gateway();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6Dns1(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6DNS1();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6Dns2(jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6DNS2();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6DHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*value = (OOIFBoolean)networkinterfaceObj->getIpv6DhcpOn();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetIpv6DNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = (OOIFBoolean)networkinterfaceObj->getIpv6DnsAuto();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6Type(jlabsNetworkInterfaceHandle identifier, OOIFNumber value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6Type( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6Addres(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6Address( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6Prefix(jlabsNetworkInterfaceHandle identifier, OOIFNumber value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6Prefix( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6Gateway(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6Gateway( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6DNS1(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DNS1( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6DNS2(jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DNS2( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6DHCPStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DhcpOn( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6DNSAutoStatus(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6DnsSAuto( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetIpv6StatelessMode(jlabsNetworkInterfaceHandle identifier, OOIFBoolean bOn )
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6StatelessMode( bOn );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsNetworkGetTunnelStartIpv4Address (jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6TunnelStart();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGetTunnelEndIpv4Address (jlabsNetworkInterfaceHandle identifier, const char **retval)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getIpv6TunnelEnd();
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsNetworkSetTunnelStartIpv4Address( jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6TunnelStartAddr( value );
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkSetTunnelEndIpv4Address( jlabsNetworkInterfaceHandle identifier, const char * value)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OOIFBoolean retval = networkinterfaceObj->setIpv6TunnalEndAddr( value );
											  
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

#define ___Support_IPv6_end___

OOIFReturnCode native_jlabsNetworkRegisterEventHandler(jlabsNetworkInterfaceHandle identifier, NetworkEventCallback cbFunc)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OplNetworkInterface::registerEventListener( networkinterfaceObj, cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkUnRegisterEventHandler(jlabsNetworkInterfaceHandle identifier)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	OplNetworkInterface::unRegisterEventListener( networkinterfaceObj );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkRefreshAddressInfo(jlabsNetworkInterfaceHandle identifier)
{
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	networkinterfaceObj->refreshData();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkGet(jlabsNetworkInterfaceHandle identifier, const char * name, const char **retval)
{
	OOIFBoolean ret;
	__TRACE();
	__RETURN_ERROR_IF(name == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	ret = networkinterfaceObj->get((char *)name, (char **)retval);
	if ( TRUE != ret )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkSet(jlabsNetworkInterfaceHandle identifier, const char * name, const char * value, OOIFBoolean *retval)
{
	__TRACE();
	__RETURN_ERROR_IF(name == NULL);
	__RETURN_ERROR_IF(value == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	OOIFBoolean ret;
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	ret = networkinterfaceObj->set((char *)name, (char *)value);
	if ( ret != ERR_OK)
	{
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkRestart(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);

	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->applyConfigure();

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkIsDown(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	OOIFBoolean ret;
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);
	
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	ret = networkinterfaceObj->getConnected();
	if(ret == true)
		*retval = FALSE;
	else
		*retval = TRUE;

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkIsUp(jlabsNetworkInterfaceHandle identifier, OOIFBoolean *retval)
{
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);
	
	OplNetworkInterface *networkinterfaceObj = static_cast<OplNetworkInterface *>(identifier);
	*retval = networkinterfaceObj->getConnected();

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkReleaseHandle(jlabsNetworkInterfaceHandle identifier)
{
	return OOIF_RETURN_OK;
}
#endif
