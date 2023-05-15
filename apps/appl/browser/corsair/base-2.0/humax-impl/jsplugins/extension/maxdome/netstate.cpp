/*************************************************************************************************************
	File 		: netstate.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/11/09
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#include "netstate.h"
#include "macro.h"
#include "netutils.h"

#include "opllocalsystem.h"
#include "oplnetwork.h"

#include <string.h>

using namespace Maxdome;

/******************************************************************************************
 * NetState
 ******************************************************************************************/
eNetworkType NetState::s_net_type = NET_TYPE_LAN;

NetState::NetState(eNetworkType type)
{
	s_net_type = type;

	m_ipaddr[0] = '\0';
	m_netmask[0] = '\0';
	m_gateway[0] = '\0';
	m_dns1[0] = '\0';
	m_dns2[0] = '\0';
}

NetState::~NetState()
{
}

int NetState::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_BOOLEAN("is_up", isUp(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("is_connected", isConnected(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("has_ip", hasIp(obj, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING("ipaddress", getIpAddr(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("netmask", getNetMask(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("gateway", getGateway(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("dns1", getDns1(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("dns2", getDns2(obj, &RETVAL), JSP_GET_VALUE);

	GET_FAIL;
}

int NetState::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_GET_NOTFOUND;
}

int NetState::constructor( jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	return JSP_CALL_ERROR;
}

int NetState::destructor(jsplugin_obj *obj)
{
	NetState *state_obj = static_cast<NetState *>(obj->plugin_private);
	if ( state_obj ) delete state_obj;

	obj->plugin_private = NULL;

	return JSP_DESTROY_OK;
}

PlugReturnType NetState::isUp(jsplugin_obj *obj, int *retval)
{
	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif )
	{
		*retval = 0;
		return PLUG_RETURN_OK;
	}

	*retval = netif->getConnected();
	delete netif;

	return PLUG_RETURN_OK;
}

PlugReturnType NetState::isConnected(jsplugin_obj *obj, int *retval)
{
	return isUp(obj, retval);
}

PlugReturnType NetState::hasIp(jsplugin_obj *obj, int *retval)
{
	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif )
	{
		*retval = 0;
		return PLUG_RETURN_OK;
	}

	*retval = netif->getEnabled();
	delete netif;

	return PLUG_RETURN_OK;
}

PlugReturnType NetState::getIpAddr(jsplugin_obj *obj, const char **retval)
{
	NetState *state_obj = static_cast<NetState *>(obj->plugin_private);
	if ( !state_obj ) return PLUG_RETURN_VALUE_NULL;

	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif ) return PLUG_RETURN_VALUE_NULL;

	memset(state_obj->m_ipaddr, 0, sizeof(state_obj->m_ipaddr));
	strncpy(state_obj->m_ipaddr, netif->getIpAddress(), IPADDR_LEN-1);
	delete netif;

	*retval = state_obj->m_ipaddr;
	
	return PLUG_RETURN_OK;
}

PlugReturnType NetState::getNetMask(jsplugin_obj *obj, const char **retval)
{
	NetState *state_obj = static_cast<NetState *>(obj->plugin_private);
	if ( !state_obj ) return PLUG_RETURN_VALUE_NULL;

	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif ) return PLUG_RETURN_VALUE_NULL;

	memset(state_obj->m_netmask, 0, sizeof(state_obj->m_netmask));
	strncpy(state_obj->m_netmask, netif->getNetmask(), IPADDR_LEN-1);
	delete netif;

	*retval = state_obj->m_netmask;
	
	return PLUG_RETURN_OK;
}

PlugReturnType NetState::getGateway(jsplugin_obj *obj, const char **retval)
{
	NetState *state_obj = static_cast<NetState *>(obj->plugin_private);
	if ( !state_obj ) return PLUG_RETURN_VALUE_NULL;

	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif ) return PLUG_RETURN_VALUE_NULL;

	memset(state_obj->m_gateway, 0, sizeof(state_obj->m_gateway));
	strncpy(state_obj->m_gateway, netif->getGateway(), IPADDR_LEN-1);
	delete netif;

	*retval = state_obj->m_gateway;
	
	return PLUG_RETURN_OK;
}

PlugReturnType NetState::getDns1(jsplugin_obj *obj, const char **retval)
{
	NetState *state_obj = static_cast<NetState *>(obj->plugin_private);
	if ( !state_obj ) return PLUG_RETURN_VALUE_NULL;

	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif ) return PLUG_RETURN_VALUE_NULL;

	memset(state_obj->m_dns1, 0, sizeof(state_obj->m_dns1));
	strncpy(state_obj->m_dns1, netif->getDNS1(), IPADDR_LEN-1);
	delete netif;

	*retval = state_obj->m_dns1;
	
	return PLUG_RETURN_OK;
}

PlugReturnType NetState::getDns2(jsplugin_obj *obj, const char **retval)
{
	NetState *state_obj = static_cast<NetState *>(obj->plugin_private);
	if ( !state_obj ) return PLUG_RETURN_VALUE_NULL;

	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif ) return PLUG_RETURN_VALUE_NULL;

	memset(state_obj->m_dns2, 0, sizeof(state_obj->m_dns2));
	strncpy(state_obj->m_dns2, netif->getDNS2(), IPADDR_LEN-1);
	delete netif;

	*retval = state_obj->m_dns2;
	
	return PLUG_RETURN_OK;
}
