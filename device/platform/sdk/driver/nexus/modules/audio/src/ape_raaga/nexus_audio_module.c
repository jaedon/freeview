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
* $brcm_Workfile: nexus_audio_module.c $
* $brcm_Revision: 37 $
* $brcm_Date: 10/12/12 3:12p $
*
* API Description:
*   API name: Audio Module
*    Top-Level Audio Module APIs
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_module.c $
* 
* 37   10/12/12 3:12p erickson
* SW7435-419: change proc to BDBG_LOG to that they are still available
*  with B_REFSW_DEBUG_LEVEL=err
* 
* 36   9/27/12 1:13p erickson
* SW7435-362: lock module during init and uninit
* 
* 35   7/3/12 6:03p mphillip
* SW7125-1296: Add audio decoder status to proc debug output
* 
* 34   5/23/12 6:13p jgarrett
* SW7425-3070: Adding NEXUS_AudioOutputPll Settings to control VCXO
*  source
* 
* 33   5/4/12 11:05a jgarrett
* SW7425-2075: Merge to main branch
* 
* 32   5/3/12 5:05p jgarrett
* SW7425-2075: Temporarily removing hbr flag from BAPE_Settings
* 
* SW7425-2075/2   5/3/12 3:45p jgarrett
* SW7425-2075: Merging latest mainline updates
* 
* 31   5/2/12 9:28a jgarrett
* SW7425-2802: Allowing capabilities without NEXUS_NUM_XXXX
* 
* 30   5/1/12 6:22p jgarrett
* SW7425-2690: Adding Audio Capabilities routine
* 
* SW7425-2075/1   4/6/12 12:43p jgarrett
* SW7425-2075: Adding support for 4x/16x compressed audio
* 
* 29   3/5/12 2:45p gmohile
* SW7231-503 : Fix audio standby err code
* 
* 28   2/17/12 6:04p jgarrett
* SW7358-214: Changing heap handle to index
* 
* 27   2/8/12 12:55p gmohile
* SW7425-1473 : Fix error handling
* 
* 26   1/16/12 6:02p gmohile
* SW7425-1473 : Merge raaga encoder support
* 
* 25   12/20/11 4:13p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* 24   12/14/11 4:52p jgarrett
* SW7425-1950: Consolidating audio codec conversion routines into core
*  module
* 
* 23   11/21/11 2:48p jgarrett
* SW7425-1406: Adding Vorbis audio support
* 
* VORBIS_DEVEL/2   11/16/11 1:51p jgarrett
* SW7425-1406: Merging latest main branch changes
* 
* 22   11/3/11 6:53p jgarrett
* SW7422-400: Enabling audio debug logs on 40nm APE chips
* 
* 21   9/27/11 4:36p jgarrett
* SW7408-304: Destroying IMG context on shutdown
*
* 20   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
*
* Nexus_APE_Integration/2   8/10/11 5:17p jgarrett
* SWDTV-6306: Enabling implicit capture to DSP instead of FMM by default
*  for DTV
*
* Nexus_APE_Integration/1   8/10/11 5:16p jgarrett
* SWDTV-6306: Adding implicit capture to DSP instead of FMM for DTV
*
* 19   8/4/11 7:33p mphillip
* SW7422-404: Update audio region verification code
*
* 18   7/21/11 4:13p gmohile
* SW7125-1014 : Rework power management
*
* SW7125-1014/1   7/20/11 4:37p gmohile
* SW7125-1014 : Rework power management
*
* 17   7/1/11 4:03p jgarrett
* SW7231-274: Adding optional audio firmware heap
*
* 16   7/1/11 9:59a jgarrett
* SW7405-5072: Adding WMA TS Support
*
* 15   6/27/11 5:42p jgarrett
* SW7231-97: Enabling DtsLegacy
*
* 14   6/24/11 11:38a gmohile
* SW7231-128 : Add BDSP standby
*
* 13   6/20/11 5:42p jgarrett
* SW7425-409: Merging VP6 support to main branch
*
* SW7425-409/2   6/8/11 6:58p jgarrett
* SW7425-409: Fixing linker issue if DSP video decoder extension is not
*  defined
*
* SW7425-409/1   6/6/11 7:23p jgarrett
* SW7425-409: Adding init hooks for VDE
*
* 12   5/13/11 5:17p gmohile
* SW7231-128 : Add Standby support
*
* 11   4/19/11 6:07p jgarrett
* SW7425-386: Enabling control of audio memory allocation
*
* 10   4/19/11 5:32p jgarrett
* SW7422-408: Adding additional codecs
*
* 9   4/18/11 10:14p gskerl
* SW7425-364: Added BAPE_Pll_EnableExternalMclk() API to APE, then called
*  it from NEXUS_AudioModule_EnableExternalMclk()
*
* 8   3/23/11 6:28p jgarrett
* SW7422-352: adding HDMI input support to nexus
*
* 7   2/22/11 5:30p jgarrett
* SW7422-146: Fixing build errors
*
* 6   1/19/11 3:01p jgarrett
* SW7422-146: Initial decode/passthrough of ac3
*
* 5   1/13/11 5:25p jgarrett
* SW7422-146: Adding encoder hooks
*
* 4   1/10/11 5:21p jgarrett
* SW7422-146: Fixing module comments
*
* 3   1/10/11 5:19p jgarrett
* SW7422-146: Adding ramp step APIs
*
* 2   1/10/11 4:20p jgarrett
* SW7422-146: Adding missing module-level routines
*
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
*
***************************************************************************/
#include "nexus_audio_module.h"
#define RAAGA_DEBUG_LOG_CHANGES 1
#include "bdsp_raaga.h"
#ifdef NEXUS_SECURITY_AUDIO_VERIFICATION
#include "nexus_audio_decoder_security.h"
#endif

BDBG_MODULE(nexus_audio_module);

NEXUS_ModuleHandle g_NEXUS_audioModule;
NEXUS_AudioModuleData g_NEXUS_audioModuleData;

/* required for the debug output */
extern NEXUS_AudioDecoder g_decoders[NEXUS_NUM_AUDIO_DECODERS];

static void NEXUS_AudioModule_Print(void)
{
    int i;
    BDBG_LOG(("Audio:"));

    BDBG_LOG((" handles: ape:%p dsp:%p",g_NEXUS_audioModuleData.apeHandle,g_NEXUS_audioModuleData.dspHandle));
    BDBG_LOG((" img ctxt:%p",g_NEXUS_audioModuleData.pImageContext));
    BDBG_LOG((" settings: wd:%d id:%d",g_NEXUS_audioModuleData.settings.watchdogEnabled,g_NEXUS_audioModuleData.settings.independentDelay));

    for (i=0; i < NEXUS_NUM_AUDIO_DECODERS; i++) {
        NEXUS_AudioDecoderStatus status;
        NEXUS_AudioDecoderHandle handle = &g_decoders[i];
        if (handle->opened) {
            NEXUS_AudioDecoder_GetStatus(handle, &status);
            BDBG_LOG((" channel%d: (%p) %s", i, handle->channel, status.locked ? "locked " : ""));
            BDBG_LOG(("  started=%c, codec=%d", status.started ? 'y' : 'n',
                status.started ? status.codec : 0));
            BDBG_LOG(("  fifo: %d/%d (%d%%), queued: %d", status.fifoDepth, status.fifoSize, status.fifoSize ? status.fifoDepth*100/status.fifoSize : 0, status.queuedFrames));
            BDBG_LOG(("  TSM: %s pts=%#x pts_stc_diff=%d errors=%d", status.tsm ? "enabled" : "disabled", status.pts, status.ptsStcDifference, status.ptsErrorCount));
            BDBG_LOG    (("  watchdogs: %d", status.numWatchdogs));
        }
    }
}

void NEXUS_AudioModule_GetDefaultSettings(
    NEXUS_AudioModuleSettings *pSettings    /* [out] */
    )
{
    BAPE_Settings apeSettings;
    BDSP_RaagaSettings raagaSettings;
    unsigned i;

    BKNI_Memset(pSettings, 0, sizeof(NEXUS_AudioModuleSettings));
    BAPE_GetDefaultSettings(&apeSettings);
    pSettings->watchdogEnabled = true;
    pSettings->maxAudioDspTasks = apeSettings.maxDspTasks;
    pSettings->maxIndependentDelay = apeSettings.maxIndependentDelay;
    pSettings->maxPcmSampleRate = apeSettings.maxPcmSampleRate;
    pSettings->numPcmBuffers = apeSettings.numPcmBuffers;
    pSettings->numCompressedBuffers = apeSettings.numCompressedBuffers;
    pSettings->numCompressed4xBuffers = apeSettings.numCompressed4xBuffers;
    pSettings->numCompressed16xBuffers = apeSettings.numCompressed16xBuffers;
    pSettings->numRfEncodedPcmBuffers = apeSettings.numRfEncodedPcmBuffers;
    pSettings->independentDelay = pSettings->maxIndependentDelay > 0 ? true : false;
    BDBG_CASSERT(NEXUS_AudioLoudnessEquivalenceMode_eNone == (int)BAPE_LoudnessEquivalenceMode_eNone);
    BDBG_CASSERT(NEXUS_AudioLoudnessEquivalenceMode_eAtscA85 == (int)BAPE_LoudnessEquivalenceMode_eAtscA85);
    BDBG_CASSERT(NEXUS_AudioLoudnessEquivalenceMode_eEbuR128 == (int)BAPE_LoudnessEquivalenceMode_eEbuR128);
    BDBG_CASSERT(NEXUS_AudioLoudnessEquivalenceMode_eMax == (int)BAPE_LoudnessEquivalenceMode_eMax);
    pSettings->loudnessMode = (NEXUS_AudioLoudnessEquivalenceMode)apeSettings.loudnessMode;
    #if NEXUS_DTV_PLATFORM
    pSettings->routeInputsToDsp = true;
    #endif

    BDSP_Raaga_GetDefaultSettings(&raagaSettings);
    for ( i = 0; i < NEXUS_AudioDspDebugType_eMax; i++ )
    {
        pSettings->dspDebugSettings.typeSettings[i].enabled = false;    /* Disable all debug by default */
        pSettings->dspDebugSettings.typeSettings[i].bufferSize = raagaSettings.debugSettings[i].bufferSize;
    }
}

NEXUS_ModuleHandle NEXUS_AudioModule_Init(
    const NEXUS_AudioModuleSettings *pSettings  /* NULL will use default settings */
    )
{
    BERR_Code errCode;
    NEXUS_ModuleSettings moduleSettings;
    BAPE_Settings apeSettings;
    BDSP_RaagaSettings raagaSettings;
    NEXUS_AudioModuleSettings audioModuleSettings;
    unsigned i;

    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow; /* decoder interface is slow */
    moduleSettings.dbgPrint = NEXUS_AudioModule_Print;
    moduleSettings.dbgModules = "nexus_audio_module";
    g_NEXUS_audioModule = NEXUS_Module_Create("audio", &moduleSettings);

    if ( NULL == g_NEXUS_audioModule )
    {
        errCode=BERR_TRACE(BERR_OS_ERROR);
        goto err_module;
    }
    NEXUS_LockModule();
    
    if ( NULL == pSettings )
    {
        NEXUS_AudioModule_GetDefaultSettings(&audioModuleSettings);
        pSettings = &audioModuleSettings;
    }

    g_NEXUS_audioModuleData.settings = *pSettings;

    BDSP_Raaga_GetDefaultSettings(&raagaSettings);

    #if 1
    {
        static BIMG_Interface imgInterface;
        /* TODO: Connect IMG to DSP interface */
        if ( Nexus_Core_P_Img_Create(NEXUS_CORE_IMG_ID_RAP, &g_NEXUS_audioModuleData.pImageContext, &imgInterface) == NEXUS_SUCCESS )
        {
            BDBG_WRN(("FW download used"));
            raagaSettings.pImageContext = g_NEXUS_audioModuleData.pImageContext;
            raagaSettings.pImageInterface = &imgInterface;
        }
    }
    #endif

    if ( pSettings->firmwareHeapIndex >= NEXUS_MAX_HEAPS ||
         g_pCoreHandles->heap[pSettings->firmwareHeapIndex] == NULL )
    {
        BDBG_ERR(("Invaild firmware heap provided."));
        goto err_heap;
    }

#ifdef NEXUS_SECURITY_AUDIO_VERIFICATION
    NEXUS_Audio_P_EnableFwVerification(&raagaSettings);
#endif

    for ( i = 0; i < NEXUS_AudioDspDebugType_eMax; i++ )
    {
        raagaSettings.debugSettings[i].enabled = pSettings->dspDebugSettings.typeSettings[i].enabled;
        raagaSettings.debugSettings[i].bufferSize = pSettings->dspDebugSettings.typeSettings[i].bufferSize;

        if ( pSettings->dspDebugSettings.typeSettings[i].enabled )
        {
            BDBG_WRN(("Enabling audio FW debug type %u [%s]", i, (i == NEXUS_AudioDspDebugType_eDramMessage)?"DRAM Message":
                                                                 (i == NEXUS_AudioDspDebugType_eUartMessage)?"UART Message":
                                                                 "Core Dump"));
        }
    }

    errCode = BDSP_Raaga_Open(&g_NEXUS_audioModuleData.dspHandle,
                              g_pCoreHandles->chp,
                              g_pCoreHandles->reg,
                              g_pCoreHandles->heap[pSettings->firmwareHeapIndex],
                              g_pCoreHandles->bint,
                              g_pCoreHandles->tmr,
                              &raagaSettings);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_dsp;
    }
#ifdef NEXUS_SECURITY_AUDIO_VERIFICATION
    errCode = NEXUS_Audio_P_FwVerification (g_NEXUS_audioModuleData.dspHandle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return NULL;
    }
#endif

    BAPE_GetDefaultSettings(&apeSettings);
    apeSettings.maxDspTasks = pSettings->maxAudioDspTasks;
    apeSettings.maxIndependentDelay = pSettings->independentDelay ? pSettings->maxIndependentDelay : 0;
    apeSettings.maxPcmSampleRate = pSettings->maxPcmSampleRate;
    apeSettings.numPcmBuffers = pSettings->numPcmBuffers;
    apeSettings.numCompressedBuffers = pSettings->numCompressedBuffers;
    apeSettings.numCompressed4xBuffers = pSettings->numCompressed4xBuffers;
    apeSettings.numCompressed16xBuffers = pSettings->numCompressed16xBuffers;
    apeSettings.numRfEncodedPcmBuffers = pSettings->numRfEncodedPcmBuffers;
    apeSettings.loudnessMode = (BAPE_LoudnessEquivalenceMode)pSettings->loudnessMode;

    if ( NEXUS_GetEnv("audio_ramp_disabled") )
    {
        apeSettings.rampPcmSamples = false;
    }

    errCode = BAPE_Open(&NEXUS_AUDIO_DEVICE_HANDLE,
                        g_pCoreHandles->chp,
                        g_pCoreHandles->reg,
                        g_pCoreHandles->heap[0],
                        g_pCoreHandles->bint,
                        g_pCoreHandles->tmr,
                        g_NEXUS_audioModuleData.dspHandle,
                        &apeSettings);

    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_ape;
    }

    errCode = NEXUS_AudioDecoder_P_Init();
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_decoder;
    }

    errCode = NEXUS_AudioInput_P_Init();
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_input;
    }

#if NEXUS_NUM_DSP_VIDEO_DECODERS
    errCode = NEXUS_AudioModule_P_InitDspVideoDecoder();
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_video;
    }
#endif

#if NEXUS_NUM_DSP_VIDEO_ENCODERS
    errCode = NEXUS_AudioModule_P_InitDspVideoEncoder();
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_encoder;
    }
#endif

    NEXUS_UnlockModule();
    return g_NEXUS_audioModule;

#if NEXUS_NUM_DSP_VIDEO_ENCODERS
err_encoder:
#if NEXUS_NUM_DSP_VIDEO_DECODERS
    NEXUS_AudioModule_P_UninitDspVideoDecoder();
err_video:    
#endif
    NEXUS_AudioInput_P_Uninit();
#else
#if NEXUS_NUM_DSP_VIDEO_DECODERS 
err_video:
    NEXUS_AudioInput_P_Uninit();
#endif
#endif
err_input:
    NEXUS_AudioDecoder_P_Uninit();
err_decoder:
    BAPE_Close(NEXUS_AUDIO_DEVICE_HANDLE);
err_ape:
    BDSP_Close(g_NEXUS_audioModuleData.dspHandle);
err_dsp:
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_audioModule);
err_heap:
    if (g_NEXUS_audioModuleData.pImageContext) {
        Nexus_Core_P_Img_Destroy(g_NEXUS_audioModuleData.pImageContext);
    }
err_module:
    return NULL;
}

void NEXUS_AudioModule_Uninit(void)
{
    NEXUS_LockModule();
#if NEXUS_NUM_DSP_VIDEO_ENCODERS
    NEXUS_AudioModule_P_UninitDspVideoEncoder();
#endif
#if NEXUS_NUM_DSP_VIDEO_DECODERS
    NEXUS_AudioModule_P_UninitDspVideoDecoder();
#endif
    NEXUS_AudioInput_P_Uninit();
    NEXUS_AudioDecoder_P_Uninit();

    if ( NEXUS_AUDIO_DEVICE_HANDLE )
    {
        BAPE_Close(NEXUS_AUDIO_DEVICE_HANDLE);
    }
    if ( g_NEXUS_audioModuleData.dspHandle )
    {
        BDSP_Close(g_NEXUS_audioModuleData.dspHandle);
    }
#ifdef NEXUS_SECURITY_AUDIO_VERIFICATION
    NEXUS_Audio_P_DisableFwVerification ();
#endif

    if (g_NEXUS_audioModuleData.pImageContext) {
        Nexus_Core_P_Img_Destroy(g_NEXUS_audioModuleData.pImageContext);
    }
    
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_audioModule);
}

void NEXUS_AudioModule_GetRampStepSettings(
    NEXUS_AudioRampStepSettings *pRampStepSettings      /* [out] ramping step size for scale change for all output ports */
    )
{
    uint32_t val;
    if ( NULL == pRampStepSettings )
    {
        return;
    }
    BAPE_GetOutputVolumeRampStep(NEXUS_AUDIO_DEVICE_HANDLE, &val);
    pRampStepSettings->mixerRampStep = val;
    BAPE_GetSampleRateConverterRampStep(NEXUS_AUDIO_DEVICE_HANDLE, &val);
    pRampStepSettings->srcRampStep = val;
}

NEXUS_Error NEXUS_AudioModule_SetRampStepSettings(
    const NEXUS_AudioRampStepSettings *pRampStepSettings  /* ramping step size for scale change for all output ports */
    )
{
    BERR_Code errCode;

    if ( NULL == pRampStepSettings )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    errCode = BAPE_SetOutputVolumeRampStep(NEXUS_AUDIO_DEVICE_HANDLE, pRampStepSettings->mixerRampStep);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_SetSampleRateConverterRampStep(NEXUS_AUDIO_DEVICE_HANDLE, pRampStepSettings->srcRampStep);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioModule_EnableExternalMclk(
    unsigned mclkIndex,
    unsigned pllIndex,
    NEXUS_ExternalMclkRate mclkRate
    )
{
    BERR_Code       errCode;
    BAPE_Pll        bapePll;
    BAPE_MclkRate   bapeMclkRate;

    switch ( pllIndex )
    {
    case 0:
        bapePll = BAPE_Pll_e0;
        break;
    case 1:
        bapePll = BAPE_Pll_e1;
        break;
    case 2:
        bapePll = BAPE_Pll_e2;
        break;

    default:
        BDBG_ERR(("pllIndex:%lu is invalid", pllIndex));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch( mclkRate )
    {
    case NEXUS_ExternalMclkRate_e128Fs:
        bapeMclkRate = BAPE_MclkRate_e128Fs;
        break;

    case NEXUS_ExternalMclkRate_e256Fs:
        bapeMclkRate = BAPE_MclkRate_e256Fs;
        break;

    case NEXUS_ExternalMclkRate_e384Fs:
        bapeMclkRate = BAPE_MclkRate_e384Fs;
        break;

    case NEXUS_ExternalMclkRate_e512Fs:
        bapeMclkRate = BAPE_MclkRate_e512Fs;
        break;

    default:
        BDBG_ERR(("mclkRate:%lu is invalid", mclkRate));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    errCode = BAPE_Pll_EnableExternalMclk( NEXUS_AUDIO_DEVICE_HANDLE, bapePll, mclkIndex, bapeMclkRate );
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

BAVC_AudioCompressionStd NEXUS_Audio_P_CodecToMagnum(NEXUS_AudioCodec codec)
{
    return NEXUS_P_AudioCodec_ToMagnum(codec);
}

NEXUS_AudioCodec NEXUS_Audio_P_MagnumToCodec(BAVC_AudioCompressionStd codec)
{
    return NEXUS_P_AudioCodec_FromMagnum(codec);
}

NEXUS_Error NEXUS_AudioModule_Standby_priv(
    bool enabled,
    const NEXUS_StandbySettings *pSettings
    )
{
    BERR_Code rc = NEXUS_SUCCESS;

    BSTD_UNUSED(pSettings);

#if NEXUS_POWER_MANAGEMENT
    if(enabled) {
	rc = BAPE_Standby(g_NEXUS_audioModuleData.apeHandle, NULL);
	if (rc) { rc = BERR_TRACE(rc); goto err; }    

	rc = BDSP_Standby(g_NEXUS_audioModuleData.dspHandle, NULL);
	if (rc) { rc = BERR_TRACE(rc); goto err; }    
    } else {
	rc = BDSP_Resume(g_NEXUS_audioModuleData.dspHandle);
	if (rc) { rc = BERR_TRACE(rc); goto err; }    

	rc = BAPE_Resume(g_NEXUS_audioModuleData.apeHandle);
	if (rc) { rc = BERR_TRACE(rc); goto err; }    
    }

err :        
#else
    BSTD_UNUSED(enabled);    
#endif    
    
    return rc;
}

void NEXUS_GetAudioCapabilities(NEXUS_AudioCapabilities *pCaps)
{
    BAPE_Capabilities apeCaps;
    unsigned i;
    BDBG_ASSERT(NULL != pCaps);

    BAPE_GetCapabilities(g_NEXUS_audioModuleData.apeHandle, &apeCaps);

    BKNI_Memset(pCaps, 0, sizeof(NEXUS_AudioCapabilities));

    pCaps->numInputs.hdmi = apeCaps.numInputs.mai;
    pCaps->numInputs.i2s = apeCaps.numInputs.i2s;
    pCaps->numInputs.spdif = apeCaps.numInputs.spdif;

    pCaps->numOutputs.audioReturnChannel = apeCaps.numOutputs.audioReturnChannel;
    pCaps->numOutputs.capture = apeCaps.numOutputs.capture;
    pCaps->numOutputs.dac = apeCaps.numOutputs.dac;
    pCaps->numOutputs.dummy = apeCaps.numOutputs.dummy;
    pCaps->numOutputs.hdmi = apeCaps.numOutputs.mai;
    pCaps->numOutputs.i2s = apeCaps.numOutputs.i2s;
    pCaps->numOutputs.loopback = apeCaps.numOutputs.loopback;
    pCaps->numOutputs.rfmod = apeCaps.numOutputs.rfmod;
    pCaps->numOutputs.spdif = apeCaps.numOutputs.spdif;

    #ifdef NEXUS_NUM_AUDIO_DECODERS
    pCaps->numDecoders = NEXUS_NUM_AUDIO_DECODERS < apeCaps.numDecoders ? NEXUS_NUM_AUDIO_DECODERS : apeCaps.numDecoders;
    #endif
    #ifdef NEXUS_NUM_AUDIO_PLAYBACKS
    pCaps->numPlaybacks = NEXUS_NUM_AUDIO_PLAYBACKS < apeCaps.numPlaybacks ? NEXUS_NUM_AUDIO_PLAYBACKS : apeCaps.numPlaybacks;
    #endif
    #ifdef NEXUS_NUM_AUDIO_INPUT_CAPTURES
    pCaps->numInputCaptures = NEXUS_NUM_AUDIO_INPUT_CAPTURES < apeCaps.numInputCaptures ? NEXUS_NUM_AUDIO_INPUT_CAPTURES : apeCaps.numInputCaptures;
    #endif
    pCaps->numVcxos = apeCaps.numVcxos;
    pCaps->numPlls = apeCaps.numPlls;
    pCaps->numNcos = apeCaps.numNcos;

    pCaps->numDsps = apeCaps.numDsps;
    for ( i = 0; i < NEXUS_AudioCodec_eMax; i++ )
    {
        BAVC_AudioCompressionStd codec = NEXUS_Audio_P_CodecToMagnum(i);
        if ( codec != BAVC_AudioCompressionStd_eMax )
        {
            pCaps->dsp.codecs[i].decode = apeCaps.dsp.codecs[codec].decode;
            pCaps->dsp.codecs[i].encode = apeCaps.dsp.codecs[codec].encode;
        }
    }
    pCaps->dsp.audysseyAbx = apeCaps.dsp.audysseyAbx;
    pCaps->dsp.audysseyAdv = apeCaps.dsp.audysseyAdv;
    pCaps->dsp.autoVolumeLevel = apeCaps.dsp.autoVolumeLevel;
    pCaps->dsp._3dSurround = apeCaps.dsp._3dSurround;
    pCaps->dsp.decodeRateControl = apeCaps.dsp.decodeRateControl;
    pCaps->dsp.dolbyDigitalReencode = apeCaps.dsp.dolbyDigitalReencode;
    pCaps->dsp.dolbyVolume258 = apeCaps.dsp.dolbyVolume;
    pCaps->dsp.echoCanceller.supported = apeCaps.dsp.echoCanceller.supported;
    pCaps->dsp.echoCanceller.algorithms[NEXUS_EchoCancellerAlgorithm_eSpeex] = apeCaps.dsp.echoCanceller.algorithms[BAPE_EchoCancellerAlgorithm_eSpeex];
    pCaps->dsp.encoder = apeCaps.dsp.encoder;
    pCaps->dsp.mixer = apeCaps.dsp.mixer;
    pCaps->dsp.muxOutput = apeCaps.dsp.muxOutput;
    pCaps->dsp.rfEncoder.supported = apeCaps.dsp.rfEncoder.supported;
    pCaps->dsp.rfEncoder.encodings[NEXUS_RfAudioEncoding_eBtsc] = apeCaps.dsp.rfEncoder.encodings[BAPE_RfAudioEncoding_eBtsc];
    pCaps->dsp.studioSound = apeCaps.dsp.studioSound;
    pCaps->dsp.truSurroundHd = apeCaps.dsp.truSurroundHd;
    pCaps->dsp.truVolume = apeCaps.dsp.truVolume;
    pCaps->equalizer.supported = apeCaps.equalizer.supported;
}
    
void NEXUS_AudioOutputPll_GetSettings(
    NEXUS_AudioOutputPll pll,
    NEXUS_AudioOutputPllSettings *pSettings       /* [out] Current Settings */
    )
{
    BAPE_PllSettings apePllSettings;
    BAPE_Pll apePll;

    apePll = (BAPE_Pll)pll;
    BDBG_ASSERT(NULL != pSettings);

    BAPE_Pll_GetSettings(g_NEXUS_audioModuleData.apeHandle, apePll, &apePllSettings);
    pSettings->mode = apePllSettings.freeRun ? NEXUS_AudioOutputPllMode_eFreeRun : NEXUS_AudioOutputPllMode_eVcxo;
    pSettings->modeSettings.vcxo.vcxo = (NEXUS_Vcxo)apePllSettings.vcxo;
}

NEXUS_Error NEXUS_AudioOutputPll_SetSettings(
    NEXUS_AudioOutputPll pll,
    const NEXUS_AudioOutputPllSettings *pSettings
    )
{
    BAPE_PllSettings apePllSettings;
    BERR_Code errCode;
    BAPE_Pll apePll;

    apePll = (BAPE_Pll)pll;
    BDBG_ASSERT(NULL != pSettings);

    BAPE_Pll_GetSettings(g_NEXUS_audioModuleData.apeHandle, apePll, &apePllSettings);
    apePllSettings.freeRun = pSettings->mode == NEXUS_AudioOutputPllMode_eFreeRun ? true : false;
    apePllSettings.vcxo = (unsigned)pSettings->modeSettings.vcxo.vcxo;       
    errCode = BAPE_Pll_SetSettings(g_NEXUS_audioModuleData.apeHandle, apePll, &apePllSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

