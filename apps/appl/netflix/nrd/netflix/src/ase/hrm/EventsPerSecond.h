/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Time.h>

#ifndef EVENTS_PER_SECOND_H
#define EVENTS_PER_SECOND_H

namespace netflix {

/** The EventsPerSecond class counts and logs events-per-second in DEBUG builds */
class EventsPerSecond
{
public:
    EventsPerSecond( const char* identifier );

    /** Indicate that an event happened */
    void count();

protected:
    void log( Time const now );

protected:
#ifdef BUILD_DEBUG
    const char*     mIdentifier;
    Time            mStart;
    int             mCount;
#endif
};

#ifdef BUILD_DEBUG
inline EventsPerSecond::EventsPerSecond( const char* identifier )
    : mIdentifier( identifier )
    , mStart( Time::mono() )
    , mCount( 0 )
{
}

inline void EventsPerSecond::count()
{
    ++mCount;
    Time const now = Time::mono();
    if ( ( now - mStart ) > Time::fromMS( 1000 ) )
    {
        log( now );
    }
}
#else
inline EventsPerSecond::EventsPerSecond( const char* /* identifier */ )
{
}

inline void EventsPerSecond::count()
{
}
#endif

}

#endif
