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
 * $brcm_Workfile: 1394_t.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/18/11 2:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/1394/1394_t.c $
 * 
 * Hydra_Software_Devel/3   10/18/11 2:20p gmullen
 * SW7425-1383: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1383/1   10/14/11 4:12p gmullen
 * SW7425-1383: Updated allPass usages to use new API
 * 
 * Hydra_Software_Devel/2   10/21/09 3:24p rjlewis
 * SW7420-380: cleaned up for Take 2 release.
 * 
 * Hydra_Software_Devel/1   10/9/09 5:47p rjlewis
 * SW7420-380: Initial release.
 * 
******************************************************************************/
/* Nexus example app: single live a/v decode from an input band */
/* Also sends same stream out the remux for 1394 testing */

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

/*#define EXTRA_OPTIONS*/ /* these are special functions to enable checking of specific functionality */
/*#define USE_SECOND_PROGRAM*/ /* use second program in stream (different PIDS) */
/*#define USE_RMX1*/  /* use Remux 1 instead of Remux 0 for output */
/*#define PASS_NULLS*/ /* enable passing MPEG NULL packets through to Remux */

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

static void print_usage(char *program)
{
	fprintf(stderr, "%s: usage:  %s [option param]\n" , program, program);
	fprintf(stderr,
		"    --channel (-c) - set isoch channel to use (default=63)\n"
		"    --speed (-s)   - set speed (0=100, 1=200, 2=400; default=0)\n"
		"    --sync (-S)    - set sync value (enables encryption); use -1 to enable swapping keys\n"
		"    --keylo (-l)   - low order 32 bits of key (when sync > 0)\n"
		"    --keyhi (-h)   - high order 24 bits of key (when sync > 0)\n"
	);
#ifdef EXTRA_OPTIONS
	fprintf(stderr,
		"    --fts (-f)     - set future time stamp value (default=0x3000)\n"
	);
	fprintf(stderr,
		"    --rate (-r)    - set the Remux rate (0=81Mhz, 3=20.25Mhz; default=3)\n"
		"    --npd (-n)     - disable the remux null packet disable option\n"
		"    --nrun (-N)    - disable the nrun enable remux function\n"
		"    --delay (-d)   - set the remux packet delay count (default is 0)\n"
		"    --bypass (-b)  - enable the remux by-pass enable bit\n"
		"    --allpass (-a) - enable the remux (and parser) in all pass mode\n"
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
static    NEXUS_RemuxHandle remux;
static    NEXUS_RemuxSettings remuxSettings;
static    NEXUS_ParserBand parserBandRmx;
static    NEXUS_ParserBand parserBand;
static    NEXUS_ParserBandSettings parserBandSettings; /* can we use this for both the parser bands? */
static    NEXUS_StcChannelHandle stcChannel;
static    NEXUS_StcChannelSettings stcSettings;
static    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
static    NEXUS_PidChannelHandle allPassToRemux;
static    NEXUS_PidChannelHandle PATPid, PMTPid;
static    NEXUS_PidChannelHandle videoPidChannelToRemux, audioPidChannelToRemux;
#ifdef PASS_NULLS
static    NEXUS_PidChannelHandle nullPidChannelToRemux;
#endif
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
	int speed = 0; /* 100 */
	int sync = 0; /* this tells us when to use encryption */
	uint32_t keylo = 0x12345678, keyhi = 0x00654321;
	bool use_all_keys = false;
	bool use_all_pass = false;

#ifdef EXTRA_OPTIONS
	int packet_delay_count = 0;
	bool null_packet_disable = true; /* don't let the Remux add extra NULL packets */
	bool nrun_enable = true;
	bool bypass_enable = false;
	int rate = 3; /* default rate for remux = 20.25 Mhz */
	int fts = 0x3000;
#endif

	char *program = *argv++; argc--;

	/* let them over-ride the defaults */
	if (argc && (!strcmp(*argv, "-?") || !strcmp(*argv, "-h") || !strcmp(*argv,"--help"))) print_usage(program);
	while (argc>0) 
	{
		printf("Processing parameter '%s' with argc = %d\n", *argv, argc);
			 if (!strcmp(*argv, "--channel") || !strcmp(*argv, "-c")) { if(--argc) channel = atoi(*++argv); }
		else if (!strcmp(*argv, "--speed")   || !strcmp(*argv, "-s")) { if(--argc) speed   = atoi(*++argv); }
		else if (!strcmp(*argv, "--sync")    || !strcmp(*argv, "-S")) { if(--argc) sync  = atoi(*++argv); }
		else if (!strcmp(*argv, "--keylo")   || !strcmp(*argv, "-l")) { if(--argc) keylo = atoi(*++argv); }
		else if (!strcmp(*argv, "--keyhi")   || !strcmp(*argv, "-h")) { if(--argc) keyhi = atoi(*++argv); }
#ifdef EXTRA_OPTIONS
		else if (!strcmp(*argv, "--fts")   || !strcmp(*argv, "-f")) { if(--argc) fts = atoi(*++argv); }

		else if (!strcmp(*argv, "--rate")  || !strcmp(*argv, "-r")) { if(--argc) rate = atoi(*++argv); }
		else if (!strcmp(*argv, "--npd")   || !strcmp(*argv, "-n")) { null_packet_disable = false; }
		else if (!strcmp(*argv, "--nrun")  || !strcmp(*argv, "-N")) { nrun_enable = false; }
		else if (!strcmp(*argv, "--bypass")|| !strcmp(*argv, "-b")) { bypass_enable = true; }
		else if (!strcmp(*argv, "--delay") || !strcmp(*argv, "-d")) { if(--argc) packet_delay_count = atoi(*++argv); }
		else if (!strcmp(*argv, "--allpass")|| !strcmp(*argv, "-a")) { use_all_pass = true; }
#endif
		else { fprintf(stderr, "%s: unknown parameter (%s)\n", program, *argv); print_usage(program); }
		argv++; argc--;
	}

#ifdef EXTRA_OPTIONS
	if (use_all_pass) rate = 1; /* toontun wants this to be 54Mhz for all pass mode */
#endif

	/* Use a negative sync value to enable using all the different keys */
	/* Also enables ability to toggle between keys */
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
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

/* 
*******************************************************
** This sets up the REMUX to get the streamer output **
*******************************************************
*/

    parserBandRmx = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBandRmx, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
	if (use_all_pass) {
        NEXUS_PidChannelSettings pidSettings;

    	parserBandSettings.allPass = true;
    	NEXUS_ParserBand_SetSettings(parserBandRmx, &parserBandSettings);
    	/* this is used to feed all data to the remux. in allpass mode, it doesn't matter what pid is specified here */

        NEXUS_PidChannel_GetDefaultSettings(&pidSettings);
        NEXUS_ParserBand_GetAllPassPidChannelIndex(parserBandRmx, (unsigned *) &pidSettings.pidChannelIndex);
        allpassPidChannel = NEXUS_PidChannel_Open(parserBandRmx, 0x0, &pidSettings); /* pid is redundant */
	} else {
    	parserBandSettings.allPass = false;
#ifdef PASS_NULLS
    	parserBandSettings.acceptNullPackets = true;
		parserBandSettings.continuityCountEnabled = false; /* note: this will enable PARSER_CONT_COUNT_IGNORE */
#endif
    	NEXUS_ParserBand_SetSettings(parserBandRmx, &parserBandSettings);
    	PATPid = NEXUS_PidChannel_Open(parserBandRmx, 0x00, NULL);
#ifdef USE_SECOND_PROGRAM
		/* note: selecting second program without changing PAT means that most TVs will not display anything */
    	PMTPid = NEXUS_PidChannel_Open(parserBandRmx, 0x3E, NULL);
    	videoPidChannelToRemux = NEXUS_PidChannel_Open(parserBandRmx, 0x11, NULL);
    	audioPidChannelToRemux = NEXUS_PidChannel_Open(parserBandRmx, 0x14, NULL);
#else
    	PMTPid = NEXUS_PidChannel_Open(parserBandRmx, 0x3F, NULL);
    	videoPidChannelToRemux = NEXUS_PidChannel_Open(parserBandRmx, 0x31, NULL);
    	audioPidChannelToRemux = NEXUS_PidChannel_Open(parserBandRmx, 0x34, NULL);
#endif
#ifdef PASS_NULLS
    	nullPidChannelToRemux = NEXUS_PidChannel_Open(parserBandRmx, 0x1FFF, NULL);
#endif
	}

    /* Configure remux */
	if (use_all_pass)
    	remuxSettings.allPass = true;
	else
    	remuxSettings.allPass = false;

#ifdef USE_RMX1
    remux = NEXUS_Remux_Open(1, &remuxSettings);
#else
    remux = NEXUS_Remux_Open(0, &remuxSettings);
#endif

	if (use_all_pass) 
    	NEXUS_Remux_AddPidChannel(remux, allPassToRemux);
	else {
    	NEXUS_Remux_AddPidChannel(remux, PATPid);
    	NEXUS_Remux_AddPidChannel(remux, PMTPid);
    	NEXUS_Remux_AddPidChannel(remux, videoPidChannelToRemux);
    	NEXUS_Remux_AddPidChannel(remux, audioPidChannelToRemux);
#ifdef PASS_NULLS
    	NEXUS_Remux_AddPidChannel(remux, nullPidChannelToRemux);
#endif
	}

/* 
**********************************************************
** This sets up the decoders to get the streamer output **
**********************************************************
*/
    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e1;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
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
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3) /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);

    printf("Step 1 - opening tnf\n"); getchar();
    BTNF_GetDefaultSettings(&settings);
    err = BTNF_Open(&handle, g_NEXUS_pCoreHandles->chp, g_NEXUS_pCoreHandles->reg, g_NEXUS_pCoreHandles->bint, &settings);
    if (err) { printf("BTNF_Open failed.  exiting.\n"); exit(1); }
    BTNF_SetMem(handle, g_NEXUS_pCoreHandles->heap[0]);

#if 1
    printf("Step 2 sending bus reset\n"); getchar();
	IssueBusReset(handle, /*root=*/true); /* we want the transmitter be the root */

    printf("Step 3 getting node ID\n"); getchar();
    printf("Our node ID is: %4x\n", GetNodeId(handle));
#endif

#ifdef EXTRA_OPTIONS
  {
	/* Change the remux settings */
	BTNF_RemuxSettings remux;
	BTNF_GetRemuxSettings(handle, &remux);
	remux.rate = rate;
	remux.null_packet_disable = null_packet_disable;
	remux.nrun_enable = nrun_enable;
	remux.bypass_enable = bypass_enable;
	remux.packet_delay_count = packet_delay_count;
	BTNF_SetRemuxSettings(handle, &remux);
  }
  {
	/* Change the transmit settings */
	BTNF_TransmitSettings transmit;
	BTNF_GetTransmitSettings(handle, &transmit);
	transmit.futureTimeStamp = fts;
	BTNF_SetTransmitSettings(handle, &transmit);
  }
#endif

	if (use_all_keys) 
	{
		/* if use all keys then set a key value for all the different emi values */
		int emi; /* make the key values unique by mucking with the low order byte */
		for (emi=1; emi<=3; emi++) {
			keylo = (keylo & ~0xFF) | (emi<<4);
			BTNF_SetKeys(handle, e_transmit, emi, false /*even*/, keyhi, keylo);
			BTNF_SetKeys(handle, e_transmit, emi, true  /*odd*/,  keyhi, keylo|1);
		}
	}
	else
	{
		/* If they want encryption, then setup the keys first */
		if (sync) BTNF_SetKeys(handle, e_transmit, (sync>>2) /*emi*/, (sync&2)?true:false /*odd/even*/, keyhi, keylo);
	} 

    printf("Press ENTER to enable transmit\n"); getchar();
    err = BTNF_EnableTransmit(handle, speed, channel, sync<<2);

	/* We always have to enable the transmit BEFORE we start the remux (and stop it before disabling transmit) */
	/* Presently, I don't need to start the remux -- the TNF PI will do this */
	/* TODO: If you write a nexus module for the tnf then we should do this here instead of in the tnf PI */
    /*NEXUS_Remux_Start(remux);*/

    for (i=0; /*i<5*/; i++) 
    {
#if 0
		BKNI_Sleep(1000); /* just keep transmitting and give up time for others to run */
#else
        char c, x;
        printf("Press ENTER to stop transmit; 'b' to issue BusReset"); 
		if (use_all_keys) printf("; 't' to toggle keys; 'e' to change emi");
		printf("\n"); 
		x = c = getchar();
		while (x != '\n' && x != '\r') x = getchar(); /* toss out everything up to CR/LF */

		switch (toupper(c)) 
		{
			case 'B': 
				IssueBusReset(handle, /*root=*/true); 
				continue;

			case 'T':
			{
				/* Toggle the odd/even bit of sync */
				if (use_all_keys) err = BTNF_Toggle(handle);
				continue;
			}

			case 'E':
			{
				/* cycle through the different EMI values */
				if (use_all_keys) {
#if 0
					/* this version maintains the encryption by going emi=3(A)->1(C) */
					sync += 4; if (sync>14) sync = (1<<2) | (sync & 2); /* bump emi -- wrap emi=3 back to 1 (maintain odd/even) */
#else
					/* this version goes emi=3(A)->0 so encypted to unencrypted */
					sync += 4; if (sync>14) sync = sync & 2; /* bump emi -- wrap emi=3 back to 0 (maintain odd/even) */
#endif
					err = BTNF_SetNewSync(handle, (sync>>2)/*emi*/, (sync&2)?true:false/*odd/even*/);
				}
				continue;
			}
		}

    	/*NEXUS_Remux_Stop(remux);*/
		err = BTNF_DisableTransmit(handle);

        printf("Press ENTER to restart transmit (use 'q' to quit)\n"); 
		x = c = getchar();
		while (x != '\n' && x != '\r') x = getchar(); /* toss out everything up to CR/LF */
		if (c == 'Q' || c == 'q') break;

        err = BTNF_EnableTransmit(handle, speed, channel, sync);
    	/*NEXUS_Remux_Start(remux);*/
#endif
    }

    printf("Press ENTER to stop decode\n"); getchar();

    /* example shutdown */
    BTNF_Close(handle);

    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3) NEXUS_AudioDecoder_Stop(compressedDecoder);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));

    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);

    /*NEXUS_Remux_Stop(remux);*/
    NEXUS_Remux_Close(remux);

	if (use_all_pass) 
    	NEXUS_PidChannel_Close(allPassToRemux);
	else {
		NEXUS_PidChannel_Close(PATPid);
		NEXUS_PidChannel_Close(PMTPid);
		NEXUS_PidChannel_Close(videoPidChannelToRemux);
		NEXUS_PidChannel_Close(audioPidChannelToRemux);
#ifdef PASS_NULLS
		NEXUS_PidChannel_Close(nullPidChannelToRemux);
#endif
	}

    NEXUS_Platform_Uninit();

    return 0;
}

