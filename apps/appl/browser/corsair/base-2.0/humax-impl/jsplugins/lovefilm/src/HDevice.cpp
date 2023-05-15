/*************************************************************************************************************
	File 		: HDevice.cpp
	author 		: STB-Component (tstbcomp@humaxdigital.com)
	comment		:
	date    	: 2013/10/21
	attention 	:

	(c) 2011-2013 Humax Co., Ltd.
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "HDevice.h"
#include "NativeHDevice.h"

#include "Player.h"

using namespace Lovefilm;

#define RELEASE_HANDLE(x)	\
		if(NULL != x){		\
			delete x;		\
			x = NULL;		\
		}					\

/*******************************************************************/
/************    		HDevice   *********************/
/*******************************************************************/
HDevice *HDevice::g_pDeviceInstance;
DeviceEventError HDevice::m_error = NULL;
HDevice::HDevice() : EventTarget()
{
	ENTRY;
	classType = CLASS_LOVEFILM_DEVICE;
	g_pDeviceInstance = this;
	native_Device_RegEventListener(HDevice::callbackEventHandler);
}

HDevice::~HDevice()
{
	ENTRY;
	g_pDeviceInstance = NULL;
	native_Device_UnRegEventListener();
}

int HDevice::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(setTime, 		obj, "n", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setMemoryWarningLevel, 	obj, "n", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(gc, 			obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(exit, 			obj, "", 	JSP_GET_VALUE_CACHE);

	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	NATIVE_GET_OBJECT("player", obj, native_PlayerCreate(&RETVAL), new Player(RETVAL), JSP_GET_VALUE);
	//@@TT:KTH Case A
	//NATIVE_GET_OBJECT("lastError", obj, natvie_Device_Get_LastError(&RETVAL), new HError(RETVAL), JSP_GET_VALUE);
	//@@TT:KTH Case B
    GET_OBJECT("lastError",		obj, new HError(m_error), JSP_GET_VALUE); 

    NATIVE_GET_STRING("interfaceVersion",	native_Device_Get_InterfaceVersion(&RETVAL), 	JSP_GET_VALUE); 
    NATIVE_GET_STRING("firmwareVersion",	native_Device_Get_FirmwareVersion(&RETVAL), 	JSP_GET_VALUE); 
    NATIVE_GET_STRING("operatingSystem",	native_Device_Get_OperatingSystem(&RETVAL), 	JSP_GET_VALUE); 
    NATIVE_GET_STRING("manufacturer",		native_Device_Get_Manufacturer(&RETVAL), 		JSP_GET_VALUE); 
    NATIVE_GET_STRING("modelName",			native_Device_Get_ModelName(&RETVAL), 			JSP_GET_VALUE); 
    NATIVE_GET_STRING("chipset",			native_Device_Get_Chipset(&RETVAL), 			JSP_GET_VALUE); 
    NATIVE_GET_STRING("uuid",				native_Device_Get_Uuid(&RETVAL), 				JSP_GET_VALUE); 
    NATIVE_GET_STRING("clientIp",			native_Device_Get_ClientIp(&RETVAL), 			JSP_GET_VALUE); 
    NATIVE_GET_STRING("clientTime",			native_Device_Get_ClientTime(&RETVAL), 			JSP_GET_VALUE); 
    NATIVE_GET_STRING("connectionType",		native_Device_Get_ConnectionType(&RETVAL), 		JSP_GET_VALUE); 
    NATIVE_GET_NUMBER("connectionStrength", native_Device_Get_ConnectionStrength(&RETVAL), 			JSP_GET_VALUE); 
     
	NATIVE_GET_STRING_COLLECTION("audioCapability", 
									   obj, native_Device_Get_AudioCapability((NJSENumber*)&SIZE, &NATIVE_ARRAY), JSP_GET_VALUE);
									   
    NATIVE_GET_NUMBER("cpuLoad",			native_Device_Get_CpuLoad(&RETVAL), 			JSP_GET_VALUE); 
    NATIVE_GET_NUMBER("memoryTotal",		native_Device_Get_MemoryTotal(&RETVAL), 		JSP_GET_VALUE); 
    NATIVE_GET_NUMBER("memoryUsed",			native_Device_Get_MemoryUsed(&RETVAL), 			JSP_GET_VALUE); 
    NATIVE_GET_NUMBER("diskStorageTotal",	native_Device_Get_DiskStorageTotal(&RETVAL), 	JSP_GET_VALUE); 
    NATIVE_GET_NUMBER("diskStorageUsed",	native_Device_Get_DiskStorageUsed(&RETVAL), 	JSP_GET_VALUE); 
    NATIVE_GET_NUMBER("screenWidth",		native_Device_Get_ScreenWidth(&RETVAL), 		JSP_GET_VALUE); 
    NATIVE_GET_NUMBER("screenHeight",		native_Device_Get_ScreenHeight(&RETVAL), 		JSP_GET_VALUE); 


	INTRINSIC_EVENT_GET(connection_changed);
	INTRINSIC_EVENT_GET(error);
	INTRINSIC_EVENT_GET(focus_changed);
	INTRINSIC_EVENT_GET(app_will_exit);
	INTRINSIC_EVENT_GET(memory_warning);

	return JSP_GET_NOTFOUND;
}

int HDevice::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	SET_READONLY("interfaceVersion");
	SET_READONLY("firmwareVersion");
	SET_READONLY("operatingSystem");
	SET_READONLY("manufacturer");
	SET_READONLY("modelName");
	SET_READONLY("chipset");
	SET_READONLY("uuid");
	SET_READONLY("clientIp");
	SET_READONLY("clientTime");
	SET_READONLY("connectionType");
	SET_READONLY("connectionStrength");
	SET_READONLY("cpuLoad");
	SET_READONLY("memoryTotal");
	SET_READONLY("memoryUsed");
	SET_READONLY("diskStorageTotal");
	SET_READONLY("diskStorageUsed");
	SET_READONLY("screenWidth");
	SET_READONLY("screenHeight");

	INTRINSIC_EVENT(connection_changed);
	INTRINSIC_EVENT(error);
	INTRINSIC_EVENT(focus_changed);
	INTRINSIC_EVENT(app_will_exit);
	INTRINSIC_EVENT(memory_warning);

	return JSP_PUT_NOTFOUND;
}

int	HDevice::setTime(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	NJSEBoolean retval = TRUE;
	CHECK_ARGUMENT_COUNT(1);
    CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

    NATIVE_CALL(native_Device_SetTime(NUMBER_VALUE(argv[0]), &retval));
	
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HDevice::setMemoryWarningLevel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	NJSEBoolean retval = TRUE;
	CHECK_ARGUMENT_COUNT(1);
    CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	native_Device_SetMemoryWarningLevel(NUMBER_VALUE(argv[0]), &retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HDevice::gc(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	NJSEBoolean retval 	= TRUE;
	
	native_Device_Gc(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HDevice::exit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	NJSEBoolean retval 	= TRUE;

	native_Device_Exit(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

void HDevice::callbackEventHandler( unsigned int event, DeviceEventHandle data)
{
	ENTRY;

	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HDevice *pDeviceInstance = g_pDeviceInstance;

	if ( NULL == pDeviceInstance )
	{
		NJSE_LOG_DEBUG("HDevice has Null instance !!!! \n");
		return ;
	}

	context = pDeviceInstance->getHost();
	NJSE_LOG_DEBUG("callbackEventHandler - context(%p), pDeviceInstance(%p) \n", context, pDeviceInstance);

	if(context){
		char event_name[32] = {0,}; 
		switch(event)
		{
			case eNATIVE_DEVICE_CONN_CHANGED: 
				strcpy(event_name, "connection_changed");
				break;
			case eNATIVE_DEVICE_ERROR:		  
				strcpy(event_name, "error");
				m_error = (DeviceEventError)data;
				break;
			case eNATIVE_DEVICE_FOCUS_CHANGED:
				strcpy(event_name, "focus_changed");
				break;
			case eNATIVE_DEVICE_APPWILL_EXIT: 
				strcpy(event_name, "app_will_exit");
				break;
			case eNATIVE_DEVICE_MEM_WARNING:  
				strcpy(event_name, "memory_warning");
				break;
			default:
				return;
		}
		e = new HDeviceResultEvent(context, event_name, event, data);					
		if( NULL != e)
		{
			pDeviceInstance->dispatchEvent(e);
		}
	}

}

/*******************************************************************/
/************  		HDeviceResultEvent   *********************/
/*******************************************************************/
HDeviceResultEvent::HDeviceResultEvent( jsplugin_obj *c, const char *eventName, int event, DeviceEventHandle data) : Event( c, eventName ),
	m_connection_type(NULL), m_category(NULL), m_type(NULL),
	m_module(NULL), m_code(NULL), m_description(NULL), 
	m_stack_trace(NULL), m_hasfocus(false), m_appname(NULL), m_available_memory(0)
	
{
	ENTRY;
	m_event 	= event;
	m_data 		= data;
	
	switch(m_event)
	{
       	case eNATIVE_DEVICE_CONN_CHANGED: 
			native_Device_Get_ConnectionType(&m_connection_type);
       		break;
    	case eNATIVE_DEVICE_ERROR:  
			native_Device_GetCategory(m_data, &m_category); 	
			native_Device_GetType(m_data, &m_type); 		
			native_Device_GetModule(m_data, &m_module);
			native_Device_GetCode(m_data, &m_code); 		
			native_Device_GetDescription(m_data, &m_description);
			native_Device_GetStackTrace(m_data, &m_stack_trace);
    		break;
       	case eNATIVE_DEVICE_FOCUS_CHANGED:
       		native_Device_GetHasFocus(m_data, &m_hasfocus);
			native_Device_GetAppName(m_data, &m_appname);
       		break;
       	case eNATIVE_DEVICE_APPWILL_EXIT: 
       		break;
       	case eNATIVE_DEVICE_MEM_WARNING:  
			native_Device_Get_MemoryAvailable(&m_available_memory);
       		break;
		default:
			break;
	}
	NJSE_LOG_DEBUG("HDeviceResultEvent name(%s), event(%d) \n", eventName, event );
}

HDeviceResultEvent::~HDeviceResultEvent()
{
	ENTRY;
}

int HDeviceResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("event",	m_event, JSP_GET_VALUE);
	switch(m_event)
	{
       	case eNATIVE_DEVICE_CONN_CHANGED: 
       		// string connection_type
			GET_STRING("connectionType",	m_connection_type,	JSP_GET_VALUE); 
       		break;
    	case eNATIVE_DEVICE_ERROR:  
    		// category, type, module, code, description, stackTrace
			GET_STRING( "category",		m_category, 	JSP_GET_VALUE);
			GET_STRING( "type",			m_type, 		JSP_GET_VALUE);
			GET_STRING( "module",		m_module,		JSP_GET_VALUE);
			GET_STRING( "code",			m_code, 		JSP_GET_VALUE);
			GET_STRING( "description",	m_description,	JSP_GET_VALUE);
			GET_STRING( "stackTrace",	m_stack_trace,	JSP_GET_VALUE);
    		break;
       	case eNATIVE_DEVICE_FOCUS_CHANGED:
       		// hasFocus, appName
       		GET_BOOLEAN("hasFocus", 	m_hasfocus, 	JSP_GET_VALUE);
			GET_STRING( "appName",		m_appname, 		JSP_GET_VALUE);
       		break;
       	case eNATIVE_DEVICE_APPWILL_EXIT: 
       		break;
       	case eNATIVE_DEVICE_MEM_WARNING:  
       		// memory_available
			GET_NUMBER("memoryAvailable", m_available_memory,	JSP_GET_VALUE); 
       		break;
		default:
			break;
	}
	return JSP_GET_NOTFOUND;
}

int HDeviceResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	
	SET_READONLY("connectionType");
	SET_READONLY("category");
	SET_READONLY("type");
	SET_READONLY("module");
	SET_READONLY("code");
	SET_READONLY("description");
	SET_READONLY("stackTrace");
	SET_READONLY("hasFocus");
	SET_READONLY("appName");
	SET_READONLY("memoryAvailable");

	return JSP_PUT_NOTFOUND;
}

int HDeviceResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;

	switch(m_event)
	{
       	case eNATIVE_DEVICE_CONN_CHANGED: 
	       	RETURN_PARAM_COUNT = 1;
			*argv = new jsplugin_value[RETURN_PARAM_COUNT];
			result = *argv;
			result[0].type = JSP_TYPE_STRING;
			result[0].u.string = m_connection_type;
       		break;
    	case eNATIVE_DEVICE_ERROR: 
	    	RETURN_PARAM_COUNT = 6;
			*argv = new jsplugin_value[RETURN_PARAM_COUNT];
			result = *argv;
			result[0].type = JSP_TYPE_STRING;
			result[0].u.string = m_category;
			result[1].type = JSP_TYPE_STRING;
			result[1].u.string = m_type;
			result[2].type = JSP_TYPE_STRING;
			result[2].u.string = m_module;
			result[3].type = JSP_TYPE_STRING;
			result[3].u.string = m_code;
			result[4].type = JSP_TYPE_STRING;
			result[4].u.string = m_description;
			result[5].type = JSP_TYPE_STRING;
			result[5].u.string = m_stack_trace;
    		break;
       	case eNATIVE_DEVICE_FOCUS_CHANGED:
       		RETURN_PARAM_COUNT = 2;
			*argv = new jsplugin_value[RETURN_PARAM_COUNT];
			result = *argv;
			result[0].type = JSP_TYPE_BOOLEAN;
			result[0].u.boolean = m_hasfocus;
			result[1].type = JSP_TYPE_STRING;
			result[1].u.string = m_appname;
       		break;
       	case eNATIVE_DEVICE_APPWILL_EXIT: 
       		break;
       	case eNATIVE_DEVICE_MEM_WARNING:  
	       	RETURN_PARAM_COUNT = 1;
			*argv = new jsplugin_value[RETURN_PARAM_COUNT];
			result = *argv;
			result[0].type = JSP_TYPE_NUMBER;
			result[0].u.number = m_available_memory;
	       	break;
		default:
			break;
	}
	return RETURN_PARAM_COUNT;
}

/*************************************************************/
/************  			HError 		  *********************/
/*************************************************************/

HError::HError(DeviceEventError id) : WrapperObject<HError>(id)
{
	ENTRY;
	classType = CLASS_DEVICE_ERROR;

}
HError::HError(const HError &origin) : WrapperObject<HError>(origin.identifier)
{
	ENTRY;
	classType = CLASS_DEVICE_ERROR;

}
HError::~HError()
{
	ENTRY;

}

int HError::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING( "category", 		native_Device_GetCategory(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( "type", 			native_Device_GetType(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( "module", 		native_Device_GetModule(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( "code", 			native_Device_GetCode(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( "description", 	native_Device_GetDescription(identifier, &RETVAL),	JSP_GET_VALUE);
	NATIVE_GET_STRING( "stackTrace", 	native_Device_GetStackTrace(identifier, &RETVAL), 	JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HError::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	
	SET_READONLY("category");
	SET_READONLY("type");
	SET_READONLY("module");
	SET_READONLY("code");
	SET_READONLY("description");
	SET_READONLY("stackTrace");

	return JSP_PUT_NOTFOUND;
}

void HError::releaseNative( DeviceEventError id)
{
	ENTRY;
	if(NULL != id)
	{
		//NATIVE_WARN_NOK(native_Device_ReleaseErrorHandle(id));
	}
}

