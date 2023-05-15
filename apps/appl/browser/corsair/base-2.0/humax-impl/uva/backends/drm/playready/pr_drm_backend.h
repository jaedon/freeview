/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#ifndef _PR_DRM_BACKEND_H_
#define _PR_DRM_BACKEND_H_

#ifdef PLAYREADY_SUPPORT

#include "../managed_drm_backend.h"
#include <list>
#include <pthread.h>
#include "drmmanager.h"
#include "oemaescommon.h"

#include <openssl/evp.h>
#include <openssl/aes.h>


class PlayReadyDRMBackend : public UVAManagedDRMBackend
{
public:
	class PlayReadyDRMDecrypter;

	PlayReadyDRMBackend(UVADRMClient *client);
	~PlayReadyDRMBackend();

	UVA_STATUS sendNeedKey(const unsigned char* init_data, size_t init_data_size, bool noKey);

//UVADRMBackend
	virtual const char* getId() const;

	virtual const char* getOIPFId() const;

	virtual UVA_STATUS generateKeyRequest(const unsigned char* init_data, size_t init_data_size);

	virtual UVA_STATUS addKey(const unsigned char* key, size_t key_size, const unsigned char* init_data, size_t init_data_size, const char* session_id);

	virtual UVA_STATUS cancelKeyRequest(const char* session_id);

	virtual UVA_STATUS sendDRMMessage(const char *msg_type, const char *msg, char *&session_id);

	virtual UVA_STATUS getKeyReleases();

	virtual UVA_STATUS addKeyReleaseCommit(const unsigned char* message, size_t message_size, const char* session_id);

	virtual UVA_STATUS onInitData(const unsigned char* init_data, size_t init_data_size);

protected:
	const unsigned char* parseInitData(const unsigned char*& init_data, size_t& init_data_size, size_t& pssh_data_size);

private:
//UVAManagedDRMBackend
	virtual UVA_STATUS constructDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter*& decrypter, bool automatic_key_aquasition);

	virtual UVA_STATUS destroyDecrypter(UVAManagedDRMBackend::UVAManagedDRMDecrypter* decrypter);


private:

	class DRMAppContext
	{
	public:
		DRMAppContext();
		~DRMAppContext();

		UVA_STATUS init();
		UVA_STATUS resize();
		UVA_STATUS resizeRevocationBuffer();
		DRM_APP_CONTEXT* getContext() { return context;}
	private:
		DRM_APP_CONTEXT *context;
		DRM_VOID *oem;
		DRM_BYTE *buffer;
		DRM_DWORD buffer_size;
		DRM_BYTE *revocation_buffer;
		DRM_DWORD revocation_buffer_size;
	};

	struct KeyExchangeSession
	{
		char *m_sessionId;

		enum SessionState
		{
			KEY_NONE, //no action has been taken yet
			KEY_CHALLENGE, //key challenge was generated in response to generate key request
			KEY_ADDED, //key was added and challenge ack response was sent as keymessage event
			KEY_SUCCESS, //challenge ack response was received and processed
			KEY_ERROR //error occurred in this session
		} m_state;

		DRMAppContext m_drm_app_context;

		DRM_CHAR *m_la_url;

		DRM_RESULT m_result;

		KeyExchangeSession() : m_sessionId(NULL), m_state(KEY_NONE), m_la_url(NULL), m_result(DRM_SUCCESS) {}

		~KeyExchangeSession()
		{
			if (m_sessionId)
				UVA_DELETE_ARRAY(m_sessionId);
			if (m_la_url)
				UVA_DELETE_ARRAY(m_la_url);
		}
	};

	static DRM_RESULT staticLicenseResponseCallback(DRM_VOID *pvData, DRM_STORE_LICENSE_CALLBACK_ENUM eReason, DRM_VOID *me);
	DRM_RESULT licenseResponseCallback(DRM_VOID *pvData, DRM_STORE_LICENSE_CALLBACK_ENUM eReason);

	UVA_STATUS createSession(KeyExchangeSession *&session);
	KeyExchangeSession* getSession(const char *sessionId);

	std::list<KeyExchangeSession*, UVA_ALLOCATOR<KeyExchangeSession*> > m_sessions;

	static unsigned int sessionNum;

	UVADRMClient *m_client;

public:
//UVADRMBackend
	class PlayReadyDRMDecrypter : public UVAManagedDRMBackend::UVAManagedDRMDecrypter
	{
	public:
		PlayReadyDRMDecrypter(PlayReadyDRMBackend* backend, bool automatic_key_aquasition);

		virtual ~PlayReadyDRMDecrypter();

	//UVAManagedDRMBackend::UVAManagedDRMDecrypter
		virtual void onKeyAdded(const unsigned char* init_data, size_t init_data_size);

	//UVADRMBackend::UVADRMDecrypter
		//Initializes the decrypter with given data
		virtual UVA_STATUS init(const unsigned char* init_data, size_t init_data_size);

		//Decrypts block of the data.
		virtual DECRYPT_STATUS decryptData(const unsigned char* in, size_t in_size, unsigned char* out, size_t& out_size, const void* iv, size_t iv_size);

		//Probes if the content is the content that this decrypter supports.
		//Customer impl: when returning YES getDecryptionKey could be called
		//If everything is ok on drm backend side this would set decryption key so decryption can proceed
		//or send event to the drm client (key needed). Different clients can respond differently:
		// OOIF will raise drmRightsErrorMessage and stop backend, html5 will send needKey event
		virtual PROBE_STATUS isContentProtected(const unsigned char* in, size_t in_size);

		virtual void stopDecrypting();

	private:
		DRM_RESULT bindReader();

		static DRM_RESULT staticPolicyCallback(const DRM_VOID *callbackData, DRM_DWORD callbackType, const DRM_VOID *me);

		DRM_RESULT policyCallback(const DRM_PLAY_OPL_EX *callbackData);

		PlayReadyDRMBackend* m_backend;

		DRMAppContext m_context;

		DRM_DECRYPT_CONTEXT m_decrypt_context;

		DRM_AES_COUNTER_MODE_CONTEXT m_aes_context;

		enum
		{
			NOT_BOUND,
			NO_KEY,
			BOUND,
			ERROR
		} m_state;
		pthread_mutex_t m_state_mutex;

		pthread_cond_t m_decrypt_cond;

		AES_KEY m_key;
		unsigned char m_iv[16];
		unsigned char m_ecount[16];
		unsigned m_num;
	};

};

#endif //PLAYREADY_SUPPORT

#endif //_PR_DRM_BACKEND_H_
