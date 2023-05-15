
#include <string.h>
#include <stdlib.h>
#include "linden_trace.h"

#include "drv_err.h"
#if defined(CONFIG_SYNC_CHANNEL) || defined(CONFIG_ASTM) || defined(CONFIG_SYNC_RECOVERY)
#include "di_audio.h"
#include "di_video.h"
#include "drv_audio.h"
#include "drv_video.h"
#include "drv_sync_common.h"
#if defined(CONFIG_SYNC_CHANNEL)
#include "drv_sync_channel.h"
#endif
#if defined(CONFIG_ASTM)
#include "drv_astm.h"
#elif defined(CONFIG_SYNC_RECOVERY)
#include "drv_sync_recovery.h"
#endif
#endif

#define DEBUG_MODULE        TRACE_MODULE_DRV_SYNC

DRV_Error DRV_SYNC_Init(void)
{
	DRV_Error nDrvErr=DRV_OK;
#if defined(CONFIG_SYNC_CHANNEL) || defined(CONFIG_ASTM) || defined(CONFIG_SYNC_RECOVERY)
	DRV_SYNC_DECODER_s hdecoder;

	DRV_VIDEO_GetVideoHandle(VID_DEC_INSTANCE, &hdecoder.hVidDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_MAIN, &hdecoder.hAudMainDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_SPDIF, &hdecoder.hAudSpdifDecoder);
	hdecoder.hAudSubDecoder = NULL;

#if defined(CONFIG_SYNC_CHANNEL)
	nDrvErr = DRV_SYNC_CHANNEL_Init(hdecoder);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d DRV_SYNC_CHANNEL_Init() error !!! \n", __FUNCTION__, __LINE__);
		nDrvErr = DRV_ERR;
		goto RET;
	}
#endif

#if defined(CONFIG_ASTM)
	nDrvErr = DRV_SYNC_ASTM_Init(hdecoder);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d DRV_SYNC_ASTM_Init() error !!! \n", __FUNCTION__, __LINE__);
		nDrvErr = DRV_ERR;
		goto RET;
	}
#elif defined(CONFIG_SYNC_RECOVERY)
	nDrvErr = DRV_SYNC_RECOVERY_Init(hdecoder);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d DRV_SYNC_ASTM_Init() error !!! \n", __FUNCTION__, __LINE__);
		nDrvErr = DRV_ERR;
		goto RET;
	}
#else
	PrintDebug("[%s] %d NOT used ASTM or SYNC_RECOVERY !!! \n", __FUNCTION__, __LINE__);	
#endif
#endif

RET:
	return nDrvErr;
}


