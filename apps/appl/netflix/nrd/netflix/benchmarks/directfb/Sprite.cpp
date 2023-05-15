/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Sprite.h>

namespace netflix {

void Sprite::setColor(const Color& color)
{
    mFillColor = color;
}

void Sprite::setText(const char* text)
{
    mText = text;
}

void Sprite::setSurface(ISurface::SharedPtr surface)
{
    mSurface = surface;
    mRect.width = surface->getWidth();
    mRect.height = surface->getHeight();
}

void Sprite::tick()
{
    mRect.x += mVelocity.x;
    mRect.y += mVelocity.y;
}

void Sprite::setVelocity(int x, int y)
{
    mVelocity.x = x;
    mVelocity.y = y;
}

void Sprite::moveTo(int x, int y)
{
    mRect.x = x;
    mRect.y = y;
}

void Sprite::render(ISurface& screen)
{
    if (mFillColor.a != 0) {
        screen.fill(mRect, mFillColor);
    }
    if (mSurface) {
        Rect r(0, 0, mSurface->getWidth(), mSurface->getHeight());
        screen.blit(*mSurface, r, mRect);
    }
    if (mText.length() > 0) {
        screen.drawText(mText.c_str(), mRect);
    }
}


} // netflix
