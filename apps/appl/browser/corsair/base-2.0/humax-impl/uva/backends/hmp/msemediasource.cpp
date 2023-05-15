/** **********************************************************************************************************
	@file 		msemediasource.cpp
	
	@date		2013/06/17
	@author		Solution - STB Component (tstbcomp@humaxdigital.com)
	@breif		MSE Media Source
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include "msemediasource.h"
#include "cso_def.h"

#include "backend_log.h"
#include <cstring>
#include <stdio.h>

LOG_CAT(backend);

/*--------------------------------------------------------------------------------------------
 * class MseMediaSourceDataLock
 *-------------------------------------------------------------------------------------------*/
MseMediaSource::MseMediaSourceDataLock::MseMediaSourceDataLock(unsigned int source_id)
	: m_source_id(0), m_needreq_count(-1), m_needreq_retry(0)
{
	m_source_id = source_id;
}

MseMediaSource::MseMediaSourceDataLock::~MseMediaSourceDataLock()
{
}

void MseMediaSource::MseMediaSourceDataLock::lock()
{
	m_needreq_count = 1;
	//LOG_INFO("MseMediaSourceDataLock: source id[%d] request_count[%d]\n", m_source_id, m_needreq_count);
}

void MseMediaSource::MseMediaSourceDataLock::unlock()
{
	m_needreq_count = -1;
	m_needreq_retry = 0;
	//LOG_INFO("MseMediaSourceDataLock: source id[%d] request_count[%d]\n", m_source_id, m_needreq_count);
}

bool MseMediaSource::MseMediaSourceDataLock::isLocking()
{
	if ( ++m_needreq_retry >= MAX_DATALOCK_RETRY )
	{
		LOG_INFO("MseMediaSourceDataLock: source id[%d]: retry overflow.\n", m_source_id);
		unlock();
	}

	return ( 0 >= m_needreq_count ) ? false : true;
}

/*--------------------------------------------------------------------------------------------
 * class MseMediaSource
 *-------------------------------------------------------------------------------------------*/
MseMediaSource::MseMediaSource(const char *mime_type, unsigned int id, eMediaType type)
	: MediaSource(mime_type), m_id(id), m_media_type(type)
{
	if ( strstr(mime_type, "h264") )
		m_codec = MseMediaSource::H264;
	else if ( strstr(mime_type, "aac") )
		m_codec = MseMediaSource::AAC;
	else
		m_codec = MseMediaSource::NONE;

	m_source_type = MSE;

	m_source_lock = new MseMediaSourceDataLock(id);

	LOG_INFO("MseMediaSource: mime_type[%s], source_id=[%d], type=[%d], codec=%d\n",
				mime_type, m_id, m_media_type, m_codec);
}

MseMediaSource::~MseMediaSource()
{
	if(m_decryption_context.m_last_kid)
		UVA_DELETE_ARRAY(m_decryption_context.m_last_kid);

	if ( m_source_lock )
	{
		delete m_source_lock;
		m_source_lock = NULL;
	}
}

unsigned int MseMediaSource::getID()
{
	return m_id;
}

MseMediaSource::eCodec MseMediaSource::getCodec()
{
	LOG_INFO("codec=%d\n", m_codec);
	return m_codec;
}

MseMediaSource::eMediaType MseMediaSource::getMediaType()
{
	LOG_INFO("type=%d\n", m_media_type);
	return m_media_type;
}

void *MseMediaSource::getSourceBuffer()
{
	return m_source_buffer;
}

void MseMediaSource::setSourceBuffer(void* source_buffer)
{
	m_source_buffer = source_buffer;
}

/*
 * private
 */
MseMediaSource::eMediaType MseMediaSource::__type(eCodec codec)
{
	eMediaType mt = UNKNOWN;

	switch ( codec )
	{
	case H264:
	case VC1:
	case MPEG2:
	case MPEG4:
	case THEORA:
	case VP8:
		mt = VIDEO;
		break;
	case AAC:
	case MP3:
	case PCM:
	case VORBIS:
		mt = AUDIO;
		break;
	default:
		break;
	}

	return mt;
}

MseMediaSource::eCodec MseMediaSource::__codec(const char *codec)
{
	eCodec c = NONE;

	if ( !codec ) return c;

	/* dangerous:: */
	if ( strstr(codec, "avc1") ) c = H264;
	else if ( strstr(codec, "mp4a") ) c = AAC;

	return c;
}

// from uva client
void MseMediaSource::unlock_NeedMoreData(void)
{
	m_source_lock->unlock();
}

// from backend(cso)
void MseMediaSource::lock_NeedMoreData(void)
{
	m_source_lock->lock();
}

bool MseMediaSource::isLock_NeedMoreData(void)
{
	return m_source_lock->isLocking();
}
