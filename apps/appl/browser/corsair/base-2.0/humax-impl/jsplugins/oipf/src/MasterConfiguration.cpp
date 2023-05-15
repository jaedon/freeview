// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "MasterConfiguration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Configuration.h"
#ifdef OIPF
#include "LocalSystem.h"
#endif // OIPF
#ifdef JLABS
#include "jlabsLocalSystem.h"
#endif // JLABS

using namespace Ooif;

int MasterConfiguration::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_OBJECT("configuration", obj, new Configuration(), JSP_GET_VALUE);
#ifdef OIPF
	GET_OBJECT("localSystem", obj, new LocalSystem(), JSP_GET_VALUE);
#endif // OIPF
#ifdef JLABS
	GET_OBJECT("jlabslocalsystem", obj, new jlabsLocalSystem(), JSP_GET_VALUE);
#endif // JLABS
	return JSP_GET_NOTFOUND;
}

int MasterConfiguration::setter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	SET_READONLY("configuration");
#ifdef OIPF
	SET_READONLY("localSystem");
#endif // OIPF
	return JSP_PUT_NOTFOUND;
}
