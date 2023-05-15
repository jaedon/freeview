// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeSecurity.h"
#include "bplglobal.h"

OOIFReturnCode native_SecurityAllowAccess(const char *protocol, const char *hostname,
        int port, OOIFBoolean  *retval)
{
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

#ifdef HBBTV_SECURITY
OOIFReturnCode native_PermissionsIsTrusted(long int window_identifier, const char *host_uri)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PermissionsIsBroadcastRelated(long int window_identifier, const char *host_uri)
{
	return OOIF_RETURN_OK;
}
#endif /* HBBTV_SECURITY */


OOIFReturnCode native_HmxPermissionsIsBroadcastRelated(long int window_identifier, const char *host_uri)
{
	if ( !BplGlobalConfig::getIsBroadcastRelated() )
	{
		return OOIF_RETURN_ERROR;
	}

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_HmxSetReactiveAit(bool flag)
{	
    BplGlobalConfig::setReactivateAit(flag);

    return OOIF_RETURN_OK;
}
