/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Timer.h>
#include <iostream>

using namespace std;


namespace netflix {

unsigned Timer::ticks()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}



void Timer::start()
{
    gettimeofday(&mTime, NULL);
}


unsigned Timer::elapsed()
{
    struct timeval now;
    struct timeval* start = &mTime;

    gettimeofday(&now, NULL);

    long int diff = (now.tv_usec + 1000000 * now.tv_sec) - (start->tv_usec + 1000000 * start->tv_sec);

    return diff < 0 ? 0 : diff / 1000;
}

} // netflix


