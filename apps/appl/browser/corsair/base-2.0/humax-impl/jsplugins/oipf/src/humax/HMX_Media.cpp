#if 0
// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "HMX_Media.h"
#include "HMX_MediaScan.h"

#include "HMX_Dlna.h"

using namespace Ooif;

int HMX_Media::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif

	GET_OBJECT("hmx_MediaScan", obj, new HMX_MediaScan(), JSP_GET_VALUE );
	GET_OBJECT("createSearchDLNA", obj, new HMX_DMSSearcher(), JSP_GET_VALUE );
	GET_OBJECT("hmx_MediaDoFile", obj, new HMX_MediaDoFile(), JSP_GET_VALUE );
	GET_OBJECT("hmx_MediaFileUtil", obj, new HMX_MediaFileUtil(), JSP_GET_VALUE );
	return JSP_GET_NOTFOUND;
}

int HMX_Media::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{

	return JSP_PUT_NOTFOUND;
}

#endif // OIPF
#endif
