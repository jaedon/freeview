/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Reader.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Reader
 */

Reader::Reader(ByteStream& byteStream, ullong numBytesAvailable)
    : byteStream_(byteStream),
      numBytesAvailable_(numBytesAvailable),
      cachedByteValidBits_(0),
      good_(true)
{
}

bool Reader::readByte(uint8_t& data)
{
    assert(isByteAligned());

    if(numBytesAvailable_ < 1)
        good_ = false;

    if(good_)
    {
        byteStream_.readByte(data);
        good_ = byteStream_.good();
    }

    if(!good_)
        return false;

    numBytesAvailable_ -= 1;
    return true;
}

bool Reader::readBytes(std::vector<uint8_t>& data)
{
    assert(isByteAligned());

    if(numBytesAvailable_ < data.size())
        good_ = false;

    if(good_ && data.size() > 0)
    {
        byteStream_.readBytes(data);
        good_ = byteStream_.good();
    }

    if(!good_)
        return false;

    numBytesAvailable_ -= data.size();
    return true;
}

bool Reader::readBytes(
    std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes)
{
    assert(isByteAligned());

    if(numBytesAvailable_ < numBytes)
        good_ = false;

    if(good_ && numBytes > 0)
    {
        byteStream_.readBytes(data, offset, numBytes);
        good_ = byteStream_.good();
    }

    if(!good_)
        return false;

    numBytesAvailable_ -= numBytes;
    return true;
}

bool Reader::skipBytes(ullong numBytes)
{
    assert(isByteAligned());

    if(numBytesAvailable_ < numBytes)
        good_ = false;

    if(good_ && numBytes > 0)
    {
        byteStream_.skipBytes(numBytes);
        good_ = byteStream_.good();
    }

    if(!good_)
        return false;

    numBytesAvailable_ -= numBytes;
    return true;
}

ullong Reader::tell() const
{
    assert(isByteAligned());
    return byteStream_.tell();
}

bool Reader::skipBits(uint32_t numBits)
{
    ullong data;
    return read(data, numBits);
}

bool Reader::skipToEOS()
{
    cachedByteValidBits_ = 0;
    return skipBytes(numBytesAvailable_);
}

bool Reader::eos() const
{
    return (numBytesAvailable_ == 0 && cachedByteValidBits_ == 0);
}

bool Reader::isByteAligned() const
{
    return (cachedByteValidBits_ == 0);
}

bool Reader::read(char& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(int16_t& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(int32_t& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(long& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(long long& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(uint16_t& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(uint32_t& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(unsigned long& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

bool Reader::read(unsigned long long& data, uint32_t numBits)
{
    return internalRead(data, numBits);
}

template <typename T> bool Reader::internalRead(T& data, uint32_t numBits)
{
    assert(numBits <= 64);
    assert(sizeof(T) * 8 >= numBits);

    if(numBits > 32)
        data = static_cast<T>(readBits64(numBits));
    else
        data = static_cast<T>(readBits32(numBits));

    return good();
}

static uint32_t mask[] = {
    0x00, // unused
    0x01, // 1 bit
    0x03, // 2 bits
    0x07, // 3 bits
    0x0F, // 4 bits
    0x1F, // 5 bits
    0x3F, // 6 bits
    0x7F, // 7 bits
    0xFF, // 8 bits
};

template <typename T> T Reader::readBits(uint32_t numBits)
{
    T result = 0;

    while(numBits > 0)
    {
        if(cachedByteValidBits_ == 0)
        {
            if(!readByte(cachedByte_))
                break;

            cachedByteValidBits_ = 8;
        }

        uint32_t bitsToAdd =
            (numBits < cachedByteValidBits_)? numBits : cachedByteValidBits_;

        cachedByteValidBits_ -= bitsToAdd;
        numBits -= bitsToAdd;

        result <<= bitsToAdd;
        result |= ((cachedByte_ >> cachedByteValidBits_) & mask[bitsToAdd]);
    }

    return result;
}

template <typename T> T Reader::readBitsFast(uint32_t numBits)
{
    T result = 0;

    uint32_t i;
    for(i = 0; i < numBits; i += 8)
    {
        if(!readByte(cachedByte_))
            break;

        result <<= 8;
        result |= cachedByte_;
    }

    return result;
}

uint32_t Reader::readBits32(uint32_t numBits)
{
    if(cachedByteValidBits_ == 0 && (numBits & 0x7) == 0)
        return readBitsFast<uint32_t>(numBits);
    else
        return readBits<uint32_t>(numBits);
}

ullong Reader::readBits64(uint32_t numBits)
{
    if(cachedByteValidBits_ == 0 && (numBits & 0x7) == 0)
        return readBitsFast<ullong>(numBits);
    else
        return readBits<ullong>(numBits);
}
