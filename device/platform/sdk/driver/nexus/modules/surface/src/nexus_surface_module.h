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
 * $brcm_Workfile: nexus_surface_module.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 6/19/12 12:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface/7400/src/nexus_surface_module.h $
 * 
 * 9   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 8   10/7/11 1:49p vsilyaev
 * SW7420-2085: Added support for object reference counting
 * 
 * 7   5/23/11 1:34p erickson
 * SW7420-1200: add NEXUS_Surface_InitPlane_priv
 * 
 * 6   3/8/11 5:11p erickson
 * SW7420-1575: add NEXUS_Surface_InitPlane for packet blit verification
 *
 * 5   6/23/10 12:14p erickson
 * SW7550-453: add nexus_striped_surface.h interface
 *
 * 4   1/26/09 11:29a erickson
 * PR51468: global variable naming convention
 *
 * 3   11/26/08 9:59a erickson
 * PR49649: added NEXUS_SurfaceCreateSettings.heap option
 *
 * 2   11/7/08 9:43a erickson
 * PR46300: moved NEXUS_Surface to module.h to allow for extension
 *
 * 1   1/18/08 2:19p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/4   9/28/07 10:01a erickson
 * PR34419: 0.1 prerel review
 *
 * Nexus_Devel/3   9/26/07 1:35p vsilyaev
 * PR 34662: Added API to return magnum surface handle
 *
 * Nexus_Devel/2   9/21/07 1:20p erickson
 * PR35198: update
 *
 * Nexus_Devel/1   9/21/07 12:35p erickson
 * PR35198: initial impl
 *
 **************************************************************************/
#ifndef NEXUS_SURFACE_MODULE_H__
#define NEXUS_SURFACE_MODULE_H__

#include "nexus_surface_thunks.h"
#include "nexus_base.h"
#include "nexus_surface.h"
#include "nexus_striped_surface.h"
#if NEXUS_SURFACE_EXTENSION
#include "nexus_surface_extension.h"
#endif
#include "nexus_surface_init.h"
#include "priv/nexus_surface_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME surface
#define NEXUS_MODULE_SELF g_NEXUS_surfaceModule

/* global handle. there is no global data. */
extern NEXUS_ModuleHandle g_NEXUS_surfaceModule;

/* This is the implementation of NEXUS_SurfaceHandle */
struct NEXUS_Surface {
    NEXUS_OBJECT(NEXUS_Surface);
    NEXUS_SurfaceSettings settings;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceMemory mem;
    void *buffer; /* set to uncached addr if NEXUS allocated externally from magnum */
    unsigned offset;
    unsigned paletteOffset;

    /* Magnum data */
    BSUR_Surface_Handle surface;
    BSUR_Palette_Handle palette;
    BMEM_Heap_Handle heap; /* which heap this surface is allocated from */
};

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_StripedSurface);


#ifdef __cplusplus
}
#endif

#endif
