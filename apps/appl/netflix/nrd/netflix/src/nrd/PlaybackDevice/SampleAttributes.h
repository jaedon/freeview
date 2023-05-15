/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef SAMPLEATTRIBUTES_H
#define SAMPLEATTRIBUTES_H

#include <nrd/IElementaryStreamPlayer.h>
#include <list>



namespace netflix {
namespace device {

class PlaybackDevice;

namespace esplayer {

class SampleAttributes : public ISampleAttributes {
public:
    SampleAttributes(int numViews = 1);
    virtual ~SampleAttributes();

    // ContentId is an identifier that tells same license is used for
    // downloadables. In Netflix case, KeyID can be contentId. This function
    // internally takes KeyID which is bytes array and converts it into
    // contentId which is string
    virtual std::string getContentId() const;
    virtual uint32_t getNumViews() const;
    virtual uint32_t getSize(uint32_t viewNum = 0) const;
    virtual llong getDiscontinuityGapInMs() const;
    virtual llong getPTS() const;
    virtual llong getDTS() const;
    virtual int32_t getTimescale() const;
    virtual bool isKeyFrame() const;
    virtual bool isDolbyVisionELFrame() const;
    virtual uint32_t getAlgorithmID() const;
    virtual uint8_t* getIVData() const;
    virtual uint32_t getIVSize() const;
    virtual std::vector<uint8_t> getKeyID() const;
    virtual const std::vector<uint32_t>& getSubsampleMapping(uint32_t viewNum = 0) const;
    virtual const std::vector<uint32_t>& getNaluSizes(uint32_t viewNum = 0) const;
    virtual bool isDependedOn() const;
    virtual const MediaAttributes* getMediaAttributes() const;

    void setSize(uint32_t len, uint32_t viewNum = 0);
    void setDiscontinuityGapInMs(llong gapInMs);
    void setPTS(llong pts);
    void setDTS(llong pts);
    void setTimescale(int32_t timescale);
    void setIsKeyFrame();
    void setIsDolbyVisionELFrame();
    void setAlgorithmID(uint32_t);
    void setIVData(uint8_t* IVData);
    void setIVData(std::vector<uint8_t> IVData);
    void setIVSize(uint32_t IVSize);
    void setKeyID(std::vector<uint8_t> keyId);
    void setIsDependedOn(bool bIsDependedOn);
    void addSubsampleMappingEntry(uint32_t clearLen, uint32_t encLen,
                                                    uint32_t buffNum = 0);
    void addNaluSizeEntry(uint32_t size, uint32_t buffNum = 0);

    void reset(esplayer::MediaType type,
               esplayer::Format3D format3D);
    void initMediaAttributes(esplayer::MediaType mediaType);


    VideoAttributes mVideoAttributes;
    AudioAttributes mAudioAttributes;
    MediaAttributes mMediaAttributes;

private:
    std::vector<uint32_t> mSampleSizes;
    // There is one nalu size vector for each view.
    std::vector<std::vector<uint32_t> > mNaluSizes;
    uint32_t mNumViews;
    llong mDiscontinuityGapInMs;
    llong mPTS;
    llong mDTS;
    llong mTimescale;
    bool  mIsKeyFrame;
    bool  mIsDolbyVisionELFrame;
    uint32_t mAlgorithmID;
    uint8_t* mIVData;
    std::vector<uint8_t> mIVDataCopy;
    uint32_t mIVSize;
    std::vector<uint8_t> mKeyID;
    std::vector<std::vector<uint32_t> > mSubsampleMappings;
    bool mIsDependedOn;
};

} // namespace esplayer
} // namespace device
} // namespace netflix


#endif
