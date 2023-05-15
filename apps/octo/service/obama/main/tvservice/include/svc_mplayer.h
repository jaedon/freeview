/** **********************************************************************************************************
	@file 		svc_mplayer.h

	@date		2011/11/05
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

#ifndef __SVC_MPLAYER_H__
#define __SVC_MPLAYER_H__

#include "htype.h"
#include "stb_params.h"
#include "octo_common.h"

#define SVC_MP_MAX_URL_LEN		(1024*2)


typedef void (*SvcMp_VerifyExternalFunction_t)(HUINT32 ulTunerId, void *userdata, void *verifyhandle, HINT32 *result);

typedef struct tagSvcMp_VerifyExternalCallback
{
	SvcMp_VerifyExternalFunction_t		verifycallback;
	void 								*userdata;
} SvcMp_VerifyExternalCallback_t;

typedef enum {
	eSVC_MP_PLAYTYPE_NONE					= 0,
	eSVC_MP_PLAYTYPE_AUTO,
	eSVC_MP_PLAYTYPE_PROGORESSIVE,
	eSVC_MP_PLAYTYPE_STREAMING,
} SvcMp_PlayType_e;

typedef enum{
	eSVC_MP_URI_NOT_SUPPORTED,
	eSVC_MP_URI_HTTP,
	eSVC_MP_URI_HTTPS,
	eSVC_MP_URI_MMS,
	eSVC_MP_URI_RTP,
	eSVC_MP_URI_VTUNER,
	eSVC_MP_URI_FILE,
	eSVC_MP_URI_DLNA,
} SvcMp_UriType_e;

typedef enum {
	eSVC_MP_STATE_IDLE					= 0,
	eSVC_MP_STATE_SESSION_CREATED,
	eSVC_MP_STATE_PROBE_FINISHED,
	eSVC_MP_STATE_PROBE_FAILED,
	eSVC_MP_STATE_PROBE_CANCELED,
	eSVC_MP_STATE_BUFFER_STARTED,
	eSVC_MP_STATE_SESSION_OPENED,
	eSVC_MP_STATE_SESSION_SETUP,
	eSVC_MP_STATE_SESSION_STARTED,
	eSVC_MP_STATE_PLAYING,
	eSVC_MP_STATE_SEEKING,
	eSVC_MP_STATE_STOPPED,
} SvcMp_MediaState_e;

typedef enum {
	eSVC_MP_EVENT_NONE						= eSEVT_MEDIA_START,
	eSVC_MP_EVENT_CONNECTING,
	eSVC_MP_EVENT_PROBE_FINISHED,
	eSVC_MP_EVENT_PROBE_FAILED,
	eSVC_MP_EVENT_PLAYING,
	eSVC_MP_EVENT_STOPPED,
	eSVC_MP_EVENT_PAUSE,
	eSVC_MP_EVENT_BEGIN_OF_STREAM,
	eSVC_MP_EVENT_END_OF_STREAM,
	eSVC_MP_EVENT_PLAY_POSITION_CHANGED,
	eSVC_MP_EVENT_PLAY_SPEED_CHANGED,
	eSVC_MP_EVENT_VIDEO_NEW_FRAME,
	eSVC_MP_EVENT_VIDEO_UNDERRUN,
	eSVC_MP_EVENT_AUDIO_NEW_FRAME,
	eSVC_MP_EVENT_SBTL_START,
	eSVC_MP_EVENT_SBTL_STOP,
	eSVC_MP_EVENT_SBTL_UPDATE,
	eSVC_MP_EVENT_ERROR,
	eSVC_MP_EVENT_SEEK_ERROR,

	eSVC_MP_EVENT_BUFFERING_CANPLAY,
	eSVC_MP_EVENT_BUFFERING_NOT_ENOUGH,
	eSVC_MP_EVENT_BUFFERING_ENOUGH,
	eSVC_MP_EVENT_BUFFERING_FINISHED,
	eSVC_MP_EVENT_BUFFERING_UPDATED,

	eSVC_MP_EVENT_OUTPUT_CONTROL,

	eSVC_MP_EVENT_PLAY_START,			// // DD player starting
	eSVC_MP_EVENT_PAT_TIMEOUT,
	eSVC_MP_EVENT_PMT_PIDREMOVED,
	eSVC_MP_EVENT_PMT_PIDCHANGED,
	eSVC_MP_EVENT_PMT_CHANGED,
	eSVC_MP_EVENT_PMT_TIMEOUT,
	eSVC_MP_EVENT_SIT_CHANGED,
	eSVC_MP_EVENT_SIT_TIMEOUT,
	eSVC_MP_EVENT_EIT_TIMEOUT,
	eSVC_MP_EVENT_EIT_CHANGED,
} SvcMp_Event_e;

typedef enum {
	eSVC_MP_LOCAL_EVENT_COMMAND			= 0,
	eSVC_MP_LOCAL_EVENT_CALLBACK
} SvcMp_EventType_e;

typedef enum {
	eSVC_MP_LOCAL_COMMAND_NONE			= 0,
	eSVC_MP_LOCAL_COMMAND_PROBE,
	eSVC_MP_LOCAL_COMMAND_PLAY,
	eSVC_MP_LOCAL_COMMAND_STOP,
	eSVC_MP_LOCAL_COMMAND_PEEK,
	eSVC_MP_LOCAL_COMMAND_PEEK_AND_BUFFERING,
	eSVC_MP_LOCAL_COMMAND_SEEK,

	eSVC_MP_LOCAL_COMMAND_SI,
	eSVC_MP_LOCAL_COMMAND_PAT_TABLE,
	eSVC_MP_LOCAL_COMMAND_PMT_TABLE,
	eSVC_MP_LOCAL_COMMAND_SIT_TABLE,
	eSVC_MP_LOCAL_COMMAND_EIT_TABLE,
} SvcMp_Command_e;

typedef enum {
	eSVC_MP_ERROR_UNKNOWN=0,
	eSVC_MP_ERROR_NETWORK_LINK_DOWN=100,
	eSVC_MP_ERROR_NETWORK_DNSLOOKUP_FAIL,
	eSVC_MP_ERROR_NETWORK_CONNECTION_REFUSED,
	eSVC_MP_ERROR_NETWORK_IGMP_JOIN_FAIL,
	eSVC_MP_ERROR_NETWORK_SELECT_TIMEOUT,
	eSVC_MP_ERROR_NETWORK_SEEK_TIMEOUT,

	/* the following http status codes are defined according to the HTTP 1.1 Specification : Pls don't touch !!*/
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_CREATED							=201 , /* Section 10.2.2: Created */

	eSVC_MP_ERROR_SERVER_HTTP_STATUS_MULTIPLE_CHOICE					=300 , /* Section 10.3.1: Multiple Choices */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_UNUSED							=306 , /* Section 10.3.7: Unused field */

	eSVC_MP_ERROR_SERVER_HTTP_STATUS_BAD_REQUEST						=400 , /* 400 , Section 10.4.1: Bad Request */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_UNAUTHORIZED						=401 , /* 401 , Section 10.4.2: Unauthorized */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_PAYMENT_REQUIRED					=402 , /* 402 , Section 10.4.3: Payment Required */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_FORBIDDEN							=403 , /* 403 , Section 10.4.4: Forbidden */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_NOT_FOUND							=404 , /* 404 , Section 10.4.5: Not Found */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED				=405 , /* 405 , Section 10.4.6: Method Not Allowed */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_NOT_ACCEPTABLE					=406 , /* 406 , Section 10.4.7: Not Acceptable */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED				=407 , /* 407 , Section 10.4.8: Proxy Authentication Required */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_REQUEST_TIMEOUT					=408 , /* 408 , Section 10.4.9: Request Time-out */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_CONFILICT							=409 , /* 409 , Section 10.4.10: Conflict */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_GONE								=410 , /* 410 , Section 10.4.11: Gone */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_LENGTH_REQUIRED					=411 , /* 411 , Section 10.4.12: Length Required */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_PRECONDITION_FAILED				=412 , /* 412 , Section 10.4.13: Precondition Failed */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE			=413 , /* 413 , Section 10.4.14: Request Entity Too Large */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE				=414 , /* 414 , Section 10.4.15: Request-URI Too Large */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE			=415 , /* 415 , Section 10.4.16: Unsupported Media Type */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE 	=416 , /* 416 , Section 10.4.17: Requested range not satisfiable */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_EXPECTATION_FAILED				=417 , /* 417 , Section 10.4.18: Expectation Failed */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR				=500 , /* 500 , Section 10.5.1: Internal Server Error */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_NOT_IMPLEMENTED					=501 , /* 501 , Section 10.5.2: Not Implemented */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_BAD_GATEWAY						=502 , /* 502 , Section 10.5.3: Bad Gateway */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE				=503 , /* 503 , Section 10.5.4: Service Unavailable */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT					=504 , /* 504 , Section 10.5.5: Gateway Time-out */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED				=505 , /* 505 , Section 10.5.6: HTTP Version not supported */
	eSVC_MP_ERROR_SERVER_HTTP_STATUS_MHEG_UNKNOWN						=622 , /* Mheg Specific */

	eSVC_MP_ERROR_SERVER_RTSP_ERROR=700,
	eSVC_MP_ERROR_SERVER_MMS_ERROR,
	eSVC_MP_ERROR_SERVER_IIS_ERROR,
	eSVC_MP_ERROR_SYSTEM_NO_MEMORY,
	eSVC_MP_ERROR_IO_READ_FAIL,
	eSVC_MP_ERROR_IO_WRITE_FAIL,
	eSVC_MP_ERROR_IO_SEEK_FAIL,
	eSVC_MP_ERROR_DATA_MALFORMED_MANIFEST,
	eSVC_MP_ERROR_DATA_MALFORMED_MP4,
	eSVC_MP_ERROR_DATA_MALFORMED_TS,
	eSVC_MP_ERROR_DATA_NO_INDEX_FILE,
} SvcMp_Error_e;

typedef struct _SvcMp_VideoTrack_t {
	DxVideoCodec_e		eVideoCodec;
	HUINT32				ulVideoPesId;
	HUINT32				ulVideoStreamId;
} SvcMp_VideoTrack_t;

typedef struct _SvcMp_AudioTrack_t {
	DxAudioCodec_e		eAudioCodec;
	HUINT32				ulAudioPesId;
	HUINT32				ulAudioStreamId;
	HINT8				szAudioLanguage[64];
} SvcMp_AudioTrack_t;

typedef struct _SvcMp_VideoWindow_t {
	HINT32			width;
	HINT32			height;
} SvcMp_VideoWindow_t;

typedef struct _SvcMp_BufferInfo_t {
	HUINT32 		ulChunks;
	HUINT32 		*ullStartTime;
	HUINT32 		*ullEndTime;
	HUINT32			ulBufferRemainedTime;
	HUINT32			ulBufferPercent;
} SvcMp_BufferInfo_t;

typedef struct _SvcMp_OutputControl_t {
	HBOOL		bHdcp;
	HBOOL		bScms;
	HBOOL		bCgms;
	HBOOL		bMacrovision;
} SvcMp_OutputControl_t;

typedef struct _SvcMp_MediaPlayer_t {
	void			*phyMediaHandle;
	HUINT32			hActionId;
	HINT32			nSpeed;
	HINT8			szUrl[SVC_MP_MAX_URL_LEN];
	HINT8			*userAgent;
	HUINT64			ullStartTime;

	SvcMp_MediaState_e				eState;
	SvcMp_PlayType_e		ePlayType;
	SvcMp_UriType_e		eUriType;

	HBOOL			bUseSubtitle;
	HBOOL			bhasSubtitle;
	HUINT32			ulSubtitleFontSize;
	HUINT32			ulSubtitleDisplayPosition;

	HBOOL			bBufferingAutoMode;
	HINT32			nBufferingThreshold;

	struct _SvcMp_MediaPlayer_t		*next;
} SvcMp_MediaPlayer_t;

typedef struct _SvcMp_Config_t {
	HBOOL			bProgressiveDownload;
	HINT8			szPDLPath[SVC_MP_MAX_URL_LEN];	/* progressive download buffer path */
} SvcMp_Config_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Initiailize MediaPlayer
 *
 *	param[in] ptConfig	the configuration for media player initialization.
 *
 *  @return ERR_OK if success
 */
HERROR SVC_MP_Init(const SvcMp_Config_t *ptConfig, PostMsgToMgrCb_t fnPostMsgToMgr);

/**
 *	Uninitiailize MediaPlayer
 *
 */
HERROR SVC_MP_UnInit(void);

/**
 *	Get default configuration of media player
 *
 *	param[out] ptConfig	the buffer will be fiiled the default configuration
 *	for media player initialization.
 *
 */
void SVC_MP_GetDefaultConfig(SvcMp_Config_t *ptConfig);

/**
 *	Create a media player instance
 *
 *	@param[in] hActionId the action ID of OCTO
 *
 *	@return the instance handle of created media player, or NULL if failure.
 */
SvcMp_MediaPlayer_t *SVC_MP_CreatePlayer(HUINT32 hActionId);

/**
 *	Destroy a media player instance
 *
 *	@param[in] player the media player instance
 */
HERROR SVC_MP_DestroyPlayer(SvcMp_MediaPlayer_t *player);

/**
 *	Get the action ID which being used by media plyaer
 *
 *	@param[in] player the media player instance
 *
 *	@return the action ID
 */
HUINT32 SVC_MP_GetActionId(SvcMp_MediaPlayer_t *player);

/**
 *	Get the instance of the media player which being played in system.
 *
 *	@param[in] hActionId the action ID of OCTO
 *
 *	@return the instance of media player, or NULL if there is no active player.
 */
SvcMp_MediaPlayer_t *SVC_MP_GetActivePlayer(HUINT32 hActionId);

/**
 *	Plays the media referenced by uri at a default speed.
 *
 *	@param[in] player the media player instance
 *	@param[in] ePlayType the download type of content when the content needs streaming protocol
 *	@param[in] szUrl the content URL
 *	@param[in] ulStartTime the start position when the playback is started. (msec)
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_Play(SvcMp_MediaPlayer_t *player, HUINT32 ulStartTime);

/**
 *	stop the playback of the current media referenced by URL.
 *
 *	@param[in] player the media player instance
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_Stop(SvcMp_MediaPlayer_t *player);

/**
 *	Sets the current play position in milliseconds to the value of ullPosition_ms.
 *
 *	@param[in] player the media player instance
 *	@param[in] ullPosition_ms the play position in milliseconds
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_Seek(SvcMp_MediaPlayer_t *player, HUINT64 ullPosition_ms);

/**
 *
 *
 */
HERROR SVC_MP_Probe(SvcMp_MediaPlayer_t *player, SvcMp_PlayType_e ePlayType,
							const HINT8 *url);

/**
 *	Getting a meta data for the content
 *
 *	@param[in] player the media player instance
 *	@param[in] ePlayType the download type of content when the content needs streaming protocol
 *	@param[in] szUrl the content URL
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_Peek(SvcMp_MediaPlayer_t *player, SvcMp_PlayType_e ePlayType,
							const HINT8 *szUrl);

#if 0
/**
 *	Getting a meta data for the content and starts the buffering.
 *	This functions is only for HTML5 VideoBackend of Opera browser.
 *
 *	@param[in] player the media player instance
 *	@param[in] ePlayType the download type of content when the content needs streaming protocol
 *	@param[in] szUrl the content URL
 *
 *	@return ERR_OK if success.
 */
HERROR eSVC_MP_PeekAndBuffering(SvcMp_MediaPlayer_t *player, SvcMp_PlayType_e ePlayType,
                            const HINT8 *szUrl);
#endif
/**
 *	Get the URL of current media content
 *
 *	@param[in] player the media player instance
 *	@param[out] pszUrlBuf the buffer which will be filled by URL data.
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetPlayUrl(SvcMp_MediaPlayer_t *player, HINT8 *pszUrlBuf);

/**
 *	Set the URL of current media content
 *
 *	@param[in] player the media player instance
 *	@param[in] pszUrlBuf the URL of media content
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_SetPlayUrl(SvcMp_MediaPlayer_t *player, const HINT8 *szUrl);

/**
 *	Get the download type if this playback is using the streaming protocol.
 *
 *	@param[in] player the media player instance
 *	@param[out] pePlayType the download type
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetPlayType(SvcMp_MediaPlayer_t *player, SvcMp_PlayType_e *pePlayType);

/**
 *	Get the play position in milliseconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[out] pePlayType the download type
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetPlayPosition(SvcMp_MediaPlayer_t *player, HUINT64 *pullPosition_ms);

/**
 *	Sets the current play position in milliseconds to the value of ullPosition_ms.
 *
 *	@param[in] player the media player instance
 *	@param[in] ullPosition_ms the play position in milliseconds
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_SetPlayPosition(SvcMp_MediaPlayer_t *player, HUINT64 ullPosition_ms);

/**
 *	Get the play speed in seconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[out] pnSpeed_ms the play speed
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetPlaySpeed(SvcMp_MediaPlayer_t *player, HINT32 *pnSpeed_ms);

/**
 *	Get the play supported speed array
 *
 *	@param[in] player the media player instance
 *	@param[out] pulSpeedNum speed array count
  *	@param[out] ppnSpeedArray speed array
 *
 *	@return ERR_OK if success.
 */

HERROR SVC_MP_GetSupportedSpeeds(SvcMp_MediaPlayer_t *player, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray);


/**
 *	Set the play speed in seconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[in] nSpeed_ms the play speed
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_SetPlaySpeed(SvcMp_MediaPlayer_t *player, HINT32 nSpeed_ms);

/**
 *	Get the full-screen mode
 *
 *	@param[in] player the media player instance
 *	@param[out] pbFullScreen the boolean value by full-screen mode.
 *	(true if the current mode is full-screen)
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetFullScreenMode(SvcMp_MediaPlayer_t *player, HBOOL *pbFullScreen);

/**
 *	Set the full-screen mode
 *
 *	@param[in] player the media player instance
 *	@param[in] bFullScreen the boolean value by full-screen mode.
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_SetFullScreenMode(SvcMp_MediaPlayer_t *player, HBOOL bFullScreen);

/**
 *	Get the total duration in milliseconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[out] pullDuration_ms the buffer for the total duration of media content
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetDuration(SvcMp_MediaPlayer_t *player, HUINT64 *pullDuration_ms);

/**
 *	Get the play state of media content. (Indication of the current play state)
 *
 *	@param[in] player the media player instance
 *	@param[out] peEvent the buffer for the play state of media content
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetPlayState(SvcMp_MediaPlayer_t *player, SvcMp_Event_e *peEvent);

/**
 *	Get DRM mode of current media content
 *
 *	@param[in] player the media player instance
 *	@param[out] pbDrm true of the media content includes a DRM data.
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_MP_GetDrmStatus(SvcMp_MediaPlayer_t *player, HBOOL *pbDrm);

/**
 *	Get a video track count
 *
 *	@param[in] player the media player instance
 *
 *	@return the video track count
 */
HINT32 SVC_MP_GetVideoTrackCount(SvcMp_MediaPlayer_t *player);

/**
 *	Get a video track information
 *
 *	@param[in] player the media player instance
 *	@param[in] nIndex the index of video track
 *	@param[out] pVideoTrack the information of video track at @nIndex
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_GetVideoTrack(SvcMp_MediaPlayer_t *player, HINT32 nIndex,
					SvcMp_VideoTrack_t *pVideoTrack);

/**
 *	Get the active video track information
 *
 *	@param[in] player the media player instance
 *	@param[out] pVideoTrack the information of active video track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_GetActiveVideoTrack(SvcMp_MediaPlayer_t *player,
					SvcMp_VideoTrack_t *pVideoTrack);

/**
 *	Set the active video track
 *
 *	@param[in] player the media player instance
 *	@param[in] pVideoTrack the information of video track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_SetActiveVideoTrack(SvcMp_MediaPlayer_t *player,
					const SvcMp_VideoTrack_t *pVideoTrack);

/**
 *	Get an audio track count
 *
 *	@param[in] player the media player instance
 *
 *	@return the audio track count
 */
HINT32 SVC_MP_GetAudioTrackCount(SvcMp_MediaPlayer_t *player);

/**
 *	Get an audio track information
 *
 *	@param[in] player the media player instance
 *	@param[in] nIndex the index of audio track
 *	@param[out] pAudioTrack the information of audio track at @nIndex
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_GetAudioTrack(SvcMp_MediaPlayer_t *player, HINT32 nIndex,
					SvcMp_AudioTrack_t *pAudioTrack);

/**
 *	Get the active audio track information
 *
 *	@param[in] player the media player instance
 *	@param[out] pAudioTrack the information of active audio track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_GetActiveAudioTrack(SvcMp_MediaPlayer_t *player,
					SvcMp_AudioTrack_t *pAudioTrack);

/**
 *	Set the active audio track
 *
 *	@param[in] player the media player instance
 *	@param[in] pAudioTrack the information of audio track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_SetActiveAudioTrack(SvcMp_MediaPlayer_t *player,
					const SvcMp_AudioTrack_t *pAudioTrack);

HERROR SVC_MP_GetAudioStereoConfig(SvcMp_MediaPlayer_t *player, DxStereoSelect_e *eStereoConfig);
HERROR SVC_MP_SetAudioStereoConfig(SvcMp_MediaPlayer_t *player, DxStereoSelect_e eStereoConfig);
HERROR SVC_MP_GetStereoType(SvcMp_MediaPlayer_t *player, DxStereoFormat_e *peStereoType);

/**
 *	Get the stream type of current media content
 *
 *	@param[in] player the media player instance
 *	@param[out] peStreamType the stream type of media content
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_GetStreamType(SvcMp_MediaPlayer_t *player, eMEDIA_StreamType_t *peStreamType);

/**
 *	Get the window size
 *	x: the start position of the abscissa
 *	y: the start position of the ordinate
 *	width: the width of the screen
 *	height: the height of the screen
 *
 *	@param[in] player the media player instance
 *	@param[out] pWindow the information of window
 *
 *
 *	@return ERR_OK if success
 */
HERROR SVC_MP_GetVideoWindow(SvcMp_MediaPlayer_t *player, SvcMp_VideoWindow_t *pWindow);

HERROR SVC_MP_GetBufferingProgress(SvcMp_MediaPlayer_t *player, HUINT32 *pulProgress);

HERROR SVC_MP_GetBufferingChunkInfo(SvcMp_MediaPlayer_t *player,
							SvcMp_BufferInfo_t *ptChunkInfo);

HERROR SVC_MP_SetBufferingScenario(SvcMp_MediaPlayer_t *player, HBOOL bAuto, HINT32 nThreshold);

HERROR SVC_MP_GetWebUserAgent(SvcMp_MediaPlayer_t *player, HINT8 *pszUserAgent);

HERROR SVC_MP_SetWebUserAgent(SvcMp_MediaPlayer_t *player, const HINT8 *szUserAgent);

HERROR SVC_MP_CaptureScreen(SvcMp_MediaPlayer_t *player, const HINT8 *szSavePath);

HINT32 SVC_MP_IsSubtitle(SvcMp_MediaPlayer_t *player);

HERROR SVC_MP_GetSubtitleDisplay(SvcMp_MediaPlayer_t *player, HBOOL *pbShow);

HERROR SVC_MP_GetSubtitleTotalLangCnt(SvcMp_MediaPlayer_t *player, HUINT32 *pnCount);

HERROR SVC_MP_GetSubtitleCurrLangIndex(SvcMp_MediaPlayer_t *player, HUINT32 *pnIndex);

HERROR SVC_MP_GetSubtitleLanguageByIndex(SvcMp_MediaPlayer_t *player, HINT32 nIndex, HINT8 *szLang);

HERROR SVC_MP_GetSubtitleCurSyncTime(SvcMp_MediaPlayer_t *player, HINT32 *ulMsTime);

HERROR SVC_MP_SetSubtitleDisplay(SvcMp_MediaPlayer_t *player, HBOOL bEnable);

HERROR SVC_MP_SetSubtitleLanguage(SvcMp_MediaPlayer_t *player, HUINT32 ulLangIdx);

HERROR SVC_MP_ChangeSubtitleSyncTime(SvcMp_MediaPlayer_t *player, HINT32 ulMsTime);

HERROR SVC_MP_SetSubtitleFontSize(SvcMp_MediaPlayer_t *player, HUINT32 ulSize);

HERROR SVC_MP_GetSubtitleFontSize(SvcMp_MediaPlayer_t *player, HUINT32 *ulSize);

HERROR SVC_MP_SetSubtitleDisplayPosition(SvcMp_MediaPlayer_t *player, HUINT32 ulPos);

HERROR SVC_MP_GetSubtitleDisplayPosition(SvcMp_MediaPlayer_t *player, HUINT32 *ulPos);

HERROR SVC_MP_SetBrowserUserAgentData(HCHAR *szString);

HERROR SVC_MP_SetDrmLicenseData(SvcMp_MediaPlayer_t *player, HCHAR *szString);

HERROR SVC_MP_GetOutputControlInfo(SvcMp_MediaPlayer_t *player, SvcMp_OutputControl_t *control);

SvcMp_Error_e SVC_MP_GetLastErrorReason(SvcMp_MediaPlayer_t *player);

HERROR SVC_MP_SetTLS(SvcMp_MediaPlayer_t *player, SvcMp_VerifyExternalCallback_t *VerifyExternalCb);

void SVC_MP_SendCommand(SvcMp_MediaPlayer_t *player, SvcMp_Command_e command, HUINT32 p1);

HERROR SVC_MP_SetAudioDualMonoConfig(SvcMp_MediaPlayer_t *mplayer, DxDualMonoSelect_e eDualMonoConfig);

HERROR SVC_MP_GetAudioDualMonoConfig(SvcMp_MediaPlayer_t *mplayer, DxDualMonoSelect_e *eDualMonoConfig);

HERROR SVC_MP_UpdateUserSelectAudioTrackInfo(SvcMp_MediaPlayer_t *mplayer, HINT32 nIndex);

#ifdef __cplusplus
};
#endif


#endif	/* __SVC_MPLAYER_H__ */
