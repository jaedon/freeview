/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary decoder.software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  .  Except as set forth in an Authorized License, Broadcom grants
 *  no license , right to use, or waiver of any kind with respect to the
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
 *  LICENSORS BE LIABLE FOR  CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR  ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_decoder_xdm.c $
 * $brcm_Revision: 13 $
 * $brcm_Date: 10/15/12 3:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder_xdm.c $
 * 
 * 13   10/15/12 3:52p bandrews
 * SW7425-4021: move fifo wd counter out of data ready; distinguish fifo
 *  empty from fifo static
 * 
 * 12   9/7/12 11:56a erickson
 * SW7435-349: add standard header
 * 
 * 11   8/30/12 3:36p bandrews
 * SW7425-3820: fix function name in XDM-specific file
 * 
 * 10   8/27/12 3:37p bandrews
 * SW7420-2314: fix usage of sync substruct
 * 
 * 9   2/24/12 2:03p vsilyaev
 * SW7425-2258: Set default frame rate
 * 
 * 8   1/18/12 10:21a jgarrett
 * SW7425-2156: Adding power management support for DSP video decoder
 * 
 * 7   12/14/11 4:24p jgarrett
 * SW7425-1962: Adding SetSettings for DSP decoder and correcting
 *  stop/start behavior
 * 
 * 6   12/8/11 11:30a jgarrett
 * SW7425-409: Removing picture provider on shutdown
 * 
 * 5   10/12/11 10:21a erickson
 * SW7425-1264: add NEXUS_TsmMode_eSimulated
 * 
 * 4   6/20/11 5:41p jgarrett
 * SW7425-409: Merging VP6 support to main branch
 * 
 * SW7425-409/1   6/6/11 7:16p jgarrett
 * SW7425-409: Adding DSP video decoder extensions for VDE
 * 
 * 3   10/11/10 12:28p vsilyaev
 * SW7422-65, SW7468-295: Enable XDM callbacks
 * 
 * 2   9/9/10 11:19a vsilyaev
 * SW7468-129: Removed incorrect test for XVD decoder
 * 
 * 1   9/8/10 12:05p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/5   8/30/10 6:24p vsilyaev
 * SW7468-129: Added link with the pcrlibrary
 * 
 * SW7468-129/4   3/16/10 10:29a vsilyaev
 * SW7468-129: Added option to use vsync source from the XVD
 * 
 * SW7468-129/3   3/9/10 7:14p vsilyaev
 * SW7468-129: Added functional software video decoder
 * 
 * SW7468-129/2   3/8/10 6:33p vsilyaev
 * SW7468-129: Added basic implementation for video audio decoder
 * 
 * SW7468-129/1   3/5/10 7:31p vsilyaev
 * SW7468-129: Added hooks for soft video decoder
 * 
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_video_decoder_xdm);

BDBG_OBJECT_ID(NEXUS_VideoDecoder_P_Xdm);

static NEXUS_Error NEXUS_VideoDecoder_P_SetTsm_Xdm(NEXUS_VideoDecoderHandle videoDecoder)
{
    bool tsm;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&videoDecoder->xdm, NEXUS_VideoDecoder_P_Xdm);

    /* Lots of things can turn off TSM */
    tsm = videoDecoder->startSettings.stcChannel &&
          videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eEnabled &&
          !NEXUS_GetEnv("force_vsync") &&
          videoDecoder->transportType != NEXUS_TransportType_eEs;

#if NEXUS_HAS_ASTM
    if (videoDecoder->astm.settings.enableAstm)
    {
        BDBG_MSG(("ASTM is %s TSM for video channel %p", videoDecoder->astm.settings.enableTsm ? "enabling" : "disabling", videoDecoder));
        tsm = tsm && videoDecoder->astm.settings.enableTsm;
    }
#endif

#if NEXUS_CRC_CAPTURE
    tsm = false;
#endif

    videoDecoder->tsm = tsm;

    BKNI_EnterCriticalSection();
    rc = BXDM_PictureProvider_SetDisplayMode_isr(videoDecoder->xdm.pictureProvider, tsm ? BXDM_PictureProvider_DisplayMode_eTSM : BXDM_PictureProvider_DisplayMode_eVirtualTSM);
    BKNI_LeaveCriticalSection();
    return rc;
}

#if !NEXUS_VIDEO_DECODER_P_USE_XVD_DIH 
static void NEXUS_VideoDecoder_P_Xdm_Display_isr(void *decoder_, int polarity_)
{
    NEXUS_VideoDecoderHandle decoder = decoder_;
    BERR_Code rc;
    BAVC_Polarity polarity = polarity_;
    BXDM_DisplayInterruptInfo interruptInfo;
    BXDM_QualifiedValue stc;
    NEXUS_StcChannelHandle stcChannel;  

    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Display_isr>: %#x %d", (unsigned)decoder, polarity_));

    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);
    if(polarity!=BAVC_Polarity_eFrame && polarity==decoder->xdm.lastPolarity) {
        BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Display_isr<: %#x repeated polarity %u %u", (unsigned)decoder, polarity, decoder->xdm.lastPolarity));
        return;
    }

    decoder->xdm.lastPolarity = polarity;
    interruptInfo.eInterruptPolarity = polarity;
    interruptInfo.stInterruptCount.bValid = true;
    interruptInfo.stInterruptCount.uiValue = decoder->xdm.interruptCount;
    interruptInfo.uiSTCCount = 1;
    stc.bValid = true;
    stcChannel = decoder->startSettings.stcChannel;
    if(stcChannel) {
        stc.bValid = true;
        NEXUS_StcChannel_GetStc_isr(stcChannel, &stc.uiValue);
    } 
    interruptInfo.astSTC = &stc;

    decoder->xdm.interruptCount++;
    rc = BXDM_DisplayInterruptHandler_Callback_isr(decoder->xdm.displayInterrupt, &interruptInfo);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Display_isr<: %#x %d", (unsigned)decoder, polarity_));
    return;
}
#endif

static BERR_Code NEXUS_VideoDecoder_P_Xdm_Picture_isr(void *decoder_, int32_t unused, BAVC_MFD_Picture *pFieldData_)
{
    NEXUS_VideoDecoderHandle decoder = decoder_;
    const BAVC_MFD_Picture *pFieldData = pFieldData_;
    BAVC_MFD_Picture modifiedFieldData;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Picture_isr>: %#x", (unsigned)decoder));
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);
    BSTD_UNUSED(unused);

    pFieldData = NEXUS_VideoDecoder_P_DataReady_Generic_Prologue_isr(decoder, pFieldData, &modifiedFieldData);
    NEXUS_VideoDecoder_P_DataReady_Generic_Epilogue_isr(decoder, pFieldData, 0);

    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Picture_isr<: %#x", (unsigned)decoder));
    return BERR_SUCCESS;
}

static void NEXUS_VideoDecoder_P_Xdm_FirstPtsReady_isr(void *pContext, int32_t unused, const BXDM_PictureProvider_PTSInfo *pPTSInfo )
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BAVC_PTSInfo pts;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&videoDecoder->xdm, NEXUS_VideoDecoder_P_Xdm);
    BSTD_UNUSED(unused);

    pts.ui32CurrentPTS = pPTSInfo->ui32RunningPTS;
    pts.ePTSType = BAVC_PTSType_eCoded;
    BDBG_MSG(("video:%d Request STC %#x", (unsigned)videoDecoder->mfdIndex, pts.ui32CurrentPTS));

    if (videoDecoder->startSettings.stcChannel) {
       rc = NEXUS_StcChannel_RequestStc_isr(videoDecoder->startSettings.stcChannel, NEXUS_StcChannelDecoderType_eVideo0, &pts);
       if (rc) {rc=BERR_TRACE(rc);} /* keep going */
    }

    NEXUS_IsrCallback_Fire_isr(videoDecoder->firstPtsCallback);
    NEXUS_IsrCallback_Fire_isr(videoDecoder->playback.firstPtsCallback);
    return;
}

static void NEXUS_VideoDecoder_P_Xdm_PtsError_isr(void *pContext, int32_t unused, const BXDM_PictureProvider_PTSInfo *pPTSInfo )
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BAVC_PTSInfo pts;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&videoDecoder->xdm, NEXUS_VideoDecoder_P_Xdm);
    BSTD_UNUSED(unused);

    pts.ui32CurrentPTS = pPTSInfo->ui32RunningPTS;
    pts.ePTSType = BAVC_PTSType_eCoded;
    BDBG_MSG(("video:%d PTS error %#x", (unsigned)videoDecoder->mfdIndex, pts.ui32CurrentPTS));

    if (videoDecoder->trickState.stcTrickEnabled && videoDecoder->startSettings.stcChannel) {
        uint32_t stc;
        /* in STC trick mode, PTS might lag the STC because of decoder drop algorithm. don't reset STC in this case. */
        NEXUS_StcChannel_GetStc_isr(videoDecoder->startSettings.stcChannel, &stc);
        if (stc > pts.ui32CurrentPTS &&
            (NEXUS_IS_DSS_MODE(videoDecoder->transportType) ? (stc - pts.ui32CurrentPTS < 27000000 * 8) : (stc - pts.ui32CurrentPTS < 45000 * 8)) ) {
            return;
        }
    }

    if (videoDecoder->startSettings.stcChannel)
    {
        rc = NEXUS_StcChannel_PtsError_isr(videoDecoder->startSettings.stcChannel, NEXUS_StcChannelDecoderType_eVideo0, &pts);
        if (rc) {rc=BERR_TRACE(rc);} /* keep going */
    }

#if 0 && NEXUS_HAS_ASTM
    if (videoDecoder->astm.settings.enableAstm && videoDecoder->astm.settings.enableTsm)
    {
        videoDecoder->astm.status.pts = pts.ui32CurrentPTS;

        if (videoDecoder->astm.settings.tsmFail_isr)
        {
            videoDecoder->astm.settings.tsmFail_isr(videoDecoder->astm.settings.callbackContext, 0);
        }
    }
#endif

    videoDecoder->pts_error_cnt++;

    NEXUS_IsrCallback_Fire_isr(videoDecoder->ptsErrorCallback);
    return;
}

NEXUS_Error NEXUS_VideoDecoder_P_Xdm_Initialize(NEXUS_VideoDecoderHandle decoder, const BXDM_Decoder_Interface *decoderInterface, void *decoderContext)
{
    BERR_Code rc;
    BXDM_PictureProvider_Settings pictureProviderSettings;
    BXDM_DisplayInterruptHandler_AddPictureProviderInterface_Settings addPictureProviderSettings;
    BXDM_DisplayInterruptHandler_Handle displayInterrupt;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Initialize: %#x", (unsigned)decoder));
    BKNI_Memset(&decoder->xdm, 0, sizeof(decoder->xdm));

#if NEXUS_VIDEO_DECODER_P_USE_XVD_DIH 
    decoder->device = &g_NEXUS_videoDecoderXvdDevices[0]; 
    displayInterrupt = decoder->device->hXdmDih[BXVD_DisplayInterrupt_eOne];
#else
    rc = BXDM_DisplayInterruptHandler_Create(&decoder->xdm.displayInterrupt);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    displayInterrupt = decoder->xdm.displayInterrupt;
#endif

    BXDM_PictureProvider_GetDefaultSettings(&pictureProviderSettings);
    rc = BXDM_PictureProvider_Create(&decoder->xdm.pictureProvider, &pictureProviderSettings);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    rc = BXDM_DIH_AddPictureProviderInterface_GetDefaultSettings (&addPictureProviderSettings);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    rc = BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt(displayInterrupt, NEXUS_VideoDecoder_P_Xdm_Picture_isr, decoder, 0);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    rc = BXDM_DisplayInterruptHandler_AddPictureProviderInterface(displayInterrupt, BXDM_PictureProvider_GetPicture_isr, decoder->xdm.pictureProvider, &addPictureProviderSettings);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    BDBG_OBJECT_SET(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);

    BKNI_EnterCriticalSection();
    rc = BXDM_PictureProvider_SetDecoderInterface_isr(decoder->xdm.pictureProvider, (void *)decoderInterface, decoderContext);
    BXDM_PictureProvider_SetPictureDropMode_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_PictureDropMode_eFrame);
    {
        BXDM_PictureProvider_TSMThresholdSettings threshold;
        /* BXDM_PictureProvider_GetDefaultTSMThresholdSettings_isr(decoder->xdm.pictureProvider, &threshold); */
        threshold.uiTooEarlyThreshold = 0;
        threshold.uiTooLateThreshold = 0;
        threshold.uiDeltaStcPtsDiffThreshold = 10;
        BXDM_PictureProvider_SetTSMThresholdSettings_isr(decoder->xdm.pictureProvider, &threshold);
    }
    BXDM_PictureProvider_SetMonitorRefreshRate_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_MonitorRefreshRate_e59_94Hz);
    BXDM_PictureProvider_Set1080pScanMode_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_1080pScanMode_eDefault);
    BXDM_PictureProvider_SetFreeze_isr(decoder->xdm.pictureProvider, false);
    BXDM_PictureProvider_SetPTSOffset_isr(decoder->xdm.pictureProvider, 0);
    BXDM_PictureProvider_SetMuteMode_isr(decoder->xdm.pictureProvider, false);
    BXDM_PictureProvider_SetSTCValid_isr(decoder->xdm.pictureProvider, true);
    BXDM_PictureProvider_SetSTCIndex_isr(decoder->xdm.pictureProvider, 0);
    BXDM_PictureProvider_SetPlaybackMode_isr(decoder->xdm.pictureProvider, true);
    BXDM_PictureProvider_SetFrameRateDetectionMode_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_FrameRateDetectionMode_eStable);
    BXDM_PictureProvider_SetASTMMode_isr(decoder->xdm.pictureProvider, false);
    BXDM_PictureProvider_SetVirtualTSMOnPCRDiscontinuityMode_isr(decoder->xdm.pictureProvider, true);
    BXDM_PictureProvider_SetChannelSyncMode_isr(decoder->xdm.pictureProvider, true);
    BXDM_PictureProvider_SetErrorHandlingMode_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_ErrorHandlingMode_eOff);
    BXDM_PictureProvider_Callback_Install_FirstCodedPTSReady_isr(decoder->xdm.pictureProvider, NEXUS_VideoDecoder_P_Xdm_FirstPtsReady_isr, decoder, 0);
    BXDM_PictureProvider_Callback_Install_PTSError_isr(decoder->xdm.pictureProvider, NEXUS_VideoDecoder_P_Xdm_PtsError_isr, decoder, 0);


    BKNI_LeaveCriticalSection();
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    /* Force apply settings to picture provider */
    rc = NEXUS_VideoDecoder_P_Xdm_ApplySettings(decoder, &decoder->settings, true);
    if ( rc )
    {

    }

    NEXUS_VideoDecoder_P_SetTsm_Xdm(decoder);

    return NEXUS_SUCCESS;
}

void NEXUS_VideoDecoder_P_Xdm_Shutdown(NEXUS_VideoDecoderHandle decoder)
{
    BERR_Code rc;
    BXDM_DisplayInterruptHandler_Handle displayInterrupt;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Shutdown: %#x", (unsigned)decoder));
    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);

#if NEXUS_VIDEO_DECODER_P_USE_XVD_DIH 
    displayInterrupt = decoder->device->hXdmDih[BXVD_DisplayInterrupt_eOne];
#else
    displayInterrupt = decoder->xdm.displayInterrupt;
#endif

    rc = BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt(displayInterrupt);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}

    rc = BXDM_DisplayInterruptHandler_RemovePictureProviderInterface(displayInterrupt, BXDM_PictureProvider_GetPicture_isr, decoder->xdm.pictureProvider);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}

    rc = BXDM_PictureProvider_Destroy(decoder->xdm.pictureProvider);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}

#if !NEXUS_VIDEO_DECODER_P_USE_XVD_DIH
    rc = BXDM_DisplayInterruptHandler_Destroy(decoder->xdm.displayInterrupt);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
#endif

    BDBG_OBJECT_UNSET(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);

    return;
}

NEXUS_Error NEXUS_VideoDecoder_P_Xdm_Start(NEXUS_VideoDecoderHandle decoder)
{
    BERR_Code rc;
    BAVC_FrameRateCode frameRate;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Start: %#x", (unsigned)decoder));


    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);


    rc = NEXUS_P_FrameRate_ToMagnum(decoder->startSettings.frameRate, &frameRate);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc); }

    BKNI_EnterCriticalSection();
    BXDM_PictureProvider_Callback_SetEnable_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_Callback_eFirstCodedPTSReady, true);
    BXDM_PictureProvider_Callback_SetEnable_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_Callback_ePTSError, true);
    rc = BXDM_PictureProvider_StartDecode_isr(decoder->xdm.pictureProvider);
    rc = BERR_TRACE(rc);
    if(rc==BERR_SUCCESS) {
        BDBG_ERR(("FrameRate:%u", frameRate));
        rc = BXDM_PictureProvider_SetDefaultFrameRate_isr(decoder->xdm.pictureProvider, frameRate);
    }
    BKNI_LeaveCriticalSection();
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    rc = NEXUS_VideoDecoder_P_SetTsm_Xdm(decoder);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}

void NEXUS_VideoDecoder_P_Xdm_Stop(NEXUS_VideoDecoderHandle decoder)
{
    BERR_Code rc;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Xdm_Stop: %#x", (unsigned)decoder));
    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);

    BKNI_EnterCriticalSection();
    BXDM_PictureProvider_Callback_SetEnable_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_Callback_eFirstCodedPTSReady, false);
    BXDM_PictureProvider_Callback_SetEnable_isr(decoder->xdm.pictureProvider, BXDM_PictureProvider_Callback_ePTSError, false);
    rc = BXDM_PictureProvider_StopDecode_isr(decoder->xdm.pictureProvider);
    BKNI_LeaveCriticalSection();
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
}


NEXUS_Error NEXUS_VideoDecoder_SetDisplayConnection_priv_Xdm(NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoderDisplayConnection *connection)
{
    BERR_Code rc;

    BDBG_MSG(("NEXUS_VideoDecoder_SetDisplayConnection_priv_Xdm: %#x", (unsigned)decoder));
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);

    decoder->displayConnection = *connection;

    if (!connection->dataReadyCallback_isr) {
        if (!decoder->settings.manualPowerState) {
            rc = NEXUS_VideoDecoder_SetPowerState(decoder, false);
            if (rc) return BERR_TRACE(rc);
        }
    }
    else if (connection->dataReadyCallback_isr) {
        /* even in manualPowerState, it's ok to automatically power up here. */
        rc = NEXUS_VideoDecoder_SetPowerState(decoder, true);
        if (rc) return BERR_TRACE(rc);
    }

#if NEXUS_VIDEO_DECODER_P_USE_XVD_DIH
    rc = NEXUS_VideoDecoder_P_SetXvdDisplayInterrupt(decoder, connection, BXVD_DisplayInterrupt_eOne);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc); }
#else
    decoder->xdm.lastPolarity = BAVC_Polarity_eFrame;
    if(connection->top.intId) {
        rc = BINT_CreateCallback(&decoder->xdm.topInt, g_pCoreHandles->bint, connection->top.intId, NEXUS_VideoDecoder_P_Xdm_Display_isr, decoder, BAVC_Polarity_eTopField);
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        rc = BINT_EnableCallback(decoder->xdm.topInt);
        if (rc) { return BERR_TRACE(rc); }
    }
    if(connection->bottom.intId) {
        rc = BINT_CreateCallback(&decoder->xdm.bottomInt, g_pCoreHandles->bint, connection->bottom.intId, NEXUS_VideoDecoder_P_Xdm_Display_isr, decoder, BAVC_Polarity_eBotField);
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        rc = BINT_EnableCallback(decoder->xdm.bottomInt);
        if (rc) { return BERR_TRACE(rc); }
    }
    if(connection->frame.intId) {
        rc = BINT_CreateCallback(&decoder->xdm.frameInt, g_pCoreHandles->bint, connection->frame.intId, NEXUS_VideoDecoder_P_Xdm_Display_isr, decoder, BAVC_Polarity_eFrame);
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        rc = BINT_EnableCallback(decoder->xdm.frameInt);
        if (rc) { return BERR_TRACE(rc); }
    }
#endif
    return NEXUS_SUCCESS;
}

void NEXUS_VideoDecoder_UpdateDisplayInformation_priv_Xdm(NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoder_DisplayInformation *pDisplayInformation)
{
    BDBG_MSG(("NEXUS_VideoDecoder_UpdateDisplayInformation_priv_Xdm: %#x", (unsigned)decoder));
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);
    decoder->displayInformation = *pDisplayInformation;
    return;
}

NEXUS_Error NEXUS_VideoDecoder_P_GetStatus_Generic_Xdm( NEXUS_VideoDecoderHandle decoder, NEXUS_VideoDecoderStatus *pStatus)
{
    BERR_Code rc = BERR_SUCCESS;
    BXDM_PictureProvider_PTSInfo ptsInfo;
    BXDM_Picture_GopTimeCode timeCode;

    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->xdm, NEXUS_VideoDecoder_P_Xdm);

    if(decoder->xdm.pictureProvider==NULL) {
        return BERR_SUCCESS;
    }
    BKNI_EnterCriticalSection();
    rc = BXDM_PictureProvider_GetCurrentPTSInfo_isr(decoder->xdm.pictureProvider, &ptsInfo);
    BKNI_LeaveCriticalSection();
    if(rc==BERR_SUCCESS) {
        pStatus->pts = ptsInfo.ui32RunningPTS;
        pStatus->ptsType = (NEXUS_PtsType)ptsInfo.ePTSType;
        pStatus->numDecoded = ptsInfo.uiPicturesDecodedCount;
        pStatus->numDisplayed =  ptsInfo.uiPicturesDecodedCount - ptsInfo.uiDisplayManagerDroppedCount;
    }

    BKNI_EnterCriticalSection();
    rc = BXDM_PictureProvider_GetCurrentTimeCode_isr(decoder->xdm.pictureProvider, &timeCode);
    BKNI_LeaveCriticalSection();
    if(rc==BERR_SUCCESS && timeCode.bValid) {
       pStatus->timeCode.hours = timeCode.uiHours;
       pStatus->timeCode.minutes = timeCode.uiMinutes;
       pStatus->timeCode.seconds = timeCode.uiSeconds;
       pStatus->timeCode.pictures = timeCode.uiPictures;
    }

    return NEXUS_SUCCESS;
}

BERR_Code NEXUS_VideoDecoder_P_GetPtsCallback_Xdm_isr(void *pContext, BAVC_PTSInfo *pPTSInfo)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BXDM_PictureProvider_PTSInfo ptsInfo;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&videoDecoder->xdm, NEXUS_VideoDecoder_P_Xdm);

    rc = BXDM_PictureProvider_GetCurrentPTSInfo_isr(videoDecoder->xdm.pictureProvider, &ptsInfo);

    /* map data structures */
    pPTSInfo->ui32CurrentPTS = ptsInfo.ui32RunningPTS;
    pPTSInfo->ePTSType = ptsInfo.ePTSType;
    return rc;
}


BERR_Code NEXUS_VideoDecoder_P_StcValidCallback_Xdm_isr(void *pContext)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&videoDecoder->xdm, NEXUS_VideoDecoder_P_Xdm);
    return BXDM_PictureProvider_SetSTCValid_isr(videoDecoder->xdm.pictureProvider, true);
}

NEXUS_Error NEXUS_VideoDecoder_P_Xdm_ApplySettings(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderSettings *pSettings, bool force)
{
    BERR_Code rc = 0;
    bool setMute = false;
    bool setFreeze = false;
    bool setUserdata = false;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&videoDecoder->xdm, NEXUS_VideoDecoder_P_Xdm);
    /* Apply settings for any XDM decoder */

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->xdm.pictureProvider) {
        goto skip;
    }

    /* convert to single XDM calls. this requires that Nexus init in the same configuration as XVD, or that it asserts its configuration elsewhere */
    if (force || pSettings->freeze != videoDecoder->settings.freeze) {
        setFreeze = true; /* defer to bottom */
    }
    if (pSettings->mute != videoDecoder->settings.mute) {
        setMute = true; /* defer to bottom */
    }
    if (force || pSettings->channelChangeMode != videoDecoder->settings.channelChangeMode) {
        rc = NEXUS_VideoDecoder_P_Xdm_SetChannelChangeMode(videoDecoder, pSettings->channelChangeMode);
        if (rc) return BERR_TRACE(rc);
    }
    if (pSettings->userDataEnabled != videoDecoder->settings.userDataEnabled) {
        setUserdata = true;
    }
    if (force || pSettings->dropFieldMode != videoDecoder->settings.dropFieldMode) {  
        BKNI_EnterCriticalSection();     
        rc = BXDM_PictureProvider_SetPictureDropMode_isr(videoDecoder->xdm.pictureProvider, pSettings->dropFieldMode ? BXDM_PictureProvider_PictureDropMode_eField : BXDM_PictureProvider_PictureDropMode_eFrame);
        BKNI_LeaveCriticalSection();
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->stillContentInterpolationMode != videoDecoder->settings.stillContentInterpolationMode) {
        BXDM_PictureProvider_SourceFormatOverride formatOverride;
        switch ( pSettings->stillContentInterpolationMode )
        {
        case NEXUS_StillContentInterpolationMode_eSingleField:
          formatOverride = BXDM_PictureProvider_SourceFormatOverride_eInterlaced;
          break;
        case NEXUS_StillContentInterpolationMode_eBothField:
        case NEXUS_StillContentInterpolationMode_eFrame:
          formatOverride = BXDM_PictureProvider_SourceFormatOverride_eProgressive;
        break;
        case NEXUS_StillContentInterpolationMode_eDefault:
          formatOverride = BXDM_PictureProvider_SourceFormatOverride_eDefault;
          break;
        default:
          return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        BKNI_EnterCriticalSection();     
        rc = BXDM_PictureProvider_SetSourceFormatOverride_isr(videoDecoder->xdm.pictureProvider, formatOverride);
        BKNI_LeaveCriticalSection();
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->movingContentInterpolationMode != videoDecoder->settings.movingContentInterpolationMode) {
        BDBG_CWARNING(NEXUS_MovingContentInterpolationMode_eMax == (NEXUS_MovingContentInterpolationMode)BXDM_PictureProvider_ScanModeOverride_eMax);
        BKNI_EnterCriticalSection();     
        rc = BXDM_PictureProvider_SetScanModeOverride_isr(videoDecoder->xdm.pictureProvider, pSettings->movingContentInterpolationMode);
        BKNI_LeaveCriticalSection();
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->scanMode != videoDecoder->settings.scanMode) {
        BDBG_CWARNING(NEXUS_VideoDecoderScanMode_eMax == (NEXUS_VideoDecoderScanMode)BXDM_PictureProvider_1080pScanMode_eMax);
        BKNI_EnterCriticalSection();     
        rc = BXDM_PictureProvider_Set1080pScanMode_isr(videoDecoder->xdm.pictureProvider, pSettings->scanMode);
        BKNI_LeaveCriticalSection();
        if (rc) return BERR_TRACE(rc);
    }
    if(force || pSettings->sourceOrientation != videoDecoder->settings.sourceOrientation || pSettings->customSourceOrientation != videoDecoder->settings.customSourceOrientation) {
        BXDM_PictureProvider_3DSettings setting3d;
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e2D == (NEXUS_VideoDecoderSourceOrientation)BXDM_PictureProvider_Orientation_e2D);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight == (NEXUS_VideoDecoderSourceOrientation)BXDM_PictureProvider_Orientation_eLeftRight);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_OverUnder == (NEXUS_VideoDecoderSourceOrientation)BXDM_PictureProvider_Orientation_eOverUnder);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame == (NEXUS_VideoDecoderSourceOrientation)BXDM_PictureProvider_Orientation_eLeftRightFullFrame);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_RightLeftFullFrame == (NEXUS_VideoDecoderSourceOrientation)BXDM_PictureProvider_Orientation_eRightLeftFullFrame);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightEnhancedResolution == (NEXUS_VideoDecoderSourceOrientation)BXDM_PictureProvider_Orientation_eLeftRightEnhancedResolution);
        BDBG_CWARNING(NEXUS_VideoDecoderSourceOrientation_eMax == (NEXUS_VideoDecoderSourceOrientation)BXDM_PictureProvider_Orientation_eMax);
        BKNI_EnterCriticalSection();
        BXDM_PictureProvider_Get3D_isr(videoDecoder->xdm.pictureProvider, &setting3d);
        setting3d.bOverrideOrientation = pSettings->customSourceOrientation;
        setting3d.eOrientation = pSettings->sourceOrientation;
        setting3d.bSetNextPointer = false;
        rc = BXDM_PictureProvider_Set3D_isr(videoDecoder->xdm.pictureProvider, &setting3d);
        BKNI_LeaveCriticalSection();
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->horizontalOverscanMode != videoDecoder->settings.horizontalOverscanMode) {
        BDBG_CWARNING(NEXUS_VideoDecoderHorizontalOverscanMode_eMax == (NEXUS_VideoDecoderHorizontalOverscanMode)BXDM_PictureProvider_HorizontalOverscanMode_eMax);
        BKNI_EnterCriticalSection();
        rc = BXDM_PictureProvider_SetHorizontalOverscanMode_isr(videoDecoder->xdm.pictureProvider, pSettings->horizontalOverscanMode);
        BKNI_LeaveCriticalSection();
        if (rc) return BERR_TRACE(rc);
    }
skip:
    NEXUS_IsrCallback_Set(videoDecoder->userdataCallback, &pSettings->appUserDataReady);
    NEXUS_IsrCallback_Set(videoDecoder->sourceChangedCallback, &pSettings->sourceChanged);
    NEXUS_IsrCallback_Set(videoDecoder->streamChangedCallback, &pSettings->streamChanged);
    NEXUS_IsrCallback_Set(videoDecoder->ptsErrorCallback, &pSettings->ptsError);
    NEXUS_IsrCallback_Set(videoDecoder->firstPtsCallback, &pSettings->firstPts);
    NEXUS_IsrCallback_Set(videoDecoder->firstPtsPassedCallback, &pSettings->firstPtsPassed);
    NEXUS_IsrCallback_Set(videoDecoder->fifoEmpty.callback, &pSettings->fifoEmpty);
    NEXUS_IsrCallback_Set(videoDecoder->afdChangedCallback, &pSettings->afdChanged);
    NEXUS_IsrCallback_Set(videoDecoder->decodeErrorCallback, &pSettings->decodeError);

    if ( pSettings->fifoThreshold != videoDecoder->settings.fifoThreshold )
    {
        LOCK_TRANSPORT();

        rc = NEXUS_Rave_SetCdbThreshold_priv(videoDecoder->rave, pSettings->fifoThreshold);

        UNLOCK_TRANSPORT();

        if (rc) return BERR_TRACE(rc);
    }

    videoDecoder->settings = *pSettings;

    if (setMute) {
        /* this setting is shared with SyncChannel, and depends on ch chg mode */
        rc = NEXUS_VideoDecoder_P_Xdm_SetMute(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }
    if (setFreeze) {
        /* this setting is shared with SyncChannel, and depends on ch chg mode */
        rc = NEXUS_VideoDecoder_P_Xdm_SetFreeze(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }
#if 0
    if (setUserdata) {
        /* this setting is shared with VideoInput's priv interface */
        rc = NEXUS_VideoDecoder_P_SetUserdata(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }
#endif

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetChannelChangeMode(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoder_ChannelChangeMode channelChangeMode)
{
    BERR_Code rc;
    BXDM_PictureProvider_ChannelChangeSettings channelChangeSettings;

    BKNI_EnterCriticalSection();
    rc = BXDM_PictureProvider_GetChannelChangeSettings_isr(
             videoDecoder->xdm.pictureProvider,
             &channelChangeSettings);

    if ( BERR_SUCCESS == rc )
    {
       switch ( channelChangeMode )
       {
          case NEXUS_VideoDecoder_ChannelChangeMode_eMute:
             channelChangeSettings.bHoldLastPicture = false;
             channelChangeSettings.bFirstPicturePreview = false;
             break;

          case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock:
             channelChangeSettings.bHoldLastPicture = true;
             channelChangeSettings.bFirstPicturePreview = false;
             break;

          case NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture:
             channelChangeSettings.bHoldLastPicture = false;
             channelChangeSettings.bFirstPicturePreview = true;
             break;

          case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture:
             channelChangeSettings.bHoldLastPicture = true;
             channelChangeSettings.bFirstPicturePreview = true;
             break;

          default:
             return BERR_TRACE(BERR_INVALID_PARAMETER);
       }

       rc = BXDM_PictureProvider_SetChannelChangeSettings_isr(videoDecoder->xdm.pictureProvider, &channelChangeSettings);
    }
    BKNI_LeaveCriticalSection();
    if (rc) return BERR_TRACE(rc);
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetMute(NEXUS_VideoDecoderHandle videoDecoder)
{
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->xdm.pictureProvider) {
        return 0;
    }
    
    /* user mute is highest precedence */
    if (videoDecoder->settings.mute)
    {
        BDBG_MSG(("Muting video per user request"));
        BKNI_EnterCriticalSection();
        rc = BXDM_PictureProvider_SetMuteMode_isr(videoDecoder->xdm.pictureProvider, videoDecoder->settings.mute);
        BKNI_LeaveCriticalSection();
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
#if NEXUS_HAS_SYNC_CHANNEL
    else if (videoDecoder->sync.mute 
        /* if preroll is nonzero, we want to unmute now */
        && !(videoDecoder->started && videoDecoder->startSettings.prerollRate))
    {
        /* SW7420-2314: if decoder was already started, don't mute */
        if (!videoDecoder->started)
        {
            NEXUS_VideoDecoderTrickState normal;
            
            /* unmute if trick mode and sync mute activated */
            NEXUS_VideoDecoder_GetNormalPlay(&normal);
            if (BKNI_Memcmp(&normal, &videoDecoder->trickState, sizeof(NEXUS_VideoDecoderTrickState)))
            {
                /* this overrides mute control of sync */
                videoDecoder->sync.mute = false;
                BKNI_EnterCriticalSection();
                rc = BXDM_PictureProvider_SetMuteMode_isr(videoDecoder->xdm.pictureProvider, false);
                BKNI_LeaveCriticalSection();
                if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
            }
            else
            {
                /* only sync mute if ch chg mode says so */
                switch (videoDecoder->settings.channelChangeMode)
                {
                    case NEXUS_VideoDecoder_ChannelChangeMode_eMute:
                        BDBG_MSG(("Muting video per sync request"));
                        BKNI_EnterCriticalSection();
                        rc = BXDM_PictureProvider_SetMuteMode_isr(videoDecoder->xdm.pictureProvider, true);
                        BKNI_LeaveCriticalSection();
                        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
                        break;
                    default:
                        break;
                }
            }
        }
    }
#endif
    else /* unmute */
    {
        BDBG_MSG(("Unmuting video"));
        BKNI_EnterCriticalSection();
        rc = BXDM_PictureProvider_SetMuteMode_isr(videoDecoder->xdm.pictureProvider, false);
        BKNI_LeaveCriticalSection();
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }

    return rc;    
}

NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetFreeze(NEXUS_VideoDecoderHandle videoDecoder)
{
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->xdm.pictureProvider) {
        return 0;
    }

    /* user freeze is highest precedence */
    if (videoDecoder->settings.freeze)
    {
        BDBG_MSG(("Freezing video per user request"));
        BKNI_EnterCriticalSection();
        rc = BXDM_PictureProvider_SetFreeze_isr(videoDecoder->xdm.pictureProvider, true);
        BKNI_LeaveCriticalSection();
        if (rc) return BERR_TRACE(rc);
    }
/* 20100326 bandrews - freeze doesn't do normal TSM in the bg while displaying the frozen frame, so can't use it */
#if 0
    else if (videoDecoder->syncSettings.mute)
    {
        /* only sync freeze if ch chg mode says so */
        switch (videoDecoder->settings.channelChangeMode)
        {
            case NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture:
            case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture:
                /* the above two modes are here because the user expects a freeze after the first
                picture is shown anyway (while TSM for the second picture matures).  XVD team says
                if both FREEZE and "first picture preview" are enabled, "first picture preview" will
                take precendence...and then freeze will occur on the first picture */
            case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock:
                BDBG_MSG(("Freezing video per sync request"));
                BKNI_EnterCriticalSection();
                rc = BXDM_PictureProvider_SetFreeze_isr(videoDecoder->xdm.pictureProvider, true);
                BKNI_LeaveCriticalSection();
                if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
                break;
            default:
                break;
        }
    }
#endif
    else /* unfreeze */
    {
        BDBG_MSG(("Unfreezing video"));
        BKNI_EnterCriticalSection();
        rc = BXDM_PictureProvider_SetFreeze_isr(videoDecoder->xdm.pictureProvider, false);
        BKNI_LeaveCriticalSection();
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }

    return rc;
}

#if NEXUS_HAS_SYNC_CHANNEL
NEXUS_Error NEXUS_VideoDecoder_SetSyncSettings_priv_Xdm(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BERR_Code rc;
    unsigned delayCallbackThreshold;
    bool oldMuteStatus;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();

    oldMuteStatus = videoDecoder->sync.settings.mute;
    
    videoDecoder->sync.settings = *pSyncSettings;

    delayCallbackThreshold = videoDecoder->sync.settings.delayCallbackThreshold;

    /* PR:48453 bandrews - convert 45 KHz to appropriate decoder units */
    switch (videoDecoder->transportType)
    {
        case NEXUS_TransportType_eDssEs:
        case NEXUS_TransportType_eDssPes:
            delayCallbackThreshold *= 600;
            break;
        default:
            break;
    }

    if (videoDecoder->dec) 
    {
        BXDM_PictureProvider_TSMThresholdSettings thresholdSettings;
        BKNI_EnterCriticalSection();
        rc = BXDM_PictureProvider_GetTSMThresholdSettings_isr(videoDecoder->xdm.pictureProvider, &thresholdSettings);
        if ( BERR_SUCCESS == rc )
        {
            thresholdSettings.uiDeltaStcPtsDiffThreshold = delayCallbackThreshold;
            rc = BXDM_PictureProvider_SetTSMThresholdSettings_isr(videoDecoder->xdm.pictureProvider, &thresholdSettings);
        }
        BKNI_LeaveCriticalSection();
        if (rc) {return BERR_TRACE(rc);}
    }

    /* the following settings are shared with the user */

    rc = NEXUS_VideoDecoder_P_Xdm_SetPtsOffset(videoDecoder);
    if (rc) {return BERR_TRACE(rc);}

    if (!videoDecoder->started)
    {
        /* if sync sets mute before video is started, it is to modify start behavior */
        videoDecoder->sync.startMuted = videoDecoder->sync.settings.mute;
    }

    videoDecoder->sync.mute = videoDecoder->sync.settings.mute;
    
    if ((oldMuteStatus == true) && (videoDecoder->sync.mute == false)) 
    {
        /* apply unmute edge */
        
        rc = NEXUS_VideoDecoder_P_SetMute(videoDecoder);
        if (rc) {return BERR_TRACE(rc);}

#if B_HAS_TRC
        /* don't count sync unmute if it occurs before first vpts passed */
        if (videoDecoder->firstPtsPassed)
        {
            BTRC_TRACE(ChnChange_SyncUnmuteVideo, STOP);
        }
#endif
    }

    return 0;
}
#endif

NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetPtsOffset(NEXUS_VideoDecoderHandle videoDecoder)
{
    unsigned syncPtsOffset;
    unsigned astmPtsOffset;
    BERR_Code rc;

    astmPtsOffset = 0;
    syncPtsOffset = 0;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->xdm.pictureProvider) {
        return 0;
    }

#if NEXUS_HAS_SYNC_CHANNEL
    syncPtsOffset = videoDecoder->sync.settings.delay;
#endif
#if NEXUS_HAS_ASTM
    astmPtsOffset = videoDecoder->astm.settings.ptsOffset;
#endif
    switch (videoDecoder->transportType)
    {
        case NEXUS_TransportType_eDssEs:
        case NEXUS_TransportType_eDssPes:
            syncPtsOffset *= 600;
            astmPtsOffset *= 600;
            break;
        default:
            break;
    }

    BKNI_EnterCriticalSection();
    rc = BXDM_PictureProvider_SetPTSOffset_isr(videoDecoder->xdm.pictureProvider, syncPtsOffset + astmPtsOffset +
        videoDecoder->settings.ptsOffset + videoDecoder->primerPtsOffset + videoDecoder->additionalPtsOffset);
    BKNI_LeaveCriticalSection();
    if ( rc ) { return BERR_TRACE(rc); }

    return BERR_SUCCESS;
}

