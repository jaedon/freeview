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
 * $brcm_Revision: 9 $
 * $brcm_Date: 6/21/12 3:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_encoder/7231/src/nexus_video_encoder.c $
 * 
 * 9   6/21/12 3:45p randyjew
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 8   6/14/12 10:32a gmohile
 * SW7425-3156: Added ClearStatus API
 * 
 * 7   4/19/12 1:46p randyjew
 * SW7425-2875: Use originalPts instead of originalPTS
 * 
 * 6   3/15/12 2:29p gmohile
 * SW7231-584,SW7231-586: Disable frame drop in nexus for DSP video encode
 * 
 * 5   3/7/12 3:15p gmohile
 * SW7231-586,SW7231-584 : go bit support for raaga encoder
 * 
 * 4   2/13/12 4:24p gmohile
 * SW7231-629 : Set end2end delay for raaga video encoder
 * 
 * 3   2/2/12 2:34p gmohile
 * SW7231-587 : Pass original PTS to raaga encoder
 * 
 * 2   1/31/12 5:01p gmohile
 * SW7425-1473 : Fix raaga encoder support
 * 
 * 1   1/16/12 6:03p gmohile
 * SW7425-1473 : Merge raaga encoder support
 * 
 **************************************************************************/
#include "nexus_video_encoder_module.h"
#include "nexus_power_management.h"

#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_display_priv.h"
#include "priv/nexus_core_img_id.h"
#include "priv/nexus_core.h"


BDBG_MODULE(nexus_video_encoder);
BDBG_FILE_MODULE(nexus_video_encoder_status);


NEXUS_VideoEncoder_P_State g_NEXUS_VideoEncoder_P_State;

#if NEXUS_NUM_DSP_VIDEO_ENCODERS

#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_VideoEncoder_P_State.config.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_VideoEncoder_P_State.config.transport)
#define LOCK_DISPLAY()    NEXUS_Module_Lock(g_NEXUS_VideoEncoder_P_State.config.display)
#define UNLOCK_DISPLAY()  NEXUS_Module_Unlock(g_NEXUS_VideoEncoder_P_State.config.display)
#define LOCK_AUDIO()    NEXUS_Module_Lock(g_NEXUS_VideoEncoder_P_State.config.audio)
#define UNLOCK_AUDIO()  NEXUS_Module_Unlock(g_NEXUS_VideoEncoder_P_State.config.audio)



void 
NEXUS_VideoEncoderModule_GetDefaultSettings( NEXUS_VideoEncoderModuleSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    
    return;
}

NEXUS_ModuleHandle 
NEXUS_VideoEncoderModule_Init(const NEXUS_VideoEncoderModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;    
    BERR_Code rc;

    BDBG_ASSERT(pSettings);
    
    BDBG_ASSERT(g_NEXUS_VideoEncoder_P_State.module==NULL);
    BDBG_ASSERT(pSettings->display);
    BDBG_ASSERT(pSettings->transport);
    BDBG_ASSERT(pSettings->audio);
    BKNI_Memset(&g_NEXUS_VideoEncoder_P_State, 0, sizeof(g_NEXUS_VideoEncoder_P_State));
    g_NEXUS_VideoEncoder_P_State.config = *pSettings;
    
    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow; /* encoder interface is slow */
    g_NEXUS_VideoEncoder_P_State.module = NEXUS_Module_Create("video_encoder", &moduleSettings);
    if(g_NEXUS_VideoEncoder_P_State.module == NULL) { rc = BERR_TRACE(BERR_OS_ERROR); goto error; }
    
    return g_NEXUS_VideoEncoder_P_State.module;
error:
    return NULL;
}

void 
NEXUS_VideoEncoderModule_Uninit(void)
{    
    unsigned i;
    NEXUS_VideoEncoder_P_Device *device = &g_NEXUS_VideoEncoder_P_State.device;

    if(g_NEXUS_VideoEncoder_P_State.module==NULL) {return;}
    
    for(i=0;i<NEXUS_NUM_DSP_VIDEO_ENCODERS;i++ ) { 
	if(device->channels[i].encoder) { 
	    NEXUS_VideoEncoder_Close(&device->channels[i]); 
	}
    }
    NEXUS_Module_Destroy(g_NEXUS_VideoEncoder_P_State.module);
    g_NEXUS_VideoEncoder_P_State.module = NULL;
    return;
}

NEXUS_Error NEXUS_VideoEncoderModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{  
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    
    return NEXUS_SUCCESS;
}

void NEXUS_VideoEncoder_GetDefaultOpenSettings(NEXUS_VideoEncoderOpenSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_VideoEncoderHandle 
NEXUS_VideoEncoder_Open(unsigned index, const NEXUS_VideoEncoderOpenSettings *pSettings)
{
    BERR_Code rc;
    NEXUS_VideoEncoderOpenSettings settings;    
    NEXUS_VideoEncoder_P_Device *device;
    NEXUS_VideoEncoderHandle encoder;
    NEXUS_RaveOpenSettings raveSettings;   
    NEXUS_DspVideoEncoderOpenSettings dspOpenSettings;

    if(index>=NEXUS_NUM_DSP_VIDEO_ENCODERS) {rc=BERR_TRACE(NEXUS_INVALID_PARAMETER);goto error;}
    
    device = &g_NEXUS_VideoEncoder_P_State.device;
    encoder = &device->channels[index];
    
    if(pSettings==NULL) {
        NEXUS_VideoEncoder_GetDefaultOpenSettings(&settings);
        pSettings=&settings;
    }
    
	NEXUS_OBJECT_SET(NEXUS_VideoEncoder, encoder);    
    encoder->openSettings = *pSettings;
    encoder->device = device;

    /* Setup rave buffer */
    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(&raveSettings);
    UNLOCK_TRANSPORT();

    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_GetRaveSettings_priv(&raveSettings);
    UNLOCK_AUDIO();

    if ( 0 != pSettings->data.fifoSize )
    {
        raveSettings.config.Cdb.Length = pSettings->data.fifoSize;
    }
    if ( 0 != pSettings->index.fifoSize )
    {
        raveSettings.config.Itb.Length = pSettings->index.fifoSize;
    }
    
    LOCK_TRANSPORT();
    encoder->raveContext = NEXUS_Rave_Open_priv(&raveSettings);
    UNLOCK_TRANSPORT();
    if ( NULL == encoder->raveContext )
    {
        (void)BERR_TRACE(BERR_UNKNOWN);        
        goto error;
    }    
    
    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_GetDefaultOpenSettings_priv(&dspOpenSettings);
    encoder->encoder = NEXUS_DspVideoEncoder_Open_priv(index, &dspOpenSettings);  
    UNLOCK_AUDIO();
    if(!encoder->encoder) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error;}
        
    encoder->started = false;
    encoder->settings.bitrateMax = 400000;
    encoder->settings.frameRate = NEXUS_VideoFrameRate_e30;
    /* Other settings are not used currently */

    return encoder;
    
error:
    return NULL;    
}

static void 
NEXUS_VideoEncoder_P_Finalizer(NEXUS_VideoEncoderHandle encoder)
{
    NEXUS_OBJECT_ASSERT(NEXUS_VideoEncoder, encoder);    
    
    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_Close_priv(encoder->encoder);
    UNLOCK_AUDIO();
    encoder->encoder = NULL;

    LOCK_TRANSPORT();
    NEXUS_Rave_Close_priv(encoder->raveContext);
    UNLOCK_TRANSPORT();
 
	NEXUS_OBJECT_UNSET(NEXUS_VideoEncoder, encoder);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_VideoEncoder, NEXUS_VideoEncoder_Close);


void 
NEXUS_VideoEncoder_GetDefaultStartSettings(NEXUS_VideoEncoderStartSettings *pSettings )
{
    NEXUS_DspVideoEncoderStartSettings startSettings;
    
    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_GetDefaultStartSettings_priv(&startSettings);    
    UNLOCK_AUDIO();

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->nonRealTime = startSettings.nonRealTime;
    pSettings->codec = startSettings.codec;
    pSettings->profile = startSettings.profile;
    pSettings->level = startSettings.level;
    pSettings->bounds.outputFrameRate.min = startSettings.framerate;
    pSettings->bounds.outputFrameRate.max = startSettings.framerate;
    pSettings->bounds.inputDimension.max.width = startSettings.width;
    pSettings->bounds.inputDimension.max.height = startSettings.height;

    return;
}

NEXUS_Error
NEXUS_VideoEncoder_P_EnqueueCb_isr(void * context, NEXUS_DisplayCapturedImage *image)
{    
    NEXUS_VideoEncoderHandle encoder = (NEXUS_VideoEncoderHandle) context;    
    NEXUS_DspVideoEncoderPicture picture;
    NEXUS_Error rc = NEXUS_UNKNOWN;

    picture.pvaddress = image->pvaddress;
    picture.width = image->width;
    picture.height = image->height;
    picture.polarity = image->polarity;
    picture.origPts = image->origPts;
    picture.framerate = image->framerate;

    BDBG_MSG(("Enqueue Pictre %08x", image->pvaddress));

    if(encoder->started)
	rc = NEXUS_DspVideoEncoder_EnqueuePicture_isr(encoder->encoder, &picture);
    
    return rc;
}

NEXUS_Error
NEXUS_VideoEncoder_P_DequeueCb_isr(void * context, NEXUS_DisplayCapturedImage *image)
{
    NEXUS_VideoEncoderHandle encoder = (NEXUS_VideoEncoderHandle) context;    
    NEXUS_DspVideoEncoderPicture picture;
    NEXUS_Error rc = NEXUS_UNKNOWN;

    if(encoder->started)
	rc = NEXUS_DspVideoEncoder_DequeuePicture_isr(encoder->encoder, &picture);

    image->pvaddress = picture.pvaddress;
    image->width = picture.width;
    image->height = picture.height;
    image->polarity = picture.polarity;
    
    BDBG_MSG(("Dequeue Pictre %08x", image->pvaddress));

    return rc;
}

NEXUS_Error 
NEXUS_VideoEncoder_Start(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderStartSettings *pSettings)
{
    BERR_Code rc;
    NEXUS_DspVideoEncoderStartSettings startSettings;
    NEXUS_DisplayEncoderSettings displaySettings;
   
    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_GetDefaultStartSettings_priv(&startSettings); 
    rc = NEXUS_DspVideoEncoder_GetExtInterruptInfo(encoder->encoder, &startSettings.extIntInfo);
    UNLOCK_AUDIO();
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    

    startSettings.nonRealTime = pSettings->nonRealTime;
    startSettings.codec = pSettings->codec;
    startSettings.profile = pSettings->profile;
    startSettings.level = pSettings->level;
    startSettings.width = pSettings->bounds.inputDimension.max.width;
    startSettings.height = pSettings->bounds.inputDimension.max.height;
    startSettings.stcChannel = pSettings->stcChannel;
    startSettings.framerate = encoder->settings.frameRate;
    startSettings.bitrate = encoder->settings.bitrateMax;
    startSettings.encoderDelay = encoder->settings.encoderDelay/27000; /* Convert from 27MHz to ms */
    startSettings.raveContext = encoder->raveContext;                    

    displaySettings.enqueueCb_isr = NEXUS_VideoEncoder_P_EnqueueCb_isr;
    displaySettings.dequeueCb_isr = NEXUS_VideoEncoder_P_DequeueCb_isr;
    displaySettings.context = encoder;
    displaySettings.encodeRate = encoder->settings.frameRate;
    displaySettings.extIntAddress = startSettings.extIntInfo.address;
    displaySettings.extIntBitNum = startSettings.extIntInfo.bit_num;

    LOCK_DISPLAY();
    rc = NEXUS_Display_SetEncoderCallback_priv(pSettings->input, &displaySettings);
    UNLOCK_DISPLAY();
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    LOCK_AUDIO();
    rc = NEXUS_DspVideoEncoder_Start_priv(encoder->encoder, &startSettings);
    UNLOCK_AUDIO();
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto dsp_error;}
    BKNI_EnterCriticalSection();
    encoder->started = true;
    BKNI_LeaveCriticalSection();

    LOCK_DISPLAY();
    rc = NEXUS_Display_EnableEncoderCallback_priv(pSettings->input);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto display_error;}
    UNLOCK_DISPLAY();

    encoder->startSettings = *pSettings;    

    return rc;

display_error:
    BKNI_EnterCriticalSection();
    encoder->started = false;
    BKNI_LeaveCriticalSection();
    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_Stop_priv(encoder->encoder);
    UNLOCK_AUDIO();      
dsp_error:
error:
    return rc;
}

void NEXUS_VideoEncoder_GetDefaultStopSettings(NEXUS_VideoEncoderStopSettings *pSettings)
{
    BSTD_UNUSED(pSettings);

    return;
}

void 
NEXUS_VideoEncoder_Stop( NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderStopSettings *pSettings)
{
    BERR_Code rc;
    
    BSTD_UNUSED(pSettings);

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);   
    
    if(!encoder->started)
	return;

    LOCK_DISPLAY();
    rc = NEXUS_Display_DisableEncoderCallback_priv(encoder->startSettings.input);
    UNLOCK_DISPLAY();

    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_Stop_priv(encoder->encoder);
    UNLOCK_AUDIO();  
    BKNI_EnterCriticalSection();
    encoder->started = false;
    BKNI_LeaveCriticalSection();
    
    LOCK_DISPLAY();
    rc = NEXUS_Display_SetEncoderCallback_priv(encoder->startSettings.input, NULL);
    UNLOCK_DISPLAY();
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
    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BDBG_ASSERT(pSettings);

    /* Apply the settings here */
    encoder->settings = *pSettings;

    return NEXUS_SUCCESS;
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
    
    LOCK_AUDIO();
    rc = NEXUS_DspVideoEncoder_GetBuffer_priv(encoder->encoder, (void *)pBuffer, pSize, (void *)pBuffer2, pSize2);
    UNLOCK_AUDIO();
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

    LOCK_AUDIO();
    rc = NEXUS_DspVideoEncoder_ReadComplete(encoder->encoder, descriptorsCompleted);
    UNLOCK_AUDIO();
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

void 
NEXUS_VideoEncoder_GetStatus(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderStatus *pStatus)
{    
    NEXUS_DspVideoEncoderStatus bufferStatus;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_VideoEncoder);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    LOCK_AUDIO();
    NEXUS_DspVideoEncoder_GetStatus(encoder->encoder, &bufferStatus);
    UNLOCK_AUDIO();        

    pStatus->bufferBase = bufferStatus.bufferBase;
    pStatus->metadataBufferBase = bufferStatus.metadataBufferBase;
   
    return;
}

NEXUS_Error
NEXUS_VideoEncoder_GetDelayRange (NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettings *pSettings, const NEXUS_VideoEncoderStartSettings *pStartSettings, NEXUS_VideoEncoderDelayRange *pDelayRange)
{    
    NEXUS_Error rc;
    uint32_t delay;  
    
    LOCK_AUDIO();
    rc = NEXUS_DspVideoEncoder_GetDelayRange(encoder->encoder, pStartSettings->bounds.inputDimension.max.width, pStartSettings->bounds.inputDimension.max.height, pSettings->frameRate, pSettings->bitrateMax, &delay);
    UNLOCK_AUDIO();   
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    /* Video encoder delay is in 27MHz ticks */
    pDelayRange->min = delay*27000;
    pDelayRange->max = delay*27000;

error:
    return rc;
}

void 
NEXUS_VideoEncoder_GetSettingsOnInputChange(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderSettingsOnInputChange *pSettings)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    
    return;
}

NEXUS_Error 
NEXUS_VideoEncoder_SetSettingsOnInputChange(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettingsOnInputChange *pSettings)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    
    return NEXUS_SUCCESS;
}

NEXUS_Error 
NEXUS_VideoEncoder_InsertRandomAccessPoint(NEXUS_VideoEncoderHandle encoder)
{
    BSTD_UNUSED(encoder);  

    return NEXUS_SUCCESS;
}

void 
NEXUS_VideoEncoder_ClearStatus(NEXUS_VideoEncoderHandle handle, const NEXUS_VideoEncoderClearStatus *pClearStatus)
{
    BSTD_UNUSED(handle); 
    BSTD_UNUSED(pClearStatus); 
    
    return;
}

void 
NEXUS_VideoEncoder_GetDefaultClearStatus(NEXUS_VideoEncoderClearStatus *pClearStatus)
{
    BSTD_UNUSED(pClearStatus); 
    
    return;
}
#else

void 
NEXUS_VideoEncoderModule_GetDefaultSettings( NEXUS_VideoEncoderModuleSettings *pSettings )
{
    BSTD_UNUSED(pSettings);
    return;
}

NEXUS_ModuleHandle 
NEXUS_VideoEncoderModule_Init(const NEXUS_VideoEncoderModuleSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    return NULL;
}

void 
NEXUS_VideoEncoderModule_Uninit(void)
{   
    return;
}

NEXUS_Error NEXUS_VideoEncoderModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{    
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}


void NEXUS_VideoEncoder_GetDefaultOpenSettings(NEXUS_VideoEncoderOpenSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    return ;
}

NEXUS_VideoEncoderHandle 
NEXUS_VideoEncoder_Open(unsigned index, const NEXUS_VideoEncoderOpenSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(index);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);    
}

void 
NEXUS_VideoEncoder_Close(NEXUS_VideoEncoderHandle encoder)
{
    BSTD_UNUSED(encoder);
    return;
}


void 
NEXUS_VideoEncoder_GetDefaultStartSettings(NEXUS_VideoEncoderStartSettings *pSettings )
{
    BSTD_UNUSED(pSettings);
    return;
}

NEXUS_Error 
NEXUS_VideoEncoder_Start(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderStartSettings *pSettings)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);   
}

void 
NEXUS_VideoEncoder_Stop( NEXUS_VideoEncoderHandle encoder)
{
    BSTD_UNUSED(encoder);
    return;
}

void 
NEXUS_VideoEncoder_GetSettings(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderSettings *pSettings)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    return;
}

NEXUS_Error 
NEXUS_VideoEncoder_SetSettings( NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettings *pSettings)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);   
}

NEXUS_Error 
NEXUS_VideoEncoder_GetBuffer( NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderDescriptor **pBuffer, size_t *pSize, const NEXUS_VideoEncoderDescriptor **pBuffer2, size_t *pSize2)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pBuffer);
    BSTD_UNUSED(pSize);
    BSTD_UNUSED(pBuffer2);
    BSTD_UNUSED(pSize2);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);   
}

NEXUS_Error 
NEXUS_VideoEncoder_ReadComplete(NEXUS_VideoEncoderHandle encoder, unsigned descriptorsCompleted)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(descriptorsCompleted);    
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);   
}

void 
NEXUS_VideoEncoder_GetStatus(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderStatus *pStatus)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pStatus);
    return;
}

NEXUS_Error
NEXUS_VideoEncoder_GetDelayRange (NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettings *pSettings, const NEXUS_VideoEncoderStartSettings *pStartSettings, NEXUS_VideoEncoderDelayRange *pDelayRange)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(pStartSettings);
    BSTD_UNUSED(pDelayRange);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);   
}

void 
NEXUS_VideoEncoder_GetSettingsOnInputChange(NEXUS_VideoEncoderHandle encoder, NEXUS_VideoEncoderSettingsOnInputChange *pSettings)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    return;
}

NEXUS_Error 
NEXUS_VideoEncoder_SetSettingsOnInputChange(NEXUS_VideoEncoderHandle encoder, const NEXUS_VideoEncoderSettingsOnInputChange *pSettings)
{
    BSTD_UNUSED(encoder);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);   
}

NEXUS_Error 
NEXUS_VideoEncoder_InsertRandomAccessPoint(NEXUS_VideoEncoderHandle encoder)
{
    BSTD_UNUSED(encoder);    
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);   
}

void 
NEXUS_VideoEncoder_ClearStatus(NEXUS_VideoEncoderHandle handle, const NEXUS_VideoEncoderClearStatus *pClearStatus)
{
    BSTD_UNUSED(encoder); 
    BSTD_UNUSED(pClearStatus); 
    
    return;
}

void 
NEXUS_VideoEncoder_GetDefaultClearStatus(NEXUS_VideoEncoderClearStatus *pClearStatus)
{
    BSTD_UNUSED(pClearStatus); 
    
    return;
}
#endif


