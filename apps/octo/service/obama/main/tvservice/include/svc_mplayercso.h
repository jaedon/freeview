/** **********************************************************************************************************
	@file 		svc_mplayer_cso.h

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

#ifndef __SVC_MPLAYERCSO_H__
#define __SVC_MPLAYERCSO_H__

#include "htype.h"
#include "stb_params.h"
#include "octo_common.h"

#define SVC_CSO_MAX_URL_LEN		(1024*2)


typedef void (*SvcCso_VerifyExternalCallback)(HUINT32 ulTunerId, void *userdata, void *verifyhandle, HINT32 *result);

typedef struct tagSvcCso_VerifyExternalCallback
{
	SvcCso_VerifyExternalCallback		verifycallback;
	void 								*userdata;
} SvcCso_VerifyExternalCallback_t;

/* MEDIA Layer EVENT */
typedef enum
{
	eSVC_CSO_EVT_VIDEO_NEW_FRAME,
	eSVC_CSO_EVT_VIDEO_UNDERRUN,
	eSVC_CSO_EVT_AUDIO_NEW_FRAME,
	eSVC_CSO_EVT_AUDIO_UNDERRUN,
	eSVC_CSO_EVT_AUDIO_FORMAT_CHANGED,
	eSVC_CSO_EVT_AUDIO_DECODER_VOLUME_CHANGED,
	eSVC_CSO_EVT_MEDIA_CHANGEPLAYTIME,
	eSVC_CSO_EVT_MEDIA_BUFFERING_UPDATED,
	eSVC_CSO_EVT_MEDIA_PAUSE,
	eSVC_CSO_EVT_MEDIA_STOPPED,
	eSVC_CSO_EVT_BEGIN_OF_STREAM,
	eSVC_CSO_EVT_END_OF_STREAM,
	eSVC_CSO_EVT_MEDIA_SUB_START,
	eSVC_CSO_EVT_MEDIA_SUB_UPDATE,
	eSVC_CSO_EVT_MEDIA_SUB_STOP,
	eSVC_CSO_EVT_ERROR,
	eSVC_CSO_EVT_MAX,
} SvcCso_Local_Event_e;

typedef enum {
	eSVC_CSO_PLAYTYPE_NONE					= 0,
	eSVC_CSO_PLAYTYPE_AUTO,
	eSVC_CSO_PLAYTYPE_PROGORESSIVE,
	eSVC_CSO_PLAYTYPE_STREAMING,
} SvcCso_PlayType_e;

typedef enum{
	eSVC_CSO_URI_NOT_SUPPORTED,
	eSVC_CSO_URI_HTTP,
	eSVC_CSO_URI_HTTPS,
	eSVC_CSO_URI_MMS,
	eSVC_CSO_URI_RTP,
	eSVC_CSO_URI_VTUNER,
	eSVC_CSO_URI_FILE,
	eSVC_CSO_URI_DLNA,
} SvcCso_UriType_e;

typedef enum {
	eSVC_CSO_STATE_PLAYING=0,
	eSVC_CSO_STATE_STOPPED,
} SvcCso_State_e;

typedef enum {
	eSVC_CSO_EVENT_NONE						= eSEVT_MEDIA_START,
	eSVC_CSO_EVENT_CONNECTING,
	eSVC_CSO_EVENT_PROBE_FINISHED,
	eSVC_CSO_EVENT_PROBE_FAILED,
	eSVC_CSO_EVENT_PLAYING,
	eSVC_CSO_EVENT_STOPPED,
	eSVC_CSO_EVENT_PAUSE,
	eSVC_CSO_EVENT_BEGIN_OF_STREAM,
	eSVC_CSO_EVENT_END_OF_STREAM,
	eSVC_CSO_EVENT_PLAY_POSITION_CHANGED,
	eSVC_CSO_EVENT_PLAY_SPEED_CHANGED,
	eSVC_CSO_EVENT_VIDEO_NEW_FRAME,
	eSVC_CSO_EVENT_VIDEO_UNDERRUN,
	eSVC_CSO_EVENT_AUDIO_NEW_FRAME,
	eSVC_CSO_EVENT_AUDIO_UNDERRUN,
	eSVC_CSO_EVENT_AUDIO_DECODER_VOLUME_CHANGED,
	eSVC_CSO_EVENT_SBTL_START,
	eSVC_CSO_EVENT_SBTL_STOP,
	eSVC_CSO_EVENT_SBTL_UPDATE,
	eSVC_CSO_EVENT_ERROR,
	eSVC_CSO_EVENT_SEEK_ERROR,

	eSVC_CSO_EVENT_BUFFERING_CANPLAY,
	eSVC_CSO_EVENT_BUFFERING_NOT_ENOUGH,
	eSVC_CSO_EVENT_BUFFERING_ENOUGH,
	eSVC_CSO_EVENT_BUFFERING_FINISHED,
	eSVC_CSO_EVENT_BUFFERING_UPDATED,

	eSVC_CSO_EVENT_OUTPUT_CONTROL,

	eSVC_CSO_EVENT_PLAY_START,			// // DD player starting
	eSVC_CSO_EVENT_PAT_TIMEOUT,
	eSVC_CSO_EVENT_PMT_PIDREMOVED,
	eSVC_CSO_EVENT_PMT_PIDCHANGED,
	eSVC_CSO_EVENT_PMT_CHANGED,
	eSVC_CSO_EVENT_PMT_TIMEOUT,
	eSVC_CSO_EVENT_SIT_CHANGED,
	eSVC_CSO_EVENT_SIT_TIMEOUT,
	eSVC_CSO_EVENT_EIT_TIMEOUT,
	eSVC_CSO_EVENT_EIT_CHANGED,
} SvcCso_Event_e;

typedef enum {
	eSVC_CSO_LOCAL_EVENT_COMMAND			= 0,
	eSVC_CSO_LOCAL_EVENT_CALLBACK
} SvcCso_EventType_e;

typedef enum {
	eSVC_CSO_LOCAL_COMMAND_NONE			= 0,
	eSVC_CSO_LOCAL_COMMAND_PLAY,
	eSVC_CSO_LOCAL_COMMAND_STOP,
	eSVC_CSO_LOCAL_COMMAND_SEEK,
} SvcCso_Command_e;

typedef enum {
	eSVC_CSO_ERROR_UNKNOWN=0,
	eSVC_CSO_ERROR_NETWORK_LINK_DOWN=100,
	eSVC_CSO_ERROR_NETWORK_DNSLOOKUP_FAIL,
	eSVC_CSO_ERROR_NETWORK_CONNECTION_REFUSED,
	eSVC_CSO_ERROR_NETWORK_IGMP_JOIN_FAIL,
	eSVC_CSO_ERROR_NETWORK_SELECT_TIMEOUT,
	eSVC_CSO_ERROR_NETWORK_SEEK_TIMEOUT,

	/* the following http status codes are defined according to the HTTP 1.1 Specification : Pls don't touch !!*/
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_CREATED							=201 , /* Section 10.2.2: Created */

	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_MULTIPLE_CHOICE					=300 , /* Section 10.3.1: Multiple Choices */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_UNUSED							=306 , /* Section 10.3.7: Unused field */

	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_BAD_REQUEST						=400 , /* 400 , Section 10.4.1: Bad Request */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_UNAUTHORIZED						=401 , /* 401 , Section 10.4.2: Unauthorized */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_PAYMENT_REQUIRED					=402 , /* 402 , Section 10.4.3: Payment Required */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_FORBIDDEN							=403 , /* 403 , Section 10.4.4: Forbidden */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_NOT_FOUND							=404 , /* 404 , Section 10.4.5: Not Found */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED				=405 , /* 405 , Section 10.4.6: Method Not Allowed */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_NOT_ACCEPTABLE					=406 , /* 406 , Section 10.4.7: Not Acceptable */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED				=407 , /* 407 , Section 10.4.8: Proxy Authentication Required */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_REQUEST_TIMEOUT					=408 , /* 408 , Section 10.4.9: Request Time-out */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_CONFILICT							=409 , /* 409 , Section 10.4.10: Conflict */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_GONE								=410 , /* 410 , Section 10.4.11: Gone */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_LENGTH_REQUIRED					=411 , /* 411 , Section 10.4.12: Length Required */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_PRECONDITION_FAILED				=412 , /* 412 , Section 10.4.13: Precondition Failed */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE			=413 , /* 413 , Section 10.4.14: Request Entity Too Large */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE				=414 , /* 414 , Section 10.4.15: Request-URI Too Large */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE			=415 , /* 415 , Section 10.4.16: Unsupported Media Type */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE 	=416 , /* 416 , Section 10.4.17: Requested range not satisfiable */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_EXPECTATION_FAILED				=417 , /* 417 , Section 10.4.18: Expectation Failed */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR				=500 , /* 500 , Section 10.5.1: Internal Server Error */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_NOT_IMPLEMENTED					=501 , /* 501 , Section 10.5.2: Not Implemented */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_BAD_GATEWAY						=502 , /* 502 , Section 10.5.3: Bad Gateway */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE				=503 , /* 503 , Section 10.5.4: Service Unavailable */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT					=504 , /* 504 , Section 10.5.5: Gateway Time-out */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED				=505 , /* 505 , Section 10.5.6: HTTP Version not supported */
	eSVC_CSO_ERROR_SERVER_HTTP_STATUS_MHEG_UNKNOWN						=622 , /* Mheg Specific */

	eSVC_CSO_ERROR_SERVER_RTSP_ERROR=700,
	eSVC_CSO_ERROR_SERVER_MMS_ERROR,
	eSVC_CSO_ERROR_SERVER_IIS_ERROR,
	eSVC_CSO_ERROR_SYSTEM_NO_MEMORY,
	eSVC_CSO_ERROR_IO_READ_FAIL,
	eSVC_CSO_ERROR_IO_WRITE_FAIL,
	eSVC_CSO_ERROR_IO_SEEK_FAIL,
	eSVC_CSO_ERROR_DATA_MALFORMED_MANIFEST,
	eSVC_CSO_ERROR_DATA_MALFORMED_MP4,
	eSVC_CSO_ERROR_DATA_MALFORMED_TS,
	eSVC_CSO_ERROR_DATA_NO_INDEX_FILE,
} SvcCso_Error_e;

typedef enum
{
	eSVC_CSO_PUMPTYPE_AV = 0,
	eSVC_CSO_PUMPTYPE_A,
	eSVC_CSO_PUMPTYPE_V,
	eSVC_CSO_PUMPTYPE_ENDOFCASE
}SvcCso_PumpType_e;

typedef enum
{
	eSVC_CSO_INPUT_STREAMTYPE_TS_MUX = 0,
	eSVC_CSO_INPUT_STREAMTYPE_ES_DIRECT,
	eSVC_CSO_INPUT_STREAMTYPE_TS_LIVE
}SvcCso_InputStreamType_e;

typedef enum
{
	eSVC_CSO_EVT_AUDIOFADE_NONE = 0,
	eSVC_CSO_EVT_AUDIOFADE_SET_FADE,
	eSVC_CSO_EVT_AUDIOFADE_VIDEO_UNDERRUN,
	eSVC_CSO_EVT_AUDIOFADE_AUDIO_UNDERRUN,
	eSVC_CSO_EVT_AUDIOFADE_VIDEO_NEW_FRAME,
	eSVC_CSO_EVT_AUDIOFADE_AUDIO_NEW_FRAME,
}SvcCso_Event_AudioFade_e;

typedef enum
{
	eSVC_CSO_STATUS_AUDIOFADE_STOPPED = 0,
	eSVC_CSO_STATUS_AUDIOFADE_RUNNING,
	eSVC_CSO_STATUS_AUDIOFADE_TERMINATE,
}SvcCso_Status_AudioFade_e;

typedef enum
{
	eSVC_CSO_EASE_AUDIOFADE_EASELINEAR = 0,
	eSVC_CSO_EASE_AUDIOFADE_EASEINCUBIC,
	eSVC_CSO_EASE_AUDIOFADE_EASEOUTCUBIC,
	eSVC_CSO_EASE_AUDIOFADE_EASECOUNT
}SvcCso_Ease_AudioFade_e;

typedef struct _SvcCso_VideoTrack_t {
	DxVideoCodec_e		eVideoCodec;
	HUINT32				ulVideoPesId;
	HUINT32				ulVideoStreamId;
} SvcCso_VideoTrack_t;

typedef struct _SvcCso_AudioTrack_t {
	DxAudioCodec_e		eAudioCodec;
	HUINT32				ulAudioPesId;
	HUINT32				ulAudioStreamId;
	HINT8				szAudioLanguage[64];
} SvcCso_AudioTrack_t;

typedef struct _SvcCso_VideoWindow_t {
	HINT32			width;
	HINT32			height;
} SvcCso_VideoWindow_t;

typedef struct _SvcCso_BufferInfo_t {
	HUINT32 		ulChunks;
	HUINT32 		*ullStartTime;
	HUINT32 		*ullEndTime;
	HUINT32			ulBufferRemainedTime;
	HUINT32			ulBufferPercent;
} SvcCso_BufferInfo_t;

typedef struct _SvcCso_OutputControl_t {
	HBOOL		bHdcp;
	HBOOL		bScms;
	HBOOL		bCgms;
	HBOOL		bMacrovision;
} SvcCso_OutputControl_t;

typedef struct _SvcCso_t {
	HUINT32			hActionId;
	HINT32			nSpeed;
	HINT8			*pszUrl;
	HUINT64			ullStartTime;
	HUINT32 		ulVideoCodecId;
	HUINT32 		ulAudioCodecId;
	HUINT32 		ulVideoStreamId;
	HUINT32 		ulAudioStreamId;

	HUINT32 		nIdxVideoDecoder;
	HUINT32 		nIdxAudioDecoder;

	SvcCso_State_e			eState;
	SvcCso_PlayType_e		ePlayType;
	SvcCso_UriType_e		eUriType;

	HBOOL			bUseSubtitle;
	HBOOL			bhasSubtitle;
	HUINT32			ulSubtitleFontSize;
	HUINT32			ulSubtitleDisplayPosition;

	HUINT32 ulVideoStreamWidth;
	HUINT32 ulVideoStreamHeight;
	DxAspectRatio_e eVideoAspectRatio;

	HBOOL			bBufferingAutoMode;
	HINT32			nBufferingThreshold;

	HBOOL			bCalypsoPlayerMode;
	void			*hMediaHandle;
	void			*pSubContext;

	HUINT32			ulDemuxId;
	HBOOL			bEnableDemuxPath;

	struct _SvcCso_t		*next;
} SvcCso_t;

typedef struct _SvcCso_Config_t {
	HBOOL			bProgressiveDownload;
	HINT8			szPDLPath[SVC_CSO_MAX_URL_LEN];	/* progressive download buffer path */
} SvcCso_Config_t;

typedef struct _SvcCso_Resource_t {
	HUINT32 *pulPlaypumpList;
	HUINT32 *pulDmaList;
	HUINT32 ulNumOfPlaypump;
	HUINT32 ulNumOfDma;
	HUINT32 ulAudioMixerHandle;
} SvcCso_Resource_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Initiailize Media
 *
 *	param[in] ptConfig	the configuration for media  initialization.
 *
 *  @return ERR_OK if success
 */
HERROR SVC_CSO_Init(PostMsgToMgrCb_t pfncPostMsgCallback);

/**
 *	Uninitiailize Media
 *
 */
HERROR SVC_CSO_UnInit(void);

/**
 *	Get default configuration of media
 *
 *	param[out] ptConfig	the buffer will be fiiled the default configuration
 *	for media  initialization.
 *
 */
void SVC_CSO_GetDefaultConfig(SvcCso_Config_t *ptConfig);

/**
 *	Create a media  instance
 *
 *	@param[in] hActionId the action ID of OCTO
 *
 *	@return the instance handle of created media, or NULL if failure.
 */
SvcCso_t *SVC_CSO_Create(HUINT32 hActionId, PostMsgToMgrCb_t pfncPostMsgCallback);

/**
 *	Destroy a media  instance
 *
 *	@param[in]  the media instance
 */
HERROR SVC_CSO_Destroy(SvcCso_t *player);

/**
 *	Get the action ID which being used by media plyaer
 *
 *	@param[in] player the media player instance
 *
 *	@return the action ID
 */
HUINT32 SVC_CSO_GetActionId(SvcCso_t *player);

/**
 *	Get the instance of the media player which being played in system.
 *
 *	@param[in] hActionId the action ID of OCTO
 *
 *	@return the instance of media player, or NULL if there is no active player.
 */
SvcCso_t *SVC_CSO_GetActivePlayer(HUINT32 hActionId);

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
HERROR SVC_CSO_Play(SvcCso_t *player, HUINT32 ulStartTime);

/**
 *	stop the playback of the current media referenced by URL.
 *
 *	@param[in] player the media player instance
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_Stop(SvcCso_t *player);

/**
 *	Sets the current play position in milliseconds to the value of ullPosition_ms.
 *
 *	@param[in] player the media player instance
 *	@param[in] ullPosition_ms the play position in milliseconds
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_Seek(SvcCso_t *player, HUINT64 ullPosition_ms);

/**
 *	Get the URL of current media content
 *
 *	@param[in] player the media player instance
 *	@param[out] pszUrlBuf the buffer which will be filled by URL data.
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetPlayUrl(SvcCso_t *player, HINT8 *pszUrlBuf);

/**
 *	Set the URL of current media content
 *
 *	@param[in] player the media player instance
 *	@param[in] pszUrlBuf the URL of media content
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_SetPlayUrl(SvcCso_t *player, const HINT8 *szUrl);

/**
 *	Get the display pic and decoded pic of decoder.
 *
 *	@param[in] player the media player instance
 *	@param[out] pulDecodedPic the num of decoded pic
 *	@param[out] pulDisplayedPic the num of discplay pic
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetDecodedPicture(SvcCso_t *player, HUINT32 *pulDecodedPic, HUINT32 *pulDisplayedPic);

/**
 *	Get the play position in milliseconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[out] pePlayType the download type
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetPlayPosition(SvcCso_t *player, HUINT64 *pullPosition_ms);

/**
 *	Sets the current play position in milliseconds to the value of ullPosition_ms.
 *
 *	@param[in] player the media player instance
 *	@param[in] ullPosition_ms the play position in milliseconds
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_SetPlayPosition(SvcCso_t *player, HUINT64 ullPosition_ms);

/**
 *	Get the play speed in seconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[out] pnSpeed_ms the play speed
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetPlaySpeed(SvcCso_t *player, HINT32 *pnSpeed_ms);

/**
 *	Set the play speed in seconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[in] nSpeed_ms the play speed
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_SetPlaySpeed(SvcCso_t *player, HINT32 nSpeed_ms);

/**
 *	Get the full-screen mode
 *
 *	@param[in] player the media player instance
 *	@param[out] pbFullScreen the boolean value by full-screen mode.
 *	(true if the current mode is full-screen)
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetFullScreenMode(SvcCso_t *player, HBOOL *pbFullScreen);

/**
 *	Set the full-screen mode
 *
 *	@param[in] player the media player instance
 *	@param[in] bFullScreen the boolean value by full-screen mode.
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_SetFullScreenMode(SvcCso_t *player, HBOOL bFullScreen);

/**
 *	Get the total duration in milliseconds of media content.
 *
 *	@param[in] player the media player instance
 *	@param[out] pullDuration_ms the buffer for the total duration of media content
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetDuration(SvcCso_t *player, HUINT64 *pullDuration_ms);

/**
 *	Get the play state of media content. (Indication of the current play state)
 *
 *	@param[in] player the media player instance
 *	@param[out] peEvent the buffer for the play state of media content
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetPlayState(SvcCso_t *player, SvcCso_Event_e *peEvent);

/**
 *	Get DRM mode of current media content
 *
 *	@param[in] player the media player instance
 *	@param[out] pbDrm true of the media content includes a DRM data.
 *
 *	@return ERR_OK if success.
 */
HERROR SVC_CSO_GetDrmStatus(SvcCso_t *player, HBOOL *pbDrm);

/**
 *	Get a video track count
 *
 *	@param[in] player the media player instance
 *
 *	@return the video track count
 */
HINT32 SVC_CSO_GetVideoTrackCount(SvcCso_t *player);

/**
 *	Get a video track information
 *
 *	@param[in] player the media player instance
 *	@param[in] nIndex the index of video track
 *	@param[out] pVideoTrack the information of video track at @nIndex
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_GetVideoTrack(SvcCso_t *player, HINT32 nIndex,
					SvcCso_VideoTrack_t *pVideoTrack);

/**
 *	Get the active video track information
 *
 *	@param[in] player the media player instance
 *	@param[out] pVideoTrack the information of active video track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_GetActiveVideoTrack(SvcCso_t *player,
					SvcCso_VideoTrack_t *pVideoTrack);

/**
 *	Set the active video track
 *
 *	@param[in] player the media player instance
 *	@param[in] pVideoTrack the information of video track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_SetActiveVideoTrack(SvcCso_t *player,
					const SvcCso_VideoTrack_t *pVideoTrack);

/**
 *	Get an audio track count
 *
 *	@param[in] player the media player instance
 *
 *	@return the audio track count
 */
HINT32 SVC_CSO_GetAudioTrackCount(SvcCso_t *player);

/**
 *	Get an audio track information
 *
 *	@param[in] player the media player instance
 *	@param[in] nIndex the index of audio track
 *	@param[out] pAudioTrack the information of audio track at @nIndex
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_GetAudioTrack(SvcCso_t *player, HINT32 nIndex,
					SvcCso_AudioTrack_t *pAudioTrack);

/**
 *	Get the active audio track information
 *
 *	@param[in] player the media player instance
 *	@param[out] pAudioTrack the information of active audio track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_GetActiveAudioTrack(SvcCso_t *player,
					SvcCso_AudioTrack_t *pAudioTrack);

/**
 *	Set the active audio track
 *
 *	@param[in] player the media player instance
 *	@param[in] pAudioTrack the information of audio track
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_SetActiveAudioTrack(SvcCso_t *player,
					const SvcCso_AudioTrack_t *pAudioTrack);
HBOOL  SVC_CSO_IsVideoCodecSupport(DxVideoCodec_e codec);
HERROR SVC_CSO_GetAudioStereoConfig(SvcCso_t *player, DxStereoSelect_e *eStereoConfig);
HERROR SVC_CSO_SetAudioStereoConfig(SvcCso_t *player, DxStereoSelect_e eStereoConfig);
HERROR SVC_CSO_GetStereoType(SvcCso_t *player, DxStereoFormat_e *peStereoType);

/**
 *	Get the stream type of current media content
 *
 *	@param[in] player the media player instance
 *	@param[out] peStreamType the stream type of media content
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_GetStreamType(SvcCso_t *player, eMEDIA_StreamType_t *peStreamType);

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
HERROR SVC_CSO_GetVideoWindow(SvcCso_t *player, SvcCso_VideoWindow_t *pWindow);

HERROR SVC_CSO_GetBufferingProgress(SvcCso_t *player, HUINT32 *pulProgress);

HERROR SVC_CSO_GetBufferingChunkInfo(SvcCso_t *player,
							SvcCso_BufferInfo_t *ptChunkInfo);

HERROR SVC_CSO_SetBufferingScenario(SvcCso_t *player, HBOOL bAuto, HINT32 nThreshold);

HERROR SVC_CSO_GetWebUserAgent(SvcCso_t *player, HINT8 *pszUserAgent);

HERROR SVC_CSO_SetWebUserAgent(SvcCso_t *player, const HINT8 *szUserAgent);

HERROR SVC_CSO_SetVideoHide(SvcCso_t *player, HBOOL bHide);

HERROR SVC_CSO_SetAudioHide(SvcCso_t *player, HBOOL bHide);

HERROR SVC_CSO_CaptureScreen(SvcCso_t *player, const HINT8 *szSavePath);

HINT32 SVC_CSO_IsSubtitle(SvcCso_t *player);

HERROR SVC_CSO_GetSubtitleDisplay(SvcCso_t *player, HBOOL *pbShow);

HERROR SVC_CSO_GetSubtitleTotalLangCnt(SvcCso_t *player, HUINT32 *pnCount);

HERROR SVC_CSO_GetSubtitleCurrLangIndex(SvcCso_t *player, HUINT32 *pnIndex);

HERROR SVC_CSO_GetSubtitleLanguageByIndex(SvcCso_t *player, HINT32 nIndex, HINT8 *szLang);

HERROR SVC_CSO_GetSubtitleCurSyncTime(SvcCso_t *player, HINT32 *ulMsTime);

HERROR SVC_CSO_SetSubtitleUsage(SvcCso_t *player, HBOOL bUse);

HERROR SVC_CSO_SetSubtitleDisplay(SvcCso_t *player, HBOOL bEnable);

HERROR SVC_CSO_SetSubtitleLanguage(SvcCso_t *player, HUINT32 ulLangIdx);

HERROR SVC_CSO_ChangeSubtitleSyncTime(SvcCso_t *player, HINT32 ulMsTime);

HERROR SVC_CSO_SetSubtitleFontSize(SvcCso_t *player, HUINT32 ulSize);

HERROR SVC_CSO_GetSubtitleFontSize(SvcCso_t *player, HUINT32 *ulSize);

HERROR SVC_CSO_SetSubtitleDisplayPosition(SvcCso_t *player, HUINT32 ulPos);

HERROR SVC_CSO_GetSubtitleDisplayPosition(SvcCso_t *player, HUINT32 *ulPos);

HERROR SVC_CSO_SetBrowserUserAgentData(HCHAR *szString);

HERROR SVC_CSO_SetDrmLicenseData(SvcCso_t *player, HCHAR *szString);

HERROR SVC_CSO_GetOutputControlInfo(SvcCso_t *player, SvcCso_OutputControl_t *control);

SvcCso_Error_e SVC_CSO_GetLastErrorReason(SvcCso_t *player);

HERROR SVC_CSO_SetTLS(SvcCso_t *player, SvcCso_VerifyExternalCallback_t *VerifyExternalCb);

void SVC_CSO_SendCommand(SvcCso_t *player, SvcCso_Command_e command, HUINT32 p1);

HERROR SVC_CSO_GetResourceMap(SvcCso_Resource_t **pMap);
void SVC_CSO_ReleaseResourceMap(void* pMap);
void SVC_CSO_SetPlaypumpType(SvcCso_t *player, HUINT32 ulIndex, SvcCso_PumpType_e ePumpType);

HERROR SVC_CSO_GetPlaypumpIndex(SvcCso_t *player, HUINT32* pulIndex);
HERROR SVC_CSO_SetCurrentPlaypump(SvcCso_t *player, HUINT32 ulIndex);

HERROR SVC_CSO_FreePlaypumpIndex(SvcCso_t *player, HUINT32 ulIndex);
HERROR SVC_CSO_GetDnPConfig(SvcCso_t *player, SvcCso_Config_t* config);
void SVC_CSO_SetDuration(SvcCso_t *player, HUINT64 ullDuration);
void SVC_CSO_SetFirstPosition(SvcCso_t *player, HUINT64 ullFirstPosition);
void SVC_CSO_SetInputStreamType(SvcCso_t *player, SvcCso_InputStreamType_e eInputStreamType);

HERROR SVC_CSO_AcquireDtcpIpKey(HINT8 **pDtcpKey, HINT32 *nSize);
void SVC_CSO_ReleaseDtcpIpKey(HINT8 *pDtcpKey);
HERROR SVC_CSO_AudioFlush(SvcCso_t *player, HUINT32 unAudioTrack);
HERROR SVC_CSO_AudioRestart(SvcCso_t *player);
HERROR SVC_CSO_AudioCodecSwitch(SvcCso_t *player, HUINT32 unAudioTrack, HUINT32 unAudioCodec);
HERROR SVC_CSO_CreateAudioFadeTask(SvcCso_t *player);
HERROR SVC_CSO_DestroyAudioFadeTask(SvcCso_t *player);
HERROR SVC_CSO_SetAudioFade(SvcCso_t *player, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames);
HERROR SVC_CSO_GetAudioDecoderVolume(SvcCso_t *player, HUINT32 *punVolume);

/**
 *	Flush Media(Decoder)
 *
 *	@param[in] player the media player instance
 *
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_Flush(SvcCso_t *player);

/**
 *	TrickMode Setting
 *
 *	@param[in] player the media player instance
 *	@param[in] TRUE = Enable, FALSE = Disable
 *
 *
 *	@return ERR_OK if success
 */
HERROR SVC_CSO_TrickMode(SvcCso_t *player, HBOOL bEnable);

/**
 *	DemuxId Setting
 *
 *	@param[in] player the media player instance
 *	@param[in] Demux Id
 *
 *
 *	@return ERR_OK if success
 */
void SVC_CSO_SetDemuxId(SvcCso_t *player, HUINT32 ulDemuxId);

/**
 *	Demux SetPath Enable Setting
 *
 *	@param[in] player the media player instance
 *	@param[in] Enabble Flag
 *
 *
 *	@return none
 */

void SVC_CSO_EnableDemuxPath(SvcCso_t *player, HBOOL enable);


#ifdef __cplusplus
};
#endif


#endif	/* __SVC_MPLAYERCSO_H__ */
