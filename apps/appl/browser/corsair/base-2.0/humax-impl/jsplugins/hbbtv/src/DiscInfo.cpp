// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "DiscInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "NativeDisc.h"

using namespace Ooif;

int DiscInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	// free, total, reserved
	NATIVE_GET_NUMBER("free", native_DiscInfoGetFree(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("total", native_DiscInfoGetTotal(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("reserved", native_DiscInfoGetReserved(&RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}
#endif // OIPF
