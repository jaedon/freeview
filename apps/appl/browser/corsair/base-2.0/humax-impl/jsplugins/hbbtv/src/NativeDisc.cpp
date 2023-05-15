// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "NativeDisc.h"

#include "opldiscinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OOIFReturnCode native_DiscInfoOnCreate()
{
	return OOIF_RETURN_OK;
}
// All return in megabytes
OOIFReturnCode native_DiscInfoGetFree(OOIFNumber *retval)
{
	*retval = 1500;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DiscInfoGetTotal(OOIFNumber *retval)
{
	*retval = 2000;
	return OOIF_RETURN_OK;
}

// The space reserved for scheduled or ongoing recordings or downloads
OOIFReturnCode native_DiscInfoGetReserved(OOIFNumber *retval)
{
	*retval = 200;
	return OOIF_RETURN_OK;
}
#endif // OIPF
