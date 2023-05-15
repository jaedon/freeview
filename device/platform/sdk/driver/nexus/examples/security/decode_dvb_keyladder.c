/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
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
 * $brcm_Workfile: decode_dvb_keyladder.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 1/27/12 11:46a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/examples/security/decode_dvb_keyladder.c $
 * 
 * 6   1/27/12 11:46a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 5   1/7/11 3:24p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 4   9/28/09 2:14p yili
 * SW7340-42:Bug fix
 * 
 * 3   5/8/09 11:18a yili
 * PR53099:Modified to accomendate Nexus Interface change.
 * 
 * 2   4/15/09 3:18p yili
 * PR53745:Minor cleanup
 * 
 * 1   4/6/09 5:31p yili
 * PR53745:Add keyladder sample code
 * 
 *****************************************************************************/

/* Nexus example app: single live a/v descrambling and decode from an input band */
/* Test stream is superscrambler_spider.ts */

/* This example shows how to use keyladder with Cuskey+Swizzle for root key generation.
   Due to differences in HW design, the value provided in this sample only works for 
   settop chips like 740x.  For other chips, the key values may need to be changed for 
   the test to pass
*/
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
#include "nexus_keyladder.h"

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

	unsigned char ucProcInForKey3[16] = { 		
		0x0f, 0x09, 0xa2, 0x06,  /*high key */  
		0x19, 0x88, 0xb6, 0x89,	    
		0x28, 0xeb, 0x90, 0x2e,   /*low key  use twice */
		0xb2, 0x36, 0x18, 0x88};

	/* DVB only requires 8 bytes procins for key4 generation since the DBV key size is 8 bytes */
	/* For AES128 and 3DES-ABA, the procinsize shall be 16 bytes */
	unsigned char ucProcInForVidOddKey4[8] = { 0x27, 0x9e, 0xc9, 0x6c, 0xc8, 0x66, 0xec, 0x10 };
	unsigned char ucProcInForVidEvenKey4[8] = { 0xe4, 0x62, 0x75, 0x1b, 0x5d, 0xd4, 0xbc, 0x02 };
	unsigned char ucProcInForAudOddKey4[8] = { 0x57, 0x94, 0x79, 0x23, 0x46, 0x6a, 0x9f, 0xa5 };
	unsigned char ucProcInForAudEvenKey4[8] = { 0x9d, 0xd1, 0xe3, 0x5b, 0xf8, 0xa6, 0x89, 0x06 };

	unsigned int videoPID, audioPID;
	NEXUS_KeySlotHandle videoKeyHandle = NULL;
	NEXUS_KeySlotHandle audioKeyHandle = NULL;
	NEXUS_SecurityAlgorithmSettings AlgConfig;
	NEXUS_PidChannelStatus pidStatus;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord encrytedCW;

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
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
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
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	/* Allocate AV keyslots */
    videoKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !videoKeyHandle)
	{
		printf("\nAllocate CA video keyslot failed \n");
		return 1;
	}
    audioKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if ( !audioKeyHandle)
	{
		printf("\nAllocate CA audio keyslot failed \n");
		return 1;
	}

	/* Config AV algorithms */
    NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
	AlgConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
	if ( NEXUS_Security_ConfigAlgorithm (videoKeyHandle, &AlgConfig) != 0 )
	{
		printf("\nConfig video CA Algorithm failed \n");
		return 1;
	}
 	if ( NEXUS_Security_ConfigAlgorithm (audioKeyHandle, &AlgConfig) != 0 )
	{
		printf("\nConfig video CA Algorithm failed \n");
		return 1;
	}
       
	/* Load AV keys using keyladder */
	/* Generate session key */
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eB;
	encryptedSessionkey.keyladderType 	= NEXUS_SecurityKeyladderType_e3Des;
	encryptedSessionkey.swizzleType		= NEXUS_SecuritySwizzleType_eSwizzle0;
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL 		= 0x00; 
	encryptedSessionkey.cusKeyVarH 		= 0xFF; 
    encryptedSessionkey.rootKeySrc 		= NEXUS_SecurityRootKeySrc_eCuskey;
	encryptedSessionkey.bRouteKey 		= false;
	encryptedSessionkey.operation 		= NEXUS_SecurityOperation_eDecrypt;
	encryptedSessionkey.operationKey2 	= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.keyEntryType 	= NEXUS_SecurityKeyType_eEven;
	/*encryptedSessionkey.pKeyData 		= ucProcInForKey3;*/
	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	if (NEXUS_Security_GenerateSessionKey(videoKeyHandle, &encryptedSessionkey) !=0)
	{
		printf("\nLoading session key failed for video\n");
		return 1;
	}
	if (NEXUS_Security_GenerateSessionKey(audioKeyHandle, &encryptedSessionkey) !=0)
	{
		printf("\nLoading session key failed for audio\n");
		return 1;
	}

	/* Load CW for auido and video */
	encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eB;
    encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    encrytedCW.keySize = sizeof(ucProcInForVidOddKey4); 
    encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
     /*encrytedCW.pKeyData = ucProcInForVidOddKey4;*/
 	BKNI_Memcpy(encrytedCW.keyData, ucProcInForVidOddKey4, encrytedCW.keySize );
    encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;     
    encrytedCW.bRouteKey = true;
	if (NEXUS_Security_GenerateControlWord(videoKeyHandle, &encrytedCW))
	{
		printf("\nLoading session key failed for video ODD key\n");
		return 1;
	}
    encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
    /*encrytedCW.pKeyData = ucProcInForVidEvenKey4;*/
 	BKNI_Memcpy(encrytedCW.keyData, ucProcInForVidEvenKey4, encrytedCW.keySize);
	if (NEXUS_Security_GenerateControlWord(videoKeyHandle, &encrytedCW))
	{
		printf("\nLoading session key failed for video EVEN key\n");
		return 1;
	}

    encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    /*encrytedCW.pKeyData = ucProcInForAudOddKey4;*/
 	BKNI_Memcpy(encrytedCW.keyData, ucProcInForAudOddKey4, encrytedCW.keySize);
	if (NEXUS_Security_GenerateControlWord(audioKeyHandle, &encrytedCW))
	{
		printf("\nLoading session key failed for audio ODD key\n");
		return 1;
	}
    encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
    /*encrytedCW.pKeyData = ucProcInForAudEvenKey4;*/
 	BKNI_Memcpy(encrytedCW.keyData, ucProcInForAudEvenKey4, encrytedCW.keySize);
	if (NEXUS_Security_GenerateControlWord(audioKeyHandle, &encrytedCW))
	{
		printf("\nLoading session key failed for audio EVEN key\n");
		return 1;
	}

	/* Add video PID channel to keyslot */
	NEXUS_PidChannel_GetStatus (videoProgram.pidChannel, &pidStatus);
	videoPID = pidStatus.pidChannelIndex;
	if ( NEXUS_Security_AddPidChannelToKeySlot(videoKeyHandle, videoPID)!= 0 )
	{
		printf("\nConfigPIDPointerTable failed \n");
		return 1;
	}

	/* Add video PID channel to keyslot */
	NEXUS_PidChannel_GetStatus (audioProgram.pidChannel, &pidStatus);
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
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

#if 1
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
    printf("Press ENTER to stop decode\n");
    getchar();

    /* example shutdown */
	NEXUS_Security_RemovePidChannelFromKeySlot(videoKeyHandle, videoPID);
	NEXUS_Security_RemovePidChannelFromKeySlot(audioKeyHandle, audioPID);
	NEXUS_Security_FreeKeySlot(videoKeyHandle);
	NEXUS_Security_FreeKeySlot(audioKeyHandle);

    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
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
