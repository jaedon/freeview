/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AVERAGE_THROUGHPUT_H
#define AVERAGE_THROUGHPUT_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdnet/AseTimeVal.h>

namespace netflix {
namespace ase
{

class AverageThroughput
{
public:
    enum State
    {
        STATE_INITIAL = 0,
        STATE_NORMAL,
        STATE_PAUSED
    };

public:
    /** Default constructor. */
    AverageThroughput()
    {
    }

    /** Constructor
    *  @param halfLife [in] The half-life of the EWMA
    *  @param interval [in] The time interval for measurements
    *  @param socketBufferSize [in] The size of the socket buffer
     *  @param intialValue [in] Initial throughput value
     *  @param excisePauses [in] If true, througput does not decay during pauses
    */
    AverageThroughput(  AseTimeVal halfLife,
                        AseTimeVal interval,
                        ullong          initialValue = 0,
                        bool            excisePauses = true,
                        bool            fastWarmup = false );


    AseTimeVal getHalfLife() const { return mHalfLife; }

    /** Change the half-life */
    void setHalfLife( AseTimeVal halfLife );

    /** Indicate bytes received
    *  @param count [in] The number of bytes received
    *  @param when [in] The time at which they bytes were read from the socket buffer
    *  @param drainingSocketReceiveBuffer [in] true if the caller believe the bytes were
    *                                          received some large unknown time before when
    *                                          because it has just resumed reading from the
    *                                          socket buffer
    */
    void bytesReceived( ullong count,
                        AseTimeVal const& when,
                        bool drainingSocketReceiveBuffer );


    /** Indicate bytes received in the past
    *  @param count [in] The number of bytes received
    *  @param then [in] The earliest time at which the bytes could have been received
    *  @param now [in] The latest time at which the bytes could have been received
    */
    void bytesReceivedEarlier(  ullong count,
                                AseTimeVal const& then,
                                AseTimeVal const& now );


    /** Indicate that reading from the socket has been paused
    *  @param when [in] The time at which reading was paused
    */
    void readingPaused( AseTimeVal const& when );

    /** Get the current throughput in bits per second
    *  @param now [in] The current time. Must be greater than or equal to the
    *                    time of the last bytesReceived call
    */
    ullong getThroughput( AseTimeVal const& now ) const;

    /** Get the state. */
    State getState() const
    {
        return mState;
    }

protected:
    void bytesReceivedNormal( ullong count, AseTimeVal const& when );

    void bytesReceivedResuming( ullong count, AseTimeVal const& when );

    ullong advance( AseTimeVal const& when, AseTimeVal& endOfInterval, ullong value ) const;

    ullong calculate( AseTimeVal const& now ) const;

    double calculateAlpha( AseTimeVal const& now ) const;

protected:

    AseTimeVal     mHalfLife;
    double              mAlpha;
    AseTimeVal     mInterval;
    ullong              mPreviousValue;
    ullong              mBytesInCurrentInterval;
    AseTimeVal     mEndOfCurrentInterval;
    State               mState;
    AseTimeVal     mPauseTime;
    bool                mExcisePauses;
    bool                mFastWarmup;

    AseTimeVal     mStartTime;
};

}} // namespace ase

#endif
