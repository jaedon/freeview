/**
 * drv_pvr.h
*/

#ifndef __DRV_PVR_H__
#define __DRV_PVR_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "di_pvr.h"
#include "drv_demux.h"
#include "drv_err.h"

#include "nexus_base_types.h"
#if	defined(CONFIG_RECORD)
#include "nexus_record.h"
#include "nexus_file.h"
#include "nexus_file_fifo.h"
#endif
#if	defined(CONFIG_PLAYBACK) || defined(CONFIG_MEDIA)
#include "nexus_playback.h"
#endif

#if	defined(CONFIG_DELAYED_RECORD) || defined(CONFIG_THUMBNAIL_DECODING)
#include "nexus_file_chunk.h"
#include "nexus_file_fifo_chunk.h"
#endif

#include "nexus_pid_channel.h"
#if defined(CONFIG_VIDEO_DECODER)
#include "nexus_video_decoder.h"
#endif
#if defined(CONFIG_AUDIO_DECODER)
#include "nexus_audio_decoder.h"
#endif
//#include "nexus_security.h"
#if defined(CONFIG_DSC)
#include "nexus_keyladder.h"
#endif
#include "nexus_playpump.h"
#if	defined(CONFIG_PVR)
#include "bfile_stdio.h"
#endif

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

#if defined(CONFIG_MAX_PLAYBACK_INSTANCE)
#define MAX_PLAY_INST_FOR_PB	CONFIG_MAX_PLAYBACK_INSTANCE
#else
#define MAX_PLAY_INST_FOR_PB	1
#endif

#if defined(CONFIG_MAX_RECORD_INSTANCE)
#define MAX_REC_INST_FOR_REC  CONFIG_MAX_RECORD_INSTANCE
#else
#define MAX_REC_INST_FOR_REC  3
#endif

#if defined(CONFIG_MAX_TRANSCODER_CHANNEL)
#define MAX_PVR_INST_FOR_TRANS CONFIG_MAX_TRANSCODER_CHANNEL
#else
#define MAX_PVR_INST_FOR_TRANS 0
#endif

#if defined(CONFIG_MAX_SATIP_CHANNEL)
#define MAX_PVR_INST_FOR_SATIP CONFIG_MAX_SATIP_CHANNEL
#else
#define MAX_PVR_INST_FOR_SATIP 0
#endif

#if defined(CONFIG_SATIP)
#define MAX_PVR_REC_SATIP_TIMEOUT 100 /* recording data callback timeout 100msec for SAT/IP */
#endif

#define	MAX_PLAY_INST_NUM		((MAX_PLAY_INST_FOR_PB)+(MAX_PVR_INST_FOR_TRANS))
#define MAX_REC_INST_NUM	((MAX_REC_INST_FOR_REC)+(MAX_PVR_INST_FOR_TRANS)+(MAX_PVR_INST_FOR_SATIP))

#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#define DRV_PVR_ATOM_SIZE ((192/4)*4096)
#else
#define DRV_PVR_ATOM_SIZE ((188/4)*4096)
#endif
#define BUFF_GROUP_0 0 /*small buffer*/
#define BUFF_GROUP_1 1 /*large buffer*/
#define BUFF_GROUP_2 2 /*ci*/
#define BUFF_GROUP_3 3 /*transcoder buffer*/
#define BUFF_GROUP_4 4 /*playready buffer*/

#define DRV_PVR_PLAY_BUFFER (DRV_PVR_ATOM_SIZE*8) /* = 1540096 or 1504K or 1.5MB (timestamp Ãß°¡ ½Ã 1572864 or 1536K or 1.5MB) */
#define DRV_PVR_PLAY_BUFFER_LARGE (DRV_PVR_ATOM_SIZE*20) /* about 3.8MB, required for smooth streaming */
#define DRV_PVR_PLAY_DESC_NUM 100

/* The default bufferSize is based on the least common mulitple of a 192 byte transport packet and 4096 atomSize (for O_DIRECT file i/o).
	    Then, multiply by 192 to get 192 of these LCM's. That's the default buffer.
	    Finally, add 64 in order to anticipate the XCPACKET_SIZE replacement logic in NEXUS_Recpump_P_Start. 256-192 = 68. This is not a crucial step. We could have chosen
	    to not add it, but it would have meant that the final buffer would be less than 192 LCM's after the XCPACKET_SIZE replacement logic. */
#if defined(CONFIG_JAPAN_SPEC) || defined(CONFIG_PVR_REC_BUFFER_2X)
#define DRV_PVR_REC_BUFFER_LARGE (DRV_PVR_ATOM_SIZE*14*2*2)	/* (DRV_PVR_ATOM_SIZE*12)*2 */ /* Need 10.28125MB for network record */
#endif
#if defined(CONFIG_TEMP_HDR4000T)
#define DRV_PVR_REC_BUFFER (DRV_PVR_ATOM_SIZE*7*2)	/* (DRV_PVR_ATOM_SIZE*7)*2 */ /* 2.625MB for CDB */
#else
#define DRV_PVR_REC_BUFFER (DRV_PVR_ATOM_SIZE*14*2)	/* (DRV_PVR_ATOM_SIZE*12)*2 */ /* Need 4.4MB for network record */
#endif

#if defined(CONFIG_LOCAL_TIMESTAMP_ONOFF_SWITCH)
#define DRV_PVR_REC_BUFFER_ADD 68
#elif defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#define DRV_PVR_REC_BUFFER_ADD 64
#else
#define DRV_PVR_REC_BUFFER_ADD 68
#endif

#define DRV_PVR_REC_DESC 12

//only use copy task
//#define CONFIG_TEST_COPY_TASK
#define DEFAULT_THUMBNAIL_DECODER 0
#define MAX_FILE_LENGTH  255
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct stDRV_PVR_REC_DEVICE_INFO
{
	HINT32 deviceId;
#if	defined(CONFIG_RECORD)
	NEXUS_RecordHandle recordHandle;
#if defined(CONFIG_DELAYED_RECORD)
	NEXUS_ChunkedFifoRecordHandle fifoRecHandle;
#else
	NEXUS_FifoRecordHandle fifoRecHandle;
#endif
#endif
#if defined(CONFIG_DELAYED_RECORD)
	int delayedRecordOn;
#endif
} DRV_PVR_REC_DEVICE_INFO;

typedef struct
{
	HUINT8 			ucAVFileName[MAX_FILE_LENGTH];
	HUINT8			ucIndexFileName[MAX_FILE_LENGTH];
	HUINT32			ulCaptureTime;
	DI_PVR_CRYPTO_SETTINGS	stDecryptSettings;
	HUINT32			VideoCodec;
	HUINT32			ulVideoPid;
	HBOOL			bTsr;
	HINT32			iRecDeviceId;
	HUINT32			ulPacketSize; /* 188 or 192*/
} DRV_PVR_THUMBNAIL_SETUP_t;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_PVR_GetPlaypumpHandleByClass(NEXUS_PlaypumpHandle* pPlaypumpHandle, HUINT32 ulClass);
DRV_Error DRV_PVR_GetPlaypumpHandle(NEXUS_PlaypumpHandle* pPlaypumpHandle);
DRV_Error DRV_PVR_GetPlaypumpHandleByIndex(HUINT32 uiIndex, void *pHandle);

#if defined(CONFIG_FAST_AUDIO_CHANGE)
DRV_Error DRV_PVR_PB_GetPlaybackHandle(NEXUS_PidChannelHandle pstPIDChannel, NEXUS_PlaybackHandle *ppstPlaybackHandle);
#endif

#if defined(CONFIG_TRANSCODER)
DRV_Error DRV_PVR_GetStreamMuxPlaypumpHandle(NEXUS_PlaypumpHandle* pPlaypumpHandle);
DRV_Error DRV_PVR_PLAY_GetDeviceIDForTranscoding(HUINT32 ulTranscoderDeviceId, HUINT32 *pulPbDeviceId);
DRV_Error DRV_PVR_REC_GetDeviceIDForTranscoding(HUINT32 ulTranscoderDeviceId, HUINT32 *pulRecDeviceId);
#endif
DRV_Error DRV_PVR_ReleasePlaypumpHandle(NEXUS_PlaypumpHandle playpumpHandle);

DRV_Error DRV_PVR_REC_Init(void);
DRV_Error DRV_PVR_REC_GetDeviceInfo(HUINT32 ulDeviceId, DRV_PVR_REC_DEVICE_INFO *pstRecDevInfo);

DRV_Error DRV_PVR_PLAY_Init(void);
DRV_Error DRV_PVR_PLAY_BackupCallback(HUINT32 ulDeviceId, HBOOL backup);
DRV_Error DRV_PVR_PLAY_PidChannelOpen(HUINT32 ulDeviceId, DRV_DEMUX_CHANNELTYPE nChannelType, HUINT16 usPid, NEXUS_PidChannelHandle *pulPidChannelHandle);
DRV_Error DRV_PVR_PLAY_PidChannelClose(HUINT32 ulDeviceId, NEXUS_PidChannelHandle ulPidChannelHandle, DRV_DEMUX_CHANNELTYPE ucChannelType);
DRV_Error DRV_PVR_PLAY_PidChannelOpenDummy(HUINT32 ulDeviceId, DRV_DEMUX_CHANNELTYPE nChannelType, HUINT16 usPid, NEXUS_PidChannelHandle ulPidChannelHandle);
DRV_Error DRV_PVR_PLAY_PidChannelCloseDummy(HUINT32 ulDeviceId, NEXUS_PidChannelHandle ulPidChannelHandle, DRV_DEMUX_CHANNELTYPE nChannelType);
#if defined(CONFIG_VIDEO_DECODER)
DRV_Error DRV_PVR_PLAY_VideoPidChannelSetting(NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_VideoDecoderHandle ulVideoDecHandle, NEXUS_VideoCodec nVideoCodec);
#endif
#if defined(CONFIG_AUDIO_DECODER)
DRV_Error DRV_PVR_PLAY_AudioPidChannelSetting(
	NEXUS_PidChannelHandle ulPidChannelHandle,
	HBOOL isSubAudioPid,
	NEXUS_AudioDecoderHandle ulAudioDecHandle,
	NEXUS_AudioDecoderHandle ulAudioPassThroughDecHandle
);
#endif
DRV_Error DRV_PVR_PLAY_SetStcChannelHandle(HUINT32 ulDeviceId, NEXUS_StcChannelHandle ulStcChannelHandle);
DRV_Error DRV_PVR_PLAY_UnsetStcChannelHandle(HUINT32 ulDeviceId);
#if defined(CONFIG_TRANSCODER) /*FIXIT: compatilbility issue*/
DRV_Error DRV_PVR_PLAY_PidChannelOpenWithDemuxId(HUINT32 ulDemuxId, DRV_DEMUX_CHANNELTYPE nChannelType, HUINT16 usPid, NEXUS_PidChannelHandle *pulPidChannelHandle);
DRV_Error DRV_PVR_PLAY_PidChannelCloseWithDemuxId(HUINT32 ulDemuxId, NEXUS_PidChannelHandle ulPidChannelHandle, DRV_DEMUX_CHANNELTYPE nChannelType);
DRV_Error DRV_PVR_PLAY_SetStcChannelHandleWithDemuxId(HUINT32 ulDemuxId, NEXUS_StcChannelHandle ulStcChannelHandle);
DRV_Error DRV_PVR_PLAY_UnsetStcChannelHandleWithDemuxId(HUINT32 ulDemuxId);
#endif
DRV_Error DRV_PVR_PLAY_SetRecordPath(HUINT32 ulDeviceId, HUINT32 ulRecDeviceId);
DRV_Error DRV_PVR_PLAY_IsPaused(HUINT32 ulDeviceId, HBOOL *pbPause );

#if	defined(CONFIG_TEST_COPY_TASK)
DRV_Error DRV_PVR_REC_StartCopyTask(void);
#endif
DRV_Error DRV_PVR_PB_ThumbnailInit(NEXUS_VideoCodec videoCodec, NEXUS_PidChannelHandle *pPidChannel, DRV_PVR_THUMBNAIL_SETUP_t *pthumbSetup);
DRV_Error DRV_PVR_PB_ThumbnailFeed(unsigned int timesec);
void DRV_PVR_PB_ThumbnailUninit(void);
DRV_Error DRV_PVR_RequestAllpassChannel(HUINT32 ulDemuxId, NEXUS_PidChannelHandle *pidChannel);

#if defined(CONFIG_PVR_EDIT)
DRV_Error DRV_PVR_EDIT_Init(void);
#endif

void DRV_PVR_TunePdflush(HBOOL bTunePdflushForUsbFlash);

#endif /* !__DRV_AUDIO_H__ */

