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
 * $brcm_Workfile: standby_transition.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 5/30/12 6:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/power_management/standby_transition.c $
 * 
 * Hydra_Software_Devel/4   5/30/12 6:14p jtna
 * SW7425-2681: added -loops cmdline option
 * 
 * Hydra_Software_Devel/3   4/3/12 4:18p jtna
 * SW7425-2681: fix unmount/remount
 * 
 * Hydra_Software_Devel/2   4/2/12 3:48p jtna
 * SW7425-2681: wrap with #if NEXUS_POWER_MANAGEMENT
 * 
 * Hydra_Software_Devel/1   3/29/12 6:22p jtna
 * SW7425-2681: add power_management/standby_transition.c
 * 
 *****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "pmlib-263x.h"

#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_frontend.h"
#include "nexus_parser_band.h"

BDBG_MODULE(standby_transition);

#if (BCHP_CHIP == 7425)
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1
#elif (BCHP_CHIP == 7420)
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1
#elif (BCHP_CHIP == 7344 || BCHP_CHIP == 7346) 
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 0
#else
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#endif

#define S3_STANDBY_SUPPORT 0 /* disabled for now, since S3 support is limited */
#define SD_DISPLAY 1

NEXUS_DisplayHandle display0, display1;
NEXUS_PlatformConfiguration platformConfig;
NEXUS_StcChannelHandle stcChannel;
NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
NEXUS_VideoWindowHandle window0, window1;
NEXUS_VideoDecoderHandle videoDecoder;
NEXUS_VideoDecoderStartSettings videoProgram;
NEXUS_AudioDecoderHandle audioDecoder;
NEXUS_AudioDecoderStartSettings audioProgram;
NEXUS_FilePlayHandle file;
NEXUS_PlaypumpHandle playpump;
NEXUS_PlaybackHandle playback;
bool playback_started=false;

/* this is a simple wrapper around pmlib to make the sample app simpler */
typedef struct pmlib_state_t
{
    bool usb;
    bool enet;
    bool moca;
    bool sata;
    bool tp1;
    bool memc1;
    bool cpu;
    bool ddr;
} pmlib_state_t;

static struct {
    void *brcm_pm_ctx;
    pmlib_state_t g_state;
   
#define MAX_MOUNTS 16
    struct {
        char device[256];
        char mountpoint[256];
    } mounts[MAX_MOUNTS];
    unsigned total_mounts;
} g_power_state;

void playback_start(void)
{
    if (playback_started) { return; }

    file = NEXUS_FilePlay_OpenPosix("videos/cnnticker.mpg", NULL);
    if (!file) {
        fprintf(stderr, "can't open file\n");
        BDBG_ASSERT(file);
    }

    NEXUS_Playback_Start(playback, file, NULL);  
    playback_started = true;
}

void playback_stop(void)
{
    if (!playback_started) { return; }
    
    NEXUS_Playback_Stop(playback);  
    NEXUS_FilePlay_Close(file); /* required for umount */
    playback_started = false;
}

void decode_start(void)
{
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
}

void decode_stop(void)
{
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
}

void start_app(void)
{
    NEXUS_DisplaySettings displaySettings;
    NEXUS_StcChannelSettings stcSettings; 
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;

    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display0 = NEXUS_Display_Open(0, &displaySettings);
#if SD_DISPLAY
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display1 = NEXUS_Display_Open(1, &displaySettings);
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
    if (platformConfig.outputs.hdmi[0]) {
        NEXUS_Display_AddOutput(display0, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    }
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (platformConfig.outputs.component[0]) {
        NEXUS_Display_AddOutput(display0, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    }
#endif

#if SD_DISPLAY
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if (platformConfig.outputs.composite[0]) {
        NEXUS_Display_AddOutput(display1, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    }
#endif
#if NEXUS_NUM_RFM_OUTPUTS
    if (platformConfig.outputs.rfm[0]) {
        NEXUS_Display_AddOutput(display1, NEXUS_Rfm_GetVideoConnector(platformConfig.outputs.rfm[0]));
    }
#endif
#endif

    window0 = NEXUS_VideoWindow_Open(display0, 0);
#if SD_DISPLAY
    window1 = NEXUS_VideoWindow_Open(display1, 0);
#endif

    playpump = NEXUS_Playpump_Open(0, NULL);
    playback = NEXUS_Playback_Create();
    
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_NUM_SPDIF_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window0, NEXUS_VideoDecoder_GetConnector(videoDecoder));
#if SD_DISPLAY
    NEXUS_VideoWindow_AddInput(window1, NEXUS_VideoDecoder_GetConnector(videoDecoder));
#endif

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x21, &playbackPidSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x22, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
       the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eMpeg;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;
    
    decode_start();
    playback_start();
}

void stop_app(void)
{
    /* Bring down system */
    playback_stop();
    decode_stop();
    
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoWindow_RemoveInput(window0, NEXUS_VideoDecoder_GetConnector(videoDecoder));
#if SD_DISPLAY
    NEXUS_VideoWindow_RemoveInput(window1, NEXUS_VideoDecoder_GetConnector(videoDecoder));
#endif
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoWindow_Close(window0);
#if SD_DISPLAY
    NEXUS_VideoWindow_Close(window1);
#endif
    NEXUS_Display_Close(display0);
#if SD_DISPLAY
    NEXUS_Display_Close(display1);
#endif
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
#if NEXUS_NUM_SPDIF_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]));
#endif
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_StcChannel_Close(stcChannel);
}

int umount_all(void)
{
    FILE *f;

    g_power_state.total_mounts = 0;

    f = fopen("/proc/mounts", "r+");
    if (!f) { return -1; }
    while (!feof(f) && g_power_state.total_mounts < MAX_MOUNTS) {
        char buf[256];
        char *c1, *c2;
        if (fgets(buf, 256, f)) {
            if (strstr(buf, "/dev/sd") == buf || strstr(buf, " nfs ") || strstr(buf, "/dev/mmcblk") ) {
                c1 = strchr(buf, ' ');
                if (!c1) { continue; }

                *c1 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].device, buf);
                c1++;
                c2 = strchr(c1, ' ');
                if (!c2) { continue; }

                *c2 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].mountpoint, c1);

                BKNI_Printf("Found mount point %d: %s %s\n", g_power_state.total_mounts, g_power_state.mounts[g_power_state.total_mounts].device, g_power_state.mounts[g_power_state.total_mounts].mountpoint);

                snprintf(buf, 256, "umount %s", g_power_state.mounts[g_power_state.total_mounts].mountpoint);
                if (!system(buf)) {
                    g_power_state.total_mounts++;
                }
            }
        }
    }
    fclose(f);
    
    return 0;
}

int wait_for_device(char *device_path, unsigned long timeout_in_ms)
{
    int rc = 0;

    if (index( device_path, ':') == NULL   ) {  /* If device has ":", then it's an NFS path... skip it. */
        unsigned long ms;
        unsigned long ms_interval = 200;
        unsigned long ms_max = timeout_in_ms;
        int rc;
        struct stat statbuf;

        for (ms=0 ; ms<ms_max ; ms+=ms_interval) {
            rc = stat(device_path, &statbuf);
            if (rc == 0) break;
            if (ms == 0) BKNI_Printf("Waiting for device %s to exist...\n", device_path);
            BKNI_Sleep(ms_interval);
        }
        if (ms >= ms_max) {
            BKNI_Printf("Device %s not found after %lu ms\n", device_path, ms );
            return 1; /* return error code */
        }
        else if (ms > 0) {
            BKNI_Printf("Device %s appeared after %lu ms\n", device_path, ms );
        }
    }

    return rc;
}

int mount_all(void)
{
    unsigned i;

    for (i=0; i<g_power_state.total_mounts; i++) {
        char buf[256];
        int rc;

        rc = wait_for_device( g_power_state.mounts[i].device, 10000 );
        if (rc) { /* Device isn't there or not ready... */
            BKNI_Printf("Can't mount device \n", g_power_state.mounts[i].device );
            continue;
        }

        snprintf(buf, 256, "mount %s %s", g_power_state.mounts[i].device, g_power_state.mounts[i].mountpoint);
        BKNI_Printf("%s\n", buf);
        system(buf);
    }
    g_power_state.total_mounts = 0;
    
    return 0;
}

void get_pmlib_state(pmlib_state_t *state)
{
    *state = g_power_state.g_state;
}

int set_pmlib_state(const pmlib_state_t *state)
{
    struct brcm_pm_state pmlib_state;
    BERR_Code rc;

    rc = brcm_pm_get_status(g_power_state.brcm_pm_ctx, &pmlib_state);
    if (rc) { BDBG_ERR(("Cannot get PM status")); }

    pmlib_state.usb_status = state->usb;
#if MEMC1_SUPPORT
    pmlib_state.memc1_status = state->memc1;
#else
    pmlib_state.memc1_status = BRCM_PM_UNDEF;
#endif
    pmlib_state.tp1_status = state->tp1;
#if 0 /* TODO: why does this not work? */
    pmlib_state.cpu_divisor = state->cpu?1:8;
#endif 
    pmlib_state.ddr_timeout = state->ddr?0:64;

    rc = brcm_pm_set_status(g_power_state.brcm_pm_ctx, &pmlib_state);
    if (rc) { BDBG_ERR(("Cannot set PM status\n")); }
    
    if (state->enet) {
        if (!g_power_state.g_state.enet) {
            system("ifup eth0");
            system("sleep 3");
            system("kill `cat /tmp/udhcpc.eth0.pid`");
        }
    }
    else {
        if (g_power_state.g_state.enet) {
            system("ifdown eth0");
        }
    }

#if MOCA_SUPPORT
    if (state->moca) {
        if (!g_power_state.g_state.moca){
            system("mocactl start");
            system("ifup eth1");
            system("sleep 3");
            system("kill `cat /tmp/udhcpc.eth1.pid`");
        }
    } 
    else {
        if (g_power_state.g_state.moca){
            system("ifdown eth1");
            system("mocactl stop");
        }
    }
#endif

    g_power_state.g_state = *state;

    return 0;
}

int active_standby(unsigned timeout)
{
    NEXUS_PlatformStandbySettings standbySettings;
    pmlib_state_t pmlib_state;
    NEXUS_Error rc;
    
    BDBG_WRN(("Enter S1"));
    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_eActive;
    /* no need to set wakeup timeout, because there's nothing to be woken up */
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

    get_pmlib_state(&pmlib_state);
    pmlib_state.usb = false;
    pmlib_state.enet = true; /* must be left on if running over NFS */
    pmlib_state.moca = false;
    pmlib_state.tp1 = false;
    pmlib_state.cpu = false;
    pmlib_state.ddr = false;
    pmlib_state.memc1 = false;
    rc = set_pmlib_state(&pmlib_state);
    if (rc!=0) { return rc; }

    BKNI_Sleep(timeout*1000); /* stay in S1 for a bit... */

    BDBG_WRN(("Exit S1"));
    get_pmlib_state(&pmlib_state);
    pmlib_state.usb = true;
    pmlib_state.enet = true;
    pmlib_state.moca = false;
    pmlib_state.tp1 = true;
    pmlib_state.cpu = true;
    pmlib_state.ddr = true;
    pmlib_state.memc1 = true;
    set_pmlib_state(&pmlib_state);
    if (rc!=0) { return rc; }

    return 0;
}

int passive_standby(unsigned timeout)
{
    NEXUS_PlatformStandbySettings standbySettings;
    NEXUS_PlatformStandbyStatus standbyStatus;
    NEXUS_Error rc;

    BDBG_WRN(("Enter S2"));

    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_ePassive;
    standbySettings.wakeupSettings.timeout = timeout;
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

    umount_all();
    brcm_pm_suspend(g_power_state.brcm_pm_ctx, BRCM_PM_STANDBY); /* put MIPS to sleep */

    /* now in standby */

    BDBG_WRN(("Exit S2"));
    NEXUS_Platform_GetStandbyStatus(&standbyStatus); /* this executes after we've woken up */
    if (!standbyStatus.wakeupStatus.timeout) {
        BDBG_ERR(("S2 wakeup did not happen from timeout"));
        return -1;
    }

    mount_all();

    return 0;
}

int deep_sleep(unsigned timeout)
{
    NEXUS_PlatformStandbySettings standbySettings;
    NEXUS_PlatformStandbyStatus standbyStatus;
    NEXUS_Error rc;
    
    BDBG_WRN(("Enter S3"));

    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_eDeepSleep;
    standbySettings.wakeupSettings.timeout = timeout;
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

#if S3_WARM_BOOT
    system("echo mem > /sys/power/state"); /* warm boot. no cold boot support in this app because it can't be tested automatically */
#endif

    BDBG_WRN(("Exit S3"));
    NEXUS_Platform_GetStandbyStatus(&standbyStatus);
    if (!standbyStatus.wakeupStatus.timeout) {
        BDBG_ERR(("S3 wakeup did not happen from timeout"));
        return -1;
    }

    return 0;
}

int resume(void)
{
    NEXUS_PlatformStandbySettings standbySettings;
    NEXUS_Error rc;
    
    BDBG_WRN(("Resume"));
    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_eOn;
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

    return 0;
}

int main(int argc, char **argv)
{
#if NEXUS_POWER_MANAGEMENT
    NEXUS_PlatformSettings platformSettings;
    NEXUS_VideoDecoderStatus videoDecoderStatus;
    NEXUS_Error rc;
    pmlib_state_t pmlib_state;
    unsigned timeout = 5; /* seconds */
    unsigned loops = 50;
    unsigned current = 0, next;
    unsigned mode = 0; /* 0 means auto, where modes are randomly switched. 1 is S1 only, 2 is S2 only, 3 is S3 only */
    int i;

    /* TODO: consider adding a cmd prompt (like nexus/utils/playback.c) that allows us to switch modes on user input */
    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            BKNI_Printf("Usage: nexus %s (options)\n", argv[0]);
            BKNI_Printf(" -s1       only test s1 standby/resume\n");
            BKNI_Printf(" -s2       only test s2 standby/resume\n");
            BKNI_Printf(" -s3       only test s3 standby/resume\n");
            BKNI_Printf(" -loops #  number of transitions to test. defaults to %u\n", loops);
            return 0;
        }
        else if (!strcmp(argv[i], "-s1")) {
            mode = 1;
        } 
        else if (!strcmp(argv[i], "-s2")){
            mode = 2;
        }
        else if (!strcmp(argv[i], "-s2")){
            mode = 3;
        }
        else if (!strcmp(argv[i], "-loops")){
            loops = strtoul(argv[++i], NULL, 0);
        }
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    g_power_state.brcm_pm_ctx = brcm_pm_init();
    /* when app is first run, enet and moca are already running */
    g_power_state.g_state.enet = true;
    g_power_state.g_state.moca = true;

    get_pmlib_state(&pmlib_state);
    pmlib_state.usb = true;
    pmlib_state.enet = true;
    pmlib_state.moca = false; /* TODO: BSEAV/app/standby/standby.c had "true" commented out */
    pmlib_state.tp1 = true;
    pmlib_state.cpu = true;
    pmlib_state.ddr = true;
    pmlib_state.memc1 = true;
    set_pmlib_state(&pmlib_state);

    start_app();

#if S3_STANDBY_SUPPORT
    #define MODES 4
#else
    #define MODES 3
#endif
    while (loops--) {
        if (current==0) {
            playback_stop();
            decode_stop();
        }

        if (mode==0) {
            /* randomly select a new mode that is not the current one */
            next = rand()%(MODES-1);
            if (next >= current) {
                if (++next==MODES) { 
                    next = 0; 
                }
            }
            BDBG_ASSERT(current!=next);
            BDBG_ASSERT(next<MODES);
        }
        else {
            next = current==0 ? mode:0;
        }
        BDBG_WRN(("loop %d: curr %d, next %d", loops, current, next));

        switch (next) {
            case 0: 
                rc = resume(); 
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoDecoderStatus);
                BDBG_ASSERT(videoDecoderStatus.numDecoded==0);

                /* start playback and check status */
                decode_start();
                playback_start();
                BKNI_Sleep(timeout*1000);
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoDecoderStatus);
                BDBG_ASSERT(videoDecoderStatus.numDecoded>0);
                break;
                
            case 1: 
                rc = active_standby(timeout); 
                if (rc) { 
                    BDBG_ERR(("Active standby failure"));
                    goto end; 
                }
                break;
            case 2: 
                rc = passive_standby(timeout); 
                if (rc) { 
                    BDBG_ERR(("Passive standby failure"));
                    goto end; 
                }
                break;
#if S3_STANDBY_SUPPORT
            case 3: 
                rc = deep_sleep(timeout);
                if (rc) { 
                    BDBG_ERR(("Deep sleep failure"));
                    goto end; 
                }
                break;
#endif
        }
        current = next;
    }
end:
    resume(); /* must resume in order to uninit */
    
    stop_app();
    NEXUS_Platform_Uninit();
        
    return rc;
#else /* NEXUS_POWER_MANAGEMENT */
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
    return 0;
#endif
}

