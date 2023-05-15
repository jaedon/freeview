/** **********************************************************************************************************
	@file 		hmpbackend.h
	
	@date		2013/06/15
	@author		Solution - STB Component (tstbcomp@humaxdigital.com)
	@breif		UVAMediaBackend
	
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

#ifndef __HMP_BACKEND_H_
#define __HMP_BACKEND_H_

#include "uva_backend.h"
#include "uva_chunkdemuxer.h"
#include "mediaplayer.h"
#include "msemediasource.h"

#include <string>
#include <pthread.h>
#include <list>
#include <vector>

typedef struct {
	int x;
	int y;
	int w;
	int h;
} WindowRect_t;

typedef enum {
	HMP_MEDIA_STATE_IDLE				= 0,
	HMP_MEDIA_STATE_CANPLAY,
	HMP_MEDIA_STATE_PLAY,
	HMP_MEDIA_STATE_SEEKING,
	HMP_MEDIA_STATE_ERROR
} eHmpMediaState;


class HAVComponent : public UVABackend::AVComponent {
public:
	HAVComponent(UVABackend::AVComponentType _type,
			const char* _encoding,
			bool _encrypted,
			double _aspect_ratio,
			const char* _language,
			bool _audio_description,
			int _audio_channels,
			bool _hearing_impaired,
			Kind _kind,
			const char* _label,
			const char* _id,
			int _bandwidth,
			const char* _representation_id,
			uintptr_t _source_id);
	void setActive(bool active);
	void backendMarkUnused();
};

/***************************************************************************************************
 * HmpMediaBackend
 ***************************************************************************************************/
class HmpMediaBackend : public UVAMediaBackend, public MediaPlayerListener {
public:
	HmpMediaBackend(UVABackendType type, UVAClient *client);
	virtual ~HmpMediaBackend();

	virtual UVA_STATUS initBackend(const char *origin, SourceDescription **sources,
						bool automatic_key_system_selection);
	virtual UVA_STATUS attachSource(SourceDescription *source);
	virtual UVA_STATUS removeSource(uintptr_t source_id);
	virtual UVA_STATUS setVideoWindow(int x, int y, int w, int h);
	virtual UVA_STATUS setClipRect(int x, int y, int w, int h);
	virtual UVA_STATUS getNativeSize(int &w, int &h, double &pixel_aspect);
	virtual UVA_STATUS getSeekableTimeRanges(ByteRange **input_byte_ranges,
						TimeRange **&output_time_ranges);
	virtual UVA_STATUS getBufferedTimeRanges(ByteRange **input_byte_ranges,
						TimeRange **&output_time_ranges);
	virtual UVA_STATUS canPlayURL(const char *url);
	virtual UVA_STATUS play();
	virtual UVA_STATUS stop() { return UVA_OK; }
	virtual UVA_STATUS pause(bool get_resources);
	virtual UVA_STATUS setPlaybackRate(double rate);
	virtual UVA_STATUS setVolume(double volume);
	virtual UVA_STATUS getDuration(double &duration);
	virtual UVA_STATUS getPosition(double &position);
	virtual UVA_STATUS setPosition(double position);
	virtual UVA_STATUS setVisibility(bool visible, bool unloaded) { return UVA_OK; }
	virtual UVA_STATUS getSizeFromDuration(double duration, size_t &size);
	virtual UVA_STATUS writeData(UVAEvent *need_data_event, size_t bytes_written,
						WriteDataStatus status);
	virtual UVA_STATUS canPlayType(const char *type, const char **codecs,
						const char *drmID);
	virtual UVA_STATUS setFullScreen(bool fullscreen);
	virtual UVA_STATUS getSupportedMimetypes(char **&mime_types);
	virtual UVA_STATUS setContentSize(uintptr_t source_id, size_t content_size);
	virtual UVA_STATUS getStartDate(double &date);
	virtual UVA_STATUS setKeySystem(const char* key_system_id);

	// not implemented
	virtual UVA_STATUS getVideoFrame(unsigned char* buffer, size_t buffer_size);
	virtual UVA_STATUS setLoop(const char* value);
	virtual UVA_STATUS selectComponent(AVComponent* component);
	virtual UVA_STATUS unselectComponent(AVComponent* component);
	virtual UVA_STATUS setSource(const char* id);
	virtual UVA_STATUS onListenerChanged(const char* event_name);
	virtual UVA_STATUS setBufferingStrategy(BufferingStrategy strategy);
	virtual UVA_STATUS getBytesInBuffer(size_t &bytes);

	UVAClient *getClient() { return m_uva_client; }
	MediaPlayer *getPlayer() { return m_player; }
	WindowRect_t &getWindowRect() { return m_rect; }
	char *getKeySystem() { return m_key_system_id; }
//	UVADRMBackend *getDRMBackend() { return m_drm_backend; }
	// DRMKEY_SYNC work around
	/////////////////////////////////////////////////
	bool getIsFirst() { return m_is_first; }
	void setIsFirst(bool isFirst) { m_is_first = isFirst; }
	
	void setMediaState(eHmpMediaState state) { m_media_state = state; }
	bool getMediaState() { return m_media_state; }
	void setAutoPlay(bool state) { m_autoplay = state; }
	bool getAutoPlay() { return m_autoplay; }
	void setLastSetPosition(double position){ m_last_seekposition = position;};

	static UVABackend::AVComponent::Kind __change_component_role(char *in);

	static HmpMediaBackend *findActiveBackend();
	static UVADRMBackend* getDRMBackend(void* me, const char* id);

	virtual void playStateChanged(eMpPlayEvent event, long long int param = 0);
private:
	bool initHmxMP(SourceDescription *source);
	bool initMseMP(void);
	bool doDRMDecryptDecoderBuffer(UVADecoderBuffer *b);
	bool __seek(HmpMediaBackend *hmp, double pos);	
	void createThread();
	UVADRMBackend *getDRMBackend();
	void removePlayer() { if(m_player) m_player = NULL; }


	static void *threadFunc(void *arg);

public:
	static std::vector<HmpMediaBackend *>	s_backends;
	static pthread_mutex_t	s_backend_mutex;

private:
	UVABackendType 		m_uva_type;
	UVAClient			*m_uva_client;
	MediaPlayer			*m_player;
	WindowRect_t		m_rect;
	char 				*m_key_system_id;
	UVADRMBackend 		*m_drm_backend;
	// DRMKEY_SYNC work around
	/////////////////////////////////////////////////
	bool				m_is_first;
	eHmpMediaState		m_media_state;
	bool				m_autoplay;
	std::list<double>	m_seeks;
	pthread_t			m_task_thread;
	pthread_mutex_t		m_mutex;
	bool 				m_quit_task;
	double				m_last_seekposition;
	std::list<HAVComponent *> m_avcomponents;
};

static inline HmpMediaBackend *getMediaBackend()
{ return *(HmpMediaBackend::s_backends.rbegin()); }

#endif //__HMP_BACKEND_H_
