/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "SampleWriterNative.h"
#include "ESPlayerConstants.h"
#include <string.h>
#include <assert.h>
#include <nrdbase/Log.h>

enum { FF_INPUT_BUFFER_PADDING_SIZE = 16 };
using namespace netflix::device::esplayer;
using namespace netflix;

SampleWriterNative::SampleWriterNative(Format3D format3D, shared_ptr<IESPlayerCallback> callback)
  : mSampleAttributes(NULL),
    mFormat3D(format3D),
    mCallback(callback)
{
}

SampleWriterNative::~SampleWriterNative()
{
}

Status SampleWriterNative::reinitBufferIfNeeded(uint32_t sizeNeeded)
{
    if(mBuffer.size() < sizeNeeded)
    {
        mBuffer.resize(sizeNeeded);
    }
    // NOTE: - Partners can also return "NO_AVAILABLE_BUFFERS" if there are none available.
    //       - The mBuffers is a byte vector (used by the reference application),
    //		   but partners should consider directly providing the
    //         decoder buffer pointers for better effeciency
    return OK;
}

// Returned view sizes include prepended parameter sets.
void  SampleWriterNative::calculateSampleSizesAndBufferPositions(const ISampleAttributes& attributes,
                                                                 uint32_t& baseViewSize,
                                                                 uint32_t& depViewSize)
{
    depViewSize = 0;
    baseViewSize = attributes.getSize();

    mViewOffsetInBuffer.assign(1,0); // Offset of the first view in the buffer.
    mBufferPositions.assign(1, 0);   // Initial write position for first view.
    mSampleSizes.assign(1, baseViewSize);

    if(mFormat3D == MVC_SPLIT)
    {
        // Offset of the second view in the buffer.
        mViewOffsetInBuffer.push_back(baseViewSize);

        // Initial write position for the second view.
        mBufferPositions.push_back(baseViewSize);

        depViewSize = attributes.getSize(1);
        mSampleSizes.push_back(depViewSize);
    }
}

Status SampleWriterNative::initSample(const ISampleAttributes& attributes)
{
    uint32_t baseViewSize = 0, depViewSize = 0;

    mSampleAttributes = &attributes;

    // There should only be more than one view for mvc split.
    if(attributes.getNumViews() > 1 && mFormat3D != MVC_SPLIT)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "Error in SampleWriterNative::initSample. "
                   "Num views = %d and mFormat3D != MVC_SPLIT", attributes.getNumViews());
        return UNEXPECTED_MEDIA_ATTRIBUTES;
    }

    calculateSampleSizesAndBufferPositions(attributes, baseViewSize, depViewSize);

    uint32_t size = baseViewSize + depViewSize + FF_INPUT_BUFFER_PADDING_SIZE;
    if (uint32_t rest = size % 4)
        size += (4 - rest);

    return reinitBufferIfNeeded(size);
}

void SampleWriterNative::reset()
{
    uint32_t numSamples = mFormat3D == MVC_SPLIT ? 2:1;

    mSampleAttributes = NULL;
    mBuffer.clear();
    mSampleSizes.assign(numSamples, 0);
    mBufferPositions.assign(numSamples, 0);
    mViewOffsetInBuffer.assign(numSamples, 0);
    mSampleAttributes = NULL;
}

uint32_t SampleWriterNative::write(const uint8_t *src, uint32_t size,
                                   uint32_t viewNum)
{
    uint32_t writeSize =  size;

    if(viewNum >= mBufferPositions.size()) {
        return 0;
    }

    if(mBufferPositions[viewNum] + size > mBuffer.size())
    {
        writeSize = mBuffer.size() - mBufferPositions[viewNum];
    }
    memcpy(&mBuffer[mBufferPositions[viewNum]], src, writeSize);
    mBufferPositions[viewNum] += writeSize;

    return writeSize;
}

uint8_t* SampleWriterNative::getBuffer(uint32_t viewNum)
{
    return &mBuffer[mViewOffsetInBuffer[viewNum]];
}

uint32_t SampleWriterNative::getSampleSize(uint32_t viewNum)
{
    return mSampleSizes[viewNum];
}

const ISampleAttributes* SampleWriterNative::getSampleAttributes()
{
    return mSampleAttributes;
}


