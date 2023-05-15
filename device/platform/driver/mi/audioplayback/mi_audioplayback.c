/*
 * Gives Audio Playback Interface with output connection
 * No resource management here (There must not be preempted the related resources (e,g AudioPlayback, HDMI, SPDIF and so on)
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>

#include "nexus_base_types.h"
#include "nexus_platform.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_playback.h"
#include "nexus_audio_input.h"

#include "berr.h"
#include "bstd_defs.h"
#include "bdbg.h"

typedef void* OPA_HANDLE;       /* Audio Playback Handle */
typedef void* OPA_RES_HANDLE;  /* Resource Handle */
typedef void* OPA_MIXER_HANDLE; /* Mixer handle */
	
//#define NULL (0)
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define AUDIOPCM_DUMP_PCM_FILE		0
#define	AUDIOPCM_DUMP_FILE_PATH		"/audiopcm_dump.wav"

#define AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE	(0x8000000)
#define AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE		(0x800000)
#define AUDIOPLAYBACK_DEFAULT_FIFO_SIZE			(128 * 1024)
#define AUDIOPLAYBACK_NETFLIX_FIFO_SIZE			(8192)
#define WAVE_HEADER_SIZE						(44)
#define WAVE_VALUE_RIFF							(0x46464952)
#define WAVE_VALUE_WAVE							(0x45564157)
#define WAVE_VALUE_DATA							(0x61746164)

#if 0
#define FILL_NUM_VALUE(a,b,c) 	do{unsigned char i = 0, j = c;\
						  				while(j--){a|=(unsigned char)(*(b + i));i++;j?a<<=8:j;}\
						  			}while(0)
#endif

#define FILL_NUM_VALUE(a,b,c) 	do{	unsigned char i = 0, j = c;\
                             		while(j--){a|=(unsigned int)(*(b+i)<<(i*8));i++;}\
								}while(0)

#define FILL_DATA_2BYTES(a,b)   do{ a=(unsigned long)(*(b+0));\
									a|=(unsigned long)(*(b+1)<<8);\
								}while(0)	

#define FILL_DATA_4BYTES(a,b)   do{ a=(unsigned long)(*(b+0));\
                                    a|=(unsigned long)(*(b+1)<<8);\
                                    a|=(unsigned long)(*(b+2)<<16);\
                                    a|=(unsigned long)(*(b+3)<<24);\
                                }while(0)		
#define READ_4BYTES	(4)
#define READ_2BYTES	(2)

#define MI_AUDIOPCM_JOB_DEFAULT_WAIT_COUNT	(100)
#define MI_AUDIOPCM_JOB_SHORT_DELAY_USEC	(5000)

#define MI_AUDIOPCM_CTX_LOCK		P_MI_AudioPCM_Context_Lock()
#define MI_AUDIOPCM_CTX_UNLOCK		P_MI_AudioPCM_Context_Unlock()

#define MI_AUDIOPCM_RES_LOCK		P_MI_AudioPCM_Resource_Lock()
#define MI_AUDIOPCM_RES_UNLOCK		P_MI_AudioPCM_Resource_Unlock()

#define MI_AUDIOPCM_STAT_LOCK		P_MI_AudioPCM_Status_Lock()
#define MI_AUDIOPCM_STAT_UNLOCK		P_MI_AudioPCM_Status_Unlock()

#if 0
#define PRINT_DEBUG printf
#define PRINT_ERROR printf
#else
#define PRINT_DEBUG(...)
#define PRINT_ERROR(...)
#endif

/* Define ERROR Code */
#define MI_ERR_OK				(0)
#define MI_ERR_FAIL				(-1)
#define MI_ERR_ALREADY_INIT		(-2)
#define MI_ERR_NOT_ALLOCATED	(-3)
#define MI_ERR_UNKNOWN_ERROR	(-99)

typedef enum mi_aud_res_connect_from
{
    eConnectFromUnknown = 0,
    eConnectFromPlayback,
    eConnectFromMixer,
} MI_AUD_RES_CONNECT_TYPE;

typedef enum
{
	eMI_AudioPCM_JobStatus_Canceled = 0,
	eMI_AudioPCM_JobStatus_Stopping,
	eMI_AudioPCM_JobStatus_Ready,
	eMI_AudioPCM_JobStatus_Running,
	//eMI_AudioPCM_JobStatus_Waiting,
} MI_AudioPCM_JobStatus_e;

typedef enum
{
	eMI_AudioPCM_OutputStatus_NeedInit = 0,
	eMI_AudioPCM_OutputStatus_DoNotInit,
} MI_AudioPCM_OutputStatus_e;

typedef enum
{
	eMI_AudioPCM_AudioPCMStatus_Initialized = 0,
	eMI_AudioPCM_AudioPCMStatus_Started,
	eMI_AudioPCM_AudioPCMStatus_Stopped,
	eMI_AudioPCM_AudioPCMStatus_Closed,
	eMI_AudioPCM_AudioPCMStatus_Destroyed,
} MI_AudioPCM_AudioPCMStatus_e;

typedef struct mi_resource_storage
{
    MI_AUD_RES_CONNECT_TYPE     eType;
    NEXUS_AudioOutputSettings   tOrgSetting;
    OPA_MIXER_HANDLE            pMixerHandle;
} MI_AUD_RES_STORAGE;

typedef struct MI_AudioPCM_AudioPCMStatus
{
	MI_AudioPCM_AudioPCMStatus_e eAudioPCMStatus;
	bool			bMute;
	pthread_mutex_t tMutex;
} MI_AudioPCM_AudioPCMStatus_t;

typedef struct MI_AudioPCM_Resource
{
	unsigned int ulAudioMixerHandle;
	pthread_mutex_t tMutex;
} MI_AudioPCM_Resource_t;

typedef struct MI_AudioPCM_Context
{
	void *hAudioPlayback;
	void *hAudioMixer;
	MI_AudioPCM_JobStatus_e eJobStatus;
	MI_AudioPCM_OutputStatus_e eOutputStatus;
	MI_AUD_RES_STORAGE	*ptStorage;
	pthread_mutex_t tMutex;
} MI_AudioPCM_Context_t;

typedef struct MI_AudioPCM_WavRiffHeader
{
	unsigned long riff; 		/* 'RIFF' */
	unsigned long riffCSize;	/* size in bytes of file - 8 */
	unsigned long wave; 		/* 'WAVE' */
	unsigned long fmt;			/* 'fmt ' */
	unsigned long headerLen;	/* header length (should be 16 for PCM) */
	unsigned short format;		/* 1 - pcm */
	unsigned short channels;	/* 1 - mono, 2 - sterio */
	unsigned long samplesSec;	/* samples / second */
	unsigned long bytesSec; 	/* bytes / second */
	unsigned short chbits;		/* channels * bits/sample /8 */
	unsigned short bps; 		/* bits per sample (8 or 16) */
								/* Extensible format */
	unsigned short cbSize;		/* 2 Size of the extension (0 or 22)  */
	unsigned short wValidBitsPerSample; /* 2 Number of valid bits  */
	unsigned short dwChannelMask; /* 4 Speaker position mask  */
	unsigned char SubFormat[16];  /* SubFormat */

	unsigned long dataSig;		/* 'data' */
	unsigned long dataLen;		/* length of data */
} MI_AudioPCM_WavRiffHeader_t;

static MI_AudioPCM_Context_t s_stAudioPCMContext;
static MI_AudioPCM_Resource_t s_stAudioPCMResource;
static MI_AudioPCM_AudioPCMStatus_t s_stAudioPCMStatus;
static bool s_bInitialized = FALSE;

void MI_AudioPCM_SetAudioPCMStatus(MI_AudioPCM_AudioPCMStatus_e eAudioPCMStatus);
int MI_AudioPCM_GetAudioPCMStatus(void);
void MI_AudioPCM_SetMuteStatus(bool bMute);
int MI_AudioPCM_GetMuteStatus(bool *pbMute);


#define ____NATIVE_NEXUS_MAPPING_INTERFACE___
OPA_HANDLE MI_AudioPlayback_Open(int szFifo)
{
	int nPbIdx;
	NEXUS_AudioPlaybackHandle pHandle = NULL;
	PRINT_DEBUG("The maximum number of audio playback on this chipset is %d\n", NEXUS_NUM_AUDIO_PLAYBACKS);
    NEXUS_AudioPlaybackOpenSettings tSettings;
	
    NEXUS_AudioPlayback_GetDefaultOpenSettings(&tSettings);
    
	if(szFifo > 0)
	{		
		PRINT_DEBUG("Audio Playback : Detect default FIFO size is (%d) ---> Changed (%d)\n", tSettings.fifoSize, szFifo);
		tSettings.fifoSize = szFifo;
	}
	tSettings.threshold = 0;
    
	for(nPbIdx = 0; nPbIdx < NEXUS_NUM_AUDIO_PLAYBACKS; nPbIdx++)
	{
        pHandle = NEXUS_AudioPlayback_Open(nPbIdx, &tSettings);
		if(pHandle)
		{
			PRINT_DEBUG("%d th audio playback is assigned\n", nPbIdx);
			break;
		}
	}
	if(!pHandle)
	{
		PRINT_DEBUG("Error : There are no remaining audio playback resources!\n");
	}
	return pHandle;
}

void MI_AudioPlayback_Close(OPA_HANDLE pHandle)
{
	NEXUS_AudioPlayback_Close( (NEXUS_AudioPlaybackHandle)pHandle );
	return;
}

int MI_AudioPlayback_GetBuffer(OPA_HANDLE pHandle, void**pBuffer, size_t *pSize)
{
	NEXUS_Error nRet = NEXUS_SUCCESS;

	if(pBuffer == NULL)
	{
		PRINT_DEBUG("@@@@@ [%s:%d] pBuffer is NULL\n", __func__, __LINE__);
		return FALSE;
	}

	if(pSize == NULL)
	{
		PRINT_DEBUG("@@@@@ [%s:%d] pSize is NULL\n", __func__, __LINE__);
		return FALSE;
	}

	nRet = NEXUS_AudioPlayback_GetBuffer( (NEXUS_AudioPlaybackHandle)pHandle, pBuffer, pSize );
	if(nRet != NEXUS_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;
}

int MI_AudioPlayback_ReadComplete(OPA_HANDLE pHandle, size_t szWritten)
{
	NEXUS_Error nRet = NEXUS_AudioPlayback_ReadComplete( (NEXUS_AudioPlaybackHandle)pHandle, szWritten);
	if(nRet != NEXUS_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;
}

int MI_AudioPlayback_Start(OPA_HANDLE pHandle, int nSampleRate, int nBitsPerSample, int bSigned, int bStereo)
{
	NEXUS_AudioPlaybackStartSettings playbackSettings;

	NEXUS_AudioPlayback_GetDefaultStartSettings(&playbackSettings);
    playbackSettings.sampleRate = nSampleRate;
    playbackSettings.bitsPerSample = nBitsPerSample;
    playbackSettings.signedData = bSigned;
    playbackSettings.stereo = bStereo;
	playbackSettings.dataCallback.callback = NULL;
	playbackSettings.dataCallback.context = NULL;
	playbackSettings.dataCallback.param = 0;

	if( NEXUS_SUCCESS != NEXUS_AudioPlayback_Start( (NEXUS_AudioPlaybackHandle)pHandle, &playbackSettings) )
	{
		PRINT_DEBUG(" Error to start playback!\n");
		return FALSE;
	}
	
	return TRUE;
}

void MI_AudioPlayback_Flush(OPA_HANDLE pHandle)
{
    NEXUS_AudioPlayback_Flush( (NEXUS_AudioPlaybackHandle)pHandle );
    return;
}

void MI_AudioPlayback_Stop(OPA_HANDLE pHandle)
{
	NEXUS_AudioPlayback_Stop( (NEXUS_AudioPlaybackHandle)pHandle );
	return;
}

OPA_MIXER_HANDLE MI_AudioPlayback_PrepareMixer(OPA_HANDLE pHandle)
{
    /* Create Mixer */
    NEXUS_AudioMixerHandle  pAudioMixer = NULL;
    NEXUS_AudioMixerSettings tSettings;
    NEXUS_AudioInput        input;

    if(!pHandle)
    {
        return NULL;
    }
    
	/* Open mixer (HW type or DSP type) */
	NEXUS_AudioMixer_GetDefaultSettings(&tSettings);
	tSettings.mixUsingDsp = FALSE;
	pAudioMixer = NEXUS_AudioMixer_Open(&tSettings);

    /* Connect playback input to Mixer */
    input = NEXUS_AudioPlayback_GetConnector(pHandle);
    NEXUS_AudioMixer_AddInput(pAudioMixer, input);

    return pAudioMixer;   
}

int MI_AudioPlayback_PrepareMixerWithHandle(OPA_HANDLE pAudioPlaybackHandle, OPA_MIXER_HANDLE pAudioMixerHandle)
{
	int nRet = 0;
	NEXUS_Error eErr = NEXUS_SUCCESS;
	NEXUS_AudioMixerHandle hAudioMixer = NULL;
	NEXUS_AudioMixerSettings tSettings;
	NEXUS_AudioInput input;

	if(pAudioPlaybackHandle == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pAudioPlaybackHandle is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
	if(pAudioMixerHandle == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pAudioMixerHandle is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	hAudioMixer = pAudioMixerHandle;

	NEXUS_AudioMixer_GetSettings(hAudioMixer, &tSettings);
	if(tSettings.mixUsingDsp)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Audio mixer is DSP mode.\033[0m\n", __func__, __LINE__);
	}
	
	input = NEXUS_AudioPlayback_GetConnector(pAudioPlaybackHandle);
	eErr = NEXUS_AudioMixer_AddInput(hAudioMixer, input);
	if(eErr != NEXUS_SUCCESS)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to add input to AudioMixer\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	return nRet;
}

int MI_AudioPlayback_RemoveInputFromMixer(OPA_HANDLE pAudioPlaybackHandle, OPA_MIXER_HANDLE pMixerHandle)
{
	int nRet = 0;
	NEXUS_Error eErr = NEXUS_SUCCESS;
	NEXUS_AudioInput input;

	if(pAudioPlaybackHandle == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pAudioPlaybackHandle is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pMixerHandle == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pMixerHandle is NULL\033[0m\n", __func__, __LINE__);
//		nRet = MI_ERR_FAIL;
		nRet = MI_ERR_OK;
		goto FUNCTION_EXIT;
	}

	input = NEXUS_AudioPlayback_GetConnector(pAudioPlaybackHandle);
	eErr = NEXUS_AudioMixer_RemoveInput(pMixerHandle, input);
	if(eErr != NEXUS_SUCCESS)
	{
		if(eErr == NEXUS_INVALID_PARAMETER)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback input(%p) is not connected to mixer(%p), eErr = %d\033[0m\n", __func__, __LINE__, (void *)input, pMixerHandle, eErr);
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] This case is not error..\033[0m\n", __func__, __LINE__);
		}
		else
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove input from AudioMixer, eErr = %d\033[0m\n", __func__, __LINE__, eErr);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}
	}
	
FUNCTION_EXIT:
	return nRet;
}

void MI_AudioPlayback_RemoveMixer(OPA_MIXER_HANDLE pHandle)
{
    /* Release Mixer */
    if(pHandle)
    {
		if(NEXUS_SUCCESS != NEXUS_AudioMixer_RemoveAllInputs(pHandle))
		{
			PRINT_DEBUG("Failed to remove all input for DAC\n");
		}
        NEXUS_AudioMixer_Close( (NEXUS_AudioMixerHandle) pHandle);
    }
}

OPA_RES_HANDLE MI_AudioPlayback_PrepareOutputPath(OPA_HANDLE pPlaybackHandle, OPA_MIXER_HANDLE pMixerHandle)
{
	NEXUS_PlatformConfiguration platformConfig;
   	MI_AUD_RES_STORAGE *pResource = NULL;
	NEXUS_HdmiOutputHandle	hHdmi = NULL;
	NEXUS_AudioDacHandle	hDac = NULL;
	NEXUS_SpdifOutputHandle	hSpdif = NULL;
	NEXUS_I2sOutputHandle	hI2s = NULL;
	NEXUS_AudioInput input = NULL;
	NEXUS_AudioOutput output = NULL;
	NEXUS_AudioOutputSettings tSettings;
	unsigned int nRet = 0;
	
	if(!pPlaybackHandle && !pMixerHandle)
	{
    	PRINT_DEBUG("Playback handle is NULL\n");
		goto __error;
	}

	pResource = calloc( 1, sizeof(MI_AUD_RES_STORAGE) );
	if(!pResource)
	{
		PRINT_DEBUG("Out of memory!\n");
		goto __error;
	}

	NEXUS_Platform_GetConfiguration(&platformConfig);

	/* We assum there are only one outputs */
	hHdmi = platformConfig.outputs.hdmi[0];
	if(NEXUS_NUM_AUDIO_DACS)
	{
		hDac = platformConfig.outputs.audioDacs[0];
	}
	if(NEXUS_NUM_SPDIF_OUTPUTS)
	{
		hSpdif = platformConfig.outputs.spdif[0];
	}
	if(NEXUS_NUM_I2S_OUTPUTS)
	{
		hI2s = platformConfig.outputs.i2s[0];
	}

    if(pPlaybackHandle && !pMixerHandle)
    {
		input = NEXUS_AudioPlayback_GetConnector(pPlaybackHandle);
        PRINT_DEBUG("Output connected from Playback\n");
    }
    else
    {
       	input = NEXUS_AudioMixer_GetConnector(pMixerHandle);
        PRINT_DEBUG("Output connected from Mixer\n");
    }
	if(!input)
	{
		PRINT_DEBUG("Can not get input connector\n");
		goto __error;
	}

	if(hHdmi)
	{		
		output = NEXUS_HdmiOutput_GetAudioConnector(hHdmi);
		if(!output)
		{
			PRINT_DEBUG("Can not get HDMI output connector\n");
			goto __error;
		}

		NEXUS_AudioOutput_GetSettings(output, &tSettings);
		memcpy(&pResource->tOrgSetting, &tSettings, sizeof(NEXUS_AudioOutputSettings));
		tSettings.nco = NEXUS_AudioOutputNco_e1;
		tSettings.muted = 0;

		if(NEXUS_SUCCESS != NEXUS_AudioOutput_SetSettings(output, &tSettings))
		{
			PRINT_DEBUG("Failed to set output settings\n");
			goto __error;
		}

		nRet = NEXUS_AudioOutput_AddInput(output, input);
		if(nRet != NEXUS_SUCCESS && nRet != NEXUS_NOT_SUPPORTED)
		{
			PRINT_DEBUG("Failed to add input to HDMI\n");
			goto __error;
		}
	}
	else
	{
		PRINT_DEBUG("HDMI Handle is NULL. This handle is essential!\n");
		goto __error;
	}

	if(hDac)
	{
		output = NEXUS_AudioDac_GetConnector(hDac);
		if(!output)
		{
			PRINT_DEBUG("Can not get DAC output connector\n");
			goto __error;
		}

		NEXUS_AudioOutput_GetSettings(output, &tSettings);
		memcpy(&pResource->tOrgSetting, &tSettings, sizeof(NEXUS_AudioOutputSettings));
		tSettings.muted = 0;

		if(NEXUS_SUCCESS != NEXUS_AudioOutput_SetSettings(output, &tSettings))
		{
			PRINT_DEBUG("Failed to set output settings\n");
			goto __error;
		}

		nRet = NEXUS_AudioOutput_AddInput(output, input);
		if(nRet != NEXUS_SUCCESS && nRet != NEXUS_NOT_SUPPORTED)
		{
			PRINT_DEBUG("Failed to add input to DAC\n");
			goto __error;
		}
	}
	if(hSpdif)
	{
		output = NEXUS_SpdifOutput_GetConnector(hSpdif);
		if(!output)
		{
			PRINT_DEBUG("Can not get SPDIF output connector\n");
			goto __error;
		}
		
		NEXUS_AudioOutput_GetSettings(output, &tSettings);
		memcpy(&pResource->tOrgSetting, &tSettings, sizeof(NEXUS_AudioOutputSettings));
		tSettings.muted = 0;
		if(NEXUS_SUCCESS != NEXUS_AudioOutput_SetSettings(output, &tSettings))
		{
			PRINT_DEBUG("Failed to set output settings\n");
			goto __error;
		}
		
		nRet = NEXUS_AudioOutput_AddInput(output, input);
		if(nRet != NEXUS_SUCCESS && nRet != NEXUS_NOT_SUPPORTED)
		{
			PRINT_DEBUG("Failed to add input to SPDIF\n");
			goto __error;
		}
	}
	if(hI2s)
	{
		output = NEXUS_I2sOutput_GetConnector(hI2s);
		if(!output)
		{
			PRINT_DEBUG("Can not get I2S output connector\n");
			goto __error;
		}
		nRet = NEXUS_AudioOutput_AddInput(output, input);
		if(nRet != NEXUS_SUCCESS && nRet != NEXUS_NOT_SUPPORTED)
		{
			PRINT_DEBUG("Failed to add input to I2S\n");
			goto __error;
		}
	}
	
	return pResource;

__error:
    /* Revert all output resources */
	if(hHdmi)
	{
		output = NEXUS_HdmiOutput_GetAudioConnector(hHdmi);
		if(!output)
		{
			PRINT_DEBUG("Can not get HDMI output connector\n");
			goto __error;
		}

        memcpy(&tSettings, &pResource->tOrgSetting, sizeof(NEXUS_AudioOutputSettings));
        if(NEXUS_SUCCESS != NEXUS_AudioOutput_SetSettings(output, &tSettings))
        {
            PRINT_DEBUG("Failed to revert original output setting for HDMI!\n");
            goto __error;
        }
        if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
        {
            PRINT_DEBUG("Failed to remove all input for HDMI\n");
            goto __error;
        }
	}
	if(hDac)
	{
		output = NEXUS_AudioDac_GetConnector(hDac);
		if(!output)
		{
			PRINT_DEBUG("Can not get DAC output connector\n");
			goto __error;
		}
		if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
		{
			PRINT_DEBUG("Failed to remove all input for DAC\n");
			goto __error;
		}
	}
	if(hSpdif)
	{
		output = NEXUS_SpdifOutput_GetConnector(hSpdif);
		if(!output)
		{
			PRINT_DEBUG("Can not get SPDIF output connector\n");
			goto __error;
		}
		if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
		{
			PRINT_DEBUG("Failed to remove all input for DAC\n");
			goto __error;
		}
	}
	if(hI2s)
	{
		output = NEXUS_I2sOutput_GetConnector(hI2s);
		if(!output)
		{
			PRINT_DEBUG("Can not get I2S output connector\n");
			goto __error;
		}
		if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
		{
			PRINT_DEBUG("Failed to remove all input for DAC\n");
			goto __error;
		}
	}    
	if(pResource)
	{
		free(pResource);
	}
	return NULL;
}

int MI_AudioPlayback_RemoveOutputPath(OPA_RES_HANDLE pResHandle)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_AudioOutput output = NULL;
    NEXUS_AudioOutputSettings tSettings;
    MI_AUD_RES_STORAGE *pResource = NULL;
	NEXUS_Platform_GetConfiguration(&platformConfig);

	if(!pResHandle)
	{
		PRINT_DEBUG("Resource handle is NULL\n");
		return FALSE;
	}
    pResource = pResHandle;

	output = NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]);
	memcpy(&tSettings, &pResource->tOrgSetting, sizeof(NEXUS_AudioOutputSettings));
	if(NEXUS_SUCCESS != NEXUS_AudioOutput_SetSettings(output, &tSettings))
	{
		PRINT_DEBUG("Failed to revert original output setting for HDMI!\n");
//		goto __error;
	}
	if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
	{
		PRINT_DEBUG("Failed to remove all input for HDMI\n");
//		goto __error;
	}
	if(NEXUS_NUM_AUDIO_DACS)
	{
		output = NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]);
		if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
		{
			PRINT_DEBUG("Failed to remove all input for DAC\n");
//			goto __error;
		}
	}
	if(NEXUS_NUM_SPDIF_OUTPUTS)
	{
		output = NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]);
		if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
		{
			PRINT_DEBUG("Failed to remove all input for SPDIF\n");
//			goto __error;
		}
	}
	if(NEXUS_NUM_I2S_OUTPUTS)
	{
		output = NEXUS_I2sOutput_GetConnector(platformConfig.outputs.i2s[0]);
		if(NEXUS_SUCCESS != NEXUS_AudioOutput_RemoveAllInputs(output))
		{
			PRINT_DEBUG("Failed to remove all input for I2S\n");
//			goto __error;
		}
	}

	return TRUE;

/*
__error:
	return FALSE;
*/	
}

int MI_AudioPlayback_GetQueuedBytes(OPA_HANDLE pHandle)
{
    NEXUS_AudioPlaybackStatus status;
    if(!pHandle)
    {
        return MI_ERR_FAIL;
    }

    NEXUS_AudioPlayback_GetStatus(pHandle, &status);    

    //PRINT_DEBUG("Status : ququedByte (%d), playedByte (%d)\n", status.queuedBytes, status.playedBytes);
    return (int)status.queuedBytes;
}

int MI_AudioPlayback_IsPlaybackStarted(OPA_HANDLE pHandle)
{
    NEXUS_AudioPlaybackStatus status;
    if(!pHandle)
    {
        return MI_ERR_FAIL;
    }

    NEXUS_AudioPlayback_GetStatus(pHandle, &status);    

    //PRINT_DEBUG("Status : ququedByte (%d), playedByte (%d)\n", status.queuedBytes, status.playedBytes);
    return (int)status.started;
}

int MI_AudioPlayback_IsConnectedToInput(OPA_HANDLE hAudioPlayback, OPA_MIXER_HANDLE hAudioMixer, bool *pbConnected)
{
	int nRet = 0;
	bool bConnected = FALSE;
	NEXUS_AudioInput hInputAudioPlayback = NULL, hInputAudioMixer = NULL;
	
	if(hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(hAudioMixer == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hAudioMixer is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pbConnected == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pbConnected is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	hInputAudioPlayback = NEXUS_AudioPlayback_GetConnector(hAudioPlayback);
	hInputAudioMixer = NEXUS_AudioMixer_GetConnector(hAudioMixer);

	NEXUS_AudioInput_IsConnectedToInput(hInputAudioPlayback, hInputAudioMixer, &bConnected);

	*pbConnected = bConnected;

FUNCTION_EXIT:
	return nRet;
}

int MI_AudioPlayback_SetMute(OPA_HANDLE hAudioPlayback, OPA_RES_HANDLE hResource, bool bMute)
{
	int nRet = MI_ERR_OK;
	NEXUS_Error eNexusErr = NEXUS_SUCCESS;
	NEXUS_PlatformConfiguration platformConfig;
   	NEXUS_HdmiOutputHandle	hHdmi = NULL;
	NEXUS_AudioDacHandle	hDac = NULL;
	NEXUS_SpdifOutputHandle	hSpdif = NULL;
	NEXUS_I2sOutputHandle	hI2s = NULL;
	NEXUS_AudioOutput output = NULL;
	NEXUS_AudioOutputSettings tSettings;
	MI_AUD_RES_STORAGE *ptStorage = NULL;

	if(hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	ptStorage = hResource;

	NEXUS_Platform_GetConfiguration(&platformConfig);

	hHdmi = platformConfig.outputs.hdmi[0];
	if(NEXUS_NUM_AUDIO_DACS)
	{
		hDac = platformConfig.outputs.audioDacs[0];
	}
	if(NEXUS_NUM_SPDIF_OUTPUTS)
	{
		hSpdif = platformConfig.outputs.spdif[0];
	}
	if(NEXUS_NUM_I2S_OUTPUTS)
	{
		hI2s = platformConfig.outputs.i2s[0];
	}

	if(hHdmi)
	{
		output = NEXUS_HdmiOutput_GetAudioConnector(hHdmi);
		if(output == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get HDMI output connector\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}

		NEXUS_AudioOutput_GetSettings(output, &tSettings);
		tSettings.muted = bMute;
		
		if(ptStorage == NULL)
		{
			PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] ptStorage is NULL, we will backup original output settings later.\033[0m\n", __func__, __LINE__);
		}
		else
		{
			/* Renew original mute setting */
			ptStorage->tOrgSetting.muted = tSettings.muted;	
		}

		eNexusErr = NEXUS_AudioOutput_SetSettings(output, &tSettings);
		if(eNexusErr != NEXUS_SUCCESS)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioOutput settings, eNexusErr = %d\033[0m\n", __func__, __LINE__, eNexusErr);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}
	}
	else
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hHdmi is NULL, this handle is essential!\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(hDac)
	{
		output = NEXUS_AudioDac_GetConnector(hDac);
		if(output == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioDac output connector\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}

		NEXUS_AudioOutput_GetSettings(output, &tSettings);
		tSettings.muted = bMute;
		
		if(ptStorage == NULL)
		{
			PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] ptStorage is NULL, we will backup original output settings later.\033[0m\n", __func__, __LINE__);
		}
		else
		{
			/* Renew original mute setting */
			ptStorage->tOrgSetting.muted = tSettings.muted;	
		}

		eNexusErr = NEXUS_AudioOutput_SetSettings(output, &tSettings);
		if(eNexusErr != NEXUS_SUCCESS)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioOutput settings, eNexusErr = %d\033[0m\n", __func__, __LINE__, eNexusErr);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}	
	}
	
	if(hSpdif)
	{
		output = NEXUS_SpdifOutput_GetConnector(hSpdif);
		if(output == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get SPDIF output connector\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}

		NEXUS_AudioOutput_GetSettings(output, &tSettings);
		tSettings.muted = bMute;

		if(ptStorage == NULL)
		{
			PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] ptStorage is NULL, we will backup original output settings later.\033[0m\n", __func__, __LINE__);
		}
		else
		{
			/* Renew original mute setting */
			ptStorage->tOrgSetting.muted = tSettings.muted;	
		}
		
		eNexusErr = NEXUS_AudioOutput_SetSettings(output, &tSettings);
		if(eNexusErr != NEXUS_SUCCESS)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioOutput settings, eNexusErr = %d\033[0m\n", __func__, __LINE__, eNexusErr);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}	
	}
	
	if(hI2s)
	{
		output = NEXUS_I2sOutput_GetConnector(hI2s);
		if(output == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get I2S output connector\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;			
		}

		NEXUS_AudioOutput_GetSettings(output, &tSettings);
		tSettings.muted = bMute;

		if(ptStorage == NULL)
		{
			PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] ptStorage is NULL, we will backup original output settings later.\033[0m\n", __func__, __LINE__);
		}
		else
		{
			/* Renew original mute setting */
			ptStorage->tOrgSetting.muted = tSettings.muted;	
		}

		eNexusErr = NEXUS_AudioOutput_SetSettings(output, &tSettings);
		if(eNexusErr != NEXUS_SUCCESS)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioOutput settings, eNexusErr = %d\033[0m\n", __func__, __LINE__, eNexusErr);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}	
	}

FUNCTION_EXIT:
	return nRet;
}

int MI_AudioPlayback_GetVolume(OPA_HANDLE hAudioPlayback, int *pnLeftVolume, int *pnRightVolume)
{
	int nRet = MI_ERR_OK;
	NEXUS_AudioPlaybackSettings stAudioPlaybackSettings;

	if(hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
	if(pnLeftVolume == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pnLeftVolume is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pnRightVolume == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pnRightVolume is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	NEXUS_AudioPlayback_GetSettings(hAudioPlayback, &stAudioPlaybackSettings);
	
	*pnLeftVolume = stAudioPlaybackSettings.leftVolume;
	*pnRightVolume = stAudioPlaybackSettings.rightVolume;

FUNCTION_EXIT:
	return nRet;
}

int MI_AudioPlayback_SetVolume(OPA_HANDLE hAudioPlayback, int nLeftVolume, int nRightVolume)
{
	int nRet = MI_ERR_OK;
	NEXUS_AudioPlaybackSettings stAudioPlaybackSettings;

	if(hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	NEXUS_AudioPlayback_GetSettings(hAudioPlayback, &stAudioPlaybackSettings);

	stAudioPlaybackSettings.leftVolume = nLeftVolume;
	stAudioPlaybackSettings.rightVolume = nRightVolume;

	NEXUS_AudioPlayback_SetSettings(hAudioPlayback, &stAudioPlaybackSettings);

FUNCTION_EXIT:
	return nRet;
}

#define ____WEBAUDIO_INTERFACE___

/////////////////////////////////////////////////////////////////
/////	Web Audio  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

static void P_MI_AudioPCM_Context_Lock(void)
{
	int nRet = MI_ERR_OK;
	nRet = pthread_mutex_lock(&s_stAudioPCMContext.tMutex);
	if(nRet)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Failed to lock mutex!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}
	return;
}

static void P_MI_AudioPCM_Context_Unlock(void)
{
	int nRet = MI_ERR_OK;
	nRet = pthread_mutex_unlock (&s_stAudioPCMContext.tMutex);
	if(nRet)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Failed to unlock mutex!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}
	return;
}

static void P_MI_AudioPCM_Resource_Lock(void)
{
	int nRet = MI_ERR_OK;
	nRet = pthread_mutex_lock(&s_stAudioPCMResource.tMutex); 
	if(nRet)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Failed to lock mutex!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}
	return;
}

static void P_MI_AudioPCM_Resource_Unlock(void)
{
	int nRet = MI_ERR_OK;
	nRet = pthread_mutex_unlock (&s_stAudioPCMResource.tMutex);
	if(nRet)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Failed to unlock mutex!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}
	return;
}

static void P_MI_AudioPCM_Status_Lock(void)
{
	int nRet = MI_ERR_OK;
	nRet = pthread_mutex_lock (&s_stAudioPCMStatus.tMutex);
	if(nRet)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Failed to lock mutex!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}
	return;	
}

static void P_MI_AudioPCM_Status_Unlock(void)
{
	int nRet = MI_ERR_OK;
	nRet = pthread_mutex_unlock (&s_stAudioPCMStatus.tMutex);
	if(nRet)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Failed to unlock mutex!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}
	return;	
}

static MI_AudioPCM_Context_t* P_MI_AudioPCM_GetContext(void)
{
	return &s_stAudioPCMContext;
}

static MI_AudioPCM_Resource_t* P_MI_AudioPCM_GetResource(void)
{
	return &s_stAudioPCMResource;
}

static int P_MI_AudioPCM_AudioMixer_Acquire(void **ppHandle)
{
	int nRet = MI_ERR_OK;

	if(ppHandle == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] ppHandle is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
	MI_AUDIOPCM_RES_LOCK;

	*ppHandle = (void *)s_stAudioPCMResource.ulAudioMixerHandle;
	
	MI_AUDIOPCM_RES_UNLOCK;
	
FUNCTION_EXIT:
	return nRet;
}

int P_MI_AudioPCM_RestoreInputToMixer(OPA_HANDLE hAudioPlayback, OPA_MIXER_HANDLE hAudioMixer)
{
	int nRet = 0;
	bool bConnected = FALSE;

	if(hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(hAudioMixer == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] hAudioMixer is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_IsConnectedToInput()\033[0m\n", __func__, __LINE__);
	nRet = MI_AudioPlayback_IsConnectedToInput(hAudioPlayback, hAudioMixer, &bConnected);
	if(nRet < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to check if AudioPlayback connects to mixer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	if(!bConnected)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] AudioPlayback is not connected to AudioMixer\033[0m\n", __func__, __LINE__);
		PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_PrepareMixerWithHandle()\033[0m\n", __func__, __LINE__);
		nRet = MI_AudioPlayback_PrepareMixerWithHandle(hAudioPlayback, hAudioMixer);
		if(nRet < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to prepare mixer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			goto FUNCTION_EXIT;
		}
	}
	else
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] already connected AudioPlayback to AudioMixer\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	return nRet;
}

int P_MI_AudioPCM_DetachOutput(MI_AudioPCM_Context_t *pstContext)
{
	int nRet = MI_ERR_OK, nStart = 0;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->eJobStatus > eMI_AudioPCM_JobStatus_Stopping)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Stopping;
	}

	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
	}
	else
	{
		nStart = MI_AudioPlayback_IsPlaybackStarted(pstContext->hAudioPlayback);
		if(nStart < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to check if AudioPlayback starts\033[0m\n", __func__, __LINE__);
		}
		else if(!nStart)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is already stopped. No need to detach\033[0m\n", __func__, __LINE__);
			goto FUNCTION_EXIT;
		}
		else
		{
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Stop()\033[0m\n", __func__, __LINE__);
			MI_AudioPlayback_Stop(pstContext->hAudioPlayback);
		}
	}

	pstContext->eOutputStatus = eMI_AudioPCM_OutputStatus_DoNotInit;
	
	if(pstContext->ptStorage == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->ptStorage is NULL\033[0m\n", __func__, __LINE__);
	}
	else
	{
		PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_RemoveOutputPath()\033[0m\n", __func__, __LINE__);
		nRet = MI_AudioPlayback_RemoveOutputPath(pstContext->ptStorage);
		if(!nRet)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove output path\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}

		if(pstContext->ptStorage != NULL)
		{
			free(pstContext->ptStorage);
			pstContext->ptStorage = NULL;
		}
	}

FUNCTION_EXIT:
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int P_MI_AudioPCM_AttachOutput(MI_AudioPCM_Context_t *pstContext)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_AudioPCMStatus_e eAudioPCMStatus = eMI_AudioPCM_AudioPCMStatus_Initialized;
	bool bMute = FALSE;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->hAudioMixer == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioMixer is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	eAudioPCMStatus = MI_AudioPCM_GetAudioPCMStatus();
	if(eAudioPCMStatus > eMI_AudioPCM_AudioPCMStatus_Stopped)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] eAudioPCMStatus is %d, so skip to prepare output..\033[0m\n", __func__, __LINE__, eAudioPCMStatus);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if( pstContext->eOutputStatus == eMI_AudioPCM_OutputStatus_DoNotInit )
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Previous detach request does not handle output. So do not handle output here\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;	
	}

	if(pstContext->ptStorage != NULL)
	{
		free(pstContext->ptStorage);
		pstContext->ptStorage = NULL;
	}

	PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_PrepareOutputPath()\033[0m\n", __func__, __LINE__);
	pstContext->ptStorage = MI_AudioPlayback_PrepareOutputPath(pstContext->hAudioPlayback, pstContext->hAudioMixer);
	if(pstContext->ptStorage == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->ptStorage is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
	/* We need to call this code because we set [muted] value to 0 forcibly at MI_AudioPlayback_PrepareOutputPath() */
	nRet = MI_AudioPCM_GetMuteStatus(&bMute);
	if(nRet != MI_ERR_OK)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get mute value, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to start AudioPCM, so do not return error.\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_OK;
	}
	else
	{
		nRet = MI_AudioPlayback_SetMute(pstContext->hAudioPlayback, pstContext->ptStorage, bMute);
		if(nRet != MI_ERR_OK)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set mute value, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to start AudioPCM, so do not return error.\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_OK;
		}
	}

FUNCTION_EXIT:
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int P_MI_AudioPCM_Stop(MI_AudioPCM_Context_t *pstContext)
{
	int nRet = MI_ERR_OK, nStart = 0;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->eJobStatus >= eMI_AudioPCM_JobStatus_Ready)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Canceled;
	}

	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
	}
	else
	{
		nStart = MI_AudioPlayback_IsPlaybackStarted(pstContext->hAudioPlayback);
		if(nStart < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get if AudioPlayback starts, nStart = %d\033[0m\n", __func__, __LINE__, nStart);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}
		else if(nStart == 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback already stopped!\033[0m\n", __func__, __LINE__);
		}
		else
		{
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Stop()\033[0m\n", __func__, __LINE__);
			MI_AudioPlayback_Stop(pstContext->hAudioPlayback);
		}
	}

	if(pstContext->eOutputStatus != eMI_AudioPCM_OutputStatus_DoNotInit)
	{
		pstContext->eOutputStatus = eMI_AudioPCM_OutputStatus_NeedInit;

		if(pstContext->ptStorage == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->ptStorage is NULL\033[0m\n", __func__, __LINE__);
		}
		else
		{
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_RemoveOutputPath()\033[0m\n", __func__, __LINE__);
			nRet = MI_AudioPlayback_RemoveOutputPath(pstContext->ptStorage);
			if(!nRet)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove output path, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			else
			{
				nRet = MI_ERR_OK;
			}
			
			if(pstContext->ptStorage != NULL)
			{
				free(pstContext->ptStorage);
				pstContext->ptStorage = NULL;
			}
		}
	}
	

FUNCTION_EXIT:
	if(!nRet)
	{
		MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Stopped);
	}
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

void MI_AudioPCM_SetAudioPCMStatus(MI_AudioPCM_AudioPCMStatus_e eAudioPCMStatus)
{
	MI_AUDIOPCM_STAT_LOCK;
	s_stAudioPCMStatus.eAudioPCMStatus = eAudioPCMStatus;
	MI_AUDIOPCM_STAT_UNLOCK;

	return;
}

int MI_AudioPCM_GetAudioPCMStatus(void)
{
	MI_AudioPCM_AudioPCMStatus_e eAudioPCMStatus = eMI_AudioPCM_AudioPCMStatus_Initialized;

	MI_AUDIOPCM_STAT_LOCK;
 	eAudioPCMStatus = s_stAudioPCMStatus.eAudioPCMStatus;
	MI_AUDIOPCM_STAT_UNLOCK;

	return (int)eAudioPCMStatus;
}

static int P_MI_AudioPCM_Open(int szfifo)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = NULL;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	MI_AUDIOPCM_CTX_LOCK;

	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->hAudioPlayback != NULL)
	{
		PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] hAudioPlayback(%p) is NOT NULL, Before MI_AudioPlayback_Close()\033[0m\n", __func__, __LINE__, pstContext->hAudioPlayback);
		MI_AudioPlayback_Close(pstContext->hAudioPlayback);
	}
	
	pstContext->hAudioPlayback = MI_AudioPlayback_Open(szfifo);
	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to open AudioPlayback\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

#if AUDIOPCM_DUMP_PCM_FILE
void P_MI_AudioPCM_DumpPCMFile(void *pBuf, unsigned int unInjSize)
{
	int fd = 0;
	ssize_t nWriteSize = 0;

	if(pBuf == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pBuf is NULL\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	
	fd = open(AUDIOPCM_DUMP_FILE_PATH, O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to open dump file, fd = %d\033[0m\n", __func__, __LINE__, fd);
	}
	else
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Succeed to open dump file, fd = %d\033[0m\n", __func__, __LINE__, fd);
	}
	
	nWriteSize = write(fd, pBuf, unInjSize);
	if(nWriteSize < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to write dump file. nWriteSize = %d\033[0m\n", __func__, __LINE__, nWriteSize);
	}
	else
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Write complete. unInjSize = %d, nWriteSize = %d\033[0m\n", __func__, __LINE__, unInjSize, nWriteSize);
	}
	
	close(fd);

FUNCTION_EXIT:
	return;
}
#endif

void MI_AudioPCM_SetMuteStatus(bool bMute)
{
	MI_AUDIOPCM_STAT_LOCK;

	s_stAudioPCMStatus.bMute = bMute;
	
	MI_AUDIOPCM_STAT_UNLOCK;

	return;
}

int MI_AudioPCM_GetMuteStatus(bool *pbMute)
{
	int nRet = MI_ERR_OK;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_STAT_LOCK;

	if(pbMute == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pbMute is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	*pbMute = s_stAudioPCMStatus.bMute;

FUNCTION_EXIT:
	MI_AUDIOPCM_STAT_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_Init(void)
{
	int nRet = MI_ERR_OK;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(s_bInitialized)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPCM is already initialized!\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_ALREADY_INIT;
		goto FUNCTION_EXIT;
	}
	else
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Initializing AudioPCM..\033[0m\n", __func__, __LINE__);
		s_bInitialized = TRUE;
	}

	/* Initialize AudioPCM Context */
	pthread_mutex_init(&s_stAudioPCMContext.tMutex, NULL);

	MI_AUDIOPCM_CTX_LOCK;

	/* Initialize AudioPCM Status */
	pthread_mutex_init(&s_stAudioPCMStatus.tMutex, NULL);
	MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Initialized);
	MI_AudioPCM_SetMuteStatus((bool)FALSE);
	
	if(s_stAudioPCMContext.ptStorage != NULL)
	{
		nRet = MI_AudioPlayback_RemoveOutputPath(s_stAudioPCMContext.ptStorage);
		if(!nRet)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove output path\033[0m\n", __func__, __LINE__);
		}
	}

	if((s_stAudioPCMContext.hAudioMixer != NULL) && (s_stAudioPCMContext.hAudioPlayback != NULL))
	{
		PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_RemoveInputFromMixer()\033[0m\n", __func__, __LINE__);
		nRet = MI_AudioPlayback_RemoveInputFromMixer(s_stAudioPCMContext.hAudioPlayback, s_stAudioPCMContext.hAudioMixer);
		if(nRet < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove AudioPlayback input(%p) from AudioMixer(%p), nRet = %d\033[0m\n", 
				__func__, __LINE__, s_stAudioPCMContext.hAudioPlayback, s_stAudioPCMContext.hAudioMixer, nRet);
		}
		else
		{
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Close()\033[0m\n", __func__, __LINE__);
			MI_AudioPlayback_Close(s_stAudioPCMContext.hAudioPlayback);
		}
	}
	
	s_stAudioPCMContext.hAudioPlayback = NULL;
	s_stAudioPCMContext.hAudioMixer = NULL;
	s_stAudioPCMContext.eJobStatus = eMI_AudioPCM_JobStatus_Canceled;
	s_stAudioPCMContext.eOutputStatus = eMI_AudioPCM_OutputStatus_NeedInit;
	if(s_stAudioPCMContext.ptStorage != NULL)
	{
		free(s_stAudioPCMContext.ptStorage);
	}
	s_stAudioPCMContext.ptStorage = NULL;
	
	MI_AUDIOPCM_CTX_UNLOCK;

	/* Initialize Audio Resource */
	pthread_mutex_init(&s_stAudioPCMResource.tMutex, NULL);

	MI_AUDIOPCM_RES_LOCK;
	s_stAudioPCMResource.ulAudioMixerHandle = 0;
	MI_AUDIOPCM_RES_UNLOCK;

FUNCTION_EXIT:
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_Destroy(void)
{
	int nRet = MI_ERR_OK;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	MI_AUDIOPCM_CTX_LOCK;

	if(MI_AudioPCM_GetAudioPCMStatus() == eMI_AudioPCM_AudioPCMStatus_Destroyed)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPCM is already destroyed\033[0m\n", __func__, __LINE__);
		MI_AUDIOPCM_CTX_UNLOCK;
		goto FUNCTION_EXIT;
	}

	if((s_stAudioPCMContext.hAudioPlayback != NULL) && (s_stAudioPCMContext.hAudioMixer != NULL))
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] We need to close hAudioPlayback and remove input from AudioMixer next initialization!\033[0m\n", __func__, __LINE__);
	}
	else
	{
		s_stAudioPCMContext.hAudioPlayback = NULL;
		s_stAudioPCMContext.hAudioMixer = NULL;
	}
	
	s_stAudioPCMContext.eOutputStatus = eMI_AudioPCM_OutputStatus_NeedInit;
	if(s_stAudioPCMContext.ptStorage != NULL)
	{
		free(s_stAudioPCMContext.ptStorage);
	}
	s_stAudioPCMContext.ptStorage = NULL;

	if(MI_AudioPCM_GetAudioPCMStatus() < eMI_AudioPCM_AudioPCMStatus_Destroyed)
	{
		MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Destroyed);
	}
	MI_AudioPCM_SetMuteStatus((bool)FALSE);
	
	nRet = pthread_mutex_destroy(&s_stAudioPCMStatus.tMutex);
	if(nRet)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to destroy AudioPCMStatus mutex, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}

	MI_AUDIOPCM_CTX_UNLOCK;

	nRet = pthread_mutex_destroy(&s_stAudioPCMContext.tMutex);
	if(nRet)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to destroy AudioPCMContext mutex, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}

	MI_AUDIOPCM_RES_LOCK;

	s_stAudioPCMResource.ulAudioMixerHandle = 0;

	MI_AUDIOPCM_RES_UNLOCK;

	nRet = pthread_mutex_destroy(&s_stAudioPCMResource.tMutex);
	if(nRet)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to destroy AudioPCMResource mutex, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
	}

	s_bInitialized = FALSE;

FUNCTION_EXIT:	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_SetResource(unsigned int ulAudioMixerHandle)
{
	int nRet = MI_ERR_OK;
		
	MI_AudioPCM_Resource_t *pstAudioPCMResource = NULL;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_RES_LOCK;

	pstAudioPCMResource = P_MI_AudioPCM_GetResource();
	if(pstAudioPCMResource == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstAudioPCMResource is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	pstAudioPCMResource->ulAudioMixerHandle = ulAudioMixerHandle;
	
	PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] pstAudioPCMResource->ulAudioMixerHandle = 0x%X\033[0m\n", __func__, __LINE__, pstAudioPCMResource->ulAudioMixerHandle);

FUNCTION_EXIT:
	MI_AUDIOPCM_RES_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_Open(void)
{
	return P_MI_AudioPCM_Open(AUDIOPLAYBACK_DEFAULT_FIFO_SIZE);
}

int MI_AudioPCM_Close(void)
{	
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = NULL;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;

	if(MI_AudioPCM_GetAudioPCMStatus() >= eMI_AudioPCM_AudioPCMStatus_Closed)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPCM is already closed!\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;		
	}

	if(pstContext->hAudioMixer == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioMixer is NULL\033[0m\n", __func__, __LINE__);
//		goto FUNCTION_EXIT;
	}

	PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_RemoveInputFromMixer()\033[0m\n", __func__, __LINE__);
	nRet = MI_AudioPlayback_RemoveInputFromMixer(pstContext->hAudioPlayback, pstContext->hAudioMixer);
	if(nRet < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove input from AudioMixer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to destroy AudioPCM, so changes nRet = %d to 0\033[0m\n", __func__, __LINE__, nRet);
		nRet = MI_ERR_OK;
		goto FUNCTION_EXIT;
	}

	PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Close()\033[0m\n", __func__, __LINE__);
	MI_AudioPlayback_Close(pstContext->hAudioPlayback);

	pstContext->hAudioPlayback = NULL;
	pstContext->hAudioMixer = NULL;
	
FUNCTION_EXIT:
	if(!nRet)
	{
		if(MI_AudioPCM_GetAudioPCMStatus() < eMI_AudioPCM_AudioPCMStatus_Closed)
		{
			MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Closed);
		}
	}
	
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;	
}

int MI_AudioPCM_AddInputToMixer(void)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = NULL;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	MI_AUDIOPCM_CTX_LOCK;

	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	nRet = P_MI_AudioPCM_AudioMixer_Acquire(&pstContext->hAudioMixer);
	if(nRet < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to acquire AudioMixer handle.\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	if(pstContext->hAudioMixer == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioMixer is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
	/* Connect playback input to Mixer */
	PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_PrepareMixerWithHandle()\033[0m\n", __func__, __LINE__);
	nRet = MI_AudioPlayback_PrepareMixerWithHandle(pstContext->hAudioPlayback, pstContext->hAudioMixer);
	if(nRet < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to prepare AudioMixer\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_Start(void)
{
	int nRet = MI_ERR_OK, nLeftVolume = 0, nRightVolume = 0;
	MI_AudioPCM_Context_t *pstContext = NULL;
	bool bMute = FALSE;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;

	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	/* Set output */
	if(pstContext->eOutputStatus == eMI_AudioPCM_OutputStatus_NeedInit)
	{
		if(pstContext->ptStorage != NULL)
		{
			free(pstContext->ptStorage);
			pstContext->ptStorage = NULL;
		}

		PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_PrepareOutputPath()\033[0m\n", __func__, __LINE__);
		pstContext->ptStorage = MI_AudioPlayback_PrepareOutputPath(pstContext->hAudioPlayback, pstContext->hAudioMixer);
		if(pstContext->ptStorage == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->ptStorage is NULL\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}

#if 1	/* Workaround code by thoh */
		/* 
			AudioPlayback volume is too low to hear when user pushes buttons in YouTube. 
			So volume value changes from default(0x800000) to 0x8000000
		*/
		nRet = MI_AudioPlayback_GetVolume(pstContext->hAudioPlayback, &nLeftVolume, &nRightVolume);
		if(nRet < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
			nRet = MI_ERR_OK;
		}
		else
		{
			if(nLeftVolume == AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE)
			{
				nLeftVolume = AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE;
			}
			if(nRightVolume == AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE)
			{
				nRightVolume = AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE;
			}

			nRet = MI_AudioPlayback_SetVolume(pstContext->hAudioPlayback, nLeftVolume, nRightVolume);
			if(nRet < 0)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
				nRet = MI_ERR_OK;
			}
		}
#endif	

		/* We need to call this code because we set [muted] value to 0 forcibly at MI_AudioPlayback_PrepareOutputPath() */
		nRet = MI_AudioPCM_GetMuteStatus(&bMute);
		if(nRet != MI_ERR_OK)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get mute value, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to start AudioPCM, so do not return error.\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_OK;
		}
		else
		{
			nRet = MI_AudioPlayback_SetMute(pstContext->hAudioPlayback, pstContext->ptStorage, bMute);
			if(nRet != MI_ERR_OK)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set mute value, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to start AudioPCM, so do not return error.\033[0m\n", __func__, __LINE__);
				nRet = MI_ERR_OK;
			}
		}
	}
	else
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Do not need to initialize output!\033[0m\n", __func__, __LINE__);
	}

	if(pstContext->eJobStatus < eMI_AudioPCM_JobStatus_Ready)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Ready;
	}

FUNCTION_EXIT:
	if(!nRet)
	{
		if(MI_AudioPCM_GetAudioPCMStatus() < eMI_AudioPCM_AudioPCMStatus_Started)
		{
			MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Started);
		}
	}
	
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_Stop(void)
{
	int nRet = MI_ERR_OK, nLeftVolume = 0, nRightVolume = 0;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	MI_AUDIOPCM_CTX_LOCK;

	nRet = P_MI_AudioPCM_Stop(P_MI_AudioPCM_GetContext());
	if(nRet == MI_ERR_OK)
	{
#if 1		/* Workaround code by thoh */
			/* 
				Restore AudioPlayback volume from 0x8000000 to default(0x800000)
			*/
		nRet = MI_AudioPlayback_GetVolume(P_MI_AudioPCM_GetContext()->hAudioPlayback, &nLeftVolume, &nRightVolume);
			if(nRet < 0)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
				nRet = MI_ERR_OK;
			}
			else
			{
				if(nLeftVolume == AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE)
				{
					nLeftVolume = AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE;
				}
				if(nRightVolume == AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE)
				{
					nRightVolume = AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE;
				}

			nRet = MI_AudioPlayback_SetVolume(P_MI_AudioPCM_GetContext()->hAudioPlayback, nLeftVolume, nRightVolume);
				if(nRet < 0)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
					nRet = MI_ERR_OK;
				}
			}
#endif	
		}
	
	MI_AUDIOPCM_CTX_UNLOCK;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_WriteData(void *pBuf, unsigned int unInjSize)
{
	int nRet = MI_ERR_OK, nNotAvailable = 0, nStart = 0;
	unsigned char *pReadPoint = NULL;
	unsigned int unBufferSize = 0;
	size_t bytesToPlay = 0, bytesPlayed = 0;
	void *pGetBuf = NULL;
	MI_AudioPCM_Context_t *pstContext = NULL;
	MI_AudioPCM_JobStatus_e ePreStatus = eMI_AudioPCM_JobStatus_Canceled;
	static MI_AudioPCM_WavRiffHeader_t tHeader;

#if AUDIOPCM_DUMP_PCM_FILE
	P_MI_AudioPCM_DumpPCMFile(pBuf, unInjSize);
#endif

	MI_AUDIOPCM_CTX_LOCK;
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pBuf == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pBuf is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(unInjSize == 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] unInjSize is %d\033[0m\n", __func__, __LINE__, unInjSize);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	pReadPoint = pBuf;
	bytesToPlay = unInjSize;

	ePreStatus = pstContext->eJobStatus;
	if(pstContext->eJobStatus < eMI_AudioPCM_JobStatus_Ready)
	{
		if(pstContext->eJobStatus == eMI_AudioPCM_JobStatus_Stopping)
		{
			pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Canceled;
		}
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	else
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Running;
	}
	
	if(bytesToPlay >= WAVE_HEADER_SIZE)
	{
		/* Check R, I, F, F */
		FILL_DATA_4BYTES(tHeader.riff, pReadPoint);
		if(tHeader.riff == WAVE_VALUE_RIFF)
		{
			PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] It has RIFF Header!!\033[0m\n", __func__, __LINE__);
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_4BYTES(tHeader.riffCSize, pReadPoint);
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_4BYTES(tHeader.wave, pReadPoint);
			if(tHeader.wave != WAVE_VALUE_WAVE)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] This buffer doesn't have W A V E characters\033[0m\n", __func__, __LINE__);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_4BYTES(tHeader.fmt, pReadPoint);
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_4BYTES(tHeader.headerLen, pReadPoint);
			if(tHeader.headerLen != 16)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] headerLen is wrong, headerLen = %lu\033[0m\n", __func__, __LINE__, tHeader.headerLen);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_2BYTES(tHeader.format, pReadPoint);
			if(tHeader.format != 0x01)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] format 0x%04X is not PCM\033[0m\n", __func__, __LINE__, tHeader.format);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			pReadPoint += READ_2BYTES;	bytesToPlay -= READ_2BYTES;
			
			FILL_DATA_2BYTES(tHeader.channels, pReadPoint);
			if(tHeader.channels > 2)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Invalid number of channels (%u) specified.\033[0m\n", __func__, __LINE__, tHeader.channels);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			pReadPoint += READ_2BYTES;	bytesToPlay -= READ_2BYTES;

			FILL_DATA_4BYTES(tHeader.samplesSec, pReadPoint);
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_4BYTES(tHeader.bytesSec, pReadPoint);
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_2BYTES(tHeader.chbits, pReadPoint);
			pReadPoint += READ_2BYTES;	bytesToPlay -= READ_2BYTES;

			FILL_DATA_2BYTES(tHeader.bps, pReadPoint);
			pReadPoint += READ_2BYTES;	bytesToPlay -= READ_2BYTES;

			FILL_DATA_4BYTES(tHeader.dataSig, pReadPoint);
			if(tHeader.dataSig != WAVE_VALUE_DATA)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] dataSig = 0x%08X, NO 'data' chunk found in this buffer\033[0m\n", __func__, __LINE__, (unsigned int)tHeader.dataSig);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			FILL_DATA_4BYTES(tHeader.dataLen, pReadPoint);
			pReadPoint += READ_4BYTES;	bytesToPlay -= READ_4BYTES;

			nStart = MI_AudioPlayback_IsPlaybackStarted(pstContext->hAudioPlayback);
			if(nStart < 0)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to check if AudioPlayback starts, nStart = %d\033[0m\n", __func__, __LINE__, nStart);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			else if(!nStart)
			{
				PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Start()\033[0m\n", __func__, __LINE__);
				nRet = MI_AudioPlayback_Start(	pstContext->hAudioPlayback, 
												tHeader.samplesSec, 
												tHeader.bps,
												(tHeader.bps != 8),
												(tHeader.channels > 1));
				if(!nRet)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to start AudioPlayback, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
					PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before P_MI_AudioPCM_RestoreInputToMixer()\033[0m\n", __func__, __LINE__);
					nRet = P_MI_AudioPCM_RestoreInputToMixer(pstContext->hAudioPlayback, pstContext->hAudioMixer);
					if(nRet < 0)
					{
						PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to restore input to mixer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
						goto FUNCTION_EXIT;
					}
					else	/* Restoring input is successful, restart AudioPlayback */
					{
						PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Start() - Restart\033[0m\n", __func__, __LINE__);
						nRet = MI_AudioPlayback_Start(	pstContext->hAudioPlayback, 
												tHeader.samplesSec, 
												tHeader.bps,
												(tHeader.bps != 8),
												(tHeader.channels > 1));
						if(!nRet)
						{
							PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to restart AudioPlayback!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
							goto FUNCTION_EXIT;
						}	
					}
				}
				
				/* Workaround : If I push first PCM data to AudioPlayback, HDMI Output doesn't output data.
								But I push DUMMY data to AudioPlayback before pushing first PCM data, 
								then HDMI Output print out data correctly. */
#if 1						
				nRet = MI_AudioPlayback_GetBuffer(pstContext->hAudioPlayback, &pGetBuf, (size_t *)&unBufferSize);
				if(!nRet)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioPlayback buffer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				}
		
				memset(pGetBuf, 0x00, unBufferSize);
				
				nRet = MI_AudioPlayback_ReadComplete(pstContext->hAudioPlayback, (size_t)unBufferSize);
				if(!nRet)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to send WriteComplete\033[0m\n", __func__, __LINE__);
				}
#endif		
			}
			else
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is already started!\033[0m\n", __func__, __LINE__);	
			}
		}
		else
		{			
			PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] This file doesn't have RIFF header!!\033[0m\n", __func__, __LINE__);
		}
	}
	else
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Inject size is so small = %d\033[0m\n", __func__, __LINE__, bytesToPlay);
	}

	nStart = MI_AudioPlayback_IsPlaybackStarted(pstContext->hAudioPlayback);
	if(nStart < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to check if AudioPlayback starts, nStart = %d\033[0m\n", __func__, __LINE__, nStart);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	else if(!nStart)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Did not start AudioPlayback yet.\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	else
	{
		if(bytesToPlay > 0)
		{
			nNotAvailable = 0;
			while(bytesToPlay > bytesPlayed)
			{
				nRet = MI_AudioPlayback_GetBuffer(pstContext->hAudioPlayback, &pGetBuf, (size_t *)&unBufferSize);
				if(!nRet)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioPlayback buffer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
					nRet = MI_ERR_FAIL;
					goto FUNCTION_EXIT;
				}
				
				if(unBufferSize)
				{
					if(unBufferSize + bytesPlayed > bytesToPlay)
					{	
						unBufferSize = bytesToPlay - bytesPlayed;
					}

					memcpy(pGetBuf, pReadPoint, unBufferSize);
					
					bytesPlayed += unBufferSize;
					pReadPoint += unBufferSize;
				}
				else
				{
					if(nNotAvailable > MI_AUDIOPCM_JOB_DEFAULT_WAIT_COUNT)
					{
						PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Cannot use AudioPlayback! unBufferSize is zero\033[0m\n", __func__, __LINE__);
						nRet = MI_ERR_FAIL;
						goto FUNCTION_EXIT;
					}
					nNotAvailable++;
					usleep(MI_AUDIOPCM_JOB_SHORT_DELAY_USEC);
					continue;
				}

				nRet = MI_AudioPlayback_ReadComplete(pstContext->hAudioPlayback, (size_t)unBufferSize);
				if(!nRet)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to send WriteComplete\033[0m\n", __func__, __LINE__);
					nRet = MI_ERR_FAIL;
					goto FUNCTION_EXIT;
				}
			}
		}
		else
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] No data found in this buffer\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}
	}

	nRet = bytesPlayed;

	if(pstContext->eJobStatus > eMI_AudioPCM_JobStatus_Ready)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Ready;
	}

FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;	
	return nRet;
}

int MI_AudioPCM_DetachOutput(void)
{
	int nRet = MI_ERR_OK, nLeftVolume = 0, nRightVolume = 0;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;
	
	nRet = P_MI_AudioPCM_DetachOutput(P_MI_AudioPCM_GetContext());
	if(nRet == MI_ERR_OK)
	{
#if 1		/* Workaround code by thoh */
			/* 
				Restore AudioPlayback volume from 0x8000000 to default(0x800000)
			*/
		nRet = MI_AudioPlayback_GetVolume(P_MI_AudioPCM_GetContext()->hAudioPlayback, &nLeftVolume, &nRightVolume);
			if(nRet < 0)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
				nRet = MI_ERR_OK;
			}
			else
			{
				if(nLeftVolume == AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE)
				{
					nLeftVolume = AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE;
				}
				if(nRightVolume == AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE)
				{
					nRightVolume = AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE;
				}

			nRet = MI_AudioPlayback_SetVolume(P_MI_AudioPCM_GetContext()->hAudioPlayback, nLeftVolume, nRightVolume);
				if(nRet < 0)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
					nRet = MI_ERR_OK;
				}
			}
#endif	
		}
	
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_AttachOutput(void)
{
	int nRet = MI_ERR_OK, nLeftVolume = 0, nRightVolume = 0;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;
	
	nRet = P_MI_AudioPCM_AttachOutput(P_MI_AudioPCM_GetContext());
	if(nRet == MI_ERR_OK)
	{
#if 1	/* Workaround code by thoh */
		/* 
			AudioPlayback volume is too low to hear when user pushes buttons in YouTube. 
			So volume value changes from default(0x800000) to 0x8000000
		*/
		nRet = MI_AudioPlayback_GetVolume(P_MI_AudioPCM_GetContext()->hAudioPlayback, &nLeftVolume, &nRightVolume);
	if(nRet < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
		nRet = MI_ERR_OK;
	}
	else
	{
		if(nLeftVolume == AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE)
		{
			nLeftVolume = AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE;
		}
		if(nRightVolume == AUDIOPLAYBACK_DEFAULT_VOLUME_VALUE)
		{
			nRightVolume = AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE;
		}

			nRet = MI_AudioPlayback_SetVolume(P_MI_AudioPCM_GetContext()->hAudioPlayback, nLeftVolume, nRightVolume);
		if(nRet < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
			nRet = MI_ERR_OK;
		}
	}
#endif	
	}

	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_PauseWriteData(void)
{
	int nRet = MI_ERR_OK, nStart = 0;
	MI_AudioPCM_Context_t *pstContext = NULL;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->eJobStatus > eMI_AudioPCM_JobStatus_Stopping)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Stopping;
	}

	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
	}
	else
	{
		nStart = MI_AudioPlayback_IsPlaybackStarted(pstContext->hAudioPlayback);
		if(nStart < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to check if AudioPlayback starts\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}
		else if(!nStart)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is already stopped\033[0m\n", __func__, __LINE__);		
			goto FUNCTION_EXIT;
		}
		else
		{
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Stop()\033[0m\n", __func__, __LINE__);
			MI_AudioPlayback_Stop(pstContext->hAudioPlayback);
		}
	}
	
FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_ResumeWriteData(void)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = NULL;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->eJobStatus < eMI_AudioPCM_JobStatus_Ready)
	{
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Before Job Status = %d\033[0m\n", __func__, __LINE__, pstContext->eJobStatus);
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Ready;
		PRINT_DEBUG("\033[1;31m @@@@@ [%s:%d] Change Job Status = %d\033[0m\n", __func__, __LINE__, pstContext->eJobStatus);
	}
	
FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_AudioPCM_SetOutputMute(bool bOutputMute)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = NULL;
	bool bMuteStatus = false;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	MI_AUDIOPCM_CTX_LOCK;

	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	nRet = MI_AudioPCM_GetMuteStatus(&bMuteStatus);
	if(nRet < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get mute status, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	if(bMuteStatus != bOutputMute)
	{
		nRet = MI_AudioPlayback_SetMute(pstContext->hAudioPlayback, pstContext->ptStorage, bOutputMute);
		if(nRet != MI_ERR_OK)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set mute mode, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			goto FUNCTION_EXIT;
		}

		MI_AudioPCM_SetMuteStatus(bOutputMute);
	}
	else
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Do not set mute mode, bMuteStatus = %d, bOutputMute = %d\033[0m\n", __func__, __LINE__, bMuteStatus, bOutputMute);
	}
	
FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);	
	return nRet;
}

#define ____NETFLIX_AUDIO_INTERFACE___
int MI_NetflixAudio_Init(void)
{
	int nRet = MI_ERR_OK;

	nRet = MI_AudioPCM_Init();
	if(nRet == MI_ERR_OK)
	{
		MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Initialized);		
	}
	return nRet;
}

int MI_NetflixAudio_Open(void)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = P_MI_AudioPCM_GetContext();	

	nRet = P_MI_AudioPCM_Open(AUDIOPLAYBACK_NETFLIX_FIFO_SIZE);
	if(nRet == MI_ERR_OK)
	{
		if(pstContext->eJobStatus < eMI_AudioPCM_JobStatus_Ready)
		{
			pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Ready;
		}
		nRet = MI_AudioPlayback_SetVolume(pstContext->hAudioPlayback, AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE, AUDIOPLAYBACK_LISTENABLE_VOLUME_VALUE);
		if(nRet < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set AudioPlayback volume, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But it is not error case, so it needs to change nRet(%d) to 0\033[0m\n", __func__, __LINE__, nRet);
			nRet = MI_ERR_OK;
		}
	}
	else
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] MI_NetflixAudio_Open failed\033[0m\n", __func__, __LINE__);
	}

	return nRet;
}

int MI_NetflixAudio_Close(void)
{	
	int nRet = MI_ERR_OK;
	int nStart = 0;
	MI_AudioPCM_Context_t *pstContext = NULL;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;

	if(MI_AudioPCM_GetAudioPCMStatus() >= eMI_AudioPCM_AudioPCMStatus_Closed)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPCM is already closed!\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;		
	}
	
	if(pstContext->hAudioMixer == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioMixer is NULL\033[0m\n", __func__, __LINE__);
	}

	PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_RemoveInputFromMixer()\033[0m\n", __func__, __LINE__);
	nRet = MI_AudioPlayback_RemoveInputFromMixer(pstContext->hAudioPlayback, pstContext->hAudioMixer);
	if(nRet < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove input from AudioMixer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] However we must clean-up audio playback. Keep going! \033[0m\n", __func__, __LINE__, nRet);
	}

	PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Close()\033[0m\n", __func__, __LINE__);
	MI_AudioPlayback_Close(pstContext->hAudioPlayback);

	pstContext->hAudioPlayback = NULL;
	pstContext->hAudioMixer = NULL;
	
FUNCTION_EXIT:
	if(!nRet)
	{
		if(MI_AudioPCM_GetAudioPCMStatus() < eMI_AudioPCM_AudioPCMStatus_Closed)
		{
			MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Closed);
		}
	}
	
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;	
}

int MI_NetflixAudio_Start(void)
{
	int nRet = MI_ERR_OK, nStart = 0;
	MI_AudioPCM_Context_t *pstContext = NULL;
	bool bMute = FALSE;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;

	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	/* Set output */
	if(pstContext->eOutputStatus == eMI_AudioPCM_OutputStatus_NeedInit)
	{
		if(pstContext->ptStorage != NULL)
		{
			MI_AudioPlayback_RemoveOutputPath(pstContext->ptStorage);
			free(pstContext->ptStorage);
			pstContext->ptStorage = NULL;
		}

		PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_PrepareOutputPath()\033[0m\n", __func__, __LINE__);
		pstContext->ptStorage = MI_AudioPlayback_PrepareOutputPath(pstContext->hAudioPlayback, pstContext->hAudioMixer);
		if(pstContext->ptStorage == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->ptStorage is NULL\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}

		/* We need to call this code because we set [muted] value to 0 forcibly at MI_AudioPlayback_PrepareOutputPath() */
		nRet = MI_AudioPCM_GetMuteStatus(&bMute);
		if(nRet != MI_ERR_OK)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get mute value, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to start AudioPCM, so do not return error.\033[0m\n", __func__, __LINE__);
			nRet = MI_ERR_OK;
		}
		else
		{
			nRet = MI_AudioPlayback_SetMute(pstContext->hAudioPlayback, pstContext->ptStorage, bMute);
			if(nRet != MI_ERR_OK)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set mute value, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to start AudioPCM, so do not return error.\033[0m\n", __func__, __LINE__);
				nRet = MI_ERR_OK;
			}
		}
	}
	else
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Do not need to initialize output!\033[0m\n", __func__, __LINE__);
	}

	if(pstContext->eJobStatus <= eMI_AudioPCM_JobStatus_Ready)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Running;
	}

	nStart = MI_AudioPlayback_IsPlaybackStarted(pstContext->hAudioPlayback);
	if(nStart < 0)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to check if AudioPlayback starts, nStart = %d\033[0m\n", __func__, __LINE__, nStart);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	else if(!nStart)
	{
		PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Start()\033[0m\n", __func__, __LINE__);
		nRet = MI_AudioPlayback_Start(	pstContext->hAudioPlayback, 48000, 16, 1, 1);
		if(!nRet)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to start AudioPlayback, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before P_MI_AudioPCM_RestoreInputToMixer()\033[0m\n", __func__, __LINE__);
			nRet = P_MI_AudioPCM_RestoreInputToMixer(pstContext->hAudioPlayback, pstContext->hAudioMixer);
			if(nRet < 0)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to restore input to mixer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				goto FUNCTION_EXIT;
			}
			else	/* Restoring input is successful, restart AudioPlayback */
			{
				PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Start() - Restart\033[0m\n", __func__, __LINE__);
				nRet = MI_AudioPlayback_Start(	pstContext->hAudioPlayback, 48000, 16, 1, 1);
				if(!nRet)
				{
					PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to restart AudioPlayback!, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
					nRet = MI_ERR_FAIL;
					goto FUNCTION_EXIT;
				}	
				nRet = MI_ERR_OK;
			}
		}	
		else
		{
			nRet = MI_ERR_OK;
		}
	}
	else
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is already started!\033[0m\n", __func__, __LINE__);	
	}

	PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback Started (APB Handle : %X, Mixer Handle : %X) \033[0m\n", __func__, __LINE__, pstContext->hAudioPlayback, pstContext->hAudioMixer);

FUNCTION_EXIT:
	if(nRet == MI_ERR_OK)
		{
		if(MI_AudioPCM_GetAudioPCMStatus() < eMI_AudioPCM_AudioPCMStatus_Stopped)
		{
			MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Started);
		}
	}
	
	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
}

int MI_NetflixAudio_GetBuffer(void **pBuf, size_t *pSize)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = NULL;

	MI_AUDIOPCM_CTX_LOCK;
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is not opened\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	if(pBuf == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pBuf is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	if(pSize == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pSize is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	if(MI_AudioPCM_GetAudioPCMStatus() >= eMI_AudioPCM_AudioPCMStatus_Closed)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is not valid\033[0m\n", __func__, __LINE__);
	}
	if(pstContext->eJobStatus < eMI_AudioPCM_JobStatus_Ready)
	{
		//PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Job status is not ready (%d)\033[0m\n", __func__, __LINE__, pstContext->eJobStatus);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	nRet = MI_AudioPlayback_GetBuffer(pstContext->hAudioPlayback, pBuf, (size_t *)pSize);
	if(!nRet)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get AudioPlayback buffer, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		nRet = MI_ERR_NOT_ALLOCATED;
	}
	
FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	
	return nRet;
}

int MI_NetflixAudio_ReadComplete(size_t szWritten)
{
	int nRet = MI_ERR_OK;
	MI_AudioPCM_Context_t *pstContext = NULL;

	MI_AUDIOPCM_CTX_LOCK;
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	if(MI_AudioPCM_GetAudioPCMStatus() >= eMI_AudioPCM_AudioPCMStatus_Closed)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback is not valid\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;		
	}
	if(pstContext->eJobStatus < eMI_AudioPCM_JobStatus_Ready)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Job status is not ready\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	
	nRet = MI_AudioPlayback_ReadComplete(pstContext->hAudioPlayback, szWritten);
	if(!nRet)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to send WriteComplete\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;		
	}

	if(pstContext->eJobStatus >= eMI_AudioPCM_JobStatus_Ready)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Running;
	}

FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	
	return nRet;
}

int MI_NetflixAudio_GetRemaining(void)
{
	MI_AudioPCM_Context_t *pstContext = NULL;
	int szQueued = 0;
	
	MI_AUDIOPCM_CTX_LOCK;
	
	pstContext = P_MI_AudioPCM_GetContext();
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

	szQueued = MI_AudioPlayback_GetQueuedBytes(pstContext->hAudioPlayback);

FUNCTION_EXIT:
	MI_AUDIOPCM_CTX_UNLOCK;
	return szQueued;
}

int MI_NetflixAudio_Stop(void)
{
	MI_AudioPCM_Context_t *pstContext = P_MI_AudioPCM_GetContext();
	int nRet = MI_ERR_OK;
	int nStart = 0;

	nRet = MI_AudioPlayback_SetMute(pstContext->hAudioPlayback, pstContext->ptStorage, TRUE);
	if(nRet != MI_ERR_OK)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to set mute value, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] But we need to start AudioPCM, so do not return error.\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_OK;
	}
	MI_AudioPCM_SetMuteStatus(TRUE);
	
#if 1
	nRet = P_MI_AudioPCM_Stop(pstContext);
	if(nRet == MI_ERR_OK)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Ready;
		PRINT_DEBUG("Job status is changed to %d\n", eMI_AudioPCM_JobStatus_Ready);
	}
#else
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	if(pstContext == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		nRet = MI_ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pstContext->eJobStatus >= eMI_AudioPCM_JobStatus_Ready)
	{
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Canceled;
	}

	if(pstContext->hAudioPlayback == NULL)
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->hAudioPlayback is NULL\033[0m\n", __func__, __LINE__);
	}
	else
	{
		nStart = MI_AudioPlayback_IsPlaybackStarted(pstContext->hAudioPlayback);
		if(nStart < 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to get if AudioPlayback starts, nStart = %d\033[0m\n", __func__, __LINE__, nStart);
			nRet = MI_ERR_FAIL;
			goto FUNCTION_EXIT;
		}
		else if(nStart == 0)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] AudioPlayback already stopped!\033[0m\n", __func__, __LINE__);
		}
		else
		{
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_Stop()\033[0m\n", __func__, __LINE__);
			MI_AudioPlayback_Stop(pstContext->hAudioPlayback);
		}
	}
#endif 

#if 0
	if(pstContext->eOutputStatus != eMI_AudioPCM_OutputStatus_DoNotInit)
	{
		pstContext->eOutputStatus = eMI_AudioPCM_OutputStatus_NeedInit;

		if(pstContext->ptStorage == NULL)
		{
			PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] pstContext->ptStorage is NULL\033[0m\n", __func__, __LINE__);
		}
		else
		{
			PRINT_DEBUG("\033[1;32m @@@@@ [%s:%d] Before MI_AudioPlayback_RemoveOutputPath()\033[0m\n", __func__, __LINE__);
			nRet = MI_AudioPlayback_RemoveOutputPath(pstContext->ptStorage);
			if(!nRet)
			{
				PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] Failed to remove output path, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
				nRet = MI_ERR_FAIL;
				goto FUNCTION_EXIT;
			}
			else
			{
				nRet = MI_ERR_OK;
			}
			
			if(pstContext->ptStorage != NULL)
			{
				free(pstContext->ptStorage);
				pstContext->ptStorage = NULL;
			}
		}
	}
#endif	

FUNCTION_EXIT:
	if(!nRet)
	{
		MI_AudioPCM_SetAudioPCMStatus(eMI_AudioPCM_AudioPCMStatus_Stopped);
		pstContext->eJobStatus = eMI_AudioPCM_JobStatus_Ready;
		PRINT_DEBUG("Job status is changed to %d\n", eMI_AudioPCM_JobStatus_Ready);		
	}
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);

	return nRet;
}

int MI_NetflixAudio_DetachOutput(void)
{
	int nRet = MI_ERR_OK;
	
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;

	nRet = P_MI_AudioPCM_DetachOutput(P_MI_AudioPCM_GetContext());
	P_MI_AudioPCM_GetContext()->eOutputStatus = eMI_AudioPCM_OutputStatus_DoNotInit;	/* Anyway set to do not init */

	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return nRet;
	}

int MI_NetflixAudio_AttachOutput(void)
{
	int nRet = MI_ERR_OK;

	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	MI_AUDIOPCM_CTX_LOCK;

	if( (MI_AudioPCM_GetAudioPCMStatus() != eMI_AudioPCM_AudioPCMStatus_Started) && (MI_AudioPCM_GetAudioPCMStatus() != eMI_AudioPCM_AudioPCMStatus_Stopped) )
	{
		PRINT_ERROR("\033[1;31m @@@@@ [%s:%d] eAudioPCMStatus is %d, so skip to prepare output..\033[0m\n", __func__, __LINE__, MI_AudioPCM_GetAudioPCMStatus() );
		nRet = MI_ERR_FAIL;
	}
	else
	{
		nRet = P_MI_AudioPCM_AttachOutput(P_MI_AudioPCM_GetContext());
	}

	P_MI_AudioPCM_GetContext()->eOutputStatus = eMI_AudioPCM_OutputStatus_NeedInit;

	MI_AUDIOPCM_CTX_UNLOCK;
	PRINT_DEBUG("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);	
	return nRet;
}

typedef struct __audio_playback_if_proto
{
	int (*init)(void);
	int (*open)(void);
	int (*close)(void);
	int (*destroy)(void);
	int (*set_resource)(unsigned int ulAudioMixerHandle);
	int (*add_input_to_mixer)(void);
	int (*start)(void);
	int (*stop)(void);
	// Interfaces for data flow control by MI_AudioPlayback
	int (*write_data)(void *pBuf, unsigned int unInjSize);
	int (*pause_write_data)(void);
	int (*resume_write_data)(void);	
	///////////////////////////////////////////////////////
	// Interfaces for data flow control by User layer
	int (*get_buffer)(void **pBuf, size_t *pSize);
	int (*read_complete)(size_t szWritten);
	///////////////////////////////////////////////////////
	int (*get_remaining)(void);
	int (*detach_output)(void);
	int (*attach_output)(void);
	int (*set_output_mute)(bool bOutputMute);
	int (*get_mute_state)(bool *pbMute);
	void (*set_mute_state)(bool bMute);
} MI_AUDIOPLAYBACK_IF_PROTO;

MI_AUDIOPLAYBACK_IF_PROTO MI_AudioPlayback_WebAudioIF =
{
	.init 				= MI_AudioPCM_Init,
	.open 				= MI_AudioPCM_Open,
	.close 				= MI_AudioPCM_Close,
	.destroy		 	= MI_AudioPCM_Destroy,
	.set_resource 		= MI_AudioPCM_SetResource,
	.add_input_to_mixer = MI_AudioPCM_AddInputToMixer,
	.start 				= MI_AudioPCM_Start,
	.stop				= MI_AudioPCM_Stop,
	.write_data			= MI_AudioPCM_WriteData,
	.pause_write_data	= MI_AudioPCM_PauseWriteData,
	.resume_write_data	= MI_AudioPCM_ResumeWriteData,	
	.get_buffer			= NULL,
	.read_complete		= NULL,
	.get_remaining		= NULL,
	.detach_output		= MI_AudioPCM_DetachOutput,
	.attach_output		= MI_AudioPCM_AttachOutput,
	.set_output_mute	= MI_AudioPCM_SetOutputMute,
	.get_mute_state		= MI_AudioPCM_GetMuteStatus,
	.set_mute_state		= MI_AudioPCM_SetMuteStatus,
};

MI_AUDIOPLAYBACK_IF_PROTO MI_AudioPlayback_NetflixAudioIF = 
{
	.init 				= MI_NetflixAudio_Init,
	.open 				= MI_NetflixAudio_Open,
	.close 				= MI_NetflixAudio_Close,
	.destroy		 	= MI_AudioPCM_Destroy,
	.set_resource 		= MI_AudioPCM_SetResource,
	.add_input_to_mixer = MI_AudioPCM_AddInputToMixer,
	.start 				= MI_NetflixAudio_Start,
	.stop				= MI_NetflixAudio_Stop,
	.write_data			= NULL,
	.pause_write_data	= MI_AudioPCM_PauseWriteData,
	.resume_write_data	= MI_AudioPCM_ResumeWriteData,	
	.get_buffer			= MI_NetflixAudio_GetBuffer,
	.read_complete		= MI_NetflixAudio_ReadComplete,
	.get_remaining		= MI_NetflixAudio_GetRemaining,
	.detach_output		= MI_NetflixAudio_DetachOutput,
	.attach_output		= MI_NetflixAudio_AttachOutput,
	.set_output_mute	= MI_AudioPCM_SetOutputMute,
	.get_mute_state		= MI_AudioPCM_GetMuteStatus,
	.set_mute_state		= MI_AudioPCM_SetMuteStatus,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

