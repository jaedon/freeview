/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IPLAYBACKDEVICE_H
#define IPLAYBACKDEVICE_H

/** @file IPlaybackDevice.h The playback device interface.
 *
 * This interface is deprecated. Partners should not implement a playback device
 * based on this interface. Instead, partners should implement elementary stream
 * players based on IElementaryStreamPlayer interface.
 */

#include <vector>

#include <nrdase/AseCommonDataTypes.h>
#include <nrd/Rect.h>
#include <nrd/IBufferManager.h>
#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>
#include <nrd/StreamAttributes.h>

#define TOBEDEPRECATED

namespace netflix {
namespace device {

/**
 * @class IPlaybackDevice IPlaybackDevice.h
 * @brief Interface for encapsulation of device playback functionalities.
 *
 * The following functions will be called from the same thread, so they
 * are guaranteed mutual exclusive.
 *
 *     feedStreamHeader()
 *     feedData()
 *     setSpeed()
 *     flush()
 *     flushAudio()
 *     endOfStream()
 */
class IPlaybackDevice
{
public:
    static const uint32_t NORMAL_SPEED     = (65536); /**< the speed value for normal speed */
    static const uint32_t NORMAL_SPEED_Po2 = (   16); /**< the normal speed value in power-of-two (2 ^ 16 = 65536) */
    static const uint32_t PAUSED_SPEED     = (    0); /**< the speed value for paused speed */

    /** Playback Device Event Identifiers. */
    enum EventID
    {
        /**
         * Presentation Timestamp Update.
         *
         * If the presentation timestamps of the streams have
         * accounted for the "pre-rolling" period before playback
         * starts, the presentation timestamps updated here should
         * have the influence of the "pre-rolling" duration removed
         * from them. In other words, it is the timestamp relative
         * to the beginning of movie.
         *
         * Post this event to update the current presentation
         * timestamp whenever the timestamp has changed from the
         * previous value by more than 500 milliseconds.
         *
         * eventParam1: the current presentation timestamp in milliseconds.
         * eventParam2: reserved (should be 0).
         */
        EVENT_PTS_UPDATE,

        /**
         * Data Exhaustion (Buffer Underflow) Notification.
         *
         * Post this event (only once) right after the playback data
         * gets exhausted in device. No more exhaustion events should
         * be posted when the data exhaustion condition persists,
         * unless after the device is reset by flush() or re-buffering
         * is done by pausing playback, and the subsequent playback
         * session triggers another data exhaustion condition.
         *
         * Note: for the devices who support dynamic audio switch, the
         *       playback data exhaustion in audio pipeline should be
         *       considered normal. Therefore, this notification should
         *       be based on video pipeline status only.
         *
         * eventParam1: reserved (should be 0).
         * eventParam2: reserved (should be 0).
         */
        EVENT_DATA_EXHAUSTED,

        /**
         * End of Stream Notification.
         *
         * Post this event (only once) right after the last
         * presentation unit gets rendered. No more end_of_stream
         * events should be posted unless after the device is reset by
         * flush() and a new session of playback triggers another
         * end_of_stream condition.
         *
         * eventParam1: reserved (should be 0).
         * eventParam2: reserved (should be 0).
         */
        EVENT_END_OF_STREAM,

        /**
         * Unrecoverable Device Playback Error Notification.
         *
         * This event should be posted if the device encounters any
         * unrecoverable error during playback.
         *
         * eventParam1: implementation specific.
         * eventParam2: implementation specific.
         */
        EVENT_ERROR,

        /**
         * Stream information.
         *
         * This event is fired only when specified by options. When fired, there
         * are two types of events. First event is for maximum resoluton and
         * framerate for entire movie when opening a new movie. After that, each
         * GOP's information is fired.
         */
        EVENT_STREAM_INFO
    };

    /** Stream Type */
    enum StreamType
    {
        AUDIO = 0, /**< Audio Only Stream */
        VIDEO = 1  /**< Video Only Stream */
    };

    /** Stream Information */
    TOBEDEPRECATED struct StreamInfo
    {
        /** Stream type */
        StreamType streamType_;

        /** Stream content profile */
        ContentProfile contentProfile_;

        /** Image width in pixels. Undefined for audio streams. */
        uint32_t videoImageWidth_;

        /** Image height in pixels. Undefined for audio streams. */
        uint32_t videoImageHeight_;

        /** Video frame rate value. Undefined for audio streams */
        uint32_t videoFrameRateValue_;

        /** Video frame rate scale. Undefined for audio streams */
        uint32_t videoFrameRateScale_;

        /** noninal bitrate in Kbps */
        uint32_t nominalBitrateInKbps_;
        /**
         * X component of the pixel aspect ratio.
         * Undefined for audio streams.
         */
        uint32_t videoPixelAspectRatioX_;

        /**
         * Y component of the pixel aspect ratio.
         * Undefined for audio streams.
         */
        uint32_t videoPixelAspectRatioY_;

        /** Header data of the stream */
        const std::vector<unsigned char>* headerData_;
    };

    /** Video stream attributes to initialize video device */
    struct CommonVideoAttributesFromManifest
    {
        /** Video Codec Type */
        VideoCodecType codecType_;

        /** 3D or not */
        Video3DType video3DType_;

        /** max video width in pixels */
        uint32_t maxVideoWidth_;

        /** Image height in pixels */
        uint32_t maxVideoHeight_;

        /** pixel aspect ratio for max video resolution */
        uint32_t pixelAspectRatioX_;
        uint32_t pixelAspectRatioY_;

        /** Video frame rate value */
        uint32_t videoFrameRateValue_;

        /** Video frame rate scale */
        uint32_t videoFrameRateScale_;

    };

    /** Audio stream attributes to initialize audio device */
    struct CommonAudioAttributesFromManifest
    {
        AudioCodecType codecType_;

        /** Number of channels */
        uint32_t mNumberOfChannels_;

        /** Sampling frequency */
        uint32_t mSamplesPerSecond_;
    };

    /**
     * From sekwon: At open() time, information we need is only for initialize
     * players. Until NRDP 4.1, we have not provide information for audio, but I
     * think we should provide audio information also to make startup delay to
     * be small.  Below is my proposal to have separate Video and Audio Stream
     * with minimal informations necessary for initialize players
     */

    /** Stream Information from Manifest*/
    struct VideoAttributesFromManifest
    {
        /** Stream content profile */
        ContentProfile contentProfile_;

        /** Content Id - a.k.a movieId, which shared DRM license */
        uint32_t contentId_;

        /** Video frame rate value. Undefined for audio streams */
        uint32_t videoFrameRateValue_;

        /** Video frame rate scale. Undefined for audio streams */
        uint32_t videoFrameRateScale_;

        /** nominal bitrate of stream */
        uint32_t nominalBitrateInKbps_;
    };

    struct AudioAttributesFromManifest
    {
        /** Stream content profile */
        ContentProfile contentProfile_;

        /** nominal bitrate of stream */
        uint32_t nominalBitrateInKbps_;
    };

public:
    /** Pre-condition: the device is currently closed. */
    virtual ~IPlaybackDevice() {}

    /**
     * Open the playback device to prepare for running, the information
     * of all streams that could be delivered to the device are included. The
     * header data from one of the streams is guaranteed, which can be used
     * to initialize the DRM module before the license challenge/response
     * process begins.
     *
     * After opened, the device has a playback speed of 0 (i.e. it's
     * paused).
     *
     * Pre-condition: the device is currently closed.
     * Pre-condition: the input vector of StreamInfo is not empty.
     * Pre-condition: the bufferManager pointer should not be NULL.
     *
     * @param[in] streams the information of all streams.
     * @param[in] containerType the stream container type.
     * @param[in] drmType the DRM system type.
     * @param[in] maxWidth optional maximum width for video. If not present will try
     *                     to read value from stream attributes
     * @param[in] maxHeight optional maximum height for video. If not present will try
     *                     to read value from stream attributes
     *
     * @return NFErr_OK if successful, NFErr_MP4ParsedData if the MP4
     *         demultiplexer reports an inconsistency in the MP4 header data,
     *         and NFErr_Bad if something else went wrong.
     */
    /**
     * deprecated API
     */
    virtual TOBEDEPRECATED NFErr open(const std::vector<StreamInfo>& streams,
                                      ContainerType containerType,
                                      DrmType drmType,
                                      uint32_t maxWidth = 0,
                                      uint32_t maxHeight = 0) = 0;
    /**
     * Open playback device to prepare for running.
     *
     * After opened, playback device has
     * - ProducerConsumerList objects for video and audio
     * - Demultiplxer object to parse stream per container type
     */
    virtual NFErr open(ContainerType containerType) = 0;

    /**
     * Initialize the playback device. Devices may implement intialization that is not
     * necessary for challenge generation here, so that it may be carried out in parallel
     * with the license exchange
     *
     * Pre-condition: the device is currently open
     */
    virtual NFErr TOBEDEPRECATED initializePlayers() { return NFErr_OK; }

    virtual NFErr initializeVideoPlayer(const std::vector<unsigned char>& headerData,
                                        const CommonVideoAttributesFromManifest &videoAttributes) = 0;

    virtual NFErr initializeAudioPlayer(const std::vector<unsigned char>& headerData,
                                        const CommonAudioAttributesFromManifest &audioAttributes) = 0;

    /**
     * Close the playback device.
     *
     * Pre-condition: the device is currently opened.
     */
    virtual void close() = 0;

    /**
     * Deliver audio/video header data to the playback device.
     *
     * Whenever the upper layer decides to switch to a different audio/video
     * stream (the "target stream"), immediately before the first stream data
     * block from the target stream is delivered via feedData(), the header
     * data of the target stream will be delivered via this function first.
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[in] headerData the audio/video header data.
     * @param[in] streamIndex the index of the stream which the delivered
     *            header data belongs to. It's a 0-based index the same as the
     *            index of the corresponding entry in the StreamInfo
     *            vector argument of open().
     * @param[in] streamType the type of stream.
     *
     * @returns NFErr_OK on success,  NFErr_MP4ParsedData if there is an error parsing a
     *          stream header, or NFErr_Bad if the manifestIndex or streamIndex are
     *          out of range.
     */
    /**
     * deprecated API
     */
    virtual TOBEDEPRECATED NFErr feedStreamHeader(const std::vector<unsigned char>& headerData,
                                                  uint32_t streamIndex,
                                                  uint32_t manifestIndex,
                                                  StreamType streamType) = 0;

    /**
     * Deliver video header data to the playback device
     * @param[in] headerData the video header data.
     * @param[in] additionalVideoAttributes
     * @param[in] streamId: a unique identifier for each stream
     * @returns NFErr_OK on success,  NFErr_MP4ParsedData if there is an error parsing a
     *          stream header, or NFErr_Bad if the manifestIndex or streamIndex are
     *          out of range.
     */
    virtual NFErr feedVideoStreamHeader(const std::vector<unsigned char>& headerData,
                                        const VideoAttributesFromManifest& additionalVideoAttributes,
                                        uint32_t streamId) = 0;
    /**
     * Deliver audio header data to the playback device
     * @param[in] headerData the audio header data.
     * @param[in] additionalVideoAttributes
     * @param[in] streamId: a unique identifier for each stream
     * @returns NFErr_OK on success,  NFErr_MP4ParsedData if there is an error parsing a
     *          stream header, or NFErr_Bad if the manifestIndex or streamIndex are
     *          out of range.
     */

    virtual NFErr feedAudioStreamHeader(const std::vector<unsigned char>& headerData,
                                        const AudioAttributesFromManifest& additionalAudioAttributes,
                                        uint32_t streamId) = 0;
    /**
     * new API
     * Deliver one drm header data to the playback device.
     * @param[in] drmHeader the drm header data
     * @return
     */
    virtual NFErr feedDrmHeader(const std::vector<unsigned char>& drmHeader) = 0;

    virtual TOBEDEPRECATED bool drmHeaderReceived() = 0;

    /**
     * The SDK calls this to allow the playback device to store stream info
     * information associated with additional manifests after
     * IPlaybackDevice::open is called.  The stream info for the 0th manifest is
     * provided via IPlaybackDevice::open().  Stream info for additional
     * manifests can then be specified using setStreamInfo.
     *
     * @param[in] streams the information of all streams.
     *
     * @param[in] manifestIndex the index of the manifest corresponding to this
     *            stream info.
     *
     * @returns NFErr_OK on success, NFErr_MP4ParsedData if there is an error parsing a
     * stream header.
     */
    virtual TOBEDEPRECATED NFErr setStreamInfo(const std::vector<StreamInfo>& streams,
                                               uint32_t manifestIndex) = 0;


    /**
     * Deliver audio/video data to the playback device. If the playback
     * device's internal pipe-line does not have enough space to accept
     * the data, NFErr_Pending will be returned and the caller should
     * wait and re-attempt delivery later.
     *
     * The IBufferManager::Buffer structure is used to pass in the data to be
     * delivered. After it is delivered and the device is done with the data,
     * the device implementation is responsible for setting the inUse_ flag
     * pointed to by IBufferManager::Buffer.inUse_, to false (as in
     * "*(dataBuffer.inUse_) = false"). The IBufferManager::Buffer structure and
     * the IBufferManager::Buffer::byteBuffer_ it points to will not be reused
     * until the inUse_ flag pointed to by IBufferManager::Buffer.inUse_ is set
     * to false by the device implementation.
     *
     * After endOfStream() is called, the caller will not be allowed
     * to deliver any audio/video data unless flush() is called to
     * reset the device first.
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[in] dataBuffer the buffer which contains the data to be
     *            delivered.
     * @param[in] streamIndex the index of the stream which the delivered
     *            data belongs to. It's a 0-based index the same as the
     *            index of the corresponding entry in the StreamInfo
     *            vector argument of open().
     * @param[in] streamType the type of stream.
     *
     * @return NFErr_OK if the data is delivered successfully;
     *         NFErr_Pending if the internal pipe-line of device is full;
     *         NFErr_NotReady if the device has space but is not ready to
     *         accept data (caller should retry);
     *         NFErr_NotAllowed if the data is rejected because
     *         endOfStream() was called;
     *         NFErr_DRMFailed if decryption failed, which is an
     *         unrecoverable error;
     *         NFErr_Bad if any other unrecoverable error happens;
     *         NFErr_ArrayOutOfBoundary if the streamIndex is invalid.
     */
    /**
     * deprecated
     */
    virtual TOBEDEPRECATED NFErr feedData(IBufferManager::Buffer& dataBuffer,
                                          uint32_t streamIndex,
                                          uint32_t manifestIndex,
                                          StreamType streamType) = 0;

    virtual NFErr feedData(IBufferManager::Buffer& dataBuffer,
                           StreamType streamType,
                           uint32_t streamId) = 0;

    /**
     * Set the playback speed of device.
     *
     * For normal speed playback, 65536 should be specified as the speed
     * argument. For playback to be paused, 0 should be specified. For
     * other playback speeds, the ratio of the specified value relative
     * to 65536 is the ratio of playback speed relative to normal speed.
     * For example, 32768 is for half-speed slow motion playback.
     *
     * Negative value of speed is illegal since reverse playback is
     * not going to be supported, this function will return NFErr_Bad
     * if negative speed is specified.
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[in] speed the playback speed. 65536 represents the normal
     *            speed, and 0 means playback is paused. N means the
     *            playback speed is N/65536 of the normal speed.
     *
     * @return NFErr_OK if succeeded, otherwise NFErr_Bad.
     */
    virtual NFErr setSpeed(int32_t speed) = 0;

    /**
     * Flush the playback device, which includes stop
     * decoding/rendering audio/video immediately and discard buffered
     * data in or between all decoding/rendering stages.
     *
     * After flush, the playback speed is reset to paused, and the
     * end-of-stream state is cleared.
     *
     * Pre-condition: the device is currently opened.
     *
     * @return NFErr_OK if succeeded (normally it should always
     *         succeed).
     */
    virtual NFErr flush() = 0;

  /**
     * Flush the playback device audio pipeline only as a preparation step
     * for dynamic audio switch. All the buffered audio data (compressed or
     * decompressed) are discarded. The playback speed is unaffected, but
     * the end-of-stream state must be cleared.
     *
     * After the audio-flush, the device should guarantee that:
     *
     * 1. The video presentation keeps going in free-run mode.
     *
     * 2. The audio data delivered after this flush might have a PTS that's
     *    smaller than the presentation reference clock when samples are
     *    ready to be rendered. The audio renderer should be able to drop
     *    the outdated samples.
     *
     * Pre-condition: the device is currently opened.
     *
     * @return NFErr_OK if succeeded (normally it should always succeed).
     *         NFErr_NotAllowed if dynamic audio switch is not supported.
     */
    virtual NFErr flushAudio() = 0;

    /**
     * Notify the playback device that the bit-stream data feeding has
     * reached the end of stream.
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[in] type the stream type of the stream that has reached end of
     *            stream.
     *
     * @return NFErr_OK if succeeded (normally it should always
     *         succeed).
     */
    virtual NFErr endOfStream(StreamType type) = 0;

    /**
     * Get event (if any) from the device event queue (first posted
     * event gets retrieved first).
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[out] eventID the event identifier for the type of posted
     *             event.
     * @param[out] eventParam1 event parameter 1, actual meaning
     *             depends on the eventID.
     * @param[out] eventParam2 event parameter 2, actual meaning
     *             depends on the eventID.
     * @param[out] eventParam3 event parameter 3, actual meaning
     *             depends on the eventID.
     *
     * @return NFErr_OK if succeeded, otherwise NFErr_NotFound (event
     *         queue is empty)
     */
    virtual NFErr getEvent(EventID& eventID,
                           ullong& eventParam1,
                           ullong& eventParam2,
                           std::string& eventParam3) = 0;

    /**
     * Get the challenge data which will be used to acquire DRM
     * license of the media.
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[out] challengeData the challenge data from the media.
     * @param[out] drmHeader the drm header parsed from the media.
     *
     * @return NFErr_OK if successful, NFErr_Bad if something went
     *         wrong, NFErr_BadParameter if a parameter is invalid,
     *         NFErr_NotAllowed if the media header does not indicate
     *         DRM.
     */
    virtual TOBEDEPRECATED NFErr getChallenge(std::vector<unsigned char>& challengeData,
                                              std::vector<unsigned char>& drmHeader) = 0;

    virtual NFErr getChallenge(std::vector<unsigned char>& challengeData) = 0;

    /**
     * Store the acquired DRM license of the media.
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[in] licenseData the license data of the media.
     * @param[out] secureStopID to return the established secureStop ID.
     *
     * @return NFErr_OK if succeeded, otherwise NFErr_Bad.
     */
    virtual NFErr storeLicense(const std::vector<unsigned char>& licenseData,
                               std::vector<unsigned char>& secureStopID,
                               uint32_t& drmSessionId) = 0;

    /**
     * Clear the previously stored DRM licenses.
     *
     * Pre-condition: the device is currently opened.
     *
     * @return NFErr_OK if succeeded, otherwise NFErr_Bad.
     */
    virtual NFErr clearLicense() = 0;

    /**
     * Set the video window location.
     *
     * Without this function being called, the default behavior is for the
     * device to play in full-screen.
     *
     * To restore full-screen playback, the caller should specify (0,0,0,0)
     * as the target rectangle.
     *
     * The actual rectangle (as the result of this call) should be returned to
     * the caller. Depends on the limitations that the device might have, the
     * actual rectangle could be different from the target rectangle specified.
     *
     * The (x,y,width,height) values of the rectangles are specified in pixels,
     * and the (x,y) coordinates of the rectangles are relative to the
     * left-upper corner of the screen.
     *
     * A non-zero duration of transition may be specified to request transition
     * animation from the old video window location to the new video window
     * location (for example, zooming in or out). Depends on the device's
     * hardware capability, a specific device implementation can insert certain
     * number of 'intermediate' video window locations to smooth out the
     * transition.
     *
     * Pre-condition: the device is currently opened.
     *
     * @param[in] targetRect the target rectangle for the new video window, or
     *            (0,0,0,0) if full-screen playback is requested.
     * @param[in] transitionDuration the duration (in milliseconds) of the
     *            transition from the previous video window to the new target
     *            video window. 0 means the video window should be relocated to
     *            the target as soon as possible.
     *
     * @return the actual rectangle as a result of this call. If the caller use
     *         (0,0,0,0) as the target rectangle, a rectangle representing the
     *         full screen area should be returned.
     */
    virtual Rect setVideoWindow(Rect targetRect, uint32_t transitionDuration) = 0;

    /**
     * Bring the video presentation plane to above the UI graphic plane. As a
     * result, the video content (within the video window) will be visible to
     * the user, while the graphic content within the same area will be hidden
     * underneath the video content.
     *
     * Device implementation of this functionality is optional. For devices that
     * do not support video-over-graphic (i.e., 'supportVideoOverGraphics' is
     * set to false in ISystem::Capability), implementation of this function
     * should be left empty, while the video presentation plane is always below
     * the UI graphic plane.
     */
    virtual TOBEDEPRECATED void bringVideoToFront(){};

    /**
     * Send the video presentation plane to below the UI graphic plane. As a
     * result, the video content (within the video window) will be visible to
     * the user only if the graphic content within the same area has non-zero
     * transparency.
     *
     * Device implementation of this functionality is optional. For devices that
     * do not support video-over-graphic (i.e., 'supportVideoOverGraphics' is
     * set to false in ISystem::Capability), implementation of this function
     * should be left empty, while the video presentation plane is always below
     * the UI graphic plane.
     */
    virtual TOBEDEPRECATED void sendVideoToBack(){};

    /**
     * A general purpose method to execute private functions,
     * implementation is not required.
     *
     * @param[in] id command ID.
     * @param[in|out] param1 first parameter.
     * @param[in|out] param2 second parameter.
     */
    virtual NFErr execute(uint32_t id, uint32_t& param1, uint32_t& param2) = 0;

    struct PlaybackQualityStats
    {
        PlaybackQualityStats()
        {
            reset();
        }

        void reset()
        {
            mNumDecoded = -1;
            mNumSkipped = -1;
            mNumReducedQualityDecodes = -1;
            mNumDecoderErrors = -1;
            mNumRendered = -1;
            mNumDropped = -1;
            mNumRendererErrors = -1;
        }

        // Decoding Stats
        int mNumDecoded; // num frames decoded
        int mNumSkipped; // num frames skipped and not decoded
        int mNumReducedQualityDecodes;  // num frames decoded at a reduced quality
        int mNumDecoderErrors; // num frames causing decoder to report a non-fatal error

        // Rendering Stats
        int mNumRendered; // num frames rendered.
        int mNumDropped;  // num frames decoded but not rendered or rendered at least 1/2 frame period late.
        int mNumRendererErrors;  // num frames causing renderer to report a non-fatal error.
    };

    /**
     * @param[out] audioStats contains stats collected since the audio player was opened
     * @param[out] videoStats contains stats collected since the video player was opened
     */
    virtual void getPlaybackQualityStats(PlaybackQualityStats& audioStats,
                                         PlaybackQualityStats& videoStats) = 0;

    /**
     * getVolume
     */
    virtual double getVolume() = 0;

    /**
     * setVolume
     */
    virtual void setVolume(double targetVolume, uint32_t transitionDuration, IAudioMixer::Ease ease) = 0;
};

}} // namespace netflix::device

#endif // NRDP_IPLAYBACKDEVICE_H
