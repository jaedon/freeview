/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Time_h
#define Time_h

#include <iostream>
#include <time.h>
#include <nrdbase/StdInt.h>
#include <climits>
#include <nrdbase/Exportable.h>

namespace netflix {

static const ullong NanoSecondsPerMillisecond = 1000 * 1000;
static const ullong MicroSecondsPerMillisecond = 1000;
static const ullong MicroSecondsPerSecond = 1000 * 1000;
static const ullong MilliSecondsPerSecond = 1000;
static const ullong MilliSecondsPerMinute = 60 * MilliSecondsPerSecond;
static const ullong MilliSecondsPerHour = 60 * MilliSecondsPerMinute;
static const ullong MilliSecondsPerDay = 24 * MilliSecondsPerHour;

class TimeMultiplier {
public:
    static void setMultiplier(float multiplier);
    static inline float multiplier() { return sMultiplier; }
    template <typename T> static inline T multiply(T ms)
    {
        if (sMultiplier != 1.0f) {
            const ullong delta = (ullong)(ms - sStarted);
            return sStarted + (T)(delta * sMultiplier);
        }
        return ms;
    }
private:
    static ullong sStarted;
    static float sMultiplier;
};

/**
 * The Time class enables the storage and manipulation of Time info.
 *
 * All Time is stored as milliseconds. With the Time class you can add, subtract and compare Time
 * values.
 *
 * Time is unsigned, so subtracting two Time values that would go -ve results in the value
 * being clamped at 0.
 */
class NRDP_EXPORTABLE Time
{
public:
    /**
     * @param val Initial time value, in milliseconds. This is a 64-bit value.
     */
    Time(ullong val = 0) : mMs(val) {}

    /**
     * Constructs a Time object from individual values of days, hours, minutes, seconds and milliseconds.
     *
     * @param days
     * @param hours
     * @param minutes
     * @param seconds
     * @param milliseconds
     */
    Time(ullong days, ullong hours, ullong minutes, ullong seconds, ullong milliseconds);

    /**
     *
     * @return Returns the value of the object, in milliseconds, as a 64 bit number.
     */
    ullong val() const { return mMs; }

    /**
     *
     * @return Returns the whole number of seconds in the object.
     * @example Time t4(1, 2, 3, 4, 5);
     *
     * t4.seconds() returns 93784
     */
    ullong seconds() const
    {
        return mMs / MilliSecondsPerSecond;
    }

    ullong ms() const { return mMs; }

    static llong systemTimeOffset();
    static void setSystemTimeOffset(llong offset);
    static void setSystemTime(const Time &time);
    static void setServerTime(const Time &time);
    static inline Time now() { return serverTime(); }
    static Time serverTime();
    static Time systemTime(); // do not use. System time can not be relied upon on many of our platforms
    static bool hasServerTime();
    static Time mono();
    static ullong monoMS();

    // More expressive creators
    static Time fromSeconds(ullong secs) { return fromMS(secs*MilliSecondsPerSecond); }
    static Time fromMS(ullong ms) { return Time(ms); }
    static Time fromRFC1123(const std::string &t);

    /**
     * Returns the number of hours in the Time object given that
     * the Time has been reduced to calender time.
     * E.g., if the Time object is expressible as HH:MM:SS YYYY
     * then this would return HH.
     */
    uint32_t reducedHours() const
    {
        return (mMs / MilliSecondsPerHour) % 24;
    }

    /**
     * Returns the number of minutes in the Time object given that
     * the Time has been reduced to calender time.
     * E.g., if the Time object is expressible as HH:MM:SS YYYY
     * then this would return MM.
     */
    uint32_t reducedMinutes() const
    {
        return (mMs / MilliSecondsPerMinute) % 60;
    }


    std::string rfc1123() const;

    /**
     * "Wed Jun 30 21:49:08 1993\n"
     *
     * @return
     */
    std::string calendarTimeText() const;

    /**
     * Output the Time object as a string, in a long format.
     *
     * @return
     */
    std::string longText() const;

    /**
     * Output a 64bit millisecond value as a string, in a long format.
     *
     * @param val
     *
     * @return
     */
    static std::string longText(ullong val)
    {
        return Time(val).longText();
    }
    /**
     * Output the Time object as a string, in a short format.
     *
     * @return
     */
    std::string text() const;

    /**
     * Output a 64bit millisecond value as a string, in a short format.
     *
     * @param val
     *
     * @return
     */
    static std::string text(ullong val)
    {
        return Time(val).text();
    }

    Time &operator+=(const Time &rhs)
    {
        mMs += rhs.mMs;
        return *this;
    }
    Time &operator-=(const Time &rhs)
    {
        mMs -= (rhs.mMs > mMs) ? mMs : rhs.mMs; // clamp to 0
        return *this;
    }
    Time &operator*=(const ullong rhs)
    {
        mMs *= rhs;
        return *this;
    }
    Time &operator/=(const ullong rhs)
    {
        mMs /= rhs;
        return *this;
    }

    bool operator<(const Time &rhs) const     { return mMs < rhs.mMs; }
    bool operator>(const Time &rhs) const     { return mMs > rhs.mMs; }
    bool operator<=(const Time &rhs) const    { return mMs <= rhs.mMs; }
    bool operator>=(const Time &rhs) const    { return mMs >= rhs.mMs; }
    bool operator==(const Time &rhs) const    { return mMs == rhs.mMs; }
    bool operator!=(const Time &rhs) const    { return mMs != rhs.mMs; }

    static Time Day() { return Time(MilliSecondsPerDay); }
    static Time Hour() { return Time(MilliSecondsPerHour); }
    static Time Minute() { return Time(MilliSecondsPerMinute); }
    static Time Second() { return Time(MilliSecondsPerSecond); }
private:
    ullong mMs;
};

// Wrapper to allow us to write AseTimeVal::convertTo<TimeMono>
struct TimeMono : public Time
{
    TimeMono() : Time() {}
    TimeMono( Time const& other ) : Time( other ) {}

    static Time now() { return mono(); }
};

inline Time operator+(const Time &lhs, const Time &rhs)
{
    return Time(lhs).operator+=(rhs);
}

inline Time operator-(const Time &lhs, const Time &rhs)
{
    return Time(lhs).operator-=(rhs);
}

inline Time operator*(const Time &lhs, const ullong rhs)
{
    return lhs.ms() * rhs;
}

inline Time operator*(const ullong lhs, const Time &rhs)
{
    return lhs * rhs.ms();
}

inline Time operator/(const Time &lhs, const ullong rhs)
{
    return Time(lhs).operator/=(rhs);
}

inline std::ostream& operator<<(std::ostream& stream, Time const& obj)
{
    stream << obj.val();
    return stream;
}

inline std::istream& operator>>(std::istream& stream, Time& obj)
{
    ullong val;
    stream >> val;
    obj = val;
    return stream;
}

} // namespace netflix

#endif
