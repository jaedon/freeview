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
 * $brcm_Workfile: nexus_graphics2d_destripe.c $
 * $brcm_Revision: 46 $
 * $brcm_Date: 7/18/12 3:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics2d/src/nexus_graphics2d_destripe.c $
 * 
 * 46   7/18/12 3:48p erickson
 * SW7425-3433: simplify dynamic PM for better performance
 * 
 * 45   1/5/12 12:28p erickson
 * SW7435-30: do not call surface module private API
 * 
 * 44   5/23/11 4:50p erickson
 * SW7420-1200: refactor grclib for optimal nexus use
 * 
 * 43   10/19/10 12:48p erickson
 * SW35230-1411: fix bottom field destripe
 * 
 * 42   8/17/10 4:22p erickson
 * SW7420-928: use NEXUS_VideoBufferType for striped surface
 *
 * 41   8/16/10 11:08a erickson
 * SW7405-3671: add packet blit API. switch to packet-based GRC
 *  implementation.
 *
 * 40   6/23/10 12:14p erickson
 * SW7550-453: refactor to use nexus_striped_surface.h interface
 *
 * 39   2/16/10 4:45p nissen
 * SW7405-3671: Fixed build warnings.
 *
 * 38   2/3/10 2:09p nissen
 * SW7405-3671: Added power down to packet destripe blit.
 *
 * 37   1/19/10 4:45p nissen
 * SW7405-3671: Added support for GRC packet blits.
 *
 * 36   11/26/09 3:15p jhaberf
 * SW35230-1: Added 35230 DTV chip support
 *
 * 35   11/19/09 4:21p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 34   10/2/09 6:57p lwhite
 * SW7468-6: Add 7468 support
 *
 * 33   8/19/09 11:13a mward
 * PR55545: Typo, missing (.
 *
 * 32   8/18/09 7:06p mward
 * PR55545: Add 7125 support.
 *
 * 31   8/12/09 11:21a vsilyaev
 * PR 57629: Replaced NEXUS_Surface_AutoFlush_priv with
 *  NEXUS_Surface_GetSurfaceAutoFlush_priv
 *
 * 30   8/10/09 6:03p vsilyaev
 * PR 57552: Use tagged tailquee to manage temporary surfaces, traverse
 *  list only after user called Checkpoint function
 *
 * 29   8/5/09 4:55p jrubio
 * PR55232: add 7342/7340 support
 *
 * 28   8/4/09 9:42a erickson
 * PR57260: added call to BGRClib_ResetState after direct access to GRC
 *
 * 27   7/29/09 4:18p nickh
 * PR56017: Add 7420B0 support for memory register naming changes
 *
 * 26   6/24/09 1:37p erickson
 * PR55968: added power management
 *
 * 25   3/11/09 9:20a erickson
 * PR53076: support NEXUS_Graphics2D_DestripeToSurface to an RGB surface
 *
 * 24   3/6/09 10:38a erickson
 * PR52859: check return code on BGRC_ResetState
 *
 * 23   2/9/09 12:30p erickson
 * PR51393: fix race condition with BGRC_IssueState and handle-
 *  >blitSurfaces
 *
 * 22   2/5/09 4:42p erickson
 * PR51876: fix memory leak
 *
 * 21   2/5/09 4:37p erickson
 * PR51866: fix warning
 *
 * 20   1/26/09 11:30a erickson
 * PR51468: global variable naming convention
 *
 * 19   1/26/09 9:50a erickson
 * PR51393: make NEXUS_Graphics2D_Destripe and NEXUS_Graphics2D_Memset32
 *  asynchronous like all other blits and fills
 *
 * 18   12/3/08 3:05p erickson
 * PR48963: set the stripe width per striped surface. this allows us to
 *  support multiple memory controllers and various stripe widths.
 *
 * 17   10/28/08 2:50p erickson
 * PR46066: fix 3548 B0 support
 *
 * 16   10/20/08 9:57a erickson
 * PR46527: extend NEXUS_Graphics2D_DestripeToSurface with optional
 *  pTargetRect
 *
 * 13   9/25/08 1:56p erickson
 * PR46066: 3548 B0 support
 *
 * 12   9/8/08 4:12p erickson
 * PR46430: allow destripe to a larger supplied surface
 *
 * 11   9/4/08 12:36p erickson
 * PR46430: added NEXUS_Graphics2D_DestripeToSurface
 *
 * 10   8/14/08 5:26p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 9   6/26/08 8:38a erickson
 * PR44159: impl two pass destripe for workaround
 *
 * 8   5/5/08 2:54p erickson
 * PR40777: handle interlaced stills using M2MC, not software
 *
 * 7   3/19/08 1:17p erickson
 * PR40307: fix systems with single MEMC
 *
 * 6   3/19/08 10:44a erickson
 * PR40307: implemented sw_destripe on 7405 non-uma
 *
 * 5   3/18/08 4:42p erickson
 * PR40307: replaced GetEnv with new internal variable from XVD
 *
 * 4   3/14/08 10:10a erickson
 * PR40316: added temp still_field env variable
 *
 * 3   3/7/08 5:35p erickson
 * PR40307: downgrade WRN to MSG
 *
 * 2   2/28/08 12:00p erickson
 * PR39786: added picMemIndex for 7400
 *
 * 1   2/25/08 2:59p erickson
 * PR39786: added NEXUS_Graphics2D_Destripe
 *
 **************************************************************************/
#include "nexus_graphics2d_module.h"
#include "nexus_graphics2d_impl.h"

BDBG_MODULE(nexus_graphics2d_destripe);

NEXUS_SurfaceHandle NEXUS_Graphics2D_Destripe( NEXUS_Graphics2DHandle handle, NEXUS_StripedSurfaceHandle stripedSurface )
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    BERR_Code rc = 0;
    NEXUS_StripedSurfaceCreateSettings stripedSurfaceCreateSettings;

    NEXUS_StripedSurface_GetCreateSettings(stripedSurface, &stripedSurfaceCreateSettings);

    BDBG_MSG(("Retrieve still %dx%d (striped %dx%d)", stripedSurfaceCreateSettings.imageWidth, stripedSurfaceCreateSettings.imageHeight, stripedSurfaceCreateSettings.stripedWidth, stripedSurfaceCreateSettings.lumaStripedHeight));
    if (stripedSurfaceCreateSettings.imageHeight == 0 || stripedSurfaceCreateSettings.imageWidth == 0) {
        BDBG_ERR(("invalid still image %dx%d", stripedSurfaceCreateSettings.imageHeight, stripedSurfaceCreateSettings.imageWidth));
        return NULL;
    }

    /* create a bsurface_t which wraps the BSUR_Handle used by VDC */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.height = stripedSurfaceCreateSettings.imageHeight;
    surfaceCreateSettings.width = stripedSurfaceCreateSettings.imageWidth;
    surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8;
    surface = NEXUS_Surface_Create(&surfaceCreateSettings);
    if (!surface) {rc=BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);return NULL;}

    rc = NEXUS_Graphics2D_DestripeToSurface(handle, stripedSurface, surface, NULL);
    if (rc) {
        rc = BERR_TRACE(rc);
        NEXUS_Surface_Destroy(surface);
        surface = NULL;
    }

    return surface;
}

NEXUS_Error NEXUS_Graphics2D_DestripeToSurface( NEXUS_Graphics2DHandle handle, NEXUS_StripedSurfaceHandle stripedSurface, NEXUS_SurfaceHandle targetSurface, const NEXUS_Rect *pTargetRect )
{
    BERR_Code err = BERR_SUCCESS;
    BM2MC_PACKET_Plane surSurface;
    unsigned paletteOffset;
    NEXUS_SurfaceCreateSettings targetSettings;
    uint32_t lumaOffset;
    uint32_t chromaOffset;
    bool interlaced;
    NEXUS_StripedSurfaceCreateSettings stripedSurfaceCreateSettings;

    NEXUS_StripedSurface_GetCreateSettings(stripedSurface, &stripedSurfaceCreateSettings);

    lumaOffset = NEXUS_AddrToOffset( stripedSurfaceCreateSettings.pLumaBuffer );
    chromaOffset = NEXUS_AddrToOffset( stripedSurfaceCreateSettings.pChromaBuffer );

    interlaced = (stripedSurfaceCreateSettings.bufferType == NEXUS_VideoBufferType_eTopField) || (stripedSurfaceCreateSettings.bufferType == NEXUS_VideoBufferType_eBotField);
    
    /* for bottom field, we must apply an offset to the start address */
    if (stripedSurfaceCreateSettings.bufferType == NEXUS_VideoBufferType_eBotField) {
        lumaOffset += stripedSurfaceCreateSettings.pitch;
        chromaOffset += stripedSurfaceCreateSettings.pitch;
    }

    NEXUS_Surface_GetCreateSettings( targetSurface, &targetSettings );

    NEXUS_Surface_InitPlaneAndPaletteOffset( targetSurface, &surSurface, &paletteOffset );

    err = BGRClib_Destripe_Blit( handle->grclib, &surSurface, (const BRect *)pTargetRect,
        lumaOffset, chromaOffset, interlaced ? stripedSurfaceCreateSettings.pitch * 2: stripedSurfaceCreateSettings.pitch,
        stripedSurfaceCreateSettings.imageWidth, interlaced ? stripedSurfaceCreateSettings.imageHeight / 2 : stripedSurfaceCreateSettings.imageHeight,
        stripedSurfaceCreateSettings.stripedWidth, stripedSurfaceCreateSettings.lumaStripedHeight, stripedSurfaceCreateSettings.chromaStripedHeight,
        NEXUS_PIXEL_FORMAT_IS_YCRCB(targetSettings.pixelFormat) ? NULL : g_NEXUS_ai32_Matrix_YCbCrtoRGB, 10);

    if( (err == BERR_OUT_OF_DEVICE_MEMORY) || (err == BERR_OUT_OF_SYSTEM_MEMORY) )
        return BERR_TRACE(NEXUS_GRAPHICS2D_QUEUE_FULL);

    return BERR_TRACE(err);
}
