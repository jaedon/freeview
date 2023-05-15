/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef DEVICE_STREAMATTRIBUTES_H
#define DEVICE_STREAMATTRIBUTES_H

#include <nrd/IElementaryStreamPlayer.h>

namespace netflix {
namespace device {

/**
 * @brief Attributes of the audio elementary stream
 */
struct AudioAttributes
{
    AudioAttributes() : streamID_(0),
                        formatID_(0),
                        numberOfChannels_(0),
                        samplesPerSecond_(0),
                        bytesPerSecond_(0),
                        blockAlignment_(0),
                        bitsPerSample_(0),
                        encrypted_(false)
    {}

    uint32_t streamID_;
    uint32_t formatID_; /** WAVEFORMATEX.wFormatTag */
    uint32_t numberOfChannels_;
    uint32_t samplesPerSecond_;
    uint32_t bytesPerSecond_;
    uint32_t nominalBitrateInKbps_;
    uint32_t blockAlignment_;
    uint32_t bitsPerSample_;
    bool     encrypted_;
    std::vector<unsigned char> codecSpecificData_;
    std::vector<unsigned char> embeddedConfigData_;
};


struct MVCAttributes
{
    uint32_t NALULengthSize_;

    // The sub-sequence paramater set and picture parameter set for the
    // dependent view are place here.
    std::vector<unsigned char> subsequenceParameterSet_;
    std::vector<unsigned char> pictureParameterSet_;

    // These maps are the ScalableNALUMapEntry tables read from the moov header
    // (see ITU-T R. 14496-15:2010). They are used to split the NALUs in MVC
    // access units into base and dependent views when an MVC decoder
    // implementation needs to access views separately (see DUAL_STREAM_MVC in
    // the MP4Demultiplexer).
    //
    // Each access unit has a sampleGroup id read from the sample to group box
    // in the track fragment. That id indexes into naluToMvcViewMaps_ to specify
    // a vector with a size equal to the number of NALUs in the access unit.
    // The entries in the vector specify the views that successive NALUs in the
    // access unit belong to.
    std::vector<std::vector<uint8_t> > naluToMvcViewMaps_;
};

struct TemporalLayerEntryInfo
{
    uint32_t tlTemporalLayerId_;
    uint32_t tlLevelIdc_;
    uint32_t tlFrameRateValue_;
    uint32_t tlFrameRateScale_;
};

struct VideoAttributes
{
    VideoAttributes() : streamID_(0),
                        formatID_(0),
                        imageWidth_(0),
                        imageHeight_(0),
                        bitsDepthLuma_(8),
                        bitsDepthChroma_(8),
                        pixelAspectRatioX_(0),
                        pixelAspectRatioY_(0),
                        colorPrimaries_(-1),
                        transferCharacteristics_(-1),
                        matrixCoefficients_(-1),
                        fullRange_(0),
                        timeScale_(0),
                        encrypted_(false),
                        temporalLayerStream_(false),
                        format3D_(esplayer::NOT_3D),
                        mvcAttributes_(NULL)
    {}

    std::string         contentID_;
    uint32_t            streamID_;
    uint32_t            formatID_; /** FourCC */
    uint32_t            imageWidth_;
    uint32_t            imageHeight_;
    uint32_t            bitsDepthLuma_;
    uint32_t            bitsDepthChroma_;
    uint32_t            pixelAspectRatioX_;
    uint32_t            pixelAspectRatioY_;
    uint32_t            frameRateValue_;
    uint32_t            frameRateScale_;
    int32_t             colorPrimaries_;
    int32_t             transferCharacteristics_;
    int32_t             matrixCoefficients_;
    int32_t             fullRange_;
    uint32_t            timeScale_;
    bool                encrypted_;
    bool                temporalLayerStream_;
    uint32_t            generalLevelIdc_;
    uint32_t            nominalBitrateInKbps_;
    std::vector<struct TemporalLayerEntryInfo> temporalLayerEntryInfo_;

    esplayer::Format3D  format3D_;
    MVCAttributes      *mvcAttributes_; // Non-null only if format3D_ is MVC.

    std::vector<unsigned char> videoParameterSet_;
    std::vector<unsigned char> sequenceParameterSet_;
    std::vector<unsigned char> pictureParameterSet_;
    std::vector<std::vector<unsigned char> > embeddedConfigData_;
};

/**
 * @brief Attributes of the stream
 */
struct StreamAttributes
{
    StreamAttributes() : streamIndex_(0),
                         defaultDrmAlgorithmID_(0),
                         defaultDrmIVSize_(0),
                         timescaleConverterRatio_(0),
                         ptsOffset_(0),
                         audioAttributes_(NULL),
                         videoAttributes_(NULL)
    {
        defaultDrmKID_.clear();
    }

    uint32_t         streamIndex_;
    uint32_t         defaultDrmAlgorithmID_;
    uint32_t         defaultDrmIVSize_;
    std::vector<uint8_t> defaultDrmKID_;
    ullong         timescaleConverterRatio_;
    llong          ptsOffset_;
    AudioAttributes* audioAttributes_;
    VideoAttributes* videoAttributes_;
};

}} // namespace netflix::device

#endif // DEVICE_STREAMATTRIBUTES_H
