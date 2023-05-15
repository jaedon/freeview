/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Buffer.h"
#include "Security.h"

#include <stdint.h>

using namespace netflix;

static bool decode(const std::vector<unsigned char> &secret,
                   const std::vector<unsigned char> &cipherEnvelope,
                   const std::vector<unsigned char> &macEnvelope,
                   std::vector<unsigned char> &data)
{
    // There must be data to be processed.
    if (cipherEnvelope.size() == 0 || macEnvelope.size() == 0) {
        return false;
    }

    // Verify the mac envelope
    if (macEnvelope.size() != netflix::EnvelopeVersionSizeInBytes + netflix::HmacSizeInBytes) {
        return false;
    }
    if (macEnvelope[0] != 0x0
        || macEnvelope[1] != 0x0
        || macEnvelope[2] != 0x0
        || macEnvelope[3] != 0x1) {
        return false;
    }

    // Verify the cipher envelope
    if (cipherEnvelope.size() < netflix::EnvelopeVersionSizeInBytes + (2 * netflix::CipherBlockSizeInBytes)) {
        return false;
    }
    if (cipherEnvelope[0] != 0x0
        || cipherEnvelope[1] != 0x0
        || cipherEnvelope[2] != 0x0
        || cipherEnvelope[3] != 0x1) {
        return false;
    }

    // Derive the key material
    std::vector<unsigned char> cipherKey;
    std::vector<unsigned char> macKey;
    if (!netflix::deriveKeyMaterial(secret, cipherKey, macKey)) {
        return false;
    }

    // Compute the HMAC-SHA-256 value of the cipher envelope
    std::vector<unsigned char> hmac;
    if (!netflix::computeHmacSha256Value(macKey, cipherEnvelope, hmac)) {
        return false;
    }

    // Verify the computed hmac value matches the expected hmac value
    if (memcmp(&hmac[0], &macEnvelope[netflix::EnvelopeVersionSizeInBytes], netflix::HmacSizeInBytes)) {
        return false;
    }

    // Extract the iv.
    std::vector<unsigned char> iv;
    iv.insert(iv.begin(),
              cipherEnvelope.begin() + netflix::EnvelopeVersionSizeInBytes,
              cipherEnvelope.begin() + netflix::EnvelopeVersionSizeInBytes + netflix::CipherBlockSizeInBytes);

    // Prepare the space for storing the plain text
    size_t plainTextLen = cipherEnvelope.size() - (netflix::EnvelopeVersionSizeInBytes + iv.size());
    std::vector<unsigned char> plainText(plainTextLen, 0xFF);
    int lenFromUpdate = 0;
    int lenFromFinal = 0;

    // Decrypt the data.
    int result = 0;
    EVP_CIPHER_CTX cipherCtx;
    EVP_CIPHER_CTX_init(&cipherCtx);
    result = EVP_DecryptInit_ex(&cipherCtx, EVP_aes_128_cbc(), NULL, &cipherKey[0], &iv[0]);
    unsigned char *cipherText = const_cast<unsigned char *>(&cipherEnvelope[netflix::EnvelopeVersionSizeInBytes + iv.size()]);
    result = EVP_DecryptUpdate(&cipherCtx, &plainText[0], &lenFromUpdate, cipherText, plainText.size());
    result = EVP_DecryptFinal_ex(&cipherCtx, &plainText[lenFromUpdate], &lenFromFinal);
    EVP_CIPHER_CTX_cleanup(&cipherCtx);

    // If decryption failed, e.g. bad padding, then exit.
    if (result != 1) {
        return false;
    }

    // Resize the plaintext to the size of decryption.
    plainText.resize(lenFromUpdate + lenFromFinal);

    data = plainText;
    return true;
}

Buffer::Status Buffer::open(const std::string &filename, const std::vector<unsigned char> &secret)
{
    mPosition = 0;
    mData.clear();
    mHasError = false;

    FILE *f = fopen(filename.c_str(), "r");
    if (!f)
        return NoFileError;

    std::vector<unsigned char> data;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size > 0) {
        data.resize(size);
        fseek(f, 0, SEEK_SET);
        if (!fread(&data[0], size, 1, f)) {
            fclose(f);
            return ReadError;
        }
    }
    fclose(f);

    if (!data.empty()) {
        std::vector<unsigned char> hmac, cipherText;
        {
            Buffer buffer(data);

            uint32_t version;
            buffer >> version;
            if (version != Buffer::Version)
                return Buffer::VersionError;

            buffer >> hmac >> cipherText;
        }
        if (!decode(secret, cipherText, hmac, mData))
            return IntegrityError;
    }
    return Ok;
}
