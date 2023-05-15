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
* $brcm_Workfile: b_playback_ip_file_streaming.c $
* $brcm_Revision: 70 $
* $brcm_Date: 11/9/12 3:22p $
*
* Description: Streaming of a pre-record or pre-downloaded file from local disk to a Remote Client using HTTP protocol
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_file_streaming.c $
* 
* 70   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 69   10/17/12 11:57a ssood
* SW7445-53: get playback_ip to compile for 7445 tool-chain
* 
* 68   9/7/12 5:39p ssood
* SW7420-2358: dont break out streaming loop if autoRewind flag is
*  enabled (after a seek/pause operation)
* 
* 67   8/3/12 2:52p ssood
* SW7425-3678: extend ip lib & ip streamer to support streaming out live,
*  file, encoded content over RTP/UDP
* 
* 66   7/16/12 3:33p ssood
* SW7425-3201: support for streaming trickmode content using playSpeed w/
*  non-Broadcom clients (disable use of BTPs and switch to i-frame only
*  modes)
* 
* 65   6/25/12 6:11p ssood
* SW7435-250: coverity fixes exposed by new coverity version
* 
* 64   6/11/12 4:57p ssood
* SW7425-3201: Add support for new DLNA PlaySpeed trickmode options in
*  playback ip library
* 
* 63   5/29/12 11:06a ssood
* SW7425-3122: add an option to disable nav file generation during media
*  probe time
* 
* 62   5/21/12 2:01p ssood
* SW7425-3042: suppress debug logging by default
* 
* 61   3/19/12 11:48a kagrawal
* SWANDROID-47: Android crashes in read() if O_DIRECT is set
* 
* 60   2/3/12 8:07p ssood
* SW7425-2331: disable autoRewind for HLS playlist files
* 
* 59   1/24/12 11:31a ssood
* SW7425-1331: insert encrypted (DTCP/IP) special data while streaming
*  out media files
* 
* 58   1/11/12 5:54p ssood
* SW7425-906: fix for guard violations during fast rewind trickplays
* 
* 57   1/4/12 3:17p ssood
* SW7231-561: increase timeout on stopping the streaming thread from
*  100msec to 1sec as under heavy load, streaming thread is not able to
*  quickly detect the stop condition
* 
* 56   10/12/11 5:03p ssood
* SW7346-341: fix cross-platform build related warnings
* 
* 55   10/7/11 6:24p ssood
* SW7346-341: free up pvr decryption related resources during session
*  stop
* 
* 54   9/29/11 4:51p ssood
* SW7425-1366: regenerate info files if media has has been modified
*  (recording & m3u8 case) + add inode # to the info file name to
*  distinguish two same media files from two different dir + fixed
*  resource leak
* 
* 53   9/26/11 10:22a ssood
* SW7346-341: Add PVR Decryption support for file streaming
* 
* 52   9/19/11 1:50p ssood
* SW7425-1040: added logic to better handle network congestion
* 
* 51   8/27/11 6:30p ssood
* SW7425-1077: throttle server streaming thread at lower speeds to avoid
*  starvation of local client trickplay at lower speed
* 
* 50   8/5/11 3:13p ssood
* SW7425-1040: add fast channel change support to IP Sessions
* 
* 49   6/21/11 9:59a ssood
* SW7425-751: fix warnings
* 
* 48   6/20/11 11:13p ssood
* SW7425-751: add configurable option for nav & nav files
* 
* 47   6/6/11 1:35a ssood
* SW7231-166: refine trickplay rates, account for timeSeekEnd from
*  ip_client
* 
* 46   5/19/11 4:35p ssood
* SW7420-1774: Add support to stream SVC/MVC videos
* 
* 45   1/26/11 7:35p ssood
* SW7425-59: remove debug prints
* 
* 44   1/26/11 7:13p ssood
* SW7425-59: correctly parse file names when it contains multiple . char
* 
* 43   1/20/11 3:54p ssood
* SW7425-59: support sending < Direct IO size
* 
* 42   1/4/11 12:14p dliu
* SW7420-1257: for RTP streaming change local port to port + 1
* 
* 41   1/2/11 5:26p ssood
* SW7420-1257: added support to stream encoded frame using unicast for
*  local client
* 
* 40   12/31/10 5:11p dliu
* SW7420-1257: Change to SD mode and fix bug when closing camera
* 
* 39   12/23/10 7:43p ssood
* SW7420-1257: work around to get trickmodes working w/ 7422 client
* 
* 38   12/23/10 5:23p ssood
* SW7420-1257: Set RTP Marker bit in last RTP packet belonging to an AV
*  frame to allow receiver distiniguish frame boudnaries for ES streams +
*  changes to include new camera APIs
* 
* 37   12/11/10 7:35a ssood
* SW7420-1257: increase the camera resolution to 1280x720 for HD video
*  conferencing session
* 
* 36   11/29/10 11:19a ssood
* SW7420-1257: fixed more compilation errors
* 
* 35   11/29/10 10:55a ssood
* SW7420-1257: fixed compilation errors due to structure name reuse
* 
* 34   11/24/10 2:58p ssood
* SW7420-1257: Video Conferencing Demo: add support for receiving input
*  camera input and stream it out
* 
* 33   11/23/10 11:55a ssood
* SW7420-1257: initial check-in for rtp/udp streaming support in
*  ip_streamer for Video Conferencing Demo
* 
* 32   9/27/10 3:39p junyel
* SW7420-1115: cross-platform code for DMS
* 
* 31   8/2/10 5:58p ssood
* SW7420-883: allow streaming to procceed on non-Direct IO capable File
*  systems
* 
* 30   7/30/10 3:23p ssood
* SW7420-883: add support to loop around files during trickplay if
*  IP_STREAMER_DEMO_MODE flag is set (default for standalone ip_streamer
*  app)
* 
* 29   7/23/10 11:49a ssood
* SW7420-502: updated to use the memory alloc/free routines from utils
* 
* 28   7/23/10 9:24a ssood
* SW7125-353: Coverity fixes + sw pacing code
* 
* 27   7/7/10 12:44p ismailk
* SW7420-842: Move the PlaybackIp_UtilsStreamingCtxOpen call to
*  B_PlaybackIp_FileStreamingStart.
* 
* 26   6/16/10 9:53p sgundime
* SW7420-689: reverse playback related changes for the byte range in time
*  seek.
* 
* 25   6/16/10 7:05p ssood
* SW7420-716: support for enabling DTCP/IP encryption during trickplay
* 
* 24   5/6/10 3:51p ssood
* SW7420-561: fixed regression in resuming from trick play: we were
*  restarting from begining
* 
* 23   4/20/10 6:08p sgundime
* SW7420-689: byteTostream is set to end-begin+1 , max end is set to
*  contentLength-1.
* 
* 22   4/16/10 5:30p ssood
* SW7125-297: Coverity fixes
* 
* 21   4/15/10 7:33p sgundime
* SW7420-689: Time seek range support is added.
* 
* 20   4/2/10 4:25p ssood
* SW7420-561: Fixed Coverity issues
* 
* 19   3/10/10 12:39p ssood
* SW7420-561: removed nexus dependency for simple file streaming (no
*  DTCP/IP or Live streaming support)
* 
* 18   1/24/10 8:56a ssood
* SW7420-454: probe is now determing frame rate from sequence header for
*  MPEG2 videos
* 
* 17   1/23/10 11:16a ssood
* SW7420-454: added support to extract 1st PTS from TS files, store in
*  info file & send to clients in HTTP resp
* 
* 16   1/19/10 4:55p ssood
* SW7420-454: convert verbose output to be available via msg modules
*  flags
* 
* 15   1/18/10 5:00p ssood
* SW7420-454: Add support to create index w/o using the createindex
*  utility
* 
* 14   1/13/10 2:51p ssood
* SW7420-454: expose additional audio & video related PSI info
* 
* 13   1/6/10 1:59p ssood
* SW7420-454: modified the server side trickmode parameters to suit the
*  new streams from Picture Quality Group
* 
* 12   1/2/10 12:04a ssood
* SW7420-454: file read offset is not being reset to begining after media
*  probe failure
* 
* 11   12/30/09 12:15a ssood
* SW7420-454: re-create info file if previously created one is incomplete
* 
* 10   12/22/09 11:03a ssood
* SW7420-454: add timeshift support for live streaming
* 
* 9   12/17/09 2:07p ssood
* SW7420-454: Add option to just open IP Streamer & IP library contexts
*  for Media Probe purposes
* 
* 8   12/16/09 2:41p ssood
* SW7420-454: create index file if doesn't exist (needs improvements)
* 
* 7   12/14/09 1:33p ssood
* SW7420-454: fix coverity reported issues
* 
* 6   12/9/09 11:00a ssood
* SW7420-454: fix merge issue: generic media playback was broken
* 
* 5   12/8/09 2:12p ssood
* SW7420-454: hardcode the minIFrame speed until player library is
*  enhanced to provide the frame type info
* 
* 4   12/8/09 1:46p ssood
* SW7420-454: Add initial support for server based trick modes (not fully
*  DLNA compliant as DLNA requires server to send MPEG compliant streams:
*  correct frame rate & w/ timing information)
* 
* 3   12/3/09 2:35p ssood
* SW7420-454: Add support to stream any generic media formats
* 
* 2   11/17/09 4:12p ssood
* SW7420-454: compile out ASF related media probe logic for now
* 
* 1   11/17/09 3:49p ssood
* SW7420-454: Enhance IP library to stream files from local disk
* 
***************************************************************************/
#if defined(LINUX) || defined(__vxworks)
#if defined(LINUX)
#include <sys/syscall.h>
#ifdef __mips__
#ifndef DMS_CROSS_PLATFORMS
#include <asm/cachectl.h>
#endif /* DMS_CROSS_PLATFORMS */
#endif /* __mips__ */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <math.h>

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"
#include "b_playback_ip_nav_indexer.h"

#include "bmedia_probe.h"
#include "bmpeg_video_probe.h"
#include "bmpeg2ts_probe.h"
#if B_HAS_ASF
#include "basf_probe.h"
#endif
#include "bfile_stdio.h"

#if defined(HUMAX_PLATFORM_BASE)
#ifdef B_HAS_VIDEO_CONFERENCING_SUPPORT
#include "video_capture.h"
#endif
#include "bcmindexer_nav.h"
#endif
#endif

BDBG_MODULE(b_playback_ip_file_streaming);

#define SEARCH_NUM_GOPS 10
#define SEARCH_MAX_FRAMES_PER_GOP 300
/* This value is the multipler used to achieve a desired playspeed when combined with iFrameModifier (how many I-Frames to advance the bcm player) */
/* speed = FRAME_RATE_MULTIPLIER * IFrameModifier */
/* we fix the FRAME_RATE_MULTIPLIER to 6. That means if we play all i-frames (iFrameModifier ==1), then we will get 6x speed */
/* 6 is chosen with following in mind: since I-frames are much larger in size, restricting this number will control the n/w b/w */
/* otherwise, if client's frame rate in trickmode is set higher, then it will consume more i-frames leading to higher n/w b/w utilization */
#define FRAME_RATE_MULTIPLIER 6         
static const struct bfile_io_write net_io_data_write = {
    B_PlaybackIp_UtilsStreamingCtxWrite,
    NULL,
    BIO_DEFAULT_PRIORITY
};

typedef struct B_PlaybackIpIndexer {
    void *context;  /* returned by the nav player */
    int framesPerGop;
    double framesPerSec;
    double frameTime;
    double minISpeed;
    double bitRate;
    int    frameRepeat;
    int    msPerGop;
    int    tmModifier;
    /* TODO: take these out */
    unsigned long first_pts;
    unsigned long duration;
} B_PlaybackIpIndexer; 

/* # of io vectors for the sendmsg() call, currently set to 2 to avoid IP fragmentation */
/* should be increased when our NIC starts supporting Segmentation Offload technology to allow single sendmsg call */
typedef struct B_PlaybackIpFileStreaming 
{
    B_PlaybackIpFileStreamingOpenSettings settings;
    int fd;
    FILE *fp;
    int indexFileFd;
    FILE *indexFileFp;
    struct bfile_io_write_net data;
    struct bfile_io_write_net dataLocalStreaming;
    B_ThreadHandle streamingThread;
    bool stop;  /* set when app wants to stop the streaming session */
    bool threadRunning; /* set when file streaming thread is running */
    BKNI_EventHandle stopStreamingEvent;
    off_t totalBytesStreamed;
    B_PlaybackIpConnectionState connectionState;    /* state of the socket: active, timeouts, error, eof */
    B_PlaybackIpIndexer indexer;
    B_PlaybackIpPsiInfo psi;
    unsigned char *streamingBuf;
    unsigned char *streamingBufOrig;
    unsigned streamingBufSize;
    unsigned char *pkt; /* buffer for storing BTPs */
    unsigned char *pktOrig; /* original buffer for storing BTPs */
    BKNI_EventHandle bufferAvailableEvent;
} B_PlaybackIpFileStreaming;

/***************************************************************************
Summary:
This function stops streaming content from a file.
***************************************************************************/
void 
B_PlaybackIp_FileStreamingClose(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle
    )
{
    BDBG_MSG(("%s: %p", __FUNCTION__, fileStreamingHandle));

    if (fileStreamingHandle) {
        if (fileStreamingHandle->fp) {
            fclose(fileStreamingHandle->fp);
        }
        if (fileStreamingHandle->fd >= 0) {
            close(fileStreamingHandle->fd);
        }
        if (fileStreamingHandle->indexFileFp) {
            if (fileStreamingHandle->indexer.context)
                nav_indexer_close(fileStreamingHandle->indexer.context);
            fclose(fileStreamingHandle->indexFileFp);
            close(fileStreamingHandle->indexFileFd);
        }

		if (fileStreamingHandle->bufferAvailableEvent)
            BKNI_DestroyEvent(fileStreamingHandle->bufferAvailableEvent);
        BKNI_Free(fileStreamingHandle);
    }
    return;
}

void B_PlaybackIp_FileStreaming_WriteCompleteCallback(void* context)
{
    BKNI_SetEvent((BKNI_EventHandle)context);
}

B_PlaybackIpFileStreamingHandle 
B_PlaybackIp_FileStreamingOpen(
    const B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings
    )
{
    B_PlaybackIpFileStreamingHandle fileStreamingHandle = NULL;

    BDBG_MSG(("%s:\n", __FUNCTION__));
    if (!fileStreamingSettings) {
        BDBG_ERR(("%s: Invalid param: Need to pass-in file open Settings\n", __FUNCTION__));
        goto error;
    }

    fileStreamingHandle = BKNI_Malloc(sizeof(B_PlaybackIpFileStreaming));
    if (!fileStreamingHandle) {
        BDBG_ERR(("%s: memory allocation failure\n", __FUNCTION__));
        goto error;
    }
    memset(fileStreamingHandle, 0, sizeof(B_PlaybackIpFileStreaming));

    fileStreamingHandle->settings = *fileStreamingSettings;
    if (fileStreamingHandle->settings.playSpeed == 0)
        fileStreamingHandle->settings.playSpeed = 1;

    fileStreamingHandle->fd = open(fileStreamingSettings->fileName, O_RDONLY | O_LARGEFILE);
    if (fileStreamingHandle->fd >= 0) 
        fileStreamingHandle->fp = fdopen(fileStreamingHandle->fd, "r");
    if (fileStreamingHandle->fd < 0 || !fileStreamingHandle->fp) {
        BDBG_ERR(("%s: failed to open file (%s), fd %d, fp %p, errno %d\n", __FUNCTION__, fileStreamingSettings->fileName, fileStreamingHandle->fd, fileStreamingHandle->fp, errno));
        goto error;
    }

    if (fileStreamingSettings->mediaProbeOnly) {
        BDBG_MSG(("Skipping streaming session setup as mediaProbeOnly flag is set"));
        goto out;
    }
    BDBG_MSG(("%s: open file (%s), fd %d, fp %p, protocol %d", __FUNCTION__, fileStreamingSettings->fileName, fileStreamingHandle->fd, fileStreamingHandle->fp, fileStreamingHandle->settings.protocol));

out:
    fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eSetup;
    return fileStreamingHandle;

error:
    B_PlaybackIp_FileStreamingClose(fileStreamingHandle); 
    return NULL;
}

void
preparePlaySpeedList(
    int minISpeed,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    /* these speeds are chosen based on how we can achieve them */
    psi->playSpeed[0] = -16*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[1] = -8*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[2] = -4*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[3] = -2*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[4] = -1*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[5] = 1*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[6] = 2*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[7] = 4*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[8] = 8*FRAME_RATE_MULTIPLIER;
    psi->playSpeed[9] = 16*FRAME_RATE_MULTIPLIER;
    psi->numPlaySpeedEntries = 10;

    /* TODO: this is being deprecated and will be removed once new server side trickmode approach gets fully tested */
    psi->httpMinIFrameSpeed = minISpeed;
}

static bool 
getNextNameValuePair(FILE *fp, char *buf, char **name, char **value)
{
    char *tmp_buf = NULL;
    
    /* each line in info file is of name=value format */
    if (!fgets(buf, 128, fp))
        goto error;
    *name = buf;
    
    if ((tmp_buf = strstr(buf, "=")) == NULL)
        goto error;
    *tmp_buf = '\0';
    
    *value = tmp_buf + 1;
    BDBG_MSG(("%s: name %s, value %s", __FUNCTION__, *name, *value));
    return true;
error:
    return false;
}

static bool
parseMediaInfo(
    char *fileName,
    FILE *fp,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    char buf[128];
    char *name, *value;
    bool foundVideo = false, foundAudio = false;

    while (getNextNameValuePair(fp, buf, &name, &value) == true) {
        if (strncasecmp(name, "containerType", 128) == 0) {
            psi->mpegType = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "duration", 128) == 0) {
            psi->duration = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "bitRate", 128) == 0) {
            psi->avgBitRate = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "vidPid", 128) == 0) {
            psi->videoPid = strtol(value, NULL, 10); 
            foundVideo = true;
        }
        else if (strncasecmp(name, "videoCodec", 128) == 0) {
            psi->videoCodec = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "extraVideoCodec", 128) == 0) {
            psi->extraVideoCodec = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "extraVidPid", 128) == 0) {
            psi->extraVideoPid = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "videoWidth", 128) == 0) {
            psi->videoWidth = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "videoHeight", 128) == 0) {
            psi->videoHeight = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "videoBitrate", 128) == 0) {
            psi->videoBitrate = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "videoFrameRate", 128) == 0) {
            psi->videoFrameRate = strtof(value, NULL); 
        }
        else if (strncasecmp(name, "audPid", 128) == 0) {
            psi->audioPid = strtol(value, NULL, 10); 
            foundAudio = true;
        }
        else if (strncasecmp(name, "audioCodec", 128) == 0) {
            psi->audioCodec = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "audioBitsPerSample", 128) == 0) {
            psi->audioBitsPerSample = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "audioSampleRate", 128) == 0) {
            psi->audioSampleRate = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "audioNumChannels", 128) == 0) {
            psi->audioNumChannels = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "audioBitRate", 128) == 0) {
            psi->audioBitrate = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "pcrPid", 128) == 0) {
            psi->pcrPid = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "tts", 128) == 0) {
            psi->transportTimeStampEnabled = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "contentLength", 128) == 0) {
            psi->contentLength = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "numPlaySpeed", 128) == 0) {
            psi->numPlaySpeedEntries = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "minISpeed", 128) == 0) {
            psi->httpMinIFrameSpeed = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "frameRepeat", 128) == 0) {
            psi->httpFrameRepeat = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "frameRateInTrickMode", 128) == 0) {
            psi->frameRateInTrickMode = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "firstPts", 128) == 0) {
            psi->firstPts = strtol(value, NULL, 10); 
        }
        else if (strncasecmp(name, "psiValid", 128) == 0) {
            psi->psiValid = strtol(value, NULL, 10); 
        }
    }

    /* now get the playspeed entries */
    if (psi->numPlaySpeedEntries)
        preparePlaySpeedList(psi->httpMinIFrameSpeed, psi);

    BDBG_MSG(("Successfully read info file for media name %s\n", fileName));
    return true;
}

void
replaceFileExtension(char *newName, int newNameSize, char *curName, char *newExtension)
{
    char *tmpPtr, *curPtr = NULL, *prevPtr = NULL;

    strncpy(newName, curName, newNameSize);
    curPtr = newName;
    while (true) {
        tmpPtr = strstr(curPtr, ".");
        if (tmpPtr) {
            curPtr = tmpPtr+1; /* point to location of current . and search for next one */
            prevPtr = tmpPtr;   /* location of current . */
            continue;
        }
        else {
            /* no more . found, break */
            break;
        }
    }
    if (prevPtr)
        *prevPtr = '\0';   /* overwrite the existing . char in the file name for following strcat to work */
    strncat(newName, newExtension, newNameSize);
}

/* build info file as follows: */
/* <infoFilesDir>/<mediaFileName>.info */
char *
buildInfoFileName(char *infoFilesDir, char *mediaFileName)
{
    int infoFileNameSize, len;
    char *infoFileName, *mediaRelativeFileNamePtr, *tmp;
    struct stat st;
    char *curPtr, *tmpPtr, *prevPtr = NULL;

    /* extract the actual file name by removing the directory path prefix */
    mediaRelativeFileNamePtr = mediaFileName;
    tmp = mediaFileName;
    while ((tmp = strstr(tmp, "/")) != NULL) {
        tmp += 1; /* move past "/" char */
        mediaRelativeFileNamePtr = tmp;
    }
    /* now mediaRelativeFileNamePtr points to the relative media file name */

    if (stat(mediaFileName, &st) < 0 ) {
        BDBG_ERR(("%s: stat() failed on the media file %s, errno %d", __FUNCTION__, mediaFileName, errno));
        perror("stat");
        return NULL;
    }
    infoFileNameSize = strlen(infoFilesDir) + 33 + strlen(mediaRelativeFileNamePtr) + 10; /* 32 bytes for i-node number, 10 extra bytes for storing optional extensions */
    infoFileName = (char *)BKNI_Malloc(infoFileNameSize+1);
    if (!infoFileName) {
        BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
        return NULL;
    }
    memset(infoFileName, 0, infoFileNameSize);
    /* concatenate the info dir path to the info file name */
    len = snprintf(infoFileName, infoFileNameSize-1, "%s/%s", infoFilesDir, mediaRelativeFileNamePtr);
    /* now insert the inode number into the info file name so that two files with same name residing in two different directories can still have the same info file when info files are kept in one directory */
    curPtr = infoFileName;
    while (true) {
        tmpPtr = strstr(curPtr, ".");
        if (tmpPtr) {
            curPtr = tmpPtr+1; /* point to location of current . and search for next . if present */
            prevPtr = tmpPtr;   /* location of current . */
            continue;
        }
        else 
            /* no more . found, break */
            break;
    }
    if (!prevPtr) {
        /* media file name didn't have any . in it, so append info extension towards its end */
        prevPtr = infoFileName + len;
    }
    len = prevPtr - infoFileName;
    snprintf(prevPtr, infoFileNameSize-1-len, "_%d%s", (int) st.st_size, B_PLAYBACK_IP_INFO_FILE_EXTENSION);
    BDBG_MSG(("info file name %s", infoFileName));
    return infoFileName;
}

int 
getStreamGopInfo(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    B_PlaybackIpIndexer *idx = &fileStreamingHandle->indexer;
    int iFrameRateRounded; /* number of i-frames in a sec: usually 1 or 2 */
    float iFrameRate; /* number of i-frames in a sec: usually 1 or 2 */

    struct index_entry_t entry1;
    struct index_entry_t *entry = &entry1;
    int count[3];
    int j,k;
    unsigned long pcr1, pcr2;
    ssize_t  start, end;
    int totalFrames=0;
    int minISpeed;

    if (idx->context == NULL)
        /* index doesn't exist for this format, shouldn't be called */
        return -1;

    if (nav_indexer_seek(idx->context, 0, &idx->duration, &idx->first_pts)) {
        BDBG_WRN(("%s: failed to seek to start of file\n", __FUNCTION__));
        return -1;
    }

    /* skip initial few I-frames */
    for (k=0;k<3;k++) {
        entry->type = 0;
        for (j=0;j<SEARCH_MAX_FRAMES_PER_GOP ;j++) {
            nav_indexer_next(idx->context, entry);
            if (entry->type == eSCTypeIFrame) {
                BDBG_MSG(("%s: found %d frames in a GOP, size %lu", __FUNCTION__, j, entry->size));
                break;
            }
        }
    }
    
    /* Now we have I-frame */
    pcr1 = entry->pcr;
    
    count[0] = 0;
    count[1] = 0;
    count[2] = 0;
    
    start = entry->offset;
    
    /* count I, B, and P frames in 1st SEARCH_NUM_GOPS */
    for (k=0; k<SEARCH_NUM_GOPS; k++) {
        entry->type = 0;
        for (j=0; j<SEARCH_MAX_FRAMES_PER_GOP; j++) {
            if (nav_indexer_next(idx->context, entry)) {
                break;
            }
            if (entry->type == eSCTypePFrame) 
                count[1]++;
            else if (entry->type == eSCTypeBFrame) 
                count[2]++;
            else if (entry->type == eSCTypeIFrame) {
                count[0]++;
                BDBG_MSG(("%s: found %d frames in a GOP, frame size %lu", __FUNCTION__, j, entry->size)); 
                break;
            }
        }
    }
    
    pcr2 = entry->pcr;
    end =  entry->offset;
    totalFrames = count[0]+count[1]+count[2];
    
    idx->framesPerGop = totalFrames / SEARCH_NUM_GOPS;
    idx->framesPerSec = totalFrames / ((pcr2 - pcr1 + .001)/45000.);
    idx->bitRate = 8 * (end - start) / ((pcr2 - pcr1 + .001)/45000.);
    idx->msPerGop = (1000.*idx->framesPerGop/idx->framesPerSec);
    iFrameRate = count[0] / ((pcr2 - pcr1 + .001)/45000.);
    if (iFrameRate > 1.0 && iFrameRate < 1.5)
        /* special case to handle cases where frameRate is just about 1, but it is really 1 i-frame per sec */
        iFrameRateRounded = 1;
    else
        iFrameRateRounded = (int) ceil(iFrameRate);
    
    /* TODO: deprecate following code once new playSpeed changes are fully tested */
    if (psi->videoCodec == NEXUS_VideoCodec_eH264) {
        if (fileStreamingHandle->settings.playSpeed == 2)
            idx->frameRepeat = 1;
        else
            idx->frameRepeat = 4;
        idx->minISpeed = idx->msPerGop/ AVC_FRAME_TIME;
        idx->minISpeed = 1;
    }
    else {
        if (fileStreamingHandle->settings.playSpeed == -1)
            idx->frameRepeat = 8;
        else if (fileStreamingHandle->settings.playSpeed == -2)
            idx->frameRepeat = 6;
        else if (fileStreamingHandle->settings.playSpeed == -4)
            idx->frameRepeat = 6;
        else
            idx->frameRepeat = 2;
        idx->minISpeed = 16;
    }
    psi->httpFrameRepeat = idx->frameRepeat;
    minISpeed = ceil(idx->minISpeed);
    /* end deprecated code */
    
    BDBG_WRN(("StreamInfo: iFrameRate %f, %d, BitRate=%lf(mbps) FramesPerSec=%lf FramesPerGoP=%d, msPerGOP=%d frameRepeat=%d minISpeed=%lf, # of I=%d P=%d B=%d, file %s", 
        iFrameRate, iFrameRateRounded, idx->bitRate, idx->framesPerSec, idx->framesPerGop,idx->msPerGop,idx->frameRepeat,idx->minISpeed, count[0],count[1],count[2], fileStreamingHandle->settings.fileName));
    preparePlaySpeedList(minISpeed, psi);
    psi->frameRateInTrickMode = FRAME_RATE_MULTIPLIER * iFrameRateRounded;
    return true;
}

bool
setIndexerMode(B_PlaybackIpIndexer *idx, int speed, B_PlaybackIpPsiInfoHandle psi)
{
    trick_mode_t tm;

    tm.speed = abs(speed);
    if (speed > 0)
        tm.direction = 1; /* forward */
    else
        tm.direction = -1; /* backward */
    tm.videoType = psi->videoCodec;
    tm.modifier = tm.speed / FRAME_RATE_MULTIPLIER;
    if (nav_indexer_mode(idx->context, &tm) == false) {
        return false;
    }
    return true;
}

bool
setupMediaIndexerTrickMode(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    int playSpeed,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    B_PlaybackIpIndexer *indexer = &fileStreamingHandle->indexer;
    B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings;

    BDBG_MSG(("%s: file streaming handle %p, playSpeed %d ", __FUNCTION__, fileStreamingHandle, playSpeed));

    if (setIndexerMode(indexer, playSpeed, psi) == false) {
        BDBG_ERR(("%s: Failed to set the indexer mode for file session %p\n", __FUNCTION__, fileStreamingHandle));
        return false;
    }
    fileStreamingSettings = &fileStreamingHandle->settings;
    if (fileStreamingSettings->beginTimeOffset || fileStreamingSettings->endTimeOffset) {
        nav_indexer_setIndexByTimeOffset(indexer->context, fileStreamingSettings->beginTimeOffset);
    }
    else if (fileStreamingSettings->beginFileOffset) {
        nav_indexer_setIndexByByteOffset(indexer->context, fileStreamingSettings->beginFileOffset);
    }
    return true;
}

bool
openMediaIndexer(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    char *indexFileName,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    bool rc = false;
    int fd = -1;
    FILE *fp = NULL;
    struct stat fileStats;
    int numRetries = 0;

    if (!psi)
        return false;

    BDBG_MSG(("%s: for index file %s", __FUNCTION__, indexFileName));
retry:
    fileStats.st_size = 0;
    if ( ((fd = open(indexFileName, O_RDONLY)) < 0) || ((fstat(fd, &fileStats) == 0) && fileStats.st_size <= 0) || ((fp = fdopen(fd, "r")) == NULL) ) {
        if (nav_indexer_create(fileStreamingHandle->settings.fileName, indexFileName, psi) != 0) {
            fileStreamingHandle->indexer.context = NULL;
            BDBG_WRN(("Failed to create the index file %s for media %s", indexFileName, fileStreamingHandle->settings.fileName));
            rc = true;    /* note: we treat this as a soft error as we can still stream the file, but can't do server side trickmodes */
            goto error;
        }
        if (numRetries++ > 1) {
            BDBG_WRN(("Failed to create the index file %s for media %s, continue w/o it", indexFileName, fileStreamingHandle->settings.fileName));
            rc = true;
            goto error;
        }
        else
            goto retry;
    }

    /* index file exists, open it */
    if (nav_indexer_open(&fileStreamingHandle->indexer.context, fp, psi) < 0) {
        BDBG_ERR(("%s: Failed to open the index for file %s", __FUNCTION__, indexFileName));
        rc = true;
        goto error;
    }

    fileStreamingHandle->indexFileFd = fd;
    fileStreamingHandle->indexFileFp = fp;
    return true;

error:
    if (fd >= 0)
        close(fd);
    if (fp != NULL)
        fclose(fp);
    return false;
}

int 
createInfoFile(
    char *fileName, 
    char *infoFileName, 
    B_PlaybackIpPsiInfoHandle psi
    )
{
    FILE *fp;

    /* Now write the PSI info into the media info file */
    if ((fp = fopen(infoFileName, "w")) == NULL) {
        BDBG_ERR(("Failed to obtain fp for Info file (%s) errno %d\n", infoFileName, errno));
        return -1;
    }
    fprintf(fp,
            "mediaFile=%s\n"
            "contentLength=%lld\n"
            ,
            fileName, 
            psi->contentLength
            );
    if (psi->psiValid) {
        fprintf(fp,
            "containerType=%d\n"
            "duration=%d\n"
            "bitRate=%d\n"
            "vidPid=%d\n"
            "videoCodec=%d\n"
            "videoWidth=%d\n"
            "videoHeight=%d\n"
            "videoBitrate=%d\n"
            "videoFrameRate=%.3f\n"
            "audPid=%d\n"
            "audioCodec=%d\n"
            "audioBitsPerSample=%d\n"
            "audioSampleRate=%d\n"
            "audioNumChannels=%d\n"
            "audioBitRate=%d\n"
            "pcrPid=%d\n"
            "tts=%d\n"
            ,
            psi->mpegType, 
            psi->duration, 
            psi->avgBitRate,
            psi->videoPid, 
            psi->videoCodec, 
            psi->videoWidth,
            psi->videoHeight,
            psi->videoBitrate,
            psi->videoFrameRate,
            psi->audioPid, 
            psi->audioCodec, 
            psi->audioBitsPerSample,
            psi->audioSampleRate,
            psi->audioNumChannels,
            psi->audioBitrate,
            psi->pcrPid, 
            psi->transportTimeStampEnabled
            );

        if (psi->extraVideoCodec != NEXUS_VideoCodec_eNone && psi->extraVideoPid != 0) {
            fprintf(fp, "extraVidPid=%d\n" "extraVideoCodec=%d\n", psi->extraVideoPid, psi->extraVideoCodec);
        }

        if (psi->numPlaySpeedEntries) {
            fprintf(fp,
                "numPlaySpeed=%d\n"
                "minISpeed=%d\n"
                "frameRepeat=%d\n"
                "frameRateInTrickMode=%d\n"
                "firstPts=%d\n"
                ,
                psi->numPlaySpeedEntries,
                psi->httpMinIFrameSpeed,
                psi->httpFrameRepeat,
                psi->frameRateInTrickMode,
                psi->firstPts
                );
        }
    }
    fprintf(fp, "psiValid=%d\n", psi->psiValid);
    fflush(fp);
    fclose(fp);
    return 0;
}

bool
getMediaInfo(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    char *infoFileName,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    int fdesc;
    bmedia_probe_t probe = NULL;
    bmedia_probe_config probe_config;
    const bmedia_probe_stream *stream = NULL;
    const bmedia_probe_track *track = NULL;
    bfile_io_read_t fd = NULL;
    bool foundAudio = false, foundVideo = false;
    char stream_info[512];
    FILE *fp = NULL;
    struct stat fileStats;
    struct stat infoFileStats;
    B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings;
    int infoFileNameLength;
    char *indexFileName;
    bool rc = false;

    fileStreamingSettings = &fileStreamingHandle->settings;
    if (fstat(fileStreamingHandle->fd, &fileStats) || fileStats.st_size <= 0) {
        BDBG_ERR(("Can't obtain file stats info on media file (%s) %d\n", fileStreamingSettings->fileName, errno));
        return false;
    }

    /* see if a valid info file already exists */
    if ((fdesc = open(infoFileName, O_RDONLY)) >= 0) {
        /* info file exists, check if it valid and media file is not newer than its info file (true for timeshifting content) */
        if ( (fstat(fdesc, &infoFileStats) == 0) && infoFileStats.st_size > 0 && infoFileStats.st_mtime > fileStats.st_mtime ) {
            /* file has some size, populate PSI structure from the info file */
            fp = fdopen(fdesc, "r");
            if (fp && parseMediaInfo(fileStreamingSettings->fileName, fp, psi)) {
                fclose(fp);
                close(fdesc);
                /* success in reading the info file */
                if (psi->psiValid == 0) {
                    /* but info file doesn't contains valid PSI info */
                    BDBG_MSG(("%s: current PSI info not valid, try to create info file for file (%s) \n", __FUNCTION__, fileStreamingSettings->fileName));
                    goto probeMedia;
                }
                /* open index file */
                if (psi->numPlaySpeedEntries) {
                    /* play speed entries only get set if a media format has separate index file and thus we can support server side trickmodes */
                    infoFileNameLength = strlen(infoFileName);
                    indexFileName = infoFileName;
                    replaceFileExtension(indexFileName, infoFileNameLength, infoFileName, B_PLAYBACK_IP_INDEX_FILE_EXTENSION);
                    if (openMediaIndexer(fileStreamingHandle, indexFileName, psi) == false) {
                        BDBG_WRN(("%s: Failed to open/obtain Index File for file (%s) \n", __FUNCTION__, fileStreamingSettings->fileName));
                        goto error;
                    }
                    if (psi->videoCodec == NEXUS_VideoCodec_eH264) {
                        if (fileStreamingHandle->settings.playSpeed == 2)
                            psi->httpFrameRepeat = 1;
                        else
                            psi->httpFrameRepeat = 4;
                    }
                    else {
                        if (fileStreamingHandle->settings.playSpeed == -1)
                            psi->httpFrameRepeat = 8;
                        else if (fileStreamingHandle->settings.playSpeed == -2)
                            psi->httpFrameRepeat = 6;
                        else if (fileStreamingHandle->settings.playSpeed == -4)
                            psi->httpFrameRepeat = 6;
                        else
                            psi->httpFrameRepeat = 2;
                    }
                }
                return true;
            }
			if (fp) fclose(fp);
        }
		close(fdesc);
    }

probeMedia:
    if ((probe = bmedia_probe_create()) == NULL) {
        BDBG_ERR(("Can't create a Media probe object for parsing file %s \n", fileStreamingSettings->fileName));
        rc = false;
        goto error;
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
	VFIO_Handle_t FileHandle;
	FileHandle.eType = VFIO_TYPE_LOCAL;
	FileHandle.ucAvailable = true;
	FileHandle.tFile.local.fin = fileStreamingHandle->fp;
	fd = bfile_stdio_read_attach(FileHandle);
#else
    fd = bfile_stdio_read_attach(fileStreamingHandle->fp);
#endif

    bmedia_probe_default_cfg(&probe_config);
    probe_config.file_name = fileStreamingSettings->fileName;
    probe_config.type = bstream_mpeg_type_unknown;
    stream = bmedia_probe_parse(probe, fd, &probe_config);

    /* now stream is either NULL, or stream descriptor with linked list of audio/video tracks */
    if (!stream) {
        BDBG_ERR(("Media probe can't parse stream '%s'", fileStreamingSettings->fileName));
        rc = false;
        psi->psiValid = false;
        goto writeInfoFile;
    }

#if 0
    /* TODO: this can be used to determine if the media format contains index info */
    if (stream->index == bmedia_probe_index_available || stream->index == bmedia_probe_index_required)
#endif
    bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
    BDBG_MSG(( "Media Probe for %s:\n" "%s\n\n", fileStreamingSettings->fileName, stream_info));

    psi->mpegType = B_PlaybackIp_UtilsMpegtype2ToNexus(stream->type);
    psi->duration = stream->duration;
    psi->avgBitRate = stream->max_bitrate;
    psi->psiValid = true;
    if (psi->mpegType == NEXUS_TransportType_eTs && ((((bmpeg2ts_probe_stream*)stream)->pkt_len) == 192)) {
        BDBG_MSG(("%s: TTS Stream\n", __FUNCTION__));
        psi->transportTimeStampEnabled = true;
    }
    else {
        psi->transportTimeStampEnabled = false;
    }

    for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
        switch(track->type) {
        case bmedia_track_type_audio:
            if(track->info.audio.codec != baudio_format_unknown && !foundAudio) {
                psi->audioPid = track->number;
                psi->audioCodec = B_PlaybackIp_UtilsAudioCodec2Nexus(track->info.audio.codec);
                psi->audioBitsPerSample = track->info.audio.sample_size;
                psi->audioSampleRate = track->info.audio.sample_rate;
                psi->audioNumChannels = track->info.audio.channel_count;
                psi->audioBitrate = 1000*track->info.audio.bitrate; /* probe returns in Kbps, convert to bps */
                foundAudio = true;
            }
            break;
            case bmedia_track_type_video:
            if (track->info.video.codec == bvideo_codec_h264_svc || track->info.video.codec == bvideo_codec_h264_mvc) {
                psi->extraVideoPid = track->number;
                psi->extraVideoCodec = B_PlaybackIp_UtilsVideoCodec2Nexus(track->info.video.codec);
            }
            else if (track->info.video.codec != bvideo_codec_unknown && !foundVideo) {
                char string[16];
                psi->videoPid = track->number;
                psi->videoCodec = B_PlaybackIp_UtilsVideoCodec2Nexus(track->info.video.codec);
                psi->videoWidth = track->info.video.width;
                psi->videoHeight = track->info.video.height;
                psi->videoBitrate = 1000*track->info.video.bitrate; /* probe returns in Kbps, convert to bps */
                if (psi->videoCodec == NEXUS_VideoCodec_eMpeg2) {
                    snprintf(string, sizeof(string)-1, "%d.%d",     /* media probe returns in 1000 FPS units, convert to string & then into float */
                        ((bmedia_probe_mpeg_video*)&track->info.video.codec_specific)->framerate/1000, 
                        ((bmedia_probe_mpeg_video*)&track->info.video.codec_specific)->framerate%1000);
                    psi->videoFrameRate = strtof(string, NULL);
                    if (psi->videoFrameRate <= 0)
                        psi->videoFrameRate = 30.;
                }

                foundVideo = true;

#if 0
                /* TODO: may need to extract extra media related info */
#if B_HAS_ASF
                if (stream->type == bstream_mpeg_type_asf) {
                    basf_probe_track *asf_track = (basf_probe_track *)track;
                    if (asf_track->aspectRatioValid) {
                        psi->aspectRatio = NEXUS_AspectRatio_eSar;
                        psi->sampleAspectRatio.x = asf_track->aspectRatio.x;
                        psi->sampleAspectRatio.y = asf_track->aspectRatio.y;
                    }
                }
#endif
#endif
            }
            break;
            case bmedia_track_type_pcr:
                psi->pcrPid = track->number;
            default:
            break;
        }
    }
    BDBG_MSG(("Media Details: container type %d, index %d, avg bitrate %d, duration %d, # of programs %d, # of tracks %d\n", 
            psi->mpegType, stream->index, psi->avgBitRate, psi->duration, stream->nprograms, stream->ntracks));
    if (!foundVideo && !foundAudio) {
        BDBG_ERR(("Media probe didn't find any audio or video tracks for media '%s'", fileStreamingSettings->fileName));
        rc = false;
        goto error;
    }

    /* open media index */
    if (!fileStreamingSettings->disableIndexGeneration && psi->mpegType == NEXUS_TransportType_eTs 
#if 0
        /* TODO: server side trickmodes didn't seem to work for Mpeg2PES files, need to look into it later */
        || psi->mpegType == NEXUS_TransportType_eMpeg2Pes
#endif
       ) {
        /* NAV index is only being used for TS files */
        /* For ASF, MP4, index is already part of the container format */
        /* TODO: include PES & VOBs at some point */
        BDBG_MSG(("%s: NAV index is only being used for TS formats (current file format %d)", __FUNCTION__, psi->mpegType));

        infoFileNameLength = strlen(infoFileName);
        indexFileName = infoFileName;
        replaceFileExtension(indexFileName, infoFileNameLength, infoFileName, B_PLAYBACK_IP_INDEX_FILE_EXTENSION);
        if (openMediaIndexer(fileStreamingHandle, indexFileName, psi) == false) {
            BDBG_WRN(("%s: Failed to create/obtain Index File for file (%s) \n", __FUNCTION__, fileStreamingSettings->fileName));
            goto error;
        }
        /* now acquire GOP related info */
        getStreamGopInfo(fileStreamingHandle, psi);

        replaceFileExtension(infoFileName, infoFileNameLength, indexFileName, B_PLAYBACK_IP_INFO_FILE_EXTENSION);
    }

writeInfoFile:
    psi->contentLength = fileStats.st_size;

    if (createInfoFile(fileStreamingSettings->fileName, infoFileName, psi)) {
        BDBG_ERR(("%s: Failed to create info file %s", __FUNCTION__, fileStreamingSettings->fileName));
        rc = false;
        goto error;
    }
    rc = true;
    BDBG_MSG(("%s: Media info file %s created", __FUNCTION__, infoFileName));
error:
    if (probe) {
        if (stream) bmedia_probe_stream_free(probe, stream);
        if (fd) bfile_stdio_read_detach(fd);
        bmedia_probe_destroy(probe);
    }
    return rc;
}

bool
getReadOffset(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,    
    int *dioBeginOffsetAdjustment,
    int *dioEndOffsetAdjustment,
    off_t *dioBeginOffset
    )
{
    int rc;        
    struct index_entry_t entry;

    while (true) {
        memset(&entry, 0, sizeof(entry));
        if ( (rc = nav_indexer_next(fileStreamingHandle->indexer.context, &entry)) ) {
            BDBG_WRN(("%s: nav_indexer_next returned %d, error or EOF", __FUNCTION__, rc));         
            return false;
        }
        if (entry.insertPkt) {
            /* special pkt is sent, so goto to next entry */
            continue;
        }
        else {          
            /* make the entry dio compatible */
            *dioBeginOffset = entry.offset & ~DIO_MASK;         
            *dioBeginOffsetAdjustment = entry.offset - *dioBeginOffset;
            *dioEndOffsetAdjustment = DIO_BLK_SIZE - ( (entry.size+*dioBeginOffsetAdjustment) % DIO_BLK_SIZE);          
            return true;
        }
    }
    return false;
}

bool
getByteRangeFromTimeRange(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle    
    )
{
    B_PlaybackIpIndexer *indexer = &fileStreamingHandle->indexer;
    B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings;
    int dioEndOffsetAdjustment = 0;  
    int dioBeginOffsetAdjustment = 0;
  
    fileStreamingSettings = &fileStreamingHandle->settings; 

    if (setIndexerMode(indexer, 1/*fileStreamingSettings->playSpeed*/, &fileStreamingHandle->psi) == false) {
        BDBG_ERR(("%s: Failed to set the indexer mode for file session %p\n", __FUNCTION__, fileStreamingHandle));
        return false;
    }   
    
    if((int)fileStreamingSettings->beginTimeOffset != 0) {
        nav_indexer_setIndexByTimeOffset(indexer->context, fileStreamingSettings->beginTimeOffset);
        getReadOffset(fileStreamingHandle, &dioBeginOffsetAdjustment, &dioEndOffsetAdjustment, &fileStreamingSettings->beginFileOffset);
        lseek(fileStreamingHandle->fd, 0, SEEK_SET);
        nav_indexer_rewind(indexer->context);    
    }
    else {
        fileStreamingSettings->beginFileOffset = 0;
    }

    if( (int)fileStreamingSettings->endTimeOffset != 0) {
        nav_indexer_setIndexByTimeOffset(indexer->context, fileStreamingSettings->endTimeOffset);
        getReadOffset(fileStreamingHandle, &dioBeginOffsetAdjustment, &dioEndOffsetAdjustment, &fileStreamingSettings->endFileOffset);
        lseek(fileStreamingHandle->fd, 0, SEEK_SET);
        nav_indexer_rewind(indexer->context);
    }
    else {
        fileStreamingSettings->endFileOffset = 0;
    }
    return true;
}

/***************************************************************************
Summary:
This function returns the Media information associated with a file in psi.
***************************************************************************/
B_PlaybackIpError
B_PlaybackIp_FileStreamingGetMediaInfo(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    B_PlaybackIpError rc;
    char *infoFileName = NULL;
    B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings;

    BDBG_MSG(("%s: %p", __FUNCTION__, fileStreamingHandle));
    BDBG_ASSERT(fileStreamingHandle);

    if (!fileStreamingHandle || !psi) {
        BDBG_ERR(("%s: Invalid params: fileStreamingHandle %p, psi %p\n", __FUNCTION__, fileStreamingHandle, psi));
        rc = B_ERROR_INVALID_PARAMETER;
        goto error;
    }
    fileStreamingSettings = &fileStreamingHandle->settings;
    memset(psi, 0, sizeof(B_PlaybackIpPsiInfo));

    /* info file is <mediaInfoFilesDir>/<fileName>.info */
    if ((infoFileName = buildInfoFileName(fileStreamingSettings->mediaInfoFilesDir, fileStreamingSettings->fileName)) == NULL) {
        BDBG_WRN(("%s: Failed to allocate memory for Info File for (%s) \n", __FUNCTION__, fileStreamingSettings->fileName));
        rc = B_ERROR_UNKNOWN;
        goto error;
    }
    BDBG_MSG(("%s: Info File name: %s", __FUNCTION__, infoFileName));

    if (getMediaInfo(fileStreamingHandle, infoFileName, psi) == false) {
        BDBG_WRN(("%s: Failed to create/obtain Info File for file (%s) \n", __FUNCTION__, fileStreamingSettings->fileName));
        rc = B_ERROR_UNKNOWN;
        goto error;
    }

    fileStreamingHandle->psi = *psi;  /* cache the PSI info */
    
    /*get byte range from time range using the indexer
     if the given end time offset is equal to total duration of the file ,
     over write the endByteOffset given by indexer with the file size
     this is needed because byte offset given by the indexer for the file end time is not matching with the end duraion given by probe*/
    if(fileStreamingHandle->settings.endTimeOffset || fileStreamingHandle->settings.beginTimeOffset) {      
       if(getByteRangeFromTimeRange(fileStreamingHandle)) {         
            if((uint)fileStreamingHandle->settings.endTimeOffset == psi->duration/1000) {               
                fileStreamingHandle->settings.endFileOffset = psi->contentLength - 1;
            }
            psi->beginFileOffset = fileStreamingHandle->settings.beginFileOffset;
            psi->endFileOffset = fileStreamingHandle->settings.endFileOffset;           
            fileStreamingHandle->psi = *psi;  
       }
    }

    rc = B_ERROR_SUCCESS;
error:
    if (infoFileName)
        BKNI_Free(infoFileName);

    return rc;
}

bool
setReadOffset(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    int *dioBeginOffsetAdjustment,
    int *dioEndOffsetAdjustment,
    unsigned char **readBuf,
    int *bytesToRead
    )
{
    int rc;
    off_t dioBeginOffset;
    struct index_entry_t entry;

    while (!fileStreamingHandle->stop) {
        memset(&entry, 0, sizeof(entry));
        if ( (rc = nav_indexer_next(fileStreamingHandle->indexer.context, &entry)) ) {
            BDBG_WRN(("%s: nav_indexer_next returned %d, error or EOF: autoRewind %d", __FUNCTION__, rc, fileStreamingHandle->settings.autoRewind));
            if (fileStreamingHandle->settings.autoRewind) {
                /* we rewind only in trickmodes */
                if (nav_indexer_rewind(fileStreamingHandle->indexer.context) < 0) {
                    BDBG_ERR(("%s: nav_indexer_rewind failed, returning failure", __FUNCTION__));
                    return false;
                }
                BKNI_Sleep(500);
                continue;
            }
            return false;
        }
        if (entry.insertPkt) {
            int bytesWritten;
            unsigned char *pkt;
            /* this is a special EOS packet that needs to be sent for AVC streams */
            entry.size = fileStreamingHandle->psi.transportTimeStampEnabled ? 192 : 188;
            BDBG_MSG(("inserting special pkts: entry size %d, insert pkt %d, sync byte %x, byte5 %x, ctrl %x, discard h %d, t %d, pts %x", 
                        entry.size, entry.insertPkt, 
                        *(char *)entry.pkt, 
                        *((char *)&entry.pkt[5]),
                        ntohl(*((int *)&entry.pkt[12])),
                        ntohl(*((int *)&entry.pkt[40])),
                        ntohl(*((int *)&entry.pkt[44])),
                        ntohl(*((int *)&entry.pkt[48]))
                     ));
            pkt = fileStreamingHandle->pkt;
            /* FOR DTCP/IP library, it needs a uncached address, but how to handle such pkt in memory */
            memcpy(pkt, entry.pkt, entry.size);
            bytesWritten = B_PlaybackIp_UtilsStreamingCtxWriteAll((struct bfile_io_write *)&fileStreamingHandle->data, pkt, entry.size);
            if (bytesWritten < (int)(entry.size)) {
                BDBG_ERR(("%s: write failed to insert %d bytes of special pkt for streaming session, wrote %d bytes, errno %d\n", __FUNCTION__, entry.size, fileStreamingHandle, bytesWritten, errno));
                fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
                return false;
            }
            /* special pkt is sent, so goto to next entry */
            continue;
        }
        else {
            /* make the entry dio compatible */
            dioBeginOffset = entry.offset & ~DIO_MASK;
            *dioBeginOffsetAdjustment = entry.offset - dioBeginOffset;
            *dioEndOffsetAdjustment = DIO_BLK_SIZE - ( (entry.size+*dioBeginOffsetAdjustment) % DIO_BLK_SIZE);
            if (lseek(fileStreamingHandle->fd, dioBeginOffset, SEEK_SET) != dioBeginOffset) {
                BDBG_ERR(("%s: Failed to set the offset to %lld for fd %d, errno %d\n", __FUNCTION__, dioBeginOffset, fileStreamingHandle->fd, errno));
                return false;
            }

            if (entry.size > fileStreamingHandle->streamingBufSize) {
                unsigned char *xbuf;
                B_PlaybackIp_UtilsFreeMemory(fileStreamingHandle->streamingBufOrig);
                if ((fileStreamingHandle->streamingBufOrig = B_PlaybackIp_UtilsAllocateMemory(entry.size + 3*DIO_BLK_SIZE, fileStreamingHandle->settings.heapHandle)) == NULL) {
                    BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
                    fileStreamingHandle->streamingBuf = NULL;
                    return false;
                }
                xbuf = fileStreamingHandle->streamingBufOrig + DIO_BLK_SIZE; 
                BDBG_MSG(("Re-allocated the streaming buf (new %p) to %d size", xbuf, entry.size));
#ifndef DMS_CROSS_PLATFORMS
#ifndef ANDROID
#define ENABLED_DIO
#endif
#endif
#ifdef ENABLED_DIO
                *readBuf = DIO_ALIGN(xbuf);
#else
                *readBuf = xbuf;
#endif
                fileStreamingHandle->streamingBuf = *readBuf;
                fileStreamingHandle->streamingBufSize = entry.size;
            }

            *bytesToRead = entry.size + *dioBeginOffsetAdjustment + *dioEndOffsetAdjustment; /* need to read full entry size + any extra dio related bytes */
            *readBuf = fileStreamingHandle->streamingBuf;
            BDBG_MSG(("%s: file offset %lld for entry offset %lld dio offset begin %d, end %d, entry size %d, bytesToRead %d", 
                        __FUNCTION__, dioBeginOffset, entry.offset, *dioBeginOffsetAdjustment, *dioEndOffsetAdjustment, entry.size, *bytesToRead));
            return true;
        }
    }
    return false;
}

double difftime1(
    struct timeval *start, 
    struct timeval *stop) 
{
    double dt = (1000000.*(stop->tv_sec - start->tv_sec)) + 1.0*(stop->tv_usec - start->tv_usec);
    return dt;
}

static void
fileHttpStreamingThread(
    void *data
    )
{
    B_PlaybackIpFileStreamingHandle fileStreamingHandle = (B_PlaybackIpFileStreamingHandle)data;
    B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings;
    int bufSize;
    unsigned char *xbuf = NULL, *readBuf;
    int fd, streamingFd;
    off_t mpegHdrBeginOffset = 0;
    off_t dioBeginOffset = 0;
    off_t bytesToStream = 0;
    off_t bytesRemaining = 0;
    int dioBeginOffsetAdjustment = 0;
    int cryptoBeginOffsetAdjustment = 0;
    int dioEndOffsetAdjustment = 0;
    int bytesWritten, bytesToWrite, bytesRead, bytesToRead;
    unsigned int loopCount = 0;
    bool useIndexer = false;
#ifdef ENABLE_SW_PACING
    double rate;
    struct timeval start, stop;
    double dt=0, maxrate;
#endif
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    unsigned char *clearBuf = NULL;
#endif

    BDBG_ASSERT(fileStreamingHandle);
    fileStreamingSettings = &fileStreamingHandle->settings;
    fd = fileStreamingHandle->fd;
    streamingFd = fileStreamingSettings->streamingFd;

    if ((fileStreamingHandle->pktOrig = B_PlaybackIp_UtilsAllocateMemory(2*224, fileStreamingSettings->heapHandle)) == NULL) {
        BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
        goto error;
    }
    fileStreamingHandle->pkt = fileStreamingHandle->pktOrig + 224;

    if (fileStreamingSettings->transportTimestampEnabled)
        bufSize = (TS_PKT_SIZE+4) * HTTP_AES_BLOCK_SIZE * STREAMING_BUF_MULTIPLE * 2;
    else
        bufSize = TS_PKT_SIZE * HTTP_AES_BLOCK_SIZE * STREAMING_BUF_MULTIPLE * 2;
    
    if ((fileStreamingHandle->streamingBufOrig = B_PlaybackIp_UtilsAllocateMemory(bufSize + 3*DIO_BLK_SIZE, fileStreamingSettings->heapHandle)) == NULL) {
        BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
        goto error;
    }
    xbuf = fileStreamingHandle->streamingBufOrig + DIO_BLK_SIZE; /* saving space for residual bytes left in clear during previous encryption operation as there were not mod 16 sized */
#ifdef ENABLED_DIO
    readBuf = DIO_ALIGN(xbuf);
#else
    readBuf = xbuf;
#endif
    fileStreamingHandle->streamingBuf = readBuf;
    fileStreamingHandle->streamingBufSize = bufSize;
    BDBG_WRN(("handle %p, fd %d, streamingFd %d, streaming buf %p, orig %p, size %d", fileStreamingHandle, fd, streamingFd, fileStreamingHandle->streamingBuf, xbuf, bufSize));

#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    if (fileStreamingHandle->data.pvrDecKeyHandle) {
        NEXUS_MemoryAllocationSettings allocSettings;
        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        if (fileStreamingSettings->heapHandle)
            allocSettings.heap = fileStreamingSettings->heapHandle;
        if (NEXUS_Memory_Allocate(bufSize, &allocSettings, (void *)(&clearBuf))) {
            BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
            goto error;
        }
    }
#endif

#ifdef ENABLED_DIO
    /* setup flag for DirectIO operations */
    if (fcntl(fd, F_SETFL, O_DIRECT) < 0) {
        BDBG_WRN(("%s: fcntl to set the O_DIRECT flag failed, fd %d, errno %d, ignore it", __FUNCTION__, fd, errno));
    }
#endif

    if (fileStreamingSettings->endFileOffset && fileStreamingSettings->playSpeed == 1) {       
        if (fileStreamingSettings->transportTimestampEnabled)
            mpegHdrBeginOffset = fileStreamingSettings->beginFileOffset & ~(TS_PKT_SIZE+4);
        else
            mpegHdrBeginOffset = fileStreamingSettings->beginFileOffset & ~TS_PKT_SIZE;
        dioBeginOffset = fileStreamingSettings->beginFileOffset & ~DIO_MASK;
        BDBG_MSG(("dioBeginOffset %lld, mpegHdrBeginOffset %lld", dioBeginOffset, mpegHdrBeginOffset));
        if (dioBeginOffset > mpegHdrBeginOffset) {
            /* dioBeginOffset is after than mpeg HDR offset, so it will not cover the mpeg header */
            /* we will need to go back another DIO block to include this mpeg block */
            dioBeginOffset -= DIO_BLK_SIZE;
            BDBG_MSG(("Adjust DIO offset to cover MPEG Hdr, dioBeginOffset %lld", dioBeginOffset));
        }
        dioBeginOffsetAdjustment = fileStreamingSettings->beginFileOffset - dioBeginOffset;
        cryptoBeginOffsetAdjustment = mpegHdrBeginOffset - dioBeginOffset;
        BDBG_MSG(("dioBeginOffsetAdjustment %d, cryptoBeginOffsetAdjustment %d", dioBeginOffsetAdjustment, cryptoBeginOffsetAdjustment));
        if (lseek(fd, dioBeginOffset, SEEK_SET) != dioBeginOffset) {
            BDBG_ERR(("%s: Failed to set the offset to %lld for fd %d, errno %d\n", __FUNCTION__, dioBeginOffset, fd, errno));
            goto error;
        }
        bytesToStream = fileStreamingSettings->endFileOffset - fileStreamingSettings->beginFileOffset + 1;
        BDBG_MSG(("offset: end %lld, begin %lld dio %lld, bytesToStream %lld for fd %d, streamingFd %d", fileStreamingSettings->endFileOffset, fileStreamingSettings->beginFileOffset, dioBeginOffset, bytesToStream, fd, streamingFd));
    }
    else if (fileStreamingHandle->indexFileFp && 
            (fileStreamingSettings->playSpeed != 1 ||   /* accounts for trick play cases */
            fileStreamingSettings->beginTimeOffset)     /* accounts for resuming to play from trick play state */
            ) {
        /* can seek using beginTimeOffset only if we have index file */
        BDBG_MSG(("%s: stream content at %d speed from time offset %0.3f using index file", __FUNCTION__, fileStreamingSettings->playSpeed, fileStreamingSettings->beginTimeOffset));
        if (setupMediaIndexerTrickMode(fileStreamingHandle, fileStreamingSettings->playSpeed, &fileStreamingHandle->psi) != true) {
            BDBG_ERR(("%s: Failed to use the index file to stream at %d speed for file streaming session %p", __FUNCTION__, fileStreamingSettings->playSpeed, fileStreamingHandle));
            goto error;
        }
        useIndexer = true;
#ifndef IP_STREAMER_DEMO_MODE
        if (fileStreamingSettings->endFileOffset > fileStreamingSettings->beginFileOffset)
            bytesToStream = fileStreamingSettings->endFileOffset - fileStreamingSettings->beginFileOffset + 1;
        else if(fileStreamingSettings->beginFileOffset > fileStreamingSettings->endFileOffset)
            bytesToStream = fileStreamingSettings->beginFileOffset - fileStreamingSettings->endFileOffset + 1;
        BDBG_MSG(("%s: offsets: end %lld, begin %lld, bytesToStream %lld", 
            __FUNCTION__, fileStreamingSettings->endFileOffset, fileStreamingSettings->beginFileOffset, bytesToStream));
#endif
    }
    else {
        /* re-position the offset to beginging as media probe may have moved it */      
        lseek(fd, 0, SEEK_SET);
    }

    fileStreamingHandle->stop = false;
    fileStreamingHandle->data.stopStreaming = false;
    fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eActive;
    bytesRemaining = bytesToStream;
    fileStreamingHandle->threadRunning = true;

#ifdef ENABLE_SW_PACING
    gettimeofday(&start, NULL);
    maxrate = (double)fileStreamingHandle->psi.avgBitRate / 1024*1024.0;
#endif

    if (fileStreamingSettings->appHeader.valid) {
        BDBG_MSG(("%s: appHeader Enabled %d, length %d", __FUNCTION__, fileStreamingSettings->appHeader.valid, fileStreamingSettings->appHeader.length));
        BDBG_MSG(("data: %x data: %x data: %x data: %x ", fileStreamingSettings->appHeader.data[0], fileStreamingSettings->appHeader.data[1], fileStreamingSettings->appHeader.data[2], fileStreamingSettings->appHeader.data[3]));
        bytesWritten = B_PlaybackIp_UtilsStreamingCtxWriteAll((struct bfile_io_write *)&fileStreamingHandle->data, fileStreamingSettings->appHeader.data, fileStreamingSettings->appHeader.length);
        if (bytesWritten != (int)fileStreamingSettings->appHeader.length) {
            /* this happens if client closed the connection or client connection is dead */
            BDBG_MSG(("%s: handle: %p, failed to write %d bytes of app header data of len %d for fd %d", __FUNCTION__, fileStreamingHandle, bytesWritten, fileStreamingSettings->appHeader.length, streamingFd));
            fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
            goto error;
        }
        BDBG_MSG(("%s: handle: %p, wrote %d bytes of app header data of len %d for fd %d", __FUNCTION__, fileStreamingHandle, bytesWritten, fileStreamingSettings->appHeader.length, streamingFd));
    }

    while (!fileStreamingHandle->stop) {
        readBuf = fileStreamingHandle->streamingBuf;
        if (useIndexer) {
            if (setReadOffset(fileStreamingHandle, &dioBeginOffsetAdjustment, &dioEndOffsetAdjustment, &readBuf, &bytesToRead) != true) {
                BDBG_WRN(("%s: setReadOffset failed, breaking out of streaming loop, streamed %lld bytes in %d send calls", __FUNCTION__, fileStreamingHandle->totalBytesStreamed, loopCount));
                fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eEof;
                break;
            }
        }
        else {
            /* streaming directly from file */
            bytesToRead = bufSize;
        }
        BDBG_MSG(("fd %d, readBUf %p, bytesToRead %d\n", fd, readBuf, bytesToRead));
        if ((bytesRead = read(fd, readBuf, bytesToRead)) <= 0) {
            BDBG_MSG(("%s: read returned %d for fd %d, errno %d, autoRewind %d", __FUNCTION__, bytesRead, fd, errno, fileStreamingSettings->autoRewind));
            if (errno == EINTR) {
                BDBG_WRN(("%s: read Interrupted for fd %d, retrying errno %d", __FUNCTION__, fd, errno));
                continue;
            }
            if (bytesRead == 0) {
                /* reached file end */
                if (fileStreamingSettings->autoRewind) {
                    lseek(fd, 0, SEEK_SET);
                    BDBG_MSG(("%s: rewinding for fd %d\n", __FUNCTION__, fd));
                    continue;
                }
                else {
                    fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eEof;
                }
            }
            else {
                perror("read:");
            }
            /* read error case */
            fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;          
            break;
        }
        if (bytesRead != bytesToRead) {
            BDBG_MSG(("%s: bytesRead %d bytesToRead %d, sd %d", __FUNCTION__, bytesRead, bytesToRead, streamingFd));            
        }
        /* determine bytes to write: account for DIO related adjustments from begining & end of the read buffer */
        bytesToWrite = bytesRead - dioBeginOffsetAdjustment - dioEndOffsetAdjustment;
        if (bytesToStream && bytesToWrite > bytesRemaining)
            bytesToWrite = bytesRemaining;
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
        if (fileStreamingHandle->data.pvrDecKeyHandle) {
            /* since we read from the DIO aligned offset (512), so this read is already AES block size (16) aligned. */
            /* decrypt the buffer */
            if (B_PlaybackIp_UtilsPvrDecryptBuffer(&fileStreamingHandle->data, readBuf+cryptoBeginOffsetAdjustment, clearBuf, bytesRead, &bytesRead) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: PVR Decryption Failed", __FUNCTION__));
                break;
            }
            readBuf = clearBuf;
            BDBG_MSG(("%s: decrypting of %d bytes buffer done", __FUNCTION__, bytesRead));
        }
#endif
        bytesWritten = B_PlaybackIp_UtilsStreamingCtxWriteAll((struct bfile_io_write *)&fileStreamingHandle->data, readBuf+dioBeginOffsetAdjustment, bytesToWrite);
        if (bytesWritten <= 0) {
            /* this happens if client closed the connection or client connection is dead */
            BDBG_WRN(("%s: failed to write %d bytes, fd %d, handle %p, wrote %d bytes, errno %d, streamed %lld bytes in %d send calls", __FUNCTION__, bytesToWrite, streamingFd, fileStreamingHandle, bytesWritten, errno, fileStreamingHandle->totalBytesStreamed, loopCount));
            fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
            break;
        }
        BDBG_MSG(("%s: wrote %d bytes for streaming fd %d, bytesToRead %d, bytesRead %d, dioBeginOffsetAdjustment %d bytes, sync %x\n", 
                    __FUNCTION__, bytesWritten, streamingFd, bytesToRead, bytesRead, dioBeginOffsetAdjustment, *(readBuf+dioBeginOffsetAdjustment)));       
        dioBeginOffsetAdjustment = 0;
        cryptoBeginOffsetAdjustment = 0;
        dioEndOffsetAdjustment = 0;
        fileStreamingHandle->totalBytesStreamed += bytesWritten;
        if (!fileStreamingSettings->autoRewind && bytesToStream && fileStreamingSettings->playSpeed == 1) {
            if (fileStreamingHandle->totalBytesStreamed >= bytesToStream) {
                BDBG_WRN(("%s: breaking from streaming loop: streamed %lld bytes for streaming fd %d, asked %lld bytes", __FUNCTION__, fileStreamingHandle->totalBytesStreamed, streamingFd, bytesToStream));
                fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eEof;            
                break;
            }
            bytesRemaining -= bytesWritten;
        }
        loopCount++;
#ifdef ENABLE_SW_PACING
        gettimeofday(&stop, NULL);
        dt = difftime1(&start, &stop);
        rate = 8.*fileStreamingHandle->totalBytesStreamed/dt;
        while (rate > maxrate) {
            usleep(10000);
            gettimeofday(&stop, NULL);
            dt = difftime1(&start,&stop);
            rate = 8.*fileStreamingHandle->totalBytesStreamed/dt;
        }

        if ((loopCount % 50) == 0) {
            BDBG_MSG(("[%6lu] Wrote %10lu Bytes in dt = %12.2fusec at rate=%2.1f/%2.1f\n", loopCount, fileStreamingHandle->totalBytesStreamed, dt, rate, maxrate));
        }
#endif
#if 0
        /* commenting this out as it was causing stutter for streaming out high bitrate MVC streams. This code introduces delay in streaming out which causes underflow at the client side */
        /* the original intent of this code was to not starve out the local playback thread on server by allowing it to run during this sleep time */
        /* TODO: 07/16/2012: remove this code once it is vetted out in the next couple of months */

        if (fileStreamingSettings->playSpeed < 4 && fileStreamingSettings->playSpeed > 0 ) {
            /* sleep is needed at lower speeds otherwise client thread on the local VMS server gets starved */
            BKNI_Sleep(30);
        }
#endif
    }
error:
    BDBG_MSG(("%s: handle %p, streamed %lld bytes for streaming fd %d in %d send calls", __FUNCTION__, fileStreamingHandle, fileStreamingHandle->totalBytesStreamed, streamingFd, loopCount));  
    if (fileStreamingHandle->streamingBufOrig) B_PlaybackIp_UtilsFreeMemory(fileStreamingHandle->streamingBufOrig);
    if (fileStreamingHandle->pktOrig) B_PlaybackIp_UtilsFreeMemory(fileStreamingHandle->pktOrig);
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    if (clearBuf) 
        NEXUS_Memory_Free(clearBuf);
#endif
    BKNI_SetEvent(fileStreamingHandle->stopStreamingEvent);
    fileStreamingHandle->threadRunning = false;
    return;
}

static void 
fileRtpUdpStreamingThread(
    void *data
    )
{
    B_PlaybackIpFileStreamingHandle fileStreamingHandle = (B_PlaybackIpFileStreamingHandle)data;
    B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings;
    int bufSize;
    unsigned char *xbuf = NULL, *readBuf;
    int fd, streamingFd;
    int bytesWritten, bytesToWrite, bytesRead, bytesToRead;
    unsigned int loopCount = 0;
    double rate = 19.4;
    struct timeval start, stop;
    double dt=0, maxrate = 19.4;

    BDBG_ASSERT(fileStreamingHandle);
    fileStreamingSettings = &fileStreamingHandle->settings;
    fileStreamingHandle->threadRunning = true;
    fd = fileStreamingHandle->fd;
    streamingFd = fileStreamingSettings->streamingFd;

    bufSize = STREAMING_BUF_SIZE + 2*DIO_BLK_SIZE;
    if ((fileStreamingHandle->streamingBufOrig = B_PlaybackIp_UtilsAllocateMemory(bufSize, fileStreamingSettings->heapHandle)) == NULL) {
        BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
        goto error;
    }
    xbuf = fileStreamingHandle->streamingBufOrig + DIO_BLK_SIZE; /* saving space for residual bytes left in clear during previous encryption operation as there were not mod 16 sized */
#ifdef ENABLED_DIO_UDP
    readBuf = DIO_ALIGN(xbuf);
    /* setup flag for DirectIO operations */
    if (fcntl(fd, F_SETFL, O_DIRECT) < 0) {
        BDBG_WRN(("%s: fcntl to set the O_DIRECT flag failed, fd %d, errno %d, ignore it", __FUNCTION__, fd, errno));
    }
#else
    readBuf = xbuf;
#endif
    fileStreamingHandle->streamingBuf = readBuf;
    fileStreamingHandle->streamingBufSize = STREAMING_BUF_SIZE;

    /* reset the file pointer */
    lseek(fd, 0, SEEK_SET);
    fileStreamingHandle->stop = false;
    fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eActive;

    maxrate = (double)fileStreamingHandle->psi.avgBitRate / (1000.0*1000);
    if (maxrate == 0) maxrate = 14.0; /* just some number */
    gettimeofday(&start, NULL);
    BDBG_WRN(("%s: handle %p, fd %d, streamingFd %d, streaming buf %p, orig %p, streaming bitrate %.1f (bits/milli-sec)", 
                __FUNCTION__, fileStreamingHandle, fd, streamingFd, fileStreamingHandle->streamingBuf, xbuf, maxrate));
#ifdef ENABLED_DIO_UDP
    bytesToRead = ((512 * (188>>2) * 7));
#else
    bytesToRead = 1316 * 48;
#endif
    gettimeofday(&start, NULL);
    while (!fileStreamingHandle->stop) {
        BDBG_MSG(("fd %d, readBuf %p, bytesToRead %d\n", fd, readBuf, bytesToRead));
        /* streaming directly from file */
        if ((bytesRead = read(fd, readBuf, bytesToRead)) <= 0) {
            BDBG_WRN(("%s: read returned %d for fd %d, errno %d, autoRewind %d", __FUNCTION__, bytesRead, fd, errno, fileStreamingSettings->autoRewind));
            if (errno == EINTR) {
                BDBG_WRN(("%s: read Interrupted for fd %d, retrying errno %d\n", __FUNCTION__, fd, errno));
                continue;
            }
            if (bytesRead == 0) {
                /* reached file end */
                if (fileStreamingSettings->autoRewind) {
                    lseek(fd, 0, SEEK_SET);
                    BDBG_WRN(("%s: rewinding for fd %d\n", __FUNCTION__, fd));
                    continue;
                }
                else {
                    fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eEof;
                }
            }
            else {
                perror("read:");
            }
            /* read error case */
            fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;          
            break;
        }
        if (bytesRead != bytesToRead) {
            BDBG_MSG(("%s: bytesRead %d bytesToRead %d, sd %d", __FUNCTION__, bytesRead, bytesToRead, streamingFd));            
        }
        /* determine bytes to write: account for DIO related adjustments from begining & end of the read buffer */
        bytesToWrite = bytesRead;
        bytesWritten = B_PlaybackIp_UtilsStreamingCtxWriteAll((struct bfile_io_write *)&fileStreamingHandle->data, readBuf, bytesToWrite);
        if (bytesWritten < bytesToWrite) {
            BDBG_MSG(("write failed to write %d bytes for streaming fd %d, wrote %d bytes, errno %d\n", bytesToWrite, streamingFd, bytesWritten, errno));           
            fileStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
            break;
        }
        BDBG_MSG(("%s: wrote %d bytes for streaming fd %d, bytesToRead %d, bytesRead %d, totalBytesWritten %lu", 
                    __FUNCTION__, bytesWritten, streamingFd, bytesToRead, bytesRead, fileStreamingHandle->totalBytesStreamed));       
        fileStreamingHandle->totalBytesStreamed += bytesWritten;
        loopCount++;
        
        gettimeofday(&stop, NULL);
        dt = difftime1(&start, &stop);
        rate = 8.*fileStreamingHandle->totalBytesStreamed/dt;
        if ((loopCount % 100) == 0) {
            BDBG_MSG(("[%6lu] Wrote %10lu Bytes in dt = %12.2fusec at rate=%2.1f/%2.1f\n", loopCount, fileStreamingHandle->totalBytesStreamed, dt, rate, maxrate));
        }
        while (rate > maxrate) {
            usleep(10000);
            gettimeofday(&stop, NULL);
            dt = difftime1(&start,&stop);
            rate = 8.*fileStreamingHandle->totalBytesStreamed/dt;
        }

    }
error:
    BKNI_Sleep(500);
    BDBG_WRN(("%s: handle %p, streamed %lld bytes for streaming fd %d in %d send calls", __FUNCTION__, fileStreamingHandle, fileStreamingHandle->totalBytesStreamed, streamingFd, loopCount));  
    BDBG_WRN(("[%6lu] Wrote %10lu Bytes in dt = %12.2fusec at rate=%2.1f/%2.1f\n", loopCount, fileStreamingHandle->totalBytesStreamed, dt, rate, maxrate));
    if (fileStreamingHandle->streamingBufOrig) B_PlaybackIp_UtilsFreeMemory(fileStreamingHandle->streamingBufOrig);
    BKNI_SetEvent(fileStreamingHandle->stopStreamingEvent);
    fileStreamingHandle->threadRunning = false;
    return;
}

/***************************************************************************
Summary:
This function starts streaming content from a file.
***************************************************************************/
B_PlaybackIpError
B_PlaybackIp_FileStreamingStart(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle 
    )
{
    B_PlaybackIpFileStreamingOpenSettings *fileStreamingSettings = &fileStreamingHandle->settings;
    void (*threadFunc)(void *);

    fileStreamingHandle->data.streamingProtocol = fileStreamingSettings->protocol;
    switch (fileStreamingSettings->protocol) {
        case B_PlaybackIpProtocol_eHttp: {
            fileStreamingHandle->data.fd = fileStreamingSettings->streamingFd;
            fileStreamingHandle->data.self = net_io_data_write;
            if (B_PlaybackIp_UtilsStreamingCtxOpen(&fileStreamingHandle->settings.securitySettings, &fileStreamingHandle->data) != B_ERROR_SUCCESS) {
                BDBG_ERR(("Failed to setup the streaming context\n"));
                return B_ERROR_UNKNOWN;
            }
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
            if (B_PlaybackIp_UtilsPvrDecryptionCtxOpen(&fileStreamingSettings->securitySettings, &fileStreamingHandle->data) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Failed to setup pvr decryption"));
                return B_ERROR_UNKNOWN;
            }
#endif
            threadFunc = fileHttpStreamingThread;
            BDBG_MSG(("%s: complete, streaming socket %d\n", __FUNCTION__, fileStreamingHandle->data.fd));
            break;
        }
        case B_PlaybackIpProtocol_eRtp: 
        case B_PlaybackIpProtocol_eUdp: {
            fileStreamingHandle->data.interfaceName = fileStreamingSettings->rtpUdpSettings.interfaceName;
            fileStreamingHandle->data.self = net_io_data_write;
            fileStreamingHandle->data.streamingSockAddr.sin_family = AF_INET;
            fileStreamingHandle->data.streamingSockAddr.sin_port = htons(fileStreamingSettings->rtpUdpSettings.streamingPort);
            if (inet_aton(fileStreamingSettings->rtpUdpSettings.streamingIpAddress, &fileStreamingHandle->data.streamingSockAddr.sin_addr) == 0) {
                BDBG_ERR(("%s: inet_aton() failed on %s", __FUNCTION__, fileStreamingSettings->rtpUdpSettings.streamingIpAddress));
                goto error;
            }
            BDBG_WRN(("Streaming URL is %s%s:%d", fileStreamingSettings->protocol == B_PlaybackIpProtocol_eRtp? "rtp://":"udp://",
                        inet_ntoa(fileStreamingHandle->data.streamingSockAddr.sin_addr), ntohs(fileStreamingHandle->data.streamingSockAddr.sin_port)));

            if (B_PlaybackIp_UtilsRtpUdpStreamingCtxOpen(&fileStreamingHandle->settings.securitySettings, &fileStreamingHandle->data) != B_ERROR_SUCCESS) {
                BDBG_ERR(("Failed to setup the streaming context\n"));
                goto error;
            }
            B_PlaybackIp_UtilsSetRtpPayloadType(fileStreamingHandle->psi.mpegType, &fileStreamingHandle->data.rtpPayloadType);
            threadFunc = fileRtpUdpStreamingThread;
            BDBG_MSG(("%s: complete for RTP/UDP streaming, socket %d\n", __FUNCTION__, fileStreamingHandle->data.fd));
            break;
        }
        default:
            BDBG_ERR(("%s: non-supported protocol %d", __FUNCTION__, fileStreamingSettings->protocol));
            goto error;
    }

    if (BKNI_CreateEvent(&fileStreamingHandle->stopStreamingEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    fileStreamingHandle->streamingThread = B_Thread_Create("HTTP Streamer", (B_ThreadFunc)threadFunc, (void *)fileStreamingHandle, NULL);
    if (fileStreamingHandle->streamingThread == NULL) {
        BDBG_ERR(("%s: Failed to create HTTP media file streaming thread \n", __FUNCTION__));
        goto error;
    }
    return B_ERROR_SUCCESS;

error:
    B_PlaybackIp_UtilsStreamingCtxClose(&fileStreamingHandle->data);
    return B_ERROR_UNKNOWN;
}


/***************************************************************************
Summary:
This function stops streaming content from a file.
***************************************************************************/
void 
B_PlaybackIp_FileStreamingStop(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle
    )
{
    BERR_Code rc;
    BDBG_MSG(("%s: %p", __FUNCTION__, (void *)fileStreamingHandle));
    BDBG_ASSERT(fileStreamingHandle);
    fileStreamingHandle->stop = true;
    fileStreamingHandle->data.stopStreaming = true;

    if (fileStreamingHandle->stopStreamingEvent) {
        if (fileStreamingHandle->threadRunning) {
            rc = BKNI_WaitForEvent(fileStreamingHandle->stopStreamingEvent, 1000);
            if (rc == BERR_TIMEOUT) 
                BDBG_WRN(("%s: stopStreamingEvent timed out", __FUNCTION__));
            else
            if (rc != 0) {
                BDBG_ERR(("%s: failed to stop the file streaming thread", __FUNCTION__));
            }
            fileStreamingHandle->threadRunning = false;
        }
        BKNI_DestroyEvent(fileStreamingHandle->stopStreamingEvent);
    }
    switch (fileStreamingHandle->settings.protocol) {
        case B_PlaybackIpProtocol_eRtp: 
        case B_PlaybackIpProtocol_eUdp: {
            B_PlaybackIp_UtilsRtpUdpStreamingCtxClose(&fileStreamingHandle->data);
            break;
        }
        case B_PlaybackIpProtocol_eHttp:
        default:
            break;
    }
    if (fileStreamingHandle->streamingThread) {
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
        B_PlaybackIp_UtilsPvrDecryptionCtxClose(&fileStreamingHandle->data);
#endif
        B_PlaybackIp_UtilsStreamingCtxClose(&fileStreamingHandle->data);
        B_Thread_Destroy(fileStreamingHandle->streamingThread);
        fileStreamingHandle->streamingThread = NULL;
    }
    return;
}

void 
B_PlaybackIp_FileStreamingGetStatus(
    B_PlaybackIpFileStreamingHandle fileStreamingHandle,
    B_PlaybackIpFileStreamingStatus *status 
    )
{
    BDBG_ASSERT(status);
    BDBG_ASSERT(fileStreamingHandle);
    status->bytesStreamed = fileStreamingHandle->totalBytesStreamed;
    status->connectionState = fileStreamingHandle->connectionState;
}


#endif /* LINUX || VxWorks */
