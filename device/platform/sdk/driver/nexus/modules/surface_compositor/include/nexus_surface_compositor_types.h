/***************************************************************************
 *     (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_surface_compositor_types.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 8/6/12 1:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/include/nexus_surface_compositor_types.h $
 * 
 * 12   8/6/12 1:30p erickson
 * SW7425-3565: add NEXUS_SurfaceComposition.contentMode
 * 
 * 11   1/26/12 5:22p erickson
 * SW7425-2116: add NEXUS_SurfaceComposition.colorMatrix
 * 
 * 10   1/10/12 3:38p vsilyaev
 * SW7420-2195: Added clipping for the client surface, currently supported
 *  for video surfaces only
 * 
 * 9   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 * 
 * 8   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 * 
 * 7   10/18/11 12:32p vsilyaev
 * SW7420-1992: Moved virtual display into the composition settings
 * 
 * 6   10/4/11 5:52p vsilyaev
 * SW7425-1364: Added ClientSurfaceId
 * 
 * 5   9/1/11 2:07p vsilyaev
 * SW7420-1992: Updated API
 * 
 * 4   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 * 
 * 3   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
 * 
 * 1   7/26/11 1:34p erickson
 * SW7420-1992: add surface_compositor module and examples
 * 
 **************************************************************************/
#ifndef NEXUS_SURFACE_COMPOSITOR_TYPES_H__
#define NEXUS_SURFACE_COMPOSITOR_TYPES_H__

#include "bstd.h"
#include "nexus_types.h"
#include "nexus_graphics2d.h"
#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
surface compositor client
**/
typedef struct NEXUS_SurfaceClient *NEXUS_SurfaceClientHandle;

/**
Summary:
surface compositor region
**/
typedef struct NEXUS_SurfaceRegion {
    uint16_t width;
    uint16_t height;
} NEXUS_SurfaceRegion;

/**
composition settings of each surface
**/
typedef struct NEXUS_SurfaceComposition
{
    NEXUS_SurfaceRegion virtualDisplay; /* if width/height non-zero, composition.position will be positioned and scaled relative to the virtualDisplay dimensions. 
                                  otherwise, it will be positioned and scaled relative to the actual display dimensions. */
    NEXUS_Rect position; /* position and size of the surface relative to the size of the parent. scaling is automatic. 
                            if rectangle is offscreen, the surface will be clipped appropriately. */
    NEXUS_Rect clipRect; /* source clipping rectangle in the coordinates of virtualDisplay
                            if set to all zeros, no source clipping will be performed.
                            negative source clipping (zoom out) is not supported.*/

    unsigned zorder; /* relative z-order to other surfaces added to this surface_compositor. 0 is on bottom. */
    bool visible; /* if true, this surface will be included in the next compositing operation. */
    bool displayCache; /* save copy of scaled client per display. this avoids slow scaled blits if surface will not be updated or resized frequently. costs memory. */
    NEXUS_BlendEquation colorBlend;  /* Specify how to compose the color component */
    NEXUS_BlendEquation alphaBlend;   /* Specify how to compose the alpha component */
    NEXUS_Pixel constantColor;       /*  Constant alpha and color. */
    int rightViewOffset; /* if display3D, it controls z-offset (in units of virtualDisplay.width) (in addition to any z-offset for the framebuffer) */
    
    bool colorMatrixEnabled; /* set true to use colorMatrix */
    NEXUS_Graphics2DColorMatrix colorMatrix; /* see NEXUS_Graphics2D_SetAlphaColorMatrix for simple matrix-based alpha blend */
    NEXUS_VideoWindowContentMode contentMode;
} NEXUS_SurfaceComposition;

/**
Identificator that uniquely identifies client surface
**/
typedef unsigned NEXUS_SurfaceCompositorClientId;


#ifdef __cplusplus
}
#endif

#endif
