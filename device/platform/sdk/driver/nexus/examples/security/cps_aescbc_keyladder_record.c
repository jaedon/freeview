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
 * $brcm_Workfile: cps_aescbc_keyladder_record.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 12/19/11 2:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/security/cps_aescbc_keyladder_record.c $
 * 
 * 1   12/19/11 2:45p mphillip
 * SW7425-1932: Add new security examples
 *
 * 
 *
 *****************************************************************************/
 /* Example record CPS AES-CBC encryption with keyladder  */
 /* This example makes a pair with  playback using "m2m_aescbc_keyladder_playback.c " */
 
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"

#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif

#include "nexus_dma.h"
#include "nexus_security.h"
#include "nexus_keyladder.h"
#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#define NUM_STATIC_CHANNELS             4
#define MAX_PROGRAM_TITLE_LENGTH       128
#define NUM_AUDIO_STREAMS               4
#define NUM_VIDEO_STREAMS               4
#define MAX_STREAM_PATH                 4

NEXUS_FrontendUserParameters userParams;
NEXUS_FrontendHandle frontend=NULL;
NEXUS_FrontendQamSettings qamSettings;
NEXUS_PlatformConfiguration platformConfig;
NEXUS_ParserBand parserBand;
NEXUS_ParserBandSettings parserBandSettings;
NEXUS_FrontendCapabilities capabilities;
NEXUS_FrontendQamStatus qamStatus;
unsigned frontendIndex;

static int currentChannel = 1; 

typedef struct g_channelInfo
{
    NEXUS_FrontendQamAnnex annex; 
    NEXUS_FrontendQamMode modulation;
    unsigned int  symbolrate;
    unsigned int  frequency;
    char programTitle[MAX_PROGRAM_TITLE_LENGTH];
    int numAudioStreams;
    int numVideoStreams;
    unsigned int  videoPids[NUM_VIDEO_STREAMS];
    unsigned int  audioPids[NUM_AUDIO_STREAMS];
    unsigned int  pcrPid[NUM_VIDEO_STREAMS];
    NEXUS_VideoCodec videoFormat[NUM_VIDEO_STREAMS];
    NEXUS_AudioCodec audioFormat[NUM_AUDIO_STREAMS];
} g_channelInfo;

static g_channelInfo channelInfo[NUM_STATIC_CHANNELS] =
{
    {NEXUS_FrontendQamAnnex_eB,NEXUS_FrontendQamMode_e64,5056941,429000000,"hockey",1,1,{0x11},
    {0x14},{0x11},{NEXUS_VideoCodec_eMpeg2},{NEXUS_AudioCodec_eAc3}},

    {NEXUS_FrontendQamAnnex_eB,NEXUS_FrontendQamMode_e64,5056941,435000000,"color beat",1,1,{0x11},
    {0x14},{0x11},{NEXUS_VideoCodec_eMpeg2},{NEXUS_AudioCodec_eAc3}},

    {NEXUS_FrontendQamAnnex_eB,NEXUS_FrontendQamMode_e64,5056941,447000000,"hockey match", 3,3,{0x11,0x21,0x41},
    {0x14,0x24,0x44},{0x11,0x21,0x41},{NEXUS_VideoCodec_eMpeg2,NEXUS_VideoCodec_eMpeg2,NEXUS_VideoCodec_eMpeg2},
    {NEXUS_AudioCodec_eAc3,NEXUS_AudioCodec_eAc3,NEXUS_AudioCodec_eAc3}},

    {NEXUS_FrontendQamAnnex_eB,NEXUS_FrontendQamMode_e256,5360537,525000000,"sc_bugs_baywatch",3,3,{0x31,0x11,0x21},
    {0x34,0x14,0x24},{0x31,0x11,0x21},{NEXUS_VideoCodec_eMpeg2,NEXUS_VideoCodec_eMpeg2,NEXUS_VideoCodec_eMpeg2},
    {NEXUS_AudioCodec_eAc3,NEXUS_AudioCodec_eAc3,NEXUS_AudioCodec_eAc3}},
};

void show_ChannelMap(void)
{
    int channelCount, esStreamCount;
    for (channelCount = 0; channelCount < NUM_STATIC_CHANNELS; channelCount++)
    {
        printf("channel index (%d)\n", channelCount);
        printf("\tannex (%d)\n",channelInfo[channelCount].annex);
        printf("\tmodulation (%d)\n",channelInfo[channelCount].modulation);
        printf("\tfrequency (%d)\n", channelInfo[channelCount].frequency);
        printf("\tsymbolrate (%d)\n", channelInfo[channelCount].symbolrate);
        printf("\tprogram_title (%s)\n",channelInfo[channelCount].programTitle);
        printf("\tno of audio streams (%d)\n",channelInfo[channelCount].numAudioStreams);
        printf("\tno of video streams (%d)\n",channelInfo[channelCount].numVideoStreams);
        printf("\tvideo_pids/format ");
        for(esStreamCount=0;esStreamCount<channelInfo[channelCount].numVideoStreams;esStreamCount++)
        {
         printf("0x%x/%d ",channelInfo[channelCount].videoPids[esStreamCount],channelInfo[channelCount].videoFormat[esStreamCount]);
        }
        printf("\n");
        printf("\taudioPids/formats " );
        for(esStreamCount=0;esStreamCount<channelInfo[channelCount].numAudioStreams;esStreamCount++)
        {
         printf("0x%x/%d ", channelInfo[channelCount].audioPids[esStreamCount],channelInfo[channelCount].audioFormat[esStreamCount]);
        }
        printf("\n");
        printf("\tpcrPid ");
        for(esStreamCount=0;esStreamCount<channelInfo[channelCount].numVideoStreams;esStreamCount++)
        {
          printf("0x%x ",channelInfo[channelCount].pcrPid[channelCount]);
        }
        printf("\n");
    }
}

void tuneChannel(int channel)
{
	NEXUS_Frontend_GetUserParameters(frontend, &userParams);
	parserBand = (NEXUS_ParserBand)userParams.param1;
	NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
	parserBandSettings.sourceTypeSettings.inputBand = userParams.param1; 
	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	parserBandSettings.transportType = NEXUS_TransportType_eTs;
	NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
	
    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = channelInfo[channel].frequency;
	qamSettings.mode = channelInfo[channel].modulation;
	qamSettings.annex = channelInfo[channel].annex;
    qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;

	NEXUS_Frontend_TuneQam(frontend,&qamSettings);
	BKNI_Sleep(2000);
	NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
	printf("receiver lock = %d\n",qamStatus.receiverLock);
	printf("symbol Rate = %d\n",qamStatus.symbolRate);		
}

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
	NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;
	NexusConfig.bRestrictEnable                                      = false;
	NexusConfig.bEncryptBeforeRave                                   = false;   
 	NexusConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted]  = true;
 	NexusConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal]      = true; 
 	NexusConfig.scValue[NEXUS_SecurityPacketType_eRestricted]        = NEXUS_SecurityAlgorithmScPolarity_eClear;
 	NexusConfig.scValue[NEXUS_SecurityPacketType_eGlobal]            = NEXUS_SecurityAlgorithmScPolarity_eClear;
	NexusConfig.operation = operation;

	if ( NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig)!= 0)
	{
		printf("\nConfigAlg clear keyslot failed \n");
		return 1;
	}

	NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eClear;
	if ( NEXUS_Security_ConfigAlgorithm (keyHandle, &NexusConfig) != 0 )
	{
		printf("\nConfig video CPS Clear Algorithm failed \n");
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


	encryptedSessionkey.custSubMode 	   = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
	encryptedSessionkey.keyMode 		   = NEXUS_SecurityKeyMode_eRegular;
	encryptedSessionkey.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;

	BKNI_Memcpy(encryptedSessionkey.keyData, ucProcInForKey3, sizeof(ucProcInForKey3));

	encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eOdd;
	if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) != 0)
	{
		printf("\nLoading session key failed \n");
		return 1;
	}

	encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eEven;
	if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) != 0)
	{
		printf("\nLoading session key failed \n");
		return 1;
	}

	encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eClear;
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
	encrytedCW.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;

	encrytedCW.keyEntryType  = NEXUS_SecurityKeyType_eOdd;
	if (NEXUS_Security_GenerateControlWord(keyHandle, &encrytedCW))
	{
		printf("\nGenerate Control Word failed for Key Slot Handle %x  Odd\n", (unsigned int)keyHandle);
		return 1;
	}

	encrytedCW.keyEntryType  = NEXUS_SecurityKeyType_eEven;
	if (NEXUS_Security_GenerateControlWord(keyHandle, &encrytedCW))
	{
		printf("\nGenerate Control Word failed for Key Slot Handle %x  Odd\n", (unsigned int)keyHandle);
		return 1;
	}

	encrytedCW.keyEntryType  = NEXUS_SecurityKeyType_eClear;
	if (NEXUS_Security_GenerateControlWord(keyHandle, &encrytedCW))
	{
		printf("\nGenerate Control Word failed for Key Slot Handle %x  Clear\n", (unsigned int)keyHandle);
		return 1;
	}

	return (0);

}
#endif

int main(void) {
#if NEXUS_HAS_SECURITY && (NEXUS_SECURITY_CHIP_SIZE==40)
#if NEXUS_HAS_RECORD

	NEXUS_StcChannelHandle stcChannel;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_DisplayHandle display;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoWindowHandle window;
	NEXUS_FileRecordHandle file;
	NEXUS_RecpumpHandle recpump;
	NEXUS_RecordHandle record;
	NEXUS_RecordPidChannelSettings pidSettings;
	NEXUS_RecordSettings recordSettings;
	NEXUS_PidChannelHandle pidChannel[2];
	NEXUS_SecurityClearKey key;
	NEXUS_DmaHandle dma;
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_VideoDecoderHandle vdecode;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_AudioDecoderHandle pcmDecoder;
	NEXUS_PidChannelStatus pidStatus;
    NEXUS_KeySlotHandle keyHandle;
    NEXUS_KeySlotHandle keyHandle2;
	unsigned int videoPID, audioPID;

	const char  *fname = "videos/aescbc_keyladder.mpg";
	const char *index = "videos/aescbc_keyladder.idx";
    uint8_t ivkeys[16] = { 0xad, 0xd6, 0x9e, 0xa3,0x89, 0xc8, 0x17, 0x72, 0x1e, 0xd4, 0x0e, 0xab,0x3d, 0xbc, 0x7a, 0xf2 };

	show_ChannelMap();

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);

    NEXUS_Platform_GetConfiguration(&platformConfig);

    for ( frontendIndex = 0; frontendIndex < MAX_STREAM_PATH; frontendIndex++ )
    {
        frontend = platformConfig.frontend[frontendIndex];
		
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            /* Does this frontend support qam? */
            if ( capabilities.qam )
            {
				NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
				tuneChannel(currentChannel);				
                break;
            }
        }
    }

    if (NULL == frontend )
    {
        fprintf(stderr, "Unable to find QAM-capable frontend\n");
        return 0;
    }


    dma = NEXUS_Dma_Open(0, NULL);

	/* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);

    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

	NEXUS_AudioOutput_AddInput(
		  NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
		  NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

	/* configure the key slot and set up the key ladder for decryption */
	if (keySlotSetup(keyHandle, NEXUS_SecurityOperation_eEncrypt))
	{
		printf("\nKeyslot setup for decryption failed \n");
		return 1;
	}

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keySize = 16;
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keyIVType	 = NEXUS_SecurityKeyIVType_eIV;
	key.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;

	BKNI_Memcpy(key.keyData, ivkeys, key.keySize );	

	NEXUS_Security_LoadClearKey(keyHandle, &key);

    keyHandle2 = NEXUS_Security_AllocateKeySlot(&keySettings);	

	if (keySlotSetup(keyHandle2, NEXUS_SecurityOperation_eEncrypt))
	{
		printf("\nKeyslot setup for decryption failed \n");
		return 1;
	}		

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keySize = 16;
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keyIVType	 = NEXUS_SecurityKeyIVType_eIV;
	key.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps;
	BKNI_Memcpy(key.keyData, ivkeys, key.keySize );	
	NEXUS_Security_LoadClearKey(keyHandle2, &key);

	recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();

    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    NEXUS_Record_SetSettings(record, &recordSettings);

    file = NEXUS_FileRecord_OpenPosix(fname,  index);

    /* configure the video pid for indexing */
    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[0], &pidSettings);

    /* the audio pid requires no special configuration */
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
	displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

	NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));	

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = pidChannel[0];
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = pidChannel[1];
    audioProgram.stcChannel = stcChannel;

	NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
	stcSettings.modeSettings.pcr.pidChannel = pidChannel[0]; /* PCR happens to be on video pid */
	NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);

	NEXUS_PidChannel_GetStatus (pidChannel[0], &pidStatus);
	videoPID = pidStatus.pidChannelIndex;
	if ( NEXUS_Security_AddPidChannelToKeySlot(keyHandle, videoPID)!= 0 )
	{
		printf("\nConfigPIDPointerTable failed \n");
		return 1;
	}

	NEXUS_PidChannel_GetStatus (pidChannel[1], &pidStatus);
	audioPID = pidStatus.pidChannelIndex;
	NEXUS_Security_AddPidChannelToKeySlot(keyHandle2, audioPID); 



    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);

    NEXUS_Record_Start(record, file);

    /* Nexus is now recording to disk */

	printf("Nexus is now recording to disk using cps keyladder  \n");
	printf("press ENTER to stop record\n");
	getchar();

 
    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);
    NEXUS_FileRecord_Close(file);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_Platform_Uninit();

#endif
#endif
    return 0;
}
