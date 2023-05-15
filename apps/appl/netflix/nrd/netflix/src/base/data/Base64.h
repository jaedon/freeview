/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Netflix_Base_Base64_h_
#define Netflix_Base_Base64_h_

#include <cstddef> // for std::size_t
#include <string>
#include <vector>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/StdInt.h>
#include <nrdbase/Compiler.h>

namespace {
static signed char const CHAR_MAP[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static signed char const URL_CHAR_MAP[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
static signed char const EQUAL = '=';
static signed char const PERIOD = '.';
} // namespace anonymous

namespace netflix {
namespace Base64 {
namespace Private {

template <typename InputIterator_, typename OutputIterator_, std::size_t N_>
OutputIterator_ toBase64(InputIterator_ begin, InputIterator_ end,
                         OutputIterator_ out,  signed char const (&table)[N_], signed char const pad) {
    NRDP_STATIC_ASSERT((N_ >= 64)); // lookup table must be this big
    InputIterator_ p = begin;
    while (p != end) {
        signed char buf[4] = { pad, pad, pad, pad };
        uint8_t const v = static_cast<uint8_t>(*p);
        buf[0] = table[v >> 2];
        int index1 = (v & 0x3) << 4; // must defer
        if (++p != end) {
            uint8_t const v = static_cast<uint8_t>(*p);
            index1 |= v >> 4;
            int index2 = (v & 0xf) << 2; // must defer
            if (++p != end) {
                uint8_t const v = static_cast<uint8_t>(*p);
                index2 |= v >> 6;
                buf[3] = table[v & 0x3f];
                ++p;
            }
            buf[2] = table[index2];
        }
        buf[1] = table[index1];
        int length = sizeof(buf) - 1;
        if (!pad) {
            while (length >= 0 && buf[length] == 0)
                --length;
        }
        out = std::copy(buf, buf + length + 1, out);
    }
    return out;
}

template <typename InputIterator_, typename OutputIterator_>
OutputIterator_ fromBase64(InputIterator_ begin, InputIterator_ end,
                           OutputIterator_ out) {
    static signed char const REVERSE[] = { // reverse LUT for base-64 decoding
        -1,-1,-1,-1,-1,-1,-1,-1,    -1,-1,99,-1,-1,99,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,    -1,-1,-1,-1,-1,-1,-1,-1,
        //    !  "  #  $  %  &  '      (  )  *  +  ,  -  .  /
        -1,-1,-1,-1,-1,-1,-1,-1,    -1,-1,-1,62,-1,62,-1,63,
        // 0  1  2  3  4  5  6  7      8  9  :  ;  <  =  >  ?
        52,53,54,55,56,57,58,59,    60,61,-1,-1,-1,-1,-1,-1,
        //    A  B  C  D  E  F  G      H  I  J  K  L  M  N  O
        -1, 0, 1, 2, 3, 4, 5, 6,     7, 8, 9,10,11,12,13,14,
        // P  Q  R  S  T  U  V  W      X  Y  Z  [  \  ]  ^  _
        15,16,17,18,19,20,21,22,    23,24,25,-1,-1,-1,-1,63,
        // @  a  b  c  d  e  f  g      h  i  j  k  l  m  n  o
        -1,26,27,28,29,30,31,32,    33,34,35,36,37,38,39,40,
        // p  q  r  s  t  u  v  w      x  y  z  {  |  }  ~
        41,42,43,44,45,46,47,48,    49,50,51,-1,-1,-1,-1,-1
    };

    InputIterator_ p = begin;
    while (p != end) {
        // Attempt to obtain 4 encoded bytes of information.
        int index[4] = { -1, -1, -1, -1 };
        for (int i = 0; i < 4 && p != end; ++i) {
            index[i] = REVERSE[*p++ & 0x7f];
            if (index[i] >= 64) { --i; continue; } // ignore certain whitespace
        }

        // Convert the 4 bytes of encoded information to 3 bytes of decoded
        // information.  Stop decoding at the first instance of -1.  Much of the
        // following code assumes incorrect input, which complicates things.
        if (index[0] < 0) break;
        { // artificial scoping
            bool const at_end = index[1] < 0;
            *out++ = (index[0] << 2) | (at_end ? 0 : (index[1] >> 4));
            if (at_end) break;
        }
        if (index[2] < 0) break;
        *out++ = ((index[1] & 0xf) << 4) | (index[2] >> 2);
        if (index[3] < 0) break;
        *out++ = ((index[2] & 0x3) << 6) | index[3];
    }
    return out;
}
}

inline size_t decode_reserve(size_t size)
{
    return (((size + 3) / 4) * 3);
}

inline size_t encode_reserve(size_t size)
{
    return (((size + 2) / 3) * 4);
}

/**
 * Return the result of a base-64 encoding operation on the range specified by
 * the supplied iterators.  This function generates a standard base-64
 * encoding.  The supplied input and output buffers should not overlap.
 * @param begin the beginning of the unencoded data.
 * @param end the end of the unencoded data.
 * @param out the target for the encoded result.
 * @return the end of the encoded result.
 */
template <typename InputIterator_, typename OutputIterator_>
OutputIterator_ encode(InputIterator_ begin, InputIterator_ end,
                       OutputIterator_ out, bool urlSafe = false)
{
    return Private::toBase64(begin, end, out,
                             (urlSafe ? URL_CHAR_MAP : CHAR_MAP),
                             (urlSafe ? 0 : EQUAL));
}

/**
 * Return the result of a base-64 decoding operation on the range specified by
 * the supplied iterators.  This function will successfully decode base-64
 * encodings that result from any encoding functions in this library.  The
 * supplied input and output buffers should not overlap.
 * @param begin the beginning of the encoded data.
 * @param end the end of the encoded data.
 * @param out the target for the decoded result.
 * @return the end of the decoded result.
 */
template <typename InputIterator_, typename OutputIterator_>
OutputIterator_ decode(InputIterator_ begin, InputIterator_ end, OutputIterator_ out)
{
    return Private::fromBase64(begin, end, out);
}

/** This is a macro function for encode. */
std::string encode(const std::string &s, bool urlSafe = false);

/** This is a macro function for decode. */
std::string decode(const std::string &s);

/** This is a macro function for encode. */
DataBuffer encode(const DataBuffer &s, bool urlSafe = false);

/** This is a macro function for decode. */
DataBuffer decode(const DataBuffer &s);

/** This is a macro function for encode. */
std::vector<unsigned char> encode(const std::vector<unsigned char> &s, bool urlSafe = false);

/** This is a macro function for decode. */
std::vector<unsigned char> decode(const std::vector<unsigned char> & s);

}} // namespace Base64

#endif // Netflix_Base_Base64_h_
