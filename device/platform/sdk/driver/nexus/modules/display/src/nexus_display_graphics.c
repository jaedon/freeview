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
 * $brcm_Workfile: nexus_display_graphics.c $
 * $brcm_Revision: 44 $
 * $brcm_Date: 9/13/12 4:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_display_graphics.c $
 * 
 * 44   9/13/12 4:23p erickson
 * SW7425-3896: deprecate NEXUS_GraphicsFramebuffer3D.rightAlpha
 * 
 * 43   5/14/12 12:30p erickson
 * SW7346-824: cache graphics settings if internal window not created
 * 
 * 42   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 41   1/13/12 8:33p erickson
 * SW7425-2139: expand list of BAVC_SourceId_eGfx ids
 * 
 * 40   9/8/11 2:18p erickson
 * SW7425-1267: add NEXUS_GraphicsSettings.horizontalCoeffIndex and
 *  .verticalCoeffIndex
 * 
 * 39   4/22/11 3:13p vsilyaev
 * SW7425-416: Updated 3D display API
 * 
 * 38   4/14/11 3:14p jtna
 * SWDTV-6321: remove duplicate perl keywords
 * 
 * 34   11/9/10 2:49p erickson
 * SW35230-1899: add NEXUS_GraphicsSettings.visible
 * 
 * 33   11/2/10 2:19p vsilyaev
 * SW7422-64: Added API to set 3D framebuffer
 * 
 * 32   11/1/10 10:28a erickson
 * SW7422-43: use new BAVC_Gfx_Picture struct
 * 
 * 31   8/6/10 1:08p erickson
 * SW7420-703: NEXUS_Surface_GetCreateSettings will always return a non-
 *  NULL heap
 *
 * 30   8/4/10 4:36p spothana
 * SW7420-886: Add support for 32b UMA mode using MEMC0 only for 97410.
 *
 * 29   7/15/10 4:23p erickson
 * SW7420-890: update 7420 fb check
 *
 * 28   6/25/10 12:12p erickson
 * SW7405-4515: add NEXUS_Display_SetGraphicsColorSettings. allow
 *  NEXUS_Display_SetGraphicsColorMatrix with NULL to unset matrix.
 *
 * 27   4/22/10 4:32p erickson
 * SW7420-703: don't assume nexusHeap[0]. just give a general warning.
 *
 * 26   4/6/10 3:20p erickson
 * SW7420-678: added check into NEXUS_Display_SetGraphicsFramebuffer for
 *  framebuffer MEMC on 7420
 *
 * 25   11/12/09 10:50a erickson
 * SW3548-2616: refactor NEXUS_Display_P_InitGraphics and related code
 *
 * 24   8/10/09 4:11p erickson
 * PR55364: factor in !handleDynamicRts to graphics test
 *
 * 23   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 22   8/6/09 2:38p erickson
 * PR56417: removed NEXUS_GraphicsSettings.gammaCorrection. feature should
 *  not be used.
 *
 * 21   7/16/09 11:50a erickson
 * PR56417: added NEXUS_GraphicsSettings.gammaCorrection
 *
 * 20   5/20/09 4:00p erickson
 * PR54512: added requirement that framebuffers with non-zero bitsPerPixel
 *  cannot be clipped
 *
 * 19   5/1/09 11:07a jgarrett
 * PR 54512: Adding gfx bits per pixel restrictions
 *
 * 18   4/2/09 11:57a erickson
 * PR53810: fix logic for setting frameBufferChanged
 *
 * 17   3/6/09 4:32p erickson
 * PR52839: added NEXUS_GraphicsFilterCoeffs to NEXUS_GraphicsSettings for
 *  controlling GFD scaling coefficients
 *
 * 16   1/9/09 12:19a erickson
 * PR48984: fix warning a different way
 *
 * 15   1/8/09 2:33a erickson
 * PR48984: fix warnings
 *
 * 14   1/6/09 11:33a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * PR48984/2   12/10/08 11:29a jgarrett
 * PR 48984: Adding additional info on VDB violations
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding vdb hooks
 *
 * 13   10/29/08 1:20p erickson
 * PR48409: remove default 1/2 width framebuffer for HD. customers must
 *  know their graphics cliprect width and program it. apps should not
 *  rely on a 1/2 width default.
 *
 * 12   9/18/08 4:19p erickson
 * PR47111: fix warnings
 *
 * 11   9/18/08 11:02a erickson
 * PR47014: update blend equation enums
 *
 * 10   9/17/08 12:29p erickson
 * PR47014: expose compositor blend equation in NEXUS_VideoWindowSettings
 *  and NEXUS_GraphicsSettings
 *
 * 9   7/3/08 5:07p erickson
 * PR43699: Remove unnecessary VDC Get functions from Nexus to avoid race
 *  conditions
 *
 * 8   6/10/08 3:29p vsilyaev
 * PR 43184: Added support for Start/Stop callbacks in linux kernel/user
 *  proxy mode
 *
 * 7   4/4/08 4:06p erickson
 * PR39257: add NEXUS_DisplayType_eBypass for DTV
 *
 * 6   3/24/08 5:35p vsilyaev
 * PR 40755: Added implementation for colorkeying
 *
 * 5   3/20/08 2:17p erickson
 * PR40156: convert to BVDC_Window_GetColorMatrix for graphics color
 *  matrix
 *
 * 4   3/3/08 9:21a erickson
 * PR40156: fix legacy VDC
 *
 * 3   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 2   2/28/08 10:42p vsilyaev
 * PR 40103: Added interfaceHandle and settings for the
 *  NEXUS_IsrCallbackCreate
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/10   11/28/07 1:43p vsilyaev
 * PR 36159: Added global UpdateMode, removed per-function ApplyChanges
 *
 * Nexus_Devel/9   10/17/07 11:05a erickson
 * PR36068: set zorder and alpha
 *
 * Nexus_Devel/8   10/16/07 12:32p erickson
 * PR36068: hardcode graphics zorder to 2 for now
 *
 * Nexus_Devel/7   10/3/07 1:24p vsilyaev
 * PR 34662: Added window clipping
 *
 * Nexus_Devel/6   10/2/07 6:18p vsilyaev
 * PR 34662: Added support for format changes
 *
 * Nexus_Devel/5   10/2/07 10:40a vsilyaev
 * PR 34662: Clear picture info in the callback
 *
 * Nexus_Devel/4   10/1/07 5:49p vsilyaev
 * PR 34662: Implemented graphics framebuffer callback
 *
 * Nexus_Devel/3   9/28/07 4:25p jgarrett
 * PR 35002: Successful 7400 build
 *
 * Nexus_Devel/2   9/26/07 5:35p vsilyaev
 * PR 34662: Fixed graphics destroy
 *
 * Nexus_Devel/1   9/26/07 1:42p vsilyaev
 * PR 34662: Implementation of display framebuffer
 *
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#include "priv/nexus_surface_priv.h"

BDBG_MODULE(nexus_display_graphics);

static bool NEXUS_Display_P_GraphicsValid(NEXUS_DisplayHandle display, const NEXUS_GraphicsSettings *pSettings, const NEXUS_GraphicsFramebuffer3D *framebuffer3d);
static void NEXUS_Display_P_DestroyGraphics(NEXUS_DisplayHandle display);

static void
NEXUS_Display_P_GraphicsNext_isr(void  *disp, int unused, BAVC_Polarity  polarity, BAVC_SourceState  state, void **picture)
{
    NEXUS_DisplayHandle display = disp;
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;

    BSTD_UNUSED(unused);
    BSTD_UNUSED(state);
    BSTD_UNUSED(polarity);
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(picture);
    *picture = NULL; /* clear picture info */
    if(graphics->queuedSurface) {
        BAVC_Gfx_Picture pic;
        BERR_Code rc;
        rc = BVDC_Source_GetSurface_isr(graphics->source, &pic);
        if (!rc) {
            if (pic.hSurface == graphics->queuedSurface) {
                graphics->queuedSurface = NULL;
                NEXUS_IsrCallback_Fire_isr(graphics->frameBufferCallback);
            }
        }
    }
    return;
}

static NEXUS_Error
NEXUS_Display_P_SetGraphicsChromaKey(const struct NEXUS_DisplayGraphics *graphics, const NEXUS_GraphicsSettings *cfg)
{
    BERR_Code rc;

    if(cfg->chromakeyEnabled) {
        uint32_t min,max,mask;
        switch(graphics->frameBufferPixelFormat) {
        default:
            min = cfg->lowerChromakey;
            max = cfg->upperChromakey;
            mask = BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8, 0xFF, 0xFF, 0xFF, 0xFF);
            break;
        case NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8:
        case NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08:
            {
            unsigned t_min, t_max;

            BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8, cfg->lowerChromakey, &t_min);
            BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8, cfg->upperChromakey, &t_max);
            min = t_min;
            max = t_max;
            mask = BPXL_MAKE_PIXEL(BPXL_eY08_Cr8_Y18_Cb8, 0xFF, 0xFF, 0xFF, 0xFF);
            break;
            }
        }
        rc = BVDC_Source_EnableColorKey(graphics->source, min, max, mask, 0 /* alpha 0 - remove it */);
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    } else {
        rc = BVDC_Source_DisableColorKey(graphics->source);
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    }
    return NEXUS_SUCCESS;
}


static BERR_Code
NEXUS_Display_P_SetGraphicsSettings(NEXUS_DisplayHandle display, const NEXUS_GraphicsSettings *cfg, bool force)
{
    BERR_Code rc;
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;

    /* Check VDB limits first */
    if ( !NEXUS_Display_P_GraphicsValid(display, cfg, &display->graphics.frameBuffer3D) )
    {
        NEXUS_Display_P_DestroyGraphics(display);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    force = force || video->lastUpdateFailed;

    if(force || graphics->cfg.horizontalFilter != cfg->horizontalFilter) {
        BDBG_CASSERT(BVDC_FilterCoeffs_eSharp == (BVDC_FilterCoeffs)NEXUS_GraphicsFilterCoeffs_eSharp);
        rc = BVDC_Source_SetHorizontalScaleCoeffs(graphics->source, cfg->horizontalFilter);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_source_cfg;}
    }
    
    if (force || 
        graphics->cfg.horizontalCoeffIndex != cfg->horizontalCoeffIndex ||
        graphics->cfg.verticalCoeffIndex != cfg->verticalCoeffIndex) 
    {
        BVDC_CoefficientIndex coeffSettings;
        BKNI_Memset(&coeffSettings, 0, sizeof(coeffSettings)); /* no GetSettings */
        coeffSettings.ulSclVertLuma = graphics->cfg.verticalCoeffIndex;
        coeffSettings.ulSclHorzLuma = graphics->cfg.horizontalCoeffIndex;
        rc = BVDC_Window_SetCoefficientIndex(graphics->windowVdc, &coeffSettings);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}
    }

    /* Set the graphics framebuffer to be full screen for its display. */
    rc = BVDC_Window_SetScalerOutput( graphics->windowVdc, 0, 0, cfg->position.width, cfg->position.height);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

    rc = BVDC_Window_SetDstRect( graphics->windowVdc, cfg->position.x, cfg->position.y, cfg->position.width, cfg->position.height);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

    /* Clip the framebuffer to match the display size */
    rc = BVDC_Window_SetSrcClip( graphics->windowVdc, cfg->clip.x, graphics->frameBufferWidth - (cfg->clip.width+cfg->clip.x), cfg->clip.y, graphics->frameBufferHeight - (cfg->clip.height+cfg->clip.y));
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

    if(force || graphics->cfg.zorder != cfg->zorder) {
        rc = BVDC_Window_SetZOrder( graphics->windowVdc, cfg->zorder);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}
    }

    if(force || graphics->cfg.alpha != cfg->alpha ||
                graphics->cfg.sourceBlendFactor != cfg->sourceBlendFactor ||
                graphics->cfg.destBlendFactor != cfg->destBlendFactor ||
                graphics->cfg.constantAlpha != cfg->constantAlpha)
    {
        rc = BVDC_Window_SetAlpha(graphics->windowVdc, cfg->alpha);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

        rc = BVDC_Window_SetBlendFactor( graphics->windowVdc, cfg->sourceBlendFactor, cfg->destBlendFactor, cfg->constantAlpha);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}
    }

    if(force || (graphics->cfg.chromakeyEnabled!=cfg->chromakeyEnabled || graphics->cfg.lowerChromakey!=cfg->lowerChromakey || graphics->cfg.upperChromakey!=cfg->upperChromakey)) {
        rc = NEXUS_Display_P_SetGraphicsChromaKey(graphics, cfg);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_source_cfg;}
    }

    if(force || (graphics->cfg.visible != cfg->visible)) {
        rc = BVDC_Window_SetVisibility(graphics->windowVdc, cfg->visible);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}
    }

    if(force || (graphics->cfg.graphics3DSettings.rightViewOffset != cfg->graphics3DSettings.rightViewOffset)) {
        rc = BVDC_Window_SetDstRightRect(graphics->windowVdc, cfg->graphics3DSettings.rightViewOffset);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}
    }
    
    rc = BVDC_Window_SetContrast(graphics->windowVdc, graphics->colorSettings.contrast);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

    rc = BVDC_Window_SetSaturation(graphics->windowVdc, graphics->colorSettings.saturation);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

    rc = BVDC_Window_SetHue(graphics->windowVdc, graphics->colorSettings.hue);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

    rc = BVDC_Window_SetBrightness(graphics->windowVdc, graphics->colorSettings.brightness);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}

    if (display->graphics.colorMatrixSet) {
        rc = NEXUS_Display_SetGraphicsColorMatrix(display, &display->graphics.colorMatrix);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window_cfg;}
    }
    
    return rc;
err_window_cfg:
err_source_cfg:
    return rc;
}

static NEXUS_Error
NEXUS_Display_P_SetGraphicsSource(NEXUS_DisplayHandle display, const NEXUS_GraphicsFramebuffer3D *frameBuffer3D, BSUR_Surface_Handle main)
{
    BERR_Code rc;
    BAVC_Gfx_Picture pic;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;

    BKNI_Memset(&pic, 0, sizeof(pic));
    pic.hSurface = main;
    pic.eInOrientation = NEXUS_P_VideoOrientation_ToMagnum(frameBuffer3D->orientation);
    pic.ucW0Alpha = frameBuffer3D->alphaW0;
    pic.ucW1Alpha = frameBuffer3D->alphaW1;
    if(frameBuffer3D->alpha || frameBuffer3D->right) {
        NEXUS_Module_Lock(video->modules.surface);
        if(frameBuffer3D->right) {
            pic.hRSurface = NEXUS_Surface_GetSurface_priv(frameBuffer3D->right);
        }
        if(frameBuffer3D->alpha) {
            pic.hAlphaSurface = NEXUS_Surface_GetSurface_priv(frameBuffer3D->alpha);
        }
        NEXUS_Module_Unlock(video->modules.surface);
    }
    rc = BVDC_Source_SetSurface( display->graphics.source, &pic);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}

static BERR_Code
NEXUS_Display_P_CreateGraphics(NEXUS_DisplayHandle display, const NEXUS_GraphicsSettings *cfg)
{
    BERR_Code rc;
    BERR_Code cleanup_rc; /* keep original error code */
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;
    static const BAVC_SourceId gfx_ids[]={BAVC_SourceId_eGfx0,BAVC_SourceId_eGfx1, BAVC_SourceId_eGfx2,BAVC_SourceId_eGfx3,BAVC_SourceId_eGfx4,BAVC_SourceId_eGfx5};
    BSUR_Surface_Handle surface;
    NEXUS_SurfaceCreateSettings surfaceCfg;

    BDBG_MSG((">graphics: %ux%u video=%#x display=%#x graphics=%#x", cfg->position.width, cfg->position.height, (unsigned)video, (unsigned)display, (unsigned)graphics));
    if (display->index >= sizeof(gfx_ids)/sizeof(*gfx_ids)) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    BDBG_ASSERT(graphics->frameBuffer3D.main);
    NEXUS_Module_Lock(video->modules.surface);
    surface = NEXUS_Surface_GetSurface_priv(graphics->frameBuffer3D.main);
    BDBG_ASSERT(surface);
    NEXUS_Module_Unlock(video->modules.surface);
    NEXUS_Surface_GetCreateSettings(graphics->frameBuffer3D.main, &surfaceCfg);
    graphics->queuedSurface = surface;
    graphics->frameBufferHeight = surfaceCfg.height;
    graphics->frameBufferWidth = surfaceCfg.width;
    graphics->frameBufferPixelFormat = surfaceCfg.pixelFormat;

    rc = BVDC_Source_Create( video->vdc, &graphics->source, gfx_ids[display->index], NULL);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_source;}
    rc = NEXUS_Display_P_SetGraphicsSource(display, &graphics->frameBuffer3D, surface);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_source_cfg;}

    rc = BVDC_Source_InstallPictureCallback(graphics->source, NEXUS_Display_P_GraphicsNext_isr, display, 0);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_source_cfg;}

    rc = BVDC_Window_Create( display->compositor, &graphics->windowVdc, BVDC_WindowId_eAuto, graphics->source, NULL);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_window;}

    rc = NEXUS_Display_P_SetGraphicsSettings(display, cfg, true);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_graphics_cfg;}

    if(video->updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(video->vdc);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_apply_changes;}
    BDBG_MSG(("<graphics:%#x", graphics->windowVdc));

    return BERR_SUCCESS;

err_apply_changes:
err_graphics_cfg:
    cleanup_rc = BVDC_AbortChanges(video->vdc);
    if (cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc);}
    cleanup_rc = BVDC_Window_Destroy(graphics->windowVdc);
    if (cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc);}
err_window:
err_source_cfg:
    cleanup_rc = BVDC_Source_Destroy(graphics->source);
    if (cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc);}
err_source:
    return rc;

}

/* destroy graphics and reset NEXUS_GraphicsSettings based on the current display format */
void
NEXUS_Display_P_ResetGraphics(NEXUS_DisplayHandle display)
{
    /* if graphics has not been created, we should reset the display dimensions for when it is */
    NEXUS_Display_P_DestroyGraphics(display);

    display->graphics.cfg.position = display->displayRect;
    display->graphics.cfg.clip = display->displayRect;
    display->graphics.validCount = 0;
    return;
}

void
NEXUS_Display_P_DestroyGraphicsSource(NEXUS_DisplayHandle display)
{
    BERR_Code rc;
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    
    if (!graphics->windowVdc) {
        return;
    }
        
    graphics->queuedSurface = NULL;
    rc = BVDC_Window_Destroy(graphics->windowVdc);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    rc = BVDC_Source_Destroy(graphics->source);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    /* If apply changes fails now, we're in an unrecoverable state, so stop here. */
    if(video->updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(video->vdc);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    graphics->source = NULL;
    graphics->windowVdc = NULL;
    
}

static void
NEXUS_Display_P_DestroyGraphics(NEXUS_DisplayHandle display)
{   
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;    

    if (!graphics->windowVdc) {
        return;
    }

    BDBG_MSG((">graphics: destroy %#x", graphics));

    NEXUS_Display_P_DestroyGraphicsSource(display);
    
    NEXUS_Graphics_GetDefaultFramebuffer3D(&graphics->frameBuffer3D);    

    BDBG_MSG(("<graphics: destroy %#x", graphics));

    return;
}

void
NEXUS_Display_GetGraphicsSettings(NEXUS_DisplayHandle display, NEXUS_GraphicsSettings *settings)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(settings);
    *settings = display->graphics.cfg;
    return;
}

NEXUS_Error
NEXUS_Display_SetGraphicsSettings(NEXUS_DisplayHandle display,const NEXUS_GraphicsSettings *settings)
{
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(settings);
    if(graphics->windowVdc && !settings->enabled) {
        NEXUS_Display_P_DestroyGraphics(display);
    }
    if(!settings->enabled) {
        NEXUS_Graphics_GetDefaultFramebuffer3D(&graphics->frameBuffer3D);
    }
    if(graphics->frameBuffer3D.main && settings->enabled) {
        if(graphics->windowVdc) {  /* update existing */
            rc = NEXUS_Display_P_SetGraphicsSettings(display, settings, false);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_graphics_cfg;}
            rc = NEXUS_Display_P_ApplyChanges();
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_apply_changes;}
        } else { /* create new graphics */
            rc = NEXUS_Display_P_CreateGraphics(display, settings);
            if(rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_create;}
        }
    } /* if no frame bufeer or graphics not enabled delay activation of graphics */
    graphics->cfg = *settings;
    NEXUS_IsrCallback_Set(graphics->frameBufferCallback, &graphics->cfg.frameBufferCallback);
    return BERR_SUCCESS;

err_apply_changes:
err_graphics_cfg:
    {
        BERR_Code rc = BVDC_AbortChanges(video->vdc);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); }
    }
err_create:
    return rc;
}

NEXUS_Error
NEXUS_Display_SetGraphicsFramebuffer(NEXUS_DisplayHandle display, NEXUS_SurfaceHandle frameBuffer)
{
    NEXUS_Error rc;
    NEXUS_GraphicsFramebuffer3D framebuffer3D;

    NEXUS_Graphics_GetDefaultFramebuffer3D(&framebuffer3D);
    framebuffer3D.orientation = NEXUS_VideoOrientation_e2D;
    framebuffer3D.main = frameBuffer;
    rc = NEXUS_Display_SetGraphicsFramebuffer3D(display, &framebuffer3D);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    return rc;
}


NEXUS_Error
NEXUS_Display_SetGraphicsFramebuffer3D(NEXUS_DisplayHandle display, const NEXUS_GraphicsFramebuffer3D *frameBuffer3D)
{
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;

    BERR_Code rc;
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(frameBuffer3D);


    if(frameBuffer3D->main==NULL) { return BERR_TRACE(NEXUS_INVALID_PARAMETER); }

    /* Check VDB limits first */
    if ( !NEXUS_Display_P_GraphicsValid(display, &display->graphics.cfg, frameBuffer3D) )
    {
        NEXUS_Display_P_DestroyGraphics(display);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    graphics->frameBuffer3D = *frameBuffer3D;
    if(graphics->windowVdc) {
        BSUR_Surface_Handle surface;
        NEXUS_SurfaceCreateSettings surfaceCfg;
        bool frameBufferChanged;

        BDBG_ASSERT(graphics->source);
        NEXUS_Module_Lock(video->modules.surface);
        surface = NEXUS_Surface_GetSurface_priv(frameBuffer3D->main);
        BDBG_ASSERT(surface);
        NEXUS_Module_Unlock(video->modules.surface);
        NEXUS_Surface_GetCreateSettings(graphics->frameBuffer3D.main, &surfaceCfg);
        if(graphics->queuedSurface) {
            BDBG_MSG(("NEXUS_Display_SetGraphicsFramebuffer: %#lx setting duplicated framebuffer %#lx %s", (unsigned long)display, (unsigned long)frameBuffer3D->main, graphics->queuedSurface==surface?"without sync":""));
        }
        graphics->queuedSurface = surface;
        frameBufferChanged = (graphics->frameBufferHeight != surfaceCfg.height || graphics->frameBufferWidth != surfaceCfg.width || graphics->frameBufferPixelFormat != surfaceCfg.pixelFormat);
        graphics->frameBufferHeight = surfaceCfg.height;
        graphics->frameBufferWidth = surfaceCfg.width;
        graphics->frameBufferPixelFormat = surfaceCfg.pixelFormat;
        rc = NEXUS_Display_P_SetGraphicsSource(display, frameBuffer3D, surface);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_set_source;}
        if(frameBufferChanged) {
            rc = NEXUS_Display_P_SetGraphicsSettings(display, &graphics->cfg, true);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_graphics_cfg;}
        }
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_apply_changes;}
    } else {
        if(graphics->cfg.enabled) {
            rc = NEXUS_Display_P_CreateGraphics(display, &graphics->cfg);
            if(rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc); goto err_create;}
        }
    }
    return BERR_SUCCESS;
err_apply_changes:
err_graphics_cfg:
    {
        BERR_Code rc = BVDC_AbortChanges(video->vdc);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); }
    }
err_set_source:
err_create:
    return rc;
}

/* NEXUS_Display_P_InitGraphics is only called from NEXUS_Display_Open */
BERR_Code
NEXUS_Display_P_InitGraphics(NEXUS_DisplayHandle display)
{
    BERR_Code rc;
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;

    graphics->source = NULL;
    graphics->windowVdc = NULL;
    graphics->frameBufferWidth = 0;
    graphics->frameBufferHeight = 0;
    graphics->frameBufferPixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    graphics->queuedSurface = NULL;
    graphics->validCount = 0;
    NEXUS_Graphics_GetDefaultFramebuffer3D(&graphics->frameBuffer3D);
    BKNI_Memset(&graphics->cfg, 0, sizeof(graphics->cfg));
    BKNI_Memset(&graphics->colorMatrix, 0, sizeof(graphics->colorMatrix));
    graphics->cfg.enabled = true;
    graphics->cfg.visible = true;
    /* cfg.position and cfg.clip are set from NEXUS_Display_P_ResetGraphics when the format changes */
    graphics->cfg.alpha = 0xFF;
    graphics->cfg.zorder = 2;
    graphics->cfg.sourceBlendFactor = NEXUS_CompositorBlendFactor_eSourceAlpha;
    graphics->cfg.destBlendFactor = NEXUS_CompositorBlendFactor_eInverseSourceAlpha;
    graphics->cfg.constantAlpha = 0xFF;
    graphics->cfg.horizontalFilter = NEXUS_GraphicsFilterCoeffs_eAnisotropic;
    NEXUS_CallbackDesc_Init(&graphics->cfg.frameBufferCallback);
    graphics->frameBufferCallback = NEXUS_IsrCallback_Create(display, NULL);
    if(!graphics->frameBufferCallback) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_callback;
    }
    NEXUS_IsrCallback_Set(graphics->frameBufferCallback, &graphics->cfg.frameBufferCallback);
    return BERR_SUCCESS;
err_callback:
    return rc;
}

/* NEXUS_Display_P_UninitGraphics is only called from NEXUS_Display_Close */
void
NEXUS_Display_P_UninitGraphics(NEXUS_DisplayHandle display)
{
    struct NEXUS_DisplayGraphics *graphics = &display->graphics;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    NEXUS_Display_P_DestroyGraphics(display);

    NEXUS_IsrCallback_Destroy(graphics->frameBufferCallback);
    return;
}

void NEXUS_Display_GetGraphicsColorMatrix( NEXUS_DisplayHandle display, NEXUS_ColorMatrix *pColorMatrix )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    *pColorMatrix = display->graphics.colorMatrix;
}

NEXUS_Error NEXUS_Display_SetGraphicsColorMatrix( NEXUS_DisplayHandle display, const NEXUS_ColorMatrix *pColorMatrix )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    display->graphics.colorMatrixSet = (pColorMatrix != NULL);
    if (pColorMatrix && &display->graphics.colorMatrix != pColorMatrix) {
        display->graphics.colorMatrix = *pColorMatrix;
    }

    if (display->graphics.windowVdc) {
        if (pColorMatrix) {
            rc = BVDC_Window_SetColorMatrix(display->graphics.windowVdc, true, pColorMatrix->coeffMatrix, pColorMatrix->shift);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            rc = BVDC_Window_SetColorMatrix(display->graphics.windowVdc, false, NULL, 0);
            if (rc) return BERR_TRACE(rc);
        }
    
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

void NEXUS_Display_GetGraphicsColorSettings( NEXUS_DisplayHandle display, NEXUS_GraphicsColorSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    *pSettings = display->graphics.colorSettings;
}

NEXUS_Error NEXUS_Display_SetGraphicsColorSettings( NEXUS_DisplayHandle display, const NEXUS_GraphicsColorSettings *pSettings )
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    display->graphics.colorSettings = *pSettings;
    if (display->graphics.windowVdc) {
        rc = BVDC_Window_SetContrast(display->graphics.windowVdc, pSettings->contrast);
        if (rc) return BERR_TRACE(rc);
        rc = BVDC_Window_SetSaturation(display->graphics.windowVdc, pSettings->saturation);
        if (rc) return BERR_TRACE(rc);
        rc = BVDC_Window_SetHue(display->graphics.windowVdc, pSettings->hue);
        if (rc) return BERR_TRACE(rc);
        rc = BVDC_Window_SetBrightness(display->graphics.windowVdc, pSettings->brightness);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

static bool NEXUS_Display_P_GraphicsValid(NEXUS_DisplayHandle display, const NEXUS_GraphicsSettings *pSettings, const NEXUS_GraphicsFramebuffer3D *framebuffer3d)
{
    bool valid=true;
    NEXUS_SurfaceHandle framebuffer = framebuffer3d->main;

    if ( pSettings->enabled )
    {
        if ( 0 == display->restrictions.gfxHeight )
        {
            BDBG_ERR(("Graphics is not supported on display %d due to bandwidth restrictions."));
            valid = false;
        }
        else
        {
            NEXUS_SurfaceCreateSettings createSettings;
            unsigned bpp;

            NEXUS_Surface_GetCreateSettings(framebuffer, &createSettings);
            bpp = NEXUS_Vdb_P_GetBpp(createSettings.pixelFormat);
            if ( createSettings.bitsPerPixel == 0 && display->restrictions.specialFeature1 != 0 )
            {
                BDBG_ERR(("Graphics bitsPerPixel setting is invalid for a framebuffer in this usage mode."));
                BDBG_ERR(("Please set NEXUS_SurfaceCreateSettings.bitsPerPixel to a non-zero value <= %d.", display->restrictions.specialFeature1));
                valid = false;
            }
            else if ( createSettings.bitsPerPixel > display->restrictions.specialFeature1 && g_NEXUS_DisplayModule_State.moduleSettings.handleDynamicRts )
            {
                BDBG_ERR(("Graphics bitsPerPixel setting is invalid for a framebuffer in this usage mode."));
                if ( display->restrictions.specialFeature1 )
                {
                    BDBG_ERR(("Please set NEXUS_SurfaceCreateSettings.bitsPerPixel to a non-zero value <= %d.", display->restrictions.specialFeature1));
                }
                else
                {
                    BDBG_ERR(("Please do not specify a bitsPerPixel value for the framebuffer in this usage mode."));
                }
                valid = false;
            }
            if ( pSettings->clip.height > display->restrictions.gfxHeight ||
                 pSettings->clip.width > display->restrictions.gfxWidth ||
                 bpp > display->restrictions.gfxBpp )
            {
                BDBG_ERR(("Graphics resolution (%dx%d %dbpp) is above current system limitations (%dx%d %dbpp).",
                          pSettings->clip.width, pSettings->clip.height, NEXUS_Vdb_P_GetBpp(createSettings.pixelFormat),
                          display->restrictions.gfxWidth, display->restrictions.gfxHeight, display->restrictions.gfxBpp));
                valid = false;
            }
            if (createSettings.bitsPerPixel != 0 &&
                (pSettings->clip.x || pSettings->clip.y || createSettings.width != pSettings->clip.width || createSettings.height != pSettings->clip.height))
            {
                BDBG_ERR(("Cannot clip a framebuffer with non-zero NEXUS_SurfaceCreateSettings.bitsPerPixel"));
                valid = false;
            }
        }
    }

    if ( !valid )
    {
        BDBG_ERR(("Disabling Graphics."));
    }

    return valid;
}

void NEXUS_Graphics_GetDefaultFramebuffer3D(NEXUS_GraphicsFramebuffer3D *pFramebuffer3D)
{
    BDBG_ASSERT(pFramebuffer3D);
    BKNI_Memset(pFramebuffer3D, 0, sizeof(*pFramebuffer3D));
    pFramebuffer3D->orientation = NEXUS_VideoOrientation_e2D;
    pFramebuffer3D->main = NULL;
    return;
}

