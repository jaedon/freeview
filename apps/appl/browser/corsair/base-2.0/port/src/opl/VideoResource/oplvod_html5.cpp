/*************************************************************************************************************
	File 		: oplvod_html5.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/19
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Â¡Â°HumaxÂ¡Â±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED Â¡Â°AS ISÂ¡Â± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*************************************************************************************************************/

#include "oplvod_html5.h"

#include "hapi.h"
#include "apk.h"

#include <cstdio>
#include <time.h>
#include <_hlib_transport.h>

#if defined(CONFIG_PROD_FVP4000T)
#include <libxml/tree.h>
#endif


#define OPLVOD_DOWNLOAD_URI_FORM    "hbuffer://cso_session/%d"

using namespace std;

#define ENTRY	HxLOG_Debug("[OplVodHtmlFive][%s:%d]++\n", __FUNCTION__, __LINE__);
#define EXIT	HxLOG_Debug("[OplVodHtmlFive][%s:%d]--\n", __FUNCTION__, __LINE__);

#define	IsHardOfHearingSubtitle(type)			(((type) == 0x05) || (0x20 <= (type) && (type) <= 0x24))

static void oplvod_download_proc(HUINT32 ulDownloadId, DxDownload_Type_e eType, DxDownload_Event_e eEvent, DxDownload_NotifierData_t *punNotifyData, void *pvUserData);

static const char *__convertAudioCodecToOplEncoding (DxAudioCodec_e eCodec)
{
	switch (eCodec)
	{
	case eDxAUDIO_CODEC_MPEG:
		return "MPEG1_L2";

	case eDxAUDIO_CODEC_MP3:
		return "MPEG1_L3";

	case eDxAUDIO_CODEC_DOLBY_AC3:
		return "AC3";

	case eDxAUDIO_CODEC_DOLBY_AC3P:
		return "E-AC3";

	case eDxAUDIO_CODEC_PCM:
		return "WAV";

	case eDxAUDIO_CODEC_AAC:
	case eDxAUDIO_CODEC_AAC_LOAS:
	case eDxAUDIO_CODEC_AAC_PLUS:
	case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
		return "HEAAC";

	case eDxAUDIO_CODEC_DTS:
	case eDxAUDIO_CODEC_DTS_HD:
		return "DTS";

	default: break;
	}

	return "UNKNOWN";
}

static eOplAVComponentSubType __convert_audio_to_subtype(OxDownload_AudioMode_e type)
{
	switch ( type )
	{
	case eOxDNLD_AUDMODE_SINGLEMONO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO;

	case eOxDNLD_AUDMODE_DUALMONO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;

	case eOxDNLD_AUDMODE_STEREO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;

	case eOxDNLD_AUDMODE_MULTILINGUAL:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL;

	case eOxDNLD_AUDMODE_SURROUND:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_SURROUND;

	case eOxDNLD_AUDMODE_DOLBY:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;

	default:
		break;
	}

	return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
}


/****************************************************************************************
 * OplVodHtmlFive
 ***************************************************************************************/
OplVodHtmlFive::OplVodHtmlFive(int aVideoResouceId)
	: OplVod(aVideoResouceId), 
	m_user_data(NULL), m_last_pos(0), m_duration(0), m_download_id(0), m_is_regist_callback(false)
{
	HERROR	hErr;
	ENTRY;

	if( !m_is_regist_callback )
	{
		hErr = APK_DOWNLOAD_RegisterNotifier (oplvod_download_proc, NULL);
		if( ERR_OK != hErr )
			HxLOG_Error("[%s:%d] APK_DOWNLOAD_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);
		m_is_regist_callback = true;
	}

	memset(&m_probe_info, 0x0, sizeof(m_probe_info));
	setDownloadState(OPLVOD_DOWNLOAD_STATE_CREATE);	
	checkVodsStatus();
}

OplVodHtmlFive::~OplVodHtmlFive()
{
	HERROR	hErr;
	ENTRY;

	if( m_is_regist_callback )
	{
		if( s_vods.size() <= 1 )
		{
		hErr = APK_DOWNLOAD_UnregisterNotifier (oplvod_download_proc, NULL);
		if( ERR_OK != hErr )
			HxLOG_Error("[%s:%d] APK_DOWNLOAD_UnregisterNotifier Error: \n", __FUNCTION__, __LINE__);
		m_is_regist_callback = false;
	}
}
}

bool OplVodHtmlFive::setProbeInfo(OplVodProbeInfo *probe_info)
{
	bool ret = false;

	m_probe_info.audio_pid = probe_info->audio_pid;
	m_probe_info.video_pid = probe_info->video_pid;
	m_probe_info.audio_codec = probe_info->audio_codec;
	m_probe_info.video_codec = probe_info->video_codec;

	return ret;
}

void OplVodHtmlFive::checkVodsStatus()
{
	HERROR eRet = ERR_OK;
	OplVod *vod = OplVod::getActiveVod();
	if(vod != this && vod != NULL)
	{
		eOplVodPlayState state = vod->getPlayState();
		if( (state == OPLVOD_PLAY_STATE_PAUSED)
			|| (state == OPLVOD_PLAY_STATE_FINISHED))
		{
			OplVodHtmlFive *html5_vod = static_cast <OplVodHtmlFive *> (vod);
			eRet = APK_MEDIA_PLAY_Stop( html5_vod->m_dest);
			html5_vod->m_is_active = false;
			html5_vod->m_play_action = VOD_ACTION_PROBE;
		}
	}
}

#if defined(CONFIG_PROD_FVP4000T)
void *OplVodHtmlFive::__find_xml_node( void *node, const char *name)
{
	xmlNodePtr _node = (xmlNodePtr)node;

	while( _node != NULL )
	{
		if ( strcmp((char *)(_node->name), "text") == 0 )
		{
			_node = _node->next;
			continue;
		}

		if( strcmp((char *)(_node->name), name) == 0 ) 
			return (void *)_node;

		if( _node->next ) _node = _node->next;
	}

	return NULL;
}

bool OplVodHtmlFive::ContentAccessStreamingInfoParse(std::string url)
{
	HxTRANSPORT_Http_t *pHttp = NULL;
	HFLOAT64 			ullDownloadSize = 0;
	HINT8 				*checkBuf     = NULL;
	HERROR 				err = ERR_OK;
	int 				ulHttpCode  = 0;
	bool 				ret = false;

	pHttp = HLIB_TRANSPORT_HttpOpen();
	if( NULL == pHttp ) return false;
	
	err = HLIB_TRANSPORT_HttpSetTimeout(pHttp, 3);
	if ( err != ERR_OK )
	{
		HLIB_TRANSPORT_HttpClose(pHttp);
	   	return false;
	}

	err = HLIB_TRANSPORT_HttpGet(pHttp, (HINT8*)url.c_str(), &checkBuf, &ullDownloadSize);
	if ( err != ERR_OK )
	{
		HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
		HLIB_TRANSPORT_HttpClose(pHttp);
		return false;
	}

	err = HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, (HxTRANSPORT_HttpResponse_e *)&ulHttpCode);
	if ( err != ERR_OK )
	{
		HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
		HLIB_TRANSPORT_HttpClose(pHttp);
	   	return false;
	}

	if( ulHttpCode != eHxTP_HTTP_RESCODE_OK ) 
	{
		HxLOG_Debug("OplVodHtmlFive::ContentAccessStreamingInfoParse - XML files is not received. Http Error (%d) \n", ulHttpCode);
		HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
		HLIB_TRANSPORT_HttpClose(pHttp);
		return false;
	}

	if( checkBuf != NULL )
	{
		xmlDocPtr 	doc = xmlParseMemory((const char *)checkBuf, 1024 * 10);
		xmlNodePtr	root = xmlDocGetRootElement(doc);
		xmlNodePtr	node = root->children;

		if(node)
		{
			node = (xmlNodePtr)__find_xml_node(node, "ContentItem");
			if(node)	
			{
				node = node->children;
				if(node)
				{
					node = (xmlNodePtr)__find_xml_node(node, "ContentURL");
					if(node){
						char *contentUrl = (char *) xmlNodeGetContent(node);    
						const char *is_url = strstr(contentUrl, "://");
						if(is_url == NULL)
						{
							string url = OplVod::m_application_url;
							std::size_t pos = url.find_last_of("/");
							if( pos )
							{
								url.erase(pos+1);
								url.append(contentUrl);
							}    

							m_url = url;
							HxLOG_Debug("OplVodHtmlFive::ContentAccessStreamingInfoParse - ContentURL(%s) \n", m_url.c_str());
							ret = true;
						}
					}
				}

			}
		}

		xmlFreeDoc(doc);
	}

    HLIB_TRANSPORT_HttpFreeBuffer(checkBuf);
    HLIB_TRANSPORT_HttpClose(pHttp);

	return ret;
}
#endif

bool OplVodHtmlFive::probe()
{
	HERROR eRet = ERR_OK;

	ENTRY;

	HxLOG_Debug("OplVodHtmlFive::probe: play_state=%d, play_action=%d, url=%s, mime type=%s\n", \
			m_play_state, m_play_action, m_url.c_str(), m_mime_type.c_str());

	if ( m_is_pvr
		|| m_url.c_str() == NULL
		|| m_play_action == VOD_ACTION_PROBE
		|| m_play_action == VOD_ACTION_PLAY )
	{
		return true;
	}

	__apply_scenario();

	if( strcmp((char *)m_mime_type.c_str(), "application/vnd.oipf.ContentAccessStreaming+xml") == 0 )
	{
#if defined(CONFIG_PROD_FVP4000T)
		if ( ContentAccessStreamingInfoParse(m_url) == false )
			return false;
#endif
	}

	this->m_is_download_vod = true;
	m_download_id = startDownload();

	HxLOG_Debug("OplVodHtmlFive::probe:download id(%d), url=%s\n", m_download_id, m_url.c_str());

	setDownloadState(OPLVOD_DOWNLOAD_STATE_STARTED);

	m_play_action = VOD_ACTION_PROBE;

	EXIT;

	return eRet == ERR_OK;
}

bool OplVodHtmlFive::play(OplVodProbeInfo *probe_info)
{
	HERROR eRet = ERR_OK;
	ENTRY;

	OplVod *vod = OplVod::getActiveVod();

	if(vod != this && vod != NULL){
		HxLOG_Debug("**************************************************************************************************\n");
		HxLOG_Debug("*** Media Change (%s -> %s)\n", vod->getUrl(), getUrl());
		HxLOG_Debug("**************************************************************************************************\n");

		OplVodHtmlFive *html5_vod = static_cast <OplVodHtmlFive *> (vod);

		eRet = APK_MEDIA_PLAY_Stop( m_dest /*ulViewId*/);
		if( eRet != ERR_OK )
			HxLOG_Error("OplVodHtmlFive::play - APK_MEDIA_PLAY_Stop Error \n");

		html5_vod->m_is_active = false;
		html5_vod->m_play_action = VOD_ACTION_PROBE;
	}
/*
	HxLOG_Debug("OplVodHtmlFive::play: play_state=%d, play_action=%d, download_state=%s, last_pos(%d), duration(%d), url(%s)\n", 
			m_play_state, m_play_action, getDownloadStateString(getDownloadState()), m_last_pos, m_duration, getDownloadUrl());
*/
	if ( m_play_action == VOD_ACTION_PLAY ) return true;

	m_is_active = true;

	if ( getDownloadState() != OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED
		&& getDownloadState() != OPLVOD_DOWNLOAD_STATE_CREATE )
	{
		return true;
	}

	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_StartInfo_t 	 unStart;
	HERROR						 hErr;

	memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));

	ulViewId = m_dest;
	unStart.stMediaPlay.eType			= eOxMP_MEDIATYPE_MEDIAPLAY;
#if defined(CONFIG_TIMESTAMP_64)
	unStart.stMediaPlay.ullStartTime 	= m_last_pos;
#else
	unStart.stMediaPlay.ulStartTime 	= m_last_pos;
#endif
	unStart.stMediaPlay.ulAudioPid 		= probe_info->audio_pid;
	unStart.stMediaPlay.ulVideoPid 		= probe_info->video_pid;
	unStart.stMediaPlay.eAudioCodec 	= static_cast<DxAudioCodec_e>(__audio_codec_to_appkit(probe_info->audio_codec));
	unStart.stMediaPlay.eVideoCodec 	= static_cast<DxVideoCodec_e>(__video_codec_to_appkit(probe_info->video_codec));

	if( getDownloadState() != OPLVOD_DOWNLOAD_STATE_CREATE )
		APK_MEDIA_PLAY_Probe( m_dest /*ulViewId*/, (HCHAR *)getDownloadUrl());

	hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);
    m_last_pos = 0;
	OplVodHtmlFive::debugVodList();

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%dn", __FUNCTION__, __LINE__, ulViewId);
		eRet = hErr;
	}

	m_session_id = ulSessionId;

	m_play_action = VOD_ACTION_PLAY;

	return eRet == ERR_OK;
}

bool OplVodHtmlFive::stop(bool internal)
{
	HERROR eRet = ERR_OK;
	OxMediaPlay_MediaType_e eOapiType;

	ENTRY;

	if ( m_play_action == VOD_ACTION_STOP
		|| m_play_action == VOD_ACTION_INTERNAL_STOP
		|| ( m_play_state == OPLVOD_PLAY_STATE_STOPPED && internal == TRUE ))
	{
		HxLOG_Debug("OplVod::stop: the playback is already stopped.\n");
		m_play_action = (internal) ? VOD_ACTION_INTERNAL_STOP : VOD_ACTION_STOP;
		return true;
	}

	APK_MEDIA_PLAY_GetPlayType( m_dest , &eOapiType );
	if ( eOxMP_MEDIATYPE_LIVE != eOapiType )
	{
		list<OplVod*>::iterator begin_it = s_vods.begin();
		for ( list<OplVod*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
		{
			if(((*it)->m_is_active == true) && (*it) == this)
				eRet = APK_MEDIA_PLAY_Stop( m_dest /*ulViewId*/);
			}
		}	

	APK_DOWNLOAD_Stop(m_download_id);

	if ( !internal )
	{
#if 0
		/* workaround for Obigo Applications: invalid handling: stopped event */
		if ( OplVodExtApp::stayMusicBaebleApp() || OplVodExtApp::stayItalianFoodApp()
				|| OplVodExtApp::stayAbsoluteRadioApp() )
		{
			HxLOG_Debug("OplVod::stop: Music Bable bug fix (workaround).\n");
			setPlayState( OPLVOD_PLAY_STATE_STOPPED );
		}
#endif

		m_play_action = VOD_ACTION_STOP;
	}
	else
	{
		m_play_action = VOD_ACTION_INTERNAL_STOP;
		HxLOG_Debug("OplVod::stop: internal stop is called.\n");
	}

	touch();

	EXIT;

	return eRet == ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
bool OplVodHtmlFive::seek(unsigned long long position)
{
	HERROR eRet = ERR_OK;

    HxLOG_Debug("OplVodHtmlFive::seek:m_play_action:%d, position:%llu\n",  m_play_action, position);
    if ( m_play_action == VOD_ACTION_PROBE )
    {
        m_last_pos = position;
        return true;
    }
	
	if ( m_is_pvr )
		position = position / 1000;

    HxLOG_Debug("OplVodHtmlFive::seek: playback position will be changed to %llu.\n", position);
	eRet = APK_MEDIA_PLAY_SetPlayPosition(m_dest, position);

	return eRet == ERR_OK;
}
#else
bool OplVodHtmlFive::seek(unsigned int position)
{
	HERROR eRet = ERR_OK;

    HxLOG_Debug("OplVodHtmlFive::seek:m_play_action:%d, position:%llu\n",  m_play_action, position);
    if ( m_play_action == VOD_ACTION_PROBE )
    {
        m_last_pos = position;
        return true;
    }
	
	if ( m_is_pvr )
		position = position / 1000;

    HxLOG_Debug("OplVodHtmlFive::seek: playback position will be changed to %llu.\n", position);
	eRet = APK_MEDIA_PLAY_SetPlayPosition(m_dest, position);

	return eRet == ERR_OK;
}
#endif

bool OplVodHtmlFive::setPlaySpeed(int speed_sec)
{
	HERROR eRet = ERR_OK;

	ENTRY;

   // play()-> rate change immediatly , not chage rate issue
//		if ( !isPlaying() )
//		{
//			HxLOG_Debug("OplVodHtmlFive::setPlaySpeed: the playback is not started yet.\n");
//			return true;
//		}

	eRet = APK_MEDIA_PLAY_SetPlaySpeed( m_dest /*ulViweId*/, speed_sec);

	if ( speed_sec == 0 ) /*  pause */
	{
		APK_MEDIA_PLAY_GetPlayPosition (m_dest, &m_last_pos );
		// (kskim2-150722) BBC Sport ÀÜ»ó ÀÌ½´·Î ÀÎÇÑ workaround 
		m_play_state = OPLVOD_PLAY_STATE_PAUSED;
	}

	EXIT;

	return eRet == ERR_OK;
}


void OplVodHtmlFive::setPlayState(eOplVodPlayState state)
{
	ENTRY;
	m_play_state = state;

	if( getActiveVod() == NULL ) return;

	switch ( m_play_state )
	{
	case OPLVOD_PLAY_STATE_ERROR:
		if ( getActiveVod()->getPlayError() != OPLVOD_PLAY_ERROR_NOT_AVAILABLE_POSITION )
		{
			m_url = "";
		}
		break;
	default:
		break;
	}
}

unsigned int OplVodHtmlFive::getBufferingInfo(unsigned  int **start_times, unsigned int **end_times)
{
	HERROR eRet = ERR_FAIL;
	
	if ( m_is_pvr ) return 0;

	unsigned int chunk_count = 0;
	if( m_is_active )
	{
		eRet = APK_MEDIA_PLAY_GetBufferingChunkInfo( m_dest, &chunk_count, start_times, end_times);
	}
	else
	{
		eRet = APK_DOWNLOAD_GetBufferingChunkInfo( m_download_id, &chunk_count, start_times, end_times);	
	}
	
	if ( eRet != ERR_OK )
	{
		return 0;
	}

	return chunk_count;
}

unsigned int OplVodHtmlFive::getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end)
{
	if ( m_is_pvr ) return 0;

	unsigned int chunk_count = 0;

#if 1
	HERROR eRet = APK_DOWNLOAD_GetSeekableRangeInfo(m_download_id, &chunk_count, seekable_start, seekable_end);	

	if ( eRet != ERR_OK )
	{
		return 0;
	}
#endif

	return chunk_count;
}

void *OplVodHtmlFive::getUserData()
{
	ENTRY;
	return m_user_data;
}

void OplVodHtmlFive::setUserData(void *user_data)
{
	ENTRY;
	if ( user_data ) m_user_data = user_data;
}

#if defined(CONFIG_TIMESTAMP_64)
unsigned long long OplVodHtmlFive::getDuration()
{	
	ENTRY;
	
	if( (m_play_state == OPLVOD_PLAY_STATE_PLAYING)
		|| (m_play_state == OPLVOD_PLAY_STATE_ERROR))
	{
		unsigned long long duration = 0;

		HERROR eRet = APK_MEDIA_PLAY_GetDurationTime( m_dest, &duration);
		if ( eRet != ERR_OK ) return m_duration;

		return ( m_is_pvr ) ? duration*1000 : duration;
	}

	return m_duration;
}
#else
unsigned int OplVodHtmlFive::getDuration()
{	
	ENTRY;
	
	if( (m_play_state == OPLVOD_PLAY_STATE_PLAYING)
		|| (m_play_state == OPLVOD_PLAY_STATE_ERROR))
	{
		unsigned int duration = 0;

		HERROR eRet = APK_MEDIA_PLAY_GetDurationTime( m_dest, &duration);
		if ( eRet != ERR_OK ) return m_duration;

		return ( m_is_pvr ) ? duration*1000 : duration;
	}

	return m_duration;
}
#endif

void OplVodHtmlFive::setDuration(unsigned int duration)
{
	ENTRY;
	APK_MEDIA_PLAY_SetDuration(m_dest, duration);
}

int OplVodHtmlFive::getPumpIndex()
{
	ENTRY;
	unsigned int index = 0;
	HERROR hErr = APK_MEDIA_PLAY_GetPumpIndex(m_dest, &index);
	if ( hErr != ERR_OK ) return -1;

	return index;
}

bool OplVodHtmlFive::getResourceMap(OplVodResourceMap *resource_map)
{
	if ( resource_map )
	{
		OxMediaPlay_ResourceMap_t map;
		HERROR hErr = APK_MEDIA_PLAY_GetResourceMap(&map);
		if ( hErr != ERR_OK ) return false;

		resource_map->num_of_playpump = map.ulNumOfPlaypump;
		resource_map->playpump_list = map.pulPlaypumpList;
		resource_map->num_of_dma = map.ulNumOfDma;
		resource_map->dma_list = map.pulDmaList;
	}
	else
	{
		return APK_MEDIA_PLAY_GetResourceMap(NULL) == ERR_OK;
	}

	return true;
}

unsigned int OplVodHtmlFive::getComponentsCount(eOplAVComponentType comp_type)
{
	unsigned int count = 0;
	OxDownload_Component_e download_comp_type = (OxDownload_Component_e)__convertComponentType(comp_type);

	APK_DOWNLOAD_GetComponentNum(m_download_id,download_comp_type,&count);

	return count;
}

bool OplVodHtmlFive::getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp)
{
	HERROR hErr = ERR_FAIL;
	OxDownload_Component_e download_comp_type = (OxDownload_Component_e)__convertComponentType(comp_type);
	OxDownload_Component_t stComponet;

	hErr = APK_DOWNLOAD_GetComponent (m_download_id, download_comp_type, index, &stComponet);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] APK_DOWNLOAD_GetComponent Error : \n", __FUNCTION__, __LINE__);
	}

	HxSTD_MemSet (out_comp, 0, sizeof(OplAVComponent_t));
	out_comp->eType = comp_type;
	
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		{
			out_comp->ulPid				=	stComponet.stVideo.nPid;
			out_comp->ulComponentTag 	=	stComponet.stVideo.nComponentTag;
			out_comp->ulStreamType		= 	stComponet.stVideo.nStreamType;
			out_comp->ulComponentType	=   (unsigned int)stComponet.stVideo.nComponentType;
			HLIB_STD_StrNCpySafe(out_comp->szText, reinterpret_cast<char *>(stComponet.stVideo.aucStreamName), OPLAVCOMPONENT_TEXT_SIZE);
		}
		break;
	case OPLAVCOMPONENT_TYPE_AUDIO:
		{
			out_comp->ulPid				=	stComponet.stAudio.nPid;
			out_comp->ulComponentTag 	=	stComponet.stAudio.nComponentTag;
			const char *encoding		= __convertAudioCodecToOplEncoding(stComponet.stAudio.eCodec);
			HLIB_STD_StrNCpySafe (out_comp->encoding, (HCHAR*)encoding, HxSTD_StrLen(encoding) + 1);

			out_comp->eSubType		= __convert_audio_to_subtype(stComponet.stAudio.eMode);
			HLIB_STD_StrNCpySafe(out_comp->szLanguage, reinterpret_cast<char *>(stComponet.stAudio.astLangInfo[0].szLangCode), 4);

			HLIB_STD_StrNCpySafe(out_comp->compName, reinterpret_cast<char *>(stComponet.stAudio.aucCompName), OPLAVCOMPONENT_COMP_NAME);
			HLIB_STD_StrNCpySafe(out_comp->compRole, reinterpret_cast<char *>(stComponet.stAudio.aucCompRole), OPLAVCOMPONENT_COMP_ROLE);

			out_comp->bAudioDescription =  OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[0].eAudType,stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
			out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[1].eAudType,stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
			out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[0].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
			out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[1].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);

			HxLOG_Debug("getComponent: nPid(%d), nComponentTag(%d), encoding(%s), eSubType(%d), aucCompName(%s), aucCompRole(%s), bAudioDescription(%d)\n", 
				stComponet.stAudio.nPid, stComponet.stAudio.nComponentTag, encoding, out_comp->eSubType, 
				stComponet.stAudio.aucCompName, stComponet.stAudio.aucCompRole, out_comp->bAudioDescription);
				
			
		}
		break;
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		{
			out_comp->ulPid	= (HUINT32)stComponet.stSubtitle.nPid;
			if( stComponet.stSubtitle.eSubtitleType == eOxDNLD_SUBTITLETYPE_Dvb )
			{
				out_comp->eSubType = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_DVB;
				out_comp->ulComponentTag = stComponet.stSubtitle.utArg.stDvbInfo.nComponentTag;
				HLIB_STD_StrNCpySafe(out_comp->szLanguage,
					reinterpret_cast<char *>(stComponet.stSubtitle.utArg.stDvbInfo.szLangCode), 4);

				out_comp->bHardOfHearing_Impaired	= IsHardOfHearingSubtitle((HUINT32)stComponet.stSubtitle.utArg.stDvbInfo.nTypeValue);
			}
			else if( stComponet.stSubtitle.eSubtitleType == eOxDNLD_SUBTITLETYPE_Ebu )
			{

				out_comp->eSubType = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_EBU;
				out_comp->ulComponentTag = stComponet.stSubtitle.utArg.stEbuInfo.nComponentTag;
				HLIB_STD_StrNCpySafe(out_comp->szLanguage,
					reinterpret_cast<char *>(stComponet.stSubtitle.utArg.stEbuInfo.szLangCode), 4);
			}
		}
		break;
	default: break;
	}


	return hErr;
}

int OplVodHtmlFive::getActiveComponentIndex(eOplAVComponentType comp_type)
{
	int index = -1;
	HERROR ret = ERR_FAIL;
	
	switch(comp_type)
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			{
				ret = APK_DOWNLOAD_GetComponentIndex(m_download_id, eOxDNLD_COMPONENT_VIDEO, &index, NULL);
			}
			break;
		case OPLAVCOMPONENT_TYPE_AUDIO:
			{
				ret = APK_DOWNLOAD_GetComponentIndex(m_download_id, eOxDNLD_COMPONENT_AUDIO, &index, NULL);
			}
			break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			{
				ret = APK_DOWNLOAD_GetComponentIndex(m_download_id, eOxDNLD_COMPONENT_SUBTITLE, &index, NULL);
			}
			break;
		default: break;	
	}

	if( ret != ERR_OK ) index = -1;
	
	return index;
}


double OplVodHtmlFive::getStartDate(void)
{
	double start_date = 0;
	HUINT8 starttime[256] = {0,};
	struct tm _base, _time;	
	time_t base, time;
	HERROR err = ERR_FAIL;

 	_base.tm_sec = 0;
    _base.tm_min = 0;
    _base.tm_hour = 0;
    _base.tm_mday = 1;
    _base.tm_mon = 0;
    _base.tm_year = 70;
    _base.tm_isdst = -1;
	_base.tm_gmtoff = 0;
	base = mktime(&_base);

	err = APK_DOWNLOAD_GetAvailabilityStartTime(m_download_id, 256, starttime);
	HxLOG_Debug("StartDate == %s", starttime);

	if( (strstr((char *)starttime, "None") != NULL ) || \
			(err == ERR_FAIL) || (starttime[0] == 0) )
	{
		HxLOG_Debug("StartDate == None");
		return -1;
	}

	strptime((char *)starttime, "%Y-%m-%dT%H:%M:%SZ", &_time); 
	_time.tm_isdst = -1;
	time = mktime(&_time);
	start_date = difftime(time + _time.tm_gmtoff, base);

	return start_date * 1000.;
}

/*
bool OplVodHtmlFive::setActiveComponent(eOplAVComponentType comp_type, int pid, char *pszLang)
{
	HERROR hErr = ERR_FAIL;
	OxDownload_Component_e download_comp_type = (OxDownload_Component_e)__convertComponentType(comp_type);
	
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		return setActiveVideoComponent(pid);
	case OPLAVCOMPONENT_TYPE_AUDIO:
		return setActiveAudioComponent(pid, pszLang);
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		return setActiveSubtitleComponent(pid, pszLang);
	default:
		break;
	}

	return false;
}

bool OplVodHtmlFive::offComponent(eOplAVComponentType comp_type, int pid)
{
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		return false;
	case OPLAVCOMPONENT_TYPE_AUDIO:
		return false;
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		return offSubtitleComponent(pid);
	default:
		break;
	}

	return false;
}
*/

int OplVodHtmlFive::__convertComponentType(eOplAVComponentType type)
{
	OxDownload_Component_e compType = eOxDNLD_COMPONENT_NONE;
	
	switch(type)
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			compType = eOxDNLD_COMPONENT_VIDEO; break;
		case OPLAVCOMPONENT_TYPE_AUDIO:
			compType = eOxDNLD_COMPONENT_AUDIO; break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			compType = eOxDNLD_COMPONENT_SUBTITLE; break;
		case OPLAVCOMPONENT_TYPE_SUPERIMPOSE:
			compType = eOxDNLD_COMPONENT_NONE; break;
		case OPLAVCOMPONENT_TYPE_DATA:
			compType = eOxDNLD_COMPONENT_DATA; break;
		case OPLAVCOMPONENT_TYPE_MAX:
			compType = eOxDNLD_COMPONENT_NONE; break;
		default: break;
	}

	return (int)compType;
}

/*
 *	Private
 */
int OplVodHtmlFive::__audio_codec_to_appkit(eOplVodAudioCodec codec)
{
	DxAudioCodec_e appkit_codec = eDxAUDIO_CODEC_UNKNOWN;
	switch ( codec )
	{
	case OPLVOD_AUDIO_CODEC_MPEG:
		appkit_codec = eDxAUDIO_CODEC_MPEG;
		break;
	case OPLVOD_AUDIO_CODEC_MP3:
		appkit_codec = eDxAUDIO_CODEC_MP3;
		break;
	case OPLVOD_AUDIO_CODEC_DOLBY_AC3:
		appkit_codec = eDxAUDIO_CODEC_DOLBY_AC3;
		break;
	case OPLVOD_AUDIO_CODEC_DOLBY_AC3P:
		appkit_codec = eDxAUDIO_CODEC_DOLBY_AC3P;
		break;
	case OPLVOD_AUDIO_CODEC_PCM:
		appkit_codec = eDxAUDIO_CODEC_PCM;
		break;
	case OPLVOD_AUDIO_CODEC_AAC:
		appkit_codec = eDxAUDIO_CODEC_AAC;
		break;
	case OPLVOD_AUDIO_CODEC_AAC_LOAS:
		appkit_codec = eDxAUDIO_CODEC_AAC_LOAS;
		break;
	case OPLVOD_AUDIO_CODEC_AAC_PLUS:
		appkit_codec = eDxAUDIO_CODEC_AAC_PLUS;
		break;
	default:
		break;
	}

	return static_cast<int>(appkit_codec);
}

int OplVodHtmlFive::__video_codec_to_appkit(eOplVodVideoCodec codec)
{
	DxVideoCodec_e appkit_codec = eDxVIDEO_CODEC_UNKNOWN;
	switch ( codec )
	{
	case OPLVOD_VIDEO_CODEC_MPEG1:
		appkit_codec = eDxVIDEO_CODEC_MPEG1;
		break;
	case OPLVOD_VIDEO_CODEC_MPEG2:
		appkit_codec = eDxVIDEO_CODEC_MPEG2;
		break;
	case OPLVOD_VIDEO_CODEC_MPEG4_PART2:
		appkit_codec = eDxVIDEO_CODEC_MPEG4_PART2;
		break;
	case OPLVOD_VIDEO_CODEC_H261:
		appkit_codec = eDxVIDEO_CODEC_H261;
		break;
	case OPLVOD_VIDEO_CODEC_H263:
		appkit_codec = eDxVIDEO_CODEC_H263;
		break;
	case OPLVOD_VIDEO_CODEC_H264:
		appkit_codec = eDxVIDEO_CODEC_H264;
		break;
	case OPLVOD_VIDEO_CODEC_H264_SVC:
		appkit_codec = eDxVIDEO_CODEC_H264_SVC;
		break;
	case OPLVOD_VIDEO_CODEC_H264_MVC:
		appkit_codec = eDxVIDEO_CODEC_H264_MVC;
		break;
	case OPLVOD_VIDEO_CODEC_VC1:
		appkit_codec = eDxVIDEO_CODEC_VC1;
		break;
	case OPLVOD_VIDEO_CODEC_VC1_SIMPLEMAIN:
		appkit_codec = eDxVIDEO_CODEC_VC1_SIMPLEMAIN;
		break;
	case OPLVOD_VIDEO_CODEC_AVS:
		appkit_codec = eDxVIDEO_CODEC_AVS;
		break;
	case OPLVOD_VIDEO_CODEC_RV40:
		appkit_codec = eDxVIDEO_CODEC_RV40;
		break;
	case OPLVOD_VIDEO_CODEC_VP6:
		appkit_codec = eDxVIDEO_CODEC_VP6;
		break;
	case OPLVOD_VIDEO_CODEC_DIVX311:
		appkit_codec = eDxVIDEO_CODEC_DIVX311;
		break;
	case OPLVOD_VIDEO_CODEC_XVID:
		appkit_codec = eDxVIDEO_CODEC_XVID;
		break;
	default:
		break;
	}

	return static_cast<int>(appkit_codec);
}


#define _________________________________download_function________________________________________

OplVodHtmlFive *OplVodHtmlFive::findVodByDownloadid(const unsigned int id)
{
	if ( s_vods.empty() ) return NULL;

	list<OplVod*>::iterator begin_it = s_vods.begin();
	for ( list<OplVod*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		OplVodHtmlFive *html5_vod = static_cast <OplVodHtmlFive *> (*it);
		if ( html5_vod->m_download_id == id ) 	return html5_vod;
	}

	return NULL;
}

const char *OplVodHtmlFive::getDownloadStateString(eOplVodDownloadState state)
{
	switch ( state )
	{
		case OPLVOD_DOWNLOAD_STATE_NONE:
			return "none";
		case OPLVOD_DOWNLOAD_STATE_STARTED:
			return "start";
		case OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED:
			return "probe complete";
		case OPLVOD_DOWNLOAD_STATE_PAUSE:
			return "pause";
		case OPLVOD_DOWNLOAD_STATE_COMPLETED:
			return "complete";
		case OPLVOD_DOWNLOAD_STATE_CREATE:
			return "create";
		default:
			return "unknown";
	}

	return NULL;
}


void  OplVodHtmlFive::debugVodList(void){
	
	list<OplVod*>::iterator begin_it = s_vods.begin();
	HxLOG_Debug( "***************************************** VOD LIST ************************************\n");
	for ( list<OplVod*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		OplVodHtmlFive *html5_vod = static_cast <OplVodHtmlFive *> (*it);
		if( NULL != html5_vod )
		{
			HxLOG_Debug( "ACTIVE[%d] URL(%s)\n", html5_vod->m_is_active, html5_vod->getUrl());
		}
	}
	HxLOG_Debug( "***************************************************************************************\n");
}

const char *OplVodHtmlFive::getDownloadUrl(){
	ENTRY;
	return m_dnld_url.c_str();
}

void OplVodHtmlFive::setDownloadState(eOplVodDownloadState state){
	ENTRY;
	m_download_state = state;
}

eOplVodDownloadState OplVodHtmlFive::getDownloadState(){
	ENTRY;
	return m_download_state;
}

int OplVodHtmlFive::startDownload(void)
{
	DxDownload_Start_t arg;
	memset(&arg, 0x00, sizeof(DxDownload_Start_t));

	arg.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.bContentVerification = true;
	arg.stIpMedia.bMhegIcsStream = false;
	HxSTD_snprintf(arg.stIpMedia.szUrl, DxDNLD_MEDIAPLAY_URL, "%s", getUrl());

	return APK_DOWNLOAD_Start (0, &arg);
}

int OplVodHtmlFive::probeDownload(void)
{	
	DxDownload_Start_t arg;
	memset(&arg, 0x00, sizeof(DxDownload_Start_t));

	arg.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.bContentVerification = false;
	arg.stIpMedia.bMhegIcsStream = false;
	HxSTD_snprintf(arg.stIpMedia.szUrl, DxDNLD_MEDIAPLAY_URL, "%s", getUrl());

	return  APK_DOWNLOAD_Probe (&arg);
}

bool OplVodHtmlFive::stopDownload(void)
{
	HERROR eRet = APK_DOWNLOAD_Stop(m_download_id);
	return eRet == ERR_OK;
}

bool OplVodHtmlFive::resumeDownload(void)
{
	HERROR eRet = APK_DOWNLOAD_Resume (m_download_id);
	return eRet == ERR_OK;
}

bool OplVodHtmlFive::pauseDownload(void)
{
	HERROR eRet = APK_DOWNLOAD_Pause (m_download_id);
	return eRet == ERR_OK;
}

bool OplVodHtmlFive::getDownloadStatus(OplDownloadStatus_t *status)
{
	HERROR eRet = ERR_OK;
	DxDownload_Status_t _status;

	memset(&_status, 0x00, sizeof(DxDownload_Status_t));
	eRet = APK_DOWNLOAD_GetStatus (m_download_id, &_status);

	return eRet == ERR_OK;
}

void OplVodHtmlFive::setDownloadInfo(unsigned int id, unsigned duration){
	
	char tmp[OxMP_MEDIAPLAY_URL];
	memset(tmp, 0x00, OxMP_MEDIAPLAY_URL);
	HxSTD_snprintf(tmp, OxMP_MEDIAPLAY_URL, OPLVOD_DOWNLOAD_URI_FORM, id);

	m_dnld_url = tmp;
	m_duration = duration;
}

static void oplvod_download_proc(HUINT32 ulDownloadId, DxDownload_Type_e eType, DxDownload_Event_e eEvent, DxDownload_NotifierData_t *punNotifyData, void *pvUserData)
{ 
	if ( OplVod::s_vods.empty() ) return;

	OplVodHtmlFive *vod = OplVodHtmlFive::findVodByDownloadid(ulDownloadId);

	if ( !vod ) return;

	switch( eEvent ) 
	{
		case eDxDNLD_EVENT_Started:
			HxLOG_Debug("download event : eDxDNLD_EVENT_Started \n");
			break;

		case eDxDNLD_EVENT_Completed:
			HxLOG_Debug("download event : eDxDNLD_EVENT_Completed \n");
			HxLOG_Debug("OplVodHtmlFive::oplvod_download_proc - download completed (url=%s, id=%d)  \n", vod->getUrl(), ulDownloadId); 
			break;

		case eDxDNLD_EVENT_Failed:
			HxLOG_Debug("download event : eDxDNLD_EVENT_Failed \n");
			break;

		case eDxDNLD_EVENT_DataReceiving:
			HxLOG_Debug("download event : eDxDNLD_EVENT_DataReceiving \n");
			break;

		case eDxDNLD_EVENT_ProbeSuccess:
			{
				HxLOG_Debug("download event : eDxDNLD_EVENT_ProbeSuccess \n");
				unsigned int seekable_chunk_count = 0;
				unsigned long long *seekable_start = NULL, *seekable_end = NULL;
				HERROR eRet = ERR_OK;

				HxLOG_Debug("OplVodHtmlFive::oplvod_download_proc - download probe success (url=%s, id=%d)  \n", vod->getUrl(), ulDownloadId); 
#if defined(CONFIG_TIMESTAMP_64)
				HxLOG_Debug("-- info : cso_session_id=%d, duration=%llu, bitrate=%d, bInfinity=%d \n", \
						punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ullDuration, punNotifyData->stDnldSessionInfo.ulMaxBitrate, punNotifyData->stDnldSessionInfo.bInfinity );
#else
				HxLOG_Debug("-- info : cso_session_id=%d, duration=%d, bitrate=%d, bInfinity=%d \n", \
						punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ulDuration, punNotifyData->stDnldSessionInfo.ulMaxBitrate, punNotifyData->stDnldSessionInfo.bInfinity );
#endif
				
				eRet = APK_DOWNLOAD_GetSeekableRangeInfo( ulDownloadId, &seekable_chunk_count, &seekable_start, &seekable_end);
				HxLOG_Debug("OplVodHtmlFive::getSeekableInfo:chunk_count %d\n", seekable_chunk_count);

#if defined(CONFIG_TIMESTAMP_64)
				vod->setDownloadInfo(punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ullDuration);
#else
				vod->setDownloadInfo(punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ulDuration);
#endif
				HxLOG_Debug("vod->getDownloadState() : %d \n", vod->getDownloadState());
				switch(vod->getDownloadState()){
					case OPLVOD_DOWNLOAD_STATE_PAUSE:
						{
							vod->setDownloadState(OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED);
							if( vod->m_is_active ) {
								OplVodProbeInfo info;
								memset(&info, 0x00, sizeof(OplVodProbeInfo));
								HxLOG_Debug("OplVodHtmlFive::DownloadProbeSuccess -  %s play retry.\n", vod->getUrl() );
								vod->play(&info);
							}
						}
						break;
					case OPLVOD_DOWNLOAD_STATE_STARTED:
						{

							vod->m_duration_Infinity = punNotifyData->stDnldSessionInfo.bInfinity;
							vod->setDownloadState(OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED);
							vod->setBufferState(OPLVOD_BUFFER_STATE_CANPLAY);
							vod->callback(OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED);

#if defined(CONFIG_TIMESTAMP_64)
							vod->setStartPos(punNotifyData->stDnldSessionInfo.ullStartPos);
#else
							vod->setStartPos(punNotifyData->stDnldSessionInfo.ulStartPos);
#endif
							vod->callback(OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED);
						}
						break;

					default: break;
				}

				OplVodHtmlFive::debugVodList();
			}
			break;
		case eDxDNLD_EVENT_ProbeFailed:
			{
				HxLOG_Debug("download event : eDxDNLD_EVENT_ProbeFailed \n");	
				vod->setPlayError(OPLVOD_PLAY_ERROR_SERVER_FAIL);
				vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);
				vod->callback(OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED);
			}
			break;
		case eDxDNLD_EVENT_DownloadInfoChanged:
			HxLOG_Debug("download event :eDxDNLD_EVENT_DownloadInfoChanged \n");
			vod->callback(OPLVOD_EVENT_TYPE_BUFFER_INFO_CHANGED);
			break;
		default:
			break;
	}
}
