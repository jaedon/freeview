/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <ISurface.h>
#include <Color.h>
#include <Rect.h>
#include <Point.h>
#include <string>

namespace netflix {

class Sprite
{
public:
    Sprite() : mFillColor(0, 0, 0, 0) {}
    Sprite(ISurface::SharedPtr surface, const Color& color, const std::string text) :
            mSurface(surface), mFillColor(color), mText(text), mRect(0, 0, surface->getWidth(), surface->getHeight())
    {}
    ~Sprite() {}

    void setColor(const Color& color);
    void setText(const char* text);
    void setSurface(ISurface::SharedPtr surface);

    void tick();
    void setVelocity(int x, int y);
    void moveTo(int x, int y);

    void render(ISurface& screen);

private:

    ISurface::SharedPtr mSurface;
    Color mFillColor;
    std::string mText;

public:

    Rect mRect;
    Point mVelocity;

};

} // netflix

#endif // SPRITE_H
