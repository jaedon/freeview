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
 * $brcm_Workfile: ip_streamer_subst.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 1/10/12 5:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/ip_streamer/ip_streamer_subst.c $
 * 
 * Hydra_Software_Devel/9   1/10/12 5:30p mward
 * SW7125-1205:  Add ip_addr and port to start settings, for use with
 * channel file parsing.
 * 
 * Hydra_Software_Devel/8   11/30/11 4:12p jtna
 * SW7405-4653: general cleanup. fix timebase configuration.
 * 
 * Hydra_Software_Devel/7   10/18/11 2:20p gmullen
 * SW7425-1383: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1383/1   10/14/11 4:12p gmullen
 * SW7425-1383: Updated allPass usages to use new API
 * 
 * Hydra_Software_Devel/6   5/24/11 6:21p jtna
 * SW7405-5336: add check for IP streamer input
 * 
 * Hydra_Software_Devel/5   3/7/11 4:44p jtna
 * SW7420-1561: fix IpsClose unwind sequence
 * 
 * Hydra_Software_Devel/4   2/25/11 7:05p jtna
 * SW7405-4653: fix broken IP playback sub unittest
 * 
 * Hydra_Software_Devel/3   7/26/10 4:45p calvinho
 * SW7405-4653: Removed unused functions
 * 
 * Hydra_Software_Devel/2   7/26/10 12:55p calvinho
 * SW7405-4653: Updated nomenclature
 * 
 * Hydra_Software_Devel/1   7/23/10 10:53a calvinho
 * SW7405-4653: Added IP Streamer input substitution
 * 
 ******************************************************************************/

#include "ip_streamer_subst.h"

#include "bstd.h"
#include "bkni.h"
#include "nexus_pid_channel.h"
#include "b_os_lib.h"
#include "b_playback_ip_lib.h"
#include "nexus_remux.h"
#include "nexus_parser_band.h"
#include "nexus_audio_output.h"

BDBG_MODULE(ip_streamer_subst);

#define IP_NETWORK_JITTER       300
#define APPSETTING_IPADDRESS    "224.1.1.10" /* multicast range is [224.0.0.0, 239.255.255.255] */
#define APPSETTING_PORT         1234
#define APPSETTING_PROTOCOL     B_PlaybackIpProtocol_eUdp

struct Ips {
    B_PlaybackIpHandle playbackIp;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PidChannelHandle playpumpPidChannel; /* the allpass pid channel */
    NEXUS_RemuxHandle remux;
};

IpsHandle IpsOpen(IpsOpenSettings *pSettings)
{
    IpsHandle ips;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpOpenPidChannelSettings pidChannelSettings;
    NEXUS_RemuxSettings remuxSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_Error rc;

    ips = BKNI_Malloc(sizeof(struct Ips));
    BDBG_ASSERT(ips);
    BKNI_Memset(ips, 0, sizeof(struct Ips));

    B_Os_Init();
    
    ips->playpump = NEXUS_Playpump_Open(pSettings->playpumpIndex, NULL);
    BDBG_ASSERT(ips->playpump);

    BKNI_Printf("Using IP Streamer Input Substitution...\n");
    /* Setup Playpump */
    NEXUS_Playpump_GetSettings(ips->playpump, &playpumpSettings);    
    playpumpSettings.acceptNullPackets = true;
    playpumpSettings.allPass = true;
    NEXUS_Playpump_SetSettings(ips->playpump, &playpumpSettings);
    
    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
    NEXUS_Playpump_GetAllPassPidChannelIndex(ips->playpump, (unsigned *) &pidChannelSettings.pidSettings.pidChannelIndex );
    pidChannelSettings.pidType = NEXUS_PidType_eOther;
    ips->playpumpPidChannel = NEXUS_Playpump_OpenPidChannel(ips->playpump, 0x0, &pidChannelSettings);

    /* Setup remux */
    NEXUS_Remux_GetDefaultSettings(&remuxSettings);
    remuxSettings.allPass = true;
    ips->remux = NEXUS_Remux_Open(pSettings->remuxIndex, &remuxSettings);
    BDBG_ASSERT(ips->remux);
    NEXUS_Remux_AddPidChannel(ips->remux, ips->playpumpPidChannel);
    
    NEXUS_Remux_Start(ips->remux);
    
    /* Setup parserBand */
    NEXUS_ParserBand_GetSettings(pSettings->parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eRemux;
    parserBandSettings.sourceTypeSettings.remux = ips->remux;
    parserBandSettings.acceptNullPackets = true;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(pSettings->parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);

    return ips;
}

unsigned IpsAddParserBandSettings(IpsHandle ips, NEXUS_ParserBandSettings *parserBandSettings)
{
    parserBandSettings->sourceType = NEXUS_ParserBandSourceType_eRemux;
    parserBandSettings->sourceTypeSettings.remux = ips->remux;
    parserBandSettings->acceptNullPackets = true;
    parserBandSettings->transportType = NEXUS_TransportType_eTs;
    return 0;
}

unsigned IpsConfigureTimebase(IpsHandle ips, IpsTimebaseSettings *pSettings)
{
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_AudioOutputSettings audioOutputSettings;
    NEXUS_AudioOutput audioOutput;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;

    if (pSettings->timebaseStc!=NEXUS_Timebase_eInvalid) {
        /* Timebase Settings for stc */
        NEXUS_Timebase_GetSettings(pSettings->timebaseStc, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.sourceSettings.pcr.pidChannel = ips->playpumpPidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = IP_NETWORK_JITTER * 91; /* in milliseconds: based on 90Khz clock */
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e244ppm;
        rc = NEXUS_Timebase_SetSettings(pSettings->timebaseStc, &timebaseSettings);
        BDBG_ASSERT(!rc);
    }

    if (pSettings->timebaseDecouple!=NEXUS_Timebase_eInvalid) {
        /* decouple display and audio output from input timebase */
        NEXUS_Timebase_GetSettings(pSettings->timebaseDecouple, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
        rc = NEXUS_Timebase_SetSettings(pSettings->timebaseDecouple, &timebaseSettings);
        BDBG_ASSERT(!rc);
    }

    if (pSettings->audioDac && pSettings->timebaseDecouple!=NEXUS_Timebase_eInvalid) {
        audioOutput = NEXUS_AudioDac_GetConnector(pSettings->audioDac);
        NEXUS_AudioOutput_GetSettings(audioOutput, &audioOutputSettings);
        audioOutputSettings.timebase = pSettings->timebaseDecouple;
        rc = NEXUS_AudioOutput_SetSettings(audioOutput, &audioOutputSettings);
        BDBG_ASSERT(!rc);
    }
    
    if (pSettings->display && pSettings->timebaseDecouple!=NEXUS_Timebase_eInvalid) {
        NEXUS_Display_GetSettings(pSettings->display, &displaySettings);
        displaySettings.timebase = pSettings->timebaseDecouple;
        rc = NEXUS_Display_SetSettings(pSettings->display, &displaySettings);
        BDBG_ASSERT(!rc);
    }

    return 0;
}

unsigned IpsStart(IpsHandle ips, const IpsStartSettings *pSettings)
{
    B_PlaybackIpSessionOpenSettings ipSessionOpenSettings;
    B_PlaybackIpSessionOpenStatus ipSessionOpenStatus;
    B_PlaybackIpSessionStartSettings ipSessionStartSettings;
    B_PlaybackIpSessionStartStatus ipSessionStartStatus;
    NEXUS_Error rc;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_AudioDecoderSettings audioDecoderSettings;
    const char* ipAddr;
    char* port_buf; 
    unsigned port;

	ipAddr = getenv("IP_STREAMER_ADDR");
	if (!ipAddr) {
		if (pSettings->ipAddr[0]) {
			ipAddr = pSettings->ipAddr;
		}
		else {
			ipAddr = APPSETTING_IPADDRESS;
		}
	}

	port_buf = getenv("IP_STREAMER_PORT");
	if (port_buf) {
		port = (unsigned) atoi(port_buf);
	} 
	else {
		if (pSettings->port) {
			port = pSettings->port;
		}
		else {
			port = APPSETTING_PORT;
		}
	}
    if (pSettings->vDecoder) {
        /* Video Decoder settings */
        NEXUS_VideoDecoder_GetSettings(pSettings->vDecoder, &videoDecoderSettings);
        videoDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45; /* in 45Khz clock */
        rc = NEXUS_VideoDecoder_SetSettings(pSettings->vDecoder, &videoDecoderSettings);
        BDBG_ASSERT(!rc);
    }

    if (pSettings->aDecoder) {
        /* Audio Decoder settings */
        NEXUS_AudioDecoder_GetSettings(pSettings->aDecoder, &audioDecoderSettings);
        audioDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;
        rc = NEXUS_AudioDecoder_SetSettings(pSettings->aDecoder, &audioDecoderSettings);
        BDBG_ASSERT(!rc);
    }

    /* Open IP applib and start a session */
    ips->playbackIp = B_PlaybackIp_Open(NULL);
    BDBG_ASSERT(ips->playbackIp);

    BKNI_Memset(&ipSessionOpenSettings, 0, sizeof(ipSessionOpenSettings));
    BKNI_Memset(&ipSessionOpenStatus, 0, sizeof(ipSessionOpenStatus));
    strncpy(ipSessionOpenSettings.socketOpenSettings.ipAddr, ipAddr, sizeof(ipSessionOpenSettings.socketOpenSettings.ipAddr)-1);
    ipSessionOpenSettings.socketOpenSettings.port = port;
    ipSessionOpenSettings.socketOpenSettings.protocol = APPSETTING_PROTOCOL;
    ipSessionOpenSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
    ipSessionOpenSettings.maxNetworkJitter = 300;
    ipSessionOpenSettings.networkTimeout = 1;  /* timeout in 1 sec during network outage events */
    rc = B_PlaybackIp_SessionOpen(ips->playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
    BDBG_ASSERT(!rc);
    
    BKNI_Memset(&ipSessionStartSettings, 0, sizeof(ipSessionStartSettings));
    ipSessionStartSettings.nexusHandles.playpump = ips->playpump;
    ipSessionStartSettings.nexusHandles.videoDecoder = pSettings->vDecoder;
    ipSessionStartSettings.nexusHandles.stcChannel = pSettings->stcChannel;
    ipSessionStartSettings.nexusHandles.primaryAudioDecoder = pSettings->aDecoder;
    ipSessionStartSettings.nexusHandlesValid = true;
    rc = B_PlaybackIp_SessionStart(ips->playbackIp, &ipSessionStartSettings, &ipSessionStartStatus);

    while (rc == B_ERROR_IN_PROGRESS) { /* alternatively, we could wait for the callback */
        BKNI_Sleep(100);
        rc = B_PlaybackIp_SessionStart(ips->playbackIp, &ipSessionStartSettings, &ipSessionStartStatus);
    }

    /* Start playpump */
    rc = NEXUS_Playpump_Start(ips->playpump);
    BDBG_ASSERT(!rc);

    /* check if data actually flowing through streamer */
    while (1) {
        static int count = 0;

        NEXUS_PlaypumpStatus playpumpStatus;
        NEXUS_Playpump_GetStatus(ips->playpump, &playpumpStatus);
        if (playpumpStatus.bytesPlayed) {
            break;
        }

        BKNI_Printf("Waiting for IP streamer input...\n");
        BKNI_Sleep(1000);

        if (count++==5) {
            BKNI_Printf("No IP streamer input. Asserting!\n");
            BDBG_ASSERT(0);
        }
    }

    return 0;
}

unsigned IpsAddStcSettings(IpsHandle ips, NEXUS_StcChannelSettings *stcSettings)
{
    stcSettings->modeSettings.pcr.offsetThreshold = IP_NETWORK_JITTER * 183; 
    stcSettings->modeSettings.pcr.maxPcrError = 0xFFFFFFFF;   /* Settop API had it set to this value */
    stcSettings->modeSettings.pcr.pidChannel = ips->playpumpPidChannel;
    stcSettings->modeSettings.pcr.disableJitterAdjustment = true;
    stcSettings->modeSettings.pcr.disableTimestampCorrection = true;
    stcSettings->autoConfigTimebase = false;
    return 0;
}

unsigned IpsAddTimebaseSettings(IpsHandle ips, NEXUS_TimebaseSettings *timebaseSettings)
{
    timebaseSettings->sourceSettings.pcr.pidChannel = ips->playpumpPidChannel;
    return 0;
}

unsigned IpsAddVideoDecoderOpenSettings(IpsHandle ips, NEXUS_VideoDecoderOpenSettings *videoDecoderOpenSettings)
{
    BSTD_UNUSED(ips);
    videoDecoderOpenSettings->fifoSize = 5 * 1024 * 1024; /* increased CDB for dejitter buffer */
    return 0;
}

unsigned IpsAddAudioDecoderOpenSettings(IpsHandle ips, NEXUS_AudioDecoderOpenSettings *audioDecoderOpenSettings)
{
    BSTD_UNUSED(ips);
    audioDecoderOpenSettings->fifoSize = 512 * 1024; /* increased CDB for dejitter buffer */
    return 0;
}

void IpsClose(IpsHandle ips)
{
    /* TODO: should we have an IpsStop() ? */
    if (ips->playbackIp) {
        B_PlaybackIp_SessionStop(ips->playbackIp);
        B_PlaybackIp_Close(ips->playbackIp);
        ips->playbackIp = NULL;
    }
    NEXUS_Remux_Stop(ips->remux);
    NEXUS_Remux_Close(ips->remux);
    NEXUS_Playpump_Stop(ips->playpump);
    NEXUS_Playpump_ClosePidChannel(ips->playpump, ips->playpumpPidChannel);
    NEXUS_Playpump_Close(ips->playpump);
    BKNI_Free(ips);
}

#if 0
NEXUS_PidChannelHandle IpsGetPidChannel(IpsHandle ips)
{
    return ips->playpumpPidChannel;
}

NEXUS_PlaypumpHandle IpsGetPlaypump(IpsHandle ips)
{
    return ips->playpump;
}
#endif
