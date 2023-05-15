/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_TRACK_BUFFER_INFO_H
#define I_TRACK_BUFFER_INFO_H

#include <nrdase/ITrackInfo.h>

namespace netflix {
namespace ase {

struct BufferedFragmentInfo : public MediaFragmentInfo
{
    // The number of bytes received so far for this fragmemt
    uint32_t        mReceived;

    // The id of the stream, to which the block belongs.
    // We need this to obtain some stream property linked with the block, e.g.
    // average bitrate of the stream, ...
    int32_t        mStreamId;
};

class ITrackBufferInfo : public enable_shared_from_this<ITrackBufferInfo>
{
public:
    typedef shared_ptr<ITrackBufferInfo> ITrackBufferInfoPtr;

    class const_iterator;
    friend class const_iterator;

    virtual ~ITrackBufferInfo() {}

    /** Get the unique id */
    virtual int32_t trackId() const = 0;

    /** Get the media type */
    virtual MediaType mediaType() const = 0;

    /** Get the start timestamp of the first buffered fragment */
    virtual AseTimeStamp getFirstFragmentStartTime() const = 0;

    /** whether the ITrackBufferInfo is empty */
    virtual bool empty() const = 0;

    /** An iterator to the first buffered fragment */
    virtual const_iterator begin() = 0;

    /** An iterator to the buffered fragment past the last one */
    virtual const_iterator end() = 0;

    class const_iterator
    {
    public:
        friend class ITrackBufferInfo;

        const_iterator( const_iterator const& other ) : mOwner( other.mOwner ), mIndex( other.mIndex ) {}

        const_iterator& operator=( const_iterator const& other ) { mOwner = other.mOwner; mIndex = other.mIndex; return *this;}

        const_iterator& operator++() { return mOwner.lock()->increment( *this ); }
        const_iterator& operator--() { return mOwner.lock()->decrement( *this ); }
        BufferedFragmentInfo operator*() { return mOwner.lock()->dereference( *this ); }

        bool operator==( const_iterator const& other ) { return mOwner.lock()->equal( *this, other ); }
        bool operator!=( const_iterator const& other ) { return !mOwner.lock()->equal( *this, other ); }

        const_iterator( shared_ptr<ITrackBufferInfo> pOwner, int index ) : mOwner( pOwner ), mIndex( index ) {}
        int getIndex() const { return mIndex; }
        void setIndex(int indx) { mIndex = indx; }

    protected:
        weak_ptr<ITrackBufferInfo>     mOwner;
        int             mIndex;
    };

protected:
    virtual const_iterator& increment( const_iterator& it ) = 0;
    virtual const_iterator& decrement( const_iterator& it ) = 0;
    virtual BufferedFragmentInfo dereference( const_iterator& it ) = 0;
    virtual bool equal( const_iterator const& lhs, const_iterator const& rhs ) = 0;

};

typedef ITrackBufferInfo::ITrackBufferInfoPtr ITrackBufferInfoPtr;

}}
#endif
