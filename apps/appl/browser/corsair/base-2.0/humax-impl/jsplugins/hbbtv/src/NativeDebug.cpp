// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeDebug.h"
#include "macros.h"

OOIFReturnCode native_debug(const char *arg)
{
	/* for web log */
	OOIF_LOG_FPRINT("%s\n", arg);
	return OOIF_RETURN_OK;
}
