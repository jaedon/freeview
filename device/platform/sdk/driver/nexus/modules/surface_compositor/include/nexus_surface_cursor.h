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
 * $brcm_Workfile: nexus_surface_cursor.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/13/12 4:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/include/nexus_surface_cursor.h $
 * 
 * 2   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 1   10/24/11 5:40p vsilyaev
 * SW7420-1992: Updated cursor support
 * 
 * 16   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 * 
 * 15   10/20/11 1:56p erickson
 * SW7420-1992: embed NEXUS_VideoWindowSettings in
 *  NEXUS_SurfaceCompositorDisplaySettings
 * 
 * 14   10/18/11 12:32p vsilyaev
 * SW7420-1992: Moved virtual display into the composition settings
 * 
 * 13   10/13/11 3:33p erickson
 * SW7420-1992: add display format change support
 * 
 * 12   10/4/11 5:52p vsilyaev
 * SW7425-1364: Added ClientSurfaceId
 * 
 * 11   10/3/11 9:25a erickson
 * SW7420-1148: add video window support
 * 
 * 10   9/7/11 7:13p vsilyaev
 * SW7420-1148: Decouple rendering of primary and other displays. Other
 *  displays would get just a copy of result rendered on primary display.
 * 
 * 9   9/1/11 2:07p vsilyaev
 * SW7420-1992: Updated API
 * 
 * 8   8/26/11 5:36p vsilyaev
 * SW7420-1992: Updated API
 * 
 * 7   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 * 
 * 6   8/22/11 4:11p vsilyaev
 * SW7420-1992: Updated implementation of surface compositor
 * 
 * 5   8/5/11 3:58p erickson
 * SW7420-1992: server can set number of framebuffers per display. revise
 *  tunneling api accordingly.
 * 
 * 4   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
 * 
 * 3   8/4/11 10:41a erickson
 * SW7420-1992: implement direct mode
 * 
 * 2   8/3/11 9:57a erickson
 * SW7420-1992: add per-display framebuffer heap to
 *  NEXUS_SurfaceCompositorSettings
 * 
 * 1   7/26/11 1:34p erickson
 * SW7420-1992: add surface_compositor module and examples
 * 
 **************************************************************************/
#ifndef NEXUS_SURFACE_CURSOR_H__
#define NEXUS_SURFACE_CURSOR_H__

#include "nexus_surface_compositor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
surface compositor cursor;
**/
typedef struct NEXUS_SurfaceCursor *NEXUS_SurfaceCursorHandle;

/**
Summary:
Static configuration of NEXUS_SurfaceCursor
**/
typedef struct NEXUS_SurfaceCursorCreateSettings {
    struct {
        NEXUS_SurfaceRegion maxSize; /* maximum cursor size, in framebuffer pixels */
    } display[NEXUS_MAX_DISPLAYS]; 
    NEXUS_SurfaceHandle surface; 
} NEXUS_SurfaceCursorCreateSettings;

/**
Summary:
Dynamic configuration of NEXUS_SurfaceCursor
**/
typedef struct NEXUS_SurfaceCursorSettings {
    NEXUS_SurfaceComposition composition;
} NEXUS_SurfaceCursorSettings;

/**
Summary:
**/
void NEXUS_SurfaceCursor_GetDefaultCreateSettings(
    NEXUS_SurfaceCursorCreateSettings *createSettings
   );

/**
Summary:
**/
NEXUS_SurfaceCursorHandle NEXUS_SurfaceCursor_Create( /* attr{destructor=NEXUS_SurfaceCursor_Destroy} */
    NEXUS_SurfaceCompositorHandle surfaceCompositor,
    const NEXUS_SurfaceCursorCreateSettings *createSettings 
   );

/**
Summary:
**/
void NEXUS_SurfaceCursor_Destroy(
    NEXUS_SurfaceCursorHandle  cursor
    );

/**
Summary:
**/
void NEXUS_SurfaceCursor_GetSettings (
    NEXUS_SurfaceCursorHandle  cursor,
    NEXUS_SurfaceCursorSettings *settings
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SurfaceCursor_SetSettings (
    NEXUS_SurfaceCursorHandle  cursor,
    const NEXUS_SurfaceCursorSettings *settings
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_SURFACE_CURSOR_H__ */

