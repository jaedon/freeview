/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef HTTP_ATTEMPT_H
#define HTTP_ATTEMPT_H

#include <vector>

#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/Time.h>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/AseUrl.h>

#include <nrdase/ByteRange.h>

namespace netflix {
namespace ase {

/** A single bandwidth measurement entry
 *  Each entry in the vector gives the bytes received in one
 *  measurement interval (indicated separately)
 */
class BwEntry
{
public:
    AseTimeVal                  mStart;
    AseTimeVal                  mEnd;
    bool                        mClosed;
    std::vector<unsigned int>   mTrace;

private:
    unsigned int                mTinterval;

public:
    // monotonic time for reporting
    Time                  mStartMono;

    BwEntry( unsigned int mTinterval );

    BwEntry& operator=( BwEntry const& other );

    void add( AseTimeVal const& now, uint32_t bytes );

    void stop( AseTimeVal const& now );

    void reset();

    unsigned int getInterval() const;

    void setInterval( unsigned int interval );

    void checkInterval( unsigned int interval ) const;
};

/** A bandwidth trace. Each entry corresponds to a period
 *  of continuous reception
 */
typedef std::vector<BwEntry> BwTrace;

/** A record of a single HTTP request */
// XXX: Can we map from url to DomainKey and stream? cdndldist needs cdnid and dlid
struct HttpAttempt
{
    int                     mTcpId;         // id of the TCP connection
//    ConstManifestUrlPtr     mOriginalUrl;   // The original Manifest URL
    AseUrl             mUrl;           // The actual requested URL
    ByteRange               mRange;         // The byte range
    bool                    mOpenRange;     // True if the actual request was for an open range
    bool                    mPipelined;     // True if the request was sent before completion of the previous one
    AseTimeVal         mTreq;          // Request time
    AseTimeVal         mTresp;         // Response time
    AseTimeVal         mTtrace;        // Start time of trace
    AseTimeVal         mTcomp;         // Completion time
    int                     mBrecv;         // body bytes received in response to this request
    int                     mHrecv;         // header bytes received in response to this request
    int                     mHttpCode;      // HTTP response code
    AseUrl             mLocation;      // Location header on redirect
    AseErrorCode            mErrorCode;     // Other error code (TODO: place holder for the error stack)
    int                     mTinterval;     // Trace interval
    BwTrace                 mTrace;         // Bandwidth trace
    std::string             mServerTcpInfo; // The server side TCP info.

    // Helper functions defined in HttpAttempt.cpp
    HttpAttempt();

    void reset();

    void add(   AseTimeVal const& now,
                uint32_t bytes,
                bool header,
                bool complete );

    void stop( AseTimeVal const& now );

    /** Pop the current trace information and reset to an empty trace */
    void pop( AseTimeVal const& now );

    void complete( AseTimeVal const& now );
};

inline BwEntry& BwEntry::operator=( BwEntry const& other )
{
    assert( mTinterval == other.mTinterval );

    mStart = other.mStart;
    mEnd = other.mEnd;
    mClosed = other.mClosed;
    mTrace = other.mTrace;
    mStartMono = other.mStartMono;

    return *this;
}

inline unsigned int BwEntry::getInterval() const
{
    return mTinterval;
}

inline void BwEntry::checkInterval( unsigned int interval ) const
{
    (void)interval;
    assert( mTinterval == interval );
}

inline HttpAttempt::HttpAttempt()
    : mTcpId( -1 )
    , mOpenRange( false )
    , mPipelined( false )
    , mTreq( AseTimeVal::ZERO )
    , mTresp( AseTimeVal::ZERO )
    , mTtrace( AseTimeVal::ZERO )
    , mTcomp( AseTimeVal::ZERO )
    , mBrecv( 0 )
    , mHrecv( 0 )
    , mHttpCode( -1 )
    , mErrorCode( AS_NO_ERROR )
    , mTinterval( 1000 )
    , mServerTcpInfo( "" )
{
}



inline void HttpAttempt::reset()
{
    mTcpId = -1;
    mOpenRange = false;
    mPipelined = false;
    mTreq = AseTimeVal::ZERO;
    mTresp = AseTimeVal::ZERO;
    mTtrace = AseTimeVal::ZERO;
    mTcomp = AseTimeVal::ZERO;
    mBrecv = 0;
    mHttpCode = -1;
    mErrorCode = AS_NO_ERROR;
    mTrace.clear();
}

inline BwEntry::BwEntry( unsigned int interval )
    : mStart( AseTimeVal::ZERO )
    , mEnd( AseTimeVal::ZERO )
    , mClosed( false )
    , mTinterval( interval > 1000 ? interval : 1000 )
{
}

inline void BwEntry::reset()
{
    mStart = AseTimeVal::ZERO;
    mEnd = AseTimeVal::ZERO;
    mClosed = false;
    mTrace.clear();
}

}} // namespace netflix::ase

#endif
