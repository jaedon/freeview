/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_surface.c $
 * $brcm_Revision: 60 $
 * $brcm_Date: 10/17/12 5:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface/src/nexus_surface.c $
 * 
 * 60   10/17/12 5:14p erickson
 * SW7435-429: add proc
 * 
 * 59   8/9/12 11:29a erickson
 * SW7425-3627: remove previous change
 * 
 * 57   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 56   8/1/12 1:37p erickson
 * SW7425-3616: do not memset(0) any palette. application must initialize.
 *  this allows apps to allocate palettes from any heap, and results in
 *  better gfx performance.
 * 
 * 55   7/9/12 1:58p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 54   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 53   1/5/12 12:28p erickson
 * SW7435-30: add NEXUS_Surface_InitPlaneAndPaletteOffset
 * 
 * 52   12/13/11 4:27p erickson
 * SW7420-2129: get current default heap using NEXUS_P_DefaultHeap
 * 
 * 51   10/7/11 1:49p vsilyaev
 * SW7420-2085: Added support for object reference counting
 * 
 * 50   8/26/11 12:43p erickson
 * SW7420-1575: NEXUS_Surface_CreateFromMagnum_priv must set surface-
 *  >offset
 * 
 * 49   5/23/11 1:34p erickson
 * SW7420-1200: add NEXUS_Surface_InitPlane_priv
 * 
 * 48   5/19/11 12:42p erickson
 * SW7422-444: back out last checkin
 * 
 * 47   5/13/11 2:55p erickson
 * SW7422-444: add check for fake address to NEXUS_Surface_GetMemory
 * 
 * 46   3/8/11 5:11p erickson
 * SW7420-1575: add NEXUS_Surface_InitPlane for packet blit verification
 *
 * 45   2/9/11 11:34a vsilyaev
 * SW7420-1441:Fixed typo
 *
 * 44   2/7/11 2:03p vsilyaev
 * SW7420-1441: Fixed calculations for the size of the surface buffer
 *
 * 43   2/3/11 6:37p vsilyaev
 * SW7420-1441: Use local (non proxied) implementation for function that
 *  flush CPU cache
 *
 * 42   12/7/10 5:44p erickson
 * SW7420-1148: remove unnecessary memset
 *
 * 41   10/22/10 2:47p erickson
 * SW7420-1205: deprecate NEXUS_SurfaceSettings.autoFlush because of
 *  inherent performance problems
 *
 * 40   8/24/10 4:26p erickson
 * SW3548-681: downgrade ERR to WRN. this is a legal call in some cases.
 *
 * 39   8/6/10 1:08p erickson
 * SW7420-703: NEXUS_Surface_GetCreateSettings
 *
 * 38   7/14/10 12:48p erickson
 * SW7405-3787: fix alignment units and calc
 *
 * 37   6/23/10 12:14p erickson
 * SW7550-453: add nexus_striped_surface.h interface
 *
 * 36   4/21/10 9:27a erickson
 * SW7420-703: add NEXUS_SurfaceModuleSettings.heapIndex for chips where
 *  graphics cannot be allocated in MEMC0
 *
 * 35   1/18/10 3:14p erickson
 * SW7405-3787: default nexus surface allocations to 4K alignment to avoid
 *  stale data in the RAC. also, alignment must apply to both the frontend
 *  (i.e. the alignment) and the backend (i.e. padding the allocation)
 *
 * 34   11/10/09 1:52p nickh
 * SW7420-166: Remove hard coded heap settings for 7420 and pass them in
 *  from Brutus/settop layer
 *
 * 33   8/21/09 5:14p vsilyaev
 * PR 57469: Added  trace statements to allow capture exact flow of 2D
 *  operations
 *
 * 32   8/12/09 11:21a vsilyaev
 * PR 57629: Replaced NEXUS_Surface_AutoFlush_priv with
 *  NEXUS_Surface_GetSurfaceAutoFlush_priv
 *
 * 31   7/29/09 4:13p nickh
 * PR56017: Add 7420B0 support
 *
 * 30   7/13/09 8:16a erickson
 * PR56673: only memset palette memory if created by nexus
 *
 * 29   5/1/09 11:07a jgarrett
 * PR 54512: Adding prediction mode
 *
 * 28   3/31/09 10:05a erickson
 * PR53661: remove NEXUS_SurfaceSettings.constantColor. it was unused. the
 *  intended functionality can be accomplished using the constantColor in
 *  NEXUS_Graphics2DBlitSettings.
 *
 * 27   3/24/09 9:41a erickson
 * PR47583: fix NEXUS_Surface_CreateFromMagnum_priv because of surface
 *  member additions.
 *
 * 26   2/25/09 7:23p nickh
 * PR52525: Implement SW workaround for 7420 3D graphics demos
 *
 * 25   1/30/09 4:26p erickson
 * PR46300: removed NEXUS_Surface_CreateSpecial
 *
 * 24   1/26/09 11:29a erickson
 * PR51468: global variable naming convention
 *
 * 23   1/13/09 6:11p erickson
 * PR46300: added NEXUS_Surface_CreateSpecial
 *
 * 22   12/30/08 9:20a erickson
 * PR50743: implement NEXUS_SurfaceCreateSettings.pPaletteMemory. return
 *  cached address for palette and implement palette flush.
 *
 * 21   12/23/08 5:09p nickh
 * PR48963: Create surfaces from MEMC1 heap for 7420
 *
 * 20   12/17/08 11:59p erickson
 * PR50438: NEXUS_Surface_Destroy should handle case where no surface-
 *  >buffer exists
 *
 * 19   11/26/08 9:59a erickson
 * PR49649: added NEXUS_SurfaceCreateSettings.heap option
 *
 * 18   11/7/08 9:43a erickson
 * PR46300: moved NEXUS_Surface to module.h to allow for extension
 *
 * 17   10/31/08 10:16a jtna
 * PR48586: initialize surface->mem.numPaletteEntries = 0 for non-palette
 *  pixel formats
 *
 * 16   10/1/08 4:48p erickson
 * PR47303: BSUR_Surface_Create api change
 *
 * 15   9/25/08 10:57a erickson
 * PR47313: fix assignment of height
 *
 * 14   9/18/08 4:17p erickson
 * PR47111: free memory on error case
 *
 * 13   8/28/08 10:42a erickson
 * PR46111: add palettePixelFormat
 *
 * 12   8/21/08 5:14p erickson
 * PR45941: added NEXUS_StripedSurface_GetStatus
 *
 * 11   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 10   5/5/08 2:54p erickson
 * PR40777: handle interlaced stills using M2MC, not software
 *
 * 9   3/31/08 1:41p erickson
 * PR41077: added NEXUS_ASSERT_MODULE to _priv function
 *
 * 8   3/26/08 12:51p erickson
 * PR40567: use Core conversion functions for pixel format
 *
 * 7   3/19/08 12:32p erickson
 * PR40307: change NEXUS_Surface_ProcessAsField_priv factor to 4 until
 *  M2MC chroma upsample can be fixed
 *
 * 6   3/14/08 10:10a erickson
 * PR40307: 40316
 *
 * 5   2/20/08 9:51a erickson
 * PR39771: check return code
 *
 * 4   2/8/08 11:40a vsilyaev
 * PR 38396: Fixed warning
 *
 * 3   1/29/08 3:36p jgarrett
 * PR 38396: Fixing externally allocated surface memory
 *
 * 3   1/29/08 3:33p jgarrett
 * PR 38396: Fixing externally allocated surface memory
 *
 * 2   1/28/08 1:00p vsilyaev
 * PR 38682: Initialize palette address to NULL
 *
 * 1   1/18/08 2:19p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#include "nexus_surface_module.h"
#include "priv/nexus_core.h"
#include "priv/nexus_core_video.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_surface);

#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

/* global instances */
NEXUS_ModuleHandle g_NEXUS_surfaceModule;
static struct NEXUS_SurfaceModuleData {
    NEXUS_SurfaceModuleSettings settings;
    struct {
        unsigned count;
        unsigned memory;
    } stats;
} g_NEXUS_SurfaceModuleData;


void NEXUS_SurfaceModule_GetDefaultSettings(NEXUS_SurfaceModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

static void NEXUS_SurfaceModule_P_Print(void)
{
    BDBG_LOG(("SurfaceModule: %d surfaces, %d bytes", g_NEXUS_SurfaceModuleData.stats.count, g_NEXUS_SurfaceModuleData.stats.memory));
}

NEXUS_ModuleHandle NEXUS_SurfaceModule_Init(const NEXUS_SurfaceModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;

    BDBG_ASSERT(!g_NEXUS_surfaceModule);

    BKNI_Memset(&g_NEXUS_SurfaceModuleData, 0, sizeof(g_NEXUS_SurfaceModuleData));
    if (pSettings) {
        g_NEXUS_SurfaceModuleData.settings = *pSettings;
    }
    else {
        NEXUS_SurfaceModule_GetDefaultSettings(&g_NEXUS_SurfaceModuleData.settings);
    }

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eHigh; /* surface interface is fast */
    moduleSettings.dbgPrint = NEXUS_SurfaceModule_P_Print;
    moduleSettings.dbgModules = "nexus_surface";
    g_NEXUS_surfaceModule = NEXUS_Module_Create("surface", &moduleSettings);
    if (!g_NEXUS_surfaceModule) {
        return NULL;
    }
    return g_NEXUS_surfaceModule;
}

void NEXUS_SurfaceModule_Uninit(void)
{
    NEXUS_Module_Destroy(g_NEXUS_surfaceModule);
    g_NEXUS_surfaceModule = NULL;
}

void NEXUS_Surface_GetDefaultCreateSettings(NEXUS_SurfaceCreateSettings *pCreateSettings)
{
    BKNI_Memset(pCreateSettings, 0, sizeof(*pCreateSettings));
    pCreateSettings->pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    pCreateSettings->palettePixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    pCreateSettings->width = 720;
    pCreateSettings->height = 480;
    pCreateSettings->alignment = 12; /* 2^12 = 4K alignment needed for RAC. see nexus_dma.h for detailed description of cache coherency requirements. */
}

static void NEXUS_Surface_P_Finalizer(NEXUS_SurfaceHandle surface)
{
    BERR_Code rc;
    NEXUS_OBJECT_ASSERT(NEXUS_Surface, surface);

    BDBG_MSG(("NEXUS_Surface_P_Destroy:%#x", (unsigned)surface));
    
    NEXUS_CLIENT_RESOURCES_RELEASE(surface,Count,NEXUS_ANY_ID);
    
    /* Destroy must safely allocs because this could be called during midway during a failed Create */
    if (surface->palette) {
        rc=BSUR_Palette_Destroy(surface->palette);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    }
    if (surface->surface) {
        rc=BSUR_Surface_Destroy(surface->surface);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    }
    if (surface->buffer && NULL == surface->createSettings.pMemory) {
        BMEM_Free(surface->heap, surface->buffer);
        g_NEXUS_SurfaceModuleData.stats.memory -= surface->mem.bufferSize;
    }
    g_NEXUS_SurfaceModuleData.stats.count--;
    NEXUS_OBJECT_DESTROY(NEXUS_Surface, surface);
    BKNI_Free(surface);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Surface, NEXUS_Surface_Destroy);

NEXUS_SurfaceHandle NEXUS_Surface_Create(const NEXUS_SurfaceCreateSettings *pCreateSettings)
{
    BERR_Code rc = BERR_SUCCESS;
    BPXL_Format pixel_format;
    NEXUS_SurfaceHandle surface;
    void *mem_uncached;
    unsigned min_pitch;
    NEXUS_SurfaceCreateSettings defaultSettings;
    BSUR_Surface_Settings surfaceSettings;

    if(!pCreateSettings) {
        NEXUS_Surface_GetDefaultCreateSettings(&defaultSettings);
        pCreateSettings = &defaultSettings;
    }
    BDBG_MSG(("NEXUS_Surface_Create %ux%u, pixel: %u",
        pCreateSettings->width, pCreateSettings->height, pCreateSettings->pixelFormat));

    if (NEXUS_P_PixelFormat_ToMagnum(pCreateSettings->pixelFormat, &pixel_format)) {
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }

    rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(surface,Count,NEXUS_ANY_ID);
    if (rc) { rc = BERR_TRACE(rc); return NULL; }
        
    surface = BKNI_Malloc(sizeof(*surface));
    if (!surface) {
        NEXUS_CLIENT_RESOURCES_RELEASE(surface,Count,NEXUS_ANY_ID); 
        rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); 
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_Surface, surface);
    surface->mem.pitch = pCreateSettings->pitch;
    surface->createSettings = *pCreateSettings;
    g_NEXUS_SurfaceModuleData.stats.count++;

    /* if NULL, get the registered default heap. modify the stored createSettings for GetCreateSettings. */
    surface->createSettings.heap = NEXUS_P_DefaultHeap(pCreateSettings->heap, NEXUS_DefaultHeapType_eAny);
    /* retain support for module-level default index, but this should not be used. */
    if (!surface->createSettings.heap && g_NEXUS_SurfaceModuleData.settings.heapIndex < NEXUS_MAX_HEAPS) {
        surface->createSettings.heap = g_pCoreHandles->nexusHeap[g_NEXUS_SurfaceModuleData.settings.heapIndex];
    }
    if (!surface->createSettings.heap) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto error;
    }
    surface->heap = NEXUS_Heap_GetMemHandle(surface->createSettings.heap);

    rc = BPXL_GetBytesPerNPixels(pixel_format, pCreateSettings->width, &min_pitch);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    if (!surface->mem.pitch) {
        surface->mem.pitch = min_pitch;
    }
    else if (surface->mem.pitch < min_pitch) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        BDBG_ERR(("specified pitch is too small"));
        goto error;
    }

    if ( pCreateSettings->pMemory )
    {
        uint32_t offset;
        offset = NEXUS_AddrToOffset(pCreateSettings->pMemory);
        if ( 0 == offset )
        {
            BDBG_ERR(("Invalid pMemory address %p specified", pCreateSettings->pMemory));
            goto error;
        }
        surface->buffer = NEXUS_OffsetToUncachedAddr(offset);
        BDBG_ASSERT(surface->buffer);
    }

    if (BPXL_IS_PALETTE_FORMAT(pixel_format)) {
        BPXL_Format palette_pixel_format;
        void *paletteMemory = NULL; /* default to magnum-allocated */

        if (pCreateSettings->pPaletteMemory) {
            uint32_t offset;
            offset = NEXUS_AddrToOffset(pCreateSettings->pPaletteMemory);
            if ( 0 == offset )
            {
                BDBG_ERR(("Invalid pPaletteMemory address %p specified", pCreateSettings->pPaletteMemory));
                goto error;
            }
            paletteMemory = NEXUS_OffsetToUncachedAddr(offset);
            BDBG_ASSERT(paletteMemory);
        }

        if (NEXUS_P_PixelFormat_ToMagnum(pCreateSettings->palettePixelFormat, &palette_pixel_format)) {
            rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
            goto error;
        }
        surface->mem.numPaletteEntries = BPXL_NUM_PALETTE_ENTRIES(pixel_format);
        rc = BSUR_Palette_Create(surface->heap,
                     surface->mem.numPaletteEntries,
                     paletteMemory,
                     palette_pixel_format,
                     BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED,
                     &surface->palette);
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

        /* get user address for palette */
        rc = BSUR_Palette_GetAddress(surface->palette, &paletteMemory);
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

        rc = BMEM_Heap_ConvertAddressToCached(surface->heap, paletteMemory, &paletteMemory);
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

        surface->mem.palette = paletteMemory;

        surface->paletteOffset = NEXUS_AddrToOffset(surface->mem.palette);
    }

    /* We always alloc the memory, not SUR. This allows us to manipulate as needed. */
    if (NULL == surface->buffer) {
        int sz = pCreateSettings->height * surface->mem.pitch;
        unsigned alignment_in_bytes;

        /* alignment applies to the backend of the buffer too. */
        alignment_in_bytes = 1 << pCreateSettings->alignment;
        if (alignment_in_bytes && (sz % alignment_in_bytes)) {
            sz += alignment_in_bytes - (sz % alignment_in_bytes);
        }
        surface->buffer = BMEM_AllocAligned(surface->heap, sz, pCreateSettings->alignment, 0);
        if (!surface->buffer) {
            BDBG_ERR(("unable to allocate surface size=%d alignment=2^%d", sz, pCreateSettings->alignment));
            goto error;
        }
        BDBG_MSG(("allocated surface mem=%p size=%d alignment=2^%d", surface->buffer, sz, pCreateSettings->alignment));
        g_NEXUS_SurfaceModuleData.stats.memory += sz;
    }
    surface->offset = NEXUS_AddrToOffset(surface->buffer);

    BSUR_Surface_GetDefaultSettings( &surfaceSettings );
    if (pCreateSettings->bitsPerPixel) {
        surfaceSettings.stTestFeature1Settings.bEnable = true;
        surfaceSettings.stTestFeature1Settings.ulBitsPerPixel = pCreateSettings->bitsPerPixel;
        surfaceSettings.stTestFeature1Settings.ulPredictionMode = 1; /* NOLEFT (1) is expected for all current usage modes due to the framebuffer size */
    }
    else {
        surfaceSettings.stTestFeature1Settings.bEnable = false;
        surfaceSettings.stTestFeature1Settings.ulBitsPerPixel = 0;
    }
    surface->mem.bufferSize = pCreateSettings->height*surface->mem.pitch;

    rc = BSUR_Surface_Create(surface->heap,
                 pCreateSettings->width, pCreateSettings->height,
                 surface->mem.pitch,
                 surface->buffer, /* never NULL */
                 pixel_format,
                 surface->palette,
                 BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED,
                 &surfaceSettings,
                 &surface->surface);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    /* For surfaces allocated by Magnum, convert to cached and use that address. */
    rc = BSUR_Surface_GetAddress(surface->surface, &mem_uncached, (uint32_t *)&surface->mem.pitch);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
    rc = BMEM_Heap_ConvertAddressToCached(surface->heap, mem_uncached, &surface->mem.buffer);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    BDBG_MSG(("NEXUS_Surface_Create:%#x %ux%u, pixel: %u", (unsigned)surface, pCreateSettings->width, pCreateSettings->height, pCreateSettings->pixelFormat));
    return surface;

error:
    NEXUS_Surface_Destroy(surface);
    return NULL;
}

NEXUS_SurfaceHandle
NEXUS_Surface_CreateFromMagnum_priv(BSUR_Surface_Handle magnum_surface)
{
    NEXUS_SurfaceHandle surface;
    void *mem_uncached;
    uint32_t width, height;
    BPXL_Format format;
    BERR_Code rc;

    BDBG_ASSERT(magnum_surface);
    NEXUS_ASSERT_MODULE();

    surface = BKNI_Malloc(sizeof(*surface));
    if (!surface) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}
    NEXUS_OBJECT_INIT(NEXUS_Surface, surface);
    surface->surface = magnum_surface;
    surface->heap = g_pCoreHandles->heap[0];

    /* For surfaces allocated by Magnum, convert to cached and use that address. */
    rc = BSUR_Surface_GetAddress(magnum_surface, &mem_uncached, (uint32_t *)&surface->mem.pitch);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_getaddress;}
    rc = BMEM_Heap_ConvertAddressToCached(surface->heap, mem_uncached, &surface->mem.buffer);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto err_address;}
    rc = BSUR_Surface_GetDimensions(magnum_surface,
        &width,
        &height);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto err_surface;}
    rc = BSUR_Surface_GetFormat(magnum_surface, &format);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto err_surface;}
    rc = BSUR_Surface_GetOffset(magnum_surface, &surface->offset);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto err_surface;}

    NEXUS_Surface_GetDefaultCreateSettings(&surface->createSettings);
    surface->mem.bufferSize = height*surface->mem.pitch;
    surface->createSettings.width = width;
    surface->createSettings.height = height;
    surface->createSettings.pixelFormat = NEXUS_P_PixelFormat_FromMagnum(format);
    return surface;

err_surface:
err_address:
err_getaddress:
    BKNI_Free(surface);
err_alloc:
    return NULL;
}

void
NEXUS_Surface_ReleaseSurface_priv(NEXUS_SurfaceHandle surface)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_DESTROY(surface, NEXUS_Surface);
    BKNI_Free(surface);
    return;
}

/*
Summary:
Frees internal resources allocated by NEXUS_Surface_CreateFromMagnum_priv
*/
void NEXUS_Surface_ReleaseSurface_priv(NEXUS_SurfaceHandle surface);

void NEXUS_Surface_GetMemory(NEXUS_SurfaceHandle surface, NEXUS_SurfaceMemory *pMemory)
{
    BDBG_OBJECT_ASSERT(surface, NEXUS_Surface);
    if ( surface->createSettings.bitsPerPixel )
    {
        BDBG_WRN(("This surface is not directly accessible by the CPU."));
    }
    *pMemory = surface->mem;
    return;
}


NEXUS_Error NEXUS_Surface_SetSettings(NEXUS_SurfaceHandle surface, const NEXUS_SurfaceSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(surface, NEXUS_Surface);
    if (pSettings->autoFlush) {
        /* See header file comments and SW7420-1205 for rationale for this failure. */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    surface->settings = *pSettings;
    return 0;
}

void NEXUS_Surface_GetSettings(NEXUS_SurfaceHandle surface, NEXUS_SurfaceSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(surface, NEXUS_Surface);
    *pSettings = surface->settings;
}


BSUR_Surface_Handle NEXUS_Surface_GetSurface_priv(NEXUS_SurfaceHandle surface)
{
    BDBG_OBJECT_ASSERT(surface, NEXUS_Surface);
    NEXUS_ASSERT_MODULE();
    return surface->surface;
}

void NEXUS_Surface_GetCreateSettings(NEXUS_SurfaceHandle surface, NEXUS_SurfaceCreateSettings *pCreateSettings)
{
    BDBG_OBJECT_ASSERT(surface, NEXUS_Surface);
    *pCreateSettings = surface->createSettings;
}

void NEXUS_Surface_InitPlane( NEXUS_SurfaceHandle surface, BM2MC_PACKET_Plane *pPlane)
{
    BDBG_OBJECT_ASSERT(surface, NEXUS_Surface);
    pPlane->address = surface->offset;
    pPlane->pitch = surface->mem.pitch;
    pPlane->format = surface->createSettings.pixelFormat;
    pPlane->width = surface->createSettings.width;
    pPlane->height = surface->createSettings.height;
}

void NEXUS_Surface_InitPlaneAndPaletteOffset( NEXUS_SurfaceHandle surface, BM2MC_PACKET_Plane *pPlane, unsigned *pPaletteOffset)
{
    BDBG_OBJECT_ASSERT(surface, NEXUS_Surface);
    pPlane->address = surface->offset;
    pPlane->pitch = surface->mem.pitch;
    pPlane->format = surface->createSettings.pixelFormat;
    pPlane->width = surface->createSettings.width;
    pPlane->height = surface->createSettings.height;
    *pPaletteOffset = surface->paletteOffset; /* will be 0 if no palette */
}

/*********************************
* striped surface
**********************************/

struct NEXUS_StripedSurface
{
    NEXUS_OBJECT(NEXUS_StripedSurface);
    NEXUS_StripedSurfaceCreateSettings createSettings;
};

void NEXUS_StripedSurface_GetDefaultCreateSettings( NEXUS_StripedSurfaceCreateSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_StripedSurfaceHandle NEXUS_StripedSurface_Create( const NEXUS_StripedSurfaceCreateSettings *pSettings )
{
    NEXUS_StripedSurfaceHandle stripedSurface;
    stripedSurface = BKNI_Malloc(sizeof(*stripedSurface));
    if (!stripedSurface) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_StripedSurface, stripedSurface);
    stripedSurface->createSettings = *pSettings;
    return stripedSurface;
}

static void NEXUS_StripedSurface_P_Finalizer( NEXUS_StripedSurfaceHandle stripedSurface )
{
    NEXUS_OBJECT_DESTROY(NEXUS_StripedSurface, stripedSurface);
    BKNI_Free(stripedSurface);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_StripedSurface, NEXUS_StripedSurface_Destroy);

void NEXUS_StripedSurface_GetCreateSettings( NEXUS_StripedSurfaceHandle stripedSurface, NEXUS_StripedSurfaceCreateSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(stripedSurface, NEXUS_StripedSurface);
    *pSettings = stripedSurface->createSettings;
}

NEXUS_Error NEXUS_StripedSurface_GetStatus( NEXUS_StripedSurfaceHandle stripedSurface, NEXUS_StripedSurfaceStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(stripedSurface, NEXUS_StripedSurface);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->width = stripedSurface->createSettings.imageWidth;
    pStatus->height = stripedSurface->createSettings.imageHeight;
    return 0;
}


