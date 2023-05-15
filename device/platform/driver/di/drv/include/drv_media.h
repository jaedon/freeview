/**
 * drv_media.h
*/

#ifndef __DRV_MEDIA_H__
#define __DRV_MEDIA_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

#include "di_video.h"
#include "di_audio.h"
#if defined(CONFIG_DI10)
#include "di_media.h"
#else
#include "di_media20.h"
#endif
#include "di_demux.h"

#include "drv_demux.h"
#include "nexus_base_types.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#if defined(CONFIG_AUDIO_DECODER)
#include "nexus_audio_decoder.h"
#include "nexus_audio_playback.h"
#endif
#include "nexus_playpump.h"


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_MEDIA_Init(void);
NEXUS_PidChannelHandle DRV_MEDIA_GetPlaybackPidChannel(DI_MEDIA_PLAYPUMP mediaType ,void* decoderHandle );
NEXUS_StcChannelHandle DRV_MEDIA_GetPlaybackStcChannelHandle(DI_MEDIA_PLAYPUMP mediaType );

/**
 * @brief		Media 설정을 종료한다.
 *
 * @param		None
 * @return		DRV_Error
 * @see			DRV_MEDIA_Init
 */
DRV_Error DRV_MEDIA_Term(void);

/**
 * @brief 		media 재생을 위해 Video Pid Channel을 얻는다.
 *
 * @param[in]	unMediaId				media Id
 * @param[in]	eVideoCodec				Video Codec
 * @param[in]	VideoDecoderHandle		Video Decoder Handle
 * @return		NEXUS_PidChannelHandle
 * @see			DRV_MEDIA_GetPlaybackAudioPidChannel
 */
NEXUS_PidChannelHandle DRV_MEDIA_GetPlaybackVideoPidChannel(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_VIDEO_FORMAT eVideoCodec, void *VideoDecoderHandle);

/**
 * @brief		media 재생을 위해 Audio Pid Channel을 얻는다.
 *
 * @param[in]	unMediaId				media Id
 * @param[in]	eAudioCodec				Audio Codec
 * @param[in]	eDecoderType			Audio Decoder Handle Type
 * @param[in]	AudioDecoderHandle		Audio Decoder Handle
 * @return		NEXUS_PidChannelHandle
 * @see			DRV_MEDIA_GetPlaybackVideoPidChannel
 */
NEXUS_PidChannelHandle DRV_MEDIA_GetPlaybackAudioPidChannel(DI_MEDIA_AUDIO_FORMAT_e eAudioCodec, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, void *AudioDecoderHandle);

/**
 * @brief		Open된 Stc Channel을 Media Type에 맞게 설정 후 얻는다.
 *
 * @param[in]	unMediaId	media Id
 * @param[in]	mediaType	media type
 * @return		NEXUS_StcChannelHandle
 * @see
 */
NEXUS_StcChannelHandle DRV_MEDIA_MP_GetPlaybackStcChannelHandle(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, DI_MEDIA_PLAYPUMP mediaType);

/**
 * @brief		Demux에서 TS에 대한 PAT, PMT Parsing을 위해 Playback Pid Channel을 Open한다.
 *
 * @param[in]	nDemuxId				Demux Id
 * @param[in]	nPid					Pid Number
 * @param[in]	nChannelType			Channel Type
 * @param[out]	phPidChannel			Pid Channel Handle
 * @return		DRV_Error
 * @see			DRV_MEDIA_ClosePlaybackPidChannel
 */
DRV_Error DRV_MEDIA_OpenPlaybackPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, HINT32 nPid, NEXUS_PidChannelHandle *phPidChannel);

/**
 * @brief		Demux에서 TS에 대한 PAT, PMT Parsing을 위해 Open한 Playback Pid Channel을 Close한다.
 *
 * @param[in]	nDemuxId				Demux Id
 * @param[in]	nChannelType			Channel Type
 * @param[in]	hPidChannel				Pid Channel Handle
 * @return		DRV_Error
 * @see			DRV_MEDIA_OpenPlaybackPidChannel
 */
DRV_Error DRV_MEDIA_ClosePlaybackPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, NEXUS_PidChannelHandle hPidChannel);

/* 
 * Audio Pid Channel Setting함수. 
 */
#if defined(CONFIG_AUDIO_DECODER)
DRV_Error DRV_MEDIA_SetAudioPidChannel(NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_AudioDecoderHandle ulAudioDecHandle,
														NEXUS_AudioDecoderHandle ulAudioPassThroughDecHandle);
#endif

/* Check Media Stat Type */
HBOOL DRV_MEDIA_CheckStart(void);

#endif /* !__DRV_MEDIA_H__ */

