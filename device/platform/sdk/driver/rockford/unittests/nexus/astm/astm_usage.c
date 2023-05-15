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
 * $brcm_Workfile: astm_usage.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/23/09 4:23p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/astm/astm_usage.c $
 * 
 * Hydra_Software_Devel/1   1/23/09 4:23p jtna
 * PR51390: added astm_usage test
 * 
******************************************************************************/
/* Nexus unittest for ASTM module usage  */
/* build it like this: make APPS=astm_usage NEXUS_ASTM_TEST_SUPPORT=n install */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_video_input.h"
#include "nexus_audio_input.h"
#if NEXUS_HAS_ASTM
#include "nexus_astm.h"
#endif

#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#if NEXUS_NUM_DISPLAYS > 1
#define HDSD_SIMUL 1
#endif

typedef enum AVDecoders
{
    AVDecoders_eVideoOnly,
    AVDecoders_eAudioOnly,
    AVDecoders_eBoth
} AVDecoders;

typedef struct pidPair
{
    unsigned vpid;
    unsigned apid;
} pidPair;

typedef struct pidChannelHandlePair
{
    NEXUS_PidChannelHandle vpid;
    NEXUS_PidChannelHandle apid;
} pidChannelHandlePair;

NEXUS_VideoDecoderHandle vdecode;
NEXUS_VideoDecoderStartSettings videoProgram;
NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
NEXUS_AudioDecoderStartSettings audioProgram;

AVDecoders StartDecoders(AVDecoders option) 
{
    if (option==AVDecoders_eVideoOnly || option==AVDecoders_eBoth) {
        NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    }
    if (option==AVDecoders_eAudioOnly || option==AVDecoders_eBoth) {
        NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
        if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
            NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram); /* Only pass through AC3 */
        }
    }
    return option;
}

AVDecoders StopDecoders(AVDecoders option) 
{
    if (option==AVDecoders_eVideoOnly || option==AVDecoders_eBoth) {
        NEXUS_VideoDecoder_Stop(vdecode);
    }
    if (option==AVDecoders_eAudioOnly || option==AVDecoders_eBoth) {
        NEXUS_AudioDecoder_Stop(pcmDecoder);
        NEXUS_AudioDecoder_Stop(compressedDecoder);
    }
    return option;
}

void ModifyAstmSettings(NEXUS_AstmSettings *settings, unsigned i, unsigned j, unsigned k, unsigned l)
{
    settings->presentationRateControl = i;
    settings->stcSource = j;
    settings->clockCoupling = k;
    switch(l) {
        case 0: settings->stcMaster = vdecode; break;
        case 1: settings->stcMaster = pcmDecoder; break;
        case 2: settings->stcMaster = compressedDecoder; break;
        default: BDBG_ASSERT(0); break;
    }
}

void RandomizeAstmSettings(NEXUS_AstmSettings *settings)
{
    settings->presentationRateControl = rand()%NEXUS_AstmPresentationRateControl_eMax;
    settings->stcSource = rand()%NEXUS_AstmStcSource_eMax;
    settings->clockCoupling = rand()%NEXUS_AstmClockCoupling_eMax;
    switch(rand()%(1+NEXUS_ASTM_AUDIO_DECODERS)) {
        case 0: settings->stcMaster = vdecode; break;
        case 1: settings->stcMaster = pcmDecoder; break;
        case 2: settings->stcMaster = compressedDecoder; break;
        default: break;
    }
}

#define SWITCH_PID_CHANNELS(pid) \
    videoProgram.pidChannel = pidChannelHandles[pid].vpid; \
    audioProgram.pidChannel = pidChannelHandles[pid].apid; \
    NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings); \
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; \
    stcSettings.mode = (pid<2)?NEXUS_StcChannelMode_ePcr /* live */:NEXUS_StcChannelMode_eAuto; /* playback */ \
    NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);

unsigned g_testcase;
#define PRINT_CURRENT_SETTINGS() \
    printf(" >TestCase%d: RateControl(i)=%d, StcSource(j)=%d, ClockCoupling(k)=%d, stcMaster=%s\n", \
    g_testcase,i,j,k,astmSettings.stcMaster==vdecode?"vdecode":(astmSettings.stcMaster==pcmDecoder?"pcm":"compressed"));

#define TEST_PLAYBACK 1 /* leave this at 1. this just indicates which portion of code has to do with getting playback to work */

int main(void)
{
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display0;
    NEXUS_VideoWindowHandle window0;
    #if HDSD_SIMUL
    NEXUS_DisplayHandle display1;
    NEXUS_VideoWindowHandle window1;
    #endif
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_StcChannelHandle stcChannel, stcChannelBogus, stcChannelHandles[3]; /* when doing a pid switch, stcChannel is modified via NEXUS_StcChannel_Get/SetSettings. that's one testcase. using stcChannelHandles[] is another */
#if NEXUS_HAS_ASTM
    NEXUS_AstmSettings astmSettings;
    NEXUS_AstmHandle astm;
#endif
    NEXUS_Error rc;
    unsigned i,j,k,l;
    NEXUS_VideoDecoderStatus videoStatus;
    NEXUS_AudioDecoderStatus audioStatus;
    pidPair pids[3];
    pidChannelHandlePair pidChannelHandles[3];
    NEXUS_PidChannelStatus pidStatus;
#if TEST_PLAYBACK
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    const char *fname = "videos/cnnticker.mpg";
    pids[2].vpid = 0x21;
    pids[2].apid = 0x22;
#endif
    /* italyriviera_spiderman2_cc_q64.mpg */
    pids[0].vpid = 0x11; /* spiderman2 */
    pids[0].apid = 0x14;
    pids[1].vpid = 0x31; /* italyriviera */
    pids[1].apid = 0x34;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings); 
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if TEST_PLAYBACK
    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);
#endif    
    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */    
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    
    /* Open the pid channels */
    pidChannelHandles[0].vpid = NEXUS_PidChannel_Open(parserBand, pids[0].vpid, NULL);
    pidChannelHandles[0].apid = NEXUS_PidChannel_Open(parserBand, pids[0].apid, NULL);
    pidChannelHandles[1].vpid = NEXUS_PidChannel_Open(parserBand, pids[1].vpid, NULL);
    pidChannelHandles[1].apid = NEXUS_PidChannel_Open(parserBand, pids[1].apid, NULL);
    pidChannelHandles[2].vpid = NEXUS_Playback_OpenPidChannel(playback, pids[2].vpid, NULL);
    pidChannelHandles[2].apid = NEXUS_Playback_OpenPidChannel(playback, pids[2].apid, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = pidChannelHandles[0].vpid;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = pidChannelHandles[0].apid;

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);              
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    audioProgram.stcChannel = videoProgram.stcChannel = stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
    BDBG_ASSERT(stcChannel);

    /* Create a bogus StcChannel for use later */
    stcSettings.timebase = NEXUS_Timebase_e0; /* TODO: use different timebase? */
    stcSettings.modeSettings.pcr.pidChannel = NEXUS_PidChannel_Open(parserBand, 99, NULL); /* specify a non-existent PCR pid here */
    stcChannelBogus = NEXUS_StcChannel_Open(1, &stcSettings);
    BDBG_ASSERT(stcChannelBogus);

    /* store all stcChannel handles to test setting the astmSettings.stcChannel to an existent but false (ie. belonging to another program) pid */
    stcChannelHandles[0] = stcChannel;
    stcSettings.modeSettings.pcr.pidChannel = pidChannelHandles[1].vpid;
    stcChannelHandles[1] = NEXUS_StcChannel_Open(2, &stcSettings);
    BDBG_ASSERT(stcChannelHandles[1]);
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* playback */
    stcSettings.modeSettings.pcr.pidChannel = pidChannelHandles[2].vpid;
    stcChannelHandles[2] = NEXUS_StcChannel_Open(3, &stcSettings);
    BDBG_ASSERT(stcChannelHandles[2]);

    /* Bring up audio decoders and connect to outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
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
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display0 = NEXUS_Display_Open(0, &displaySettings);
    window0 = NEXUS_VideoWindow_Open(display0, 0);
    NEXUS_Display_AddOutput(display0, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));

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
    NEXUS_Astm_GetDefaultSettings(&astmSettings);
    astmSettings.stcChannel = stcChannel;
    astmSettings.videoDecoder = vdecode;
    astmSettings.audioDecoder[0] = pcmDecoder;
    astmSettings.audioDecoder[1] = compressedDecoder;
    astmSettings.stcMaster = pcmDecoder; /* audio master is preferred */
#endif


/**********************************************************
*
* Beginning of Tests 
*
**********************************************************/

#define ENABLE_TESTCASE0  1
#define ENABLE_TESTCASE1  1
#define ENABLE_TESTCASE2  1
#define ENABLE_TESTCASE3  1
#define ENABLE_TESTCASE4  1
#define ENABLE_TESTCASE5  1
#define ENABLE_TESTCASE6  1
#define ENABLE_TESTCASE7  1
#define ENABLE_TESTCASE8  1
#define ENABLE_TESTCASE9  1
#define ENABLE_TESTCASE10 1

#define SLEEP_PERIOD 1500 /* the sleep period between each step in a testcase */
#define MANUAL_TEST 0 /* if set to 1, requires manual user input for some testcases to continue. this helps monitor some tests. 
                         TODO: ideally the monitoring for unexpected behavior would be automated  */

#if (MANUAL_TEST==1)
#define TEST_PAUSE() getchar();
#else
#define TEST_PAUSE() BKNI_Sleep(SLEEP_PERIOD);
#endif

/* TestCase0: Basic decode with ASTM */
#if ENABLE_TESTCASE0
    g_testcase = 0;
    astm = NEXUS_Astm_Create(&astmSettings);

    /* sanity check on a bunch of stuff */
    BDBG_ASSERT(videoProgram.codec == NEXUS_VideoCodec_eMpeg2);
    NEXUS_PidChannel_GetStatus(videoProgram.pidChannel, &pidStatus);
    BDBG_ASSERT(pidStatus.pid == 0x11);
    BDBG_ASSERT(audioProgram.codec == NEXUS_AudioCodec_eAc3);
    NEXUS_PidChannel_GetStatus(audioProgram.pidChannel, &pidStatus);
    BDBG_ASSERT(pidStatus.pid == 0x14);
    NEXUS_StcChannel_GetSettings(videoProgram.stcChannel, &stcSettings);
    BDBG_ASSERT(stcSettings.modeSettings.pcr.pidChannel == videoProgram.pidChannel);
    BDBG_ASSERT(stcSettings.mode == NEXUS_StcChannelMode_ePcr);
   
    StartDecoders(AVDecoders_eBoth);
    BKNI_Sleep(SLEEP_PERIOD*10);
    /* TODO: verify ASTM active */
    StopDecoders(AVDecoders_eBoth);
    
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase1: Test astmSettings.enabled and astmSettings.enableAutomaticLifecycleControl */
#if ENABLE_TESTCASE1
    g_testcase = 1;
    astmSettings.enabled = false;
    astm = NEXUS_Astm_Create(&astmSettings);
    StartDecoders(AVDecoders_eBoth);
    BKNI_Sleep(SLEEP_PERIOD);
    /* TODO: verify ASTM inactive */
    StopDecoders(AVDecoders_eBoth);
    
    astmSettings.enabled = true;
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    StartDecoders(AVDecoders_eBoth);
    BKNI_Sleep(SLEEP_PERIOD); /* wait until ASTM becomes active */
    astmSettings.enabled = false;
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    /* TODO: verify ASTM inactive */
    StopDecoders(AVDecoders_eBoth);

    #if 0 /* TODO: test enableAutomaticLifecycleControl thoroughly once public getStatus is available */    
    astmSettings.enabled = true;  
    astmSettings.enableAutomaticLifecycleControl = false;
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    StartDecoders(AVDecoders_eBoth);
    BKNI_Sleep(SLEEP_PERIOD);
    /* TODO: verify ASTM inactive */
    StopDecoders(AVDecoders_eBoth);

    rc = NEXUS_Astm_Start(astm); /* now start ASTM manually */
    BDBG_ASSERT(!rc);
    /* TODO: what happens if you query ASTM now, before decoders are started? */
    /* TODO: verify ASTM inactive */
    StartDecoders(AVDecoders_eVideoOnly);
    /* TODO: verify ASTM active */
    StartDecoders(AVDecoders_eAudioOnly);
    /* TODO: verify ASTM active */
    StopDecoders(AVDecoders_eVideoOnly);
    /* TODO: verify ASTM active */
    StopDecoders(AVDecoders_eAudioOnly);
    /* TODO: verify ASTM active */

    /* TODO: enableAutomaticLifecycleControl should be changeable on the fly */
    #endif
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase2: Cycle through all enums in settings and use these with NEXUS_Astm_Create() */
#if ENABLE_TESTCASE2
    g_testcase = 2;
    for (i=0; i<NEXUS_AstmPresentationRateControl_eMax; i++) {
        for (j=0; j<NEXUS_AstmStcSource_eMax; j++) {
            for (k=0; k<NEXUS_AstmClockCoupling_eMax; k++) {
                for (l=0; l<1+NEXUS_ASTM_AUDIO_DECODERS; l++) {
                    ModifyAstmSettings(&astmSettings, i, j, k, l);
                    PRINT_CURRENT_SETTINGS();
                    astm = NEXUS_Astm_Create(&astmSettings);
                    BDBG_ASSERT(astm);

                    StartDecoders(AVDecoders_eBoth);
                    BKNI_Sleep(SLEEP_PERIOD);

                    StopDecoders(AVDecoders_eBoth);
                    NEXUS_Astm_Destroy(astm);
                }
            }
        }
    }
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase3: Cycle through all enums in settings and use these with NEXUS_Astm_SetSettings() while decoders are running */
#if ENABLE_TESTCASE3
    g_testcase = 3;
    astm = NEXUS_Astm_Create(&astmSettings);
    StartDecoders(AVDecoders_eBoth);

    for (l=0; l<1+NEXUS_ASTM_AUDIO_DECODERS; l++) {
        for (k=0; k<NEXUS_AstmClockCoupling_eMax; k++) {
            for (j=0; j<NEXUS_AstmStcSource_eMax; j++) {
                for (i=0; i<NEXUS_AstmPresentationRateControl_eMax; i++) {
                    ModifyAstmSettings(&astmSettings, i, j, k, l);
                    PRINT_CURRENT_SETTINGS();
                    rc = NEXUS_Astm_SetSettings(astm, &astmSettings);
                    BDBG_ASSERT(!rc);
                    BKNI_Sleep(SLEEP_PERIOD);
                }
            }
        }
    }
    StopDecoders(AVDecoders_eBoth);
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase4: Similar to TestCase2, but stop decoders first before NEXUS_Astm_SetSettings(), then start decoders again */
#if ENABLE_TESTCASE4
    g_testcase = 4;
    astm = NEXUS_Astm_Create(&astmSettings);
    StartDecoders(AVDecoders_eBoth);

    for (k=0; k<NEXUS_AstmClockCoupling_eMax; k++) {
        for (l=0; l<1+NEXUS_ASTM_AUDIO_DECODERS; l++) {
            for (i=0; i<NEXUS_AstmPresentationRateControl_eMax; i++) {
                for (j=0; j<NEXUS_AstmStcSource_eMax; j++) {
                    ModifyAstmSettings(&astmSettings, i, j, k, l);
                    StopDecoders(AVDecoders_eBoth);
                    PRINT_CURRENT_SETTINGS();
                    rc = NEXUS_Astm_SetSettings(astm, &astmSettings);
                    BDBG_ASSERT(!rc);
                    StartDecoders(AVDecoders_eBoth);
                    BKNI_Sleep(SLEEP_PERIOD);

                    RandomizeAstmSettings(&astmSettings);
                    rc = NEXUS_Astm_SetSettings(astm, &astmSettings);
                    BDBG_ASSERT(!rc);
                    BKNI_Sleep(SLEEP_PERIOD);
                }
            }
        }
    }
    StopDecoders(AVDecoders_eBoth);
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase5: 
start both decoders -> stop one decoder -> set random settings -> stop the other decoder -> 
set random settings -> start one decoder -> set random settings -> start the other decoder -> set settings */
#if ENABLE_TESTCASE5
    g_testcase = 5;
    astm = NEXUS_Astm_Create(&astmSettings);
    StartDecoders(AVDecoders_eBoth);

    for (k=0; k<NEXUS_AstmClockCoupling_eMax; k++) {
        for (l=0; l<1+NEXUS_ASTM_AUDIO_DECODERS; l++) {
            for (i=0; i<NEXUS_AstmPresentationRateControl_eMax; i++) {
                for (j=0; j<NEXUS_AstmStcSource_eMax; j++) {
                    AVDecoders av;
                    if (rand()%2) { /* stop one of the decoders */
                        av = StopDecoders(AVDecoders_eVideoOnly);
                    }
                    else {
                        av = StopDecoders(AVDecoders_eAudioOnly);
                    }
                    RandomizeAstmSettings(&astmSettings);
                    PRINT_CURRENT_SETTINGS();
                    rc = NEXUS_Astm_SetSettings(astm, &astmSettings);
                    BDBG_ASSERT(!rc);
                    BKNI_Sleep(SLEEP_PERIOD);

                    StopDecoders(av==AVDecoders_eVideoOnly?AVDecoders_eAudioOnly:AVDecoders_eVideoOnly); /* stop the other decoder */
                    RandomizeAstmSettings(&astmSettings);
                    PRINT_CURRENT_SETTINGS();
                    rc = NEXUS_Astm_SetSettings(astm, &astmSettings);
                    BDBG_ASSERT(!rc);
                    BKNI_Sleep(SLEEP_PERIOD);

                    if (rand()%2) { /* start one of the decoders */
                        av = StartDecoders(AVDecoders_eVideoOnly);
                    }
                    else {
                        av = StartDecoders(AVDecoders_eAudioOnly);
                    }
                    RandomizeAstmSettings(&astmSettings);
                    PRINT_CURRENT_SETTINGS();
                    rc = NEXUS_Astm_SetSettings(astm, &astmSettings);
                    BDBG_ASSERT(!rc);
                    BKNI_Sleep(SLEEP_PERIOD);

                    StartDecoders(av==AVDecoders_eVideoOnly?AVDecoders_eAudioOnly:AVDecoders_eVideoOnly); /* start the other decoder */
                    ModifyAstmSettings(&astmSettings, i, j, k, l);
                    PRINT_CURRENT_SETTINGS();
                    rc = NEXUS_Astm_SetSettings(astm, &astmSettings); 
                    BDBG_ASSERT(!rc);
                    BKNI_Sleep(SLEEP_PERIOD);

                }
            }
        }
    }
    StopDecoders(AVDecoders_eBoth);
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase6: Switching between two live decodes normally:
set astm settings for decode0 -> start decode0 -> stop decode0 -> 
set astm settings for decode1 -> start decode1 -> stop decode1 -> repeat */
#if ENABLE_TESTCASE6
    g_testcase = 6;
    astm = NEXUS_Astm_Create(&astmSettings);

    for (i=0; i<3; i++) {
        StartDecoders(AVDecoders_eBoth);
        BKNI_Sleep(SLEEP_PERIOD);
        StopDecoders(AVDecoders_eBoth);

        /* switch to other PID */
        SWITCH_PID_CHANNELS(1);

        StartDecoders(AVDecoders_eBoth);
        BKNI_Sleep(SLEEP_PERIOD);
        StopDecoders(AVDecoders_eBoth);

        /* switch back to other PID */
        SWITCH_PID_CHANNELS(0);
    }
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif

    /* sanity check on stcChannelHandles we have created, before moving on */
    NEXUS_StcChannel_GetSettings(stcChannelHandles[0], &stcSettings);
    BDBG_ASSERT(stcSettings.modeSettings.pcr.pidChannel == pidChannelHandles[0].vpid);
    BDBG_ASSERT(stcSettings.mode == NEXUS_StcChannelMode_ePcr);
    NEXUS_StcChannel_GetSettings(stcChannelHandles[1], &stcSettings);
    BDBG_ASSERT(stcSettings.modeSettings.pcr.pidChannel == pidChannelHandles[1].vpid);
    BDBG_ASSERT(stcSettings.mode == NEXUS_StcChannelMode_ePcr);
    NEXUS_StcChannel_GetSettings(stcChannelHandles[2], &stcSettings);
    BDBG_ASSERT(stcSettings.modeSettings.pcr.pidChannel == pidChannelHandles[2].vpid);
    BDBG_ASSERT(stcSettings.mode == NEXUS_StcChannelMode_eAuto);



/* TestCase 7: Attempt to change to a bogus stcChannel, during decode and between decode start/stop. 
Try the same thing with a false stcChannel. A false stChannel refers to a stcChannel mapped to a pcr pid belonging to a different program.
This test ideally requires manual monitoring of A/V output for no unexpected behavior */
#if ENABLE_TESTCASE7
    g_testcase = 7;
    astm = NEXUS_Astm_Create(&astmSettings);
    NEXUS_Astm_GetSettings(astm, &astmSettings);
    printf(" >TestCase%d: Manual monitoring recommended for this testcase\n", g_testcase);
    TEST_PAUSE();

    SWITCH_PID_CHANNELS(0);
    StartDecoders(AVDecoders_eBoth);
    BKNI_Sleep(SLEEP_PERIOD);
    StopDecoders(AVDecoders_eBoth);

    /* set to bogus stcChannel and start decode */
    astmSettings.stcChannel = stcChannelBogus;
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    StartDecoders(AVDecoders_eBoth);
    printf(" >TestCase%d: set bogus stcChannel and start decode\n", g_testcase);
    TEST_PAUSE();

    /* set back to correct stcChannel during decode */
    astmSettings.stcChannel = stcChannel;
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    printf(" >TestCase%d: set back to correct stcChannel during decode\n", g_testcase);
    TEST_PAUSE();
    StopDecoders(AVDecoders_eBoth);

    /* start decode again and set to bogus stcChannel _during_ decode */
    StartDecoders(AVDecoders_eBoth);
    printf(" >TestCase%d: stop, then start decode with correct stcChannel\n", g_testcase);
    TEST_PAUSE();
    astmSettings.stcChannel = stcChannelBogus;
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    printf(" >TestCase%d: set bogus stcChannel during decode\n", g_testcase);
    TEST_PAUSE();

    /* set back to correct stcChannel during decode */
    astmSettings.stcChannel = stcChannel;
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    printf(" >TestCase%d: set back to correct stcChannel during decode\n", g_testcase);
    TEST_PAUSE();
    StopDecoders(AVDecoders_eBoth);

    /* now repeat the above with a false stcChannel */

    /* set to false stcChannel and start decode */
    astmSettings.stcChannel = stcChannelHandles[1];
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    StartDecoders(AVDecoders_eBoth);
    printf(" >TestCase%d: set false stcChannel and start decode\n", g_testcase);
    TEST_PAUSE();

    /* set back to correct stcChannel during decode */
    BDBG_ASSERT(stcChannelHandles[0]==stcChannel);
    astmSettings.stcChannel = stcChannelHandles[0];
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    printf(" >TestCase%d: set back to correct stcChannel during decode\n", g_testcase);
    TEST_PAUSE();
    StopDecoders(AVDecoders_eBoth);

    /* start decode again and set to false stcChannel _during_ decode */
    StartDecoders(AVDecoders_eBoth);
    printf(" >TestCase%d: stop, then start decode with correct stcChannel\n", g_testcase);
    TEST_PAUSE();
    astmSettings.stcChannel = stcChannelHandles[1];
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    printf(" >TestCase%d: set false stcChannel during decode\n", g_testcase);
    TEST_PAUSE();

    /* set back to correct stcChannel during decode */
    astmSettings.stcChannel = stcChannelHandles[0];
    NEXUS_Astm_SetSettings(astm, &astmSettings);
    printf(" >TestCase%d: set back to correct stcChannel during decode\n", g_testcase);
    TEST_PAUSE();
    StopDecoders(AVDecoders_eBoth);

    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase8: Switching between two live decodes abnormally (incorrect astm settings -> start decode -> correct settings -> incorrect astm settings -> stop decode):
set stcChannel for decode1 -> start decode0 -> set stcChannel for decode0 -> set stcChannel for decode1 -> stop decode0 -> 
set stcChannel for decode0 -> start decode1 -> set stcChannel for decode1 -> set stcChannel for decode0 -> stop decode1 -> repeat */
#if ENABLE_TESTCASE8
    g_testcase = 8;
    astm = NEXUS_Astm_Create(&astmSettings);
    NEXUS_Astm_GetSettings(astm, &astmSettings);
    printf(" >TestCase%d: Manual monitoring recommended for this testcase\n", g_testcase);
    TEST_PAUSE();

    for (i=0; i<3; i++) {
        /* set stcChannel for decode1 and start decode0 */
        SWITCH_PID_CHANNELS(0);
        astmSettings.stcChannel = stcChannelHandles[1];
        NEXUS_Astm_SetSettings(astm, &astmSettings);
        StartDecoders(AVDecoders_eBoth);
        printf(" >TestCase%d: stop decode, switch pids, set false stcChannel and start decode\n", g_testcase);
        TEST_PAUSE();

        /* set correct stcChannel */
        astmSettings.stcChannel = stcChannelHandles[0];
        NEXUS_Astm_SetSettings(astm, &astmSettings);
        printf(" >TestCase%d: set correct stcChannel during decode\n", g_testcase);
        TEST_PAUSE();

        /* set false stcChannel */
        astmSettings.stcChannel = stcChannelHandles[1];
        NEXUS_Astm_SetSettings(astm, &astmSettings);
        printf(" >TestCase%d: set false stcChannel during decode\n", g_testcase);
        TEST_PAUSE();
      
        StopDecoders(AVDecoders_eBoth);
        /* switch to other PID */
        SWITCH_PID_CHANNELS(1);
        /* set stcChannel for decode0 and start decode1 */
        astmSettings.stcChannel = stcChannelHandles[0];
        NEXUS_Astm_SetSettings(astm, &astmSettings);
        StartDecoders(AVDecoders_eBoth);
        printf(" >TestCase%d: stop decode, switch pids, set false stcChannel and start decode\n", g_testcase);
        TEST_PAUSE();

        /* set correct stcChannel */
        astmSettings.stcChannel = stcChannelHandles[1];
        NEXUS_Astm_SetSettings(astm, &astmSettings);
        printf(" >TestCase%d: set correct stcChannel during decode\n", g_testcase);
        TEST_PAUSE();

        /* set false stcChannel */
        astmSettings.stcChannel = stcChannelHandles[0];
        NEXUS_Astm_SetSettings(astm, &astmSettings);
        printf(" >TestCase%d: set false stcChannel during decode\n", g_testcase);
        TEST_PAUSE();

        StopDecoders(AVDecoders_eBoth);
    }
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase9: Switching between decode and playback: decode -> playback -> repeat*/
#if ENABLE_TESTCASE9
    g_testcase = 9;
    astm = NEXUS_Astm_Create(&astmSettings);
    printf(" >TestCase%d: Manual monitoring recommended for this testcase\n", g_testcase);
    TEST_PAUSE();

    for (i=0; i<3; i++) {
        /* switch to live decode */
        videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
        audioProgram.codec = NEXUS_AudioCodec_eAc3;
        SWITCH_PID_CHANNELS(rand()%2); /* one of the live decodes, randomly */
        StartDecoders(AVDecoders_eBoth);
        printf(" >TestCase%d: start decode\n", g_testcase);
        TEST_PAUSE();
        StopDecoders(AVDecoders_eBoth);

        /* switch to playback */
        videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
        audioProgram.codec = NEXUS_AudioCodec_eMpeg;
        SWITCH_PID_CHANNELS(2);
        StartDecoders(AVDecoders_eBoth);
        NEXUS_Playback_Start(playback, file, NULL);
        printf(" >TestCase%d: start playback\n", g_testcase);
        TEST_PAUSE();
        NEXUS_Playback_Stop(playback);
        StopDecoders(AVDecoders_eBoth);
    }
    
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif



/* TestCase10: Setting decoder into vsync mode, and back into TSM mode */
#if ENABLE_TESTCASE10
    g_testcase = 10;
    astm = NEXUS_Astm_Create(&astmSettings);
    /* start in TSM mode */
    SWITCH_PID_CHANNELS(0);
    BDBG_ASSERT(videoProgram.stcChannel==stcChannel);
    BDBG_ASSERT(audioProgram.stcChannel==stcChannel);
    StartDecoders(AVDecoders_eBoth);
    /* TODO: verify that we are in TSM mode and ASTM is active */

    /* video vsync mode */
    StopDecoders(AVDecoders_eVideoOnly);
    videoProgram.stcChannel = NULL;
    StartDecoders(AVDecoders_eVideoOnly);
    NEXUS_VideoDecoder_GetStatus(vdecode, &videoStatus);
    BDBG_ASSERT(videoStatus.tsm == false);
    /* TODO: verify that ASTM is inactive */
    TEST_PAUSE();
    StopDecoders(AVDecoders_eBoth);
    videoProgram.stcChannel = stcChannel; /* restore TSM mode */
    StartDecoders(AVDecoders_eBoth);
    
    /* audio vsync mode */
    StopDecoders(AVDecoders_eAudioOnly);
    audioProgram.stcChannel = NULL;
    StartDecoders(AVDecoders_eAudioOnly);
    NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
    BDBG_ASSERT(audioStatus.tsm == false);
    /* TODO: verify that ASTM is inactive */
    TEST_PAUSE();
    StopDecoders(AVDecoders_eBoth);
    audioProgram.stcChannel = stcChannel; /* restore TSM mode */
    StartDecoders(AVDecoders_eBoth);

    /* a+v vsync mode */
    StopDecoders(AVDecoders_eBoth);    
    videoProgram.stcChannel = NULL;
    audioProgram.stcChannel = NULL;
    StartDecoders(AVDecoders_eBoth);
    /* TODO: verify that ASTM is inactive */
    TEST_PAUSE();
    StopDecoders(AVDecoders_eBoth);
    NEXUS_Astm_Destroy(astm);
    printf(" >Completed TestCase%d\n", g_testcase);
#endif

    /* shutdown gracefully */
    NEXUS_FilePlay_Close(file);
    NEXUS_PidChannel_CloseAll(parserBand);
    NEXUS_Playback_CloseAllPidChannels(playback);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_Display_Close(display0);
#if HDSD_SIMUL
    NEXUS_Display_Close(display1);
#endif
    NEXUS_Platform_Uninit();

    return 0;
} 

