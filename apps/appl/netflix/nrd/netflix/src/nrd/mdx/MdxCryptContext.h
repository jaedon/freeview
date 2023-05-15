/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __CRYPT_CONTEXT_H__
#define __CRYPT_CONTEXT_H__

#include <vector>
#include <nrdbase/tr1.h>

namespace netflix {

class HMAC;

namespace mdx {

/**
 * @class CryptContext CryptContext.h
 * @brief A custom context to encrypt/decrypt data using keys extracted from a
 *        given shared secret
 */
class CryptContext
{
public:
    /**
     * Create a custom crypt context
     *
     * @param[in] sharedSecret The shared secret to extract keys from (Base64 encoded)
     *    The HMAC key is the first 32 bytes of the shared secret
     *    The Encryption key is the last 16 bytes of the shared secret
     */
    CryptContext(const std::vector<unsigned char> &sharedSecret);

    /**
     * Encrypt the given data
     *
     * @param[in] data The data to encrypt
     * @returns the resulting encrypted data (Base64 encoded)
     *     if unable to encrypt data, returns an empty vector
     */
    std::vector<unsigned char> encrypt(const std::vector<unsigned char> &data);

    /**
     * Decrypt the given data
     *
     * @param[in] data The data to decrypt (Base64 encoded)
     * @returns the resulting decrypted data
     *     if unable to decrypt data, returns an empty vector
     */
    std::vector<unsigned char> decrypt(const std::vector<unsigned char> &data);

    /**
     * Calculate the HMAC of the given data
     *
     * @param[out] hmac The calculated hmac
     * @param[in] data The data to calculate the HMAC of
     * @returns true if the HMAC was successfully calculated
     */
    bool calculateHmac(/*out*/std::vector<unsigned char> &hmac,
                       const std::vector<unsigned char> &data);

private:
    std::vector<unsigned char> mSharedSecret;
};

}} // namespace netflix::mdx

#endif // __CRYPT_CONTEXT_H__
