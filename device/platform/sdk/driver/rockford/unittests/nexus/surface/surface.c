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
 * $brcm_Workfile: surface.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/7/10 6:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/surface/surface.c $
 * 
 * Hydra_Software_Devel/5   5/7/10 6:38p mward
 * SW7125-253:  Allow compilation with C++, eliminate "invalid conversion"
 * errors.
 * 
 * Hydra_Software_Devel/4   2/12/10 1:30p mward
 * SW3548-2778: remove unused NEXUS_PixelFormat
 * 
 * Hydra_Software_Devel/3   2/4/10 2:47p mward
 * SW7420-515: remove NEXUS_PixelFormat_eA0 and
 * NEXUS_PixelFormat_eVideoTunnel.
 * 
 * Hydra_Software_Devel/2   4/17/09 9:30a erickson
 * PR54304: extend test
 *
 * Hydra_Software_Devel/1   10/31/08 10:32a jtna
 * PR42679: added new unittest for Surface module
 *
 *****************************************************************************/
/* automated unittest for Nexus surface module */

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "nexus_memory.h"
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

void block(void)
{
    NEXUS_CallbackDesc desc;
    BERR_Code rc;
    desc.callback = callback;
    desc.context = event;
    NEXUS_Graphics2D_Checkpoint(gfx, &desc);
    rc = BKNI_WaitForEvent(event, 0x3E8*3); /* wait upto 3 seconds. if there's no callback, it's a failure */
    BDBG_ASSERT(!rc);
}

void test_pixel(NEXUS_SurfaceHandle surface, uint32_t color)
{
    NEXUS_SurfaceMemory mem;
    NEXUS_Surface_Flush(surface);
    block();
    NEXUS_Surface_GetMemory(surface, &mem);
    printf("\tgot %#x for color %#x\n", *(uint32_t*)mem.buffer, color);
#if 0
    getchar();
#endif
}

/* Returns the bits per pixel for given pixel format. There's a Nexus function to do this properly, but it's a priv function. Use this for now. */
unsigned get_bpp(NEXUS_PixelFormat format)
{
    switch (format) {
        case NEXUS_PixelFormat_eR5_G6_B5:         /* 16-bit, no per-pixel alpha */
        case NEXUS_PixelFormat_eA1_R5_G5_B5:      /* 16-bit */
        case NEXUS_PixelFormat_eR5_G5_B5_A1:      /* 16-bit */
        case NEXUS_PixelFormat_eA4_R4_G4_B4:      /* 16-bit */
        case NEXUS_PixelFormat_eR4_G4_B4_A4:      /* 16-bit */
            return 16;
        case NEXUS_PixelFormat_eA8_R8_G8_B8:      /* 32-bit */
        case NEXUS_PixelFormat_eR8_G8_B8_A8:      /* 32-bit */
            return 32;

        case NEXUS_PixelFormat_ePalette8:         /* 8-bit Palette index */
            return 8;
        case NEXUS_PixelFormat_eA8_Palette8:      /* A8 and 8-bit Palette index */
            return 16;

        case NEXUS_PixelFormat_eA8:               /* 8-bit alpha-only surface (constant color) */
            return 8;

        case NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
        case NEXUS_PixelFormat_eY08_Cr8_Y18_Cb8:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
        case NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
        case NEXUS_PixelFormat_eY18_Cr8_Y08_Cb8:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
        case NEXUS_PixelFormat_eCb8_Y08_Cr8_Y18:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
        case NEXUS_PixelFormat_eCb8_Y18_Cr8_Y08:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
        case NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
        case NEXUS_PixelFormat_eCr8_Y08_Cb8_Y18:  /* 32-bit for 2 pixels: YCbCr422 w/ 8-bit values */
            return 16;
        case NEXUS_PixelFormat_eX2_Cr10_Y10_Cb10: /* 32-bit for 1 pixel:  YCbCr444 w/ 10-bit values */
        case NEXUS_PixelFormat_eA8_Y8_Cb8_Cr8: /* YCbCr444 w/ 8-bit values */
        case NEXUS_PixelFormat_eCr8_Cb8_Y8_A8: /* YCbCr444 w/ 8-bit values */
        case NEXUS_PixelFormat_eY8_Cb8_Cr8_A8: /* YCbCr444 w/ 8-bit values */
        case NEXUS_PixelFormat_eA8_Cr8_Cb8_Y8: /* YCbCr444 w/ 8-bit values */
        case NEXUS_PixelFormat_eYCbCr422_10bit:   /* YCbCr422 w/ 10-bit values */
            return 32;
        case NEXUS_PixelFormat_ePalette2:         /* 2-bit palette index */
            return 2;
        case NEXUS_PixelFormat_ePalette4:         /* 4-bit palette index */
            return 4;
        default:
            BDBG_ASSERT(0);
    }
    return 0;
}


int main(void)
{
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceSettings surfaceSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    void *pMem = NULL;
    NEXUS_Error rc;
    unsigned i, j;

    /* Bring up all modules for a platform in a default configuration for this platform */
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
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* go through all pixel formats */
    for (i = NEXUS_PixelFormat_eUnknown; i < NEXUS_PixelFormat_eMax; i++) {

        /* unsupported formats */
        if (i==NEXUS_PixelFormat_eUnknown) {
            continue;
        }
        /* TODO: these formats get stuck in NEXUS_Graphics2D_Checkpoint() after 2D_Fill */
        if (i==NEXUS_PixelFormat_eX2_Cr10_Y10_Cb10 ||
            i==NEXUS_PixelFormat_eYCbCr422_10bit) {
            continue;
        }

        printf("testing pixel format %d...(%s)\n", i,
            NEXUS_PIXEL_FORMAT_IS_PALETTE(i) ? "palette" : (NEXUS_PIXEL_FORMAT_IS_YCRCB(i) ? "YCrCb" : "RGB") );

        /* Test cases:
           j=0: manual mem alloc with NEXUS_Memory_Allocate(). all asserts should pass.
           j=1: manual mem alloc with NEXUS_Memory_Allocate() with more mem allocated than necessary. all asserts should pass.
           j=2: auto mem alloc. all asserts should pass.
           j=3: auto mem alloc. request an unreasonably large amount of memory. this should fail gracefully.
           j=4: auto mem alloc, with more mem allocated than necessary (pitch > width*sizeof(pixel). all asserts should pass.
        */
        for (j = 0; j < 5; j++) {
            printf("\ttest case %d...\n", j);
            if (j<2) { /* manual mem alloc */
                unsigned numBytes = formatInfo.height*formatInfo.width*get_bpp((NEXUS_PixelFormat)i)/8;
                if (j==1) {
                    numBytes *= 2;
                }
                rc = NEXUS_Memory_Allocate(numBytes, NULL, &pMem);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(pMem);
            }
            else {
                BDBG_ASSERT(!pMem);
            }

            NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
            createSettings.pixelFormat = (NEXUS_PixelFormat)i;
            createSettings.width = formatInfo.width;
            createSettings.height = formatInfo.height;
            if (j==3) {
                printf("expecting failure...\n");
                /* request something too large */
                createSettings.width = 0xFFFF;
                createSettings.height = 0xFFFF;
            }
            if (j==4) {
                createSettings.pitch = createSettings.width * (get_bpp((NEXUS_PixelFormat)i)/8) * 2;
            }
            createSettings.alignment = rand() % 17; /* for manual mem alloc, this should do nothing */
            createSettings.pMemory = pMem;

            /* create the surface */
            framebuffer = NEXUS_Surface_Create(&createSettings);
            if (j==3) {
                BDBG_ASSERT(!framebuffer);
                continue;
            }
            BDBG_ASSERT(framebuffer);

            /* get surface memory and check some parameters */
            NEXUS_Surface_GetMemory(framebuffer, &mem);
            BDBG_ASSERT(mem.pitch>=createSettings.width*get_bpp((NEXUS_PixelFormat)i)/8);
            BDBG_ASSERT(mem.buffer);
            if (j<2) {
                BDBG_ASSERT(mem.buffer == pMem);
            }
            else {
                BDBG_ASSERT((unsigned)mem.buffer % (unsigned)(1<<createSettings.alignment) == 0);
            }

            NEXUS_Surface_GetSettings(framebuffer, &surfaceSettings);
            surfaceSettings.autoFlush = true;
            NEXUS_Surface_SetSettings(framebuffer, &surfaceSettings);

            /* fill surface with blue */
            if (NEXUS_PIXEL_FORMAT_IS_PALETTE(i)) {
                BDBG_ASSERT(mem.palette);
                if (i==NEXUS_PixelFormat_eA8_Palette8) {
                    BDBG_ASSERT(mem.numPaletteEntries == (unsigned)(1 << (get_bpp((NEXUS_PixelFormat)i)/2)));
                }
                else {
                    BDBG_ASSERT(mem.numPaletteEntries == (unsigned)(1 << get_bpp((NEXUS_PixelFormat)i)));
                }
                mem.palette[0] = 0xFF0000FF; /* RGB blue */
                BKNI_Memset(mem.buffer, 0, createSettings.height * mem.pitch);
                NEXUS_Surface_Flush(framebuffer);
            }
            else {
                BDBG_ASSERT(!mem.palette);
                BDBG_ASSERT(!mem.numPaletteEntries);

                NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
                fillSettings.surface = framebuffer;
                if (!NEXUS_PIXEL_FORMAT_IS_YCRCB(i)) {
                    fillSettings.color = 0xFF0000FF; /* RGB blue */
                }
                else {
                    fillSettings.color = 0xFF29F06E; /* YCrCb blue */
                }
                rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
                BDBG_ASSERT(!rc);
            }

            NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
            graphicsSettings.enabled = true;
            graphicsSettings.clip.width = formatInfo.width; /* no GFD upscale */
            rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
            BDBG_ASSERT(!rc);
            rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
            BDBG_ASSERT(!rc);

            test_pixel(framebuffer, fillSettings.color);

            NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
            graphicsSettings.enabled = false;
            rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
            BDBG_ASSERT(!rc);

            if (pMem) {
                NEXUS_Memory_Free(pMem);
                pMem=NULL;
            }
            NEXUS_Surface_Destroy(framebuffer);
        }
    }

    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Display_RemoveAllOutputs(display);
    NEXUS_Display_Close(display);

    printf("test completed\n");
    return 0;
}

