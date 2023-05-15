/** **********************************************************************************************************
	@file 		NativePlayer.cpp

	@date		2013/10/22
	@author		STB-Component
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
#include "NativePlayer.h"
#include "NativeBufferingTypes.h"
#include "Log.h"
#include "DrmConfig.h"
#include "PlayReadyDrmConfig.h"
#include "AbrConfig.h"
#include "SmoothStreamingAbrConfig.h"

#include <list>

#ifdef DUMMY_OPL
#include "dummy_opl/opl_audiostream.h"
#include "dummy_opl/opl_videostream.h"
#include "dummy_opl/opl_player.h"
#endif

PlayerListener *g_PlayerListener = NULL;

using namespace std;
using namespace Lovefilm;

list<fragment_data *> g_Fragments;
fragment_data *Fragment_GetInstance(FragmentHandle id)
{
	for (list<fragment_data *>::iterator it = g_Fragments.begin(); it != g_Fragments.end(); it++)
	{
		if (static_cast<FragmentHandle>(*it) == id)
			return *it;
	}
	return NULL;
}

	
#ifdef DUMMY_OPL
static void native_Player_event_listener(void *handle, eOplPlayerEventType type)
{
	OplPlayer *player = static_cast<OplPlayer *>(handle);
	if ( handle == NULL ) return;

	PlayerEventInfo info;
	switch (type)
	{
		case OPLPLAYER_EVENT_TYPE_CONNECTION_ATTEMPTED:
			g_PlayerListener(PLAYER_EVENT_CONNECTION_ATTEMPTED, NULL, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_CONNECTION_ESTABLISHED:
			g_PlayerListener(PLAYER_EVENT_CONNECTION_ESTABLISHED, NULL, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_METADATA:
			g_PlayerListener(PLAYER_EVENT_METADATA, NULL, static_cast<PlayerHandle>(player));			
			break;
			
		case OPLPLAYER_EVENT_TYPE_STREAM_STARTED_DOWNLOADING:
			info.player_stream_started_downloading.stream = NULL; // actual stream handle. 
			g_PlayerListener(PLAYER_EVENT_STREAM_STARTED_DOWNLOADING, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_STREAM_STOPPED_DOWNLOADING:
			info.player_stream_stopped_downloading.stream = NULL; // actual stream handle. 
			g_PlayerListener(PLAYER_EVENT_STREAM_STOPPED_DOWNLOADING, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_STREAM_BECAME_ACTIVE:
			info.player_stream_became_active.stream = NULL; // actual stream handle. 
			g_PlayerListener(PLAYER_EVENT_STREAM_BECAME_ACTIVE, &info, static_cast<PlayerHandle>(player));
			break;			
			
		case OPLPLAYER_EVENT_TYPE_BUFFER_STARTED:
			info.player_buffer_started.type = (char *)STR_INITIAL_LOAD; // actual buffering type
			info.player_buffer_started.stream = NULL; // actual stream handle. 
			g_PlayerListener(PLAYER_EVENT_BUFFER_STARTED, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_BUFFER_COMPLETED:
			info.player_buffer_completed.type = (char *)STR_INITIAL_LOAD; // actual buffering type
			info.player_buffer_completed.stream = NULL; // actual stream handle. 
			g_PlayerListener(PLAYER_EVENT_BUFFER_COMPLETED, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_FRAGMENTS_DOWNLOADED:
		{
			fragment_data *newFragment = new fragment_data;
			newFragment->fragmentIndex = 0;
			newFragment->fragmentSize = 0;
			newFragment->downloadDuration = 0;
			newFragment->retryAttemps = 0;
			newFragment->serverIp = 0;
			newFragment->stream = NULL;

			g_Fragments.push_back(newFragment);
			g_PlayerListener(PLAYER_EVENT_FRAGMENTS_DOWNLOADED, NULL, static_cast<PlayerHandle>(player));
		}
			break;
			
		case OPLPLAYER_EVENT_TYPE_READY:
			g_PlayerListener(PLAYER_EVENT_READY, NULL, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_STREAM_SWITCH_STARTED:
			info.player_stream_switch_started.targetStream = 0;
			g_PlayerListener(PLAYER_EVENT_STREAM_SWITCH_STARTED, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_STREAM_SWITCH_COMPLETED:
			info.player_stream_switch_completed.newStream = 0;
			info.player_stream_switch_completed.streamBitrate = 0;
			g_PlayerListener(PLAYER_EVENT_STREAM_SWITCH_COMPLETED, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_PLAYING:
			g_PlayerListener(PLAYER_EVENT_PLAYING, NULL, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_PAUSED:
			g_PlayerListener(PLAYER_EVENT_PAUSED, NULL, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_SEEK_STARTED:
			info.player_seek_started.targetTime = 0;
			g_PlayerListener(PLAYER_EVENT_SEEK_STARTED, &info, static_cast<PlayerHandle>(player));
			break;

		case OPLPLAYER_EVENT_TYPE_SEEK_COMPLETED:
			info.player_seek_completed.currentTime= 0;
			g_PlayerListener(PLAYER_EVENT_SEEK_COMPLETED, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_PLAYBACK_SPEED_CHANGED:
			info.player_playback_speed_changed.speed = 0;
			g_PlayerListener(PLAYER_EVENT_PLAYBACK_SPEED_CHANGED, &info, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_VOLUME_CHANGED:
			info.player_volume_changed.speed = 0;
			g_PlayerListener(PLAYER_EVENT_VOLUME_CHANGED, &info, static_cast<PlayerHandle>(player));			
			break;
			
		case OPLPLAYER_EVENT_TYPE_STOPPED:
			g_PlayerListener(PLAYER_EVENT_STOPPED, NULL, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_ENDED:
			g_PlayerListener(PLAYER_EVENT_ENDED, NULL, static_cast<PlayerHandle>(player));
			break;
			
		case OPLPLAYER_EVENT_TYPE_UNLOADED:
			g_PlayerListener(PLAYER_EVENT_UNLOADED, NULL, static_cast<PlayerHandle>(player));
			break;
			
		default:
			break;
	}
}
#endif	

NJSEReturnCode native_PlayerCreate(PlayerHandle *id)
{
#ifdef DUMMY_OPL
	OplPlayer *player = new OplPlayer();
	if(player)
	{
		player->setListener(native_Player_event_listener);
		*id = static_cast<void *>(player);
	}
	else
#endif		
		*id = NULL;
	
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerReleaseHandle(PlayerHandle i )
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(i);

	delete player;
#endif
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetListener(PlayerListener listener)
{
	g_PlayerListener = listener;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetFullScreen(PlayerHandle id, NJSEBoolean fullscreen)
{
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerOnListenerAccessed(PlayerHandle id, const char *eventName)
{
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetVisibility(PlayerHandle id, NJSEBoolean visibility, NJSEBoolean unloaded)
{
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetVideoWindow(PlayerHandle id, int x, int y, int width, int height)
{
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetAvailableStreams(int *count, StreamTypesHandle **handles)
{
#ifdef DUMMY_OPL
	*count = 2;
	void **myHandles = (void **)malloc(sizeof(void *)*2);

	myHandles[0] = opl_audiostream_get_dummy();
	myHandles[1] = opl_videostream_get_dummy();
	*handles = myHandles;
#else
	*count = 0;
	*handles = NULL;
#endif

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetActiveStreams(int *count, StreamTypesHandle **handles)
{
	*count = 0;
	*handles = NULL;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetDownloadingStreams(int *count, StreamTypesHandle **handles)
{
	*count = 0;
	*handles = NULL;

	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetCurrentTime(NJSENumber *currentTime)
{
	*currentTime = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetCurrentBufferSize(NJSENumber *currentBufferSize)
{
	*currentBufferSize = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetCurrentBufferTime(NJSENumber *currentBufferTime)
{
	*currentBufferTime = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetMaxBufferSize(NJSENumber *maxBufferSize)
{
	*maxBufferSize = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetDroppedFrames(NJSENumber *droppedFrames)
{
	*droppedFrames = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetAverageFrameRate(NJSENumber *averageFrameRate)
{
	*averageFrameRate = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetAverageBandwidth(NJSENumber *averageBandwidth)
{
	*averageBandwidth= 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetFragments(int *count, FragmentHandle **handles)
{
	void **myHandles = NULL;
	int index = 0;
	
	*count = g_Fragments.size();
	if(*count != 0 )
		myHandles = new FragmentHandle [*count];
	
	for (list<fragment_data *>::iterator it = g_Fragments.begin(); it != g_Fragments.end(); it++)
	{
		myHandles[index] = static_cast<FragmentHandle>(*it);
		index++;
	}
	*handles = myHandles;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetFragmentIndex(FragmentHandle i, NJSENumber *index)
{
	fragment_data *fragment = Fragment_GetInstance(i);
	if(!fragment)
		return NJSE_RETURN_VALUE_NULL;

	*index = fragment->fragmentIndex;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetFragmentSize(FragmentHandle i, NJSENumber *size)
{
	fragment_data *fragment = Fragment_GetInstance(i);
	if(!fragment)
		return NJSE_RETURN_VALUE_NULL;

	*size = fragment->fragmentSize;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetFragmentDownloadDuration(FragmentHandle i, NJSENumber *duration)
{
	fragment_data *fragment = Fragment_GetInstance(i);
	if(!fragment)
		return NJSE_RETURN_VALUE_NULL;

	*duration = fragment->downloadDuration;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetFragmentRetryAttempts(FragmentHandle i, NJSENumber *retry)
{
	fragment_data *fragment = Fragment_GetInstance(i);
	if(!fragment)
		return NJSE_RETURN_VALUE_NULL;

	*retry = fragment->retryAttemps;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetFragmentServerIp(FragmentHandle i, NJSENumber *serverIp)
{
	fragment_data *fragment = Fragment_GetInstance(i);
	if(!fragment)
		return NJSE_RETURN_VALUE_NULL;

	*serverIp = fragment->serverIp;
	return NJSE_RETURN_OK;
}
NJSEReturnCode native_PlayerGetFragmentStream(FragmentHandle i, StreamTypesHandle *handle)
{
	fragment_data *fragment = Fragment_GetInstance(i);
	if(!fragment)
		return NJSE_RETURN_VALUE_NULL;

	*handle = fragment->stream;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetAssetUrl(PlayerHandle id, const char *url, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setAssetUrl(url);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetAssetUrl(PlayerHandle id, char **url)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	*url = player->getAssetUrl();
#endif	
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerConfigureDrm(PlayerHandle id, void * drmConfig, NJSEBoolean *ret)
{
	DrmConfig *drm = (DrmConfig *)drmConfig;
	PlayReadyDrmConfig *playready = (PlayReadyDrmConfig *)drm->getPlayReady();
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setDrmConfig((const char *) playready->getDRMType());
#endif	

	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerConfigureAbr(PlayerHandle id, void * drmConfig, NJSEBoolean *ret)
{
	AbrConfig *abr = (AbrConfig *)drmConfig;
	SmoothStreamingAbrConfig *smooth = (SmoothStreamingAbrConfig *)abr->getSmoothStreaming();
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setBandwidthMultiplier(smooth->getBandWidthMultiplier());
	///// set all Abr configurations here. 
#endif	

	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerLoadMetadata(PlayerHandle id, NJSEBoolean *ret)
{
	// load metadata and fire one of following events. 
	/*
		Device.player.player_connection_attempted
		Device.player.player_connection_established
		Device.player.player_metadata 
	*/
	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetPalyReadyChallenge(PlayerHandle id, char **ret)
{
	//make PalyreadyChallenge
	char *PalyreadyChallenge = (char *)"PalyreadyChallenge";
	*ret = PalyreadyChallenge;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetPlayReadyLicense(PlayerHandle id, const char *license, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setPlayReadyLicense(license);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerPlay(PlayerHandle id, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->play();
#endif	
	// fire following event.
	/*
		Device.player.player_playing
	*/
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerPause(PlayerHandle id, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->pause();
#endif	
	// fire following event.
	/*
		Device.player.player_paused
	*/
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSeek(PlayerHandle id, NJSENumber time, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->seek(time);
#endif	
	// fire one of following events. 
	/*
		Device.player.player_seek_started
		Device.player.player_buffer_started
		Device.player.player_buffer_completed
		Device.player.player_seek_completed 
	*/

	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerStop(PlayerHandle id, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->stop();
#endif	
	// fire following event. 
	/*
		Device.player.player_stopped 
	*/

	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerUnload(PlayerHandle id, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->unload();
#endif	
	// fire following event. 
	/*
		Device.player.player_unloaded 
	*/

	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetPlaybackSpeed(PlayerHandle id, NJSENumber speed, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setPlaybackSpeed(speed);
#endif	
	// fire following event. 
	/*
		Device.player.player_playback_speed_changed 
	*/

	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetBufferTimeRequiredForInitialPlayback(PlayerHandle id, NJSENumber time, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setBufferTimeRequiredForInitialPlayback(time);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetBufferTimeRequiredAfterExhaustion(PlayerHandle id, NJSENumber time, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setBufferTimeRequiredAfterExhaustion(time);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetBufferTimeRequiredAfterStreamSwitch(PlayerHandle id, NJSENumber time, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setBufferTimeRequiredAfterStreamSwitch(time);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetBufferTimeRequiredAfterSeeking(PlayerHandle id, NJSENumber time, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setBufferTimeRequiredAfterSeeking(time);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetFragmentsDownloadedEventFrequency(PlayerHandle id, NJSENumber frequency, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setFragmentsDownloadedEventFrequency(frequency);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetActiveStream(PlayerHandle id, const char * type, StreamTypesHandle stream, NJSEBoolean *ret)
{
	//firing one of follwing events. 
	/*
		Device.player.player_stream_switch_started
		Device.player.player_stream_started_downloading
		Device.player.player_stream_became_active
		Device.player.player_stream_switch_completed 
	*/
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetPosition(PlayerHandle id, NJSENumber *x, NJSENumber *y)
{
	*x = 0;
	*y = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetPosition(PlayerHandle id, NJSENumber x, NJSENumber y, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setPosition(x, y);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetSize(PlayerHandle id, NJSENumber *width, NJSENumber *height)
{
	*width = 0;
	*height = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetSize(PlayerHandle id, NJSENumber width, NJSENumber height, NJSEBoolean *ret)
{
#ifdef DUMMY_OPL
	OplPlayer *player = static_cast<OplPlayer *>(id);
	player->setSize(width, height);
#endif	
	*ret = TRUE;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerGetVolume(PlayerHandle id, NJSENumber *volume)
{
	*volume = 0;
	return NJSE_RETURN_OK;
}

NJSEReturnCode native_PlayerSetVolume(PlayerHandle id, NJSENumber volume, NJSEBoolean *ret)
{
	//firing following event
	/*
		Device.player.player_volume_changed 
	*/
	*ret = TRUE;
	return NJSE_RETURN_OK;
}
