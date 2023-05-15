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
 * $brcm_Workfile: palette.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/3/12 1:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/surface/palette.c $
 * 
 * Hydra_Software_Devel/7   8/3/12 1:35p erickson
 * SWDEPRECATED-3044: palettes require 32 byte alignment, close display to
 * avoid GFD ARC error
 * 
 * Hydra_Software_Devel/6   8/2/12 3:19p erickson
 * SWDEPRECATED-3044: assert if surface can't be created
 * 
 * Hydra_Software_Devel/5   5/26/10 1:02p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/4   4/16/10 2:13p erickson
 * SWDEPRECATED-3044: fix warnings
 *
 * Hydra_Software_Devel/3   6/1/09 9:21a erickson
 * PR42679: fix for DTV
 *
 * Hydra_Software_Devel/2   1/30/09 4:25p erickson
 * PR50743: DTV fix
 *
 * Hydra_Software_Devel/1   12/30/08 9:16a erickson
 * PR50743: add palette test
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_memory.h"
#include "nexus_display.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BDBG_MODULE(palette);

int main(void)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_SurfaceMemory mem;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    unsigned i;
    unsigned count;
    unsigned loops = 100;
    NEXUS_Error rc;
    NEXUS_MemoryAllocationSettings allocSettings;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_ePalette8;
    createSettings.width = 1920;
    createSettings.height = 1080;

    rc = NEXUS_Memory_Allocate(createSettings.width * createSettings.height, NULL, &createSettings.pMemory);
    BDBG_ASSERT(!rc);

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.alignment = 32;
    rc = NEXUS_Memory_Allocate(sizeof(NEXUS_Pixel) * 256, &allocSettings, &createSettings.pPaletteMemory);
    BDBG_ASSERT(!rc);

    surface = NEXUS_Surface_Create(&createSettings);
    BDBG_ASSERT(surface);

    NEXUS_Surface_GetMemory(surface, &mem);
    BDBG_WRN(("buffer %p, expected %p", mem.buffer, createSettings.pMemory));
    BDBG_ASSERT(mem.buffer == createSettings.pMemory);
    BDBG_WRN(("palette %p, expected %p", mem.palette, createSettings.pPaletteMemory));
    BDBG_ASSERT(mem.palette == createSettings.pPaletteMemory);

    /* set the palette with a gradient of gray */
    for (i=0;i<mem.numPaletteEntries;i++) {
        mem.palette[i] = 0xff000000 | i<<16 | i<<8 | i;
    }
    BKNI_Memset(mem.buffer, 0, createSettings.height * mem.pitch);
    NEXUS_Surface_Flush(surface);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    /* graphicsSettings.position will default to the display size */
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Display_SetGraphicsFramebuffer(display, surface);
    BDBG_ASSERT(!rc);

    /* draw gradient */
    for (i=0;i<createSettings.height;i++) {
        BKNI_Memset((uint8_t*)mem.buffer + i*mem.pitch, i % 256, mem.pitch);
    }

    /* now change the palette */
    count = 0;
    i = 0;
    while (--loops) {
        mem.palette[i] = 0xff000000 | count;
        /* printf("palette[%d] = 0x%08x\n", i, mem.palette[i]); */
        NEXUS_Surface_Flush(surface);
        rc = NEXUS_Display_SetGraphicsFramebuffer(display, surface);
        BDBG_ASSERT(!rc);

        if (++count > 0xffffff) count = 0;
        if (++i > 255) i = 0;
    }

    NEXUS_Display_Close(display);
    NEXUS_Memory_Free(createSettings.pMemory);
    NEXUS_Memory_Free(createSettings.pPaletteMemory);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Platform_Uninit();
    printf("pass\n");
    return 0;
}

