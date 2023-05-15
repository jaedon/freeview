/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef Stopwatch_h
#define Stopwatch_h

#include <ostream>
#include <list>

#include <nrdbase/Time.h>
#include <nrdbase/Exportable.h>

#include <sys/time.h>

#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

namespace netflix {

/*
 * The Stopwatch class lets you time stuff. It keeps
 * track of lap times and total elapsed time.
 */

class NRDP_EXPORTABLE Stopwatch
{
public:

    typedef double ms;

    typedef std::list<ms> LapTimes;

    /*
     * Creates a new Stopwatch with the start time set to now (mono).
     */

    Stopwatch(bool begin=true)
    {
        if(begin) {
            start();
        } else {
            mStart = mLap = mPaused = 0;
            mStopped = true;
        }
    }

    /*
     * Sets the start time to now. Clears all laps and restarts the
     * Stopwatch. Returns elapsed() as it was before it was reset.
     */

    inline ms restart()
    {
        ms result = elapsed();
        start();
        return result;
    }

    inline void start()
    {
        mStart = mLap = Stopwatch::mono();
        mLaps.clear();
        mStopped = false;
        mPaused = 0;
    }


    inline bool isStopped() const { return mStopped; }
    inline bool isPaused() const { return mPaused; }

    /*
     * Pauses the stopwatch, so that it is no longer tracking time.
     * The number of pauses must be matched with the number of resumes.
     */

    inline void pause()
    {
        if (!mPaused++)
        {
            mLap = Stopwatch::mono();
        }
    }

    /*
     * Resumes tracking of time.
     */

    inline void resume()
    {
        if (!mPaused)
        {
            return;
        }
        if (!--mPaused)
        {
            mStart += Stopwatch::mono() - mLap;
        }
    }

    /*
     * Stops the Stopwatch. From this point on, elapsed will be the time
     * between when it was started and when it was stopped. After it is
     * stopped, no more laps are accepted. Returns elapsed().
     * If the Stopwatch was paused, it is resumed before it stops.
     */

    inline ms stop()
    {
        if ( mPaused )
        {
            mPaused = 1;
            resume();
        }
        if ( ! mStopped )
        {
            mLap = Stopwatch::mono();
            mStopped = true;
        }
        return elapsed();
    }

    /*
     * Returns and stores the time (in ms) since the start of the last lap,
     * or the start of the stop watch (if there are no laps).
     * It also starts a new lap.
     * If the Stopwatch is stopped or paused, it does not start a new lap and instead
     * returns zero.
     * If final is true, the Stopwatch is stopped after this lap time is
     * recorded.
     */

    inline ms lap( bool final = false )
    {
        if ( mStopped || mPaused )
        {
            return 0;
        }

        ms now( Stopwatch::mono() );
        ms result = now - mLap;
        mLaps.push_back( result );
        mLap = now;
        mStopped = final;
        return result;
    }

    /*
     * Returns the elapsed time (in ms) since the Stopwatch was started
     * regardless of laps. If the Stopwatch was stopped, it returns the
     * time between when it started and when it was stopped.
     */

    inline ms elapsed() const
    {
        return ( ( mStopped || mPaused ) ? mLap : Stopwatch::mono() ) - mStart;
    }

    /*
     * Returns the time (in ms) when the Stopwatch was started.
     */

    inline ms started() const
    {
        return mStart;
    }


    /*
     * Returns the average lap time (in ms) or zero if there are no laps.
     * It can also optionally return the total time for all laps.
     */

    inline double average( ms * total = 0 ) const
    {
        if ( mLaps.empty() )
        {
            if ( total )
            {
                *total = 0;
            }
            return 0;
        }

        ms t = 0;

        for ( LapTimes::const_iterator it = mLaps.begin(); it != mLaps.end(); ++it )
        {
            t += *it;
        }

        if ( total )
        {
            *total = t;
        }

        return t / (double) mLaps.size();
    }

    /*
     * Returns a reference to a list of lap times (in ms).
     */

    const LapTimes & laps() const
    {
        return mLaps;
    }

    /*
     * Returns current monotonic time including fractional milliseconds.
     */

    static inline ms mono()
    {
#ifdef __APPLE__
        mach_timebase_info_data_t timebaseInfo;
        mach_timebase_info(&timebaseInfo);
        const ms result((double(mach_absolute_time()) * timebaseInfo.numer) / (1000000.0f * timebaseInfo.denom));
#else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        const ms result((double(ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000.0f ));
#endif
        return TimeMultiplier::multiply(result);
    }

private:
    ms          mStart;
    ms          mLap;
    bool        mStopped;
    unsigned    mPaused;
    LapTimes    mLaps;
};

inline std::ostream & operator << ( std::ostream & stream , const Stopwatch & sw )
{
    const Stopwatch::LapTimes & laps( sw.laps() );

    if ( laps.empty() )
    {
        stream << sw.elapsed() << " ms";
    }
    else
    {
        Stopwatch::ms total = 0;

        double average = sw.average( & total );

        stream
            << total << " ms"
            << " : " << laps.size() << " cycles"
            << " : " << average << " ms average"
            << " : " << 1000.0f / ( total / laps.size() ) << " Hz"
            << " :";

        for ( Stopwatch::LapTimes::const_iterator it = laps.begin(); it != laps.end(); ++it )
        {
            stream << " " << *it;
        }
    }

    return stream;
}

} // namespace netflix


#endif /* Stopwatch_h */
