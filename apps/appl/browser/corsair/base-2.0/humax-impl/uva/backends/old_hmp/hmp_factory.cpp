/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include <stdio.h>

#include <uva_backend.h>
#include <uva_memory.h>

#include "hmp_backend.h"
#include "mse_backend.h"

#define ENTRY fprintf(stdout, "| %s\n", __PRETTY_FUNCTION__)

#define MSE_TEST	0

extern "C"
UVABackend *UVA_RequestBackend(UVABackend::UVABackendType type, UVAClient* client)
{
	ENTRY;
	switch (type)
	{
		case UVABackend::UVAVideoBroadcastBackendType:
			return NULL;

		case UVABackend::UVAVideoBackendType:
		case UVABackend::UVAAudioBackendType:
		{
#if MSE_TEST
			MseMediaBackend *resource = UVA_NEW(MseMediaBackend, (type, client));
#else
			HmpMediaBackend *resource = UVA_NEW(HmpMediaBackend, (type, client));
#endif
			return resource;
		}

		default:
			return NULL;
	}
}

extern "C"
UVA_STATUS UVA_ReleaseBackend(UVABackend *backend)
{
	UVA_DELETE(backend);
	return UVA_OK;
}
