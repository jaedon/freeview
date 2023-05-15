/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IMEDIA_SINK_H
#define IMEDIA_SINK_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/AseTimeStamp.h>
#include <nrdase/AseBuffer.h>
#include <nrdase/MimeType.h>

namespace netflix {
namespace ase {
/**
 * @class IMediaSink
 * @brief IMediaSink accespts a/v data in sequence from the streaming manager, and delivers the data
 *        to the device
 */
class IMediaSink
{
public:
    typedef shared_ptr<IMediaSink> IMediaSinkPtr;

    virtual ~IMediaSink() {}

    virtual bool canPlayType( MimeType& /*mime*/ ) const { return false; }

    /**
     * deliver a block of data to media sink
     * @param[in] dataBlock: a block of data
     * @param[in] manifestIndex the index of the period
     * @param[in] trackIndex the index of track in the original manifest
     * @param[in] streamIndex the index of the stream in the original manifest
     * @param[in] startOfFragment: true if the block is the first one of a media fragment,
     *            e.g. a video gop in many cases
     * @param[in] pts: For the first block of a fragment this is the earliest PTS of any
	 *				sample in the fragment. For subsequent blocks this is an approximation
	 *				of the earliest pts of any sample in the fragment not completely contained
	 *				in earlier blocks.
	 */
    virtual void deliver( AseBufferPtr const&    dataBlock,
                          int32_t manifestIndex,
                          int32_t trackIndex, int32_t streamIndex,
                          bool                startOfFragment,
                          AseTimeStamp const&    pts ) = 0;

    /**
     * Indicate that a new stream in the provided track is coming
     * @param[in] mediaHeader
     * @param[in] mediaHeaderSize
     * @param[in] manifestIndex the index of the period
     * @param[in] trackIndex the index of track in the original manifest
     * @param[in] streamIndex the index of the stream in the original manifest
     */
    virtual void newStream( unsigned char const* mediaHeader,
                            size_t  mediaHeaderSize,
                            int32_t manifestIndex,
                            int32_t trackIndex,
                            int32_t streamIndex ) = 0;

    /**
     * obtain the current playback information
     * @param[out] currentPlayoutRate
     * @param[out] currentPlayoutTime negative when unsure
     */
    virtual void currentPlaybackInfo( double& currentPlayoutRate,
                                      AseTimeStamp& currentPlayoutTime ) = 0;

    /**
     * notify that the end of the track is reached
     * @param[in] trackIndex
     */
    virtual void endOfTrack(int32_t manifestIndex, int32_t trackIndex) = 0;

    /**
     * flush the given media type's data
     * @param[in] mediaType
     */
    virtual void flushMedia(MediaType mediaType) = 0;

    virtual void updateStreamingBuffer() = 0;


};

typedef IMediaSink::IMediaSinkPtr IMediaSinkPtr;

}}

#endif
