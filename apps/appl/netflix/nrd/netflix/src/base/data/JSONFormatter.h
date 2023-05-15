/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef JSONFormatter_h
#define JSONFormatter_h

#include <nrdbase/Variant.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/StreamBuffer.h>

namespace netflix {

class JSONFormatter
{
public:
    enum Flag {
        None = 0x00,
        NullVariantToEmptyString = 0x01,
        Pretty = 0x02,
        HalfPretty = 0x04, // use spaces but not newlines
        UnprettySmallObjects = 0x08,
        EscapeSingleQuote = 0x10,
        DataBuffersAsArrays = 0x20,
        NullVariantToUndefined = 0x40
    };
    JSONFormatter(uint32_t flags = None);
    ~JSONFormatter() {}
    std::string format(const Variant &variant, bool *truncatedPtr = 0) const
    {
        std::ostringstream stream;
        bool truncated;
        format(variant, stream, truncated);
        std::string ret = stream.str();
        if (truncatedPtr)
            *truncatedPtr = truncated;

        if (truncated) {
            assert(mMaxLength);
            ret.resize(mMaxLength);
            ret += "\nContents truncated...";
        }
        return ret;
    }

    void format(const Variant &variant, DataBuffer &out, bool *truncatedPtr = 0) const
    {
        StreamBuffer buf(out);
        std::ostream stream(&buf);
        bool truncated;
        format(variant, stream, truncated);
        if (truncatedPtr)
            *truncatedPtr = truncated;

        if (truncated) {
            assert(mMaxLength);
            out.resize(mMaxLength);
            out += "\nContents truncated...";
        }
    }

    const char *mimeType() const;
    uint32_t flags() const { return mFlags; }
    static std::string escape(const std::string & string, uint32_t flags = None);
    inline int maxStringLength() const { return mMaxStringLength; }
    inline void setMaxStringLength(int m) { mMaxStringLength = m; }
    inline void setMaxLength(int m) { mMaxLength = m; }
    inline int maxLength() const { return mMaxLength; }
private:
    void format(const Variant &variant, std::basic_ostream<char> &stream, bool &truncated) const;
    std::string jsonObject(const Variant &value) const;
    static void escape(std::basic_ostream<char> &os, const char *string, int length, uint32_t flags);
    void formatString(std::basic_ostream<char> &os, const char *string, int length) const;
    static std::string formatIndent(int indent);
    const uint32_t mFlags;
    int mMaxStringLength, mMaxLength;
};
}

#endif
