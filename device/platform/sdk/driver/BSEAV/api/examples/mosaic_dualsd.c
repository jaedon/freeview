/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: mosaic_dualsd.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/mosaic_dualsd.c $
 * 
 * 3   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <bsettop.h>

#define NUM_MOSAICS (2)
#define MOSAIC_WIDTH 720
#define MOSAIC_HEIGHT 480

/* Settop Resources */
static bdisplay_t g_display;

static bdecode_window_t g_primary_window;
static bdecode_window_t g_mosaic_windows[NUM_MOSAICS];

static bdecode_t  g_primary_decode;
static bdecode_t  g_mosaic_decodes[NUM_MOSAICS];

static bband_t    g_band;
static bstream_t  g_mosaic_streams[NUM_MOSAICS];

/* PIDs */
static struct
{
	uint16_t pid;
	bvideo_codec format;
} g_video_programs[NUM_MOSAICS] = 
{
	{0x11, bvideo_codec_mpeg2},
	{0x21, bvideo_codec_mpeg2}
};

static struct
{
	uint16_t pid;
	baudio_format format;
} g_audio_programs[NUM_MOSAICS] = 
{
	{0x14, baudio_format_ac3},
	{0x24, baudio_format_ac3}
};

static bresult Display_Init(void)
{
	bresult rc;
	bdisplay_settings display_settings;

	g_display = bdisplay_open(B_ID(0));
	if ( NULL == g_display )
	{
		fprintf(stderr, "Unable to open display\n");
		return berr_external_error;
	}	

	bdisplay_get(g_display, &display_settings);
	display_settings.composite = NULL;
	display_settings.svideo = NULL;
	display_settings.rf = NULL;
	display_settings.format = bvideo_format_1080i;	

	rc = bdisplay_set(g_display, &display_settings);
	if ( rc )
	{
		fprintf(stderr, "Unable to set display settings\n");
		return rc;
	}

	/* Success */
	return b_ok;
}

static bresult Display_Uninit(void)
{
	bdisplay_close(g_display);
	return b_ok;
}

static bresult Window_Init(void)
{
	bresult rc;
	int i;
	
	g_primary_window = bdecode_window_open(B_ID(0), g_display);
	if ( NULL == g_primary_window )
	{
		fprintf(stderr, "Unable to open primary decode window\n");
		return berr_external_error;
	}

	for ( i = 0; i < NUM_MOSAICS; i++ )
	{
		bdecode_window_settings window_settings;
	
		g_mosaic_windows[i] = bdecode_window_open_mosaic(g_primary_window, B_ID(i));
		if ( NULL == g_mosaic_windows[i] )
		{
			fprintf(stderr, "Unable to open mosaic window %d\n", i);
			return berr_external_error;
		}
		
		rc = bdecode_window_get(g_mosaic_windows[i], &window_settings);			
		if ( rc )
		{
			fprintf(stderr, "Unable to get window %d settings\n", i);
			return rc;
		}

		window_settings.position.x = i*MOSAIC_WIDTH;
		window_settings.position.y = 0;
		window_settings.position.width = MOSAIC_WIDTH;
		window_settings.position.height = MOSAIC_HEIGHT;

		rc = bdecode_window_set(g_mosaic_windows[i], &window_settings);
		if ( rc )
		{
			fprintf(stderr, "Unable to set window %d settings\n", i);
			return rc;
		}
	}	
	
	return b_ok;
}

bresult Window_Uninit(void)
{
	int i;

	for ( i = NUM_MOSAICS-1; i >= 0; i-- )
	{
		bdecode_window_close(g_mosaic_windows[i]);
	}	

	bdecode_window_close(g_primary_window);

	return b_ok;
}

bresult Decode_Init(void)
{
	int i;
	bdecode_mosaic_settings mosaic_settings;

	g_primary_decode = bdecode_open(B_ID(0));
	if ( NULL == g_primary_decode )
	{
		fprintf(stderr, "Unable to open primary decoder\n");
		return berr_external_error;
	}

	bdecode_mosaic_settings_init(g_primary_decode, &mosaic_settings);
	mosaic_settings.height = MOSAIC_HEIGHT;
	mosaic_settings.width = MOSAIC_WIDTH;

	for ( i = 0; i < NUM_MOSAICS; i++ )
	{
		g_mosaic_decodes[i] = bdecode_open_mosaic(g_primary_decode, B_ID(i), &mosaic_settings);
		if ( NULL == g_mosaic_decodes[i] )
		{
			fprintf(stderr, "Unable to open mosaic decoder %d", i);
			return berr_external_error;
		}
	}

	return b_ok;
}

static bresult Decode_Uninit(void)
{
	int i;

	for ( i = NUM_MOSAICS-1; i >= 0; i-- )
	{
		bdecode_close(g_mosaic_decodes[i]);
	}

	bdecode_close(g_primary_decode);

	return b_ok;
}

static bresult Decode_Start(void)
{
	int i;
	bstream_mpeg mpeg;

	/* Open the streamer for the band, this could be a tuner, etc. */
	g_band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
	if ( NULL == g_band )
	{
		fprintf(stderr, "Unable to open streamer input\n");
		return berr_external_error;
	}

	bstream_mpeg_init(&mpeg);

	/* Open the primary stream first -- this could come from playback instead... */
	mpeg.video[0].pid = g_video_programs[0].pid;
	mpeg.video[0].format = g_video_programs[0].format;
	mpeg.audio[0].pid = g_audio_programs[0].pid;
	mpeg.audio[0].format = g_audio_programs[0].format;
	mpeg.pcr_pid = g_video_programs[0].pid;
	g_mosaic_streams[0] = bstream_open(g_band, &mpeg);
	if ( NULL == g_mosaic_streams[0] )
	{
		fprintf(stderr, "Unable to open stream 0\n");
		return berr_external_error;
	}

	/* Open children */
	for ( i = 1; i < NUM_MOSAICS; i++ )
	{
		/* Open the primary stream first */
		mpeg.video[0].pid = g_video_programs[i].pid;
		mpeg.video[0].format = g_video_programs[i].format;
		mpeg.audio[0].pid = g_audio_programs[i].pid;
		mpeg.audio[0].format = g_audio_programs[i].format;
		mpeg.pcr_pid = g_video_programs[i].pid;
		g_mosaic_streams[i] = bstream_open_child(g_mosaic_streams[0], &mpeg);
		if ( NULL == g_mosaic_streams[i] )
		{
			fprintf(stderr, "Unable to open stream %d\n", i);
			return berr_external_error;
		}
	}

	/* Now, start decode on all streams */
	for ( i = 0; i < NUM_MOSAICS; i++ )
	{
		bresult rc;

		printf("Starting Decode %d\n", i);
		fflush(stdout);
		rc = bdecode_start(g_mosaic_decodes[i], g_mosaic_streams[i], g_mosaic_windows[i]);
		if ( rc )
		{
			fprintf(stderr, "Unable to start decode %d\n", i);
			return rc;
		}
	}

	/* All done */
	return b_ok;
}

static bresult Decode_Stop(void)
{
	return b_ok;
}

int main(int argc, char **argv)
{
	bresult rc;

	#if 1
	fprintf(stderr, "Forcing sync_disabled=1\n");
	putenv("sync_disabled=1");
	#endif

	rc = bsettop_init(BSETTOP_VERSION);
	if ( rc ) 
	{
		fprintf(stderr, "Unable to init settop API\n");
		return -1;
	}

	rc = Display_Init();
	if ( rc )
	{
		fprintf(stderr, "Unable to init display\n");
		return -1;
	}	

	rc = Window_Init();
	if ( rc )
	{
		fprintf(stderr, "Unable to init windows\n");
		return -1;		
	}

	rc = Decode_Init();
	if ( rc )
	{
		fprintf(stderr, "Unable to init decoders\n");
		return -1;
	}

	rc = Decode_Start();
	if ( rc )
	{
		fprintf(stderr, "Unable to start decoders\n");
		return -1;
	}

	printf("Decode started, press enter to exit\n");
	getchar();

	rc = Decode_Stop();
	if ( rc )
	{
		fprintf(stderr, "Unable to stop decoders\n");
		return -1;
	}
	
	rc = Decode_Uninit();
	if ( rc )
	{
		fprintf(stderr, "Unable to close decoders\n");
		return -1;
	}

	rc = Window_Uninit();
	if ( rc )
	{
		fprintf(stderr, "Unable to close windows\n");
		return -1;
	}

	rc = Display_Uninit();
	if ( rc )
	{
		fprintf(stderr, "Unable to close display\n");
		return -1;
	}

	bsettop_uninit();

	printf("Program Exited Successfully\n");

	return 0;
}
