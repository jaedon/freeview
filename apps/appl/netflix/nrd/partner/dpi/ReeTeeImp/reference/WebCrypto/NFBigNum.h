/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef __BIGNUM_H__
#define __BIGNUM_H__

#include <string>
#include <openssl/bn.h>
#include <nrdbase/DataBuffer.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>

#include <nrdbase/tr1.h>

namespace netflix {

/**
 * C++ wrapper of OpenSSL BIGNUM values.
 * @author mzollinger.
 */
class BigNum {
public:
    explicit BigNum(const DataBuffer &data);
    explicit BigNum(const BIGNUM *val = 0);
    BigNum(const unsigned char *buf, size_t sz);
    BigNum(const BigNum &rhs);
    bool operator==(const BigNum &rhs) const;
    bool operator!=(const BigNum &rhs) const;
    DataBuffer encode() const;
    size_t size() const;
    const BIGNUM * getBIGNUM() const;
private:
    shared_ptr<const BIGNUM> mValue;
};

inline BigNum::BigNum(const BIGNUM *val)
    : mValue(val ? BN_dup(val) : BN_new(), BN_free)
{
    assert(mValue);
}

inline BigNum::BigNum(const DataBuffer &data)
    : mValue(BN_bin2bn(&data[0], data.size(), NULL), BN_free)
{
    assert(mValue);
}

inline BigNum::BigNum(const unsigned char *buf, size_t sz)
    : mValue(BN_bin2bn(buf, sz, NULL), BN_free)
{
    assert(mValue);
}

inline BigNum::BigNum(const BigNum &rhs)
    : mValue(BN_dup(rhs.mValue.get()), BN_free)
{
    assert(mValue);
}

inline bool BigNum::operator==(const BigNum &rhs) const
{
    return BN_cmp(mValue.get(), rhs.mValue.get()) == 0;
}

inline bool BigNum::operator!=(const BigNum &rhs) const
{
    return !(operator==(rhs));
}

inline DataBuffer BigNum::encode() const
{
    DataBuffer buf;
    buf.resize(size());
    BN_bn2bin(mValue.get(), &buf[0]);
    return buf;
}

inline size_t BigNum::size() const
{
    return BN_num_bytes(mValue.get());
}

inline const BIGNUM * BigNum::getBIGNUM() const
{
    return mValue.get();
}


} // namespace netflix::crypto

#endif // __BIGNUM_H__
