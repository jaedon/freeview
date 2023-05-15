// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __MACROS
#define __MACROS

#include "ObjectInstance.h"
#include "EventTarget.h"
#include "VisualObject.h"
#include "NJSEUtils.h"
#include "NativeNJSETypes.h"
#include "Exception.h"
#include <stdlib.h>
#include <stdio.h>
#include "Log.h"

namespace Lovefilm
{
extern jsplugin_callbacks *g_opera_callbacks;

// used at entry point from jsplugin
#define ENTRY NJSE_LOG_DEBUG("=> Entering %s::%s \n", __FILE__, __FUNCTION__)

// This macro simply returns an object by calling create_object with
// the appropriate context. For use in getters, see GET_OBJECT

#define RETURN_OBJECT(context, instance, returnobj, returntype) \
	do { \
		ObjectInstance *theinst = instance; \
		jsplugin_obj *theobj; \
		int r = g_opera_callbacks->create_object(context, \
												 ObjectInstance::getter_proxy, \
												 ObjectInstance::setter_proxy, \
												 ObjectInstance::destructor_proxy, \
												 ObjectInstance::gc_proxy, \
												 &theobj); \
		if (r != JSP_CREATE_OK) { \
			delete theinst; \
			return JSP_GET_ERROR; \
		} else { \
			theobj->plugin_private = theinst; \
			theinst->setHost(theobj); \
			r = g_opera_callbacks->add_unload_listener(theobj, \
													   ObjectInstance::unload_proxy); \
			if (r != JSP_CALLBACK_OK) \
				NJSE_LOG_WARNING("Failed to add unload listener \n"); \
			returnobj->type = JSP_TYPE_OBJECT; \
			returnobj->u.object = theobj; \
			return returntype; \
		} \
	} while (0)

// Similarly, this macro returns a function. See also GET_FUNCTION

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

// This macro returns an already existing native object
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
			NJSE_LOG_WARNING("null value found when string was expected \n"); \
			RETURN_NULL(returntype); \
		} \
		result->type = JSP_TYPE_STRING; \
		result->u.string = str_value; \
		return returntype; \
	} while (0)

#define RETURN_STRING_WITH_LENGTH(value, length, returntype) \
	do { \
		const char *str_value = value; \
		int str_length = length; \
		if (!str_value) { \
			NJSE_LOG_WARNING("null value found when string was expected \n"); \
			RETURN_NULL(returntype); \
		} \
		result->type = JSP_TYPE_STRING_WITH_LENGTH; \
		result->u.s.string = str_value; \
		result->u.s.len = str_length; \
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

//NOTE: strings[i] will be deallocated in the collection's destructor
#define RETURN_STRING_COLLECTION(statement) \
	do { \
		StringCollection *collection = new StringCollection; \
		char **strings = NULL; \
		int count = statement; \
		for (int i=0; i<count; i++) { \
			collection->addItem(strings[i]); \
		} \
		HLIB_STD_MemFree(strings); \
		return collection; \
	} while (0)

// These functions should be used in getters, they simply incapsulate
// those above with an if statement

#define GET_OBJECT(identifier, context, instance, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_OBJECT(context, instance, result, returntype); \
	} while (0)

#define GET_FUNCTION(identifier, context, signature, returntype) \
	do { \
		if (strcmp(name, #identifier) == 0) \
			RETURN_FUNCTION(identifier, context, signature, returntype); \
	} while (0)

#define GET_RENAMED_FUNCTION(identifier, function, context, signature, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_FUNCTION(function, context, signature, returntype); \
	} while (0)

#define GET_NUMBER(identifier, value, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_NUMBER(value, returntype); \
	} while (0)

#define GET_BOOLEAN(identifier, value, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_BOOLEAN(value, returntype); \
	} while (0)

#define GET_STRING(identifier, value, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_STRING(value, returntype); \
	} while (0)

#define GET_STRING_WITH_LENGTH(identifier, value, length, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_STRING_WITH_LENGTH(value, length, returntype); \
	} while (0)

#define GET_NULL(identifier) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_NULL(JSP_GET_VALUE); \
	} while (0)

#define GET_UNDEFINED(identifier) \
	do { \
		if (strcmp(name, identifier) == 0) \
			RETURN_UNDEFINED(JSP_GET_VALUE); \
	} while (0)

#define GET_FAIL \
	do { \
		NJSE_LOG_WARNING("Failed to find the following variable: %s \n", name); \
		return JSP_GET_NOTFOUND; \
	} while (0)

// Helper functions specifically for classes with embedded data structs
#define DATA_GET_NUMBER_OR_UNDEFINED(identifier) \
	do { \
		if (strcmp(name, #identifier) == 0) { \
			if (!data_wrapper) { \
				RETURN_NULL(JSP_GET_VALUE_CACHE); \
			} \
			else if (data_wrapper->data.has_##identifier) { \
				RETURN_NUMBER(data_wrapper->data.identifier, JSP_GET_VALUE_CACHE); \
			} \
			else { \
				RETURN_UNDEFINED(JSP_GET_VALUE_CACHE); \
			} \
		} \
 	} while (0)

#define DATA_GET_STRING_OR_UNDEFINED(identifier) \
	do { \
		if (strcmp(name, #identifier) == 0) { \
			if (!data_wrapper) { \
				RETURN_NULL(JSP_GET_VALUE_CACHE); \
			} \
			else if (data_wrapper->data.has_##identifier) { \
				RETURN_STRING(data_wrapper->data.identifier, JSP_GET_VALUE_CACHE); \
			} \
			else { \
				RETURN_UNDEFINED(JSP_GET_VALUE_CACHE); \
			} \
		} \
	} while (0)

#define DATA_GET_NUMBER(identifier) \
	do { \
		if (!data_wrapper) \
			GET_NULL(#identifier); \
		else \
			GET_NUMBER(#identifier, data_wrapper->data.identifier, JSP_GET_VALUE_CACHE); \
	} while (0)

#define DATA_GET_RENAMED_STRING(identifier, renamed_identifier) \
	do { \
		if (!data_wrapper) \
			GET_NULL(#identifier); \
		else \
			GET_STRING(#identifier, data_wrapper->data.renamed_identifier, JSP_GET_VALUE_CACHE); \
	} while (0)

#define DATA_GET_STRING(identifier) \
	DATA_GET_RENAMED_STRING(identifier, identifier)

#define DATA_GET_STRING_WITH_LENGTH(identifier, length) \
	do { \
		if (!data_wrapper) \
			GET_NULL(#identifier); \
		else \
			GET_STRING_WITH_LENGTH(#identifier, data_wrapper->data.identifier, data_wrapper->data.length, JSP_GET_VALUE_CACHE); \
	} while (0)

#define DATA_GET_BOOLEAN(identifier) \
	do { \
		if (!data_wrapper) \
			GET_NULL(#identifier); \
		else \
			GET_BOOLEAN(#identifier, data_wrapper->data.identifier, JSP_GET_VALUE_CACHE); \
	} while (0)

#define DATA_FREE_STRING(identifier) \
	do { \
		if (data.identifier) \
			HLIB_STD_MemFree(data.identifier); \
	} while (0)

// Exceptions!

#define THROW_EXCEPTION(context, name, message, result, returntype) \
	RETURN_OBJECT(context, new Exception(name, message), result, returntype)

// These are setters

// SET_WITH_ACTION is a setter which does not actually set anything,
// but executes a statement instead. Mostly useful for setters which
// wrap down to native functionality

// FIXME: Break down in SET_STRING_WITH ... etc to verify type of
// value set? Or maybe use macro below

#define SET_WITH_ACTION(identifier, action) \
	do { \
		if (strcmp(name, identifier) == 0) \
		{ \
			action; \
			return JSP_PUT_OK; \
		} \
	} while (0)

// For serializing events mostly, set result[x] to an object with the given ObjectInstance

#define SET_SERIALIZED_OBJECT(instance, index) \
	do { \
		jsplugin_obj *theobj; \
		ObjectInstance *theinst = instance; \
		int r = g_opera_callbacks->create_object(context, \
												 ObjectInstance::getter_proxy, \
												 ObjectInstance::setter_proxy, \
												 ObjectInstance::destructor_proxy,\
												 ObjectInstance::gc_proxy, \
												 &theobj); \
		if (r != JSP_CREATE_OK) { \
			delete[] *argv; \
			delete theinst; \
			*argv = NULL; \
			return 0; \
		} else { \
			theobj->plugin_private = theinst; \
			theinst->setHost(theobj); \
			r = g_opera_callbacks->add_unload_listener(theobj, ObjectInstance::unload_proxy); \
			if (r != JSP_CALLBACK_OK) \
				NJSE_LOG_WARNING("Failed to add unload listener \n"); \
			result[index].type = JSP_TYPE_OBJECT; \
			result[index].u.object = theobj; \
		} \
	} while (0)

// Parameters
#define CHECK_ARGUMENT_COUNT(count) CHECK_ARGUMENT_COUNT_MIN_MAX(count, count)

#define CHECK_ARGUMENT_COUNT_MIN_MAX(min, max) \
	do { \
		if (argc < min) { \
			NJSE_LOG_WARNING("EE Invalid argument count: %d instead of %d \n", argc, min); \
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		} \
		else if (argc > max) { \
			NJSE_LOG_WARNING("Invalid argument count: %d instead of %d \n", argc, max); \
		} \
	} while (0)


// Checking arguments shouldn't really be necessary, since core
// converts them, but it's useful to find errors in prototype
// declarations in the plugin

#define CHECK_ARGUMENT(index, wantedtype) \
	do { \
		if (argv[index].type != wantedtype) { \
			NJSE_LOG_WARNING("EE Invalid parameter:, in parameter %d \n", index); \
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		} \
	} while (0)

// Intrinsic listener handling
#define _INTRINSIC_EVENT(event) \
	do { \
		if (value->type == JSP_TYPE_NATIVE_OBJECT) \
			_NATIVE_SET(internalAddEventListener(event, value, true)); \
		else if (value->type == JSP_TYPE_NULL) \
			_NATIVE_SET(removeIntrinsicListener(event)); \
		else \
			NJSE_LOG_WARNING("Invalid listener received when trying to set an intrinsic listener for %s \n", event); \
	} while (0)

#define INTRINSIC_EVENT(event) \
	do { \
		if (!strcmp(name, "on"#event)) \
			_INTRINSIC_EVENT(#event); \
	} while (0)

#define INTRINSIC_EVENT_ATTR(event) \
	do { \
		if (!strcasecmp(name, "on"#event)) { \
			jsplugin_value jsp_value; \
			jsp_value.type = JSP_TYPE_STRING; \
			jsp_value.u.string = value; \
			internalAddEventListener(#event, &jsp_value, true); \
		} \
	} while (0)

#define INTRINSIC_EVENT_GET(event) \
	do { \
		if (!strcmp(name, "on"#event)) { \
			jsplugin_obj *pluginobj; \
			NATIVE_GET( findIntrinsicListener(#event, &pluginobj)); \
			RETURN_NATIVE_OBJ(pluginobj, JSP_GET_VALUE); \
		} \
	} while (0)

// Create new objects when application is parsed
#define HANDLE_OBJECT(mime, instance, returntype) \
	do { \
		if (!strcasecmp(type, mime)) \
			HANDLE_OBJECT_CREATE(instance, returntype); \
	} while (0)

#define HANDLE_OBJECT_CREATE(instance, returntype) \
	do { \
		ObjectInstance *theinst = instance; \
		if (!theinst) \
			return JSP_OBJECT_NOMEM; \
		object_obj->plugin_private = theinst; \
		*getter = ObjectInstance::getter_proxy; \
		*setter = ObjectInstance::setter_proxy; \
		*destructor = ObjectInstance::destructor_proxy; \
		*trace = ObjectInstance::gc_proxy; \
		int r = g_opera_callbacks->add_unload_listener(object_obj, \
													   ObjectInstance::unload_proxy); \
		if (r != JSP_CALLBACK_OK) \
			NJSE_LOG_WARNING("Failed to add unload listener \n"); \
		r = g_opera_callbacks->set_attr_change_listener(object_obj, ObjectInstance::attr_change_proxy); \
		if (r != JSP_CALLBACK_OK) \
			NJSE_LOG_WARNING("Failed to add attr_change listener \n"); \
		r = g_opera_callbacks->set_param_set_listener(object_obj, ObjectInstance::param_set_proxy); \
		if (r != JSP_CALLBACK_OK) \
			NJSE_LOG_WARNING("Failed to add param_set listener \n"); \
		theinst->setHost(object_obj); \
		theinst->setAttrs(attrs_count, attrs); \
		return returntype; \
	} while (0)

//Help macros for jsplugin_value* argv handling

#define TYPE_NUMBER(arg) \
	(arg.type == JSP_TYPE_NUMBER && !NJSEUtils::isNaN(arg.u.number))

#define TYPE_STRING(arg) \
	(arg.type == JSP_TYPE_STRING)

#define TYPE_BOOLEAN(arg) \
	(arg.type == JSP_TYPE_BOOLEAN)

#define NUMBER_VALUE(arg) \
	arg.u.number

#define STRING_VALUE(arg) \
	arg.u.string

#define STRING_LENGTH(arg) \
	arg.u.s.len

#define BOOLEAN_VALUE(arg) \
	arg.u.boolean

#define OBJECT_VALUE_OR_THROW(var_name, arg, obj_type) \
	OBJECT_VALUE(arg,obj_type); \
	do { \
		if (!var_name) \
		{ \
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		} \
	} while (0)

#define OBJECT_VALUE(arg, object_type) \
	( \
	(!&(arg) ||((arg).type != JSP_TYPE_OBJECT)) ? \
		NULL : \
		EXTRACT_OBJECT<object_type>((arg).u.object) \
	)

#define EXTRACT_OBJECT_OR_THROW(var_name, arg, obj_type) \
	EXTRACT_OBJECT<obj_type>(arg); \
	do { \
		if (!var_name) \
		{ \
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		} \
	} while (0)

template <class T>
inline T * EXTRACT_OBJECT(jsplugin_obj * arg)
{
	if (!arg)
		return NULL;
	ObjectInstance * o = (ObjectInstance*)(arg->plugin_private);
	if ( !o || o->getType() != T::class_name)
		return NULL;
	return static_cast<T*>(o);
}
template <>
inline ObjectInstance * EXTRACT_OBJECT<ObjectInstance>(jsplugin_obj * arg)
{
	if (!arg)
		return NULL;
	return (ObjectInstance*)(arg->plugin_private);
}

template <>
inline EventTarget * EXTRACT_OBJECT<EventTarget>(jsplugin_obj * arg)
{
	if (!arg)
		return NULL;
	ObjectInstance *o= (ObjectInstance*)(arg->plugin_private);
	if (!o)
		return NULL;
	if (o->getType() < CLASSTYPE_EVENTTARGET)
	{
		return NULL;
	}
	return static_cast<EventTarget*>(o);
}

template <>
inline VisualObject * EXTRACT_OBJECT<VisualObject>(jsplugin_obj * arg)
{
	if (!arg)
		return NULL;
	ObjectInstance *o= (ObjectInstance*)(arg->plugin_private);
	if (!o)
		return NULL;
	if (o->getType() < CLASSTYPE_VISUAL)
	{
		return NULL;
	}
	return static_cast<VisualObject*>(o);
}

// Declares a variable VAR of type CTYPE and copies the value of the
// NUM'th argument into it, but only if it matches TYPE, where TYPE
// is the 'XXX' part of the TYPE_XXX macros, e.g. STRING for strings
// and NUMBER for numbers (from TYPE_STRING and TYPE_NUMBER). If it
// fails, it throws an exception.
#define GET_ARGUMENT_OR_THROW(NUM, TYPE, CTYPE, VAR) \
	CTYPE VAR; \
	do { \
		if (argc > NUM && TYPE_##TYPE(argv[NUM])) \
			VAR = TYPE##_VALUE(argv[NUM]); \
		else \
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
	} while (0)

// Same as GET_ARGUMENT_OR_THROW, but with special handling for the
// special values null and undefined (along with the argument not having
// been passed at all). These cases are remapped to a specified default
// value instead. Also declares a boolean variable has_VAR indicating
// whether the value was read from the argument (true) or if the default
// value was used (false).
#define GET_OPTIONAL_ARGUMENT_OR_THROW(NUM, TYPE, CTYPE, VAR, DEFAULT) \
	CTYPE VAR; \
	bool has_##VAR; \
	do { \
		if (argc <= NUM || argv[NUM].type == JSP_TYPE_UNDEFINED || argv[NUM].type == JSP_TYPE_NULL) { \
			VAR = (DEFAULT); \
			has_##VAR = false; \
		} else if (TYPE_##TYPE(argv[NUM])) { \
			VAR = TYPE##_VALUE(argv[NUM]); \
			has_##VAR = true; \
		} else { \
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		} \
	} while (0)



// Help macros for calling native methods
// internal
#define THROW_OOM_ERR(context, result, returntype) \
	THROW_EXCEPTION(context, "Error", "OutOfMemoryError", result, returntype)

#define THROW_GENERIC_ERR(context, result, returntype) \
	THROW_EXCEPTION(context, "Error", "Error", result, returntype)

#define THROW_TYPE_ERR(context, result, returntype) \
	THROW_EXCEPTION(context, "TypeError", "TypeError", result, returntype)

#define THROW_SECURITY_ERR(context, result, returntype) \
	THROW_EXCEPTION(context, "SecurityError", "SecurityError", result, returntype)

#define THROW_WRONG_ARG_ERR( context, result, returntype) \
	THROW_EXCEPTION(context, "Error", "WRONG_ARGUMENTS_ERR", result, returntype)

// for getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
#define NATIVE_GET(native_getter) \
	do { \
		NJSEReturnCode ret = native_getter; \
		switch (ret) \
		{ \
		case NJSE_RETURN_OK: \
			break;	/*handle in calling macro */ \
		case NJSE_RETURN_VALUE_NULL: \
			RETURN_NULL(JSP_GET_VALUE); \
		case NJSE_RETURN_VALUE_UNDEFINED: \
			RETURN_UNDEFINED(JSP_GET_VALUE); \
		case NJSE_RETURN_ERROR: \
			THROW_GENERIC_ERR(obj, result, JSP_GET_EXCEPTION); \
		case NJSE_RETURN_OOM_ERROR: \
			THROW_OOM_ERR(obj, result, JSP_GET_EXCEPTION); \
		case NJSE_RETURN_TYPE_ERROR: \
			THROW_TYPE_ERR(obj, result, JSP_GET_EXCEPTION); \
		case NJSE_RETURN_SECURITY_ERROR: \
			THROW_SECURITY_ERR(obj, result, JSP_GET_EXCEPTION); \
		case NJSE_RETURN_NOT_SUPPORTED: \
			return JSP_GET_NOTFOUND; \
		default: \
			NJSE_LOG_ERROR("Error in return type from native function \n"); \
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
			NJSENumber RETVAL; \
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

/* Note that variables SIZE and NATIVE_ARRAY are to be used in native call */
#define _GET_COLLECTION(context, native_getter, collection_creater, returntype) \
		int SIZE = 0; \
		NATIVE_GET(native_getter); \
		RETURN_OBJECT(context, collection_creater, result, returntype); \

#define NATIVE_GET_VOIDP_COLLECTION(identifier, context, native_getter, collection_creater, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) { \
			void **NATIVE_ARRAY; \
			_GET_COLLECTION(context, native_getter, collection_creater, returntype); \
		} \
	} while (0)

#define NATIVE_GET_NUMBERS_COLLECTION(identifier, context, native_getter, collection_creater, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) { \
			NJSENumber *NATIVE_ARRAY; \
			_GET_COLLECTION(context, native_getter, collection_creater, returntype) \
		} \
	} while (0)

#define NATIVE_GET_STRING_COLLECTION(identifier, context, native_getter, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) { \
			char **NATIVE_ARRAY; \
			_GET_COLLECTION(context, native_getter, \
							NJSEUtils::CreateStringCollection(SIZE, NATIVE_ARRAY), \
							returntype); \
		} \
	} while (0)

#define NATIVE_GET_STRING_COLLECTION2(identifier, context, native_getter, collection_creater, returntype) \
	do { \
		if (strcmp(name, identifier) == 0) { \
			char **NATIVE_ARRAY; \
			_GET_COLLECTION(context, native_getter, \
							collection_creater, \
							returntype); \
		} \
	} while (0)

#define _NATIVE_SET(ret) \
	do { \
		switch (ret) \
		{ \
		case NJSE_RETURN_OK: \
			return JSP_PUT_OK; \
		case NJSE_RETURN_ERROR: \
			THROW_GENERIC_ERR(obj, value, JSP_PUT_EXCEPTION); \
		case NJSE_RETURN_OOM_ERROR: \
			THROW_OOM_ERR(obj, value, JSP_PUT_EXCEPTION); \
		case NJSE_RETURN_TYPE_ERROR: \
			THROW_TYPE_ERR(obj, value, JSP_PUT_EXCEPTION); \
		case NJSE_RETURN_SECURITY_ERROR: \
			THROW_SECURITY_ERR(obj, value, JSP_PUT_EXCEPTION); \
		case NJSE_RETURN_NOT_SUPPORTED: \
			return JSP_PUT_NOTFOUND; \
		default: \
			NJSE_LOG_ERROR("Unexpected return type received from native function \n"); \
			return JSP_PUT_NOTFOUND; \
		} \
	} while (0)

// for setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
#define NATIVE_SET(id, native_setter) \
	do { \
		if (strcmp(name, id) == 0) { \
			NJSEReturnCode ret = native_setter; \
			_NATIVE_SET(ret); \
		} \
	} while (0)

#define SET_READONLY(id) \
	do { \
		if (strcmp(name, id) == 0) { \
			return JSP_PUT_READONLY; \
		} \
	} while (0)

#define SET_NUMBER(identifier, var, returntype) \
	do { \
		if (strcmp(name, identifier) == 0 && value->type == JSP_TYPE_NUMBER) {\
			var = value->u.number;\
			return returntype; \
		}\
	} while (0)

#define SET_BOOLEAN(identifier, var, returntype) \
	do { \
		if (strcmp(name, identifier) == 0 && value->type == JSP_TYPE_BOOLEAN) {\
			var = value->u.boolean;\
			return returntype; \
		}\
	} while (0)


// for callers: (jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
#define _NATIVE_CALL(ret) \
		switch (ret) \
		{ \
		case NJSE_RETURN_OK: \
			break;	/*handle in calling method */ \
		case NJSE_RETURN_VALUE_NULL: \
			RETURN_NULL(JSP_CALL_VALUE); \
		case NJSE_RETURN_VALUE_UNDEFINED: \
			return JSP_CALL_NO_VALUE; \
		case NJSE_RETURN_ERROR: \
			THROW_GENERIC_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		case NJSE_RETURN_OOM_ERROR: \
			THROW_OOM_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		case NJSE_RETURN_TYPE_ERROR: \
			THROW_TYPE_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		case NJSE_RETURN_SECURITY_ERROR: \
			THROW_SECURITY_ERR(this_obj, result, JSP_CALL_EXCEPTION); \
		case NJSE_RETURN_NOT_SUPPORTED: \
			return JSP_CALL_ERROR; \
		default: \
			NJSE_LOG_ERROR("Unexpected return type received from native function \n"); \
			return JSP_CALL_ERROR; \
		}

#define NATIVE_CALL(native_caller) \
	do { \
		NJSEReturnCode ret = native_caller; \
		_NATIVE_CALL(ret); \
	} while (0)

// for handle_object:
// Note that no errors can be thrown to HTML parser, failure will mean
// that a generic DOM object will be created instead
#define NATIVE_HANDLE_OBJECT(mime, createfunc, returntype) \
	do { \
		if (strcasecmp(type, mime) == 0) { \
			ObjectInstance *RETVAL = NULL; \
			NJSEReturnCode ret = createfunc; \
			switch (ret) \
			{ \
			case NJSE_RETURN_OK: \
				HANDLE_OBJECT_CREATE(RETVAL, returntype); \
				break; \
			default: \
				return JSP_OBJECT_ERROR; \
			} \
		} \
	} while (0)

/* Note that variables WINDOW and HOST can be used in native call when obj is set */
#define NATIVE_CALL_HANDLE_OBJ(native_caller, instance_creator, obj) \
	do { \
		long int WINDOW = 0; \
		char *HOST = NULL; \
		if (obj) \
		{ \
			WINDOW = NJSEUtils::getWindow(obj); \
			HOST = NJSEUtils::getHost(obj); \
		} \
		NJSEReturnCode ret = native_caller; \
		if (ret == NJSE_RETURN_OK) \
		{ \
			*result = instance_creator; \
			if (!*result) \
				ret = NJSE_RETURN_OOM_ERROR; \
		} \
		else { \
			NJSE_LOG_WARNING("non-OK return value from native call to create object (return code = %d) \n", ret); \
		} \
		return ret; \
	} while (0)

#define NATIVE_WARN_NOK(native_func) \
	do { \
		if ((native_func) != NJSE_RETURN_OK) \
			NJSE_LOG_WARNING("Non-OK return type from native function when OK was expected \n"); \
	} while (0)


/** Note that variable OBJ is to be used in creator call */
#define CREATE_OBJECT(creator) \
	do { \
		ObjectInstance *OBJ; \
		NATIVE_CALL(creator); \
		RETURN_OBJECT(this_obj, OBJ, result, JSP_CALL_VALUE); \
	} while (0)


} /* namespace Lovefilm */

#endif // __MACROS
