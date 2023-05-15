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
* $brcm_Workfile: b_playback_ip_live_streaming.c $
* $brcm_Revision: 58 $
* $brcm_Date: 11/9/12 3:22p $
*
* Description: Live Streaming of Data to a Remote Client
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_live_streaming.c $
* 
* 58   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 57   10/17/12 11:57a ssood
* SW7445-53: get playback_ip to compile for 7445 tool-chain
* 
* 56   10/16/12 5:22p ssood
* SW7231-858: converted socket connect to non-blocking operation with a
*  timeout
* 
* 55   8/16/12 6:06p ssood
* SW7425-3482: added support for allowing clients to specify the pids to
*  demux in Sat/IP protocol + handle pilotTone setting
* 
* 54   8/3/12 2:52p ssood
* SW7425-3678: extend ip lib & ip streamer to support streaming out live,
*  file, encoded content over RTP/UDP
* 
* 53   5/21/12 2:01p ssood
* SW7425-3042: suppress debug logging by default
* 
* 52   5/18/12 4:30p ssood
* SW7425-3042: record live session if env variable enableRecording is set
* 
* 51   5/17/12 6:36p ssood
* SW7425-3042: simplified build flags & code by using nexus multi process
*  capabilities
* 
* 50   5/3/12 4:16p ssood
* SW7346-805: change the name of streaming thread to indicate the source
*  of data (either rave buffer or fifo)
* 
* 49   5/1/12 6:22p ssood
* SW7346-805: enhance ip_streamer to use playback path for streaming out
*  timeshifted live content
* 
* 48   3/1/12 7:01p ssood
* SW7425-2331: add logic to handle timeout events during HLS session
*  (while waiting for client to re-connect for the next segment)
* 
* 47   2/28/12 6:48p ssood
* SW7425-2331: align the buffer size to match the AES block size for non-
*  HLS cases
* 
* 46   2/22/12 4:10p ssood
* SW7425-2331: Add support for dynamic bitrate adjustment during HLS
*  streaming
* 
* 45   2/3/12 3:23p ssood
* SW7425-1331: fixed compilation error for non-nexus security builds
* 
* 44   2/2/12 8:11p ssood
* SW7425-1331: added support in live streaming modules for inserting
*  encrypted app specific header
* 
* 43   2/2/12 6:57p ssood
* SW7425-2331: Add media segmentation support for HLS Server
* 
* 42   10/12/11 5:03p ssood
* SW7346-341: fix cross-platform build related warnings
* 
* 41   9/30/11 9:46a ssood
* SW7346-341: keep netrecord function outside the live streaming so that
*  brutus can build
* 
* 40   9/26/11 10:22a ssood
* SW7346-341: Add PVR Decryption support for file streaming
* 
* 39   9/19/11 1:50p ssood
* SW7425-1040: added logic to better handle network congestion
* 
* 38   8/30/11 5:09p ssood
* SW7425-504: lowered the recpump service interval to 100msec
* 
* 37   8/22/11 12:21p ssood
* SW7425-1040: add logic to Skip PSI probing for FCC & provide end of
*  streaming callback
* 
* 36   8/15/11 12:32p jrubio
* SW7425-1040: fix compilation error
* 
* 35   8/5/11 11:33p ssood
* SW7425-1040: fix compilation error
* 
* 34   8/5/11 5:18p ssood
* SW7425-1040: fix warnings
* 
* 33   8/5/11 3:13p ssood
* SW7425-1040: add fast channel change support to IP Sessions
* 
* 32   6/18/11 11:52a ssood
* SW7425-718: 3128 related changes: use different frontend resources for
*  each streaming session even though it is same channel
* 
* 31   4/22/11 2:28p ssood
* SW7425-365: new tool chain required changing gethostbyname to
*  getaddrinfo
* 
* 30   3/17/11 4:36p ssood
* SW7420-1616: initial check-in for pvr encryption support
* 
* 29   9/7/10 5:51p ssood
* SW7420-883: added check to timeout a session if there is underflow of
*  live data
* 
* 28   9/1/10 10:22p ssood
* SW7420-883: delay checking the dtcp AKE completion by moving it from
*  session open to start time
* 
* 27   8/9/10 11:27a ssood
* SW7420-883: added code to detect transcoding session error and recover
*  gracefully
* 
* 26   4/2/10 4:23p ssood
* SW7420-561: Fixed Coverity issues
* 
* 25   3/8/10 11:18a ssood
* SW7420-561: fixed a compile warning for non-live streaming case
* 
* 24   1/19/10 4:55p ssood
* SW7420-454: convert verbose output to be available via msg modules
*  flags
* 
* 23   1/3/10 11:47p ssood
* SW7420-454: try reading more often incase of underflow to prevent
*  initial client stutters
* 
* 22   12/29/09 7:57p ssood
* SW7420-454: fix a race condition between app & streaming thread
* 
* 21   12/23/09 12:39a ssood
* SW7420-454: increase the time to wait for timeshift recording to start
*  before streaming starts to upto 10sec before reporting error
* 
* 20   12/22/09 11:03a ssood
* SW7420-454: add timeshift support for live streaming
* 
* 19   11/17/09 3:48p ssood
* SW7420-454: Enhance IP library to stream files from local disk
* 
* 18   11/1/09 1:26p ssood
* SW7420-340: got non-blocking write mode to work
* 
* 17   10/30/09 3:32p ssood
* SW7420-340: set file handle to NULL for failure cases
* 
* 16   10/30/09 11:05a ssood
* SW7420-340: work-around to fix initial macro-blocking which happens
*  when we use non-blocking mode
* 
* 15   10/22/09 12:55p ssood
* SW7420-340: converted streaming socket to non-blocking to avoid getting
*  blocked when client silently goes away
* 
* 14   10/10/09 11:54p ssood
* SW7420-340: dont block io thread if socket write q is full
* 
* 13   10/8/09 9:18a ssood
* SW7420-340: turned on print messages during Streaming session open &
*  close
* 
* 12   10/7/09 3:48p ssood
* SW7420-340: added code to monitor encryption time related stats
* 
* 11   10/6/09 4:58p ssood
* SW7420-340: added support for streaming DTCP/IP encrypted stream to > 1
*  clients + fixed compile errors for clear channels
* 
* 10   10/6/09 12:18p ssood
* SW7420-340: Add DTCP/IP support to ip_streamer & IP Applib
* 
* 9   10/2/09 4:50p ssood
* SW7420-340: fixing a compilation error
* 
* 8   10/2/09 4:23p ssood
* SW7420-340: temporary work around to handle network error condition
*  upon write to socket
* 
* 7   9/16/09 10:29a ssood
* SW7420-340: changed liveStreaming to LiveStreaming as it will
*  eventually also include streaming from disk
* 
* 6   9/7/09 4:44p ssood
* SW7420-340: modified live streaming api to differentiate it w/ network
*  recording
* 
* 5   9/1/09 7:34p ssood
* SW7420-340: initial check-in to support streaming of Live IP data
* 
* 4   5/11/09 5:14p ssood
* PR54955: Record TTS streams over HTTP: added HTTP header to indicate to
*  server that client is sending a TTS stream
* 
* 3   2/27/09 11:45a ssood
* PR50311: fixing a compilation warning
* 
* 2   11/13/08 6:06p lwhite
* PR47685: Coverity fixes
* 
* 1   11/7/08 2:53p jrubio
* PR48782: adding netrecord functions
* 
*
* 
***************************************************************************/

#if defined(LINUX) || defined(__vxworks)

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"

BDBG_MODULE(b_playback_ip_live_streaming);
#if defined(LINUX)
#include <sys/syscall.h>
#ifdef __mips__
#include <asm/cachectl.h>
#endif
#include <errno.h>
/* needed for the tcpconnect */
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
#endif

static bool enableRecording = false; /* gets set via env variable if we need to record a live channel for testing purpose */

static off_t
B_PlaybackIp_StreamNetDataTrim(bfile_io_write_t self, off_t trim_pos)
{
    BSTD_UNUSED(self);
    BSTD_UNUSED(trim_pos);
    return 0;
}

static off_t
B_PlaybackIp_StreamNetIndexTrim(bfile_io_write_t self, off_t trim_pos)
{
   BSTD_UNUSED(self);
   BSTD_UNUSED(trim_pos);
   return 0;
}


static const struct bfile_io_write net_io_data_write = { 
    B_PlaybackIp_UtilsStreamingCtxWrite,
    B_PlaybackIp_StreamNetDataTrim,
    BIO_DEFAULT_PRIORITY
};

static const struct bfile_io_write net_io_index_write = { 
    B_PlaybackIp_UtilsStreamingCtxWrite,
    B_PlaybackIp_StreamNetIndexTrim,
    BIO_DEFAULT_PRIORITY
};

void
B_PlaybackIp_NetworkRecordingStop(struct NEXUS_FileRecord *file)
{
    struct bfile_out_net *fileRecord = (struct bfile_out_net *)file;

    BDBG_ASSERT(file);
    BDBG_MSG(("%s: %p", __FUNCTION__, (void *)file));
    if(file->index) {
        if(fileRecord->index.fd>0) {
            BDBG_MSG(("closing index fd %d\n", fileRecord->index.fd));
            close(fileRecord->index.fd);
            fileRecord->index.fd = -1;
        }
    }

    if(fileRecord->data.fd>0) {
        BDBG_MSG(("closing data fd %d\n", fileRecord->data.fd));
        close(fileRecord->data.fd);
        fileRecord->data.fd = -1;
    }

    BKNI_Free(file);
    return;
}

/* NETRECORD This Function will start the  socket Connection  to stream out the data */
B_PlaybackIpError
B_PlaybackIp_NetworkRecordingStart(
    B_PlaybackIpSocketOpenSettingsHandle socketOpenSettings, 
    const char *fname, 
    const char *indexname,
    B_PlaybackIpPsiInfoHandle psi, 
    /*B_PlaybackIpSocketStateHandle socketState,*/
    NEXUS_FileRecordHandle *fileRecordPtr
    )
{
    int rc = -1;
    char *wbuf = NULL;
    struct bfile_out_net *fileRecord = NULL;

    /*BSTD_UNUSED(socketState);*/
    BDBG_MSG(("%s:\n", __FUNCTION__));

    wbuf = (char *)BKNI_Malloc(TMP_BUF_SIZE);
    fileRecord = BKNI_Malloc(sizeof(*fileRecord));
    if (!wbuf || !fileRecord) {
        BDBG_ERR(("%s: memory allocation failure\n", __FUNCTION__));
        rc = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }
    memset(wbuf, 0, TMP_BUF_SIZE);
    memset(fileRecord, 0, sizeof(*fileRecord));

    switch (socketOpenSettings->protocol) {
        /* Build a Post Request then Connect */
    case B_PlaybackIpProtocol_eHttp: {
        if (!psi || !fname ) {
           BDBG_ERR(("%s: Incorrect Args for Network Recording over HTTP, psi %p, fname %p\n",
                       __FUNCTION__, (void *)psi, (void *)fname)); 
           rc = B_ERROR_INVALID_PARAMETER;
           goto error;
        } 

        /* TODO: may need to convert the codecs from Nexus to Settop Type */
        /* as these are used to build Info file which expect values in Settop & not Nexus types */
        snprintf(wbuf, TMP_BUF_SIZE, "POST /%s HTTP/1.1\r\n"
                                 "Stream-Info: yes\r\nPcr-Pid: %x\r\nAudio-Pid: %x\r\n"
                                 "Video-Pid: %x\r\nVideo-Type: %x\r\nAudio-Type: %x\r\n"
                                 "TTS: %d\r\n"
                                 "\r\n"
                                  ,fname,psi->pcrPid,psi->audioPid,psi->videoPid,
                                  psi->videoCodec, psi->audioCodec, psi->transportTimeStampEnabled);

        if (B_PlaybackIp_UtilsTcpSocketConnect(NULL, socketOpenSettings->ipAddr, socketOpenSettings->port, 0 /* non-blocking mode */, &fileRecord->data.fd) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: Failed to Connect to the Server, errno %d\n", __FUNCTION__, errno));
            rc = B_ERROR_OS_ERROR;
            goto error;
        } 
        else {
            BDBG_WRN(("Connected filename %s", fname));
            /* now send the Post request */
            rc = write(fileRecord->data.fd,wbuf,strlen(wbuf));
            if (rc <= 0 || (unsigned)rc != strlen(wbuf)) {
                BDBG_ERR(("%s: Failed to Send the Post request for writing AV content to Server, errno %d\n", 
                            __FUNCTION__, errno));
                rc = B_ERROR_OS_ERROR;
                goto error;
            } 
            fsync(fileRecord->data.fd);
            BKNI_Sleep(200);
        }

        fileRecord->data.self = net_io_data_write;
        fileRecord->self.data = &(fileRecord->data.self);

        /* Check if we also need to record the index data */
        if (indexname) {
            memset(wbuf, 0, TMP_BUF_SIZE);
            BDBG_WRN(("Connected filename %s\n", indexname));
            snprintf(wbuf,TMP_BUF_SIZE, "POST /%s HTTP/1.1\r\n\r\n", indexname);
            
            if (B_PlaybackIp_UtilsTcpSocketConnect(NULL, socketOpenSettings->ipAddr, socketOpenSettings->port+1, 0 /* non-blocking mode */, &fileRecord->index.fd) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Failed to Connect to the Server, errno %d\n", __FUNCTION__, errno));
                rc = B_ERROR_OS_ERROR;
                goto error;
            } else {
                BDBG_WRN(("Connected for sending Index data\n"));
                /* send the post request */
                rc = write(fileRecord->index.fd, wbuf, strlen(wbuf));
                if (rc <= 0 || (unsigned)rc != strlen(wbuf)) {
                    BDBG_ERR(("%s: Failed to Send the Post request for writing Index data to Server, errno %d\n", 
                                __FUNCTION__, errno));
                    rc = B_ERROR_OS_ERROR;
                    goto error;
                }
                BKNI_Sleep(100);
            }

            fileRecord->index.self = net_io_index_write;
            fileRecord->self.index = &(fileRecord->index.self);
        }
    
        fileRecord->self.close = B_PlaybackIp_NetworkRecordingStop;
        rc = B_ERROR_SUCCESS;
        break;
        }
    default:
        BDBG_ERR((" non-supported socket type"));
        break;
    }

error:
    if (wbuf) BKNI_Free(wbuf);
    if (rc != B_ERROR_SUCCESS) {
        if (fileRecord) {
            if (fileRecord->index.fd) 
                close(fileRecord->index.fd);
            if (fileRecord->data.fd) 
                close(fileRecord->data.fd);
            BKNI_Free(fileRecord);
        }
    }
    else {
        if (fileRecord)
            *fileRecordPtr = &fileRecord->self;
    }
    return rc;
}

extern int createInfoFile( char *fileName, char *infoFileName, B_PlaybackIpPsiInfoHandle psi);
extern void replaceFileExtension(char *newName, int newNameSize, char *curName, char *newExtension);

typedef struct B_PlaybackIpLiveStreaming 
{
    B_PlaybackIpLiveStreamingOpenSettings settings;
    NEXUS_FilePlayHandle filePlayHandle;
    NEXUS_FifoRecordHandle fifoFileHandle;
    NEXUS_RecpumpHandle recpumpHandle;
    int fd;
    FILE *fp;
    int indexFileFd;
    FILE *indexFileFp;
    struct bfile_io_write_net data;
    B_ThreadHandle streamingThread;
    bool stop;  /* set when app wants to stop the streaming session */
    bool threadRunning; /* set when file streaming thread is running */
    BKNI_EventHandle startStreamingEvent;
    BKNI_EventHandle stopStreamingEvent;
    BKNI_EventHandle dataReadyEvent;
    off_t totalBytesStreamed;
    B_PlaybackIpConnectionState connectionState;    /* state of the socket: active, timeouts, error, eof */
    B_PlaybackIpPsiInfo psi;
    unsigned char *streamingBuf;
    unsigned char *streamingBufOrig;
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    unsigned char *clearBuf;
#endif
    unsigned streamingBufSize;
}B_PlaybackIpLiveStreaming;

/* Functions to stream live content from a QAM/VSB/IP Source to a local client */
void
B_PlaybackIp_LiveStreamingClose(B_PlaybackIpLiveStreamingHandle liveStreamingHandle)
{

    if (liveStreamingHandle) {
        BDBG_MSG(("%s: liveStreamingHandle %p, filePlayHandle %p", __FUNCTION__, liveStreamingHandle, liveStreamingHandle->filePlayHandle));
        B_PlaybackIp_UtilsStreamingCtxClose(&liveStreamingHandle->data);
        if (liveStreamingHandle->filePlayHandle) {
            NEXUS_FilePlay_Close(liveStreamingHandle->filePlayHandle);
        }
        BKNI_Free(liveStreamingHandle);
    }
    return;
}
/* This Function will start the  socket Connection  to stream out the data */
B_PlaybackIpLiveStreamingHandle
B_PlaybackIp_LiveStreamingOpen(
    B_PlaybackIpLiveStreamingOpenSettings *liveStreamingSettings
    )
{
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle = NULL;
    char *infoFileName;
    char *pValue = NULL;

    BDBG_MSG(("%s:", __FUNCTION__));
    if (!liveStreamingSettings) {
        BDBG_ERR(("%s: Invalid param: Need to pass-in live streaming open Settings\n", __FUNCTION__));
        goto error;
    }

    liveStreamingHandle = BKNI_Malloc(sizeof(B_PlaybackIpLiveStreaming));
    if (!liveStreamingHandle) {
        BDBG_ERR(("%s: memory allocation failure\n", __FUNCTION__));
        goto error;
    }
    memset(liveStreamingHandle, 0, sizeof(B_PlaybackIpLiveStreaming));

    liveStreamingHandle->settings = *liveStreamingSettings;
    liveStreamingHandle->fifoFileHandle = liveStreamingSettings->fifoFileHandle;
    liveStreamingHandle->recpumpHandle = liveStreamingSettings->recpumpHandle;
    liveStreamingHandle->dataReadyEvent = liveStreamingSettings->dataReadyEvent;

    /* Create Info file */
    if (liveStreamingSettings->psi) {
        infoFileName = liveStreamingSettings->fileName;
        replaceFileExtension(infoFileName, strlen(liveStreamingSettings->fileName), liveStreamingSettings->fileName, B_PLAYBACK_IP_INFO_FILE_EXTENSION);
        if (createInfoFile(liveStreamingSettings->fileName, infoFileName, liveStreamingSettings->psi)) {
            BDBG_ERR(("%s: Failed to create info file %s", __FUNCTION__, liveStreamingSettings->fileName));
            goto error;
        }
    }
    pValue = getenv("enableRecording");
    if (pValue) {
        enableRecording = true;
        BDBG_WRN(("%s: recording of live sessions is enabled...", __FUNCTION__));
    }
    BDBG_MSG(("%s: ", __FUNCTION__));
    return liveStreamingHandle;

error:
    if (liveStreamingHandle) {
        BKNI_Free(liveStreamingHandle);
    }
    return NULL;
}

static void 
liveStreamingThreadFromFifo(
    void *data
    )
{
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle = (B_PlaybackIpLiveStreamingHandle)data;
    B_PlaybackIpLiveStreamingOpenSettings *liveStreamingSettings;
    unsigned char *xbuf, *readBuf;
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    unsigned char *clearBuf;
    int cryptoUnAlignedBytes;
    int cryptoAlignedSize;
#endif
    int streamingFd;
    NEXUS_FilePlayHandle filePlayHandle;
    int bytesWritten, bytesToWrite =0, bytesRead, bytesToRead;
    unsigned int loopCount = 0;
    char indexFileName[128];
    int readTimeouts = 0;
    int bufSize;
    bool firstTime = true;
    bool streamingDisabled = false;
    NEXUS_MemoryAllocationSettings allocSettings;

    BDBG_ASSERT(liveStreamingHandle);
    liveStreamingSettings = &liveStreamingHandle->settings;
    streamingFd = liveStreamingSettings->streamingFd;

    /* setup bytes to read: account for transport timestamp & aes encryption */
    if (liveStreamingSettings->transportTimestampEnabled)
        bufSize = (TS_PKT_SIZE+4) * HTTP_AES_BLOCK_SIZE * STREAMING_BUF_MULTIPLE;
    else
        bufSize = TS_PKT_SIZE * HTTP_AES_BLOCK_SIZE * STREAMING_BUF_MULTIPLE;
    
    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    if (liveStreamingSettings->heapHandle)
        allocSettings.heap = liveStreamingSettings->heapHandle;
    if (NEXUS_Memory_Allocate(bufSize + 2*DIO_BLK_SIZE, &allocSettings, (void *)(&liveStreamingHandle->streamingBufOrig))) {
        BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
        goto error;
    }
    xbuf = liveStreamingHandle->streamingBufOrig + DIO_BLK_SIZE;
    liveStreamingHandle->streamingBuf = readBuf = DIO_ALIGN(xbuf);
    bytesToRead = bufSize;
    liveStreamingHandle->streamingBufSize = bufSize;

#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    if (liveStreamingHandle->data.pvrDecKeyHandle) {
        if (NEXUS_Memory_Allocate(bufSize, &allocSettings, (void *)(&clearBuf))) {
            BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
            goto error;
        }
        liveStreamingHandle->clearBuf = clearBuf;
    }
#endif

    replaceFileExtension(indexFileName, strlen(liveStreamingSettings->fileName), liveStreamingSettings->fileName, B_PLAYBACK_IP_INDEX_FILE_EXTENSION);
    filePlayHandle = NEXUS_FifoPlay_Open(liveStreamingSettings->fileName, indexFileName, liveStreamingSettings->fifoFileHandle);
    if (!filePlayHandle) {
        BDBG_ERR(("%s: NEXUS_FifoPlay_Open() failed to open circular file (%s) handle for live streaming, fifoFileHandle %p", 
                    __FUNCTION__, liveStreamingSettings->fileName, liveStreamingSettings->fifoFileHandle));
        goto error;
    }
    liveStreamingHandle->filePlayHandle = filePlayHandle;

    BDBG_WRN(("%s: handle %p, streamingFd %d, streaming buf %p", __FUNCTION__, liveStreamingHandle, streamingFd, liveStreamingHandle->streamingBuf));
    /* seek to begining of file */
    while (loopCount++ < 100) {
        if (filePlayHandle->file.data->seek(filePlayHandle->file.data, 0, SEEK_SET) != 0) {
            if (loopCount == 100) {
                BDBG_ERR(("%s: underflow, no data coming in, return failure, fd %p, loopCount %d", __FUNCTION__, filePlayHandle->file.data, loopCount));
                goto error;
            }
            else {
                BDBG_MSG(("%s: underflow: no data yet in the FIFO, retrying after small sleep, offset %d for fd %p, loopCount %d", __FUNCTION__, 0, filePlayHandle->file.data, loopCount));
            }
            BKNI_Sleep(130);
            continue;
        }
        break;
    }
    
    loopCount = 0;
    liveStreamingHandle->stop = false;
    liveStreamingHandle->threadRunning = true;
    liveStreamingHandle->connectionState = B_PlaybackIpConnectionState_eActive; 
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    cryptoAlignedSize = bufSize;
#endif

    if (liveStreamingHandle->startStreamingEvent)
        BKNI_SetEvent(liveStreamingHandle->startStreamingEvent);
    BDBG_WRN(("%s: Ready for streaming: handle %p, streamingFd %d", __FUNCTION__, liveStreamingHandle, streamingFd));
    while (!liveStreamingHandle->stop) {
        readBuf = liveStreamingHandle->streamingBuf;
        if (readTimeouts > 200) {
            liveStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
            BDBG_WRN(("%s: too many timeouts (cnt %d, time %d) while reading the live fifo, breaking out of streaming loop", __FUNCTION__, readTimeouts, (readTimeouts*100)/1000));
            break;
        }
        if (liveStreamingHandle->data.fd <= 0) {
            off_t curOffset;
            BDBG_MSG(("%s: streaming session is not yet enabled, skipping %d bytes", __FUNCTION__, bytesToWrite));
            streamingDisabled = true;
            curOffset = filePlayHandle->file.data->seek(filePlayHandle->file.data, 0, SEEK_CUR);
            BDBG_MSG(("cur offset %lld, ts aligned mod %d, aligned %lld", curOffset, curOffset%188, curOffset - curOffset%188));
        }
        else {
            if (streamingDisabled) {
                off_t curOffset;
                int offset;
                /* streaming was disabled, but is now being enabled */
                streamingDisabled = false;
                /* reset the file offset to TS friendly point and resume streaming from there */
                curOffset = filePlayHandle->file.data->seek(filePlayHandle->file.data, 0, SEEK_CUR);
                BDBG_WRN(("enable streaming: cur offset %lld, ts aligned mod %d, aligned %lld", curOffset, curOffset%188, curOffset - curOffset%188));
                /* calculate offset to go back */
                offset = curOffset%(512*188); /* to make the offset DIO & TS aligned */
#if 1
                offset += (512*188*10*4); /* goback additional few seconds so as to allow quick TSM lock at the client */
#endif
                if (filePlayHandle->file.data->seek(filePlayHandle->file.data, -curOffset, SEEK_CUR) < 0) {
                    BDBG_ERR(("%s: failed to shift back the current ffset by %d amount, errno %d", __FUNCTION__, curOffset, errno));
                    break;
                }
                curOffset = filePlayHandle->file.data->seek(filePlayHandle->file.data, 0, SEEK_CUR);
                BDBG_WRN(("enable streaming: aligned cur offset %lld by %d bytes ", curOffset, offset));
            }
        }
        BDBG_MSG(("readBUf %p, bytesToRead %d\n", readBuf, bytesToRead));
        if ((bytesRead = (int)filePlayHandle->file.data->read(filePlayHandle->file.data, (void *)readBuf, (size_t)bytesToRead)) <= 0) {
            BDBG_MSG(("%s: read returned %d errno %d\n", __FUNCTION__, bytesRead, errno));
            if (errno == EINTR) {
                BDBG_WRN(("%s: read Interrupted, retrying errno %d\n", __FUNCTION__, errno));
                BKNI_Sleep(100);
                continue;
            }
            if (bytesRead == 0) {
                /* reached file end */
                BDBG_MSG(("%s: reached EOF for circular file, sleep for 1 second and then retry reading", __FUNCTION__));
                readTimeouts++;
                BKNI_Sleep(100);
                continue;
            }
            else {
                BDBG_WRN(("%s: read error for circular file fd %p, rewinding to begining", __FUNCTION__, filePlayHandle->file.data));
                readTimeouts++;
                BKNI_Sleep(100);
                filePlayHandle->file.index->seek(filePlayHandle->file.index, 0, SEEK_SET);
                filePlayHandle->file.data->seek(filePlayHandle->file.data, 0, SEEK_SET);
                continue;
            }
        }
        if (firstTime) {
            BDBG_WRN(("%s: bytesRead %d bytesToRead %d, sd %d", __FUNCTION__, bytesRead, bytesToRead, streamingFd));
            firstTime = false;
        }
        readTimeouts = 0;
        if (bytesRead != bytesToRead)
            BDBG_MSG(("%s: bytesRead %d bytesToRead %d, sd %d, aligned %d", __FUNCTION__, bytesRead, bytesToRead, streamingFd, bytesRead % 188 ));
        bytesToWrite = bytesRead;
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
        if (liveStreamingHandle->data.pvrDecKeyHandle) {
            /* make sure that buffer size is MPEG2 TS, DIO, & AES Block size aligned */
            /* move past the bytes that were re-read to be dio compliant */
            cryptoUnAlignedBytes = bytesRead % cryptoAlignedSize;
            if (cryptoUnAlignedBytes) {
                /* read amount is not crypto aligned, so trim the read and move the file position back by that much */
                BDBG_MSG(("%s: bytesRead %d are not crypto size %d aligned, trim by %d bytes", __FUNCTION__, bytesRead, cryptoAlignedSize, cryptoUnAlignedBytes));
                bytesRead -= cryptoUnAlignedBytes;
                if (filePlayHandle->file.data->seek(filePlayHandle->file.data, -cryptoUnAlignedBytes, SEEK_CUR) < 0) {
                    BDBG_ERR(("%s: failed to shift back the current cryptoUnAlignedBytes by %d amount, errno %d", __FUNCTION__, cryptoUnAlignedBytes, errno));
                    break;
                }
                else
                    BDBG_MSG(("%s: shift back the file position by  %d offsets, crypto aligned bytesRead %d", __FUNCTION__, cryptoUnAlignedBytes, bytesRead));
                if (bytesRead == 0) {
                    BKNI_Sleep(20);
                    continue;
                }
            }
            /* decrypt the buffer */
            if (B_PlaybackIp_UtilsPvrDecryptBuffer(&liveStreamingHandle->data, readBuf, clearBuf, bytesRead, &bytesToWrite) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: PVR Decryption Failed", __FUNCTION__));
                break;
            }
            readBuf = clearBuf;
            BDBG_MSG(("%s: decrypting of %d bytes buffer done", __FUNCTION__, bytesRead));
        }
#endif
        bytesWritten = B_PlaybackIp_UtilsStreamingCtxWriteAll((struct bfile_io_write *)&liveStreamingHandle->data, readBuf, bytesToWrite);
        if (bytesWritten <= 0) {
            /* this happens if client closed the connection or client connection is dead */
            BDBG_MSG(("%s: failed to write %d bytes for streaming fd %d, wrote %d bytes, errno %d", __FUNCTION__, bytesToWrite, streamingFd, bytesWritten, errno));
            liveStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
            break;
        }
        BDBG_MSG(("%s: wrote %d bytes for streaming fd %d, bytesToRead %d, bytesRead %d, sync %x", 
                    __FUNCTION__, bytesWritten, streamingFd, bytesToRead, bytesRead, *(readBuf)));
        liveStreamingHandle->totalBytesStreamed += bytesWritten;
        loopCount++;
        readBuf = liveStreamingHandle->streamingBuf;
    }

error:
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    if (liveStreamingHandle->clearBuf) 
        NEXUS_Memory_Free(liveStreamingHandle->clearBuf);
#endif
    if (liveStreamingSettings->eventCallback && !liveStreamingHandle->stop) {
        /* app has defined eventCallback function & hasn't yet issued the stop, invoke the callback */
        BDBG_WRN(("%s: invoking End of Streaming callback for ctx %p", __FUNCTION__, liveStreamingHandle));
        liveStreamingSettings->eventCallback(liveStreamingSettings->appCtx, B_PlaybackIpEvent_eServerEndofStreamReached);
    }
    BDBG_WRN(("%s: Done: handle %p, streamed %lld bytes for streaming fd %d in %d send calls", __FUNCTION__, liveStreamingHandle, liveStreamingHandle->totalBytesStreamed, streamingFd, loopCount));
    if (liveStreamingHandle->streamingBufOrig) 
        NEXUS_Memory_Free(liveStreamingHandle->streamingBufOrig);
    if (liveStreamingHandle->stopStreamingEvent)
        BKNI_SetEvent(liveStreamingHandle->stopStreamingEvent);
    liveStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
    liveStreamingHandle->threadRunning = false;
    return;
}

/* Non timehisft case */
static void 
liveStreamingThreadFromRaveBuffer(
    void *data
    )
{
    int rc;
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle = (B_PlaybackIpLiveStreamingHandle)data;
    B_PlaybackIpLiveStreamingOpenSettings *liveStreamingSettings;
    void *readBuf = NULL;
    size_t clearBufferSize;
    unsigned * indexBuf = NULL;
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    unsigned char *clearBuf;
#endif
    size_t bytesRead = 0, bytesToRead, indexBytesRead, totalBytesRead;
    int streamingFd;
    int bytesWritten = 0, bytesToWrite;
    unsigned int loopCount = 0;
    int readTimeouts = 0;
    bool firstTime = true;
    NEXUS_RecpumpStatus status;
    unsigned long long bytesRecordedTillPreviousGop = 0;
    unsigned long long bytesRecordedTillCurrentGop = 0;
    int gopsSentInHlsSegment = 0;
    FILE *fclear = NULL;
    static int fileNameSuffix = 0;
    char recordFileName[32];
    bool gotErrorInStreamingLoop = false;
    B_PlaybackIpEventIds eventId = B_PlaybackIpEvent_eServerEndofStreamReached;
#if 0
#define ENABLE_SW_PACING
#endif
#ifdef ENABLE_SW_PACING
    double rate;
    struct timeval start, stop;
    double dt=0, maxrate;
#endif


    BDBG_ASSERT(liveStreamingHandle);
    loopCount = 0;
    liveStreamingSettings = &liveStreamingHandle->settings;
    streamingFd = liveStreamingSettings->streamingFd;
    liveStreamingHandle->connectionState = B_PlaybackIpConnectionState_eActive;

    if (enableRecording) {
        memset(recordFileName, 0, sizeof(recordFileName));
        snprintf(recordFileName, sizeof(recordFileName)-1, "./videos/rec%d_0.ts", fileNameSuffix++);
        fclear = fopen(recordFileName, "w+b");
    }
    /* setup bytes to read: account for transport timestamp & aes encryption */
    if (liveStreamingSettings->transportTimestampEnabled)
        clearBufferSize = (TS_PKT_SIZE+4) * HTTP_AES_BLOCK_SIZE * STREAMING_BUF_MULTIPLE * 4;
    else
        clearBufferSize = TS_PKT_SIZE * HTTP_AES_BLOCK_SIZE * STREAMING_BUF_MULTIPLE * 4;
    
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    if (liveStreamingHandle->data.pvrDecKeyHandle) {
        NEXUS_MemoryAllocationSettings allocSettings;
        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        if (liveStreamingSettings->heapHandle)
            allocSettings.heap = liveStreamingSettings->heapHandle;
        if (NEXUS_Memory_Allocate(clearBufferSize, &allocSettings, (void *)(&clearBuf))) {
            BDBG_ERR(("%s: memory allocation failure at %d\n", __FUNCTION__, __LINE__));
            goto error;
        }
        liveStreamingHandle->clearBuf = clearBuf;
    }
#endif

    /* let the main thread know that we have started */
    liveStreamingHandle->threadRunning = true;
    liveStreamingHandle->stop = false;
    if (liveStreamingHandle->startStreamingEvent)
        BKNI_SetEvent(liveStreamingHandle->startStreamingEvent);
    BDBG_WRN(("%s: Ready for streaming: handle %p, streamingFd %d", __FUNCTION__, liveStreamingHandle, streamingFd));

#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    if (liveStreamingSettings->appHeader.valid) {
        BDBG_MSG(("%s: appHeader Enabled %d, length %d", __FUNCTION__, liveStreamingSettings->appHeader.valid, liveStreamingSettings->appHeader.length));
        BDBG_MSG(("appHeader data: %x data: %x data: %x data: %x ", liveStreamingSettings->appHeader.data[0], liveStreamingSettings->appHeader.data[1], liveStreamingSettings->appHeader.data[2], liveStreamingSettings->appHeader.data[3]));
        bytesWritten = B_PlaybackIp_UtilsStreamingCtxWriteAll((struct bfile_io_write *)&liveStreamingHandle->data, liveStreamingSettings->appHeader.data, liveStreamingSettings->appHeader.length);
        if (bytesWritten != (int)liveStreamingSettings->appHeader.length) {
            /* this happens if client closed the connection or client connection is dead */
            BDBG_WRN(("%s: handle: %p, failed to write %d bytes of app header data of len %d for fd %d", __FUNCTION__, liveStreamingHandle, bytesWritten, liveStreamingSettings->appHeader.length, streamingFd));
            goto error;
        }
        BDBG_MSG(("%s: handle: %p, wrote %d bytes of app header data of len %d for fd %d", __FUNCTION__, liveStreamingHandle, bytesWritten, liveStreamingSettings->appHeader.length, streamingFd));
    }
#endif

#ifdef ENABLE_SW_PACING
    gettimeofday(&start, NULL);
    maxrate = 13.;
#endif
    while (!liveStreamingHandle->stop) {
#define LIVE_STREAMING_DATA_READY_TIMEOUT_INTERVAL 260
#define LIVE_STREAMING_DATA_READY_MAX_TIMEOUT 5000
        rc = BKNI_WaitForEvent(liveStreamingHandle->dataReadyEvent, LIVE_STREAMING_DATA_READY_TIMEOUT_INTERVAL);
        if (liveStreamingHandle->stop) {
            BDBG_WRN(("%s: app asked us to stop streaming (handle %p)", __FUNCTION__, liveStreamingHandle));
            break;
        }

        if (rc != 0 && rc != BERR_TIMEOUT) {
            /* error case, we are done */
            BDBG_WRN(("%s: dataReadyEvent ERROR (%d), breaking out of streaming loop ", __FUNCTION__, rc));
            break;
        }
        BKNI_ResetEvent(liveStreamingHandle->dataReadyEvent);

        /* check for read timeouts */
        if (readTimeouts * LIVE_STREAMING_DATA_READY_TIMEOUT_INTERVAL > LIVE_STREAMING_DATA_READY_MAX_TIMEOUT) {
            BDBG_ERR(("%s: handle %p, too many timeouts (cnt %d, time %d) while waiting for live data from recpump for streamingFd %d, breaking out of streaming loop", 
                        __FUNCTION__, liveStreamingHandle, readTimeouts, LIVE_STREAMING_DATA_READY_MAX_TIMEOUT, streamingFd));
            break;
        }
        /* determine how many bytes to read from the rave buffer */
        /* for HLS sessions, we read GOP at a time */
        if (liveStreamingSettings->hlsSession) {
            /* either we had a timeout or we actually got the data ready event: try reading */
            if (NEXUS_Recpump_GetIndexBuffer(liveStreamingHandle->recpumpHandle, (const void **)&indexBuf, (size_t *)&indexBytesRead) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Recpump_GetIndexBuffer failed, breaking out of streaming loop", __FUNCTION__));
                break;
            }
#define BRCM_TPIT_ENTRY_SIZE 24 /* Each TPIT entry is 6 dwords: 24 bytes */
            if (indexBuf && indexBytesRead) {
                unsigned long long highByte;
                /* we got some data, reset the readTimeout counter */
                readTimeouts = 0;
                if (firstTime) {
                    if (indexBytesRead <= 2*BRCM_TPIT_ENTRY_SIZE) {
                        /* wait until first two full GOPs are available in rave */
                        BDBG_MSG(("%s: waiting until first 3 full GOPs are received", __FUNCTION__));
                        continue;
                    }
                    else {
                        /* got atleast 1st 2 GOPs, so get the bytesRecordedTill last gop */
                        BDBG_ASSERT(indexBytesRead%BRCM_TPIT_ENTRY_SIZE==0);
                        BDBG_MSG(("%s: received 1st 3 GOPs, last RAP entry %p", __FUNCTION__, (unsigned *)((unsigned)indexBuf + (indexBytesRead - BRCM_TPIT_ENTRY_SIZE))));
                        indexBuf = (unsigned *)((unsigned)indexBuf + (indexBytesRead - BRCM_TPIT_ENTRY_SIZE));
                    }
                }
                /* accumulated upto 2nd Random Access Indicator, i.e. one full GOP */
                firstTime = false;
                /* calculate the GOP size, so that only that many bytes are read from rave and streamed to the client */
                highByte = ((unsigned long long)*(indexBuf+2) >> 24);
                bytesRecordedTillCurrentGop = highByte << 32;
                BDBG_MSG(("bytesRecordedTillCurrentGop %llu", bytesRecordedTillCurrentGop));
                bytesRecordedTillCurrentGop |= (unsigned long long)*(indexBuf+3);
                BDBG_MSG(("final bytesRecordedTillCurrentGop %llu", bytesRecordedTillCurrentGop));
                bytesToRead = bytesRecordedTillCurrentGop  - bytesRecordedTillPreviousGop;
                bytesRecordedTillPreviousGop = bytesRecordedTillCurrentGop;
                BDBG_MSG(("%s: CTX %p: Got full GOP: index bytesRead %d, bytesToRead %d, streaming fd %d", __FUNCTION__, liveStreamingHandle, indexBytesRead, bytesToRead, streamingFd));

#ifdef DEBUG
                BDBG_MSG(("%s: GOT full GOP: index bytesRead %d, tipt[0] 0x%x, tpit[2] 0x%x, tpit[3] 0x%x, bytesToRead %d", __FUNCTION__, indexBytesRead, *indexBuf, *(indexBuf+2), *(indexBuf+3), bytesToRead));
                NEXUS_Recpump_GetStatus(liveStreamingHandle->recpumpHandle, &status);
                BDBG_WRN(("%s: streamingFd %d: Recpump status: FIFO: depth %d, size %d", __FUNCTION__, streamingFd, status.data.fifoDepth, status.data.fifoSize));
#endif
            }
            else {
                readTimeouts++;
                BDBG_MSG(("%s: handle %p, for streamingFd %d, readTimeouts %d, wait until 1 full GOP is received", __FUNCTION__, liveStreamingHandle, streamingFd, readTimeouts));
                continue;
            }
        }
        else {
            /* non-HLS session: try to read upto one clearBufferSize */
            bytesToRead = clearBufferSize;
        }

        totalBytesRead = 0;
        while (totalBytesRead < bytesToRead) {
            if (liveStreamingHandle->stop) {
                BDBG_WRN(("%s: app asked us to stop streaming (handle %p)", __FUNCTION__, liveStreamingHandle));
                gotErrorInStreamingLoop = true;
                break;
            }

            /* read the remaining bytes in this GOP and stream them out */
            if (NEXUS_Recpump_GetDataBuffer(liveStreamingHandle->recpumpHandle, (const void **)&readBuf, (size_t *)&bytesRead) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Recpump_GetDataBuffer failed, breaking out of streaming loop", __FUNCTION__));
                gotErrorInStreamingLoop = true;
                break;
            }
            
            if (readBuf == NULL || bytesRead <= 0) {
                readTimeouts++;
                /* break out of this inner loop, but check after this loop will again try to read more data */
                break;
            }
            readTimeouts = 0;

            if (bytesRead > (bytesToRead-totalBytesRead)) 
                /* read more than the GOP size, so trim it */
                bytesRead = bytesToRead-totalBytesRead;
            if (bytesRead > clearBufferSize)
                /* read more than the buffer size, so trim it (as we try to write only a small size buffer to keep low latency) */
                bytesRead = clearBufferSize;
            BDBG_MSG(("%s: bytesToRead %d, bytesRead %d, totalBytesRead %d", __FUNCTION__, bytesToRead, bytesRead, totalBytesRead));

            if (!liveStreamingSettings->hlsSession) {
                /* only send AES block size aligned bytes, its is fine for continous live streaming */
                if (liveStreamingSettings->protocol == B_PlaybackIpProtocol_eHttp) {
                    bytesRead -= bytesRead%HTTP_AES_BLOCK_SIZE; 
                }
#if 0
                /* let it send whatever amount of data we get for now and not worry about any size alignment */
                else {
                    bytesRead -= bytesRead%1316; 
                }
#endif
                bytesToRead = bytesRead;
                BDBG_MSG(("%s: bytesToRead %d, adjusted bytesRead %d, totalBytesRead %d", __FUNCTION__, bytesToRead, bytesRead, totalBytesRead));
                if (bytesToRead <= 0) {
                    BDBG_MSG(("%s: recpump underflow (bytesRead %d), sleeping for 100msec...", __FUNCTION__, bytesRead));
                    continue;
                }
            }
            /* NOTE: else for HLS if security is enabled, need to handle the case where data read is not AES block aligned */
            /* the issue is that last few bytes of a GOP may not be AES block size aligned and thus we will need to insert padding */

            /* send bytes just read */
            bytesToWrite = bytesRead;
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
            if (liveStreamingHandle->data.pvrDecKeyHandle) {
                /* decrypt the buffer first */
                if (B_PlaybackIp_UtilsPvrDecryptBuffer(&liveStreamingHandle->data, readBuf, clearBuf, bytesRead, &bytesToWrite) != B_ERROR_SUCCESS) {
                    BDBG_ERR(("%s: handle %p, PVR Decryption Failed", __FUNCTION__, liveStreamingHandle));
                    gotErrorInStreamingLoop = true;
                    break;
                }
                readBuf = clearBuf;
                BDBG_MSG(("%s: decrypting of %d bytes buffer done, bytesToWrite %d", __FUNCTION__, bytesRead, bytesToWrite));
            }
#endif
            bytesWritten = B_PlaybackIp_UtilsStreamingCtxWriteAll((struct bfile_io_write *)&liveStreamingHandle->data, readBuf, bytesToWrite);
            if (bytesWritten <= 0) {
                /* this happens if client closed the connection or client connection is dead */
                BDBG_WRN(("%s: failed to write %d bytes for streaming fd %d, wrote %d bytes, errno %d, streamed %lld bytes in %d calls", __FUNCTION__, bytesToWrite, streamingFd, bytesWritten, errno, liveStreamingHandle->totalBytesStreamed, loopCount));
                gotErrorInStreamingLoop = true;
                liveStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
                if (NEXUS_Recpump_DataReadComplete(liveStreamingHandle->recpumpHandle, 0) != NEXUS_SUCCESS) {
                    BDBG_ERR(("%s: NEXUS_Recpump_DataReadComplete failed, breaking out of streaming loop", __FUNCTION__));
                    break;
                }
                break;
            }
            /* write data to file */
            if (enableRecording && fclear) {
                fwrite(readBuf, 1, bytesToWrite, fclear); 
            }

            /* tell rave that we have consumed bytesWritten amount of data */
            if (NEXUS_Recpump_DataReadComplete(liveStreamingHandle->recpumpHandle, bytesWritten) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Recpump_DataReadComplete failed, breaking out of streaming loop", __FUNCTION__));
                gotErrorInStreamingLoop = true;
                break;
            }
            totalBytesRead += bytesRead;
            liveStreamingHandle->totalBytesStreamed += bytesWritten;
            loopCount++;
        }

        if (readTimeouts) {
            /* didn't get any data, retry */
            BDBG_MSG(("%s: handle %p, for streamingFd %d, readTimeouts %d, wait until more data is received", __FUNCTION__, liveStreamingHandle, streamingFd, readTimeouts));
            continue;
        }
        if (gotErrorInStreamingLoop == true) {
            BDBG_MSG(("%s: gotErrorInStreamingLoop: breaking out of streaming loop", __FUNCTION__));
            break;
        }

        BDBG_MSG(("%s: bytesToRead %d bytes for streaming fd %d, wrote %d bytes", __FUNCTION__, bytesToRead, streamingFd, totalBytesRead));
        if (liveStreamingSettings->hlsSession) {
            gopsSentInHlsSegment++;
            if (NEXUS_Recpump_IndexReadComplete(liveStreamingHandle->recpumpHandle, indexBytesRead) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Recpump_IndexReadComplete failed, breaking out of streaming loop", __FUNCTION__));
                break;
            }
            BDBG_MSG(("%s: gopsSentInHlsSegment %d", __FUNCTION__, gopsSentInHlsSegment));
#define GOPS_IN_A_SEGMENT 3 /* pass it from app */
            if (gopsSentInHlsSegment % GOPS_IN_A_SEGMENT == 0/*liveStreamingSettings->hlsSegmentSize*/) {
                /* 3 GOPs in a segment */
                BDBG_MSG(("%s: finished writing one segment, # of GOPs sent %d, closing current socket fd %d and waiting for client to request new segment", __FUNCTION__, gopsSentInHlsSegment, streamingFd));
                /* we have to close the socket here as that is the only way client knows to request the next segment */
                shutdown(streamingFd, 2);
                close(streamingFd);
                if (liveStreamingSettings->eventCallback && !liveStreamingHandle->stop) {
                    /* app has defined eventCallback function & hasn't yet issued the stop, invoke the callback */
                    BDBG_MSG(("%s: invoking End of segment callback for ctx %p", __FUNCTION__, liveStreamingHandle));
                    liveStreamingSettings->eventCallback(liveStreamingSettings->appCtx, B_PlaybackIpEvent_eServerEndofSegmentReached);
                }
                if (fclear) {
                    fflush(fclear);
                    fclose(fclear);
                    snprintf(recordFileName, sizeof(recordFileName)-1, "./videos/rec%d_%d.ts", fileNameSuffix, gopsSentInHlsSegment/GOPS_IN_A_SEGMENT);
                    fclear = fopen(recordFileName, "w+b");
                }
                /* wait for app to resume steaming, otherwise timeout and close */
                rc = BKNI_WaitForEvent(liveStreamingHandle->startStreamingEvent, 1000);
                if (rc == BERR_TIMEOUT || rc != 0) {
                    BDBG_MSG(("%s: resume streaming event timedout or error on event, handle %p, client is done w/ HLS session", __FUNCTION__, liveStreamingHandle));
                    eventId = B_PlaybackIpEvent_eServerStartStreamingTimedout;
                    break;
                }
                streamingFd = liveStreamingHandle->data.fd; /* updated by the SetSettings function */
                BDBG_MSG(("%s: CTX %p: resume streaming on fd %d", __FUNCTION__, liveStreamingHandle, streamingFd));
            }
        }

        /* look for recpump overflow before returning to top of the loop */
        NEXUS_Recpump_GetStatus(liveStreamingHandle->recpumpHandle, &status);
        if (loopCount % 50 == 0) {
            BDBG_MSG(("%s: streamingFd %d: Recpump status: FIFO: depth %d, size %d, read %d, written %d, totolBytesStreamd %lu", __FUNCTION__, streamingFd, status.data.fifoDepth, status.data.fifoSize, bytesRead, bytesWritten, liveStreamingHandle->totalBytesStreamed));
        }
        if (status.data.fifoDepth >= .95 * status.data.fifoSize) {
            if (NEXUS_Recpump_GetDataBuffer(liveStreamingHandle->recpumpHandle, (const void **)&readBuf, (size_t *)&bytesRead) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Recpump_GetDataBuffer failed, breaking out of streaming loop", __FUNCTION__));
                break;
            }
            BDBG_WRN(("%s: streamingFd %d: Recpump overflow, flushing the FIFO: depth %d, size %d, read %d", __FUNCTION__, streamingFd, status.data.fifoDepth, status.data.fifoSize, bytesRead));
            if (NEXUS_Recpump_DataReadComplete(liveStreamingHandle->recpumpHandle, bytesRead) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Recpump_DataReadComplete failed, breaking out of streaming loop", __FUNCTION__));
                break;
            }
            BKNI_Sleep(50); /* wait a bit for new data to accumulate in recpump FIFO */
        }
#ifdef ENABLE_SW_PACING
        gettimeofday(&stop, NULL);
        dt = difftime1(&start, &stop);
        rate = 8.*liveStreamingHandle->totalBytesStreamed/dt;
        if ((loopCount % 50) == 0) {
            BDBG_MSG(("[%6lu] Wrote %10lu Bytes in dt = %12.2fusec at rate=%2.1f/%2.1f\n", loopCount, liveStreamingHandle->totalBytesStreamed, dt, rate, maxrate));
        }
        while (rate > maxrate) {
            usleep(10000);
            gettimeofday(&stop, NULL);
            dt = difftime1(&start,&stop);
            rate = 8.*liveStreamingHandle->totalBytesStreamed/dt;
        }

#endif
    }
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
error:
    if (liveStreamingHandle->clearBuf) NEXUS_Memory_Free(liveStreamingHandle->clearBuf);
#endif
    if (liveStreamingSettings->eventCallback && !liveStreamingHandle->stop) {
        /* app has defined eventCallback function & hasn't yet issued the stop, invoke the callback */
        BDBG_MSG(("%s: invoking End of Streaming callback for ctx %p", __FUNCTION__, liveStreamingHandle));
        liveStreamingSettings->eventCallback(liveStreamingSettings->appCtx, eventId);
    }
        if (enableRecording && fclear) {
            fflush(fclear);
            fclose(fclear);
        }
    BDBG_MSG(("%s: Done: handle %p, streamed %lld bytes for streaming fd %d in %d send calls ", __FUNCTION__, liveStreamingHandle, liveStreamingHandle->totalBytesStreamed, streamingFd, loopCount));
    if (liveStreamingHandle->stopStreamingEvent)
        BKNI_SetEvent(liveStreamingHandle->stopStreamingEvent);
    liveStreamingHandle->connectionState = B_PlaybackIpConnectionState_eError;
    liveStreamingHandle->threadRunning = false;
    return;
}

/***************************************************************************
Summary:
This function starts streaming content from a file.
***************************************************************************/
 
B_PlaybackIpError
B_PlaybackIp_LiveStreamingSetSettings(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle,
    B_PlaybackIpLiveStreamingSettings *settings
    )
{
    B_PlaybackIpLiveStreamingOpenSettings *liveStreamingSettings = &liveStreamingHandle->settings;

    BDBG_MSG(("%s: proto %d", __FUNCTION__, liveStreamingSettings->protocol));
    switch (liveStreamingSettings->protocol) {
    case B_PlaybackIpProtocol_eHttp: {
        if (settings->streamingEnabled) {
            liveStreamingHandle->data.fd = settings->streamingFd;
            liveStreamingHandle->data.self = net_io_data_write;
            if (B_PlaybackIp_UtilsStreamingCtxOpen(&liveStreamingSettings->securitySettings, &liveStreamingHandle->data) != B_ERROR_SUCCESS) {
                BDBG_ERR(("Failed to setup the streaming context\n"));
                return B_ERROR_UNKNOWN;
            }
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
            if (B_PlaybackIp_UtilsPvrDecryptionCtxOpen(&liveStreamingSettings->securitySettings, &liveStreamingHandle->data) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Failed to setup pvr decryption"));
                return B_ERROR_UNKNOWN;
            }
#endif
            BDBG_MSG(("%s: complete, streaming socket %d", __FUNCTION__, liveStreamingHandle->data.fd));
        }
        else if (settings->resumeStreaming && liveStreamingSettings->hlsSession) {
            liveStreamingHandle->data.fd = settings->streamingFd;
            if (settings->hlsSegmentSize > 0)
                liveStreamingSettings->hlsSegmentSize = settings->hlsSegmentSize;
            if (liveStreamingHandle->startStreamingEvent)
                BKNI_SetEvent(liveStreamingHandle->startStreamingEvent);
            else {
                BDBG_ERR(("%s: CTX %p: startStreamingEvent is NULL for new fd %d, returning error", __FUNCTION__, liveStreamingHandle, settings->streamingFd));
                return B_ERROR_UNKNOWN;
            }
            BDBG_MSG(("%s: CTX %p: Generated event to resume streaming on fd %d, hlsSegmentSize %d", __FUNCTION__, liveStreamingHandle, settings->streamingFd, liveStreamingSettings->hlsSegmentSize));
        }
        break;
        }
    default:
        BDBG_ERR((" non-supported socket type"));
        return B_ERROR_UNKNOWN;
    }

    return B_ERROR_SUCCESS;
}

B_PlaybackIpError
B_PlaybackIp_LiveStreamingStart(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle
    )
{
    BERR_Code rc;
    B_ThreadFunc liveStreamingThreadFunc;
    B_PlaybackIpLiveStreamingOpenSettings *liveStreamingSettings = &liveStreamingHandle->settings;

    switch (liveStreamingSettings->protocol) {
    case B_PlaybackIpProtocol_eHttp: {
        if (liveStreamingSettings->streamingDisabled) {
            BDBG_MSG(("%s: streaming fd is not yet set for this session (%p), defer streaming setup", __FUNCTION__, liveStreamingHandle));
            liveStreamingHandle->data.fd = -1;
            liveStreamingHandle->data.self = net_io_data_write;
            break;
        }
        liveStreamingHandle->data.fd = liveStreamingSettings->streamingFd;
        liveStreamingHandle->data.streamingProtocol = B_PlaybackIpProtocol_eHttp;
        liveStreamingHandle->data.self = net_io_data_write;
        liveStreamingHandle->data.liveStreaming = true;
        if (B_PlaybackIp_UtilsStreamingCtxOpen(&liveStreamingSettings->securitySettings, &liveStreamingHandle->data) != B_ERROR_SUCCESS) {
            BDBG_ERR(("Failed to setup the streaming context\n"));
            return B_ERROR_UNKNOWN;
        }
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
        if (B_PlaybackIp_UtilsPvrDecryptionCtxOpen(&liveStreamingSettings->securitySettings, &liveStreamingHandle->data) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: Failed to setup pvr decryption"));
            return B_ERROR_UNKNOWN;
        }
#endif
        break;
        }
        case B_PlaybackIpProtocol_eRtp: 
        case B_PlaybackIpProtocol_eUdp: {
            liveStreamingHandle->data.interfaceName = liveStreamingSettings->rtpUdpSettings.interfaceName;
            liveStreamingHandle->data.streamingProtocol = liveStreamingSettings->protocol;
            liveStreamingHandle->data.self = net_io_data_write;
            liveStreamingHandle->data.streamingSockAddr.sin_family = AF_INET;
            liveStreamingHandle->data.streamingSockAddr.sin_port = htons(liveStreamingSettings->rtpUdpSettings.streamingPort);
            if (inet_aton(liveStreamingSettings->rtpUdpSettings.streamingIpAddress, &liveStreamingHandle->data.streamingSockAddr.sin_addr) == 0) {
                BDBG_ERR(("%s: inet_aton() failed on %s", __FUNCTION__, liveStreamingSettings->rtpUdpSettings.streamingIpAddress));
                goto error;
            }
            BDBG_WRN(("Streaming URL is %s%s:%d", liveStreamingSettings->protocol == B_PlaybackIpProtocol_eRtp? "rtp://":"udp://",
                        inet_ntoa(liveStreamingHandle->data.streamingSockAddr.sin_addr), ntohs(liveStreamingHandle->data.streamingSockAddr.sin_port)));

            if (B_PlaybackIp_UtilsRtpUdpStreamingCtxOpen(&liveStreamingHandle->settings.securitySettings, &liveStreamingHandle->data) != B_ERROR_SUCCESS) {
                BDBG_ERR(("Failed to setup the streaming context\n"));
                goto error;
            }
            B_PlaybackIp_UtilsSetRtpPayloadType(liveStreamingSettings->psi->mpegType, &liveStreamingHandle->data.rtpPayloadType);
            BDBG_MSG(("%s: complete for RTP/UDP streaming, socket %d\n", __FUNCTION__, liveStreamingHandle->data.fd));
            break;
        }
    default:
        BDBG_ERR((" non-supported socket type"));
        break;
    }

    if (BKNI_CreateEvent(&liveStreamingHandle->startStreamingEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    if (BKNI_CreateEvent(&liveStreamingHandle->stopStreamingEvent)) {
        BDBG_ERR(("%s: Failed to create an event\n", __FUNCTION__));
        goto error;
    }
    if (liveStreamingSettings->enableTimeshifting)
        liveStreamingThreadFunc = liveStreamingThreadFromFifo;
    else
        liveStreamingThreadFunc = liveStreamingThreadFromRaveBuffer;
    liveStreamingHandle->streamingThread = B_Thread_Create("Live Streamer", (B_ThreadFunc)liveStreamingThreadFunc, (void *)liveStreamingHandle, NULL);
    if (liveStreamingHandle->streamingThread == NULL) {
        BDBG_ERR(("%s: Failed to create HTTP media file streaming thread \n", __FUNCTION__));
        goto error;
    }
    rc = BKNI_WaitForEvent(liveStreamingHandle->startStreamingEvent, 1000);
    if (rc == BERR_TIMEOUT) {
        BDBG_WRN(("%s: startStreamingEvent timed out", __FUNCTION__));
        goto error;
    }
    else if (rc != 0) {
        BDBG_ERR(("%s: failed to start the streaming thread", __FUNCTION__));
        goto error;
    }
    BKNI_ResetEvent(liveStreamingHandle->startStreamingEvent);

    BDBG_MSG(("%s: CTX %p: complete, streaming socket %d", __FUNCTION__, liveStreamingHandle, liveStreamingHandle->data.fd));
    return B_ERROR_SUCCESS;

error:
    B_PlaybackIp_LiveStreamingStop(liveStreamingHandle);
    B_PlaybackIp_UtilsStreamingCtxClose(&liveStreamingHandle->data);
    return B_ERROR_UNKNOWN;
}


/***************************************************************************
Summary:
This function stops streaming content from a file.
***************************************************************************/
void 
B_PlaybackIp_LiveStreamingStop(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle
    )
{
    BERR_Code rc;
    BDBG_MSG(("%s: %p", __FUNCTION__, (void *)liveStreamingHandle));
    BDBG_ASSERT(liveStreamingHandle);
    liveStreamingHandle->stop = true;

    if (liveStreamingHandle->stopStreamingEvent) {
		/* send an event to the feeder thread indicating */ 
		/* that it no longer needs to wait for data as app is stopping this session */
        if (liveStreamingHandle->dataReadyEvent)
            BKNI_SetEvent(liveStreamingHandle->dataReadyEvent);
        if (liveStreamingHandle->threadRunning) {
            rc = BKNI_WaitForEvent(liveStreamingHandle->stopStreamingEvent, 1000);
            if (rc == BERR_TIMEOUT) 
                BDBG_WRN(("%s: stopStreamingEvent timed out", __FUNCTION__));
            else
            if (rc != 0) {
                BDBG_ERR(("%s: failed to stop the file streaming thread", __FUNCTION__));
            }
            liveStreamingHandle->threadRunning = false;
        }
        BKNI_DestroyEvent(liveStreamingHandle->stopStreamingEvent);
        liveStreamingHandle->stopStreamingEvent = NULL;
    }
    switch (liveStreamingHandle->settings.protocol) {
        case B_PlaybackIpProtocol_eRtp: 
        case B_PlaybackIpProtocol_eUdp: {
            B_PlaybackIp_UtilsRtpUdpStreamingCtxClose(&liveStreamingHandle->data);
            break;
        }
        case B_PlaybackIpProtocol_eHttp:
        default:
            break;
    }
    if (liveStreamingHandle->startStreamingEvent) {
        BKNI_DestroyEvent(liveStreamingHandle->startStreamingEvent);
        liveStreamingHandle->startStreamingEvent = NULL;
    }
#if NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
    B_PlaybackIp_UtilsPvrDecryptionCtxClose(&liveStreamingHandle->data);
#endif
    if (liveStreamingHandle->streamingThread) {
        B_Thread_Destroy(liveStreamingHandle->streamingThread);
        liveStreamingHandle->streamingThread = NULL;
    }
    BDBG_MSG(("%s: DONE %p", __FUNCTION__, (void *)liveStreamingHandle));
    return;
}

void 
B_PlaybackIp_LiveStreamingGetStatus(
    B_PlaybackIpLiveStreamingHandle liveStreamingHandle,
    B_PlaybackIpLiveStreamingStatus *status 
    )
{
    BDBG_ASSERT(status);
    BDBG_ASSERT(liveStreamingHandle);
    status->bytesStreamed = liveStreamingHandle->totalBytesStreamed;
    status->connectionState = liveStreamingHandle->connectionState;
}

#endif /* LINUX || VxWorks */
