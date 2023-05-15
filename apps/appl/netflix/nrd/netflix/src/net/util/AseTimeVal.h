/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ASE_TIME_VAL_H
#define ASE_TIME_VAL_H

#include <nrdbase/StdInt.h>

namespace netflix {

/**
 * The AseTimeVal class enables the storage and manipulation of AseTimeVal info.
 *
 * All AseTimeVal is stored as microseconds. With the AseTimeVal class you can add, subtract and compare AseTimeVal
 * values.
 *
 * AseTimeVal is unsigned, so subtracting two AseTimeVal values that would go -ve results in the value
 * being clamped at 0.
 */
class AseTimeVal
{
public:
    /**
     * @param val Initial time value, in microseconds. This is a 64-bit value.
     */
    AseTimeVal(ullong val = 0) : time(val) {}

    /**
     * Constructs a AseTimeVal object from individual values of days, hours, minutes, seconds and microseconds.
     *
     * @param days
     * @param hours
     * @param minutes
     * @param seconds
     * @param milliseconds
     * @param microseconds
     */
    AseTimeVal(
        ullong days,
        ullong hours,
        ullong minutes,
        ullong seconds,
        ullong milliseconds,
        ullong microseconds );

    /**
     *
     * @return Returns the value of the object, in microseconds, as a 64 bit number.
     */
    ullong val() const {return time;}

    /**
     *
     * @return Returns the whole number of seconds in the object.
     * @example AseTimeVal t4(1, 2, 3, 4, 5);
     *
     * t4.seconds() returns 93784
     */
    ullong seconds() const
    {
        return time / MicroSecondsPerSecond;
    }

    ullong ms() const { return time / MicroSecondsPerMillisecond; }
    ullong us() const { return time; }

    static AseTimeVal now();

    // More expressive creators
    static AseTimeVal fromSeconds(int secs) { return fromUS( secs * MicroSecondsPerSecond ); }
    static AseTimeVal fromMS(ullong ms) { return fromUS( ms * MicroSecondsPerMillisecond ); }
    static AseTimeVal fromUS(ullong us) { return AseTimeVal(us); }

    /**
     * Returns the number of hours in the AseTimeVal object given that
     * the AseTimeVal has been reduced to calender time.
     * E.g., if the AseTimeVal object is expressible as HH:MM:SS YYYY
     * then this would return HH.
     */
    uint32_t reducedHours() const
    {
        return (time / MicroSecondsPerHour) % 24;
    }

    /**
     * Returns the number of minutes in the AseTimeVal object given that
     * the AseTimeVal has been reduced to calender time.
     * E.g., if the AseTimeVal object is expressible as HH:MM:SS YYYY
     * then this would return MM.
     */
    uint32_t reducedMinutes() const
    {
        return (time / MicroSecondsPerMinute) % 60;
    }

    AseTimeVal &operator+=(const AseTimeVal &rhs)
    {
        if ((*this != AseTimeVal::INFINITE) && (rhs != AseTimeVal::INFINITE))
        {
            time += rhs.time;
        }
        else
        {
            *this = AseTimeVal::INFINITE;
        }

        return *this;
    }
    AseTimeVal &operator-=(const AseTimeVal &rhs)
    {
        if ((*this != AseTimeVal::INFINITE) && (rhs != AseTimeVal::INFINITE))
        {
            time -= (rhs.time > time) ? time : rhs.time; // clamp to 0
        }
        else if ((*this == AseTimeVal::INFINITE) && (rhs != AseTimeVal::INFINITE))
        {
            *this = AseTimeVal::INFINITE;
        }
        else
        {
            time = 0ULL;
        }

        return *this;
    }
    AseTimeVal &operator*=(const AseTimeVal &rhs)
    {
        if ((*this != AseTimeVal::INFINITE) && (rhs != AseTimeVal::INFINITE))
        {
            time *= rhs.time;
        }
        else
        {
            *this = AseTimeVal::INFINITE;
        }

        return *this;
    }
    AseTimeVal &operator/=(const AseTimeVal &rhs)
    {
        if ((*this != AseTimeVal::INFINITE) && (rhs != AseTimeVal::INFINITE))
        {
            time /= rhs.time;
        }
        else if ((*this == AseTimeVal::INFINITE) && (rhs != AseTimeVal::INFINITE))
        {
            *this = AseTimeVal::INFINITE;
        }
        else if ((*this == AseTimeVal::INFINITE) && (rhs == AseTimeVal::INFINITE))
        {
            time = 1ULL;
        }
        else
        {
            time = 0ULL;
        }

        return *this;
    }

    bool operator<(const AseTimeVal &rhs) const     { return time < rhs.time; }
    bool operator>(const AseTimeVal &rhs) const     { return time > rhs.time; }
    bool operator<=(const AseTimeVal &rhs) const    { return time <= rhs.time; }
    bool operator>=(const AseTimeVal &rhs) const    { return time >= rhs.time; }
    bool operator==(const AseTimeVal &rhs) const    { return time == rhs.time; }
    bool operator!=(const AseTimeVal &rhs) const    { return time != rhs.time; }

    /* conversion to/from other time classes */
    template<class _Time> _Time convertTo() const;
    template<class _Time> static AseTimeVal convertFrom( _Time const& other );

    static const ullong NanoSecondsPerMillisecond = (1000 * 1000);
    static const ullong MicroSecondsPerMillisecond = 1000;
    static const ullong MicroSecondsPerSecond = (1000 * 1000);
    static const ullong MilliSecondsPerSecond = 1000;
    static const ullong MicroSecondsPerMinute = (60 * MicroSecondsPerSecond);
    static const ullong MicroSecondsPerHour = (60 * MicroSecondsPerMinute);
    static const ullong MicroSecondsPerDay = (24 * MicroSecondsPerHour);

    static AseTimeVal const DAY;
    static AseTimeVal const HOUR;
    static AseTimeVal const MINUTE;
    static AseTimeVal const SECOND;
    static AseTimeVal const MILLISECOND;
    static AseTimeVal const MICROSECOND;

    static AseTimeVal const ZERO;
    static AseTimeVal const INFINITE;

private:
    ullong time; // us
};

inline AseTimeVal operator+(const AseTimeVal &lhs, const AseTimeVal &rhs)
{
    return AseTimeVal(lhs).operator+=(rhs);
}
inline AseTimeVal operator-(const AseTimeVal &lhs, const AseTimeVal &rhs)
{
    return AseTimeVal(lhs).operator-=(rhs);
}
inline AseTimeVal operator*(const AseTimeVal &lhs, const ullong rhs)
{
    return AseTimeVal(lhs).operator*=(rhs);
}
inline AseTimeVal operator/(const AseTimeVal &lhs, const ullong rhs)
{
    return AseTimeVal(lhs).operator/=(rhs);
}
inline ullong operator/( AseTimeVal const& lhs, AseTimeVal const& rhs )
{
    return AseTimeVal(lhs).operator/=(rhs).val();
}

template<class _Time>
_Time AseTimeVal::convertTo() const
{
    if ( time == 0 ) return _Time();

    ullong const localnow = AseTimeVal::now().ms();
    ullong const othernow = _Time::now().ms();

    if ( localnow > othernow )
    {
        ullong const offset_ms = ( localnow - othernow );
        return _Time::fromMS( ( ms() > offset_ms ) ? ( ms() - offset_ms ) : 0 );
    }
    else
    {
        ullong const offset_ms = ( othernow - localnow );
        return _Time::fromMS( ms() + offset_ms );
    }
}

template<class _Time>
AseTimeVal AseTimeVal::convertFrom( _Time const& other )
{
    if ( other == _Time() ) return AseTimeVal::ZERO;

    ullong const localnow = AseTimeVal::now().ms();
    ullong const othernow = _Time::now().ms();

    if ( localnow > othernow )
    {
        ullong const offset_ms = ( localnow - othernow );
        return AseTimeVal::fromMS( other.ms() + offset_ms );
    }
    else
    {
        ullong const offset_ms = ( othernow - localnow );
        return AseTimeVal::fromMS( ( other.ms() > offset_ms ) ? ( other.ms() - offset_ms ) : 0 );
    }
}

}
#endif //ASE_TIME_VAL_H
