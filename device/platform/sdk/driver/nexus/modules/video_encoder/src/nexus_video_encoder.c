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
 * $brcm_Workfile: nexus_video_encoder.c $
 * $brcm_Revision: 58 $
 * $brcm_Date: 10/2/12 4:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_encoder/src/nexus_video_encoder.c $
 * 
 * 58   10/2/12 4:51p mward
 * SW7435-165:  Fail gracefully when opening an encoder that is
 *  unavailable due to RTS use case setting.
 * 
 * 57   8/15/12 2:33p vsilyaev
 * SW7435-311: Disable modifying state of already opened encoder channel
 * 
 * 56   8/13/12 1:21p vsilyaev
 * SW7425-3707: Added compile time test to number of supported stop modes
 * 
 * 55   7/18/12 12:32p vsilyaev
 * SW7425-3156: Fixed typo
 * 
 * 54   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 53   6/8/12 12:17p vsilyaev
 * SW7425-3156: Added ClearStatus API
 * 
 * 52   6/5/12 12:26p vsilyaev
 * SW7425-3167: Merged to mainline
 * 
 * SW7425-3167/1   6/5/12 11:47a nilesh
 * SW7425-3167: Removed trackInput and restartGopOnSceneChange from
 *  NEXUS_VideoEncoderStreamStructure
 * 
 * 51   5/18/12 12:05p mward
 * SW7435-165: Run-time selection of transcode use cases
 * 
 * 50   4/17/12 3:34p vsilyaev
 * SW7425-2875: Use originalPts instead of originalPTS
 * 
 * 49   4/16/12 11:25a vsilyaev
 * SW7425-2875: Updated layout of the encoder descriptor
 * 
 * 48   4/12/12 12:32p hongtaoz
 * SW7435-100: use debug message level;
 * 
 * 47   4/11/12 2:51p hongtaoz
 * SW7435-100: added single/dual xcode usage support;
 * 
 * 46   4/11/12 11:22a hongtaoz
 * SW7425-2864: enable STG display user data callback after VCE channel
 *  started;
 * 
 * 45   3/27/12 1:43p erickson
 * SW7425-2607: use NEXUS_MAX_VCE_DEVICES
 * 
 * 44   3/5/12 6:15p hongtaoz
 * SW7435-7: program VCE instance ID for the devices;
 * 
 * 43   2/9/12 11:10a vsilyaev
 * SW7425-2349: Added adaptiveLowDelayMode configuration
 * 
 * 42   1/26/12 12:14p vsilyaev
 * SW7425-2266: Added [optional] parameter to the NEXUS_VideoEncoder_Stop
 *  function
 * 
 * 41   1/25/12 2:00p vsilyaev
 * SW7425-2220,SW7425-997: Change BVCE_Channel_StopEncode() prototype to
 *  support different stop modes
 * 
 * SW7425-2220/1   1/25/12 4:50p nilesh
 * SW7425-2220,SW7425-997: Change BVCE_Channel_StopEncode() prototype to
 *  support different stop modes
 * 
 * 40   12/21/11 11:54a vsilyaev
 * SW7425-1882: Use BVCE_Debug_ReadBuffer  instead of
 *  BVCE_Debug_ReadBuffer_isr
 * 
 * 39   12/16/11 5:29p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/4   12/16/11 2:14p ajagmag
 * SW7425-1878: Update based on discussions with David E.
 * 
 * SW7425-1878/3   12/15/11 7:17a ajagmag
 * SW7425-1878: Updated as per XUDlib review changes
 * 
 * SW7425-1878/2   12/13/11 4:11p ajagmag
 * SW7425-1878: Removed stgRate from XUDlib settings.
 * 
 * SW7425-1878/1   12/9/11 1:34p ajagmag
 * SW7425-1878: Integrate XUDlib into nexus
 * 
 * 38   12/12/11 6:48p vsilyaev
 * SW7425-1848: Added "video_encoder_verification"
 * 
 * 37   12/9/11 3:53p vsilyaev
 * SW7425-1566: Added NEXUS_VideoEncoderType
 * 
 * 36   12/8/11 3:16p vsilyaev
 * SW7425-1746: Added NEXUS_VideoEncoder_InsertRandomAccessPoint
 * 
 * 35   11/9/11 3:32p hongtaoz
 * SW7425-825: added all ARCs debug log; make it compile option the same
 *  as VCE PI;
 * 
 * 34   10/24/11 2:02p vsilyaev
 * SW7425-1580: GetStatus - removed I picture count, added dropped counts
 * 
 * SW7425-1580/1   10/24/11 4:48p nilesh
 * SW7425-1580: GetStatus - removed I picture count, added dropped counts
 * 
 * 33   10/4/11 3:33p hongtaoz
 * SW7425-1393: fixed coverity defect (OVERRUN_STATIC);
 * 
 * 32   9/27/11 3:50p hongtaoz
 * SW7425-1321: made encoder channel/output instance ID assignment
 *  scalable for multiple cores;
 * 
 * 31   9/19/11 4:57p hongtaoz
 * SW7425-1321: added instance number to channel open and output open;
 * 
 * 30   9/13/11 1:13p hongtaoz
 * SW7425-1278: fixed log read buffer assertion bound check;
 * 
 * 29   9/7/11 4:04p gmohile
 * SW7425-533 : Add VCE standby support
 * 
 * 28   9/2/11 3:48p vsilyaev
 * SW7425-1225: Added video encoder descriptor flags
 * 
 * 27   8/19/11 2:40p vsilyaev
 * SW7425-651: Fixed typo
 * 
 * 26   8/16/11 6:25p vsilyaev
 * SW7425-651: Added video encoder bounds to facilitate lower encoding
 *  delay
 * 
 * 25   4/24/11 10:53p hongtaoz
 * SW7425-431: cancel the encoder log timer when close to avoid the nexus
 *  segfault during quit of close/open test;
 * 
 * 24   4/24/11 7:34p hongtaoz
 * SW7425-75: use module message level for frame base address debug;
 * 
 * 23   4/20/11 7:31p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 22   4/18/11 1:04p vsilyaev
 * SW7425-171: Added NEXUS_VideoEncoderSettingsOnInputChange
 * 
 * 21   4/14/11 11:14a vsilyaev
 * SW7425-75: Propagate pMetadataBufferBaseAddress through from nexus
 *  video encoder to the muxes
 * 
 * 20   4/8/11 1:03p vsilyaev
 * SW7425-232: Updated to the latest mux code
 * 
 * 19   3/4/11 1:13a hongtaoz
 * SW7425-40: allow user to set cdb/itb buffer size;
 * 
 * 18   3/3/11 11:19a vsilyaev
 * SW7425-141, SW7425-137: Updated mapping to VCE PI
 * 
 * SW7425-137/2   3/2/11 11:48p nilesh
 * SW7425-141: Changed "EndToEndDelay" to "A2PDelay"
 * 
 * SW7425-137/1   3/2/11 11:43p nilesh
 * SW7425-137: Removed uiMin and uiTarget bitrate settings
 * 
 * 17   2/22/11 4:41p vsilyaev
 * SW7425-102: Added API to control the video encoder delay
 * 
 * 16   2/16/11 6:44p hongtaoz
 * SW7425-40: Use non-secure heap[0] on memc0 for ITB heap;
 * 
 * 15   2/16/11 2:34p vsilyaev
 * SW7425-40: Added system heap
 * 
 * 14   2/11/11 8:39p vsilyaev
 * SW7425-81: Added image interface plumbing to the video encoder module
 * 
 * 13   2/10/11 1:07p vsilyaev
 * SW7425-40: Improved shutdown sequence
 * 
 * 12   2/8/11 10:22p hongtaoz
 * SW7425-40: fixed a typo to set eProfile;
 * 
 * 11   2/8/11 5:41p vsilyaev
 * SW7425-40: Updated profile/level configuration
 * 
 * 10   2/8/11 5:03p vsilyaev
 * SW7425-40: Updated to match latest BVCE
 * 
 * 9   2/7/11 12:15p vsilyaev
 * SW7425-40: Updated to match latest BVCE
 * 
 * 8   2/7/11 9:41a vsilyaev
 * SW7425-40: Updated to match latest BVCE
 * 
 * 7   1/17/11 4:46p vsilyaev
 * SW7425-65: Added StcChannel to the videoEncoder
 * 
 * 6   12/30/10 11:47a vsilyaev
 * SW7425-40: Added mapping for frame rate
 * 
 * 5   12/28/10 6:37p vsilyaev
 * SW7425-40: Set defaul profile and level
 * 
 * 4   12/26/10 4:16p vsilyaev
 * SW7425-40: Fixed shutdown sequence
 * 
 * 3   12/16/10 4:06p vsilyaev
 * SW7425-40: Propagate more configuration to the video encoder
 * 
 * 2   12/13/10 7:02p vsilyaev
 * SW7425-40: Added system initialization for the StreamMux and
 *  VideoEncoder modules
 * 
 * 1   12/9/10 8:11p vsilyaev
 * SW7425-40: Video Encoder module
 * 
 *
 **************************************************************************/
#include "nexus_video_encoder_module.h"
#include "nexus_power_management.h"

#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_display_priv.h"
#include "priv/nexus_core_img_id.h"
#include "priv/nexus_core.h"
#include "bxudlib.h"

BDBG_MODULE(nexus_video_encoder);
BDBG_FILE_MODULE(nexus_video_encoder_status);

NEXUS_VideoEncoder_P_State g_NEXUS_VideoEncoder_P_State;

void 
NEXUS_VideoEncoderModule_GetDefaultSettings( NEXUS_VideoEncoderModuleSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    /* we can't set meaningless default heap assignment since it's chip/platform speciific */

    return;
}

NEXUS_ModuleHandle 
NEXUS_VideoEncoderModule_Init(const NEXUS_VideoEncoderModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    unsigned i;
    BERR_Code rc;

    BDBG_ASSERT(pSettings);

    BDBG_CASSERT(NEXUS_NUM_VCE_DEVICES <= NEXUS_MAX_VCE_DEVICES);
    BDBG_ASSERT(g_NEXUS_VideoEncoder_P_State.module==NULL);
    BDBG_ASSERT(pSettings->display);
    BDBG_ASSERT(pSettings->transport);
    BKNI_Memset(&g_NEXUS_VideoEncoder_P_State, 0, sizeof(g_NEXUS_VideoEncoder_P_State));
    g_NEXUS_VideoEncoder_P_State.config = *pSettings;
    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow; /* encoder interface is slow */
    g_NEXUS_VideoEncoder_P_State.module = NEXUS_Module_Create("video_encoder", &moduleSettings);
    if(g_NEXUS_VideoEncoder_P_State.module == NULL) { rc = BERR_TRACE(BERR_OS_ERROR); goto error; }

    /* Open ViCE devices */
    for(i=0;i<NEXUS_NUM_VCE_DEVICES;i++ ) {
        BVCE_OpenSettings  vceSettings;
        NEXUS_VideoEncoder_P_Device *device = g_NEXUS_VideoEncoder_P_State.vceDevices+i;
        g_NEXUS_VideoEncoder_P_State.vceDevices[i].vce = NULL;

        BKNI_Memset(device->channels, 0, sizeof(device->channels));

        if (-1==pSettings->vceMapping[i*NEXUS_NUM_VCE_CHANNELS].device) {
            continue; /* not mapped / not used */
        }
        device->mem = g_pCoreHandles->heap[pSettings->heapIndex[i].system];
        BVCE_GetDefaultOpenSettings(&vceSettings);
        if(pSettings->heapSize[i].general) {
            vceSettings.stMemoryConfig.uiGeneralMemSize = pSettings->heapSize[i].general;
        }
        if(pSettings->heapSize[i].picture) {
            vceSettings.stMemoryConfig.uiPictureMemSize = pSettings->heapSize[i].picture;
        }
        if(pSettings->heapSize[i].secure) {
            vceSettings.stMemoryConfig.uiSecureMemSize = pSettings->heapSize[i].secure;
        }

        vceSettings.hFirmwareMem[0] = g_pCoreHandles->heap[pSettings->heapIndex[i].firmware[0]];
        vceSettings.hFirmwareMem[1] = g_pCoreHandles->heap[pSettings->heapIndex[i].firmware[1]];
        vceSettings.hSecureMem = g_pCoreHandles->heap[pSettings->heapIndex[i].secure];
        vceSettings.hPictureMem = g_pCoreHandles->heap[pSettings->heapIndex[i].picture];
        vceSettings.hGeneralMem = g_pCoreHandles->heap[pSettings->heapIndex[i].general];
        if (Nexus_Core_P_Img_Create(NEXUS_CORE_IMG_ID_VCE,&device->img_context,&device->img_interface )== NEXUS_SUCCESS)
        {
            BDBG_WRN(("FW download used"));
            vceSettings.pImgInterface = &device->img_interface;
            vceSettings.pImgContext   = device->img_context;
        }
        vceSettings.bVerificationMode = NEXUS_GetEnv("video_encoder_verification")!=NULL;
        vceSettings.uiInstance = pSettings->vceMapping[i*NEXUS_NUM_VCE_CHANNELS].device;
        BDBG_MSG(("VCE[%u] fw heap[%u] secure heap[%u] pic heap[%u]", vceSettings.uiInstance, 
            pSettings->heapIndex[i].firmware[0], pSettings->heapIndex[i].secure, pSettings->heapIndex[i].picture));
        rc = BVCE_Open( &device->vce, g_pCoreHandles->chp, g_pCoreHandles->reg, device->mem, g_pCoreHandles->bint, &vceSettings);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    }
    return g_NEXUS_VideoEncoder_P_State.module;
error:
    return NULL;
}

void 
NEXUS_VideoEncoderModule_Uninit(void)
{
    unsigned i,j;
    if(g_NEXUS_VideoEncoder_P_State.module==NULL) {return;}
    for(i=0;i<NEXUS_NUM_VCE_DEVICES;i++ ) {
        NEXUS_VideoEncoder_P_Device *device = g_NEXUS_VideoEncoder_P_State.vceDevices+i;
        if (!device->vce) {
            continue;
        }
        for(j=0;j<NEXUS_NUM_VCE_CHANNELS;j++ ) {
            if(device->channels[j].enc) {
                NEXUS_VideoEncoder_Close(&device->channels[j]);
            }
        }
        BVCE_Close(device->vce);
        if (device->img_context) {
            Nexus_Core_P_Img_Destroy(device->img_context);
        }
    }
    NEXUS_Module_Destroy(g_NEXUS_VideoEncoder_P_State.module);
    g_NEXUS_VideoEncoder_P_State.module = NULL;
    return;
}

NEXUS_Error NEXUS_VideoEncoderModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{    
    NEXUS_Error rc=NEXUS_SUCCESS;

#if NEXUS_POWER_MANAGEMENT
    unsigned i;    

    BSTD_UNUSED(pSettings);
    
    for(i=0; i<NEXUS_NUM_VCE_DEVICES; i++) {    
    NEXUS_VideoEncoder_P_Device *device = g_NEXUS_VideoEncoder_P_State.vceDevices+i;    
    if(device->vce) {
        if(enabled) {
            rc = BVCE_Power_Standby(device->vce);
        } else {
            rc = BVCE_Power_Resume(device->vce);
        }
    }    
    }
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
#endif
    
    return rc;
}


void NEXUS_VideoEncoder_GetDefaultOpenSettings(NEXUS_VideoEncoderOpenSettings *pSettings)
{
    BVCE_Output_AllocBuffersSettings outputBuffers;
    BVCE_Channel_OpenSettings encSettings;
    
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BVCE_Output_GetDefaultAllocBuffersSettings(&outputBuffers);
    BVCE_Channel_GetDefaultOpenSettings(&encSettings);
    pSettings->data.fifoSize = outputBuffers.stConfig.Cdb.Length;
    pSettings->index.fifoSize = outputBuffers.stConfig.Itb.Length;
    pSettings->type = encSettings.eMultiChannelMode;
    BDBG_CASSERT((int)BVCE_MultiChannelMode_eMulti == (int)NEXUS_VideoEncoderType_eMulti);
    BDBG_CASSERT((int)BVCE_MultiChannelMode_eSingle == (int)NEXUS_VideoEncoderType_eSingle);
    BDBG_CASSERT((int)BVCE_MultiChannelMode_eMultiNRTOnly == (int)NEXUS_VideoEncoderType_eMultiNonRealTime);
    BDBG_CWARNING((int)BVCE_MultiChannelMode_eMax == (int)NEXUS_VideoEncoderType_eMax);
    return ;
}

#if BVCE_P_DUMP_ARC_DEBUG_LOG
static void
NEXUS_VideoEncoder_P_LogTimer(void *context)
{
    unsigned i, j;
    unsigned logged;

    BSTD_UNUSED(context);
    for(logged=0,i=0;i<NEXUS_NUM_VCE_DEVICES;i++) {
        NEXUS_VideoEncoder_P_Device *device = &g_NEXUS_VideoEncoder_P_State.vceDevices[i];
        uint32_t bytesRead = 0;
        char log[128];

        for(j=0; j<BVCE_ArcInstance_eMax; j++) {
            BVCE_Debug_ReadBuffer( device->vce, j,log,sizeof(log)-1,&bytesRead);
            if(bytesRead>0) {
                BDBG_ASSERT(bytesRead<sizeof(log));
                logged += bytesRead;
                log[bytesRead]='\0';
                BDBG_MSG(("ARC%d:%s", j, log));
            }
        }
    }
    g_NEXUS_VideoEncoder_P_State.logTimer = NEXUS_ScheduleTimer(logged>0?10:100, NEXUS_VideoEncoder_P_LogTimer, NULL);
    return;
}
#endif

NEXUS_VideoEncoderHandle 
NEXUS_VideoEncoder_Open(unsigned index, const NEXUS_VideoEncoderOpenSettings *pSettings)
{
    BERR_Code rc;
    NEXUS_VideoEncoderOpenSettings settings;
    BVCE_Channel_OpenSettings encSettings;
    BVCE_Output_OpenSettings outputSettings;
    NEXUS_VideoEncoder_P_Device *device = &g_NEXUS_VideoEncoder_P_State.vceDevices[0];
    NEXUS_VideoEncoderHandle encoder=NULL;
    BVCE_Output_AllocBuffersSettings outputBuffers;
    BVCE_Channel_EncodeSettings encodeSettings;

    if(index>=NEXUS_NUM_VCE_CHANNELS*NEXUS_NUM_VCE_DEVICES) {rc=BERR_TRACE(NEXUS_INVALID_PARAMETER);goto error;}

    /* video encoder index/device/channel mapping */
    device = &g_NEXUS_VideoEncoder_P_State.vceDevices[index/NEXUS_NUM_VCE_CHANNELS];
    encoder = &device->channels[index%NEXUS_NUM_VCE_CHANNELS];
	if (!device->vce) goto error; /* Encoder may be unavailable due to RTS use case setting */
    if(encoder->enc!=NULL) {rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);encoder=NULL;goto error; }

    if(pSettings==NULL) {
        NEXUS_VideoEncoder_GetDefaultOpenSettings(&settings);
        pSettings=&settings;
    }
    NEXUS_OBJECT_INIT(NEXUS_VideoEncoder, encoder);
    encoder->openSettings = *pSettings;
    encoder->device = device;
    BVCE_Channel_GetDefaultOpenSettings(&encSettings);

    /* the channel instance ID is the context ID in a core or device */
    encSettings.uiInstance = index%NEXUS_NUM_VCE_CHANNELS;
    encSettings.eMultiChannelMode = pSettings->type;
    rc = BVCE_Channel_Open(device->vce, &encoder->enc, &encSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    BVCE_Output_GetDefaultOpenSettings(&outputSettings);

    /* the output instance ID is the context ID in a core or device */
    outputSettings.uiInstance = index%NEXUS_NUM_VCE_CHANNELS;
    rc = BVCE_Output_Open(device->vce, &encoder->output, &outputSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    BVCE_Output_GetDefaultAllocBuffersSettings(&outputBuffers);

#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE)
    /* Set the CABAC CDB to little endian mode so we can dump the ES using the MIPs */
    outputBuffers.stConfig.Cdb.LittleEndian = true;
#endif
    /* allow user to set cdb/itb size */
    outputBuffers.stConfig.Cdb.Length = pSettings->data.fifoSize;
    outputBuffers.stConfig.Itb.Length = pSettings->index.fifoSize;
    BDBG_MSG(("CDB size = 0x%x, ITB size = 0x%x", outputBuffers.stConfig.Cdb.Length, outputBuffers.stConfig.Itb.Length));
    /* ITB buffer must be non-secure to be parsed by host mux manager; and
     * ITB buffer must be on MEMC0 since CABAC hw generates the ITB and CABAC is tied to MEMC0 for security reason; */
    outputBuffers.hITBMem = g_pCoreHandles->heap[0];
    rc = BVCE_Output_AllocBuffers(device->vce, &encoder->buffers, &outputBuffers);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    rc = BVCE_Output_SetBuffers(encoder->output, encoder->buffers);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    BVCE_Channel_GetDefaultEncodeSettings(&encodeSettings);
    encoder->settings.bitrateMax = encodeSettings.stBitRate.uiMax;
    encoder->settings.encoderDelay = encodeSettings.uiA2PDelay;
    encoder->settings.frameRate = NEXUS_P_FrameRate_FromMagnum(encodeSettings.stFrameRate.eFrameRate);
    encoder->settings.variableFrameRate = encodeSettings.stFrameRate.bVariableFrameRateMode;
    encoder->settings.enableFieldPairing = encodeSettings.bITFPEnable;
    encoder->settings.streamStructure.openGop = encodeSettings.stGOPStructure.bAllowOpenGOP;
    encoder->settings.streamStructure.framesP = encodeSettings.stGOPStructure.uiNumberOfPFrames;
    encoder->settings.streamStructure.framesB = encodeSettings.stGOPStructure.uiNumberOfBFrames;
#if BVCE_P_DUMP_ARC_DEBUG_LOG
    g_NEXUS_VideoEncoder_P_State.logTimer = NEXUS_ScheduleTimer(100, NEXUS_VideoEncoder_P_LogTimer, NULL);
#endif
    return encoder;

error:
    if(encoder) {
        encoder->enc = NULL; /* clear 'enc' field, it's used to mark 'opened' encoders */
    }
    return NULL;
}

static void 
NEXUS_VideoEncoder_P_Finalizer(NEXUS_VideoEncoderHandle encoder)
{
    NEXUS_OBJECT_ASSERT(NEXUS_VideoEncoder, encoder);
    /* cancel timer here to be symmetric with open */
#if BVCE_P_DUMP_ARC_DEBUG_LOG
    if(g_NEXUS_VideoEncoder_P_State.logTimer) {
        NEXUS_CancelTimer(g_NEXUS_VideoEncoder_P_State.logTimer);
        g_NEXUS_VideoEncoder_P_State.logTimer = NULL;
    }
#endif
    BVCE_Channel_Close(encoder->enc);
    BVCE_Output_Close(encoder->output);
    BVCE_Output_FreeBuffers(encoder->buffers);
    NEXUS_OBJECT_DESTROY(NEXUS_VideoEncoder, encoder);
    encoder->enc = NULL;
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_VideoEncoder, NEXUS_VideoEncoder_Close);

void 
NEXUS_VideoEncoder_GetDefaultStartSettings(NEXUS_VideoEncoderStartSettings *pSettings )
{
    BVCE_Channel_StartEncodeSettings encodeSettings;

    BVCE_Channel_GetDefaultStartEncodeSettings(&encodeSettings);

    BDBG_CASSERT(NEXUS_VideoCodecLevel_eUnknown ==  (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_eUnknown);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e00 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e00);          
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e10 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e10);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e1B ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e1B);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e11 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e11);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e12 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e12);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e13 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e13);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e20 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e20);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e21 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e21);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e22 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e22);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e30 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e30);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e31 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e31);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e32 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e32);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e40 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e40);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e41 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e41);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e42 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e42);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e50 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e50);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e51 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e51);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e60 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e60);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_e62 ==       (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_e62);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_eLow ==      (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_eLow);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_eMain ==     (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_eMain);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_eHigh ==     (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_eHigh);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_eHigh1440 == (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_eHigh1440);
    BDBG_CASSERT(NEXUS_VideoCodecLevel_eMax ==      (NEXUS_VideoCodecLevel)BAVC_VideoCompressionLevel_eMax);

    BDBG_CASSERT(NEXUS_VideoCodecProfile_eUnknown ==           (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eUnknown);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eSimple ==            (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eSimple);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eMain ==              (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eMain);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eHigh ==              (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eHigh);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eAdvanced ==          (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eAdvanced);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eJizhun ==            (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eJizhun);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eSnrScalable ==       (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eSnrScalable);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eSpatiallyScalable == (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eSpatiallyScalable);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eAdvancedSimple ==    (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eAdvancedSimple);
    BDBG_CASSERT(NEXUS_VideoCodecProfile_eBaseline ==          (NEXUS_VideoCodecProfile)BAVC_VideoCompressionProfile_eBaseline);

 
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->nonRealTime = encodeSettings.bNonRealTimeEncodeMode;
    pSettings->lowDelayPipeline = encodeSettings.bPipelineLowDelayMode;
    pSettings->rateBufferDelay = encodeSettings.uiRateBufferDelay;
    pSettings->adaptiveLowDelayMode = encodeSettings.bAdaptiveLowDelayMode;
    pSettings->encodeUserData = false;
    pSettings->bounds.inputDimension.max.width = encodeSettings.stBounds.stDimensions.stMax.uiWidth;
    pSettings->bounds.inputDimension.max.height = encodeSettings.stBounds.stDimensions.stMax.uiHeight;
    pSettings->bounds.outputFrameRate.min = NEXUS_P_FrameRate_FromMagnum(encodeSettings.stBounds.stFrameRate.eMin);
    pSettings->bounds.outputFrameRate.max = NEXUS_P_FrameRate_FromMagnum(encodeSettings.stBounds.stFrameRate.eMax);
    pSettings->bounds.inputFrameRate.min = NEXUS_P_FrameRate_FromMagnum(encodeSettings.stBounds.stInputFrameRate.eMin);

    pSettings->interlaced = encodeSettings.eInputType == BAVC_ScanType_eInterlaced;
    pSettings->codec = NEXUS_P_VideoCodec_FromMagnum(encodeSettings.stProtocolInfo.eProtocol);
    pSettings->profile = encodeSettings.stProtocolInfo.eProtocol;
    pSettings->level = encodeSettings.stProtocolInfo.eLevel;
    pSettings->stcChannel = NULL;

    return;
}

static NEXUS_Error
NEXUS_VideoEncoder_P_ConvertSettings(const NEXUS_VideoEncoderSettings *pSettings, BVCE_Channel_EncodeSettings *encodeSettings)
{
    BERR_Code rc;

    BVCE_Channel_GetDefaultEncodeSettings(encodeSettings);
    rc = NEXUS_P_FrameRate_ToMagnum(pSettings->frameRate, &encodeSettings->stFrameRate.eFrameRate);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
    encodeSettings->stFrameRate.bVariableFrameRateMode = pSettings->variableFrameRate;
    encodeSettings->bITFPEnable = pSettings->enableFieldPairing;
    encodeSettings->stBitRate.uiMax = pSettings->bitrateMax;
    encodeSettings->stGOPStructure.bAllowOpenGOP = pSettings->streamStructure.openGop; 
    encodeSettings->stGOPStructure.uiNumberOfPFrames = pSettings->streamStructure.framesP;
    encodeSettings->stGOPStructure.uiNumberOfBFrames = pSettings->streamStructure.framesB;
    encodeSettings->uiA2PDelay = pSettings->encoderDelay;
    return BERR_SUCCESS;
}

NEXUS_Error 
NEXUS_VideoEncoder_P_ConvertStartSettings(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderStartSettings *pSettings, BVCE_Channel_StartEncodeSettings *startEncodeSettings)
{
    BERR_Code rc;
    unsigned stcChannelIndex;

    if(pSettings->stcChannel == NULL) {rc=BERR_TRACE(NEXUS_INVALID_PARAMETER);goto error;}

    BVCE_Channel_GetDefaultStartEncodeSettings(startEncodeSettings);
    startEncodeSettings->stProtocolInfo.eProtocol = NEXUS_P_VideoCodec_ToMagnum(pSettings->codec, NEXUS_TransportType_eEs);
    startEncodeSettings->stProtocolInfo.eLevel    = pSettings->level;
    startEncodeSettings->stProtocolInfo.eProfile  = pSettings->profile;
    startEncodeSettings->bNonRealTimeEncodeMode   = pSettings->nonRealTime;
    startEncodeSettings->bPipelineLowDelayMode    = pSettings->lowDelayPipeline;
    startEncodeSettings->bAdaptiveLowDelayMode    = pSettings->adaptiveLowDelayMode;
    startEncodeSettings->uiRateBufferDelay        = pSettings->rateBufferDelay;
    startEncodeSettings->stBounds.stDimensions.stMax.uiWidth = pSettings->bounds.inputDimension.max.width; 
    startEncodeSettings->stBounds.stDimensions.stMax.uiHeight = pSettings->bounds.inputDimension.max.height;
    rc = NEXUS_P_FrameRate_ToMagnum(pSettings->bounds.outputFrameRate.min, &startEncodeSettings->stBounds.stFrameRate.eMin);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
    rc = NEXUS_P_FrameRate_ToMagnum(pSettings->bounds.outputFrameRate.max, &startEncodeSettings->stBounds.stFrameRate.eMax);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
    rc = NEXUS_P_FrameRate_ToMagnum(pSettings->bounds.inputFrameRate.min, &startEncodeSettings->stBounds.stInputFrameRate.eMin);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    startEncodeSettings->eInputType = pSettings->interlaced ? BAVC_ScanType_eInterlaced : BAVC_ScanType_eProgressive;
    startEncodeSettings->hOutputHandle = encoder->output;
    NEXUS_Module_Lock(g_NEXUS_VideoEncoder_P_State.config.transport);
    NEXUS_StcChannel_GetIndex_priv(pSettings->stcChannel, &stcChannelIndex); 
    NEXUS_Module_Unlock(g_NEXUS_VideoEncoder_P_State.config.transport);
    startEncodeSettings->uiStcIndex = stcChannelIndex;

    return BERR_SUCCESS;

error:
    return rc;
}


static NEXUS_Error
NEXUS_VideoEncoder_P_ApplySettings(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettings *pSettings)
{
    BVCE_Channel_EncodeSettings encodeSettings;
    BERR_Code rc;

    rc = NEXUS_VideoEncoder_P_ConvertSettings(pSettings, &encodeSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    rc = BVCE_Channel_SetEncodeSettings(encoder->enc, &encodeSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

NEXUS_Error 
NEXUS_VideoEncoder_Start(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderStartSettings *pSettings)
{
    BERR_Code rc;
    BVCE_Channel_StartEncodeSettings startEncodeSettings;
    BXUDlib_Settings xudSettings;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pSettings);

    xudSettings.sinkInterface.pPrivateSinkContext = (void *)encoder->enc;
    xudSettings.sinkInterface.userDataAdd_isr = (BXUDlib_UserDataSink_Add)BVCE_Channel_UserData_AddBuffers_isr;
    xudSettings.sinkInterface.userDataStatus_isr = (BXUDlib_UserDataSink_Status)BVCE_Channel_UserData_GetStatus_isr;
    
    rc = NEXUS_VideoEncoder_P_ConvertStartSettings(encoder, pSettings, &startEncodeSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    rc = NEXUS_VideoEncoder_P_ApplySettings(encoder, &encoder->settings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    rc = BVCE_Channel_StartEncode(encoder->enc, &startEncodeSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    encoder->startSettings = *pSettings;

    NEXUS_Module_Lock(g_NEXUS_VideoEncoder_P_State.config.display);
    rc = NEXUS_DisplayModule_SetUserDataEncodeMode_priv(pSettings->input, pSettings->encodeUserData, &xudSettings);
    NEXUS_Module_Unlock(g_NEXUS_VideoEncoder_P_State.config.display);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

void 
NEXUS_VideoEncoder_GetDefaultStopSettings(NEXUS_VideoEncoderStopSettings *pSettings)
{
    BVCE_Channel_StopEncodeSettings stopEncodeSettings;

    BDBG_ASSERT(pSettings);
    BDBG_CASSERT((unsigned)NEXUS_VideoEncoderStopMode_eNormal==(unsigned)BVCE_Channel_StopMode_eNormal);
    BDBG_CASSERT((unsigned)NEXUS_VideoEncoderStopMode_eImmediate==(unsigned)BVCE_Channel_StopMode_eImmediate);
    BDBG_CWARNING((unsigned)NEXUS_VideoEncoderStopMode_eMax==(unsigned)BVCE_Channel_StopMode_eMax);
    BVCE_Channel_GetDefaultStopEncodeSettings(&stopEncodeSettings);
    pSettings->mode = stopEncodeSettings.eStopMode;
    return;
}

void 
NEXUS_VideoEncoder_Stop( NEXUS_VideoEncoderHandle encoder,const NEXUS_VideoEncoderStopSettings *pSettings)
{
    BERR_Code rc;
    BVCE_Channel_StopEncodeSettings stopSettings;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);

    NEXUS_Module_Lock(g_NEXUS_VideoEncoder_P_State.config.display);
    rc = NEXUS_DisplayModule_SetUserDataEncodeMode_priv(encoder->startSettings.input, false, NULL);
    NEXUS_Module_Unlock(g_NEXUS_VideoEncoder_P_State.config.display);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    
    BVCE_Channel_GetDefaultStopEncodeSettings(&stopSettings);
    if(pSettings) {
        stopSettings.eStopMode = pSettings->mode;
    }
    rc = BVCE_Channel_StopEncode(encoder->enc, &stopSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    return;
}

void 
NEXUS_VideoEncoder_GetSettings(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pSettings);
    *pSettings = encoder->settings;
    return;
}

NEXUS_Error 
NEXUS_VideoEncoder_SetSettings( NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettings *pSettings)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pSettings);

    rc = NEXUS_VideoEncoder_P_ApplySettings(encoder, pSettings);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    encoder->settings = *pSettings;

    return NEXUS_SUCCESS;
error:
    return rc;
}



NEXUS_Error 
NEXUS_VideoEncoder_GetBuffer( NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderDescriptor **pBuffer, size_t *pSize, const NEXUS_VideoEncoderDescriptor **pBuffer2, size_t *pSize2)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pBuffer);
    BDBG_ASSERT(pSize);
    BDBG_ASSERT(pBuffer2);
    BDBG_ASSERT(pSize2);

    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, flags, BAVC_VideoBufferDescriptor, stCommon.uiFlags);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, originalPts, BAVC_VideoBufferDescriptor, stCommon.uiOriginalPTS);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, pts, BAVC_VideoBufferDescriptor, stCommon.uiPTS);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, stcSnapshot, BAVC_VideoBufferDescriptor, stCommon.uiSTCSnapshot);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, escr, BAVC_VideoBufferDescriptor, stCommon.uiESCR);

    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, ticksPerBit, BAVC_VideoBufferDescriptor, stCommon.uiTicksPerBit);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, shr, BAVC_VideoBufferDescriptor, stCommon.iSHR);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, offset, BAVC_VideoBufferDescriptor, stCommon.uiOffset);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, length, BAVC_VideoBufferDescriptor, stCommon.uiLength);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, dts, BAVC_VideoBufferDescriptor, uiDTS);
    NEXUS_ASSERT_FIELD(NEXUS_VideoEncoderDescriptor, dataUnitType, BAVC_VideoBufferDescriptor, uiDataUnitType);
    NEXUS_ASSERT_STRUCTURE(NEXUS_VideoEncoderDescriptor, BAVC_VideoBufferDescriptor);

    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_ORIGINALPTS_VALID ==  BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID);
    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_PTS_VALID == BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID);
    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_FRAME_START ==  BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START);
    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_EOS ==  BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS );
    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_METADATA == BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA );
    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_VIDEOFLAG_DTS_VALID == BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID);
    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_VIDEOFLAG_RAP == BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP);
    BDBG_CASSERT(NEXUS_VIDEOENCODERDESCRIPTOR_VIDEOFLAG_DATA_UNIT_START == BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START);


    rc = BVCE_Output_GetBufferDescriptors(encoder->output, (void *)pBuffer, pSize, (void *)pBuffer2, pSize2);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

NEXUS_Error 
NEXUS_VideoEncoder_ReadComplete(NEXUS_VideoEncoderHandle encoder, unsigned descriptorsCompleted)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);

    rc = BVCE_Output_ConsumeBufferDescriptors(encoder->output , descriptorsCompleted);

    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

void 
NEXUS_VideoEncoder_GetStatus(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderStatus *pStatus)
{
    NEXUS_Error rc;
    BVCE_Channel_Status status;
    BAVC_VideoBufferStatus bufferStatus;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);

    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_INVALID_INPUT_DIMENSION        == NEXUS_VIDEOENCODER_ERROR_INVALID_INPUT_DIMENSION);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_LATE                 == NEXUS_VIDEOENCODER_ERROR_USER_DATA_LATE);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_DUPLICATE            == NEXUS_VIDEOENCODER_ERROR_USER_DATA_DUPLICATE);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_ADJUSTS_WRONG_FRAME_RATE       == NEXUS_VIDEOENCODER_ERROR_ADJUSTS_WRONG_FRAME_RATE);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_BVN_FRAME_RATE     == NEXUS_VIDEOENCODER_ERROR_UNSUPPORTED_BVN_FRAME_RATE);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_RESOLUTION         == NEXUS_VIDEOENCODER_ERROR_UNSUPPORTED_RESOLUTION);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_MIN_FRAME_RATE    == NEXUS_VIDEOENCODER_ERROR_MISMATCH_BVN_MIN_FRAME_RATE);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_PIC_RESOLUTION    == NEXUS_VIDEOENCODER_ERROR_MISMATCH_BVN_PIC_RESOLUTION);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_MAX_BITRATE_EXCEEDED           == NEXUS_VIDEOENCODER_ERROR_MAX_BITRATE_EXCEEDED);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_BIN_BUFFER_FULL                ==  NEXUS_VIDEOENCODER_ERROR_BIN_BUFFER_FULL);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_CDB_FULL                       ==  NEXUS_VIDEOENCODER_ERROR_CDB_FULL);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_PICARC_TO_CABAC_DINO_BUFFER_FULL == NEXUS_VIDEOENCODER_ERROR_PICARC_TO_CABAC_DINO_BUFFER_FULL);
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_ERROR_EBM_FULL                       == NEXUS_VIDEOENCODER_ERROR_EBM_FULL);
                                                                        
    BDBG_CASSERT(BVCE_CHANNEL_STATUS_FLAGS_EVENT_INPUT_CHANGE                   == NEXUS_VIDEOENCODER_EVENT_INPUT_CHANGE);
        

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BVCE_Output_GetBufferStatus(encoder->output, &bufferStatus);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    pStatus->bufferBase = bufferStatus.stCommon.pFrameBufferBaseAddress;
    pStatus->metadataBufferBase = bufferStatus.stCommon.pMetadataBufferBaseAddress;
    BDBG_MODULE_MSG(nexus_video_encoder_status, ("base addr@%#x : metadata base addr@%#x", bufferStatus.stCommon.pFrameBufferBaseAddress, bufferStatus.stCommon.pMetadataBufferBaseAddress));
    rc = BVCE_Channel_GetStatus(encoder->enc, &status);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    pStatus->errorFlags = status.uiErrorFlags;
    pStatus->errorCount = status.uiTotalErrors;
    pStatus->picturesReceived = status.uiTotalPicturesReceived;
    pStatus->picturesDroppedFRC = status.uiTotalPicturesDroppedDueToFrameRateConversion;
    pStatus->picturesDroppedErrors = status.uiTotalPicturesDroppedDueToErrors;
    pStatus->picturesEncoded = status.uiTotalPicturesEncoded;
    pStatus->pictureIdLastEncoded = status.uiLastPictureIdEncoded;

    return; 
error:
    return;
}

void NEXUS_VideoEncoder_GetDefaultClearStatus(NEXUS_VideoEncoderClearStatus *pClearStatus)
{
    BDBG_ASSERT(pClearStatus);
    pClearStatus->errorFlags = 
        NEXUS_VIDEOENCODER_ERROR_INVALID_INPUT_DIMENSION |
        NEXUS_VIDEOENCODER_ERROR_USER_DATA_LATE |
        NEXUS_VIDEOENCODER_ERROR_USER_DATA_DUPLICATE |
        NEXUS_VIDEOENCODER_ERROR_ADJUSTS_WRONG_FRAME_RATE |
        NEXUS_VIDEOENCODER_ERROR_UNSUPPORTED_BVN_FRAME_RATE |
        NEXUS_VIDEOENCODER_ERROR_UNSUPPORTED_RESOLUTION |
        NEXUS_VIDEOENCODER_ERROR_MISMATCH_BVN_MIN_FRAME_RATE |
        NEXUS_VIDEOENCODER_ERROR_MISMATCH_BVN_PIC_RESOLUTION |
        NEXUS_VIDEOENCODER_ERROR_MAX_BITRATE_EXCEEDED |
        NEXUS_VIDEOENCODER_ERROR_BIN_BUFFER_FULL |
        NEXUS_VIDEOENCODER_ERROR_CDB_FULL |
        NEXUS_VIDEOENCODER_ERROR_PICARC_TO_CABAC_DINO_BUFFER_FULL |
        NEXUS_VIDEOENCODER_ERROR_EBM_FULL;

    pClearStatus->eventFlags = NEXUS_VIDEOENCODER_EVENT_INPUT_CHANGE;

    return;
}

void NEXUS_VideoEncoder_ClearStatus(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderClearStatus *pClearStatus)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pClearStatus);
    if(pClearStatus==NULL) {
        rc = BVCE_Channel_ClearStatus(encoder->enc, NULL);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    } else {
        BVCE_Channel_Status status;
        BKNI_Memset(&status, 0, sizeof(status));
        status.uiErrorFlags = pClearStatus->errorFlags;
        status.uiEventFlags = pClearStatus->eventFlags;
        rc = BVCE_Channel_ClearStatus(encoder->enc, &status);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    }
error:
    return;
}

NEXUS_Error
NEXUS_VideoEncoder_GetDelayRange(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettings *pSettings, const NEXUS_VideoEncoderStartSettings *pStartSettings, NEXUS_VideoEncoderDelayRange *pDelayRange)
{
    BERR_Code rc;
    BVCE_Channel_StartEncodeSettings startEncodeSettings;
    BVCE_Channel_EncodeSettings encodeSettings;
    BVCE_A2PDelay A2PDelay;


    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pStartSettings);
    BDBG_ASSERT(pDelayRange);


    rc = NEXUS_VideoEncoder_P_ConvertStartSettings(encoder, pStartSettings, &startEncodeSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    rc = NEXUS_VideoEncoder_P_ConvertSettings(pSettings, &encodeSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    rc = BVCE_GetA2PDelayInfo(&encodeSettings, &startEncodeSettings, &A2PDelay);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    pDelayRange->min = A2PDelay.uiMin;
    pDelayRange->max = A2PDelay.uiMax;

    return NEXUS_SUCCESS;

error:
    return rc;
}

void 
NEXUS_VideoEncoder_GetSettingsOnInputChange(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderSettingsOnInputChange *pSettings)
{
    BVCE_Channel_EncodeSettings_OnInputChange settings;
    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pSettings);

    BKNI_Memset(&settings, 0, sizeof(settings));
    BVCE_Channel_GetEncodeSettings_OnInputChange(encoder->enc, &settings);
    pSettings->bitrateMax = settings.stBitRate.uiMax;

    return;
}

NEXUS_Error NEXUS_VideoEncoder_SetSettingsOnInputChange(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettingsOnInputChange *pSettings)
{
    BVCE_Channel_EncodeSettings_OnInputChange settings;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pSettings);
    BVCE_Channel_GetEncodeSettings_OnInputChange(encoder->enc, &settings);
    settings.stBitRate.uiMax = pSettings->bitrateMax ;
    rc = BVCE_Channel_SetEncodeSettings_OnInputChange(encoder->enc, &settings);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_VideoEncoder_InsertRandomAccessPoint(NEXUS_VideoEncoderHandle encoder)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    rc = BVCE_Channel_BeginNewRAP(encoder->enc);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}

