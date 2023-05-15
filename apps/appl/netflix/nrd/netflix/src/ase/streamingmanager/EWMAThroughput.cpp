/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "EWMAThroughput.h"

#include <cstdio>

using namespace std;
using namespace netflix::ase;


EWMAThroughput::EWMAThroughput(uint32_t maxTcpRecvBuffer)
            : mSampleWindow( AseTimeVal::fromMS(100) ),
              mId( 0 ),
              mThroughput( 3000000 ),
              mSampleStartTime( AseTimeVal::ZERO ),
              mSampleEndTime( AseTimeVal::ZERO ),
              mTotalBytes(0),
              mInited( false ),
              mPaused( false ),
              mPauseStartTime( AseTimeVal::ZERO ),
              mMaxTcpRecvBuffer(maxTcpRecvBuffer),
              mSkippedBytes(0),
              mIgnoreDataInBuffer(false)
{
}

/** Set ID. */
void EWMAThroughput::setId( uint32_t id )
{
    mId = id;
}

/** Set the initial throughput. */
void EWMAThroughput::setInitialThroughput( ullong initialThroughput )
{
    mThroughput = initialThroughput >> 10;
}

void EWMAThroughput::bytesReceived( ullong bytes )
{
    AseTimeVal now = AseTimeVal::now();

    do
    {
        if (!mInited)
        {
            // ignore the first block
            mSampleStartTime = now;

            mInited = true;

            break;
        }

        if (mPaused)
        {
            mSampleStartTime = now - (mPauseStartTime - mSampleStartTime);

            mPaused = false;
        }

        if ( mIgnoreDataInBuffer && (mSkippedBytes < mMaxTcpRecvBuffer) )
        {
            // ignore the data in the tcp receive buffer when resuming downloading
            mSkippedBytes += bytes;

            break;
        }

        if (now > (mSampleStartTime + mSampleWindow))
        {
            // wrap up a sample
            ullong mSampleThroughput = mTotalBytes * 8ULL / mSampleWindow.ms();

            mThroughput = (ullong) ((1.0 - EWMA_THROUGHPUT_ALPHA) * (double) mThroughput + EWMA_THROUGHPUT_ALPHA * (double) mSampleThroughput);

            AseTimeVal emptyPeriod = now - (mSampleStartTime + mSampleWindow);

            while (emptyPeriod > mSampleWindow)
            {
                mThroughput = (ullong) ((1.0 - EWMA_THROUGHPUT_ALPHA) * (double) mThroughput);

                emptyPeriod = emptyPeriod - mSampleWindow;
            }

            mTotalBytes = bytes;

            mSampleStartTime = now;
        }
        else
        {
            mSampleEndTime = now;

            mTotalBytes += bytes;
        }

    } while (false);

    return;
}
	
void EWMAThroughput::readingPaused( bool ignoreBufferAtResume )
{
    AseTimeVal now = AseTimeVal::now();
    mSkippedBytes = 0;
    mIgnoreDataInBuffer = ignoreBufferAtResume;
    mPaused = true;
    mPauseStartTime = now;

    return;
}
	
ullong EWMAThroughput::getThroughput()
{
    return mThroughput << 10;
}

