/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Time.h>
#include "AseTimeVal.h"

namespace netflix {


AseTimeVal::AseTimeVal(
            ullong days,
            ullong hours,
            ullong minutes,
            ullong seconds,
            ullong milliseconds,
            ullong microseconds )
{
    time = (days * MicroSecondsPerDay) +
           (hours * MicroSecondsPerHour) +
           (minutes * MicroSecondsPerMinute) +
           (seconds * MicroSecondsPerSecond) +
           (milliseconds * MicroSecondsPerMillisecond) +
           microseconds;
}

AseTimeVal AseTimeVal::now()
{
    return AseTimeVal( Time::mono().val() * MicroSecondsPerMillisecond );
}

AseTimeVal const AseTimeVal::DAY(MicroSecondsPerDay);
AseTimeVal const AseTimeVal::HOUR(MicroSecondsPerHour);
AseTimeVal const AseTimeVal::MINUTE(MicroSecondsPerMinute);
AseTimeVal const AseTimeVal::SECOND(MicroSecondsPerSecond);
AseTimeVal const AseTimeVal::MILLISECOND(MicroSecondsPerMillisecond);
AseTimeVal const AseTimeVal::MICROSECOND(1);

AseTimeVal const AseTimeVal::ZERO( 0ULL );
AseTimeVal const AseTimeVal::INFINITE( 0xFFFFFFFFFFFFFFFFULL );

}

