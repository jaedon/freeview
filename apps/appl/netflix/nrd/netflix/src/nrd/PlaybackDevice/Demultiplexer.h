/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_DEMULTIPLEXER_H
#define DEVICE_DEMULTIPLEXER_H

#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/IPlaybackDevice.h>
#include <nrd/IBufferManager.h>
#include <list>


namespace netflix {
namespace device {

struct StreamAttributes;
struct VideoAttributes;
struct AudioAttributes;

/**
 * @class Demultiplexer Demultiplexer.h
 * @brief Demultiplexer interface
 */
class Demultiplexer : public enable_shared_from_this<Demultiplexer>
{
public:
    /**
     * A contiguous segment of data.
     */
    struct DataSegment
    {
        unsigned char* data;
        uint32_t numBytes;
    };

public:

    virtual ~Demultiplexer() {}

    /**
     * From the input data, locate and deliver the audio/video access-units into the
     * audio/video ring buffers.
     *
     * @param[in] data the input data to be processed.
     * @param[in] dataSize number of bytes in the input data.
     * @param[in] isStartOfGop true if the data is at the start of GOP.
     * @param[in] gopTimeOffset the presentation time offset (in milliseconds)
     *            of the corresponding GOP. If isStartOfGop is false, the
     *            value of this argument is undefined and should be ignored.
     * @param[in] streamAttributes attributes of the stream which the input data
     *            belongs to.
     *
     * @return NFErr_OK if data is processed and delivered successfully.
     *         NFErr_Pending if the available space in ring buffers is too small,
     *         nothing are delivered.
     *         NFErr_DRMFailed if decryption failed.
     *         NFErr_Bad if something else is wrong.
     */

    /*
     * For any after object creation work
     */
    virtual bool init() = 0;

    virtual void close() = 0;

    /**
     * Deliver a special marker into the audio/video ring buffers to
     * notify the downstream components about the end-of-stream.
     */
    virtual void deliverEndOfStream(IPlaybackDevice::StreamType type) = 0;

    /**
     * Flag that the data delivered right after this point is discontinuous
     * from previous data.
     */
    virtual void setDiscontinuity() = 0;

    /**
     * Flag that the audio data delivered right after this point is discontinuous
     * from previous data.
     */
    virtual void setAudioDiscontinuity() = 0;

    /**
     * Get Sample duration of video
     */
    virtual uint32_t getVideoSampleDuration() = 0;

    /**
     * Get/Set current playback position for sample skipping processing while demuxing
     */
    virtual ullong  getCurrentPlaybackPosition() = 0;
    virtual void    setCurrentPlaybackPosition(ullong pts) = 0;

    virtual esplayer::Status getNextSample(esplayer::MediaType type, esplayer::ISampleWriter& sampleWriter) = 0;
    virtual esplayer::Status getNextSampleSize(esplayer::MediaType type,
                                               uint32_t& sampleSize,
                                               uint32_t viewNum) = 0;
    virtual esplayer::Status getNextSampleTimestamps(esplayer::MediaType type,
                                                     llong& dts,
                                                     llong& pts) = 0;

    virtual void set3DFormat(esplayer::Format3D format3D) = 0;

};

}} // namespace netflix::device

#endif // DEVICE_DEMULTIPLEXER_H
