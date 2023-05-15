/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonHarfbuzz.h"
#include <FontManager.h>

#include <hb.h>
#include <hb-ft.h>

#include <nrdbase/Log.h>
#include <nrd/AppLog.h>
#include <GibbonConfiguration.h>
# include <graphite2/Font.h>
# include <unicode/utypes.h>
# include <unicode/uversion.h>
# include <unicode/ures.h>
# include <nrdbase/ReadDir.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ADVANCES_H
#include FT_MODULE_H
#include FT_SIZES_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_OUTLINE_H
#include FT_STROKER_H

using namespace netflix::gibbon;
using namespace netflix;

static hb_bool_t getGlyph(hb_font_t* font, void* font_data, hb_codepoint_t unicode,
                          hb_codepoint_t variation_selector, hb_codepoint_t* glyph, void* user_data);
static hb_position_t getGlyphHAdvance(hb_font_t *font, void *font_data, hb_codepoint_t glyph, void *user_data);
static hb_position_t getGlyphVAdvance(hb_font_t *font, void *font_data, hb_codepoint_t glyph, void *user_data);
static hb_bool_t getGlyphHOrigin(hb_font_t *font, void *font_data, hb_codepoint_t glyph,
                                 hb_position_t *x, hb_position_t *y, void *user_data);
static hb_bool_t getGlyphVOrigin(hb_font_t *font, void *font_data, hb_codepoint_t glyph,
                                 hb_position_t *x, hb_position_t *y, void *user_data);
static hb_position_t getGlyphHKerning(hb_font_t *font, void *font_data,
                                      hb_codepoint_t left_glyph, hb_codepoint_t right_glyph, void *user_data);
static hb_position_t getGlyphVKerning(hb_font_t *font, void *font_data,
                                      hb_codepoint_t left_glyph, hb_codepoint_t right_glyph, void *user_data);
static hb_bool_t getGlyphExtents(hb_font_t *font, void *font_data, hb_codepoint_t glyph,
                                 hb_glyph_extents_t *extents, void *user_data);
static hb_bool_t getGlyphContourPoint(hb_font_t *font, void *font_data, hb_codepoint_t glyph, unsigned int point_index,
                                      hb_position_t *x, hb_position_t *y, void *user_data);
static hb_bool_t getGlyphName(hb_font_t *font, void *font_data, hb_codepoint_t glyph,
                              char *name, unsigned int size, void *user_data);
static hb_bool_t getGlyphFromName(hb_font_t *font, void *font_data, const char *name, int len,
                                  hb_codepoint_t *glyph, void *user_data);

Variant GibbonHarfbuzz::getLibraryInfo()
{
    Variant version;

    version["name"] = "harfbuzz";
    version["compiled"] = HB_VERSION_STRING;
    version["runtime"] = hb_version_string();

    std::string shapers;
    shapers = "shapers: ";

    const char **shaper_list = hb_shape_list_shapers();
    for (; *shaper_list; shaper_list++) {
        shapers.append(*shaper_list).append(",");
    }
    if (shapers.size() > 1)
        shapers.resize(shapers.size() - 1);

    version["options"] = shapers;
    return version;
}

Variant GibbonHarfbuzz::getGraphiteInfo()
{
    Variant version;
    version["name"] = "graphite2";

    std::ostringstream os;
    os << GR2_VERSION_MAJOR << "." << GR2_VERSION_MINOR << "." << GR2_VERSION_BUGFIX;
    version["compiled"] = os.str();
    os.str("");

    int major = 0, minor = 0, bugFix = 0;
    gr_engine_version(&major, &minor, &bugFix);

    os << major << "." << minor << "." << bugFix;
    version["runtime"] = os.str();

    return version;
}


Variant GibbonHarfbuzz::getICUInfo()
{
    Variant version;
    version["name"] = "icu";
    version["compiled"] = U_ICU_VERSION;

    UVersionInfo icuVersion;
    char icuVersionString[U_MAX_VERSION_STRING_LENGTH];

    u_getVersion(icuVersion);
    u_versionToString(icuVersion, icuVersionString);
    version["runtime"] = icuVersionString;

    std::string options;
    std::ostringstream icuData;
    std::ostringstream icuDataDir;

    icuData << "icudt" << U_ICU_VERSION_MAJOR_NUM << U_ICUDATA_TYPE_LETTER;
    options.append("data: ").append(icuData.str()).append("; ");


    options.append("dictionaries: ");
    icuDataDir << (Configuration::dataReadPath() + "/icu/").c_str() << icuData.str() << "/brkitr";
    bool files = false;
    const std::vector<ReadDir::Entry> entries = ReadDir::entries(icuDataDir.str());
    for (std::vector<ReadDir::Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        std::string filename(it->fileName());
        std::string::size_type idx;
        idx = filename.rfind('.');
        if(idx != std::string::npos) {
            if (filename.substr(idx + 1) == "dict") {
                options.append(filename).append(",");
                files = true;
            }
        }
    }
    if (files)
        options.resize(options.size() - 1); // remove trailing comma


    version["options"] = options;

    return version;
}


GibbonHarfbuzz::GibbonHarfbuzz(GibbonFreetype* freetype, FT_Size size, int attrs, const FontValues& values)
    : mFreetype(freetype), mSize(size), mAttributes(attrs), mFontValues(values), mKerning(true)
{
    mFuncs = hb_font_funcs_create();
    hb_font_funcs_set_glyph_func(mFuncs, getGlyph, this, 0);
    hb_font_funcs_set_glyph_h_advance_func(mFuncs, getGlyphHAdvance, this, 0);
    hb_font_funcs_set_glyph_v_advance_func(mFuncs, getGlyphVAdvance, this, 0);
    hb_font_funcs_set_glyph_h_origin_func(mFuncs, getGlyphHOrigin, this, 0);
    hb_font_funcs_set_glyph_v_origin_func(mFuncs, getGlyphVOrigin, this, 0);
    hb_font_funcs_set_glyph_h_kerning_func(mFuncs, getGlyphHKerning, this, 0);
    hb_font_funcs_set_glyph_v_kerning_func(mFuncs, getGlyphVKerning, this, 0);
    hb_font_funcs_set_glyph_extents_func(mFuncs, getGlyphExtents, this, 0);
    hb_font_funcs_set_glyph_contour_point_func(mFuncs, getGlyphContourPoint, this, 0);
    hb_font_funcs_set_glyph_name_func(mFuncs, getGlyphName, this, 0);
    hb_font_funcs_set_glyph_from_name_func(mFuncs, getGlyphFromName, this, 0);

    FT_Select_Charmap(size->face, FT_ENCODING_UNICODE);
    hb_face_t* face = hb_ft_face_create(size->face, 0);
    mFont = hb_font_create(face);
    hb_face_destroy(face);
    hb_font_set_funcs(mFont, mFuncs, this, 0);
    hb_font_set_scale(mFont,
                      (int) (((uint64_t) size->metrics.x_scale * (uint64_t) size->face->units_per_EM + (1<<15)) >> 16),
                      (int) (((uint64_t) size->metrics.y_scale * (uint64_t) size->face->units_per_EM + (1<<15)) >> 16));
    hb_font_set_ppem(mFont,
                     size->metrics.x_ppem,
                     size->metrics.y_ppem);

    mUnicode = hb_unicode_funcs_get_default();
}

GibbonHarfbuzz::~GibbonHarfbuzz()
{
    hb_font_funcs_destroy(mFuncs);
    hb_font_destroy(mFont);
}

const ftcache::Glyph* GibbonHarfbuzz::requestGlyph(uint32_t cp)
{
    ftcache::Glyph::SharedPtr g;
    if (cp < mGlyphs.size())
        g = mGlyphs[cp];
    if (g)
        return g.get();
    g = mFreetype->requestGlyph(cp, 0, GibbonFreetype::RequestGeometry);
    if (g) {
        if (cp >= mGlyphs.size())
            mGlyphs.resize(cp + 1);
        mGlyphs[cp] = g;
        return g.get();
    }
    return 0;
}

bool GibbonHarfbuzz::hasCodepoint(uint32_t cp) const
{
    hb_codepoint_t idx;
    if (hb_font_get_glyph(mFont, cp, 0, &idx))
        return true;

    // Do not decompose halfwidth and fullwidth forms
    if (cp >= 0xFF00 && cp <= 0xFFEF) {
        return false;
    }

/*
    hb_codepoint_t decomposed[HB_UNICODE_MAX_DECOMPOSITION_LEN];
    const unsigned int cnt = hb_unicode_decompose_compatibility(mUnicode, cp, decomposed);
    if (cnt) {
        for (unsigned int i = 0; i < cnt; ++i) {
            if (!hb_font_get_glyph(mFont, decomposed[i], 0, &idx))
                return false;
        }
        return true;
    }
*/

    return false;
}

// Code based on hb-ft.cc from HarfBuzz, MIT Licensed:

/*
 * Copyright (C) 2009  Red Hat, Inc.
 * Copyright (C) 2009  Keith Stribley
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Red Hat Author(s): Behdad Esfahbod
 */

hb_bool_t getGlyph(hb_font_t* /*font*/, void* font_data, hb_codepoint_t unicode,
                   hb_codepoint_t variation_selector, hb_codepoint_t* glyph, void* /*user_data*/)
{
    GibbonHarfbuzz* hb = static_cast<GibbonHarfbuzz*>(font_data);
    FT_Face face = hb->size()->face;
    if (variation_selector) {
        *glyph = FT_Face_GetCharVariantIndex(face, unicode, variation_selector);
        return *glyph != 0;
    }

    *glyph = FT_Get_Char_Index(face, unicode);
    if (!*glyph && unicode != 0x00a0 && unicode != 0x200b && unicode != 0x200c &&
                   unicode != 0x200d && unicode != 0x200e && unicode != 0x200f &&
                   unicode != 0xfeff && unicode != 0x2029) {
        // Do not report missing:
        //  U+00A0 (NON-BREAKING SPACE)
        //  U+200B (ZERO WIDTH SPACE)
        //  U+200C (ZERO WIDTH NON-JOINER)
        //  U+200D (ZERO WIDTH JOINER)
        //  U+200E (LEFT-TO-RIGHT MARK)
        //  U+200F (RIGHT-TO-LEFT MARK)
        //  U+FEFF (ZERO WIDTH NO-BREAK SPACE)
        //  U+2029 (PARAGRAPH SEPARATOR)

        NTRACES(TRACE_GIBBON_FONTS, "[harfbuzz] No glyph for code point U+" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << unicode);
    }
    return *glyph != 0;
}

hb_position_t getGlyphHAdvance(hb_font_t */*font*/, void *font_data,
                               hb_codepoint_t glyph, void */*user_data*/)
{
    GibbonHarfbuzz* hb = static_cast<GibbonHarfbuzz*>(font_data);
    const ftcache::Glyph* ftg = hb->requestGlyph(glyph);
    if (!ftg)
        return 0;
    return ftg->origAdvance;
}

hb_position_t getGlyphVAdvance(hb_font_t */*font*/, void *font_data,
                               hb_codepoint_t glyph, void */*user_data*/)
{
    GibbonHarfbuzz* hb = static_cast<GibbonHarfbuzz*>(font_data);
    FT_Face face = hb->size()->face;
    const int load_flags = hb->freetype()->loadFlags() | FT_LOAD_VERTICAL_LAYOUT;
    FT_Fixed v;

    if (FT_Get_Advance(face, glyph, load_flags, &v))
        return 0;

    /* Note: FreeType's vertical metrics grows downward while other FreeType coordinates
     * have a Y growing upward.  Hence the extra negation. */
    return (-v + (1<<9)) >> 10;
}

hb_bool_t getGlyphHOrigin(hb_font_t */*font*/, void */*font_data*/, hb_codepoint_t /*glyph*/,
                          hb_position_t */*x*/, hb_position_t */*y*/, void */*user_data*/)
{
    return true;
}

hb_bool_t getGlyphVOrigin(hb_font_t */*font*/, void *font_data, hb_codepoint_t glyph,
                          hb_position_t *x, hb_position_t *y, void */*user_data*/)
{
    GibbonHarfbuzz* hb = static_cast<GibbonHarfbuzz*>(font_data);
    const ftcache::Glyph* ftg = hb->requestGlyph(glyph);
    if (!ftg)
        return false;

    /* Note: FreeType's vertical metrics grows downward while other FreeType coordinates
     * have a Y growing upward.  Hence the extra negation. */
    *x = ftg->horiBearingX -   ftg->vertBearingX;
    *y = ftg->horiBearingY - (-ftg->vertBearingY);

    return true;
}

hb_position_t getGlyphHKerning(hb_font_t *font, void *font_data,
                               hb_codepoint_t left_glyph, hb_codepoint_t right_glyph,
                               void */*user_data*/)
{
    GibbonHarfbuzz* hb = static_cast<GibbonHarfbuzz*>(font_data);
    if (!hb->kerning())
        return 0;
    FT_Face face = hb->size()->face;
    FT_Vector kerningv;

    unsigned int xppem, yppem;
    hb_font_get_ppem(font, &xppem, &yppem);

    FT_Kerning_Mode mode = xppem ? FT_KERNING_DEFAULT : FT_KERNING_UNFITTED;
    if (FT_Get_Kerning(face, left_glyph, right_glyph, mode, &kerningv))
        return 0;

    return kerningv.x;
}

hb_position_t getGlyphVKerning(hb_font_t */*font*/, void */*font_data*/,
                               hb_codepoint_t /*left_glyph*/, hb_codepoint_t /*right_glyph*/,
                               void */*user_data*/)
{
    return 0;
}

hb_bool_t getGlyphExtents(hb_font_t */*font*/, void *font_data, hb_codepoint_t glyph,
                          hb_glyph_extents_t *extents, void */*user_data*/)
{
    GibbonHarfbuzz* hb = static_cast<GibbonHarfbuzz*>(font_data);
    const ftcache::Glyph* ftg = hb->requestGlyph(glyph);
    if (!ftg)
        return false;

    extents->x_bearing = ftg->horiBearingX;
    extents->y_bearing = ftg->horiBearingY;
    extents->width = ftg->width;
    extents->height = ftg->height;

    return true;
}

hb_bool_t getGlyphContourPoint(hb_font_t */*font*/, void *font_data, hb_codepoint_t glyph,
                               unsigned int point_index,
                               hb_position_t *x, hb_position_t *y, void */*user_data*/)
{
    GibbonHarfbuzz* hb = static_cast<GibbonHarfbuzz*>(font_data);
    FT_Face face = hb->size()->face;
    const int load_flags = hb->freetype()->loadFlags();

    if (FT_Load_Glyph(face, glyph, load_flags))
        return false;
    if (hb->attributes() & FontManager::Slant) {
        FT_Matrix transform;
        transform.xx = 0x10000L;
        transform.yx = 0x00000L;

        transform.xy = static_cast<FT_Fixed>(hb->fontValues().slantFactor * 65536); // convert to 16.16 format
        transform.yy = 0x10000L;

        FT_Outline_Transform(&face->glyph->outline, &transform);
    }
    if (hb->attributes() & FontManager::Embolden) {
        FT_Outline_Embolden(&face->glyph->outline,
                            FT_MulFix(static_cast<FT_Long>(face->units_per_EM * hb->fontValues().emboldenFactor),
                                      face->size->metrics.y_scale) / 24);
    }

    if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
        return false;

    if (point_index >= (unsigned int) face->glyph->outline.n_points)
        return false;

    *x = face->glyph->outline.points[point_index].x;
    *y = face->glyph->outline.points[point_index].y;

    return true;
}

hb_bool_t getGlyphName(hb_font_t */*font*/, void *font_data, hb_codepoint_t glyph,
                       char *name, unsigned int size, void */*user_data*/)
{
    FT_Face face = static_cast<GibbonHarfbuzz*>(font_data)->size()->face;

    hb_bool_t ret = !FT_Get_Glyph_Name(face, glyph, name, size);
    if (ret && (size && !*name))
        ret = false;

    return ret;
}

hb_bool_t getGlyphFromName(hb_font_t */*font*/, void *font_data, const char *name, int len,
                           hb_codepoint_t *glyph, void */*user_data*/)
{
    FT_Face face = static_cast<GibbonHarfbuzz*>(font_data)->size()->face;

    if (len < 0) {
        *glyph = FT_Get_Name_Index(face, (FT_String *) name);
    } else {
        /* Make a nul-terminated version. */
        char buf[128];
        len = std::min(len, (int) sizeof(buf) - 1);
        strncpy(buf, name, len);
        buf[len] = '\0';
        *glyph = FT_Get_Name_Index(face, buf);
    }

    if(*glyph == 0) {
        /* Check whether the given name was actually the name of glyph 0. */
        char buf[128];
        if (!FT_Get_Glyph_Name(face, 0, buf, sizeof(buf)) &&
            len < 0 ? !strcmp(buf, name) : !strncmp(buf, name, len))
            return true;
    }

    return *glyph != 0;
}
