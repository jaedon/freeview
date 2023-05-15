/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef AUDIOESPLAYERNATIVE_H
#define AUDIOESPLAYERNATIVE_H

/** @file AudioESPlayerNative.h - Reference implementation of the audio
 * IElementaryStreamPlayer.
 *
 * AudioESPlayerNative is the reference application's implementation of an audio
 * elementary stream player. It implements the IElementaryStreamPlayer interface
 * (see IElementaryStreamPlayer.h). A device partner may use or modify this
 * header and accompanying .cpp as needed.
 *
 * An AudioESPlayerNative is created by the
 * PlaybackGroupNative::createStreamPlayer() method each time a movie is loaded
 * for viewing and persists until the movie is unloaded. When it's created, the
 * player spawns a decoder thread and creates an audio renderer which in turn
 * spawns a renderer thread. The decoder thread uses the IESPlayerCallback
 * object it receives when it is created to pull audio samples from the netlix
 * application. When it receives a sample, it decrypts it decodes the sample and
 * adds it to the renderer's input queue. The renderer thread uses the
 * PlaybackGroupNative's ReferenceClock and the PTS timestamps on the decoded
 * frames to determine when to remove decoded frames from its input queue and
 * write them to the audio driver.
 *
 * In the reference application, there there are several different options for
 * the audio renderer that is used. The AudioESPlayer uses the ESManager (which
 * is available via the PlaybackGroupNative) to get the RendererManager that
 * provides the audio renderer that was selected at run time.
 */

#include "ESPlayerNative.h"

namespace netflix {
namespace device {
namespace esplayer {

class DeviceThread;

class NRDP_EXPORTABLE AudioESPlayerNative :  public ESPlayerNative
{
public:

    virtual ~AudioESPlayerNative();

    virtual NFErr init(const struct StreamPlayerInitData& initData,
                       shared_ptr<IESPlayerCallback> callback,
                       PlaybackGroupNative* playbackGroup);

    virtual void flush();
    virtual void close();
    virtual void decoderTask();
    virtual bool inputsAreExhausted();
    virtual MediaType getMediaType();
    virtual bool readyForPlaybackStart();
    virtual void enable();
    virtual void underflowReporter();

    bool isDisabledOrPending();

    class DecodedAudioBuffer
    {
    public:
        DecodedAudioBuffer() : size_(0) {}

        unsigned char* data()                { assert(size_ > 0); return data_;                         }
        uint32_t size()                      { return size_;                                            }
        void setDataBytes(uint32_t bytes)    { assert(size_ > 0 && bytes <= size_); dataBytes_ = bytes; }
        void setTimestamp(int64_t timestamp) { assert(size_ > 0); timestamp_ = timestamp;               }
        int64_t& getTimestamp()              { return timestamp_;}

    private:
        unsigned char* data_;
        uint32_t       size_;
        uint32_t       dataBytes_;
        int64_t        timestamp_;
    };

private:
  friend class PlaybackGroupNative;
    AudioESPlayerNative();
    virtual void beginFlush();
    virtual void endFlush();

    bool playbackPending();
    void setPlaybackPending(bool);

    shared_ptr<SampleWriterNative> mSampleWriter;
    std::auto_ptr<DeviceThread> mAudioDecoderThread;

    // During on-the-fly audio switch, the audio player is disabled, then
    // flushed, then re-enabled -- all while the PlaybackGroup's is in the PLAY
    // state. When the player is re-enabled, it might not have actually decoded
    // enough audio frames to ensure that it can immediately begin playing back
    // without underflowing.  The mPlaybackPending flag ensures that rendering
    // only begins when enough frames are available for playback.
    //
    // mPlaybackPending is modified on the decoder and SDK pumping loop threads
    // and read on the audio renderer thread. Access is protected with the
    // mDecoderTaskMutex.
    bool mPlaybackPending;
    bool mInputExhausted;
    bool mIsFlushing;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif
