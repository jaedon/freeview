/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
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
 * $brcm_Workfile: bcmnexus_pool.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/20 $
 * $brcm_Date: 3/19/12 3:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_pool.c $
 *
 * DirectFB_1_4_15_Port/20   3/19/12 3:03p christ
 * SW7231-728: DirectFB-1.4.15: Segfault on slave app exit due to
 *  uninitialised handle in bcmJoinGraphics3DPool().
 *
 * DirectFB_1_4_15_Port/19   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/18   3/1/12 7:58p christ
 * SW7420-2273: DirectFB-1.4.15: Use correct Nexus heap for secondary
 *  display frame buffers
 *
 * DirectFB_1_4_15_Port/17   3/1/12 1:20p christ
 * SW7231-695: DirectFB-1.4.15: Refactor 3D Pool code to remove general
 *  failure.
 *
 * DirectFB_1_4_15_Port/16   2/28/12 5:26p robertwm
 * SW7420-2267: DirectFB-1.4.15: Off-screen surface buffers need to be
 *  cleared out after being allocated.
 *
 * DirectFB_1_4_15_Port/15   2/28/12 3:18p robertwm
 * SW7420-2267: DirectFB-1.4.15: Off-screen surface buffers need to be
 *  cleared out after being allocated.
 *
 * DirectFB_1_4_15_Port/14   2/16/12 4:29p christ
 * SW7425-2410: DirectFB-1.4.15: More efficient address conversion in
 *  BcmLock.
 *
 * DirectFB_1_4_15_Port/13   2/9/12 2:37p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/12   2/6/12 3:43p robertwm
 * SW7420-2233: DirectFB-1.4.15: Core: Surface allocation object, avoid
 *  system calls when rendering
 *
 * DirectFB_1_4_15_Port/11   2/2/12 2:38p robertwm
 * SW7420-2234: DirectFB: DFB Surface acess in Slave app with secure
 *  fusion causes seg-fault/corrupt buffer contents.
 * SW7420-2233: DirectFB-1.4.15: Core: Surface allocation object, avoid
 *  system calls when rendering.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:33p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/10   11/11/11 3:42p christ
 * SW7425-1689: DirectFB: Remove non secure fusion code from system
 *  driver.
 *
 * DirectFB_1_4_15_Port/9   11/7/11 4:19p christ
 * SW7425-1689: DirectFB: Remove non secure fusion code from system
 *  driver.
 *
 * DirectFB_1_4_15_Port/8   10/21/11 5:14p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/7   10/20/11 2:46p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/6   10/17/11 5:31p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/5   10/5/11 12:25p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 *
 * DirectFB_1_4_14_Port/4   9/23/11 12:48p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/2   8/12/11 4:44p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/5   8/15/11 3:12p christ
 * SW7425-1107: DirectFB: System pool driver does large memset on surface
 *  allocation
 *
 * DirectFB_1_4_5_Port/4   7/18/11 4:25p apandav
 *  SW7425-907: DirectFB: Reduce decode memory requirements for
 *  ImageProvider by striping
 *
 * DirectFB_1_4_5_Port/3   6/30/11 6:01p robertwm
 * SW7425-821: DirectFB: bcmLock() code should return physical address
 *  based on hwaddress.
 *
 * DirectFB_1_4_5_Port/2   6/24/11 3:13p christ
 * SW7420-1962: DirectFB: Improve debug for heap usage
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:05p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/47   6/10/11 5:17p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/6   4/29/11 1:47p tross
 * Ensure byte alignment is always set.
 * Correct byte alignment of palette.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/5   4/11/11 7:06p tross
 * Merge changes to get STB to compile.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   4/7/11 8:18p tross
 * Merge from latest.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   4/6/11 2:45p tross
 * Corect palette heap ptr when heap manager is used.
 * Correct display size for Bluray.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/46   4/4/11 3:25p christ
 * SW7420-1758: DirectFB: Clean up build process - less verbose build &
 *  remove warnings
 *
 * DirectFB_1_4_Port/45   3/17/11 6:52p robertwm
 * SW7422-378: DirectFB: Destroying a Palettised surface in a client
 *  application causes NEXUS assertion during free.
 *
 * DirectFB_1_4_Port/44   3/17/11 12:40p robertwm
 * SW7422-378: DirectFB: Destroying a Palettised surface in a client
 *  application causes NEXUS assertion during free.
 * SW7422-385: DirectFB: Using Image Provider results in some images being
 *  displayed incorrectly.
 *
 * DirectFB_1_4_Port/43   3/1/11 8:49p robertwm
 * SW7420-1559: DirectFB: NEXUS surface assertion in multi-app mode when
 *  closing down a DFB client.
 *
 * DirectFB_1_4_Port/42   3/1/11 12:01p christ
 * SW7420-1545: DirectFB: Application hang when using multi app DirectFB
 *  with SawMan window manager.
 *
 * DirectFB_1_4_Port/41   2/23/11 6:04p christ
 * SW7425-108: DirectFB: Insignia LUT8 tests failing on 7425 packet blit
 *  build.
 *
 * DirectFB_1_4_Port/40   2/20/11 7:34p robertwm
 * SW7420-1394: video layer support in DFB.
 * SW7422-275: DirectFB: Enabling video layer causes Segmentation fault.
 *
 * DirectFB_1_4_Port/39   2/17/11 11:12p robertwm
 * SW7420-1510: DirectFB: Reduce number of IPC calls for gfx operations
 *  using packet-blit interface.
 * SW7422-275: DirectFB: Enabling video layer causes Segmentation fault.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/38   2/8/11 4:58p robertwm
 * SW7420-1470: DirectFB: Ensure width/height/pitch of PX3D texture
 *  surfaces is a power of 2.
 * SW7420-1472: DirectFB: df_brcmTest fails to display preallocated
 *  surfaces.
 *
 * DirectFB_1_4_Port/37   2/8/11 3:18p robertwm
 * SW7420-1472: DirectFB: df_brcmTest fails to display preallocated
 *  surfaces.
 *
 * DirectFB_1_4_Port/36   2/8/11 1:23p robertwm
 * SW7420-1468: DirectFB: Add context debug color output.
 * SW7420-1470: DirectFB: Ensure width/height/pitch of PX3D texture
 *  surfaces is a power of 2.
 *
 * DirectFB_1_4_Port/35   2/3/11 6:02p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps.
 *
 * DirectFB_1_4_Port/34   1/19/11 12:25p scchang
 * SW7420-1394: video layer support in DFB
 *
 * DirectFB_1_4_Port/33   12/16/10 5:54p robertwm
 * SW35230-2597: DirectFB: Cannot build bcmnexus_pool.c due to recent
 *  changes to support multiple gfx heaps.
 *
 * DirectFB_1_4_Port/32   12/14/10 7:25p robertwm
 * SW7422-147: DirectFB: Cannot run df_dok or df_window with changes for
 *  OpenGL ES 2.0
 *
 * DirectFB_1_4_Port/31   12/14/10 1:17p robertwm
 * SW7422-140: DirectFB: Support OpenGL ES 2.0 applications on the VC-4
 *  core.
 *
 * DirectFB_1_4_Port/30   12/3/10 8:39p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/29   12/1/10 3:47p christ
 * SW7420-1283: DirectFB: Add support for extra Nexus heap on dual MEMC
 *  parts
 *
 * DirectFB_1_4_Port/28   10/19/10 5:41p robertwm
 * SW7420-1189: DirectFB: Kernel Oops when running in multi-app mode and
 *  using palletised surfaces.
 *
 * DirectFB_1_4_Port/27   10/12/10 2:54p robertwm
 * SW7550-589: DirectFB: Add LUT4 pixel format support to DirectFB.
 *
 * DirectFB_1_4_Port/26   9/23/10 2:48p robertwm
 * SW7405-4891: DirectFB: When running multiple instances of df_andi, a
 *  deadlock can occur.
 *
 * DirectFB_1_4_Port/25   9/15/10 4:29p robertwm
 * SW7405-4875: DirectFB: M2MC trampling over memory when surface is
 *  reconfigured.
 *
 * DirectFB_1_4_Port/24   8/20/10 3:40p robertwm
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/23   8/17/10 11:55a robertwm
 * SW7405-4768: DirectFB: Cannot build DirectFB-1.4.1 Phase 2.5 release on
 *  7550/7540 chips.
 *
 * DirectFB_1_4_Port/22   8/16/10 11:39p robertwm
 * SW7405-4761: DirectFB: Optimise the surface pool locking code.
 *
 * DirectFB_1_4_Port/21   8/11/10 2:00a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/20   7/26/10 5:32p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/19   7/22/10 4:23p robertwm
 * SW7405-4649: DirectFB: Optimise surface locking and wait idle syncing.
 *
 * DirectFB_1_4_Port/18   7/2/10 12:23p robertwm
 * SW3556-1141: DirectFB: The Broadcom surface pool should support cursor
 *  surface types.
 *
 * DirectFB_1_4_Port/17   5/27/10 9:10p robertwm
 * SW7420-786: DirectFB: the Broadcom Surface Pool should NOT allow
 *  internal/system memory allocations.
 * SW7420-785: DirectFB: 7420 cannot blit from palletised surface created
 *  in heap1.
 *
 * DirectFB_1_4_Port/16   4/9/10 4:52p robertwm
 * SW7420-510: DirectFB: Support Vertical Scaler in Graphics Feeder.
 * SW7405-4180: DirectFB: Require independent setting of graphics
 *  resolution on multiple output paths.
 * SW7405-4184: Allow independent mode and resolution to be specified.
 * SW7405-4185: DirectFB: Cache flushing issues when changing between GPU
 *  and CPU.
 *
 * DirectFB_1_4_Port/15   1/25/10 10:24a robertwm
 * SW7405-3827: DirectFB: system module not including
 *  bcmnexus_gfx_defines.h.
 *
 * DirectFB_1_4_Port/14   1/21/10 1:35p robertwm
 * SW7405-3814: DirectFB: Changing surface configuration when in
 *  DLSCL_EXCLUSIVE layer mode causes seg fault.
 *
 * DirectFB_1_4_Port/13   1/20/10 5:30p robertwm
 * SW7405-3814: DirectFB: Changing surface configuration when in
 *  DLSCL_EXCLUSIVE layer mode causes seg fault.
 *
 * DirectFB_1_4_Port/12   1/4/10 6:01p robertwm
 * SW7405-3701: DirectFB: Different video and graphics resolutions.
 *
 * DirectFB_1_4_Port/11   12/23/09 10:19p robertwm
 * SW7420-510: DirectFB: Support Vertical Scaler in Graphics Feeder.
 *
 * DirectFB_1_4_Port/10   12/23/09 9:56p robertwm
 * SW7420-510: DirectFB: Support Vertical Scaler in Graphics Feeder.
 *
 * DirectFB_1_4_Port/9   12/23/09 2:23p robertwm
 * SW7550-51: Support DirectFB on 7550.
 *
 * DirectFB_1_4_Port/SW7550-51/1   12/22/09 3:22p mphillip
 * SW7550-51: Add 7550 build support (on branch)
 *
 * DirectFB_1_4_Port/8   12/16/09 9:49p robertwm
 * SW7400-2643: DirectFB: Add support for setting/getting video modes.
 *
 * DirectFB_1_4_Port/7   12/4/09 9:01p robertwm
 * SW7405-3550: DirectFB: BMEM: Back guard violation in block!
 *
 * DirectFB_1_4_Port/6   12/4/09 7:40p robertwm
 * SW7405-3529: cacheflush issues (fixed flush when using sub-surfaces).
 *
 * DirectFB_1_4_Port/5   12/3/09 1:37p robertwm
 * SW7405-3529: Cacheflush issues.
 *
 * DirectFB_1_4_Port/4   11/25/09 11:08a robertwm
 * SW7405-3438: DirectFB: Palette based blits do not work correctly.
 * SW7405-3471: Nexus needs to support additional pixel formats for
 *  DirectFB.
 *
 * DirectFB_1_4_Port/3   8/21/09 11:12a robertwm
 * PR57917: DirectFB: testman and testrun don't run anymore in 1.4.1
 *  (SaWMan)
 *
 * DirectFB_1_4_Port/2   8/7/09 3:45p robertwm
 * PR56902: Require single graphics memory heap to avoid fragmentation.
 *
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 *****************************************************************************/

#include <string.h>

#include "bcmnexus.h"
#include "bcmnexus_layer.h"
#include "bcmnexus_pool.h"
#include "bcmnexus_utils.h"
#include "nexus_base_mmap.h"

#include <config.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/modules.h>

#include <fusion/conf.h>
#include <fusion/shmalloc.h>

#include <core/core.h>
#include <core/gfxcard.h>
#include <core/system.h>
#include <core/surface_pool.h>
#include <core/palette.h>
#include <core/layers.h>
#include <core/layers_internal.h>
#include <core/screens.h>
#include <core/coredefs.h>

#include <directfb_util.h>

#ifdef __GNUC__
#define UNUSED_ __attribute__ ((unused))
#else
#define UNUSED_
#endif

/* Uncomment out the line below to use Nexus memory that has been pre-allocated
   for creating surfaces... */
/*#define PREALLOCATE_MEMORY */

#define CYAN_COLOUR "\033[36m"
#define BLUE_COLOUR "\033[34m"
#define NO_COLOUR "\033[0m"

D_DEBUG_DOMAIN( bcmnexusPool,        "bcmNexus/Pool",        "Broadcom NEXUS Surface Pool" );
D_DEBUG_DOMAIN( bcmnexusPoolX,       "bcmNexus/PoolX",       "Broadcom NEXUS Surface Pool eXtended Debug" );
D_DEBUG_DOMAIN( bcmnexusPoolPalette, "bcmNexus/PoolPalette", "Broadcom NEXUS Surface Pool Palette Debug" );

struct bcmPoolData {
     FusionSHMPoolShared        *pool;              /* this is where the memory is */
     DFBBCMNEXUS                *bcmnexus;          /* shared struct to NEXUS handles e.a. */
     int                         refs;              /* Reference count */
};

struct bcmPoolLocal
{
     CoreDFB                    *core;
     struct bcmPoolData         *pd;
     int                         refs;              /* Reference count */
     DFB_Platform_P_PoolHandle   gfx3DPoolHandle;   /* Handle for the 3D graphics pool */
};


/**********************************************************************************************************************/
static DFBResult
createNexusSurface( CoreDFB                  *core,
                    CoreSurfaceAllocation    *allocation,
                    struct bcmAllocationData *ad,
                    struct bcmPoolLocal      *pl )
{
     struct bcmPoolData            *pd = pl->pd;
     CoreSurfaceBuffer             *buffer;
     CoreSurface                   *surface;
     CorePalette                   *palette;
     NEXUS_SurfaceCreateSettings    createSettings;
     NEXUS_MemoryAllocationSettings paletteMemorySettings;
     NEXUS_PixelFormat              eFormat;
     NEXUS_SurfaceMemory            memory;
     NEXUS_MemoryStatus             memoryStatus;
     CoreGraphicsDevice            *device;
     CardLimitations                limits;

     buffer  = allocation->buffer;

     device = dfb_core_get_part( core, DFCP_GRAPHICS );
     D_ASSERT(device != NULL);
     dfb_gfxcard_get_limits(device, &limits);
     if (!limits.surface_byteoffset_alignment)
         limits.surface_byteoffset_alignment = 32;

     surface = buffer->surface;
     palette = surface->palette;

     eFormat = bcmnexus_getNexusPixelFormat(buffer->format);

     if (eFormat == NEXUS_PixelFormat_eUnknown)
     {
        D_ERROR( "%s: unsupported format %s\n",__FUNCTION__, dfb_pixelformat_name(surface->config.format) );
        return DFB_UNSUPPORTED;
     }

     ad->gfx3DPoolHandle = pl->gfx3DPoolHandle;

     /* IMPORTANT: The GFD doesn't support 15 or 24-bit formats, so we need to make sure
        that the pixelformat for the primary layer isn't one of these formats. */
     if (surface->type & CSTF_LAYER)
     {
#ifdef BCMNEXUS_PIXELFORMAT_R8_G8_B8_SUPPORTED
         if (eFormat == NEXUS_PixelFormat_eR8_G8_B8)
         {
            D_WARN("bcmNexus/Pool: GFD cannot support DSPF_RGB24, defaulting to DSPF_RGB32 instead.\n");
            eFormat = bcmnexus_getNexusPixelFormat(DSPF_RGB32);
         }
#endif

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
         if (surface->resource_id == DLID_PRIMARY)
         {
             BM2MC_PACKET_Plane framebufferPlane;
             BERR_Code rc;
             rc = NEXUS_SurfaceClient_AcquireTunneledSurface(GET_DISPLAY_HND(pd->bcmnexus,0), &ad->bcmSurfaceHandle);
             D_DEBUG_AT( bcmnexusPool,"acquired tunnel surface %#lx\n", (unsigned long)ad->bcmSurfaceHandle);
             if ( rc != NEXUS_SUCCESS || ad->bcmSurfaceHandle==NULL )
             {
                 D_ERROR( "bcmNexus/Pool: NEXUS_SurfaceClient_GetTunneledSurface failed!\n" );
                 return DFB_LIMITEXCEEDED;
             }
             NEXUS_Surface_GetCreateSettings(ad->bcmSurfaceHandle, &createSettings);
             NEXUS_Surface_GetMemory(ad->bcmSurfaceHandle, &memory);
             if( eFormat != createSettings.pixelFormat, surface->config.size.w != createSettings.width  || surface->config.size.h != createSettings.height) {
                 D_ERROR( "bcmNexus/Pool: invalid data pixel:%u,%u width:%u,%u height:%u,%u pitch:%u,%u !",
                          eFormat, createSettings.pixelFormat, surface->config.size.w, createSettings.width,
                          surface->config.size.h, createSettings.height, ad->pitch, memory.pitch );
                 D_ERROR( "bcmNexus/Pool: invalid data!\n");
                 return DFB_LIMITEXCEEDED;
             }

             /* Stop trying to be clever and let NEXUS figure these out.*/
             NEXUS_Surface_InitPlane(ad->bcmSurfaceHandle, &framebufferPlane);
             /* store buffer info */
             ad->pitch        = framebufferPlane.pitch;
             ad->allocation   = allocation;
             ad->address      = memory.buffer;
             ad->pixelformat  = framebufferPlane.format;
             ad->offsetInHeap = 0;
             ad->hwaddress    = framebufferPlane.address;
             ad->size = createSettings.height * memory.pitch;

             return DFB_OK;
         }
#endif

#ifndef BCMNEXUS_GFD0_HAS_VERTICAL_SCALER
         /* Check to see if we are creating the actual framebuffer.  If so, then this needs to
            have a maximum height of all possible TV resolutions as we don't have a vertical
            scaler in the graphics feeder path.
         */
         if (surface->resource_id == DLID_PRIMARY)
         {
             surface->config.min_size.h = MAX(surface->config.min_size.h, BCMNEXUS_MAX_HD_DISPLAY_HEIGHT);
         }
#endif
#ifndef BCMNEXUS_GFD1_HAS_VERTICAL_SCALER
         /* Check to see if we are creating the actual framebuffer.  If so, then this needs to
            have a maximum height of all possible TV resolutions as we don't have a vertical
            scaler in the graphics feeder path.
         */
         if (surface->resource_id == (DLID_PRIMARY+1))
         {
              surface->config.min_size.h = MAX(surface->config.min_size.h, BCMNEXUS_MAX_SD_DISPLAY_HEIGHT);
          }
#endif
     }

     NEXUS_Surface_GetDefaultCreateSettings(&createSettings);


          dfb_gfxcard_calc_buffer_size( dfb_gfxcard_get_primary(), buffer, &ad->pitch, &ad->size );

          /* Always ensure that the palette is created in heap 0 for off-screen surfaces, as otherwise the surface won't be
            blitted correctly on platforms such as the 7420 where RTS dictates that the palette and the M2MC
            packet-buffer must be on heap 0. If the surface is the actual framebuffer (GFD), then the palette must be on
            heap 1 for the 7420, because there is no bandwidth allocated on memc0 (client 29 on mem0 is disabled).
          */
          if (DFB_PIXELFORMAT_IS_INDEXED(surface->config.format)) {
              NEXUS_Memory_GetDefaultAllocationSettings(&paletteMemorySettings);
              paletteMemorySettings.alignment = 32;  /* Palette needs to be aligned on a 32-byte boundary */

              if (surface->type & CSTF_LAYER)
                  if (surface->resource_id == DLID_PRIMARY)
                      paletteMemorySettings.heap = GET_DISPLAY_HEAP_HND(pd->bcmnexus);
                  else
                      paletteMemorySettings.heap = GET_2ND_DISPLAY_HEAP_HND(pd->bcmnexus);
              else
                  paletteMemorySettings.heap = GET_PALETTE_HEAP_HND(pd->bcmnexus);

              /* From bsur.c:
                 M2MC HW accesses palette as 1024 bytes, regardless of palette format.
                 so we must alloc a constant 1024 to avoid ARC violations. */
              if (NEXUS_Memory_Allocate(1024, &paletteMemorySettings, &createSettings.pPaletteMemory) != NEXUS_SUCCESS)
              {
                  D_ERROR("%s: Error creating Memory of %d bytes for palette!\n", __FUNCTION__, palette->num_entries*sizeof(NEXUS_Pixel));
                  return DFB_NOSHAREDMEMORY;
              }

              /* Save address of palette for later */
              ad->palette = createSettings.pPaletteMemory;

              D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() Palette memory=%p heap=%p [%d entries]\n"NO_COLOUR,
                          __FUNCTION__, (void *)createSettings.pPaletteMemory, (void *)paletteMemorySettings.heap, palette->num_entries);
          }

          /* If we have created a pre-allocated surface in BMEM, then we don't have to allocate any surface data */
          if ((surface->type & CSTF_PREALLOCATED) && (buffer->policy == CSP_VIDEOONLY)) {
               int                     index = dfb_surface_buffer_index( buffer );
               DFB_PlatformNexusHandle heap;
               NEXUS_MemoryStatus      memoryStatus;

               D_ASSERT( surface->config.preallocated[index].handle != NULL );
               D_ASSERT( surface->config.preallocated[index].pitch != 0 );

               heap = surface->config.preallocated[index].handle;

               if (NEXUS_Heap_GetStatus( heap, &memoryStatus ) != NEXUS_SUCCESS)
                   return DFB_FAILURE;

               /* Update the pitch to the actual provided one... */
               ad->pitch                = surface->config.preallocated[index].pitch;

               createSettings.height    = surface->config.size.h;
               createSettings.pMemory   = (u8*) memoryStatus.addr + surface->config.preallocated[index].offset;
               createSettings.heap      = heap;
          }
          else {
               createSettings.height    = MAX(surface->config.size.h, surface->config.min_size.h);

               /* This surface is allocated on video layer? */
               if ((surface->type & CSTF_LAYER) && (surface->resource_id == (DLID_PRIMARY+2))) {
                    createSettings.heap = GET_VIDEO_HEAP_HND(pd->bcmnexus);
               }
          }
          createSettings.pixelFormat    = eFormat;
          createSettings.width          = surface->config.size.w;
          createSettings.pitch          = ad->pitch;
          createSettings.alignment      = 12; /* Ensure surface is aligned on 4096Kbyte boundary to help prevent "cache crosstalk" */

     if (createSettings.heap == NULL)
     {
          /* Layer and VC-4 allocations must be from the display heap */
          if (surface->type & CSTF_LAYER)
              createSettings.heap = (surface->resource_id == DLID_PRIMARY) ? GET_DISPLAY_HEAP_HND(pd->bcmnexus) : GET_2ND_DISPLAY_HEAP_HND(pd->bcmnexus);
          else
              createSettings.heap = GET_OFFSCREEN_HEAP_HND(pd->bcmnexus);

          if (createSettings.heap == NULL)
          {
              D_ERROR( "bcmNexus/Pool: Invalid heap for surface allocation\n");
              return DFB_NOVIDEOMEMORY;
          }

          /* Check that we can actually allocate this amount of memory from our desired pool.
           * If not fall back to the secondry pool if possible */
          if (NEXUS_Heap_GetStatus(createSettings.heap, &memoryStatus) != NEXUS_SUCCESS)
               return DFB_FAILURE;

              if (memoryStatus.largestFreeBlock < (unsigned int)ad->size)
              {
                  D_ERROR( "bcmNexus/Pool: Insufficient memory to create surface (%d bytes short) - %s heap\n",
                           ad->size - memoryStatus.largestFreeBlock,
                       (createSettings.heap == GET_OFFSCREEN_HEAP_HND(pd->bcmnexus)) ? "offscreen" : "primary");
              return DFB_NOVIDEOMEMORY;
          }
     }
#ifdef PREALLOCATE_MEMORY
     if (createSettings.pMemory == NULL)
     {
          NEXUS_MemoryAllocationSettings surfaceMemorySettings;
          NEXUS_Memory_GetDefaultAllocationSettings(&surfaceMemorySettings);
          surfaceMemorySettings.alignment = 4096;  /* Surface needs to be aligned on a 4K boundary */
          surfaceMemorySettings.heap = createSettings.heap;

          if (NEXUS_Memory_Allocate((size_t)ad->size, &surfaceMemorySettings, &createSettings.pMemory) != NEXUS_SUCCESS)
          {
              D_ERROR( "bcmNexus/Pool: Insufficient preallocated memory to create surface (%d bytes)\n", ad->size);
              return DFB_NOVIDEOMEMORY;
          }
     }
#endif

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_MULTIBUFFER_MODE

    /* Stereoscopic 3D adjustments ...
     * In order to cope with the Surface compositor API for stereoscopic display it is necessary to pass a single
     * buffer containing the left and right eye views unlike the normal nexus approach of passing 2 separate buffers.
     * The simplest way of doing this is to create a double width buffer when creating the left eye surface and alter
     * the pitch. Then when we come to allocate the right eye we get the buffer details from the left eye and massage
     * the surface details to use the extra allocated space for the right eye.
     *
     * The static below is required as if you allocate a tripple buffered frame buffer all the left eyes then the right
     * eyes are allocated in that order.
     */

    static unsigned int buffer_index = 0;

    if (surface->config.caps & DSCAPS_STEREO)
    {
        D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s: Surface is for %s eye\n", __FUNCTION__, dfb_surface_get_stereo_eye(surface) == DSSE_LEFT ? "DSSE_LEFT" : "DSSE_RIGHT");

        if (dfb_surface_get_stereo_eye(surface) == DSSE_LEFT)
        {
            /*This is the LEFT eye buffer, make space for the RIGHT eye buffer*/
            createSettings.pitch *=2;
            createSettings.width *=2;
            ad->pitch = createSettings.pitch;
            buffer_index = 0;
        }
        else
        {
            CoreSurfaceAllocation *leftSurfaceAllocation = NULL;
            struct bcmAllocationData *leftBcmAd = NULL;
            NEXUS_SurfaceMemory leftSurfaceMemory;

            /*RIGHT eye*/
            /*The LEFT eye has the space for the RIGHT eye's buffer, fetch it.*/
            dfb_surface_set_stereo_eye(surface, DSSE_LEFT);

            leftSurfaceAllocation = dfb_surface_buffer_find_allocation(surface->buffers[buffer_index % 3], CSAID_NONE, CSAF_NONE,false);
            if (leftSurfaceAllocation == NULL)
            {
                 D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR "%s: leftSurfaceAllocation == NULL\n", __FUNCTION__);
                  return DFB_FAILURE;
            }
            buffer_index ++;
            leftBcmAd = leftSurfaceAllocation->data;
            if (leftBcmAd == NULL)
            {
                 D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR "%s: leftBcmAd == NULL\n", __FUNCTION__);
                 return DFB_FAILURE;
            }

            NEXUS_Surface_GetMemory(leftBcmAd->bcmSurfaceHandle, &leftSurfaceMemory);

            createSettings.pMemory = (void*)((unsigned int)(leftSurfaceMemory.buffer) + (leftSurfaceMemory.pitch / 2));
            createSettings.pitch = leftSurfaceMemory.pitch;
            ad->pitch = createSettings.pitch;

            dfb_surface_set_stereo_eye(surface, DSSE_RIGHT);

        }
    }
#endif

     ad->bcmSurfaceHandle = NEXUS_Surface_Create(&createSettings);
     if (ad->bcmSurfaceHandle==NULL) {
         D_ERROR( "bcmNexus/Pool: NEXUS_Surface_Create failed!\n" );
         return DFB_NOVIDEOMEMORY;
     }

     NEXUS_Surface_GetMemory(ad->bcmSurfaceHandle, &memory);

     if (memory.buffer == NULL)
     {
         D_ERROR("Failed to get buffer information for surface %p\n", (void *)ad->bcmSurfaceHandle);
         return DFB_NOVIDEOMEMORY;
     }

     /* store buffer info */
     ad->allocation      = allocation;
     ad->address         = memory.buffer;
     ad->pixelformat     = createSettings.pixelFormat;

     D_ASSERT(createSettings.heap);

     if (NEXUS_Heap_GetStatus(createSettings.heap, &memoryStatus) != NEXUS_SUCCESS)
         return DFB_FAILURE;

     ad->heap         = createSettings.heap;
     ad->offsetInHeap = (uint8_t*)memory.buffer - (uint8_t*)memoryStatus.addr;
     ad->hwaddress    = memoryStatus.offset + ad->offsetInHeap;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s: Nexus_Surface_Created() %s: sur handle %p vaddr=%p hwaddr=%p [heap: handle %p, vaddr=%p hwaddr=%p]"
                               ", WxH=%dx%d, pitch=%d bytes, size=%d bytes, dfb_pixelformat=%s\n"NO_COLOUR, __FUNCTION__,
                               (surface->type & CSTF_LAYER) ? "framebuffer" : "offscreen",
                               (void*)(ad->bcmSurfaceHandle), (void *)ad->address, (void *)ad->hwaddress,
                               (void *)ad->heap, (void *)memoryStatus.addr, (void *)memoryStatus.offset,
                               createSettings.width, createSettings.height, ad->pitch, ad->size, dfb_pixelformat_name(buffer->format) );

     /* Load palette if it has one */
     if (DFB_PIXELFORMAT_IS_INDEXED(surface->config.format)) {
          unsigned int i;

          D_ASSERT(paletteMemorySettings.heap);

          if (NEXUS_Heap_GetStatus(paletteMemorySettings.heap, &memoryStatus) != NEXUS_SUCCESS)
              return DFB_FAILURE;

          ad->hwpalette = memoryStatus.offset + (uint8_t*)memory.palette - (uint8_t*)memoryStatus.addr;

          for (i=0; i<palette->num_entries; i++)
          {
               memory.palette[i] = PIXEL_ARGB(
                    palette->entries[i].a,
                    palette->entries[i].r,
                    palette->entries[i].g,
                    palette->entries[i].b);
               D_DEBUG_AT( bcmnexusPoolPalette, CYAN_COLOUR"Palette entry %d: 0x%08x\n"NO_COLOUR, i, memory.palette[i] );
          }
          memory.numPaletteEntries = palette->num_entries;
          D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"Palette copied to %p [num entries=%d]\n"NO_COLOUR, (void *)memory.palette, memory.numPaletteEntries );
     }

     return DFB_OK;
}

/**********************************************************************************************************************/

static int bcmPoolDataSize( void )
{
     return sizeof(struct bcmPoolData);
}

static int bcmPoolLocalDataSize( void )
{
    return sizeof(struct bcmPoolLocal);
}

static int bcmAllocationDataSize( void )
{
     return sizeof(struct bcmAllocationData);
}

static DFBResult
bcmInitPoolCommon( CoreDFB             *core,
                   struct bcmPoolData  *pd,
                   struct bcmPoolLocal *pl,
                   DFBBCMNEXUS         *bcmnexus )
{
     DFBResult  result = DFB_OK;

     pl->core     = core;
     pd->pool     = dfb_core_shmpool( core );
     pd->bcmnexus = bcmnexus;
     pl->pd       = pd;

     return result;
}

static DFBResult
bcmInitPool( CoreDFB                    *core,
             CoreSurfacePool            *pool,
             void                       *pool_data,
             void                       *pool_local,
             void                       *system_data,
             CoreSurfacePoolDescription *ret_desc )
{
     DFBResult            result;
     struct bcmPoolData  *pd = pool_data;
     struct bcmPoolLocal *pl = pool_local;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_ASSERT( ret_desc != NULL );

     snprintf( ret_desc->name, DFB_SURFACE_POOL_DESC_NAME_LENGTH, "BRCM Surface Pool" );

     result = bcmInitPoolCommon(core, pd, pl, system_data);

     if (result == DFB_OK)
     {
         ret_desc->caps = CSPCAPS_VIRTUAL | CSPCAPS_PHYSICAL;

         ret_desc->access[CSAID_CPU]    = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
         ret_desc->access[CSAID_GPU]    = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
         ret_desc->access[CSAID_LAYER0] = CSAF_READ;
         ret_desc->access[CSAID_LAYER1] = CSAF_READ;
         ret_desc->access[CSAID_LAYER2] = CSAF_READ;
         ret_desc->access[CSAID_LAYER3] = CSAF_READ;
         ret_desc->access[CSAID_LAYER4] = CSAF_READ;
         ret_desc->access[CSAID_LAYER5] = CSAF_READ;
         ret_desc->access[CSAID_LAYER6] = CSAF_READ;
         ret_desc->access[CSAID_LAYER7] = CSAF_READ;

         ret_desc->types    = CSTF_LAYER | CSTF_WINDOW | CSTF_CURSOR | CSTF_FONT | CSTF_SHARED | CSTF_EXTERNAL | CSTF_PREALLOCATED;
         ret_desc->priority = CSPP_PREFERED;
     }

     /* TODO: Add size field */

     return result;
}

static DFBResult
bcmInitGraphics3DPool( CoreDFB                    *core,
                       CoreSurfacePool            *pool,
                       void                       *pool_data,
                       void                       *pool_local,
                       void                       *system_data,
                       CoreSurfacePoolDescription *ret_desc )
{
     DFBResult                                 result;
     struct bcmPoolData                       *pd = pool_data;
     struct bcmPoolLocal                      *pl = pool_local;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_ASSERT( ret_desc != NULL );

     snprintf( ret_desc->name, DFB_SURFACE_POOL_DESC_NAME_LENGTH, "BRCM Graphics3D Surface Pool" );

     result = bcmInitPoolCommon(core, pd, pl, system_data);

     if (result == DFB_OK)
     {
         /* We are intentionally not checking the return code of this function so that the register pool
            in bcmnexus.c always succeeds. We check in the test if the pool handle is valid instead */
         DFB_Platform_P_Graphics3D_PoolInit( &(pl->gfx3DPoolHandle));

         ret_desc->caps = CSPCAPS_VIRTUAL | CSPCAPS_PHYSICAL;

         ret_desc->access[CSAID_CPU]    = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
         ret_desc->access[CSAID_GPU]    = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
         ret_desc->access[CSAID_LAYER0] = CSAF_READ;
         ret_desc->access[CSAID_LAYER1] = CSAF_READ;
         ret_desc->access[CSAID_LAYER2] = CSAF_READ;
         ret_desc->access[CSAID_LAYER3] = CSAF_READ;
         ret_desc->access[CSAID_LAYER4] = CSAF_READ;
         ret_desc->access[CSAID_LAYER5] = CSAF_READ;
         ret_desc->access[CSAID_LAYER6] = CSAF_READ;
         ret_desc->access[CSAID_LAYER7] = CSAF_READ;

         ret_desc->types    = CSTF_LAYER | CSTF_WINDOW | CSTF_SHARED | CSTF_EXTERNAL;
         ret_desc->priority = CSPP_PREFERED;
     }

     /* TODO: Add size field */

     return result;
}


static DFBResult
bcmJoinPool( UNUSED_ CoreDFB         *core,
                     CoreSurfacePool *pool,
             UNUSED_ void            *pool_data,
             UNUSED_ void            *pool_local,
             UNUSED_ void            *system_data )
{
     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     return DFB_OK;
}

static DFBResult
bcmJoinGraphics3DPool( UNUSED_ CoreDFB         *core,
                               CoreSurfacePool *pool,
                       UNUSED_ void            *pool_data,
                               void            *pool_local,
                       UNUSED_ void            *system_data )
{
     struct bcmPoolLocal                      *pl = pool_local;

     pl->gfx3DPoolHandle = NULL;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     /* We are intentionally not checking the return code of this function so that the join pool
        in bcmnexus.c always succeeds. We check in the test if the pool handle is valid instead */
     DFB_Platform_P_Graphics3D_PoolInit( &(pl->gfx3DPoolHandle));

     return DFB_OK;

}

static DFBResult
bcmDestroyPool( CoreSurfacePool *pool,
       UNUSED_  void            *pool_data,
       UNUSED_  void            *pool_local )
{
     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     return DFB_OK;
}

static DFBResult
bcmDestroyGraphics3DPool( CoreSurfacePool *pool,
                 UNUSED_  void            *pool_data,
                          void            *pool_local )
{
     struct bcmPoolLocal *pl = pool_local;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     if (pl->gfx3DPoolHandle)
     {
        DFB_Platform_P_Graphics3D_PoolUninit(pl->gfx3DPoolHandle);
        pl->gfx3DPoolHandle = NULL;
     }

     return DFB_OK;
}

static DFBResult
bcmLeavePool( CoreSurfacePool *pool,
      UNUSED_ void            *pool_data,
      UNUSED_ void            *pool_local )
{
     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     return DFB_OK;
}

static DFBResult
bcmLeaveGraphics3DPool( CoreSurfacePool *pool,
               UNUSED_  void            *pool_data,
                        void            *pool_local )
{
     struct bcmPoolLocal *pl = pool_local;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s()\n"NO_COLOUR, __FUNCTION__ );

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     if (pl->gfx3DPoolHandle)
     {
        DFB_Platform_P_Graphics3D_PoolUninit(pl->gfx3DPoolHandle);
        pl->gfx3DPoolHandle = NULL;
     }

     return DFB_OK;
}

static DFBResult
bcmTestConfig( CoreSurfacePool         *pool,
     UNUSED_   void                    *pool_data,
     UNUSED_   void                    *pool_local,
               CoreSurfaceBuffer       *buffer,
               const CoreSurfaceConfig *config )
{
     NEXUS_PixelFormat        eFormat;
     DFBSurfaceCapabilities   caps;
     DFBResult                result    = DFB_OK;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
     D_ASSERT( config != NULL );

     caps = buffer->surface->config.caps;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() format=%s, type=0x%08x, policy=0x%08x, caps=0x%08x\n"NO_COLOUR, __FUNCTION__,
                 dfb_pixelformat_name(config->format), buffer->type, buffer->policy, caps );

     eFormat = bcmnexus_getNexusPixelFormat(config->format);

     if (eFormat == NEXUS_PixelFormat_eUnknown)
     {
        D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s: unsupported format %s\n"NO_COLOUR, __FUNCTION__, dfb_pixelformat_name(config->format) );
        result = DFB_UNSUPPORTED;
     }

     /* We only support hardware acceleration of preallocated data from the external video memory */
     if (D_FLAGS_IS_SET(buffer->type, CSTF_PREALLOCATED) && !D_FLAGS_IS_SET(buffer->type, CSTF_EXTERNAL))
     {
          D_DEBUG_AT( bcmnexusPool, "%s: cannot support preallocated surface data from internal memory!\n", __FUNCTION__ );
          result = DFB_UNSUPPORTED;
     }

     if (caps & DSCAPS_GL)
     {
        D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s: unsupported surface caps 0x%08x\n"NO_COLOUR, __FUNCTION__, caps );
        result = DFB_UNSUPPORTED;
     }

     return result;
}

static DFBResult
bcmTestGraphics3DConfig( CoreSurfacePool         *pool,
                 UNUSED_ void                    *pool_data,
                 UNUSED_ void                    *pool_local,
                         CoreSurfaceBuffer       *buffer,
                         const CoreSurfaceConfig *config )
{
     NEXUS_PixelFormat        eFormat;
     DFBSurfaceCapabilities   caps;
     DFBResult                result    = DFB_OK;
     struct bcmPoolLocal     *pl        = pool_local;


     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
     D_ASSERT( config != NULL );

     caps = buffer->surface->config.caps;

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() format=%s, caps=0x%08x\n"NO_COLOUR, __FUNCTION__, dfb_pixelformat_name(config->format), caps );

     eFormat = bcmnexus_getNexusPixelFormat(config->format);

     if (eFormat == NEXUS_PixelFormat_eUnknown || DFB_PIXELFORMAT_IS_INDEXED(config->format))
     {
        D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s: unsupported format %s\n"NO_COLOUR, __FUNCTION__, dfb_pixelformat_name(config->format) );
        result = DFB_UNSUPPORTED;
     }

     if (!(caps & DSCAPS_GL))
     {
        D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s: unsupported surface caps 0x%08x\n"NO_COLOUR, __FUNCTION__, caps );
        result = DFB_UNSUPPORTED;
     }

     if (!(pl->gfx3DPoolHandle))
     {
        D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s: no valid platform pool!\n"NO_COLOUR, __FUNCTION__);
        result = DFB_UNSUPPORTED;
     }

     return result;
}

static DFBResult
bcmPreAlloc( CoreSurfacePool             *pool,
     UNUSED_ void                        *pool_data,
     UNUSED_ void                        *pool_local,
             const DFBSurfaceDescription *description,
             CoreSurfaceConfig           *config )
{
     unsigned int i, num = 1;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );

     D_DEBUG_AT( bcmnexusPool,
                 CYAN_COLOUR"%s() description=%p, config=%p [master=%c]\n"NO_COLOUR,
                 __FUNCTION__, (void*) description, (void*) config, dfb_core_is_master( dfb_bcmnexus_core ) ? 'y' : 'n' );

     if (config->caps & DSCAPS_DOUBLE)
          num = 2;
     else if (config->caps & DSCAPS_TRIPLE)
          num = 3;

     if (!(config->caps & DSCAPS_VIDEOONLY))
          return DFB_UNSUPPORTED;

     for (i=0; i<num; i++) {
          DFB_PlatformNexusHandle heap;
          NEXUS_MemoryStatus      memoryStatus;

          heap = DFB_Platform_P_GetGraphicsHeap( description->preallocated[i].data );
          if (!heap) {
               D_ERROR( "bcmNexus/Pool: Failed to determine heap for surface pre-allocation (address %p)!\n",
                        description->preallocated[i].data );
               return DFB_UNSUPPORTED;
          }

          if (NEXUS_Heap_GetStatus( heap, &memoryStatus ) != NEXUS_SUCCESS)
              return DFB_FAILURE;

          config->preallocated[i].handle = heap;
          config->preallocated[i].offset = (long) description->preallocated[i].data - (long) memoryStatus.addr;
          config->preallocated[i].pitch  = description->preallocated[i].pitch;
     }

     return DFB_OK;
}

static DFBResult
bcmAllocateBuffer( CoreSurfacePool       *pool,
          UNUSED_  void                  *pool_data,
                   void                  *pool_local,
                   CoreSurfaceBuffer     *buffer,
                   CoreSurfaceAllocation *allocation,
                   void                  *alloc_data )
{
     DFBResult                     result;
     struct bcmAllocationData     *ad   = alloc_data;
     struct bcmPoolLocal          *pl   = pool_local;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );

     D_DEBUG_AT( bcmnexusPool,
                 CYAN_COLOUR"%s() surface=%p, buffer=%p, allocation=%p, surface type=0x%08x, caps=0x%08x, size=%dx%d, format=%s, policy=0x%08x [master=%c]\n"
                 NO_COLOUR, __FUNCTION__, (void *)buffer->surface, (void *)buffer,  (void *)allocation, allocation->type, allocation->config.caps,
                 allocation->config.size.w, allocation->config.size.h, dfb_pixelformat_name(buffer->format), buffer->policy,
                 dfb_core_is_master( dfb_bcmnexus_core ) ? 'y' : 'n' );

     /* allocate the NEXUS surface */
     result = createNexusSurface( pl->core, allocation, ad, pl );
     if (result == DFB_OK) {
          allocation->size = ad->size;
          if (!((buffer->type & CSTF_LAYER) && (buffer->resource_id == (DLID_PRIMARY+2))))
          {
              D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() allocation=%d, flush handle=%p\n"NO_COLOUR, __FUNCTION__,
                         allocation->size, (void *)ad->bcmSurfaceHandle);

              NEXUS_Surface_Flush(ad->bcmSurfaceHandle);
          }
     }
     return result;
}

static DFBResult
bcmDeallocateBuffer( CoreSurfacePool       *pool,
                     void                  *pool_data,
             UNUSED_ void                  *pool_local,
             UNUSED_ CoreSurfaceBuffer     *buffer,
                     CoreSurfaceAllocation *allocation,
                     void                  *alloc_data )
{
     struct bcmAllocationData     *ad   = alloc_data;
     struct bcmPoolData           *pd   = pool_data;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );

     D_DEBUG_AT( bcmnexusPool,
                 CYAN_COLOUR"%s() handle=%p [addr: %p], type=0x%08x, size=%dx%d, format=%s [master=%c]\n"NO_COLOUR,
                 __FUNCTION__, (void *)ad->bcmSurfaceHandle, ad->address, allocation->type,
                 allocation->config.size.w, allocation->config.size.h, dfb_pixelformat_name(allocation->config.format),
                 dfb_core_is_master( dfb_bcmnexus_core ) ? 'y' : 'n' );

     /* deallocate the NEXUS surface */
     if (ad->bcmSurfaceHandle) {

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_MULTIBUFFER_MODE
         if (allocation &&
                allocation->type & CSTF_LAYER
                && allocation->resource_id == DLID_PRIMARY)
         {
             D_DEBUG_AT( bcmnexusPool,
                         CYAN_COLOUR"%s() Clearing NSC queue \n"NO_COLOUR,__FUNCTION__);
             DFB_Platform_P_ClearDisplayGraphicsFramebuffer3D(GET_DISPLAY_HND(pd->bcmnexus, PRIMARY_DISPLAY_ID));
         }
#endif

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
         if (allocation &&
                allocation->type & CSTF_LAYER
                && allocation->resource_id == DLID_PRIMARY)
         {
             D_DEBUG_AT( bcmnexusPool, "releasing tunnel surface %#lx\n", (unsigned long)ad->bcmSurfaceHandle);
             NEXUS_SurfaceClient_ReleaseTunneledSurface(GET_DISPLAY_HND(pd->bcmnexus,0),ad->bcmSurfaceHandle);
             /* BDBG_ASSERT(!rc); */
             ad->bcmSurfaceHandle = NULL;
             goto done;
         }
#endif
          /* Ensure the graphics h/w has finished before removing memory! */
#if NEXUS_HAS_GRAPHICS2D || NEXUS_HAS_ZSP_GRAPHICS
          if ((ad->syncIssued == false) || GET_GRAPHICS2D_SYNC(pd->bcmnexus))
#else
          if (true)
#endif
          {
              D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() sync\n"NO_COLOUR, __FUNCTION__);
              dfb_gfxcard_sync();
          } else {
              ad->syncIssued = false;
          }

          if (!((allocation->type & CSTF_LAYER) && (allocation->resource_id == (DLID_PRIMARY+2))))
          {
              D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() flush handle=%p\n"NO_COLOUR, __FUNCTION__, (void *)ad->bcmSurfaceHandle);
              NEXUS_Surface_Flush(ad->bcmSurfaceHandle);
          }


          /* Free up the palette if it was created */
          if (ad->palette != NULL) {
              D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() About to free palette memory %p\n"NO_COLOUR, __FUNCTION__, (void *)ad->palette );
              NEXUS_Memory_Free(ad->palette);
              ad->palette = NULL;
          }

          /* If not a pre-allocated surface, then we can free up the Nexus memory */
#ifdef PREALLOCATE_MEMORY
          if (!(allocation->type & CSTF_PREALLOCATED))
          {
              NEXUS_SurfaceMemory mem;

              D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() About to get surface handle %p memory\n"NO_COLOUR, __FUNCTION__, (void *)ad->bcmSurfaceHandle );
              NEXUS_Surface_GetMemory(ad->bcmSurfaceHandle, &mem);
              D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() About to free buffer memory %p\n"NO_COLOUR, __FUNCTION__, (void *)mem.buffer );
              NEXUS_Memory_Free(mem.buffer);
          }
#endif
          NEXUS_Surface_Destroy(ad->bcmSurfaceHandle );


          ad->bcmSurfaceHandle = NULL;
     }

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
  done:
#endif

     D_DEBUG_AT( bcmnexusPool, CYAN_COLOUR"%s() exiting.\n"NO_COLOUR, __FUNCTION__ );
     return DFB_OK;
}

static DFBResult
bcmPreLock(         CoreSurfacePool        *pool,
                    void                   *pool_data,
            UNUSED_ void                   *pool_local,
                    CoreSurfaceAllocation  *allocation,
                    void                   *alloc_data,
                    CoreSurfaceAccessorID   accessor,
                    CoreSurfaceAccessFlags  access )
{
     struct bcmPoolData       *pd       = pool_data;
     struct bcmAllocationData *ad       = alloc_data;
     bool                      flush_me = false;
     bool                      sync_me  = false;
     int                       locks;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );

     locks = dfb_surface_allocation_locks( allocation );

     D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() surface=%p, allocation=%p, addr=%p, pitch=%d, handle=%p accessor=%d, access=0x%08x, CPU accessed=0x%08x, "
                                "GPU accessed=0x%08x, type=0x%08x, flags=0x%08x, caps=0x%08x, locks=%d\n"NO_COLOUR, __FUNCTION__,
                                (void *)allocation->surface, (void *)allocation, ad->address, ad->pitch, (void*)ad->bcmSurfaceHandle, accessor, access,
                                allocation->accessed[CSAID_CPU], allocation->accessed[CSAID_GPU], allocation->type, allocation->config.flags,
                                allocation->config.caps, locks );

     /* Check Layer lock */
     if (accessor == (CSAID_LAYER0 + allocation->resource_id))
     {
          /* ...if software has written before... */
          if (allocation->accessed[CSAID_CPU] & CSAF_WRITE) {
              flush_me = true;
              /* Clear CPU W flags to avoid unwanted flushes */
              if (!locks)
                   allocation->accessed[CSAID_CPU] &= ~CSAF_WRITE;
          }

          /* If hardware has written or is writing... */
          if (allocation->accessed[CSAID_GPU] & CSAF_WRITE) {
               sync_me = true;
               flush_me = true;
               /* Clear GPU R flag to avoid unwanted flushes and syncs */
               if (!locks)
                   allocation->accessed[CSAID_GPU] &= ~CSAF_WRITE;
          }
     }

     /* Software read/write access... */
     if (accessor == CSAID_CPU) {
          /* If hardware has written or is writing... */
          if (allocation->accessed[CSAID_GPU] & CSAF_WRITE) {
               /* ...wait for the operation to finish. */
               sync_me = true;
               /* Software read access after hardware write requires flush of the (bus) read cache. */
               flush_me = true;

               if (!locks) {
                    /* ...clear hardware write access. */
                    allocation->accessed[CSAID_GPU] &= ~CSAF_WRITE;
                    /* ...clear hardware read access (to avoid syncing twice). */
                    allocation->accessed[CSAID_GPU] &= ~CSAF_READ;
               }
          }

          /* Software write access... */
          if (access & CSAF_WRITE) {
               /* ...if hardware has (to) read... */
               if (allocation->accessed[CSAID_GPU] & CSAF_READ) {
                    /* ...wait for the operation to finish. */
                    sync_me = true;

                    /* ...clear hardware read access. */
                    if (!locks)
                         allocation->accessed[CSAID_GPU] &= ~CSAF_READ;
               }
          }
     }

     /* Hardware read access... */
     if (accessor == CSAID_GPU) {
          /* ...if software has written before... */
          if (allocation->accessed[CSAID_CPU] & CSAF_WRITE) {
               /* ...flush texture cache. */
               flush_me = true;

               /* ...clear software write access. */
               if (!locks)
                    allocation->accessed[CSAID_CPU] &= ~CSAF_WRITE;
          }
     }

     /* Do we need to perform a graphics sync operation? */
     if (sync_me == true)
     {
#if NEXUS_HAS_GRAPHICS2D || NEXUS_HAS_ZSP_GRAPHICS
         if ((ad->syncIssued == false) || GET_GRAPHICS2D_SYNC(pd->bcmnexus))
#else
         if (true)
#endif
         {
             D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() sync\n"NO_COLOUR, __FUNCTION__);
             dfb_gfxcard_sync();
         } else {
             ad->syncIssued = false;
         }
     }

     /* Do we need to flush the surface cache? */
     if (flush_me == true) {
          if (!((allocation->type & CSTF_LAYER) && (allocation->resource_id == (DLID_PRIMARY+2))))
          {
              D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() flush handle=%p\n"NO_COLOUR, __FUNCTION__, (void *)ad->bcmSurfaceHandle);
              NEXUS_Surface_Flush(ad->bcmSurfaceHandle);
          }
     }

     D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() Exit\n"NO_COLOUR, __FUNCTION__ );
     return DFB_OK;

}

static DFBResult
bcmLock(         CoreSurfacePool       *pool,
         UNUSED_ void                  *pool_data,
         UNUSED_ void                  *pool_local,
                 CoreSurfaceAllocation *allocation,
                 void                  *alloc_data,
                 CoreSurfaceBufferLock *lock )
{
     struct bcmAllocationData *ad       = alloc_data;

     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
     D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );

     D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() surface=%p, allocation=%p, addr=%p, pitch=%d, handle=%p accessor=%d, access=0x%08x, CPU accessed=0x%08x, "
                                "GPU accessed=0x%08x, type=0x%08x, flags=0x%08x, caps=0x%08x\n"NO_COLOUR,
                 __FUNCTION__, (void *)allocation->surface, (void *)allocation, ad->address, ad->pitch, (void*)ad->bcmSurfaceHandle, lock->accessor,
                 lock->access, allocation->accessed[CSAID_CPU], allocation->accessed[CSAID_GPU], allocation->type, allocation->config.flags, allocation->config.caps );

     /* Setup handle to surface so that graphics driver can access it */
     lock->handle = ad->bcmSurfaceHandle;
     lock->addr   = ad->address;
     lock->phys   = (unsigned long)ad->hwaddress;
     lock->offset = 0; /* irrelevant */
     lock->pitch  = ad->pitch; /* ad->pitch */


     /* For secure fusion applications we need to do some address mapping as the virtual address space for each process is different */
     if (fusion_config->secure_fusion && !dfb_core_is_master(dfb_bcmnexus_core) )
     {
         lock->addr = NEXUS_OffsetToCachedAddr(ad->hwaddress);

         D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() Secure fusion client fixing up virtual address from %p to %p \n"NO_COLOUR,
                     __FUNCTION__, ad->address, lock->addr);
     }

     D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() Exit\n"NO_COLOUR, __FUNCTION__ );
     return DFB_OK;

}

static DFBResult
bcmUnlock(         CoreSurfacePool       *pool,
           UNUSED_ void                  *pool_data,
           UNUSED_ void                  *pool_local,
                   CoreSurfaceAllocation *allocation,
                   void                  *alloc_data,
                   CoreSurfaceBufferLock *lock )
{
     D_MAGIC_ASSERT( pool, CoreSurfacePool );
     D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
     D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );

     D_DEBUG_AT( bcmnexusPoolX, BLUE_COLOUR"%s() surface=%p, allocation=%p, addr=%p, pitch=%d, handle=%p accessor=%d, access=0x%08x, CPU accessed=0x%08x, "
                 "GPU accessed=0x%08x, type=0x%08x, flags=0x%08x, caps=0x%08x\n"NO_COLOUR, __FUNCTION__, (void *)allocation->surface, (void *)allocation,
                 ((struct bcmAllocationData*)alloc_data)->address, ((struct bcmAllocationData*)alloc_data)->pitch,
                 (void*)((struct bcmAllocationData*)alloc_data)->bcmSurfaceHandle, lock->accessor, lock->access,
                 allocation->accessed[CSAID_CPU], allocation->accessed[CSAID_GPU], allocation->type, allocation->config.flags, allocation->config.caps );

     return DFB_OK;
}

const SurfacePoolFuncs bcmnexusPoolFunctions = {
     .PoolDataSize       = bcmPoolDataSize,
     .PoolLocalDataSize  = bcmPoolLocalDataSize,
     .AllocationDataSize = bcmAllocationDataSize,
     .InitPool           = bcmInitPool,
     .JoinPool           = bcmJoinPool,
     .DestroyPool        = bcmDestroyPool,
     .LeavePool          = bcmLeavePool,
     .TestConfig         = bcmTestConfig,
     .PreAlloc           = bcmPreAlloc,
     .AllocateBuffer     = bcmAllocateBuffer,
     .DeallocateBuffer   = bcmDeallocateBuffer,
     .PreLock            = bcmPreLock,
     .Lock               = bcmLock,
     .Unlock             = bcmUnlock
};

const SurfacePoolFuncs bcmnexusGraphics3DPoolFunctions = {
     .PoolDataSize       = bcmPoolDataSize,
     .PoolLocalDataSize  = bcmPoolLocalDataSize,
     .AllocationDataSize = bcmAllocationDataSize,
     .InitPool           = bcmInitGraphics3DPool,
     .JoinPool           = bcmJoinGraphics3DPool,
     .DestroyPool        = bcmDestroyGraphics3DPool,
     .LeavePool          = bcmLeaveGraphics3DPool,
     .TestConfig         = bcmTestGraphics3DConfig,
     .PreAlloc           = bcmPreAlloc,
     .AllocateBuffer     = bcmAllocateBuffer,
     .DeallocateBuffer   = bcmDeallocateBuffer,
     .PreLock            = bcmPreLock,
     .Lock               = bcmLock,
     .Unlock             = bcmUnlock
};
