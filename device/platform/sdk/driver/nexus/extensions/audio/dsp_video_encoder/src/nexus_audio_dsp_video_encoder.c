/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_dsp_video_encoder.c $
* $brcm_Revision: 17 $
* $brcm_Date: 10/16/12 11:03a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/extensions/audio/dsp_video_encoder/src/nexus_audio_dsp_video_encoder.c $
* 
* 17   10/16/12 11:03a gmohile
* SW7231-1037 : Enable dblk and subpelMV for sub 720p res
* 
* 16   6/26/12 3:57p gmohile
* SW7231-792 : Disable EOS
* 
* 15   6/26/12 3:28p gmohile
* SW7231-792 : Merger 720p raaga encode support
* 
* 14   6/25/12 5:05p gmohile
* SW7231-873 : Add mp4 mux support
* 
* 13   4/2/12 10:54a gmohile
* SW7231-750 : Disable frame queueing during watchdog
* 
* 12   3/15/12 2:29p gmohile
* SW7231-584,SW7231-586: Disable frame drop in nexus for DSP video encode
* 
* 11   3/7/12 1:44p gmohile
* SW7231-584,SW7231-586 : Add go bit support
* 
* 10   2/13/12 4:24p gmohile
* SW7231-629 : Set end2end delay for raaga video encoder
* 
* 9   2/2/12 2:34p gmohile
* SW7231-587 : Pass original PTS to raaga encoder
* 
* 8   11/22/11 11:23a gmohile
* SW7425-1473 : Add stc channel to video encode
* 
* 7   11/17/11 3:33p gmohile
* SW7425-1473 : Update video encoder
* 
* 6   11/14/11 1:11p gmohile
* SW7425-1473 : Update raaga encode support
* 
* 5   11/11/11 3:10p gmohile
* SW7425-1473 : Add frame drop mechanism in nexus
* 
* 4   11/9/11 1:50p gmohile
* SW7425-1473 : Add picture queue mechanism
* 
* 3   11/1/11 11:01a gmohile
* SW7425-1473 : Add enqueue picture function
* 
* 2   10/28/11 4:54p gmohile
* SW7425-1473 : Update raaga encode support
* 
* 1   10/21/11 11:43a gmohile
* SW7425-1473 : Add dsp video encoder extension
* 
* 
***************************************************************************/

#include "nexus_audio_module.h"
#include "bchp_xpt_pcroffset.h"

BDBG_MODULE(nexus_audio_dsp_video_encoder);

BDBG_OBJECT_ID(NEXUS_DspVideoEncoder);


struct NEXUS_DspVideoEncoder {
    BDBG_OBJECT(NEXUS_DspVideoEncoder)
    bool opened;
    bool started;
    NEXUS_RaveStatus raveStatus;
    BVEE_ChannelHandle veeChannel;
    BVEE_ChannelStartSettings veeStartSettings;
    NEXUS_DspVideoEncoderStartSettings startSettings;  
    BVEE_ExtInterruptHandle veeIntHandle;
    bool watchdog;
};

static struct NEXUS_DspVideoEncoder g_encoders[NEXUS_NUM_DSP_VIDEO_ENCODERS];
static BKNI_EventHandle g_watchdogEvent;
static NEXUS_EventCallbackHandle g_watchdogCallback;

static void NEXUS_DspVideoEncoder_P_Watchdog_isr(void *pParam1, int param2);
static void NEXUS_DspVideoEncoder_P_Watchdog(void *pParam);

#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.transport)

#define STC_CONTEXT_STEP ( BCHP_XPT_PCROFFSET_STC1_CTRL - BCHP_XPT_PCROFFSET_STC0_CTRL )

uint32_t NEXUS_DspVideoEncoder_P_GetStcRegAddr(unsigned index)
{    
    return  BCHP_XPT_PCROFFSET_STC0_LO + (STC_CONTEXT_STEP * index);
}

void NEXUS_DspVideoEncoder_GetDefaultOpenSettings_priv( NEXUS_DspVideoEncoderOpenSettings *pSettings )
{
    NEXUS_ASSERT_MODULE();
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));  
    return;
}

NEXUS_DspVideoEncoderHandle NEXUS_DspVideoEncoder_Open_priv(unsigned index, const NEXUS_DspVideoEncoderOpenSettings *openSettings)
{
    NEXUS_DspVideoEncoderHandle  encoder;
    BVEE_ChannelOpenSettings channelOpenSettings; 
    NEXUS_Error rc;    

    NEXUS_ASSERT_MODULE();
    BSTD_UNUSED(openSettings);
    if(index>=NEXUS_NUM_DSP_VIDEO_ENCODERS) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_args;}

    encoder = &g_encoders[index];
    if ( encoder->opened )
    {
        BDBG_ERR(("DSP Video Encoder %u already opened", index));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_args;
    }

    BKNI_Memset(encoder, 0, sizeof(*encoder));
    BDBG_OBJECT_SET(encoder, NEXUS_DspVideoEncoder);
    encoder->opened = true;

    BVEE_Channel_GetDefaultOpenSettings(&channelOpenSettings);
    channelOpenSettings.maxQueuedPictures = 24;
    channelOpenSettings.resolution.width=1280;
    channelOpenSettings.resolution.height=720;   
    rc = BVEE_Channel_Open(g_NEXUS_veeHandle, index, &channelOpenSettings, &encoder->veeChannel);    
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_channel_open;}    

    rc = BVEE_Channel_AllocExtInterrupt(encoder->veeChannel, &encoder->veeIntHandle);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_int_alloc;} 

    return encoder;

err_int_alloc:
    BVEE_Channel_Close(encoder->veeChannel);  
err_channel_open:
    BKNI_Memset(encoder, 0, sizeof(*encoder));
err_args:
    return NULL;
}

void NEXUS_DspVideoEncoder_Close_priv(NEXUS_DspVideoEncoderHandle encoder)
{   
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(encoder, NEXUS_DspVideoEncoder);
    BVEE_Channel_FreeExtInterrupt(encoder->veeChannel, encoder->veeIntHandle);
    BVEE_Channel_Close(encoder->veeChannel);    
    BKNI_Memset(encoder, 0, sizeof(*encoder));  /* Also invalidates object ID */
    return;
}

NEXUS_Error NEXUS_DspVideoEncoder_GetExtInterruptInfo(NEXUS_DspVideoEncoderHandle encoder, NEXUS_DspVideoEncoderExtInterruptInfo *extIntInfo)
{
    BERR_Code rc=NEXUS_SUCCESS;
    BVEE_ExtInterruptInfo veeExtIntInfo;

    rc = BVEE_Channel_GetExtInterruptInfo(encoder->veeChannel, encoder->veeIntHandle, &veeExtIntInfo);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    
    extIntInfo->address = veeExtIntInfo.address;
    extIntInfo->bit_num = veeExtIntInfo.bit_num;
    
error:
    return rc;    
}

void NEXUS_DspVideoEncoder_GetDefaultStartSettings_priv( NEXUS_DspVideoEncoderStartSettings *pSettings )
{
    BVEE_ChannelStartSettings startSettings;

    NEXUS_ASSERT_MODULE(); 
         
    BVEE_Channel_GetDefaultStartSettings(&startSettings);
    
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    
    pSettings->nonRealTime = startSettings.nonRealTime;
    pSettings->codec = NEXUS_P_VideoCodec_FromMagnum(startSettings.codec);
    pSettings->framerate = NEXUS_P_FrameRate_FromMagnum(startSettings.frameRate);
    pSettings->width = startSettings.ui32EncodPicWidth;
    pSettings->height = startSettings.ui32EncodPicHeight;    

    return;
}

NEXUS_Error NEXUS_DspVideoEncoder_Start_priv(NEXUS_DspVideoEncoderHandle encoder, const NEXUS_DspVideoEncoderStartSettings *startSettings)
{
    BERR_Code rc;
    unsigned stcChannelIndex;


    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(encoder, NEXUS_DspVideoEncoder);

    if ( encoder->started )
    {
        BDBG_ERR(("Already running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BVEE_Channel_GetDefaultStartSettings(&encoder->veeStartSettings);
    encoder->veeStartSettings.nonRealTime = startSettings->nonRealTime;
    encoder->veeStartSettings.codec = NEXUS_P_VideoCodec_ToMagnum(startSettings->codec, NEXUS_TransportType_eEs);
    rc = NEXUS_P_FrameRate_ToMagnum(startSettings->framerate, &encoder->veeStartSettings.frameRate);
    encoder->veeStartSettings.ui32EncodPicWidth = startSettings->width;
    encoder->veeStartSettings.ui32EncodPicHeight = startSettings->height;
    encoder->veeStartSettings.ui32TargetBitRate = startSettings->bitrate;
    encoder->veeStartSettings.ui32End2EndDelay = startSettings->encoderDelay;
    encoder->veeStartSettings.pxlformat = BAVC_YCbCrType_e4_2_2; /* TODO : Do not hardcode */   
    
    if(startSettings->width < 1280 && startSettings->height < 720) {
	encoder->veeStartSettings.bDblkEnable = true;
	encoder->veeStartSettings.bSubPelMvEnable = true;
    }

    /* Picture Quality Settings */    
    encoder->veeStartSettings.ui32IntraPeriod = 30;
    encoder->veeStartSettings.ui32IDRPeriod = 30;
    encoder->veeStartSettings.bRateControlEnable = 1;

    /*External Interrupt Info */
    encoder->veeStartSettings.extIntCfg.enableInterrupts = true;
    encoder->veeStartSettings.extIntCfg.numInterrupts = 1;
    encoder->veeStartSettings.extIntCfg.interruptInfo[0].interruptRegAddr = startSettings->extIntInfo.address;
    encoder->veeStartSettings.extIntCfg.interruptInfo[0].interruptBit = startSettings->extIntInfo.bit_num;
   
    encoder->veeStartSettings.sendMetadata = true;

    if(startSettings->raveContext) {
        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetStatus_priv(startSettings->raveContext, &encoder->raveStatus);
        UNLOCK_TRANSPORT();
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto  err_rave_status;}
        encoder->veeStartSettings.pContextMap = &encoder->raveStatus.xptContextMap;
    } else {
	BDBG_WRN(("Encoder start requires Rave Context"));
	goto err_rave_status;
    }
    
    LOCK_TRANSPORT();
    NEXUS_StcChannel_GetIndex_priv(startSettings->stcChannel, &stcChannelIndex);
    UNLOCK_TRANSPORT();
    encoder->veeStartSettings.stcIndx = (int) stcChannelIndex;


    BDBG_MSG(("Starting VEE Channel %#x", encoder->veeChannel));
    rc = BVEE_Channel_Start(encoder->veeChannel, &encoder->veeStartSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_start;}
    encoder->startSettings = *startSettings;
    encoder->started = true;

    return NEXUS_SUCCESS;

err_start:
err_rave_status:
    return BERR_TRACE(rc);
}

void NEXUS_DspVideoEncoder_Stop_priv(NEXUS_DspVideoEncoderHandle encoder)
{ 

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(encoder, NEXUS_DspVideoEncoder);

    BDBG_MSG(("Stopping VEE Channel %#x", encoder->veeChannel));
    BVEE_Channel_Stop(encoder->veeChannel);
    encoder->started = false;   

    return;
}

NEXUS_Error NEXUS_DspVideoEncoder_Flush_priv(NEXUS_DspVideoEncoderHandle encoder)
{
    BERR_Code rc;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(encoder, NEXUS_DspVideoEncoder);

    if ( !encoder->started )
    {
        BDBG_ERR(("Encoder is not running.  Cannot flush."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }


    /* TODO: Do we need disable for flush/flush?  */
    BVEE_Channel_Stop(encoder->veeChannel);    

    if(encoder->startSettings.raveContext) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Disable_priv(encoder->startSettings.raveContext);
        NEXUS_Rave_Flush_priv(encoder->startSettings.raveContext);
        UNLOCK_TRANSPORT();
    }

    rc = BVEE_Channel_Start(encoder->veeChannel, &encoder->veeStartSettings);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    if(encoder->startSettings.raveContext) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Enable_priv(encoder->startSettings.raveContext);
        UNLOCK_TRANSPORT();
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_DspVideoEncoder_GetBuffer_priv(NEXUS_DspVideoEncoderHandle encoder, const NEXUS_DspVideoEncoderDescriptor **pBuffer, size_t *pSize, const NEXUS_DspVideoEncoderDescriptor **pBuffer2, size_t *pSize2)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_DspVideoEncoder);
    BDBG_ASSERT(pBuffer);
    BDBG_ASSERT(pSize);
    BDBG_ASSERT(pBuffer2);
    BDBG_ASSERT(pSize2);

    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, flags, BAVC_VideoBufferDescriptor, stCommon.uiFlags);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, originalPTS, BAVC_VideoBufferDescriptor, stCommon.uiOriginalPTS);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, pts, BAVC_VideoBufferDescriptor, stCommon.uiPTS);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, escr, BAVC_VideoBufferDescriptor, stCommon.uiESCR);

    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, ticksPerBit, BAVC_VideoBufferDescriptor, stCommon.uiTicksPerBit);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, shr, BAVC_VideoBufferDescriptor, stCommon.iSHR);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, offset, BAVC_VideoBufferDescriptor, stCommon.uiOffset);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, length, BAVC_VideoBufferDescriptor, stCommon.uiLength);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, dts, BAVC_VideoBufferDescriptor, uiDTS);
    NEXUS_ASSERT_FIELD(NEXUS_DspVideoEncoderDescriptor, dataUnitType, BAVC_VideoBufferDescriptor, uiDataUnitType);
    NEXUS_ASSERT_STRUCTURE(NEXUS_DspVideoEncoderDescriptor, BAVC_VideoBufferDescriptor);

    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_FLAG_ORIGINALPTS_VALID ==  BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID);
    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_FLAG_PTS_VALID == BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID);
    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_FLAG_FRAME_START ==  BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START);
    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_FLAG_EOS ==  BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS );
    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_FLAG_METADATA == BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA );
    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_VIDEOFLAG_DTS_VALID == BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID);
    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_VIDEOFLAG_RAP == BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP);
    BDBG_CASSERT(NEXUS_DSPVIDEOENCODERDESCRIPTOR_VIDEOFLAG_DATA_UNIT_START == BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START);

    rc = BVEE_Channel_GetBufferDescriptors(encoder->veeChannel, (void *)pBuffer, pSize, (void *)pBuffer2, pSize2);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

NEXUS_Error NEXUS_DspVideoEncoder_ReadComplete(NEXUS_DspVideoEncoderHandle encoder, unsigned descriptorsCompleted)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_DspVideoEncoder);

    rc = BVEE_Channel_ConsumeBufferDescriptors(encoder->veeChannel, descriptorsCompleted);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
}

void NEXUS_DspVideoEncoder_GetStatus(NEXUS_DspVideoEncoderHandle encoder, NEXUS_DspVideoEncoderStatus *pStatus)
{
    NEXUS_Error rc;
    BAVC_VideoBufferStatus bufferStatus;

    BDBG_OBJECT_ASSERT(encoder, NEXUS_DspVideoEncoder);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BVEE_Channel_GetBufferStatus(encoder->veeChannel, &bufferStatus);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    pStatus->bufferBase = bufferStatus.stCommon.pFrameBufferBaseAddress;
    pStatus->metadataBufferBase = bufferStatus.stCommon.pMetadataBufferBaseAddress;
    
error:
    return;
}

void NEXUS_DspVideoEncoder_GetRaveSettings_priv(NEXUS_RaveOpenSettings *raveSettings)
{
    NEXUS_ASSERT_MODULE();
    
    BKNI_Memset(raveSettings, 0, sizeof(*raveSettings));
    
    raveSettings->config.Cdb.Length = 256*1024;
    raveSettings->config.Cdb.Alignment = 4; 
    raveSettings->config.Itb.Length = 128*1024;
    raveSettings->config.Itb.Alignment = 4; 
#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
    raveSettings->config.Cdb.LittleEndian = true;
#else
    raveSettings->config.Cdb.LittleEndian = false;
#endif

    return;
}

NEXUS_Error NEXUS_DspVideoEncoder_EnqueuePicture_isr(NEXUS_DspVideoEncoderHandle encoder, NEXUS_DspVideoEncoderPicture *picture)
{    
    BVEE_PictureDescriptor descriptor;
    BERR_Code rc;       
    
    if(encoder->watchdog) {
	return NEXUS_NOT_AVAILABLE;
    }

    descriptor.pvaddress = picture->pvaddress;
    descriptor.width = picture->width;
    descriptor.height = picture->height;
    descriptor.polarity = picture->polarity;
    descriptor.originalPts.ui32CurrentPTS = picture->origPts;

#if 0
    descriptor.frameRate = encoder->veeStartSettings.frameRate;
#else 
    NEXUS_P_FrameRate_ToMagnum(picture->framerate, &descriptor.frameRate); /* No drop ins nexus. DSP firmware will handle frame drops */
#endif    

    NEXUS_StcChannel_GetStc_isr(encoder->startSettings.stcChannel, &descriptor.STC_Lo);

    BDBG_MSG(("Enqueue %#x", descriptor.pvaddress));

    rc = BVEE_Channel_EnqueuePicture_isr(encoder->veeChannel, &descriptor);
    
    return rc;
}

NEXUS_Error NEXUS_DspVideoEncoder_DequeuePicture_isr(NEXUS_DspVideoEncoderHandle encoder, NEXUS_DspVideoEncoderPicture *picture)
{    
    BVEE_PictureDescriptor descriptor;    
    
    if(encoder->watchdog) {
	picture->pvaddress = NULL;
	return NEXUS_NOT_AVAILABLE;
    }

    descriptor.pvaddress = BVEE_Channel_DequeuePicture_isr(encoder->veeChannel);
   
    BDBG_MSG(("Dequeue %#x", descriptor.pvaddress));   
    
    picture->pvaddress = descriptor.pvaddress;     
    
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_DspVideoEncoder_GetDelayRange(NEXUS_DspVideoEncoderHandle encoder, uint32_t width, uint32_t height, NEXUS_VideoFrameRate framerate, uint32_t bitrate, uint32_t *delay)
{
    BERR_Code rc;
    BVEE_Resolution resolution;
    BAVC_FrameRateCode veeframerate;

    BSTD_UNUSED(encoder);

    resolution.width = width;
    resolution.height = height;

    rc = NEXUS_P_FrameRate_ToMagnum(framerate, &veeframerate);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    rc = BVEE_GetA2PDelay(delay, resolution, veeframerate, bitrate);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    return rc;
}

NEXUS_Error NEXUS_DspVideoEncoder_P_InitWatchdog(void)
{
    BERR_Code errCode;
    BVEE_InterruptHandlers interrupts;

    errCode = BKNI_CreateEvent(&g_watchdogEvent);
    if ( errCode ) 
    {
        return BERR_TRACE(errCode);
    }
    g_watchdogCallback = NEXUS_RegisterEvent(g_watchdogEvent, NEXUS_DspVideoEncoder_P_Watchdog, NULL);
    if ( NULL == g_watchdogCallback ) 
    {
        BKNI_DestroyEvent(g_watchdogEvent);
        g_watchdogEvent = NULL;
        return BERR_TRACE(BERR_OS_ERROR);
    }

    /* Allow watchdog to be disabled for debugging */
    if ( !NEXUS_GetEnv("no_watchdog") )
    {
        BVEE_GetInterruptHandlers(g_NEXUS_veeHandle, &interrupts);
        interrupts.watchdog.pCallback_isr = NEXUS_DspVideoEncoder_P_Watchdog_isr;
        errCode = BVEE_SetInterruptHandlers(g_NEXUS_veeHandle, &interrupts);
        if ( errCode ) 
        {
            NEXUS_UnregisterEvent(g_watchdogCallback);
            g_watchdogCallback = NULL;
            BKNI_DestroyEvent(g_watchdogEvent);
            g_watchdogEvent = NULL;
            return BERR_TRACE(errCode);
        }
    }


    return BERR_SUCCESS;
}

void NEXUS_DspVideoEncoder_P_UninitWatchdog(void)
{
    BVEE_InterruptHandlers interrupts;

    BVEE_GetInterruptHandlers(g_NEXUS_veeHandle, &interrupts);
    interrupts.watchdog.pCallback_isr = NULL;
    BVEE_SetInterruptHandlers(g_NEXUS_veeHandle, &interrupts);

    NEXUS_UnregisterEvent(g_watchdogCallback);
    g_watchdogCallback = NULL;
    BKNI_DestroyEvent(g_watchdogEvent);
    g_watchdogEvent = NULL;
}

static void NEXUS_DspVideoEncoder_P_Watchdog_isr(void *pParam1, int param2)
{
    BSTD_UNUSED(pParam1);
    BSTD_UNUSED(param2);
    BDBG_ERR(("DSP Video Watchdog Interrupt Received"));
    BKNI_SetEvent_isr(g_watchdogEvent);
}

static void NEXUS_DspVideoEncoder_P_Watchdog(void *pParam)
{
    unsigned i;

    BSTD_UNUSED(pParam);

    LOCK_TRANSPORT();
    for ( i = 0; i < NEXUS_NUM_DSP_VIDEO_ENCODERS; i++ )
    {
	g_encoders[i].watchdog = true;

        if ( g_encoders[i].opened && g_encoders[i].started )
        {
            NEXUS_Rave_Disable_priv(g_encoders[i].startSettings.raveContext);
            NEXUS_Rave_Flush_priv(g_encoders[i].startSettings.raveContext);
        }
    }
    UNLOCK_TRANSPORT();

    /* Process watchdog event */
    BVEE_ProcessWatchdogInterrupt(g_NEXUS_veeHandle);

    /* Restart RAVE contexts */
    LOCK_TRANSPORT();
    for ( i = 0; i < NEXUS_NUM_DSP_VIDEO_ENCODERS; i++ )
    {
        if ( g_encoders[i].opened && g_encoders[i].started )
        {
            NEXUS_Rave_Enable_priv(g_encoders[i].startSettings.raveContext);
        }
	
	g_encoders[i].watchdog = false;
    }
    UNLOCK_TRANSPORT();    
}

