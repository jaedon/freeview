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
 * $brcm_Workfile: blit_performance.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 7/23/12 4:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/blit_performance.c $
 * 
 * Hydra_Software_Devel/6   7/23/12 4:39p jessem
 * SW7425-3568: Added BKNI_DestroyEvent(event).
 *
 * Hydra_Software_Devel/5   12/13/11 11:33a erickson
 * SW7231-536: clean shutdown, remove old code
 *
 * Hydra_Software_Devel/4   8/31/10 8:59a randyjew
 * SW7468-143: Support for boards without component and/or composite.
 *
 * Hydra_Software_Devel/3   8/30/10 9:44a erickson
 * SWDEPRECATED-3044: fix use of NEXUS_Graphics2D_Checkpoint
 *
 * Hydra_Software_Devel/2   3/19/09 10:35a erickson
 * PR42679: clarify
 *
 * Hydra_Software_Devel/1   12/3/08 5:05p erickson
 * PR42679: add test
 *
 * 7   11/21/08 8:01a erickson
 * PR42679: remove unneeded NEXUS_Surface_Flush
 *
 * 6   10/31/08 12:18p erickson
 * PR42679: add panel support
 *
 * 5   10/23/08 3:24p jtna
 * PR48190: use openSettings to specify max number of async operations
 *
 * 4   8/20/08 1:47p jgarrett
 * PR45866: added NEXUS_Graphics2DOpenSettings to NEXUS_Graphics2D_Open
 *
 * 3   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 *****************************************************************************/
/* Nexus example app: show graphics framebuffer with CPU drawing */

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

BDBG_MODULE(blit_performance);

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(int argc, char **argv)
{
    NEXUS_SurfaceHandle surface0, surface1;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Graphics2DHandle gfx;
    BKNI_EventHandle event;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DOpenSettings openSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_VideoFormatInfo info;
    unsigned i;
    unsigned loops = 1000;
    unsigned bpp = 32;

    if (argc > 1) bpp = atoi(argv[1]);
    if (argc > 2) loops = atoi(argv[2]);

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    switch (bpp) {
    default:
    case 32:
        createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        break;
    case 16:
        createSettings.pixelFormat = NEXUS_PixelFormat_eR5_G6_B5;
        break;
    case 8:
        createSettings.pixelFormat = NEXUS_PixelFormat_ePalette8;
        break;
    }
    createSettings.width = info.width;
    createSettings.height = info.height;
    surface0 = NEXUS_Surface_Create(&createSettings);
    surface1 = NEXUS_Surface_Create(&createSettings);

    BKNI_CreateEvent(&event);

    NEXUS_Graphics2D_GetDefaultOpenSettings(&openSettings);
    openSettings.maxOperations = 500;
    gfx = NEXUS_Graphics2D_Open(0, &openSettings);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    BDBG_WRN(("start %d blits with %d bpp surfaces (blits are not visible, only measuring speed)", loops, bpp));
    for (i=0;i<loops;i++) {
        NEXUS_Error rc;
        NEXUS_Graphics2DBlitSettings blitSettings;

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = surface0;
        blitSettings.output.surface = surface1;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        if (rc || i+1==loops) {
            rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                BKNI_WaitForEvent(event, 0xffffffff);
            }
        }
    }
    BDBG_WRN(("end"));

    NEXUS_Graphics2D_Close(gfx);
	BKNI_DestroyEvent(event);
	NEXUS_Surface_Destroy(surface0);
    NEXUS_Surface_Destroy(surface1);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}

