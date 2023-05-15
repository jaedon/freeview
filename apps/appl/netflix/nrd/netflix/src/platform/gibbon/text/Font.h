/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef FONT_H_
#define FONT_H_

#include <nrdbase/tr1.h>
#include <string>
#include <nrdbase/StdInt.h>
#include <sstream>
#include <algorithm>

#include <Rect.h>
#include <gibbon/config.h>

#include <GibbonFreetype.h>
#include <assert.h>

#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
# include <directfb.h>
#endif

#ifdef GIBBON_GRAPHICSENGINE_PS3
# include "FontSystem.h"
#endif

namespace netflix {
namespace gibbon {

class Style;
class Font
{
public:
    friend class FontManager;
    friend class Style;

    typedef shared_ptr<Font> SharedPtr;

    enum Weight
    {
        WEIGHT_NORMAL,
        WEIGHT_BOLD
    };

    enum Style
    {
        STYLE_NORMAL,
        STYLE_ITALIC
    };

    enum Classification
    {
        CLASS_SPACE
    };

    inline bool isEmpty() const { return mFamily.empty(); }

    inline int cmap() const { return mCMap; }
    inline Rect bbox() const { return mBbox; }
    inline const std::string &getFamily() const { return mFamily; }
    inline int32_t getPixelSize() const { return mPixelSize; }
    inline Font::Weight getWeight() const { return mWeight; }
    inline Font::Style getStyle() const { return mStyle; }
    inline const std::string &getFileName() const { return mFileName; }
    inline const DataBuffer& getTTF() const { return mTTF; }
    inline static std::string getKey(const std::string& _family, const uint32_t _pixelSize, const Font::Weight _weight, const Font::Style _style)
    {
        std::ostringstream stream;

        stream << _family << ":" << _pixelSize << ":" << (int)_weight << ":" << (int)_style;
        return stream.str();
    }

    inline std::string describe() const {
        return getKey(mFamily, mPixelSize, mWeight, mStyle);
    }

    bool isClassification(Classification cls, uint32_t idx);
    std::string getGlyphName(uint32_t idx);

    Coordinate getAscent(shared_ptr<netflix::gibbon::Style> style);
    Coordinate getDescent(shared_ptr<netflix::gibbon::Style> style);

    inline int minExtentX()
    {
        if (!mFreetype && !initFreeType())
            return 0;
        return mFreetype->minExtentX();
    }

    void setExceptions(const std::map<long unsigned int, long unsigned int>& exceptions)
    {
        mExceptions = exceptions;
    }

    bool hasCodepoint(uint32_t cp)
    {
        if (cp < 32)
            return false;
        if (!mFreetype && !initFreeType(false))
            return false;
        // check exceptions
        std::map<long unsigned int, long unsigned int>::const_iterator it = mExceptions.upper_bound(cp);
        if (it != mExceptions.begin()) {
            --it;
            if (it->first <= cp && it->second >= cp)
                return false;
        }
        const bool has = mFreetype->hasCodepoint(cp);
        if (has && !mFreetype->persisted())
            mFreetype->persist();
        return has;
    }

    void ensureCached()
    {
        if (mFreetype)
            mFreetype->ensureCached();
    }

    ~Font() { cleanup(); }

    static void unregister(const std::string& filename, int attributes);

    inline GibbonFreetype::SharedPtr getFreetype() { if(!mFreetype) initFreeType(); return mFreetype; }

protected:
    void init();
    void cleanup();

    GibbonFreetype::SharedPtr mFreetype;
    bool initFreeType(bool persist = true);

    void initClassification(Classification cls, int cnt, ...);

private:
    std::string mFamily;
    uint32_t mPixelSize;
    Font::Weight mWeight;
    Font::Style mStyle;
    std::string mFileName;
    DataBuffer mTTF;
    int mAttributes;
    FontValues mFontValues;
    std::map<long unsigned int, long unsigned int> mExceptions;
    std::vector<std::vector<uint32_t> > mClassifications;
    int mCMap;
    Rect mBbox;

    Font(const std::string& family, const std::string& filename, uint32_t pixelSize, Font::Weight weight, Font::Style style, int attributes, const FontValues &fontValues, int cmap = -1, Rect bbox = Rect(), const DataBuffer& ttf = DataBuffer())
        : mFamily(family), mPixelSize(pixelSize), mWeight(weight), mStyle(style), mFileName(filename), mTTF(ttf), mAttributes(attributes), mFontValues(fontValues), mCMap(cmap), mBbox(bbox)
    {
        init();
    }

    Font() : mFamily(""), mPixelSize(0), mWeight(WEIGHT_NORMAL), mStyle(STYLE_NORMAL), mFileName(""), mAttributes(0), mCMap(0)
    {

    }
};

inline bool Font::isClassification(Classification cls, uint32_t idx)
{
    if(!mFreetype)
        initFreeType();
    const std::vector<uint32_t>& v = mClassifications[cls];
    return std::binary_search(v.begin(), v.end(), idx);
}

}} // netflix::gibbon

inline std::ostream& operator<< (std::ostream& out, const netflix::gibbon::Font::Weight weight)
{
    if (weight == netflix::gibbon::Font::WEIGHT_BOLD)
        out << "Bold";
    else
        out << "Regular";

    return out;
}

inline std::ostream& operator<< (std::ostream& out, const netflix::gibbon::Font::Style style)
{
    if (style == netflix::gibbon::Font::STYLE_ITALIC)
        out << "Italic";
    else
        out << "Normal";

    return out;
}

inline std::ostream& operator<< (std::ostream& out, const netflix::gibbon::Font& font)
{
    out << font.getFamily() << " " << font.getWeight() << " " << font.getStyle();
    return out;
}




#endif /* FONT_H_ */
