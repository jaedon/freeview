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
 * $brcm_Workfile: compression_app.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 1/30/09 4:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/surface/compression_app.c $
 * 
 * Hydra_Software_Devel/4   1/30/09 4:26p erickson
 * PR46300: removed NEXUS_Surface_CreateSpecial
 *
 * Hydra_Software_Devel/3   1/13/09 6:09p erickson
 * PR46300: moved compression code to mainline as special feature 1
 *
 * Hydra_Software_Devel/2   12/4/08 9:54a erickson
 * PR46302: fix compression setting, test shutdown
 *
 * Hydra_Software_Devel/1   11/7/08 9:51a erickson
 * PR46300: added graphics compression example
 *
 * 10   10/17/08 5:40p jtna
 * PR46852: removed sleep
 *
 * 9   9/12/08 12:16p erickson
 * PR46852: add hdmi output with temp sleep
 *
 * 8   7/18/08 11:31a jgarrett
 * PR 44953: Removing -Wstrict-prototypes warnings
 *
 * 7   7/7/08 2:40p erickson
 * PR44619: remove duplicate NEXUS_PanelOutput_Open calls.
 *
 * 6   6/19/08 2:36p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 5   6/19/08 10:46a erickson
 * PR42678: switch to output_type=panel|component
 *
 * 4   5/21/08 11:29a erickson
 * PR42678: fix graphics for Custom0 format
 *
 * 3   5/21/08 10:33a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 2   4/1/08 11:39a erickson
 * PR38442: use is_panel_output() for consistent DVO output selection
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_display.h"
#include "nexus_graphics2d.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nexus_platform_boardcfg.h"

int main(void)
{
    NEXUS_SurfaceHandle surface, framebuffer;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_SurfaceMemory mem;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
    NEXUS_VideoFormatInfo formatInfo;
    int i;
    unsigned loops = 10000;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);

    /* create uncompressed surface for CPU write */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    /* create compressed frame buffer used by GFD */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    createSettings.bitsPerPixel = 14;
    framebuffer = NEXUS_Surface_Create(&createSettings);

    /* fill with black */
    BKNI_Memset(mem.buffer, 0, createSettings.height * mem.pitch);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* draw random dots */
    while (loops--) {
        unsigned x = rand() % createSettings.width;
        unsigned y = rand() % createSettings.height;
        NEXUS_Pixel *buf = (NEXUS_Pixel *)((uint8_t *)mem.buffer+y*mem.pitch);
        buf[x] = rand();
        if (i%1000==0) {
            NEXUS_Surface_Flush(surface);

            /* blitting from uncompressed into compressed buffer will trigger GRC/M2MC compression */
            rc = NEXUS_Graphics2D_FastBlit(gfx, framebuffer, NULL, surface, NULL,
                NEXUS_BlitColorOp_eCopySource, NEXUS_BlitAlphaOp_eCopySource, 0);
            BDBG_ASSERT(!rc);

            printf(".");
            fflush(stdout);

            /* a sleep is a simple way to avoid over scheduling the blitter */
            BKNI_Sleep(100);
        }
    }

    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Platform_Uninit();

    return 0;
}

