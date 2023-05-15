/** **********************************************************************************************************
	@file 		NativePlayer.h

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

#ifndef _NATIVE_PLAYER_H_
#define _NATIVE_PLAYER_H_

#include "NativeNJSETypes.h"
#include "NativeStreamTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void* PlayerHandle;
typedef void* FragmentHandle;

typedef enum {
	PLAYER_EVENT_CONNECTION_ATTEMPTED = 0,
	PLAYER_EVENT_CONNECTION_ESTABLISHED = 1,
	PLAYER_EVENT_METADATA = 2,
	PLAYER_EVENT_STREAM_STARTED_DOWNLOADING = 3,
	PLAYER_EVENT_STREAM_STOPPED_DOWNLOADING = 4,
	PLAYER_EVENT_STREAM_BECAME_ACTIVE = 5,
	PLAYER_EVENT_BUFFER_STARTED = 6,
	PLAYER_EVENT_BUFFER_COMPLETED = 7,
	PLAYER_EVENT_FRAGMENTS_DOWNLOADED = 8,
	PLAYER_EVENT_READY = 9,
	PLAYER_EVENT_STREAM_SWITCH_STARTED = 10,
	PLAYER_EVENT_STREAM_SWITCH_COMPLETED = 11,
	PLAYER_EVENT_PLAYING = 12,
	PLAYER_EVENT_PAUSED = 13, 
	PLAYER_EVENT_SEEK_STARTED = 14, 
	PLAYER_EVENT_SEEK_COMPLETED = 15, 
	PLAYER_EVENT_PLAYBACK_SPEED_CHANGED = 16, 
	PLAYER_EVENT_VOLUME_CHANGED = 17,
	PLAYER_EVENT_STOPPED = 18,
	PLAYER_EVENT_ENDED = 19,
	PLAYER_EVENT_UNLOADED = 20
} PlayerEvent;

typedef struct {
	int fragmentIndex;
	int fragmentSize;
	int downloadDuration;
	int retryAttemps;
	int serverIp;
	StreamTypesHandle stream;
}fragment_data;

typedef union {
	struct {
		StreamTypesHandle stream;
	} player_stream_started_downloading;

	struct {
		StreamTypesHandle stream;
	} player_stream_stopped_downloading;

	struct {
		StreamTypesHandle stream;
	} player_stream_became_active;

	struct {
		char * type;
		StreamTypesHandle stream;
	} player_buffer_started;

	struct {
		char * type;
		StreamTypesHandle stream;
	} player_buffer_completed;

	struct {
		NJSENumber targetStream;
	}player_stream_switch_started;

	struct {
		NJSENumber newStream;
		NJSENumber streamBitrate;
	}player_stream_switch_completed;

	struct {
		NJSENumber targetTime;
	}player_seek_started;	

	struct {
		NJSENumber currentTime;
	}player_seek_completed;	

	struct {
		NJSENumber speed;
	}player_playback_speed_changed; 

	struct {
		NJSENumber speed; // Hawaii Spec is misspelled - volume to speed. 
	}player_volume_changed; 

} PlayerEventInfo;


typedef void (PlayerListener)(PlayerEvent event, PlayerEventInfo *info, PlayerHandle id);

NJSEReturnCode native_PlayerCreate(PlayerHandle *id);
NJSEReturnCode native_PlayerReleaseHandle(PlayerHandle i );

NJSEReturnCode native_PlayerSetListener(PlayerListener listener);

NJSEReturnCode native_PlayerSetFullScreen(PlayerHandle id, NJSEBoolean fullscreen);
NJSEReturnCode native_PlayerOnListenerAccessed(PlayerHandle id, const char *eventName);
NJSEReturnCode native_PlayerSetVisibility(PlayerHandle id, NJSEBoolean visibility, NJSEBoolean unloaded);
NJSEReturnCode native_PlayerSetVideoWindow(PlayerHandle id, int x, int y, int width, int height);

NJSEReturnCode native_PlayerGetAvailableStreams(int *count, StreamTypesHandle **handles);
NJSEReturnCode native_PlayerGetActiveStreams(int *count, StreamTypesHandle **handles);
NJSEReturnCode native_PlayerGetDownloadingStreams(int *count, StreamTypesHandle **handles);
NJSEReturnCode native_PlayerGetCurrentTime(NJSENumber *currentTime);
NJSEReturnCode native_PlayerGetCurrentBufferSize(NJSENumber *currentBufferSize);
NJSEReturnCode native_PlayerGetCurrentBufferTime(NJSENumber *currentBufferTime);
NJSEReturnCode native_PlayerGetMaxBufferSize(NJSENumber *maxBufferSize);
NJSEReturnCode native_PlayerGetDroppedFrames(NJSENumber *droppedFrames);
NJSEReturnCode native_PlayerGetAverageFrameRate(NJSENumber *averageFrameRate);
NJSEReturnCode native_PlayerGetAverageBandwidth(NJSENumber *averageBandwidth);

NJSEReturnCode native_PlayerGetFragments(int *count, FragmentHandle **handles);
NJSEReturnCode native_PlayerGetFragmentIndex(FragmentHandle i, NJSENumber *index);
NJSEReturnCode native_PlayerGetFragmentSize(FragmentHandle i, NJSENumber *size);
NJSEReturnCode native_PlayerGetFragmentDownloadDuration(FragmentHandle i, NJSENumber *duration);
NJSEReturnCode native_PlayerGetFragmentRetryAttempts(FragmentHandle i, NJSENumber *retry);
NJSEReturnCode native_PlayerGetFragmentServerIp(FragmentHandle i, NJSENumber *serverIp);
NJSEReturnCode native_PlayerGetFragmentStream(FragmentHandle i, StreamTypesHandle *handle);

NJSEReturnCode native_PlayerSetAssetUrl(PlayerHandle id, const char *url, NJSEBoolean *ret);
NJSEReturnCode native_PlayerGetAssetUrl(PlayerHandle id, char **url);
NJSEReturnCode native_PlayerConfigureDrm(PlayerHandle id, void * drmConfig, NJSEBoolean *ret);
NJSEReturnCode native_PlayerConfigureAbr(PlayerHandle id, void * drmConfig, NJSEBoolean *ret);
NJSEReturnCode native_PlayerLoadMetadata(PlayerHandle id, NJSEBoolean *ret);
NJSEReturnCode native_PlayerGetPalyReadyChallenge(PlayerHandle id, char **ret);
NJSEReturnCode native_PlayerSetPlayReadyLicense(PlayerHandle id, const char *license, NJSEBoolean *ret);
NJSEReturnCode native_PlayerPlay(PlayerHandle id, NJSEBoolean *ret);
NJSEReturnCode native_PlayerPause(PlayerHandle id, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSeek(PlayerHandle id, NJSENumber time, NJSEBoolean *ret);
NJSEReturnCode native_PlayerStop(PlayerHandle, NJSEBoolean *ret);
NJSEReturnCode native_PlayerUnload(PlayerHandle, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSetPlaybackSpeed(PlayerHandle id, NJSENumber speed, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSetBufferTimeRequiredForInitialPlayback(PlayerHandle id, NJSENumber time, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSetBufferTimeRequiredAfterExhaustion(PlayerHandle id, NJSENumber time, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSetBufferTimeRequiredAfterStreamSwitch(PlayerHandle id, NJSENumber time, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSetBufferTimeRequiredAfterSeeking(PlayerHandle id, NJSENumber time, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSetFragmentsDownloadedEventFrequency(PlayerHandle id, NJSENumber frequency, NJSEBoolean *ret);
NJSEReturnCode native_PlayerSetActiveStream(PlayerHandle id, const char *type, StreamTypesHandle stream, NJSEBoolean *ret);
NJSEReturnCode native_PlayerGetPosition(PlayerHandle id, NJSENumber *x, NJSENumber *y);
NJSEReturnCode native_PlayerSetPosition(PlayerHandle id, NJSENumber x, NJSENumber y, NJSEBoolean *ret);
NJSEReturnCode native_PlayerGetSize(PlayerHandle id, NJSENumber *width, NJSENumber *height);
NJSEReturnCode native_PlayerSetSize(PlayerHandle id, NJSENumber width, NJSENumber height, NJSEBoolean *ret);
NJSEReturnCode native_PlayerGetVolume(PlayerHandle id, NJSENumber *volume);
NJSEReturnCode native_PlayerSetVolume(PlayerHandle id, NJSENumber volume, NJSEBoolean *ret);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_NATIVE_PLAYER_H_


