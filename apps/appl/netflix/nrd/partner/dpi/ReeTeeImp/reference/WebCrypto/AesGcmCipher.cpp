/*
 *
 *  Copyright 2014 Netflix, Inc.
 *
 *     Licensed under the Apache License, Version 2.0 (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *
 */

#include "AesGcmCipher.h"

#include <assert.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Log.h>

#include "ScopedOpenSSL.h"

#ifdef EVP_CTRL_GCM_SET_IVLEN
#define HAVE_GCM_OPENSSL
#endif

// TODO: Look into the new CRYPTO_gcm128_* OpenSSL routines that appeared in
// 1.0.1c. Much cleaner interface!

using namespace netflix;

//////////
//
// sizeOutput
//
//////////
static inline void sizeOutput(size_t inSize, /*out*/DataBuffer& outVuc)
{
    // size the output buffer
    // according to the openssl docs:
    // the amount of data written may be as large as (in.size() + cipher_block_size - 1)
    size_t outSize = inSize + AES_BLOCK_SIZE - 1;
    // the output buffer must also be a multiple of blocksize
    if ((outSize % AES_BLOCK_SIZE) != 0)
        outSize = ((outSize / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
    outVuc.resize(outSize);
}

//////////
//
// AesGcmCipher
//
//////////
AesGcmCipher::AesGcmCipher(const DataBuffer& key,
                           const DataBuffer& iv)
    :   key_(key),
        iv_(iv),
        keyLength_(static_cast<KeyLength>(key.size()))
{
}

AesGcmCipher::~AesGcmCipher()
{
}

//////////
//
// encrypt
//
//////////
bool AesGcmCipher::encrypt(const DataBuffer& clearText,
                           const DataBuffer& aad,
                           /*out*/DataBuffer& cipherText,
                           /*out*/DataBuffer& mac)
{
#ifndef HAVE_GCM_OPENSSL
    NRDP_UNUSED(clearText);
    NRDP_UNUSED(aad);
    NRDP_UNUSED(cipherText);
    NRDP_UNUSED(mac);
    Log::fatal(TRACE_CRYPTO, "AsmGcmCipher::encrypt requires a later version of OpenSSL");
    return false;
#else
    // create the cipher context; ScopedOpenSSL ensures deletion on scope exit
    ScopedOpenSSL<EVP_CIPHER_CTX, EVP_CIPHER_CTX_free> pCtx(EVP_CIPHER_CTX_new());

    // set cipher
    int ret = EVP_EncryptInit(pCtx.get(), getCipher(), NULL, NULL);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
                  "AesGcmCipher::encrypt: EVP_DecryptInit fail setting cipher");
        return false;
    }

    // set iv length
    ret = EVP_CIPHER_CTX_ctrl(pCtx.get(), EVP_CTRL_GCM_SET_IVLEN, iv_.size(), NULL);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO, "AesGcmCipher::decrypt: EVP_CIPHER_CTX_ctrl "
                  "EVP_CTRL_GCM_SET_IVLEN fail");
        return false;
    }

    // set key and iv
    ret = EVP_EncryptInit(pCtx.get(), NULL, &key_[0], &iv_[0]);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
                  "AesGcmCipher::decrypt: EVP_DecryptInit fail setting key/iv");
        return false;
    }

    // add Additional Authenticated Data (AAD)
    int nBytes = 0;
    ret = EVP_EncryptUpdate(pCtx.get(), NULL, &nBytes, &aad[0], aad.size());
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
                  "AesGcmCipher::decrypt: EVP_DecryptUpdate fail when setting AAD");
        return false;
    }

    // size the output buffer
    sizeOutput(clearText.size(), cipherText);

    // do the encrypt
    nBytes = 0;
    ret = EVP_EncryptUpdate(pCtx.get(), &cipherText[0], &nBytes, &clearText[0],
                            clearText.size());
    if (!ret)
    {
        cipherText.clear();
        Log::warn(TRACE_CRYPTO,
                  "AesGcmCipher::encrypt: EVP_DecryptUpdate fail during encrypt");
        return false;
    }
    NTRACE(TRACE_CRYPTO, "AesGcmCipher::encrypt: EVP_EncryptUpdate %u bytes in, "
           "%d bytes out", clearText.size(), nBytes);

    // finalization
    int nTotalBytes = nBytes;
    ret = EVP_EncryptFinal(pCtx.get(), &cipherText[0], &nBytes);
    if (!ret)
    {
        cipherText.clear();
        Log::warn(TRACE_CRYPTO, "AesGcmCipher::encrypt: EVP_EncryptFinal fail");
        return false;
    }
    nTotalBytes += nBytes;  // the actual clearText size is in nTotalBytes
    assert(nTotalBytes);
    cipherText.resize(nTotalBytes);

    // get the tag (MAC)
    const int TAG_LENGTH = 16;  // always want a 128-bit tag
    mac.resize(TAG_LENGTH);
    ret = EVP_CIPHER_CTX_ctrl(pCtx.get(), EVP_CTRL_GCM_GET_TAG, TAG_LENGTH, &mac[0]);
    if (!ret)
    {
        cipherText.clear();
        mac.clear();
        Log::warn(TRACE_CRYPTO,
                  "AesGcmCipher::encrypt: EVP_CIPHER_CTX_ctrl fail while getting tag");
        return false;
    }

    return true;
#endif
}

//////////
//
// decrypt
//
//////////
bool AesGcmCipher::decrypt(const DataBuffer& cipherText,
                           const DataBuffer& aad,
                           const DataBuffer& mac,
                           /*out*/DataBuffer& clearText)
{
#ifndef HAVE_GCM_OPENSSL
    NRDP_UNUSED(clearText);
    NRDP_UNUSED(aad);
    NRDP_UNUSED(cipherText);
    NRDP_UNUSED(mac);
    Log::fatal(TRACE_CRYPTO, "AsmGcmCipher::encrypt requires a later version of OpenSSL");
    return false;
#else
    // create the cipher context; ScopedOpenSSL ensures deletion on scope exit
    ScopedOpenSSL<EVP_CIPHER_CTX, EVP_CIPHER_CTX_free> pCtx(EVP_CIPHER_CTX_new());

    // set cipher
    int ret = EVP_DecryptInit(pCtx.get(), getCipher(), NULL, NULL);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
                  "AesGcmCipher::decrypt: EVP_DecryptInit fail setting cipher");
        return false;
    }

    // set iv length
    ret = EVP_CIPHER_CTX_ctrl(pCtx.get(), EVP_CTRL_GCM_SET_IVLEN, iv_.size(), NULL);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
            "AesGcmCipher::decrypt: EVP_CIPHER_CTX_ctrl EVP_CTRL_GCM_SET_IVLEN fail");
        return false;
    }

    // set key and iv
    ret = EVP_DecryptInit(pCtx.get(), NULL, &key_[0], &iv_[0]);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
            "AesGcmCipher::decrypt: EVP_DecryptInit fail setting key/iv");
        return false;
    }

    // set authentication tag (MAC)
    ret = EVP_CIPHER_CTX_ctrl(pCtx.get(), EVP_CTRL_GCM_SET_TAG, mac.size(),
                              (void*)&mac[0]);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
            "AesGcmCipher::decrypt: EVP_CIPHER_CTX_ctrl EVP_CTRL_GCM_SET_TAG fail");
        return false;
    }

    // add Additional Authenticated Data (AAD)
    int nBytes = 0;
    ret = EVP_DecryptUpdate(pCtx.get(), NULL, &nBytes, &aad[0], aad.size());
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
            "AesGcmCipher::decrypt: EVP_DecryptUpdate fail when setting AAD");
        return false;
    }

    // size the output buffer; do not put result in output until auth passes
    sizeOutput(cipherText.size(), clearText);

    // do the decrypt now
    nBytes = 0;
    ret = EVP_DecryptUpdate(pCtx.get(), &clearText[0], &nBytes, &cipherText[0],
                            cipherText.size());
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
                  "AesGcmCipher::decrypt: EVP_DecryptUpdate fail during decrypt");
        clearText.clear();
        return false;
    }
    NTRACE(TRACE_CRYPTO,
           "AesGcmCipher::decrypt: EVP_DecryptUpdate %u bytes in, %d bytes out",
           cipherText.size(), nBytes);

    // finalization and authentication; if ret != 1 authentication failed
    int nTotalBytes = nBytes;
    ret = EVP_DecryptFinal(pCtx.get(), &clearText[0], &nBytes);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO, "AesGcmCipher::decrypt: authentication failed");
        clearText.clear();
        return false;
    }
    nTotalBytes += nBytes;

    // the actual clearText size is in nTotalBytes
    assert(nTotalBytes);
    clearText.resize(nTotalBytes);

    return true;
#endif
}

//////////
//
// getCipher
//
//////////
const EVP_CIPHER* AesGcmCipher::getCipher() const
{
    const EVP_CIPHER* cipher = NULL;
    int cipherLen = 0;
    switch (keyLength_)
    {
#ifdef HAVE_GCM_OPENSSL
        case KL128: cipherLen = 128; cipher = EVP_aes_128_gcm(); break;
        case KL192: cipherLen = 192; cipher = EVP_aes_192_gcm(); break;
        case KL256: cipherLen = 256; cipher = EVP_aes_256_gcm(); break;
#endif
        default:    assert(false);                               break;
    }
    NTRACE(TRACE_CRYPTO, "AesGcmCipher::getCipher: keyLen = %d", cipherLen);
    (void)cipherLen;
    return cipher;
}
