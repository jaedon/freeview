/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/StringCompressor.h>

#include <zlib.h>
#include <nrdbase/Base64.h>

using namespace netflix;

#define BUFFER_SIZE 1024

//////////
//
// deflate
//
//////////
template <typename T>
static NFErr deflate(T &dst, const T &src)
{
    // setup zlib context with default allocators
    z_stream zctx;
    zctx.zalloc = Z_NULL;
    zctx.zfree = Z_NULL;
    zctx.opaque = Z_NULL;
    zctx.avail_in = 0;
    zctx.next_in = Z_NULL;

    // point context at the source string
    zctx.next_in = (Bytef *)src.data();
    zctx.avail_in = src.length();

    // init zlib library for deflation at max compression
    if (deflateInit(&zctx, Z_BEST_COMPRESSION) != Z_OK)
        return NFErr_Bad;

    // temporary buffer for deflated data
    char buffer[BUFFER_SIZE];

    // clear the destination string
    dst.clear();
    dst.reserve(src.size() / 2);

    int zerr = 0;
    do
    {
        // point context at the output buffer
        zctx.next_out = (Bytef *)buffer;
        zctx.avail_out = BUFFER_SIZE;

        zerr = ::deflate(&zctx, Z_FINISH);
        if ((zerr != Z_OK) && (zerr != Z_STREAM_END))
        {
            deflateEnd(&zctx); // don't leak the context memory
            return NFErr_Bad;
        }

        // output processed data to dst string
        int processed = BUFFER_SIZE - zctx.avail_out;
        dst.append(buffer, processed);
    }
    while (zerr != Z_STREAM_END);

    // free context memory
    deflateEnd(&zctx);
    dst.resize(dst.size());

    return NFErr_OK;
}

template <typename T>
static NFErr deflateGzip(T &dst, const T &src)
{
    // setup zlib context with default allocators
    z_stream zctx;
    zctx.zalloc = Z_NULL;
    zctx.zfree = Z_NULL;
    zctx.opaque = Z_NULL;

    // point context at the source string
    zctx.next_in = (Bytef *)src.data();
    zctx.avail_in = src.length();

    // init zlib library for deflation at max compression
    if (deflateInit2(&zctx,
                     Z_BEST_COMPRESSION,
                     Z_DEFLATED,
                     (15 + 16),
                     9,
                     Z_DEFAULT_STRATEGY ) != Z_OK)
    {
        return NFErr_Bad;
    }

    // temporary buffer for deflated data
    char buffer[BUFFER_SIZE];

    // clear the destination string
    dst.clear();
    dst.reserve(src.size() / 2);

    int zerr = 0;
    do
    {
        // point context at the output buffer
        zctx.next_out = (Bytef *)buffer;
        zctx.avail_out = BUFFER_SIZE;

        zerr = ::deflate(&zctx, Z_FINISH);
        if ((zerr != Z_OK) && (zerr != Z_STREAM_END))
        {
            deflateEnd(&zctx); // don't leak the context memory
            return NFErr_Bad;
        }

        // output processed data to dst string
        int processed = BUFFER_SIZE - zctx.avail_out;
        dst.append(buffer, processed);
    }
    while (zerr != Z_STREAM_END);

    // free context memory
    deflateEnd(&zctx);
    dst.resize(dst.size());

    return NFErr_OK;
}

NFErr StringCompressor::deflate(std::string &dst, const std::string &src)
{
    return ::deflate<std::string>(dst, src);
}

NFErr StringCompressor::deflateGzip(std::string &dst, const std::string &src)
{
    return ::deflateGzip<std::string>(dst, src);
}


NFErr StringCompressor::deflateB64(std::string &dst, const std::string &src)
{
    // first deflate the src
    std::string compressed;
    NFErr err = deflate(compressed, src);
    if (err != NFErr_OK)
        return err;

    // then bas64 encode it
    dst = Base64::encode(compressed);

    return NFErr_OK;
}


NFErr StringCompressor::deflate(DataBuffer &dst, const DataBuffer &src)
{
    return ::deflate<DataBuffer>(dst, src);
}

NFErr StringCompressor::deflateGzip(DataBuffer &dst, const DataBuffer &src)
{
    return ::deflateGzip<DataBuffer>(dst, src);
}

NFErr StringCompressor::deflateB64(DataBuffer &dst, const DataBuffer &src)
{
    // first deflate the src
    DataBuffer compressed;
    const NFErr err = deflate(compressed, src);
    if (err != NFErr_OK)
        return err;

    // then bas64 encode it
    dst = compressed.encode(DataBuffer::Encoding_Base64);

    return NFErr_OK;
}


//////////
//
// inflate
//
//////////

// file-scoped function that knows how to deflate gzip and non-gzip
template <typename T>
static NFErr inflate_(T &dst, const T &src, bool isGzip)
{
    // setup zlib context with default allocators
    z_stream zctx;
    zctx.zalloc = Z_NULL;
    zctx.zfree = Z_NULL;
    zctx.opaque = Z_NULL;
    zctx.avail_in = 0;
    zctx.next_in = Z_NULL;

    // init zlib library for inflation
    if (isGzip)
    {
        if (inflateInit2(&zctx, 16+MAX_WBITS) != Z_OK)
            return NFErr_Bad;
        if (src.size() >= 4) {
            // See http://www.onicos.com/staff/iz/formats/gzip.html
            // The last 4 bytes indicate the size of the uncompressed data
            const unsigned char *buf = reinterpret_cast<const unsigned char*>(src.c_str() + (src.size() - 4));
            unsigned int num = buf[0];
            num |= static_cast<unsigned int>(buf[1]) << 8;
            num |= static_cast<unsigned int>(buf[2]) << 16;
            num |= static_cast<unsigned int>(buf[3]) << 24;
            if (num < 1024 * 1024 * 5) {
                dst.reserve(num);
            }
        }
    }
    else
    {
        if (inflateInit(&zctx) != Z_OK)
            return NFErr_Bad;
    }

    // point context at the source string
    zctx.next_in = (Bytef *)src.data();
    zctx.avail_in = src.length();

    // temporary buffer for deflated data
    char buffer[BUFFER_SIZE];

    // clear the destination std::string
    dst.clear();

    int zerr = 0;
    do
    {
        // point context at the output buffer
        zctx.next_out = (Bytef *)buffer;
        zctx.avail_out = BUFFER_SIZE;

        zerr = ::inflate(&zctx, Z_SYNC_FLUSH);
        if ((zerr != Z_OK) && (zerr != Z_STREAM_END))
        {
            inflateEnd(&zctx); // don't leak the context memory
            return NFErr_Bad;
        }

        // output processed data to dst std::string
        int processed = BUFFER_SIZE - zctx.avail_out;
        dst.append(buffer, processed);
    }
    while (zerr != Z_STREAM_END);

    // free context memory
    inflateEnd(&zctx);

    return NFErr_OK;
}

NFErr StringCompressor::inflate(std::string &dst, const std::string &src)
{
    return inflate_(dst, src, false);   // false means non-gzip
}

NFErr StringCompressor::inflateB64(std::string &dst, const std::string &src)
{
    // first decode the src
    std::string decoded(Base64::decode(src));

    // then inflate it
    return inflate(dst, decoded);
}

NFErr StringCompressor::inflateGzip(std::string &dst, const std::string &src)
{
    return inflate_(dst, src, true);    // true means gzip
}

NFErr StringCompressor::inflate(DataBuffer &dst, const DataBuffer &src)
{
    return inflate_(dst, src, false);   // false means non-gzip
}

NFErr StringCompressor::inflateB64(DataBuffer &dst, const DataBuffer &src)
{
    // first decode the src
    DataBuffer decoded = src.decode(DataBuffer::Encoding_Base64);

    // then inflate it
    return inflate(dst, decoded);
}

NFErr StringCompressor::inflateGzip(DataBuffer &dst, const DataBuffer &src)
{
    return inflate_(dst, src, true);    // true means gzip
}
