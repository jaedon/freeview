/** **********************************************************************************************************
	@file 		hmpbackend.cpp

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

#include "hmpbackend.h"
#include "hmxmediaplayer.h"
#include "msemediaplayer.h"
#include "msemediasource.h"

#include "oplvodhtml5handle.h"
#include "oplvideoutil.h"

#include <uva_client.h>
#include <uva_event.h>

#include "backend_log.h"

#include <iostream>
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <list>

using namespace std;

LOG_CAT(backend);

#ifdef DEBUG
//#define DUMP_SAMPLES_TO "/tmp/samples"
#define DUMP_DECRYPTED "/tmp/decrypted"
#endif

#define BACKEND_CHECK_INIT(p)	\
	do {	\
		if ( !p ) return UVA_OK;	\
	} while(0)

#if ENABLE_MSE
static const char *__supported_mimetypes[]  = {
	"audio/mpeg", "audio/mp2", "audio/mp3", "audio/mp4", "video/mpeg", "video/mp4",
	"video/h264", "video/H264", "video/mpeg4", "video/avi", "video/x-msvideo",
	"video/x-ms-asf", "video/x-ms-wmv", "audio/x-ms-wma", "application/vnd.apple.mpegurl",
	"application/vnd.ms-playready.initiator+xml", "application/vnd.ms-sstr+xml", "application/dash+xml",
	"application/x-mpegurl", "application/vnd.oipf.ContentAccessStreaming+xml", "video/mp2t", NULL
};
#else
static const char *__supported_mimetypes[]  = {
	"audio/mpeg", "audio/mp2", "audio/mp3", "video/mpeg", "video/mp4",
	"video/h264", "video/H264", "video/mpeg4", "video/avi", "video/x-msvideo",
	"video/x-ms-asf", "video/x-ms-wmv", "audio/x-ms-wma", "application/vnd.apple.mpegurl",
	"application/vnd.ms-playready.initiator+xml", "application/vnd.ms-sstr+xml", "application/dash+xml",
	"application/x-mpegurl", "application/vnd.oipf.ContentAccessStreaming+xml", "video/mp2t", NULL
};
#endif


#define ENABLE_TASK 1

#define ONETIME_SAMPLE_COUNT 60
#define INF (1.0/0.0)

static const char *__not_supported_ext[] = {
	".ogg", ".webm", NULL
};

/***************************************************************************************************
 * HAVComponent
 ***************************************************************************************************/

HAVComponent::HAVComponent(UVABackend::AVComponentType _type, const char* _encoding, bool _encrypted,
		double _aspect_ratio, const char* _language, bool _audio_description, int _audio_channels,
		bool _hearing_impaired, Kind _kind, const char* _label, const char* _id, int _bandwidth,
		const char* _representation_id, uintptr_t _source_id) :
		AVComponent::AVComponent(_type, _encoding, _encrypted, _aspect_ratio, _language, _audio_description,
			_audio_channels, hearing_impaired, _kind, _label, _id, _bandwidth, _representation_id, _source_id)
{

}

void HAVComponent::setActive(bool active)
{
	is_active = active;
}

void HAVComponent::backendMarkUnused()
{
	AVComponent::backendMarkUnused();
}

/***************************************************************************************************
 * HmpMediaBackend
 ***************************************************************************************************/
std::vector<HmpMediaBackend *>	HmpMediaBackend::s_backends=std::vector<HmpMediaBackend *>();
pthread_mutex_t	HmpMediaBackend::s_backend_mutex = PTHREAD_MUTEX_INITIALIZER;

HmpMediaBackend::HmpMediaBackend(UVABackendType type, UVAClient *client)
	: UVAMediaBackend(type),
	m_uva_type(type),
	m_uva_client(client),
	m_player(NULL),
	m_key_system_id(NULL),
	m_drm_backend(NULL),
	m_is_first(false),
	m_media_state(HMP_MEDIA_STATE_IDLE),
	m_autoplay(false),
	m_task_thread(0),
	m_quit_task(false),
	m_last_seekposition(0)
{
	LOG_ENTRY;
	pthread_mutex_lock(&s_backend_mutex);
	s_backends.push_back(this);
	pthread_mutex_unlock(&s_backend_mutex);

	pthread_mutex_init(&m_mutex, NULL);
}

HmpMediaBackend::~HmpMediaBackend()
{
	LOG_ENTRY;

	if( !this->m_avcomponents.empty() )
	{
		UVAClient *client = this->getClient();

		for( std::list<HAVComponent *>::iterator it = m_avcomponents.begin(); it!=m_avcomponents.end(); ++it )
		{
			HAVComponent *component = (*it);
			if( component )
			{
				component->backendMarkUnused();
				UVAEvent *event = UVA_NEW(ComponentChangedEvent, (component, ComponentChangedEvent::REMOVED));
				if ( event ) client->handleEvent(event);
			}
		}
		m_avcomponents.clear();
	}


#if ENABLE_TASK
	if ( m_task_thread != 0 )
	{
		pthread_mutex_lock(&m_mutex);
		m_quit_task = true;
		pthread_mutex_unlock(&m_mutex);

		int status = 0;
		pthread_join(m_task_thread, reinterpret_cast<void **>(&status));
		m_task_thread = 0;
		pthread_mutex_destroy(&m_mutex);
	}

	if ( m_player )
	{
		delete m_player;
		m_player = NULL;
	}
#else
	if ( m_player )
	{
		delete m_player;
		m_player = NULL;
	}
#endif

	if (m_key_system_id)
		uva_free(m_key_system_id);
	m_key_system_id = NULL;

	pthread_mutex_lock(&s_backend_mutex);
	std::vector<HmpMediaBackend*>::iterator player;
	for( player = s_backends.begin(); player != s_backends.end(); player++ )
	{
		if( *player == this )
		{
			s_backends.erase(player);
			break;
		}
	}
	pthread_mutex_unlock(&s_backend_mutex);
}

UVA_STATUS HmpMediaBackend::initBackend(const char *origin, SourceDescription **sources,
			bool automatic_key_system_selection)
{
	LOG_ENTRY;

	LOG_INFO(" initBackend : origin - %s\n", origin);
	//LOG_INFO(" initBackend : automatic_key_system_selection - %s\n", automatic_key_system_selection? "TRUE":"FALSE");

	for ( int i=0; sources[i]!=NULL; ++i )
	{
		if ( sources[i]->type == SourceDescription::BACKEND_STREAMING )
		{
			LOG_INFO("BACKEND_STREAMING::SoruceDescriptor::mimetype=%s, url=%s\n",
								(sources[i]->mime_type) ? sources[i]->mime_type : "null",
								sources[i]->description.backend_streaming.url);
			if ( !initHmxMP(sources[i]) ) return UVA_NOT_SUPPORTED;
			break;
		}
	}

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::attachSource(SourceDescription *source)
{
	LOG_ENTRY;

	if( !m_player )
		if( false == initMseMP() )	return UVA_ERROR;

	if( source )
	{
		LOG_INFO("source[%d] has been attached.\n", source->source_id);
		LOG_INFO("initMseMP: source->mime_type=[%s] source_id=[%d] duration=[%d] \n",  source->mime_type,
												source->source_id,
												source->description.opera_streaming.content_size);
		MseMediaSource *s = NULL;
		MseMediaSource::eMediaType type = MseMediaSource::UNKNOWN;
		if ( strstr(source->mime_type, "video") != NULL )
		{
			type = MseMediaSource::VIDEO;
		}
		else if ( strstr(source->mime_type, "audio") != NULL )
		{
			type = MseMediaSource::AUDIO;
		}

		s = new MseMediaSource(source->mime_type,
				source->source_id, type);
		if( !s ) return UVA_ERROR;

		m_is_first = true;

		if(0 < source->description.opera_streaming.content_size)
			m_player->setDuration(source->description.opera_streaming.content_size);

		if(!m_player->attachSource(s)) return UVA_ERROR;

		setMediaState(HMP_MEDIA_STATE_CANPLAY);

	}
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::removeSource(uintptr_t source_id)
{
	LOG_ENTRY;

	if ( !m_player ) return UVA_OK;
	if ( !m_player->detachSource(source_id) ) return UVA_ERROR;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setVideoWindow(int x, int y, int w, int h)
{
	//LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	//LOG_INFO("x=%d:y=%d:w=%d:h=%d\n", x, y, w, h);

	if ( m_rect.x == x && m_rect.y == y && m_rect.w == w && m_rect.h == h )
	{
		// skip duplicated window setting
		return UVA_OK;
	}

	if ( !m_player->setVideoWindow(x, y, w, h) )
		return UVA_ERROR;

	m_rect.x = x; m_rect.y = y; m_rect.w = w; m_rect.h = h;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setClipRect(int x, int y, int w, int h)
{
	//LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	//LOG_INFO("x=%d:y=%d:w=%d:h=%d\n", x, y, w, h);

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getNativeSize(int &w, int &h, double &pixel_aspect)
{
	LOG_ENTRY;
	UVA_STATUS hRet = UVA_OK;

	if(0!=strstr(m_player->getName().c_str(), MSE_MP))
	{
		hRet = UVA_NOT_INITIALIZED;
	}
	else if(0!=strstr(m_player->getName().c_str(), HMX_MP))
	{
		// getWindowSize 는 video stream size를 가져오면 안되고, video window size를 가져와야 함
		//if(m_player->getWindowSize(&w, &h) != true)
		//{
			w = OSD_WIDTH;
			h = OSD_HEIGHT;
			pixel_aspect = 1;
		//}
		//else
		//{
		//	pixel_aspect = 1;
		//}

	}
	else
	{
		hRet = UVA_NOT_INITIALIZED;
	}
	return hRet;
}

UVA_STATUS HmpMediaBackend::getSeekableTimeRanges(ByteRange **input_byte_ranges,
			TimeRange **&output_time_ranges)
{
//	LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	unsigned long long *seekable_start = NULL, *seekable_end = NULL;
	int count = m_player->getSeekableInfo(&seekable_start, &seekable_end);
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
			output_time_ranges[i]->first = static_cast<double>(seekable_start[i]/1000);
			output_time_ranges[i]->second = static_cast<double>(seekable_end[i]/1000);
		}
		output_time_ranges[count] = NULL;
	}

	return UVA_OK;

	//	LOG_ENTRY;
	// BBC 문제로 인하여 seekable range 를 무조건 0~duration 범위로 올려주도록 수정
	// 확인 이후에 app 문제인지 media 문제인지 수정 방향 결정
	/*
	BACKEND_CHECK_INIT(m_player);

	unsigned int duration = m_player->getDuration();
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

	return UVA_OK;
	*/
}

UVA_STATUS HmpMediaBackend::getBufferedTimeRanges(ByteRange **input_byte_ranges,
			TimeRange **&output_time_ranges)
{
	//LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

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

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::canPlayURL(const char *url)
{
	LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);
	for ( int i=0; __not_supported_ext[i]!=NULL; ++i )
		if ( strstr(url, __not_supported_ext[i]) ) return UVA_NOT_SUPPORTED;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::play()
{
	LOG_ENTRY;

	if ( !m_player ) return UVA_NOT_INITIALIZED;

	if ( getMediaState() == HMP_MEDIA_STATE_IDLE )
	{
		LOG_INFO("This is by autoplay.\n");
		setAutoPlay(true);
		return UVA_OK;
	}

	if ( !m_player->play() )
	{

		return UVA_ERROR;
	}

	setMediaState(HMP_MEDIA_STATE_PLAY);

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::pause(bool get_resources)
{
	LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	if ( getMediaState() == HMP_MEDIA_STATE_IDLE )
	{
		LOG_INFO("Current state is not playing state.(%d)\n", getMediaState());
		return UVA_OK;
	}

	if ( !m_player->pause() )
		return UVA_ERROR;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setPlaybackRate(double rate)
{
	LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	if ( !m_player->setPlaybackRate(rate) )
		return UVA_ERROR;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setVolume(double volume)
{
	LOG_ENTRY;

	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::getDuration(double &duration)
{
	//LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	duration = m_player->getDuration();

	//LOG_INFO("Duration:%f\n", duration);

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getPosition(double &position)
{
//	LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	position = m_player->getPosition();

//	LOG_INFO("Position:%f\n", position);

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setPosition(double position)
{
	LOG_ENTRY;

	BACKEND_CHECK_INIT(m_player);

	LOG_INFO("setPosition:%f\n", position);

#if ENABLE_TASK
 	if ( m_player->getName() == MSE_MP )
	{
		m_seeks.clear();
		m_seeks.push_front(position);
	}
	else if ( m_player->getName() == HMX_MP )
	{
		m_seeks.clear();
		m_seeks.push_front(position);
		if ( !m_player->seek(position) )
			return UVA_ERROR;
	}
	else
	{
		return UVA_ERROR;
	}
#else
	if ( !m_player->seek(position) )
		return UVA_ERROR;

	setMediaState(HMP_MEDIA_STATE_SEEKING);
#endif

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getSizeFromDuration(double duration, size_t &size)
{
	LOG_ENTRY;
	return UVA_NOT_SUPPORTED;
}


UVA_STATUS HmpMediaBackend::writeData(UVAEvent* need_data_event, size_t bytes_written,
			WriteDataStatus status)
{
	int sample_count = 0;
	int data_size = 0;
	BACKEND_CHECK_INIT(m_player);
	LOG_INFO("++++++ \n");

	//LOG_INFO("bytes_written=[%d], type=[%d], status=[%d]\n", bytes_written, need_data_event->type, status);

	if ( need_data_event->type != UVAEvent::NeedMSEData )
	{
		return UVA_OK;
	}

	MseMediaPlayer *mse_mp = dynamic_cast<MseMediaPlayer *>(m_player);
	if ( !mse_mp ) return UVA_ERROR;

	NeedMSEDataEvent* evt = static_cast<NeedMSEDataEvent*>(need_data_event);
	for ( int cnt=0; evt->samples[cnt]; cnt++ )
	{
		UVADecoderBuffer* b = evt->samples[cnt];

		if (b->GetKID() && getDRMBackend())
		{
			// 만약에 동시에 2개의 Device가 같은 영상을 재생 할 경우,
			// Youtube에서 releaseDecrypter()가 불리면서 재생중인 Sample의 KID를 가진 Decrypter를 지우게 된다.
			// Sample에 맞는 KID의 Decrypter가 없을 경우,
			// doDRMDecryptDecoderBuffer()에서는 지워진 KID의 Decrypter를 새로 생성하여 Decryption을 하면서 Delay(1초이상)가 생긴다.
			if(!doDRMDecryptDecoderBuffer(b))
			{
				LOG_ERROR("DRMDecryption for Encrypted sample failed \n");
				status = WRITE_DATA_ERROR;
				break;
			}
		}
		else if(b->GetKID())
			LOG_INFO("Encrypted sample but No DRM set\n");

		if ( status == WRITE_DATA_OK )
		{
			MseBuffer buffer;
			buffer.pts = b->pts_sec;
			buffer.dts = b->dts_sec;
			buffer.duration = b->duration_sec;
			buffer.data_size = b->GetDataSize();
			data_size+=buffer.data_size;
			sample_count = cnt;
			buffer.data = reinterpret_cast<void*>(const_cast<unsigned char *>(b->GetWritableData()));//(b->GetData()));

			//LOG_INFO("b->pts_sec : %f, m_last_seekposition : %f \n", b->pts_sec, m_last_seekposition);
		  	/*
			if( b->pts_sec < m_last_seekposition )
			{
				//1 #### Skip Stream
				printf("skip !!!!!!!!!! [%s] \n", UVAStreamConfig::av(b->config.codec) == UVAStreamConfig::AUDIO_STREAM ? "AUDIO" : "VIDEO");
			}
			else
		  	*/
			{
				if ( UVAStreamConfig::av(b->config.codec) == UVAStreamConfig::AUDIO_STREAM )
				{
					mse_mp->pushAudio(&buffer);
				}
				else if ( UVAStreamConfig::av(b->config.codec) == UVAStreamConfig::VIDEO_STREAM )
				{
		  	  		if( b->pts_sec < m_last_seekposition )
			  	 	{

			  	  	}
			  	  	else
			  	 	{
					mse_mp->pushVideo(&buffer);
				}
			}
		  	}
		}
	}

	if ( status == WRITE_DATA_EOS )
	{
		LOG_INFO("WRITE_DATA_EOS......\n");
		mse_mp->setEos(evt->source_id);
	}
	else if ( status == WRITE_DATA_ERROR )
	{
		LOG_INFO("WRITE_DATA_ERROR......\n");
	}
	mse_mp->unlockSource(evt->source_id);

	LOG_INFO("------ [sample count : %5d | data size : %10d | source id : %d] \n",
			sample_count+1, data_size, evt->source_id);

	UVA_DELETE(evt);

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::canPlayType(const char *type, const char **codecs, const char *drmID)
{
	LOG_ENTRY;

	bool supported_type = false;

	if ( !type ) return UVA_OK;

	LOG_INFO("mimetype: %s\n", type);
	for ( int i=0; __supported_mimetypes[i]!=NULL; ++i )
		if ( strcmp(type,  __supported_mimetypes[i]) == 0 )
			supported_type = true;

#if !ENABLE_MSE
	unsigned int codec_count = 0;
#endif
	for ( int i=0; codecs[i]!=NULL; ++i )
	{
#if !ENABLE_MSE
		codec_count++;
#endif
		LOG_INFO("codec information = %s\n", codecs[i]);
	}
#if !ENABLE_MSE
	if ( codec_count <= 1 )
	{
		LOG_INFO("we don't support MSE streaming.(return NOT_SUPPORTED)\n");
		return UVA_NOT_SUPPORTED;
	}
#endif

	LOG_INFO("drmId: %s\n", drmID? drmID: "NULL");

	if( !supported_type && drmID==NULL )
		return UVA_NOT_SUPPORTED;

	if( drmID )
	{
		char ** drm_backends_ids = NULL;
		UVA_STATUS ret = UVADRMBackendManager::getSupportedBackendIDs((char**&)*(&drm_backends_ids));
		if ( ret != UVA_OK )
		{
			LOG_ERROR("UVADRMBackendManager::getSupportedBackendIDs Failed. return: %d\n", ret);
			return UVA_ERROR;
		}
		supported_type = false;
		unsigned int count = 0;

		while( drm_backends_ids[count] )
		{
			if (!strcmp(drm_backends_ids[count], drmID))
			{
				supported_type = true;
		//		break;
			}
			uva_free(drm_backends_ids[count]);
			count++;
		}
		UVA_DELETE_ARRAY(drm_backends_ids);
	}

	LOG_INFO("this type is %s\n", supported_type ? "supported" : "not supported");

	return ( supported_type ) ? UVA_OK : UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::setFullScreen(bool fullscreen)
{
	LOG_ENTRY;

	if ( !m_player->setFullScreen(fullscreen) )
		return UVA_ERROR;

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getSupportedMimetypes(char **&mime_types)
{
	LOG_ENTRY;

	mime_types = UVA_NEW_ARRAY(char*, sizeof(__supported_mimetypes));
	if ( mime_types )
	{
		int num_supported = 0;
		for ( int i=0; __supported_mimetypes[i]!=NULL; ++i )
		{
			const char* type = __supported_mimetypes[i];
			mime_types[num_supported] = uva_strdup(type);
			if ( mime_types[num_supported] == NULL )
				break;
			num_supported++;
		}
		mime_types[num_supported] = NULL;
	}

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::setContentSize(uintptr_t source_id, size_t content_size)
{
	LOG_ENTRY;

	LOG_INFO("setContentSize: source=[%d], duration[%d].\n", source_id, content_size);
	m_player->setDuration(static_cast<double>(content_size));

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getStartDate(double &date)
{
	LOG_ENTRY;

	double _date;
	if ( !m_player ) return UVA_NOT_INITIALIZED;

	_date = m_player->getStartDate();
	if( _date<0 ) return UVA_ERROR;

	LOG_INFO("getStartDate: %f\n", _date);
	date = _date;

	return UVA_OK;
}

//when uva client remove drmbackend, uva backend don't know that.
// So when get drmback end, we have to use the uva client
UVADRMBackend *HmpMediaBackend::getDRMBackend()
{
	//return m_drm_backend;
	if(m_key_system_id)
	{
		m_drm_backend = m_uva_client->getDRMBackend(m_key_system_id);
	}
	else
		m_drm_backend = NULL;

	return m_drm_backend;
}

UVA_STATUS HmpMediaBackend::setKeySystem(const char* key_system_id)
{
	LOG_ENTRY;

	if( m_key_system_id )
	{
		//free all decrypters
		if( m_drm_backend && m_player )
		{
			MseMediaPlayer* mse_mp = (MseMediaPlayer * )m_player;
			mse_mp->releaseAllDecrypter(m_drm_backend);
		}
		uva_free(m_key_system_id);
		m_key_system_id = NULL;
	}

	if( key_system_id )
	{
		m_drm_backend = m_uva_client->getDRMBackend(key_system_id);
		m_key_system_id = uva_strdup(key_system_id);
		if( !m_key_system_id )
			return UVA_ERROR;
	}
	else
		return UVA_ERROR;
	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::getVideoFrame(unsigned char* buffer, size_t buffer_size)
{
	LOG_INFO(" NOT IMPLEMENTED !\n");
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::setLoop(const char* value)
{
	LOG_INFO(" NOT IMPLEMENTED !\n");
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::selectComponent(AVComponent* component)
{
	LOG_ENTRY;
	if ( !m_player ) return UVA_NOT_INITIALIZED;

	if( component )
	{
		int i = 0;
		for( std::list<HAVComponent *>::iterator it = m_avcomponents.begin(); it!=m_avcomponents.end(); ++it )
		{
			HAVComponent *hcomponent = (*it);
			LOG_INFO("selectComponent: hcomponent (%p), component (%p) \n", hcomponent, component);
			if( hcomponent == component )
			{
				LOG_INFO("selectComponent: component(%p), type(%d), id(%d)\n", component, component->type, component->source_id);
				hcomponent->setActive(true);
				m_player->selectComponent((eMpPlayAVComponentType)component->type, i);
				break;
			}
			if( component->type == hcomponent->type ) ++i;
		}
	}

	return UVA_OK;
}

UVA_STATUS HmpMediaBackend::unselectComponent(AVComponent* component)
{
	LOG_ENTRY;

	if ( !m_player ) return UVA_NOT_INITIALIZED;

	if( component )
	{
		int i = 0;
		for( std::list<HAVComponent *>::iterator it = m_avcomponents.begin(); it!=m_avcomponents.end(); ++it )
		{
			HAVComponent *hcomponent = (*it);
			LOG_INFO("unselectComponent: hcomponent (%p), component (%p) \n", hcomponent, component);
			if( hcomponent == component )
			{
				LOG_INFO("unselectComponent: component(%p), type(%d), id(%d)\n", component, component->type, component->source_id);
				hcomponent->setActive(false);
				m_player->unselectComponent((eMpPlayAVComponentType)component->type, i);
				break;
			}
			if( component->type == hcomponent->type ) ++i;
		}
	}

	return UVA_OK;

}

UVA_STATUS HmpMediaBackend::setSource(const char* id)
{
	LOG_INFO(" NOT IMPLEMENTED !\n");
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::onListenerChanged(const char* event_name)
{
	LOG_INFO(" NOT IMPLEMENTED !\n");
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::setBufferingStrategy(BufferingStrategy strategy)
{
	LOG_INFO(" NOT IMPLEMENTED !\n");
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS HmpMediaBackend::getBytesInBuffer(size_t &bytes)
{
	LOG_INFO(" NOT IMPLEMENTED !\n");
	return UVA_NOT_SUPPORTED;
}



/***************************************************************************************************
 * private functions
 ***************************************************************************************************/
bool HmpMediaBackend::initHmxMP(SourceDescription *source)
{
	m_player = new HmxMediaPlayer();
	if ( !m_player ) return false;

#if ENABLE_TASK
	createThread();
#endif

	MediaSource *s = NULL;
	if ( source )
	{
		s = new MediaSource((source->mime_type) ? source->mime_type : "",
					source->description.backend_streaming.url);
		if ( !s ) return false;
		if ( !m_player->attachSource(s) ) return false;
	}

	m_player->setListener(this);
	return m_player->init(s);
}

bool HmpMediaBackend::initMseMP(void)
{
	if(!m_player)
	{
		pthread_mutex_lock(&s_backend_mutex);

		// 오직 1개의 MseMediaPlayer만 존재해야 함
		if (MseMediaPlayer::clearPlayers() == true)
		{

			// MseMediaPlayer object가 삭제되엇으니, m_player도 삭제되어야 함
			std::vector<HmpMediaBackend*>::iterator it;
			for( it = s_backends.begin(); it != s_backends.end(); it++ )
			{
				(*it)->removePlayer();
			}
		}

		pthread_mutex_unlock(&s_backend_mutex);

		m_player = new MseMediaPlayer();
		if(!m_player) return false;

		m_player->setListener(this);
		m_player->init(NULL);

#if ENABLE_TASK
		createThread();
#endif
	}
	return true;
}

bool HmpMediaBackend::doDRMDecryptDecoderBuffer(UVADecoderBuffer *b)
{
	MseMediaPlayer *mse_mp = dynamic_cast<MseMediaPlayer *>(m_player);
	if ( !mse_mp ) return false;
	enum{ IDX_AUDIO=0, IDX_VIDEO=1 };

	int index = UVAStreamConfig::av(b->config.codec) == UVAStreamConfig::AUDIO_STREAM ? IDX_AUDIO : IDX_VIDEO;
	MseMediaSource *as = NULL;

	if(index == IDX_AUDIO)
		as = dynamic_cast<MseMediaSource *>(mse_mp->getActiveAudioSource());
	else
		as = dynamic_cast<MseMediaSource *>(mse_mp->getActiveVideoSource());
	if ( !as ) return false;

	MseMediaSource::DecryptionContext &decryption_ctx = as->m_decryption_context;

	const unsigned char* kid = b->GetKID();
	size_t kid_size = b->GetKIDSize();
	const unsigned char* iv = b->GetIV();
	size_t iv_size = b->GetIVSize();

		if( decryption_ctx.m_last_kid && !memcmp(decryption_ctx.m_last_kid, kid, kid_size))
	{ //same
			decryption_ctx.m_last_kid_ptr = kid;
		}
		else
	{ //diff
		if(!decryption_ctx.m_last_kid)
		{
				decryption_ctx.m_last_kid = UVA_NEW_ARRAY(unsigned char, kid_size);
			if(decryption_ctx.m_last_kid == NULL)
			{
				LOG_ERROR("UVA_NEW_ARRAY fail m_last_kid\n");
				return false;
			}
			memset(decryption_ctx.m_last_kid, 0x00, kid_size);
			LOG_INFO("[%s] UVA_NEW_ARRAY\n", index==IDX_AUDIO?"AUDIO":"VIDEO");
		}
#ifdef DEBUG
		LOG_INFO("[%s] OLD KID: ", index==IDX_AUDIO?"AUDIO":"VIDEO");
		for (int i = 0; i < (int)kid_size; i++) {
			fprintf(stderr, "%.2x",decryption_ctx.m_last_kid[i]);
		}
		fprintf(stderr, "\n");
		LOG_INFO("[%s] NEW KID: ", index==IDX_AUDIO?"AUDIO":"VIDEO");
		for (int i = 0; i < (int)kid_size; i++) {
			fprintf(stderr, "%.2x",kid[i]);
		}
		fprintf(stderr, "\n");
#endif
		memcpy(decryption_ctx.m_last_kid, kid, kid_size);

			if(decryption_ctx.m_decrypter)
		{
				getDRMBackend()->releaseDecrypter(decryption_ctx.m_decrypter);
			decryption_ctx.m_decrypter = NULL;
			}
		}

	if(getDRMBackend()->getDecrypter(decryption_ctx.m_decrypter, decryption_ctx.m_last_kid, kid_size, false) != UVA_OK)
	{
			LOG_ERROR("[%s] ERROR getDecrypter\n", index==IDX_AUDIO?"AUDIO":"VIDEO");
	}

	if( decryption_ctx.m_decrypter )
	{
		unsigned char* encrypted = b->GetWritableData();
		unsigned char* decrypted = UVA_NEW_ARRAY(unsigned char, b->GetDataSize());
		memset(decrypted, 0x00, b->GetDataSize());

		if(decrypted == NULL)
		{
			LOG_ERROR("Out of Memory\n");
			return false;
		}
		unsigned int pulTotalEncBufSize = 0;
		unsigned int ulIdx = 0;

		UVADecoderBuffer::SubSampleEncryptionInfo* info = b->GetSubSampleDecryptionData();
		UVADecoderBuffer::SubSampleEncryptionInfo* next = NULL;
		unsigned int total = 0;
		bool success = true;

		while(info)
		{
			next = info->next;
			encrypted += info->unencrypted_data_count;

			memcpy(decrypted+pulTotalEncBufSize, encrypted, info->encrypted_data_count);

			pulTotalEncBufSize += info->encrypted_data_count;

			encrypted += info->encrypted_data_count;
			total += info->encrypted_data_count + info->unencrypted_data_count;
			info = next;
		}

		if( decryption_ctx.m_decrypter->decryptData(decrypted, pulTotalEncBufSize, decrypted, pulTotalEncBufSize, iv, iv_size) != UVADRMBackend::UVADRMDecrypter::OK )
		{
			success = false;
		}
		else if (iv)
		{
			iv = NULL;
			iv_size = 0;
		}

		info = b->GetSubSampleDecryptionData();
		encrypted = b->GetWritableData();
		while(info)
		{
			next = info->next;
			encrypted += info->unencrypted_data_count;
			memcpy(encrypted, decrypted+ulIdx, info->encrypted_data_count);
			ulIdx += info->encrypted_data_count;
			encrypted+= info->encrypted_data_count;

			UVA_DELETE(info);
			info = next;
		}

		UVA_DELETE_ARRAY(decrypted);

		if( success && total == b->GetDataSize())
		{
			b->SetUnencrypted();
			if( decryption_ctx.m_decrypter != NULL)
			{
				getDRMBackend()->releaseDecrypter(decryption_ctx.m_decrypter);
				decryption_ctx.m_decrypter = NULL;
			}
			return true;
		}
		else
		{
			LOG_ERROR("@@@@@@@@@@@@@@@@@@@@ decrypteData failed @@@@@@@@@@@@@@@@@ \n");
	}
	}
	else
	{
		LOG_INFO("[%d] decryption_ctx.m_decrypter = %x\n", __LINE__, decryption_ctx.m_decrypter);
		LOG_ERROR("NO Decrypter\n");
	}

	if(decryption_ctx.m_decrypter != NULL)
	{
		getDRMBackend()->releaseDecrypter(decryption_ctx.m_decrypter);
		decryption_ctx.m_decrypter = NULL;
	}

	return false;
}


#if ENABLE_TASK
bool HmpMediaBackend::__seek(HmpMediaBackend *hmp, double pos)
{
	LOG_INFO("position=[%f]\n", pos);

	MediaPlayer *mp = hmp->getPlayer();

 	if ( mp->getName() == HMX_MP )
 	{
 		HmxMediaPlayer *hmx_mp = dynamic_cast<HmxMediaPlayer *>(mp);
		if (hmx_mp && !hmx_mp->seek(pos) ) return false;
 	}
 	else if ( mp->getName() == MSE_MP )
 	{
 		MseMediaPlayer *mse_mp = dynamic_cast<MseMediaPlayer *>(mp);
		if ( mse_mp && !mse_mp->seek(pos) ) return false;
	}

	return true;
}

void HmpMediaBackend::createThread()
{
	m_quit_task = false;

	int r = pthread_create(&m_task_thread, NULL, threadFunc, this);
	if ( r != 0 )
	{
		LOG_INFO("Cannot create threadFunc task.\n");
	}
}
#endif

/***************************************************************************************************
 * static functions
 ***************************************************************************************************/
UVADRMBackend* HmpMediaBackend::getDRMBackend(void* me, const char* id)
{
	//LOG_ENTRY;
	LOG_INFO("getDRMBackend: %s\n", id);

	HmpMediaBackend *hmp = static_cast<HmpMediaBackend *>(me);
	return hmp->m_uva_client->getDRMBackend(id);
}


void HmpMediaBackend::playStateChanged(eMpPlayEvent event, long long int param)
{
	UVAClient *client = this->getClient();

	MediaPlayer *player = this->getPlayer();
	if ( !player ) return;

	switch ( event )
	{
	case MB_PLAYEVENT_CANPLAY:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_CANPLAY\n");
		unsigned dur = player->getDuration();
		UVAEvent *event;

		LOG_INFO("duration=[%d]\n", dur);



        if(dur != 0)
        {
            for( int i = 0; i<MP_COMPONENT_TYPE_MAX; ++i )
            {
                int active_index = -1;
                int c = player->getComponentCount((eMpPlayAVComponentType)i);
                LOG_INFO("getComponentCount=[%d], type = %d\n", c, i);
                if( c > 0 )
                {
                    active_index = player->getActiveComponentIndex((eMpPlayAVComponentType)i);
                }

                for( int j = 0; j<c; ++j )
                {
                    MpPlayAVComponent_t avcomponet;
                    player->getComponent((eMpPlayAVComponentType)i, j, &avcomponet);
                    AVComponent::Kind kind = (AVComponent::Kind)__change_component_role(avcomponet.compRole);

                    HAVComponent* component = new HAVComponent(
                        (AVComponentType)i,             //AVComponentType _type,
                        avcomponet.encoding,            //const char* _encoding,
                        0,                              //bool _encrypted,
                        avcomponet.fAspectRatio,        //double _aspect_ratio,
                        avcomponet.szLanguage,          //const char* _language,
                        avcomponet.bAudioDescription,   //bool _audio_description,
                        avcomponet.ulAudioChannels,     //int _audio_channels,
                        0,                              //bool _hearing_impaired,
                        kind,                           //Kind _kind,
                        NULL,                           //const char* _label,
                        avcomponet.compName,            //const char* _id,
                        0,                              //int _bandwidth,
                        NULL,                           //const char* _representation_id,
                        (uintptr_t)avcomponet.ulPid     //uintptr_t _source_id
                        );

                    if( j == active_index )
                    {
                        LOG_INFO("current active index = %d\n", active_index);
                        component->setActive(true);
                    }

                    m_avcomponents.push_back(component);

                    LOG_INFO("UVAEvent::ComponentChangedEvent ADDED : type(%d), encoding(%s), aspectratio(%f), language(%s), audio description(%d), audio channel(%d), kind(%d), comp name(%s), sourid(%d), component(%p)\n",
                        i, avcomponet.encoding, avcomponet.fAspectRatio, avcomponet.szLanguage, avcomponet.bAudioDescription,
                        avcomponet.ulAudioChannels, kind, avcomponet.compName, avcomponet.ulPid, component);
                    event = UVA_NEW(ComponentChangedEvent, (component, ComponentChangedEvent::ADDED));
                    if ( !event )    return;
                    client->handleEvent(event);
                }
            }

            /* to make canplay event to this media element of javascript layer, 
            we have to make duration changed event and immediately video resize event. 
            so we pre-calculate the pixel_aspect */
            int w = 0, h = 0;
            player->getWindowSize(&w, &h);
            double pixel_aspect = (double)(OSD_WIDTH * h) / (double)(OSD_HEIGHT * w);

            LOG_INFO("UVAEvent::DurationChangedEvent\n");
            if( player->getDurationInfinity() )
				event = UVA_NEW(DurationChangedEvent, INF);
			else
			    event = UVA_NEW(DurationChangedEvent, (dur));
			if ( !event ) return;
			client->handleEvent(event);

            LOG_INFO("UVAEvent::VideoResizedEvent\n");
            event = UVA_NEW(VideoResizedEvent, (w, h, pixel_aspect));
            if ( !event )     return;
            client->handleEvent(event);
		}

        this->setMediaState(HMP_MEDIA_STATE_CANPLAY);

        if ( this->getAutoPlay() )
		{
			player->play();
			this->setMediaState(HMP_MEDIA_STATE_CANPLAY);
		}

		// work around
		/////////////////////////////////////////////////
        event = UVA_NEW(UVAEvent, (UVAEvent::BufferingStreamUnderrun));
		if ( !event )    return;
        client->handleEvent(event);

		event = UVA_NEW(UVAEvent, (UVAEvent::BufferingStreamBuffered));
		if ( !event )        return;
		client->handleEvent(event);
        /////////////////////////////////////////////////
	}
		break;
	case MB_PLAYEVENT_FINISH:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_FINISH\n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::PlaybackFinished));
		if ( !event ) return;
		client->handleEvent(event);

		event = UVA_NEW(UVAEvent, (UVAEvent::BufferingProgress));
		if ( !event ) return;
		client->handleEvent(event);
	}
		break;
	case MB_PLAYEVENT_ERROR:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_ERROR\n");
		this->setMediaState(HMP_MEDIA_STATE_ERROR);
		if( player->getErrorType() == MP_ERROR_SERVER_FAIL )
		{
			LOG_INFO("UVAEvent::BufferingNetworkError \n");
			UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingNetworkError));
			if ( !event ) return;
			client->handleEvent(event);
		}
		else
		{
			LOG_INFO("UVAEvent::DecodeError \n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::DecodeError));
		if ( !event ) return;
		client->handleEvent(event);
	}
	}
		break;
	case MB_PLAYEVENT_UNDERRUN:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_UNDERRUN\n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingStreamUnderrun));
		if ( !event ) return;
		client->handleEvent(event);
	}
		break;
	case MB_PLAYEVENT_MSE_UNDERRUN:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_MSE_UNDERRUN\n");
		MseMediaPlayer *mse_mp = dynamic_cast<MseMediaPlayer *>(player);
		if ( !mse_mp ) return;
		MseMediaSource *as = dynamic_cast<MseMediaSource *>(mse_mp->getActiveAudioSource());
		if ( as )
		{
			UVAEvent *event = UVA_NEW(BufferingStreamUnderrunEvent, (as->getID()));
			if ( !event ) return;
			client->handleEvent(event);
			LOG_INFO("EVENT: MB_PLAYEVENT_MSE_UNDERRUN for AUDIO [id:%d] \n", as->getID());

		}

		MseMediaSource *vs = dynamic_cast<MseMediaSource *>(mse_mp->getActiveVideoSource());
		if ( vs )
		{
			UVAEvent *event = UVA_NEW(BufferingStreamUnderrunEvent, (vs->getID()));
			if ( !event ) return;
			client->handleEvent(event);
			LOG_INFO("EVENT: MB_PLAYEVENT_MSE_UNDERRUN for VIDEO [id:%d] \n", vs->getID());
		}
	}

		break;
	case MB_PLAYEVENT_BUFFERED:
	case MB_PLAYEVENT_MSE_BUFFERED:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_BUFFERED\n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingStreamBuffered));
		if ( !event ) return;
		client->handleEvent(event);
	}
		break;
	case MB_PLAYEVENT_POSITION_CHANGED:
	{
//		LOG_INFO("EVENT: MB_PLAYEVENT_POSITION_CHANGED\n");

		if (!this->m_seeks.empty())
		{
			this->m_seeks.clear();
			UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::SeekCompleted));
			if ( !event ) return;
			client->handleEvent(event);
		}
		/*
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingProgress));
		if ( !event ) return;
		client->handleEvent(event);
		*/
	}
		break;
	case MB_PLAYEVENT_NEEDMOREDATA:
	{
		//LOG_INFO("EVENT: MB_PLAYEVENT_NEEDMOREDATA(src_id=%lld)\n", param);
		// DRMKEY_SYNC work around
		/////////////////////////////////////////////////
		if ( OplVodHtml5Handle::m_need_key == true || this->getIsFirst() )
		{
			this->setIsFirst(false);
			LOG_INFO("EVENT: MB_PLAYEVENT_NEEDMOREDATA. but ignore it because DRM key isn't set yet.\n");
			return;
		}
		/////////////////////////////////////////////////

		MseMediaPlayer *mse_mp = dynamic_cast<MseMediaPlayer *>(player);
		if ( !mse_mp ) return;
		if ( mse_mp->isSourceLocking(param) )
		{
			LOG_INFO("EVENT: MB_PLAYEVENT_NEEDMOREDATA.: current source is locked.(IGNORE)[%lld]\n", param);
			return;
		}

		UVAEvent *event = UVA_NEW(NeedMSEDataEvent, (param, ONETIME_SAMPLE_COUNT));
		if ( !event ) return;
		client->handleEvent(event);

		mse_mp->lockSource(param);
	}
		break;
	case MB_PLAYEVENT_SEEK_READY:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_SEEK_READY\n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::SeekCompleted));
		if ( !event ) return;
		client->handleEvent(event);
		MseMediaPlayer *mse_mp = dynamic_cast<MseMediaPlayer *>(player);
		if ( mse_mp == NULL || !mse_mp->resume() ) return;
		this->setMediaState(HMP_MEDIA_STATE_PLAY);
	}
		break;
	case MB_PLAYEVENT_SEEK_COMPLETE:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_SEEK_COMPLETE\n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::SeekCompleted));
		if ( !event ) return;
		client->handleEvent(event);
		this->setMediaState(HMP_MEDIA_STATE_PLAY);
	}
		break;
	case MB_PLAYEVENT_FLUSH_COMPLETED:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_FLUSH_COMPLETED(pos=%lld)\n", param);
		double seek_pos = static_cast<double>(param)/1000.0;
		MseMediaPlayer *mse_mp = dynamic_cast<MseMediaPlayer *>(player);
		if ( !mse_mp ) return;

		MseMediaSource *as = dynamic_cast<MseMediaSource *>(mse_mp->getActiveAudioSource());
		if ( as )
		{
			LOG_INFO("setPosition: MB_STREAMING_MSE - AudioSource_id=[%d], pos=[%f]\n",
					as->getID(), seek_pos);
			UVAEvent *event = UVA_NEW(CancelDataRequestsEvent, (seek_pos, as->getID()));
			client->handleEvent(event);
		}
		MseMediaSource *vs = dynamic_cast<MseMediaSource *>(mse_mp->getActiveVideoSource());
		if ( vs )
		{
			LOG_INFO("setPosition: MB_STREAMING_MSE - VideoSource_id=[%d], pos=[%f]\n",
					vs->getID(), seek_pos);
			UVAEvent *event = UVA_NEW(CancelDataRequestsEvent, (seek_pos, vs->getID()));
			client->handleEvent(event);
		}
	}
		break;
	case MB_PLAYEVENT_BUFFER_PROGRESS:
	{
		//LOG_INFO("EVENT: MB_PLAYEVENT_BUFFER_PROGRESS\n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingProgress));
		if ( !event ) return;
		client->handleEvent(event);
	}
		break;
	case MB_PLAYEVENT_BUFFER_IDLE:
	{
		//LOG_INFO("EVENT: MB_PLAYEVENT_BUFFER_IDLE\n");
		UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::BufferingIdle));
		if ( !event ) return;
		client->handleEvent(event);
	}
		break;

	case MB_PLAYEVENT_VIDEOWINDOW_CHANGED:
	{
		int w = 0, h = 0;
		double pixel_aspect = 0;
		LOG_INFO("EVENT: MB_PLAYEVENT_VIDEOWINDOW_CHANGED\n");
		player->getWindowSize(&w, &h);
		// PAR = DAR / SAR
		pixel_aspect = (double)(OSD_WIDTH * h) / (double)(OSD_HEIGHT * w);
		LOG_INFO("EVENT: w %d h %d\n", w, h);
		UVAEvent* event = UVA_NEW(VideoResizedEvent, (w, h, pixel_aspect));
		if ( !event ) return;
		client->handleEvent(event);
	}
		break;
	case MB_PLAYEVENT_COMPONENT_CHANGED:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_COMPONENT_CHANGED\n");
		eMpPlayAVComponentType type = MP_COMPONENT_TYPE_MAX;
		int index = 0;
		player->getSelectedChangedTypeIndex(&type, &index);

		if( !m_avcomponents.empty() )
		{
			int count = 0;
			for( std::list<HAVComponent *>::iterator it = m_avcomponents.begin(); it!=m_avcomponents.end(); ++it )
			{
				HAVComponent *component = (*it);
				if( component )
				{
					LOG_INFO("CHANGED: component(%p), type(%d), id(%d), is_active(%d), index (%d)\n",
							component, component->type, component->source_id, component->isActive(), index);

					if( ((eMpPlayAVComponentType)component->type == type) && (index == count++) )
					{
						UVAEvent* event = UVA_NEW(ComponentChangedEvent, (component, ComponentChangedEvent::CHANGED));
						if ( !event ) return;
						client->handleEvent(event);
					}
				}
			}
		}
	}
		break;
	case MB_PLAYEVENT_DURATION_CHANGED:
	{
		LOG_INFO("EVENT: MB_PLAYEVENT_DURATION_CHANGED\n");
		unsigned dur = player->getDuration();
		UVAEvent *event;

		LOG_INFO("duration=[%d]\n", dur);

		if ( dur != 0 )
		{
			if( player->getDurationInfinity() )
				event = UVA_NEW(DurationChangedEvent, INF);
			else
				event = UVA_NEW(DurationChangedEvent, (dur));

			client->handleEvent(event);
		}
	}
		break;
	default:
		break;
	}
}

HmpMediaBackend *HmpMediaBackend::findActiveBackend()
{
	pthread_mutex_lock(&s_backend_mutex);
	if ( s_backends.empty() )
	{
		pthread_mutex_unlock(&s_backend_mutex);
		return NULL;
	}

	HmpMediaBackend *backend = s_backends.back();
	pthread_mutex_unlock(&s_backend_mutex);

	return backend;
}

#if ENABLE_TASK
void *HmpMediaBackend::threadFunc(void *arg)
{
	HmpMediaBackend *hmp = static_cast<HmpMediaBackend *>(arg);
	if ( !hmp ) return NULL;

	while ( !hmp->m_quit_task )
	{
		pthread_mutex_lock(&hmp->m_mutex);
		if ( !hmp->m_seeks.empty() && hmp->m_player->getName() == MSE_MP &&
				( hmp->getMediaState() == HMP_MEDIA_STATE_IDLE
				|| hmp->getMediaState() == HMP_MEDIA_STATE_CANPLAY
				|| hmp->getMediaState() == HMP_MEDIA_STATE_PLAY )
			)
		{
			double pos = hmp->m_seeks.back();
			hmp->m_seeks.pop_back();
			hmp->setMediaState(HMP_MEDIA_STATE_SEEKING);
			if(hmp->__seek(hmp, pos)) hmp->setLastSetPosition(pos);
		}
		pthread_mutex_unlock(&hmp->m_mutex);

		usleep(100000);
	}

	LOG_INFO("Destroying Command Thread......\n");

	return NULL;
}
#endif

UVABackend::AVComponent::Kind HmpMediaBackend::__change_component_role(char *in)
{
	char *role = NULL;

	if( !in ) return UVABackend::AVComponent::NONE;

	role = strstr(in, "alternate");
	if( NULL != role )
	{
		if( (strstr(in, "main") == NULL)
			&& (strstr(in, "commentary") == NULL)
			&& (strstr(in, "dub") == NULL) )
		{
			return UVABackend::AVComponent::ALTERNATIVE;
		}
		role = NULL;
	}

	role = strstr(in, "caption");
	if( NULL != role )
	{
		if( strstr(in, "main") != NULL )
		{
			return UVABackend::AVComponent::CAPTIONS;
		}
		role = NULL;
	}

	role = strstr(in, "description");
	if( NULL != role )
	{
		if( strstr(in, "supplementary") != NULL )
		{
			return UVABackend::AVComponent::DESCRIPTION;
		}
		role = NULL;
	}

	role = strstr(in, "main");
	if( NULL != role )
	{
		if( (strstr(in, "caption") == NULL)
			&& (strstr(in, "subtitle") == NULL)
			&& (strstr(in, "dub") == NULL) )
		{
			return UVABackend::AVComponent::MAIN;
		}
		if( strstr(in, "supplementary") != NULL )
		{
			return UVABackend::AVComponent::MAIN_DESC;
		}
		role = NULL;
	}

	role = strstr(in, "subtitle");
	if( NULL != role )
	{
		if( strstr(in, "main") != NULL )
		{
			return UVABackend::AVComponent::SUBTITLES;
		}
		role = NULL;
	}

	role = strstr(in, "dub");
	if( NULL != role )
	{
		if( strstr(in, "main") != NULL )
		{
			return UVABackend::AVComponent::TRANSLATION;
		}
		role = NULL;
	}

	role = strstr(in, "commentary");
	if( NULL != role )
	{
		if( strstr(in, "main") == NULL )
		{
			return UVABackend::AVComponent::COMMENTARY;
		}
		role = NULL;
	}

	return UVABackend::AVComponent::NONE;
}

