/*************************************************************************************************************
	File 		: ipconfig.cpp
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

#include "ipconfig.h"
#include "macro.h"
#include "nettype.h"
#include "netutils.h"

#include "opllocalsystem.h"
#include "oplnetwork.h"

#include <string.h>

using namespace Maxdome;

/******************************************************************************************
 * IpConfig
 ******************************************************************************************/
eNetworkType IpConfig::s_net_type = NET_TYPE_LAN;

IpConfig::IpConfig(eNetworkType type)
{
	s_net_type = type;
}

IpConfig::~IpConfig()
{
}

int IpConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_BOOLEAN("dhcp", isDhcp(obj, &RETVAL), JSP_GET_VALUE);

	GET_FAIL;
}

int IpConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_GET_NOTFOUND;
}

int IpConfig::constructor( jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	return JSP_CALL_ERROR;
}

int IpConfig::destructor(jsplugin_obj *obj)
{
	IpConfig *conf_obj = static_cast<IpConfig *>(obj->plugin_private);
	if ( conf_obj ) delete conf_obj;

	obj->plugin_private = NULL;

	return JSP_DESTROY_OK;
}
	
PlugReturnType IpConfig::isDhcp(jsplugin_obj *obj, int *retval)
{
	OplNetworkInterface *netif = (s_net_type == NET_TYPE_LAN) ?
		NetUtils::getLanNetwork() : NetUtils::getWlanNetwork();
	if ( !netif )
	{
		*retval = 0;
		return PLUG_RETURN_OK;
	}

	*retval = netif->getDHCPStatus();
	delete netif;

	return PLUG_RETURN_OK;
}
