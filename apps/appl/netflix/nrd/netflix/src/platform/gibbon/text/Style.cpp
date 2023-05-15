/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Style.h>
#include <Screen.h>
#include <FontManager.h>
#include <GibbonConfiguration.h>

#include "Style.h"

using namespace netflix;
using namespace netflix::gibbon;

Style::SharedPtr Style::sDefaultStyle;

Style::Style()
:
    mWrap(false),
    mFamily("Arial_for_Netflix"),
    mPixelSize(30),
    mLineHeight(CoordinateMax),
    mIndent(0),
    mWeight(Font::WEIGHT_BOLD),
    mStyle(Font::STYLE_NORMAL),
    mColor(0, 0, 0),
    mBackgroundColor(0, 0, 0, 0),
    mShadow(0, 0),
    mPadding(0, 0),
    mAlign(ALIGN_LEFT),
    mMaxLines(0),
    mUnderline(false),
    mStrikethrough(false),
    mRichText(false),
    mDirection(GibbonConfiguration::defaultDirection() == "rtl" ? 1 : 0),
    mNeedsPDI(false),
    mRise(0),
    mLocale(GibbonConfiguration::defaultLocale()),
    mScript("")
{
    if(Screen::SharedPtr screen = gibbonApp()->getScreen())
        mPixelSize = uint32_t(mPixelSize * screen->getCoordinateScale());
}

Style::~Style()
{
}

Font::SharedPtr Style::getFont() const
{
    if (!mFont)
        mFont = FontManager::getFont(mFamily, mPixelSize, mWeight, mStyle);
    return mFont;
}

void Style::setFont(Font::SharedPtr font)
{
    if (font && font != mFont) {
        setFontFamily(mFont->getFamily());
        setFontWeight(mFont->getWeight());
        setFontStyle(mFont->getStyle());
        mFont = font;
    }
}

void Style::setFontFamily(std::string family)
{
    if (mFamily != family) {
        mFamily = family;
        dirtyFont();
    }
}

void Style::setFontPixelSize(uint32_t size)
{
    if (mPixelSize != size) {
        mPixelSize = size;
        dirtyFont();
    }
}

void Style::setLineHeight(Coordinate size)
{
    if (size <= 0)
        size = CoordinateMax;
    if (mLineHeight != size) {
        mLineHeight = size;
        dirtyFont();
    }
}

void Style::setFontWeight(Font::Weight weight)
{
    if (mWeight != weight) {
        mWeight = weight;
        dirtyFont();
    }
}

void Style::setFontStyle(Font::Style style)
{
    if (mStyle != style) {
        mStyle = style;
        dirtyFont();
    }
}

void Style::setLocale(const std::string& locale)
{
    if (mLocale != locale) {
        mLocale = locale;
        if(mLocale.empty())
            mLocale = GibbonConfiguration::defaultLocale();
        dirtyFont();
    }
}

uint8_t Style::diff(Style::SharedPtr style) const
{
    uint8_t result = TextEngine::DIFF_NONE;
    if(style) {
        result |= mShadow.diff(style->mShadow);
        result |= mEdgeEffect.diff(style->mEdgeEffect);
        if(style->mWrap != mWrap || style->mPixelSize != mPixelSize ||  style->mLineHeight != mLineHeight ||
           style->mStyle != mStyle || style->mWeight != mWeight ||
           style->mAlign != mAlign || style->mMaxLines != mMaxLines ||
           style->mUnderline != mUnderline || style->mStrikethrough != mStrikethrough ||
           style->mRichText != mRichText || style->mDirection != mDirection || style->mNeedsPDI != mNeedsPDI ||
           style->mScript != mScript || style->mRise != mRise ||
           style->mLocale != mLocale || style->mFamily != mFamily ||
           style->mTruncation != mTruncation || style->mTypography != mTypography)
            result |= TextEngine::DIFF_LAYOUT|TextEngine::DIFF_RENDER;
        else if(style->mColor != mColor || style->mBackgroundColor != mBackgroundColor ||
                style->mCursor != mCursor)
            result |= TextEngine::DIFF_RENDER;
    } else {
        result |= TextEngine::DIFF_LAYOUT|TextEngine::DIFF_RENDER;
    }
    return result;
}
