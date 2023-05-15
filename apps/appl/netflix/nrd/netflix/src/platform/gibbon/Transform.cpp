/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Transform.h"

#include <stdio.h>

using namespace netflix;
using namespace netflix::gibbon;

Transform Transform::invert(bool *ok)
{
    float det_1;
    float abssum, temp;

#  define PRECISION_LIMIT (1.0e-15)

    /*
      Calculate the determinant of submatrix A and determine if the
      the matrix is singular.
    */
    temp =  mMatrix[0][0] * mMatrix[1][1] * mMatrix[2][2];
    det_1 = temp; abssum = fabsf(temp);
    temp =  mMatrix[1][0] * mMatrix[2][1] * mMatrix[0][2];
    det_1 += temp; abssum += fabsf(temp);
    temp =  mMatrix[2][0] * mMatrix[0][1] * mMatrix[1][2];
    det_1 += temp; abssum += fabsf(temp);
    temp = -mMatrix[2][0] * mMatrix[1][1] * mMatrix[0][2];
    det_1 += temp; abssum += fabsf(temp);
    temp = -mMatrix[1][0] * mMatrix[0][1] * mMatrix[2][2];
    det_1 += temp; abssum += fabsf(temp);
    temp = -mMatrix[0][0] * mMatrix[2][1] * mMatrix[1][2];
    det_1 += temp; abssum += fabsf(temp);

    Transform result;
    /* Is the submatrix A singular? */
    if ((det_1 == 0.0f) || (fabsf(det_1 / abssum) < PRECISION_LIMIT)) { /* Matrix M has no inverse */
        if(ok)
            *ok = false;
        return result;
    }

    /* Calculate inverse(A) = adj(A) / det(A) */
    det_1 = 1.0f / det_1;
    result.mMatrix[0][0] =   (mMatrix[1][1] * mMatrix[2][2] - mMatrix[2][1] * mMatrix[1][2]) * det_1;
    result.mMatrix[1][0] = - (mMatrix[0][1] * mMatrix[2][2] - mMatrix[2][1] * mMatrix[0][2]) * det_1;
    result.mMatrix[2][0] =   (mMatrix[0][1] * mMatrix[1][2] - mMatrix[1][1] * mMatrix[0][2]) * det_1;
    result.mMatrix[0][1] = - (mMatrix[1][0] * mMatrix[2][2] - mMatrix[2][0] * mMatrix[1][2]) * det_1;
    result.mMatrix[1][1] =   (mMatrix[0][0] * mMatrix[2][2] - mMatrix[2][0] * mMatrix[0][2]) * det_1;
    result.mMatrix[2][1] = - (mMatrix[0][0] * mMatrix[1][2] - mMatrix[1][0] * mMatrix[0][2]) * det_1;
    result.mMatrix[0][2] =   (mMatrix[1][0] * mMatrix[2][1] - mMatrix[2][0] * mMatrix[1][1]) * det_1;
    result.mMatrix[1][2] = - (mMatrix[0][0] * mMatrix[2][1] - mMatrix[2][0] * mMatrix[0][1]) * det_1;
    result.mMatrix[2][2] =   (mMatrix[0][0] * mMatrix[1][1] - mMatrix[1][0] * mMatrix[0][1]) * det_1;

    /* Calculate -C * inverse(A) */
    result.mMatrix[3][0] = - (mMatrix[0][3] * result.mMatrix[0][0] +
                             mMatrix[1][3] * result.mMatrix[1][0] +
                             mMatrix[2][3] * result.mMatrix[2][0]);
    result.mMatrix[3][1] = - (mMatrix[0][3] * result.mMatrix[0][1] +
                             mMatrix[1][3] * result.mMatrix[1][1] +
                             mMatrix[2][3] * result.mMatrix[2][1]);
    result.mMatrix[3][2] = - (mMatrix[0][3] * result.mMatrix[0][2] +
                             mMatrix[1][3] * result.mMatrix[1][2] +
                             mMatrix[2][3] * result.mMatrix[2][2]);

    /* Fill in last column */
    result.mMatrix[0][3] = result.mMatrix[1][3] = result.mMatrix[2][3] = 0.0;
    result.mMatrix[3][3] = 1.0;
    result.mType = mType;
    if(ok)
        *ok = true;
    return result;
}

void Transform::translate(float tx, float ty, float tz)
{
    mMatrix[3][0] += (mMatrix[0][0] * tx + mMatrix[1][0] * ty + mMatrix[2][0] * tz);
    mMatrix[3][1] += (mMatrix[0][1] * tx + mMatrix[1][1] * ty + mMatrix[2][1] * tz);
    mMatrix[3][2] += (mMatrix[0][2] * tx + mMatrix[1][2] * ty + mMatrix[2][2] * tz);
    mMatrix[3][3] += (mMatrix[0][3] * tx + mMatrix[1][3] * ty + mMatrix[2][3] * tz);

    mType |= Type_Translate;
    if(!gibbon_float_compare(tz, 1.0f))
        mType |= Type_3D;
}

void Transform::scale(float sx, float sy, float sz)
{
    mMatrix[0][0] *= sx;
    mMatrix[0][1] *= sx;
    mMatrix[0][2] *= sx;
    mMatrix[0][3] *= sx;

    mMatrix[1][0] *= sy;
    mMatrix[1][1] *= sy;
    mMatrix[1][2] *= sy;
    mMatrix[1][3] *= sy;

    mMatrix[2][0] *= sz;
    mMatrix[2][1] *= sz;
    mMatrix[2][2] *= sz;
    mMatrix[2][3] *= sz;

    mType |= Type_Scale;
    if(!gibbon_float_compare(sz, 1.0f))
        mType |= Type_3D;
}

void Transform::rotate(float angle, float x, float y, float z)
{
    const float a = angle * (float)M_PI / 180.0f;
    const float s = sinf(a), c = cosf(a), mag = sqrtf(x * x + y * y + z * z);
    if (mag > 0.0f) {
        const float ic = 1.0f - c;
        x /= mag;
        y /= mag;
        z /= mag;
        Transform rot;
        rot.mMatrix[0][0] = x * x * ic + c;
        rot.mMatrix[1][0] = x * y * ic - z * s;
        rot.mMatrix[2][0] = x * z * ic + y * s;
        rot.mMatrix[3][0] = 0.0f;
        rot.mMatrix[0][1] = y * x * ic + z * s;
        rot.mMatrix[1][1] = y * y * ic + c;
        rot.mMatrix[2][1] = y * z * ic - x * s;
        rot.mMatrix[3][1] = 0.0f;
        rot.mMatrix[0][2] = x * z * ic - y * s;
        rot.mMatrix[1][2] = y * z * ic + x * s;
        rot.mMatrix[2][2] = z * z * ic + c;
        rot.mMatrix[3][2] = 0.0f;
        rot.mMatrix[0][3] = 0.0f;
        rot.mMatrix[1][3] = 0.0f;
        rot.mMatrix[2][3] = 0.0f;
        rot.mMatrix[3][3] = 1.0f;
        rot.mType |= Type_Rotate;
        if(!gibbon_float_compare(z, 1.0f))
            rot.mType |= Type_3D;
        *this *= rot;
    }
}

void Transform::frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    if (left == right || bottom == top || nearPlane == farPlane)
        return;

    const float width = right - left, clip = farPlane - nearPlane;
    const float invheight = top - bottom;

    Transform frustum;
    frustum.mMatrix[0][0] = 2.0f * nearPlane / width;
    frustum.mMatrix[1][0] = 0.0f;
    frustum.mMatrix[2][0] = (left + right) / width;
    frustum.mMatrix[3][0] = 0.0f;
    frustum.mMatrix[0][1] = 0.0f;
    frustum.mMatrix[1][1] = 2.0f * nearPlane / invheight;
    frustum.mMatrix[2][1] = (top + bottom) / invheight;
    frustum.mMatrix[3][1] = 0.0f;
    frustum.mMatrix[0][2] = 0.0f;
    frustum.mMatrix[1][2] = 0.0f;
    frustum.mMatrix[2][2] = -(nearPlane + farPlane) / clip;
    frustum.mMatrix[3][2] = -2.0f * nearPlane * farPlane / clip;
    frustum.mMatrix[0][3] = 0.0f;
    frustum.mMatrix[1][3] = 0.0f;
    frustum.mMatrix[2][3] = -1.0f;
    frustum.mMatrix[3][3] = 0.0f;
    frustum.mType |= Type_3D;
    *this *= frustum;
}

void Transform::perspective(float angle, float aspect, float nearPlane, float farPlane)
{
    if (nearPlane == farPlane || aspect == 0.0f)
        return;

    const float radians = (angle / 2.0f) * M_PI / 180.0f, sine = sinf(radians);
    if (sine == 0.0f)
        return;
    const float cotan = cosf(radians) / sine, clip = farPlane - nearPlane;

    Transform persp;
    persp.mMatrix[0][0] = cotan / aspect;
    persp.mMatrix[1][0] = 0.0f;
    persp.mMatrix[2][0] = 0.0f;
    persp.mMatrix[3][0] = 0.0f;
    persp.mMatrix[0][1] = 0.0f;
    persp.mMatrix[1][1] = cotan;
    persp.mMatrix[2][1] = 0.0f;
    persp.mMatrix[3][1] = 0.0f;
    persp.mMatrix[0][2] = 0.0f;
    persp.mMatrix[1][2] = 0.0f;
    persp.mMatrix[2][2] = -(nearPlane + farPlane) / clip;
    persp.mMatrix[3][2] = -(2.0f * nearPlane * farPlane) / clip;
    persp.mMatrix[0][3] = 0.0f;
    persp.mMatrix[1][3] = 0.0f;
    persp.mMatrix[2][3] = -1.0f;
    persp.mMatrix[3][3] = 0.0f;
    persp.mType |= Type_3D;
    *this *= persp;
}

void Transform::ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    if (left == right || bottom == top || nearPlane == farPlane)
        return;
    const float width = right - left, clip = farPlane - nearPlane;
    const float invheight = top - bottom;

    Transform ortho;
    ortho.mMatrix[0][0] = 2.0f / width;
    ortho.mMatrix[1][0] = 0.0f;
    ortho.mMatrix[2][0] = 0.0f;
    ortho.mMatrix[3][0] = -(left + right) / width;
    ortho.mMatrix[0][1] = 0.0f;
    ortho.mMatrix[1][1] = 2.0f / invheight;
    ortho.mMatrix[2][1] = 0.0f;
    ortho.mMatrix[3][1] = -(top + bottom) / invheight;
    ortho.mMatrix[0][2] = 0.0f;
    ortho.mMatrix[1][2] = 0.0f;
    ortho.mMatrix[2][2] = -2.0f / clip;
    ortho.mMatrix[3][2] = -(nearPlane + farPlane) / clip;
    ortho.mMatrix[0][3] = 0.0f;
    ortho.mMatrix[1][3] = 0.0f;
    ortho.mMatrix[2][3] = 0.0f;
    ortho.mMatrix[3][3] = 1.0f;
    ortho.mType |= Type_3D;
    *this *= ortho;
}

Transform Transform::multiply(const Transform &t1, const Transform &t2)
{
    Transform result;
    result.mMatrix[0][0] = t1.mMatrix[0][0] * t2.mMatrix[0][0] +
                          t1.mMatrix[1][0] * t2.mMatrix[0][1] +
                          t1.mMatrix[2][0] * t2.mMatrix[0][2] +
                          t1.mMatrix[3][0] * t2.mMatrix[0][3];
    result.mMatrix[0][1] = t1.mMatrix[0][1] * t2.mMatrix[0][0] +
                          t1.mMatrix[1][1] * t2.mMatrix[0][1] +
                          t1.mMatrix[2][1] * t2.mMatrix[0][2] +
                          t1.mMatrix[3][1] * t2.mMatrix[0][3];
    result.mMatrix[0][2] = t1.mMatrix[0][2] * t2.mMatrix[0][0] +
                          t1.mMatrix[1][2] * t2.mMatrix[0][1] +
                          t1.mMatrix[2][2] * t2.mMatrix[0][2] +
                          t1.mMatrix[3][2] * t2.mMatrix[0][3];
    result.mMatrix[0][3] = t1.mMatrix[0][3] * t2.mMatrix[0][0] +
                          t1.mMatrix[1][3] * t2.mMatrix[0][1] +
                          t1.mMatrix[2][3] * t2.mMatrix[0][2] +
                          t1.mMatrix[3][3] * t2.mMatrix[0][3];
    result.mMatrix[1][0] = t1.mMatrix[0][0] * t2.mMatrix[1][0] +
                          t1.mMatrix[1][0] * t2.mMatrix[1][1] +
                          t1.mMatrix[2][0] * t2.mMatrix[1][2] +
                          t1.mMatrix[3][0] * t2.mMatrix[1][3];
    result.mMatrix[1][1] = t1.mMatrix[0][1] * t2.mMatrix[1][0] +
                          t1.mMatrix[1][1] * t2.mMatrix[1][1] +
                          t1.mMatrix[2][1] * t2.mMatrix[1][2] +
                          t1.mMatrix[3][1] * t2.mMatrix[1][3];
    result.mMatrix[1][2] = t1.mMatrix[0][2] * t2.mMatrix[1][0] +
                          t1.mMatrix[1][2] * t2.mMatrix[1][1] +
                          t1.mMatrix[2][2] * t2.mMatrix[1][2] +
                          t1.mMatrix[3][2] * t2.mMatrix[1][3];
    result.mMatrix[1][3] = t1.mMatrix[0][3] * t2.mMatrix[1][0] +
                          t1.mMatrix[1][3] * t2.mMatrix[1][1] +
                          t1.mMatrix[2][3] * t2.mMatrix[1][2] +
                          t1.mMatrix[3][3] * t2.mMatrix[1][3];
    result.mMatrix[2][0] = t1.mMatrix[0][0] * t2.mMatrix[2][0] +
                          t1.mMatrix[1][0] * t2.mMatrix[2][1] +
                          t1.mMatrix[2][0] * t2.mMatrix[2][2] +
                          t1.mMatrix[3][0] * t2.mMatrix[2][3];
    result.mMatrix[2][1] = t1.mMatrix[0][1] * t2.mMatrix[2][0] +
                          t1.mMatrix[1][1] * t2.mMatrix[2][1] +
                          t1.mMatrix[2][1] * t2.mMatrix[2][2] +
                          t1.mMatrix[3][1] * t2.mMatrix[2][3];
    result.mMatrix[2][2] = t1.mMatrix[0][2] * t2.mMatrix[2][0] +
                          t1.mMatrix[1][2] * t2.mMatrix[2][1] +
                          t1.mMatrix[2][2] * t2.mMatrix[2][2] +
                          t1.mMatrix[3][2] * t2.mMatrix[2][3];
    result.mMatrix[2][3] = t1.mMatrix[0][3] * t2.mMatrix[2][0] +
                          t1.mMatrix[1][3] * t2.mMatrix[2][1] +
                          t1.mMatrix[2][3] * t2.mMatrix[2][2] +
                          t1.mMatrix[3][3] * t2.mMatrix[2][3];
    result.mMatrix[3][0] = t1.mMatrix[0][0] * t2.mMatrix[3][0] +
                          t1.mMatrix[1][0] * t2.mMatrix[3][1] +
                          t1.mMatrix[2][0] * t2.mMatrix[3][2] +
                          t1.mMatrix[3][0] * t2.mMatrix[3][3];
    result.mMatrix[3][1] = t1.mMatrix[0][1] * t2.mMatrix[3][0] +
                          t1.mMatrix[1][1] * t2.mMatrix[3][1] +
                          t1.mMatrix[2][1] * t2.mMatrix[3][2] +
                          t1.mMatrix[3][1] * t2.mMatrix[3][3];
    result.mMatrix[3][2] = t1.mMatrix[0][2] * t2.mMatrix[3][0] +
                          t1.mMatrix[1][2] * t2.mMatrix[3][1] +
                          t1.mMatrix[2][2] * t2.mMatrix[3][2] +
                          t1.mMatrix[3][2] * t2.mMatrix[3][3];
    result.mMatrix[3][3] = t1.mMatrix[0][3] * t2.mMatrix[3][0] +
                          t1.mMatrix[1][3] * t2.mMatrix[3][1] +
                          t1.mMatrix[2][3] * t2.mMatrix[3][2] +
                          t1.mMatrix[3][3] * t2.mMatrix[3][3];
    result.mType = t1.mType | t2.mType;
    return result;
}
