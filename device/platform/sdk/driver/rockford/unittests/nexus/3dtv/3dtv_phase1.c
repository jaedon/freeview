/***************************************************************************
*     (c)2004-2009 Broadcom Corporation
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
* $brcm_Workfile: 3dtv_phase1.c $
* $brcm_Revision: Hydra_Software_Devel/11 $
* $brcm_Date: 4/19/10 10:34a $
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/3dtv/3dtv_phase1.c $
* 
* Hydra_Software_Devel/11   4/19/10 10:34a erickson
* SW3548-2885: fix sbs/ou mode
*
* Hydra_Software_Devel/10   4/15/10 5:13p hongtaoz
* SW3548-2828: set display to 2160i48 format if input is 2160i48;
*
* Hydra_Software_Devel/9   4/8/10 6:04p hongtaoz
* SW3548-2727: removed nexus window close/open in source pending callback
* to be thread safe;
*
* Hydra_Software_Devel/8   3/19/10 3:14p hongtaoz
* SW3548-2727: added source pending handling according to source format
* dynamically for 3dtv sample app; added over/under half resolution
* display option;
*
* Hydra_Software_Devel/7   3/8/10 5:36p rgreen
* SW3548-2642: Update app  to loop until supported 3D format found
*
* Hydra_Software_Devel/6   1/29/10 12:43p erickson
* SW3548-2727: added graphics
*
* Hydra_Software_Devel/5   1/29/10 11:15a erickson
* SW3548-2727: add 720p (initial), combine interleaved and sbs apps, add
* sleep to increase reliability, add sourceClip api
*
* Hydra_Software_Devel/4   1/21/10 11:27a erickson
* SW3548-2727: add hddvi settings, set after input is connected
*
* Hydra_Software_Devel/3   1/20/10 10:32a erickson
* SW3548-2727: fix NEXUS_VideoWindow_AddInput call
*
* Hydra_Software_Devel/2   1/19/10 4:48p erickson
* SW3548-2727: modify hdmi input api
*
* Hydra_Software_Devel/1   1/19/10 2:33p erickson
* SW3548-2727: initial 3dtv phase1 app
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_panel_output.h"

#include "nexus_hdmi_input.h"

#include "nexus_video_hdmi_input.h"
#include "nexus_video_input.h"
#include "nexus_core_utils.h"
#include "nexus_graphics2d.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

BDBG_MODULE(3dtv_phase1);

struct b_3dtv_path
{
    NEXUS_VideoInput videoInput0, videoInput1;
    NEXUS_HdmiInputHandle hdmiInput0, hdmiInput1;
    NEXUS_VideoWindowHandle window0, window1;
    NEXUS_DisplayHandle display;
    bool bSbs;
    bool bOverUnder;

    NEXUS_Graphics2DHandle gfx;
    NEXUS_SurfaceHandle offscreenSurface, framebuffer, displaySurface;
};

void configure_3d_videos(struct b_3dtv_path *pContext);
void configure_3d_graphics(struct b_3dtv_path *pContext);

void source_pending(void *context, int param)
{
    struct b_3dtv_path *pContext = context;
    NEXUS_Error rc;

    BSTD_UNUSED(param);

    /* the application can also invoke PQ changes during the source pending callback */
    printf("source pending triggered...\n");

    /* add video handling */
    configure_3d_videos(pContext);

    /* add gfx handling */
    if(pContext->gfx)
        configure_3d_graphics(pContext);

    rc = NEXUS_VideoInput_SetResumeMode(pContext->videoInput0, NEXUS_VideoInputResumeMode_eNow);
    BDBG_ASSERT(!rc);
    printf("source resumed.\n");
}



int main(int argc, char **argv)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowSettings windowSettings;
    struct b_3dtv_path path = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, false, false, NULL,NULL,NULL,NULL};
    NEXUS_VideoInputSettings videoInputSettings;
    NEXUS_HdmiInputSettings hdmiInputSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;
    NEXUS_VideoFormat displayFormat = NEXUS_VideoFormat_eCustom0;

    if (argc > 1 && !strcmp(argv[1], "sbs")) {
        path.bSbs = true;
    }
    if (argc > 1 && !strcmp(argv[1], "ou")) {
        path.bOverUnder = true;
    }

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);

    if (!path.bSbs && !path.bOverUnder) {
        /* customize for our 3D panel */
        platformSettings.displayModuleSettings.panel.dvoLinkMode = NEXUS_PanelOutputLinkMode_e3DPanel;
    }

    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
    NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
    hdmiInputSettings.timebase = NEXUS_Timebase_e0;
    path.hdmiInput0 = NEXUS_HdmiInput_Open(0, &hdmiInputSettings);
    path.videoInput0 = NEXUS_HdmiInput_GetVideoConnector(path.hdmiInput0);

    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
    hdmiInputSettings.timebase = NEXUS_Timebase_e0;
    hdmiInputSettings.masterHdmiInput = path.hdmiInput0;
    path.hdmiInput1 = NEXUS_HdmiInput_Open(1, &hdmiInputSettings);
    path.videoInput1 = NEXUS_HdmiInput_GetVideoConnector(path.hdmiInput1);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = displayFormat;
    display = NEXUS_Display_Open(0, &displaySettings);
    path.display = display;
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_ASSERT(!rc);
    fprintf(stderr, "Panel output ready\n");

    path.window0 = NEXUS_VideoWindow_Open(display, 0);
    path.window1 = NEXUS_VideoWindow_Open(display, 1);

    rc = NEXUS_VideoWindow_AddInput(path.window0, path.videoInput0);
    BDBG_ASSERT(!rc);

    NEXUS_VideoWindow_GetSettings(path.window1, &windowSettings);
    windowSettings.position.x = 0;
    windowSettings.position.y = 0;
    windowSettings.position.width = 100;
    windowSettings.position.height = 100;
    windowSettings.visible = false;
    rc = NEXUS_VideoWindow_SetSettings(path.window1, &windowSettings);
    rc = NEXUS_VideoWindow_AddInput(path.window1, path.videoInput1);
    BDBG_ASSERT(!rc);

    /* enable app-driven source pending */
    rc = NEXUS_VideoInput_SetResumeMode(path.videoInput0, NEXUS_VideoInputResumeMode_eManual);
    BDBG_ASSERT(!rc);

    NEXUS_VideoInput_GetSettings(path.videoInput0, &videoInputSettings);
    videoInputSettings.sourcePending.callback = source_pending;
    videoInputSettings.sourcePending.context = &path;
    rc = NEXUS_VideoInput_SetSettings(path.videoInput0, &videoInputSettings);
    BDBG_ASSERT(!rc);

    printf("Press ENTER to display 3D graphics\n");
    getchar();

    /**
    Graphics
    **/
    {
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;

    NEXUS_Display_GetSettings(path.display, &displaySettings);
    path.gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.width = 720;
    surfaceCreateSettings.height = 480;
    path.offscreenSurface = NEXUS_Surface_Create(&surfaceCreateSettings);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = path.offscreenSurface;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(path.gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = path.offscreenSurface;
    fillSettings.rect.x = 100;
    fillSettings.rect.y = 100;
    fillSettings.rect.width = 200;
    fillSettings.rect.height = 200;
    fillSettings.color = 0xff00ff00; /* green box */
    rc = NEXUS_Graphics2D_Fill(path.gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    configure_3d_graphics(&path);
    }

    getchar();

    return 0;
}

void configure_3d_videos(struct b_3dtv_path *pContext)
{
    NEXUS_VideoInputStatus status;
    NEXUS_VideoHdmiInputSettings videoHdmiInputSettings;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo displayFormatInfo;
    NEXUS_Error rc;

    NEXUS_VideoInput_GetStatus(pContext->videoInput0, &status);
    BDBG_WRN(("3DTV source %dx%d", status.width, status.height));

    if (status.height == 2205) {

        BKNI_Sleep(100);
        /* 1080p24 3D */

        NEXUS_VideoHdmiInput_GetSettings(pContext->videoInput1, &videoHdmiInputSettings);
        videoHdmiInputSettings.autoFormat = false;
        videoHdmiInputSettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;
        videoHdmiInputSettings.autoPosition = false;
        videoHdmiInputSettings.position.x = 192; /* hstart */
        videoHdmiInputSettings.position.y = 1023; /* vstart */
        videoHdmiInputSettings.deEnabled = true;
        videoHdmiInputSettings.formatTolerance.width = 5;
        videoHdmiInputSettings.formatTolerance.height = 3;
        rc = NEXUS_VideoHdmiInput_SetSettings(pContext->videoInput1, &videoHdmiInputSettings);
        BDBG_ASSERT(!rc);

        /* require 2nd window path */

        if(pContext->bSbs) {/* side-by-side half resolution */
            NEXUS_Display_GetSettings(pContext->display, &displaySettings);
            displaySettings.format = NEXUS_VideoFormat_eCustom0;
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            NEXUS_Display_SetSettings(pContext->display, &displaySettings);

            /* show two windows side-by-side */
            NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width/2;
            windowSettings.position.height = displayFormatInfo.height;
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 0;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
            windowSettings.position.x = displayFormatInfo.width/2;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width/2;
            windowSettings.position.height = displayFormatInfo.height;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
            BDBG_ASSERT(!rc);
        }
        else if(pContext->bOverUnder) {/* over under half */
            NEXUS_Display_GetSettings(pContext->display, &displaySettings);
            displaySettings.format = NEXUS_VideoFormat_eCustom0;
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            NEXUS_Display_SetSettings(pContext->display, &displaySettings);

            /* show two windows over-under */
            NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height/2;
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 0;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
            windowSettings.position.x = 0;
            windowSettings.position.y = displayFormatInfo.height/2;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height/2;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
            BDBG_ASSERT(!rc);
        }
        else /* 2160i48 full resolution */
        {
            NEXUS_Display_GetSettings(pContext->display, &displaySettings);
            displaySettings.format = NEXUS_VideoFormat_eCustom1920x2160i_48Hz;
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            NEXUS_Display_SetSettings(pContext->display, &displaySettings);
            NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
            BDBG_ASSERT(!rc);
        }
        NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
        windowSettings.sourceClip.left = 0;
        windowSettings.sourceClip.right = 0;
        windowSettings.sourceClip.top = 0;
        windowSettings.sourceClip.bottom = 0;
        rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
        BDBG_ASSERT(!rc);
        NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
        windowSettings.sourceClip.left = 0;
        windowSettings.sourceClip.right = 0;
        windowSettings.sourceClip.top = 143;
        windowSettings.sourceClip.bottom = 0;
        windowSettings.visible = true;
        rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
        BDBG_ASSERT(!rc);
    }
    else if (status.height == 1470) {
        if(pContext->bSbs) /* side-by-side half */
        {
            NEXUS_VideoFormat_GetInfo(status.format, &displayFormatInfo);
            BKNI_Printf("+++++ HDMI0 format = %dx%d%s@%d/100 Hz\n", displayFormatInfo.width, displayFormatInfo.height,
                displayFormatInfo.interlaced?"i":"p", status.refreshRate);
            NEXUS_Display_GetSettings(pContext->display, &displaySettings);
            displaySettings.format = (status.refreshRate==50000)? NEXUS_VideoFormat_eCustom1 : NEXUS_VideoFormat_eCustom0;
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            NEXUS_Display_SetSettings(pContext->display, &displaySettings);

            /* require 2nd window path */
            /* 720p 3D */
            NEXUS_VideoHdmiInput_GetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            videoHdmiInputSettings.autoFormat = true;
            videoHdmiInputSettings.autoPosition = false;
            videoHdmiInputSettings.deEnabled = false;
            videoHdmiInputSettings.formatTolerance.width = 5;
            videoHdmiInputSettings.formatTolerance.height = 3;
            rc = NEXUS_VideoHdmiInput_SetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 750;
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width/2;
            windowSettings.position.height = displayFormatInfo.height;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 750;
            windowSettings.sourceClip.bottom = 0;
            windowSettings.position.x = displayFormatInfo.width/2;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width/2;
            windowSettings.position.height = displayFormatInfo.height;
            windowSettings.visible = true;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
            BDBG_ASSERT(!rc);
        }
        else if (pContext->bOverUnder)/* over/under half */
        {
            /* display format -> 1080p over/under w/o active space */
            NEXUS_VideoFormat_GetInfo(status.format, &displayFormatInfo);
            BKNI_Printf("+++++ HDMI0 format = %dx%d%s@%d/100 Hz\n", displayFormatInfo.width, displayFormatInfo.height,
                displayFormatInfo.interlaced?"i":"p", status.refreshRate);
            NEXUS_Display_GetSettings(pContext->display, &displaySettings);
            displaySettings.format = (status.refreshRate==50000)? NEXUS_VideoFormat_eCustom1 : NEXUS_VideoFormat_eCustom0;
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            NEXUS_Display_SetSettings(pContext->display, &displaySettings);

            /* require 2nd window path */

            NEXUS_VideoHdmiInput_GetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            videoHdmiInputSettings.autoFormat = true;
            videoHdmiInputSettings.autoPosition = false;
            videoHdmiInputSettings.deEnabled = false;
            videoHdmiInputSettings.formatTolerance.width = 5;
            videoHdmiInputSettings.formatTolerance.height = 3;
            rc = NEXUS_VideoHdmiInput_SetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 750;
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height/2;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 750;
            windowSettings.sourceClip.bottom = 0;
            windowSettings.position.x = 0;
            windowSettings.position.y = displayFormatInfo.height/2;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height/2;
            windowSettings.visible = true;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
            BDBG_ASSERT(!rc);
        }
        else /* over/under 720p 3D w active space pass-thru -> x425 */
        {
            /* display format -> 720p_3D pass-thru */
            NEXUS_Display_GetSettings(pContext->display, &displaySettings);
            displaySettings.format = status.format;
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            BKNI_Printf("+++++ (a) HDMI0 format = %dx%d%c@%d/100 Hz\n", displayFormatInfo.width, displayFormatInfo.height,
                displayFormatInfo.interlaced?'i':'p', displayFormatInfo.verticalFreq);
            NEXUS_Display_SetSettings(pContext->display, &displaySettings);

            NEXUS_VideoHdmiInput_GetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            videoHdmiInputSettings.autoFormat = true;
            videoHdmiInputSettings.autoPosition = false;
            videoHdmiInputSettings.deEnabled = false;
            videoHdmiInputSettings.formatTolerance.width = 5;
            videoHdmiInputSettings.formatTolerance.height = 3;
            rc = NEXUS_VideoHdmiInput_SetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 0;
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
            BDBG_ASSERT(!rc);

            /* shrink and hide the win1 */
            NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 0;
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = 100;
            windowSettings.position.height = 100;
            windowSettings.visible = false;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
        }
   }
    else /* non-3D format -> main full-screen display 1080p */
    {
            NEXUS_Display_GetSettings(pContext->display, &displaySettings);
            displaySettings.format = status.format;
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            BKNI_Printf("+++++ (b) HDMI0 format = %dx%d%c@%d/100 Hz\n", displayFormatInfo.width, displayFormatInfo.height,
                displayFormatInfo.interlaced?'i':'p', displayFormatInfo.verticalFreq);
            displaySettings.format = (status.format == NEXUS_VideoFormat_eCustom1920x2160i_48Hz)? NEXUS_VideoFormat_eCustom1920x2160i_48Hz:
               ((status.refreshRate==50000)? NEXUS_VideoFormat_eCustom1 : NEXUS_VideoFormat_eCustom0);
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
            NEXUS_Display_SetSettings(pContext->display, &displaySettings);

            NEXUS_VideoHdmiInput_GetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            videoHdmiInputSettings.autoFormat = true;
            videoHdmiInputSettings.autoPosition = false;
            videoHdmiInputSettings.deEnabled = false;
            videoHdmiInputSettings.formatTolerance.width = 5;
            videoHdmiInputSettings.formatTolerance.height = 3;
            rc = NEXUS_VideoHdmiInput_SetSettings(pContext->videoInput1, &videoHdmiInputSettings);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(pContext->window0, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 0;
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = displayFormatInfo.width;
            windowSettings.position.height = displayFormatInfo.height;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window0, &windowSettings);
            BDBG_ASSERT(!rc);

            /* shrink and hide the win1 */
            NEXUS_VideoWindow_GetSettings(pContext->window1, &windowSettings);
            windowSettings.sourceClip.left = 0;
            windowSettings.sourceClip.right = 0;
            windowSettings.sourceClip.top = 0;
            windowSettings.sourceClip.bottom = 0;
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = 100;
            windowSettings.position.height = 100;
            windowSettings.visible = false;
            rc = NEXUS_VideoWindow_SetSettings(pContext->window1, &windowSettings);
    }
}

void configure_3d_graphics(struct b_3dtv_path *pContext)
{
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_SurfaceMemory surMemory;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo displayFormatInfo;
    NEXUS_Error rc;

    NEXUS_Display_GetSettings(pContext->display, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
#define MAX_GRAPHICS_FB_WIDTH 1366
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    if (displaySettings.format == NEXUS_VideoFormat_eCustom1920x2160i_48Hz) {
        surfaceCreateSettings.width = MAX_GRAPHICS_FB_WIDTH*2;
        surfaceCreateSettings.height = displayFormatInfo.height/2;
    }
    else {
        surfaceCreateSettings.width = MAX_GRAPHICS_FB_WIDTH;
        surfaceCreateSettings.height = displayFormatInfo.height;
    }
    if(pContext->framebuffer)
    {
        /* disable gfx first */
        NEXUS_Display_GetGraphicsSettings(pContext->display, &graphicsSettings);
        graphicsSettings.enabled = false;
        rc = NEXUS_Display_SetGraphicsSettings(pContext->display, &graphicsSettings);
        BDBG_ASSERT(!rc);

        /* then destroy surfaces */
        NEXUS_Surface_Destroy(pContext->framebuffer);
        pContext->framebuffer = NULL;

        if(pContext->displaySurface)
        {
            NEXUS_Surface_Destroy(pContext->displaySurface);
            pContext->displaySurface = NULL;
        }
    }
    pContext->framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);

    /* blit from offscreen to framebuffer with offset for 3D effect */
    if (displaySettings.format == NEXUS_VideoFormat_eCustom1920x2160i_48Hz) {
        /* TODO: apply znorm offset */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH;
        blitSettings.output.rect.height = surfaceCreateSettings.height;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = MAX_GRAPHICS_FB_WIDTH;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH;
        blitSettings.output.rect.height = surfaceCreateSettings.height;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        BKNI_Printf("Done with 2160i gfx blit...\n");
    }
    else if(pContext->bSbs) {/* side-by-side half resolution */
        /* TODO: apply znorm offset */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH/2;
        blitSettings.output.rect.height = surfaceCreateSettings.height;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = MAX_GRAPHICS_FB_WIDTH/2;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH/2;
        blitSettings.output.rect.height = surfaceCreateSettings.height;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        BKNI_Printf("Done with sbs gfx blit...\n");
    }
    else if(pContext->bOverUnder) {/* over/under half resolution LG */
        /* TODO: apply znorm offset */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH/2;
        blitSettings.output.rect.height = surfaceCreateSettings.height;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = MAX_GRAPHICS_FB_WIDTH/2;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH/2;
        blitSettings.output.rect.height = surfaceCreateSettings.height;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        BKNI_Printf("Done with over/under gfx blit...\n");
    }
    else if(displayFormatInfo.height == 1470) {/* 720p3D  over/under frame packed 1470p */
        /* TODO: apply znorm offset */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH;
        blitSettings.output.rect.height = 720;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 750;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH;
        blitSettings.output.rect.height = 720;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        BKNI_Printf("Done with 720p3D gfx 2D blit...\n");
    }
    else /* 2D gfx */
    {
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = pContext->offscreenSurface;
        blitSettings.output.surface = pContext->framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = MAX_GRAPHICS_FB_WIDTH;
        blitSettings.output.rect.height = displayFormatInfo.height;
        rc = NEXUS_Graphics2D_Blit(pContext->gfx, &blitSettings);
        BDBG_ASSERT(!rc);
    }

    NEXUS_Display_GetGraphicsSettings(pContext->display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = MAX_GRAPHICS_FB_WIDTH;
    graphicsSettings.clip.height = displayFormatInfo.height;
    rc = NEXUS_Display_SetGraphicsSettings(pContext->display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    surfaceCreateSettings.width = MAX_GRAPHICS_FB_WIDTH;
    surfaceCreateSettings.height = displayFormatInfo.height;
    NEXUS_Surface_GetMemory(pContext->framebuffer, &surMemory);
    surfaceCreateSettings.pMemory = surMemory.buffer;
    pContext->displaySurface = NEXUS_Surface_Create(&surfaceCreateSettings);
    rc = NEXUS_Display_SetGraphicsFramebuffer(pContext->display, pContext->displaySurface);
    BDBG_ASSERT(!rc);
}

