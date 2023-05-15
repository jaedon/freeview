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
 * $brcm_Workfile: ip_streamer_lib.h $
 * $brcm_Revision: 55 $
 * $brcm_Date: 11/9/12 3:22p $
 * 
 * Module Description:
 *  ip streamer interface include file
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/playback_ip/apps/ip_streamer_lib.h $
 * 
 * 55   11/9/12 3:22p ssood
 * SW7425-4169: allow apps to pass in configurable nexus heap handle
 * 
 * 54   9/13/12 2:26p aginesh
 * SW7435-353: remove tabs
 * 
 * 53   9/13/12 11:52a aginesh
 * SW7435-353: callback function declarations
 * 
 * 52   9/13/12 11:19a aginesh
 * SW7435-353: changes to get ip_streamer to work with trellis
 * 
 * 51   9/11/12 3:57p leisun
 * SWSECURITY-48: DTCP-IP change. Auto-generating test keys, consolidate
 *  test/production build, make common DRM default ON
 * 
 * 50   8/16/12 6:06p ssood
 * SW7425-3482: added support for allowing clients to specify the pids to
 *  demux in Sat/IP protocol + handle pilotTone setting
 * 
 * 49   7/28/12 3:24p ssood
 * SW7346-944: dont skip over programs with ca_pid set (as it seems to be
 *  set even for clear channels) + support cciMode & priority parms for
 *  OFDM tuning
 * 
 * 48   7/26/12 8:33p ssood
 * SW7346-944: add support to stream from OFDM-capable frontend
 * 
 * 47   7/19/12 7:31p ssood
 * SW7425-3482: updated sat/ip code to match the 94528 board: vert
 *  polarization on 1st 4528 & rest on 2nd + updated logic to calculate
 *  nexus freq based on downconverted & mixed settings
 * 
 * 46   7/13/12 12:11a ssood
 * SW7425-3482: extend ip_streamer to support BSkyB's Sat/Ip Protocol
 * 
 * 45   5/17/12 6:36p ssood
 * SW7425-3042: simplified build flags & code by using nexus multi process
 *  capabilities
 * 
 * 44   5/14/12 4:33p ssood
 * SW7425-3042: refactor ip_streamer into multiple sub files
 * 
 * 43   5/1/12 6:22p ssood
 * SW7346-805: enhance ip_streamer to use playback path for streaming out
 *  timeshifted live content
 * 
 * 42   3/12/12 4:41p gkamosa
 * SW7425-2543: Merge support for 3383 SMS platform
 * 
 * Centaurus_Phase_1.0/6   3/8/12 4:59p gkamosa
 * SW7425-2337: remove code to allow display to be turned off
 * 
 * 41   3/4/12 9:47a ssood
 * SW7425-2331: dont close pid channels for shared xcode sessions until
 *  the last session is closed + coverity fixes
 * 
 * 40   2/3/12 8:07p ssood
 * SW7425-2331: disable autoRewind for HLS playlist files
 * 
 * 39   2/2/12 6:57p ssood
 * SW7425-2331: Add media segmentation support for HLS Server
 * 
 * 38   1/24/12 11:30a ssood
 * SW7425-1331: insert encrypted (DTCP/IP) special data while streaming
 *  out media files
 * 
 * 37   12/29/11 12:20p alanlam
 * SW3128-62: Add support fo spectrum analyzer data retrieval.
 * 
 * 36   12/8/11 7:40p ssood
 * SW7231-516: enhance ip_streamer to stream out raaga encoded TS streams
 *  from live or file sources
 * 
 * 35   10/14/11 10:18a ssood
 * SW7425-889: initial check-in for enabling audio in the live xcoded
 *  streams
 * 
 * 34   10/7/11 6:10p ssood
 * SW7346-341: allow apps to pass-in the key slot pointer
 * 
 * 33   9/30/11 10:32p ssood
 * SW7346-341: assigned more friendly name for the PVR decryption related
 *  variables
 * 
 * 32   9/29/11 7:03p ssood
 * SW7346-312: Expose QAM Annex & Symbol Rate settings to DMS app
 * 
 * 31   9/9/11 9:26a micl
 * SW7425-1077: IBC SAT changes
 * 
 * 30   8/22/11 12:21p ssood
 * SW7425-1040: add logic to Skip PSI probing for FCC & provide end of
 *  streaming callback
 * 
 * 29   8/10/11 1:44p micl
 * SW7425-1075: Adding QAM statuses to AppLibs DMS
 * 
 * 28   8/5/11 3:14p ssood
 * SW7425-1040: add fast channel change support to IP Sessions
 * 
 * 27   7/14/11 12:01p ssood
 * SW7346-309: Add support for Streaming from Satellite Source
 * 
 * 26   6/30/11 12:15a ssood
 * SW7425-813: add support to transcode & stream session alongside main &
 *  pip decode
 * 
 * 25   6/20/11 11:13p ssood
 * SW7425-751: add configurable option for nav & nav files
 * 
 * 24   3/18/11 6:44p ssood
 * SW7420-1616: allow app to pass-in dma handle
 * 
 * 23   3/16/11 4:55p ssood
 * SW7420-1616: initial check-in for pvr encryption support
 * 
 * 22   1/2/11 5:26p ssood
 * SW7420-1257: added support to stream encoded frame using unicast for
 *  local client
 * 
 * 21   12/31/10 3:25p ssood
 * SW7425-59: add change to support clean stop/stop sequence w/ 7425
 *  xcoder + insert PAT/PMT into outgoing stream
 * 
 * 20   12/30/10 1:06p ssood
 * SW7425-59: surround all 7425 transcoder changes w/
 *  NEXUS_HAS_VIDEO_ENCODER compile flag
 * 
 * 19   12/30/10 12:55p ssood
 * SW7425-59: initial 7425 transcoder support on h/w platform (no PSI, no
 *  PCRs, no Audio in xcoded stream)
 * 
 * 18   12/28/10 12:28p ssood
 * SW7425-59: Add support for using HDMI-in as the source for
 *  transcoding+streaming out
 * 
 * 17   12/26/10 10:45p ssood
 * SW7425-59: Add support for 7425 transcoder
 * 
 * 16   11/23/10 11:55a ssood
 * SW7420-1257: initial check-in for rtp/udp streaming support in
 *  ip_streamer for Video Conferencing Demo
 * 
 * 15   8/12/10 11:24a ssood
 * SW7420-883: added support for streaming same transcoding session to
 *  multiple clients
 * 
 * 14   8/6/10 6:33p ssood
 * SW7420-883: allow only one transcoding session at a time
 * 
 * 13   8/5/10 7:32a ssood
 * SW7420-883: added support for allowing external apps to configure
 *  transcoding parameters into ip streamer library
 * 
 * 12   7/30/10 2:17p garetht
 * SW7420-919: Add CableCARD support to Ip_streamer
 * 
 * 11   7/27/10 2:08p ssood
 * SW7420-883: add transcoding support to IP Streamer (no audio
 *  transcoding yet)
 * 
 * 10   4/15/10 7:49p sgundime
 * SW7420-689: time seek range support is added.
 * 
 * 10   4/15/10 7:33p sgundime
 * SW7420-689: Time seek range support is added.
 * 
 * 9   1/27/10 10:18a ssood
 * SW7420-454: conditionally compile Live Streaming Code using
 *  LIVE_STREAMING_SUPPORT
 * 
 * 8   1/19/10 4:55p ssood
 * SW7420-454: convert verbose output to be available via msg modules
 *  flags
 * 
 * 7   1/14/10 6:41p ssood
 * SW7408-47: Add support to compile IP Streamer App on platforms w/ no
 *  frontend support
 * 
 * 6   12/23/09 1:48p ssood
 * SW7420-454: new IpStreamer_SessionStatus() API to indicate session
 *  state & recording buffer duration
 * 
 * 5   12/22/09 11:03a ssood
 * SW7420-454: add timeshift support for live streaming
 * 
 * 4   12/17/09 2:07p ssood
 * SW7420-454: Add option to just open IP Streamer & IP library contexts
 *  for Media Probe purposes
 * 
 * 3   12/7/09 6:19p ssood
 * SW7420-454: converted timeOffset to floating point to match w/ DLNA NPT
 *  format
 * 
 * 2   11/24/09 10:48a ssood
 * SW7420-454: add option to allow apps to specify a specific interface
 *  name to receive live IP content
 * 
 * 1   11/17/09 2:58p ssood
 * SW7420-454: Enhance IP Streamer to stream files from local disk
 * 
 * 
 ******************************************************************************/
#ifndef __IP_STREAMER_LIB_H__
#define __IP_STREAMER_LIB_H__

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#ifndef DMS_CROSS_PLATFORMS
#include "nexus_display.h"
#endif /* DMS_CROSS_PLATFORMS */


/**
Summary:
Callback functions to NEXUS server process required in multiprocess environment
**/
typedef int (*B_IpStreamer_RequestDecoder) (void);

typedef void (*B_IpStreamer_ReleaseDecoder) (int decoderIndex);

typedef bool (*B_IpStreamer_ConnectAudioDummy) (NEXUS_AudioDecoderHandle decoder, int audioDummyIndex);

typedef void (*B_IpStreamer_DisconnectAudioDummy) (int audioDummyIndex);



typedef enum {
    IpStreamerSrc_eIp, /* For streaming out content coming from IP frontend */
    IpStreamerSrc_eQam, /* For streaming out content coming from QAM frontend */
    IpStreamerSrc_eVsb, /* For streaming out content coming from VSB frontend (off-air) */
    IpStreamerSrc_eStreamer, /* For streaming out content coming from Streamer input */
    IpStreamerSrc_eFile, /* For streaming out pre-recorded content from local disk */
    IpStreamerSrc_eSat, /* For streaming out content coming from Satellite frontend */
    IpStreamerSrc_eHdmi, /* For streaming out encoded content coming from HDMI in (BlueRay player) */
    IpStreamerSrc_eOfdm, /* For streaming out encoded content coming from OFDM-capable frontends */
    IpStreamerSrc_eMax /* Max allowed enum */
}IpStreamerSrc;

typedef struct IpStreamerStreamingOutCfg {
    char *streamingIpAddress; /* remote IP address to stream to */
    int streamingPort;  /* remote Port # to stream to */
    B_PlaybackIpProtocol streamingProtocol; /* Protocol: UDP/RTP */
    char *url;  /* url (file name) for RTP/UDP streaming */
} IpStreamerStreamingOutCfg;

typedef struct IpStreamerGlobalCfg{
    bool multiProcessEnv; /*set to true when running with trellis*/
    int dtcpAkePort;  /* dtcp-ip port to receive AKE requests on */
    int dtcpKeyFormat;  /* DTCP-IP key format*/
    bool ckc_check;     /* Enable content key confirmation for sink device */
    bool slaveMode; /* set for VMS, allows sharing of Nexus handles between two independent Apps */
    bool printStats;
    char *serverIpAddress;
    char interfaceName[16];
    char timeshiftDirPath[32]; /* directory where timeshifted files are stored */
    int timeshiftBufferInterval; /* how many seconds worth of stream should be cached in the timeshift buffer */
    int maxBitRate; /* max bitrate for the live sessions */
#ifdef NEXUS_HAS_DMA
    NEXUS_DmaHandle dmaHandle; /* Nexus M2M DMA handle */
#endif
    NEXUS_HeapHandle heapHandle; /* nexus heap Handle, can be null if app doesn't have a preference */

    /* following options need to be set only when IP Streamer is run in standalone mode (i.e. outside of DLNA DMS) */
    int listeningPort;  /* port to receive HTTP requests on */
    bool accelSocket; /* once accelerated sockets are supported on 7420, this option can be enabled */
    char rootDir[64]; /* root directory */
    int numStreamingSessions; /* max # of streaming threads to start */
    int disableFrontend; /* if set, frontends (qam/vsb/ip) are not used for live streaming */
    IpStreamerStreamingOutCfg streamingCfg;
    bool freeDmaHandle;

    /*call back function to nexus server process*/
    B_IpStreamer_RequestDecoder requestDecoder;
    B_IpStreamer_ReleaseDecoder releaseDecoder;
    B_IpStreamer_ConnectAudioDummy connectAudioDummy;
    B_IpStreamer_DisconnectAudioDummy disconnectAudioDummy;

} IpStreamerGlobalCfg;

#ifdef NEXUS_HAS_VIDEO_ENCODER
typedef struct IpStreamerTranscodeConfig {
    int transportBitrate; /* desired transport bitrate of transcoded stream */
    NEXUS_TransportType transportType; /* Transport type of the transcoded stream */
    bool outVideo;  /* set if Video in enabled in transcoded stream */
    bool outAudio;  /* set if Audio is enabled in transcoded stream */
    int outVideoPid;
    int outPcrPid;
    int outAudioPid;
    NEXUS_VideoCodec outVideoCodec; /* video codec of transcoded stream */
    NEXUS_DisplayAspectRatio outAspectRatio;
    NEXUS_VideoFrameRate outFrameRate;
    unsigned outWidth;
    unsigned outHeight;
    NEXUS_AudioCodec outAudioCodec; /* audio codec of transcoded stream */
    int outPatPid;
    int outPmtPid;
    bool outInterlaced;
}IpStreamerTranscodeConfig;
#endif

/* App specific Header that gets inserted before media is streamed out */
typedef struct {
    bool valid; /* set if appHeader is valid */
    unsigned length; /* length of app header bytes: should be mod16 aligned if it needs to be encrypted */
    uint8_t data[192]; /* app header */
} IpStreamerAppHeader;

/* per session configuration */
typedef struct IpStreamerConfig{
    IpStreamerSrc srcType;

    bool mediaProbeOnly;     /* set this flag if the ip streaming context is being used for determining media probe info only */
    bool skipPsiAcquisition; /* set this flag if app already has PSI information for this file/channel */
    B_PlaybackIp_EventCallback eventCallback; /* callback function to receive any asynchronous events from the IP library */
    void *appCtx;
#ifdef NEXUS_HAS_FRONTEND
    /* QAM Src related settings */
    NEXUS_FrontendQamMode qamMode;  /* qam modulation mode */
    NEXUS_FrontendQamAnnex qamAnnex;
    NEXUS_FrontendSatelliteMode satMode;  /* sat mode */
    int diseqcVoltage;
    int symbolRate;
    bool toneEnabled;
    bool verticalPolarization;
    int fec;
    /* VSB Src related settings */
    NEXUS_FrontendVsbMode vsbMode;
    /* OFDM related config */
    int bandwidth;
    NEXUS_FrontendOfdmMode ofdmMode;
    NEXUS_FrontendOfdmCciMode cciMode;
    NEXUS_FrontendOfdmPriority ofdmPriority;
    bool terrestrial;
#endif
    uint32_t frequency; /* frequency in Mhz (e.g. 333) */
    int subChannel; /* sub channel number: starts from 1 */
#ifdef STREAMER_CABLECARD_SUPPORT
    uint32_t sourceId;
#endif
    /* IP Src related settings */
    int iphVersion;
    char srcIpAddress[256]; /* IP Address of Receiving Host */
    int srcPort; /* Port # */
    char interfaceName[16]; /* Interface to send multicast join requests on */
    B_PlaybackIpProtocol srcProtocol; /* Protocol: UDP/RTP */
    bool accelSocket; /* once accelerated sockets are supported on 7420, this option can be enabled */

    B_PlaybackIpSecurityProtocol security;  /* which security protocol to use */

    /* File Source related settings */
    off_t beginFileOffset;
    off_t endFileOffset;
    int playSpeed;  /* speed at which to stream the file */
    double beginTimeOffset; /* stream file starting from these many seconds */
    double endTimeOffset; 

    /* Record Destination related settings */
    bool recDstEnabled; /* if set, local recording is enabled */

    /* Note: local data is being recorded in clear */
    char fileName[128];  /* name of the file where av data should be recorded to or streamed from */
    char indexFileName[128]; /* name of the file where av data index should be written to or used for streaming */
    char mediaInfoFilesDir[128];  /* directory name where the info & nav files should be created */

    /* IP Destination related settings */
    bool ipDstEnabled; /* if set, live channel is streamed over the network */
    int streamingFd; /* socket to which av content needs to be streamed to */
    int streamingFdLocal; /* socket fd on which to stream data for local client */
    bool useLiveIpMode; /* true: Push mode (locks to incoming PCR); false: Pull Mode (receiver controls the sender) */
    bool encryptionEnabled; /* if set, streaming data is encrypted as per the Dst (DTCP/IP for Ip Dst & Plain AES for Local Rec Dst) */
    bool pvrEncryptionEnabled; /* if set, encrypt stream before writing to timeshift fifo & decrypt it before DTCP/IP */
    bool pvrDecryptionEnabled; /* if set, encrypt stream before writing to timeshift fifo & decrypt it before DTCP/IP */
    void *pvrDecKeyHandle;   /* set if PvrDecEnabled field is set in the URL */
    int emiValue; /* EMI value for DTCP/IP headers */
    bool transcodeEnabled; /* if set, transcoding is enabled */
    bool usePlaybackForStreamingFiles;
    bool enableTimeshifting; /* enables timeshifting of live channels */
    bool enableTimestamps; /* enables 4 byte binary timestamp in the recording of the live streams */
#ifdef NEXUS_HAS_VIDEO_ENCODER
    IpStreamerTranscodeConfig transcode;
#endif
#ifdef NEXUS_HAS_HDMI_INPUT
    int hdmiInputId; /* index of HDMI input to use */
#endif

    IpStreamerStreamingOutCfg streamingCfg; /* contains configuration for streaming out using RTP/UDP protocols */
    IpStreamerAppHeader appHeader;     /* app specific binary data */
    bool hlsSession;         /* set if this is a HLS session */
    int hlsSegmentSize;      /* size of a hls segment in bytes */
    bool autoRewind;        /* if set, automatically rewind file when eof is reached */

    bool satIpProtocolSession; /* set to true if this session follows the ASTRA's Sat/IP Protocol Spec */
#define IP_STREAMER_MAX_PIDS_PER_PROGRAM 16 /* should be enough for SPTS program */
    uint16_t pidList[IP_STREAMER_MAX_PIDS_PER_PROGRAM]; /* list of pids: sat/ip client can provide the list of pids it wants server to demux & stream out */
    int pidListCount; /* count of sat/ip client provided list of pids to demux */
    bool enableAllpass; /* sat/ip client can even ask server to stream the whole mpts stream out to the client */
    bool noDemuxOutput; /* sat/ip client can ask server to just setup the pipe and not demux & send any streams to the client */
    bool pilotToneSpecified; /* true if client specified a value for the pilotTone */
    bool pilotTone; /* client can set this to be on or off */

    /* following options need to be set only when IP Streamer is run in standalone mode (i.e. outside of DLNA DMS) */
    bool headRequest; /* set when client sends HTTP HEAD request */
} IpStreamerConfig;

/* Initializes IP Streamers' global context for the given configuration */
void * B_IpStreamer_Init(IpStreamerGlobalCfg *ipStreamerGlobalCfg);
void B_IpStreamer_UnInit(void *ipStreamerGlobalCtx);

typedef struct IpStreamerOpenSettings {
    int streamingFd;         /* socket file descriptor using which live/file content should be streamed, not used for recording only or media probing case */
    int streamingFdLocal; /* socket fd on which to stream data for local client */
    char *requestUri;        /* URI associated with this streaming context: identifies live vs file streaming, recording on/off */
    bool mediaProbeOnly;     /* set this flag if the ip streaming context is being used for determining media probe info only */
    bool skipPsiAcquisition; /* set this flag if app already has PSI information for this file/channel */
    B_PlaybackIp_EventCallback eventCallback; /* callback function to receive any asynchronous events from the IP library */
    void *appCtx;
    void *pvrDecKeyHandle;   /* set if PvrDecEnabled field is set in the URL */
    bool hlsSession;         /* set if this is a HLS session */
    int hlsSegmentSize;      /* size of a hls segment in bytes */
    IpStreamerAppHeader appHeader;     /* app specific binary data */
    bool autoRewind;        /* if set, automatically rewind file when eof is reached */
}IpStreamerOpenSettings;

/* Opens per session context for a give session configuration */
void *B_IpStreamer_SessionOpen(void *ipStreamerGlobalCtx, IpStreamerOpenSettings *openSettings);
void B_IpStreamer_SessionClose(void *ipStreamerCtx);

/* returns the PSI info associated with a live channel in psiOut */
int B_IpStreamer_SessionAcquirePsiInfo(void *ipStreamerCtx, B_PlaybackIpPsiInfo *psiOut);

/* Settings that can be updated when live streaming start is already called */
typedef struct IpStreamerSettings
{
    bool streamingEnabled; /* flag to indicate to live streaming thread that streaming has now been enabled (Fast Channel Change) */
    bool resumeStreaming; /* flag to indicate to live streaming thread to resume streaming for the new HLS segment */
    int streamingFd; /* socket fd on which to stream data on */
    bool xcodeModifyBitrate; /* set when app wants to dynamically modify the bitrate of current encoding session (w/o restarting it) */
    unsigned int xcodeBitrate; /* bitrate */
    bool hlsSession;         /* set if this is a HLS session */
    int hlsSegmentSize;      /* size of a hls segment in bytes */
} IpStreamerSettings;
/* Opens per session context for a give session configuration */
int B_IpStreamer_SessionSetSettings(void *ipStreamerCtx, IpStreamerSettings *sessionSettings);

/* Opens per session context for a give session configuration */
int B_IpStreamer_SessionStart(void *ipStreamerCtx, B_PlaybackIpPsiInfo *psi);
void B_IpStreamer_SessionStop(void *ipStreamerCtx);

typedef struct IpStreamerSessionStatus {
    bool active;       /* set as long as session is active (no underflow from source, client still alive & receiving, no recording error, etc.*/ 
    /* TODO: add more status fields like: socket state, bytes streamed, bytes recorded, etc. */
    int recordingDuration; /* duration in seconds of the session being recorded */
}IpStreamerSessionStatus;

/* returns true if Streaming session is successfully ongoing, false otherwise where upon app should stop & close streaming session */
void B_IpStreamer_SessionStatus(void *ipStreamerCtx, IpStreamerSessionStatus *status);

/* frontend statuses that can be returned */
typedef enum IpStreamer_FrontendMode
{
    /* satellite modes */
    IpStreamer_FrontendMode_eDvb,
    IpStreamer_FrontendMode_eDss,
    IpStreamer_FrontendMode_eDcii,
    IpStreamer_FrontendMode_eQpskTurbo,
    IpStreamer_FrontendMode_eTurboQpsk=IpStreamer_FrontendMode_eQpskTurbo,
    IpStreamer_FrontendMode_e8pskTurbo,
    IpStreamer_FrontendMode_eTurbo8psk=IpStreamer_FrontendMode_e8pskTurbo,
    IpStreamer_FrontendMode_eTurbo,
    IpStreamer_FrontendMode_eQpskLdpc,
    IpStreamer_FrontendMode_e8pskLdpc,
    IpStreamer_FrontendMode_eLdpc,
    IpStreamer_FrontendMode_eBlindAcquisition,
    
    /* cable modes */
    IpStreamer_FrontendMode_e16,
    IpStreamer_FrontendMode_e32,
    IpStreamer_FrontendMode_e64,
    IpStreamer_FrontendMode_e128,
    IpStreamer_FrontendMode_e256,
    IpStreamer_FrontendMode_e512,
    IpStreamer_FrontendMode_e1024,
    IpStreamer_FrontendMode_e2048,
    IpStreamer_FrontendMode_e4096,
    IpStreamer_FrontendMode_eAuto_64_256, /* Automatically scan both QAM-64 and QAM-256.
                                           Not available on all chipsets. */
    /* terrestrial modes */
    IpStreamer_FrontendMode_eDvbt,
    IpStreamer_FrontendMode_eDvbt2,
    IpStreamer_FrontendMode_eIsdbt,

    IpStreamer_FrontendMode_eMax
} IpStreamer_FrontendMode;

typedef enum IpStreamer_FrontendAnnex
{
    IpStreamer_FrontendQamAnnex_eA,
    IpStreamer_FrontendQamAnnex_eB,
    IpStreamer_FrontendQamAnnex_eMax
} IpStreamer_FrontendAnnex;

typedef enum IpStreamer_FrontendInversion
{
    IpStreamer_FrontendInversion_eScan,
    IpStreamer_FrontendInversion_eNormal,
    IpStreamer_FrontendInversion_eI,
    IpStreamer_FrontendInversion_eQ,
    IpStreamer_FrontendInversion_eMax
} IpStreamer_FrontendInversion;

typedef struct IpStreamerSoftDecision {
    int i;
    int q;
}IpStreamerSoftDecision;

/*returns 0 if successful, else return 1 */
int B_IpStreamer_GetFrontendSoftDecision(void * dlnaGlobalCtx, int frontendNo, IpStreamerSoftDecision *softDecision, int length);

typedef struct IpStreamerStatus {
    IpStreamer_FrontendMode mode;
    IpStreamer_FrontendInversion spectralInversion;
    
    unsigned codeRateNumerator; /* Code rate detected */
    unsigned codeRateDenominator;
    unsigned frequency;         /* actual tuner frequency */

    bool tunerLocked;           /* true if the tuner is locked */
    bool demodLocked;           /* true if the demodulator is locked */
    bool bertLocked;            /* true if the BER tester is locked.  If so, see berEstimate. */

    unsigned channel;           /* Channel number */
    unsigned symbolRate;        /* In baud */
    int symbolRateError;        /* In baud */

    int carrierOffset;          /* In Hz */
    int carrierError;           /* In Hz */
    unsigned sampleClock;       /* In Hz */
    unsigned outputBitRate;     /* Output bit rate in bps */

    unsigned ifAgcLevel;        /* IF AGC level in units of 1/10 percent */
    unsigned rfAgcLevel;        /* tuner AGC level in units of 1/10 percent */
    unsigned intAgcLevel;       /* Internal AGC level in units of 1/10 percent */
    unsigned snrEstimate;       /* SNR in 1/100 dB */
    unsigned berEstimate;       /* Bit error rate as log10 of 0.0-1.0 range.
                                    1.0  => 100% => 0
                                    0.1  => 10%  => -1
                                    0.01 => 1%   => -2
                                    0    => 0%   => 1 (special value for NONE)
                                    If bertLocked == false, it's set to 1. */

    unsigned fecPhase;          /* 0, 90, 180, 270 */
    unsigned fecCorrected;      /* cumulative block correctable errors */
    unsigned fecUncorrected;    /* cumulative block uncorrectable errors */
    unsigned fecClean;          /* cumulative clean block count */
    unsigned bitErrCorrected;   /* cumulative bit correctable errors */
    unsigned reacquireCount;    /* cumulative reacquisition count */
    unsigned berErrors;         /* BER error count - valid if bertLocked is true */
    unsigned preViterbiErrorCount;    /* accumulated pre-Viterbi error count */
    unsigned mpegErrors;        /* mpeg frame error count */
    unsigned mpegCount;         /* total mpeg frame count */
    unsigned ifAgc;             /* if agc value from hw unmodified */
    unsigned rfAgc;             /* rf agc value from hw unmodified */
    unsigned agf;               /* AGF integrator value */
    unsigned timeElapsed;       /* time elapsed in milliseconds since the last call to NEXUS_Frontend_ResetStatus.
                                   the elapsed time is measured at the same time that the rest of the values in NEXUS_FrontendSatelliteStatus are captured. */

    float power;

    unsigned chipId;
    unsigned chipVersion;
    unsigned bondingOption;
    unsigned apMicrocodeVersion;
    unsigned hostConfigurationVersion;

    /* cable specifics */
    bool receiverLock;
    bool fecLock;
    int dsChannelPower;
    IpStreamer_FrontendAnnex annex;
}IpStreamerStatus;

int B_IpStreamer_GetFrontendStatus(void * dlnaGlobalCtx, int frontendNo, IpStreamerSrc *src, IpStreamerStatus *status);

typedef struct IpStreamerSpecAParams
{
    int channel;            /* frontendNo */
    uint32_t fStart;
    uint32_t fStop;
    uint32_t fftSize;
    uint32_t numOfAvgBins;
    uint32_t numOfSamples;
    uint32_t *data;         /* pointer to the array where to store the data */
} IpStreamerSpecAParams;

int B_IpStreamer_SpecAOpen(void * dlnaGlobalCtx, int frontendNo, int* selectedFrontendNo);
int B_IpStreamer_SpecAClose(void * dlnaGlobalCtx, int frontendNo);
int B_IpStreamer_GetFrontendSpecAData(void * dlnaGlobalCtx, int frontendNo, IpStreamerSpecAParams *specAParams);

#ifdef NEXUS_HAS_VIDEO_ENCODER
bool B_IpStreamer_InsertPatPmtTables(void *ipStreamerCtx);
#endif

#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
extern NEXUS_KeySlotHandle _createKeyHandle(NEXUS_SecurityOperation operationType);
#endif
extern int getEnvVariableValue(char *pName, unsigned long defaultValue);



#endif /* __IP_STREAMER_LIB_H__ */
