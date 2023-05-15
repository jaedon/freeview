/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: ip_streamer.h $
 * $brcm_Revision: 63 $
 * $brcm_Date: 10/1/12 11:03a $
 * 
 * Module Description:
 *  ip streamer include file
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/playback_ip/apps/ip_streamer.h $
 * 
 * 63   10/1/12 11:03a ssood
 * SW7425-3970: added support to transcode from IP src
 * 
 * 62   9/13/12 11:19a aginesh
 * SW7435-353: changes to get ip_streamer to work with trellis
 * 
 * 61   8/16/12 6:06p ssood
 * SW7425-3482: added support for allowing clients to specify the pids to
 *  demux in Sat/IP protocol + handle pilotTone setting
 * 
 * 60   7/26/12 8:33p ssood
 * SW7346-944: add support to stream from OFDM-capable frontend
 * 
 * 59   7/13/12 12:11a ssood
 * SW7425-3482: extend ip_streamer to support BSkyB's Sat/Ip Protocol
 * 
 * 58   6/29/12 6:50p ssood
 * SW7346-897: raaga encoding changes for 7346 platform
 * 
 * 57   5/29/12 10:20a ssood
 * SW7425-3042: fixed a warning for platforms with 2 parser bands
 * 
 * 56   5/17/12 6:36p ssood
 * SW7425-3042: simplified build flags & code by using nexus multi process
 *  capabilities
 * 
 * 55   5/14/12 4:33p ssood
 * SW7425-3042: refactor ip_streamer into multiple sub files
 * 
 * 54   5/3/12 4:04p ssood
 * SW7346-805: further refinements to play timeshifted content via the
 *  nexus playback
 * 
 * 53   5/1/12 6:22p ssood
 * SW7346-805: enhance ip_streamer to use playback path for streaming out
 *  timeshifted live content
 * 
 * 52   4/13/12 7:39p ssood
 * SW7346-782: added support for streaming from streamer input
 * 
 * 51   3/4/12 9:47a ssood
 * SW7425-2331: dont close pid channels for shared xcode sessions until
 *  the last session is closed + coverity fixes
 * 
 * 50   1/18/12 3:20p ssood
 * SW7231-603: fixed cross compilation warnings
 * 
 * 49   1/5/12 1:37a ssood
 * SW7231-561: lower the number of recpumps used for streaming to clients
 *  to provide memory for xcode path
 * 
 * 48   1/4/12 3:22p ssood
 * SW7231-561: use fast status call to determine frontend tuning status
 * 
 * 47   12/29/11 12:20p alanlam
 * SW3128-62: Add support fo spectrum analyzer data retrieval.
 * 
 * 46   12/19/11 12:56a ssood
 * SW7231-516: raaga encoding support: added changes to support running
 *  raaga based xcode alongside the local decode on BMC
 * 
 * 45   12/8/11 7:40p ssood
 * SW7231-516: enhance ip_streamer to stream out raaga encoded TS streams
 *  from live or file sources
 * 
 * 44   10/18/11 7:25a ssood
 * SW7425-889: enabled audio in the live xcoded streams for streaming to
 *  iOS devices
 * 
 * 43   10/14/11 10:18a ssood
 * SW7425-889: initial check-in for enabling audio in the live xcoded
 *  streams
 * 
 * 42   9/26/11 10:22a ssood
 * SW7346-341: Add PVR Decryption support for file streaming
 * 
 * 41   8/27/11 7:37p ssood
 * SW7425-1077: lowered the number of recpumps for streaming to allow 4
 *  FCC clients on VMS
 * 
 * 40   8/5/11 3:13p ssood
 * SW7425-1040: add fast channel change support to IP Sessions
 * 
 * 39   7/15/11 2:13p ssood
 * SW7346-309: move MAX_PROGRAMS_PER_FREQUENCY out of LIVE_STREAMING flag
 * 
 * 38   7/14/11 12:01p ssood
 * SW7346-309: Add support for Streaming from Satellite Source
 * 
 * 37   6/18/11 11:52a ssood
 * SW7425-718: 3128 related changes: use different frontend resources for
 *  each streaming session even though it is same channel
 * 
 * 36   4/29/11 2:10p ssood
 * SW7425-175: more changes to handle stop/start condition on xcoded
 *  sessions
 * 
 * 35   3/18/11 6:44p ssood
 * SW7420-1616: allow app to pass-in dma handle
 * 
 * 34   3/16/11 4:55p ssood
 * SW7420-1616: initial check-in for pvr encryption support
 * 
 * 33   2/17/11 6:24p ssood
 * SW7425-59: initial check-in for audio passthru support for transcoded
 *  channels
 * 
 * 32   1/20/11 3:28p ssood
 * SW7425-59: switch decode to live mode in xcode path + all ces related
 *  changes + support for 2nd xcode client
 * 
 * 31   12/31/10 3:25p ssood
 * SW7425-59: add change to support clean stop/stop sequence w/ 7425
 *  xcoder + insert PAT/PMT into outgoing stream
 * 
 * 30   12/30/10 1:06p ssood
 * SW7425-59: surround all 7425 transcoder changes w/
 *  NEXUS_HAS_VIDEO_ENCODER compile flag
 * 
 * 29   12/30/10 12:55p ssood
 * SW7425-59: initial 7425 transcoder support on h/w platform (no PSI, no
 *  PCRs, no Audio in xcoded stream)
 * 
 * 28   12/28/10 12:28p ssood
 * SW7425-59: Add support for using HDMI-in as the source for
 *  transcoding+streaming out
 * 
 * 27   12/26/10 10:45p ssood
 * SW7425-59: Add support for 7425 transcoder
 * 
 * 26   9/2/10 3:11p garetht
 * SW7420-1046: Add support for sending 6xQAM256 streams to the cablecard
 * 
 * 25   8/12/10 11:28a ssood
 * SW7420-883: fixed compiler error for non-transcode case
 * 
 * 24   8/12/10 11:24a ssood
 * SW7420-883: added support for streaming same transcoding session to
 *  multiple clients
 * 
 * 23   7/27/10 2:08p ssood
 * SW7420-883: add transcoding support to IP Streamer (no audio
 *  transcoding yet)
 * 
 * 22   1/27/10 10:18a ssood
 * SW7420-454: conditionally compile Live Streaming Code using
 *  LIVE_STREAMING_SUPPORT
 * 
 * 21   1/14/10 6:41p ssood
 * SW7408-47: Add support to compile IP Streamer App on platforms w/ no
 *  frontend support
 * 
 * 20   12/22/09 11:03a ssood
 * SW7420-454: add timeshift support for live streaming
 * 
 * 19   12/15/09 9:38a ssood
 * SW7420-454: Add support to share a parser band among clients using same
 *  frequency & channel numbers
 * 
 * 18   11/20/09 1:35p jrubio
 * SW7335-537: change number of recpumps
 * 
 * 17   11/17/09 2:58p ssood
 * SW7420-454: Enhance IP Streamer to stream files from local disk
 * 
 * 16   11/2/09 3:56p ssood
 * SW7420-340: add support for recording a live stream
 * 
 * 15   10/20/09 6:33p ssood
 * SW7420-340: remove nexus playback, audio, video decoder dependencies
 *  for SMS compilation
 * 
 * 14   10/11/09 8:52p ssood
 * SW7420-340: added support to bind ip_streamer to a particular network
 *  interface
 * 
 * 13   10/10/09 10:49p ssood
 * SW7420-340: Changes to allow non-Broadcom DLNA clients play live
 *  content
 * 
 * 12   10/9/09 12:29p ssood
 * SW7420-340: add support to play live streams on non-Broadcom clients
 * 
 * 11   10/7/09 3:46p ssood
 * SW7420-340: added STANDL
 * 
 * 10   10/6/09 12:18p ssood
 * SW7420-340: Add DTCP/IP support to ip_streamer & IP Applib
 * 
 * 9   10/2/09 5:08p ssood
 * SW7420-340: Adapt number of resources as per # of video decoders needed
 *  for SMS/VMS differentiation
 * 
 * 8   10/2/09 4:12p ssood
 * SW7420-340: add logic to recover from buffer overflow condition (caused
 *  by client backing up)
 * 
 * 7   10/2/09 11:54a ssood
 * SW7420-340: increased the max programs per channels to 24
 * 
 * 6   9/29/09 11:01p ssood
 * SW7420-340: return error to client if PSI acquisition fails + wait for
 *  signal lock
 * 
 * 5   9/25/09 6:31p ismailk
 * SW7405-3080: Add a field for the serverIp addr in the
 *  IpStreamerGlobalCfg structure.
 * 
 * 4   9/25/09 5:15p ssood
 * SW7420-340: Add support for VSB frontend
 * 
 * 3   9/17/09 2:25p ssood
 * SW7420-340: include nexus files pertaining to ip streaming only
 * 
 * 2   9/16/09 2:33p ssood
 * SW7420-340: print debug stats
 * 
 * 1   9/16/09 10:50a ssood
 * SW7420-340: Adding example apps for demonstrating IP Streaming & Client
 *  features of IP Applib
 * 
 ******************************************************************************/
#ifndef __IP_STREAMER_H__
#define __IP_STREAMER_H__

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "nexus_file_fifo.h"

#ifdef NEXUS_HAS_VIDEO_ENCODER
#include "b_os_lib.h"
#include "nexus_video_encoder.h"
#include "nexus_audio_encoder.h"
#endif
#if NEXUS_HAS_STREAM_MUX
#include "nexus_stream_mux.h"
#endif
#ifdef NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif

#if NEXUS_HAS_HDMI_INPUT
#include "nexus_hdmi_input.h"
#endif

#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif

/* confirm the max jitter we can have on DoCSYS/IP Path */
#define DOCSIS_IP_NETWORK_JITTER 100
#define IP_STREAMER_DEFAULT_SERVER_PORT 5000
#define IP_STREAMER_DEFAULT_DTCP_IP_PORT 8000

/* Max Number of concurrent sessions that need to be supported for a platform */
#define IP_STREAMER_MAX_THREADS 8

/* Max Number of IP Frontends for a platforms: assume 1/2 for live ip streaming */
#define IP_STREAMER_NUM_PLAYPUMPS_FOR_LIVE_IP (NEXUS_NUM_PLAYPUMPS/2)
/* recording sessions over IP */
#define IP_STREAMER_NUM_RECPUMPS_FOR_RECORD (2)
/* how many live sessions (qam/sat/ip/vsb/streamer) can be streamed by recording via recpump */
#define IP_STREAMER_NUM_RECPUMPS_FOR_STREAMING (NEXUS_NUM_RECPUMPS/2)

#define IP_STREAMER_NUM_PARSER_BANDS NEXUS_NUM_PARSER_BANDS

/* leaving two parser bands for satellite input: special case for 7346 */
#if (NEXUS_NUM_PARSER_BANDS > 2)
#define IP_STREAMER_NUM_STREAMER_INPUT NEXUS_NUM_PARSER_BANDS-2
#else
#define IP_STREAMER_NUM_STREAMER_INPUT 1
#endif

#ifdef NEXUS_HAS_VIDEO_ENCODER
typedef struct TranscoderDst {
    /* nexus resources needed to setup the decode and re-encode pipe */
    NEXUS_Timebase timebase;
    int displayIndex; /* compositor to connect to the video encoder */
    NEXUS_StcChannelHandle decodeStcChannel;
    NEXUS_StcChannelHandle encodeStcChannel;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_DisplayHandle displayTranscode;
    NEXUS_VideoWindowHandle windowTranscode;
    NEXUS_DisplayHandle displayMain;
    NEXUS_VideoWindowHandle windowMain;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelHandle syncChannel;
#endif
    int decoderIndex;

    NEXUS_VideoEncoderHandle videoEncoder;
    NEXUS_StreamMuxHandle streamMux;
    NEXUS_AudioMuxOutputHandle audioMuxOutput;
    NEXUS_AudioEncoderHandle audioEncoder;
    NEXUS_PlaypumpHandle playpumpTranscodeVideo;
    NEXUS_PlaypumpHandle playpumpTranscodeAudio;
    NEXUS_PlaypumpHandle playpumpTranscodeSystemData;

    NEXUS_PidChannelHandle transcodeVideoPidChannelCopy;
    NEXUS_PidChannelHandle transcodeAudioPidChannelCopy;

    /* AV pid channel pointers used in the xcode path */
    /* stored here so that they are closed only when last instance of a xcode session is closed */
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_PidChannelHandle audioPidChannel;
    NEXUS_PidChannelHandle pcrPidChannel;
    
/* multi buffer to allow the background PSI packet to update CC while foreground one is waiting for TS mux pacing */
#define IP_STREAMER_PSI_QUEUE_CNT 4 /* every second insert a PSI, so it takes 4 secs to circle; hopefully its transfer won't be delay that far */
    void *pat[IP_STREAMER_PSI_QUEUE_CNT];
    void *pmt[IP_STREAMER_PSI_QUEUE_CNT];
    unsigned ccValue;
	B_MutexHandle mutexSystemData;
	B_SchedulerHandle schedulerSystemData;
	B_SchedulerTimerId systemDataTimer;
	B_ThreadHandle schedulerThread;
	bool systemDataTimerIsStarted;
    NEXUS_StreamMuxSystemData psi[2];
    BKNI_EventHandle finishEvent;  /* set when Finished callback is received from Mux mgr */
    bool inUse;
    int refCount;
    bool started; /* set when transcoding is started on this context */
    int contextId; /* transcoder context id */
} TranscoderDst; 
#endif /* NEXUS_HAS_VIDEO_ENCODER */

#define MAX_PROGRAMS_PER_FREQUENCY 24
#if NEXUS_HAS_FRONTEND
typedef struct QamSrc {
    NEXUS_FrontendHandle frontendHandle;
    uint32_t frequency;
    int refCount;   /* how many streaming sessions are using this frontend */
    B_PlaybackIpPsiInfo psi[MAX_PROGRAMS_PER_FREQUENCY];
    int numProgramsFound; /* how many sub-channels are found in this frequency */
    bool psiAcquiring; /* set if PSI info is been acquired */
    bool skipFrontendTuning; /* set if frontend tuner is already setup during PSI discovery */
    BKNI_EventHandle psiAcquiredEvent;  /* set when PSI aquisition is completed */
    BKNI_EventHandle signalLockedEvent;  /* set when tuner locks in to the frequency */
    BKNI_EventHandle signalSpecADataRdy; /* set when all the spectrum analyzer data has been received*/
	int totalFftSamples; /* total number of FFT samples received for spectrum analyzer */
	uint32_t *fftData; /* pointer to the array that stroes the FFT data for spectrum analyzer */
	bool started; /* set when Src is started */
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to one src when it shared between multiple clients */
#ifdef NEXUS_HAS_VIDEO_ENCODER
    bool transcodeEnabled;  /* set when the 1st transcoding session is started for this qam frontend */
#endif
} QamSrc;

typedef struct SatSrc {
    NEXUS_FrontendHandle frontendHandle;
    uint32_t frequency;
    int refCount;   /* how many streaming sessions are using this frontend */
    B_PlaybackIpPsiInfo psi[MAX_PROGRAMS_PER_FREQUENCY];
    int numProgramsFound; /* how many sub-channels are found in this frequency */
    bool psiAcquiring; /* set if PSI info is been acquired */
    bool skipFrontendTuning; /* set if frontend tuner is already setup during PSI discovery */
    BKNI_EventHandle psiAcquiredEvent;  /* set when PSI aquisition is completed */
    BKNI_EventHandle signalLockedEvent;  /* set when tuner locks in to the frequency */
    bool started; /* set when Src is started */
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to one src when it shared between multiple clients */
#ifdef NEXUS_HAS_VIDEO_ENCODER
    bool transcodeEnabled;  /* set when the 1st transcoding session is started for this qam frontend */
#endif
} SatSrc;

typedef struct OfdmSrc {
    NEXUS_FrontendHandle frontendHandle;
    uint32_t frequency;
    int refCount;   /* how many streaming sessions are using this frontend */
    B_PlaybackIpPsiInfo psi[MAX_PROGRAMS_PER_FREQUENCY];
    int numProgramsFound; /* how many sub-channels are found in this frequency */
    bool psiAcquiring; /* set if PSI info is been acquired */
    bool skipFrontendTuning; /* set if frontend tuner is already setup during PSI discovery */
    BKNI_EventHandle psiAcquiredEvent;  /* set when PSI aquisition is completed */
    BKNI_EventHandle signalLockedEvent;  /* set when tuner locks in to the frequency */
    bool started; /* set when Src is started */
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to one src when it shared between multiple clients */
#ifdef NEXUS_HAS_VIDEO_ENCODER
    bool transcodeEnabled;  /* set when the 1st transcoding session is started for this qam frontend */
#endif
} OfdmSrc;
#endif /* NEXUS_HAS_FRONTEND */

typedef struct StreamerSrc {
    int refCount;   /* how many streaming sessions are using this frontend */
    B_PlaybackIpPsiInfo psi[MAX_PROGRAMS_PER_FREQUENCY];
    int numProgramsFound; /* how many sub-channels are found in this frequency */
    bool psiAcquiring; /* set if PSI info is been acquired */
    bool skipFrontendTuning; /* set if frontend tuner is already setup during PSI discovery */
    BKNI_EventHandle psiAcquiredEvent;  /* set when PSI aquisition is completed */
    BKNI_EventHandle signalLockedEvent;  /* set when tuner locks in to the frequency */
    bool started; /* set when Src is started */
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to one src when it shared between multiple clients */
#ifdef NEXUS_HAS_VIDEO_ENCODER
    bool transcodeEnabled;  /* set when the 1st transcoding session is started for this streamer source */
#endif
} StreamerSrc;

typedef struct IpSrc {
    B_PlaybackIpHandle playbackIp;
#ifndef DMS_CROSS_PLATFORMS
    NEXUS_PlaypumpHandle playpump;
#endif /* DMS_CROSS_PLATFORMS */
    char srcIpAddress[256];     /* IP Address of Receiving Host */
    int srcPort;     /* Port # */
    B_PlaybackIpProtocol srcProtocol; /* Protocol: UDP/RTP */
    int refCount;   /* how many streaming sessions are using this frontend */
    B_PlaybackIpPsiInfo psi[MAX_PROGRAMS_PER_FREQUENCY];
    int numProgramsFound; /* how many sub-channels in this IP tuner */
    bool psiAcquiring; /* set if PSI info is been acquired */
    BKNI_EventHandle psiAcquiredEvent;  /* set when PSI aquisition is completed */
    bool started; /* set when Src is started */
    bool transcodeEnabled;  /* set when the 1st transcoding session is started for this parser band */
#ifdef NEXUS_HAS_VIDEO_ENCODER
    TranscoderDst *transcoderDst; /* handle to the transcoder context */
    IpStreamerTranscodeConfig transcode;
#endif
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to one src when it shared between multiple clients */
} IpSrc;

typedef struct ParserBand {
    NEXUS_ParserBand parserBand;
    uint32_t frequency;
    int subChannel;
    int refCount;
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to one parser band when it shared between multiple clients */
    bool transcodeEnabled;  /* set when the 1st transcoding session is started for this parser band */
#ifdef NEXUS_HAS_VIDEO_ENCODER
    TranscoderDst *transcoderDst; /* handle to the transcoder context */
    IpStreamerTranscodeConfig transcode;
#endif
} ParserBand; 

typedef struct IpDst {
#ifndef DMS_CROSS_PLATFORMS
    NEXUS_RecpumpHandle recpumpHandle;
    NEXUS_RecpumpHandle recpumpAllpassHandle;
#ifdef NEXUS_HAS_RECORD
    NEXUS_RecordHandle recordHandle;
    NEXUS_FileRecordHandle fileRecordHandle;
    NEXUS_FifoRecordHandle fifoFileHandle;
#endif
    NEXUS_Timebase timebase;
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle;
#endif /* DMS_CROSS_PLATFORMS */
    bool inUse;
    BKNI_EventHandle dataReadyEvent; /* indicates when data is available in the recpump buffer */
} IpDst; 

typedef struct RecDst {
#ifdef NEXUS_HAS_RECORD
    NEXUS_RecpumpHandle recpumpHandle;
    NEXUS_RecordHandle recordHandle;
    NEXUS_FileRecordHandle fileRecordHandle;
#endif
    bool inUse;
} RecDst; 

#ifdef NEXUS_HAS_HDMI_INPUT
typedef struct HdmiSrc {
    int hdmiInputId;
    NEXUS_Timebase timebase;
    NEXUS_HdmiInputHandle hdmiInput;
#ifdef NEXUS_HAS_VIDEO_ENCODER
    TranscoderDst *transcoderDst; /* handle to the transcoder context */
#endif
    bool inUse;
    int refCount;
} HdmiSrc; 
#endif

typedef struct FileSrc {
#ifdef NEXUS_HAS_VIDEO_ENCODER
    TranscoderDst *transcoderDst; /* handle to the transcoder context */
#endif
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaypumpHandle playpumpHandle;
    NEXUS_PlaybackHandle playbackHandle;
    NEXUS_FilePlayHandle filePlayHandle;
    NEXUS_StcChannelHandle playbackStcChannel;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_Timebase timebase;
#endif
    B_PlaybackIpFileStreamingHandle fileStreamingHandle;
    bool inUse;
    int refCount;
} FileSrc; 

/* state info common to all threads */
typedef struct IpStreamerGlobalCtx {
    IpStreamerGlobalCfg globalCfg;
    void *dtcpCtx;

#if NEXUS_HAS_FRONTEND
    /* list of all available vsb sources: one per vsb tuner */
    BKNI_MutexHandle vsbSrcMutex;
    QamSrc vsbSrcList[NEXUS_MAX_FRONTENDS];

    /* list of all available qam sources: one per qam tuner */
    BKNI_MutexHandle qamSrcMutex;
    QamSrc qamSrcList[NEXUS_MAX_FRONTENDS];

    /* list of all available sat sources: one per sat tuner */
    BKNI_MutexHandle satSrcMutex;
    SatSrc satSrcList[NEXUS_MAX_FRONTENDS];

    /* list of all available ofdm sources: one per ofdm tuner */
    BKNI_MutexHandle ofdmSrcMutex;
    OfdmSrc ofdmSrcList[NEXUS_MAX_FRONTENDS];
#endif /* NEXUS_HAS_FRONTEND */

    /* list of all available ip sources: one per playback channel */
    BKNI_MutexHandle ipSrcMutex; /* lock to synchronize access to the list of ip srcs */
#ifndef DMS_CROSS_PLATFORMS
    IpSrc ipSrcList[IP_STREAMER_NUM_PLAYPUMPS_FOR_LIVE_IP];
    ParserBand parserBandList[NEXUS_NUM_PARSER_BANDS];
    StreamerSrc streamerSrcList[NEXUS_NUM_PARSER_BANDS];
    IpDst ipDstList[NEXUS_NUM_RECPUMPS];
    RecDst recDstList[NEXUS_NUM_RECPUMPS/2];
#endif /* DMS_CROSS_PLATFORMS */
    /* list of all available pid parsers */
    BKNI_MutexHandle parserBandMutex;
    /* list of all available streamer sources: one per parser band */
    BKNI_MutexHandle streamerSrcMutex;
    /* list of all available Ip destinations: one per rave context */
    BKNI_MutexHandle ipDstMutex; /* lock to synchronize access to the list of ip srcs */
    /* list of all available record destinations: one per rave context */
    BKNI_MutexHandle recDstMutex;
#ifdef NEXUS_HAS_VIDEO_ENCODER
    /* list of all available xcode destinations */
    BKNI_MutexHandle transcoderDstMutex;
    TranscoderDst transcoderDstList[NEXUS_NUM_VIDEO_ENCODERS];
#endif

#ifdef NEXUS_HAS_HDMI_INPUT
    BKNI_MutexHandle hdmiSrcMutex;
    HdmiSrc hdmiSrcList[NEXUS_NUM_HDMI_INPUTS];
#endif

    /* following fields are only used when IP Streamer is used in the standalone mode (i.e. w/o DLNA DMS) */
    /* listening socket's fd */
    int listeningFd;
} IpStreamerGlobalCtx;

/* per thread state info */
typedef struct IpStreamerCtx
{
    IpStreamerGlobalCtx *globalCtx;
    IpStreamerConfig cfg;
    IpStreamerSrc srcType;
    B_PlaybackIpSocketState socketState;
#ifndef DMS_CROSS_PLATFORMS
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_PidChannelHandle audioPidChannel;
    NEXUS_PidChannelHandle pcrPidChannel;
    NEXUS_PidChannelHandle patPidChannel;
    NEXUS_PidChannelHandle pmtPidChannel;
    NEXUS_PidChannelHandle allPassPidChannel;
#endif /* DMS_CROSS_PLATFORMS */
#ifdef STREAMER_CABLECARD_SUPPORT
    NEXUS_PidChannelHandle caPidChannel;
#endif
#if NEXUS_HAS_FRONTEND
    QamSrc *qamSrc;
    QamSrc *vsbSrc;
    SatSrc *satSrc;
    OfdmSrc *ofdmSrc;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendVsbSettings vsbSettings;
    NEXUS_FrontendOfdmSettings ofdmSettings;
#endif /* NEXUS_HAS_FRONTEND */
    IpSrc *ipSrc;   /* either ipSrc or qamSrc point is valid based on srcType */
    StreamerSrc *streamerSrc;
    FileSrc *fileSrc;
    IpDst *ipDst;
#ifdef NEXUS_HAS_HDMI_INPUT
    HdmiSrc *hdmiSrc;
#endif
    ParserBand *parserBandPtr;
    RecDst *recDst;
    uint64_t curBytesReceived;  /* # of bytes received from a live channel: either streamed on IP or recorded locally */
    int underflowWarningCount;  /* how many seconds we haven't received any new data in the streaming/recording buffer */

    bool ipStreamingInProgress; /* set when we are streaming a live channel to an IP client */
    bool fileStreamingInProgress; /* set when we are streaming a media file to an IP client */
    bool localRecordingInProgress; /* set when we are recording to local disk */
    bool overflow; /* set when we overflow happeneded */
    bool skipPsiAcquisition; /* if set, skip PSI acquisition, gets set when same channel is re-tuned */
#ifdef B_HAS_DTCP_IP
	void *AkeHandle;
	void *dtcpCtx;
#endif
    bool transcodeEnabled;
#ifdef NEXUS_HAS_VIDEO_ENCODER
    TranscoderDst *transcoderDst;
    NEXUS_PidChannelHandle transcodeVideoPidChannel;
    NEXUS_PidChannelHandle transcodeAudioPidChannel;
    NEXUS_PidChannelHandle transcodePcrPidChannel;
    NEXUS_PidChannelHandle transcodePatPidChannel;
    NEXUS_PidChannelHandle transcodePmtPidChannel;
    bool transcodingInProgress; /* set when we are transcoding a session */
#endif
#ifdef NEXUS_HAS_SECURITY
    NEXUS_KeySlotHandle pvrEncKeyHandle;
    NEXUS_KeySlotHandle pvrDecKeyHandle;
#endif
    NEXUS_PidChannelHandle pidChannelList[IP_STREAMER_MAX_PIDS_PER_PROGRAM];
}IpStreamerCtx;
#endif /* __IP_STREAMER_H__ */
