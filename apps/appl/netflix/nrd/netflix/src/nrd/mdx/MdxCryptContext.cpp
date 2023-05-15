/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxCryptContext.h"

#include <nrdbase/Base64.h>
#include <nrdbase/HMAC.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <nrdbase/Log.h>
#include "MdxLog.h"

using namespace netflix::mdx;
using namespace netflix;

static const unsigned int MDX_ENC_KEY_SIZE = AES_BLOCK_SIZE; // MDX uses 16-byte encryption key

// maximum DER data length supported for MDX data
static const unsigned int MDX_MAX_DER_SIZE = 0x7fff;

// ciphertext octets get DER-encoded, so they are limited by MDX_MAX_DER_SIZE
static const unsigned int MDX_MAX_CTEXT_SIZE = MDX_MAX_DER_SIZE;

// Due to CBC padding, plaintext is smaller than ciphertext by at least 1 byte
static const unsigned int MDX_MAX_PTEXT_SIZE = MDX_MAX_CTEXT_SIZE - 1;

// envelope mandatory header: 1 version byte + 1 IV length byte + AES_BLOCK_SIZE of IV bytes
static const unsigned int MDX_ENV_HEADER_SIZE = 1 + 1 + AES_BLOCK_SIZE;

// shortest envelope adds ciphertext of AES_BLOCK_SIZE size (with 1 length byte prefix)
static const unsigned int MDX_MIN_ENV_SIZE = MDX_ENV_HEADER_SIZE + 1 + AES_BLOCK_SIZE;

// longest envelope adds ciphertext of MDX_MAX_CTEXT_SIZE size (with 3 length bytes prefix)
static const unsigned int MDX_MAX_ENV_SIZE = MDX_ENV_HEADER_SIZE + 3 + MDX_MAX_CTEXT_SIZE;

// B64 encoding stretches data by 4/3 - perhaps, +1 sometimes, but does not matter
static const unsigned int MDX_MAX_ENV_B64_SIZE = MDX_MAX_ENV_SIZE * 4 / 3;

static const unsigned char ENV_VERSION = 0x02; // Cryptext envelope version


//////////
//
// CryptContext
//
//////////
CryptContext::CryptContext(const std::vector<unsigned char> &ss64)
{
    // Base64 decode the shared secret
    mSharedSecret = Base64::decode(ss64);
    assert(mSharedSecret.size() == 48);
}

/*
 * calculate the number of bytes to DER-encode the length
 */
static int der_len_len(const unsigned int inlen) {
    int len; // DER data type byte
    if (inlen <= 0x7f) {
        len = 1;
    } else if (inlen <= 0x7fff) {
        len = 2;
    } else { // bigger size not supported
        len = -1;
    }
    return len;
}

/*
 * read DER-encoded length and return the pointer past the length bytes
 */
static unsigned char * der_read_len(unsigned char * inbuf, int * const outlen) {
    if (inbuf[0] <= 0x7f) {
      *outlen = inbuf[0];
      inbuf += 1;
    } else {
      *outlen = ((inbuf[0] & 0x7F) << 8) | (inbuf[1] & 0xFF);
      inbuf += 2;
    }
    return inbuf;
}

/*
 * write DER-encoded length and return the pointer past the length bytes
 */
static unsigned char * der_write_len(unsigned char * outbuf, const unsigned int inlen) {
    if (inlen <= 0x7f) {
        *outbuf = (unsigned char)inlen; ++outbuf;
    } else if (inlen <= 0x7fff) {
      *outbuf = (0x80 | (inlen >> 8)); ++outbuf;
      *outbuf = (unsigned char)(inlen & 0xff); ++outbuf;
    } else { // bigger size not supported
        outbuf = NULL;
    }

    return outbuf;
}

/*
 * write DER-encoded octets - length bytes at the beginning
 */
static unsigned char * der_write_octets(unsigned char * outbuf, const unsigned char * const inbuf, const unsigned int inlen) {
    outbuf = der_write_len(outbuf, inlen);
    if (outbuf == NULL) return NULL;
    memcpy(outbuf, inbuf, inlen);
    return outbuf + inlen;
}

#define EMPTY_VECTOR std::vector<unsigned char>()

/*
 * AES-CBC Encrypt
 */
static const std::vector<unsigned char> _encrypt(const std::vector<unsigned char> &plaintext, const unsigned char key[MDX_ENC_KEY_SIZE], const unsigned char iv[AES_BLOCK_SIZE])
{
    const int cipherlen = (plaintext.size()/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE; // may be up to AES_BLOCK_SIZE padding bytes
    std::vector<unsigned char> ciphertext(cipherlen);

    EVP_CIPHER_CTX cipherCtx;
    EVP_CIPHER_CTX_init(&cipherCtx);

    int result = EVP_EncryptInit_ex(&cipherCtx, EVP_aes_128_cbc(), NULL, key, iv);
    if (result != 1) {
        Log::error(TRACE_MDX, "Encrypt Init failure %d\n", result);
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }

    int outlen;
    unsigned char * outbuf = &ciphertext[0];
    result = EVP_EncryptUpdate(&cipherCtx, outbuf, &outlen, &plaintext[0], plaintext.size());
    if (result != 1) {
        Log::error(TRACE_MDX, "Encrypt Update failure %d\n", result);
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }

    int tmplen;
    result = EVP_EncryptFinal_ex(&cipherCtx, outbuf+outlen, &tmplen);
    if (result != 1) {
        Log::error(TRACE_MDX, "Encrypt Final failure %d\n", result);
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }
    outlen += tmplen;

    if (outlen != cipherlen) {
        Log::error(TRACE_MDX, "Encrypt Final failure: Unexpected Ciphertext Length %d, Expected %d\n", outlen, cipherlen);
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }

    EVP_CIPHER_CTX_cleanup(&cipherCtx);

    return ciphertext;
}

/*
 * AES-CBC Decrypt
 */
static const std::vector<unsigned char> _decrypt(const std::vector<unsigned char> &ciphertext, const unsigned char key[MDX_ENC_KEY_SIZE], const unsigned char iv[AES_BLOCK_SIZE])
{
    std::vector<unsigned char> plaintext(ciphertext.size());

    EVP_CIPHER_CTX cipherCtx;
    EVP_CIPHER_CTX_init(&cipherCtx);

    int result = EVP_DecryptInit_ex(&cipherCtx, EVP_aes_128_cbc(), NULL, key, iv);
    if (result != 1) {
        Log::error(TRACE_MDX, "Decrypt Init failure %d\n", result);
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }

    int outlen;
    unsigned char * outbuf = &plaintext[0];
    result = EVP_DecryptUpdate(&cipherCtx, outbuf, &outlen, &ciphertext[0], ciphertext.size());
    if (result != 1) {
        Log::error(TRACE_MDX, "Decrypt Update failure %d\n", result);
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }

    int tmplen;
    result = EVP_DecryptFinal_ex(&cipherCtx, outbuf+outlen, &tmplen);
    if (result != 1) {
        Log::error(TRACE_MDX, "Decrypt Final failure %d\n", result);
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }
    outlen += tmplen;

    if (outlen >= (int)ciphertext.size()) {
        Log::error(TRACE_MDX, "Decrypt Final failure: Plaintext Length %d Exceeds or Equals Ciphertext Length %zu\n", outlen, ciphertext.size());
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    } else if (outlen + AES_BLOCK_SIZE >= (int)ciphertext.size()) {
        plaintext.resize(outlen);
    } else {
        Log::error(TRACE_MDX, "Decrypt Final failure: Plaintext Length %d Too Small Compared to Ciphertext Length %zu\n", outlen, ciphertext.size());
        EVP_CIPHER_CTX_cleanup(&cipherCtx);
        return EMPTY_VECTOR;
    }

    EVP_CIPHER_CTX_cleanup(&cipherCtx);

    return plaintext;
}

/*
 * create encrypt envelope
 */
std::vector<unsigned char> CryptContext::encrypt(const std::vector<unsigned char> &plaintext)
{
    // validate shared secret size
    if (mSharedSecret.size() != 48) {
        Log::error(TRACE_MDX, "Encrypt Failure: wrong shared secret size %zu\n", mSharedSecret.size());
        return EMPTY_VECTOR;
    }

    // validate plaintext size
    if (plaintext.size() > MDX_MAX_PTEXT_SIZE) {
        Log::error(TRACE_MDX, "Encrypt Failure: Plaintext Size %zu Exceeds Limit %d\n", plaintext.size(), MDX_MAX_PTEXT_SIZE);
        return EMPTY_VECTOR;
    }

    // initialize IV
    unsigned char iv[AES_BLOCK_SIZE];
    RAND_bytes(iv, sizeof(iv)); // initialize IV

    // compute ciphertext
    const std::vector<unsigned char> ciphertext = _encrypt(plaintext, &mSharedSecret[32], iv);
    if (ciphertext == EMPTY_VECTOR) {
        Log::error(TRACE_MDX, "Encrypt Failure: Cannot Encrypt Plaintext of Size %zu\n", plaintext.size());
        return EMPTY_VECTOR;
    }

    // validate ciphertext size - seems redundant to validating plaintext size, but is not really, if _encrypt() misbehaves
    if (ciphertext.size() > MDX_MAX_CTEXT_SIZE) {
        Log::error(TRACE_MDX, "Encrypt Failure: Ciphertext Size %zu Exceeds Limit %d\n", ciphertext.size(), MDX_MAX_CTEXT_SIZE);
        return EMPTY_VECTOR;
    }

    // write envelope
    const int envelopeLength = 1 + der_len_len(sizeof(iv)) + sizeof(iv) + der_len_len(ciphertext.size()) + ciphertext.size();
    std::vector<unsigned char> envelope(envelopeLength);
    unsigned char *p = &envelope[0];

    // append Version
    *p = ENV_VERSION;
    p++;

    // append IV Octets
    p = der_write_octets(p, iv, sizeof(iv)); // write IV octets and skip them
    if (p == NULL) {
        Log::error(TRACE_MDX, "Encrypt Failure: Cannot Append IV Octets\n");
        return EMPTY_VECTOR;
    }

    // append Ciphertext Octets
    p = der_write_octets(p, &ciphertext[0], ciphertext.size()); // write ciphertext octets and skip them
    if (p == NULL) {
        Log::error(TRACE_MDX, "Encrypt Failure: Cannot Append Ciphertext Octets\n");
        return EMPTY_VECTOR;
    }

    // b64 encode envelope
    return Base64::encode(envelope);
}

/*
 * process decrypt envelope
 */
std::vector<unsigned char> CryptContext::decrypt(const std::vector<unsigned char> &envelope_b64)
{
    // validate shared secret size
    if (mSharedSecret.size() != 48) {
        Log::error(TRACE_MDX, "Decrypt Failure: wrong shared secret size %zu\n", mSharedSecret.size());
        return EMPTY_VECTOR;
    }

    // Check b64 envelope size
    if (envelope_b64.size() > MDX_MAX_ENV_B64_SIZE) {
        Log::error(TRACE_MDX, "Decrypt Failure: Envelope Size %zu Exceeds Limit %d\n", envelope_b64.size(), MDX_MAX_ENV_B64_SIZE);
        return EMPTY_VECTOR;
    }

    // b64 decode the data
    std::vector<unsigned char> envelope = Base64::decode(envelope_b64);
    if (envelope.size() < MDX_MIN_ENV_SIZE) {
        Log::error(TRACE_MDX, "Decrypt Failure: Envelope Size Too Small: %zu, Minimum Size: %d\n", envelope.size(), MDX_MIN_ENV_SIZE);
        return EMPTY_VECTOR;
    }

    unsigned char * p = &envelope[0];

    // Verify Version
    if (*p != ENV_VERSION) {
        Log::error(TRACE_MDX, "Decrypt Failure: Unexpected Envelope Version %x\n", *p);
        return EMPTY_VECTOR;
    }
    p++; // skip version byte

    // Verify IV length - should be AES_BLOCK_SIZE, fitting into one byte
    if (*p != AES_BLOCK_SIZE) {
        Log::error(TRACE_MDX, "Unexpected IV Size %d\n", (int)*p);
        return EMPTY_VECTOR;
    }
    p++; // skip IV length byte

    // point at IV bytes
    unsigned char * const iv = p;
    p += AES_BLOCK_SIZE; // skip IV

    // parse and validate ciphertext
    int cipherlen;
    p = der_read_len(p, &cipherlen);
    if (p == NULL) {
        Log::error(TRACE_MDX, "Envelope Ciphertext Parsing Failure\n");
        return EMPTY_VECTOR;
    }

    // ciphertext must be in AES_BLOCK_SIZE blocks
    if (cipherlen % AES_BLOCK_SIZE) {
        Log::error(TRACE_MDX, "Invalid Ciphertext Length %d, Not Multiple of %d\n", cipherlen, AES_BLOCK_SIZE);
        return EMPTY_VECTOR;
    }

    // ciphertext is the last element in the envelope, so both must end at the same offset
    // EW 03/23/2015: The following bounds checking code causes the MS compiler to throw and error. 
    // Check pointers instead.
    //if ((p + cipherlen) != &envelope[envelope.size()]) {
    if ((p + cipherlen) != (&envelope[0] + envelope.size())) {
        Log::error(TRACE_MDX, "Unexpected Ciphertext Length %d, Not Aligned With the End Of Envelope\n", cipherlen);
        return EMPTY_VECTOR;
    }

    // wrap cipertext into a vector
    const std::vector<unsigned char> ciphertext(p, p + cipherlen);

    // decrypt ciphertext
    return _decrypt(ciphertext, &mSharedSecret[32], iv);
}

//////////
//
// calculateHmac
//
//////////
bool CryptContext::calculateHmac(/*out*/std::vector<unsigned char> &out,
                                 const std::vector<unsigned char> &data)
{
    if (mSharedSecret.size() != 48) {
        Log::error(TRACE_MDX, "HMAC failure shared secret wrong size %zu\n", mSharedSecret.size());
        return false;
    }
    HMAC hmac(&mSharedSecret[0], 32);
    hmac.update(data);
    out = Base64::encode(hmac.final<std::vector<unsigned char> >());
    return true;
}
