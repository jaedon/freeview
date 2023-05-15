/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: nano_directfb_func.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/6/12 6:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/nano_dfb/nano_directfb_func.c $
 * 
 * Hydra_Software_Devel/2   9/6/12 6:08p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "directfb.h"
#include "nexus_platform.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "bwin.h"

BDBG_MODULE(nano_directfb);

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */
#if 0
#undef BDBG_MSG
#define BDBG_MSG BDBG_WRN
#endif

/**
this is a crude implementation
TODO:
- BDBG_OBJECT protection
- error recovery, memory leaks, etc.
- user input events
- wait for framebuffer callback
**/

struct IDirectFBSurfaceState {
    struct IDirectFBState *dfb;
    NEXUS_SurfaceHandle surface[2];
    unsigned current_surface;
    bool flush_needed[2]; /* set true after every unlock, test before every blit or when setting the framebuffer */
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceMemory mem[2];
    uint32_t color;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    unsigned blittingFlags;
    unsigned porterDuffFlags;
    unsigned renderOptions;
};

struct IDirectFBState {
    NEXUS_Graphics2DHandle gfx;
    BKNI_EventHandle checkpoint_event, packetspace_available_event;
    NEXUS_DisplayHandle display;
    IDirectFBSurface *framebuffer;
    bool checkpoint_needed; /* set true after every blit, test before every lock or when setting the framebuffer */
    NEXUS_VideoFormatInfo displayFormat;
    
    bwin_engine_t win_engine;
};

struct IDirectFBImageProviderState {
    struct IDirectFBState *dfb;
    const char *filename;
    bwin_image_t image;
    bwin_framebuffer_t win_framebuffer;
};

static void check_checkpoint(struct IDirectFBSurfaceState *surface)
{
    if (surface->dfb->checkpoint_needed) {
        int rc = NEXUS_Graphics2D_Checkpoint(surface->dfb->gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            rc = BKNI_WaitForEvent(surface->dfb->checkpoint_event, 3000);
            BDBG_ASSERT(!rc);
        }
        surface->dfb->checkpoint_needed = false;
    }
}
static void check_flush(struct IDirectFBSurfaceState *surface)
{
    if (surface->flush_needed[0]) {
        NEXUS_Surface_Flush(surface->surface[0]);
        surface->flush_needed[0] = false;
    }
    if (surface->flush_needed[1]) {
        NEXUS_Surface_Flush(surface->surface[1]);
        surface->flush_needed[1] = false;
    }
}

static int IDirectFBEventBuffer_GetEvent(IDirectFBEventBuffer *IeventBuffer, DFBInputEvent *event)
{
    BSTD_UNUSED(IeventBuffer);
    BSTD_UNUSED(event);
    return -1; /* no events */
}

static void IDirectFBEventBuffer_Release(IDirectFBEventBuffer *IeventBuffer)
{
    BKNI_Free(IeventBuffer);
}

static void IDirectFBSurface_GetSize(const IDirectFBSurface *Isurface, unsigned *width, unsigned *height)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    *width = surface->createSettings.width;
    *height = surface->createSettings.height;
}

static int IDirectFBSurface_Lock(IDirectFBSurface *Isurface, int flags, void **buffer, unsigned *pitch)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    check_checkpoint(surface);
    surface->flush_needed[surface->current_surface] = (flags & DSLF_WRITE);
    *buffer = surface->mem[surface->current_surface].buffer;
    *pitch = surface->mem[surface->current_surface].pitch;
    return 0;
}

static void IDirectFBSurface_Unlock(IDirectFBSurface *Isurface)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    /* no op */
}

static void IDirectFBSurface_SetColor(IDirectFBSurface *Isurface, unsigned r, unsigned g, unsigned b, unsigned a)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    surface->color = a<<24 | r<<16 | g<<8 | b;
}

static void IDirectFBSurface_SetBlittingFlags(IDirectFBSurface *Isurface, DFBSurfaceBlittingFlags flags)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    surface->blittingFlags = flags;
}

static void IDirectFBSurface_SetPorterDuff(IDirectFBSurface *Isurface, int flags)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    surface->porterDuffFlags = flags;
}

static void IDirectFBSurface_SetRenderOptions(IDirectFBSurface *Isurface, int flags)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    surface->renderOptions = flags;
}

#define COPY2NEXUSRECT(PNEXUS_RECT, PDFB_RECT) do { \
    (PNEXUS_RECT)->x = (PDFB_RECT)->x; \
    (PNEXUS_RECT)->y = (PDFB_RECT)->y; \
    (PNEXUS_RECT)->width = (PDFB_RECT)->w; \
    (PNEXUS_RECT)->height = (PDFB_RECT)->h; \
    } while (0)
    
static int b_bound_rect(DFBRectangle *pRect, unsigned w, unsigned h)
{
    if (pRect->x < 0) {
        pRect->w += pRect->x;
        pRect->x = 0;
    }
    if (pRect->x + pRect->w > (int)w) {
        pRect->w = (int)w - pRect->x;
    }
    if (pRect->y < 0) {
        pRect->h += pRect->y;
        pRect->y = 0;
    }
    if (pRect->y + pRect->h > (int)h) {
        pRect->h = (int)h - pRect->y;
    }
}

bool g_debug = false;
static void IDirectFBSurface_StretchBlit(IDirectFBSurface *Isurface, const IDirectFBSurface *Isource_surface, const DFBRectangle *sourceRect, const DFBRectangle *destRect)
{
    int rc;
    struct IDirectFBSurfaceState *surface = Isurface->state;
    struct IDirectFBSurfaceState *source_surface = Isource_surface->state;
    DFBRectangle localSourceRect = {0,0,0,0};
    DFBRectangle localDestRect = {0,0,0,0};
    
    /* copy and clip */
    if (sourceRect && sourceRect->w && sourceRect->h) {
        localSourceRect = *sourceRect;
    }
    else {
        localSourceRect.w = source_surface->createSettings.width;
        localSourceRect.h = source_surface->createSettings.height;
    }
    if (destRect && destRect->w && destRect->h) {
        localDestRect = *destRect;
    }
    else {
        localDestRect.w = surface->createSettings.width;
        localDestRect.h = surface->createSettings.height;
    }
    /* TODO: logic probably not right */
    b_bound_rect(&localSourceRect, source_surface->createSettings.width, source_surface->createSettings.height);
    b_bound_rect(&localDestRect, surface->createSettings.width, surface->createSettings.height);
    if (localSourceRect.w <= 0 || localSourceRect.h <= 0) return;
    if (localDestRect.w <= 0 || localDestRect.h <= 0) return;
    
    /* don't getdefaultsettings here. too slow. */
    surface->blitSettings.output.surface = surface->surface[surface->current_surface];
    COPY2NEXUSRECT(&surface->blitSettings.output.rect, &localDestRect);
    surface->blitSettings.source.surface = source_surface->surface[source_surface->current_surface];
    COPY2NEXUSRECT(&surface->blitSettings.source.rect, &localSourceRect);
    
    if (surface->porterDuffFlags & DSPD_DST_IN) {
        static const NEXUS_BlendEquation g_colorBlend = { BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, 
            BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero };
        static const NEXUS_BlendEquation g_alphaBlend = { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, 
            BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero };

        surface->blitSettings.dest.surface = surface->surface[surface->current_surface];
        COPY2NEXUSRECT(&surface->blitSettings.dest.rect, &localDestRect);
        surface->blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
        surface->blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;
        surface->blitSettings.colorBlend = g_colorBlend;
        surface->blitSettings.alphaBlend = g_alphaBlend;
    }
    else if (surface->blittingFlags & DSBLIT_BLEND_ALPHACHANNEL) {
        surface->blitSettings.dest.surface = surface->surface[surface->current_surface];
        COPY2NEXUSRECT(&surface->blitSettings.dest.rect, &localDestRect);
        
        if (surface->blittingFlags & DSBLIT_COLORIZE) {
            static const NEXUS_BlendEquation g_colorBlend = { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, 
                BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero };
            static const NEXUS_BlendEquation g_alphaBlend = { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, 
                BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero };
            surface->blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
            surface->blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;
            surface->blitSettings.colorBlend = g_colorBlend;
            surface->blitSettings.alphaBlend = g_alphaBlend;
            surface->blitSettings.constantColor = surface->color;
            
            surface->blitSettings.alphaOp = NEXUS_BlitColorOp_eUseDestAlpha; /* WRONG: using wrong enum */
        }
        else if (surface->blittingFlags & DSBLIT_BLEND_COLORALPHA) {
            surface->blitSettings.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
            surface->blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
            surface->blitSettings.constantColor = surface->color;
        }
        else {
            surface->blitSettings.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
            surface->blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
            surface->blitSettings.constantColor = 0xFF000000;
        }
    }
    else {
        /* copy */
        surface->blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        surface->blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
    }
    
    BDBG_MSG(("blit %d,%d,%d,%d (%p %dx%d) -> %d/%d -> %d,%d,%d,%d (%p %dx%d)", 
        surface->blitSettings.source.rect.x, surface->blitSettings.source.rect.y, surface->blitSettings.source.rect.width, surface->blitSettings.source.rect.height,
        Isource_surface, source_surface->createSettings.width, source_surface->createSettings.height,
        surface->blitSettings.colorOp, surface->blitSettings.alphaOp,
        surface->blitSettings.output.rect.x, surface->blitSettings.output.rect.y, surface->blitSettings.output.rect.width, surface->blitSettings.output.rect.height,
        Isurface, surface->createSettings.width, surface->createSettings.height ));
        
    check_flush(surface);
    rc = NEXUS_Graphics2D_Blit(surface->dfb->gfx, &surface->blitSettings);
    BDBG_ASSERT(!rc);
    surface->dfb->checkpoint_needed = true;
}

static void IDirectFBSurface_Blit(IDirectFBSurface *Isurface, const IDirectFBSurface *Isource_surface, const DFBRectangle *sourceRect, int x, int y )
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    struct IDirectFBSurfaceState *source_surface = Isource_surface->state;
    DFBRectangle rect;
    
    rect.x = x;
    rect.y = y;
    rect.w = sourceRect?sourceRect->w:source_surface->createSettings.width;
    rect.h = sourceRect?sourceRect->h:source_surface->createSettings.height;
    return IDirectFBSurface_StretchBlit(Isurface, Isource_surface, sourceRect, &rect);
}

static void IDirectFBSurface_BatchBlit(IDirectFBSurface *Isurface, const IDirectFBSurface *Isource_surface, const DFBRectangle *sourceRect, const DFBPoint *destPoint, unsigned number)
{
    unsigned i;
    for (i=0;i<number;i++) {
        DFBRectangle rect;
        rect.x = destPoint[i].x;
        rect.y = destPoint[i].y;
        rect.w = sourceRect[i].w;
        rect.h = sourceRect[i].h;
        IDirectFBSurface_StretchBlit(Isurface, Isource_surface, &sourceRect[i], &rect);
    }
}

static void IDirectFBSurface_FillRectangles(IDirectFBSurface *Isurface, const DFBRectangle *destRect, unsigned number)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    unsigned i;
    int rc;
    
    check_flush(surface);
    
    /* don't getdefaultsettings here. too slow. */
    surface->fillSettings.surface = surface->surface[surface->current_surface];
    surface->fillSettings.color = surface->color;
    for (i=0;i<number;i++) {
        COPY2NEXUSRECT(&surface->fillSettings.rect, &destRect[i]);
        rc = NEXUS_Graphics2D_Fill(surface->dfb->gfx, &surface->fillSettings);
        BDBG_ASSERT(!rc);
        BDBG_MSG(("fill %d,%d,%d,%d %p %x", 
            destRect[i].x, destRect[i].y, destRect[i].w, destRect[i].h,
            surface, surface->color));
    }
    surface->dfb->checkpoint_needed = true;
}

static void IDirectFBSurface_Clear(IDirectFBSurface *Isurface, unsigned r, unsigned g, unsigned b, unsigned a)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    uint32_t temp_color = surface->color;
    DFBRectangle rect = {0,0,0,0}; /* whole surface */
    IDirectFBSurface_SetColor(Isurface, r, g, b, a);
    IDirectFBSurface_FillRectangles(Isurface, &rect, 1);
    surface->color = temp_color;
}

static void IDirectFBSurface_Flip(IDirectFBSurface *Isurface, void *unknown, int flags)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    int rc;
    if (surface->dfb->framebuffer == Isurface && surface->surface[1]) {
        check_checkpoint(surface);
        check_flush(surface);
        BDBG_MSG_TRACE(("set fb %d", surface->current_surface));
        rc = NEXUS_Display_SetGraphicsFramebuffer(surface->dfb->display, surface->surface[surface->current_surface]);
        BDBG_ASSERT(!rc);
        surface->current_surface = 1 - surface->current_surface;
    }
}

static void IDirectFBSurface_Release(IDirectFBSurface *Isurface)
{
    struct IDirectFBSurfaceState *surface = Isurface->state;
    if (surface->dfb->framebuffer == Isurface) {
        surface->dfb->framebuffer = NULL;
    }
    NEXUS_Surface_Destroy(surface->surface[0]);
    if (surface->surface[1]) {
        NEXUS_Surface_Destroy(surface->surface[1]);
    }
    BKNI_Free(surface);
    BKNI_Free(Isurface);
}

static void IDirectFBImageProvider_GetSurfaceDescription(IDirectFBImageProvider *Iprovider, DFBSurfaceDescription *pDesc)
{
    struct IDirectFBImageProviderState *provider = Iprovider->state;
    bwin_image_settings image_settings;
    
    BKNI_Memset(pDesc, 0, sizeof(*pDesc));
    
    provider->image = bwin_image_load(provider->dfb->win_engine, provider->filename);
    if (!provider->image) {
        BDBG_ERR(("Unable to load image %s", provider->filename));
        return;
    }
    
    pDesc->flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
    bwin_image_get_settings(provider->image, &image_settings);
    pDesc->width = image_settings.width;
    pDesc->height = image_settings.height;
    pDesc->pixelformat = DSPF_ARGB;
    
    BDBG_MSG_TRACE(("image %s: %dx%d", provider->filename, pDesc->width, pDesc->height));
}

static int  IDirectFBImageProvider_RenderTo(IDirectFBImageProvider *Iprovider, IDirectFBSurface *Isurface, void *unknown)
{
    struct IDirectFBImageProviderState *provider = Iprovider->state;
    struct IDirectFBSurfaceState *surface = Isurface->state;
    bwin_framebuffer_settings fb_settings;
    bwin_rect imagerect = {0,0,0,0};
    
    check_checkpoint(surface);
    
    bwin_framebuffer_settings_init(&fb_settings);
    imagerect.width = fb_settings.width = surface->createSettings.width;
    imagerect.height = fb_settings.height = surface->createSettings.height;
    fb_settings.pitch = surface->mem[surface->current_surface].pitch;
    fb_settings.buffer = surface->mem[surface->current_surface].buffer;
    fb_settings.pixel_format = bwin_pixel_format_a8_r8_g8_b8; /* TODO: support more */
    provider->win_framebuffer = bwin_open_framebuffer(provider->dfb->win_engine, &fb_settings);
    if (!provider->win_framebuffer) return BERR_TRACE(-1);

    /* get the toplevel window */
    bwin_get_framebuffer_settings(provider->win_framebuffer, &fb_settings);

    bwin_image_render(fb_settings.window, provider->image, bwin_image_render_mode_single_load, &imagerect, NULL, NULL);    

    surface->flush_needed[surface->current_surface] = true;
    
    return 0;
}

static void IDirectFBImageProvider_Release(IDirectFBImageProvider *Iprovider)
{
    struct IDirectFBImageProviderState *provider = Iprovider->state;
    if (provider->win_framebuffer) {
        bwin_close_framebuffer(provider->win_framebuffer);
    }
    if (provider->image) {
        bwin_image_close(provider->image);
    }
    BKNI_Free(provider);
    BKNI_Free(Iprovider);
}

static int IDirectFB_CreateSurface(IDirectFB *Idfb, const DFBSurfaceDescription *pDesc, IDirectFBSurface **p_surface)
{
    struct IDirectFBState *dfb = Idfb->state;
    IDirectFBSurface *Isurface;
    struct IDirectFBSurfaceState *surface;
    
    Isurface = BKNI_Malloc(sizeof(*Isurface));
    BKNI_Memset(Isurface, 0, sizeof(*Isurface));
    surface = Isurface->state = BKNI_Malloc(sizeof(*surface));
    BKNI_Memset(surface, 0, sizeof(*surface));
    
    NEXUS_Surface_GetDefaultCreateSettings(&surface->createSettings);
    
    if (pDesc->flags & DSDESC_WIDTH) {
        surface->createSettings.width = pDesc->width;
    }
    else {
        surface->createSettings.width = dfb->displayFormat.width;
    }
    if (pDesc->flags & DSDESC_HEIGHT) {
        surface->createSettings.height = pDesc->height;
    }
    else {
        surface->createSettings.height = dfb->displayFormat.height;
    }
    if (pDesc->flags & DSDESC_PIXELFORMAT) {
        switch (pDesc->pixelformat) {
        case DSPF_A8: surface->createSettings.pixelFormat = NEXUS_PixelFormat_eA8; break;
        case DSPF_ARGB:  surface->createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8; break;
        default: return BERR_TRACE(-1);
        }
    }
    surface->surface[0] = NEXUS_Surface_Create(&surface->createSettings);
    if (!surface->surface[0]) {
        return BERR_TRACE(-1);
    }
    NEXUS_Surface_GetMemory(surface->surface[0], &surface->mem[0]);
    
    if (pDesc->flags & DSDESC_CAPS) {
        if (pDesc->caps & DSCAPS_DOUBLE) {
            /* double buffer */
            surface->surface[1] = NEXUS_Surface_Create(&surface->createSettings);
            if (!surface->surface[1]) {
                return BERR_TRACE(-1);
            }
            NEXUS_Surface_GetMemory(surface->surface[1], &surface->mem[1]);
        }
        
        if (pDesc->caps & DSCAPS_PRIMARY) {
            if (dfb->framebuffer) {
                return BERR_TRACE(-1);
            }
            dfb->framebuffer = Isurface;
        }
    }
    
    surface->dfb = dfb;
    
    Isurface->GetSize = IDirectFBSurface_GetSize;
    Isurface->Lock = IDirectFBSurface_Lock;
    Isurface->Unlock = IDirectFBSurface_Unlock;
    Isurface->Clear = IDirectFBSurface_Clear;
    Isurface->SetColor = IDirectFBSurface_SetColor;
    Isurface->Blit = IDirectFBSurface_Blit;
    Isurface->StretchBlit = IDirectFBSurface_StretchBlit;
    Isurface->SetBlittingFlags = IDirectFBSurface_SetBlittingFlags;
    Isurface->SetPorterDuff = IDirectFBSurface_SetPorterDuff;
    Isurface->SetRenderOptions = IDirectFBSurface_SetRenderOptions;
    Isurface->FillRectangles = IDirectFBSurface_FillRectangles;
    Isurface->BatchBlit = IDirectFBSurface_BatchBlit;
    Isurface->Flip = IDirectFBSurface_Flip;
    Isurface->Release = IDirectFBSurface_Release;
    
    NEXUS_Graphics2D_GetDefaultFillSettings(&surface->fillSettings);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&surface->blitSettings);
    
    *p_surface = Isurface;
    
    return 0;
}

static int IDirectFB_CreateImageProvider(IDirectFB *Idfb, const char *name, IDirectFBImageProvider **p_provider)
{
    struct IDirectFBState *dfb = Idfb->state;
    IDirectFBImageProvider *Iprovider;
    struct IDirectFBImageProviderState *provider;
    
    Iprovider = BKNI_Malloc(sizeof(*Iprovider));
    BKNI_Memset(Iprovider, 0, sizeof(*Iprovider));
    provider = Iprovider->state = BKNI_Malloc(sizeof(*provider));
    BKNI_Memset(provider, 0, sizeof(*provider));
    
    provider->filename = name;
    provider->dfb = dfb;
    Iprovider->GetSurfaceDescription = IDirectFBImageProvider_GetSurfaceDescription;
    Iprovider->RenderTo = IDirectFBImageProvider_RenderTo;
    Iprovider->Release = IDirectFBImageProvider_Release;
    *p_provider = Iprovider;
    return 0;
}

static int IDirectFB_SetCooperativeLevel(IDirectFB *Idfb, int flags)
{
    struct IDirectFBState *dfb = Idfb->state;
    BSTD_UNUSED(flags);
}

static int IDirectFB_CreateInputEventBuffer(IDirectFB *Idfb, int flags, int unknown, IDirectFBEventBuffer **p_eventBuffer)
{
    struct IDirectFBState *dfb = Idfb->state;
    IDirectFBEventBuffer *IeventBuffer;
    
    BSTD_UNUSED(flags);
    BSTD_UNUSED(unknown);
    IeventBuffer = BKNI_Malloc(sizeof(*IeventBuffer));
    BKNI_Memset(IeventBuffer, 0, sizeof(*IeventBuffer));
    
    IeventBuffer->GetEvent = IDirectFBEventBuffer_GetEvent;
    IeventBuffer->Release = IDirectFBEventBuffer_Release;
    *p_eventBuffer = IeventBuffer;
    return 0;
}

static void IDirectFB_Release(IDirectFB *Idfb)
{
    struct IDirectFBState *dfb = Idfb->state;
    
    /* TODO: more cleanup */
    NEXUS_Display_Close(dfb->display);
    NEXUS_Graphics2D_Close(dfb->gfx);
    NEXUS_Platform_Uninit();
    
    bwin_close_engine(dfb->win_engine);
    
    BKNI_Free(dfb);
    BKNI_Free(Idfb);
}

static void event_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int DirectFBCreate(IDirectFB **p_dfb)
{
    struct IDirectFB *Idfb;
    struct IDirectFBState *dfb;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    int rc;
    NEXUS_GraphicsSettings displayGfxSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    bwin_engine_settings win_engine_settings;
    
    Idfb = BKNI_Malloc(sizeof(*Idfb));
    BKNI_Memset(Idfb, 0, sizeof(*Idfb));
    dfb = Idfb->state = BKNI_Malloc(sizeof(*dfb));
    BKNI_Memset(dfb, 0, sizeof(*dfb));
    
    Idfb->CreateSurface = IDirectFB_CreateSurface;
    Idfb->CreateImageProvider = IDirectFB_CreateImageProvider;
    Idfb->SetCooperativeLevel = IDirectFB_SetCooperativeLevel;
    Idfb->CreateInputEventBuffer = IDirectFB_CreateInputEventBuffer;
    Idfb->Release = IDirectFB_Release;
    
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return BERR_TRACE(-1);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    BKNI_CreateEvent(&dfb->checkpoint_event);
    BKNI_CreateEvent(&dfb->packetspace_available_event);
    
    dfb->gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    
    NEXUS_Graphics2D_GetSettings(dfb->gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = event_callback;
    gfxSettings.checkpointCallback.context = dfb->checkpoint_event;
    gfxSettings.packetSpaceAvailable.callback = event_callback;
    gfxSettings.packetSpaceAvailable.context = dfb->packetspace_available_event;
    rc = NEXUS_Graphics2D_SetSettings(dfb->gfx, &gfxSettings);
    if (rc) return BERR_TRACE(-1);
    
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    dfb->display = NEXUS_Display_Open(0, &displaySettings);
    if (!dfb->display) return BERR_TRACE(-1);
    
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &dfb->displayFormat);
    
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(dfb->display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    rc = NEXUS_Display_AddOutput(dfb->display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    BDBG_ASSERT(!rc);
#endif

    NEXUS_Display_GetGraphicsSettings(dfb->display, &displayGfxSettings);
    displayGfxSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(dfb->display, &displayGfxSettings);
    BDBG_ASSERT(!rc);
    
    bwin_engine_settings_init(&win_engine_settings);
    dfb->win_engine = bwin_open_engine(&win_engine_settings);
    BDBG_ASSERT(dfb->win_engine);
    
    *p_dfb = Idfb;
    return 0;
}

int DirectFBInit(int *argc, char ***argv)
{
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
    return 0;
}
