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
 * $brcm_Workfile: decode_3desecb_cpd.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 1/27/12 11:47a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/security/noBSP/decode_3desecb_cpd.c $
 * 
 * 3   1/27/12 11:47a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 2   8/10/10 3:34p erickson
 * SW7420-934: rename NEXUS_Playpump_ReadComplete to
 *  NEXUS_Playpump_WriteComplete
 * 
 * 1   6/11/10 6:51p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 *****************************************************************************/
/* Nexus example app: playpump and decode */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_playpump.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "nexus_security.h"

/* the following define the input file and its characteristics -- these will vary by input file */
#define FILE_NAME "videos/stream_3des_cps_even_odd.mpg"
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define VIDEO_PID 0x11

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}


static NEXUS_Error ConfigureKeySlotFor3DesCPD (NEXUS_KeySlotHandle keyHandle, 
											 unsigned char * pkeyEven, 
											 unsigned char * pKeyOdd)
{
	NEXUS_SecurityAlgorithmSettings AlgConfig;
	NEXUS_SecurityClearKey key;

	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.bGlobalEnable = true;
	AlgConfig.bRestrictEnable = true;
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
	AlgConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
	AlgConfig.dest =NEXUS_SecurityAlgorithmConfigDestination_eCpd;

	if ( NEXUS_Security_ConfigAlgorithm (keyHandle, &AlgConfig) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed \n");
		return 1;
	}
       
	/* Load AV keys */
	key.keySize = 16; 
	key.dest =NEXUS_SecurityAlgorithmConfigDestination_eCpd;
	
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	BKNI_Memcpy(key.keyData,pKeyOdd,key.keySize);
	if ( NEXUS_Security_LoadClearKey (keyHandle, &key) != 0 )
	{
		printf("\nLoad CPS ODD key failed \n");
		return 1;
	}

	key.keyEntryType = NEXUS_SecurityKeyType_eEven;
	BKNI_Memcpy(key.keyData,pkeyEven,key.keySize);
	if ( NEXUS_Security_LoadClearKey (keyHandle, &key) != 0 )
	{
		printf("\nLoad CPS EVEN key failed \n");
		return 1;
	}

	key.keyEntryType = NEXUS_SecurityKeyType_eClear;
	BKNI_Memcpy(key.keyData,pkeyEven,key.keySize);
	if ( NEXUS_Security_LoadClearKey (keyHandle, &key) != 0 )
	{
		printf("\nLoad CPS EVEN key failed \n");
		return 1;
	}

	return 0;
}

int main(void)
{
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname = FILE_NAME;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
#endif
    NEXUS_KeySlotHandle videoKeyHandle = NULL;
    NEXUS_KeySlotHandle audioKeyHandle = NULL;
    NEXUS_KeySlotHandle videoKeyHandle2 = NULL;
    NEXUS_KeySlotHandle audioKeyHandle2 = NULL;
    NEXUS_SecurityKeySlotSettings keySlotSettings;    
    NEXUS_PidChannelStatus pidStatus;
    unsigned int videoPID;
/*    unsigned int audioPID, counter = 0; */
  
	unsigned char VidEvenCpsControlWord[] = { 
							    0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73,  
		                        0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7 
								};   
    unsigned char VidOddCpsControlWord[] = { 
							    0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73,  
		                        0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7 
								};   
 	unsigned char AudEvenCpsControlWord[] = { 
							    0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73,  
		                        0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7 
								};   
   unsigned char AudOddCpsControlWord[] = {
							    0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73,  
		                        0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7 
								};   

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

	/***************************************************************************************
		Config CA descrambler 
	***************************************************************************************/
	
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp; 
	
	/* Allocate AV keyslots */
    videoKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !videoKeyHandle)
	{
		printf("\nAllocate CACP video keyslot failed \n");
		return 1;
	}

    videoKeyHandle2 = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !videoKeyHandle2)
	{
		printf("\nAllocate CACP video keyslot 2 failed \n");
		return 1;
	}
	
    audioKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !audioKeyHandle)
	{
		printf("\nAllocate CACP audio keyslot failed \n");
		return 1;
	}

    audioKeyHandle2 = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !audioKeyHandle2)
	{
		printf("\nAllocate CACP audio keyslot 2 failed \n");
		return 1;
	}

#if 1
	if ( ConfigureKeySlotFor3DesCPD (videoKeyHandle, 
									VidEvenCpsControlWord, 
									VidOddCpsControlWord) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed for video keyslot 1 \n");
		return 1;
	}

	if ( ConfigureKeySlotFor3DesCPD (videoKeyHandle2, 
									VidEvenCpsControlWord, 
									VidOddCpsControlWord) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed for video keyslot 2 \n");
		return 1;
	}
   
	if ( ConfigureKeySlotFor3DesCPD (audioKeyHandle, 
									AudEvenCpsControlWord, 
									AudOddCpsControlWord) != 0 )
	{
		printf("\nConfig audio CPS Algorithm failed for audio keyslot 1 \n");
		return 1;
	}

	if ( ConfigureKeySlotFor3DesCPD (audioKeyHandle2, 
									AudEvenCpsControlWord, 
									AudOddCpsControlWord) != 0 )
	{
		printf("\nConfig audio CPS Algorithm failed for audio keyslot 2 \n");
		return 1;
	}

	printf("\nConfigure Security ok! \n");

#endif

    playpump = NEXUS_Playpump_Open(0, NULL);

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, VIDEO_PID, NULL);

	/* Add video PID channel to keyslot */
	NEXUS_PidChannel_GetStatus (videoPidChannel, &pidStatus);
	videoPID = pidStatus.pidChannelIndex;
	if ( NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle, videoPID)!= 0 )
	{
		printf("\nConfigPIDPointerTable failed \n");
		return 1;
	}

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    /* Bring up video decoder and display. No audio to keep the example simple. */
    display = NEXUS_Display_Open(0, NULL);
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

    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

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
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (!status.queueDepth) break;
            }
            fseek(file, 0, SEEK_SET);
            NEXUS_Playpump_Flush(playpump);
        }
        else {
            rc = NEXUS_Playpump_WriteComplete(playpump, 0, n);
            BDBG_ASSERT(!rc);
            printf("played %d bytes\n", n);
        }
    }
    return 0;

error:
    return 1;
}

