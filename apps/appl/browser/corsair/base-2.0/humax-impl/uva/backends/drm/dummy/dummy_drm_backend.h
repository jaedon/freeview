/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#ifndef _DUMMY_DRM_BACKEND_H_
#define _DUMMY_DRM_BACKEND_H_

#ifdef DUMMY_SUPPORT

#include "../managed_drm_backend.h"

class DummyDRMBackend : public UVAManagedDRMBackend
{
public:
	DummyDRMBackend(UVADRMClient *client);
	~DummyDRMBackend();

	virtual const char* getId() const;

	virtual const char* getOIPFId() const;

	virtual UVA_STATUS generateKeyRequest(const unsigned char* init_data, size_t init_data_size);

	virtual UVA_STATUS addKey(const unsigned char* key, size_t key_size, const unsigned char* init_data, size_t init_data_size, const char* session_id);

	virtual UVA_STATUS cancelKeyRequest(const char* session_id);

	virtual UVA_STATUS sendDRMMessage(const char *msg_type, const char *msg, char *&session_id);

	virtual UVA_STATUS getKeyReleases();

	virtual UVA_STATUS addKeyReleaseCommit(const unsigned char* message, size_t message_size, const char* session_id);

	virtual UVA_STATUS onInitData(const unsigned char* init_data, size_t init_data_size);

private:
	UVADRMClient *m_client;
	unsigned int session_num;

	virtual UVA_STATUS constructDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter*& decrypter, bool automatic_key_aquasition);

	virtual UVA_STATUS destroyDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter* decrypter);
};

#endif //DUMMY_SUPPORT

#endif //_DUMMY_DRM_BACKEND_H_
