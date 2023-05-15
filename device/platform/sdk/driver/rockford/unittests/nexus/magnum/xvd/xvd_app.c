/******************************************************************************
 *    (c)2008 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
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
 * $brcm_Workfile: xvd_app.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/14/09 10:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/xvd/xvd_app.c $
 * 
 * Hydra_Software_Devel/1   5/14/09 10:51a jtna
 * PR54515: added xvd_app
 * 
 *****************************************************************************/
/* sample application doing playback or decode with a minimal set of nexus modules and calling XVD directly */

#include <stdio.h>
#include "nexus_types.h"
#include "bxvd.h"

/* Nexus public API includes */
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"

/* Nexus private API includes */
#include "priv/nexus_rave_priv.h"
#include "priv/nexus_stc_channel_priv.h"
#include "nexus_video_decoder_types.h"
#include "priv/nexus_video_decoder_priv.h"
#include "nexus_base.h" /* for NEXUS_Module_Lock/Unlock */
#include "../src/nexus_platform_priv.h" /* for g_NEXUS_platformHandles.transport */


BDBG_MODULE(xvd_app);

/* globals */
BXVD_Handle xvd;
BXVD_ChannelHandle channel;
NEXUS_RaveHandle rave;
NEXUS_VideoDecoderDisplayConnection g_displayConnection;
NEXUS_VideoInputObject g_videoInput;

#define LOCK_TRANSPORT()   NEXUS_Module_Lock(g_NEXUS_platformHandles.transport);
#define UNLOCK_TRANSPORT() NEXUS_Module_Unlock(g_NEXUS_platformHandles.transport);


#define USE_PLAYBACK 1 /* if 0, use streamer input */

#if 0
#define STREAM_FILENAME "videos/cnnticker.mpg"
#define STREAM_XPTTYPE  NEXUS_TransportType_eTs
#define STREAM_VPID     0x21
#define STREAM_VCODEC   NEXUS_VideoCodec_eMpeg2
#endif
#if 1
#define STREAM_FILENAME "videos/Speed_720.wmv"
#define STREAM_XPTTYPE  NEXUS_TransportType_eAsf
#define STREAM_VPID     0x2
#define STREAM_VCODEC   NEXUS_VideoCodec_eVc1SimpleMain
#endif

/* nexus -> magnum codec mappings */
struct codec_map {
    NEXUS_VideoCodec nexus_codec;
    BAVC_VideoCompressionStd magnum_codec;
} g_codec_map[] = {
    {NEXUS_VideoCodec_eH263, BAVC_VideoCompressionStd_eH263},
    {NEXUS_VideoCodec_eH264, BAVC_VideoCompressionStd_eH264},
    {NEXUS_VideoCodec_eVc1, BAVC_VideoCompressionStd_eVC1},
    {NEXUS_VideoCodec_eVc1SimpleMain, BAVC_VideoCompressionStd_eVC1SimpleMain},
    {NEXUS_VideoCodec_eMpeg1, BAVC_VideoCompressionStd_eMPEG1},
    {NEXUS_VideoCodec_eMpeg4Part2, BAVC_VideoCompressionStd_eMPEG4Part2},
    {NEXUS_VideoCodec_eDivx311, BAVC_VideoCompressionStd_eMPEG4Part2},
    {NEXUS_VideoCodec_eAvs, BAVC_VideoCompressionStd_eAVS},
    /* TODO: BAVC_VideoCompressionStd_eMPEG2DTV and BAVC_VideoCompressionStd_eMPEG2_DSS_PES 
       are transport_type-dependent */
    {NEXUS_VideoCodec_eMpeg2, BAVC_VideoCompressionStd_eMPEG2}
};

BAVC_VideoCompressionStd get_magnum_codec(NEXUS_VideoCodec nexus_codec)
{
    unsigned i;
    for (i=0; i<sizeof(g_codec_map)/sizeof(*g_codec_map);i++) {
        if (g_codec_map[i].nexus_codec == nexus_codec) {
            return g_codec_map[i].magnum_codec;
        }
    }
    return BAVC_VideoCompressionStd_eMPEG2_DSS_PES+1;
}

void xvd_dataready_isr(void *parm1, int parm2, void *field)
{
    BAVC_MFD_Picture *pFieldData = (BAVC_MFD_Picture*)field;

#if 0 /* print channel status */
    BXVD_ChannelStatus xvdChannelStatus;
    BERR_Code rc;
    rc = BXVD_GetChannelStatus_isr(channel, &xvdChannelStatus);
    BDBG_ASSERT(!rc);

    printf("ch[%d] %p %dx%d%c, AdjQp %u%s%s\n",
        0,
        pFieldData->pLuminanceFrameBufferAddress, /* use to check for unique buffers */
        pFieldData->ulDisplayHorizontalSize, pFieldData->ulDisplayVerticalSize,
        pFieldData->eSourcePolarity==BAVC_Polarity_eTopField?'t':pFieldData->eSourcePolarity==BAVC_Polarity_eBotField?'b':'p',
        pFieldData->ulAdjQp,
        pFieldData->bMute?" muted":"",
        pFieldData->bPictureRepeatFlag?" repeat":"");
#endif

    BSTD_UNUSED(parm1);
    BSTD_UNUSED(parm2);

    if (g_displayConnection.dataReadyCallback_isr)
    {
        (*g_displayConnection.dataReadyCallback_isr)(g_displayConnection.callbackContext, pFieldData);
    }
}

void open_xvd(void)
{
    BERR_Code rc;
    BXVD_Settings xvdSettings;
    BXVD_ChannelSettings channelSettings;
    NEXUS_RaveOpenSettings raveSettings;
    int pic_buf_length;
    
    BXVD_GetDefaultSettings(&xvdSettings);
    xvdSettings.uiAVDInstance = 0;
    xvdSettings.hFrmMemory = g_pCoreHandles->heap[0]; /* FW always in main heap */

    rc = BXVD_Open(&xvd, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[0], g_pCoreHandles->heap[0], g_pCoreHandles->bint, &xvdSettings);
    BDBG_ASSERT(!rc);
    
    NEXUS_VIDEO_INPUT_INIT(&g_videoInput, NEXUS_VideoInputType_eDecoder, (void*)1);
    
    BXVD_GetChannelDefaultSettings(xvd, 0, &channelSettings);
    rc = BXVD_OpenChannel(xvd, &channel, 0, &channelSettings);
    BDBG_ASSERT(!rc);
    
    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(&raveSettings);
    raveSettings.allocSoftRave = true; /* needed for vc1 & asf */
    UNLOCK_TRANSPORT();
    
    rc = BXVD_InstallDeviceInterruptCallback(xvd, BXVD_DeviceInterrupt_ePictureDataReady0,
        xvd_dataready_isr, NULL, 0);
    BDBG_ASSERT(!rc);
    rc = BXVD_GetBufferConfig(xvd, &raveSettings.config, &pic_buf_length);
    BDBG_ASSERT(!rc);
    
    /* set up rave */
    LOCK_TRANSPORT();
    rave = NEXUS_Rave_Open_priv(&raveSettings);
    BDBG_ASSERT(rave);
    UNLOCK_TRANSPORT();
}

void start_xvd(NEXUS_PidChannelHandle pidChannel, NEXUS_VideoCodec codec)
{
    BERR_Code rc;
    BXVD_DecodeSettings cfg;
    NEXUS_RaveSettings raveSettings;
    NEXUS_RaveStatus raveStatus;
    NEXUS_PidChannelStatus pidChannelStatus;
    int ptsOffset = 0;
    
    NEXUS_PidChannel_GetStatus(pidChannel, &pidChannelStatus);
    
    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultSettings_priv(&raveSettings);
    raveSettings.pidChannel = pidChannel;
    raveSettings.bandHold = pidChannelStatus.playback;
    rc = NEXUS_Rave_ConfigureVideo_priv(rave, codec, &raveSettings);
    rc |= NEXUS_Rave_GetStatus_priv(rave, &raveStatus);
    UNLOCK_TRANSPORT();
    
    rc = BXVD_SetDisplayOffset(channel, ptsOffset);
    BDBG_ASSERT(!rc);
    
    BXVD_GetDecodeDefaultSettings(channel, &cfg);
    cfg.eVideoCmprStd = get_magnum_codec(codec);
    cfg.pContextMap = &raveStatus.xptContextMap;
    cfg.bPlayback = pidChannelStatus.playback;

    rc = BXVD_StartDecode(channel, &cfg);
    BDBG_ASSERT(!rc);

    NEXUS_PidChannel_ConsumerStarted(pidChannel); /* needed to unpause playback & stuff like that */

    LOCK_TRANSPORT();
    NEXUS_Rave_Enable_priv(rave);
    UNLOCK_TRANSPORT();

    rc = BXVD_SetVideoDisplayMode(channel, BXVD_DisplayMode_eVSYNCMode); /* vsync mode only for now */

    BDBG_ASSERT(!rc);
}

#if USE_PLAYBACK
static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}
#endif

int main(int argc, char **argv)
{
    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfiguration;
#if (!USE_PLAYBACK)
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
#else
    FILE* file;
    char *fname = STREAM_FILENAME;
    BKNI_EventHandle event;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
#endif
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_DisplaySettings displaySettings;

    printf("starting nexus platform with minimal modules\n");
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openI2c = false;
    platformSettings.openFrontend = false;
    platformSettings.openFpga=false;
    platformSettings.openOutputs = true; /* open video outputs */
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Platform_GetConfiguration(&platformConfiguration);
    BDBG_SetModuleLevel("nexus_platform", BDBG_eMsg);

    /* open XVD manually */
    open_xvd();

#if USE_PLAYBACK
    BKNI_CreateEvent(&event);
    playpump = NEXUS_Playpump_Open(0, NULL);

    if (argc>1) {
        fname = argv[1];
    }

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    playpumpSettings.transportType = STREAM_XPTTYPE;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, STREAM_VPID, NULL);

#else /* streamer input */
    /* map a parser band to the streamer input band */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = NEXUS_InputBand_e5; /* 97405 */
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
#endif

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfiguration.outputs.component[0]));
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfiguration.outputs.composite[0]));
    window = NEXUS_VideoWindow_Open(display, 0);
    
    /* bring up decoder and connect to display */
    rc = NEXUS_VideoWindow_AddInput(window, &g_videoInput);
    BDBG_ASSERT(!rc);

    /* start decoder */
    start_xvd(videoPidChannel, STREAM_VCODEC);

#if USE_PLAYBACK
    while (1) {
        void *buffer;
        size_t buffer_size;
        int n;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
            break;
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

        n = fread(buffer, 1, buffer_size, file);
        if (n < 0) goto error;
        if (n == 0) {
            /* wait for the decoder to reach the end of the content before looping */
            while (1) {
                BXVD_ChannelStatus channelStatus;
                BXVD_GetChannelStatus(channel, &channelStatus);
                if (!channelStatus.ulPictureDeliveryCount) break;
            }
            fseek(file, 0, SEEK_SET);
            NEXUS_Playpump_Flush(playpump);
        }
        else {
            BXVD_PTSInfo pPTSInfo;
            rc = NEXUS_Playpump_ReadComplete(playpump, 0, n);
            BDBG_ASSERT(!rc);
            BXVD_GetPTS(channel, &pPTSInfo);
            printf("playback... PTS=%#x\n", pPTSInfo.ui32RunningPTS);
        }
    }

#else /* streamer input */
    while (1) {
        BXVD_PTSInfo pPTSInfo;
        BXVD_GetPTS(channel, &pPTSInfo);
        printf("decoding... PTS=%#x\n", pPTSInfo.ui32RunningPTS);
        BKNI_Sleep(1000);
    }
#endif

    printf("shutdown\n");
    NEXUS_Platform_Uninit();
    return 0;

error:
    return 1;
}


/* Nexus video_decoder module functions needed by display or transport */

NEXUS_Error NEXUS_VideoDecoder_GetSyncStatus_isr(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncStatus *pSyncStatus)
{   
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(pSyncStatus);

    /* unused */
    BDBG_ASSERT(0);
    return NEXUS_SUCCESS;
}

void
NEXUS_VideoDecoder_UpdateDisplayInformation_priv( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoder_DisplayInformation *displayInformation)
{
    BERR_Code rc;
    uint32_t refreshRate;

    BDBG_ASSERT(displayInformation);
    BDBG_MSG(("NEXUS_VideoDecoder_UpdateDisplayInformation_priv: %#lx  refreshRate:%u", (unsigned long)videoDecoder, displayInformation->refreshRate));

    switch(displayInformation->refreshRate) {
    case 2398:
    case 2397: /* real value is 23.976 */
        refreshRate = BXVD_MONITOR_REFRESH_RATE_23_976Hz; break;
    case 2400:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_24Hz; break;
    case 2500:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_25Hz; break;
    case 2997:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_29_97Hz; break;
    case 3000:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_30Hz; break;
    case 4800:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_48Hz; break;
    case 5000:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_50Hz; break;
    case 5994:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_59_94Hz; break;
    default:
    case 6000:
        refreshRate = BXVD_MONITOR_REFRESH_RATE_60Hz; break;
    }
    rc = BXVD_SetMonitorRefreshRate(channel, refreshRate);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    return;   
}

/* needed for NEXUS_VideoWindow_AddInput() */
void NEXUS_VideoDecoder_GetHeap_priv( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_HeapHandle *pHeap )
{
    BSTD_UNUSED(videoDecoder);
    *pHeap = 0; /* TODO: 0 is the only value that works. why? */
    return;
}

void NEXUS_VideoDecoder_GetSyncSettings_priv(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(pSyncSettings);

    /* unused */
    BDBG_ASSERT(0);
    return;
}

NEXUS_Error NEXUS_VideoDecoder_SetSyncSettings_priv(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(pSyncSettings);

    /* unused */
    BDBG_ASSERT(0);
    return NEXUS_SUCCESS;
}

void NEXUS_VideoDecoder_GetSourceId_priv(NEXUS_VideoDecoderHandle videoDecoder, BAVC_SourceId *pSource)
{
    BSTD_UNUSED(videoDecoder);
    *pSource = BAVC_SourceId_eMpeg0;
}

void NEXUS_VideoDecoder_GetDisplayConnection_priv(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderDisplayConnection *connection)
{
    BSTD_UNUSED(videoDecoder);
    *connection = g_displayConnection;
}

NEXUS_Error NEXUS_VideoDecoder_SetDisplayConnection_priv(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderDisplayConnection *connection)
{
    BERR_Code rc;
#if BCHP_CHIP == 7405
    BXVD_DeviceVdcInterruptSettings VDCDevIntrSettings;

    BDBG_ASSERT((int)videoDecoder == 1);
    VDCDevIntrSettings.VDCIntId_Topfield = connection->top.intId;
    VDCDevIntrSettings.VDCIntId_Botfield = connection->bottom.intId;
    VDCDevIntrSettings.VDCIntId_Frame = connection->frame.intId;
    VDCDevIntrSettings.eDisplayInterrupt = BXVD_DisplayInterrupt_eZero;

    rc = BXVD_RegisterVdcDeviceInterrupt(xvd, &VDCDevIntrSettings);
    if (rc) {return BERR_TRACE(rc);}
#else
    BDBG_ASSERT((int)videoDecoder == 1);
    rc = BXVD_RegisterVdcInterrupt(channel, connection->top.intId, BAVC_Polarity_eTopField);
    if (rc) {return BERR_TRACE(rc);}
    rc = BXVD_RegisterVdcInterrupt(channel, connection->bottom.intId, BAVC_Polarity_eBotField);
    if (rc) {return BERR_TRACE(rc);}
    rc = BXVD_RegisterVdcInterrupt(channel, connection->frame.intId, BAVC_Polarity_eFrame);
    if (rc) {return BERR_TRACE(rc);}
#endif

    g_displayConnection = *connection;
    return 0;
}
