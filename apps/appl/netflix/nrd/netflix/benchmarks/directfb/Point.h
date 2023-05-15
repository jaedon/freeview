/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef POINT_H
#define POINT_H

namespace netflix {

class Point
{
public:
    Point() : x(0), y(0) {}
    Point(int ix, int iy) : x(ix), y(iy) {}
    ~Point() {}

    bool operator==(const Point& p) const
    {
        return x == p.x && y == p.y;
    }

    bool operator!=(const Point& p) const
    {
        return x != p.x || y != p.y;
    }

    int x, y;
};

} // netflix

#endif // POINT_H
