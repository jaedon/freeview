/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_hdmitx.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/28/12 3:48p $
 *
 * Module Description: HDMI Abstraction for APE audio testing
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_hdmitx.c $
 * 
 * Hydra_Software_Devel/2   8/28/12 3:48p jgarrett
 * SW7425-3629: Adding timer handle to BHDM_Settings
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 * 
 * Hydra_Software_Devel/1   4/11/12 7:25p jgarrett
 * SW7425-2075: Adding HDMITX support and HBR test tone app
 *
 ***************************************************************************/
#include "bape.h"
#include "bfmt.h"
#include "bavc.h"
#include "apetest_hdmitx.h"

BDBG_MODULE(apetest_hdmitx);

void APETEST_HDMITX_GetDefaultSettings(
    APETEST_HDMITX_Settings *pSettings    /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    pSettings->videoFormat = BFMT_VideoFmt_e720p;
    pSettings->aspectRatio = BFMT_AspectRatio_e16_9;
    pSettings->channelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e2Channels;
    pSettings->speakerAllocation = 0x3; /* Stereo */
}
#include "apetest_hdmitx.h"

#ifdef APETEST_HDMITX_ENABLED

/* VDC includes */
#include "brdc.h"
#include "bvdc.h"

/* HDM includes */
#include "bhdm.h"
#include "bhdm_edid.h"
#include "bi2c.h"

/* I2C Line Definitions per-board */
#if (BCHP_CHIP==7468)
#define BTST_P_I2C_CHANNEL                  (1)
#elif (BCHP_CHIP==7550) || (BCHP_CHIP==7408)
#define BTST_P_I2C_CHANNEL                  (2)
#elif (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435)
#define BTST_P_I2C_CHANNEL                  (0)
#else
#define BTST_P_I2C_CHANNEL                  (3)
#endif

/* Module Handles */
static BVDC_Handle g_hVdc;
static BRDC_Handle g_hRdc;
static BVDC_Compositor_Handle g_hCompositor;
static BVDC_Display_Handle g_hDisplay;
static BI2C_Handle g_hI2c;
static BI2C_ChannelHandle g_hI2cChn;
static BREG_I2C_Handle g_hRegI2c;
static BHDM_Handle g_hHdm;
static BAPE_MaiOutputHandle g_hMaiOutput;

static void display_callback_isr(void *pParam1, int param2, void *pData)
{
    BAVC_VdcDisplay_Info *pVdcRateInfo;
    BVDC_Display_CallbackData *pCallbackData = pData;
    BHDM_Handle hHdm = (BHDM_Handle)param2;
    BSTD_UNUSED(pParam1);

    pVdcRateInfo = &(pCallbackData->sDisplayInfo);
    if (pCallbackData->stMask.bRateChange) 
    {
        BDBG_WRN(("VDC Rate Change"));
        BHDM_AudioVideoRateChangeCB_isr(hHdm, BHDM_Callback_Type_eVideoChange, pVdcRateInfo);
    }
}

static BAVC_AudioSamplingRate sample_rate_to_avc(unsigned sampleRate)
{
    switch ( sampleRate )
    {
    case 32000:
        return BAVC_AudioSamplingRate_e32k;    /* 32K Sample rate */
    case 44100:
        return BAVC_AudioSamplingRate_e44_1k;    /* 44.1K Sample rate */
    case 48000:
        return BAVC_AudioSamplingRate_e48k;      /* 48K Sample rate */
    case 96000:
        return BAVC_AudioSamplingRate_e96k;      /* 96K Sample rate */
    case 16000:
        return BAVC_AudioSamplingRate_e16k;      /* 16K Sample rate */
    case 22050:
        return BAVC_AudioSamplingRate_e22_05k;   /* 22.05K Sample rate */
    case 24000:
        return BAVC_AudioSamplingRate_e24k;      /* 24K Sample rate */
    case 64000:
        return BAVC_AudioSamplingRate_e64k;      /* 64K Sample rate */
    case 88200:
        return BAVC_AudioSamplingRate_e88_2k;    /* 88.2K Sample rate */
    case 128000:
        return BAVC_AudioSamplingRate_e128k;     /* 128K Sample rate */
    case 176400:
        return BAVC_AudioSamplingRate_e176_4k;   /* 176.4K Sample rate */
    case 192000:
        return BAVC_AudioSamplingRate_e192k;     /* 192K Sample rate */
    case 8000:
        return BAVC_AudioSamplingRate_e8k;       /* 8K Sample rate */
    case 12000:
        return BAVC_AudioSamplingRate_e12k;      /* 12K Sample rate */
    case 11025:
        return BAVC_AudioSamplingRate_e11_025k;  /* 11.025K Sample rate */
    default:
        return 0;
    }
}

static void mai_callback_isr(void *pParam1, int param2, unsigned sampleRate)
{
    BAVC_Audio_Info audioInfo;
    BHDM_Handle hHdm = (BHDM_Handle)param2;
    BSTD_UNUSED(pParam1);

    BDBG_WRN(("MAI Callback - SR %u", sampleRate));

    audioInfo.eAudioSamplingRate = sample_rate_to_avc(sampleRate);

    BHDM_AudioVideoRateChangeCB_isr(hHdm, BHDM_Callback_Type_eAudioChange, &audioInfo);
}


/***************************************************************************
Summary:
Initialize the HDMI outuput
****************************************************************************/
BERR_Code APETEST_HDMITX_Init(
    BFramework_Info *pFrmInfo,
    const APETEST_HDMITX_Settings *pSettings,
    BAPE_MaiOutputHandle hMaiOutput
    )
{
    BERR_Code retCode;

    BVDC_Settings vdcSettings;    
    BVDC_Display_Settings vdcDisplayCfg;
    BHDM_Settings hdmSettings;
    uint8_t supported;
    bool hdmiDevice;
    BHDM_EDID_RxVendorSpecificDB vendorDb;
    BHDM_OutputFormat   eOutputMode ;
    BAVC_MatrixCoefficients colorimetry;

    BI2C_ChannelSettings stI2cChannelSettings;
    BAVC_HDMI_AudioInfoFrame audioInfoFrame;
    BVDC_Display_CallbackSettings displayCbSettings;

    BAPE_MaiOutputInterruptHandlers maiInterrupts;

    /* Get HDMI I2C Controller */
    retCode = BI2C_Open(&g_hI2c, pFrmInfo->hChp, pFrmInfo->hReg, pFrmInfo->hInt, NULL);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    BI2C_GetChannelDefaultSettings(g_hI2c, BTST_P_I2C_CHANNEL, &stI2cChannelSettings);
    stI2cChannelSettings.clkRate = BI2C_Clk_eClk100Khz;
    retCode = BI2C_OpenChannel(g_hI2c, &g_hI2cChn, BTST_P_I2C_CHANNEL, &stI2cChannelSettings);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }
    retCode = BI2C_CreateI2cRegHandle(g_hI2cChn, &g_hRegI2c);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    BHDM_GetDefaultSettings(&hdmSettings);
    hdmSettings.hTMR = pFrmInfo->hTmr;
    retCode = BHDM_Open(&g_hHdm, pFrmInfo->hChp, pFrmInfo->hReg, pFrmInfo->hInt, g_hRegI2c, &hdmSettings);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BRDC_Open(&g_hRdc, pFrmInfo->hChp, pFrmInfo->hReg, pFrmInfo->hMem, NULL);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    BVDC_GetDefaultSettings(&vdcSettings);
    retCode = BVDC_Open(&g_hVdc,
                        pFrmInfo->hChp, 
                        pFrmInfo->hReg, 
                        pFrmInfo->hMem, 
                        pFrmInfo->hInt, 
                        g_hRdc,
                        pFrmInfo->hTmr, 
                        &vdcSettings);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_Compositor_Create(g_hVdc, &g_hCompositor, BVDC_CompositorId_eCompositor0, NULL);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    BVDC_Display_GetDefaultSettings(BVDC_DisplayId_eDisplay0, &vdcDisplayCfg);
    retCode = BVDC_Display_Create(g_hCompositor, &g_hDisplay, BVDC_DisplayId_eDisplay0, &vdcDisplayCfg);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_Display_InstallCallback(g_hDisplay, (BVDC_CallbackFunc_isr)display_callback_isr, g_hDisplay, (int)g_hHdm);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_ApplyChanges(g_hVdc);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    BVDC_Display_GetCallbackSettings(g_hDisplay, &displayCbSettings);
    displayCbSettings.stMask.bRateChange = 1;
    retCode = BVDC_Display_SetCallbackSettings(g_hDisplay, &displayCbSettings);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_Compositor_SetBackgroundColor(g_hCompositor, 0xff, 0x00, 0x00); /* Set Red to identify we're active (most TV's show blue or black if not */
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_Display_SetVideoFormat(g_hDisplay, pSettings->videoFormat);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_Display_SetTimebase(g_hDisplay, BAVC_Timebase_e0);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_ApplyChanges(g_hVdc);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    /* Open HDMI */
    retCode = BHDM_EDID_Initialize(g_hHdm);
    if ( retCode == BHDM_NO_RX_DEVICE )
    {
        BDBG_WRN(("No HDMI Device"));
        return BERR_SUCCESS;
    }
    else if ( retCode )
    {
        return BERR_TRACE(retCode);
    }
    retCode = BHDM_EDID_IsRxDeviceHdmi(g_hHdm, &vendorDb, &hdmiDevice);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }
    eOutputMode = hdmiDevice ? BHDM_OutputFormat_eHDMIMode : BHDM_OutputFormat_eDVIMode ;
    retCode = BHDM_EDID_GetSupportedColorimetry(g_hHdm, eOutputMode, pSettings->videoFormat, &colorimetry) ;
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }
    retCode = BHDM_EDID_VideoFmtSupported(g_hHdm, pSettings->videoFormat, &supported);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }
    retCode = BVDC_Display_SetHdmiConfiguration(g_hDisplay, BVDC_Hdmi_0, colorimetry);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }
    retCode = BVDC_Display_SetHdmiSyncOnly(g_hDisplay, false);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    retCode = BVDC_ApplyChanges(g_hVdc);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    /* Enable the HDMI output */
    BHDM_GetHdmiSettings(g_hHdm, &hdmSettings);

    hdmSettings.eOutputFormat = (hdmiDevice)?BHDM_OutputFormat_eHDMIMode:BHDM_OutputFormat_eDVIMode;
    hdmSettings.eAspectRatio = pSettings->aspectRatio;
    hdmSettings.eInputVideoFmt = pSettings->videoFormat;
    hdmSettings.eColorimetry = colorimetry;

    retCode = BHDM_EnableDisplay(g_hHdm, &hdmSettings);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    /* Send Audio Info Frame */
    retCode = BHDM_GetAudioInfoFramePacket(g_hHdm, &audioInfoFrame);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }
    audioInfoFrame.ChannelCount = pSettings->channelCount;
    audioInfoFrame.SpeakerAllocation = pSettings->speakerAllocation;
    audioInfoFrame.DownMixInhibit = 0;  /* default */
    audioInfoFrame.LevelShift = 0;  /* default */
    retCode = BHDM_SetAudioInfoFramePacket(g_hHdm, &audioInfoFrame);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    BHDM_GetHdmiSettings(g_hHdm, &hdmSettings);
    hdmSettings.eAudioSamplingRate = BAVC_AudioSamplingRate_e48k;
    hdmSettings.eAudioBits = BAVC_AudioBits_e16;
    hdmSettings.eAudioFormat = BAVC_AudioFormat_ePCM;
    retCode = BHDM_EnableDisplay(g_hHdm, &hdmSettings);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    g_hMaiOutput = hMaiOutput;
    BAPE_MaiOutput_GetInterruptHandlers(hMaiOutput, &maiInterrupts);
    maiInterrupts.sampleRate.pCallback_isr = mai_callback_isr;
    maiInterrupts.sampleRate.pParam1 = hMaiOutput;
    maiInterrupts.sampleRate.param2 = (int)g_hHdm;
    retCode = BAPE_MaiOutput_SetInterruptHandlers(hMaiOutput, &maiInterrupts);
    if ( retCode )
    {
        return BERR_TRACE(retCode);
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Un-Initialize the HDMI outuput
****************************************************************************/
void APETEST_HDMITX_Uninit(
    void
    )
{
    if ( g_hMaiOutput )
    {
        BAPE_MaiOutputInterruptHandlers maiInterrupts;
        BAPE_MaiOutput_GetInterruptHandlers(g_hMaiOutput, &maiInterrupts);
        maiInterrupts.sampleRate.pCallback_isr = NULL;
        (void)BAPE_MaiOutput_SetInterruptHandlers(g_hMaiOutput, &maiInterrupts);
        g_hMaiOutput = NULL;
    }

    if ( g_hHdm )
    {
        BHDM_DisableDisplay(g_hHdm);
        BHDM_Close(g_hHdm);
        g_hHdm = NULL;
    }
    if ( g_hRegI2c )
    {
        BI2C_CloseI2cRegHandle(g_hRegI2c);
        g_hRegI2c = NULL;
    }
    if ( g_hI2cChn )
    {
        BI2C_CloseChannel(g_hI2cChn);
        g_hI2cChn = NULL;
    }
    if ( g_hI2c )
    {
        BI2C_Close(g_hI2c);
        g_hI2c = NULL;
    }
    if ( g_hDisplay )
    {
        BVDC_Display_Destroy(g_hDisplay);
        g_hDisplay = NULL;
    }
    if ( g_hCompositor )
    {
        BVDC_Compositor_Destroy(g_hCompositor);
        g_hCompositor = NULL;
    }
    if ( g_hVdc )
    {
        BVDC_Close(g_hVdc);
        g_hVdc = NULL;
    }
    if ( g_hRdc )
    {
        BRDC_Close(g_hRdc);
        g_hRdc = NULL;
    }
}

#else /* #ifdef APE_HDMITX_ENABLED */

BERR_Code APETEST_HDMITX_Init(
    BFramework_Info *pFrmInfo,
    const APETEST_HDMITX_Settings *pSettings,
    BAPE_MaiOutputHandle hMaiOutput
    )
{
    BSTD_UNUSED(pFrmInfo);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(hMaiOutput);
    BDBG_WRN(("HDMITX Disabled"));
    return BERR_SUCCESS;
}

void APETEST_HDMITX_Uninit(
    void
    )
{
    return;
}
#endif /* #ifdef APE_HDMITX_ENABLED */

