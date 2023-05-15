/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef UNIONCAST_H_
#define UNIONCAST_H_

// This cast uses an union to circumvent the error
// "ISO C++ forbids casting between pointer-to-function and pointer-to-object"
// when using dlsym, among others. You should probably not rely much on this
// and change your code instead.

namespace netflix {
template <class SOURCE, class TARGET>
class union_cast
{
    union {
        SOURCE in;
        TARGET out;
    } values;
public:
    union_cast(SOURCE value) { values.in = value; }
    inline operator const TARGET() const { return values.out; }
};
}

#endif /* UNIONCAST_H_ */
