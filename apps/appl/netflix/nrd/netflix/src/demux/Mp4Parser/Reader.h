/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_Reader__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_Reader__

#include <nrdbase/StdInt.h>
#include <cassert>
#include <vector>

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * ByteStream
 */

class ByteStream
{
public:
    virtual ~ByteStream() {}

    virtual bool readByte(uint8_t& data) = 0;
    virtual bool readBytes(std::vector<uint8_t>& data) = 0;
    virtual bool readBytes(
        std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes) = 0;
    virtual bool skipBytes(ullong numBytes) = 0;
    virtual ullong tell() const = 0;
    virtual bool good() const = 0;
};

/*******************************************************************************
 * Reader
 */

class Reader : public ByteStream
{
private:
    ByteStream& byteStream_;
    ullong numBytesAvailable_;
    uint8_t cachedByte_;
    uint32_t cachedByteValidBits_;
    bool good_;

public:
    Reader(ByteStream& byteStream, ullong numBytesAvailable);

    virtual bool readByte(uint8_t& data);
    virtual bool readBytes(std::vector<uint8_t>& data);
    virtual bool readBytes(
        std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes);
    virtual bool skipBytes(ullong numBytes);
    virtual ullong tell() const;

    virtual bool good() const { return good_; }

    bool skipBits(uint32_t numBits);
    bool skipToEOS();
    bool eos() const;
    bool isByteAligned() const;

    bool read(char& data, uint32_t numBits);
    bool read(int16_t& data, uint32_t numBits);
    bool read(int32_t& data, uint32_t numBits);
    bool read(long& data, uint32_t numBits);
    bool read(long long& data, uint32_t numBits);
    bool read(uint16_t& data, uint32_t numBits);
    bool read(uint32_t& data, uint32_t numBits);
    bool read(unsigned long& data, uint32_t numBits);
    bool read(unsigned long long& data, uint32_t numBits);

private:
    template <typename T> bool internalRead(T& data, uint32_t numBits);
    template <typename T> T readBits(uint32_t numBits);
    template <typename T> T readBitsFast(uint32_t numBits);

    uint32_t readBits32(uint32_t numBits);
    ullong readBits64(uint32_t numBits);
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_Reader__
