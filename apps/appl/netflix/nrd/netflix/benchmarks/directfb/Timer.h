/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <sys/time.h>

namespace netflix {

class Timer
{
public:
    Timer() {}
    ~Timer() {}

    void start();
    unsigned elapsed();

    static unsigned ticks();

private:

    struct timeval mTime;
};


} // netflix

#endif // TIMER_H