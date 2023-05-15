#ifndef GIBBONHARFBUZZ_H
#define GIBBONHARFBUZZ_H

/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <gibbon/config.h>

#include "GibbonFreetype.h"
#include <FontManagerValues.h>
#include <map>

struct hb_font_t;
struct hb_font_funcs_t;
struct hb_unicode_funcs_t;

typedef struct FT_SizeRec_*  FT_Size;

namespace netflix {
namespace gibbon {

class GibbonHarfbuzz
{
public:
    GibbonHarfbuzz(GibbonFreetype* ft, FT_Size s, int attributes, const FontValues& fontValues);
    ~GibbonHarfbuzz();

    hb_font_t* font() const { return mFont; }
    hb_font_funcs_t* funcs() const { return mFuncs; }
    FT_Size size() const { return mSize; }
    GibbonFreetype* freetype() const { return mFreetype; }
    int attributes() const { return mAttributes; }
    const FontValues& fontValues() const { return mFontValues; }

    void setKerning(bool kerning) { mKerning = kerning; }
    bool kerning() const { return mKerning; }

    bool hasCodepoint(uint32_t cp) const;

    const ftcache::Glyph::SharedPtr& glyph(uint32_t cp) const;
    const ftcache::Glyph* requestGlyph(uint32_t cp);

    static Variant getLibraryInfo();
    static Variant getGraphiteInfo();
    static Variant getICUInfo();

private:
    hb_font_t* mFont;
    hb_font_funcs_t* mFuncs;
    hb_unicode_funcs_t* mUnicode;
    std::vector<ftcache::Glyph::SharedPtr> mGlyphs;
    ftcache::Glyph::SharedPtr mEmptyGlyph;

    GibbonFreetype* mFreetype;
    FT_Size mSize;
    int mAttributes;
    FontValues mFontValues;
    bool mKerning;
};

inline const ftcache::Glyph::SharedPtr& GibbonHarfbuzz::glyph(uint32_t cp) const
{
    if (cp < mGlyphs.size())
        return mGlyphs[cp];
    return mEmptyGlyph;
}

}} // namespace netflix::gibbon

#endif // GIBBONHARFBUZZ_H
