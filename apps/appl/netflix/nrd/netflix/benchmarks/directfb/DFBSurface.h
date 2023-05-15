/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DFBSurface_h
#define DFBSurface_h

#include <ISurface.h>
#include <Rect.h>

#include <directfb.h>

namespace netflix {

class DFBSurface : public ISurface
{
public:

    virtual ~DFBSurface();

    unsigned calculateSize();

    void setCompositionMode(ISurface::CompositionMode mode);

    void blit(ISurface& srcSurface, const Rect& srcRect, const Rect& dstRect);

    void clear();

    void fill(const Rect& rect, const Color& c);

    void drawRect(const Rect& rect, const Color& color);

    void drawText(const char* text, const Rect& rect);

    bool load(const char* path);

    bool load(const char* data, unsigned size);

    int getHeight() const;
    int getWidth() const;
    int getDepth() const;
    ISurface::PixelFormat getPixelFormat() const;

    void setScale(float scale)
    {
        mScale = scale;
    }

    float getScale() const
    {
        return mScale;
    }

    float getRotationX() const
    {
        return 0;
    }

    void setRotationX(float)
    {
    }

    float getRotationY() const
    {
        return 0;
    }

    void setRotationY(float)
    {
    }

    float getRotationZ() const
    {
        return 0;
    }

    void setRotationZ(float)
    {
    }

    void setTransformOrigin(int /*x*/, int /*y*/)
    {
    }

    void clearClip();
    void setClip(const Rect& clipRect);

    void setAlpha(unsigned alpha);
    unsigned getAlpha() const;

    void setPerspective(int /*perspective*/)
    {
    }

    int getPerspective() const
    {
        return 0;
    }

    IDirectFBSurface* getDFBSurface() const
    {
        return mSurface;
    }

    void flip(const Rect &r);

    static void setDirectFB(IDirectFB* dfb);
    static void setDirectFBFont(IDirectFBFont* font);

protected:

    friend class ISurface;

    static IDirectFB* getDirectFB();

    DFBSurface();
    DFBSurface(unsigned width, unsigned height, ISurface::PixelFormat pf, unsigned long flags);

    ISurface::CompositionMode mCompMode;
    IDirectFBSurface* mSurface;
    unsigned mAlpha;
    float mScale;

private:
};

} // netflix

#endif
