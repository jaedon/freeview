#if defined(CONFIG_VK_STDLIB)
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <fcntl.h>
#include <unistd.h>
#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_media.h"
#include "di_calypso.h"
#include "drv_calypso.h"
#include "drv_demux.h"
#include "drv_audio.h"
#include "drv_video.h"
#include "drv_pvr.h"
//#include "drv_drm.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_video_decoder_types.h"
#include "drv_nvram.h"
#include "drv_crypt.h"
#include "crc.h"

#define DI_CSO_DEBUG

#ifdef DI_CSO_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		DI_UART_Print
#endif

#ifdef DI_CSO_DEBUG
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter
#define PrintExit
#endif

#define UNUSED_PARAM(x)  ((void)x)

#define IP_NETWORK_JITTER 300	/* for Live Multicast */
#define EOF_DETECT_THRESHOLD  4

typedef void (*DI_CSOFakeCSOCallback_t)	(HUINT32 VcodecId, HUINT32 AcodecId);

typedef struct
{
	HUINT32 ulPlaypumpIndex;
	HBOOL bUsed;
} DI_CSO_PumpInfo_t;

typedef struct
{
	HBOOL bDirection;
	HUINT64 ullPreviousPosition;
	HUINT64 ullDuration;
	HUINT32 uiPreviousCdbDepth;
	HULONG	ulPreviousSystemTick;
	HUINT32 uiEofGauge;
	HUINT32 uiCurrentPlaypumpIndex;
	NEXUS_VideoDecoderHandle hVideoDecoder;
	NEXUS_AudioDecoderHandle hAudioDecoder;
	NEXUS_PidChannelHandle hVideoPidChannel;
	NEXUS_PidChannelHandle hAudioPidChannel;
	NEXUS_PidChannelHandle hPcrPidChannel;
	HUINT32 unVideoPidOfPidChannel;
	HUINT32 unAudioPidOfPidChannel;
	HUINT32 unVideoStreamId; /* Current Track setting Video Pid */
	HUINT32 unAudioStreamId; /* Current Track setting Audio Pid */
	HUINT32 unVideoCodecId;
	HUINT32 unAudioCodecId;
	HUINT32 nIdxVideoDecoder;
	HUINT32 nIdxAudioDecoder;
	HUINT64 ullFirstPosition;
	DI_MEDIA_PlaySpeed_e eSpeed;
	HUINT32 ulDemuxId;
	DI_CSO_PumpInfo_t stPumpInfo[DI_CSO_PUMPTYPE_ENDOFCASE];
	DI_CSO_InputStreamType_e eInputStreamType;
} DI_CSO_Instance;

//static HBOOL g_bProbeFailure;

static CSO_HANDLE g_pCurrentSession = NULL; /* for Live Multicast */

#define DI_CSO_MAX_VIEW_NUM 2

/* Cso Pool: Support Dual Decoding(Multi View) */
static DI_CSO_Instance *hCsoPool[DI_CSO_MAX_VIEW_NUM] = {NULL,};
static HUINT32 s_ulCsoStart = 0;

#if __________p__________
#endif

static void P_CSO_remove_common_factors(unsigned val1, unsigned val2, unsigned *pResult1, unsigned *pResult2)
{
	static const unsigned factors[] = {5, 3};
	static const unsigned numFactors = sizeof(factors)/sizeof(unsigned);
	unsigned factor, index;

	/* Remove factors of 2 first to use AND/shift operations instead of div/mod */
	while ( val1 >= 2 && val2 >= 2 )
	{
		if ( (val1 & 1) || (val2 & 1) )
		{
			break;
		}

		val1 >>= 1;
		val2 >>= 1;
	}

	/* Now go through remaining factors */
	for ( index = 0; index < numFactors; index++ )
	{
		factor = factors[index];
		while ( val1 >= factor && val2 >= factor )
		{
			if ( (val1 % factor) || (val2 % factor) )
			{
				break;
			}

			val1 /= factor;
			val2 /= factor;
		}
	}

	*pResult1 = val1;
	*pResult2 = val2;
}

static int P_CSO_GetDecodeRate(DI_MEDIA_PlaySpeed_e eSpeed)
{
	int iSpeed=0;

	if(eSpeed == eDI_MEDIA_Speed_Pause)
	{
		iSpeed = 0;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X_1_10)
	{
		iSpeed = 100;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X_1_8)
	{
		iSpeed = 125;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X_1_4)
	{
		iSpeed = 250;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X_1_2)
	{
		iSpeed = 500;
	}
	else if(eSpeed == eDI_MEDIA_Speed_Normal)
	{
		iSpeed = 1000;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X12_10)
	{
		iSpeed = 1200;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X2)
	{
		iSpeed = 2000;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X4)
	{
		iSpeed = 4000;
	}
	else if(eSpeed == eDI_MEDIA_Speed_FF_X8)
	{
		iSpeed = 8000;
	}
	else
	{
		PrintError("[%s][%d] invalid eSpeed!\n", __FUNCTION__,__LINE__);
		iSpeed = 0;
	}

	return iSpeed;
}

#if __________drv__________
#endif

DRV_Error DRV_CSO_Init(void)
{
	HINT32 nIdx = 0;

	/* Init Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		hCsoPool[nIdx] = NULL;
	}

	return DRV_OK;
}

DI_ERR_CODE DRV_CSO_Probe (void *pHTuner, HUINT8 *pUrl, DI_CSO_INFO_t *stCsoFileInfo)
{
	pHTuner = pHTuner;
	pUrl = pUrl;
	stCsoFileInfo = stCsoFileInfo;
	return DI_ERR_OK;
}

void DRV_CSO_PrintProbeInfo (DI_CSO_INFO_t *pCsoInfo)
{
	pCsoInfo = pCsoInfo;
	return;
}

DRV_Error DRV_CSO_Term(void)
{
	return DRV_OK;
}

NEXUS_StcChannelHandle DRV_CSO_GetPlaypumpStcChannelHandle(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, DI_MEDIA_PLAYPUMP csoType)
{
	NEXUS_StcChannelSettings stcSettings;
	DI_CSO_Instance *pstCsoInstance = NULL;
	DI_MEDIA_AUDIO_TYPE_e eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_NONE;
	HINT32 nIdx=0;
	NEXUS_Error nResult;

	UNUSED_PARAM(csoType);

	PrintEnter;

	if(eAudioDecoderType == DI_MEDIA_AUDIO_DECODER_SPDIF)
	{
		/* SPDIF일 경우 MAIN의 Setting 값을 사용한다.
			Audio Decoder Sub일 경우 SPDIF를 사용하기 위해선 추가 작업이 필요 하다고 판단 됨.*/
		eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_MAIN;
		PrintError("[%s][%d] DI_MEDIA_AUDIO_DECODER_SPDIF\n", __FUNCTION__,__LINE__);
	}
	else
	{
		eTempAudioDecoderType = eAudioDecoderType;
	}


	/* Find Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(eVideoDecoderType != DI_MEDIA_VIDEO_DECODER_NONE)
		{
			if(hCsoPool[nIdx]->nIdxVideoDecoder == eVideoDecoderType)
			{
				pstCsoInstance = hCsoPool[nIdx];
				break;
			}
		}
		else if (eTempAudioDecoderType != DI_MEDIA_AUDIO_DECODER_NONE)
		{
			if(hCsoPool[nIdx]->nIdxAudioDecoder == eTempAudioDecoderType)
			{
				pstCsoInstance = hCsoPool[nIdx];
				break;
			}
		}
	}

	if(nIdx == DI_CSO_MAX_VIEW_NUM || pstCsoInstance == NULL)
	{
		PrintError("%s(%d) Find Cso Handle failed!\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	PrintError("%s(%d) [YKKIM + %p ]\n", __FUNCTION__, __LINE__,DRV_DEMUX_GetStcChannelHandle());
	NEXUS_StcChannel_GetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
	PrintError("%s(%d) [YKKIM - %p ]\n", __FUNCTION__, __LINE__,DRV_DEMUX_GetStcChannelHandle());

	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.modeSettings.pcr.pidChannel = NULL;

	if(pstCsoInstance->hPcrPidChannel != NULL)
	{
		stcSettings.modeSettings.pcr.pidChannel = pstCsoInstance->hPcrPidChannel;
	}
	else
	{
		if(pstCsoInstance->hVideoPidChannel != NULL)
		{
			stcSettings.modeSettings.pcr.pidChannel = pstCsoInstance->hVideoPidChannel;
			pstCsoInstance->hPcrPidChannel = stcSettings.modeSettings.pcr.pidChannel;
		}
		else if(pstCsoInstance->hAudioPidChannel != NULL)
		{
			stcSettings.modeSettings.pcr.pidChannel = pstCsoInstance->hAudioPidChannel;
			pstCsoInstance->hPcrPidChannel = stcSettings.modeSettings.pcr.pidChannel;
		}
	}

	switch(pstCsoInstance->eInputStreamType)
	{
		case DI_CSO_INPUT_STREAMTYPE_TS_MUX:
			stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
			break;
		case DI_CSO_INPUT_STREAMTYPE_ES_DIRECT:
			stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eEs;
			break;
		case DI_CSO_INPUT_STREAMTYPE_TS_LIVE:
			PrintDebug("[%s][%d] DI_CSO_INPUT_STREAMTYPE_TS_LIVE\n", __FUNCTION__, __LINE__);
			{/* for Live Multicast */
				NEXUS_TimebaseSettings timebaseSettings;

				/* reprogram the timebase: increase track range & max pcr errors */
				(void)NEXUS_Timebase_GetSettings(stcSettings.timebase, &timebaseSettings);
				timebaseSettings.sourceType                         = NEXUS_TimebaseSourceType_ePcr;
				timebaseSettings.sourceSettings.pcr.pidChannel      = pstCsoInstance->hPcrPidChannel;
				timebaseSettings.sourceSettings.pcr.maxPcrError     = IP_NETWORK_JITTER * 91;    /* in milliseconds: based on 90Khz clock */
				timebaseSettings.sourceSettings.pcr.jitterCorrection = NEXUS_TristateEnable_eDisable;
				timebaseSettings.sourceSettings.pcr.trackRange      = NEXUS_TimebaseTrackRange_e244ppm;
				(void)NEXUS_Timebase_SetSettings(stcSettings.timebase, &timebaseSettings);

				/* STC resetting for IP live service considering jitter about 300ms */
				stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* Live mode */
				stcSettings.autoConfigTimebase = false;
				/* offset threshold: uses upper 32 bits (183ticks/msec) of PCR clock */
				stcSettings.modeSettings.pcr.offsetThreshold= IP_NETWORK_JITTER * 183;
				stcSettings.modeSettings.pcr.pidChannel = pstCsoInstance->hPcrPidChannel;
				/* max pcr error: uses upper 32 bits (183ticks/msec) of PCR clock */
				stcSettings.modeSettings.pcr.maxPcrError= IP_NETWORK_JITTER * 183;
				stcSettings.modeSettings.pcr.disableTimestampCorrection=true;
				stcSettings.modeSettings.pcr.disableJitterAdjustment=true;
			}
			break;
		default :
			PrintError("[%s][%d] Can't find StreamType  %d\n", __FUNCTION__, __LINE__, pstCsoInstance->eInputStreamType);
	}

	nResult = NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_StcChannel_SetRate failed!\n");
		return NULL;
	}

	PrintExit;

	return DRV_DEMUX_GetStcChannelHandle();
}

NEXUS_PidChannelHandle DRV_CSO_GetPlaypumpVideoPidChannel(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_VIDEO_FORMAT eVideoCodec, void *VideoDecoderHandle)
{
	NEXUS_PlaypumpOpenPidChannelSettings pidSettings;
	NEXUS_PidChannelHandle videoPidChannel = NULL;
	NEXUS_PlaypumpSettings playpumpSettings;
	NEXUS_PlaypumpHandle playpump = NULL;
	DRV_Error dResult;
	DI_CSO_Instance *pstCsoInstance = NULL;
	HINT32 nIdx=0;
	NEXUS_Error nResult;

	UNUSED_PARAM(eVideoDecoderType);

	PrintEnter;

	if(VideoDecoderHandle == NULL)
	{
		PrintError("[%s][%d] Invalid Parameter! VideoDecoderHandle: %s\n", __FUNCTION__,__LINE__, VideoDecoderHandle?"T":"F");
		return NULL;
	}

	/* Find Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(hCsoPool[nIdx] == NULL)
		{
			PrintError("hCsoPool[%d] is NULL!\n", nIdx);
		}
		else
		{
			if(hCsoPool[nIdx]->nIdxVideoDecoder == eVideoDecoderType)
			{
				pstCsoInstance = hCsoPool[nIdx];
				PrintDebug("%s(%d)nIdx:%d, pstCsoInstance:%04X\n", __FUNCTION__, __LINE__, nIdx, (HUINT32)pstCsoInstance);
				break;
			}
			else
			{
				PrintDebug("%s(%d)hCsoPool[%d]->nIdxVideoDecoder:%d, eVideoDecoderType:%04X\n", __FUNCTION__, __LINE__, nIdx, hCsoPool[nIdx]->nIdxVideoDecoder, eVideoDecoderType);
			}
		}
	}

	if(nIdx == DI_CSO_MAX_VIEW_NUM || pstCsoInstance == NULL)
	{
		PrintError("%s(%d) Find Cso Handle failed!\n", __FUNCTION__, __LINE__);
		return NULL;
	}


	/********************
	 *	1. Get the current Playpump Handle
	 ********************/
	HUINT32 ulPumpIndex = 0;
	if(pstCsoInstance->stPumpInfo[DI_CSO_PUMPTYPE_V].bUsed == TRUE)
	{
		ulPumpIndex = pstCsoInstance->stPumpInfo[DI_CSO_PUMPTYPE_V].ulPlaypumpIndex;
		PrintDebug("%s(%d) stPumpInfo[DI_CSO_PUMPTYPE_V].ulPlaypumpIndex : %u\n", __FUNCTION__, __LINE__, ulPumpIndex);
	}
	else
	{
		ulPumpIndex = pstCsoInstance->uiCurrentPlaypumpIndex;
		PrintDebug("%s(%d) uiCurrentPlaypumpIndex : %u\n", __FUNCTION__, __LINE__, ulPumpIndex);
	}

	dResult = DRV_PVR_GetPlaypumpHandleByIndex(ulPumpIndex, &playpump);
	if((dResult != DRV_OK) || (playpump == NULL))
	{
		PrintError("DRV_PVR_GetPlaypumpHandle failed! pumphandle:%08X\n", (HUINT32)playpump);
		return NULL;
	}

	/********************
	*	2. Setup asf/avi(for packed xvid) Transport Type
	 ********************/
	(void)NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
	if(eVideoCodec == DI_VIDEO_FORMAT_VC1SimpleMain || eVideoCodec == DI_VIDEO_FORMAT_VC1/* || eVideoCodec == DI_VIDEO_FORMAT_DIVX_311*/)
	{
		playpumpSettings.originalTransportType = NEXUS_TransportType_eAsf;
	}
	else if(eVideoCodec == DI_VIDEO_FORMAT_MPEG4Part2)
	{
		playpumpSettings.originalTransportType = NEXUS_TransportType_eAvi;
	}
	else
	{
		playpumpSettings.originalTransportType = NEXUS_TransportType_eUnknown;
	}

	/* Nexus PlayPump의 TransportType의 Setting는 Pump의 PidChannel이 열려 있으면 변경하지 못한다.
		(참고 : NEXUS_Playpump_SetSettings()의 BLST_S_FIRST(&p->pid_list)!=NULL))
		OCTO에서는 DI_Video의 Start에서 이 함수를 호출하고
		아래의 Playpump_OpenPidChannel 이후에는 TransportType는 변경을 하지 못한다.
		그러므로 Playpump_OpenPidChannel을 하기전에 TransportType를 변경 하도록 해야 한다.*/
	switch(pstCsoInstance->eInputStreamType)
	{
		case DI_CSO_INPUT_STREAMTYPE_TS_MUX:
		case DI_CSO_INPUT_STREAMTYPE_TS_LIVE:
			playpumpSettings.transportType = NEXUS_TransportType_eTs;
			break;
		case DI_CSO_INPUT_STREAMTYPE_ES_DIRECT:
			playpumpSettings.transportType = NEXUS_TransportType_eEs;
			break;
		default :
			PrintError("[%s][%d] Can't find StreamType	%d\n", __FUNCTION__, __LINE__, pstCsoInstance->eInputStreamType);
	}

	nResult = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_GetStatus failed!\n");
		return NULL;
	}

	/********************
	 *	3. Open Video Channel
	 ********************/
	(void)NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidSettings);
	pidSettings.pidType = NEXUS_PidType_eVideo;
#if (NEXUS_VERSION >= 1250)
	pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
#endif
	if(pstCsoInstance->hVideoPidChannel == NULL || (pstCsoInstance->unVideoPidOfPidChannel != pstCsoInstance->unVideoStreamId))
	{
		if(pstCsoInstance->hVideoPidChannel != NULL && playpump != NULL)
	{
			(void)NEXUS_Playpump_ClosePidChannel(playpump, pstCsoInstance->hVideoPidChannel);
		}
		pstCsoInstance->hVideoPidChannel = videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pstCsoInstance->unVideoStreamId, &pidSettings);
		pstCsoInstance->unVideoPidOfPidChannel = pstCsoInstance->unVideoStreamId;
	}
	else
	{
		videoPidChannel = pstCsoInstance->hVideoPidChannel;
	}
	pstCsoInstance->hVideoDecoder = VideoDecoderHandle;
	PrintExit;
	return videoPidChannel;
}

HBOOL DRV_CSO_CheckStart(void)
{
	return s_ulCsoStart==0 ? FALSE : TRUE;
}

/**
 * @brief		DI Cso Audio Codec을 Nexus의 Audio Codec Type으로 변환한다.
 *
 * @param[in]	eAudioCodec 	Di Cso Audio Codec Type
 * @return		NEXUS_AudioCodec	Nexus Audio Codec Type
 * @see			DRV_CSO_GetPlaybackAudioPidChannel
 */
NEXUS_AudioCodec P_CSO_ConvertCsoAudioCodecToNexus(DI_CSO_AUDIO_FORMAT_e eAudioCodec)
{
	NEXUS_AudioCodec nexusCodec = NEXUS_AudioCodec_eUnknown;

	PrintEnter;

	switch (eAudioCodec)
	{
		case DI_CSO_AUDIO_FORMAT_MPEG:
			nexusCodec = NEXUS_AudioCodec_eMpeg;
			break;
		case DI_CSO_AUDIO_FORMAT_MP3:
			nexusCodec = NEXUS_AudioCodec_eMp3;
			break;
		case DI_CSO_AUDIO_FORMAT_AAC:
			nexusCodec = NEXUS_AudioCodec_eAac;
			break;
		case DI_CSO_AUDIO_FORMAT_AAC_ADTS:
			nexusCodec = NEXUS_AudioCodec_eAacAdts;
			break;
		case DI_CSO_AUDIO_FORMAT_AAC_LOAS:
			nexusCodec = NEXUS_AudioCodec_eAacLoas;
			break;
		case DI_CSO_AUDIO_FORMAT_AAC_PLUS:
			nexusCodec = NEXUS_AudioCodec_eAacPlus;
			break;
		case DI_CSO_AUDIO_FORMAT_AAC_PLUS_LOAS:
			nexusCodec = NEXUS_AudioCodec_eAacPlusLoas;
			break;
		case DI_CSO_AUDIO_FORMAT_AAC_PLUS_ADTS:
			nexusCodec = NEXUS_AudioCodec_eAacPlusAdts;
			break;
		case DI_CSO_AUDIO_FORMAT_AC3:
			nexusCodec = NEXUS_AudioCodec_eAc3;
			break;
		case DI_CSO_AUDIO_FORMAT_AC3_PLUS:
			nexusCodec = NEXUS_AudioCodec_eAc3Plus;
			break;
		case DI_CSO_AUDIO_FORMAT_PCM:
			nexusCodec = NEXUS_AudioCodec_eLpcmDvd;//pcm???
			break;
		case DI_CSO_AUDIO_FORMAT_DTS:
			nexusCodec = NEXUS_AudioCodec_eDts;
			break;
		case DI_CSO_AUDIO_FORMAT_LPCM_HD_DVD:
			nexusCodec = NEXUS_AudioCodec_eLpcmHdDvd;
			break;
		case DI_CSO_AUDIO_FORMAT_LPCM_BLUERAY:
			nexusCodec = NEXUS_AudioCodec_eLpcmBluRay;
			break;
		case DI_CSO_AUDIO_FORMAT_DTS_HD:
			nexusCodec = NEXUS_AudioCodec_eDtsHd;
			break;
		case DI_CSO_AUDIO_FORMAT_WMA_STD:
			nexusCodec = NEXUS_AudioCodec_eWmaStd;
			break;
		case DI_CSO_AUDIO_FORMAT_WMA_PRO:
			nexusCodec = NEXUS_AudioCodec_eWmaPro;
			break;
		case DI_CSO_AUDIO_FORMAT_FLAC:
			nexusCodec = NEXUS_AudioCodec_eFlac;
			break;
		case DI_CSO_AUDIO_FORMAT_LPCM_DVD:
			nexusCodec = NEXUS_AudioCodec_eLpcmDvd;
			break;
		case DI_CSO_AUDIO_FORMAT_PCMWAVE:
			nexusCodec = NEXUS_AudioCodec_ePcmWav;
			break;
		case DI_CSO_AUDIO_FORMAT_ADPCM:
			nexusCodec = NEXUS_AudioCodec_eAdpcm;
			break;
		case DI_CSO_AUDIO_FORMAT_VORBIS:
			nexusCodec = NEXUS_AudioCodec_eVorbis;
			break;
#if defined(CONFIG_AUDIO_DEC_OPUS)
		case DI_CSO_AUDIO_FORMAT_OPUS:
			nexusCodec = NEXUS_AudioCodec_eOpus;
			break;
#endif
		case DI_CSO_AUDIO_FORMAT_APE:
			nexusCodec = NEXUS_AudioCodec_eApe;
			break;
		case DI_CSO_AUDIO_FORMAT_DRA:
			nexusCodec = NEXUS_AudioCodec_eDra;
			break;
		default:
			nexusCodec = NEXUS_AudioCodec_eUnknown;
			break;
	}

	PrintExit;

	return nexusCodec;
}

NEXUS_PidChannelHandle DRV_CSO_GetPlaypumpAudioPidChannel(DI_MEDIA_AUDIO_FORMAT_e eAudioCodec, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, void *AudioDecoderHandle)
{
	NEXUS_PlaypumpOpenPidChannelSettings pidSettings;
	NEXUS_PidChannelHandle audioPidChannel = NULL;
	NEXUS_PlaypumpHandle playpump = NULL;
	NEXUS_PlaypumpSettings playpumpSettings;
	DRV_Error dResult;
	DI_CSO_Instance *pstCsoInstance = NULL;
	HINT32 nIdx=0;
	DI_MEDIA_AUDIO_TYPE_e eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_NONE;
	NEXUS_Error nResult;

	PrintEnter;

	if(AudioDecoderHandle == NULL)
	{
		PrintError("[%s][%d] Invalid Parameter! AudioDecoderHandle: %s\n", __FUNCTION__,__LINE__, AudioDecoderHandle?"T":"F");
		return NULL;
	}

	if(eAudioDecoderType == DI_MEDIA_AUDIO_DECODER_SPDIF)
	{
		/* SPDIF일 경우 MAIN의 Setting 값을 사용한다.
			Audio Decoder Sub일 경우 SPDIF를 사용하기 위해선 추가 작업이 필요 하다고 판단 됨.*/
		eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_MAIN;
		PrintError("[%s][%d] DI_MEDIA_AUDIO_DECODER_SPDIF\n", __FUNCTION__,__LINE__);
	}
	else
	{
		eTempAudioDecoderType = eAudioDecoderType;
	}


	/* Find Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(hCsoPool[nIdx] == NULL)
		{
			PrintError("hCsoPool[%d] is NULL!\n", nIdx);
		}
		else
		{
			if(hCsoPool[nIdx]->nIdxAudioDecoder == eTempAudioDecoderType)
			{
				pstCsoInstance = hCsoPool[nIdx];
				PrintDebug("%s(%d)nIdx:%d, pstCsoInstance:%04X\n", __FUNCTION__, __LINE__, nIdx, (HUINT32)pstCsoInstance);
				break;
			}
			else
			{
				PrintDebug("%s(%d)hCsoPool[%d]->nIdxAudioDecoder:%d, eAudioDecoderType:%04X\n", __FUNCTION__, __LINE__, nIdx, hCsoPool[nIdx]->nIdxAudioDecoder, eAudioDecoderType);
			}
		}
	}

	if(nIdx == DI_CSO_MAX_VIEW_NUM || pstCsoInstance == NULL)
	{
		PrintError("%s(%d) Find Cso Handle failed!\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	/********************
	 *	1. Get the current Playpump Handle
	 ********************/
	HUINT32 ulPumpIndex = 0;
	if(pstCsoInstance->stPumpInfo[DI_CSO_PUMPTYPE_A].bUsed == TRUE)
	{
		ulPumpIndex = pstCsoInstance->stPumpInfo[DI_CSO_PUMPTYPE_A].ulPlaypumpIndex;
		PrintDebug("%s(%d) tPumpInfo[DI_CSO_PUMPTYPE_A].ulPlaypumpIndex : %u\n", __FUNCTION__, __LINE__, ulPumpIndex);
	}
	else
	{
		ulPumpIndex = pstCsoInstance->uiCurrentPlaypumpIndex;
		PrintDebug("%s(%d) uiCurrentPlaypumpIndex : %u\n", __FUNCTION__, __LINE__, ulPumpIndex);
	}

	dResult = DRV_PVR_GetPlaypumpHandleByIndex(ulPumpIndex, &playpump);
	if((dResult != DRV_OK) || (playpump == NULL))
	{
		PrintError("DRV_PVR_GetPlaypumpHandle failed! pumphandle:%08X\n", (HUINT32)playpump);
		return NULL;
	}

	/* Nexus PlayPump의 TransportType의 Setting는 Pump의 PidChannel이 열려 있으면 변경하지 못한다.
		(참고 : NEXUS_Playpump_SetSettings()의 BLST_S_FIRST(&p->pid_list)!=NULL))
		OCTO에서는 DI_Audio의 Start에서 이 함수를 호출하고
		아래의 Playpump_OpenPidChannel 이후에는 TransportType는 변경을 하지 못한다.
		그러므로 Playpump_OpenPidChannel을 하기전에 TransportType를 변경 하도록 해야 한다.*/
	(void)NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
	switch(pstCsoInstance->eInputStreamType)
	{
		case DI_CSO_INPUT_STREAMTYPE_TS_MUX:
		case DI_CSO_INPUT_STREAMTYPE_TS_LIVE:
			playpumpSettings.transportType = NEXUS_TransportType_eTs;
			break;
		case DI_CSO_INPUT_STREAMTYPE_ES_DIRECT:
			playpumpSettings.transportType = NEXUS_TransportType_eEs;
			break;
		default :
			PrintError("[%s][%d] Can't find StreamType	%d\n", __FUNCTION__, __LINE__, pstCsoInstance->eInputStreamType);
	}
	nResult = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_GetStatus failed!\n");
		return NULL;
	}

	/********************
	 *	2. Open Audio Channel
	 ********************/
	(void)NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidSettings);
	pidSettings.pidType = NEXUS_PidType_eAudio;
	pidSettings.pidTypeSettings.audio.codec = P_CSO_ConvertCsoAudioCodecToNexus(eAudioCodec);
#if (NEXUS_VERSION >= 1250)
	pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
#endif
	/* SPDIF일 경우 MAIN()의 AudioPidChannel을 Return한다. */
	if(eAudioDecoderType == DI_MEDIA_AUDIO_DECODER_SPDIF)
	{
		audioPidChannel = pstCsoInstance->hAudioPidChannel;
		PrintError("[%s][%d] DI_CSO_AUDIO_DECODER_SPDIF\n", __FUNCTION__,__LINE__);
	}
	else
	{
		if(pstCsoInstance->hAudioPidChannel == NULL || (pstCsoInstance->unAudioPidOfPidChannel != pstCsoInstance->unAudioStreamId))
		{
			if(pstCsoInstance->hAudioPidChannel != NULL && playpump != NULL)
		{
				(void)NEXUS_Playpump_ClosePidChannel(playpump, pstCsoInstance->hAudioPidChannel);
			}
			pstCsoInstance->hAudioPidChannel = audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, pstCsoInstance->unAudioStreamId, &pidSettings);
			pstCsoInstance->unAudioPidOfPidChannel = pstCsoInstance->unAudioStreamId;
		}
		else
		{
			audioPidChannel = pstCsoInstance->hAudioPidChannel;
		}
		pstCsoInstance->hAudioDecoder = AudioDecoderHandle;
	}

	PrintExit;
	return audioPidChannel;
}

DRV_Error DRV_CSO_GetSpeed (DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_PlaySpeed_e *peSpeed)
{
	HINT32 nIdx = 0;
	DI_CSO_Instance *pstCsoInstance = NULL;

	/* Find Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(hCsoPool[nIdx] == NULL)
		{
			PrintError("hCsoPool[%d] is NULL!\n", nIdx);
		}
		else
		{
			if(hCsoPool[nIdx]->nIdxVideoDecoder == eVideoDecoderType)
			{
				pstCsoInstance = hCsoPool[nIdx];
				PrintError("%s(%d)nIdx:%d, pstCsoInstance:%04X\n", __FUNCTION__, __LINE__, nIdx, (HUINT32)pstCsoInstance);
				break;
			}
			else
			{
				PrintError("%s(%d)hCsoPool[%d]->nIdxVideoDecoder:%d, eVideoDecoderType:%04X\n", __FUNCTION__, __LINE__, nIdx, hCsoPool[nIdx]->nIdxVideoDecoder, eVideoDecoderType);
			}
		}
	}

	if(nIdx == DI_CSO_MAX_VIEW_NUM || pstCsoInstance == NULL)
	{
		PrintError("%s(%d) Find Cso Handle failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	*peSpeed = pstCsoInstance->eSpeed;

	return DRV_ERR;
}

DRV_Error DRV_CSO_SetAudioPidChannel(NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_AudioDecoderHandle ulAudioDecHandle,
		NEXUS_AudioDecoderHandle ulAudioPassThroughDecHandle)
{
	UNUSED_PARAM(ulPidChannelHandle);
	UNUSED_PARAM(ulAudioDecHandle);
	UNUSED_PARAM(ulAudioPassThroughDecHandle);

	/* jhchoi2 : Workaround Patch Return is DRV_OK.
	Cso에서 Playback을 사용하지 않고 Playpump로 변경되어서
	Sub Audio Handle을 Setting할 API가 없어 DRV_OK로 Patch를 진행 함
	만약 Audio의 ByPass의 동작이 되지 않을 경우 패치가 필요*/
	return DRV_OK;
}

DRV_Error DRV_CSO_OpenPlaypumpPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, HINT32 nPid, NEXUS_PidChannelHandle *phPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	NEXUS_PlaypumpOpenPidChannelSettings pidSettings;
	HUINT32 nIdx =0;
	DI_CSO_Instance *pstCsoInstance = NULL;
	NEXUS_PlaypumpHandle hPlaypump = NULL;

	PrintEnter;

	PrintDebug("%s(%d) Info> nDemuxId(%d) , nChannelType(%d) , nPid(0x%x)\n",__FUNCTION__,__LINE__,nDemuxId, nChannelType, nPid);

#if 0   /* Support SECTION Filtering */
	/* The Media module doesn't support the SECTION Filtering */
	if(nChannelType == DI_DEMUX_CHANNELTYPE_SECTION)
	{
		PrintError("[%s] Not Support Setion Filtering in MEDIA Interface!\n\r",__FUNCTION__);
		PrintExit;
		return DRV_ERR;
	}
#endif

	/* Find Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(hCsoPool[nIdx] == NULL)
		{
			PrintError("hCsoPool[%d] is NULL!\n", nIdx);
		}
		else
		{
			if(hCsoPool[nIdx]->ulDemuxId == (HUINT32)nDemuxId)
			{
				pstCsoInstance = hCsoPool[nIdx];
				PrintDebug("%s(%d)nIdx:%d, pstCsoInstance:%04X\n", __FUNCTION__, __LINE__, nIdx, (HUINT32)pstCsoInstance);
				break;
			}
			else
			{
				PrintDebug("%s(%d)hCsoPool[%d]->ulDemuxId:%d, nChannelType:%04X\n", __FUNCTION__, __LINE__, nIdx, hCsoPool[nIdx]->ulDemuxId, nChannelType);
			}
		}
	}

	if(nIdx >= DI_CSO_MAX_VIEW_NUM)
	{
		PrintError("[%s] invalid nDemuxId.\n", __func__);
		nDrvRet = DRV_ERR;
	}
	else
	{
#if 0   /* Support SECTION Filtering */
		/* validation checking */
		if(s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_READY)
		{
			PrintError("[%s] Media doesn't ready.)\n", __func__);
			PrintExit;
			return DRV_ERR;
		}
#endif
		if(pstCsoInstance->eInputStreamType == DI_CSO_INPUT_STREAMTYPE_ES_DIRECT)
		{
			PrintError("%s(%d) Can't Support INPUT_STREAMTYPE_ES_DIRECT\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
		nDrvRet = DRV_PVR_GetPlaypumpHandleByIndex(pstCsoInstance->uiCurrentPlaypumpIndex, &hPlaypump);
		if((nDrvRet != DRV_OK) || (hPlaypump == NULL))
		{
			PrintError("DRV_PVR_GetPlaypumpHandle failed!\n");
			PrintExit;
			return DRV_ERR;
		}

		NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidSettings);
		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO:
				{
					pidSettings.pidType = NEXUS_PidType_eVideo;
#if (NEXUS_VERSION >= 1250)
					pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
					*phPidChannel = NEXUS_Playpump_OpenPidChannel(hPlaypump, nPid, &pidSettings);
					pstCsoInstance->hVideoPidChannel = *phPidChannel;
					pstCsoInstance->unVideoPidOfPidChannel = nPid;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO:
				{
#if (NEXUS_VERSION >= 1250)
					pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
					pidSettings.pidType = NEXUS_PidType_eAudio;
					pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
					*phPidChannel = NEXUS_Playpump_OpenPidChannel(hPlaypump, nPid, &pidSettings);
					pstCsoInstance->hAudioPidChannel = *phPidChannel;
					pstCsoInstance->unAudioPidOfPidChannel = nPid;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF:
				*phPidChannel = pstCsoInstance->hAudioPidChannel;
				break;
			case DI_DEMUX_CHANNELTYPE_RECORD:
				PrintError("[%s] Not Support Type\n", __func__);
				PrintExit;
				return DRV_ERR;
			case DI_DEMUX_CHANNELTYPE_SECTION:
				pidSettings.pidType = NEXUS_PidType_eOther;
				*phPidChannel = NEXUS_Playpump_OpenPidChannel(hPlaypump, nPid, &pidSettings);
				break;
			case DI_DEMUX_CHANNELTYPE_PES:
				pidSettings.pidType = NEXUS_PidType_eOther;
				*phPidChannel = NEXUS_Playpump_OpenPidChannel(hPlaypump, nPid, &pidSettings);
				break;
			case DI_DEMUX_CHANNELTYPE_PCR:
				{
					pidSettings.pidType = NEXUS_PidType_eOther;
					if(pstCsoInstance->unVideoPidOfPidChannel == (HUINT32)nPid)
					{
						*phPidChannel = pstCsoInstance->hVideoPidChannel;
					}
					else if(pstCsoInstance->unAudioPidOfPidChannel == (HUINT32)nPid)
					{
						*phPidChannel = pstCsoInstance->hAudioPidChannel;
					}
					else
					{
						*phPidChannel = NEXUS_Playpump_OpenPidChannel(hPlaypump, nPid, &pidSettings);
					}
					pstCsoInstance->hPcrPidChannel = *phPidChannel;
				}
				break;
				/* for descramble for dsc */
			case DI_DEMUX_CHANNELTYPE_PRIVATE:
				pidSettings.pidType = NEXUS_PidType_eOther;
				*phPidChannel = NEXUS_Playpump_OpenPidChannel(hPlaypump, nPid, &pidSettings);
				break;

			default:
				pidSettings.pidType = NEXUS_PidType_eUnknown;
				break;
		}
	}

	PrintExit;

	return nDrvRet;
}

DRV_Error DRV_CSO_ClosePlaypumpPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, NEXUS_PidChannelHandle hPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	HUINT32 nIdx =0;
	DI_CSO_Instance *pstCsoInstance = NULL;
	NEXUS_PlaypumpHandle hPlaypump = NULL;

	PrintEnter;

	PrintDebug("%s(%d) Info> nDemuxId(%d) , nChannelType(%d) \n",__FUNCTION__,__LINE__,nDemuxId, nChannelType);

	/* Find Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(hCsoPool[nIdx] == NULL)
		{
			PrintError("hCsoPool[%d] is NULL!\n", nIdx);
			nDrvRet = DRV_ERR;
		}
		else
		{
			if(hCsoPool[nIdx]->ulDemuxId == (HUINT32)nDemuxId)
			{
				pstCsoInstance = hCsoPool[nIdx];
				PrintDebug("%s(%d)nIdx:%d, pstCsoInstance:%04X\n", __FUNCTION__, __LINE__, nIdx, (HUINT32)pstCsoInstance);
				break;
			}
			else
			{
				PrintDebug("%s(%d)hCsoPool[%d]->ulDemuxId:%d, nChannelType:%04X\n", __FUNCTION__, __LINE__, nIdx, hCsoPool[nIdx]->ulDemuxId, nChannelType);
			}
		}
	}

	if(nIdx >= DI_CSO_MAX_VIEW_NUM)
	{
		PrintError("[%s] invalid nDemuxId.\n", __func__);
		nDrvRet = DRV_ERR;
	}
	else
	{
		if(pstCsoInstance->eInputStreamType == DI_CSO_INPUT_STREAMTYPE_ES_DIRECT)
		{
			PrintError("%s(%d) Can't Support INPUT_STREAMTYPE_ES_DIRECT\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}

		/** Nexus Pid Channel을 Close한다. */
		if(hPidChannel != NULL)
		{
			nDrvRet = DRV_PVR_GetPlaypumpHandleByIndex(pstCsoInstance->uiCurrentPlaypumpIndex, &hPlaypump);
			if((nDrvRet != DRV_OK) || (hPlaypump == NULL))
			{
				PrintError("DRV_PVR_GetPlaypumpHandle failed!\n");
				return DRV_ERR;
			}

			if(hPlaypump != NULL)
			{
				(void)NEXUS_Playpump_ClosePidChannel(hPlaypump, hPidChannel);
				PrintError("%s(%d) hPlaypump(%08X), Close Session hPidChannel(%08X)\n", __FUNCTION__, __LINE__, (HUINT32)hPlaypump, (HUINT32)hPidChannel);
			}
			else
			{
				PrintError("%s(%d) hPlaypump is NULL\n", __FUNCTION__, __LINE__);
				nDrvRet = DRV_ERR;
			}

			switch(nChannelType)
			{
				case DI_DEMUX_CHANNELTYPE_VIDEO:
					pstCsoInstance->hVideoPidChannel = NULL;
					break;
				case DI_DEMUX_CHANNELTYPE_AUDIO:
					pstCsoInstance->hAudioPidChannel = NULL;
					break;
				case DI_DEMUX_CHANNELTYPE_PCR:
					pstCsoInstance->hPcrPidChannel = NULL;
					break;
				default:
					break;
			}
		}
	}
	return nDrvRet;
}

/*------------------------------------------------------------------------------------------------------------------------------------------*/


#if __________di__________
#endif
/* Start global function prototypes */

DI_ERR_CODE DI_CSO_Init ( DI_CSO_CONFIG_t stConfig )
{
	UNUSED_PARAM(stConfig);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetDefaultConfiguration( DI_CSO_CONFIG_t *pstConfig )
{
	UNUSED_PARAM(pstConfig);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_Uninit (void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_CreateSession (HINT8 *pURL, CSO_HANDLE *pHSession)
{
	DI_CSO_Instance* pstCsoInstance=NULL;

	if(*pHSession != NULL)
	{
		PrintError("%s(%d) CSO_HANDLE is Not Init! : %X\n", __FUNCTION__, __LINE__, (HUINT32)*pHSession);
		return DI_ERR_ERROR;
	}

	UNUSED_PARAM(pURL);
	pstCsoInstance = (DI_CSO_Instance*)VK_MEM_Alloc(sizeof(DI_CSO_Instance));
	if(pstCsoInstance == NULL)
	{
		PrintError("[%s][%d] Out of memory!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(pstCsoInstance, 0x00, sizeof(DI_CSO_Instance));

	pstCsoInstance->eSpeed = eDI_MEDIA_Speed_Normal;

	*pHSession = pstCsoInstance;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_DestroySession (CSO_HANDLE pHSession)
{
	DI_CSO_Instance* pstCsoInstance=(DI_CSO_Instance* )pHSession;
	HINT32 nIdx = 0;

	if(pstCsoInstance == NULL)
	{
		return DI_ERR_OK;
	}

	/* Delete Pool Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(hCsoPool[nIdx] == pstCsoInstance)
		{
			hCsoPool[nIdx] = NULL;
		}
	}
	VK_MEM_Free(pstCsoInstance);
	pstCsoInstance = NULL;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_OpenSession (CSO_HANDLE pHSession)
{
	UNUSED_PARAM(pHSession);

	return DI_ERR_OK;

}

DI_ERR_CODE DI_CSO_CloseSession (CSO_HANDLE pHSession)
{
	UNUSED_PARAM(pHSession);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_ProbeSession (CSO_HANDLE pHSession)
{
	UNUSED_PARAM(pHSession);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_ProbeCancel (CSO_HANDLE pHSession)
{
	UNUSED_PARAM(pHSession);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetProbeInfo (CSO_HANDLE pHSession, DI_CSO_INFO_t *pstProbeInfo )
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(pstProbeInfo);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetDecoderIndex (CSO_HANDLE pHSession, HUINT32 nIdxVideoDecoder,  HUINT32 nIdxAudioDecoder)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	HINT32 nIdx = 0;

	if(pHSession == NULL)
	{
		PrintError("%s(%d) CSO_HANDLE is NULL!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	/* Find Cso Hendle */
	for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
	{
		if(hCsoPool[nIdx] == pstCsoInstance)
		{
			PrintError("%s(%d)nIdx:%d, pstCsoInstance:%04X\n", __FUNCTION__, __LINE__, nIdx, (HUINT32)pstCsoInstance);
			break;
		}
	}

	if(nIdx == DI_CSO_MAX_VIEW_NUM)
	{
		/* First Setting */
		/* Put Cso Hendle */
		for(nIdx = 0; nIdx < DI_CSO_MAX_VIEW_NUM; ++nIdx)
		{
			if(hCsoPool[nIdx] == NULL)
			{
				hCsoPool[nIdx] = pstCsoInstance;
				PrintError("%s(%d)nIdx:%d, pstCsoInstance:%04X\n", __FUNCTION__, __LINE__, nIdx, (HUINT32)pstCsoInstance);
				break;
			}
		}

		if(nIdx == DI_CSO_MAX_VIEW_NUM)
		{
			PrintError("[#ERROR] %s failed!\n", __FUNCTION__);
			return DI_ERR_ERROR;
		}
	}

	hCsoPool[nIdx]->nIdxVideoDecoder = nIdxVideoDecoder;
	hCsoPool[nIdx]->nIdxAudioDecoder = nIdxAudioDecoder;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetupSession (CSO_HANDLE pHSession, HUINT32 *pulCsoId)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(pulCsoId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_ReleaseSession (CSO_HANDLE pHSession, HUINT32 unCsoId)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(unCsoId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetAudioTrack(CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT8 ucAudioTrack)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(unCsoId);
	UNUSED_PARAM(ucAudioTrack);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetActiveAudioTrack(CSO_HANDLE pHSession, HUINT32 unAudioCodecId, HUINT32 unAudioStreamId)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;

	pstCsoInstance->unAudioStreamId = unAudioStreamId;
	pstCsoInstance->unAudioCodecId = unAudioCodecId;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetActiveAudioTrack(CSO_HANDLE pHSession, HUINT32 *unAudioCodecId, HUINT32 *unAudioStreamId)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;

	*unAudioStreamId = pstCsoInstance->unAudioStreamId;
	*unAudioCodecId = pstCsoInstance->unAudioCodecId;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetActiveVideoTrack(CSO_HANDLE pHSession, HUINT32 unVideoCodecId, HUINT32 unVideoStreamId)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;

	pstCsoInstance->unVideoStreamId = unVideoStreamId;
	pstCsoInstance->unVideoCodecId = unVideoCodecId;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetActiveVideoTrack(CSO_HANDLE pHSession, HUINT32 *unVideoCodecId, HUINT32 *unVideoStreamId)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;

	*unVideoStreamId = pstCsoInstance->unVideoStreamId;
	*unVideoCodecId = pstCsoInstance->unVideoCodecId;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetUserAgentString(CSO_HANDLE pHSession,  HINT8 *pUserAgentString, HINT32 nSize)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(pUserAgentString);
	UNUSED_PARAM(nSize);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetPosition (CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT64 ullPosition)
{

	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(unCsoId);
	UNUSED_PARAM(ullPosition);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetDecodedPicture (CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT32 *pulDecodedPic, HUINT32 *pulDisplayedPic )
{
	DRV_Error dResult;
	NEXUS_Error nResult;
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_VideoDecoderStatus videoStatus;

	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	UNUSED_PARAM(unCsoId);

	/*************************
	 *	1. Check Video pts if started.
	 *************************/
	dResult = DRV_VIDEO_GetVideoHandle(pstCsoInstance->nIdxVideoDecoder-DI_MEDIA_VIDEO_DECODER_MAIN, &videoDecoder);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_VIDEO_GetVideoHandle failed!\n");
		return DI_ERR_ERROR;
	}

	nResult = NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_GetStatus failed!\n");
		return DI_ERR_ERROR;
	}

	if(videoStatus.started == TRUE)
	{
		*pulDecodedPic = videoStatus.numDecoded;
		*pulDisplayedPic = videoStatus.numDisplayed;
	}
//Done:
	return DI_ERR_OK;
}

#define NEXUS_BMEDIA_TIME_SCALE_BASE 100
#define NEXUS_BMEDIA_PTS_MODULO		(((uint64_t)1)<<32)
#define NEXUS_BMPEG2PES_PTS_UNITS	 45000
#define NEXUS_BMEDIA_PTS_SCALE (NEXUS_BMPEG2PES_PTS_UNITS/1000)

DI_ERR_CODE DI_CSO_GetPosition (CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT64 *pullPosition)
{
	DRV_Error dResult;
	NEXUS_Error nResult;
	HUINT32 pts=0;
	HUINT64 ullCurrentPosition=0;
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_VideoDecoderStatus videoStatus;
	NEXUS_AudioDecoderStatus audioStatus;

	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	UNUSED_PARAM(unCsoId);

	/*************************
	 *	1. Check Video pts if started.
	 *************************/
	dResult = DRV_VIDEO_GetVideoHandle(pstCsoInstance->nIdxVideoDecoder-DI_MEDIA_VIDEO_DECODER_MAIN, &videoDecoder);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_VIDEO_GetVideoHandle failed!\n");
		return DI_ERR_ERROR;
	}

	nResult = NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_GetStatus failed!\n");
		return DI_ERR_ERROR;
	}

	if(videoStatus.started == TRUE)
	{
		pts = videoStatus.pts;
		ullCurrentPosition = ((uint64_t)pts)*NEXUS_BMEDIA_TIME_SCALE_BASE/(NEXUS_BMEDIA_PTS_SCALE*NEXUS_BMEDIA_TIME_SCALE_BASE);
		//goto Done;
		//PrintDebug("V: %llu \n", ullCurrentPosition);
	}

	/*************************
	 *	2. Check Audio pts if started.
	 *************************/
	(void) DRV_AUDIO_GetDecoderHandle(pstCsoInstance->nIdxAudioDecoder-DI_MEDIA_AUDIO_DECODER_MAIN, &audioDecoder);
	nResult = NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
		return DI_ERR_ERROR;
	}

	if(audioStatus.started == TRUE)
	{
		if(pts < audioStatus.pts) /* Video Playtime < Audio Playtime OR Video not started */
		{
			pts = audioStatus.pts;
			ullCurrentPosition = ((uint64_t)pts)*NEXUS_BMEDIA_TIME_SCALE_BASE/(NEXUS_BMEDIA_PTS_SCALE*NEXUS_BMEDIA_TIME_SCALE_BASE);
			//PrintDebug("A: %llu \n", ullCurrentPosition);
		}
	}

//Done:

	if((int64_t)(ullCurrentPosition - pstCsoInstance->ullFirstPosition) > 0)
	{
		ullCurrentPosition = ullCurrentPosition - pstCsoInstance->ullFirstPosition;
	}
	else
	{
		ullCurrentPosition = 0;
	}

	*pullPosition = ullCurrentPosition;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetSpeed (CSO_HANDLE pHSession, HUINT32 unCsoId, DI_MEDIA_PlaySpeed_e eSpeed )
{
	unsigned decode_rate=0;
	unsigned denominator,numerator;
	DRV_Error dResult;
	NEXUS_Error nResult;
	NEXUS_StcChannelHandle stcChannel=NULL;
	NEXUS_VideoDecoderTrickState vdecState;
	NEXUS_AudioDecoderTrickState adecState;
	NEXUS_VideoDecoderHandle videoDecoder;
	NEXUS_AudioDecoderHandle audioDecoder;
	NEXUS_VideoDecoderStatus videoStatus;
	NEXUS_AudioDecoderStatus audioStatus;

	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	UNUSED_PARAM(unCsoId);
	PrintEnter;

	decode_rate = P_CSO_GetDecodeRate(eSpeed);

	/********************
	 *	1. invalidate stc channel
	 ********************/
	stcChannel = DRV_DEMUX_GetStcChannelHandle();
	nResult = NEXUS_StcChannel_Invalidate(stcChannel);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_StcChannel_Invalidate failed!\n");
		return DI_ERR_ERROR;
	}

	/********************
	 *	2. set the rate to Video Decoder
	 ********************/
	dResult = DRV_VIDEO_GetVideoHandle(pstCsoInstance->nIdxVideoDecoder-DI_MEDIA_VIDEO_DECODER_MAIN, &videoDecoder);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_VIDEO_GetVideoHandle failed!\n");
		return DI_ERR_ERROR;
	}

	nResult = NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_GetStatus failed!\n");
		return DI_ERR_ERROR;
	}

	if(videoStatus.started == TRUE)
	{
		NEXUS_VideoDecoder_GetTrickState(videoDecoder, &vdecState);
		vdecState.rate = decode_rate;
#if (NEXUS_VERSION >= 1403)
		vdecState.stcTrickEnabled = (vdecState.rate != NEXUS_NORMAL_PLAY_SPEED);
#endif

		nResult = NEXUS_VideoDecoder_SetTrickState(videoDecoder, &vdecState);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_VideoDecoder_SetTrickState failed\n");
			return DI_ERR_ERROR;
		}
	}

	/********************
	 *	3. set the rate to Audio Decoder
	 ********************/
	(void)DRV_AUDIO_GetDecoderHandle(pstCsoInstance->nIdxAudioDecoder-DI_MEDIA_AUDIO_DECODER_MAIN, &audioDecoder);

	nResult = NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
		return DI_ERR_ERROR;
	}

	if(audioStatus.started == TRUE)
	{

#if (NEXUS_VERSION >= 1501)
		(void)NEXUS_AudioDecoder_GetTrickState(audioDecoder, &adecState);

		adecState.rate = decode_rate;
		adecState.stopForTrick = true;
		nResult = NEXUS_AudioDecoder_SetTrickState(audioDecoder, &adecState);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_SetTrickState failed\n");
			return DI_ERR_ERROR;
		}

		(void)NEXUS_AudioDecoder_GetTrickState(audioDecoder, &adecState);

		adecState.rate = decode_rate;
		adecState.stopForTrick = false;
		nResult = NEXUS_AudioDecoder_SetTrickState(audioDecoder, &adecState);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_SetTrickState failed\n");
			return DI_ERR_ERROR;
		}
#else
		(void)NEXUS_AudioDecoder_GetTrickState(audioDecoder, &adecState);

		adecState.rate = decode_rate;
		nResult = NEXUS_AudioDecoder_SetTrickState(audioDecoder, &adecState);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_SetTrickState failed\n");
			return DI_ERR_ERROR;
		}
#endif

	}

	/********************
	 *	4. stcchannel setting
	 ********************/
	stcChannel = DRV_DEMUX_GetStcChannelHandle();
	if(stcChannel == NULL)
	{
		PrintError("DRV_DEMUX_GetStcChannelHandle failed!\n");
		return DI_ERR_ERROR;
	}

	P_CSO_remove_common_factors(decode_rate, NEXUS_NORMAL_PLAY_SPEED, &numerator, &denominator);
	nResult = NEXUS_StcChannel_SetRate(stcChannel, numerator, denominator - 1);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_StcChannel_SetRate failed!\n");
		return DI_ERR_ERROR;
	}

	pstCsoInstance->eSpeed = eSpeed;

	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetSpeed (CSO_HANDLE pHSession, HUINT32 unCsoId, DI_MEDIA_PlaySpeed_e *peSpeed)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	UNUSED_PARAM(unCsoId);

	PrintEnter;
	*peSpeed = pstCsoInstance->eSpeed;
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_StartSession(CSO_HANDLE pHSession, HUINT32 unCsoId, HULONG ulStartTime)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	UNUSED_PARAM(unCsoId);
	UNUSED_PARAM(ulStartTime);

	PrintEnter;

	pstCsoInstance->eSpeed = eDI_MEDIA_Speed_Normal;

	++s_ulCsoStart; /* Cso Start For DRV_CSO_CheckStart()*/

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_CleanPlaypumpPidChannel (CSO_HANDLE pHSession)
{
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	NEXUS_AudioDecoderStatus audioStatus;
	NEXUS_VideoDecoderStatus videoStatus;
	NEXUS_PlaypumpHandle playpump = NULL;
	DRV_Error nDrvRet = DRV_OK;

	PrintEnter;

	audioStatus.started = FALSE;
	videoStatus.started = FALSE;

	if(pstCsoInstance == NULL)
	{
		PrintError("%s(%d) Find Cso Handle failed!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	nDrvRet = DRV_PVR_GetPlaypumpHandleByIndex(pstCsoInstance->uiCurrentPlaypumpIndex, &playpump);
	if((nDrvRet != DRV_OK) || (playpump == NULL))
	{
		PrintError("DRV_PVR_GetPlaypumpHandle failed!\n");
		return DRV_ERR;
	}

	if(playpump != NULL)
	{
		if((pstCsoInstance->hAudioPidChannel == pstCsoInstance->hPcrPidChannel) || (pstCsoInstance->hVideoPidChannel == pstCsoInstance->hPcrPidChannel))
		{
			pstCsoInstance->hPcrPidChannel = NULL;
		}
		else if(pstCsoInstance->hPcrPidChannel != NULL)
		{
			(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hPcrPidChannel);
			pstCsoInstance->hPcrPidChannel = NULL;
		}

		if(pstCsoInstance->hVideoPidChannel != NULL)
		{
			(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hVideoPidChannel);
			pstCsoInstance->hVideoPidChannel = NULL;
		}
		if(pstCsoInstance->hAudioPidChannel != NULL)
		{
			(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hAudioPidChannel);
			pstCsoInstance->hAudioPidChannel = NULL;
		}
	}

	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_StopSession (CSO_HANDLE pHSession, HUINT32 unCsoId)
{
	UNUSED_PARAM(unCsoId);
	NEXUS_Error nResult;
	NEXUS_StcChannelHandle stcChannel=NULL;
	NEXUS_StcChannelSettings stcSettings;
	DI_CSO_Instance *pstCsoInstance = (DI_CSO_Instance*)pHSession;
	NEXUS_PlaypumpHandle playpump = NULL;
	DRV_Error nDrvRet = DRV_OK;
	HUINT32 nIdx = 0;

	if(s_ulCsoStart > 0)
		--s_ulCsoStart; /* Cso Start For DRV_CSO_CheckStart()*/

	/* PidChannel Close */
	for(nIdx = 0; nIdx < DI_CSO_PUMPTYPE_ENDOFCASE; ++nIdx)
	{
		if(pstCsoInstance->stPumpInfo[nIdx].bUsed == TRUE)
		{
			nDrvRet = DRV_PVR_GetPlaypumpHandleByIndex(pstCsoInstance->stPumpInfo[nIdx].ulPlaypumpIndex, &playpump);
			if((nDrvRet != DRV_OK) || (playpump == NULL))
			{
				PrintError("DRV_PVR_GetPlaypumpHandle failed!\n");
				return DRV_ERR;
			}

			if((nIdx == DI_CSO_PUMPTYPE_A)&&(pstCsoInstance->hAudioPidChannel != NULL))
			{
				(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hAudioPidChannel);
				pstCsoInstance->hAudioPidChannel = NULL;
			}
			if((nIdx == DI_CSO_PUMPTYPE_V)&&(pstCsoInstance->hVideoPidChannel != NULL))
			{
				(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hVideoPidChannel);
				pstCsoInstance->hVideoPidChannel = NULL;
			}
			pstCsoInstance->stPumpInfo[nIdx].bUsed = FALSE;
			pstCsoInstance->stPumpInfo[nIdx].ulPlaypumpIndex = 0;
		}
	}

	nDrvRet = DRV_PVR_GetPlaypumpHandleByIndex(pstCsoInstance->uiCurrentPlaypumpIndex, &playpump);
	if((nDrvRet != DRV_OK) || (playpump == NULL))
	{
		PrintError("DRV_PVR_GetPlaypumpHandle failed!\n");
		return DRV_ERR;
	}

	if(playpump != NULL)
	{
		if((pstCsoInstance->hAudioPidChannel == pstCsoInstance->hPcrPidChannel)
				|| (pstCsoInstance->hVideoPidChannel == pstCsoInstance->hPcrPidChannel))
		{
			pstCsoInstance->hPcrPidChannel = NULL;
		}
		else if(pstCsoInstance->hPcrPidChannel != NULL)
		{
			(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hPcrPidChannel);
			pstCsoInstance->hPcrPidChannel = NULL;
		}

		if(pstCsoInstance->hVideoPidChannel != NULL)
		{
			(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hVideoPidChannel);
			pstCsoInstance->hVideoPidChannel = NULL;
		}
		if(pstCsoInstance->hAudioPidChannel != NULL)
		{
			(void)NEXUS_Playpump_ClosePidChannel (playpump, pstCsoInstance->hAudioPidChannel);
			pstCsoInstance->hAudioPidChannel = NULL;
		}
	}

	/* StcChannel normal */
	stcChannel = DRV_DEMUX_GetStcChannelHandle();
	if(stcChannel != NULL)
	{
		(void)NEXUS_StcChannel_SetRate(stcChannel, 1, 0 /* HW adds one */ );

		(void)NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0;
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		stcSettings.modeSettings.pcr.pidChannel = NULL;
		nResult = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_StcChannel_SetRate failed!\n");
			return DI_ERR_ERROR;
		}
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_PauseSession (CSO_HANDLE pHSession, HUINT32 unCsoId)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(unCsoId);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_ResumeSession (CSO_HANDLE pHSession, HUINT32 unCsoId)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(unCsoId);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetSessionSettings (CSO_HANDLE pHSession, const DI_CSO_SessionSettings_t *pSettings)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(pSettings);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetSessionSettings (CSO_HANDLE pHSession, DI_CSO_SessionSettings_t *pstSettings)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(pstSettings);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetBufferingStatus (CSO_HANDLE pHSession, DI_CSO_BufferStatus_t *pstBufferStatus)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(pstBufferStatus);

	return DI_ERR_ERROR;
}

DI_ERR_CODE DI_CSO_StartBuffering (CSO_HANDLE pHSession, HINT8 *pSaveFileName)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(pSaveFileName);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_GetBufferInfo(CSO_HANDLE pHSession, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets)
{
	UNUSED_PARAM(pHSession);
	UNUSED_PARAM(uiChunks);
	UNUSED_PARAM(iStartOffsets);
	UNUSED_PARAM(iEndOffsets);

	//	return DI_CSO_GetBufferingStatus(CSO_CSO_HANDLE pHSession)
	return DI_ERR_OK;
}

/* Initialized DTCP  */
#define ENCRYPTED_DTCPKEY_LEN		(8 + 348 + 4 + 8)
DI_ERR_CODE DI_CSO_DTCP_AcquireDtcpIpKey(HINT8 **pDtcpKey, HINT32 *nSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HINT8	*pcBuffer=NULL;
	int dtcp_result = 0;

	PrintEnter;

	pcBuffer = VK_MEM_Alloc(ENCRYPTED_DTCPKEY_LEN);
	if(pcBuffer != NULL)
	{
		VK_MEM_Memset(pcBuffer, 0x0, ENCRYPTED_DTCPKEY_LEN);
		dtcp_result = DRV_NVRAM_GetField(DI_NVRAM_FIELD_DTCP_KEY, 0, pcBuffer, ENCRYPTED_DTCPKEY_LEN);
		if (dtcp_result != DRV_OK)
		{
			PrintError("Can't read DTCP Key from NAND flash\n");
			VK_MEM_Free(pcBuffer);
			return DI_ERR_ERROR;
 		}

		/* if dtcp key is encrypted, make sure to decrypt */
		{
			if( (pcBuffer[0] == 'D') && (pcBuffer[1] == 'T') && (pcBuffer[2] == 'C') && (pcBuffer[3] == 'P') &&
				(pcBuffer[4] == '-') && (pcBuffer[5] == 'I') && (pcBuffer[6] == 'P') && (pcBuffer[7] == ':') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-8] == ':') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-7] == 'D') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-6] == 'T') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-5] == 'C') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-4] == 'P') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-3] == '-') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-2] == 'I') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-1] == 'P'))

			{
				DRV_Error	drvErr = 0;
				stCrypt_t	CryptInfo;

				unsigned long crc = 0;
				unsigned long org_crc = 0;
				unsigned char * crc_temp = NULL;

				VK_MEM_Memset((void *)&CryptInfo, 0, sizeof(stCrypt_t));
				CryptInfo.etCryptMode = DI_CRYPT_TDES_DTCPIP;
				CryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
				CryptInfo.etCryptData = DI_CRYPT_BLOCK;
				CryptInfo.pKey = NULL;

				CryptInfo.ulKeySize = 16;
				CryptInfo.pSrcBuffer = DRV_CRYPT_MemAlloc(348 + 4);
				if (CryptInfo.pSrcBuffer == NULL)
				{
					PrintError("Error DI_CRYPT_MemAlloc pSrcBuffer NULL\n");
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				VK_MEM_Memcpy((void *)CryptInfo.pSrcBuffer, (void *)(pcBuffer+8), 352);

				CryptInfo.pDstBuffer = CryptInfo.pSrcBuffer;
				if (CryptInfo.pDstBuffer == NULL)
				{
					PrintError("Error DI_CRYPT_MemAlloc pDstBuffer NULL\n");
					DRV_CRYPT_MemFree(CryptInfo.pSrcBuffer);
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				CryptInfo.ulBuffSize = 352;

				drvErr = DRV_CRYPT_Decrypt(&CryptInfo);
				if (drvErr != DRV_OK)
				{
					PrintError("Error DI_CRYPT_Encrypt\n");
					DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				crc = DRV_UTIL_GetCrc32(CryptInfo.pDstBuffer, 348);
				org_crc = htonl(crc);
				crc_temp = (unsigned char *)&org_crc;

				PrintData("[%02x %02x %02x %02x == %02x %02x %02x %02x]\n", crc_temp[0], crc_temp[1], crc_temp[2], crc_temp[3],
					CryptInfo.pDstBuffer[348 + 0], CryptInfo.pDstBuffer[348 + 1], CryptInfo.pDstBuffer[348 + 2], CryptInfo.pDstBuffer[348 + 3]);

				if(VK_memcmp(crc_temp, CryptInfo.pDstBuffer + 348, 4))
				{
					PrintError("CRC did not match.");
					DRV_CRYPT_MemFree(CryptInfo.pSrcBuffer);
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				VK_MEM_Memset(pcBuffer, 0x0, ENCRYPTED_DTCPKEY_LEN);
				VK_MEM_Memcpy(pcBuffer, (void *)CryptInfo.pSrcBuffer, 348);

				DRV_CRYPT_MemFree(CryptInfo.pSrcBuffer);
			}
			else
			{
				PrintData("Encryption Header not found\n");
			}
		}
	}

	*pDtcpKey = pcBuffer;
	*nSize = 348;

	PrintExit;

	return nRet;
}

void DI_CSO_DTCP_ReleaseDtcpIpKey(HINT8 *pDtcpKey)
{
	if(pDtcpKey!=NULL)
	{
		VK_MEM_Free(pDtcpKey);
	}
}

DI_ERR_CODE DI_CSO_DTCP_Init ( void )
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetCurrentPlaypump(CSO_HANDLE pHSession, HUINT32 uiIndex)
{
	DRV_Error dResult = DRV_OK;
	NEXUS_Error nResult;
	NEXUS_PlaypumpHandle playpump = NULL;
	NEXUS_PlaypumpSettings playpumpSettings;

	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;
	pstCsoInstance->uiCurrentPlaypumpIndex = uiIndex;

	dResult = DRV_PVR_GetPlaypumpHandleByIndex(uiIndex, &playpump);
	if((dResult != DRV_OK) || (playpump == NULL))
	{
		PrintError("DRV_PVR_GetPlaypumpHandle failed! pumphandle:%08X\n", (HUINT32)playpump);
		return DI_ERR_ERROR;
	}
	/* Clear Playpump Setting */
	(void)NEXUS_Playpump_GetDefaultSettings(&playpumpSettings);
	nResult = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Playpump_SetSettings failed!\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_SetPlaypumpType(CSO_HANDLE pHSession, HUINT32 ulIndex, DI_CSO_PumpType_e ePumpType)
{
	DRV_Error dResult = DRV_OK;
	NEXUS_Error nResult;
	NEXUS_PlaypumpHandle playpump = NULL;
	NEXUS_PlaypumpSettings playpumpSettings;
	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;

	pstCsoInstance->stPumpInfo[ePumpType].ulPlaypumpIndex = ulIndex;
	pstCsoInstance->stPumpInfo[ePumpType].bUsed = TRUE;

	dResult = DRV_PVR_GetPlaypumpHandleByIndex(ulIndex , &playpump);
	if((dResult != DRV_OK) || (playpump == NULL))
	{
		PrintError("DRV_PVR_GetPlaypumpHandle failed! pumphandle:%08X\n", (HUINT32)playpump);
		return DI_ERR_ERROR;
	}
	/* Clear Playpump Setting */
	(void)NEXUS_Playpump_GetDefaultSettings(&playpumpSettings);
	nResult = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Playpump_SetSettings failed!\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

void DI_CSO_SetFirstPosition(CSO_HANDLE pHSession, HUINT64 ullFirstPosition)
{
	NEXUS_Error nResult;
	NEXUS_StcChannelHandle stcChannel=NULL;
	unsigned denominator,numerator;
	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;

	/********************
	 *	1. Set FirstPosition
	 ********************/
	pstCsoInstance->ullFirstPosition = ullFirstPosition; /* adjust PlayTime */

	/********************
	 *	2. Init StcSetting
	 ********************/
	stcChannel = DRV_DEMUX_GetStcChannelHandle();
	if(stcChannel == NULL)
	{
		PrintError("DRV_DEMUX_GetStcChannelHandle failed!\n");
		return;
	}

	P_CSO_remove_common_factors(1000, NEXUS_NORMAL_PLAY_SPEED, &numerator, &denominator);
	nResult = NEXUS_StcChannel_SetRate(stcChannel, numerator, denominator - 1);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_StcChannel_SetRate failed!\n");
		return;
	}
}

void DI_CSO_SetInputStreamType(CSO_HANDLE pHSession, DI_CSO_InputStreamType_e eInputStreamType)
{
	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;

	pstCsoInstance->eInputStreamType = eInputStreamType;
}

void DI_CSO_SetDuration(CSO_HANDLE pHSession, HUINT64 ullDuration)
{

	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;

	/********************
	 *	1. Set Duration
	 ********************/
	pstCsoInstance->ullDuration = ullDuration - (ullDuration%1000);

	return;
}

DI_ERR_CODE DI_CSO_GetCurrentStatus(CSO_HANDLE pHSession, HBOOL* bBOF, HBOOL* bEOF)
{
	HBOOL bValidAudioPosition=FALSE;
	HBOOL bValidVideoPosition=FALSE;
	NEXUS_Error nResult;
	NEXUS_AudioDecoderStatus audioStatus;
	NEXUS_VideoDecoderStatus videoStatus;
	HUINT64 ullCurrentPosition=0;
	HUINT32 uiCdbDepth=0;
	HULONG ulCurrentSystemTick = 0;
	HULONG ulPreviousTickGap = 0;;
	NEXUS_VideoDecoderTrickState pVideoTrickState;
	HBOOL bTrickMode = FALSE;
	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;

	if((bBOF == NULL) || (bEOF == NULL))
	{
		PrintError("[%s][%d] Invalid Parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(pstCsoInstance == NULL)
	{
		PrintError("[%s][%d] Current Cso Handle is NULL!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	audioStatus.started = FALSE;
	videoStatus.started = FALSE;

	/* fix prevent issue #229940 */
	videoStatus.pts = 0;	

	/********************
	 *	1. Position Check
	 ********************/
	if(pstCsoInstance->hVideoDecoder != NULL)
	{
		nResult = NEXUS_VideoDecoder_GetStatus(pstCsoInstance->hVideoDecoder, &videoStatus);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s][%d] NEXUS_VideoDecoder_GetStatus failed!\n", __FUNCTION__,__LINE__);
			return DI_ERR_ERROR;
		}

		if(videoStatus.started == TRUE)
		{
			bValidVideoPosition = TRUE;
			ullCurrentPosition = ((uint64_t)videoStatus.pts)*NEXUS_BMEDIA_TIME_SCALE_BASE/(NEXUS_BMEDIA_PTS_SCALE*NEXUS_BMEDIA_TIME_SCALE_BASE);
			uiCdbDepth = videoStatus.fifoDepth;
		}

		(void)NEXUS_VideoDecoder_GetTrickState(pstCsoInstance->hVideoDecoder, &pVideoTrickState );
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s][%d] NEXUS_VideoDecoder_GetTrickState failed!\n", __FUNCTION__,__LINE__);
			return DI_ERR_ERROR;
		}

		if(pVideoTrickState.decodeMode == NEXUS_VideoDecoderDecodeMode_eI)
		{
			bTrickMode = TRUE;
		}
	}

	//if(bValidVideoPosition == FALSE)
	{
		if(pstCsoInstance->hAudioDecoder != NULL)
		{
			nResult = NEXUS_AudioDecoder_GetStatus(pstCsoInstance->hAudioDecoder, &audioStatus);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("[%s][%d] NEXUS_AudioDecoder_GetStatus failed!\n", __FUNCTION__,__LINE__);
				return DI_ERR_ERROR;
			}

			if(audioStatus.started == TRUE)
			{
				bValidAudioPosition = TRUE;
				if(videoStatus.pts < audioStatus.pts)
				{
					ullCurrentPosition = ((uint64_t)audioStatus.pts)*NEXUS_BMEDIA_TIME_SCALE_BASE/(NEXUS_BMEDIA_PTS_SCALE*NEXUS_BMEDIA_TIME_SCALE_BASE);
					uiCdbDepth = audioStatus.fifoDepth;
				}
			}
		}
	}

	if((int64_t)(ullCurrentPosition - pstCsoInstance->ullFirstPosition) > 0)
	{
		ullCurrentPosition = ullCurrentPosition-pstCsoInstance->ullFirstPosition;
	}
	else
	{
		ullCurrentPosition = 0;
	}

	//ullCurrentPosition -= (ullCurrentPosition%1000);
	/********************
	 *	2. Direction Check
	 ********************/
	if((bValidAudioPosition == TRUE) || (bValidVideoPosition == TRUE))
	{
		if(pstCsoInstance->ullPreviousPosition != ullCurrentPosition)
		{
			if(pstCsoInstance->ullPreviousPosition < ullCurrentPosition)
			{
				pstCsoInstance->bDirection = TRUE;
			}
			else if(pstCsoInstance->ullPreviousPosition > ullCurrentPosition)
			{
				pstCsoInstance->bDirection = FALSE;
			}
		}
	}
	else
	{
		PrintError("AudioDecoder Handle: %s\nVideoDecoder Handle: %s\nAudio Started: %s\nVideo Started: %s\nNo Active Decoders!\n",
				pstCsoInstance->hAudioDecoder?"T":"F",
				pstCsoInstance->hVideoDecoder?"T":"F",
				audioStatus.started?"T":"F",
				videoStatus.started?"T":"F");
		return DI_ERR_ERROR;
	}

	/********************
	 *	3. Check BOF
	 ********************/
	if((ullCurrentPosition == 0) &&
			(pstCsoInstance->bDirection == FALSE) &&
			(pstCsoInstance->ullPreviousPosition != ullCurrentPosition) &&
			(uiCdbDepth != 0) &&
			(bTrickMode != TRUE))
	{
		PrintDebug("[di_calypso] %s():L(%d). BOF Detection!!!", __FUNCTION__, __LINE__);
		*bBOF = TRUE;
	}
	else
	{
		*bBOF = FALSE;
	}

	/********************
	 *	4. Check EOF
	 ********************/
	ulCurrentSystemTick = VK_TIMER_GetSystemTick();
	if((pstCsoInstance->ullDuration != 0) && ((ullCurrentPosition+1000) >= pstCsoInstance->ullDuration) && (pstCsoInstance->bDirection == TRUE))
	{
		if(pstCsoInstance->uiPreviousCdbDepth == uiCdbDepth)
		{
			if (pstCsoInstance->ulPreviousSystemTick && ulCurrentSystemTick > pstCsoInstance->ulPreviousSystemTick)
			{
				/* milliseconds */
				ulPreviousTickGap = ulCurrentSystemTick - pstCsoInstance->ulPreviousSystemTick;
			}

			if (pstCsoInstance->ullPreviousPosition + ulPreviousTickGap > pstCsoInstance->ullDuration)
			{
				if(pstCsoInstance->uiEofGauge < EOF_DETECT_THRESHOLD)
				{
					pstCsoInstance->uiEofGauge = EOF_DETECT_THRESHOLD + 1;
					PrintDebug("[di_calypso] %s():L(%d). EOF Detection!!! ullPreviousPosition(%lld), ullCurrentPosition(%lld), ulPreviousTickGap(%ld)\n",
							__FUNCTION__,__LINE__, pstCsoInstance->ullPreviousPosition, ullCurrentPosition, ulPreviousTickGap);
					*bEOF = TRUE;
				}
				else
				{
					*bEOF = FALSE;
				}
			}
			else if ( pstCsoInstance->ullPreviousPosition == ullCurrentPosition)
			{
				if(pstCsoInstance->uiEofGauge < EOF_DETECT_THRESHOLD)
				{
					pstCsoInstance->uiEofGauge++;
					*bEOF = FALSE;
				}
				else if(pstCsoInstance->uiEofGauge == EOF_DETECT_THRESHOLD)
				{
					PrintDebug("[di_calypso] %s():L(%d). EOF Gauge Detection!!! ullPreviousPosition(%lld), ullCurrentPosition(%lld), ulPreviousTickGap(%ld)\n",
							__FUNCTION__,__LINE__, pstCsoInstance->ullPreviousPosition, ullCurrentPosition, ulPreviousTickGap);
					pstCsoInstance->uiEofGauge++;
					*bEOF = TRUE;
				}
				else
				{
					*bEOF = FALSE;
				}
			}
		}
		else
		{
			if((audioStatus.started == TRUE || audioStatus.started == TRUE) &&  (pstCsoInstance->ullDuration == 0))
			{
				PrintDebug("Cso Duration is 0\n");
			}
			*bEOF = FALSE;
			pstCsoInstance->uiEofGauge = 0;
		}
	}
	else
	{
		*bEOF = FALSE;
		pstCsoInstance->uiEofGauge = 0;
	}

	pstCsoInstance->ulPreviousSystemTick = ulCurrentSystemTick;
	pstCsoInstance->ullPreviousPosition = ullCurrentPosition;
	pstCsoInstance->uiPreviousCdbDepth = uiCdbDepth;
	return DI_ERR_OK;
}

CSO_HANDLE DI_CSO_GetCurrentCsoInstance(void)
{
	return g_pCurrentSession;
}

void DI_CSO_SetCurrentCsoInstance(CSO_HANDLE pHSession)
{
	UNUSED_PARAM(pHSession);
	g_pCurrentSession = pHSession;
}

DI_ERR_CODE DI_CSO_Flush(CSO_HANDLE pHSession)
{
	NEXUS_Error nResult;
	NEXUS_StcChannelHandle hStc;
	UNUSED_PARAM(pHSession);

	PrintEnter;

	hStc = DRV_DEMUX_GetStcChannelHandle();
	if(hStc == NULL)
	{
		PrintError("[%s][%d] DRV_CSO_MP_GetPlaybackStcChannelHandle failed!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	nResult = NEXUS_StcChannel_Invalidate(hStc);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s][%d] NEXUS_StcChannel_Invalidate failed!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_CSO_TrickMode(CSO_HANDLE pHSession, HBOOL bEnable)
{
	NEXUS_Error nResult;

	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;
	PrintEnter;

	NEXUS_VideoDecoderTrickState tSettings;

	if(pstCsoInstance->hVideoDecoder == NULL)
	{
		PrintDebug("[%s][%d] Info: Video Handle is NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_OK;
	}

	(void)NEXUS_VideoDecoder_GetTrickState(pstCsoInstance->hVideoDecoder, &tSettings);

	PrintDebug("[%s][%d] Trick Mode : %s\n", __FUNCTION__, __LINE__, bEnable == TRUE ? "TRUE" : "FALSE");
	if(bEnable == TRUE)
	{
		tSettings.decodeMode = NEXUS_VideoDecoderDecodeMode_eI;
		tSettings.hostTrickModesEnabled = true;
		tSettings.tsmEnabled = NEXUS_TsmMode_eDisabled;
	}
	else
	{
		tSettings.decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
		tSettings.hostTrickModesEnabled = false;
		tSettings.tsmEnabled = NEXUS_TsmMode_eEnabled;
	}

	nResult = NEXUS_VideoDecoder_SetTrickState(pstCsoInstance->hVideoDecoder, &tSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s][%d] NEXUS_VideoDecoder_SetTrickState failed!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	PrintExit;
	return DI_ERR_OK;
}

void DI_CSO_SetDemuxId(CSO_HANDLE pHSession, HUINT32 ulDemuxId)
{
	DI_CSO_Instance* pstCsoInstance = (DI_CSO_Instance*)pHSession;
	PrintError("[%s][%d] +ulDemuxId : %u\n", __FUNCTION__,__LINE__, ulDemuxId);
	pstCsoInstance->ulDemuxId = ulDemuxId;
	PrintError("[%s][%d] -ulDemuxId : %u\n", __FUNCTION__,__LINE__, pstCsoInstance->ulDemuxId);
}

