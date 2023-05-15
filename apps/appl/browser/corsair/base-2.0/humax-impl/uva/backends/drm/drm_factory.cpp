/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include "drm_constants.h"
#include "uva_memory.h"
#include "uva_backend.h"
#include "backend_log.h"

#ifdef DUMMY_SUPPORT
#include "dummy_drm_backend.h"
#endif //DUMMY_SUPPORT

#ifdef PLAYREADY_SUPPORT
#include "pr_drm_backend.h"
#endif //PLAYREADY_SUPPORT

#ifdef CLEARKEY_SUPPORT
#include "ck_drm_backend.h"
#endif //CLEARKEY_SUPPORT


LOG_CAT(drmbackend);

extern "C"
UVADRMBackend* UVADRM_RequestBackend(const char* id, UVADRMClient* client)
{
	LOG_ENTRY;

#ifdef DUMMY_SUPPORT
	if (!strcmp(id, dummy_EME_ID) || !strcmp(id, dummy_OIPF_ID) )
		return UVA_NEW(DummyDRMBackend, (client));
#endif //DUMMY_SUPPORT

#ifdef PLAYREADY_SUPPORT
	if (!strcmp(id, playready_EME_ID) || !strcmp(id, playready_OIPF_ID) )
		return UVA_NEW(PlayReadyDRMBackend, (client));
#endif //PLAYREADY_SUPPORT

#ifdef CLEARKEY_SUPPORT
	if (!strcmp(id, clearkey_EME_ID) || !strcmp(id, clearkey_OIPF_ID) )
		return UVA_NEW(ClearKeyDRMBackend, (client));
#endif //CLEARKEY_SUPPORT

	return NULL;
}

extern "C"
UVA_STATUS UVADRM_ReleaseBackend(UVADRMBackend* backend)
{
	LOG_ENTRY;
	if(backend)
		UVA_DELETE(backend);
	return UVA_OK;
}

extern "C"
	UVA_STATUS UVADRM_GetSupportedBackendIDs(char**& backend_ids)
{
	LOG_ENTRY;

	unsigned char size = 1; //dummy + null terminator
#ifdef DUMMY_SUPPORT
	size += 2;
#endif //DUMMY_SUPPORT
#ifdef PLAYREADY_SUPPORT
	size += 2;
#endif //PLAYREADY_SUPPORT
#ifdef CLEARKEY_SUPPORT
	size += 2;
#endif //CLEARKEY_SUPPORT
	backend_ids = (char **)UVA_NEW_ARRAY(const char*, size);
	if (!backend_ids)
		return UVA_ERROR;
	unsigned int count = 0;
#ifdef DUMMY_SUPPORT
	backend_ids[count++] = (char *)uva_strdup(dummy_EME_ID);
	backend_ids[count++] = (char *)uva_strdup(dummy_OIPF_ID);
#endif //DUMMY_SUPPORT
#ifdef PLAYREADY_SUPPORT
	backend_ids[count++] = (char *)uva_strdup(playready_EME_ID);
	backend_ids[count++] = (char *)uva_strdup(playready_OIPF_ID);
#endif //PLAYREADY_SUPPORT
#ifdef CLEARKEY_SUPPORT
	backend_ids[count++] = (char *)uva_strdup(clearkey_EME_ID);
	backend_ids[count++] = (char *)uva_strdup(clearkey_OIPF_ID);
#endif //CLEARKEY_SUPPORT
	backend_ids[count++] = NULL;
	return UVA_OK;
}
