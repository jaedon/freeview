/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef RECT_H
#define RECT_H

#include <ostream>

namespace netflix {

class Rect
{
public:
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(int ix, int iy, int iw, int ih) : x(ix), y(iy), width(iw), height(ih) {}
    Rect(const Rect& r) { x = r.x; y = r.y; width = r.width; height = r.height; }
    Rect& operator=(const Rect& r) { x = r.x; y = r.y; width = r.width; height = r.height; return *this; }
    ~Rect() {}

    int x, y, width, height;

    inline int right() const { return x + width; }
    inline int bottom() const { return y + height; }

    inline void left(int tx) { width += x - tx; x = tx; }
    inline void top(int ty) { height += y - ty; y = ty; }
    inline void right(int r) { width = r - x; }
    inline void bottom(int b) { height = b - y; }

    bool operator==(const Rect& r) const
    {
        return x == r.x && y == r.y && width == r.width && height == r.height;
    }

    bool operator!=(const Rect& r) const
    {
        return x != r.x || y != r.y || width != r.width || height != r.height;
    }

    inline void set(int ix, int iy, int iw, int ih)
    {
        x = ix;
        y = iy;
        width = iw;
        height = ih;
    }

    void clear() { x = 0; y = 0; width = 0; height = 0; }

    bool overlaps(const Rect& r) const
    {
        return !(x > r.right() || y > r.bottom() || right() < r.x || bottom() < r.y);
    }

    void unite(const Rect& r)
    {
        if (x > r.x) left(r.x);
        if (y > r.y) top(r.y);
        if (right() < r.right()) right(r.right());
        if (bottom() < r.bottom()) bottom(r.bottom());
    }

    Rect intersect(const Rect& r) const
    {
        if (overlaps(r))
        {
            Rect sectRect(x > r.x ? x : r.x, y > r.y ? y : r.y, 0, 0);
            sectRect.right(right() < r.right() ? right() : r.right());
            sectRect.bottom(bottom() < r.bottom() ? bottom() : r.bottom());
            return sectRect;
        }

        return Rect();
    }

    bool empty() const
    {
        return width <= 0 || height <= 0;
    }

    friend std::ostream& operator<< (std::ostream& out, const Rect& r);
};

} // netflix

#endif // RECT_H
