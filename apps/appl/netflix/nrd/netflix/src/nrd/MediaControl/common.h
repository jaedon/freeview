/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIACONTROL_COMMON_H
#define MEDIACONTROL_COMMON_H

//==============================================================================
// Disclaimer:
//
// 1. Within the MediaControl and its sub-modules, all time values are
//    in milliseconds unless stated otherwise.
//
//==============================================================================

namespace netflix {
namespace mediacontrol {

// shared constants

static const int32_t  MAX_INT32            = 0x7FFFFFFF;
static const uint32_t MAX_UINT32           = 0xFFFFFFFF;
static const uint32_t INVALID_STREAM_INDEX = MAX_UINT32;
static const uint32_t INVALID_PTS          = MAX_UINT32;
static const uint32_t INVALID_DURATION     = MAX_UINT32;
static const uint32_t MAX_PTS              = MAX_UINT32 - 1;
}} // namespace netflix::mediacontrol

#endif // MEDIACONTROL_COMMON_H
