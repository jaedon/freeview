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
 * $brcm_Workfile: tune_ofdm.c $
 * $brcm_Revision: 20 $
 * $brcm_Date: 10/8/12 1:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/frontend/tune_ofdm.c $
 * 
 * 20   10/8/12 1:08p erickson
 * SW7425-3782: rework mtsif
 * 
 * 19   8/7/12 5:37p mphillip
 * SW7231-924: Merge NEXUS_Frontend_Acquire changes to main
 * 
 * cng_unittest/1   7/30/12 5:00p cng
 * SW7231-924: Use NEXUS_Frontend_Acquire
 * 
 * 18   6/1/12 5:04p jtna
 * SW7552-282: check for return status
 * 
 * 17   5/10/12 7:32p jtna
 * SW7425-2752: update nexus examples to use
 *  NEXUS_ParserBandSourceType_eMtsif for MTSIF-based frontends
 * 
 * 16   1/27/12 4:17p rjlewis
 * SW3461-139: DVBT2 Partial Status usage example.
 * 
 * 15   1/26/12 6:42p rjlewis
 * SW3461-51: removed legacy status.
 * 
 * 14   1/25/12 5:25p rjlewis
 * SW3461-51: Bug fix.
 * 
 * 13   1/25/12 4:59p rjlewis
 * SW7425-1136: Added HDMI support.
 * 
 * 12   1/25/12 3:16p erickson
 * SW3461-51: merge
 * 
 * 3461_PARTIAL_STATUS/2   1/25/12 3:05p vishk
 * SW3461-51: Implement asynchronous nexus apis for retrieval of partial
 *  DVBT2 status. Cleanup of enum and api names.
 * 
 * 3461_PARTIAL_STATUS/1   1/23/12 4:19p vishk
 * SW3461-51: Implement asynchronous nexus apis for retrieval of partial
 *  DVBT2 status.
 * 
 * 11   1/20/12 9:28a erickson
 * SW7552-196: update
 * 
 * 10   9/12/11 6:56p mphillip
 * SW7231-367: Update tune_ofdm example
 * 
 * 9   6/10/11 11:12a mphillip
 * SW3461-5: Update OFDM example application
 * 
 * 8   3/22/10 2:35a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 7   12/11/09 4:09p erickson
 * SWDEPRECATED-2425: add printf to clarify cmdline usage
 *
 * 6   4/28/09 10:02a erickson
 * PR54218: update for current OFDM signal in lab headend
 *
 * 5   2/18/09 2:01p erickson
 * PR51841: remove examples_lib
 *
 * 5   8/5/08 1:14p erickson
 * PR45425: don't assume straight IB->PB mapping in example apps.
 *  explicitly map.
 *
 * 4   4/4/08 4:02p erickson
 * PR35551: separate qam and vsb tune examples
 *
 * 2   2/12/08 2:33p jgarrett
 * PR 39009: Unifying status values
 *
 *****************************************************************************/
/* Example to tune a OFDM channel using nexus */

#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(tune_ofdm);

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendFastStatus status;

    BSTD_UNUSED(param);
    NEXUS_Frontend_GetFastStatus(frontend, &status);
    printf("OFDM Lock callback: lockStatus=%d\n", status.lockStatus);
}

static void async_status_ready_callback(void *context, int param)
{
    BSTD_UNUSED(param);

    fprintf(stderr, "async_status_ready_callback\n"); 
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendOfdmSettings ofdmSettings;
    NEXUS_FrontendOfdmMode mode = NEXUS_FrontendOfdmMode_eDvbt;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAcquireSettings settings;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    unsigned i=0, j=0;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel, pcrPidChannel;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FrontendDvbt2StatusReady statusReady;
    NEXUS_FrontendDvbt2Status dvbt2Status;
    NEXUS_FrontendOfdmStatus status;
    /* default freq & qam mode */
    unsigned freq = 578;
    BKNI_EventHandle statusEvent;
    unsigned videopid = 0x579;
    unsigned audiopid = 0x57a;
    unsigned pcrpid = videopid;
    unsigned videoCodec = NEXUS_VideoCodec_eMpeg2;
    unsigned audioCodec = NEXUS_AudioCodec_eMpeg;
    
    /* allow cmdline freq & qam mode for simple test */
    if (argc > 1) {
        mode = atoi(argv[1]);
    }
    if (argc > 2) {
        freq = atoi(argv[2]);
        if (argc > 3) {
            videopid = atoi(argv[3]);
            if (argc > 4) {
                audiopid = atoi(argv[4]);
                if (argc > 5) {
                    videoCodec = atoi(argv[5]);                 
                    if (argc > 6) {
                        audioCodec = atoi(argv[6]);
                }
            }
        }
    }
    }

    printf("*********mode %d videopid %d audiopid %d videoCodec %d audioCodec %d\n", mode, videopid, audiopid, videoCodec, audioCodec);
    
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
    settings.capabilities.ofdm = true;
    frontend = NEXUS_Frontend_Acquire(&settings);
    if (!frontend) {
        fprintf(stderr, "Unable to find OFDM-capable frontend\n");
        return -1;
    }

    BKNI_CreateEvent(&statusEvent);

    NEXUS_Frontend_GetDefaultOfdmSettings(&ofdmSettings);
    
    if(mode == NEXUS_FrontendOfdmMode_eDvbt){
        freq = 578;
        videopid = 0x579;
        audiopid = 0x57a;
        pcrpid = 0x579;
        ofdmSettings.bandwidth = 8000000;
        videoCodec = NEXUS_VideoCodec_eMpeg2;
        audioCodec = NEXUS_AudioCodec_eMpeg;

        ofdmSettings.manualTpsSettings = false;
        ofdmSettings.pullInRange = NEXUS_FrontendOfdmPullInRange_eWide;
        ofdmSettings.cciMode = NEXUS_FrontendOfdmCciMode_eNone;;
    }
    else if(mode == NEXUS_FrontendOfdmMode_eDvbt2){
        freq = 602;
        videopid = 0x65;
        audiopid = 0x66;
        pcrpid = 0x65;
        ofdmSettings.bandwidth = 8000000;
        videoCodec = NEXUS_VideoCodec_eH264;
        audioCodec = NEXUS_AudioCodec_eAacPlus;
        
        ofdmSettings.dvbt2Settings.plpMode = false;
        ofdmSettings.dvbt2Settings.plpId = 0;     
    }
    else if(mode == NEXUS_FrontendOfdmMode_eIsdbt){
        freq = 473;
        videopid = 0x200;
        audiopid = 0x300;
        pcrpid = 0x150;
        ofdmSettings.bandwidth = 6000000;
        videoCodec = NEXUS_VideoCodec_eMpeg2;
        audioCodec = NEXUS_AudioCodec_eAac;
    }

    ofdmSettings.frequency = freq * 1000000;
    ofdmSettings.acquisitionMode = NEXUS_FrontendOfdmAcquisitionMode_eAuto;
    ofdmSettings.terrestrial = true;
    ofdmSettings.spectrum = NEXUS_FrontendOfdmSpectrum_eAuto;
    ofdmSettings.mode = mode;
    ofdmSettings.lockCallback.callback = lock_callback;
    ofdmSettings.lockCallback.context = frontend;
    ofdmSettings.asyncStatusReadyCallback.callback = async_status_ready_callback;
    ofdmSettings.asyncStatusReadyCallback.context = statusEvent;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    /* Map a parser band to the demod's input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    if (userParams.isMtsif) {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
    }
    else {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    }    
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    videoPidChannel = NEXUS_PidChannel_Open(parserBand, videopid, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, audiopid, NULL);
    pcrPidChannel = NEXUS_PidChannel_Open(parserBand, pcrpid, NULL);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = videoCodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);

    NEXUS_AudioOutput_AddInput( NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                                NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 ) {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput( NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                    NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
#if NEXUS_NUM_HDMI_OUTPUTS
        NEXUS_AudioOutput_AddInput( NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
                                    NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
#endif
    }
    else {
        NEXUS_AudioOutput_AddInput( NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                    NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_NUM_HDMI_OUTPUTS
        NEXUS_AudioOutput_AddInput( NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
                                    NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
    }

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
#endif

    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_Frontend_TuneOfdm(frontend, &ofdmSettings);

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

    printf("mode = %d\n",mode);

    if(mode == NEXUS_FrontendOfdmMode_eDvbt2){
        printf("Press enter to get Dvbt2 partial status.\n");
        getchar();
        for (i=0; i<NEXUS_FrontendDvbt2StatusType_eMax; i++){
            rc = NEXUS_Frontend_RequestDvbt2AsyncStatus(frontend, (NEXUS_FrontendDvbt2StatusType)i);
            if(rc){rc = BERR_TRACE(rc); goto done;}

            rc = BKNI_WaitForEvent(statusEvent, 1000);
            if(rc){rc = BERR_TRACE(rc); goto done;}

            rc =  NEXUS_Frontend_GetDvbt2AsyncStatusReady(frontend, &statusReady);
            if(rc){rc = BERR_TRACE(rc); goto done;}

            for (j=0; j<NEXUS_FrontendDvbt2StatusType_eMax; j++){
                if(statusReady.type[j]){
                printf("Retrieving status type %d.\n", j);
                    rc = NEXUS_Frontend_GetDvbt2AsyncStatus(frontend, (NEXUS_FrontendDvbt2StatusType)j, &dvbt2Status);
                    if(rc){rc = BERR_TRACE(rc); goto done;}             
                }
            }
            printf("Press enter to get next partial status.\n");
            getchar();
        }   
    }

    printf("Press enter to get Ofdm sync status.\n");
    printf("NOTE: This api for DVBT2 will soon be depricated.\n");
    getchar();

    rc = NEXUS_Frontend_GetOfdmStatus(frontend, &status);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    printf("Synchronous Ofdm lock = %d\n", status.fecLock);     
    
    printf("Press enter to get async status.\n");
    printf("NOTE: This api for DVBT2 will soon be depricated.\n");
    getchar();

    rc = NEXUS_Frontend_RequestOfdmAsyncStatus(frontend);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BKNI_WaitForEvent(statusEvent, 2000);
    if(rc == NEXUS_TIMEOUT) {
        printf("Status not returned.\n");
    }
    
    rc = NEXUS_Frontend_GetOfdmAsyncStatus(frontend , &status);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    printf("Asynchronous Ofdm lock = %d, receiver lock = %d\n", status.fecLock, status.receiverLock);

    printf("Press enter to get exit.\n");
    getchar();

done:
    /* example shutdown */
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        NEXUS_AudioDecoder_Stop(compressedDecoder);
    }

    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_PidChannel_Close(pcrPidChannel);

    NEXUS_Frontend_Untune(frontend);
    if (statusEvent) {
        BKNI_DestroyEvent(statusEvent);
    }

    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]));
#endif
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Frontend_Release(frontend);
    NEXUS_Platform_Uninit();
    return 0;
}

