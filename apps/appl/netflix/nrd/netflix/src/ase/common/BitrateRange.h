/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __BITRATE_RANGE_H
#define __BITRATE_RANGE_H

#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase {
struct BitrateRange
{
    uint32_t mMinVideoBitrate;
    uint32_t mMaxVideoBitrate;
    netflix::ContentProfile mContentProfile;
};

}} // namespace netflix::mediacontrol

#endif // __BITRATE_RANGE_H
