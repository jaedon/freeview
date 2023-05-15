/***************************************************************************
 *    (c)2005-2012 Broadcom Corporation
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
 * $brcm_Workfile: bcmnexus_utils.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 3/7/12 5:27p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_utils.c $
 * 
 * DirectFB_1_4_15_Port/2   3/7/12 5:27p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 * 
 * DirectFB_1_4_15_Port/2   10/5/11 12:27p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:24a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 5:07p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/10   3/17/11 12:39p robertwm
 * SW7422-359: DirectFB: Add support for A8 Palette8 (DSPF_ALUT8) support
 *  into DirectFB.
 * 
 * DirectFB_1_4_Port/9   2/9/11 4:27p robertwm
 * SW7422-252: DirectFB: Add ABGR pixelformat support to DirectFB.
 * 
 * DirectFB_1_4_Port/8   12/27/10 2:44p scchang
 * SW7550-527: Add dual output support on 7550
 * 
 * DirectFB_1_4_Port/7   10/12/10 2:54p robertwm
 * SW7550-589: DirectFB: Add LUT4 pixel format support to DirectFB.
 * 
 * DirectFB_1_4_Port/6   8/16/10 11:45p robertwm
 * SW7405-4762: DirectFB: Prevent potential seg fault in copy_palette().
 * 
 * DirectFB_1_4_Port/5   8/6/10 9:50p robertwm
 * SW7405-4726: DirectFB: bcmnexus_utils.c fails to compile with LATEST
 *  version of Nexus.
 * 
 * DirectFB_1_4_Port/4   7/26/10 5:32p jackf
 * SW35230-771: Added include for shim_nexus.h.
 * 
 * DirectFB_1_4_Port/3   5/27/10 9:12p robertwm
 * SW7420-784: DirectFB: SetPalette on a non-layer surface doesn't work.
 * 
 * DirectFB_1_4_Port/2   3/2/10 12:42p robertwm
 * SW3556-1069: DirectFB: Run-time dynamic linking between gfxdriver and
 *  system drivers not working
 * 
 * DirectFB_1_4_Port/3   1/25/10 10:25a robertwm
 * SW7405-3827: DirectFB: system module not including
 *  bcmnexus_gfx_defines.h.
 * 
 * DirectFB_1_4_Port/2   1/14/10 7:03p robertwm
 * SW3556-996: DirectFB: Implement ImageProvider for DirectFB-1.4.1 using
 *  SID.
 * 
 * DirectFB_1_4_Port/1   12/23/09 4:18a robertwm
 * SW7550-51: Support DirectFB on 7550.
 * 
 ***************************************************************************/

#include "bcmnexus_utils.h"
#include <nexus_memory.h>
#include <nexus_types.h>
#include <core/palette.h>
#include <gfx/convert.h>

NEXUS_PixelFormat
bcmnexus_getNexusPixelFormat(DFBSurfacePixelFormat format)
{
    NEXUS_PixelFormat eFormat;

    switch (format)
    {
    /* Constant Colour Formats */
#ifdef BCMNEXUS_PIXELFORMAT_A1_SUPPORTED
    case DSPF_A1:
        eFormat = NEXUS_PixelFormat_eA1;
        break;
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A4_SUPPORTED
    case DSPF_A4:
        eFormat = NEXUS_PixelFormat_eA4;
        break;
#endif
    case DSPF_A8:
        eFormat = NEXUS_PixelFormat_eA8;
        break;

    /* Palette Formats */
    case DSPF_LUT2:
        eFormat = NEXUS_PixelFormat_ePalette2;
        break;
    case DSPF_LUT4:
        eFormat = NEXUS_PixelFormat_ePalette4;
        break;
    case DSPF_LUT8:
        eFormat = NEXUS_PixelFormat_ePalette8;
        break;
    case DSPF_ALUT8:
        eFormat = NEXUS_PixelFormat_eA8_Palette8;
        break;

    /* 16-bit RGB formats */
#ifdef BCMNEXUS_PIXELFORMAT_X4_R4_G4_B4_SUPPORTED        
    case DSPF_RGB444:
        eFormat = NEXUS_PixelFormat_eX4_R4_G4_B4;
        break;
#endif
#ifdef BCMNEXUS_PIXELFORMAT_X1_R5_G5_B5_SUPPORTED
    case DSPF_RGB555:
        eFormat = NEXUS_PixelFormat_eX1_R5_G5_B5;
        break;
#endif
#ifdef BCMNEXUS_PIXELFORMAT_X1_B5_G5_R5_SUPPORTED
    case DSPF_BGR555:
        eFormat = NEXUS_PixelFormat_eX1_B5_G5_R5;
        break;
#endif
    case DSPF_ARGB1555:
        eFormat = NEXUS_PixelFormat_eA1_R5_G5_B5;
        break;
    case DSPF_RGB16:
        eFormat = NEXUS_PixelFormat_eR5_G6_B5;
        break;
    case DSPF_ARGB4444:
        eFormat = NEXUS_PixelFormat_eA4_R4_G4_B4;
        break;
    case DSPF_RGBA4444:
        eFormat = NEXUS_PixelFormat_eR4_G4_B4_A4;
        break;

    /* 24-bit RGB formats */
#ifdef BCMNEXUS_PIXELFORMAT_R8_G8_B8_SUPPORTED
    case DSPF_RGB24:
        eFormat = NEXUS_PixelFormat_eR8_G8_B8;
        break;
#endif

    /* 32-bit RGB formats */
    case DSPF_ARGB:
        eFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        break;
#if defined(BCMNEXUS_PIXELFORMAT_A8_B8_G8_R8_SUPPORTED) && defined(PIXEL_ABGR)
    case DSPF_ABGR:
        eFormat = NEXUS_PixelFormat_eA8_B8_G8_R8;
        break;
#endif
    case DSPF_RGB32:
#ifdef BCMNEXUS_PIXELFORMAT_X8_R8_G8_B8_SUPPORTED
        eFormat = NEXUS_PixelFormat_eX8_R8_G8_B8;
#else
        eFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
#endif
        break;

    /* YUV formats */
    case DSPF_YUY2:
        eFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;
        break;
    case DSPF_UYVY:
        eFormat = NEXUS_PixelFormat_eY18_Cr8_Y08_Cb8;
      break;
    case DSPF_AYUV:
        eFormat = NEXUS_PixelFormat_eA8_Y8_Cb8_Cr8;
        break;
    default:
        eFormat = NEXUS_PixelFormat_eUnknown;
        break;
    }
    return eFormat;
}

DFBSurfacePixelFormat
bcmnexus_getDFBSurfacePixelFormat(NEXUS_PixelFormat format)
{
    DFBSurfacePixelFormat eFormat;

    switch (format)
    {
    /* Constant Colour Formats */
#ifdef BCMNEXUS_PIXELFORMAT_A1_SUPPORTED
    case NEXUS_PixelFormat_eA1:
        eFormat = DSPF_A1;
        break;
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A4_SUPPORTED
    case NEXUS_PixelFormat_eA4:
        eFormat = DSPF_A4;
        break;
#endif
    case NEXUS_PixelFormat_eA8:
        eFormat = DSPF_A8;
        break;

    /* Palette Formats */
    case NEXUS_PixelFormat_ePalette2:
        eFormat = DSPF_LUT2;
        break;
    case NEXUS_PixelFormat_ePalette4:
        eFormat = DSPF_LUT4;
        break;
    case NEXUS_PixelFormat_ePalette8:
        eFormat = DSPF_LUT8;
        break;
    case NEXUS_PixelFormat_eA8_Palette8:
        eFormat = DSPF_ALUT8;
        break;

    /* 16-bit RGB formats */
#ifdef BCMNEXUS_PIXELFORMAT_X4_R4_G4_B4_SUPPORTED        
    case NEXUS_PixelFormat_eX4_R4_G4_B4:
        eFormat = DSPF_RGB444;
        break;
#endif
#ifdef BCMNEXUS_PIXELFORMAT_X1_R5_G5_B5_SUPPORTED
    case NEXUS_PixelFormat_eX1_R5_G5_B5:
        eFormat = DSPF_RGB555;
        break;
#endif
#ifdef BCMNEXUS_PIXELFORMAT_X1_B5_G5_R5_SUPPORTED
    case NEXUS_PixelFormat_eX1_B5_G5_R5:
        eFormat = DSPF_BGR555;
        break;
#endif
    case NEXUS_PixelFormat_eA1_R5_G5_B5:
        eFormat = DSPF_ARGB1555;
        break;
    case NEXUS_PixelFormat_eR5_G6_B5:
        eFormat = DSPF_RGB16;
        break;
    case NEXUS_PixelFormat_eA4_R4_G4_B4:
        eFormat = DSPF_ARGB4444;
        break;
    case NEXUS_PixelFormat_eR4_G4_B4_A4:
        eFormat = DSPF_RGBA4444;
        break;

    /* 24-bit RGB formats */
#ifdef BCMNEXUS_PIXELFORMAT_R8_G8_B8_SUPPORTED
    case NEXUS_PixelFormat_eR8_G8_B8:
        eFormat = DSPF_RGB24;
        break;
#endif

    /* 32-bit RGB formats */
#ifdef BCMNEXUS_PIXELFORMAT_X8_R8_G8_B8_SUPPORTED
    case NEXUS_PixelFormat_eX8_R8_G8_B8:
        eFormat = DSPF_RGB32;
        break;
#endif
#if defined(BCMNEXUS_PIXELFORMAT_A8_B8_G8_R8_SUPPORTED) && defined(PIXEL_ABGR)
    case NEXUS_PixelFormat_eA8_B8_G8_R8:
        eFormat = DSPF_ABGR;
        break;
#endif

    /* YUV formats */
    case NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08:
        eFormat = DSPF_YUY2;
        break;
    case NEXUS_PixelFormat_eY18_Cr8_Y08_Cb8:
        eFormat = DSPF_UYVY;
      break;
    case NEXUS_PixelFormat_eA8_Y8_Cb8_Cr8:
        eFormat = DSPF_AYUV;
        break;
    default:
        eFormat = DSPF_ARGB;
        break;
    }
    return eFormat;
}

DFB_PlatformPixelFormat
bcmnexus_getDFBPlatformPixelFormat(DFBSurfacePixelFormat format)
{
    DFB_PlatformPixelFormat eFormat;

    switch (format)
    {
        case DSPF_ARGB1555:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ARGB1555;
            break;

        case DSPF_RGB16:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGB16;
            break;

        case DSPF_RGB24:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGB24;
            break;

        case DSPF_RGB32:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGB32;
            break;

        case DSPF_ARGB:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ARGB;
            break;

        case DSPF_A8:
            eFormat = DFB_PLATFORM_PIXELFORMAT_A8;
            break;

        case DSPF_YUY2:
            eFormat = DFB_PLATFORM_PIXELFORMAT_YUY2;
            break;

        case DSPF_RGB332:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGB332;
            break;

        case DSPF_UYVY:
            eFormat = DFB_PLATFORM_PIXELFORMAT_UYVY;
            break;

        case DSPF_I420:
            eFormat = DFB_PLATFORM_PIXELFORMAT_I420;
            break;

        case DSPF_YV12:
            eFormat = DFB_PLATFORM_PIXELFORMAT_YV12;
            break;

        case DSPF_LUT8:
            eFormat = DFB_PLATFORM_PIXELFORMAT_LUT8;
            break;

        case DSPF_ALUT44:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ALUT44;
            break;

        case DSPF_AiRGB:
            eFormat = DFB_PLATFORM_PIXELFORMAT_AiRGB;
            break;

        case DSPF_A1:
            eFormat = DFB_PLATFORM_PIXELFORMAT_A1;
            break;

        case DSPF_NV12:
            eFormat = DFB_PLATFORM_PIXELFORMAT_NV12;
            break;

        case DSPF_NV16:
            eFormat = DFB_PLATFORM_PIXELFORMAT_NV16;
            break;

        case DSPF_ARGB2554:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ARGB2554;
            break;

        case DSPF_ARGB4444:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ARGB4444;
            break;

        case DSPF_RGBA4444:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGBA4444;
            break;

        case DSPF_NV21:
            eFormat = DFB_PLATFORM_PIXELFORMAT_NV21;
            break;

        case DSPF_AYUV:
            eFormat = DFB_PLATFORM_PIXELFORMAT_AYUV;
            break;

        case DSPF_A4:
            eFormat = DFB_PLATFORM_PIXELFORMAT_A4;
            break;

        case DSPF_ARGB1666:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ARGB1666;
            break;

        case DSPF_ARGB6666:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ARGB6666;
            break;

        case DSPF_RGB18:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGB18;
            break;

        case DSPF_LUT2:
            eFormat = DFB_PLATFORM_PIXELFORMAT_LUT2;
            break;

        case DSPF_RGB444:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGB444;
            break;

        case DSPF_RGB555:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGB555;
            break;

        case DSPF_BGR555:
            eFormat = DFB_PLATFORM_PIXELFORMAT_BGR555;
            break;

        case DSPF_LUT4:
            eFormat = DFB_PLATFORM_PIXELFORMAT_LUT4;
            break;

        case DSPF_RGBA5551:
            eFormat = DFB_PLATFORM_PIXELFORMAT_RGBA5551;
            break;

        case DSPF_YUV444P:
            eFormat = DFB_PLATFORM_PIXELFORMAT_YUV444P;
            break;

        case DSPF_ARGB8565:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ARGB8565;
            break;

        case DSPF_AVYU:
            eFormat = DFB_PLATFORM_PIXELFORMAT_AVYU;
            break;

        case DSPF_VYU:
            eFormat = DFB_PLATFORM_PIXELFORMAT_VYU;
            break;

        case DSPF_A1_LSB:
            eFormat = DFB_PLATFORM_PIXELFORMAT_A1_LSB;
            break;

        case DSPF_YV16:
            eFormat = DFB_PLATFORM_PIXELFORMAT_YV16;
            break;

        case DSPF_ABGR:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ABGR;
            break;

        case DSPF_ALUT8:
            eFormat = DFB_PLATFORM_PIXELFORMAT_ALUT8;
            break;

        default:
            eFormat = DFB_PLATFORM_PIXELFORMAT_UNKNOWN;
            break;
    }
    return eFormat;
}

void
copy_palette( CoreSurface *surface, NEXUS_SurfaceHandle hSurface )
{
	CorePalette        *palette;
	NEXUS_SurfaceMemory memory;
	unsigned int        i;

	if (hSurface && ((surface->config.format==DSPF_LUT8) || (surface->config.format==DSPF_ALUT8) || 
                     (surface->config.format==DSPF_LUT4) || (surface->config.format==DSPF_LUT2)))
    {
		NEXUS_Surface_GetMemory(hSurface, &memory);
        palette = surface->palette;
		for (i=0; i<palette->num_entries; i++)
		{
			memory.palette[i] = PIXEL_ARGB(
				palette->entries[i].a,
				palette->entries[i].r,
				palette->entries[i].g,
				palette->entries[i].b);
		}
		memory.numPaletteEntries = palette->num_entries;
        NEXUS_Memory_FlushCache(memory.palette, sizeof(NEXUS_Pixel) * memory.numPaletteEntries);
	}
}

