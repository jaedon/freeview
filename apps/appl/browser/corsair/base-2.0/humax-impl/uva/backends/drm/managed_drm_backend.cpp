/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include <list>

#include "managed_drm_backend.h"
#include "backend_log.h"

// DRMKEY_SYNC work around
/////////////////////////////////////////////////
#include "oplvodhtml5handle.h"

LOG_CAT(drm);

UVAManagedDRMBackend::UVAManagedDRMBackend()
{
	LOG_ENTRY;
	// DRMKEY_SYNC work around
	/////////////////////////////////////////////////	
	OplVodHtml5Handle::m_need_key = true;
	pthread_mutex_init(&m_mutex, NULL);
}

UVAManagedDRMBackend::~UVAManagedDRMBackend()
{
	LOG_ENTRY;
	// DRMKEY_SYNC work around
	/////////////////////////////////////////////////
	OplVodHtml5Handle::m_need_key = false;
	while(m_decrypters.size() > 0)
	{
		UVA_DELETE((UVAManagedDRMDecrypter *)m_decrypters.front().m_decrypter);
		m_decrypters.pop_front();
	}	

	LOG_ASSERT(m_decrypters.empty());
	pthread_mutex_destroy(&m_mutex);
}

//virtual
UVA_STATUS UVAManagedDRMBackend::getDecrypter(UVADRMDecrypter*& decrypter, const unsigned char* init_data, size_t init_data_size, bool automatic_key_aquasition)
{
	//LOG_ENTRY;
	
	if (!init_data || !init_data_size)
		return UVA_ERROR;

#ifdef DEBUG
	{
		static unsigned char s_init_data[16] = {0,};
		if(memcmp(s_init_data, init_data, 16))
		{
			LOG_INFO("OLD KID: ");
			for (int i = 0; i < (int)16; i++) {
				fprintf(stderr, "%.2x",s_init_data[i]);
			}
			fprintf(stderr, "\n");
		
			LOG_INFO("NEW KID: ");
	for (int i = 0; i < (int)init_data_size; i++) {
		fprintf(stderr, "%.2x",init_data[i]);
	}
			memcpy(s_init_data, init_data, 16);
	fprintf(stderr, "\n");
		
		}
	}
#endif //DEBUG

	AutoMutex am(&m_mutex);

	std::list<ListEntry, UVA_ALLOCATOR<ListEntry> >::iterator it = m_decrypters.begin();
	while (it != m_decrypters.end())
	{
		if (it->m_init_data_size == init_data_size && !memcmp(init_data, it->m_init_data, init_data_size))
		{
			it->m_refs++;
			decrypter = it->m_decrypter;
			return UVA_OK;
		}
		it++;
	}

	ListEntry entry;
	entry.m_init_data = (unsigned char*)uva_malloc(init_data_size);
	if (!entry.m_init_data)
		return UVA_ERROR;

	entry.m_init_data_size = init_data_size;
	memcpy(entry.m_init_data, init_data, init_data_size);

	UVA_STATUS status = constructDecrypter(entry.m_decrypter, automatic_key_aquasition);
	if (status != UVA_OK)
	{
		decrypter = NULL;
		return UVA_ERROR;
	}

	status = entry.m_decrypter->init(init_data, init_data_size);
	if (status == UVA_OK)
	{
		try
		{
			entry.m_refs = 1;
			LOG_INFO("getDecrypter(%d) entry->m_refs = %d,  entry.m_decrypter= %x\n", __LINE__, entry.m_refs, entry.m_decrypter);
			m_decrypters.push_back(entry);
		}
		catch (...)
		{
			status = UVA_ERROR;
		}
	}
	if (status != UVA_OK)
	{
		destroyDecrypter(entry.m_decrypter);
		entry.m_decrypter = NULL;
		uva_free(entry.m_init_data);
	}
	//Pointer to init data in entry was either copied to the one in list
	//or data was deleted in case of error,
	//null it here to avoid double delete when we exit function.
	entry.m_init_data = NULL;
	decrypter = entry.m_decrypter;
	return status;
}

//virtual
UVA_STATUS UVAManagedDRMBackend::releaseDecrypter(UVADRMDecrypter* decrypter)
{
	//LOG_ENTRY;
	
	AutoMutex am(&m_mutex);

	UVA_STATUS status = UVA_OK;
	std::list<ListEntry, UVA_ALLOCATOR<ListEntry> >::iterator it = m_decrypters.begin();
	while (it != m_decrypters.end())
	{
		if (it->m_decrypter == decrypter)
		{
			it->m_refs--;
			
			if (it->m_refs == 0)
			{
				LOG_INFO("releaseDecrypter(%d) it->m_refs = %d, decrypter = %x\n", __LINE__, it->m_refs, decrypter);
				status = destroyDecrypter(it->m_decrypter);
				m_decrypters.erase(it);
				break;
			}
		}
		it++;
	}
	return status;
}

void UVAManagedDRMBackend::notifyKeyAdded()
{
	LOG_ENTRY;
	// DRMKEY_SYNC work around
	/////////////////////////////////////////////////	
	OplVodHtml5Handle::m_need_key = false;
	
	AutoMutex am(&m_mutex);

	std::list<ListEntry, UVA_ALLOCATOR<ListEntry> >::iterator it = m_decrypters.begin();
	while (it != m_decrypters.end())
	{
		it->m_decrypter->onKeyAdded(it->m_init_data, it->m_init_data_size);
		it++;
	}
}

//virtual
void UVAManagedDRMBackend::stopDecrypters()
{
	AutoMutex am(&m_mutex);

	std::list<ListEntry, UVA_ALLOCATOR<ListEntry> >::iterator it = m_decrypters.begin();
	while (it != m_decrypters.end())
	{
		it->m_decrypter->stopDecrypting();
		it++;
	}
}
