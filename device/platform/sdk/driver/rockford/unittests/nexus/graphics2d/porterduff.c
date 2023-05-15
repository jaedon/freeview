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
 * $brcm_Workfile: porterduff.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/10/10 6:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/porterduff.c $
 * 
 * Hydra_Software_Devel/5   5/10/10 6:56p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/4   10/23/08 3:59p erickson
 * PR42679: update
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

const char *pd_str[] = {
    "Clear",
    "Src",
    "Dst",
    "SrcOver",
    "DstOver",
    "SrcIn",
    "DstIn",
    "SrcOut",
    "DstOut",
    "SrcAtop",
    "DstAtop",
    "Xor"
};

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(void)
{
    NEXUS_SurfaceHandle srcSurface, destSurface, framebuffer;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle event;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DPorterDuffBlitSettings porterDuffBlit;
    NEXUS_Error rc;
    NEXUS_SurfaceMemory mem;
    unsigned x, y;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_ASSERT(!rc);
#else
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);

    BKNI_CreateEvent(&event);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* create black framebuffer */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    framebuffer = NEXUS_Surface_Create(&createSettings);
    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF000000; /* black */
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    createSettings.width = 100;
    createSettings.height = 100;
    srcSurface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(srcSurface, &mem);
    for (y=0;y<createSettings.height;y++) {
        for (x=0;x<createSettings.width;x++) {
            ((uint32_t*)mem.buffer)[y * mem.pitch/4 + x] = (x < (createSettings.height-y)) ? 0xFFFF0000 : 0;
        }
    }
    NEXUS_Surface_Flush(srcSurface);

    createSettings.width = 100;
    createSettings.height = 100;
    destSurface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(destSurface, &mem);
    for (y=0;y<createSettings.height;y++) {
        for (x=0;x<createSettings.width;x++) {
            ((uint32_t*)mem.buffer)[y * mem.pitch/4 + x] = (x > y) ? 0xFF00FF00 : 0;
        }
    }
    NEXUS_Surface_Flush(destSurface);

    NEXUS_Graphics2D_GetDefaultPorterDuffBlitSettings(&porterDuffBlit);
    porterDuffBlit.sourceSurface = srcSurface;
    porterDuffBlit.destSurface = destSurface;
    porterDuffBlit.outSurface = framebuffer;
    porterDuffBlit.outRect.x = 50;
    porterDuffBlit.outRect.y = 50;
    porterDuffBlit.outRect.width = 100;
    porterDuffBlit.outRect.height = 100;
    for (porterDuffBlit.operation = NEXUS_PorterDuffOp_eClear; porterDuffBlit.operation < NEXUS_PorterDuffOp_eMax; 
		  porterDuffBlit.operation = (NEXUS_PorterDuffOp)(porterDuffBlit.operation+1)) {
        rc = NEXUS_Graphics2D_PorterDuffBlit(gfx, &porterDuffBlit);
        assert(!rc);

        printf("%s operation\n", pd_str[porterDuffBlit.operation]);
        BKNI_Sleep(2000);

        /* erase framebuffer */
        fillSettings.surface = framebuffer;
        fillSettings.color = 0xFF000000; /* black */
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    }

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = false;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Surface_Destroy(srcSurface);
    NEXUS_Surface_Destroy(destSurface);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Display_RemoveAllOutputs(display);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}
