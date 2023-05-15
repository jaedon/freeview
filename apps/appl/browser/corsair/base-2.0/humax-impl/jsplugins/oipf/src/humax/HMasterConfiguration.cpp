// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "HMasterConfiguration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "HConfiguration.h"
#ifdef OIPF
#include "HLocalSystem.h"
#endif // OIPF

using namespace Ooif;

int HMasterConfiguration::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_OBJECT("configuration", obj, new HConfiguration(), JSP_GET_VALUE);
#ifdef OIPF
	GET_OBJECT("localSystem", obj, new HLocalSystem(), JSP_GET_VALUE);
#endif // OIPF
	return JSP_GET_NOTFOUND;
}

int HMasterConfiguration::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	// No settable values in this object
	return JSP_PUT_NOTFOUND;
}
