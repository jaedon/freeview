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
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELYn
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
 * $brcm_Workfile: ip_streamer_transcode.c $
 * $brcm_Revision: 18 $
 * $brcm_Date: 11/9/12 3:23p $
 *
 * Module Description: 
 *  main test app for ip_streamer
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/playback_ip/apps/ip_streamer_transcode.c $
 * 
 * 18   11/9/12 3:23p ssood
 * SW7425-4169: allow apps to pass in configurable nexus heap handle
 * 
 * 17   11/1/12 4:37p ssood
 * SW7231-858: coverity fixes
 * 
 * 16   10/25/12 10:24a piyushg
 * SW7435-452: Fixed compiler warning.
 * 
 * 15   10/24/12 4:53p piyushg
 * SW7435-452: Fixed compiler warning.
 * 
 * 14   10/2/12 8:00p ssood
 * SW7425-3970: added support to transcode from IP src using raaga encoder
 * 
 * 13   10/1/12 11:03a ssood
 * SW7425-3970: added support to transcode from IP src
 * 
 * 12   9/18/12 6:28p ssood
 * SW7435-361: move StcChannel_Open to StartTranscoder as it now requires
 *  pcrPidCh to be specified
 * 
 * 11   9/13/12 2:36p aginesh
 * SW7435-353: remove tabs
 * 
 * 10   9/13/12 11:24a aginesh
 * SW7435-353: changes to get ip_streamer to work with trellis
 * 
 * 9   8/15/12 5:26p ssood
 * SW7435-311: initialize display format to 720p even though it later
 *  modified using customerFormat settings
 * 
 * 8   7/19/12 1:10p ssood
 * SW7425-3494: add support for transcode and stream out from pre-recorded
 *  file using Vice2
 * 
 * 7   6/29/12 6:50p ssood
 * SW7346-897: raaga encoding changes for 7346 platform
 * 
 * 6   6/25/12 6:08p ssood
 * SW7435-250: coverity fixes exposed by new coverity version
 * 
 * 5   6/6/12 3:04p ssood
 * SW7425-3167: removed trackInput from NEXUS_VideoEncoderStreamStructure
 * 
 * 4   5/31/12 4:13p ssood
 * SW7425-3042: enable pcr pid insertion for xcode channels
 * 
 * 3   5/21/12 2:01p ssood
 * SW7425-3042: suppress debug logging by default
 * 
 * 2   5/17/12 6:36p ssood
 * SW7425-3042: simplified build flags & code by using nexus multi process
 *  capabilities
 * 
 * 1   5/14/12 4:32p ssood
 * SW7425-3042: refactor ip_streamer into multiple sub files
 * 
 *
 ******************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>

#include "bstd.h"
#include "bkni.h"
#include "blst_list.h"
#include "blst_queue.h"

#ifndef DMS_CROSS_PLATFORMS                                                                                              
#include "nexus_platform.h"                                                                                              
#if NEXUS_HAS_FRONTEND                                                                                                   
#include "nexus_frontend.h"                                                                                              
#endif                                                                                                                   
#include "nexus_parser_band.h"                                                                                           
#include "nexus_pid_channel.h"                                                                                           
#include "nexus_playpump.h"                                                                                              
#include "nexus_message.h"                                                                                               
#include "nexus_timebase.h"                                                                                              
#include "nexus_recpump.h"                                                                                               
#ifdef NEXUS_HAS_RECORD                                                                                                  
#include "nexus_record.h"                                                                                                
#endif                                                                                                                   
#include "nexus_file_fifo.h"                                                                                             
#include "b_psip_lib.h"                                                                                                  
#include "nexus_core_utils.h"                                                                                            
#endif /* DMS_CROSS_PLATFORMS */                                                                                         
#include "b_playback_ip_lib.h"                                                                                           

#ifdef B_HAS_DTCP_IP
#include "b_dtcp_applib.h"
#include "b_dtcp_constants.h"
#endif

#include "nexus_core_utils.h"
#include "ip_streamer_lib.h"
#include "ip_streamer.h"
#include "ip_http.h"
#include "b_os_lib.h"

#ifdef NEXUS_HAS_VIDEO_ENCODER
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_video_adj.h"
#include "nexus_audio_input.h"
#endif


BDBG_MODULE(ip_streamer);
#if 0
#define BDBG_MSG_FLOW(x)  BDBG_WRN( x );
#else
#define BDBG_MSG_FLOW(x)  
#endif

#define IP_NETWORK_JITTER 300
extern void closeNexusPidChannels(IpStreamerCtx *ipStreamerCtx);

#ifdef NEXUS_HAS_VIDEO_ENCODER
int
initNexusTranscoderDstList(
    IpStreamerGlobalCtx *ipStreamerGlobalCtx
    )
{
    int i;

    /* create the per dst list mutex */
    if (BKNI_CreateMutex(&ipStreamerGlobalCtx->transcoderDstMutex) != 0) {
        BDBG_ERR(("BKNI_CreateMutex failed at %d", __LINE__));
        return -1;
    }
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    for (i=0; i<NEXUS_NUM_DSP_VIDEO_ENCODERS; i++) 
#else
    for (i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) 
#endif
    {
        memset(&ipStreamerGlobalCtx->transcoderDstList[i], 0, sizeof(TranscoderDst));
        ipStreamerGlobalCtx->transcoderDstList[i].inUse = false;
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
        /* so far, RAAGA only has b/w to do 1 low res encode, that is why fixing it to 0 */
        ipStreamerGlobalCtx->transcoderDstList[i].displayIndex = 1;
#else
        ipStreamerGlobalCtx->transcoderDstList[i].displayIndex = NEXUS_ENCODER_DISPLAY_IDX-i;
#endif
         if (BKNI_CreateEvent(&ipStreamerGlobalCtx->transcoderDstList[i].finishEvent )) {
             BDBG_ERR(("Failed to create event at %d", __LINE__));
             return -1;
         }
    }

    BDBG_MSG(("%s: Transcoder Dst Initialized", __FUNCTION__));
    return 0;
}

void
unInitNexusTranscoderDstList(
    IpStreamerGlobalCtx *ipStreamerGlobalCtx
    )
{
    int i;

    if (ipStreamerGlobalCtx->transcoderDstMutex)
        BKNI_DestroyMutex(ipStreamerGlobalCtx->transcoderDstMutex);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    for (i=0; i<NEXUS_NUM_DSP_VIDEO_ENCODERS; i++) 
#else
    for (i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) 
#endif
    {
        if (ipStreamerGlobalCtx->transcoderDstList[i].finishEvent)
            BKNI_DestroyEvent(ipStreamerGlobalCtx->transcoderDstList[i].finishEvent);
    }
    BDBG_MSG(("%s: Transcoder Dst Un-Initialized", __FUNCTION__));
}
#endif /* NEXUS_HAS_VIDEO_ENCODER */

static void
transcoderFinishCallback(void *context, int param)
{
    TranscoderDst *transcoderDst = (TranscoderDst *)context;

    BSTD_UNUSED(param);
    BDBG_MSG(("Finish callback invoked on transcoder ctx %p, now stop the transcoder", transcoderDst));
    BKNI_SetEvent(transcoderDst->finishEvent);
}

#include "tshdrbuilder.h"
/* TSHDRBUILDER has one extra byte at the beginning to describe the variable length TS header buffer */
#define IP_STREAMER_TS_HEADER_BUF_LENGTH   189
static void 
B_IpStreamer_AddPatPmt(
    void *pat,
    void *pmt,
    uint16_t pmtPid,
    uint16_t pcrPid,
    uint16_t vidPid,
    uint16_t audioPid,
    uint8_t  videoStreamType,
    uint8_t  audioStreamType
    )
{
    uint8_t pat_pl_buf[IP_STREAMER_TS_HEADER_BUF_LENGTH], pmt_pl_buf[IP_STREAMER_TS_HEADER_BUF_LENGTH];
    size_t pat_pl_size, pmt_pl_size;
    size_t buf_used = 0;
    size_t payload_pked = 0;
    unsigned streamNum;

    TS_PAT_state patState;
    TS_PSI_header psi_header;
    TS_PMT_state pmtState;
    TS_PAT_program program;
    TS_PMT_stream pmt_stream;

    TS_PID_info pidInfo;
    TS_PID_state pidState;

    /* == CREATE PSI TABLES == */
    TS_PSI_header_Init(&psi_header);
    TS_PAT_Init(&patState, &psi_header, pat_pl_buf, IP_STREAMER_TS_HEADER_BUF_LENGTH);

    TS_PAT_program_Init(&program, 1, pmtPid);
    TS_PAT_addProgram(&patState, &pmtState, &program, pmt_pl_buf, IP_STREAMER_TS_HEADER_BUF_LENGTH);

    TS_PMT_setPcrPid(&pmtState, pcrPid);

    TS_PMT_stream_Init(&pmt_stream, videoStreamType, vidPid);
    TS_PMT_addStream(&pmtState, &pmt_stream, &streamNum);

    if (audioPid) {
        TS_PMT_stream_Init(&pmt_stream, audioStreamType, audioPid);
        TS_PMT_addStream(&pmtState, &pmt_stream, &streamNum);
    }

    TS_PAT_finalize(&patState, &pat_pl_size);
    TS_PMT_finalize(&pmtState, &pmt_pl_size);

    /* == CREATE TS HEADERS FOR PSI INFORMATION == */
    TS_PID_info_Init(&pidInfo, 0x0, 1);
    pidInfo.pointer_field = 1;
    TS_PID_state_Init(&pidState);
    TS_buildTSHeader(&pidInfo, &pidState, pat, IP_STREAMER_TS_HEADER_BUF_LENGTH, &buf_used, patState.size, &payload_pked, 1);
    BKNI_Memcpy((uint8_t*)pat + buf_used, pat_pl_buf, pat_pl_size);

    TS_PID_info_Init(&pidInfo, pmtPid, 1);
    pidInfo.pointer_field = 1;
    TS_PID_state_Init(&pidState);
    TS_buildTSHeader(&pidInfo, &pidState, pmt, IP_STREAMER_TS_HEADER_BUF_LENGTH, &buf_used, pmtState.size, &payload_pked, 1);
    BKNI_Memcpy((uint8_t*)pmt + buf_used, pmt_pl_buf, pmt_pl_size);
}

static void 
B_IpStreamer_InsertSystemDataTimer(
    void *context)
{
    NEXUS_Error rc = NEXUS_UNKNOWN;
    uint8_t ccByte;
    TranscoderDst *transcoderDst = context;

    if (!transcoderDst)
        return;

    ++transcoderDst->ccValue;/* increment CC synchronously with PAT/PMT */
    ccByte = *((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4); /* the 1st byte of pat/pmt arrays is for TSheader builder use */

    /* need to increment CC value for PAT/PMT packets */
    ccByte = (ccByte & 0xf0) | (transcoderDst->ccValue & 0xf);
    *((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4) = ccByte;
    /* need to flush the cache before set to TS mux hw */
    NEXUS_Memory_FlushCache((void*)((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4), 1);
    /* ping pong PAT pointer */
    transcoderDst->psi[0].pData = (void*)((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 1);

    ccByte = *((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4);
    ccByte = (ccByte & 0xf0) | (transcoderDst->ccValue & 0xf);
    *((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4) = ccByte;
    NEXUS_Memory_FlushCache((void*)((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4), 1);
    /* ping pong PMT pointer */
    transcoderDst->psi[1].pData = (void*)((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 1);

    rc = NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[0]);
    if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }
    rc = NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[1]);
    if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }
#if 0
    BDBG_MSG(("insert PAT&PMT... ccPAT = %x ccPMT=%x", *((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4) & 0xf,
                *((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue  % IP_STREAMER_PSI_QUEUE_CNT] + 4) & 0xf));
#endif
    if (transcoderDst->systemDataTimerIsStarted) {
        transcoderDst->systemDataTimer = B_Scheduler_StartTimer(
                transcoderDst->schedulerSystemData,transcoderDst->mutexSystemData, 1000, B_IpStreamer_InsertSystemDataTimer, transcoderDst);
        if (transcoderDst->systemDataTimer==NULL) {
            BDBG_ERR(("%s: Failed to schedule timer to periodically insert PAT/PMT", __FUNCTION__));
            goto error;
        }
    }
error:
    return;
}

static void 
B_IpStreamer_StopSystemData(
    IpStreamerCtx *ipStreamerCtx)
{
    unsigned i;
    TranscoderDst *transcoderDst;

    BDBG_MSG(("%s: enter", __FUNCTION__));
    if (!ipStreamerCtx || !(transcoderDst = ipStreamerCtx->transcoderDst))
        return;

    /* cancel system data timer */
    if (transcoderDst->systemDataTimerIsStarted) {
        B_Scheduler_CancelTimer(transcoderDst->schedulerSystemData, transcoderDst->systemDataTimer);
        B_Scheduler_Stop(transcoderDst->schedulerSystemData);
        transcoderDst->systemDataTimerIsStarted = false;
    }
    if (transcoderDst->schedulerSystemData) {
        B_Scheduler_Destroy(transcoderDst->schedulerSystemData);
        transcoderDst->schedulerSystemData = NULL;
    }
    if (transcoderDst->schedulerThread) {
        B_Thread_Destroy(transcoderDst->schedulerThread);
        transcoderDst->schedulerThread = NULL;
    }
    if (transcoderDst->mutexSystemData) {
        B_Mutex_Destroy(transcoderDst->mutexSystemData);
        transcoderDst->mutexSystemData = NULL;
    }

    for (i=0; i<IP_STREAMER_PSI_QUEUE_CNT; i++) {
        if (transcoderDst->pat[i]) NEXUS_Memory_Free(transcoderDst->pat[i]);
        if (transcoderDst->pmt[i]) NEXUS_Memory_Free(transcoderDst->pmt[i]);
    }
    BDBG_MSG(("%s: exit", __FUNCTION__));
}

/*******************************
 * Add system data to stream_mux
 */
static bool 
B_IpStreamer_SetupSystemData(
    IpStreamerCtx *ipStreamerCtx
    )
{
    unsigned i;
    uint8_t videoStreamType = 0, audioStreamType = 0;
    uint16_t audioPid = 0, pcrPid, videoPid = 0, pmtPid;
    TranscoderDst *transcoderDst;
    B_ThreadSettings settingsThread;
    NEXUS_Error rc = NEXUS_UNKNOWN;
    NEXUS_MemoryAllocationSettings allocSettings;

    if (!ipStreamerCtx || !(transcoderDst = ipStreamerCtx->transcoderDst))
        return false;

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    if (ipStreamerCtx->globalCtx->globalCfg.heapHandle)
        allocSettings.heap = ipStreamerCtx->globalCtx->globalCfg.heapHandle;
    BDBG_MSG(("%s: CTX %p, transcoderDst %p", __FUNCTION__, ipStreamerCtx, transcoderDst));
    for (i=0; i<IP_STREAMER_PSI_QUEUE_CNT; i++) {
        if (NEXUS_Memory_Allocate(IP_STREAMER_TS_HEADER_BUF_LENGTH, &allocSettings, &transcoderDst->pat[i])) {
            BDBG_ERR(("NEXUS Memory alloc Error at %d, returning..", __LINE__));
            goto error;
        }
        if (NEXUS_Memory_Allocate(IP_STREAMER_TS_HEADER_BUF_LENGTH, &allocSettings, &transcoderDst->pmt[i])) {
            BDBG_ERR(("NEXUS Memory alloc Error at %d, returning..", __LINE__));
            goto error;
        }
    }

    /* decide the av stream types to set in PMT */
    if (ipStreamerCtx->cfg.transcode.outVideoPid) {
        videoPid = ipStreamerCtx->cfg.transcode.outVideoPid;
        switch (ipStreamerCtx->cfg.transcode.outVideoCodec) {
            case NEXUS_VideoCodec_eMpeg2:         videoStreamType = 0x2; break;
            case NEXUS_VideoCodec_eMpeg4Part2:    videoStreamType = 0x10; break;
            case NEXUS_VideoCodec_eH264:          videoStreamType = 0x1b; break;
            case NEXUS_VideoCodec_eVc1SimpleMain: videoStreamType = 0xea; break;
            default: BDBG_ERR(("%s: Video encoder codec %d is not supported!", __FUNCTION__, ipStreamerCtx->cfg.transcode.outVideoCodec)); goto error;
        }
        BDBG_MSG(("%s: video pid %d, nexus codec %d, stream type %x", __FUNCTION__, videoPid, ipStreamerCtx->cfg.transcode.outVideoCodec, videoStreamType));
    }

    if (ipStreamerCtx->cfg.transcode.outAudio) {
        audioPid = ipStreamerCtx->cfg.transcode.outAudioPid;
        switch(ipStreamerCtx->cfg.transcode.outAudioCodec) {
            case NEXUS_AudioCodec_eMpeg:         audioStreamType = 0x4; break;
            case NEXUS_AudioCodec_eMp3:          audioStreamType = 0x4; break;
            case NEXUS_AudioCodec_eAac:          audioStreamType = 0xf; break; /* ADTS */
            case NEXUS_AudioCodec_eAacPlus:      audioStreamType = 0x11; break;/* LOAS */
                                                 /* MP2TS doesn't allow 14496-3 AAC+ADTS; here is placeholder to test AAC-HE before LOAS encode is supported; */
            case NEXUS_AudioCodec_eAacPlusAdts:  audioStreamType = 0x11; break;
            case NEXUS_AudioCodec_eAc3:          audioStreamType = 0x81; break;
            default: BDBG_ERR(("Audio encoder codec %d is not supported!", ipStreamerCtx->cfg.transcode.outAudioCodec)); goto error;
        }
        BDBG_MSG(("%s: audio pid %d, nexus codec %d, stream type %x", __FUNCTION__, audioPid, ipStreamerCtx->cfg.transcode.outAudioCodec, audioStreamType));
    }
    pcrPid = ipStreamerCtx->cfg.transcode.outPcrPid;
    pmtPid = ipStreamerCtx->cfg.transcode.outPmtPid;
    BDBG_MSG(("%s: pcr pid %d, pmt pid %d", __FUNCTION__, pcrPid, pmtPid));
    B_IpStreamer_AddPatPmt(transcoderDst->pat[0], transcoderDst->pmt[0], pmtPid, pcrPid, videoPid, audioPid, videoStreamType, audioStreamType);

    for (i=0; i<IP_STREAMER_PSI_QUEUE_CNT; i++) {
        if (i > 0) {
            BKNI_Memcpy(transcoderDst->pat[i], transcoderDst->pat[0], IP_STREAMER_TS_HEADER_BUF_LENGTH);
            BKNI_Memcpy(transcoderDst->pmt[i], transcoderDst->pmt[0], IP_STREAMER_TS_HEADER_BUF_LENGTH);
        }
        NEXUS_Memory_FlushCache(transcoderDst->pat[i], IP_STREAMER_TS_HEADER_BUF_LENGTH);
        NEXUS_Memory_FlushCache(transcoderDst->pmt[i], IP_STREAMER_TS_HEADER_BUF_LENGTH);
    }

    if (!ipStreamerCtx->cfg.hlsSession) {
        /* for HLS Sessions, PAT/PMT are manually inserted at the start of each segment */
        BKNI_Memset(transcoderDst->psi, 0, sizeof(transcoderDst->psi));
        transcoderDst->psi[0].size = 188;
        /* TSHDRBUILDER has one extra byte at the beginning to describe the variable length TS header buffer */
        transcoderDst->psi[0].pData = (void*)((uint8_t*)transcoderDst->pat[0] + 1);
        transcoderDst->psi[0].timestampDelta = 0;
        transcoderDst->psi[1].size = 188;
        /* TSHDRBUILDER has one extra byte at the beginning to describe the variable length TS header buffer */
        transcoderDst->psi[1].pData = (void*)((uint8_t*)transcoderDst->pmt[0] + 1);
        transcoderDst->psi[1].timestampDelta = 0;
        rc = NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[0]);
        if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }
        rc = NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[1]);
        if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }
        BDBG_MSG(("insert PAT&PMT... ccPAT = %x ccPMT=%x", *((uint8_t*)transcoderDst->pat[0] + 4) & 0xf,
                    *((uint8_t*)transcoderDst->pmt[0] + 4) & 0xf));

        /* schedule a periodic timer to insert PAT/PMT */
        if ((transcoderDst->mutexSystemData = B_Mutex_Create(NULL)) == NULL) {
            BDBG_ERR(("%s: Failed to create mutex", __FUNCTION__));
            goto error;
        }
        if ((transcoderDst->schedulerSystemData = B_Scheduler_Create(NULL)) == NULL) {
            BDBG_ERR(("%s: Failed to create scheduler object", __FUNCTION__));
            goto error;
        }
        /* create thread to run scheduler */
        B_Thread_GetDefaultSettings(&settingsThread);
        transcoderDst->schedulerThread = B_Thread_Create("IpStreamer_SystemDataScheduler", 
                (B_ThreadFunc)B_Scheduler_Run,
                transcoderDst->schedulerSystemData,
                &settingsThread);
        if (transcoderDst->schedulerThread == NULL) {
            BDBG_ERR(("%s: Failed to create scheduler thread", __FUNCTION__));
            goto error;
        }
        transcoderDst->systemDataTimer = B_Scheduler_StartTimer(
                transcoderDst->schedulerSystemData,
                transcoderDst->mutexSystemData, 1000, B_IpStreamer_InsertSystemDataTimer, transcoderDst);
        if (transcoderDst->systemDataTimer==NULL) {
            BDBG_ERR(("%s: Failed to schedule timer to periodically insert PAT/PMT", __FUNCTION__));
            goto error;
        }
        transcoderDst->systemDataTimerIsStarted = true;
    }
    BDBG_MSG(("%s: successful", __FUNCTION__));
    return true;

error:
    B_IpStreamer_StopSystemData(ipStreamerCtx);
    return false;
}

#if 0
/************************************
 * Set up encoder AV sync.
 * encode setting and startSetting to be set after end-to-end delay is determined */
/* get end-to-end delay (Dee) for audio and video encoders;
 * TODO: match AV delay! In other words,
 *   if (aDee > vDee) {
 *       vDee' = aDee' = aDee;
 *   }
 *   else {
 *       vDee' = aDee' = vDee;
 *   }
 */
static void xcode_av_sync( 
        TranscodeContext  *transcoderDst,
        NEXUS_VideoEncoderSettings *pVideoEncoderConfig,
        NEXUS_VideoEncoderStartSettings *pVideoEncoderStartConfig )
{
    NEXUS_VideoEncoderDelayRange videoDelay;
    NEXUS_AudioMuxOutputDelayStatus audioDelayStatus;
    NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
    unsigned Dee;

    /******************************************
     * add configurable delay to video path 
     */
    /* disable Inverse Telecine Field Pairing for extreme low delay mode
     * NOTE: ITFP is encoder feature to detect and lock on 3:2/2:2 cadence in the video content to help 
     * efficient coding for interlaced formats; disabling ITFP will impact the bit efficiency but reduce the encode delay. */
    if(transcoderDst->bCustomizeDelay)
    {
        pVideoEncoderConfig->enableFieldPairing = transcoderDst->encodeSettings.enableFieldPairing;

        /* 0 to use default 750ms rate buffer delay; TODO: allow user to adjust it to lower encode delay at cost of quality reduction! */
        pVideoEncoderStartConfig->rateBufferDelay = transcoderDst->encodeSettings.rateBufferDelay;

        /* to allow 23.976p passthru; TODO: allow user to configure minimum framerate to achieve lower delay!
         * Note: lower minimum framerate means longer encode delay */
        pVideoEncoderStartConfig->bounds.inputFrameRate.min = transcoderDst->encodeSettings.inputFrameRateMin;

        /* to allow 15 ~ 60p dynamic frame rate coding TODO: allow user to config higher minimum frame rate for lower delay! */
        pVideoEncoderStartConfig->bounds.outputFrameRate.min = transcoderDst->encodeSettings.outputFrameRateMin;
        pVideoEncoderStartConfig->bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

        /* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
        pVideoEncoderStartConfig->bounds.inputDimension.max.width = transcoderDst->encodeSettings.inputMaxWidth;
        pVideoEncoderStartConfig->bounds.inputDimension.max.height = transcoderDst->encodeSettings.inputMaxHeight;
    }
    else
    {
        pVideoEncoderConfig->enableFieldPairing = true;

        /* 0 to use default 750ms rate buffer delay; TODO: allow user to adjust it to lower encode delay at cost of quality reduction! */
        pVideoEncoderStartConfig->rateBufferDelay = 0;

        /* to allow 23.976p passthru; TODO: allow user to configure minimum framerate to achieve lower delay!
         * Note: lower minimum framerate means longer encode delay */
        pVideoEncoderStartConfig->bounds.inputFrameRate.min = NEXUS_VideoFrameRate_e23_976;

        /* to allow 15 ~ 60p dynamic frame rate coding TODO: allow user to config higher minimum frame rate for lower delay! */
        pVideoEncoderStartConfig->bounds.outputFrameRate.min = NEXUS_VideoFrameRate_e14_985;
        pVideoEncoderStartConfig->bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

        /* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
        pVideoEncoderStartConfig->bounds.inputDimension.max.width = 1920;
        pVideoEncoderStartConfig->bounds.inputDimension.max.height = 1088;
    }

    /*****************************************
     * calculate video encoder A2P delay 
     */
    /* NOTE: video encoder delay is in 27MHz ticks */
    NEXUS_VideoEncoder_GetDelayRange(transcoderDst->videoEncoder, pVideoEncoderConfig, pVideoEncoderStartConfig, &videoDelay);
    BDBG_WRN(("\n\tVideo encoder end-to-end delay = [%u ~ %u] ms", videoDelay.min/27000, videoDelay.max/27000));

    if(transcoderDst->inputSettings.bAudioInput)
    {
        NEXUS_AudioMuxOutput_GetDelayStatus(transcoderDst->audioMuxOutput, transcoderDst->encodeSettings.encoderAudioCodec, &audioDelayStatus);
        BDBG_WRN(("\n\tAudio codec %d end-to-end delay = %u ms", transcoderDst->encodeSettings.encoderAudioCodec, audioDelayStatus.endToEndDelay));

        Dee = audioDelayStatus.endToEndDelay * 27000; /* in 27MHz ticks */
        if(Dee > videoDelay.min)
        {
            if(Dee > videoDelay.max)
            {
                BDBG_ERR(("Audio Dee is way too big! Use video Dee max!"));
                Dee = videoDelay.max;
            }
            else
            {
                BDBG_WRN(("Use audio Dee %u ms %u ticks@27Mhz!", Dee/27000, Dee));
            }
        }
        else
        {
            Dee = videoDelay.min;
            BDBG_WRN(("Use video Dee %u ms %u ticks@27Mhz!", Dee/27000, Dee));
        }
        pVideoEncoderConfig->encoderDelay = Dee;

        /* Start audio mux output */
        NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
        /* audio NRT requires mux out to take NRT decode STC */
        audioMuxStartSettings.stcChannel        = transcoderDst->bNonRealTime?
            transcoderDst->stcAudioChannel : transcoderDst->stcChannelTranscode;
        audioMuxStartSettings.presentationDelay = Dee/27000;/* in ms */
        audioMuxStartSettings.nonRealTime       = transcoderDst->bNonRealTime;
        NEXUS_AudioMuxOutput_Start(transcoderDst->audioMuxOutput, &audioMuxStartSettings);
    }
    else
        pVideoEncoderConfig->encoderDelay = videoDelay.min;
}
#endif

void
stopNexusTranscoderDst(
    IpStreamerCtx *ipStreamerCtx
    )
{
    if (ipStreamerCtx->transcoderDst->refCount > 1) {
        BDBG_MSG(("%s: session is being shared w/ another transcoding session, returning", __FUNCTION__));
        return;
    }

    if (ipStreamerCtx->transcoderDst->systemDataTimerIsStarted)
        B_IpStreamer_StopSystemData(ipStreamerCtx);

    if (ipStreamerCtx->transcodingInProgress) {
        NEXUS_StopCallbacks(ipStreamerCtx->transcoderDst->videoDecoder);
        if (ipStreamerCtx->transcoderDst->videoDecoder)
            NEXUS_VideoDecoder_Stop(ipStreamerCtx->transcoderDst->videoDecoder);
        if (ipStreamerCtx->transcoderDst->audioDecoder)
            NEXUS_AudioDecoder_Stop(ipStreamerCtx->transcoderDst->audioDecoder);
        if (ipStreamerCtx->transcoderDst->audioMuxOutput)
            NEXUS_AudioMuxOutput_Stop(ipStreamerCtx->transcoderDst->audioMuxOutput);
        if (ipStreamerCtx->transcoderDst->videoEncoder)
            NEXUS_VideoEncoder_Stop(ipStreamerCtx->transcoderDst->videoEncoder,NULL);
        /* wait for stream mux to cleanly finish */
        if (ipStreamerCtx->transcoderDst->streamMux) {
            BERR_Code rc;
            NEXUS_StreamMux_Finish(ipStreamerCtx->transcoderDst->streamMux);
            rc = BKNI_WaitForEvent(ipStreamerCtx->transcoderDst->finishEvent, 4000);
            if (rc != 0)
                BDBG_WRN(("%s: %s while waiting for Stream mux to finish", __FUNCTION__, rc == BERR_TIMEOUT ? "Timeout" : "Error"));
            /* for transcoded sessions, pid channels need to be removed before stream mux is stopped otherwise, it frees the pid channels */
            if (ipStreamerCtx->cfg.enableTimeshifting) {
                NEXUS_Record_Stop(ipStreamerCtx->ipDst->recordHandle);
                NEXUS_Record_RemoveAllPidChannels(ipStreamerCtx->ipDst->recordHandle);
            }
            else {
                if (ipStreamerCtx->ipStreamingInProgress == true) {
                    NEXUS_Recpump_Stop(ipStreamerCtx->ipDst->recpumpHandle);
                }
                if (ipStreamerCtx->ipDst && ipStreamerCtx->ipDst->recpumpHandle)
                    NEXUS_Recpump_RemoveAllPidChannels(ipStreamerCtx->ipDst->recpumpHandle);
            }
            if (ipStreamerCtx->transcoderDst->streamMux)
                NEXUS_StreamMux_Stop(ipStreamerCtx->transcoderDst->streamMux);
        }
        /* since opening of decode stc channel is done in the Start call, closing of the stc channel will need to happen in this stop call */
        if (ipStreamerCtx->transcoderDst->decodeStcChannel)
            NEXUS_StcChannel_Close(ipStreamerCtx->transcoderDst->decodeStcChannel);
        ipStreamerCtx->transcoderDst->decodeStcChannel = NULL;

        ipStreamerCtx->transcodingInProgress = false;
        ipStreamerCtx->transcoderDst->started = false;
        BDBG_MSG(("CTX %p: Transcoder dst %p is stopped", ipStreamerCtx, ipStreamerCtx->transcoderDst));
    }
}

extern int stopNexusDst( IpStreamerCtx *ipStreamerCtx);
extern void closeNexusDst( IpStreamerCtx *ipStreamerCtx);
void
closeNexusTranscoderPipe(
    IpStreamerCtx *ipStreamerCtx,
    TranscoderDst *transcoderDst
    )
{
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelSettings syncChannelSettings;
#endif
    if (!ipStreamerCtx || !transcoderDst)
        return;
    if (transcoderDst->refCount > 0)
        transcoderDst->refCount--;
    if (transcoderDst->refCount == 0) {
        BDBG_MSG(("%s: Closing transcoder pipe, ref cnt %d", __FUNCTION__, transcoderDst->refCount));
        if (ipStreamerCtx->transcodingInProgress) {
            /* stop the transcoding pipe first as its stop was delayed to the close call */
            stopNexusTranscoderDst(ipStreamerCtx);

            /* now stop/close the destination */
            /* this is delayed from the normal stop call as transcoder had to be stopped before we can stop its consumer destination */
            stopNexusDst(ipStreamerCtx);
            closeNexusDst(ipStreamerCtx);
            closeNexusPidChannels(ipStreamerCtx);
            ipStreamerCtx->transcodingInProgress = false;
        }
        else {
            closeNexusDst(ipStreamerCtx);
            closeNexusPidChannels(ipStreamerCtx);
        }
#if NEXUS_HAS_SYNC_CHANNEL
        /* disconnect sync channel */
        if (transcoderDst->syncChannel) {
            NEXUS_SyncChannel_GetSettings(transcoderDst->syncChannel, &syncChannelSettings);
            syncChannelSettings.videoInput = NULL;
            syncChannelSettings.audioInput[0] = NULL;
            syncChannelSettings.audioInput[1] = NULL;
            NEXUS_SyncChannel_SetSettings(transcoderDst->syncChannel, &syncChannelSettings);
            NEXUS_SyncChannel_Destroy(transcoderDst->syncChannel);
            transcoderDst->syncChannel = NULL;
        }
#endif
        if (transcoderDst->timebase != NEXUS_Timebase_eInvalid)
            NEXUS_Timebase_Close(transcoderDst->timebase);
        transcoderDst->timebase = NEXUS_Timebase_eInvalid;

        /* now close the transcoder pipe */
        if (transcoderDst->videoEncoder)
            NEXUS_VideoEncoder_Close(transcoderDst->videoEncoder);
        transcoderDst->videoEncoder = NULL;
        BDBG_MSG(("Closed NEXUS_VideoEncoder_Close ...."));
        if (transcoderDst->streamMux)
            NEXUS_StreamMux_Destroy(transcoderDst->streamMux);
        transcoderDst->streamMux = NULL;
#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
        if (transcoderDst->windowMain) {
#if NEXUS_NUM_HDMI_INPUT
            if (ipStreamerCtx->cfg.srcType == IpStreamerSrc_eHdmi)
                NEXUS_VideoWindow_RemoveInput(transcoderDst->windowMain, NEXUS_HdmiInput_GetVideoConnector(ipStreamerCtx->hdmiSrc->hdmiInput));
            else
#endif
                NEXUS_VideoWindow_RemoveInput(transcoderDst->windowMain, NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder));
            NEXUS_VideoWindow_Close(transcoderDst->windowMain);
        }
        transcoderDst->windowMain = NULL;
        if (transcoderDst->displayMain)
            NEXUS_Display_Close(transcoderDst->displayMain);
        transcoderDst->displayMain = NULL;
#endif
        if (transcoderDst->windowTranscode) {
            NEXUS_VideoWindow_RemoveAllInputs(transcoderDst->windowTranscode);
            NEXUS_VideoWindow_Close(transcoderDst->windowTranscode);
        }
        transcoderDst->windowTranscode = NULL;
        if (transcoderDst->videoDecoder) {
            NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder));
            NEXUS_VideoDecoder_Close(transcoderDst->videoDecoder);
            if(ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv)
            {   /*informing NEXUS server process that this decoder is free*/    
                ipStreamerCtx->globalCtx->globalCfg.releaseDecoder(transcoderDst->decoderIndex);
            }
        }
        transcoderDst->videoDecoder = NULL;
        if (transcoderDst->displayTranscode)
            NEXUS_Display_Close(transcoderDst->displayTranscode);
        transcoderDst->displayTranscode = NULL;

        if (transcoderDst->audioDecoder) {
            NEXUS_PlatformConfiguration platformConfig;

            NEXUS_Platform_GetConfiguration(&platformConfig);
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(transcoderDst->audioMuxOutput));
            NEXUS_AudioEncoder_RemoveAllInputs(transcoderDst->audioEncoder);
            NEXUS_AudioInput_Shutdown(NEXUS_AudioEncoder_GetConnector(transcoderDst->audioEncoder));
            NEXUS_AudioEncoder_Close(transcoderDst->audioEncoder);
            if(ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv)
            {
                ipStreamerCtx->globalCtx->globalCfg.disconnectAudioDummy(transcoderDst->contextId);
            }
            else
            {
                NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDummyOutput_GetConnector(platformConfig.outputs.audioDummy[transcoderDst->contextId]));
                NEXUS_AudioOutput_Shutdown(NEXUS_AudioDummyOutput_GetConnector(platformConfig.outputs.audioDummy[transcoderDst->contextId]));
            }
            NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
            NEXUS_AudioMuxOutput_Destroy(transcoderDst->audioMuxOutput);
            NEXUS_AudioDecoder_Close(transcoderDst->audioDecoder);
            transcoderDst->audioDecoder = NULL;
            transcoderDst->audioEncoder = NULL;
            transcoderDst->audioMuxOutput = NULL;
        }
        transcoderDst->audioDecoder = NULL;
        if (transcoderDst->encodeStcChannel)
            NEXUS_StcChannel_Close(transcoderDst->encodeStcChannel);
        transcoderDst->encodeStcChannel = NULL;
        if (transcoderDst->playpumpTranscodeVideo)
            NEXUS_Playpump_Close(transcoderDst->playpumpTranscodeVideo);
        transcoderDst->playpumpTranscodeVideo = NULL;
        if (transcoderDst->playpumpTranscodeAudio)
            NEXUS_Playpump_Close(transcoderDst->playpumpTranscodeAudio);
        transcoderDst->playpumpTranscodeAudio = NULL;
        if (transcoderDst->playpumpTranscodeSystemData)
            NEXUS_Playpump_Close(transcoderDst->playpumpTranscodeSystemData);
        transcoderDst->playpumpTranscodeSystemData = NULL;
        transcoderDst->inUse = false;
    }
    else {
        BDBG_MSG(("%s: session is being shared (ref cnt %d) w/ another transcoding session, returning", __FUNCTION__, transcoderDst->refCount));
        if (ipStreamerCtx->ipStreamingInProgress) {
            BDBG_MSG(("%s: close ip dst resources", __FUNCTION__));
            stopNexusDst(ipStreamerCtx);
            closeNexusDst(ipStreamerCtx);
        }
    }
    BDBG_MSG(("%s: Done", __FUNCTION__));
}

#ifndef IP_STREAMER_DEMO_MODE
static void 
sourceChangedCallback(void *context, int param)
{
    NEXUS_VideoEncoderSettings videoEncoderConfig;
    IpStreamerCtx *ipStreamerCtx = (IpStreamerCtx *)context;
    NEXUS_DisplayCustomFormatSettings customFormatSettings;
    BSTD_UNUSED(param);
    if (ipStreamerCtx->transcoderDst && ipStreamerCtx->transcoderDst->videoDecoder) {
        NEXUS_VideoDecoderStatus status;
        if (NEXUS_VideoDecoder_GetStatus(ipStreamerCtx->transcoderDst->videoDecoder, &status) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: NEXUS_VideoDecoder_GetStatus Failed", __FUNCTION__));
            return;
        }
        BDBG_MSG(("%s: res: source %dx%d, coded %dx%d, display %dx%d, ar %d, fr %d, interlaced %d video format %d, muted %d", __FUNCTION__,
                    status.source.width, status.source.height, 
                    status.coded.width, status.coded.height, 
                    status.display.width, status.display.height, 
                    status.aspectRatio, 
                    status.frameRate, 
                    status.interlaced, 
                    status.format, 
                    status.muted 
                    ));
        if (status.source.width < ipStreamerCtx->cfg.transcode.outWidth && status.source.height < ipStreamerCtx->cfg.transcode.outHeight) {
            /* TODO: may need to add check for increasing res as well */
            BDBG_WRN(("%s: lower the display resolution & bitrate, target res %dx%d, source res %dx%d", __FUNCTION__,
                    ipStreamerCtx->cfg.transcode.outWidth, ipStreamerCtx->cfg.transcode.outHeight,
                    status.source.width, status.source.height)); 
            NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
            customFormatSettings.width = status.source.width;
            customFormatSettings.height = status.source.height;
            customFormatSettings.refreshRate = 60000;
            customFormatSettings.interlaced = ipStreamerCtx->cfg.transcode.outInterlaced;
            customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
            customFormatSettings.sampleAspectRatio.x = status.source.width;
            customFormatSettings.sampleAspectRatio.y = status.source.height;
            customFormatSettings.dropFrameAllowed = true;
            if (NEXUS_Display_SetCustomFormatSettings(ipStreamerCtx->transcoderDst->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings) != 0) {
                BDBG_ERR(("%s: ERROR: NEXUS_Display_SetCustomFormatSettings() Failed", __FUNCTION__));
                return;
            }
            BDBG_WRN(("%s: Successfully Set Custom Format settings for display", __FUNCTION__));

            NEXUS_VideoEncoder_GetSettings(ipStreamerCtx->transcoderDst->videoEncoder, &videoEncoderConfig);
            videoEncoderConfig.bitrateMax = 2000000; /* TODO: set it based on the minBitrate from device profile */
            NEXUS_VideoEncoder_SetSettings(ipStreamerCtx->transcoderDst->videoEncoder, &videoEncoderConfig);
            BDBG_WRN(("%s: modified encode bitrate to %d", __FUNCTION__, videoEncoderConfig.bitrateMax));
        }
    }
}
#endif

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
TranscoderDst *
openNexusRaagaTranscoderPipe(
        IpStreamerConfig *ipStreamerCfg,
        IpStreamerCtx *ipStreamerCtx
        )
{
    int i;
    TranscoderDst *transcoderDst = NULL;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlaypumpOpenSettings playpumpConfig;
    NEXUS_StreamMuxCreateSettings muxCreateSettings;
    NEXUS_VideoWindowMadSettings madSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoWindowScalerSettings scalerSettings;
    NEXUS_AudioEncoderSettings encoderSettings;
    NEXUS_ClientConfiguration clientConfig;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelSettings syncChannelSettings;
#endif

    BDBG_MSG(("%s: Setup transcoder", __FUNCTION__));
    /* open available transcoder dst handle */
    for (transcoderDst=NULL,i=0; i<NEXUS_NUM_DSP_VIDEO_ENCODERS; i++) {
        if (!ipStreamerCtx->globalCtx->transcoderDstList[i].inUse) {
            transcoderDst = &ipStreamerCtx->globalCtx->transcoderDstList[i];
            ipStreamerCtx->transcoderDst = transcoderDst;
            transcoderDst->inUse = true;
            BDBG_MSG(("%s: Found Free Transcoder Dst entry: idx %d, addr %p, total Transcoder dst entries %d", __FUNCTION__, i, transcoderDst, NEXUS_NUM_DSP_VIDEO_ENCODERS));
            break;
        }
    }
    if (!transcoderDst) {
        BDBG_ERR(("%s: ERROR: No Free Transcoder Dst entry, max %d, can't start new transcoder session", __FUNCTION__, i));
        goto error;
    }
    ipStreamerCtx->transcoderDst->refCount = 1;

    /* now setup this transcoding pipeline */

#if NEXUS_HAS_SYNC_CHANNEL
    /* create a sync channel */
    NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
    transcoderDst->syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
    if (!transcoderDst->syncChannel) {
        BDBG_ERR(("%s: ERROR: Failed to create nexus sync channel", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Created nexus sync channel", __FUNCTION__));
#endif

    if ((transcoderDst->timebase = NEXUS_Timebase_Open(NEXUS_ANY_ID)) == NEXUS_Timebase_eInvalid) {
        BDBG_ERR(("%s: ERROR: NEXUS_Timebase_Open Failed to open a free Timebase", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: using timebase %d for decode", __FUNCTION__, transcoderDst->timebase));

    /* defer opening of decodeStcChannel to Start time as StcChannel_Open now requires app to provide pcrPidChannel */
    /* which is not available until PSI discovery is complete after this call. */

    /* video decoder is not needed for straight hdmi sources */
    if (ipStreamerCfg->srcType != IpStreamerSrc_eHdmi) {
        /* open decoder */
        NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
        NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
        if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
            videoDecoderOpenSettings.fifoSize = 3*1024*1024; /* 3MB: for over 1 sec of jitter buffer for 20Mpbs stream */
            BDBG_MSG(("%s: Increasing Video Decoder Fifo size to %d", __FUNCTION__, videoDecoderOpenSettings.fifoSize));
        }
        BDBG_MSG(("%s: Turning off svc/mvc decode feature for decoder idx %d", __FUNCTION__, i));
        videoDecoderOpenSettings.enhancementPidChannelSupported = false;

        if(ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv){
            /*in multiprocess environment, NEXUS server process returns the decoder to be used, we do not iterate through the loop*/
            i = ipStreamerCtx->globalCtx->globalCfg.requestDecoder();
            BDBG_MSG(("%s: decoder returned from NEXUS server %d", __FUNCTION__, i));
            if(i != -1) {
                transcoderDst->videoDecoder = NEXUS_VideoDecoder_Open(i, &videoDecoderOpenSettings); /* take default capabilities */
                /*remember the decoder index, NEXUS server needs to be informed at the end of transcode session*/
                transcoderDst->decoderIndex = i;
            } else {
                BDBG_ERR(("%s: ERROR: Can't get a free Decoder from NEXUS server %d", __FUNCTION__, i));
                goto error;
            }
            if(!transcoderDst->videoDecoder) {
                /*inform NEXUS server that this decoder is still unused*/
                ipStreamerCtx->globalCtx->globalCfg.releaseDecoder(transcoderDst->decoderIndex);
                BDBG_ERR(("%s: ERROR: Can't get a free Decoder Handle, max idx %d", __FUNCTION__, i));
                goto error;
            }
         } else {
            /* leaving decoder 0 for local client app (BMC) as it is hardcoded to only use decoder 0 for main  and pip is not supported on raaga based chips yet */
            for (i=1; i<NEXUS_NUM_VIDEO_DECODERS; i++) {
                transcoderDst->videoDecoder = NEXUS_VideoDecoder_Open(i, &videoDecoderOpenSettings); /* take default capabilities */
                if (transcoderDst->videoDecoder)
                    break;
            }
            if (!transcoderDst->videoDecoder) {
                BDBG_ERR(("%s: ERROR: Can't get a free Decoder Handle, max idx %d", __FUNCTION__, i));
                goto error;
            }
        }
      /* turn off svc/mvc decodes to save heap memory space */
        NEXUS_VideoDecoder_GetSettings(transcoderDst->videoDecoder, &videoDecoderSettings);
        for (i=0;i<NEXUS_VideoCodec_eMax;i++) {
            switch (videoDecoderSettings.supportedCodecs[i]) {
                case NEXUS_VideoCodec_eH264_Svc:
                case NEXUS_VideoCodec_eH264_Mvc:
                    videoDecoderSettings.supportedCodecs[i] = NEXUS_VideoCodec_eNone;
                    break;
                default:
                    break;
            }
        }
#ifndef IP_STREAMER_DEMO_MODE
        videoDecoderSettings.sourceChanged.callback = sourceChangedCallback;
        videoDecoderSettings.sourceChanged.context = ipStreamerCtx;
#endif
        if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
            videoDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;    /* In 45Khz clock */
        }
        if (NEXUS_VideoDecoder_SetSettings(transcoderDst->videoDecoder, &videoDecoderSettings) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to turn off SVC/MVC decode option on 2nd decoder", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: turned off SVC/MVC decode option on 2nd decoder", __FUNCTION__));
    }

    /* encoders/mux require different STC broadcast mode from decoder */
    /* open 2nd STC channel for this purpose */
    NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
#if defined(NEXUS_HAS_VIDEO_ENCODER) && defined(NEXUS_HAS_HDMI_INPUT)
    if (ipStreamerCfg->srcType == IpStreamerSrc_eHdmi)
        stcSettings.timebase = ipStreamerCtx->hdmiSrc->timebase;
    else 
#endif
        stcSettings.timebase = transcoderDst->timebase;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;
    transcoderDst->encodeStcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    if (!transcoderDst->encodeStcChannel) {
        BDBG_ERR(("%s: ERROR: Can't get a free STC Channel Handle, max idx %d", __FUNCTION__, i));
        goto error;
    }
    BDBG_MSG(("%s: opened encode STC Channel", __FUNCTION__));

    /* open & configure encoder */
    for (i=0; i<NEXUS_NUM_DSP_VIDEO_ENCODERS; i++) {
        transcoderDst->videoEncoder = NEXUS_VideoEncoder_Open(i, NULL); /* take default capabilities */
        transcoderDst->contextId = i;
        if (transcoderDst->videoEncoder)
            break;
    }
    if (!transcoderDst->videoEncoder) {
        BDBG_ERR(("%s: ERROR: Can't get a free Video Encoder Handle, max idx %d", __FUNCTION__, i));
        goto error;
    }
    BDBG_MSG(("%s: opened video encoder %p for transcode ctx %p", __FUNCTION__, transcoderDst->videoEncoder, transcoderDst));

    /* open & setup the display: note transcode video format settings are specified here */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e480p;
#if defined(NEXUS_HAS_VIDEO_ENCODER) && defined(NEXUS_HAS_HDMI_INPUT)
    if (ipStreamerCfg->srcType == IpStreamerSrc_eHdmi)
        displaySettings.timebase = ipStreamerCtx->hdmiSrc->timebase;
#endif
    transcoderDst->displayTranscode = NEXUS_Display_Open(transcoderDst->displayIndex, &displaySettings);
    if (!transcoderDst->displayTranscode) {
        BDBG_ERR(("%s: ERROR: Can't get a free Display Handle, max idx %d", __FUNCTION__, transcoderDst->displayIndex));
        goto error;
    }

#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
    {
        NEXUS_PlatformConfiguration platformConfig;
        NEXUS_VideoFormatInfo videoInfo;

        NEXUS_Platform_GetConfiguration(&platformConfig);
        /* Bring up video display and outputs */
        NEXUS_Display_GetDefaultSettings(&displaySettings);
        displaySettings.format = NEXUS_VideoFormat_e720p;
        transcoderDst->displayMain = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
        if(platformConfig.outputs.component[0]){
            NEXUS_Display_AddOutput(transcoderDst->displayMain, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
        if(platformConfig.outputs.hdmi[0]){
            NEXUS_Display_AddOutput(transcoderDst->displayMain, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
        }
#endif
        if ((transcoderDst->windowMain = NEXUS_VideoWindow_Open(transcoderDst->displayMain, 1)) == NULL) {
            BDBG_ERR(("%s: ERROR: Can't get a free Video Window on Display", __FUNCTION__));
            goto error;
        }
        NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoInfo);
        NEXUS_VideoWindow_GetSettings(transcoderDst->windowMain, &windowSettings); 
        windowSettings.position.x = 0;
        windowSettings.position.y = 0;
        windowSettings.position.width = videoInfo.width;
        windowSettings.position.height = videoInfo.height;
        if (NEXUS_VideoWindow_SetSettings(transcoderDst->windowMain, &windowSettings) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_SetSettings Failed", __FUNCTION__));
            goto error;
        }

    }
#endif
#if 0
    /* TODO: check if this is needed, encode.c app doesn't set it */
    NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
    customFormatSettings.width = ipStreamerCfg->transcode.outWidth;
    customFormatSettings.height = ipStreamerCfg->transcode.outHeight;
    customFormatSettings.refreshRate = 60000;
    customFormatSettings.interlaced = ipStreamerCfg->transcode.outInterlaced;
    customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
    customFormatSettings.sampleAspectRatio.x = ipStreamerCfg->transcode.outWidth;
    customFormatSettings.sampleAspectRatio.y = ipStreamerCfg->transcode.outHeight;
    customFormatSettings.dropFrameAllowed = true;
    if (NEXUS_Display_SetCustomFormatSettings(transcoderDst->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings) != 0) {
        BDBG_ERR(("%s: ERROR: NEXUS_Display_SetCustomFormatSettings() Failed", __FUNCTION__));
        goto error;
    }
    BDBG_WRN(("%s: Successfully Set Custom Format settings for display", __FUNCTION__));
#endif

    if ((transcoderDst->windowTranscode = NEXUS_VideoWindow_Open(transcoderDst->displayTranscode, 0)) == NULL) {
        BDBG_ERR(("%s: ERROR: Can't get a free Video Window on Display", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: display %p & video window %p are opened for transcoder ctx %p", __FUNCTION__, transcoderDst->displayTranscode, transcoderDst->windowTranscode, transcoderDst));

    /* set transcoder minimum display format before AddInput to avoid black frame transition during dynamic resolution change */
    NEXUS_VideoWindow_GetSettings(transcoderDst->windowTranscode, &windowSettings);
    windowSettings.position.width = ipStreamerCfg->transcode.outWidth;
    windowSettings.position.height = ipStreamerCfg->transcode.outHeight;
    windowSettings.pixelFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;;
    windowSettings.visible = false;
    if (NEXUS_VideoWindow_SetSettings(transcoderDst->windowTranscode, &windowSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_SetSettings Failed for the transcode window", __FUNCTION__));
        goto error;
    }

    /* set transcoder window vnet mode bias to avoid black frame transition during dynamic resolution change */
    NEXUS_VideoWindow_GetScalerSettings(transcoderDst->windowTranscode, &scalerSettings);
    scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
    scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
    if (NEXUS_VideoWindow_SetScalerSettings(transcoderDst->windowTranscode, &scalerSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_SetScalerSettings Failed for transcode window", __FUNCTION__));
        goto error;
    }

    /* enable deinterlacer to improve quality */
    NEXUS_VideoWindow_GetMadSettings(transcoderDst->windowTranscode, &madSettings);
    madSettings.deinterlace = true;
    madSettings.enable22Pulldown = true;
    madSettings.enable32Pulldown = true;
    if (NEXUS_VideoWindow_SetMadSettings(transcoderDst->windowTranscode, &madSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_SetMadSettings() failed to enable MAD ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Enabled MAD De-interlacer", __FUNCTION__));


#if defined(NEXUS_HAS_VIDEO_ENCODER) && defined(NEXUS_HAS_HDMI_INPUT)
    if (ipStreamerCfg->srcType == IpStreamerSrc_eHdmi) {
        /* connect hdmi to the display (which feeds into encoder) */
        if (NEXUS_VideoWindow_AddInput(transcoderDst->windowTranscode, NEXUS_HdmiInput_GetVideoConnector(ipStreamerCtx->hdmiSrc->hdmiInput)) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_AddInput() failed to connect decoder to display", __FUNCTION__));
            goto error;
        }

#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
        NEXUS_VideoWindow_AddInput(transcoderDst->windowMain, NEXUS_HdmiInput_GetVideoConnector(ipStreamerCtx->hdmiSrc->hdmiInput));
#endif
    }
    else
#endif
    {
        /* connect decoder to the display (which feeds into encoder) */
        if (NEXUS_VideoWindow_AddInput(transcoderDst->windowTranscode, NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder)) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_AddInput() failed to connect decoder to display", __FUNCTION__));
            goto error;
        }
    }

#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
    if (NEXUS_VideoWindow_AddInput(transcoderDst->windowMain, NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder)) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_AddInput() failed to connect decoder to local display", __FUNCTION__));
        goto error;
    }
#endif

    if (ipStreamerCfg->transcode.outAudio) {
        NEXUS_PlatformConfiguration platformConfig;
        NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
        NEXUS_Platform_GetConfiguration(&platformConfig);
        NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
        if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
            audioDecoderOpenSettings.fifoSize = 512*1024; /* increasing to act as de-jitter buffer for ip */
            BDBG_MSG(("%s: Increasing Audio Decoder Fifo size to %d", __FUNCTION__, audioDecoderOpenSettings.fifoSize));
        }
        for (i=0; i<NEXUS_NUM_AUDIO_DECODERS; i++) {
            transcoderDst->audioDecoder = NEXUS_AudioDecoder_Open(i, &audioDecoderOpenSettings);
            if (transcoderDst->audioDecoder)
                break;
        }
        if (!transcoderDst->audioDecoder) {
            BDBG_ERR(("%s: ERROR: Can't open audio decoder id %d", __FUNCTION__, i));
            goto error;
        }
        BDBG_MSG(("%s: opened audio decoder %p for transcode ctx %p", __FUNCTION__, transcoderDst->audioDecoder, transcoderDst));

        /* Create audio mux output */
        if ((transcoderDst->audioMuxOutput = NEXUS_AudioMuxOutput_Create(NULL)) == NULL) {
            BDBG_ERR(("%s: ERROR: Can't create Audio Stream Mux handle", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: Audio Stream Mux %p is created for transcoder ctx %p", __FUNCTION__, transcoderDst->audioMuxOutput, ipStreamerCtx->transcoderDst));

        /* Open audio encoder */
        NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
        encoderSettings.codec = ipStreamerCfg->transcode.outAudioCodec;
        transcoderDst->audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
        if (!transcoderDst->audioEncoder) {
            BDBG_ERR(("%s: ERROR: Can't open audio encoder handle", __FUNCTION__));
            goto error;
        }

        /* Connect encoder to decoder */
        if (NEXUS_AudioEncoder_AddInput(
                    transcoderDst->audioEncoder,
                    NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo)) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: NEXUS_AudioEncoder_AddInput() failed to connect encoder w/ decoder", __FUNCTION__));
            goto error;
        }

        /* Connect mux to encoder */
        if (NEXUS_AudioOutput_AddInput(
                    NEXUS_AudioMuxOutput_GetConnector(transcoderDst->audioMuxOutput), 
                    NEXUS_AudioEncoder_GetConnector(transcoderDst->audioEncoder)) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: NEXUS_AudioOutput_AddInput() failed to connect mux to encoder", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: audio encode is successfully setup", __FUNCTION__));
        /* Connect audio decoder to a dummy output so that encode path doesn't consume a real audio output which can be used for local decode */
        if(ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv){
            /*in multi process environment, NEXUS server process makes the connection*/
            if(!ipStreamerCtx->globalCtx->globalCfg.connectAudioDummy(transcoderDst->audioDecoder, transcoderDst->contextId))   {
                BDBG_ERR(("%s: ERROR: NEXUS_AudioOutput_AddInput() failed to add spdif audio input", __FUNCTION__));
                goto error;
            }
        } else {
            if (NEXUS_AudioOutput_AddInput(
                    NEXUS_AudioDummyOutput_GetConnector(platformConfig.outputs.audioDummy[transcoderDst->contextId]),
                    NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo)) != NEXUS_SUCCESS) {    
                        BDBG_ERR(("%s: ERROR: NEXUS_AudioOutput_AddInput() failed to add spdif audio input", __FUNCTION__));
                        goto error;
            }
        }

#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
#if NEXUS_NUM_HDMI_OUTPUTS
        if(platformConfig.outputs.hdmi[0]){
            NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]), NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
        }
        NEXUS_AudioOutput_AddInput( NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

#endif
#endif
        BDBG_MSG(("%s: audio encode is successfully setup", __FUNCTION__));
    }

    /* display is connected to the encoder module during encoder start */

    /* open the stream mux */
    NEXUS_StreamMux_GetDefaultCreateSettings(&muxCreateSettings);
    muxCreateSettings.finished.callback = transcoderFinishCallback;
    muxCreateSettings.finished.context = transcoderDst;
    muxCreateSettings.finished.param = 0;
    if ((transcoderDst->streamMux = NEXUS_StreamMux_Create(&muxCreateSettings)) == NULL) {
        BDBG_ERR(("%s: ERROR: Can't create Stream Mux handle", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Stream Mux %p is created for transcoder ctx %p", __FUNCTION__, transcoderDst->streamMux, ipStreamerCtx->transcoderDst));


    /* and finally open the playpump channels that are used by mux to feed the Audio & Video ES as well as System data to the xpt h/w */
    /* that is where the actual muxing of stream happens */

    /* reduce FIFO size allocated for playpump used for the transcoded streams */
    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpConfig);
    playpumpConfig.fifoSize = 16384; /* reduce FIFO size allocated for playpump */
    playpumpConfig.numDescriptors = 64; /* set number of descriptors */
    playpumpConfig.streamMuxCompatible = true;
    
    if(ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv)
    {       
        NEXUS_Platform_GetClientConfiguration(&clientConfig);    
        playpumpConfig.heap = clientConfig.heap[1]; /* playpump requires heap with eFull mapping */
    }

    if ((transcoderDst->playpumpTranscodeVideo = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig)) == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to Open Nexus Playpump idx ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Nexus Playpump idx is opened for video transcoder ctx %p", __FUNCTION__, ipStreamerCtx->transcoderDst));
    if ((transcoderDst->playpumpTranscodeAudio = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig)) == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to Open Nexus Playpump idx ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Nexus Playpump idx is opened for audio transcoder ctx %p", __FUNCTION__, ipStreamerCtx->transcoderDst));
    if ((transcoderDst->playpumpTranscodeSystemData = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig)) == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to Open Nexus Playpump idx ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Nexus Playpump idx is opened for system data transcoder ctx %p", __FUNCTION__, ipStreamerCtx->transcoderDst));
    BDBG_MSG(("%s: transcoder ctx %p setup is successful", __FUNCTION__, ipStreamerCtx->transcoderDst));

    return transcoderDst;

error:
    closeNexusTranscoderPipe(ipStreamerCtx, transcoderDst);
    ipStreamerCtx->transcoderDst = NULL;
    return NULL;
}

#else /* !NEXUS_NUM_DSP_VIDEO_ENCODERS */

TranscoderDst *
openNexusVice2TranscoderPipe(
        IpStreamerConfig *ipStreamerCfg,
        IpStreamerCtx *ipStreamerCtx
        )
{
    int i;
    TranscoderDst *transcoderDst = NULL;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlaypumpOpenSettings playpumpConfig;
    NEXUS_StreamMuxCreateSettings muxCreateSettings;
    NEXUS_VideoWindowMadSettings madSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_DisplayCustomFormatSettings customFormatSettings;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoWindowScalerSettings scalerSettings;
    NEXUS_AudioEncoderSettings encoderSettings;
    NEXUS_ClientConfiguration clientConfig;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelSettings syncChannelSettings;
#endif

    BDBG_MSG(("%s: Setup transcoder", __FUNCTION__));
    /* open available transcoder dst handle */
    for (transcoderDst=NULL,i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) {
        if (!ipStreamerCtx->globalCtx->transcoderDstList[i].inUse) {
            transcoderDst = &ipStreamerCtx->globalCtx->transcoderDstList[i];
            transcoderDst->contextId = i;
            ipStreamerCtx->transcoderDst = transcoderDst;
            transcoderDst->inUse = true;
            BDBG_MSG(("%s: Found Free Transcoder Dst entry: idx %d, addr %p, total Transcoder dst entries %d", __FUNCTION__, i, transcoderDst, NEXUS_NUM_VIDEO_ENCODERS));
            break;
        }
    }
    if (!transcoderDst) {
        BDBG_ERR(("%s: ERROR: No Free Transcoder Dst entry, max %d, can't start new transcoder session", __FUNCTION__, i));
        goto error;
    }
    ipStreamerCtx->transcoderDst->refCount = 1;

    /* now setup this transcoding pipeline */
#if NEXUS_HAS_SYNC_CHANNEL
    /* create a sync channel */
    NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
    transcoderDst->syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
    if (!transcoderDst->syncChannel) {
        BDBG_ERR(("%s: ERROR: Failed to create nexus sync channel", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Created nexus sync channel", __FUNCTION__));
#endif

    /* decoder, stc channel are not needed for straight hdmi sources */
    if (ipStreamerCfg->srcType != IpStreamerSrc_eHdmi) {
        NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;

        if ((transcoderDst->timebase = NEXUS_Timebase_Open(NEXUS_ANY_ID)) == NEXUS_Timebase_eInvalid) {
            BDBG_ERR(("%s: ERROR: NEXUS_Timebase_Open Failed to open a free Timebase", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: using timebase %d for decode", __FUNCTION__, transcoderDst->timebase));
        /* defer opening of decodeStcChannel to Start time as StcChannel_Open now requires app to provide pcrPidChannel */
        /* which is not available until PSI discovery is complete after this call. */

        /* open decoder */
        NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
        if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
            videoDecoderOpenSettings.fifoSize = 3*1024*1024; /* 3MB: for over 1 sec of jitter buffer for 20Mpbs stream */
            BDBG_MSG(("%s: Increasing Video Decoder Fifo size to %d", __FUNCTION__, videoDecoderOpenSettings.fifoSize));
        }
        BDBG_MSG(("%s: Turning off svc/mvc decode feature for decoder idx %d", __FUNCTION__, i));
        videoDecoderOpenSettings.enhancementPidChannelSupported = false;

        if (ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv) {
            /*in multiprocess environment, NEXUS server process returns the decoder to be used, we do not iterate through the loop*/
            i = ipStreamerCtx->globalCtx->globalCfg.requestDecoder();
            BDBG_MSG(("%s: decoder returned from NEXUS server %d", __FUNCTION__, i));
            if(i != -1) {
                transcoderDst->videoDecoder = NEXUS_VideoDecoder_Open(i, &videoDecoderOpenSettings); /* take default capabilities */
                /*remember the decoder index, NEXUS server needs to be informed at then end of transcode session*/
                transcoderDst->decoderIndex = i;
            } else {
                BDBG_ERR(("%s: ERROR: Can't get a free Decoder from NEXUS server %d", __FUNCTION__, i));
                goto error;
            }
            if(!transcoderDst->videoDecoder) {
                /*inform NEXUS server that this decoder is still unused*/
                ipStreamerCtx->globalCtx->globalCfg.releaseDecoder(transcoderDst->decoderIndex);
                BDBG_ERR(("%s: ERROR: Can't get a free Decoder Handle, max idx %d", __FUNCTION__, i));
                goto error;
            }
        } else {
        /* ssood: 04/29/2011: leaving decoder 0 & 1 for local client app (BMC) as it is hardcoded to only use decoder 0 & 1 for main & pip decodes */
#if (BCHP_CHIP == 7435)
            for (i=0; i<NEXUS_NUM_VIDEO_DECODERS; i++) /* initial bringup on 03/07/2012: quad xcode requires all 4 decoders */
#else
            for (i=1; i<NEXUS_NUM_VIDEO_DECODERS; i++)  /* leave one decoder for local main decode */
#endif
            {
                transcoderDst->videoDecoder = NEXUS_VideoDecoder_Open(i, &videoDecoderOpenSettings); /* take default capabilities */
                if (transcoderDst->videoDecoder)
                    break;
            }   
            if (!transcoderDst->videoDecoder) {
                BDBG_ERR(("%s: ERROR: Can't get a free Decoder Handle, max idx %d", __FUNCTION__, i));
                goto error;
            }
        }
    }
    /* leave the first few stc channels for local decode purposes */
    NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
    stcSettings.timebase = transcoderDst->timebase;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;
    BDBG_MSG(("%s: using timebase %d for encode", __FUNCTION__, stcSettings.timebase));
    stcSettings.autoConfigTimebase = false;
    transcoderDst->encodeStcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    if (!transcoderDst->encodeStcChannel) {
        BDBG_ERR(("%s: ERROR: Can't get a free STC Channel Handle, max idx %d", __FUNCTION__, i));
        goto error;
    }
    BDBG_MSG(("%s: opened encode STC Channel id", __FUNCTION__));


    /* open & configure encoder */
    for (i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) {
        transcoderDst->videoEncoder = NEXUS_VideoEncoder_Open(i, NULL); /* take default capabilities */
        if (transcoderDst->videoEncoder)
            break;
    }
    if (!transcoderDst->videoEncoder) {
        BDBG_ERR(("%s: ERROR: Can't get a free Video Encoder Handle, max idx %d", __FUNCTION__, i));
        goto error;
    }
    BDBG_MSG(("%s: opened video encoder %p for transcode ctx %p", __FUNCTION__, transcoderDst->videoEncoder, transcoderDst));

#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
    {
        NEXUS_PlatformConfiguration platformConfig;

        NEXUS_Platform_GetConfiguration(&platformConfig);
        /* Bring up video display and outputs */
        transcoderDst->displayMain = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
        if(platformConfig.outputs.component[0]){
            NEXUS_Display_AddOutput(transcoderDst->displayMain, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
        if(platformConfig.outputs.hdmi[0]){
            NEXUS_Display_AddOutput(transcoderDst->displayMain, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
        }
#endif
        transcoderDst->windowMain = NEXUS_VideoWindow_Open(transcoderDst->displayMain, 0);
        assert(transcoderDst->windowMain);
    }
#endif
    /* open & setup the display: note transcode video format settings are specified here */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    displaySettings.frameRateMaster = NULL;/* disable frame rate tracking for now */

    transcoderDst->displayTranscode = NEXUS_Display_Open(transcoderDst->displayIndex, &displaySettings);/* cmp3 for transcoder */
    if (!transcoderDst->displayTranscode) {
        BDBG_ERR(("%s: ERROR: Can't get a free Display Handle, max idx %d", __FUNCTION__, transcoderDst->displayIndex));
        goto error;
    }

    NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
    customFormatSettings.width = ipStreamerCfg->transcode.outWidth;
    customFormatSettings.height = ipStreamerCfg->transcode.outHeight;
    customFormatSettings.refreshRate = 60000;
    customFormatSettings.interlaced = ipStreamerCfg->transcode.outInterlaced;
    customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
    customFormatSettings.sampleAspectRatio.x = ipStreamerCfg->transcode.outWidth;
    customFormatSettings.sampleAspectRatio.y = ipStreamerCfg->transcode.outHeight;
    customFormatSettings.dropFrameAllowed = true;
    if (NEXUS_Display_SetCustomFormatSettings(transcoderDst->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings) != 0) {
        BDBG_ERR(("%s: ERROR: NEXUS_Display_SetCustomFormatSettings() Failed", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Successfully Set Custom Format settings for display", __FUNCTION__));

    if ((transcoderDst->windowTranscode = NEXUS_VideoWindow_Open(transcoderDst->displayTranscode, 0)) == NULL) {
        BDBG_ERR(("%s: ERROR: Can't get a free Video Window on Display", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: display %p & video window %p are opened for transcoder ctx %p", __FUNCTION__, transcoderDst->displayTranscode, transcoderDst->windowTranscode, transcoderDst));

    /* turn off svc/mvc decodes to save heap memory space */
    NEXUS_VideoDecoder_GetSettings(transcoderDst->videoDecoder, &videoDecoderSettings);
    for (i=0;i<NEXUS_VideoCodec_eMax;i++) {
        switch (videoDecoderSettings.supportedCodecs[i]) {
            case NEXUS_VideoCodec_eH264_Svc:
            case NEXUS_VideoCodec_eH264_Mvc:
                videoDecoderSettings.supportedCodecs[i] = NEXUS_VideoCodec_eNone;
                break;
            default:
                break;
        }
    }
#ifndef IP_STREAMER_DEMO_MODE
    videoDecoderSettings.sourceChanged.callback = sourceChangedCallback;
    videoDecoderSettings.sourceChanged.context = ipStreamerCtx;
#endif
    if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
        videoDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;    /* In 45Khz clock */
    }
    if (NEXUS_VideoDecoder_SetSettings(transcoderDst->videoDecoder, &videoDecoderSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: Failed to turn off SVC/MVC decode option on 2nd decoder", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: turned off SVC/MVC decode option on 2nd decoder", __FUNCTION__));

    /* set transcoder minimum display format before AddInput to avoid black frame transition during dynamic resolution change */
    NEXUS_VideoWindow_GetSettings(transcoderDst->windowTranscode, &windowSettings);
    windowSettings.minimumDisplayFormat = NEXUS_VideoFormat_e1080p;
    windowSettings.scaleFactorRounding.enabled = false;
    windowSettings.scaleFactorRounding.horizontalTolerance = 0;
    windowSettings.scaleFactorRounding.verticalTolerance   = 0;
    if (NEXUS_VideoWindow_SetSettings(transcoderDst->windowTranscode, &windowSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_SetSettings Failed for the transcode window", __FUNCTION__));
        goto error;
    }

    /* set transcoder window vnet mode bias to avoid black frame transition during dynamic resolution change */
    NEXUS_VideoWindow_GetScalerSettings(transcoderDst->windowTranscode, &scalerSettings);
    scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerAfterCapture;
    scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
    if (NEXUS_VideoWindow_SetScalerSettings(transcoderDst->windowTranscode, &scalerSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_SetScalerSettings Failed for transcode window", __FUNCTION__));
        goto error;
    }

#if defined(NEXUS_HAS_VIDEO_ENCODER) && defined(NEXUS_HAS_HDMI_INPUT)
    if (ipStreamerCfg->srcType == IpStreamerSrc_eHdmi) {
        /* connect hdmi to the display (which feeds into encoder) */
        if (NEXUS_VideoWindow_AddInput(transcoderDst->windowTranscode, NEXUS_HdmiInput_GetVideoConnector(ipStreamerCtx->hdmiSrc->hdmiInput)) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_AddInput() failed to connect decoder to display", __FUNCTION__));
            goto error;
        }

#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
        NEXUS_VideoWindow_AddInput(transcoderDst->windowMain, NEXUS_HdmiInput_GetVideoConnector(ipStreamerCtx->hdmiSrc->hdmiInput));
#endif
    }
    else
#endif
    {
        /* connect decoder to the display (which feeds into encoder) */
        if (NEXUS_VideoWindow_AddInput(transcoderDst->windowTranscode, NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder)) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_AddInput() failed to connect decoder to display", __FUNCTION__));
            goto error;
        }
        if (ipStreamerCfg->transcode.outAudio) {
            NEXUS_PlatformConfiguration platformConfig;
            NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;

            NEXUS_Platform_GetConfiguration(&platformConfig);
            NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
            if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
                audioDecoderOpenSettings.fifoSize = 512*1024; /* increasing to act as de-jitter buffer for ip */
                BDBG_MSG(("%s: Increasing Audio Decoder Fifo size to %d", __FUNCTION__, audioDecoderOpenSettings.fifoSize));
            }

            for (i=2; i<NEXUS_NUM_AUDIO_DECODERS; i++) /* leave 1 & 2 decoders for local app */
            {
                /* May be unnecessary after SW7435-66 */
#if (BCHP_CHIP == 7435)
                audioDecoderOpenSettings.dspIndex = i>(NEXUS_NUM_AUDIO_DECODERS/NEXUS_NUM_AUDIO_DSP)? 1:0;
#endif
                transcoderDst->audioDecoder = NEXUS_AudioDecoder_Open(i, &audioDecoderOpenSettings);
                if (transcoderDst->audioDecoder) 
                    break;
            }
            if (!transcoderDst->audioDecoder) {
                BDBG_ERR(("%s: ERROR: Can't open audio decoder upto id %d", __FUNCTION__, i-1));
                goto error;
            }
            BDBG_MSG(("%s: opened audio decoder %p for transcode ctx %p", __FUNCTION__, transcoderDst->audioDecoder, transcoderDst));

            /* Create audio mux output */
            if ((transcoderDst->audioMuxOutput = NEXUS_AudioMuxOutput_Create(NULL)) == NULL) {
                BDBG_ERR(("%s: ERROR: Can't create Audio Stream Mux handle", __FUNCTION__));
                goto error;
            }
            BDBG_MSG(("%s: Audio Stream Mux %p is created for transcoder ctx %p", __FUNCTION__, transcoderDst->audioMuxOutput, ipStreamerCtx->transcoderDst));

            /* Open audio encoder */
            NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
            encoderSettings.codec = ipStreamerCfg->transcode.outAudioCodec;
            transcoderDst->audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
            if (!transcoderDst->audioEncoder) {
                BDBG_ERR(("%s: ERROR: Can't open audio encoder handle", __FUNCTION__));
                goto error;
            }

            /* Connect audio decoder to output */
            
            if(ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv)
            {/*in multi process environment, NEXUS server process makes the connection*/
                if( !ipStreamerCtx->globalCtx->globalCfg.connectAudioDummy(transcoderDst->audioDecoder, transcoderDst->contextId))
                {               
                    BDBG_ERR(("%s: ERROR: NEXUS_AudioOutput_AddInput() failed to add spdif audio input", __FUNCTION__));
                    goto error;
                }
            }
            else
            {            
                if (NEXUS_AudioOutput_AddInput(
                                NEXUS_AudioDummyOutput_GetConnector(platformConfig.outputs.audioDummy[transcoderDst->contextId]),
                                NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo)) != NEXUS_SUCCESS) {
                        BDBG_ERR(("%s: ERROR: NEXUS_AudioOutput_AddInput() failed to add spdif audio input", __FUNCTION__));
                        goto error;
                }
            }

            /* Connect encoder to decoder */
            if (NEXUS_AudioEncoder_AddInput(
                        transcoderDst->audioEncoder,
                        NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo)) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: ERROR: NEXUS_AudioEncoder_AddInput() failed to connect encoder w/ decoder", __FUNCTION__));
                goto error;
            }

            /* Connect mux to encoder */
            if (NEXUS_AudioOutput_AddInput(
                NEXUS_AudioMuxOutput_GetConnector(transcoderDst->audioMuxOutput), 
                NEXUS_AudioEncoder_GetConnector(transcoderDst->audioEncoder)) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: ERROR: NEXUS_AudioOutput_AddInput() failed to connect mux to encoder", __FUNCTION__));
                goto error;
            }
            BDBG_MSG(("%s: audio encode is successfully setup", __FUNCTION__));
        }

#ifdef B_HAS_DISPLAY_LOCAL_FOR_ENCODE
        NEXUS_VideoWindow_AddInput(transcoderDst->windowMain, NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder));
#endif
    }
    NEXUS_VideoWindow_GetMadSettings(transcoderDst->windowTranscode, &madSettings);
    madSettings.deinterlace = true;
    madSettings.enable22Pulldown = true;
    madSettings.enable32Pulldown = true;
    if (NEXUS_VideoWindow_SetMadSettings(transcoderDst->windowTranscode, &madSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: ERROR: NEXUS_VideoWindow_SetMadSettings() failed to enable MAD ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Enabled MAD De-interlacer", __FUNCTION__));

    /* display is connected to the encoder module during encoder start */

    /* open the stream mux */
    NEXUS_StreamMux_GetDefaultCreateSettings(&muxCreateSettings);
    muxCreateSettings.finished.callback = transcoderFinishCallback;
    muxCreateSettings.finished.context = transcoderDst;
    muxCreateSettings.finished.param = 0;
    if ((transcoderDst->streamMux = NEXUS_StreamMux_Create(&muxCreateSettings)) == NULL) {
        BDBG_ERR(("%s: ERROR: Can't create Stream Mux handle", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Stream Mux %p is created for transcoder ctx %p", __FUNCTION__, transcoderDst->streamMux, ipStreamerCtx->transcoderDst));


    /* and finally open the playpump channels that are used by mux to feed the Audio & Video ES as well as System data to the xpt h/w */
    /* that is where the actual muxing of stream happens */

    /* reduce FIFO size allocated for playpump used for the transcoded streams */
    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpConfig);
    playpumpConfig.fifoSize = 16384; /* reduce FIFO size allocated for playpump */
    playpumpConfig.numDescriptors = 64; /* set number of descriptors */
    playpumpConfig.streamMuxCompatible = true;
    
    if(ipStreamerCtx->globalCtx->globalCfg.multiProcessEnv)
    {       
        NEXUS_Platform_GetClientConfiguration(&clientConfig);    
        playpumpConfig.heap = clientConfig.heap[1]; /* playpump requires heap with eFull mapping */
    }

    if ((transcoderDst->playpumpTranscodeVideo = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig)) == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to Open Nexus Playpump ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Nexus Playpump is opened for video transcoder ctx %p", __FUNCTION__, ipStreamerCtx->transcoderDst));
    if ((transcoderDst->playpumpTranscodeAudio = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig)) == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to Open Nexus Playpump ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Nexus Playpump is opened for audio transcoder ctx %p", __FUNCTION__, ipStreamerCtx->transcoderDst));
    if ((transcoderDst->playpumpTranscodeSystemData = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig)) == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to Open Nexus Playpump ", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: Nexus Playpump is opened for system data transcoder ctx %p", __FUNCTION__, ipStreamerCtx->transcoderDst));
    BDBG_MSG(("%s: transcoder ctx %p setup is successful", __FUNCTION__, ipStreamerCtx->transcoderDst));

    return transcoderDst;

error:
    closeNexusTranscoderPipe(ipStreamerCtx, transcoderDst);
    ipStreamerCtx->transcoderDst = NULL;
    return NULL;
}

#endif /* NEXUS_NUM_DSP_VIDEO_ENCODERS */

TranscoderDst *
openNexusTranscoderPipe(
        IpStreamerConfig *ipStreamerCfg,
        IpStreamerCtx *ipStreamerCtx
        )
{
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
    return openNexusRaagaTranscoderPipe(ipStreamerCfg, ipStreamerCtx);
#else
    return openNexusVice2TranscoderPipe(ipStreamerCfg, ipStreamerCtx);
#endif
}

extern int mapFrameRateToGopSize(NEXUS_VideoFrameRate frameRate);
bool
B_IpStreamer_InsertPatPmtTables(
    void *ctx
    )
{
#if 0
    /* commented out till I decide whether app should periodically insert PAT/PMT or let it happen via the a periodic timer thread */
    NEXUS_Error rc = NEXUS_UNKNOWN;
    IpStreamerCtx *ipStreamerCtx = (IpStreamerCtx *)ctx;
    TranscoderDst *transcoderDst;

    if (!ipStreamerCtx)
        return false;
    transcoderDst = ipStreamerCtx->transcoderDst;
    if (!transcoderDst)
        return false;

    rc = NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[0]);
    if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }

    rc = NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[1]);
    if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }

    if (0)
    {
        /* test code to dynamically control the bitrate */
        int bitrate;
        NEXUS_VideoEncoderSettings videoEncoderConfig;
        BDBG_WRN(("enter new bitrate........."));
        scanf("%d", &bitrate);
        BDBG_WRN(("entered new bitrate.........: %d", bitrate));

        NEXUS_VideoEncoder_GetSettings(transcoderDst->videoEncoder, &videoEncoderConfig);
        videoEncoderConfig.bitrateMax = bitrate;
        NEXUS_VideoEncoder_SetSettings(transcoderDst->videoEncoder, &videoEncoderConfig);
        BDBG_WRN(("%s: ################### modified bitrate to %d ###############", __FUNCTION__, bitrate));

    }
    /* TODO: this sleep shouldn't be needed anymore here, take it out */
    BKNI_Sleep(100);
    return true;;
error:
    return false;;
#else
    BSTD_UNUSED(ctx);
    return true;
#endif
}

extern void * B_PlaybackIp_UtilsAllocateMemory(int size);
#if 0
#define RECORD_CLEAR_DATA
#endif
#ifdef RECORD_CLEAR_DATA
#include <stdio.h>
static FILE * fclear = NULL;
#endif
void 
_sendPatPmtOnce(
    IpStreamerCtx *ipStreamerCtx
    )
{
    int sent;
    TranscoderDst *transcoderDst = ipStreamerCtx->transcoderDst;
    uint8_t ccByte;

    if (!transcoderDst)
        return;

    /* Note: we are inserting PAT/PMT manually as for some reason non-Broadcom clients are not working without it */
    /* that needs to be further looked at */
    transcoderDst->ccValue++;/* increment CC synchronously with PAT/PMT */
    ccByte = *((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4); /* the 1st byte of pat/pmt arrays is for TSheader builder use */

    /* need to increment CC value for PAT/PMT packets */
    ccByte = (ccByte & 0xf0) | (transcoderDst->ccValue & 0xf);
    *((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4) = ccByte;
    /* need to flush the cache before set to TS mux hw */
    NEXUS_Memory_FlushCache((void*)((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4), 1);
    /* ping pong PAT pointer */
    transcoderDst->psi[0].pData = (void*)((uint8_t*)transcoderDst->pat[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 1);

    ccByte = *((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4);
    ccByte = (ccByte & 0xf0) | (transcoderDst->ccValue & 0xf);
    *((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4) = ccByte;
    NEXUS_Memory_FlushCache((void*)((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 4), 1);
    /* ping pong PMT pointer */
    transcoderDst->psi[1].pData = (void*)((uint8_t*)transcoderDst->pmt[transcoderDst->ccValue % IP_STREAMER_PSI_QUEUE_CNT] + 1);

    /* code to manually send out PAT/PMT once */
    sent = write(ipStreamerCtx->cfg.streamingFd, transcoderDst->psi[0].pData, 188);
    if (sent != 188) BDBG_WRN(("!!!!!!!!!!!!!1Failed to send PAT........."));
    sent = write(ipStreamerCtx->cfg.streamingFd, transcoderDst->psi[1].pData, 188);
    if (sent != 188) BDBG_WRN(("!!!!!!!!!!!!!1Failed to send PMT........."));
    BDBG_MSG(("%s: !!!!!!!! directly sending PAT & PMT over n/w sd %d once .........", __FUNCTION__, ipStreamerCtx->cfg.streamingFd));
    transcoderDst->ccValue++;/* increment CC synchronously with PAT/PMT */
#ifdef RECORD_CLEAR_DATA
    if(fclear == NULL)
        fclear = fopen("/data/videos/patPmt.ts", "w+b");
    if (fclear) {
        fwrite(transcoderDst->psi[0].pData, 1, 188, fclear); 
        fwrite(transcoderDst->psi[1].pData, 1, 188, fclear); 
        fflush(fclear);
    }
#endif
}

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
#define startNexusTranscoderDst startNexusRaagaTranscoderDst
int
startNexusRaagaTranscoderDst(
    B_PlaybackIpPsiInfo *psi,
    IpStreamerConfig *ipStreamerCfg,
    IpStreamerCtx *ipStreamerCtx
    )
{
    NEXUS_Error rc = NEXUS_UNKNOWN;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;
    NEXUS_VideoEncoderSettings videoEncoderConfig;
    NEXUS_VideoEncoderDelayRange videoDelay;
    NEXUS_StreamMuxStartSettings muxConfig;
    NEXUS_StreamMuxOutput muxOutput;
    NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
    unsigned Dee = 0;
    NEXUS_AudioMuxOutputDelayStatus audioDelayStatus;
    TranscoderDst *transcoderDst = ipStreamerCtx->transcoderDst;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelSettings syncChannelSettings;
#endif
    NEXUS_StcChannelSettings stcSettings;

    if (transcoderDst->started) {
        BDBG_MSG(("%s: transcoder dst (%p) is already started, refCount %d", __FUNCTION__, transcoderDst, transcoderDst->refCount));
        ipStreamerCtx->transcodingInProgress = true;
        ipStreamerCtx->transcodeVideoPidChannel = transcoderDst->transcodeVideoPidChannelCopy;
        ipStreamerCtx->transcodeAudioPidChannel = transcoderDst->transcodeAudioPidChannelCopy;
        _sendPatPmtOnce(ipStreamerCtx);
        return 0;
    }

    if (B_IpStreamer_SetupSystemData(ipStreamerCtx) == false) {
        BDBG_ERR(("%s: CTX %p: Failed to setup the system data for PSI insertion", __FUNCTION__, ipStreamerCtx));
        goto error;
    }
    _sendPatPmtOnce(ipStreamerCtx);

#if NEXUS_HAS_SYNC_CHANNEL
    /* connect sync channel */
    NEXUS_SyncChannel_GetSettings(transcoderDst->syncChannel, &syncChannelSettings);
    syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder);
    if (transcoderDst->audioDecoder)
        syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
    NEXUS_SyncChannel_SetSettings(transcoderDst->syncChannel, &syncChannelSettings);
#endif

    NEXUS_VideoEncoder_GetDefaultStartSettings(&videoEncoderStartConfig);
    videoEncoderStartConfig.codec = ipStreamerCtx->cfg.transcode.outVideoCodec;
    if (ipStreamerCtx->cfg.transcode.outVideoCodec == NEXUS_VideoCodec_eH264) {
        videoEncoderStartConfig.profile = NEXUS_VideoCodecProfile_eBaseline;
        videoEncoderStartConfig.level = NEXUS_VideoCodecLevel_e30;
    }
    else {
        videoEncoderStartConfig.profile = NEXUS_VideoCodecProfile_eMain;
        videoEncoderStartConfig.level = NEXUS_VideoCodecLevel_eHigh;
    }
    videoEncoderStartConfig.input = transcoderDst->displayTranscode;
    videoEncoderStartConfig.bounds.inputDimension.max.width = ipStreamerCfg->transcode.outWidth;
    videoEncoderStartConfig.bounds.inputDimension.max.height = ipStreamerCfg->transcode.outHeight;
    videoEncoderStartConfig.stcChannel = transcoderDst->encodeStcChannel;

    /* these encoder settings can also be updated during runtime, that is why they are in a different structure */
    NEXUS_VideoEncoder_GetSettings(transcoderDst->videoEncoder, &videoEncoderConfig);
    videoEncoderConfig.bitrateMax = ipStreamerCfg->transcode.transportBitrate;
    videoEncoderConfig.frameRate = ipStreamerCfg->transcode.outFrameRate;
    videoEncoderConfig.streamStructure.framesP = 29;
    videoEncoderConfig.streamStructure.framesB = 0;
    /* NOTE: video encoder delay is in 27MHz ticks */
    NEXUS_VideoEncoder_GetDelayRange(transcoderDst->videoEncoder, &videoEncoderConfig, &videoEncoderStartConfig, &videoDelay);
    BDBG_MSG(("%s: Video encoder end-to-end delay, min = %u ms; maximum allowed: %u ms", __FUNCTION__, videoDelay.min/27000, videoDelay.max/27000));

    if (ipStreamerCtx->cfg.transcode.outAudio) {
        NEXUS_AudioMuxOutput_GetDelayStatus(transcoderDst->audioMuxOutput, ipStreamerCtx->cfg.transcode.outAudioCodec, &audioDelayStatus);
        BDBG_MSG(("%s: Audio codec %d end-to-end delay = %u ms", __FUNCTION__, ipStreamerCtx->cfg.transcode.outAudioCodec, audioDelayStatus.endToEndDelay));
        Dee = audioDelayStatus.endToEndDelay * 27000; /* in 27MHz ticks */
        if (Dee > videoDelay.min) {
            if (Dee > videoDelay.max) {
                Dee = videoDelay.max;
                BDBG_WRN(("Audio Dee is way too big! Use video Dee max %u", Dee));
            }
            else {
                BDBG_MSG(("Use audio Dee %u ms %u ticks@27Mhz", Dee/27000, Dee));
            }
        } else {
            Dee = videoDelay.min;
            BDBG_MSG(("%s: Use video Dee %u ms or %u ticks@27Mhz!", __FUNCTION__, Dee/27000, Dee));
        }
    }
    videoEncoderConfig.encoderDelay = Dee;
    BDBG_MSG(("%s: Video Encoder Settings: FR %d, VFR %d, Bitrate %d, #P %d, #B %d, Delay %d", __FUNCTION__, 
                videoEncoderConfig.frameRate,
                videoEncoderConfig.variableFrameRate,
                videoEncoderConfig.bitrateMax,
                videoEncoderConfig.streamStructure.framesP,
                videoEncoderConfig.streamStructure.framesB,
                videoEncoderConfig.encoderDelay
                ));
    if (NEXUS_VideoEncoder_SetSettings(transcoderDst->videoEncoder, &videoEncoderConfig)) {
        BDBG_ERR(("%s: ERROR: Failed to set the Video Encoder Configuration", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: video encoder %p settings are updated for transcode ctx %p", __FUNCTION__, transcoderDst->videoEncoder, transcoderDst));

    /* start mux */
    NEXUS_StreamMux_GetDefaultStartSettings(&muxConfig);
    muxConfig.transportType = ipStreamerCtx->cfg.transcode.transportType;
    muxConfig.stcChannel = transcoderDst->encodeStcChannel;
    muxConfig.video[0].pid = ipStreamerCtx->cfg.transcode.outVideoPid;
    muxConfig.video[0].encoder = transcoderDst->videoEncoder;
    muxConfig.video[0].playpump = transcoderDst->playpumpTranscodeVideo;
    if (ipStreamerCtx->cfg.transcode.outAudio) {
        muxConfig.audio[0].pid = ipStreamerCtx->cfg.transcode.outAudioPid;
        muxConfig.audio[0].muxOutput = transcoderDst->audioMuxOutput;
        muxConfig.audio[0].playpump = transcoderDst->playpumpTranscodeAudio;
    }
    muxConfig.pcr.pid = ipStreamerCtx->cfg.transcode.outPcrPid;
    muxConfig.pcr.playpump = transcoderDst->playpumpTranscodeSystemData;
    muxConfig.pcr.interval = 50; /* PCR interval */
    rc = NEXUS_StreamMux_Start(transcoderDst->streamMux, &muxConfig, &muxOutput);
    if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }

    BDBG_MSG(("%s: Stream Mux is started for transcoder ctx %p", __FUNCTION__, transcoderDst));
    ipStreamerCtx->transcodeVideoPidChannel = muxOutput.video[0];
    transcoderDst->transcodeVideoPidChannelCopy = ipStreamerCtx->transcodeVideoPidChannel;
    if (ipStreamerCtx->cfg.transcode.outAudio) {
        ipStreamerCtx->transcodeAudioPidChannel = muxOutput.audio[0];
        transcoderDst->transcodeAudioPidChannelCopy = ipStreamerCtx->transcodeAudioPidChannel;
    }
    /* opening of decodeStcChannel is deferred to this point as StcChannel_Open now requires app to provide pcrPidChannel */
    /* which is not available until PSI discovery is complete after the open call. */
    NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
    stcSettings.timebase = transcoderDst->timebase;
    if (ipStreamerCfg->srcType == IpStreamerSrc_eFile) {
        stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    }
#ifdef NEXUS_HAS_HDMI_INPUT
    else if (ipStreamerCfg->srcType == IpStreamerSrc_eHdmi) {
        stcSettings.timebase = ipStreamerCtx->hdmiSrc->timebase;
        /* need STC channel for audio decoder */
        stcSettings.autoConfigTimebase = false;
        stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    }
#endif
    else if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
        /* since live ip has much higher jitter, we need to increase the error thresholds in the dpcr block */

        /* Update STC Channel Settings to accomodate Network Jitter */
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* Live Mode */
        /* offset threshold: uses upper 32 bits (183ticks/msec) of PCR clock */
        stcSettings.modeSettings.pcr.offsetThreshold = IP_NETWORK_JITTER * 183; 
        /* max pcr error: uses upper 32 bits (183ticks/msec) of PCR clock */
        stcSettings.modeSettings.pcr.maxPcrError =  IP_NETWORK_JITTER * 183;
        if (ipStreamerCtx->transcoderDst->pcrPidChannel)
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->pcrPidChannel;
        else
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->videoPidChannel;
        /*  PCR Offset "Jitter Adjustment" is not suitable for use with IP channels Channels, so disable it */
        stcSettings.modeSettings.pcr.disableJitterAdjustment = true;
        /* Disable Auto Timestamp correction for PCR Jitter */
        stcSettings.modeSettings.pcr.disableTimestampCorrection = true;
        /* We just configured the Timebase, so turn off auto timebase config */
        stcSettings.autoConfigTimebase = false;
        BDBG_MSG (("%s: Configured stc channel with high jitter %d", __FUNCTION__, IP_NETWORK_JITTER));
    }
    else {
        stcSettings.mode = NEXUS_StcChannelMode_ePcr;
        if (ipStreamerCtx->pcrPidChannel)
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->pcrPidChannel;
        else
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->videoPidChannel; /* PCR happens to be on video pid */
    }
    transcoderDst->decodeStcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    if (!transcoderDst->decodeStcChannel) {
        BDBG_ERR(("%s: ERROR: Can't get a free STC Channel Handle", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: opened decode STC Channel", __FUNCTION__));

    if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
        /* program the timebase 0: increase its track range & max pcr errors */
        NEXUS_TimebaseSettings timebaseSettings;
        NEXUS_Timebase_GetSettings(transcoderDst->timebase, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.freeze = false;
        if (ipStreamerCtx->transcoderDst->pcrPidChannel)
            timebaseSettings.sourceSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->pcrPidChannel;
        else
            timebaseSettings.sourceSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->videoPidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = IP_NETWORK_JITTER * 183/2;    /* in milliseconds: based on 90Khz clock */
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e244ppm;
        rc = NEXUS_Timebase_SetSettings(transcoderDst->timebase, &timebaseSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, returning..", rc, __LINE__)); goto error;}
        BDBG_MSG (("%s: Configured timebase with high jitter %d", __FUNCTION__, IP_NETWORK_JITTER));
    }

    if (ipStreamerCtx->cfg.srcType != IpStreamerSrc_eHdmi) {
        /* Start decoder */
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = psi->videoCodec;
        videoProgram.pidChannel = transcoderDst->videoPidChannel;
        videoProgram.stcChannel = transcoderDst->decodeStcChannel;
        rc = NEXUS_VideoDecoder_Start(transcoderDst->videoDecoder, &videoProgram);
        if (rc) {
            BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__));
            goto error;
        }
        BDBG_MSG(("%s: Video Decoder is started for transcoder ctx %p", __FUNCTION__, transcoderDst));
    }

    if (psi->audioPid && ipStreamerCtx->cfg.transcode.outAudio) {
        NEXUS_AudioDecoderStartSettings audioProgram;

        NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
        audioMuxStartSettings.stcChannel = transcoderDst->encodeStcChannel;
        audioMuxStartSettings.presentationDelay = Dee/27000; /* in ms */

        rc = NEXUS_AudioMuxOutput_Start(transcoderDst->audioMuxOutput, &audioMuxStartSettings);
        if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }
        BDBG_MSG(("%s: Audio Mux is started for transcoder ctx %p", __FUNCTION__, transcoderDst));

        if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
            NEXUS_AudioDecoderSettings audioDecoderSettings;
            NEXUS_AudioDecoder_GetSettings(transcoderDst->audioDecoder, &audioDecoderSettings);		
            audioDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;    /* In 45Khz clock */
            rc = NEXUS_AudioDecoder_SetSettings(transcoderDst->audioDecoder, &audioDecoderSettings);
            if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning..", rc, __LINE__)); goto error;}
        }

        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
#if defined(NEXUS_HAS_HDMI_INPUT)
        if (ipStreamerCtx->cfg.srcType == IpStreamerSrc_eHdmi) {
            audioProgram.input = NEXUS_HdmiInput_GetAudioConnector(ipStreamerCtx->hdmiSrc->hdmiInput);
            audioProgram.delayMode = NEXUS_AudioDecoderDelayMode_eFixed;
        }
        else 
#endif
        {
            audioProgram.codec = psi->audioCodec;
            audioProgram.pidChannel = transcoderDst->audioPidChannel;
            audioProgram.stcChannel = transcoderDst->decodeStcChannel;
        }
        rc = NEXUS_AudioDecoder_Start(transcoderDst->audioDecoder, &audioProgram);
        if (rc) {
            BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__));
            goto error;
        }
        BDBG_MSG(("%s: Audio Decoder is started for transcoder ctx %p", __FUNCTION__, transcoderDst));
    }

    if (!ipStreamerCfg->hlsSession) {
        NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[0]);
        NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[1]);
    }

    rc = NEXUS_VideoEncoder_Start(transcoderDst->videoEncoder, &videoEncoderStartConfig);
    if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }
    BDBG_MSG(("%s: Video Encoder is started for transcoder ctx %p, CTX %p", __FUNCTION__, transcoderDst, ipStreamerCtx));

    ipStreamerCtx->transcodingInProgress = true;
    transcoderDst->started = true;
    BDBG_MSG(("%s: CTX %p: Transcoder Dst %p is started", __FUNCTION__, ipStreamerCtx, transcoderDst));
    return 0;

error:
    /* stop the transcoding pipe first as its stop was delayed to the close call */
    stopNexusTranscoderDst(ipStreamerCtx);
    return -1;
}
#else
#define startNexusTranscoderDst startNexusVice2TranscoderDst
int
startNexusVice2TranscoderDst(
    B_PlaybackIpPsiInfo *psi,
    IpStreamerConfig *ipStreamerCfg,
    IpStreamerCtx *ipStreamerCtx
    )
{
    NEXUS_Error rc = NEXUS_UNKNOWN;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;
    NEXUS_VideoEncoderSettings videoEncoderConfig;
    NEXUS_VideoEncoderDelayRange videoDelay;
    NEXUS_StreamMuxStartSettings muxConfig;
    NEXUS_StreamMuxOutput muxOutput;
    NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
    unsigned Dee = 0;
    NEXUS_AudioMuxOutputDelayStatus audioDelayStatus;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelSettings syncChannelSettings;
#endif
    TranscoderDst *transcoderDst = ipStreamerCtx->transcoderDst;
    NEXUS_StcChannelSettings stcSettings;

    if (transcoderDst->started) {
        BDBG_MSG(("%s: transcoder dst (%p) is already started, refCount %d", __FUNCTION__, transcoderDst, transcoderDst->refCount));
        ipStreamerCtx->transcodingInProgress = true;
        ipStreamerCtx->transcodeVideoPidChannel = transcoderDst->transcodeVideoPidChannelCopy;
        ipStreamerCtx->transcodeAudioPidChannel = transcoderDst->transcodeAudioPidChannelCopy;
        _sendPatPmtOnce(ipStreamerCtx);
        return 0;
    }

    if (B_IpStreamer_SetupSystemData(ipStreamerCtx) == false) {
        BDBG_ERR(("%s: CTX %p: Failed to setup the system data for PSI insertion", __FUNCTION__, ipStreamerCtx));
        goto error;
    }
    _sendPatPmtOnce(ipStreamerCtx);

#if NEXUS_HAS_SYNC_CHANNEL
    /* connect sync channel */
    NEXUS_SyncChannel_GetSettings(transcoderDst->syncChannel, &syncChannelSettings);
    syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(transcoderDst->videoDecoder);
    if (transcoderDst->audioDecoder)
        syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(transcoderDst->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
    NEXUS_SyncChannel_SetSettings(transcoderDst->syncChannel, &syncChannelSettings);
#endif

    NEXUS_VideoEncoder_GetDefaultStartSettings(&videoEncoderStartConfig);
    videoEncoderStartConfig.codec = ipStreamerCtx->cfg.transcode.outVideoCodec;
    if (ipStreamerCtx->cfg.transcode.outVideoCodec == NEXUS_VideoCodec_eH264) {
        videoEncoderStartConfig.profile = NEXUS_VideoCodecProfile_eMain;
        videoEncoderStartConfig.level = NEXUS_VideoCodecLevel_e31;
    }
    else {
        videoEncoderStartConfig.profile = NEXUS_VideoCodecProfile_eMain;
        videoEncoderStartConfig.level = NEXUS_VideoCodecLevel_eHigh;
    }
    videoEncoderStartConfig.input = transcoderDst->displayTranscode;
    videoEncoderStartConfig.stcChannel = transcoderDst->encodeStcChannel;
    videoEncoderStartConfig.adaptiveLowDelayMode = true;
    videoEncoderStartConfig.rateBufferDelay = 0;
    videoEncoderStartConfig.bounds.inputFrameRate.min = NEXUS_VideoFrameRate_e23_976;
    videoEncoderStartConfig.bounds.outputFrameRate.min = NEXUS_VideoFrameRate_e23_976;
    videoEncoderStartConfig.bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

    /* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
    videoEncoderStartConfig.bounds.inputDimension.max.width = 1920;
    videoEncoderStartConfig.bounds.inputDimension.max.height = 1088;

    /* these encoder settings can also be updated during runtime, that is why they are in a different structure */
    NEXUS_VideoEncoder_GetSettings(transcoderDst->videoEncoder, &videoEncoderConfig);
    videoEncoderConfig.bitrateMax = ipStreamerCfg->transcode.transportBitrate;
    /* 10/10/2011: gop tracking feature is not yet enabled in the vice, until then manually ocnfigure the gop */
    if (ipStreamerCtx->cfg.transcode.outVideoCodec == NEXUS_VideoCodec_eH264) {
        if (ipStreamerCtx->cfg.hlsSession)
            videoEncoderConfig.streamStructure.framesP = (mapFrameRateToGopSize(ipStreamerCfg->transcode.outFrameRate) - 1)/2;
        else 
            videoEncoderConfig.streamStructure.framesP = 29;
        videoEncoderConfig.streamStructure.framesB = 0;
    }
    else {
        videoEncoderConfig.streamStructure.framesP = 14;
        videoEncoderConfig.streamStructure.framesB = 0;
    }
    videoEncoderConfig.enableFieldPairing = true;

    videoEncoderConfig.frameRate = ipStreamerCfg->transcode.outFrameRate;
    videoEncoderConfig.variableFrameRate = false;
    videoEncoderConfig.bitrateMax = ipStreamerCfg->transcode.transportBitrate;

    /* NOTE: video encoder delay is in 27MHz ticks */
    NEXUS_VideoEncoder_GetDelayRange(transcoderDst->videoEncoder, &videoEncoderConfig, &videoEncoderStartConfig, &videoDelay);
    BDBG_MSG(("%s: Video encoder end-to-end delay, min = %u ms; maximum allowed: %u ms", __FUNCTION__, videoDelay.min/27000, videoDelay.max/27000));

    if (psi->audioPid && ipStreamerCtx->cfg.transcode.outAudio) {
        NEXUS_AudioMuxOutput_GetDelayStatus(transcoderDst->audioMuxOutput, ipStreamerCtx->cfg.transcode.outAudioCodec, &audioDelayStatus);
        BDBG_MSG(("%s: Audio codec %d end-to-end delay = %u ms", __FUNCTION__, ipStreamerCtx->cfg.transcode.outAudioCodec, audioDelayStatus.endToEndDelay));
        Dee = audioDelayStatus.endToEndDelay * 27000; /* in 27MHz ticks */
        if (Dee > videoDelay.min) {
            if (Dee > videoDelay.max) {
                Dee = videoDelay.max;
                BDBG_WRN(("Audio Dee is way too big! Use video Dee max %u", Dee));
            }
            else {
                BDBG_MSG(("Use audio Dee %u ms %u ticks@27Mhz", Dee/27000, Dee));
            }
        } else {
            Dee = videoDelay.min;
            BDBG_MSG(("%s: Use video Dee %u ms or %u ticks@27Mhz!", __FUNCTION__, Dee/27000, Dee));
        }
        videoEncoderConfig.encoderDelay = Dee;
    }
    else {
        videoEncoderConfig.encoderDelay = videoDelay.min;
    }
    BDBG_MSG(("%s: Video Encoder Settings: FR %d, VFR %d, Bitrate %d, #P %d, #B %d, Delay %d", __FUNCTION__, 
                videoEncoderConfig.frameRate,
                videoEncoderConfig.variableFrameRate,
                videoEncoderConfig.bitrateMax,
                videoEncoderConfig.streamStructure.framesP,
                videoEncoderConfig.streamStructure.framesB,
                videoEncoderConfig.encoderDelay
                ));
    if (NEXUS_VideoEncoder_SetSettings(transcoderDst->videoEncoder, &videoEncoderConfig)) {
        BDBG_ERR(("%s: ERROR: Failed to set the Video Encoder Configuration", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: video encoder %p settings are updated for transcode ctx %p", __FUNCTION__, transcoderDst->videoEncoder, transcoderDst));

    /* start mux */
    NEXUS_StreamMux_GetDefaultStartSettings(&muxConfig);
    muxConfig.transportType = ipStreamerCtx->cfg.transcode.transportType;
    muxConfig.stcChannel = transcoderDst->encodeStcChannel;
    muxConfig.video[0].pid = ipStreamerCtx->cfg.transcode.outVideoPid;
    muxConfig.video[0].encoder = transcoderDst->videoEncoder;
    muxConfig.video[0].playpump = transcoderDst->playpumpTranscodeVideo;
    if (ipStreamerCtx->cfg.transcode.outAudio) {
        muxConfig.audio[0].pid = ipStreamerCtx->cfg.transcode.outAudioPid;
        muxConfig.audio[0].muxOutput = transcoderDst->audioMuxOutput;
        muxConfig.audio[0].playpump = transcoderDst->playpumpTranscodeAudio;
    }
    muxConfig.pcr.pid = ipStreamerCtx->cfg.transcode.outPcrPid;
    muxConfig.pcr.playpump = transcoderDst->playpumpTranscodeSystemData;
    muxConfig.pcr.interval = 50; /* PCR interval */
    rc = NEXUS_StreamMux_Start(transcoderDst->streamMux, &muxConfig, &muxOutput);
    if (rc) { BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__)); goto error; }

    BDBG_MSG(("%s: Stream Mux is started for transcoder ctx %p", __FUNCTION__, transcoderDst));
    ipStreamerCtx->transcodeVideoPidChannel = muxOutput.video[0];
    transcoderDst->transcodeVideoPidChannelCopy = ipStreamerCtx->transcodeVideoPidChannel;
    if (ipStreamerCtx->cfg.transcode.outAudio) {
        ipStreamerCtx->transcodeAudioPidChannel = muxOutput.audio[0];
        transcoderDst->transcodeAudioPidChannelCopy = ipStreamerCtx->transcodeAudioPidChannel;
    }

    /* opening of decodeStcChannel is deferred to this point as StcChannel_Open now requires app to provide pcrPidChannel */
    /* which is not available until PSI discovery is complete after the open call. */
    NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
    stcSettings.timebase = transcoderDst->timebase;
    if (ipStreamerCfg->srcType == IpStreamerSrc_eFile) {
        stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    }
    else if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
        /* since live ip has much higher jitter, we need to increase the error thresholds in the dpcr block */

        /* Update STC Channel Settings to accomodate Network Jitter */
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* Live Mode */
        /* offset threshold: uses upper 32 bits (183ticks/msec) of PCR clock */
        stcSettings.modeSettings.pcr.offsetThreshold = IP_NETWORK_JITTER * 183; 
        /* max pcr error: uses upper 32 bits (183ticks/msec) of PCR clock */
        stcSettings.modeSettings.pcr.maxPcrError =  IP_NETWORK_JITTER * 183;
        if (ipStreamerCtx->transcoderDst->pcrPidChannel)
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->pcrPidChannel;
        else
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->videoPidChannel;
        /*  PCR Offset "Jitter Adjustment" is not suitable for use with IP channels Channels, so disable it */
        stcSettings.modeSettings.pcr.disableJitterAdjustment = true;
        /* Disable Auto Timestamp correction for PCR Jitter */
        stcSettings.modeSettings.pcr.disableTimestampCorrection = true;
        /* We just configured the Timebase, so turn off auto timebase config */
        stcSettings.autoConfigTimebase = false;
        BDBG_MSG (("%s: Configured stc channel with high jitter %d", __FUNCTION__, IP_NETWORK_JITTER));
    }
    else {
        stcSettings.mode = NEXUS_StcChannelMode_ePcr;
        /* enable live mode for transcoding case: it is done here as pidChannel needs to be specified in Stc_SetSettings() */
        if (ipStreamerCtx->pcrPidChannel)
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->pcrPidChannel;
        else
            stcSettings.modeSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->videoPidChannel; /* PCR happens to be on video pid */
    }
    transcoderDst->decodeStcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    if (!transcoderDst->decodeStcChannel) {
        BDBG_ERR(("%s: ERROR: Can't get a free STC Channel Handle", __FUNCTION__));
        goto error;
    }
    BDBG_MSG(("%s: opened decode STC Channel", __FUNCTION__));

    if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
        /* program the timebase 0: increase its track range & max pcr errors */
        NEXUS_TimebaseSettings timebaseSettings;
        NEXUS_Timebase_GetSettings(transcoderDst->timebase, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.freeze = false;
        if (ipStreamerCtx->transcoderDst->pcrPidChannel)
            timebaseSettings.sourceSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->pcrPidChannel;
        else
            timebaseSettings.sourceSettings.pcr.pidChannel = ipStreamerCtx->transcoderDst->videoPidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = IP_NETWORK_JITTER * 183/2;    /* in milliseconds: based on 90Khz clock */
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e244ppm;
        rc = NEXUS_Timebase_SetSettings(transcoderDst->timebase, &timebaseSettings);
        if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, returning..", rc, __LINE__)); goto error;}
        BDBG_MSG (("%s: Configured timebase with high jitter %d", __FUNCTION__, IP_NETWORK_JITTER));
    }

    if (ipStreamerCtx->cfg.srcType != IpStreamerSrc_eHdmi) {
        /* Start decoder */
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = psi->videoCodec;
        videoProgram.pidChannel = transcoderDst->videoPidChannel;
        videoProgram.stcChannel = transcoderDst->decodeStcChannel;
        rc = NEXUS_VideoDecoder_Start(transcoderDst->videoDecoder, &videoProgram);
        if (rc) {
            BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__));
            goto error;
        }
        BDBG_MSG(("%s: Video Decoder is started for transcoder ctx %p", __FUNCTION__, transcoderDst));
    }

    if (psi->audioPid && ipStreamerCtx->cfg.transcode.outAudio) {
        NEXUS_AudioDecoderStartSettings audioProgram;
        NEXUS_AudioDecoderSettings audioDecoderSettings;

        if (ipStreamerCfg->srcType == IpStreamerSrc_eIp) {
            NEXUS_AudioDecoder_GetSettings(transcoderDst->audioDecoder, &audioDecoderSettings);		
            audioDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;    /* In 45Khz clock */
            rc = NEXUS_AudioDecoder_SetSettings(transcoderDst->audioDecoder, &audioDecoderSettings);
            if (rc) {BDBG_WRN(("NEXUS Error (%d) at %d, returning..", rc, __LINE__)); goto error;}
        }
        NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
        audioMuxStartSettings.stcChannel = transcoderDst->encodeStcChannel;
        audioMuxStartSettings.presentationDelay = Dee/27000; /* in ms */
        rc = NEXUS_AudioMuxOutput_Start(transcoderDst->audioMuxOutput, &audioMuxStartSettings);
        if (rc) {
            BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__));
            goto error;
        }
        BDBG_MSG(("%s: Audio Mux is started for transcoder ctx %p", __FUNCTION__, transcoderDst));

        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.codec = psi->audioCodec;
        audioProgram.pidChannel = transcoderDst->audioPidChannel;
        audioProgram.stcChannel = transcoderDst->decodeStcChannel;
        rc = NEXUS_AudioDecoder_Start(transcoderDst->audioDecoder, &audioProgram);
        if (rc) {
            BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__));
            goto error;
        }
        BDBG_MSG(("%s: Audio Decoder is started for transcoder ctx %p", __FUNCTION__, transcoderDst));
    }

    if (!ipStreamerCfg->hlsSession) {
        NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[0]);
        NEXUS_StreamMux_AddSystemDataBuffer(transcoderDst->streamMux, &transcoderDst->psi[1]);
    }

    rc = NEXUS_VideoEncoder_Start(transcoderDst->videoEncoder, &videoEncoderStartConfig);
    if (rc) {
        BDBG_ERR(("NEXUS Error at %d, returning..", __LINE__));
        goto error;
    }
    BDBG_MSG(("%s: Video Encoder is started for transcoder ctx %p, CTX %p", __FUNCTION__, transcoderDst, ipStreamerCtx));

    ipStreamerCtx->transcodingInProgress = true;
    transcoderDst->started = true;
    BDBG_MSG(("%s: CTX %p: Transcoder Dst %p is started", __FUNCTION__, ipStreamerCtx, transcoderDst));
    return 0;

error:
    /* stop the transcoding pipe first as its stop was delayed to the close call */
    stopNexusTranscoderDst(ipStreamerCtx);
    return -1;
}

#endif

int
setupNexusTranscoderPidChannels(
    B_PlaybackIpPsiInfo *psi,
    IpStreamerConfig *ipStreamerCfg,
    IpStreamerCtx *ipStreamerCtx
    )
{
    int rc;
    NEXUS_PlaypumpOpenPidChannelSettings pidChannelSettings;

    /* note: we need to start the xcoder pipe as video pid channel is being created by mux mgr start call */
    BKNI_AcquireMutex(ipStreamerCtx->globalCtx->transcoderDstMutex);
    rc = startNexusTranscoderDst(psi, ipStreamerCfg, ipStreamerCtx);
    BKNI_ReleaseMutex(ipStreamerCtx->globalCtx->transcoderDstMutex);
    if (rc) {BDBG_ERR(("NEXUS Error (%d) at %d, returning...", rc, __LINE__)); return -1;}

    if (ipStreamerCfg->transcode.outPcrPid) {
        NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
        pidChannelSettings.pidType = NEXUS_PidType_eOther;
        ipStreamerCtx->transcodePcrPidChannel = NEXUS_Playpump_OpenPidChannel(ipStreamerCtx->transcoderDst->playpumpTranscodeSystemData, ipStreamerCfg->transcode.outPcrPid, &pidChannelSettings);
    }
    else
        ipStreamerCtx->transcodePcrPidChannel = NULL;

    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
    pidChannelSettings.pidType = NEXUS_PidType_eOther;
    ipStreamerCtx->transcodePatPidChannel = NEXUS_Playpump_OpenPidChannel(ipStreamerCtx->transcoderDst->playpumpTranscodeSystemData, ipStreamerCfg->transcode.outPatPid, &pidChannelSettings);

    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
    pidChannelSettings.pidType = NEXUS_PidType_eOther;
    ipStreamerCtx->transcodePmtPidChannel = NEXUS_Playpump_OpenPidChannel(ipStreamerCtx->transcoderDst->playpumpTranscodeSystemData, ipStreamerCfg->transcode.outPmtPid, &pidChannelSettings);
    BDBG_MSG(("%s: allocated pat pid ch pat %p, ch %p, pcr %p", __FUNCTION__, ipStreamerCtx->transcodePatPidChannel, ipStreamerCtx->transcodePmtPidChannel, ipStreamerCtx->transcodePcrPidChannel));
    return 0;
}

void
closeNexusTranscoderPidChannels(
    IpStreamerCtx *ipStreamerCtx
    )
{
    BDBG_MSG(("%s: Closing", __FUNCTION__));
    if (ipStreamerCtx->transcodePcrPidChannel) {
        NEXUS_Playpump_ClosePidChannel(ipStreamerCtx->transcoderDst->playpumpTranscodeSystemData, ipStreamerCtx->transcodePcrPidChannel);
        ipStreamerCtx->transcodePcrPidChannel = NULL;
    }
    if (ipStreamerCtx->transcodePatPidChannel) {
        NEXUS_Playpump_ClosePidChannel(ipStreamerCtx->transcoderDst->playpumpTranscodeSystemData, ipStreamerCtx->transcodePatPidChannel);
        ipStreamerCtx->transcodePatPidChannel = NULL;
    }
    if (ipStreamerCtx->transcodePmtPidChannel) {
        NEXUS_Playpump_ClosePidChannel(ipStreamerCtx->transcoderDst->playpumpTranscodeSystemData, ipStreamerCtx->transcodePmtPidChannel);
        ipStreamerCtx->transcodePmtPidChannel = NULL;
    }
    if (ipStreamerCtx->transcoderDst->videoPidChannel) {
        NEXUS_PidChannel_Close(ipStreamerCtx->transcoderDst->videoPidChannel);
        ipStreamerCtx->transcoderDst->videoPidChannel = NULL;
    }
    if (ipStreamerCtx->transcoderDst->audioPidChannel) {
        NEXUS_PidChannel_Close(ipStreamerCtx->transcoderDst->audioPidChannel);
        ipStreamerCtx->transcoderDst->audioPidChannel = NULL;
    }
    if (ipStreamerCtx->transcoderDst->pcrPidChannel) {
        NEXUS_PidChannel_Close(ipStreamerCtx->transcoderDst->pcrPidChannel);
        ipStreamerCtx->transcoderDst->pcrPidChannel = NULL;
    }
    BDBG_MSG(("%s: Closed", __FUNCTION__));
}

int
openNexusTranscoderPidChannels(
    B_PlaybackIpPsiInfo *psi,
    IpStreamerConfig *ipStreamerCfg,
    IpStreamerCtx *ipStreamerCtx
    )
{
    /* for transcode paths: live pid channels -> AV decoders && xcode pid channels -> recpump */
    if (ipStreamerCtx->transcoderDst->refCount == 1) {
        /* allocate pid channels for 1st instance of a xcode session (which can be shared among clients) */
        if (psi->videoPid)
            ipStreamerCtx->transcoderDst->videoPidChannel = NEXUS_PidChannel_Open(ipStreamerCtx->parserBandPtr->parserBand, psi->videoPid, NULL);
        else
            ipStreamerCtx->transcoderDst->videoPidChannel = NULL;
        if (psi->audioPid)
            ipStreamerCtx->transcoderDst->audioPidChannel = NEXUS_PidChannel_Open(ipStreamerCtx->parserBandPtr->parserBand, psi->audioPid, NULL);
        else
            ipStreamerCtx->transcoderDst->audioPidChannel = NULL;
        if (psi->pcrPid != psi->videoPid) {
            ipStreamerCtx->transcoderDst->pcrPidChannel = NEXUS_PidChannel_Open(ipStreamerCtx->parserBandPtr->parserBand, psi->pcrPid, NULL);
        }
        else
            ipStreamerCtx->transcoderDst->pcrPidChannel = NULL;
    }
    /* else: for xcode sessions w/ refCount > 1, simple xcode path is already setup, we dont need to create any pidChannels for live -> AV decoder path */

    if (setupNexusTranscoderPidChannels(psi, ipStreamerCfg, ipStreamerCtx) < 0) {
        BDBG_ERR(("%s: setupNexusTranscoderPidChannels() Failed", __FUNCTION__));
        return -1;
    }
    return 0;
}

