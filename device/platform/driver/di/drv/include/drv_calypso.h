/**
 * drv_calypso.h
*/

#ifndef __DRV_CALYPSO_H__
#define __DRV_CALYPSO_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "di_calypso.h"
#include "di_demux.h"
#include "drv_err.h"
#include "di_media_types.h"

#include "nexus_base_types.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"
#include "nexus_playpump.h"


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_CSO_Init(void);
void DRV_CSO_PrintProbeInfo (DI_CSO_INFO_t *pCsoInfo);
DI_ERR_CODE DRV_CSO_Probe (void *pHTuner, HUINT8 *pUrl, DI_CSO_INFO_t *stCsoFileInfo);

DRV_Error DRV_CSO_OpenPlaypumpPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, HINT32 nPid, NEXUS_PidChannelHandle *phPidChannel);
DRV_Error DRV_CSO_ClosePlaypumpPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, NEXUS_PidChannelHandle hPidChannel);

DRV_Error DRV_CSO_SetMulticastVPidChannel(NEXUS_PidChannelHandle hPidChannel, 
                                       NEXUS_VideoDecoderHandle hVideoDec, 
                                       NEXUS_VideoCodec nVideoCodec);
DRV_Error DRV_CSO_SetMulticastAPidChannel(NEXUS_PidChannelHandle hPidChannel, 
                                       HBOOL isSubAudioPid,
                                       NEXUS_AudioDecoderHandle hAudioDec,
                                       NEXUS_AudioDecoderHandle hAudioPassThroughDec);
DRV_Error DRV_CSO_SetMulticastStcChannel(HUINT32 uTSInjID, NEXUS_StcChannelHandle hStcChannel);
DRV_Error DRV_CSO_UnsetMulticastStcChannel(HUINT32 uTSInjID);
DRV_Error DRV_CSO_GetSpeed (DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType ,DI_MEDIA_PlaySpeed_e *peSpeed);

/* 
 * Audio Pid Channel SettingÇÔ¼ö. 
 */
DRV_Error DRV_CSO_SetAudioPidChannel(NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_AudioDecoderHandle ulAudioDecHandle,
														NEXUS_AudioDecoderHandle ulAudioPassThroughDecHandle);
NEXUS_StcChannelHandle DRV_CSO_GetPlaypumpStcChannelHandle(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, DI_MEDIA_PLAYPUMP csoType);
NEXUS_PidChannelHandle DRV_CSO_GetPlaypumpAudioPidChannel(DI_MEDIA_AUDIO_FORMAT_e eAudioCodec, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, void *AudioDecoderHandle);
NEXUS_PidChannelHandle DRV_CSO_GetPlaypumpVideoPidChannel(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_VIDEO_FORMAT eVideoCodec, void *VideoDecoderHandle);

/* Check Media Stat Type */
HBOOL DRV_CSO_CheckStart(void);

#endif /* !__DRV_MEDIA20_H__ */
