/*******************************************************************/
/**
 * @file	di_transcoder.c
 *
 * Source Code for transcoder Module
 *
 * @author	HyunSeok Choi (hs.choi@humaxdigital.com)
 * @data	2012/05/08
 * @attention	Copyright (c) 2012 Humax - All rights reserved
 *
 **/
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "taskdef.h"
#include "drv_err.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_video_enc.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "drv_audio_enc.h"
#include "drv_stream_mux.h"
#include "di_transcoder.h"
#include "di_demux.h"
#include "drv_demux.h"
#include "di_uart.h"
#include "drv_pvr.h"
#include "nexus_stc_channel.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

//#define DI_TRANSCODER_DEBUG
//#define DI_TRANSCODER_ENTER_LEAVE_DEBUG
//#define DI_TRANSCODER_SEE_PARAMETERS

#ifndef DEBUG_MODULE
#ifdef DI_TRANSCODER_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#endif

#ifdef DI_TRANSCODER_ENTER_LEAVE_DEBUG
#define PrintEnter()	DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()	DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()
#define PrintExit()
#endif
#endif /* #ifndef DEBUG_MODULE */

#define PrintStatus(...)	DI_UART_Print (__VA_ARGS__)

#define UNREFENCED_PARAM(x) (x=x)

#if defined(CONFIG_PIP)
#define NUM_OF_TRANSCODER 1 /* Support single transcoder */
#else
#define NUM_OF_TRANSCODER 2
#endif

#define DI_TRANSCODER_DEFAULT_TIMEOUT 3000; /* 3 sec timeout for Probing */
#define DI_TRANSCODER_ADJUST_AUDIO_DELAY 810000; /*30*27000 = 810000 */
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct _CALLBACK_INFO
{
	EVENT_CALLBACK_FUNCTION	pfnCallback;
} CALLBACK_INFO;

typedef struct TranscoderInst
{
	unsigned long 	ulSemId_DI;
	unsigned long 	ulSemId_DRV;
	HBOOL	bStart;
	HBOOL	bInitialized;

	HUINT32 ulDemuxId;
	DI_TRANSCODER_SOURCE_TYPE eTransSourceType;
	DI_TRANSCODER_ENCODE_MODE eTransEncodeMode;

	DRV_VIDEO_ENC_Set_t VidEncSettings;
	DRV_AUDIO_ENC_Set_t	AudEncSettings;
	CALLBACK_INFO stCallbackInfo[DI_TRANSCODER_NOTIFY_EVT_END];
	HUINT32 ulProbeTimeout;
	unsigned long ulProbeCheckId;

} DRV_TranscoderInst_t;

extern DRV_Error DRV_DEMUX_GetOutPutDeviceType(HUINT32 ulDemuxId , DI_DEMUX_OUTPUTDEVICETYPE * etConsumer);
extern DRV_Error DRV_DEMUX_GetInputDevice(HUINT32 ulDemuxId , DI_DEMUX_INPUTDEVICETYPE * etProducer, HUINT32 * ulDeviceId);

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
static DRV_TranscoderInst_t	s_stTranscoder[NUM_OF_TRANSCODER];
static DI_TRANSCODER_CAP_t	s_stTranscoderCap[NUM_OF_TRANSCODER] =
{
	{ 0,	0,	DI_TRANSCODER_SOURCE_WATCH | DI_TRANSCODER_SOURCE_BACKGROUND ,
	DI_VIDEO_FORMAT_MPEG2 | DI_VIDEO_FORMAT_MPEG4Part2 | DI_VIDEO_FORMAT_H264 | DI_VIDEO_FORMAT_VC1,
	DI_AUDFORMAT_MPEG | DI_AUDFORMAT_MP3 | DI_AUDFORMAT_AAC | DI_AUDFORMAT_AAC_ADTS | DI_AUDFORMAT_AAC_LOAS | DI_AUDFORMAT_AAC_PLUS| \
	DI_AUDFORMAT_AAC_PLUS_LOAS | DI_AUDFORMAT_AAC_PLUS_ADTS | DI_AUDFORMAT_AC3 | DI_AUDFORMAT_AC3_PLUS | DI_AUDFORMAT_PCM,
	DI_TRANSCODER_MUX_TS | DI_TRANSCODER_MUX_MP4 | DI_TRANSCODER_MUX_ASF }
#if (NUM_OF_TRANSCODER == 2)
,
	{ 0,	1,	DI_TRANSCODER_SOURCE_WATCH | DI_TRANSCODER_SOURCE_BACKGROUND,
	DI_VIDEO_FORMAT_MPEG2 | DI_VIDEO_FORMAT_MPEG4Part2 | DI_VIDEO_FORMAT_H264 | DI_VIDEO_FORMAT_VC1,
	DI_AUDFORMAT_MPEG | DI_AUDFORMAT_MP3 | DI_AUDFORMAT_AAC | DI_AUDFORMAT_AAC_ADTS | DI_AUDFORMAT_AAC_LOAS | DI_AUDFORMAT_AAC_PLUS| \
	DI_AUDFORMAT_AAC_PLUS_LOAS | DI_AUDFORMAT_AAC_PLUS_ADTS | DI_AUDFORMAT_AC3 | DI_AUDFORMAT_AC3_PLUS | DI_AUDFORMAT_PCM,
	DI_TRANSCODER_MUX_TS | DI_TRANSCODER_MUX_MP4 | DI_TRANSCODER_MUX_ASF }
#endif
};

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_TRANSCODER_NOTIFY_EVT eEvent
 * @param[in] void *pvEventData
 * @return void
 * @par requirements
 * @see APIs
*/
static void P_TRANSCODER_EVTCall(HUINT32 ulDeviceId, DI_TRANSCODER_NOTIFY_EVT eEvent, void *pvEventData)
{

	DI_TRANSCODER_CALLBACK_DATA_t	stCallback;

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized == FALSE)
	{
		PrintError("[%s:%d] ulDeviceId error (ulDeviceId = %d)\n", __func__, __LINE__, ulDeviceId);
		return;
	}

	if(s_stTranscoder[ulDeviceId].stCallbackInfo[eEvent].pfnCallback != NULL)
	{
		stCallback.ulDeviceId = ulDeviceId;
		stCallback.eEvent = eEvent;
		stCallback.pvEventData = pvEventData;
		(*s_stTranscoder[ulDeviceId].stCallbackInfo[eEvent].pfnCallback)(&stCallback);
	}
	else
	{
		PrintError("[%s] eEvent = %d (ID=%d) is not registered !!!\n",__func__,eEvent,ulDeviceId);
	}

	return;
}

static void P_TRANSCODER_EncProbeCheck(unsigned long TimerId, void *params)
{
	HUINT32 ulDeviceId = *(HUINT32 *)params;
	UNREFENCED_PARAM(TimerId);
	P_TRANSCODER_EVTCall(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_FAIL, NULL);
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DRV_VIDEO_NOTIFY_ENC eEvent
 * @param[in] void *pvParam
 * @return void
 * @par requirements
 * @see APIs
*/
static void DRV_TRANSCODER_VIDEO_ENC_CB(HUINT32 ulDeviceId, DRV_VIDEO_NOTIFY_ENC eEvent, void *pvParam)
{
	UNREFENCED_PARAM(pvParam);
	switch(eEvent)
	{
		case DRV_VIDEO_NOTIFY_ENC_DEC_SEQ_INFO:
			P_TRANSCODER_EVTCall(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_SEQUENCE_HEADER_INFO, NULL);
			break;
		case DRV_VIDEO_NOTIFY_ENC_DEC_PROBE:
			VK_TIMER_Cancel(s_stTranscoder[ulDeviceId].ulProbeCheckId);
			s_stTranscoder[ulDeviceId].ulProbeCheckId = VK_TIMERID_NULL;
			P_TRANSCODER_EVTCall(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS, NULL);
			break;
		default:
			break;
	}
}

/**
 * @param[in] HUINT32 *pulNumOfDevice
 * @param[in] DI_TRANSCODER_CAP_t **ppCapInfo
 * @param[out]	DI_ERR_CODE
 * @return success DI_ERR_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_TRANSCODER_Init(HUINT32 ulDeviceId)
{

	int nRet;
	DRV_Error DrvErr=DRV_OK;
	char ucSemName[20];

	VK_MEM_Memset(ucSemName, 0x00, 20);
	VK_snprintf(ucSemName, 18, "di_transcoder%d_di", ulDeviceId);

	if(ulDeviceId >= NUM_OF_TRANSCODER)
	{
		PrintError("[%s] %d idx is not supported.\n\r", __func__,ulDeviceId);
		return DRV_ERR;
	}

	nRet = VK_SEM_Create(&s_stTranscoder[ulDeviceId].ulSemId_DI, ucSemName, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		DrvErr=DRV_ERR;
		goto err;
	}
	VK_MEM_Memset(ucSemName, 0x00, 20);
	VK_snprintf(ucSemName, 19, "di_transcoder%d_drv", ulDeviceId);
	nRet = VK_SEM_Create(&s_stTranscoder[ulDeviceId].ulSemId_DRV, ucSemName, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		DrvErr=DRV_ERR;
		goto err;
	}

	DrvErr=DRV_VIDEO_EncInit(ulDeviceId);
	if(DrvErr != DRV_OK)
	{
		PrintError("[%s] fail video encoder init (%d)\n",__func__,ulDeviceId);
		goto err;
	}

	DrvErr=DRV_STREAM_MUX_Init(ulDeviceId);
	if(DrvErr != DRV_OK)
	{
		PrintError("[%s] fail stream mux init (%d)\n",__func__,ulDeviceId);
		goto err;
	}

	if(ulDeviceId == 0)
	{
		DrvErr=DRV_AUDIO_EncInit();
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] fail stream mux init (%d)\n",__func__,ulDeviceId);
			goto err;
		}
	}

	if(ulDeviceId == 0)
		DRV_VIDEO_EncRegisterEventCallback(DRV_TRANSCODER_VIDEO_ENC_CB);

	s_stTranscoder[ulDeviceId].bInitialized = TRUE;
	s_stTranscoder[ulDeviceId].bStart = FALSE;
	s_stTranscoder[ulDeviceId].ulProbeTimeout = DI_TRANSCODER_DEFAULT_TIMEOUT;
	s_stTranscoder[ulDeviceId].ulProbeCheckId = VK_TIMERID_NULL;

	return DrvErr;
err:
	VK_SEM_Destroy(s_stTranscoder[ulDeviceId].ulSemId_DRV);
	VK_SEM_Destroy(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DrvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 ulDemuxId
 * @param[in] DI_TRANSCODER_SOURCE_TYPE eTranscoderSourceType
 * @return success DRV_OK fail DRV_ERR
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_TRANSCODER_SetStcChannel(HUINT32 ulDeviceId, HUINT32 ulDemuxId, DI_TRANSCODER_SOURCE_TYPE eTransSourceType)
{

	NEXUS_StcChannelSettings stcSettings;
	HUINT32 ulFirstPlaybackPbIdx=0;
	NEXUS_StcChannelHandle pStcChannel;
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	HUINT32 ulStcChannelIdx=0;
#endif
	DRV_Error DrvErr=DRV_OK;
	int nRet=VK_OK;

	PrintEnter();

	PrintDebug("[%s:%d] ulDeviceId = %d, ulDemuxId = %d, eTransSourceType = %d\n", __func__, __LINE__, ulDeviceId, ulDemuxId, eTransSourceType);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized == FALSE || s_stTranscoder[ulDeviceId].bStart == TRUE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DRV_ERR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DRV);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema \n",__func__);
		return DRV_ERR;
	}

	if(eTransSourceType == DI_TRANSCODER_SOURCE_BACKGROUND)
	{
		ulFirstPlaybackPbIdx = DRV_DEMUX_GetFirstPlaybackParserIndex();

		PrintDebug("[%s:%d] DRV_DEMUX_GetSTCChannel(%d)\n", __func__, __LINE__, ulDeviceId);
		pStcChannel = DRV_DEMUX_GetSTCChannel(ulDeviceId+1);

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
		DrvErr = DRV_DEMUX_GetStcChannelIndex(pStcChannel, &ulStcChannelIdx);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] failed to get STC channel index\n", __func__);
			goto err;
		}
		PrintDebug("[%s] STC handle(0x%x), STC index(%d), ulDeivceId(%d).\n", __func__, pStcChannel, ulStcChannelIdx, ulDeviceId);

		NEXUS_StcChannel_GetDefaultSettings(ulStcChannelIdx, &stcSettings);

		if(ulDemuxId >= ulFirstPlaybackPbIdx) // for playback or playback routing
		{
			stcSettings.timebase = NEXUS_Timebase_e0 + ulStcChannelIdx;
			stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
			stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
		}
		else
		{
			stcSettings.timebase = NEXUS_Timebase_e0 + ulStcChannelIdx;
			stcSettings.mode = NEXUS_StcChannelMode_ePcr;
			if(ulDeviceId == 0)
				stcSettings.modeSettings.pcr.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_PCR_TRANS_1);
			else
				stcSettings.modeSettings.pcr.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_PCR_TRANS_2);

		}
#else
		NEXUS_StcChannel_GetDefaultSettings((ulDeviceId * 2) + 1, &stcSettings);

		if(ulDemuxId >= ulFirstPlaybackPbIdx) // for playback or playback routing
		{
			stcSettings.timebase = NEXUS_Timebase_e0 + ((ulDeviceId * 2) +1 );
			stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
			stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
		}
		else
		{
			stcSettings.timebase = NEXUS_Timebase_e0 + ((ulDeviceId * 2) +1 );
			stcSettings.mode = NEXUS_StcChannelMode_ePcr;
			if(ulDeviceId == 0)
				stcSettings.modeSettings.pcr.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_PCR_TRANS_1);
			else
				stcSettings.modeSettings.pcr.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_PCR_TRANS_2);

		}
#endif

		DrvErr = NEXUS_StcChannel_SetSettings(pStcChannel, &stcSettings);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] failed connecting PCR channel to STC\n", __func__);
			goto err;
		}
		if(ulDemuxId >= ulFirstPlaybackPbIdx) // for playback or playback routing
		{
			DI_DEMUX_OUTPUTDEVICETYPE eOutputType;
			HUINT32 unPbDevId = ulDeviceId+1;
			
			PrintDebug("[%s:%d] DRV_PVR_PLAY_SetStcChannelHandle(%d)\n", __func__, __LINE__, ulDeviceId);
			DrvErr = DRV_DEMUX_GetOutPutDeviceType(ulDemuxId,&eOutputType);
			if((DrvErr == DRV_OK)&&(eOutputType == DI_DEMUX_OUTPUT_TRANSCODER))
			{
				DI_DEMUX_INPUTDEVICETYPE etProducer;
				HUINT32 ulInputDeviceId;
				DrvErr = DRV_DEMUX_GetInputDevice(ulDemuxId , &etProducer, &ulInputDeviceId);
				if((DrvErr == DRV_OK)&&(etProducer == DI_DEMUX_INPUT_PLAYER))
				{
					unPbDevId = ulInputDeviceId;
				}
			}
			
			DrvErr = DRV_PVR_PLAY_SetStcChannelHandle(unPbDevId, pStcChannel);
			if(DrvErr != DRV_OK)
			{
				PrintError("[%s] stc channel set fail\n", __func__);
			}
		}
	}

err:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DRV);
	return DrvErr;

}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 ulDemuxId
 * @param[in] DI_TRANSCODER_SOURCE_TYPE eTranscoderSourceType
 * @return success DRV_OK fail DRV_ERR
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_TRANSCODER_UnSetStcChannel(HUINT32 ulDeviceId, HUINT32 ulDemuxId, DI_TRANSCODER_SOURCE_TYPE eTransSourceType)
{
	HUINT32 ulFirstPlaybackPbIdx=0;
	DRV_Error DrvErr=DRV_OK;
	int nRet=VK_OK;

	PrintEnter();

	PrintDebug("[%s:%d] ulDeviceId = %d, ulDemuxId = %d, eTransSourceType = %d\n", __func__, __LINE__, ulDeviceId, ulDemuxId, eTransSourceType);

	if ((ulDeviceId>=NUM_OF_TRANSCODER) || s_stTranscoder[ulDeviceId].bInitialized == FALSE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DRV_ERR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DRV);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema \n",__func__);
		return DRV_ERR;
	}

	if(eTransSourceType == DI_TRANSCODER_SOURCE_BACKGROUND)
	{
		ulFirstPlaybackPbIdx = DRV_DEMUX_GetFirstPlaybackParserIndex();

		PrintDebug("[%s:%d] ulFirstPlaybackPbIdx (%d)", __func__, __LINE__, ulFirstPlaybackPbIdx);

		if(ulDemuxId >= ulFirstPlaybackPbIdx) // for playback or playback routing
		{
			DI_DEMUX_OUTPUTDEVICETYPE eOutputType;
			HUINT32 unPbDevId = ulDeviceId+1;
		
			PrintDebug("[%s:%d] DRV_PVR_PLAY_UnsetStcChannelHandle (%d)", __func__, __LINE__, ulDeviceId);
			DrvErr = DRV_DEMUX_GetOutPutDeviceType(ulDemuxId,&eOutputType);
			if((DrvErr == DRV_OK)&&(eOutputType == DI_DEMUX_OUTPUT_TRANSCODER))
			{
				DI_DEMUX_INPUTDEVICETYPE etProducer;
				HUINT32 ulInputDeviceId;
				DrvErr = DRV_DEMUX_GetInputDevice(ulDemuxId , &etProducer, &ulInputDeviceId);
				if((DrvErr == DRV_OK)&&(etProducer == DI_DEMUX_INPUT_PLAYER))
				{
					unPbDevId = ulInputDeviceId;
				}
			}
			
			DrvErr = DRV_PVR_PLAY_UnsetStcChannelHandle(unPbDevId);
			if(DrvErr != DRV_OK)
			{
				PrintError("[%s] stc channel unset fail\n", __func__);
			}
		}
	}

	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DRV);
	return DrvErr;
}

DRV_Error DRV_TRANSCODER_GetEncodeMode(HUINT32 ulDeviceId, DI_TRANSCODER_ENCODE_MODE *pEncodeMode)
{
	DRV_Error DrvErr=DRV_OK;

	PrintEnter();

	*pEncodeMode= s_stTranscoder[ulDeviceId].eTransEncodeMode;

	PrintExit();

	return DrvErr;
}

DRV_Error DRV_TRANSCODER_GetMuxDuration(HUINT32 ulDemuxId,HUINT32 *ulCount)
{
	DRV_Error DrvErr=DRV_OK;
	HUINT32 ulIndex = 0;

	for(ulIndex = 0; ulIndex < NUM_OF_TRANSCODER; ulIndex++)
	{
		if((s_stTranscoder[ulIndex].bStart == TRUE)&&(s_stTranscoder[ulIndex].ulDemuxId == ulDemuxId))
		{
			DrvErr = DRV_STREAM_MUX_GetDuration(ulIndex,ulCount);
			if(DrvErr == DRV_OK)
			{
				return DRV_OK;
			}
			else
			{
				return DRV_ERR;
			}
		}
	}
	return DRV_ERR;
}

/**
 * @param[in] HUINT32 *pulNumOfDevice
 * @param[in] DI_TRANSCODER_CAP_t **ppCapInfo
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_GetCapability(HUINT32 *pulNumOfDevice, DI_TRANSCODER_CAP_t **ppCapInfo)
{
	HUINT32 unCount;

	PrintEnter();

	*pulNumOfDevice	= NUM_OF_TRANSCODER;
	for(unCount=0 ; unCount< NUM_OF_TRANSCODER; unCount++)
	{
		ppCapInfo[unCount]	= &s_stTranscoderCap[unCount];
	}

	PrintDebug("[%s] Currently, the number of the transcoder is %d\n",__func__,NUM_OF_TRANSCODER);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_TRANSCODER_SetStereo(HUINT32 ulDeviceId, DI_AUDIO_STEREO_MODE dualMode)
{
	DI_ERR_CODE DiErr=DI_ERR_OK;
	DRV_AUDIO_ENC_STEREO_MODE eAudioMode;
	DRV_Error DrvErr=DRV_ERR;
	int nRet=VK_OK;

	PrintEnter();
	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized == FALSE )
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema \n",__func__);
		return DI_ERR_ERROR;
	}
	switch(dualMode)
	{
		case DI_AUDIO_DUAL_Stereo:
			eAudioMode = DRV_AUDIO_ENC_Stereo;
			break;
		case DI_AUDIO_DUAL_LeftOnly:
			eAudioMode = DRV_AUDIO_ENC_LeftOnly;
			break;
		case DI_AUDIO_DUAL_RightOnly:
			eAudioMode = DRV_AUDIO_ENC_RightOnly;
			break;
/*
		case DI_AUDIO_DUAL_Mix:
			eAudioMode = DRV_AUDIO_ENC_Mix;
			break;
*/
		default:
			PrintError("[%s] does not support (ulDeviceId = %d)\n",__func__,ulDeviceId);
			DiErr=DI_ERR_ERROR;
			goto done;
			break;
	}
	DrvErr=DRV_AUDIO_EncSetStereo(ulDeviceId, eAudioMode);
	if(DrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_AUDIO_EncSetStereo (dualMode = %d)\n",__func__,dualMode);
		DiErr=DI_ERR_ERROR;
		goto done;
	}


done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DiErr;
}


/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_TRANSCODER_Settings_t *Settings
 * @return success DI_ERR_OK  fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_SetSettings(HUINT32 ulDeviceId, DI_TRANSCODER_Settings_t *Settings)
{

	DRV_AUDIO_ENC_Set_t AudEncSettings;
	DRV_Error DrvErr=DRV_ERR;
	DI_ERR_CODE DiErr=DI_ERR_OK;
	int nRet=VK_OK;

	PrintEnter();

	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized == FALSE || s_stTranscoder[ulDeviceId].bStart == TRUE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema \n",__func__);
		return DI_ERR_ERROR;
	}

	VK_memset(&AudEncSettings,0x0, sizeof(AudEncSettings));

#if defined(DI_TRANSCODER_SEE_PARAMETERS)
	DI_UART_Print("[%s]Settings->eTransSourceType     = 0x%x \n",__func__, Settings->eTransSourceType);
	DI_UART_Print("[%s]Settings->eTransEncodeMode     = 0x%x \n",__func__, Settings->eTransEncodeMode);
	DI_UART_Print("[%s]Settings->eInputVideoFormat    = 0x%x \n",__func__, Settings->eInputVideoFormat);
	DI_UART_Print("[%s]Settings->eOutputVideoFormat   = 0x%x \n",__func__, Settings->eOutputVideoFormat);
	DI_UART_Print("[%s]Settings->eInputAudioFormat   = 0x%x \n",__func__, Settings->eInputAudioFormat);
	DI_UART_Print("[%s]Settings->eOutputAudioFormat   = 0x%x \n",__func__, Settings->eOutputAudioFormat);
	DI_UART_Print("[%s]Settings->bVideoDefaultProfile = 0x%x \n",__func__, Settings->bVideoDefaultProfile);
	DI_UART_Print("[%s]Settings->eCodecLevel          = 0x%x \n",__func__, Settings->eCodecLevel);
	DI_UART_Print("[%s]Settings->eCodecProfile        = 0x%x \n",__func__, Settings->eCodecProfile);

	DI_UART_Print("[%s] Settings->stVideoSettings.eRes                             = 0x%x\n",__func__,Settings->stVideoSettings.eRes);
	DI_UART_Print("[%s] Settings->stVideoSettings.eFrameRate                       = 0x%x\n",__func__,Settings->stVideoSettings.eFrameRate);
	DI_UART_Print("[%s] Settings->stVideoSettings.eAspectRatio                     = 0x%x\n",__func__,Settings->stVideoSettings.eAspectRatio);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomRes.bCustomRes           = 0x%x\n",__func__,Settings->stVideoSettings.stCustomRes.bCustomRes);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomRes.ulWidth              = %d\n",__func__,Settings->stVideoSettings.stCustomRes.ulWidth);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomRes.ulHeight             = %d\n",__func__,Settings->stVideoSettings.stCustomRes.ulHeight);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomRes.ulRefreshRate        = 0x%x\n",__func__,Settings->stVideoSettings.stCustomRes.ulRefreshRate);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomRes.bInterlaced          = 0x%x\n",__func__,Settings->stVideoSettings.stCustomRes.bInterlaced);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioX = 0x%x\n",__func__,Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioX );
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioY = 0x%x\n",__func__,Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioY );
	DI_UART_Print("[%s] Settings->stVideoSettings.ulMaxBitRate                     = 0x%x\n",__func__,Settings->stVideoSettings.ulMaxBitRate);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomIPB.bCustomIPB           = 0x%x\n",__func__,Settings->stVideoSettings.stCustomIPB.bCustomIPB);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomIPB.ulFrameP             = 0x%x\n",__func__,Settings->stVideoSettings.stCustomIPB.ulFrameP);
	DI_UART_Print("[%s] Settings->stVideoSettings.stCustomIPB.ulFrameB             = 0x%x\n",__func__,Settings->stVideoSettings.stCustomIPB.ulFrameB);

	DI_UART_Print("[%s]Settings->ulDemuxId    = 0x%x \n",__func__, Settings->ulDemuxId);
	DI_UART_Print("[%s]Settings->usVideoPid   = 0x%x \n",__func__, Settings->usVideoPid);
	DI_UART_Print("[%s]Settings->usAudioPid   = 0x%x \n",__func__, Settings->usAudioPid);
	DI_UART_Print("[%s]Settings->usPcrPid     = 0x%x \n",__func__, Settings->usPcrPid);
#endif
	s_stTranscoder[ulDeviceId].ulDemuxId = Settings->ulDemuxId;

	DrvErr=DRV_TRANSCODER_SetStcChannel(ulDeviceId, Settings->ulDemuxId, Settings->eTransSourceType);
	if(DrvErr != DRV_OK)
	{
		PrintError("[%s] fail to set stc channel (ulDeviceId = %d)\n",__func__,ulDeviceId);
		DiErr=DI_ERR_ERROR;
		goto done;
	}

	s_stTranscoder[ulDeviceId].eTransSourceType = Settings->eTransSourceType;
	s_stTranscoder[ulDeviceId].eTransEncodeMode = Settings->eTransEncodeMode;

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AUDIO)
	{

		AudEncSettings.eTransType = Settings->eTransSourceType;
		AudEncSettings.nInputType = Settings->eInputAudioFormat;
		AudEncSettings.nOutputType = Settings->eOutputAudioFormat;
		AudEncSettings.ulDemuxId = Settings->ulDemuxId;

		if(Settings->stAudioSettings.eEncType == DI_TRANSCODER_AUDIO_ENCODING )
		{
			AudEncSettings.bEncoding = TRUE;
		}
		else
		{
			AudEncSettings.bEncoding = FALSE;
		}

		AudEncSettings.ulMultiAudioCnt = 1;
		if(Settings->stAudioSettings.bMultiAudio == TRUE)
		{
			AudEncSettings.ulMultiAudioCnt = Settings->stAudioSettings.ulMultiAudioCnt;
		}

		DrvErr=DRV_AUDIO_EncSettings(ulDeviceId, &AudEncSettings);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] fail to set audio encoder (ulDeviceId = %d)\n",__func__,ulDeviceId);
			DiErr=DI_ERR_ERROR;
			goto done;
		}

		s_stTranscoder[ulDeviceId].AudEncSettings.eTransType = Settings->eTransSourceType;
		s_stTranscoder[ulDeviceId].AudEncSettings.nInputType = Settings->eInputAudioFormat;
		s_stTranscoder[ulDeviceId].AudEncSettings.nOutputType = Settings->eOutputAudioFormat;
		s_stTranscoder[ulDeviceId].AudEncSettings.ulDemuxId = Settings->ulDemuxId;
	}

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_VIDEO)
	{

		s_stTranscoder[ulDeviceId].VidEncSettings.eTransType = Settings->eTransSourceType;
		s_stTranscoder[ulDeviceId].VidEncSettings.ulDemuxId = Settings->ulDemuxId;
		s_stTranscoder[ulDeviceId].VidEncSettings.eDecCodec = Settings->eInputVideoFormat;
		s_stTranscoder[ulDeviceId].VidEncSettings.eCodec = Settings->eOutputVideoFormat;

		s_stTranscoder[ulDeviceId].VidEncSettings.eRes = Settings->stVideoSettings.eRes;
		s_stTranscoder[ulDeviceId].VidEncSettings.eFrameRate = Settings->stVideoSettings.eFrameRate;
		s_stTranscoder[ulDeviceId].VidEncSettings.eAspectRatio = Settings->stVideoSettings.eAspectRatio;
		s_stTranscoder[ulDeviceId].VidEncSettings.ulMaxBitRate = Settings->stVideoSettings.ulMaxBitRate;

		s_stTranscoder[ulDeviceId].VidEncSettings.eLevel = Settings->eCodecLevel;
		s_stTranscoder[ulDeviceId].VidEncSettings.eProfile = Settings->eCodecProfile;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.bCustomIPB = Settings->stVideoSettings.stCustomIPB.bCustomIPB;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.ulFrameP = Settings->stVideoSettings.stCustomIPB.ulFrameP;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.ulFrameB = Settings->stVideoSettings.stCustomIPB.ulFrameB;
		s_stTranscoder[ulDeviceId].VidEncSettings.bDefaultProfile = Settings->bVideoDefaultProfile;

		if(Settings->stVideoSettings.stCustomRes.bCustomRes == TRUE)
		{
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.bCustom = TRUE;
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulWidth = Settings->stVideoSettings.stCustomRes.ulWidth;
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulHeight = Settings->stVideoSettings.stCustomRes.ulHeight;
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulRefreshRate = Settings->stVideoSettings.stCustomRes.ulRefreshRate;
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.bInterlaced = Settings->stVideoSettings.stCustomRes.bInterlaced;
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulSampleAspectRatioX = Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioX;
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulSampleAspectRatioY = Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioY;
		}
		else
		{
			s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.bCustom = FALSE;
		}

		DrvErr=DRV_VIDEO_EncSettings(ulDeviceId, &s_stTranscoder[ulDeviceId].VidEncSettings);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] fail to set video encoder (ulDeviceId = %d)\n",__func__,ulDeviceId);
			DiErr=DI_ERR_ERROR;
			goto done;
		}
	}

done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DiErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_TRANSCODER_UpdateSettings_t *Settings
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_UpdateSetSettings(HUINT32 ulDeviceId, DI_TRANSCODER_UpdateSettings_t *Settings)
{
	DRV_Error DrvErr=DRV_ERR;
	DI_ERR_CODE DiErr=DI_ERR_OK;
	int nRet=VK_OK;

	PrintEnter();

	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized == FALSE || s_stTranscoder[ulDeviceId].bStart == FALSE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema \n",__func__);
		return DI_ERR_ERROR;
	}

	s_stTranscoder[ulDeviceId].VidEncSettings.ulMaxBitRate = Settings->stVideoSettings.ulMaxBitRate;
	s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.bCustomIPB = Settings->stVideoSettings.stCustomIPB.bCustomIPB;
	s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.ulFrameP = Settings->stVideoSettings.stCustomIPB.ulFrameP;
	s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.ulFrameB = Settings->stVideoSettings.stCustomIPB.ulFrameB;
	s_stTranscoder[ulDeviceId].VidEncSettings.eRes = Settings->stVideoSettings.eRes;
	s_stTranscoder[ulDeviceId].VidEncSettings.eAspectRatio = Settings->stVideoSettings.eAspectRatio;
	s_stTranscoder[ulDeviceId].VidEncSettings.eFrameRate = Settings->stVideoSettings.eFrameRate;
	s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.bCustom = Settings->stVideoSettings.stCustomRes.bCustomRes;
	if(Settings->stVideoSettings.stCustomRes.bCustomRes == TRUE)
	{
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulWidth = Settings->stVideoSettings.stCustomRes.ulWidth;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulHeight= Settings->stVideoSettings.stCustomRes.ulHeight;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulRefreshRate = Settings->stVideoSettings.stCustomRes.ulRefreshRate;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.bInterlaced = Settings->stVideoSettings.stCustomRes.bInterlaced;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulSampleAspectRatioX= Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioX;
		s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulSampleAspectRatioY= Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioY;
	}
	DrvErr=DRV_VIDEO_UpdateEncSettings(ulDeviceId, &s_stTranscoder[ulDeviceId].VidEncSettings);
	if(DrvErr != DRV_OK)
	{
		PrintError("[%s] fail to set update video encoder (ulDeviceId = %d)\n",__func__,ulDeviceId);
		DiErr=DI_ERR_ERROR;
		goto done;
	}

done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DiErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_TRANSCODER_UpdateSettings_t *Settings
 * @return success DI_ERR_OK  fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_UpdateGetSettings(HUINT32 ulDeviceId, DI_TRANSCODER_UpdateSettings_t *Settings)
{
	int nRet=VK_OK;

	PrintEnter();

	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized == FALSE || s_stTranscoder[ulDeviceId].bStart == FALSE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema \n",__func__);
		return DI_ERR_ERROR;
	}

	DRV_VIDEO_UpdateEncGetSettings(ulDeviceId, &s_stTranscoder[ulDeviceId].VidEncSettings);
	Settings->stVideoSettings.ulMaxBitRate		= s_stTranscoder[ulDeviceId].VidEncSettings.ulMaxBitRate;
	Settings->stVideoSettings.eRes 				= s_stTranscoder[ulDeviceId].VidEncSettings.eRes;
	Settings->stVideoSettings.eFrameRate 		= s_stTranscoder[ulDeviceId].VidEncSettings.eFrameRate;
	Settings->stVideoSettings.eAspectRatio 		= s_stTranscoder[ulDeviceId].VidEncSettings.eAspectRatio;
	Settings->stVideoSettings.stCustomIPB.bCustomIPB 	= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.bCustomIPB;
	Settings->stVideoSettings.stCustomIPB.ulFrameP		= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.ulFrameP;
	Settings->stVideoSettings.stCustomIPB.ulFrameB		= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomIPB.ulFrameB;
	Settings->stVideoSettings.stCustomRes.bCustomRes	= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.bCustom;
	Settings->stVideoSettings.stCustomRes.ulWidth 		= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulWidth;
	Settings->stVideoSettings.stCustomRes.ulHeight 		= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulHeight;
	Settings->stVideoSettings.stCustomRes.ulRefreshRate = s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulRefreshRate;
	Settings->stVideoSettings.stCustomRes.bInterlaced 	= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.bInterlaced;
	Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioX 	= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulSampleAspectRatioX;
	Settings->stVideoSettings.stCustomRes.ulSampleAspectRatioY 	= s_stTranscoder[ulDeviceId].VidEncSettings.stCustomRes.ulSampleAspectRatioY;

	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DI_ERR_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_TRANSCODER_MUX_TYPE eMuxType
 * @param[in] DI_TRANSCODER_MUX_INFO *Info
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_Start(HUINT32 ulDeviceId, DI_TRANSCODER_MUX_TYPE eMuxType, DI_TRANSCODER_MUX_INFO *Info)
{

	HUINT32 min=0, max=0;
	HUINT32 Dee=0;
	DRV_STREAM_MUX_INFORMATION s_stMuxInfo;
	DRV_Error DrvErr=DRV_OK;
	DI_ERR_CODE DiErr = DI_ERR_OK;
	int nRet=VK_OK;

	UNREFENCED_PARAM(eMuxType);

	PrintEnter();

	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE || s_stTranscoder[ulDeviceId].bStart == TRUE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}

	VK_memset(&s_stMuxInfo,0x0, sizeof(s_stMuxInfo));

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_VIDEO)
	{
		DrvErr=DRV_VIDEO_GetEncDelayRange(ulDeviceId,&min,&max);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] fail to get video delay range. \n",__func__);
			DiErr = DI_ERR_ERROR;
			goto err;
		}
	}
	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AUDIO)
	{
		DrvErr=DRV_AUDIO_GetEncDelayStatus(ulDeviceId, &Dee);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] fail to get audio delay range. \n",__func__);
			DiErr = DI_ERR_ERROR;
			goto err;
		}
	}
#if (NEXUS_VERSION < 1404)
	if(s_stTranscoder[ulDeviceId].AudEncSettings.nInputType == s_stTranscoder[ulDeviceId].AudEncSettings.nOutputType)
	{
		Dee = Dee + DI_TRANSCODER_ADJUST_AUDIO_DELAY;
	}
#endif
	if(Dee > min)
	{
		if(Dee > max)
			Dee = max;
	}
	else
		Dee = min;

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AUDIO)
	{
		DrvErr=DRV_AUDIO_EncStart(ulDeviceId, Dee);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] fail to start audio encoder.\n",__func__);
			DiErr = DI_ERR_ERROR;
			goto audiostarterr;
		}
		s_stMuxInfo.audioCodec = Info->audioCodec;
		s_stMuxInfo.audioPid = Info->audioPid;
		s_stMuxInfo.ulAudCnt = 1;
		if(Info->bMultiAudio == TRUE)
		{
			s_stMuxInfo.ulAudCnt = Info->ulMultiAudioCnt;
			s_stMuxInfo.audioSubPid = Info->audioSubPid;
		}
	}

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_VIDEO)
	{
		DrvErr=DRV_VIDEO_EncStart(ulDeviceId, Dee);
		if(DrvErr != DRV_OK)
		{
			PrintError("[%s] fail to start video encoder.\n",__func__);
			DiErr = DI_ERR_ERROR;
			goto videostarterr;
		}
		s_stMuxInfo.videoCodec = Info->videoCodec;
		s_stMuxInfo.videoPid = Info->videoPid;
	}

	s_stMuxInfo.pcrPid = Info->pcrPid;
	s_stMuxInfo.NonPcrInsert = Info->NonPcrInsert;
	s_stMuxInfo.index = Info->index;
	if( s_stTranscoder[ulDeviceId].eTransSourceType == DI_TRANSCODER_SOURCE_WATCH )
	{
		s_stMuxInfo.bIsWatch = TRUE;
	}
	else
	{
		s_stMuxInfo.bIsWatch = FALSE;
	}

	
	s_stMuxInfo.ulDemuxId = s_stTranscoder[ulDeviceId].ulDemuxId;

	DrvErr=DRV_STREAM_MUX_Start(ulDeviceId,&s_stMuxInfo);
	if(DrvErr != DRV_OK)
	{
		PrintError("[%s] fail to start stream mux.\n",__func__);
		DiErr = DI_ERR_ERROR;
		goto systemstarterr;
	}


	s_stTranscoder[ulDeviceId].bStart = TRUE;
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	PrintExit();
	return DiErr;

systemstarterr:
	DRV_VIDEO_EncStop(ulDeviceId);
videostarterr:
	DRV_AUDIO_EncStop(ulDeviceId);
audiostarterr:
err:
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	PrintExit();
	return DiErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT16 psiPid
 * @param[in] HUINT16 psiSize
 * @param[in] HUINT8 *psiData
 * @param[in] HUINT8 contflag
 * @param[in] unsigned long timeOut
 * @param[in] HUINT32 *pIndexId
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_SetPSIData(HUINT32 ulDeviceId,HUINT16 psiPid,HUINT32 psiSize,HUINT8 *psiData,HUINT8 contflag,unsigned long timeOut, HUINT32 *pIndexId )
{
	DRV_Error drvErr = 0;
	HUINT32 index = 0;

	PrintEnter();

	drvErr = DRV_STREAM_MUX_SetPSIData(ulDeviceId,psiPid,psiSize,psiData,contflag,timeOut,&index);
	if(drvErr != 0)
	{
		return DI_ERR_ERROR;
	}

	*pIndexId  = index;

	PrintExit();

	return drvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 ulIndex
 * @param[in] HUINT32 psiSize
 * @param[in] HUINT8 *pPSIData
 * @param[in] HUINT8 startflag
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_UpdatePSIData(HUINT32 ulDeviceId,HUINT32 ulIndex, HUINT32 psiSize,HUINT8 *pPSIData,HUINT8 startflag)
{
	DRV_Error drvErr = 0;

	PrintEnter();

	drvErr = DRV_STREAM_MUX_UpdatePSIData(ulDeviceId,ulIndex,psiSize,pPSIData,startflag);
	if(drvErr != 0)
	{
		return DI_ERR_ERROR;
	}

	PrintExit();

	return drvErr;
}

DI_ERR_CODE DI_TRANSCODER_SetPesData(HUINT32 ulDeviceId,HUINT32 ulInputDemuxId,HUINT16 psiPid,HUINT32 *pIndexId )
{
	DRV_Error drvErr = 0;
	HUINT32 index = 0;

	PrintEnter();

	drvErr = DRV_STREAM_MUX_SetPesData(ulDeviceId,ulInputDemuxId,psiPid,&index);
	if(drvErr != 0)
	{
		return DI_ERR_ERROR;
	}

	*pIndexId  = index;

	PrintExit();

	return drvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_Stop(HUINT32 ulDeviceId)
{
	DRV_Error drvErr = DRV_OK;
	DI_ERR_CODE DiErr = DI_ERR_OK;
	int nRet;

	PrintEnter();

	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE || s_stTranscoder[ulDeviceId].bStart == FALSE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d, bStart = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bStart);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AUDIO)
	{
		drvErr = DRV_AUDIO_EncStop(ulDeviceId);
		if(drvErr != DRV_OK)
		{
			PrintError("[%s] DRV_STREAM_MUX_Stop error.\n",__func__);
			DiErr = DI_ERR_ERROR;
			goto done;
		}
	}

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_VIDEO)
	{
		drvErr = DRV_VIDEO_EncStop(ulDeviceId);
		if(drvErr != DRV_OK)
		{
			PrintError("[%s] DRV_STREAM_MUX_Stop error.\n",__func__);
			DiErr = DI_ERR_ERROR;
			goto done;
		}
	}

	drvErr = DRV_STREAM_MUX_Stop(ulDeviceId);
	if(drvErr != DRV_OK)
	{
		PrintError("[%s] DRV_STREAM_MUX_Stop error.\n",__func__);
		DiErr = DI_ERR_ERROR;
		goto done;
	}

	s_stTranscoder[ulDeviceId].bStart = FALSE;

done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DiErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_ReStart(HUINT32 ulDeviceId)
{
	DRV_Error drvErr = DRV_OK;
	DI_ERR_CODE DiErr = DI_ERR_OK;
	int nRet;

	PrintEnter();

	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AUDIO)
	{
		drvErr = DRV_AUDIO_EncRestart(ulDeviceId,s_stTranscoder[ulDeviceId].ulDemuxId);
		if(drvErr != DRV_OK)
		{
			PrintError("[%s] fail to DRV_AUDIO_EncRestartl (ulDeviceId = %d)\n",__func__,ulDeviceId);
			DiErr=DI_ERR_ERROR;
			goto done;
		}
	}


	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_VIDEO)
	{
		drvErr = DRV_VIDEO_EncRestart(ulDeviceId, s_stTranscoder[ulDeviceId].ulDemuxId);
		if(drvErr != DRV_OK)
		{
			PrintError("[%s] fail to DRV_VIDEO_EncRestart (ulDeviceId = %d)\n",__func__,ulDeviceId);
			DiErr=DI_ERR_ERROR;
			goto done;
		}
	}
done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);

	return DiErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_Close(HUINT32 ulDeviceId)
{
	DRV_Error drvErr = DRV_OK;
	DI_ERR_CODE DiErr = DI_ERR_OK;
	int nRet;

	PrintEnter();

	PrintDebug("[%s] ulDeviceId = %d\n", __func__, ulDeviceId);

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
	{
		PrintError("[%s] ulDeviceId error (ulDeviceId=%d, bInitialized = %d)\n",
			__func__, ulDeviceId, ulDeviceId>=NUM_OF_TRANSCODER?FALSE:s_stTranscoder[ulDeviceId].bInitialized);
		return DI_ERR_ERROR;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema.\n", __func__);
		return DI_ERR_ERROR;
	}

	nRet = DRV_TRANSCODER_UnSetStcChannel(ulDeviceId, s_stTranscoder[ulDeviceId].ulDemuxId, s_stTranscoder[ulDeviceId].eTransSourceType);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to DRV_TRANSCODER_UnSetStcChannel (ulDeviceId = %d)\n", __func__, ulDeviceId);
		#if 0 //continue unset process
		VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
		return DI_ERR_ERROR;
		#endif
	}

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AUDIO)
	{
		drvErr = DRV_AUDIO_EncUnsettings(ulDeviceId);
		if(drvErr != DRV_OK)
		{
			PrintError("[%s] fail to DRV_AUDIO_EncUnsettings (ulDeviceId = %d)\n", __func__, ulDeviceId);
			DiErr = DI_ERR_ERROR;
			goto done;
		}
	}

	if(s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_AV ||
		s_stTranscoder[ulDeviceId].eTransEncodeMode == DI_TRANSCODER_ENCODE_VIDEO)
	{
		drvErr = DRV_VIDEO_EncUnsettings(ulDeviceId);
		if(drvErr != DRV_OK)
		{
			PrintError("[%s] fail to DRV_VIDEO_EncUnsettings (ulDeviceId = %d)\n", __func__, ulDeviceId);
			DiErr = DI_ERR_ERROR;
			goto done;
		}
	}

	s_stTranscoder[ulDeviceId].ulDemuxId = 0;
	s_stTranscoder[ulDeviceId].bStart = FALSE;

done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);

	return DI_ERR_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 ulDemuxId
 * @param[in] DI_VIDEO_FORMAT eVideoCodec
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_ProbeStart(HUINT32 ulDeviceId, HUINT32 ulDemuxId, DI_VIDEO_FORMAT eVideoCodec)
{
	int nRet;
	DRV_Error drvErr = DRV_OK;
	DI_ERR_CODE rc=DI_ERR_OK;
	PrintEnter();

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
		return DI_ERR_ERROR;

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}

	drvErr=DRV_VIDEO_DecProbeStart(ulDeviceId, ulDemuxId, eVideoCodec);
	if(drvErr != DRV_OK)
	{
		PrintError("[%s] fail to DRV_VIDEO_DecProbeStart!\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	s_stTranscoder[ulDeviceId].VidEncSettings.eDecCodec = eVideoCodec;

	if(s_stTranscoder[ulDeviceId].ulProbeCheckId != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(s_stTranscoder[ulDeviceId].ulProbeCheckId);
		s_stTranscoder[ulDeviceId].ulProbeCheckId = VK_TIMERID_NULL;
	}

	nRet=VK_TIMER_EventAfter(s_stTranscoder[ulDeviceId].ulProbeTimeout, P_TRANSCODER_EncProbeCheck, &ulDeviceId, sizeof(ulDeviceId),(unsigned long *)&s_stTranscoder[ulDeviceId].ulProbeCheckId);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to VK_TIMER_EventAfter for P_TRANSCODER_EncProbeCheck. \n",__func__);
	}

done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return rc;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_ProbeStop(HUINT32 ulDeviceId)
{
	int nRet;
	DRV_Error drvErr = DRV_OK;
	DI_ERR_CODE rc=DI_ERR_OK;
	PrintEnter();

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
		return DI_ERR_ERROR;

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}
	drvErr=DRV_VIDEO_DecProbeStop(ulDeviceId);
	if(drvErr != DRV_OK)
	{
		PrintError("[%s] fail to DRV_VIDEO_DecProbeStop!\n",__func__);
		rc=DI_ERR_ERROR;
	}

	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return rc;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_ProbeClose(HUINT32 ulDeviceId)
{
	int nRet;
	DRV_Error drvErr = DRV_OK;
	DI_ERR_CODE rc=DI_ERR_OK;
	PrintEnter();

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
		return DI_ERR_ERROR;

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}
	drvErr=DRV_VIDEO_DecProbeClose(ulDeviceId);
	if(drvErr != DRV_OK)
	{
		PrintError("[%s] fail to DRV_VIDEO_DecProbeClose!\n",__func__);
		rc=DI_ERR_ERROR;
	}

	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DI_ERR_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 ulTimeout
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_ProbeTimeOut(HUINT32 ulDeviceId, HUINT32 ulTimeout) /* timeout in ms */
{
	int nRet;
	PrintEnter();

	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
		return DI_ERR_ERROR;

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}

	s_stTranscoder[ulDeviceId].ulProbeTimeout = ulTimeout;

	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DI_ERR_OK;
}
/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_TRANSCODER_DEC_StreamInfo_t *stream_info
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_GetDecSequenceHeaderInfo(HUINT32 ulDeviceId, DI_TRANSCODER_DEC_StreamInfo_t *stream_info)
{
	int nRet;
	DRV_Error drvErr = DRV_OK;
	DI_ERR_CODE rc=DI_ERR_OK;
	NEXUS_VideoDecoderStreamInformation StreamInfo;
	NEXUS_VideoDecoderStatus Status;

	PrintEnter();
	if (ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
		return DI_ERR_ERROR;

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}

	if(stream_info == NULL)
	{
		PrintError("[%s] stream_info is NULL.\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	drvErr=DRV_VIDEO_GetDecSequenceInfo(ulDeviceId, &StreamInfo, &Status);
	if(drvErr != DRV_OK)
	{
		PrintError("[%s] fail to DRV_VIDEO_GetDecSeqenceInfo!\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}
	stream_info->ulWidth = StreamInfo.sourceHorizontalSize;
	stream_info->ulHeight = StreamInfo.sourceVerticalSize;
	stream_info->ulDisplayWidth = StreamInfo.displayHorizontalSize;
	stream_info->ulDisplayHeight = StreamInfo.displayVerticalSize;
	stream_info->eAspectRatio = DRV_VIDEO_EncGetDiAspectRatio(0,StreamInfo.aspectRatio);
	stream_info->eFrameRate = DRV_VIDEO_GetDiFrameRate(0,StreamInfo.frameRate);
	if(stream_info->eAspectRatio == DI_VIDEO_ASPECTRATIO_SAR)
	{
		stream_info->usSampleAspectRatioX=StreamInfo.sampleAspectRatioX;
		stream_info->usSampleAspectRatioY=StreamInfo.sampleAspectRatioY;
	}
	else
	{
		stream_info->usSampleAspectRatioX=0;
		stream_info->usSampleAspectRatioY=0;
	}
	stream_info->eVideoFormat = DRV_VIDEO_GetDiFormat(0,s_stTranscoder[ulDeviceId].VidEncSettings.eDecCodec);
	stream_info->bProgressive = StreamInfo.streamProgressive;
	stream_info->ucColorPrimarie = StreamInfo.colorPrimaries;
	stream_info->eCodecLevel = DRV_VIDEO_EncGetDiCodecLevel(0, Status.protocolLevel);
	stream_info->eCodecProfile = DRV_VIDEO_EncGetDiCodecProfile(0,Status.protocolProfile);

	PrintDebug("[%s] ulWidth %d \n",__func__,stream_info->ulWidth);
	PrintDebug("[%s] ulHeight %d \n",__func__,stream_info->ulHeight);
	PrintDebug("[%s] eAspectRatio %d \n",__func__,stream_info->eAspectRatio);
	PrintDebug("[%s] usSampleAspectRatioX %d \n",__func__,stream_info->usSampleAspectRatioX);
	PrintDebug("[%s] usSampleAspectRatioY %d \n",__func__,stream_info->usSampleAspectRatioY);
	PrintDebug("[%s] eVideoFormat %d \n",__func__,stream_info->eVideoFormat);
	PrintDebug("[%s] bProgressive %d \n",__func__,stream_info->bProgressive);
	PrintDebug("[%s] ucColorPrimarie %d \n",__func__,stream_info->ucColorPrimarie);
	PrintDebug("[%s] eCodecLevel %d \n",__func__,stream_info->eCodecLevel);
	PrintDebug("[%s] eCodecProfile %d \n",__func__,stream_info->eCodecProfile);

done:
	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return rc;

}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_TRANSCODER_NOTIFY_EVT nEvent
 * @param[in] TRANSCODER_EVENT_CALLBACK_FUNCTION fnTranscoderCallback
 * @return success DI_ERR_OK fail DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DI_ERR_CODE DI_TRANSCODER_RegisterEventCallback(HUINT32 ulDeviceId, DI_TRANSCODER_NOTIFY_EVT eEvent, TRANSCODER_EVENT_CALLBACK_FUNCTION fnTranscoderCallback)
{
	int nRet;

	PrintEnter();
	if(ulDeviceId>=NUM_OF_TRANSCODER || s_stTranscoder[ulDeviceId].bInitialized==FALSE)
		return DI_ERR_ERROR;

	if(eEvent >= DI_TRANSCODER_NOTIFY_EVT_END || fnTranscoderCallback == NULL)
	{
		PrintError("[%s] the number of event is too many or Callback is NULL.\n",__func__);
		return DI_ERR_INVALID_PARAM;
	}

	nRet=VK_SEM_Get(s_stTranscoder[ulDeviceId].ulSemId_DI);
	if(nRet != VK_OK)
	{
		PrintError("[%s] fail to get sema. \n",__func__);
		return DI_ERR_ERROR;
	}

	s_stTranscoder[ulDeviceId].stCallbackInfo[eEvent].pfnCallback=fnTranscoderCallback;

	PrintExit();
	VK_SEM_Release(s_stTranscoder[ulDeviceId].ulSemId_DI);
	return DI_ERR_OK;
}
