/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <GibbonFreetype.h>
#include <GibbonApplication.h>
#include <GibbonConfiguration.h>
#include <SurfaceDecoder.h>
#include <SurfaceCache.h>
#include <GraphicsEngine.h>
#include <FontManager.h>
#include <RectPacker.h>
#include <Screen.h>
#include <Style.h>
#include <TextEngine.h>
#include <Widget.h>
#include <algorithm>
#include <assert.h>
#include <map>
#include <set>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <nrdbase/Dumper.h>
#include <nrdbase/LRU.h>
#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Time.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/utf8.h>
#include <nrd/AppLog.h>
#include <TextLocale.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_SIZES_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_OUTLINE_H
#include FT_STROKER_H

#include "GibbonHarfbuzz.h"
#include <hb.h>
#include <hb-ft.h>

#if FREETYPE_MAJOR > 2 || (FREETYPE_MAJOR == 2 && FREETYPE_MINOR > 4) || (FREETYPE_MAJOR == 2 && FREETYPE_MINOR == 4 && FREETYPE_PATCH > 5)
#define FREETYPE_REREQUEST_METRICS
#endif

using namespace netflix::gibbon;
using namespace netflix;

// Lovely FreeType

#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s) case e: return s;
#define FT_ERROR_START_LIST
#define FT_ERROR_END_LIST

const char* GibbonFreetype::freetypeErrorString(int code)
{
    switch (code) {
#include FT_ERRORS_H
    }
    return "unknown";
}

NRDP_DEFINE_OBJECTCOUNT(GibbonFreetypeGlyphSpan);
NRDP_DEFINE_OBJECTCOUNT(GibbonFreetypeCacheSize);
NRDP_DEFINE_OBJECTCOUNT(GibbonFreetypeCacheFace);
NRDP_DEFINE_OBJECTCOUNT(GibbonFreetype);

namespace netflix {
namespace gibbon {

class GibbonFreetypeDumper : public Dumper
{
public:
    GibbonFreetypeDumper() { }

protected:
    virtual std::vector<std::string> consoleEntry(const unsigned int idx, const std::string& section, const Variant& value);
    virtual std::vector<std::string> consolePostProcess(const Variant& values);
};

#define FTCEIL(x) ((((x) + 63) & -64) >> 6)
#define FTROUND(x) (static_cast<int>(roundf((x) / 64.)))
#define FTMOD(x) ((x) % 64)
#define FTFIXED(x) static_cast<int>((x) * 64)

template<typename T>
T biggest(T a, T b);

template<>
int biggest<int>(int a, int b)
{
    return (abs(a) > abs(b)) ? a : b;
}

template<>
float biggest<float>(float a, float b)
{
    return (fabsf(a) > fabsf(b)) ? a : b;
}

class GlyphSpan : public Noncopyable
{
public:
    GlyphSpan(FT_Library lib, int width)
        : advance(0), horiAdvance(0), width(0), height(0),
          horiBearingX(0), horiBearingY(0), vertBearingX(0), vertBearingY(0),
          origAdvance(0), outlineWidth(width), library(lib)
    {
        NRDP_OBJECTCOUNT_REF(GibbonFreetypeGlyphSpan, this);
    }

    ~GlyphSpan()
    {
        NRDP_OBJECTCOUNT_DEREF(GibbonFreetypeGlyphSpan, this);
    }

    bool setGlyph(const FT_GlyphSlot slot, int attributes, const FontValues &fontValues);

    int size() const;

    struct Span
    {
        Span(int _x, int _y, int _width, int _coverage)
            : x(_x), y(_y), width(_width), coverage(_coverage)
        {
        }

        int x, y, width, coverage;
    };

    ftcache::FtRect bounds;
    int advance, horiAdvance;
    int width, height;
    int horiBearingX, horiBearingY, vertBearingX, vertBearingY;
    int origAdvance;

    int outlineWidth;

    std::vector<Span> spans;
    std::vector<Span> outline;

private:
    static void graySpans(const int y, const int count, const FT_Span* const spans, void* const user);

    FT_Library library;
};

struct SpanUserData
{
    void reset(std::vector<GlyphSpan::Span>* s) { span = s; }

    std::vector<GlyphSpan::Span>* span;
};

inline void GlyphSpan::graySpans(const int y, const int count, const FT_Span* const spans, void* const user)
{
    SpanUserData* const data = reinterpret_cast<SpanUserData* const>(user);
    for (int i = 0; i < count; ++i) {
        data->span->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
    }
}

static inline void adjustSpans(std::vector<GlyphSpan::Span>& spans, int diff)
{
    std::vector<GlyphSpan::Span>::iterator s = spans.begin();
    const std::vector<GlyphSpan::Span>::const_iterator end = spans.end();
    while (s != end) {
        s->x += diff;
        ++s;
    }
}

inline int GlyphSpan::size() const
{
    return sizeof(GlyphSpan) + (sizeof(Span) * outline.size()) + (sizeof(Span) * spans.size());
}

// The following code was lifted from libass which was released under the following license:

/*
 * Copyright (C) 2006 Evgeniy Stepanov <eugeni.stepanov@gmail.com>
 *
 * This file is part of libass.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \brief Calculate the cbox of a series of points
 */
static void get_contour_cbox(FT_BBox *box, FT_Vector *points, int start, int end)
{
    box->xMin = box->yMin = INT_MAX;
    box->xMax = box->yMax = INT_MIN;
    int i;

    for (i = start; i <= end; i++) {
        box->xMin = (points[i].x < box->xMin) ? points[i].x : box->xMin;
        box->xMax = (points[i].x > box->xMax) ? points[i].x : box->xMax;
        box->yMin = (points[i].y < box->yMin) ? points[i].y : box->yMin;
        box->yMax = (points[i].y > box->yMax) ? points[i].y : box->yMax;
    }
}

/**
 * \brief Determine winding direction of a contour
 * \return direction; 0 = clockwise
 */
static int get_contour_direction(FT_Vector *points, int start, int end)
{
    int i;
    long long sum = 0;
    int x = points[start].x;
    int y = points[start].y;
    for (i = start + 1; i <= end; i++) {
        sum += x * (points[i].y - y) - y * (points[i].x - x);
        x = points[i].x;
        y = points[i].y;
    }
    sum += x * (points[start].y - y) - y * (points[start].x - x);
    return sum > 0;
}

/**
 * \brief Apply fixups to please the FreeType stroker and improve the
 * rendering result, especially in case the outline has some anomalies.
 * At the moment, the following fixes are done:
 *
 * 1. Reverse contours that have "inside" winding direction but are not
 *    contained in any other contours' cbox.
 * 2. Remove "inside" contours depending on border size, so that large
 *    borders do not reverse the winding direction, which leads to "holes"
 *    inside the border. The inside will be filled by the border of the
 *    outside contour anyway in this case.
 *
 * \param outline FreeType outline, modified in-place
 * \param border_x border size, x direction, d6 format
 * \param border_x border size, y direction, d6 format
 */
static void fix_freetype_stroker(FT_Outline *outline, int border_x, int border_y)
{
    int nc = outline->n_contours;
    int begin, stop;
    char modified = 0;
    char *valid_cont = reinterpret_cast<char*>(malloc(nc));
    int start = 0;
    int end = -1;
    FT_BBox *boxes = reinterpret_cast<FT_BBox*>(malloc(nc * sizeof(FT_BBox)));
    int i, j;
    int inside_direction;

    inside_direction = FT_Outline_Get_Orientation(outline) ==
        FT_ORIENTATION_TRUETYPE;

    // create a list of cboxes of the contours
    for (i = 0; i < nc; i++) {
        start = end + 1;
        end = outline->contours[i];
        get_contour_cbox(&boxes[i], outline->points, start, end);
    }

    // for each contour, check direction and whether it's "outside"
    // or contained in another contour
    end = -1;
    for (i = 0; i < nc; i++) {
        start = end + 1;
        end = outline->contours[i];
        int dir = get_contour_direction(outline->points, start, end);
        valid_cont[i] = 1;
        if (dir == inside_direction) {
            for (j = 0; j < nc; j++) {
                if (i == j)
                    continue;
                if (boxes[i].xMin >= boxes[j].xMin &&
                    boxes[i].xMax <= boxes[j].xMax &&
                    boxes[i].yMin >= boxes[j].yMin &&
                    boxes[i].yMax <= boxes[j].yMax)
                    goto check_inside;
            }
            /* "inside" contour but we can't find anything it could be
             * inside of - assume the font is buggy and it should be
             * an "outside" contour, and reverse it */
            for (j = 0; j < (end - start) / 2; j++) {
                FT_Vector temp = outline->points[start + 1 + j];
                char temp2 = outline->tags[start + 1 + j];
                outline->points[start + 1 + j] = outline->points[end - j];
                outline->points[end - j] = temp;
                outline->tags[start + 1 + j] = outline->tags[end - j];
                outline->tags[end - j] = temp2;
            }
            dir ^= 1;
        }
  check_inside:
        if (dir == inside_direction) {
            FT_BBox box;
            get_contour_cbox(&box, outline->points, start, end);
            int width = box.xMax - box.xMin;
            int height = box.yMax - box.yMin;
            if (width < border_x * 2 || height < border_y * 2) {
                valid_cont[i] = 0;
                modified = 1;
            }
        }
    }

    // if we need to modify the outline, rewrite it and skip
    // the contours that we determined should be removed.
    if (modified) {
        int p = 0, c = 0;
        for (i = 0; i < nc; i++) {
            if (!valid_cont[i])
                continue;
            begin = (i == 0) ? 0 : outline->contours[i - 1] + 1;
            stop = outline->contours[i];
            for (j = begin; j <= stop; j++) {
                outline->points[p].x = outline->points[j].x;
                outline->points[p].y = outline->points[j].y;
                outline->tags[p] = outline->tags[j];
                p++;
            }
            outline->contours[c] = p - 1;
            c++;
        }
        outline->n_points = p;
        outline->n_contours = c;
    }

    free(boxes);
    free(valid_cont);
}

bool GlyphSpan::setGlyph(const FT_GlyphSlot slot, int attributes, const FontValues &fontValues)
{
    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
        return false;

    spans.clear();
    outline.clear();
    bounds.clear();

    FT_Error err;

    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));

    SpanUserData user = { &spans };

    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = GlyphSpan::graySpans;
    params.user = &user;

    if (attributes & FontManager::Slant) {
        FT_Matrix transform;
        transform.xx = 0x10000L;
        transform.yx = 0x00000L;

        transform.xy = static_cast<FT_Fixed>(fontValues.slantFactor * 65536); // convert to 16.16 format
        transform.yy = 0x10000L;

        FT_Outline_Transform(&slot->outline, &transform);
    }
    if (attributes & FontManager::Embolden) {
        const FT_Pos str = FT_MulFix(static_cast<FT_Long>(slot->face->units_per_EM * fontValues.emboldenFactor),
                                     slot->face->size->metrics.y_scale) / 24;
        FT_Outline_Embolden(&slot->outline, str);
        if (slot->advance.x)
            slot->advance.x += str;
        slot->metrics.width += str;
        slot->metrics.horiAdvance += str;
    }

    err = FT_Outline_Render(library, &slot->outline, &params);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to render outline: %s", GibbonFreetype::freetypeErrorString(err));
        return false;
    }

    if (outlineWidth > 0) {
        FT_Stroker stroker;
        err = FT_Stroker_New(library, &stroker);
        if (err) {
            Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to create new stroker: %s", GibbonFreetype::freetypeErrorString(err));
            return false;
        }
        FT_Stroker_Set(stroker, outlineWidth * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

        FT_Glyph glyph;
        err = FT_Get_Glyph(slot, &glyph);
        if (err) {
            FT_Stroker_Done(stroker);
            Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to get glyph for slot: %s", GibbonFreetype::freetypeErrorString(err));
            return false;
        }
        err = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
        if (err) {
            FT_Stroker_Done(stroker);
            FT_Done_Glyph(glyph);
            Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to stroke border: %s", GibbonFreetype::freetypeErrorString(err));
            return false;
        }
        if (glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
            user.reset(&outline);

            FT_Outline* ol = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
            fix_freetype_stroker(ol, outlineWidth * 64, outlineWidth * 64);

            FT_Outline_Render(library, ol, &params);
            if (err) {
                FT_Stroker_Done(stroker);
                FT_Done_Glyph(glyph);
                Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to render outline for border: %s", GibbonFreetype::freetypeErrorString(err));
                return false;
            }

            adjustSpans(spans, outlineWidth);
            adjustSpans(outline, outlineWidth);
        }
        FT_Stroker_Done(stroker);
        FT_Done_Glyph(glyph);
    }

    horiBearingX = slot->metrics.horiBearingX;
    horiBearingY = slot->metrics.horiBearingY;
    vertBearingX = slot->metrics.vertBearingX;
    vertBearingY = slot->metrics.vertBearingY;
    width = slot->metrics.width;
    height = slot->metrics.height;
    horiAdvance = FTCEIL(slot->metrics.horiAdvance) + (outlineWidth * 2);
    advance = FTCEIL(slot->advance.x) + (outlineWidth * 2);
    origAdvance = slot->advance.x;

    if (!spans.empty()) {
        Span& front = spans.front();
        ftcache::FtRect rect(front.x, front.y, front.x, front.y);
        std::vector<Span>::const_iterator s = spans.begin();
        std::vector<Span>::const_iterator end = spans.end();
        while (s != end) {
            rect.include(s->x, s->y);
            rect.include(s->x + s->width - 1, s->y);
            ++s;
        }
        s = outline.begin();
        end = outline.end();
        while (s != end) {
            rect.include(s->x, s->y);
            rect.include(s->x + s->width - 1, s->y);
            ++s;
        }
        bounds = rect;
    }
    if (attributes & FontManager::Slant) {
        horiBearingX = FTFIXED(bounds.xmin);
        width = FTFIXED(bounds.width());
    }

    return true;
}

namespace ftcache {

class CacheFace;
class CacheEntry;

class CacheSize : public Noncopyable
{
public:
    CacheSize(FT_Size s, shared_ptr<CacheFace> f, int p) : size(s), face(f), maxWidth(-1), maxAdvance(-1), point(p)
    {
        NRDP_OBJECTCOUNT_REF(GibbonFreetypeCacheSize, this);
    }
    ~CacheSize()
    {
        NRDP_OBJECTCOUNT_DEREF(GibbonFreetypeCacheSize, this);
        if (face.use_count() == 1) {
            face.reset();
            GibbonFreetype::verifyCache();
        }
    }

    FT_Size size;
    shared_ptr<CacheFace> face;
    int maxWidth, maxAdvance;
    int point;
};

class CacheFace : public Noncopyable
{
public:
    CacheFace(const std::string& id, FT_Face f, const shared_ptr<CacheEntry> e)
        : ident(id), face(f), touched(Time::monoMS()), attributes(0), entry(e)
    {
        ++num;
        NRDP_OBJECTCOUNT_REF(GibbonFreetypeCacheFace, this);
    }
    ~CacheFace();

    void touch() { touched = Time::monoMS(); }

    std::string ident;
    FT_Face face;
    ullong touched;
    int attributes;
    FontValues fontValues;
    shared_ptr<CacheEntry> entry;

    static int num;
};

int CacheFace::num = 0;

class CacheEntry : public enable_shared_from_this<CacheEntry>, public Noncopyable
{
public:
    enum Type { Unknown, Url, Buffer, Mmap };

    CacheEntry() : mType(Unknown), mAttributes(FontManager::None), mCMap(-1)
    {
        memset(&mBbox, '\0', sizeof(FT_BBox));
    }

    CacheEntry(const std::string& ident, const std::string& url, int attributes, const FontValues &fontValues, const Rect& bbox, int cmap = -1)
        : mIdent(ident), mType(Url), mAttributes(attributes), mCMap(cmap), mFontValues(fontValues)
    {
        mBbox.xMin = static_cast<FT_Pos>(bbox.x);
        mBbox.xMax = static_cast<FT_Pos>(bbox.width);
        mBbox.yMin = static_cast<FT_Pos>(bbox.y);
        mBbox.yMax = static_cast<FT_Pos>(bbox.height);
        adjustBbox();
        mData.url = new std::string(url);
    }
    CacheEntry(const std::string& ident, const DataBuffer& buffer, int attributes, const FontValues &fontValues, const Rect& bbox, int cmap = -1)
        : mIdent(ident), mType(Buffer), mAttributes(attributes), mCMap(cmap), mFontValues(fontValues)
    {
        mBbox.xMin = static_cast<FT_Pos>(bbox.x);
        mBbox.xMax = static_cast<FT_Pos>(bbox.width);
        mBbox.yMin = static_cast<FT_Pos>(bbox.y);
        mBbox.yMax = static_cast<FT_Pos>(bbox.height);
        adjustBbox();
        mData.buffer = new DataBuffer(buffer);
    }
    ~CacheEntry() { clear(); }

    void clear()
    {
        switch (mType) {
        case Unknown:
            break;
        case Url:
            delete mData.url;
            break;
        case Buffer:
                delete mData.buffer;
            break;
        case Mmap:
            if (!--mData.data->ref) {
                if (mData.data->data)
                    munmap(mData.data->data, mData.data->size);
                if (mData.data->fd != -1)
                    ::close(mData.data->fd);
                delete mData.data;
            }
            mData.data = 0;
        }
    }
    const std::string& url() const { assert(mType == Url); return *mData.url; }
    const DataBuffer& buffer() const { assert(mType == Buffer); return *mData.buffer; }
    unsigned int size() const
    {
        switch (mType) {
        case Unknown:
            break;
        case Url:
            return mData.url->size();
        case Buffer:
            return mData.buffer->size();
        case Mmap:
            return mData.data->size;
        }
        return 0;
    }
    int attributes() const { return mAttributes; }
    int cmap() const { return mCMap; }
    FontValues fontValues() const { return mFontValues; }
    Type type() const { return mType; }
    FT_BBox bbox() const { return mBbox; }

    shared_ptr<CacheFace> face();
    bool cached() const { return static_cast<bool>(mFace.lock()); }

    bool valid() const
    {
        return (mFace.lock().get());
    }

private:
    void adjustBbox()
    {
        if (mAttributes & FontManager::Slant) {
            // given the height and the angle, what's the additional width?
            // const FT_Pos height = mBbox.yMax - mBbox.yMin;
            const FT_Pos additional = static_cast<FT_Pos>(ceil(mBbox.yMax * mFontValues.slantFactor));
            mBbox.xMax += additional;
        }
    }

private:
    std::string mIdent;
    Type mType;
    struct MmapData {
        int fd;
        void* data;
        unsigned int size;
        int ref;
    };
    union {
        std::string* url;
        DataBuffer* buffer;
        MmapData* data;
    } mData;
    int mAttributes, mCMap;
    FontValues mFontValues;
    FT_BBox mBbox;
    weak_ptr<CacheFace> mFace;
};

struct SizeEntry
{
    std::map<int, shared_ptr<CacheSize> > sizes;

    ullong touched() const
    {
        if (sizes.empty())
            return 0;
        const shared_ptr<CacheFace> face = sizes.begin()->second->face;
        return face->touched;
    }

    Variant toVariant() const;
};

Variant SizeEntry::toVariant() const
{
    Variant data;
    std::map<int, shared_ptr<CacheSize> >::const_iterator it = sizes.begin();
    const std::map<int, shared_ptr<CacheSize> >::const_iterator end = sizes.end();
    while (it != end) {
        data.push_back(it->first);
        ++it;
    }
    return data;
}

Variant Glyph::toVariant() const
{
    Variant data;
    if (!glyphNode)
        return Variant();
    const rectpacker::Rect& r = glyphNode->rect;
    Variant rect;
    rect["x"] = r.x;
    rect["y"] = r.y;
    rect["width"] = r.width();
    rect["height"] = r.height();
    data["rect"] = rect;
    return data;
}

class SpanCacheKey
{
public:
    SpanCacheKey(shared_ptr<CacheSize> s, FT_UInt g, int a, int o)
        : ident(s->face->ident), size(s->point), glyph(g), attributes(a), outline(o)
    {
    }

    bool operator<(const SpanCacheKey& other) const
    {
        const int cmp = strcmp(ident.c_str(), other.ident.c_str());
        if (cmp < 0)
            return true;
        else if (cmp > 0)
            return false;
        if (size < other.size)
            return true;
        else if (size > other.size)
            return false;
        if (glyph < other.glyph)
            return true;
        else if (glyph > other.glyph)
            return false;
        if (attributes < other.attributes)
            return true;
        else if (attributes > other.attributes)
            return false;
        if (outline < other.outline)
            return true;
        return false;
    }

private:
    std::string ident;
    int size;
    FT_UInt glyph;
    int attributes;
    int outline;

    friend std::ostream& operator<<(std::ostream &out, const SpanCacheKey &key);
};

class GlyphSpanCost
{
public:
    int operator()(const shared_ptr<GlyphSpan>& span)
    {
        return span->size();
    }
};

struct FreetypeMemory : public FT_MemoryRec_
{
    long total;
    std::map<void*, long> allocated;
    netflix::Mutex mutex;

    void init();
    FreetypeMemory() : total(0), mutex(netflix::ZERO_MUTEX, "FreeTypeMemoryMutex")
    {}
};

static void* FreetypeMemoryAlloc(FT_Memory memory, long size)
{
    void* ret = malloc(size);
    FreetypeMemory* mem = static_cast<FreetypeMemory*>(memory);
    ScopedMutex(mem->mutex);
    mem->allocated[ret] = size;
    mem->total += size;
    return ret;
}

static void* FreetypeMemoryRealloc(FT_Memory memory, long cur, long size, void* block)
{
    void* ret = realloc(block, size);
    FreetypeMemory* mem = static_cast<FreetypeMemory*>(memory);
    ScopedMutex(mem->mutex);
    if (ret != block) {
        std::map<void*, long>::iterator it = mem->allocated.find(block);
        if (it != mem->allocated.end())
            mem->allocated.erase(it);
        else
            Log::error(TRACE_GIBBON_TEXT, "FreetypeMemoryRealloc: block not found: %p", block);
    }
    mem->allocated[ret] = size;
    mem->total -= cur;
    mem->total += size;
    return ret;
}

static void FreetypeMemoryFree(FT_Memory memory, void* block)
{
    free(block);
    FreetypeMemory* mem = static_cast<FreetypeMemory*>(memory);
    ScopedMutex(mem->mutex);
    std::map<void*, long>::iterator it = mem->allocated.find(block);
    if (it == mem->allocated.end()) {
        // bad!
        Log::error(TRACE_GIBBON_TEXT, "FreetypeMemoryFree: block not found: %p", block);
        return;
    }
    mem->total -= it->second;
    mem->allocated.erase(it);
}

void FreetypeMemory::init()
{
    total = 0;
    user = 0;
    alloc = FreetypeMemoryAlloc;
    free = FreetypeMemoryFree;
    realloc = FreetypeMemoryRealloc;
}

class LocaleCacheKey
{
public:
    LocaleCacheKey(const std::string& l, Font::Weight w, Font::Style s, int sz)
        : locale(l), weight(w), style(s), size(sz)
    {
    }

    bool operator<(const LocaleCacheKey& other) const
    {
        if (locale < other.locale)
            return true;
        if (locale > other.locale)
            return false;
        if (weight < other.weight)
            return true;
        if (weight > other.weight)
            return false;
        if (style < other.style)
            return true;
        if (style > other.style)
            return false;
        if (size < other.size)
            return true;
        return false;
    }

    std::string locale;
    Font::Weight weight;
    Font::Style style;
    int size;
};

struct LocaleData
{
    unsigned long startCp, endCp;
    weak_ptr<CacheSize> size;
};

template<typename Glyph>
struct GlyphDtor
{
public:
    void operator()(Glyph& glyph)
    {
        if (glyph->glyphNode)
            glyph->glyphNode->leaf = false;
    }
};

template<typename Glyph>
struct GlyphPurgeAllow
{
public:
    bool operator()(const Glyph& glyph)
    {
        return glyph.use_count() == 1;
    }
};

std::ostream& operator<<(std::ostream &out, const SpanCacheKey &key)
{
    out << "id:" << key.ident
        << ",sz:" << key.size
        << ",gl:" << key.glyph
        << ",at:" << key.attributes
        << ",ol:" << key.outline;
    return out;
}

class LRUDumper
{
public:
    template<typename T>
    static Variant dump(const T& lru);
    template<typename T>
    static Variant dumpPtr(const T& lru);
};

template<typename T>
inline Variant LRUDumper::dump(const T& lru)
{
    Variant data;

    typename T::const_iterator it = lru.begin();
    const typename T::const_iterator end = lru.end();
    while (it != end) {
        std::ostringstream strm;
        strm << it.key();
        data[strm.str()] = it->toVariant();
        ++it;
    }

    return data;
}

template<typename T>
inline Variant LRUDumper::dumpPtr(const T& lru)
{
    Variant data;

    typename T::const_iterator it = lru.begin();
    const typename T::const_iterator end = lru.end();
    while (it != end) {
        std::ostringstream strm;
        strm << it.key();
        data[strm.str()] = (*it)->toVariant();
        ++it;
    }

    return data;
}

class Cache
{
public:
    Cache();
    ~Cache();

    typedef LRU<SpanCacheKey, Glyph::SharedPtr, LRUOne<Glyph::SharedPtr>, GlyphDtor<Glyph::SharedPtr>, GlyphPurgeAllow<Glyph::SharedPtr> > GlyphCache;
    typedef LRU<std::string, SizeEntry, LRUOne<SizeEntry> > SizeCache;

    typedef std::map<LocaleCacheKey, std::map<uint32_t, LocaleData> > LocaleCache;

    void clear();

    Variant lruToVariant();

    static Cache* instance() { return inst; }
    static FT_Library freetype() { return ft; }
    static void cleanup();
    static void setCacheSizes(int sizeCacheSize, int glyphCacheWidth, int glyphCacheHeight);

    shared_ptr<CacheSize> entry(bool persist, const std::string& ident, int size);

    void addFace(const std::string& ident, const std::string& url, const int attributes, const FontValues &fontValues, const Rect& bbox, int cmap = -1);
    void addFace(const std::string& ident, const DataBuffer& data, const int attributes, const FontValues &fontValues, const Rect& bbox, int cmap = -1);
    void removeFace(const std::string& ident);

    void dumpCacheInformation() const;
    Variant cacheInformation() const;

    shared_ptr<GibbonFreetypeDumper> dumper(const DumpInfo& dump) const;

    Glyph::SharedPtr requestGlyph(const shared_ptr<CacheSize>& size, FT_UInt glyph, int outline = 0,
                                  GibbonFreetype::RequestMode mode = GibbonFreetype::RequestRender);
    bool renderGlyph(Glyph* ptr, const shared_ptr<CacheSize>& size, FT_UInt glyph, int outline = 0);
    bool renderGlyph(Glyph* glyph, const GlyphSpan& span, bool outline);

    Surface::SharedPtr surface() const
    {
        if (!cacheSurface)
            const_cast<Cache*>(this)->recreateGlyphCache();
        return cacheSurface;
    }

    bool purgeCache();
    void clearCache();
    void dropGlyphCache();
    void recreateGlyphCache();

    void verify();
    void persist(const shared_ptr<CacheSize>& size);

    static rectpacker::Rect adjustRect(const rectpacker::Rect& r)
    {
        return rectpacker::Rect(r.x, r.y, r.right - glyphPadding, r.bottom - glyphPadding);
    }

    static Mutex& mutex() { return ftMutex; }

private:
    void drawSpans(Surface::Data& data, const std::vector<GlyphSpan::Span>& spans,
                   const FtRect& bounds, int width, int height);

private:
    FreetypeMemory memory;

    std::map<std::string, shared_ptr<CacheEntry> > entries;
    SizeCache sizeCache;
    GlyphCache glyphCache;
    LocaleCache localeCache;

    Surface::SharedPtr cacheSurface;
    rectpacker::RectPacker cachePacker;

    int glyphCacheWidth, glyphCacheHeight;
    int glyphCacheUsed;

    int blitDataSize;
    int purgeCount;
    int clearCount;
    unsigned int renderHit, renderMiss, geomHit, geomMiss;
    unsigned char* blitData;

    static int defaultSize;
    static int defaultGlyphCacheWidth;
    static int defaultGlyphCacheHeight;

    static Mutex ftMutex;
    static FT_Library ft;
    static Cache* inst;

    static int glyphPadding;

    friend class CacheEntry;
};

int Cache::defaultSize = 0;
int Cache::defaultGlyphCacheWidth = 1024;
int Cache::defaultGlyphCacheHeight = 1024;
Cache* Cache::inst = 0;
FT_Library Cache::ft = 0;
Mutex Cache::ftMutex(GIBBON_FREETYPE_MUTEX, "GibbonFreetype mutex");
int Cache::glyphPadding = 0;

shared_ptr<CacheFace> CacheEntry::face()
{
    shared_ptr<CacheFace> f = mFace.lock();
    if (f) {
        f->touch();
        return f;
    }

    FT_Face face = 0;
    if (mType == Url || mType == Mmap) {
        if (mType == Url) {
            const std::string url = *mData.url;
            mType = Mmap;
            delete mData.url;
            mData.data = new MmapData;
            mData.data->ref = 1;
            mData.data->data = 0;
            mData.data->fd = ::open(url.c_str(), O_RDONLY);
            if (mData.data->fd == -1) {
                Log::error(TRACE_GIBBON_TEXT, "Freetype: Unable to open file %s", url.c_str());
                return f;
            }
            struct stat s;
            if (::fstat(mData.data->fd, &s) == -1) {
                ::close(mData.data->fd);
                mData.data->fd = -1;
                Log::error(TRACE_GIBBON_TEXT, "Freetype: Unable to stat file %s", url.c_str());
                return f;
            }
            mData.data->size = s.st_size;
            mData.data->data = ::mmap(0, s.st_size, PROT_READ, MAP_SHARED, mData.data->fd, 0);
            if (mData.data->data == MAP_FAILED || !mData.data->data) {
                ::close(mData.data->fd);
                mData.data->fd = -1;
                mData.data->data = 0;
                Log::error(TRACE_GIBBON_TEXT, "Freetype: Unable to mmap file %s", url.c_str());
                return f;
            }
        }
        if (!mData.data->data)
            return f;
        FT_Error err = FT_New_Memory_Face(Cache::ft, reinterpret_cast<unsigned char*>(mData.data->data), mData.data->size, 0, &face);
        if (err || !face) {
            if (mType == Url)
                Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to initialize font from mmap (%s): %s", mData.url->c_str(), GibbonFreetype::freetypeErrorString(err));
            else
                Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to initialize font from mmap (size: %u): %s", mData.data->size, GibbonFreetype::freetypeErrorString(err));
            return f;
        }
    } else { // Buffer
        assert(mType == Buffer);
        FT_Error err = FT_New_Memory_Face(Cache::ft, mData.buffer->constData(), mData.buffer->size(), 0, &face);
        if (err || !face) {
            Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to initialize font from data (%d bytes): %s", mData.buffer->size(), GibbonFreetype::freetypeErrorString(err));
            return f;
        }
    }

    assert(face);

    // select cmap
    if (mCMap != -1) {
        if (mCMap >= 0 && mCMap < face->num_charmaps) {
            FT_Set_Charmap(face, face->charmaps[mCMap]);
        } else {
            Log::error(TRACE_GIBBON_TEXT, "Invalid charmap %d for '%s', max %d", mCMap, mIdent.c_str(), face->num_charmaps);
        }
    }

    f.reset(new CacheFace(mIdent, face, shared_from_this()));
    mFace = f;
    return f;
}

Cache::Cache()
    : sizeCache(defaultSize), glyphCacheWidth(defaultGlyphCacheWidth), glyphCacheHeight(defaultGlyphCacheHeight),
      glyphCacheUsed(0), blitDataSize(0), purgeCount(0), clearCount(0), renderHit(0), renderMiss(0),
      geomHit(0), geomMiss(0), blitData(0)
{
    assert(!inst);
    assert(!ft);

    memory.init();

    glyphPadding = GibbonApplication::instance()->getScreen()->glyphPadding();

    inst = this;
    FT_Error err = FT_New_Library(&memory, &ft);
    if (err || !ft) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to initialize freetype: %s", GibbonFreetype::freetypeErrorString(err));
        return;
    }
    FT_Add_Default_Modules(ft);
}

Cache::~Cache()
{
    assert(inst);
    clear();
    inst = 0;
}

Variant Cache::lruToVariant()
{
    Variant ret;
    ret["glyphs"] = LRUDumper::dumpPtr(glyphCache);
    ret["sizes"] = LRUDumper::dump(sizeCache);
    return ret;
}

void Cache::clear()
{
    if (blitData) {
        free(blitData);
        blitData = 0;
    }

    glyphCacheUsed = 0;
    blitDataSize = 0;
    purgeCount = 0;
    clearCount = 0;
    renderHit = renderMiss = 0;
    geomHit = geomMiss = 0;

    sizeCache.clear();
    entries.clear();

    localeCache.clear();
    glyphCache.clear();

    cacheSurface.reset();
    cachePacker.destroy();

    if (!CacheFace::num) {
        cleanup();
    }
}

void Cache::verify()
{
    std::map<std::string, shared_ptr<CacheEntry> >::iterator it = entries.begin();
    while (it != entries.end()) {
        if (!it->second->valid()) {
            entries.erase(it++);
        } else {
            ++it;
        }
    }
}

void Cache::persist(const shared_ptr<CacheSize>& size)
{
    const std::string& ident = size->face->ident;
    const int point = size->point;

    SizeCache::iterator sc = sizeCache.find(ident);
    if (sc == sizeCache.end()) {
        sc = sizeCache.insert(ident, SizeEntry()).first;
    }

    std::map<int, shared_ptr<CacheSize> >::const_iterator s = sc->sizes.find(point);
    if (s != sc->sizes.end())
        return;

    sc->sizes[point] = size;
}

inline void Cache::drawSpans(Surface::Data& data, const std::vector<GlyphSpan::Span>& spans,
                             const FtRect& bounds, int width, int height)
{
    //const int maxY = ascender + descender + 1;
    const int xmin = bounds.xmin;
    const int ymin = bounds.ymin;

    Color pixel(255, 255, 255, 0);

    int line;
    std::vector<GlyphSpan::Span>::const_iterator it = spans.begin();
    std::vector<GlyphSpan::Span>::const_iterator end = spans.end();
    while (it != end) {
        line = height - (it->y - ymin) - 1;
        assert(line < height && line >= 0);

        //const Color pixel(it->coverage, it->coverage, it->coverage, it->coverage);
        pixel.a = it->coverage;
        for (int i = it->x; i < it->x + it->width; ++i) {
            assert((i - xmin) >= 0 && (i - xmin) < width);
            NRDP_UNUSED(width);
            data.setPixel(i - xmin, line, pixel);
        }
        ++it;
    }
}

bool Cache::purgeCache()
{
    NTRACE(TRACE_GIBBON_TEXT, "Purging the glyph cache down to %d", (int)((float)glyphCache.currentSize() * 0.7));
    if (!glyphCache.purge((int)((float)glyphCache.currentSize() * 0.7))) {
        Log::warn(TRACE_GIBBON_TEXT, "Unable to purge from the glyph cache");
        return false;
    }

    glyphCacheUsed = cachePacker.used();
    return true;
}

void Cache::recreateGlyphCache()
{
    if (cacheSurface) {
        cacheSurface.reset();
        glyphCache.clear();
    }
    glyphCacheUsed = 0;
    assert(MutexStack::locked(ftcache::Cache::mutex().name()));
    ftcache::Cache::mutex().unlock();
    Surface::SharedPtr surface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(glyphCacheWidth,
                                                                                    glyphCacheHeight,
                                                                                    Surface::Flags_Alpha
                                                                                    | Surface::Flags_Text
                                                                                    | Surface::Flags_RenderTarget,
                                                                                    true);
#if defined(GIBBON_GRAPHICSENGINE_GLES2) && defined(BUILD_DEBUG)
    // clear the glyph cache
    if (surface) {
        const unsigned int sz = surface->calculateSize();
        unsigned char* buf = new unsigned char[sz];
        memset(buf, '\0', sz);
        surface->upload(0, 0, surface->getWidth(), surface->getHeight(), buf, 0);
        delete[] buf;
    }
#endif
    ftcache::Cache::mutex().lock();

    if (!surface || !surface->isValid()) {
        cacheSurface.reset();
    } else {
        cacheSurface = surface;
        cacheSurface->setDescription("GibbonFreeType::Cache");
        cachePacker.init(glyphCacheWidth, glyphCacheHeight);
    }
}

void Cache::dropGlyphCache()
{
    if (cacheSurface) {
        cacheSurface.reset();
        glyphCache.clear();
        glyphCacheUsed = 0;
        cachePacker.destroy();
    }
}

void Cache::clearCache()
{
    ++clearCount;
    recreateGlyphCache();
}

bool Cache::renderGlyph(Glyph* glyph, const GlyphSpan& span, bool outline)
{
    assert(GibbonApplication::isRenderThread());

    const int width = span.bounds.width();
    const int height = span.bounds.height();

    enum { MaxPurgeBeforeClear = 5 };

    glyph->glyphNode = cachePacker.insert(width + glyphPadding, height + glyphPadding);
    if (!glyph->glyphNode) {
        // purge
        if (!purgeCache() || ++purgeCount >= MaxPurgeBeforeClear) {
            // unable to purge, throw everything out
            NTRACE(TRACE_GIBBON_TEXT, "Unable to purge 1st, clearing");
            clearCache();
            purgeCount = 0;
        }
        glyph->glyphNode = cachePacker.insert(width + glyphPadding, height + glyphPadding);
        if (!glyph->glyphNode) {
            if (!purgeCount) {
                Log::error(TRACE_GIBBON_TEXT, "Unable to purge/clear glyph cache");
                return false;
            }
            // we didn't completely clear the cache earlier, try again
            NTRACE(TRACE_GIBBON_TEXT, "Unable to purge 2nd, clearing");
            clearCache();
            purgeCount = 0;
            glyph->glyphNode = cachePacker.insert(width + glyphPadding, height + glyphPadding);
            if (!glyph->glyphNode) {
                Log::error(TRACE_GIBBON_TEXT, "Unable to purge/clear glyph cache on second attempt");
                return false;
            }
        }
    }

    assert(glyph->glyphNode);

    const rectpacker::Rect& r = glyph->glyphNode->rect;
    glyphCacheUsed += (r.width() * r.height());

    const int textBpp = Surface::bytesPerPixel(GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Text));
    if (blitDataSize < ((width + glyphPadding) * (height + glyphPadding) * textBpp)) {
        const int size = (width + glyphPadding) * (height + glyphPadding) * textBpp;
        if(blitData)
            free(blitData);
#ifdef GIBBON_FREETYPE_USE_MALLOC
        blitDataSize = size;
        blitData = static_cast<unsigned char*>(malloc(blitDataSize));
#else
        const int align = sysconf(_SC_PAGESIZE);
        blitDataSize = size + (size % align);
        if(posix_memalign((void**)&blitData, align, blitDataSize)) {
            if(blitData)
                free(blitData);
            blitData = 0;
        }
#endif
        assert(blitData);
    }

    {
        Surface::Data surfaceData(blitData, width + glyphPadding, height + glyphPadding,
                                  GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Text));
        memset(surfaceData.data(), 0, (height + glyphPadding) * surfaceData.bytesPerLine());
        drawSpans(surfaceData, (!outline ? span.spans : span.outline), span.bounds, width, height);
    }
    cacheSurface->upload(r.x, r.y, width + glyphPadding, height + glyphPadding, blitData);
    glyph->cacheSerial = cachePacker.serial();

    return true;
}

bool Cache::renderGlyph(Glyph* ptr, const shared_ptr<CacheSize>& size, FT_UInt glyph, int outline)
{
    assert(GibbonApplication::isRenderThread());

    if (!cacheSurface) {
        recreateGlyphCache();
        if (!cacheSurface)
            return false;
    }

    if (ptr->glyphNode && ptr->cacheSerial == cachePacker.serial()) {
        ++renderHit;
        return true;
    }
    ++renderMiss;

    const int attributes = (size->face->attributes & ~FontManager::Monospace);
    const FontValues& fontValues = size->face->fontValues;
    const FT_Int32 flags = GibbonFreetype::loadFlags(attributes);

    const FT_Face face = size->face->face;
    const FT_Error err = FT_Load_Glyph(face, glyph, flags);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to load glyph (render): %u %s from %s",
                   glyph, GibbonFreetype::freetypeErrorString(err), size->face->ident.c_str());
        return false;
    }

    GlyphSpan span(ft, outline);
    span.setGlyph(face->glyph, attributes, fontValues);

    return renderGlyph(ptr, span, (outline != 0));
}


Glyph::SharedPtr Cache::requestGlyph(const shared_ptr<CacheSize>& size, FT_UInt glyph, int outline,
                                     GibbonFreetype::RequestMode mode)
{
    const int attributes = (size->face->attributes & ~FontManager::Monospace);
    const FontValues& fontValues = size->face->fontValues;

    Glyph::SharedPtr glyphptr;
    const SpanCacheKey key(size, glyph, attributes, outline);
    GlyphCache::iterator cacheEntry = glyphCache.find(key);
    if (cacheEntry != glyphCache.end()) {
        glyphptr = cacheEntry.constValue();
        if (mode == GibbonFreetype::RequestGeometry) {
            ++geomHit;
            return glyphptr;
        } else if (glyphptr->glyphNode && glyphptr->cacheSerial == cachePacker.serial()) {
            ++renderHit;
            return glyphptr;
        }
    }
    if (mode == GibbonFreetype::RequestGeometry)
        ++geomMiss;
    else
        ++renderMiss;

    const FT_Int32 flags = GibbonFreetype::loadFlags(attributes);

    const FT_Face face = size->face->face;
    const FT_Error err = FT_Load_Glyph(face, glyph, flags);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to load glyph (request): %u %s from %s",
                   glyph, GibbonFreetype::freetypeErrorString(err), size->face->ident.c_str());
        return Glyph::SharedPtr();
    }

    GlyphSpan span(ft, outline);
    span.setGlyph(face->glyph, attributes, fontValues);

    const bool insert = !glyphptr;
    if (insert)
        glyphptr.reset(new Glyph);
    glyphptr->bounds = span.bounds;
    glyphptr->origAdvance = span.origAdvance;
    glyphptr->advance = span.advance;
    glyphptr->horiBearingX = span.horiBearingX;
    glyphptr->horiBearingY = span.horiBearingY;
    glyphptr->vertBearingX = span.vertBearingX;
    glyphptr->vertBearingY = span.vertBearingY;
    glyphptr->width = span.width;
    glyphptr->height = span.height;
    glyphptr->horiAdvance = span.horiAdvance;
    glyphptr->outlineWidth = span.outlineWidth;

    if (mode == GibbonFreetype::RequestGeometry) {
        glyphptr->glyphNode = 0;
        glyphptr->cacheSerial = 0;

        return (!insert || glyphCache.insert(key, glyphptr).second) ? glyphptr : Glyph::SharedPtr();
    }

    if (!cacheSurface) {
        recreateGlyphCache();
        if (!cacheSurface)
            return Glyph::SharedPtr();
    }

    if (!renderGlyph(glyphptr.get(), span, (outline != 0))) {
        return Glyph::SharedPtr();
    }

    return (!insert || glyphCache.insert(key, glyphptr).second) ? glyphptr : Glyph::SharedPtr();
}

void Cache::cleanup()
{
    if (ft) {
        FT_Done_Library(ft);
        ft = 0;
    }
}

void Cache::setCacheSizes(int sizeCacheSize, int glyphCacheWidth, int glyphCacheHeight)
{
    if (GibbonConfiguration::fontVerifyGlyphCacheCapacity()) {
        glyphCacheWidth = std::max(std::min(glyphCacheWidth, 2048), 1024);
        glyphCacheHeight = std::max(std::min(glyphCacheHeight, 2048), 1024);
    }
    if (sizeCacheSize != -1)
        defaultSize = sizeCacheSize;
    if (glyphCacheWidth != -1)
        defaultGlyphCacheWidth = glyphCacheWidth;
    if (glyphCacheHeight != -1)
        defaultGlyphCacheHeight = glyphCacheHeight;
    if (inst) {
        if (sizeCacheSize != -1)
            inst->sizeCache.setMaxSize(sizeCacheSize);
        if (glyphCacheWidth != -1)
            inst->glyphCacheWidth = glyphCacheWidth;
        if (glyphCacheHeight != -1)
            inst->glyphCacheHeight = glyphCacheHeight;
        inst->recreateGlyphCache();
    }
}

#ifdef FREETYPE_REREQUEST_METRICS
extern "C" void FT_Request_Metrics(FT_Face         face,
                                   FT_Size_Request req);
#endif

shared_ptr<CacheSize> Cache::entry(bool persist, const std::string& ident, int size)
{
    std::map<std::string, shared_ptr<CacheEntry> >::iterator e = entries.find(ident);
    if (e == entries.end())
        return shared_ptr<CacheSize>();

    shared_ptr<CacheFace> face = e->second->face();
    if (!face)
        return shared_ptr<CacheSize>();
    face->attributes = e->second->attributes();
    face->fontValues = e->second->fontValues();

    bool created = false;
    SizeCache::iterator sc = sizeCache.find(ident);
    if (sc == sizeCache.end()) {
        sc = sizeCache.insert(ident, SizeEntry()).first;
        created = true;
    }

    std::map<int, shared_ptr<CacheSize> >::const_iterator s = sc.constValue().sizes.find(size);
    if (s != sc.constValue().sizes.end())
        return s->second;

    FT_Error err;
    FT_Size ftsize = 0;

    err = FT_New_Size(face->face, &ftsize);
    if (err || !ftsize) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to new size: %s from %s",
                   GibbonFreetype::freetypeErrorString(err), ident.c_str());
        return shared_ptr<CacheSize>();
    }
    err = FT_Activate_Size(ftsize);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to activate size for initalization: %s from %s",
                   GibbonFreetype::freetypeErrorString(err), ident.c_str());
        return shared_ptr<CacheSize>();
    }
    err = FT_Set_Pixel_Sizes(face->face, size, size);
#ifdef FREETYPE_REREQUEST_METRICS
    {
        FT_Size_RequestRec req;

        req.type           = FT_SIZE_REQUEST_TYPE_NOMINAL;
        req.width          = size << 6;
        req.height         = size << 6;
        req.horiResolution = 0;
        req.vertResolution = 0;

        FT_Request_Metrics(ftsize->face, &req);
    }
#endif
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to set pixel size: %s from %s",
                   GibbonFreetype::freetypeErrorString(err), ident.c_str());
        return shared_ptr<CacheSize>();
    }

    shared_ptr<CacheSize> csize(new CacheSize(ftsize, face, size));
    if (persist) {
        sc->sizes[size] = csize;
    } else if (created) {
        // take it out
        assert(sc != sizeCache.end());
        sizeCache.erase(sc);
    }
    return csize;
}

void Cache::addFace(const std::string& ident, const std::string& url, const int attributes, const FontValues &fontValues, const Rect& bbox, int cmap)
{
    entries[ident].reset(new CacheEntry(ident, url, attributes, fontValues, bbox, cmap));
}

void Cache::addFace(const std::string& ident, const DataBuffer& data, const int attributes, const FontValues &fontValues, const Rect& bbox, int cmap)
{
    entries[ident].reset(new CacheEntry(ident, data, attributes, fontValues, bbox, cmap));
}

void Cache::removeFace(const std::string& ident)
{
    std::map<std::string, shared_ptr<CacheEntry> >::iterator it = entries.find(ident);
    if (it == entries.end()) {
#if defined(BUILD_DEBUG)
        Log::error(TRACE_GIBBON_TEXT, "Unable to find ident '%s' in cache", ident.c_str());
#endif
        return;
    }
    entries.erase(it);
}

static int readMmapSizes()
{
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f)
        return 0;
    int total = 0;
    char buf[1024];
    size_t len;
    while (!feof(f) && fgets(buf, 1024, f)) {
        len = strlen(buf);
        if (len < 5)
            continue;
        if (!strncmp(buf + len - 5, ".ttf", 4)) {
            char* lim = strchr(buf, ' ');
            if (lim <= buf)
                continue;
            *lim = '\0';
            lim = strchr(buf, '-');
            if (lim <= buf)
                continue;
            *lim = '\0';
            const int len = lim - buf;
            char* end;
            const ullong a = strtoull(buf, &end, 16);
            if (end - buf != 8)
                continue;
            const ullong b = strtoull(buf + len + 1, &end, 16);
            if (end - (buf + len + 1) != 8)
                continue;
            if (b < a)
                continue;
            total += (b - a);
        }
    }
    fclose(f);
    return total;
}

shared_ptr<GibbonFreetypeDumper> Cache::dumper(const DumpInfo& dump) const
{
    NRDP_UNUSED(dump);

    const int mmaps = readMmapSizes();

    shared_ptr<GibbonFreetypeDumper> d(new GibbonFreetypeDumper);
    d->keys() << "ident" << "type" << "info" << "cached" << "sizes";

    std::map<std::string, shared_ptr<CacheEntry> >::const_iterator it = entries.begin();
    int totalBuffers = 0;
    while (it != entries.end()) {
        Variant sizes;
        SizeCache::const_iterator sc = sizeCache.find(it->first);
        if (sc != sizeCache.end()) {
            std::map<int, shared_ptr<CacheSize> >::const_iterator size = sc.value().sizes.begin();
            while (size != sc.value().sizes.end()) {
                sizes.push_back(size->first);
                ++size;
            }

        }
        switch (it->second->type()) {
        case CacheEntry::Url:
            d->values() << it->first << it->second->type() << it->second->url()
                        << it->second->cached() << sizes;
            break;
        case CacheEntry::Buffer:
            totalBuffers += it->second->buffer().size();
            // fall through
        case CacheEntry::Mmap:
            d->values() << it->first << it->second->type() << it->second->size()
                        << it->second->cached() << sizes;
            break;
        default:
            break;
        }
        ++it;
    }
    Dumper& dumper = *d;
    dumper["totalBuffers"] = totalBuffers;
    dumper["mmaps"] = mmaps;
    dumper["memory"] = memory.total;
    dumper["glyphs"] = glyphCache.count();
    dumper["width"] = glyphCacheWidth;
    dumper["height"] = glyphCacheHeight;
    dumper["used"] = glyphCacheUsed;
    dumper["currentSize"] = glyphCache.currentSize();
    dumper["renderHits"] = renderHit;
    dumper["renderMisses"] = renderMiss;
    dumper["geomHits"] = geomHit;
    dumper["geomMisses"] = geomMiss;
    dumper["purgeCount"] = purgeCount;
    dumper["clearCount"] = clearCount;

    return d;
}

void Cache::dumpCacheInformation() const
{
    shared_ptr<GibbonFreetypeDumper> d = dumper(DumpInfo());

    Log::warn(TRACE_UI_ENGINE, "Cached face entries (used %d, max %d):", sizeCache.currentSize(), sizeCache.maximumSize());

    const std::vector<std::string> data = d->toConsole();
    for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
        Log::warn(TRACE_GIBBON_SURFACE, "%s", it->c_str());
    }
}

Variant Cache::cacheInformation() const
{
    shared_ptr<GibbonFreetypeDumper> d = dumper(DumpInfo());
    return d->variant();
}

CacheFace::~CacheFace()
{
    NRDP_OBJECTCOUNT_DEREF(GibbonFreetypeCacheFace, this);
    FT_Done_Face(face);
    if (!--num && !Cache::instance())
        Cache::cleanup();
}

} // namespace ftcache

std::vector<std::string> GibbonFreetypeDumper::consoleEntry(const unsigned int, const std::string&, const Variant& value)
{
    std::vector<std::string> str;
    switch (value["type"].value<unsigned int>()) {
    case ftcache::CacheEntry::Url:
        str.push_back(StringFormatter::sformat("  Ident: '%s', url: '%s', in cache: %s",
                                               value["ident"].string().c_str(),
                                               value["info"].string().c_str(),
                                               (value["cached"].boolean() ? "true" : "false")));
        break;
    case ftcache::CacheEntry::Buffer:
        str.push_back(StringFormatter::sformat("  Ident: '%s', buffer size: %d, in cache: %s",
                                               value["ident"].string().c_str(),
                                               value["info"].value<int>(),
                                               (value["cached"].boolean() ? "true" : "false")));
        break;
    case ftcache::CacheEntry::Mmap:
        str.push_back(StringFormatter::sformat("  Ident: '%s', mmap size: %d, in cache: %s",
                                               value["ident"].string().c_str(),
                                               value["info"].value<int>(),
                                               (value["cached"].boolean() ? "true" : "false")));
        break;
    default:
        break;
    }
    const Variant& sizes = value["sizes"];
    if (sizes.isArray()) {
        Variant::ArrayIterator it = sizes.arrayBegin();
        const Variant::ArrayIterator end = sizes.arrayEnd();
        while (it != end) {
            str.push_back(StringFormatter::sformat("    size %d", it->value<int>()));
            ++it;
        }
    } else {
        str.push_back("    Not in the size cache");
    }
    return str;
}

std::vector<std::string> GibbonFreetypeDumper::consolePostProcess(const Variant& values)
{
    std::vector<std::string> str;
    const int totalBuffers = values["totalBuffers"].value<int>();
    const int mmaps = values["mmaps"].value<int>();
    const int memory = values["memory"].value<int>();
    str.push_back(StringFormatter::sformat("Font memory used by built-in resources %d",
                                           totalBuffers));
    str.push_back(StringFormatter::sformat("Font memory used by FreeType mmaps %d",
                                           mmaps));
    str.push_back(StringFormatter::sformat("Font memory used by FreeType mallocs %d",
                                           memory));
    const int w = values["width"].value<int>();
    const int h = values["height"].value<int>();
    const int u = values["used"].value<int>();
    str.push_back(StringFormatter::sformat("Glyph cache, number of glyphs %d, available size %dx%d, usage %.2f%%",
                                           values["glyphs"].value<int>(), w, h,
                                           (static_cast<double>(u) / (w * h)) * 100.));
    str.push_back(StringFormatter::sformat("Total memory used %d", totalBuffers + mmaps + memory + values["currentSize"].value<int>()));

    const int renderHit = values["renderHits"].value<int>();
    const int renderMiss = values["renderMisses"].value<int>();
    const int geomHit = values["geomHits"].value<int>();
    const int geomMiss = values["geomMisses"].value<int>();
    str.push_back(StringFormatter::sformat("Hits: render %u geom %u, Misses: render %u geom %u, Hit%%: %.02f",
                                           renderHit, geomHit, renderMiss, geomMiss,
                                           (renderHit + geomHit) / static_cast<float>(renderHit + geomHit + renderMiss + geomMiss) * 100.));
    str.push_back(StringFormatter::sformat("Purge count since clear %u, clear count %u",
                                           values["purgeCount"].value<unsigned int>(),
                                           values["clearCount"].value<unsigned int>()));
    return str;
}

}
}

GibbonFreetype::GibbonFreetype(const std::string& ident, int size, const shared_ptr<ftcache::CacheSize>& entry)
    : mEntry(entry), mSize(size), mIdent(ident), mPersisted(false)
{
    NRDP_OBJECTCOUNT_REF(GibbonFreetype, this);
    NRDP_UNUSED(mSize);
}

GibbonFreetype::~GibbonFreetype()
{
    NRDP_OBJECTCOUNT_DEREF(GibbonFreetype, this);
}

bool GibbonFreetype::hasCodepoint(uint32_t cp)
{
    return mHarfbuzz->hasCodepoint(cp);
}

shared_ptr<ftcache::CacheSize> GibbonFreetype::activate()
{
    assert(mEntry->size);
    const FT_Error err = FT_Activate_Size(mEntry->size);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to activate size from cache: %s from %s",
                   freetypeErrorString(err), mEntry->face->ident.c_str());
        return shared_ptr<ftcache::CacheSize>();
    }
    return mEntry;
}

unsigned int GibbonFreetype::loadFlags(int attributes)
{
    FT_Int32 flags = FT_LOAD_NO_BITMAP|FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

    if (attributes & FontManager::Autohinter)
        flags |= FT_LOAD_FORCE_AUTOHINT;
    else
        flags |= FT_LOAD_NO_AUTOHINT;

    if (!(attributes & FontManager::Hinting))
        flags |= FT_LOAD_NO_HINTING;

    if (attributes & FontManager::HintLight)
        flags |= FT_LOAD_TARGET_LIGHT;

    if (attributes & FontManager::HintNormal)
        flags |= FT_LOAD_TARGET_NORMAL;

    if (attributes & FontManager::HintMono)
        flags |= FT_LOAD_TARGET_MONO;

    if (attributes & FontManager::HintLcd)
        flags |= FT_LOAD_TARGET_LCD;

    if (attributes & FontManager::HintLcdV)
        flags |= FT_LOAD_TARGET_LCD_V;

    return flags;
}

unsigned int GibbonFreetype::loadFlags() const
{
    return loadFlags(mEntry->face->attributes & ~FontManager::Monospace);
}

ftcache::Glyph::SharedPtr GibbonFreetype::requestGlyph(unsigned int glyph, int outline, RequestMode mode) const
{
    return ftcache::Cache::instance()->requestGlyph(mEntry, glyph, outline, mode);
}

shared_ptr<GibbonFreetypeDumper> GibbonFreetype::dumper(const DumpInfo& dump) const
{
    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* c = ftcache::Cache::instance();
    if (!c)
        return shared_ptr<GibbonFreetypeDumper>();
    return c->dumper(dump);
}

void GibbonFreetype::dumpCacheInformation()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* c = ftcache::Cache::instance();
    if (!c)
        return;
    c->dumpCacheInformation();
}

Variant GibbonFreetype::cacheInformation()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* c = ftcache::Cache::instance();
    if (!c)
        return Variant();
    return c->cacheInformation();
}

Variant GibbonFreetype::lruInformation()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* c = ftcache::Cache::instance();
    if (!c)
        return Variant();
    return c->lruToVariant();
}

void GibbonFreetype::saveCacheSurface()
{
    Surface::SharedPtr cacheSurface;
    {
        ScopedMutex locker(ftcache::Cache::mutex());
        ftcache::Cache* c = ftcache::Cache::instance();
        if (!c)
            return;
        cacheSurface = c->surface();
    }
    if(cacheSurface && SurfaceDecoders::instance()->encode(cacheSurface, "/tmp/fontcache")) {
        Log::warn(TRACE_GIBBON_TEXT, "Glyph cache saved as /tmp/fontcache (use %ld)", cacheSurface.use_count());
    }
}

Variant GibbonFreetype::getLibraryInfo()
{
    std::ostringstream os;
    Variant version;

    version["name"] = "freetype";
    os << FREETYPE_MAJOR << "." << FREETYPE_MINOR << "." << FREETYPE_PATCH;
    version["compiled"] = os.str();
    os.str("");

    FT_Library ft;
    FT_Error err = FT_Init_FreeType(&ft);
    bool ok = true;

    if (err || !ft) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: (getLibraryInfo) Unable to initialize freetype: %s", freetypeErrorString(err));
        version["runtime"] = freetypeErrorString(err);
        ok = false;
    }

    if (ok) {
        FT_Int amajor, aminor, apatch;

        FT_Library_Version(ft, &amajor, &aminor, &apatch);
        err = FT_Done_FreeType(ft);
        if (err) {
            Log::error(TRACE_GIBBON_TEXT, "FreeType: (getLibraryInfo) Unable to close freetype: %s", freetypeErrorString(err));
            version["runtime"] = freetypeErrorString(err);
            ok = false;
        }

        if (ok) {
            os << amajor << "." << aminor << "." << apatch;
            version["runtime"] = os.str();
        }
    }

    return version;
}

void GibbonFreetype::setCacheSizes(int sizeCacheSize, int glyphCacheWidth, int glyphCacheHeight)
{
    ftcache::Cache::setCacheSizes(sizeCacheSize, glyphCacheWidth, glyphCacheHeight);
}

void GibbonFreetype::verifyCache()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* cache = ftcache::Cache::instance();
    if (cache)
        cache->verify();
}

void GibbonFreetype::persist()
{
    assert(!mPersisted);
    mPersisted = true;

    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* cache = ftcache::Cache::instance();
    if (cache)
        cache->persist(mEntry);
}

void GibbonFreetype::unregisterFont(const std::string& fileName, int attributes)
{
    std::stringstream identStream;
    identStream << fileName << ":" << attributes;
    std::string ident = identStream.str();

    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* cache = ftcache::Cache::instance();
    if (cache)
        cache->removeFace(ident);
}

GibbonFreetype::SharedPtr GibbonFreetype::create(bool persist, const std::string& fileName, int sz, int attributes, const FontValues &fontValues, const Rect& bbox, const DataBuffer& ttf, int cmap)
{
    ScopedMutex locker(ftcache::Cache::mutex());
    ftcache::Cache* cache = ftcache::Cache::instance();
    if (!cache) {
        ftcache::Cache::setCacheSizes(GibbonConfiguration::fontFaceCacheCapacity(),
                                      GibbonConfiguration::fontGlyphCacheWidth(),
                                      GibbonConfiguration::fontGlyphCacheHeight());
        cache = new ftcache::Cache;
    }

    std::stringstream identStream;
    identStream << fileName << ":" << attributes;
    std::string ident = identStream.str();

    shared_ptr<ftcache::CacheSize> size = cache->entry(persist, ident, sz);
    if (!size) {
        if (!ttf.isEmpty()) {
            cache->addFace(ident, ttf, attributes, fontValues, bbox, cmap);
        } else {
            const std::string url = Configuration::resourceURL(fileName);
            if (url.size() > 7 && url.substr(0, 7) == "file://") {
                cache->addFace(ident, url.substr(7), attributes, fontValues, bbox, cmap);
            } else {
                const DataBuffer data = Configuration::resourceContent(fileName);
                if (!data.data()) {
                    Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to get resource %s", ident.c_str());
                    return GibbonFreetype::SharedPtr();
                }
                cache->addFace(ident, data, attributes, fontValues, bbox, cmap);
            }
        }
        size = cache->entry(persist, ident, sz);
        if (!size)
            return GibbonFreetype::SharedPtr();
    }

    GibbonFreetype::SharedPtr freetype(new GibbonFreetype(fileName, sz, size));
    freetype->mHarfbuzz.reset(new GibbonHarfbuzz(freetype.get(), size->size, size->face->attributes, size->face->fontValues));
    return freetype;
}

static inline int spacing(int tracking, int width, int advance, bool last)
{
    int ret = 0;
    if (last)
        ret = std::max(width, advance) + std::max(tracking, 0);
    else if (advance)
        ret = advance + tracking;
    return std::max(ret, 0);
}

static inline void calcMaxAdvance(shared_ptr<ftcache::CacheSize> size)
{
    FT_UInt idx = FT_Get_Char_Index(size->face->face, 0x2003); // EM SPACE
    if (!idx) {
        idx = FT_Get_Char_Index(size->face->face, 'M');
        if (!idx) {
            idx = FT_Get_Char_Index(size->face->face, ' ');
        }
    }
    const ftcache::Glyph::SharedPtr& glyph = ftcache::Cache::instance()->requestGlyph(size, idx, 0, GibbonFreetype::RequestGeometry);
    if (!glyph) {
        size->maxWidth = 0;
        size->maxAdvance = 0;
        return;
    }
    size->maxWidth = glyph->bounds.width();
    size->maxAdvance = glyph->horiAdvance;
}

uint32_t GibbonFreetype::fontIndex(uint32_t cp) const
{
    return FT_Get_Char_Index(mEntry->size->face, cp);
}

std::string GibbonFreetype::getGlyphName(uint32_t cp)
{
    char name[64];
    FT_Get_Glyph_Name(mEntry->size->face, cp, name, 63);

    return std::string(name);
}

void GibbonFreetype::setupHB(hb_buffer_t* buf, shared_ptr<Style> style)
{
    const FT_Error err = FT_Activate_Size(mEntry->size);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to activate size from cache: %s from %s",
                   freetypeErrorString(err), mEntry->face->ident.c_str());
        return;
    }

    netflix::gibbon::Locale locale(style->getLocale());
    if (locale.language.empty()) {
        assert(false);
        locale.language = "en"; // Default to English (ie, Latin shaper) if no language is specified (shouldn't happen)
    }

    hb_buffer_set_script(buf, HB_SCRIPT_INVALID);
    hb_buffer_set_language(buf, hb_language_from_string(locale.language.c_str(), locale.language.size()));
    hb_buffer_set_direction(buf, style->getDirection() ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
    hb_buffer_guess_segment_properties(buf);

    mHarfbuzz->setKerning(style->getTypography().kerning);
    std::string feature_settings = style->getTypography().fontFeatures;
    if (feature_settings.empty()) {
        hb_shape(mHarfbuzz->font(), buf, 0, 0);
    } else {
        std::stringstream fs(feature_settings);
        std::string item;
        std::vector<hb_feature_t> features;
        while (std::getline(fs, item, ',')) {
            hb_feature_t feature;
            if (hb_feature_from_string(item.c_str(), item.size(), &feature)) {
                features.push_back(feature);
            }
        }
        hb_shape(mHarfbuzz->font(), buf, &features[0], features.size());
    }
}

struct Render
{
    Render(GibbonFreetype* freetype, GibbonFreetype::TextData* data, const Rect& r, Style::SharedPtr st)
        : previousGlyph(0), size(freetype->size()), ignoreEmpty(true), harfbuzz(freetype->harfbuzz().get()),
          rect(r), style(st), textData(data), outlineWidth(0)
    {
        if (size->maxWidth == -1)
            calcMaxAdvance(size);
        monoSpaced = size->face->attributes & FontManager::Monospace;

        x = FTFIXED(rect.x);
        y = (int)rect.y;
        tr = FTFIXED(style->getTypography().tracking);

        const EdgeEffect edgeEffect = style->getEdgeEffect();
        if (!edgeEffect.empty() && edgeEffect.type == EdgeEffect::EDGE_EFFECT_OUTLINE)
            outlineWidth = edgeEffect.width;

        outlineColor = style->getEdgeEffect().color1;
        regularColor = style->getColor();
    }

    void operator()(const hb_glyph_info_t& info, const hb_glyph_position_t& position)
    {
        //const FT_Face face = size->face->face;
        ftcache::FtRect bounds;

        ftcache::Cache* cache = ftcache::Cache::instance();

        if (outlineWidth) {
            assert(outlineWidth > 0);
            ftcache::Glyph::SharedPtr glyph = cache->requestGlyph(size, info.codepoint, outlineWidth,
                                                                  GibbonFreetype::RequestRender);
            if (!glyph) {
                Log::error(TRACE_GIBBON_TEXT, "Unable to get glyph for outline, 0x%06X from %s",
                           info.codepoint, size->face->ident.c_str());
                return;
            }

            const int dsty = FTCEIL(size->size->metrics.ascender) - glyph->bounds.ymax - 1;
            const int gy = y + dsty - (position.y_offset / 64);

            textData->addEntry(GibbonFreetype::TextData::Outline, glyph, FTCEIL(x + position.x_offset) + glyph->bounds.xmin - outlineWidth, gy, cache->surface(), outlineColor);
        }

        {
            ftcache::Glyph::SharedPtr glyph = harfbuzz->glyph(info.codepoint);
            if (glyph) {
                if (!cache->renderGlyph(glyph.get(), size, info.codepoint, 0)) {
                    Log::error(TRACE_GIBBON_TEXT, "Unable to render glyph, 0x%06X from %s",
                               info.codepoint, size->face->ident.c_str());

                    return;
                }
            } else {
                glyph = cache->requestGlyph(size, info.codepoint, 0,
                                            GibbonFreetype::RequestRender);
                if (!glyph) {
                    Log::error(TRACE_GIBBON_TEXT, "Unable to get glyph for render, 0x%06X from %s",
                               info.codepoint, size->face->ident.c_str());

                    return;
                }
            }

            const int dsty = FTCEIL(size->size->metrics.ascender) - glyph->bounds.ymax - 1;
            const int gy = y + dsty - (position.y_offset / 64);

            textData->addEntry(GibbonFreetype::TextData::Regular, glyph, FTCEIL(x + position.x_offset) + glyph->bounds.xmin, gy, cache->surface(), regularColor);

            bounds = glyph->bounds;
        }

        x += spacing(tr, FTFIXED(bounds.totalWidth()), position.x_advance, false);

        y -= FTCEIL(position.y_advance);

        previousGlyph = info.codepoint;
    }

    int x, y, tr;
    FT_UInt previousGlyph;
    shared_ptr<ftcache::CacheSize> size;
    bool ignoreEmpty;
    bool monoSpaced;
    GibbonHarfbuzz* harfbuzz;
    Rect rect;
    Style::SharedPtr style;
    GibbonFreetype::TextData* textData;
    int outlineWidth;
    Color regularColor, outlineColor;
};

void GibbonFreetype::prepareText(hb_buffer_t* buffer, int start, int end,
                                 const Rect& rect, shared_ptr<Style> style, TextData* data)
{
    ScopedMutex locker(ftcache::Cache::mutex());

    unsigned int glyph_count;

    const FT_Error err = FT_Activate_Size(mEntry->size);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to activate size from cache: %s from %s",
                   freetypeErrorString(err), mEntry->face->ident.c_str());
        return;
    }

    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buffer, &glyph_count);

    if (end == -1 || end > static_cast<int>(glyph_count))
        end = glyph_count;
    if (start >= end)
        return;

    Render r(this, data, rect, style);
    if (!r.size)
        return;
    for (int i = start; i < end; ++i) {
        r(glyph_info[i], glyph_pos[i]);
    }
}

void GibbonFreetype::prepareText(const char* text, const Rect& rect, const Rect& dstRect, shared_ptr<Style> style, TextData* data)
{
    NRDP_UNUSED(dstRect);

    hb_buffer_t* buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, strlen(text), 0, -1);
    setupHB(buf, style);

    prepareText(buf, 0, -1, rect, style, data);

    hb_buffer_destroy(buf);
}

struct IntExtents
{
    IntExtents()
        : x1(0), y1(0), x2(0), y2(0)
    {
    }

    bool isEmpty() const { return x2 == y2 && x1 == y1; }
    int width() const { return x2 - x1; }
    int height() const { return y2 - y1; }
    void clear() { x1 = y1 = x2 = y2 = 0; }

    int x1, y1;
    int x2, y2;
};

struct Extents
{
    Extents(GibbonFreetype* ft, const shared_ptr<netflix::gibbon::Style>& st, int cpos, IntExtents* l, IntExtents* i)
        : freetype(ft), style(st), size(freetype->size()), currentPos(cpos),
          logical(l ? *l : empty), ink(i ? *i : empty), harfbuzz(ft->harfbuzz().get())
    {
        if (size->maxWidth == -1)
            calcMaxAdvance(size);
        tr = FTFIXED(style->getTypography().tracking);
        ascender = size->size->metrics.ascender;
        descender = size->size->metrics.descender;
    }
    ~Extents()
    {
    }
    void setRects(IntExtents* l, IntExtents* i)
    {
        logical = *l;
        ink = *i;
    }

    inline void unite(IntExtents& ext, int x, int y, int width, int height)
    {
        if (x < ext.x1)
            ext.x1 = x;
        if (y < ext.y1)
            ext.y1 = y;
        if (x + width > ext.x2)
            ext.x2 = x + width;
        if (y + height > ext.y2)
            ext.y2 = y + height;
    }

    inline void measure(const ftcache::Glyph::SharedPtr& glyph, const hb_glyph_position_t& position)
    {
        unite(ink, currentPos + glyph->horiBearingX + position.x_offset, -glyph->horiBearingY,
              glyph->width, glyph->height);
        //const int w = FTCEIL(position.x_offset + position.x_advance) + (last ? 0 : static_cast<int>(style->getTypography().tracking));
        const int w = position.x_advance + tr;
        unite(logical, currentPos, -ascender,
              w, ascender - descender);

        currentPos += position.x_advance + tr;
    }

    void operator()(const hb_glyph_info_t& info, const hb_glyph_position_t& position, bool /*last*/)
    {
        {
            const ftcache::Glyph::SharedPtr& glyph = harfbuzz->glyph(info.codepoint);
            if (glyph) {
                measure(glyph, position);
                return;
            }
        }
        const ftcache::Glyph::SharedPtr glyph = ftcache::Cache::instance()->requestGlyph(size, info.codepoint, 0,
                                                                                         GibbonFreetype::RequestGeometry);
        if (glyph)
            measure(glyph, position);
    }

    GibbonFreetype* freetype;
    shared_ptr<netflix::gibbon::Style> style;
    shared_ptr<ftcache::CacheSize> size;
    int currentPos, tr;
    IntExtents& logical;
    IntExtents& ink;
    IntExtents empty;
    int ascender, descender;
    GibbonHarfbuzz* harfbuzz;
};

void GibbonFreetype::getExtents(hb_buffer_t* buffer, shared_ptr<netflix::gibbon::Style> style, Rect* logical, Rect* ink, unsigned int start, unsigned int end)
{
    ScopedMutex locker(ftcache::Cache::mutex());

    const FT_Error err = FT_Activate_Size(mEntry->size);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to activate size from cache: %s from %s",
                   freetypeErrorString(err), mEntry->face->ident.c_str());
        return;
    }

    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buffer, &glyph_count);

    if (end == INT_MAX || end > glyph_count)
        end = glyph_count;

    if (logical)
        logical->clear();
    if (ink)
        ink->clear();

    IntExtents logicale, inke;
    Extents e(this, style, 0, &logicale, &inke);
    if (!e.size)
        return;
    for (unsigned int i = start; i < end; ++i) {
        e(glyph_info[i], glyph_pos[i], i == (end - 1));
    }

    if (logical)
        *logical = Rect(FTCEIL(logicale.x1), FTCEIL(logicale.y1), FTCEIL(logicale.width()), FTCEIL(logicale.height()));
    if (ink)
        *ink = Rect(FTCEIL(inke.x1), FTCEIL(inke.y1), FTCEIL(inke.width()), FTCEIL(inke.height()));
}

Rect GibbonFreetype::getCursorRect(hb_buffer_t* buffer, shared_ptr<netflix::gibbon::Style> style, unsigned int start, unsigned int index)
{
    ScopedMutex locker(ftcache::Cache::mutex());

    const FT_Error err = FT_Activate_Size(mEntry->size);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to activate size from cache: %s from %s",
                   freetypeErrorString(err), mEntry->face->ident.c_str());
        return Rect();
    }

    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buffer, &glyph_count);

    if (index > glyph_count)
        index = glyph_count;

    IntExtents logical, ink;
    Extents e(this, style, 0, &logical, &ink);
    if (!e.size)
        return Rect();
    int prevx2 = 0;

    for (unsigned int i = start; i < index; ++i) {
        prevx2 = logical.x2;
        e(glyph_info[i], glyph_pos[i], false);
    }

    return Rect(FTCEIL(logical.x2), -FTCEIL(e.ascender), FTCEIL(logical.x2 - prevx2), FTCEIL(e.ascender - e.descender));
}

void GibbonFreetype::getExtents(const char* text, shared_ptr<netflix::gibbon::Style> style, Rect* logical, Rect* ink)
{
    //ScopedMutex locker(ftcache::Cache::mutex());
    hb_buffer_t* buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, strlen(text), 0, -1);
    setupHB(buf, style);

    getExtents(buf, style, logical, ink);

    hb_buffer_destroy(buf);
}

int GibbonFreetype::minExtentX()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    const shared_ptr<ftcache::CacheSize> size = activate();
    const float ratioX = static_cast<float>(size->size->metrics.x_ppem) / size->face->face->units_per_EM;
    //const float ratioY = static_cast<float>(size->size->metrics.y_ppem) / size->face->face->units_per_EM;
    FT_BBox bbox = size->face->entry->bbox();
    if (bbox.xMin == bbox.xMax) {
        bbox = size->face->face->bbox;
        if (size->maxAdvance == -1)
            calcMaxAdvance(size);
        bbox.xMax -= size->maxAdvance;
    }
    const FT_Pos xMin = abs(static_cast<int>(floorf(bbox.xMin * ratioX)));
    const FT_Pos xMax = static_cast<int>(ceilf(bbox.xMax * ratioX));
    return std::max(xMin, xMax);
}

Coordinate GibbonFreetype::getAscent(Style::SharedPtr style)
{
    ScopedMutex locker(ftcache::Cache::mutex());
    const shared_ptr<ftcache::CacheSize> size = activate();
    if (size) {
        Coordinate sz = FTCEIL(size->size->metrics.ascender);
        const EdgeEffect edgeEffect = style->getEdgeEffect();
        if (edgeEffect.type == EdgeEffect::EDGE_EFFECT_OUTLINE)
            sz += edgeEffect.width;
        return sz;
    }
    return 0;
}

Coordinate GibbonFreetype::getDescent(Style::SharedPtr style)
{
    ScopedMutex locker(ftcache::Cache::mutex());
    const shared_ptr<ftcache::CacheSize> size = activate();
    if (size) {
        Coordinate sz = FTCEIL(size->size->metrics.descender);
        const EdgeEffect edgeEffect = style->getEdgeEffect();
        if (edgeEffect.type == EdgeEffect::EDGE_EFFECT_OUTLINE)
            sz += edgeEffect.width;
        return sz;
    }
    return 0;
}

hb_font_t* GibbonFreetype::harfbuzzFont() const
{
    const FT_Error err = FT_Activate_Size(mEntry->size);
    if (err) {
        Log::error(TRACE_GIBBON_TEXT, "FreeType: Unable to activate size from cache: %s from %s",
                   freetypeErrorString(err), mEntry->face->ident.c_str());
    }
    return mHarfbuzz->font();
}

Surface::SharedPtr GibbonFreetype::cacheSurface() const
{
    ScopedMutex locker(ftcache::Cache::mutex());
    return ftcache::Cache::instance()->surface();
}

void GibbonFreetype::clearCache()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    if (ftcache::Cache* cache = ftcache::Cache::instance())
        cache->dropGlyphCache();
}

void GibbonFreetype::destroyCache()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    delete ftcache::Cache::instance();
}

void GibbonFreetype::cleanup()
{
    ScopedMutex locker(ftcache::Cache::mutex());
    if (ftcache::Cache* cache = ftcache::Cache::instance())
        cache->clear();
}

rectpacker::Rect GibbonFreetype::adjustRect(const rectpacker::Rect& r)
{
    return ftcache::Cache::adjustRect(r);
}
