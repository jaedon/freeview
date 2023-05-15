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
 * $brcm_Workfile: palette_blit2.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/palette_blit2.c $
 * 
 * Hydra_Software_Devel/7   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* Nexus example app: show graphics framebuffer with CPU drawing */

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

int main(void)
{
    NEXUS_Error rc;
    NEXUS_SurfaceHandle framebuffer, surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned i;
    unsigned loops = 10;

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
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif

#endif
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);

    while (--loops) {
        /* create the ARGB framebuffer */
        NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
        createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        createSettings.width = formatInfo.width;
        createSettings.height = formatInfo.height;
        #if BCHP_CHIP == 7420
         /* Specify heap1 as nexus defaults to heap0 */
         createSettings.heap = platformConfig.heap[1];
        #endif
        framebuffer = NEXUS_Surface_Create(&createSettings);

        /* create the palettized surface */
        createSettings.pixelFormat = NEXUS_PixelFormat_ePalette8;
        createSettings.width = formatInfo.width;
        createSettings.height = formatInfo.height;
        surface = NEXUS_Surface_Create(&createSettings);
        NEXUS_Surface_GetMemory(surface, &mem);
        /* set the palette with a gradient */
        for (i=0;i<mem.numPaletteEntries;i++) {
            switch (i/(256/3)) {
            case 0:  mem.palette[i] = 0xff000000 | i<<16; break;
            case 1:  mem.palette[i] = 0xff000000 | i<<8; break;
            default: mem.palette[i] = 0xff000000 | i; break;
            }
        }
        /* draw the palette surface */
        for (i=0;i<createSettings.height;i++) {
            BKNI_Memset((uint8_t*)mem.buffer + i*mem.pitch, i, mem.pitch);
        }
        NEXUS_Surface_Flush(surface);

        gfx = NEXUS_Graphics2D_Open(0, NULL);
        BDBG_ASSERT(gfx);

        /* blit from the palette surface to the ARGB framebuffer */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = surface;
        blitSettings.output.surface = framebuffer;
        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);

        /* show the framebuffer */
        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = true;
        rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(rand() % 5000);

        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = false;
        rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        BDBG_ASSERT(!rc);
        NEXUS_Graphics2D_Close(gfx);
        NEXUS_Surface_Destroy(surface);
        NEXUS_Surface_Destroy(framebuffer);
    }

    NEXUS_Display_RemoveAllOutputs(display);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}

