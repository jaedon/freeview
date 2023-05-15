/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/HttpAttempt.h>
#include <nrdase/DebugLogging.h>

using namespace netflix;
using namespace netflix::ase;

DEFINE_LOGGER( HttpAttempt );

void BwEntry::add( AseTimeVal const& now, uint32_t bytes )
{
    assert( mTinterval >= 1000 );

    if ( mStart == AseTimeVal::ZERO )
    {
        mStart = now;
        mStartMono = Time::mono();
    }

    if ( mEnd < now )
        mEnd = now;

    if( now >= mStart && bytes > 0 )
    {
        AseTimeVal elapsed = now - mStart;
        if ( mTinterval > 0 )
        {
            unsigned int bucket = elapsed.ms() / mTinterval;
            if ( (unsigned int) bucket >= mTrace.size() )
            {
                mTrace.resize( bucket + 1 );
            }

            mTrace.back() += bytes;
        }
    }
}

void BwEntry::stop( AseTimeVal const& now )
{
    assert( mTinterval >= 1000 );

    mClosed = true;

    if ( mStart == AseTimeVal::ZERO )
    {
        mStart = now;
        mStartMono = Time::mono();
    }

    if ( mEnd < now )
        mEnd = now;

    if (!mTrace.empty())
    {
        AseTimeVal const elapsed = mEnd - mStart;
        if ( mTinterval > 0 )
        {
            unsigned int const bucket = elapsed.ms() / mTinterval;
            if ( (unsigned int) bucket >= mTrace.size() )
            {
                mTrace.push_back( 0 );
            }
        }
    }
}

void BwEntry::setInterval( unsigned int interval )
{
    assert( interval >= 1000 );

    mTinterval = interval;
}

void HttpAttempt::add( AseTimeVal const& now,
                       uint32_t bytes,
                       bool header,
                       bool bComplete )
{
/*    DEBUGLOG( HttpAttempt, "Add %u bytes @%llums, %sheader, %scomplete",
                            bytes,
                            now.ms(),
                            ( header ? "" : "not " ),
                            ( bComplete ? "" : "not " ) );

*/
    assert( mTinterval >= 1000 );

    if ( mTresp == AseTimeVal::ZERO )
    {
        mTresp = now;
        mTtrace = now;
    }

    if ( mTrace.empty() )
    {
        mTrace.push_back( BwEntry( mTinterval ) );
    }
    else if ( mTrace.back().mClosed )
    {
        if ( now > mTrace.back().mEnd )
        {
            if ( ( now.ms() - mTrace.back().mEnd.ms() ) < 10 )
            {
                // Ignore short pauses
                mTrace.back().mClosed = false;
            }
            else
            {
                mTrace.push_back( BwEntry( mTinterval ) );
            }
        }
    }

    mTrace.back().add( now, header ? 0 : bytes );

    if ( header )
    {
        mHrecv += bytes;
    }
    else
    {
        mBrecv += bytes;
    }

    if ( bComplete )
    {
        complete( now );
    }
}

void HttpAttempt::stop( AseTimeVal const& now )
{
    assert( mTinterval >= 1000 );

    if ( mTrace.empty() || mTrace.back().mClosed )
        return;

    DEBUGLOG( HttpAttempt, "Stopping reception @%llums", now.ms() );

    mTrace.back().stop( now );
}

void HttpAttempt::pop( AseTimeVal const& /*now*/ )
{
    if( !mTrace.empty() )
    {
        mTtrace = mTrace.back().mEnd;

        mTrace.clear();
    }
}

void HttpAttempt::complete( AseTimeVal const& now )
{
    if ( mTrace.empty() )
    {
        mTcomp = now;
    }
    else
    {
        if ( !mTrace.back().mClosed )
        {
            mTrace.back().stop( now );
        }

        mTcomp = mTrace.back().mEnd;
    }
}

