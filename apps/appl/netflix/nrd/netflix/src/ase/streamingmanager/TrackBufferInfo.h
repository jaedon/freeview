/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TRACK_BUFFER_INFO_H
#define TRACK_BUFFER_INFO_H

#include <deque>

#include <nrdase/ITrackBufferInfo.h>
#include "MediaFragment.h"

namespace netflix {
namespace ase {
class MediaStream;
class MediaTrack;

/**
 * @class TrackBufferInfo
 * @brief Class that implements ITrackBufferInfo
 */
class TrackBufferInfo : public ITrackBufferInfo
{
public:
    typedef shared_ptr<TrackBufferInfo> TrackBufferInfoPtr;

    TrackBufferInfo( shared_ptr<MediaTrack> pMediaTrack,
                     int32_t trackId, MediaType mediaType,
                     std::deque<MediaFragmentPtr>& mediaFragments );
    virtual ~TrackBufferInfo() {}

    // ITrackBufferInfo interface
    virtual int32_t trackId() const;
    virtual MediaType mediaType() const;
    virtual AseTimeStamp getFirstFragmentStartTime() const;
    virtual bool empty() const;
    virtual const_iterator begin();
    virtual const_iterator end();

    /** update the mMediaFragments based on the playback time */
    shared_ptr<MediaStream> updatePlaybackTime( AseTimeStamp const& playbackTime );

    int getPlaybackBitrate();
    void setTrackId(int32_t trackId);

protected:
    // ITrackBufferInfo protected methods
    virtual const_iterator& increment( const_iterator& it );
    virtual const_iterator& decrement( const_iterator& it );
    virtual BufferedFragmentInfo dereference( const_iterator& it );
    virtual bool equal( const_iterator const& lhs, const_iterator const& rhs );

    bool isValid(int indx);

    weak_ptr<MediaTrack> mMediaTrackPtr;

    uint32_t mTrackId;
    MediaType mMediaType;

    std::deque<MediaFragmentPtr>& mMediaFragments;
};

typedef TrackBufferInfo::TrackBufferInfoPtr TrackBufferInfoPtr;

}}
#endif
