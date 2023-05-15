/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_stream_mux.c $
 * $brcm_Revision: 42 $
 * $brcm_Date: 9/18/12 3:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/stream_mux/src/nexus_stream_mux.c $
 * 
 * 42   9/18/12 3:15p hongtaoz
 * SW7425-3425: always clear the fnrt at stop
 * 
 * 41   9/6/12 9:49a hongtaoz
 * SW7425-3425: added FNRT support to stream mux;
 * 
 * 40   8/16/12 3:28p vsilyaev
 * SW7425-3684: Added duration of muxed portion of stream/file
 * 
 * 39   7/24/12 1:40p vsilyaev
 * SW7425-3540: Use static allocation for large data structures
 * 
 * 38   7/24/12 1:02p vsilyaev
 * SW7425-3540: Use static allocation for large data structures
 * 
 * 37   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 36   7/19/12 12:57p vsilyaev
 * SW7425-3076: Added API to explicitly control memory allocation by the
 *  mux
 * 
 * 35   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 34   5/17/12 11:59a vsilyaev
 * SW7425-2606: Added setting for mux service period
 * 
 * 33   5/11/12 3:30p erickson
 * SW7425-3017: finish last checkin
 * 
 * 32   5/10/12 8:23a erickson
 * SW7425-3017: register pid channels with object database for
 *  multiprocess use
 * 
 * 31   4/30/12 1:30p vsilyaev
 * SW7425-2927: Merged 'Added setting of metadata buffer base address in
 *  NEXUS_StreamMux_P_GetAudioBufferStatus()'
 * 
 * SW7425-1221/1   4/24/12 1:37p nilesh
 * SW7425-2927: Added setting of metadata buffer base address in
 *  NEXUS_StreamMux_P_GetAudioBufferStatus()
 * 
 * 30   3/29/12 4:36p hongtaoz
 * SW7425-2674, SW7435-48: condition stream mux STC channel binary mode
 *  with RT mode;
 * 
 * 29   3/29/12 12:17p vsilyaev
 * SW7425-2694: Allow user to specify the stream PES id
 * 
 * 28   2/28/12 4:19p vsilyaev
 * SW7425-2271: Added option to capture user data
 * 
 * 27   2/27/12 5:43p hongtaoz
 * SW7425-2348: stop mux timer for both finished and stopped states;
 * 
 * 26   2/13/12 3:07p vsilyaev
 * SW7425-2385: Account for different number of pids supported in the mux
 *  library
 * 
 * 25   1/26/12 1:24p vsilyaev
 * SW7425-2271: Added user data configuration
 * 
 * 24   11/29/11 4:28p vsilyaev
 * SW7425-1853: Don't place BMUXlib_TS_StartSettings on stack
 * 
 * 23   10/24/11 2:17p vsilyaev
 * SW7425-830: Added code to initialize uiNumValidXXXPIDs
 * 
 * 22   6/9/11 1:04p vsilyaev
 * SW7425-484: Added non-realtime options
 * 
 * 21   4/14/11 11:14a vsilyaev
 * SW7425-75: Propagate pMetadataBufferBaseAddress through from nexus
 *  video encoder to the muxes
 * 
 * 20   3/14/11 1:48p vsilyaev
 * SW7425-167: Fixed typo
 * 
 * 19   2/10/11 1:08p vsilyaev
 * SW7425-39: Added code to notify application about 'finish' event
 * 
 * 18   2/8/11 6:44p hongtaoz
 * SW7425-39: fixed video PES stream ID assignment;
 * 
 * 17   2/8/11 5:11p vsilyaev
 * SW7425-39: Updated API so it could pass user/kernel proxy
 * 
 * 16   2/7/11 9:49a vsilyaev
 * SW7425-39: Updated to match latest mux library
 * 
 * 15   1/24/11 5:35p hongtaoz
 * SW7425-39: connect TS MUXlib's transport channel interface with audio
 *  mux channel;
 * 
 * 14   1/3/11 2:43p jgarrett
 * SW7425-41: Adding audio pid channels to mux output
 * 
 * 13   12/30/10 7:41p vsilyaev
 * SW7425-39: Removed special timestamp mode
 * 
 * 12   12/30/10 6:42p vsilyaev
 * SW7425-39: Use special timestamp mode for mux
 * 
 * 11   12/30/10 4:20p vsilyaev
 * SW7425-39: Remove playback from mux
 * 
 * 10   12/30/10 12:16p jgarrett
 * SW7425-41: Linked audio mux output and stream mux
 * 
 * 10   12/30/10 12:10p jgarrett
 * SW7425-41: Linking audio mux output to stream mux
 * 
 * 9   12/30/10 11:35a vsilyaev
 * SW7425-39: Added control over PCR bits
 * 
 * 8   12/29/10 12:32p vsilyaev
 * SW7425-39: Updated shutdown sequence
 * 
 * 7   12/28/10 6:38p vsilyaev
 * SW7425-39: Updated to match latest TsMux
 * 
 * 6   12/28/10 5:45p vsilyaev
 * SW7425-39: Open Audio/Video PidChannel inside the stream mux module
 * 
 * 5   12/23/10 3:25p vsilyaev
 * SW7425-39: Start/stop playpump inside mux
 * 
 * 4   12/22/10 6:48p vsilyaev
 * SW7425-39: Updated MUX module
 * 
 * 3   12/16/10 7:42p vsilyaev
 * SW7425-39: Added video/transport interfaces
 * 
 * 2   12/13/10 7:02p vsilyaev
 * SW7425-40: Added system initialization for the StreamMux and
 *  VideoEncoder modules
 * 
 * 1   12/9/10 8:08p vsilyaev
 * SW7425-39: Mux module
 * 
 *
 **************************************************************************/
#include "nexus_stream_mux_module.h"
#include "priv/nexus_playpump_priv.h"
#include "nexus_video_encoder_output.h"
#include "nexus_class_verification.h"
#include "priv/nexus_pid_channel_priv.h"

BDBG_MODULE(nexus_stream_mux);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

NEXUS_StreamMux_P_State g_NEXUS_StreamMux_P_State;

void 
NEXUS_StreamMuxModule_GetDefaultSettings( NEXUS_StreamMuxModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

NEXUS_ModuleHandle 
NEXUS_StreamMuxModule_Init( const NEXUS_StreamMuxModuleSettings *pSettings)
{
    NEXUS_Error rc;

    NEXUS_ModuleSettings moduleSettings;
    BDBG_ASSERT(g_NEXUS_StreamMux_P_State.module==NULL);
    BDBG_ASSERT(pSettings->transport);
    BDBG_ASSERT(pSettings->videoEncoder);
    BDBG_ASSERT(pSettings->audio);
    BKNI_Memset(&g_NEXUS_StreamMux_P_State, 0, sizeof(g_NEXUS_StreamMux_P_State));
    g_NEXUS_StreamMux_P_State.config = *pSettings;
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eHigh;
    g_NEXUS_StreamMux_P_State.module = NEXUS_Module_Create("stream_mux", &moduleSettings);
    if(g_NEXUS_StreamMux_P_State.module == NULL) { rc = BERR_TRACE(BERR_OS_ERROR); goto error; }

    return g_NEXUS_StreamMux_P_State.module;

error:
    return NULL;
}

void 
NEXUS_StreamMuxModule_Uninit(void)
{
    if(g_NEXUS_StreamMux_P_State.module==NULL) {return;}

    NEXUS_Module_Destroy(g_NEXUS_StreamMux_P_State.module);
    g_NEXUS_StreamMux_P_State.module = NULL;
    return;
}

void 
NEXUS_StreamMux_GetDefaultCreateSettings( NEXUS_StreamMuxCreateSettings *pSettings )
{
    BMUXlib_TS_CreateSettings muxCreateSettings;
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BMUXlib_TS_GetDefaultCreateSettings(&muxCreateSettings);
    pSettings->memoryConfiguration.systemBufferSize = muxCreateSettings.stMemoryConfig.uiSystemBufferSize;
    pSettings->memoryConfiguration.sharedBufferSize = muxCreateSettings.stMemoryConfig.uiSharedBufferSize;
    return;
}


NEXUS_StreamMuxHandle 
NEXUS_StreamMux_Create( const NEXUS_StreamMuxCreateSettings *pSettings )
{
    NEXUS_StreamMuxHandle  mux;
    NEXUS_StreamMuxCreateSettings createSettings;
    NEXUS_Error rc;
    BMUXlib_TS_CreateSettings muxCreateSettings;
    NEXUS_TsMuxSettings tsMuxSettings;

    if(pSettings==NULL) {
        NEXUS_StreamMux_GetDefaultCreateSettings(&createSettings);
        pSettings = &createSettings;
    }
    mux = BKNI_Malloc(sizeof(*mux));
    if(mux==NULL) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}
    NEXUS_OBJECT_INIT(NEXUS_StreamMux, mux);
    BKNI_Memset(&mux->fnrt, 0, sizeof(NEXUS_P_StreamMux_Fnrt));
    mux->createSettings = *pSettings;
    mux->muxTimer = NULL;
    mux->started = false;

    mux->finishedCallback = NEXUS_TaskCallback_Create(mux, NULL);
    if(!mux->finishedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    NEXUS_TaskCallback_Set(mux->finishedCallback, &pSettings->finished);

    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    NEXUS_TsMux_GetDefaultSettings_priv(&tsMuxSettings);
    mux->tsMux = NEXUS_TsMux_Create_priv();
    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
    if(mux->tsMux==NULL) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}

    BMUXlib_TS_GetDefaultCreateSettings(&muxCreateSettings);
    muxCreateSettings.hMem = g_pCoreHandles->heap[0];
    muxCreateSettings.stMemoryConfig.uiSystemBufferSize = pSettings->memoryConfiguration.systemBufferSize;
    muxCreateSettings.stMemoryConfig.uiSharedBufferSize = pSettings->memoryConfiguration.sharedBufferSize; 
    rc = BMUXlib_TS_Create(&mux->mux, &muxCreateSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    return mux;

error:
    return NULL;
}

static void 
NEXUS_StreamMux_P_Finalizer(NEXUS_StreamMuxHandle mux)
{
    NEXUS_OBJECT_ASSERT(NEXUS_StreamMux, mux);
    if(mux->started) {
        NEXUS_StreamMux_Stop(mux);
    }
    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    NEXUS_TsMux_Destroy_priv(mux->tsMux);
    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
    BMUXlib_TS_Destroy(mux->mux);
    NEXUS_TaskCallback_Destroy(mux->finishedCallback);
    NEXUS_OBJECT_DESTROY(NEXUS_StreamMux, mux);
    BKNI_Free(mux);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_StreamMux, NEXUS_StreamMux_Destroy);

void 
NEXUS_StreamMux_GetDefaultStartSettings(NEXUS_StreamMuxStartSettings *pSettings)
{
    unsigned i;
    BMUXlib_TS_StartSettings *muxStartSettings = &g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetDefaultStartSettings.muxStartSettings;

    g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetDefaultStartSettings.cookie = NEXUS_StreamMux_GetDefaultStartSettings;
    
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BMUXlib_TS_GetDefaultStartSettings(muxStartSettings);

    for(i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
        pSettings->video[i].pesId = 0xE0;
        pSettings->audio[i].pesId = 0xC0;
    }
    pSettings->latencyTolerance = muxStartSettings->uiServiceLatencyTolerance;
    pSettings->nonRealTime = false;
    pSettings->nonRealTimeRate = 2 * NEXUS_NORMAL_PLAY_SPEED;
    pSettings->servicePeriod = muxStartSettings->uiServicePeriod;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    pSettings->pcr.nonPcrInsert = false;
#endif
    BDBG_ASSERT(g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetDefaultStartSettings.cookie == NEXUS_StreamMux_GetDefaultStartSettings);
    return;
}

static BERR_Code
NEXUS_StreamMux_P_AddTransportDescriptors(void *context, const BMUXlib_TS_TransportDescriptor *descriptors, size_t count, size_t *queuedCount)
{
    NEXUS_PlaypumpHandle playpump = context;
    unsigned i;
    NEXUS_Error rc;

    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(descriptors);
    BDBG_ASSERT(queuedCount);
    BDBG_MSG_TRACE(("AddTransportDescriptors: %#x count:%u", (unsigned)playpump, count));
    *queuedCount=0;
    for(;count>0;) {
        unsigned nconvert = count<=NEXUS_STREAM_MUX_P_MAX_DESCRIPTORS?count:NEXUS_STREAM_MUX_P_MAX_DESCRIPTORS;
        BPVRlib_Feed_ExtendedOffsetEntry *entries = g_NEXUS_StreamMux_P_State.entries;
        size_t nconsumed;
        for(i=0;i<nconvert;i++,entries++,descriptors++) {
            entries->baseEntry.offset = NEXUS_AddrToOffset(descriptors->pBufferAddress);
            entries->baseEntry.len = descriptors->uiBufferLength;
            entries->flags = descriptors->stTsMuxDescriptorConfig;
        }
        NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
        rc = NEXUS_Playpump_AddExtendedOffsetEntries_priv(playpump, g_NEXUS_StreamMux_P_State.entries, nconvert, &nconsumed);
        NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        BDBG_ASSERT(nconsumed<=count);
        count -= nconsumed;
        *queuedCount += nconsumed;
        if(nconsumed!=nconvert) {
            break;
        }
    }
    BDBG_MSG_TRACE(("AddTransportDescriptors: %#x queue:%u", (unsigned)playpump, *queuedCount));
    return BERR_SUCCESS;
}

static BERR_Code
NEXUS_StreamMux_P_GetCompletedTransportDescriptors( void *context, size_t *completedCount)
{
    NEXUS_PlaypumpHandle playpump = context;
    BERR_Code rc;

    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(completedCount);
    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    rc = NEXUS_Playpump_GetCompleted_priv(playpump, completedCount);
    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
    BDBG_MSG_TRACE(("GetCompletedTransportDescriptors: %#x count:%u", (unsigned)playpump, *completedCount));
    return BERR_TRACE(rc);
}

static BERR_Code
NEXUS_StreamMux_P_GetVideoBufferDescriptors( void *context, const BAVC_VideoBufferDescriptor *descriptors0[], size_t *numDescriptors0, const BAVC_VideoBufferDescriptor *descriptors1[], size_t *numDescriptors1) 
{
    BERR_Code rc;
    NEXUS_ASSERT_MODULE();
    NEXUS_ASSERT_STRUCTURE(NEXUS_VideoEncoderDescriptor, BAVC_VideoBufferDescriptor);
    rc = NEXUS_VideoEncoder_GetBuffer(context, (const NEXUS_VideoEncoderDescriptor**)descriptors0, numDescriptors0, (const NEXUS_VideoEncoderDescriptor**)descriptors1, numDescriptors1);
    BDBG_MSG_TRACE(("GetVideoBufferDescriptors:%#x numDescriptors:%u:%u", (unsigned)context, *numDescriptors0, *numDescriptors1));
    return BERR_TRACE(rc);
}

static BERR_Code
NEXUS_StreamMux_P_ConsumeVideoBufferDescriptors( void *context, size_t numDescriptors)
{
    BERR_Code rc;

    NEXUS_ASSERT_MODULE();
    rc = NEXUS_VideoEncoder_ReadComplete(context, numDescriptors);
    return BERR_TRACE(rc);
}

static BERR_Code
NEXUS_StreamMux_P_GetVideoBufferStatus( void *context, BAVC_VideoBufferStatus *status)
{
    NEXUS_VideoEncoderStatus encoderStatus;

    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(status);
    NEXUS_VideoEncoder_GetStatus(context, &encoderStatus);
    BKNI_Memset(status,0,sizeof(*status));
    status->stCommon.pFrameBufferBaseAddress = encoderStatus.bufferBase;
    status->stCommon.pMetadataBufferBaseAddress = encoderStatus.metadataBufferBase;
    return BERR_SUCCESS;
}

static BERR_Code
NEXUS_StreamMux_P_GetAudioBufferDescriptors(
   void *pvContext,
   const BAVC_AudioBufferDescriptor *astDescriptors0[], /* Can be NULL if uiNumDescriptors0=0. */
   size_t *puiNumDescriptors0,
   const BAVC_AudioBufferDescriptor *astDescriptors1[], /* Needed to handle FIFO wrap. Can be NULL if uiNumDescriptors1=0. */
   size_t *puiNumDescriptors1
   )
{
    BERR_Code rc;
    NEXUS_ASSERT_MODULE();
    NEXUS_ASSERT_STRUCTURE(NEXUS_AudioMuxOutputFrame, BAVC_AudioBufferDescriptor);
    rc = NEXUS_AudioMuxOutput_GetBuffer(pvContext, (const NEXUS_AudioMuxOutputFrame**)astDescriptors0, puiNumDescriptors0, (const NEXUS_AudioMuxOutputFrame**)astDescriptors1, puiNumDescriptors1);
    BDBG_MSG_TRACE(("GetAudioBufferDescriptors:%#x numDescriptors:%u:%u", (unsigned)pvContext, *puiNumDescriptors0, *puiNumDescriptors1));
    return BERR_TRACE(rc);
    
}

static BERR_Code
NEXUS_StreamMux_P_ConsumeAudioBufferDescriptors(
   void *pvContext,
   size_t uiNumDescriptors
   )
{
    BERR_Code rc;

    NEXUS_ASSERT_MODULE();
    rc = NEXUS_AudioMuxOutput_ReadComplete(pvContext, uiNumDescriptors);
    return BERR_TRACE(rc);
}

static BERR_Code
NEXUS_StreamMux_P_GetAudioBufferStatus(
   void *pvContext,
   BAVC_AudioBufferStatus *pBufferStatus
   )
{
    NEXUS_AudioMuxOutputStatus encoderStatus;
    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(pBufferStatus);
    NEXUS_AudioMuxOutput_GetStatus(pvContext, &encoderStatus);
    pBufferStatus->stCommon.pFrameBufferBaseAddress = encoderStatus.pBufferBase;
    pBufferStatus->stCommon.pMetadataBufferBaseAddress = encoderStatus.pMetadataBufferBase;
    return BERR_SUCCESS;
}

static BERR_Code
NEXUS_StreamMux_P_TS_GetTransportStatus( void *context, BMUXlib_TS_TransportStatus *status)
{
    NEXUS_StreamMuxHandle mux=context;
    NEXUS_TsMuxStatus tsMuxStatus;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    
    BKNI_Memset(status, 0, sizeof(*status));

    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    NEXUS_TsMux_GetStatus_priv(mux->tsMux, &tsMuxStatus);

    status->uiSTC = tsMuxStatus.uiSTC;
    status->uiESCR = tsMuxStatus.uiESCR;

    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
    return BERR_SUCCESS;
}


static BERR_Code
NEXUS_StreamMux_P_TS_GetTransportSettings(void *context, BMUXlib_TS_TransportSettings *settings)
{
    NEXUS_StreamMuxHandle mux=context;
    NEXUS_TsMuxSettings tsMuxSettings;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    BDBG_ASSERT(settings);
    BKNI_Memset(settings, 0, sizeof(*settings));

    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    NEXUS_TsMux_GetSettings_priv(mux->tsMux, &tsMuxSettings);
    settings->uiMuxDelay = tsMuxSettings.uiMuxDelay;
    settings->stNonRealTimeSettings.uiPacingCounter = tsMuxSettings.AFAPSettings.uiPacingCounter;
    settings->bNonRealTimeMode = tsMuxSettings.bAFAPMode;

    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
    return BERR_SUCCESS;
}

static BERR_Code
NEXUS_StreamMux_P_TS_SetTransportSettings( void *context, const BMUXlib_TS_TransportSettings *settings)
{
    NEXUS_StreamMuxHandle mux=context;
    NEXUS_TsMuxSettings tsMuxSettings;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    BDBG_ASSERT(settings);

    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    NEXUS_TsMux_GetSettings_priv(mux->tsMux, &tsMuxSettings);
    tsMuxSettings.uiMuxDelay = settings->uiMuxDelay;
    tsMuxSettings.AFAPSettings.uiPacingCounter = settings->stNonRealTimeSettings.uiPacingCounter;
    tsMuxSettings.bAFAPMode = settings->bNonRealTimeMode;
    tsMuxSettings.AFAPSettings.bEnablePacingPause = settings->bNonRealTimeMode;
    tsMuxSettings.AFAPSettings.uiPacingSpeed = mux->startSettings.nonRealTimeRate / NEXUS_NORMAL_PLAY_SPEED;
    rc = NEXUS_TsMux_SetSettings_priv(mux->tsMux, &tsMuxSettings, mux->startSettings.stcChannel);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto done; }

done:
    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
    return rc;
}


#define B_CAPTURE_USER_DATA 0 
#if B_CAPTURE_USER_DATA
#include <stdio.h>
#endif

static BERR_Code 
NEXUS_StreamMux_P_TS_GetUserDataBuffer(void *pvContext, const void **pBuffer0, size_t *puiLength0, const void **pBuffer1, size_t *puiLength1)
{
    BERR_Code rc;
    rc = NEXUS_Message_GetBufferWithWrap(pvContext, pBuffer0, puiLength0, pBuffer1, puiLength1);
#if B_CAPTURE_USER_DATA
    if(rc==NEXUS_SUCCESS && *puiLength0) {
        static int n=0;
        char fname[64];
        FILE *fout;
        BKNI_Snprintf(fname, sizeof(fname), "user_data_%u.ts", n);
        n++;
        fout = fopen(fname,"wb");
        if(fout) {
            fwrite(*pBuffer0, 1, *puiLength0, fout);
            if(*puiLength1) {
                fwrite(*pBuffer1, 1, *puiLength1, fout);
            }
            fclose(fout);
        }
    }
#endif /* B_CAPTURE_USER_DATA */
    return rc;
}

static BERR_Code NEXUS_StreamMux_P_TS_ConsumeUserDataBuffer(void *pvContext, size_t uiNumBytesConsumed)
{
    return NEXUS_Message_ReadComplete(pvContext, uiNumBytesConsumed);
}



static NEXUS_Error 
NEXUS_StreamMux_P_AddPid(BMUXlib_TS_StartSettings *muxStartSettings, NEXUS_PlaypumpHandle playpump, unsigned *channel, NEXUS_Timebase timebase, NEXUS_PidChannelHandle *pidChannel, uint16_t pesId, uint16_t tsPid)
{
    NEXUS_PlaypumpSettings settings;
    BMUXlib_TS_TransportChannelInterface *ch;
    BERR_Code rc;

    if(*channel>=BMUXLIB_TS_MAX_TRANSPORT_INSTANCES) { return BERR_TRACE(NEXUS_NOT_SUPPORTED); }
    if(playpump==NULL) {return BERR_TRACE(NEXUS_INVALID_PARAMETER);}

    NEXUS_Playpump_GetSettings(playpump, &settings);
    settings.transportType = pidChannel ? NEXUS_TransportType_eMpeg2Pes : NEXUS_TransportType_eTs;
    settings.originalTransportType = settings.transportType;
    settings.timestamp.timebase = timebase;
    settings.timestamp.pacing = true;

    rc = NEXUS_Playpump_SetSettings(playpump, &settings);
    if(rc) {rc=BERR_TRACE(rc);goto error;}
    if(pidChannel) {
        NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
        *pidChannel = NEXUS_Playpump_OpenPidChannel_priv(playpump, pesId, tsPid, NULL);
        NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
        if(*pidChannel==NULL) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error; }
       
        NEXUS_OBJECT_REGISTER(NEXUS_PidChannel, *pidChannel, Open);
    }

    ch = muxStartSettings->transport.stChannelInterface+(*channel);
    (*channel)++;
    ch->fAddTransportDescriptors = NEXUS_StreamMux_P_AddTransportDescriptors;
    ch->fGetCompletedTransportDescriptors = NEXUS_StreamMux_P_GetCompletedTransportDescriptors;
    ch->pContext = playpump;
    return NEXUS_SUCCESS; 

error:
    return rc;
}

static void
NEXUS_StreamMux_P_MuxTimer(void *context)
{
    NEXUS_StreamMuxHandle mux=context;
    BERR_Code rc;
    BMUXlib_DoMux_Status muxStatus;
    unsigned nextExecutionTime;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);

    mux->muxTimer = NULL;
    rc = BMUXlib_TS_DoMux(mux->mux, &muxStatus);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto error; }
    mux->completedDuration = muxStatus.uiCompletedDuration;
    nextExecutionTime = muxStatus.uiNextExecutionTime;
    BDBG_MSG(("MuxTimer:%#x nextExecutionTime:%u state:%u", (unsigned)mux, nextExecutionTime, (unsigned)muxStatus.eState));
    if(muxStatus.eState!=BMUXlib_State_eFinished && muxStatus.eState!=BMUXlib_State_eStopped) {
        mux->muxTimer = NEXUS_ScheduleTimer(nextExecutionTime, NEXUS_StreamMux_P_MuxTimer, mux);
        if(mux->muxTimer==NULL) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error; }
    } else if(muxStatus.eState==BMUXlib_State_eFinished) {
        BDBG_MSG(("MuxTimer:%#x finished", (unsigned)mux));
        NEXUS_TaskCallback_Fire(mux->finishedCallback);
    }

    return;
error:
    return;
}

static NEXUS_Error
NEXUS_StreamMux_P_ControlOnePlaypump(NEXUS_StreamMuxHandle mux, NEXUS_PlaypumpHandle playpump, bool start, bool es)
{
    NEXUS_Error rc;
    if(start) {
        NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
        rc = NEXUS_TsMux_AddPlaypump_priv(mux->tsMux, playpump);
        rc = BERR_TRACE(rc);
        if(rc==BERR_SUCCESS) {
            rc = NEXUS_Playpump_StartMuxInput_priv(playpump);
        }
        NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
        return BERR_TRACE(rc);
    } else {
        NEXUS_Playpump_Stop(playpump);
        NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
        NEXUS_TsMux_RemovePlaypump_priv(mux->tsMux, playpump);
        NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
        if(es) {
            /* NEXUS_Playpump_ClosePidChannel will call nexus_unregister */
            NEXUS_Playpump_CloseAllPidChannels(playpump);
        }
        return NEXUS_SUCCESS;
    }
}

static NEXUS_Error
NEXUS_StreamMux_P_ControlAllPlaypumps(NEXUS_StreamMuxHandle mux, const NEXUS_StreamMuxStartSettings *pSettings, bool start)
{
    unsigned i;
    NEXUS_Error rc;

    if(mux->fnrt.hFnrt) {/* only one playpump per output PID */
        for(i=0;i<mux->fnrt.numVideoPids;i++) {
            rc = NEXUS_StreamMux_P_ControlOnePlaypump(mux, mux->fnrt.videoOutputs[i].playpump,start,true);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
        for(i=0;i<mux->fnrt.numAudioPids;i++) {
            rc = NEXUS_StreamMux_P_ControlOnePlaypump(mux, mux->fnrt.audioOutputs[i].playpump,start,true);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
    } else {
        for(i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            if(pSettings->video[i].encoder==NULL) {
                break;
            }
            rc = NEXUS_StreamMux_P_ControlOnePlaypump(mux, pSettings->video[i].playpump,start,true);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
        for(i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            if(pSettings->audio[i].muxOutput==NULL) {
                break;
            }
            rc = NEXUS_StreamMux_P_ControlOnePlaypump(mux, pSettings->audio[i].playpump,start,true);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
    }
    if(pSettings->pcr.playpump!=NULL) {
        rc = NEXUS_StreamMux_P_ControlOnePlaypump(mux, pSettings->pcr.playpump,start,false);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    }
    return NEXUS_SUCCESS;
error:
    return rc;
}


NEXUS_Error 
NEXUS_StreamMux_Start( NEXUS_StreamMuxHandle mux, const NEXUS_StreamMuxStartSettings *pSettings, NEXUS_StreamMuxOutput *pMuxOutput)
{
    NEXUS_Error rc;
    unsigned channel=0;
    unsigned i, j, k;
    NEXUS_Timebase timebase = NEXUS_Timebase_eInvalid;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_TsMuxSettings tsMuxSettings;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pMuxOutput);

    BKNI_Memset(pMuxOutput, 0, sizeof(*pMuxOutput));
    if(pSettings->stcChannel==NULL) { rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error;}

    NEXUS_StcChannel_GetSettings(pSettings->stcChannel, &stcSettings);
    timebase = stcSettings.timebase;
    if(!pSettings->nonRealTime) {/* NRT mode could share the decoder STC which should be in MOD300 mode */
        stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;
    }
    rc = NEXUS_StcChannel_SetSettings(pSettings->stcChannel, &stcSettings);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}


    BMUXlib_TS_GetDefaultStartSettings(&mux->muxStartSettings);
    mux->muxStartSettings.uiServiceLatencyTolerance = pSettings->latencyTolerance;
    mux->muxStartSettings.transport.stDeviceInterface.pContext = mux;
    mux->muxStartSettings.transport.stDeviceInterface.fGetTransportSettings = NEXUS_StreamMux_P_TS_GetTransportSettings;
    mux->muxStartSettings.transport.stDeviceInterface.fSetTransportSettings = NEXUS_StreamMux_P_TS_SetTransportSettings;
    mux->muxStartSettings.transport.stDeviceInterface.fGetTransportStatus = NEXUS_StreamMux_P_TS_GetTransportStatus;
    mux->muxStartSettings.bNonRealTimeMode = pSettings->nonRealTime;
    mux->muxStartSettings.uiServicePeriod = pSettings->servicePeriod;

    channel=0;
    mux->muxStartSettings.uiNumValidVideoPIDs = 0;
    mux->muxStartSettings.uiNumValidAudioPIDs = 0;
    mux->muxStartSettings.uiNumValidUserdataPIDs = 0;

    /* 1) search if FNRTlib is needed to group inputs to shared FNRT video output interface */
    for(i=j=k=0; i < NEXUS_MAX_MUX_PIDS; i++) {
        if(pSettings->video[i].encoder==NULL) {
            continue;
        }
        /* to find matched PID to share the same fnrt output interface */
        for(k=0; k<=j; k++) {
            if(pSettings->video[i].pid == mux->fnrt.videoOutputs[k].pid) {
                mux->fnrt.videoInputs[i].output = k;/* input[i] -> output[k] */
                if(mux->fnrt.hFnrt == NULL) {/* create FNRTlib if trackID matches */
                    rc = BMUXlib_FNRT_Create(&mux->fnrt.hFnrt,NULL);
                    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
                }
                break;
            }
        }
        if(k > j) {/* no match needs to create a new fnrt interface */
            mux->fnrt.videoInputs[i].output = j;/* input[i] -> output[j] */
            mux->fnrt.videoOutputs[j].pid   = pSettings->video[i].pid;
            mux->fnrt.videoOutputs[j].pesId = pSettings->video[i].pesId;
            mux->fnrt.videoOutputs[j].playpump = pSettings->video[i].playpump;
            j++;
        }
    }
    if(j>BMUXLIB_TS_MAX_VIDEO_PIDS) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}
    mux->fnrt.numVideoPids = j;/* number of output video PIDs */

    /* 2) search if FNRTlib is needed to group inputs to shared FNRT audio output interface */
    for(i=j=k=0; i < NEXUS_MAX_MUX_PIDS; i++) {
        if(pSettings->audio[i].muxOutput==NULL) {
            continue;
        }
        /* to find matched PID to share the same fnrt output interface */
        for(k=0; k<=j; k++) {
            if(pSettings->audio[i].pid == mux->fnrt.audioOutputs[k].pid) {
                mux->fnrt.audioInputs[i].output = k;/* input[i] -> output[k] */
                if(mux->fnrt.hFnrt == NULL) {/* create FNRTlib if trackID matches */
                    rc = BMUXlib_FNRT_Create(&mux->fnrt.hFnrt,NULL);
                    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
                }
                break;
            }
        }
        if(k > j) {/* no match needs to create a new fnrt interface */
            mux->fnrt.audioInputs[i].output = j;/* input[i] -> output[j] */
            mux->fnrt.audioOutputs[j].pid   = pSettings->audio[i].pid;
            mux->fnrt.audioOutputs[j].pesId = pSettings->audio[i].pesId;
            mux->fnrt.audioOutputs[j].playpump = pSettings->audio[i].playpump;
            j++;
        }
    }
    if(j>BMUXLIB_TS_MAX_AUDIO_PIDS) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}
    mux->fnrt.numAudioPids = j;/* number of output audio PIDs */

    /* 3) map each encoder to an FNRT interface if enabled and connect FNRT to mux */
    if(mux->fnrt.hFnrt) {
        BMUXlib_FNRT_InputSettings inputSetting;
        BMUXlib_FNRT_OutputSettings outputSetting;

        /* attach video encoders to an FNRT interface */
        BMUXlib_FNRT_GetDefaultInputSettings(&inputSetting);
        for(i=0; i < NEXUS_MAX_MUX_PIDS; i++) {
            if(pSettings->video[i].encoder==NULL) {
                continue;
            }
            mux->fnrt.videoInputs[i].interface.pContext = pSettings->video[i].encoder;
            mux->fnrt.videoInputs[i].interface.fGetBufferDescriptors = NEXUS_StreamMux_P_GetVideoBufferDescriptors;
            mux->fnrt.videoInputs[i].interface.fConsumeBufferDescriptors = NEXUS_StreamMux_P_ConsumeVideoBufferDescriptors;
            mux->fnrt.videoInputs[i].interface.fGetBufferStatus = NEXUS_StreamMux_P_GetVideoBufferStatus;
            inputSetting.uiGroupId = mux->fnrt.videoInputs[i].output;/* input -> output group mapping */
            BMUXlib_FNRT_AddVideoInputInterface(mux->fnrt.hFnrt, &mux->fnrt.videoInputs[i].interface, &inputSetting);
        }
        /* get the grouped FNRT output video interface for each PID */
        BMUXlib_FNRT_GetDefaultOutputSettings(&outputSetting);
        for(i=0; i < mux->fnrt.numVideoPids; i++) {
            outputSetting.uiGroupId = i;
            BMUXlib_FNRT_GetVideoOutputInterface(mux->fnrt.hFnrt,&mux->fnrt.videoOutputs[i].interface, &outputSetting);
        }

        /* attach audio encoders to an FNRT interface */
        for(i=0; i < NEXUS_MAX_MUX_PIDS; i++) {
            if(pSettings->audio[i].muxOutput==NULL) {
                continue;
            }
            mux->fnrt.audioInputs[i].interface.pContext = pSettings->audio[i].muxOutput;
            mux->fnrt.audioInputs[i].interface.fGetBufferDescriptors = NEXUS_StreamMux_P_GetAudioBufferDescriptors;
            mux->fnrt.audioInputs[i].interface.fConsumeBufferDescriptors = NEXUS_StreamMux_P_ConsumeAudioBufferDescriptors;
            mux->fnrt.audioInputs[i].interface.fGetBufferStatus = NEXUS_StreamMux_P_GetAudioBufferStatus;
            inputSetting.uiGroupId = mux->fnrt.audioInputs[i].output;/* input -> output group mapping */
            BMUXlib_FNRT_AddAudioInputInterface(mux->fnrt.hFnrt, &mux->fnrt.audioInputs[i].interface, &inputSetting);
        }
        /* get the grouped FNRT output audio interface for each PID */
        for(i=0; i < mux->fnrt.numAudioPids; i++) {
            outputSetting.uiGroupId = i;
            BMUXlib_FNRT_GetAudioOutputInterface(mux->fnrt.hFnrt,&mux->fnrt.audioOutputs[i].interface, &outputSetting);
        }
    }

    /* 4) update muxlib start settings */
    if(mux->fnrt.hFnrt) {
        for(i=0; i < mux->fnrt.numVideoPids; i++) {
            mux->muxStartSettings.video[i].uiTransportChannelIndex = channel;
            BDBG_MSG(("NEXUS_StreamMux_Start:%#x using channel %u for video[%u]=%#x", (unsigned)mux, channel, i, mux->fnrt.videoOutputs[i].pid));
            rc = NEXUS_StreamMux_P_AddPid(&mux->muxStartSettings, mux->fnrt.videoOutputs[i].playpump, &channel, timebase, &pMuxOutput->video[i], mux->fnrt.videoOutputs[i].pesId, mux->fnrt.videoOutputs[i].pid);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
            mux->muxStartSettings.video[i].uiPID            = mux->fnrt.videoOutputs[i].pid;
            mux->muxStartSettings.video[i].uiPESStreamID    = mux->fnrt.videoOutputs[i].pesId;
            mux->muxStartSettings.video[i].stInputInterface = mux->fnrt.videoOutputs[i].interface;
        }
        mux->muxStartSettings.uiNumValidVideoPIDs = mux->fnrt.numVideoPids;

        for(i=0; i < mux->fnrt.numAudioPids; i++) {
            mux->muxStartSettings.audio[i].uiTransportChannelIndex = channel;
            BDBG_MSG(("NEXUS_StreamMux_Start:%#x using channel %u for audio[%u]=%#x", (unsigned)mux, channel, i, mux->fnrt.audioOutputs[i].pid));
            rc = NEXUS_StreamMux_P_AddPid(&mux->muxStartSettings, mux->fnrt.audioOutputs[i].playpump, &channel, timebase, &pMuxOutput->audio[i], mux->fnrt.audioOutputs[i].pesId, mux->fnrt.audioOutputs[i].pid);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
            mux->muxStartSettings.audio[i].uiPID            = mux->fnrt.audioOutputs[i].pid;
            mux->muxStartSettings.audio[i].uiPESStreamID    = mux->fnrt.audioOutputs[i].pesId;
            mux->muxStartSettings.audio[i].stInputInterface = mux->fnrt.audioOutputs[i].interface;
        }
        mux->muxStartSettings.uiNumValidAudioPIDs = mux->fnrt.numAudioPids;
    } else {/* non-FNRT: one output PID per input interface */
        for(i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            const NEXUS_StreamMuxVideoPid *video = pSettings->video+i;
            if(video->encoder==NULL) {
                break;
            }
            if(i>=BMUXLIB_TS_MAX_VIDEO_PIDS) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}
            mux->muxStartSettings.video[i].uiTransportChannelIndex = channel;
            BDBG_MSG(("NEXUS_StreamMux_Start:%#x using channel %u for video[%u]=%#x", (unsigned)mux, channel, i, video->pid));
            rc = NEXUS_StreamMux_P_AddPid(&mux->muxStartSettings, video->playpump, &channel, timebase, &pMuxOutput->video[i], video->pesId, video->pid);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
            mux->muxStartSettings.video[i].uiPID = video->pid;
            mux->muxStartSettings.video[i].uiPESStreamID = video->pesId;
            mux->muxStartSettings.video[i].stInputInterface.pContext = video->encoder;
            mux->muxStartSettings.video[i].stInputInterface.fGetBufferDescriptors = NEXUS_StreamMux_P_GetVideoBufferDescriptors;
            mux->muxStartSettings.video[i].stInputInterface.fConsumeBufferDescriptors = NEXUS_StreamMux_P_ConsumeVideoBufferDescriptors;
            mux->muxStartSettings.video[i].stInputInterface.fGetBufferStatus = NEXUS_StreamMux_P_GetVideoBufferStatus;
            mux->muxStartSettings.uiNumValidVideoPIDs = i+1;
        }
        for(i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            const NEXUS_StreamMuxAudioPid *audio= pSettings->audio+i;
            if(audio->muxOutput==NULL) {
                break;
            }
            if(i>=BMUXLIB_TS_MAX_AUDIO_PIDS) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}
            mux->muxStartSettings.audio[i].uiTransportChannelIndex = channel;
            BDBG_MSG(("NEXUS_StreamMux_Start:%#x using channel %u for audio[%u]=%#x", (unsigned)mux, channel, i, audio->pid));
            rc = NEXUS_StreamMux_P_AddPid(&mux->muxStartSettings, audio->playpump, &channel, timebase, &pMuxOutput->audio[i], audio->pesId, audio->pid);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
            mux->muxStartSettings.audio[i].uiPID = audio->pid;
            mux->muxStartSettings.audio[i].uiPESStreamID = audio->pesId;
            mux->muxStartSettings.audio[i].stInputInterface.pContext = audio->muxOutput;
            mux->muxStartSettings.audio[i].stInputInterface.fGetBufferDescriptors = NEXUS_StreamMux_P_GetAudioBufferDescriptors;
            mux->muxStartSettings.audio[i].stInputInterface.fConsumeBufferDescriptors = NEXUS_StreamMux_P_ConsumeAudioBufferDescriptors;
            mux->muxStartSettings.audio[i].stInputInterface.fGetBufferStatus = NEXUS_StreamMux_P_GetAudioBufferStatus;
            mux->muxStartSettings.uiNumValidAudioPIDs=i+1;
        }
    }

    if(pSettings->pcr.playpump!=NULL) {
        mux->muxStartSettings.stPCRData.uiTransportChannelIndex = channel;
        BDBG_MSG(("NEXUS_StreamMux_Start:%#x using channel %u for pcr %#x", (unsigned)mux, channel,  pSettings->pcr.pid));
        rc = NEXUS_StreamMux_P_AddPid(&mux->muxStartSettings, pSettings->pcr.playpump, &channel, timebase, NULL, 0, 0);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        mux->muxStartSettings.stPCRData.uiPID = pSettings->pcr.pid;
        mux->muxStartSettings.stPCRData.uiInterval = pSettings->pcr.interval;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		mux->muxStartSettings.stPCRData.bNonPcrInsert = pSettings->pcr.nonPcrInsert;
#endif
    }
    for(i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
        if(pSettings->userdata[i].message==NULL) {
            break;
        }
        if(i>=BMUXLIB_TS_MAX_USERDATA_PIDS) {
            BDBG_ERR(("only %u userdata pids is supported", BMUXLIB_TS_MAX_USERDATA_PIDS));
            rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error;
        }
        BDBG_MSG(("NEXUS_StreamMux_Start:%#x using message %p for userdata %u", (unsigned)mux, pSettings->userdata[i].message, i));
        mux->muxStartSettings.userdata[i].stUserDataInterface.pContext = pSettings->userdata[i].message;
        mux->muxStartSettings.userdata[i].stUserDataInterface.fGetUserDataBuffer = NEXUS_StreamMux_P_TS_GetUserDataBuffer;
        mux->muxStartSettings.userdata[i].stUserDataInterface.fConsumeUserDataBuffer = NEXUS_StreamMux_P_TS_ConsumeUserDataBuffer;
        mux->muxStartSettings.uiNumValidUserdataPIDs = i+1;
    }

    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    BXPT_TsMux_GetDefaultSettings(&tsMuxSettings);
    rc = NEXUS_TsMux_SetSettings_priv(mux->tsMux, &tsMuxSettings, pSettings->stcChannel);
    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto error; }

    mux->startSettings = *pSettings;

    rc = NEXUS_StreamMux_P_ControlAllPlaypumps(mux, &mux->startSettings, true);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}


    rc = BMUXlib_TS_Start(mux->mux, &mux->muxStartSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    mux->muxTimer = NEXUS_ScheduleTimer(0, NEXUS_StreamMux_P_MuxTimer, mux);
    if(mux->muxTimer==NULL) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error; }
    mux->started = true;
    mux->completedDuration = 0;

    return NEXUS_SUCCESS;
error:
    return rc;
}

void 
NEXUS_StreamMux_Finish(NEXUS_StreamMuxHandle mux)
{
    BERR_Code rc;
    BMUXlib_TS_FinishSettings muxFinishSettings;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    BMUXlib_TS_GetDefaultFinishSettings(&muxFinishSettings);
    rc = BMUXlib_TS_Finish(mux->mux, &muxFinishSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    return;

error:
    return;
}

void 
NEXUS_StreamMux_Stop(NEXUS_StreamMuxHandle mux)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    if(!mux->started) {
        return;
    }
    rc = BMUXlib_TS_Stop(mux->mux);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    if(mux->muxTimer) {
        NEXUS_CancelTimer(mux->muxTimer);
        mux->muxTimer = NULL;
    }
    rc = NEXUS_StreamMux_P_ControlAllPlaypumps(mux, &mux->startSettings, false);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    NEXUS_Module_Lock(g_NEXUS_StreamMux_P_State.config.transport);
    NEXUS_Module_Unlock(g_NEXUS_StreamMux_P_State.config.transport);

    mux->started = false;

    /* detach FNRT */
    if(mux->fnrt.hFnrt) {
        BMUXlib_FNRT_Destroy(mux->fnrt.hFnrt);
    }
    BKNI_Memset(&mux->fnrt, 0, sizeof(NEXUS_P_StreamMux_Fnrt));
    return;

error:
    return;
}

void 
NEXUS_StreamMux_GetDefaultSystemData(NEXUS_StreamMuxSystemData *pSystemDataBuffer)
{
    BDBG_ASSERT(pSystemDataBuffer);
    BKNI_Memset(pSystemDataBuffer, 0, sizeof(*pSystemDataBuffer));
    return;
}

NEXUS_Error 
NEXUS_StreamMux_AddSystemDataBuffer(NEXUS_StreamMuxHandle mux, const NEXUS_StreamMuxSystemData *pSystemDataBuffer)
{
    BERR_Code rc;
    size_t queuedCount;

    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    BDBG_ASSERT(pSystemDataBuffer);

    NEXUS_ASSERT_STRUCTURE(NEXUS_StreamMuxSystemData, BMUXlib_TS_SystemData);
    NEXUS_ASSERT_FIELD(NEXUS_StreamMuxSystemData, timestampDelta, BMUXlib_TS_SystemData, uiTimestampDelta);
    NEXUS_ASSERT_FIELD(NEXUS_StreamMuxSystemData, size, BMUXlib_TS_SystemData, uiSize);
    NEXUS_ASSERT_FIELD(NEXUS_StreamMuxSystemData, pData, BMUXlib_TS_SystemData, pData);

    queuedCount = 0;
    rc = BMUXlib_TS_AddSystemDataBuffers(mux->mux, (void *)pSystemDataBuffer, 1, &queuedCount);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    if(queuedCount==0) {rc=BERR_TRACE(NEXUS_NOT_AVAILABLE); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

void 
NEXUS_StreamMux_GetCompletedSystemDataBuffers(NEXUS_StreamMuxHandle mux, size_t *pCompletedCount)
{
    BERR_Code rc;


    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    BDBG_ASSERT(pCompletedCount);

    rc = BMUXlib_TS_GetCompletedSystemDataBuffers(mux->mux, pCompletedCount);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    return;

error:
    return;
}

NEXUS_Error 
NEXUS_StreamMux_GetStatus( NEXUS_StreamMuxHandle mux, NEXUS_StreamMuxStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(mux, NEXUS_StreamMux);
    BDBG_ASSERT(pStatus);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->duration = mux->completedDuration;

    return NEXUS_SUCCESS;
}

void NEXUS_StreamMux_GetDefaultConfiguration(NEXUS_StreamMuxConfiguration *pConfiguration)
{
    BMUXlib_TS_StartSettings *startSettings = &g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetDefaultConfiguration.startSettings;
    BMUXlib_TS_MuxSettings *muxSettings = &g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetDefaultConfiguration.muxSettings;

    g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetDefaultConfiguration.cookie = NEXUS_StreamMux_GetDefaultConfiguration;

    BMUXlib_TS_GetDefaultStartSettings(startSettings);
    BMUXlib_TS_GetDefaultMuxSettings(muxSettings);
    BKNI_Memset(pConfiguration, 0, sizeof(*pConfiguration));
    pConfiguration->servicePeriod = startSettings->uiServicePeriod;
    pConfiguration->latencyTolerance = startSettings->uiServiceLatencyTolerance;
    pConfiguration->nonRealTime = startSettings->bNonRealTimeMode;
    pConfiguration->videoPids = 1;
    pConfiguration->audioPids = 1;
    pConfiguration->userDataPids = 0;
    pConfiguration->pcrPidinterval = startSettings->stPCRData.uiInterval;
    pConfiguration->systemDataBitRate = muxSettings->uiSystemDataBitRate;

    BDBG_ASSERT(g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetDefaultConfiguration.cookie == NEXUS_StreamMux_GetDefaultConfiguration);
    return;
}

void NEXUS_StreamMux_GetMemoryConfiguration(const NEXUS_StreamMuxConfiguration *pConfiguration, NEXUS_StreamMuxMemoryConfiguration *pMemoryConfiguration)
{
    BMUXlib_TS_MuxConfig *muxConfig = &g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetMemoryConfiguration.muxConfig;
    BMUXlib_TS_MemoryConfig *memoryConfig = &g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetMemoryConfiguration.memoryConfig;

    g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetMemoryConfiguration.cookie = NEXUS_StreamMux_GetMemoryConfiguration;
    BDBG_ASSERT(pConfiguration);
    BDBG_ASSERT(pMemoryConfiguration);
    BMUXlib_TS_GetDefaultStartSettings(&muxConfig->stMuxStartSettings);
    BMUXlib_TS_GetDefaultMuxSettings(&muxConfig->stMuxSettings);

    muxConfig->stMuxStartSettings.uiServicePeriod = pConfiguration->servicePeriod;
    muxConfig->stMuxStartSettings.uiServiceLatencyTolerance = pConfiguration->latencyTolerance;
    muxConfig->stMuxStartSettings.bNonRealTimeMode = pConfiguration->nonRealTime;
    muxConfig->stMuxStartSettings.uiNumValidVideoPIDs = pConfiguration->videoPids;
    muxConfig->stMuxStartSettings.uiNumValidAudioPIDs = pConfiguration->audioPids;
    muxConfig->stMuxStartSettings.uiNumValidUserdataPIDs = pConfiguration->userDataPids;
    muxConfig->stMuxStartSettings.stPCRData.uiInterval = pConfiguration->pcrPidinterval;
    muxConfig->stMuxSettings.uiSystemDataBitRate = pConfiguration->systemDataBitRate;
    BMUXlib_TS_GetMemoryConfig(muxConfig, memoryConfig);
    BKNI_Memset(pMemoryConfiguration, 0, sizeof(*pMemoryConfiguration));
    pMemoryConfiguration->systemBufferSize = memoryConfig->uiSystemBufferSize;
    pMemoryConfiguration->sharedBufferSize = memoryConfig->uiSharedBufferSize;
    BDBG_ASSERT(g_NEXUS_StreamMux_P_State.functionData.NEXUS_StreamMux_GetMemoryConfiguration.cookie == NEXUS_StreamMux_GetMemoryConfiguration);
    return;
}

