// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Capabilities.h"

#include <stdio.h>
#include <string.h>

#include "jsplugin.h"

#include "Exception.h"
#include "macros.h"

using namespace Ooif;

int Capabilities::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif

	GET_FUNCTION(hasCapability, obj, "s", JSP_GET_VALUE_CACHE);

	if (strcmp("xmlCapabilities", name) == 0)
	{
		return getXmlCapabilities(obj, name, result);
	}

	NATIVE_GET_NUMBER("extraSDVideoDecodes",
	                  native_CapabilityGetExtraSDVideoDecodes(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("extraHDVideoDecodes",
	                  native_CapabilityGetExtraHDVideoDecodes(&RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int Capabilities::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	SET_READONLY("xmlCapabilities");
	SET_READONLY("extraSDVideoDecodes");
	SET_READONLY("extraHDVideoDecodes");

	return JSP_PUT_NOTFOUND;
}

int Capabilities::hasCapability(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	// Verify calling arguments
	CHECK_ARGUMENT_COUNT(1);

	// Call native DRM agent API
	// depends on application?
	OOIFBoolean retval;
	NATIVE_CALL(native_CapabilityHasCapability(STRING_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int Capabilities::getXmlCapabilities(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	const char *xml;
	NATIVE_GET(native_CapabilityGetXmlCapabilities(&xml));

	const char *before = "new DOMParser().parseFromString(\"";
	const char *after  = "\", 'text/xml')";

	int escape_char_needed = 0;

	char *i = (char*)xml;
	while (*i != '\0')
	{
		if (*i == '\\' || *i == '"')
			escape_char_needed++;
		i++;
	}

	char *final = OOIFUtils::getTmpBuffer(strlen(before)+strlen(xml)+strlen(after) + escape_char_needed + 1);

	if (!final)
	{
		return JSP_GET_NOMEM; // Opera OOM
	}

	strcpy(final, before);

	// Copy xml to final but escape " and \ characters
	char *final_ptr = final + strlen(final);
	i = (char*)xml;  // Reset pointer to start of xml
	while(*i != '\0')
	{
		if(*i == '\\' || *i == '"') *final_ptr++ = '\\';
		*final_ptr++ = *i++;
	}
	*final_ptr++ = '\0';

	strcat(final, after);

	result->u.string = final;
	result->type = JSP_TYPE_EXPRESSION;

	return JSP_GET_VALUE;
}
