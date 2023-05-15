/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GIBBON_RECT_H
#define GIBBON_RECT_H

#include <Point.h>
#include <Size.h>

#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <nrdbase/StdInt.h>
#include <stdlib.h>

namespace netflix {
namespace gibbon {

class Rect
{
public:
    inline Rect() : x(0), y(0), width(0), height(0) {}
    inline Rect(Coordinate ix, Coordinate iy, Coordinate iw, Coordinate ih) : x(ix), y(iy), width(iw), height(ih) {}
    inline Rect(const Point &origin, const Size &size) : x(origin.x), y(origin.y), width(size.width), height(size.height) {}
    inline Rect(const Point &origin, Coordinate iw, Coordinate ih) : x(origin.x), y(origin.y), width(iw), height(ih) {}
    inline Rect(Coordinate ix, Coordinate iy, const Size &size) : x(ix), y(iy), width(size.width), height(size.height) {}
    inline Rect(const Rect &r) { x = r.x; y = r.y; width = r.width; height = r.height; }
    inline Rect(const Size &s) { x = 0; y = 0; width = s.width; height = s.height; }
    inline Rect &operator=(const Rect &r) { x = r.x; y = r.y; width = r.width; height = r.height; return *this; }
    inline ~Rect() {}

    Coordinate x, y, width, height;

    inline Point topLeft() const { return Point(x, y); }
    inline Point topRight() const { return Point(x, y+height); }
    inline Point bottomLeft() const { return Point(x+width, y); }
    inline Point bottomRight() const { return Point(x+width, y+height); }
    inline Point center() const { return Point(x+(width/2), y+(height/2)); }

    inline Coordinate right() const { return x + width; }
    inline Coordinate bottom() const { return y + height; }
    inline Size size() const { return Size(width, height); }
    inline Point origin() const { return topLeft(); }

    inline void setOrigin(Point point) { x = point.x; y = point.y; }
    inline void setOrigin(Coordinate ix, Coordinate iy) { x = ix; y = iy; }

    inline void left(Coordinate tx) { width += x - tx; x = tx; }
    inline void top(Coordinate ty) { height += y - ty; y = ty; }
    inline void right(Coordinate r) { width = r - x; }
    inline void bottom(Coordinate b) { height = b - y; }

    inline void push(Point point)
    {
        *this += point;
        width += point.x;
        height += point.y;
    }

    inline bool operator==(const Rect &r) const
    {
#ifdef GIBBON_COORDINATE_FLOAT
        return (gibbon_float_compare(x, r.x) && gibbon_float_compare(y, r.y)
                && gibbon_float_compare(width, r.width) && gibbon_float_compare(height, r.height));
#else
        return x == r.x && y == r.y && width == r.width && height == r.height;
#endif
    }

    inline bool operator!=(const Rect &r) const
    {
        return !(*this == r);
    }

    inline Rect& operator+=(const Point &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    inline Rect& operator-=(const Point &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    inline const Rect operator+(const Point &p) const
    {
        return Rect(*this) += p;
    }

    inline const Rect operator-(const Point &p) const
    {
        return Rect(*this) -= p;
    }

    inline Rect& operator+=(const Size &s)
    {
        width += s.width;
        height += s.height;
        return *this;
    }

    inline Rect& operator+=(const Rect& r)
    {
        x += r.x;
        y += r.y;
        width += r.width;
        height += r.height;
        return *this;
    }

    inline Rect& operator-=(const Rect& r)
    {
        x -= r.x;
        y -= r.y;
        width -= r.width;
        height -= r.height;
        return *this;
    }

    inline Rect& operator-=(const Size &s)
    {
        width -= s.width;
        height -= s.height;
        return *this;
    }

    inline Rect operator+(const Size &s) const
    {
        return Rect(*this) += s;
    }

    inline Rect operator-(const Size &s) const
    {
        return Rect(*this) -= s;
    }


    inline void set(Coordinate ix, Coordinate iy, Coordinate iw, Coordinate ih)
    {
        x = ix;
        y = iy;
        width = iw;
        height = ih;
    }

    inline void clear() { x = 0; y = 0; width = 0; height = 0; }

    inline bool intersects(const Rect &r) const { return overlaps(r); }
    inline bool overlaps(const Rect &r) const
    {
        if(empty() || r.empty())
            return false;
        return !(x >= r.right() || y >= r.bottom() || right() <= r.x || bottom() <= r.y);
    }

    inline void unite(const Rect &r)
    {
        *this = united(r);
    }

    inline Rect united(const Rect &r) const
    {
        if (width <=0 && height <= 0)
            return r;
        else if (r.width <= 0 && r.height <= 0)
            return *this;
        Rect uniteRect(x < r.x ? x : r.x, y < r.y ? y : r.y, 0, 0);
        uniteRect.right(right() > r.right() ? right() : r.right());
        uniteRect.bottom(bottom() > r.bottom() ? bottom() : r.bottom());
        return uniteRect;
    }

    inline Rect &scale(float scaleX, float scaleY)
    {
        width = GIBBON_CEIL(width * scaleX);
        height = GIBBON_CEIL(height * scaleY);
        return *this;
    }

    inline Rect scaled(float scaleX, float scaleY) const
    {
        return Rect(x, y, GIBBON_CEIL(width*scaleX), GIBBON_CEIL(height*scaleY));
    }

    inline Rect &operator|=(const Rect &r)
    {
        unite(r);
        return *this;
    }

    inline Rect operator|(const Rect &r) const
    {
        return united(r);
    }

    inline Rect &operator&=(const Rect &r)
    {
        intersect(r);
        return *this;
    }

    inline Rect operator&(const Rect &r) const
    {
        return intersected(r);
    }

    inline bool contains(const Point &p) const
    {
        return (x <= p.x && x+width >= p.x && y <= p.y && y+height >= p.y);
    }

    inline bool contains(const Rect &r) const
    {
        return (x <= r.x && (x + width) >= (r.x + r.width) && y <= r.y && (y + height) >= (r.y + r.height));
    }

    inline Rect intersected(const Rect &r) const
    {
        if (overlaps(r)) {
            Rect sectRect(x > r.x ? x : r.x, y > r.y ? y : r.y, 0, 0);
            sectRect.right(right() < r.right() ? right() : r.right());
            sectRect.bottom(bottom() < r.bottom() ? bottom() : r.bottom());
            return sectRect;
        }
        return Rect();
    }

    inline void intersect(const Rect &r)
    {
        *this = intersected(r);
    }

    inline Rect normalized() const
    {
        Rect result(x, y, width, height);
        if(result.width < 0) {
            result.width = -result.width;
            result.x -= result.width;
        }
        if(result.height < 0) {
            result.height = -result.height;
            result.y -= result.height;
        }
        return result;
    }

    inline void normalize() {
        *this = normalized();
    }

    inline void inflate(const Coordinate amount) {
        x -= amount;
        y -= amount;
        width += (2 * amount);
        height += (2 * amount);
    }

    inline void inflate(const Coordinate amountX, const Coordinate amountY) {
        x -= amountX;
        y -= amountY;
        width += (2 * amountX);
        height += (2 * amountY);
    }

    inline bool empty() const { return width <= 0 || height <= 0; }

    inline std::vector<Rect> subtract(const Rect& other)
    {
        std::vector<Rect> result;

        if (!intersects(other)) {
            result.push_back(*this);
            return result;
        }

        Coordinate savedY = this->y;
        Coordinate originalHeight = this->height;

        Coordinate ar = this->x + this->width;
        Coordinate ab = this->y + this->height;

        Coordinate br = other.x + other.width;
        Coordinate bb = other.y + other.height;

        // Subtract off any area on top where A extends past B
        if (other.y > this->y) {
            Rect r = Rect(this->x, this->y, this->width, other.y - this->y);
            result.push_back(r);
            savedY = other.y;
            // If we're moving the top down, we also need to subtract the height diff.
            originalHeight -= other.y - this->y;
        }

        // Subtract off any area on bottom where A extends past B
        if (bb < ab) {
            Rect r = Rect(this->x, bb, this->width, ab - bb);
            result.push_back(r);
            originalHeight = bb - savedY;
        }

        // Subtract any area on left where A extends past B
        if (other.x > this->x) {
            Rect r = Rect(this->x, savedY, other.x - this->x, originalHeight);
            result.push_back(r);
        }

        // Subtract any area on right where A extends past B
        if (br < ar) {
            Rect r = Rect(br, savedY, ar - br, originalHeight);
            result.push_back(r);
        }

        return result;
    }

    inline std::string toString(bool points=false) const
    {
        std::stringstream out;
        out << "[" << x << "," << y;
        if(points)
            out << "-" << right() << "," << bottom();
        else
            out << "+" << width << "x" << height;
        out << "]";
        return out.str();
    }

    inline Variant toVariant() const
    {
        Variant data;
        data["x"] = x;
        data["y"] = y;
        data["width"] = width;
        data["height"] = height;
        return data;
    }

    static Rect fromString(const std::string& str, bool* ok = 0);

#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    inline DFBRectangle toDFBRect() const
    {
        DFBRectangle ret;
        ret.x = x;
        ret.y = y;
        ret.w = width;
        ret.h = height;
        return ret;
    }
    static inline DFBRectangle toDFBRect(int x, int y, int w, int h)
    {
        DFBRectangle ret;
        ret.x = x;
        ret.y = y;
        ret.w = w;
        ret.h = h;
        return ret;
    }
    static inline DFBRectangle toDFBRect(int w, int h)
    {
        DFBRectangle ret;
        ret.x = 0;
        ret.y = 0;
        ret.w = w;
        ret.h = h;
        return ret;
    }
    inline DFBRegion toDFBRegion() const
    {
        DFBRegion ret;
        ret.x1 = x;
        ret.y1 = y;
        ret.x2 = x + width - 1;
        ret.y2 = y + height - 1;
        return ret;
    }
#endif
};

inline Rect Rect::fromString(const std::string& str, bool* ok)
{
    const char* seps = ",+x ";
    const char* ign = "[]\t";
#ifdef GIBBON_COORDINATE_FLOAT
    const char* leg = "-0123456789.";
#else
    const char* leg = "-0123456789";
#endif
    const char* start = str.c_str();
    const char* end = start + str.size();

    Rect out;
    int curcoord = 0;
    Coordinate* coords[] = { &out.x, &out.y, &out.width, &out.height };

    for (const char* cur = start; cur < end;) {
        if (strchr(ign, *cur)) {
            ++cur;
            continue;
        }
        if (curcoord >= 4) {
            if (ok)
                *ok = false;
            return Rect();
        }
        Coordinate& c = *coords[curcoord];
        if (strchr(leg, *cur)) {
            // try to call strtof/strtol
            char* fin;
#ifdef GIBBON_COORDINATE_FLOAT
            // this api sucks
            if (cur + 1 < end && *cur == '0' && *(cur + 1) == 'x') {
                c = 0;
                fin = const_cast<char*>(cur + 1);
            } else {
                c = strtof(cur, &fin);
            }
#else
            c = strtol(cur, &fin, 10);
#endif
            if (fin == cur) { // bad
                if (ok)
                    *ok = false;
                return Rect();
            }
            cur = fin;
            assert(cur <= end);
            if (cur == end) {
                if (curcoord == 3) {
                    if (ok)
                        *ok = true;
                    return out;
                }
                if (ok)
                    *ok = false;
                return Rect();
            }
            if (strchr(seps, *cur)) {
                ++curcoord;
                ++cur;
                if (curcoord >= 4) {
                    if (ok)
                        *ok = false;
                    return Rect();
                }
            } else if (!strchr(ign, *cur)) {
                if (ok)
                    *ok = false;
                return Rect();
            }
        } else {
            if (ok)
                *ok = false;
            return Rect();
        }
    }
    if (curcoord == 3) {
        if (ok)
            *ok = true;
        return out;
    }
    if (ok)
        *ok = false;
    return Rect();
}

inline std::ostream &operator<<(std::ostream &out, const Rect &r)
{
    return out << r.toString();
}

inline Serializer &operator<<(Serializer &serializer, const Rect &r)
{
    serializeCoordinate(serializer, r.x);
    serializeCoordinate(serializer, r.y);
    serializeCoordinate(serializer, r.width);
    serializeCoordinate(serializer, r.height);
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, Rect &r)
{
    deserializeCoordinate(deserializer, r.x);
    deserializeCoordinate(deserializer, r.y);
    deserializeCoordinate(deserializer, r.width);
    deserializeCoordinate(deserializer, r.height);
    return deserializer;
}

}} // namespace netflix::gibbon

#endif // GIBBON_RECT_H
