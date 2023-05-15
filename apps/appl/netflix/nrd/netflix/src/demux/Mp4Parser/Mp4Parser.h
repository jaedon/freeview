/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_Mp4Parser__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_Mp4Parser__

#include "Context.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

class Box;
class Reader;
class ByteStream;

/*******************************************************************************
 * Mp4Parser
 */

class Mp4Parser
{
private:
    std::list< shared_ptr<Box> > boxes_;

public:
    void reset();

    void parseAll(
        Context& context, const unsigned char* data, uint32_t dataSize);

    void parseMovieBox(
        Context& context, const unsigned char* data, uint32_t dataSize);

    bool getMovieFragmentSize(ByteStream& reader, ullong& size);
    void parseMovieFragment(
        Context& context, const unsigned char* data, uint32_t dataSize);
    void parseMovieFragment(Context& context, ByteStream& bytestream);

    void parseNetflixHeader(
        Context& context, const unsigned char* data, uint32_t dataSize);

    void parseNetflixIndex(
        Context& context, const unsigned char* data, uint32_t dataSize);

    void getNextBoxInfo(
        const unsigned char* data, uint32_t dataSize, ullong& size, uint32_t& boxType);

    void dumpBoxes();

private:
    void parseAll(Context& context, Reader& reader);
    void parseMovieBox(Context& context, Reader& reader);
    void parseMovieFragment(Context& context, Reader& reader);
    void parseNetflixHeader(Context& context, Reader& reader);
    void parseNetflixIndex(Context& context, Reader& reader);
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_Mp4Parser__
