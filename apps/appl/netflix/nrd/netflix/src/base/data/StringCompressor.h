/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __STRINGCOMPRESSOR_H__
#define __STRINGCOMPRESSOR_H__

#include <string>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/NFErr.h>

namespace netflix {

// helper class to deflating and inflating strings

class StringCompressor
{
public:
    // deflates the source string to the destination string
    // the B64 version will deflate the src but the b64 encode the dst
    // the Gzip version will add a gzip header & trailer to the dst
    static netflix::NFErr deflate(std::string &dst, const std::string &src);
    static netflix::NFErr deflateB64(std::string &dst, const std::string &src);
    static netflix::NFErr deflateGzip(std::string &dst, const std::string &src);

    static netflix::NFErr deflate(DataBuffer &dst, const DataBuffer &src);
    static netflix::NFErr deflateB64(DataBuffer &dst, const DataBuffer &src);
    static netflix::NFErr deflateGzip(DataBuffer &dst, const DataBuffer &src);

    static netflix::NFErr inflate(std::string &dst, const std::string &src);
    static netflix::NFErr inflateB64(std::string &dst, const std::string &src);
    static netflix::NFErr inflateGzip(std::string &dst, const std::string &src);

    static netflix::NFErr inflate(DataBuffer &dst, const DataBuffer &src);
    static netflix::NFErr inflateB64(DataBuffer &dst, const DataBuffer &src);
    static netflix::NFErr inflateGzip(DataBuffer &dst, const DataBuffer &src);

};


}

#endif // __STRINGCOMPRESSOR_H__
