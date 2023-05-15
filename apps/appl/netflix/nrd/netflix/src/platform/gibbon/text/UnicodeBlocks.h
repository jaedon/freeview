/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef UNICODE_BLOCKS_H_
#define UNICODE_BLOCKS_H_

#include <string>
#include <vector>

#include <stdint.h>

namespace netflix {
namespace gibbon {

class UnicodeBlocks
{
public:
    typedef std::vector<std::string> Fonts;

    struct Range
    {
        inline Range(uint32_t _start, uint32_t _end, const char *_name) : start(_start), end(_end), name(_name) { }
        Fonts fonts;
        const uint32_t start, end;
        const char *name;
    };
    static Range *find(uint32_t character);
    static void dumpFonts();
};

}}

#endif /* UNICODE_BLOCKS_H_ */
