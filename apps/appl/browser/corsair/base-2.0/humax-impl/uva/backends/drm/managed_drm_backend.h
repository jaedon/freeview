/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#ifndef _MANAGED_DRM_BACKEND_H_
#define _MANAGED_DRM_BACKEND_H_

#include "uva_backend.h"
#include <pthread.h>
#include <list>
#include "stdio.h"

class UVAManagedDRMBackend : public UVADRMBackend
{
public:
	UVAManagedDRMBackend();
	virtual ~UVAManagedDRMBackend();

	/**
	 * Creates decrypter that can be used in media stack to decrypt encrypted content.
	 *
	 * The implementation is thread safe. It assures that same decrypter
	 * is return for same init data. Decrypters are reference counted.
	 *
	 * \param decrypter pointer to decrypter that will be created
	 * \param automatic_key_aquasition flag specifing if decrypter should perform
	 * automatic key/license acquasition for content it is working with (if available)
	 * \return UVA_OK on success, other value otherwise
	 */
	virtual UVA_STATUS getDecrypter(UVADRMDecrypter*& decrypter, const unsigned char* init_data, size_t init_data_size, bool automatic_key_aquasition);

	/**
	 * Release decrypter previously requested from this DRM backend.
	 *
	 * The implementation keeps track of how many times same decrypter was requested
	 * and destroyes it only if it is unused.
	 *
	 * \param decrypter decrypter to be released
	 * \return UVA_OK on success, other value otherwise
	 */
	virtual UVA_STATUS releaseDecrypter(UVADRMDecrypter* decrypter);

	/**
	 * Stops all decoders created by this backend.
	 *
	 * Will be called prior to destruction of media player.
	 * May be used to prevent possible deadlocks when decrypter would be waiting for keys.
	 */
	virtual void stopDecrypters();

	class UVAManagedDRMDecrypter : public UVADRMBackend::UVADRMDecrypter
	{
	public:
		virtual ~UVAManagedDRMDecrypter() {}
		/**
		 * Notification that new key was added by DRM backend.
		 *
		 * Data with which this decrypter was inited is passed for convenience
		 * to ease up key lookup.
		 *
			 * \param init_data initialization data specific to this backend decrypter.
			 * \param init_data_size initialization data size.
		 */
		virtual void onKeyAdded(const unsigned char* init_data, size_t init_data_size) = 0;
	};

protected:

	/**
	 * Sends notification to all decrypters that new key was added.
	 */
	void notifyKeyAdded();

private:
	/**
	 * Constructs decrypter that can be used in media stack to decrypt encrypted content.
	 *
	 * \param decrypter pointer to decrypter that will be created
	 * \param automatic_key_aquasition flag specifing if decrypter should perform
	 * automatic key/license acquasition for content it is working with (if available)
	 * \return UVA_OK on success, other value otherwise
	 */
	virtual UVA_STATUS constructDecrypter(UVAManagedDRMDecrypter*& decrypter, bool automatic_key_aquasition) = 0;

	/**
	 * Destroys decrypter that can be used in media stack to decrypt encrypted content.
	 *
	 * \param decrypter pointer to decrypter that will be created
	 * \param automatic_key_aquasition flag specifing if decrypter should perform
	 * automatic key/license acquasition for content it is working with (if available)
	 * \return UVA_OK on success, other value otherwise
	 */
	virtual UVA_STATUS destroyDecrypter(UVAManagedDRMDecrypter* decrypter) = 0;

	/**
	 * List entry holding info about created decrypter
	 */
	struct ListEntry
	{
		unsigned char* m_init_data;
		size_t m_init_data_size;
		unsigned int m_refs;
		UVAManagedDRMBackend::UVAManagedDRMDecrypter* m_decrypter;

		ListEntry() : m_init_data(NULL), m_init_data_size(0), m_refs(0), m_decrypter(0) {}

		~ListEntry()
		{
			if (m_init_data)
				uva_free(m_init_data);
		}
	};

	/**
	 * Locks the mutex for the lifetime of the instance.
	 * */
	class AutoMutex
	{
	public:
		/**
		 * Locks the mutex.
		 **/
		AutoMutex(pthread_mutex_t* mutex) : mutex(mutex) { pthread_mutex_lock(mutex); }

		/**
		 * Unlocks the mutex.
		 **/
		~AutoMutex() { pthread_mutex_unlock(mutex); }
	private:
		/**
		 * Copying is forbidden
		 **/
		AutoMutex(const AutoMutex& other);

		/**
		 * Assignment is forbidden
		 **/
		AutoMutex& operator=(const AutoMutex& am);

		pthread_mutex_t* mutex; ///<Mutex to lock
	};

	pthread_mutex_t m_mutex; ///< Access mutex for thread safety
	std::list<ListEntry, UVA_ALLOCATOR<ListEntry> > m_decrypters; ///< List holding created decrypters and info about them
};

#endif //_MANAGED_DRM_BACKEND_H_
