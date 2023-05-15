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
 * $brcm_Workfile:  $
 * $brcm_Revision:  $
 * $brcm_Date: $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log:  $
 * 
 *
******************************************************************************/
/* Nexus example app: single live a/v descrambling and decode from an input band */
/* Test stream is ocean_11.ts */

/* This example shows how to use keyladder( route key4)  to generate CW.
   Due to differences in HW design, the value provided in this sample only works for settop chips 7550.  
   For other chips, the key values may need to be changed for the test to pass
*/

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
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include "nexus_security.h"
#include "nexus_keyladder.h"

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x41
#define AUDIO_PID 0x44

static int Prompt(void);
void Descrmbler_Dvbcsa_SwKey(NEXUS_VideoDecoderStartSettings *pVideoProgram, NEXUS_AudioDecoderStartSettings *pAudioProgram );
void Descrmbler_Dvbcsa_CustKeyLadder(NEXUS_VideoDecoderStartSettings *pVideoProgram, NEXUS_AudioDecoderStartSettings *pAudioProgram);
void Descrmbler_Dvbcsa_OtpKeyLadder(NEXUS_VideoDecoderStartSettings *pVideoProgram, NEXUS_AudioDecoderStartSettings *pAudioProgram);

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);

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

    /* Bring up display and outputs */
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
#if NEXUS_NUM_SVIDEO_OUTPUTS    
    NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
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

#if 0
    /* Print status while decoding */
    for (;;) {
        NEXUS_VideoDecoderStatus status;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;

        NEXUS_VideoDecoder_GetStatus(vdecode, &status);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
        NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
        printf("audio0            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        NEXUS_AudioDecoder_GetStatus(compressedDecoder, &audioStatus);
        if ( audioStatus.started )
        {
            printf("audio1            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
                audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        }
        BKNI_Sleep(1000);
    }
#else

{
	unsigned int command_id;
	
	while(1) {
		BKNI_Printf("\nSelect ICAM test to Test:\n");
		BKNI_Printf("1) DVB-CSA SW Key\n");
		BKNI_Printf("2) DVB-CSA CUST Key ( Route key4 )\n");
		BKNI_Printf("3) DVB-CSA OTP Key ( Route key4 )\n");
		BKNI_Printf("0) Exit\n");

		command_id = Prompt();

		switch (command_id) {
			case 0:  /* Exit */
				return 0;				

			case 1:
				Descrmbler_Dvbcsa_SwKey(&videoProgram , &audioProgram);
				break;		

			case 2:
				Descrmbler_Dvbcsa_CustKeyLadder(&videoProgram , &audioProgram);
				break;		

			case 3:
				Descrmbler_Dvbcsa_OtpKeyLadder(&videoProgram , &audioProgram);
				break;
						
			default:
				BKNI_Printf("\nInvalid Choice!, command_id = %d\n\n", command_id);
				break;
		}
	}
}	

    printf("Press ENTER to stop decode\n");
    getchar();

    /* example shutdown */
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        NEXUS_AudioDecoder_Stop(compressedDecoder);
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
    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}

static int Prompt()
{
   char choice[10];
   unsigned char input_char;
   unsigned int command_id;

   input_char = 0;
   
   BKNI_Printf("\nChoice: ");

   while (input_char == 0) {
      input_char = getchar(); 
	if (input_char == '\n')
		input_char = 0;
   }

   choice[0] = input_char;
   choice[1] = '\0';

   if ((choice[0] == 'y') || (choice[0] == 'Y') ||
       (choice[0] == 'n') || (choice[0] == 'N'))
      return choice[0];

   sscanf(choice,"%x\n",&command_id);
   return command_id;
}


void Descrmbler_Dvbcsa_SwKey(NEXUS_VideoDecoderStartSettings *pVideoProgram, NEXUS_AudioDecoderStartSettings *pAudioProgram )
{
    NEXUS_KeySlotHandle videoKeyHandle = NULL;
    NEXUS_KeySlotHandle audioKeyHandle = NULL;
    NEXUS_SecurityAlgorithmSettings AlgConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_PidChannelStatus pidStatus;
    NEXUS_SecurityKeySlotSettings keySlotSettings;

	unsigned char cwAudioEven[8] = { 0xd3, 0x94, 0xdd, 0x44, 0x13, 0xd5, 0xd1, 0xb9 };	
	unsigned char cwAudioOdd[8] = { 0xd6, 0x91, 0xd8, 0x3f, 0x13, 0xd0, 0xd4, 0xb7 }; 
	unsigned char cwVideoEven[8] = { 0xbe, 0xf9, 0xb0, 0x67, 0x13, 0xb8, 0xbc, 0x87 };	
	unsigned char cwVideoOdd[8] = { 0xbc, 0xfb, 0xb2, 0x69, 0x13, 0xba, 0xbe, 0x8b };

	/* Allocate CA video keyslot */
	printf("Allocate CA video keyslot\n");
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	videoKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !videoKeyHandle)
	{
	    printf("\nAllocate CA video keyslot failed \n");
	    return ;
	}

	/* Add video PID channel to keyslot */
	printf("Add video PID channel to keyslot \n");
	NEXUS_PidChannel_GetStatus (pVideoProgram->pidChannel, &pidStatus);  
	if ( NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nAdd video PID channel to keyslot failed \n");
	    return ;
	}
 
	/* Config video CA Algorithm for both even and odd*/
	printf("Configure video CA Algorithm for both even and odd \n");
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;

	if ( NEXUS_Security_ConfigAlgorithm (videoKeyHandle, &AlgConfig) != 0 )
	{
	    printf("\nConfigure Video CA Algorithm Even failed \n");
	    return ;
	}
	
	/* Load Video keys */
	printf("Load video Odd key \n");
	key.keySize = 8; 
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	BKNI_Memcpy(key.keyData,cwVideoOdd,sizeof(cwVideoOdd));
	if ( NEXUS_Security_LoadClearKey (videoKeyHandle, &key) != 0 )
	{
	    printf("\nLoad Video Odd key failed \n");
	    return ;
	}

	printf("Load video ODD key \n");
	key.keyEntryType = NEXUS_SecurityKeyType_eEven;
	BKNI_Memcpy(key.keyData,cwVideoEven,sizeof(cwVideoEven));
	if ( NEXUS_Security_LoadClearKey (videoKeyHandle, &key) != 0 )
	{
	    printf("\nLoad Video Even key failed \n");
	    return ;
	}

	/* Allocate CA Audio keyslot */
	printf("Allocate CA Audio keyslot\n");	
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	audioKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !audioKeyHandle)
	{
	    printf("\nAllocate CA Audio Keyslot failed \n");
	    return ;
	}

	/* Add Audio PID channel to keyslot */
	printf("Add Audio PID channel to keyslot \n");
	NEXUS_PidChannel_GetStatus (pAudioProgram->pidChannel, &pidStatus);

	if ( NEXUS_Security_AddPidChannelToKeySlot(audioKeyHandle, pidStatus.pidChannelIndex)!= 0 )   
	{
	    printf("\nAdd Audio PID channel to keyslot failed \n");
	    return ;
	}

	/* Configure Audio CA Algorithm for both even and odd */
	printf("Configure Audio CA Algorithm for both even and odd \n");
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;

	if ( NEXUS_Security_ConfigAlgorithm (audioKeyHandle, &AlgConfig) != 0 )
	{
	    printf("\nConfigure Audio CA Algorithm failed \n");
	    return ;
	}

	/* Load AV keys */
	printf("Load Audio Odd key \n");
	key.keySize = 8; 
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	BKNI_Memcpy(key.keyData,cwAudioOdd,sizeof(cwAudioOdd));
	if ( NEXUS_Security_LoadClearKey (audioKeyHandle, &key) != 0 )
	{
	    printf("\nLoad Audio Odd key failed \n");
	    return ;
	}

	printf("Load Audio Even key \n");
	key.keyEntryType = NEXUS_SecurityKeyType_eEven;
	BKNI_Memcpy(key.keyData,cwAudioEven,sizeof(cwAudioEven));
	if ( NEXUS_Security_LoadClearKey (audioKeyHandle, &key) != 0 )
	{
	    printf("\nLoad Audio Even Key failed \n");
	    return ;
	}

	printf ("\nSecurity Configure OK\n");

	printf("Press ENTER to stop decode\n");
	getchar();
	getchar();

 	/* Remove Audio PID channel from keyslot */
	printf("Remove Audio PID channel from keyslot \n");
	NEXUS_PidChannel_GetStatus (pAudioProgram->pidChannel, &pidStatus);
	if ( NEXUS_Security_RemovePidChannelFromKeySlot(audioKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nRemove video PID channel from keyslot failed \n");
	    return ;
	}
   
 	/* Free CA Audio keyslot */
	printf("Free CA Audio keyslot\n");	
	NEXUS_Security_FreeKeySlot(audioKeyHandle);

 	/* Remove Video PID channel from keyslot */
	printf("Remove Video PID channel from keyslot \n");
	NEXUS_PidChannel_GetStatus (pVideoProgram->pidChannel, &pidStatus);  
	if ( NEXUS_Security_RemovePidChannelFromKeySlot(videoKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nRemove Video PID channel from keyslot failed \n");
	    return ;
	}

 	/* Free CA Video keyslot */
	printf("Free CA Video keyslot\n");	
	NEXUS_Security_FreeKeySlot(videoKeyHandle);

	printf ("\nFree Keyslot OK\n");
}

/*For this test , you need use 7550 OTP test sample Key */
void Descrmbler_Dvbcsa_OtpKeyLadder(NEXUS_VideoDecoderStartSettings *pVideoProgram, NEXUS_AudioDecoderStartSettings *pAudioProgram)
{
    NEXUS_KeySlotHandle videoKeyHandle = NULL;
    NEXUS_KeySlotHandle audioKeyHandle = NULL;
    NEXUS_SecurityAlgorithmSettings AlgConfig;
    NEXUS_PidChannelStatus pidStatus;
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
    NEXUS_SecurityEncryptedControlWord encrytedCW;
	
	unsigned char ucProcInForKey3[16] = { 		
	0x11,0x11,0x11,0x11 ,0x22,0x22,0x22,0x22,
	0x33,0x33,0x33,0x33 ,0x44,0x44,0x44,0x44,
		};

	unsigned char ucProcInKey4_AudioEven[8] = { 
	0x2D,0xF8,0x56,0xBF ,0x8B,0x3C,0xA4,0xAB
		};
	unsigned char ucProcInKey4_AudioOdd[8] = { 
	0x63,0xDE,0x8F,0xF5 ,0x09,0x25,0x65,0x69
		};
	unsigned char ucProcInKey4_VideoEven[8] = { 
	0x4F,0x09,0x18,0x78 ,0x2C,0xAC,0xB8,0x9F
		};
	unsigned char ucProcInKey4_VideoOdd[8] = { 
	0x63,0x0E,0x39,0xF7 ,0x15,0xBA,0x0E,0xD7
		};

	/* Allocate CA Video keyslot */
	printf("Allocate CA video keyslot\n");
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	videoKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !videoKeyHandle)
	{
	    printf("\nAllocate CA Video keyslot failed \n");
	    return ;
	}

	/* Add video PID channel to keyslot */
	printf("Add video PID channel to keyslot \n");
	NEXUS_PidChannel_GetStatus (pVideoProgram->pidChannel, &pidStatus);  
	if ( NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nAdd video PID channel to keyslot failed \n");
	    return ;
	}
 
	/* Config video CA Algorithm for both even and odd*/
	printf("Configure video CA Algorithm for both even and odd \n");
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;

	if ( NEXUS_Security_ConfigAlgorithm (videoKeyHandle, &AlgConfig) != 0 )
	{
	    printf("\nConfigure Video CA Algorithm Even failed \n");
	    return ;
	}

	/* Genearate key 2 and key3  */
	printf("Genearate key 2 and key3 \n");
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eB;
	encryptedSessionkey.swizzleType	= NEXUS_SecuritySwizzleType_eSwizzle0;
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 	= 0xFF; 
	encryptedSessionkey.cusKeyVarH 	= 0x00; 	
	encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eOtpKeyB;
	encryptedSessionkey.bRouteKey 		= false;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	if (NEXUS_Security_GenerateSessionKey(videoKeyHandle, &encryptedSessionkey) !=0)
	{
		printf("\nGenearate key 2 and key3 \n");
		return ;
	}

	/* Gnerate key 4 and route to Video Even key */
	printf("Gnerate key 4 and route to Video Even key \n");
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eB;
	encrytedCW.keySize = sizeof(ucProcInKey4_VideoEven); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_VideoEven, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(videoKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Video Even key failed\n");
		return ;
	}

	/* Gnerate key 4 and route to Video Odd key */
	printf("Gnerate key 4 and route to Video Odd key \n");
	encrytedCW.keySize = sizeof(ucProcInKey4_VideoOdd); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_VideoOdd, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord( videoKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Video Odd key failed\n");
		return ;
	}

	/* Allocate CA Audio keyslot */
	printf("Allocate CA Audio keyslot\n");	
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	audioKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !audioKeyHandle)
	{
	    printf("\nAllocate CA Audio Keyslot failed \n");
	    return ;
	}

	/* Add Audio PID channel to keyslot */
	printf("Add Audio PID channel to keyslot \n");
	NEXUS_PidChannel_GetStatus (pAudioProgram->pidChannel, &pidStatus);

	if ( NEXUS_Security_AddPidChannelToKeySlot(audioKeyHandle, pidStatus.pidChannelIndex)!= 0 )   
	{
	    printf("\nAdd Audio PID channel to keyslot failed \n");
	    return ;
	}

	/* Configure Audio CA Algorithm for both even and odd */
	printf("Configure Audio CA Algorithm for both even and odd \n");
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;

	if ( NEXUS_Security_ConfigAlgorithm (audioKeyHandle, &AlgConfig) != 0 )
	{
	    printf("\nConfigure Audio CA Algorithm failed \n");
	    return ;
	}
	
	/* Gnerate key 4 and route to Audio Odd key */
	printf("Gnerate key 4 and route to Audio Odd key \n");
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eB;
	encrytedCW.keySize = sizeof(ucProcInKey4_AudioOdd); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_AudioOdd, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(audioKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Audio Odd key failed\n");
		return ;
	}

	/* Gnerate key 4 and route to Audio Even key */
	printf("Gnerate key 4 and route to Audio Even key \n");
	encrytedCW.keySize = sizeof(ucProcInKey4_AudioEven); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_AudioEven, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(audioKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Audio Even key failed\n");
		return ;
	}

	printf ("\nSecurity Config OK\n");
	
	printf("Press ENTER to stop decode\n");
	getchar();
	getchar();

 	/* Remove Audio PID channel from keyslot */
	printf("Remove Audio PID channel from keyslot \n");
	NEXUS_PidChannel_GetStatus (pAudioProgram->pidChannel, &pidStatus);
	if ( NEXUS_Security_RemovePidChannelFromKeySlot(audioKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nRemove video PID channel from keyslot failed \n");
	    return ;
	}
   
 	/* Free CA Audio keyslot */
	printf("Free CA Audio keyslot\n");	
	NEXUS_Security_FreeKeySlot(audioKeyHandle);

 	/* Remove Video PID channel from keyslot */
	printf("Remove Video PID channel from keyslot \n");
	NEXUS_PidChannel_GetStatus (pVideoProgram->pidChannel, &pidStatus);  
	if ( NEXUS_Security_RemovePidChannelFromKeySlot(videoKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nRemove Video PID channel from keyslot failed \n");
	    return ;
	}

 	/* Free CA Video keyslot */
	printf("Free CA Video keyslot\n");	
	NEXUS_Security_FreeKeySlot(videoKeyHandle);

	printf ("\nFree Keyslot OK\n");
}

void Descrmbler_Dvbcsa_CustKeyLadder(NEXUS_VideoDecoderStartSettings *pVideoProgram, NEXUS_AudioDecoderStartSettings *pAudioProgram)
{
    NEXUS_KeySlotHandle videoKeyHandle = NULL;
    NEXUS_KeySlotHandle audioKeyHandle = NULL;
    NEXUS_SecurityAlgorithmSettings AlgConfig;
    NEXUS_PidChannelStatus pidStatus;
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
    NEXUS_SecurityEncryptedControlWord encrytedCW;
	
	unsigned char ucProcInForKey3[16] = { 		
	0x11,0x11,0x11,0x11 ,0x22,0x22,0x22,0x22,
	0x33,0x33,0x33,0x33 ,0x44,0x44,0x44,0x44,
		};

	unsigned char ucProcInKey4_AudioEven[8] = { 
	0x04,0x32,0x41,0xAC ,0xFF,0xFA,0x4F,0x24
		};
	unsigned char ucProcInKey4_AudioOdd[8] = { 
	0x65,0x53,0xD3,0x6C ,0xB4,0xF0,0xDE,0x3E
		};
	unsigned char ucProcInKey4_VideoEven[8] = { 
	0xED,0x2C,0x87,0x1C ,0xA5,0x54,0xB0,0x66
		};
	unsigned char ucProcInKey4_VideoOdd[8] = { 
	0x73,0xD1,0xA0,0x83 ,0xEF,0xEC,0xBE,0x12
		};

	/* Allocate CA Video keyslot */
	printf("Allocate CA video keyslot\n");
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	videoKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !videoKeyHandle)
	{
	    printf("\nAllocate CA Video keyslot failed \n");
	    return ;
	}

	/* Add video PID channel to keyslot */
	printf("Add video PID channel to keyslot \n");
	NEXUS_PidChannel_GetStatus (pVideoProgram->pidChannel, &pidStatus);  
	if ( NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nAdd video PID channel to keyslot failed \n");
	    return ;
	}
 
	/* Config video CA Algorithm for both even and odd*/
	printf("Configure video CA Algorithm for both even and odd \n");
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;

	if ( NEXUS_Security_ConfigAlgorithm (videoKeyHandle, &AlgConfig) != 0 )
	{
	    printf("\nConfigure Video CA Algorithm Even failed \n");
	    return ;
	}

	/* Genearate key 2 and key3  */
	printf("Genearate key 2 and key3 \n");
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eB;
	encryptedSessionkey.swizzleType	= NEXUS_SecuritySwizzleType_eSwizzle0; 
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 	= 0xFF; 
	encryptedSessionkey.cusKeyVarH 	= 0x00; 	
	encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eCuskey;
	encryptedSessionkey.bRouteKey 		= false;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	if (NEXUS_Security_GenerateSessionKey(videoKeyHandle, &encryptedSessionkey) !=0)
	{
		printf("\nGenearate key 2 and key3 failed \n");
		return ;
	}

	/* Gnerate key 4 and route to Video Even key */
	printf("Gnerate key 4 and route to Video Even key \n");
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eB;
	encrytedCW.keySize = sizeof(ucProcInKey4_VideoEven); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_VideoEven, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(videoKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Video Even key failed\n");
		return ;
	}

	/* Gnerate key 4 and route to Video ODD key  */
	printf("Gnerate key 4 and route to ODD key \n");
	encrytedCW.keySize = sizeof(ucProcInKey4_VideoOdd); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	/*encrytedCW.pKeyData = ucProcInKey4;*/
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_VideoOdd, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord( videoKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Video ODD key failed\n");
		return ;
	}

	/* Allocate CA Audio keyslot */
	printf("Allocate CA Audio keyslot\n");	
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	audioKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !audioKeyHandle)
	{
	    printf("\nAllocate CA Audio Keyslot failed \n");
	    return ;
	}

	/* Add Audio PID channel to keyslot */
	printf("Add Audio PID channel to keyslot \n");
	NEXUS_PidChannel_GetStatus (pAudioProgram->pidChannel, &pidStatus);

	if ( NEXUS_Security_AddPidChannelToKeySlot(audioKeyHandle, pidStatus.pidChannelIndex)!= 0 )   
	{
	    printf("\nAdd Audio PID channel to keyslot failed \n");
	    return ;
	}

	/* Configure Audio CA Algorithm for both even and odd */
	printf("Configure Audio CA Algorithm for both even and odd \n");
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;

	if ( NEXUS_Security_ConfigAlgorithm (audioKeyHandle, &AlgConfig) != 0 )
	{
	    printf("\nConfigure Audio CA Algorithm failed \n");
	    return ;
	}
	
	/* Gnerate key 4 and route to Audio ODD key  */
	printf("Gnerate key 4 and route to Audio ODD key \n");
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eB;
	encrytedCW.keySize = sizeof(ucProcInKey4_AudioOdd); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	/*encrytedCW.pKeyData = ucProcInKey4;*/
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_AudioOdd, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(audioKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Audio ODD key failed\n");
		return ;
	}

	/* Gnerate key 4 and route to Audio Even key  */
	printf("Gnerate key 4 and route to Audio Even key \n");
	encrytedCW.keySize = sizeof(ucProcInKey4_AudioEven); 
	encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
	/*encrytedCW.pKeyData = ucProcInKey4;*/
	BKNI_Memcpy(encrytedCW.keyData, ucProcInKey4_AudioEven, encrytedCW.keySize);
	encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(audioKeyHandle, &encrytedCW))
	{
		printf("\nGnerate key 4 and route to Audio Even key failed\n");
		return ;
	}
	
	printf ("\nSecurity Config OK\n");

	printf("Press ENTER to stop decode\n");
	getchar();
	getchar();

 	/* Remove Audio PID channel from keyslot */
	printf("Remove Audio PID channel from keyslot \n");
	NEXUS_PidChannel_GetStatus (pAudioProgram->pidChannel, &pidStatus);
	if ( NEXUS_Security_RemovePidChannelFromKeySlot(audioKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nRemove video PID channel from keyslot failed \n");
	    return ;
	}
   
 	/* Free CA Audio keyslot */
	printf("Free CA Audio keyslot\n");	
	NEXUS_Security_FreeKeySlot(audioKeyHandle);

 	/* Remove Video PID channel from keyslot */
	printf("Remove Video PID channel from keyslot \n");
	NEXUS_PidChannel_GetStatus (pVideoProgram->pidChannel, &pidStatus);  
	if ( NEXUS_Security_RemovePidChannelFromKeySlot(videoKeyHandle, pidStatus.pidChannelIndex)!= 0 )
	{
	    printf("\nRemove Video PID channel from keyslot failed \n");
	    return ;
	}

 	/* Free CA Video keyslot */
	printf("Free CA Video keyslot\n");	
	NEXUS_Security_FreeKeySlot(videoKeyHandle);

	printf ("\nFree Keyslot OK\n");
}

