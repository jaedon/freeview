/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Time.h"

#include <assert.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

#include <nrdbase/Log.h>
#include <nrdbase/config.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>

using namespace netflix;

static Mutex sMutex(TIME_MUTEX, "Time");
static bool sHasTimeOffset = false;
static llong sSystemTimeOffset = 0;
static llong sTimeOffset = 0;

Time::Time(ullong days, ullong hours, ullong minutes, ullong seconds, ullong milliseconds)
{
    mMs = (days * MilliSecondsPerDay) +
           (hours * MilliSecondsPerHour) +
           (minutes * MilliSecondsPerMinute) +
           (seconds * MilliSecondsPerSecond) +
           milliseconds;
}

static inline ullong mono()
{
#ifdef __APPLE__
    mach_timebase_info_data_t timebaseInfo;
    mach_timebase_info(&timebaseInfo);
    const ullong result = ((mach_absolute_time() * timebaseInfo.numer) / (1000 * 1000 * timebaseInfo.denom));
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    const ullong result = ((ts.tv_sec * MilliSecondsPerSecond) + (ts.tv_nsec / NanoSecondsPerMillisecond));
#endif
    return result;
}

ullong Time::monoMS()
{
    return TimeMultiplier::multiply(::mono());
}

Time Time::mono()
{
    return Time::fromMS(Time::monoMS());
}

std::string Time::rfc1123() const
{
    char result[64];
    time_t const sec = static_cast<time_t>(seconds());
    strftime(result, sizeof(result), "%a, %d %b %Y %H:%M:%S %Z", localtime(&sec));
    return result;
}

std::string Time::calendarTimeText() const
{
    time_t const sec = static_cast<time_t>(seconds());
    char buf[26];

    ctime_r(&sec, buf);

    return std::string(buf, 24);
}

std::string Time::longText() const
{
    char buf[128];
    snprintf(buf, sizeof buf,
        "%llu days, %llu hours, %llu minutes, %llu seconds, %llu milliseconds",
        mMs / MilliSecondsPerDay,
        (mMs / MilliSecondsPerHour) % 24,
        (mMs / MilliSecondsPerMinute) % 60,
        (mMs / MilliSecondsPerSecond) % 60,
        mMs % 1000);
    return buf;
}

std::string Time::text() const
{
    char buf[64];
    snprintf(buf, sizeof buf,
        "%llu:%llu:%llu:%llu:%llu",
        mMs / MilliSecondsPerDay,
        (mMs / MilliSecondsPerHour) % 24,
        (mMs / MilliSecondsPerMinute) % 60,
        (mMs / MilliSecondsPerSecond) % 60,
        mMs % 1000);
    return buf;
}

void Time::setServerTime(const Time &time)
{
    const ullong cur = ::mono();
    const ullong ms = time.ms();
    ScopedMutex lock(sMutex);
    sHasTimeOffset = true;
    sTimeOffset = ms - cur;
}

bool Time::hasServerTime()
{
    ScopedMutex lock(sMutex);
    return sHasTimeOffset;
}

static int month_number_to_month_name(const char *s) {
    static const char *month_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", 0
    };
    for(int i = 0; month_names[i]; i++) {
        if(!strcmp(s, month_names[i]))
            return i;
    }
    return -1;
}

static int offsetFromTZ(const char* tz, int len, bool* ok = 0)
{
    if (!tz || !len) {
        if (ok)
            *ok = false;
        return 0;
    }
    if (tz[0] == '+' || tz[0] == '-') {
        char* end;
        long int val = strtol(tz + 1, &end, 10);
        if (end != tz + 5) {
            if (ok)
                *ok = false;
            return 0;
        }
        if (ok)
            *ok = true;
        const int hrs = val / 100;
        const int mins = val % 100;
        val = ((hrs * 3600) + (mins * 60)) * ((tz[0] == '+') ? 1 : -1);
        return val;
    }
    if (ok)
        *ok = true;
    if ((len == 3 && (!strncmp(tz, "GMT", 3)))
        || (len == 2 && !strncmp(tz, "UT", 2)))
        return 0;
    if (len != 3) {
        if (ok)
            *ok = false;
        return 0;
    }
    if (!strncmp(tz, "EDT", 3))
        return -4 * 3600;
    if (!strncmp(tz, "EST", 3)
        || !strncmp(tz, "CDT", 3))
        return -5 * 3600;
    if (!strncmp(tz, "CST", 3)
        || !strncmp(tz, "MDT", 3))
        return -6 * 3600;
    if (!strncmp(tz, "MST", 3)
        || !strncmp(tz, "PDT", 3))
        return -7 * 3600;
    if (!strncmp(tz, "PST", 3))
        return -8 * 3600;
    if (ok)
        *ok = false;
    return 0;
}

static int calculateUTCOffset()
{
    time_t localTime = time(0);
    tm localt;
    localtime_r(&localTime, &localt);

    // Get the difference between this time zone and UTC on the 1st of January of this year.
    localt.tm_sec = 0;
    localt.tm_min = 0;
    localt.tm_hour = 0;
    localt.tm_mday = 1;
    localt.tm_mon = 0;
    // Not setting localt.tm_year!
    localt.tm_wday = 0;
    localt.tm_yday = 0;
    localt.tm_isdst = 0;
#ifdef NRDP_HAS_TM_GMTOFF
    localt.tm_gmtoff = 0;
#endif
#ifdef NRDP_HAS_TM_ZONE
    localt.tm_zone = 0;
#endif

#ifdef NRDP_HAS_TIMEGM
    time_t utcOffset = timegm(&localt) - mktime(&localt);
#else
    // Using a canned date of 01/01/2009 on platforms with weaker date-handling foo.
    localt.tm_year = 109;
    time_t utcOffset = 1230768000 - mktime(&localt);
#endif

    return static_cast<int>(utcOffset);
}

Time Time::fromRFC1123(const std::string &_dateStr)
{
    struct tm tm;
    (void) memset(&tm, 0, sizeof(tm));
    int sec = 0, min = 0, hour = 0, mday = 0, month = 0, year = 0, tzb = 0, tze = 0;

    char mon[32];
    const char *dateStr = _dateStr.c_str();
    if (((sscanf(dateStr, "%d/%3s/%d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) == 6) ||
         (sscanf(dateStr, "%d %3s %d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) == 6) ||
         (sscanf(dateStr, "%*3s, %d %3s %d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) == 6) ||
         (sscanf(dateStr, "%*3s, %d-%3s-%d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) == 6) ||
         (sscanf(dateStr, "%d-%3s-%d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) == 6)) &&
        (month = month_number_to_month_name(mon)) != -1) {
        tm.tm_mday = mday;
        tm.tm_mon = month;
        tm.tm_year = year;
        tm.tm_hour = hour;
        tm.tm_min = min;
        tm.tm_sec = sec;

        if (tm.tm_year > 1900)
            tm.tm_year -= 1900;
        else if (tm.tm_year < 70)
            tm.tm_year += 100;

        bool ok;
        const int oft = offsetFromTZ(dateStr + tzb, tze - tzb, &ok);
        if (ok) {
            const int system_clock_offset = calculateUTCOffset();
            const llong epoch = (llong) mktime(&tm); // coercing time_t into int64_t so that we can print it
            const llong seconds = epoch + oft + system_clock_offset;
            if(epoch == -1 || seconds < 0) {
                Log::error(TRACE_SYSTEM, "Bad RFC1123 date: %s (%lld, %lld, %d, %d)", dateStr, seconds, epoch, oft, system_clock_offset);
                // return the beginning of the epoch time, which will be treated as expired in
                // NetworkManager::CookieManager::processCookie(...)
                return Time();
            }
            return Time::fromSeconds(seconds);
        }
    }
    return Time();
}

static inline ullong systemTime()
{
    struct timeval tv;
    ::gettimeofday(&tv, 0);
    ScopedMutex lock(sMutex);
    return (tv.tv_sec * MilliSecondsPerSecond) + (tv.tv_usec / MicroSecondsPerMillisecond) + sSystemTimeOffset;
}

void Time::setSystemTime(const Time &time)
{
    ScopedMutex lock(sMutex);
    sSystemTimeOffset = 0;
    const llong cur = ::systemTime();
    sSystemTimeOffset = static_cast<llong>(time.val()) - cur;
}

void Time::setSystemTimeOffset(llong offset)
{
    ScopedMutex lock(sMutex);
    sSystemTimeOffset = offset;
}

llong Time::systemTimeOffset()
{
    ScopedMutex lock(sMutex);
    return sSystemTimeOffset;
}

Time Time::serverTime()
{
    llong value = 0;
    bool offset = false;
    {
        ScopedMutex lock(sMutex);
        if (sHasTimeOffset) {
            value = sTimeOffset;
            offset = true;
        }
    }
    if (!offset) {
        value = ::systemTime();
    } else {
        value += ::mono();
    }
    return std::max<ullong>(value, 1262307661000ll); // Fri, 01 Jan 2010 01:01:01 GMT
}

Time Time::systemTime()
{
    return ::systemTime();
}

ullong TimeMultiplier::sStarted(false);
float TimeMultiplier::sMultiplier = 1.0;
void TimeMultiplier::setMultiplier(float multiplier)
{
    sMultiplier = 1.0f;
    sStarted = Time::monoMS();
    sMultiplier = multiplier;
}

