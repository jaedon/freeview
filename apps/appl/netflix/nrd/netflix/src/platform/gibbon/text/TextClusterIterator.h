/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEXTCLUSTERITERATOR_H
#define TEXTCLUSTERITERATOR_H

#include <itemizer/TextItemizer.h>
#include <hb.h>
#include <limits.h>
#include <assert.h>

namespace netflix {
namespace gibbon {

class TextClusterIterator
{
public:
    TextClusterIterator(const GlyphString& string, int start = 0, int end = -1);
    TextClusterIterator(hb_buffer_t* buffer, int start = 0, int end = -1);
    ~TextClusterIterator();

    bool atEnd() const;

    uint32_t cluster() const;
    int pos() const;

    uint32_t operator++();    // prefix
    uint32_t operator++(int); // postfix

private:
    void init();

private:
    hb_buffer_t* mBuffer;
    hb_glyph_info_t* mInfo;
    unsigned int mSize;
    int mPos, mStart, mEnd;
};

class TextClusterReverseIterator
{
public:
    TextClusterReverseIterator(const GlyphString& string, int start = 0, int end = -1);
    TextClusterReverseIterator(hb_buffer_t* buffer, int start = 0, int end = -1);
    ~TextClusterReverseIterator();

    bool atEnd() const;

    uint32_t cluster() const;
    int pos() const;

    uint32_t operator++();    // prefix
    uint32_t operator++(int); // postfix

private:
    void init();

private:
    hb_buffer_t* mBuffer;
    hb_glyph_info_t* mInfo;
    unsigned int mSize;
    int mPos, mStart, mEnd;
};

inline TextClusterIterator::TextClusterIterator(const GlyphString& string, int start, int end)
    : mBuffer(string.buffer()), mPos(0), mStart(start), mEnd(end)
{
    init();
}

inline TextClusterIterator::TextClusterIterator(hb_buffer_t* buffer, int start, int end)
    : mBuffer(buffer), mPos(0), mStart(start), mEnd(end)
{
    init();
}

inline TextClusterIterator::~TextClusterIterator()
{
    hb_buffer_destroy(mBuffer);
}

inline void TextClusterIterator::init()
{
    hb_buffer_reference(mBuffer);

    mInfo = hb_buffer_get_glyph_infos(mBuffer, &mSize);
    if (!mInfo || !mSize) {
        mStart = mEnd = 0;
        return;
    }
    if (mEnd == -1)
        mEnd = static_cast<int>(mSize);
    assert(mStart < mEnd);
    mPos = mStart;
}

inline bool TextClusterIterator::atEnd() const
{
    return (mPos >= mEnd);
}

inline uint32_t TextClusterIterator::cluster() const
{
    return mInfo[mPos].cluster;
}

inline int TextClusterIterator::pos() const
{
    return mPos;
}

inline uint32_t TextClusterIterator::operator++()
{
    assert(!atEnd());
    const uint32_t oc = mInfo[mPos].cluster;
    uint32_t c;
    do {
        ++mPos;
        if (mPos == mEnd)
            return UINT_MAX;
        c = mInfo[mPos].cluster;
    } while (c == oc);
    return c;
}

inline uint32_t TextClusterIterator::operator++(int)
{
    assert(!atEnd());
    const uint32_t oc = mInfo[mPos].cluster;
    uint32_t c;
    do {
        ++mPos;
        if (mPos == mEnd)
            return oc;
        c = mInfo[mPos].cluster;
    } while (c == oc);
    return oc;
}

inline TextClusterReverseIterator::TextClusterReverseIterator(const GlyphString& string, int start, int end)
    : mBuffer(string.buffer()), mStart(start), mEnd(end)
{
    init();
}

inline TextClusterReverseIterator::TextClusterReverseIterator(hb_buffer_t* buffer, int start, int end)
    : mBuffer(buffer), mStart(start), mEnd(end)
{
    init();
}

inline TextClusterReverseIterator::~TextClusterReverseIterator()
{
    hb_buffer_destroy(mBuffer);
}

inline void TextClusterReverseIterator::init()
{
    hb_buffer_reference(mBuffer);

    mInfo = hb_buffer_get_glyph_infos(mBuffer, &mSize);
    if (!mInfo || !mSize) {
        mStart = mEnd = 0;
        mPos = -1;
        return;
    }
    if (mEnd == -1)
        mEnd = static_cast<int>(mSize);
    assert(mStart < mEnd);
    mPos = mEnd - 1;
}

inline bool TextClusterReverseIterator::atEnd() const
{
    return (mPos < mStart);
}

inline uint32_t TextClusterReverseIterator::cluster() const
{
    return mInfo[mPos].cluster;
}

inline int TextClusterReverseIterator::pos() const
{
    return mPos;
}

inline uint32_t TextClusterReverseIterator::operator++()
{
    assert(!atEnd());
    const uint32_t oc = mInfo[mPos].cluster;
    uint32_t c;
    do {
        --mPos;
        if (mPos == mStart - 1)
            return UINT_MAX;
        c = mInfo[mPos].cluster;
    } while (c == oc);
    return c;
}

inline uint32_t TextClusterReverseIterator::operator++(int)
{
    assert(!atEnd());
    const uint32_t oc = mInfo[mPos].cluster;
    uint32_t c;
    do {
        --mPos;
        if (mPos == mStart - 1)
            return oc;
        c = mInfo[mPos].cluster;
    } while (c == oc);
    return oc;
}

}} // namespace netflix::gibbon

#endif
