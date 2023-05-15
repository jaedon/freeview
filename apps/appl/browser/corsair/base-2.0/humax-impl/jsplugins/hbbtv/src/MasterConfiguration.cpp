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

/** 
 * @ToDo originally, localSystem property is not supported for HbbTV requirement. 
   but the Juke Box application require the property to login, so we remove OIPF condition.
   so I marked which code are fixed due to above issue with JUKEBOX SUPPORT as a comment.
 */
#if defined(OIPF) || defined(HBBTV) //JUKEBOX_SUPPORT
#include "LocalSystem.h"
#endif // OIPF

using namespace Ooif;

int MasterConfiguration::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_OBJECT("configuration", obj, new Configuration(), JSP_GET_VALUE);
	
#if defined(OIPF) || defined(HBBTV) //JUKEBOX_SUPPORT
	GET_OBJECT("localSystem", obj, new LocalSystem(), JSP_GET_VALUE);
#endif // defined(OIPF) || defined(HBBTV)
	return JSP_GET_NOTFOUND;
}

int MasterConfiguration::setter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	SET_READONLY("configuration");
#if defined(OIPF) || defined(HBBTV) //JUKEBOX_SUPPORT
	SET_READONLY("localSystem");
#endif // OIPF
	return JSP_PUT_NOTFOUND;
}
