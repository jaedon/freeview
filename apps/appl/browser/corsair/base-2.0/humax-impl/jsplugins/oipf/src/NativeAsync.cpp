
// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OOIF_ASYNC
#include <stdio.h>
#include <stdlib.h>

#include "NativeAsync.h"

OOIFReturnCode native_AsyncSetResumer(asyncResumer resumer)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AsyncSetTokenGetter(asyncResumeTokenGetter getter)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AsyncRevokeToken(ResumeToken token)
{
	return OOIF_RETURN_OK;
}
#endif // OOIF_ASYNC
