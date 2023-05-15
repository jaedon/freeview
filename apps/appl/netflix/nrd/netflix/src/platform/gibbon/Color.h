/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef COLOR_H
#define COLOR_H

#include <assert.h>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <nrdbase/Serialize.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class Color
{
public:
    Color() : r(0), g(0), b(0), a(255) {}
    Color(unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia = 255) :
            r(ir), g(ig), b(ib), a(ia) {}
    ~Color() {}

    Color(const char* text) : r(0), g(0), b(0), a(255)
    {
        const char* hexString = "#ffffffff"; //Opaque white is the default
        const char* cssColors[18][2] =
        {
                { "maroon", "#800000" }, { "red", "#ff0000" }, { "orange", "#ffa500" },
                { "yellow", "#ffff00" }, { "olive", "#808000" }, { "purple", "#800080" },
                { "fuchsia", "#ff00ff" }, { "white", "#ffffff" }, { "lime", "#00ff00" },
                { "green", "#008000" }, { "navy", "#000080" }, { "blue", "#0000ff" },
                { "aqua", "#00ffff" }, { "teal", "#008080" }, { "black", "#000000" },
                { "silver", "#c0c0c0" }, { "gray", "#808080" },
                { NULL , NULL }
        };

        if (*text != '#') { // named color
            int i = 0;

            do {
                if (strcmp(text, cssColors[i][0]) == 0) {
                    hexString = cssColors[i][1];
                    break;
                }
                i++;
            } while (cssColors[i][0] != NULL);

        } else {
            hexString = text;
        }

        int c1 = 0, c2 = 0, c3 = 0, c4 = 255;

        int result = sscanf(hexString + 1, "%02x%02x%02x%02x", &c1, &c2, &c3, &c4);
        if (result < 3) { // See if this is a #xxx string
            result = sscanf(hexString + 1, "%01x%01x%01x%01x", &c1, &c2, &c3, &c4);
            if (result < 3 ) { // it is not, bad color. Reset the defaults.
                r = 0;
                g = 0;
                b = 0;
                a = 255;
            } else { // it is. We need to duplicate each color definition, ie #abc should become #aabbcc
                c1 = (c1 << 4) + c1;
                c2 = (c2 << 4) + c2;
                c3 = (c3 << 4) + c3;

                if (result == 4)
                    c4 = (c4 << 4) + c4;
            }
        }

        r = c1;
        g = c2;
        b = c3;
        a = c4;
    }

    inline bool operator==(const Color& c) const
    {
        return r == c.r && g == c.g && b == c.b && a == c.a;
    }

    inline bool operator!=(const Color& c) const
    {
        return !(*this == c);
    }

    void normalized( float & nr , float & ng , float & nb , float & na ) const
    {
        nr = r / 255.0f;
        ng = g / 255.0f;
        nb = b / 255.0f;
        na = a / 255.0f;
    }

    void normalized( float * n ) const
    {
        assert( n );

        *n++ = r / 255.0f;
        *n++ = g / 255.0f;
        *n++ = b / 255.0f;
        *n   = a / 255.0f;
    }

    inline bool empty() const
    {
        return r == 0 && g == 0 && b == 0 && a == 0;
    }

    inline bool operator!() const
    {
        return empty();
    }

    inline std::string toString(bool verbose = true) const
    {
        std::stringstream out;
        if (verbose) {
            out << "["
                << std::hex
                << std::setiosflags( std::ios_base::uppercase )
                << std::setfill('0')
                << std::setw(2) << (unsigned) r
                << std::setw(2) << (unsigned) g
                << std::setw(2) << (unsigned) b
                << std::setw(2) << (unsigned) a
                << "]";
        } else {
            out << (int)r << ":" << (int)g << ":" << (int)b << ":" << (int)a;
        }
        return out.str();
    }

    Variant toVariant() const;

    // This order is relied upon by Surface::Data::pixel/setPixel
    unsigned char r, g, b, a;
};

inline std::ostream &operator<< (std::ostream &out, const Color &c)
{
    return out << c.toString();
}

inline Serializer &operator<<(Serializer &serializer, const Color &c)
{
    serializer << c.r << c.g << c.b << c.a;
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, Color &c)
{
    deserializer >> c.r >> c.g >> c.b >> c.a;
    return deserializer;
}

inline Variant Color::toVariant() const
{
    Variant data;
    data["r"] = r;
    data["g"] = g;
    data["b"] = b;
    data["a"] = a;
    return data;
}

}} // namespace netflix::gibbon

#endif // COLOR_H
