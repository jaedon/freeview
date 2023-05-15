// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Exception.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Lovefilm;

Exception::Exception(const char *_name, const char *_message)
{
	ENTRY;
	name = strdup(_name);
	message = strdup(_message);
}

Exception::~Exception()
{
	free(name);
	free(message);
}

int Exception::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_STRING("name", this->name, JSP_GET_VALUE_CACHE);
	GET_STRING("message", message, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}
