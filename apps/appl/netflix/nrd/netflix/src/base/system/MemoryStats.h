/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEMORYSTATS_H
#define MEMORYSTATS_H

#include <string>

#include <nrdbase/Variant.h>

namespace netflix {
namespace util {

namespace MemoryStats
{
    // it returns a raw, platform-specific dump of the data that
    // is used to calculate a call to collect. In Linux implementations,
    // this returns the contents of /proc/[pid]/smaps.
    Variant collectRaw(int pid=-1);
}

}} // netflix::util

#endif

