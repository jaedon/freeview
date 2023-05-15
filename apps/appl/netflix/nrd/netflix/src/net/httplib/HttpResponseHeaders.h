/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HttpResponseHeaders_h
#define HttpResponseHeaders_h

#include <utility>
#include <string>
#include <nrdbase/RefCounted.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/Variant.h>

struct HttpResponseHeadersDataCount : public netflix::ObjectCount::Record
{
public:
    inline HttpResponseHeadersDataCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    virtual std::string describe(void *object) const;
};

NRDP_DECLARE_OBJECTCOUNT_RECORD(HttpResponseHeadersDataCount, HttpResponseHeadersData);

namespace netflix {
class HttpResponseHeaders
{
public:
    HttpResponseHeaders()
        : mData(0)
    {}

    HttpResponseHeaders(const HttpResponseHeaders &other)
        : mData(other.mData)
    {
        if (mData)
            mData->ref();
    }

    HttpResponseHeaders &operator=(const HttpResponseHeaders &other)
    {
        if (mData)
            mData->deref();
        mData = other.mData;
        if (mData)
            mData->ref();
        return *this;
    }

    ~HttpResponseHeaders()
    {
        if (mData)
            mData->deref();
    }

    void clear()
    {
        if (mData) {
            mData->deref();
            mData = 0;
        }
    }

    size_t size() const
    {
        return mData ? mData->ptr()->headers.size() : 0;
    }

    bool empty() const
    {
        return !mData || mData->ptr()->headers.empty();
    }

    void reserve(size_t size)
    {
        detach();
        if (!mData) {
            mData = new RefCounted<Data>(size);
        }
    }

    void resize(size_t size)
    {
        if (mData) {
            detach();
        } else if (size) {
            mData = new RefCounted<Data>(size);
        } else {
            return;
        }
        mData->ptr()->headers.resize(size);
    }

    std::string toString() const
    {
        return mData ? mData->ptr()->toString() : std::string();
    }

    Variant toVariant() const
    {
        Variant ret;
        ret.resizeArray(size());
        int i = 0;
        for (HttpResponseHeaders::const_iterator it = begin(); it != end(); ++it) {
            ret[i++] = it->header + ": " + it->value;
        }
        return ret;
    }

    operator Variant () const
    {
        return toVariant();
    }

    struct Header {
        Header(const std::string &k = std::string(), const std::string &v = std::string())
            : header(k), value(v)
        {
        }
        Header(const Header &other)
            : header(other.header), value(other.value)
        {
        }

        Header(const std::pair<std::string, std::string> &pair)
            : header(pair.first), value(pair.second)
        {
        }

        operator std::pair<std::string, std::string> () const
        {
            return std::make_pair(header, value);
        }

        std::string header;
        std::string value;
    };

    typedef std::vector<Header>::const_iterator const_iterator;
    typedef std::vector<Header>::iterator iterator;

    const_iterator begin() const
    {
        if (!mData)
            return null().begin();
        return mData->ptr()->headers.begin();
    }
    const_iterator end() const
    {
        if (!mData)
            return null().end();
        return mData->ptr()->headers.end();
    }
    iterator begin()
    {
        if (!mData) {
            return null().begin();
        }
        mData = static_cast<RefCounted<Data> *>(mData)->detach();
        return mData->ptr()->headers.begin();
    }
    iterator end()
    {
        if (!mData) {
            return null().end();
        }
        mData = static_cast<RefCounted<Data> *>(mData)->detach();
        return mData->ptr()->headers.end();
    }
    bool detach()
    {
        if (mData) {
            mData = static_cast<RefCounted<Data> *>(mData)->detach();
            return true;
        }
        return false;
    }

    bool contains(const std::string &header) const
    {
        return find(header) != end();
    }

    std::string value(const std::string &header, const std::string &def = std::string()) const
    {
        const const_iterator it = find(header);
        return it == end() ? def : it->value;
    }

    int indexOf(const std::string &header, int start) const
    {
        if (!mData || static_cast<size_t>(start) >= mData->ptr()->headers.size())
            return -1;

        const const_iterator it = find(header, begin() + start);
        if (it == end())
            return -1;
        return it - begin();
    }

    const_iterator find(const std::string &header) const
    {
        if (!mData)
            return end();
        return find(header, begin());
    }

    const_iterator find(const std::string &header, const_iterator begin) const
    {
        if (!mData)
            return end();

        const const_iterator e = end();
        while (begin != e && strcasecmp(begin->header.c_str(), header.c_str()))
            ++begin;

        return begin;
    }

    iterator find(const std::string &header)
    {
        if (!mData)
            return end();
        return find(header, begin()); // begin() will detach
    }

    iterator find(const std::string &header, iterator start)
    {
        const iterator e = end();
        if (!mData)
            return e;

        while (start != e && strcasecmp(start->header.c_str(), header.c_str()))
            ++start;

        return start;
    }

    const Header &operator[](size_t idx) const
    {
        if (mData)
            return mData->ptr()->headers[idx];
        return null()[idx];
    }

    Header &operator[](size_t idx)
    {
        assert(mData);
        if (mData) {
            mData = static_cast<RefCounted<Data> *>(mData)->detach();
            assert(mData->ptr()->headers.size() > idx);
            return mData->ptr()->headers[idx];
        }
        return null()[idx];
    }

    void push_back(const Header &header)
    {
        if (!mData) {
            mData = new RefCounted<Data>(1);
        } else {
            mData = static_cast<RefCounted<Data> *>(mData)->detach();
        }
        mData->ptr()->headers.push_back(header);
    }

    void push_back(const std::pair<std::string, std::string> &header)
    {
        push_back(Header(header));
    }

    iterator insert(iterator position, const Header &header)
    {
        if (!mData) {
            mData = new RefCounted<Data>(1);
        } else {
            mData = static_cast<RefCounted<Data> *>(mData)->detach();
        }
        if (mData->ptr()->headers.empty())
            position = mData->ptr()->headers.begin();
        return mData->ptr()->headers.insert(position, header);
    }
    iterator insert(iterator position, const std::pair<std::string, std::string> &header)
    {
        return insert(position, Header(header));
    }
private:
    static std::vector<Header> &null();
    struct Data
    {
        Data(int reserveSize = 0)
        {
            NRDP_OBJECTCOUNT_REF(HttpResponseHeadersData, this);
            if (reserveSize)
                headers.reserve(reserveSize);
        }
        Data(const Data &other)
            : headers(other.headers)
        {
            NRDP_OBJECTCOUNT_REF(HttpResponseHeadersData, this);
        }
        ~Data()
        {
            NRDP_OBJECTCOUNT_DEREF(HttpResponseHeadersData, this);
        }

        std::string toString() const
        {
            std::string ret;
            for (std::vector<Header>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
                if (!ret.empty())
                    ret += '\n';
                ret += it->header + ": " + it->value;
            }
            return ret;
        }

        std::vector<Header> headers;
    private:
        Data &operator=(const Data &);
    };
    friend struct ::HttpResponseHeadersDataCount;
    RefCounted<Data> *mData;
};
};

#endif
