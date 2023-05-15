/**
 * drv_demux.h
*/

#ifndef __DRV_DEMUX_H__
#define __DRV_DEMUX_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include "di_demux.h"

#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_stc_channel.h"
#include "nexus_pid_channel.h"

#include "di_config.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#if defined(CONFIG_BMEM_OPTIMIZE)
#if CONFIG_NUM_PARSER_BANDS <= 4	// 0=16, 1=32, 2=48, 3=64, 4=80, 5=96, 6=112, 7=128, 8=144
#if defined(CONFIG_CAS_NA) || defined(CONFIG_CAS_CX)
#define	DRV_DMX_NUM_PID_CHANNEL (16*(CONFIG_NUM_PARSER_BANDS + 2))
#else
#define	DRV_DMX_NUM_PID_CHANNEL (16*(CONFIG_NUM_PARSER_BANDS + 1))
#endif
#else
#if (defined(CONFIG_CAS_NA) || defined(CONFIG_CAS_CX)) && !defined(CONFIG_TEMP_ICORDPRO)
/* The CAS contents offer more services(audio, subtitle. etc...) than FTA contents
   thus, providing more pid channels are necessary for CAS. */
#define	DRV_DMX_NUM_PID_CHANNEL (16*(4 + 1) + 8*(CONFIG_NUM_PARSER_BANDS-4) + 40)
#else
#if defined(CONFIG_TEMP_HDR4000T)
#define	DRV_DMX_NUM_PID_CHANNEL (16*(4 + 1) + 8*(CONFIG_NUM_PARSER_BANDS-4) + 24) /* up to 128 */
#else
#define	DRV_DMX_NUM_PID_CHANNEL (16*(4 + 1) + 8*(CONFIG_NUM_PARSER_BANDS-4))
#endif
#endif
#endif
#else
#if defined(CONFIG_ZAPPING_ONLY)
#define DRV_DMX_NUM_PID_CHANNEL					32
#else
#if defined(CONFIG_TEMP_HMS1000SPH2)
#define DRV_DMX_NUM_PID_CHANNEL					160
#else
#define DRV_DMX_NUM_PID_CHANNEL					128 // 7405 has 256 pid channel but we use 128
#endif
#endif
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum _DRV_DEMUX_PLAYBACK_MODE
{
	DRV_DEMUX_PLAYBACK_INTERNAL,
	DRV_DEMUX_PLAYBACK_EXTERNAL,
	DRV_DEMUX_PLAYBACK_UNKNOWN
}DRV_DEMUX_PLAYBACK_MODE;

#if defined(CONFIG_DI20)
/**
@brief Pid channel type
*/
typedef enum DRV_DEMUX_CHANNELTYPE
{
	DRV_DEMUX_CHANNELTYPE_VIDEO = 0,
	DRV_DEMUX_CHANNELTYPE_AUDIO,
	DRV_DEMUX_CHANNELTYPE_AUDIO_MAIN = 1,
	DRV_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION,
	DRV_DEMUX_CHANNELTYPE_AUDIO_SUB = 2,
	DRV_DEMUX_CHANNELTYPE_AUDIO_SPDIF,
	DRV_DEMUX_CHANNELTYPE_VIDEO_PRIMER,
	DRV_DEMUX_CHANNELTYPE_AUDIO_PRIMER,
	DRV_DEMUX_CHANNELTYPE_PCR_PRIMER,
	DRV_DEMUX_CHANNELTYPE_PCR,
	DRV_DEMUX_CHANNELTYPE_RECORD,
	DRV_DEMUX_CHANNELTYPE_SECTION,
	DRV_DEMUX_CHANNELTYPE_PES,
	DRV_DEMUX_CHANNELTYPE_ALLPASS,
	DRV_DEMUX_CHANNELTYPE_PRIVATE, /*Channel created by demux internally (shall not be used for channel requested by above layers explicitly.)*/
	DRV_DEMUX_CHANNELTYPE_REMUX,
	/* add for PIP*/
	DRV_DEMUX_CHANNELTYPE_VIDEO_SUB,
	DRV_DEMUX_CHANNELTYPE_PCR_SUB,
	/* add for transcode*/
	DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_1,
	DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_1,
	DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1,
	DRV_DEMUX_CHANNELTYPE_PCR_TRANS_1,
	DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_2,
	DRV_DEMUX_CHANNELTYPE_AUDIO_TRANS_2,
	DRV_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2,
	DRV_DEMUX_CHANNELTYPE_PCR_TRANS_2,
	DRV_DEMUX_CHANNELTYPE_MAX
} DRV_DEMUX_CHANNELTYPE;
#else
typedef enum DRV_DEMUX_CHANNELTYPE
{
	DRV_DEMUX_CHANNELTYPE_VIDEO = 0,
	DRV_DEMUX_CHANNELTYPE_AUDIO,
	DRV_DEMUX_CHANNELTYPE_AUDIO_MAIN = 1,
	DRV_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION,
	DRV_DEMUX_CHANNELTYPE_AUDIO_SUB = 2,
	DRV_DEMUX_CHANNELTYPE_AUDIO_SPDIF,
	DRV_DEMUX_CHANNELTYPE_VIDEO_PRIMER,
	DRV_DEMUX_CHANNELTYPE_AUDIO_PRIMER,
	DRV_DEMUX_CHANNELTYPE_PCR_PRIMER,
	DRV_DEMUX_CHANNELTYPE_PCR,
	DRV_DEMUX_CHANNELTYPE_RECORD,
	DRV_DEMUX_CHANNELTYPE_SECTION,
	DRV_DEMUX_CHANNELTYPE_PES,
	DRV_DEMUX_CHANNELTYPE_ALLPASS,
	DRV_DEMUX_CHANNELTYPE_PRIVATE, /*Channel created by demux internally (shall not be used for channel requested by above layers explicitly.)*/
	DRV_DEMUX_CHANNELTYPE_REMUX,
	DRV_DEMUX_CHANNELTYPE_MAX
} DRV_DEMUX_CHANNELTYPE;
#endif

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_DEMUX_Init(void);

NEXUS_PidChannelHandle DRV_DEMUX_GetVideoPidChannel(HUINT32 unDeviceId);
NEXUS_PidChannelHandle DRV_DEMUX_GetAudioPidChannel(HUINT32 unDeviceId);
NEXUS_PidChannelHandle DRV_DEMUX_GetPcrPidChannel(void);
NEXUS_StcChannelHandle DRV_DEMUX_GetStcChannelHandle(void);
#if defined(CONFIG_PIP)
NEXUS_PidChannelHandle DRV_DEMUX_GetPIPPcrPidChannel(void);
NEXUS_StcChannelHandle DRV_DEMUX_GetPIPStcChannelHandle(void);
#endif
NEXUS_PidChannelHandle DRV_DEMUX_GetRecordPidChannel(HUINT32 channelIndex);
HUINT32	DRV_DEMUX_GetSyncCount(HUINT32 ulIBIndex);

DRV_Error DRV_DEMUX_BackupCallback(HBOOL backup);
HBOOL DRV_DEMUX_IsConnected(HUINT32 unDeviceId);
DRV_Error DRV_DEMUX_PrintInfo(HUINT32 demux_id);
DRV_Error DRV_DEMUX_PrintFilterInfo(HUINT32);
DRV_Error DRV_DEMUX_RouteToInternalRmxFeedback(HUINT32 ulDemuxId);
DRV_Error DRV_DEMUX_SetSectionDbgMode(HUINT32 SectionType, HBOOL bOn);
DRV_Error DRV_DEMUX_DebugSectionFilter(void);
NEXUS_ParserBand DRV_DEMUX_GetParserBandByDemuxId(HUINT32 demuxId);
DRV_Error DRV_DEMUX_StartGettingTSCStatus(HUINT32 ulMainSubId);
DRV_Error DRV_DEMUX_StopGettingTSCStatus(HUINT32 ulMainSubId);
DRV_Error DRV_DEMUX_GetTscStatus(HUINT32 ulMainSubId);

DRV_Error DRV_DEMUX_GetPlaybackOutputPath (HUINT32, HBOOL *);
DRV_Error DRV_DEMUX_StartRecordPID(HUINT32 ulRecDeviceId, HUINT16 usPid, HUINT32 *pulPIDChannel, HBOOL bFullTs);
DRV_Error DRV_DEMUX_StopRecordPID(HUINT32 ulPIDChannel, HBOOL bFullTs);
DRV_Error DRV_DEMUX_StartRecordTSC(HUINT32);
HUINT32 DRV_DEMUX_GetPIDChannelForDSC(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulPidChannel);
DRV_Error DRV_DEMUX_ConnectPIDChannelToDSC(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulPidChannel);
DRV_Error DRV_DEMUX_DisconnectPIDChannelFromDSC(HUINT32 ulPidChannel);
#if (CONFIG_DI_DEMUX_WORKAROUND_FOR_CI_PLUS_TYPE == DI_DEMUX_WORKAROUND_FOR_CI_PLUS_TYPE_ENABLED)
DRV_Error DRV_DEMUX_ConnectPIDChannelToDSC2nd(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulPidChannel);
DRV_Error DRV_DEMUX_DisconnectPIDChannelFromDSC2nd(HUINT32 ulPidChannel);
#endif
HBOOL DRV_DEMUX_CheckIfFilterBeAttached(DRV_DEMUX_CHANNELTYPE channelType);
DRV_Error DRV_DEMUX_CamBypass(unsigned short);
DRV_Error DRV_DEMUX_CamPassthrough(unsigned short);
DRV_Error DRV_DEMUX_GetCAMBypassStatus(unsigned short, HUINT32 *, HBOOL *);

HBOOL DRV_DEMUX_IsPlaybackPidChannel(NEXUS_PidChannelHandle,  DRV_DEMUX_PLAYBACK_MODE *);
DRV_Error DRV_SetTs2CIInput( HUINT32 ulTunerId,HUINT32 ulCiInput );

#if defined(CONFIG_TRANSCODER)
NEXUS_StcChannelHandle DRV_DEMUX_GetSTCChannel( HUINT32 unDeviceId );
NEXUS_StcChannelHandle DRV_DEMUX_GetSTCChannelTranscode( HUINT32 unDeviceId );
NEXUS_PidChannelHandle DRV_DEMUX_GetPidChannelForTranscode(HUINT32 ulDemuxId,DRV_DEMUX_CHANNELTYPE drvChannelType);
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
DRV_Error DRV_DEMUX_GetVideoPrimerPidChannel(HUINT32 ulPrimerIndex, NEXUS_PidChannelHandle *hPidChannel);
NEXUS_StcChannelHandle DRV_DEMUX_GetPrimerStcChannelHandle(HUINT32 ulPrimerIndex);
DRV_Error DRV_DEMUX_GetStcChannelIndex(NEXUS_StcChannelHandle hStcHandle, HUINT32 *pStcChannelIndex);
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
DRV_Error DRV_DEMUX_GetAudioPrimerPidChannel(HUINT32 ulPrimerIndex, NEXUS_PidChannelHandle *hPidChannel);
#endif
int DRV_DEMUX_GetFirstPlaybackParserIndex(void);
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
DRV_Error DRV_DEMUX_SetPlaybackPath(HUINT32 ulDemuxId);
#endif


#if defined(CONFIG_DEMOD_TSMF)
DRV_Error DRV_DEMUX_GetLiveInputBand(HINT32 nSearchIdx, HUINT32* punDemuxId, HUINT32* punInputBand);
NEXUS_InputBand DRV_DEMUX_GetUnusedInputBand(void);
#endif

DRV_Error DRV_DEMUX_GetRecordPidChannelInfo(HUINT32 channelIndex, HUINT32 *pulDemuxId, HUINT16 *pusPid);
HBOOL DRV_DEMUX_CheckIfDecoderInputIsMediaPlayer(void);

DRV_Error DRV_DEMUX_GetOutPutDeviceType(HUINT32 ulDemuxId , DI_DEMUX_OUTPUTDEVICETYPE * etConsumer);
DRV_Error DRV_DEMUX_GetInputDevice(HUINT32 ulDemuxId , DI_DEMUX_INPUTDEVICETYPE * etProducer, HUINT32 * ulDeviceId);

#endif /* !__DRV_DEMUX_H__ */

