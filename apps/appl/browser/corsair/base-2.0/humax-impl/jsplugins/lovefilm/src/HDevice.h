/*************************************************************************************************************
	File 		: HDevice.h
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


#ifndef __HDEVICE_H__
#define __HDEVICE_H__

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"
#include "NativeHDevice.h"

namespace Lovefilm 
{

	class HDevice : public EventTarget
	{
	private:
		static HDevice *g_pDeviceInstance;
		static DeviceEventError m_error;
	public:
		HDevice(); 
		virtual ~HDevice();	
		
		static const ClassType class_name = CLASS_LOVEFILM_DEVICE;
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		
		static int	setTime(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);		
		static int	setMemoryWarningLevel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		
		static int	gc(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);	
		static int	exit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		
		static void callbackEventHandler(unsigned int event, void *data);
		
	};

	class HDeviceResultEvent : public Event
	{
	private :
		int 	m_event;
		DeviceEventHandle 	m_data;
		const char *m_connection_type;
		const char *m_category;
		const char *m_type;
		const char *m_module;
		const char *m_code;
		const char *m_description;
		const char *m_stack_trace;
		NJSEBoolean m_hasfocus;
		const char *m_appname;
		NJSENumber m_available_memory;
		
	public :
		HDeviceResultEvent( jsplugin_obj *c, const char *eventName, int event, DeviceEventHandle data );
		virtual ~HDeviceResultEvent();	
	
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};	
	
	class HError : public ObjectInstance, public WrapperObject<HError> 
	{
	private:

	public:
		HError(DeviceEventError id);
		HError(const HError &origin);
		virtual ~HError();
		
		static const ClassType class_name = CLASS_DEVICE_ERROR; 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		
		static void releaseNative( DeviceEventError id);
	};
	

} /* namespace Lovefilm */

#endif // __HDEVICE_H__
