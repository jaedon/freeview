/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/** @file IElementaryStreamPlayer.h The elementary stream player interface
 *
 * IElementaryStreamPlayer defines an interface between the NRDP SDK and the
 * device implementation that does not require the device implementation to
 * parse a media container such as PIFF or MP4. With IElementaryStreamPlayer,
 * the SDK does the container parsing and delivers time-stamped samples of
 * encrypted elementary stream data directly to the device. In order to
 * implement IElementaryStreamPlayer, the device needs to be able to decrypt
 * encrypted samples, decode elementary stream samples, and render the
 * time-stamped samples in synchronization with other elementary streams in the
 * same presentation.
 *
 * Because audio and video streams are rendered together in synchronization,
 * stream players are created and controlled within the context of a playback
 * group so that common state between the audio and video player such as
 * reference clock and play or paused state can be shared. Playback groups are
 * created via a manager so that if multiple presentations need to be played at
 * the same time, the manager can coordinate resources such as decoders and
 * rendering devices that may be shared among groups. The manager also handles
 * content protection (DRM) functionality.
 *
 * Content protection (DRM) session initialization may occur while a stream is
 * already playing.  Unencrypted samples may be delivered for the initial
 * portion of a stream and playback may start before the license for the
 * encrypted portion of the stream is acquired.
 *
 * For the SDK's application to play back a presentation using
 * IElementaryStreamPlayer, it first uses IElementaryStreamManager to create a
 * playback group. Once the playback group is created, the SDK initializes an
 * audio and a video stream player into the group using
 * IPlaybackGroup::createStreamPlayer. For each stream player the SDK provides a
 * callback object that implements IESPlayerCallback.  The device's stream
 * player implementation uses the callback to pull media samples from the SDK,
 * report playback-related events, and report errors.
 *
 * Once created, the device's stream player implementation should immediately
 * begin pulling samples of media from the application, decrypting samples as
 * needed, decoding and buffering decoded media for rendering. The application
 * begins playback by trying to set the playback group's playback state to play
 * from pause. The device may return a code that indicates that it is not ready
 * to begin playback, and the application will wait some time and try again.
 */

#ifndef IELEMENTARYSTREAMPLAYER_H
#define IELEMENTARYSTREAMPLAYER_H

#include <nrdbase/tr1.h>
#include <nrdbase/StdInt.h>
#include <nrdbase/NFErr.h>
#include <vector>
#include <string>
#include "IAudioMixer.h"
#include "IDeviceError.h"


namespace netflix {
namespace device {
namespace esplayer {

enum MediaType {
    AUDIO = 0,
    VIDEO = 1
};

/**
 * Attributes specific to a video encoding.
 */
enum Format3D {
    NOT_3D,
    MVC_COMBINED,
    MVC_SPLIT
};

/**
 * VideoAttributes specify the properties of a video elementary stream.  They
 * are passed to the device implementation at the time a stream player is
 * created. Updates are also passed to the implementation with the first sample
 * (access unit) of each new fragment because the encoding attributes may be
 * different for the new fragment.
 */
struct VideoAttributes
{
    /**
     * "Codecs Parameter" string that specifies the stream codec.
     */
    std::string  mCodecParam;

    /** Frame rate is specified as a rational number.  Frame rate in seconds is
     *  given by mFrameRateValue / mFrameRateScale.
     *
     *  The SDK does not know the frame rate until a fragment header is
     *  parsed. A fragment header is not parsed until after
     *  PlaybackGroup::createStreamPlayer is used to create an elementary stream
     *  player. Therefore, the VideoAttributes structure furnished in the
     *  initData argument of PlaybackGroup::createStreamPlayer will not have a
     *  valid mFrameRateValue and mFrameRateScale. They will be set to 0 in this
     *  case. The values will be available with the VideoAttributes structure
     *  that is furnished with the first media sample that is delivered via
     *  IESPlayerCallback::GetNextMediaSample().
     */
    uint32_t mFrameRateValue;
    uint32_t mFrameRateScale;

    /**
     * 3D attributes. If this is not 3D video, this will be set to NOT_3D. For
     * 3D, width, height, and frame-rate attributes will be specified as
     * pertaining to each single view.
     *
     * For MVC, an mFormat3D of MVC_COMBINED indicates that access units will be
     * delivered with base and dependent nalus interleaved. A mFormat3D of
     * MVC_SPLIT indicates that access units will be delivered with the base
     * NALUs separated from dependent NALUs. For both MVC_COMBINED and
     * MVC_SPLIT, subsequence parameter set and the dependent-view picture
     * parameter set will be in a separate byte arrays.
     */
    Format3D mFormat3D;

    /** Image width in pixels */
    uint32_t mImageWidth;

    /** Image height in pixels */
    uint32_t mImageHeight;

    /** X component of the pixel aspect ratio*/
    uint32_t mPixelAspectRatioX;

    /** Y component of the pixel aspect ratio */
    uint32_t mPixelAspectRatioY;

    /** Bit depth for Luma component */
    uint32_t mBitsDepthLuma;

    /** Bit depth for chroma component */
    uint32_t mBitsDepthChroma;

    /**
     * Unencrypted byte arrays of configuration data specific to the video
     * encoding. The format of the data is codec-specific.  Here we define the
     * data format for the case of ISO/IEC 14496-10-2010 (AVC/MVC) and 14496-2
     * (MPEG-4) video.
     *
     * For AVC, the mCodecSpecificData vector is of length 1 where
     * mCodecSpecificData[0] contains the sequence parameter set NAL unit
     * followed by the picture parameter set NAL unit, each preceded by a 4-byte
     * Annex B start code 0x00000001. For MVC in MVC_COMBINED mode,
     * mCodecSpecificData is of length 1 and mCodecSpecificData[0] contains the
     * sequence parameter set for the base view, followed by the subsequence
     * parameter set for the dependent view, the picture parameter set for the
     * base view, and the picture parameter set for the dependent view. Each
     * NALU is preceded by a 4-byte Annex B start code.  mCodecSpecificDataLen
     * contains the lengths of the byte arrays. For MVC_SPLIT mode, the
     * mCodecSpecificData vector is of length 2. mCodecSpecificData[0] contains
     * the sequence parameter set and picture parameter set for the base view
     * and mCodecSpecificData[1] contains the subsequence picture parameter sets
     * for the dependent view.
     *
     * For MPEG4 video, the
     * mCodecSpecificData is of length 0.
     */
    std::vector<const uint8_t*> mCodecSpecificData;
    std::vector<uint32_t> mCodecSpecificDataLen;
};

/* Attributes specific to an audio encoding. */
  struct AudioAttributes {
    /**
     * "Codecs Parameter" string as in
     * http://tools.ietf.org/search/draft-gellens-mime-bucket-bis-09 as the
     * string is defined in the context of an ISO file. The string will be in
     * the form of a simp-list production defined in
     * http://tools.ietf.org/search/draft-gellens-mime-bucket-bis-09, without
     * the enclosing DQUOTE characters.
     *
     * For example, HE-AAC would be expressed as
     *
     *     mp4a.40.2, mp4a.40.5
     *
     * since the HE profile contains both AAC-LC and SBR. Dolby Digital Plus
     * would be expressed as
     *
     *      ec-3.A6
     */
    std::string mCodecParam;

    /** Number of channels */
    uint32_t mNumberOfChannels;

    /** Sampling frequency */
    uint32_t mSamplesPerSecond;

    /** Bit rate */
    uint32_t mBitrate;

    /** Block Align */
    uint32_t mBlockAlignment;

    /**
     * Byte array of configuration data for the video encoding.  The format of
     * the data is codec-specific. For HE-AAC, the data mCodecSpecificData is
     * the AudioSpecificConfig data as defined in 14496-3-2009 Sec. 1.6.2.1.
     * The length of the codec specific data is given by mCodecSpecificDataLen;
     */
    const uint8_t *mCodecSpecificData;
    uint32_t mCodecSpecificDataLen;
};


/**
 *  MediaAttributes describe a media encoding.
 */
struct MediaAttributes
{
    /* MediaType - AUDIO or VIDEO */
    MediaType mStreamType;

    /**
     * If the mStreamType is audio, the mVideoAttributes field will be set to
     * NULL and the mAudioAttributes field will be valid. If the mStreamType is
     * video, the mVideoAttributes will be valid and mAudioAttributes will be
     * NULL.
     */
    AudioAttributes *mAudioAttributes;
    VideoAttributes *mVideoAttributes;
};

/**
 * An elementary stream player is created and initialized based on the
 * StreamPlayerInitData that is passed into
 * IPlaybackGroup::createStreamPlayer().
 */
struct StreamPlayerInitData {

    /* The media attributes of the initial elementary stream that will be fed to
     *  this stream player.
     */
    MediaAttributes *mInitialStreamAttributes;

    /* The maximum image width that a video stream player may need to handle.
     * For audio streams, this field is undefined.
     */
    uint32_t         mMaxWidth;

    /* The maximum image height that a video stream player may need to
     * handle. For audio streams, this field is undefined.
     */
    uint32_t         mMaxHeight;


    /* The maximum frame rate that a video stram player may need to handle.
     * max video frame rate is mMaxVideoFrameRateValue/mMaxVideoFrameRateScale.
     * For audio streams, this filed is undefined
     */
    uint32_t         mMaxVideoFrameRateValue;
    uint32_t         mMaxVideoFrameRateScale;

    /* Protection-system-specific data from the DASH ProtectionSystemHeader
     * box. Needed to initialize decryption context.
     */
    const uint8_t*   mDrmHeader;
    uint32_t         mDrmHeaderLen;

    /* Color information for display configuration
     * Value is based on ISO/IEC 23008-2 Table E-3, E-4, E-5.
     * When not available, mFullRange is set 0, while others are set with -1.
     */
    int32_t mColorPrimaries;
    int32_t mTransferCharacteristics;
    int32_t mMatrixCoefficient;
    int32_t mFullRange;
};

/**
 * The properties of samples (referred to as access units in the context of
 * H.264 AVC and MVC) are provided to stream player implementations via the
 * ISampleAttributes interface.
 *
 * The interface refers to views and viewNum. These pertain to MVC video
 * delivered in MVC_SPLIT mode. For audio, 2D video, and MVC_COMBINED mode MVC,
 * there is only one "view", indexed by viewNum 0. In the case when MVC is provided to
 * the device in MVC_SPLIT mode with base and dependent NALUs separated, there
 * are two views. The viewNum 0 pertains to the base view NALUs, viewNum
 * 1 to the dependent.
 */
class ISampleAttributes
{
public:
    virtual ~ISampleAttributes() {}

    /**
     * @return contentId
     */
    virtual std::string getContentId() const = 0;

    /**
     * @return 1 except for the case when the sample is MVC and the MVC stream
     *         player has been initialized in MVC_SPLIT. In the case of
     *         MVC_SPLIT, getNumViews will return 2.
     */
    virtual uint32_t getNumViews() const = 0;

    /*
     * @param[in] viewNum should be 0 except for the case when the elementary
     *            stream player has been initialized to receive MVC samples in
     *            MVC_SPLIT mode.  In this case, viewNum 0 will select the
     *            buffer containing the base view NALUs and viewNum 1 will
     *            select the buffer containing the dependent view NALUs.
     *
     * @return the length of the sample data in the byte buffer or 0 if
     *         viewNum is out of range.
     */
    virtual uint32_t getSize(uint32_t viewNum = 0) const = 0;

    /**
     * @return if bigger than 1, the discontinuous time gap between current
     *         sample and previous sample.
     */
    virtual llong getDiscontinuityGapInMs() const = 0;

    /** @return the presentation time stamp for this sample. PTS values are in
     *          units of the timescale returned by getTimescale(). PTS values
     *          for the samples of a presentation start at 0.
     */
    virtual llong getPTS() const = 0;

    /** @return the decoding time stamp for this sample. DTS values are
     *          referenced to time 0 which is the time at the begining of the
     *          presentation. */
    virtual llong getDTS() const = 0;


    /* @return the timescale of the PTS and DTS values returned by getPTS and
     *         getDTS.  For example, getPTS() / getTimescale() is in units of
     *         seconds.
     **/
    virtual int32_t getTimescale() const = 0;

    /**
     * @return true only if the sample is an independently decodable (e.g. IDR)
     *         frame where all subsequent frames in presentation order are also
     *         later in decode order and are fully decodable without samples
     *         that precede the key frame in decode order.
     */
    virtual bool isKeyFrame() const = 0;

    /**
     * @return true only if the sample is Enhanced layer frame of Dolby Vision
     *
     */
    virtual bool isDolbyVisionELFrame() const = 0;

    /**
     * Sample encryption
     *
     * @return an identifier of the encryption algorithm used to encrypt the
     *         track. 0: not encrypted, 1: AES 128-bit in CTRmode. If
     *         algorithmID is 0, IVData_, keyID, and subsampleMapping_ should be
     *         ignored.
     */
    virtual uint32_t getAlgorithmID() const = 0;

    /**
     * @return the initialization vector required for decryption of the sample.
     */
    virtual uint8_t* getIVData() const = 0;

    /**
     * @param[in] viewNum should be 0 except for the case when the elementary
     *            stream player has been initialized to receive MVC samples in
     *            MVC_SPLIT mode.  In this case, viewNum 0 will select the
     *            IVData size for the base view NALUs and viewNum 1 will
     *            select the IVData size for the dependent view NALUs.
     *
     * @return the size in bytes of the IVData field.
     */
    virtual uint32_t getIVSize() const = 0;

    /*
     * @return DRM key identifier that uniquely identifies the key needed to decrypt
     *         the sample.
     */
    virtual std::vector<uint8_t> getKeyID() const = 0;

    /* Each encrypted elementary stream sample contains portions of clear and
     * portions of encrypted content. The subsample mapping describes which
     * portions are clear and which portions are encrypted. It is a succession
     * of integer pairs indicating clear and encrypted byte lengths (ex. "4, 6,
     * 2, 0" would describe a 12-byte sample which had 4 bytes of clear, 6 bytes
     * of encrypted, 2 bytes of clear, 0 bytes of encrypted).
     *
     * @param[in] viewNum should be 0 except for the case when the elementary
     *            stream player has been initialized to receive MVC samples in
     *            MVC_SPLIT mode.  In the MVC_SPLIT case, viewNum 0 will select
     *            the subsample mapping for the base view NALUs and viewNum 1
     *            will select the subsample mapping for the dependent view
     *            NALUs.
     *
     * @return the subsample mapping array.
     */
    virtual const std::vector<uint32_t>& getSubsampleMapping(uint32_t viewNum = 0) const = 0;

    /**
     * For AVC or MVC streams, returns a vector containing the byte-sizes of the
     * sequence of NALUs that make up the access unit. The sizes include the
     * 4-byte Annex B start code.
     *
     * For media types other than AVC or MVC, returns an empty vector.
     */
    virtual const std::vector<uint32_t>& getNaluSizes(uint32_t viewNum = 0) const = 0;

    /**
     * A stream player may wish to skip the decoding of a sample. It may do so
     * to maintain A/V synch, for example. Because of prediction dependencies
     * however, skipping the decoding of a sample may impact whether it is
     * possible to correctly decode subsequent samples. The isDependedOn method
     * provides information about the impact that skipping the decoding of a
     * sample will have.
     *
     * @return true if the picture is used for reference, false otherwise. For
     * CE3 video, will return false only if the sample is a B-picture.
     */
    virtual bool isDependedOn() const = 0;

    /**
     * @return the new media attributes if the encoding has changed. Returns
     *         NULL unless the media attributes have changed since the last
     *         sample.  For video, media attributes will only change on IDR
     *         samples.
     */
    virtual const MediaAttributes* getMediaAttributes() const = 0;
};


/*
 * ISampleWriter allows the device implementation to define how sample (access
 * unit) data is transfered between the SDK and the device. The device provides
 * the SDK with an object implementing ISampleWriter and the SDK uses it to
 * transfer sample data to the device.
 *
 * To use ISampleWriter, the SDK first calls initSample(). This allows the
 * writer implementation to see the sample properties, including the size,
 * before any data is written. If the implementation returns OK the SDK will
 * then call write() one or more times to deliver the sample data. The sum of
 * write sizes will be equal to the sample size specified in the pSample.
 */
class ISampleWriter
{
public:
    virtual ~ISampleWriter()
    {
    }

    /**
     * @param[in]: sampleAttributes an interface to an ISampleAttributes object
     *             that provides attributes of the media sample including the
     *             size of the sample. The sampleAttributes and data it returns
     *             are only valid if initSample returns OK. The remain
     *             valid until the next time initSample() is called.
     *
     * @return  NO_AVAILABLE_BUFFERS if the device cannot currently write the sample.
     *          UNEXPECTED_MEDIA_ATTRIBUTES if there is a problem with the pSample attributes.
     *          ERROR if there is an unrecoverable writer error
     *          OK if sample writer successfully initializes to write the current sample.
     */
    virtual Status initSample(const ISampleAttributes& sampleAttributes) = 0;

    /* @param[in] ptr a pointer to the media sample bytes to be written.  The
     *            pointer data is only valid until write() returns.
     *
     * @param[in] size the number of bytes that should be written.
     * @param[in] viewNum should be 0 unless the stream player was created in MVC_SPLIT mode.
     *            In MVC_SPLIT mode, writes to the base (left) view are specified with
     *            viewNum = 0, writes to the dependent (right) view are specified with
     *            viewNum = 1.
     * @return number of bytes successfully written.
     * */
    virtual uint32_t write(const uint8_t *ptr, uint32_t size, uint32_t viewNum = 0) = 0;

};



/**
 * The application implements an ESPlayerCallback object to allow the device
 * to pull elementary stream samples from the application, notify the
 * application of playback-related events such as a change in playback position
 * or underflow, and to deliver error notifications to the application.
 *
 * Reentrancy: The ESPlayerCallback defined by the application will call
 * ISampleWriter methods from within callback methods. Likewise, the callback
 * methods on the callback may call IElementaryStreamPlayer and IPlaybackGroup
 * methods such as IElementaryStreamPlayer::disable/enable.  The device
 * implementations of IElementaryStreamPlayer, IPlaybackGroup, and
 * IElementaryStreamManager methods should not call callback methods.
 */
class IESPlayerCallback {
public:
    virtual ~IESPlayerCallback() {}

    /**
     * The device pulls samples from the SDK by calling getNextMediaSample.
     * getNextMediaSample takes as an argument an ISampleWriter that the device
     * has to implement. In turn, getNextMediaSample passes the sample writer an
     * ISampleAttributes interface when it calls ISampleWriter::initSample. The
     * ISampleAttributes passed to the sample writer is only valid until
     * getNextMediaSample returns. Likewise, any structures or data returned by
     * ISampleAttributes methods are only valid until getNextMediaSample
     * returns.
     *
     * Each time getNextMediaSample is called, the SDK uses the device's
     * ISampleWriter implementation to deliver the next media sample (referred
     * to as access units in the H.264 standard) in the presentation. Samples
     * are delivered in decoding order. The getNextMediaSample method will not
     * block but may return with a code, NO_AVAILABLE_SAMPLES indicating that no
     * sample was written. If getNextMediaSample() returns NO_AVAILABLE_SAMPLES,
     * the implementation should wait some time and attempt to get the sample
     * again.
     *
     * When the ISampleAttributes used to initialize the sampleWriter contains
     * codec-specific data such as H.264 sequence and picture parameter sets,
     * the SDK will prepend these bytes to the sample using the
     * sampleWriter. For example, the SDK will prepend H.264 sequence and
     * picture parameter sets to IDR samples. Similarly, by default the SDK will
     * prepend ADTS headers to AAC samples.
     *
     * @param[in] sampleWriter: An object implementing ISampleWriter that the
     *            SDK will use to write the sample data to the device. The SDK
     *            will only use the sampleWriter within the particular
     *            getNextMediaSample call and will not keep a reference to the
     *            object after getNextMediaSample has returned.
     *
     * @return OK if a sample is retrieved successfully
     *         NO_AVAILABLE_SAMPLES if there is currently no available
     *             sample to retrieve and not all samples in the presentation
     *             have been delivered.
     *         END_OF_STREAM if all the samples in the presentation have
     *             been delivered.
     *         STREAM_ERROR if there has been an unrecoverable error
     *             retrieving a sample.
     *         NO_AVAILABLE_BUFFERS if the writer returns no available buffers.
     *         ERROR if there is some other writer error.
     */
    virtual Status getNextMediaSample(ISampleWriter& sampleWriter) = 0;

    /**
     * With getNextMediaSampleSize, the device can inquire about the size of the
     * sample that will be delivered upon the next call to
     * getNextMediaSample(). Calling getNextMediaSampleSize() any number of
     * times will not "dequeue" the next sample that will be delivered when
     * getNextMediaSample() is called. An implementation can use this method to
     * check the size of the next media sample before calling
     * getNextMediaSample() to actually receive the sample.
     *
     * @param[out] size: when the method returns OK, size will contain the size
     *             of the next media sample that will be delivered via
     *             getNextMediaSample().
     *
     * @param[in]  viewNum: for 3D streams delivered in separate buffers for
     *             base and dependent views (MVC_SPLIT), viewNum specifies which
     *             view's size should be returned.
     *
     * @return OK if a sample is retrieved successfully
     *         NO_AVAILABLE_SAMPLES if there is currently no available
     *             sample to retrieve and not all samples in the presentation
     *             have been delivered.
     *         END_OF_STREAM if all the samples in the presentation have
     *             been delivered.
     *         STREAM_ERROR if there has been an unrecoverable error
     *             retrieving a sample.
     *         ERROR if there is some other writer error.
     */
    virtual Status getNextMediaSampleSize(uint32_t& size, uint32_t viewNum = 0) = 0;


    /**
     * With getNextMediaSampleTimestamps, the device can inquire about the
     * decoding time stamp (DTS) and presentation timestamp (PTS) of the sample
     * that will be delivered upon the next call to
     * getNextMediaSample(). Calling getNextMediaSampleTimestamps() any number
     * of times will not "dequeue" the next sample that will be delivered when
     * getNextMediaSample() is called. An implementation can use this method to
     * check the DTS or PTS of the next media sample before calling
     * getNextMediaSample() to actually receive the sample.
     *
     * @param[out] dts: when the method returns OK, dts will contain the
     *             decoding time stamp (DTS) of the next media sample that will
     *             be delivered via getNextMediaSample().
     *
     * @param[out] pts: when the method returns OK, pts will contain the
     *             presentation time stamp (PTS) of the next media sample that
     *             will be delivered via getNextMediaSample().
     *
     * @return OK if a sample is retrieved successfully
     *         NO_AVAILABLE_SAMPLES if there is currently no available
     *             sample to retrieve and not all samples in the presentation
     *             have been delivered.
     *         END_OF_STREAM if all the samples in the presentation have
     *             been delivered.
     *         STREAM_ERROR if there has been an unrecoverable error
     *             retrieving a sample.
     *         ERROR if there is some other writer error.
     */
    virtual Status getNextMediaSampleTimestamps(llong& dts,
                                                llong& pts) = 0;

    /**
     * Inform application of the current playback position.  The timestamp is in
     * units of millisecond.
     *
     * The event can be reported as often as every time a sample is rendered, or
     * at a minimum 4 times a second when the playback state is PLAY. During a
     * period when the playback state is PAUSE, at most one update playback
     * position should be called.
     *
     * @param[in] timestamp the current value of the playback clock position
     *            in milliseconds.
     */
    virtual void updatePlaybackPosition(const llong& timestamp) = 0;

    /**
     * Report playback pipeline underflow.
     *
     * If a stream player runs out of playback data while its PlaybackGroup is
     * in a non-paused state and the player has not been disabled via
     * IElementaryStreamPlayer::disable(), this function must be immediately
     * called to report the underflow (NOTE: The SDK application will never
     * disable a video IElementaryStreamPlayer, only audio, as of the 3.2 SDK).
     *
     * After the underflow has been reported, it should not be reported again
     * until one of the following sequences occurs:
     *
     * 1. IPlaybackGroup::flush() -> ... -> IPlaybackGroup::setPlaybackState(PLAY)
     * 2. IElementaryStreamPlayer::flush() -> ...
     *    ... -> IPlaybackGroup::setPlaybackState(PLAY)
     * 3. IPlaybackGroup::setPlaybackState(PAUSE) -> ... ->
     * IPlaybackGroup::setPlaybackState(PLAY)
     *
     * If the playback group is in the PAUSE state, reportUnderflow() must not
     * be called.
     */
    virtual void reportUnderflow() = 0;

    /**
     * Indicates whether the SDK has received reportUnderflow(). Provided for
     * convenience to assist a device implementation in tracking whether it has
     * called reportUnderflow(). If a device has run out of playback data, the
     * player has not been disabled via IElementaryStreamPlayer::disable(), and
     * the player is in a non-paused state, it should call reportUnderflow() if
     * reportUnderflowReceived() returns false.
     *
     * @return true if reportUnderflow() has been called since
     *         1) IPlaybackGroup::flush() -> ... -> IPlaybackGroup::setPlaybackState(PLAY),
     *         2) IElementaryStreamPlayer::flush() -> ... -> IPlaybackGroup::setPlaybackState(PLAY), or
     *         3) IPlaybackGroup::setPlaybackState(PAUSE) -> ... -> IPlaybackGroup::setPlaybackState(PLAY)
     *         false otherwise
     */
    virtual bool reportUnderflowReceived() = 0;

    /**
     * When getNextMediaSample() has returned END_OF_STREAM playbackCompleted()
     * should be reported once when all playback data has been decoded and
     * rendered. playbackCompleted should not be reported again until either of
     * the following occur:
     *
     * 1) IElementaryStreamPlayer::flush() ->...-> setPlaybackState(PLAY),
     * 2) IPlaybackGroup::flush() -> ... -> setPlaybackState(PLAY)
     */
    virtual void playbackCompleted() = 0;

    /**
     * Indicates whether the SDK has received
     * playbackCompletedReceived(). Provided for convenience to assist a device
     * implementation in tracking tracking whether it has called
     * playbackCompleted(). If a device has run out of playback data, the player
     * has not been disabled via IElementaryStreamPlayer::disable(), and the
     * playback group is in a non-paused state, the player should call
     * reportUnderflow() if reportUnderflowReceived() returns false.
     *
     * @return true if reportUnderflow() has been called since
     *         1) IElementaryStreamPlayer::flush() ->...-> setPlaybackState(PLAY),
     *         2) IPlaybackGroup::flush() -> ... -> setPlaybackState(PLAY)
     *         false otherwise
     */
    virtual bool playbackCompletedReceived() = 0;

    /**
     * Report an unrecoverable playback error.
     *
     * When creating a IDeviceError object, errorCategory must be one of the following Status codes
     *
     *    UNEXPECTED_MEDIA_ATTRIBUTES if ISampleAttributes::getMediaAttributes()
     *        returns attributes describing a media type not supported by the
     *        player.
     *
     *    STREAM_ERROR if ESPlayerCallback::getNextMediaSample() returns
     *         STREAM_ERROR or the player encounters an error in a delivered
     *         elementary stream sample before the sample is decoded.
     *
     *    DECRYPTION_ERROR if there is an error decrypting the sample
     *
     *    DECODE_ERROR if there is an error decoding the sample
     *
     *    ERROR if there is any other error.
     *
     * @param[in] IDeviceError or NFErr created for above errorCategory
     *
     */
    virtual void reportError(IDeviceError err) = 0;

    virtual void reportError(NFErr err) = 0;

    static const int STAT_NOT_SET = -1;
    /**
     * Report statistics about frames decoded and skipped. The arguments should
     * be the number of decoded frames, skipped frames, etc, since the previous
     * call to updateDecodingStats or since the elementary stream player was
     * created, if updateDecodingStats has not been called before on this
     * callback.
     *
     * If the device does not support reporting of non-fatal errors or reduced
     * quality decodes, the arguments should be set to STAT_NOT_SET, not 0. This
     * will allow the SDK to distinguish between the cases where errors or
     * low-quality decodes occurred and the cases where the device does not
     * track these values.
     *
     * @param[in] numDecoded: num frames decoded.
     * @param[in] numSkipped: num frames skipped and not decoded. If the device
     *            does not skip frames, the value STAT_NOT_SET should be used
     *            for this argument.
     * @param[in] numReducdedQualityDecodes: num frames decoded at a reduced quality.
     *            For example, the H.264 video decoder did not execute in-loop
     *            deblocking to decrease up decoding time. If the device does
     *            not implement any decoding "short-cuts" that reduce decoded
     *            quality, the value STAT_NOT_SET should be used for this
     *            argument.
     * @param[in] numErrors: num frames causing decoder to report a non-fatal
     *            error. If the device does not report non-fatal decoding errors
     *            on a frame-by-frame basis, the value STAT_NOT_SET should be
     *            used for this argument.
     */
    virtual void updateDecodingStats(int numDecoded, int numSkipped,
                                     int numReducedQualityDecodes,
                                     int numErrors) = 0;

    /**
     * Report statistics about frames rendered or dropped. The arguments should
     * be the number of rendered frames, drop frames, etc, since the previous
     * call to updateRenderingStats or since the elementary stream player was
     * created, if updateRenderingStats has not been called before on this
     * callback.
     *
     * If the device does not support tracking of non-fatal errors, the argument
     * should be set to STAT_NOT_SET, not 0. This allows the SDK to distinguish
     * between the case where no errors occurred and the case where the device
     * does not report non-fatal rendering errors.
     *
     * @param[in] numRendered: num frames rendered.
     * @param[in] numDropped: num frames decode but not rendered or rendered at 1/2 period late.
     * @param[in] numErrors: num frames causing renderer to report a non-fatal
     *            error. If the device does not report non-fatal rendering errors
     *            on a frame-by-frame basis, the value STAT_NOT_SET should be
     *            used for this argument.
     */
    virtual void updateRenderingStats(int numRendered, int numDropped,
                                      int numErrors) = 0;

};

/**
 * Interface allowing the SDK to control the stream player directly instead of
 * via the PlaybackGroup. The interface is always created via
 * IPlaybackGroup::createStreamPlayer().
 */
class IElementaryStreamPlayer
{
public:
    virtual ~IElementaryStreamPlayer()
    {
    }

    /**
     * Flush the stream player. The stream player should stop decoding and
     * rendering immediately and should discard all buffered data in all
     * decoding and rendering stages.
     *
     * Concurrency: IElementaryStreamPlayer::flush() is called on an SDK media
     * control thread. Decoding and rendering occurs on a thread or threads
     * created by the device implementation. If the device calls
     * IESPlayerCallback methods on those threads for events that occur before
     * the player is flushed, IElementaryStreamPlayer::flush() must not return
     * before the IESPlayerCallback methods return.  For instance, if the device
     * encounters the conditions which cause it to initiate a call to
     * IESPlayerCallback::playbackComplete() shortly before or while a
     * IElementaryStreamPlayer::flush() is called, the
     * IESPlayerCallback::playbackComplete() call must return before
     * IElementaryStreamPlayer::flush() returns.
     *
     * NOTE: For SDK version 4.0, only an audio stream player needs to support
     * flush in the IElementaryStreamPlayer.  In 4.0 video streams will never be
     * flushed independently, only jointly with the audio streams via the
     * IPlaybackGroup::flush().
     */
    virtual void flush() = 0;

    /**
     * Disable or enable playback and rendering on the player.  When a stream is
     * disabled, its output is not rendered. In addition, when the stream is in
     * the disabled state, an underflow event should not be triggered when the
     * playback pipeline runs out of data. When the stream is initialized, it
     * should be in the enabled state by default.
     *
     * NOTE: For SDK version 4.0, only the audio stream needs to support
     * enable(), disable(), and isDisabled(). When the audio stream is disabled,
     * it is then flushed.  After the flush, the SDK will enable the audio
     * player.  The player should begin playing back when it has received enough
     * samples to begin playback.
     */
    virtual void disable() = 0;
    virtual void enable() = 0;
    virtual bool isDisabled() = 0;

    /**
     * setVolume sets volume to arbitrary value with fade effect
     * @param[in] target: target volume. 0 to 1
     * @param[in] duration: duration in milliseconds to reach target percentage
     *                      from current volume percentage
     * @param[in] ease: easing function
     * @return void : UI does not rely on any return value of this DPI since
     *                UI checks ISystem::capability flags before calling this method.
     *                It is device manufacturer's responsibility to behave exactly
     *                as advertised in ISystem::Capability.
     */
    virtual void setVolume(double   target /* target */,
                           uint32_t duration = 0 /* in milliseconds */,
                           IAudioMixer::Ease ease = IAudioMixer::EaseLinear /* easing function */) = 0;

    /**
     * getVolume returns current volume in percentage
     * @return percentage of current volume
     */
    virtual double getVolume() = 0;
};


/**
 * An implementation of IPlaybackGroup encapsulates state common to elementary
 * stream players that are rendering the same presentation. It also provides a
 * single point where playback of the overall presentation is controlled.
 *
 * A playback group is always created via
 * IElementaryStreamManager::createPlaybackGroup().
 **/
class IPlaybackGroup
{
public:
    virtual ~IPlaybackGroup() {}

    /**
     * Create a stream player object within this playback group. The
     * player implementation should have access to the group's synchronization
     * clock and playback (paused or play) state. For instance, when the group's
     * playback state is set, the player implementations should respond by
     * rendering or not rendering frames.
     *
     * When initializing a new elementary stream player for MVC playback, the
     * IESPlayerCallback interface can be initialized to provide base and
     * dependent view NALUs interleaved in a single sample buffer or it can be
     * initialized to provide base and dependent view NALUs in separate
     * buffers. By default, the SDK will attempt to initialize an
     * IESPlayerCallback that delivers base and dependent view NALUs
     * interleaved in a single sample buffer. The mFormat3D of the
     * VideoAttributes will be set to MVC_COMBINED in this case. If
     * createStreamPlayer returns SPLIT_MVC_REQUIRED, the SDK
     * will attempt to initialize a stream player callback that delivers
     * base and dependent NALUs in separate buffers.  The mFormat3D of the
     * VideoAttributes will be set to MVC_SPLIT in this case.
     *
     * @param[in] initData: provides data needed to initialize the stream player.
     * @params[in] callback: is a object that implements IESPlayerCallback;
     * @params[out] streamPlayer: If createStreamPlayer() returns OK,
     *                            streamPlayer should be a valid pointer to an
     *                            IElementaryStreamPlayer object that has been
     *                            created. If createStreamPlayer() returns
     *                            something other than OK, streamPlayer should
     *                            be NULL. When the SDK is finished with the
     *                            player, the SDK will call IPlaybackGroup::
     *                            destroyStreamPlayer() on the streamPlayer.
     *
     *  @return NFErr_OK on success, if fails, return NFErr object with
     *          IDeviceError. When it fails, inspect errorCategory from
     *          IDeviceError. SPLIT_MVC_REQUIRED if the VideoAttributes in the
     *          initData argument specifies MVC_COMBINED but MVC_SPLIT is
     *          required.  UNEXPECTED_MEDIA_ATTRIBUTES if the media specified is
     *          not playable on the device INITIALIZATION_ERROR for other
     *          errors.
     */
    virtual NFErr createStreamPlayer(const struct StreamPlayerInitData& initData,
                                     shared_ptr<IESPlayerCallback> callback,
                                     IElementaryStreamPlayer*& streamPlayer) = 0;

    /**
     * Destroy a stream player.  The SDK will call this when it is finished with a
     * stream player that it has created via IPlaybackGroup::createStreamPlayer().
     * IPlaybackGroup::destroyStreamPlayer() also signals the device
     * implementation to delete decryption contexts allocated to the player.
     */
    virtual void destroyStreamPlayer(IElementaryStreamPlayer* streamPlayer) = 0;


    enum PlaybackState {
        PAUSE = 0,
        PLAY = 1
    };

    /**
     * Set the playback state. This controls whether rendering is paused or
     * playing for the stream players in the group. When setPlaybackState(PLAY)
     * is called, the device might not be ready to begin playback.  For
     * instance, the device not have received and decoded enough samples of
     * audio or video to begin playing back without stuttering. In this case,
     * setPlaybackState() return false.  The SDK will then wait a few ms and
     * attempt to set the state to play again.  If the SDK tries to set the
     * state to PLAY for 30 s and device continues to return false, the SDK will
     * abort playback.
     *
     * @param[in] state: the playback state.
     *
     * @return true if te state is succesfully changed. In the case when the
     *         state is being set to PLAY, this means that all playback
     *         pipelines were ready to begin playing back. Returns false
     *         otherwise.
     */
    virtual bool setPlaybackState(PlaybackState state) = 0;

    /**
     * Get the playback state for the stream players in the group.
     *
     * @return current playback state.
     */
    virtual PlaybackState getPlaybackState() = 0;

    struct Rect
    {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };


    /**
     * When flush() is called on the playback group, the playback state should
     * be set to paused and all stream players in the group should be flushed.
     *
     * When a stream player in the group is flushed, it should stop decoding and
     * rendering immediately and should discard all buffered data in all
     * decoding and rendering stages.
     *
     * Concurrency: IPlaybackGroup::flush() is called on an SDK media control
     * thread. Decoding and rendering occurs on a thread or threads created by
     * the device implementation. If the device calls IESPlayerCallback methods
     * on its threads for events that occur before the players are flushed,
     * IPlaybackGroup::flush() must not return before the IESPlayerCallback
     * methods return.  For instance, if the device encounters the conditions
     * which cause it to initiate a call to
     * IESPlayerCallback::playbackComplete() shortly before or while a
     * IPlaybackGroup::flush() is called, the
     * IESPlayerCallback::playbackComplete() call must return before
     * IPlaybackGroup::flush() returns.

     */
    virtual void flush() = 0;

    /**
     * Set the video window location.
     *
     * The default behavior is for the
     * device to play in full-screen.
     *
     * To restore full-screen playback, the caller should specify (0,0,0,0)
     * as the target rectangle.
     *
     * The actual rectangle (as the result of this call) should be returned to
     * the caller. Depending on the limitations that the device might have, the
     * actual rectangle could be different from the target rectangle specified.
     *
     * The (x,y,width,height) values of the rectangles are specified in pixels,
     * and the (x,y) coordinates of the rectangles are relative to the
     * left-upper corner of the screen.
     *
     * A non-zero duration of transition may be specified to request transition
     * animation from the old video window location to the new video window
     * location (for example, zooming in or out). Depending on the device's
     * hardware capability, a device implementation can insert intermediate
     * video window locations to smooth the transition.
     *
     * Pre-condition: a video stream player has been created via
     * createStreamPlayer().
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
    virtual Rect setVideoWindow(const Rect& targetRect,
                                uint32_t transitionDuration,
                                uint32_t zorder = 0) = 0;

    /**
     * Order the video presentation plane above the UI graphic plane. As a
     * result, the video content (within the video window) will be visible to
     * the user, while the graphic content within the same area will be hidden
     * underneath the video content.
     *
     * Device implementation of this functionality is optional. For devices that
     * do not support video display over the UI graphics plane, the
     * implementation of this function should be left empt and the video
     * presentation plane should always be below the UI graphic plane.
     */
    virtual void bringVideoToFront() = 0;

    /**
     * Send the video presentation plane to below the UI graphic plane. As a
     * result, the video content (within the video window) will be visible to
     * the user only if the graphic content within the same area has non-zero
     * transparency.
     *
     * Device implementation of this functionality is optional. For devices that
     * do not support video display over the UI graphic plane, implementation of
     * this function should be left empty and the video presentation plane
     * should always be below the UI graphic plane.
     */
    virtual void sendVideoToBack() = 0;

};


/**
 * IElementaryStreamManager is an interface to a manager object that controls
 * the creation and playback state of IPlaybackGroup objects. The manager also
 * implements IDRMManager.
 *
 * The IElementaryStreamManager allows multiple groups to be created and
 * destroyed.  The implementation could then support simultaneous playback of
 * multiple streams while being able to manage shared memory, decoding, and
 * rendering resources.
 */
class IElementaryStreamManager
{
public:
    virtual ~IElementaryStreamManager() {}

    /**
     * Create common state needed to control streams that will play back in
     * synchronization.  This may include a common reference clock and common
     * playback state, PAUSE or PLAY.
     *
     * @params[out] playbackGroup: If createPlaybackGroup() returns OK,
     *                             playbackGroup should contain a valid pointer
     *                             to an IPlaybackGroup object that has been
     *                             created.  When it is done with the playback
     *                             group, the SDK will call
     *                             IElementaryStreamManager::destroyPlaybackGroup()
     *                             on playbackGroup. If createPlaybackGroup()
     *                             returns something other than OK,
     *                             playbackGroup should be NULL.
     *
     * @return NFErr_OK on success
     */
    virtual NFErr createPlaybackGroup(IPlaybackGroup*& playbackGroup, uint32_t pipelineId = 0 )
    {
        NRDP_UNUSED(playbackGroup);
        NRDP_UNUSED(pipelineId);
        return NFErr_NotImplemented;
    }

    /**
     * Destroy a playback group.  The SDK will call this when it is done with a
     * playback group that it has created via
     * IElementaryStreamManger::createPlaybackGroup().
     */
    virtual void destroyPlaybackGroup(IPlaybackGroup* playbackGroup) = 0;
};

} // esplayer
} // device
} //netflix

#endif
