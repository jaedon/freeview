/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AsciiTable_h
#define AsciiTable_h

#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Variant.h>
#include <nrdbase/StringUtils.h>

namespace netflix {

class AsciiTable
{
public:
    inline AsciiTable(int count, ...);
    inline AsciiTable(const char **fields);
    inline AsciiTable(const std::string &fields); // comma separated

    template <typename T1>
    inline void addRow(T1 t1);
    template <typename T1, typename T2>
    inline void addRow(T1 t1, T2 t2);
    template <typename T1, typename T2, typename T3>
    inline void addRow(T1 t1, T2 t2, T3 t3);
    template <typename T1, typename T2, typename T3, typename T4>
    inline void addRow(T1 t1, T2 t2, T3 t3, T4 t4);
    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    inline void addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5);
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline void addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6);
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline void addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7);
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    inline void addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8);
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    inline void addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9);

    class Stream
    {
    public:
        template <typename T> Stream operator<<(const T &t)
        {
            std::ostringstream out;
            out << std::boolalpha << t;
            mData->mRow.push_back(out.str());
            return *this;
        }
    private:
        Stream(AsciiTable *table)
            : mData(new Data(table))
        {
        }
        struct Data {
            Data(AsciiTable *table)
                : mTable(table)
            {}
            ~Data()
            {
                mTable->addRow(mRow);
            }
            AsciiTable *mTable;
            std::vector<std::string> mRow;
        };
        shared_ptr<Data> mData;
        friend class AsciiTable;
    };

    template <typename T>
    Stream operator<<(const T &t)
    {
        Stream stream(this);
        stream << t;
        return stream;
    }

    void addRow(const std::vector<std::string> &row);
    template <typename T> inline void addRow(int count, ...);

    inline void addFormat(const char *format, ...) _NETFLIX_PRINTF_ATTR(2, 3);

    enum DisplayMode {
        Display_Table,
        Display_List
    };
    inline std::string toString(DisplayMode mode = Display_Table) const;
    inline Variant toVariant() const;
private:
    void initHeaders(const std::vector<std::string> &headers);
    enum Type {
        Invalid = 0,
        Int = 'd',
        Unsigned = 'u',
        Double = 'f',
        String = 's',
        Char = 'c',
        Hex = 'x',
        Pointer = 'p',
        Long = Int * 2,
        LongLong = Int * 3,
        UnsignedLong = Unsigned * 2,
        UnsignedLongLong = Unsigned * 3
    };
    static Type type(const char *ch)
    {
        switch (*ch) {
        case Int:
        case Unsigned:
        case String:
        case Char:
        case Hex:
        case Double:
        case Pointer:
            return static_cast<Type>(*ch);
        case 'z':
        case 'l':
            switch (*(ch + 1)) {
            case 'd': return Long;
            case 'u': return UnsignedLong;
            case 'l':
                switch (*(ch + 2)) {
                case 'd': return LongLong;
                case 'u': return UnsignedLongLong;
                }
            }
            break;
        }
        printf("Invalid format character %c\n", *ch);
        return Invalid;
    }
    enum Alignment {
        Align_Left,
        Align_Right
    };
    void writeRow(std::ostringstream &out, const std::vector<std::string> &row,
                  const std::vector<int> &maxLengths, Alignment alignment) const
    {
        if (alignment == Align_Left) {
            out << "| ";
        } else {
            out << '|';
        }
        const size_t count = mHeaders.size();
        for (size_t i=0; i<count; ++i) {
            writeField(out, maxLengths.at(i) + 1, row.size() > i ? row.at(i) : std::string(), alignment);
            if (alignment == Align_Left) {
                out << '|';
                if (i + 1 < count)
                    out << ' ';
            } else {
                out << " |";
            }
        }
        out << '\n';
    }
    static void writeField(std::ostringstream &out, size_t width, const std::string &string, Alignment alignment)
    {
        assert(string.size() <= width);
        if (alignment == Align_Right) {
            if (width > string.size())
                out << std::string(width - string.size(), ' ');
            out << string;
        } else {
            out << string;
            if (width > string.size())
                out << std::string(width - string.size(), ' ');
        }
    }
    template <typename T> void add(T t)
    {
        assert(!mValues.empty());
        Variant &row = mValues.back();
        row.push_back(t);
        assert(row.size() <= static_cast<int>(mHeaders.size()));
    }

    enum HeaderFlag {
        None = 0x0,
        PrettySize = 0x1
    };
    struct Header {
        Header(const std::string &t)
            : flags(None)
        {
            const int pipe = t.find('|');
            const int colon = t.find(':');
            assert(pipe == -1 || pipe >= colon);
            if (pipe != -1) {
                std::vector<std::string> pipes;
                StringTokenizer::tokenize(t, pipes, "|");
                const int c = pipes.size();
                for (int i=1; i<c; ++i) {
                    if (!strcasecmp(pipes.at(i).c_str(), "prettysize")) {
                        flags |= PrettySize;
                    } else {
                        fprintf(stderr, "Unknown flag: [%s]\n", pipes.at(i).c_str());
                    }
                }
            }
            if (colon == -1) {
                if (pipe == -1) {
                    text = t;
                } else {
                    text = t.substr(0, pipe);
                }
            } else {
                text = t.substr(0, colon);
                if (pipe == -1) {
                    suffix = t.substr(colon + 1);
                } else {
                    suffix = t.substr(colon + 1, pipe - colon - 1);
                }
            }
        }

        std::string text;
        std::string suffix;
        unsigned int flags;
    };
    std::vector<Header> mHeaders;
    std::vector<Variant> mValues;
};

inline AsciiTable::AsciiTable(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    std::vector<std::string> split;
    split.reserve(count);
    for (int i=0; i<count; ++i) {
        const std::string header = va_arg(ap, const char*);
        split.push_back(header);
    }
    va_end(ap);
    initHeaders(split);
}

inline AsciiTable::AsciiTable(const char **headers)
{
    std::vector<std::string> split;
    while (*headers) {
        split.push_back(*headers);
        ++headers;
    }
    initHeaders(split);
}

inline AsciiTable::AsciiTable(const std::string &headers)
{
    std::vector<std::string> split;
    StringTokenizer::tokenize(headers, split, ",");
    initHeaders(split);
}

inline void AsciiTable::initHeaders(const std::vector<std::string> &headers)
{
    assert(!headers.empty());
    assert(mHeaders.empty());
    mHeaders.reserve(headers.size());
    for (std::vector<std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        mHeaders.push_back(*it);
    }
}

template <typename T1>
inline void AsciiTable::addRow(T1 t1)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
}

template <typename T1, typename T2>
inline void AsciiTable::addRow(T1 t1, T2 t2)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
}

template <typename T1, typename T2, typename T3>
inline void AsciiTable::addRow(T1 t1, T2 t2, T3 t3)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
    add(t3);
}

template <typename T1, typename T2, typename T3, typename T4>
inline void AsciiTable::addRow(T1 t1, T2 t2, T3 t3, T4 t4)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
    add(t3);
    add(t4);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline void AsciiTable::addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
    add(t3);
    add(t4);
    add(t5);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline void AsciiTable::addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
    add(t3);
    add(t4);
    add(t5);
    add(t6);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline void AsciiTable::addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
    add(t3);
    add(t4);
    add(t5);
    add(t6);
    add(t7);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline void AsciiTable::addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
    add(t3);
    add(t4);
    add(t5);
    add(t6);
    add(t7);
    add(t8);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline void AsciiTable::addRow(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9)
{
    mValues.push_back(std::vector<std::string>());
    add(t1);
    add(t2);
    add(t3);
    add(t4);
    add(t5);
    add(t6);
    add(t7);
    add(t8);
    add(t9);
}

template <typename T>
inline void AsciiTable::addRow(int count, ...)
{
    assert(!mHeaders.empty());
    va_list ap;
    va_start(ap, count);
    mValues.push_back(std::vector<std::string>());
    for (int i=0; i<count; ++i) {
        T t = va_arg(ap, T);
        add(t);
    }
    va_end(ap);
}

inline void AsciiTable::addRow(const std::vector<std::string> &row)
{
    assert(row.size() <= mHeaders.size());
    mValues.push_back(row);
}

inline void AsciiTable::addFormat(const char *format, ...)
{
    assert(!mHeaders.empty());
    va_list ap;
    va_start(ap, format);
    mValues.push_back(std::vector<std::string>());
    const char *ch = format;
    while (true) {
        ch = strchr(ch, '%');
        if (!ch)
            break;
        switch (type(++ch)) {
        case Invalid: assert(0); break;
        case Int: add(va_arg(ap, int)); break;
        case Unsigned: add(va_arg(ap, unsigned)); break;
        case Double: add(va_arg(ap, double)); break;
        case String: add(va_arg(ap, const char *)); break;
        case Char: add(static_cast<char>(va_arg(ap, int))); break;
        case Long: add(static_cast<long long>(va_arg(ap, long))); break;
        case LongLong: add(va_arg(ap, long long)); break;
        case UnsignedLong: add(static_cast<unsigned long long>(va_arg(ap, unsigned long))); break;
        case UnsignedLongLong: add(va_arg(ap, unsigned long long)); break;
        case Pointer: add(va_arg(ap, void*)); break;
        case Hex: {
            const int val = va_arg(ap, int);
            char buf[32];
            snprintf(buf, sizeof(buf), "0x%x", val);
            add(buf);
            break; }
        }
    }
}

inline std::string AsciiTable::toString(DisplayMode mode) const
{
    std::ostringstream out;
    std::vector<std::string> headers;
    headers.reserve(mHeaders.size());
    std::vector<int> maxLengths(mHeaders.size());
    int idx = 0;
    if (mode == Display_Table) {
        for (std::vector<Header>::const_iterator it = mHeaders.begin(); it != mHeaders.end(); ++it) {
            headers.push_back(it->text);
            maxLengths[idx++] = it->text.size();
        }
    }

    std::vector<std::vector<std::string> > data;
    for (Variant::ArrayIterator it = mValues.begin(); it != mValues.end(); ++it) {
        data.push_back(std::vector<std::string>());
        std::vector<std::string> &row = data.back();
        const Variant &rowVariant = *it;
        const int count = rowVariant.size();
        row.resize(count);
        for (int i=0; i<count; ++i) {
            std::string &str = row[i];
            if (mHeaders.at(i).flags & PrettySize) {
                str = prettyBytes(rowVariant[i].value<double>());
            } else {
                str = rowVariant[i].value<std::string>();
            }
            if (!mHeaders.at(i).suffix.empty())
                str += mHeaders.at(i).suffix;
            maxLengths[i] = std::max<int>(maxLengths.at(i), str.size());
        }
    }

    std::string line;
    if (mode == Display_Table) {
        writeRow(out, headers, maxLengths, Align_Left);
        int rowLength = out.str().length() - 1;
        line.assign(rowLength, '-');
        out << line << '\n';
    }

    for (std::vector<std::vector<std::string> >::const_iterator it = data.begin(); it != data.end(); ++it) {
        const std::vector<std::string> &row = *it;
        if (mode == Display_Table) {
            writeRow(out, row, maxLengths, Align_Right);
        } else {
            for (std::vector<std::string>::const_iterator rit = row.begin(); rit != row.end(); ++rit) {
                if (rit != row.begin())
                    out << ", ";
                out << *rit;
            }
        }
    }
    if (mode == Display_Table) {
        out << line << '\n';
        return line + '\n' + out.str();
    }
    return out.str();
}

inline Variant AsciiTable::toVariant() const
{
    Variant ret;
    Variant headers;
    const int count = mHeaders.size();
    headers.resizeArray(count);
    for (int i=0; i<count; ++i) {
        const Header &h = mHeaders.at(i);
        Variant &header = headers[i];
        header["text"] = h.text;
        if (!h.suffix.empty())
            header["suffix"] = h.suffix;
        if (h.flags & PrettySize)
            header["flags"] = "PrettySize";
    }

    ret["headers"] = headers;
    ret["data"] = mValues;
    return ret;
}
}

#endif
