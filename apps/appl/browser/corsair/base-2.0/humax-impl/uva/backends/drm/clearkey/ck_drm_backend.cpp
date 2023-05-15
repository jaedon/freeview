/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#ifdef CLEARKEY_SUPPORT

#include "ck_drm_backend.h"
#include "uva_event.h"
#include "uva_client.h"
#include "drm_constants.h"
#include "uva_event.h"
#include "backend_log.h"
#include <sys/time.h>

#include <fcntl.h>
#include <unistd.h>

LOG_CAT(drm);

#define SESSION_ID_LENGTH 8

static unsigned short __urandom(void)
{
	int fd = open("/dev/urandom", O_RDONLY);
	unsigned short r = 0;
	ssize_t ret = -1;

	if (fd < 0) {
		return 0;
	}

	ret = read(fd, &r, sizeof (r));

	if (fd >= 0) close(fd);

	return (unsigned short)r;
}

ClearKeyDRMBackend::BinArray::BinArray(const unsigned char* array, size_t size)
	: size(size)
	, array(UVA_NEW_ARRAY(unsigned char, size))
{
	if (this->array)
		memcpy(this->array, array, size);
	else throw std::bad_alloc();
}

ClearKeyDRMBackend::BinArray::~BinArray()
{
	if (array)
		UVA_DELETE_ARRAY(array);
}

ClearKeyDRMBackend::BinArray::BinArray(const BinArray& other)
	: size(other.size)
	, array(UVA_NEW_ARRAY(unsigned char, size))
{
	if (this->array)
		memcpy(this->array, other.array, size);
	else throw std::bad_alloc();
}

ClearKeyDRMBackend::BinArray& ClearKeyDRMBackend::BinArray::operator=(const BinArray& other)
{
	if (array)
		UVA_DELETE_ARRAY(array);
	array = UVA_NEW_ARRAY(unsigned char, other.size);
	if(!array)
		throw std::bad_alloc();
	memcpy(array, other.array, other.size);
	size = other.size;
	return *this;
}

bool ClearKeyDRMBackend::BinArray::operator<(const BinArray& other) const
{
	LOG_ASSERT(array);
	LOG_ASSERT(other.array);
	if(size == other.size)
		return (memcmp(array, other.array, size) < 0);
	return (size < other.size);
}

ClearKeyDRMBackend::ClearKeyDRMBackend(UVADRMClient *client) : m_client(client)
{
	LOG_ENTRY;
}

ClearKeyDRMBackend::~ClearKeyDRMBackend()
{
	LOG_ENTRY;

	while ( !m_sessions.empty() )
	{
		UVA_DELETE((ClearKeySession*)m_sessions.front());
		m_sessions.pop_front();
	}
}


//virtual
const char* ClearKeyDRMBackend::getId() const
{
	return clearkey_EME_ID;
}

//virtual
const char* ClearKeyDRMBackend::getOIPFId() const
{
	return clearkey_OIPF_ID;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::generateKeyRequest(const unsigned char* init_data, size_t init_data_size)
{
	LOG_ENTRY;

	ClearKeySession *session = NULL;

	if ( createSession(session) != UVA_OK )
	{
		LOG_ERROR("create session fail!!!\n");
		return UVA_ERROR;
	}

	UVAKeyMessageDRMEvent *event = UVA_NEW(UVAKeyMessageDRMEvent, (getId(), session->session_id, init_data, init_data_size, NULL, true));
	if (!event) {
		return UVA_ERROR;
	}
	m_client->handleDRMEvent(event);
	return UVA_OK;
}

ClearKeyDRMBackend::BinArray* ClearKeyDRMBackend::getKeyForInitData(const ClearKeyDRMBackend::BinArray& init)
{
	KeyArray::iterator ret = m_key_init_mapping.find(init);
	if (ret != m_key_init_mapping.end())
		return &((*ret).second);
	else
		return NULL;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::addKey(const unsigned char* key, size_t key_size, const unsigned char* init_data, size_t init_data_size, const char* session_id)
{
	ClearKeySession* session = NULL;

	if ( session_id )
	{
		session = getSession(session_id);
		if ( session == NULL )
		{
			LOG_ERROR("this session(%s) is unrecognized.\n", session_id);
			return UVA_NOT_INITIALIZED;
		}
	}
	else
	{
		if ( createSession(session) != UVA_OK )
		{
			LOG_ERROR("create session fail!!!\n");
			return UVA_ERROR;
		}
	}
#ifdef DEBUG
	LOG_INFO("KID (kid size = %zu bytes): ", init_data_size);
	for (int i = 0; i < (int)init_data_size; i++) {
		fprintf(stderr, "0x%02x ", (int)init_data[i]);
	}
	fprintf(stderr, "\n");

	LOG_INFO("KEY (key size = %zu bytes): ", key_size);
	for (int i = 0; i < (int)key_size; i++) {
		fprintf(stderr, "0x%02x ", (int)key[i]);
	}
	fprintf(stderr, "\n");
#endif //DEBUG

	try {
		m_key_init_mapping.erase(BinArray(init_data, init_data_size));
		m_key_init_mapping.insert(std::pair<BinArray, BinArray>(BinArray(init_data, init_data_size), BinArray(key, key_size)));

		UVADRMEvent *event = UVA_NEW(UVADRMEvent, (UVADRMEvent::KEY_COMPLETE, getId(), session_id, true));
		if (!event) {
			return UVA_ERROR;
		}
		m_client->handleDRMEvent(event);
		notifyKeyAdded();
		session->session_state = 1;		// keyadded event has been fired
	} catch (std::bad_alloc) {
		return UVA_ERROR;
	}
	return UVA_OK;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::cancelKeyRequest(const char* session_id)
{
	/*
	 * If sessionId is not null and is unrecognized or not mapped to the keySystem, throw an INVALID_ACCESS_ERR.
	 * If a keyadded event has already been fired for this sessionId, throw an INVALID_STATE_ERR.
	 * Clear any internal state associated with the sessionId (or if this is null with the keySystem for this media element). This sessionId will now be unrecognized.
	 */

	ClearKeySession *session = getSession(session_id);

	if ( session )
	{
		if ( session->session_state )
		{
			LOG_ERROR("keyadded event has already been fired.\n");
			return UVA_ERROR;
		}

		UVA_DELETE(session);
		m_sessions.remove(session);

		return UVA_OK;
	}
	else
	{
		LOG_ERROR("this session(%s) is unrecognized.\n", session_id);
		return UVA_NOT_SUPPORTED;
	}
}

//virtual
UVA_STATUS ClearKeyDRMBackend::sendDRMMessage(const char *msg_type, const char *msg, char *&session_id)
{
	// OIPF specific
	return UVA_NOT_SUPPORTED;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::destroyDecrypter(UVAManagedDRMDecrypter* decrypter)
{
	LOG_ENTRY;
	BinArray& d_init = static_cast<ClearKeyDRMDecrypter*>(decrypter)->getId();
	m_key_init_mapping.erase(d_init);
	UVA_DELETE(decrypter);
	return UVA_OK;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::getKeyReleases()
{
	LOG_ENTRY;
	return UVA_NOT_SUPPORTED;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::addKeyReleaseCommit(const unsigned char* message, size_t message_size, const char* session_id)
{
	LOG_ENTRY;
	return UVA_NOT_SUPPORTED;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::onInitData(const unsigned char* init_data, size_t init_data_size)
{
	LOG_ENTRY;
	return UVA_OK;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::constructDecrypter(UVAManagedDRMDecrypter*& decrypter, bool automatic_key_aquasition)
{
	LOG_ENTRY;

		decrypter = UVA_NEW(ClearKeyDRMDecrypter, (this, automatic_key_aquasition));
	if (!decrypter) {
		return UVA_ERROR;
	}
	return UVA_OK;
}

UVA_STATUS ClearKeyDRMBackend::createSession(ClearKeySession *&session)
{
	struct timeval tp;

	session = UVA_NEW(ClearKeySession, ());
	if ( !session )
	{
		return UVA_ERROR;
	}

	session->session_id = UVA_NEW_ARRAY(char, SESSION_ID_LENGTH);
	if ( !session->session_id )
	{
		UVA_DELETE(session);
		return UVA_ERROR;
	}

	gettimeofday(&tp, NULL);
	srandom(tp.tv_sec | tp.tv_usec);

	// make random 8-digit numerical string
	snprintf(session->session_id, SESSION_ID_LENGTH+1, "%d", (int)__urandom());

	session->session_state = 0;

	m_sessions.push_back(session);

	return UVA_OK;
}

ClearKeyDRMBackend::ClearKeySession* ClearKeyDRMBackend::getSession(const char *sessionId)
{
	std::list<ClearKeySession *>::iterator it;
	for (it = m_sessions.begin(); it != m_sessions.end(); it++)
	{
		if ( strncmp((*it)->session_id, sessionId, SESSION_ID_LENGTH) == 0 )
		{
			return (*it);
		}
	}

	return NULL;
}

ClearKeyDRMBackend::ClearKeyDRMDecrypter::ClearKeyDRMDecrypter(ClearKeyDRMBackend* backend, bool automatic_key_aquasition):
		m_init(), m_backend(backend), m_num(0), m_has_key(false)
{
	LOG_ENTRY;
}

// virtual
void ClearKeyDRMBackend::ClearKeyDRMDecrypter::onKeyAdded(const unsigned char* init_data, size_t init_data_size)
{
	LOG_ASSERT(m_backend);
	LOG_ASSERT(init_data_size == m_init.size);
	LOG_ASSERT(memcmp(init_data, m_init.array, m_init.size) == 0);

	BinArray* key = m_backend->getKeyForInitData(m_init);
	if (key)
	{
		LOG_INFO("setting key (size=%zu)\n", key->size);
		LOG_ASSERT(key->size == 16);
		int ret = AES_set_encrypt_key(key->array, 128, &m_key);
		if (ret != 0) {
			m_has_key = false;
			LOG_ERROR("AES_set_encrypt_key() failed!\n");
		} else {
			m_has_key = true;
		}
	}
}

ClearKeyDRMBackend::ClearKeyDRMDecrypter::~ClearKeyDRMDecrypter()
{
	LOG_ENTRY;
}

//virtual
UVA_STATUS ClearKeyDRMBackend::ClearKeyDRMDecrypter::init(const unsigned char *init_data, size_t init_data_size)
{
	LOG_ENTRY;
	LOG_ASSERT(!m_init.array);
	LOG_ASSERT(m_backend);

	m_init.array = UVA_NEW_ARRAY(unsigned char, init_data_size);
	if (!m_init.array) {
		return UVA_ERROR;
	}
	memcpy(m_init.array, init_data, init_data_size);
	m_init.size = init_data_size;

	onKeyAdded(init_data, init_data_size);
	if (!m_has_key)
		m_backend->generateKeyRequest(init_data, init_data_size);

	return UVA_OK;
}

//virtual
void ClearKeyDRMBackend::ClearKeyDRMDecrypter::stopDecrypting()
{
}

//virtual
UVADRMBackend::UVADRMDecrypter::DECRYPT_STATUS ClearKeyDRMBackend::ClearKeyDRMDecrypter::decryptData(
		const unsigned char *in, size_t in_size,
		unsigned char *out, size_t &out_size,
		const void *iv, size_t iv_size)
{
	//LOG_INFO("in_size=%lu iv_size=%lu iv=%p m_num=%u\n", in_size, iv_size, iv, m_num);

	if (!m_has_key)
		return ERROR;

	if (iv != 0) {
		LOG_ASSERT(iv_size == 8 || iv_size == 16);

		m_num = 0;
		memset(m_ecount, 0, sizeof(m_ecount));
		memset(m_iv, 0, sizeof(m_iv));
		memcpy(m_iv, iv, iv_size);
	}

	/*
	  SUPER SAFE VERSION
	unsigned char buf[in_size + 256];
	AES_ctr128_encrypt(in, buf, in_size, &m_key, m_iv, m_ecount, &m_num);
	memcpy(out, buf, in_size);
	*/
	AES_ctr128_encrypt(in, out, in_size, &m_key, m_iv, m_ecount, &m_num);

	out_size = in_size;

	return OK;
}

//virtual
UVADRMBackend::UVADRMDecrypter::PROBE_STATUS ClearKeyDRMBackend::ClearKeyDRMDecrypter::isContentProtected(const unsigned char* in, size_t in_size)
{
	return NO;
}

#endif //CLEARKEY_SUPPORT
