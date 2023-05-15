/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <cassert>
#include <list>
#include <string.h>
#include <limits.h>

#include <nrdbase/Log.h>
#include <nrddemux/Context.h>
#include <nrddemux/Mp4Parser.h>
#include <nrddemux/Reader.h>
#include <nrd/NrdConfiguration.h>

#include "StreamAttributes.h"
#include "SampleAttributes.h"
#include "DeviceConstants.h"
#include "Mp4Demultiplexer.h"
#include "StreamParser.h"

using namespace std;
using namespace netflix;
using namespace netflix::device;
using namespace netflix::containerlib::mp4parser;

#define _FOURCC_LE_(a,b,c,d)  (a | (b << 8) | (c << 16) | (d << 24))
#define _FOURCC_BE_(a,b,c,d)  (d | (c << 8) | (b << 16) | (a << 24))

#ifdef NDEBUG
#define PRINT(fmt, ...)
#else
#include <stdio.h>
#define PRINT(fmt, ...) printf("DEMUX DEBUG:: " fmt, __VA_ARGS__ )
#endif

static const uint32_t MAX_UINT32           = 0xFFFFFFFF;
static const uint32_t INVALID_PTS          = MAX_UINT32;

DashDemultiplexer::DashDemultiplexer()
{

}

DashDemultiplexer::~DashDemultiplexer()
{

}

DashDemultiplexer::DataBlock::DataBlock() :
    dataBuffer_(NULL),
    offset_(0),
    streamAttributes_(NULL),
    isParsed_(false)
{
}

DashDemultiplexer::DataBlock::~DataBlock()
{
}

shared_ptr<EventQueue> DashDemultiplexer::getAttachedEventQueue()
{
    return mEventQueue;
}

uint32_t DashDemultiplexer::DataBlock::getDataSize()
{
    return dataBuffer_.dataSize_ - offset_;
}

unsigned char* DashDemultiplexer::DataBlock::getData()
{
    if(offset_ >= dataBuffer_.dataSize_)
    {
#if defined(STREAM_ERROR_DEBUG)
        if(streamAttributes_->videoAttributes_){
            printf("getData return NULL\n");
        }
#endif
        return NULL;
    }
#if defined(STREAM_ERROR_DEBUG)
    if(streamAttributes_->videoAttributes_){
        printf("getData (dataBuffer_.byteBuffer + offset_) = %u + %u = %u\n",
               (unsigned char*)dataBuffer_.byteBuffer_ ,offset_,
               ((unsigned char*)dataBuffer_.byteBuffer_ + offset_));
    }
#endif
    return ((unsigned char*)dataBuffer_.byteBuffer_ + offset_);
}

void DashDemultiplexer::DataBlock::setDataBuffer(IBufferManager::Buffer& buffer)
{
    dataBuffer_ = buffer;
}

void DashDemultiplexer::DataBlock::setStreamAttributes(StreamAttributes *attributes)
{
    streamAttributes_ = attributes;
}

void DashDemultiplexer::DataBlock::setIsParsed()
{
    isParsed_ = true;
}

bool DashDemultiplexer::DataBlock::isParsed()
{
    return isParsed_;
}

const IBufferManager::Buffer&  DashDemultiplexer::DataBlock::getDataBuffer() const
{
    return dataBuffer_;
}

const StreamAttributes*  DashDemultiplexer::DataBlock::getStreamAttributes() const
{
    return streamAttributes_;
}

void DashDemultiplexer::DataBlock::reset()
{
    offset_ = 0;
    streamAttributes_ = NULL;
    isParsed_ = false;
    dataBuffer_.inUse_ = NULL;
    dataBuffer_.byteBuffer_ = NULL;
}

DataBlockStream::DataBlockStream(IConsumerListView<DataBlock>::iterator startBlock,
                                 IConsumerListView<DataBlock>::iterator endBlock) :
  startBlock_(startBlock),
  endBlock_(endBlock),
  totalOffset_(0),
  totalSize_(0),
  currentBlockOffset_(0),
  currentBlock_(startBlock_),
  good_(true)
{
    assert(startBlock_ != NULL);

    IConsumerListView<DataBlock>::iterator block;
    for(block = startBlock_; block != endBlock_; ++block)
    {
        totalSize_+= block->getDataSize();
    }
}

void DataBlockStream::reset()
{
    totalOffset_ = 0;
    currentBlockOffset_ = 0;
    currentBlock_ = startBlock_;
    good_ = true;
}

virtual bool DataBlockStream::readByte(uint8_t& data)
{
    if(totalOffset_ >= totalSize_)
    {
        good_ = false;
        return false;
    }

    while(currentBlock_ != endBlock_ && currentBlock_->getDataSize() - currentBlockOffset_ < 1)
    {
        ++currentBlock_;
        currentBlockOffset_ = 0;
    }

    // We should have at least a byte because we checked above.
    assert(currentBlock_ != endBlock_);

    data = *(currentBlock_->getData() + currentBlockOffset_);
    currentBlockOffset_++;
    totalOffset_++;
    return true;
}

virtual bool DataBlockStream::readBytes(std::vector<uint8_t>& data)
{
    if(good_ == false)
    {
        return false;
    }
    if(data.size() > totalSize_ - totalOffset_)
    {
        good_ = false;
        return false;
    }
    if(data.size() > 0)
    {
        writeBytes(data, 0, data.size());
    }
    return true;
}

virtual bool DataBlockStream::readBytes(std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes)
{
    assert(offset < data.size() &&
           offset + numBytes <= data.size() &&
           offset + numBytes > offset);

    if(good_ == false)
    {
        return false;
    }
    if(numBytes > totalSize_ - totalOffset_)
    {
        good_ = false;
        return false;
    }
    if(numBytes > 0)
    {
        writeBytes(data, offset, numBytes);
    }
    return true;
}

bool DataBlockStream::writeBytes(esplayer::ISampleWriter& writer, uint32_t numBytes)
{
    if(good_ == false)
    {
        return false;
    }
    if(numBytes > totalSize_ - totalOffset_)
    {
        good_ = false;
        return false;
    }
    if(numBytes > 0)
    {
        if(writeBytesInternal(writer, numBytes) == false)
        {
            good_ = false;
            return false;
        }
    }
    return true;

}

bool DataBlockStream::writeBytesInternal(esplayer::ISampleWriter& writer, uint32_t numBytes)
{
    uint32_t numBytesWritten = 0;
    while(numBytesWritten < numBytes)
    {
        assert(currentBlock_ != endBlock_);

        if(currentBlock_->getDataSize() - currentBlockOffset_ >= numBytes - numBytesWritten) {
            uint32_t writeSize = numBytes - numBytesWritten;
            if(writer.write( currentBlock_->getData() + currentBlockOffset_, writeSize) != writeSize)
            {
                return false;
            }
            currentBlockOffset_ += writeSize;
            break;
        } else {
            uint32_t writeSize = currentBlock_->getDataSize() - currentBlockOffset_;
            if(writer.write( currentBlock_->getData() + currentBlockOffset_, writeSize) != writeSize)
            {
                return false;
            }
            numBytesWritten += writeSize;
            ++currentBlock_;
            currentBlockOffset_ = 0;
        }
    }
    totalOffset_+= numBytes;
    return true;
}


void DataBlockStream::writeBytes(std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes)
{
    uint32_t numBytesWritten = 0;
    while(numBytesWritten < numBytes)
    {
        assert(currentBlock_ != endBlock_);

        if(currentBlock_->getDataSize() - currentBlockOffset_ >= numBytes - numBytesWritten) {
            uint32_t writeSize = numBytes - numBytesWritten;
            memcpy(&data[numBytesWritten+offset], currentBlock_->getData() + currentBlockOffset_, writeSize);
            currentBlockOffset_ += writeSize;
            break;
        } else {
            uint32_t writeSize = currentBlock_->getDataSize() - currentBlockOffset_;
            memcpy(&data[numBytesWritten+offset], currentBlock_->getData() + currentBlockOffset_, writeSize);
            numBytesWritten += writeSize;
            ++currentBlock_;
            currentBlockOffset_ = 0;
        }
    }
    totalOffset_+= numBytes;
}

virtual bool DataBlockStream::skipBytes(ullong numBytes)
{
    if(good_ == false)
    {
        return false;
    }
    if(numBytes > totalSize_ - totalOffset_)
    {
        good_ = false;
        return false;
    }
    ullong numBytesSkipped = 0;
    while(numBytesSkipped < numBytes)
    {
        assert(currentBlock_ != endBlock_);

        if(currentBlock_->getDataSize() - currentBlockOffset_ >= numBytes - numBytesSkipped) {
            uint32_t skipSize = numBytes - numBytesSkipped;
            currentBlockOffset_ += skipSize;
            break;
        } else {
            uint32_t skipSize = currentBlock_->getDataSize() - currentBlockOffset_;
            numBytesSkipped += skipSize;
            currentBlockOffset_ = 0;
            ++currentBlock_;
        }
    }
    totalOffset_+= numBytes;
    return true;
}

virtual ullong DataBlockStream::tell() const
{
    return totalOffset_;
}

virtual bool DataBlockStream::good() const
{
    return DataBlockStream::good_;
}

ullong DataBlockStream::getNumBytesAvailable()
{
    return totalSize_;
}
