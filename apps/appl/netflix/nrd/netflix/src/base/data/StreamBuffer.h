/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef StreamBuffer_h
#define StreamBuffer_h

#include <iostream>
#include <nrdbase/DataBuffer.h>

namespace netflix {

class StreamBuffer : public std::streambuf
{
public:
    StreamBuffer(const DataBuffer &buffer)
        : mOut(0)
    {
        mStart = mCurrent = buffer.data<const char>();
        mEnd = mStart + buffer.size();
    }

    StreamBuffer(DataBuffer &out)
        : mOut(&out)
    {
        mStart = mCurrent = out.constData<const char>();
        mEnd = mStart + out.size();
    }

    StreamBuffer(const char *data, int size)
        : mStart(data), mCurrent(data), mEnd(data + size)
    {
    }

    bool atEnd() const
    {
        return mCurrent == mEnd;
    }
    virtual int_type underflow()
    {
        if (atEnd())
            return traits_type::eof();
        return traits_type::to_int_type(*mCurrent);
    }

    virtual int_type uflow()
    {
        if (atEnd())
            return traits_type::eof();
        return traits_type::to_int_type(*mCurrent++);
    }

    virtual int_type pbackfail(int_type ch)
    {
        if (mCurrent == mStart || (ch != traits_type::eof() && ch != *(mCurrent - 1)))
            return traits_type::eof();
        return traits_type::to_int_type(*--mCurrent);
    }

    virtual std::streamsize showmanyc()
    {
        return mEnd - mCurrent;
    }

    std::streamsize xsputn(const char_type* s, std::streamsize n)
    {
        assert(mOut);
        mOut->append(s, n);
        return n;
    }
    int_type overflow(int_type c)
    {
        assert(mOut);
        mOut->append(static_cast<char_type>(c));
        return 1;
    }

private:
    const char *mStart, *mCurrent, *mEnd;
    DataBuffer *mOut;
};
}

#endif
