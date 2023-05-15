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
 * $brcm_Workfile: 1394_r.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/21/09 3:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/1394/1394_r.c $
 * 
 * Hydra_Software_Devel/2   10/21/09 3:25p rjlewis
 * SW7420-380: cleaned up for Take 2 release.
 * 
 * Hydra_Software_Devel/1   10/9/09 5:47p rjlewis
 * SW7420-380: Initial release.
 * 
 * 16   1/15/09 12:58p erickson
 * PR35457: simplify example for nexus overview presentations
 *
******************************************************************************/
/* Nexus example app: single live a/v decode from an input band */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "nexus_types.h"

/* Nexus public API includes */
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

#include "priv/nexus_core.h"
#include "btnf.h"

/* Compile time options -- normally leave undefined */
/*#define EXTRA_OPTIONS*/ /* these are special functions to enable checking of specific functionality */
/*#define USE_SECOND_PROGRAM*/ /* use second program in stream (different PIDS) */

static void IssueBusReset(BTNF_Handle handle, bool becomeRoot)
{
    BERR_Code err;
	if (becomeRoot) {
    	err = BTNF_ForceRoot(handle);
    	err = BTNF_EnableContender(handle);
	}
    err = BTNF_IssueBusReset(handle);
}

static uint16_t GetNodeId(BTNF_Handle handle)
{
    BERR_Code err;
	uint16_t node;
	err = BTNF_GetNodeID(handle, &node);
	return node;
}

#if 0
static void printInputBandSettings(NEXUS_InputBandSettings *settings, const char *header)
{
	printf("Inputband settings:\n");
	printf("%s: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", header, 
		settings->clockActiveHigh,
		settings->dataLsbFirst,
		settings->dataActiveHigh,
		settings->validEnabled,
		settings->validActiveHigh,
		settings->useInternalSync,
		settings->useSyncAsValid,
		settings->syncActiveHigh,
		settings->errorEnabled,
		settings->errorActiveHigh,
		settings->parallelInput,
		settings->packetLength
	);
}
#endif

static void print_usage(char *program)
{
	fprintf(stderr, "%s: usage:  %s [option param]\n" , program, program);
	fprintf(stderr, 
		"    --channel (-c) - set isoch channel to use (default=63)\n"
		"    --sync (-S)    - set sync value (enables decryption); use -1 to enable swapping keys\n"
		"    --keylo (-l)   - low order 32 bits of key (when sync > 0)\n"
		"    --keyhi (-h)   - high order 24 bits of key (when sync > 0)\n"
		);
#ifdef EXTRA_OPTIONS
	fprintf(stderr, 
		"    --test (-t)    - enable test mode for checking changes in sync field\n"
		"    --jitter (-j)  - enable jitter compensation option\n"
		);
#endif
	exit(-1);
}

/* Note: I had weird problems when I updated this test code to include some extra options.  I found that the PID parser
** was only passing some MPEG packets -- I was losing packets.  I don't know why.  I #ifdef out all the new code and 
** it worked again.  I added JUST the variable declarations (getting unused warnings) and it happened again.
** I tried renaming the variables (could there be a conflict between the local and a global???) but it didn't help.
** Finally, I moved all the nexus structures off the stack and made them global.  This means they are all being initialized
** to zero and not relying on the get/set routines to initialize them.  This worked.  I think someone somewhere isn't properly
** initializing something and this caused my problems.  I don't have time to research this -- I'm going with what works.
*/

/* Compile time options -- normally leave undefined */
/* I had these as stack variables but it was causing the software to crash */
static    NEXUS_PlatformSettings platformSettings;
static    NEXUS_PlatformConfiguration platformConfig;
static    NEXUS_InputBand inputBand;
static    NEXUS_ParserBand parserBand;
static    NEXUS_ParserBandSettings parserBandSettings; /* can we use this for both the parser bands? */
static    NEXUS_StcChannelHandle stcChannel;
static    NEXUS_StcChannelSettings stcSettings;
static    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
static    NEXUS_DisplayHandle display;
static    NEXUS_DisplaySettings displaySettings;
static    NEXUS_VideoWindowHandle window;
static    NEXUS_VideoDecoderHandle vdecode;
static    NEXUS_VideoDecoderStartSettings videoProgram;
static    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
static    NEXUS_AudioDecoderStartSettings audioProgram;

int main(int argc, char *argv[])
{
    int i;
    BERR_Code err;
    BTNF_Handle handle;
    BTNF_DefaultSettings settings;

	/* These are the default parameters I'll use for the receive operation. */
	int channel = 63;
	int sync = 0; /* this tells us when to use encryption */
	uint32_t keylo = 0x12345678, keyhi = 0x00654321;
	bool use_all_keys = false;
#ifdef EXTRA_OPTIONS
	bool test_mode = false;
	bool jitter = true;
#endif

	char *program = *argv++; argc--;

	/* let them over-ride the defaults */
	if (argc && (!strcmp(*argv, "-?") || !strcmp(*argv, "-h") || !strcmp(*argv,"--help"))) print_usage(program);
	while (argc>0) 
	{
		printf("Processing parameter '%s' with argc = %d\n", *argv, argc);
			 if (!strcmp(*argv, "--channel") || !strcmp(*argv, "-c")) { if(--argc) channel = atoi(*++argv); }
		else if (!strcmp(*argv, "--sync")    || !strcmp(*argv, "-S")) { if(--argc) sync  = atoi(*++argv); }
		else if (!strcmp(*argv, "--keylo")   || !strcmp(*argv, "-l")) { if(--argc) keylo = atoi(*++argv); }
		else if (!strcmp(*argv, "--keyhi")   || !strcmp(*argv, "-h")) { if(--argc) keyhi = atoi(*++argv); }
#ifdef EXTRA_OPTIONS
		else if (!strcmp(*argv, "--test")    || !strcmp(*argv, "-t")) { test_mode = true; }
		else if (!strcmp(*argv, "--jitter")  || !strcmp(*argv, "-j")) { jitter = false; }
#endif
		else { fprintf(stderr, "%s: unknown parameter (%s)\n", program, *argv); print_usage(program); }
		argv++; argc--;
	}

	/* Use a negative sync value to enable using all the different keys */
	if (sync < 0) {
		printf("Enabling 'use_all_keys' mode!\n");
		use_all_keys = true;
		sync = -sync;
	}

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    inputBand = NEXUS_InputBand_e6; /* this should be the 1394 receive */

  {
	NEXUS_InputBandSettings settings;

	NEXUS_InputBand_GetSettings(inputBand, &settings); /* get current */
	/*printInputBandSettings(&settings, "was:");*/

	/* These are what I need them to be (mostly default values) */
    settings.clockActiveHigh = true;
    settings.dataLsbFirst = false;
    settings.dataActiveHigh = true;
    settings.validEnabled = true;
    settings.validActiveHigh = true;
    settings.useInternalSync = true;
    settings.syncActiveHigh = false;
    settings.errorEnabled = false;
    settings.errorActiveHigh = true;
    settings.parallelInput = false;
    settings.packetLength = 188;
	/*printInputBandSettings(&settings, " is:");*/

	NEXUS_InputBand_SetSettings(inputBand, &settings);
  }

/* 
**********************************************************
** This sets up the decoders to get the streamer output **
**********************************************************
*/
    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);

#if 0
	/* These are the default (current?) settings */
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    parserBandSettings.timestampType = NEXUS_TransportTimestampType_eNone;
    parserBandSettings.continuityCountEnabled = true;
    parserBandSettings.allPass = false;
    parserBandSettings.acceptNullPackets = false;
    parserBandSettings.maxDataRate = 25000000;
#else
	/* modify them as needed */
    /*parserBandSettings.acceptNullPackets = true;*/ /* this is bad -- don't do this */
#endif

    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
#ifdef USE_SECOND_PROGRAM
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
#else
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x34, NULL);
#endif

/*//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

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
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3)
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

    /* Bring up display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#if 0 /* this is causing the composite to fail on the phase 3 release. */
    if (platformConfig.outputs.svideo[0])
        NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3)
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram); /* Only pass through AC3 */

    printf("Step 1 - opening tnf\n"); getchar();
    BTNF_GetDefaultSettings(&settings);
    err = BTNF_Open(&handle, g_NEXUS_pCoreHandles->chp, g_NEXUS_pCoreHandles->reg, g_NEXUS_pCoreHandles->bint, &settings);
    if (err) { printf("BTNF_Open failed.  exiting.\n"); exit(1); }
    BTNF_SetMem(handle, g_NEXUS_pCoreHandles->heap[0]);

#if 1
    printf("Step 2 sending bus reset\n"); getchar();
	IssueBusReset(handle, /*root=*/false); /* let the transmitter be the root */

    printf("Step 3 getting node ID\n"); getchar();
    printf("Our node ID is: %4x\n", GetNodeId(handle));
#endif

#ifdef EXTRA_OPTIONS
  {
	/* Change the receive settings */
	BTNF_ReceiveSettings receive;
	BTNF_GetReceiveSettings(handle, &receive);
	receive.jitterCompensationDisable = jitter;
	BTNF_SetReceiveSettings(handle, &receive);
  }
#endif

	if (use_all_keys) 
	{
		/* if use all keys then set a key value for all the different emi values */
		int emi; /* make the key values unique by mucking with the low order byte */
		for (emi=1; emi<=3; emi++) {
			keylo = (keylo & ~0xFF) | (emi<<4);
			BTNF_SetKeys(handle, e_receive, emi, false /*even*/, keyhi, keylo);
			BTNF_SetKeys(handle, e_receive, emi, true  /*odd*/,  keyhi, keylo|1);
		}
	}
	else
	{
		/* If they want encryption, then setup the keys first */
		if (sync) BTNF_SetKeys(handle, e_receive, (sync>>2) /*emi*/, (sync&2)?true:false /*odd/even*/, keyhi, keylo);
	} 

    printf("Press ENTER to enable receive\n"); getchar();
    err = BTNF_EnableReceive(handle, channel, sync<<2);

    for (i=0; /*i<5*/; i++) 
    {
#ifdef EXTRA_OPTIONS
	  if (test_mode)
	  {	
		uint32_t value;
		BKNI_Sleep(1000); /* just keep receiving and give up time for others to run */

		/* This is a test to see if we're getting the data corruption issue where the sync is 
		** changing without us wanting it to.  This will just print the status.
		*/
		if (!BTNF_AreInterruptsBeingUsed()) {
			BTNF_ReadInterruptStatus(handle, rx, &value);
			BTNF_ClearInterruptStatus(handle, rx, 0xffffffff);
#if 0
			if (value & 0x7F00) 
#else
			if (value) 
#endif
				printf("sync values are changing (%x)\n", value);
		}
	  } 
	  else 
#endif
	  {
        char x, c;
        printf("Press ENTER to stop receive\n"); 
		x = c = getchar();
		while (x != '\n' && x != '\r') x = getchar(); /* toss out everything up to CR/LF */

        err = BTNF_DisableReceive(handle);

        printf("Press ENTER to restart receive (ues 'q' to quit)\n"); x = c = getchar();
		while (x != '\n' && x != '\r') x = getchar(); /* toss out everything up to CR/LF */
		if (c == 'Q' || c == 'q') break;

        err = BTNF_EnableReceive(handle, channel, sync);
	  }
    }

    printf("Press ENTER to stop decode\n"); getchar();

    /* example shutdown */
    BTNF_Close(handle);

    NEXUS_AudioDecoder_Stop(pcmDecoder);
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3) NEXUS_AudioDecoder_Stop(compressedDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
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

    return 0;
}

