#ifndef __DI_AUDIO_H__
#define __DI_AUDIO_H__
//=============================================================================
//
//  Copyright (C) 2008.  HUMAX INC.
//  All rights reserved.  No part of this program may be reproduced.
//
//
//=============================================================================
//
// Module       : di_audio.h
// Description  : Audio settings
//
//=============================================================================

//=============================================================================
//Include Files
//=============================================================================
#include "htype.h"
#include "di_err.h"

/******************************************************************************
* Definitions, typedef & const data
*******************************************************************************/
/** @brief Sound¿« Range ±∏¡∂. */
typedef enum
{
	DI_AUDIOTYPE_STREAM,
	DI_AUDIOTYPE_MEDIA,
	DI_AUDIOTYPE_PLAYBACK,
	DI_AUDIOTYOE_MAX
} DI_AUDIO_TYPE;

typedef enum
{
	DI_AUDFORMAT_MPEG						= 0x00000001,
	DI_AUDFORMAT_MP3						= 0x00000002,
	DI_AUDFORMAT_AAC						= 0x00000004,
	DI_AUDFORMAT_AAC_ADTS					= DI_AUDFORMAT_AAC,
	DI_AUDFORMAT_AAC_LOAS					= 0x00000008,
	DI_AUDFORMAT_AAC_PLUS					= 0x00000010,
	DI_AUDFORMAT_AAC_PLUS_LOAS				= DI_AUDFORMAT_AAC_PLUS,	 /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) sync and LATM mux */
	DI_AUDFORMAT_AAC_PLUS_ADTS				= 0x00000020,
	DI_AUDFORMAT_AC3						= 0x00000040,
	DI_AUDFORMAT_AC3_PLUS					= 0x00000080,
	DI_AUDFORMAT_PCM						= 0x00000100,
    DI_AUDFORMAT_DTS						= 0x00000200,            /* Digital Digital Surround sound. */
    DI_AUDFORMAT_LPCM_HD_DVD				= 0x00000400,      /* LPCM, HD-DVD mode */
    DI_AUDFORMAT_LPCM_BLUERAY				= 0x00000800,     /* LPCM, Blu-Ray mode */
    DI_AUDFORMAT_DTS_HD						= 0x00001000,          /* Digital Digital Surround sound, HD */
    DI_AUDFORMAT_WMA_STD					= 0x00002000,         /* WMA Standard */
    DI_AUDFORMAT_WMA_PRO					= 0x00004000,         /* WMA Professional */
    DI_AUDFORMAT_LPCM_DVD					= 0x00008000,        /* LPCM, DVD mode */
    DI_AUDFORMAT_PCMWAVE					= 0x00010000,
    DI_AUDFORMAT_DRA						= 0x00020000,
    DI_AUDFORMAT_FLAC						= 0x00040000,
	DI_AUDFORMAT_ADPCM						= 0x00080000,
	DI_AUDFORMAT_VORBIS						= 0x00100000,
	DI_AUDFORMAT_APE						= 0x00200000,
	DI_AUDFORMAT_OPUS						= 0x00400000,
	DI_AUDFORMAT_NONE						= 0x0
}DI_AUDIO_FORMAT;

typedef enum
{
	DI_AUDIO_DUAL_Stereo,
	DI_AUDIO_DUAL_LeftOnly,
	DI_AUDIO_DUAL_RightOnly,
	DI_AUDIO_DUAL_Mix
}DI_AUDIO_STEREO_MODE;

typedef enum {
	DI_AUDIO_DECODER_MAIN,		// Main decoder (MPEG, AC3 downmix .... )
	DI_AUDIO_DECODER_SUB,		// Audio Description
	DI_AUDIO_DECODER_SPDIF		// AC3 bypass
} DI_AUDIO_DECODER_TYPE;


typedef enum
{
	DI_AUDIO_MODE_UNKNOWN,
	DI_AUDIO_MODE_STEREO,
	DI_AUDIO_MODE_MUTI_STEREO,	// AAC,AC3  Over 2 Channel
	DI_AUDIO_MODE_MONO,
	DI_AUDIO_MODE_DUAL_MONO
}DI_AUDIO_MODE;

typedef struct
{
	DI_AUDIO_FORMAT		eFormat; //MP3,AAC,AC3,MPEG2°¶
	DI_AUDIO_MODE		eMode;
	HUINT32				ulSampleRate;
	HUINT32				ulRemainDataSize;
} DI_AUDIO_STREAM_INFO_t;

typedef struct DI_AUDIO_CAP_s
{
	HUINT32						group_id;
	HUINT32						decoder_id;
	DI_AUDIO_DECODER_TYPE		decoder_type;
	DI_AUDIO_FORMAT				support_format;
	HUINT32						volume_max;
	HUINT32						ulPrimerCount;
} DI_AUDIO_CAP_t;

typedef enum
{
	DI_AUDIO_RUNNING_STATE_CHANGED = 0,
	DI_AUDIO_FORMAT_CHANGED,
	DI_AUDIO_MEMORY_STREAM_STARTED,
	DI_AUDIO_MEMORY_STREAM_COMPLETED,
 /* only use it for TV platform*/
	DI_AUDIO_SIF_MODE_CHANGED,
	DI_AUDIO_HEADPHONE_CONNECTION,
	DI_AUDIO_DATAEMPTY,
	DI_AUDIO_DATAFLOW,	
/* for audio error log (CONFIG_AUDIO_ERROR_LOG)*/		
	DI_AUDIO_ERRORS_OCCURED,
	DI_AUDIO_EVENT_NOTIFY_END
}DI_AUDIO_NOTIFY_EVT;

typedef enum
{
	DI_AUDIO_STATE_UNKNOWN = 0,
	DI_AUDIO_STATE_DECODING,
	DI_AUDIO_STATE_NODATA		// NO DATA evnet should be notified to MW
								// when there is no decoded frame for more than 2 seconds
} DI_AUDIO_RUNNING_STATE;

typedef struct DI_AUDIO_RUNNING_STATE_EVT_s {
	DI_AUDIO_RUNNING_STATE state;
} DI_AUDIO_RUNNING_STATE_EVT_t;

#if 0
/* If PAL of OCTO 1.0/2.0 be changed to new format, Use This enum value */
typedef enum
{
	DI_AUDOUT_DOWNMIX=0,
	DI_AUDOUT_BYPASS,
	DI_AUDOUT_TRANSCODING
} DI_AUDIO_DIGITAL_OUTPUT_MODE;
#else
typedef enum
{
	DI_AUDOUT_SPDIF_DOWNMIX=0,
	DI_AUDOUT_SPDIF_BYPASS,
	DI_AUDOUT_SPDIF_TRANSCODING
} DI_AUDIO_SPDIF_MODE;

typedef enum
{
	DI_AUDOUT_HDMI_DOWNMIX=0,
	DI_AUDOUT_HDMI_BYPASS,
	DI_AUDOUT_HDMI_TRANSCODING
} DI_AUDIO_HDMI_MODE;
#endif

typedef enum
{
    DI_AUDOUT_COPYRIGHT_NO_RESTRICTION = 0,
    DI_AUDOUT_COPYRIGHT_ONE_COPY,
    DI_AUDOUT_COPYRIGHT_NO_COPY
} DI_AUDIO_COPYRIGHT_MODE;

typedef struct DI_AUDIO_FORMAT_CHANGED_EVT_s {
	HUINT32		dummy; // TBD
} DI_AUDIO_FORMAT_CHANGED_EVT_t;

/* for audio error log (CONFIG_AUDIO_ERROR_LOG)*/
typedef struct
{
	HUINT32 ulPtsCountError;
	HUINT32 ulFrameError;
} DI_AUDIO_ERROR_INFO_EVT_t;

typedef void (*pfnAUDIO_EVT_CALLBACK )(void  *pEventData); // pEvent = DI_AUDIO_CALLBACK_DATA *

typedef	struct
{
	HUINT32					ulDecId;
	HUINT32					ulRequestId;
	DI_AUDIO_NOTIFY_EVT		nEvent;
	void					*pvEventData;	// (DI_AUDIO_FORMAT_CHANGED_EVT_t *) or (DI_AUDIO_RUNNING_STATE_EVT_t *)
}DI_AUDIO_CALLBACK_DATA_t;

typedef enum
{
	DI_AUDIO_OUTPUT_ALL=0,
	DI_AUDIO_OUTPUT_DAC = 1,
	DI_AUDIO_OUTPUT_SPDIF = 2,
	DI_AUDIO_OUTPUT_HDMI = 4,
	DI_AUDIO_OUTPUT_SPEAKER = 8,
	DI_AUDIO_OUTPUT_HP = 16,
	DI_AUDIO_OUTPUT_SCART_1 = 32,
	DI_AUDIO_OUTPUT_SCART_2 = 64,
	DI_AUDIO_OUTPUT_I2S = 128
} DI_AUDIO_OUTPUT;

/*************************** TV define **********************************/
typedef enum
{
	eDI_AUDIO_SYSTEM_AUTO = 0,
	eDI_AUDIO_SYSTEM_BG,
	eDI_AUDIO_SYSTEM_I,
	eDI_AUDIO_SYSTEM_DK,
	eDI_AUDIO_SYSTEM_L,
	eDI_AUDIO_SYSTEM_MN,
	eDI_AUDIO_SYSTEM_UNKNOWN,
	eDI_AUDIO_SYSTEM_NUM
} DI_AUDIO_SIF_SYSTEM_e;


typedef enum
{
	eDI_AUDIO_MONO = 0,
	eDI_AUDIO_A2_STEREO,
	eDI_AUDIO_A2_DUAL,
	eDI_AUDIO_A2_DUAL_I,
	eDI_AUDIO_A2_DUAL_II,
	eDI_AUDIO_NICAM_MONO,
	eDI_AUDIO_NICAM_STEREO,
	eDI_AUDIO_NICAM_DUAL,
	eDI_AUDIO_NICAM_DUAL_I,
	eDI_AUDIO_NICAM_DUAL_II,
	eDI_AUDIO_MODE_NUM
} DI_AUDIO_SIF_MODE_e;


typedef enum
{
	eDI_AUDIO_HP_CONNECTED,
	eDI_AUDIO_HP_DISCONNECTED,
	eDI_AUDIO_HP_STATE_NUM
} DI_AUDIO_HP_STATE_e;


typedef enum
{
	eEQ_BAND_80Hz = 0,
	eEQ_BAND_220Hz,
	eEQ_BAND_470Hz,
	eEQ_BAND_1KHz,
	eEQ_BAND_2_2KHz,
	eEQ_BAND_4_7KHz,
	eEQ_BAND_12KHz,
	eEQ_BAND_NUM
} DI_AUDIO_EQ_BAND_INDEX_e;

typedef enum
{
	eDI_AUDIO_SR_32KHz  =  0x1,
	eDI_AUDIO_SR_44KHz  =  0x2,
	eDI_AUDIO_SR_48KHz  =  0x4,
	eDI_AUDIO_SR_88KHz  =  0x8,
	eDI_AUDIO_SR_96KHz  = 0x10,
	eDI_AUDIO_SR_176KHz = 0x20,
	eDI_AUDIO_SR_192KHz = 0x40,
	eDI_AUDIO_SR_MAX = 0x80
}DI_AUDIO_SAMPLE_RATE_e;

typedef struct
{
	HUINT32		ulRangeMin;
	HUINT32		ulRangeMax;
} DI_AUDIO_RANGE_t;

typedef struct
{
	DI_AUDIO_RANGE_t 	stBass;
	DI_AUDIO_RANGE_t 	stTreble;
	DI_AUDIO_RANGE_t 	stBalance;
	DI_AUDIO_RANGE_t 	stSpeakerVolume;
	DI_AUDIO_RANGE_t 	stHPVolume;
	DI_AUDIO_RANGE_t 	stEqualizer;
	DI_AUDIO_RANGE_t 	stDescriptionVolume;
} DI_AUDIO_RANGE_PARAM_t;

typedef struct
{
	DI_AUDIO_TYPE eAudioType;
	HUINT32 ulPrimerIndex;
	DI_AUDIO_FORMAT eFormat;
} DI_AUDIO_PRIMER_t;

typedef struct
{
	HUINT32 ulDepth;  /* depth in bytes of the decoder buffer */
	HUINT32 ulTotalSize;   /* total size in bytes of the decoder buffer */
} DI_AUDIO_DECODER_STATUS_t;

/*************************** TV define **********************************/

/*****************************************************************************/
/*   Global function prototypes                                              */
/*****************************************************************************/
DI_ERR_CODE DI_AUDIO_GetOutputNum(DI_AUDIO_OUTPUT eType, HUINT8* ucNum);
DI_ERR_CODE DI_AUDIO_GetCapability(HUINT32 *pucNumOfDevice , DI_AUDIO_CAP_t **ppCapInfo);
DI_ERR_CODE DI_AUDIO_Start(HUINT32 ulDevId ,DI_AUDIO_FORMAT nType);
DI_ERR_CODE DI_AUDIO_Stop (HUINT32 ulDevId);
DI_ERR_CODE DI_AUDIO_Pause(HUINT32 ulDevId, HBOOL pause);
DI_ERR_CODE DI_AUDIO_SetSpdifOutput (DI_AUDIO_SPDIF_MODE mode);
DI_ERR_CODE DI_AUDIO_SetHdmiOutput( DI_AUDIO_HDMI_MODE mode);
DI_ERR_CODE DI_AUDIO_SetHraOutput(HBOOL bSetPath);
DI_ERR_CODE DI_AUDIO_SetStereo(HUINT32 ulDevId, DI_AUDIO_STEREO_MODE dualMode);
DI_ERR_CODE DI_AUDIO_SetVolume(DI_AUDIO_OUTPUT eOutput, HUINT32 ucVolume);
DI_ERR_CODE DI_AUDIO_RegisterEventCallback(HUINT32 uDeviceId, DI_AUDIO_NOTIFY_EVT nEvent, pfnAUDIO_EVT_CALLBACK pfnCallback, HUINT32* ulCallerId);
DI_ERR_CODE DI_AUDIO_SetMute(DI_AUDIO_OUTPUT eOutput, HBOOL mute);
DI_ERR_CODE DI_AUDIO_StartMemoryStream(HUINT32 ulRequestId, DI_AUDIO_FORMAT dType, void *dMemBuf, HULONG dMemBufSize , HUINT32 dLoopCount, DI_AUDIO_OUTPUT output);
DI_ERR_CODE DI_AUDIO_StopMemoryStream(HUINT32 ulRequestId);
DI_ERR_CODE DI_AUDIO_PauseMemoryStream(HUINT32 ulRequestId, HBOOL pause);

DI_ERR_CODE DI_AUDIO_DelayOutput(HUINT32 ulDevId, HINT32 delayMs);
DI_ERR_CODE DI_AUDIO_GetState(HUINT32 ulDevId, DI_AUDIO_RUNNING_STATE *pnAudioState);
DI_ERR_CODE DI_AUDIO_SetEventTimeOut(HUINT32 ulDevId, HUINT32 timeout);
DI_ERR_CODE DI_AUDIO_SetSCMS(DI_AUDIO_COPYRIGHT_MODE mode);
DI_ERR_CODE DI_AUDIO_GetStreamInfo(HUINT32 ulDevId, DI_AUDIO_STREAM_INFO_t *ptAudioStreamInfo);
DI_ERR_CODE DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT eOutputType, HBOOL OnOff, DI_AUDIO_FORMAT outType);
DI_ERR_CODE DI_AUDIO_SetMediaType(HUINT32 ulDevId, DI_AUDIO_TYPE nType);
DI_ERR_CODE DI_AUDIO_SetSync(HBOOL bOnOff);
DI_ERR_CODE DI_AUDIO_Flush(HUINT32 ulDevId);
DI_ERR_CODE DI_AUDIO_StartPrimer(HUINT32 ulDevId, DI_AUDIO_PRIMER_t *pPrimerSetting);
DI_ERR_CODE DI_AUDIO_StopPrimer(HUINT32 ulDevId, DI_AUDIO_PRIMER_t *pPrimerSetting);
DI_ERR_CODE DI_AUDIO_SelectPrimer(HUINT32 ulDevId, HUINT32 ulPrimerIndex);
DI_ERR_CODE DI_AUDIO_SetDecoderVolume(HUINT32 ulDevId, HUINT32 ulDecoderVolume);
DI_ERR_CODE DI_AUDIO_GetHRAOutputSampleRate(DI_AUDIO_SAMPLE_RATE_e eInput, DI_AUDIO_SAMPLE_RATE_e *eOutput);
DI_ERR_CODE DI_AUDIO_GetOutputSampleRate(DI_AUDIO_SAMPLE_RATE_e eInput, DI_AUDIO_SAMPLE_RATE_e *eOutput);
DI_ERR_CODE DI_AUDIO_GetPlaybackHandle(HUINT32 ulDeviceId,void *handle);
DI_ERR_CODE DI_AUDIO_SetAllPath(HUINT32 ulDeviceId,HBOOL bOnOff);
DI_ERR_CODE DI_AUDIO_GetDecoderHandle(HUINT32 ulDeviceId,void *handle);
DI_ERR_CODE DI_AUDIO_GetMixerHandle(void *handle);
DI_ERR_CODE DI_AUDIO_GetCurrentDecoderStatus(HUINT32 ulDeviceId, DI_AUDIO_DECODER_STATUS_t *pstAudioDecoderStatus);
#endif /* _DI_AUDIO_H_ */

