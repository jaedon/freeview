/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: playback_svc_mvc_demo.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 6/25/12 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/playback_svc_mvc_demo.c $
 * 
 * Hydra_Software_Devel/12   6/25/12 3:29p jtna
 * SW7422-170: decrease CDB size requirement
 * 
 * Hydra_Software_Devel/11   3/28/11 1:42p jtna
 * SW7422-170: add stcchannel, which appears to help with SVC 3D video
 * "racing"
 * 
 * Hydra_Software_Devel/10   3/25/11 7:04p jtna
 * SW7422-170: add SVC 3D support
 * 
 * Hydra_Software_Devel/9   3/18/11 12:33p jtna
 * SW7422-170: initial app refactoring
 * 
 * Hydra_Software_Devel/8   2/23/11 3:23p jtna
 * SW7422-183: remove EOS wrap workaround
 * 
 * Hydra_Software_Devel/7   2/22/11 4:13p jtna
 * SW7422-184: remove AVC<->SVC/MVC switch workaround
 * 
 * Hydra_Software_Devel/6   1/6/11 4:40p jtna
 * SW7422-170: remove code that reapplies XVD 3d setting after each video
 * decoder stop
 * 
 * Hydra_Software_Devel/5   1/6/11 4:28p jtna
 * SW7422-181: removed workaround for pink/green garbage in MVC decode
 * 
 * Hydra_Software_Devel/4   1/6/11 4:16p jtna
 * SW7422-170: fix MVC to half-res SVC transition no decode issue. app
 * bug.
 * 
 * Hydra_Software_Devel/3   1/6/11 11:22a jtna
 * SW7422-170: set default frame rate to 60 and remove hack of setting
 * moving content interpolation mode
 * 
 * Hydra_Software_Devel/2   1/5/11 4:00p jtna
 * SW7422-170: more comments on how to control the app
 * 
 * Hydra_Software_Devel/1   1/5/11 3:27p jtna
 * SW7422-170: add playback_svc_mvc_demo.c. lots of hacks and workarounds
 * for now.
 * 
 *****************************************************************************/

/* Compile this app with UNITTEST_BWIN_SUPPORT=y.

Controls:
- Number buttons are mapped to streams. The stream locations are hard-coded.
- Vol+ selects SVC or MVC mode, depending on the stream. Vol- selects AVC mode.
- Ch+ selects a higher display output format. Ch- selects a lower display output format.
- Info toggles the text

*/

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_hdmi_output.h"
#include "nexus_core_utils.h"
#include "nexus_ir_input.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#include "nexus_graphics2d.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bkni.h"
#include "bwin.h"

BDBG_MODULE(playback_svc_mvc_demo);

#define DISPLAY_TEXT 1

void restart_playback(void);
int open_file(int index);
NEXUS_VideoFormat get_nexus_video_format(int direction);
void change_decode_mode(bool avc_only, bool switch_3d);
void change_display_format(NEXUS_VideoFormat format, bool init);
static void set_hdmi_vsi(void);
void print_settings(void);
void update_text(void);

/* current state */
typedef enum {
    streamtype_eSvc = 0,
    streamtype_eMvc,
    streamtype_eSvc3DHalf, /* half-res 3D, which is just 2D+VSI */
    streamtype_eSvc3DFull  /* referred to as "3D SVC". AVC is half-res 3D and SVC and full-res 3D */
} streamtype;

struct {
    char name[256];
    streamtype type;

    /* preferred display video format index in g_videoFormatsSvc and g_videoFormatsMvc arrays.
       nexus could query it, but it's faster this way */
    unsigned videoFormatSMvcIndex;
    unsigned videoFormatAvcIndex;

    NEXUS_VideoFrameRate framerate; /* the SVC test streams do not have a coded framerate which can cause issues. See SW7422-175 */
    /* TODO: ask wwan if SVC 3D monsterhouse stream in AVC mode should be 1080p60 and 1080p24 */
} g_filelist[] = {

    {"/mnt/usb/streams/svc/cactus.svc.2d.9s.v8.ts",                     streamtype_eSvc,       0, 1, NEXUS_VideoFrameRate_e60},     /* 1 */
    {"/mnt/usb/streams/svc/basketballdrive.svc.2d.9s.v8.ts",            streamtype_eSvc,       0, 1, NEXUS_VideoFrameRate_e60},     /* 2 */
    {"/mnt/usb/streams/svc/test_sequenceB.ts",                          streamtype_eSvc,       0, 1, NEXUS_VideoFrameRate_e60},     /* 3 */
    {"/mnt/usb/streams/svc/uruguayvnetherlands.3d.sbs.60s.v6.ts",       streamtype_eSvc3DHalf, 0, 1, NEXUS_VideoFrameRate_e60},     /* 4 */
    {"/mnt/usb/streams/mvc/monsterhouse.mvc.remux3.ts",                 streamtype_eMvc,       0, 0, NEXUS_VideoFrameRate_e23_976}, /* 5 */ /* MVC streams have an ES-coded frame rate of 23.976 but a TS-coded frame rate of 24 */
    {"/mnt/usb/streams/mvc/openseason.mvc.remux.ts",                    streamtype_eMvc,       0, 0, NEXUS_VideoFrameRate_e23_976}, /* 6 */
    {"/mnt/usb/streams/svc/planetearthd1-2.svc.2d.60s.v7.1.ts",         streamtype_eSvc,       1, 2, NEXUS_VideoFrameRate_e60},     /* 7 */
    {"/mnt/usb/streams/svc/planetearthd1-2.svc.2d.60s.v7.2.ts",         streamtype_eSvc,       1, 2, NEXUS_VideoFrameRate_e60},     /* 8 */
    {"/mnt/usb/streams/svc/planetearthd1-2.svc.2d.60s.v7.ts",           streamtype_eSvc,       1, 2, NEXUS_VideoFrameRate_e60},     /* 9 */
    {"/mnt/usb/streams/svc/Allegro_SVC_Inter_04_Cabac_HD2@60Hz_1.2.ts", streamtype_eSvc,       0, 1, NEXUS_VideoFrameRate_e60},     /* 0 */
    {"/mnt/usb/streams/svc/uruguayvnetherlands.3d.sbs.60s.v6.ts",       streamtype_eSvc,       0, 1, NEXUS_VideoFrameRate_e60},     /* * */
    {"/mnt/usb/streams/svc/svc3d_monsterhouse.ts",                      streamtype_eSvc3DFull, 0, 0, NEXUS_VideoFrameRate_e60},     /* # */
};
#define NUM_STREAMS (sizeof(g_filelist)/sizeof(*g_filelist))

struct {
    NEXUS_VideoFormat format;
    NEXUS_VideoFormat format3d; /* equivalent 3d format */
    char name[8];
} 
g_videoFormatsSvc[] = { /* used for SVC 2D and SVC half-res 3D */
    {NEXUS_VideoFormat_e1080p, 0, "1080p"},
    {NEXUS_VideoFormat_e720p,  0, "720p"},
    {NEXUS_VideoFormat_e480p,  0, "480p"}
},
g_videoFormatsMvc[] = { /* used for MVC and SVC full-res 3D */
    {NEXUS_VideoFormat_e1080p24hz, NEXUS_VideoFormat_e3D_1080p_24Hz, "1080p"},
    {NEXUS_VideoFormat_e720p,      NEXUS_VideoFormat_e3D_720p,       "720p"}
};

#define NUM_SVC_FORMATS (sizeof(g_videoFormatsSvc)/sizeof(*g_videoFormatsSvc))
#define NUM_MVC_FORMATS (sizeof(g_videoFormatsMvc)/sizeof(*g_videoFormatsMvc))

struct g_state {
    unsigned fileIndex;
    bool avc_only; /* if false, then MVC or SVC, depending on stream */
    unsigned videoFormatIndex; /* videoformat in the g_videoFormatsSvc and g_videoFormatsMvc arrays */
    bool text_on;
} g_state;


static NEXUS_IrInputHandle g_irHandle;

void ir_callback(void* pParam, int iParam)
{
    NEXUS_IrInputEvent irEvent;
    size_t numEvents = 1;
    bool overflow;
    unsigned int keypress;
    int openfile = -1;
    NEXUS_Error rc = 0;

    BSTD_UNUSED(iParam);
    BSTD_UNUSED(pParam);

    while (numEvents && rc==0) {
        rc = NEXUS_IrInput_GetEvents(g_irHandle, &irEvent, 1, &numEvents, &overflow);

        if (overflow) {
            printf("Overflow, rc: %d, numEvents:%d\n", rc, numEvents);
        }

        if (numEvents && !irEvent.repeat) {
            keypress = (irEvent.code & 0xffff);

#if 0
            if (keypress == 53266 /* exit */ || keypress == 24586 /* power */) {
                exit(-1);
            }
#else
            if (0) {}
#endif
            else if (keypress == 61441) /* 1 */ { openfile = 0; }
            else if (keypress == 57346) /* 2 */ { openfile = 1; }
            else if (keypress == 53251) /* 3 */ { openfile = 2; }
            else if (keypress == 49156) /* 4 */ { openfile = 3; }
            else if (keypress == 45061) /* 5 */ { openfile = 4; }
            else if (keypress == 40966) /* 6 */ { openfile = 5; }
            else if (keypress == 36871) /* 7 */ { openfile = 6; }
            else if (keypress == 32776) /* 8 */ { openfile = 7; }
            else if (keypress == 28681) /* 9 */ { openfile = 8; }
            else if (keypress == 0)     /* 0 */ { openfile = 9; }
            else if (keypress == 36886) /* * */ { openfile = 10; }
            else if (keypress == 61456) /* # */ { openfile = 11; }

            /* TODO: vol+/- trigger a display format change. we should have a mode that just restarts playback without a display format change */
            else if (keypress == 12301) { /* vol+: SVC/MVC */
                if (g_state.avc_only) {
                    NEXUS_VideoFormat format;
                    g_state.videoFormatIndex = g_filelist[g_state.fileIndex].videoFormatSMvcIndex;
                    g_state.avc_only = false;
                    format = get_nexus_video_format(0);
                    change_display_format(format, false);

                    change_decode_mode(g_state.avc_only, true);
                    restart_playback();

                    if (g_filelist[g_state.fileIndex].type==streamtype_eSvc3DFull) {
                        set_hdmi_vsi();
                    }
                }
            }          
            else if (keypress == 8206) {  /* vol-: AVC */
                if (!g_state.avc_only) {
                    NEXUS_VideoFormat format;
                    g_state.videoFormatIndex = g_filelist[g_state.fileIndex].videoFormatAvcIndex;
                    g_state.avc_only = true;
                    format = get_nexus_video_format(0);
                    change_display_format(format, false);

                    change_decode_mode(g_state.avc_only, true);
                    restart_playback();

                    if (g_filelist[g_state.fileIndex].type==streamtype_eSvc3DFull) {
                        set_hdmi_vsi();
                    }
                }
            }
            else if (keypress == 20491) { /* chan+ */
                NEXUS_VideoFormat format;
                format = get_nexus_video_format(1);
                change_display_format(format, false);
            }
            else if (keypress == 16396) { /* chan- */
                NEXUS_VideoFormat format;
                format = get_nexus_video_format(-1);
                change_display_format(format, false);
            }
            else if (keypress == 41011) { /* info */
                g_state.text_on = !g_state.text_on;
                update_text();
            }

            /* if changing the playback file... */
            if (openfile!=-1) {
                /* note, changing the playback file preserves your decode mode (e.g. if in AVC, we stay in AVC) */
                streamtype prev, next;
                bool mode_switch, halfres3d_switch;
                NEXUS_VideoFormat curFormat, newFormat;
                BDBG_ASSERT(openfile < (int)NUM_STREAMS);

                prev = g_filelist[g_state.fileIndex].type;
                next = g_filelist[openfile].type;
                mode_switch = (prev!=streamtype_eMvc && next==streamtype_eMvc) || 
                              (prev==streamtype_eMvc && next!=streamtype_eMvc) ||
                              (prev!=streamtype_eSvc3DFull && next==streamtype_eSvc3DFull) || 
                              (prev==streamtype_eSvc3DFull && next!=streamtype_eSvc3DFull);
                halfres3d_switch = (prev!=streamtype_eSvc3DHalf && next==streamtype_eSvc3DHalf) || 
                                   (prev==streamtype_eSvc3DHalf && next!=streamtype_eSvc3DHalf) ||
                                   (next==streamtype_eSvc3DFull);
                curFormat = get_nexus_video_format(0);

                g_state.fileIndex = openfile;
                if (open_file(g_state.fileIndex)==0) {

                    /* a display format switch may be needed */
                    g_state.videoFormatIndex = g_state.avc_only ? g_filelist[g_state.fileIndex].videoFormatAvcIndex : g_filelist[g_state.fileIndex].videoFormatSMvcIndex;
                    newFormat = get_nexus_video_format(0);
                    if (curFormat != newFormat) {
                        change_display_format(newFormat, false);
                    }

                    /* a MVC->SVC or SVC->MVC switch requires a video decoder reconfiguration */
                    if (!g_state.avc_only && mode_switch) {
                        change_decode_mode(g_state.avc_only, true);
                    }

                    if (halfres3d_switch) {
                        set_hdmi_vsi();
                    }
                    restart_playback();
                    print_settings();
                }
                openfile = -1;
            }
            printf("Keypress = %d\n", keypress);
        }
    }
}

NEXUS_PlatformConfiguration platformConfig;
NEXUS_VideoDecoderHandle videoDecoder;
NEXUS_VideoDecoderStartSettings videoProgram;
NEXUS_PidChannelHandle videoPidChannel = NULL, enhancementVideoPidChannel = NULL;
NEXUS_PlaybackHandle playback;
NEXUS_PlaypumpHandle playpump;
NEXUS_FilePlayHandle fileplay = NULL;
NEXUS_DisplayHandle display;

const char *fontname =  "./arial_italic_35_aa.bwin_font";
#define VPID 0x31
#define EPID 0x32


bwin_engine_t win_engine;
bwin_framebuffer_t fb = NULL;
bwin_font_t font;
bwin_framebuffer_settings fb_settings;

NEXUS_Rect viewableRect;
#define DEFAULT_CANVAS_WIDTH 720
#define DEFAULT_CANVAS_HEIGHT 480
NEXUS_Rect offscreenRect = {0,0,DEFAULT_CANVAS_WIDTH,DEFAULT_CANVAS_HEIGHT};

NEXUS_Graphics2DHandle gfx;
NEXUS_SurfaceHandle framebuffer = NULL;
NEXUS_SurfaceHandle offscreen_surface = NULL;
NEXUS_Graphics2DFillSettings fillSettings;

unsigned get_framerate_hint(NEXUS_VideoFrameRate framerate)
{
    if (framerate==NEXUS_VideoFrameRate_e23_976) {
        return 24;
    }
    else if (framerate==NEXUS_VideoFrameRate_e60) {
        return 60;
    }
    else {
        return 0;
    }
}


/* returns appropriate video format based on current state (fileIndex, videoFormatIndex, avc_only)*/
NEXUS_VideoFormat get_nexus_video_format(int direction)
{
    NEXUS_VideoFormat videoFormat;

    if (direction>0) {
        if (g_state.videoFormatIndex>0) {
            g_state.videoFormatIndex--; /* up means increase video format, but our struct is in the opposite direction */
        }
    }
    else if (direction<0) {
        unsigned num_formats = NUM_SVC_FORMATS;
        if (g_filelist[g_state.fileIndex].type==streamtype_eMvc || g_filelist[g_state.fileIndex].type==streamtype_eSvc3DFull) {
            num_formats = NUM_MVC_FORMATS;
        }
        
        if (g_state.videoFormatIndex<num_formats-1) {
            g_state.videoFormatIndex++;
        }
    }

    switch (g_filelist[g_state.fileIndex].type) {
        case streamtype_eSvc:
        case streamtype_eSvc3DHalf:
            videoFormat = g_videoFormatsSvc[g_state.videoFormatIndex].format;
            break;
        case streamtype_eMvc:
        case streamtype_eSvc3DFull:
            if (!g_state.avc_only) {
                videoFormat = g_videoFormatsMvc[g_state.videoFormatIndex].format3d;
            }
            else  {
                videoFormat = g_videoFormatsMvc[g_state.videoFormatIndex].format;
            }
            break;
        default: 
            BDBG_ASSERT(0);           
    }
    
    return videoFormat;
}


void print_settings(void)
{
    NEXUS_VideoFormat videoFormat;
    videoFormat = get_nexus_video_format(0);

    printf("****************************************************************\n");
    printf("file: %s\n", g_filelist[g_state.fileIndex].name);
    printf("type: %d, mode: %s, output: %d (%d)\n", g_filelist[g_state.fileIndex].type, 
        g_state.avc_only?"AVC":"SVC/MVC", g_state.videoFormatIndex, videoFormat);
    printf("****************************************************************\n");

#if 0 /* sanity checks */
{
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Display_GetSettings(display, &displaySettings);
    BDBG_ASSERT(displaySettings.format==videoFormat);
        
    if (g_state.avc_only) {
        BDBG_ASSERT(videoProgram.codec==NEXUS_VideoCodec_eH264);
    }
    else {
        BDBG_ASSERT(videoProgram.codec!=NEXUS_VideoCodec_eH264);
    }
}
#endif
}

void update_text(void)
{
#if DISPLAY_TEXT
    NEXUS_VideoDecoderStatus videoDecoderStatus;
    NEXUS_VideoFormat videoFormat;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_Error rc;
    char msg1[32], msg2[32];
    unsigned y = 380;

    if (videoDecoder==NULL || framebuffer==NULL) {
        return;
    }
    
    if (!g_state.text_on || g_filelist[g_state.fileIndex].type == streamtype_eSvc3DHalf || 
        (g_filelist[g_state.fileIndex].type == streamtype_eSvc3DFull && g_state.avc_only)) {
        /* blit from offscreen to the framebuffer - this erases what's on the framebuffer */
        rc = NEXUS_Graphics2D_FastBlit(gfx, framebuffer, NULL, offscreen_surface, NULL, NEXUS_BlitColorOp_eCopySource, NEXUS_BlitAlphaOp_eCopySource, 0);
        BDBG_ASSERT(!rc);
        return;
    }

    videoFormat = get_nexus_video_format(0);
    NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoDecoderStatus);
    NEXUS_VideoFormat_GetInfo(videoFormat, &videoFormatInfo);

    sprintf(msg1, "Decode: %s %d%s%d", 
        g_state.avc_only?"AVC":(g_filelist[g_state.fileIndex].type==streamtype_eMvc?"MVC":"SVC"), 
        videoDecoderStatus.source.height==1088?1080:videoDecoderStatus.source.height,
        videoDecoderStatus.interlaced?"i":"p",
        get_framerate_hint(g_filelist[g_state.fileIndex].framerate));

    sprintf(msg2, "STB Output: %d%s%d", 
        videoFormatInfo.digitalHeight, 
        videoFormatInfo.interlaced?"i":"p",
        get_framerate_hint(g_filelist[g_state.fileIndex].framerate));

    /* get the toplevel window */
    bwin_get_framebuffer_settings(fb, &fb_settings);

    bwin_draw_text(fb_settings.window, font, viewableRect.x, y, msg1, -1, 0xAAFF0000, NULL);
    y += 40;
    bwin_draw_text(fb_settings.window, font, viewableRect.x, y, msg2, -1, 0xAAFF0000, NULL);

    /* must flush before blit, otherwise M2MC might copy from invalid memory */
    NEXUS_Surface_Flush(offscreen_surface);

    /* blit from offscreen to the framebuffer */
    rc = NEXUS_Graphics2D_FastBlit(gfx, framebuffer, NULL, offscreen_surface, NULL, NEXUS_BlitColorOp_eCopySource, NEXUS_BlitAlphaOp_eCopySource, 0);
    BDBG_ASSERT(!rc);

    /* clear offscreen surface */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = offscreen_surface;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);

#if 0
    BKNI_Sleep(100); /* poor man's checkpoint. enough time to get fill done. */
#endif
#endif
}

static void set_hdmi_vsi(void)
{
    NEXUS_HdmiOutputVendorSpecificInfoFrame vsi;
    NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(platformConfig.outputs.hdmi[0], &vsi);
    if (g_filelist[g_state.fileIndex].type == streamtype_eSvc3DHalf || (g_filelist[g_state.fileIndex].type == streamtype_eSvc3DFull && g_state.avc_only)) {
        vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
        vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eSidexSideHalf;
        printf("set_hdmi_vsi: SBS\n");
    }
    else {
        vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eNone;
        printf("set_hdmi_vsi: off\n");
    }
    NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(platformConfig.outputs.hdmi[0], &vsi);
}

static void source_changed(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);

    /* update text */
    printf("source_changed\n");
    update_text();
}

void change_display_format(NEXUS_VideoFormat format, bool init)
{
    NEXUS_DisplaySettings displaySettings;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_SurfaceMemory mem;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Error rc;

    NEXUS_Display_GetSettings(display, &displaySettings);
    if (displaySettings.format==format && !init) {
        return;
    }   

    printf("change_display_format: %d -> %d\n", displaySettings.format, format);
    displaySettings.format = format;
    NEXUS_Display_SetSettings(display, &displaySettings);
    NEXUS_VideoFormat_GetInfo(format, &videoFormatInfo);

#if DISPLAY_TEXT
    if (framebuffer) {
        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = false;
        NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        NEXUS_Surface_Destroy(framebuffer);
        NEXUS_Surface_Destroy(offscreen_surface);
        bwin_close_framebuffer(fb);
    }

    /* create framebuffer */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.width = DEFAULT_CANVAS_WIDTH;
    surfaceCreateSettings.height = DEFAULT_CANVAS_HEIGHT;
    surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);

    /* bwin will render into on offscreen image so we can increase font size by blitting */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.width = offscreenRect.width;
    surfaceCreateSettings.height = offscreenRect.height;
    surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    offscreen_surface = NEXUS_Surface_Create(&surfaceCreateSettings);
    NEXUS_Surface_GetMemory(offscreen_surface, &mem);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = offscreen_surface;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    bwin_framebuffer_settings_init(&fb_settings);
    fb_settings.width = surfaceCreateSettings.width;
    fb_settings.height = surfaceCreateSettings.height;
    fb_settings.pitch = mem.pitch;
    fb_settings.buffer = mem.buffer;
    fb_settings.pixel_format = bwin_pixel_format_a8_r8_g8_b8;
    fb = bwin_open_framebuffer(win_engine, &fb_settings);
    BDBG_ASSERT(fb);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.position.width = videoFormatInfo.width;
    graphicsSettings.position.height = videoFormatInfo.height;
    graphicsSettings.clip.width = DEFAULT_CANVAS_WIDTH;
    graphicsSettings.clip.height = DEFAULT_CANVAS_HEIGHT;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);
#endif

    update_text();
}

void restart_playback(void)
{
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playback_Stop(playback);

    NEXUS_Playback_Start(playback, fileplay, NULL);
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
}

int open_file(int index)
{
    if (index >= (int)NUM_STREAMS) {
        printf("No file mapped on index %d\n", index);
        return -1;
    }
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_VideoDecoder_Flush(videoDecoder);

    if (fileplay) {
        NEXUS_FilePlay_Close(fileplay);
        fileplay = NULL;
    }

    fileplay = NEXUS_FilePlay_OpenPosix(g_filelist[index].name, NULL);
    if (!fileplay) {
        fprintf(stderr, "Can't open file: %s\n", g_filelist[index].name);
        exit(-1);
    }
    return 0;
}

void open_video_pid_channels(bool avc_only)
{
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    if (videoPidChannel) {
        NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
        NEXUS_Playback_ClosePidChannel(playback, enhancementVideoPidChannel);
    }
    
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = avc_only ? NEXUS_VideoCodec_eH264 : 
        (g_filelist[g_state.fileIndex].type==streamtype_eMvc?NEXUS_VideoCodec_eH264_Mvc:NEXUS_VideoCodec_eH264_Svc);
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, VPID, &playbackPidSettings);
    enhancementVideoPidChannel = NEXUS_Playback_OpenPidChannel(playback, EPID, &playbackPidSettings);
}

/* called when changing between AVC/SVC/MVC modes */
void change_decode_mode(bool avc_only, bool switch_3d)
{
    NEXUS_VideoDecoderSettings settings;
    printf("change_decode_mode: %s\n", avc_only?"AVC":"SVC/MVC");

    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playback_Stop(playback);

    open_video_pid_channels(avc_only);

    videoProgram.codec = avc_only ? NEXUS_VideoCodec_eH264 : 
        (g_filelist[g_state.fileIndex].type==streamtype_eMvc?NEXUS_VideoCodec_eH264_Mvc:NEXUS_VideoCodec_eH264_Svc);
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.enhancementPidChannel = avc_only ? NULL : enhancementVideoPidChannel;
    videoProgram.frameRate = g_filelist[g_state.fileIndex].framerate;

    if (switch_3d) {
        if ((g_filelist[g_state.fileIndex].type==streamtype_eMvc || g_filelist[g_state.fileIndex].type==streamtype_eSvc3DFull) && !g_state.avc_only) {
            printf("change_decode_mode: 3D\n");
            NEXUS_VideoDecoder_GetSettings(videoDecoder, &settings);
            settings.customSourceOrientation = true;
            settings.sourceOrientation = NEXUS_VideoOrientation_e3D_Left;
            NEXUS_VideoDecoder_SetSettings(videoDecoder, &settings);
        }
        else {
            printf("change_decode_mode: 2D\n");
            NEXUS_VideoDecoder_GetSettings(videoDecoder, &settings);
            settings.customSourceOrientation = false;
            settings.sourceOrientation = NEXUS_VideoOrientation_e2D;
            NEXUS_VideoDecoder_SetSettings(videoDecoder, &settings);
        }

        {
            NEXUS_VideoDecoderExtendedSettings videoDecoderExtendedSettings;
            bool enable = (g_filelist[g_state.fileIndex].type==streamtype_eSvc3DFull) && !g_state.avc_only;
            NEXUS_VideoDecoder_GetExtendedSettings(videoDecoder, &videoDecoderExtendedSettings);
            if (videoDecoderExtendedSettings.svc3dEnabled != enable) {
                videoDecoderExtendedSettings.svc3dEnabled = enable;
                printf("change_decode_mode: SVC 3D: %d\n", enable);
                NEXUS_VideoDecoder_SetExtendedSettings(videoDecoder, &videoDecoderExtendedSettings);
            }
        }
    }
}

int main(int argc, const char *argv[])  {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoFormat videoFormat;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_IrInputSettings irSettings;
    
    bwin_engine_settings win_engine_settings;
    unsigned lineheight;
    #define DEFAULT_VIEWABLE_MARGIN 50
    unsigned viewableMargin = DEFAULT_VIEWABLE_MARGIN;
    viewableRect.x = viewableMargin;
    viewableRect.y = viewableMargin;
    viewableRect.width = offscreenRect.width - viewableMargin; /* no text wrap implemented, so go off to the right. */
    viewableRect.height = offscreenRect.height - 2*viewableMargin;
    
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    /* set initial state. this does not get reset on a AVC<->MVC/SVC transition */
    g_state.fileIndex = 0;
    g_state.avc_only = false;
    g_state.videoFormatIndex = 0;
    g_state.text_on = true;

    videoFormat = get_nexus_video_format(0);
    
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Open IR module */
    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = ir_callback;
    g_irHandle = NEXUS_IrInput_Open(0, &irSettings);

    /* Open playback */
    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = videoFormat;
    displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;
    displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo; /* fix it to HDMI master mode so we don't constantly have to change it */
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* bring up bwin with a single framebuffer */
    bwin_engine_settings_init(&win_engine_settings);
    win_engine = bwin_open_engine(&win_engine_settings);
    BDBG_ASSERT(win_engine);

    font = bwin_open_font(win_engine, fontname, -1, true);
    BDBG_ASSERT(font);
    bwin_get_font_height(font, &lineheight);

    /* set the framebuffer */
    change_display_format(videoFormat, true);

    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.fifoSize = 3 * 1024 * 1024;
    videoDecoderOpenSettings.svc3dSupported = true;
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.sourceChanged.callback = source_changed;
    NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);

    /* this opens the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.stcChannel = stcChannel;
    change_decode_mode(g_state.avc_only, false);

    /* Open file after decoder and playback are setup */
    open_file(g_state.fileIndex);
    restart_playback();

    while (1) {
        NEXUS_PlaybackStatus playbackStatus;
        NEXUS_VideoDecoderStatus videoStatus;
        NEXUS_Playback_GetStatus(playback, &playbackStatus);
        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
        if (playbackStatus.fifoSize) { /* fifoSize can be NULL due to a race condition with Playback_Stop and Playback_GetStatus */
            printf("pos %lu, last %lu, state %d, pfifo %d%%, vfifo %d%% pts %d, diff %d\n", playbackStatus.position, playbackStatus.last, playbackStatus.state,
                playbackStatus.fifoDepth/playbackStatus.fifoSize*100, videoStatus.fifoDepth/videoStatus.fifoSize*100,
                videoStatus.pts, videoStatus.ptsStcDifference);
        }

        BKNI_Sleep(1000);
    }

    return 0;
}
