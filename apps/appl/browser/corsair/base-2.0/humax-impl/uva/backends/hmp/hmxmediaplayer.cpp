/** **********************************************************************************************************
	@file 		hmxmediaplayer.cpp
	
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

#include "hmxmediaplayer.h"
#include "mediasource.h"

#include "oplvodhtml5handle.h"
#include "opllocalsystem.h"
#include "bplglobal.h"

#include "backend_log.h"

#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

using namespace std;

LOG_CAT(backend);

#define HMX_PROGRESS_INTERVAL	2000
#define NO_BUFFERED_COUNT_LIMIT	20


/*--------------------------------------------------------------------------------------------
 * class HmxBufferState
 *-------------------------------------------------------------------------------------------*/
HmxMediaPlayer::HmxBufferState::HmxBufferState(HmxMediaPlayer *player)
	: m_hmx_player(player),
	m_buffer_endpos(0),
	m_buffer_progress(0),
	m_thread_exit(false),
	m_no_buffered(0)
{
	int r = pthread_mutex_init(&m_mtx, NULL);
	if ( r != 0 )
	{
		LOG_ERROR("Cannot init thread cond.\n");
	}
	
	r = pthread_create(&m_thread, NULL, HmxBufferState::threadFunc, this);
	if ( r != 0 )
	{
		LOG_ERROR("Cannot create HmxMediaPlayer thread.\n");
	}
}

HmxMediaPlayer::HmxBufferState::~HmxBufferState()
{
	pthread_mutex_lock(&m_mtx);
	m_thread_exit = true;

	pthread_mutex_unlock(&m_mtx);

	int status = 0;
	pthread_join(m_thread, reinterpret_cast<void **>(&status));

	pthread_mutex_destroy(&m_mtx);
}

void HmxMediaPlayer::HmxBufferState::touch()
{
	m_buffer_eventq.push(BUFFER_EVENT_PROGRESS);
}


void HmxMediaPlayer::HmxBufferState::doPeriod()
{
	pthread_mutex_lock(&m_mtx);
	if ( !m_buffer_eventq.empty() )
		{
		eBufferEvent evt = m_buffer_eventq.front();
		m_buffer_eventq.pop();
	    switch ( evt )
	    {
            case BUFFER_EVENT_PROGRESS:
                m_hmx_player->m_listener->playStateChanged(MB_PLAYEVENT_BUFFER_PROGRESS);
                m_no_buffered = 0;
                break;
            default:
                break;
		}
	}
	else
	{
		m_no_buffered++;
		if ( m_no_buffered > NO_BUFFERED_COUNT_LIMIT )
	    	m_hmx_player->m_listener->playStateChanged(MB_PLAYEVENT_BUFFER_IDLE);
	}
	pthread_mutex_unlock(&m_mtx);
}

void *HmxMediaPlayer::HmxBufferState::threadFunc(void *arg)
{
	HmxBufferState *state = static_cast<HmxBufferState *>(arg);
	if ( !state ) return NULL;

	LOG_INFO("BufferStateThread is created.\n");

	while ( !state->m_thread_exit )
	{
		usleep(100000);
		state->doPeriod();
	}

	LOG_INFO("BufferStateThread is destroyed.\n");
	return NULL;
}

/*--------------------------------------------------------------------------------------------
 * class HmxMediaPlayer
 *-------------------------------------------------------------------------------------------*/
HmxMediaPlayer::HmxMediaPlayer()
	: m_source(NULL), m_position(0), m_duration(INFINITY), m_buffer_state(NULL), m_player(NULL), m_error_code(MP_ERROR_NO_ERROR)
{
	LOG_ENTRY;

	if ( !m_player )
	{
		if(BplGlobalConfig::getIsRunningHbbTVApp())
			m_player = OplVodHtml5Handle::create(OplVideoHandle::EHandleOwnerTypeUserHbbtv);
		else
			m_player = OplVodHtml5Handle::create(OplVideoHandle::EHandleOwnerTypeUserOipf);
		LOG_ASSERT(m_player);
		m_player->setPlayerType(OPLVOD_PLAYER_TYPE_HMX);
	}
	else
	{
		m_player->stop();
	}
	
	m_buffer_state = new HmxBufferState(this);
}

HmxMediaPlayer::~HmxMediaPlayer()
{
	if ( m_buffer_state )
	{
		delete m_buffer_state;
		m_buffer_state = NULL;
	}

	if ( m_player )
	{
			m_player->stop();
			OplVideoHandle::unRegist(m_player);
			delete m_player;
			m_player = NULL;
	}
}

bool HmxMediaPlayer::init(MediaSource *source)
{
	if ( !source ) return false;

	m_name = HMX_MP;

	m_player->setUrl(source->getUrl().c_str());
	m_player->setMimeType(source->getMimeType().c_str());
	m_player->setListener((void *)this, reinterpret_cast<void (*)(void*, void*, eOplVodEventType)>(onEventCallback));
	m_player->probe();

	m_source = source;

	return true;
}

bool HmxMediaPlayer::deinit()
{
	LOG_ENTRY;

	m_player->setListener(NULL, NULL);

	return true;
}

bool HmxMediaPlayer::play()
{
	LOG_ENTRY;

	if ( !m_player->play() )
	{
		LOG_ERROR("Cannot play the content");
		return false;
	}

	if ( m_player->getPlayState() == OPLVOD_PLAY_STATE_PAUSED
		|| m_player->getPlayState() == OPLVOD_PLAY_STATE_FINISHED )
	{
		LOG_INFO("If current state is paused or finished,\
					the playback rate should be changed to normal");
		m_player->setPlaySpeed(100);
	}

	return true;
}

bool HmxMediaPlayer::pause()
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

bool HmxMediaPlayer::seek(double pos)
{
	LOG_ENTRY;

	if( m_player->getPlayState() == OPLVOD_PLAY_STATE_FINISHED )
	{
		m_player->setPlaySpeed(0);
	}

#if defined(CONFIG_TIMESTAMP_64)
	if ( !m_player->seek(static_cast<unsigned long long>(pos*1000)) )
#else
	if ( !m_player->seek(static_cast<unsigned int>(pos*1000)) )
#endif
	{
		LOG_ERROR("Cannot change the position to %f\n", pos);
		return false;
	}

	/* not sure if we have to call setPlaySpeed */
	if ( m_player->getPlayState() == OPLVOD_PLAY_STATE_PLAYING
			|| m_player->getPlayState() == OPLVOD_PLAY_STATE_PAUSED
			|| m_player->getPlayState() == OPLVOD_PLAY_STATE_FINISHED )
	{
		//m_player->setPlaySpeed(100);
	}

	//callback(MB_PLAYEVENT_SEEK_COMPLETE);

	return true;
}

bool HmxMediaPlayer::setPlaybackRate(double rate)
{
	LOG_ENTRY;

	if ( !m_player->setPlaySpeed(rate*100) )
	{
		LOG_ERROR("Cannot chagne the playback rate to %f)\n", rate);
		return false;
	}

	return true;
}

bool HmxMediaPlayer::attachSource(const MediaSource *source)
{
	LOG_ENTRY;
    
	if ( !source ) return false;

	if ( m_source )
	{
		m_player->stop();
		delete m_source;
	}
	m_source = const_cast<MediaSource *>(source);

	return true;
}

bool HmxMediaPlayer::detachSource(unsigned int id)
{
	LOG_ENTRY;

	if ( !m_source ) return false;

	m_player->stop();
	delete m_source;
	m_source = NULL;

	return true;
}

double HmxMediaPlayer::getPosition()
{
	//LOG_ENTRY;

	if ( m_player->getPlayState() == OPLVOD_PLAY_STATE_PLAYING ){
		if ( (unsigned int)m_position > (double)0.000000 )
			m_position += (double)0.000001;
	}

	//LOG_INFO("position=[%f]\n", m_position);
	
	return m_position;
}

void HmxMediaPlayer::setPosition(double position)
{
	//LOG_ENTRY;
	m_position = position;
}

double HmxMediaPlayer::getDuration()
{
	//LOG_ENTRY;

	return m_duration;
}

bool HmxMediaPlayer::getDurationInfinity()
{
	LOG_ENTRY;
	
	return m_player->getDurationInfinity();
}

void HmxMediaPlayer::setDuration(double duration)
{
	LOG_ENTRY;

	m_duration = duration;
}

bool HmxMediaPlayer::getWindowSize(int *width, int *height)
{
	LOG_ENTRY;

	return m_player->getVideoResolution(width, height);
}

bool HmxMediaPlayer::setVideoWindow(int x, int y, int w, int h)
{
	//LOG_ENTRY;

	return m_player->setVideoPosition(x, y, w, h);
}

bool HmxMediaPlayer::setFullScreen(bool full_screen)
{
	LOG_ENTRY;

	return m_player->setFullScreen(full_screen);
}

unsigned int HmxMediaPlayer::getBufferingInfo(unsigned  int **start_times,
				unsigned int **end_times)
{
	return m_player->getBufferingInfo(start_times, end_times);
}

unsigned int HmxMediaPlayer::getSeekableInfo(unsigned long long **seekable_start,
				unsigned long long **seekable_end)
{
	return m_player->getSeekableInfo(seekable_start, seekable_end);
}

bool HmxMediaPlayer::setVolume(double volume)
{
	OplLocalSystem::setVolume(static_cast<unsigned int>(volume*100));

	return true;
}

int HmxMediaPlayer::getComponentCount(eMpPlayAVComponentType type)
{
	LOG_ENTRY;
	
	return (int)m_player->getComponentsCount((eOplAVComponentType)type);
}
bool HmxMediaPlayer::getComponent(eMpPlayAVComponentType type, int index, MpPlayAVComponent_t *component)
{
	LOG_ENTRY;
	OplAVComponent_t comp;
	m_player->getComponent((eOplAVComponentType)type, index, &comp);

	if( component )
	{
		component->ulComponentTag = comp.ulComponentTag;
		component->ulPid = comp.ulPid;
		component->eType = (eMpPlayAVComponentType)comp.eType;
		component->eSubType = (eMpPlayComponentSubType)comp.eSubType;	
		component->bHardOfHearing_Impaired = comp.bHardOfHearing_Impaired;
		component->fAspectRatio = comp.fAspectRatio;
		strncpy(component->szLanguage, comp.szLanguage, MP_AVCOMPONENT_LANG_SIZE);
		component->bAudioDescription = comp.bAudioDescription;
		component->ulAudioChannels = comp.ulAudioChannels;
		strncpy(component->encoding, comp.encoding, MP_AVCOMPONENT_TEXT_SIZE);
		strncpy(component->compName, comp.compName, MP_AVCOMPONENT_NAME_SIZE);
		strncpy(component->compRole, comp.compRole, MP_AVCOMPONENT_ROLE_SIZE);
	}
	
	return true;
}

int HmxMediaPlayer::getActiveComponentIndex(eMpPlayAVComponentType type)
{
	LOG_ENTRY;
	int index = -1;

	index = m_player->getActiveComponentIndex((eOplAVComponentType)type);
	
	return index;
}

bool HmxMediaPlayer::selectComponent(eMpPlayAVComponentType type, unsigned int index)
{
	bool ret = false;
	LOG_ENTRY;
	if( m_player )
	{
		ret = m_player->setActiveComponent((eOplAVComponentType)type, index);
	}

	return ret;
}

bool HmxMediaPlayer::unselectComponent(eMpPlayAVComponentType type, unsigned int id)
{
	bool ret = false;
	LOG_ENTRY;
	if( m_player )
	{
		ret = m_player->unsetActiveComponent((eOplAVComponentType)type, id);
	}

	return ret;
}

bool HmxMediaPlayer::getSelectedChangedTypeIndex(eMpPlayAVComponentType *type, int *index)
{
	bool ret = false;
	eOplAVComponentType comp_type = OPLAVCOMPONENT_TYPE_MAX;
	int comp_index = 0;
	
	LOG_ENTRY;
	
	if( m_player )
	{		
		ret = m_player->setSelectedComponentTypeIndex(&comp_type, &comp_index);
		*index = comp_index;
		switch(comp_type)
		{
			case OPLAVCOMPONENT_TYPE_VIDEO:
				*type = MP_COMPONENT_TYPE_VIDEO; break;
			case OPLAVCOMPONENT_TYPE_AUDIO:
				*type = MP_COMPONENT_TYPE_AUDIO; break;
			case OPLAVCOMPONENT_TYPE_SUBTITLE:
				*type = MP_COMPONENT_TYPE_SUBTITLE; break;
			default: break;
		}	
	}

	return ret;
}


double HmxMediaPlayer::getStartDate(void)
{
	LOG_ENTRY;
	double date = 0;
	
	if( m_player )
	{
		date = m_player->getStartDate();
	}

	return date;
}

eMPPlayError HmxMediaPlayer::getErrorType(void)
{
	return m_error_code;
}

/*
 *	Static
 */
void HmxMediaPlayer::onEventCallback(void *hPlayer, void *handle, int type)
{
	HmxMediaPlayer *player = static_cast<HmxMediaPlayer *>(getPlayer(hPlayer));
	if ( !player ) return;
	
	switch ( type )
	{
	case OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED:
		return player->onPlayStateChanged(handle);
	case OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED:
		return player->onBufferStateChanged(handle);
	case OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED:
		return player->onPlayPositionChanged(handle);
	case OPLVOD_EVENT_TYPE_BUFFER_INFO_CHANGED:
		return player->onBufferInfoChanged(handle);
	case OPLVOD_EVENT_TYPE_VIDEOWINDOW_CHANGED:
		return player->onVideoWindowChanged(handle);
	case OPLVOD_EVENT_TYPE_SELECTED_COMPONENT_CHANGED:
	case OPLVOD_EVENT_TYPE_UNSELECTED_COMPONENT_CHANGED:
		return player->onSelectedComponentChanged(handle);
	case OPLVOD_EVENT_TYPE_PLAY_DURATION_CHANGED:
		return player->onPlayDurationChanged(handle);
	default:
		break;
	}
}

void HmxMediaPlayer::onPlayStateChanged(void *handle)
{
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	eOplVodPlayState state = vod->getPlayState();
	switch ( state )
	{
	case OPLVOD_PLAY_STATE_FINISHED:	
		this->m_listener->playStateChanged(MB_PLAYEVENT_FINISH);
		break;
	case OPLVOD_PLAY_STATE_ERROR:
		m_error_code = (eMPPlayError)vod->getPlayError();
		this->m_listener->playStateChanged(MB_PLAYEVENT_ERROR);
		break;
	case OPLVOD_PLAY_STATE_STOPPED:
		break;
	case OPLVOD_PLAY_STATE_PLAYING:
		this->m_listener->playStateChanged(MB_PLAYEVENT_PLAYING);
		break;
	case OPLVOD_PLAY_STATE_PAUSED:
	case OPLVOD_PLAY_STATE_CONNECTING:
	case OPLVOD_PLAY_STATE_BUFFERING:
	default:
		break;
	}
}

void HmxMediaPlayer::onBufferStateChanged(void *handle)
{
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	eOplVodBufferState state = vod->getBufferState();
	switch ( state )
	{
	case OPLVOD_BUFFER_STATE_CANPLAY:
		this->setDuration(static_cast<double>(vod->getDuration()/1000.0));
		this->m_listener->playStateChanged(MB_PLAYEVENT_CANPLAY);
		break;
	case OPLVOD_BUFFER_STATE_NOT_ENOUGH:
		this->m_listener->playStateChanged(MB_PLAYEVENT_UNDERRUN);
		break;
	case OPLVOD_BUFFER_STATE_ENOUGH:
		this->m_listener->playStateChanged(MB_PLAYEVENT_BUFFERED);
		break;
	case OPLVOD_BUFFER_STATE_FINISHED:
	default:
		break;
	}
}

void HmxMediaPlayer::onPlayPositionChanged(void *handle)
{
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	this->setPosition(static_cast<double>(vod->getPlayPosition()/1000.0));

	this->m_listener->playStateChanged(MB_PLAYEVENT_POSITION_CHANGED);
}

void HmxMediaPlayer::onPlayDurationChanged(void *handle)
{
	
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	this->setDuration(static_cast<double>(vod->getDuration()/1000.0));
	this->m_listener->playStateChanged(MB_PLAYEVENT_DURATION_CHANGED);
}

void HmxMediaPlayer::onBufferInfoChanged(void *handle)
{
	this->getBufferState()->touch();
}

void HmxMediaPlayer::onVideoWindowChanged(void *handle)
{
	this->m_listener->playStateChanged(MB_PLAYEVENT_VIDEOWINDOW_CHANGED);

}

void HmxMediaPlayer::onSelectedComponentChanged(void *handle)
{
	OplVodHtml5Handle *vod = static_cast<OplVodHtml5Handle *>(handle);
	if ( !vod ) return;

	this->m_listener->playStateChanged(MB_PLAYEVENT_COMPONENT_CHANGED);
}

