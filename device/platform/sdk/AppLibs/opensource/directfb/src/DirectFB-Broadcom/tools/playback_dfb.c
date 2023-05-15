/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: playback_dfb.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/3 $
 * $brcm_Date: 3/19/12 3:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/tools/playback_dfb.c $
 *
 * DirectFB_1_4_15_Port/3   3/19/12 3:37p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree.
 *
 * DirectFB_1_4_15_Port/2   2/27/12 1:57p robertwm
 * SW7231-690: DirectFB-1.4.15: Add decode server/client test to help with
 *  DFB platform and Nexus MP usage.
 *
 * DirectFB_1_4_15_Port/1   10/19/11 3:38p christ
 * SW7420-2102: DirectFB: Require example DFB multi-app test applications
 *  with video
 *
 * 10   3/22/10 2:29a randyjew
 * SW7468-143: Support for boards without component and/or composite
 *
 * 9   5/26/09 12:27p jtna
 * PR51841: correct audio PID for cnnticker.mpg is 0x22
 *
 * 8   3/5/09 5:46p vsilyaev
 * PR 52579: Updated circular FIFO/timeshifting code
 *
 * 7   2/19/09 1:32p erickson
 * PR51841: refactor
 *
 * 6   2/19/09 11:14a erickson
 * PR51841: add NEXUS_PlaybackPidChannelSettings
 *
 * 5   2/4/09 12:49p erickson
 * PR51841: simplify example
 *
 *****************************************************************************/
/* Nexus example app: playback and decode */
#define inline __inline__

#include "dfb_platform.h"
#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#include <directfb.h>

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#define DFB_PLATFOM_INIT    1 /* Set to 1 to allow DFB to perform the initialisation rather than Nexus */
#define NEXUS_MASTER        1 /* Set to 1 to allow this app to be run as the master in the system */
#define DFB_MASTER          1

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(...)                                                     \
     {                                                                    \
          err = __VA_ARGS__;                                              \
          if (err != DFB_OK) {                                            \
               fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );     \
               DirectFBErrorFatal( #__VA_ARGS__, err );                   \
          }                                                               \
     }

void print_usage(void)
{
     printf ("DirectFB Video Playback Demo\n\n");
     printf ("Usage: playback_dfb [options]\n\n");
     printf ("Options:\n\n");
     printf ("  --file                       MPEG2 transport stream for playback.\n");
     printf ("  --apid                       Audio PID (decimal).\n");
     printf ("  --vpid                       Video PID (decimal).\n");
     printf ("  --vcodec                     Video Codec (decimal): 0=MPEG-2,   1=H.264, 2=MPEG-1, 3=VC-1,\n");
     printf ("  --acodec                     Audio Codec (decimal): 0=MPEG-1/2, 1=AC-3,  2=AC3+,   3=AAC\n");
     printf ("  --help                       Print usage information.\n");
     printf ("  --dfb-help                   Output DirectFB usage information.\n\n");
}

typedef struct vcodec_t
{
    NEXUS_VideoCodec codec;
    char *name;
} vcodec_t;

typedef struct acodec_t
{
    NEXUS_AudioCodec codec;
    char *name;
} acodec_t;

static const vcodec_t videoCodecs[] =
{
    { NEXUS_VideoCodec_eMpeg2,   "MPEG-2" },
    { NEXUS_VideoCodec_eH264,    "H.264"  },
    { NEXUS_VideoCodec_eMpeg1,   "MPEG-1" },
    { NEXUS_VideoCodec_eVc1,     "VC-1"   },
};

static const acodec_t audioCodecs[] =
{
    { NEXUS_AudioCodec_eMpeg,    "MPEG"   },
    { NEXUS_AudioCodec_eAc3,     "AC-3"   },
    { NEXUS_AudioCodec_eAc3Plus, "AC-3+"  },
    { NEXUS_AudioCodec_eAac,     "AAC"    },
};


int main(int argc, char *argv[])  {
#if DFB_MASTER
    IDirectFB               *dfb;
    IDirectFBFont           *font;
    IDirectFBSurface        *windowSurface;
    IDirectFBSurface        *primary;
    IDirectFBImageProvider  *provider;
    IDirectFBDisplayLayer   *layer;
    IDirectFBWindow         *window2;
    DFBFontDescription       desc;
    DFBWindowDescription     wdesc;
    DFBResult                err;
    int                      fontheight, xres, yres, str_len;
    char                     buf[512];
#endif

#if NEXUS_HAS_PLAYBACK
    DFB_PlatformSettings dfbPlatformSettings;
#if NEXUS_MASTER && !DFB_PLATFOM_INIT
    NEXUS_PlatformSettings platformSettings;
#endif
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoCodec vcodecIdx = 0; /* Default to MPEG-2 Video */
    NEXUS_AudioCodec acodecIdx = 0; /* Default to MPEG Audio */
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    char fname[64];
    int n,apid=-1,vpid=-1;
    char inputc;
    bool bInput = true;

    /* parse command line */
    for (n = 1; n < argc; n++) {
        if (strncmp (argv[n], "--", 2) == 0) {
            if (strcmp (argv[n] + 2, "help") == 0) {
                print_usage();
                return 0;
            }
            else
            {
                if (strcmp (argv[n] + 2, "file") == 0 &&
                    ++n < argc &&
                    sscanf (argv[n], "%s", fname) == 1) {
                    continue;
                }
                else if (strcmp (argv[n] + 2, "vpid") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%d", &vpid) == 1) {
                    continue;
                }

                else if (strcmp (argv[n] + 2, "apid") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%d", &apid) == 1) {
                    continue;
                }
                else if (strcmp (argv[n] + 2, "vcodec") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%u", &vcodecIdx) == 1) {
                    if (vcodecIdx > sizeof(videoCodecs)/sizeof(videoCodecs[0])) {
                        fprintf(stderr, "ERROR: Invalid video codec specified!\n");
                        print_usage();
                        return -1;
                    }
                    continue;
                }
                else if (strcmp (argv[n] + 2, "acodec") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%u", &acodecIdx) == 1) {
                    if (acodecIdx > sizeof(audioCodecs)/sizeof(audioCodecs[0])) {
                        fprintf(stderr, "ERROR: Invalid audio codec specified!\n");
                        print_usage();
                        return -1;
                    }
                    continue;
                }
            }
        }
    }

    if (argc < 2)
    {
        printf("Need to supply at least a filename & one PID!!\n\n");
        print_usage();
        return -1;
    }

    printf("Playback of %s - audio %d(codec=%s) - video %d(codec=%s)\n", fname, apid, audioCodecs[acodecIdx].name, vpid, videoCodecs[vcodecIdx].name);

#if DFB_PLATFOM_INIT

#if NEXUS_MASTER
    /* Now that Nexus is initialised and we have opened up the display
       module, we can initialise DirectFB with this information. */
    DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eMasterNexusUninitialized, &dfbPlatformSettings);
    DFB_Platform_Init(&dfbPlatformSettings);
#else
    DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eSlaveNexusUninitialized, &dfbPlatformSettings);
    DFB_Platform_Init(&dfbPlatformSettings);
#endif

#else

#if NEXUS_MASTER
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    NEXUS_Platform_Init(&platformSettings);
    DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eMasterNexusInitialized, &dfbPlatformSettings);
    DFB_Platform_Init(&dfbPlatformSettings);
#else
    NEXUS_Platform_Join();
    DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eSlaveNexusInitialized, &dfbPlatformSettings);
    DFB_Platform_Init(&dfbPlatformSettings);
#endif

#endif

#if DFB_MASTER
     DirectFBInit( &argc, &argv );

     /* create the super interface */
     DirectFBCreate( &dfb );
#endif

    DFB_Platform_GetSettings(&dfbPlatformSettings);

    display = dfbPlatformSettings.display[0].handle;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
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
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
#if NEXUS_NUM_AUDIO_DACS
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    window = dfbPlatformSettings.displayWindow[0].handle;
    /*window = NEXUS_VideoWindow_Open(display, 0);*/

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoWindow_AddInput(dfbPlatformSettings.displayWindow[1].handle, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);

    if (vpid != -1)
    {
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidSettings.pidTypeSettings.video.codec = videoCodecs[vcodecIdx].codec; /* must be told codec for correct handling */
        playbackPidSettings.pidTypeSettings.video.index = true;
        playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, vpid, &playbackPidSettings);

        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = videoCodecs[vcodecIdx].codec;
        videoProgram.pidChannel = videoPidChannel;
        videoProgram.stcChannel = stcChannel;

        /* Start decoder */
        NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    }

    if (apid != -1)
    {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, apid, &playbackPidSettings);

        /* Set up decoder Start structures now. We need to know the audio codec to properly set up
           the audio outputs. */

        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.codec = audioCodecs[acodecIdx].codec;
        audioProgram.pidChannel = audioPidChannel;
        audioProgram.stcChannel = stcChannel;

        /* Start decoder */
        NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    }
    /*Remove Component and Composite output from Display 0 and Display 1. HDMI output is connected to display 0. Menu Options can then be used to add/remove output type*/
    DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[0].handle, DFB_PlatformDisplayOutputType_eComposite, 0);
    DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[0].handle, DFB_PlatformDisplayOutputType_eComponent, 0);

    DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[1].handle, DFB_PlatformDisplayOutputType_eComponent, 0);
    DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[1].handle, DFB_PlatformDisplayOutputType_eComposite, 0);


    /* Start playback */
    NEXUS_Playback_Start(playback, file, NULL);

#if DFB_MASTER

    DFBCHECK(dfb->GetDisplayLayer( dfb, DLID_PRIMARY, &layer ));
    layer->SetCooperativeLevel( layer, DLSCL_ADMINISTRATIVE );

    /* Find out the size of the primary surface */
    layer->GetSurface(layer, &primary);

    primary->GetSize(primary,&xres,&yres);

    primary->Release(primary);

    /* Create a window to put our graphics into, using a window so we can do multi app compositing */
    wdesc.flags  = ( DWDESC_POSX | DWDESC_POSY |
                     DWDESC_WIDTH | DWDESC_HEIGHT );
    wdesc.width = xres;
    wdesc.height = yres;
    wdesc.posx = 0;
    wdesc.posy = 0;

    layer->CreateWindow( layer, &wdesc, &window2 );
    window2->GetSurface( window2, &windowSurface );

    windowSurface->SetColor( windowSurface, 0, 0, 0, 0x30 );
    windowSurface->SetBlittingFlags(windowSurface, DSBLIT_BLEND_COLORALPHA );

    /* Get DirectFB logo from PNG file */
    DFBCHECK(dfb->CreateImageProvider( dfb,
                                       DATADIR"/desktop.png",
                                       &provider ));

    /* Copy logo to surface */
    provider->RenderTo( provider, windowSurface, NULL );
    provider->Release( provider );

    /* Load font */
    desc.flags = DFDESC_HEIGHT;
    desc.height = 24;

    DFBCHECK(dfb->CreateFont( dfb, FONT, &desc, &font ));
    DFBCHECK(font->GetHeight( font, &fontheight ));
    DFBCHECK(windowSurface->SetFont( windowSurface, font ));

    snprintf( buf, sizeof(buf), "Now playing: %s",fname);

    font->GetStringWidth(font, buf, -1, &str_len);

    /* Draw box to bound the text */
    windowSurface->SetColor( windowSurface, 0, 0, 60, 0xa0 );
    windowSurface->FillRectangle( windowSurface, 10, 40, str_len+20, fontheight+5 );

    /* Print playback information */
    windowSurface->SetColor( windowSurface, 190, 210, 255, 0xFF );
    windowSurface->DrawString( windowSurface, buf, -1, 20, 40, DSTF_LEFT | DSTF_TOP );

    window2->SetOpacity( window2, 0x80 );

    /* Flip display to make it all appear */
    windowSurface->Flip( windowSurface, NULL, 0 );

    font->Release(font); /* Finished with font */

#endif

    while (bInput)
    {
        printf("1) add HDMI Output\n");
        printf("2) remove HDMI Output\n");
        printf("3) add Component Output to Display 0\n");
        printf("4) remove Component Output from Display 0\n");
        printf("5) add Composite Outputto Display 0\n");
        printf("6) remove Composite Output from Display 0\n");
        printf("7) add Composite Output to Display 1\n");
        printf("8) remove Composite Output from Display 1\n");
        printf("9) add Component Output to Display 1\n");
        printf("a) remove Component Output from Display 1\n");
        printf("0) Quit Application\n");
        printf("  ==> \n");

        inputc = getchar();
        switch (inputc)
        {
            case '0':
                bInput = false;
                break;
            case '1':
                DFB_Platform_AddDisplayOutput(display, DFB_PlatformDisplayOutputType_eHdmi, 0);
                break;
            case '2':
                DFB_Platform_RemoveDisplayOutput(display, DFB_PlatformDisplayOutputType_eHdmi, 0);
                break;
            case '3':
                DFB_Platform_AddDisplayOutput(dfbPlatformSettings.display[0].handle, DFB_PlatformDisplayOutputType_eComponent, 0);
                break;
            case '4':
                DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[0].handle, DFB_PlatformDisplayOutputType_eComponent, 0);
                break;
            case '5':
                DFB_Platform_AddDisplayOutput(dfbPlatformSettings.display[0].handle, DFB_PlatformDisplayOutputType_eComposite, 0);
                break;
            case '6':
                DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[0].handle, DFB_PlatformDisplayOutputType_eComposite, 0);
                break;
            case '7':
                DFB_Platform_AddDisplayOutput(dfbPlatformSettings.display[1].handle, DFB_PlatformDisplayOutputType_eComposite, 0);
                break;
            case '8':
                DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[1].handle, DFB_PlatformDisplayOutputType_eComposite, 0);
                break;
            case '9':
                DFB_Platform_AddDisplayOutput(dfbPlatformSettings.display[1].handle, DFB_PlatformDisplayOutputType_eComponent, 0);
                break;
            case 'a':
                DFB_Platform_RemoveDisplayOutput(dfbPlatformSettings.display[1].handle, DFB_PlatformDisplayOutputType_eComponent, 0);
                break;
        }

    }
    /* Bring down system */
    NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    if (vpid != -1)
        NEXUS_VideoDecoder_Stop(videoDecoder);
    if (apid != -1)
        NEXUS_AudioDecoder_Stop(audioDecoder);

    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    /*NEXUS_VideoWindow_Close(window);*/
#if NEXUS_NUM_AUDIO_DACS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
#endif
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_StcChannel_Close(stcChannel);

#if DFB_MASTER
    window2->Destroy(window2);
    dfb->Release( dfb );
#elif DFB_PLATFOM_INIT
    DFB_Platform_Uninit();
#endif
#endif
    return 0;
}
