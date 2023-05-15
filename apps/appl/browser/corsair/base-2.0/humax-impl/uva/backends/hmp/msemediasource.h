/** **********************************************************************************************************
	@file 		msemediasource.h
	
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

#ifndef __MSE_MEDIA_SOURCE_H__
#define __MSE_MEDIA_SOURCE_H__

#include "mediasource.h"

#include <uva_backend.h>

/*--------------------------------------------------------------------------------------------
 * class MseMediaSource
 *-------------------------------------------------------------------------------------------*/
class MseMediaSource : public MediaSource {
public:
	enum eMediaType {
		UNKNOWN		= 0,
		VIDEO,
		AUDIO,
		COMBINED
	};

	enum eCodec {
		NONE		= 0,
		// video
		H264		= 0x10,
		VC1,
		MPEG2,
		MPEG4,
		THEORA,
		VP8,
		// audio
		AAC			= 0x20,
		MP3,
		PCM,
		VORBIS
	};

	class DecryptionContext {
	public:
		DecryptionContext() : m_last_kid(NULL), m_last_kid_ptr(NULL), m_decrypter(NULL) {}

	public:
		unsigned char* m_last_kid;
		const unsigned char* m_last_kid_ptr;
		UVADRMBackend::UVADRMDecrypter* m_decrypter;
	};

private:
	class MseMediaSourceDataLock {
	public:
		enum {
			MAX_DATALOCK_RETRY = 10
		};

		MseMediaSourceDataLock(unsigned int source_id);
		virtual ~MseMediaSourceDataLock();

		void lock();
		void unlock();
		bool isLocking();

	private:
		unsigned int m_source_id;
		int m_needreq_count;
		int m_needreq_retry;
	};

public:
	MseMediaSource(const char *mime_type, unsigned int id, eMediaType type);
	virtual ~MseMediaSource();

	unsigned int getID();
	eCodec getCodec();
	eMediaType getMediaType();
	void *getSourceBuffer();
	void setSourceBuffer(void* source_buffer);	
	DecryptionContext m_decryption_context;

private:
	eMediaType __type(eCodec codec);
	eCodec __codec(const char *codec);
	unsigned int __cso_codec();	

public:
	void lock_NeedMoreData(void);
	void unlock_NeedMoreData(void);
	bool isLock_NeedMoreData(void);

protected:
	unsigned int	m_id;
	eCodec			m_codec;
	eMediaType		m_media_type;
	void*			m_source_buffer;
	MseMediaSourceDataLock	*m_source_lock;
};

#endif	/* __MSE_MEDIA_SOURCE_H__ */
