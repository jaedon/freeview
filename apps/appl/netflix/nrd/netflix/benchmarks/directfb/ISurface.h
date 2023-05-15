/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ISURFACE_H
#define ISURFACE_H

#include <Rect.h>
#include <Color.h>
#include <tr1.h>
#include <string>

namespace netflix {

class ISurface
{
public:

    typedef shared_ptr<ISurface> SharedPtr;
    typedef weak_ptr<ISurface> WeakPtr;

    enum PixelFormat
    {
        PIXELFORMAT_DEFAULT,
        PIXELFORMAT_RGB,
        PIXELFORMAT_ARGB,
    };

    enum Flags
    {
        FLAGS_SCREEN_SURFACE = 1
    };

    enum CompositionMode
    {
        COMPMODE_NONE,
        COMPMODE_BLEND,
    };

    static ISurface::SharedPtr create();
    static ISurface::SharedPtr create(unsigned width, unsigned height, PixelFormat pf = PIXELFORMAT_DEFAULT, unsigned long flags = 0);
    static unsigned estimateSize(unsigned width, unsigned height, PixelFormat pf);

    virtual ~ISurface() {}

    virtual unsigned calculateSize() = 0;

    virtual void setCompositionMode(ISurface::CompositionMode mode) = 0;

    virtual void blit(ISurface& srcSurface, const Rect& srcRect, const Rect& dstRect) = 0;

    virtual void drawText(const char* text, const Rect& rect) = 0;

    virtual void clear() = 0;

    virtual void fill(const Rect& rect, const Color& color) = 0;

    virtual void drawRect(const Rect& rect, const Color& color) = 0;

    virtual bool load(const char* path) = 0;

    virtual bool load(const char* data, unsigned size) = 0;

    virtual int getHeight() const = 0;

    virtual int getWidth() const = 0;

    virtual int getDepth() const = 0;

    virtual ISurface::PixelFormat getPixelFormat() const = 0;

    virtual void setScale(float scale) = 0;

    virtual float getScale() const = 0;

    virtual float getRotationX() const = 0;

    virtual void setRotationX(float rotX) = 0;

    virtual float getRotationY() const = 0;

    virtual void setRotationY(float rotY) = 0;

    virtual float getRotationZ() const = 0;

    virtual void setRotationZ(float rotZ) = 0;

    virtual void setTransformOrigin(int x, int y) = 0;

    virtual void setClip(const Rect& clipRect) = 0;
    virtual void clearClip() = 0;

    virtual void setAlpha(unsigned alpha) = 0;
    virtual unsigned getAlpha() const = 0;

    virtual void setPerspective(int perspective) = 0;

    virtual int getPerspective() const = 0;

    virtual void flip(const Rect& r) = 0;

    protected:
        ISurface() {}
};

} // netflix

#endif // ISURFACE_H
