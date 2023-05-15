/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 * This file describes an abstract API for obtaining information about
 * the available tracks and streams and their bitrate profile at
 * a fragment level. i.e. it provides the information about the
 * "available choices" for the stream selection algorithm.
 *
 * This is provided as an abstract API to decouple the implementation
 * of the rate adaptation heuristics from the core StreamManager
 * implementation (the alternative of copying the stream information
 * to a neutral data structure would involve copying around quite
 * a lot of information each time a rate decision is required).
 *
 * The user of this interface starts with an ITrackInfo object providing
 * the information for a single track.
 */
#ifndef I_TRACK_INFO_H
#define I_TRACK_INFO_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdase/AseTimeStamp.h>
#include "StartplayPoint.h"

#include <vector>

namespace netflix {
namespace ase {

class IStreamingLocationSet;

/**
 * Contains information about a single fragment of a stream
 */
struct MediaFragmentInfo
{
    /** Playout duration of the fragment */
    AseTimeInterval    mDuration;

    /** Size of the fragment in bytes */
    int32_t        mSize;

    MediaFragmentInfo( AseTimeInterval const& duration, int32_t const& size );
    MediaFragmentInfo();
};

/**
 * @class IStreamInfo
 * @brief stream info, which includes the id and index map of the stream.
 *        The last media fragment is an invalid one, which indicates the end time
 *        and end offset.
 */
class IStreamInfo : public enable_shared_from_this<IStreamInfo>
{
public:
    typedef shared_ptr<IStreamInfo> IStreamInfoPtr;

    class const_iterator;
    friend class const_iterator;

    virtual ~IStreamInfo() {}

    /** get the unique id for the stream */
    virtual int32_t streamId() const = 0;

    virtual std::string const& streamKey() const = 0;

    /** has fragments? */
    virtual bool empty() const = 0;

    /** is complete? */
    virtual bool complete() const = 0;

    /** obtain the stream's preferred location set */
    virtual AseErrorCode getPreferredLocationSet(shared_ptr<IStreamingLocationSet>& pStreamingLocationSet) = 0;

    /** Get the start timestamp of the first fragment */
    virtual AseTimeStamp firstFragmentTime() const = 0;

    /** Get the timestamp of the current fragment */
    virtual AseTimeStamp currentFragmentTime() const = 0;

    /** Get the current fragment size*/
    virtual uint32_t getCurrentFragmentSize() const = 0;

    /** startplay curve of the given stream */
    virtual std::vector<StartplayPoint>  getStartplayCurve() const = 0;

    /** get the average bitrate of the given stream */
    virtual int getAverageBitrate() const = 0;

    /** whether the stream is available for streaming */
    virtual bool isAvailable() const = 0;

    /** An iterator to the first fragment */
    virtual const_iterator begin() = 0;

    /** An iterator to the element past the last one */
    virtual const_iterator end() = 0;

    class const_iterator
    {
    public:
        friend class IStreamInfo;

        const_iterator( const_iterator const& other ) : mOwner( other.mOwner ), mIndex( other.mIndex ) {}
        const_iterator& operator=( const_iterator const& other )
        {
            mOwner = other.mOwner;
            mIndex = other.mIndex;
            return *this;
        }

        const_iterator& operator++() { return mOwner.lock()->increment( *this ); }
        MediaFragmentInfo operator*() { return mOwner.lock()->dereference( *this ); }

        bool operator==( const_iterator const& other ) { return mOwner.lock()->equal( *this, other ); }
        bool operator!=( const_iterator const& other ) { return !mOwner.lock()->equal( *this, other ); }

        const_iterator( shared_ptr<IStreamInfo> pOwner, int index ) : mOwner( pOwner ), mIndex( index ) {}

        int getIndex() const { return mIndex; }
        void setIndex(int indx) { mIndex = indx; }

    protected:
        weak_ptr<IStreamInfo>    mOwner;
        int             mIndex;
    };

protected:
    virtual const_iterator& increment( const_iterator& it ) = 0;
    virtual MediaFragmentInfo dereference( const_iterator& it ) = 0;
    virtual bool equal( const_iterator const& lhs, const_iterator const& rhs ) = 0;

};

typedef IStreamInfo::IStreamInfoPtr IStreamInfoPtr;

class ITrackInfo
{
public:
    typedef shared_ptr<ITrackInfo> ITrackInfoPtr;

    class const_iterator;
    friend class const_iterator;

    virtual ~ITrackInfo() {}

    /** Get the unique id */
    virtual int32_t trackId() const = 0;

    /** Get the media type */
    virtual MediaType mediaType() const = 0;

    /** An iterator to the current selected stream */
    virtual const_iterator begin() = 0;

    /** An iterator to the stream after the last one */
    virtual const_iterator end() = 0;

    /** the iterator to the current selected stream */
    virtual const_iterator current() = 0;

    /** whether the track is completely downloaded */
    virtual bool isCompleted() = 0;

    class const_iterator
    {
    public:
        friend class ITrackInfo;

        const_iterator( const_iterator const& other ) : mOwner( other.mOwner ), mIndex( other.mIndex ) {}

        const_iterator& operator=( const_iterator const& other ) { mOwner = other.mOwner; mIndex = other.mIndex; return *this;}

        const_iterator& operator++() { return mOwner.lock()->increment( *this ); }
        const_iterator& operator--() { return mOwner.lock()->decrement( *this ); }
        IStreamInfoPtr operator*() { return mOwner.lock()->dereference( *this ); }

        bool operator==( const_iterator const& other ) { return mOwner.lock()->equal( *this, other ); }
        bool operator!=( const_iterator const& other ) { return !mOwner.lock()->equal( *this, other ); }

        bool hasNext() { return mOwner.lock()->hasNext( *this ); }
        bool hasPrevious() { return mOwner.lock()->hasPrevious( *this ); }

        const_iterator( shared_ptr<ITrackInfo> pOwner, int index ) : mOwner( pOwner ), mIndex( index ) {}
        int getIndex() const { return mIndex; }
        void setIndex(int indx) { mIndex = indx; }
    protected:
        weak_ptr<ITrackInfo>     mOwner;
        int             mIndex;
    };

protected:
    virtual const_iterator& increment( const_iterator& it ) = 0;
    virtual const_iterator& decrement( const_iterator& it ) = 0;
    virtual IStreamInfoPtr dereference( const_iterator& it ) = 0;
    virtual bool equal( const_iterator const& lhs, const_iterator const& rhs ) = 0;
    virtual bool hasNext( const_iterator const& it ) = 0;
    virtual bool hasPrevious( const_iterator const& it ) = 0;
};

typedef ITrackInfo::ITrackInfoPtr ITrackInfoPtr;


inline
MediaFragmentInfo::MediaFragmentInfo( AseTimeInterval const& duration, int32_t const& size )
    : mDuration( duration )
    , mSize( size )
{
}

inline
MediaFragmentInfo::MediaFragmentInfo()
    : mDuration( AseTimeStamp::ZERO ),
      mSize( 0 )
{
}

}}
#endif  /* I_TRACK_INFO_H */
