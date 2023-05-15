/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EffectsDFB.h"
#include <SurfaceCache.h>
#include <Screen.h>

using namespace netflix;
using namespace netflix::gibbon;

DisplayList::SharedPtr EffectMaskDFB::DFBRenderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    if(Surface::SharedPtr mask = mEffect->getMask()) {
        Surface::SharedPtr surface = dl->flatten();
        const Rect dstRect = getRect(area);
        Surface::SharedPtr result = requestSurface(dstRect.width, dstRect.height);
        IDirectFBSurface *dfb = result->getDFBSurface();

        DFBSurfaceBlittingFlags flags = DSBLIT_SRC_MASK_ALPHA;
        flags |= DSBLIT_BLEND_ALPHACHANNEL;
        if (surface->hasAlpha() && !(surface->format() & Surface::Premultiplied))
            flags |= DSBLIT_SRC_PREMULTIPLY;
        DIRECTFB_CHECK(dfb->SetBlittingFlags(dfb, flags));
        DIRECTFB_CHECK(dfb->SetSourceMask(dfb, mask->getDFBSurface(), 0, 0, DSMF_NONE));
        DIRECTFB_CHECK(dfb->SetDrawingFlags(dfb, DSDRAW_BLEND));
        DIRECTFB_CHECK(dfb->SetPorterDuff(dfb, DSPD_SRC_OVER));

        DFBRectangle srcDFBRect;
        srcDFBRect.x = area.x;
        srcDFBRect.y = area.y;
        srcDFBRect.w = area.width;
        srcDFBRect.h = area.height;
        DIRECTFB_CHECK(dfb->Blit(dfb, surface->getDFBSurface(), &srcDFBRect, 0, 0));
        return DisplayList::SharedPtr(new DisplayList(result));
    }
    return dl;
}

DisplayList::SharedPtr EffectTransformDFB::DFBRenderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    Surface::SharedPtr surface = dl->flatten();

    const Rect dstRect = getRect(area);
    Surface::SharedPtr result = requestSurface(dstRect.size());
    IDirectFBSurface *dfb = result->getDFBSurface();

    DFBSurfaceBlittingFlags flags = DSBLIT_NOFX;
    if (surface->hasAlpha() && !(surface->format() & Surface::Premultiplied))
        flags |= DSBLIT_SRC_PREMULTIPLY;
    DIRECTFB_CHECK(dfb->SetBlittingFlags(dfb, flags));
    DIRECTFB_CHECK(dfb->SetDrawingFlags(dfb, DSDRAW_BLEND));
    const Transform transform = getTransform();
    s32 dfbMatrix[9];
    for(int i = 0, x = 0; x < 3; ++x) {
        for(int y = 0; y < 3; ++y, ++i) {
            const double f = fabs(transform.matrix(x, y));
            const int ip = (int)f;
            const int dp = (int)((f-ip)*1000);
            dfbMatrix[i] = ((ip&0xFFFF)<<16) | (dp&0xFFFF);
            if(transform.matrix(x, y) < 0)
                dfbMatrix[i] = -dfbMatrix[i];
        }
    }
    DIRECTFB_CHECK(dfb->SetMatrix(dfb, dfbMatrix));
    DIRECTFB_CHECK(dfb->SetRenderOptions(dfb, DSRO_ALL));

    DFBRectangle srcDFBRect;
    srcDFBRect.x = area.x;
    srcDFBRect.y = area.y;
    srcDFBRect.w = area.width;
    srcDFBRect.h = area.height;
    DIRECTFB_CHECK(dfb->Blit(dfb, surface->getDFBSurface(), &srcDFBRect, 0, 0));
    return DisplayList::SharedPtr(new DisplayList(result));
}
