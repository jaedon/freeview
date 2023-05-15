/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef URLENCODER_H
#define URLENCODER_H

#include <string>
#include <sstream>
#include <stdio.h>
#include <limits.h>

namespace netflix {

class UrlEncoder
{
public:
    static std::string decode(const std::string &str) { return decode<std::string>(str.c_str(), str.size()); }
    template <typename T>
    static T decode(const char *src, int len = INT_MAX)
    {
        if (!src)
            return T();
        bool bailOut = false;
        T dst;
        if (len != INT_MAX)
            dst.reserve(len);
        while (*src && len-- > 0) {
            switch (*src) {
            case '+': // + is space
                src++;
                dst += ' ';
                break;
            case '%': // Hex digit syntax
                src++;
                if (src[0] && src[1]) {
                    const unsigned char ch = (hexValue(src[0]) * 16) + hexValue(src[1]);
                    dst += ch;
                    src += 2;
                } else {
                    bailOut = true;
                }
                break;
            default:
                dst += *src++;
                break;
            }
            if (bailOut)
                break;
        }
        return dst;
    }
    static std::string encode(const std::string &str) { return encode<std::string>(str.c_str(), str.size()); }

    template <typename T>
    static T encode(const char *value, int len = INT_MAX)
    {
        if (!value)
            return T();

        enum { BufSize = 4 };
        char buf[BufSize];

        T ret;
        if (len != INT_MAX)
            ret.reserve(len);

        while (*value && len-- > 0) {
            char c = *value++;

            switch (c) {
                // The "unreserved" characters are copied as-is along with: '$', '+', '!', '*', '\', '(',
                // ')', and ','. See RFC 1738.
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '-':
            case '_':
            case '.':
            case '!':
            case '~':
            case '*':
            case '\'':
            case '(':
            case ')':
                // This adds one character.
                ret += c;
                break;

                // Everything else converts 'c' to '%##', increasing the
                // final string length by two.
            default:
                snprintf(buf, BufSize, "%%%02X", c);
                ret.append(buf, 3);
                // Insert character as hex. This adds three characters.
                break;
            }
        }

        return ret;
    }
private:
    static unsigned char hexValue(unsigned char c)
    {
        if (c >= 'A' && c <= 'F')
            return 10 + (c - 'A');
        if (c >= 'a' && c <= 'f')
            return 10 + (c - 'a');
        return c - '0';
    }
    UrlEncoder() {}
};

}

#endif // URLENCODER_H
