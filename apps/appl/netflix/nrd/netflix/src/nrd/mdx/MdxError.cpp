/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxError.h"

const char *netflix::mdx::toString(MdxErrorCode error)
{
    switch(error)
    {
#define DEFINE_ERROR(errorName, errorValue)         \
        case errorValue:                            \
        {                                           \
            static const char *name = #errorName;   \
            return name;                            \
        }

#include "MdxErrorInternal.h"

#undef DEFINE_ERROR
    default:
        break;
    }

    return "";
}