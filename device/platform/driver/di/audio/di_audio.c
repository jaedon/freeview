/*
 * @defgroup		DI_AUDIO DI_AUDIO : DI_AUDIO module
 * @author			swlee2
 * @note			DI_AUDIO APIs
 * @brief			Define DI_AUDIO APIs
 * @file 			di_audio.c                           */


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
	#include <string.h>
	#include <stdlib.h>
#endif

#include "linden_trace.h"

#include "bstd_defs.h"
#if ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_common.h"
#endif
#if defined(BCHP_HIFIDAC_CTRL0_REG_START)
/* OLD AUDIO DAC STYLE*/
#else
#if ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_aud_fmm_iop_out_dac_ctrl_0.h"
#endif
#endif
#include "nexus_platform.h"

#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_playpump.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_playback.h"
#include "nexus_ac3_encode.h"
#include "nexus_dts_encode.h"
#if defined(CONFIG_HDMI_IN)
#include "nexus_hdmi_output.h"
#include "nexus_hdmi_input.h"
#endif

#include "nexus_audio_dummy_output.h"
#if defined(CONFIG_BLUETOOTH)
#include "nexus_audio_capture.h"
#endif
#if defined(CONFIG_HDMI_IN)
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
#include "nexus_audio_input_capture.h"
#endif
#endif
#include "vkernel.h"
#include "taskdef.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "drv_demux.h"
#include "drv_pvr.h"
#include "di_demux.h"
#if defined(CONFIG_MEDIA)
#if defined(CONFIG_DI10)
#include "drv_media_10.h"
#else
#include "drv_media.h"
#endif
#endif
#if defined(CONFIG_CALYPSO)
#include "drv_calypso.h"
#endif
#include "drv_err.h"

#include "di_scart.h"
#include "drv_scart.h"
#include "di_uart.h"
#include "di_media.h"
#include "drv_hdmi.h"
#include "drv_gpio.h"
#include "drv_rfvod.h"
#if defined(CONFIG_SYNC_CHANNEL)
#include "drv_sync_channel.h"
#endif
#if defined(CONFIG_ASTM)
#include "drv_astm.h"
#endif
#if defined(CONFIG_SYNC_RECOVERY)
#include "drv_sync_recovery.h"
#endif

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
#include "drv_audio_hra.h"
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
#include "drv_video.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/*	AUDIO_SYNC_OFFSET should be defined at "product.config".
*	AUDIO_SYNC_OFFSET value can be defined each of projects.
*/
#if defined CONFIG_AUDIO_SYNC_OFFSET
	#define PTSOFFSET	CONFIG_AUDIO_SYNC_OFFSET
#else
	#define PTSOFFSET	0
#endif

#if defined CONFIG_AUDIO_SYNC_MPEG_OFFSET
	#define MPEG_PTSOFFSET	CONFIG_AUDIO_SYNC_MPEG_OFFSET
#else
	#define MPEG_PTSOFFSET	0
#endif

#if defined CONFIG_AUDIO_SYNC_AC3_OFFSET
	#define AC3_PTSOFFSET	CONFIG_AUDIO_SYNC_AC3_OFFSET
#else
	#define AC3_PTSOFFSET	0
#endif

#if defined CONFIG_AUDIO_SYNC_AC3P_OFFSET
	#define AC3P_PTSOFFSET	CONFIG_AUDIO_SYNC_AC3P_OFFSET
#else
	#define AC3P_PTSOFFSET	0
#endif

#if defined CONFIG_AUDIO_SYNC_AAC_OFFSET
	#define AAC_PTSOFFSET	CONFIG_AUDIO_SYNC_AAC_OFFSET
#else
	#define AAC_PTSOFFSET	0
#endif

#if defined CONFIG_AUDIO_SYNC_AACP_OFFSET
	#define AACP_PTSOFFSET	CONFIG_AUDIO_SYNC_AACP_OFFSET
#else
	#define AACP_PTSOFFSET	0
#endif

#if defined CONFIG_AUDIO_PTS_THRESHOLD_ADJUSTMENT
	#define AUDIO_PTS_THRESHOLD	CONFIG_AUDIO_PTS_THRESHOLD_VALUE
#else
	/* If zero (default), PTS Threshold is 8 ms */
	#define AUDIO_PTS_THRESHOLD	0
#endif

#define DEBUG_MODULE TRACE_MODULE_DI_AUDIO
#define DI_AUDIO_DEBUG

//#define PRINT_AUDIO_PTS_INFO /* for debug pts info*/

/* It synchronizes the SUB audio to the Main audio for DOLBY certification.
    Dolby require that main and associated audio are synchronous within +- 0.5 ms
	Do not use during normal operation. */
//#define SYNC_SUB_AUDIO_FOR_DOLBY_CERTI

/*******************************************************************/
/******************************* extern ******************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

static int16_t PCMsamples[48];
static const int16_t s_PCMsamples[48] =
{
	0,
	4276,
	8480,
	12539,
	16383,
	19947,
	23169,
	25995,
	28377,
	30272,
	31650,
	32486,
	32767,
	32486,
	31650,
	30272,
	28377,
	25995,
	23169,
	19947,
	16383,
	12539,
	8480,
	4276,
	0,
	-4277,
	-8481,
	-12540,
	-16384,
	-19948,
	-23170,
	-25996,
	-28378,
	-30273,
	-31651,
	-32487,
	-32767,
	-32487,
	-31651,
	-30273,
	-28378,
	-25996,
	-23170,
	-19948,
	-16384,
	-12540,
	-8481,
	-4277
};

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/

//20090121 :: POOH :: group이 늘어나면 init부터 정리 필요....
#define DI_AUDIO_DECODER_MAX 	DI_AUDIO_DECODER_SPDIF+1	//max값 좀 만들어 두지....

#if defined(CONFIG_AUDIO_VOLUME_LEVEL_MAX)
#define AUDIO_LEVEL_PERCENT_MAX	100
#define AUDIO_LEVEL_MAX		CONFIG_AUDIO_VOLUME_LEVEL_MAX_VALUE
#define AUDIO_LEVEL_MIN		0
#define AUDIO_VOLUME_UNIT	-(4000/AUDIO_LEVEL_MAX)
#else
#if defined(CONFIG_DI10)
//volume bar is divided as 20 pieces : VOLUME_UNIT -200 is 2 DECIBEL
#define AUDIO_LEVEL_PERCENT_MAX	100
#define AUDIO_LEVEL_MAX		20
#define AUDIO_LEVEL_MIN		0
#define AUDIO_VOLUME_UNIT	-200
#else
//volume bar is divided as 100 pieces : VOLUME_UNIT -200 is 2 DECIBEL
#define AUDIO_LEVEL_PERCENT_MAX	100
#define AUDIO_LEVEL_MAX		100
#define AUDIO_LEVEL_MIN		0
#define AUDIO_VOLUME_UNIT	-40
#endif
#endif
#define AUDIO_MAX_DECIBEL	NEXUS_AUDIO_VOLUME_DB_MAX
#define AUDIO_MIN_DECIBEL	NEXUS_AUDIO_VOLUME_DB_MIN

//linear volume 0~0x800000, over 0x800000 value is not affect to volume.
#define AUDIO_LEVEL_LINEAR_UNIT (NEXUS_AUDIO_VOLUME_LINEAR_NORMAL) / (AUDIO_LEVEL_PERCENT_MAX)

#define DI_AUDIO_UNDERRUN_TIMEOUT_RES	100
#define DI_AUDIO_UNDERRUN_TIMEOUT_DEFAULT	2000/DI_AUDIO_UNDERRUN_TIMEOUT_RES

#define DI_AUDIO_MSG_SIZE MSGQ_SIZE_NORMAL
#define DI_AUDIO_DECODING_EVENT_LIMIT 5

/* audio and media have each callbacks..So Currently 2 kinds of callback exists. */
#define AUDIO_CALLBACK_OWNER_MAX 5
#define AUDIO_EVENT_NUM	DI_AUDIO_EVENT_NOTIFY_END

#define AUDIO_DEFAULT_SAMPLERATE 48000;
#define AUDIO_DEFAULT_BITSPERSAMPLE 16;

#define AUDIO_DELAY_MAX	4000
#define AUDIO_HEAAC_DEC_VOLUME 4

#if defined(BCHP_HIFIDAC_CTRL0_REG_START)
/* OLD DAC Volume Modification */
/* 7344/7354/7346/7356/7425/7424/7358/7301*/

#define DAC_VOLUME_MODIFICATION 40

#elif defined(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL_0_ANALOG_CTRL_REG_0)
/* AMP DAC Volume Modification */
/*7445*/

#define DAC_VOLUME_MODIFICATION 0

#else
/*NEW DAC Volume Modification*/
/* 7231/7229  7429/7241/7428/7430 7435   (20131007) */

#define DAC_VOLUME_MODIFICATION 100

#endif

#if defined(CONFIG_DI20)
#define DI_AUDIO_HDMI_OUTPUTS  NEXUS_NUM_HDMI_OUTPUTS
#define DI_AUDIO_SPDIF_OUTPUTS  NEXUS_NUM_SPDIF_OUTPUTS
#define DI_AUDIO_RCA_OUTPUTS  NEXUS_NUM_COMPOSITE_OUTPUTS
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
#define AUDIO_PRIMER_MAX_COUNT CONFIG_PRIMER_COUNT
#else
#define AUDIO_PRIMER_MAX_COUNT 0
#endif

typedef enum
{
	DI_AUDIO_FREE=0,
	DI_AUDIO_ALLOCATED,
	DI_AUDIO_START,
	DI_AUDIO_PAUSED,
	DI_AUDIO_STOP
}DI_AUDIO_STATUS;

typedef enum
{
	DI_AUDIO_UNMUTE_READY=0,		//when decoder started...
	DI_AUDIO_UNMUTE_NOT_READY
}DI_MUTE_STATUS;

typedef enum
{
	DI_AUDIO_MEMORY_STREAM_PLAY=0,
	DI_AUDIO_MEMORY_STREAM_STOP,
	DI_AUDIO_MEMORY_STREAM_PAUSE,
	DI_AUDIO_MEMORY_STREAM_MONITOR,
	DI_AUDIO_MEMORY_PCM_PLAY,
	DI_AUDIO_MEMORY_PCM_STOP,
	DI_AUDIO_MEMORY_PCM_PAUSE
} DI_AUDIO_STREAM_REQUEST;

typedef enum
{
	DI_AUDIO_TASK1=0,
	DI_AUDIO_TASK2,
	DI_AUDIO_TASK_BUSY
}DI_AUDIO_TASK_ID;

typedef enum
{
	DI_AUDIO_MEMPOOL1=0,
	DI_AUDIO_MEMPOOL2,
	DI_AUDIO_MEMPOOL_BUSY
}DI_AUDIO_MEMPOOL_ID;

typedef enum
{
	DI_AUDIO_MIXER_TYPE_NONE=0,
	DI_AUDIO_MIXER_TYPE_HW_MIXER,
	DI_AUDIO_MIXER_TYPE_DSP_MIXER
}DI_AUDIO_MIXER_TYPE;

typedef enum
{
	DI_AUDIO_DECODER_MODE_NONE=0,
	DI_AUDIO_DECODER_MODE_DESCRIPTION,
	DI_AUDIO_DECODER_MODE_DECODER,
	DI_AUDIO_DECODER_MODE_PASSTHRU
}DI_AUDIO_DECODER_MODE;

typedef enum
{
	DI_AUDIO_INPUT_NONE=0,
	DI_AUDIO_INPUT_MIXER,
	DI_AUDIO_INPUT_DECODER,
	DI_AUDIO_INPUT_ENCODER,
	DI_AUDIO_INPUT_PLAYBACK,
	DI_AUDIO_INPUT_PLAYPUMP,
	DI_AUDIO_INPUT_MAX
}DI_AUDIO_INPUT_INFO;

typedef struct
{
	DI_AUDIO_STREAM_REQUEST sRequest;
	HULONG pos;
	HUINT32 ulTaskIndex;
	HUINT32 sDevId;
	HUINT32 sLoopCount;
	HUINT32 ulRequestId;
	HUINT32 ulTimeTicket;
	unsigned char *sMemBuf;
	unsigned long sMemBufSize;
	bool sPause;
} DI_AUDIO_STREAM_MSG;

typedef struct
{
	unsigned long			esDataSize;
	unsigned long long		remainDataSize;
	unsigned long			loopCount;
}DI_AudioStreamPlayInfo;

typedef struct
{
	pfnAUDIO_EVT_CALLBACK		audioEventCallback[AUDIO_ALL_DECODERS][AUDIO_EVENT_NUM];
	HBOOL						bOccupied;
} DI_AudioCallback;

#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
typedef struct
{
	/* Is started primer */
	HBOOL bStartPrimer;

	/* LIVE TS, Media, Playback */
	DI_AUDIO_TYPE eAudioType;

	/* Start or stop primer index */
	HUINT32 ulSelectedIndex;

	/* Used for playback or Media, Not for LIVE TS */
	NEXUS_PlaybackHandle hPlayBack;

	/* Track setting info */
	HBOOL bOccupied[AUDIO_PRIMER_MAX_COUNT];
	DI_AUDIO_FORMAT eFormat[AUDIO_PRIMER_MAX_COUNT];
	NEXUS_AudioDecoderPrimerHandle hPrimer[AUDIO_PRIMER_MAX_COUNT];
	NEXUS_AudioDecoderStartSettings decoderSettings[AUDIO_PRIMER_MAX_COUNT];
}DI_AudioMultiTrack_t;
#endif

typedef struct
{
	HBOOL bUsedDummy;
	NEXUS_AudioInput input;

}DI_AudioDummyOutput_t;

typedef struct
{
	HBOOL	bInitialized;
	NEXUS_AudioDecoderHandle 	audioDecoder[AUDIO_ALL_DECODERS];
	NEXUS_AudioMixerHandle		audioMixer;
    NEXUS_Ac3EncodeHandle 		ac3Encoder;
    NEXUS_DtsEncodeHandle 		dtsEncoder;
#if defined(CONFIG_BLUETOOTH)
	NEXUS_AudioCaptureHandle	audioCapture[NEXUS_NUM_AUDIO_CAPTURES];
#endif
#if defined(CONFIG_HDMI_IN)
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
	NEXUS_AudioInputCaptureHandle 	audioInputCapture[NEXUS_NUM_AUDIO_INPUT_CAPTURES];
#endif
#endif

#if defined(NEXUS_NUM_AUDIO_DUMMY_OUTPUTS)
	DI_AudioDummyOutput_t		audioDummyOutput[NEXUS_NUM_AUDIO_DUMMY_OUTPUTS];
#endif
} DI_AudioInstance;

typedef struct
{
	HBOOL			bMixerOpened;
	DI_AUDIO_MIXER_TYPE	nMixerType;
} DI_AudioMixerStatus;

typedef struct
{
#if defined(CONFIG_DI10)
#else
	DI_AUDIO_TYPE nType;
#endif
	DI_AUDIO_FORMAT nFormat;
	DI_AUDIO_STATUS nState;
	DI_AUDIO_RUNNING_STATE_EVT_t rState;
	DI_AUDIO_DECODER_MODE nDecoderMode;
#if defined(CONFIG_AUDIO_ERROR_LOG) 	/* for audio error log */
	DI_AUDIO_ERROR_INFO_EVT_t stAudErrInfo;
#endif
	HBOOL	bDecoderOpened;
	HBOOL	bDecoderMute;
	HBOOL	bDecoding;
	HBOOL	bDataComing;
	HBOOL	bLocked;

	NEXUS_AudioDecoderStartSettings audioSettings;
	NEXUS_AudioDecoderConnectorType audioConnectorType;

	HUINT32 eventLimit;
	HUINT32 prevPts;

	unsigned last_depth;
}DI_AudioStatus;

typedef struct
{
	HBOOL	bHdmiTranscoding;
	HBOOL	bSpdifTranscoding;
	DI_AUDIO_FORMAT eHdmiTransFormat;
	DI_AUDIO_FORMAT eSpdifTransFormat;
}DI_AudioTransInfo_t;

typedef struct
{
	HBOOL	bDownMixing;

	HBOOL	bConnectedAc3EncoderInput;
	HBOOL	bConnectedDtsEncoderInput;

	HBOOL	bI2sMute;
	HBOOL	bAnalogMute;
	HBOOL	bSpdifMute;
	HBOOL	bHdmiMute;
	HBOOL	bVCRScartMute;
	HBOOL	bForceDownmix;
	HBOOL	bHdmiDummyPlayed;
	HBOOL	bForceOutputPath;

	HBOOL	bI2sPath;
	DI_AUDIO_SPDIF_MODE eSpdifMode;
	DI_AUDIO_HDMI_MODE eHdmiMode;
	DI_AUDIO_HDMI_MODE ePrivHdmiMode;

	NEXUS_AudioDacHandle 	audioDacs[NEXUS_NUM_AUDIO_DACS];
	NEXUS_SpdifOutputHandle	spdifOutputs[NEXUS_NUM_SPDIF_OUTPUTS];
	NEXUS_I2sOutputHandle	i2sOutputs[NEXUS_NUM_I2S_OUTPUTS];

	HUINT32 audioDacPortNum;
	HUINT32 spdifOutPortNum;
	HUINT32 i2sOutPortNum;

	HUINT32 ulAnalogVolume;
	HUINT32 ulSpdifVolume;
	HUINT32 ulHdmiVolume;
	HUINT32 ulI2sVolume;

	NEXUS_AudioVolumeType volumeType;

	HBOOL	bI2sOutputPathConnected;
	HBOOL	bDacOutputPathConnected;
	HBOOL	bHdmiOutputPathConnected;
	HBOOL	bSpdifOutputPathConnected;
	HBOOL	bBtCaptureOutputPathConnected;

	DI_AUDIO_INPUT_INFO eI2sInput;
	DI_AUDIO_INPUT_INFO eDacInput;
	DI_AUDIO_INPUT_INFO eHdmiInput;
	DI_AUDIO_INPUT_INFO eSpdifInput;

	DI_AudioTransInfo_t stTranscodingInfo;

} DI_AudioOutput;

#if defined(TEMP_HRA_SELF_TEST)
typedef struct
{
	HBOOL					bStart;
	NEXUS_FilePlayHandle	hFile;
	NEXUS_PlaypumpHandle	hPlayPump;
	NEXUS_PlaybackHandle	hPlayback;
} DI_HdAudio;
#endif

typedef struct
{
	NEXUS_AudioPlaybackHandle	gPlayback;
	HBOOL	bStart;
} DI_AudioPlayback;

typedef struct
{
	NEXUS_PlaypumpHandle 		gPlaypump;
	HBOOL	bStart;
} DI_AudioPlaypump;

typedef struct
{
	HUINT32 				ulDevId;
	HUINT32					ulPCMIndex;
	HBOOL 					bOccupied;
	HULONG 					s_ulAudioStreamTaskId;
	HULONG 					s_ulAudioStreamMsgId;
	DI_AudioStreamPlayInfo 	stAudioStreamInfo;
	unsigned char* pbuffStartPoint; /*Beep Sound data buffer */
}DI_AudioTaskInfo;

typedef struct
{
	HBOOL 						bOccupied;
	HUINT32 					ulTaskIndex;
	HUINT32 					ulRequestId;
	HUINT32						ulTimeTicket;
	DI_AUDIO_DECODER_TYPE		eDecoder;
} DI_Audio_MemoryStream_Pool;

typedef struct
{
	NEXUS_AudioCodec codec; 	/* Audio stream format */
	unsigned sampleRate;		/* In Hz */
	unsigned bitrate;
	HUINT32 ulAcmod;
}DI_Audio_Stream_Info;

typedef struct
{
	HBOOL bTsm;
	/*	BasePtsOffset is always effective at any streams.
	*/
	HUINT32 ulBasePtsOffset;
	/*	All kind of codec can have its own delay value.
	*/
	HUINT32 ulMpegPtsOffset;
	HUINT32 ulAc3PtsOffset;
	HUINT32 ulAc3plusPtsOffset;
	HUINT32 ulAacPtsOffset;
	HUINT32 ulAacplusPtsOffset;
	HINT32 nUserOffset;
}DI_Audio_Sync_Status;

static DI_AudioInstance				stAudioInstance;
static DI_AudioStatus				stAudioStatus[AUDIO_ALL_DECODERS];
static DI_AudioOutput				stAudioOutput;
static DI_AudioCallback				stAudioCallbacks[AUDIO_CALLBACK_OWNER_MAX];
static DI_AudioPlayback				stAudioPlayback[2];	/* Task Id dependent */
static DI_AudioPlaypump				stAudioPlaypump[2]; /* Task Id dependent */
static DI_AudioTaskInfo				stAudioTaskInfo[2];
static DI_Audio_MemoryStream_Pool	stMemoryStreamPool[2];
static DI_Audio_Stream_Info			stAudioStreamInfo;
static DI_Audio_Sync_Status			stAudioSyncStatus;
static DI_AudioMixerStatus			stAudioMixerStatus;
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DI_AudioMultiTrack_t			stMultiTrackInfo[AUDIO_ALL_DECODERS];
#endif

#if defined(TEMP_HRA_SELF_TEST)
static DI_HdAudio					stHdAudioInstance;
#endif

static DI_AUDIO_CAP_t g_AudioCap[AUDIO_ALL_DECODERS] =
{
//	decoder_id	decoder_type	supported_format    volume max value
	{0, 0, DI_AUDIO_DECODER_MAIN,	DI_AUDFORMAT_MPEG |DI_AUDFORMAT_MP3 |
									DI_AUDFORMAT_AAC | DI_AUDFORMAT_AAC_ADTS | DI_AUDFORMAT_AAC_LOAS |
									DI_AUDFORMAT_AAC_PLUS | DI_AUDFORMAT_AAC_PLUS_LOAS | DI_AUDFORMAT_AAC_PLUS_ADTS |
									DI_AUDFORMAT_AC3 | DI_AUDFORMAT_AC3_PLUS |
#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO) || defined(CONFIG_AUDIO_DEC_FLAC)
									DI_AUDFORMAT_FLAC |
#endif
									DI_AUDFORMAT_PCM ,
									AUDIO_LEVEL_MAX, AUDIO_PRIMER_MAX_COUNT},


	{0, 1, DI_AUDIO_DECODER_SUB,
									DI_AUDFORMAT_MPEG |DI_AUDFORMAT_MP3 |
									DI_AUDFORMAT_AAC | DI_AUDFORMAT_AAC_ADTS | DI_AUDFORMAT_AAC_LOAS |
									DI_AUDFORMAT_AAC_PLUS | DI_AUDFORMAT_AAC_PLUS_LOAS | DI_AUDFORMAT_AAC_PLUS_ADTS |
									DI_AUDFORMAT_AC3 | DI_AUDFORMAT_AC3_PLUS |
#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO) || defined(CONFIG_AUDIO_DEC_FLAC)
									DI_AUDFORMAT_FLAC |
#endif

									DI_AUDFORMAT_PCM,
									AUDIO_LEVEL_MAX, AUDIO_PRIMER_MAX_COUNT},

	{0, 2, DI_AUDIO_DECODER_SPDIF,	DI_AUDFORMAT_AC3, AUDIO_LEVEL_MAX, AUDIO_PRIMER_MAX_COUNT}
};

static unsigned long 		s_ulAudioDISemId;
static unsigned long 		s_ulAudioDRVSemId;
static unsigned long 		s_ulAudioPSemId;

static unsigned long 		s_ulAudioMemStreamId;

static unsigned long		s_ulAudioEventTaskId ;

static HUINT32				gTimeTicket;
static HBOOL				gIsVolumeAdjusted;

static HBOOL				s_HdmiInitialized = FALSE;

static HINT8				cMediaRunningState = DI_AUDIO_EVENT_NOTIFY_END;
static HUINT32				unCurrentPts = 0;

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
DRV_Error DRV_AUDIO_GetStreamInfo(HUINT32 ulDevId, DI_AUDIO_STREAM_INFO_t *ptAudioStreamInfo);

#if defined(CONFIG_HDMI_IN)
DRV_Error DRV_AUDIO_SetHdmiAudioInputToOutput(HUINT32 ulDevId, NEXUS_HdmiInputHandle hdmiInput);
DRV_Error DRV_AUDIO_UnsetHdmiAudioInputToOutput(HUINT32 ulDevId);
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_AUDIO_OpenPrimers(HUINT32 ulDevId);
static DRV_Error P_AUDIO_StartPrimers(HUINT32 ulDevId, HUINT32 ulPrimerIndex);
static DRV_Error P_AUDIO_StopPrimers(HUINT32 ulDevId, HUINT32 ulPrimerIndex);
static DRV_Error P_AUDIO_StopPrimerAndStartDecode(HUINT32 ulDevId, HUINT32 ulStartTrackIndex, DI_AUDIO_FORMAT nType);
static DRV_Error P_AUDIO_StopDecodeAndStartPrimer(HUINT32 ulDevId, HUINT32 ulStopTrackIndex);
#endif

#if __________p__________
#endif

#if defined(CONFIG_BLUETOOTH)
static DRV_Error P_AUDIO_AddBluetoothPath(void)
{
	NEXUS_Error nResult;
	NEXUS_AudioInput input = NULL;
	NEXUS_AudioOutput output = NULL;
	NEXUS_AudioOutputSettings nSettings;

	if((stAudioInstance.audioCapture[0] != NULL) && (stAudioInstance.audioMixer != NULL))
	{
		/*
		   (NCO index : capture 0, HDMI 1, DummyOutput 2)
		    Set to audio capture nco 0.
		    If NCO is not enough, Left outputs will be allcated to PLLs
		*/
		output = NEXUS_AudioCapture_GetConnector(stAudioInstance.audioCapture[0]);
		NEXUS_AudioOutput_GetSettings(output, &nSettings);
		nSettings.nco = NEXUS_AudioOutputNco_e0;
		nResult = NEXUS_AudioOutput_SetSettings(output, &nSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] %d NEXUS_AudioOutput_SetSettings failed!\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}

		/* Connect capture to Mixer  */
		input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
		output = NEXUS_AudioCapture_GetConnector(stAudioInstance.audioCapture[0]);
		nResult = NEXUS_AudioOutput_AddInput(output, input);
		if(nResult == NEXUS_NOT_SUPPORTED)
		{
			/*	When Netfilx is killed, Output resoure is not removed.
                for this case, try remove output and retry add input   */
			(void)NEXUS_AudioOutput_RemoveAllInputs(output);
			nResult = NEXUS_AudioOutput_AddInput(output, input);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioOutput_AddInput failed!\n");
				return DRV_ERR;
			}
		}
		else if	(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioMixer_AddInput failed!\n");
			return DRV_ERR;
		}

		stAudioOutput.bBtCaptureOutputPathConnected = TRUE;

		return DRV_OK;
	}
	else
	{
		PrintError("[%s] %d Mixer or Capture handle is NULL \n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
}

static DRV_Error P_AUDIO_RemoveBluetoothPath(void)
{
	NEXUS_AudioOutput		output;
	NEXUS_Error				nResult;

	if(stAudioOutput.bBtCaptureOutputPathConnected == TRUE)
	{
		output = NEXUS_AudioCapture_GetConnector(stAudioInstance.audioCapture[0]);
		nResult = NEXUS_AudioOutput_RemoveAllInputs(output);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_RemoveAllInputs failed!\n");
			return DRV_ERR;
		}

		stAudioOutput.bBtCaptureOutputPathConnected = FALSE;
	}
	else
	{
		PrintError("[%s] %d Capture already disconnected !!! \n", __FUNCTION__, __LINE__);
	}
	return DRV_OK;
}
#endif

static NEXUS_AudioInput P_AUDIO_GetInput(HUINT32 ulDevId, NEXUS_AudioDecoderConnectorType etype)
{//Sunwoo
	NEXUS_AudioInput	input=NULL;

	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		input = NEXUS_AudioDecoder_GetConnector(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], etype);
	}
	else if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		input = NEXUS_AudioDecoder_GetConnector(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB], etype);
	}
	else if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		input = NEXUS_AudioDecoder_GetConnector(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SPDIF], etype);
	}
	else
	{
		PrintError("Invalid ulDevid!: %d\n", ulDevId);
		return NULL;
	}
	stAudioStatus[ulDevId].audioConnectorType = etype;

	return input;

}

static NEXUS_AudioOutput P_AUDIO_GetOutput(HUINT32 ulOutput)
{
	DRV_Error drvResult = DRV_OK;
	NEXUS_AudioOutput 	output = NULL;
	NEXUS_HdmiOutputHandle hHdmi = NULL;

	if(ulOutput == DI_AUDIO_OUTPUT_DAC)
	{
		output = NEXUS_AudioDac_GetConnector(stAudioOutput.audioDacs[stAudioOutput.audioDacPortNum]);
	}
	else if(ulOutput == DI_AUDIO_OUTPUT_SPDIF)
	{
		output = NEXUS_SpdifOutput_GetConnector(stAudioOutput.spdifOutputs[stAudioOutput.spdifOutPortNum]);
	}
	else if(ulOutput == DI_AUDIO_OUTPUT_HDMI)
	{
		drvResult = DRV_HDMI_GetHdmiOutputHandle(&hHdmi);
		if(drvResult == DRV_OK)
		{
			output = NEXUS_HdmiOutput_GetAudioConnector(hHdmi);
		}
		else
		{
			PrintError("[%s] DRV_HDMI_GetHdmiOutputHandle error %d \n", __FILE__, drvResult);
			output = NULL;
		}
	}
	else if(ulOutput == DI_AUDIO_OUTPUT_I2S)
	{
		output = NEXUS_I2sOutput_GetConnector(stAudioOutput.i2sOutputs[stAudioOutput.i2sOutPortNum]);
	}
	return output;
}

#if 0 //not used
static HUINT32 P_AUDIO_GetRequestIdFromTaskId(HUINT32 ulTaskId)
{//Sunwoo
	HUINT32 ulRequestId;
	if(stMemoryStreamPool[0].ulTaskIndex == ulTaskId)
	{
		ulRequestId = stMemoryStreamPool[0].ulRequestId;
	}
	else if(stMemoryStreamPool[1].ulTaskIndex == ulTaskId)
	{
		ulRequestId = stMemoryStreamPool[1].ulRequestId;
	}
	else
	{
		PrintError("The Task#%d is not busy!\n", ulTaskId);
		ulRequestId = 99;
	}
	return ulRequestId;
}
#endif

static HBOOL P_AUDIO_CheckEmptyCallbackIndex(HUINT32 ulId)
{//Sunwoo
	HUINT32 nEvent, nDecoder;
	HBOOL bEmpty = TRUE;

	for(nDecoder=0; nDecoder< AUDIO_ALL_DECODERS; nDecoder++)
	{
		for(nEvent=0; nEvent < AUDIO_EVENT_NUM; nEvent ++)
		{
			if(stAudioCallbacks[ulId].audioEventCallback[nDecoder][nEvent] != NULL)
			{
				bEmpty = FALSE;
			}
		}
	}
	return bEmpty;
}

static HBOOL P_AUDIO_CheckSPDIFStart(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{//Sunwoo
	/*	This Function makes a decision that SPDIF decoder should be started or not.
	*	If DD Downmixing is supported & at COMPRESSED mode, it should start SPDIF decoder internally.
	*	Only at following two cases, SPDIF decoder should be started.
	*	1. AC3 & Compressed mode.
	*	2. AC3Plus & Compressed mode && HDMI hotplug is positive.
	*/
	HBOOL bStart = FALSE;

#if defined(CONFIG_DI10)
	if(nType >= DI_AUDFORMAT_MEDIA_MPEG)
	{
		nType &= 0x0fffffff;
	}
#endif
	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		if(stAudioOutput.bDownMixing == TRUE)
		{
#if defined(CONFIG_JAPAN_SPEC)
			BSTD_UNUSED(nType);
			if(nType == DI_AUDFORMAT_NONE)
			{
				bStart = FALSE;
			}
			else
			{
				if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_BYPASS)
				{
					bStart = TRUE;
				}

				if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
				{
					bStart = TRUE;
				}
			}
#else
			if(nType == DI_AUDFORMAT_AC3)
			{
#if defined(CONFIG_DI10)
				if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_COMPRESSED)
#else
				if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_BYPASS)
#endif
				{
					bStart = TRUE;
				}

				if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
				{
					bStart = TRUE;
				}
			}

			else if((nType == DI_AUDFORMAT_AC3_PLUS) ||
				(nType == DI_AUDFORMAT_AAC_PLUS) ||
				(nType == DI_AUDFORMAT_AAC_PLUS_LOAS) ||
				(nType == DI_AUDFORMAT_AAC_PLUS_ADTS) ||
				(nType == DI_AUDFORMAT_AAC) ||
				(nType == DI_AUDFORMAT_AAC_LOAS))
			{
				if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
				{
					bStart = TRUE;
				}
			}
			else
			{
				bStart = FALSE;
			}
#endif
		}
	}

	return bStart;
}

static HBOOL P_AUDIO_CheckAllStopped(void)
{//Sunwoo
	/*	This function returns TRUE only when all the decoders and playback and playpump are stopped.
	*	The targets are Main Decoder, Sub Decoder, SPDIF Decoder,
	*	Playpump1, Playpump2, AudioPlayback1, AudioPlayback2.
	*/
	HBOOL bAllStopped = TRUE;

	/*	1. Decoder Check
	*/
	if((stAudioStatus[DI_AUDIO_DECODER_MAIN].nState != DI_AUDIO_STOP) ||
	  (stAudioStatus[DI_AUDIO_DECODER_SUB].nState != DI_AUDIO_STOP) ||
	  (stAudioStatus[DI_AUDIO_DECODER_SPDIF].nState != DI_AUDIO_STOP))
	{
		bAllStopped = FALSE;
	}

	/*	2. Playpump Check
	*/
	if((stAudioPlaypump[0].bStart == TRUE) || (stAudioPlaypump[1].bStart == TRUE))
	{
		bAllStopped = FALSE;
	}

	/*	3. AudioPlayback Check
	*/
	if((stAudioPlayback[0].bStart == TRUE) || (stAudioPlayback[1].bStart == TRUE))
	{
		bAllStopped = FALSE;
	}

	return bAllStopped;

}

static NEXUS_AudioCodec P_AUDIO_ConvertCodecToNexus(DI_AUDIO_FORMAT nType)
{
	NEXUS_AudioCodec nexusCodec = NEXUS_AudioCodec_eUnknown;

	switch (nType)
	{
		case DI_AUDFORMAT_MPEG:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_MPEG:
#endif
			nexusCodec = NEXUS_AudioCodec_eMpeg;
			break;

		case DI_AUDFORMAT_MP3:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_MP3:
#endif
			nexusCodec = NEXUS_AudioCodec_eMp3;
			break;

		case DI_AUDFORMAT_AAC: // same as DI_AUDFORMAT_AAC_ADTS
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_AAC:
#endif
			nexusCodec = NEXUS_AudioCodec_eAac;
			break;

		case DI_AUDFORMAT_AAC_LOAS:
			nexusCodec = NEXUS_AudioCodec_eAacLoas;
			break;

		case DI_AUDFORMAT_AAC_PLUS:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_AAC_PLUS:
#endif
			nexusCodec = NEXUS_AudioCodec_eAacPlus; //load???? adts????
			break;

#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_AAC_PLUS_LOAS:
		case DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS:
			nexusCodec = NEXUS_AudioCodec_eAacPlusLoas; //load???? adts????
			break;
#endif

		case DI_AUDFORMAT_AAC_PLUS_ADTS:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS:
#endif
			nexusCodec = NEXUS_AudioCodec_eAacPlusAdts;
			break;

		case DI_AUDFORMAT_AC3:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_AC3:
#endif
			nexusCodec = NEXUS_AudioCodec_eAc3;
			break;

		case DI_AUDFORMAT_AC3_PLUS:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_AC3_PLUS:
#endif
			nexusCodec = NEXUS_AudioCodec_eAc3Plus;
			break;

		case DI_AUDFORMAT_PCM:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_PCM:
#endif
			nexusCodec = NEXUS_AudioCodec_eLpcmDvd;//pcm???
			break;

		case DI_AUDFORMAT_DTS:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_DTS:
#endif
			nexusCodec = NEXUS_AudioCodec_eDts;
			break;

		case DI_AUDFORMAT_LPCM_HD_DVD:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_LPCM_HD_DVD:
#endif
			nexusCodec = NEXUS_AudioCodec_eLpcmHdDvd;
			break;

		case DI_AUDFORMAT_LPCM_BLUERAY:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_LPCM_BLUERAY:
#endif
			nexusCodec = NEXUS_AudioCodec_eLpcmBluRay;//pcm???
			break;

		case DI_AUDFORMAT_DTS_HD:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_DTS_HD:
#endif
			nexusCodec = NEXUS_AudioCodec_eDtsHd; //load???? adts????
			break;

		case DI_AUDFORMAT_WMA_STD:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_WMA_STD:
#endif
			nexusCodec = NEXUS_AudioCodec_eWmaStd;
			break;

		case DI_AUDFORMAT_WMA_PRO:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_WMA_PRO:
#endif
			nexusCodec = NEXUS_AudioCodec_eWmaPro;
			break;

		case DI_AUDFORMAT_LPCM_DVD:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_LPCM_DVD:
#endif
			nexusCodec = NEXUS_AudioCodec_eLpcmDvd;//pcm???
			break;

		case DI_AUDFORMAT_PCMWAVE:
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_PCMWAVE:
#endif
			nexusCodec = NEXUS_AudioCodec_ePcmWav;
			break;

		case DI_AUDFORMAT_FLAC:
			nexusCodec = NEXUS_AudioCodec_eFlac;
			break;

		case DI_AUDFORMAT_ADPCM:
			nexusCodec = NEXUS_AudioCodec_eAdpcm;
			break;

		case DI_AUDFORMAT_VORBIS:
			nexusCodec = NEXUS_AudioCodec_eVorbis;
			break;
#if defined(CONFIG_AUDIO_DEC_OPUS)
		case DI_AUDFORMAT_OPUS:
			nexusCodec = NEXUS_AudioCodec_eOpus;
			break;
#endif
		case DI_AUDFORMAT_APE:
			nexusCodec = NEXUS_AudioCodec_eApe;
			break;

		default:
			nexusCodec = NEXUS_AudioCodec_eUnknown;
			break;
	}

	return nexusCodec;
}

#if 0 // not used
static DI_AUDIO_FORMAT P_AUDIO_ConvertCodecToDi(NEXUS_AudioCodec nType)
{
	DI_AUDIO_FORMAT diCodec = 0;

	switch (nType)
	{
		case NEXUS_AudioCodec_eMpeg:
			diCodec = DI_AUDFORMAT_MPEG;
			break;

		case NEXUS_AudioCodec_eMp3:
			diCodec = DI_AUDFORMAT_MP3;
			break;

		case NEXUS_AudioCodec_eAac:
			diCodec = DI_AUDFORMAT_AAC;
			break;

		case NEXUS_AudioCodec_eAacLoas:
			diCodec = DI_AUDFORMAT_AAC_LOAS;
			break;

		case NEXUS_AudioCodec_eAacPlus:
			diCodec = DI_AUDFORMAT_AAC_PLUS;
			break;

		case NEXUS_AudioCodec_eAacPlusAdts:
			diCodec = DI_AUDFORMAT_AAC_PLUS_ADTS;
			break;

		case NEXUS_AudioCodec_eAc3:
			diCodec = DI_AUDFORMAT_AC3;
			break;

		case NEXUS_AudioCodec_eAc3Plus:
			diCodec = DI_AUDFORMAT_AC3_PLUS;
			break;

		case NEXUS_AudioCodec_eLpcmDvd:
		case NEXUS_AudioCodec_eLpcmHdDvd:
		case NEXUS_AudioCodec_eLpcmBluRay:
			diCodec = DI_AUDFORMAT_PCM;
			break;

		case NEXUS_AudioCodec_eDts:
		case NEXUS_AudioCodec_eDtsHd:
		case NEXUS_AudioCodec_eWmaStd:
		case NEXUS_AudioCodec_eWmaPro:
		case NEXUS_AudioCodec_eAvs:
		default:
			diCodec = 0;
			break;
	}

	return diCodec;

}
#endif

#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
static DI_MEDIA_AUDIO_FORMAT_e  P_AUDIO_ConvertCodecToMedia(DI_AUDIO_FORMAT nType)
{//Sunwoo
	DI_MEDIA_AUDIO_FORMAT_e 	eMediaAudioCodec;
	switch(nType)
	{
#if defined(CONFIG_DI10)
		case DI_AUDFORMAT_MEDIA_MPEG:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_MPEG;
			break;

		case DI_AUDFORMAT_MEDIA_MP3:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_MP3;
			break;

		case DI_AUDFORMAT_MEDIA_AAC:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC;
			break;

		case DI_AUDFORMAT_MEDIA_AAC_PLUS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS;	//load???? adts????
			break;

		case DI_AUDFORMAT_MEDIA_AC3:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AC3;
			break;

		case DI_AUDFORMAT_MEDIA_AC3_PLUS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AC3_PLUS;
			break;

		case DI_AUDFORMAT_MEDIA_PCM:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_PCM;//pcm???
			break;

		case DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_ADTS;
			break;

		case DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_LOAS;
			break;

		case DI_AUDFORMAT_MEDIA_DTS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_DTS;
			break;

		case DI_AUDFORMAT_MEDIA_LPCM_HD_DVD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_LPCM_HD_DVD;
			break;

		case DI_AUDFORMAT_MEDIA_LPCM_BLUERAY:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_LPCM_BLUERAY;
			break;

		case DI_AUDFORMAT_MEDIA_DTS_HD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_DTS_HD;
			break;

		case DI_AUDFORMAT_MEDIA_WMA_STD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_WMA_STD;
			break;

		case DI_AUDFORMAT_MEDIA_WMA_PRO:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_WMA_PRO;
			break;

		case DI_AUDFORMAT_MEDIA_LPCM_DVD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_LPCM_DVD;
			break;

		case DI_AUDFORMAT_MEDIA_PCMWAVE:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_PCMWAVE;
			break;

		case DI_AUDFORMAT_MEDIA_DRA:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_DRA;
			break;
#else
		case DI_AUDFORMAT_MPEG:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_MPEG;
			break;

		case DI_AUDFORMAT_MP3:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_MP3;
			break;

		case DI_AUDFORMAT_AAC:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC;
			break;
#if 0
		case DI_AUDFORMAT_AAC_ADTS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_ADTS;
			break;
#endif
		case DI_AUDFORMAT_AAC_LOAS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_LOAS;
			break;

		case DI_AUDFORMAT_AAC_PLUS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS;
			break;
#if 0
		case DI_AUDFORMAT_AAC_PLUS_LOAS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_LOAS;
			break;
#endif
		case DI_AUDFORMAT_AAC_PLUS_ADTS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_ADTS;
			break;

		case DI_AUDFORMAT_AC3:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AC3;
			break;

		case DI_AUDFORMAT_AC3_PLUS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_AC3_PLUS;
			break;

		case DI_AUDFORMAT_PCM:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_PCM;//pcm???
			break;

		case DI_AUDFORMAT_DTS:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_DTS;
			break;

		case DI_AUDFORMAT_LPCM_HD_DVD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_LPCM_HD_DVD;
			break;

		case DI_AUDFORMAT_LPCM_BLUERAY:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_LPCM_BLUERAY;
			break;

		case DI_AUDFORMAT_DTS_HD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_DTS_HD;
			break;

		case DI_AUDFORMAT_WMA_STD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_WMA_STD;
			break;

		case DI_AUDFORMAT_WMA_PRO:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_WMA_PRO;
			break;

		case DI_AUDFORMAT_LPCM_DVD:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_LPCM_DVD;
			break;

		case DI_AUDFORMAT_PCMWAVE:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_PCMWAVE;
			break;

		case DI_AUDFORMAT_FLAC:
			 eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_FLAC;
			 break;

		case DI_AUDFORMAT_ADPCM:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_ADPCM;
			break;

		case DI_AUDFORMAT_VORBIS:
			 eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_VORBIS;
			 break;
#if defined(CONFIG_AUDIO_DEC_OPUS)
		case DI_AUDFORMAT_OPUS:
			 eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_OPUS;
			 break;
#endif
		case DI_AUDFORMAT_APE:
			 eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_APE;
			 break;
#endif

		default:
			eMediaAudioCodec = DI_MEDIA_AUDIO_FORMAT_NONE;
			break;
	}

	return eMediaAudioCodec;

}
#endif

static DI_AUDIO_MODE P_AUDIO_ConvertCodecModeToDi(NEXUS_AudioCodec nType, HUINT32 ulMode )
{
	DI_AUDIO_MODE diMode = 0;

	switch(nType)
	{
		case NEXUS_AudioCodec_eAc3:
		case NEXUS_AudioCodec_eAc3Plus:
			switch(ulMode)
			{
				case NEXUS_AudioAc3Acmod_eTwoMono_1_ch1_ch2:
    			case NEXUS_AudioAc3Acmod_eOneCenter_1_0_C:
					diMode = DI_AUDIO_MODE_MONO;
					break;
    			case NEXUS_AudioAc3Acmod_eTwoChannel_2_0_L_R:
					diMode = DI_AUDIO_MODE_STEREO;
					break;
    			case NEXUS_AudioAc3Acmod_eThreeChannel_3_0_L_C_R:
    			case NEXUS_AudioAc3Acmod_eThreeChannel_2_1_L_R_S:
    			case NEXUS_AudioAc3Acmod_eFourChannel_3_1_L_C_R_S:
    			case NEXUS_AudioAc3Acmod_eFourChannel_2_2_L_R_SL_SR:
    			case NEXUS_AudioAc3Acmod_eFiveChannel_3_2_L_C_R_SL_SR:
					diMode = DI_AUDIO_MODE_MUTI_STEREO;
					break;
				default:
					diMode = DI_AUDIO_MODE_UNKNOWN;
					break;
			}
			break;
		case NEXUS_AudioCodec_eMpeg:
		case NEXUS_AudioCodec_eMp3:
			switch(ulMode)
			{
				case NEXUS_AudioMpegChannelMode_eStereo:
				case NEXUS_AudioMpegChannelMode_eIntensityStereo:
					diMode = DI_AUDIO_MODE_STEREO;
					break;
				case NEXUS_AudioMpegChannelMode_eDualChannel:
					diMode = DI_AUDIO_MODE_DUAL_MONO;
					break;
				case NEXUS_AudioMpegChannelMode_eSingleChannel:
					diMode = DI_AUDIO_MODE_MONO;
					break;
				default:
					diMode = DI_AUDIO_MODE_UNKNOWN;
					break;
			}
			break;
		case NEXUS_AudioCodec_eAac:
		case NEXUS_AudioCodec_eAacLoas:
		case NEXUS_AudioCodec_eAacPlus:
		case NEXUS_AudioCodec_eAacPlusAdts:
			switch(ulMode)
			{
				case NEXUS_AudioAacAcmod_eTwoMono_1_ch1_ch2:
					diMode = DI_AUDIO_MODE_DUAL_MONO;
					break;
				case NEXUS_AudioAacAcmod_eOneCenter_1_0_C:
					diMode = DI_AUDIO_MODE_MONO;
					break;
				case NEXUS_AudioAacAcmod_eTwoChannel_2_0_L_R:
					diMode = DI_AUDIO_MODE_STEREO;
					break;
				case NEXUS_AudioAacAcmod_eThreeChannel_3_0_L_C_R:
				case NEXUS_AudioAacAcmod_eThreeChannel_2_1_L_R_S:
				case NEXUS_AudioAacAcmod_eFourChannel_3_1_L_C_R_S:
				case NEXUS_AudioAacAcmod_eFourChannel_2_2_L_R_SL_SR:
				case NEXUS_AudioAacAcmod_eFiveChannel_3_2_L_C_R_SL_SR:
					diMode = DI_AUDIO_MODE_MUTI_STEREO;
					break;
				default:
					diMode = DI_AUDIO_MODE_UNKNOWN;
					break;
			}
			break;
		default:
			diMode = DI_AUDIO_MODE_UNKNOWN;
			break;
	}
	return diMode;

}

static DRV_Error P_AUDIO_SetTsmMode(HBOOL bTsm)
{//Sunwoo
	HINT32 vResult;

	vResult = VK_SEM_Get(s_ulAudioPSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}
	stAudioSyncStatus.bTsm = bTsm;

	VK_SEM_Release(s_ulAudioPSemId);
	return DRV_OK;
}

static DRV_Error P_AUDIO_SetVolume(HUINT32 ulOutput, HUINT32 ulVolume)//HUINT32 ulDevId, HUINT32 ulVolume)
{//Sunwoo
	NEXUS_Error nResult = NEXUS_SUCCESS;
	NEXUS_AudioOutput output;
	NEXUS_AudioOutputSettings nSettings;

	output = P_AUDIO_GetOutput(ulOutput);
	NEXUS_AudioOutput_GetSettings(output, &nSettings);

	nSettings.volumeType = NEXUS_AudioVolumeType_eDecibel;
	if(ulVolume == 0)
	{
		nSettings.leftVolume = AUDIO_MIN_DECIBEL;
		nSettings.rightVolume = AUDIO_MIN_DECIBEL;
	}
	else
	{
		nSettings.leftVolume = AUDIO_VOLUME_UNIT*(AUDIO_LEVEL_MAX-ulVolume);
		nSettings.rightVolume = AUDIO_VOLUME_UNIT*(AUDIO_LEVEL_MAX-ulVolume);
		if(ulOutput == DI_AUDIO_OUTPUT_DAC)
		{
			/* 40nm chipset level difference soulution */
			/*Down -40 (0.4DB) : -200 is 2DB. */
			/*  different level for new dac */
			nSettings.leftVolume -= DAC_VOLUME_MODIFICATION;
			nSettings.rightVolume -= DAC_VOLUME_MODIFICATION;

		}

		/* LoudnessMatching : EBU-R128
		This standardizes Dolby decoders to output Stereo and Multichannel PCM at -20dB.
		Non-Dolby decoders will output Stereo and Multichannel PCM at -23dB.
		All encoders and passthrough configurations will output compressed at -31dB.

		The application must set the volume for outputs accordingly so
		that PCM stereo is output to "active" outputs such as DAC at -23 dB
		and PCM is sent to "passive" outputs such as SPDIF at -31dB.

		This can be done by calling NEXUS_AudioOutput_SetSettings() to program
		the volume for active active outputs with the value 0x5A9DF7 (-3dB)
		and passive outputs with the value 0x241346 (-11dB) for Dolby codecs.

		For non-Dolby codecs, use the value 0x800000 (-0dB) for active outputs
		and 0x32F52C (-8dB) for passive outputs.

		*/

		if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AC3 ||
			stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AC3_PLUS ||
			stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC ||
			stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_ADTS ||
			stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_LOAS ||
			stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_PLUS ||
			stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_PLUS_LOAS ||
			stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_PLUS_ADTS)
#if defined(CONFIG_JAPAN_SPEC)
		{
			/*Dolby decoders*/
			if(ulOutput == DI_AUDIO_OUTPUT_DAC || ulOutput == DI_AUDIO_OUTPUT_HDMI || ulOutput == DI_AUDIO_OUTPUT_SPDIF)
			{
				/* Active outputs */
				nSettings.leftVolume -= (3*100);
				nSettings.rightVolume -= (3*100);
			}
		}
#else
		{
			/*Dolby decoders*/
			if(ulOutput == DI_AUDIO_OUTPUT_DAC || ulOutput == DI_AUDIO_OUTPUT_HDMI)
			{
				/* Active outputs */
				nSettings.leftVolume -= (3*100);
				nSettings.rightVolume -= (3*100);
			}

			if(ulOutput == DI_AUDIO_OUTPUT_SPDIF)
			{
				/* Passive output */
				nSettings.leftVolume -= (11*100);
				nSettings.rightVolume -= (11*100);
			}
		}
		else
		{
			/*Non Dolby decoders*/
			if(ulOutput == DI_AUDIO_OUTPUT_DAC || ulOutput == DI_AUDIO_OUTPUT_HDMI)
			{
				/* Active outputs */
			}

			if(ulOutput == DI_AUDIO_OUTPUT_SPDIF)
			{
				/* Passive output */
				nSettings.leftVolume -= (8*100);
				nSettings.rightVolume -= (8*100);
			}

		}

		if((nSettings.leftVolume > AUDIO_MAX_DECIBEL) || (nSettings.rightVolume > AUDIO_MAX_DECIBEL))
		{
			nSettings.leftVolume = AUDIO_MAX_DECIBEL;
			nSettings.rightVolume = AUDIO_MAX_DECIBEL;
		}

		if((nSettings.leftVolume < AUDIO_MIN_DECIBEL) || (nSettings.rightVolume < AUDIO_MIN_DECIBEL))
		{
			nSettings.leftVolume = AUDIO_MIN_DECIBEL;
			nSettings.rightVolume = AUDIO_MIN_DECIBEL;
		}
#endif /* defined(CONFIG_JAPAN_SPEC) */
	}

	nResult = NEXUS_AudioOutput_SetSettings(output, &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioOutput_SetSettings failed!\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

#if 0 // not used
static DRV_Error P_AUDIO_SetDecoderMute(HUINT32 ulDevId, HBOOL mute)
{//Sunwoo
	NEXUS_AudioDecoderSettings	dSettings;
	NEXUS_Error nResult;

	/*	Decoder Mute Function ***********
	*	This Function make a specific Decoder Mute on/off
	*	We have three decoders (Main/SUB/SPDIF)
	*********************************************/

	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &dSettings);

	dSettings.muted = mute;

	nResult = NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &dSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_SetSettings failed!\n");
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

static DRV_Error P_AUDIO_SetOutputMute(HUINT32 ulOutput, HBOOL mute)
{//Sunwoo
	NEXUS_AudioOutputSettings nSettings;
	NEXUS_Error nResult;
	NEXUS_AudioOutput output;
	HINT32	vResult;

	PrintDebug("[%s] %d ulOutput=%d mute=%d \n", __FUNCTION__, __LINE__, ulOutput, mute);

	vResult = VK_SEM_Get(s_ulAudioPSemId);
	if(vResult != VK_OK)
	{
		PrintError("[%s] %d VK_SEM_Get failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	/*	Output Mute Function ***********
	*	This Function make a specific Output Mute on/off
	*	We have three outputs (DAC/SPDIF/HDMI)
	********************************************/
	output = P_AUDIO_GetOutput(ulOutput);
	NEXUS_AudioOutput_GetSettings(output, &nSettings);

	nSettings.muted = mute;

	nResult = NEXUS_AudioOutput_SetSettings(output, &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d NEXUS_AudioOutput_SetSettings failed!\n", __FUNCTION__, __LINE__);
		goto err_reurn;
	}

	VK_SEM_Release(s_ulAudioPSemId);
	return DRV_OK;

err_reurn:
	VK_SEM_Release(s_ulAudioPSemId);
	return DRV_ERR;
}

static DRV_Error P_AUDIO_SetTranscodingPath(HUINT32 ulDevId, DI_AUDIO_OUTPUT eOutput, DI_AUDIO_FORMAT nType)
{//Sunwoo
	NEXUS_Error nResult;
	NEXUS_AudioInput input = NULL;
	NEXUS_AudioOutput output = NULL;
	HBOOL bRunTranscoding = FALSE;
	DI_AUDIO_FORMAT	TransOutFormat = DI_AUDFORMAT_NONE;

	if(eOutput == DI_AUDIO_OUTPUT_HDMI)
	{
		bRunTranscoding = stAudioOutput.stTranscodingInfo.bHdmiTranscoding;
		TransOutFormat = stAudioOutput.stTranscodingInfo.eHdmiTransFormat;
	}
	else if(eOutput == DI_AUDIO_OUTPUT_SPDIF)
	{
		bRunTranscoding = stAudioOutput.stTranscodingInfo.bSpdifTranscoding;
		TransOutFormat = stAudioOutput.stTranscodingInfo.eSpdifTransFormat;
	}
	else
	{
		PrintError("P_AUDIO_SetTranscodingPath() failed! eOutput=%d\n", eOutput);
		return DRV_ERR;
	}

	if(bRunTranscoding == FALSE)
	{
		PrintError("Transcoding is off\n");
		return DRV_OK;
	}

	/*	!!!!!!!!NOTICE!!!!!!!!
	*	DDP transcoding to DD should not use AC3 encoder.
	*	It only can be implemented by using a pcm decoder with compressed option.
	*******************************************************************/

	if(TransOutFormat == DI_AUDFORMAT_DTS)
	{
		/* Encoder input should be connected from one resource,
		and transcding support input resource from only main decoder */
		if(stAudioOutput.bConnectedDtsEncoderInput == FALSE)
		{
			input = NEXUS_AudioDecoder_GetConnector(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN],
													NEXUS_AudioDecoderConnectorType_eMultichannel);
			nResult = NEXUS_DtsEncode_AddInput(stAudioInstance.dtsEncoder, input);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_DtsEncode_AddInput failed!\n");
				return DRV_ERR;
			}
			else
			{
				stAudioStatus[ulDevId].audioConnectorType = NEXUS_AudioDecoderConnectorType_eMultichannel;
				stAudioOutput.bConnectedDtsEncoderInput = TRUE;
			}
		}
		input = NEXUS_DtsEncode_GetConnector(stAudioInstance.dtsEncoder);
	}
	else if(TransOutFormat == DI_AUDFORMAT_AC3)
	{
		if(nType == DI_AUDFORMAT_AC3_PLUS
#if defined(CONFIG_DI10)
		|| nType == DI_AUDFORMAT_MEDIA_AC3_PLUS
#endif
		)
		{
			input = P_AUDIO_GetInput(DI_AUDIO_DECODER_MAIN, NEXUS_AudioDecoderConnectorType_eCompressed);
			stAudioStatus[ulDevId].audioConnectorType = NEXUS_AudioDecoderConnectorType_eCompressed;
		}
		else if((nType == DI_AUDFORMAT_AAC_PLUS)||
			(nType == DI_AUDFORMAT_AAC_PLUS_LOAS)||
			(nType == DI_AUDFORMAT_AAC_PLUS_ADTS) ||
			(nType == DI_AUDFORMAT_AAC) ||
			(nType == DI_AUDFORMAT_AAC_LOAS)
#if defined(CONFIG_DI10)
			|| (nType == DI_AUDFORMAT_MEDIA_AAC_PLUS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC)
#endif
			)
		{
			/* Encoder input should be connected from one resource,
			and transcding support input resource from only main decoder */
			if(stAudioOutput.bConnectedAc3EncoderInput == FALSE)
			{
				input = NEXUS_AudioDecoder_GetConnector(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN],
														NEXUS_AudioDecoderConnectorType_eMultichannel);
				nResult = NEXUS_Ac3Encode_AddInput(stAudioInstance.ac3Encoder, input);
				if(nResult != NEXUS_SUCCESS)
				{
					PrintError("NEXUS_Ac3Encode_AddInput failed!\n");
					return DRV_ERR;
				}
				else
				{
					stAudioStatus[ulDevId].audioConnectorType = NEXUS_AudioDecoderConnectorType_eMultichannel;
					stAudioOutput.bConnectedAc3EncoderInput = TRUE;
				}
			}
			input = NEXUS_Ac3Encode_GetConnector(stAudioInstance.ac3Encoder);
		}
		else
		{
			PrintError("Only AAC, AAC+ and AC3+ can be transcoded to AC3!\n");
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("Transcoding Format is not prepared!\n");
		return DRV_ERR;
	}

	if(eOutput == DI_AUDIO_OUTPUT_SPDIF)
	{
		output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_SPDIF);
		stAudioOutput.bSpdifOutputPathConnected = TRUE;
	}
	else if(eOutput == DI_AUDIO_OUTPUT_HDMI)
	{
		output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);
		stAudioOutput.bHdmiOutputPathConnected = TRUE;
	}
	else
	{
		PrintError("Only Spdif, Hdmi Output can support Transcoding!\n");
		return DRV_ERR;
	}

	nResult = NEXUS_AudioOutput_AddInput(output, input);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioOutput_AddInput failed!\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_AUDIO_AddMixerInputPath(HUINT32 ulDevId)
{
	NEXUS_Error nResult;
	NEXUS_AudioInput input = NULL;

	if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		PrintError("Pass-through deocder cann't be connected to MIXER !!! \n");
		return DRV_ERR;
	}

	input = P_AUDIO_GetInput(ulDevId, NEXUS_AudioDecoderConnectorType_eStereo);
	nResult = NEXUS_AudioMixer_AddInput(stAudioInstance.audioMixer, input);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioMixer_AddInput failed!\n");
		return DRV_ERR;
	}
	PrintDebug("Decoder[%d]: MAIN decoder is connected with audioMixer!\n", ulDevId);
	stAudioStatus[ulDevId].audioConnectorType = NEXUS_AudioDecoderConnectorType_eStereo;

	return DRV_OK;
}

#if defined(TEMP_HRA_SELF_TEST) || defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
static DRV_Error P_AUDIO_AddI2sOutputPath(HUINT32 ulDevId)
{
	NEXUS_Error nResult;
	NEXUS_AudioInput input = NULL;
	NEXUS_AudioOutput output = NULL;

	BSTD_UNUSED(ulDevId);

	input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_I2S);
	nResult = NEXUS_AudioOutput_AddInput(output, input);
	if(nResult == NEXUS_NOT_SUPPORTED)
	{
		/*	When Netfilx is killed, Output resoure is not removed.
			for this case, try remove output and retry add input   */
		(void)NEXUS_AudioOutput_RemoveAllInputs(output);
		nResult = NEXUS_AudioOutput_AddInput(output, input);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_AddInput failed!\n");
			return DRV_ERR;
		}
	}
	else if	(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioMixer_AddInput failed!\n");
		return DRV_ERR;
	}

	stAudioOutput.bI2sOutputPathConnected = TRUE;
	stAudioOutput.eI2sInput = DI_AUDIO_INPUT_MIXER;
	PrintDebug("Decoder[%d]: audioMixer is connected to I2S Output!\n", ulDevId);

	return DRV_OK;
}
#endif
static DRV_Error P_AUDIO_AddDacOutputPath(HUINT32 ulDevId)
{
	NEXUS_Error nResult;
	NEXUS_AudioInput input = NULL;
	NEXUS_AudioOutput output = NULL;

	BSTD_UNUSED(ulDevId);

	input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_DAC);
	nResult = NEXUS_AudioOutput_AddInput(output, input);
	if(nResult == NEXUS_NOT_SUPPORTED)
	{
		/*	When Netfilx is killed, Output resoure is not removed.
			for this case, try remove output and retry add input   */
		(void) NEXUS_AudioOutput_RemoveAllInputs(output);
		nResult = NEXUS_AudioOutput_AddInput(output, input);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_AddInput failed!\n");
			return DRV_ERR;
		}
	}
	else if	(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioMixer_AddInput failed!\n");
		return DRV_ERR;
	}
	
	stAudioOutput.bDacOutputPathConnected = TRUE;
	stAudioOutput.eDacInput = DI_AUDIO_INPUT_MIXER;
	PrintDebug("Decoder[%d]: audioMixer is connected with DAC Output!\n", ulDevId);

	return DRV_OK;
}

static DRV_Error P_AUDIO_AddSpdifOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{//Sunwoo
	NEXUS_AudioInput		input = NULL;
	NEXUS_AudioOutput		output = NULL;
	NEXUS_Error 			nResult;
	DRV_Error				dResult;
	DI_AUDIO_INPUT_INFO eInputType = DI_AUDIO_INPUT_NONE;

	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
#if defined(CONFIG_DI10)
		if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_PCM)
#else
		if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_DOWNMIX)
#endif
		{
			input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
			output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_SPDIF);
			eInputType = DI_AUDIO_INPUT_MIXER;
		}
#if defined(CONFIG_DI10)
		else if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_COMPRESSED)
		{
			if((nType == DI_AUDFORMAT_AC3_PLUS)||
				(nType == DI_AUDFORMAT_AAC_PLUS)||
				(nType == DI_AUDFORMAT_AAC_PLUS_LOAS)||
				(nType == DI_AUDFORMAT_AAC_PLUS_ADTS)
#if defined(CONFIG_JAPAN_SPEC)
#else
				|| (nType == DI_AUDFORMAT_AAC) ||
				(nType == DI_AUDFORMAT_AAC_LOAS)
#endif
				|| (nType == DI_AUDFORMAT_AAC_LOAS) ||
				(nType == DI_AUDFORMAT_MEDIA_AC3_PLUS) ||
				(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS) ||
				(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS) ||
				(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS) ||
				(nType == DI_AUDFORMAT_MEDIA_AAC)
			)

#else
		else if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_TRANSCODING)
		{
#endif
			{
				dResult = P_AUDIO_SetTranscodingPath(ulDevId, DI_AUDIO_OUTPUT_SPDIF, nType);
				if(dResult != DRV_OK)
				{
					PrintError("P_AUDIO_SetTranscodingPath failed!\n");
					return DRV_ERR;
				}

				if(stAudioOutput.stTranscodingInfo.bSpdifTranscoding == TRUE)
				{
					eInputType = DI_AUDIO_INPUT_ENCODER;
				}
				else
				{
					eInputType = DI_AUDIO_INPUT_NONE;
				}
			}
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
#if defined(CONFIG_DI10)
		if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_PCM)
#else
		if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_DOWNMIX)
#endif
		{
			input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
			output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_SPDIF);
			eInputType = DI_AUDIO_INPUT_MIXER;
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
#if defined(CONFIG_DI10)
		if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_COMPRESSED)
		{
			if((nType != DI_AUDFORMAT_AC3_PLUS)&&
				(nType != DI_AUDFORMAT_AAC_PLUS)&&
				(nType != DI_AUDFORMAT_AAC_PLUS_LOAS)&&
				(nType != DI_AUDFORMAT_AAC_PLUS_ADTS)
#if defined(CONFIG_JAPAN_SPEC)
#else
				&& (nType != DI_AUDFORMAT_AAC) &&
				(nType != DI_AUDFORMAT_AAC_LOAS)
#endif
				&& (nType != DI_AUDFORMAT_MEDIA_AC3_PLUS) &&
				(nType != DI_AUDFORMAT_MEDIA_AAC_PLUS) &&
				(nType != DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS) &&
				(nType != DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS) &&
				(nType != DI_AUDFORMAT_MEDIA_AAC)
			)
#else
		if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_BYPASS)
		{
#endif
			{
				input = P_AUDIO_GetInput(DI_AUDIO_DECODER_SPDIF, NEXUS_AudioDecoderConnectorType_eCompressed);
				output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_SPDIF);
				stAudioStatus[ulDevId].audioConnectorType = NEXUS_AudioDecoderConnectorType_eCompressed;
				eInputType = DI_AUDIO_INPUT_DECODER;
			}
		}
	}

	if((input != NULL) && (output != NULL))
	{
		nResult = NEXUS_AudioOutput_AddInput(output, input);
		if(nResult == NEXUS_NOT_SUPPORTED)
		{
			/*	When Netfilx is killed, Output resoure is not removed.
				for this case, try remove output and retry add input   */
			(void)NEXUS_AudioOutput_RemoveAllInputs(output);
			nResult = NEXUS_AudioOutput_AddInput(output, input);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioOutput_AddInput failed!\n");
				return DRV_ERR;
			}
		}
		else if	(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_AddInput failed!\n");
			return DRV_ERR;
		}
		
		stAudioOutput.bSpdifOutputPathConnected = TRUE;
	}

	stAudioOutput.eSpdifInput = eInputType;

	return DRV_OK;
}


static DRV_Error P_AUDIO_AddHdmiOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{//Sunwoo
	NEXUS_AudioInput		input = NULL;
	NEXUS_AudioOutput		output = NULL;
	NEXUS_Error 			nResult;
	DRV_Error				dResult;
	DI_AUDIO_INPUT_INFO eInputType = DI_AUDIO_INPUT_NONE;
	NEXUS_AudioOutputSettings nSettings;

	if(stAudioOutput.bHdmiDummyPlayed == TRUE)
	{
		dResult = DRV_AUDIO_PlaybackDummyToHDMI(FALSE);
		if (dResult != DRV_OK)
		{
			PrintError("[%s] %d Can't stop PlaybackDummyToHDMI \n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
	}

	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_DOWNMIX)
		{
			input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
			output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);
			eInputType = DI_AUDIO_INPUT_MIXER;
		}
		else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
		{
			/* Nothing to do
			*/
		}
		else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_TRANSCODING)
		{
			/* Transcoding
			*/
			dResult = P_AUDIO_SetTranscodingPath(ulDevId, DI_AUDIO_OUTPUT_HDMI, nType);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_SetTranscodingPath failed!\n");
				return DRV_ERR;
			}

			if(stAudioOutput.stTranscodingInfo.bHdmiTranscoding == TRUE)
			{
				eInputType = DI_AUDIO_INPUT_ENCODER;
			}
			else
			{
				eInputType = DI_AUDIO_INPUT_NONE;
			}
		}

	}
	else if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_DOWNMIX)
		{
			input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
			output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);
			eInputType = DI_AUDIO_INPUT_MIXER;
		}
		else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
		{
			/* Nothing to do
			*/
		}
		else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_TRANSCODING)
		{
			/* Nothing to do
			*/
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_DOWNMIX)
		{
			/* Nothing to do
			*/
		}
		else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
		{
			input = P_AUDIO_GetInput(DI_AUDIO_DECODER_SPDIF, NEXUS_AudioDecoderConnectorType_eCompressed);
			output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);
			stAudioStatus[ulDevId].audioConnectorType = NEXUS_AudioDecoderConnectorType_eCompressed;
			eInputType = DI_AUDIO_INPUT_DECODER;
		}
		else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_TRANSCODING)
		{
			/* Nothing to do
			*/
		}
	}

	if((input != NULL) && (output != NULL))
	{
		/*
		   (NCO index : capture 0, HDMI 1, DummyOutput 2)
		    Set to audio capture nco 1.
		    If NCO is not enough, Left outputs will be allcated to PLLs
		*/
		output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);
		NEXUS_AudioOutput_GetSettings(output, &nSettings);
		nSettings.nco = NEXUS_AudioOutputNco_e1;
		nResult = NEXUS_AudioOutput_SetSettings(output, &nSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] %d NEXUS_AudioOutput_SetSettings failed!\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}

		nResult = NEXUS_AudioOutput_AddInput(output, input);
		if(nResult == NEXUS_NOT_SUPPORTED)
		{
			/*	When Netfilx is killed, Output resoure is not removed.
				for this case, try remove output and retry add input   */
			(void)NEXUS_AudioOutput_RemoveAllInputs(output);
			nResult = NEXUS_AudioOutput_AddInput(output, input);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioOutput_AddInput failed!\n");
				return DRV_ERR;
			}
		}
		else if	(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_AddInput failed!\n");
			return DRV_ERR;
		}
		
		stAudioOutput.bHdmiOutputPathConnected = TRUE;
	}

	stAudioOutput.eHdmiInput = eInputType;

	return DRV_OK;
}

#if 0
static DRV_Error P_AUDIO_RemoveMixerInputPath(HUINT32 ulDevId)
{
	NEXUS_AudioInput		input;
	NEXUS_Error				nResult;

	if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		PrintError("Pass-through deocder cann't be removed from MIXER !!! \n");
		return DRV_ERR;
	}

	if(stAudioStatus[ulDevId].bDecoderOpened == TRUE)
	{
		input = P_AUDIO_GetInput(ulDevId, NEXUS_AudioDecoderConnectorType_eStereo);
		nResult = NEXUS_AudioMixer_RemoveInput(	stAudioInstance.audioMixer, input);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioMixer_RemoveInput failed!\n");
			return DRV_ERR;
		}
	}

	return DRV_OK;
}
#endif

#if defined(TEMP_HRA_SELF_TEST) || defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
static DRV_Error P_AUDIO_RemoveI2sOutputPath(HUINT32 ulDevId)
{
	NEXUS_AudioOutput		output;
	NEXUS_Error				nResult;

	BSTD_UNUSED(ulDevId);

	if(stAudioOutput.bI2sOutputPathConnected == TRUE)
	{
		output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_I2S);
		nResult = NEXUS_AudioOutput_RemoveAllInputs(output);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_AudioOutput_RemoveAllInputs failed!\n", __FUNCTION__);
			return DRV_ERR;
		}

		stAudioOutput.bI2sOutputPathConnected = FALSE;
		stAudioOutput.eI2sInput = DI_AUDIO_INPUT_NONE;
	}
	else
	{
		PrintError("I2S output path is already disconnected !!! \n");
	}

	return DRV_OK;

}
#endif

static DRV_Error P_AUDIO_RemoveDacOutputPath(HUINT32 ulDevId)
{
	NEXUS_AudioOutput		output;
	NEXUS_Error				nResult;

	BSTD_UNUSED(ulDevId);

	if(stAudioOutput.bDacOutputPathConnected == TRUE)
	{
		output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_DAC);
		nResult = NEXUS_AudioOutput_RemoveAllInputs(output);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_RemoveAllInputs failed!\n");
			return DRV_ERR;
		}

		stAudioOutput.bDacOutputPathConnected = FALSE;
		stAudioOutput.eDacInput = DI_AUDIO_INPUT_NONE;
	}
	else
	{
		PrintError("DAC output path is already disconnected !!! \n");
	}

	return DRV_OK;

}

static DRV_Error P_AUDIO_RemoveSpdifOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{//Sunwoo
	NEXUS_AudioOutput		output = NULL;
	NEXUS_Error 			nResult;

	BSTD_UNUSED(ulDevId);

	if(stAudioOutput.bSpdifOutputPathConnected == FALSE)
	{
		PrintError("Spdif has no connection!!\n");
		return DRV_OK;
	}

	if(stAudioOutput.stTranscodingInfo.bSpdifTranscoding == TRUE)
	{
		if((nType == DI_AUDFORMAT_AC3_PLUS) ||
			(nType == DI_AUDFORMAT_AAC_PLUS) ||
			(nType == DI_AUDFORMAT_AAC_PLUS_LOAS) ||
			(nType == DI_AUDFORMAT_AAC_PLUS_ADTS) ||
			(nType == DI_AUDFORMAT_AAC) ||
			(nType == DI_AUDFORMAT_AAC_LOAS)
#if defined(CONFIG_DI10)
			|| (nType == DI_AUDFORMAT_MEDIA_AC3_PLUS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC)
#endif
		)
		{
			if(stAudioOutput.stTranscodingInfo.eSpdifTransFormat == DI_AUDFORMAT_DTS)
			{
				if(stAudioOutput.bConnectedDtsEncoderInput == TRUE)
				{
					nResult = NEXUS_DtsEncode_RemoveAllInputs(stAudioInstance.dtsEncoder);
					if(nResult != NEXUS_SUCCESS)
					{
						PrintError("NEXUS_DtsEncode_RemoveAllInputs failed!\n");
						return DRV_ERR;
					}
					else
					{
						stAudioOutput.bConnectedDtsEncoderInput = FALSE;
					}
				}
				/* Don't Close */
				/* NEXUS_DtsEncode_Close(stAudioInstance.dtsEncoder); */
			}
			else if(stAudioOutput.stTranscodingInfo.eSpdifTransFormat == DI_AUDFORMAT_AC3)
			{
				if((nType == DI_AUDFORMAT_AAC_PLUS)||
				(nType == DI_AUDFORMAT_AAC_PLUS_LOAS)||
				(nType == DI_AUDFORMAT_AAC_PLUS_ADTS)||
				(nType == DI_AUDFORMAT_AAC)||
				(nType == DI_AUDFORMAT_AAC_LOAS)
#if defined(CONFIG_DI10)
				|| (nType == DI_AUDFORMAT_MEDIA_AAC_PLUS) ||
				(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS) ||
				(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS) ||
				(nType == DI_AUDFORMAT_MEDIA_AAC)
#endif
				)
				{
					if(stAudioOutput.bConnectedAc3EncoderInput == TRUE)
					{
						nResult = NEXUS_Ac3Encode_RemoveAllInputs(stAudioInstance.ac3Encoder);
						if(nResult != NEXUS_SUCCESS)
						{
							PrintError("NEXUS_Ac3Encode_RemoveAllInputs failed!\n");
							return DRV_ERR;
						}
						else
						{
							stAudioOutput.bConnectedAc3EncoderInput = FALSE;
						}
					}
				}
			}
		}
	}

	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_SPDIF);

	if(output != NULL)
	{
		nResult = NEXUS_AudioOutput_RemoveAllInputs(output);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_RemoveAllInputs failed!\n");
			return DRV_ERR;
		}
		stAudioOutput.bSpdifOutputPathConnected = FALSE;
	}
	stAudioOutput.eSpdifInput = DI_AUDIO_INPUT_NONE;
	return DRV_OK;
}

static DRV_Error P_AUDIO_RemoveHdmiOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{//Sunwoo
	NEXUS_AudioOutput		output = NULL;
	NEXUS_Error 			nResult;

	BSTD_UNUSED(ulDevId);

	if(stAudioOutput.bHdmiOutputPathConnected == FALSE)
	{
		PrintDebug("Hdmi has no connection!!\n");
		return DRV_OK;
	}

	if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_TRANSCODING)
	{
		if(stAudioOutput.stTranscodingInfo.eHdmiTransFormat == DI_AUDFORMAT_DTS)
		{
			if(stAudioOutput.bConnectedDtsEncoderInput == TRUE)
			{
				nResult = NEXUS_DtsEncode_RemoveAllInputs(stAudioInstance.dtsEncoder);
				if(nResult != NEXUS_SUCCESS)
				{
					PrintError("NEXUS_DtsEncode_RemoveAllInputs failed!\n");
					return DRV_ERR;
				}
				else
				{
					stAudioOutput.bConnectedDtsEncoderInput = FALSE;
				}
			}
		}
		else if(stAudioOutput.stTranscodingInfo.eHdmiTransFormat == DI_AUDFORMAT_AC3)
		{
			if((nType == DI_AUDFORMAT_AAC_PLUS)||
			(nType == DI_AUDFORMAT_AAC_PLUS_LOAS)||
			(nType == DI_AUDFORMAT_AAC_PLUS_ADTS)||
			(nType == DI_AUDFORMAT_AAC)||
			(nType == DI_AUDFORMAT_AAC_LOAS)
#if defined(CONFIG_DI10)
			|| (nType == DI_AUDFORMAT_MEDIA_AAC_PLUS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_LOAS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS) ||
			(nType == DI_AUDFORMAT_MEDIA_AAC)
#endif
			)
			{
				if(stAudioOutput.bConnectedAc3EncoderInput == TRUE)
				{
					nResult = NEXUS_Ac3Encode_RemoveAllInputs(stAudioInstance.ac3Encoder);
					if(nResult != NEXUS_SUCCESS)
					{
						PrintError("NEXUS_Ac3Encode_RemoveAllInputs failed!\n");
						return DRV_ERR;
					}
					else
					{
						stAudioOutput.bConnectedAc3EncoderInput = FALSE;
					}
				}
			}
		}
	}

	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);

	if(output != NULL)
	{
		nResult = NEXUS_AudioOutput_RemoveAllInputs(output);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioOutput_RemoveAllInputs failed!\n");
			return DRV_ERR;
		}
		stAudioOutput.bHdmiOutputPathConnected = FALSE;
	}
	stAudioOutput.eHdmiInput = DI_AUDIO_INPUT_NONE;
	return DRV_OK;
}

static DRV_Error P_AUDIO_ChangeSpdifOutput(DI_AUDIO_SPDIF_MODE mode)
{//Sunwoo
	HUINT32				ulIndex;
	DRV_Error			dResult;
	DI_AUDIO_FORMAT		nCodec[AUDIO_ALL_DECODERS];
	HBOOL				bRestart[AUDIO_ALL_DECODERS];

	for(ulIndex=0; ulIndex<AUDIO_ALL_DECODERS; ulIndex++)
	{
		nCodec[ulIndex] = DI_AUDFORMAT_NONE;
		bRestart[ulIndex] = FALSE;
	}

	/*	1. Stop Decoder **********
	****************************/
	for(ulIndex=0; ulIndex<AUDIO_ALL_DECODERS; ulIndex++)
	{
		if(stAudioStatus[ulIndex].nState == DI_AUDIO_START)
		{
			bRestart[ulIndex] = TRUE;
			nCodec[ulIndex] = stAudioStatus[ulIndex].nFormat;

			dResult = DRV_AUDIO_Stop(ulIndex);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_AUDIO_Stop failed!\n");
				return DRV_ERR;
			}
		}
		else
		{
			bRestart[ulIndex] = FALSE;
		}
	}

	/*	2. Mode Change **********
	***************************/
	stAudioOutput.eSpdifMode = mode;

	/*	3. Start Decoder **********
	****************************/
	nCodec[DI_AUDIO_DECODER_SPDIF] = nCodec[DI_AUDIO_DECODER_MAIN];
	bRestart[DI_AUDIO_DECODER_SPDIF] = P_AUDIO_CheckSPDIFStart(DI_AUDIO_DECODER_MAIN, nCodec[DI_AUDIO_DECODER_SPDIF]);

	if(bRestart[DI_AUDIO_DECODER_SUB] == TRUE)
	{
		dResult = DRV_AUDIO_Start(DI_AUDIO_DECODER_SUB, nCodec[DI_AUDIO_DECODER_SUB]);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_Start failed! decoder %d \n", DI_AUDIO_DECODER_SUB);
			return DRV_ERR;
		}
	}
	if(bRestart[DI_AUDIO_DECODER_MAIN] == TRUE)
	{
		dResult = DRV_AUDIO_Start(DI_AUDIO_DECODER_MAIN, nCodec[DI_AUDIO_DECODER_MAIN]);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_Start failed! decoder %d \n", DI_AUDIO_DECODER_MAIN);
			return DRV_ERR;
		}
	}
	if(bRestart[DI_AUDIO_DECODER_SPDIF] == TRUE)
	{
		dResult = DRV_AUDIO_Start(DI_AUDIO_DECODER_SPDIF, nCodec[DI_AUDIO_DECODER_SPDIF]);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_Start failed! decoder %d \n", DI_AUDIO_DECODER_SPDIF);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

static DRV_Error P_AUDIO_ChangeHdmiOutput(DI_AUDIO_HDMI_MODE mode)
{//Sunwoo
	HUINT32				ulIndex;
	DRV_Error			dResult;
	DI_AUDIO_FORMAT		nCodec[AUDIO_ALL_DECODERS];
	HBOOL				bRestart[AUDIO_ALL_DECODERS];

	for(ulIndex=0; ulIndex<AUDIO_ALL_DECODERS; ulIndex++)
	{
		nCodec[ulIndex] = DI_AUDFORMAT_NONE;
		bRestart[ulIndex] = FALSE;
	}

	/*	1. Stop Decoder **********
	****************************/
	for(ulIndex=0; ulIndex<AUDIO_ALL_DECODERS; ulIndex++)
	{
		if(stAudioStatus[ulIndex].nState == DI_AUDIO_START)
		{
			bRestart[ulIndex] = TRUE;
			nCodec[ulIndex] = stAudioStatus[ulIndex].nFormat;

			dResult = DRV_AUDIO_Stop(ulIndex);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_AUDIO_Stop failed!\n");
				return DRV_ERR;
			}
		}
		else
		{
			bRestart[ulIndex] = FALSE;
		}
	}

	/*	2. Mode Change **********
	***************************/
	stAudioOutput.eHdmiMode = mode;

	/*	3. Volume adjusting **********
	*******************************/
	stAudioOutput.ulHdmiVolume = stAudioOutput.ulAnalogVolume;
	if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_DOWNMIX)
	{
		dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_HDMI, stAudioOutput.ulHdmiVolume);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetHdmiVolume failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);

			return DRV_ERR;
		}
	}

	/*	4. Start Decoder **********
	****************************/
	nCodec[DI_AUDIO_DECODER_SPDIF] = nCodec[DI_AUDIO_DECODER_MAIN];
	bRestart[DI_AUDIO_DECODER_SPDIF] = P_AUDIO_CheckSPDIFStart(DI_AUDIO_DECODER_MAIN, nCodec[DI_AUDIO_DECODER_SPDIF]);

	if(bRestart[DI_AUDIO_DECODER_SUB] == TRUE)
	{
		dResult = DRV_AUDIO_Start(DI_AUDIO_DECODER_SUB, nCodec[DI_AUDIO_DECODER_SUB]);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_Start failed!\n");
			return DRV_ERR;
		}
	}
	if(bRestart[DI_AUDIO_DECODER_MAIN] == TRUE)
	{
		dResult = DRV_AUDIO_Start(DI_AUDIO_DECODER_MAIN, nCodec[DI_AUDIO_DECODER_MAIN]);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_Start failed!\n");
			return DRV_ERR;
		}
	}
	if(bRestart[DI_AUDIO_DECODER_SPDIF] == TRUE)
	{
		dResult = DRV_AUDIO_Start(DI_AUDIO_DECODER_SPDIF, nCodec[DI_AUDIO_DECODER_SPDIF]);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_Start failed!\n");
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

#if 0
static void P_AUDIO_LockChanged(void * context, int param)
{//Sunwoo
	NEXUS_AudioDecoderHandle 	audioDecoder = (NEXUS_AudioDecoderHandle)context;
	HUINT32 					ulDevId = (HUINT32)param;
	NEXUS_AudioDecoderStatus	audioStatus;
	NEXUS_Error					nResult;

	if (audioDecoder )
	{
		/*	Lock/Unlock Callback ***********
		*	Lock occurs at anytime when there's a valid fram sync.
		*	Unlock occurs just only when pid is changed or Nodata condition.
		*	Unlock doesn't occur when signal is removed or decoder is stopped.
		************************************************************/
		nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[ulDevId], &audioStatus);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
			return ;
		}

		stAudioStatus[ulDevId].bLocked = audioStatus.locked;
		PrintDebug("stAudioStatus[%d].bLocked: [%s]\n", ulDevId, stAudioStatus[ulDevId].bLocked?"Y":"N");
	}
}
#endif

static DRV_Error P_AUDIO_SendEventMessage(HUINT32 ulDevId, DI_AUDIO_NOTIFY_EVT ulEvent)
{
	DI_AUDIO_CALLBACK_DATA_t 		toMA;
	HUINT32							callbackNum;

	VK_MEM_Memset(&toMA,0x00,sizeof(DI_AUDIO_CALLBACK_DATA_t));

	for(callbackNum=0; callbackNum<AUDIO_CALLBACK_OWNER_MAX; callbackNum++)
	{
		if(stAudioCallbacks[callbackNum].audioEventCallback[ulDevId][ulEvent] != NULL)
		{
			if(ulEvent == DI_AUDIO_RUNNING_STATE_CHANGED)
			{
				if(stAudioStatus[ulDevId].rState.state == DI_AUDIO_STATE_NODATA)
				{
					PrintDebug("Callback[%d]::Decoder[%d] P_AUDIO_Task: State: %s\n", callbackNum, ulDevId, "NODATA");
				}
				else if(stAudioStatus[ulDevId].rState.state == DI_AUDIO_STATE_DECODING)
				{
					PrintDebug("Callback[%d]::Decoder[%d] P_AUDIO_Task: State: %s\n", callbackNum, ulDevId, "DECODING");
				}
				else
				{
					PrintError("Unknown Status! You can't notify MW at this status\n");
					return DRV_ERR;
				}
			}
			else if((ulEvent == DI_AUDIO_FORMAT_CHANGED) || (ulEvent == DI_AUDIO_DATAEMPTY) || (ulEvent == DI_AUDIO_DATAFLOW))
			{
				PrintDebug("Callback[%d]::Decoder[%d] P_AudioSourceChanged!!\n", callbackNum, ulDevId);
			}
			else if((ulEvent == DI_AUDIO_MEMORY_STREAM_STARTED)||
				  (ulEvent == DI_AUDIO_MEMORY_STREAM_COMPLETED))
			{
/* PCM & Non-PCM are using this context */
#if 0
				/*	Only Non-PCM memory stream should follow this way */
				if((stMemoryStreamPool[0].bOccupied == TRUE) && (stMemoryStreamPool[0].eDecoder == ulDevId))
				{
					toMA.ulRequestId = stMemoryStreamPool[0].ulRequestId;
				}
				else if((stMemoryStreamPool[1].bOccupied == TRUE) && (stMemoryStreamPool[1].eDecoder == ulDevId))
				{
					toMA.ulRequestId = stMemoryStreamPool[1].ulRequestId;
				}
				else
				{
					PrintError("There no memory stream play task activated.\n");
					return DRV_ERR;
				}
#endif
				PrintDebug("[%s] MemoryStream Callback is called\n", __FUNCTION__);
			}

			if(stAudioCallbacks[callbackNum].audioEventCallback[ulDevId][ulEvent] != NULL)
			{
				toMA.nEvent = ulEvent;
				toMA.ulDecId = ulDevId;
				toMA.pvEventData = &(stAudioStatus[ulDevId].rState);
				stAudioCallbacks[callbackNum].audioEventCallback[ulDevId][ulEvent]((void*)&toMA);
			}
		}
	}

	return DRV_OK;
}

static void P_AudioFifoUnderFlow(void *context, int param)
{
	HBOOL						bFireCallback=FALSE;
	NEXUS_AudioDecoderHandle 	audioDecoder = (NEXUS_AudioDecoderHandle)context;
	HUINT32 					ulDevId = (HUINT32)param;
	DRV_Error					dResult;
	NEXUS_Error nResult;
	NEXUS_AudioDecoderStatus currentStatus;

	if (audioDecoder )
	{
		nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &currentStatus);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
			return ;
		}

		if(currentStatus.pts != 0)
		{
			if(stAudioStreamInfo.codec != currentStatus.codec)
			{
				bFireCallback = TRUE;
				stAudioStreamInfo.codec = currentStatus.codec;
				PrintDebug("DI_AUDIO::Codec changed!\n");
			}
			if(stAudioStreamInfo.sampleRate != currentStatus.sampleRate)
			{
				bFireCallback = TRUE;
				stAudioStreamInfo.sampleRate = currentStatus.sampleRate;
				PrintDebug("DI_AUDIO::SampleRate changed!\n");
			}
			if(stAudioStreamInfo.bitrate != currentStatus.codecStatus.ac3.bitrate)
			{
				bFireCallback = TRUE;
				stAudioStreamInfo.bitrate = currentStatus.codecStatus.ac3.bitrate;
				PrintDebug("DI_AUDIO::BitRate changed!\n");
			}

			if(bFireCallback == TRUE)
			{
				/* Callback part */
				dResult = P_AUDIO_SendEventMessage(ulDevId, DI_AUDIO_DATAEMPTY);
				if(dResult != DRV_OK)
				{
					PrintError("P_AUDIO_SendEventMessage failed!\n");
					return ;
				}
				else
				{
					cMediaRunningState = DI_AUDIO_DATAEMPTY;
					unCurrentPts = currentStatus.pts;
				}
			}
			else
			{
				PrintDebug("A callback has been fired from NEXUS, but Not figured out what it is...!\n");
			}
		}
	}
}

#if defined(PRINT_AUDIO_PTS_INFO)
static void P_AudioFirstPtsCallback(void *context, int param)
{
	NEXUS_AudioDecoderHandle 	audioDecoder = (NEXUS_AudioDecoderHandle)context;
	HUINT32 					ulDevId = (HUINT32)param;
	NEXUS_Error					nResult;
	NEXUS_AudioDecoderStatus 	currentStatus;

	if (audioDecoder)
	{
		nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &currentStatus);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
			return;
		}
	}

	PrintError("FirstPts is occurred deviceId (%d) started (%d) TSM (%d) locked (%d) fifoDepth (%d)\n",ulDevId,currentStatus.started, currentStatus.tsm, currentStatus.locked, currentStatus.fifoDepth );

	return;
}


static void P_AudioPtsErrorCallback(void *context, int param)
{
	NEXUS_AudioDecoderHandle 	audioDecoder = (NEXUS_AudioDecoderHandle)context;
	HUINT32 					ulDevId = (HUINT32)param;	
	NEXUS_AudioDecoderStatus 	currentStatus;
	NEXUS_Error	nResult;
	
	if (audioDecoder)
	{	
		nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &currentStatus);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
			return ;
		}
	}

	PrintError("Pts Error is occurred (%d) started (%d) TSM (%d) locked (%d) fifoDepth (%d)\n", ulDevId, currentStatus.started, currentStatus.tsm, currentStatus.locked, currentStatus.fifoDepth );

	return;
	
}
#endif


static void P_AudioSourceChanged(void *context, int param)
{//Sunwoo
	HBOOL						bFireCallback=FALSE;
	NEXUS_AudioDecoderHandle 	audioDecoder = (NEXUS_AudioDecoderHandle)context;
	HUINT32 					ulDevId = (HUINT32)param;
	DRV_Error					dResult;
	NEXUS_Error nResult;
	NEXUS_AudioDecoderStatus currentStatus;
	if (audioDecoder )
	{
		nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[0], &currentStatus);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
			return ;
		}
		if(stAudioStreamInfo.codec != currentStatus.codec)
		{
			bFireCallback = TRUE;
			stAudioStreamInfo.codec = currentStatus.codec;
			PrintDebug("DI_AUDIO::Codec changed!\n");
		}
		if(stAudioStreamInfo.sampleRate != currentStatus.sampleRate)
		{
			bFireCallback = TRUE;
			stAudioStreamInfo.sampleRate = currentStatus.sampleRate;
			PrintDebug("DI_AUDIO::SampleRate changed!\n");
		}

		if((currentStatus.codec == NEXUS_AudioCodec_eMpeg)|| (currentStatus.codec == NEXUS_AudioCodec_eMp3))
		{
		/* MPEG or MP3 */
			/* bitrate */
			if(stAudioStreamInfo.bitrate != currentStatus.codecStatus.mpeg.bitrate)
			{
				bFireCallback = TRUE;
				stAudioStreamInfo.bitrate = currentStatus.codecStatus.mpeg.bitrate;
				PrintDebug("DI_AUDIO::MPEG.BitRate changed!\n");
			}
			/* acMode */
			if(stAudioStreamInfo.ulAcmod != currentStatus.codecStatus.mpeg.channelMode)
			{
				bFireCallback = TRUE;
				stAudioStreamInfo.ulAcmod = currentStatus.codecStatus.mpeg.channelMode;
				PrintDebug("DI_AUDIO::MPEG.channelMode changed!\n");
			}
		}
		else if((currentStatus.codec >= NEXUS_AudioCodec_eAac) && (currentStatus.codec <= NEXUS_AudioCodec_eAacPlusAdts))
		{
		/* AAC */

			if(stAudioStreamInfo.bitrate != currentStatus.codecStatus.aac.bitrate)
			{
				bFireCallback = TRUE;
				PrintDebug("DI_AUDIO::AAC.BitRate %u -> %u ", stAudioStreamInfo.bitrate, currentStatus.codecStatus.aac.bitrate);
				stAudioStreamInfo.bitrate = currentStatus.codecStatus.aac.bitrate;
				PrintDebug("DI_AUDIO::AAC.BitRate changed!\n", stAudioStreamInfo.bitrate);
			}
			/* acMode */
			if(stAudioStreamInfo.ulAcmod != currentStatus.codecStatus.aac.acmod)
			{
				bFireCallback = TRUE;
				PrintDebug("DI_AUDIO::AAC.Acmod  0x%x --> 0x%x \n",stAudioStreamInfo.ulAcmod,currentStatus.codecStatus.aac.acmod);
				stAudioStreamInfo.ulAcmod = currentStatus.codecStatus.aac.acmod;
				PrintDebug("DI_AUDIO::AAC.Acmod changed!\n");
			}
		}
		else if((currentStatus.codec == NEXUS_AudioCodec_eAc3) || (currentStatus.codec == NEXUS_AudioCodec_eAc3Plus))
		{
		/* AC3 */
			/* bitrate */
			if(stAudioStreamInfo.bitrate != currentStatus.codecStatus.ac3.bitrate)
			{
				bFireCallback = TRUE;
				stAudioStreamInfo.bitrate = currentStatus.codecStatus.ac3.bitrate;
				PrintDebug("DI_AUDIO::AC3.BitRate changed!\n");
			}
			/* acMode */
			if(stAudioStreamInfo.ulAcmod != currentStatus.codecStatus.ac3.acmod)
			{
				bFireCallback = TRUE;
				stAudioStreamInfo.ulAcmod = currentStatus.codecStatus.ac3.acmod;
				PrintDebug("DI_AUDIO::AC3.Acmod changed!\n");
			}
		}
		else
		{
			PrintDebug("[%s] Can't fire callback !!!, NEXUS_AudioCodec = %d \n", __FUNCTION__, currentStatus.codec);
		}

		if(bFireCallback == TRUE)
		{
			/* Callback part */
			dResult = P_AUDIO_SendEventMessage(ulDevId, DI_AUDIO_FORMAT_CHANGED);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_SendEventMessage failed!\n");
				return ;
			}
		}
		else
		{
			PrintDebug("[%s] A callback has been fired from NEXUS, but Not figured out what it is...!\n", __FUNCTION__);
		}
	}
}

static DRV_Error P_AUDIO_PCMStreamplay_Stop(HUINT32 ulRequestId, HUINT32 ulTaskIndex)
{//Sunwoo
	DRV_Error			dResult;
	HUINT32				ulDevId=DI_AUDIO_DECODER_MAIN;
	HBOOL bAllStopped;
	DI_AUDIO_STREAM_MSG 		audMsg;
	HINT32						vResult;

	VK_MEM_Memset(&audMsg,0x00,sizeof(DI_AUDIO_STREAM_MSG));

	if(stAudioPlayback[ulTaskIndex].gPlayback == NULL)
	{
		PrintError("Playback object is NULL!\n");
		return DRV_ERR;
	}

	if((stMemoryStreamPool[0].bOccupied == TRUE) && (stMemoryStreamPool[0].ulTaskIndex== ulTaskIndex))
	{
		stMemoryStreamPool[0].bOccupied = FALSE;
	}
	else if((stMemoryStreamPool[1].bOccupied == TRUE) && (stMemoryStreamPool[1].ulTaskIndex == ulTaskIndex))
	{
		stMemoryStreamPool[1].bOccupied = FALSE;
	}

	NEXUS_AudioPlayback_Stop(stAudioPlayback[ulTaskIndex].gPlayback);
	stAudioPlayback[ulTaskIndex].bStart = FALSE;

	bAllStopped = P_AUDIO_CheckAllStopped();
	if(bAllStopped == TRUE)
	{
		dResult = P_AUDIO_RemoveDacOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDacOutputPath failed!\n");
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveSpdifOutputPath(ulDevId, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveHdmiOutputPath(ulDevId, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			return DRV_ERR;
		}
#if defined(CONFIG_BLUETOOTH)
		dResult = P_AUDIO_RemoveBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
			return DRV_ERR;
		}
#endif
	}

	audMsg.sLoopCount = 0;
	audMsg.sMemBuf = 0;
	audMsg.sMemBufSize = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
	audMsg.sRequest = DI_AUDIO_MEMORY_PCM_STOP;
	audMsg.ulTaskIndex = ulTaskIndex;
	audMsg.pos = 0;
	audMsg.ulRequestId = ulRequestId;
	audMsg.ulTimeTicket = 0;

	vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
		return DRV_ERR;
	}

	DD_MEM_Free(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint);
	stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;

	return DRV_OK;
}

static DRV_Error P_AUDIO_PCMStreamplay_Start(DI_AUDIO_STREAM_MSG stAudMsg)
{//Sunwoo
	NEXUS_Error					nResult;
	unsigned char *				buffer;
	unsigned long				buffer_size;
	HUINT32 					currentCount;
	HUINT32 					accum_amount=0;
	HINT32						vResult;
	HUINT32						padding_amount=0;
	DI_AUDIO_STREAM_MSG			audMsg;
	NEXUS_AudioPlaybackStatus	status;
	DRV_Error					dResult;
	HULONG						ulPos= stAudMsg.pos;
	HUINT32						ulTaskIndex = stAudMsg.ulTaskIndex;
	HUINT32						dLoopCount = stAudMsg.sLoopCount;
	HUINT32						ulRequestId = stAudMsg.ulRequestId;
	HUINT32						ulTimeTicket = stAudMsg.ulTimeTicket;
	unsigned char*				dMemBuf = stAudMsg.sMemBuf;
	unsigned long				dRemainSize = stAudMsg.sMemBufSize;
	HUINT32						ulDevId = stAudMsg.sDevId;

	VK_MEM_Memset(&audMsg,0x00,sizeof(DI_AUDIO_STREAM_MSG));

	if(stAudioPlayback[ulTaskIndex].bStart == FALSE)
	{
		stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;
		return DRV_OK;
	}
	//1. Get Buffer size
	nResult = NEXUS_AudioPlayback_GetBuffer(stAudioPlayback[ulTaskIndex].gPlayback, (void **)&buffer, (size_t *)&buffer_size);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioPlayback_GetBuffer Failed!\n");
		return DRV_ERR;
	}

	//2. Buffer Size Check & fill in
	if(buffer_size > dRemainSize)	//if buffer is larger than source
	{
		/* buffer를 0으로 채우고 */
		VK_MEM_Memset(buffer, 0, buffer_size);
		padding_amount = buffer_size;
		VK_MEM_Memcpy((void *)buffer, &(dMemBuf[ulPos]), dRemainSize);//dMemBuf, dRemainSize);
		accum_amount += dRemainSize;
		if(stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount != 0)
			dLoopCount--;

		if((buffer_size - dRemainSize)>stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize)
		{
			if(dLoopCount != 0 || stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount == 0)
			{
				buffer+=dRemainSize;
				currentCount = (buffer_size-dRemainSize) / stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
				for( ; currentCount>0; currentCount--)
				{
					VK_MEM_Memcpy((void *)buffer, dMemBuf, stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize);
					buffer += stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
					accum_amount += stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
					if(stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount != 0)
					{
						dLoopCount--;
						if(dLoopCount == 0)
							break;
					}
				}
			}
		}
		/*	0인 부분의 크기를 알아두고... */
		if(padding_amount > accum_amount)
		{
			padding_amount -= accum_amount;
		}
		else
		{
			padding_amount = 0;
		}

		if(dLoopCount != 0 || stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount == 0) //retry to call this function with the left amount of loopcount
		{
			nResult = NEXUS_AudioPlayback_ReadComplete(stAudioPlayback[ulTaskIndex].gPlayback, accum_amount);//buffer_size);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioPlayback_ReadComplete failed!\n");
				return DRV_ERR;
			}

			audMsg.sDevId = ulDevId;
			audMsg.sLoopCount = dLoopCount;
			audMsg.sMemBuf = dMemBuf;
			audMsg.sMemBufSize = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
			audMsg.sRequest = DI_AUDIO_MEMORY_PCM_PLAY;
			audMsg.ulTaskIndex = ulTaskIndex;
			audMsg.pos = 0;
			audMsg.ulRequestId = ulRequestId;
			audMsg.ulTimeTicket = ulTimeTicket;

			vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
			if(vResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
				return DRV_ERR;
			}
		}
		else
		{
			/* buffer_size만큼 무조건 밀어넣어준다. */
			nResult = NEXUS_AudioPlayback_ReadComplete(stAudioPlayback[ulTaskIndex].gPlayback, buffer_size);//accum_amount);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioPlayback_ReadComplete failed!\n");
				return DRV_ERR;
			}
		}
	}
	else	//if buffer is smaller than source
	{
		if(buffer_size > 0)
		{
			VK_MEM_Memcpy((void *)buffer, &(dMemBuf[ulPos]), buffer_size);
			nResult = NEXUS_AudioPlayback_ReadComplete(stAudioPlayback[ulTaskIndex].gPlayback, buffer_size);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioPlayback_ReadComplete failed!\n");
				return DRV_ERR;
			}
		}

		audMsg.sDevId = ulDevId;
		audMsg.pos = ulPos+buffer_size;
		audMsg.sLoopCount = dLoopCount;
		audMsg.sMemBuf = dMemBuf;
		audMsg.sMemBufSize = dRemainSize - buffer_size;
		audMsg.sRequest = DI_AUDIO_MEMORY_PCM_PLAY;
		audMsg.ulTaskIndex = ulTaskIndex;
		audMsg.ulRequestId = ulRequestId;
		audMsg.ulTimeTicket = ulTimeTicket;

		vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
		if(vResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
			return DRV_ERR;
		}
	}

	if(dLoopCount == 0 && stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount !=0)
	{
		for(;;)
		{
			NEXUS_AudioPlayback_GetStatus(stAudioPlayback[ulTaskIndex].gPlayback, &status);

			if(status.queuedBytes > padding_amount)
			{
				VK_TASK_Sleep(20);
			}
			else
			{
				break;
			}
		}
		dResult = P_AUDIO_PCMStreamplay_Stop(ulRequestId, ulTaskIndex);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_PCMStreamplay_Stop failed!\n");
			return DRV_ERR;
		}

		stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;
	}
	return DRV_OK;
}

static DRV_Error P_AUDIO_PauseDecoder(HUINT32 ulDevId, HBOOL pause)
{//Sunwoo
	NEXUS_AudioDecoderTrickState	pTrickState;
	NEXUS_Error						nResult;

	NEXUS_AudioDecoder_GetTrickState(stAudioInstance.audioDecoder[ulDevId], &pTrickState);
	if(pause == TRUE)
	{
		pTrickState.rate = 0;
	}
	else
	{
		pTrickState.rate = NEXUS_NORMAL_DECODE_RATE;
	}

	nResult = NEXUS_AudioDecoder_SetTrickState(stAudioInstance.audioDecoder[ulDevId], &pTrickState);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_SetTrickState Failed!\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

#if defined(CONFIG_DI20)
static DRV_Error P_AUDIO_FlushDecoder(HUINT32 ulDevId)
{
	NEXUS_Error nResult;

	if((ulDevId != DI_AUDIO_DECODER_MAIN) && (ulDevId != DI_AUDIO_DECODER_SUB))
	{
		PrintError("[%s]%d Error : ulDevId %d \n",__FUNCTION__,__LINE__, ulDevId);
		return DRV_ERR;
	}
	else if(stAudioStatus[ulDevId].nState == DI_AUDIO_STOP)
	{
		PrintError("[%s]%d Decoder %d was already flush \n",__FUNCTION__,__LINE__, ulDevId);
		return DRV_OK;
	}
	else
	{
		nResult = NEXUS_AudioDecoder_Flush(stAudioInstance.audioDecoder[ulDevId]);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d Error : NEXUS_AudioDecoder_Flush FAILED ulDevId %d \n",__FUNCTION__,__LINE__, ulDevId);
			return DRV_ERR;
		}

		if((ulDevId == DI_AUDIO_DECODER_MAIN) &&
			(stAudioStatus[DI_AUDIO_DECODER_SPDIF].nState == DI_AUDIO_START))
		{
			nResult = NEXUS_AudioDecoder_Flush(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SPDIF]);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d Error : NEXUS_AudioDecoder_Flush FAILED ulDevId %d \n",__FUNCTION__,__LINE__, DI_AUDIO_DECODER_SPDIF);
				return DRV_ERR;
			}
		}
	}

	return DRV_OK;
}
#endif

static DRV_Error P_AUDIO_StopMemoryStream(HUINT32 ulRequestId, HUINT32 ulTaskIndex)
{//Sunwoo
	HUINT32						ulDevId;
	DRV_Error					dResult;
	HUINT32						ulPoolIndex;
	HBOOL						bAllStopped;
	DI_AUDIO_FORMAT nType;
	DI_AUDIO_STREAM_MSG 		audMsg;
	HINT32						vResult;
/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("HUINT32 ulRequestId: %d, HUINT32 ulTaskIndex: %d\n", ulRequestId, ulTaskIndex);

/*///////////////////////////////////*/

	VK_MEM_Memset(&audMsg,0x00,sizeof(DI_AUDIO_STREAM_MSG));

	if((stMemoryStreamPool[0].bOccupied == TRUE) && (stMemoryStreamPool[0].ulRequestId == ulRequestId))
	{
		ulPoolIndex = 0;
	}
	else if((stMemoryStreamPool[1].bOccupied == TRUE) && (stMemoryStreamPool[1].ulRequestId == ulRequestId))
	{
		ulPoolIndex = 1;
	}
	else
	{
		PrintError("Request Index is invalid! you didn't start memory stream play with requestId %d\n", ulRequestId);
		return DRV_ERR;
	}
	ulDevId = stMemoryStreamPool[ulPoolIndex].eDecoder;
	nType = stAudioStatus[ulDevId].nFormat;

	audMsg.sDevId = ulDevId;
	audMsg.sLoopCount = 0;
	audMsg.sMemBuf = 0;
	audMsg.sMemBufSize = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
	audMsg.sRequest = DI_AUDIO_MEMORY_STREAM_STOP;
	audMsg.ulTaskIndex = ulTaskIndex;
	audMsg.pos = 0;
	audMsg.ulRequestId = ulRequestId;
	audMsg.ulTimeTicket = 0;

	vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
		return DRV_ERR;
	}

	stMemoryStreamPool[ulPoolIndex].bOccupied = FALSE;

	DD_MEM_Free(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint);

	if(stAudioPlaypump[ulTaskIndex].gPlaypump != NULL)
	{
		NEXUS_Playpump_Stop(stAudioPlaypump[ulTaskIndex].gPlaypump);
		NEXUS_Playpump_CloseAllPidChannels(stAudioPlaypump[ulTaskIndex].gPlaypump);
		stAudioPlaypump[ulTaskIndex].bStart = FALSE;
#if defined(CONFIG_PLAYBACK)
		DRV_PVR_ReleasePlaypumpHandle(stAudioPlaypump[ulTaskIndex].gPlaypump);
#endif
		stAudioPlaypump[ulTaskIndex].gPlaypump = NULL;
	}

	if(stAudioStatus[ulDevId].nState != DI_AUDIO_STOP)
	{
		NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[ulDevId]);
		stAudioStatus[ulDevId].nState = DI_AUDIO_STOP;
		stAudioStatus[ulDevId].rState.state = DI_AUDIO_STATE_UNKNOWN;
		stAudioStatus[ulDevId].audioSettings.pidChannel = NULL;
		stAudioStatus[ulDevId].nFormat = DI_AUDFORMAT_NONE;
	}

	bAllStopped = P_AUDIO_CheckAllStopped();
	if(bAllStopped == TRUE)
	{
		dResult = P_AUDIO_RemoveDacOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDacOutputPath failed!\n");
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveSpdifOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveHdmiOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			return DRV_ERR;
		}
#if defined(CONFIG_BLUETOOTH)
		dResult = P_AUDIO_RemoveBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
			return DRV_ERR;
		}
#endif
	}

	stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;

	return DRV_OK;
}

static DRV_Error P_AUDIO_StartMemoryStream(DI_AUDIO_STREAM_MSG stAudMsg)//(HUINT32 ulTimeTicket, HUINT32 ulRequestId, HUINT32 ulDevId, unsigned char	*pMemBuf, unsigned long pRemainSize, HUINT32 ulPos, HUINT32 pLoopCount, HUINT32 ulTaskIndex)
{//sunwoo
	HINT32 					vResult;
	unsigned char * 		buffer;
	unsigned long 			buffer_size;
	unsigned long 			accum_amount=0;
	unsigned long 			currentCount;
	DI_AUDIO_STREAM_MSG 	audMsg;
	NEXUS_Error 			nResult;
	HULONG					ulPos = stAudMsg.pos;
	HUINT32					ulTaskIndex = stAudMsg.ulTaskIndex;
	HUINT32					ulDevId = stAudMsg.sDevId;
	HUINT32					pLoopCount = stAudMsg.sLoopCount;
	HUINT32					ulRequestId = stAudMsg.ulRequestId;
	HUINT32					ulTimeTicket = stAudMsg.ulTimeTicket;
	unsigned char*			pMemBuf = stAudMsg.sMemBuf;
	unsigned long			pRemainSize = stAudMsg.sMemBufSize;

	if(stAudioPlaypump[ulTaskIndex].bStart == FALSE)
	{
		stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;
		return DRV_OK;
	}

	VK_MEM_Memset(&audMsg,0x00,sizeof(DI_AUDIO_STREAM_MSG));

	//1. Get buffer size
	nResult = NEXUS_Playpump_GetBuffer(stAudioPlaypump[ulTaskIndex].gPlaypump, (void **)&buffer, (size_t *)&buffer_size);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Playpump_GetBuffer Failed!\n");
		return DRV_ERR;
	}

	//2. Buffer Size Check & fill in
	if(buffer_size >= pRemainSize)	//if buffer is larger than source
	{
		VK_MEM_Memcpy((void *)buffer, &pMemBuf[ulPos], pRemainSize);
		accum_amount += pRemainSize;
		if(stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount != 0)
		{
			pLoopCount--;
		}

		if((buffer_size - pRemainSize)>stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize)
		{
			if(pLoopCount != 0 || stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount == 0)
			{
				buffer+=pRemainSize;
				currentCount = (buffer_size-pRemainSize) / stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
				for( ; currentCount>0; currentCount--)
				{
					VK_MEM_Memcpy((void *)buffer, pMemBuf, stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize);
					buffer += stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
					accum_amount += stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
					pLoopCount--;
					if(pLoopCount == 0)
						break;
				}
			}
		}

		nResult = NEXUS_Playpump_ReadComplete(stAudioPlaypump[ulTaskIndex].gPlaypump, 0, accum_amount);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s][%d] NEXUS_Playpump_ReadComplete Failed! : ret(0x%x)\n", __FUNCTION__, __LINE__, nResult);
			return DRV_ERR;
		}

		if(pLoopCount != 0 || stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount == 0) //retry to call this function with left amount of loopcount
		{
			audMsg.sDevId = ulDevId;
			audMsg.sLoopCount = pLoopCount;
			audMsg.sMemBuf = pMemBuf;
			audMsg.sMemBufSize = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
			audMsg.pos = 0;
			audMsg.sRequest = DI_AUDIO_MEMORY_STREAM_PLAY;
			audMsg.ulTaskIndex = ulTaskIndex;
			audMsg.ulRequestId = ulRequestId;
			audMsg.ulTimeTicket = ulTimeTicket;

			vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
			if(vResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
				return DRV_ERR;
			}
		}
	}
	else	//if buffer is smaller than source
	{
		if(buffer_size > 0)
		{
			VK_MEM_Memcpy((void *)buffer, &pMemBuf[ulPos], buffer_size);
			nResult = NEXUS_Playpump_ReadComplete(stAudioPlaypump[ulTaskIndex].gPlaypump, 0, buffer_size);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("[%s][%d] NEXUS_Playpump_ReadComplete Failed! : ret(0x%x)\n", __FUNCTION__, __LINE__, nResult);
				return DRV_ERR;
			}
		}

		audMsg.sDevId = ulDevId;
		audMsg.sLoopCount = pLoopCount;
		audMsg.sMemBuf = pMemBuf;
		audMsg.sMemBufSize = pRemainSize - buffer_size;
		audMsg.pos = ulPos + buffer_size;
		audMsg.sRequest = DI_AUDIO_MEMORY_STREAM_PLAY;
		audMsg.ulTaskIndex = ulTaskIndex;
		audMsg.ulRequestId = ulRequestId;
		audMsg.ulTimeTicket = ulTimeTicket;

		vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
		if(vResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
			return DRV_ERR;
		}
	}


	//8. 작업이 종료 된 후 전부 close
	if(pLoopCount == 0 && stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount != 0)
	{
		NEXUS_AudioDecoderStatus status;
		nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[audMsg.sDevId], &status);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
			return DRV_ERR;
		}

		stAudioStatus[audMsg.sDevId].last_depth = status.fifoDepth;

		audMsg.sRequest = DI_AUDIO_MEMORY_STREAM_MONITOR;
		audMsg.sDevId = ulDevId;
		audMsg.ulTaskIndex = ulTaskIndex;
		audMsg.ulRequestId = ulRequestId;
		audMsg.ulTimeTicket = ulTimeTicket;

		vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
		if(vResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

static DRV_Error P_AUDIO_PauseMemoryStream(HUINT32 ulDevId, HBOOL pause)
{
	DRV_Error dResult;

	dResult = P_AUDIO_PauseDecoder(ulDevId, pause);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_PauseMemoryStream failed!\n");
		return DRV_ERR;
	}
	return DRV_OK;
}

static DRV_Error P_AUDIO_IsDataComing(HUINT32 ulDevId)
{//Sunwoo
	NEXUS_Error nResult;
	NEXUS_AudioDecoderStatus currentStatus;

	/*	Decoding Status Decision ************
	*	CDB level (fifo depth) shows the data buffered before Audio Decoder.
	*	But, If scrambled data comes, we should stare at the pts whether it changes.
	*	status: [fifo depth value / pts ]
	*	Decoding: [none-zero / none-zero]
	*	Nodata: zero / zero
	*	Scrambled data: none-zero / zero
	**********************************************************************/
	nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[ulDevId], &currentStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
		return DRV_ERR;
	}

	if(currentStatus.fifoDepth == 0)
	{
		stAudioStatus[ulDevId].bDataComing = FALSE;
	}
	else
	{
		stAudioStatus[ulDevId].bDataComing = TRUE;
	}

	return DRV_OK;
}

static HBOOL P_AUDIO_IsDownMixSupport(HUINT32 ulDevId)
{//Sunwoo
	if(g_AudioCap[ulDevId].support_format & DI_AUDFORMAT_AC3)
	{
		return TRUE;
	}
	return FALSE;
}

static HUINT32 P_AUDIO_FindFreeTaskId(void)
{//Sunwoo
	if(stAudioTaskInfo[0].bOccupied == FALSE)
	{
		return DI_AUDIO_TASK1;
	}
	else if(stAudioTaskInfo[1].bOccupied == FALSE)
	{
		return DI_AUDIO_TASK2;
	}

	return DI_AUDIO_TASK_BUSY;
}

static HUINT32 P_AUDIO_FindFreeMemStreamId(void)
{//Sunwoo
	HUINT32 ulPoolIndex;
	if(stMemoryStreamPool[0].bOccupied == FALSE)
	{
		ulPoolIndex = DI_AUDIO_MEMPOOL1;
	}
	else if(stMemoryStreamPool[1].bOccupied == FALSE)
	{
		ulPoolIndex = DI_AUDIO_MEMPOOL2;
	}
	else
	{
		PrintError("There's no free MemoryStream instance. You're already started 2 memory streams\n");
		ulPoolIndex = DI_AUDIO_MEMPOOL_BUSY;
	}

	return ulPoolIndex;
}

static HUINT32 P_AUDIO_GetEmptyCallbackIndex(void)
{//Sunwoo
	HUINT32 nOwner;

	for(nOwner=0; nOwner<AUDIO_CALLBACK_OWNER_MAX; nOwner++)
	{
		if(stAudioCallbacks[nOwner].bOccupied == FALSE)
		{
			stAudioCallbacks[nOwner].bOccupied = TRUE;
			/* NULL is register flag...so, nOwner should not be zero(null) */
			return nOwner+1;
		}
	}

	PrintError("There's No Empty Callback pointer!\n");
	return AUDIO_CALLBACK_OWNER_MAX+2;
}

static HUINT32 P_AUDIO_GetDefaultDelay(DI_AUDIO_FORMAT nType)
{//Sunwoo
	if(nType == DI_AUDFORMAT_MPEG)
	{
		return stAudioSyncStatus.ulMpegPtsOffset;
	}
	else if(nType == DI_AUDFORMAT_AC3)
	{
		return stAudioSyncStatus.ulAc3PtsOffset;
	}
	else if(nType == DI_AUDFORMAT_AC3_PLUS)
	{
		return stAudioSyncStatus.ulAc3plusPtsOffset;
	}
	else if((nType == DI_AUDFORMAT_AAC) || (nType == DI_AUDFORMAT_AAC_ADTS) || (nType == DI_AUDFORMAT_AAC_LOAS))
	{
		return stAudioSyncStatus.ulAacPtsOffset;
	}
	else if((nType == DI_AUDFORMAT_AAC_PLUS) || (nType == DI_AUDFORMAT_AAC_PLUS_LOAS) || (nType == DI_AUDFORMAT_AAC_PLUS_ADTS))
	{
		return stAudioSyncStatus.ulAacplusPtsOffset;
	}
	else
	{
		return 0;
	}
}

static DRV_Error P_AUDIO_GetPCMHeaderInfo(HUINT32 ulTaskIndex, HUINT32 *ulSampleRate, HUINT32 *ulBitsPerSample, bool *bStereo, HULONG dMemBufSize)
{//Sunwoo
	HUINT32 								skip;
	HUINT32 								pcmIndex=0;
	HUINT32 								BytesPerSample=0;

	if(VK_strncmp("RIFF ", (char *)stAudioTaskInfo[ulTaskIndex].pbuffStartPoint, 4) == 0)
	{
		PrintDebug("Wav file detected!\n");
		pcmIndex = 12;
		while(1)
		{
			if(VK_strncmp("fmt", (char *)(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint)+pcmIndex, 3) == 0)
			{
				*ulSampleRate = stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+12];
				*ulSampleRate |= (stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+13])<<8;
				*ulSampleRate |= (stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+14])<<16;
				*ulSampleRate |= (stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+15])<<24;

				*ulBitsPerSample = stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+22];
				*ulBitsPerSample |= (stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+23])<<8;

				BytesPerSample = stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+20];
				BytesPerSample |= (stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+21])<<8;

				if(BytesPerSample == 1)
				{
					*bStereo = FALSE;
				}
				else if(BytesPerSample == 2)
				{
					if(*ulBitsPerSample == 8)
					{
						*bStereo = TRUE;
					}
					else
					{
						*bStereo = FALSE;
					}
				}
				else if(BytesPerSample == 4)
				{
					*bStereo = TRUE;
				}
				else
				{
					PrintError("Invalid stereo value from Wav header!\n");
					return DRV_ERR;
				}
				pcmIndex += 24;
				continue;
			}
			else if(VK_strncmp("data", (char *)(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint)+pcmIndex, 4) == 0)
			{
				stAudioTaskInfo[ulTaskIndex].ulPCMIndex = pcmIndex;
				break;
			}
			else
			{
				if(pcmIndex >= dMemBufSize)
				{
					PrintError("There's no fmt chunk!\n");
					return DRV_ERR;
				}
				skip = stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex];
                skip |= stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+1]<<8;
                skip |= stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+2]<<16;
                skip |= stAudioTaskInfo[ulTaskIndex].pbuffStartPoint[pcmIndex+3]<<24;
				PrintDebug("chunk size: %d\n", skip);
                pcmIndex += skip;
			}
		}
	}
	PrintDebug("PCM header:: SampleRate: %d\n", *ulSampleRate);
	PrintDebug("PCM header:: BitsPerSample: %d\n", *ulBitsPerSample);
	PrintDebug("PCM header:: BytesPerSample: %d\n", BytesPerSample);
	PrintDebug("PCM header:: data offset: %d\n", pcmIndex);
	PrintDebug("PCM header:: Stereo: %s\n", *bStereo?"Y":"N");

	return DRV_OK;
}

static DRV_Error P_AUDIO_DelayDecoder(HUINT32 ulDevId, HINT32 uiDelay)
{//Sunwoo
	NEXUS_Error nResult;
	NEXUS_AudioDecoderSettings settings;

	/*	45KHz	****************************************
		45 offset value means 1ms
	****************************************************/

	if(uiDelay < 0)
	{
		PrintError("uiDelay should be a positive value!\n");
		return DRV_ERR;
	}
	else
	{
		uiDelay += stAudioSyncStatus.ulBasePtsOffset;
		uiDelay = uiDelay*45;

		NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &settings);
		PrintDebug("Current Pts Offset is %d New Pts Offset is %d\n", settings.ptsOffset, uiDelay);

		settings.ptsOffset = uiDelay;

		nResult = NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &settings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_SetSettings Failed!\n");
			return DRV_ERR;
		}
	}
	return DRV_OK;
}

static DRV_Error P_AUDIO_RestartDecoder(HUINT32 ulDevId)
{//Sunwoo
	HBOOL bAllStopped;
	NEXUS_Error nResult;
	DRV_Error dResult;
	DI_AUDIO_FORMAT nType;
	HINT32 vResult;

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("HUINT32 ulDevId: %d\n", ulDevId);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioPSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	if(stAudioStatus[ulDevId].nState != DI_AUDIO_START)
	{
		PrintError("Decoder [%d] is not active now!\n", ulDevId);
		VK_SEM_Release(s_ulAudioPSemId);
		return DRV_OK;
	}

	/*	1. Stopping the decoder
	*	Stop the chosen decoder at first.
	*/
	NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[ulDevId]);

#if defined(CONFIG_SYNC_CHANNEL)
	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		dResult = DRV_SYNC_SetSyncChannel(DRV_SYNC_AUDMAIN,stAudioStatus[ulDevId].audioConnectorType);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SYNC_SetSyncChannel failed!\n");
			goto out;
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		dResult = DRV_SYNC_SetSyncChannel(DRV_SYNC_AUDSPDIF,stAudioStatus[ulDevId].audioConnectorType);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SYNC_SetSyncChannel failed!\n");
			goto out;
		}
	}
#endif

	/*	2. Starting the decoder
	*	Start the chosen decoder without any setting changes.
	*/
	nResult = NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[ulDevId], &stAudioStatus[ulDevId].audioSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_Start failed!\n");
		goto out;
	}
	VK_SEM_Release(s_ulAudioPSemId);
	return DRV_OK;

	/*	If there's an error case, all paths and status should be removed!
	*/
out:
	/*	NOTICE!!
	*	nFormat should be set as DI_AUDFORMAT_NONE after removing digital output path!!!
	*/
	nType = stAudioStatus[ulDevId].nFormat;
	stAudioStatus[ulDevId].nFormat = DI_AUDFORMAT_NONE;
	stAudioStatus[ulDevId].nState = DI_AUDIO_STOP;
	stAudioStatus[ulDevId].rState.state = DI_AUDIO_STATE_UNKNOWN;
	/*	NOTICE!!
	*	When the decoder stops, the bTsm should be set as TRUE!
	*	Because, decoder should start in TSM mode.
	*/
	dResult = P_AUDIO_SetTsmMode(TRUE);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetTsmMode failed!\n");
		VK_SEM_Release(s_ulAudioPSemId);
		return DRV_ERR;
	}

	bAllStopped = P_AUDIO_CheckAllStopped();
	if(bAllStopped == TRUE)
	{
		dResult = P_AUDIO_RemoveDacOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDacOutputPath failed!\n");
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveSpdifOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			VK_SEM_Release(s_ulAudioPSemId);
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveHdmiOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			VK_SEM_Release(s_ulAudioPSemId);
			return DRV_ERR;
		}
#if defined(CONFIG_BLUETOOTH)
		dResult = P_AUDIO_RemoveBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
			VK_SEM_Release(s_ulAudioPSemId);
			return DRV_ERR;
		}
#endif
	}
	VK_SEM_Release(s_ulAudioPSemId);
	return DRV_ERR;
}

#ifdef CONFIG_DDCO_CERTI
static DRV_Error P_AUDIO_SetOutputType(HUINT32 ulDevId)
{//Sunwoo
	NEXUS_Error nResult;
	NEXUS_AudioDecoderStatus nStatus;
	NEXUS_AudioDecoderSettings nSettings;
	NEXUS_AudioDecoderOutputMode outputMode;
#if defined(CONFIG_SYNC_CHANNEL)
	DRV_Error dResult;
#endif

	if(ulDevId != DI_AUDIO_DECODER_MAIN)
	{
		PrintError("Output Type Setting is available only for pcm decoder!\n");
		return DRV_ERR;
	}

/*************************
*	1. OutputType Setting.
*************************/
	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &nSettings);
	nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[ulDevId], &nStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
		return DRV_ERR;
	}
	outputMode = nSettings.outputMode;

	if((nStatus.codecStatus.aac.acmod == NEXUS_AudioAacAcmod_eThreeChannel_2_1_L_R_S)||
		(nStatus.codecStatus.aac.acmod == NEXUS_AudioAacAcmod_eFourChannel_2_2_L_R_SL_SR))
	{
		nSettings.outputMode = NEXUS_AudioDecoderOutputMode_e2_0;
	}
	else
	{
		nSettings.outputMode = NEXUS_AudioDecoderOutputMode_eAuto;
	}

	nResult = NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_SetSettings failed!\n");
		return DRV_ERR;
	}

/*************************
*	2. Decoder restart.
*************************/
	if((stAudioStatus[ulDevId].nState == DI_AUDIO_START) && (outputMode != nSettings.outputMode))
	{
		NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[ulDevId]);
#if defined(CONFIG_SYNC_CHANNEL)
		dResult = DRV_SYNC_SetSyncChannel(DRV_SYNC_AUDMAIN,stAudioStatus[ulDevId].audioConnectorType);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SYNC_SetSyncChannel failed!\n");
			return DRV_ERR;
		}
#endif
		nResult = NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[ulDevId], &stAudioStatus[ulDevId].audioSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_Start failed!\n");
			return DRV_ERR;
		}
	}

	return DRV_OK;
}
#endif

#if __________task__________
#endif

static void P_AudioEventTask(void* param)
{//Sunwoo
	HUINT32 						i;
	HUINT32 						notify = 0; 	//0:Nodata 1:decoding
	HUINT32 						nodataLevel[AUDIO_ALL_DECODERS];
	HUINT32 						decodingLevel[AUDIO_ALL_DECODERS];
	DRV_Error 						dResult;
	NEXUS_Error 					nResult;
	NEXUS_AudioDecoderStatus 		currentStatus;
#if defined(CONFIG_AUDIO_ERROR_LOG) /* for audio error log */
	unsigned long ulPrevDecodeErr=0;
	unsigned long ulDecodeErr=0;
	unsigned long ulPrevPtsErr=0;
	unsigned long ulPtsErr=0;
	HUINT32 callbackNum;
	DI_AUDIO_CALLBACK_DATA_t toMA;
#endif

	BSTD_UNUSED(param);

	for(i = 0; i<3; i++)
	{
		nodataLevel[i] = 0;
		decodingLevel[i] = 0;
	}

	while(1)
	{
#if defined(CONFIG_AUDIO_ERROR_LOG) /* for audio error log */
		VK_MEM_Memset(&toMA,0x00,sizeof(DI_AUDIO_CALLBACK_DATA_t));
#endif
		for(i = 0; i<AUDIO_ALL_DECODERS; i++)
		{
			/*	1. Status Check	*****************************************************
			*	If decoder is not activated, the status variable should be UNKNOWN &
			*	both NODATA level & DECODING level should be reset.
			*	And if status is NODATA, make the decoder muted.
			*************************************************************************/
			if(stAudioStatus[i].nState != DI_AUDIO_START)
			{
				nodataLevel[i] = 0;
				decodingLevel[i] = 0;
				continue;
			}

			nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[i], &currentStatus);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
				continue;
			}

#if defined(PRINT_AUDIO_PTS_INFO)
			if(i == DI_AUDIO_DECODER_MAIN)
			{
				HUINT32 stc = 0;
				if(stAudioStatus[i].audioSettings.stcChannel != NULL)
				{
					NEXUS_StcChannel_GetStc(stAudioStatus[i].audioSettings.stcChannel,&stc);
				}	

				DI_UART_Print("\033[1;32m\t\t[%s:%d]\t\t"
				
				              "Audio Decoder[%d] : PTS (%#x) STC (%#x) Diff(%d) fifoDepth (%d(%d%%))"

				              "\033[00m\n", __func__, __LINE__,
								i,
								currentStatus.pts,
								stc,
								currentStatus.ptsStcDifference,
								currentStatus.fifoDepth,
								currentStatus.fifoSize?(currentStatus.fifoDepth*100)/currentStatus.fifoSize:0);
			}
#endif
			if((cMediaRunningState == DI_AUDIO_DATAEMPTY) && (unCurrentPts != 0) && (unCurrentPts != currentStatus.pts))
			{
				DI_VIDEO_RUNNING_STATE_EVT_t runningState;
				runningState.state=TRUE;
				(void)P_AUDIO_SendEventMessage(DI_AUDIO_DECODER_MAIN, DI_AUDIO_DATAFLOW);
				cMediaRunningState = DI_VIDEO_NOTIFY_EVT_END;
			}

			/*	Temp1 **********
			*	If Channel Mode changed, call the registered callbacks!
			*************************************************/
			if(i == DI_AUDIO_DECODER_MAIN)
			{
				if(currentStatus.sampleRate == 0)
				{
					PrintDebug("Decoder isn't ready yet or input stream is invalid!\n");
				}
			}
#if 0
			/*	Temp2 **********
			*	32KHz DDP stream should be downmixed !
			***************************************/
			if(i == DI_AUDIO_DECODER_MAIN)
			{
				if(currentStatus.sampleRate == 32000)
				{
					if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AC3_PLUS)
					{
						if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
						{
							dResult = P_AUDIO_ChangeHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);
							if(dResult != DRV_OK)
							{
								PrintError("P_AUDIO_ChangeHdmiOutput failed!\n");
								continue;
							}
							PrintError("Go to the ForceDownmix Mode!\n");
						}
					}
				}
				else if(currentStatus.sampleRate == 44100)
				{
					if((stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_PLUS) ||
						(stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_PLUS_LOAS) ||
						(stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_PLUS_ADTS) ||
						(stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC) ||
						(stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AAC_LOAS))
					{
						if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_COMPRESSED)
						{
							dResult = P_AUDIO_ChangeSpdifOutput(DI_AUDOUT_SPDIF_PCM);
							if(dResult != DRV_OK)
							{
								PrintError("P_AUDIO_ChangeSpdifOutput failed!\n");
								continue;
							}
							PrintError("Go to the ForceDownmix Mode!\n");
						}
						if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_TRANSCODING)
						{
							dResult = P_AUDIO_ChangeHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);
							if(dResult != DRV_OK)
							{
								PrintError("P_AUDIO_ChangeHdmiOutput failed!\n");
								continue;
							}
							PrintError("Go to the ForceDownmix Mode!\n");
						}
					}
				}
			}
#else
			/* DDC, DDT configuration. For more infomation, see the MS11 system menual document */
			if(i == DI_AUDIO_DECODER_MAIN
				&& stAudioStatus[DI_AUDIO_DECODER_MAIN].nFormat == DI_AUDFORMAT_AC3_PLUS)
			{
				if(currentStatus.sampleRate == 32000 || currentStatus.sampleRate == 44100)
				{
#if defined(CONFIG_DI10)
					if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_COMPRESSED)
					{
						dResult = P_AUDIO_ChangeSpdifOutput(DI_AUDOUT_SPDIF_PCM);
						if(dResult != DRV_OK)
						{
							PrintError("P_AUDIO_ChangeSpdifOutput failed!\n");
							continue;
						}
						PrintError("[%s] %d : SPDIF Go to the ForceDownmix Mode!\n", __FUNCTION__, __LINE__);
					}
#else
					if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_BYPASS
						|| stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_TRANSCODING)
					{
						dResult = P_AUDIO_ChangeSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
						if(dResult != DRV_OK)
						{
							PrintError("P_AUDIO_ChangeSpdifOutput failed!\n");
							continue;
						}
						PrintError("[%s] %d : SPDIF Go to the ForceDownmix Mode!\n", __FUNCTION__, __LINE__);
					}
#endif
					if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_TRANSCODING
						|| stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
					{
						dResult = P_AUDIO_ChangeHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);
						if(dResult != DRV_OK)
						{
							PrintError("P_AUDIO_ChangeHdmiOutput failed!\n");
							continue;
						}
						PrintError("[%s] %d : HDMI Go to the ForceDownmix Mode!\n", __FUNCTION__, __LINE__);
					}
				}
			}
#endif
			/*	2. Decoder monitoring Part	*****************************************
			*	Check the CDB level & current Pts value.
			*	If Pts value is being changed, the status is obviously DECODING.
			*	If CDB level is not zero and Pts is not changed,
			*	Scrambed data is coming into decoder. It is also dealt as NODATA.
			*	If both CDB level & Pts value are stable, it is NODATA.
			*************************************************************************/

			dResult = P_AUDIO_IsDataComing(i);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_IsDecoding failed!\n");
				continue;
			}

			if(stAudioStatus[i].bDataComing == FALSE)
			{//Nodata
				notify = DI_AUDIO_STATE_NODATA;
			}
			else if(stAudioStatus[i].bDataComing == TRUE)
			{
				if(stAudioStatus[i].prevPts == currentStatus.pts)
				{//scrambled data
					notify = DI_AUDIO_STATE_NODATA;
				}
				else if(stAudioStatus[i].prevPts != currentStatus.pts)
				{//Decoding
					notify = DI_AUDIO_STATE_DECODING;
					stAudioStatus[i].prevPts = currentStatus.pts;
				}
			}
			stAudioStatus[i].prevPts = currentStatus.pts;


			/*	3. Notifying Part	*************************************************
			*	Just let MW know what status the decoder has.
			*	NODATA should be sent up after Time out value.(stAudioStatus[i].eventLimit)
			*	DECODING should be sent up after 2 successful frame decodings.
			*************************************************************************/

			if(notify == DI_AUDIO_STATE_NODATA)
			{
				if(stAudioStatus[i].rState.state == DI_AUDIO_STATE_NODATA)
				{
					continue;
				}
				if(nodataLevel[i] == stAudioStatus[i].eventLimit)
				{
					decodingLevel[i] = 0;
					stAudioStatus[i].rState.state = DI_AUDIO_STATE_NODATA;
					dResult = P_AUDIO_SendEventMessage(i, DI_AUDIO_RUNNING_STATE_CHANGED);
					if(dResult != DRV_OK)
					{
						PrintError("P_AUDIO_SendEventMessage failed!\n");
						continue;
					}
				}
				else
				{
					nodataLevel[i]++;
				}
			}
			else if(notify == DI_AUDIO_STATE_DECODING)
			{
				if(stAudioStatus[i].rState.state == DI_AUDIO_STATE_DECODING)
				{
#if defined(CONFIG_AUDIO_ERROR_LOG) /* for audio error log */
					ulDecodeErr = ((int)(currentStatus.frameErrors- ulPrevDecodeErr) > 0 ? currentStatus.frameErrors - ulPrevDecodeErr : 0);
					ulPtsErr = ((int)(currentStatus.ptsErrorCount- ulPrevPtsErr) > 0 ? currentStatus.ptsErrorCount - ulPrevPtsErr : 0);

					if((ulDecodeErr > 0)||(ulPtsErr > 0))
					{
						stAudioStatus[DI_AUDIO_DECODER_MAIN].stAudErrInfo.ulFrameError = ulDecodeErr;
						stAudioStatus[DI_AUDIO_DECODER_MAIN].stAudErrInfo.ulPtsCountError = ulPtsErr;

						for(callbackNum=0; callbackNum<AUDIO_CALLBACK_OWNER_MAX; callbackNum++)
						{
							if(stAudioCallbacks[callbackNum].audioEventCallback[DI_AUDIO_DECODER_MAIN][DI_AUDIO_ERRORS_OCCURED] != NULL)
							{
								toMA.ulRequestId = 0; /* dummy */
								toMA.nEvent = DI_AUDIO_ERRORS_OCCURED;
								toMA.ulDecId = DI_AUDIO_DECODER_MAIN;
								toMA.pvEventData = &(stAudioStatus[DI_AUDIO_DECODER_MAIN].stAudErrInfo);
								stAudioCallbacks[callbackNum].audioEventCallback[DI_AUDIO_DECODER_MAIN][DI_AUDIO_ERRORS_OCCURED]((void*)&toMA);
							}
						}
						ulPrevDecodeErr = currentStatus.frameErrors;
						ulPrevPtsErr = currentStatus.ptsErrorCount;
					}
#endif
					continue;
				}
				if(decodingLevel[i] == DI_AUDIO_DECODING_EVENT_LIMIT)
				{
					nodataLevel[i] = 0;
					stAudioStatus[i].rState.state = DI_AUDIO_STATE_DECODING;
					dResult = P_AUDIO_SendEventMessage(i, DI_AUDIO_RUNNING_STATE_CHANGED);
					if(dResult != DRV_OK)
					{
						PrintError("P_AUDIO_SendEventMessage failed!\n");
						continue;
					}
				}
				else
				{
					decodingLevel[i]++;
				}
			}

			/*	4. Status Update	**************************************************
			*	stAudioStatus[].unmuteEnable & stAudioStatus[].bDecoding should be updated here.
			*	Just after successful informing MW current status.
			**************************************************************************/
			if(stAudioStatus[i].rState.state == DI_AUDIO_STATE_NODATA)
			{
				stAudioStatus[i].bDecoding = FALSE;
			}
			else if(stAudioStatus[i].rState.state == DI_AUDIO_STATE_DECODING)
			{
				stAudioStatus[i].bDecoding = TRUE;
			}
		}
		VK_TASK_Sleep(DI_AUDIO_UNDERRUN_TIMEOUT_RES);
	}
}

static void P_AudioStreamTask(void *param)
{
	BSTD_UNUSED(param);
	HINT32						vResult;
	NEXUS_Error 				nResult;
	DI_AUDIO_STREAM_MSG			audMsg;
	DRV_Error					dResult;
	HUINT32						EmptyChkCnt=0;
	NEXUS_AudioDecoderStatus 	status;
	HUINT32 pulTaskId = (HUINT32)param;
	HUINT32 callbackNum;
	DI_AUDIO_CALLBACK_DATA_t 		toMA;

	while(1)
	{
		VK_MEM_Memset(&toMA,0x00,sizeof(DI_AUDIO_CALLBACK_DATA_t));
		VK_MEM_Memset(&audMsg,0x00,sizeof(DI_AUDIO_STREAM_MSG));
		//1. MSG Receive
		vResult = VK_MSG_Receive(stAudioTaskInfo[pulTaskId].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG));
		if(vResult != VK_OK)
		{
			PrintError("[%s] [%d]\n", __FUNCTION__,__LINE__);
			continue;
		}
		VK_SEM_Get(s_ulAudioMemStreamId);

		if((stMemoryStreamPool[0].ulTimeTicket != audMsg.ulTimeTicket) && (stMemoryStreamPool[1].ulTimeTicket != audMsg.ulTimeTicket))
		{
			VK_SEM_Release(s_ulAudioMemStreamId);
			continue;
		}

		//2. Check the contents & function calls
		if(audMsg.sRequest == DI_AUDIO_MEMORY_STREAM_PLAY)
		{
			if(stAudioPlaypump[audMsg.ulTaskIndex].bStart == TRUE)
			{
				dResult = P_AUDIO_StartMemoryStream(audMsg);
				if(dResult != DRV_OK)
				{
					PrintError("P_AUDIO_StartMemoryStream Failed!\n");
				}

				dResult = P_AUDIO_SendEventMessage(audMsg.sDevId, DI_AUDIO_MEMORY_STREAM_STARTED);
				if(dResult != DRV_OK)
				{
					PrintError("P_AUDIO_SendEventMessage failed!\n");
				}
			}
		}
		else if(audMsg.sRequest == DI_AUDIO_MEMORY_STREAM_STOP)
		{
			dResult = P_AUDIO_SendEventMessage(audMsg.sDevId, DI_AUDIO_MEMORY_STREAM_COMPLETED);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_SendEventMessage failed!\n");
			}
		}
		else if(audMsg.sRequest == DI_AUDIO_MEMORY_STREAM_PAUSE)
		{
		}
		else if(audMsg.sRequest == DI_AUDIO_MEMORY_STREAM_MONITOR)
		{
			/*	if memory stream pause request has come in, just keep staring at ...
			*/
			if(stAudioStatus[audMsg.sDevId].nState == DI_AUDIO_PAUSED)
			{
				vResult = VK_MSG_SendTimeout(stAudioTaskInfo[audMsg.ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
				if(vResult != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
				}
				VK_SEM_Release(s_ulAudioMemStreamId);
				continue;
			}
			if(stAudioPlaypump[audMsg.ulTaskIndex].bStart == FALSE)
			{
				EmptyChkCnt = 0;
			}
			else
			{
				nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[audMsg.sDevId], &status);
				if(nResult != NEXUS_SUCCESS)
				{
					PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
					VK_SEM_Release(s_ulAudioMemStreamId);
					continue;
				}

				if (status.fifoDepth == stAudioStatus[audMsg.sDevId].last_depth)
				{
					if(EmptyChkCnt > 10)
					{
						EmptyChkCnt = 0;
						dResult = P_AUDIO_StopMemoryStream(audMsg.ulRequestId, audMsg.ulTaskIndex);
						if(dResult != DRV_OK)
						{
							PrintError("P_AUDIO_StopMemoryStream failed!\n");
						}

						VK_SEM_Release(s_ulAudioMemStreamId);
						continue;
					}
					else
					{
						EmptyChkCnt++;
					}
				}
				else
				{
					EmptyChkCnt = 0;
					stAudioStatus[audMsg.sDevId].last_depth = status.fifoDepth;
				}

				vResult = VK_MSG_SendTimeout(stAudioTaskInfo[audMsg.ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
				if(vResult != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
				}
			}
		}
		else if(audMsg.sRequest == DI_AUDIO_MEMORY_PCM_PLAY)
		{
			if(stAudioPlayback[audMsg.ulTaskIndex].bStart == TRUE)
			{
				dResult = P_AUDIO_PCMStreamplay_Start(audMsg);
				if(dResult != DRV_OK)
				{
					PrintError("P_AUDIO_PCMStreamplay_Start Failed!\n");
				}

				dResult = P_AUDIO_SendEventMessage(audMsg.sDevId, DI_AUDIO_MEMORY_STREAM_STARTED);
				if(dResult != DRV_OK)
				{
					PrintError("P_AUDIO_SendEventMessage failed!\n");
				}
			}
		}
		else if(audMsg.sRequest == DI_AUDIO_MEMORY_PCM_STOP)
		{
			for(callbackNum=0; callbackNum<AUDIO_CALLBACK_OWNER_MAX; callbackNum++)
			{
				if(stAudioCallbacks[callbackNum].audioEventCallback[DI_AUDIO_DECODER_MAIN][DI_AUDIO_MEMORY_STREAM_COMPLETED] != NULL)
				{
					toMA.ulRequestId = audMsg.ulRequestId;
					toMA.nEvent = DI_AUDIO_MEMORY_STREAM_COMPLETED;
					toMA.ulDecId = DI_AUDIO_DECODER_MAIN;
					toMA.pvEventData = &(stAudioStatus[DI_AUDIO_DECODER_MAIN].rState);
					stAudioCallbacks[callbackNum].audioEventCallback[DI_AUDIO_DECODER_MAIN][DI_AUDIO_MEMORY_STREAM_COMPLETED]((void*)&toMA);
				}
			}
		}
		VK_SEM_Release(s_ulAudioMemStreamId);

		VK_TASK_Sleep(10);
	}
}

#if __________drv___________
#endif

static void P_AUDIO_SetCodecSetting(DI_AUDIO_DECODER_TYPE decoderId)
{
	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);

	NEXUS_AudioDecoderCodecSettings codecSettings;

	NEXUS_AudioDecoder_GetCodecSettings(stAudioInstance.audioDecoder[decoderId], NEXUS_AudioCodec_eAc3, &codecSettings);
	codecSettings.codec = NEXUS_AudioCodec_eAc3;
	codecSettings.codecSettings.ac3.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eRf;
	codecSettings.codecSettings.ac3.drcModeDownmix = NEXUS_AudioDecoderDolbyDrcMode_eRf;
	codecSettings.codecSettings.ac3.stereoDownmixMode = NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic;
	codecSettings.codecSettings.ac3.dialogNormalization = TRUE;
	NEXUS_AudioDecoder_SetCodecSettings(stAudioInstance.audioDecoder[decoderId], &codecSettings);

	NEXUS_AudioDecoder_GetCodecSettings(stAudioInstance.audioDecoder[decoderId], NEXUS_AudioCodec_eAc3Plus, &codecSettings);
	codecSettings.codec = NEXUS_AudioCodec_eAc3Plus;
	codecSettings.codecSettings.ac3Plus.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eRf;
	codecSettings.codecSettings.ac3Plus.drcModeDownmix = NEXUS_AudioDecoderDolbyDrcMode_eRf;
	codecSettings.codecSettings.ac3Plus.stereoDownmixMode = NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic;
	codecSettings.codecSettings.ac3Plus.dialogNormalization = TRUE;
	NEXUS_AudioDecoder_SetCodecSettings(stAudioInstance.audioDecoder[decoderId], &codecSettings);

	NEXUS_AudioDecoder_GetCodecSettings(stAudioInstance.audioDecoder[decoderId], NEXUS_AudioCodec_eAac, &codecSettings);
	codecSettings.codec = NEXUS_AudioCodec_eAac;
	codecSettings.codecSettings.aac.drcMode = NEXUS_AudioDecoderDolbyPulseDrcMode_eRf;
#if defined(CONFIG_DOLBY_DOWNMIXMODE_LORO)
	codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLoRo;
#else
	codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
#endif
	codecSettings.codecSettings.aac.drcTargetLevel = 124;
#if (BCHP_CHIP != 7231)	// Kevin
	codecSettings.codecSettings.aac.drcDefaultLevel = 92;
#endif
	NEXUS_AudioDecoder_SetCodecSettings(stAudioInstance.audioDecoder[decoderId], &codecSettings);

	NEXUS_AudioDecoder_GetCodecSettings(stAudioInstance.audioDecoder[decoderId], NEXUS_AudioCodec_eAacPlus, &codecSettings);
	codecSettings.codec = NEXUS_AudioCodec_eAacPlus;
	codecSettings.codecSettings.aacPlus.drcMode = NEXUS_AudioDecoderDolbyPulseDrcMode_eRf;
#if defined(CONFIG_DOLBY_DOWNMIXMODE_LORO)
	codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLoRo;
#else
	codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
#endif
	codecSettings.codecSettings.aacPlus.drcTargetLevel = 124;
#if (BCHP_CHIP != 7231)	// Kevin
	codecSettings.codecSettings.aacPlus.drcDefaultLevel = 92;
#endif
	NEXUS_AudioDecoder_SetCodecSettings(stAudioInstance.audioDecoder[decoderId], &codecSettings);

}

static HBOOL P_AUDIO_CheckMixerTypeConvert(HUINT32 ulDevId, DI_AUDIO_FORMAT nType, DI_AUDIO_MIXER_TYPE *nMixerType)
{
	HBOOL bConvert = FALSE;
	DI_AUDIO_MIXER_TYPE curMixerType = DI_AUDIO_MIXER_TYPE_NONE;
	DI_AUDIO_MIXER_TYPE changeMixerType = DI_AUDIO_MIXER_TYPE_NONE;

	/*get current mixer type */
	curMixerType = stAudioMixerStatus.nMixerType;

	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		if(stAudioStatus[DI_AUDIO_DECODER_SUB].nState == DI_AUDIO_STOP)
		{
			/* Main audio start without starting sub audio */
			changeMixerType = DI_AUDIO_MIXER_TYPE_HW_MIXER;
		}
		else
		{
			/*Main audio start after starting sub audio */
			changeMixerType = curMixerType;
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		switch(nType)
		{
			/* Default & MPEG2 AD : HW Mixer */
#if !defined(CONFIG_TEMP_HDR4000T)
			/* for Fade-out feature on MPEG AD (DTG spec)*/
			case DI_AUDFORMAT_MPEG:
#endif
			case DI_AUDFORMAT_MP3:
			case DI_AUDFORMAT_PCM:
				changeMixerType = DI_AUDIO_MIXER_TYPE_HW_MIXER;
				break;

			/* MS 10&11 : HW Mixer */
#if defined(CONFIG_TEMP_HDR4000T)
			/* for Fade-out feature on MPEG AD (DTG spec)*/
			case DI_AUDFORMAT_MPEG:
#endif
			case DI_AUDFORMAT_AC3:
			case DI_AUDFORMAT_AC3_PLUS:
			case DI_AUDFORMAT_AAC:
			case DI_AUDFORMAT_AAC_LOAS:
			case DI_AUDFORMAT_AAC_PLUS:
			case DI_AUDFORMAT_AAC_PLUS_ADTS:
				changeMixerType = DI_AUDIO_MIXER_TYPE_DSP_MIXER;
				break;
			default:
				changeMixerType = DI_AUDIO_MIXER_TYPE_HW_MIXER;
				break;
		}
	}
	else
	{

	}

	if(curMixerType == changeMixerType)
	{
		bConvert= FALSE;
		*nMixerType = curMixerType;
		PrintDebug("[%s].%d Converting of Mixer type is not necessary now=%d \n",__FUNCTION__,__LINE__, curMixerType);
	}
	else
	{
		bConvert= TRUE;
		*nMixerType = changeMixerType;
		PrintDebug("[%s].%d Mixer type convert %d --> %d !!! \n",__FUNCTION__,__LINE__, curMixerType, changeMixerType);
	}

	return bConvert;
}

static DRV_Error P_AUDIO_ConvertMixerType(DI_AUDIO_MIXER_TYPE nMixerType)
{
	DRV_Error						dResult = DRV_OK;
	NEXUS_AudioMixerSettings		audioMixerSettings;
	NEXUS_AudioInput				input=NULL;

	/* Remove all input to mixer and close Mixer  */
	if(stAudioMixerStatus.bMixerOpened == TRUE)
	{
		/* Mixer output path is already disconnected by DRV_AUDIO_STOP() */
		NEXUS_AudioMixer_RemoveAllInputs(stAudioInstance.audioMixer);
		NEXUS_AudioMixer_Close(stAudioInstance.audioMixer);
		stAudioInstance.audioMixer = NULL;
		stAudioMixerStatus.bMixerOpened = FALSE;
		stAudioMixerStatus.nMixerType = DI_AUDIO_MIXER_TYPE_NONE;
	}

	/* Open mixer (HW type or DSP type) */
	NEXUS_AudioMixer_GetDefaultSettings(&audioMixerSettings);
	if(nMixerType == DI_AUDIO_MIXER_TYPE_DSP_MIXER)
	{
		audioMixerSettings.mixUsingDsp = TRUE;
	}
	else
	{
		audioMixerSettings.mixUsingDsp = FALSE;
	}

	stAudioInstance.audioMixer = NEXUS_AudioMixer_Open(&audioMixerSettings);
	stAudioMixerStatus.bMixerOpened = TRUE;

	if(audioMixerSettings.mixUsingDsp == TRUE)
	{
		stAudioMixerStatus.nMixerType = DI_AUDIO_MIXER_TYPE_DSP_MIXER;
	}
	else
	{
		stAudioMixerStatus.nMixerType = DI_AUDIO_MIXER_TYPE_HW_MIXER;
	}
	PrintDebug("[%s].%d Open Mixer type %d \n",__FUNCTION__,__LINE__, stAudioMixerStatus.nMixerType);

	/* Connect main decoder to Mixer  */
	dResult = P_AUDIO_AddMixerInputPath(DI_AUDIO_DECODER_MAIN);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_AddDacOutputPath failed!\n");
		return DRV_ERR;
	}

	/* Connect playback input to Mixer */
	input = NEXUS_AudioPlayback_GetConnector(stAudioPlayback[0].gPlayback);
	NEXUS_AudioMixer_AddInput(stAudioInstance.audioMixer, input);

	input = NEXUS_AudioPlayback_GetConnector(stAudioPlayback[1].gPlayback);
	NEXUS_AudioMixer_AddInput(stAudioInstance.audioMixer, input);

	return dResult;
}

static DRV_Error P_AUDIO_CheckSubDecoderModeChange(HUINT32 ulDevId, DI_AUDIO_FORMAT nType, DI_AUDIO_DECODER_MODE *decoderMode)
{
	HBOOL bChange = FALSE;
	DI_AUDIO_DECODER_MODE			changeMode = DI_AUDIO_DECODER_MODE_NONE;
	DI_AUDIO_DECODER_MODE			currentMode = DI_AUDIO_DECODER_MODE_NONE;

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);

	currentMode = stAudioStatus[DI_AUDIO_DECODER_SUB].nDecoderMode;

	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		if(stAudioStatus[DI_AUDIO_DECODER_SUB].nState == DI_AUDIO_STOP)
		{
			/* Main audio start without starting sub audio */
			changeMode = DI_AUDIO_DECODER_MODE_DECODER;
		}
		else
		{
			/*Main audio start after starting sub audio */
			changeMode = currentMode;
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		switch(nType)
		{
			case DI_AUDFORMAT_MPEG:
				changeMode = DI_AUDIO_DECODER_MODE_DESCRIPTION;
				break;
			case DI_AUDFORMAT_PCM:
			case DI_AUDFORMAT_MP3:
			case DI_AUDFORMAT_AC3:
			case DI_AUDFORMAT_AC3_PLUS:
			case DI_AUDFORMAT_AAC:
			case DI_AUDFORMAT_AAC_LOAS:
			case DI_AUDFORMAT_AAC_PLUS:
			case DI_AUDFORMAT_AAC_PLUS_ADTS:
				changeMode = DI_AUDIO_DECODER_MODE_DECODER;
				break;

			default:
				changeMode = DI_AUDIO_DECODER_MODE_DECODER;
				break;
		}
	}
	else
	{
	}

	if(currentMode == changeMode)
	{
		bChange = FALSE;
		*decoderMode = currentMode;
		PrintDebug("[%s].%d Sub decoder mode change is not necessary now=%d\n",__FUNCTION__,__LINE__, currentMode);
	}
	else
	{
		bChange = TRUE;
		*decoderMode = changeMode;
		PrintDebug("[%s].%d Sub decoder mode change from %d to change %d !!! \n",__FUNCTION__,__LINE__, currentMode, changeMode);
	}
	return bChange;
}

static DRV_Error P_AUDIO_SetSecondaryDecoderMode(DI_AUDIO_DECODER_MODE decoderMode, DI_AUDIO_MIXER_TYPE nMixerType)
{
	DRV_Error 						dResult = DRV_ERR;
	NEXUS_Error 					nResult;
	NEXUS_AudioDecoderSettings 		mainDecoderSettings;
	NEXUS_AudioDecoderOpenSettings 	audioOpenSettings;
	NEXUS_AudioMixerSettings		audioMixerSettings;

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);

	/* Open decoder (Descriptor or Decoder Mode) */
	if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_START)
	{
		NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN]);
	}

	/* Sub deocder Close */
	if(stAudioStatus[DI_AUDIO_DECODER_SUB].bDecoderOpened == TRUE)
	{
		NEXUS_AudioDecoder_Close(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB]);
		stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB] = NULL;
		stAudioStatus[DI_AUDIO_DECODER_SUB].bDecoderOpened = FALSE;
		stAudioStatus[DI_AUDIO_DECODER_SUB].nDecoderMode = DI_AUDIO_DECODER_MODE_NONE;
	}

	switch(decoderMode)
	{
		/* MS10 and MS11 only */
		case DI_AUDIO_DECODER_MODE_DECODER:
			PrintDebug("[%s].%d Reopend sub decoder : decoder \n",__FUNCTION__,__LINE__);
			NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioOpenSettings);
			//audioOpenSettings.fifoSize = tAudioCnxt->tBspCfgAudioCnxt->sDecorder[AKYO_AudioDecoderType_Secondary].uCdbSize;
			audioOpenSettings.type = NEXUS_AudioDecoderType_eDecode;
			audioOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_e5_1;
			stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB] = NEXUS_AudioDecoder_Open(DI_AUDIO_DECODER_SUB,&audioOpenSettings);
			stAudioStatus[DI_AUDIO_DECODER_SUB].bDecoderOpened = TRUE;
			stAudioStatus[DI_AUDIO_DECODER_SUB].nDecoderMode = DI_AUDIO_DECODER_MODE_DECODER;

			/* set sub decoder codec */
			P_AUDIO_SetCodecSetting(DI_AUDIO_DECODER_SUB);

			/* add decoder to mixer */
			dResult = P_AUDIO_AddMixerInputPath(DI_AUDIO_DECODER_SUB);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_AddDacOutputPath failed!\n");
				return DRV_ERR;
			}

			/* Set mixer master */
			if(nMixerType == DI_AUDIO_MIXER_TYPE_DSP_MIXER)
			{
				/* DSP Mixer */
				NEXUS_AudioMixer_GetSettings(stAudioInstance.audioMixer, &audioMixerSettings);
				audioMixerSettings.master = P_AUDIO_GetInput(DI_AUDIO_DECODER_MAIN, NEXUS_AudioDecoderConnectorType_eStereo);
				audioMixerSettings.mixUsingDsp = TRUE;
				NEXUS_AudioMixer_SetSettings(stAudioInstance.audioMixer, &audioMixerSettings);
			}
			else
			{
				/* HW Mixer */
				NEXUS_AudioMixer_GetSettings(stAudioInstance.audioMixer, &audioMixerSettings);
				audioMixerSettings.master = P_AUDIO_GetInput(DI_AUDIO_DECODER_MAIN, NEXUS_AudioDecoderConnectorType_eStereo);
				NEXUS_AudioMixer_SetSettings(stAudioInstance.audioMixer, &audioMixerSettings);
			}

			/* Set descriptionDecoder in main decoder to NULL  */
			NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &mainDecoderSettings);
			mainDecoderSettings.descriptionDecoder = NULL;
			NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &mainDecoderSettings);

#if defined(CONFIG_SYNC_CHANNEL)
			DRV_SYNC_SetAudioDecoderHandle(DRV_SYNC_AUDSUB, &stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB]);
#endif
			break;

		/* Non-MS10 or MS11. and Main audio only */
		case DI_AUDIO_DECODER_MODE_DESCRIPTION:
			PrintDebug("[%s].%d Reopend sub decoder : description \n",__FUNCTION__,__LINE__);
			NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioOpenSettings);
			audioOpenSettings.type = NEXUS_AudioDecoderType_eAudioDescriptor;
			audioOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_eNone;
			stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB] = NEXUS_AudioDecoder_Open(DI_AUDIO_DECODER_SUB, &audioOpenSettings);
			stAudioStatus[DI_AUDIO_DECODER_SUB].bDecoderOpened = TRUE;
			stAudioStatus[DI_AUDIO_DECODER_SUB].nDecoderMode = DI_AUDIO_DECODER_MODE_DESCRIPTION;

			/* add input path to mixer */
			dResult = P_AUDIO_AddMixerInputPath(DI_AUDIO_DECODER_SUB);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_AddDacOutputPath failed!\n");
				return DRV_ERR;
			}

			/* Set mixer master */
			NEXUS_AudioMixer_GetSettings(stAudioInstance.audioMixer, &audioMixerSettings);
			audioMixerSettings.master = P_AUDIO_GetInput(DI_AUDIO_DECODER_MAIN, NEXUS_AudioDecoderConnectorType_eStereo);
			NEXUS_AudioMixer_SetSettings(stAudioInstance.audioMixer, &audioMixerSettings);

			/* Set descriptionDecoder in main decoder to Sub decoder  */
			NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &mainDecoderSettings);
			mainDecoderSettings.descriptionDecoder = stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB];
			NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &mainDecoderSettings);

#if defined(CONFIG_SYNC_CHANNEL)
			DRV_SYNC_SetAudioDecoderHandle(DRV_SYNC_AUDSUB, &stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SUB]);
#endif

			break;

		default:
			break;
	}

	if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_START)
	{
		nResult = NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[0], &stAudioStatus[0].audioSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_Start failed!\n");
			return DRV_ERR;
		}
	}

	/* Set init stAudioStatus of sub decoder */
#if defined(CONFIG_DI10)
#else
	stAudioStatus[DI_AUDIO_DECODER_SUB].nType = DI_AUDIOTYPE_STREAM;
#endif
	stAudioStatus[DI_AUDIO_DECODER_SUB].nState = DI_AUDIO_STOP;
	stAudioStatus[DI_AUDIO_DECODER_SUB].nFormat = DI_AUDFORMAT_NONE;

	/* Init delay of decoder */
	dResult = P_AUDIO_DelayDecoder(DI_AUDIO_DECODER_SUB, 0);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_DelayDecoder failed!\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_AUDIO_SetPidChannelForTrickPlay(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{
	DRV_Error dResult = DRV_OK;
	HBOOL isSubAudioPid = FALSE;
	HBOOL bPbPidChannel = FALSE;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode;
	NEXUS_PidChannelHandle ulPidChannelHandle=NULL;
	NEXUS_AudioDecoderHandle ulPrimaryDecoder=NULL;
	NEXUS_AudioDecoderHandle ulSecondaryDecoder=NULL;

#if defined(CONFIG_DI20)
	BSTD_UNUSED(nType);
#endif

#if defined(CONFIG_DI10)
	if(nType < DI_AUDFORMAT_MEDIA_MPEG)
#else
	if(stAudioStatus[ulDevId].nType == DI_AUDIOTYPE_STREAM)
#endif
	{
#if defined(CONFIG_PLAYBACK)
		if(ulDevId == DI_AUDIO_DECODER_MAIN || ulDevId == DI_AUDIO_DECODER_SUB)
		{
			/* Check Record whether playback or TS play*/
			bPbPidChannel = DRV_DEMUX_IsPlaybackPidChannel(stAudioStatus[ulDevId].audioSettings.pidChannel, &ePlaybackMode);
			if(bPbPidChannel)
			{
				/* Record playback */
				if(stAudioStatus[ulDevId].nState == DI_AUDIO_START)
				{
					/*
					Set pidChannel params.
					When pidChannel is opened, primary & secondary are set to NULL .
					Set decoder handle as primary.
					*/
					ulPidChannelHandle = stAudioStatus[ulDevId].audioSettings.pidChannel;
					ulPrimaryDecoder = stAudioInstance.audioDecoder[ulDevId];
					ulSecondaryDecoder = NULL;
					isSubAudioPid = (ulDevId == DI_AUDIO_DECODER_SUB ? TRUE : FALSE);
				}
				else
				{
					PrintError("[%s] [%d]Error : ulDevId(0x%X) was not started !!! \n",__FUNCTION__,__LINE__, ulDevId);
					return DRV_ERR;
				}
			}
			else
			{
				/* TS play */
				return DRV_OK;
			}
		}
		else if(ulDevId == DI_AUDIO_DECODER_SPDIF)
		{
			/* Check Record whether playback or TS play*/
			bPbPidChannel = DRV_DEMUX_IsPlaybackPidChannel(stAudioStatus[DI_AUDIO_DECODER_MAIN].audioSettings.pidChannel, &ePlaybackMode);
			if(bPbPidChannel)
			{
				/* Record playback */
				if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_START &&
					stAudioStatus[DI_AUDIO_DECODER_SPDIF].nState == DI_AUDIO_START)
				{
					/*
					Set pidChannel params.
					When pidChannel is opened, primary & secondary are set to NULL .
					Set primary(to main decoder) & secondary(to passThrough decoder) decoder handle.
					*/
					ulPidChannelHandle = stAudioStatus[DI_AUDIO_DECODER_MAIN].audioSettings.pidChannel;
					ulPrimaryDecoder = stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN];
					ulSecondaryDecoder = stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SPDIF];
					isSubAudioPid = FALSE;
				}
				else
				{
					PrintError("[%s] [%d]Error : Main or SPDIF was not started !!! \n",__FUNCTION__,__LINE__);
					return DRV_ERR;
				}
			}
			else
			{
				/* TS play */
				return DRV_OK;
			}
		}
		else
		{
			PrintError("[%s] [%d]Error : Unknown decoder index (0x%X) \n",__FUNCTION__,__LINE__, ulDevId);
			return DRV_ERR;
		}

		PrintDebug("[%s] [%d] DRV_PVR_PLAY_AudioPidChannelSetting !!! \n",__FUNCTION__,__LINE__);
		dResult = DRV_PVR_PLAY_AudioPidChannelSetting(ulPidChannelHandle, isSubAudioPid, ulPrimaryDecoder, ulSecondaryDecoder);
		if (dResult != DRV_OK)
		{
			PrintError("[%s] [%d]Error(0x%X) \r\n",__FUNCTION__,__LINE__, dResult);
			return DRV_ERR;
		}
#endif
	}
#if defined(CONFIG_DI10)
	else if((DI_AUDFORMAT_MEDIA_MPEG <= nType) && (nType < DI_AUDFORMAT_MEDIA_DRA))
#else
	else if(stAudioStatus[DI_AUDIO_DECODER_SPDIF].nType == DI_AUDIOTYPE_MEDIA)
#endif
	{
		/*
		Media play.
		When pidChannel is opened, secondary is only NULL. (Primary already is set to Main decoder)
		Set secondary (to passThrough decoder) decoder handle.
		*/
		if(ulDevId == DI_AUDIO_DECODER_SPDIF)
		{
			if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_START &&
				stAudioStatus[DI_AUDIO_DECODER_SPDIF].nState == DI_AUDIO_START)
			{
#if !defined(CONFIG_MEDIA) && !defined(CONFIG_CALYPSO)
				PrintError("[%s] [%d] Not support MEDIA format. Error !!! \n",__FUNCTION__,__LINE__);
				return DRV_ERR;
#endif
#if defined(CONFIG_MEDIA)
				if(DRV_MEDIA_CheckStart())
				{
					PrintDebug("[%s] [%d] DRV_MEDIA_SetAudioPidChannel(Main, passthrough) !!! \n",__FUNCTION__,__LINE__);
					dResult = DRV_MEDIA_SetAudioPidChannel(stAudioStatus[DI_AUDIO_DECODER_MAIN].audioSettings.pidChannel,
															stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN],
															stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SPDIF]);
				}
				if ( dResult != DRV_OK )
				{
					PrintError("[%s] [%d]Error(0x%X) \r\n",__FUNCTION__,__LINE__, dResult);
					VK_SEM_Release(s_ulAudioDISemId);
					return DRV_ERR;
				}
#endif
#if defined(CONFIG_CALYPSO)
				if(DRV_CSO_CheckStart())
				{
					PrintDebug("[%s] [%d] DRV_CSO_SetAudioPidChannel(Main, passthrough) !!! \n",__FUNCTION__,__LINE__);
					dResult = DRV_CSO_SetAudioPidChannel(stAudioStatus[DI_AUDIO_DECODER_MAIN].audioSettings.pidChannel,
															stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN],
															stAudioInstance.audioDecoder[DI_AUDIO_DECODER_SPDIF]);
				}
				if ( dResult != DRV_OK )
				{
					PrintError("[%s] [%d]Error(0x%X) \r\n",__FUNCTION__,__LINE__, dResult);
					VK_SEM_Release(s_ulAudioDISemId);
					return DRV_ERR;
				}
#endif
			}
			else
			{
				PrintError("[%s] [%d]Error : Main or SPDIF was not started !!! \n",__FUNCTION__,__LINE__);
				return DRV_ERR;
			}
		}
	}
	else
	{
		PrintError("[%s] [%d] Error !!! \n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}
#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO) || defined(CONFIG_AUDIO_DEC_FLAC)
static DRV_Error P_AUDIO_GetHRAOutputSampleRate(DI_AUDIO_SAMPLE_RATE_e eInput, DI_AUDIO_SAMPLE_RATE_e *eOutput)
{
	DI_ERR_CODE 				diResult = DI_ERR_OK;

	switch(eInput)
	{
		case eDI_AUDIO_SR_32KHz:
			*eOutput = eDI_AUDIO_SR_32KHz;
			break;

		case eDI_AUDIO_SR_44KHz:
			*eOutput = eDI_AUDIO_SR_44KHz;
			break;

		case eDI_AUDIO_SR_48KHz:
			*eOutput = eDI_AUDIO_SR_48KHz;
			break;

		case eDI_AUDIO_SR_88KHz:
			*eOutput = eDI_AUDIO_SR_88KHz;
			break;

		case eDI_AUDIO_SR_96KHz:
			*eOutput = eDI_AUDIO_SR_96KHz;
			break;

		case eDI_AUDIO_SR_176KHz:
			*eOutput = eDI_AUDIO_SR_88KHz;
			break;

		case eDI_AUDIO_SR_192KHz:
			*eOutput = eDI_AUDIO_SR_96KHz;
			break;

		default :
			PrintError("[%s] ERROR : input sample rate is wroung ! %d \n", __FUNCTION__, eInput);
			*eOutput = eDI_AUDIO_SR_MAX;
			diResult = DI_ERR_ERROR;
			break;
	}

	return diResult;
}
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
static DRV_Error P_AUDIO_OpenPrimers(HUINT32 ulDevId)
{
	HUINT32	i = 0;
	HUINT32 ulOpenedPrimerCnt = 0;

	for(i=0; i < AUDIO_PRIMER_MAX_COUNT; i++)
	{
		/* Open audio primer */
		stMultiTrackInfo[ulDevId].hPrimer[i] = NEXUS_AudioDecoderPrimer_Open(stAudioInstance.audioDecoder[ulDevId]);
		if(stMultiTrackInfo[ulDevId].hPrimer[i] == NULL)
		{
			PrintError("[%s] audio decoder primer[%d] open fail (ulDevId=%d) \n", __FUNCTION__, i, ulDevId);
			goto RET_ERR;
		}
		ulOpenedPrimerCnt++;
	}

	return DRV_OK;

RET_ERR :

	if(ulOpenedPrimerCnt > 0)
	{
		for(i=0; i<ulOpenedPrimerCnt; i++)
		{
			NEXUS_AudioDecoderPrimer_Close(stMultiTrackInfo[ulDevId].hPrimer[i]);
		}
	}
	return DRV_ERR;
}

static DRV_Error P_AUDIO_StartPrimers(HUINT32 ulDevId, HUINT32 ulPrimerIndex)
{
	HUINT32 ulIndex = 0;
	DRV_Error dResult = DRV_OK;
	NEXUS_Error nResult = NEXUS_SUCCESS;
	NEXUS_PlaybackPidChannelSettings stPbPidSettings;
	NEXUS_StcChannelHandle stcChannel;
	NEXUS_PidChannelHandle pidChannel;

	HAPPY(stcChannel);
	HAPPY(pidChannel);
	HAPPY(stPbPidSettings);

	PrintDebug("[%s].%d ulDevId=%d ulPrimerIndex=%d\n",__FUNCTION__,__LINE__, ulDevId, ulPrimerIndex);

	ulIndex = ulPrimerIndex;

	if(stMultiTrackInfo[ulDevId].eAudioType == DI_AUDIOTYPE_STREAM)
	{
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
		/* Live TS play */
		NEXUS_AudioDecoder_GetDefaultStartSettings(&stMultiTrackInfo[ulDevId].decoderSettings[ulIndex]);

		/* set secondary decoder */
		if(ulDevId == DI_AUDIO_DECODER_SUB)
		{
			stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].secondaryDecoder = true;
		}

		/* set codec list */
		stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].codec = P_AUDIO_ConvertCodecToNexus(stMultiTrackInfo[ulDevId].eFormat[ulIndex]);

		/* set pid channels */
		dResult = DRV_DEMUX_GetAudioPrimerPidChannel(ulIndex, &(stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel));
		if(dResult != DRV_OK)
		{
			PrintError("[%s] DRV_DEMUX_GetAudioPrimerPidChannel is failed (ulDevId=%d)(index=%d)\n", __FUNCTION__, ulDevId, ulIndex);
			return dResult;
		}

		/* set STC channel */
		stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(ulIndex);

		/* Start primer */
		nResult = NEXUS_AudioDecoderPrimer_Start(stMultiTrackInfo[ulDevId].hPrimer[ulIndex], &stMultiTrackInfo[ulDevId].decoderSettings[ulIndex]);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_AudioDecoderPrimer_Start fail (ulIndex=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			return DRV_ERR;
		}
#else
		PrintError("[%s] eAudioType is wrong (ulIndex=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
		return DRV_ERR;
#endif
	}
	else if(stMultiTrackInfo[ulDevId].eAudioType == DI_AUDIOTYPE_MEDIA)
	{
#if defined(CONFIG_FAST_AUDIO_CHANGE)
		/* Media play */
		DI_MEDIA_AUDIO_TYPE_e eMediaAudioType;
		DI_MEDIA_AUDIO_FORMAT_e eMediaAudioCodec;

		switch (ulDevId)
		{
			case DI_AUDIO_DECODER_MAIN:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_MAIN;
				break;

			case DI_AUDIO_DECODER_SUB:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_SUB;
				break;

			case DI_AUDIO_DECODER_SPDIF:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_SPDIF;
				break;

			default:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_NONE;
				break;
		}

		if (eMediaAudioType == DI_MEDIA_AUDIO_DECODER_NONE)
		{
			PrintError("[%s] Invalide MediaAudioType \n", __FUNCTION__);
			return DRV_ERR;
		}

		eMediaAudioCodec = P_AUDIO_ConvertCodecToMedia(stMultiTrackInfo[ulDevId].eFormat[ulIndex]);
		if(eMediaAudioCodec == DI_MEDIA_AUDIO_FORMAT_NONE)
		{
			PrintError("[%s] Invalide MediaAudioCodec \n",__FUNCTION__);
			return DRV_ERR;
		}

#if defined(CONFIG_MEDIA)
		if(DRV_MEDIA_CheckStart())
		{
			pidChannel = DRV_MEDIA_GetPlaybackAudioPidChannel(eMediaAudioCodec, eMediaAudioType, (void *)stAudioInstance.audioDecoder[ulDevId]);
			stcChannel = DRV_MEDIA_MP_GetPlaybackStcChannelHandle(DI_MEDIA_VIDEO_DECODER_NONE,eMediaAudioType, DI_MEDIA_PLAYPUMP_AUDIO_MAIN);
		}
#endif
#if defined(CONFIG_CALYPSO)
		if(DRV_CSO_CheckStart())
		{
			pidChannel = DRV_CSO_GetPlaypumpAudioPidChannel(eMediaAudioCodec,eMediaAudioType, (void *)stAudioInstance.audioDecoder[ulDevId]);
			stcChannel = DRV_CSO_GetPlaypumpStcChannelHandle(DI_MEDIA_VIDEO_DECODER_NONE,eMediaAudioType,DI_MEDIA_PLAYPUMP_AUDIO_MAIN);
		}
#endif

		NEXUS_AudioDecoder_GetDefaultStartSettings(&stMultiTrackInfo[ulDevId].decoderSettings[ulIndex]);

		/* set secondary decoder */
		if(ulDevId == DI_AUDIO_DECODER_SUB)
		{
			stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].secondaryDecoder = true;
		}

		/* set codec list */
		stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].codec = P_AUDIO_ConvertCodecToNexus(stMultiTrackInfo[ulDevId].eFormat[ulIndex]);

		/* set pid channels */
#if defined(CONFIG_MEDIA)
#if defined(CONFIG_DI10)
		if(DRV_MEDIA_CheckStart())
		{
			dResult = DRV_MEDIA_GetAudioTrackPidChannel(ulIndex, &(stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel));
		}
		if(dResult != DRV_OK)
		{
			PrintError("[%s] DRV_MEDIA_GetAudioTrackPidChannel is failed (ulDevId=%d)(index=%d)\n", __FUNCTION__, ulDevId, ulIndex);
			return dResult;
		}
#endif
#endif
#if defined(CONFIG_CALYPSO)
		if(DRV_CSO_CheckStart())
		{
			stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel = pidChannel;
		}
#endif

		/* set STC channel */
		stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].stcChannel = stcChannel;

		/* Start primer */
		nResult = NEXUS_AudioDecoderPrimer_Start(stMultiTrackInfo[ulDevId].hPrimer[ulIndex], &stMultiTrackInfo[ulDevId].decoderSettings[ulIndex]);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_AudioDecoderPrimer_Start fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			return DRV_ERR;
		}

#if defined(CONFIG_MEDIA) && !defined(CONFIG_CALYPSO)
#if defined(CONFIG_DI10)
		/* Get Playback handle */
		dResult = DRV_MEDIA_GetPlaybackHandle(&stMultiTrackInfo[ulDevId].hPlayBack);
		if(stMultiTrackInfo[ulDevId].hPlayBack == NULL || dResult != DRV_OK)
		{
			PrintError("ERROR : DRV_MEDIA_GetPlaybackHandle hPlayBack 0x%x dResult %d \n", stMultiTrackInfo[ulDevId].hPlayBack, dResult);
			goto RET_ERR;
		}

		/* Set primer handle to playback pid channel */
		nResult = NEXUS_Playback_GetPidChannelSettings(stMultiTrackInfo[ulDevId].hPlayBack,
														stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel,
														&stPbPidSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Playback_GetPidChannelSettings fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			goto RET_ERR;
		}

		stPbPidSettings.pidTypeSettings.audio.primer = stMultiTrackInfo[ulDevId].hPrimer[ulIndex];
		nResult = NEXUS_Playback_SetPidChannelSettings(stMultiTrackInfo[ulDevId].hPlayBack,
														stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel,
														&stPbPidSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Playback_SetPidChannelSettings fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			goto RET_ERR;
		}
#endif
#endif
#else
		PrintError("[%s] eAudioType is wrong (ulIndex=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
		return DRV_ERR;
#endif
	}
	else if(stMultiTrackInfo[ulDevId].eAudioType == DI_AUDIOTYPE_PLAYBACK)
	{
#if defined(CONFIG_FAST_AUDIO_CHANGE)
		/* Playback play */
		NEXUS_AudioDecoder_GetDefaultStartSettings(&stMultiTrackInfo[ulDevId].decoderSettings[ulIndex]);

		/* set secondary decoder */
		if(ulDevId == DI_AUDIO_DECODER_SUB)
		{
			stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].secondaryDecoder = true;
		}

		/* set codec list */
		stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].codec = P_AUDIO_ConvertCodecToNexus(stMultiTrackInfo[ulDevId].eFormat[ulIndex]);

		/* set pid channels */
		dResult = DRV_DEMUX_GetAudioPrimerPidChannel(ulIndex, &(stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel));
		if(dResult != DRV_OK)
		{
			PrintError("[%s] DRV_DEMUX_GetAudioPrimerPidChannel is failed (ulDevId=%d)(index=%d)\n", __FUNCTION__, ulDevId, ulIndex);
			return dResult;
		}

		/* set STC channel */
		if(stAudioSyncStatus.bTsm == TRUE)
		{
			stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].stcChannel = DRV_DEMUX_GetStcChannelHandle();
		}
		else
		{
			stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].stcChannel = NULL;
		}

		/* Start primer */
		nResult = NEXUS_AudioDecoderPrimer_Start(stMultiTrackInfo[ulDevId].hPrimer[ulIndex], &stMultiTrackInfo[ulDevId].decoderSettings[ulIndex]);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_AudioDecoderPrimer_Start fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			return DRV_ERR;
		}

		/* Get Playback handle */
		dResult = DRV_PVR_PB_GetPlaybackHandle(stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel,
												&stMultiTrackInfo[ulDevId].hPlayBack);
		if(stMultiTrackInfo[ulDevId].hPlayBack == NULL || dResult != DRV_OK)
		{
			PrintError("ERROR : DRV_PVR_PB_GetPlaybackHandle hPlayBack 0x%x dResult %d \n", stMultiTrackInfo[ulDevId].hPlayBack, dResult);
			goto RET_ERR;
		}

		/* Set primer handle to playback pid channel */
		nResult = NEXUS_Playback_GetPidChannelSettings(stMultiTrackInfo[ulDevId].hPlayBack,
														stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel,
														&stPbPidSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Playback_GetPidChannelSettings fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			goto RET_ERR;
		}

		stPbPidSettings.pidTypeSettings.audio.primer = stMultiTrackInfo[ulDevId].hPrimer[ulIndex];
		nResult = NEXUS_Playback_SetPidChannelSettings(stMultiTrackInfo[ulDevId].hPlayBack,
														stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel,
														&stPbPidSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Playback_SetPidChannelSettings fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			goto RET_ERR;
		}
#else
		PrintError("[%s] eAudioType is wrong (ulIndex=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
		return DRV_ERR;
#endif
	}
	else
	{
		PrintError("[%s] Unknown Audio Type (ulDevId=%d)(Type=%d)\n", __FUNCTION__, ulDevId, stMultiTrackInfo[ulDevId].eAudioType);
		return DRV_ERR;
	}

	stMultiTrackInfo[ulDevId].bOccupied[ulIndex] = TRUE;
	stMultiTrackInfo[ulDevId].bStartPrimer = TRUE;

	return DRV_OK;

#if defined(CONFIG_FAST_AUDIO_CHANGE)
RET_ERR :
	NEXUS_AudioDecoderPrimer_Stop(stMultiTrackInfo[ulDevId].hPrimer[ulIndex]);
	stMultiTrackInfo[ulDevId].bOccupied[ulIndex] = FALSE;
#endif

	return DRV_ERR;
}


static DRV_Error P_AUDIO_StopPrimers(HUINT32 ulDevId, HUINT32 ulPrimerIndex)
{
	HBOOL bPrimerStart = FALSE;
	HUINT32 ulIndex = 0;
	NEXUS_Error nResult = NEXUS_SUCCESS;
	NEXUS_PlaybackPidChannelSettings stPbPidSettings;

	PrintDebug("[%s].%d ulDevId=%d ulPrimerIndex=%d\n",__FUNCTION__,__LINE__, ulDevId, ulPrimerIndex);

	ulIndex = ulPrimerIndex;
	if(stMultiTrackInfo[ulDevId].eAudioType == DI_AUDIOTYPE_STREAM)
	{
		/* stop primer */
		NEXUS_AudioDecoderPrimer_Stop(stMultiTrackInfo[ulDevId].hPrimer[ulIndex]);
	}
	else if(stMultiTrackInfo[ulDevId].eAudioType == DI_AUDIOTYPE_MEDIA ||
			stMultiTrackInfo[ulDevId].eAudioType == DI_AUDIOTYPE_PLAYBACK)
	{
		/* Set primer handle to pid channel as NULL */
		nResult = NEXUS_Playback_GetPidChannelSettings(stMultiTrackInfo[ulDevId].hPlayBack,
														stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel,
														&stPbPidSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Playback_GetPidChannelSettings fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			return DRV_ERR;
		}

		stPbPidSettings.pidTypeSettings.audio.primer = NULL;
		nResult = NEXUS_Playback_SetPidChannelSettings(stMultiTrackInfo[ulDevId].hPlayBack,
														stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel,
														&stPbPidSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Playback_SetPidChannelSettings fail (i=%d)(ulDevId=%d)\n", __FUNCTION__, ulIndex, ulDevId);
			return DRV_ERR;
		}

		/* stop primer */
		NEXUS_AudioDecoderPrimer_Stop(stMultiTrackInfo[ulDevId].hPrimer[ulIndex]);
	}
	else
	{
		PrintError("[%s] Unknown Audio Type (ulDevId=%d)(Type=%d)\n", __FUNCTION__, ulDevId, stMultiTrackInfo[ulDevId].eAudioType);
		return DRV_ERR;
	}

	stMultiTrackInfo[ulDevId].bOccupied[ulIndex] = FALSE;

	for(ulIndex=0; ulIndex < AUDIO_PRIMER_MAX_COUNT; ulIndex++)
	{
		if(stMultiTrackInfo[ulDevId].bOccupied[ulIndex] == TRUE)
		{
			bPrimerStart = TRUE;
		}
	}

	stMultiTrackInfo[ulDevId].bStartPrimer = bPrimerStart;

	return DRV_OK;
}

static DRV_Error P_AUDIO_StopPrimerAndStartDecode(HUINT32 ulDevId, HUINT32 ulStartTrackIndex, DI_AUDIO_FORMAT nType)
{
	HUINT32 ulIndex = 0;
	NEXUS_Error nResult = NEXUS_SUCCESS;

	PrintDebug("[%s].%d ulDevId=%d index=%d \n",__FUNCTION__,__LINE__, ulDevId, ulStartTrackIndex);

	ulIndex = ulStartTrackIndex;
	if(stMultiTrackInfo[ulDevId].eFormat[ulIndex] != nType)
	{
		PrintError("[%s] Format miss match !!! (ulDevId=%d)(index=%d)\n", __FUNCTION__, ulDevId, ulIndex);
		return DRV_ERR;
	}

	nResult = NEXUS_AudioDecoderPrimer_StopPrimerAndStartDecode(stMultiTrackInfo[ulDevId].hPrimer[ulIndex], stAudioInstance.audioDecoder[ulDevId]);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_AudioDecoderPrimer_StopPrimerAndStartDecode fail (ulDevId=%d)(index=%d)\n", __FUNCTION__, ulDevId, ulIndex);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_AUDIO_StopDecodeAndStartPrimer(HUINT32 ulDevId, HUINT32 ulStopTrackIndex)
{
	HUINT32 ulIndex = 0;
	NEXUS_Error nResult = NEXUS_SUCCESS;

	PrintDebug("[%s].%d ulDevId=%d index=%d \n",__FUNCTION__,__LINE__, ulDevId, ulStopTrackIndex);

	ulIndex = ulStopTrackIndex;

	/* stop audio decoding & start primer */
	nResult = NEXUS_AudioDecoderPrimer_StopDecodeAndStartPrimer(stMultiTrackInfo[ulDevId].hPrimer[ulIndex], stAudioInstance.audioDecoder[ulDevId]);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_AudioDecoderPrimer_StopDecodeAndStartPrimer fail (ulDevId=%d)(index=%d)\n", __FUNCTION__, ulDevId, ulIndex);
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

DRV_Error DRV_AUDIO_AddDacOutputPath(HUINT32 ulDevId)
{
	DRV_Error eDrvRet = DRV_OK;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvRet=P_AUDIO_AddDacOutputPath(ulDevId);
	if(eDrvRet != DRV_OK)
	{
		PrintError("[%s] ERROR : DRV_AUDIO_AddDacOutputPath failed\n",__FUNCTION__);
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	return eDrvRet;
}

DRV_Error DRV_AUDIO_AddHdmiOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{
	DRV_Error eDrvRet = DRV_OK;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvRet=P_AUDIO_AddHdmiOutputPath(ulDevId, nType);
	if(eDrvRet != DRV_OK)
	{
		PrintError("[%s] ERROR : DRV_AUDIO_AddHdmiOutputPath failed\n",__FUNCTION__);
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	return eDrvRet;
}

DRV_Error DRV_AUDIO_AddSpdifOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{
	DRV_Error eDrvRet = DRV_OK;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvRet=P_AUDIO_AddSpdifOutputPath(ulDevId, nType);
	if(eDrvRet != DRV_OK)
	{
		PrintError("[%s] ERROR : DRV_AUDIO_AddSpdifOutputPath failed\n",__FUNCTION__);
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	return eDrvRet;
}

DRV_Error DRV_AUDIO_RemoveDacOutputPath(HUINT32 ulDevId)
{
	DRV_Error eDrvRet = DRV_OK;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvRet=P_AUDIO_RemoveDacOutputPath(ulDevId);
	if(eDrvRet != DRV_OK)
	{
		PrintError("[%s] ERROR : DRV_AUDIO_RemoveDacOutputPath failed\n",__FUNCTION__);
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	return eDrvRet;
}

DRV_Error DRV_AUDIO_RemoveHdmiOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{
	DRV_Error eDrvRet = DRV_OK;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvRet=P_AUDIO_RemoveHdmiOutputPath(ulDevId, nType);
	if(eDrvRet != DRV_OK)
	{
		PrintError("[%s] ERROR : DRV_AUDIO_RemoveHdmiOutputPath failed\n",__FUNCTION__);
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	return eDrvRet;
}

DRV_Error DRV_AUDIO_RemoveSpdifOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{
	DRV_Error eDrvRet = DRV_OK;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvRet=P_AUDIO_RemoveSpdifOutputPath(ulDevId, nType);
	if(eDrvRet != DRV_OK)
	{
		PrintError("[%s] ERROR : DRV_AUDIO_RemoveSpdifOutputPath failed\n",__FUNCTION__);
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	return eDrvRet;
}

DRV_Error DRV_AUDIO_Init(void)
{//Sunwoo
	HUINT32 i,j,k=0;
	HUINT32 ulDevId;
	HUINT32 ulTaskId=0;
	HINT32	vResult;
	DRV_Error dResult;
	NEXUS_AudioInput input;
	NEXUS_AudioOutput output;
	NEXUS_Error nResult;
	NEXUS_AudioOutputSettings nSettings;
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_AudioDecoderOpenSettings openSettings;
	NEXUS_AudioDecoderSettings decoderSettings;
#if defined(CONFIG_BLUETOOTH)
	NEXUS_AudioCaptureOpenSettings tCaptureOpenSettings;
#endif

	NEXUS_Platform_GetConfiguration(&platformConfig);
	VK_MEM_Memcpy(stAudioOutput.audioDacs, platformConfig.outputs.audioDacs, sizeof(NEXUS_AudioDacHandle)*NEXUS_NUM_AUDIO_DACS );
	VK_MEM_Memcpy(stAudioOutput.spdifOutputs, platformConfig.outputs.spdif, sizeof(NEXUS_SpdifOutputHandle)*NEXUS_NUM_SPDIF_OUTPUTS );
	VK_MEM_Memcpy(stAudioOutput.i2sOutputs, platformConfig.outputs.i2s, sizeof(NEXUS_I2sOutputHandle)*NEXUS_NUM_I2S_OUTPUTS );

	stAudioOutput.audioDacPortNum = 0;
	stAudioOutput.spdifOutPortNum = 0;
	stAudioOutput.i2sOutPortNum = 0;
	stAudioInstance.bInitialized = TRUE;

	VK_MEM_Memset(&(stAudioOutput.stTranscodingInfo), 0x00, sizeof(stAudioOutput.stTranscodingInfo));

	stAudioOutput.eDacInput = DI_AUDIO_INPUT_NONE;
	stAudioOutput.eHdmiInput = DI_AUDIO_INPUT_NONE;
	stAudioOutput.eSpdifInput = DI_AUDIO_INPUT_NONE;

#if defined(NEXUS_NUM_AUDIO_DUMMY_OUTPUTS)
	for (i=0; i<NEXUS_NUM_AUDIO_DUMMY_OUTPUTS; i++)
	{
		stAudioInstance.audioDummyOutput[i].bUsedDummy = FALSE;
		stAudioInstance.audioDummyOutput[i].input = NULL;
	}
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	for(i=0; i<AUDIO_ALL_DECODERS; i++)
	{
		VK_MEM_Memset(&(stMultiTrackInfo[i]), 0x00, sizeof(DI_AudioMultiTrack_t));
	}
#endif


/*************************
*	1. Decoder handle open & Setting
*************************/
	/* Main decoder  */
	NEXUS_AudioDecoder_GetDefaultOpenSettings(&openSettings);
	ulDevId = DI_AUDIO_DECODER_MAIN;
	openSettings.type = NEXUS_AudioDecoderType_eDecode;
	openSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_e5_1;
#if defined(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT)	
	openSettings.cdbHeap = platformConfig.heap[NEXUS_VIDEO_SECURE_HEAP];
#endif
	stAudioInstance.audioDecoder[ulDevId] = NEXUS_AudioDecoder_Open(ulDevId, &openSettings);
	stAudioStatus[DI_AUDIO_DECODER_MAIN].nDecoderMode = DI_AUDIO_DECODER_MODE_DECODER;
	stAudioStatus[DI_AUDIO_DECODER_MAIN].bDecoderOpened = TRUE;

	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &decoderSettings);
	decoderSettings.sourceChanged.callback = P_AudioSourceChanged;
	decoderSettings.fifoUnderflow.callback = P_AudioFifoUnderFlow;
#if defined(PRINT_AUDIO_PTS_INFO)
	decoderSettings.firstPts.callback = P_AudioFirstPtsCallback;
	decoderSettings.ptsError.callback = P_AudioPtsErrorCallback;	
	decoderSettings.firstPts.context = stAudioInstance.audioDecoder[ulDevId];
	decoderSettings.firstPts.param = ulDevId;
	decoderSettings.ptsError.context = stAudioInstance.audioDecoder[ulDevId];
	decoderSettings.ptsError.param = ulDevId;
#endif
	decoderSettings.sourceChanged.context = stAudioInstance.audioDecoder[ulDevId];
	decoderSettings.sourceChanged.param = ulDevId;
#if defined(CONFIG_AUDIO_PTS_THRESHOLD_ADJUSTMENT)
	decoderSettings.gaThreshold = AUDIO_PTS_THRESHOLD;
#endif
	NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &decoderSettings);
#if 0 //primer temp
	/* Sub decoder  */
	NEXUS_AudioDecoder_GetDefaultOpenSettings(&openSettings);
	ulDevId = DI_AUDIO_DECODER_SUB;
	openSettings.type = NEXUS_AudioDecoderType_eAudioDescriptor;
	openSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_eNone;
	stAudioInstance.audioDecoder[ulDevId] = NEXUS_AudioDecoder_Open(ulDevId, &openSettings);
	stAudioStatus[DI_AUDIO_DECODER_MAIN].nDecoderMode = DI_AUDIO_DECODER_MODE_DESCRIPTION;
	stAudioStatus[DI_AUDIO_DECODER_MAIN].bDecoderOpened = TRUE;

	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &decoderSettings);
	decoderSettings.sourceChanged.callback = P_AudioSourceChanged;
	decoderSettings.fifoUnderflow.callback = P_AudioFifoUnderFlow;
	decoderSettings.sourceChanged.context = stAudioInstance.audioDecoder[ulDevId];
	decoderSettings.sourceChanged.param = ulDevId;
#if defined(CONFIG_AUDIO_PTS_THRESHOLD_ADJUSTMENT)
	decoderSettings.gaThreshold = AUDIO_PTS_THRESHOLD;
#endif
	NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &decoderSettings);
#endif

	/* SPDIF decoder */
	NEXUS_AudioDecoder_GetDefaultOpenSettings(&openSettings);
	ulDevId = DI_AUDIO_DECODER_SPDIF;
	openSettings.type = NEXUS_AudioDecoderType_eDecode;
	openSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_eNone;
#if defined(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT)	
	openSettings.cdbHeap = platformConfig.heap[NEXUS_VIDEO_SECURE_HEAP];
#endif							
	stAudioInstance.audioDecoder[ulDevId] = NEXUS_AudioDecoder_Open(ulDevId, &openSettings);
	stAudioStatus[DI_AUDIO_DECODER_SPDIF].nDecoderMode = DI_AUDIO_DECODER_MODE_PASSTHRU;
	stAudioStatus[DI_AUDIO_DECODER_SPDIF].bDecoderOpened = TRUE;

	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &decoderSettings);
	decoderSettings.sourceChanged.callback = P_AudioSourceChanged;
	decoderSettings.fifoUnderflow.callback = P_AudioFifoUnderFlow;
	decoderSettings.sourceChanged.context = stAudioInstance.audioDecoder[ulDevId];
	decoderSettings.sourceChanged.param = ulDevId;
#if defined(CONFIG_AUDIO_PTS_THRESHOLD_ADJUSTMENT)
	decoderSettings.gaThreshold = AUDIO_PTS_THRESHOLD;
#endif
	NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &decoderSettings);


/*************************
*	2. Codec settings
*************************/
	P_AUDIO_SetCodecSetting(DI_AUDIO_DECODER_MAIN);

/*************************
*	4. Event callback init.
*************************/
	for (i=0; i<AUDIO_ALL_DECODERS; i++)
	{
		for(j=0; j<AUDIO_EVENT_NUM; j++)
		{
			for(k=0; k<AUDIO_CALLBACK_OWNER_MAX; k++)
			{
				stAudioCallbacks[k].audioEventCallback[i][j] = NULL;
				stAudioCallbacks[k].bOccupied = FALSE;
			}
		}
	}

/*************************
*	5. Mixer Open.
*************************/
	/* Open HW Mixer */
	stAudioInstance.audioMixer = NEXUS_AudioMixer_Open(NULL);
	stAudioMixerStatus.bMixerOpened = TRUE;
	stAudioMixerStatus.nMixerType = DI_AUDIO_MIXER_TYPE_HW_MIXER;


/*************************
*	7. Main deocder connect to mixer.
*************************/
	dResult = P_AUDIO_AddMixerInputPath(DI_AUDIO_DECODER_MAIN);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_AddMixerInputPath failed!\n");
		return DRV_ERR;
	}

/*************************
*	6. Playback Init. & connection with mixer.
*************************/
	stAudioPlayback[0].bStart = FALSE;
	stAudioPlayback[1].bStart = FALSE;

	stAudioPlayback[0].gPlayback = NEXUS_AudioPlayback_Open(0, NULL);
	stAudioPlayback[1].gPlayback = NEXUS_AudioPlayback_Open(1, NULL);

	input = NEXUS_AudioPlayback_GetConnector(stAudioPlayback[0].gPlayback);
	NEXUS_AudioMixer_AddInput(stAudioInstance.audioMixer, input);

	input = NEXUS_AudioPlayback_GetConnector(stAudioPlayback[1].gPlayback);
	NEXUS_AudioMixer_AddInput(stAudioInstance.audioMixer, input);

/*************************
*	8. Encoder handle open.
*************************/
	stAudioInstance.ac3Encoder = NEXUS_Ac3Encode_Open(NULL);

/*************************
*	9. Status init.
*************************/
	for (i=0; i<AUDIO_ALL_DECODERS; i++)
	{
#if defined(CONFIG_DI10)
#else
		stAudioStatus[i].nType = DI_AUDIOTYPE_STREAM;
#endif
		stAudioStatus[i].nState = DI_AUDIO_STOP;
		stAudioStatus[i].nFormat = DI_AUDFORMAT_NONE;
	}
	stAudioOutput.bDownMixing = P_AUDIO_IsDownMixSupport(DI_AUDIO_DECODER_MAIN);
	stAudioOutput.bAnalogMute = FALSE;
	stAudioOutput.bSpdifMute = FALSE;
	stAudioOutput.bHdmiMute = FALSE;
	stAudioOutput.bVCRScartMute = FALSE;
	stAudioOutput.bForceDownmix = FALSE;
	stAudioOutput.bSpdifOutputPathConnected = FALSE;
	stAudioOutput.bHdmiOutputPathConnected = FALSE;
	stAudioOutput.bForceOutputPath = FALSE;

	stAudioInstance.dtsEncoder = NEXUS_DtsEncode_Open(NULL);

	stAudioSyncStatus.ulBasePtsOffset = PTSOFFSET;
	stAudioSyncStatus.ulMpegPtsOffset = MPEG_PTSOFFSET;
	stAudioSyncStatus.ulAc3PtsOffset = AC3_PTSOFFSET;
	stAudioSyncStatus.ulAc3plusPtsOffset = AC3P_PTSOFFSET;
	stAudioSyncStatus.ulAacPtsOffset = AAC_PTSOFFSET;
	stAudioSyncStatus.ulAacplusPtsOffset = AACP_PTSOFFSET;
	stAudioSyncStatus.nUserOffset = 0;

	stAudioTaskInfo[0].bOccupied = FALSE;
	stAudioTaskInfo[1].bOccupied = FALSE;

/*************************
*	10. Msg, Sem, Task init.
*************************/
	vResult = VK_SEM_Create(&s_ulAudioDISemId,"di_audio", DEFAULT_SUSPEND_TYPE);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_SEM_Create(&s_ulAudioDRVSemId,"di_audio", DEFAULT_SUSPEND_TYPE);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_SEM_Create(&s_ulAudioPSemId, "di_audio", DEFAULT_SUSPEND_TYPE);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_SEM_Create(&s_ulAudioMemStreamId,"di_mem_stream", DEFAULT_SUSPEND_TYPE);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	ulTaskId=0;
	vResult = VK_MSG_Create(DI_AUDIO_MSG_SIZE, sizeof(DI_AUDIO_STREAM_MSG), "AudStreamMsg", &stAudioTaskInfo[ulTaskId].s_ulAudioStreamMsgId, VK_SUSPENDTYPE_FIFO);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Create( P_AudioStreamTask,
								AUDIO_CLIP_TASK_PRIORITY,
								AUDIO_CLIP_TASK_STACK_SIZE,
								"AudStream",
								(void *)ulTaskId,
								&stAudioTaskInfo[ulTaskId].s_ulAudioStreamTaskId,
								FALSE);

	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Start(stAudioTaskInfo[ulTaskId].s_ulAudioStreamTaskId);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	ulTaskId = 1;
	vResult = VK_MSG_Create(DI_AUDIO_MSG_SIZE, sizeof(DI_AUDIO_STREAM_MSG), "AudStreamMsg", &stAudioTaskInfo[ulTaskId].s_ulAudioStreamMsgId, VK_SUSPENDTYPE_FIFO);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Create( P_AudioStreamTask,
								AUDIO_CLIP_TASK_PRIORITY,
								AUDIO_CLIP_TASK_STACK_SIZE,
								"AudStream",
								(void *)ulTaskId,
								&stAudioTaskInfo[ulTaskId].s_ulAudioStreamTaskId,
								FALSE);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Start(stAudioTaskInfo[ulTaskId].s_ulAudioStreamTaskId);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Create( P_AudioEventTask,
								AUDIO_MSG_TASK_PRIORITY,
								AUDIO_MSG_TASK_STACK_SIZE,
								"AudEvent",
								NULL,
								&s_ulAudioEventTaskId,
								FALSE);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vResult = VK_TASK_Start(s_ulAudioEventTaskId);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

/*************************
*	11. Volume Init.
*************************/
#if defined(CONFIG_JAPAN_SPEC)
	NEXUS_AudioDecoderCodecSettings Settings;

	NEXUS_AudioDecoder_GetCodecSettings(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], NEXUS_AudioCodec_eAac,&Settings);
	Settings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
	NEXUS_AudioDecoder_SetCodecSettings(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN],&Settings);
#endif

	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_DAC);
	NEXUS_AudioOutput_GetSettings(output, &nSettings);
	nSettings.volumeType = NEXUS_AudioVolumeType_eDecibel;
	nSettings.leftVolume = 0;
	nSettings.rightVolume = 0;
	nResult = NEXUS_AudioOutput_SetSettings(output, &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d NEXUS_AudioOutput_SetSettings failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	stAudioOutput.volumeType = nSettings.volumeType;
	stAudioOutput.ulAnalogVolume = AUDIO_LEVEL_MAX;
	stAudioOutput.ulSpdifVolume = AUDIO_LEVEL_MAX;
	stAudioOutput.ulHdmiVolume = AUDIO_LEVEL_MAX;
	gIsVolumeAdjusted = FALSE;

/*************************
*	12. Decoder delay Init.
*************************/
	dResult = P_AUDIO_DelayDecoder(DI_AUDIO_DECODER_MAIN, 0);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_DelayDecoder failed!\n");
		return DRV_ERR;
	}
	dResult = P_AUDIO_DelayDecoder(DI_AUDIO_DECODER_SPDIF, 0);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_DelayDecoder failed!\n");
		return DRV_ERR;
	}

/*************************
*	13. HWMUTE off
*************************/
	dResult = DRV_AUDIO_HWMUTE(FALSE);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_AUDIO_HWMUTE failed!\n");
		return DRV_ERR;
	}

/*************************
*	14. Set TSM mode
*************************/
	dResult = P_AUDIO_SetTsmMode(TRUE);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetTsmMode failed!\n");
		return DRV_ERR;
	}

#if defined(CONFIG_BLUETOOTH)
/*************************
*	15. Capture handle Open & Setting for Bluetooth Audio
*************************/
	NEXUS_AudioCapture_GetDefaultOpenSettings(&tCaptureOpenSettings);
	/* TODO: Please with the fifo size and threshold to fix BT auddio issues */
	tCaptureOpenSettings.fifoSize = 60*1024;
	tCaptureOpenSettings.threshold = 50;
	tCaptureOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_eStereo;
	stAudioInstance.audioCapture[0] = NEXUS_AudioCapture_Open(0, &tCaptureOpenSettings);
	if (stAudioInstance.audioCapture[0] == NULL)
	{
		PrintError("[%s] Unable to Open Capture Channel\n", __FUNCTION__);
		return DRV_ERR;
	}
#endif


#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
	dResult = DRV_AUDIO_HRA_Init();
	if(dResult != DRV_OK)
	{
		PrintError("DRV_AUDIO_HRA_Init failed!\n");
		return DRV_ERR;
	}
#if defined(TEMP_HRA_SELF_TEST)
	stHdAudioInstance.bStart = FALSE;
#endif
#endif


#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	for(i=0; i<AUDIO_ALL_DECODERS; i++)
	{
#if 1 //temp	bmem
		if(i==1)
			continue;
#endif
		dResult = P_AUDIO_OpenPrimers(i);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_OpenPrimers failed!\n");
			return DRV_ERR;
		}
	}
#endif

	return DRV_OK;
}

DRV_Error DRV_AUDIO_Start(HUINT32 ulDevId, DI_AUDIO_FORMAT nType)
{
	HUINT32 ulDelay;
	HINT32 vResult;
	HBOOL bAllStopped;
	DRV_Error dResult;
	NEXUS_Error nResult;
	NEXUS_AudioDecoderSettings nSettings;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode = DRV_DEMUX_PLAYBACK_UNKNOWN;
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	DI_MEDIA_AUDIO_TYPE_e eMediaAudioType;
	DI_MEDIA_AUDIO_FORMAT_e eMediaAudioCodec;
#endif
#if defined(CONFIG_ASTM)
	HBOOL astmStarted = FALSE;
	HBOOL bIsRfvodMode = FALSE;
#endif
	HBOOL bIsPlayback = FALSE;

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("HUINT32 ulDevId: %d DI_AUDIO_FORMAT nType: 0x%08X\n", ulDevId, nType);

	vResult = VK_SEM_Get(s_ulAudioDRVSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}


	if(ulDevId != DI_AUDIO_DECODER_SPDIF)
	{
		DI_AUDIO_DECODER_MODE decoderMode = DI_AUDIO_DECODER_MODE_DECODER;
		DI_AUDIO_MIXER_TYPE nMixerType = DI_AUDIO_MIXER_TYPE_HW_MIXER;
		HBOOL bConvertMixer = FALSE;
		HBOOL bChangeSubDecoderMode = FALSE;

		bConvertMixer = P_AUDIO_CheckMixerTypeConvert(ulDevId, nType, &nMixerType);
		if(bConvertMixer)
		{
			dResult = P_AUDIO_ConvertMixerType(nMixerType);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_ConvertMixerType be failed!\n");
				goto error_out;
			}

			/* Force sub decoder Setting */
			P_AUDIO_CheckSubDecoderModeChange(ulDevId, nType, &decoderMode);
			dResult = P_AUDIO_SetSecondaryDecoderMode(decoderMode, nMixerType);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_SetSecondaryDecoderMode be failed!\n");
				goto error_out;
			}
		}
		else
		{
			bChangeSubDecoderMode = P_AUDIO_CheckSubDecoderModeChange(ulDevId, nType, &decoderMode);
			if(bChangeSubDecoderMode)
			{
				dResult = P_AUDIO_SetSecondaryDecoderMode(decoderMode, nMixerType);
				if(dResult != DRV_OK)
				{
					PrintError("P_AUDIO_SetSecondaryDecoderMode be failed!\n");
					goto error_out;
				}
			}
		}
	}
#if 0
	if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		dResult = DRV_AUDIO_ConnectAudioDummyOutput(P_AUDIO_GetInput(DI_AUDIO_DECODER_MAIN, NEXUS_AudioDecoderConnectorType_eStereo));
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_ConnectAudioDummyOutput be failed!\n");
			goto error_out;
		}
	}
#endif
/*****************************
*	1. Parameter setting
******************************/
	NEXUS_AudioDecoder_GetDefaultStartSettings(&stAudioStatus[ulDevId].audioSettings);

	if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		stAudioStatus[ulDevId].audioSettings.secondaryDecoder = TRUE;
	}

#if defined(SYNC_SUB_AUDIO_FOR_DOLBY_CERTI)
	/* It synchronizes the SUB audio to the Main audio for DOLBY certification.
	    Dolby require that main and associated audio are synchronous within +- 0.5 ms
	    Do not use during normal operation.
	    see NEXUS_AudioDecoderStartSettings description */
	if( ulDevId == DI_AUDIO_DECODER_SUB || stAudioStatus[DI_AUDIO_DECODER_SUB].nState == DI_AUDIO_START )
	{
		stAudioStatus[ulDevId].audioSettings.forceCompleteFirstFrame = TRUE;
	}
#endif

#if defined(CONFIG_DI10)
	if(nType < DI_AUDFORMAT_MEDIA_MPEG)
#else
	if(stAudioStatus[ulDevId].nType != DI_AUDIOTYPE_MEDIA)
#endif
	{
		stAudioStatus[ulDevId].audioSettings.pidChannel = DRV_DEMUX_GetAudioPidChannel(ulDevId);
		stAudioStatus[ulDevId].audioSettings.codec = P_AUDIO_ConvertCodecToNexus(nType);
		stAudioStatus[ulDevId].audioSettings.stcChannel = NULL;
		if(stAudioSyncStatus.bTsm == TRUE)
		{
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
			if(stMultiTrackInfo[ulDevId].bStartPrimer == TRUE)
			{
				/* LIVE */
				stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(stMultiTrackInfo[ulDevId].ulSelectedIndex);
			}
			else
			{
				/* Playback */
				stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
			}
#elif defined(CONFIG_FAST_CHANNEL_CHANGE)
			HUINT32 uiVideoPrimerIndex;

			if(DRV_VIDEO_IsPrimerStarted())
			{
				/* LIVE with video primer only */
				dResult = DRV_VIDEO_GetPrimerIndex(0, &uiVideoPrimerIndex);
				if(dResult != DRV_OK)
				{
					PrintError("DRV_VIDEO_GetPrimerIndex be failed!\n");
					goto error_out;
				}
				stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(uiVideoPrimerIndex);
			}
			else
			{
				/* Playback */
				stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
			}
#else
			stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
#endif
		}
	}
	else
	{
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
		stAudioStatus[ulDevId].audioSettings.codec = P_AUDIO_ConvertCodecToNexus(nType);
		switch (ulDevId)
		{
			case DI_AUDIO_DECODER_MAIN:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_MAIN;
				break;

			case DI_AUDIO_DECODER_SUB:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_SUB;
				break;

			case DI_AUDIO_DECODER_SPDIF:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_SPDIF;
				break;

			default:
				eMediaAudioType = DI_MEDIA_AUDIO_DECODER_NONE;
				break;
		}

		if (eMediaAudioType == DI_MEDIA_AUDIO_DECODER_NONE)
		{
			PrintError("[%s] Invalide MediaAudioType \n", __FUNCTION__);
			goto error_out;
		}

		eMediaAudioCodec = P_AUDIO_ConvertCodecToMedia(nType);
		if(eMediaAudioCodec == DI_MEDIA_AUDIO_FORMAT_NONE)
		{
			PrintError("[%s] Invalide MediaAudioCodec \n",__FUNCTION__);
			goto error_out;
		}
#if defined(CONFIG_MEDIA)
		if(DRV_MEDIA_CheckStart())
		{
			stAudioStatus[ulDevId].audioSettings.pidChannel = DRV_MEDIA_GetPlaybackAudioPidChannel(
								eMediaAudioCodec,
								eMediaAudioType,
								(void *)stAudioInstance.audioDecoder[ulDevId]);

			stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_MEDIA_MP_GetPlaybackStcChannelHandle(
								DI_MEDIA_VIDEO_DECODER_NONE,
								eMediaAudioType,
								DI_MEDIA_PLAYPUMP_AUDIO_MAIN);
		}
#endif
#if defined(CONFIG_CALYPSO)
		if(DRV_CSO_CheckStart())
		{
			stAudioStatus[ulDevId].audioSettings.pidChannel = DRV_CSO_GetPlaypumpAudioPidChannel(
								eMediaAudioCodec,
								eMediaAudioType,
								(void *)stAudioInstance.audioDecoder[ulDevId]);

			stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_CSO_GetPlaypumpStcChannelHandle(
								DI_MEDIA_VIDEO_DECODER_NONE,
								eMediaAudioType,
								DI_MEDIA_PLAYPUMP_AUDIO_MAIN);
		}
#endif
#if defined(CONFIG_PUMP)
		if(DRV_PUMP_CheckAudioStart())
		{
			stAudioStatus[ulDevId].audioSettings.pidChannel = DRV_PUMP_GetPlaypumpAudioPidChannel(eMediaAudioType);
			stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_PUMP_GetPlaypumpAudioStcChannel();
		}
#endif

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO) || defined(CONFIG_AUDIO_DEC_FLAC)
		if(nType == DI_AUDFORMAT_FLAC ||
			nType == DI_AUDFORMAT_ADPCM ||
			nType == DI_AUDFORMAT_VORBIS ||
			nType == DI_AUDFORMAT_APE)
		{
			stAudioStatus[ulDevId].audioSettings.maxOutputRate = MAX_OUTPUT_RATE_HRA;
		}
		else
		{
			stAudioStatus[ulDevId].audioSettings.maxOutputRate = MAX_OUTPUT_RATE_NON_HRA;
		}
#endif

#endif
	}

/*****************************
*	2. SetSettings
******************************/

	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &nSettings);
	/*	NOTICE!!
	*	LFE channel data should not be transmitted to Stereo output.
	*	So, LFE mode must be turned on. It means that LFE data will be send out through its own channel.
	*/
#if 0
	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		if((nType == DI_AUDFORMAT_AC3_PLUS)||
			(nType == DI_AUDFORMAT_AAC_PLUS)||
			(nType == DI_AUDFORMAT_AAC_PLUS_ADTS)||
			(nType == DI_AUDFORMAT_AAC_PLUS_LOAS))
		{
			nSettings.outputLfeMode = NEXUS_AudioDecoderOutputLfeMode_eOn;
		}
	}
#endif

	/* external path playback인 경우 audio PID channel이 live parser와 연결되기 때문에 NEXUS에서 live mode로 동작함.
		따라서 liveOverride flag on하여 playback mode로 동작하도록 함. */
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	if(stMultiTrackInfo[ulDevId].bStartPrimer == TRUE)
	{
		HUINT32 ulIndex = stMultiTrackInfo[ulDevId].ulSelectedIndex;
		bIsPlayback = DRV_DEMUX_IsPlaybackPidChannel(stMultiTrackInfo[ulDevId].decoderSettings[ulIndex].pidChannel, &ePlaybackMode);
	}
	else
	{
		bIsPlayback = DRV_DEMUX_IsPlaybackPidChannel(stAudioStatus[ulDevId].audioSettings.pidChannel, &ePlaybackMode);
	}
#else
	bIsPlayback = DRV_DEMUX_IsPlaybackPidChannel(stAudioStatus[ulDevId].audioSettings.pidChannel, &ePlaybackMode);
#endif

	if(bIsPlayback&& ePlaybackMode == DRV_DEMUX_PLAYBACK_EXTERNAL)
	{
		nSettings.liveOverride = TRUE;
	}
	else
	{
		nSettings.liveOverride = FALSE;
	}

	nResult = NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_SetSettings failed!\n");
		goto error_out;
	}

/*****************************
*	NOTICE!!
*	The code below is only for MS10 dual decoding...
*	It is not needed for AudioDescription
*****************************/

/*****************************
*	MS10 Support
******************************/
#if 0
	if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		NEXUS_AudioInput input;
		NEXUS_AudioOutput output;

		if(stAudioMixerStatus.nMixerType == DI_AUDIO_MIXER_TYPE_DSP_MIXER &&
			stAudioStatus[DI_AUDIO_DECODER_SUB].nDecoderMode == DI_AUDIO_DECODER_MODE_DECODER)
		{
			if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_START)
			{
				NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[0]);
			}

#if defined(CONFIG_DI10)
			if(stAudioOutput.eSpdifMode != DI_AUDOUT_SPDIF_PCM)
#else
			if(stAudioOutput.eSpdifMode != DI_AUDOUT_SPDIF_DOWNMIX)
#endif
			{
				output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_SPDIF);
				NEXUS_AudioOutput_RemoveAllInputs(output);

				input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
				NEXUS_AudioOutput_AddInput(output, input);

#if defined(CONFIG_DI10)
				stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_PCM;
#else
				stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_DOWNMIX;
#endif
				stAudioOutput.bSpdifOutputPathConnected = TRUE;
				stAudioOutput.eSpdifInput = DI_AUDIO_INPUT_MIXER;
			}
			if(stAudioOutput.eHdmiMode != DI_AUDOUT_HDMI_DOWNMIX)
			{
				output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);
				NEXUS_AudioOutput_RemoveAllInputs(output);

				input = NEXUS_AudioMixer_GetConnector(stAudioInstance.audioMixer);
				NEXUS_AudioOutput_AddInput(output, input);

				stAudioOutput.eHdmiMode = DI_AUDOUT_HDMI_DOWNMIX;
				stAudioOutput.bHdmiOutputPathConnected = TRUE;
				stAudioOutput.eHdmiInput = DI_AUDIO_INPUT_MIXER;
			}

			if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_START)
			{
				NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[0], &stAudioStatus[0].audioSettings);
			}
		}
	}
#endif
/*****************************
*	4. Routing  output path
******************************/
	if(stAudioOutput.bDacOutputPathConnected == FALSE)
	{
		PrintDebug("P_AUDIO_AddDacOutputPath is FALSE !\n");
		dResult = P_AUDIO_AddDacOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddDacOutputPath failed!\n");
			goto error_out;
		}
	}

	if(stAudioOutput.bSpdifOutputPathConnected == FALSE)
	{
		PrintDebug("bSpdifOutputPathConnected is FALSE !\n");
		dResult = P_AUDIO_AddSpdifOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddSpdifOutputPath failed!\n");
			goto error_out;
		}
	}

	if(stAudioOutput.bHdmiOutputPathConnected == FALSE)
	{
		PrintDebug("bHdmiOutputPathConnected is FALSE !\n");
		dResult = P_AUDIO_AddHdmiOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddHdmiOutputPath failed!\n");
			goto error_out;
		}
	}

#if defined(CONFIG_BLUETOOTH)
	if(stAudioOutput.bBtCaptureOutputPathConnected == FALSE)
	{
		dResult = P_AUDIO_AddBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddBluetoothPath be failed!\n");
			goto error_out;
		}
	}
#endif

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
	if(stAudioStatus[ulDevId].nType == DI_AUDIOTYPE_MEDIA &&
		stAudioOutput.bI2sOutputPathConnected == FALSE &&
		stAudioOutput.bI2sPath)
	{
		PrintDebug("P_AUDIO_AddI2sOutputPath is FALSE !\n");
		dResult = P_AUDIO_AddI2sOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddI2sOutputPath failed!\n");
			goto error_out;
		}
	}
#endif

/*****************************
*	5. Sync Setting
******************************/

	ulDelay = P_AUDIO_GetDefaultDelay(nType);
	ulDelay += stAudioSyncStatus.nUserOffset;

	dResult = P_AUDIO_DelayDecoder(ulDevId, ulDelay);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_DelayDecoder failed!\n");
		goto error_out;
	}

#if defined(CONFIG_SYNC_CHANNEL)
#if defined(CONFIG_DI10)
	if(nType < DI_AUDFORMAT_MEDIA_MPEG)
#else
	if(stAudioStatus[ulDevId].nType < DI_AUDIOTYPE_MEDIA)
#endif
	{
		if(ulDevId == DI_AUDIO_DECODER_MAIN)
		{
			dResult = DRV_SYNC_SetSyncChannel(DRV_SYNC_AUDMAIN, stAudioStatus[ulDevId].audioConnectorType);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_SYNC_SetSyncChannel failed!\n");
				goto error_out;
			}
		}
		else if(ulDevId == DI_AUDIO_DECODER_SUB)
		{
			dResult = DRV_SYNC_SetSyncChannel( DRV_SYNC_AUDSUB, stAudioStatus[ulDevId].audioConnectorType);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_SYNC_SetSyncChannel failed!\n");
				goto error_out;
			}
		}
		else if(ulDevId == DI_AUDIO_DECODER_SPDIF)
		{
			dResult = DRV_SYNC_SetSyncChannel(DRV_SYNC_AUDSPDIF, stAudioStatus[ulDevId].audioConnectorType);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_SYNC_SetSyncChannel failed!\n");
				goto error_out;
			}
		}
	}
#endif

#if defined(CONFIG_SYNC_RECOVERY)
	DRV_SYNC_SetAudioTsmMode(stAudioSyncStatus.bTsm);
#endif

/*****************************
*	6. A/V sync
******************************/
#if defined(CONFIG_ASTM)
	DRV_RFVOD_GetRfvodMode(&bIsRfvodMode);
#if defined(CONFIG_DI10)
	if((nType < DI_AUDFORMAT_MEDIA_MPEG) && (bIsRfvodMode == FALSE))
#else
	if((stAudioStatus[ulDevId].nType < DI_AUDIOTYPE_MEDIA) && (bIsRfvodMode == FALSE))
#endif
	{
		if(ePlaybackMode == DRV_DEMUX_PLAYBACK_UNKNOWN)
		{
			astmStarted=DRV_SYNC_GetAstmStarted();
			if(astmStarted == TRUE)
				DRV_SYNC_AstmStop();

			if(ulDevId == DI_AUDIO_DECODER_MAIN)
				DRV_SYNC_SetAstmSettings(DRV_SYNC_AUDMAIN);

			if(ulDevId == DI_AUDIO_DECODER_SPDIF)
				DRV_SYNC_SetAstmSettings(DRV_SYNC_AUDSPDIF);
			DRV_SYNC_AstmStart();
		}
	}
	else
	{
		/* Request from IPM-SW1T swlee2  */
		DRV_SYNC_AstmStop();
	}
#endif

/*****************************
*	7. Decoder start
******************************/
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	if(stMultiTrackInfo[ulDevId].bStartPrimer == TRUE)
	{
		dResult = P_AUDIO_StopPrimerAndStartDecode(ulDevId, stMultiTrackInfo[ulDevId].ulSelectedIndex, nType);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] P_AUDIO_StopPrimerAndStartDecode failed! ulDevId=%d\n", __FUNCTION__, ulDevId);
			goto error_out;
		}
	}
	else
	{
		nResult = NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[ulDevId], &stAudioStatus[ulDevId].audioSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_Start failed!\n");
			goto error_out;
		}
	}
#else
	nResult = NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[ulDevId], &stAudioStatus[ulDevId].audioSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_Start failed!\n");
		goto error_out;
	}
#endif


/*****************************
*	8. Status update
******************************/
	stAudioStatus[ulDevId].nFormat = nType;
	stAudioStatus[ulDevId].nState = DI_AUDIO_START;

/*****************************
*	9. Control volume & A/V sync
******************************/
#if defined(CONFIG_SCART_6417)
	dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_DAC, AUDIO_LEVEL_MAX);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetVolume : DI_AUDIO_OUTPUT_DAC failed!\n");
		goto error_out;
	}
#else
	dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_DAC, stAudioOutput.ulAnalogVolume);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetVolume : DI_AUDIO_OUTPUT_DAC failed!\n");
		goto error_out;
	}
#endif
	dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_SPDIF, stAudioOutput.ulSpdifVolume);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetVolume : DI_AUDIO_OUTPUT_SPDIF failed!\n");
		goto error_out;
	}
	dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_HDMI, stAudioOutput.ulHdmiVolume);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetVolume : DI_AUDIO_OUTPUT_HDMI failed!\n");
		goto error_out;
	}

/***************************************************************************
*	10. Set pidChannel for trick play of MEDIA & Recoder playback.
****************************************************************************/
	if(ePlaybackMode != DRV_DEMUX_PLAYBACK_EXTERNAL)
	{
		dResult = P_AUDIO_SetPidChannelForTrickPlay(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetPidChannelForTrickPlay failed!\n");
			goto error_out;
		}
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	return DRV_OK;

error_out:
	bAllStopped = P_AUDIO_CheckAllStopped();
	if(bAllStopped == TRUE)
	{
		dResult = P_AUDIO_RemoveDacOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDacOutputPath failed!\n");
		}
		dResult = P_AUDIO_RemoveSpdifOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
		}
		dResult = P_AUDIO_RemoveHdmiOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
		}
#if defined(CONFIG_BLUETOOTH)
		dResult = P_AUDIO_RemoveBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
		}
#endif

	}

#if defined(CONFIG_ASTM)
	/* Request from IPM-SW1T swlee2  */
	DRV_SYNC_AstmStop();
#endif

	VK_SEM_Release(s_ulAudioDRVSemId);

	return DRV_ERR;
}

DRV_Error DRV_AUDIO_Stop (HUINT32 ulDevId)
{//Sunwoo
	HINT32 vResult;
	HBOOL bAllStopped;
	DRV_Error dResult;
	DI_AUDIO_FORMAT nType;
#if defined(CONFIG_ASTM)
	HBOOL	bIsVideoValid = FALSE;
	HBOOL bIsRfvodMode = FALSE;
#endif

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("HUINT32 ulDevId: %d\n", ulDevId);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDRVSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	if(( stAudioStatus[ulDevId].nState == DI_AUDIO_STOP ) || ( stAudioStatus[ulDevId].nState == DI_AUDIO_FREE ))
	{
		VK_SEM_Release(s_ulAudioDRVSemId);
		return DRV_OK;
	}

	if (!stAudioInstance.audioDecoder[ulDevId])
	{
		PrintError("decoder[%d] handle is NULL!\n", ulDevId);
		VK_SEM_Release(s_ulAudioDRVSemId);
		return DRV_ERR;
	}
	/*****************************
	*	1. Decoder Stop
	******************************/
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	if(stMultiTrackInfo[ulDevId].bStartPrimer == TRUE)
	{
		dResult = P_AUDIO_StopDecodeAndStartPrimer(ulDevId, stMultiTrackInfo[ulDevId].ulSelectedIndex);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] P_AUDIO_StopPrimerAndStartDecode failed! ulDevId=%d\n", __FUNCTION__, ulDevId);
		}
	}
	else
	{
		NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[ulDevId]);
	}
#else
	NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[ulDevId]);
#endif

#if defined(CONFIG_SYNC_CHANNEL)
	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		dResult = DRV_SYNC_UnsetSyncChannel(DRV_SYNC_AUDMAIN);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SYNC_UnsetSyncChannel failed!\n");
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		dResult = DRV_SYNC_UnsetSyncChannel(DRV_SYNC_AUDSUB);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SYNC_UnsetSyncChannel failed!\n");
		}
	}
	else if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		dResult = DRV_SYNC_UnsetSyncChannel(DRV_SYNC_AUDSPDIF);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SYNC_UnsetSyncChannel failed!\n");
		}
	}
#endif

#if defined(CONFIG_ASTM)
	DRV_RFVOD_GetRfvodMode(&bIsRfvodMode);
#if defined(CONFIG_DI10)
	if((stAudioStatus[ulDevId].nFormat < DI_AUDFORMAT_MEDIA_MPEG)
		&& (bIsRfvodMode == FALSE))
#else
	if((stAudioStatus[ulDevId].nType < DI_AUDIOTYPE_MEDIA)
		&& (bIsRfvodMode == FALSE))
#endif
	{
		DRV_SYNC_AstmStop();

		if(ulDevId == DI_AUDIO_DECODER_MAIN)
			DRV_SYNC_UnsetAstmSettings(DRV_SYNC_AUDMAIN);

		if(ulDevId == DI_AUDIO_DECODER_SPDIF)
			DRV_SYNC_UnsetAstmSettings(DRV_SYNC_AUDSPDIF);

		/* If only audio decoder is stopped but still working video decoder, need to re-start ASTM module. */
		bIsVideoValid = DRV_SYNC_GetAstmVideoValid();
		if(bIsVideoValid == TRUE)
		{
			PrintDebug("Astm restart even if stop in audio\n");
			DRV_SYNC_AstmStart();
		}
		else
		{
			PrintDebug("Astm normally is stopped in audio\n");
		}
	}
	else
	{
	    /* Request from IPM-SW1T swlee2  */
		DRV_SYNC_AstmStop();
	}
#endif

	nType = stAudioStatus[ulDevId].nFormat;
	stAudioStatus[ulDevId].nFormat = DI_AUDFORMAT_NONE;
	stAudioStatus[ulDevId].nState = DI_AUDIO_STOP;
	stAudioStatus[ulDevId].rState.state = DI_AUDIO_STATE_UNKNOWN;

	dResult = P_AUDIO_SetTsmMode(TRUE);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetTsmMode failed!\n");
		VK_SEM_Release(s_ulAudioDRVSemId);
		return DRV_ERR;
	}

/*****************************
*	2. Path removing
******************************/
	bAllStopped = P_AUDIO_CheckAllStopped();
	if( bAllStopped == TRUE
#if defined(CONFIG_MIC)
	&& stAudioOutput.bForceOutputPath == FALSE
#endif
	)
	{
		dResult = P_AUDIO_RemoveDacOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDacOutputPath failed!\n");
			VK_SEM_Release(s_ulAudioDRVSemId);
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveSpdifOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			VK_SEM_Release(s_ulAudioDRVSemId);
			return DRV_ERR;
		}

		dResult = P_AUDIO_RemoveHdmiOutputPath(ulDevId, nType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
			VK_SEM_Release(s_ulAudioDRVSemId);
			return DRV_ERR;
		}
#if defined(CONFIG_BLUETOOTH)
		dResult = P_AUDIO_RemoveBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
			return DRV_ERR;
		}
#endif

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
		dResult = P_AUDIO_RemoveI2sOutputPath(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveI2sOutputPath be failed!\n");
			return DRV_ERR;
		}
#endif
	}

#if 0
	if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		dResult = DRV_AUDIO_DisconnectAudioDummyOutput(P_AUDIO_GetInput(DI_AUDIO_DECODER_MAIN, NEXUS_AudioDecoderConnectorType_eStereo));
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_DisconnectAudioDummyOutput is failed!\n");
			return DRV_ERR;
		}
	}
#endif
	VK_SEM_Release(s_ulAudioDRVSemId);
	return DRV_OK;
}


DRV_Error DRV_AUDIO_ChangeSyncMode(HUINT32 ulDevId, HBOOL bTsm)
{//Sunwoo
	DRV_Error dResult;
	HINT32 vResult;

	vResult = VK_SEM_Get(s_ulAudioDRVSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	if(ulDevId == DI_AUDIO_DECODER_SUB)
	{
		VK_SEM_Release(s_ulAudioDRVSemId);
		return DRV_OK;
	}

	/*	1. Changing the Tsm mode
	*	TRUE bTsm indicates that the STC should be adapted by PCR.
	*/
	if(bTsm == TRUE)
	{
		stAudioStatus[ulDevId].audioSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
	}
	else
	{
		stAudioStatus[ulDevId].audioSettings.stcChannel = NULL;
	}

	dResult = P_AUDIO_SetTsmMode(bTsm);
	if(dResult != DRV_OK)
	{
		VK_SEM_Release(s_ulAudioDRVSemId);
		return DRV_ERR;
	}

	dResult = P_AUDIO_RestartDecoder(ulDevId);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_RestartDecoder faild!\n");
		VK_SEM_Release(s_ulAudioDRVSemId);

		return DRV_ERR;
	}
	VK_SEM_Release(s_ulAudioDRVSemId);

	return DRV_OK;
}

DRV_Error DRV_AUDIO_PrintInfo(void)
{//Sunwoo
	HINT32 vResult;
	HUINT32 i;
	DRV_Error drvResult;
	NEXUS_AudioDecoderStatus currentStatus[3];
	NEXUS_AudioDecoderSettings nSettings;
	NEXUS_Error nResult;
	char* contents;

	vResult = VK_SEM_Get(s_ulAudioDRVSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	/* Local Status
	*/
	contents = "Decoder";
	DI_UART_Print("==============================[%s]============================\n\n",contents);
	for(i=0; i<= DI_AUDIO_DECODER_SPDIF; i++)
	{
		if(stAudioStatus[i].nState == DI_AUDIO_START)
		{
			contents = "DI_AUDIO_START";
		}
		else if(stAudioStatus[i].nState == DI_AUDIO_PAUSED)
		{
			contents = "DI_AUDIO_PAUSED";
		}
		else if(stAudioStatus[i].nState == DI_AUDIO_STOP)
		{
			contents = "DI_AUDIO_STOP";
		}
		DI_UART_Print("Decoder[%d]: %s: ",i ,contents);

		if(stAudioStatus[i].nState == DI_AUDIO_START)
		{
			if(stAudioStatus[i].bDecoding == TRUE)
			{
				contents = "Decoding";
			}
			else if(stAudioStatus[i].bDecoding == FALSE)
			{
				contents = "Not Decoding";
			}
		}
		else if(stAudioStatus[i].nState == DI_AUDIO_STOP)
		{
			contents = "Unknown";
		}
		DI_UART_Print("%s\n", contents);
	}

	contents = "MIXER";
	DI_UART_Print("==============================[%s]============================\n\n",contents);
	if(stAudioMixerStatus.nMixerType == DI_AUDIO_MIXER_TYPE_HW_MIXER)
	{
		DI_UART_Print("MIXTER TYPE : DI_AUDIO_MIXER_TYPE_HW_MIXER \n");
	}
	else if(stAudioMixerStatus.nMixerType == DI_AUDIO_MIXER_TYPE_DSP_MIXER)
	{
		DI_UART_Print("MIXTER TYPE : DI_AUDIO_MIXER_TYPE_DSP_MIXER \n");
	}
	else
	{
		DI_UART_Print("MIXTER TYPE : DI_AUDIO_MIXER_TYPE_NONE \n");
	}

	contents = "Codec";
	DI_UART_Print("\n==============================[%s]==============================\n\n",contents);
	for(i=0; i<=DI_AUDIO_DECODER_SPDIF; i++)
	{
		DI_AUDIO_FORMAT curFormat=DI_AUDFORMAT_NONE;
		curFormat = stAudioStatus[i].nFormat;

#if defined(CONFIG_DI10)
		if(curFormat >= DI_AUDFORMAT_MEDIA_MPEG)
		{
			curFormat &= 0x0fffffff;
		}
#endif

		if(curFormat == DI_AUDFORMAT_MPEG)
		{
			contents = "DI_AUDFORMAT_MPEG";
		}
		else if(curFormat == DI_AUDFORMAT_MP3)
		{
			contents = "DI_AUDFORMAT_MP3";
		}
		else if((curFormat == DI_AUDFORMAT_AAC) || (curFormat == DI_AUDFORMAT_AAC_ADTS) || (curFormat == DI_AUDFORMAT_AAC_LOAS))
		{
			contents = "DI_AUDFORMAT_AAC";
		}
		else if((curFormat == DI_AUDFORMAT_AAC_PLUS) || (curFormat == DI_AUDFORMAT_AAC_PLUS_LOAS) || (curFormat == DI_AUDFORMAT_AAC_PLUS_ADTS))
		{
			contents = "DI_AUDFORMAT_AAC_PLUS";
		}
		else if(curFormat == DI_AUDFORMAT_AC3)
		{
			contents = "DI_AUDFORMAT_AC3";
		}
		else if(curFormat == DI_AUDFORMAT_AC3_PLUS)
		{
			contents = "DI_AUDFORMAT_AC3_PLUS";
		}
		else if(curFormat == DI_AUDFORMAT_WMA_STD)
		{
			contents = "DI_AUDFORMAT_WMA_STD";
		}
		else if(curFormat == DI_AUDFORMAT_FLAC)
		{
			contents = "DI_AUDFORMAT_FLAC";
		}
		else if(curFormat == DI_AUDFORMAT_NONE)
		{
			contents = "DI_AUDFORMAT_NONE";
		}
		else
		{
			contents = "Unknown";
		}
		DI_UART_Print("Decoder[%d]: %s\n",i ,contents);
	}

	contents = "Volume";
	DI_UART_Print("\n==============================[%s]=============================\n\n",contents);
	DI_UART_Print("Analog \tVolume: %d Mute:[%s]\n", stAudioOutput.ulAnalogVolume,  stAudioOutput.bAnalogMute?"on":"off");
	DI_UART_Print("Spdif \tVolume: %d Mute:[%s]\n", stAudioOutput.ulSpdifVolume, stAudioOutput.bSpdifMute?"on":"off");
	DI_UART_Print("Hdmi \tVolume: %d Mute:[%s]\n", stAudioOutput.ulHdmiVolume, stAudioOutput.bHdmiMute?"on":"off");
	DI_UART_Print("I2S \tVolume: %d Mute:[%s]\n", stAudioOutput.ulI2sVolume, stAudioOutput.bI2sMute?"on":"off");
{
	NEXUS_AudioOutput output;
	NEXUS_AudioOutputSettings nSettings;

	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_DAC);
	NEXUS_AudioOutput_GetSettings(output, &nSettings);
	DI_UART_Print("DAC setting: Mute %d \n", nSettings.muted);

	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_SPDIF);
	NEXUS_AudioOutput_GetSettings(output, &nSettings);
	DI_UART_Print("SPDIF setting: Mute %d \n", nSettings.muted);

	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_HDMI);
	NEXUS_AudioOutput_GetSettings(output, &nSettings);
	DI_UART_Print("HDMI setting: Mute %d \n", nSettings.muted);

	output = P_AUDIO_GetOutput(DI_AUDIO_OUTPUT_I2S);
	NEXUS_AudioOutput_GetSettings(output, &nSettings);
	DI_UART_Print("I2S setting: Mute %d \n", nSettings.muted);

}
	contents = "Digital Output";
	DI_UART_Print("\n==============================[%s]=====================\n\n",contents);
#if defined(CONFIG_DI10)
	if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_COMPRESSED)
	{
		contents = "Bypassing";
	}
	else if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_PCM)
	{
		contents = "Decoded PCM";
	}

#else
	if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_DOWNMIX)
	{
		contents = "Decoded PCM";
	}
	else if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_BYPASS)
	{
		contents = "Bypassing";
	}
	else if(stAudioOutput.eSpdifMode == DI_AUDOUT_SPDIF_TRANSCODING)
	{
		contents = "Transcoding";
	}
#endif
	DI_UART_Print("SPDIF output dout mode is %s\n",contents);

	if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_BYPASS)
	{
		contents = "Bypassing";
	}
	else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_DOWNMIX)
	{
		contents = "Decoded PCM";
	}
	else if(stAudioOutput.eHdmiMode == DI_AUDOUT_HDMI_TRANSCODING)
	{
		contents = "Transcoding";
	}
	DI_UART_Print("HDMI output dout mode is %s\n",contents);

	contents = "Output Type";
	DI_UART_Print("\n==============================[%s]========================\n\n",contents);
	for(i=0; i<=DI_AUDIO_DECODER_SPDIF; i++)
	{
		if(stAudioStatus[i].nState != DI_AUDIO_START)
		{
			DI_UART_Print("Decoder[%d]: Not working!\n", i);
		}
		else
		{
			if(stAudioStatus[i].audioConnectorType == NEXUS_AudioDecoderConnectorType_eStereo)
			{
				DI_UART_Print("Decoder[%d]: NEXUS_AudioDecoderConnectorType_eStereo\n",i);
			}
			else if(stAudioStatus[i].audioConnectorType == NEXUS_AudioDecoderConnectorType_eMultichannel)
			{
				DI_UART_Print("Decoder[%d]: NEXUS_AudioDecoderConnectorType_eMultichannel\n",i);
			}
			else if(stAudioStatus[i].audioConnectorType == NEXUS_AudioDecoderConnectorType_eCompressed)
			{
				DI_UART_Print("Decoder[%d]: NEXUS_AudioDecoderConnectorType_eCompressed\n",i);
			}
		}
	}

	if(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN] != NULL)
	{
		NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[DI_AUDIO_DECODER_MAIN], &nSettings);
		if(nSettings.outputMode == 0)
		{
			DI_UART_Print("\nPCM Decoder output mode: Auto\n");
		}
		else
		{
			DI_UART_Print("PCM Decoder output mode: %d\n", nSettings.outputMode);
		}
	}

	contents = "DAC connection";
	DI_UART_Print("\n==============================[%s]========================\n\n",contents);
	DI_UART_Print("DAC output path connection : %d\n", stAudioOutput.bDacOutputPathConnected);

	switch(stAudioOutput.eDacInput)
	{
		case DI_AUDIO_INPUT_NONE:
			DI_UART_Print("DAC input : NONE\n");
			break;
		case DI_AUDIO_INPUT_MIXER:
			DI_UART_Print("DAC input : MIXER\n");
			break;
		case DI_AUDIO_INPUT_DECODER:
			DI_UART_Print("DAC input : DECODER\n");
			break;
		case DI_AUDIO_INPUT_ENCODER:
			DI_UART_Print("DAC input : ENCODER\n");
			break;
		default:
			DI_UART_Print("DAC input : what is it ? \n");
			break;
	}

	contents = "HDMI connection";
	DI_UART_Print("\n==============================[%s]========================\n\n",contents);
	DI_UART_Print("HDMI output path connection : %d\n", stAudioOutput.bHdmiOutputPathConnected);

	switch(stAudioOutput.eHdmiInput)
	{
		case DI_AUDIO_INPUT_NONE:
			DI_UART_Print("HDMI input : NONE\n");
			break;
		case DI_AUDIO_INPUT_MIXER:
			DI_UART_Print("HDMI input : MIXER\n");
			break;
		case DI_AUDIO_INPUT_DECODER:
			DI_UART_Print("HDMI input : DECODER\n");
			break;
		case DI_AUDIO_INPUT_ENCODER:
			DI_UART_Print("HDMI input : ENCODER\n");
			break;
		default:
			DI_UART_Print("HDMI input : what is it ? \n");
			break;
	}

	contents = "SPDIF connection";
	DI_UART_Print("\n==============================[%s]========================\n\n",contents);
	DI_UART_Print("SPDIF output path connection : %d\n", stAudioOutput.bHdmiOutputPathConnected);

	switch(stAudioOutput.eSpdifInput)
	{
		case DI_AUDIO_INPUT_NONE:
			DI_UART_Print("SPDIF input : NONE\n");
			break;
		case DI_AUDIO_INPUT_MIXER:
			DI_UART_Print("SPDIF input : MIXER\n");
			break;
		case DI_AUDIO_INPUT_DECODER:
			DI_UART_Print("SPDIF input : DECODER\n");
			break;
		case DI_AUDIO_INPUT_ENCODER:
			DI_UART_Print("SPDIF input : ENCODER\n");
			break;
		default:
			DI_UART_Print("SPDIF input : what is it ? \n");
			break;
	}

	contents = "I2S connection";
	DI_UART_Print("\n==============================[%s]========================\n\n",contents);
	DI_UART_Print("I2S output path connection : %d\n", stAudioOutput.bI2sOutputPathConnected);

	switch(stAudioOutput.eI2sInput)
	{
		case DI_AUDIO_INPUT_NONE:
			DI_UART_Print("I2S input : NONE\n");
			break;
		case DI_AUDIO_INPUT_MIXER:
			DI_UART_Print("I2S input : MIXER\n");
			break;
		case DI_AUDIO_INPUT_DECODER:
			DI_UART_Print("I2S input : DECODER\n");
			break;
		case DI_AUDIO_INPUT_ENCODER:
			DI_UART_Print("I2S input : ENCODER\n");
			break;
		default:
			DI_UART_Print("I2S input : what is it ? \n");
			break;
	}

	contents = "Bluetooth Capture connection";
	DI_UART_Print("\n==============================[%s]========================\n\n",contents);
	DI_UART_Print("Capture output path connection : %d\n", stAudioOutput.bBtCaptureOutputPathConnected);

	/* Nexus Status
	*/
	for(i=0; i<=DI_AUDIO_DECODER_SPDIF;i++)
	{
		if(stAudioInstance.audioDecoder[i] != NULL)
		{
			nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[i], &currentStatus[i]);
			if(nResult != NEXUS_SUCCESS)
			{
				DI_UART_Print("[%s] %d NEXUS_AudioDecoder_GetStatus failed [%d] \n", __FUNCTION__, __LINE__, i);
				VK_SEM_Release(s_ulAudioDRVSemId);
				return DRV_ERR;
			}
		}
	}

	contents = "Sync";
	DI_UART_Print("\n==============================[%s]===============================\n\n",contents);
	DI_UART_Print("Sync Mode: %s by DI\n\n", stAudioSyncStatus.bTsm?"bTsm":"Vsync");

	for(i=0; i<=DI_AUDIO_DECODER_SPDIF;i++)
	{
		if(stAudioInstance.audioDecoder[i] != NULL)
		{
			DI_UART_Print("Decoder[%d]: TSM : %s by nexus\n", i, currentStatus[i].tsm ? "On":"Off");
			DI_UART_Print("Decoder[%d]: Pts: %d  PtsErrorCount: %d  PtsStcDiff: %d\n\n",
				i, currentStatus[i].pts, currentStatus[i].ptsErrorCount, currentStatus[i].ptsStcDifference);
		}
	}
	DI_UART_Print("\nBasePtsOffset: %d\n",stAudioSyncStatus.ulBasePtsOffset);
	DI_UART_Print("MpegPtsOffset: %d\n",stAudioSyncStatus.ulMpegPtsOffset);
	DI_UART_Print("Ac3PtsOffset: %d\n",stAudioSyncStatus.ulAc3PtsOffset);
	DI_UART_Print("Ac3+PtsOffset: %d\n",stAudioSyncStatus.ulAc3plusPtsOffset);
	DI_UART_Print("AacPtsOffset: %d\n",stAudioSyncStatus.ulAacPtsOffset);
	DI_UART_Print("Aac+PtsOffset: %d\n",stAudioSyncStatus.ulAacplusPtsOffset);
	DI_UART_Print("User Offset: %d\n",stAudioSyncStatus.nUserOffset);


	contents = "Data Info";
	DI_UART_Print("\n==============================[%s]==========================\n\n",contents);
	for(i=0; i<=DI_AUDIO_DECODER_SPDIF;i++)
	{
		if(stAudioInstance.audioDecoder[i] != NULL)
		{
			DI_UART_Print("Decoder[%d] FIFO DEPTH / FIFO SIZE (CDB level): %d / %d\n",i, currentStatus[i].fifoDepth, currentStatus[i].fifoSize);
			DI_UART_Print("Decoder[%d] Sample rate: %d Hz\n",i, currentStatus[i].sampleRate);
			DI_UART_Print("Decoder[%d] Queued Frame amount: %d\n", i, currentStatus[i].queuedFrames);
			DI_UART_Print("Decoder[%d] framesDecoded: %d\n", i, currentStatus[i].framesDecoded);
			DI_UART_Print("Decoder[%d] frameErrors: %d\n", i, currentStatus[i].frameErrors);
			DI_UART_Print("Decoder[%d] numFifoOverflows: %d\n", i, currentStatus[i].numFifoOverflows);
			DI_UART_Print("Decoder[%d] numFifoUnderflows: %d\n", i, currentStatus[i].numFifoUnderflows);
			DI_UART_Print("Decoder[%d] numBytesDecoded: %d\n\n", i, currentStatus[i].numBytesDecoded);
		}
	}

	contents = "Audio mode Info";
	DI_UART_Print("\n==============================[%s]==========================\n\n",contents);

	for(i=0; i<=DI_AUDIO_DECODER_SPDIF;i++)
	{
		DI_AUDIO_STREAM_INFO_t tAudioStreamInfo;

		drvResult = DRV_AUDIO_GetStreamInfo(i,&tAudioStreamInfo);
		if(drvResult == DRV_OK)
		{
			if(tAudioStreamInfo.eMode == DI_AUDIO_MODE_UNKNOWN)
			{
				DI_UART_Print("Decoder[%d] eMode is DI_AUDIO_MODE_UNKNOWN\n",i);
			}
			else if(tAudioStreamInfo.eMode == DI_AUDIO_MODE_STEREO)
			{
				DI_UART_Print("Decoder[%d] eMode is DI_AUDIO_MODE_STEREO\n",i);
			}
			else if(tAudioStreamInfo.eMode == DI_AUDIO_MODE_MUTI_STEREO)
			{
				DI_UART_Print("Decoder[%d] eMode is DI_AUDIO_MODE_MUTI_STEREO\n",i);
			}
			else if(tAudioStreamInfo.eMode == DI_AUDIO_MODE_MONO)
			{
				DI_UART_Print("Decoder[%d] eMode is DI_AUDIO_MODE_MONO\n",i);
			}
			else if(tAudioStreamInfo.eMode == DI_AUDIO_MODE_DUAL_MONO)
			{
				DI_UART_Print("Decoder[%d] eMode s DI_AUDIO_MODE_DUAL_MONO\n",i);
			}
			else
			{
				DI_UART_Print("Decoder[%d] eMode is INVALID\n",i);
			}

			if(stAudioInstance.audioDecoder[i] != NULL)
			{
				NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[i], &nSettings);
				if(nSettings.dualMonoMode == NEXUS_AudioDecoderDualMonoMode_eStereo)
				{
					DI_UART_Print("Decoder[%d] DualMonoMode  is NEXUS_AudioDecoderDualMonoMode_eStereo\n",i);
				}
				else if(nSettings.dualMonoMode == NEXUS_AudioDecoderDualMonoMode_eLeft)
				{
					DI_UART_Print("Decoder[%d] DualMonoMode  is NEXUS_AudioDecoderDualMonoMode_eLeft\n",i);
				}
				else if(nSettings.dualMonoMode == NEXUS_AudioDecoderDualMonoMode_eRight)
				{
					DI_UART_Print("Decoder[%d] DualMonoMode  is NEXUS_AudioDecoderDualMonoMode_eRight\n",i);
				}
				else if(nSettings.dualMonoMode == NEXUS_AudioDecoderDualMonoMode_eMix)
				{
					DI_UART_Print("Decoder[%d] DualMonoMode  is NEXUS_AudioDecoderDualMonoMode_eMix\n",i);
				}
			}

		}
		else if(stAudioStatus[i].rState.state != DI_AUDIO_STATE_DECODING)
		{
			DI_UART_Print("Decoder[%d] is not decoding \n", i);
		}
		else
		{
			DI_UART_Print("Decoder[%d] DRV_AUDIO_GetStreamInfo is faied \n", i);
		}

		DI_UART_Print("\n");
	}

	contents = "End of audio info";
	DI_UART_Print("\n==============================[%s]==========================\n\n",contents);

	VK_SEM_Release(s_ulAudioDRVSemId);
	return DRV_OK;
}

void DRV_AUDIO_GetDecoderHandle(HUINT32 ulDevId, NEXUS_AudioDecoderHandle 	*audioDecoder)
{//Sunwoo
	*audioDecoder = stAudioInstance.audioDecoder[ulDevId];
}

#if defined(CONFIG_BLUETOOTH)
void DRV_AUDIO_GetCaptureHandle(HUINT32 ulDevId, NEXUS_AudioCaptureHandle *audioCapture)
{//hkryu
	*audioCapture= stAudioInstance.audioCapture[ulDevId];
}
#endif

DRV_Error DRV_AUDIO_BeepPlay(bool onoff)
{
	DI_AUDIO_STREAM_MSG 				audMsg;
	HINT32 								vResult;
	NEXUS_AudioPlaybackStartSettings 	playbackSettings;
	NEXUS_Error 						nResult;
	HUINT32								ulTaskIndex=0;
	DRV_Error							dResult;
	HUINT32								ulPoolIndex=0;
	HBOOL								bAllStopped;
/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("bool onoff: %s\n", onoff?"TRUE":"FALSE");

/*///////////////////////////////////*/
	VK_MEM_Memset(&audMsg,0x00,sizeof(DI_AUDIO_STREAM_MSG));

	if(onoff == FALSE)
	{
		if((stMemoryStreamPool[0].bOccupied == TRUE) && (stMemoryStreamPool[0].ulRequestId == 99))
		{
			ulTaskIndex = stMemoryStreamPool[0].ulTaskIndex;
		}
		else if((stMemoryStreamPool[1].bOccupied == TRUE) && (stMemoryStreamPool[1].ulRequestId == 99))
		{
			ulTaskIndex = stMemoryStreamPool[1].ulTaskIndex;
		}
		else
		{
			PrintError("Beep is already off\n");
			return DRV_OK;
		}

		vResult = VK_SEM_Get(s_ulAudioMemStreamId);
		if(vResult != VK_OK)
		{
			PrintError("VK_SEM_Get failed!\n");
			return DRV_ERR;
		}

		dResult = P_AUDIO_PCMStreamplay_Stop(99, ulTaskIndex);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_PCMStreamplay_Stop failed!\n");
			VK_SEM_Release(s_ulAudioMemStreamId);
			return DRV_ERR;
		}
		VK_SEM_Release(s_ulAudioMemStreamId);

		return DRV_OK;
	}
	else
	{
		ulTaskIndex = P_AUDIO_FindFreeTaskId();
		if(ulTaskIndex == DI_AUDIO_TASK_BUSY)
		{
			PrintError("There's no free task!\n");
			return DI_ERR_ERROR;
		}
		stAudioTaskInfo[ulTaskIndex].bOccupied = TRUE;

		ulPoolIndex = P_AUDIO_FindFreeMemStreamId();
		if(ulPoolIndex == DI_AUDIO_MEMPOOL_BUSY)
		{
			PrintError("There's no free Memory pool!\n");
			stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;
			return DI_ERR_ERROR;
		}

		stMemoryStreamPool[ulPoolIndex].ulRequestId = 99;
		stMemoryStreamPool[ulPoolIndex].bOccupied = TRUE;
		stMemoryStreamPool[ulPoolIndex].ulTaskIndex = ulTaskIndex;

		if(gTimeTicket > 99)
		{
			gTimeTicket = 0;
		}
		stMemoryStreamPool[ulPoolIndex].ulTimeTicket = ++gTimeTicket;

		if(stAudioOutput.bDacOutputPathConnected == FALSE)
		{
			dResult = P_AUDIO_AddDacOutputPath(DI_AUDIO_DECODER_MAIN);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_AddDacOutputPath failed!\n");
				goto out;
			}
		}

		if(stAudioOutput.bSpdifOutputPathConnected == FALSE)
		{
#if defined(CONFIG_DI10)
			stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_PCM;
#else
			stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_DOWNMIX;
#endif
			dResult = P_AUDIO_AddSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_AddDigitalOutputPath failed!\n");
				goto out;
			}
		}

		if(stAudioOutput.bHdmiOutputPathConnected == FALSE)
		{
			stAudioOutput.eHdmiMode = DI_AUDOUT_HDMI_DOWNMIX;
			dResult = P_AUDIO_AddHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_AddDigitalOutputPath failed!\n");
				goto path_remove;
			}
		}

#if defined(CONFIG_BLUETOOTH)
		if(stAudioOutput.bBtCaptureOutputPathConnected == FALSE)
		{
			dResult = P_AUDIO_AddBluetoothPath();
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_AddBluetoothPath be failed!\n");
				goto path_remove;
			}
		}
#endif

		//2. connect audioplayback to dac
		NEXUS_AudioPlayback_GetDefaultStartSettings(&playbackSettings);
		playbackSettings.sampleRate = AUDIO_DEFAULT_SAMPLERATE;// makes 2KHz tone... why??;;;;;
		playbackSettings.bitsPerSample = AUDIO_DEFAULT_BITSPERSAMPLE;
		playbackSettings.signedData = TRUE;
		playbackSettings.stereo = TRUE;
		playbackSettings.dataCallback.callback = NULL;
		playbackSettings.dataCallback.context = NULL;
		playbackSettings.dataCallback.param = 0;

		//3. audio playback start
		if(stAudioPlayback[ulTaskIndex].bStart == TRUE)
		{
			PrintError("Playback is being used!\n");
			goto path_remove;
		}

		VK_TASK_Sleep(100);

		nResult = NEXUS_AudioPlayback_Start(stAudioPlayback[ulTaskIndex].gPlayback, &playbackSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioPlayback_Start failed!\n");
			goto path_remove;
		}
		stAudioPlayback[ulTaskIndex].bStart = TRUE;

		//4. MSG preparation
		stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize = sizeof(PCMsamples)*2;
		stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount = 0;
		stAudioTaskInfo[ulTaskIndex].pbuffStartPoint = (unsigned char *)DD_MEM_Alloc(sizeof(PCMsamples)*2);

		HUINT32 i;

		for(i=0;i<48;i++)
		{
			PCMsamples[i] = s_PCMsamples[i];
		}

		for(i=0;i<48;i++)
		{
			VK_MEM_Memcpy(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint+(i*4) , ((unsigned char *)(PCMsamples)+(i*2)) , 2);
			VK_MEM_Memcpy(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint+(i*4)+2 , ((unsigned char *)(PCMsamples)+(i*2)) , 2);
		}

		audMsg.sDevId = DI_AUDIO_DECODER_MAIN;
		audMsg.sMemBuf = stAudioTaskInfo[ulTaskIndex].pbuffStartPoint;
		audMsg.ulRequestId = 99;
		audMsg.sMemBufSize = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
		audMsg.sLoopCount = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount;
		audMsg.ulTaskIndex = ulTaskIndex;
		audMsg.pos = 0;
		audMsg.sRequest = DI_AUDIO_MEMORY_PCM_PLAY;
		audMsg.ulTimeTicket = stMemoryStreamPool[ulPoolIndex].ulTimeTicket;
		//5. MSG sending to stream task

		vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
		if(vResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
			goto path_remove;
		}
		return DRV_OK;
	}

path_remove:
	bAllStopped = P_AUDIO_CheckAllStopped();
	if(bAllStopped == TRUE)
	{
		dResult = P_AUDIO_RemoveDacOutputPath(DI_AUDIO_DECODER_MAIN);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDacOutputPath failed!\n");
		}

		dResult = P_AUDIO_RemoveHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveHdmiOutputPath faild!\n");
		}

		dResult = P_AUDIO_RemoveSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveSpdifOutputPath faild!\n");
		}
#if defined(CONFIG_BLUETOOTH)
		dResult = P_AUDIO_RemoveBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
		}
#endif
	}
out:
	stMemoryStreamPool[ulPoolIndex].bOccupied = FALSE;
	stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;
	stAudioPlayback[ulTaskIndex].bStart = FALSE;
	return DRV_ERR;
}

DRV_Error DRV_AUDIO_HWMUTE(HBOOL mute)
{
#if defined(CONFIG_AUDIO_NONE_HWMUTE)
	BSTD_UNUSED(mute);
#else
	GPIO_STATE_t 	val;
	DRV_Error 		dResult;

#if defined(CONFIG_AUDIO_LOW_HWMUTE)
	if(mute == TRUE)
	{
		val = GPIO_LOW;
	}
	else
	{
		val = GPIO_HI;
	}
#else
	if(mute == TRUE)
	{
		val = GPIO_HI;
	}
	else
	{
		val = GPIO_LOW;
	}
#endif
	dResult = DRV_GPIO_Write(GPIO_ID_HW_MUTE, val);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_GPIO_Write failed!\n");
		return DRV_ERR;
	}
#endif
	return DRV_OK;

}

DRV_Error DRV_AUDIO_GetAudioFormat(HUINT32 ulDevId, DI_AUDIO_FORMAT *pnType)
{
	HINT32	vResult;

	*pnType = DI_AUDFORMAT_NONE;

	vResult = VK_SEM_Get(s_ulAudioDRVSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DRV_ERR;
	}

	if(stAudioStatus[ulDevId].nState == DI_AUDIO_START)
	{
		*pnType = stAudioStatus[ulDevId].nFormat;
	}
	VK_SEM_Release(s_ulAudioDRVSemId);

	return DRV_OK;
}

DRV_Error DRV_AUDIO_CallbackCheck(void)
{//Sunwoo
	HUINT32 i,j,k;

	for(i =0; i<AUDIO_ALL_DECODERS; i++)
	{
		PrintDebug("Decoder[%d]\n",i);
		for(j=0; j<AUDIO_EVENT_NUM;j++)
		{
			PrintDebug("Event[%d]: ",j);
			for(k=0;k<AUDIO_CALLBACK_OWNER_MAX; k++)
			{
				if(stAudioCallbacks[k].audioEventCallback[i][j] == NULL)
					PrintDebug("[X]");
				else
					PrintDebug("[O]");
			}
			PrintDebug("\n");
		}
	}
	return DRV_OK;
}

DRV_Error DRV_AUDIO_SetDescriptionPidChannel(HUINT16 usPid)
{//Sunwoo
	DI_ERR_CODE dResult;

	Pid_Info pidInfo;

	VK_MEM_Memset(&pidInfo,0x00,sizeof(Pid_Info));

	pidInfo.pidBitmask = 0x00000008;
	pidInfo.usAudioSubPid = usPid;

	dResult = DI_DEMUX_StartPID(0, &pidInfo);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_DEMUX_StartPID failed!\n");
		return DRV_ERR;
	}
	return DRV_OK;
}

NEXUS_AudioCodec DRV_AUDIO_ConvertCodecToNexus(DI_AUDIO_FORMAT nType)
{
	return P_AUDIO_ConvertCodecToNexus(nType);
}

DRV_Error DRV_AUDIO_PlaybackDummyToHDMI(HBOOL isStart)
{
#if (NEXUS_VERSION >= 1290) && !defined(CONFIG_TEMP_HMX4KJP)
	/*
	According to HDMI field test, This function is no more necessary at URSR driver.
	*/
	BSTD_UNUSED(isStart);
	return DRV_OK;
#else
	DRV_Error drvResult = DRV_OK;
	HBOOL								bAllInputStopped = FALSE;
	DI_ERR_CODE 						dResult;
	NEXUS_Error 						nResult;
	NEXUS_AudioInput					input = NULL;
	NEXUS_AudioOutput					output = NULL;
	NEXUS_AudioPlaybackStartSettings	playbackSettings;
	NEXUS_HdmiOutputHandle	hHandle = NULL;
	unsigned char *				buffer;
	unsigned long				buffer_size;

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("bool isStart : %s\n", isStart?"TRUE":"FALSE");

	drvResult = DRV_HDMI_GetHdmiOutputHandle(&hHandle);
	if(drvResult != DRV_OK)
	{
		PrintError("[%s] DRV_HDMI_GetHdmiOutputHandle error %d \n", __FILE__, drvResult);
		goto err_return;
	}

	if (hHandle == NULL)
	{
		PrintError("[%s] %d Error : HDMI handle is NULL!!! \n", __FUNCTION__, __LINE__);
		goto err_return;
	}

	if (isStart == TRUE)
	{
		/* Check to whether need playback dummy or not */
		bAllInputStopped = P_AUDIO_CheckAllStopped();
		if(bAllInputStopped == FALSE ||
			stAudioOutput.bHdmiDummyPlayed == TRUE ||
			stAudioOutput.bHdmiOutputPathConnected == TRUE)
		{
			DI_UART_Print("[%s] %d Playback dummy is unnecessary. \n", __FUNCTION__, __LINE__);
			goto err_return;
		}
		else
		{
			DI_UART_Print("[%s] %d Playback dummy play !!!. \n", __FUNCTION__, __LINE__);
			DI_UART_Print("[%s] %d bAllInputStopped %d \n", __FUNCTION__, __LINE__, bAllInputStopped);
			DI_UART_Print("[%s] %d bHdmiDummyPlayed %d \n", __FUNCTION__, __LINE__, stAudioOutput.bHdmiDummyPlayed);
			DI_UART_Print("[%s] %d bHdmiOutputPathConnected %d \n", __FUNCTION__, __LINE__, stAudioOutput.bHdmiOutputPathConnected);
		}

		/* Connect Playback to HDMI output */
		input = NEXUS_AudioPlayback_GetConnector(stAudioPlayback[0].gPlayback);
		output = NEXUS_HdmiOutput_GetAudioConnector(hHandle);

		if((input != NULL) && (output != NULL))
		{
			dResult = NEXUS_AudioOutput_AddInput(output, input);
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s] %d Error NEXUS_AudioOutput_AddInput!\n", __FUNCTION__, __LINE__);
				goto err_return;
			}
			stAudioOutput.bHdmiDummyPlayed = TRUE;
		}
		else
		{
			PrintError("[%s] %d Error : Input 0x%x ouput 0x%x\n", __FUNCTION__, __LINE__, input, output);
			goto err_return;
		}

		/* Start playback */
		NEXUS_AudioPlayback_GetDefaultStartSettings(&playbackSettings);
		playbackSettings.sampleRate = AUDIO_DEFAULT_SAMPLERATE;
		playbackSettings.bitsPerSample = AUDIO_DEFAULT_BITSPERSAMPLE;
		playbackSettings.signedData = TRUE;
		playbackSettings.stereo = TRUE;
		playbackSettings.dataCallback.callback = NULL;
		playbackSettings.dataCallback.context = NULL;
		playbackSettings.dataCallback.param = 0;

		nResult = NEXUS_AudioPlayback_Start(stAudioPlayback[0].gPlayback, &playbackSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioPlayback_Start failed!\n");
			goto err_remove_input;
		}
		nResult = NEXUS_AudioPlayback_GetBuffer(stAudioPlayback[0].gPlayback, (void **)&buffer, (size_t *)&buffer_size);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioPlayback_GetBuffer Failed!\n");
			return DRV_ERR;
		}

		VK_MEM_Memset(buffer, 0, buffer_size);

		nResult = NEXUS_AudioPlayback_ReadComplete(stAudioPlayback[0].gPlayback, buffer_size);//buffer_size);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioPlayback_ReadComplete failed!\n");
			return DRV_ERR;
		}
		stAudioPlayback[0].bStart = TRUE;

		return DRV_OK;
	}
	else
	{
		if (stAudioOutput.bHdmiDummyPlayed == FALSE)
		{
			return DRV_OK;
		}

		/* Stop playback */
		NEXUS_AudioPlayback_Stop(stAudioPlayback[0].gPlayback);
		stAudioPlayback[0].bStart = FALSE;

		/* Remove dummy input of HDMI output */
		output = NEXUS_HdmiOutput_GetAudioConnector(hHandle);
		if (output != NULL)
		{
			NEXUS_AudioOutput_RemoveAllInputs(output);
		}
		stAudioOutput.bHdmiDummyPlayed = FALSE;

		return DRV_OK;
	}

err_remove_input:
	NEXUS_AudioOutput_RemoveAllInputs(output);
	stAudioOutput.bHdmiOutputPathConnected = FALSE;
err_return:
	return DRV_ERR;

#endif

}

DRV_Error DRV_AUDIO_ConnectAudioDummyOutput(NEXUS_AudioInput input)
{
#if defined(NEXUS_NUM_AUDIO_DUMMY_OUTPUTS)
	int i=0;
	NEXUS_Error 	nResult = NEXUS_SUCCESS;
	NEXUS_AudioOutput output=NULL;
	NEXUS_PlatformConfiguration platformConfig;

	for (i=0; i<NEXUS_NUM_AUDIO_DUMMY_OUTPUTS; i++)
	{
		if(stAudioInstance.audioDummyOutput[i].bUsedDummy == FALSE)
		{
			NEXUS_Platform_GetConfiguration(&platformConfig);

			output = NEXUS_AudioDummyOutput_GetConnector(platformConfig.outputs.audioDummy[i]);
			nResult = NEXUS_AudioOutput_AddInput(output, input);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_AudioOutput_AddInput failed! \n", __FUNCTION__);
				return DRV_ERR;
			}
			else
			{
				stAudioInstance.audioDummyOutput[i].bUsedDummy = TRUE;
				stAudioInstance.audioDummyOutput[i].input = input;
				PrintDebug("[%s] Dummy[%d] is connected \n", __FUNCTION__, i);
				return DRV_OK;
			}
		}
	}

	PrintError("[%s] There is NO free AudioDummyOutput \n", __FUNCTION__);
	return DRV_ERR;
#else
	(void)input;
	return DRV_OK;
#endif
}

DRV_Error DRV_AUDIO_GetAudioDummyOutputHandle(NEXUS_AudioInput input, NEXUS_AudioOutput *output)
{
#if defined(NEXUS_NUM_AUDIO_DUMMY_OUTPUTS)
	int i=0;
	NEXUS_PlatformConfiguration platformConfig;

	for (i=0; i<NEXUS_NUM_AUDIO_DUMMY_OUTPUTS; i++)
	{
		if(stAudioInstance.audioDummyOutput[i].bUsedDummy == TRUE && stAudioInstance.audioDummyOutput[i].input == input)
		{
			NEXUS_Platform_GetConfiguration(&platformConfig);
			*output = NEXUS_AudioDummyOutput_GetConnector(platformConfig.outputs.audioDummy[i]);
			PrintDebug("[%s] Get Dummy[%d] handle\n", __FUNCTION__, i);
			return DRV_OK;
		}
	}

	PrintError("[%s] There is no matching AudioDummyOutput Handle \n", __FUNCTION__);
	return DRV_ERR;
#else
	(void)input;
 	(void)output;
	return DRV_ERR;
#endif
}

DRV_Error DRV_AUDIO_DisconnectAudioDummyOutput(NEXUS_AudioInput input)
{
#if defined(NEXUS_NUM_AUDIO_DUMMY_OUTPUTS)
	int i=0;
	NEXUS_Error 	nResult = NEXUS_SUCCESS;
	NEXUS_AudioOutput output=NULL;
	NEXUS_PlatformConfiguration platformConfig;

	for (i=0; i<NEXUS_NUM_AUDIO_DUMMY_OUTPUTS; i++)
	{
		if(stAudioInstance.audioDummyOutput[i].input == input)
		{
			NEXUS_Platform_GetConfiguration(&platformConfig);
			output = NEXUS_AudioDummyOutput_GetConnector(platformConfig.outputs.audioDummy[i]);
			nResult = NEXUS_AudioOutput_RemoveAllInputs(output);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("[%s] %d NEXUS_AudioOutput_RemoveAllInputs failed! \n",__FUNCTION__,__LINE__);
				return DRV_ERR;
			}
			else
			{
				NEXUS_AudioOutput_Shutdown(output);
				stAudioInstance.audioDummyOutput[i].bUsedDummy = FALSE;
				stAudioInstance.audioDummyOutput[i].input = NULL;
				PrintDebug("[%s] Dummy[%d] is freed \n", __FUNCTION__, i);
				return DRV_OK;
			}
		}
	}

	PrintError("[%s] Can't find AudioDummyOutput\n", __FUNCTION__);
	return DRV_ERR;
#else
	(void)input;
	return DRV_OK;
#endif
}

DRV_Error DRV_AUDIO_GetStreamInfo(HUINT32 ulDevId, DI_AUDIO_STREAM_INFO_t *ptAudioStreamInfo)
{
	NEXUS_Error 				nResult;
	NEXUS_AudioDecoderStatus	currentStatus;

	if(stAudioStatus[ulDevId].rState.state != DI_AUDIO_STATE_DECODING)
	{
#if defined (CONFIG_WORK_AROUND_LOCAL_STREAMINFO)
		ptAudioStreamInfo->eFormat = stAudioStatus[ulDevId].nFormat;
		ptAudioStreamInfo->eMode = DI_AUDIO_MODE_STEREO;
		ptAudioStreamInfo->ulSampleRate = 48000;

		PrintDebug("Decoder is not decoding input stream, Stream Information is local value!\n");
		return DRV_OK;
#else
		PrintDebug("Decoder is not decoding input stream, Stream Information is incorrect!\n");
		return DRV_ERR;
#endif
	}

	nResult = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[ulDevId], &currentStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
		return DRV_ERR;
	}

	ptAudioStreamInfo->eFormat = stAudioStatus[ulDevId].nFormat;//P_AUDIO_ConvertCodecToDi(currentStatus.codec);
	ptAudioStreamInfo->ulSampleRate = currentStatus.sampleRate;

#if defined(CONFIG_DI10)
	if(stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MPEG ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MP3 ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MEDIA_MPEG ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MEDIA_MP3)
	{
		ptAudioStreamInfo->eMode = P_AUDIO_ConvertCodecModeToDi(currentStatus.codec,currentStatus.codecStatus.mpeg.channelMode);
	}
	else if(stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AC3 ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AC3_PLUS ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MEDIA_AC3 ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MEDIA_AC3_PLUS)
	{
		ptAudioStreamInfo->eMode = P_AUDIO_ConvertCodecModeToDi(currentStatus.codec,currentStatus.codecStatus.ac3.acmod);
	}
	else if(stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC_LOAS ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC_PLUS ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC_PLUS_ADTS ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MEDIA_AAC ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MEDIA_AAC_PLUS ||
	stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MEDIA_AAC_PLUS_ADTS)
	{
		ptAudioStreamInfo->eMode = P_AUDIO_ConvertCodecModeToDi(currentStatus.codec,currentStatus.codecStatus.aac.acmod);
	}
#else
	if(stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MPEG ||
		stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_MP3)
	{
		ptAudioStreamInfo->eMode = P_AUDIO_ConvertCodecModeToDi(currentStatus.codec,currentStatus.codecStatus.mpeg.channelMode);
	}
	else if(stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AC3 ||
		stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AC3_PLUS)
	{
		ptAudioStreamInfo->eMode = P_AUDIO_ConvertCodecModeToDi(currentStatus.codec,currentStatus.codecStatus.ac3.acmod);
	}
	else if(stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC ||
		stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC_LOAS ||
		stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC_PLUS ||
		stAudioStatus[ulDevId].nFormat == DI_AUDFORMAT_AAC_PLUS_ADTS)
	{
		ptAudioStreamInfo->eMode = P_AUDIO_ConvertCodecModeToDi(currentStatus.codec,currentStatus.codecStatus.aac.acmod);
	}
#endif
	else
	{
		PrintError("Can't not get codec mode! \n");
		ptAudioStreamInfo->eMode = DI_AUDIO_MODE_UNKNOWN;
		return DRV_ERR;
	}

	return DRV_OK;
}

#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
HUINT32 DRV_AUDIO_GetPrimerIndex(HUINT32 ulDevId, HUINT32 *pPrimerIndex)
{
	DRV_Error dResult = DRV_OK;

	if(pPrimerIndex == NULL)
	{
		PrintError("[%s] ERROR : pPrimerIndex is NULL \n", __FUNCTION__);
		return DRV_ERR;
	}

	if(stMultiTrackInfo[ulDevId].bStartPrimer == TRUE)
	{
		*pPrimerIndex = stMultiTrackInfo[ulDevId].ulSelectedIndex;
	}
	else
	{
		PrintDebug("[%s] Starting primer is not selected !!! \n",__func__);
		dResult = DI_ERR_NO_ALLOC;
	}

	return dResult;
}

HBOOL DRV_AUDIO_IsStartedPrimer(HUINT32 ulDevId)
{
	PrintEnter();

	return stMultiTrackInfo[ulDevId].bStartPrimer;

	PrintExit();
}
#endif

#if defined(TEMP_HRA_SELF_TEST)
#include <string.h>

typedef struct
{
	char aFilePath[512];
	DI_AUDIO_FORMAT eFormat;
} DI_AUDIO_HRA_TRACK_INFO;

typedef struct
{
	HBOOL bGetList;
	HUINT32 ulTotalCount;
	DI_AUDIO_HRA_TRACK_INFO *ppTrackList;
} DI_AUDIO_HRA_INFO;

static DI_AUDIO_HRA_INFO gHraInstance;

DRV_Error DRV_AUDIO_GetHraTrackList(void);
DRV_Error DRV_AUDIO_StartHraTrack(int track_num);
DRV_Error DRV_AUDIO_StopHraTrack(void);
DRV_Error DRV_AUDIO_ShowHraTrackList(void);

DRV_Error DRV_AUDIO_ShowHraTrackList(void)
{
	int i = 0;
	DI_UART_Print("[HRA track list]\n");
	for(i=0; i<gHraInstance.ulTotalCount; i++)
	{
		DI_UART_Print("[%d] %s \n",i, gHraInstance.ppTrackList[i].aFilePath);
	}
}

DRV_Error DRV_AUDIO_GetHraTrackList(void)
{
	FILE *fp = NULL;
	HINT32 nRet = 0;
	long int ulFileSize=0;
	char *pBuf = NULL;
	size_t readSize = 0;
	HUINT32 index = 0;
	HCHAR TotalCnt = 0;
	HCHAR* pTemp = 0;
	int i=0;

	VK_SYSTEM_Command("find /media/ -name *.flac -maxdepth 1 > tmp/hra_list.log");
	VK_SYSTEM_Command("find /media/ -name *.wav -maxdepth 1 >> tmp/hra_list.log");

	fp = fopen("tmp/hra_list.log","r");
	if(fp == NULL)
	{
		PrintError("[DRV_AUDIO_GetHraTrackList] Err fopen \n");
		return DRV_ERR;
	}

	while(1)
	{
		/* Get file size */
		nRet = fseek(fp, 0, SEEK_END);
		if(nRet != 0)
		{
			fclose(fp);
			return DRV_ERR;
		}

		ulFileSize = ftell(fp);
		if(ulFileSize <= 0)
		{
			fclose(fp);
			return DRV_ERR;
		}

		nRet = fseek(fp, 0, SEEK_SET);
		if(nRet != 0)
		{
			fclose(fp);
			return DRV_ERR;
		}

		pBuf = DD_MEM_Alloc(ulFileSize+1);
		if(pBuf == NULL)
		{
			fclose(fp);
			return DRV_ERR;
		}
		VK_MEM_Memset(pBuf, 0x0, ulFileSize+1);

		/* Read AP info from file */
		readSize = fread(pBuf, 1, ulFileSize, fp);
		if (readSize != (size_t)ulFileSize)
		{
			DD_MEM_Free(pBuf);
			fclose(fp);
			return DRV_ERR;
		}

		pBuf[readSize] = '\0';
		break;
	}

	index = 0;
	TotalCnt = 0;
	while(VK_strncmp(&pBuf[index], "\0", 1))
	{
		pTemp = VK_strstr(&pBuf[index],"\n");
		TotalCnt++;
		index += (pTemp-&pBuf[index])+1;
	}

	if(gHraInstance.bGetList == TRUE && gHraInstance.ppTrackList != NULL)
	{
		DD_MEM_Free(gHraInstance.ppTrackList);
	}
	VK_MEM_Memset(&gHraInstance, 0x00, sizeof(DI_AUDIO_HRA_INFO));
	gHraInstance.ulTotalCount = TotalCnt;
	gHraInstance.ppTrackList = DD_MEM_Alloc(TotalCnt*(sizeof(DI_AUDIO_HRA_TRACK_INFO)));

	index = 0;
	for(i=0; i<gHraInstance.ulTotalCount; i++)
	{
		pTemp = VK_strstr(&pBuf[index],"\n");
		//gHraInstance.ppTrackList[i] = DD_MEM_Alloc((pTemp-&pBuf[index]));
		VK_strncpy(gHraInstance.ppTrackList[i].aFilePath , &pBuf[index], (pTemp-&pBuf[index]));
		gHraInstance.ppTrackList[i].aFilePath[(pTemp-&pBuf[index])] = '\0';
		index += (pTemp-&pBuf[index])+1;

		if(VK_strstr(gHraInstance.ppTrackList[i].aFilePath, "flac") != NULL)
			gHraInstance.ppTrackList[i].eFormat = DI_AUDFORMAT_FLAC;

		if(VK_strstr(gHraInstance.ppTrackList[i].aFilePath, "wav") != NULL)
			gHraInstance.ppTrackList[i].eFormat = DI_AUDFORMAT_PCMWAVE;
	}

	gHraInstance.bGetList = TRUE;

	DD_MEM_Free(pBuf);
	fclose(fp);

	return DRV_OK;
}
DRV_Error DRV_AUDIO_StartHraTrack(int track_num)
{
	NEXUS_FilePlayHandle hFilePlay;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings pidSettings;
	NEXUS_PlaybackStartSettings playbackStartSettings;

	if(stHdAudioInstance.bStart == TRUE)
	{
		DI_UART_Print("[HRA] Already Started previous track. Enter 'audio hra stop' !!! \n");
		return DRV_OK;
	}

	/* Open HRA file */
	stHdAudioInstance.hFile = NEXUS_FilePlay_OpenPosix(gHraInstance.ppTrackList[track_num].aFilePath, NULL);

	/* Get playback */
	stHdAudioInstance.hPlayback = NEXUS_Playback_Create();

	/* Get playpump */
	DRV_PVR_GetPlaypumpHandle(&stHdAudioInstance.hPlayPump);

	/* Set playback setting */
	NEXUS_Playback_GetSettings(stHdAudioInstance.hPlayback, &playbackSettings);
	playbackSettings.playpump = stHdAudioInstance.hPlayPump;
	if(gHraInstance.ppTrackList[track_num].eFormat == DI_AUDFORMAT_FLAC)
		playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eFlac;
	else if(gHraInstance.ppTrackList[track_num].eFormat == DI_AUDFORMAT_PCMWAVE)
		playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eWav;
	else
		DI_UART_Print("[HRA] eFormat Error !!! \n");
#if 0
	playbackSettings.endOfStreamCallback.callback = eof;
	playbackSettings.endOfStreamCallback.context = event;
#endif
#if 0
	playbackSettings.enableStreamProcessing = true; /* this should be set to true if application needs to seek in the MP3 file, it'd cause decoder to receive MPEG-2 PES data */
#endif
	NEXUS_Playback_SetSettings(stHdAudioInstance.hPlayback, &playbackSettings);

	/* Set decoder starting setting */
	NEXUS_AudioDecoder_GetDefaultStartSettings(&stAudioStatus[0].audioSettings);
	stAudioStatus[0].audioSettings.maxOutputRate = MAX_OUTPUT_RATE_HRA;
	stAudioStatus[0].audioSettings.codec = NEXUS_AudioCodec_eFlac;

    NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    pidSettings.pidSettings.pidTypeSettings.audio.codec = stAudioStatus[0].audioSettings.codec;
    pidSettings.pidTypeSettings.audio.primary = stAudioInstance.audioDecoder[0];
	stAudioStatus[0].audioSettings.pidChannel = NEXUS_Playback_OpenPidChannel(stHdAudioInstance.hPlayback, 0x1, &pidSettings);

	/* Start playback */
	NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
	playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
	NEXUS_Playback_Start(stHdAudioInstance.hPlayback, stHdAudioInstance.hFile, &playbackStartSettings);

	/* Set path of outputs */
	P_AUDIO_AddI2sOutputPath(0);
	P_AUDIO_AddDacOutputPath(0);
	P_AUDIO_AddSpdifOutputPath(0, DI_AUDFORMAT_FLAC);
	P_AUDIO_AddHdmiOutputPath(0, DI_AUDFORMAT_FLAC);

	/* decoder start */
	NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[0], &stAudioStatus[0].audioSettings);
	stHdAudioInstance.bStart = TRUE;
	DI_UART_Print("[HRA] Started File : %s \n",gHraInstance.ppTrackList[track_num].aFilePath);

	return DRV_OK;
}

DRV_Error DRV_AUDIO_StopHraTrack(void)
{
	NEXUS_FilePlayHandle hFilePlay;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings pidSettings;
	NEXUS_PlaybackStartSettings playbackStartSettings;

	/* Decoder stop */
	NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[0]);

	/* Playback stop */
	NEXUS_Playback_Stop(stHdAudioInstance.hPlayback);

	/* Remove path of outputs */
	P_AUDIO_RemoveI2sOutputPath(0);
	P_AUDIO_RemoveDacOutputPath(0);
	P_AUDIO_RemoveSpdifOutputPath(0, DI_AUDFORMAT_FLAC);
	P_AUDIO_RemoveHdmiOutputPath(0, DI_AUDFORMAT_FLAC);

	/* Free another */
	DRV_PVR_ReleasePlaypumpHandle(stHdAudioInstance.hPlayPump);
	NEXUS_Playback_ClosePidChannel(stHdAudioInstance.hPlayback, stAudioStatus[0].audioSettings.pidChannel);
    NEXUS_Playback_Destroy(stHdAudioInstance.hPlayback);
	NEXUS_FilePlay_Close(stHdAudioInstance.hFile);
	stHdAudioInstance.bStart = FALSE;
	return DRV_OK;
}

#endif

#if __________di____________
#endif

DI_ERR_CODE DI_AUDIO_Init(void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_Start(HUINT32 ulDevId ,DI_AUDIO_FORMAT nType)
{
	DRV_Error 					dResult = DRV_OK;
	HINT32						vResult;
	HBOOL						bSPDIFStart;

/*///////////Debug Message///////////*/

	DI_UART_Print("[%s].%d\n",__FUNCTION__,__LINE__);
	DI_UART_Print("HUINT32 ulDevId: %d, DI_AUDIO_FORMAT nType: 0x%08X\n", ulDevId, nType);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		PrintError("Pass through audio should be controled by DI audio! \n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_NOT_SUPPORTED;
	}

#if defined(CONFIG_SYNC_RECOVERY)
	DRV_SYNC_SetTsmMode();
#endif
	/*	1. DRV Decoder Start
	*	If Digital Output mode is Compressed, turn on the Spdif decoder.
	*/
	dResult = DRV_AUDIO_Start( ulDevId, nType);
	if ( dResult != DRV_OK )
	{
		PrintError("DRV_AUDIO_Start failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);

		return DI_ERR_ERROR;
	}

	bSPDIFStart = P_AUDIO_CheckSPDIFStart(ulDevId, nType);
	if(bSPDIFStart == TRUE)
	{
		if(stAudioStatus[DI_AUDIO_DECODER_SPDIF].nState != DI_AUDIO_START)
		{
			dResult = DRV_AUDIO_Start( DI_AUDIO_DECODER_SPDIF, nType);
			if ( dResult != DRV_OK )
			{
				PrintError("DRV_AUDIO_Start failed!\n");
				VK_SEM_Release(s_ulAudioDISemId);

				return DI_ERR_ERROR;
			}
		}
	}

	VK_SEM_Release(s_ulAudioDISemId);


	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_Stop (HUINT32 ulDevId)
{//Sunwoo
	DRV_Error 	eDRVError = DRV_OK;
	HINT32		vResult;
	DI_AUDIO_FORMAT			nCodec;
	HBOOL		bLocalSpdifStart=FALSE;

/*///////////Debug Message///////////*/

	DI_UART_Print("[%s].%d\n",__FUNCTION__,__LINE__);
	DI_UART_Print("HUINT32 ulDevId: %d\n", ulDevId);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		PrintError("Pass through audio should be controled by DI audio! \n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_NOT_SUPPORTED;
	}

	nCodec = stAudioStatus[ulDevId].nFormat;
	eDRVError = DRV_AUDIO_Stop( ulDevId );
	if ( eDRVError != DRV_OK )
	{
		PrintError("DRV_AUDIO_Stop failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);

		return DI_ERR_ERROR;
	}
	bLocalSpdifStart = P_AUDIO_CheckSPDIFStart(ulDevId, nCodec);
	if(bLocalSpdifStart == TRUE)
	{
		if(stAudioStatus[DI_AUDIO_DECODER_SPDIF].nState != DI_AUDIO_STOP)
		{
			eDRVError = DRV_AUDIO_Stop( DI_AUDIO_DECODER_SPDIF );
			if ( eDRVError != DRV_OK )
			{
				PrintError("DRV_AUDIO_Stop failed!\n");
				VK_SEM_Release(s_ulAudioDISemId);

				return DI_ERR_ERROR;
			}
		}
	}
	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_Pause(HUINT32 ulDevId, HBOOL pause)
{
	DRV_Error dResult;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDISemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	dResult = P_AUDIO_PauseDecoder(ulDevId, pause);
	if(dResult != DRV_OK)
	{
		PrintError("DI_AUDIO_Pause failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

#if defined(CONFIG_DI20)
DI_ERR_CODE DI_AUDIO_Flush(HUINT32 ulDevId)
{
	DRV_Error dResult;
	int nVkRet;

	nVkRet = VK_SEM_Get(s_ulAudioDISemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	dResult = P_AUDIO_FlushDecoder(ulDevId);
	if(dResult != DRV_OK)
	{
		PrintError("DI_AUDIO_Flush failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_AUDIO_StartPrimer(HUINT32 ulDevId, DI_AUDIO_PRIMER_t *pPrimerSetting)
{
	int nVkRet = 0;
	HUINT32 index = 0;
	DRV_Error dResult = DRV_OK;

	PrintDebug("[%s] ulDevId=%d \n", __FUNCTION__, ulDevId);

#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	nVkRet = VK_SEM_Get(s_ulAudioDISemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	if(pPrimerSetting == NULL)
	{
		PrintError("[%s] ERROR : pFormatList is NULL \n", __FUNCTION__);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	if(pPrimerSetting->ulPrimerIndex >= AUDIO_PRIMER_MAX_COUNT)
	{
		PrintError("[%s] ERROR : ulPrimerIndex(%d) >= AUDIO_PRIMER_MAX_COUNT(%d)\n", __FUNCTION__, pPrimerSetting->ulPrimerIndex, AUDIO_PRIMER_MAX_COUNT );
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		PrintError("[%s] Pass through audio should be controled by DI audio! ulDevId=%d \n", __FUNCTION__, ulDevId);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_NOT_SUPPORTED;
	}

	index = pPrimerSetting->ulPrimerIndex;
	if(stMultiTrackInfo[ulDevId].bOccupied[index] == TRUE)
	{
		PrintError("[%s] Primer[%d] is already occupied !!! ulDevId=%d \n", __FUNCTION__, pPrimerSetting->ulPrimerIndex, ulDevId);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	/* Start primer with new setting */
	stMultiTrackInfo[ulDevId].eAudioType = pPrimerSetting->eAudioType;
	stMultiTrackInfo[ulDevId].eFormat[index] = pPrimerSetting->eFormat;

	dResult = P_AUDIO_StartPrimers(ulDevId, index);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] P_AUDIO_StartPrimers failed! ulDevId=%d \n", __FUNCTION__, ulDevId);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		stMultiTrackInfo[DI_AUDIO_DECODER_SPDIF].eAudioType = stMultiTrackInfo[ulDevId].eAudioType;
		stMultiTrackInfo[DI_AUDIO_DECODER_SPDIF].eFormat[index] = stMultiTrackInfo[ulDevId].eFormat[index];

		dResult = P_AUDIO_StartPrimers(DI_AUDIO_DECODER_SPDIF, index);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] P_AUDIO_StartPrimers failed! ulDevId=%d \n", __FUNCTION__, DI_AUDIO_DECODER_SPDIF);
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}

	VK_SEM_Release(s_ulAudioDISemId);
#else
	BSTD_UNUSED(ulDevId);
	BSTD_UNUSED(pPrimerSetting);
	BSTD_UNUSED(nVkRet);
	BSTD_UNUSED(index);
	BSTD_UNUSED(dResult);
#endif
	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_StopPrimer(HUINT32 ulDevId, DI_AUDIO_PRIMER_t *pPrimerSetting)
{
	int nVkRet = 0;
	HUINT32 index = 0;
	DRV_Error dResult = DRV_OK;

	PrintDebug("[%s] ulDevId=%d \n", __FUNCTION__, ulDevId);

#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	nVkRet = VK_SEM_Get(s_ulAudioDISemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	if(pPrimerSetting == NULL)
	{
		PrintError("[%s] ERROR : pFormatList is NULL \n", __FUNCTION__);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	if(pPrimerSetting->ulPrimerIndex >= AUDIO_PRIMER_MAX_COUNT)
	{
		PrintError("[%s] ERROR : ulPrimerIndex(%d) >= AUDIO_PRIMER_MAX_COUNT(%d)\n", __FUNCTION__, pPrimerSetting->ulPrimerIndex, AUDIO_PRIMER_MAX_COUNT );
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}
	if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		PrintError("[%s] Pass through audio should be controled by DI audio! ulDevId=%d \n", __FUNCTION__, ulDevId);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_NOT_SUPPORTED;
	}

	index = pPrimerSetting->ulPrimerIndex;
	if(stMultiTrackInfo[ulDevId].bStartPrimer != TRUE ||
		stMultiTrackInfo[ulDevId].bOccupied[index] != TRUE)
	{
		PrintError("[%s] Missmatch primer !!! ulDevId=%d index=%d \n", __FUNCTION__, ulDevId, index);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}


	dResult = P_AUDIO_StopPrimers(ulDevId, index);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] P_AUDIO_StopPrimers failed! ulDevId=%d \n", __FUNCTION__, ulDevId);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		dResult = P_AUDIO_StopPrimers(DI_AUDIO_DECODER_SPDIF, index);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] P_AUDIO_StopPrimers failed! ulDevId=%d \n", __FUNCTION__, DI_AUDIO_DECODER_SPDIF);
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}

	VK_SEM_Release(s_ulAudioDISemId);
#else
	BSTD_UNUSED(ulDevId);
	BSTD_UNUSED(pPrimerSetting);
	BSTD_UNUSED(nVkRet);
	BSTD_UNUSED(index);
	BSTD_UNUSED(dResult);
#endif
	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_SelectPrimer(HUINT32 ulDevId, HUINT32 ulPrimerIndex)
{
	int nVkRet = 0;

	PrintDebug("[%s] ulDevId=%d \n", __FUNCTION__, ulDevId);

#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER) || defined(CONFIG_FAST_AUDIO_CHANGE)
	nVkRet = VK_SEM_Get(s_ulAudioDISemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	stMultiTrackInfo[ulDevId].ulSelectedIndex = ulPrimerIndex;
	if(ulDevId == DI_AUDIO_DECODER_MAIN)
	{
		stMultiTrackInfo[DI_AUDIO_DECODER_SPDIF].ulSelectedIndex = ulPrimerIndex;
	}

	VK_SEM_Release(s_ulAudioDISemId);
#else
	BSTD_UNUSED(ulDevId);
	BSTD_UNUSED(ulPrimerIndex);
	BSTD_UNUSED(nVkRet);
#endif
	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_SetStereo(HUINT32 ulDevId, DI_AUDIO_STEREO_MODE dualMode)
{//Sunwoo
	NEXUS_AudioDecoderSettings		pSettings;
	NEXUS_Error 					eNError = NEXUS_SUCCESS;
	HINT32							vResult;

	/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("HUINT32 ulDevId: %d, DI_AUDIO_STEREO_MODE dualMode: %d\n", ulDevId, dualMode);

	/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	/*	1. Decoder Setting **********
	****************************/
	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &pSettings);
	if(dualMode == DI_AUDIO_DUAL_Stereo)
	{
		pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eStereo;
	}
	else if(dualMode == DI_AUDIO_DUAL_LeftOnly)
	{
		pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eLeft;
	}
	else if(dualMode == DI_AUDIO_DUAL_RightOnly)
	{
		pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eRight;
	}
	else if(dualMode == DI_AUDIO_DUAL_Mix)
	{
		pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eMix;
	}

	eNError = NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &pSettings);
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_SetSettings failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);

		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_SetVolume(DI_AUDIO_OUTPUT eOutput, HUINT32 ulVolume)
{//Sunwoo
	DRV_Error 	dResult = DRV_OK;
	HINT32 		vResult;
	HUINT32		ulTarget=0;

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("DI_AUDIO_OUTPUT eOutput: %d, HUINT32 ulVolume: %d\n", eOutput, ulVolume);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

/*************************
*	1. Volume Ctrl Target Check
*************************/
	if(eOutput > DI_AUDIO_OUTPUT_HDMI && eOutput != DI_AUDIO_OUTPUT_I2S)
	{
		PrintError("Invalid Output Parameter! only DAC, SPDIF, HDMI output are available!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	if(eOutput == DI_AUDIO_OUTPUT_ALL)
	{
		ulTarget |= DI_AUDIO_OUTPUT_DAC;
		ulTarget |= DI_AUDIO_OUTPUT_SPDIF;
		ulTarget |= DI_AUDIO_OUTPUT_HDMI;
#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
		ulTarget |= DI_AUDIO_OUTPUT_I2S;
#endif
	}
	else
	{
		ulTarget = eOutput;
	}

	if(ulVolume > AUDIO_LEVEL_MAX)
	{
		PrintError("Audio Volume value is out of range, Parameter was %d\n", ulVolume);
		ulVolume = AUDIO_LEVEL_MAX;
	}

/*************************
*	2.1. Set up DAC
*************************/
	if(ulTarget & DI_AUDIO_OUTPUT_DAC)
	{
#if defined(CONFIG_SCART_6417)
		dResult = DRV_SCART_SetVolume(ulVolume);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SCART_SetVolume failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
#else
		dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_DAC, ulVolume);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetVolume failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
#endif
		stAudioOutput.ulAnalogVolume = ulVolume;
	}

/*************************
*	2.2. Set up SPDIF
*************************/
	if(ulTarget & DI_AUDIO_OUTPUT_SPDIF)
	{
		dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_SPDIF, ulVolume);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetVolume failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
		stAudioOutput.ulSpdifVolume = ulVolume;
	}

/*************************
*	2.3. Set up HDMI
*************************/
	if(ulTarget & DI_AUDIO_OUTPUT_HDMI)
	{
		dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_HDMI, ulVolume);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetVolume failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
		stAudioOutput.ulHdmiVolume = ulVolume;
	}

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
	/* I2S volume control */
	if(ulTarget & DI_AUDIO_OUTPUT_I2S)
	{
		dResult = P_AUDIO_SetVolume(DI_AUDIO_OUTPUT_I2S, ulVolume);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetVolume failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
		stAudioOutput.ulI2sVolume = ulVolume;
	}
#endif

	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_OK;
}

#if defined(CONFIG_DI10)
DI_ERR_CODE DI_AUDIO_SetDigitalOutput(HUINT32 ulDecId, DI_AUDIO_DOUT_MODE mode)
{//Sunwoo
	BSTD_UNUSED(ulDecId);
	BSTD_UNUSED(mode);

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_AUDIO_SetSpdifOutput (DI_AUDIO_SPDIF_MODE mode)
{//Sunwoo
	HINT32					vResult;
	DRV_Error				dResult = DRV_OK;

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("DI_AUDIO_DOUT_MODE mode: %d\n", mode);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

/***************************
*	1. Spdif mode check
***************************/
	if(stAudioOutput.eSpdifMode == mode)
	{
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_OK;
	}

/***************************
*	2. Spdif mode change
***************************/
	dResult = P_AUDIO_ChangeSpdifOutput(mode);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_ChangeSpdifOutput failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;

}

DI_ERR_CODE DI_AUDIO_SetHdmiOutput( DI_AUDIO_HDMI_MODE mode)
{//Sunwoo
	HINT32					vResult;
	DRV_Error				dResult = DRV_OK;

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("DI_AUDIO_DOUT_MODE mode: %d\n", mode);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

/***************************
*	1. Hdmi mode check
***************************/

	if(stAudioOutput.eHdmiMode == mode)
	{
		stAudioOutput.eHdmiMode = mode;
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_OK;
	}

/***************************
*	2. Hdmi mode update
***************************/
	s_HdmiInitialized = FALSE;

	dResult = P_AUDIO_ChangeHdmiOutput(mode);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_ChangeHdmiOutput failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;

}

DI_ERR_CODE DI_AUDIO_SetHraOutput(HBOOL bSetPath)
{
	HINT32					vResult;

	PrintDebug("[%s].%d bSetPath : %d \n",__FUNCTION__,__LINE__, bSetPath);

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if(stAudioOutput.bI2sPath == bSetPath)
	{
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_OK;
	}
	else
	{
		stAudioOutput.bI2sPath = bSetPath;
	}


	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;

}

DI_ERR_CODE DI_AUDIO_SetMute(DI_AUDIO_OUTPUT eOutput, HBOOL mute)
{//Sunwoo
	DRV_Error 	dResult;
	HUINT32		vResult;
	HUINT32		ulTarget=0;

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("DI_AUDIO_OUTPUT eOutput: %d, HBOOL mute: %s\n", eOutput, mute?"TRUE":"FALSE");

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

/*************************
*	1. Mute Target Check
*************************/
	if(eOutput == DI_AUDIO_OUTPUT_ALL)
	{
		ulTarget |= DI_AUDIO_OUTPUT_DAC;
		ulTarget |= DI_AUDIO_OUTPUT_SPDIF;
		ulTarget |= DI_AUDIO_OUTPUT_HDMI;
		ulTarget |= DI_AUDIO_OUTPUT_SCART_2;
#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
		ulTarget |= DI_AUDIO_OUTPUT_I2S;
#endif
	}
	else if((eOutput == DI_AUDIO_OUTPUT_SPEAKER) ||(eOutput == DI_AUDIO_OUTPUT_HP)||(eOutput == DI_AUDIO_OUTPUT_SCART_1))
	{
		PrintError("eOutput: 0x%x is not supported!\n", eOutput);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_NOT_SUPPORTED;
	}
	else
	{
		ulTarget = eOutput;
	}

/*************************
*	2.1. Mute on DAC
*************************/
	if(ulTarget & DI_AUDIO_OUTPUT_DAC)
	{
#if defined(CONFIG_SCART_6417)
		dResult = DRV_SCART_SetTVMute(mute);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SCART_SetTVMute failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
#else
		dResult = P_AUDIO_SetOutputMute(DI_AUDIO_OUTPUT_DAC, mute);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetOutputMute failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
#endif
		stAudioOutput.bAnalogMute = mute;
	}

/*************************
*	2.2. Mute on SPDIF
*************************/
	if(ulTarget & DI_AUDIO_OUTPUT_SPDIF)
	{
		dResult = P_AUDIO_SetOutputMute(DI_AUDIO_OUTPUT_SPDIF, mute);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetOutputMute failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
		stAudioOutput.bSpdifMute = mute;
	}

/*************************
*	2.3. Mute on HDMI
*************************/
	if(ulTarget & DI_AUDIO_OUTPUT_HDMI)
	{
		dResult = P_AUDIO_SetOutputMute(DI_AUDIO_OUTPUT_HDMI, mute);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetOutputMute failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
/*************************
*	Workaround for Panasonic TV
*************************/
//mute fuction was added for Panasoninc TX-P42S20ES.  Pop noise during channel change.(2010.08)
//this fuction is the cause of no sound issue with LG TV when changing from AC3 to PCM.
//it's checked that there's not pop noise with panasonic tv after removing the function(2011.10)
/* 2011.12.29 : VTC-HD06 Audio pop noise workaround when channel zapping */
#if 0
		DRV_HDMI_SetAudioMute( mute );
#endif

#if defined(CONFIG_WORKAROUND_SHARP_TV_AUDIO_NOISE)
/*************************
*	Workaround for sharp TV zapping noise on radio service.
**************************/
		{
			extern HUINT8 gVendorID[2];
			if (mute == TRUE && stAudioOutput.bHdmiMute == FALSE)
			{
				if (gVendorID[0] == 0x4D && gVendorID[1] == 0x10)
				{
					VK_TASK_Sleep(200);
				}
			}
		}
#endif
		stAudioOutput.bHdmiMute = mute;
	}

/*************************
*	2.4. Mute on VCR SCART
*************************/
#if defined(CONFIG_SCART)
	if(ulTarget & DI_AUDIO_OUTPUT_SCART_2)
	{
		dResult = DRV_SCART_SetVCRMute(mute);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_SCART_SetVCRMute failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
		stAudioOutput.bVCRScartMute = mute;
	}
#endif

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
	/* I2S Mute control */
	if(ulTarget & DI_AUDIO_OUTPUT_I2S)
	{
		dResult = P_AUDIO_SetOutputMute(DI_AUDIO_OUTPUT_I2S, mute);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SetOutputMute failed!\n");
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}

		stAudioOutput.bI2sMute = mute;
	}
#endif

	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_RegisterEventCallback(HUINT32 ulDevId, DI_AUDIO_NOTIFY_EVT nEvent, pfnAUDIO_EVT_CALLBACK pfnCallback, HUINT32* ulCallerId)
{//Sunwoo
	HUINT32 	vResult;
	HUINT32		ulId;
	HBOOL		bEmpty;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	/*	Getting ID ************************************
	*	If ulCallerId is 0, It's the first request.
	**************************************************/

	if(*ulCallerId == 0)
	{
		*ulCallerId = P_AUDIO_GetEmptyCallbackIndex();
		if(*ulCallerId == (AUDIO_CALLBACK_OWNER_MAX+2))
		{
			PrintError("There's no empty Callback pointer\n");
			VK_SEM_Release(s_ulAudioDISemId);

			return DI_ERR_ERROR;
		}
	}

	/*	Callback Function Changing ******************************
	*	If ulCallerId isn't NULL it's changing the callback function.
	*	1 is added to callerId when an empty pointer found.
	*	(so actual pointer location is ID-1)
	*********************************************************/
	ulId = (*ulCallerId)-1;

	stAudioCallbacks[ulId].audioEventCallback[ulDevId][nEvent] = *pfnCallback;
	bEmpty = P_AUDIO_CheckEmptyCallbackIndex(ulId);
	if(bEmpty)
	{
		stAudioCallbacks[ulId].bOccupied = TRUE;
	}

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_StartMemoryStream(HUINT32 ulRequestId, DI_AUDIO_FORMAT dType, void *dMemBuf, HULONG dMemBufSize , HUINT32 dLoopCount, DI_AUDIO_OUTPUT output)
{//Sunwoo
	DI_AUDIO_STREAM_MSG 					audMsg;
	DRV_Error								dResult;
	NEXUS_Error 							nResult;
	NEXUS_AudioPlaybackStartSettings		playbackSettings;
	HINT32									vResult;
	HUINT32									ulTaskIndex=DI_AUDIO_TASK_BUSY;
	NEXUS_PlaypumpSettings					playpumpSettings;
	HUINT32									ulPoolIndex=0;
	DI_AUDIO_DECODER_TYPE 					eDecoder=DI_AUDIO_DECODER_MAIN;
	HUINT32									ulLimit=0;
/*///////////Debug Message///////////*/

	DI_UART_Print("[%s].%d\n",__FUNCTION__,__LINE__);
	DI_UART_Print("HUINT32 ulRequestId: %d, DI_AUDIO_FORMAT dType: %d HULONG dMemBufSize: %d HUINT32 dLoopCount: %d\n",
					ulRequestId, dType, dMemBufSize, dLoopCount);

/*///////////////////////////////////*/

	VK_MEM_Memset(&audMsg,0x00,sizeof(DI_AUDIO_STREAM_MSG));

	if(output > DI_AUDIO_OUTPUT_HDMI)
	{
		PrintError("Invalid output value!\n");
		return DI_ERR_ERROR;
	}

	if (dMemBufSize <= 44)
	{
		PrintError("Invalid buffer size (%d)!\n", dMemBufSize);
		return DI_ERR_ERROR;
	}

	/*	1. Task & Pool Registration **********
	*	Because the communication through MSG can make a kind of delay,
	*	If two start functions are called almost simultaneously,
	*	It could fail to get a task index though it is empty.
	***********************************************/
	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	while(1)
	{
		if((stMemoryStreamPool[0].bOccupied == TRUE && stMemoryStreamPool[0].ulRequestId == ulRequestId) ||
		  (stMemoryStreamPool[1].bOccupied == TRUE && stMemoryStreamPool[1].ulRequestId == ulRequestId))
		{
			if(ulLimit <5)
			{
				ulLimit++;
				PrintDebug("%d ms\n", ulLimit*10);
				VK_TASK_Sleep(10);
			}
			else
			{
				PrintError("RequestId: %d is already running! RequestId should be unique!\n", ulRequestId);
				VK_SEM_Release(s_ulAudioDISemId);
				return DI_ERR_ERROR;
			}
		}
		else
		{
			break;
		}
	}

	ulTaskIndex = P_AUDIO_FindFreeTaskId();
	if(ulTaskIndex == DI_AUDIO_TASK_BUSY)
	{
		PrintError("There's no free task!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}
	stAudioTaskInfo[ulTaskIndex].bOccupied = TRUE;

	ulPoolIndex = P_AUDIO_FindFreeMemStreamId();
	if(ulPoolIndex == DI_AUDIO_MEMPOOL_BUSY)
	{
		PrintError("There's no free Memory pool!\n");
		stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}
	stMemoryStreamPool[ulPoolIndex].ulRequestId = ulRequestId;
	stMemoryStreamPool[ulPoolIndex].ulTaskIndex = ulTaskIndex;
	stMemoryStreamPool[ulPoolIndex].bOccupied = TRUE;

	if(gTimeTicket > 99)
	{
		gTimeTicket = 0;
	}
	stMemoryStreamPool[ulPoolIndex].ulTimeTicket = ++gTimeTicket;

	if(stAudioOutput.bDacOutputPathConnected == FALSE)
	{
		dResult = P_AUDIO_AddDacOutputPath(DI_AUDIO_DECODER_MAIN);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddDacOutputPath failed!\n");
			goto not_remove_path;
		}
	}

	if(stAudioOutput.bSpdifOutputPathConnected == FALSE)
	{
#if defined(CONFIG_DI10)
		stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_PCM;
#else
		stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_DOWNMIX;
#endif
		dResult = P_AUDIO_AddSpdifOutputPath(DI_AUDIO_DECODER_MAIN, dType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddSpdifOutputPath failed!\n");
			goto dac_remove;
		}
	}
	if(stAudioOutput.bHdmiOutputPathConnected == FALSE)
	{
		stAudioOutput.eHdmiMode = DI_AUDOUT_HDMI_DOWNMIX;

		dResult = P_AUDIO_AddHdmiOutputPath(DI_AUDIO_DECODER_MAIN, dType);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddHdmiOutputPath failed!\n");
			goto spdif_remove;
		}
	}
#if defined(CONFIG_BLUETOOTH)
	if(stAudioOutput.bBtCaptureOutputPathConnected == FALSE)
	{
		dResult = P_AUDIO_AddBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddBluetoothPath be failed!\n");
			goto hdmi_remove;
		}
	}
#endif

	stAudioTaskInfo[ulTaskIndex].pbuffStartPoint = DD_MEM_Alloc(dMemBufSize);
	if (stAudioTaskInfo[ulTaskIndex].pbuffStartPoint == NULL)
	{
		PrintError("DI_AUDIO_StartMemoryStream malloc fail!\n");
		goto out;
	}

	VK_MEM_Memcpy(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint, dMemBuf, dMemBufSize);

	/*	2. PCM reqeust **********
	*	If the type is PCM, use the audioPlayback.
	*	PCM dependent information locate in the stAudioPlayback.
	***********************************************************/
	if(dType == DI_AUDFORMAT_PCM)
	{
		NEXUS_AudioPlayback_GetDefaultStartSettings(&playbackSettings);
		playbackSettings.sampleRate = AUDIO_DEFAULT_SAMPLERATE;
		playbackSettings.bitsPerSample = AUDIO_DEFAULT_BITSPERSAMPLE;
		dResult = P_AUDIO_GetPCMHeaderInfo(ulTaskIndex, &playbackSettings.sampleRate,
											&playbackSettings.bitsPerSample, &playbackSettings.stereo, dMemBufSize);
		if(dResult != DRV_OK)
		{
			PrintError("PCM Header Parsing Error!\n");
			goto out;
		}
		memmove(stAudioTaskInfo[ulTaskIndex].pbuffStartPoint, stAudioTaskInfo[ulTaskIndex].pbuffStartPoint+44, dMemBufSize-44);
		dMemBufSize -= 44;

		playbackSettings.signedData = TRUE;
		playbackSettings.dataCallback.callback = NULL;
		playbackSettings.dataCallback.context = NULL;
		playbackSettings.dataCallback.param = 0;

		nResult = NEXUS_AudioPlayback_Start(stAudioPlayback[ulTaskIndex].gPlayback, &playbackSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioPlayback_Start failed!\n");
			goto out;
		}
		stAudioPlayback[ulTaskIndex].bStart = TRUE;

		stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize = dMemBufSize;
		stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount = dLoopCount;
	}

	/*	3. None-PCM request **********
	*	If the type is none-pcm, use the playpump
	***************************************/
	else
	{
		/*	3.1. Decoder Select **********
		*	Find a free decoder.
		******************************/
		if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_STOP)
		{
			PrintDebug("[%s].%d Select Main decoder !\n",__FUNCTION__,__LINE__);
			eDecoder = DI_AUDIO_DECODER_MAIN;
		}
		else if(stAudioStatus[DI_AUDIO_DECODER_SUB].nState == DI_AUDIO_STOP)
		{
			PrintDebug("[%s].%d Select Sub decoder !\n",__FUNCTION__,__LINE__);
			eDecoder = DI_AUDIO_DECODER_SUB;
		}
		else
		{
			PrintError("There's not enough decoder to start memory stream, Stop at least more than one decoder!\n");
			goto out;
		}
		stMemoryStreamPool[ulPoolIndex].eDecoder = eDecoder;
		stAudioStatus[eDecoder].nFormat = dType;

		/*	3.2. Setting Playpump & Msg **********
		**************************************/
#if defined (CONFIG_PLAYBACK)
		if(eDecoder == DI_AUDIO_DECODER_MAIN)
		{
			DRV_PVR_GetPlaypumpHandle(&stAudioPlaypump[ulTaskIndex].gPlaypump);
		}
		else if(eDecoder == DI_AUDIO_DECODER_SUB)
		{
			DRV_PVR_GetPlaypumpHandle(&stAudioPlaypump[ulTaskIndex].gPlaypump);
		}
		else
#endif
		{
			PrintError("Wrong Decoder! Only Main, Sub Decoder can be used!\n");
			goto out;
		}

		NEXUS_Playpump_GetDefaultSettings(&playpumpSettings);
		playpumpSettings.transportType = NEXUS_TransportType_eEs;
		playpumpSettings.dataCallback.callback = NULL;
		playpumpSettings.dataCallback.context = NULL;

		nResult = NEXUS_Playpump_SetSettings(stAudioPlaypump[ulTaskIndex].gPlaypump, &playpumpSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Playpump_SetSettings failed!\n");
			goto out;
		}

		stAudioStatus[eDecoder].audioSettings.pidChannel = NEXUS_Playpump_OpenPidChannel(stAudioPlaypump[ulTaskIndex].gPlaypump, 0xC0, NULL);
		stAudioStatus[eDecoder].audioSettings.codec = P_AUDIO_ConvertCodecToNexus(dType);
		stAudioStatus[eDecoder].audioSettings.stcChannel = NULL;

		nResult = NEXUS_Playpump_Start(stAudioPlaypump[ulTaskIndex].gPlaypump);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("Pump Start Failed!\n");
			goto out;
		}
		stAudioPlaypump[ulTaskIndex].bStart = TRUE;

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO) || defined(CONFIG_AUDIO_DEC_FLAC)
		if(dType == DI_AUDFORMAT_FLAC ||
			dType == DI_AUDFORMAT_ADPCM ||
			dType == DI_AUDFORMAT_VORBIS ||
			dType == DI_AUDFORMAT_APE)
		{
			stAudioStatus[eDecoder].audioSettings.maxOutputRate = MAX_OUTPUT_RATE_HRA;
		}
		else
		{
			stAudioStatus[eDecoder].audioSettings.maxOutputRate = MAX_OUTPUT_RATE_NON_HRA;
		}
#endif

		nResult = NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[eDecoder], &stAudioStatus[eDecoder].audioSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_Start Failed!\n");
			goto out;
		}
		stAudioStatus[eDecoder].nState = DI_AUDIO_START;

		stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize = dMemBufSize;
		stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount = dLoopCount;

		/*	3.3 Callback process **********
		*******************************/
		dResult = P_AUDIO_SendEventMessage(eDecoder, DI_AUDIO_MEMORY_STREAM_STARTED);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_SendEventMessage failed!\n");
			goto out;
		}
	}

	/*	4. Sending a message **********
	*	Sending a request message to the allocated task
	********************************************/
	if(dType == DI_AUDFORMAT_PCM)
	{
		audMsg.sRequest = DI_AUDIO_MEMORY_PCM_PLAY;
	}
	else
	{
		audMsg.sRequest = DI_AUDIO_MEMORY_STREAM_PLAY;
	}
	audMsg.pos = 0;
	audMsg.ulTaskIndex = ulTaskIndex;
	audMsg.sLoopCount = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.loopCount;
	audMsg.ulRequestId = ulRequestId;
	audMsg.ulTimeTicket = stMemoryStreamPool[ulPoolIndex].ulTimeTicket;
	audMsg.sMemBuf = stAudioTaskInfo[ulTaskIndex].pbuffStartPoint;
	audMsg.sMemBufSize = stAudioTaskInfo[ulTaskIndex].stAudioStreamInfo.esDataSize;
	audMsg.sDevId = eDecoder;

	vResult = VK_MSG_SendTimeout(stAudioTaskInfo[ulTaskIndex].s_ulAudioStreamMsgId, &audMsg, sizeof(DI_AUDIO_STREAM_MSG), 0);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,vResult);
		goto out;
	}
	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_OK;

out:
#if defined(CONFIG_BLUETOOTH)
	dResult = P_AUDIO_RemoveBluetoothPath();
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
	}
hdmi_remove:
#endif
	dResult = P_AUDIO_RemoveHdmiOutputPath(DI_AUDIO_DECODER_MAIN, dType);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_RemoveHdmiOutputPath faild!\n");
	}
spdif_remove:
	dResult = P_AUDIO_RemoveSpdifOutputPath(DI_AUDIO_DECODER_MAIN, dType);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_RemoveSpdifOutputPath faild!\n");
	}

dac_remove:
	dResult = P_AUDIO_RemoveDacOutputPath(DI_AUDIO_DECODER_MAIN);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_RemoveDacOutputPath faild!\n");
	}

not_remove_path:

	stMemoryStreamPool[ulPoolIndex].bOccupied = FALSE;
	stAudioTaskInfo[ulTaskIndex].bOccupied = FALSE;

	if(dType == DI_AUDFORMAT_PCM)
	{
		stAudioPlayback[ulTaskIndex].bStart = FALSE;
	}
	else
	{
		stAudioPlaypump[ulTaskIndex].bStart = FALSE;
	}
	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_ERROR;
}

DI_ERR_CODE DI_AUDIO_StopMemoryStream(HUINT32 ulRequestId)
{//Sunwoo
	int vResult;
	HUINT32 ulTaskIndex;
	HUINT32 ulPoolIndex;
	DRV_Error dResult;

	/*	1. Task Search **********
	*********************************/
	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if((stMemoryStreamPool[0].bOccupied == TRUE) && (stMemoryStreamPool[0].ulRequestId == ulRequestId))
	{
		ulPoolIndex = 0;
	}
	else if((stMemoryStreamPool[1].bOccupied == TRUE) && (stMemoryStreamPool[1].ulRequestId == ulRequestId))
	{
		ulPoolIndex= 1;
	}
	else
	{
		PrintError("stMemoryStreamPool[0]: %s ulRequestId: %d\n", stMemoryStreamPool[0].bOccupied?"Y":"N", stMemoryStreamPool[0].ulRequestId);
		PrintError("stMemoryStreamPool[1]: %s ulRequestId: %d\n", stMemoryStreamPool[1].bOccupied?"Y":"N", stMemoryStreamPool[1].ulRequestId);
		PrintError("Request Index is invalid! you didn't start memory stream play with requestId %d\n", ulRequestId);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_OK;
	}
	ulTaskIndex = stMemoryStreamPool[ulPoolIndex].ulTaskIndex;

	/*	2. Stop action **********
	*	All the condition checks should be done after getting semaphore!!
	**********************************************************/
	vResult = VK_SEM_Get(s_ulAudioMemStreamId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	if(stAudioPlayback[ulTaskIndex].bStart == TRUE)
	{
		dResult = P_AUDIO_PCMStreamplay_Stop(ulRequestId, ulTaskIndex);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_PCMStreamplay_Stop failed!\n");
			VK_SEM_Release(s_ulAudioMemStreamId);
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}

	if(stAudioPlaypump[ulTaskIndex].bStart == TRUE)
	{
		dResult = P_AUDIO_StopMemoryStream(ulRequestId, ulTaskIndex);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_StopMemoryStream failed!\n");
			VK_SEM_Release(s_ulAudioMemStreamId);
			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}

	VK_SEM_Release(s_ulAudioMemStreamId);
	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_OK;
}


DI_ERR_CODE DI_AUDIO_PauseMemoryStream(HUINT32 ulRequestId, HBOOL pause)
{//Sunwoo
	DRV_Error	dResult;
	HINT32		vResult;
	HUINT32		ulDevId;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if((stMemoryStreamPool[0].ulRequestId == ulRequestId)&&(stMemoryStreamPool[0].bOccupied == TRUE))
	{
		ulDevId = stMemoryStreamPool[0].eDecoder;
	}
	else if((stMemoryStreamPool[1].ulRequestId == ulRequestId)&&(stMemoryStreamPool[1].bOccupied == TRUE))
	{
		ulDevId = stMemoryStreamPool[1].eDecoder;
	}
	else
	{
		PrintError("There's no memorystream with requestId %d\n", ulRequestId);
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	dResult = P_AUDIO_PauseMemoryStream(ulDevId,  pause);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_PauseMemoryStream failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	stAudioStatus[ulDevId].nState = (pause?DI_AUDIO_PAUSED:DI_AUDIO_START);

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_DelayOutput(HUINT32 ulDevId, HINT32 delayMs)
{
	HINT32						vResult;
	DRV_Error					dResult;
	HUINT32 ulCodecDelay;
	HUINT32 i;
	HINT32 setDelayMs;

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("HUINT32 ulDevId: %d HINT32 delayMs: %d\n", ulDevId, delayMs);

/*///////////////////////////////////*/

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

/*	1. delay MS converting
*	MW send down a negative value. Delay amount should be a positive value.
*/
	setDelayMs = abs(delayMs);
	if(setDelayMs > AUDIO_DELAY_MAX)
	{
		PrintError("Out of Delay range! It's only within 400ms\n");
		VK_SEM_Release(s_ulAudioDISemId);

		return DI_ERR_ERROR;
	}

/*	2. Set the delay value
*	NOTICE!!
*	ulDevId is an identifier of decoders.
*	But, SPDIF decoder resource is on DI's hand, so delay setting is handled also under DI.
*	Temporarily, set the delay value to all of the decoders.
*/
	stAudioSyncStatus.nUserOffset = setDelayMs;
	ulCodecDelay = P_AUDIO_GetDefaultDelay(stAudioStatus[ulDevId].nFormat);
	setDelayMs += ulCodecDelay;

	for(i=0; i<=DI_AUDIO_DECODER_SPDIF; i++)
	{
		if(stAudioStatus[i].nState == DI_AUDIO_START)
		{
			dResult = P_AUDIO_DelayDecoder(i, setDelayMs);
			if(dResult != DRV_OK)
			{
				PrintError("P_AUDIO_DelayDecoder failed!\n");
				VK_SEM_Release(s_ulAudioDISemId);

				return DI_ERR_ERROR;
			}
		}
	}

#if 0 /* No more necessary */
/*	3. Restart decoders to adjust the delay value.
*/
	if(stAudioStatus[DI_AUDIO_DECODER_SUB].nState == DI_AUDIO_START)
	{
		dResult = P_AUDIO_RestartDecoder(DI_AUDIO_DECODER_SUB);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RestartDecoder failed!\n");

			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}
	if(stAudioStatus[DI_AUDIO_DECODER_MAIN].nState == DI_AUDIO_START)
	{
		dResult = P_AUDIO_RestartDecoder(DI_AUDIO_DECODER_MAIN);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RestartDecoder failed!\n");

			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}
	if(stAudioStatus[DI_AUDIO_DECODER_SPDIF].nState == DI_AUDIO_START)
	{
		dResult = P_AUDIO_RestartDecoder(DI_AUDIO_DECODER_SPDIF);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RestartDecoder failed!\n");

			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}
#endif
	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}


DI_ERR_CODE DI_AUDIO_GetCapability(HUINT32 *pucNumOfDevice , DI_AUDIO_CAP_t **ppCapInfo)
{//Sunwoo
	HINT32						vResult;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}
#if defined(CONFIG_DI20)
	*pucNumOfDevice = AUDIO_ALL_DECODERS-1; /* fix the  real decoder number=2 except passthru spdif decoder */
#else
	*pucNumOfDevice = AUDIO_ALL_DECODERS;
#endif
	*ppCapInfo = &g_AudioCap[DI_AUDIO_DECODER_MAIN];

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_GetState(HUINT32 ulDevId, DI_AUDIO_RUNNING_STATE *pnAudioState)
{//Sunwoo
	HINT32						vResult;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

	// 1. 메인 오디오의 상태가 start(enabled) 와 pause인 경우만 ok
	if((stAudioStatus[ulDevId].nState == DI_AUDIO_START) || (stAudioStatus[ulDevId].nState == DI_AUDIO_PAUSED))
	{
		*pnAudioState = stAudioStatus[ulDevId].rState.state;//gState[ulDevId];
	}
	else
	{
		PrintDebug("[DI_AUDIO_GetState] No Activated Audio Decoder\n");
		VK_SEM_Release(s_ulAudioDISemId);

		return DI_ERR_ERROR;
	}
	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_SetEventTimeOut(HUINT32 ulDevId, HUINT32 timeout)
{//Sunwoo
	HUINT32 		timeval;
	HINT32 			vResult;

/*///////////Debug Message///////////*/

	PrintDebug("[%s].%d\n",__FUNCTION__,__LINE__);
	PrintDebug("HUINT32 ulDevId: %d HUINT32 timeout: %d\n", ulDevId, timeout);

/*///////////////////////////////////*/


	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if(timeout < DI_AUDIO_UNDERRUN_TIMEOUT_DEFAULT)
	{
		stAudioStatus[ulDevId].eventLimit = DI_AUDIO_UNDERRUN_TIMEOUT_DEFAULT;
		PrintDebug("Timeout is too short, Default timeout value is set\n");

		VK_SEM_Release(s_ulAudioDISemId);

		return DI_ERR_OK;
	}
	timeval = (timeout/DI_AUDIO_UNDERRUN_TIMEOUT_RES);
	timeval += (timeout%DI_AUDIO_UNDERRUN_TIMEOUT_RES)?1:0;
	stAudioStatus[ulDevId].eventLimit= timeval;
	//temp
	stAudioStatus[DI_AUDIO_DECODER_SUB].eventLimit = timeval;
	stAudioStatus[DI_AUDIO_DECODER_SPDIF].eventLimit = timeval;

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_SetSCMS(DI_AUDIO_COPYRIGHT_MODE mode)
{//Sunwoo
	NEXUS_SpdifOutputSettings 	lSettings;
	NEXUS_Error 				nResult;
	HINT32 						vResult;
#if 0
	DRV_Error					dResult;
#endif
	HUINT32						ulDevId;


	/*	SCMS setting is available only for downmixing */
	ulDevId = DI_AUDIO_DECODER_MAIN;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	NEXUS_SpdifOutput_GetSettings(stAudioOutput.spdifOutputs[stAudioOutput.spdifOutPortNum], &lSettings);

	/*	SCMS Category code [8-15] bits ***********
	*	Japan
	*	- No Restriction: 001 0000L (L:X)
	*	- One Copy: 001 0000L (L: 0)
	*	- No Copy: 001 0000L (L: 1)
	*	Europe
	*	- No Restriction: 001 1000L (L:X)
	*	- One Copy: 100 1100L (L:0)
	*	- No Copy: 100 1100L (L:1)
	*******************************************/

#if defined(CONFIG_JAPAN_SPEC)
	if(mode == DI_AUDOUT_COPYRIGHT_NO_RESTRICTION)
	{
		lSettings.channelStatusInfo.swCopyRight = FALSE;
		lSettings.channelStatusInfo.categoryCode = 4;	/* 0 0000 100 */
	}
	else if(mode == DI_AUDOUT_COPYRIGHT_ONE_COPY)
	{
		lSettings.channelStatusInfo.swCopyRight = TRUE;
		lSettings.channelStatusInfo.categoryCode = 4;	/* 0 0000 100 */
	}
	else if(mode == DI_AUDOUT_COPYRIGHT_NO_COPY)
	{
		lSettings.channelStatusInfo.swCopyRight = TRUE;
		lSettings.channelStatusInfo.categoryCode = 132;	/* 1 0000 100 */
	}

#else  /*europe*/
	if(mode == DI_AUDOUT_COPYRIGHT_NO_RESTRICTION)			/* 8~14bits are inserted inversely */
	{
		lSettings.channelStatusInfo.swCopyRight = FALSE;
		lSettings.channelStatusInfo.categoryCode = 0x0c;	/* 0 0001 100 */
	}
	else if(mode == DI_AUDOUT_COPYRIGHT_ONE_COPY)
	{
		lSettings.channelStatusInfo.swCopyRight = TRUE;
		lSettings.channelStatusInfo.categoryCode = 0x19;	/* 0 0011 001 */
	}
	else if(mode == DI_AUDOUT_COPYRIGHT_NO_COPY)
	{
		lSettings.channelStatusInfo.swCopyRight = TRUE;
		lSettings.channelStatusInfo.categoryCode = 0x99;	/* 1 0011 001 */
	}

#endif

	nResult = NEXUS_SpdifOutput_SetSettings(stAudioOutput.spdifOutputs[stAudioOutput.spdifOutPortNum], &lSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_SpdifOutput_SetSettings failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);

		return DI_ERR_ERROR;
	}

	/* While Decoder is running, Changing of output settings should be possible
		 so block below code */
#if 0
	if(stAudioStatus[ulDevId].nState == DI_AUDIO_START)
	{
		dResult = P_AUDIO_RestartDecoder(ulDevId);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RestartDecoder failed!\n");

			VK_SEM_Release(s_ulAudioDISemId);
			return DI_ERR_ERROR;
		}
	}
#endif

	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;

}

DI_ERR_CODE DI_AUDIO_GetStreamInfo      (HUINT32 ulDevId, DI_AUDIO_STREAM_INFO_t *ptAudioStreamInfo)
{
	HINT32						vResult;
	DRV_Error 					drvResult;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	drvResult = DRV_AUDIO_GetStreamInfo(ulDevId, ptAudioStreamInfo);
	if(drvResult != DRV_OK)
	{
		PrintError("DRV_AUDIO_GetStreamInfo failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_OK;
}

#if defined(CONFIG_DI10)
DI_ERR_CODE DI_AUDIO_SetTranscoding(DI_AUDIO_FORMAT outType)
{
	PrintDebug("[%s].%d format %d \n",__FUNCTION__,__LINE__, outType);

	stAudioOutput.stTranscodingInfo.bHdmiTranscoding = TRUE;
	stAudioOutput.stTranscodingInfo.bSpdifTranscoding = TRUE;
	stAudioOutput.stTranscodingInfo.eHdmiTransFormat = outType;
	stAudioOutput.stTranscodingInfo.eSpdifTransFormat = outType;

	return DI_ERR_OK;
}
#else
DI_ERR_CODE DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT eOutputType, HBOOL OnOff, DI_AUDIO_FORMAT outType)
{
	PrintDebug("[%s].%d eOutputType %d OnOff %d format %d \n",__FUNCTION__, __LINE__, eOutputType, OnOff, outType);

	if(eOutputType == DI_AUDIO_OUTPUT_HDMI)
	{
		stAudioOutput.stTranscodingInfo.bHdmiTranscoding = OnOff;
		stAudioOutput.stTranscodingInfo.eHdmiTransFormat = outType;
	}
	else if(eOutputType == DI_AUDIO_OUTPUT_SPDIF)
	{
		stAudioOutput.stTranscodingInfo.bSpdifTranscoding = OnOff;
		stAudioOutput.stTranscodingInfo.eSpdifTransFormat = outType;
	}
	else
	{
		PrintError("DI_AUDIO_SetTranscoding() failed!\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_DI10)
#else
DI_ERR_CODE DI_AUDIO_SetMediaType(HUINT32 ulDevId, DI_AUDIO_TYPE nType)
{//Sunwoo
	if(ulDevId == DI_AUDIO_DECODER_SPDIF)
	{
		return DI_ERR_OK;
	}
	else
	{
		stAudioStatus[ulDevId].nType = nType;
		PrintDebug("[%s] ulDevId = %d, nType = %d!\n", __FUNCTION__, ulDevId, nType);
		if(ulDevId == DI_AUDIO_DECODER_MAIN)
		{
			stAudioStatus[DI_AUDIO_DECODER_SPDIF].nType = nType;
			PrintDebug("[%s] ulDevId = %d, nType = %d!\n", __FUNCTION__, DI_AUDIO_DECODER_SPDIF, nType);
		}
	}

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_AUDIO_SetSync(HBOOL bOnOff)
{
	DRV_Error					dResult;
	HINT32						vResult;
	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	dResult = P_AUDIO_SetTsmMode(bOnOff);
	if(dResult != DRV_OK)
	{
		PrintError("P_AUDIO_SetTsmMode failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_OK;
}

#if defined(CONFIG_DI20)
DI_ERR_CODE DI_AUDIO_GetOutputNum(DI_AUDIO_OUTPUT eType, HUINT8* ucNum)
{
	DI_ERR_CODE rc=DI_ERR_OK;

	switch(eType)
	{
		case DI_AUDIO_OUTPUT_HDMI:
			*ucNum = DI_AUDIO_HDMI_OUTPUTS;
			break;

		case DI_AUDIO_OUTPUT_SPDIF:
			*ucNum = DI_AUDIO_SPDIF_OUTPUTS;
			break;

		case DI_AUDIO_OUTPUT_DAC:
			*ucNum = DI_AUDIO_RCA_OUTPUTS;
			break;

		case DI_AUDIO_OUTPUT_ALL:
			*ucNum = DI_AUDIO_HDMI_OUTPUTS+DI_AUDIO_SPDIF_OUTPUTS+DI_AUDIO_RCA_OUTPUTS;
			break;

		default:
			*ucNum = 0;
			rc = DI_ERR_ERROR;
	}

	return rc;
}
#endif
DI_ERR_CODE DI_AUDIO_SetDecoderVolume(HUINT32 ulDevId, HUINT32 ulDecoderVolume)
{
	HINT32						vResult;
	DRV_Error 					drvResult;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	drvResult = DRV_AUDIO_SetDecoderVolume(ulDevId, ulDecoderVolume);
	if(drvResult != DRV_OK)
	{
		PrintError("DRV_AUDIO_GetStreamInfo failed!\n");
		VK_SEM_Release(s_ulAudioDISemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulAudioDISemId);
	return DI_ERR_OK;
}
#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO) || defined(CONFIG_AUDIO_DEC_FLAC)
DI_ERR_CODE DI_AUDIO_GetHRAOutputSampleRate(DI_AUDIO_SAMPLE_RATE_e eInput, DI_AUDIO_SAMPLE_RATE_e *eOutput)
{
	return P_AUDIO_GetHRAOutputSampleRate(eInput, eOutput);
}

DI_ERR_CODE DI_AUDIO_GetOutputSampleRate(DI_AUDIO_SAMPLE_RATE_e eInput, DI_AUDIO_SAMPLE_RATE_e *eOutput)
{
	HINT32						vResult;
	DI_ERR_CODE 				diResult = DI_ERR_OK;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	if(stAudioStatus[DI_AUDIO_DECODER_MAIN].audioSettings.maxOutputRate == MAX_OUTPUT_RATE_HRA)
	{
		diResult = P_AUDIO_GetHRAOutputSampleRate(eInput, eOutput);
	}
	else
	{
		switch(eInput)
		{
			case eDI_AUDIO_SR_32KHz:
				*eOutput = eDI_AUDIO_SR_32KHz;
				break;

			case eDI_AUDIO_SR_44KHz:
				*eOutput = eDI_AUDIO_SR_44KHz;
				break;

			case eDI_AUDIO_SR_48KHz:
				*eOutput = eDI_AUDIO_SR_48KHz;
				break;

			case eDI_AUDIO_SR_88KHz:
				*eOutput = eDI_AUDIO_SR_44KHz;
				break;

			case eDI_AUDIO_SR_96KHz:
				*eOutput = eDI_AUDIO_SR_48KHz;
				break;

			case eDI_AUDIO_SR_176KHz:
				*eOutput = eDI_AUDIO_SR_44KHz;
				break;

			case eDI_AUDIO_SR_192KHz:
				*eOutput = eDI_AUDIO_SR_48KHz;
				break;

			default :
				PrintError("[%s] ERROR : input sample rate is wroung ! %d \n", __FUNCTION__, eInput);
				*eOutput = eDI_AUDIO_SR_MAX;
				diResult = DI_ERR_ERROR;
				break;
		}
	}

	VK_SEM_Release(s_ulAudioDISemId);

ret :
	return DI_ERR_OK;
}
#endif
DRV_Error DRV_AUDIO_SetDecoderVolume(HUINT32 ulDevId, HUINT32 ulDecoderVolume)
{
	HUINT32 ulVolume;
	NEXUS_Error nResult = NEXUS_SUCCESS;
	NEXUS_AudioDecoderSettings nSettings;
	DRV_Error dResult = DRV_OK;
	int nVkRet,i=0;

	PrintEnter();

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(ulDecoderVolume > AUDIO_LEVEL_PERCENT_MAX)
	{
		PrintError("Audio Decoder Volume value is out of range, Parameter was %d\n", ulDecoderVolume);
		ulVolume = AUDIO_LEVEL_PERCENT_MAX;
	}
	else
	{
		ulVolume = ulDecoderVolume;
	}

	NEXUS_AudioDecoder_GetSettings(stAudioInstance.audioDecoder[ulDevId], &nSettings);
	for(i=0;i<NEXUS_AudioChannel_eMax;i++)
	{
		nSettings.volumeMatrix[i][i] = AUDIO_LEVEL_LINEAR_UNIT*ulVolume;
	}
	nResult=NEXUS_AudioDecoder_SetSettings(stAudioInstance.audioDecoder[ulDevId], &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_SetSettings Failed!\n");
		dResult = DRV_ERR;
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	PrintExit();
	return dResult;

}

#if defined(CONFIG_HDMI_IN)
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
DRV_Error DRV_AUDIO_InitAudioInputCapture(HUINT32 ulDevId)
{
    DRV_Error nDrvErr = DRV_OK;

    stAudioInstance.audioInputCapture[ulDevId] = NEXUS_AudioInputCapture_Open(ulDevId, NULL);
    if(stAudioInstance.audioInputCapture[ulDevId] == NULL)
    {
        PrintError("[%s] %d Error NEXUS_AudioInputCapture_Open!\n", __FUNCTION__, __LINE__);
        return DRV_ERR;
    }

    return nDrvErr;
}
#endif

DRV_Error DRV_AUDIO_SetHdmiAudioInputToOutput(HUINT32 ulDevId, NEXUS_HdmiInputHandle hdmiInput)
{
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
    NEXUS_AudioInputCaptureStartSettings hAudioInputCaptureStartSettings;
    NEXUS_AudioInput hAudioInput;
    NEXUS_HdmiOutputHandle hHdmiOutputHandle;
    NEXUS_Error nResult = NEXUS_SUCCESS;
    DRV_Error nDrvErr = DRV_OK;

    NEXUS_AudioInputCapture_GetDefaultStartSettings(&hAudioInputCaptureStartSettings);
    hAudioInputCaptureStartSettings.input = NEXUS_HdmiInput_GetAudioConnector(hdmiInput);

    nDrvErr = DRV_HDMI_GetHdmiOutputHandle(&hHdmiOutputHandle);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_HDMI_GetHdmiOutputHandle error %d \n", __FILE__, nDrvErr);
		return nDrvErr;
	}

    nResult = NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hHdmiOutputHandle));
    if(nResult != NEXUS_SUCCESS)
    {
    	PrintError("NEXUS_AudioOutput_RemoveAllInputs failed!\n");
    	return DRV_ERR;
    }

    hAudioInput = NEXUS_AudioInputCapture_GetConnector(stAudioInstance.audioInputCapture[ulDevId]);

    nResult = NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hHdmiOutputHandle), hAudioInput);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioOutput_AddInput failed!\n");
		return DRV_ERR;
	}

    nResult = NEXUS_AudioInputCapture_Start(stAudioInstance.audioInputCapture[ulDevId], &hAudioInputCaptureStartSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioInputCapture_Start failed!\n");
		return DRV_ERR;
	}

    return nDrvErr;
#else
	NEXUS_AudioDecoderStartSettings audioStartSettings;
	DRV_Error nDrvErr = DRV_OK;

	NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[ulDevId]);

	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioStartSettings);
	audioStartSettings.input = NEXUS_HdmiInput_GetAudioConnector(hdmiInput);

	NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[ulDevId], &audioStartSettings);

	return nDrvErr;
#endif
}

DRV_Error DRV_AUDIO_UnsetHdmiAudioInputToOutput(HUINT32 ulDevId)
{
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
    NEXUS_HdmiOutputHandle hHdmiOutputHandle;
    NEXUS_Error nResult = NEXUS_SUCCESS;
    DRV_Error nDrvErr = DRV_OK;

    nDrvErr = DRV_HDMI_GetHdmiOutputHandle(&hHdmiOutputHandle);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_HDMI_GetHdmiOutputHandle error %d \n", __FILE__, nDrvErr);
		return nDrvErr;
	}

    NEXUS_AudioInputCapture_Stop(stAudioInstance.audioInputCapture[ulDevId]);

    nResult = NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hHdmiOutputHandle));
    if(nResult != NEXUS_SUCCESS)
    {
    	PrintError("NEXUS_AudioOutput_RemoveAllInputs failed!\n");
    	return DRV_ERR;
    }

    return nDrvErr;
#else
	DRV_Error nDrvErr = DRV_OK;

	NEXUS_AudioDecoder_Stop(stAudioInstance.audioDecoder[ulDevId]);
	NEXUS_AudioDecoder_Start(stAudioInstance.audioDecoder[ulDevId], &stAudioStatus[ulDevId].audioSettings);

	return nDrvErr;
#endif
}
#endif

#if defined(CONFIG_MIC)
DRV_Error DRV_AUDIO_ConnectMicToMixer(NEXUS_AudioPlaybackHandle micPlayback)
{
	NEXUS_AudioInput input;
    	NEXUS_Error nResult = NEXUS_SUCCESS;
	DRV_Error dResult=DRV_OK;
	int nVkRet;

	PrintEnter();

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	input = NEXUS_AudioPlayback_GetConnector(micPlayback);
	nResult = NEXUS_AudioMixer_AddInput(stAudioInstance.audioMixer, input);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d Error NEXUS_AudioMixer_AddInput!\n", __FUNCTION__, __LINE__);
		dResult=DRV_ERR;
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	PrintExit();
	return dResult;

}

DRV_Error DRV_AUDIO_DisconnectMicToMixer(NEXUS_AudioPlaybackHandle micPlayback)
{
	NEXUS_AudioInput input;
	DRV_Error dResult=DRV_OK;
    	NEXUS_Error nResult=NEXUS_SUCCESS;
	int nVkRet;

	PrintEnter();

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	input = NEXUS_AudioPlayback_GetConnector(micPlayback);
	nResult = NEXUS_AudioMixer_RemoveInput(stAudioInstance.audioMixer, input);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d Error NEXUS_AudioMixer_RemoveInput!\n", __FUNCTION__, __LINE__);
		dResult=DRV_ERR;
	}

	VK_SEM_Release(s_ulAudioDRVSemId);
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_AUDIO_SetAllOutputPCM(void)
{
	DRV_Error dResult=DRV_OK;
	int nVkRet=VK_OK;

	PrintEnter();

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* During to use MIC, not disconnect Output Path as a PCM */
	stAudioOutput.bForceOutputPath = TRUE;

	if(stAudioOutput.bDacOutputPathConnected == FALSE)
	{
		dResult = P_AUDIO_AddDacOutputPath(DI_AUDIO_DECODER_MAIN);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddDacOutputPath failed!\n");
		}
	}

	if(stAudioOutput.bSpdifOutputPathConnected == FALSE)
	{
#if defined(CONFIG_DI10)
		stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_PCM;
#else
		stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_DOWNMIX;
#endif
		dResult = P_AUDIO_AddSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddDigitalOutputPath failed!\n");
		}
	}

	if(stAudioOutput.bHdmiOutputPathConnected == FALSE)
	{
		stAudioOutput.eHdmiMode = DI_AUDOUT_HDMI_DOWNMIX;
		dResult = P_AUDIO_AddHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddDigitalOutputPath failed!\n");
		}
	}

#if defined(CONFIG_BLUETOOTH)
	if(stAudioOutput.bBtCaptureOutputPathConnected == FALSE)
	{
		dResult = P_AUDIO_AddBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_AddBluetoothPath be failed!\n");
		}
	}
#endif
	VK_SEM_Release(s_ulAudioDRVSemId);

	PrintExit();

	return dResult;
}

DRV_Error DRV_AUDIO_UnsetAllOutputPCM(void)
{
	DRV_Error dResult=DRV_OK;
	int nVkRet=VK_OK;

	PrintEnter();

	nVkRet = VK_SEM_Get(s_ulAudioDRVSemId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] ERROR : VK_SEM_Get failed\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* During to use MIC, not disconnect Output Path as a PCM */
	stAudioOutput.bForceOutputPath = FALSE;

	if(stAudioStatus[0].bDecoding == TRUE) /* MAIN Decoder */
	{
		PrintError(" Main Decoder is still working not remove output path !");
		VK_SEM_Release(s_ulAudioDRVSemId);
		return DRV_OK;
	}

	if(stAudioOutput.bDacOutputPathConnected == TRUE)
	{
		dResult = P_AUDIO_RemoveDacOutputPath(DI_AUDIO_DECODER_MAIN);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDacOutputPath failed!\n");
		}
	}

	if(stAudioOutput.bSpdifOutputPathConnected == TRUE)
	{
#if defined(CONFIG_DI10)
		stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_PCM;
#else
		stAudioOutput.eSpdifMode = DI_AUDOUT_SPDIF_DOWNMIX;
#endif
		dResult = P_AUDIO_RemoveSpdifOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
		}
	}

	if(stAudioOutput.bHdmiOutputPathConnected == TRUE)
	{
		stAudioOutput.eHdmiMode = DI_AUDOUT_HDMI_DOWNMIX;
		dResult = P_AUDIO_RemoveHdmiOutputPath(DI_AUDIO_DECODER_MAIN, DI_AUDFORMAT_PCM);
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveDigitalOutputPath failed!\n");
		}
	}

#if defined(CONFIG_BLUETOOTH)
	if(stAudioOutput.bBtCaptureOutputPathConnected == TRUE)
	{
		dResult = P_AUDIO_RemoveBluetoothPath();
		if(dResult != DRV_OK)
		{
			PrintError("P_AUDIO_RemoveBluetoothPath be failed!\n");
		}
	}
#endif
	VK_SEM_Release(s_ulAudioDRVSemId);

	PrintExit();

	return dResult;

}
#endif /* CONFIG_MIC */

DRV_Error DRV_AUDIO_GetMixerHandle(NEXUS_AudioMixerHandle *hMixer)
{
	if(stAudioInstance.audioMixer == NULL)
	{
		PrintError("Audio Mixer is not opened!");
		*hMixer = NULL;
		return DRV_ERR;
	}

	*hMixer = stAudioInstance.audioMixer;

	return DRV_OK;
}

DRV_Error DRV_AUDIO_GetPlaybackHandle(HUINT32 ulDeviceId, NEXUS_AudioPlaybackHandle *hDecoder)
{

	*hDecoder = stAudioPlayback[ulDeviceId].gPlayback;

	return DRV_OK;
}

DI_ERR_CODE DI_AUDIO_GetPlaybackHandle(HUINT32 ulDeviceId,void *handle)
{

	DRV_AUDIO_GetPlaybackHandle(ulDeviceId,(NEXUS_AudioPlaybackHandle*)handle);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_GetDecoderHandle(HUINT32 ulDeviceId,void *handle)
{

	DRV_AUDIO_GetDecoderHandle(ulDeviceId,(NEXUS_AudioDecoderHandle*)handle);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_GetMixerHandle(void *handle)
{
	DRV_Error nDrvErr;

	if(handle == NULL)
	{
		PrintError("Invalid Parmeter!\n");
		return DI_ERR_INVALID_PARAM;
	}

	nDrvErr = DRV_AUDIO_GetMixerHandle((NEXUS_AudioMixerHandle *)handle);
	if (nDrvErr != DRV_OK)
	{
		PrintError("error DRV_AUDIO_GetMixerHandle!\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_AUDIO_SetAllPath(HUINT32 ulDeviceId,HBOOL bOnOff)
{
	HINT32	vResult;

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if(bOnOff == TRUE)
	{
		DRV_AUDIO_AddDacOutputPath(ulDeviceId);
		DRV_AUDIO_AddHdmiOutputPath(ulDeviceId,DI_AUDFORMAT_NONE);
		DRV_AUDIO_AddSpdifOutputPath(ulDeviceId,DI_AUDFORMAT_NONE);
	}
	else
	{
		DRV_AUDIO_RemoveDacOutputPath(ulDeviceId);
		DRV_AUDIO_RemoveHdmiOutputPath(ulDeviceId,DI_AUDFORMAT_NONE);
		DRV_AUDIO_RemoveSpdifOutputPath(ulDeviceId,DI_AUDFORMAT_NONE);
	}
	VK_SEM_Release(s_ulAudioDISemId);

	return DI_ERR_OK;

}

DI_ERR_CODE DI_AUDIO_GetCurrentDecoderStatus(HUINT32 ulDeviceId, DI_AUDIO_DECODER_STATUS_t *pstAudioDecoderStatus)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	NEXUS_Error unNexusErr;
	NEXUS_AudioDecoderStatus decoderStatus;
	HINT32	vResult;

	PrintEnter(); 

	vResult = VK_SEM_Get(s_ulAudioDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	if(pstAudioDecoderStatus == NULL)
	{
		PrintError("[%s] pstAudioDecoderStatus is NULL\n", __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	unNexusErr = NEXUS_AudioDecoder_GetStatus(stAudioInstance.audioDecoder[ulDeviceId], &decoderStatus);
	if (unNexusErr)
	{
		PrintDebug("[%d]%s err from NEXUS_AudioDecoder_GetStatus",__LINE__, __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}
	pstAudioDecoderStatus->ulDepth = decoderStatus.fifoDepth;
	pstAudioDecoderStatus->ulTotalSize = decoderStatus.fifoSize;
	
done:
	VK_SEM_Release(s_ulAudioDISemId);
	PrintExit();

	return rc;
}


