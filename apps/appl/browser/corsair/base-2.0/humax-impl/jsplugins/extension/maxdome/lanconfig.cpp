/*************************************************************************************************************
	File 		: lanconfig.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/11/08
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

#include "lanconfig.h"
#include "ipconfig.h"
#include "netstate.h"
#include "macro.h"
#include "netutils.h"

#include "opllocalsystem.h"
#include "oplnetwork.h"

#include <string.h>

using namespace Maxdome;



/******************************************************************************************
 * LanConfigObject
 ******************************************************************************************/
LanConfigObject::LanConfigObject()
{
	m_macaddr[0] = '\0';
}

LanConfigObject::~LanConfigObject()
{
}

int LanConfigObject::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("macaddress", getMacAddress(obj, &RETVAL), JSP_GET_VALUE);
	GET_OBJECT("ipconfig", obj, new IpConfig(NET_TYPE_LAN), IpConfig::getter,
			IpConfig::destructor, result, JSP_GET_VALUE);
	GET_OBJECT("netstate", obj, new NetState(NET_TYPE_LAN), NetState::getter,
			NetState::destructor, result, JSP_GET_VALUE);

	GET_FAIL;
}

int LanConfigObject::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_GET_NOTFOUND;
}

int LanConfigObject::constructor( jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	GLOBAL_RETURN_OBJECT(function_obj, new LanConfigObject(), getter,
				destructor, result, JSP_CALL_VALUE);

	return JSP_CALL_ERROR;
}

int LanConfigObject::destructor(jsplugin_obj *obj)
{
	LanConfigObject *lan_obj = static_cast<LanConfigObject *>(obj->plugin_private);
	if ( lan_obj ) delete lan_obj;

	obj->plugin_private = NULL;

	return JSP_DESTROY_OK;
}

PlugReturnType LanConfigObject::getMacAddress(jsplugin_obj *obj, const char **retval)
{
	LanConfigObject *lan_obj = static_cast<LanConfigObject *>(obj->plugin_private);
	if ( !lan_obj ) return PLUG_RETURN_VALUE_NULL;

	OplNetworkInterface *netif = NetUtils::getLanNetwork();
	if ( !netif ) return PLUG_RETURN_VALUE_NULL;

	memset(lan_obj->m_macaddr, 0, sizeof(lan_obj->m_macaddr));
	strncpy(lan_obj->m_macaddr, netif->getMacAddress(), MAC_ADDR_LEN-1);
	delete netif;

	*retval = lan_obj->m_macaddr;

	return PLUG_RETURN_OK;
}
