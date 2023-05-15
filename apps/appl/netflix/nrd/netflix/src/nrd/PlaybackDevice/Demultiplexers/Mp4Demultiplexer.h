/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_MP4DEMULTIPLEXER_H
#define DEVICE_MP4DEMULTIPLEXER_H

#include <vector>
#include <nrdbase/tr1.h>
#include <nrd/ISystem.h>

#include "../Demultiplexer.h"
#include "EventQueue.h"
#include "ProducerConsumerList.h"

namespace netflix {
namespace device {

struct AudioAttributes;
struct VideoAttributes;

/**
 * @class Mp4Demultiplexer Mp4Demultiplexer.h
 * @brief Implementation of MP4 demultiplexer.
 */
class Mp4Demultiplexer : public Demultiplexer
{
public:
    class DataBlock
    {
    public:
        DataBlock();
        ~DataBlock();

        /* Get the number of bytes between the current offset and the
           end of the block. */
        uint32_t getDataSize();

        /* Get a pointer to data starting at the current offset. */
        unsigned char* getData();

        /* Move the offset forward in the block.  This will reduce the
           data size correspondingly. */
        void advanceOffset(uint32_t numBytes);

        void setDataBuffer(IBufferManager::Buffer& buffer);
        void setStreamAttributes(StreamAttributes *attributes);

        void setIsParsed();
        bool isParsed();

        const IBufferManager::Buffer& getDataBuffer() const;
        const StreamAttributes* getStreamAttributes() const;
        void reset();

    private:
        IBufferManager::Buffer dataBuffer_;
        uint32_t offset_;
        StreamAttributes* streamAttributes_;
        bool isParsed_;
    };

private:
    class StreamParser;

public:
    /**
     * Parse the stream header to retrieve audio/video/drm informations.
     *
     * @param[in]  headerData the stream header data to be parsed.
     * @param[in]  headerDataSize the stream header data size in bytes.
     * @param[out] streamAttributes the retrieved stream attribute information.
     * @param[out] preroll the audio/video preroll setting (in milliseconds).
     * @param[out] drmHeader the retrieved DRM header information.
     * @param[in]  drmSystemID a 128-bits UUID which identifies the DRM system
     *             used. NULL means "don't care".
     * @param[out] nflxHeader the retrieved Netflix DRM header information.
     *
     * @return NFErr_OK if stream header parsing is successful.
     *         NFErr_MP4ParsedData if header parsing failed.
     */
    static NFErr parseStreamHeader(const unsigned char*        headerData,
                                   uint32_t                    headerDataSize,
                                   StreamAttributes&           streamAttributes,
                                   std::vector<unsigned char>* drmHeader,
                                   const unsigned char*        drmSystemID,
                                   std::vector<unsigned char>* nflxHeader);

    Mp4Demultiplexer(IConsumerListView<DataBlock>& audioBlockList,
                     IConsumerListView<DataBlock>& videoBlockList,
                     esplayer::Format3D format3D,
                     const std::vector<ContentProfile>& videoProfiles,
                     const bool supportSeamlessFramerateSwitching,
                     shared_ptr<EventQueue> eventQueue
        );

    bool     init();
    void     close();
    void     deliverEndOfStream(IPlaybackDevice::StreamType type);
    void     setDiscontinuity();
    void     setAudioDiscontinuity();
    uint32_t getVideoSampleDuration();
    void     set3DFormat(esplayer::Format3D format3D);

    inline ullong   getCurrentPlaybackPosition(){ return currentPlaybackPositionPTS_; }
    inline void     setCurrentPlaybackPosition(ullong pts){ currentPlaybackPositionPTS_ = pts; }

    static void copyAttributes(VideoAttributes *videoAttr,
                               esplayer::MediaAttributes& mediaAttr,
                               esplayer::Format3D format3D = esplayer::MVC_COMBINED);

    static void copyAttributes(const AudioAttributes *audioAttr,
                               esplayer::MediaAttributes& mediaAttr);


    esplayer::Status getNextSample(esplayer::MediaType,
                                   esplayer::ISampleWriter& sampleWriter);
    esplayer::Status getNextSampleSize(esplayer::MediaType,
                                       uint32_t& sampleSize,
                                       uint32_t viewNum);
    esplayer::Status getNextSampleTimestamps(esplayer::MediaType type,
                                             llong& dts,
                                             llong& pts);

    shared_ptr<EventQueue> getAttachedEventQueue();

private:
    shared_ptr<StreamParser> audioParser_;
    shared_ptr<StreamParser> videoParser_;

    bool     firstVideoKeyFramePTSIsValid_;
    ullong firstVideoKeyFramePTS_;
    std::vector<ContentProfile> videoProfiles_;
    bool supportSeamlessFramerateSwitching_;

    // adding current playback PTS reported from player so that skipping samples
    // can be more accurate
    ullong currentPlaybackPositionPTS_;

    shared_ptr<EventQueue> eventQueue_;
};

}} // namespace netflix::device

#endif // DEVICE_MP4DEMULTIPLEXER_H
