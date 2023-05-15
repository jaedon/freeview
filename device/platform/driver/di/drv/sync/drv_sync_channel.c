/*
2011/11/24 : Initial version
*/

#include <string.h>
#include <stdlib.h>
#include "linden_trace.h"

#include "drv_err.h"
#include "di_audio.h"
#include "di_video.h"
#include "drv_audio.h"
#include "drv_video.h"
#include "drv_demux.h"
#include "drv_sync_channel.h"

#include "nexus_sync_channel.h"

#define DEBUG_MODULE        TRACE_MODULE_DRV_SYNC

typedef struct DRV_SYNC {

	NEXUS_VideoDecoderHandle hVidDecoder;
	NEXUS_AudioDecoderHandle hAudMainDecoder;
	NEXUS_AudioDecoderHandle hAudSpdifDecoder;
	NEXUS_AudioDecoderHandle hAudSubDecoder;
	NEXUS_SyncChannelHandle nSyncChannel;
} DRV_SYNC_s;


static DRV_SYNC_s sSync;

DRV_Error DRV_SYNC_SetSyncChannel(DRV_SYNC_AV eSyncHandle, NEXUS_AudioDecoderConnectorType eAudConType)
{
	NEXUS_SyncChannelSettings nSyncChannelSettings;
	NEXUS_SyncChannel_GetSettings(sSync.nSyncChannel,&nSyncChannelSettings);
	PrintDebug("Set %d \n", eSyncHandle);

	switch(eSyncHandle)
	{
		case DRV_SYNC_VID:
			if(sSync.hVidDecoder == NULL)
			{
				PrintError("Video decoder is NULL\n");
				return 	DRV_ERR;
			}
			nSyncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(sSync.hVidDecoder);
			break;
		case DRV_SYNC_AUDMAIN:
			if(sSync.hAudMainDecoder == NULL)
			{
				PrintError("Audio MAIN decoder is NULL\n");
				return 	DRV_ERR;
			}
			nSyncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(sSync.hAudMainDecoder,eAudConType);
			break;			
		case DRV_SYNC_AUDSUB:
			if(sSync.hAudSubDecoder == NULL)
			{
				PrintError("Audio SUB decoder is NULL\n");
				return 	DRV_ERR;
			}
			nSyncChannelSettings.audioInput[1] = NEXUS_AudioDecoder_GetConnector(sSync.hAudSubDecoder,eAudConType);
			break;			
		case DRV_SYNC_AUDSPDIF:
			if(sSync.hAudSpdifDecoder == NULL)
			{
				PrintError("Audio SPDIF decoder is NULL\n");
				return 	DRV_ERR;
			}
			nSyncChannelSettings.audioInput[2] = NEXUS_AudioDecoder_GetConnector(sSync.hAudSpdifDecoder,eAudConType);
			break;		
		default:
			break;
	}	
	NEXUS_SyncChannel_SetSettings(sSync.nSyncChannel,&nSyncChannelSettings);
	return DRV_OK;
}

DRV_Error DRV_SYNC_UnsetSyncChannel(DRV_SYNC_AV eSyncHandle)
{
	NEXUS_SyncChannelSettings nSyncChannelSettings;
	NEXUS_SyncChannel_GetSettings(sSync.nSyncChannel,&nSyncChannelSettings);
	PrintDebug("UnSet %d \n", eSyncHandle);

	switch(eSyncHandle)
	{
		case DRV_SYNC_VID:
			nSyncChannelSettings.videoInput = NULL;
			break;
		case DRV_SYNC_AUDMAIN:
			nSyncChannelSettings.audioInput[0] = NULL;
			break;
		case DRV_SYNC_AUDSUB:
			nSyncChannelSettings.audioInput[1] = NULL;
			break;
		case DRV_SYNC_AUDSPDIF:
			nSyncChannelSettings.audioInput[2] = NULL;
			break;
		default:
			break;
	}
	NEXUS_SyncChannel_SetSettings(sSync.nSyncChannel,&nSyncChannelSettings);
	return DRV_OK;
}

DRV_Error DRV_SYNC_CHANNEL_Init(DRV_SYNC_DECODER_s hdecoder)
{
	DRV_Error nDrvErr=DRV_OK;

	NEXUS_SyncChannelSettings nSyncChannelSettings;
	NEXUS_SyncChannel_GetDefaultSettings(&nSyncChannelSettings);
	nSyncChannelSettings.enableMuteControl = false;

	sSync.nSyncChannel = NEXUS_SyncChannel_Create(&nSyncChannelSettings);

#if 0
	DRV_VIDEO_GetVideoHandle(VID_DEC_INSTANCE, &sSync.hVidDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_MAIN, &sSync.hAudMainDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_SPDIF, &sSync.hAudSpdifDecoder);
#else
	sSync.hVidDecoder = hdecoder.hVidDecoder;
	sSync.hAudMainDecoder = hdecoder.hAudMainDecoder;
	sSync.hAudSubDecoder = hdecoder.hAudSubDecoder;	
	sSync.hAudSpdifDecoder = hdecoder.hAudSpdifDecoder;	
#endif

	return nDrvErr;
}

DRV_Error DRV_SYNC_SetAudioDecoderHandle(DRV_SYNC_AV eSyncHandle, NEXUS_AudioDecoderHandle* hAudDecoder)
{
	switch(eSyncHandle)
	{
		case DRV_SYNC_AUDMAIN:
			sSync.hAudMainDecoder = *hAudDecoder;
			break;

		case DRV_SYNC_AUDSUB:
			sSync.hAudSubDecoder = *hAudDecoder;
			break;

		case DRV_SYNC_AUDSPDIF:
			sSync.hAudSpdifDecoder = *hAudDecoder;
			break;

		default:
			PrintError("Invalid Parameter\n");
			return DRV_ERR;
			break;
	}

	return DRV_OK;
}

