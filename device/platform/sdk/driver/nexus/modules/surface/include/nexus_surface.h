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
 * $brcm_Workfile: nexus_surface.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 1/5/12 12:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface/7400/include/nexus_surface.h $
 * 
 * 23   1/5/12 12:27p erickson
 * SW7435-30: add NEXUS_Surface_InitPlaneAndPaletteOffset
 * 
 * 22   3/8/11 5:11p erickson
 * SW7420-1575: add NEXUS_Surface_InitPlane for packet blit verification
 *
 * 21   2/3/11 6:37p vsilyaev
 * SW7420-1441: Use local (non proxied) implementation for function that
 *  flush CPU cache
 *
 * 20   10/22/10 2:47p erickson
 * SW7420-1205: deprecate NEXUS_SurfaceSettings.autoFlush because of
 *  inherent performance problems
 *
 * 19   10/7/10 11:04a erickson
 * SW7420-1148: add destructor hint
 *
 * 18   8/6/10 1:08p erickson
 * SW7420-703: NEXUS_Surface_GetCreateSettings
 *
 * 17   6/23/10 12:14p erickson
 * SW7550-453: add nexus_striped_surface.h interface
 *
 * 16   1/21/10 9:54a erickson
 * SW7405-3787: add comments to NEXUS_Surface_Flush re: cache coherency
 *  problems caused by the RAC
 *
 * 15   3/31/09 10:05a erickson
 * PR53661: remove NEXUS_SurfaceSettings.constantColor. it was unused. the
 *  intended functionality can be accomplished using the constantColor in
 *  NEXUS_Graphics2DBlitSettings.
 *
 * 14   1/30/09 4:26p erickson
 * PR46300: removed NEXUS_Surface_CreateSpecial
 *
 * 13   1/13/09 6:11p erickson
 * PR46300: added NEXUS_Surface_CreateSpecial
 *
 * 12   12/30/08 9:17a erickson
 * PR50743: added NEXUS_SurfaceCreateSettings.pPaletteMemory
 *
 * 11   11/26/08 9:59a erickson
 * PR49649: added NEXUS_SurfaceCreateSettings.heap option
 *
 * 10   10/31/08 12:26p erickson
 * PR48586: update comment
 *
 * 9   8/28/08 10:42a erickson
 * PR46111: add palettePixelFormat
 *
 * 8   8/21/08 5:14p erickson
 * PR45941: added NEXUS_StripedSurface_GetStatus
 *
 * 7   4/30/08 10:52a erickson
 * PR35457: improve Surface comments. move general items to nexus_types.h.
 *
 * 6   3/26/08 12:51p erickson
 * PR40567: moved NEXUS_PixelFormat to nexus_types.h
 *
 * PR40567/2   3/26/08 6:23p dyzhang
 * PR40567: move NEXUS_PixelFormat from nexus_surface.h to nexus_types.h
 *
 * PR40567/1   3/26/08 4:26p xhuang
 * PR40567:add pixel format set in Nexus
 *
 * 5   2/21/08 1:44p erickson
 * PR39786: add pixel format macros
 *
 * 4   1/30/08 4:59p erickson
 * PR36808: move NEXUS_Pixel to Base
 *
 * 3   1/28/08 12:33p vsilyaev
 * PR 38682: Added support for attribute tags applied to structure
 *  members.
 *
 * 2   1/23/08 9:46a erickson
 * PR35198: clarify comments
 *
 * 1   1/18/08 2:15p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#ifndef NEXUS_SURFACE_H__
#define NEXUS_SURFACE_H__

#include "nexus_types.h"
#include "bm2mc_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=*******************************************
The Surface module allows the creation and management of graphics surfaces in memory.
These surfaces can be used by other Modules such as Graphics2D, Display, VideoDecoder and PictureDecoder.
*********************************************/

/**
Summary:
Handle for the Surface interface.

Description:
A graphics surface is a block of memory with various properties include width, height, pitch, and pixel format.

See Also:
NEXUS_Surface_GetMemory - used for host to write directly to memory
NEXUS_Display_SetGraphicsFramebuffer - set the current graphics framebuffer
NEXUS_Graphics2DFillSettings - fill a surface with a pixel
NEXUS_Graphics2DBlitSettings - blit from one surface to another
**/
typedef struct NEXUS_Surface *NEXUS_SurfaceHandle;

/*
Summary:
Information about the memory used by a surface.

Description:
Returned by NEXUS_Surface_GetMemory
*/
typedef struct NEXUS_SurfaceMemory
{
    void *buffer; /*  attr{memory=cached}  Pointer to cached memory. Directly accessible by the application.
                        This must be typecasted to the correct pixel format. For example
                        (unsigned short *) for a 16 bit pixel format or (unsigned long *) for a 32 bit pixel format. */
    unsigned int pitch; /* The memory width of the surface in bytes.
                            Pitch is based on the number of bytes used for all the pixels in one line along with an optional
                            padding on the right side. This padding maybe required for a variety of reasons (e.g. word alignment).
                            Pitch must be used to calculate the next line of the surface.
                            See the example in the NEXUS_Surface_GetMemory documentation. */

    NEXUS_Pixel *palette; /* attr{memory=cached}  This points to the palette memory for palette pixel formats. Otherwise NULL.
                             The palette pixel format is always ARGB8888.
                             The app can update the palette memory directly. After making a change, call NEXUS_Surface_Flush to
                             flush cached memory. If this surface is the framebuffer, you must also call NEXUS_Display_SetGraphicsFramebuffer
                             to read the updated palette into the graphics feeder. */
    unsigned numPaletteEntries; /* number of entries in palette */
    size_t  bufferSize;     /* size of the surface buffer */
} NEXUS_SurfaceMemory;

/*
Summary:
Settings needed to create a surface.

Description:
These cannot be changed after the surface has been created.
For palettized surfaces, a default palette of all black will be created.
*/
typedef struct NEXUS_SurfaceCreateSettings
{
    NEXUS_PixelFormat pixelFormat; /* pixel format of the surface. */
    NEXUS_PixelFormat palettePixelFormat; /* pixel format of NEXUS_SurfaceMemory.palette if pixelFormat is a palette format.
                                             Typically NEXUS_PixelFormat_eA8_R8_G8_B8 or NEXUS_PixelFormat_eA8_Y8_Cb8_Cr8 are supported. */
    uint16_t width;            /* visible width of the surface in pixels. */
    uint16_t height;           /* visible height of the surface in pixels. */

    unsigned int alignment;    /* optional buffer alignment specified as a power of 2, measured in bytes.
                                  0 is no alignment (default), 1 is 2-byte aligned, 2 is 4-byte aligned, etc. */

    unsigned int pitch;        /* optional buffer pitch, measured in bytes. 0 is the default pitch (width * sizeof(pixel)). */

    void *pMemory;             /* attr{memory=cached} Device memory address to use for the surface. Must be allocated using NEXUS_Memory_Allocate().
                                  If NULL, Nexus will allocate.
                                  Size of user allocated buffer must be >= height * pitch, where pitch >= width * sizeof(pixel); otherwise there will be a memory overrun. */
    void *pPaletteMemory;      /* attr{memory=cached} Device memory address to use for the palette. Must be allocated using NEXUS_Memory_Allocate().
                                  If NULL, Nexus will allocate.
                                  Size of user allocated buffer must be >= sizeof(NEXUS_Pixel) * expected NEXUS_SurfaceMemory.numPaletteEntries; otherwise there will be a memory overrun.
                                  Palette memory must be 32 byte aligned. */

    NEXUS_HeapHandle heap;     /* Optional handle for memory heap. If NULL, the surface and palette will be allocated from heap[0]. */
    unsigned bitsPerPixel;
} NEXUS_SurfaceCreateSettings;

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_Surface_Create
*/
void NEXUS_Surface_GetDefaultCreateSettings(
    NEXUS_SurfaceCreateSettings *pCreateSettings /* [out] */
    );

/**
Summary:
Creates a new surface using the create settings.

Description:
A surface is a block of memory with the properties of height, width, pitch and pixel format.

The user is responsible for call NEXUS_Surface_Destroy for any surface which is created.
Surfaces are not automatically freed by another Interface.

Memory is allocated from the memory heap provided by the Core module.

This function returns NULL if there is no enough memory for the surface.

See Also:
NEXUS_Surface_GetDefaultCreateSettings
NEXUS_Surface_GetCreateSettings
NEXUS_Surface_Destroy
**/
NEXUS_SurfaceHandle NEXUS_Surface_Create( /* attr{destructor=NEXUS_Surface_Destroy}  */
    const NEXUS_SurfaceCreateSettings *pCreateSettings
    );

/**
Summary:
Destroy a surface and free its resources.

Description:
This surface should not be currently in use by another module.
After calling NEXUS_Surface_Destroy, the handle is no longer valid and should not be used.

If you have written to cached memory and have not flushed, you should call NEXUS_Surface_Flush
before deleting this surface. This cannot be done manually without adversely impacting performance.

See Also:
NEXUS_Surface_Create
**/
void NEXUS_Surface_Destroy(
    NEXUS_SurfaceHandle surface
    );

/*
Summary:
Get settings used to create the surface.

Description:
NEXUS_SurfaceCreateSettings.heap will be populated with the default heap if the user passed in NULL.
*/
void NEXUS_Surface_GetCreateSettings(
    NEXUS_SurfaceHandle surface,
    NEXUS_SurfaceCreateSettings *pCreateSettings /* [out] */
    );

/**
Summary:
Returns information needed for direct memory writes.

Description:
In order for direct memory writes to be successful, you must know the
format specified by NEXUS_PixelFormat.
You cannot assume that pitch == width * sizeof(pixelformat).

Example:
    //assuming 32bpp surface
    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);
    for (y=0;y<createSettings.height;y++) {
        for (x=0;x<createSettings.width;x++) {
            ((uint32_t*)mem.buffer)[y*mem.pitch + x] = value;
        }
    }
    NEXUS_Surface_Flush(surface);

See Also:
NEXUS_Surface_Flush
**/
void NEXUS_Surface_GetMemory(
    NEXUS_SurfaceHandle surface,
    NEXUS_SurfaceMemory *pMemory /* [out] */
    );

/**
Summary:
Flush a surface's cached memory and palette

Description:
If you are doing direct memory writes to the surface, you should flush before doing any hardware read/write
operations (e.g. Graphics2D blit/fill or setting the Display framebuffer).

Also, if you are doing direct memory reads from a surface, you should also flush after doing any hardwware write
operations. See the module overview in nexus_dma.h for a full discussion of cache coherency problems caused
by the RAC (read-ahead cache).

If you have updated the palette and this surface is the framebuffer, you must also call NEXUS_Display_SetGraphicsFramebuffer
to read the updated palette into the graphics feeder.
**/
void NEXUS_Surface_Flush( /* attr{local=true}  */
    NEXUS_SurfaceHandle surface
    );

/*
Summary:
Settings for a surface returned by NEXUS_Surface_GetSettings.
*/
typedef struct NEXUS_SurfaceSettings
{
    bool autoFlush; /* deprecated. Minimal cache flush is essential for performance; therefore app should call NEXUS_Surface_Flush manually
                       and only when needed. See NEXUS_Surface_Flush for notes. */
} NEXUS_SurfaceSettings;

/**
Summary:
Apply NEXUS_SurfaceSettings to a surface.
**/
NEXUS_Error NEXUS_Surface_SetSettings(
    NEXUS_SurfaceHandle surface,
    const NEXUS_SurfaceSettings *pSettings
    );

/*
Summary:
Get NEXUS_SurfaceSettings for a surface.
*/
void NEXUS_Surface_GetSettings(
    NEXUS_SurfaceHandle surface,
    NEXUS_SurfaceSettings *pSettings /* [out] */
    );

/*
Summary:
Initialize a BM2MC_PACKET_Plane for use in NEXUS_Graphics2D packet blit

Description:
This will set the required address, format and pitch for the blit.
It will also set the optional width and height so that export NEXUS_GRAPHICS2D_VERIFY_PACKETS=y
can verify the source, dest and output rectangle bounds.
*/
void NEXUS_Surface_InitPlane(
    NEXUS_SurfaceHandle surface,
    BM2MC_PACKET_Plane *pPlane /* [out] */
    );

/*
Summary:
Initialize a BM2MC_PACKET_Plane for use in NEXUS_Graphics2D packet blit and also retrieve palette offset
*/
void NEXUS_Surface_InitPlaneAndPaletteOffset(
    NEXUS_SurfaceHandle surface,
    BM2MC_PACKET_Plane *pPlane, /* [out] */
    unsigned *pPaletteOffset    /* [out] physical address if palette memory if application. 0 if non-palettized surface. */
    );
    
#ifdef __cplusplus
}
#endif

#endif
