/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEXTTRUNCATION_H
#define TEXTTRUNCATION_H

namespace netflix {
namespace gibbon {

#include <nrdbase/Variant.h>

class TextTruncation
{
public:

    enum Position {
        POSITION_NONE,
        POSITION_START,
        POSITION_MIDDLE,
        POSITION_END
    };

    static std::string defaultEllipsis()
    {
        const unsigned char data[] = { 0xE2, 0x80, 0xA6 }; // U+2026
        return std::string(reinterpret_cast<const char*>(data), sizeof(data));
    }

    TextTruncation() : position(POSITION_NONE), ellipsis(defaultEllipsis()) { }
    TextTruncation(Position pos, const std::string &ell) : position(pos), ellipsis(ell) { }
    ~TextTruncation() {}

    bool operator==(const TextTruncation& t) const
    {
        return position == t.position && ellipsis == t.ellipsis;
    }

    bool operator!=(const TextTruncation& t) const
    {
        return !(*this == t);
    }

    bool empty() const {
        return position == POSITION_NONE;
    }

    inline bool operator!() const
    {
        return empty();
    }

    Position position;
    std::string ellipsis;

    inline std::string toString(bool verbose = false) const
    {
        std::stringstream out;
        if (verbose) {
            out << "Truncation (";
            switch (position) {
            case TextTruncation::POSITION_NONE:
                out << "none";
                break;
            case TextTruncation::POSITION_START:
                out << "start";
                break;
            case TextTruncation::POSITION_MIDDLE:
                out << "middle";
                break;
            case TextTruncation::POSITION_END:
                out << "end";
                break;
            default:
                break;
            }

            if (position != TextTruncation::POSITION_NONE) {
                out << ", ellipsis=|" << ellipsis << "|";
            }

            out << ")";
        } else {
            out << (int)position << ":" << ellipsis;
        }
        return out.str();
    }

    Variant toVariant() const
    {
        Variant data;
        data["position"] = position;
        data["ellipsis"] = ellipsis;
        return data;
    }
};

inline std::ostream &operator<< (std::ostream &out, const TextTruncation &t)
{
    return out << t.toString(true);
}

}} // namespace netflix::gibbon

#endif // TEXTTRUNCATION_H
