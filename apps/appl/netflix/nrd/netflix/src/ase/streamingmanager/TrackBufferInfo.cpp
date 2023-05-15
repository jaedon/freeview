/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "TrackBufferInfo.h"

using namespace std;
using namespace netflix::ase;

TrackBufferInfo::TrackBufferInfo( shared_ptr<MediaTrack> pMediaTrack,
                     int32_t trackId, MediaType mediaType,
                     std::deque<MediaFragmentPtr>& mediaFragments )
            : mMediaTrackPtr(pMediaTrack),
              mTrackId(trackId), mMediaType(mediaType),
              mMediaFragments(mediaFragments)
{

}

int32_t TrackBufferInfo::trackId() const
{
    return mTrackId;
}

void TrackBufferInfo::setTrackId(int32_t trackId)
{
    mTrackId = trackId;
}

MediaType TrackBufferInfo::mediaType() const
{
    return mMediaType;
}

AseTimeStamp TrackBufferInfo::getFirstFragmentStartTime() const
{
    if ( mMediaFragments.empty() )
        return AseTimeStamp::INFINITE;
    else
    {
        MediaFragmentPtr pMediaFragment = mMediaFragments.front();
        return pMediaFragment->getStartPts();
    }
}

bool TrackBufferInfo::empty() const
{
    return mMediaFragments.empty();
}

ITrackBufferInfo::const_iterator TrackBufferInfo::begin()
{
    const_iterator iter = const_iterator(shared_from_this(), 0);
    return iter;
}

ITrackBufferInfo::const_iterator TrackBufferInfo::end()
{
    const_iterator iter = const_iterator(shared_from_this(), mMediaFragments.size());
    return iter;
}

ITrackBufferInfo::const_iterator& TrackBufferInfo::increment( const_iterator& it )
{
    it.setIndex(it.getIndex() + 1);
    return it;
}

ITrackBufferInfo::const_iterator& TrackBufferInfo::decrement( const_iterator& it )
{
    it.setIndex(it.getIndex() - 1);
    return it;
}

BufferedFragmentInfo TrackBufferInfo::dereference( const_iterator& it )
{
    if ( isValid(it.getIndex()) )
        return mMediaFragments[it.getIndex()]->getFragmentInfo();
    else
    {
        BufferedFragmentInfo fragmentInfo;
		fragmentInfo.mReceived = 0;
        fragmentInfo.mStreamId = -1;
        return fragmentInfo;
    }
}

bool TrackBufferInfo::equal( const_iterator const& lhs, const_iterator const& rhs )
{
    if ( !isValid(lhs.getIndex()) && !isValid(rhs.getIndex()) )
        return true;

    return (lhs.getIndex() == rhs.getIndex());
}

bool TrackBufferInfo::isValid( int index )
{
    if ( (index >= 0) && ((uint32_t)index < mMediaFragments.size()) )
        return true;
    else
        return false;
}

shared_ptr<MediaStream> TrackBufferInfo::updatePlaybackTime( AseTimeStamp const& playbackTime )
{
    shared_ptr<MediaStream> pPrevMediaStream = shared_ptr<MediaStream>();
    while ( mMediaFragments.size() > 0 )
    {
        MediaFragmentPtr pMediaFragment = mMediaFragments.front();
        BufferedFragmentInfo fragmentInfo = pMediaFragment->getFragmentInfo();
        AseTimeStamp endTime = pMediaFragment->getStartPts() + fragmentInfo.mDuration;
        if ( (endTime > playbackTime) || !pMediaFragment->downloadedAndForwarded() )
            break;
        else
        {
            pPrevMediaStream = pMediaFragment->getMediaStream();
            mMediaFragments.pop_front();
            mMediaTrackPtr.lock()->decrBufferSize(fragmentInfo.mSize);
        }
    }
    return pPrevMediaStream;
}

int TrackBufferInfo::getPlaybackBitrate()
{
    if ( mMediaFragments.size() > 0 )
    {
        MediaFragmentPtr pMediaFragment = mMediaFragments.front();
        return pMediaFragment->getNominalBitrate();
    }
    else
        return 0;
}
