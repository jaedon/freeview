/**
 * @file	di_transcoder.h
 * @brief	Transcoder module DI layer header file
 * @version 	$Revision: 0.9.0 $
 * @date 	$Date: 2012/05/08 12:00:00 $
 * @author
 */

/** @defgroup	DI_TRANSCODER	TRANSCODER COMMON IF OF DI LAYER
 *  This is DI_TRANSCODER module of DI layer.
 *  @{
 */


#ifndef __DI_TRANSCODER_H__
#define __DI_TRANSCODER_H__

/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "di_err.h" /* if di module is included, di_err.h is included first!Later this line will be removed.*/

#include "di_video.h"
#include "di_audio.h"

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/
#define NUM_OF_MAX_TRANSCODERS				16	/* Temporarily, define max counter */

/**
 * Define transcoder type
 */
typedef enum
{
	DI_TRANSCODER_SOURCE_WATCH			= 0x00000001,	/* WATCH Path */			
	DI_TRANSCODER_SOURCE_BACKGROUND			= 0x00000002, 	/* BACKGROUND Path */
	DI_TRANSCODER_NONE				= 0x00000000
} DI_TRANSCODER_SOURCE_TYPE;

/**
 * Define transcoder MUX (container) type
 */
typedef enum
{
	DI_TRANSCODER_MUX_TS				= 0x00000001,	/* MPEG2 TS */			
	DI_TRANSCODER_MUX_MP4				= 0x00000002, 	/* MP4 */
	DI_TRANSCODER_MUX_ASF				= 0x00000004, 	/* ASF */
	DI_TRANSCODER_MUX_NONE				= 0x00000000
} DI_TRANSCODER_MUX_TYPE;

/**
 * Enum video codec levels
 */
typedef enum
{
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_00 = 0,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_10,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_1B,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_11,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_12,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_13,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_20,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_21,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_22,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_30,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_31,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_32,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_40,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_41,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_42,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_50,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_51,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_60,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_62,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_LOW,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_MAIN,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH1440,
	DI_TRANSCODER_VIDEO_CODEC_LEVEL_MAX

} DI_TRANSCODER_VIDEO_CODEC_LEVEL;

/**
 * Enum video codec profiles 
 */
typedef enum
{
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_SIMPLE = 0,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_HIGH,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCED,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_JIZHUN,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_SNRSCALABLE,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_SPATIALLYSCALABLE,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCEDSIMPLE,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE,
	DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAX

} DI_TRANSCODER_VIDEO_CODEC_PROFILE;

/**
* Enum Transcoder Mode
*/
typedef enum
{
	DI_TRANSCODER_ENCODE_AV=0,
	DI_TRANSCODER_ENCODE_VIDEO,
	DI_TRANSCODER_ENCODE_AUDIO,
	DI_TRANSCODER_ENCODE_MAX

} DI_TRANSCODER_ENCODE_MODE;

typedef enum
{
	DI_TRANSCODER_AUDIO_BYPASS= 0,
	DI_TRANSCODER_AUDIO_ENCODING,
	DI_TRANSCODER_AUDIO_MAX
} DI_TRANSCODER_AUDIO_ENC_TYPE;

/**
 * Define transcoder video settings
 */
typedef struct
{
	DI_VIDEO_HD_RESOLUTION				eRes;
	DI_VIDEO_FrameRate_t				eFrameRate;
	DI_VIDEO_AspectRatio_t				eAspectRatio;

	struct {
		HBOOL					bCustomRes;
		HUINT32					ulWidth;			
		HUINT32					ulHeight;			
		HUINT32					ulRefreshRate;
		HBOOL					bInterlaced;
		HUINT32					ulSampleAspectRatioX;
		HUINT32					ulSampleAspectRatioY;
	} stCustomRes;

	HUINT32						ulMaxBitRate;
	struct {
		HBOOL					bCustomIPB;	
		HUINT32					ulFrameP;
		HUINT32					ulFrameB;
	} stCustomIPB;

} DI_TRANSCODER_VideoSettings_t;

typedef struct
{
	DI_TRANSCODER_AUDIO_ENC_TYPE eEncType;
	HUINT32	ulMultiAudioCnt;
	HBOOL	bMultiAudio;
} DI_TRANSCODER_AudioSettings_t;

/**
 * Define transcoder Update Settings
 */
typedef struct
{
	DI_TRANSCODER_VideoSettings_t			stVideoSettings;
} DI_TRANSCODER_UpdateSettings_t;


/**
 * Define transcoder Settings
 */
typedef struct
{
	
	DI_TRANSCODER_SOURCE_TYPE				eTransSourceType;
	DI_TRANSCODER_ENCODE_MODE				eTransEncodeMode;
	DI_VIDEO_FORMAT							eInputVideoFormat;
	DI_VIDEO_FORMAT							eOutputVideoFormat;
	DI_AUDIO_FORMAT							eInputAudioFormat;
	DI_AUDIO_FORMAT							eOutputAudioFormat;

	HBOOL									bVideoDefaultProfile; /* If TURE, set default Level/Profile/BitRate GOP group by Resolution and FrameRate info */
	DI_TRANSCODER_VIDEO_CODEC_LEVEL			eCodecLevel;
	DI_TRANSCODER_VIDEO_CODEC_PROFILE		eCodecProfile;
	DI_TRANSCODER_VideoSettings_t			stVideoSettings;
	
	HUINT32						ulDemuxId;
	HUINT16						usVideoPid;
	HUINT16						usAudioPid;
	HUINT16						usPcrPid;
	DI_TRANSCODER_AudioSettings_t	stAudioSettings;
} DI_TRANSCODER_Settings_t;

/**
 * Define MUX Infomation Settings
 */
typedef struct
{
	HUINT8	index;
	HUINT32 videoCodec;
	HUINT16 videoPid;
	HUINT32 audioCodec;
	HUINT16 audioPid;
	HUINT16 audioSubPid;
	HUINT16 pcrPid;
	HUINT16 PMTPid;
	HUINT8 *PATData;
	HUINT32 sizePAT;
	HUINT8 *PMTData;
	HUINT32 sizePMT;
	HBOOL NonPcrInsert;
	HBOOL bMultiAudio;
	HUINT32 ulMultiAudioCnt;
} DI_TRANSCODER_MUX_INFO;

/**
 * Define transcoder capability
 */
typedef struct
{
	HUINT32						ulGroupId;
	HUINT32						ulTranscoderId;
	DI_TRANSCODER_SOURCE_TYPE			eTransSourceType;
	DI_VIDEO_FORMAT					eVideoFormat;
	DI_AUDIO_FORMAT					eAudioFormat;
	DI_TRANSCODER_MUX_TYPE				eTranscoderMuxType;
} DI_TRANSCODER_CAP_t;

/**
 * Define transcoder video sequence header info 
 */
typedef struct
{
	HUINT32					ulWidth;
	HUINT32					ulHeight;
	HUINT32					ulDisplayWidth;
	HUINT32					ulDisplayHeight;
	DI_VIDEO_AspectRatio_t			eAspectRatio;
	HUINT16					usSampleAspectRatioX;
	HUINT16					usSampleAspectRatioY;
	DI_VIDEO_FrameRate_t			eFrameRate;
	DI_VIDEO_FORMAT				eVideoFormat;
	HBOOL					bProgressive;
	HBOOL					ucColorPrimarie;
	DI_TRANSCODER_VIDEO_CODEC_LEVEL		eCodecLevel;
	DI_TRANSCODER_VIDEO_CODEC_PROFILE	eCodecProfile;
} DI_TRANSCODER_DEC_StreamInfo_t;

/**
 * Define transcoder video sequence header info 
 */
typedef enum
{
	DI_TRANSCODER_NOTIFY_EVT_DEC_SEQUENCE_HEADER_INFO,
	DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_FAIL,
	DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS,
	DI_TRANSCODER_NOTIFY_EVT_END
} DI_TRANSCODER_NOTIFY_EVT;

typedef	struct
{
	HUINT32					ulDeviceId;
	DI_TRANSCODER_NOTIFY_EVT		eEvent;
	void					*pvEventData; /* Sequence Header, Probe etc.. */
}DI_TRANSCODER_CALLBACK_DATA_t;

typedef void (*TRANSCODER_EVENT_CALLBACK_FUNCTION )(void *pCallbackData);

DI_ERR_CODE DI_TRANSCODER_GetCapability(HUINT32 *pulNumofDevice, DI_TRANSCODER_CAP_t **ppCapInfo);
DI_ERR_CODE DI_TRANSCODER_SetStereo(HUINT32 ulDeviceId, DI_AUDIO_STEREO_MODE dualMode);
DI_ERR_CODE DI_TRANSCODER_SetSettings(HUINT32 ulDeviceId, DI_TRANSCODER_Settings_t *Settings);
DI_ERR_CODE DI_TRANSCODER_Start(HUINT32 ulDeviceId, DI_TRANSCODER_MUX_TYPE eMuxType, DI_TRANSCODER_MUX_INFO *Info);
DI_ERR_CODE DI_TRANSCODER_Stop(HUINT32 ulDeviceId);
DI_ERR_CODE DI_TRANSCODER_Close(HUINT32 ulDeviceId);
DI_ERR_CODE DI_TRANSCODER_ReStart(HUINT32 ulDeviceId);

DI_ERR_CODE DI_TRANSCODER_SetPSIData(HUINT32 ulDeviceId,HUINT16 psiPid,HUINT32 psiSize,HUINT8 *psiData,HUINT8 contflag,unsigned long timeOut, HUINT32 *pIndexId );
DI_ERR_CODE DI_TRANSCODER_SetPesData(HUINT32 ulDeviceId,HUINT32 ulInputDemuxId,HUINT16 psiPid,HUINT32 *pIndexId );
DI_ERR_CODE DI_TRANSCODER_UpdatePSIData(HUINT32 ulDeviceId,HUINT32 ulIndex, HUINT32 psiSize,HUINT8 *pPSIData,HUINT8 startflag);
DI_ERR_CODE DI_TRANSCODER_UpdateSetSettings(HUINT32 ulDeviceId, DI_TRANSCODER_UpdateSettings_t *Settings);
DI_ERR_CODE DI_TRANSCODER_UpdateGetSettings(HUINT32 ulDeviceId, DI_TRANSCODER_UpdateSettings_t *Settings);
DI_ERR_CODE DI_TRANSCODER_ProbeStart(HUINT32 ulDeviceId, HUINT32 ulDemuxId, DI_VIDEO_FORMAT eVideoCodec);
DI_ERR_CODE DI_TRANSCODER_ProbeStop(HUINT32 ulDeviceId);
DI_ERR_CODE DI_TRANSCODER_ProbeClose(HUINT32 ulDeviceId);
DI_ERR_CODE DI_TRANSCODER_ProbeTimeOut(HUINT32 ulDeviceId, HUINT32 ulTimeout);/* timeout in ms */
DI_ERR_CODE DI_TRANSCODER_GetDecSequenceHeaderInfo(HUINT32 ulDeviceId, DI_TRANSCODER_DEC_StreamInfo_t *stream_info);
DI_ERR_CODE DI_TRANSCODER_RegisterEventCallback(HUINT32 ulDeviceId, DI_TRANSCODER_NOTIFY_EVT eEvent, EVENT_CALLBACK_FUNCTION fnTranscoderCallback);

#endif /* _DI_TRANSCODER_H_ */
