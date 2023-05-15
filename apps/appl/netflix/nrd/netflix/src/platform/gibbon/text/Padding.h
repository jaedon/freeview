/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef PADDING_H
#define PADDING_H

#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class Padding
{
public:
    Padding() : horizontal(-1), vertical(-1) { }
    Padding(int pad) : horizontal(pad), vertical(pad) { }
    Padding(int h, int v) : horizontal(h), vertical(v) { }

    inline bool operator==(const Padding& p) const
    {
        return horizontal == p.horizontal && vertical == p.vertical;
    }

    inline bool operator!=(const Padding& p) const
    {
        return !(*this == p);
    }

    std::string toString(bool verbose = false) const
    {
        std::stringstream out;
        if (verbose) {
            out << "Padding (";
            if (horizontal < 0) {
                out << "h: empty, ";
            } else {
                out << "h:" << horizontal << ", ";
            }
            if (vertical < 0) {
                out << "v: empty";
            } else {
                out << "v: " << vertical;
            }
            out << ")";
        } else {
            if (horizontal < 0) {
                out << "empty:";
            } else {
                out << horizontal;
            }
            if (vertical < 0) {
                out << "empty";
            } else {
                out << vertical;
            }
        }
        return out.str();
    }

    Variant toVariant() const
    {
        Variant data;
        data["horizontal"] = horizontal;
        data["vertical"] = vertical;
        return data;
    }

    int horizontal, vertical;
};

inline std::ostream &operator<< (std::ostream &out, const Padding &p)
{
    return out << p.toString(true);
}

}} // namespace netflix::gibbon

#endif
