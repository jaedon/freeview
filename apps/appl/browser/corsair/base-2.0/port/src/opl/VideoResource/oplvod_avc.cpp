/*************************************************************************************************************
	File 		: oplvod_avc.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/10/16
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (징째Humax징짹) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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

#include "apk.h"
#include "oplvod_avc.h"
#include "oplvod_extapp.h"

/****************************************************************************************
 * OplVodAVControl 
 ***************************************************************************************/
#define ENTRY	HxLOG_Debug("[OplVodAVControl][%s:%d]++\n", __FUNCTION__, __LINE__);
#define EXIT	HxLOG_Debug("[OplVodAVControl][%s:%d]--\n", __FUNCTION__, __LINE__);

std::list<OplVodAVControl *> OplVodAVControl::s_av_vods;

OplVodAVControl::OplVodAVControl(int aVideoResouceId)
	: OplVod(aVideoResouceId), m_cache(false), m_cached_position(0)
{
	s_av_vods.push_back(this);	
}

OplVodAVControl::~OplVodAVControl()
{
	if(!m_queue_url.empty()) m_queue_url.clear();
	s_av_vods.remove(this);
}

bool OplVodAVControl::probe()
{
	return OplVod::probe(); 
}

bool OplVodAVControl::play()
{
	ENTRY;
	
	HERROR eRet = ERR_OK;

	OplVodAVControl* running_vod = NULL;
	HxLOG_Debug("OplVodAVControl::play:cache=%d\n", m_cache);
	if( m_cache )
	{
		for ( std::list<OplVodAVControl*>::iterator it=s_av_vods.begin(); it!=s_av_vods.end(); it++ )
		{
			if( (*it) != this )
			{
				HxLOG_Debug("OplVodAVControl::play:has a running application\n");
				running_vod = (*it);
			}
		}

		if( running_vod && running_vod->getPlayState() == OPLVOD_PLAY_STATE_PLAYING)
		{
			running_vod->m_cached_position = running_vod->getPlayPosition();	
			running_vod->m_cached_url = running_vod->m_url;
			HxLOG_Debug("OplVodAVControl::play:position=%d, url=%s\n", running_vod->m_cached_position, running_vod->m_cached_url.c_str());
			running_vod->stop();
		}
	}

	if ( m_play_state == OPLVOD_PLAY_STATE_STOPPED
			|| m_play_state == OPLVOD_PLAY_STATE_FINISHED
			|| m_play_state == OPLVOD_PLAY_STATE_ERROR 
			|| m_play_state == OPLVOD_PLAY_STATE_PAUSED
			|| m_play_action == OplVod::VOD_ACTION_NONE 
			|| m_play_action == OplVod::VOD_ACTION_STOP
			|| m_play_action == OplVod::VOD_ACTION_INTERNAL_STOP )
	{
		if ( !OplVod::probe() )
		{
			return false;
		}

		if ( !OplVod::play() )
		{
			return false;
		}

		// (workaround) handstudio app 에서 playstate를 사용하지 않아 재생이 정상적으로 되지 않는 문제
		// stoped 상태에서 play(0) 일경우 pause 로 변해야 하는 상황에 대한 처리는 필요 
		// connecting 으로 판단해 주기전에 paused 인 상태일 경우에 대해서 처리가 필요 
		HxLOG_Debug("OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED: %s ==> %s\n",
				OplVod::getPlayStateString(this->getPlayState()), OplVod::getPlayStateString(OPLVOD_PLAY_STATE_CONNECTING));
		this->callback(OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED);
		this->setPlayState(OPLVOD_PLAY_STATE_CONNECTING);
	}

	touch();
	
	EXIT;

	return eRet == ERR_OK;
}

bool OplVodAVControl::stop(bool internal)
{
	ENTRY;
	
	HERROR eRet = ERR_OK;
	OxMediaPlay_MediaType_e eOapiType;

	HxLOG_Debug("OplVod::stop: m_url = NULL, m_play_action : %d, m_play_state : %d\n", m_play_action, m_play_state);
	this->m_url = "";
	
	if ( m_play_action == VOD_ACTION_STOP
		|| m_play_action == VOD_ACTION_INTERNAL_STOP
		|| ( m_play_state == OPLVOD_PLAY_STATE_STOPPED && internal == TRUE ))
	{
		//APK_MEDIA_PLAY_Stop( m_dest /*ulViewId*/);
		HxLOG_Debug("OplVod::stop: the playback is already stopped.\n");
		m_play_action = (internal) ? VOD_ACTION_INTERNAL_STOP : VOD_ACTION_STOP;
		m_play_state = OPLVOD_PLAY_STATE_STOPPED;
		return true;
	}

	APK_MEDIA_PLAY_GetPlayType( m_dest , &eOapiType );
	if ( eOxMP_MEDIATYPE_LIVE != eOapiType )
	{
		eRet = APK_MEDIA_PLAY_Stop( m_dest /*ulViewId*/);
	}

	if ( !internal )
	{
		/* workaround for Obigo Applications: invalid handling: stopped event */
		if ( OplVodExtApp::stayMusicBaebleApp() || OplVodExtApp::stayItalianFoodApp()
				|| OplVodExtApp::stayAbsoluteRadioApp() )
		{
			HxLOG_Debug("OplVod::stop: Music Bable bug fix (workaround).\n");
			setPlayState( OPLVOD_PLAY_STATE_STOPPED );
		}

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



bool OplVodAVControl::queue(const char *url)
{
	ENTRY;
	HxLOG_Debug("OplVod::queue:url=%s\n", url);
	std::string add_url = url;
	if( strcmp(url, "null") == 0 )
	{
		if( !m_queue_url.empty() ) 
		{
			HxLOG_Debug("OplVod::queue:clear\n");
			m_queue_url.clear();
		}
		
		EXIT;
		return true;
	}
	
	m_queue_url.push_back(add_url);
	EXIT;
	
	return true;
}

bool OplVodAVControl::setLoop(int count)
{
	ENTRY;
	HxLOG_Debug("OplVod::setLoop:count%d\n", count);
	m_loop = count;
	EXIT;
	return true;
}

bool OplVodAVControl::setCache(bool cache)
{
	ENTRY;
	HxLOG_Debug("OplVod::setCache=%d\n", cache);
	m_cache = cache;
	EXIT;
	return true;
}

bool OplVodAVControl::getCache()
{
	ENTRY;
	EXIT;
	return m_cache;
}

bool OplVodAVControl::cachePlay()
{
	ENTRY;
	OplVodAVControl* running_vod = NULL;
	for ( std::list<OplVodAVControl*>::iterator it=s_av_vods.begin(); it!=s_av_vods.end(); it++ )
	{
		running_vod = (*it);
		if( running_vod && running_vod->m_cached_url.c_str() && running_vod->m_cached_position )
		{
			HxLOG_Debug("OplVodAVControl::cachePlay:m_cached_position=%d, m_cached_url=%s, m_url=%s\n", running_vod->m_cached_position, running_vod->m_cached_url.c_str(), running_vod->m_url.c_str());
			running_vod->setUrl(running_vod->m_cached_url.c_str());
			running_vod->probe();

			HUINT32 ulSessionId = 0;
			OxMediaPlay_StartInfo_t unStart;
			HxSTD_memset( &unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t) );
			unStart.stMediaPlay.eType = eOxMP_MEDIATYPE_MEDIAPLAY;
#if defined(CONFIG_TIMESTAMP_64)
			unStart.stMediaPlay.ullStartTime = running_vod->m_cached_position;
#else
			unStart.stMediaPlay.ulStartTime = running_vod->m_cached_position;
#endif
			HxSTD_StrNCpy( unStart.stMediaPlay.szUrl, running_vod->m_cached_url.c_str(), sizeof(running_vod->m_cached_url.c_str())-1 );
			
			if ( ERR_OK != APK_MEDIA_PLAY_Start (m_dest, &unStart, &ulSessionId) )
			{
				HxLOG_Error ("[%s:%d] Error> APK_MEDIA_PLAY_Start err: [ViewId=%u, SessionId=%u]\n", __FUNCTION__, __LINE__, m_dest, ulSessionId);
				return false;
			}
			
			m_play_action = VOD_ACTION_PLAY;
			m_session_id = ulSessionId;
			m_is_active = true;
		}
	}
	EXIT;
	return true;
}

