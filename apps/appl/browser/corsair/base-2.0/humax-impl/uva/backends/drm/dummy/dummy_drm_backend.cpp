/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#ifdef DUMMY_SUPPORT

#include "dummy_drm_backend.h"
#include "drm_constants.h"
#include "uva_event.h"
#include "uva_client.h"

#include <stdio.h>

DummyDRMBackend::DummyDRMBackend(UVADRMClient *client) : UVAManagedDRMBackend(), m_client(client), session_num(0)
{
}

DummyDRMBackend::~DummyDRMBackend()
{
}

//virtual
const char* DummyDRMBackend::getId() const
{
	return dummy_EME_ID;
}

//virtual
const char* DummyDRMBackend::getOIPFId() const
{
	return dummy_OIPF_ID;
}

//virtual
UVA_STATUS DummyDRMBackend::generateKeyRequest(const unsigned char* init_data, size_t init_data_size)
{
	char session_id[11];
	session_id[snprintf(session_id, sizeof(session_id), "%d", session_num++)] = '\0';

	UVAKeyMessageDRMEvent *event = UVA_NEW(UVAKeyMessageDRMEvent, (getId(), session_id, (const unsigned char*)"Password?", 10, "www.example.org", true));
	if (!event)
		return UVA_ERROR;
	return m_client->handleDRMEvent(event);
}

//virtual
UVA_STATUS DummyDRMBackend::addKey(const unsigned char* key, size_t key_size, const unsigned char* init_data, size_t init_data_size, const char* session_id)
{
	if (!strncmp((const char*)key, "Open Sesame", key_size))
	{
		UVADRMEvent *event = UVA_NEW(UVADRMEvent, (UVADRMEvent::KEY_COMPLETE, getId(), session_id, true));
		if (!event)
			return UVA_ERROR;
		return m_client->handleDRMEvent(event);
	}
	else
	{
		UVAKeyErrorDRMEvent *event = UVA_NEW(UVAKeyErrorDRMEvent, (getId(), session_id, UVAKeyErrorDRMEvent::MEDIA_KEYERR_UNKNOWN, 0, true));
		if (!event)
			return UVA_ERROR;
		return m_client->handleDRMEvent(event);
	}
}

//virtual
UVA_STATUS DummyDRMBackend::cancelKeyRequest(const char* session_id)
{
	return UVA_OK;
}

//virtual
UVA_STATUS DummyDRMBackend::getKeyReleases()
{
	char session_id[11];
	session_id[snprintf(session_id, sizeof(session_id), "%d", session_num)] = '\0';

	UVAKeyMessageDRMEvent *event = UVA_NEW(UVAKeyMessageDRMEvent, (getId(), session_id, (const unsigned char*)"Close Password?", 16, "www.example.org", false));
	if (!event)
		return UVA_ERROR;
	return m_client->handleDRMEvent(event);
}

//virtual
UVA_STATUS DummyDRMBackend::addKeyReleaseCommit(const unsigned char* message, size_t message_size, const char* session_id)
{
	if (!strncmp((const char*)message, "Close Sesame", message_size))
	{
		UVADRMEvent *event = UVA_NEW(UVADRMEvent, (UVADRMEvent::KEY_COMPLETE, getId(), session_id, false));
		if (!event)
			return UVA_ERROR;
		return m_client->handleDRMEvent(event);
	}
	else
	{
		UVAKeyErrorDRMEvent *event = UVA_NEW(UVAKeyErrorDRMEvent, (getId(), session_id, UVAKeyErrorDRMEvent::MEDIA_KEYERR_UNKNOWN, 0, false));
		if (!event)
			return UVA_ERROR;
		return m_client->handleDRMEvent(event);
	}
}

//virtual
UVA_STATUS DummyDRMBackend::sendDRMMessage(const char *msg_type, const char *msg, char *&session_id)
{
	char session_a[11];
	session_a[snprintf(session_a, sizeof(session_a), "%d", session_num++)] = '\0';
	char* session = (char*)uva_malloc(sizeof(char)*(strlen(session_a) + 1));
	if (!session)
		return UVA_ERROR;
	strncpy(session, session_a, strlen(session_a) + 1);

	const char* message;
	double result;
	if (!strcmp("Open Sesame", msg))
	{
		message = "Opened!";
		result = 0;
	}
	else
	{
		message = "Nope!";
		result = 1;
	}
	UVAResultMessageDRMEvent *event = UVA_NEW(UVAResultMessageDRMEvent, (getOIPFId(), session, message, result, true));
	if (!event)
	{
		uva_free(session);
		session_id = NULL;
		return UVA_ERROR;
	}
	session_id = session;
	return m_client->handleDRMEvent(event);
}

//virtual
UVA_STATUS DummyDRMBackend::constructDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter*& decrypter, bool automatic_key_aquasition)
{
	decrypter = NULL;
	return UVA_ERROR;
}

//virtual
UVA_STATUS DummyDRMBackend::destroyDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter* decrypter)
{
	return UVA_ERROR;
}

//virtual
UVA_STATUS DummyDRMBackend::onInitData(const unsigned char* init_data, size_t init_data_size)
{
	char session_id[11];
	session_id[snprintf(session_id, sizeof(session_id), "%d", session_num)] = '\0';

	UVANeedKeyDRMEvent *event = UVA_NEW(UVANeedKeyDRMEvent, (getId(), session_id, init_data, init_data_size, false));
	if (!event)
		return UVA_ERROR;
	return m_client->handleDRMEvent(event);
}

#endif //DUMMY_SUPPORT
