/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef __DIGESTALGO_H__
#define __DIGESTALGO_H__

#include <string>
#include <openssl/evp.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Noncopyable.h>

namespace netflix {

/**
 * Digest Algorithm.
 */
class DigestAlgo : private Noncopyable {
public:
    static shared_ptr<const DigestAlgo> SHA1();
    static shared_ptr<const DigestAlgo> SHA224();
    static shared_ptr<const DigestAlgo> SHA256();
    static shared_ptr<const DigestAlgo> SHA384();
    static shared_ptr<const DigestAlgo> SHA512();

    int nid() const { return nid_; }
    const EVP_MD * evp_md() const { return evp_md_; }
    std::string toString() const { return name_; }

private:
    DigestAlgo(const std::string name, int nid, const EVP_MD *evp_md);
    const std::string name_;
    const int nid_;
    const EVP_MD *evp_md_;
};

} // namespace netflix

#endif // __DIGESTALGO_H__
