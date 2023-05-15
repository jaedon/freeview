/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: allpass_test.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/allpass_test.c $
 * 
 * Hydra_Software_Devel/5   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
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
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_hdmi_output.h"
#if 0
#include "nexus_transcoder.h"
#endif

#include "nexus_recpump.h"
#include "nexus_record.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

struct handles
{
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderStartSettings videoProgram;
} handles[2] ;

NEXUS_PlatformConfiguration platformConfig;

#ifdef IP_STREAMER_SUBST
IpsHandle ips;
#endif

int VideoDecodeAndDisplay( 
	unsigned int id, 
	uint16_t video_pid, 
	NEXUS_VideoCodec compstd,
    NEXUS_ParserBand parserBand,
    NEXUS_PidChannelHandle pidChannel
	)
{
	int i;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsTimebaseSettings ipsTimebaseSettings;
#endif
	
    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&handles[id].videoProgram);
    handles[id].videoProgram.codec = compstd;
	if(NULL == pidChannel)
	    handles[id].videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, video_pid, NULL);
	else
		handles[id].videoProgram.pidChannel = pidChannel;

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
#if NO_TSM
		printf("Disabling TSM ... ");
	    handles[id].videoProgram.stcChannel = NULL ;
#else
	{
		printf("Enabling TSM ... ");
	    NEXUS_StcChannel_GetDefaultSettings(id, &stcSettings);              
	    stcSettings.timebase = NEXUS_Timebase_e0+id;
	    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
	    stcSettings.modeSettings.pcr.pidChannel = handles[id].videoProgram.pidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
        IpsAddStcSettings(ips, &stcSettings);
#endif
	    handles[id].videoProgram.stcChannel = NEXUS_StcChannel_Open(id, &stcSettings);
	}
#endif
    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    handles[id].display = NEXUS_Display_Open(id, &displaySettings);
    handles[id].window = NEXUS_VideoWindow_Open(handles[id].display, 0);
	if(id == 1)
	{
	#if HDMI
		NEXUS_HdmiOutputSettings hdmiSettings;
	    /* Install hotplug callback -- video only for now */
	    NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
	    hdmiSettings.hotplugCallback.callback = hotplug_callback;
	    hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
	    hdmiSettings.hotplugCallback.param = (int)handles[id].display;
	    NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
	#endif
        #if NEXUS_NUM_SVIDEO_OUTPUTS
        if ( platformConfig.outputs.svideo[0] )
        {
            NEXUS_Display_AddOutput(handles[id].display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
			printf("Attaching Svideo ...\n");
        }
        if ( platformConfig.outputs.composite[0])
        {
            NEXUS_Display_AddOutput(
				handles[id].display, 
				NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0])
			);
			printf("Attaching Composite ...\n");
        }
		#endif
	}
	else
	{
		NEXUS_DisplaySettings dispSettings ;

	    NEXUS_Display_AddOutput(handles[id].display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));

		NEXUS_Display_GetSettings(handles[id].display, &dispSettings);
		printf("Current Display Settings %d\n", dispSettings.format) ;
		dispSettings.format = NEXUS_VideoFormat_e480p ;
		NEXUS_Display_SetSettings(handles[id].display, &dispSettings);

	}
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0+id;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1+id;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = handles[id].display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    handles[id].vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(handles[id].window, NEXUS_VideoDecoder_GetConnector(handles[id].vdecode));

    /* Start Decoders */
    printf("codec[%d] = %d\n", id, handles[id].videoProgram.codec);
    NEXUS_VideoDecoder_Start(handles[id].vdecode, &handles[id].videoProgram);

    for (i=0;i<5;i++) {
        NEXUS_VideoDecoderStatus status;
        uint32_t stc;
        BKNI_Memset(&status, 0, sizeof(NEXUS_VideoDecoderStatus));
        NEXUS_VideoDecoder_GetStatus(handles[id].vdecode, &status);
        if(handles[id].videoProgram.stcChannel) 
            NEXUS_StcChannel_GetStc(handles[id].videoProgram.stcChannel, &stc);
        printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
        BKNI_Sleep(1000);
    }
    
    return 0;
}


NEXUS_FileRecordHandle file;
NEXUS_RecordHandle record;

void RecordInput(void)
{
    NEXUS_RecpumpHandle recpump;
    NEXUS_ParserBandSettings parserBandSettings;
	NEXUS_PidChannelHandle recordPidChannel ;
	NEXUS_PidChannelSettings pidChannelSettings ;

	NEXUS_InputBand inputBand ;

    NEXUS_RecordPidChannelSettings pidCfg;
    NEXUS_RecordSettings recordCfg;
    const char *fname = "example.mpg";
    const char *index = NULL;

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e2, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	parserBandSettings.sourceTypeSettings.inputBand = inputBand ;
	parserBandSettings.transportType = NEXUS_TransportType_eTs;
#ifdef IP_STREAMER_SUBST
    IpsAddParserBandSettings(ips, &parserBandSettings);
#endif
	parserBandSettings.allPass = true ;
	parserBandSettings.acceptNullPackets = true ;
    NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e2, &parserBandSettings);


    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordCfg);
    recordCfg.recpump = recpump;
    NEXUS_Record_SetSettings(record, &recordCfg);

    file = NEXUS_FileRecord_OpenPosix(fname,  index);

    NEXUS_Record_GetDefaultPidChannelSettings(&pidCfg);
    pidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidCfg.recpumpSettings.pidTypeSettings.video.index = false;
    pidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264;

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    NEXUS_ParserBand_GetAllPassPidChannelIndex(NEXUS_ParserBand_e2, (unsigned *) &pidChannelSettings.pidChannelIndex);
    recordPidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e2, 0x11, &pidChannelSettings);
    NEXUS_Record_AddPidChannel(record,
        recordPidChannel,
        &pidCfg);

    NEXUS_Record_Start(record, file);

	printf("\n\nRecord Handles recpump=%p, record=%p, file=%p\n\n", (void *)recpump, (void *)record, (void *)file);
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
	NEXUS_InputBand inputBand ;
    NEXUS_ParserBandSettings parserBandSettings;
#ifdef IP_STREAMER_SUBST
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings); 
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e0, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	parserBandSettings.sourceTypeSettings.inputBand = inputBand ;
	parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e0, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    printf("Started IP_Streamer\n");
#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

	VideoDecodeAndDisplay(
		0, 
		0x11, 
		NEXUS_VideoCodec_eMpeg2, 
		NEXUS_ParserBand_e0,
		NULL) ;

	RecordInput() ;

	while(1);

	return 0;
}
