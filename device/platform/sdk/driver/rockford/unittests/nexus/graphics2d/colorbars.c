/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: colorbars.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/9/09 5:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/colorbars.c $
 * 
 * Hydra_Software_Devel/2   10/9/09 5:33p erickson
 * SWDEPRECATED-2425: add HD/SD support
 *
 * Hydra_Software_Devel/1   9/25/09 5:14p erickson
 * SWDEPRECATED-2425: add SMPTE colorbar generator app
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#endif
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

/* SMPTE color bars */

#define NUM_COLORS 7
    uint32_t g_colors[NUM_COLORS] = {
        0xFFFFFFFF, /* white */
        0xFFFFFF00, /* yellow */
        0xFF00FFFF, /* cyan */
        0xFF00FF00, /* green */
        0xFFFF00FF, /* magenta */
        0xFFFF0000, /* red */
        0xFF0000FF  /* blue */
    };

int main(void)
{
    NEXUS_SurfaceHandle framebuffer0, framebuffer1;
    NEXUS_SurfaceCreateSettings hdCreateSettings, sdCreateSettings;
    NEXUS_DisplayHandle display0, display1;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    BKNI_EventHandle event;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo info;
    int i;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* HD display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display0 = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display0, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);
    NEXUS_Surface_GetDefaultCreateSettings(&hdCreateSettings);
    hdCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    hdCreateSettings.width = info.width;
    hdCreateSettings.height = info.height;
    framebuffer0 = NEXUS_Surface_Create(&hdCreateSettings);

    /* SD display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display1 = NEXUS_Display_Open(1, &displaySettings);
    NEXUS_Display_AddOutput(display1, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);
    NEXUS_Surface_GetDefaultCreateSettings(&sdCreateSettings);
    sdCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    sdCreateSettings.width = info.width;
    sdCreateSettings.height = info.height;
    framebuffer1 = NEXUS_Surface_Create(&sdCreateSettings);

    BKNI_CreateEvent(&event);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* draw for the HD display */
    for (i=0;i<NUM_COLORS;i++) {
        NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
        fillSettings.surface = framebuffer0;
        fillSettings.rect.width = hdCreateSettings.width/NUM_COLORS;
        fillSettings.rect.x = fillSettings.rect.width * i;
        fillSettings.rect.y = 0;
        fillSettings.rect.height = hdCreateSettings.height;
        fillSettings.color = g_colors[i];
        NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    }

    /* downscale for SD display */
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = framebuffer0;
    blitSettings.output.surface = framebuffer1;
    NEXUS_Graphics2D_Blit(gfx, &blitSettings);

    NEXUS_Display_GetGraphicsSettings(display0, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display0, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display0, framebuffer0);

    NEXUS_Display_GetGraphicsSettings(display1, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display1, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display1, framebuffer1);

    getchar();

    return 0;
}

