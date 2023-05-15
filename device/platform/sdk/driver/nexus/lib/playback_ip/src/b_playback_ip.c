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
* $brcm_Workfile: b_playback_ip.c $
* $brcm_Revision: 121 $
* $brcm_Date: 10/30/12 11:08a $
*
* Description: IP Applib Implementation
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip.c $
* 
* 121   10/30/12 11:08a ssood
* SW7231-858: more changes to support & handle multiple PTS discontinuity
*  events
* 
* 120   10/16/12 5:18p ssood
* SW7231-858: handle case where multiple PTS discontinuities come back-
*  to-back
* 
* 119   9/18/12 10:07a ssood
* SW7425-3957: crash in playback_ip when LIVEMEDIA_SUPPORT is not defined
* 
* 118   9/15/12 3:10p ssood
* SW7231-1010: fixed runtime buffer depth calculation due to compiler
*  change
* 
* 117   9/6/12 4:17p ssood
* SW7425-3673: added support for seek/pause/play of HLS sessions
* 
* 116   8/28/12 2:35p ssood
* SW7231-974: extend playback_ip current position logic to handle PTS
*  discontinuities during normal playback
* 
* 115   7/17/12 5:17p gskerl
* SW7346-853: Merged from branch SW7346-853 to main.
* 
* SW7346-853/2   7/10/12 3:15p gskerl
* SW7346-853: Updated branch SW7346-853 from main/LATEST.
* 
* SW7346-853/1   7/5/12 3:00p gskerl
* SW7346-853: Added support for MPEG-DASH to B_PlaybackIp_GetStatus().
* 
* 114   6/13/12 7:24a ssood
* SW7231-766: Coverity fixes for HLS player logic
* 
* 113   6/11/12 11:20a ssood
* SW7425-3207: coverity fix
* 
* 112   6/8/12 7:11p ssood
* SW7425-3201: Add support for new DLNA PlaySpeed trickmode options in
*  playback ip library
* 
* 111   6/5/12 6:46p ssood
* SW7231-766: add support for downloading encrypted HLS keys + support
*  for HLS spec version 3
* 
* 110   5/23/12 3:13p ssood
* SW7425-2894: suppressing extra debug log from ip client modules
* 
* 109   3/12/12 4:41p gkamosa
* SW7425-2543: Merge support for 3383 SMS platform
* 
* Centaurus_Phase_1.0/2   3/6/12 6:03p gkamosa
* SW7425-2337: Merge branch to tip
* 
* 108   1/11/12 2:50p ssood
* SW7425-2061: wrap simple decoders call for NEXUS_HAS_SIMPLE_DECODER
* 
* 107   12/27/11 10:07p ssood
* SW7425-2061: add simple decoder support to ip playback
* 
* Centaurus_Phase_1.0/1   2/14/12 5:04p jmiddelk
* SW7425-2337: Add support for 3383sms platform
* 
* 106   11/23/11 2:10p ssood
* SW7408-109: RVU Trickplay support: added FFWD & FRWD
* 
* 105   10/21/11 2:22p ssood
* SW7231-405: add debug print about stream video info
* 
* 104   8/26/11 4:15p dliu
* SW7425-1077: In ptsCallback, always check for videoDecoder handle valid
* 
* 103   8/25/11 6:04p ssood
* SW7425-1077: dont overwrite firstPts in the firstPtsPassed callback as
*  it happens upon each discontinuity
* 
* 102   8/8/11 10:46a mward
* SW7125-1038: Power on/off DMA if using Netaccel.
* 
* 101   8/4/11 4:45p ssood
* SW7425-1040: add fast channel change support to IP Sessions
* 
* 100   6/6/11 7:34p ssood
* SW7231-166: fix avgBitRate calculations for fragmented mp4 streams +
*  allow dataReadCallback for streams w/o index data
* 
* 99   6/6/11 1:28a ssood
* SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
*  eventCallback support for end of stream playback
* 
* 98   5/26/11 11:31p ssood
* SW7231-166: dont create Nexus File Handle for cases where Nexus
*  playpump will get used
* 
* 97   5/25/11 3:35p ssood
* SW7231-166: fixed some bugs during failing trickplay cases
* 
* 96   5/23/11 3:44p ssood
* SW7231-166: Extend Playback IP trickmodes to be compatible with latest
*  DLNA Guidelines
* 
* 95   3/28/11 10:59a ssood
* SW7420-502: increase the waittime to allow media probe to
*  finish/timeout
* 
* 94   2/23/11 11:48a ssood
* SW7420-502: clock recovery support for RVU client
* 
* 93   2/18/11 5:15p zhangjq
* SW7420-1419:Clean up Android Media Player code
* 
* 92   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 91   8/27/10 3:59p ssood
* SW7420-502: added partial support to play RVU sessions in live mode
* 
* 90   6/16/10 5:05p ssood
* SW7420-716: initial RVU trickmode related check-ins
* 
* 89   4/16/10 5:02p ssood
* SW7420-561: Added support for Additional Philips RTSP features:
*  configurable keepalive timer, allow apps to send custom headers to
*  servers, generate EOF events to app upon RTSP heartbeat failures
* 
* 88   4/2/10 4:18p ssood
* SW7420-561: add support to handle large URLs (HTML5 URLs can be over
*  256bytes)
* 
* 87   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/3   2/18/10 12:43a ssood
* SW7420-561: added checks to validate IP state during IP API calls
* 
* SW7420-561/2   2/16/10 10:42p ssood
* SW7420-561: Changes to support basic RTSP w/o trickmodes
* 
* SW7420-561/1   2/16/10 10:10a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 86   2/5/10 7:04p ismailk
* SW7420-570: open() returns -1 if unable to access the bcmrgshim driver.
* 
* 85   1/29/10 6:13p ismailk
* SW7420-559: Enable EROUTER_SUPPORT flag to enable the WAN unicast
*  ioctl.
* 
* 84   1/23/10 10:17a ssood
* SW7420-454: further changes to condense the ip playback log
* 
* 83   1/20/10 4:13p ssood
* SW7420-454: more changes to compress the verbose output
* 
* 82   1/19/10 4:55p ssood
* SW7420-454: convert verbose output to be available via msg modules
*  flags
* 
* 81   12/23/09 3:28p erickson
* SW7420-516: convert non-error BDBG_ERR to BDBG_WRN
*
* 80   11/24/09 10:48a ssood
* SW7420-454: add option to allow apps to specify a specific interface
*  name to receive live IP content
*
* 79   11/20/09 10:04a ssood
* SW7420-454: set IP playback position to 0 if we dont get the valid
*  position (haven't started the playback or some other error)
*
* 78   11/18/09 7:16p ssood
* SW7420-340: set playback position to duration if playback reports
*  incorrect value
*
* 77   10/21/09 3:03p ssood
* SW7420-340: clear the status field before setting it
*
* 76   10/20/09 6:33p ssood
* SW7420-340: remove nexus playback, audio, video decoder dependencies
*  for SMS compilation
*
* 75   10/1/09 11:00a jjordan
* SW7405-3107: add IP recv timeout status to Settop API
*
* 74   9/18/09 9:49a lwhite
* SW7420-340: Set playing state before launching proccessing thread
*
* 73   8/24/09 3:23p ssood
* SW7405-1689: adapting server based trickmodes to Nexus playback changes
*  due to OTF support
*
* 72   8/14/09 5:00p ssood
* PR50202: First check-in for clientside trickmode support for MPEG2 TS
*  format
*
* 71   8/6/09 7:32p ssood
* PR48146: commented out a debug message
*
* 70   8/6/09 3:38p ssood
* PR48146: add HTTP redirect support
*
* 69   8/6/09 10:18a ssood
* PR43746: added support for calculating more precise bitrates for TS &
*  VOB files (support index reads from middle & end of content)
*
* 68   7/30/09 10:31a ssood
* PR43746: provide current playback position via the IP GetStatus
*
* 67   7/29/09 6:14p ssood
* PR43746: extend IP trick modes to support Disconnect And Seek based
*  paused operations (DLNA requirement)
*
* 66   7/24/09 12:01p ssood
* PR56519: Allow apps to provide additional HTTP headers fields for
*  outgoing GET requests
*
* 65   7/21/09 4:18p ssood
* PR43746: Further work for Server based HTTP trick mode support
*
* 64   7/16/09 12:59p ssood
* PR43746: Add Server based HTTP trick mode support
*
* 63   6/25/09 11:14a ssood
* PR53773: removing a debug comment
*
* 62   6/25/09 2:30a ssood
* PR55417: added option to allocate HTTP precharge buffer either at init
*  or run time
*
* 61   6/23/09 11:55p ssood
* PR56279: Coverity error of reverse NULL
*
* 60   6/22/09 6:31p ssood
* PR55417: set HTTP index cache size also based on the user provided size
*  (otherwise, default size is too small) + fix for n/w timeout during
*  index cache reading
*
* 59   6/22/09 1:44p ssood
* PR55417: Precharging support for TS/VOB formats: moved the HTTP
*  data/index cache allocations to IP Applib Open time, lowered default
*  cache sizes
*
* 58   6/18/09 10:24a ssood
* PR55417: provide http return code code via the GetStatus to app
*
* 57   6/16/09 3:00p ssood
* PR55417: accounting for fudge factor in max buffer depth calculations
*
* 56   6/15/09 11:31p ssood
* PR55417: fixing rewind related bug
*
* 55   6/12/09 9:16p ssood
* PR55908: Add IPv6 Support to IP Applib
*
* 54   6/7/09 11:45p ssood
* PR55417: Adding support for runtime buffering of HTTP content (used
*  during seeks & n/w loss events)
*
* 53   6/5/09 1:35p ssood
* PR55626: DLNA guidelines mandate client to only maintain no more than 1
*  connection to server, we had one connx per data cache, changed that to
*  closing/opening 1 connx
*
* 52   6/4/09 5:55p ssood
* PR55325: return buffer depth in GetStatus only if max bitrate is
*  available
*
* 51   5/29/09 6:17p ssood
* PR55325: cleanup in debug code
*
* 50   5/28/09 4:54p ssood
* PR55325: allow app to specify n/w timeout & buffer cache size
*
* 49   5/28/09 11:38a ssood
* PR55325: added additional states to IP Applib to provide flexibility in
*  the order of API calls
*
* 48   5/26/09 11:49a ssood
* PR53773: added debug code to track a crash during quit
*
* 47   5/25/09 11:30p ssood
* PR55325: Add support for pre-charging network buffer for HTTP playback
*
* 46   5/15/09 1:08p ssood
* PR53773: adding support for skipping Media probe: needed for RAD/EA
*  playback where app apriori have media info
*
* 45   5/8/09 4:35p jjordan
* PR54886: fix TTS initial buffer depth
*
* 44   5/1/09 4:50p ssood
* PR53773: adding support for WMA playback over HTTP
*
* 43   4/21/09 3:32p ssood
* PR53773: handle cases where some security protocols dont encrypt HTTP
*  response header
*
* 42   4/21/09 10:59a lwhite
* PR53373: Add extern for securityContextOpen
*
* 41   4/17/09 2:58p lwhite
* PR53373: Cleanup ssl init
*
* 40   4/16/09 5:18p lwhite
* PR53373: Clone session support
*
* 39   4/14/09 5:02p lwhite
* PR54157: Fixed memory leak
*
* 38   4/6/09 6:09p ssood
* PR53773 : refactored basic TCP socket functions for reuse by other
*  internal modules
*
* 37   4/3/09 9:13a ssood
* PR53773 : code cleanup
*
* 36   4/2/09 11:36p ssood
* PR53773 : changes to support security framework for SSL, DTCP-IP,
*  RAD/ES protocols
*
* 35   3/2/09 2:44p ssood
* PR50311: brutus quit crashes when we quit from a HTTP channel where
*  connect attempt had failed
*
* 34   2/27/09 11:55a ssood
* PR50311: Adding support for HTTP Chunk Transfer Encoding
*
* 33   12/4/08 10:13a jjordan
* PR49937: Add support for RTP over TTS
*
* 32   11/7/08 5:30p ssood
* PR48146: HTTP processing now happens in the content of Nexus Playback
*  Player thread, so removing the local thread logic (as it was causing
*  sync issues during quit)
*
* 31   11/7/08 2:54p jrubio
* PR48782: modify stop
*
* 30   11/5/08 7:46p ssood
* PR48146: fixed a core dump during failure path
*
* 29   11/5/08 4:12p ssood
* PR48146: more work on support needed for MP4/ASF playback
*
* 29   11/5/08 3:37p ssood
* PR48146: more work on support needed for MP4/ASF playback
*
* 28   10/31/08 1:12p jjordan
* PR47230: fix warning
*
* 27   10/31/08 12:30p jjordan
* PR47230: Add TTS Pacing
*
* 26   10/27/08 11:57a ssood
* PR45431: adding MP4 & ASF playback support over HTTP
*
* 25   10/13/08 5:46p ssood
* PR47521: improved support for low-bit rate streams + handle unmarked
*  discontinuity condition
*
* 25   10/13/08 5:42p ssood
* PR47521: improved support for low-bit rate streams + handle unmarked
*  discontinuity condition
*
* 24   10/10/08 6:53p lwhite
* PR47685: Coverity fixes
*
* 23   10/7/08 11:32p erickson
* PR47232: ---------------------------
*
* 22   10/3/08 3:32p lwhite
* PR42739: Added RTSP timeline support
*
* 21   9/3/08 1:59p lwhite
* PR42739: Http standard socket fixes
*
* 20   8/18/08 11:05a ssood
* PR45431: Fixed RTP to work standard sockets
*
* 19   8/14/08 12:29p ssood
* PR45431: fixed compilation errors w/ LIVEMEDIA_SUPPORT=n
*
* 18   8/11/08 10:13a ssood
* PR45431: Add HTTP playback support for ASF content
*
* 17   8/5/08 1:12p ssood
* PR42739: splitting up UDP & RTP Processing code into separate sub-
*  modules
*
* 16   8/5/08 7:59a ssood
* PR45400: make code compilable with -std=c89 for maximum compatibility
*
* PR45400/1   8/5/08 12:44p dyzhang
* PR45400: kylin should compile with -std=c89 for maximum compatibility
*
* 15   7/31/08 10:42a ssood
* PR42739: Fixing macroblocking issue which happens during PSI discovery
*
* 14   7/29/08 3:49p ssood
* PR42739: fixing HTTP decode related errors
*
* 13   7/28/08 4:19p ssood
* PR42739: fixed a core dump during close
*
* 12   7/28/08 3:27p ssood
* PR42739: further changes for supporting example app for IP Applib
*
* 11   7/24/08 1:23p ssood
* PR42739: changes for supporting example app for IP Applib
*
* 10   7/21/08 4:06p ssood
* PR42739: code changes to simplify ip app lib interface
*
* 9   7/14/08 7:12p ssood
* PR42739: added option to disable netdma filter to fix macroblocking
*  issue after pid discovery
*
* 8   7/14/08 5:37p ssood
* PR42739: RTCP stats function was not using correct pkt offset
*
* 7   7/14/08 4:28p lwhite
* PR42739: Added RTCP support
*
* 6   7/9/08 9:53p ssood
* PR42739: added support for high jitter + receiving variable size UDP
*  packets
*
* 5   6/18/08 7:38a ssood
* PR43744: Channel Change script fails on IP channels after 5-10min
*
* 5   6/18/08 7:35a ssood
* PR43744: Channel Change script fails on IP channels after 5-10min
*
* 4   6/13/08 9:52p ssood
* PR42739: Added check to monitor CDB depth & throttle sender for HTTP
*  playback
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

 /**
 * Note this file is currently only compiled for LINUX platforms. Even though it
 * has VxWorks & WinCE related hash defines, the code needs more porting
 * work for these OS platforms.
 * If the feature isn't compiled in, then the only functions in this .c file
 * should be the public stubs.
 **/
#if defined(LINUX) || defined(__vxworks)

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"
#include "b_playback_ip_lm_helper.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include "nexus_power_management.h"

BDBG_MODULE(b_playback_ip);

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

/***************************************************************************
Summary:
Private handle for each Playback IP App Lib context
***************************************************************************/

/* forward declarations: */
#ifndef B_HAS_SMS_GATEWAY
/* When SMS Gateway is configured, IP Applib is used only for receiving Live IP traffic which is mainly using UDP/RTP traffic. So we dont need any of the HTTP module. */
extern B_PlaybackIpError B_PlaybackIp_HttpSessionOpen( B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, B_PlaybackIpSessionOpenStatus *openStatus);
extern B_PlaybackIpError B_PlaybackIp_HttpSessionSetup(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionSetupSettings *setupSettings, B_PlaybackIpSessionSetupStatus *setupStatus);
extern B_PlaybackIpError B_PlaybackIp_HttpSessionSetup(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionSetupSettings *setupSettings, B_PlaybackIpSessionSetupStatus *setupStatus);
extern B_PlaybackIpError B_PlaybackIp_HttpSessionStart(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionStartSettings *startSettings, B_PlaybackIpSessionStartStatus *startStatus);
extern void B_PlaybackIp_HttpSessionStop(B_PlaybackIpHandle playback_ip);
extern void B_PlaybackIp_HttpSessionClose(B_PlaybackIpHandle playback_ip);
B_PlaybackIpError B_PlaybackIp_HttpGetCurrentPlaybackPosition(B_PlaybackIpHandle playback_ip, NEXUS_PlaybackPosition *currentPosition);
#endif
extern B_PlaybackIpError B_PlaybackIp_UdpSessionOpen( B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, B_PlaybackIpSessionOpenStatus *openStatus);
extern B_PlaybackIpError B_PlaybackIp_UdpSessionStart(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionStartSettings *startSettings, B_PlaybackIpSessionStartStatus *startStatus);
extern void B_PlaybackIp_UdpSessionClose(B_PlaybackIpHandle playback_ip);

/* remove RTP temporaroly, zhangjq,  20101221 */
#ifndef ANDROID
extern B_PlaybackIpError B_PlaybackIp_RtpSessionOpen( B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, B_PlaybackIpSessionOpenStatus *openStatus);
extern B_PlaybackIpError B_PlaybackIp_RtpSessionStart(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionStartSettings *startSettings, B_PlaybackIpSessionStartStatus *startStatus);
extern void B_PlaybackIp_RtpSessionStop(B_PlaybackIpHandle playback_ip);
extern void B_PlaybackIp_RtpSessionClose(B_PlaybackIpHandle playback_ip);
#endif

extern B_PlaybackIpError B_PlaybackIp_RtspSessionOpen( B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionOpenSettings *openSettings, B_PlaybackIpSessionOpenStatus *openStatus);
extern B_PlaybackIpError B_PlaybackIp_RtspSessionStart(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionStartSettings *startSettings, B_PlaybackIpSessionStartStatus *startStatus);
extern B_PlaybackIpError B_PlaybackIp_RtspSessionSetup(B_PlaybackIpHandle playback_ip, B_PlaybackIpSessionSetupSettings *setupSettings, B_PlaybackIpSessionSetupStatus *setupStatus);

/* TODO: look into using a per IP session context mutext to prevent multiple threads invoking the same/different APIs for a given IP session context */

/***************************************************************************
Summary:
This function initializes an Playback IP channel on the settings selected. A shallow
copy of the B_PlaybackIp_Settings structure is made in this call. The private App Lib
structure is malloc'ed.
***************************************************************************/
B_PlaybackIpHandle B_PlaybackIp_Open(
    const B_PlaybackIpOpenSettings *pSettings
    )
{
    B_PlaybackIpHandle playback_ip = NULL;

    BDBG_MSG(("%s: PKTS_PER_READ %d\n", __FUNCTION__, PKTS_PER_READ));
    BSTD_UNUSED(pSettings);

    /* allocate playback_ip context */
    playback_ip = (B_PlaybackIpHandle) BKNI_Malloc(sizeof(B_PlaybackIp));
    if (NULL == playback_ip)
        return NULL;
    BKNI_Memset(playback_ip, 0, sizeof(B_PlaybackIp));

    /* move to open state */
    playback_ip->playback_state = B_PlaybackIpState_eOpened;

    return (B_PlaybackIpHandle) playback_ip;
}

/*
Summary:
    Close an IP playback channel.
    This function de-initializes the Playback IP App Lib. The private App Lib structure is freed.
*/
B_PlaybackIpError 
B_PlaybackIp_Close(
    B_PlaybackIpHandle playback_ip
    )
{
    if (!playback_ip) {
        BDBG_ERR(("%s: NULL playback_ip handle\n", __FUNCTION__));
        return B_ERROR_INVALID_PARAMETER;
    }

    BDBG_MSG(("%s: ", __FUNCTION__));

    switch (playback_ip->playback_state) {
    case B_PlaybackIpState_eSessionSetupInProgress:
    case B_PlaybackIpState_eSessionSetup:
    case B_PlaybackIpState_eSessionStartInProgress:
    case B_PlaybackIpState_ePlaying:
    case B_PlaybackIpState_eStopping:
    case B_PlaybackIpState_eBuffering:
    case B_PlaybackIpState_ePaused:
    case B_PlaybackIpState_eEnteringTrickMode:
    case B_PlaybackIpState_eTrickMode:
        B_PlaybackIp_SessionStop(playback_ip);
        /* continue w/ session close cleanup */
    case B_PlaybackIpState_eSessionOpenInProgress:
    case B_PlaybackIpState_eSessionOpened:
    /* case B_PlaybackIpState_eStopped: */
        B_PlaybackIp_SessionClose(playback_ip);
        /* continue w/ playback ip channel cleanup */
    case B_PlaybackIpState_eOpened:
    default:
        break;
        /* do the playback channel related cleanup */
    }

    BKNI_Free(playback_ip);

    return BERR_SUCCESS;
}

B_PlaybackIpError 
B_PlaybackIp_SessionOpen(
    B_PlaybackIpHandle playback_ip,
	B_PlaybackIpSessionOpenSettings *openSettings,
	B_PlaybackIpSessionOpenStatus *openStatus /* [out] */
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;
    B_PlaybackIpSocketState *socketState;
    char *pValue = NULL;

    if (!playback_ip || !openSettings || !openStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, openSettings %p, openStatus %p\n", __FUNCTION__, playback_ip, openSettings, openStatus));
        return B_ERROR_INVALID_PARAMETER;
    }

    /* Validate if app can call this API in the current IP Playback State */
    switch (playback_ip->playback_state) {
    case B_PlaybackIpState_eOpened:
    case B_PlaybackIpState_eSessionOpenInProgress:
        /* continue below with this API */
        break;
    default:
        /* In all other states, app can't call this API */
        BDBG_ERR(("ERROR: Can't call %s() in this state %d\n", __FUNCTION__, playback_ip->playback_state));
        return B_ERROR_NOT_SUPPORTED;
    }

    if (openSettings->nonBlockingMode) {
        if (!openSettings->eventCallback) {
            BDBG_ERR(("%s: invalid param: eventCallback must be specified for nonBlockingMode operation", __FUNCTION__));
            return B_ERROR_INVALID_PARAMETER;
        }

        /* if the API is already in progress, return INCOMPLETE */
        if (playback_ip->apiInProgress) {
            return B_ERROR_IN_PROGRESS;
        }

        /* So Api is not in progress, check if it is completed, them jump to returning results to app */
        if (playback_ip->apiCompleted) {
            BDBG_MSG(("%s: previously started session open operation completed, playback_ip %p", __FUNCTION__, playback_ip));
            goto apiDone;
        }
        
        /* else fall below to perform session initialization */
    }

#ifdef B_HAS_NETACCEL
	NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eDma, true);
#endif
    /* reset playback ip context */
    memset(playback_ip, 0, sizeof(B_PlaybackIp));
    B_PlaybackIp_GetDefaultSettings(&playback_ip->settings);

    pValue = getenv("enableRecording");
    if (pValue)
        playback_ip->enableRecording = true;
    else 
        playback_ip->enableRecording = false;

    pValue = getenv("ipVerboseLog");
    if (pValue)
        playback_ip->ipVerboseLog = true;
    else 
        playback_ip->ipVerboseLog = false;

    /* now API is progress, update state to reflect that */
    playback_ip->playback_state = B_PlaybackIpState_eSessionOpenInProgress;

    BDBG_MSG(("%s: playback_ip %p, openSettings %p, openStatus %p, state %d, proto %d\n", 
                __FUNCTION__, playback_ip, openSettings, openStatus, playback_ip->playback_state, openSettings->socketOpenSettings.protocol));

    playback_ip->openSettings = *openSettings;
    if (openSettings->socketOpenSettings.url != NULL) {
        /* cache a copy of the URL */
        if ((playback_ip->openSettings.socketOpenSettings.url = strdup(openSettings->socketOpenSettings.url)) == NULL) {
            BDBG_ERR(("%s: Failed to duplicate URL string due to out of memory condition", __FUNCTION__));
            goto error;
        }
    }
    socketState = &openStatus->socketState;
    memset(openStatus, 0, sizeof(B_PlaybackIpSessionOpenStatus));

    /* create events */
    if (BKNI_CreateEvent(&playback_ip->playback_halt_event)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    if (BKNI_CreateEvent(&playback_ip->read_callback_event)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    if (BKNI_CreateEvent(&playback_ip->preChargeBufferEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    if (BKNI_CreateEvent(&playback_ip->newJobEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    if (BKNI_CreateEvent(&playback_ip->liveMediaSyncEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    /* create mutex for serializing flow among different flows invoking a IP playback session */
    if (BKNI_CreateMutex(&playback_ip->lock) != 0) {
        BDBG_ERR(("Failed to create BKNI mutex at %d", __LINE__));
        errorCode = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }


apiDone:
    switch (openSettings->socketOpenSettings.protocol) {
#ifndef B_HAS_SMS_GATEWAY
    case B_PlaybackIpProtocol_eHttp:
        errorCode = B_PlaybackIp_HttpSessionOpen(playback_ip, openSettings, openStatus);
        break;
#endif
    case B_PlaybackIpProtocol_eUdp:
    case B_PlaybackIpProtocol_eRtpFec:
        errorCode = B_PlaybackIp_UdpSessionOpen(playback_ip, openSettings, openStatus);
        break;
    case B_PlaybackIpProtocol_eRtsp:
#ifdef LIVEMEDIA_SUPPORT
        errorCode = B_PlaybackIp_RtspSessionOpen(playback_ip, openSettings, openStatus);
#else
        errorCode = B_ERROR_INVALID_PARAMETER;
        BDBG_ERR(("%s: RTSP protocol requires compilation of Live Media library (build w/ LIVEMEDIA_SUPPORT=y)", __FUNCTION__));
#endif
        break;
        
#ifndef ANDROID         
    case B_PlaybackIpProtocol_eRtp:
#ifdef LIVEMEDIA_SUPPORT
        errorCode = B_PlaybackIp_RtpSessionOpen(playback_ip, openSettings, openStatus);
#else
        errorCode = B_ERROR_INVALID_PARAMETER;
        BDBG_ERR(("%s: RTP protocol requires compilation of Live Media library (build w/ LIVEMEDIA_SUPPORT=y)", __FUNCTION__));
#endif
        break;
    case B_PlaybackIpProtocol_eRtpNoRtcp:
        errorCode = B_PlaybackIp_RtpSessionOpen(playback_ip, openSettings, openStatus);
        break;
#endif  
      
    default:
        errorCode = B_ERROR_INVALID_PARAMETER;
        break;
    }
    if (errorCode == B_ERROR_IN_PROGRESS)
        return B_ERROR_IN_PROGRESS;
    else if (errorCode != B_ERROR_SUCCESS) 
        goto error;
    
    /* now this API is successfully completed, update state to reflect that */
    playback_ip->playback_state = B_PlaybackIpState_eSessionOpened;
    playback_ip->protocol = openSettings->socketOpenSettings.protocol;
    playback_ip->socketState = openStatus->socketState;
    playback_ip->settings.ipMode = openSettings->ipMode;
    playback_ip->settings.networkTimeout = openSettings->networkTimeout;
    BDBG_MSG(("%s: Session Open completed: socket fd %d\n", __FUNCTION__, playback_ip->socketState.fd));
    return B_ERROR_SUCCESS;

error:
    if (playback_ip->openSettings.socketOpenSettings.url)
        free(playback_ip->openSettings.socketOpenSettings.url);
    if (playback_ip->playback_halt_event)
        BKNI_DestroyEvent(playback_ip->playback_halt_event);
    if (playback_ip->preChargeBufferEvent)
        BKNI_DestroyEvent(playback_ip->preChargeBufferEvent);
    if (playback_ip->newJobEvent)
        BKNI_DestroyEvent(playback_ip->newJobEvent);
    if (playback_ip->read_callback_event)
        BKNI_DestroyEvent(playback_ip->read_callback_event);
    if (playback_ip->liveMediaSyncEvent)
        BKNI_DestroyEvent(playback_ip->liveMediaSyncEvent);
    if (playback_ip->lock) {
        BKNI_DestroyMutex(playback_ip->lock);
        playback_ip->lock = NULL;
    }

    /* back to Opened state */
    playback_ip->playback_state = B_PlaybackIpState_eOpened;
    playback_ip->openSettings.eventCallback = NULL;
    BDBG_ERR(("%s() ERRRO: playback_ip %p, errorCode %d, fd %d\n", __FUNCTION__, playback_ip, errorCode, playback_ip->socketState.fd));
    return errorCode;
}

B_PlaybackIpError 
B_PlaybackIp_SessionSetup(
    B_PlaybackIpHandle playback_ip,
	B_PlaybackIpSessionSetupSettings *setupSettings,
	B_PlaybackIpSessionSetupStatus *setupStatus /* [out] */
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;

    BDBG_MSG(("%s: playback_ip %p, setupSettings %p, setupStatus %p\n", __FUNCTION__, playback_ip, setupSettings, setupStatus));
    if (!playback_ip || !setupSettings || !setupStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, setupSettings %p, setupStatus %p\n", __FUNCTION__, playback_ip, setupSettings, setupStatus));
        return B_ERROR_INVALID_PARAMETER;
    }

    /* Validate if app can call this API in the current IP Playback State */
    switch (playback_ip->playback_state) {
    case B_PlaybackIpState_eSessionOpened:
    case B_PlaybackIpState_eSessionSetupInProgress:
        /* continue below with this API */
        break;
    default:
        /* In all other states, app can't call this API */
        BDBG_ERR(("ERROR: Can't call %s() in this state %d\n", __FUNCTION__, playback_ip->playback_state));
        return B_ERROR_NOT_SUPPORTED;
    }

    /* now API is progress, update state to reflect that */
    playback_ip->playback_state = B_PlaybackIpState_eSessionSetupInProgress;

    memset(setupStatus, 0, sizeof(B_PlaybackIpSessionSetupStatus));
    playback_ip->setupSettings = *setupSettings;

    switch (playback_ip->openSettings.socketOpenSettings.protocol) {
#ifndef B_HAS_SMS_GATEWAY
    case B_PlaybackIpProtocol_eHttp:
        errorCode = B_PlaybackIp_HttpSessionSetup(playback_ip, setupSettings, setupStatus);
        break;
#endif
#ifdef LIVEMEDIA_SUPPORT
    case B_PlaybackIpProtocol_eRtsp:
        errorCode = B_PlaybackIp_RtspSessionSetup(playback_ip, setupSettings, setupStatus);
        break;
#endif
    default:
        /* this function is NOP for rest of the protocols */
        errorCode = B_ERROR_SUCCESS;
        break;
    }

    if (errorCode == B_ERROR_IN_PROGRESS)
        return B_ERROR_IN_PROGRESS;
    else if (errorCode != B_ERROR_SUCCESS) 
        goto error;

    /* now this API is successfully completed, update state to reflect that */
    playback_ip->playback_state = B_PlaybackIpState_eSessionSetup;
    playback_ip->sessionSetupCompleted = true;
    BDBG_MSG(("%s: Session Setup completed: socket fd %d\n", __FUNCTION__, playback_ip->socketState.fd));
    return B_ERROR_SUCCESS;

error:
    BDBG_ERR(("%s() ERRRO: playback_ip %p, errorCode %d, fd %d\n", __FUNCTION__, playback_ip, errorCode, playback_ip->socketState.fd));
    playback_ip->playback_state = B_PlaybackIpState_eSessionOpened;
    return errorCode;
}

extern B_PlaybackIpError http_get_current_pts( B_PlaybackIpHandle playback_ip, unsigned int *currentPts);
static void 
ptsErrorCallback(void *context, int param)
{
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)context;
    uint32_t currentPts = 0;
    uint32_t currentContigousStreamDuration = 0;
    uint32_t stc;
    uint32_t nextPts = 0;
    BSTD_UNUSED(param);

    if (playback_ip == NULL) {
        BDBG_ERR(("%s: ERROR: playback_ip context became NULL", __FUNCTION__));
        return;
    }
    /* when Display Manager (DM) detects a PTS discontinuity, nexus updates the STC with the PTS of the new frame with the discontinuity */
    /* and triggers this callback. Here, we obtain the current STC and use that as the firstPts of the new segment */
    /* we also get the currentPts which still points to the last frame before the discontinuity. */
    /* (this is because it takes decoder more time to decode the i-frame after the discontinuity and also it takes */
    /* sometime for its PTS to pass) and thus be available to the DM and thus reflected in the pts value) */
    /* and use the current firstPts and this pts to determine the duration of this segment before the discontinuity */
    /* Also, we accumulate the durations of all contigous segments and maintain them in currentPosition */

    NEXUS_StcChannel_GetStc(playback_ip->nexusHandles.stcChannel, &stc); 
    /* current STC will become new firstPts */
    nextPts = stc;

    /* get pts of the last frame before disontinuity */
    if ( (http_get_current_pts(playback_ip, &currentPts) != B_ERROR_SUCCESS) || currentPts == 0) {
        BDBG_MSG(("%s: DM either failed to return PTS or returned 0 as current PTS, use the lastUsedPts %x as the currentPts", __FUNCTION__, playback_ip->lastUsedPts));
        currentPts = playback_ip->lastUsedPts;
    }

    if (currentPts < playback_ip->firstPts) {
        /* shouldn't happen as currentPts returned by the DM corresponds to the last frame before the discontinuity */
        /* and this whole segment should have been contigous, so something is not as we expect */
        /* note: it is confusing from the decoder status comment, but this currentPts is not the PTS corresponding to the frame after the discontinuity */
        BDBG_ERR(("%s: ERROR: current PTS %x is smaller than firstPTS %x, nextPts %x, streamDurationUntilLastDiscontinuity %d msec", __FUNCTION__, currentPts, playback_ip->firstPts, nextPts, playback_ip->streamDurationUntilLastDiscontinuity));
        currentContigousStreamDuration = 0;
    }
    else if (currentPts == playback_ip->ptsDuringLastDiscontinuity) {
        /* current decoded pts still happens to be same as the one during previous discontinuity, ignore it */
        /* this can happen when we get back to back PTS discontinuities */
        BDBG_MSG(("%s: current PTS %x is same as ptsDuringLastDiscontinuity %x, nextPts %x, streamDurationUntilLastDiscontinuity %d msec", __FUNCTION__, currentPts, playback_ip->ptsDuringLastDiscontinuity, nextPts, playback_ip->streamDurationUntilLastDiscontinuity));
        currentContigousStreamDuration = 0;
    }
    else {
        /* currentPts is good, so calculate the duration of the contigous segment upto this point */
        currentContigousStreamDuration = (currentPts - playback_ip->firstPts) / 45;
    }
    playback_ip->streamDurationUntilLastDiscontinuity += currentContigousStreamDuration;
    BDBG_ERR(("%s: contingous segment's firstPts %x, lastPts %x, nextPts %x, total streamDurationUntilLastDiscontinuity %d msec", __FUNCTION__, playback_ip->firstPts, currentPts, nextPts, playback_ip->streamDurationUntilLastDiscontinuity));
    playback_ip->firstPtsBeforePrevDiscontinuity = playback_ip->firstPts;
    playback_ip->firstPts = nextPts; /* this nextPts becomes the first PTS of segment after discontinuity */
    /* dont reset lastUsedPts to nextPts, instead to the currentPts (which is the last pts before this discontinuity */
    /* this allows us to correctly calculate the position if app asks for it before we have a chance to display the next picture */
    playback_ip->lastUsedPts = currentPts;
    playback_ip->ptsDuringLastDiscontinuity = currentPts;
    playback_ip->getNewPtsAfterDiscontinuity = true;
    return;
}

/* 
   This callback is invoked when 1st frame is decoded:
   - after the initial start of stream playback
   - after seek operation
   - after trickmode operation
   - after resuming from trickmode operation
   It sets the firstPts when this stream is being played for the very 1st time. 
   In other cases, it tries to determine the stream discontinuity after seek or trickplay operation
   and accordingly update the firstPts.
*/
static void 
firstPtsCallback(void *context, int param)
{
    uint32_t currentPts;
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)context;

    BSTD_UNUSED(param);
    BDBG_ASSERT(playback_ip);

    /* get pts corresponding to the 1st frame */
    if ((http_get_current_pts(playback_ip, &currentPts) != B_ERROR_SUCCESS) || currentPts == 0) {
        BDBG_MSG(("%s: DM either failed to return PTS or returned 0 as current PTS, not setting the firstPts", __FUNCTION__));
        return;
    }

    /* reset the mediaStartTime as this callback can also come after a seek or av decoder flush */
    playback_ip->mediaStartTimeNoted = false;
    if (!playback_ip->lastSeekPositionSet) {
#if 0
        /* in case of Boxee, where stream can have lots of artifacts, AV decoders cat get stuck. */
        /* stcChannel/pcrlib is flushing the decoder fifos and thus 1stPts interrupt is getting called again */
        /* we may need to not reset this value and instead add up the current time upto now into this */
        playback_ip->streamDurationUntilLastDiscontinuity = 0;
#else
        playback_ip->streamDurationUntilLastDiscontinuity += (playback_ip->lastUsedPts-playback_ip->firstPts)/45;
        BDBG_MSG(("%s: first pts 0x%x, last pts 0x%x, additional duration %d, totoal dur %d", __FUNCTION__, 
                    playback_ip->firstPts, playback_ip->lastUsedPts, (playback_ip->lastUsedPts-playback_ip->firstPts)/45, playback_ip->streamDurationUntilLastDiscontinuity));
#endif
        /* if lastSeekPosition is not set, then we haven't yet done either pause/resume w/ disconnect/reconnect method, seek, or a trickplay */
        /* So this has to be the PTS corresponding to the 1st frame of this stream. We note this as firstPts for the current position calculations. */
        playback_ip->firstPts = currentPts;
        playback_ip->lastUsedPts = currentPts;
        BDBG_MSG(("%s: first decoded pts 0x%x", __FUNCTION__, currentPts));
    }
    else {
        /* lastSeekPosition is set, so we have done either pause/resume w/ disconnect/reconnect method, seek, or a trickplay */
        /* we compute the currentPositionUsingPts = currentPts - firstPts; and compare it to the lastSeekPosition */
        /* if currentPositionUsingPts is not way too off from the lastSeekPosition, we consider it more accurate */
        /* (as server may not be able to accurately seek to the lastSeekPosition) and thus dont change the 1st pts */
        /* Otherwise, there may have been PTS discontinuity in the content we skipped over due to trickplay and thus our 1st pts will need to be reset */
        NEXUS_PlaybackPosition currentPositionUsingPts;
        uint32_t positionDelta;
        int i;

        /* calculate the current position using pts */
        currentPositionUsingPts = (currentPts - playback_ip->firstPts)/45;

        if (playback_ip->streamDurationUntilLastDiscontinuity) {
            if (currentPts <= playback_ip->firstPts) {
                /* we had a PTS discontinuity & now new position is before this contigous block, so reset this duration */
                playback_ip->streamDurationUntilLastDiscontinuity = 0;
                /* pts will get reset below as positionDelta would be too big */
            }
            else {
                /* we had a discontinuity, but new position is after the firstPts, so account for this duration until last discontinuity */
                currentPositionUsingPts += playback_ip->streamDurationUntilLastDiscontinuity;
            }
        }

        /* now compare this pts computed position to the we had actually seeked or resumed from */
        i = currentPositionUsingPts - playback_ip->lastSeekPosition; 
        positionDelta = abs(i);
#define MAX_POSITION_DELTA 10000
        if (positionDelta > MAX_POSITION_DELTA) { 
            /* two positions are differ by over a max, so there must be PTS discontinuity. Reset 1st PTS. */
            playback_ip->streamDurationUntilLastDiscontinuity = playback_ip->lastSeekPosition;
            BDBG_MSG(("%s: PTS discontinuity: Current Position %u after trickplay using PTS calculation is off from lastSeekedPosition %d by over %d msec, max position delta %d, reset first pts from 0x%x to 0x%x, streamDurationUntilLastDiscontinuity %d msec", 
                        __FUNCTION__, currentPositionUsingPts, playback_ip->lastSeekPosition, positionDelta, MAX_POSITION_DELTA, playback_ip->firstPts, currentPts, playback_ip->streamDurationUntilLastDiscontinuity));
            playback_ip->firstPts = currentPts;
            playback_ip->lastUsedPts = currentPts;
        }
        else {
            BDBG_MSG(("%s: Current Position %u after trickplay using PTS calculation is more accurate than the one from lastSeekedPosition %d, not resetting first pts from 0x%x to 0x%x, streamDurationUntilLastDiscontinuity %d msec", 
                        __FUNCTION__, currentPositionUsingPts, playback_ip->lastSeekPosition, playback_ip->firstPts, currentPts, playback_ip->streamDurationUntilLastDiscontinuity));
            playback_ip->lastUsedPts = currentPts;
        }
        /* reset lastSeekPosition */
        playback_ip->lastSeekPosition = 0;
        playback_ip->lastSeekPositionSet = false;
    }

    B_Time_Get(&playback_ip->mediaStartTime);
    playback_ip->mediaStartTimeNoted = true;
}

static void 
firstPtsPassedCallback(void *context, int param)
{
    uint32_t currentPts;
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)context;

    BSTD_UNUSED(param);
    BDBG_ASSERT(playback_ip);

    /* get pts corresponding to the 1st frame */
    if ((http_get_current_pts(playback_ip, &currentPts) != B_ERROR_SUCCESS) || currentPts == 0) {
        BDBG_MSG(("%s: DM either failed to return PTS or returned 0 as current PTS, not setting the firstPts", __FUNCTION__));
        return;
    }

    BDBG_MSG(("%s: first passed pts 0x%x", __FUNCTION__, currentPts));
}

static void 
sourceChangedCallback(void *context, int param)
{
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)context;
    NEXUS_VideoDecoderStatus status;
    BSTD_UNUSED(param);
    if (playback_ip->nexusHandles.videoDecoder) {
        if (NEXUS_VideoDecoder_GetStatus(playback_ip->nexusHandles.videoDecoder, &status) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: NEXUS_VideoDecoder_GetStatus() Failed", __FUNCTION__));
            return;
        }
        BDBG_WRN(("%s: res: source %dx%d, coded %dx%d, display %dx%d, ar %d, fr %d, interlaced %d video format %d, muted %d", __FUNCTION__,
                    status.source.width, status.source.height, 
                    status.coded.width, status.coded.height, 
                    status.display.width, status.display.height, 
                    status.aspectRatio, 
                    status.frameRate, 
                    status.interlaced, 
                    status.format, 
                    status.muted
                    ));
        playback_ip->frameRate = status.frameRate;
    }
#ifdef NEXUS_HAS_SIMPLE_DECODER
    else if (playback_ip->nexusHandles.simpleVideoDecoder) {
        if (NEXUS_SimpleVideoDecoder_GetStatus(playback_ip->nexusHandles.simpleVideoDecoder, &status) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: NEXUS_VideoDecoder_GetStatus() Failed", __FUNCTION__));
            return;
        }
        BDBG_WRN(("%s: res: source %dx%d, coded %dx%d, display %dx%d, ar %d, fr %d, interlaced %d video format %d, muted %d", __FUNCTION__,
                    status.source.width, status.source.height, 
                    status.coded.width, status.coded.height, 
                    status.display.width, status.display.height, 
                    status.aspectRatio, 
                    status.frameRate, 
                    status.interlaced, 
                    status.format, 
                    status.muted
                    ));
        playback_ip->frameRate = status.frameRate;
    }
#endif
}

B_PlaybackIpError 
B_PlaybackIp_SessionStart(
    B_PlaybackIpHandle playback_ip,
	B_PlaybackIpSessionStartSettings *startSettings,
	B_PlaybackIpSessionStartStatus *startStatus /* [out] */
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;
    NEXUS_VideoDecoderSettings videoDecoderSettings;

    BDBG_MSG(("%s: playback_ip %p, startSettings %p, startStatus %p\n", __FUNCTION__, playback_ip, startSettings, startStatus));
    if (!playback_ip || !startSettings || !startStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, startSettings %p, startStatus %p\n", __FUNCTION__, playback_ip, startSettings, startStatus));
        return B_ERROR_INVALID_PARAMETER;
    }

    /* Validate if app can call this API in the current IP Playback State */
    switch (playback_ip->playback_state) {
    case B_PlaybackIpState_eSessionOpened:
        if (playback_ip->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eHttp ||
            playback_ip->openSettings.socketOpenSettings.protocol == B_PlaybackIpProtocol_eRtsp) {
            /* SessionSetup needs to be called at least once for the HTTP/RTSP protocols */
            if (!playback_ip->sessionSetupCompleted) {
                BDBG_ERR(("%s: ERROR: Ip_SessionSetup() needs to be called before Ip_SessionStart() for HTTP/RTSP protocols, ip state %d", 
                        __FUNCTION__, playback_ip->playback_state));
                return B_ERROR_NOT_SUPPORTED;
            }
            else {
                /* session setup has been completed atleast once for this session, so we can proceed w/ start below */
                break;
            }
        }
        else {
            /* For all other protocols, app can call SessionStart w/o calling SessionSetup */
            /* so continue below with this API */
            break;
        }
    case B_PlaybackIpState_eSessionSetup:
    case B_PlaybackIpState_eSessionStartInProgress:
        /* continue below with this API */
        break;
    default:
        /* In all other states, app can't call this API */
        BDBG_ERR(("ERROR: Can't call %s() in this state %d\n", __FUNCTION__, playback_ip->playback_state));
        return B_ERROR_NOT_SUPPORTED;
    }

    /* now API is progress, update state to reflect that */
    playback_ip->playback_state = B_PlaybackIpState_eSessionStartInProgress;

    memset(startStatus, 0, sizeof(B_PlaybackIpSessionStartStatus));
    playback_ip->startSettings = *startSettings;
    if (startSettings->nexusHandlesValid)
        playback_ip->nexusHandles = startSettings->nexusHandles;
    playback_ip->byte_count = 0;
    playback_ip->numRecvTimeouts = 0;

    if (playback_ip->nexusHandles.videoDecoder) {
        NEXUS_VideoDecoder_GetSettings(playback_ip->nexusHandles.videoDecoder, &videoDecoderSettings);
        videoDecoderSettings.firstPts.callback = firstPtsCallback;
        videoDecoderSettings.firstPts.context = playback_ip;
        videoDecoderSettings.firstPtsPassed.callback = firstPtsPassedCallback;
        videoDecoderSettings.firstPtsPassed.context = playback_ip;
        videoDecoderSettings.sourceChanged.callback = sourceChangedCallback;
        videoDecoderSettings.sourceChanged.context = playback_ip;
		videoDecoderSettings.ptsError.callback = ptsErrorCallback;
        videoDecoderSettings.ptsError.context = playback_ip;
        if (NEXUS_VideoDecoder_SetSettings(playback_ip->nexusHandles.videoDecoder, &videoDecoderSettings) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to set the 1st pts callback\n", __FUNCTION__));
            goto error;
        }
    }
#ifdef NEXUS_HAS_SIMPLE_DECODER
    else if (playback_ip->nexusHandles.simpleVideoDecoder){
        NEXUS_SimpleVideoDecoder_GetSettings(playback_ip->nexusHandles.simpleVideoDecoder, &videoDecoderSettings);
        videoDecoderSettings.firstPts.callback = firstPtsCallback;
        videoDecoderSettings.firstPts.context = playback_ip;
        videoDecoderSettings.firstPtsPassed.callback = firstPtsPassedCallback;
        videoDecoderSettings.firstPtsPassed.context = playback_ip;
        videoDecoderSettings.sourceChanged.callback = sourceChangedCallback;
        videoDecoderSettings.sourceChanged.context = playback_ip;
		videoDecoderSettings.ptsError.callback = ptsErrorCallback;
        videoDecoderSettings.ptsError.context = playback_ip;
        if (NEXUS_SimpleVideoDecoder_SetSettings(playback_ip->nexusHandles.simpleVideoDecoder, &videoDecoderSettings) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to set the 1st pts callback\n", __FUNCTION__));
            goto error;
        }
    }
#endif

    switch (playback_ip->openSettings.socketOpenSettings.protocol) {
#ifndef B_HAS_SMS_GATEWAY
    case B_PlaybackIpProtocol_eHttp:
        errorCode = B_PlaybackIp_HttpSessionStart(playback_ip, startSettings, startStatus);
        break;
#endif
#ifdef LIVEMEDIA_SUPPORT
    case B_PlaybackIpProtocol_eRtsp:
        errorCode = B_PlaybackIp_RtspSessionStart(playback_ip, startSettings, startStatus);
        break;
#endif

#ifndef ANDROID 
    case B_PlaybackIpProtocol_eRtp:
    case B_PlaybackIpProtocol_eRtpNoRtcp:
        errorCode = B_PlaybackIp_RtpSessionStart(playback_ip, startSettings, startStatus);
        break;
#endif
        
    case B_PlaybackIpProtocol_eUdp:
        errorCode = B_PlaybackIp_UdpSessionStart(playback_ip, startSettings, startStatus);
        break;
    case B_PlaybackIpProtocol_eRtpFec:
    default:
        BDBG_MSG(("%s: Session start failed: protocol %d not supported", __FUNCTION__, playback_ip->openSettings.socketOpenSettings.protocol));
        errorCode = B_ERROR_INVALID_PARAMETER;
        break;
    }

    if (errorCode == B_ERROR_IN_PROGRESS)
        return B_ERROR_IN_PROGRESS;
    else if (errorCode != B_ERROR_SUCCESS) 
        goto error;

    /* now this API is successfully completed, update state to reflect that */
    playback_ip->playback_state = B_PlaybackIpState_ePlaying;
    BDBG_MSG(("%s() completed successfully, playback_ip %p\n", __FUNCTION__, playback_ip));
    return B_ERROR_SUCCESS;

error:
    playback_ip->playback_state = B_PlaybackIpState_eSessionSetup;
    return errorCode;
}

/*
Summary:
    Stops IP playback.
Description:
    This function must be called when app no longer wants to tune to a live IP or a playback channel over network.
    Depending on the current state of the playback IP channel, this function stops the IP internal thread
    and does some protocol specific cleanup (stop reading data from socket, etc.). It then changes
    state to eStopped.
Note:
    Socket is not closed in this call. Thus, App can temporarily stop receiving live data and then
    restart receiving data on this socket using the B_PlaybackIp_Start().
    App must call B_PlaybackIp_SessionClose() to free up the socket related resources.
    Also, this is a blocking function call.
*/
B_PlaybackIpError B_PlaybackIp_SessionStop(
    B_PlaybackIpHandle playback_ip
    )
{
    BERR_Code rc;
#ifdef B_HAS_NETACCEL
    STRM_SockSetFilterState_t sockFilterState;
#endif

    BDBG_MSG(("%s: playback ip state %d\n", __FUNCTION__, playback_ip->playback_state));
    switch (playback_ip->playback_state) {
    case B_PlaybackIpState_eOpened:
    case B_PlaybackIpState_eSessionOpenInProgress:
    case B_PlaybackIpState_eSessionOpened:
    case B_PlaybackIpState_eSessionSetup:
    case B_PlaybackIpState_eStopping:
    case B_PlaybackIpState_eSessionSetupInProgress:
    /* case B_PlaybackIpState_eStopped:*/
    default:
        BDBG_MSG(("%s: nothing to stop in this state %d... ", __FUNCTION__, playback_ip->playback_state));
        return B_ERROR_SUCCESS;

    case B_PlaybackIpState_eSessionStartInProgress:
    case B_PlaybackIpState_ePlaying:
    case B_PlaybackIpState_eBuffering:
    case B_PlaybackIpState_ePaused:
    case B_PlaybackIpState_eEnteringTrickMode:
    case B_PlaybackIpState_eTrickMode:
        /* Continue below to Stop the IP Session */
        break;
    }

    /* change to stopping state as stopping the IP thread can take some time */
    playback_ip->playback_state = B_PlaybackIpState_eStopping;

    rc = BKNI_WaitForEvent(playback_ip->playback_halt_event, IP_HALT_TASK_TIMEOUT_MSEC);
    if (rc == BERR_TIMEOUT) {
        BDBG_WRN(("%s: playback_halt_event was timed out", __FUNCTION__));
    } else if (rc!=0) {
        BDBG_ERR(("%s: failed to stop the IP thread: playback_halt_event timed out due to error rc = %d", __FUNCTION__, rc));
        goto error;
    }
    B_Thread_Destroy(playback_ip->playbackIpThread);
    BDBG_MSG(("%s: Playback IP thread is stopped \n", __FUNCTION__));

    switch (playback_ip->openSettings.socketOpenSettings.protocol) {
    case B_PlaybackIpProtocol_eUdp:
    case B_PlaybackIpProtocol_eRtpFec:
        break;
#ifndef B_HAS_SMS_GATEWAY
    case B_PlaybackIpProtocol_eHttp:
        B_PlaybackIp_HttpSessionStop(playback_ip);
        break;
#endif

#ifndef ANDROID 
    case B_PlaybackIpProtocol_eRtp:
    case B_PlaybackIpProtocol_eRtsp:
    case B_PlaybackIpProtocol_eRtpNoRtcp:
        B_PlaybackIp_RtpSessionStop(playback_ip);
        break;
#endif        
        
    default:
        BDBG_WRN(("%s: Bad protocol", __FUNCTION__));
        break;
    }

    if (playback_ip->protocol != B_PlaybackIpProtocol_eHttp) {
#ifdef B_HAS_NETACCEL
        /* filter only needs to be disbled for live protocols */
        sockFilterState.filterEnable = 0;
        if (setsockopt(playback_ip->socketState.fd, SOCK_BRCM_DGRAM, STRM_SOCK_SET_FILTER_STATE, &sockFilterState, sizeof(sockFilterState)) != 0)
        {
            BDBG_ERR(("%s: setsockopt() ERROR:", __FUNCTION__));
        }
        BDBG_ERR(("%s: Disabled Net DMA filter\n", __FUNCTION__));
#endif
    }

    playback_ip->playback_state = B_PlaybackIpState_eStopped; /* Note: this state is same as eSessionOpened, this duplicate name is aimed to provide clarity */
    return B_ERROR_SUCCESS;

error:
    BDBG_ERR(("%s: error", __FUNCTION__));
    return B_ERROR_UNKNOWN;
}

/*
Summary:
    Close a IP playback socket.
Description:
    Close a IP playback socket opened by bplayback_socketState_open(). Playback
    must be stopped BEFORE socket is closed
*/
B_PlaybackIpError B_PlaybackIp_SessionClose(
    B_PlaybackIpHandle playback_ip /* Handle returned by bplayback_ip_open */
    )
{
    BERR_Code rc;

    /* TODO: if state is SessionOpenInProgress, need to cancel that action */

    switch (playback_ip->playback_state) {
    case B_PlaybackIpState_eOpened:
        /* since session is not yet started, there is nothing to close, return */
        return B_ERROR_SUCCESS;

    case B_PlaybackIpState_eSessionStartInProgress:
    case B_PlaybackIpState_ePlaying:
    case B_PlaybackIpState_eBuffering:
    case B_PlaybackIpState_ePaused:
    case B_PlaybackIpState_eEnteringTrickMode:
    case B_PlaybackIpState_eTrickMode:
        B_PlaybackIp_SessionStop(playback_ip);
        /* continue below w/ session close cleanup */
        
    case B_PlaybackIpState_eSessionSetupInProgress:
        /* temporarily change state to Stopping to allow media probing to fail & return */
        playback_ip->playback_state = B_PlaybackIpState_eStopping;
        rc = BKNI_WaitForEvent(playback_ip->playback_halt_event, IP_HALT_TASK_TIMEOUT_MSEC);
        if (rc == BERR_TIMEOUT) {
            BDBG_WRN(("%s: playback_halt_event was timed out", __FUNCTION__));
        } else if (rc!=0) {
            BDBG_ERR(("%s: failed to stop the media probe thread: playback_halt_event timed out due to error rc = %d", __FUNCTION__, rc));
        }
    case B_PlaybackIpState_eSessionOpenInProgress:
    case B_PlaybackIpState_eSessionSetup:
    case B_PlaybackIpState_eStopped:
    /* case B_PlaybackIpState_eSessionOpened: */
        /* continue below w/ session close cleanup */
        break;
    default:
    case B_PlaybackIpState_eStopping:
        /* error */
        BDBG_ERR(("ERROR: can't call %s() in this state %d as another thread is concurrently calling Ip_SessionStop()", __FUNCTION__, playback_ip->playback_state));
        return B_ERROR_NOT_SUPPORTED;
    }

    BDBG_MSG(("%s: ip state %d", __FUNCTION__, playback_ip->playback_state));

    /* switch to the opened state, this channel can be used for another session */
    playback_ip->playback_state = B_PlaybackIpState_eOpened;

    /* do socket related cleanup */
    close(playback_ip->socketState.fd);
    memset(&playback_ip->socketState, 0, sizeof(playback_ip->socketState));
    BDBG_MSG(("%s: closed fd %d (cur state %d)", __FUNCTION__, playback_ip->socketState.fd, playback_ip->playback_state));
    playback_ip->socketState.fd = 0;

    switch (playback_ip->protocol) {
#ifndef B_HAS_SMS_GATEWAY
    case B_PlaybackIpProtocol_eHttp:
        B_PlaybackIp_HttpSessionClose(playback_ip);
        break;
#endif
    case B_PlaybackIpProtocol_eUdp:
        B_PlaybackIp_UdpSessionClose(playback_ip);
        break;
        
#ifndef ANDROID         
    case B_PlaybackIpProtocol_eRtp:
    case B_PlaybackIpProtocol_eRtsp:
    case B_PlaybackIpProtocol_eRtpNoRtcp:
        B_PlaybackIp_RtpSessionClose(playback_ip);
        break;
#endif
        
    default:
        break;
    }

    if (playback_ip->openSettings.socketOpenSettings.url)
        free(playback_ip->openSettings.socketOpenSettings.url);
    BKNI_DestroyEvent(playback_ip->playback_halt_event);
    playback_ip->playback_halt_event = NULL;
    BKNI_DestroyEvent(playback_ip->preChargeBufferEvent);
    BKNI_DestroyEvent(playback_ip->newJobEvent);
    BKNI_DestroyEvent(playback_ip->read_callback_event);
    BKNI_DestroyEvent(playback_ip->liveMediaSyncEvent);
    if (playback_ip->lock) {
        BKNI_DestroyMutex(playback_ip->lock);
        playback_ip->lock = NULL;
    }
#ifdef B_HAS_NETACCEL
	NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eDma, false);
#endif
    return B_ERROR_SUCCESS;
}

/* routine to return the Playback IP Status */
B_PlaybackIpError
B_PlaybackIp_GetStatus(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpStatus *ipStatus
    )
{
    B_PlaybackIpError rc = B_ERROR_SUCCESS;
    int cacheIndex;
    if (!ipStatus || !playback_ip) {
        return B_ERROR_INVALID_PARAMETER;
    }
    BKNI_Memset(ipStatus, 0, sizeof(*ipStatus));
    ipStatus->ipState = playback_ip->playback_state;
#ifndef B_HAS_SMS_GATEWAY
    cacheIndex = playback_ip->lastUsedCacheIndex;
    if (playback_ip->dataCache[cacheIndex].inUse && playback_ip->psi.avgBitRate) {
        ipStatus->maxBufferDuration = ((playback_ip->dataCache[cacheIndex].size - playback_ip->cacheDepthFudgeFactor)*8) / playback_ip->psi.avgBitRate;  /* in sec */
        ipStatus->maxBufferDuration *= 1000; /* in msec */
        if (playback_ip->lastSeekOffset < playback_ip->dataCache[cacheIndex].startOffset || playback_ip->lastSeekOffset > playback_ip->dataCache[cacheIndex].endOffset) {
            /* we have seeked or are trying to play from outside the current cache range, so our buffer duration is 0 */
            ipStatus->curBufferDuration = 0;
        }
        else {
            /* seek/play point is in the cache */
            ipStatus->curBufferDuration = ((playback_ip->dataCache[cacheIndex].endOffset - playback_ip->lastSeekOffset)*8) / playback_ip->psi.avgBitRate;  /* in sec */
            ipStatus->curBufferDuration *= 1000; /* in msec */
        }
    }
    ipStatus->numRecvTimeouts = playback_ip->numRecvTimeouts;
    ipStatus->serverClosed = playback_ip->serverClosed;
    ipStatus->httpStatusCode = playback_ip->statusCode;

    if (B_PlaybackIp_HttpGetCurrentPlaybackPosition(playback_ip, &ipStatus->position) != B_ERROR_SUCCESS) {
        BDBG_MSG(("%s: Failed to determine the current playback position, setting it to 0\n", __FUNCTION__));
        ipStatus->position = 0;
    }

#if 0
    BDBG_MSG(("%s: buffer duration: cur %d, max %d, state %d, closed %d, offsets: last seeked %lld, end %lld, start %lld, current position %u\n",
                __FUNCTION__, ipStatus->curBufferDuration ,ipStatus->maxBufferDuration, ipStatus->ipState, playback_ip->serverClosed, playback_ip->lastSeekOffset, playback_ip->dataCache[cacheIndex].endOffset, playback_ip->dataCache[cacheIndex].startOffset, ipStatus->position));
#endif
#else
    BSTD_UNUSED(cacheIndex);
#endif

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT               
    if (playback_ip->hlsSessionEnabled) {
        ipStatus->first = 0;
        if (playback_ip->hlsSessionState && playback_ip->hlsSessionState->currentPlaylistFile)
            ipStatus->last = playback_ip->hlsSessionState->currentPlaylistFile->totalDuration;
    }
#endif /* B_HAS_HLS_PROTOCOL_SUPPORT */

#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT               
    if (playback_ip->mpegDashSessionEnabled) {
        ipStatus->first = 0;
        if (playback_ip->mpegDashSessionState && playback_ip->mpegDashSessionState->currentPlaylistFile)
            ipStatus->last = playback_ip->mpegDashSessionState->currentPlaylistFile->totalDuration;
    }
#endif /* B_HAS_MPEG_DASH_PROTOCOL_SUPPORT */

#ifdef LIVEMEDIA_SUPPORT
    if((playback_ip->protocol == B_PlaybackIpProtocol_eRtp) || (playback_ip->protocol == B_PlaybackIpProtocol_eRtsp)) {
        B_PlaybackIp_liveMediaSessionRange(playback_ip->lm_context, &ipStatus->position, &ipStatus->last);
        ipStatus->first = 0;
    }
#endif

    rc = B_ERROR_SUCCESS;
    return rc;
}

#if !defined(B_HAS_SMS_GATEWAY) || defined(SMS93383_SUPPORT)
void print_av_pipeline_buffering_status(
    B_PlaybackIpHandle playback_ip
    )
{
    NEXUS_VideoDecoderStatus videoStatus;
    NEXUS_AudioDecoderStatus audioStatus;
    NEXUS_PlaybackStatus playbackStatus;

    if (playback_ip->nexusHandles.videoDecoder) {
        if (NEXUS_VideoDecoder_GetStatus(playback_ip->nexusHandles.videoDecoder, &videoStatus) == NEXUS_SUCCESS) {
            BDBG_WRN(("Video Decoder Status: pts %u, fifoSize %d, fifoDepth %d, fullness %d%%",
                videoStatus.pts, videoStatus.fifoSize, videoStatus.fifoDepth, videoStatus.fifoSize?(videoStatus.fifoDepth*100)/videoStatus.fifoSize:0));
        }
    }
    if (playback_ip->nexusHandles.primaryAudioDecoder) {
        if (NEXUS_AudioDecoder_GetStatus(playback_ip->nexusHandles.primaryAudioDecoder, &audioStatus) == NEXUS_SUCCESS) {
            BDBG_WRN(("Audio Decoder Status: pts %u, fifoSize %d, fifoDepth %d, fullness %d%%",
                audioStatus.pts, audioStatus.fifoSize, audioStatus.fifoDepth, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0));
        }
    }
    if (playback_ip->nexusHandles.playback) {
        if (NEXUS_Playback_GetStatus(playback_ip->nexusHandles.playback, &playbackStatus) == NEXUS_SUCCESS) {
            BDBG_WRN(("Playback Status: PB buffer depth %d, size %d, fullness %d%%, played bytes %lld",
                    playbackStatus.fifoDepth, playbackStatus.fifoSize, (playbackStatus.fifoDepth*100)/playbackStatus.fifoSize, playbackStatus.bytesPlayed));
        }
    }
}
#endif

/***************************************************************************
Summary:
This function returns the default and recommended values for the Playback IP App Lib
public settings. A pointer to a valid B_PlaybackIpSettings structure must be
provide or an error will be returned.
***************************************************************************/
B_PlaybackIpError B_PlaybackIp_GetDefaultSettings(
    B_PlaybackIpSettings *pSettings)
{
    if (NULL == pSettings)
        return B_ERROR_INVALID_PARAMETER;
    BKNI_Memset( pSettings, 0, sizeof(B_PlaybackIpSettings) );

    pSettings->ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
    pSettings->maxNetworkJitter = 300;
    pSettings->preChargeBuffer = false;
    pSettings->networkTimeout = HTTP_SELECT_TIMEOUT;
    pSettings->networkBufferSize = HTTP_DATA_CACHE_SIZE;
    pSettings->useNexusPlaypump = false;
    B_PlaybackIp_TtsThrottle_ParamsInit(&pSettings->ttsParams.throttleParams);
    return B_ERROR_SUCCESS;
}

/***************************************************************************
Summary:
This function returns the current values for the Playback IP App Lib
public settings.
***************************************************************************/
B_PlaybackIpError B_PlaybackIp_GetSettings(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSettings *pSettings)
{
    if (NULL == pSettings)
        return B_ERROR_INVALID_PARAMETER;

    BKNI_Memcpy(pSettings, &playback_ip->settings, sizeof(B_PlaybackIpSettings) );
    return B_ERROR_SUCCESS;
}

/***************************************************************************
Summary:
This function updates the current values for the Playback IP App Lib
public settings.
***************************************************************************/
B_PlaybackIpError B_PlaybackIp_SetSettings(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSettings *pSettings)
{
    BERR_Code rc;

    BDBG_ASSERT(playback_ip);
    if (NULL == pSettings)
        return B_ERROR_INVALID_PARAMETER;

    /* update Nexus Handles if app has supplied them */
    if (pSettings->nexusHandlesValid) {
        if (pSettings->nexusHandles.playpump)
            playback_ip->nexusHandles.playpump = pSettings->nexusHandles.playpump;
        if (pSettings->nexusHandles.playback)
            playback_ip->nexusHandles.playback = pSettings->nexusHandles.playback;
        if (pSettings->nexusHandles.videoDecoder)
            playback_ip->nexusHandles.videoDecoder = pSettings->nexusHandles.videoDecoder;
        if (pSettings->nexusHandles.simpleVideoDecoder)
            playback_ip->nexusHandles.simpleVideoDecoder = pSettings->nexusHandles.simpleVideoDecoder;
        if (pSettings->nexusHandles.simpleAudioDecoder)
            playback_ip->nexusHandles.simpleAudioDecoder = pSettings->nexusHandles.simpleAudioDecoder;
        if (pSettings->nexusHandles.primaryAudioDecoder)
            playback_ip->nexusHandles.primaryAudioDecoder = pSettings->nexusHandles.primaryAudioDecoder;
        if (pSettings->nexusHandles.secondaryAudioDecoder)
            playback_ip->nexusHandles.secondaryAudioDecoder = pSettings->nexusHandles.secondaryAudioDecoder;
        if (pSettings->nexusHandles.stcChannel)
            playback_ip->nexusHandles.stcChannel = pSettings->nexusHandles.stcChannel;
    }

    BDBG_MSG(("%s: preChargeBuffer %d, ipState %d", __FUNCTION__, pSettings->preChargeBuffer, playback_ip->playback_state));
    switch (playback_ip->playback_state) {
    case B_PlaybackIpState_ePlaying:
        if (pSettings->preChargeBuffer) {
            if (!playback_ip->psi.avgBitRate) {
                BDBG_ERR(("%s: Can't enable Runtime Buffering since we dont know the avg stream bitrate", __FUNCTION__));
                return B_ERROR_INVALID_PARAMETER;
            }

            /* user set the preChargeBuffer flag, tell Http thread to start pre-charging work */
            playback_ip->preChargeBuffer = true;
            /* send event to let http thread know app would like it to start buffering */
            BKNI_SetEvent(playback_ip->newJobEvent);

            /* now wait for http thread to acknowledge start of buffering */
            rc = BKNI_WaitForEvent(playback_ip->preChargeBufferEvent, HTTP_PRE_CHARGE_EVENT_TIMEOUT);
            if (rc == BERR_TIMEOUT) {
                BDBG_WRN(("%s: timed out for pre-charging complete event", __FUNCTION__));
                return B_ERROR_UNKNOWN;
            } else if (rc!=0) {
                BDBG_WRN(("%s: got error while trying to wait for pre-charging complete event, rc %d", __FUNCTION__, rc));
                return B_ERROR_UNKNOWN;
            }
            BDBG_MSG(("%s: Enabled pre-charging of network buffer", __FUNCTION__));
        }
        /* can't change any settings in playing state, we ignore the settings */
        return B_ERROR_SUCCESS;
    case B_PlaybackIpState_eBuffering:
        if (!pSettings->preChargeBuffer) {
            /* we are currently pre-charging and user wants us to stop pre-charging */
            playback_ip->preChargeBuffer = false;
            rc = BKNI_WaitForEvent(playback_ip->preChargeBufferEvent, HTTP_PRE_CHARGE_EVENT_TIMEOUT);
            if (rc == BERR_TIMEOUT) {
                BDBG_WRN(("%s: timed out for pre-charging complete event", __FUNCTION__));
                return B_ERROR_UNKNOWN;
            } else if (rc!=0) {
                BDBG_WRN(("%s: got error while trying to wait for pre-charging complete event, rc %d", __FUNCTION__, rc));
                return B_ERROR_UNKNOWN;
            }
            BDBG_MSG(("%s: Stopped pre-charging of network buffer", __FUNCTION__));
        }
        /* can't change any settings in buffering state, we ignore the settings */
        return B_ERROR_SUCCESS;
    break;
    default:
        if (pSettings->preChargeBuffer) {
            BDBG_ERR(("%s: IP Playback Channel is not yet setup to be able to pre-charge network buffer (current state %d)\n", __FUNCTION__, playback_ip->playback_state));
            return B_ERROR_INVALID_PARAMETER;
        }
        /* states other than ePlaying or eBuffering, update settings */
        if (pSettings->useNexusPlaypump)
            playback_ip->useNexusPlaypump = true;
        BKNI_Memcpy(&playback_ip->settings, pSettings, sizeof(B_PlaybackIpSettings) );
        return B_ERROR_SUCCESS;
    }
}

/***************************************************************************
Summary:
This function detects if the stream is TTS or not
***************************************************************************/
B_PlaybackIpError B_PlaybackIp_DetectTts(
    B_PlaybackIpHandle playback_ip,
    bool *isTts
    )
{
    int i;
    int ret;
    fd_set fds;
    struct timeval socketTimeout;
    struct sockaddr *from;
    socklen_t   fromLen;
    int bytesRecv = 0;
    B_PlaybackIpError rc = B_ERROR_SUCCESS;
    int offset = 0;
    int pktCount = 0;
    int tsHdrCount = 0;
    char *buf = &playback_ip->temp_buf[0];

#ifdef B_HAS_NETACCEL
    STRM_SockRecvParams_t sockRecvParams;
    STRM_SockSetFilterState_t filterState;

    memset(&sockRecvParams, 0, sizeof(sockRecvParams));
    sockRecvParams.pktsPerRecv = 1;

    switch(playback_ip->protocol) {
    case B_PlaybackIpProtocol_eUdp:
    sockRecvParams.hdrOffset = sizeof(struct udp_hdr);
        break;
    case B_PlaybackIpProtocol_eRtp:
    case B_PlaybackIpProtocol_eRtpNoRtcp:
        sockRecvParams.hdrOffset = 0;
        buf = &playback_ip->temp_buf[8];
        break;
    default:
        sockRecvParams.hdrOffset = 0;
        break;
    }
    if (setsockopt(playback_ip->socketState.fd, SOCK_BRCM_DGRAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)))
    {
        BDBG_ERR(("%s: setsockopt() ERROR:", __FUNCTION__));
        return B_ERROR_OS_ERROR;
    }
#endif

    BDBG_WRN(("%s: entering: %p", __FUNCTION__, playback_ip));

    if( IN_MULTICAST(ntohl(playback_ip->socketState.local_addr.sin_addr.s_addr)) ) {
        from = (struct sockaddr *) &playback_ip->socketState.remote_addr;
        fromLen = sizeof(struct sockaddr);
    }
    else {
        from = NULL;
        fromLen = 0;
    }

    for(i=0; ; i++) {
        if(i>=20) {
            rc = B_ERROR_UNKNOWN;
            goto error;
        }

        bytesRecv = recvfrom(playback_ip->socketState.fd,
                              (void *)playback_ip->temp_buf,
                              IP_MAX_PKT_SIZE,
                              0,
                              from,
                              &fromLen);

        if (bytesRecv==0) {
            BDBG_WRN(("server has closed connection"));
            goto wait;
        } else if (bytesRecv > 0) {
            /* got some data */
            goto done;
        } /* bytesRecv < 0 */

        if (errno == EINTR) {
            continue;
        }
#if defined(__vxworks)
        else if(errno != EWOULDBLOCK) {
            BDBG_ERR(("recvfrom error"));
        }
#else
        else if (errno != EAGAIN) {
            BDBG_ERR(("recvfrom error"));
            /* JJ - removed code: this is a normal condition that can occur when the server is overloaded */
        }
#endif

wait:
        /* no data received: wait for some */
        FD_ZERO(&fds);
        FD_SET(playback_ip->socketState.fd, &fds);
        socketTimeout.tv_sec = 0;
        socketTimeout.tv_usec = IP_RECV_TIMEOUT_USEC;
        /* BDBG_MSG_FLOW(("select")); */
        ret = select(playback_ip->socketState.fd+1, &fds, NULL, NULL, &socketTimeout);

        if (ret == 0) {
            BDBG_WRN(("Receive timeout"));
        } else if (ret<0 && errno != EINTR) {
            BDBG_ERR(("select error"));
            rc = B_ERROR_UNKNOWN;
            goto error;
        }
    }

done:

/* JJ - debug */
#if 0
    BDBG_WRN(("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
              buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15]));
#endif

    switch(playback_ip->protocol) {
    case B_PlaybackIpProtocol_eUdp:
        offset = 0;
        break;
    case B_PlaybackIpProtocol_eRtp:
    case B_PlaybackIpProtocol_eRtpNoRtcp:
    case B_PlaybackIpProtocol_eRtsp:
        {
            int cc;
            int ext_len = 0;
            bool ext = false;

            /* b_play_ip_parse_rtp_header(playback_ip->temp_buf); */
            if(((buf[0] & 0xC0) >> 6) != 2) goto error;
            if(buf[0] & 0x10) ext = true;
            cc = buf[0] & 0x0F;
            offset = 12+(cc*4);
            if(ext) {
                ext_len = ((buf[offset+2]<<8)+buf[offset+3]*4)+4;
                offset += ext_len;
            }
            BDBG_MSG(("RTP header size: %d", offset));
        }
        break;
    default:
        BDBG_WRN(("Bad protocol"));
        break;
    }

#ifdef B_HAS_NETACCEL
    switch(playback_ip->protocol) {
    case B_PlaybackIpProtocol_eUdp:
        break;
    case B_PlaybackIpProtocol_eRtp:
    case B_PlaybackIpProtocol_eRtpNoRtcp:
        bytesRecv -= 8;
        break;
    default:
        break;
    }
#endif

    for(i=offset; i<bytesRecv; i+=188) {
        pktCount++;
        if(buf[i] == 0x47) {
            tsHdrCount++;
        }
    }
    if(tsHdrCount==pktCount) {
        /* it's a TS stream */
        BDBG_MSG(("it's a TS stream"));
        *isTts = false;
    }
    else {
        pktCount = 0;
        tsHdrCount = 0;
        for(i=offset; i<bytesRecv; i+=192) {
            pktCount++;
            if(buf[i+4] == 0x47) {
                tsHdrCount++;
            }
        }
        if(tsHdrCount==pktCount) {
            /* it's a TTS stream */
            BDBG_MSG(("it's a TTS stream"));
            *isTts = true;
        }
        else {
            BDBG_WRN(("TS/TTS auto-detect fail!"));
            *isTts = false;
        }
    }

error:

#ifdef B_HAS_NETACCEL
    /* now disable the net dma filter */
    memset(&filterState, 0, sizeof(filterState));
    filterState.filterEnable = 0;
    sockRecvParams.pktsPerRecv = 1;

    if (setsockopt(playback_ip->socketState.fd, SOCK_BRCM_DGRAM, STRM_SOCK_SET_FILTER_STATE, &filterState, sizeof(filterState)))
    {
        BDBG_ERR(("%s: setsockopt() ERROR:", __FUNCTION__));
        return B_ERROR_OS_ERROR;
    }

    /* TODO: may need to drain any remaining packets from the recvq or */
    /* throw away 1 queue size worth (~256) initial packets in B_PlaybackIp_RtpProcessing() */
#endif

    return rc;
}

#endif /* LINUX || VxWorks */
