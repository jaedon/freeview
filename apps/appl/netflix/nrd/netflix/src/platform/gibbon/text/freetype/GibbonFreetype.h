/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GIBBONFREETYPE_H
#define GIBBONFREETYPE_H

#include <Rect.h>
#include <nrdbase/tr1.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/DumpInfo.h>
#include <nrdbase/Variant.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/Noncopyable.h>
#include <FontManagerValues.h>
#include <RectPacker.h>
#include <Color.h>

#include <gibbon/config.h>

#include <string>
#include <map>

struct hb_glyph_position_t;
struct hb_glyph_info_t;
struct hb_font_t;
struct hb_buffer_t;

namespace netflix {
namespace gibbon {

class Style;
class Surface;

class GibbonHarfbuzz;

class GibbonFreetypeDumper;

namespace ftcache {
class Cache;
class CacheSize;

struct FtRect
{
    FtRect() : xmin(0), xmax(0), ymin(0), ymax(0) { }
    FtRect(int left, int top, int right, int bottom)
        : xmin(left), xmax(right), ymin(top), ymax(bottom) { }

    void include(int x, int y)
    {
        xmin = std::min(xmin, x);
        ymin = std::min(ymin, y);
        xmax = std::max(xmax, x);
        ymax = std::max(ymax, y);
    }

    void clear() { xmin = xmax = ymin = ymax = 0; }
    bool isEmpty() const { return xmin == 0 && xmax == 0 && ymin == 0 && ymax == 0; }

    Rect toRect() const { return Rect(xmin, ymin, width(), height()); }

    int width() const { return xmax - xmin + 1; }
    int height() const { return ymax - ymin + 1; }
    int totalWidth() const { return xmax - xmin + 1 + (xmin > 0 ? xmin : 0); }
    int totalHeight() const { return ymax - ymin + 1 + (ymin > 0 ? ymin : 0); }

    int xmin, xmax, ymin, ymax;
};

struct Glyph
{
    typedef shared_ptr<Glyph> SharedPtr;

    rectpacker::RectPacker::Node* glyphNode;

    FtRect bounds;
    int advance, horiAdvance;
    int horiBearingX, horiBearingY, vertBearingX, vertBearingY;
    int width, height;
    int origAdvance;

    int outlineWidth;
    unsigned int cacheSerial;

    Variant toVariant() const;
};
}

class GibbonFreetype : public Noncopyable
{
public:
    ~GibbonFreetype();

    typedef shared_ptr<GibbonFreetype> SharedPtr;
    typedef weak_ptr<GibbonFreetype> WeakPtr;

    enum RequestMode { RequestRender, RequestGeometry };
    ftcache::Glyph::SharedPtr requestGlyph(unsigned int glyph, int outline = 0,
                                           RequestMode mode = RequestRender) const;

    static SharedPtr create(bool persist, const std::string& ident, int size, int attributes, const netflix::gibbon::FontValues &fontValues, const Rect& bbox, const DataBuffer& ttf, int cmap = -1);
    static void cleanup();
    static void clearCache();
    static void destroyCache();
    static Variant getLibraryInfo();

    static void setCacheSizes(int sizeCacheSize, int glyphCacheWidth, int glyphCacheHeight);
    static rectpacker::Rect adjustRect(const rectpacker::Rect& r);

    int minExtentX();

    Coordinate getAscent(shared_ptr<Style> style);
    Coordinate getDescent(shared_ptr<Style> style);
    void getExtents(hb_buffer_t* buffer, shared_ptr<netflix::gibbon::Style> style, Rect* logical, Rect* ink, unsigned int start = 0, unsigned int end = INT_MAX);
    void getExtents(const char* text, shared_ptr<netflix::gibbon::Style> style, Rect* logical, Rect* ink);
    Rect getCursorRect(hb_buffer_t* buffer, shared_ptr<netflix::gibbon::Style>, unsigned int start, unsigned int index);

    unsigned int loadFlags() const;
    static unsigned int loadFlags(int attributes);

    struct TextData
    {
        enum Type { Regular, Outline };

        virtual ~TextData() { }

        virtual void addEntry(Type type, const ftcache::Glyph::SharedPtr& glyph, int tx, int ty, const shared_ptr<Surface>& surface, const Color& color) = 0;
        virtual void clear() = 0;
    };

    void prepareText(const char* text, const Rect& rect, const Rect& dstRect, shared_ptr<Style> style, TextData* data);
    void prepareText(hb_buffer_t* buffer, int start, int end, const Rect& rect,
                     shared_ptr<Style> style, TextData* data);

    uint32_t fontIndex(uint32_t cp) const;
    std::string getGlyphName(uint32_t cp);

    hb_font_t* harfbuzzFont() const;
    shared_ptr<GibbonHarfbuzz> harfbuzz() const { return mHarfbuzz; }

    shared_ptr<Surface> cacheSurface() const;

    static void dumpCacheInformation();
    static Variant cacheInformation();
    static Variant lruInformation();
    static void saveCacheSurface();

    shared_ptr<ftcache::CacheSize> activate();
    shared_ptr<ftcache::CacheSize> size() const { return mEntry; }

    static void unregisterFont(const std::string& fileName, int attributes);

    std::string ident() const { return mIdent; }

    bool hasCodepoint(uint32_t cp);

    void ensureCached();
    void persist();
    bool persisted() const { return mPersisted; }

    static const char* freetypeErrorString(int code);

private:
    static void verifyCache();

private:
    void setupHB(hb_buffer_t* buf, shared_ptr<Style> style);
    shared_ptr<GibbonFreetypeDumper> dumper(const DumpInfo& dump = DumpInfo()) const;

    GibbonFreetype(const std::string& ident, int size, const shared_ptr<ftcache::CacheSize>& entry);

    shared_ptr<ftcache::CacheSize> mEntry;
    int mSize;
    std::string mIdent;
    shared_ptr<GibbonHarfbuzz> mHarfbuzz;
    bool mPersisted;

    friend class ftcache::CacheSize;
};

inline void GibbonFreetype::ensureCached()
{
    if (mEntry.use_count() == 1) {
        mPersisted = false;
        persist();
    }
}

}}

#endif
