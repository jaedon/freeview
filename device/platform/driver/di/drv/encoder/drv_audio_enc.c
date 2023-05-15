/*
 * @defgroup		DI_AUDIO DI_AUDIO : DI_AUDIO module
 * @author	       Jason Han.
 * @note			Drv_AUDIO APIs
 * @brief			Define Drv_AUDIO APIs
 * @file 			drv_audio_enc.c                           */


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include <string.h>
#include "linden_trace.h"

#include "bstd_defs.h"
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
#include "nexus_audio_mux_output.h"
#include "nexus_audio_encoder.h"
#include "nexus_video_encoder.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_audio.h"
#include "drv_audio_enc.h"
#include "drv_audio.h"

#include "di_video.h"
#include "drv_video.h"
#include "drv_video_enc.h"

#include "drv_demux.h"
#include "drv_pvr.h"
#include "drv_media.h"
#include "drv_err.h"

#include "di_scart.h"
#include "drv_scart.h"
#include "di_uart.h"
#include "di_media.h"
#include "drv_hdmi.h"
#include "drv_gpio.h"
#if defined(CONFIG_SYNC_CHANNEL)
#include "drv_sync_channel.h"
#endif
#if defined(CONFIG_ASTM)
#include "drv_astm.h"
#endif
#if defined(CONFIG_SYNC_RECOVERY)
#include "drv_sync_recovery.h"
#endif

#include "di_transcoder.h"
#include "drv_transcoder.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE TRACE_MODULE_DI_AUDIO
#define DI_AUDIO_DEBUG
#define UNUSED(x) (void)(x)

#define ENC_DECODER_CNT		(NEXUS_NUM_AUDIO_DECODERS-AUDIO_ALL_DECODERS)
#define	ENC_DECODER_START_INDEX	(ENC_DECODER_CNT)
#define ENC_MULTI_AUDIO_CNT	2
/*******************************************************************/
/******************************* extern ******************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct
{
	NEXUS_AudioDecoderHandle	hDecoder[ENC_MULTI_AUDIO_CNT];
	NEXUS_AudioEncoderHandle	hEncoder[ENC_MULTI_AUDIO_CNT];
	NEXUS_AudioMuxOutputHandle	hMuxOutput[ENC_MULTI_AUDIO_CNT];
	NEXUS_AudioMixerHandle		hDspMixer[ENC_MULTI_AUDIO_CNT];
	NEXUS_AudioMixerHandle      hHwMixer;
	HUINT32 ulMulAudCnt;
} DI_EncInstance;

typedef struct
{	
	HBOOL	bWatching;
	HBOOL	bDecoderMute;
	HBOOL	bEncoding;
	NEXUS_AudioEncoderSettings encoderSettings;
	NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
	NEXUS_AudioDecoderStartSettings audioSettings[ENC_MULTI_AUDIO_CNT];
}DI_EncStatus;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DI_EncInstance		stAudEncIns[ENC_DECODER_CNT];
static DI_EncStatus			stAudEncStatus[ENC_DECODER_CNT];
static unsigned long 		s_ulAudEncSemId;

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

DRV_Error P_AUDIO_GetEncDecoderIndex(HUINT32 ulDevId, HUINT32 ulIndex,HUINT32 *ulEncDevId)
{
	DRV_Error nDrvErr = DRV_OK;

#if (CONFIG_MAX_TRANSCODER_CHANNEL==1)
	*ulEncDevId = (AUDIO_ALL_DECODERS + (ulDevId*ENC_DECODER_CNT)+ulIndex);
#else
	*ulEncDevId = (AUDIO_ALL_DECODERS + ulDevId);
#endif
	if(*ulEncDevId >= NEXUS_NUM_AUDIO_DECODERS)
	{
		*ulEncDevId = NEXUS_NUM_AUDIO_DECODERS;	
		nDrvErr = DRV_ERR;
	}
	return nDrvErr;
}

DRV_Error DRV_AUDIO_EncInit(void)
{
	DRV_Error nDrvErr = DRV_OK;
	HINT32 result;

	result = VK_SEM_Create(&s_ulAudEncSemId,"di_audio_enc", DEFAULT_SUSPEND_TYPE);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}
	return nDrvErr;
}
DRV_Error DRV_AUDIO_EncSetStereo(HUINT32 ulDevId, DRV_AUDIO_ENC_STEREO_MODE dualMode)
{
	HINT32		vResult;
	NEXUS_Error dResult;
	HUINT32	ulMultiAudCnt = 0;
	HUINT32 ulIndex = 0;
	
	vResult = VK_SEM_Get(s_ulAudEncSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	if(stAudEncStatus[ulDevId].bEncoding != TRUE)
	{
		PrintError("[%s]%d does not support \n",__FUNCTION__,__LINE__);
		goto error_out;
	}
	ulMultiAudCnt = stAudEncIns[ulDevId].ulMulAudCnt;
	for(ulIndex = 0; ulIndex < ulMultiAudCnt ; ulIndex++)
	{
		NEXUS_AudioDecoderSettings		pSettings;
	
		NEXUS_AudioDecoder_GetSettings(stAudEncIns[ulDevId].hDecoder[ulIndex], &pSettings);
		switch(dualMode)
		{
			case DRV_AUDIO_ENC_Stereo:
				pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eStereo;
				break;
			case DRV_AUDIO_ENC_LeftOnly:
				pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eLeft;
				break;
			case DRV_AUDIO_ENC_RightOnly:
				pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eRight;
				break;
			case DRV_AUDIO_ENC_Mix:
				pSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eMix;
				break;
			default:
				PrintError("[%s]%d does not support (dualMode =%d)\n",__FUNCTION__,__LINE__,dualMode);
				goto error_out;
				break;
		}
		
		dResult = NEXUS_AudioDecoder_SetSettings(stAudEncIns[ulDevId].hDecoder[ulIndex], &pSettings);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_AudioDecoder_SetSettings failed!\n");
			goto error_out;
		}
	}
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_OK;
		 
error_out:
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_ERR;

}

DRV_Error DRV_AUDIO_EncSettings(HUINT32 ulDevId, DRV_AUDIO_ENC_Set_t *Settings)
{
	DRV_Error	eDRVError = DRV_OK;
	HINT32		vResult;
	NEXUS_Error dResult;
	NEXUS_AudioMixerSettings audioMixerSettings;
	HUINT32 ulEncDevId;
	NEXUS_AudioOutput audioOutput;
	NEXUS_AudioOutputSettings audioOutputSettings;
	HBOOL		bBypass = FALSE;
	DI_TRANSCODER_ENCODE_MODE eEncMode = DI_TRANSCODER_ENCODE_AV;
	HUINT32 ulIndex = 0;
	
	vResult = VK_SEM_Get(s_ulAudEncSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	/** Initialization **/
	for(ulIndex = 0; ulIndex < ENC_MULTI_AUDIO_CNT; ulIndex++)
	{
		stAudEncIns[ulDevId].hEncoder[ulIndex] = NULL;
	}
	stAudEncStatus[ulDevId].encoderSettings.codec = DRV_AUDIO_ConvertCodecToNexus(DI_AUDFORMAT_NONE);
	
	if((Settings->nInputType == Settings->nOutputType) && (Settings->bEncoding != TRUE))
	{
		bBypass = TRUE;
	}

	/* Open the audio and pcr pid channel */
	if(DRV_TRANSCODER_GetEncodeMode(ulDevId, &eEncMode) != DRV_OK)
	{
		goto error_out;
	}

    /* Open the hw mixer -> dummy output */
    stAudEncIns[ulDevId].hHwMixer = NEXUS_AudioMixer_Open(NULL);
   	if(stAudEncIns[ulDevId].hHwMixer == NULL)
	{
		PrintError("[%s]%d NEXUS_AudioMixer_AddInput failed! \n",__FUNCTION__,__LINE__);
		goto error_out;
	}
	/* Attach outputs for real-time transcoding */
	eDRVError = DRV_AUDIO_ConnectAudioDummyOutput(NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hHwMixer));
	if(eDRVError != DRV_OK)
	{
		PrintError("[%s]%d DRV_AUDIO_ConnectAudioDummyOutput failed! \n",__FUNCTION__,__LINE__);
		goto error_out;
	}

	for(ulIndex = 0 ; ulIndex < Settings->ulMultiAudioCnt ; ulIndex ++)
	{
		NEXUS_AudioDecoder_GetDefaultStartSettings(&stAudEncStatus[ulDevId].audioSettings[ulIndex]);
		if(Settings->eTransType == DI_TRANSCODER_SOURCE_WATCH)
		{
			stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel = DRV_DEMUX_GetAudioPidChannel(0);
			stAudEncStatus[ulDevId].audioSettings[ulIndex].stcChannel = DRV_DEMUX_GetStcChannelHandle();	
		}
		else
		{
			if(ulIndex == 0)
			{
				stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel = DRV_DEMUX_GetPidChannelForTranscode(Settings->ulDemuxId,
											(ulDevId == 0) ? DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_1 : DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_2);
			}
			else
			{
				stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel = DRV_DEMUX_GetPidChannelForTranscode(Settings->ulDemuxId,
											(ulDevId == 0) ? DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1 : DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2);
			}
			if(stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel == NULL)
			{
				PrintError("[%s]%d DRV_DEMUX_GetPidChannelForTranscode failed! ulDevId %d ulIndex %d\n",__FUNCTION__,__LINE__,ulDevId,ulIndex);
				goto error_out;
			}
			
			stAudEncStatus[ulDevId].audioSettings[ulIndex].stcChannel = DRV_DEMUX_GetSTCChannel(ulDevId+1);//to use the same stcchannel
		}		
		if(eEncMode == DI_TRANSCODER_ENCODE_AUDIO)
		{
			stAudEncStatus[ulDevId].audioSettings[ulIndex].stcChannel = NULL;
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
		stAudEncStatus[ulDevId].audioSettings[ulIndex].codec = DRV_AUDIO_ConvertCodecToNexus(Settings->nInputType);
		stAudEncStatus[ulDevId].audioSettings[ulIndex].nonRealTime = FALSE;

		/* Open the audio decoder */
		
		/* Connect audio decoders to outputs */
		eDRVError = P_AUDIO_GetEncDecoderIndex(ulDevId, ulIndex, &ulEncDevId);
		if(eDRVError != DRV_OK)
		{
			PrintError("[%s]%d Wroung Decoder index ulDevId=%d ulEncDevId=%d \n",__FUNCTION__,__LINE__, ulDevId, ulEncDevId);
			goto error_out;
		}

		stAudEncIns[ulDevId].hDecoder[ulIndex] = NEXUS_AudioDecoder_Open(ulEncDevId, NULL);
		if(stAudEncIns[ulDevId].hDecoder[ulIndex]==NULL)
		{
			PrintError("[%s]%d NEXUS_AudioDecoder_Open failed! ulEncDevId %d, ulIndex %d\n",__FUNCTION__,__LINE__,ulEncDevId,ulIndex);
			goto error_out;
		}
		/* Open audio mixer.  The mixer can be left running at all times to provide continuous audio output despite input discontinuities.	*/
		NEXUS_AudioMixer_GetDefaultSettings(&audioMixerSettings);
		audioMixerSettings.mixUsingDsp = true;
		if(bBypass == FALSE)
		{
			audioMixerSettings.outputSampleRate = 48000;/* fixed to allow gap filling for bogus audio PID */
		}
		stAudEncIns[ulDevId].hDspMixer[ulIndex] = NEXUS_AudioMixer_Open(&audioMixerSettings);
		if(stAudEncIns[ulDevId].hDspMixer[ulIndex] == NULL)
		{
			PrintError("[%s]%d NEXUS_AudioMixer_AddInput failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}

		/* Open audio mux output */
		stAudEncIns[ulDevId].hMuxOutput[ulIndex] = NEXUS_AudioMuxOutput_Create(NULL);

		if(bBypass == FALSE)
		{
			eDRVError = DRV_AUDIO_ConnectAudioDummyOutput(NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hDspMixer[ulIndex]));
			if(eDRVError != DRV_OK)
			{
				PrintError("[%s]%d DRV_AUDIO_ConnectAudioDummyOutput failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
			/*<<:  Open audio encoder start */
			NEXUS_AudioEncoder_GetDefaultSettings(&stAudEncStatus[ulDevId].encoderSettings);
			stAudEncStatus[ulDevId].encoderSettings.codec = DRV_AUDIO_ConvertCodecToNexus(Settings->nOutputType);
			stAudEncIns[ulDevId].hEncoder[ulIndex] = NEXUS_AudioEncoder_Open(&stAudEncStatus[ulDevId].encoderSettings);
			/*>>:  Open audio encoder start */
			
			/*<<:  Connect decoder -> dsp mixer and set as master */
			dResult = NEXUS_AudioMixer_AddInput(stAudEncIns[ulDevId].hDspMixer[ulIndex],
				NEXUS_AudioDecoder_GetConnector(stAudEncIns[ulDevId].hDecoder[ulIndex], NEXUS_AudioDecoderConnectorType_eStereo));
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioMixer_AddInput failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
			audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(stAudEncIns[ulDevId].hDecoder[ulIndex], 
										NEXUS_AudioDecoderConnectorType_eStereo);
			
			dResult = NEXUS_AudioMixer_SetSettings(stAudEncIns[ulDevId].hDspMixer[ulIndex], &audioMixerSettings);
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioMixer_SetSettings failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
			/*>> :  Connect decoder -> dsp mixer and set as master */
			
			/*<< : Connect mixer to encoder */
			dResult = NEXUS_AudioEncoder_AddInput(stAudEncIns[ulDevId].hEncoder[ulIndex], 
												NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hDspMixer[ulIndex]));
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioEncoder_AddInput failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
			/*>> : Connect mixer to encoder */

			/*<< : Connect encoder -> mux out */
			dResult = NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(stAudEncIns[ulDevId].hMuxOutput[ulIndex]), 
												NEXUS_AudioEncoder_GetConnector(stAudEncIns[ulDevId].hEncoder[ulIndex]));

			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioOutput_AddInput failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
			/*>> : Connect encoder -> mux out */
			
		}
		else
		{
			/***** BYPASS *****/
			/* dspMixer -> hwMixer -> dummy0 out */
			 NEXUS_AudioMixer_AddInput(stAudEncIns[ulDevId].hHwMixer,NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hDspMixer[ulIndex]));

			/* Connect decoder to mixer and set as master */
			dResult = NEXUS_AudioMixer_AddInput(stAudEncIns[ulDevId].hDspMixer[ulIndex],
				NEXUS_AudioDecoder_GetConnector(stAudEncIns[ulDevId].hDecoder[ulIndex], NEXUS_AudioDecoderConnectorType_eCompressed));
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioMixer_AddInput failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
		
			audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(stAudEncIns[ulDevId].hDecoder[ulIndex], 
										NEXUS_AudioDecoderConnectorType_eCompressed);
			dResult = NEXUS_AudioMixer_SetSettings(stAudEncIns[ulDevId].hDspMixer[ulIndex], &audioMixerSettings);
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioMixer_SetSettings failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
					
			/* Connect mixer to muxout */
			dResult = NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(stAudEncIns[ulDevId].hMuxOutput[ulIndex]), 
												NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hDspMixer[ulIndex]));
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioOutput_AddInput failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
			
		}
		
	}

	eDRVError = DRV_AUDIO_GetAudioDummyOutputHandle(NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hHwMixer), &audioOutput);
	if(eDRVError == DRV_OK)
	{
		NEXUS_AudioOutput_GetSettings(audioOutput, &audioOutputSettings);
		if(ulDevId == 0) /* Audio Transcoder 1 */
		{
			audioOutputSettings.nco = NEXUS_AudioOutputNco_e1;
			audioOutputSettings.pll = NEXUS_AudioOutputPll_e1;
		}
		else /* Audio Transcoder 2 */
		{
			audioOutputSettings.nco = NEXUS_AudioOutputNco_e2;
			audioOutputSettings.pll = NEXUS_AudioOutputPll_e2;
		}
		dResult = NEXUS_AudioOutput_SetSettings(audioOutput, &audioOutputSettings);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioOutput_SetSettings failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}
	}

	if(bBypass == TRUE)
	{
		stAudEncStatus[ulDevId].bEncoding = FALSE;
	}
	else
	{
		stAudEncStatus[ulDevId].bEncoding = TRUE;
	}
	if(Settings->eTransType == DI_TRANSCODER_SOURCE_WATCH)
	{
		stAudEncStatus[ulDevId].bWatching = TRUE;
	}
	else
	{
		stAudEncStatus[ulDevId].bWatching = FALSE;
	}
	
	stAudEncIns[ulDevId].ulMulAudCnt = Settings->ulMultiAudioCnt;

	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_OK;
	
error_out:
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_ERR;
}

DRV_Error DRV_AUDIO_EncUnsettings(HUINT32 ulDevId)
{
	HINT32		vResult;
	NEXUS_Error dResult;
	DRV_Error	eDRVError = DRV_OK;
	NEXUS_AudioOutput audioOutput;
	NEXUS_AudioOutputSettings audioOutputSettings;	
	HUINT32	ulMultiAudCnt = 0;
	HUINT32 ulIndex = 0;

	vResult = VK_SEM_Get(s_ulAudEncSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	PrintDebug("[%s] ulDevId=%d \n",__FUNCTION__, ulDevId);

	ulMultiAudCnt = stAudEncIns[ulDevId].ulMulAudCnt;

	eDRVError = DRV_AUDIO_GetAudioDummyOutputHandle(NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hHwMixer), &audioOutput);
	if(eDRVError == DRV_OK)
	{
		NEXUS_AudioOutput_GetSettings(audioOutput, &audioOutputSettings);
		audioOutputSettings.nco = NEXUS_AudioOutputNco_e0;
		audioOutputSettings.pll = NEXUS_AudioOutputPll_e0;
		dResult = NEXUS_AudioOutput_SetSettings(audioOutput, &audioOutputSettings);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioOutput_SetSettings failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}
	}

	if(	stAudEncStatus[ulDevId].bEncoding == TRUE)
	{
	
		for(ulIndex = 0; ulIndex < ulMultiAudCnt ; ulIndex++)
		{
			/* Disconnect Dummy output of decoder */
			eDRVError = DRV_AUDIO_DisconnectAudioDummyOutput(NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hDspMixer[ulIndex]));
			if(eDRVError != DRV_OK)
			{
				PrintError("[%s]%d DRV_AUDIO_DisconnectAudioDummyOutput failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}
		}
	}
	
	dResult = NEXUS_AudioMixer_RemoveAllInputs(stAudEncIns[ulDevId].hHwMixer);
	if(dResult != NEXUS_SUCCESS)
	{
		PrintError("[%s]%d NEXUS_AudioMixer_RemoveAllInputs failed! \n",__FUNCTION__,__LINE__);
		goto error_out;
	}		

	/* Disconnect Dummy output of decoder */
	eDRVError = DRV_AUDIO_DisconnectAudioDummyOutput(NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hHwMixer));
	if(eDRVError != DRV_OK)
	{
		PrintError("[%s]%d DRV_AUDIO_DisconnectAudioDummyOutput failed! \n",__FUNCTION__,__LINE__);
		goto error_out;
	}
	NEXUS_AudioMixer_Close(stAudEncIns[ulDevId].hHwMixer);

	for(ulIndex = 0; ulIndex < ulMultiAudCnt ; ulIndex++)
	{

		/* Disconnect input & output of encoder, Close encoder */
		dResult = NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(stAudEncIns[ulDevId].hMuxOutput[ulIndex]));
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioOutput_RemoveAllInputs failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}		
	
		if( stAudEncStatus[ulDevId].bEncoding == TRUE)
		{
			dResult = NEXUS_AudioEncoder_RemoveAllInputs(stAudEncIns[ulDevId].hEncoder[ulIndex]);
			if(dResult != NEXUS_SUCCESS)
			{
				PrintError("[%s]%d NEXUS_AudioEncoder_RemoveAllInputs failed! \n",__FUNCTION__,__LINE__);
				goto error_out;
			}		
			NEXUS_AudioInput_Shutdown(NEXUS_AudioEncoder_GetConnector(stAudEncIns[ulDevId].hEncoder[ulIndex]));
			NEXUS_AudioEncoder_Close(stAudEncIns[ulDevId].hEncoder[ulIndex]);
		}

		/* Disconnect input of Mixer, and then close mixer */
		dResult = NEXUS_AudioMixer_RemoveAllInputs(stAudEncIns[ulDevId].hDspMixer[ulIndex]);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioMixer_RemoveAllInputs failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}			
		NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(stAudEncIns[ulDevId].hDspMixer[ulIndex]));
		NEXUS_AudioMixer_Close(stAudEncIns[ulDevId].hDspMixer[ulIndex]);

		/* Destory Muxoutput */
		NEXUS_AudioOutput_Shutdown(NEXUS_AudioMuxOutput_GetConnector(stAudEncIns[ulDevId].hMuxOutput[ulIndex]));
		NEXUS_AudioMuxOutput_Destroy(stAudEncIns[ulDevId].hMuxOutput[ulIndex]);

		/* Close Transcoding decoder */ 
		if( stAudEncStatus[ulDevId].bEncoding == TRUE)
		{
			NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(stAudEncIns[ulDevId].hDecoder[ulIndex], NEXUS_AudioDecoderConnectorType_eStereo));
		}
		else
		{
			NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(stAudEncIns[ulDevId].hDecoder[ulIndex], NEXUS_AudioDecoderConnectorType_eCompressed));

		}
		NEXUS_AudioDecoder_Close(stAudEncIns[ulDevId].hDecoder[ulIndex]);

		stAudEncIns[ulDevId].hDecoder[ulIndex] = NULL;
		stAudEncIns[ulDevId].hMuxOutput[ulIndex] = NULL;
		stAudEncIns[ulDevId].hDspMixer[ulIndex] = NULL;		
		stAudEncIns[ulDevId].hEncoder[ulIndex] = NULL;	
	}
	
	stAudEncIns[ulDevId].hHwMixer = NULL;
	stAudEncStatus[ulDevId].bEncoding = FALSE;

	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_OK;
	
error_out:
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_ERR;
}

DRV_Error DRV_AUDIO_EncRestart(HUINT32 ulDevId,HUINT32 ulDemuxId)
{
	HINT32		vResult;
	NEXUS_Error dResult;
	HBOOL	bIsPlayback = FALSE;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode;
	DI_TRANSCODER_ENCODE_MODE eEncMode = DI_TRANSCODER_ENCODE_AV;
	HUINT32	ulMultiAudCnt = 0;
	HUINT32 ulIndex = 0;
	
	vResult = VK_SEM_Get(s_ulAudEncSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}
	ulMultiAudCnt = stAudEncIns[ulDevId].ulMulAudCnt;

	if(DRV_TRANSCODER_GetEncodeMode(ulDevId, &eEncMode) != DRV_OK)
	{
		goto error_out;
	}

	for(ulIndex = 0; ulIndex < ulMultiAudCnt; ulIndex++)
	{
		NEXUS_AudioDecoder_Stop(stAudEncIns[ulDevId].hDecoder[ulIndex]);

		if(stAudEncStatus[ulDevId].bWatching == TRUE)
		{
			stAudEncStatus[ulDevId].audioSettings[0].pidChannel = DRV_DEMUX_GetAudioPidChannel(0);
			stAudEncStatus[ulDevId].audioSettings[0].stcChannel = DRV_DEMUX_GetStcChannelHandle();
		}
		else
		{
			if(ulIndex == 0)
			{
				stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId,
											(ulDevId == 0) ? DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_1 : DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_2);
			}
			else
			{
				stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId,
											(ulDevId == 0) ? DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1 : DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2);
			}
			if(stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel == NULL)
			{
				PrintError("[%s]%d DRV_DEMUX_GetPidChannelForTranscode failed! ulDevId %d ulIndex %d\n",__FUNCTION__,__LINE__,ulDevId,ulIndex);
				goto error_out;
			}
			
			stAudEncStatus[ulDevId].audioSettings[ulIndex].stcChannel = DRV_DEMUX_GetSTCChannel(ulDevId+1);//to use the same stcchannel
		}		
		if(eEncMode == DI_TRANSCODER_ENCODE_AUDIO)
		{
			stAudEncStatus[ulDevId].audioSettings[ulIndex].stcChannel = NULL;
		}

		dResult = NEXUS_AudioDecoder_Start(stAudEncIns[ulDevId].hDecoder[ulIndex], &stAudEncStatus[ulDevId].audioSettings[ulIndex]);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioDecoder_Start failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}
		
		bIsPlayback=DRV_DEMUX_IsPlaybackPidChannel(stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel, &ePlaybackMode);

		if(bIsPlayback == TRUE)
		{
			DRV_PVR_PLAY_AudioPidChannelSetting(stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel, (ulIndex==0)?false:true, stAudEncIns[ulDevId].hDecoder[ulIndex],NULL);
		}
	}
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_OK;
		
error_out:
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_ERR;

}

DRV_Error DRV_AUDIO_EncStart(HUINT32 ulDevId, HUINT32 ulDelay)
{
	HINT32		vResult;
	NEXUS_Error dResult;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode;
	HBOOL	bIsPlayback = FALSE;
	HUINT32	ulMultiAudCnt = 0;
	HUINT32 ulIndex = 0;
	
	vResult = VK_SEM_Get(s_ulAudEncSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	ulMultiAudCnt = stAudEncIns[ulDevId].ulMulAudCnt;

	/* Start audio mux output */
	NEXUS_AudioMuxOutput_GetDefaultStartSettings(&stAudEncStatus[ulDevId].audioMuxStartSettings);
	
	/* audio NRT requires mux out to take NRT decode STC */
	stAudEncStatus[ulDevId].audioMuxStartSettings.stcChannel        = DRV_DEMUX_GetSTCChannelTranscode(ulDevId);
	stAudEncStatus[ulDevId].audioMuxStartSettings.presentationDelay = (ulDelay/27000);/* in ms */
	stAudEncStatus[ulDevId].audioMuxStartSettings.nonRealTime       = FALSE;

	for(ulIndex = 0; ulIndex < ulMultiAudCnt ; ulIndex++)
	{
		/* Start audio mux output */
		dResult = NEXUS_AudioMuxOutput_Start(stAudEncIns[ulDevId].hMuxOutput[ulIndex], &stAudEncStatus[ulDevId].audioMuxStartSettings);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioMuxOutput_Start failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}	

		/* Start audio mixer */
		dResult = NEXUS_AudioMixer_Start(stAudEncIns[ulDevId].hDspMixer[ulIndex]);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioMixer_Start failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}

		/* Start audio Decoder */
		dResult = NEXUS_AudioDecoder_Start(stAudEncIns[ulDevId].hDecoder[ulIndex], &stAudEncStatus[ulDevId].audioSettings[ulIndex]);
		if(dResult != NEXUS_SUCCESS)
		{
			PrintError("[%s]%d NEXUS_AudioDecoder_Start failed! \n",__FUNCTION__,__LINE__);
			goto error_out;
		}

		bIsPlayback=DRV_DEMUX_IsPlaybackPidChannel(stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel, &ePlaybackMode);

		if(bIsPlayback == TRUE)
		{
			DRV_PVR_PLAY_AudioPidChannelSetting(stAudEncStatus[ulDevId].audioSettings[ulIndex].pidChannel, (ulIndex==0)?false:true, stAudEncIns[ulDevId].hDecoder[ulIndex],NULL);
		}

	}
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_OK;
	
error_out:
	VK_SEM_Release(s_ulAudEncSemId);
	return DRV_ERR;

}

DRV_Error DRV_AUDIO_EncStop(HUINT32 ulDevId)
{
	DRV_Error	eDRVError = DRV_OK;
	HINT32		vResult;
	HUINT32	ulMultiAudCnt = 0;
	HUINT32 ulIndex = 0;

	vResult = VK_SEM_Get(s_ulAudEncSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	ulMultiAudCnt = stAudEncIns[ulDevId].ulMulAudCnt;

	for(ulIndex = 0; ulIndex < ulMultiAudCnt ; ulIndex++)
	{
		/* Stop audio Decoder */
		NEXUS_AudioDecoder_Stop(stAudEncIns[ulDevId].hDecoder[ulIndex]);

		/* Stop audio mixer */
		NEXUS_AudioMixer_Stop(stAudEncIns[ulDevId].hDspMixer[ulIndex]);

		/* Stop audio mux output */
		NEXUS_AudioMuxOutput_Stop(stAudEncIns[ulDevId].hMuxOutput[ulIndex]);
	}
	VK_SEM_Release(s_ulAudEncSemId);	
	
	return eDRVError;
}

DRV_Error DRV_AUDIO_GetAudioMuxOutput(HUINT32 ulAudioDevId,HUINT32 ulIndex, NEXUS_AudioMuxOutputHandle *muxOutput)
{
	DRV_Error nDrvErr = DRV_OK;
	/* should be changed */
	*muxOutput = stAudEncIns[ulAudioDevId].hMuxOutput[ulIndex];
	return nDrvErr;
}

DRV_Error DRV_AUDIO_GetEncDelayStatus(HUINT32 ulAudioDevId, unsigned *Dee)
{
	NEXUS_Error dResult;
	NEXUS_AudioMuxOutputDelayStatus audioDelayStatus;

	dResult = NEXUS_AudioMuxOutput_GetDelayStatus(stAudEncIns[ulAudioDevId].hMuxOutput[0], 
										stAudEncStatus[ulAudioDevId].encoderSettings.codec, 
										&audioDelayStatus);

	if(dResult != NEXUS_SUCCESS)
	{
		PrintError("[%s]%d NEXUS_AudioMuxOutput_GetDelayStatus failed! \n",__FUNCTION__,__LINE__);
		*Dee = 0;
		return DRV_ERR;
	}
	else
	{
		*Dee = (audioDelayStatus.endToEndDelay * 27000); /* in 27MHz ticks */
		return DRV_OK;
	}
}

DRV_Error DRV_AUDIO_MuxOutputShowStatus(HUINT32 ulAudioDevId)
{
    NEXUS_Error dResult;
    NEXUS_AudioMuxOutputHandle audioMuxOutputHandle;
    NEXUS_AudioMuxOutputStatus audioMuxOutputStatus;
	HUINT32	ulMultiAudCnt = 0;
	HUINT32 ulIndex = 0;

	ulMultiAudCnt = stAudEncIns[ulAudioDevId].ulMulAudCnt;
	for(ulIndex = 0; ulIndex < ulMultiAudCnt ; ulIndex++)
	{

	    audioMuxOutputHandle = stAudEncIns[ulAudioDevId].hMuxOutput[ulIndex];

	    dResult = NEXUS_AudioMuxOutput_GetStatus(audioMuxOutputHandle, &audioMuxOutputStatus);

	    if(dResult != NEXUS_SUCCESS)
	    {
	        PrintError("[%s]%d NEXUS_AudioMuxOutput_GetStatus failed! \n",__FUNCTION__,__LINE__);
	        return DRV_ERR;
	    }
	    
	    PrintError("[%d]pBufferBase = 0x%X\n", ulIndex,audioMuxOutputStatus.pBufferBase);
	    PrintError("[%d]pMetadataBufferBase = 0x%X\n", ulIndex,audioMuxOutputStatus.pMetadataBufferBase);
	    
#if (NEXUS_VERSION >= 1304)        
	    PrintError("[%d]numFrames = 0x%X\n", ulIndex,audioMuxOutputStatus.numFrames);
	    PrintError("[%d]numErrorFrames = 0x%X\n", ulIndex,audioMuxOutputStatus.numErrorFrames);
#endif    
	}
    return DRV_OK;
}
