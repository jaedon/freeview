/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <Rect.h>

#include <string.h>

namespace netflix {
namespace gibbon {

class Transform
{
public:
    enum Type {
        Type_Translate = 0x01,
        Type_Scale     = 0x02,
        Type_Rotate    = 0x04,
        Type_3D        = 0x10,
    };

    inline Transform() : mType(0) {
        memset(mMatrix, 0, sizeof(mMatrix));
        mMatrix[0][0] = 1.0f;
        mMatrix[1][1] = 1.0f;
        mMatrix[2][2] = 1.0f;
        mMatrix[3][3] = 1.0f;
    }
    inline Transform(const Transform &transform) { copy(transform); }

    inline uint8_t type() const { return mType; }
    inline bool isIdentity() const { return !mType; }
    inline bool is3D() const { return mType & Type_3D; }

    Transform invert(bool *ok=0);
    static Transform multiply(const Transform &transform1, const Transform &transform2);

    inline Point map(const Point &point) const {
        if(isIdentity())
            return point;
        const Coordinate xin = point.x, yin = point.y;
        const float x = xin * mMatrix[0][0] + yin * mMatrix[1][0] + mMatrix[3][0],
                    y = xin * mMatrix[0][1] + yin * mMatrix[1][1] + mMatrix[3][1],
                    w = xin * mMatrix[0][3] + yin * mMatrix[1][3] + mMatrix[3][3];
        if (w == 1.0f)
            return Point(GIBBON_ROUND(x), GIBBON_ROUND(y));
        return Point(GIBBON_ROUND(x / w), GIBBON_ROUND(y / w));
    }

    inline Point3D map(const Point3D &point) const {
        if(isIdentity())
            return point;
        const Coordinate xin = point.x, yin = point.y, zin = point.z;
        const float x = xin * mMatrix[0][0] + yin * mMatrix[1][0] + zin * mMatrix[2][0] + mMatrix[3][0],
                    y = xin * mMatrix[0][1] + yin * mMatrix[1][1] + zin * mMatrix[2][1] + mMatrix[3][1],
                    z = xin * mMatrix[0][2] + yin * mMatrix[1][2] + zin * mMatrix[2][2] + mMatrix[3][2],
                    w = xin * mMatrix[0][3] + yin * mMatrix[1][3] + zin * mMatrix[2][3] + mMatrix[3][3];
        if (w == 1.0f)
            return Point3D(x, y, z);
        return Point3D(GIBBON_ROUND(x / w), GIBBON_ROUND(y / w), GIBBON_ROUND(z / w));
    }

    inline Rect map(const Rect &rect) const {
        if(isIdentity())
            return rect;
        const Point tl(map(Point(rect.x, rect.y))), tr(map(Point(rect.x+rect.width, rect.y))),
                    bl(map(Point(rect.x, rect.y+rect.height))), br(map(Point(rect.x+rect.width, rect.y+rect.height)));
        const Coordinate xmin = std::min(std::min(tl.x, tr.x), std::min(bl.x, br.x)),
                         xmax = std::max(std::max(tl.x, tr.x), std::max(bl.x, br.x)),
                         ymin = std::min(std::min(tl.y, tr.y), std::min(bl.y, br.y)),
                         ymax = std::max(std::max(tl.y, tr.y), std::max(bl.y, br.y));
        return Rect(xmin, ymin, xmax-xmin, ymax-ymin);
    }

    inline std::string toString() const
    {
        std::stringstream out;
        if(isIdentity()) {
            out << "IDENTITY";
        } else {
            if(is3D())
                out << "3D:";
            if(mType & Type_Scale)
                out << "SCALE:";
            if(mType & Type_Rotate)
                out << "ROTATE:";
            if(mType & Type_Translate)
                out << "TRANSLATE:";
            for(int x = 0; x < 4; ++x) {
                out << "[";
                for(int y = 0; y < 4; ++y) {
                    if(y)
                        out << " ";
                    out << mMatrix[x][y];
                }
                out << "]" << std::endl;
            }
        }
        return out.str();
    }

    inline const float *matrix() const { return (float*)mMatrix; }
    inline float matrix(int x, int y) const { return mMatrix[x][y]; }
    inline void setMatrix(int x, int y, float v) { mMatrix[x][y] = v; }

    inline bool operator==(const Transform &rhs) const
    {
        return compare(rhs);
    }
    inline bool operator!=(const Transform &rhs) const
    {
        return !compare(rhs);
    }

    inline Transform &operator=(const Transform &rhs)
    {
        if(!isIdentity() || !rhs.isIdentity())
            copy(rhs);
        return *this;
    }

    inline Transform &operator*=(const Transform &rhs)
    {
        *this = multiply(*this, rhs);
        return *this;
    }

    inline bool compare(const Transform &rhs) const {
        if(mType != rhs.mType)
            return false;
        for(int x = 0; x < 4; ++x) {
            for(int y = 0; y < 4; ++y) {
                if(!gibbon_float_compare(mMatrix[x][y], rhs.mMatrix[x][y]))
                    return false;
            }
        }
        return true;
    }

    inline void copy(const Transform &rhs) {
        mType = rhs.mType;
        memcpy(mMatrix, rhs.mMatrix, sizeof(mMatrix));
    }

    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);
    void rotate(float angle, float x, float y, float z);

    void frustum(float left, float right, float bottom, float top, float nearZ, float farZ);
    void perspective(float fovy, float aspect, float zNear, float zFar);
    void ortho(float left, float right, float bottom, float top, float nearZ, float farZ);

private:
    uint8_t mType;
    float mMatrix[4][4];
};

inline std::ostream &operator<< (std::ostream &out, const Transform &t)
{
    return out << t.toString();
}

inline Transform operator*(const Transform &lhs, const Transform &rhs)
{
    return Transform::multiply(lhs, rhs);
}

}} // namespace netflix::gibbon

#endif /* __TRANSFORM_H__ */
