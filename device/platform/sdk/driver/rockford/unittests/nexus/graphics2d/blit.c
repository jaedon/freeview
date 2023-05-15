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
 * $brcm_Workfile: blit.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 2/21/12 12:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/blit.c $
 * 
 * Hydra_Software_Devel/7   2/21/12 12:53p erickson
 * SWDEPRECATED-3044: fix checkpoint
 * 
 * Hydra_Software_Devel/6   8/27/10 2:09p randyjew
 * SW7468-143: Support for boards without component and/or composite.
 * 
 * Hydra_Software_Devel/5   3/19/09 10:35a erickson
 * PR42679: clarify
 *
 * Hydra_Software_Devel/4   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   9/25/08 12:16p jtna
 * PR46683: add test for NEXUS_Graphics2DBlitSettings.conversionMatrix
 *
 * Hydra_Software_Devel/2   9/18/08 11:53a erickson
 * PR47014: update
 *
 * Hydra_Software_Devel/1   9/17/08 10:32a erickson
 * PR47014: add blit test app
 *
 * 4   8/20/08 1:47p jgarrett
 * PR45866: added NEXUS_Graphics2DOpenSettings to NEXUS_Graphics2D_Open
 *
 * 3   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
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

NEXUS_Graphics2DHandle gfx;
BKNI_EventHandle event;

void callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx)
{
    NEXUS_Graphics2DSettings gfxSettings;
    int rc;
    
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.pollingCheckpoint = true;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);
    do {
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_BUSY) {
            BKNI_Sleep(1);
            continue;
        }
        BDBG_ASSERT(!rc);
        break;
    } while (1);
}

void test_pixel(NEXUS_SurfaceHandle surface, uint32_t expected_value)
{
    NEXUS_SurfaceMemory mem;

    NEXUS_Surface_Flush(surface);
    checkpoint(gfx);
    NEXUS_Surface_GetMemory(surface, &mem);
    printf("got %#x, expected %#x\n", *(uint32_t*)mem.buffer, expected_value);
    BDBG_ASSERT(*(uint32_t*)mem.buffer == expected_value);
#if 0
    getchar();
#endif
}

/* sample conversion matrix */
/* R = R * 0   + G * 0   + B * 0   + 128 */
/* G = R * 0   + G * 0.5 + B * 0   + 0 */
/* B = R * 0.5 + G * 0   + B * 0.5 + 0 */
/* A = A * 0.5 + 1 */
static const NEXUS_Graphics2DColorMatrix sampleMatrix = {
    10, /* shift */
{
    (int32_t) 0,                       /*  R factor for R */
    (int32_t) 0,                       /*  G factor for R */
    (int32_t) 0,                       /*  B factor for R */
    (int32_t) 0,                       /*  A factor for R */
    (int32_t) (128 * (1 << 10)),       /* Increment for R */
    (int32_t) 0,                       /*  R factor for G */
    (int32_t) (0.5f * (1 << 10)),      /*  G factor for G */
    (int32_t) 0,                       /*  B factor for G */
    (int32_t) 0,                       /*  A factor for G */
    (int32_t) 0,                       /* Increment for G */
    (int32_t) (0.5f * (1 << 10)),      /*  R factor for B */
    (int32_t) 0,                       /*  G factor for B */
    (int32_t) (0.5f * (1 << 10)),      /*  B factor for B */
    (int32_t) 0,                       /*  A factor for B */
    (int32_t) 0,                       /* Increment for B */
    (int32_t) 0,                       /*  R factor for A */
    (int32_t) 0,                       /*  G factor for A */
    (int32_t) 0,                       /*  B factor for A */
    (int32_t) (0.5f * (1 << 10)),      /*  A factor for A */
    (int32_t) (1 * (1 << 10)),         /* Increment for A */
}};

int main(void)
{
    NEXUS_SurfaceHandle surface, framebuffer;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    framebuffer = NEXUS_Surface_Create(&createSettings);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* fill framebuffer with black before showing */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = formatInfo.width; /* no GFD upscale */
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    surface = NEXUS_Surface_Create(&createSettings);

    /************************************/
    printf("test 1: copy red on green\n");
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0xFFFF0000;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.width = formatInfo.width/2; /* upper left quadrant */
    blitSettings.output.rect.height = formatInfo.height/2;
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
    test_pixel(framebuffer, 0xFFFF0000);

    /************************************/
    printf("test 2: blend red with green using source alpha, but leave output opaque \n");
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0x80FF0000;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.dest.surface = framebuffer;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.width = formatInfo.width/2; /* upper left quadrant */
    blitSettings.output.rect.height = formatInfo.height/2;
    blitSettings.constantColor = 0xFF000000;
    blitSettings.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
    test_pixel(framebuffer, 0xFF807f00);

    /************************************/
    printf("test 3: blend red with green using source alpha, copy source alpha\n");
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0x80FF0000;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.dest.surface = framebuffer;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.width = formatInfo.width/2; /* upper left quadrant */
    blitSettings.output.rect.height = formatInfo.height/2;
    blitSettings.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
    test_pixel(framebuffer, 0x80807f00);

    /************************************/
    printf("test 4: blend red with green using constant alpha\n");
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0xFFFF0000;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.dest.surface = framebuffer;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.width = formatInfo.width/2; /* upper left quadrant */
    blitSettings.output.rect.height = formatInfo.height/2;
    blitSettings.constantColor = 0x80000000;
    blitSettings.colorOp = NEXUS_BlitColorOp_eUseConstantAlpha;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
    test_pixel(framebuffer, 0xFF807f00);

    /************************************/
    printf("test 5: blend red with green using source alpha, copy source alpha, but do it with a blend equation\n");
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0x80FF0000;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.dest.surface = framebuffer;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.width = formatInfo.width/2; /* upper left quadrant */
    blitSettings.output.rect.height = formatInfo.height/2;
    blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
    blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;
    blitSettings.colorBlend.b = NEXUS_BlendFactor_eSourceAlpha;
    blitSettings.colorBlend.subtract_cd = false;
    blitSettings.colorBlend.c = NEXUS_BlendFactor_eDestinationColor;
    blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;
    blitSettings.colorBlend.subtract_e = false;
    blitSettings.colorBlend.e = NEXUS_BlendFactor_eZero;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;
    blitSettings.alphaBlend.a = NEXUS_BlendFactor_eSourceAlpha;
    blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;
    blitSettings.alphaBlend.subtract_cd = false;
    blitSettings.alphaBlend.c = NEXUS_BlendFactor_eZero;
    blitSettings.alphaBlend.d = NEXUS_BlendFactor_eZero;
    blitSettings.alphaBlend.subtract_e = false;
    blitSettings.alphaBlend.e = NEXUS_BlendFactor_eZero;
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
    test_pixel(framebuffer, 0x80807f00);

    /************************************/
    printf("test 6: blend red with green using constant alpha, but do it with a blend equation\n");
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0xFFFF0000;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.dest.surface = framebuffer;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.width = formatInfo.width/2; /* upper left quadrant */
    blitSettings.output.rect.height = formatInfo.height/2;
    blitSettings.constantColor = 0x80000000;
    blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
    blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;
    blitSettings.colorBlend.b = NEXUS_BlendFactor_eConstantAlpha;
    blitSettings.colorBlend.subtract_cd = false;
    blitSettings.colorBlend.c = NEXUS_BlendFactor_eDestinationColor;
    blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseConstantAlpha;
    blitSettings.colorBlend.subtract_e = false;
    blitSettings.colorBlend.e = NEXUS_BlendFactor_eZero;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;
    blitSettings.alphaBlend.a = NEXUS_BlendFactor_eSourceAlpha;
    blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;
    blitSettings.alphaBlend.subtract_cd = false;
    blitSettings.alphaBlend.c = NEXUS_BlendFactor_eZero;
    blitSettings.alphaBlend.d = NEXUS_BlendFactor_eZero;
    blitSettings.alphaBlend.subtract_e = false;
    blitSettings.alphaBlend.e = NEXUS_BlendFactor_eZero;
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
    test_pixel(framebuffer, 0xFF807f00);

    /************************************/
    printf("test 7: convert green to red/green blend using conversion matrix\n");
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.width = formatInfo.width/2; /* upper left quadrant */
    blitSettings.output.rect.height = formatInfo.height/2;
    blitSettings.conversionMatrix = sampleMatrix;
    blitSettings.conversionMatrixEnabled = true;
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
    test_pixel(framebuffer, 0x80807f00);

    printf("success\n");
    return 0;
}

