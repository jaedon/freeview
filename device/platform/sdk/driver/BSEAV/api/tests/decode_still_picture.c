/***************************************************************************
 *     Copyright (c) 2006-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: decode_still_picture.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 10/4/07 5:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/decode_still_picture.c $
 * 
 * 14   10/4/07 5:59p katrep
 * PR35747: Added support for 7405.
 * 
 * 13   1/10/07 11:46a erickson
 * PR26351: added sleep to avoid loss of 4K zero-flush in kernel mode
 * 
 * 12   12/14/06 2:48p jgarrett
 * PR 20139: Reorganizing calls to support bsettop_init() failing.
 * 
 * 11   6/28/06 7:08p jgarrett
 * PR 22063: reworked command-line processing and allow multiple images to
 * be specified on the command line.  Also added -auto mode for stress
 * testing
 * 
 * 10   5/8/06 1:18p erickson
 * PR18616: added event to start decode only after playback is done
 * 
 * 9   4/24/06 5:06p erickson
 * PR18616: added -video option, default live decode to start, added --
 * help
 * 
 **********************************************************************/
#include "bsettop.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdio.h>

/* callback when still is available */
static void still_picture_done(void *data)
{
    BKNI_SetEvent((BKNI_EventHandle)data);
}

const char *g_pszImageFileTable[] = {
    "cnnticker.still.es",
    "I_Frame.es",
    "lquant.mpg",
};

const char **g_pszImageFiles = g_pszImageFileTable;
int g_nImages = sizeof(g_pszImageFileTable)/sizeof(const char *);

static bplayback_loopmode eos_callback(void *context) 
{
    BKNI_SetEvent((BKNI_EventHandle)context);
    return bplayback_loopmode_pause;
}

int main(int argc, const char **argv) {
    bresult rc;
    bdecode_t decode0;
    #if BCHP_CHIP==7400
    bdecode_t decode1;
    #else
    bdecode_t decode1;
    #endif
    bdisplay_t display;
    bgraphics_t graphics;
    bdecode_window_t window;
    bplaypump_t playpump;
    bplayback_t playback;
    bplayback_file_t file;
    bstream_mpeg mpeg;
    bband_t band;
    bstream_t live_stream, still_stream;
    bplayback_params playback_params;
    bsurface_t still_surface, framebuffer;
    BKNI_EventHandle event;
    bsurface_create_settings surface_create_settings;
    bsettop_rect destrect = {0,0,360,240}; /* display still in upper left corner */
    bdecode_settings decode_settings;
    bgraphics_settings gs;
    int image_index = 0;
    int curarg = 1;
    unsigned short video_pid = 0x11;
    bool auto_mode=false;

    rc = bsettop_init(BSETTOP_VERSION);
    if (rc) goto error;
    
    decode0 = bdecode_open(B_ID(0)); /* main decode */
    #if BCHP_CHIP==7400 || BCHP_CHIP==7405 
    decode1 = bdecode_open(B_ID(2));
    #else
    decode1 = bdecode_open(B_ID(1)); /* still picture decode */
    #endif
    display = bdisplay_open(B_ID(0));
    graphics = bgraphics_open(B_ID(0), display);
    window = bdecode_window_open(B_ID(0), display);
    playpump = bplaypump_open(B_ID(1), NULL); /* must use second playpump for still */
    playback = bplayback_open();

    while (curarg < argc) 
    {
        if (!strcmp(argv[curarg], "--help")) 
        {
            /* Dump Usage */
            printf("Usage: ./decode_still_picture [-video PID] [-auto] [still_image_1] ... [still_image_n]\n");
            exit(1);
        }
        else if (!strcmp(argv[curarg], "-video") && curarg+1 < argc) 
        {
            /*  Use video pid specified on command line */
            video_pid = strtoul(argv[++curarg], NULL, 0);
        }
        else if ( !strcmp(argv[curarg], "-auto") )
        {
            /* Automatically advance as fast as possible to stress-test */
            auto_mode = true;
        }
        else 
        {
            /* Use still images specified on the command line */
            g_pszImageFiles = &argv[curarg];
            g_nImages = argc-curarg;
            break;
        }
        curarg++;
    }
    
/* we have three options for graphics + video:
1. use color key
  - this causes video to bleed through. we are using pure black for the colorkey color.
  - but it allows us to you YCrYCb for the framebuffer, so there's no pixel conversion which affects color
  - BUG: I'm having a problem with YCrCb colorkey. So it's not usable. However it's helpful if you want to eliminate
    color conversion.
2. use alpha per pixel (ARGB)
  - this allows us to use no color key
  - but there's no YCrYCrb with alpha per pixel, so we must convert
3. make the frame buffer smaller than full screen.
  - then you can have YCrYCb but not require colorkey. this is nice for test. but real UI's don't do this.
  
Options 1 and 2 are implemented here. Option 2 is default. If you uncomment #define USE_YCRCB_AND_COLORKEY, you'll get option 1.
*/

#if 0
#define USE_YCRCB_AND_COLORKEY
#endif

    /* create graphics framebuffer to display still */
    bsurface_create_settings_init(&surface_create_settings, graphics);
#ifdef USE_YCRCB_AND_COLORKEY
    /* you will see some video come through */
    surface_create_settings.pixel_format = bgraphics_pixel_format_y08_cb8_y18_cr8; /* no conversion, better color */
#else
    /* this means there will be a colorspace conversion */
    surface_create_settings.pixel_format = bgraphics_pixel_format_a1_r5_g5_b5;
#endif
    framebuffer = bgraphics_create_framebuffer(graphics, &surface_create_settings, false /* single buffered */);
    
    bgraphics_get(graphics, &gs);
#ifdef USE_YCRCB_AND_COLORKEY
/*  gs.upper_chromakey = 0x00800080;
    gs.lower_chromakey = 0x00800080;
*/
    gs.chromakey_enabled = true;
#else   
    gs.chromakey_enabled = false;
#endif
    bgraphics_set(graphics, &gs);
    
    /* PID of 0 specifies no video */
    if (video_pid) 
    {   
        /* first, start the live decode from streamer (TODO: hardcoded avc pid) */
        bstream_mpeg_init(&mpeg);
        mpeg.video[0].pid = video_pid;
        band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
        if (!band) 
            goto error;
        live_stream = bstream_open(band, &mpeg);
        if (!live_stream) 
            goto error;
        if (bdecode_start(decode0, live_stream, window)) 
            goto error;
            
        if ( !auto_mode )
        {
            printf("press Enter to decode first still...\n");
            getchar();
        }
    }

    BKNI_CreateEvent(&event);
    bdecode_get(decode1, &decode_settings);
    decode_settings.still_picture_done = still_picture_done;
    decode_settings.callback_context = (void*)event;
    bdecode_set(decode1, &decode_settings);
        
    /* send still pictures, one at a time */
    for ( image_index=0; ; image_index = (image_index+1)%g_nImages )
    {
        BKNI_EventHandle playback_done_event;
        
        /* copy into upper left corner of framebuffer */
        bsurface_fill(framebuffer, &destrect, 0xFF0000FF);

        file = bplayback_file_open(g_pszImageFiles[image_index], NULL);
        if ( NULL == file )
        {
            /* don't fail hard on this error, it happens often */
            fprintf(stderr, "Unable to open file '%s', skipping.\n", g_pszImageFiles[image_index]);
            if ( !auto_mode )
            {
                printf("Press enter for next image ('%s')...\n\n", g_pszImageFiles[(image_index+1)%g_nImages]); 
                getchar(); /* press enter to decode next still image */
            }
            continue;
        }

        /* MPEG2 video ES */
        bstream_mpeg_init(&mpeg);
        mpeg.video[0].pid = 1;
        mpeg.mpeg_type = bstream_mpeg_type_es;
        
        BKNI_CreateEvent(&playback_done_event);
        
        bplayback_params_init(&playback_params, playback);
        playback_params.index_format = bindex_format_none;
        playback_params.end_of_stream = eos_callback;
        playback_params.callback_context = playback_done_event;
    
        still_stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
        if (!still_stream) 
        {
            printf("Unable to start playback on still picture %s\n", g_pszImageFiles[image_index]);
            goto error;
        }
            
        /* don't start decode until we've fed everything */
        BKNI_WaitForEvent(playback_done_event, BKNI_INFINITE);
        
        /* Playback will now send 4K of zeros to flush last startcode through XPT.
        There is no callback for this, so we sleep a small amount. This is a weakness in the Settop API. 
        This is essential for the kernel mode because the proxy layer will be locked as soon as we call bdecode_start,
        and bplayback won't be able to get the 4K through. */
        BKNI_Sleep(10);
        
        if (bdecode_start(decode1, still_stream, NULL)) 
        {
            printf("Unable to start decode on still picture %s\n", g_pszImageFiles[image_index]);
            goto error;
        }
        
        /* wait for picture to decode */
        printf("Waiting for still image...\n\n");   
        BKNI_WaitForEvent(event, BKNI_INFINITE);
        
        still_surface = bdecode_acquire_capture_buffer(decode1, graphics, NULL);
        if (!still_surface) {
            printf("Unable to decode still picture %s\n", g_pszImageFiles[image_index]);
            goto error;
        }
        
        /* copy into upper left corner of framebuffer */
        bsurface_copy(framebuffer, &destrect, still_surface, NULL);

        if ( !auto_mode )
        {
            printf("Still image '%s' should be visible.\nPress enter for next image ('%s')...\n\n", g_pszImageFiles[image_index],
                   g_pszImageFiles[(image_index+1)%g_nImages]); 
            getchar(); /* press enter to decode next still image */
        }

        /* tear down */
        bdecode_stop(decode1);
        bplayback_stop(playback);
        bplayback_file_close(file);

        bsurface_destroy(still_surface);
    }
    
    return 0;
error:
    return 1;
}
