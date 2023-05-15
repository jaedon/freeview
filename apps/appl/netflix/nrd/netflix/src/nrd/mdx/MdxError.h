/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MDXERROR_H__
#define __MDXERROR_H__

namespace netflix
{
namespace mdx
{
    enum MdxErrorCode
    {
#define DEFINE_ERROR(error, value) error = value,
        #include "MdxErrorInternal.h"
        MdxError_Last
#undef DEFINE_ERROR
    };

    const char *toString(MdxErrorCode error);

}} // namespace netflix::mdx

#endif // __MDXERROR_H__
