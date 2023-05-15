/** **********************************************************************************************************
	@file 		mse_backend.cpp
	
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

#include "mse_backend.h"

#include <uva_client.h>
#include <uva_event.h>

#include "oplvideoutil.h"

#include <iostream>
#include <cstring>

using namespace std;

#define ENTRY	cout << __FILE__ << "::" << __FUNCTION__ << ":: " << "++:" << m_uva_client << endl
#define EXIT	cout << __FILE__ << "::" << __FUNCTION__ << ":: " << "--:" << m_uva_client << endl

static const char* s_supported_types[]  = {
	"audio/mpeg", "audio/mp2", "audio/mp3", "audio/mp4", "video/mpeg", "video/mp4", "video/h264", "video/H264",
	"video/mpeg4", "video/avi", "video/x-msvideo", "video/x-ms-asf", "video/x-ms-wmv", "audio/x-ms-wma", NULL
};


/***************************************************************************************************
 * MseMediaBackend
 ***************************************************************************************************/
MseMediaBackend *MseMediaBackend::s_theApp = NULL;

MseMediaBackend::MseMediaBackend(UVABackendType type, UVAClient *client)
	: UVAMediaBackend(type),
	m_uva_type(type),
	m_uva_client(client)
{
	ENTRY;

	s_theApp = this;

	EXIT;
}

MseMediaBackend::~MseMediaBackend()
{
	ENTRY;

	s_theApp = NULL;

	EXIT;
}

UVA_STATUS MseMediaBackend::initBackend(const char *origin, SourceDescription **sources,
			bool automatic_key_system_selection)
{
	ENTRY;

	for ( int i=0; sources[i]!=NULL; ++i )
	{
		cout << "Sources [" << i << "]" << endl;

		if ( sources[i]->type != SourceDescription::BACKEND_STREAMING ) continue;

		cout << "Stream Type: " << sources[i]->type << endl;
		cout << "backend url: " << sources[i]->description.backend_streaming.url << endl;
		break;
	}

	EXIT;
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::attachSource(SourceDescription *source)
{
	ENTRY;

	cout << "Stream Type: " << source->type << endl;
	cout << "content_size: " << source->description.opera_streaming.content_size << endl;
	cout << "seekable: " << source->description.opera_streaming.seekable << endl;
	cout << "source id: " << source->description.opera_streaming.source_id << endl;

	UVAEvent *event = UVA_NEW(DurationChangedEvent, (180));
	if ( !event ) return UVA_NOT_SUPPORTED;
	m_uva_client->handleEvent(event);

	event = UVA_NEW(NeedMSEDataEvent, (source->description.opera_streaming.source_id, 10));
	m_uva_client->handleEvent(event);

	char path[1024] = {0,};
	sprintf(path, "%s%d", "./msevideo_", source->description.opera_streaming.source_id);
	m_videodump.open(path);
	sprintf(path, "%s%d", "./mseaudio_", source->description.opera_streaming.source_id);
	m_audiodump.open(path);

	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::removeSource(uintptr_t source_id)
{
	ENTRY;

	m_videodump.close();
	m_audiodump.close();

	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::setVideoWindow(int x, int y, int w, int h)
{
	ENTRY;

	cout << "x=" << x << " , y=" << y << " w=" << w << " h=" << h << endl;
	
	EXIT;
	
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::setClipRect(int x, int y, int w, int h)
{
	ENTRY;

	cout << "x=" << x << " , y=" << y << " w=" << w << " h=" << h << endl;

	EXIT;
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::getNativeSize(int &w, int &h, double &pixel_aspect)
{
	ENTRY;

	w = OSD_WIDTH;
	h = OSD_HEIGTH;
	pixel_aspect = 0.0;

	EXIT;
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::getSeekableTimeRanges(ByteRange **input_byte_ranges,
			TimeRange **&output_time_ranges)
{
	ENTRY;
	EXIT;
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::getBufferedTimeRanges(ByteRange **input_byte_ranges,
			TimeRange **&output_time_ranges)
{
	ENTRY;
	EXIT;
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::canPlayURL(const char *url)
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

UVA_STATUS MseMediaBackend::play()
{
	ENTRY;
	EXIT;
	
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::stop()
{
	ENTRY;
	EXIT;

	return UVA_OK;
}

UVA_STATUS MseMediaBackend::pause(bool get_resources)
{
	ENTRY;
	EXIT;
	
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::setPlaybackRate(double rate)
{
	ENTRY;
	EXIT;
		
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::setVolume(double volume)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::getDuration(double &duration)
{
	ENTRY;
	EXIT;

	return UVA_OK;
}

UVA_STATUS MseMediaBackend::getPosition(double &position)
{
	ENTRY;
	EXIT;

	return UVA_OK;
}

UVA_STATUS MseMediaBackend::setPosition(double position)
{
	ENTRY;

	cout << "pos: " << position << endl;

	UVAEvent* event = UVA_NEW(UVAEvent, (UVAEvent::SeekCompleted));
	if ( !event ) return UVA_ERROR;
	m_uva_client->handleEvent(event);
	
	EXIT;
		
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::setVisibility(bool visible, bool unloaded)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::getSizeFromDuration(double duration, size_t &size)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::writeData(UVAEvent* need_data_event, size_t bytes_written,
			WriteDataStatus status)
{
	if ( need_data_event->type == UVAEvent::NeedMSEData )
	{
		NeedMSEDataEvent* evt = static_cast<NeedMSEDataEvent*>(need_data_event);
		int cnt = 0;
		for ( cnt=0; evt->samples[cnt]; cnt++ )
		{
			UVADecoderBuffer* b = evt->samples[cnt];
			if ( UVAStreamConfig::av(b->config.codec) == UVAStreamConfig::AUDIO_STREAM )
			{
				printf("Dumping audio sample %d (size %d) PTS:%.3f DTS:%.3f\n",
						cnt, b->GetDataSize(), b->pts_sec, b->dts_sec);
				//m_audiodump.dump(b->GetData(), b->GetDataSize());
			}
			else if ( UVAStreamConfig::av(b->config.codec) == UVAStreamConfig::VIDEO_STREAM )
			{
				printf("Dumping video sample %d (size %d) PTS:%.3f DTS:%.3f\n",
						cnt, b->GetDataSize(), b->pts_sec, b->dts_sec);
				//m_videodump.dump(b->GetData(), b->GetDataSize());
			}

			printf("Extra data size = %d\n",  b->config.extra_data_size);

			if ( b->config.extra_data_size > 0 )
			{
				switch( b->config.codec )
				{
					case UVAStreamConfig::H264:
						printf("VideoData: H264\n");
						break;
					case UVAStreamConfig::AAC:
						printf("AudioData: AAC\n");
						break;
					default:
						printf("what codec???: %d\n", b->config.codec);
						break;
				}
			}
		}

		// For test: needData event generation
		switch ( status )
		{
		case WRITE_DATA_OK:
		{
			UVAEvent *event = UVA_NEW(NeedMSEDataEvent, (evt->source_id, 10));
			m_uva_client->handleEvent(event);
		}
			break;
		case WRITE_DATA_EOS:
			printf("WRITE_DATA_EOS\n");
			break;
		case WRITE_DATA_ERROR:
			printf("WRITE_DATA_ERROR\n");
			break;
		}

		UVA_DELETE(evt);
		return UVA_OK;
	}

	return UVA_ERROR;
}

UVA_STATUS MseMediaBackend::canPlayType(const char *type, const char **codecs, const char *drmID)
{
	ENTRY;

	if ( !type ) return UVA_OK;

	cout << "type = " << type << endl;
	for ( int i = 0; s_supported_types[i] != NULL; i++ )
	{
		const char* _type = s_supported_types[i];
		if ( strcmp(type, _type) == 0 )
		{
			for ( int j = 0; codecs[j] != NULL; j++ )
			{
				cout << "[" << j << "] " << codecs[j] << endl;
			}
			cout << "OK" << endl;
			EXIT;
			return UVA_OK;
		}
	}

	cout << "FAIL" << endl;
	EXIT;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::setFullScreen(bool fullscreen)
{
	ENTRY;
	EXIT;
	
	return UVA_OK;
}

UVA_STATUS MseMediaBackend::getSupportedMimetypes(char **&mime_types)
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

UVA_STATUS MseMediaBackend::getComponents(AVComponent **&components)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::unselectComponent(AVComponent *component)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

UVA_STATUS MseMediaBackend::selectComponent(AVComponent *component)
{
	ENTRY;
	return UVA_NOT_SUPPORTED;
}

#if 0
UVA_STATUS MseMediaBackend::setContentSize(uintptr_t source_id, size_t content_size)
{
	ENTRY;
	cout << "content_size: " << content_size << endl;
	return UVA_OK;
}
#endif
