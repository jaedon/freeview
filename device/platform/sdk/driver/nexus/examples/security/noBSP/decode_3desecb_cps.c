/******************************************************************************
 *    (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: decode_3desecb_cps.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/27/12 12:02p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/examples/security/noBSP/decode_3desecb_cps.c $
 * 
 * 4   1/27/12 12:02p rjlewis
 * SW7425-1136: fixed compile error.
 * 
 * 3   1/27/12 11:47a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 2   8/10/10 4:08p erickson
 * SW7420-934: rename NEXUS_Recpump_DataWriteComplete/IndexWriteComplete
 *  to NEXUS_Recpump_DataReadComplete/IndexReadComplete
 * 
 * 1   6/11/10 6:51p qcheng
 * SW7550-102 : Add Security support for 7550
 * 
 *****************************************************************************/

/* Nexus example app: single live a/v decode from an input band and PVR record by using CPS*/
/* Test stream is spiderman.ts */

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
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nexus_security.h"
#include "nexus_recpump.h"

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#define keyladder_test
#ifdef keyladder_test
#include "nexus_keyladder.h"
#endif 

/* the following define the input file and its characteristics -- these will vary by input file */
#define DATA_FILE_NAME "videos/stream_3des_cps_even_odd.mpg"
#define INDEX_FILE_NAME "videos/stream_3des_cps_even_odd.idx"
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x11
#define AUDIO_PID 0x14

static void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("overflow %s\n", (const char *)context);
}

#ifndef keyladder_test
static NEXUS_Error ConfigureKeySlotFor3DesCPS (NEXUS_KeySlotHandle keyHandle, 
											 unsigned char * pkeyEven, 
											 unsigned char * pKeyOdd);
#else
static NEXUS_Error ConfigureKeySlotFor3DesCPSKL (NEXUS_KeySlotHandle keyHandle );
#endif

int main(void)
{
    FILE *data_file;
    FILE *index_file;
    const char data_fname[] = DATA_FILE_NAME;
    const char index_fname[] = INDEX_FILE_NAME;
    BKNI_EventHandle event;
    NEXUS_PlatformConfiguration platformConfig;

    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;

    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;

#ifndef keyladder_test
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
#endif

	unsigned int videoPID, audioPID, counter = 0;
	NEXUS_KeySlotHandle videoKeyHandle = NULL;
	NEXUS_KeySlotHandle audioKeyHandle = NULL;
	NEXUS_KeySlotHandle videoKeyHandle2 = NULL;
	NEXUS_KeySlotHandle audioKeyHandle2 = NULL;
	NEXUS_PidChannelStatus pidStatus;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL); 
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    printf("press ENTER to start record\n");
    getchar();
    
    BKNI_CreateEvent(&event);

    recpump = NEXUS_Recpump_Open(0, NULL);
    
    data_file = fopen(data_fname, "wb");
    if (!data_file) {
        fprintf(stderr, "can't open data file:%s\n", data_fname);
        goto error;
    }
    index_file = fopen(index_fname, "wb");
    if (!index_file) {
        fprintf(stderr, "can't open index file:%s\n", index_fname);
        goto error;
    }

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback;
    recpumpSettings.data.dataReady.context = event;
    recpumpSettings.index.dataReady.callback = dataready_callback; /* same callback */
    recpumpSettings.index.dataReady.context = event; /* same event */
    recpumpSettings.data.overflow.callback = overflow_callback;
    recpumpSettings.data.overflow.context = "data";
    recpumpSettings.index.overflow.callback = overflow_callback;
    recpumpSettings.index.overflow.context = "index";
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);
        
    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
    addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
    addPidChannelSettings.pidTypeSettings.video.index = true;
    addPidChannelSettings.pidTypeSettings.video.codec = VIDEO_CODEC;

    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    NEXUS_Recpump_AddPidChannel(recpump, videoPidChannel, &addPidChannelSettings);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);
    NEXUS_Recpump_AddPidChannel(recpump, audioPidChannel, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

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

	/***************************************************************************************
		Config CPS (CP scrambler) 
	***************************************************************************************/
#ifndef keyladder_test
	if ( ConfigureKeySlotFor3DesCPS (videoKeyHandle, 
									VidEvenCpsControlWord, 
									VidOddCpsControlWord) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed for video keyslot 1 \n");
		return 1;
	}

	if ( ConfigureKeySlotFor3DesCPS (videoKeyHandle2, 
									VidEvenCpsControlWord, 
									VidOddCpsControlWord) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed for video keyslot 2 \n");
		return 1;
	}
   
	if ( ConfigureKeySlotFor3DesCPS (audioKeyHandle, 
									AudEvenCpsControlWord, 
									AudOddCpsControlWord) != 0 )
	{
		printf("\nConfig audio CPS Algorithm failed for audio keyslot 1 \n");
		return 1;
	}

	if ( ConfigureKeySlotFor3DesCPS (audioKeyHandle2, 
									AudEvenCpsControlWord, 
									AudOddCpsControlWord) != 0 )
	{
		printf("\nConfig audio CPS Algorithm failed for audio keyslot 2 \n");
		return 1;
	}

#else

	if ( ConfigureKeySlotFor3DesCPSKL (videoKeyHandle) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed for video keyslot 1 \n");
		return 1;
	}

	if ( ConfigureKeySlotFor3DesCPSKL (videoKeyHandle2) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed for video keyslot 2 \n");
		return 1;
	}
   
	if ( ConfigureKeySlotFor3DesCPSKL (audioKeyHandle) != 0 )
	{
		printf("\nConfig audio CPS Algorithm failed for audio keyslot 1 \n");
		return 1;
	}

	if ( ConfigureKeySlotFor3DesCPSKL (audioKeyHandle2) != 0 )
	{
		printf("\nConfig audio CPS Algorithm failed for audio keyslot 2 \n");
		return 1;
	}
#endif

	/* Add video PID channel to keyslot */
	NEXUS_PidChannel_GetStatus (videoPidChannel, &pidStatus);
	videoPID = pidStatus.pidChannelIndex;
	if ( NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle, videoPID)!= 0 )
	{
		printf("\nConfigPIDPointerTable failed \n");
		return 1;
	}

	NEXUS_PidChannel_GetStatus (audioPidChannel, &pidStatus);
	audioPID = pidStatus.pidChannelIndex;
	NEXUS_Security_AddPidChannelToKeySlot(audioKeyHandle, audioPID); 

	printf ("\nSecurity Config OK\n");

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SCART_INPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
#if NEXUS_NUM_SCART_INPUTS>1
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
#endif
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

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

    NEXUS_Recpump_Start(recpump);

    while (1) {
        const void *data_buffer, *index_buffer;
        size_t data_buffer_size, index_buffer_size;
        int n;

        if (NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size))
            break;
        if (NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size))
            break;
        if (data_buffer_size == 0 && index_buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
		}
	       
		if (data_buffer_size) {
			n = fwrite(data_buffer, 1, data_buffer_size, data_file);
			if (n < 0) goto error;
			NEXUS_Recpump_DataReadComplete(recpump, n);
		}
		if (index_buffer_size) {
			n = fwrite(index_buffer, 1, index_buffer_size, index_file);
			if (n < 0) goto error;
			NEXUS_Recpump_IndexReadComplete(recpump, n);
		}

		/* Switch SC polarity for output packets */
		counter++;
		if ( (counter & 1) == 0 )
		{
			NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle, videoPID);
			NEXUS_Security_AddPidChannelToKeySlot(audioKeyHandle, audioPID); 
		}
		else
		{
			NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle2, videoPID);
			NEXUS_Security_AddPidChannelToKeySlot(audioKeyHandle2, audioPID); 
		}
	}	
    /* loops forever */
	
error:
    return 1;
}

#ifndef keyladder_test

static NEXUS_Error ConfigureKeySlotFor3DesCPS (NEXUS_KeySlotHandle keyHandle, 
											 unsigned char * pkeyEven, 
											 unsigned char * pKeyOdd)
{
	NEXUS_SecurityAlgorithmSettings AlgConfig;
	NEXUS_SecurityClearKey key;
	unsigned i;

	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.bGlobalEnable = true;
	AlgConfig.bRestrictEnable = false;
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
	AlgConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
	AlgConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;
    for (i=0; i<NEXUS_SecurityPacketType_eMax;i++) {
		AlgConfig.modifyScValue[i] = true; 
		AlgConfig.scValue[i] =  NEXUS_SecurityAlgorithmScPolarity_eOdd; 
	}
	if ( NEXUS_Security_ConfigAlgorithm (keyHandle, &AlgConfig) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed \n");
		return 1;
	}
       
	/* Load AV keys */
	key.keySize = 16; 
	key.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;
	
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

#else /*keyladder_test*/

static NEXUS_Error ConfigureKeySlotFor3DesCPSKL (NEXUS_KeySlotHandle keyHandle )
{
	unsigned i;
	NEXUS_SecurityAlgorithmSettings AlgConfig;
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW;
	
	unsigned char ucProcInForKey3[16] = { 	
		0x00,0x00,0x11,0x11 ,0x55,0x55,0x66,0x66,
		0x11,0x22,0x33,0x44 ,0x55,0x66,0x77,0x88,
			};

	unsigned char ucProcInKey4_VideoEven[16] = { 
		0x28,0x30,0x46,0x70 ,0x71,0x4B,0x8C,0x75,
		0xEB,0x46,0x04,0xBB ,0x96,0xD0,0x48,0x88,
			};
	
	unsigned char ucProcInKey4_VideoOdd[16] = { 
		0x28,0x30,0x46,0x70 ,0x71,0x4B,0x8C,0x75,
		0xEB,0x46,0x04,0xBB ,0x96,0xD0,0x48,0x88,
			};

	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.bGlobalEnable = true;
	AlgConfig.bRestrictEnable = false;
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
	AlgConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
	AlgConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;
    for (i=0; i<NEXUS_SecurityPacketType_eMax;i++) {
		AlgConfig.modifyScValue[i] = true; 
		AlgConfig.scValue[i] =  NEXUS_SecurityAlgorithmScPolarity_eOdd; 
	}
	if ( NEXUS_Security_ConfigAlgorithm (keyHandle, &AlgConfig) != 0 )
	{
		printf("\nConfig video CPS Algorithm failed \n");
		return 1;
	}
       
	/* Genearate key 2 and key3  */
	encryptedSessionkey.keyladderID		= NEXUS_SecurityKeyladderID_eA;	/* NEXUS_SecurityKeyladderID_eA is CP keyaldder */
	encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eCuskey;
	encryptedSessionkey.swizzleType 	= NEXUS_SecuritySwizzleType_eSwizzle0;
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 		= 0xFF; 
	encryptedSessionkey.cusKeyVarH 		= 0x00; 	
	encryptedSessionkey.bRouteKey 		= false;
	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
	{
		printf("\nLoading session key failed \n");
		return 1;
	}

	/* Gnerate key 4 and route to CW  */
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;	/* NEXUS_SecurityKeyladderID_eA is CP keyaldder */
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
	encrytedCW.keySize = sizeof(ucProcInKey4_VideoEven); 
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_VideoEven, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(keyHandle, &encrytedCW))
	{
		printf("\nLoading session key failed for Even key\n");
		return  1;
	}

	/* Gnerate key 4 and route to CW  */
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;	/* NEXUS_SecurityKeyladderID_eA is CP keyaldder */
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	encrytedCW.keySize = sizeof(ucProcInKey4_VideoOdd); 
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_VideoOdd, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(keyHandle, &encrytedCW))
	{
		printf("\nLoading session key failed for ODD key\n");
		return 1 ;
	}

	return 0;
}

#endif /*keyladder_test*/

