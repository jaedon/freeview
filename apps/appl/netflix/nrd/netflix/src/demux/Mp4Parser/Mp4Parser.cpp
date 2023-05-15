/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Mp4Parser.h"
#include <stdio.h>
#include <string.h>

#include "ISOBoxes.h"
#include "NFLXBoxes.h"

#include "Reader.h"
#include "Dumper.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;


/*******************************************************************************
 * MemByteStream
 */

class MemByteStream : public ByteStream
{
private:
    static const uint32_t INVALID_OFFSET = 0xFFFFFFFF;

    const unsigned char* data_;
    uint32_t dataSize_;
    uint32_t offset_;

public:
    MemByteStream(const unsigned char* data, uint32_t dataSize)
        : data_(data), dataSize_(dataSize), offset_(0) {}

    virtual bool readByte(uint8_t& data)
    {
        if(offset_ == INVALID_OFFSET)
            return false;

        if(offset_ >= dataSize_)
        {
            offset_ = INVALID_OFFSET;
            return false;
        }

        data = data_[offset_];
        offset_ ++;

        return true;
    }

    virtual bool readBytes(std::vector<uint8_t>& data)
    {
        if(offset_ == INVALID_OFFSET)
            return false;

        if(offset_ + data.size() > dataSize_)
        {
            offset_ = INVALID_OFFSET;
            return false;
        }

        if(data.size() > 0)
        {
            memcpy(&data[0], &data_[offset_], data.size());
            offset_ += data.size();
        }

        return true;
    }

    virtual bool readBytes(
        std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes)
    {
        assert(offset < data.size() &&
               offset + numBytes <= data.size() &&
               offset + numBytes > offset);

        if(offset_ == INVALID_OFFSET)
            return false;

        if(offset_ + numBytes > dataSize_)
        {
            offset_ = INVALID_OFFSET;
            return false;
        }

        if(numBytes > 0)
        {
            memcpy(&data[offset], &data_[offset_], numBytes);
            offset_ += numBytes;
        }

        return true;
    }

    virtual bool skipBytes(ullong numBytes)
    {
        if(offset_ == INVALID_OFFSET)
            return false;

        if(offset_ + numBytes > dataSize_)
        {
            offset_ = INVALID_OFFSET;
            return false;
        }

        offset_ += numBytes;
        return true;
    }

    virtual ullong tell() const { return offset_; }

    virtual bool good() const { return offset_ != INVALID_OFFSET; }
};

/*******************************************************************************
 * ConsoleDumper
 */

class ConsoleDumper : public Dumper
{
protected:
    virtual void output(std::string outputMessage)
    {
        printf("%s\n", outputMessage.c_str());
    }
};

/*******************************************************************************
 * Mp4Parser
 */

#define ADD_STD_BOX(_class_, _restriction_)\
    handlers.addHandler(\
        new StandardBoxReadHandler<_class_>(BoxReadHandler::_restriction_))

#define ADD_EXT_BOX(_class_, _restriction_)\
    handlers.addHandler(\
        new ExtendedBoxReadHandler<_class_>(BoxReadHandler::_restriction_))

void Mp4Parser::reset()
{
    boxes_.clear();
}

void Mp4Parser::parseAll(
    Context& context, const unsigned char* data, uint32_t dataSize)
{
    MemByteStream byteStream(data, dataSize);
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    parseAll(context, reader);
}

void Mp4Parser::parseMovieBox(
    Context& context, const unsigned char* data, uint32_t dataSize)
{
    MemByteStream byteStream(data, dataSize);
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    parseMovieBox(context, reader);
}

void Mp4Parser::parseMovieFragment(
    Context& context, const unsigned char* data, uint32_t dataSize)
{
    MemByteStream byteStream(data, dataSize);
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    parseMovieFragment(context, reader);
}

void Mp4Parser::parseMovieFragment(Context& context, ByteStream& byteStream)
{
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    parseMovieFragment(context, reader);
}

void Mp4Parser::parseNetflixHeader(
    Context& context, const unsigned char* data, uint32_t dataSize)
{
    MemByteStream byteStream(data, dataSize);
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    parseNetflixHeader(context, reader);
}

void Mp4Parser::parseNetflixIndex(
    Context& context, const unsigned char* data, uint32_t dataSize)
{
    MemByteStream byteStream(data, dataSize);
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    parseNetflixIndex(context, reader);
}

void Mp4Parser::getNextBoxInfo(const unsigned char* data, uint32_t dataSize, ullong& size, uint32_t& boxType)
{
    MemByteStream byteStream(data, dataSize);
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    Box::getNextBoxInfo(reader, size, boxType);
}

void Mp4Parser::parseAll(Context& context, Reader& reader)
{
    BoxReadHandlerList handlers;

    ADD_STD_BOX(FileTypeBox,      MANDATORY_UNIQUE);
    ADD_STD_BOX(MovieBox,         MANDATORY_UNIQUE);
    ADD_STD_BOX(MediaDataBox,     OPTIONAL);
    ADD_STD_BOX(MovieFragmentBox, OPTIONAL);
    ADD_EXT_BOX(NetflixHeaderBox, OPTIONAL_UNIQUE);
    ADD_EXT_BOX(NetflixIndexBox,  OPTIONAL_UNIQUE);

    Box::readBoxes(context, handlers, boxes_, reader);
}

void Mp4Parser::parseMovieBox(Context& context, Reader& reader)
{
    BoxReadHandlerList handlers;
    ADD_STD_BOX(MovieBox, MANDATORY_UNIQUE);

    Box::readBoxes(context, handlers, boxes_, reader);
}

bool Mp4Parser::getMovieFragmentSize(ByteStream& byteStream, ullong& size)
{
    uint32_t boxType;
    Reader reader(byteStream, Box::UNLIMITED_SIZE);
    Box::getNextBoxInfo(reader, size, boxType);
    if(boxType !=  Box::buildFourCC("moof"))
        return false;
    return true;
}

void Mp4Parser::parseMovieFragment(Context& context, Reader& reader)
{
    BoxReadHandlerList handlers;
    ADD_STD_BOX(MovieFragmentBox, OPTIONAL);

    Box::readBoxes(context, handlers, boxes_, reader);
}

void Mp4Parser::parseNetflixHeader(Context& context, Reader& reader)
{
    BoxReadHandlerList handlers;
    ADD_EXT_BOX(NetflixHeaderBox, OPTIONAL_UNIQUE);

    Box::readBoxes(context, handlers, boxes_, reader);
}

void Mp4Parser::parseNetflixIndex(Context& context, Reader& reader)
{
    BoxReadHandlerList handlers;
    ADD_EXT_BOX(NetflixIndexBox, OPTIONAL_UNIQUE);

    Box::readBoxes(context, handlers, boxes_, reader);
}

void Mp4Parser::dumpBoxes()
{
    ConsoleDumper consoleDumper;
    Box::dumpBoxes(boxes_, consoleDumper);
}
