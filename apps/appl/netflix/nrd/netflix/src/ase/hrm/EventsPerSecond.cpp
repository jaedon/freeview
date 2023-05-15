/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifdef BUILD_DEBUG

#include <nrdbase/Log.h>
#include "EventsPerSecond.h"

using namespace netflix;


void EventsPerSecond::log( Time const now )
{
    uint32_t const ms = ( now - mStart ).ms();
    NTRACE( TRACE_THREAD, "%s: %d events-per-second in %ums",
        mIdentifier,
        mCount * 1000 / ms,
        ms );
    mStart = now;
    mCount = 0;
}
#endif
