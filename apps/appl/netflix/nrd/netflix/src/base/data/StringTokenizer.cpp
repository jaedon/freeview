/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "StringTokenizer.h"
#include <stdio.h>

using namespace netflix;

std::string StringTokenizer::trim(const std::string &s, const std::string &w)
{
    const size_t beginStr = s.find_first_not_of(w);
    if (beginStr == std::string::npos) // no content
        return "";
    const size_t endStr = s.find_last_not_of(w);
    const size_t range = endStr - beginStr + 1;
    return s.substr(beginStr, range);
}

void StringTokenizer::tokenize(const std::string &text,
                               std::vector<std::string> &tokens,
                               const std::string &delimiters, unsigned int flags)
{
    size_t last = 0;
    while (true) {
        const size_t next = text.find_first_of(delimiters, last);
        if (next == std::string::npos)
            break;
        if (next - last || (flags & Tokenize_KeepEmpty))
            tokens.push_back(text.substr(last, next - last));
        last = next+1;
    }
    if (last < text.size() || (flags & Tokenize_KeepEmpty))
        tokens.push_back(text.substr(last));
}
