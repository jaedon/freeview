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
* $brcm_Workfile: b_playback_ip_utils.c $
* $brcm_Revision: 100 $
* $brcm_Date: 11/15/12 12:06a $
*
* Description: utility routines used by IP Applib
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_utils.c $
* 
* 100   11/15/12 12:06a ssood
* SW7231-858: fixed compilation warning for non-DTCP builds
* 
* 99   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 98   10/31/12 10:49p ssood
* SW7231-858: use a second connet call to determine the status of
*  previous non-blocking select call once select returns write ready for
*  this socket
* 
* 97   10/16/12 5:20p ssood
* SW7231-858: converted socket connect to non-blocking operation with a
*  timeout
* 
* 96   8/13/12 2:17p ssood
* SW7346-911: deduce the content length from Content-Range Header for
*  chunk encoding case if returned by server
* 
* 95   8/3/12 2:52p ssood
* SW7425-3678: extend ip lib & ip streamer to support streaming out live,
*  file, encoded content over RTP/UDP
* 
* 94   7/17/12 5:18p gskerl
* SW7346-853: Merged from branch SW7346-853 to main.
* 
* SW7346-853/2   7/10/12 3:16p gskerl
* SW7346-853: Updated branch SW7346-853 from main/LATEST.
* 
* SW7346-853/1   7/5/12 3:05p gskerl
* SW7346-853: Added support for MPEG-DASH to
*  B_PlaybackIp_UtilsAllocateMemory() and B_PlaybackIp_UtilsFreeMemory().
* 
* 93   7/16/12 5:20p ssood
* SW7425-3502: add new av codec support to playback_ip
* 
* 92   6/25/12 6:11p ssood
* SW7435-250: coverity fixes exposed by new coverity version
* 
* 91   6/15/12 6:26p ssood
* SW7231-858: more hls related improvements
* 
* 90   6/13/12 1:54p ssood
* SW7231-858: supressing debug logs for HLS playback sessions
* 
* 89   6/13/12 1:10p ssood
* SW7231-858: fix GetLine function to not account for last nl char
* 
* 88   5/23/12 3:13p ssood
* SW7425-2894: suppressing extra debug log from ip client modules
* 
* 87   5/21/12 2:01p ssood
* SW7425-3042: suppress debug logging by default
* 
* 86   4/25/12 9:45a ssood
* SW7231-766: add support for handling null char in HTTP response headers
* 
* 85   1/18/12 3:16p ssood
* SW7231-603: diable use of netaccel for HTTP protocol (due to lack of
*  any performance gains) + coverity fix
* 
* 84   1/11/12 2:50p ssood
* SW7425-2061: wrap simple decoders call for NEXUS_HAS_SIMPLE_DECODER
* 
* 83   12/27/11 10:15p ssood
* SW7425-2061: add simple decoder support to ip playback
* 
* 82   12/27/11 5:29p ssood
* SW7231-405: handle case where some URLs have additional CR char before
*  NL char
* 
* 81   12/14/11 6:32p ssood
* SW7231-462: move nexus related declarations inside ifdef for Cross-
*  Platform DMS builds
* 
* 80   11/11/11 6:31p mananp
* SW7231-462:Setting the heap while allocating memory from a client
*  during Multipr
* ocessing
* 
* 79   10/12/11 5:03p ssood
* SW7346-341: fix cross-platform build related warnings
* 
* 78   9/26/11 10:22a ssood
* SW7346-341: Add PVR Decryption support for file streaming
* 
* 77   9/19/11 1:50p ssood
* SW7425-1040: added logic to better handle network congestion
* 
* 76   8/5/11 2:43p ssood
* SW7425-1040: video decoder is not started before IP due to fast channel
*  change, so take out its status check
* 
* 75   8/4/11 4:45p ssood
* SW7425-1040: add fast channel change support to IP Sessions
* 
* 74   6/13/11 7:13p ssood
* SW7231-193: prefer IPv4 address over IPv6 address if available in DNS
*  resolution list
* 
* 73   6/6/11 1:28a ssood
* SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
*  eventCallback support for end of stream playback
* 
* 72   6/3/11 5:34p ssood
* SW7231-166: coverity fixes
* 
* 71   5/24/11 11:36a ssood
* SW7405-4392: Coverity fixes
* 
* 70   4/22/11 2:28p ssood
* SW7425-365: new tool chain required changing gethostbyname to
*  getaddrinfo
* 
* 69   3/24/11 8:37a ssood
* SW7420-1616: added dummy functions for non-DMA platforms
* 
* 68   3/17/11 4:41p ssood
* SW7420-1616: remove debug code
* 
* 67   3/17/11 4:36p ssood
* SW7420-1616: initial check-in for pvr encryption support
* 
* 66   2/23/11 1:41p zhangjq
* SW7420-1419:Clean up Android Media Player code
* 
* 65   2/21/11 9:40a zhangjq
* SW7420-1419:Clean up Android Media Player code
* 
* 64   2/21/11 9:36a zhangjq
* SW7420-1419:Clean up Android Media Player code
* 
* 62   1/27/11 10:06a ssood
* SW7422-227: add Nexus to Bmedia type convertion for SVC & MVC codecs
* 
* 61   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 60   11/29/10 1:02p mward
* SW7420-1217:  Eliminate "unused" warning when !B_HAS_DTCP_IP.
* 
* 59   11/24/10 2:58p ssood
* SW7420-1257: Video Conferencing Demo: add support for receiving input
*  camera input and stream it out
* 
* 58   11/23/10 11:55a ssood
* SW7420-1257: initial check-in for rtp/udp streaming support in
*  ip_streamer for Video Conferencing Demo
* 
* 57   11/11/10 10:34a ssood
* SW7420-1217: added support for playing unbounded (live) streams
* 
* 56   10/27/10 8:27p ssood
* SW7420-1217: initial check-in for HLS support
* 
* 55   9/28/10 9:25a junyel
* SW7420-1115: cross-platform code for DMS
* 
* 54   9/27/10 10:56a ssood
* SW7420-561: add logic to detect dead clients
* 
* 53   9/2/10 10:44a dliu
* SW7408-122: fix BDBG_WRN message with wrong number of arguments
* 
* 52   8/18/10 9:51a ssood
* SW7420-883: fixed a compiler warning for non-DTCP case
* 
* 51   8/14/10 4:50p ssood
* SW7420-883: retry DTCP call to check the AKE completion status
* 
* 50   8/5/10 2:47p jjordan
* SW7405-4205: add socket flush to facilitate accurate targetting of TTS
*  buffer fullness at start of decode
* 
* 49   7/23/10 11:10a ssood
* SW7420-502: RVU client: fixed subtle issues w/ playing DTCP encrypted
*  HTTP Chunk encoded content from HMC30 server
* 
* 48   6/16/10 5:05p ssood
* SW7420-716: initial RVU trickmode related check-ins
* 
* 47   4/16/10 6:10p ssood
* SW7420-561: Coverity fix
* 
* 46   4/2/10 5:56p ssood
* SW7468-198: Coverity fixes + RTSP related changes (requested by
*  Philips)
* 
* 45   3/16/10 3:09p ssood
* SW7420-559: carry forward the EROUTER related changes for WAN unicast
*  support
* 
* 44   3/15/10 4:40p ssood
* SW7125-233: Coverity fixes
* 
* 43   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/2   3/1/10 12:55p ssood
* SW7420-604: Enable optional support for streaming both file & live
*  content using HTTP Xfer Chunk Encoding (build flag
*  USE_HTTP_CHUNK_ENCODING)
* 
* SW7420-561/1   2/16/10 10:10a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 42   1/27/10 7:53p ssood
* SW7420-454: Enable support audio codecs used in 3GPP & Blue Rays
* 
* 41   1/23/10 10:17a ssood
* SW7420-454: further changes to condense the ip playback log
* 
* 40   1/20/10 4:13p ssood
* SW7420-454: more changes to compress the verbose output
* 
* 39   1/19/10 4:55p ssood
* SW7420-454: convert verbose output to be available via msg modules
*  flags
* 
* 38   12/23/09 4:24p erickson
* SW7420-516: convert non-error BDBG_ERR to BDBG_WRN
* 
* 37   12/23/09 12:36a ssood
* SW7420-454: increase the socket write queue size back to 600K to
*  possibly reduce the buffer underflows at the client
* 
* 36   12/22/09 11:03a ssood
* SW7420-454: add timeshift support for live streaming
* 
* 35   12/14/09 3:03p ssood
* SW7420-454: fix coverity reported issues
* 
* 34   12/14/09 1:04p ssood
* SW7420-502: set option to enable PCP per encrypted payload
* 
* 33   12/10/09 7:38p ssood
* SW7420-502: Send Live Streaming using HTTP Chunk Encoding (needs
*  USE_HTTP_CHUNK_ENCODING flag)
* 
* 32   12/8/09 8:50a ssood
* SW7420-454: added DTCP/IP related debug code
* 
* 31   11/24/09 10:50a ssood
* SW7420-454: added correct check for EAGAIN condition upon recvfrom()
* 
* 30   11/18/09 12:20p ssood
* SW7420-454: fixed warnings for non DTCP/IP builds
* 
* 29   11/17/09 3:48p ssood
* SW7420-454: Enhance IP library to stream files from local disk
* 
* 28   10/22/09 12:55p ssood
* SW7420-340: converted streaming socket to non-blocking to avoid getting
*  blocked when client silently goes away
* 
* 27   10/20/09 6:33p ssood
* SW7420-340: remove nexus playback, audio, video decoder dependencies
*  for SMS compilation
* 
* 26   10/1/09 11:00a jjordan
* SW7405-3107: add IP recv timeout status to Settop API
* 
* 25   9/22/09 7:06p ssood
* SW7405-2559: Adding WAV file playback support
* 
* 24   9/21/09 12:09p ssood
* SW7405-2559: Adding LPCM playback support
* 
* 23   9/8/09 9:58a ssood
* SW7420-340: dont flush playpump either if backend handles are not
*  provided (true for live streaming case)
* 
* 22   8/14/09 5:00p ssood
* PR50202: First check-in for clientside trickmode support for MPEG2 TS
*  format
* 
* 21   8/6/09 3:38p ssood
* PR48146: add HTTP redirect support
* 
* 20   7/16/09 12:59p ssood
* PR43746: Add Server based HTTP trick mode support
* 
* 19   6/23/09 11:49p ssood
* PR56278: Coverity fixes: unreachable code
* 
* 18   6/18/09 10:24a ssood
* PR55417: provide http return code code via the GetStatus to app
* 
* 17   5/26/09 11:49a ssood
* PR53773: added debug code to track a crash during quit
* 
* 16   5/22/09 1:56p lwhite
* PR55410: Add util to wait for playpump
* 
* 15   5/13/09 10:50a ssood
* PR53773: added comments for the HTTP utility function
* 
* 14   5/13/09 10:39a ssood
* PR53773: created utitlity functions for HTTP parsing so that RAD/EA
*  module can use it as well
* 
* 13   4/8/09 4:20p ssood
* PR50311: socket tune up related cleanup
* 
* 12   4/7/09 10:01a ssood
* PR53773 : fixing compilation errors
* 
* 11   4/6/09 6:09p ssood
* PR53773 : refactored basic TCP socket functions for reuse by other
*  internal modules
* 
* 10   3/13/09 6:02p ssood
* PR50311: added check for socket timeout (EAGAIN)
* 
* 9   2/27/09 11:55a ssood
* PR50311: Adding support for HTTP Chunk Transfer Encoding
* 
* 8   12/4/08 10:13a jjordan
* PR49937: Add support for RTP over TTS
* 
* 7   10/31/08 12:30p jjordan
* PR47230: Add TTS Pacing
* 
* 6   10/27/08 11:50a ssood
* PR45431: adding MP4 & ASF playback support over HTTP
* 
* 5   10/14/08 2:43p lwhite
* PR47685: Coverity fixes
* 
* 4   10/13/08 5:42p ssood
* PR47521: improved support for low-bit rate streams + handle unmarked
*  discontinuity condition
* 
* 3   8/18/08 11:05a ssood
* PR45431: Fixed RTP to work standard sockets
* 
* 2   8/5/08 1:12p ssood
* PR42739: splitting up UDP & RTP Processing code into separate sub-
*  modules
* 
* 1   6/11/08 6:21p ssood
* PR42739: adding IP Applib files
* 
***************************************************************************/
#ifndef _WIN32_WCE 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#else /* _WIN32_WCE */
#include <windows.h>
    typedef HANDLE pthread_t;
    #define close(x) closesocket(x)
    unsigned long errno = 0;
    #define EINTR -1
#endif /* _WIN32_WCE */

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"
#include "b_os_lib.h"
#include "bdbg.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bpool.h"
#include "barena.h"
#include "bioatom.h"
#include "blst_queue.h"
#include "brtp_spf.h"

BDBG_MODULE(b_playback_ip_utils);

#ifdef B_HAS_NETACCEL
#if defined(LINUX)
#include <sys/syscall.h>
#include <asm/cachectl.h>
#endif

static int _cacheflush(char * addr, int size, int type)
{
    return syscall(SYS_cacheflush, addr, size, type); 
}
#endif

void B_PlaybackIp_UtilsTuneNetworkStack(
    int fd
    )
{
#ifdef LINUX
    int     new_rmem_max = NEW_RMEM_MAX;
    /* 
     * Linux kernel tuning: set socket receive buffer to 100k by default. 
     * It works with low bit rate stream up to ~14Mbps. With higher bit rate (19 or above)Mbps,
     * Linux starts dropping UDP packets. We need to increase our UDP socket receive buffer size 
     * by using setsockopt(). Therefore, /proc/sys/net/core/rmem_max needs to be changed.
     */
    FILE    *f;
    char    buf[80];
    int     rmax;
    int size = new_rmem_max;
    socklen_t len;
    size_t tmpLen;

    f = fopen("/proc/sys/net/core/rmem_max", "rw");
    if (f) {
        tmpLen = fread(buf, 1, sizeof(buf)-1, f);
        buf[tmpLen] = '\0';
        rmax = strtol(buf, (char **)NULL, 10);
        fclose(f);
        BDBG_MSG(("*** rmem_max %d\n", rmax));
        if (rmax < NEW_RMEM_MAX) {
            /* it is the default value, make it bigger */
            BDBG_MSG(("%s: Increasing default rmem_max from %d to rmem_max %d\n", __FUNCTION__, rmax, NEW_RMEM_MAX));
            tmpLen = snprintf(buf, sizeof(buf), "%d", NEW_RMEM_MAX);
            f = fopen("/proc/sys/net/core/rmem_max", "w");
            if (f) {
                fwrite(buf, 1, tmpLen, f);
                fclose(f);
                new_rmem_max = NEW_RMEM_MAX;
            } else
                new_rmem_max = rmax;
        } else
            new_rmem_max = rmax;
    }

    /* now increase socket receive buffer size */

    len = sizeof(size);
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, &len) == 0)
        BDBG_MSG(("%s: current socket receive buffer size = %d KB\n", __FUNCTION__, size/1024));
    if (size < new_rmem_max) {
        size = new_rmem_max;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, len) != 0)
            BDBG_ERR(("%s: ERROR: can't set UDP receive buffer to %d, errno=%d\n", __FUNCTION__, size, errno));
        len = sizeof(size);
        if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, &len) == 0)
            BDBG_MSG(("%s: new socket receive buffer size = %d KB\n", __FUNCTION__, size/1024));
    }
#else
    BSTD_UNUSED(fd);
#endif
}

#define SOCKET_WRITE_QUEUE_SIZE (300*1024)
void 
B_PlaybackIp_UtilsTuneNetworkStackTx(
    int sd
    )
{
    FILE *f;
    char buf[80];
    int wmax;
    int size;
    socklen_t len;
    size_t tmpLen;

    /* 
     * Linux kernel TX tuning: set socket send buffer to 300K by default. 
     * For higher bitrate HD streams, streamer can't send data at the natural 
     * stream bitrate and thus causes client underflows.
     * We need to increase our socket send buffer size by using setsockopt(). 
     * Therefore, /proc/sys/net/core/wmem_max needs to be changed.
     */
    f = fopen("/proc/sys/net/core/wmem_max", "rw");
    if (f) {
        tmpLen = fread(buf, 1, sizeof(buf)-1, f);
        buf[tmpLen] = '\0';
        wmax = strtol(buf, (char **)NULL, 10);
        fclose(f);
        BDBG_MSG(("current wmem_max %d\n", wmax));
        if (wmax < 2*SOCKET_WRITE_QUEUE_SIZE) {
            /* it is the default value, make it bigger */
            BDBG_MSG(("%s: Increasing default wmem_max from %d to %d\n", __FUNCTION__, wmax, 2*SOCKET_WRITE_QUEUE_SIZE));
            tmpLen = snprintf(buf, sizeof(buf)-1, "%d", (2*SOCKET_WRITE_QUEUE_SIZE));
            f = fopen("/proc/sys/net/core/wmem_max", "w");
            if (f) {
                fwrite(buf, 1, tmpLen, f);
                fclose(f);
            }
        }
    }
    else {
        BDBG_MSG(("%s: Failed to open wmem_max proc variable\n", __FUNCTION__));
    }

    len = sizeof(size);
    if (getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, &len) == 0)
        BDBG_MSG(("%s: current socket send buffer size = %d KB\n", __FUNCTION__, size/1024));
    if (size < SOCKET_WRITE_QUEUE_SIZE) {
        size = SOCKET_WRITE_QUEUE_SIZE;
        if (setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, len) != 0)
            BDBG_ERR(("%s: ERROR: can't set send buffer size to %d, errno=%d\n", __FUNCTION__, size, errno));
        len = sizeof(size);
        size = 0;
        if (getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, &len) == 0)
            BDBG_MSG(("%s: updated socket send buffer size = %d KB\n", __FUNCTION__, size/1024));
    }

    f = fopen("/proc/sys/net/ipv4/tcp_retries2", "rw");
    if (f) {
        int tcp_retries2;
        tmpLen = fread(buf, 1, sizeof(buf)-1, f);
        buf[tmpLen] = '\0';
        tcp_retries2 = strtol(buf, (char **)NULL, 10);
        fclose(f);
        BDBG_MSG(("current tcp_retries2 %d\n", tcp_retries2));
        if (tcp_retries2 > 6) {
            /* it is the default value, make it bigger */
            BDBG_MSG(("%s: Reducing default tcp_retries2 from %d to %d\n", __FUNCTION__, tcp_retries2, 6));
            tmpLen = snprintf(buf, sizeof(buf)-1, "%d", 6);
            f = fopen("/proc/sys/net/ipv4/tcp_retries2", "w");
            if (f) {
                fwrite(buf, 1, tmpLen, f);
                fclose(f);
            }
        }
    }
    else {
        BDBG_MSG(("%s: Failed to open tcp_retries2 proc variable\n", __FUNCTION__));
    }
}

/**
  * simple function to wait for socket ready event from the socket (indicates when more data can be sent on the socket)
  * returns -1 for errors, 0 for timeout, 1 for success
 **/
int 
B_PlaybackIp_UtilsWaitForSocketWriteReady(
    int fd,
    int timeout /* in usec */
    )
{
    int rc;
    fd_set wfds;
    struct timeval tv;

    for(;;) {
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);
        tv.tv_sec = 0; 
        tv.tv_usec = timeout;

        if ( (rc = select(fd +1, NULL, &wfds, NULL, &tv)) < 0 && rc != EINTR) {
            BDBG_ERR(("%s: ERROR: select(): errno = %d", __FUNCTION__, errno));
            return -1;
        }
        else if (rc == EINTR) {
            BDBG_ERR(("%s: select returned EINTR, continue the loop", __FUNCTION__));
            continue;
        }
        else if (rc == 0) {
            /* select timeout */
            BDBG_MSG(("%s: ERROR: select timed out (%d user val %d usec) in waiting for socket to be ready to accept more data (fd %d)\n", 
                            __FUNCTION__, tv.tv_usec, timeout, fd));
            return 0;
        }

        if (!FD_ISSET(fd, &wfds)) {
            /* some select event but our FD not set: No more data - wait */
            BDBG_MSG(("%s: some select event but our FD (%d) not set: No more data - retry select", __FUNCTION__, fd));
            continue;
        }
        break;
    }
    return 1;
}

/**
  * simple function to wait for data ready event from the socket 
 **/
int 
B_PlaybackIp_UtilsWaitForSocketData(
    B_PlaybackIpHandle playback_ip,
    bool *recvTimeout
    )
{
    int rc;
    int fd;
    fd_set rfds;
    struct timeval tv;
    int dbgCnt = 0;
    int result = -1;
    static int logError = 0;

    fd = playback_ip->socketState.fd; 
    *recvTimeout = false;
    for(;;) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping) { 
            goto done; 
        }

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        if (playback_ip->disableLiveMode) {
            /* increase the socket timeout as scaled data may take longer to arrive */
            tv.tv_sec = 30;
            tv.tv_usec = 0;
            BDBG_MSG(("Live mode is disabled, so using the default large timeout of %d sec\n", tv.tv_sec));
        }
        else {
            tv.tv_sec = 0; 
            tv.tv_usec = (playback_ip->settings.maxNetworkJitter + 100) * 1000; /* converted to micro sec */
        }

        if ( (rc = select(fd +1, &rfds, NULL, NULL, &tv)) < 0 && rc != EINTR) {
            BDBG_ERR(("%s: ERROR: select(): errno = %d, fd %d", __FUNCTION__, errno, fd));
            goto done;
        }
        else if (rc == EINTR) {
            BDBG_ERR(("select returned EINTR, continue the loop"));
            continue;
        }
        else if (rc == 0) {
            /* select timeout: we didn't receive any packets even after max jitter interval */
            /* if in non-paused state, indicate to caller this timeout event. For LIVE IP Channels, this event is */
            /* considered as unmarked discontinuity */
            if (playback_ip->playback_state == B_PlaybackIpState_ePaused) {
                BDBG_MSG(("%s: timed out during pause state, continue waiting on select (fd %d, timeout %d)\n", __FUNCTION__, fd, tv.tv_sec));
                continue;
            }

            if (dbgCnt++ % 1 == 0)
                BDBG_MSG(("%s: timed out in waiting for data from server (fd %d, timeout %d): coming out of select loop", 
                            __FUNCTION__, fd, playback_ip->settings.maxNetworkJitter));
            *recvTimeout = true;
            playback_ip->numRecvTimeouts++;
            logError = 1;
            result = 0;
            break;
        }
        else {
            *recvTimeout = false;
        }

        if (!FD_ISSET(fd, &rfds)) {
            /* some select event but our FD not set: No more data - wait */
            continue;
        }

        /* there is some data in the socket */
        result = 0;
        if (logError) {
            BDBG_MSG(("%s: Network Data is now coming again", __FUNCTION__));
            logError = 0;
        }
        break;
    }
done: 
    return result;
}
#ifndef DMS_CROSS_PLATFORMS    
int B_PlaybackIp_UtilsGetPlaypumpBuffer(
    B_PlaybackIpHandle playback_ip, 
    unsigned int size
    ) 
{
    NEXUS_Error rc;
    for(;;) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode || playback_ip->playback_state == B_PlaybackIpState_eEnteringTrickMode) {
            goto error;
        }

        if (NEXUS_Playpump_GetBuffer(playback_ip->nexusHandles.playpump, &playback_ip->buffer, &playback_ip->buffer_size)) {
            BDBG_ERR(("Returned error from NEXUS_Playpump_GetBuffer()!"));
            goto error;
        }

        if (playback_ip->buffer_size == 0) {
            NEXUS_PlaypumpStatus ppStatus;
            /* bplaypump_flush(playback_ip->nexusHandles.playpump); */
            rc = NEXUS_Playpump_GetStatus(playback_ip->nexusHandles.playpump, &ppStatus);
            if (!rc) BDBG_MSG(("Returned 0 buffer size from GetBuffer()!, fifo dep %d, size %d, desc sz %d dep %d", ppStatus.fifoDepth, ppStatus.fifoSize, ppStatus.descFifoDepth, ppStatus.descFifoSize));
            BKNI_Sleep(200);
            continue;
        }
        else if(playback_ip->buffer_size >= size) {
            /* constrain the amount of data we're going to read from the socket */
            playback_ip->buffer_size = size;
            BDBG_MSG(("%s: got buffer of size %d from the playpump\n", __FUNCTION__, size));
            break;
        }
        BDBG_MSG_FLOW(("skip buffer %d", playback_ip->buffer_size));
        /* release buffer unused, it's too small */
        if(NEXUS_Playpump_ReadComplete(playback_ip->nexusHandles.playpump, playback_ip->buffer_size, 0)) {
            BDBG_ERR(("Returned error from NEXUS_Playpump_ReadComplete()!"));
            BKNI_Sleep(1);
            continue;
        }
    }
    return (playback_ip->buffer_size);
error:
    return -1;
}

int 
B_PlaybackIp_UtilsFlushAvPipeline(
    B_PlaybackIpHandle playback_ip
    )
{
    NEXUS_Error nrc;
    /* flush the pipeline only if backends are associated w/ this live IP session */
    if (!playback_ip->nexusHandles.videoDecoder && !playback_ip->nexusHandles.primaryAudioDecoder && !playback_ip->nexusHandles.simpleVideoDecoder && !playback_ip->nexusHandles.simpleAudioDecoder) 
        /* no backend handles are associated, we may be just forwarding the live IP content, so no need to flush playpump either */
        goto out;

    BDBG_MSG(("%s: flushing AV Pipeline (Playpump & AV Decoders)", __FUNCTION__));
    nrc = NEXUS_Playpump_Flush(playback_ip->nexusHandles.playpump);
    if (nrc!= NEXUS_SUCCESS) {
        BDBG_ERR(("%s: Failed to flush the Nexus Playpump\n", __FUNCTION__));
    }
#ifndef B_HAS_SMS_GATEWAY
    if (playback_ip->nexusHandles.stcChannel) {
        if (NEXUS_StcChannel_Invalidate(playback_ip->nexusHandles.stcChannel) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to invalidate STC channel\n", __FUNCTION__));
            return -1;
        }
    }
    if (playback_ip->nexusHandles.videoDecoder) {
       NEXUS_VideoDecoder_Flush(playback_ip->nexusHandles.videoDecoder);
    }
#ifdef NEXUS_HAS_SIMPLE_DECODER
    else if (playback_ip->nexusHandles.simpleVideoDecoder) {
        NEXUS_SimpleVideoDecoder_Flush(playback_ip->nexusHandles.simpleVideoDecoder);
    }
#endif
    if (playback_ip->nexusHandles.primaryAudioDecoder) {
        NEXUS_AudioDecoder_Flush(playback_ip->nexusHandles.primaryAudioDecoder);
    }
    if (playback_ip->nexusHandles.secondaryAudioDecoder) {
        NEXUS_AudioDecoder_Flush(playback_ip->nexusHandles.secondaryAudioDecoder);
    }
#ifdef NEXUS_HAS_SIMPLE_DECODER
    if (playback_ip->nexusHandles.simpleAudioDecoder) {
        NEXUS_SimpleAudioDecoder_Flush(playback_ip->nexusHandles.simpleAudioDecoder);
    }
#endif
#endif

out:
    /* get an adequately sized buffer from the playpump, only for non-HTTP protocols */
    if (playback_ip->protocol != B_PlaybackIpProtocol_eHttp)  {
        if (B_PlaybackIp_UtilsGetPlaypumpBuffer(playback_ip, MAX_BUFFER_SIZE) < 0)
        return -1;
    }
    return 0;
}
#endif /* DMS_CROSS_PLATFORMS */    

int 
B_PlaybackIp_UtilsFlushSocket(
    B_PlaybackIpHandle playback_ip
    )
{
#if defined(B_HAS_NETACCEL)
    int cummulativeTotal = 0;
    int i;
    int totalBytesRecv;
    bool recvTimeout = false;
    B_PlaybackIpError rc;

    BDBG_MSG(("Flushing (netaccel) socket..."));

    for(i=0; i<4; i++) {
        rc = B_PlaybackIp_UtilsReadUdpPayloadChunk(
            playback_ip, 
            playback_ip->discard_buf,
            200*IP_MAX_PKT_SIZE,
            1,
            playback_ip->bytesRecvPerPkt,
            &totalBytesRecv,
            &recvTimeout
            );
        if (rc != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: Network Read Error", __FUNCTION__));
            return -1;
        }

        BDBG_MSG(("totalBytesRecv: %ld", totalBytesRecv));
        cummulativeTotal += totalBytesRecv;
        if(cummulativeTotal > 1024*1024) { break; }
    }

    BDBG_MSG(("Socket flushed."));
#else
    BSTD_UNUSED(playback_ip);

    BDBG_MSG(("Not flushing (regular) socket"));
#endif

    return 0;
}

/**
  * simple function to recv a chunk of data from a datagram socket 
  * it may make several recv calls to read the data chunk.
 **/
B_PlaybackIpError 
B_PlaybackIp_UtilsReadUdpPayloadChunk(
    B_PlaybackIpHandle playback_ip,
    void *buf,       /* buffer big enough to hold (bufSize * iterations) bytes */
    int bufSize,     /* how much to read per iteration */
    int iterations, /* number of iterations (i.e. recv calls) */
    int *bytesRecv,    /* pointer to list of bytesRecv for each recvfrom call */
    int *totalBytesRecv, /* total bytes received */
    bool *recvTimeout   /* set if timeout occurs while waiting to receive a IP packet (helps detect unmarked discontinuity */
    )
{
    int i;
    int fd;
    int bytesRead = 0;
    char *bufp;

    fd = playback_ip->socketState.fd;
    bufp = (char *)buf;
    *totalBytesRecv = 0;

    for (i=0; i<iterations; i++)
    {
        /* keep waiting until there is enough data read for consumption in the socket */
        if (B_PlaybackIp_UtilsWaitForSocketData(playback_ip, recvTimeout) != 0)
            goto error;

        if (*recvTimeout == true) {
            /* reset the timeout flag if we have received any data, this allows caller to process this data */
            /* this can only happen for normal sockets, where we have to make multiple recvfrom calls to receive a chunk */
            /* next read will hit the timeout flag if there is a true n/w loss event */
            if (*totalBytesRecv != 0)
                *recvTimeout = false;
            goto out;
        }

        /* read from the socket & check errors */
        bytesRead = recvfrom(fd, bufp, bufSize, 0, NULL, NULL);
#if defined(__vxworks)
        if (bytesRead < 0 && bytesRead != EWOULDBLOCK) 
        {
            BDBG_ERR(("%s: recvfrom ERROR: errno = %d", __FUNCTION__, errno));
            goto error;
        }
#else   /* Linux */
        if (bytesRead  < 0  && errno != EINTR && errno != EAGAIN) 
        {
            BDBG_ERR(("%s: recvfrom ERROR: errno = %d", __FUNCTION__, errno));
            goto error;
        }
#endif
        else if (bytesRead == 0) {
            BDBG_ERR(("%s: No more data from Server, we are done!", __FUNCTION__));
            goto error;
        }
        else if (errno == EINTR || errno == EAGAIN) {
            BDBG_ERR(("%s: Recvfrom System Call interrupted or timed out (errno = %d), retry it\n", __FUNCTION__, errno));
            continue;
        }
        *bytesRecv = bytesRead;
        bufp += bytesRead;
        *totalBytesRecv += bytesRead;
        bytesRecv++;
    }

out:
    return B_ERROR_SUCCESS;
error:
    return B_ERROR_SOCKET_ERROR;
}

#define CONVERT(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].settop == settop_value) { \
            return g_struct[i].nexus; \
        } \
    } \
    BDBG_ERR(("unable to find Settop API value %d in %s", settop_value, #g_struct)); \
    return g_struct[0].nexus

#define CONVERT_NEXUS(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].nexus == nexus_value) { \
            return g_struct[i].settop; \
        } \
    } \
    BDBG_ERR(("unable to find Nexus value %d in %s", nexus_value, #g_struct)); \
    return g_struct[0].settop

static struct {
    NEXUS_VideoCodec nexus;
    bvideo_codec settop;
} g_videoCodec[] = {
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_none},
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_unknown},
    {NEXUS_VideoCodec_eMpeg1, bvideo_codec_mpeg1},
    {NEXUS_VideoCodec_eMpeg2, bvideo_codec_mpeg2},
    {NEXUS_VideoCodec_eMpeg4Part2, bvideo_codec_mpeg4_part2},
    {NEXUS_VideoCodec_eH263, bvideo_codec_h263},
    {NEXUS_VideoCodec_eH264, bvideo_codec_h264},
    {NEXUS_VideoCodec_eVc1, bvideo_codec_vc1},
    {NEXUS_VideoCodec_eVc1SimpleMain, bvideo_codec_vc1_sm},
    {NEXUS_VideoCodec_eDivx311, bvideo_codec_divx_311},
    {NEXUS_VideoCodec_eH264_Svc, bvideo_codec_h264_svc},
    {NEXUS_VideoCodec_eH264_Mvc, bvideo_codec_h264_mvc},
    {NEXUS_VideoCodec_eAvs, bvideo_codec_avs},
    {NEXUS_VideoCodec_eSpark, bvideo_codec_spark},
    {NEXUS_VideoCodec_eVp6, bvideo_codec_vp6},
    {NEXUS_VideoCodec_eRv40, bvideo_codec_rv40},
    {NEXUS_VideoCodec_eVp8, bvideo_codec_vp8}
};

static struct {
    NEXUS_AudioCodec nexus;
    baudio_format settop;
} g_audioCodec[] = {
   {NEXUS_AudioCodec_eUnknown, baudio_format_unknown},
   {NEXUS_AudioCodec_eMpeg, baudio_format_mpeg},
   {NEXUS_AudioCodec_eMp3, baudio_format_mp3},
   {NEXUS_AudioCodec_eAac, baudio_format_aac},
   {NEXUS_AudioCodec_eAacPlus, baudio_format_aac_plus},
   {NEXUS_AudioCodec_eAacPlusAdts, baudio_format_aac_plus_adts},
   {NEXUS_AudioCodec_eAacPlusLoas, baudio_format_aac_plus_loas},
   {NEXUS_AudioCodec_eAc3, baudio_format_ac3},
   {NEXUS_AudioCodec_eAc3Plus, baudio_format_ac3_plus},
   {NEXUS_AudioCodec_eDts, baudio_format_dts},
   {NEXUS_AudioCodec_eLpcmHdDvd, baudio_format_lpcm_hddvd},
   {NEXUS_AudioCodec_eLpcmBluRay, baudio_format_lpcm_bluray},
   {NEXUS_AudioCodec_eDtsHd, baudio_format_dts_hd},
   {NEXUS_AudioCodec_eWmaStd, baudio_format_wma_std},
   {NEXUS_AudioCodec_eWmaPro, baudio_format_wma_pro},
   {NEXUS_AudioCodec_eLpcmDvd, baudio_format_lpcm_dvd},
   {NEXUS_AudioCodec_eAvs, baudio_format_avs},
   {NEXUS_AudioCodec_ePcmWav, baudio_format_pcm},
   {NEXUS_AudioCodec_eAmr, baudio_format_amr},
   {NEXUS_AudioCodec_eDra, baudio_format_dra},
   {NEXUS_AudioCodec_eCook, baudio_format_cook},
   {NEXUS_AudioCodec_eVorbis, baudio_format_vorbis}
};

static struct {
    NEXUS_TransportType nexus;
    bstream_mpeg_type settop;
} g_mpegType[] = {
    {NEXUS_TransportType_eTs, bstream_mpeg_type_unknown},
    {NEXUS_TransportType_eEs, bstream_mpeg_type_es},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_bes},
    {NEXUS_TransportType_eMpeg2Pes, bstream_mpeg_type_pes},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_ts},
    {NEXUS_TransportType_eDssEs, bstream_mpeg_type_dss_es},
    {NEXUS_TransportType_eDssPes, bstream_mpeg_type_dss_pes},
    {NEXUS_TransportType_eVob, bstream_mpeg_type_vob},
    {NEXUS_TransportType_eAsf, bstream_mpeg_type_asf},
    {NEXUS_TransportType_eAvi, bstream_mpeg_type_avi},
    {NEXUS_TransportType_eMpeg1Ps, bstream_mpeg_type_mpeg1},
    {NEXUS_TransportType_eMp4, bstream_mpeg_type_mp4},
    {NEXUS_TransportType_eMkv, bstream_mpeg_type_mkv},
    {NEXUS_TransportType_eWav, bstream_mpeg_type_wav},
    {NEXUS_TransportType_eFlv, bstream_mpeg_type_flv},
    {NEXUS_TransportType_eRmff, bstream_mpeg_type_rmff},
    {NEXUS_TransportType_eOgg, bstream_mpeg_type_ogg}
}; 

NEXUS_VideoCodec B_PlaybackIp_UtilsVideoCodec2Nexus(bvideo_codec settop_value)
{
    CONVERT(g_videoCodec);
} 

NEXUS_AudioCodec B_PlaybackIp_UtilsAudioCodec2Nexus(baudio_format settop_value)
{   
    CONVERT(g_audioCodec);
}

NEXUS_TransportType B_PlaybackIp_UtilsMpegtype2ToNexus(bstream_mpeg_type settop_value)
{  
    CONVERT(g_mpegType);
} 

bstream_mpeg_type B_PlaybackIp_UtilsNexus2Mpegtype(NEXUS_TransportType nexus_value)
{  
    CONVERT_NEXUS(g_mpegType);
}
    
int 
_http_socket_select_write_events(volatile B_PlaybackIpState *playbackIpState, int fd, int timeout) 
{
    int rc = -1;
    fd_set wfds;
    struct timeval tv;
    int timeoutInterval = 250; /* in msec */
    int timeoutIterations = (timeout * 1000) / timeoutInterval;
    int numTimeoutInterations = 0;

    BDBG_MSG(("%s: fd %d", __FUNCTION__, fd));
    while (numTimeoutInterations++ < timeoutIterations) {
        if (playbackIpState != NULL && ((*playbackIpState == B_PlaybackIpState_eStopping) || (*playbackIpState == B_PlaybackIpState_eStopped) || (*playbackIpState == B_PlaybackIpState_eWaitingToEnterTrickMode))) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of select loop due to state (%d) change, # of timeout iterations %d, total timeout %d seconds\n", __FUNCTION__, *playbackIpState, numTimeoutInterations, (numTimeoutInterations*timeoutInterval)/1000));
            return -1;
        }
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);
        tv.tv_sec = 0; 
        tv.tv_usec = timeoutInterval * 1000; /* in micro-secs */

        rc = select(fd +1, NULL, &wfds, NULL, &tv);
        if (rc < 0) {
            if (errno == EINTR) {
                BDBG_MSG(("%s: select System Call interrupted, retrying\n", __FUNCTION__));
                continue;
            }
            BDBG_ERR(("%s: ERROR: select(): rc %d, errno %d, fd %d", __FUNCTION__, rc, errno, fd));
            return -1;
        }

        if (rc == 0 || !FD_ISSET(fd, &wfds)) {
            /* select timeout or some select event but our FD not set: No more data - wait */
            BDBG_MSG(("%s: selectTimeout is true, retry select on this socket", __FUNCTION__));
            continue;
        }
        /* ready event on socket, return succcess */
        BDBG_MSG(("%s: select write ready event (before the timeout of %d sec), iterations: cur %d, max %d, errno %d", __FUNCTION__, timeout, numTimeoutInterations, timeoutIterations, errno));
        return 0;
    }
    BDBG_WRN(("%s: select timed out after %d sec for write ready event, iterations: cur %d, max %d\n", __FUNCTION__, timeout, numTimeoutInterations, timeoutIterations));
    return -1;
}

#define MAXADDRNAME 256
/* 
 * This function creates a TCP connection to server, makes socket non-blocking 
 * and return the socket descriptor back to caller.
 */
B_PlaybackIpError 
B_PlaybackIp_UtilsTcpSocketConnect(
    volatile B_PlaybackIpState *playbackIpState,
    char *server, 
    int port, 
    bool nonBlockingSocket,
    int *psd
    )
{
    int sd = -1;
    int rc;
    struct sockaddr *localAddr;
    socklen_t localAddrLen;
    struct sockaddr_in localAddrIpv4;
    struct sockaddr_in6 localAddrIpv6;
	struct addrinfo hints;
	struct addrinfo *addrInfo = NULL;
	struct addrinfo *addrInfoNode = NULL;
    char portString[16];
    B_PlaybackIpError status = B_ERROR_SOCKET_ERROR;
	char hostbuf[MAXADDRNAME+1];
	char serverbuf[NI_MAXSERV];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* we dont know the family */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    memset(portString, 0, sizeof(portString));  /* getaddrinfo() requires port # in the string form */
    snprintf(portString, sizeof(portString)-1, "%d", port);
    if (getaddrinfo(server, portString, &hints, &addrInfo) != 0) {
        BDBG_ERR(("%s: ERROR: getaddrinfo failed for server:port: %s:%d, errno %d", __FUNCTION__, server, port, errno));
        perror("getaddrinfo");
        return B_ERROR_INVALID_PARAMETER;
    }
    BDBG_MSG(("%s: socket family %d, type %d, protocol %d, ai_next %p", 
                __FUNCTION__, addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol, addrInfo->ai_next));
    getnameinfo(addrInfo->ai_addr, addrInfo->ai_addrlen, hostbuf, sizeof(hostbuf), serverbuf, sizeof(serverbuf), NI_NUMERICHOST | NI_NUMERICSERV);
    BDBG_MSG(("IP Addr = %s, Port = %s\n", hostbuf, serverbuf));
    for (addrInfoNode = addrInfo; addrInfoNode != NULL; addrInfoNode = addrInfo->ai_next) {
        if (addrInfoNode->ai_family == AF_INET)
            break;
    }
    if (!addrInfoNode) {
        BDBG_ERR(("%s: ERROR: no IPv4 address available for this server, no support for IPv6 yet"));
        goto error;
    }
    addrInfo = addrInfoNode;

#ifdef B_HAS_NETACCEL1
    /* for now, not using accelerated sockets for HTTP as there aren't any performance gains using it */
    sd = socket(addrInfo->ai_family, SOCK_BRCM_STREAM, addrInfo->ai_protocol);
#else
    sd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
#endif
    if (sd < 0) {
        BDBG_ERR(("%s: socket() error, errno %d", __FUNCTION__, errno));
        perror("socket");
        status = B_ERROR_SOCKET_ERROR;
        goto error;
    }

    if (addrInfo->ai_family == AF_INET6) {
        memset((char *)&localAddrIpv6, 0, sizeof(localAddrIpv6));
        localAddrIpv6.sin6_family = addrInfo->ai_family;
        localAddrIpv6.sin6_addr = in6addr_any;
        localAddrIpv6.sin6_port = htons(0);
        localAddr = (struct sockaddr *)&localAddrIpv6;
        localAddrLen = sizeof(localAddrIpv6);
        if (((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id == 0) {
            BDBG_MSG(("IPv6 scope id is not set, setting it to 1"));
            localAddrIpv6.sin6_scope_id = 1;
        }
        else {
            BDBG_MSG(("IPv6 scope id is set to %d", ((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id));
            localAddrIpv6.sin6_scope_id = ((struct sockaddr_in6 *)addrInfo->ai_addr)->sin6_scope_id;
        }
    }
    else {
        localAddrIpv4.sin_family = addrInfo->ai_family;
        localAddrIpv4.sin_addr.s_addr = htonl(INADDR_ANY);
        localAddrIpv4.sin_port = htons(0);
        localAddr = (struct sockaddr *)&localAddrIpv4;
        localAddrLen = sizeof(localAddrIpv4);
        BDBG_MSG(("ipv4 path"));
    }

    if (bind(sd, localAddr, localAddrLen)) {
        BDBG_ERR(("%s: bind() error, errno %d", __FUNCTION__, errno));
        perror("bind");
        status = B_ERROR_SOCKET_ERROR;
        goto error;
    }

    if (nonBlockingSocket) {
        int nonblock = 1;
        if (ioctl(sd, FIONBIO, (int) &nonblock) == -1) {
            BDBG_WRN(("%s: can't set O_NONBLOCK mode, errno %d", __FUNCTION__, errno));
            goto error;
        }
    }

    /* Connect to server */
    if ((rc = connect(sd, addrInfo->ai_addr, addrInfo->ai_addrlen)) != 0) {
        if (errno == EINPROGRESS) {
            /* non-blocking connect hasn't succeeded yet, wait for sometime until this operation completes */
            if (_http_socket_select_write_events(playbackIpState, sd, HTTP_SELECT_TIMEOUT) == 0) {
                errno =0;
                if ((rc = connect(sd, addrInfo->ai_addr, addrInfo->ai_addrlen)) == 0) {
                    BDBG_MSG(("%s: connect success, errno %d", __FUNCTION__, errno));
                    goto successCase;
                }
            }
            /* socket connect either timed out or met a failure, continue below */
        }
        BDBG_ERR(("%s: TCP connect Error to %s:%d failed, errno %d", __FUNCTION__, server, port, errno));
        perror("connect");
        status = B_ERROR_SOCKET_ERROR;
        goto error;
    }

successCase:
    BDBG_MSG(("%s: TCP: Connection to %s:%d Success (fd %d)\n", __FUNCTION__, server, port, sd));
    *psd = sd;

    status = B_ERROR_SUCCESS;
    if (addrInfo)
        freeaddrinfo(addrInfo);
    return status;

error:
    if (sd >= 0)
        close(sd);
    if (addrInfo)
        freeaddrinfo(addrInfo);
    return status;
}

/* 
 * This function writes the given data to the socket and returns any errors or the bytes wrote.
 * It returns: 
 *  =-1: for errors other than EINTR & EAGAIN during write call or when channel change occurs
 *  = 0: for EOF where server closed the TCP connection
 *  > 0: for success indicating number of bytes written to the socket
 */
int 
B_PlaybackIp_UtilsTcpSocketWrite(
    volatile B_PlaybackIpState *playbackIpState, 
    int sd, 
    char *wbuf, 
    int wbuf_len)
{
    int rc;
#if 0
    B_Time end_time;
    int write_time = 0;
    static int avg_write_time = 0;
    static int min_write_time = 9999999;
    static int max_write_time = 0;
#endif

    if (!wbuf || wbuf_len <= 0) {
        BDBG_ERR(("%s: invalid parameters to write, wbuf %p, wbuf_len %d\n", __FUNCTION__, wbuf, wbuf_len));
        return -1;
    }

#ifdef B_HAS_NETACCEL
    _cacheflush(wbuf, strlen(wbuf), DCACHE);
    /* b_cacheflush(wbuf, strlen(wbuf)); */
#endif

    while (true) {
        if (playbackIpState && ((*playbackIpState == B_PlaybackIpState_eStopping) || (*playbackIpState == B_PlaybackIpState_eStopped) || (*playbackIpState == B_PlaybackIpState_eWaitingToEnterTrickMode))) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of write loop due to state (%d) change", __FUNCTION__, *playbackIpState));
            return -1;
        }
        rc = write(sd, wbuf, wbuf_len);
        if (rc <= 0) {
            if (errno == EINTR || errno == EAGAIN) {
                BDBG_ERR(("%s: write System Call interrupted or timed out, retrying (rc %d, errno %d)\n", __FUNCTION__, rc, errno));
                continue;
            }
            BDBG_ERR(("%s: write ERROR: rc %d, errno %d", __FUNCTION__, rc, errno));
            return -1;
        }
        if (rc != wbuf_len) {
            BDBG_ERR(("%s: Failed to write the complete (%d) get request, wrote only %d bytes, retrying...\n", __FUNCTION__, wbuf_len, rc));
            wbuf += rc;
            wbuf_len -= rc;
            continue;
        }
        BDBG_MSG(("%s: wrote %d out of %d bytes to network\n", __FUNCTION__, rc, wbuf_len));
        break;
    }

#if 0
    /* this is debug code */
    BDBG_WRN(("Get Request: \n%s", wbuf));
    B_Time_Get(&end_time);
    write_time = B_Time_Diff(&end_time, &playback_ip->start_time);
    if (avg_write_time != 0)
        avg_write_time = (avg_write_time + write_time) / 2;
    else
        avg_write_time = write_time;
    if (write_time > max_write_time)
        max_write_time = write_time;
    if (write_time < min_write_time)
        min_write_time = write_time;
    BDBG_MSG(("socket (fd = %d) write time: cur %u, avg %u, max %u, min %u\n",
        sd, write_time, avg_write_time, max_write_time, min_write_time));
#endif

    return wbuf_len;
}

char *B_PlaybackIp_UtilsStristr ( char *str, char *subStr)
{
   char *p, *startn = 0, *np = 0;

   /* TODO add logic to ignore initial white spaces */
    for (p = str; *p; p++) {
        if (np) {
            if (toupper(*p) == toupper(*np)) {
                if (!*++np)
                    return startn;
            } else
                np = 0;
        } else if (toupper(*p) == toupper(*subStr)) {
            np = subStr + 1;
            startn = p;
        }
    }
    return 0;
}

int 
B_PlaybackIp_UtilsGetLine(char **linePtr, int *linePtrLength, char *buffer, int bufferLength)
{
    char *p;
    int processed = 0;

    BDBG_MSG(("%s: out buffer length %d, in line buffer len %d", __FUNCTION__, *linePtrLength, bufferLength));
    for (p = buffer; processed <= bufferLength; p++) {
        if (*p == '\n' || processed == bufferLength) {
            processed += 1; /* add an extra byte to include the space for null char */
            if (*linePtr == NULL || (*linePtr && processed > *linePtrLength)) {
                BDBG_MSG(("%s: linePtr buffer (%p) is either NULL or small (needed %d, size %d), reallocing it", __FUNCTION__, *linePtr, processed, *linePtrLength));
                if ((*linePtr = (char *)realloc(*linePtr, processed)) == NULL) {
                    BDBG_ERR(("%s: Failed re-allocating linePtr buffer to size %d", __FUNCTION__, processed));
                    return -1;
                }
                BDBG_MSG(("%s: linePtr buffer (%p) is realloced to size %d", __FUNCTION__, *linePtr, processed));
            }
            memcpy(*linePtr, buffer, processed);
            if (buffer[processed-2] == '\r')
                (*linePtr)[processed -2] = '\0';
            else
                (*linePtr)[processed -1] = '\0';
            *linePtrLength = processed;
            return processed;
        }
        processed++;
    }
    BDBG_MSG(("%s: processed %d, bufferLength %d", __FUNCTION__, processed, bufferLength));
    return -1;
}

/* Parses the input for pattern begin; followed by pattern end */
int 
B_PlaybackIp_UtilsParseToken(char *input, char *output, int output_len, char *begin, char *end)
{
	char *p_begin = strstr(input, begin);
	char *p_end;
	char temp;

	if (p_begin)
	{
		p_begin += strlen(begin);
		p_end = B_PlaybackIp_UtilsStristr(p_begin,end);
		if(!p_end)
			return -1;
		temp = *p_end;
		*p_end = 0;
		BDBG_MSG(("TokenFound = [%s]",p_begin));
#ifndef DMS_CROSS_PLATFORMS
		strlcpy(output,p_begin, output_len-1);
#else
		if ( output_len > 1 )
			strncpy(output,p_begin, output_len-1);
#endif /* DMS_CROSS_PLATFORMS */
		*p_end = temp;
		return 0;
	}
	return -1; /* Not found */
}

int
http_get_payload_content_length(char *http_hdr, off_t *length, bool *chunk_encoding) 
{
    char *tmp1;
    char *endStr;

    /* 
     * search for Transfer-Encoding (case insensative) header, if found, look for chunked value.
     * if found, then server is transferring data using chunked encoding.
     * Otherwise, look for Content-Length header, if found, use that as length
     * In all other cases, content length is undefined and we rely on server to
     * close the connection.

     * Also, chunk encoding format can also have a trailer at the end. It is indicated by the 
     * Trailer header in the message headers. For now, we dont support it and mark such message
     * as not supported.
     */

    /* parse chunk tranfer encoding header if present */
    *chunk_encoding = false;
    tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Transfer-Encoding:");
    if (tmp1) {
        tmp1 += strlen("Transfer-Encoding:");
        endStr = strstr(tmp1, "\r\n");
        if (endStr) {
            endStr[0] = '\0';
            if (B_PlaybackIp_UtilsStristr(tmp1, "chunked"))
                *chunk_encoding = true;
            endStr[0] = '\r';
            BDBG_MSG(("Server is using chunked Transfer-Encoding to send the Content\n"));
            *length = 0;
            /* we used to return at this point, but now we are trying to find contentLength by parsing the Content-Range header if present */
            tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Content-Range:");
            if (tmp1) {
                /* example of Content-Range header */
                /* Content-Range: bytes 21010-47021/47022 */
                tmp1 += strlen("Content-Range:");
                endStr = strstr(tmp1, "\r\n");
                if (endStr) {
                    char *tmp2;
                    endStr[0] = '\0';
                    /* now look for the instance-length which is after the / char */
                    tmp2 = strstr(tmp1, "/");
                    if (tmp2) {
                        /* instance lenght is given, make sure it is not * which indicates unknown value */
                        /* skip over / char */
                        tmp2++;
                        if (*tmp2 != '*') {
                            *length = strtoll(tmp2, (char **)NULL, 10);
                            BDBG_WRN(("Content-Length: %lld, determined by using the Content-Range header", *length));
                        }
                    }
                    endStr[0] = '\r';
                }
                else {
                    BDBG_ERR(("%s: Received incorrect HTTP Message Header: CRLF pair is missing\n", __FUNCTION__));
                    return -1;
                }
            }
            return 0;
        }
        else {
            BDBG_ERR(("%s: Received incorrect HTTP Message Header: CRLF pair is missing\n", __FUNCTION__));
            return -1;
        }
    }

    /* server is not using chunked tranfer encoding, search for Content-Length Header if present */
    tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Content-Length: ");
    if (tmp1) {
        tmp1 += strlen("Content-Length: ");
        endStr = strstr(tmp1, "\r\n");
        if (endStr) {
            endStr[0] = '\0';
            *length = strtoll(tmp1, (char **)NULL, 10);
            endStr[0] = '\r';
            BDBG_MSG(("Content-Length: %lld\n", *length));
        }
        else {
            BDBG_ERR(("%s: Received incorrect HTTP Message Header: CRLF pair is missing\n", __FUNCTION__));
            return -1;
        }
    }
    else {
        /* neither server sent the Content-Length nor it is using the chunk transfer encoding */
        /* set length to 0, player will keep asking for content until server closes connection */
        *length = 0;
    }
    return 0;
}

char *
_http_response_header_terminator(
    char *buffer, 
    int bufferLen
    )
{
    char *ptr, *ptrEnd;
    int cnt = 0;

    /* HTTP header is terminated with "[r]\n[\r]\n" pattern */
    /* to simplyfy, we just look for "\n[\r]\n" pattern and return the pointer to the next char */
    ptr = buffer;
    ptrEnd = buffer+bufferLen;

    /* look for \n\r\n or \n\n pattern anywhere in the string */
    for (; ptr < ptrEnd-2; ptr++, cnt++) {
        if (*ptr == '\n') {
            if (ptr[1] == '\r' && ptr[2] == '\n') {
                return ptr + 3;
            }
            else if (ptr[1] == '\n') {
                return ptr + 2;
            }
        }
    }
    /* didn't find the "\n][\r]\n" pattern, so look for "\n\n" directly before the end */
    if (ptr[0] == '\n' && ptr[1] == '\n')
        return ptr + 2;

    /* HTTP header terminator is not yet found */
    return NULL;
}

/* 
 * This function parses the given HTTP response header and returns various header information.
 * It returns: 
 *  !B_ERROR_SUCCESS when any errors are encountered during HTTP Response Header parsing.
 *  B_ERROR_SUCCESS when HTTP Response header is successfully parsed. HttpFields are filled-in.
 */
B_PlaybackIpError 
B_PlaybackIp_UtilsHttpResponseParse(
    char *rbuf, 
    unsigned int bytesRead,
    B_PlaybackIpHttpMsgFields *httpFields)
{
    char tmpChar;
    char tmpChar0;
    char *tmpPtr;
    char *endStr;

    if (rbuf == NULL || httpFields == NULL) {
        return B_ERROR_INVALID_PARAMETER;
    }
    memset(httpFields, 0, sizeof(B_PlaybackIpHttpMsgFields));

    if (bytesRead < strlen(HTTP_HDR_TERMINATOR_STRING)) {
        httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eIncompleteHdr;
        return B_ERROR_INVALID_PARAMETER;
    }

	tmpPtr = _http_response_header_terminator(rbuf, bytesRead);
    if (!tmpPtr) {
        httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eIncompleteHdr;
        return B_ERROR_PROTO;
    }

    /* Complete HTTP Response Header is found */
    httpFields->httpHdr = rbuf;
    httpFields->httpPayload = tmpPtr;
    httpFields->httpPayloadLength = bytesRead - (httpFields->httpPayload - httpFields->httpHdr);

    /* prepare rbuf for string manipulations */
    tmpChar0 = rbuf[bytesRead-1];
    rbuf[bytesRead-1] = '\0';

    /* Now look for the status code in the HTTP response */
    /* 1xx response codes are continue messages. Caller should go back to reading the next response from server */
    /* 2xx response codes indicate the successful response */
    /* Other responses are treated as errors or not supported */
    tmpPtr = B_PlaybackIp_UtilsStristr(rbuf, "HTTP/1.1 ");
    if (!tmpPtr) 
        tmpPtr = B_PlaybackIp_UtilsStristr(rbuf, "HTTP/1.0 ");
    if (!tmpPtr) {
        tmpPtr = B_PlaybackIp_UtilsStristr(rbuf, "ICY 200 OK");
        if (tmpPtr) {
            BDBG_WRN(("%s: Received ICY Header in response", __FUNCTION__));
            rbuf[bytesRead-1] = tmpChar0;
            return B_ERROR_SUCCESS;
        }
        else {
            BDBG_ERR(("%s: ERROR: Received incorrect HTTP response -->\n%s", __FUNCTION__, rbuf));
            httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eIncorrectHdr;
            return B_ERROR_PROTO;
        }
    }
    tmpPtr += strlen("HTTP/1.1 ");
    endStr = strstr(tmpPtr, "\r\n");
    if (!endStr) {
        BDBG_ERR(("%s: ERROR: Received invalid HTTP response -->\n%s", __FUNCTION__, rbuf));
        httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eIncorrectHdr;
        return B_ERROR_PROTO;
    }

    *endStr = '\0';
    httpFields->statusCode = strtol(tmpPtr, (char **)NULL, 10);
    *endStr = '\r';
    if (httpFields->statusCode < 200) {
        tmpChar = httpFields->httpPayload[0];
        httpFields->httpPayload[0] = '\0';
        BDBG_WRN(("%s: Received HTTP 1xx status code (%d), HTTP Response: ----->\n%s\n", __FUNCTION__, httpFields->statusCode, rbuf));
        httpFields->httpPayload[0] = tmpChar;
        httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eReadNextHdr;
        return B_ERROR_PROTO;
    }
    else if (httpFields->statusCode < 300) {
		BDBG_MSG(("%s: Received valid HTTP Response (status code %d, size %d)", __FUNCTION__, httpFields->statusCode, bytesRead));
        tmpChar = httpFields->httpPayload[0];
        httpFields->httpPayload[0] = '\0';
        BDBG_MSG(("%s: HTTP Get Response: ---->\n%s", __FUNCTION__, rbuf));
        httpFields->httpPayload[0] = tmpChar;
    }
    else if (httpFields->statusCode < 400) {
        BDBG_MSG(("%s: Received HTTP Redirect (status code %d, size %d)\n", __FUNCTION__, httpFields->statusCode, bytesRead));
        tmpChar = httpFields->httpPayload[0];
        httpFields->httpPayload[0] = '\0';
        BDBG_MSG(("%s: HTTP Get Response: ---->\n%s", __FUNCTION__, rbuf));
        httpFields->httpPayload[0] = tmpChar;
        httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eStatusRedirect;
        return B_ERROR_PROTO;
    }
    else {
        tmpChar = httpFields->httpPayload[0];
        httpFields->httpPayload[0] = '\0';
        BDBG_ERR(("ERROR: HTTP Request Failed, status code %d, Received incorrect HTTP response: --->\n%s", httpFields->statusCode, rbuf));
        httpFields->httpPayload[0] = tmpChar;
        if (httpFields->statusCode >=400 && httpFields->statusCode < 500)
            httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eStatusBadRequest;
        else if (httpFields->statusCode >=500 && httpFields->statusCode < 600)
            httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eStatusServerError;
        else
            httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eStatusNotSupported;
        return B_ERROR_PROTO;
    }
    rbuf[bytesRead-1] = tmpChar0;

    /* 
     * Now that we have complete HTTP Response Message header, see if server has sent any pertinent headers.
     * Currently, we are interested in Content-Length, Content-Type, Transfer-Encoding headers
     * We process Transfer-Encoding & Content-Length headers in this function as these determine 
     * where HTTP payload starts 
     */
    httpFields->chunkEncoding = false;
    if (http_get_payload_content_length(rbuf, &httpFields->contentLength, &httpFields->chunkEncoding)) {
        BDBG_ERR(("%s: ERROR: HTTP Content Length parsing failed, content length %lld, chunk encoding enabled %d\n",
            __FUNCTION__, httpFields->contentLength, httpFields->chunkEncoding));
        httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eIncorrectHdr;
        return B_ERROR_PROTO;
    }

    /* 
    * If server is using HTTP Chunked Transfer-Encoding to send the content,
    * we will need to skip the chunk headers also from the payload pointer.
    */
#ifndef ANDROID    
    BDBG_MSG(("Received Valid HTTP Response: status code %d, content length %lld, chunk xfer encoding %s, initial_payload_len %d", 
        httpFields->statusCode, httpFields->contentLength, (httpFields->chunkEncoding == true) ? "Yes":"No", httpFields->httpPayloadLength));
#else
	BDBG_MSG(("Received Valid HTTP Response: status code %d, content length %lld, chunk xfer encoding %d, initial_payload_len %d", 
        httpFields->statusCode, httpFields->contentLength, httpFields->chunkEncoding == true, httpFields->httpPayloadLength));
#endif
                
    httpFields->parsingResult = B_PlaybackIpHttpHdrParsingResult_eSuccess;
    return B_ERROR_SUCCESS;
}


#define IP_SETUP_INTERVAL_TIMEOUT_SECS  2

#ifndef  DMS_CROSS_PLATFORMS   
int 
B_PlaybackIp_UtilsWaitForPlaypumpDecoderSetup(B_PlaybackIpHandle playback_ip)
{
    NEXUS_Error rc;
    BERR_Code berr;
    NEXUS_PlaypumpStatus        pumpStatus;
    BDBG_MSG(("Entered %s()\n", __FUNCTION__));
    memset(&pumpStatus, 0, sizeof(NEXUS_PlaypumpStatus));

    /* check if playpump has started */
    rc = NEXUS_Playpump_GetStatus(playback_ip->nexusHandles.playpump, &pumpStatus);
    if (!rc) {
        if (pumpStatus.started) {
            /* Playpump has started.  No need to wait for event */
            BDBG_MSG(("%s: Playpump has started.  No need to wait for event", __FUNCTION__));
            return 0;
        }
    }

    /* wait for playpump to start */
    for (;;) {
        if ((playback_ip->playback_state == B_PlaybackIpState_eStopping) || 
            (playback_ip->playback_state == B_PlaybackIpState_eStopped) ) { 
            BDBG_ERR(("%s: ERROR: PlaybackIp stopping: state %d", __FUNCTION__, playback_ip->playback_state));
            return -1;
        }
        berr = BKNI_WaitForEvent(playback_ip->read_callback_event, IP_SETUP_INTERVAL_TIMEOUT_SECS*1000);
        if (berr == BERR_TIMEOUT) {
            BDBG_WRN(("%s: Playpump start interval timed out.", __FUNCTION__ ));
            /* continue; */
        } else if (berr!=0) {
            BDBG_ERR(("%s: ERROR: Playpump status wait failed: %d", __FUNCTION__,berr ));
            return -1;
        }

        /* Get playpump status */
        rc = NEXUS_Playpump_GetStatus(playback_ip->nexusHandles.playpump, &pumpStatus);
        if (!rc) {
            if (pumpStatus.started) {
                /* Playpump has started. */ 
                return 0;
            }
        } else {
            BDBG_ERR(("%s: ERROR: Playpump get status failed: %d", __FUNCTION__,rc ));
        }
    }

    return -1;
}
#endif /* DMS_CROSS_PLATFORMS */   

/* 
Summary:
Returns bits 'e'..'b' from word 'w', 

Example:
    B_GET_BITS(0xDE,7,4)==0x0D
    B_GET_BITS(0xDE,3,0)=0x0E 
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

int
B_PlaybackIp_UtilsBuildWavHeader(
    uint8_t *buf, 
    size_t bufSize, 
    unsigned bitsPerSample, 
    unsigned sampleRate, 
    unsigned numChannels
    )
{
    static const uint8_t wavTemplate[] = {
        'R','I','F','F',
        0xFF,0xFF,0xFF,0xFF,     /* unbounded size */
        'W','A','V','E',
        'f','m','t',' ',
        0x12, 0x00, 0x00, 0x00, /* length of fmt block */
        0x01, 0x00,             /* FormatTag -> LPCM */
        0x00, 0x00,             /* Channels */
        0x00, 0x00, 0x00, 0x00, /* sample rate */
        0x00, 0x00, 0x00, 0x00, /* average byte rate */
        0x00, 0x00,             /* block align */
        0x00, 0x00,             /* bits per sample */
        0x00, 0x00,             /* size of extra block */
        'd','a','t','a',
        0xFF,0xFF,0xFF,0x00     /* unbounded size */
    };
    unsigned blockAlign = (bitsPerSample/8)*numChannels;
    unsigned byteRate = blockAlign*sampleRate;

    if(bufSize<sizeof(wavTemplate)) {
        /* not enough size for the header */
        BDBG_WRN(("%s: ERROR: not enough size (%d) building the Wave header\n", __FUNCTION__, bufSize, sizeof(wavTemplate)));
        return -1;
    }
    BKNI_Memcpy(buf, wavTemplate, sizeof(wavTemplate));
    /* populate variable fields */
    buf[22] = B_GET_BITS(numChannels, 7, 0);
    buf[23] = B_GET_BITS(numChannels, 15, 8);

    buf[24] = B_GET_BITS(sampleRate, 7, 0);
    buf[25] = B_GET_BITS(sampleRate, 15, 8);
    buf[26] = B_GET_BITS(sampleRate, 23, 16);
    buf[27] = B_GET_BITS(sampleRate, 31, 24);

    buf[28] = B_GET_BITS(byteRate, 7, 0);
    buf[29] = B_GET_BITS(byteRate, 15, 8);
    buf[30] = B_GET_BITS(byteRate, 23, 16);
    buf[31] = B_GET_BITS(byteRate, 31, 24);

    buf[32] = B_GET_BITS(blockAlign, 7, 0);
    buf[33] = B_GET_BITS(blockAlign, 15, 8);

    buf[34] = B_GET_BITS(bitsPerSample, 7, 0);
    buf[35] = B_GET_BITS(bitsPerSample, 15, 8);

    return sizeof(wavTemplate);
}

void
B_PlaybackIp_UtilsByteSwap(
    char *buf, 
    int bufSize
    )
{
    int i;
    char tmp;

    for (i=0; i<bufSize; i+=2) {
        tmp = buf[i];
        buf[i] = buf[i+1];
        buf[i+1] = tmp;
    }
    BDBG_MSG(("%s: Byte-swapped %d bytes\n", __FUNCTION__, bufSize));
}

/* # of io vectors for the sendmsg() call, currently set to 2 to avoid IP fragmentation */
/* should be increased when our NIC starts supporting Segmentation Offload technology to allow single sendmsg call */
#define B_RTP_IOVEC 2   
#define B_UDP_IOVEC 1
#define RTP_UDP_AV_PAYLOAD_SIZE 1316
/* this is for little endian */
typedef struct RtpHeaderBits {
    unsigned int cc:4;              /* CSRC identifiers */
    unsigned int x:1;               /* extension headers */
    unsigned int p:1;               /* padding */
    unsigned int v:2;               /* version */
    unsigned int pt:7;              /* payload type, see RFC 1890 */
    unsigned int m:1;               /* marker */
    unsigned int seq:16;            /* sequence number */
}RtpHeaderBits;

typedef struct RtpHeader {
  RtpHeaderBits b;
  int timestamp;
  int csrc;
}RtpHeader;

int buildRtpHeader(unsigned short nextSeq, RtpHeader *rh, int payloadType, bool lastPktInAvFrame)
{
    memset(rh, 0, sizeof(RtpHeader));
    rh->b.v = 2;
    if (lastPktInAvFrame)
        rh->b.m = 1;
    rh->b.pt = payloadType;
    rh->timestamp = 0; /* TODO: may need to set this at some point */
    rh->csrc = 0x12121212;
    rh->b.seq = htons(nextSeq);
    return (sizeof(RtpHeader));
}

/* buids the iovec: even # contains RTP header, odd # contains the next RTP_UDP_AV_PAYLOAD_SIZE byte payload */
int 
buildRtpIovec(struct bfile_io_write_net *data, unsigned char *buf, int bufSize, int *totalBytesInIovec, int *totalPayloadCopied) 
{
    int offset, i, bytesToSend, rtpHdrSize; 
    bool lastPktInAvFrame = false;
    struct bfile_io_write_net *file = (struct bfile_io_write_net *)data;
    char rtpHdr[32];

    BDBG_MSG(("%s: build iovec for %d bytes of payload", __FUNCTION__, bufSize));
    /* chop bufSize bytes to RTP_UDP_AV_PAYLOAD_SIZE size IP packets and prepend each w/ RTP header */
    for (
         offset=0,i=0,*totalBytesInIovec=0, *totalPayloadCopied=0;
         bufSize>0 && i<B_RTP_IOVEC;
         i+=2, offset+=RTP_UDP_AV_PAYLOAD_SIZE, bufSize-=bytesToSend
         ) 
    {
        if (bufSize <= RTP_UDP_AV_PAYLOAD_SIZE) {
            /* last RTP packet for this frame, set the marker bit */
            lastPktInAvFrame = true;
        }
        else {
            lastPktInAvFrame = false;
        }
        BDBG_MSG(("%s: lastPktInFrame %d", __FUNCTION__, lastPktInAvFrame));
        rtpHdrSize = buildRtpHeader(data->nextSeq++, (RtpHeader *)rtpHdr, data->rtpPayloadType, lastPktInAvFrame);

        bytesToSend = bufSize >= RTP_UDP_AV_PAYLOAD_SIZE ? RTP_UDP_AV_PAYLOAD_SIZE : bufSize;
        file->iovec[i].iov_base = (char *)rtpHdr;
        file->iovec[i].iov_len = rtpHdrSize;
        file->iovec[i+1].iov_base = buf+offset;
        file->iovec[i+1].iov_len = bytesToSend;
        *totalBytesInIovec += bytesToSend + rtpHdrSize;
        *totalPayloadCopied += bytesToSend;
        BDBG_MSG(("%s: iovecs[%d].len %d, iovecs[%d].len %d", __FUNCTION__, i, file->iovec[i].iov_len, i+1, file->iovec[i+1].iov_len));
    }
    BDBG_MSG(("%s: built %d iovecs for %d bytes, total in iovec (including RTP headers) %d", __FUNCTION__, i, *totalPayloadCopied, *totalBytesInIovec));
    return i;
}

/* buids the iovec: each iovec contains one UDP payload */
int 
buildUdpIovec(struct bfile_io_write_net *data, unsigned char *buf, int bufSize, int *totalBytesInIovec, int *totalPayloadCopied) 
{
    int offset, i, bytesToSend; 
    struct bfile_io_write_net *file = (struct bfile_io_write_net *)data;

    BDBG_MSG(("%s: build iovec for %d bytes ", __FUNCTION__, bufSize));
    for (
         offset=0,i=0,*totalBytesInIovec=0, *totalPayloadCopied=0;
         bufSize>0 && i<B_UDP_IOVEC; 
         i+=1, offset+=RTP_UDP_AV_PAYLOAD_SIZE, bufSize-=bytesToSend
         ) 
    {
        bytesToSend = bufSize >= RTP_UDP_AV_PAYLOAD_SIZE ? RTP_UDP_AV_PAYLOAD_SIZE : bufSize;
        file->iovec[i].iov_base = buf+offset;
        file->iovec[i].iov_len = bytesToSend;
        *totalBytesInIovec += bytesToSend;
        *totalPayloadCopied += bytesToSend;
        BDBG_MSG(("%s: iovecs[%d].len %d", __FUNCTION__, i, file->iovec[i].iov_len));
    }
    BDBG_MSG(("%s: built %d iovecs for %d bytes, total in iovec (including RTP headers) %d", __FUNCTION__, i, *totalPayloadCopied, *totalBytesInIovec));
    return i;
}

int 
sendRtpUdpChunk(struct bfile_io_write_net *data, unsigned char *buf, int bufSize)
{
    int bytesWritten = 0, totalWritten = 0, iovecCount;
    struct msghdr msg;
    int totalBytesToWrite;
    int totalPayloadCopied;

    BDBG_MSG(("%s: write %d bytes on socket %d", __FUNCTION__, bufSize, data->fd));

    memset(&msg, 0, sizeof(struct msghdr));
    msg.msg_name = (struct sockaddr *)&data->streamingSockAddr; 
    msg.msg_namelen = sizeof(struct sockaddr_in);
    while (bufSize > 0) {
        if (data->streamingProtocol == B_PlaybackIpProtocol_eRtp) {
            iovecCount = buildRtpIovec(data, buf, bufSize, &totalBytesToWrite, &totalPayloadCopied);
        }
        else {
            iovecCount = buildUdpIovec(data, buf, bufSize, &totalBytesToWrite, &totalPayloadCopied);
        }
        msg.msg_iov = data->iovec;
        msg.msg_iovlen = iovecCount;
        if ((bytesWritten = sendmsg(data->fd, &msg, 0)) <= 0) {
            perror("sendmsg():");
            goto out;
        }
        if (totalBytesToWrite != bytesWritten) {
            BDBG_ERR(("%s: Failed to sendmsg all bytes: asked %d, sent %d", __FUNCTION__, totalBytesToWrite, bytesWritten));
            goto out;
        }
        buf += totalPayloadCopied;
        bufSize -= totalPayloadCopied;
        totalWritten += totalPayloadCopied;
        if (bufSize > 0) {
            BDBG_MSG(("%s: finished writing %d bytes (payload bytes %d), to write %d\n", __FUNCTION__, bytesWritten, totalPayloadCopied, bufSize));
        }
    }
out:
    BDBG_MSG(("%s: wrote %d bytes of payload\n", __FUNCTION__, totalWritten));
    return totalWritten;
}

#if 1
#define USE_NON_BLOCKING_MODE
#endif
#define SELECT_TIMEOUT_FOR_SOCKET_SEND 100000
#define MAX_SELECT_TIMEOUT_COUNT 50 /* timeout duraton of 5 sec */
/* loops until all bytes are sent or we get an error */
static int 
sendData(int fd, void *outBuf, int bytesToSend) 
{
    int bytesSent = 0;
    int rc;
    int loopCount = 0;

    while (bytesToSend) {
        loopCount++;
#ifdef USE_NON_BLOCKING_MODE
        if (B_PlaybackIp_UtilsWaitForSocketWriteReady(fd, SELECT_TIMEOUT_FOR_SOCKET_SEND /* timeout in usec*/) < 0) {
            BDBG_ERR(("%s: Select ERROR", __FUNCTION__));
            return -1;
        }
#endif
        BDBG_MSG(("Ok to write %d bytes \n", bytesToSend));
#ifdef USE_NON_BLOCKING_MODE
        rc = send(fd, (void*)outBuf, bytesToSend, MSG_NOSIGNAL | MSG_DONTWAIT );
#else
        rc = send(fd, (void*)outBuf, bytesToSend, MSG_NOSIGNAL );
#endif
        if (rc < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                BDBG_MSG(("%s: got error: timeout or eagain error :%d, retrying", __FUNCTION__, errno));
                continue;
            }
            /* it is not EINTR & EAGAIN, so all errors are serious network failures */
            BDBG_MSG(("%s: send() ERROR:%d", __FUNCTION__, errno));
            return -1;
        }
        BDBG_MSG(("sent %d bytes out of bytesToSend %d\n", rc, bytesToSend));
        bytesSent += rc;
        bytesToSend -= rc;
        outBuf = (unsigned char *)outBuf + rc;
    }
    BDBG_MSG(("%s: send data in %d attempts, bytesSent %d for socket %d", __FUNCTION__, loopCount, bytesSent, fd));
    return bytesSent;
}

/* tries to send the length bytes: determines how much data can socket q take and writes that much */ 
/* caller is required to resend any remaining bytes */
ssize_t
B_PlaybackIp_UtilsStreamingCtxWrite(bfile_io_write_t self, const void *buf, size_t length)
{
    struct bfile_io_write_net *file = (struct bfile_io_write_net *) self;
    ssize_t rc;
    size_t bytesSent = 0;
    int writeQueueDepth;
    unsigned int bytesConsumed = 0;     /* how many out of length bytes were actually consumed (sent) */
#ifdef B_HAS_DTCP_IP
    unsigned int bytesToEncrypt;     /* how many bytes to encrypt */
    unsigned int bytesEncrypted;     /* how many bytes were encrypted */
    unsigned int encryptedBufSize;   /* encrypted bytes including any DTCP headers + padding */
    struct timeval curTime, startTime, endTime;
#endif
    size_t writeQueueSpaceAvail; /* current space available in write q */
    int outBufLength;       /* how many bytes to send out */
    void *outBuf;           /* pointer to the out buffer */
#ifdef B_USE_HTTP_CHUNK_ENCODING
    char chunkHdr[16];
    int chunkHdrToSend;
#endif
    
    BDBG_ASSERT(file);

#ifdef USE_NON_BLOCKING_MODE
    /* check is socket has some space to send data */
    if ((rc = B_PlaybackIp_UtilsWaitForSocketWriteReady(file->fd, SELECT_TIMEOUT_FOR_SOCKET_SEND /* timeout in usec*/)) <= 0) {
        return rc;
    }
#endif

    /* now check how much data we can currently send */
    if (ioctl(file->fd, SIOCOUTQ, &writeQueueDepth)) {
        BDBG_WRN(("%s: failed to get tcp write q depth for socket %d", __FUNCTION__, file->fd));
        writeQueueDepth = 0;
    }
    writeQueueSpaceAvail = file->writeQueueSize - writeQueueDepth;
    BDBG_MSG(("%s: Write %d bytes for socket %d (wr q depth %d, size %d, rem %d)\n", __FUNCTION__, length, file->fd, writeQueueDepth, file->writeQueueSize, writeQueueSpaceAvail));
    file->writeQueueFullTimeouts = 0;
    if (length > writeQueueSpaceAvail) {
        BDBG_MSG(("Write trimming # of bytes to write (%d) to the write q available space (%d) for socket %d\n", length, writeQueueSpaceAvail, file->fd));
        length = writeQueueSpaceAvail;
    }
    if (length > 0) {
#ifdef B_HAS_DTCP_IP
        if (file->dtcpStreamHandle) {
            /* DTCP/IP encryption is enabled, so encrypt the outgoing data */
            bytesToEncrypt = length > STREAMING_BUF_SIZE ? STREAMING_BUF_SIZE : (length - length%16);
            BDBG_MSG(("Encrypt %d bytes, asked %d", bytesToEncrypt, length));
            encryptedBufSize = file->encryptedBufSize;
            gettimeofday(&startTime, NULL);
            if (DtcpAppLib_StreamPacketizeData(
                        file->dtcpStreamHandle,
                        file->dtcpAkeHandle,
                        (unsigned char *)buf,   /* In: clear buffer */
                        bytesToEncrypt,         /* In: clear buffer size */
                        &file->encryptedBuf,    /* OUT: encrypted buffer */
                        &encryptedBufSize,      /* In/OUT: max size of encrypted buffer/encrypted buffer size (may include additional 14 bytes for DTCP/IP header) */
                        &bytesEncrypted         /* OUT: Total input data processed for this call*/
                        ) != BERR_SUCCESS) {
                    BDBG_ERR(("ERROR: Failed to encrypt data for DTCP stream (fd %d)\n", file->fd));
                    return -1;
            }
            gettimeofday(&endTime, NULL);
            timeSub(&endTime, &startTime, &curTime);
            timeAdd(&file->totalTime, &curTime, &file->totalTime);
            if (curTime.tv_sec > 1) {
                BDBG_WRN(("TOOK over a second for encrypt operation: time: cur sec %d, usec %d\n", curTime.tv_sec, curTime.tv_usec));
            }
            else {
                if (curTime.tv_usec < file->minTime.tv_usec)
                    file->minTime.tv_usec = curTime.tv_usec;
                if (curTime.tv_usec > file->maxTime.tv_usec)
                    file->maxTime.tv_usec = curTime.tv_usec;
            }
            BDBG_MSG(("time: cur sec %d, usec %d, total sec %d, usec %d, iter %u\n", 
                        curTime.tv_sec, curTime.tv_usec,
                        file->totalTime.tv_sec, file->totalTime.tv_usec, file->totalIterations));
            file->totalIterations++;
            BDBG_MSG(("Encrypted bytes: %d, asked %d, to send %d\n", bytesEncrypted, bytesToEncrypt, encryptedBufSize));
            if (bytesEncrypted > bytesToEncrypt) {
                BDBG_ERR(("ERROR: BUG in DTCP/IP layer: bytes to encrypt %d, encrypted %d\n", bytesToEncrypt, bytesEncrypted));
                return -1;
            }
            outBuf = file->encryptedBuf;
            outBufLength = encryptedBufSize;
        }
        else 
#endif
        {
            outBuf = (void *)buf;
            outBufLength = length;
        }
#ifdef B_USE_HTTP_CHUNK_ENCODING
        {
            memset(chunkHdr, 0, sizeof(chunkHdr));
            chunkHdrToSend = snprintf(chunkHdr, sizeof(chunkHdr)-1, "%x\r\n", outBufLength);
            rc = sendData(file->fd, chunkHdr, chunkHdrToSend);
            if (rc != chunkHdrToSend) {
                BDBG_MSG(("%s: Failed to send %d bytes of chunk header begining, sent %d for socket %d", __FUNCTION__, chunkHdrToSend, rc, file->fd));
                return -1;
            }
            BDBG_MSG(("wrote %d bytes of chunk hdr beginging: %s, chunkhdr size %d", chunkHdrToSend, chunkHdr, outBufLength));
        }
#endif
        if (file->streamingProtocol == B_PlaybackIpProtocol_eHttp)
            rc = sendData(file->fd, outBuf, outBufLength);
        else
            rc = sendRtpUdpChunk(file, outBuf, outBufLength);
        if (rc != outBufLength) {
            BDBG_MSG(("%s: Failed to send %d bytes, sent %d for socket %d", __FUNCTION__, outBufLength, rc, file->fd));
            return -1;
        }
        bytesSent += rc;

#ifdef B_HAS_DTCP_IP
        if (file->dtcpStreamHandle) {
            /* Note: bytesConsumed may not be equal to bytes sent as DTCP library will add PCP header to each encrypted payload */
            bytesConsumed += bytesEncrypted;
        }
        else 
#endif
        {
            bytesConsumed += rc;
        }
#ifdef B_USE_HTTP_CHUNK_ENCODING
        {
            /* send end of current chunk header */
            memset(chunkHdr, 0, sizeof(chunkHdr));
            chunkHdrToSend = snprintf(chunkHdr, sizeof(chunkHdr)-1, "\r\n");
            rc = sendData(file->fd, chunkHdr, chunkHdrToSend);
            if (rc != chunkHdrToSend) {
                BDBG_MSG(("%s: Failed to send %d bytes of chunk header end, sent %d for socket %d", __FUNCTION__, chunkHdrToSend, rc, file->fd));
                return -1;
            }
            BDBG_MSG(("wrote %d bytes of chunk hdr end: %x %x", chunkHdrToSend, chunkHdr[0], chunkHdr[1]));
        }
#endif
    }
    file->totalBytesConsumed += bytesConsumed;
    BDBG_MSG(("%s: asked %d, Wrote %d, Consumed %d (total %lld) bytes for fd %d\n", __FUNCTION__, length, bytesSent, bytesConsumed, file->totalBytesConsumed, file->fd));
    return bytesConsumed;
}

#if 0
void 
look_for_sync_bytes(unsigned char *buf, int bufSize)
{
    int i;
    for (i=0; i<bufSize; i+=188) {
        if (buf[i] != 0x47) {
            BDBG_ERR(("###################### sync byte not found at %d offset, instead got %x\n", i, buf[i]));
        }
        else
            BDBG_MSG(("~~~~~~~~~~~~~~~~~~~~~ sync byte (%x) found at %d offset", buf[i], i));
    }
}
#endif

/* send the bufSize data unless app stops the flow */
ssize_t 
B_PlaybackIp_UtilsStreamingCtxWriteAll(
    bfile_io_write_t self, 
    const void *buf, 
    size_t bufSize
    )
{
    int bytesWritten = 0;
    ssize_t totalWritten = 0;
    int bytesToWrite;
    struct bfile_io_write_net *file = (struct bfile_io_write_net *)self;
    BDBG_MSG(("%s: write %d bytes\n", __FUNCTION__, bufSize));

#if 0
    look_for_sync_bytes(buf, bufSize);
#endif
    if (file->fd <= 0) {
        BDBG_MSG(("%s: streaming session is not yet enabled, skipping %d bytes", __FUNCTION__, bufSize));
        /* we pretend that we have sent these bytes, this is mainly for Fast Channel Change scenario */
        /* where whole pipe is setup but not data is being streamed out as client hasn't yet tuned to that channel */
        return bufSize;
    }

#ifdef B_HAS_DTCP_IP
    if (!file->liveStreaming && file->residualBytesLength) {
        /* for live streaming, caller ensures that buffer is always mod 16 aligned */
        buf = (unsigned char *)buf - file->residualBytesLength;
        memcpy((unsigned char *)buf, file->residualBytesToEncrypt, file->residualBytesLength);
        bufSize += file->residualBytesLength;
        BDBG_MSG(("%s: Added %d residual bytes for encryption", __FUNCTION__, file->residualBytesLength));
        file->residualBytesLength = 0;
    }
#endif
    while (bufSize) {
        if (file->stopStreaming == true) {
            BDBG_MSG(("%s: app wants to stop streaming, breaking out for fd %d", __FUNCTION__, file->fd));
            return -1;
        }
        
        bytesToWrite = bufSize;
        bytesWritten = B_PlaybackIp_UtilsStreamingCtxWrite((struct bfile_io_write *)file, (void *)buf, (size_t) bytesToWrite);
        if (bytesWritten < 0) {
            /* encountered some error including too many timeouts to write the stream, return */
            return bytesWritten;
        }
        else if (bytesWritten == 0) {
            /* we were not able to send any data, most likely socket is backed up due to n/w congestion or powered off client */
            BDBG_MSG(("%s: Select timeout, # of timeouts %d", __FUNCTION__, file->selectTimeouts));
            /* select timeout */
            file->selectTimeouts++;
            if (file->liveStreaming && file->selectTimeouts > MAX_SELECT_TIMEOUT_COUNT) {
                /* if we need to support pause by connection stalling, then this check only makes sense for Live content */
                BDBG_ERR(("%s: consecutive select timeouts (count %d, duration %d) for socket %d, client is not receiving data, return fatal error", 
                            __FUNCTION__, file->selectTimeouts, (SELECT_TIMEOUT_FOR_SOCKET_SEND * MAX_SELECT_TIMEOUT_COUNT)/(SELECT_TIMEOUT_FOR_SOCKET_SEND*10), file->fd));
                return -1;
            }
            /* retry to send data and see if we succeed */
            continue;
        }
        file->selectTimeouts = 0;
        buf = (unsigned char *)buf + bytesWritten;
        bufSize -= bytesWritten;
        totalWritten += bytesWritten;
        if (bufSize) {
            BDBG_MSG(("%s: wrote %d bytes, to write %d\n", __FUNCTION__, bytesWritten, bufSize));
        }
#ifdef B_HAS_DTCP_IP
        if (!file->liveStreaming && bufSize < 16 && bufSize > 0) {
            BDBG_MSG(("remaining residual bytes %d", bufSize));
            file->residualBytesLength = bufSize;
            memcpy(file->residualBytesToEncrypt, buf, bufSize);
            totalWritten += bufSize;
            break;
        }
#endif
    }
    BDBG_MSG(("%s: wrote %d bytes\n", __FUNCTION__, totalWritten));
    return totalWritten;
}

void
B_PlaybackIp_UtilsStreamingCtxClose(struct bfile_io_write_net *data)
{
#ifdef B_HAS_DTCP_IP
    if (!data)
        return;
    if (data->totalIterations)
        BDBG_MSG(("###### total time: sec %d, usec %d, iter %u, avg usec %d, min %d, max %d ######\n", 
                data->totalTime.tv_sec, data->totalTime.tv_usec, data->totalIterations,
                ((data->totalTime.tv_sec *1000000) + data->totalTime.tv_usec) / data->totalIterations,
                data->minTime.tv_usec,
                data->maxTime.tv_usec
                ));
    if (data->dtcpStreamHandle) {
        DtcpAppLib_CloseStream(data->dtcpStreamHandle);
        NEXUS_Memory_Free(data->encryptedBuf);
        data->dtcpStreamHandle = NULL;
        data->encryptedBuf = NULL;
    }
#else
    BSTD_UNUSED(data);
#endif
}

#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
static void m2mDmaJobCompleteCallback(void *pParam, int iParam)
{
	iParam=iParam;
	BDBG_MSG(("%s: fired", __FUNCTION__));
    BKNI_SetEvent(pParam);
}

void
B_PlaybackIp_UtilsPvrDecryptionCtxClose(struct bfile_io_write_net *data)
{
    if (data->event) {
        BKNI_DestroyEvent(data->event);
        data->event = NULL;
    }
    if (data->dmaJobHandle) {
	    NEXUS_DmaJob_Destroy(data->dmaJobHandle);
        data->dmaJobHandle = NULL;
    }
}

B_PlaybackIpError
B_PlaybackIp_UtilsPvrDecryptionCtxOpen(B_PlaybackIpSecurityOpenSettings *securitySettings, struct bfile_io_write_net *data)
{
    NEXUS_DmaJobSettings jobSettings;

    if (!securitySettings->enableDecryption)
        return B_ERROR_SUCCESS;

    data->pvrDecKeyHandle = securitySettings->pvrDecKeyHandle;
    data->dmaHandle = (NEXUS_DmaHandle)securitySettings->dmaHandle;

    if (BKNI_CreateEvent(&data->event)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks = 1;
	jobSettings.keySlot = data->pvrDecKeyHandle;
	jobSettings.dataFormat = NEXUS_DmaDataFormat_eMpeg;
    jobSettings.completionCallback.callback = m2mDmaJobCompleteCallback;
    jobSettings.completionCallback.context = data->event;
	data->dmaJobHandle = NEXUS_DmaJob_Create(data->dmaHandle, &jobSettings);
    if (data->dmaJobHandle == NULL) {
        BDBG_ERR(("%s: Failed to create Nexus DMA job", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: PVR Decryption setup is successful, jobHandle %p", __FUNCTION__, data->dmaJobHandle));
    return B_ERROR_SUCCESS;

error:
    B_PlaybackIp_UtilsPvrDecryptionCtxClose(data);
    return B_ERROR_PROTO;
}

B_PlaybackIpError
B_PlaybackIp_UtilsPvrDecryptBuffer(struct bfile_io_write_net *data, unsigned char *encryptedBuf, unsigned char *clearBuf, int bufSize, int *outBufSize)
{
    BERR_Code rc;
    NEXUS_Error nrc;
    B_PlaybackIpError status = B_ERROR_PROTO;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobStatus jobStatus;

    BDBG_MSG(("%s: encryptedBuf %p, clear %p, length %d", __FUNCTION__, encryptedBuf, clearBuf, bufSize));

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr = encryptedBuf;
	blockSettings.pDestAddr = clearBuf;
    *outBufSize = blockSettings.blockSize = bufSize;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	blockSettings.cached = false;
    BDBG_MSG(("%s: DMA job %p, size %d", __FUNCTION__, data->dmaJobHandle, blockSettings.blockSize));
	if ((nrc = NEXUS_DmaJob_ProcessBlocks(data->dmaJobHandle, &blockSettings, 1)) != NEXUS_DMA_QUEUED) {
        BDBG_ERR(("%s: NEXUS_DmaJob_ProcessBlocks Failed: dmaJobHandle %p, nexus rc %d", __FUNCTION__, data->dmaJobHandle, nrc));
        goto error;
    }
    BDBG_MSG(("%s: DMA job %p sumitted", __FUNCTION__, data->dmaJobHandle));
    rc = BKNI_WaitForEvent(data->event, 10000);
    if (rc == BERR_TIMEOUT || rc != 0) {
        BDBG_ERR(("%s: Nexus DMA Job completion event failed for PVR decryption, rc %d: %s", __FUNCTION__, rc, rc == BERR_TIMEOUT? "event timeout in 10sec":"event failure"));
        goto error;
    }
    NEXUS_DmaJob_GetStatus(data->dmaJobHandle, &jobStatus);
    if (jobStatus.currentState != NEXUS_DmaJobState_eComplete) {
        BDBG_ERR(("%s: DMA job completion failed, dma job current state %d", __FUNCTION__, jobStatus.currentState));
        goto error;
    }
    BDBG_MSG(("%s: DMA job completed", __FUNCTION__));

    status = B_ERROR_SUCCESS;
error:
    return status;
}
#endif

B_PlaybackIpError
B_PlaybackIp_UtilsStreamingCtxOpen(B_PlaybackIpSecurityOpenSettings *securitySettings, struct bfile_io_write_net *data)
{
    socklen_t len;
    struct sockaddr_in remoteAddr;
    int remoteAddrLen;
    char remoteAddrStr[32];
    B_PlaybackIpError rc;
#ifdef B_HAS_DTCP_IP
    NEXUS_MemoryAllocationSettings allocSettings;
    int iter=0;
#endif

    data->minTime.tv_usec = 2*1000*1000*1000;

    B_PlaybackIp_UtilsTuneNetworkStackTx(data->fd);

    /* get the socket write queue size: needed for determining how much data can be written to socket */
    len = sizeof(data->writeQueueSize);
    if (getsockopt(data->fd, SOL_SOCKET, SO_SNDBUF, &data->writeQueueSize, &len)) {
        BDBG_ERR(("ERROR: can't get socket write q size\n"));
        rc = B_ERROR_PROTO;
        goto error;
    }

    /* For now, all outgoing encryption is going to be using DTCP/IP */
#ifdef B_HAS_DTCP_IP
    if (!securitySettings->enableEncryption) {
        BDBG_MSG(("%s: enable decryption is not set", __FUNCTION__));
        return B_ERROR_SUCCESS;
    }

    remoteAddrLen = sizeof(remoteAddr);
    if (getpeername(data->fd, (struct sockaddr *)&remoteAddr, (socklen_t *)&remoteAddrLen) != 0) {
        BDBG_ERR(("ERROR: Failed to obtain remote IP address, errno: %d\n", errno));
        rc = B_ERROR_PROTO;
        goto error;
    }
    strncpy(remoteAddrStr, inet_ntoa(remoteAddr.sin_addr), sizeof(remoteAddrStr)-1);

    BDBG_MSG(("Configure DTCP/IP (ctx %p) for dst %s, emi %d\n", securitySettings->initialSecurityContext, remoteAddrStr, securitySettings->settings.dtcpIp.emiValue));
    data->dtcpCtx = securitySettings->initialSecurityContext;
    /* Check if DTCP AKE was done successfully */
    do {
        DtcpAppLib_GetSinkAkeSession(data->dtcpCtx, remoteAddrStr, &data->dtcpAkeHandle);
        if (data->dtcpAkeHandle) {
            break;
        } else {
            BDBG_MSG(("%s: Request from %s didn't pass AKE procedure: iter=%d\n", __FUNCTION__, remoteAddrStr, iter));
        }
        BKNI_Sleep(50);
    }
    while (++iter < 60);
    if (iter == 60) {rc = B_ERROR_PROTO; goto error;}
            
    BDBG_MSG(("Request from %s passed AKE procedure, akeHandle %p\n", remoteAddrStr, data->dtcpAkeHandle));

    /* open DTCP content source stream */
    if ((data->dtcpStreamHandle = DtcpAppLib_OpenSourceStream(
                    data->dtcpAkeHandle, 
                    B_StreamTransport_eHttp, 
                    DTCP_CONTENT_LENGTH_UNLIMITED, 
                    securitySettings->settings.dtcpIp.emiValue,
                    B_Content_eAudioVisual, 
                    data->liveStreaming ? 0 : 8388608)) == NULL) /* for live streaming, we set the PCP size to 0 (which enables DTCP lib to prepend PCP on every encrypted block), else on every 8MB boundary */
    {
        BDBG_ERR(("ERROR: Failed to open stream for AKE handle %p\n",  data->dtcpAkeHandle));
        rc = B_ERROR_PROTO;
        goto error;
    }
    
    /* allocate the buffer where outgoing data should get encrypted */
    data->encryptedBufSize = STREAMING_BUF_SIZE + ENCRYPTION_PADDING;
    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    if (data->heapHandle)
        allocSettings.heap = data->heapHandle;
    if (NEXUS_Memory_Allocate(data->encryptedBufSize, &allocSettings, (void *)(&data->encryptedBuf))) {
        BDBG_ERR(("%s: memory allocation failure\n", __FUNCTION__));
        rc = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }
    BDBG_MSG(("Opened DTCP source stream %p, encrypted buf ptr %p", data->dtcpStreamHandle, data->encryptedBuf));
#else
    BSTD_UNUSED(securitySettings);
    BSTD_UNUSED(remoteAddr);
    BSTD_UNUSED(remoteAddrLen);
    BSTD_UNUSED(remoteAddrStr);
    BSTD_UNUSED(rc);
#endif
    return B_ERROR_SUCCESS;

error:
    B_PlaybackIp_UtilsStreamingCtxClose(data);
    return rc;
}

void
B_PlaybackIp_UtilsSetRtpPayloadType(
    NEXUS_TransportType mpegType, int *rtpPayloadType)
{
    switch (mpegType) 
    {
        case NEXUS_TransportType_eEs:
            *rtpPayloadType = 97;
            break;
        case NEXUS_TransportType_eTs:
            *rtpPayloadType = 33;
            break;
        default:
            BDBG_WRN(("%s: using RTP payload type for TS for format %d", __FUNCTION__, mpegType));
            *rtpPayloadType = 33;
    }
}

void *
B_PlaybackIp_UtilsAllocateMemory(int size, NEXUS_HeapHandle heapHandle)
{
#if B_HAS_LIVE_STREAMING || B_HAS_DTCP_IP || B_HAS_HLS_PROTOCOL_SUPPORT || B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
    NEXUS_MemoryAllocationSettings allocSettings;
    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    unsigned char *xbuf;

    if (heapHandle)
        allocSettings.heap = heapHandle;
    if (NEXUS_Memory_Allocate(size, &allocSettings, (void *)(&xbuf))) 
        return NULL;
    else
        return xbuf;
#else
    BSTD_UNUSED(heapHandle);
    return BKNI_Malloc(size);
#endif
}


void 
B_PlaybackIp_UtilsFreeMemory(void *buffer)
{
#if B_HAS_LIVE_STREAMING || B_HAS_DTCP_IP || B_HAS_HLS_PROTOCOL_SUPPORT || B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
    NEXUS_Memory_Free(buffer);
#else
    BKNI_Free(buffer);
#endif
}

void
B_PlaybackIp_UtilsRtpUdpStreamingCtxClose(struct bfile_io_write_net *data)
{
    if (data->iovec)
        B_PlaybackIp_UtilsFreeMemory(data->iovec);
    close(data->fd);
}

B_PlaybackIpError
B_PlaybackIp_UtilsRtpUdpStreamingCtxOpen(B_PlaybackIpSecurityOpenSettings *securitySettings, struct bfile_io_write_net *data)
{
    socklen_t len;
    B_PlaybackIpError rc;
    struct ifreq ifr;

    data->minTime.tv_usec = 2*1000*1000*1000;

    if ((data->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        /* Socket Create Error */
        BDBG_ERR(("%s: failed to create a datagram socket", __FUNCTION__));
        perror("socket:");
        rc = B_ERROR_PROTO;
        goto error;
    }
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, data->interfaceName, sizeof(ifr.ifr_name)-1);
    if (setsockopt(data->fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr) ) < 0 ) {
        BDBG_ERR(("%s: failed to bind a datagram socket", __FUNCTION__));
        perror("SO_BINDTODEVICE");
        rc = B_ERROR_PROTO;
        goto error;
    }
    BDBG_MSG(("%s: Bind %d socket to i/f %s successful", __FUNCTION__, data->fd, data->interfaceName));

#if 0
    /* I dont think we need to tune the network buffers for outgoing sessions */
    B_PlaybackIp_UtilsTuneNetworkStackTx(data->fd);
#endif

    /* get the socket write queue size: needed for determining how much data can be written to socket */
    len = sizeof(data->writeQueueSize);
    if (getsockopt(data->fd, SOL_SOCKET, SO_SNDBUF, &data->writeQueueSize, &len)) {
        BDBG_ERR(("ERROR: can't get socket write q size\n"));
        rc = B_ERROR_PROTO;
        goto error;
    }

    if ((data->iovec = B_PlaybackIp_UtilsAllocateMemory(sizeof(struct iovec)*B_RTP_IOVEC, data->heapHandle)) == NULL) {
        BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
        rc = B_ERROR_PROTO;
        goto error;
    }

#ifdef B_HAS_DTCP_IP
    if (!securitySettings->enableEncryption)
        return B_ERROR_SUCCESS;

    BDBG_ERR(("%s: ERROR: Encrypting RTP session over DTCP/IP is not yet supported", __FUNCTION__));
    rc = B_ERROR_PROTO;
#else
    BSTD_UNUSED(securitySettings);
    rc = B_ERROR_SUCCESS;
#endif

error:
    return rc;
}

#ifdef EROUTER_SUPPORT
B_PlaybackIpError 
B_PlaybackIp_UtilsSetErouterFilter(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionOpenSettings *openSettings
    )
{
    B_PlaybackIpError rc;

    if (strcmp(openSettings->socketOpenSettings.ipAddr, "192.168.0.1")) {
        /* open handle to bcmrgshim */
        playback_ip->bcmrgDrvFd = open("/dev/bcmrgshim", O_RDWR);
        if (playback_ip->bcmrgDrvFd != -1) {
            /* open unicast WAN port */
            IOCTL_IF_LIST_PORTNUMS uniport;
            uniport.portNumb = openSettings->socketOpenSettings.port;
            uniport.add = WAN_PORT_ADD;
            if (ioctl(playback_ip->bcmrgDrvFd, IOCTL_ADD_DELETE_WANUNICAST_PACKET, (void *) &uniport)) {
                BDBG_ERR(("ERROR: Failed to add WAN port: %d\n", uniport.portNumb));
                rc = B_ERROR_SOCKET_ERROR;
                goto error;
            } else {
                BDBG_MSG(("Success Added WAN port: %d\n", uniport.portNumb));
            }
        } else {
            BDBG_ERR(("Unable to open bcmrgshim driver, bcmrgDrvFd=%x\n Check if the erouter is running!\n", playback_ip->bcmrgDrvFd));
            rc = B_ERROR_SOCKET_ERROR;
            goto error;
        }
    }
    rc = B_ERROR_SUCCESS;

error:
    return rc;
}

void 
B_PlaybackIp_UtilsUnsetErouterFilter(
    B_PlaybackIpHandle playback_ip
    )
{
    if (strcmp(playback_ip->openSettings.socketOpenSettings.ipAddr, "192.168.0.1")) {
        if (playback_ip->bcmrgDrvFd != -1) {
            IOCTL_IF_LIST_PORTNUMS uniport;
            /* delete unicast WAN port */
            uniport.portNumb = playback_ip->openSettings.socketOpenSettings.port;
            uniport.add = WAN_PORT_DELETE;
            if (ioctl(playback_ip->bcmrgDrvFd, IOCTL_ADD_DELETE_WANUNICAST_PACKET, (void *) &uniport)) {
                BDBG_ERR(("ERROR: Failed to delete WAN port: %d\n", uniport.portNumb));
            } else {
                BDBG_MSG(("Success Deleted port: %d\n", uniport.portNumb));
            }
            /* close handle to bcmrgshim */
            close(playback_ip->bcmrgDrvFd);
        }
    }
}
#endif

