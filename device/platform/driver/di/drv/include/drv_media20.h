/**
 * drv_media20.h
*/

#ifndef __DRV_MEDIA20_H__
#define __DRV_MEDIA20_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "di_media20.h"
#include "di_demux.h"
#include "drv_err.h"
#include "di_media_types.h"

#include "nexus_base_types.h"
#include "nexus_pid_channel.h"
#if defined(CONFIG_VIDEO_DECODER)
#include "nexus_video_decoder.h"
#endif
#if defined(CONFIG_AUDIO_DECODER)
#include "nexus_audio_decoder.h"
#endif
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
void DRV_MEDIA_PrintProbeInfo (DI_MEDIA_INFO_t *pMediaInfo);
DI_ERR_CODE DRV_MEDIA_Probe (void *pHTuner, HUINT8 *pUrl, DI_MEDIA_INFO_t *stMediaFileInfo);

DRV_Error DRV_MEDIA_OpenPumpPidChannel(HUINT32 uDeviceID, DI_DEMUX_CHANNELTYPE nChannelType, HUINT16 usPid, NEXUS_PidChannelHandle *pulPidChannelHandle);
DRV_Error DRV_MEDIA_ClosePumpPidChannel(HUINT32 uDeviceID, NEXUS_PidChannelHandle ulPidChannelHandle, DI_DEMUX_CHANNELTYPE nChannelType);
#if defined(CONFIG_VIDEO_DECODER)
DRV_Error DRV_MEDIA_SetMulticastVPidChannel(NEXUS_PidChannelHandle hPidChannel, 
                                       NEXUS_VideoDecoderHandle hVideoDec, 
                                       NEXUS_VideoCodec nVideoCodec);
#endif
#if defined(CONFIG_AUDIO_DECODER)
DRV_Error DRV_MEDIA_SetMulticastAPidChannel(NEXUS_PidChannelHandle hPidChannel, 
                                       HBOOL isSubAudioPid,
                                       NEXUS_AudioDecoderHandle hAudioDec,
                                       NEXUS_AudioDecoderHandle hAudioPassThroughDec);
#endif
DRV_Error DRV_MEDIA_SetMulticastStcChannel(HUINT32 uTSInjID, NEXUS_StcChannelHandle hStcChannel);
DRV_Error DRV_MEDIA_UnsetMulticastStcChannel(HUINT32 uTSInjID);
DRV_Error DRV_MEDIA_GetPlaybackSpeed (DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType ,DI_MEDIA_PlaySpeed_e *peSpeed);

#if defined(CONFIG_AUDIO_DECODER)
/* 
 * Audio Pid Channel SettingÇÔ¼ö. 
 */
DRV_Error DRV_MEDIA_SetAudioPidChannel(NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_AudioDecoderHandle ulAudioDecHandle,
														NEXUS_AudioDecoderHandle ulAudioPassThroughDecHandle);

#endif			
#endif /* !__DRV_MEDIA20_H__ */
