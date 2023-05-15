/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include <stdio.h>

#include <uva_backend.h>
#include <uva_memory.h>

#include "backend_log.h"

#include "hmpbackend.h"

LOG_CAT(backend);

extern "C"
UVABackend *UVA_RequestBackend(UVABackend::UVABackendType type, UVAClient* client)
{
	LOG_ENTRY;

	switch (type)
	{
		case UVABackend::UVAVideoBroadcastBackendType:
			return NULL;

		case UVABackend::UVAVideoBackendType:
		case UVABackend::UVAAudioBackendType:
		{
			HmpMediaBackend *resource = UVA_NEW(HmpMediaBackend, (type, client));
			return resource;
		}

		default:
			return NULL;
	}
}

extern "C"
UVA_STATUS UVA_ReleaseBackend(UVABackend *backend)
{
	LOG_ENTRY;

	UVA_DELETE(backend);
	return UVA_OK;
}
