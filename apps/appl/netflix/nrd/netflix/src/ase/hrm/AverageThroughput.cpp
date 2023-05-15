/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AverageThroughput.h"
#include <nrdbase/tr1.h>
#include <assert.h>
#include <algorithm>

using namespace std;
using namespace netflix;
using namespace netflix::ase;

AverageThroughput::AverageThroughput(	AseTimeVal  halfLife,
                                        AseTimeVal  interval,
                                        ullong      initialValue,
                                        bool        excisePauses,
                                        bool        fastWarmup )
    : mHalfLife( halfLife )
    , mInterval( interval )
    , mPreviousValue( initialValue )
    , mBytesInCurrentInterval( 0 )
    , mState( STATE_INITIAL )
    , mExcisePauses( excisePauses )
    , mFastWarmup( fastWarmup )
{
    if ( mInterval < AseTimeVal::fromMS( 50 ) )
    {
        mInterval = AseTimeVal::fromMS( 50 );
    }

    setHalfLife( halfLife );
}

void AverageThroughput::setHalfLife( AseTimeVal halfLife )
{
    //(alpha^^(halfLife/interval)) = 0.5
    //alpha = 0.5 ^^ ( interval/halfLife )
    mHalfLife = halfLife;
    mAlpha = pow( (double) 0.5, (double) mInterval.val() / halfLife.val() );

    // pow( 0.5, 0.0001 ) = 0.9999306876841536 corresponds to a half life 10000 times the interval
    if ( mAlpha > 0.9999306876841536 )
        mAlpha = 0.9999306876841536;
}

void AverageThroughput::bytesReceived( ullong count, AseTimeVal const& when,
                                        bool drainingSocketReceiveBuffer  )
{
    switch( mState )
    {
        case STATE_INITIAL:
            mStartTime = when;
            mEndOfCurrentInterval = when + mInterval;
            mBytesInCurrentInterval = drainingSocketReceiveBuffer ? 0 : count;
            mState = STATE_NORMAL;
            break;

        case STATE_PAUSED:
            if ( mExcisePauses )
            {
                AseTimeVal const pause = ( when - mPauseTime );
                mEndOfCurrentInterval += pause;
                mStartTime += pause;
            }
            mState = STATE_NORMAL;

            // Fall-through

        case STATE_NORMAL:
            if ( drainingSocketReceiveBuffer )
                bytesReceivedResuming( count, when );
            else
                bytesReceivedNormal( count, when );
            break;
    }
}

void AverageThroughput::bytesReceivedEarlier(   ullong count,
                                                AseTimeVal const& then,
                                                AseTimeVal const& now )
{
    switch( mState )
    {
        case STATE_INITIAL:
            {
                int n = ( now - then + mInterval - AseTimeVal::MICROSECOND ) / mInterval;

                if ( n > 0 )
                {
                    ullong const rate = count * 8000 / ( now - then ).ms();
                    double const alphan = pow( mAlpha, n );
                    mPreviousValue = (ullong)(alphan * mPreviousValue + ( 1.0 - alphan ) * rate);
                }
            }

            mStartTime = now;
            mEndOfCurrentInterval = now + mInterval;
            mBytesInCurrentInterval = 0;
            mState = STATE_NORMAL;

            break;


        case STATE_PAUSED:
            if ( mExcisePauses )
            {
                mEndOfCurrentInterval += ( now - mPauseTime );
            }
            mState = STATE_NORMAL;

        case STATE_NORMAL:
            {
                if ( now >= mEndOfCurrentInterval )
                    bytesReceivedNormal( 0, now );

                AseTimeVal const startOfCurrentInterval = mEndOfCurrentInterval - mInterval;

                if ( then >= startOfCurrentInterval )
                {
                    mBytesInCurrentInterval += count;
                }
                else
                {
                    ullong const current = count * ( now - startOfCurrentInterval ).ms() / ( now - then ).ms();
                    mBytesInCurrentInterval += current;

                    int n = ( startOfCurrentInterval - then + mInterval - AseTimeVal::MICROSECOND ) / mInterval;
                    if ( n > 0 )
                    {
                        ullong const rate = count * 8000 / ( now - then ).ms();
                        mPreviousValue += (ullong)(( 1.0 - pow( mAlpha, n ) ) * rate);
                    }
                }
            }
    }
}

void AverageThroughput::bytesReceivedNormal( ullong count, AseTimeVal const& when )
{
    if ( when >= mEndOfCurrentInterval )
    {
        mPreviousValue = calculate( when );
        mEndOfCurrentInterval += mInterval;
        mPreviousValue = advance( when, mEndOfCurrentInterval, mPreviousValue );
        mBytesInCurrentInterval = 0;
    }

    mBytesInCurrentInterval += count;
}

void AverageThroughput::bytesReceivedResuming( ullong count, AseTimeVal const& /*when*/ )
{
    // If we not excising the paused time, assign these bytes to the interval when we paused
    // otherwise ignore them
    if ( !mExcisePauses )
    {
        mBytesInCurrentInterval += count;
    }

    return;
}

ullong AverageThroughput::advance( AseTimeVal const& when, AseTimeVal& endOfInterval, ullong value ) const
{
    if ( when > endOfInterval )
    {
        double const alpha = calculateAlpha( when );
        int const steps = 1 + ( when - endOfInterval ) / mInterval;
        endOfInterval += mInterval * steps;
        value = (ullong)(pow( alpha, (double) steps ) * value);
    }

    return value;
}

ullong AverageThroughput::calculate( AseTimeVal const& now ) const
{
    double const alpha = calculateAlpha( now );

    return (ullong)( alpha * mPreviousValue
            + ( 1.0 - alpha ) * mBytesInCurrentInterval * 8000 / mInterval.ms() );
}

double AverageThroughput::calculateAlpha( AseTimeVal const& now ) const
{
    return ( mFastWarmup && ( now - mStartTime < mHalfLife * 2 ) )
            ? pow( (double) 0.5, (double) mInterval.val() / ( ( now - mStartTime ) / 2 ).val() )
            : mAlpha;
}

void AverageThroughput::readingPaused( AseTimeVal const& when )
{
    switch( mState )
    {
        case STATE_INITIAL:
            mEndOfCurrentInterval = when + mInterval;
            // drop though

        case STATE_NORMAL:
            mState = STATE_PAUSED;
            bytesReceivedNormal( 0, when );
            mPauseTime = when;
            break;

        case STATE_PAUSED:
            break;
    }
}

ullong AverageThroughput::getThroughput( AseTimeVal const& now ) const
{
    if ( mState == STATE_INITIAL )
        return mPreviousValue;

    bool const backDate = ((mState == STATE_PAUSED ) && (mExcisePauses));

    AseTimeVal endOfInterval = mEndOfCurrentInterval;
    return advance( backDate ? mPauseTime : now, endOfInterval, calculate( now ) );
}
