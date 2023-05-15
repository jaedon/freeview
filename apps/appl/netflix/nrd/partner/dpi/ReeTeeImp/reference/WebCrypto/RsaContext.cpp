/*
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
#include "RsaContext.h"

#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include <nrdbase/Log.h>

#include <nrdbase/DigestAlgo.h>
#include <nrdbase/Digester.h>

#include "ScopedOpenSSL.h"
#include "NFBigNum.h"

using namespace netflix;

static inline int nidSha(RsaContext::ShaAlgo shaAlgo)
{
    switch(shaAlgo)
    {
        case RsaContext::SHA1:   return NID_sha1;
        case RsaContext::SHA224: return NID_sha224;
        case RsaContext::SHA256: return NID_sha256;
        case RsaContext::SHA384: return NID_sha384;
        case RsaContext::SHA512: return NID_sha512;
        default:
            assert(false);
            break;
    }
    return NID_sha256;  // required to make compiler happy
}

//////////
//
// RsaContext
//
//////////
RsaContext::RsaContext()
    : pOsslRsa_(NULL),
      hasPrivateKey_(false),
      hasPublicKey_(false)
{
}

RsaContext::~RsaContext()
{
    if (pOsslRsa_)
        RSA_free(pOsslRsa_);
}

//////////
//
// generate
//
//////////
bool RsaContext::generate(uint32_t nBits, uint64_t publicExponent)
{
    // make sure publicExponent is odd
    publicExponent |= 1;

    // this method clobbers any existing context
    if (pOsslRsa_)
        RSA_free(pOsslRsa_);

    // The OpenSSL spec says the publicExponent must be odd. Yung found that
    // RSA_generate_key hangs if this is not true, so we better make it true.
    // Typical values are 3, 17 or 65537, with 65537 the most common.
    publicExponent |= (uint64_t)1;

    bool keygenSuccess = false;
    uint32_t retryCount = 0;
    const uint32_t MAX_RETRIES=4;
    while (!keygenSuccess && (retryCount < MAX_RETRIES))
    {
        pOsslRsa_ = RSA_generate_key(nBits, publicExponent, 0, 0);
        if (pOsslRsa_)
            keygenSuccess = (RSA_check_key(pOsslRsa_) == 1);
        if (!keygenSuccess && pOsslRsa_)
            RSA_free(pOsslRsa_);
        retryCount++;
    }

    if (keygenSuccess)
    {
        hasPrivateKey_ = true;
        hasPublicKey_  = true;
#ifdef BUILD_DEBUG
        DataBuffer publicKey;
        getPublicPkcs1(publicKey);
        DataBuffer privateKey;
        getPrivatePkcs1(privateKey);
        NTRACE(TRACE_CRYPTO, "RsaContext::generate: public key = %s",
               publicKey.toHex().c_str());
        NTRACE(TRACE_CRYPTO, "RsaContext::generate: private key = %s",
               privateKey.toHex().c_str());
#endif
    }
    return keygenSuccess;
}

//////////
//
// setRaw
//
//////////
bool RsaContext::setRaw(const DataBuffer &pubMod,
                        const DataBuffer &pubExp,
                        const DataBuffer &privMod)
{
    // I'm not sure how setting values directly in the RSA structure affects the
    // integrity of any existing data there, so best to start empty.
    if (pOsslRsa_)
        RSA_free(pOsslRsa_);
    pOsslRsa_ = RSA_new();
    // The RSA struct takes ownership of the input bignums, so we need to dup
    pOsslRsa_->n = BN_dup(BigNum(pubMod.data(), pubMod.size()).getBIGNUM());
    pOsslRsa_->e = BN_dup(BigNum(pubExp.data(), pubExp.size()).getBIGNUM());
    if (!privMod.empty())
    {
        pOsslRsa_->d = BN_dup(BigNum(privMod.data(), privMod.size()).getBIGNUM());
        hasPrivateKey_ = true;
    }
    else
    {
        pOsslRsa_->d = NULL;
    }
    pOsslRsa_->p = NULL;
    pOsslRsa_->q = NULL;
    hasPublicKey_ = true;
    return true;
}

//////////
//
// getPublicRaw
//
//////////
bool RsaContext::getPublicRaw(/*out*/DataBuffer &pubMod,
                              /*out*/DataBuffer &pubExp)
{
    if (!hasPublicKey_)
        return false;
    pubMod = BigNum(pOsslRsa_->n).encode();
    pubExp = BigNum(pOsslRsa_->e).encode();
    return true;
}

//////////
//
// setPublicPkcs1
//
//////////
bool RsaContext::setPublicPkcs1(const DataBuffer &pubKeyDer)
{
    NTRACE(TRACE_CRYPTO, "RsaContext::setPublicPkcs1: key = %s",
           pubKeyDer.toHex().c_str());

    const unsigned char * buf = &pubKeyDer[0];
    RSA *rsa = d2i_RSAPublicKey(NULL, &buf, pubKeyDer.size());
    if (rsa == NULL) {
        Log::error(TRACE_LOG, "[%s:%d]: if (rsa == NULL) {", __func__, __LINE__);
        return false;
    }
    if (pOsslRsa_)
        RSA_free(pOsslRsa_);
    pOsslRsa_ = rsa;
    hasPublicKey_ = true;
    return true;
}

//////////
//
// setPrivatePkcs1
//
//////////
bool RsaContext::setPrivatePkcs1(const DataBuffer &privKeyDer)
{
    NTRACE(TRACE_CRYPTO, "RsaContext::setPrivatePkcs1: key = %s",
           privKeyDer.toHex().c_str());

    const unsigned char * buf = &privKeyDer[0];
    RSA *rsa = d2i_RSAPrivateKey(NULL, &buf, privKeyDer.size());
    if (rsa == NULL)
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::setPrivatePkcs1: d2i_RSAPrivateKey failed");
        return false;
    }
    if (pOsslRsa_)
        RSA_free(pOsslRsa_);
    pOsslRsa_ = rsa;
    hasPrivateKey_ = true;
    return true;
}

//////////
//
// getPublicPkcs1
//
//////////
bool RsaContext::getPublicPkcs1(/*out*/DataBuffer &pubKeyDer) const
{
    if (!hasPublicKey_)
        return false;
    int keyLen = i2d_RSAPublicKey(pOsslRsa_, NULL);
    pubKeyDer.resize(keyLen);
    unsigned char * buf = &pubKeyDer[0];
    i2d_RSAPublicKey(pOsslRsa_, &buf);
    return true;
}

//////////
//
// getPrivatePkcs1
//
//////////
bool RsaContext::getPrivatePkcs1(/*out*/DataBuffer &privKeyDer) const
{
    if (!hasPrivateKey_)
        return false;
    int keyLen = i2d_RSAPrivateKey(pOsslRsa_, NULL);
    privKeyDer.resize(keyLen);
    unsigned char * buf = &privKeyDer[0];
    i2d_RSAPrivateKey(pOsslRsa_, &buf);

    NTRACE(TRACE_CRYPTO, "RsaContext::getPrivatePkcs1: key = %s",
           privKeyDer.toHex().c_str());

    return true;
}

//////////
//
// setPublicSpki
//
//////////
bool RsaContext::setPublicSpki(const DataBuffer &pubKeySpkiDer)
{
    NTRACE(TRACE_CRYPTO, "RsaContext::setPublicSpki: key = %s",
           pubKeySpkiDer.toHex().c_str());

    const unsigned char * buf = &pubKeySpkiDer[0];
    RSA *rsa = d2i_RSA_PUBKEY(NULL, &buf, pubKeySpkiDer.size());
    if (rsa == NULL) {
        Log::error(TRACE_LOG, "[%s:%d]: if (rsa == NULL) {", __func__, __LINE__);
        return false;
    }
    if (pOsslRsa_)
        RSA_free(pOsslRsa_);
    pOsslRsa_ = rsa;
    hasPublicKey_ = true;
    return true;
}

//////////
//
// getPublicSpki
//
//////////
bool RsaContext::getPublicSpki(/*out*/DataBuffer &pubKeySpkiDer) const
{
    if (!hasPublicKey_)
        return false;
    int keyLen = i2d_RSA_PUBKEY(pOsslRsa_, NULL);
    pubKeySpkiDer.resize(keyLen);
    unsigned char * buf = &pubKeySpkiDer[0];
    i2d_RSA_PUBKEY(pOsslRsa_, &buf);
    return true;
}

//////////
//
// setPrivatePkcs8
//
//////////
bool RsaContext::setPrivatePkcs8(const DataBuffer &pkcs8)
{
    // OpenSSL does not make it easy to import a private key in PKCS#8 format.
    // Must go through some monkey-motions.

    // make a mem BIO pointing to the incoming PKCS#8 data
    char* const data = reinterpret_cast<char*>(const_cast<uint8_t*>(&pkcs8[0]));
    ScopedOpenSSL<BIO, BIO_free_all> bio(BIO_new_mem_buf(data, pkcs8.size()));
    if (!bio.get())
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::setPrivatePkcs8: BIO_new_mem_buf() failed");
        return false;
    }

    // get a PKCS8_PRIV_KEY_INFO struct from the BIO
    ScopedOpenSSL<PKCS8_PRIV_KEY_INFO, PKCS8_PRIV_KEY_INFO_free> p8inf(
        d2i_PKCS8_PRIV_KEY_INFO_bio(bio.get(), NULL));
    if (!p8inf.get())
    {
        Log::warn(TRACE_CRYPTO,
            "RsaContext::setPrivatePkcs8: d2i_PKCS8_PRIV_KEY_INFO_bio() failed");
        return false;
    }

    // create a EVP_PKEY from the PKCS8_PRIV_KEY_INFO
    ScopedOpenSSL<EVP_PKEY, EVP_PKEY_free> pkey(EVP_PKCS82PKEY(p8inf.get()));
    if (!pkey.get())
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::setPrivatePkcs8: EVP_PKCS82PKEY() failed");
        return false;
    }

    // get the RSA struct from the EVP_PKEY
    RSA * const rsa = EVP_PKEY_get1_RSA(pkey.get());
    if (!rsa)
    {
        Log::warn(TRACE_CRYPTO,
                  "RsaContext::setPrivatePkcs8: EVP_PKEY_get1_RSA() failed");
        return false;
    }

    // save the RSA struct to this
    pOsslRsa_ = rsa;
    hasPrivateKey_ = true;
    return true;
}

//////////
//
// getPrivatePkcs8
//
//////////
bool RsaContext::getPrivatePkcs8(/*out*/DataBuffer &pkcs8) const
{
    if (!hasPrivateKey_)
        return false;

    ScopedOpenSSL<EVP_PKEY, EVP_PKEY_free> pkey(EVP_PKEY_new());
    if (pkey.get() == NULL)
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::getPrivatePkcs8: EVP_PKEY_new() failed");
        return false;
    }
    int ret = EVP_PKEY_set1_RSA(pkey.get(), pOsslRsa_);
    if (!ret)
    {
        Log::warn(TRACE_CRYPTO,
                  "RsaContext::getPrivatePkcs8: EVP_PKEY_set1_RSA() failed");
        return false;
    }
    ScopedOpenSSL<PKCS8_PRIV_KEY_INFO, PKCS8_PRIV_KEY_INFO_free> p8inf(
        EVP_PKEY2PKCS8(pkey.get()));
    if (p8inf.get() == NULL)
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::getPrivatePkcs8: EVP_PKEY2PKCS8() failed");
        return false;
    }
    int outLen = i2d_PKCS8_PRIV_KEY_INFO(p8inf.get(), NULL);
    if (outLen <= 0)
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::getPrivatePkcs8: i2d_PKCS8_PRIV_KEY_INFO() "
                  "returned bad length");
        return false;
    }
    pkcs8.resize(outLen);
    unsigned char * buf = &pkcs8[0];
    ret = i2d_PKCS8_PRIV_KEY_INFO(p8inf.get(), &buf);
    if (!ret)
    {
        pkcs8.clear();
        Log::warn(TRACE_CRYPTO,
                  "RsaContext::i2d_PKCS8_PRIV_KEY_INFO: EVP_PKEY_set1_RSA() failed");
        return false;
    }
    return true;
}

//////////
//
// publicEncrypt
//
//////////
bool RsaContext::publicEncrypt(const DataBuffer &in,
                               /*out*/DataBuffer &out, Padding padding)
{
    if (!hasPublicKey_)
        return false;
    int osslPadding = RSA_NO_PADDING;
    switch (padding)
    {
        case NONE:          osslPadding = RSA_NO_PADDING;           break;
        case PKCS1:         osslPadding = RSA_PKCS1_PADDING;        break;
        case PKCS1_OAEP:    osslPadding = RSA_PKCS1_OAEP_PADDING;   break;
        default:            assert(false);                          break;
    }
    const size_t rsaSize = RSA_size(pOsslRsa_);
    if (rsaSize <= 0)
        return false;
    if ((size_t)in.size() > (rsaSize - 11))
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::publicEncrypt: input message too long");
        return false;
    }
    out.resize(rsaSize);
    int res = RSA_public_encrypt(in.size(), &in[0], &out[0],
            pOsslRsa_, osslPadding);
    if (res == -1)
    {
        out.clear();
        Log::warn(TRACE_CRYPTO, "RsaContext::publicEncrypt: RSA_public_encrypt() failed");
        return false;
    }
    out.resize(res);
    return true;
}

//////////
//
// privateDecrypt
//
//////////
bool RsaContext::privateDecrypt(const DataBuffer &in,
                                /*out*/DataBuffer &out,
                                Padding padding)
{
    if (!hasPrivateKey_)
        return false;
    int osslPadding = RSA_NO_PADDING;
    switch (padding)
    {
        case NONE:          osslPadding = RSA_NO_PADDING;           break;
        case PKCS1:         osslPadding = RSA_PKCS1_PADDING;        break;
        case PKCS1_OAEP:    osslPadding = RSA_PKCS1_OAEP_PADDING;   break;
        default:            assert(false);                          break;
    }
    const int rsaSize = RSA_size(pOsslRsa_);
    if (rsaSize <= 0)
        return false;
    out.resize(rsaSize);
    int res = RSA_blinding_on(pOsslRsa_, NULL); // blinding for private key ops only
    if (res != 1)
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::privateDecrypt: RSA_blinding_on() failed");
        return false;
    }
    res = RSA_private_decrypt(in.size(), &in[0], &out[0], pOsslRsa_, osslPadding);
    RSA_blinding_off(pOsslRsa_);
    if (res == -1)
    {
        out.clear();
        Log::warn(TRACE_CRYPTO,
                  "RsaContext::privateDecrypt: RSA_private_decrypt() failed");
        return false;
    }
    out.resize(res);
    return true;
}

//////////
//
// privateSign
//
//////////
bool RsaContext::privateSign(const DataBuffer &in, ShaAlgo shaAlgo,
                             /*out*/DataBuffer &out)
{
    if (!hasPrivateKey_)
        return false;

    // first need to calculate the hash, using the specified ShaAlgo
    const DataBuffer hash = computeDigest(in, shaAlgo);

    // now sign the computed hash
    const int rsaSize = RSA_size(pOsslRsa_);
    if (rsaSize <= 0)
        return false;
    out.resize(rsaSize);
    // use blinding for all private key operations
    if (RSA_blinding_on(pOsslRsa_, NULL) != 1)
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::privateSign: RSA_blinding_on() failed");
        return false;
    }
    unsigned int outLen;
    int res = RSA_sign(nidSha(shaAlgo), &hash[0], hash.size(), &out[0],
                       &outLen, pOsslRsa_);
    RSA_blinding_off(pOsslRsa_);
    if (res != 1)
    {
        out.clear();
        Log::warn(TRACE_CRYPTO, "RsaContext::privateSign: RSA_sign() failed");
        return false;
    }

    // size and return the signature
    out.resize(outLen);

    return true;
}

//////////
//
// publicVerify
//
//////////
bool RsaContext::publicVerify(const DataBuffer &in, ShaAlgo shaAlgo,
                              const DataBuffer &sig)
{
    if (!hasPublicKey_)
        return false;

    // first need to calculate the hash on the input data, using the specified ShaAlgo
    const DataBuffer hash = computeDigest(in, shaAlgo);

    // now verify the computed hash
    if (RSA_blinding_on(pOsslRsa_, NULL) != 1)
    {
        Log::warn(TRACE_CRYPTO, "RsaContext::privateSign: RSA_blinding_on() failed");
        return false;
    }
    // OSX RAS_verify needs the sigbuf to not be const but promises not to modify it
    const int res = RSA_verify(nidSha(shaAlgo), &hash[0], hash.size(),
                         (unsigned char *)&sig[0], sig.size(), pOsslRsa_);
    RSA_blinding_off(pOsslRsa_);
    if (res != 1)
    {
        NTRACE(TRACE_CRYPTO, "RsaContext::publicVerify: RSA_verify() failed");
        return false;
    }
    return true;
}

//////////
//
// computeDigest
//
//////////
DataBuffer RsaContext::computeDigest(const DataBuffer &in, ShaAlgo shaAlgo)
{
    shared_ptr<const DigestAlgo> digestAlgo;
    switch (shaAlgo)
    {
    case SHA1:      digestAlgo = DigestAlgo::SHA1();    break;
    case SHA224:    digestAlgo = DigestAlgo::SHA224();  break;
    case SHA256:    digestAlgo = DigestAlgo::SHA256();  break;
    case SHA384:    digestAlgo = DigestAlgo::SHA384();  break;
    case SHA512:    digestAlgo = DigestAlgo::SHA512();  break;
    default:        assert(false);                      break;
    }
    Digester digester(digestAlgo);
    digester.init();
    digester.update(in);
    return digester.final();
}
