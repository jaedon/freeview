/** **********************************************************************************************************
	@file 		msemediaplayer.cpp
	
	@date		2013/06/15
	@author		Solution - STB Component (tstbcomp@humaxdigital.com)
	@breif		Humax Media Player
	
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

#include "msemediaplayer.h"
#include "msemediasource.h"

#include "backend_log.h"

#include "oplvodhtml5handle.h"
#include "oplvod_html5.h"
#include "oplvideoutil.h"
#include "opllocalsystem.h"

#include "cso_def.h"

#if ENABLE_MSE
#include "stdlib.h"
#include "cso_uva.h"
#endif

using namespace std;

LOG_CAT(backend);

/*--------------------------------------------------------------------------------------------
 * class MseMediaPlayer
 *-------------------------------------------------------------------------------------------*/
list<MseMediaPlayer *> MseMediaPlayer::s_players;

MseMediaPlayer::MseMediaPlayer()
	: m_player(NULL), m_active_video_source(NULL), m_active_audio_source(NULL),
		m_position(0.0), m_duration(0.0), m_session_id(0), m_seeking(false)
{
	LOG_ENTRY;

	pthread_mutex_init(&m_mtx, NULL);

	/* protect to the player handle from garbage collector */
	/*if ( !s_players.empty() ) // move hmpmediabackend
	{
		pthread_mutex_lock(&m_mtx);
		__clear_players();
		pthread_mutex_unlock(&m_mtx);
	}*/

	m_player = OplVodHtml5Handle::create(OplVideoHandle::EHandleOwnerTypeUserOipf);
	LOG_ASSERT(m_player);
	m_player->setPlayerType(OPLVOD_PLAYER_TYPE_MSE);
	s_players.push_back(this);
#if ENABLE_MSE	
	cso_open();
#endif
	m_name = MSE_MP;
	LOG_INFO("MseMediaPlayer has been created. (count=%d)\n", s_players.size());
}

MseMediaPlayer::~MseMediaPlayer()
{
	LOG_ENTRY;

#if ENABLE_MSE
	cso_close();
#endif

	vector<MseMediaSource *>::iterator it;
	for ( it=m_sources.begin(); it!=m_sources.end(); ++it )
	{
		MseMediaSource *s = *it;
		delete s;
	}
	m_sources.clear();

	s_players.remove(this);

	if ( s_players.empty() )
	{
		if ( m_player )
		{
			m_player->stop();
			OplVideoHandle::unRegist(m_player);
			delete m_player;
			m_player = NULL;
		}
	}

	map<unsigned int, MseSourceMap *>::iterator map_it;
	for ( map_it=m_source_map.begin(); map_it!=m_source_map.end(); ++map_it )
	{
		if ( NULL != map_it->second )
		{
			delete map_it->second;
			m_source_map.erase(map_it);
			break;
		}
	}

	pthread_mutex_destroy(&m_mtx);
	
	LOG_INFO("MseMediaPlayer has been destroyed. (count=%d)\n", s_players.size());
}

bool MseMediaPlayer::init(MediaSource *source)
{
	LOG_ENTRY;
	
	if ( source ) attachSource(source);

	// to OBAMA
	m_player->setListener((void *)this, reinterpret_cast<void (*)(void*, void*, eOplVodEventType)>(onEventCallback));

	m_pump_index = 0;
	m_session_id = 0;

	return true;
}

bool MseMediaPlayer::deinit()
{
	LOG_ENTRY;

	return true;
}

bool MseMediaPlayer::play()
{
	LOG_ENTRY;

	eOplVodPlayState state = m_player->getPlayState();
	LOG_INFO("playState=[%d]\n", state);	
	if ( state == OPLVOD_PLAY_STATE_PAUSED 
		|| state == OPLVOD_PLAY_STATE_PLAYING )
	{
		if ( !m_player->setPlaySpeed(100) )
		{
			LOG_ERROR("Cannot resume the content\n");
			return false;
		}
	}
	else
	{
#if ENABLE_MSE
	CSO_ProbeInfo_t *info = (CSO_ProbeInfo_t *)calloc(1, sizeof(CSO_ProbeInfo_t));
        if ( info == NULL ) return false;
		
		if ( CSO_UVA_Probe(m_session_id) != CSO_ERR_OK
			|| CSO_UVA_Probe(m_session_id) != CSO_ERR_OK
			|| CSO_UVA_ProbeInfo(m_session_id, info) != CSO_ERR_OK )
		{
        		free(info);
			LOG_ERROR("Cannot play the content.\n");
			return false;
		}

		OplVodProbeInfo probe_info;
		memset(&probe_info, 0x0, sizeof(probe_info));
		
		probe_info.audio_pid = info->audio_stream_id;
		probe_info.video_pid = info->video_stream_id;
		probe_info.audio_codec = (eOplVodAudioCodec)__to_mediapb_audio_codec(info->audio_codec);
		probe_info.video_codec = (eOplVodVideoCodec)__to_mediapb_video_codec(info->video_codec);

        	free(info);

		LOG_INFO("probe_info.audio_id=[%d]\n", probe_info.audio_pid);
		LOG_INFO("probe_info.video_id=[%d]\n", probe_info.video_pid);
		LOG_INFO("probe_info.audio_codec=[%d]\n", probe_info.audio_codec);
		LOG_INFO("probe_info.video_codec=[%d]\n\n", probe_info.video_codec);	
		
		m_player->setProbeInfo(&probe_info);
#endif

		if ( !m_player->play() )
		{
			LOG_ERROR("Cannot play the content\n");
			return false;
		}
	}

	return true;
}

bool MseMediaPlayer::resume()
{
	LOG_ENTRY;

#if ENABLE_MSE
	CSO_CTRL_PARAM_t cso_ctrl_info;

	LOG_INFO("before call CSO_UVA_Control\n");

	CSO_ERR err = CSO_UVA_Control(m_session_id, eCSO_CTRL_RESUME, &cso_ctrl_info);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_Control(eCSO_CTRL_RESUME)\n", err);
		return false;
	}	
#endif	

	m_listener->playStateChanged(MB_PLAYEVENT_SEEK_COMPLETE);

	if ( !m_player->setPlaySpeed(100) )
	{
		LOG_ERROR("Cannot resume the content\n");
		return false;
	}
	
	return true;
}

bool MseMediaPlayer::pause()
{
	LOG_ENTRY;

	eOplVodPlayState state = m_player->getPlayState();
	if ( state != OPLVOD_PLAY_STATE_FINISHED )
	{
		if ( !m_player->setPlaySpeed(0) )
		{
			LOG_ERROR("Cannot pause the content\n");
			return false;
		}
	}
	
	return true;
}

bool MseMediaPlayer::seek(double pos)
{
	LOG_ENTRY;

	eOplVodPlayState state = m_player->getPlayState();

	LOG_INFO("playState=[%d]\n", state);

	unlockAllSources();

	m_listener->playStateChanged(MB_PLAYEVENT_FLUSH_COMPLETED, pos*1000);

	if ( state == OPLVOD_PLAY_STATE_STOPPED
			|| state == OPLVOD_PLAY_STATE_ERROR )
	{
		m_position = pos;
		LOG_INFO("This seek is ingored because current state is stopped or error state.\n");
		m_listener->playStateChanged(MB_PLAYEVENT_SEEK_COMPLETE);
		return true;
	}

#if ENABLE_MSE
	CSO_CTRL_PARAM_t cso_ctrl_info;
	cso_ctrl_info.ullPosition = static_cast<unsigned int>(pos*1000);

	LOG_INFO("before call CSO_UVA_Control\n");

	CSO_ERR err = CSO_UVA_Control(m_session_id, eCSO_CTRL_SEEK, &cso_ctrl_info);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_Control(eCSO_CTRL_SEEK)\n", err);
		return false;
	}	
	LOG_INFO("CSO_UVA_Control(eCSO_CTRL_SEEK) has started successfully.\n");
#endif	

	m_position = pos;
	m_seeking = true;

	return true;
}

bool MseMediaPlayer::setPlaybackRate(double rate)
{
	LOG_ENTRY;

	return true;
}

bool MseMediaPlayer::attachSource(const MediaSource *source)
{
	LOG_ENTRY;

	MseMediaSource *s = dynamic_cast<MseMediaSource *>(const_cast<MediaSource *>(source));
	if ( !s ) return false;

#if ENABLE_MSE
	CSO_ERR err = CSO_UVA_ResigterEventCallback(m_session_id, (void *)onCsoEventCallback);
	m_session = m_session_id;
	
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_ResigterEventCallback()\n", err);
		return false;
	}

	void *src_buf = NULL;

	LOG_INFO("source_id=[%d]", s->getID());
	LOG_INFO("media_type=[%d]", __to_cso_mediatype(s->getMediaType()));
	LOG_INFO("codec=[%d]", __to_cso_codec(s->getCodec()));

	err = CSO_UVA_AddSource(m_session_id, s->getID(),
			static_cast<CSO_MEDIATYPE>(__to_cso_mediatype(s->getMediaType())),
			static_cast<unsigned int>(__to_cso_codec(s->getCodec())), &src_buf);
	if ( err != CSO_ERR_OK )
	{
		LOG_INFO("CSO_ERR=[%d] Failed to call CSO_UVA_AddSource()\n", err);
		return false;
	}

	s->setSourceBuffer(src_buf);
	
	if ( s->getMediaType() == MseMediaSource::VIDEO )
		m_active_video_source = s;
	else
		m_active_audio_source = s;

	m_sources.push_back(s);

	MseSourceMap *src_map = new MseSourceMap();
	src_map->src_buf = src_buf;
	src_map->eos = false;
	m_source_map[s->getID()] = src_map;
#endif	
	return true;
}

bool MseMediaPlayer::detachSource(unsigned int id)
{
	LOG_ENTRY;

	if ( m_sources.empty() ) return false;

	vector<MseMediaSource *>::iterator it;
	for ( it=m_sources.begin(); it!=m_sources.end(); ++it )
	{
		MseMediaSource *s = *it;
		if ( s->getID() == id )
		{
			delete s;
			m_sources.erase(it);
		}
	}

	map<unsigned int, MseSourceMap *>::iterator map_it;
	for ( map_it=m_source_map.begin(); map_it!=m_source_map.end(); ++map_it )
	{
		if ( map_it->first == id )
		{
			delete map_it->second;
			m_source_map.erase(map_it);
			break;
		}
	}

	return true;
}

MediaSource *MseMediaPlayer::getActiveVideoSource()
{
	return m_active_video_source;
}

MediaSource *MseMediaPlayer::getActiveAudioSource()
{
	return m_active_audio_source;
}

double MseMediaPlayer::getPosition()
{
	//LOG_ENTRY;
	//LOG_INFO("position=[%f], bufferstate=%d \n", m_position, m_player->getBufferState());

	if( OPLVOD_PLAY_STATE_PLAYING == m_player->getPlayState() &&
		OPLVOD_BUFFER_STATE_NOT_ENOUGH != m_player->getBufferState())
	{
		m_position+=0.003;
	}
	
	return m_position;
}

void MseMediaPlayer::setPosition(double position)
{
	//LOG_ENTRY;

	if ( position == 0 ) return;
	if ( position <= m_position ) return;
	if ( m_seeking ) return;

	m_position = position;
}

double MseMediaPlayer::getDuration()
{
	//LOG_ENTRY;

	return m_duration;
}

bool MseMediaPlayer::getDurationInfinity()
{
	return 0;
}

void MseMediaPlayer::setDuration(double duration)
{
	LOG_ENTRY;

	m_player->setDuration(duration);
	m_duration = duration;

	LOG_INFO("Duration setting: %f\n", duration);
}

bool MseMediaPlayer::getWindowSize(int *width, int *height)
{
	LOG_ENTRY;

	if ( width ) *width = OSD_WIDTH;
	if ( height ) *height = OSD_HEIGHT;

	return true;
}

bool MseMediaPlayer::setVideoWindow(int x, int y, int w, int h)
{
	//LOG_ENTRY;

	return m_player->setVideoPosition(x, y, w, h);
}

bool MseMediaPlayer::setFullScreen(bool full_screen)
{
	LOG_ENTRY;

	return m_player->setFullScreen(full_screen);
}

unsigned int MseMediaPlayer::getBufferingInfo(unsigned  int **start_times,
				unsigned int **end_times)
{
	static unsigned int s_start_time = 0;
	static unsigned int s_end_time = 0;
	s_end_time = m_duration;

	if ( s_end_time == 0 ) return 0;

	*start_times = &s_start_time;
	*end_times = &s_end_time;

	return 1;
}

unsigned int MseMediaPlayer::getSeekableInfo(unsigned long long **seekable_start, 
	unsigned long long **seekable_end)
{
	return 0;
}


bool MseMediaPlayer::setVolume(double volume)
{
	OplLocalSystem::setVolume(static_cast<unsigned int>(volume*100));

	return true;
}

bool MseMediaPlayer::pushVideo(MseBuffer *buffer)
{
//	LOG_ENTRY;
	if ( !getActiveVideoSource() )
		return false;
	
	MseMediaSource *vs = dynamic_cast<MseMediaSource *>(getActiveVideoSource());
	if ( !vs ) return false;
	
	return push(vs, buffer);
}


bool MseMediaPlayer::pushAudio(MseBuffer *buffer)
{
//	LOG_ENTRY;
	if ( !getActiveAudioSource() )
		return false;
	
	MseMediaSource *as = dynamic_cast<MseMediaSource *>(getActiveAudioSource());
	if ( !as ) return false;
	
	return push(as, buffer);
}

bool MseMediaPlayer::setEos(unsigned int source_id)
{
	LOG_ENTRY;

#if ENABLE_MSE
	MseSourceMap *src_map = m_source_map[source_id];
	if ( src_map->src_buf && !src_map->eos )
	{
		CSO_ERR err = CSO_UVA_SRCBUF_EOS(src_map->src_buf);
		if ( err != CSO_ERR_OK )
			LOG_INFO("Failed to call CSO_UVA_SRCBUF_Write()\n");

		map<unsigned int, MseSourceMap *>::iterator map_it;
		for ( map_it=m_source_map.begin(); map_it!=m_source_map.end(); ++map_it )
		{
			if ( map_it->first == source_id )
			{
				map_it->second->eos = true;
				break;
			}
		}
	}
#endif

	return true;
}

MseMediaSource *MseMediaPlayer::findSource(unsigned int source_id)
{
	std::vector<MseMediaSource *>::iterator it;
	for ( it=m_sources.begin(); it!=m_sources.end(); ++it )
	{
		MseMediaSource *s = *it;
		if ( !s ) continue;
		if ( s->getID() == source_id ) return s;
	}
	return NULL;
}

void MseMediaPlayer::lockSource(unsigned int source_id)
{
	MseMediaSource *s = findSource(source_id);
	if ( !s ) return;
	s->lock_NeedMoreData();
}

void MseMediaPlayer::unlockSource(unsigned int source_id)
{
	MseMediaSource *s = findSource(source_id);
	if ( !s ) return;
	s->unlock_NeedMoreData();
}

bool MseMediaPlayer::isSourceLocking(unsigned int source_id)
{
	MseMediaSource *s = findSource(source_id);
	if ( !s ) return false;
	return s->isLock_NeedMoreData();
}

void MseMediaPlayer::unlockAllSources(void)
{
	std::vector<MseMediaSource *>::iterator it;
	for ( it=m_sources.begin(); it!=m_sources.end(); ++it )
	{
		if ( NULL != *it ) (*it)->unlock_NeedMoreData();
	}
}

void MseMediaPlayer::releaseAllDecrypter(void* param)
{
	UVADRMBackend * drmbackend = (UVADRMBackend *) param;
	
	if(m_sources.size())
	{
		vector<MseMediaSource *>::iterator it;
		for ( it=m_sources.begin(); it!=m_sources.end(); ++it )
		{
			MseMediaSource *s = *it;

			if(s->m_decryption_context.m_decrypter)
				drmbackend->releaseDecrypter(s->m_decryption_context.m_decrypter);
			s->m_decryption_context.m_decrypter = NULL;
			s->m_decryption_context.m_last_kid_ptr = NULL;
		}		
	}
}

/*
 *	Private
 */
bool MseMediaPlayer::clearPlayers()
{
	if(!MseMediaPlayer::s_players.empty())
	{

		list<MseMediaPlayer *>::iterator it, temp;
		for ( it=s_players.begin(); it!=s_players.end(); )
		{
			/* it를 바로 삭제할 경우, it의 next 포인터가 사라진다. */
			temp = it;
			it++;
			delete (*temp);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MseMediaPlayer::cso_open()
{
#if ENABLE_MSE
	OplVodResourceMap map;	
	OplVodHtmlFive::getResourceMap(&map);	

	CSO_RESOURCE_MAP_t cso_map;
	cso_map.hPlaypumpList = map.playpump_list;
	cso_map.hDmaList = map.dma_list;
	cso_map.uiNumOfPlaypump = map.num_of_playpump;
	cso_map.uiNumOfDma = map.num_of_dma;

	LOG_INFO("map.num_of_playpump=[%d]\n", map.num_of_playpump);
	LOG_INFO("map.num_of_dma=[%d]\n", map.num_of_dma);	

	CSO_ERR err = CSO_UVA_SetResourceMap(&cso_map);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("Failed to call CSO_UVA_SetResourceMap()\n");	
		return false;
	}

	err = CSO_UVA_Init(NULL);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("Dumplicated UVA Init.\n");
	}

	err = CSO_UVA_Open(&m_session_id);		
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_Open()\n", err);
		return false;
	}

	LOG_INFO("CSO module is opened successfully (session_id=%d)\n", m_session_id);

#endif
	return true;
}

bool MseMediaPlayer::cso_close()
{
#if ENABLE_MSE
	CSO_ERR err = CSO_UVA_Stop(m_session_id);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_Stop()\n", err);
		return false;
	}

#if 0
	err = CSO_UVA_ResigterEventCallback(m_session_id, (void *)NULL);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_UnResigterEventCallback()\n", err);
		return false;
	}
	m_callback = NULL;
#endif

	err = CSO_UVA_Close(m_session_id);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_Close()\n", err);
		return false;
	}
	m_session_id = 0;

	LOG_INFO("CSO module is closed successfully.\n");
#endif

	return true;
}

bool MseMediaPlayer::push(MseMediaSource * s, MseBuffer *buffer)
{
//	LOG_ENTRY;

#if ENABLE_MSE	
	CSO_ERR err = CSO_UVA_SRCBUF_Write(s->getSourceBuffer(), buffer->pts, buffer->dts,
						buffer->duration, buffer->data_size, buffer->data, 0, NULL);
	if ( err != CSO_ERR_OK )
		LOG_INFO("Failed to call CSO_UVA_SRCBUF_Write()\n");
#endif

	return true;
}

int MseMediaPlayer::__to_cso_mediatype(MseMediaSource::eMediaType type)
{
#if ENABLE_MSE	
	int cso_type = CSO_MEDIATYPE_UNKNOWN;

	switch ( type )
	{
	case MseMediaSource::VIDEO:
		cso_type = CSO_MEDIATYPE_VIDEO;
		break;
	case MseMediaSource::AUDIO:
		cso_type = CSO_MEDIATYPE_AUDIO;
		break;
	default:
		break;
	}

	return cso_type;
#else
	return 0;
#endif
}

int MseMediaPlayer::__to_cso_codec(MseMediaSource::eCodec codec)
{
#if ENABLE_MSE	
	int cso_codec = 0;

	switch ( codec ) 
	{
		case MseMediaSource::H264:
			cso_codec = CSO_VCODEC_H264;
			break;
		case MseMediaSource::VC1:
			cso_codec = CSO_VCODEC_VC1;
			break;
		case MseMediaSource::MPEG2:
			cso_codec = CSO_VCODEC_MPEG2;
			break;
		case MseMediaSource::MPEG4:
			cso_codec = CSO_VCODEC_MPEG4Part2;
			break;
		case MseMediaSource::AAC:
			cso_codec = CSO_ACODEC_AAC;
			break;			
		case MseMediaSource::MP3:
			cso_codec = CSO_ACODEC_MP3;
			break;
		case MseMediaSource::PCM:
			cso_codec = CSO_ACODEC_PCM_S16BE;
			break;
		default:
			break;
	}

	return cso_codec;
#else
	return 0;
#endif
}

int MseMediaPlayer::__to_mediapb_video_codec(unsigned int codec)
{
#if ENABLE_MSE	
	int video_codec = 0;

	switch ( codec ) 
	{
		case CSO_VCODEC_H264:
			video_codec = OPLVOD_VIDEO_CODEC_H264;
			break;
		case CSO_VCODEC_VC1:
			video_codec = OPLVOD_VIDEO_CODEC_VC1;
			break;
		case CSO_VCODEC_MPEG2:
			video_codec = OPLVOD_VIDEO_CODEC_MPEG2;
			break;
		case CSO_VCODEC_MPEG4Part2:
			video_codec = OPLVOD_VIDEO_CODEC_MPEG4_PART2;
			break;
		default:
			video_codec = OPLVOD_VIDEO_CODEC_UNKNOWN;
			break;
	}

	return video_codec;
#else
	return 0;
#endif
}

int MseMediaPlayer::__to_mediapb_audio_codec(unsigned int codec)
{
#if ENABLE_MSE	
	int audio_codec = 0;

	switch ( codec )
	{
	case CSO_ACODEC_AAC:
		audio_codec = OPLVOD_AUDIO_CODEC_AAC;
		break;			
	case CSO_ACODEC_MP3:
		audio_codec = OPLVOD_AUDIO_CODEC_MP3;
		break;
	case CSO_ACODEC_PCM_S16BE:
		audio_codec = OPLVOD_AUDIO_CODEC_PCM;
		break;
	default:
		audio_codec = OPLVOD_AUDIO_CODEC_UNKNOWN;
	}
	
	return audio_codec;
#else
	return 0;
#endif
}

void MseMediaPlayer::__seek(OplVodHtml5Handle *vod, double pos)
{
	if ( !vod->seek(static_cast<unsigned int>(pos)) )
	{
		LOG_ERROR("Cannot change the position to %f\n", pos);
		return;
	}
}

int MseMediaPlayer::getComponentCount(eMpPlayAVComponentType type)
{
	return 0;
}
bool MseMediaPlayer::getComponent(eMpPlayAVComponentType type, int index, MpPlayAVComponent_t *component)
{
	return true;
}

bool MseMediaPlayer::selectComponent(eMpPlayAVComponentType type, unsigned int id)
{
	return true;
}
bool MseMediaPlayer::unselectComponent(eMpPlayAVComponentType type, unsigned int id)
{
	return true;
}
bool MseMediaPlayer::getSelectedChangedTypeIndex(eMpPlayAVComponentType *type, int *index)
{
	return true;
}

int MseMediaPlayer::getActiveComponentIndex(eMpPlayAVComponentType type)
{
	return 0;
}



/*
 *	Static
 */
void MseMediaPlayer::onCsoEventCallback(unsigned int session_id, int evt, long long int param)
{
#if ENABLE_MSE
	MseMediaPlayer *player = static_cast<MseMediaPlayer *>(getPlayer(session_id));
	if ( !player ) return;

	OplVodHtml5Handle *vod = player->getVod();

	pthread_mutex_lock(&player->m_mtx);
	switch ( evt )
	{
		case CSO_EVENT_MEDIA_SEEK_COMPLETE:
			LOG_INFO("CSO_EVENT_MEDIA_SEEK_COMPLETE: param=[%lld]\n", param);
			player->__seek(vod, param);
			break;
		case CSO_EVENT_MEDIA_NEEDMOREDATA:
			player->m_listener->playStateChanged(MB_PLAYEVENT_NEEDMOREDATA, param);
			break;

		default:
			break;
	
	}
	pthread_mutex_unlock(&player->m_mtx);
#endif
}


void MseMediaPlayer::onEventCallback(void *hPlayer, void *handle, int type)
{
	MseMediaPlayer *player = static_cast<MseMediaPlayer *>(getPlayer(hPlayer));
	if ( !player ) return;

	switch ( type )
	{
	case OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED:
//		LOG_INFO("OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED\n");
		return player->onPlayStateChanged(handle);
	case OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED:
//		LOG_INFO("OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED\n");
		return player->onPlayPositionChanged(handle);
	case OPLVOD_EVENT_TYPE_PUMP_INDEX_CHANGED:
//		LOG_INFO("OPLVOD_EVENT_TYPE_PUMP_INDEX_CHANGED\n");
		return player->onPlayPumpIndexChanged(handle);
	case OPLVOD_EVENT_TYPE_SEEK_READY:
//		LOG_INFO("OPLVOD_EVENT_TYPE_SEEK_READY\n");
		return player->onSeekReady(handle);
	case OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED:
//		LOG_INFO("OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED\n");
		return player->onBufferStateChanged(handle);
	default:
		break;
	}
}

void MseMediaPlayer::onBufferStateChanged(void *handle)
{
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	eOplVodBufferState state = vod->getBufferState();
	switch ( state )
	{
	case OPLVOD_BUFFER_STATE_CANPLAY:
		LOG_INFO("OPLVOD_BUFFER_STATE_CANPLAY\n");
		break;
	case OPLVOD_BUFFER_STATE_NOT_ENOUGH:
		LOG_INFO("OPLVOD_BUFFER_STATE_NOT_ENOUGH\n");
		this->m_listener->playStateChanged(MB_PLAYEVENT_MSE_UNDERRUN);
		break;
	case OPLVOD_BUFFER_STATE_ENOUGH:
		LOG_INFO("OPLVOD_BUFFER_STATE_ENOUGH\n");
		this->m_listener->playStateChanged(MB_PLAYEVENT_MSE_BUFFERED);
		break;
	case OPLVOD_BUFFER_STATE_FINISHED:
		LOG_INFO("OPLVOD_BUFFER_STATE_FINISHED\n");
	default:
		break;
	}
}

void MseMediaPlayer::onPlayStateChanged(void *handle)
{
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	pthread_mutex_lock(&this->m_mtx);
	eOplVodPlayState state = vod->getPlayState();
	switch ( state )
	{
	case OPLVOD_PLAY_STATE_FINISHED:	
		LOG_INFO("OPLVOD_PLAY_STATE_FINISHED\n");
		this->m_listener->playStateChanged(MB_PLAYEVENT_FINISH);
		break;
	case OPLVOD_PLAY_STATE_ERROR:
		LOG_INFO("OPLVOD_PLAY_STATE_ERROR\n");
		this->m_listener->playStateChanged(MB_PLAYEVENT_ERROR);
		break;
	case OPLVOD_PLAY_STATE_PLAYING:
		LOG_INFO("OPLVOD_PLAY_STATE_PLAYING\n");
		this->m_listener->playStateChanged(MB_PLAYEVENT_PLAYING);
		break;
	case OPLVOD_PLAY_STATE_PAUSED:
		LOG_INFO("OPLVOD_PLAY_STATE_PAUSED\n");
		this->m_listener->playStateChanged(MB_PLAYEVENT_PAUSED);	
		break;
	default:
		break;
	}
	pthread_mutex_unlock(&this->m_mtx);
}

void MseMediaPlayer::onPlayPositionChanged(void *handle)
{
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	pthread_mutex_lock(&this->m_mtx);
	this->setPosition(static_cast<double>(vod->getPlayPosition()/1000.0)/*+0.01*/);
	pthread_mutex_unlock(&this->m_mtx);
}

void MseMediaPlayer::onPlayPumpIndexChanged(void *handle)
{
	LOG_INFO("OPLVOD_EVENT_TYPE_PUMP_INDEX_CHANGED\n");

#if ENABLE_MSE

	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	pthread_mutex_lock(&this->m_mtx);
	int index = vod->getPumpIndex();
	this->setPumpIndex(index);

	LOG_INFO("OPLVOD_EVENT_TYPE_PUMP_INDEX_CHANGED:: index = %d\n", index); 

	CSO_ERR err = CSO_UVA_Start(this->getSessionID(), index);
	if ( err != CSO_ERR_OK )
	{
		LOG_ERROR("CSO_ERR=[%d] Failed to call CSO_UVA_Start()\n", err);
		pthread_mutex_unlock(&this->m_mtx);
		return;
	}
	pthread_mutex_unlock(&this->m_mtx);
#endif
}

void MseMediaPlayer::onSeekReady(void *handle)
{
	LOG_INFO("OPLVOD_EVENT_TYPE_SEEK_READY\n");

	pthread_mutex_lock(&this->m_mtx);
	this->m_listener->playStateChanged(MB_PLAYEVENT_SEEK_READY);
	this->m_seeking = false;
	pthread_mutex_unlock(&this->m_mtx);
}
