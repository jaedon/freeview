/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: m2m_aescbc_keyladder_playback.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 12/19/11 2:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/security/m2m_aescbc_keyladder_playback.c $
 * 
 * 1   12/19/11 2:45p mphillip
 * SW7425-1932: Add new security examples
 *
 *****************************************************************************/
 /* Example m2m descramble playback for encrypted stream with keyladder   */
 /* This example makes a pair with record using "cps_aescbc_keyladder_record.c "  or " m2m_aescbc_keyladder_record.c" */

#include "nexus_platform.h"
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
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "nexus_security.h"
#include "nexus_keyladder.h"

#include "nexus_dma.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(playback_encrypted);


#if NEXUS_HAS_SECURITY && (NEXUS_SECURITY_CHIP_SIZE==40)
    /* Some aspects of this configuration are specific to 40nm HSM */
int keySlotSetup(
		NEXUS_KeySlotHandle      keyHandle,
		NEXUS_SecurityOperation  operation
)
{
	NEXUS_SecurityAlgorithmSettings 	NexusConfig;
	NEXUS_SecurityEncryptedSessionKey	encryptedSessionkey;
	NEXUS_SecurityEncryptedControlWord	encrytedCW;

	unsigned char ucProcInForKey3[16] = { 		
		0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,	    
		0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88};

	unsigned char ucProcInForKey4[16] = { 
		0xe4, 0x62, 0x75, 0x1b, 0x5d, 0xd4, 0xbc, 0x02, 
	    0x27, 0x9e, 0xc9, 0x6c, 0xc8, 0x66, 0xec, 0x10};


	/* configure the key slot */
	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
	NexusConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes;
	NexusConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCbc;
	NexusConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
	NexusConfig.ivMode				= NEXUS_SecurityIVMode_eRegular;
	NexusConfig.solitarySelect		= NEXUS_SecuritySolitarySelect_eClear;
	NexusConfig.caVendorID			= 0x1234;
	NexusConfig.askmModuleID		= NEXUS_SecurityAskmModuleID_eModuleID_4;
	NexusConfig.otpId				= NEXUS_SecurityOtpId_eOtpVal;
	NexusConfig.key2Select			= NEXUS_SecurityKey2Select_eReserved1;
	NexusConfig.operation			= operation;
	NexusConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eOdd;
	NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;

	if ( NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig)!= 0)
	{
		printf("\nConfigAlg clear keyslot failed \n");
		return 1;
	}

	/* set up the key ladder to route out the key for (operation) */
	NEXUS_Security_GetDefaultSessionKeySettings(&encryptedSessionkey);
	encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA; 
	encryptedSessionkey.keyladderType	= NEXUS_SecurityKeyladderType_e3Des;
	encryptedSessionkey.swizzleType 	= NEXUS_SecuritySwizzleType_eNone;
	encryptedSessionkey.cusKeyL 		= 0x00; 
	encryptedSessionkey.cusKeyH 		= 0x00; 
	encryptedSessionkey.cusKeyVarL		= 0x00; 
	encryptedSessionkey.cusKeyVarH		= 0xFF; 
	encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	encryptedSessionkey.sessionKeyOp	= NEXUS_SecuritySessionKeyOp_eNoProcess;
	encryptedSessionkey.bASKMMode		= false;
	encryptedSessionkey.rootKeySrc		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
	encryptedSessionkey.bSwapAESKey 	= false;
	encryptedSessionkey.keyDestIVType	= NEXUS_SecurityKeyIVType_eNoIV;
	encryptedSessionkey.bRouteKey		= false;
	encryptedSessionkey.operation		= NEXUS_SecurityOperation_eDecrypt;
	encryptedSessionkey.operationKey2	= NEXUS_SecurityOperation_eEncrypt;
	encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eOdd;

	encryptedSessionkey.custSubMode 	   = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	encryptedSessionkey.keyMode 		   = NEXUS_SecurityKeyMode_eRegular;
	encryptedSessionkey.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;

	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) != 0)
	{
		printf("\nLoading session key failed \n");
		return 1;
	}

	/* Load CW - key4 -- routed out */
	NEXUS_Security_GetDefaultControlWordSettings(&encrytedCW);
	encrytedCW.keyladderID   = NEXUS_SecurityKeyladderID_eA;
	encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	encrytedCW.keySize       = sizeof(ucProcInForKey4); 
	encrytedCW.keyEntryType  = NEXUS_SecurityKeyType_eOdd;
	encrytedCW.custSubMode		  = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	encrytedCW.keyMode			  = NEXUS_SecurityKeyMode_eRegular;

	BKNI_Memcpy(encrytedCW.keyData, ucProcInForKey4, encrytedCW.keySize);
	encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;	 
	encrytedCW.bRouteKey = true;
	encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	encrytedCW.keyGenCmdID	 = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	encrytedCW.bSwapAESKey	 = false;
	encrytedCW.bASKMMode	 = false; 
	encrytedCW.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	
	if (NEXUS_Security_GenerateControlWord(keyHandle, &encrytedCW))
	{
		printf("\nGenerate Control Word failed for Key Slot Handle %x\n", (unsigned int)keyHandle);
		return 1;
	}

	return (0);

}
#endif


int main(void)  {
	int command;
#if NEXUS_HAS_SECURITY && (NEXUS_SECURITY_CHIP_SIZE==40)
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	NEXUS_SecurityInvalidateKeySettings   invSettings;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityClearKey key;
    NEXUS_DmaHandle dma;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_PlaybackTrickModeSettings trick;

    const char *fname = "videos/aescbc_keyladder.mpg";
    const char *index = "videos/aescbc_keyladder.idx";
	uint8_t ivkeys[16] = { 0xad, 0xd6, 0x9e, 0xa3,0x89, 0xc8, 0x17, 0x72, 0x1e, 0xd4, 0x0e, 0xab,0x3d, 0xbc, 0x7a, 0xf2 };

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    dma = NEXUS_Dma_Open(0, NULL);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!keyHandle) {
        printf("\nAllocate enc keyslot failed \n");
        return 1;
    }

	invSettings.keyDestEntryType  = NEXUS_SecurityKeyType_eOddAndEven;
	invSettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
	invSettings.keyDestBlckType   = NEXUS_SecurityAlgorithmConfigDestination_eCa;
	invSettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;
	NEXUS_Security_InvalidateKey(keyHandle, &invSettings);

	invSettings.keyDestEntryType  = NEXUS_SecurityKeyType_eOddAndEven;
	invSettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
	invSettings.keyDestBlckType   = NEXUS_SecurityAlgorithmConfigDestination_eCps;
	invSettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;
	NEXUS_Security_InvalidateKey(keyHandle, &invSettings);

	invSettings.keyDestEntryType  = NEXUS_SecurityKeyType_eOddAndEven;
	invSettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
	invSettings.keyDestBlckType   = NEXUS_SecurityAlgorithmConfigDestination_eCpd;
	invSettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;
	NEXUS_Security_InvalidateKey(keyHandle, &invSettings);

	/* configure the key slot and set up the key ladder for decryption */
	if (keySlotSetup(keyHandle, NEXUS_SecurityOperation_eDecrypt))
	{
		printf("\nKeyslot setup for decryption failed \n");
		return 1;
	}

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keySize = 16;
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keyIVType	 = NEXUS_SecurityKeyIVType_eIV;
	key.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	BKNI_Memcpy(key.keyData, ivkeys, key.keySize ); 
	NEXUS_Security_LoadClearKey(keyHandle, &key);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    file = NEXUS_FilePlay_OpenPosix(fname, index);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
#if 0 /* move "playpumpSettings" about security context to playpump instead of playback */
    playbackSettings.playpumpSettings.securityDma = dma;
	playbackSettings.playpumpSettings.securityDmaDataFormat= NEXUS_DmaDataFormat_eMpeg;
    playbackSettings.playpumpSettings.securityContext = decKeyHandle;
#endif
	playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
#if 0
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#else
NEXUS_AudioOutput_AddInput(
	  NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
	  NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

#endif
    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
#if NEXUS_NUM_COMPONENT_OUTPUTS
   if(platformConfig.outputs.component[0]){
    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
   }
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif

	NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));

    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x11, &playbackPidSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
	playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x14, &playbackPidSettings);

	NEXUS_Playpump_GetSettings(playpump,&playpumpSettings);
	playpumpSettings.securityDma = dma;
    playpumpSettings.securityDmaDataFormat= NEXUS_DmaDataFormat_eMpeg;
	playpumpSettings.securityContext = keyHandle;
	NEXUS_Playpump_SetSettings(playpump,&playpumpSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Start decoders */
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    /* Start playback */
    NEXUS_Playback_Start(playback, file, NULL);

    /* Playback state machine is driven from inside Nexus. */
    printf("Press ENTER to quit\n");

	fflush(stdin);
	do{
		printf("[1] 1x Rewind\n");
		printf("[2] 2x Fast Forward\n");		
		printf("[3] Pause\n");
		printf("[4] Resume\n");
		printf("[0] quit\n ");

		
		scanf("%d",&command);
		switch(command)
		{
			case 1: 
				printf("\n1x Rewind\n");
				NEXUS_Playback_GetDefaultTrickModeSettings(&trick);
				trick.rate = -NEXUS_NORMAL_PLAY_SPEED;
				NEXUS_Playback_TrickMode(playback, &trick);
				break;
			case 2: 
				printf("\n2x FF\n");
				NEXUS_Playback_GetDefaultTrickModeSettings(&trick);
				trick.rate = 2*NEXUS_NORMAL_PLAY_SPEED;
				NEXUS_Playback_TrickMode(playback, &trick);
				break;
			case 3: 
				 printf("\nPause\n");
				 NEXUS_Playback_Pause(playback);
				 break;
			case 4: 
				printf("\n Resume\n");
				NEXUS_Playback_Play(playback);
				break;

			case 99: printf("Quit \n");break;
		}
	}while ( 0 != command );

    getchar();   
    /* Bring down system */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();

#endif
#endif
    return 0;
}
