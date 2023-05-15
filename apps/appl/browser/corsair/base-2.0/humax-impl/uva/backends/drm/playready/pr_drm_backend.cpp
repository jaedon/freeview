/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#ifdef PLAYREADY_SUPPORT

#include "pr_drm_backend.h"
#include "drmlicacqv3.h"
#include "uva_event.h"
#include "uva_client.h"
#include "drm_constants.h"

#include "backend_log.h"

#include "cso_uva.h"

LOG_CAT(drm);

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef PLAY_READY_25

#ifndef DRM_E_LICENSENOTFOUND
#define DRM_E_LICENSENOTFOUND	DRM_E_LICENSE_NOT_FOUND
#endif

#ifndef DRM_E_REVOCATION_BUFFERTOOSMALL
#define DRM_E_REVOCATION_BUFFERTOOSMALL	DRM_E_REVOCATION_BUFFER_TOO_SMALL
#endif

const DRM_WCHAR store_name[] =
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('y','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('e','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('y','\0'), ONE_WCHAR('2','\0'), ONE_WCHAR('5','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e','\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h','\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s','\0'), ONE_WCHAR('\0','\0')
};

#else
const DRM_WCHAR store_name[] =
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e','\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h','\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s','\0'), ONE_WCHAR('\0','\0')
};
#endif /*else PLAY_READY_25*/
const DRM_CONST_STRING store_name_str = { store_name, sizeof(store_name) / sizeof(DRM_WCHAR) };
const DRM_CONST_STRING *rights_strs[] = { &g_dstrWMDRM_RIGHT_PLAYBACK };

extern "C" {
DRM_VOID * Oem_Create(void);
DRM_VOID Oem_Destroy( DRM_VOID *oem);
}

unsigned int PlayReadyDRMBackend::sessionNum = 0;

__attribute__((constructor))
static void InitDRM(void)
{
	Drm_Platform_Initialize();
}

__attribute__((destructor))
static void UninitDRM(void)
{
	Drm_Platform_Uninitialize();
}

PlayReadyDRMBackend::DRMAppContext::DRMAppContext() : context(NULL), oem(NULL), buffer(NULL), buffer_size(0), revocation_buffer(NULL), revocation_buffer_size(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE)
{
	LOG_ENTRY;
}

PlayReadyDRMBackend::DRMAppContext::~DRMAppContext()
{
	LOG_ENTRY;
	
	if (context)
	{
		Drm_Uninitialize(context);
		UVA_DELETE(context);
		context = NULL;
	}
	if (buffer)
	{
		UVA_DELETE_ARRAY(buffer);
		buffer = NULL;
	}
	if (revocation_buffer)
	{
		UVA_DELETE_ARRAY(revocation_buffer);
		revocation_buffer = NULL;
	}

	if(oem)
	{
		Oem_Destroy(oem);
		oem = NULL;
	}
}

UVA_STATUS PlayReadyDRMBackend::DRMAppContext::init()
{
	LOG_ENTRY;

	if (context)
		return UVA_ERROR;
	context = UVA_NEW(DRM_APP_CONTEXT, ());
	buffer = UVA_NEW_ARRAY(DRM_BYTE, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE * 2+1);
	if (!context || !buffer)
		return UVA_ERROR;
	buffer_size = MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE * 2;
	memset(buffer, 0x00, buffer_size+1);
	
	oem = (DRM_VOID*)Oem_Create();
	if(oem == NULL)
	{
		LOG_ENTRY;
		goto error;
	}
	
	if (DRM_FAILED(Drm_Initialize(context, oem, buffer, buffer_size, &store_name_str)))
	{
		LOG_ENTRY;
		goto error;
	}
	
	if(CSO_ERR_OK != CSO_UVA_GetSecureClock((void *)context))
	{
		LOG_ERROR("FAIL : GetSecureClock, Ignore Error of SecureClock, Error dectection is Bind Function\n");
	}
	LOG_ENTRY;

	return UVA_OK;

error:
	UVA_DELETE(context);
	context = NULL;
	UVA_DELETE_ARRAY(buffer);
	buffer = NULL;
	if(oem != NULL)
	{
		Oem_Destroy(oem);
		oem = NULL;
	}
	return UVA_ERROR;
}

UVA_STATUS PlayReadyDRMBackend::DRMAppContext::resize()
{
	LOG_ENTRY;
	
	if (!context)
		return UVA_NOT_INITIALIZED;
	DRM_DWORD new_buffer_size = buffer_size * 2;
	DRM_BYTE *new_buffer = UVA_NEW_ARRAY(DRM_BYTE, new_buffer_size+1);
	if (!new_buffer)
		return UVA_ERROR;
	memset(new_buffer, 0x00, new_buffer_size+1);
	if (DRM_FAILED(Drm_ResizeOpaqueBuffer(context, new_buffer, new_buffer_size)))
	{
		UVA_DELETE_ARRAY(new_buffer);
		return UVA_ERROR;
	}
	UVA_DELETE_ARRAY(buffer);
	buffer = new_buffer;
	buffer_size = new_buffer_size;
	return UVA_OK;
}

UVA_STATUS PlayReadyDRMBackend::DRMAppContext::resizeRevocationBuffer()
{
	LOG_ENTRY;
	
	if (!context)
		return UVA_NOT_INITIALIZED;

	DRM_DWORD new_revocation_buffer_size = revocation_buffer_size * 2;
	DRM_BYTE *new_revocation_buffer = UVA_NEW_ARRAY(DRM_BYTE, new_revocation_buffer_size+1);
	if (!new_revocation_buffer)
		return UVA_ERROR;
	memset(new_revocation_buffer, 0x00, new_revocation_buffer_size+1);
	if (DRM_FAILED(Drm_Revocation_SetBuffer(context, new_revocation_buffer, new_revocation_buffer_size)))
	{
		UVA_DELETE_ARRAY(new_revocation_buffer);
		LOG_ENTRY;
		return UVA_ERROR;
	}
	if (revocation_buffer)
		UVA_DELETE_ARRAY(revocation_buffer);
	revocation_buffer = new_revocation_buffer;
	revocation_buffer_size = new_revocation_buffer_size;
	return UVA_OK;
}

PlayReadyDRMBackend::PlayReadyDRMBackend(UVADRMClient *client) : UVAManagedDRMBackend(), m_client(client)
{
	LOG_ENTRY;

	/* Copy playready.bin to HUMAX NVRAM */
	struct stat st;
#ifdef 	PLAY_READY_25
	if((0 == stat("/var/lib/humaxtv/drm/playready25/playready25.bin", &st) && S_ISREG(st.st_mode)) != TRUE)
	{
		system("rm -rf /var/lib/humaxtv/drm/playready25/*");
		system("mkdir -p /var/lib/humaxtv/drm/playready25");
		system("cp -a /usr/drm/playready25.bin /var/lib/humaxtv/drm/playready25/");
	}
#else
	if((0 == stat("/var/lib/humaxtv/drm/playready.bin", &st) && S_ISREG(st.st_mode)) != TRUE)
	{
		system("rm -rf /var/lib/humaxtv/drm");
		system("mkdir /var/lib/humaxtv/drm");
		system("cp -a /usr/drm/playready.bin /var/lib/humaxtv/drm/");
	}	
#endif
}

PlayReadyDRMBackend::~PlayReadyDRMBackend()
{
	LOG_ENTRY;
	
	while(m_sessions.size() > 0)
	{
		UVA_DELETE((KeyExchangeSession*)m_sessions.front());
		m_sessions.pop_front();
	}
}

UVA_STATUS PlayReadyDRMBackend::createSession(PlayReadyDRMBackend::KeyExchangeSession *&session)
{
	LOG_ENTRY;
	
	session = UVA_NEW(KeyExchangeSession, ());
	if (!session)
		return UVA_ERROR;
	UVA_STATUS status = session->m_drm_app_context.init();
	if (status == UVA_OK)
	{
		session->m_sessionId = UVA_NEW_ARRAY(char, 11);
		if (session->m_sessionId)
			session->m_sessionId[snprintf(session->m_sessionId, 11, "%d", sessionNum++)] = '\0';
		else
			status = UVA_ERROR;
	}
	if (status != UVA_OK)
	{
		status = UVA_ERROR;
		UVA_DELETE(session);
		session = NULL;
	}
	return status;
}

PlayReadyDRMBackend::KeyExchangeSession* PlayReadyDRMBackend::getSession(const char *sessionId)
{
	LOG_ENTRY;
	
	std::list<KeyExchangeSession*, UVA_ALLOCATOR<KeyExchangeSession*> >::iterator it;
	for (it = m_sessions.begin(); it != m_sessions.end(); it++)
	{
		if (!strcmp((*it)->m_sessionId, sessionId))
		{
			return *it;
		}
	}
	return NULL;
}

//virtual
const char* PlayReadyDRMBackend::getId() const
{
	return playready_EME_ID;
}

//virtual
const char* PlayReadyDRMBackend::getOIPFId() const
{
	return playready_OIPF_ID;
}

static inline uint32_t getSize(const unsigned char* data)
{
	LOG_ENTRY;
	
#ifdef LITTLE_ENDIAN
	return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
#else //LITTLE_ENDIAN
	return *(uint32_t*)data;
#endif //LITTLE_ENDIAN
}

const unsigned char* PlayReadyDRMBackend::parseInitData(const unsigned char*& init_data, size_t& init_data_size, size_t& pssh_data_size)
{
	LOG_ENTRY;
	
	//Check init data is concatenated pssh boxes
	//If yes extract the data for playready
	pssh_data_size = init_data_size;
	const unsigned char* pssh_data = init_data;
	const unsigned char* start_data = init_data;
	bool error = false;
	while (init_data_size > 0)
	{
		pssh_data = init_data;
		if (init_data_size < 12 + 16 + 4 || memcmp(pssh_data + 4, "pssh", 4))
		{
			//Content is not a pssh box
			if (start_data != pssh_data) //Unexpected data after we started parsing = error;
			{
				error = true;
			}
			else
			{
				init_data += init_data_size;
				init_data_size = 0;
			}
			break;
		}
		uint32_t box_size = getSize(pssh_data);
		if (init_data_size < box_size) //incomplete/corrupted data?
		{
			error = true;
			break;
		}
		if (!memcmp(pssh_data + 12, playready_UUID, 16))
		{
			pssh_data += 12 + 16; //+ fullbox + uuid
			pssh_data_size = getSize(pssh_data);
			pssh_data += 4; //+size
			if (init_data_size - (12 + 16 + 4) < pssh_data_size) //incomplete/corrupted data?
			{
				error = true;
			}
			else //we found playready pssh box
			{
				init_data += box_size;
				init_data_size -= box_size;
			}
			break;
		}
		init_data += box_size;
		init_data_size -= box_size;
	}
	if (error)
	{
		pssh_data = NULL;
		pssh_data_size = 0;
		init_data = NULL;
		init_data_size = 0;
	}
	return pssh_data;
}

//virtual
UVA_STATUS PlayReadyDRMBackend::generateKeyRequest(const unsigned char* init_data, size_t init_data_size)
{
	LOG_ENTRY;
	
	UVA_STATUS status = UVA_ERROR;
	bool message_sent = false;
	do
	{
		size_t data_size = 0;
		const unsigned char* data = parseInitData(init_data, init_data_size, data_size);
		if (init_data_size == 0 && message_sent)
			return status;

		KeyExchangeSession* session = NULL;
		status = createSession(session);
		if (status != UVA_OK)
			return status;

		status = UVA_ERROR;
		DRM_RESULT result = DRM_E_BUFFERTOOSMALL;
		do
		{
			result = Drm_Content_SetProperty(session->m_drm_app_context.getContext(), DRM_CSP_AUTODETECT_HEADER, (const DRM_BYTE*) data, (DRM_DWORD) data_size);
			if (result == DRM_E_BUFFERTOOSMALL && session->m_drm_app_context.resize() != UVA_OK)
				break;
		}
		while (result == DRM_E_BUFFERTOOSMALL);

		DRM_DWORD la_url_size = 0;
		DRM_DWORD challenge_size = 0;
		DRM_CHAR *la_url = NULL;
		DRM_BYTE *challenge = NULL;
		enum
		{
			NONE,
			SILENT,
			NON_SILENT
		}
		la_url_type = NONE;
		bool keep_session = false;

		if (DRM_SUCCEEDED(result))
		{
			//Try to get silent license acquisition URL first
			result = Drm_LicenseAcq_GenerateChallenge(session->m_drm_app_context.getContext(), rights_strs, 1, NULL, NULL, 0, la_url, &la_url_size, NULL, NULL, challenge, &challenge_size);
			if (result == DRM_E_NO_URL) //If there is no silent LA URL, try to get non-silent one
			{
				result = Drm_LicenseAcq_GenerateChallenge(session->m_drm_app_context.getContext(), rights_strs, 1, NULL, NULL, 0, NULL, NULL, la_url, &la_url_size, challenge, &challenge_size);
				if (result != DRM_E_NO_URL)
					la_url_type = NON_SILENT;
				else
					result = Drm_LicenseAcq_GenerateChallenge(session->m_drm_app_context.getContext(), rights_strs, 1, NULL, NULL, 0, NULL, NULL, NULL, NULL, challenge, &challenge_size);
			}
			else
			{
				la_url_type = SILENT;
			}
			if (result == DRM_E_BUFFERTOOSMALL)
			{
				if (la_url_type != NONE)
				{
					la_url = UVA_NEW_ARRAY(DRM_CHAR, la_url_size + 1);
					if (la_url == NULL)
					{
						LOG_ERROR("failed to memory allocation.\n");
						return UVA_ERROR;
					}
					memset(la_url, 0x00, la_url_size + 1);
				}

				challenge = UVA_NEW_ARRAY(DRM_BYTE, challenge_size+1);
				if (challenge == NULL)
				{
					LOG_ERROR("failed to memory allocation.\n");
					if (la_url)
					{
						UVA_DELETE_ARRAY(la_url);
					}

					return UVA_ERROR;
				}
				memset(challenge, 0x00, challenge_size+1);

				if ((la_url || la_url_type == NONE) && challenge)
				{
					switch (la_url_type)
					{
					case SILENT:
						result = Drm_LicenseAcq_GenerateChallenge(session->m_drm_app_context.getContext(), rights_strs, 1, NULL, NULL, 0, la_url, &la_url_size, NULL, NULL, challenge, &challenge_size);
						break;
					case NON_SILENT:
						result = Drm_LicenseAcq_GenerateChallenge(session->m_drm_app_context.getContext(), rights_strs, 1, NULL, NULL, 0, NULL, NULL, la_url, &la_url_size, challenge, &challenge_size);
						break;
					case NONE:
						result = Drm_LicenseAcq_GenerateChallenge(session->m_drm_app_context.getContext(), rights_strs, 1, NULL, NULL, 0, NULL, NULL, NULL, NULL, challenge, &challenge_size);
						break;
					}
				}
			}
		}

		if (DRM_SUCCEEDED(result) && challenge != NULL)
		{
			if (la_url)
				la_url[la_url_size] = 0;
			if (session->m_la_url)
			{
				UVA_DELETE_ARRAY(session->m_la_url);
				session->m_la_url = NULL;
			}
			if (la_url_size)
			{
				session->m_la_url = la_url;
				la_url = NULL;
			}
				for(int i=0 ; i<5 ; ++i)
				{
					UVAKeyMessageDRMEvent *event = UVA_NEW(UVAKeyMessageDRMEvent, (getId(), session->m_sessionId, (const unsigned char*) challenge, (const size_t) challenge_size, (const char*)session->m_la_url, true));
					if (event)
					{
						session->m_state = KeyExchangeSession::KEY_CHALLENGE;
						status = m_client->handleDRMEvent(event);
						keep_session = true;
						message_sent = true;
					}
				}
			if(keep_session == TRUE && message_sent == TRUE)
			{
				m_sessions.push_back(session);
			}
		}
		else
		{
			UVAKeyErrorDRMEvent::MediaKeyError error;
			switch (result)
			{
				default:
					error = UVAKeyErrorDRMEvent::MEDIA_KEYERR_UNKNOWN;
					break;
			}
			UVAKeyErrorDRMEvent *event = UVA_NEW(UVAKeyErrorDRMEvent, (getId(), NULL, error, 0, true));
			if (event)
			{
				session->m_state = KeyExchangeSession::KEY_ERROR;
				status = m_client->handleDRMEvent(event);
				message_sent = true;
			}
		}

		if (la_url)
			UVA_DELETE_ARRAY(la_url);
		if (challenge)
			UVA_DELETE_ARRAY(challenge);
		if (!keep_session)
			UVA_DELETE(session);
	}
	while (status == UVA_OK && init_data_size > 0);
	return status;
}

//virtual
UVA_STATUS PlayReadyDRMBackend::addKey(const unsigned char* key, size_t key_size, const unsigned char* init_data, size_t init_data_size, const char* session_id)
{
	LOG_ENTRY;
	
	enum
	{
		NONE,
		SUCCESS,
		ERROR
	}
	send_message = NONE;

	KeyExchangeSession* session = NULL;
	if (session_id)
	{
		session = getSession(session_id);
		if (!session)
			return UVA_NOT_INITIALIZED;
	}
	else
	{
		createSession(session);
		if (!session)
			return UVA_ERROR;
	}

	//Playready APIs will change the data in key while parsing, copy it
	DRM_BYTE *key_copy = UVA_NEW_ARRAY(DRM_BYTE, key_size+1);
	if (!key_copy)
	{
		//if we had created a session, destroy it
		if (!session_id)
			UVA_DELETE(session);
		return UVA_ERROR;
	}
	memset(key_copy, 0x00, key_size+1);
	memcpy(key_copy, key, key_size);

	UVA_STATUS status = UVA_OK;
	DRM_RESULT result = DRM_SUCCESS;
	if (session->m_state < KeyExchangeSession::KEY_ADDED)
	{
		DRM_LICENSE_RESPONSE response;
		memset(&response, 0, sizeof(DRM_LICENSE_RESPONSE));
		do
		{
			if ((status = session->m_drm_app_context.resizeRevocationBuffer()) != UVA_OK)
				break;
			result = Drm_LicenseAcq_ProcessResponse(session->m_drm_app_context.getContext(), DRM_PROCESS_LIC_RESPONSE_NO_FLAGS, (pfnLicenseResponseCallback)staticLicenseResponseCallback, (DRM_VOID*)this, key_copy, (DRM_DWORD) key_size, &response);
		}
		while (result == DRM_E_REVOCATION_BUFFERTOOSMALL);

		if ((DRM_SUCCEEDED(result) || (result == DRM_S_MORE_DATA)) && status == UVA_OK)
		{
			//This means that response was processed succesfully
			//iterate through responses to check if we actually added licenses
			//We will send KEY_COMPLETE only if all licenses in response were added successfully
			for (unsigned int i = 0; i < response.m_cAcks; i++)
			{
				result = response.m_rgoAcks[i].m_dwResult;
				if (DRM_FAILED(result))
					break;
			}
			if (response.m_eType == eV3Protocol) //Need to generate ack for v3 protocol even if we failed to add licenses
			{
				DRM_BYTE *ack = NULL;
				DRM_DWORD ack_size = 0;
				send_message = ERROR;
				session->m_result = result; //Store result in session, we will use it after ack response is received
				result = Drm_LicenseAcq_GenerateAck(session->m_drm_app_context.getContext(), &response, ack, &ack_size);
				if (result == DRM_E_BUFFERTOOSMALL)
				{
					ack = UVA_NEW_ARRAY(DRM_BYTE, ack_size+1);
					if (!ack)
					{
						status = UVA_ERROR;
					}
					else
					{
						memset(ack, 0x00, ack_size+1);
						result = Drm_LicenseAcq_GenerateAck(session->m_drm_app_context.getContext(), &response, ack, &ack_size);
						if (DRM_SUCCEEDED(result))
						{
							UVAKeyMessageDRMEvent *event = UVA_NEW(UVAKeyMessageDRMEvent, (getId(), session->m_sessionId, (const unsigned char*) ack, (const size_t) ack_size, (const char*)session->m_la_url, true));
							session->m_state = KeyExchangeSession::KEY_ADDED;
							if (event)
							{
								status = m_client->handleDRMEvent(event);
								if (!session_id)
									m_sessions.push_back(session);
								if (status == UVA_OK)
									send_message = NONE;
							}
							UVA_DELETE_ARRAY(ack);
						}
					}
				}
				else if (DRM_SUCCEEDED(result)) //if there was no transaction id in response no ack is needed and we are done
				{
					send_message = DRM_SUCCEEDED(result) ? SUCCESS : ERROR;
				}
			}
			else //Nothing to do for other protocols, just check storage result to determine status
			{
				send_message =  DRM_SUCCEEDED(result) ? SUCCESS : ERROR;
			}
		}
		else
		{
			send_message = ERROR;
		}
	}
	else
	{
		DRM_RESULT ack_result;
		result =  Drm_LicenseAcq_ProcessAckResponse(session->m_drm_app_context.getContext(), key_copy, (DRM_DWORD) key_size, &ack_result);
		if (DRM_SUCCEEDED(result) || (result == DRM_S_MORE_DATA)) //if processing response succeeded
			result = ack_result;
		if (DRM_SUCCEEDED(result) || (result == DRM_S_MORE_DATA)) //and was handled properly
			result = session->m_result; //use result of licenses storage to
		send_message = (DRM_SUCCEEDED(result) || (result == DRM_S_MORE_DATA)) ? SUCCESS : ERROR; //check what kind of message we should send
	}

	UVA_DELETE_ARRAY(key_copy);

	if (send_message != NONE)
	{
		UVADRMEvent *event = NULL;

		if (send_message == ERROR)
		{
			UVAKeyErrorDRMEvent::MediaKeyError error;
			switch (result)
			{
				//if we got some crap as response, treat it as service error
				case DRM_E_CERTIFICATE_REVOKED:
				case DRM_E_XMLNOTFOUND:
				case DRM_E_SOAPXML_XML_FORMAT:
				case DRM_E_INVALID_LICENSE_RESPONSE_SIGNATURE:
				case DRM_E_LICENSE_RESPONSE_SIGNATURE_MISSING:
					error = UVAKeyErrorDRMEvent::MEDIA_KEYERR_SERVICE;
					break;
				default:
					error = UVAKeyErrorDRMEvent::MEDIA_KEYERR_UNKNOWN;
					break;
			}
			event = UVA_NEW(UVAKeyErrorDRMEvent, (getId(), session_id, error, 0, true));
			session->m_state = KeyExchangeSession::KEY_ERROR;
		}
		else
		{
			session->m_state = KeyExchangeSession::KEY_SUCCESS;
			event = UVA_NEW(UVADRMEvent, (UVADRMEvent::KEY_COMPLETE, getId(), session_id, true));

			notifyKeyAdded();
		}
		if (event)
			status = m_client->handleDRMEvent(event);
		else
			status = UVA_ERROR;
	}

	//We tried to send completed/error message, no need to store session anymore
	if (session->m_state > KeyExchangeSession::KEY_ADDED)
	{
		m_sessions.remove(session);
		UVA_DELETE(session);
	}
	return status;
}

//static
DRM_RESULT PlayReadyDRMBackend::staticLicenseResponseCallback(DRM_VOID *pvData, DRM_STORE_LICENSE_CALLBACK_ENUM eReason, DRM_VOID *me)
{
	return static_cast<PlayReadyDRMBackend*>(me)->licenseResponseCallback(pvData, eReason);
}

DRM_RESULT PlayReadyDRMBackend::licenseResponseCallback(DRM_VOID *pvData, DRM_STORE_LICENSE_CALLBACK_ENUM eReason)
{
	return DRM_SUCCESS;
}


//virtual
UVA_STATUS PlayReadyDRMBackend::addKeyReleaseCommit(const unsigned char* message, size_t message_size, const char* session_id)
{
	return UVA_ERROR;
}

//virtual
UVA_STATUS PlayReadyDRMBackend::getKeyReleases()
{
	return UVA_OK;
}

//virtual
UVA_STATUS PlayReadyDRMBackend::cancelKeyRequest(const char* session_id)
{
	LOG_ENTRY;
	
	KeyExchangeSession* session = getSession(session_id);
	if (session)
	{
		//TODO: should we remove license from store if we send responseack (in KEY_ADDED state)?
		m_sessions.remove(session);
		UVA_DELETE(session);
	}
	return UVA_OK;
}

//virtual
UVA_STATUS PlayReadyDRMBackend::sendDRMMessage(const char *msg_type, const char *msg, char *&session_id)
{
	return UVA_ERROR;
}


UVA_STATUS PlayReadyDRMBackend::sendNeedKey(const unsigned char* init_data, size_t init_data_size, bool noKey)
{
	LOG_ENTRY;
	
	UVANeedKeyDRMEvent *event = UVA_NEW(UVANeedKeyDRMEvent, (getId(), NULL, init_data, init_data_size, noKey));
	if (!event)
		return UVA_ERROR;
	return m_client->handleDRMEvent(event);
}

//virtual
UVA_STATUS PlayReadyDRMBackend::constructDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter*& decrypter, bool automatic_key_aquasition)
{
	LOG_ENTRY;
	
	decrypter = UVA_NEW(PlayReadyDRMDecrypter, (this, automatic_key_aquasition));
	if (!decrypter)
		return UVA_ERROR;
	return UVA_OK;
}

//virtual
UVA_STATUS PlayReadyDRMBackend::destroyDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter* decrypter)
{
	LOG_ENTRY;
	
	UVA_DELETE(decrypter);
	return UVA_OK;
}

//virtual
UVA_STATUS PlayReadyDRMBackend::onInitData(const unsigned char* init_data, size_t init_data_size)
{
	LOG_ENTRY;
	
	sendNeedKey(init_data, init_data_size, false);
	return UVA_OK;
}

PlayReadyDRMBackend::PlayReadyDRMDecrypter::PlayReadyDRMDecrypter(PlayReadyDRMBackend* backend, bool automatic_key_aquasition) :
m_backend(backend)
, m_state(NOT_BOUND)
{
	LOG_ENTRY;
	
	memset(&m_decrypt_context, 0, sizeof(DRM_DECRYPT_CONTEXT));
	memset(&m_aes_context, 0, sizeof(DRM_AES_COUNTER_MODE_CONTEXT));
	pthread_cond_init(&m_decrypt_cond, NULL);
	pthread_mutex_init(&m_state_mutex, NULL);
}

PlayReadyDRMBackend::PlayReadyDRMDecrypter::~PlayReadyDRMDecrypter()
{
	LOG_ENTRY;
	
#ifdef PLAY_READY_25
	Drm_Reader_Close(&m_decrypt_context);
#endif
	pthread_cond_destroy(&m_decrypt_cond);
	pthread_mutex_destroy(&m_state_mutex);
}

//virtual
void PlayReadyDRMBackend::PlayReadyDRMDecrypter::stopDecrypting()
{
	LOG_ENTRY;
	
	pthread_cond_signal(&m_decrypt_cond);
}

const char Base64_Encoding_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

//virtual

UVA_STATUS PlayReadyDRMBackend::PlayReadyDRMDecrypter::init(const unsigned char* init_data, size_t init_data_size)
{
	LOG_ENTRY;
	
	if (m_state != NOT_BOUND || !init_data || init_data_size != 16)
		return UVA_ERROR;

	//Playready uses guids in their system, need to switch byte order of uuid to convert to guid
	unsigned char guid[16];
	memcpy(guid, init_data, init_data_size);
	unsigned char tmp = guid[0];
	guid[0] = guid[3];
	guid[3] = tmp;
	tmp = guid[1];
	guid[1] = guid[2];
	guid[2] = tmp;
	tmp = guid[4];
	guid[4] = guid[5];
	guid[5] = tmp;
	tmp = guid[6];
	guid[6] = guid[7];
	guid[7] = tmp;

	DRM_WCHAR encoded_kid[24];

	int len = 16;
	unsigned char *in = guid;
	DRM_WCHAR* out = encoded_kid;
	unsigned char tempchar1, tempchar2, tempchar3;
	unsigned char outcode1, outcode2, outcode3, outcode4;

	while(len > 0)
	{
		tempchar1 = *(in++);
		tempchar2 = (len > 1 ? *(in++) : 0);
		tempchar3 = (len > 2 ? *(in++) : 0);

		outcode1 = (tempchar1 >> 2) & 0x3f;
		outcode2 = (((tempchar1 << 4) & 0x30) | ((tempchar2 >> 4) & 0x0f));
		outcode3 = (len > 1 ? (((tempchar2 << 2) & 0x3c) | ((tempchar3 >> 6) & 0x03)) : 64);
		outcode4 = (len > 2 ? (tempchar3 & 0x3f) : 64);

		len -= 3;

		*(out++) = ONE_WCHAR(Base64_Encoding_chars[(size_t)outcode1], '\0');
		*(out++) = ONE_WCHAR(Base64_Encoding_chars[(size_t)outcode2], '\0');
		*(out++) = ONE_WCHAR(Base64_Encoding_chars[(size_t)outcode3], '\0');
		*(out++) = ONE_WCHAR(Base64_Encoding_chars[(size_t)outcode4], '\0');
	}

	UVA_STATUS status = m_context.init();
	if (status != UVA_OK)
		return status;

	DRM_RESULT result = DRM_E_BUFFERTOOSMALL;
	do
	{
		result = Drm_Content_SetProperty(m_context.getContext(), DRM_CSP_KID, (const DRM_BYTE*) encoded_kid, (DRM_DWORD) sizeof(encoded_kid));
		if (result == DRM_E_BUFFERTOOSMALL && m_context.resize() != UVA_OK)
			break;
	}
	while (result == DRM_E_BUFFERTOOSMALL);

	if (DRM_FAILED(result))
		return UVA_ERROR;

	pthread_mutex_lock(&m_state_mutex);
	result = bindReader();
	if (result == DRM_E_LICENSENOTFOUND)
	{
		LOG_INFO("DRM_E_LICENSENOTFOUND need Sync(need call AddKey form WebApp)\n");
		usleep(5000); /* wair 5msec, workaround code: getDecrypt() harded call form WebApp(or Opera) */
		pthread_mutex_unlock(&m_state_mutex);
		return UVA_ERROR;
	}
	if (m_state == NO_KEY)
		m_backend->sendNeedKey(init_data, init_data_size, true);
	pthread_mutex_unlock(&m_state_mutex);
	if (DRM_FAILED(result) && result != DRM_E_LICENSENOTFOUND)
		return UVA_ERROR;
	return UVA_OK;
}

DRM_RESULT PlayReadyDRMBackend::PlayReadyDRMDecrypter::bindReader()
{
	LOG_ENTRY;
	
	DRM_RESULT result = DRM_E_BUFFERTOOSMALL;
	do
	{
		result = Drm_Reader_Bind(m_context.getContext(), rights_strs, 1, (DRMPFNPOLICYCALLBACK)staticPolicyCallback, (DRM_VOID*)this, &m_decrypt_context);
		if (result == DRM_E_BUFFERTOOSMALL && m_context.resize() != UVA_OK)
			break;
		else if (result == DRM_E_RIV_TOO_SMALL && m_context.resizeRevocationBuffer() != UVA_OK)
			break;
	} while (result == DRM_E_BUFFERTOOSMALL || result == DRM_E_RIV_TOO_SMALL);

	if (DRM_SUCCEEDED(result))
	{
		result = Drm_Reader_Commit(m_context.getContext(), (DRMPFNPOLICYCALLBACK)staticPolicyCallback, (DRM_VOID*)this);
		if (DRM_SUCCEEDED(result))
			m_state = BOUND;
	}
	else if(result == DRM_E_LICENSENOTFOUND)
	{
		LOG_ENTRY;
		m_state = NO_KEY;
	}
	else if(result == DRM_E_CLK_NOT_SET)
	{
		LOG_ERROR("A license was found but could not be used as the secure clock is not set.\n");
		LOG_ENTRY;
		m_state = ERROR;
	}
	if (DRM_FAILED(result) && result != DRM_E_LICENSENOTFOUND)
	{
		LOG_ENTRY;
		m_state = ERROR;
	}
	return result;
}

DRM_RESULT PlayReadyDRMBackend::PlayReadyDRMDecrypter::staticPolicyCallback(const DRM_VOID *callbackData, DRM_DWORD callbackType, const DRM_VOID *me)
{
	if (callbackType != DRM_PLAY_OPL_CALLBACK)
		return DRM_E_INVALIDARG;
	return const_cast<PlayReadyDRMBackend::PlayReadyDRMDecrypter*>(static_cast<const PlayReadyDRMBackend::PlayReadyDRMDecrypter*>(me))->policyCallback((DRM_PLAY_OPL_EX*)callbackData);
}

DRM_RESULT PlayReadyDRMBackend::PlayReadyDRMDecrypter::policyCallback(const DRM_PLAY_OPL_EX *callbackData)
{
	//store playback permission levels here?
	return DRM_SUCCESS;
}

//virtual
void PlayReadyDRMBackend::PlayReadyDRMDecrypter::onKeyAdded(const unsigned char* init_data, size_t init_data_size)
{
	LOG_ENTRY;
	
	pthread_mutex_lock(&m_state_mutex);
	if (m_state == NO_KEY)
	{
		bindReader();
		if (m_state != NO_KEY)
			pthread_cond_signal(&m_decrypt_cond);
	}
	pthread_mutex_unlock(&m_state_mutex);
}

#define COCTAIL_LAST_DATA_COUNT 15

//virtual
UVADRMBackend::UVADRMDecrypter::DECRYPT_STATUS PlayReadyDRMBackend::PlayReadyDRMDecrypter::decryptData(const unsigned char *in, size_t in_size, unsigned char *out, size_t &out_size, const void* iv, size_t iv_size)
{
	if ((iv_size != 16 && iv_size != 8 && iv_size != 0) || out_size < in_size || in_size < 1)
		return UVADRMBackend::UVADRMDecrypter::ERROR;

	pthread_mutex_lock(&m_state_mutex);
	if (m_state == NO_KEY)
	{
		//pthread_cond_wait(&m_decrypt_cond, &m_state_mutex);
		pthread_mutex_unlock(&m_state_mutex);
		LOG_ENTRY;
		return UVADRMBackend::UVADRMDecrypter::ERROR;		
	}
	if (m_state != BOUND)
	{
		pthread_mutex_unlock(&m_state_mutex);
		LOG_ENTRY;
		return UVADRMBackend::UVADRMDecrypter::ERROR;
	}
	//pthread_mutex_unlock(&m_state_mutex);

	if (iv_size != 0)
	{
#ifdef LITTLE_ENDIAN
		const unsigned char* civ = static_cast<const unsigned char*>(iv);
		m_aes_context.qwInitializationVector = (DRM_UINT64)civ[0] << 56 | (DRM_UINT64)civ[1] << 48 | (DRM_UINT64)civ[2] << 40 | (DRM_UINT64)civ[3] << 32 | (DRM_UINT64)civ[4] << 24 | (DRM_UINT64)civ[5] << 16 | (DRM_UINT64)civ[6] << 8 | (DRM_UINT64)civ[7];
#else //LITTLE_ENDIAN
		m_aes_context.qwInitializationVector = *(DRM_UINT64*)iv;
#endif //LITTLE_ENDIAN
		if (iv_size == 16)
#ifdef LITTLE_ENDIAN
			m_aes_context.qwBlockOffset = (DRM_UINT64)civ[8] << 56 | (DRM_UINT64)civ[9] << 48 | (DRM_UINT64)civ[10] << 40 | (DRM_UINT64)civ[11] << 32 | (DRM_UINT64)civ[12] << 24 | (DRM_UINT64)civ[13] << 16 | (DRM_UINT64)civ[14] << 8 | (DRM_UINT64)civ[15];
#else //LITTLE_ENDIAN
			m_aes_context.qwBlockOffset = *(DRM_UINT64*)(iv + 8);
#endif //LITTLE_ENDIAN
		else
			m_aes_context.qwBlockOffset = 0;
		m_aes_context.bByteOffset = 0;
	}
	
	if (out != in)
		memcpy(out, in, in_size);
#ifdef  PLAY_READY_25 /* Playready 25 decrypt */
	DRM_RESULT result = Drm_Reader_InitDecrypt(&m_decrypt_context,
			in_size >= COCTAIL_LAST_DATA_COUNT ? out + in_size - COCTAIL_LAST_DATA_COUNT : out,
			in_size >= COCTAIL_LAST_DATA_COUNT ? COCTAIL_LAST_DATA_COUNT : in_size);
	if (DRM_FAILED(result))
	{
		LOG_ENTRY;
		pthread_mutex_unlock(&m_state_mutex);
		return UVADRMBackend::UVADRMDecrypter::ERROR;
	}
	
	result = Drm_Reader_Decrypt(&m_decrypt_context, &m_aes_context, (DRM_BYTE*)out, (DRM_DWORD)in_size);
	if (DRM_FAILED(result))
	{
		LOG_ENTRY;
		pthread_mutex_unlock(&m_state_mutex);
		return UVADRMBackend::UVADRMDecrypter::ERROR;
	}
#else /* 2.0 Decrypt by CSO*/
	if (iv != 0)
	{
		LOG_ASSERT(iv_size == 8 || iv_size == 16);
		memset(m_iv, 0, sizeof(m_iv));
		memcpy(m_iv, iv, iv_size);

		m_num = 0;
		memset(m_ecount, 0, sizeof(m_ecount));
	}

	{
#if 0	/* S/W(OpenSSL) Decrypt */
		int ret = AES_set_encrypt_key(cipher_ctx->cipher.cipherAES.rgbContentKey, 128, &m_key);
		if (ret != 0) {
			//m_has_key = false;
			LOG_ERROR("AES_set_encrypt_key() failed!\n");
			pthread_mutex_unlock(&m_state_mutex);
			return UVADRMBackend::UVADRMDecrypter::ERROR;
		}

		AES_ctr128_encrypt(in, out, in_size, &m_key, m_iv, m_ecount, &m_num);
		
#else	/* H/W(Nexus) Decrypt */
		DRM_CIPHER_CONTEXT *cipher_ctx=NULL;

		cipher_ctx = (DRM_CIPHER_CONTEXT *)m_decrypt_context.rgbBuffer;
		if(cipher_ctx == NULL)
		{
			LOG_ENTRY;
			pthread_mutex_unlock(&m_state_mutex);
			return UVADRMBackend::UVADRMDecrypter::ERROR;
		}	

		if(CSO_ERR_OK != CSO_UVA_DoDecrypt((uint8_t*)in, (uint8_t*)out, in_size, (uint8_t*)cipher_ctx->cipher.cipherAES.rgbContentKey, (uint8_t*)m_iv, CSO_DECRYPTOR_TYPE_AES_128_CTR))
		{
			LOG_ERROR("CSO_UVA_DoDecrypt is Failed !! \n");
			pthread_mutex_unlock(&m_state_mutex);
			return UVADRMBackend::UVADRMDecrypter::ERROR;			
		}
#endif	/* OpenSSL or Nexus*/
	}
#endif	/* S/W(Playready) or ETC(Nexus, Openssl) decrypt */

	out_size = in_size;
	pthread_mutex_unlock(&m_state_mutex);
	
	return UVADRMBackend::UVADRMDecrypter::OK;
}

//virtual
UVADRMBackend::UVADRMDecrypter::PROBE_STATUS PlayReadyDRMBackend::PlayReadyDRMDecrypter::isContentProtected(const unsigned char* in, size_t in_size)
{
	return NO;
}

#endif //PLAYREADY_SUPPORT
