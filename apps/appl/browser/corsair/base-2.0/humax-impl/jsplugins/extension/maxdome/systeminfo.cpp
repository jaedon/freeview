/*************************************************************************************************************
	File 		: systeminfo.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/11/07
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

#include "systeminfo.h"
#include "plugtype.h"
#include "macro.h"
#include "netutils.h"

#include "oplnetwork.h"
#include "opllocalsystem.h"

#include <cstdio>
#include <cstring>
#include <sys/sysinfo.h>

using namespace Maxdome;

/******************************************************************************************
 * SystemInfoObject
 ******************************************************************************************/
SystemInfoObject::SystemInfoObject()
{
	m_model_name[0] = '\0';
	m_serial_no[0] = '\0';
	m_secure_serial_no[0] = '\0';
	m_sw_version[0] = '\0';
	m_debug_info[0] = '\0';
}

SystemInfoObject::~SystemInfoObject()
{
}

int SystemInfoObject::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("productname", getProductName(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("serialnumber", getSerialNumber(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("secureSerialNumber", getSecureSerialNumber(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("firmwareversion", getFirmwareVersion(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("resolution", getResolution(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("network", getNetworkType(obj, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("vendor", getVenderName(obj, &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(debug_free, obj, "", JSP_GET_VALUE_CACHE);

	GET_FAIL;
}

int SystemInfoObject::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_GET_NOTFOUND;
}

int SystemInfoObject::constructor( jsplugin_obj *this_obj, jsplugin_obj *function_obj,
			int argc, jsplugin_value *argv, jsplugin_value *result )
{
	GLOBAL_RETURN_OBJECT(function_obj, new SystemInfoObject(), getter,
				destructor, result, JSP_CALL_VALUE);

	return JSP_CALL_ERROR;
}

int SystemInfoObject::destructor(jsplugin_obj *obj)
{
	SystemInfoObject *sys_obj = static_cast<SystemInfoObject *>(obj->plugin_private);
	if ( sys_obj ) delete sys_obj;

	obj->plugin_private = NULL;

	return JSP_DESTROY_OK;
}

PlugReturnType SystemInfoObject::getProductName(jsplugin_obj *obj, const char **retval)
{
	SystemInfoObject *sys_obj = static_cast<SystemInfoObject *>(obj->plugin_private);
	if ( !sys_obj ) return PLUG_RETURN_VALUE_NULL;

	if ( !OplLocalSystem::getModelName(sys_obj->m_model_name) )
		return PLUG_RETURN_VALUE_NULL;

	*retval = sys_obj->m_model_name;

	return PLUG_RETURN_OK;
}

PlugReturnType SystemInfoObject::getSerialNumber(jsplugin_obj *obj, const char **retval)
{
	SystemInfoObject *sys_obj = static_cast<SystemInfoObject *>(obj->plugin_private);
	if ( !sys_obj ) return PLUG_RETURN_VALUE_NULL;

	if ( !OplLocalSystem::getSerialNumber(sys_obj->m_serial_no) )
		return PLUG_RETURN_VALUE_NULL;

	*retval = sys_obj->m_serial_no;

	return PLUG_RETURN_OK;
}

PlugReturnType SystemInfoObject::getSecureSerialNumber(jsplugin_obj *obj, const char **retval)
{
	SystemInfoObject *sys_obj = static_cast<SystemInfoObject *>(obj->plugin_private);
	if ( !sys_obj ) return PLUG_RETURN_VALUE_NULL;

	char serial_no[1024] = {0,};
	if ( !OplLocalSystem::getSerialNumber(serial_no) )
		return PLUG_RETURN_VALUE_NULL;

	//

	*retval = sys_obj->m_secure_serial_no;

	return PLUG_RETURN_OK;
}

PlugReturnType SystemInfoObject::getFirmwareVersion(jsplugin_obj *obj, const char **retval)
{
	SystemInfoObject *sys_obj = static_cast<SystemInfoObject *>(obj->plugin_private);
	if ( !sys_obj ) return PLUG_RETURN_VALUE_NULL;

	char version[32] = {0,};
	if ( !OplLocalSystem::getSoftwareVersion(version) )
		return PLUG_RETURN_VALUE_NULL;

	memset(sys_obj->m_sw_version, 0, sizeof(sys_obj->m_sw_version));
	strncpy(sys_obj->m_sw_version, version+7, SW_VERSION_LEN-1);

	*retval = sys_obj->m_sw_version;

	return PLUG_RETURN_OK;
}

PlugReturnType SystemInfoObject::getResolution(jsplugin_obj *obj, double *retval)
{
	*retval = SYSINFO_RESOLUTION_720P;

	return PLUG_RETURN_OK;
}

PlugReturnType SystemInfoObject::getNetworkType(jsplugin_obj *obj, double *retval)
{
	OplNetworkInterface *netif = NetUtils::getActiveNetwork();
	if ( !netif ) return PLUG_RETURN_VALUE_UNDEFINED;

	*retval = netif->getNetworkType();
	delete netif;

	return PLUG_RETURN_OK;
}

PlugReturnType SystemInfoObject::getVenderName(jsplugin_obj *obj, const char **retval)
{
	SystemInfoObject *sys_obj = static_cast<SystemInfoObject *>(obj->plugin_private);
	if ( !sys_obj ) return PLUG_RETURN_VALUE_NULL;

	if ( !OplLocalSystem::getVendorName( sys_obj->m_vender_name ))
		return PLUG_RETURN_VALUE_NULL;

	*retval = sys_obj->m_vender_name;

	return PLUG_RETURN_OK;
}

int SystemInfoObject::debug_free(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
					jsplugin_value *argv, jsplugin_value *result)
{
	SystemInfoObject *sys_obj = static_cast<SystemInfoObject *>(this_obj->plugin_private);
	if ( !sys_obj ) return PLUG_RETURN_VALUE_UNDEFINED;

	struct sysinfo info;
	if ( sysinfo(&info) != 0 )
		return PLUG_RETURN_VALUE_UNDEFINED;

	snprintf(sys_obj->m_debug_info, sizeof(sys_obj->m_debug_info),
			"MemTotal : %d kB MemAlloc : %d kB\n", info.totalram/1000, info.freeram/1000);

	RETURN_STRING(sys_obj->m_debug_info, JSP_CALL_VALUE);

	return PLUG_RETURN_OK;
}
