/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: incorrect_codec.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/14/09 3:31p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/astm/incorrect_codec.c $
 * 
 * Hydra_Software_Devel/1   5/14/09 3:31p jtna
 * PR54883: added incorrect_codec
 * 
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
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
#include "nexus_spdif_output.h"
#if NEXUS_NUM_COMPONENT_OUTPUTS
#include "nexus_component_output.h"
#endif
#include "nexus_composite_output.h"
#if NEXUS_HAS_ASTM
#include "nexus_astm.h"
#endif
#include "nexus_playback.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#if !NEXUS_DTV_PLATFORM && NEXUS_NUM_DISPLAYS > 1
#define HDSD_SIMUL 1
#endif

#define FILENAME "videos/discoveryAvcHD.mpg"
#define VIDEO_CODEC NEXUS_VideoCodec_eVc1 /* correct codec is NEXUS_VideoCodec_eH264. specify an incorrect codec on purpose */
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x1522
#define AUDIO_PID 0x1523

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display0;
    NEXUS_VideoWindowHandle window0;
    #if HDSD_SIMUL
    NEXUS_DisplayHandle display1;
    NEXUS_VideoWindowHandle window1;
    #endif
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_StcChannelHandle stcChannel;
#if NEXUS_HAS_ASTM
    NEXUS_AstmSettings astmSettings;
    NEXUS_AstmHandle astm;
#endif
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidCfg;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_FilePlayHandle file;
    NEXUS_AudioDecoderStatus audioStatus;
    NEXUS_VideoDecoderStatus videoStatus;
    unsigned cnt = 0;
    uint32_t lastPts;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings); 
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Open file */
    printf("\nOpening %s\n", FILENAME);
    file = NEXUS_FilePlay_OpenPosix(FILENAME, NULL);
    BDBG_ASSERT(file);
    
    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */    
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    
    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);              
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Open playpump and playback */
    playpump = NEXUS_Playpump_Open(0, NULL);
    playback = NEXUS_Playback_Create();
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* Bring up audio decoders and connect to outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( AUDIO_CODEC == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                   NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                   NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

    /* bring up HD/SD simul display */
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display0 = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display0, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display0 = NEXUS_Display_Open(0, &displaySettings);
    window0 = NEXUS_VideoWindow_Open(display0, 0);
    NEXUS_Display_AddOutput(display0, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif

#if HDSD_SIMUL
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display1 = NEXUS_Display_Open(1, &displaySettings);
    window1 = NEXUS_VideoWindow_Open(display1, 0);
    NEXUS_Display_AddOutput(display1, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
    
    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window0, NEXUS_VideoDecoder_GetConnector(vdecode));
#if HDSD_SIMUL
    NEXUS_VideoWindow_AddInput(window1, NEXUS_VideoDecoder_GetConnector(vdecode));
#endif
    
    /* set frame rate master */    
    NEXUS_Display_GetSettings(display0, &displaySettings);
    displaySettings.frameRateMaster = NEXUS_VideoDecoder_GetConnector(vdecode);
    NEXUS_Display_SetSettings(display0, &displaySettings);    

#if NEXUS_HAS_ASTM
    /* create an astm instance and connect */
    NEXUS_Astm_GetDefaultSettings(&astmSettings);
    astmSettings.stcChannel = stcChannel;
    astmSettings.videoDecoder = vdecode;
    astmSettings.audioDecoder[0] = pcmDecoder;
    astmSettings.audioDecoder[1] = compressedDecoder;
#if 1
    astmSettings.stcMaster = pcmDecoder; /* audio master is preferred */
#else
    astmSettings.stcMaster = vdecode;
#endif
    astm = NEXUS_Astm_Create(&astmSettings);
#endif

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
    playbackPidCfg.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidCfg.pidTypeSettings.video.codec = VIDEO_CODEC;
    playbackPidCfg.pidTypeSettings.video.decoder = vdecode;
    playbackPidCfg.pidTypeSettings.video.index = true;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, VIDEO_PID, &playbackPidCfg);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
    playbackPidCfg.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidCfg.pidTypeSettings.audio.primary = pcmDecoder;
    playbackPidCfg.pidTypeSettings.audio.secondary = compressedDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, AUDIO_PID, &playbackPidCfg);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( AUDIO_CODEC == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

#if NEXUS_HAS_ASTM
    NEXUS_Astm_Start(astm); /* start me up */
#endif

    rc = NEXUS_Playback_Start(playback, file, NULL);
    BDBG_ASSERT(!rc);

    while (1) {
        BKNI_Sleep(500);
        NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
        NEXUS_VideoDecoder_GetStatus(vdecode, &videoStatus);

        printf("[%3d] aPTS=0x%08x, aFIFO=%2d%% (vFIFO=%2d%%)\n", cnt++, audioStatus.pts,
            audioStatus.fifoSize ? audioStatus.fifoDepth*100/audioStatus.fifoSize : 0,
            videoStatus.fifoSize ? videoStatus.fifoDepth*100/videoStatus.fifoSize : 0);

        lastPts = audioStatus.pts;
        if (cnt > 50) {
            /* for now, just exit after a while */
            break;
        }
    }

    /* shutdown */
    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    NEXUS_AudioDecoder_Stop(compressedDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_Astm_Stop(astm);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Astm_Destroy(astm);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));    
    NEXUS_VideoWindow_RemoveAllInputs(window0);
    NEXUS_VideoWindow_RemoveAllInputs(window1);
    NEXUS_VideoWindow_Close(window0);
    NEXUS_VideoWindow_Close(window1);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_Display_Close(display0);
    NEXUS_Display_Close(display1);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();
    return 0;
}

