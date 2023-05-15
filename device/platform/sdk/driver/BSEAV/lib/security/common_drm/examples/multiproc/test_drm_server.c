/*
 * test_widevine_xpt_srever.c
 *
  *    (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: test_drm_server.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/16/12 4:41p $
 *
 * Module Description:
 *	A simple server to test (nexus-) multiprocess DRM features...
 *
 * Revision History:
 * 	04/23/2012 - created
 *
 *
 */
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_platform_server.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_decoder.h"
#include "nexus_video_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_mixer.h"
#include "nexus_hdmi_output.h"
#include "nexus_ac3_encode.h"
#include "nexus_simple_video_decoder_server.h"
#include "nexus_simple_audio_decoder_server.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void print_usage(void)
{
    printf(
    "usage: nexus test_drm_srvr [--help|-h]\n"
    "options:\n"
    "  --help|-h      print this help screen\n\n"
    "once successfully started, in other terminals/processes run any test_*_client, for ex.:"
    "  nexus test_widevine_xpt_client [-d <DRM_file_name>] <output_file_name>  , or\n"
    "  nexus test_widevine_2_client <DRM_file_name> , and s.o.\n"
    "  (for help run each with --help|-h arg, e.g. 'nexus test_xxx --help' )\n\n"
    "watch and report any suspicious activity, or otherwise enjoy..."
    );
}

#ifdef NUM_SIMPLE_VIDEO_DECODERS
#error
#error "Where does NUM_SIMPLE_VIDEO_DECODERS come from? Chek for possible mismatch?"
#endif
#define NUM_SIMPLE_VIDEO_DECODERS 1

#ifdef NUM_AUDIO_DECODERS
#error "Where does NUM_AUDIO_DECODERS come from? Chek for possible mismatch?"
#endif
#define NUM_AUDIO_DECODERS 2

#ifdef NUM_SIMPLE_AUDIO_DECODERS
#error "Where does NUM_SIMPLE_AUDIO_DECODERS come from? Check for possible mismatch?"
#endif
#define NUM_SIMPLE_AUDIO_DECODERS 2

#ifdef NUM_AUDIO_PLAYBACK
#error "Where does NUM_AUDIO_PLAYBACK come from? Check for possible mismatch?"
#endif
#define NUM_AUDIO_PLAYBACK 2

#ifdef NUM_SIMPLE_AUDIO_PLAYBACK
#error "Where does NUM_SIMPLE_AUDIO_PLAYBACK come from? Check for possible mismatch?"
#endif
#define NUM_SIMPLE_AUDIO_PLAYBACK 2

struct NEXUS_DATA {
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderHandle simpleVideoDecoders[NUM_SIMPLE_VIDEO_DECODERS];
    NEXUS_AudioDecoderHandle audioDecoders[NUM_AUDIO_DECODERS];
    NEXUS_AudioMixerHandle audioMixer;
    NEXUS_Ac3EncodeHandle ac3Encoder;
    NEXUS_AudioPlaybackHandle audioPlaybacks[NUM_AUDIO_PLAYBACK];
    NEXUS_SimpleAudioDecoderHandle simpleAudioDecoders[NUM_SIMPLE_AUDIO_DECODERS];
    NEXUS_SimpleAudioPlaybackHandle simpleAudioPlaybacks[NUM_SIMPLE_AUDIO_PLAYBACK];
};

/* Sad enough the platConfig.outputs is anonymous struct :-(
 * i.e. I could have avoided a few additions here
 */
static NEXUS_Error
AddDisplayOutputs(NEXUS_DisplayHandle, NEXUS_PlatformConfiguration* );

static NEXUS_Error
CreateSimpleVideoDecoder(NEXUS_VideoDecoderHandle, NEXUS_VideoWindowHandle,
		         	     NEXUS_SimpleVideoDecoderHandle* );

static NEXUS_Error
CreateAudioDecodersEtc(NEXUS_AudioDecoderHandle*, NEXUS_AudioMixerHandle*,
					   NEXUS_Ac3EncodeHandle*, NEXUS_AudioPlaybackHandle*);

static NEXUS_Error
CreateSimpleAudioDecoders(NEXUS_AudioDecoderHandle* audioDecoder,
						  NEXUS_AudioMixerHandle audioMixer,
					 	  NEXUS_AudioPlaybackHandle* audioPlayback,
					 	  NEXUS_PlatformConfiguration* platConfig,
					 	  NEXUS_SimpleAudioDecoderHandle* simpleAudioDecoder);

static NEXUS_Error
CreateSimpleAudioPlaybacks(NEXUS_SimpleAudioDecoderHandle* simpleAudioDecoder,
						   NEXUS_AudioPlaybackHandle* audioPlayback,
						   NEXUS_SimpleAudioPlaybackHandle* simpleAudioPlayback);

void NexusDataInitialize(struct NEXUS_DATA* nexusData);
void NexusDataFinalize(struct NEXUS_DATA* nexusData);

BDBG_MODULE(test_drm_server);

int main(int argc, char **argv)
{
	struct NEXUS_DATA nexus;

    int exit_code = 0;
    NEXUS_Error rc;

    if (argc > 1 &&  (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        print_usage();
        return 0;
    }

    rc = NEXUS_Platform_Init(NULL);
    if (NEXUS_SUCCESS == rc) /* if successfully init - let also unint */
    {
		NexusDataInitialize(&nexus); /* in other words - zero it out, so finalize can check and close non-zeroed elem. */
		while (NEXUS_SUCCESS == rc) {	/* well, not really a loop, just a way to avoid a number of indent-s and goto-s */
						/*	yeah, assuming also any of the called functions dumps some diagnostics...	*/
			unsigned displayIndex = 0; /* with some vague intention to use many displays... whenever/if possible?! */
			unsigned decoderIndex = 0; /* idem */

			/* TODDDO Not sure if this can fail?? */
			NEXUS_Platform_GetConfiguration(&nexus.platformConfig);

			/* open display and connect video decoder to a window */
			nexus.display = NEXUS_Display_Open(displayIndex, NULL);
			BDBG_MSG(("nexus.display 0x%x", nexus.display));
			if (!nexus.display) break;

			rc = AddDisplayOutputs(nexus.display, &nexus.platformConfig);
			if (NEXUS_SUCCESS != rc) break;

			nexus.window = NEXUS_VideoWindow_Open(nexus.display, 0);
			BDBG_MSG(("nexus.window 0x%x", nexus.window));
			if (!nexus.window) break;

			nexus.videoDecoder = NEXUS_VideoDecoder_Open(decoderIndex, NULL);
			BDBG_MSG(("nexus.videoDecoder 0x%x", nexus.videoDecoder));
			if (!nexus.videoDecoder) break;

			rc = CreateSimpleVideoDecoder(nexus.videoDecoder, nexus.window, nexus.simpleVideoDecoders);
			if (NEXUS_SUCCESS != rc) break;

			rc = CreateAudioDecodersEtc(nexus.audioDecoders, &nexus.audioMixer,
										&nexus.ac3Encoder, nexus.audioPlaybacks);
			if (NEXUS_SUCCESS != rc) break;

#define ENABLE_AC3_TRANSCODING (0)  /* Change this to 1 to enable AC3 Transcoding.
										Disconnected nodes are not supported on all platforms. */
#if ENABLE_AC3_TRANSCODING
			NEXUS_Ac3Encode_AddInput(nexus.ac3Encoder,
									 NEXUS_AudioDecoder_GetConnector(audioDecoder[0],
										NEXUS_AudioDecoderConnectorType_eMultichannel));
#endif

			rc = CreateSimpleAudioDecoders(nexus.audioDecoders, nexus.audioMixer,
										   nexus.audioPlaybacks, &nexus.platformConfig,
										   nexus.simpleAudioDecoders);
			if (NEXUS_SUCCESS != rc) break;

			rc = CreateSimpleAudioPlaybacks(nexus.simpleAudioDecoders, nexus.audioPlaybacks,
											nexus.simpleAudioPlaybacks);
			if (NEXUS_SUCCESS != rc) break;

			{
			    NEXUS_PlatformStartServerSettings serverSettings;
				NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
				/* client is written this way */
				/* Nope! */
				serverSettings.allowUnauthenticatedClients = true;
				serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eProtected;
				/* As seen elsewhere:	'for purposes of example, allow access to main heap'
				 * On the other hand most clients seems to need it, they
				 * 						'must use heap with driver-side mapping' (?!?!) */
			    serverSettings.unauthenticatedConfiguration.heap[0] = nexus.platformConfig.heap[0];

				/* for purposes of example, allow access to main heap */
				/** ... without a particular reason - I'll skip it...
				serverSettings.unauthenticatedConfiguration.heap[0] = nexus.platformConfig.heap[0];
				... ***/
				BDBG_MSG(("NEXUS_Platform_StartServer"));
				rc = NEXUS_Platform_StartServer(&serverSettings);
				if (NEXUS_SUCCESS != rc) break;

			    printf("Press ENTER to shutdown decode_server\n");
			    getchar();

			    /* stop the server before closing resources that may be in use by clients.
			    if it's an untrusted client, handle verification may fail the call. but a trusted client bypasses the
			    check and could kill the server. */
				BDBG_MSG(("NEXUS_Platform_StopServer()"));
			    NEXUS_Platform_StopServer();
			}
			break;
		}
		BDBG_MSG(("NexusDataFinalize(&nexus)"));
		NexusDataFinalize(&nexus);
		BDBG_MSG(("NEXUS_Platform_Uninit()"));
		NEXUS_Platform_Uninit();
	}
    else {
    	/* printf("bummer!"); */
    	printf("NEXUS_Platform_Init failed, error %d", rc);
    	exit_code = rc;
	}
    return exit_code;
}

static NEXUS_Error
AddDisplayOutputs(NEXUS_DisplayHandle display, NEXUS_PlatformConfiguration* platConfig)
{
	NEXUS_Error rc = 0;

#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display,
    						 	 NEXUS_ComponentOutput_GetConnector(
    						 			 platConfig->outputs.component[0]));
    BDBG_ASSERT(NEXUS_SUCCESS == rc);
#endif

#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display,
    							 NEXUS_CompositeOutput_GetConnector(
    									 platConfig->outputs.composite[0]));
    BDBG_ASSERT(NEXUS_SUCCESS == rc);
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
    {
        NEXUS_HdmiOutputStatus hdmiStatus;
		rc = NEXUS_Display_AddOutput(display,
									 NEXUS_HdmiOutput_GetVideoConnector(
											 platConfig->outputs.hdmi[0]));

		BDBG_ASSERT(NEXUS_SUCCESS == rc);
		rc = NEXUS_HdmiOutput_GetStatus(platConfig->outputs.hdmi[0], &hdmiStatus);
		if ( NEXUS_SUCCESS == rc && hdmiStatus.connected )
		{
	        NEXUS_DisplaySettings displaySettings;
			/* If current display format is not supported by monitor, switch to monitor's preferred format.
			   If other connected outputs do not support the preferred format, a harmless error will occur. */
			NEXUS_Display_GetSettings(display, &displaySettings);
			if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
				displaySettings.format = hdmiStatus.preferredVideoFormat;
				NEXUS_Display_SetSettings(display, &displaySettings);
			}
		}
    }
#endif
	BDBG_MSG(("AddDisplayOutputs returns %d", rc));

    return rc;
}


static NEXUS_Error
CreateSimpleVideoDecoder(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoWindowHandle window,
		         	     NEXUS_SimpleVideoDecoderHandle* simpleVideoDecoder)
{
	NEXUS_SimpleVideoDecoderServerSettings settings;
	NEXUS_SimpleVideoDecoder_GetDefaultServerSettings(&settings);
	settings.videoDecoder = videoDecoder;
	settings.window[0] = window; /* SimpleVideoDecoder will do the connection */

	*simpleVideoDecoder = NEXUS_SimpleVideoDecoder_Create(0, &settings);
	BDBG_MSG(("nexus.simpleVideoDecoder 0x%x", *simpleVideoDecoder));
	BDBG_ASSERT(*simpleVideoDecoder);
	return *simpleVideoDecoder ? 0 : /* FIXME - w a comprehensible code */ 33;
}

static NEXUS_Error
CreateAudioDecodersEtc(NEXUS_AudioDecoderHandle* audioDecoder,
					  NEXUS_AudioMixerHandle* audioMixer,
					  NEXUS_Ac3EncodeHandle* ac3Encoder,
					  NEXUS_AudioPlaybackHandle* audioPlayback)
{
	NEXUS_Error rc = 0;
	/* create audio decoders */
	int i;
	for (i = 0 ; i < NUM_AUDIO_DECODERS && !rc; ++i ) {
		audioDecoder[i] = NEXUS_AudioDecoder_Open(i, NULL);
		BDBG_ASSERT(*audioDecoder);
		rc = audioDecoder[i] ? rc : 55; /*FIXME w error, which makes sense */
	}

	*audioMixer = NEXUS_AudioMixer_Open(NULL);
	BDBG_ASSERT(*audioMixer);
	rc = *audioMixer ? rc : 55; /* idem */

	*ac3Encoder = NEXUS_Ac3Encode_Open(NULL);
	BDBG_ASSERT(*ac3Encoder);
	rc = *ac3Encoder ? rc : 55; /* idem */

	for (i = 0 ; i < NUM_AUDIO_PLAYBACK && !rc ; ++i ) {
		audioPlayback[i] = NEXUS_AudioPlayback_Open(NEXUS_ANY_ID, NULL);
		BDBG_ASSERT(*audioPlayback);
		rc = audioPlayback[i] ? rc : 55; /*FIXME w error, which makes sense */
	}
	return rc;
}

/* create simple audio decoder */
static NEXUS_Error
CreateSimpleAudioDecoders(NEXUS_AudioDecoderHandle* audioDecoder,
						  NEXUS_AudioMixerHandle audioMixer,
					 	  NEXUS_AudioPlaybackHandle* audioPlayback,
					 	  NEXUS_PlatformConfiguration* platConfig,
					 	  NEXUS_SimpleAudioDecoderHandle* simpleAudioDecoder)
{
	NEXUS_Error rc = 0;
    NEXUS_SimpleAudioDecoderServerSettings settings;
    NEXUS_SimpleAudioDecoder_GetDefaultServerSettings(&settings);
    settings.primary = audioDecoder[0];
    settings.secondary = audioDecoder[1];

    /* any mixed output must be connected outside of the simple decoder
     * and are not configurable per codec. they are used for primary decoder
     *  PCM output as well as PCM playback. */
    rc = NEXUS_AudioMixer_AddInput(audioMixer,
    							  NEXUS_AudioDecoder_GetConnector(audioDecoder[0],
    									 NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(NEXUS_SUCCESS == rc);

    rc = rc || NEXUS_AudioMixer_AddInput(audioMixer, NEXUS_AudioPlayback_GetConnector(audioPlayback[0]));
    BDBG_MSG(("NEXUS_AudioMixer_AddInput returned %d", rc));
    BDBG_ASSERT(NEXUS_SUCCESS == rc);

    rc = rc || NEXUS_AudioMixer_AddInput(audioMixer, NEXUS_AudioPlayback_GetConnector(audioPlayback[1]));
    BDBG_ASSERT(NEXUS_SUCCESS == rc);

    rc = rc || NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platConfig->outputs.audioDacs[0]),
    									  NEXUS_AudioMixer_GetConnector(audioMixer));
    BDBG_ASSERT(NEXUS_SUCCESS == rc);

#if NEXUS_NUM_HDMI_OUTPUTS
#define HDMI_HAS_MIXED_PCM 1
#if HDMI_HAS_MIXED_PCM
    rc = rc || NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(platConfig->outputs.hdmi[0]),
    									  NEXUS_AudioMixer_GetConnector(audioMixer));
    BDBG_ASSERT(NEXUS_SUCCESS == rc);
#else
    settings.hdmi.outputs[0] = platConfig->outputs.hdmi[0];
    /* leave settings.hdmi.input[] = NULL, which will output unmixed PCM */
#endif
#endif

    /* for this example, SPDIF will be an unmixed output, which is configurable per codec */
    settings.spdif.outputs[0] = platConfig->outputs.spdif[0];
    /* if input[] is unspecified, it will be PCM from primary decoder */
    settings.spdif.input[NEXUS_AudioCodec_eAc3] = NEXUS_AudioDecoder_GetConnector(audioDecoder[1],
    														NEXUS_AudioDecoderConnectorType_eCompressed);
    settings.spdif.input[NEXUS_AudioCodec_eAc3Plus] = NEXUS_AudioDecoder_GetConnector(audioDecoder[0],
    														NEXUS_AudioDecoderConnectorType_eCompressed);
#if ENABLE_AC3_TRANSCODING
    settings.spdif.input[NEXUS_AudioCodec_eAacAdts] =
    settings.spdif.input[NEXUS_AudioCodec_eAacLoas] =
    settings.spdif.input[NEXUS_AudioCodec_eAacPlusAdts] =
    settings.spdif.input[NEXUS_AudioCodec_eAacPlusLoas] = NEXUS_Ac3Encode_GetConnector(ac3Encoder);
#endif
    *simpleAudioDecoder = NEXUS_SimpleAudioDecoder_Create(0, &settings);
	BDBG_ASSERT(*simpleAudioDecoder);

	rc = *simpleAudioDecoder ? rc : /* FIXME - w a comprehensible code */ 99;
	return rc;
}

/* create simple audio playback. it is linked to the audio decoder for timebase.
but it is acquired separately by the app.
if more than one SimpleAudioDecoder is created, the user should create an index scheme to separate them. */
static NEXUS_Error
CreateSimpleAudioPlaybacks(NEXUS_SimpleAudioDecoderHandle* simpleAudioDecoder,
						   NEXUS_AudioPlaybackHandle* audioPlayback,
						   NEXUS_SimpleAudioPlaybackHandle* simpleAudioPlayback)
{
	NEXUS_Error rc = 0;
    NEXUS_SimpleAudioPlaybackServerSettings settings;

    int i;
    for (i = 0 ; i < NUM_SIMPLE_AUDIO_PLAYBACK && !rc ; ++i) {
    	NEXUS_SimpleAudioPlayback_GetDefaultServerSettings(&settings);
    	settings.decoder = *simpleAudioDecoder;
    	settings.playback = audioPlayback[i];
    	simpleAudioPlayback[i] = NEXUS_SimpleAudioPlayback_Create(i, &settings);
    	BDBG_ASSERT(simpleAudioPlayback[i]);
    	rc = simpleAudioPlayback[i] ? rc : 133; /* FIXME with a reasonable error */
    }

    return rc;
}

void NexusDataInitialize(struct NEXUS_DATA* nexusData)
{
	BDBG_MSG(("In NexusDataInitialize"));
	BKNI_Memset(nexusData, 0, sizeof(struct NEXUS_DATA));
}

/*
struct NEXUS_DATA {
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderHandle simpleVideoDecoders[NUM_SIMPLE_VIDEO_DECODERS];
    NEXUS_AudioDecoderHandle audioDecoders[NUM_AUDIO_DECODERS];
    NEXUS_AudioMixerHandle audioMixer;
    NEXUS_Ac3EncodeHandle ac3Encoder;
    NEXUS_AudioPlaybackHandle audioPlaybacks[NUM_AUDIO_PLAYBACK];
    NEXUS_SimpleAudioDecoderHandle simpleAudioDecoders[NUM_SIMPLE_AUDIO_DECODERS];
    NEXUS_SimpleAudioPlaybackHandle simpleAudioPlaybacks[NUM_SIMPLE_AUDIO_PLAYBACK];
};
 */

void NexusDataFinalize(struct NEXUS_DATA* nexusData)
{
	char* sanity = (char*) nexusData;
	int i;
	for (i = NUM_SIMPLE_AUDIO_PLAYBACK - 1 ; i >= 0 ; --i ) {
		if (nexusData->simpleAudioPlaybacks[i]) {
			NEXUS_SimpleAudioPlayback_Destroy(nexusData->simpleAudioPlaybacks[i]);
			BDBG_MSG(("NEXUS_SimpleAudioPlayback_Destroy(nexusData->simpleAudioPlaybacks[%d])", i));
		}
		/* if DEBUG - for sanity check */
		nexusData->simpleAudioPlaybacks[i] = 0;
	}

	for (i = NUM_SIMPLE_AUDIO_DECODERS - 1 ; i >= 0 ; --i ) {
		if (nexusData->simpleAudioDecoders[i]) {
			NEXUS_SimpleAudioDecoder_Destroy(nexusData->simpleAudioDecoders[i]);
			BDBG_MSG(("NEXUS_SimpleAudioDecoder_Destroy(nexusData->simpleAudioDecoders[%d])", i));
		}
		/* if DEBUG - for sanity check */
		nexusData->simpleAudioDecoders[i] = 0;
	}


	if (nexusData->ac3Encoder) {
		NEXUS_Ac3Encode_RemoveAllInputs(nexusData->ac3Encoder);
		BDBG_MSG(("NEXUS_Ac3Encode_RemoveAllInputs(nexusData->ac3Encoder (== 0x%x)))", nexusData->ac3Encoder));
	    NEXUS_AudioInput_Shutdown(NEXUS_Ac3Encode_GetConnector(nexusData->ac3Encoder));
		BDBG_MSG(("NEXUS_AudioInput_Shutdown(NEXUS_Ac3Encode_GetConnector(nexusData->ac3Encoder == 0x%x)))", nexusData->ac3Encoder));
	    NEXUS_Ac3Encode_Close(nexusData->ac3Encoder);
		BDBG_MSG(("NEXUS_Ac3Encode_Close(nexusData->ac3Encoder == 0x%x))", nexusData->ac3Encoder));
		nexusData->ac3Encoder = 0;
	}

    /* A bit clueless how correct this sequence is... I shall check ASAP */
	if (nexusData->audioMixer) {
		NEXUS_AudioMixer_RemoveAllInputs(nexusData->audioMixer);
		if (nexusData->audioDecoders[0]) {
			NEXUS_AudioInput_Shutdown(
					NEXUS_AudioDecoder_GetConnector(nexusData->audioDecoders[0],
												NEXUS_AudioDecoderConnectorType_eMultichannel));
			BDBG_MSG(("NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(nexusData->audioDecoders[0], ...)" ));
		}
	}

	for ( i = NUM_AUDIO_PLAYBACK - 1 ; i >= 0 ; --i ) {
		if (nexusData->audioPlaybacks[i]) {
			NEXUS_AudioPlayback_Close(nexusData->audioPlaybacks[i]);
			BDBG_MSG(("NEXUS_AudioPlayback_Close(nexusData->audioPlaybacks[%d] (== 0x%x))",
					  i, nexusData->audioPlaybacks[i]));
		}
		/* if DEBUG - for sanity check */
		nexusData->audioPlaybacks[i] = 0;
	}

    if(nexusData->audioMixer) {
		NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(nexusData->audioMixer));
		BDBG_MSG(("NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(nexusData->audioMixer (== 0x%x)))",
				  nexusData->audioMixer));
		NEXUS_AudioMixer_Close(nexusData->audioMixer);
		BDBG_MSG(("NEXUS_AudioMixer_Close(nexusData->audioMixer (== 0x%x))",
				  nexusData->audioMixer));
		nexusData->audioMixer = 0;
    }
    /* A bit clueless how correct this sequence is... I shall check ASAP  - END */


	for ( i = NUM_AUDIO_DECODERS - 1 ; i >= 0 ; --i ) {
		if (nexusData->audioDecoders[i]) {
			NEXUS_AudioDecoder_Close(nexusData->audioDecoders[i]);
			BDBG_MSG(("NEXUS_AudioDecoder_Close(nexusData->audioDecoders[%d] (==0x%x))",
					  i, nexusData->audioDecoders[i]));
		}
		/* if DEBUG - for sanity check */
		nexusData->audioDecoders[i] = 0;
	}

	for ( i = NUM_SIMPLE_VIDEO_DECODERS - 1 ; i >= 0 ; --i ) {
		if (nexusData->simpleVideoDecoders[i]) {
			NEXUS_SimpleVideoDecoder_Destroy(nexusData->simpleVideoDecoders[i]);
			BDBG_MSG(("NEXUS_SimpleVideoDecoder_Destroy(nexusData->simpleVideoDecoders[%d] (==0x%x))",
					  i, nexusData->simpleVideoDecoders[i]));
		}
		/* if DEBUG - for sanity check */
		nexusData->simpleVideoDecoders[i] = 0;
	}

	if (nexusData->videoDecoder) {
		NEXUS_VideoDecoder_Close(nexusData->videoDecoder);
		BDBG_MSG(("NEXUS_VideoDecoder_Close(nexusData->videoDecoder (==0x%x))",
				  nexusData->videoDecoder));
		nexusData->videoDecoder = 0;
	}

	if (nexusData->window) {
		NEXUS_VideoWindow_Close(nexusData->window);
		BDBG_MSG(("NEXUS_VideoWindow_Close(nexusData->window (==0x%x))",
				nexusData->window));
		nexusData->window = 0;
	}

	if (nexusData->display) {
		NEXUS_Display_Close(nexusData->display);
		BDBG_MSG(("NEXUS_Display_Close(nexusData->display (==0x%x))",
				nexusData->display));
		nexusData->display = 0;
	}

	/* Now either ... */
	BKNI_Memset(&nexusData->platformConfig, 0, sizeof(nexusData->platformConfig));

	/* ... and ASSERT the whole nexusData structure for == 0
	 * 		or assert the handle-s only (IMO the former is simpler/safer!)
	 * 		Then bail out (rather assured no resources are held!)
	 */
	/* BKNI_Memtest ??? */
	for (i = 0 ; (unsigned)i < sizeof(*nexusData) ; ++i, ++sanity)
		BDBG_ASSERT(0 == *sanity);
}


