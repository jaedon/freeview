/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_COMMON_H
#define DEVICE_COMMON_H

#include <nrdbase/StdInt.h>

#define _FOURCC_LE_(a,b,c,d)  (a | (b << 8) | (c << 16) | (d << 24))
#define _FOURCC_BE_(a,b,c,d)  (d | (c << 8) | (b << 16) | (a << 24))

namespace netflix {
namespace device {

static const int64_t  INVALID_TIMESTAMP = (  -1);
static const uint32_t TIMESTAMP_FREQ_HZ = (1000);

// Access Unit Flags
static const uint32_t AU_KEY_FRAME     = (1 << 0);
static const uint32_t AU_DISCONTINUITY = (1 << 1);
static const uint32_t AU_END_OF_STREAM = (1 << 2);

struct AccessUnitAttributes
{
    uint32_t flags_;
    uint32_t manifestIndex_;
    uint32_t streamIndex_;
    int64_t  DTS_;
    int64_t  PTS_;
};

class VolatileInt60
{
private:

    //
    // The stored value has 60 bits, and it is stored in two 32-bit (atomic-access)
    // variables, as depicted below:
    //
    //     valueLSB_: [xx][ b29 ~ b0  ]
    //     valueMSB_: [yy][ b59 ~ b30 ]
    //
    // NOTE: 1. [xx] and [yy] are 2-bit serial numbers that should be checked to make
    //          sure the MSB/LSB are in-sync.
    //
    //       2. "b59" should represent the sign-bit.
    //

    uint32_t volatile valueLSB_;
    uint32_t volatile valueMSB_;
    bool   volatile valid_;
    uint32_t serialNumberMask_;

public:
    enum RESULT { VINT_OK, VINT_INVALID, VINT_BUSY };

    VolatileInt60() : valueLSB_(0), valueMSB_(0), valid_(false), serialNumberMask_(0) {}

    RESULT getValue(int64_t& value, uint32_t& serial) const
    {
        if(!valid_)
            return VINT_INVALID;

        uint32_t LSB = valueLSB_;
        uint32_t MSB = valueMSB_;
        uint32_t ser = (LSB & 0xC0000000);

        if((MSB & 0xC0000000) != ser) // serial number checking failed
            return VINT_BUSY;

        value = (MSB & 0x3FFFFFFF);
        value <<= 30;
        value |= (LSB & 0x3FFFFFFF);

        if((MSB & 0x20000000) != 0) // "b59" is 1, treat it as negative
            value |= 0xF000000000000000LL;

        serial = (ser >> 30);
        return VINT_OK;
    }

    void setValue(int64_t value)
    {
        // The four highest bits are discarded from the value to store.

        valueMSB_ = (static_cast<uint32_t>(value >> 30) & 0x3FFFFFFF) | serialNumberMask_;
        valueLSB_ = (static_cast<uint32_t>(value      ) & 0x3FFFFFFF) | serialNumberMask_;

        valid_ = true;
        serialNumberMask_ += 0x40000000;
    }

    void invalidate() { valid_ = false; }
};

}} // namespace netflix::device

#endif // DEVICE_COMMON_H
