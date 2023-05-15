/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Font.h>

#include <GibbonFreetype.h>
#include <GibbonApplication.h>
#include <stdarg.h>

using namespace netflix::gibbon;
using namespace netflix;

void Font::init()
{
    //initFreeType();
}

void Font::cleanup()
{
    mFreetype.reset();
}

void Font::initClassification(Classification cls, int cnt, ...)
{
    std::vector<uint32_t> v;

    v.push_back(0); // Always consider .notdef as trimmable

    va_list ap;
    va_start(ap, cnt);
    for (int i = 0; i < cnt; ++i) {
        const uint32_t idx = mFreetype->fontIndex(va_arg(ap, uint32_t));
        if (idx)
            v.push_back(idx);
    }
    va_end(ap);

    std::sort(v.begin(), v.end());

    if (mClassifications.size() <= static_cast<size_t>(cls))
        mClassifications.resize(cls + 1);
    mClassifications[cls] = v;
}

bool Font::initFreeType(bool persist)
{
    assert(!mFreetype);
    mFreetype = GibbonFreetype::create(persist, mFileName, mPixelSize, mAttributes, mFontValues, mBbox, mTTF, mCMap);
    if (mFreetype) {
        initClassification(
            CLASS_SPACE, 19,
            0x0020, // space
            0x1680, // ogham space mark
            0x180e, // mongolian vowel separator
            0x2000, // en quad
            0x2001, // em quad
            0x2002, // en space
            0x2003, // em space
            0x2004, // three-per-em space
            0x2005, // four-per-em space
            0x2006, // six-per-em space
            0x2007, // figure space
            0x2008, // punctuation space
            0x2009, // thin space
            0x200a, // hair space
            0x202f, // narrow no-break space
            0x205f, // medium mathematical space
            0x3000, // ideographic space
            0x200b, // zero width space
            0x200c  // zero width non-joiner
            );
        return true;
    }
    return false;
}

Coordinate Font::getAscent(shared_ptr<netflix::gibbon::Style> style)
{
    if (!mFreetype && !initFreeType())
        return 0;
    return mFreetype->getAscent(style);
}

Coordinate Font::getDescent(shared_ptr<netflix::gibbon::Style> style)
{
    if (!mFreetype && !initFreeType())
        return 0;
    return mFreetype->getDescent(style);
}

std::string Font::getGlyphName(uint32_t idx)
{
    if (!mFreetype && !initFreeType())
        return "";
    return mFreetype->getGlyphName(idx);
}

void Font::unregister(const std::string& fileName, int attributes)
{
    GibbonFreetype::unregisterFont(fileName, attributes);
}
