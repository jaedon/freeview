/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SECURITY_H__
#define __SECURITY_H__

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

namespace netflix {

enum {
    SecretSizeInBytes = 48,
    CipherBlockSizeInBytes = 16,
    HmacSizeInBytes = 32,
    EnvelopeVersionSizeInBytes = 4
};

static inline bool deriveKeyMaterial(const std::vector<unsigned char> &secret,
                                     std::vector<unsigned char> &cipherKey,
                                     std::vector<unsigned char> &macKey)
{
    // The secret MUST be 48 byes.
    if (secret.size() != SecretSizeInBytes)
        return false;

    // First 16 bytes become the cipher key.
    cipherKey.insert(cipherKey.begin(), secret.begin(), secret.begin() + 16);

    // Last 32 bytes become the mac key.
    macKey.insert(macKey.begin(), secret.begin() + 16, secret.begin() + 48);

    return true;
}
static inline bool computeHmacSha256Value(std::vector<unsigned char> &macKey,
                                          const std::vector<unsigned char> &in,
                                          std::vector<unsigned char> &out)
{
    std::vector<unsigned char> hmac(HmacSizeInBytes, 0);
    unsigned int lenFromFinal = 0;

    // Compute the hmac value.
    HMAC_CTX hmacCtx;
    HMAC_Init(&hmacCtx, &macKey[0], macKey.size(), EVP_sha256());
    HMAC_Update(&hmacCtx, &in[0], in.size());
    HMAC_Final(&hmacCtx, &hmac[0], &lenFromFinal);
    HMAC_CTX_cleanup(&hmacCtx);

    // Sanity check the computed hmac value is of expected size.
    if (lenFromFinal != HmacSizeInBytes)
        return false;

    out = hmac;
    return true;
}

}

#endif /* __SECURITY_H__ */
