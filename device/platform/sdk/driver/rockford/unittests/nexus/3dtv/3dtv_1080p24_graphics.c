/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: 3dtv_1080p24_graphics.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/24/10 12:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/3dtv/3dtv_1080p24_graphics.c $
 * 
 * Hydra_Software_Devel/2   2/24/10 12:54p erickson
 * SW3548-2727: dtv support
 *
 * Hydra_Software_Devel/1   2/24/10 12:43p erickson
 * SW3548-2727: add 1080p24hz 3DTV graphics example
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_display.h"
#include "nexus_graphics2d.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_hdmi_output.h"
#include "nexus_component_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <string.h>

/**
3DTV 1080p24hz graphics uses a 3840x1080 surface where is left half is left eye, right half is right eye.

The M2MC (blitter) will access this 3540x1080 surface as left and right halves.
However, in 1080p24Hz 3DTV mode the GFD will need to perform an interlaced scanout of the same surface memory as a 1920x2160 surface.
In order to do this, we create a second surface which points to the first surface's memory.

Also, this technique can be tested on non-3DTV capable hardware by rendering into a 3840x540 surface,
then scanning out as 1920x1080. See #define TRUE_3DTV.
**/

#define TRUE_3DTV 0

int main(void)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo displayFormatInfo;
    NEXUS_SurfaceHandle framebuffer, interlacedFramebuffer;
    NEXUS_SurfaceCreateSettings createSurfaceSettings, createInterlacedSurfaceSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DHandle gfx;
#if NEXUS_DTV_PLATFORM
#else
    NEXUS_HdmiOutputHandle hdmiOutput;
#endif
    NEXUS_Error rc;
    NEXUS_SurfaceMemory mem;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if TRUE_3DTV
    displaySettings.format = NEXUS_VideoFormat_eCustom1920x2160i_48Hz;
#else
    displaySettings.format = NEXUS_VideoFormat_eCustom0; /* 1080p test mode */
#endif
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_ASSERT(!rc);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if TRUE_3DTV
    displaySettings.format = NEXUS_VideoFormat_e3D_1080p_24Hz;
#else
    displaySettings.format = NEXUS_VideoFormat_e1080i; /* 1080i test mode */
#endif
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
    hdmiOutput = platformConfig.outputs.hdmi[0];
    if (hdmiOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(hdmiOutput));
        BDBG_ASSERT(!rc);
    }
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);

    /* bring up graphics */
    NEXUS_Surface_GetDefaultCreateSettings(&createSurfaceSettings);
    createSurfaceSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
#if TRUE_3DTV
    createSurfaceSettings.width = 3840;
    createSurfaceSettings.height = 1080;
#else
    createSurfaceSettings.width = 3840;
    createSurfaceSettings.height = 540;
#endif
    framebuffer = NEXUS_Surface_Create(&createSurfaceSettings);
    NEXUS_Surface_GetMemory(framebuffer, &mem);

    NEXUS_Surface_GetDefaultCreateSettings(&createInterlacedSurfaceSettings);
    createSurfaceSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createInterlacedSurfaceSettings.width = createSurfaceSettings.width / 2;
    createInterlacedSurfaceSettings.height = createSurfaceSettings.height * 2;
    createInterlacedSurfaceSettings.pMemory = mem.buffer;
    interlacedFramebuffer = NEXUS_Surface_Create(&createInterlacedSurfaceSettings);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* fill left with black */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.rect.x = 0;
    fillSettings.rect.y = 0;
    fillSettings.rect.width = createSurfaceSettings.width/2;
    fillSettings.rect.height = createSurfaceSettings.height;
    fillSettings.color = 0xFF000000;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);

    /* fill right with white */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.rect.x = createSurfaceSettings.width/2;
    fillSettings.rect.y = 0;
    fillSettings.rect.width = createSurfaceSettings.width/2;
    fillSettings.rect.height = createSurfaceSettings.height;
    fillSettings.color = 0xFFFFFFFF;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = displayFormatInfo.width;
    graphicsSettings.clip.height = displayFormatInfo.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    /* use the interlacedFramebuffer, not framebuffer */
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, interlacedFramebuffer);
    BDBG_ASSERT(!rc);

    /* you should see gray, not black or white, with an interference pattern (which varies depending on the TV). */

    getchar();

    /* another way to test - start and exit the app multiple times. every time you quit, only one field will be displayed.
    sometimes it will be solid white. sometimes solid black. never gray. */

    return 0;
}
