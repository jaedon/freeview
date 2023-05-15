/** **********************************************************************************************************
	@file 		hmp_backend.cpp
	
	@date		2012/11/12
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif		
	
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

#include "hmp_backend.h"

#include <uva_client.h>
#include <uva_event.h>

#include "oplvodavchandle.h"
#include "oplvideoutil.h"

#include <iostream>
#include <cstring>


using namespace std;

#define ENTRY	cout << __FILE__ << "::" << __FUNCTION__ << ":: " << "++:" << m_player << endl
#define EXIT	cout << __FILE__ << "::" << __FUNCTION__ << ":: " << "--:" << m_player << endl

static const char* s_supported_types[]  = {
	"audio/mpeg", "audio/mp2", "audio/mp3", "video/mpeg", "video/mp4", "video/h264", "video/H264",
	"video/mpeg4", "video/avi", "video/x-msvideo", "video/x-ms-asf", "video/x-ms-wmv", "audio/x-ms-wma", NULL
};


/***************************************************************************************************
 * HmpMediaBackend
 ***************************************************************************************************/
HmpMediaBackend *HmpMediaBackend::s_theApp = NULL;
hmp_winrect_t HmpMediaBackend::s_latest_window_rect;

HmpMediaBackend::HmpMediaBackend(UVABackendType type, UVAClient *client)
	: UVAMediaBackend(type),
	m_uva_type(type),
	m_uva_client(client),
	m_player(NULL)
{
	ENTRY;

	m_player = OplVodAvcHandle::create(OplVideoHandle::EHandleOwnerTypeUserOipf);
	if ( !m_player )
	{
		cerr << "FATAL: HmpMediaBackend(): Insufficient memory." << endl;
	}

	s_theApp = this;

	EXIT;
}

HmpMediaBackend::~HmpMediaBackend()
{
	ENTRY;

	if ( m_player )
	{
		OplVideoHandle::unRegist(m_player);
		delete m_player;
		m_player = NULL;
	}

	s_theApp = NULL;

	EXIT;
}

UVA_STATUS HmpMediaBackend::initBackend(const char *origin, SourceDescription **sources,
			bool automatic_key_system_selection)
{
	ENTRY;

	for ( int i=0; sources[i]!=NULL; ++i )
	{
		cout << "Sources [" << i << "]" << endl;

		if ( sources[i]->type != SourceDescription::BACKEND_STREAMING ) continue;

		cout << "Stream Type: " << sources[i]->type << endl;
		cout << "backend url: " << sources[i]->description.backend_streaming.url << endl;

		m_player->setListener(\
			reinterpret_cast<void (*)(void*, eOplVodEventType)>(onEventCallback));
		m_player->setUrl(sources[i]->description.backend_streaming.url);
		m_player->probe();
		break;
	}

	EXIT;
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::attachSource(SourceDescription *source)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::removeSource(uintptr_t source_id)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::setVideoWindow(int x, int y, int w, int h)
{
	//ENTRY;

	//cout << "x=" << x << " , y=" << y << " w=" << w << " h=" << h << endl;

	/* skip setting the window if the values of window setting is same to previous one. */
	if ( s_latest_window_rect.x == x && s_latest_window_rect.y == y
		&& s_latest_window_rect.w == w && s_latest_window_rect.h == h )
	{
		return UVA_OK;
	}

	if ( !m_player->setWindow(x, y, w, h) )
		return UVA_ERROR;
	
	s_latest_window_rect.x = x;
	s_latest_window_rect.y = y;
	s_latest_window_rect.w = w;
	s_latest_window_rect.h = h;
	
	EXIT;
	
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setClipRect(int x, int y, int w, int h)
{
	/*
	ENTRY;

	cout << "x=" << x << " , y=" << y << " w=" << w << " h=" << h << endl;

	EXIT;
	*/
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getNativeSize(int &w, int &h, double &pixel_aspect)
{
	ENTRY;

	w = OSD_WIDTH;
	h = OSD_HEIGHT;
	pixel_aspect = 0.0;

	EXIT;
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getSeekableTimeRanges(ByteRange **input_byte_ranges,
			TimeRange **&output_time_ranges)
{
	ENTRY;

#if defined(CONFIG_TIMESTAMP_64)
	unsigned long long duration = m_player->getDuration();
#else
	unsigned int duration = m_player->getDuration();
#endif
	duration /= 1000;

	if ( duration > 0 )
	{
		output_time_ranges = UVA_NEW_ARRAY(TimeRange*, 2);
		output_time_ranges[0] = UVA_NEW(TimeRange, ());
		output_time_ranges[0]->first = 0.0;
		output_time_ranges[0]->second = static_cast<double>(duration);
		output_time_ranges[1] = NULL;
	}
	else
	{
		output_time_ranges = UVA_NEW_ARRAY(TimeRange*, 1);
		output_time_ranges[0] = NULL;
	}

	EXIT;
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getBufferedTimeRanges(ByteRange **input_byte_ranges,
			TimeRange **&output_time_ranges)
{
//	ENTRY;

	unsigned int *start_times = NULL, *end_times = NULL;
	int count = m_player->getBufferingInfo(&start_times, &end_times);
	if ( count <= 0 )
	{
		output_time_ranges = UVA_NEW_ARRAY(TimeRange*, 1);
		output_time_ranges[0] = NULL;
	}
	else
	{
		output_time_ranges = UVA_NEW_ARRAY(TimeRange*, count+1);
		for ( int i=0; i<count; ++i )
		{
			output_time_ranges[i] = UVA_NEW(TimeRange, ());
			output_time_ranges[i]->first = static_cast<double>(start_times[i]/1000);
			output_time_ranges[i]->second = static_cast<double>(end_times[i]/1000);
		}
		output_time_ranges[count] = NULL;
	}

//	EXIT;
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::canPlayURL(const char *url)
{
	ENTRY;

	cout << "url: " << url << endl;

	if ( strstr(url, ".ogg")
		|| strstr(url, ".webm") )
	{
		cerr << "NOT SUPPORTED" << endl;
		return UVA_NOT_SUPPORTED;
	}

	EXIT;

	return UVA_OK;	
}

UVA_STATUS HmpMediaBackend::play()
{
	ENTRY;

	if ( !m_player->play() )
		return UVA_ERROR;

	if ( m_player->getPlayState() == OPLVOD_PLAY_STATE_PAUSED 
			|| m_player->getPlayState() == OPLVOD_PLAY_STATE_FINISHED )
	{
		m_player->setPlaySpeed(100);
	}
	
	EXIT;
	
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::stop()
{
	ENTRY;

	if ( !m_player->stop() )
		return UVA_ERROR;

	EXIT;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::pause(bool get_resources)
{
	ENTRY;

	if ( !m_player->setPlaySpeed(0) )
		return UVA_ERROR;

	EXIT;
	
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setPlaybackRate(double rate)
{
	ENTRY;

	if ( !m_player->setPlaySpeed(rate*100) )
		return UVA_ERROR;
	
	EXIT;
		
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setVolume(double volume)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::getDuration(double &duration)
{
//	ENTRY;

#if defined(CONFIG_TIMESTAMP_64)
	unsigned long long _duration = m_player->getDuration();
#else
	unsigned int _duration = m_player->getDuration();
#endif
	_duration /= 1000;
	duration = static_cast<double>(_duration);

//	cout << "duration: " << duration << endl;

//	EXIT;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getPosition(double &position)
{
	//ENTRY;

	unsigned int _pos = m_player->getPlayPosition();
	_pos /= 1000;
	position = static_cast<double>(_pos);

	//EXIT;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setPosition(double position)
{
	ENTRY;

	cout << "pos: " << position << endl;

	if ( !m_player->seek(position*1000) )
		return UVA_ERROR;
	
	/* not sure if we have to call setPlaySpeed */
	if ( m_player->getPlayState() == OPLVOD_PLAY_STATE_PLAYING
			|| m_player->getPlayState() == OPLVOD_PLAY_STATE_PAUSED
			|| m_player->getPlayState() == OPLVOD_PLAY_STATE_FINISHED )
	{
		m_player->setPlaySpeed(100);
	}

	UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::SeekCompleted));
	if ( !event ) return UVA_ERROR;
	m_uva_client->handleEvent(event);
	
	EXIT;
		
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setVisibility(bool visible, bool unloaded)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::getSizeFromDuration(double duration, size_t &size)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::writeData(UVAEvent* need_data_event, size_t bytes_written,
			WriteDataStatus status)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::canPlayType(const char *type, const char **codecs, const char *drmID)
{
	ENTRY;

	if ( !type ) return UVA_OK;

	cout << "type = " << type << endl;
	for ( int i = 0; s_supported_types[i] != NULL; i++ )
	{
		const char* _type = s_supported_types[i];
		if ( strcmp(type, _type) == 0 )
		{
			EXIT;
			return UVA_OK;
		}
	}

	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::setFullScreen(bool fullscreen)
{
	ENTRY;

	if ( !m_player->setFullScreen(fullscreen) )
		return UVA_ERROR;
	
	EXIT;
	
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getSupportedMimetypes(char **&mime_types)
{
	ENTRY;

	mime_types = UVA_NEW_ARRAY(char*, sizeof(s_supported_types));
	if ( mime_types )
	{
		int num_supported = 0;
		for ( int i = 0; s_supported_types[i] != NULL; i++ )
		{
			const char* type = s_supported_types[i];
			mime_types[num_supported] = uva_strdup(type);
			if ( mime_types[num_supported] == NULL )
				break;
			num_supported++;
		}
		mime_types[num_supported] = NULL;
	}

	EXIT;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getComponents(AVComponent **&components)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::unselectComponent(AVComponent *component)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::selectComponent(AVComponent *component)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

void HmpMediaBackend::onEventCallback(void *handle, int type)
{
	cout << __FUNCTION__ << ": type: " << type << endl;
	switch ( type )
	{
	case OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED:
		return onPlayStateChanged(handle);
	
	case OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED:
		return onBufferStateChanged(handle);
	
	case OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED:
		return onPlayPositionChanged(handle);
	
	default:
		break;
	}
}

void HmpMediaBackend::onPlayStateChanged(void *handle)
{
	HmpMediaBackend *hmp_backend = getBackend();
	if ( !hmp_backend ) return;

	UVAClient *client = hmp_backend->getClient();
	if ( !client ) return;

	OplVodAvcHandle *vod = static_cast<OplVodAvcHandle *>(handle);
	if ( !vod ) return;

	eOplVodPlayState state = vod->getPlayState();

	cout << __FUNCTION__ << ": play state: " << state << endl;

	switch ( state )
	{
	case OPLVOD_PLAY_STATE_STOPPED:
		break;
	case OPLVOD_PLAY_STATE_PLAYING:
		break;
	case OPLVOD_PLAY_STATE_PAUSED:
		break;
	case OPLVOD_PLAY_STATE_CONNECTING:
		break;
	case OPLVOD_PLAY_STATE_BUFFERING:
		break;
	case OPLVOD_PLAY_STATE_FINISHED:
	{
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::PlaybackFinished));
		if ( !event ) return;
		client->handleEvent(event);

		 event = UVA_NEW(UVAEvent, (UVAEvent::BufferingProgress));
		if ( !event ) return;
		client->handleEvent(event);
		break;
	}
	case OPLVOD_PLAY_STATE_ERROR:
	{
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::DecodeError));
		if ( !event ) return;
		client->handleEvent(event);
		break;
	}
	default:
		break;
	}
}

void HmpMediaBackend::onBufferStateChanged(void *handle)
{
	HmpMediaBackend *hmp_backend = getBackend();
	if ( !hmp_backend ) return;

	UVAClient *client = hmp_backend->getClient();
	if ( !client ) return;

	OplVodAvcHandle *vod = static_cast<OplVodAvcHandle *>(handle);
	if ( !vod ) return;

	eOplVodBufferState state = vod->getBufferState();

	cout << __FUNCTION__ << ": buffer state: " << state << endl;

	switch ( state )
	{
	case OPLVOD_BUFFER_STATE_CANPLAY:
	{
#if defined(CONFIG_TIMESTAMP_64)
		unsigned long long dur = vod->getDuration();
#else
		unsigned dur = vod->getDuration();
#endif
		cout << "duratioin=" << dur << endl;
		UVAEvent *event = UVA_NEW(DurationChangedEvent, (dur/1000));
		if ( !event ) return;
		client->handleEvent(event);

		event = UVA_NEW(VideoResizedEvent, (s_latest_window_rect.w, s_latest_window_rect.y, 0));
		if ( !event ) return;
		client->handleEvent(event);
		break;
	}

	case OPLVOD_BUFFER_STATE_NOT_ENOUGH:
	{
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingStreamUnderrun));
		if ( !event ) return;
		client->handleEvent(event);
		break;
	}
	
	case OPLVOD_BUFFER_STATE_ENOUGH:
	{
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingStreamBuffered));
		if ( !event ) return;
		client->handleEvent(event);
		break;
	}
	
	case OPLVOD_BUFFER_STATE_FINISHED:
		break;
	
	default:
		break;
	}
}

void HmpMediaBackend::onPlayPositionChanged(void *handle)
{
	HmpMediaBackend *hmp_backend = getBackend();
	if ( !hmp_backend ) return;

	OplVodAvcHandle *vod = static_cast<OplVodAvcHandle *>(handle);
	if ( !vod ) return;

	UVAClient *client = hmp_backend->getClient();
	if ( !client ) return;

	UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingProgress));
	if ( !event ) return;
	client->handleEvent(event);
}
