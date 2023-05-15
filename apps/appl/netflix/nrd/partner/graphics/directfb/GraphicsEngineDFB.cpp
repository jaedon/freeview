/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <GraphicsEngine.h>
#include <GibbonApplication.h>
#include <SurfaceCache.h>
#include <Screen.h>

#include <nrdbase/Time.h>
#include <nrd/AppLog.h>

#include <list>
#include <stdio.h>
#include <string.h>

#if 0
# define GRAPHICSENGINE_DFB_DEBUG
#endif

using namespace netflix::gibbon;
using namespace netflix;

extern void dumpDFBSurfaceInfo(IDirectFBSurface* surface);

void GraphicsEngine::begin_sys()
{
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "** DFB::Init(%p)", mSurface.get());
#endif
}

void GraphicsEngine::end_sys()
{
    if (mSurface)
        setClip_sys(0);
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "** DFB::Cleanup(%p)", mSurface.get());
#endif
}

void GraphicsEngine::initCommands_sys()
{
}

void GraphicsEngine::setScale_sys(float scale)
{
    NRDP_UNUSED(scale);
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::SetScale(%p): %f", mSurface.get(), scale);
#endif
}

void GraphicsEngine::setOpacity_sys(float opacity)
{
    NRDP_UNUSED(opacity);
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::SetOpacity(%p): %f", mSurface.get(), opacity);
#endif
}

void GraphicsEngine::setSmoothScale_sys(bool smooth)
{
    IDirectFBSurface *dfb = mSurface->getDFBSurface();
    if(!dfb)
        return;

    if(dfb->SetRenderOptions) {
        DFBSurfaceRenderOptions opts = DSRO_NONE;
        if(smooth)
            opts |= DSRO_SMOOTH_UPSCALE | DSRO_SMOOTH_DOWNSCALE;
        DIRECTFB_CHECK(dfb->SetRenderOptions(dfb, opts));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
        Log::error(TRACE_GIBBON_GRAPHICS, "DFB::SetSmoothScale(%p): %d", mSurface.get(), smooth);
#endif
    }
}

void GraphicsEngine::setClip_sys(const Rect *r)
{
    IDirectFBSurface *dfb = mSurface->getDFBSurface();
    if(!dfb)
        return;

    if(r) {
        const DFBRegion rgn = r->toDFBRegion();
        DIRECTFB_CHECK(dfb->SetClip(dfb, &rgn));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
        Log::error(TRACE_GIBBON_GRAPHICS, "DFB::SetClip(%p): %s", mSurface.get(), r->toString().c_str());
#endif
    } else {
        DIRECTFB_CHECK(dfb->SetClip(dfb, 0));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
        Log::error(TRACE_GIBBON_GRAPHICS, "DFB::ClearClip(%p)", mSurface.get());
#endif
    }
}

void GraphicsEngine::setCompositionMode_sys(GraphicsEngine::CompositionMode mode)
{
    IDirectFBSurface *dfb = mSurface->getDFBSurface();
    if(!dfb)
        return;

    switch (mode) {
    case CompositionMode_SourceOver: {
        DIRECTFB_CHECK(dfb->SetDrawingFlags(dfb, DSDRAW_BLEND));
        break; }
    case CompositionMode_Source:
        DIRECTFB_CHECK(dfb->SetDrawingFlags(dfb, DSDRAW_NOFX));
        break;
    }

    const DFBSurfacePorterDuffRule porterDuffRules[] = { DSPD_SRC, DSPD_SRC_OVER };
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::SetCompositionMode(%p): %d (%d %d)", mSurface.get(), mode, porterDuffRules[0], porterDuffRules[1]);
#endif
    DIRECTFB_CHECK(dfb->SetPorterDuff(dfb, porterDuffRules[mode]));
}

void GraphicsEngine::setColor(const Color &color)
{
    const Surface::Format alpha_fmt = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Alpha);
    IDirectFBSurface *surface = mSurface->getDFBSurface();
    assert(surface);
    const float o = mState.opacity * (static_cast<float>(color.a) / 255.f);
    unsigned char r = color.r;
    unsigned char g = color.g;
    unsigned char b = color.b;
    if( alpha_fmt & Surface::Premultiplied ) {
        r = static_cast<unsigned char>(static_cast<float>(r) * o);
        g = static_cast<unsigned char>(static_cast<float>(g) * o);
        b = static_cast<unsigned char>(static_cast<float>(b) * o);
    }
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::SetColor(%p): %d %d %d %d (%f)", mSurface.get(), color.r, color.g, color.b, color.a, mState.opacity);
#endif
    DIRECTFB_CHECK(surface->SetColor(surface, r, g, b, static_cast<unsigned char>((static_cast<float>(color.a) * mState.opacity))));
}

void GraphicsEngine::blit_sys(const Surface::SharedPtr &srcSurface, const Rect &srcRect, const Rect &_dstRect, const Color *color)
{
    IDirectFBSurface *src_dfb = srcSurface->getDFBSurface();
    IDirectFBSurface *dst_dfb = mSurface->getDFBSurface();
    if(!src_dfb || !dst_dfb)
        return;
    const Rect dstRect = _dstRect.scaled(mState.scale, mState.scale);
    if(dstRect.empty())
        return;

    DFBSurfaceBlittingFlags flags = DSBLIT_NOFX;
    const bool srcAlpha = srcSurface->hasAlpha();
    const bool srcOpacity = !gibbon_float_compare(mState.opacity, 1.0);
    if(mState.compositionMode == CompositionMode_SourceOver && srcAlpha)
        flags |= DSBLIT_BLEND_ALPHACHANNEL;
    if(color) {
        flags |= DSBLIT_COLORIZE;
        if(srcOpacity) {
            flags |= DSBLIT_BLEND_COLORALPHA;
            DIRECTFB_CHECK(dst_dfb->SetColor(dst_dfb, color->r, color->g, color->b, mState.opacity * color->a));
        } else {
            if(color->a < 255)
                flags |= DSBLIT_BLEND_COLORALPHA;
            DIRECTFB_CHECK(dst_dfb->SetColor(dst_dfb, color->r, color->g, color->b, color->a));
        }
    } else if(srcOpacity) {
        flags |= DSBLIT_BLEND_COLORALPHA;
        DIRECTFB_CHECK(dst_dfb->SetColor(dst_dfb, 255, 255, 255, mState.opacity * 255));
    }
    if(srcAlpha && (srcSurface->mFormat & Surface::Premultiplied)) {
        if(srcOpacity)
            flags |= DSBLIT_SRC_PREMULTCOLOR;
    } else if(srcAlpha || srcOpacity) {
        flags |= DSBLIT_SRC_PREMULTIPLY;
    }
    mSurface->setBlittingFlags(flags);

    DFBRectangle srcDFBRect;
    srcDFBRect.x = srcRect.x;
    srcDFBRect.y = srcRect.y;
    srcDFBRect.w = srcRect.width;
    srcDFBRect.h = srcRect.height;

    DFBResult result = DFB_OK;
    if(srcRect.width != dstRect.width || srcRect.height != dstRect.height) {
        DFBRectangle dstDFBRect;
        dstDFBRect.x = dstRect.x;
        dstDFBRect.y = dstRect.y;
        dstDFBRect.w = dstRect.width;
        dstDFBRect.h = dstRect.height;

#if defined(GIBBON_DFB_STRETCH_INTO_TEMPORARY)
        if(mState.compositionMode != CompositionMode_Source) {
            dstDFBRect.x = dstDFBRect.y = 0;

            static Surface::WeakPtr tmpWeakSurface;
            Surface::SharedPtr tmpSurface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(tmpWeakSurface);
            Coordinate w = tmpSurface ? tmpSurface->getWidth() : 0;
            Coordinate h = tmpSurface ? tmpSurface->getHeight() : 0;

            IDirectFBSurface* tmpDFBSurface = 0;
            if(!tmpSurface || w < dstRect.width || h < dstRect.height) {
                if(dstRect.width > w)
                    w = dstRect.width;
                if(dstRect.height > h)
                    h = dstRect.height;
                unsigned char flags = Surface::Flags_RenderTarget;
                if(mSurface->hasAlpha())
                    flags |= Surface::Flags_Alpha;
                tmpSurface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(w, h, flags);
                tmpSurface->setDescription("DFB Temporary Scaler");
                tmpDFBSurface = tmpSurface->getDFBSurface();
                assert(tmpDFBSurface);
                tmpWeakSurface = tmpSurface;
                tmpDFBSurface->SetDrawingFlags(tmpDFBSurface, DSDRAW_NOFX);
                tmpDFBSurface->SetBlittingFlags(tmpDFBSurface, DSBLIT_NOFX);
                tmpDFBSurface->SetPorterDuff(tmpDFBSurface, DSPD_NONE);
            } else {
                tmpDFBSurface = tmpSurface->getDFBSurface();
                GibbonApplication::instance()->getSurfaceCache()->touch(tmpSurface);
            }
            assert(tmpSurface && tmpDFBSurface);
            //dumpDFBSurfaceInfo(tmpDFBSurface);
            if(tmpDFBSurface->SetRenderOptions) {
                DFBSurfaceRenderOptions opts = DSRO_NONE;
                if(mState.smoothScale)
                    opts |= DSRO_SMOOTH_UPSCALE | DSRO_SMOOTH_DOWNSCALE;
                DIRECTFB_CHECK(tmpDFBSurface->SetRenderOptions(tmpDFBSurface, opts));
            }
            tmpDFBSurface->StretchBlit(tmpDFBSurface, src_dfb, &srcDFBRect, &dstDFBRect);
            result = dst_dfb->Blit(dst_dfb, tmpDFBSurface, &dstDFBRect, dstRect.x, dstRect.y);
#if defined(GRAPHICSENGINE_DFB_DEBUG)
            Log::error(TRACE_GIBBON_GRAPHICS, "DFB::CachedBlit(%p/%p): (%s) %d %d %d %d -> %d %d %d %d (%f)", mSurface.get(), tmpSurface.get(), srcSurface->describe().c_str(),
                       srcDFBRect.x, srcDFBRect.y, srcDFBRect.w, srcDFBRect.h, dstRect.x, dstRect.y, dstDFBRect.w, dstDFBRect.h, mState.opacity);
#endif
        } else
#endif
        {
            result = dst_dfb->StretchBlit(dst_dfb, src_dfb, &srcDFBRect, &dstDFBRect);
#if defined(GRAPHICSENGINE_DFB_DEBUG)
            Log::error(TRACE_GIBBON_GRAPHICS, "DFB::StretchBlit(%p): (%s) %d %d %d %d -> %d %d %d %d (%f)", mSurface.get(), srcSurface->describe().c_str(),
                       srcDFBRect.x, srcDFBRect.y, srcDFBRect.w, srcDFBRect.h, dstDFBRect.x, dstDFBRect.y, dstDFBRect.w, dstDFBRect.h, mState.opacity);
#endif
        }
    } else {
        //     printf("blitting on 0x%02x from 0x%02x\n", mSurface->format(), srcSurface->format());
        result = dst_dfb->Blit(dst_dfb, src_dfb, &srcDFBRect, dstRect.x, dstRect.y);
#if defined(GRAPHICSENGINE_DFB_DEBUG)
        Log::error(TRACE_GIBBON_GRAPHICS, "DFB::Blit(%p): (%s) %d %d %d %d -> %d %d (%f)", mSurface.get(), srcSurface->describe().c_str(),
                   srcDFBRect.x, srcDFBRect.y, srcDFBRect.w, srcDFBRect.h, dstRect.x, dstRect.y, mState.opacity);
#endif
    }
    if(result != DFB_OK) {
        int sw, sh, dw, dh;
        src_dfb->GetSize(src_dfb, &sw, &sh);

        dst_dfb->GetSize(dst_dfb, &dw, &dh);
        Log::error(TRACE_GIBBON_GRAPHICS, "BLT FAILED!!! src %d %d %d %d of %d %d - dest %d %d %d %d of %d %d - %s",
                   srcDFBRect.x, srcDFBRect.y, srcDFBRect.w, srcDFBRect.h, sw, sh,
                   dstRect.x, dstRect.y,
                   dstRect.width, dstRect.height,
                   dw, dh, DirectFBErrorString(result));
    }
}

void GraphicsEngine::drawText_sys(hb_buffer_t* text, int start, int end, const Rect& r, const Rect& dstRect, const Style::SharedPtr &style) /* TODO: Check whether too large strings forward to dfb/fusion  */
{
    drawText_internal(text, start, end, r, dstRect, style);
}

void GraphicsEngine::clear_sys(const Rect &_rect)
{
    IDirectFBSurface *dfb = mSurface->getDFBSurface();
    if(!dfb)
        return;
    const Rect rect = _rect.scaled(mState.scale, mState.scale);
    if(rect.empty())
        return;

    DIRECTFB_CHECK(dfb->SetColor(dfb, 0, 0, 0, 0));
    DIRECTFB_CHECK(dfb->SetDrawingFlags(dfb, DSDRAW_NOFX));
    DIRECTFB_CHECK(dfb->FillRectangle(dfb, rect.x, rect.y, rect.width, rect.height));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::Clear(%p): %d %d %d %d", mSurface.get(), rect.x, rect.y, rect.width, rect.height);
#endif
    setCompositionMode_sys(mState.compositionMode);
}

void GraphicsEngine::fill_sys(const Rect &_rect, const Color &c)
{
    IDirectFBSurface *dfb = mSurface->getDFBSurface();
    if(!dfb)
        return;
    const Rect rect = _rect.scaled(mState.scale, mState.scale);
    if(rect.empty())
        return;

    setColor(c);
    DIRECTFB_CHECK(dfb->FillRectangle(dfb, rect.x, rect.y, rect.width, rect.height));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::Fill(%p): %d %d %d %d #%02x%02x%02x%02x", mSurface.get(),
               rect.x, rect.y, rect.width, rect.height, c.r, c.g, c.b, c.a);
#endif
}

void GraphicsEngine::drawRect_sys(const Rect &_rect, const Color &c)
{
    IDirectFBSurface *dfb = mSurface->getDFBSurface();
    if(!dfb)
        return;
    const Rect rect = _rect.scaled(mState.scale, mState.scale);
    if(rect.empty())
        return;

    setColor(c);
    DIRECTFB_CHECK(dfb->DrawRectangle(dfb, rect.x, rect.y, rect.width, rect.height));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::Draw(%p): %d %d %d %d #%02x%02x%02x%02x", mSurface.get(),
               rect.x, rect.y, rect.width, rect.height, c.r, c.g, c.b, c.a);
#endif
}

void GraphicsEngine::batchBlit_sys(const Surface::SharedPtr &surface, const std::vector<Rect> &src,
                                   const std::vector<Rect> &dst, const Color *color)
{
    assert(surface && surface->isValid());
    assert(src.size() == dst.size());

    IDirectFBSurface *src_dfb = surface->getDFBSurface();
    IDirectFBSurface *dst_dfb = mSurface->getDFBSurface();
    if(!src_dfb || !dst_dfb)
        return;

    DFBSurfaceBlittingFlags flags = DSBLIT_NOFX;
    const bool srcAlpha = surface->hasAlpha();
    const bool srcOpacity = !gibbon_float_compare(mState.opacity, 1.0);
    if(mSurface->mFormat & Surface::Premultiplied) {
        if((srcAlpha || srcOpacity) && !(surface->mFormat & Surface::Premultiplied))
            flags |= DSBLIT_SRC_PREMULTIPLY;
    }
    if(mState.compositionMode == CompositionMode_SourceOver && srcAlpha)
        flags |= DSBLIT_BLEND_ALPHACHANNEL;

    if(color) {
        flags |= DSBLIT_COLORIZE;
        if(color->a == 255 && !srcOpacity)
            flags |= DSBLIT_SRC_PREMULTCOLOR;
        if(srcOpacity) {
            flags |= DSBLIT_BLEND_COLORALPHA;
            DIRECTFB_CHECK(dst_dfb->SetColor(dst_dfb, color->r, color->g, color->b, mState.opacity * color->a));
        } else {
            if(color->a < 255)
                flags |= DSBLIT_BLEND_COLORALPHA;
            DIRECTFB_CHECK(dst_dfb->SetColor(dst_dfb, color->r, color->g, color->b, color->a));
        }
    } else if(srcOpacity) {
        flags |= DSBLIT_BLEND_COLORALPHA;
        DIRECTFB_CHECK(dst_dfb->SetColor(dst_dfb, 255, 255, 255, mState.opacity * 255));
    }
    mSurface->setBlittingFlags(flags);

    size_t sz = src.size();

    std::vector<DFBRectangle> rects;
    std::vector<DFBPoint> points;
    rects.reserve(sz);
    points.reserve(sz);

    for(size_t i = 0; i < sz; ++i) {
        const Rect& sr = src.at(i);
        const Rect& dr = dst.at(i);
        const DFBRectangle r = sr.toDFBRect();
        const DFBPoint p = dr.topLeft().toDFBPoint();
        rects.push_back(r);
        points.push_back(p);
    }

    assert(rects.size() == src.size());
    assert(points.size() == dst.size());

    enum { BlitSize = 1024 };
    unsigned int pos = 0;
    while (sz > BlitSize) {
        DIRECTFB_CHECK(dst_dfb->BatchBlit(dst_dfb, src_dfb, &rects[pos], &points[pos], BlitSize));
        sz -= BlitSize;
        pos += BlitSize;
    }
    if (sz) {
        DIRECTFB_CHECK(dst_dfb->BatchBlit(dst_dfb, src_dfb, &rects[pos], &points[pos], sz));
    }
}

void GraphicsEngine::flush_sys()
{
    if(IDirectFB *d = GibbonApplication::instance()->getScreen()->getDirectFB())
        d->WaitIdle(d);
}

