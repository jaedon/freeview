/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SHADOW_H
#define SHADOW_H

#include "Color.h"
#include <TextEngine.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class Shadow
{
public:
    inline Shadow() : offsetX(0), offsetY(0) { }
    inline Shadow(int oX, int oY) : offsetX(oX), offsetY(oY) { }
    inline ~Shadow() {}

    inline bool operator==(const Shadow& s) const
    {
        return color == s.color && offsetX == s.offsetX && offsetY == s.offsetY;
    }

    inline bool operator!=(const Shadow& s) const
    {
        return !(*this == s);
    }

    inline uint8_t diff(const Shadow &s) const {
        if(s.offsetX != offsetX || s.offsetY != offsetY)
            return TextEngine::DIFF_LAYOUT|TextEngine::DIFF_RENDER;
        if(s.color != color)
            return TextEngine::DIFF_RENDER;
        return TextEngine::DIFF_NONE;
    }

    inline bool empty() const
    {
        return offsetX == 0 && offsetY == 0;
    }

    inline bool operator!() const
    {
        return empty();
    }

    inline bool isVisible() const
    {
        return !empty() && color.a;
    }

    inline std::string toString(bool verbose = false) const
    {
        std::stringstream out;
        if (verbose) {
            out << "Shadow (";
            if (empty()) {
                out << "empty";
            } else {
                out << Point(offsetX, offsetY) << ", color=" << color;
            }

            out << ")";
        } else {
            out << offsetX << ":" << offsetY << ":" << color;
        }
        return out.str();
    }

    inline Variant toVariant() const
    {
        Variant data;
        data["offsetX"] = offsetX;
        data["offsetY"] = offsetY;
        data["color"] = color.toVariant();
        return data;
    }

    int offsetX;
    int offsetY;
    Color color;
};

inline std::ostream &operator<< (std::ostream &out, const Shadow &s)
{
    return out << s.toString(true);
}

}} // namespace netflix::gibbon

#endif // SHADOW_H
