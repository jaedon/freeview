/***************************************************************************
*     (c)2003-2012 Broadcom Corporation
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
* $brcm_Workfile: b_playback_ip_priv.h $
* $brcm_Revision: 104 $
* $brcm_Date: 11/9/12 3:22p $
*
* Description: IP Applib Private Definitions
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_priv.h $
* 
* 104   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 103   10/30/12 10:12a ssood
* SW7231-858: additional flags to handle PTS discontinuity case
* 
* 102   10/16/12 5:17p ssood
* SW7231-858: increased IP_HALT_TASK_TIMEOUT to 5 sec to allow thread
*  synchronization to correctly work + added a variable to track when we
*  are in the hls seek transition logic + added state to track the pts
*  during the last discontinuity
* 
* 101   9/7/12 5:52p ssood
* SW7420-2358: fixed bug where seek to 0 offset would seek to current
*  position
* 
* 100   9/6/12 4:17p ssood
* SW7425-3673: added support for seek/pause/play of HLS sessions
* 
* 99   8/28/12 2:35p ssood
* SW7231-974: extend playback_ip current position logic to handle PTS
*  discontinuities during normal playback
* 
* 98   8/3/12 2:52p ssood
* SW7425-3678: extend ip lib & ip streamer to support streaming out live,
*  file, encoded content over RTP/UDP
* 
* 97   7/31/12 4:18p ssood
* SW7346-347: code to cache & re-use HTTP cookie + fixes for Http Xfer
*  encoding + debug related additions
* 
* 96   7/17/12 5:15p gskerl
* SW7346-853: Merged from branch SW7346-853 to main.
* 
* SW7346-853/5   7/17/12 11:52a gskerl
* SW7346-853: Added prototype for
*  B_PlaybackIp_XmlElem_PrintRecursively().
* 
* SW7346-853/4   7/16/12 5:39p gskerl
* SW7346-853: Added prototype for B_PlaybackIp_XmlElem_GetTag().
* 
* SW7346-853/3   7/10/12 5:17p gskerl
* SW7346-853: Define some structs when there is either HLS or MPEG-DASH
*  support.
* 
* SW7346-853/2   7/6/12 10:37a gskerl
* SW7346-853: Added prototypes for C helper funcs ChildData(),
*  FirstChild(), NextChild() and FindNextChild().
* 
* SW7346-853/1   6/20/12 6:46p gskerl
* SW7346-853: Added structures for MPEG-DASH. Added prototypes for XML
*  parsing wrapper functions in b_playback_ip_xml_helper.cpp.
* 
* 95   6/8/12 7:11p ssood
* SW7425-3201: Add support for new DLNA PlaySpeed trickmode options in
*  playback ip library
* 
* 94   6/5/12 6:46p ssood
* SW7231-766: add support for downloading encrypted HLS keys + support
*  for HLS spec version 3
* 
* 93   5/23/12 3:13p ssood
* SW7425-2894: suppressing extra debug log from ip client modules
* 
* 92   4/3/12 12:31p ssood
* SW7425-2783: added support to handle HTTP redirects during HLS sessions
* 
* 91   2/9/12 4:49p ssood
* SW7425-2319: Add support for IP Tuner Lock/Unlock mechanism
* 
* 90   1/24/12 11:31a ssood
* SW7425-1331: insert encrypted (DTCP/IP) special data while streaming
*  out media files
* 
* 89   1/17/12 5:40p ssood
* SW7231-603: limit media probe time
* 
* 88   11/23/11 2:10p ssood
* SW7408-109: RVU Trickplay support: added FFWD & FRWD
* 
* 87   9/26/11 10:22a ssood
* SW7346-341: Add PVR Decryption support for file streaming
* 
* 86   9/19/11 1:50p ssood
* SW7425-1040: added logic to better handle network congestion
* 
* 85   8/27/11 6:30p ssood
* SW7425-1077: throttle server streaming thread at lower speeds to avoid
*  starvation of local client trickplay at lower speed
* 
* 84   6/6/11 1:28a ssood
* SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
*  eventCallback support for end of stream playback
* 
* 83   5/26/11 11:31p ssood
* SW7231-166: dont create Nexus File Handle for cases where Nexus
*  playpump will get used
* 
* 82   3/17/11 4:36p ssood
* SW7420-1616: initial check-in for pvr encryption support
* 
* 81   1/2/11 5:26p ssood
* SW7420-1257: added support to stream encoded frame using unicast for
*  local client
* 
* 80   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 79   12/8/10 3:57p ssood
* SW7420-1217: refined support for playing unbounded streams
* 
* 78   11/24/10 2:59p ssood
* SW7420-1257: Video Conferencing Demo: add support for receiving input
*  camera input and stream it out
* 
* 77   11/23/10 11:55a ssood
* SW7420-1257: initial check-in for rtp/udp streaming support in
*  ip_streamer for Video Conferencing Demo
* 
* 76   11/11/10 10:34a ssood
* SW7420-1217: added support for playing unbounded (live) streams
* 
* 75   10/29/10 9:05a ssood
* SW7420-1217: basic support to play both variant & non-variant bounded
*  playlists containing clear content
* 
* 74   10/27/10 8:28p ssood
* SW7420-1217: initial check-in for HLS support
* 
* 73   10/8/10 6:07p ssood
* SW7420-1088: add support for time based seeks
* 
* 72   10/6/10 6:27p ssood
* SW7420-1014: sometimes video will restart when changing FF rate + pause
*  macroblocking fix
* 
* 71   9/28/10 11:29a junyel
* SW7420-1115: cross-platform code for DMS
* 
* 70   9/27/10 3:41p junyel
* SW7420-1115: cross-platform code for DMS
* 
* 68   9/17/10 8:21a ssood
* SWGERRARD-547: support to expose multiple multiple programs and tracks
*  within a multi-program stream
* 
* 67   8/27/10 3:59p ssood
* SW7420-502: added partial support to play RVU sessions in live mode
* 
* 66   8/5/10 2:47p jjordan
* SW7405-4205: add socket flush to facilitate accurate targetting of TTS
*  buffer fullness at start of decode
* 
* 65   7/23/10 11:10a ssood
* SW7420-502: RVU client: fixed subtle issues w/ playing DTCP encrypted
*  HTTP Chunk encoded content from HMC30 server
* 
* 64   6/16/10 5:05p ssood
* SW7420-716: initial RVU trickmode related check-ins
* 
* 63   5/6/10 3:54p ssood
* SW7420-561: increased the default size of streaming buffer to avoid re-
*  allocation of buffer during streaming
* 
* 62   4/21/10 11:13a ssood
* SW7420-561: cleaned up the request & response buffer management logic
* 
* 61   4/2/10 5:56p ssood
* SW7468-198: Coverity fixes + RTSP related changes (requested by
*  Philips)
* 
* 60   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/3   3/1/10 7:55a ssood
* SW7420-561: added support to dynamically adjust for DTCP PCP header +
*  plus fixed a bug causing periodica macroblocking when HTTP Chunk Xfer
*  Encoding was used w/ DTCP/IP Decryption
* 
* SW7420-561/2   2/18/10 12:43a ssood
* SW7420-561: added checks to validate IP state during IP API calls
* 
* SW7420-561/1   2/16/10 10:11a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 59   1/29/10 6:13p ismailk
* SW7420-559: Enable EROUTER_SUPPORT flag to enable the WAN unicast
*  ioctl.
* 
* 58   1/7/10 10:48a jjordan
* SW7405-3174: increase "IP_MAX_ITEMS" multiplier to allow RTP reordering
*  to function
* 
* 57   12/22/09 11:03a ssood
* SW7420-454: add timeshift support for live streaming
* 
* 56   12/12/09 6:34p ssood
* SW7420-502: only process current chunk header end & next chunk header
*  begining when >2 bytes have been read
* 
* 55   12/10/09 6:52p ssood
* SW7420-502: track number of HTTP xfer chunks
* 
* 54   11/17/09 3:48p ssood
* SW7420-454: Enhance IP library to stream files from local disk
* 
* 53   10/22/09 12:55p ssood
* SW7420-340: converted streaming socket to non-blocking to avoid getting
*  blocked when client silently goes away
* 
* 52   10/7/09 3:48p ssood
* SW7420-340: added code to monitor encryption time related stats
* 
* 51   10/6/09 4:58p ssood
* SW7420-340: added support for streaming DTCP/IP encrypted stream to > 1
*  clients + fixed compile errors for clear channels
* 
* 50   10/6/09 12:18p ssood
* SW7420-340: Add DTCP/IP support to ip_streamer & IP Applib
* 
* 49   10/2/09 4:23p ssood
* SW7420-340: temporary work around to handle network error condition
*  upon write to socket
* 
* 48   10/1/09 10:51a jjordan
* SW7405-3107: add IP recv timeout status to Settop API
* 
* 48   10/1/09 10:51a jjordan
* SW7405-3107: add IP recv timeout status to Settop API
* 
* 48   10/1/09 10:15a jjordan
* SW7405-3107: add IP recv timeout status to Settop API
* 
* 47   9/21/09 12:09p ssood
* SW7405-2559: Adding LPCM playback support
* 
* 46   9/16/09 4:50p ssood
* SW7420-340: workaround for getting audio to work on live channels
*  coming over HTTP, delay the playback by 2 sec
* 
* 45   8/24/09 3:23p ssood
* SW7405-1689: adapting server based trickmodes to Nexus playback changes
*  due to OTF support
* 
* 44   8/14/09 5:00p ssood
* PR50202: First check-in for clientside trickmode support for MPEG2 TS
*  format
* 
* 43   8/6/09 10:18a ssood
* PR43746: added support for calculating more precise bitrates for TS &
*  VOB files (support index reads from middle & end of content)
* 
* 42   7/29/09 6:14p ssood
* PR43746: extend IP trick modes to support Disconnect And Seek based
*  paused operations (DLNA requirement)
* 
* 41   7/21/09 4:18p ssood
* PR43746: Further work for Server based HTTP trick mode support
* 
* 40   7/16/09 12:59p ssood
* PR43746: Add Server based HTTP trick mode support
* 
* 39   6/22/09 1:44p ssood
* PR55417: Precharging support for TS/VOB formats: moved the HTTP
*  data/index cache allocations to IP Applib Open time, lowered default
*  cache sizes
* 
* 38   6/21/09 8:55p ssood
* PR55417: switching to 1 data cache model to lower memory usage of HTTP
*  module
* 
* 37   6/18/09 10:24a ssood
* PR55417: provide http return code code via the GetStatus to app
* 
* 36   6/16/09 3:00p ssood
* PR55417: accounting for fudge factor in max buffer depth calculations
* 
* 35   6/15/09 11:36p ssood
* PR55417: fixing rewind related bug
* 
* 35   6/15/09 11:31p ssood
* PR55417: fixing rewind related bug
* 
* 34   6/15/09 9:23p lwhite
* PR53773: Add Netio shutdown
* 
* 33   6/10/09 8:36p lwhite
* PR53773: Add RadEa time/byte offset
* 
* 32   6/7/09 11:45p ssood
* PR55417: Adding support for runtime buffering of HTTP content (used
*  during seeks & n/w loss events)
* 
* 31   6/5/09 1:35p ssood
* PR55626: DLNA guidelines mandate client to only maintain no more than 1
*  connection to server, we had one connx per data cache, changed that to
*  closing/opening 1 connx
* 
* 30   5/29/09 6:18p ssood
* PR55325: run time buffer management: support reading less than a data
*  chunk (can happen due to temporary n/w loss)
* 
* 29   5/28/09 4:54p ssood
* PR55325: allow app to specify n/w timeout & buffer cache size
* 
* 28   5/25/09 11:30p ssood
* PR55325: Add support for pre-charging network buffer for HTTP playback
* 
* 27   5/14/09 6:26p ssood
* PR53773: lowered the socket receive q size, otherwise system becoming
*  unstable due to running out of skbs
* 
* 26   5/8/09 4:36p jjordan
* PR54886: fix TTS initial buffer depth
* 
* 25   4/21/09 3:32p ssood
* PR53773: handle cases where some security protocols dont encrypt HTTP
*  response header
* 
* 24   4/20/09 3:02p ssood
* PR50311: Added support for playing longer duration (> 1hr) MP4 files:
*  need to keep two data caches (one each for Audio & Video)
* 
* 23   4/10/09 3:49p lwhite
* PR53773: Suspend support added
* 
* 22   4/6/09 6:09p ssood
* PR53773 : refactored basic TCP socket functions for reuse by other
*  internal modules
* 
* 21   4/2/09 11:37p ssood
* PR53773 : changes to support security framework for SSL, DTCP-IP,
*  RAD/ES protocols
* 
* 20   3/27/09 1:52p ssood
* PR48146: add support for MP4 that have index data (MDAT object) at the
*  end
* 
* 19   3/19/09 2:16p ssood
* PR48146: add rewind support for MP4
* 
* 18   3/10/09 3:08p ssood
* PR50311: added support to rewind HTTP session
* 
* 17   3/5/09 9:59p ssood
* PR50311: fix for differentiating between EOF & other network errors
* 
* 16   3/5/09 10:40a ssood
* PR50311: Adding support for handling larger response messages
* 
* 15   3/4/09 12:51p ssood
* PR50311: added counter to track total bytes read
* 
* 14   3/2/09 2:44p ssood
* PR50311: brutus quit crashes when we quit from a HTTP channel where
*  connect attempt had failed
* 
* 13   2/27/09 11:55a ssood
* PR50311: Adding support for HTTP Chunk Transfer Encoding
* 
* 12   12/5/08 10:00a lwhite
* PR50000: Fixed HTTP memory leak
* 
* 11   12/4/08 10:13a jjordan
* PR49937: Add support for RTP over TTS
* 
* 10   10/31/08 12:30p jjordan
* PR47230: Add TTS Pacing
* 
* 9   10/27/08 11:57a ssood
* PR45431: adding MP4 & ASF playback support over HTTP
* 
* 8   10/13/08 5:40p ssood
* PR47521: improved support for low-bit rate streams
* 
* 7   8/18/08 11:05a ssood
* PR45431: Fixed RTP to work standard sockets
* 
* 6   8/11/08 10:13a ssood
* PR45431: Add HTTP playback support for ASF content
* 
* 5   8/1/08 3:35p lwhite
* PR42739: Increase IP items for Bitband server delay
* 
* 4   7/28/08 3:27p ssood
* PR42739: further changes for supporting example app for IP Applib
* 
* 3   7/21/08 4:06p ssood
* PR42739: code changes to simplify ip app lib interface
* 
* 2   7/14/08 4:25p lwhite
* PR42739: Added RTCP support
* 
* 1   6/13/08 7:29p ssood
* PR42739: Adding new files for HTTP playback
* 
***************************************************************************/
#ifndef __B_PLAYBACK_IP_PRIV__
#define __B_PLAYBACK_IP_PRIV__

#include <fcntl.h>

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
#include <sys/uio.h>
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

#include "b_os_lib.h"
#include "bdbg.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bpool.h"
#include "barena.h"
#include "bioatom.h"
#include "blst_queue.h"
#include "brtp_spf.h"
#include "bfile_io.h"
#include "bfile_buffered.h"
#include "nexus_types.h"
#include "nexus_file.h"
#include "nexus_file_pvr.h"
#include "bmedia_probe.h"
#include "bmedia_player.h"
#include "bmedia_util.h"
#include "b_playback_ip_tts_throttle.h"

#ifdef B_HAS_DTCP_IP
#include "b_dtcp_applib.h"
#include "b_dtcp_constants.h"
#define ENCRYPTION_PADDING  DTCP_CONTENT_PACKET_HEADER_SIZE + HTTP_AES_BLOCK_SIZE-1                     	
#endif
#define STREAMING_BUF_SIZE (DIO_BLK_SIZE*188*4)
#define STREAMING_BUF_MULTIPLE 32 /* can't be less than this as read size has to be DIO aligned */
#define HTTP_AES_BLOCK_SIZE 16
#define HTTP_DEFAULT_PORT 80

#ifdef B_HAS_NETACCEL
#include "bnetaccel_info.h"
#endif

#define RTP_PAYLOAD_TYPE_MP2T			33			/* MPEG2 TS stream */
#define RTP_PAYLOAD_TYPE_ES			    97			/* ES streams */
#define MAX_READ						160
#define TEST_BUFFER_SIZE				65536
#define IP_MAX_PWM_VALUE				0x7FFF
#define IP_MIN_PWM_VALUE				(-IP_MAX_PWM_VALUE)
#define IP_POSTCHARGE_PERIOD_MSEC		5000
#define IP_MAX_DECODE_POLLS 			10
#define IP_UNDERFLOW_PERIOD_MSEC		100

/* slew 270 Hz (10 ppm) @ 100 Hz/sec, 
 * in steps of 10Hz 
 */
#define IP_PWM_STEP_PERIOD				100			/* 0.1 second */
#define IP_PWM_STEP_SIZE				75			/* 10Hz (Hz/bit = 4386/0x7fff = 0.13385 Hz/bit, ie. 0.00494 ppm/bit) */
#define IP_PWM_STEPS_PER_VIOLATION		27

#if defined (LINUX)
  #define STATUS_REPORTING_CONSTANT 	1
  #define NEW_RMEM_MAX					(1*256*1024)
#else
  #define STATUS_REPORTING_CONSTANT 	0
#endif  

#if 0
#define BDBG_MSG_FLOW(x)  BDBG_WRN( x );
#else
#define BDBG_MSG_FLOW(x)  
#endif

/* #define B_RECORD_IP */
#ifdef B_RECORD_IP
  #include <stdio.h>
#endif

#ifdef __vxworks
#if !defined (USE_PTHREAD)
    typedef int pthread_t;
#endif
#endif

/**
* Playback IP module can receive IP encapsulated AV data from
* network using following 3 compile options:
*       -use normal sockets (true when B_HAS_NETACCEL &
*        B_HAS_PLAYPUMP_IP is not defined)
*       -use legacy IP Playpump (true when B_HAS_PLAYPUMP_IP is
*        defined), this i/f is being deprecated)
*       -use recommended accelerated sockets (which uses
*        Broadcom's Accelerated Sockets i/f) (true when
*        B_HAS_NETACCEL is defined & B_HAS_PLAYPUMP_IP is not
*        defined).
*/                       
#define PKTS_PER_CHUNK 8		/* how many packets to process at one time */
#if !defined(B_HAS_NETACCEL) || defined(B_HAS_PLAYPUMP_IP)
#define PKTS_PER_READ 1		            /* For normal sockets, UDP allows only 1pkt/recvfrom call*/
#else
#define PKTS_PER_READ PKTS_PER_CHUNK	/* Accelerated Sockets allows packet aggregation */
#endif
#define IP_MAX_ITEMS	(PKTS_PER_CHUNK * 128)/* max number of packets that can be pending in the filter */
#define IP_MAX_PKT_SIZE	1400
#define TMP_BUF_SIZE	1400
#define DISCARD_BUFFER_SIZE IP_MAX_PKT_SIZE*200

#define TS_PKT_SIZE					188
#define TS_PKTS_PER_UDP_PKT			7
#define	UDP_PAYLOAD_SIZE			TS_PKT_SIZE * TS_PKTS_PER_UDP_PKT
#define RTP_PAYLOAD_SIZE			UDP_PAYLOAD_SIZE
#define IP_HALT_TASK_TIMEOUT_MSEC	5000
#define IP_RECV_TIMEOUT_USEC		250000
#define IP_RECV_BURST_USEC			(IP_RECV_TIMEOUT_USEC/5)

#define abs(x) (x>0 ? x : -x)
#define MAX_BUFFER_SIZE ((1316+28) * PKTS_PER_CHUNK)
/* Note: we used to maintain two separate data caches for supporting MP4 files where AV can be in far offsets */
/* However, such a support required double the memory. Also, since most common MP4 will have AV interleved */
/* within few seconds, so we may not need the 2nd cache. This also saves about 12MB of memory used by 2nd cache */
#define HTTP_MAX_DATA_CACHES 1
#define HTTP_SELECT_TIMEOUT 60  /* DLNA requirement */
#define HTTP_DATA_CACHE_CHUNK_SIZE 96000
#define HTTP_INDEX_CACHE_CHUNK_SIZE 16384
/* during runtime pre-charging, we keep a certain amount of previous data in the cache as for MP4 player can request some of the previous data */
#define HTTP_CACHE_DEPTH_FUDGE_FACTOR (10 * HTTP_DATA_CACHE_CHUNK_SIZE)
            
/* default cache sizes */
#define HTTP_INDEX_CACHE_SIZE (HTTP_DATA_CACHE_CHUNK_SIZE * 20)
#define HTTP_DATA_CACHE_SIZE (HTTP_DATA_CACHE_CHUNK_SIZE * 20)

/* timeout for app thread to wait for acknowledgement from HTTP thread of starting or stopping pre-charging */
#define HTTP_PRE_CHARGE_EVENT_TIMEOUT 6000

#define B_PLAYBACK_IP_INFO_FILE_EXTENSION ".info"
#define B_PLAYBACK_IP_INDEX_FILE_EXTENSION ".nav"
#define DIO_BLK_SIZE		(512)
#define DIO_MASK			(DIO_BLK_SIZE - 1)
#define DIO_ALIGN(x) (unsigned char *)(((unsigned long) x + DIO_MASK) & ~DIO_MASK)

#ifndef timeAdd
    #define timeAdd(_a, _b, _res)              \
  do {                          \
    (_res)->tv_usec = (_a)->tv_usec + (_b)->tv_usec;    \
    (_res)->tv_sec = (_a)->tv_sec + (_b)->tv_sec;   \
    if ((_res)->tv_usec >= 1000000)         \
      {                         \
        (_res)->tv_usec -= 1000000;         \
        (_res)->tv_sec++;               \
      }                         \
  } while (0)
#endif

#ifndef timeSub
    #define timeSub(_a, _b, _res)              \
  do {                          \
    (_res)->tv_usec = (_a)->tv_usec - (_b)->tv_usec;    \
    (_res)->tv_sec = (_a)->tv_sec - (_b)->tv_sec;   \
    if ((_res)->tv_usec < 0) {              \
      (_res)->tv_usec += 1000000;           \
      (_res)->tv_sec--;                 \
    }                           \
  } while (0)
#endif

#ifdef EROUTER_SUPPORT
#define IOCTL_ADD_DELETE_WANUNICAST_PACKET  816
#define WAN_PORT_DELETE						0
#define WAN_PORT_ADD						1
typedef struct ioctl_if_list_portnums {
    unsigned short add;			// 0 - delete, 1 add
    unsigned short portNumb;
} IOCTL_IF_LIST_PORTNUMS;
#endif

typedef struct B_PlaybackIp_GlobalContext{
    B_SchedulerHandle scheduler;
    B_ThreadHandle schedulerThread;
}B_PlaybackIp_GlobalContext;


/**
Summary:
This struct defines the fields of the RTP header
 **/
/* # of rtp header cached for RTCP stats purpose */
#define B_MAX_RTP_HEADERS              4000
typedef struct B_PlaybackIp_RtpStatsHeader {
    uint8_t version;
    uint8_t padding;
    bool extension;
    uint8_t csrc_count;
    uint8_t marker;
    uint8_t payload_type;
    uint16_t sequence_num;
    uint32_t timestamp;
    uint32_t ssrc;

    struct {
        uint16_t defined_by_profile;
        uint16_t length;
    } extension_header;
} B_PlaybackIp_RtpStatsHeader;

typedef struct B_PlaybackIp_RtpHeaderData {
    B_PlaybackIp_RtpStatsHeader header[B_MAX_RTP_HEADERS];
    unsigned entry_cnt;
    BKNI_MutexHandle lock;
} B_PlaybackIp_RtpHeaderData;

#define RTP_FIXED_HEADER_SIZE 12

typedef enum B_PlaybackIp_RtspCtrlCmdType {
    B_PlaybackIp_RtspCtrlCmdType_Play,
    B_PlaybackIp_RtspCtrlCmdType_Pause,
    B_PlaybackIp_RtspCtrlCmdType_Resume
} B_PlaybackIp_RtspCtrlCmdType;

typedef struct B_PlaybackIp_RtspCtrlParams {
    long start;
    long end;
    long scale;
} B_PlaybackIp_RtspCtrlParams;

typedef struct B_PlaybackIpRtspCtrlCmd {
    B_PlaybackIp_RtspCtrlCmdType type;
    B_PlaybackIp_RtspCtrlParams params;
} B_PlaybackIp_RtspCtrlCmd;

/**
* Summary:
* State of the software dejitter buffer
* (used only for software managed TSM case)
**/
typedef enum B_PlaybackIpBufferState {
    B_PlaybackIpBufferState_eInit,
    B_PlaybackIpBufferState_ePreCharging,
    B_PlaybackIpBufferState_ePostCharging,
    B_PlaybackIpBufferState_ePlaying,
    B_PlaybackIpBufferState_eOverflowing,
    B_PlaybackIpBufferState_eUnderflowing
} B_PlaybackIpBufferState;

/**
* Summary:
* structure to hold the incoming packets
* (contains either one packet (for standard socket or multiple
* packets for accelerated sockets)
**/
struct B_PlaybackIpItem;	/* forward declaration */
BLST_Q_HEAD(B_PlaybackIpItemQueue, B_PlaybackIpItem);
typedef struct B_PlaybackIpItem 
{
    BLST_Q_ENTRY(B_PlaybackIpItem) item_queue;
    BLST_Q_ENTRY(B_PlaybackIpItem) active_item_queue;
    uint8_t *data;			/* Netaccel aggregates incoming packets, malloc buffer for multiple pkts */
    B_PlaybackIpHandle playback_ip;
    int item_index;
    bool block_end;
    struct B_PlaybackIpItem *block_start; /* points to the 1st item in a item block */
                                        /* For Accelerated socket, item block logically groups PKTS_PER_CHUNK items into one block */
                                        /* as accelerated sockets need a contiguous buffer for reading multiple pkts at once*/
}B_PlaybackIpItem; 

/* implementation of bfile_io_read */
struct bfile_io_read_net {
    struct bfile_io_read self;
    B_PlaybackIpHandle playback_ip;
    int fd;
    off_t offset;
    bool socketError;   /* set for any of these conditions: 1) n/w error (EPIPE), 2) server closed, 3) bytes read != bytes asked 4) EOF */
};

/* implementation of NEXUS_FilePlay */
typedef struct bfile_in_net {
    struct NEXUS_FilePlay self;
    struct bfile_io_read_net data;
    struct bfile_io_read_net index;
} bfile_in_net;

/* implementation of bfile_io_write */
struct bfile_io_write_net {
    struct bfile_io_write self;
    B_PlaybackIpHandle playback_ip;
    int fd;
    int writeQueueSize; /* socket write queue size */
    int selectTimeouts; /* number of consecutive select timeouts indicating socket write q full event */
    int writeQueueFullTimeouts; /* number of consecutive times we were unable to stream due to write q being full */
    bool liveStreaming; /* true if we are streaming a live channel */
    B_PlaybackIpProtocol streamingProtocol; /* HTTP, RTP, or UDP */
    struct sockaddr_in streamingSockAddr;   /* socket address info of streaming destination */
    struct sockaddr_in streamingSockAddrLocal;   /* socket address info of streaming destination (local client) */
    int streamingFdLocal;   /* socket fd info of streaming destination (local client) */
    int rtpPayloadType; /* value of payload type field of the outgoing RTP header */
    unsigned short nextSeq; /* next seq # for RTP protocol */
    struct iovec *iovec;
    char *interfaceName;
#ifdef B_HAS_DTCP_IP
    void *dtcpCtx;
    void *dtcpStreamHandle;
    void *dtcpAkeHandle;
    unsigned char *encryptedBuf;
    unsigned int encryptedBufSize;
    int residualBytesLength;    /* # of bytes that were not encrypted in the previous encrypt operation due to AES block size limitation of 16 bytes */
    char residualBytesToEncrypt[ENCRYPTION_PADDING]; /* actual bytes to encrypt */
#endif
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    NEXUS_KeySlotHandle pvrDecKeyHandle; /* handle for pvr decryption */
    NEXUS_DmaHandle dmaHandle;
    NEXUS_DmaJobHandle dmaJobHandle;
    BKNI_EventHandle event;
#endif
    uint64_t totalBytesConsumed;
    struct timeval totalTime;
    struct timeval minTime;
    struct timeval maxTime;
    uint32_t totalIterations;
    bool stopStreaming; /* this flag is set when app wants streaming thread to stop streaming */
    NEXUS_HeapHandle heapHandle;
};

/* implementation of NEXUS_FileRecord */
typedef struct bfile_out_net {
    struct NEXUS_FileRecord self;
    struct bfile_io_write_net data;
    struct bfile_io_write_net index;
} bfile_out_net;

typedef struct B_PlaybackIpNetIo {
    int (*read)(void *securityHandle, B_PlaybackIpHandle playback_ip, int sd, char *rbuf, int rbuf_len); /* method to read data */
    int (*write)(void *securityHandle, volatile B_PlaybackIpState *playbackIpState, int sd, char *wbuf, int wbuf_len); /* method to write data */
    void (*suspend)(void *securityHandle); /* method to suspend session. Ip socket closed, but security session intact */
    void (*close)(void *securityHandle); /* method to close security  session. */
    void (*shutdown)(void *securityHandle, unsigned int flag); /* method to shutdown security  session. */
} B_PlaybackIpNetIo;

typedef struct B_PlaybackIpAvCacheStats {
    unsigned int hits;
    unsigned int partialHits;
    unsigned int cacheable;
    unsigned int miss;
}B_PlaybackIpAvCacheStats; 

/* Cache structure used to hold index & data */
typedef struct B_PlaybackIpAvCache {
    char *cache; /* pointer to buffer where actual data is cached */
    bool inUse;  /* true if data cache is being used */
    volatile unsigned int depth; /* how many bytes are currently cached */
    volatile unsigned int size; /* max bytes that can be cached */
    off_t startOffset; /* linear offset of current first byte in data cache */
    off_t endOffset;   /* linear offset of current last byte in data cache */
    off_t firstOffset;   /* 1st data offset asked by the player, used to detect rewind condition */
    unsigned int writeIndex; /* byte offset in data cache where next byte (from n/w) can be written into */
    unsigned int readIndex; /* byte offset in data cache where startOffset starts */
    B_Time lastUsedTime; /* time in msec when this cache was last used */
    B_PlaybackIpAvCacheStats stats; /* cache performance stats */
} B_PlaybackIpAvCache;


#if defined (B_HAS_HLS_PROTOCOL_SUPPORT) || defined (B_HAS_MPEG_DASH_PROTOCOL_SUPPORT)

typedef struct EncryptionInfo {
    bool useMediaSegSeqAsIv;
    union {
        struct {
           bool none;
        } none;
        struct {
            unsigned char key[16]; /* 128 bit key for AES-128 */
            unsigned char iv[16]; /* 128 bit IV for AES-128 */
        } aes128;
    }encryptionMethod;
} EncryptionInfo;

/*struct MediaFileSegmentInfo;*/	/* forward declaration */
typedef struct MediaFileSegmentInfo{
    BLST_Q_ENTRY(MediaFileSegmentInfo) next;
    char *absoluteUri; /* complete URI string of the media segment */
    char *uri; /* file name part of the media file segment uri */
    char *server; /* pointer to the server name of the media segment uri */
    unsigned port; /* port number of the media segment uri */
    B_PlaybackIpProtocol protocol;
    B_PlaybackIpSecurityProtocol securityProtocol;  /* security protocol being used */
    NEXUS_PlaybackPosition duration; /* duration of media file segment in milli-seconds */
    char *title; /* media segment title */
    char *date; /* date & time associated with a URI */
    char *time;
    int mediaSequence; /* number of this uri in the media stream presentation */
    bool markedDiscontinuity; /* need to reset the decoders when starting playback from this point */
    /* security *sec; */ /* security keys associated with this uri */
    EncryptionInfo encryptionInfo; /* stores info for decrypting the media */
} MediaFileSegmentInfo;
BLST_Q_HEAD(MediaFileSegmentInfoQueue, MediaFileSegmentInfo);

/* list of playlist files built from a variant playlist file */
typedef struct PlaylistFileInfo{
    BLST_Q_ENTRY(PlaylistFileInfo) next;    /* next playlist file */
    MediaFileSegmentInfo *currentMediaFileSegment; /* pointer to the current segment being downloaded */
    struct MediaFileSegmentInfoQueue mediaFileSegmentInfoQueueHead; /* queue of media segments belonging to this playlist file */
    /* playlist file attributes */
    char *absoluteUri; /* complete URI string of the playlist file */
    char *uri;  /* file name part of the playlist file uri */
    char *server; /* pointer to the playlist server name uri */
    unsigned port; /* port number of the playlist server uri */
    B_PlaybackIpProtocol protocol;
    short version; /* number (1 by default, 2 w/ AES) */
    short programId; /* number that uniquely identifies a particular presentation within the scope of a playlist file */
    unsigned bandwidth; /* in bps (0 if the variant playlist file doesn't exist) */
    NEXUS_PlaybackPosition totalDuration; /* playlist duration in milli-seconds, sum of durations of media files within a playlist */
    NEXUS_PlaybackPosition maxMediaSegmentDuration; /* max duration of any media stream segment: in msec */
    int mediaSegmentBaseSequence; /* base sequence number of the media sequence in the current playlist */
    bool allowCaching; /* if set, media stream can be cached (i.e. recorded) */
    int initialMinimumReloadDelay; /* duration of the last media file in playlist: used the interval for re-downloading the unbounded playlist */
    /*
       -codecs: in RFC4281 format
       -resolution:HxV
    */
    /* state needed during playback */
    bool bounded; /* (set if end tag is present) */
    int lastUsedUriIndex; /* index of the last URI being downloaded */
    int numMediaSegments; /* number of media segments in a playlist */
    bool playlistHasChanged; /* set when playlist re-download has refreshed the playlist */
} PlaylistFileInfo;
BLST_Q_HEAD(PlaylistFileInfoQueue, PlaylistFileInfo);

#endif  /*  defined (B_HAS_HLS_PROTOCOL_SUPPORT) || defined (B_HAS_MPEG_DASH_PROTOCOL_SUPPORT) */

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT

#define HLS_NUM_SEGMENT_BUFFERS 2
typedef struct HlsSegmentBuffer {
    char *buffer;
    int bufferSize;
    int bufferDepth;
    bool filled;    /* set when the buffer is filled w/ media segment */
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to this structure by HLS download & playback threads */
    bool markedDiscontinuity;
}HlsSegmentBuffer;

typedef struct HlsSessionState {
    struct PlaylistFileInfoQueue playlistFileInfoQueueHead;
    PlaylistFileInfo *currentPlaylistFile;
    char *playlistBuffer;
    int playlistBufferSize;
    int playlistBufferDepth;
    unsigned int playlistBufferReadIndex;
    B_ThreadHandle hlsSegmentDownloadThread;
    bool hlsSegmentDownloadThreadDone; /* set when download thread either gets an error or is done downloading all media segments in a playlist */
    bool hlsPlaybackThreadDone;
    B_ThreadHandle hlsPlaylistReDownloadThread;
    bool hlsPlaylistReDownloadThreadDone;
    HlsSegmentBuffer segmentBuffer[HLS_NUM_SEGMENT_BUFFERS];
    BKNI_EventHandle bufferFilledEvent;
    BKNI_EventHandle bufferEmptiedEvent;
    BKNI_EventHandle reDownloadPlaylistEvent;
    BKNI_EventHandle segDownloadThreadPausedEvent;
    BKNI_EventHandle playbackThreadPausedEvent;
    BKNI_EventHandle segDownloadThreadPauseDoneEvent;
    BKNI_EventHandle playbackThreadPauseDoneEvent;
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to this structure by Nexus IO, App, Http threads */
    int networkBandwidth; /* network b/w that is achieved while downloading a media segment */
    int playlistSocketFd;   /* socket file descriptor of the playlist file */
    bool downloadedAllSegmentsInCurrentPlaylist; /* set when all media segments in the current playlist have been downloaded: used to detect when to switch playlists in case base seq # wraps around */
    bool resetPlaylist; /* set to reset the playlist so that we start download from the 1st segment of the playlist */
    unsigned maxNetworkBandwidth; /* max network bandwidth set via max_nw_bw */
    bool seekOperationStarted; /* set when HLS specific seek work is in progress */
}HlsSessionState;
#endif

#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT

#define MPEG_DASH_NUM_SEGMENT_BUFFERS 2
typedef struct MpegDashSegmentBuffer {
    char *buffer;
    int bufferSize;
    int bufferDepth;
    bool filled;    /* set when the buffer is filled w/ media segment */
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to this structure by download & playback threads */
    bool markedDiscontinuity;
}MpegDashSegmentBuffer;

typedef struct MpegDashSessionState {
    struct PlaylistFileInfoQueue playlistFileInfoQueueHead;
    PlaylistFileInfo *currentPlaylistFile;
    char *playlistBuffer;
    int playlistBufferSize;
    int playlistBufferDepth;
    unsigned int playlistBufferReadIndex;
    B_ThreadHandle mpegDashSegmentDownloadThread;
    bool mpegDashSegmentDownloadThreadDone; /* set when download thread either gets an error or is done downloading all media segments in a playlist */
    bool mpegDashPlaybackThreadDone;
    B_ThreadHandle mpegDashPlaylistReDownloadThread;
    bool mpegDashPlaylistReDownloadThreadDone;
    MpegDashSegmentBuffer segmentBuffer[MPEG_DASH_NUM_SEGMENT_BUFFERS];
    BKNI_EventHandle bufferFilledEvent;
    BKNI_EventHandle bufferEmptiedEvent;
    BKNI_EventHandle reDownloadPlaylistEvent;
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to this structure by Nexus IO, App, Http threads */
    int networkBandwidth; /* network b/w that is achieved while downloading a media segment */
    int playlistSocketFd;   /* socket file descriptor of the playlist file */
    bool downloadedAllSegmentsInCurrentPlaylist; /* set when all media segments in the current playlist have been downloaded: used to detect when to switch playlists in case base seq # wraps around */
    bool resetPlaylist; /* set to reset the playlist so that we start download from the 1st segment of the playlist */
    unsigned maxNetworkBandwidth; /* max network bandwidth set via max_nw_bw */
}MpegDashSessionState;

#endif

/* TODO: cleanup this structure */
typedef struct B_PlaybackIp
{
    B_PlaybackIpSettings settings;    
    B_PlaybackIpSessionOpenSettings openSettings;
    B_PlaybackIpSessionOpenStatus openStatus;
	B_PlaybackIpSessionSetupSettings setupSettings;
	B_PlaybackIpSessionSetupStatus setupStatus;
    bool sessionSetupCompleted; /* set when session setup is successfully completed */
	B_PlaybackIpSessionStartSettings startSettings;
	B_PlaybackIpSessionStartStatus startStatus;
    char *responseMessage; /* cached response message string */
    char *requestMessage; /* buffer to build the outgoing request message */
    B_PlaybackIpSocketState socketState;
    B_PlaybackIpPsiInfo psi;
    NEXUS_TransportType http_content_type;
    bool apiInProgress; /* if set, an API call is in progress, cleared when api is completed */
    bool apiCompleted; /* if set, an API call is completed and results can be returned in next API call */
    BKNI_EventHandle liveMediaSyncEvent; /* event to wait for completion of a command submitted to live media library */
    BKNI_EventHandle playback_halt_event;
    BKNI_EventHandle preChargeBufferEvent;
    BKNI_EventHandle newJobEvent;  /* event to indicate new tasks to the worker thread (e.g. used to wake up http thread to do either buffering or trick play operations for RVU) */
    BKNI_EventHandle read_callback_event;
    B_PlaybackIpState playback_state;
#ifndef DMS_CROSS_PLATFORMS
    B_PlaybackIpNexusHandles nexusHandles;
    NEXUS_PlaypumpSettings pumpparams;
#endif /* DMS_CROSS_PLATFORMS */        
    NEXUS_FilePlayHandle nexusFileHandle;
    bfile_io_read_t bufferedFdIndex;
    /* TODO: is this needed bplayback_ip_buffer_scheme 	ip_buffer_scheme;*/
#if 0
    bstream_t 					stream;
    b_timer_t 					pwm_slew_timer;
    long						current_pwm_value;
    unsigned int				num_pwm_steps_pending;
    bool						forcing_pwm_slow;
    bool						buffer_fill_period_expired;
    bool                        ip_recording;
#endif
    B_PlaybackIpProtocol protocol;
    unsigned int payload_size;
    B_PlaybackIp_TtsThrottleHandle ttsThrottle;
    unsigned numRecvTimeouts;                   /* count of the number of receive timeouts since start of IP playback */
#ifdef LIVEMEDIA_SUPPORT
    void *lm_context;
#endif
    B_ThreadHandle playbackIpThread;
    B_ThreadHandle httpSessionSetupThread;
#if TODO
    B_PlaybackIpHandlerickmode_params trickmode_param;
    bdecode_status decode_status;
#endif
#ifdef EROUTER_SUPPORT
	int bcmrgDrvFd;
#endif
    char temp_buf[TMP_BUF_SIZE+1];  /* extra 1 byte to allow '\0' to be placed at end for string operations */
    int tempBytesToRead;    /* used for handling timeout cases during chunk header processing */
    int tempBytesRead;  
    char *discard_buf;
    void *buffer;
    size_t buffer_size;
    struct B_PlaybackIpItem 	*item;
    uint8_t *item_mem;
    struct B_PlaybackIpItemQueue active_item; /* items pending in filter */
    struct B_PlaybackIpItemQueue free_item; /* items ready for use */
    unsigned long byte_count;
    batom_factory_t factory;
    batom_pipe_t pipe_out; 	/* pipe data out from playback to filter */
    batom_pipe_t pipe_in; 	/* pipe data into playback from filter */
    brtp_t rtp; 		/* the RTP filter */
    bool socket_flush;  /* Set by the controller to request socket evacuate */
    int	initial_data_len;
    unsigned int firstPts;  /* Cached First PTS value sent by the server */
    unsigned int streamDurationUntilLastDiscontinuity; /* cumulative stream duration until the last PTS discontinuity */
    unsigned int lastUsedPts;  	/* PTS used during last trickplay operation */
    unsigned int ptsDuringLastDiscontinuity;  	/* pts of the displayed frame before the last dicontinuity */
    unsigned int firstPtsBeforePrevDiscontinuity;  	/* first pts of the contigous block before the previous discontinuity */
    bool getNewPtsAfterDiscontinuity;  	/* set during the ptsErrorCallback to indicate that we need to get new currentPts when next get position is called */
    unsigned int speed;  	/* new speed value passed by the application */
    off_t contentLength;      /* length of the content being played via HTTP */
    B_Time start_time;
    B_PlaybackIp_RtpHeaderData *rtp_hdr_data;
    struct bfile_in_net *file;
    struct bfile_out_net *fileOut;
    bfile_buffered_t fileBuffered;
    bool useNexusPlaypump;  /* set if app has indicated to use Nexus Playpump to feed IP session data (instead of default Nexus Playback) */
    char *indexCache;
    volatile unsigned int indexCacheDepth;
    volatile unsigned int indexCacheSize;
    off_t indexCacheStartOffset; /* linear offset of currnet first byte in index cache */
    off_t indexCacheEndOffset;   /* linear offset of currnet last byte in index cache */
    unsigned int indexCacheWriteIndex;  /* byte offset in index cache where next byte (from n/w) can be written into */
    unsigned int indexCacheReadIndex;  /* byte offset in index cache where indexCacheStartOffset starts */
    int indexCacheInit;  /* set after data cache is initialized */
    int indexSeekCount;  /* # of times seek has been called for index */
    bool indexAtEnd; /* true if index towards the end of the file */
    B_PlaybackIpAvCache dataCache[HTTP_MAX_DATA_CACHES];   /* we maintain two data caches for serving two different offset ranges (when AV data can be far apart) */
    int lastUsedCacheIndex; /* index of the cache that was used in the last read callback from Nexus */
    int cacheIndexUsingSocket; /* index of the cache that last read from the network socket */
    off_t lastSeekOffset;    /* offset into file that was used in the last seek callback from playback io threads */
    bool preChargeBuffer;   /* if set, HTTP thread starts pre-charging n/w buffer (data cache) by reading data from socket */
    int networkTimeout; /* timeout for network calls like select, read, etc. */
    bool selectTimeout; /* true when select call times out */
    bool rewind; /* set when file has been rewind */
    int *bytesRecvPerPkt; /* array of values representing the number of bytes received in each packet */
    bool chunkEncoding; /* if true, server is sending content using HTTP Chunk Transfer Encoding */
    bool adjustBytesToRead; /* if true, adjust the bytesToRead variable if DTCP PCP header is found */
    off_t chunkSize; /* current chunk size */
    off_t bytesLeftInCurChunk; /* current chunk size */
    off_t totalRead; /* total bytes Read */
    off_t totalConsumed; /* total bytes consumed by the Media player */
    char *chunkPlayloadStartPtr;  /* points to the start of payload read along w/ the next chunk header */
    int chunkPayloadLength;  /* length of the payload read along with the next chunk header */
    int num_chunks_received;  /* how many chunks have been completely received */
    bool serverClosed; /* true if server has closed the connection */
    bool reOpenSocket; /* true if current socket needs to be closed & reopened due to server closed or cache change case */
    bool reOpenIndexSocket; /* true if current socket needs to be reopened due to socket being owned for data */	
    B_PlaybackIpNetIo netIo; /* interface for network IO, implemented by each security module */
    void *securityHandle;   /* opaque handle of the security module for the currently active connection */
    bool printedOnce;   /* used to throttle the error message prints */
    bool readCallBackInProgress;   /* set when nexus io thread has invoked the data read callback */
    int cacheDepthFudgeFactor;   /* 0 during initial buffering, set during runtime buffering to HTTP_CACHE_DEPTH_FUDGE_FACTOR */
    unsigned int byteRangeOffset; /* byte range offset calculated in security session open. miliseconds */
    int statusCode;         /* http status code */
    B_PlaybackIpTrickModesSettings ipTrickModeSettings;
    bool streamingFormat;   /* set for TS, MP3, VoB, ES formats, false for MP4 & ASF (where index is present & are always cached) */
#ifndef DMS_CROSS_PLATFORMS    
    NEXUS_PlaybackPosition pausedPosition; /* position in msec where playback is currently paused */
    NEXUS_PlaybackPosition seekPosition; /* position in msec where playback needs to seek to */
    bool useSeekPosition; /* use seekPosition variable instead of current stream position while resuming the play */
    NEXUS_PlaybackPosition lastSeekPosition; /* position in msec where playback needs to start either after seek or trickmode */
    bool lastSeekPositionSet; /* true when a seek operation is carried out but firstPts callback hasn't yet come */
#endif /* DMS_CROSS_PLATFORMS */    
    off_t pausedByteOffset;    /* paused position in bytes from where to begin playback upon play */
    bool useNewOffset;  /* force next NetRangeReq call to use the pausedByteOffset even if it is 0 */
    bool socketClosed; /* set when socket has been closed: mainly used during pause using disconnect/seek method */
    bool firstDataReadCallbackDone; /* set when 1st read callback is done */
    bool waveHeaderInserted; /* set when wave header is inserted */
    bool dtcpPcpHeaderFound; /* set when DTCP/IP wrapper layer lowers the chunksize to account for PCP header */
    B_PlaybackIpProtocol mediaTransportProtocol; /* applicable for RTSP sessions only: transport protocol for carrying AV */
    bool disableLiveMode; /* if set, live mode is disabled (i.e. dont timeout select call on a live ip socket) */
    bool sendNextTimeSeekReq; /* set during RVU trick play we need to send the next time seek req to server */
    double initialTimeSeekRangeBegin; /* set upon each trick play command */
    double timeSeekRangeInterval; /* interval for the time seek range request */
    BKNI_MutexHandle lock;  /* lock to synchronize simultaneous access to this structure by Nexus IO, App, Http threads */
    B_Time mediaStartTime;
    bool mediaStartTimeNoted;
    unsigned int mediaCurrentlyPlayed;
    const bmedia_probe_stream *stream;
    bmedia_probe_t probe;
#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    bool hlsSessionEnabled; /* set if HTTP Live Streaming (HLS) is being used for the current session */
    HlsSessionState *hlsSessionState;
#endif
#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
    bool mpegDashSessionEnabled; /* set if MPEG-DASH is being used for the current session */
    MpegDashSessionState *mpegDashSessionState;
#endif
    unsigned int prevPlayed;
    bool stcTrickMode;
    unsigned int stcRate;
    bool mediaProbeStartTimeNoted; 
    B_Time mediaProbeStartTime; /* time when media probe is started */
    B_PlaybackIpAppHeader appHeader;
    bool ipTunerLockedEventSent; /* set when lock event is sent */
    bool ipTunerUnLockedEventSent; /* set when unlock event is sent */
    bool ipVerboseLog; /* if set, detailed logging is enabled on console log */
    bool enableRecording; /* if set, downloaded media stream is captured into a test file */
    NEXUS_VideoFrameRate frameRate; /* nexus video frame rate */
    char *cookieFoundViaHttpRedirect; /* caches pointer to the cookie header found in the HTTP redirect response */
} B_PlaybackIp;

/* Output structure used in security session opens */
typedef struct B_PlaybackIpSecurityOpenOutputParams {
    B_PlaybackIpNetIo *netIoPtr; /* interface for network IO, implemented by each security module */
    void **securityHandle; 
    unsigned int byteRangeOffset; /* byte range offset calculated from seekTimeOffset */
} B_PlaybackIpSecurityOpenOutputParams;

typedef enum {
    HTTP_URL_IS_DIRECT, /* e.g. www.mydeo.com/portugal.mpg */
    HTTP_URL_IS_ASX,    /* e.g. www.mydeo.com/portugal.asp */
    HTTP_URL_IS_REDIRECT    /* e.g. www.mydeo.com/portugal.asp */
}http_url_type_t;
extern int http_parse_url(char *server, unsigned *portPtr, char **uri, char *url);
extern int http_parse_redirect(char *server, unsigned *portPtr, B_PlaybackIpProtocol *protocol, char **urlPtr, char **cookie, char *http_hdr);

struct udp_hdr {
    unsigned short   source;
    unsigned short   dest;
    unsigned short   len;
    unsigned short   checksum;
};


typedef struct B_PlaybackIp_RtpHeader {
    uint16_t word0;
#define VERSION_MASK 0xc000
#define VERSION_SHIFT 14
#define PADDING_MASK 0x2000
#define PADDING_SHIFT 13
#define EXTENSION_MASK 0x1000
#define EXTENSION_SHIFT 12
#define CSRC_MASK 0x0f00
#define CSRC_SHIFT 8
#define MARKER_MASK 0x0080
#define MARKER_SHIFT 7
#define TYPE_MASK 0x007f
#define TYPE_SHIFT 0
    uint16_t sequenceNum;
    uint32_t timestamp;
    uint32_t ssrc;
} B_PlaybackIp_RtpHeader;

#ifdef __cplusplus
extern "C" {
#endif
B_PlaybackIpError B_PlaybackIp_GetRtpHeaderData(
    B_PlaybackIpHandle p, 
    unsigned int maxDataSize,
    B_PlaybackIp_RtpStatsHeader *rtpHeaderData,
    unsigned *entry_cnt
    );
#ifdef __cplusplus
}
#endif


/* Wrappers for C++ XML Parser Functions in "b_playback_ip_xml_helper.cpp" */
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct MXmlElement   *B_PlaybackIp_XmlElement;

    /* Create a tree of XML objects by parsing an XML string. */
    B_PlaybackIp_XmlElement B_PlaybackIp_Xml_Create( const char *str );

    /* Destroy the tree of XML objects. */
    void B_PlaybackIp_Xml_Destroy(B_PlaybackIp_XmlElement xmlElemRoot );

    /* Print an XML element (non-recursively). */
    void B_PlaybackIp_XmlElem_Print(const B_PlaybackIp_XmlElement xmlElem , int level);

    /* Print an XML element (non-recursively). */
    void B_PlaybackIp_XmlElem_PrintRecursively(const B_PlaybackIp_XmlElement xmlElem, int level);

    /* Return a pointer to an element's tag (so you can tell what the element is). */
    const char *B_PlaybackIp_XmlElem_GetTag(B_PlaybackIp_XmlElement xmlElem);

    /* Return a pointer to an element's data (which technically belongs to a tagless child element). */
    const char *B_PlaybackIp_XmlElem_ChildData(B_PlaybackIp_XmlElement xmlElem);

    /* Find the first child element. */
    B_PlaybackIp_XmlElement B_PlaybackIp_XmlElem_FirstChild(B_PlaybackIp_XmlElement xmlElem);

    /* Find the next child element. */
    B_PlaybackIp_XmlElement B_PlaybackIp_XmlElem_NextChild(B_PlaybackIp_XmlElement xmlElem);

    /* Find a child element with a specific tag name. */
    B_PlaybackIp_XmlElement B_PlaybackIp_XmlElem_FindChild(B_PlaybackIp_XmlElement xmlElem , const char *tag);

    /* Find the next child with a specific tag name. */
    B_PlaybackIp_XmlElement B_PlaybackIp_XmlElem_FindNextChild(B_PlaybackIp_XmlElement xmlElem, B_PlaybackIp_XmlElement xmlElemStartChild, const char *tag);

    /* Find a specific attribute of an element (by name), and return a pointer to it's value string. */
    const char *B_PlaybackIp_XmlElem_FindAttrValue(B_PlaybackIp_XmlElement xmlElem , const char *name);

#ifdef __cplusplus
}
#endif


#endif /* __B_PLAYBACK_IP_PRIV__*/
