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

#include <sstream>
#include <string>
#include <nrdbase/StdInt.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Serialize.h>
#include <limits.h>
#include <float.h>
#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
#include <directfb.h>
#endif

namespace netflix {
namespace gibbon {

#ifdef GIBBON_COORDINATE_FLOAT
typedef float Coordinate;
const Coordinate CoordinateMax = FLT_MAX;
#define GIBBON_ROUND(val) (isnormal(val) ? val : 0.f)
#define GIBBON_FLOOR(val) (isnormal(val) ? val : 0.f)
#define GIBBON_CEIL(val) (isnormal(val) ? val : 0.f)
#define GIBBON_ABS(val) fabs(isnormal(val) ? val : 0.f)
#else
typedef int32_t Coordinate;
const Coordinate CoordinateMax = INT_MAX;
#define GIBBON_ROUND(val) static_cast<Coordinate>(round(isnormal(val) ? val : 0.f))
#define GIBBON_FLOOR(val) static_cast<Coordinate>(floor(isnormal(val) ? val : 0.f))
#define GIBBON_CEIL(val) static_cast<Coordinate>(ceil(isnormal(val) ? val : 0.f))
#define GIBBON_ABS(val) abs(isnormal(val) ? val : 0.f)
#endif


const float Epsilon = 0.00001f;
static inline bool gibbon_float_compare(float l, float r) { return fabs(l - r) <= (Epsilon * std::max(fabs(l), fabs(r))); }
static inline bool gibbon_float_is_zero(float f) { return fabs(f) <= Epsilon; }

template<typename T>
inline bool gibbon_coordinate_greaterThan(T l, T r)
{
    return l > r;
}
template<>
inline bool gibbon_coordinate_greaterThan<float>(float l, float r)
{
    return (l - r) > (Epsilon * l);
}

class Point
{
public:
    inline Point() : x(0), y(0) {}
    inline Point(Coordinate ix, Coordinate iy) : x(ix), y(iy) {}
    inline ~Point() {}

    inline bool operator==(const Point& p) const
    {
#ifdef GIBBON_COORDINATE_FLOAT
        return (gibbon_float_compare(x, p.x) && gibbon_float_compare(y, p.y));
#else
        return x == p.x && y == p.y;
#endif
    }

    inline void clear()
    {
        x = 0;
        y = 0;
    }

    inline bool empty() const
    {
        return (x != 0 && y != 0);
    }

    inline bool operator!=(const Point& p) const
    {
        return !(*this == p);
    }

    inline Point& operator+=(const Point &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    inline Point& operator-=(const Point &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    inline const Point operator+(const Point &p) const
    {
        return Point(*this) += p;
    }

    inline const Point operator-(const Point &p) const
    {
        return Point(*this) -= p;
    }

    inline std::string toString() const
    {
        std::stringstream out;
        out << "[" << x << "," << y << "]";
        return out.str();
    }

    inline Variant toVariant() const
    {
        Variant data;
        data["x"] = x;
        data["y"] = y;
        return data;
    }

#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    DFBPoint toDFBPoint() const
    {
        DFBPoint ret;
        ret.x = x;
        ret.y = y;
        return ret;
    }
#endif

    Coordinate x, y;
};

inline std::ostream &operator<< (std::ostream &out, const Point &p)
{
    return out << p.toString();
}

class Point3D
{
public:
    inline Point3D() : x(0), y(0), z(0) {}
    inline Point3D(const Point &point) : x(point.x), y(point.y), z(0) {}
    inline Point3D(Coordinate ix, Coordinate iy, Coordinate iz=0) : x(ix), y(iy), z(iz) {}
    inline ~Point3D() {}

    inline bool operator==(const Point3D& p) const
    {
#ifdef GIBBON_COORDINATE_FLOAT
        return (gibbon_float_compare(x, p.x) && gibbon_float_compare(y, p.y) && gibbon_float_compare(z, p.z));
#else
        return x == p.x && y == p.y && z == p.z;
#endif
    }

    inline bool operator!=(const Point3D& p) const
    {
        return !(*this == p);
    }

    inline Point3D& operator+=(const Point3D &p)
    {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }

    inline Point3D& operator-=(const Point3D &p)
    {
        x -= p.x;
        y -= p.y;
        z -= p.z;
        return *this;
    }

    inline const Point3D operator+(const Point3D &p) const
    {
        return Point3D(*this) += p;
    }

    inline const Point3D operator-(const Point3D &p) const
    {
        return Point3D(*this) -= p;
    }

    inline std::string toString() const
    {
        std::stringstream out;
        out << "[" << x << "," << y << "," << z << "]";
        return out.str();
    }
    Coordinate x, y, z;
};

inline std::ostream &operator<<(std::ostream &out, const Point3D &p)
{
    return out << p.toString();
}

inline static void serializeCoordinate(Serializer &serializer, const Coordinate &coord)
{
    const float flt = coord;
    serializer << flt;
}

inline static void deserializeCoordinate(Deserializer &deserializer, Coordinate &coord)
{
    float flt;
    deserializer >> flt;
    coord = flt;
}

}} // namespace netflix::gibbon

#endif // POINT_H
