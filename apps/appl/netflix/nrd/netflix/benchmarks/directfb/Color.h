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

namespace netflix {

class Color
{
public:
    Color() : r(0), g(0), b(0), a(255) {}
    Color(unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia = 255) :
            r(ir), g(ig), b(ib), a(ia) {}
    ~Color() {}

    bool operator==(const Color& c) const
    {
        return r == c.r && g == c.g && b == c.b && a == c.a;
    }

    bool operator!=(const Color& c) const
    {
        return r != c.r || g != c.g || b != c.b || a != c.a;
    }

    unsigned char r, g, b, a;
};

} // netflix

#endif // COLOR_H
