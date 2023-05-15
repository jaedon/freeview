/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "SampleAttributes.h"
#include "DeviceConstants.h"
#include <nrdbase/Log.h>
#include <assert.h>

using namespace netflix::device;
using namespace netflix::device::esplayer;

SampleAttributes::~SampleAttributes()
{
}

SampleAttributes::SampleAttributes(int numViews) :
  mNumViews(numViews),
  mDiscontinuityGapInMs(0),
  mPTS(0),
  mDTS(0),
  mTimescale(DeviceConstants::DEFAULT_TIMESCALE),
  mIsKeyFrame(false),
  mAlgorithmID(0),
  mIVData(NULL),
  mIVSize(0),
  mIsDependedOn(true)
{

    mSampleSizes.resize(mNumViews,0);
    mTimescale = DeviceConstants::DEFAULT_TIMESCALE;
    mIsKeyFrame = false;
    mIsDolbyVisionELFrame = false;
    mAlgorithmID = 0;

    mKeyID.clear();
    mSubsampleMappings.resize(mNumViews);

    mMediaAttributes.mAudioAttributes = NULL;
    mMediaAttributes.mVideoAttributes = NULL;
}

void SampleAttributes::reset(esplayer::MediaType type,
                               esplayer::Format3D format3D)
{
    // There are at most two views, one for base and one for dependent in the
    // case of mvc split.  For audio and 2D video, only mSampleSizes[0] will be
    // used.
    if(type == esplayer::VIDEO && format3D == esplayer::MVC_SPLIT)
    {
        mNumViews = 2;
    } else
    {
        mNumViews = 1;
    }
    mSampleSizes.clear();
    mSampleSizes.resize(mNumViews,0);
    mPTS = 0;
    mDTS = 0;
    mTimescale = DeviceConstants::DEFAULT_TIMESCALE;
    mIsKeyFrame = false;
    mIsDolbyVisionELFrame = false;
    mAlgorithmID = 0;
    mIVData = NULL;
    mIVSize = 0;
    mKeyID.clear();
    mSubsampleMappings.clear();
    mSubsampleMappings.resize(mNumViews);
    mNaluSizes.clear();
    mNaluSizes.resize(mNumViews);
    mIsDependedOn = true;
    mMediaAttributes.mAudioAttributes = NULL;
    mMediaAttributes.mVideoAttributes = NULL;
}

void SampleAttributes::initMediaAttributes(esplayer::MediaType mediaType)
{
    if(mediaType == VIDEO)
    {
        mMediaAttributes.mAudioAttributes = NULL;
        mMediaAttributes.mVideoAttributes = &mVideoAttributes;
    } else {
        mMediaAttributes.mAudioAttributes = &mAudioAttributes;
        mMediaAttributes.mVideoAttributes = NULL;
    }
}

std::string SampleAttributes::getContentId() const
{
    std::string contentId((char*)&mKeyID[0], mKeyID.size());
    return contentId;
}

uint32_t SampleAttributes::getSize(uint32_t viewNum) const
{
    assert(viewNum < mSampleSizes.size());

    if(viewNum >= mSampleSizes.size())
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "Error: SampleAttributes::getBufferLen: "
               "viewNum = %d out of range.", viewNum);
        return 0;
    }

    return mSampleSizes[viewNum];
}

llong SampleAttributes::getDiscontinuityGapInMs() const
{
    return mDiscontinuityGapInMs;
}

llong SampleAttributes::getPTS() const
{
    return mPTS;
}

llong SampleAttributes::getDTS() const
{
    return mDTS;
}

int32_t SampleAttributes::getTimescale() const
{
    return mTimescale;
}

bool  SampleAttributes::isKeyFrame() const
{
    return mIsKeyFrame;
}

bool SampleAttributes::isDolbyVisionELFrame() const
{
    return mIsDolbyVisionELFrame;
}

uint32_t SampleAttributes::getAlgorithmID() const
{
    return mAlgorithmID;
}

uint8_t* SampleAttributes::getIVData() const
{
    return mIVData;
}

uint32_t SampleAttributes::getIVSize() const
{
    return mIVSize;
}

std::vector<uint8_t> SampleAttributes::getKeyID() const
{
    return mKeyID;
}

const std::vector<uint32_t>& SampleAttributes::getSubsampleMapping(uint32_t viewNum) const
{
    assert(viewNum < mSubsampleMappings.size());

    if(viewNum >= mSubsampleMappings.size())
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "Error: SampleAttributes::getSubsampleMapping: "
               "viewNum = %d out of range.", viewNum);
        return mSubsampleMappings[0];
    }
    return mSubsampleMappings[viewNum];
}

const std::vector<uint32_t>& SampleAttributes::getNaluSizes(uint32_t viewNum) const
{
    assert(viewNum < mNaluSizes.size());
    if(viewNum >= mNaluSizes.size())
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "Error: SampleAttributes::getNaluSizes: "
               "viewNum = %d out of range.", viewNum);
        return mNaluSizes[0];
    }
    return mNaluSizes[viewNum];
}

bool SampleAttributes::isDependedOn() const
{
    return mIsDependedOn;
}

void SampleAttributes::setIsDependedOn(bool bIsDependedOn)
{
    mIsDependedOn = bIsDependedOn;
}

const MediaAttributes* SampleAttributes::getMediaAttributes() const
{
    if(mMediaAttributes.mVideoAttributes || mMediaAttributes.mAudioAttributes)
    {
        return &mMediaAttributes;
    }
    return NULL;
}

void SampleAttributes::setSize(uint32_t len, uint32_t viewNum)
{
    assert(viewNum < mNumViews);
    mSampleSizes[viewNum] = len;
}

void SampleAttributes::setDiscontinuityGapInMs(llong gapInMs)
{
    mDiscontinuityGapInMs = gapInMs;
}

void SampleAttributes::setPTS(llong pts)
{
    mPTS = pts;
}

void SampleAttributes::setDTS(llong dts)
{
    mDTS = dts;
}

void SampleAttributes::setTimescale(int32_t timescale)
{
    mTimescale = timescale;
}

void SampleAttributes::setIsKeyFrame()
{
    mIsKeyFrame = true;
}

void SampleAttributes::setIsDolbyVisionELFrame()
{
    mIsDolbyVisionELFrame = true;
}

void SampleAttributes::setAlgorithmID(uint32_t id)
{
    mAlgorithmID = id;
}

void SampleAttributes::setIVData(uint8_t* IVData)
{
    mIVData = IVData;
}

void SampleAttributes::setIVData(std::vector<uint8_t> IVData)
{
    mIVDataCopy = IVData;
    if (IVData.size() > 0)
        mIVData = &mIVDataCopy[0];
    else
        mIVData = 0;
}

void SampleAttributes::setIVSize(uint32_t IVSize)
{
    mIVSize = IVSize;
}

void SampleAttributes::setKeyID(std::vector<uint8_t> keyId)
{
    mKeyID = keyId;
}

void SampleAttributes::addNaluSizeEntry(uint32_t size, uint32_t buffNum)
{
    assert(buffNum < mNumViews);
    mNaluSizes[buffNum].push_back(size);
}

void SampleAttributes::addSubsampleMappingEntry(uint32_t clearLen, uint32_t encLen,
                                                   uint32_t viewNum)
{
    assert(viewNum < mNumViews);
    mSubsampleMappings[viewNum].push_back(clearLen);
    mSubsampleMappings[viewNum].push_back(encLen);
}


uint32_t SampleAttributes::getNumViews() const
{
    return mNumViews;

}

