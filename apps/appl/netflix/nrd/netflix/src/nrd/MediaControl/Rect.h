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

#include <nrdbase/StdInt.h>

namespace netflix {

/**
 * @class Rect Rect.h
 * @brief A data structure which describes a rectangular area.
 */
struct Rect
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};

} // namespace netflix

#endif // RECT_H
