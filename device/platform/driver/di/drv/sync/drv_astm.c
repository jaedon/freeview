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
#include "drv_astm.h"

#include "nexus_sync_channel.h"
#include "nexus_astm.h"
#if defined(CONFIG_ASTM_ASYNC_TIMER)
#include "vkernel.h"
#endif

#define DEBUG_MODULE        TRACE_MODULE_DRV_SYNC

typedef struct DRV_SYNC {

	NEXUS_VideoDecoderHandle hVidDecoder;
	NEXUS_AudioDecoderHandle hAudMainDecoder;
	NEXUS_AudioDecoderHandle hAudSpdifDecoder;
	NEXUS_AstmHandle	hAstm;

	HBOOL	AstmVidFlags;
	HBOOL 	AstmAudFlags;
	HBOOL 	AstmAudSpdifFlags;
	HBOOL	AstmStartFlags;
	HBOOL	AstmVidStartFlag;
	HBOOL	AstmAudStartFlag;
	HBOOL	AstmAudSpdifStartFlag;
} DRV_SYNC_s;


static DRV_SYNC_s sSync;
#if defined(CONFIG_ASTM_ASYNC_TIMER)
#define ASYNC_MODE_CHANGE_COUNT 4
static unsigned long s_AstmTimerId = 0;
static HBOOL bAsyncMode = FALSE;
#endif

#if defined(CONFIG_ASTM_ASYNC_TIMER)
static void P_SYNC_AstmAsyncTimer(unsigned long ulTimerId, void *pvParam)
{
	DRV_Error dResult= DRV_OK;
	unsigned long ulClockCnt = 0;
	unsigned long ulPresentationCnt = 0;

	BSTD_UNUSED(ulTimerId);
	BSTD_UNUSED(pvParam);

	PrintDebug("[%s] \n", __FUNCTION__);

	NEXUS_Astm_GetStausChangeCnt(&ulClockCnt, &ulPresentationCnt);
	if(bAsyncMode == FALSE &&
		(ulClockCnt > ASYNC_MODE_CHANGE_COUNT || ulPresentationCnt > ASYNC_MODE_CHANGE_COUNT))
	{
		PrintError("[%s] A/V are changed async mode (ulClockCnt=%d)(ulPresentationCnt=%d)!!!\n", __FUNCTION__, ulClockCnt, ulPresentationCnt);
		NEXUS_Astm_ClearStausChangeCnt();

		dResult = DRV_VIDEO_Restart(0,FALSE);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_ChangeSyncMode faild!\n");
		}

		dResult = DRV_AUDIO_ChangeSyncMode(DI_AUDIO_DECODER_MAIN, FALSE);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_ChangeSyncMode faild!\n");
		}

		dResult = DRV_AUDIO_ChangeSyncMode(DI_AUDIO_DECODER_SPDIF, FALSE);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_AUDIO_ChangeSyncMode faild!\n");
		}

		bAsyncMode = TRUE;
	}

	if(bAsyncMode == TRUE)
	{
		NEXUS_Astm_ClearStausChangeCnt();
	}
}
#endif

DRV_Error DRV_SYNC_SetAstmSettings(DRV_SYNC_AV eSyncHandle)
{
	NEXUS_AstmSettings astmSettings;

	NEXUS_Astm_GetSettings(sSync.hAstm, &astmSettings);
	NEXUS_PidChannelHandle hPidHandle;
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	DRV_Error nDrvErr = DRV_OK;
	HUINT32 uiPrimerIndex=0;
#endif
	switch(eSyncHandle)
	{
		case DRV_SYNC_VID:
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
			if(DRV_VIDEO_IsPrimerStarted())
			{
				nDrvErr = DRV_VIDEO_GetPrimerIndex(VID_DEC_INSTANCE, &uiPrimerIndex);
				if(nDrvErr == DRV_OK)
				{
					nDrvErr = DRV_DEMUX_GetVideoPrimerPidChannel(uiPrimerIndex, &hPidHandle);
					if(nDrvErr != DRV_OK)
					{
						hPidHandle = NULL;
						PrintError("DRV_DEMUX_GetVideoPrimerPidChannel failed!\n");
					}
				}
				else
				{
					hPidHandle = NULL;
					PrintError("DRV_VIDEO_GetPrimerIndex failed!\n");
				}

			}
			else
			{
				hPidHandle = DRV_DEMUX_GetVideoPidChannel(VID_DEC_INSTANCE);
			}
#else
			hPidHandle = DRV_DEMUX_GetVideoPidChannel(VID_DEC_INSTANCE);
#endif
			if(hPidHandle != NULL)
			{
				astmSettings.videoDecoder=sSync.hVidDecoder;
				sSync.AstmVidStartFlag=TRUE;
				if((sSync.AstmAudFlags == FALSE) && (sSync.AstmAudSpdifFlags == FALSE))
				{
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
					if(DRV_VIDEO_IsPrimerStarted())
					{
						astmSettings.stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(uiPrimerIndex);
					}
					else
					{
						astmSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
					}
#else
					astmSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
#endif
					astmSettings.stcMaster=sSync.hVidDecoder;
					PrintDebug("stcMaster is Video\n");
					sSync.AstmVidFlags=TRUE;
				}
				else
				{
					sSync.AstmVidFlags=FALSE;
				}
			}
			else
			{
				astmSettings.videoDecoder=NULL;
				sSync.AstmVidFlags=FALSE;
			}
			break;
		case DRV_SYNC_AUDMAIN:
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
			if(DRV_AUDIO_IsStartedPrimer(DI_AUDIO_DECODER_MAIN))
			{
				nDrvErr = DRV_AUDIO_GetPrimerIndex(DI_AUDIO_DECODER_MAIN, &uiPrimerIndex);
				if(nDrvErr == DRV_OK)
				{
					nDrvErr = DRV_DEMUX_GetAudioPrimerPidChannel(uiPrimerIndex, &hPidHandle);
					if(nDrvErr != DRV_OK)
					{
						hPidHandle = NULL;
						PrintError("DRV_DEMUX_GetVideoPrimerPidChannel failed!\n");
					}
				}
				else
				{
					hPidHandle = NULL;
					PrintError("DRV_AUDIO_GetPrimerIndex failed!\n");
				}
			}
			else
			{
				hPidHandle = DRV_DEMUX_GetAudioPidChannel(DI_AUDIO_DECODER_MAIN);
			}
#elif defined(CONFIG_FAST_CHANNEL_CHANGE)
			hPidHandle = DRV_DEMUX_GetAudioPidChannel(DI_AUDIO_DECODER_MAIN);
#else
			hPidHandle = DRV_DEMUX_GetAudioPidChannel(DI_AUDIO_DECODER_MAIN);
#endif
			if(hPidHandle != NULL)
			{
				astmSettings.audioDecoder[AUD_ASTM_MAIN_INSTANCE]=sSync.hAudMainDecoder;
				sSync.AstmAudStartFlag=TRUE;
				if((sSync.AstmVidFlags == FALSE) && (sSync.AstmAudSpdifFlags == FALSE))
				{
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
					if(DRV_AUDIO_IsStartedPrimer(DI_AUDIO_DECODER_MAIN))
					{
						astmSettings.stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(uiPrimerIndex);
					}
					else
					{
						astmSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
					}
#else
					/* Case of radio channel */
					astmSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
#endif
					astmSettings.stcMaster=sSync.hAudMainDecoder;
					PrintDebug("stcMaster is Audio Main\n");
					sSync.AstmAudFlags=TRUE;
				}
				else
				{
					sSync.AstmAudFlags=FALSE;
				}
			}
			else
			{
				astmSettings.audioDecoder[AUD_ASTM_MAIN_INSTANCE]=NULL;
				sSync.AstmAudFlags=FALSE;
			}
			break;
		case DRV_SYNC_AUDSPDIF:
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
			if(DRV_AUDIO_IsStartedPrimer(DI_AUDIO_DECODER_SPDIF))
			{
				nDrvErr = DRV_AUDIO_GetPrimerIndex(DI_AUDIO_DECODER_SPDIF, &uiPrimerIndex);
				if(nDrvErr == DRV_OK)
				{
					nDrvErr = DRV_DEMUX_GetAudioPrimerPidChannel(uiPrimerIndex, &hPidHandle);
					if(nDrvErr != DRV_OK)
					{
						hPidHandle = NULL;
						PrintError("DRV_DEMUX_GetVideoPrimerPidChannel failed!\n");
					}
				}
				else
				{
					hPidHandle = NULL;
					PrintError("DRV_AUDIO_GetPrimerIndex failed!\n");
				}
			}
			else
			{
				hPidHandle = DRV_DEMUX_GetAudioPidChannel(DI_AUDIO_DECODER_SPDIF);
			}
#elif defined(CONFIG_FAST_CHANNEL_CHANGE)
			hPidHandle = DRV_DEMUX_GetAudioPidChannel(DI_AUDIO_DECODER_SPDIF);
#else
			hPidHandle = DRV_DEMUX_GetAudioPidChannel(DI_AUDIO_DECODER_SPDIF);
#endif
			if(hPidHandle != NULL)
			{
				astmSettings.audioDecoder[AUD_ASTM_SPDIF_INSTANCE]=sSync.hAudSpdifDecoder;
				sSync.AstmAudSpdifStartFlag=TRUE;
				if((sSync.AstmVidFlags == FALSE) && (sSync.AstmAudFlags == FALSE))
				{
#if defined(CONFIG_FAST_CHANNEL_CHANGE_AUDIO_PRIMER)
					if(DRV_AUDIO_IsStartedPrimer(DI_AUDIO_DECODER_SPDIF))
					{
						astmSettings.stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(uiPrimerIndex);
					}
					else
					{
						astmSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
					}
#else
					astmSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();
#endif
					astmSettings.stcMaster=sSync.hAudSpdifDecoder;
					PrintDebug("stcMaster is Audio Spdif\n");
					sSync.AstmAudSpdifFlags=TRUE;
				}
				else
				{
					sSync.AstmAudSpdifFlags=FALSE;
				}
			}
			else
			{
				astmSettings.audioDecoder[AUD_ASTM_SPDIF_INSTANCE]=NULL;
				sSync.AstmAudSpdifFlags=FALSE;
			}
			break;
		default:
			break;
	}

	NEXUS_Astm_SetSettings(sSync.hAstm, &astmSettings);

#if defined(CONFIG_ASTM_ASYNC_TIMER)
	NEXUS_Astm_ClearStausChangeCnt();
	bAsyncMode = FALSE;
#endif

	return DRV_OK;
}

DRV_Error DRV_SYNC_UnsetAstmSettings(DRV_SYNC_AV eSyncHandle)
{
	NEXUS_AstmSettings astmSettings;

	NEXUS_Astm_GetSettings(sSync.hAstm, &astmSettings);

	switch(eSyncHandle)
	{
		case DRV_SYNC_VID:
			astmSettings.videoDecoder=NULL;
			sSync.AstmVidFlags=FALSE;
			sSync.AstmVidStartFlag=FALSE;
			break;
		case DRV_SYNC_AUDMAIN:
			astmSettings.audioDecoder[AUD_ASTM_MAIN_INSTANCE]=NULL;
			sSync.AstmAudFlags=FALSE;
			sSync.AstmAudStartFlag=FALSE;
			break;
		case DRV_SYNC_AUDSPDIF:
			astmSettings.audioDecoder[AUD_ASTM_SPDIF_INSTANCE]=NULL;
			sSync.AstmAudSpdifFlags=FALSE;
			sSync.AstmAudSpdifStartFlag=FALSE;
			break;
		default:
			break;
	}

	if((sSync.AstmVidStartFlag == TRUE) || (sSync.AstmAudStartFlag == TRUE) || (sSync.AstmAudSpdifStartFlag == TRUE))
	{
		if(sSync.AstmVidStartFlag == TRUE)
		{
			astmSettings.stcMaster=sSync.hVidDecoder;
		}
		else if(sSync.AstmAudStartFlag == TRUE)
		{
			astmSettings.stcMaster=sSync.hAudMainDecoder;
		}
		else
		{
			astmSettings.stcMaster=sSync.hAudSpdifDecoder;
		}
	}
	else
	{
		astmSettings.stcMaster=NULL;
#if (NEXUS_VERSION >= 1290)
		astmSettings.stcChannel=NULL;
		PrintDebug("[%s] STC master and channel handle are null.\n", __FUNCTION__);
#endif
	}

	NEXUS_Astm_SetSettings(sSync.hAstm, &astmSettings);
	return DRV_OK;
}

DRV_Error DRV_SYNC_AstmStart(void)
{
	if((sSync.hAstm != NULL) && (sSync.AstmStartFlags == FALSE))
	{
		NEXUS_Astm_Start(sSync.hAstm);
		sSync.AstmStartFlags = TRUE;
	}

	return DRV_OK;

}

DRV_Error DRV_SYNC_AstmStop(void)
{
	if((sSync.hAstm != NULL) && (sSync.AstmStartFlags == TRUE))
	{
		NEXUS_Astm_Stop(sSync.hAstm);
		sSync.AstmStartFlags = FALSE;
	}

	return DRV_OK;

}

HBOOL DRV_SYNC_GetAstmStarted(void)
{
	return sSync.AstmStartFlags;
}

HBOOL DRV_SYNC_GetAstmVideoValid(void)
{
	return sSync.AstmVidStartFlag;
}

HBOOL DRV_SYNC_GetAstmAudioValid(void)
{
	return sSync.AstmAudStartFlag;
}

DRV_Error DRV_SYNC_ASTM_Init(DRV_SYNC_DECODER_s hdecoder)
{
#if defined(CONFIG_ASTM_ASYNC_TIMER)
	int ret = 0;
#endif
	DRV_Error nDrvErr=DRV_OK;
	NEXUS_AstmSettings astmSettings;

	NEXUS_Astm_GetDefaultSettings(&astmSettings);
#if 0
	DRV_VIDEO_GetVideoHandle(VID_DEC_INSTANCE, &sSync.hVidDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_MAIN, &sSync.hAudMainDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_SPDIF, &sSync.hAudSpdifDecoder);
#else
	sSync.hVidDecoder = hdecoder.hVidDecoder;
	sSync.hAudMainDecoder = hdecoder.hAudMainDecoder;
	sSync.hAudSpdifDecoder = hdecoder.hAudSpdifDecoder;
#endif
	astmSettings.stcChannel = DRV_DEMUX_GetStcChannelHandle();

	astmSettings.videoDecoder = sSync.hVidDecoder;
	astmSettings.audioDecoder[AUD_ASTM_MAIN_INSTANCE] = sSync.hAudMainDecoder;
	astmSettings.audioDecoder[AUD_ASTM_SPDIF_INSTANCE] = sSync.hAudSpdifDecoder;
	astmSettings.stcMaster = sSync.hAudMainDecoder; /* default Audio Master */

	astmSettings.clockCoupling = NEXUS_AstmClockCoupling_eAdaptive;
	astmSettings.stcSource = NEXUS_AstmStcSource_eAdaptive;
	astmSettings.presentationRateControl = NEXUS_AstmPresentationRateControl_eAdaptive;
	astmSettings.enableAutomaticLifecycleControl = false;

	astmSettings.videoPresenterConfig.maxAllowableFirstPtsStcDiff=0;
	astmSettings.audioPresenterConfig[0].maxAllowableFirstPtsStcDiff=0;
	astmSettings.audioPresenterConfig[1].maxAllowableFirstPtsStcDiff=0;

#if defined(CONFIG_WORKAROUND_ASTM_LIVE_TUNE)
	astmSettings.videoPresenterConfig.passEventCountThreshold = 0; /* Modify 0 */
	astmSettings.videoPresenterConfig.failEventCountThreshold = 5; /* Modify 5 */

	astmSettings.audioPresenterConfig[0].passEventCountThreshold = 0; /* Modify 0 */
	astmSettings.audioPresenterConfig[1].passEventCountThreshold = 0; /* Modify 0 */

#if defined(CONFIG_TEMP_CDIG1000C) // for Nordig certification
	astmSettings.audioPresenterConfig[0].failEventCountThreshold = 4; /* Modify 4 */
	astmSettings.audioPresenterConfig[1].failEventCountThreshold = 4; /* Modify 4 */
#else
	astmSettings.audioPresenterConfig[0].failEventCountThreshold = 6; /* Modify 6 */
	astmSettings.audioPresenterConfig[1].failEventCountThreshold = 6; /* Modify 6 */
#endif
#else
	astmSettings.videoPresenterConfig.passEventCountThreshold = 3; /* Modify 0 to 3 */
	astmSettings.videoPresenterConfig.failEventCountThreshold = 4; /* Modify 10 to 4 */

	astmSettings.audioPresenterConfig[0].passEventCountThreshold = 4; /* Modify 0 to 4 */
	astmSettings.audioPresenterConfig[1].passEventCountThreshold = 4; /* Modify 0 to 4 */

	astmSettings.audioPresenterConfig[0].failEventCountThreshold = 5; /* Modify 10 to 5 */
	astmSettings.audioPresenterConfig[1].failEventCountThreshold = 5; /* Modify 10 to 5 */
#endif

#if defined(CONFIG_TEMP_HDNANOECO)
	astmSettings.presentationConfig.initialAcquisitionTime = 2500; /* 1000ms */
	astmSettings.presentationConfig.processingFrequency = 200; /* 200ms */
	astmSettings.presentationConfig.settlingTime = 100; /* 100ms */
	astmSettings.presentationConfig.tsmDisabledWatchdogTimeout = 0; /* Not working */
	astmSettings.presentationConfig.preferredPresentationRateControl = NEXUS_PresentationRateControl_eTimeStamp;

	astmSettings.clockCouplingConfig.initialAcquisitionTime = 2500; /* 1000ms */
	astmSettings.clockCouplingConfig.processingFrequency = 1000; /* 1000ms */
	astmSettings.clockCouplingConfig.settlingTime = 100; /* 100ms */
#elif defined(CONFIG_TEMP_HMS1000SPH2) || defined(CONFIG_TEMP_ICORDPRO) || defined(CONFIG_TEMP_HGS1000S)
	/*	if use SCD antenna, It takes a long time to tune.
		So, If call tune and decoder start(include ASTM start) at same time, 
		ASTM should increase the "initialAcquisitionTime" 
		( iCordPro ) */
	astmSettings.presentationConfig.initialAcquisitionTime = 3000; /* 1000ms */
	astmSettings.presentationConfig.processingFrequency = 200; /* 200ms */
	astmSettings.presentationConfig.settlingTime = 100; /* 100ms */
	astmSettings.presentationConfig.tsmDisabledWatchdogTimeout = 0; /* Not working */
	astmSettings.presentationConfig.preferredPresentationRateControl = NEXUS_PresentationRateControl_eTimeStamp;

	astmSettings.clockCouplingConfig.initialAcquisitionTime = 3000; /* 1000ms */
	astmSettings.clockCouplingConfig.processingFrequency = 1000; /* 1000ms */
	astmSettings.clockCouplingConfig.settlingTime = 100; /* 100ms */
#else
	astmSettings.presentationConfig.initialAcquisitionTime = 1200; /* 1000ms */
	astmSettings.presentationConfig.processingFrequency = 200; /* 200ms */
	astmSettings.presentationConfig.settlingTime = 100; /* 100ms */
	astmSettings.presentationConfig.tsmDisabledWatchdogTimeout = 0; /* Not working */
	astmSettings.presentationConfig.preferredPresentationRateControl = NEXUS_PresentationRateControl_eTimeStamp;

	astmSettings.clockCouplingConfig.initialAcquisitionTime = 1000; /* 1000ms */
	astmSettings.clockCouplingConfig.processingFrequency = 1000; /* 1000ms */
	astmSettings.clockCouplingConfig.settlingTime = 100; /* 100ms */
#endif

#if defined(CONFIG_ASTM_DEVIANT_COUNT_THRESHOLD)
	astmSettings.clockCouplingConfig.deviantCountThreshold = CONFIG_ASTM_DEVIANT_COUNT_THRESHOLD_VALUE;
#endif

	astmSettings.enabled = TRUE;

	sSync.hAstm = NEXUS_Astm_Create(&astmSettings);

	sSync.AstmVidFlags=FALSE;
	sSync.AstmAudFlags=FALSE;
	sSync.AstmAudSpdifFlags=FALSE;
	sSync.AstmStartFlags=FALSE;

	/* Re-initialize echo decoder handle for astm module */
	NEXUS_Astm_GetSettings(sSync.hAstm, &astmSettings);
	astmSettings.videoDecoder=NULL;
	astmSettings.audioDecoder[AUD_ASTM_MAIN_INSTANCE]=NULL;
	astmSettings.audioDecoder[AUD_ASTM_SPDIF_INSTANCE]=NULL;
	astmSettings.stcMaster=NULL;
#if (NEXUS_VERSION >= 1290)
	astmSettings.stcChannel=NULL;
#endif
	NEXUS_Astm_SetSettings(sSync.hAstm, &astmSettings);

#if defined(CONFIG_ASTM_ASYNC_TIMER)
	ret = VK_TIMER_EventEvery(1000, P_SYNC_AstmAsyncTimer, (void *)NULL, 0, (unsigned long *)&s_AstmTimerId);
	if(ret != VK_OK)
	{
		PrintError("[%s] VK_TIMER_EventEvery was failed s_AstmTimerId %d \n",__func__, s_AstmTimerId);
	}
#endif

	return nDrvErr;
}

