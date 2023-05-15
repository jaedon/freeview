/*************************************************************************************************************
	File 		: macro.h
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

#ifndef __MACRO_H__
#define __MACRO_H__

#include <stdio.h>
#include "libplugin.h"

namespace Maxdome
{

extern jsplugin_callbacks *g_opera_callbacks;

#define ENTRY \
	PLUGIN_Print("=> Entering: name=%s\n", name);

#define GET_FAIL \
	PLUGIN_Print("Failed to find the following variable: %s\n", name); \
	return JSP_GET_NOTFOUND;


#define GLOBAL_RETURN_OBJECT(context, instance, getter, destructor, returnobj, returntype) \
	do { \
		if ( !instance ) \
			return JSP_CALL_NOMEM; \
		\
		jsplugin_obj *theobj; \
		int r = g_opera_callbacks->create_object(context, \
												 getter, \
												 NULL, \
												 destructor, \
												 NULL, \
												 &theobj); \
		if (r != JSP_CREATE_OK) { \
			delete instance; \
			return JSP_GET_ERROR; \
		} else { \
			theobj->plugin_private = instance; \
			returnobj->type = JSP_TYPE_OBJECT; \
			returnobj->u.object = theobj; \
			return returntype; \
		} \
	} while (0)

#define GLOBAL_CREATE_OBJECT(context, instance_creater, returnobj, returntype) \
	do { \
		jsplugin_obj *theobj; \
		int r = g_opera_callbacks->create_function(context, \
												   NULL, NULL, NULL, \
												   instance_creater, \
												   "", \
												   NULL, \
												   NULL, \
												   &theobj); \
		if (r != JSP_CREATE_OK) { \
			return JSP_GET_ERROR; \
		} \
		else \
		{ \
			returnobj->u.object = theobj; \
			returnobj->type = JSP_TYPE_OBJECT; \
			return returntype; \
		} \
	} while (0)

#define GLOBAL_GET_OBJECT(identifier, context, instance_creater, returnobj, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) { \
			GLOBAL_CREATE_OBJECT(context, instance_creater, returnobj, returntype);\
		} \
	} while (0)

#define RETURN_OBJECT(context, instance, getter, desturtor, returnobj, returntype) \
	do { \
		if ( !instance ) \
			return JSP_CALL_NOMEM; \
		\
		jsplugin_obj *theobj; \
		int r = g_opera_callbacks->create_object(context, \
												 getter, \
												 NULL, \
												 destructor, \
												 NULL, \
												 &theobj); \
		if (r != JSP_CREATE_OK) { \
			delete instance; \
			return JSP_GET_ERROR; \
		} else { \
			theobj->plugin_private = instance; \
			returnobj->type = JSP_TYPE_OBJECT; \
			returnobj->u.object = theobj; \
			return returntype; \
		} \
	} while (0)

#define RETURN_FUNCTION(objname, context, signature, returntype) \
	do { \
		jsplugin_obj *theobj; \
		int r = g_opera_callbacks->create_function(context, \
												   NULL, NULL, \
												   objname, \
												   NULL, \
												   signature, \
												   NULL, \
												   NULL, \
												   &theobj); \
		if (r != JSP_CREATE_OK) { \
			return JSP_GET_ERROR; \
		} \
		else \
		{ \
			result->u.object = theobj; \
			result->type = JSP_TYPE_OBJECT; \
			return returntype; \
		} \
	} while (0)

#define RETURN_NATIVE_OBJ(obj, returntype) \
	do { \
		result->type = JSP_TYPE_NATIVE_OBJECT; \
		result->u.object = obj; \
		return returntype; \
	} while (0)


#define RETURN_BOOLEAN(value, returntype) \
	do { \
		result->type = JSP_TYPE_BOOLEAN; \
		result->u.boolean = value; \
		return returntype; \
	} while (0)

#define RETURN_NUMBER(value, returntype) \
	do { \
		result->type = JSP_TYPE_NUMBER; \
		result->u.number = value; \
		return returntype; \
	} while (0)

#define RETURN_STRING(value, returntype) \
	do { \
		const char *str_value = value; \
		if (!str_value) { \
			RETURN_NULL(returntype); \
		} \
		result->type = JSP_TYPE_STRING; \
		result->u.string = str_value; \
		return returntype; \
	} while (0)

#define RETURN_NULL(returntype) \
	do { \
		result->type = JSP_TYPE_NULL; \
		return returntype; \
	} while (0)

#define RETURN_UNDEFINED(returntype) \
	do { \
		result->type = JSP_TYPE_UNDEFINED; \
		return returntype; \
	} while (0)

#define NATIVE_GET(native_getter) \
	do { \
		int ret = native_getter; \
		switch (ret) \
		{ \
		case PLUG_RETURN_OK: \
			break;	/*handle in calling macro */ \
		case PLUG_RETURN_VALUE_NULL: \
			RETURN_NULL(JSP_GET_VALUE); \
		case PLUG_RETURN_VALUE_UNDEFINED: \
			RETURN_UNDEFINED(JSP_GET_VALUE); \
		case PLUG_RETURN_NOT_SUPPORTED: \
		default: \
			return JSP_GET_NOTFOUND; \
		} \
	} while (0)

/** Note that variable RETVAL is to be used in native call */
#define NATIVE_GET_BOOLEAN(id, native_getter, returntype) \
	do { \
		if (strcmp(name, id) == 0) { \
			int RETVAL; \
			NATIVE_GET(native_getter); \
			RETURN_BOOLEAN(RETVAL, returntype); \
		} \
	} while (0)

#define NATIVE_GET_ENUM(id, enum_type, native_getter, returntype) \
	do { \
		if (strcmp(name, id) == 0) { \
			enum_type RETVAL; \
			NATIVE_GET(native_getter); \
			RETURN_NUMBER(RETVAL, returntype); \
		} \
	} while (0)

#define NATIVE_GET_NUMBER(id, native_getter, returntype) \
	do { \
		if (strcmp(name, id) == 0) { \
			double RETVAL; \
			NATIVE_GET(native_getter); \
			RETURN_NUMBER(RETVAL, returntype); \
		} \
	} while (0)

#define NATIVE_GET_STRING(id, native_getter, returntype) \
	do { \
		if (strcmp(name, id) == 0) { \
			const char* RETVAL; \
			NATIVE_GET(native_getter); \
			RETURN_STRING(RETVAL, returntype); \
		} \
	} while (0)

#define NATIVE_GET_OBJECT(identifier, context, native_getter, instance_creater, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) { \
			void *RETVAL; \
			NATIVE_GET(native_getter); \
			RETURN_OBJECT(context, instance_creater, result, returntype); \
		} \
	} while (0)

#define GET_OBJECT(identifier, context, instance, getter, destructor, returnobj, returntype)   \
	do { \
		if (strcmp(name, identifier) == 0)                                  \
			RETURN_OBJECT(context, instance, getter, destrutor, returnobj, returntype);       \
	} while (0)

#define GET_FUNCTION(identifier, context, signature, returntype) \
	do { \
		if (strcmp(name, #identifier) == 0) \
			RETURN_FUNCTION(identifier, context, signature, returntype); \
	} while (0)

}	/* namespace */

#endif

