/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef StringTokenizer_h
#define StringTokenizer_h

#include <string>
#include <vector>

namespace netflix {


/**
 * Utility for tokenizing a string.
 */
class StringTokenizer
{
public:
    static std::string trim(const std::string &s, const std::string &w = " \t");
    enum TokenizeFlag {
        Tokenize_None = 0x0,
        Tokenize_KeepEmpty = 0x1
    };

    static void tokenize(const std::string &text,
                         std::vector<std::string> &tokens,
                         const std::string &delimiters = " ",
                         unsigned int flags = Tokenize_None);
    static void tokenize(const std::string &text,
                         std::vector<std::string> &tokens,
                         char delimiter,
                         unsigned int flags = Tokenize_None)
    {
        tokenize(text, tokens, std::string(&delimiter, 1), flags);
    }

    static inline std::string join(const std::vector<std::string> &strings, const std::string &delimiter)
    {
        std::string ret;
        const int count = strings.size();
        for (int i=0; i<count; ++i) {
            if (i)
                ret += delimiter;
            ret += strings.at(i);
        }
        return ret;
    }

private:

    StringTokenizer();
};

}

#endif
