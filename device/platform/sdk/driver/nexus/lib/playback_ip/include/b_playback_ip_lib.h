/***************************************************************************
*     (c)2008-2012 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
* 
* $brcm_Workfile: b_playback_ip_lib.h $
* $brcm_Revision: 104 $
* $brcm_Date: 11/9/12 3:22p $
*
* Description: header file for Playback IP App Lib
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/include/b_playback_ip_lib.h $
* 
* 104   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 103   9/13/12 11:51a aginesh
* SW7435-353: move declarations to ip_streamer_lib.h
* 
* 102   9/13/12 11:16a aginesh
* SW7435-353: changes to get ip_streamer to work with trellis
* 
* 101   8/3/12 2:52p ssood
* SW7425-3678: extend ip lib & ip streamer to support streaming out live,
*  file, encoded content over RTP/UDP
* 
* 100   7/17/12 4:41p gskerl
* SW7346-853: Merged from branch SW7346-853 to main.
* 
* SW7346-853/1   6/27/12 4:42p gskerl
* SW7346-853: Added mpegDashSessionEnabled field to the
*  B_PlaybackIpPsiInfo struct.
* 
* 99   6/8/12 7:11p ssood
* SW7425-3201: Add support for new DLNA PlaySpeed trickmode options in
*  playback ip library
* 
* 98   5/29/12 11:06a ssood
* SW7425-3122: add an option to disable nav file generation during media
*  probe time
* 
* 97   5/18/12 4:56p ssood
* SW7231-830: add support for HTTP proxy
* 
* 96   5/17/12 6:36p ssood
* SW7425-3042: simplified build flags & code by using nexus multi process
*  capabilities
* 
* 95   5/1/12 6:22p ssood
* SW7346-805: enhance ip_streamer to use playback path for streaming out
*  timeshifted live content
* 
* 94   3/1/12 7:01p ssood
* SW7425-2331: add logic to handle timeout events during HLS session
*  (while waiting for client to re-connect for the next segment)
* 
* 93   2/9/12 3:31p ssood
* SW7425-2319: Add support for IP Tuner Lock/Unlock mechanism
* 
* 92   2/2/12 8:11p ssood
* SW7425-1331: added support in live streaming modules for inserting
*  encrypted app specific header
* 
* 91   2/2/12 6:57p ssood
* SW7425-2331: Add media segmentation support for HLS Server
* 
* 90   1/24/12 11:30a ssood
* SW7425-1331: insert encrypted (DTCP/IP) special data while streaming
*  out media files
* 
* 89   1/17/12 5:40p ssood
* SW7231-603: limit media probe time
* 
* 88   12/27/11 10:07p ssood
* SW7425-2061: add simple decoder support to ip playback
* 
* 87   9/29/11 6:00p ssood
* SW7346-341: encapsulate nexus dependent changes so that non-nexus
*  platforms continue to build
* 
* 86   9/26/11 10:22a ssood
* SW7346-341: Add PVR Decryption support for file streaming
* 
* 85   8/5/11 3:13p ssood
* SW7425-1040: add fast channel change support to IP Sessions
* 
* 84   6/20/11 11:14p ssood
* SW7425-751: add configurable option for nav & nav files
* 
* 83   6/6/11 1:36a ssood
* SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
*  eventCallback support for end of stream playback
* 
* 82   5/26/11 11:31p ssood
* SW7231-166: dont create Nexus File Handle for cases where Nexus
*  playpump will get used
* 
* 81   5/23/11 3:44p ssood
* SW7231-166: Extend Playback IP trickmodes to be compatible with latest
*  DLNA Guidelines
* 
* 80   5/19/11 4:28p ssood
* SW7420-1774: Add support to decode SVC/MVC videos
* 
* 79   3/28/11 11:02a ssood
* SW7420-502: clarified the usage of the additionalHeader fields (needs
*  to be CRNL terminated)
* 
* 78   3/16/11 5:39p ssood
* SW7420-1616: initial check-in for pvr encryption support
* 
* 77   2/23/11 11:48a ssood
* SW7420-502: clock recovery support for RVU client
* 
* 76   1/2/11 5:26p ssood
* SW7420-1257: added support to stream encoded frame using unicast for
*  local client
* 
* 75   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 74   11/24/10 2:58p ssood
* SW7420-1257: Video Conferencing Demo: add support for receiving input
*  camera input and stream it out
* 
* 73   11/23/10 11:55a ssood
* SW7420-1257: initial check-in for rtp/udp streaming support in
*  ip_streamer for Video Conferencing Demo
* 
* 72   10/29/10 9:05a ssood
* SW7420-1217: basic support to play both variant & non-variant bounded
*  playlists containing clear content
* 
* 71   9/27/10 3:36p junyel
* SW7420-1115: cross-platform code for DMS
* 
* 70   9/17/10 8:21a ssood
* SWGERRARD-547: support to expose multiple multiple programs and tracks
*  within a multi-program stream
* 
* 69   7/30/10 2:16p garetht
* SW7420-919 : Add CableCARD support to Ip_streamer
* 
* 68   6/16/10 5:05p ssood
* SW7420-716: initial RVU trickmode related check-ins
* 
* 67   4/15/10 7:48p sgundime
* SW7420-689: time seek range support is added.
* 
* 67   4/15/10 7:33p sgundime
* SW7420-689: Time seek range support is added.
* 
* 66   4/2/10 4:18p ssood
* SW7420-561: add support to handle large URLs (HTML5 URLs can be over
*  256bytes)
* 
* 65   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/3   2/18/10 12:43a ssood
* SW7420-561: added checks to validate IP state during IP API calls
* 
* SW7420-561/2   2/16/10 10:42p ssood
* SW7420-561: Changes to support basic RTSP w/o trickmodes
* 
* SW7420-561/1   2/16/10 11:29a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 64   1/13/10 2:51p ssood
* SW7420-454: expose additional audio & video related PSI info
* 
* 63   12/22/09 11:03a ssood
* SW7420-454: add timeshift support for live streaming
* 
* 62   12/17/09 2:07p ssood
* SW7420-454: Add option to just open IP Streamer & IP library contexts
*  for Media Probe purposes
* 
* 61   12/8/09 1:48p ssood
* SW7420-454: make file streaming settings as const
* 
* 60   12/8/09 10:35a ssood
* SW7420-454: extended file streaming open settings to allow app to set
*  play speed & time seek range
* 
* 59   11/24/09 10:48a ssood
* SW7420-454: add option to allow apps to specify a specific interface
*  name to receive live IP content
* 
* 58   11/17/09 2:58p ssood
* SW7420-454: Enhance IP Streamer to stream files from local disk
* 
* 57   10/9/09 12:29p ssood
* SW7420-340: add support to play live streams on non-Broadcom clients
* 
* 56   10/6/09 12:18p ssood
* SW7420-340: Add DTCP/IP support to ip_streamer & IP Applib
* 
* 55   10/1/09 11:00a jjordan
* SW7405-3107: add IP recv timeout status to Settop API
* 
* 55   10/1/09 10:59a jjordan
* SW7405-3107: add IP recv timeout status to Settop API
* 
* 54   9/21/09 12:09p ssood
* SW7405-2559: Adding LPCM playback support
* 
* 53   9/16/09 10:29a ssood
* SW7420-340: changed liveStreaming to LiveStreaming as it will
*  eventually also include streaming from disk
* 
* 52   9/8/09 4:34p ssood
* SW7420-340: added support to retrieve the live channel parameters from
*  HTTP Get request
* 
* 51   9/7/09 4:44p ssood
* SW7420-340: modified live streaming api to differentiate it w/ network
*  recording
* 
* 50   9/1/09 7:34p ssood
* SW7420-340: initial check-in to support streaming of Live IP data
* 
* 49   9/1/09 2:15p lwhite
* SW7405-2176: Conditionally call SSL_library_init
* 
* 48   8/27/09 2:00p ssood
* SW7405-1689: only do detailed payload probing is requested by app
*  (otherwise probe is taking over 5 secs)
* 
* 47   8/6/09 3:38p ssood
* PR48146: add HTTP redirect support
* 
* 46   7/29/09 6:14p ssood
* PR43746: extend IP trick modes to support Disconnect And Seek based
*  paused operations (DLNA requirement)
* 
* 45   7/24/09 12:01p ssood
* PR56519: Allow apps to provide additional HTTP headers fields for
*  outgoing GET requests
* 
* 44   7/21/09 4:18p ssood
* PR43746: Further work for Server based HTTP trick mode support
* 
* 43   7/16/09 1:01p ssood
* PR43746: Add Server based HTTP trick mode support
* 
* 43   7/16/09 12:59p ssood
* PR43746: Add Server based HTTP trick mode support
* 
* 43   7/16/09 12:59p ssood
* PR43746: Add Server based HTTP trick mode support
* 
* 42   7/13/09 11:31a leisun
* PR 55693: Re-design interfaces, and other cleanups
* 
* 41   7/6/09 3:55p ssood
* PR56519: Add a flag to allow apps to specify disabling of the Range
*  Header
* 
* 40   7/1/09 6:30p ssood
* PR53773: handle case where DTCP/IP library init may fail
* 
* 39   6/25/09 3:25p ssood
* PR53773: Allow app to specify Nexus M2M handle if available
* 
* 38   6/25/09 2:30a ssood
* PR55417: added option to allocate HTTP precharge buffer either at init
*  or run time
* 
* 37   6/23/09 6:21p ssood
* PR53773: Adding DTCP/IP support
* 
* 36   6/18/09 10:24a ssood
* PR55417: provide http return code code via the GetStatus to app
* 
* 35   6/15/09 11:37p ssood
* PR55417: fixing rewind related bug
* 
* 35   6/15/09 11:31p ssood
* PR55417: fixing rewind related bug
* 
* 34   6/10/09 8:29p lwhite
* PR53773: Add RadEa time/byte offset
* 
* 33   6/5/09 1:35p ssood
* PR55626: DLNA guidelines mandate client to only maintain no more than 1
*  connection to server, we had one connx per data cache, changed that to
*  closing/opening 1 connx
* 
* 32   5/28/09 4:54p ssood
* PR55325: allow app to specify n/w timeout & buffer cache size
* 
* 31   5/28/09 11:38a ssood
* PR55325: added additional states to IP Applib to provide flexibility in
*  the order of API calls
* 
* 30   5/25/09 11:30p ssood
* PR55325: Add support for pre-charging network buffer for HTTP playback
* 
* 29   5/15/09 1:55p ssood
* PR53773: adding support for skipping Media probe: needed for RAD/EA
*  playback where app apriori have media info
* 
* 28   5/14/09 3:29p ssood
* PR53773: Adil's implementation of RAD/EA: updated RAD/EA init func
*  prototype
* 
* 27   5/13/09 10:39a ssood
* PR53773: created utitlity functions for HTTP parsing so that RAD/EA
*  module can use it as well
* 
* 26   5/1/09 4:39p ssood
* PR53773: adding support for WMA playback over HTTP
* 
* 25   5/1/09 12:00p ssood
* PR53773: RAD/EA specific changes: EA data is in not yet SSL encrypted
* 
* 24   4/30/09 11:47a lwhite
* PR53373: Client authentication added
* 
* 23   4/21/09 3:32p ssood
* PR53773: handle cases where some security protocols dont encrypt HTTP
*  response header
* 
* 22   4/21/09 10:57a lwhite
* PR53373: Add SslInit structure
* 
* 21   4/17/09 2:57p lwhite
* PR53373: Cleanup ssl init
* 
* 20   4/16/09 5:19p lwhite
* PR53373: Clone session support
* 
* 19   4/6/09 5:17p ssood
* PR53773 : adding RAD/ES specific initial security configuration
* 
* 18   4/6/09 9:14a ssood
* PR53773 : expose media duration to the applib caller
* 
* 17   4/2/09 11:36p ssood
* PR53773 : changes to support security framework for SSL, DTCP-IP,
*  RAD/ES protocols
* 
* 16   3/1/09 10:38a ssood
* PR50311: added support for HTTP playback of content w/ extra 4 byte
*  DLNA timestamps
* 
* 15   2/3/09 3:16p jjordan
* PR49937: add TTS throttle timebase setting
* 
* 14   11/9/08 10:52a ssood
* PR48133: Increase URL length to 256 to match the DLNA spec
* 
* 13   11/7/08 2:58p jrubio
* PR48782: add network record
* 
* 12   10/31/08 12:30p jjordan
* PR47230: Add TTS Pacing
* 
* 11   10/13/08 5:43p ssood
* PR47521: improved support for low-bit rate streams + handle unmarked
*  discontinuity condition
* 
* 10   8/11/08 10:13a ssood
* PR45431: Add HTTP playback support for ASF content
* 
* 9   8/5/08 7:59a ssood
* PR45400: make code compilable with -std=c89 for maximum compatibility
* 
* PR45400/1   8/5/08 12:44p dyzhang
* PR45400: kylin should compile with -std=c89 for maximum compatibility
* 
* 8   7/28/08 3:28p ssood
* PR42739: further changes for supporting example app for IP Applib
* 
* 7   7/21/08 4:19p ssood
* PR42739: Moving LiveMedia wrapper functionality part of IP Applib
* 
* 6   7/21/08 4:06p ssood
* PR42739: code changes to simplify ip app lib interface
* 
* 5   7/14/08 4:22p lwhite
* PR42739: Added RTCP support
* 
* 4   7/9/08 9:53p ssood
* PR42739: added support for high jitter + receiving variable size UDP
*  packets
* 
* 3   6/13/08 7:28p ssood
* PR42739: Adding support for HTTP & RTSP playback
* 
* 2   6/12/08 8:30a ssood
* PR42739: Enabling RTP support using LiveMedia library
* 
* 1   6/11/08 6:21p ssood
* PR42739: adding IP Applib files
* 
***************************************************************************/
#ifndef B_PLAYBACK_IP_LIB_H__
#define B_PLAYBACK_IP_LIB_H__

#include "bstd.h"
#include "nexus_types.h"
#include "nexus_file_fifo.h"
#ifndef DMS_CROSS_PLATFORMS
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#include "nexus_playback.h"
#include "nexus_video_decoder_types.h"
#include "nexus_recpump.h"
#ifdef NEXUS_HAS_DMA
#include "nexus_dma.h"
#endif
#ifdef NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif

#endif /* DMS_CROSS_PLATFORMS */
#ifndef _WIN32_WCE 

#ifdef __vxworks
  #include <sysLib.h>
  #include <string.h>
  #include <stdlib.h>
  #include <logLib.h>
  #include <sys/times.h>
  #include <selectLib.h>
  #include <taskLib.h>
  #include <semLib.h>
  #include <sys/ioctl.h>
  #include <hostLib.h>
#else
  #include <memory.h>
  #include <pthread.h>
  #include <sys/time.h>
#endif

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#else /* _WIN32_WCE */
#include <windows.h>
    typedef HANDLE pthread_t;
    #define close(x) closesocket(x)
    unsigned long errno = 0;
    #define EINTR -1
#endif /* _WIN32_WCE */

/**************************************************************************
 * This IP Applib module provides IP STB playback functions including:
 *
 *     - opening/closing of IP sockets 
 *     - playback thread create/destroy (which reads incoming data from socket & feeds to Playpump)
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Generic Error Codes

Description:
These error codes will match those returned by nexus (NEXUS_Error) and 
magnum (BERR_Code).  These may be used throughout application libraries
for consistency. Playback IP Applib specific Error codes follow the standard ones.
***************************************************************************/
typedef unsigned B_PlaybackIpError;
#ifdef DMS_CROSS_PLATFORMS
typedef unsigned int NEXUS_PlaybackPosition;
#endif

/**
Summary:
Standard Nexus error codes.
**/
#define B_ERROR_SUCCESS              0  /* success (always zero) */
#define B_ERROR_NOT_INITIALIZED      1  /* parameter not initialized */
#define B_ERROR_INVALID_PARAMETER    2  /* parameter is invalid */
#define B_ERROR_OUT_OF_MEMORY        3  /* out of heap memory */
#define B_ERROR_TIMEOUT              5  /* reached timeout limit */
#define B_ERROR_OS_ERROR             6  /* generic OS error */
#define B_ERROR_LEAKED_RESOURCE      7  /* resource being freed has attached resources that haven't been freed */
#define B_ERROR_NOT_SUPPORTED        8  /* requested feature is not supported */
#define B_ERROR_UNKNOWN              9  /* unknown */

/** 
Playback IP Applib specific Error codes.
**/
#define B_ERROR_PROTO                10  /* protocol Error */
#define B_ERROR_SOCKET_ERROR         11  /* socket operation failed */
#define B_ERROR_IN_PROGRESS          12  /* Requested API is in progress, completion notified via callback */
#define B_ERROR_PSI_PROBE_FAILED     13  /* Media probe failed to obtain PSI info on stream */

/*
IP Library Client Side APIs:

B_PlaybackIp_Open();

B_PlaybackIp_SessionOpen();     

B_PlaybackIp_SendRtspMethod();

B_PlaybackIp_SessionSetup();    

B_PlaybackIp_SessionStart();

B_PlaybackIp_SessionStop();

B_PlaybackIp_SessionClose(); 

B_PlaybackIp_Close();

B_PlaybackIp_EventCallback(): 

B_PlaybackIp_Play():

B_PlaybackIp_Pause():

B_PlaybackIp_Seek():

B_PlaybackIp_Trickmode():

B_PlaybackIp_GetTrickModeSettings();

B_PlaybackIp_GetStatus();

B_PlaybackIp_GetSettings();

B_PlaybackIp_SetSettings();

B_PlaybackIp_GetDefaultSettings();

B_PlaybackIp_NetworkRecordingStart();

B_PlaybackIp_NetworkRecordingStop();

B_PlaybackIp_DetectTts();

Security related APIs

 */

/**
Summary:
Public handle for Playback IP App Lib
**/
typedef struct B_PlaybackIp * B_PlaybackIpHandle;

/**
Summary:
Playback IP App Lib open settings structure
**/
typedef struct B_PlaybackIpOpenSettings
{
    int unused;
} B_PlaybackIpOpenSettings;

/**
Summary:
This function initializes the App Lib based on the settings selected. 
**/
B_PlaybackIpHandle B_PlaybackIp_Open(
    const B_PlaybackIpOpenSettings *pSettings
    );

/**
Summary:
This function de-initializes the App Lib. The private App Lib structure is freed.
**/
B_PlaybackIpError B_PlaybackIp_Close(
    B_PlaybackIpHandle playback_ip
    );

/**
Summary:
Enums defining supported transport protocols
**/
typedef enum B_PlaybackIpProtocol 
{
    B_PlaybackIpProtocol_eUdp,          /* Plain UDP */
    B_PlaybackIpProtocol_eRtp,          /* RTP (requires LIVEMEDIA_SUPPORT compile flag) */
    B_PlaybackIpProtocol_eRtsp,         /* RTSP (requires LIVEMEDIA_SUPPORT compile flag) */
    B_PlaybackIpProtocol_eHttp,         /* HTTP over TCP */
    B_PlaybackIpProtocol_eRtpNoRtcp,    /* RTP (w/o LIVEMEDIA_SUPPORT), no RTCP supported */
    B_PlaybackIpProtocol_eRtpFec,       /* RTP Streaming with FEC: not yet supported */
    B_PlaybackIpProtocol_eHttps,        /* HTTP w/ TLS/SSL */
    B_PlaybackIpProtocol_eMax     
} B_PlaybackIpProtocol;

/**
Summary:
Enums defining supported security protocols
**/
typedef enum B_PlaybackIpSecurityProtocol 
{
    B_PlaybackIpSecurityProtocol_None,    /* Clear Content */
    B_PlaybackIpSecurityProtocol_Ssl,     /* SSL/TLS protected Content */
    B_PlaybackIpSecurityProtocol_DtcpIp,  /* DTCP-IP protected Content */
    B_PlaybackIpSecurityProtocol_RadEa,   /* Rhapsody's RAD-EA protected Content */
    B_PlaybackIpSecurityProtocol_Aes128,  /* Generic AES protocol: used in HLS protected content */
    B_PlaybackIpSecurityProtocol_Max      /* Max value */
} B_PlaybackIpSecurityProtocol;

/**
Summary:
Configurable options for security protocols
**/
typedef struct B_PlaybackIpSecurityOpenSettings
{
    B_PlaybackIpSecurityProtocol securityProtocol;  /* security protocol being used */
    void *initialSecurityContext;         /* optional security protocol specific context: used for passing-in initial DTCP context */
#if NEXUS_HAS_DMA
    NEXUS_DmaHandle dmaHandle;                      /* optional dma handle: used to PVR decryption before streaming out */
#endif
    bool enableDecryption;                /* decrypt n/w data only when this flag is set */
                                          /* needed for some security protocols (RAD/EA, DTCP/IP) where some initial data like HTTP Response is not encrypted */
    bool enableEncryption;                /* encrypt outgoing streamed data only when this flag is set */
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    NEXUS_KeySlotHandle pvrDecKeyHandle;  /* set if PVR Decryption needs to be performed before streaming out the live stream */
#endif
    union {
        struct {
            int voidVar;                  /* just a place holder as we dont need any settings for clear channels */
        } clear;
#ifdef B_HAS_SSL
        struct {
            int dummySsl;                 /* placeholder to avoid compilation warnings */
        } ssl;
#endif
#ifdef B_HAS_DTCP_IP
        struct {
            char *dstIpAddress;
            int emiValue;
        } dtcpIp;
#endif
#ifdef B_HAS_RAD_EA
        struct {
            char *eaUrl;                  /* URL for EA session */
            char *trackId;
            char *logon;
            char *playbackSessionId;
            char *cobrandId;
            int radversion;
            char *format;
            int bitrate;
            char *developerKey;
            int startOffset;
            bool radioChannel;
            char *channelId;
            int position;
            int count;
        } radEa;
#endif
#ifdef B_HAS_HTTP_AES_SUPPORT
        struct {
            unsigned char key[16]; /* 128 bit key for AES-128 */
            unsigned char iv[16]; /* 128 bit IV for AES-128 */
        } aes128;
#endif
    } settings;
} B_PlaybackIpSecurityOpenSettings;

/**
Summary:
This structure used to communicate PSI information back to the caller. 

Description:
IP Applib internally uses the Media Probe library to determine the PSI information of the content.
In addition, incase content is being played from a server using Broadcom's httpstreamer app,
(which supports server based trick modes), additional trick mode related info is also returned.
In DLNA case, this information may be already known to the app when it parsed the Protocol Info
fields of the DLNA Resources.
**/
typedef struct B_PlaybackIpPsiInfo 
{
    bool psiValid;                              /* if true, only then remaining fields contain valid PSI information */
    NEXUS_VideoCodec videoCodec;                /* Rest of the fields are the standard PSI information */
    NEXUS_VideoCodec extraVideoCodec;             /* AVC Extension for SVC/MVC: extra video codec */
    NEXUS_AudioCodec audioCodec;
    int videoPid;
    int extraVideoPid;                            /* AVC Extension for SVC/MVC: extra video pid */
    /* TODO: convert audioPid into list to allow storing multiple audio pids */
    int audioPid;
    int pcrPid;
    int pmtPid;
    NEXUS_TransportType mpegType;
    bool transportTimeStampEnabled;             /* indicates if MPEG2 TS content contains 4 byte DLNA timpstamp */
    bool liveChannel;                           /* if set, this PSI info pertains to a live channel */
    unsigned duration;                          /* duration of stream in milliseconds or 0 if unknown */
    unsigned avgBitRate;                        /* average stream bitrate in bits/second or 0 if unknown */
    NEXUS_PlaybackPosition maxBufferDuration;   /* how much of content can be buffered in HTTP Cache, in milli-sec (based on max bitrate & HTTP cache size */
    /* these are server based trick modes related info: proprietary info provided by the Broadcom's ip streamer app */
    uint32_t firstPts;
    unsigned numPlaySpeedEntries;
    int httpFrameRepeat;                        /* frame repeat count: computed & sent by the HTTP Streamer server */
    int httpMinIFrameSpeed;                     /* Server only sends I Frames at this or higher speed */
    int frameRateInTrickMode;                   /* indicates to the client the frameRate that it should set in the trickmodes in order to achieve a given play speed */
#define IP_PVR_PLAYSPEED_MAX_COUNT  (128)
    int playSpeed[IP_PVR_PLAYSPEED_MAX_COUNT];
    off64_t contentLength;                        /* 0 if not known */

    /* detailed vidoe info */
    float videoFrameRate;                       /* video frame rate in fps, or 0 if unknown */
    uint16_t videoWidth;                        /* coded video width, or 0 if unknown */
    uint16_t videoHeight;                       /* coded video height, or 0 if unknown  */
    unsigned videoBitrate;                      /* video bitrate in Kbps, or 0 if unknown */

    /* detailed audio info */
    unsigned audioBitsPerSample;                /* number of bits in the each sample, or 0 if unknown */
    unsigned audioSampleRate;                   /* audio sampling rate in Hz, or 0 if unknown */
    unsigned audioNumChannels;                  /* number of channels, or 0 if unknown  */
    unsigned audioBitrate;                      /* audio bitrate in Kbps, or 0 if unknown */
    off64_t beginFileOffset;
    off64_t endFileOffset;
#ifdef STREAMER_CABLECARD_SUPPORT
    uint8_t pmtBuffer[4096];
    uint32_t pmtBufferSize;
#endif
    bool hlsSessionEnabled;                     /* set if current session is being receiving using HTTP Live Streaming (HLS) Protocol */
    bool mpegDashSessionEnabled;                /* set if current session is being receiving using MPEG-DASH Protocol */
} B_PlaybackIpPsiInfo;
typedef struct B_PlaybackIpPsiInfo *B_PlaybackIpPsiInfoHandle;

/**
Summary:
This strcuture is used to communicate the buffering scheme employed by the IP STB
B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip: 
    AV decoders lock to PCRs, 
    CDBs are used as dejitter buffer, 
    AV decodes are delayed to absorb the jitter
    all outputs are free run due to high jitter 
    Free running outputs cause Sync-Slip & Video frames & Audio samples are repeated or dropped
B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip
    Requires sender to include 4 byte Timestamps in addition to 188 byte TS packets (DLNA TTS profile)
**/
typedef enum B_PlaybackIpClockRecoveryMode
{
    B_PlaybackIpClockRecoveryMode_ePull,                  /* Pull mode: clock is initialied to 1st PTS and then freely run */
    B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip,   /* Content pushed by Server using PCR */
    B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip, /* SW Buffering & Content Paced by local Playback Hardware using TTS Timestamps */
    B_PlaybackIpClockRecoveryMode_ePushWithPcrNoSyncSlip  /* SW Buffering & Content Paced by local Playback Hardware using PCRs as Timestamps */
} B_PlaybackIpClockRecoveryMode;

/**
Summary:
Handles to various Nexus modules

Description:
App is required to configure all the relevant Nexus Modules. IP Applib only needs these handles for control purposes.
E.g. in HTTP/RTSP case, one usage is to configure the AV decoders & Plaback/Playpump during trickplay transitions.
In RTP/UDP case, it uses these handles to flush the AV pipeline when there is a discontinuity due to network timeouts, etc.
**/
#ifndef DMS_CROSS_PLATFORMS
typedef struct B_PlaybackIpNexusHandles
{
    NEXUS_PlaypumpHandle playpump;          /* required for Live RTP/UDP/RTSP/Server Trickmodes based sessions */
    NEXUS_PlaybackHandle playback;          /* required for HTTP based sessions */
    NEXUS_VideoDecoderHandle videoDecoder;  /* required if video decode is enabled else must be NULL */
    NEXUS_SimpleVideoDecoderHandle simpleVideoDecoder;  /* alternative solution for when video decoder is not available */
    NEXUS_SimpleAudioDecoderHandle simpleAudioDecoder;  /* alternative solution for when audio decoder is not available */
    NEXUS_AudioDecoderHandle primaryAudioDecoder;   /* required if audio decode is enabled else must be NULL */
    NEXUS_AudioDecoderHandle secondaryAudioDecoder; /* required if audio decode is enabled else must be NULL */
    NEXUS_StcChannelHandle stcChannel;      /* required for video & audio enabled streams */
} B_PlaybackIpNexusHandles;
#endif /* DMS_CROSS_PLATFORMS */
/**
Summary:
Various callback events
Note: Some of these events are generated in response to the IP Applib APIs invoked in the non-blocking mode. 
Events only indicate that API is now completed. App still need to reinvoke the api corresponding to the event
for completion status. Only B_ERROR_SUCCESS return code indicates valid values in the returned Status structures.

Also, there are other events which are asynchronously generated and are not associated w/ any particular API.
App can invoke the GetStatus() API to retrieve the detailed event specific status. 
**/
typedef enum 
{
    /* Event to indicate SessionOpen is completed: socket, bind, connect and protocol specific messages are exchanged */
    B_PlaybackIpEvent_eSessionOpenDone,
    /* Event to indicate SessionSetup is completed: for HTTP Media probe is completed */ 
    /* For RTSP basic commands OPTIONS,DESCRIBE and SETUP are sent and response is processed */
    B_PlaybackIpEvent_eSessionSetupDone,
    /* Event to indicate SessionStart is completed (RTSP Play is completed) */
    B_PlaybackIpEvent_eSessionStartDone,
    /* Event to indicate completion of request to playback media at Normal 1x Speed */
    B_PlaybackIpEvent_ePlaying,
    /* Event to indicate completion of request to pause the current media playback */
    B_PlaybackIpEvent_ePaused,
    /* Event to indicate completion of request to pause the current media playback */
    B_PlaybackIpEvent_eStopped,
    /* Event to indicate completion of request to seek media playback to new position */
    B_PlaybackIpEvent_eSeekComplete,
	/* Event to indicate a forward (FFWD) or backward (FREW) scan is initiated */
    B_PlaybackIpEvent_eScanStarted,
#if 0
    /* TODO: need to look into if we can support these events */
	/*
    /// This event meant for enabling application to trigger for a state transition if required to reset the 
    /// state to maintain synch with server. Taking as example, if server doesn't get any response within 
    /// specified time out time, it closes the connection with particular client sending an ANNOUNCE message.
    /// Now if client needs to get streaming again from server, new session need to be achieved. So need to 
    /// reset the current state ie closing the current active session,(show any UI or msg to user) and take 
    /// decision according to user.
    /// Response to Server notification, notified to client via ANNOUNCEMENT protocol command.
    /// Server keeps track of client responses, as time out configured , and if client doesnt respond within the 
    /// Timeout period, automatically closes the session with client, sending ANNOUNCE command.
	*/
    B_PlaybackIpEvent_eServerClientSessionTimeOut,
	/*
    /// This event meant for enabling application to trigger for a state transition if required to reset the 
    /// state to maintain synch with server. 
    /// Response to Server notification, notified to client via ANNOUNCEMENT protocol command.
	*/
    B_PlaybackIpEvent_eServerItemStepped,
	/*
    /// This event meant for enabling application to trigger for a state transition if required to reset the 
    /// state to maintain synch with server. 
    /// Response to Server notification, notified to client via ANNOUNCEMENT protocol command.
	*/
    B_PlaybackIpEvent_eServerCurrentItemRemoved,
#endif
    /* TODO: may need to add a thread to keep track of one way messages from the server */
	/*
    /// This event meant for enabling application to trigger for a state transition if required to reset the 
    /// state to maintain synch with server. 
    /// Response to Server notification, notified to client via ANNOUNCEMENT protocol command.
    /// Server keeps track of current track and as and when it reaches to end of file, it will
    /// notify to client via ANNOUNCEMENT protocol command.
	*/
    B_PlaybackIpEvent_eServerEndofStreamReached,
	/* Event to indicate error during stream playback */
    B_PlaybackIpEvent_eErrorDuringStreamPlayback,
	/* Event to indicate buffer precharge is completed */
    B_PlaybackIpEvent_ePreChargeDone,
	/* Event to indicate IP tuner lock: when 1st packet is received in beginning or after a network timeout exceeding the defined network jitter for a live UDP/RTP/RTSP/HTTP channel */
    B_PlaybackIpEvent_eIpTunerLocked,
	/* Event to indicate IP tuner unlock: when no packet is received for network timeout exceeding the defined network jitter for a live UDP/RTP/RTSP/HTTP channel */
    B_PlaybackIpEvent_eIpTunerUnLocked,
    /* Event to indicate when streaming thread has sent one full segment and is waiting for app to resume streaming of next segment */
    B_PlaybackIpEvent_eServerEndofSegmentReached,
    /* Event to indicate when streaming thread timesout waiting for app to start or resume streaming of next segment */
    B_PlaybackIpEvent_eServerStartStreamingTimedout,
	/* Invalid event */
    B_PlaybackIpEvent_eMax
} B_PlaybackIpEventIds;

/**
Summary:
Callback function to indicate either non-blocking API completion or some asynchronous event
**/
typedef void (*B_PlaybackIp_EventCallback)(void * appCtx, B_PlaybackIpEventIds eventId);

/**
Summary:
This structure returns the actual socket related state back to the application.
**/
typedef struct B_PlaybackIpSocketState
{
    int fd;                             /* socket descriptor being used by the IP Applib to receive data */
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    struct hostent *he;
    socklen_t addr_len;
} B_PlaybackIpSocketState;
typedef struct B_PlaybackIpSocketState *B_PlaybackIpSocketStateHandle;

/**
Summary:
Socket Open Settings
**/
typedef struct B_PlaybackIpSocketOpenSettings
{
    char ipAddr[256];                   /* IP Address (in dotted decimal or char name) string */
    unsigned port;                      /* port number of IP Channel (in host byte order) */
    char *interfaceName;                /* interface name to send multicast joins requests on (for receiving live UDP/RTP sessions) */
    B_PlaybackIpProtocol protocol;      /* protocol: UDP, RTP, HTTP, RTSP */
    char *url;                          /* pointer to URL for HTTP or RTSP protocols: conveys content URL */
    bool useProxy;                      /* if set, ipAddr and port corresponds to the proxy server address:port and url contains the absolute URI */
} B_PlaybackIpSocketOpenSettings;
typedef struct B_PlaybackIpSocketOpenSettings *B_PlaybackIpSocketOpenSettingsHandle;

/**
Summary:
HTTP Session Open Settings
**/
typedef struct B_PlaybackIpHttpSessionOpenSettings
{
    unsigned networkBufferSize;         /* size of network cache, in bytes, where data is buffered (currently used for HTTP based Pull Modes) */
                                        /* 0: IP Applib uses a default size of couple of MB */
    char *additionalHeaders;            /* additional HTTP headers that app wants to include in the outgoing Get Request: need to be \r\n terminated */
    char *userAgent;                    /* app can override the default UserAgent string used in the outgoing HTTP Get Request: string only */
    bool copyResponseHeaders;           /* if true, a copy of the HTTP response Headers is provided back to app */
                                        /* this enables app to process any custom app specific Headers (getProtocolInfo header for obtain protocol info) */
    bool rvuCompliant;                  /* if set, this session should use RVU Compliant headers & Protocol */
} B_PlaybackIpHttpSessionOpenSettings; 

/**
Summary:
RTSP Session Open Settings
**/
typedef struct B_PlaybackIpRtspSessionOpenSettings
{
    char *additionalHeaders;            /* additional headers that app wants to include in the outgoing RTSP OPTION Request */
    char *userAgent;                    /* app can override the default UserAgent string used in the outgoing OPTION Request*/
    bool copyResponseHeaders;           /* if true, a copy of the OPTION response Headers is provided back to app */
                                        /* this enables app to process & retrieve any custom app specific Headers (ServiceGroup, Version, AppData, etc.) */
} B_PlaybackIpRtspSessionOpenSettings; 

/**
Summary:
Aggregate IP Session Open Settings
**/
typedef struct B_PlaybackIpSessionOpenSettings
{
    B_PlaybackIpSocketOpenSettings socketOpenSettings;  /* socket related settings */
    B_PlaybackIpSecurityOpenSettings security;          /* Security Open Settings */
    B_PlaybackIpClockRecoveryMode ipMode;               /* clock recovery mode */
                                                        /* if App apriori knows the PSI info, then it has enough info to decide which clock recovery mode to use */
                                                        /* ip_client.c has an example function useLiveModeForIpPlayback() to aid apps in determining this flag */
    int maxNetworkJitter;                               /* Maximum network jitter in millisec: used to determining packet loss conditions */
                                                        /* this is considered as unmarked discontinuity and AV pipeline is flushed at this point */
    bool useNexusPlaypump;                              /* true: tells IP Applib to feed network stream to the Nexus Playpump, False: tells IP Applib that Nexus Playback will pull stream from IP Applib */
                                                        /* if App apriori knows the PSI info, then it has enough info to decide weather to use Nexus Playpump or Playback */
                                                        /* ip_client.c has an example function useNexusPlaypumpForIpPlayback() to aid apps in determining this. */
    unsigned networkTimeout;                            /* provides the timeout value, in seconds, for network operations like select, read. */
                                                        /* 0: IP Applib uses a default of 30 sec */
    bool nonBlockingMode;                               /* if set, requires apps to also set following eventCallback function field */
                                                        /* if IP lib can't immediately complete an IP library API, it will return B_ERROR_IN_PROGRESS. */ 
                                                        /* The API is asynchronously carried out in the IP library thread context */
                                                        /* and app is notified of its completion via eventCallback. App is then required to *re-issue* the */
                                                        /* same IP library API to read the results (success/failure). */
    B_PlaybackIp_EventCallback eventCallback;           /* callback function if nonBlockingMode is desired or to receive any asynchronous events from the IP library */
    void *appCtx;                                       /* app context handle used in the callback function */
    union {
        B_PlaybackIpHttpSessionOpenSettings http;
        B_PlaybackIpRtspSessionOpenSettings rtsp;
    }u;
    NEXUS_HeapHandle heapHandle;                        /* nexus heap Handle, can be null if app doesn't have a preference */
} B_PlaybackIpSessionOpenSettings;

/**
Summary:
HTTP & RTSP specific Session Open Status (all output parameters should be included in this structure)
TODO: May need to separate the HTTP & RTSP status, so far it seems to have common fields.
**/
typedef struct B_PlaybackIpHttpSessionOpenStatus
{
    int statusCode;                     /* Returned status code for the Request */
    char *responseHeaders;              /* pointer to response headers string if copyResponseHeaders flag is set in open settings */
                                        /* IP Applib frees this memory when IP Session is closed */
} B_PlaybackIpHttpSessionOpenStatus, B_PlaybackIpRtspSessionOpenStatus;

/**
Summary:
Aggregate IP Session Open Status
**/
typedef struct B_PlaybackIpSessionOpenStatus
{
    B_PlaybackIpSocketState socketState;
    union {
        B_PlaybackIpHttpSessionOpenStatus http;
        B_PlaybackIpRtspSessionOpenStatus rtsp;
    }u;
} B_PlaybackIpSessionOpenStatus;

/**
Summary:
API to Open a IP Session

Description:
This API will do the initial socket related configuration (socket,bind,connect,setsockopt,multicast join, etc.). 
Also, depending on the chosen protocol, it will start a session w/ the server. E.g. in HTTP case, it will 
send out the HTTP Get Request and process the Get response. In RTSP case, it will send the RTSP Option message 
and process response. This API can be used in both blocking & non-blocking mode in which a callback in invoked
to indicate completion and then App needs to re-invoke this API to read the status (success/failure).
**/
B_PlaybackIpError B_PlaybackIp_SessionOpen(
    B_PlaybackIpHandle playback_ip,                 /* [in] */
    B_PlaybackIpSessionOpenSettings *openSettings,  /* [in] */
    B_PlaybackIpSessionOpenStatus *openStatus       /* [out] */
    );

/**
Summary:
RTSP Methods that app can specifically choose to invoke 
**/
typedef enum B_PlaybackIpRtspMethodName 
{
    B_PlaybackIpRtspMethod_eOptions,        /* RTSP OPTIONS Method */
    B_PlaybackIpRtspMethod_eDescribe,       /* RTSP DESCRIBE Method */
    B_PlaybackIpRtspMethod_eSetup,          /* RTSP SETUP Method */
    B_PlaybackIpRtspMethod_eGetParameter,   /* RTSP GET PARAMETER Method */
    B_PlaybackIpRtspMethod_eAnnounce        /* RTSP ANNOUNCE Method */
} B_PlaybackIpRtspMethodName;

/**
Summary:
RTSP Method related Settings
**/
typedef struct B_PlaybackIpRtspMethodSettings
{
    B_PlaybackIpRtspMethodName name;        /* RTSP Method name: */
    char *additionalHeaders;                /* additional headers that app wants to include in the outgoing RTSP Method */
    char *userAgent;                        /* app can override the default UserAgent string used in the outgoing Method Request*/
    bool copyResponseHeaders;               /* if true, a copy of the RTPS Method response Headers is provided back to app */
                                            /* this enables app to process & retrieve any custom app specific Headers (ServiceGroup, Version, AppData, etc.) */
} B_PlaybackIpRtspMethodSettings; 

/**
Summary:
RTSP Send Method Status 
**/
typedef struct B_PlaybackIpRtspMethodStatus
{
    int statusCode;                 /* Returned status code for the Request */
    char *responseHeaders;          /* pointer to response headers string if copyResponseHeaders flag is set in open settings */
                                    /* this allows apps to parse any custom response headers from the RTSP server and extract any fields for further use */
                                    /* this buffer is re-used in subsequent calls so app can't hold on it */
} B_PlaybackIpRtspMethodStatus;

/**
Summary:
API to allow app to send a particular RTSP method to the server 
**/
B_PlaybackIpError B_PlaybackIp_SendRtspMethod(
    B_PlaybackIpHandle playback_ip,                     /* [in] */
    B_PlaybackIpRtspMethodSettings *settings,           /* [in] */
    B_PlaybackIpRtspMethodStatus *status                /* [out]*/
    );

/**
Summary:
HTTP Session Setup Settings
**/
typedef struct B_PlaybackIpHttpSessionSetupSettings
{
    NEXUS_TransportType contentTypeHint; /* app can hint the content type if it is known */
    off64_t contentLengthHint;        /* app can hint content (useful for chunk xfer encoding case where server doesn't send it in HTTP resp, 0 if not known */
    bool enablePayloadScanning;     /* if true, set up probe engine to determine accurate stream duration using payload scanning & random access */
                                    /* Note: setting this option will delay the start of video, but is needed to determine accurate info for perforing trick modes on TS & PES/VOB formats */
    bool skipPsiParsing;            /* if true, IP Applib doesn't probe the content for PSI information, app should apriori have this info */
    long psiParsingTimeLimit;       /* max number of msec allowed for psi parsing/media probe operation */
    unsigned avgBitRate;            /* average stream bitrate in bps if already known or 0 if unknown */ 
                                    /* If App set skipPsiParsing, then it is required to set avgBitRate value if app also wants to enable Runtime Buffering at IP Playback level. */
    unsigned seekOffsetTime;        /* initial time offset in milliseconds */
    bool disableRangeHeader;        /* if true, IP Applib doesn't include the Range header in the Get Requests */
    bool dontUseIndex;              /* dont use index for playback: false by default */
    /*TODO: may be this should be part of the Start call */
    bool convertLpcmToWave;         /* if Set, app is requesting to convert incoming LPCM files into Wave by inserting the Wave header before the data */
    unsigned bitsPerSample; 
    unsigned sampleRate;
    unsigned numChannels;
} B_PlaybackIpHttpSessionSetupSettings;

/**
Summary:
RTSP Session Setup Settings
**/
typedef struct B_PlaybackIpRtspSessionSetupSettings
{
    char *additionalHeaders;        /* additional headers that app wants to include in the all subsequent RTSP Requests (OPTION/DESCRIBE/SETUP) */
    bool alwaysUseAdditionalHeaders; /* indicates if all subsequent call should use these additional headers */
    char *userAgent;                /* app can override the default UserAgent string used in the outgoing OPTION/SETUP Request*/
    bool copyResponseHeaders;       /* if true, a copy of the SETUP response Headers is provided back to app */
                                    /* this enables app to process any custom app specific Headers (ServiceGroup, Version, AppData, etc.) */
} B_PlaybackIpRtspSessionSetupSettings;

/**
Summary:
Aggregate Session Setup Settings
**/
typedef struct B_PlaybackIpSessionSetupSettings
{
    unsigned unused;
    union {
        B_PlaybackIpHttpSessionSetupSettings http;
        B_PlaybackIpRtspSessionSetupSettings rtsp;
    }u;
} B_PlaybackIpSessionSetupSettings;

/* App specific Header that gets inserted before media is streamed out */
typedef struct 
{
    bool valid; /* set if appHeader is valid */
    unsigned length; /* length of app header bytes: should be mod16 aligned if it needs to be encrypted */
    uint8_t data[192]; /* pointer to app header */
} B_PlaybackIpAppHeader;

/**
Summary:
HTTP Setup Status (output parameters of SessionSetup() )
**/
typedef struct B_PlaybackIpHttpSessionSetupStatus
{
    B_PlaybackIpPsiInfo psi;        /* PSI info for the media: valid only if psi.psiValid field is true */
    NEXUS_FilePlayHandle file;      /* file handle abstraction of the the network socket which allows apps/Nexus Playback to pull n/w data using file operations (read/seek/bounds) */
    const void *stream;             /* pointer to the bmedia_probe_stream structure to allow apps to extract additional program/track info */
    B_PlaybackIpAppHeader appHeader; /* appSpecific binary header sent by the server */
} B_PlaybackIpHttpSessionSetupStatus;

/**
Summary:
RTSP Setup Status (output parameters of SessionSetup() )
**/
#define MAX_SCALE_LIST_ENTRIES 16 
typedef struct B_PlaybackIpRtspSessionSetupStatus
{
    int statusCode;                 /* Returned status code for the Request */
    char *responseHeaders;          /* pointer to response headers string if copyResponseHeaders flag is set in open settings */
                                    /* this allows apps to parse any custom response headers from the RTSP server and extract any fields for further use */
                                    /* this buffer is re-used in subsequent calls so app can't hold on it */
    int scaleListEntries;           /* number of entries in the scale list provided by the server, 0 if none */
    float scaleList[MAX_SCALE_LIST_ENTRIES]; /* scale list provided by the server, 0 if none */
} B_PlaybackIpRtspSessionSetupStatus;

/**
Summary:
Aggregate Session Setup Status
**/
typedef struct B_PlaybackIpSessionSetupStatus
{
    unsigned unused;
    union {
        B_PlaybackIpHttpSessionSetupStatus http;
        B_PlaybackIpRtspSessionSetupStatus rtsp;
    }u;
}B_PlaybackIpSessionSetupStatus;

/**
Summary:
Session Setup API
Description:
This API establishes media session with the server and is applicable for HTTP and RTSP protocols only. E.g. for HTTP
protocol, is app has requested, this API can interact with the server to probe the media details (PSI, bitrate, etc.). 
For RTSP protocol, this API will send OPTIONS, DESCRIBE, and SETUP messages to the server and process the response.

App can choose to work in the non-blocking mode in which case API returns E_IN_PROGRESS. Later when api is completed,
a callback is invoked w/ event B_PlaybackIpEvent_eSessionSetupDone. App should then re-call this api to determine

Note: this API doesn't do any work for UDP/RTP protocols.
**/
B_PlaybackIpError B_PlaybackIp_SessionSetup(
    B_PlaybackIpHandle playback_ip,                     /* [in] */
    B_PlaybackIpSessionSetupSettings *setupSettings,    /* [in] */
    B_PlaybackIpSessionSetupStatus *setupStatus         /* [out]*/
    );

/**
Summary:
API to close IP Playback session (informs server about session going away: RTSP TEARDOWN, HTTP: socket close)
**/
B_PlaybackIpError B_PlaybackIp_SessionClose(
    B_PlaybackIpHandle playback_ip /* Handle returned by bplayback_ip_open */
    );

/**
Summary:
IP Session Start Settings
**/
typedef struct B_PlaybackIpHttpSessionStartSettings
{
    bool preChargeBuffer;           /* true:  tells IP Applib to start pre-charging its network buffer */
                                    /* false: tells IP Applib to stop pre-charging if its pre-charging its network buffer */
} B_PlaybackIpHttpSessionStartSettings;

/**
Summary:
RTSP Session Start Settings
**/
typedef struct B_PlaybackIpRtspSessionStartSettings
{
    B_PlaybackIpProtocol mediaTransportProtocol;    /* Raw UDP or RTP: protocol used to transport media from server */
                                    /* if set to B_PlaybackIpProtocol_eMax, IP library determines it from the Transport header in the SETUP response, default of RTP */
    NEXUS_PlaybackPosition start;   /* start position in milli-sec: 0 means from the start */
    NEXUS_PlaybackPosition end;     /* end position in milli-sec: 0 means play to the end */
    int keepAliveInterval;          /* interval in seconds to send out the Keep alive heartbeats to the server, 0: IP library chooses the interval */
} B_PlaybackIpRtspSessionStartSettings;

/**
Summary:
Aggregate Session Setup Settings
**/
typedef struct B_PlaybackIpSessionStartSettings
{
    bool nexusHandlesValid;         /* set this flag if nexusHandles are being set */
#ifndef DMS_CROSS_PLATFORMS
    B_PlaybackIpNexusHandles nexusHandles; /* nexus handles: Playback handle is only needed for HTTP sessions */
#endif /* DMS_CROSS_PLATFORMS */
    union {
        B_PlaybackIpHttpSessionStartSettings http;
        B_PlaybackIpRtspSessionStartSettings rtsp;
    }u;
    NEXUS_TransportType mpegType;
} B_PlaybackIpSessionStartSettings;

/**
Summary:
HTTP Setup Status (output parameters of SessionStart() )
**/
typedef struct B_PlaybackIpHttpSessionStartStatus
{
    NEXUS_PlaybackPosition curBufferDuration;   /* if preChargeBuffer flag is set, this field indicates how much of content is currently buffered in HTTP Cache, in milli-sec */
    NEXUS_PlaybackPosition maxBufferDuration;   /* if preChargeBuffer flag is set, this field indicates how much of content can be buffered in HTTP Cache, in milli-sec (based on max bitrate & HTTP cache size */
} B_PlaybackIpHttpSessionStartStatus;

/* TODO: B_PlaybackIpRtspSessionStartStatus  --> B_PlaybackIpRtspSessionStatus if all return status seems to be the same */
/**
Summary:
RTSP Setup Status (output parameters of SessionStart() )
**/
typedef struct B_PlaybackIpRtspSessionStartStatus
{
    int statusCode;                 /* Returned status code for the Request */
    char *responseHeaders;          /* pointer to response headers string if copyResponseHeaders flag is set in open settings */
                                    /* this allows apps to parse any custom response headers from the RTSP server and extract any fields for further use */
                                    /* this buffer is re-used in subsequent calls so app can't hold on it */
} B_PlaybackIpRtspSessionStartStatus;

/**
Summary:
Aggregate Session Setup Status
**/
typedef struct B_PlaybackIpSessionStartStatus
{
    unsigned unused;
    union {
        B_PlaybackIpHttpSessionStartStatus http;
        B_PlaybackIpRtspSessionStartStatus rtsp;
    }u;
} B_PlaybackIpSessionStartStatus;

/**
Summary:
API to start IP Session
For HTTP protocol, it starts the buffering thread.
For RTSP protocol, it sends PLAY command to the server and status 
UDP/RTP: starts the thread to receive UDP/RTP data and start processing it. This also happens for RTSP case.
**/
B_PlaybackIpError B_PlaybackIp_SessionStart(
    B_PlaybackIpHandle playback_ip, 
    B_PlaybackIpSessionStartSettings *startSettings,
    B_PlaybackIpSessionStartStatus *startStatus
    );

/**
Summary:
API to stop IP Session
**/
B_PlaybackIpError B_PlaybackIp_SessionStop(
    B_PlaybackIpHandle playback_ip
    );

/**
Summary:
TTS throttle parameters
**/
typedef struct B_PlaybackIp_TtsThrottle_Params {
#ifndef DMS_CROSS_PLATFORMS
    NEXUS_PlaypumpHandle playPump;      /* handle of the playpump associated with this TTS throttle */
#endif /* DMS_CROSS_PLATFORMS */
    NEXUS_Timebase timebase;            /* timebase that will be frequency controlled by the TTS throttle */
    unsigned initBufDepth;              /* sets the initial buffer depth (used as a target buffer depth by TTS pacing) */
    unsigned minBufDepth;               /* minimum buffer depth before buffer depth violation callback is called */
    unsigned maxBufDepth;               /* minimum buffer depth before buffer depth violation callback is called */
    unsigned maxClockMismatch;          /* specify the maximum clock mismatch (in ppm) between server/encoder and STB */
    void (*bufDepthViolationCallback)(void *context, bool isMax); /* function will be called if max/min buffer depth is violated */
    void *bufDepthViolationCbContext;   /* context passed into buf_depth_violation_callback */
} B_PlaybackIp_TtsThrottle_Params;

typedef struct B_PlaybackIp_TtsParams {
    bool autoDetect;                    /* automatically detect if the stream is TTS or TS */
    unsigned pacingMaxError;            /* set the timestamp error bound, as used by the playback pacing logic */
    B_PlaybackIp_TtsThrottle_Params throttleParams;
} B_PlaybackIp_TtsParams;

/**
Summary:
Playback IP App Lib settings structure
**/
typedef struct B_PlaybackIpSettings
{
    B_PlaybackIpClockRecoveryMode ipMode; /* clock recovery mode for Live channels */
                                          /* ip_client.c has an example function useLiveModeForIpPlayback() to aid apps in determining this flag */
    int maxNetworkJitter;               /* Maximum network jitter in millisec: used to determining packet loss conditions */
                                        /* this is considered as unmarked discontinuity and AV pipeline is flushed at this point */
    B_PlaybackIp_TtsParams ttsParams;   /* TTS related parameters */

    bool nexusHandlesValid;             /* set this flag if nexusHandles are being set */
#ifndef DMS_CROSS_PLATFORMS
    B_PlaybackIpNexusHandles nexusHandles; /* provides handles to various Nexus modules */
#endif /* DMS_CROSS_PLATFORMS */
    bool useNexusPlaypump;              /* true: tells IP Applib to feed network stream to the Nexus Playpump, False: tells IP Applib that Nexus Playback will pull stream from IP Applib */
                                        /* ip_client.c has an example function useNexusPlaypumpForIpPlayback() to aid apps in determining the value of this flag */
    bool preChargeBuffer;               /* true:  tells IP Applib to start pre-charging its network buffer */
                                        /* false: tells IP Applib to stop pre-charging if its pre-charging its network buffer */
    unsigned networkTimeout;            /* provides the timeout value, in seconds, for network operations like select, read. */
                                        /* 0: IP Applib uses a default of 30 sec */
    unsigned networkBufferSize;         /* size of network cache, in bytes, where data is buffered (currently used for HTTP based Pull Modes) */
                                        /* 0: IP Applib uses a default size of couple of MB */
} B_PlaybackIpSettings;

/**
Summary:
This function returns the default and recommended values for the IP App Lib public settings. 
**/
B_PlaybackIpError B_PlaybackIp_GetDefaultSettings(
    B_PlaybackIpSettings *pSettings
    );

/**
Summary:
This function returns the current values for the Playback IP App Lib public settings. 
**/
B_PlaybackIpError B_PlaybackIp_GetSettings(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSettings *pSettings
    );

/**
Summary:
This function updates the current values for the Playback IP App Lib public settings. 
**/
B_PlaybackIpError B_PlaybackIp_SetSettings(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSettings *pSettings
    );

/**
Summary:
Supported Trick play types

Description:
These types allows app to specify whether it wants to use client based (ByteRange or TimeSeekRange) or server based (PlaySpeed) trickplays
**/
typedef enum B_PlaybackIpTrickModesMethod 
{
    B_PlaybackIpTrickModesMethod_UseByteRange,      /* Client side trick play: Use HTTP Range header to request chunks of data */
                                                    /* requires apps to configure using Nexus Playback for this mode */
    B_PlaybackIpTrickModesMethod_UseTimeSeekRange,  /* Server side trick play: Use Time ranges instead of byte ranges to achieve affect of trick modes */
                                                    /* requires apps to configure using Nexus Playpump for this mode */
    B_PlaybackIpTrickModesMethod_UsePlaySpeed,      /* Server side trick play: inform server about the desired play speed and play the returned scaled data */
                                                    /* requires apps to configure using Nexus Playpump for this mode */
    B_PlaybackIpTrickModesMethod_UseRvuSpec,        /* RVU trickmodes: combination of client & server side trick modes */
                                                    /* requires apps to configure using Nexus Playpump for this mode */
    B_PlaybackIpTrickModesMethod_Max
} B_PlaybackIpTrickModesMethod;

/**
Summary:
Various methods to pause a playback session

Description:
Only applicable for HTTP transport where either client can stall TCP connection or disconnect & reconnect w/ time seek
**/
typedef enum B_PlaybackIpPauseMethod 
{
    B_PlaybackIpPauseMethod_UseConnectionStalling,  /* stall TCP connection by stop reading from socket: provides instant resumption */
    B_PlaybackIpPauseMethod_UseDisconnectAndSeek,   /* close current TCP connection and re-open new one upon play w/ new seek offset */
    B_PlaybackIpPauseMethod_Max
} B_PlaybackIpPauseMethod;

/**
Summary:
Trickmode settings
**/
typedef struct B_PlaybackIpTrickModesSettings 
{
    B_PlaybackIpTrickModesMethod method;        /* type of trick mode to perform */
    int rate;                                   /* desired rate/speed (in the units of 1, 2, 3, -1, -2, -3): simplified rate to provide next speed */
    bool absoluteRateDefined;                   /* if set, IP library should simply use the rate value defined in the next absoluteRate field & ignore the previous rate field */
    int  absoluteRate;                          /* Speed control based on units of NEXUS_NORMAL_DECODE_RATE: */
                                                /* NEXUS_NORMAL_DECODE_RATE = normal play. */
                                                /* 0 = pause. */
                                                /* NEXUS_NORMAL_DECODE_RATE*2 = 2x fast-forward. */
                                                /* NEXUS_NORMAL_DECODE_RATE/2 = 2x slow-motion. */

    int frameRateInTrickMode;                   /* frameRate that it should set in the trickmodes in order to achieve a given play speed */
                                                /* server w/ playspeed support provides this value via the DLNA FrameRateInTrickMode header */
    /* note: this is being deprecated */
    unsigned frameRepeat;                       /* specifies the number of times the decoder should display each frame/field */
                                                /* typically set based on the frameRepeat value provided by the server */
    B_PlaybackIpPauseMethod pauseMethod;        /* indicates how to pause: just stall pipeline/connection or Disconnect and reconnect to the paused location  */
    NEXUS_PlaybackPosition seekPosition;        /* seek position in millisecond */
    bool seekPositionIsRelative;                /* if set, then seekPosition is a relative to current position */
    bool seekBackward;                          /* if set, then seekPosition is a relative to current position in the backward direction */
#ifndef DMS_CROSS_PLATFORMS
    NEXUS_VideoDecoderDecodeMode decodeMode;    /* allows apps to instruct decoder to only play certain frames types (e.g. I, IP), default is all fed frames */
#endif   /* DMS_CROSS_PLATFORMS  */ 
} B_PlaybackIpTrickModesSettings;

/**
Summary:
API to pause the IP Session (applicable for RTSP & HTTP Sessions)
**/
B_PlaybackIpError B_PlaybackIp_Pause(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *trickMode
    );

/**
Summary:
API to resume the IP Session (applicable for RTSP & HTTP Sessions)
**/
B_PlaybackIpError B_PlaybackIp_Play(
    B_PlaybackIpHandle playback_ip
    );

/**
Summary:
API to carry out the trickMode commands (applicable for RTSP & HTTP Sessions)
**/
B_PlaybackIpError B_PlaybackIp_TrickMode(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *trickMode
    ); 

/**
Summary:
API to carry out the seek trickMode command (HTTP & RTSP sessions Sessions)
**/
B_PlaybackIpError B_PlaybackIp_Seek(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *trickMode
    ); 

/**
Summary:
API to get trickmode parameter for a given rate
**/
B_PlaybackIpError B_PlaybackIp_GetTrickModeByRate(
    B_PlaybackIpHandle playback_ip,
    int rate,
    B_PlaybackIpTrickModesSettings *trickMode
    ); 

/**
Summary:
API to provide the current/default trickmode Settings 
**/
B_PlaybackIpError B_PlaybackIp_GetTrickModeSettings(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    ); 

/**
Summary:
State of a IP playback context: relevant for non-live sessions
(e.g. for HTTP or RTSP VoD playbacks)
**/
typedef enum B_PlaybackIpState 
{
    B_PlaybackIpState_eClosed,                  /* Initial closed state */
    B_PlaybackIpState_eOpened,                  /* Playback IP session context is allocated, but not initialized */
    B_PlaybackIpState_eSessionOpenInProgress,   /* SessionOpen is in progress */
    B_PlaybackIpState_eSessionOpened,           /* SessionOpen is successfully completed */
    B_PlaybackIpState_eStopped = B_PlaybackIpState_eSessionOpened, /* SessionStop is completed: equivalent to eSetup State */
    B_PlaybackIpState_eSessionSetupInProgress,  /* SessionSetup is in progress */
    B_PlaybackIpState_eSessionSetup,            /* SessionSetup is completed */
    B_PlaybackIpState_eSessionStartInProgress,  /* SessionStart is in progress */
    B_PlaybackIpState_ePlaying,                 /* SessionStart is completed and we are ready to play */
    B_PlaybackIpState_eStopping,                /* SessionStop is in progress */
    B_PlaybackIpState_ePaused,                  /* IP Session has been paused */
    B_PlaybackIpState_eWaitingToEnterTrickMode, /* IP Session is waiting to enter trick mode state as either Nexus IO or HTTP thread is currently using the session */
    B_PlaybackIpState_eEnteringTrickMode,       /* IP Session is entering trick mode state */
    B_PlaybackIpState_eTrickMode,               /* IP Session is in trick mode state */
    B_PlaybackIpState_eBuffering,               /* IP Session is buffering data */
    B_PlaybackIpState_eMax                      /* Invalid State */
} B_PlaybackIpState;

/**
Summary:
SSL Specific Settings 
**/
typedef struct B_PlaybackIpStatus 
{
    NEXUS_PlaybackPosition curBufferDuration;   /* how much of content is currently buffered in HTTP Cache, in milli-sec */
    NEXUS_PlaybackPosition maxBufferDuration;   /* how much of content can be buffered in HTTP Cache, in milli-sec (based on max bitrate & HTTP cache size */
    B_PlaybackIpState ipState;                  /* current state of the IP Applib */
    bool serverClosed;                          /* set to true if server has closed the connection before the GetStatus() call */
    int httpStatusCode;                         /* Returned status code for the HTTP Request */
    /* need to see if we will continue to need these fields for drawing the playback bar, this info can be obtained directly from Playback */
    /* can only be an issue for HTTP playback from httpstreamer */
    NEXUS_PlaybackPosition first;               /* first timestamp of stream played in mSec */
    NEXUS_PlaybackPosition last;                /* last timestamp of stream played in mSec */
    NEXUS_PlaybackPosition position;            /* current timestamp of stream played in mSec */
    unsigned numRecvTimeouts;                   /* count of the number of receive timeouts since start of IP playback */
} B_PlaybackIpStatus;

/**
Summary:
This function returns the IP Playback status (current PTS, Playpump FIFO Info, etc.)
TODO: add info regarding socket stats (bytes received, etc.) in the Status Structure
**/
B_PlaybackIpError B_PlaybackIp_GetStatus(
    B_PlaybackIpHandle playback_ip, 
    B_PlaybackIpStatus *ipStatus    /* [out]: returned status */
    );

/**
Summary:
API to detect if the stream contains timestamps or not
**/
B_PlaybackIpError B_PlaybackIp_DetectTts(
    B_PlaybackIpHandle playback_ip,
    bool *isTts
    ); 

#ifdef B_HAS_SSL
/**
Summary:
SSL Specific Settings 
**/
typedef struct B_PlaybackIpSslInitSettings
{
    char *rootCaCertPath;                 /* path for root CA cert, can be turned into array of multiple CA certs */
    bool clientAuth;                      /* client Authentication. Set true for following paramerters */
    char *ourCertPath;                    /* path for our certificate */
    char *privKeyPath;                    /* path for private keys */
    char *password;                       /* set to NULL if no password */
    bool sslLibInitDone;                  /* Set true if previously called SSL_library_init() */ 
} B_PlaybackIpSslInitSettings;

/**
Summary:
API to create SSL context.
**/
void* B_PlaybackIp_SslInit(             /* returns ssl context */
        B_PlaybackIpSslInitSettings *initSettings
        );

/**
Summary:
API to destroy SSL context
**/
void B_PlaybackIp_SslUnInit(void *ctx);

#endif

#ifdef B_HAS_RAD_EA
/**
Summary:
API to create RAD/EA context.
**/
void* B_PlaybackIp_RadEaInit(             /* returns radea context */
        B_PlaybackIpSslInitSettings *initSettings
        );

void B_PlaybackIp_RadEaUnInit(void *ctx);

#endif

#ifdef B_HAS_DTCP_IP
/**
Summary:
APIs to initialize & un-initialize DTCP/IP context.
**/
void * B_PlaybackIp_DtcpIpInit(void *);
void B_PlaybackIp_DtcpIpUnInit(void *);
#endif

/**
Summary:
This function sets up the network record functionality where a live stream 
is recorded on the specified server.
**/
B_PlaybackIpError B_PlaybackIp_NetworkRecordingStart(
    B_PlaybackIpSocketOpenSettingsHandle socketOpenSettings, 
    const char *fname, const char *indexname,
    B_PlaybackIpPsiInfoHandle psi, 
    NEXUS_FileRecordHandle *fileRecordPtr
    );

/**
Summary:
This function stops the network recording session.
**/
void B_PlaybackIp_NetworkRecordingStop(
    struct NEXUS_FileRecord *file
    );

/* *********************************************** */
/*       HTTP Streaming Server Specific APIs       */
/* *********************************************** */
typedef enum B_PlaybackIpConnectionState 
{
    B_PlaybackIpConnectionState_eSetup,  /* Socket connection w/ peer is being setup and not yet xfering AV data */
    B_PlaybackIpConnectionState_eActive, /* Socket connection w/ peer is active (sending or receiving AV data) */
    B_PlaybackIpConnectionState_eTimeout,/* Socket connection w/ peer has timeouts in either sending or receiving data */
    B_PlaybackIpConnectionState_eError,  /* Socket connection w/ peer has encoutered some errors: peer abnormally closed */
    B_PlaybackIpConnectionState_eEof,    /* Socket connection w/ peer has been gracefully closed by the peer */
    B_PlaybackIpConnectionState_eMax     /* Max value */
} B_PlaybackIpConnectionState;

typedef struct B_PlaybackIpLiveStreamingRtpUdpOpenSettings {
    int streamingPort; /* Port # to stream for RTP/UDP protocols */
    char streamingIpAddress[32]; /* IP address to stream for RTP/UDP protocols */
	char *interfaceName; /* For UDP/RTP protocol, app should provide the interface name to stream out on (defaults to eth0) */
} B_PlaybackIpLiveStreamingRtpUdpOpenSettings;

/* For Streaming from a Live (QAM/VSM/IP) or File (local disk) sources */
typedef struct B_PlaybackIpLiveStreamingOpenSettings
{
    char fileName[128]; /* Name of the file to stream */
    off64_t beginFileOffset;
    off64_t endFileOffset;
    bool autoRewind;    /* set to indicate stream rewind upon eof */
    int playSpeed;
    B_PlaybackIpProtocol protocol; /* protocol: UDP, RTP, HTTP */
    int streamingFd; /* socket fd on which to stream data on (not needed for RTP/UDP protocols) */
    B_PlaybackIpLiveStreamingRtpUdpOpenSettings rtpUdpSettings;
    B_PlaybackIpSecurityOpenSettings securitySettings; /* Security Open Settings */
    B_PlaybackIpPsiInfoHandle psi;    /* since app had to know the PSI settings for live streams, it passes them to IP library */
    NEXUS_FifoRecordHandle fifoFileHandle;
    NEXUS_RecpumpHandle recpumpHandle;
    B_PlaybackIp_EventCallback eventCallback;           /* callback function if nonBlockingMode is desired or to receive any asynchronous events from the IP library */
    void *appCtx;                                       /* app context handle used in the callback function */
    bool streamingDisabled; /* flag to indicate to live streaming thread that streaming has been disabled for this thread (Fast Channel Change) */
    BKNI_EventHandle dataReadyEvent;                    /* recpump uses this event to notify IP library about availability of data in the recpump buffer */
    bool transportTimestampEnabled;                     /* indicates if MPEG2 TS content contains 4 byte DLNA timpstamp */
    bool hlsSession;         /* set if this is a HLS session */
    int hlsSegmentSize;      /* size of a hls segment in bytes */
    B_PlaybackIpAppHeader appHeader;    /* app header that gets inserted before media is streamed out */
    bool enableTimeshifting;    /* set if app has enabled timeshifting */
    bool enableTimestamps;      /* set if app has enabled timestamps in the recorded live streams */
    NEXUS_HeapHandle heapHandle; /* nexus heap Handle, can be null if app doesn't have a preference */
} B_PlaybackIpLiveStreamingOpenSettings;

/* Settings that can be updated when live streaming start is already called */
typedef struct B_PlaybackIpLiveStreamingSettings
{
    bool streamingEnabled; /* flag to indicate to live streaming thread that streaming has now been enabled (Fast Channel Change) */
    bool resumeStreaming; /* flag to indicate to live streaming thread to resume streaming for the new HLS segment */
    int streamingFd; /* socket fd on which to stream data on */
    int hlsSegmentSize;      /* size of a hls segment in bytes */
} B_PlaybackIpLiveStreamingSettings;

/***************************************************************************
Summary:
This function starts streaming the live content from rave buffers to a network client.
***************************************************************************/
typedef struct B_PlaybackIpLiveStreaming *B_PlaybackIpLiveStreamingHandle;
B_PlaybackIpLiveStreamingHandle
B_PlaybackIp_LiveStreamingOpen(
    B_PlaybackIpLiveStreamingOpenSettings *liveStreamingSettings
    );

/***************************************************************************
Summary:
This function stops the live streaming function.
***************************************************************************/
void B_PlaybackIp_LiveStreamingClose(B_PlaybackIpLiveStreamingHandle liveStreamingHandle);
B_PlaybackIpError
B_PlaybackIp_LiveStreamingStart(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle
    );
void 
B_PlaybackIp_LiveStreamingStop(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle
    );

typedef struct B_PlaybackIpLiveStreamingStatus 
{
    off64_t bytesStreamed;
    B_PlaybackIpConnectionState connectionState;
} B_PlaybackIpLiveStreamingStatus;
void 
B_PlaybackIp_LiveStreamingGetStatus(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle,
    B_PlaybackIpLiveStreamingStatus *status 
    );
B_PlaybackIpError
B_PlaybackIp_LiveStreamingSetSettings(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle,
    B_PlaybackIpLiveStreamingSettings *settings
    );

typedef struct B_PlaybackIpFileStreaming *B_PlaybackIpFileStreamingHandle;
typedef struct B_PlaybackIpFileStreamingOpenSettings 
{
    char fileName[128]; /* Name of the file to stream */
    char mediaInfoFilesDir[128];  /* directory name where the info & nav files should be created */
    off64_t beginFileOffset;
    off64_t endFileOffset;
    bool autoRewind;    /* set to indicate stream rewind upon eof */
    int playSpeed;
    double beginTimeOffset;
    double endTimeOffset;    
    B_PlaybackIpProtocol protocol; /* protocol: UDP, RTP, HTTP */
    int streamingFd; /* socket fd on which to stream data on */
    B_PlaybackIpLiveStreamingRtpUdpOpenSettings rtpUdpSettings;
    bool mediaProbeOnly; /* if set, session is opened for media probe only */
    B_PlaybackIpSecurityOpenSettings securitySettings; /* Security Open Settings */
    B_PlaybackIp_EventCallback eventCallback;           /* callback function if nonBlockingMode is desired or to receive any asynchronous events from the IP library */
    void *appCtx;                                       /* app context handle used in the callback function */
    bool transportTimestampEnabled;                     /* indicates if MPEG2 TS content contains 4 byte DLNA timpstamp */
    B_PlaybackIpAppHeader appHeader;    /* app header that gets inserted before media is streamed out */
    bool disableIndexGeneration; /* if set, nav index is not generated during media probing */
    NEXUS_HeapHandle heapHandle; /* nexus heap Handle, can be null if app doesn't have a preference */
} B_PlaybackIpFileStreamingOpenSettings;

/***************************************************************************
Summary:
This function starts streaming the content from local file to a network client.
***************************************************************************/
B_PlaybackIpFileStreamingHandle
B_PlaybackIp_FileStreamingOpen(
    const B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings
    );

/***************************************************************************
Summary:
This function stops streaming content from a file.
***************************************************************************/
void 
B_PlaybackIp_FileStreamingClose(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle
    );

B_PlaybackIpError
B_PlaybackIp_FileStreamingStart(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle
    );

void 
B_PlaybackIp_FileStreamingStop(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle
    );

B_PlaybackIpError
B_PlaybackIp_FileStreamingGetMediaInfo(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    B_PlaybackIpPsiInfoHandle psi
    );

typedef struct B_PlaybackIpFileStreamingStatus 
{
    off64_t bytesStreamed;
    B_PlaybackIpConnectionState connectionState;
} B_PlaybackIpFileStreamingStatus;

void 
B_PlaybackIp_FileStreamingGetStatus(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    B_PlaybackIpFileStreamingStatus *status 
    );

#ifdef __cplusplus
}
#endif
#endif /* #ifndef B_PLAYBACK_IP_LIB_H__ */
