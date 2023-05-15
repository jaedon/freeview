/***************************************************************************
*     (c)2004-2008 Broadcom Corporation
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
* $brcm_Workfile: spdif_input.c $
* $brcm_Revision: 2 $
* $brcm_Date: 10/22/08 7:30p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/spdif_input.c $
* 
* 2   10/22/08 7:30p jgarrett
* PR 47360: Adding compressed SPDIF passthrough for HDMI/SPDIF inputs
* 
* 1   1/18/08 2:13p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/2   11/30/07 1:17p erickson
* PR34925: rename to nexus_stc_channel.h
* 
* Nexus_Devel/1   11/29/07 10:32a jgarrett
* PR 37471: Adding spdif_input
* 
* Nexus_Devel/5   11/19/07 1:20p erickson
* PR34925: split demux.h into separate interface files
* 
* Nexus_Devel/4   11/16/07 3:32p erickson
* PR36814: added NEXUS_HdmiInput_SetHdcpKeySet call
* 
* Nexus_Devel/3   11/15/07 9:31a erickson
* PR34662: added #include nexus_video_window. removed unnecessary
* windowSettings.visible = true.
* 
* Nexus_Devel/2   11/9/07 3:16p erickson
* PR36814: added config of timebase
* 
* Nexus_Devel/1   11/8/07 2:39p erickson
* PR36814: add hdmi_input
* 
***************************************************************************/

/* Nexus example app: single live video decode from 3563 tuner */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_stc_channel.h"
#include "nexus_spdif_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

int main(void)
{
    NEXUS_SpdifInputHandle spdifInput;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_StcChannelSettings stcSettings;
        
    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL); 
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
    NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

    spdifInput = NEXUS_SpdifInput_Open(0, NULL);
    
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.input = NEXUS_SpdifInput_GetConnector(spdifInput);
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.autoConfigTimebase = false;
    audioProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                               NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    #if 1
    fprintf(stderr, "Starting Audio Decoder\n");
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    #endif

    printf("press any key to exit\n");
    getchar();

    fprintf(stderr, "Stopping decoder\n");
    NEXUS_AudioDecoder_Stop(audioDecoder);
    fprintf(stderr, "Shutdown decoder connector\n");
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    fprintf(stderr, "Shutdown spdif input connector\n");
    NEXUS_AudioInput_Shutdown(NEXUS_SpdifInput_GetConnector(spdifInput));
    fprintf(stderr, "Close decoder\n");
    NEXUS_AudioDecoder_Close(audioDecoder);
    fprintf(stderr, "Close spdif input\n");
    NEXUS_SpdifInput_Close(spdifInput);
    fprintf(stderr, "Close stc channel\n");
    NEXUS_StcChannel_Close(audioProgram.stcChannel);

    return 0;
}

