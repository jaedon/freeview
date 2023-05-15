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
* $brcm_Workfile: encode_audio_es.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/5/12 10:37a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/encoder/encode_audio_es.c $
* 
* Hydra_Software_Devel/2   10/5/12 10:37a hongtaoz
* SW7435-394: updated output codec list and removed input pcr pid
* setting;
* 
* Hydra_Software_Devel/1   4/8/11 6:49p hongtaoz
* SW7425-165: move configurable nexus transcode unittests to rockford;
* 
* 2   3/16/11 4:32p hongtaoz
* SW7425-165: merge fix to mainline;
* 
* SW7425-165/2   3/14/11 6:43p vanessah
* SW7425-165: fix makefile error and some UI issue
* 
* 2   2/25/11 3:08p vsilyaev
* SW7425-39, SW7425-40: Merged examples
* 
* SW7425_bringup/6   2/20/11 4:44p hongtaoz
* SW7425-85: add AC3->AAC transcode test;
* 
* SW7425_bringup/5   2/7/11 9:09p hongtaoz
* SW7425-41: adapted to the new API;
* 
* SW7425_bringup/4   1/28/11 6:06p hongtaoz
* SW7425-41: added STC channel config to audio transcoder;
* 
* SW7425_bringup/3   1/19/11 8:08p hongtaoz
* SW7425-41: init descs number in for loop;
* 
* SW7425_bringup/2   1/18/11 7:46p hongtaoz
* SW7425-41: added audio descriptor capture to the test;
* 
* SW7425_bringup/1   1/17/11 11:21p hongtaoz
* SW7425-41: add encode_audio example to transcode audio es output;
* 
* 2   1/3/11 11:14a jgarrett
* SW7425-41: Fixing playpump assignment
* 
* 1   12/30/10 12:10p jgarrett
* SW7425-41: Adding mux output example
* 
* 5   12/29/10 12:32p vsilyaev
* SW7425-39: Updated shutdown sequence
* 
* 4   12/28/10 5:45p vsilyaev
* SW7425-39: Open Audio/Video PidChannel inside the stream mux module
* 
* 3   12/16/10 12:39p vsilyaev
* SW7425-39, SW7425-40: Merged into mainline
* 
* SW7425_bringup/2   12/15/10 10:51p hongtaoz
* SW7425-39, SW7425-40: updated examples with missed calls and re-ordered
*  encoder/display open sequence;
* 
* SW7425_bringup/1   12/15/10 5:34p hongtaoz
* SW7425-39, SW7425-40: updated default encoder display index and encoder
*  settings for 7425 bringup; fixed typo for PCR playpump PID channel
*  open;
* 
* 2   12/13/10 6:59p vsilyaev
* SW7425-39: Added support for MUX input
* 
* 1   12/13/10 3:37p vsilyaev
* SW7425-40: Video encoder examples
* 
*
*****************************************************************************/
/* Nexus unittest app: encode audio */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_AUDIO_MUX_OUTPUT
#include "nexus_playback.h"
#include "nexus_record.h"
#include "nexus_file.h"
#include "nexus_video_encoder.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#include "nexus_audio_mux_output.h"
#include "nexus_audio_encoder.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#ifndef NEXUS_ENCODER_DISPLAY_IDX
#define NEXUS_ENCODER_DISPLAY_IDX   3
#endif

#define BTST_AC3_TRANSCODER_PASSTHRU    1
#define BTST_MPG_TRANSCODER_PASSTHRU    0
#define BTST_AAC_TRANSCODER_PASSTHRU    0

/* set to 0 to show decode only test */
#define BTST_ENABLE_TRANSCODE            1
#define BTST_ENABLE_PASSTHROUGH          0

/* Encode parameters */
typedef struct EncodeSettings {
	char                    fname[256];
	NEXUS_AudioCodec        eAudioCodec;
} EncodeSettings;

/* Input stream parameters */
typedef struct InputSettings{
	char                 fname[256];
	NEXUS_TransportType  eStreamType;          
	NEXUS_AudioCodec     eAudioCodec;    
	int                  iAudioPid;
	int                  iPcrPid;
}InputSettings;

BDBG_MODULE(encode_audio_es);

int main(void)  {
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_AUDIO_MUX_OUTPUT
	NEXUS_PlatformSettings platformSettings;
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_StcChannelHandle stcChannel, stcChannelTranscode;
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_PidChannelHandle audioPidChannel;
	NEXUS_DisplayHandle display;
	NEXUS_AudioDecoderHandle audioDecoder;
#if BTST_ENABLE_PASSTHROUGH
	NEXUS_AudioDecoderHandle audioPassthrough;
#endif
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_FilePlayHandle file;
	NEXUS_PlaypumpHandle playpump;
	NEXUS_PlaybackHandle playback;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	NEXUS_AudioMuxOutputHandle audioMuxOutput;
	NEXUS_AudioMuxOutputStatus audioMuxStatus;
	NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
#if BTST_ENABLE_TRANSCODE 
	NEXUS_AudioEncoderSettings encoderSettings;
	NEXUS_AudioEncoderHandle audioEncoder;
#endif
	size_t bytes;
	FILE *fout, *fdesc;
	InputSettings stInput;
	EncodeSettings stEncode;



	printf("\n Input stream file: ");                                        scanf("%s", stInput.fname);
	printf("\n Input stream type  (1) Es (2) Ts: ");                     scanf("%d", (int32_t*)&stInput.eStreamType);
	printf("\n Audio input codec:\n"
		" (%d)Mpg\n"
		" (%d)AAC\n"
		" (%d)AAC+Loas\n"
		" (%d)AAC+Adts\n"
		" (%d)AC3\n"
		" (%d)AC3+\n",
		NEXUS_AudioCodec_eMpeg,
		NEXUS_AudioCodec_eAac,
		NEXUS_AudioCodec_eAacPlusLoas,
		NEXUS_AudioCodec_eAacPlusAdts,
		NEXUS_AudioCodec_eAc3,
		NEXUS_AudioCodec_eAc3Plus);
	scanf("%d", (int32_t*)&stInput.eAudioCodec);
	printf("\n Audio pid: ");                                             scanf("%d", &stInput.iAudioPid);
	printf("\n encode stream: ");                                        scanf("%s", stEncode.fname);
	printf("\n Encode Audio Codec: (%d)Mpg (%d)Mp3 (%d)AAC (%d)AAC+ (%d)AAC+Loas (%d)AAC+Adts (%d)AC3: ",
		NEXUS_AudioCodec_eMpeg,
		NEXUS_AudioCodec_eMp3,
		NEXUS_AudioCodec_eAac,
		NEXUS_AudioCodec_eAacPlus,
		NEXUS_AudioCodec_eAacPlusLoas,
		NEXUS_AudioCodec_eAacPlusAdts,
		NEXUS_AudioCodec_eAc3); 
	scanf("%d", (int32_t*)&stEncode.eAudioCodec);


	printf("\n****************************************************************\n");
	printf("\n Input Parameters\n");
	printf("\n Input filename           %s", stInput.fname);
	printf("\n Streamtype %d input Audio Codec %d", stInput.eStreamType, stInput.eAudioCodec);
	printf("\n aPid %4d", stInput.iAudioPid);
	printf("\n\n Encode Parameters\n");
	printf("\n Output Encode filename   %s", stEncode.fname);
	printf("\n Encode Codec  %d", stEncode.eAudioCodec);
	printf("\n****************************************************************\n");


	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openFrontend = false;
	NEXUS_Platform_Init(&platformSettings);
	NEXUS_Platform_GetConfiguration(&platformConfig);

	playpump = NEXUS_Playpump_Open(0, NULL);
	assert(playpump);
	playback = NEXUS_Playback_Create();
	assert(playback);

	file = NEXUS_FilePlay_OpenPosix(stInput.fname, NULL);
	if (!file) {
		fprintf(stderr, "can't open file:%s\n", stInput.fname);
		return -1;
	}

	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

	/* encoders/mux require different STC broadcast mode from decoder */
	NEXUS_StcChannel_GetDefaultSettings(1, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0;/* should be the same timebase for end-to-end locking */
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
	stcChannelTranscode = NEXUS_StcChannel_Open(1, &stcSettings);


	NEXUS_Playback_GetSettings(playback, &playbackSettings);
	playbackSettings.playpump = playpump;
	/* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
	playbackSettings.playpumpSettings.transportType = stInput.eStreamType;
	playbackSettings.stcChannel = stcChannel;
	NEXUS_Playback_SetSettings(playback, &playbackSettings);


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

	/* Open the audio decoder */
	audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
#if BTST_ENABLE_PASSTHROUGH
	audioPassthrough = NEXUS_AudioDecoder_Open(1, NULL);
#endif
	/* Open the audio and pcr pid channel */
	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
	playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder; /* must be told codec for correct handling */
#if BTST_ENABLE_PASSTHROUGH
	playbackPidSettings.pidTypeSettings.audio.secondary = audioPassthrough;
#endif

	audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, stInput.iAudioPid, &playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;

	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	the audio outputs. */
	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);

	audioProgram.codec = stInput.eAudioCodec;
	audioProgram.pidChannel = audioPidChannel;
	audioProgram.stcChannel = stcChannel;

	/* Connect audio decoders to outputs */
	NEXUS_AudioOutput_AddInput(
		NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
		NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if BTST_ENABLE_PASSTHROUGH
	NEXUS_AudioOutput_AddInput(
		NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
		NEXUS_AudioDecoder_GetConnector(audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
#endif
	/* Open audio mux output */
	audioMuxOutput = NEXUS_AudioMuxOutput_Create(NULL);
	assert(audioMuxOutput);
#if BTST_ENABLE_TRANSCODE 
	/* Open audio encoder */
	NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
	encoderSettings.codec = stEncode.eAudioCodec;
	audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
	assert(audioEncoder);
	/* Connect encoder to decoder */
	NEXUS_AudioEncoder_AddInput(audioEncoder,
		NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
	/* Connect mux to encoder */
	NEXUS_AudioOutput_AddInput(
		NEXUS_AudioMuxOutput_GetConnector(audioMuxOutput), NEXUS_AudioEncoder_GetConnector(audioEncoder));
#else    
	/* Connect mux to encoder */
	NEXUS_AudioOutput_AddInput(
		NEXUS_AudioMuxOutput_GetConnector(audioMuxOutput),
		NEXUS_AudioDecoder_GetConnector(audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
#endif

	/* Start audio mux output */
	NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
	audioMuxStartSettings.stcChannel = stcChannelTranscode;
	NEXUS_AudioMuxOutput_Start(audioMuxOutput, &audioMuxStartSettings);
	/* Start decoders */
	NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
#if BTST_ENABLE_PASSTHROUGH
	NEXUS_AudioDecoder_Start(audioPassthrough, &audioProgram);
#endif
	NEXUS_AudioMuxOutput_GetStatus(audioMuxOutput, &audioMuxStatus);
	fprintf(stderr, "audioMuxStatus.pBufferBase: 0x%p\n", audioMuxStatus.pBufferBase);
	/* Start playback */
	NEXUS_Playback_Start(playback, file, NULL);

	fout = fopen(stEncode.fname,"wb");
	fdesc = fopen("audio_desc.txt","w");
	fprintf(fdesc, "flags(h) origPts(h)         pts(Lh)     escr(h) ticksPerBit(u) shr(d) offset(h) length(h) protocol(u)\n");
	for(;;) {
		size_t size[2];
		const NEXUS_AudioMuxOutputFrame *desc[2];
		unsigned i,j;
		unsigned descs;


		NEXUS_AudioMuxOutput_GetBuffer(audioMuxOutput, &desc[0], &size[0], &desc[1], &size[1]);
		if(size[0]==0 && size[1]==0) {
			NEXUS_AudioDecoderStatus astatus;

			NEXUS_AudioDecoder_GetStatus(audioDecoder, &astatus);
			fflush(fout);
			fprintf(stderr, "encoded %u bytes.... decode:%u\t\r", bytes, astatus.pts);
			BKNI_Sleep(30);
			continue;
		}
		for(descs=0,j=0;j<2;j++) {
			descs+=size[j];
			for(i=0;i<size[j];i++) {
				if(desc[j][i].length > 0)
				{
					fwrite((const uint8_t *)audioMuxStatus.pBufferBase + desc[j][i].offset, desc[j][i].length, 1, fout);
					fprintf(fdesc, "%8x %8x   %08x%08x %8x %5u %5d %8x %8x\n", desc[j][i].flags, desc[j][i].originalPts, 
						(uint32_t)(desc[j][i].pts>>32), (uint32_t)(desc[j][i].pts & 0xffffffff), desc[j][i].escr, 
						desc[j][i].ticksPerBit, desc[j][i].shr, desc[j][i].offset, desc[j][i].length);
				}
				bytes+= desc[j][i].length;
			}
		}
		NEXUS_AudioMuxOutput_ReadComplete(audioMuxOutput, descs);
		fflush(fout);
		fflush(fdesc);
	}

	/* Playback state machine is driven from inside Nexus. */
	printf("Press ENTER to quit\n");
	getchar();

	/* Bring down system */
	NEXUS_Playback_Stop(playback);
	NEXUS_FilePlay_Close(file);
#if BTST_ENABLE_PASSTHROUGH
	NEXUS_AudioDecoder_Stop(audioPassthrough);
#endif
	NEXUS_AudioDecoder_Stop(audioDecoder);
	NEXUS_Playback_Destroy(playback);
	NEXUS_Playpump_Close(playpump);
	NEXUS_AudioMuxOutput_Destroy(audioMuxOutput);
	NEXUS_Display_Close(display);
	NEXUS_StcChannel_Close(stcChannel);

	NEXUS_AudioDecoder_Close(audioDecoder);
#if BTST_ENABLE_PASSTHROUGH
	NEXUS_AudioDecoder_Close(audioPassthrough);
#endif    
	NEXUS_Platform_Uninit();

#endif
	return 0;
}


