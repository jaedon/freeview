/**
 * drv_audio.h
*/

#ifndef __DRV_AUDIO_H__
#define __DRV_AUDIO_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

#if defined(CONFIG_AUDIO_DECODER)
#include "nexus_audio_decoder.h"
#endif
#include "nexus_hdmi_output.h"
#if defined(CONFIG_BLUETOOTH)
#include "nexus_audio_capture.h"
#endif
#if defined(CONFIG_AUDIO_DECODER)
#include "nexus_audio_playback.h"
#endif
/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define AUDIO_ALL_DECODERS	(3) /* 3 is Main/Sub/PassThou decoder*/

#define MAX_PCM_SAMPLE_RATE	(96000) /* 96 KHz */
#define MAX_OUTPUT_RATE_HRA	(96000) /* 96 KHz */
#define MAX_OUTPUT_RATE_NON_HRA	(48000) /* 48 KHz */

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
#define TEMP_HRA_SELF_TEST /* Should be removed after build up MEDIA & MW & APP  */
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_AUDIO_Init(void);
DRV_Error DRV_AUDIO_Start(HUINT32 ulDevId ,DI_AUDIO_FORMAT nType);
DRV_Error DRV_AUDIO_Stop (HUINT32 ulDevId);
DRV_Error DRV_AUDIO_ChangeSyncMode(HUINT32 ulDevId, HBOOL bTsm);
DRV_Error DRV_AUDIO_PrintInfo(void);
DRV_Error DRV_AUDIO_BeepPlay(bool onoff);
DRV_Error DRV_AUDIO_HWMUTE(HBOOL mute);
DRV_Error DRV_AUDIO_GetAudioFormat(HUINT32 ulDevId, DI_AUDIO_FORMAT *pnType);
DRV_Error DRV_AUDIO_CallbackCheck(void);
#if defined(CONFIG_AUDIO_DECODER)
void DRV_AUDIO_GetDecoderHandle(HUINT32 ulDevId, NEXUS_AudioDecoderHandle *audioDecoder);
#if defined(CONFIG_BLUETOOTH)
void DRV_AUDIO_GetCaptureHandle(HUINT32 ulDevId, NEXUS_AudioCaptureHandle *audioCapture);
#endif
NEXUS_AudioCodec DRV_AUDIO_ConvertCodecToNexus(DI_AUDIO_FORMAT nType);
#endif
DRV_Error DRV_AUDIO_PlaybackDummyToHDMI(HBOOL isStart);
DRV_Error DRV_AUDIO_ConnectAudioDummyOutput(NEXUS_AudioInput input);
DRV_Error DRV_AUDIO_DisconnectAudioDummyOutput(NEXUS_AudioInput input);
DRV_Error DRV_AUDIO_GetAudioDummyOutputHandle(NEXUS_AudioInput input, NEXUS_AudioOutput *output);
DRV_Error DRV_AUDIO_AddDacOutputPath(HUINT32 ulDevId);
DRV_Error DRV_AUDIO_AddHdmiOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType);
DRV_Error DRV_AUDIO_AddSpdifOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType);
DRV_Error DRV_AUDIO_RemoveDacOutputPath(HUINT32 ulDevId);
DRV_Error DRV_AUDIO_RemoveHdmiOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType);
DRV_Error DRV_AUDIO_RemoveSpdifOutputPath(HUINT32 ulDevId, DI_AUDIO_FORMAT nType);
HUINT32 DRV_AUDIO_GetPrimerIndex(HUINT32 ulDevId, HUINT32 *pPrimerIndex);
HBOOL DRV_AUDIO_IsStartedPrimer(HUINT32 ulDevId);
#if defined(CONFIG_HDMI_IN)
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
DRV_Error DRV_AUDIO_InitAudioInputCapture(HUINT32 ulDevId);
#endif
#endif
#if defined(CONFIG_MIC)
DRV_Error DRV_AUDIO_ConnectMicToMixer(NEXUS_AudioPlaybackHandle micPlayback);
DRV_Error DRV_AUDIO_DisconnectMicToMixer(NEXUS_AudioPlaybackHandle micPlayback);
DRV_Error DRV_AUDIO_SetAllOutputPCM(void);
DRV_Error DRV_AUDIO_UnsetAllOutputPCM(void);
#endif

DRV_Error DRV_AUDIO_SetDecoderVolume(HUINT32 ulDevId, HUINT32 ulDecoderVolume);
#endif /* !__DRV_AUDIO_H__ */

