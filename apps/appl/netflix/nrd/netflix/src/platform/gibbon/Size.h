/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GIBBON_SIZE_H
#define GIBBON_SIZE_H

#include <Point.h>

#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <nrdbase/tr1.h>
#include <nrdbase/StdInt.h>

namespace netflix {
namespace gibbon {

class Size
{
public:
    inline Size() : width(0), height(0) {}
    inline Size(Coordinate iw, Coordinate ih) : width(iw), height(ih) {}
    inline Size(const Size& s) { width = s.width; height = s.height; }
    inline Size& operator=(const Size& s) { width = s.width; height = s.height; return *this; }
    inline ~Size() {}

    Coordinate width, height;

    inline bool operator==(const Size &s) const
    {
#ifdef GIBBON_COORDINATE_FLOAT
        return (gibbon_float_compare(width, s.width) && gibbon_float_compare(height, s.height));
#else
        return width == s.width && height == s.height;
#endif
    }

    inline bool operator!=(const Size &s) const
    {
        return !(*this == s);
    }

    inline void set(Coordinate iw, Coordinate ih)
    {
        width = iw;
        height = ih;
    }

    inline Size &scale(float scaleX, float scaleY)
    {
        width = GIBBON_CEIL(width * scaleX);
        height = GIBBON_CEIL(height * scaleY);
        return *this;
    }

    inline Size scaled(float scaleX, float scaleY) const
    {
        return Size(GIBBON_CEIL(width*scaleX), GIBBON_CEIL(height*scaleY));
    }

    inline void clear() { width = 0; height = 0; }

    inline bool empty() const
    {
        return width <= 0 || height <= 0;
    }

    inline std::string toString() const
    {
        std::stringstream out;
        out << "[" << width << "x" << height << "]";
        return out.str();
    }

    inline Variant toVariant() const
    {
        Variant data;
        data["width"] = width;
        data["height"] = height;
        return data;
    }
};

inline Serializer &operator<<(Serializer &serializer, const Size &s)
{
    serializeCoordinate(serializer, s.width);
    serializeCoordinate(serializer, s.height);
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, Size &s)
{
    deserializeCoordinate(deserializer, s.width);
    deserializeCoordinate(deserializer, s.height);
    return deserializer;
}
}
}

#endif
