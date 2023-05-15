/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#ifndef _CK_DRM_BACKEND_H_
#define _CK_DRM_BACKEND_H_

#ifdef CLEARKEY_SUPPORT

#include "../managed_drm_backend.h"
#include <uva_backend.h>
#include <map>
#include <utility>
#include <openssl/evp.h>
#include <openssl/aes.h>

class ClearKeyDRMBackend : public UVAManagedDRMBackend
{
	struct BinArray {
		size_t size; // Do not change the order!
		unsigned char* array;

		BinArray() : size(0), array(NULL) { }
		BinArray(const unsigned char* array, size_t size);
		BinArray(const BinArray& other);
		BinArray& operator=(const BinArray& other);
		bool operator<(const BinArray& other) const;
		~BinArray();
	};

	struct ClearKeySession {
		char * session_id;	// The sessionId string is numerical
		int session_state;	// 1 means keyadded event has been fired, 0 means not

		ClearKeySession() : session_id(NULL), session_state(0) { }
		~ClearKeySession()
		{
			if ( session_id )
			{
				UVA_DELETE_ARRAY(session_id);
				session_id = NULL;
			}
		}
	};
	
public:
	class ClearKeyDRMDecrypter;

	ClearKeyDRMBackend(UVADRMClient *client);
	virtual ~ClearKeyDRMBackend();

	//UVADRMBackend
	virtual const char* getId() const;
	virtual const char* getOIPFId() const;
	virtual UVA_STATUS generateKeyRequest(const unsigned char* init_data, size_t init_data_size);
	virtual UVA_STATUS addKey(const unsigned char* key, size_t key_size, const unsigned char* init_data, size_t init_data_size, const char* session_id);
	virtual UVA_STATUS cancelKeyRequest(const char* session_id);
	virtual UVA_STATUS getKeyReleases();
	virtual UVA_STATUS addKeyReleaseCommit(const unsigned char* message, size_t message_size, const char* session_id);
	virtual UVA_STATUS sendDRMMessage(const char *msg_type, const char *msg, char *&session_id);
	virtual UVA_STATUS onInitData(const unsigned char* init_data, size_t init_data_size);

	// ManagedDrmBackend
	virtual UVA_STATUS constructDecrypter(UVAManagedDRMDecrypter*& decrypter, bool automatic_key_aquasition);
	virtual UVA_STATUS destroyDecrypter(UVAManagedDRMDecrypter* decrypter);

	class ClearKeyDRMDecrypter : public UVAManagedDRMDecrypter
	{
	public:
		ClearKeyDRMDecrypter(ClearKeyDRMBackend* backend, bool automatic_key_aquasition);

		virtual ~ClearKeyDRMDecrypter();

		virtual void onKeyAdded(const unsigned char* init_data, size_t init_data_size);

		//UVADRMBackend::UVADRMDecrypter
		//Initializes the decrypter with given data
		virtual UVA_STATUS init(const unsigned char* init_data, size_t init_data_size);

		//Decrypts block of the daconstructDecrypterta.
		virtual DECRYPT_STATUS decryptData(const unsigned char* in, size_t in_size, unsigned char* out, size_t& out_size, const void* iv, size_t iv_size);

		//Probes if the content is the content that this decrypter supports.
		//Customer impl: when returning YES getDecryptionKey could be called
		//If everything is ok on drm backend side this would set decryption key so decryption can proceed
		//or send event to the drm client (key needed). Different clients can respond differently:
		// OOIF will raise drmRightsErrorMessage and stop backend, html5 will send needKey event
		virtual PROBE_STATUS isContentProtected(const unsigned char* in, size_t in_size);

		//Sets initialization vector for this decrypter.
		//virtual UVA_STATUS setIV(void* iv, size_t iv_size);

		//In multi threaded backend can be used to prevent deadlocks when shutting down player.
		virtual void stopDecrypting();

		BinArray& getId() { return m_init; }

	private:
		BinArray m_init;

		ClearKeyDRMBackend* m_backend;

		// AES 128-bit CTR related context
		AES_KEY m_key;
		unsigned char m_iv[16];
		unsigned char m_ecount[16];
		unsigned m_num;
		bool m_has_key;
	};

private:
	BinArray* getKeyForInitData(const BinArray& init);
	UVADRMClient *m_client;

	// Mapping between init_data (key) and encryption key (value).
	typedef std::map<BinArray, BinArray, std::less<BinArray>, UVA_ALLOCATOR<std::pair<BinArray,BinArray> > > KeyArray;
	KeyArray m_key_init_mapping;

	std::list<ClearKeySession *> m_sessions;

	UVA_STATUS createSession(ClearKeySession *&session);
	ClearKeySession* getSession(const char *sessionId);
};

#endif // CLEARKEY_SUPPORT

#endif //_CK_DRM_BACKEND_H_
