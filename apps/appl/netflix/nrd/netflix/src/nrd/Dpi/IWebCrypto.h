/*
 *  Copyright 1997-2015 Netflix, Inc.
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

#ifndef __IWEBCRYPTO_H__
#define __IWEBCRYPTO_H__

#include <memory>
#include <nrdbase/StdInt.h>
#include <string>
#include <vector>
#include <set>

#include <nrdbase/NFErr.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/tr1.h>

namespace netflix {

class Variant;

/** @file IWebCrypto.h The header file for Web Crypto DPI.
 *
 * Low-level crypto functions for WebCrypto implementation.
 * This class provides the operations required to implement a subset of
 * the Web Crypto API.
 *
 * The actual implementation of this interface must be located within
 * a trusted execution environment (TEE).
 * Operations should be executed in their entirety within the TEE to ensure
 * data cannot be tampered with and security cannot be circumvented. This
 * includes all parsing/encoding, integrity protection/verification, and key
 * usage enforcement in addition to the crypto operations and protection of
 * keying material.
 *
  * Upcoming changes
  * -----------------
  * In an upcoming release, this API will change in the following ways:
  *
  *  1) Algorithm parameters will be formalized into a stricter structure
  *     and will no longer be variants.
  *
  *  2) The API will be kept in sync with changes to the Web Crypto API.
  *
  */

class IWebCrypto : Noncopyable
{
public:
    virtual ~IWebCrypto() {}

    //---------------- Key Store ---------------------------------------------//

    enum KeyFormat
    {
        RAW,    //< An unformatted sequence of bytes. Intended for secret keys.
        PKCS8,  //< The DER encoding of the PrivateKeyInfo structure from RFC 5208.
        SPKI,   //< The DER encoding of the SubjectPublicKeyInfo structure from RFC 5280.
        JWK,    //< The key is represented as JSON according to the JSON Web Key format.
        INVALID_KEYFORMAT
    };
    enum Algorithm
    {
        HMAC,
        AES_CBC,
        AES_GCM,
        AES_CTR,
        RSAES_PKCS1_V1_5,
        RSASSA_PKCS1_V1_5,
        RSA_OAEP,
        SHA1,
        SHA224,
        SHA256,
        SHA384,
        SHA512,
        AES_KW,
        DH,
        SYSTEM,
        NFLX_DH,
        ECC,
        INVALID_ALGORITHM
    };
    enum KeyType {SECRET, PUBLIC, PRIVATE};
    enum KeyUsage {ENCRYPT = 0x01, DECRYPT = 0x02, SIGN = 0x04, VERIFY = 0x08, DERIVE = 0x10, WRAP = 0x20, UNWRAP = 0x40};

    /**
     *
     * This method imports a key into the local key store.
     * If the format is JWK, the algorithm, usage and extractable flags
     * may be overriden by the JWK contents.
     *
     * @param[in] format The format of the keyData containing the key
     * @param[in] keyData The data containing the key
     * @param[in] algorithm The full details about the key generation algorithm.
     * @param[in] extractable Whether or not the raw keying material may be
     *     exported by the application.
     * @param[in] keyUsage A vector of KeyUsage, indicating what operations may
     *     be used with this key.
     * @param[out] keyHandle The handle of the imported key in the key store.
     * @param[out] keyType The type of the key, deduced from format and keyData
     *
     * Return values
     * -------------
     * In general, use the following error codes:
     *
     * - `NFErr_OK`           - for success
     * - `NFErr_MalformedData`  - if input data is malformed
     * - `NFErr_BadParameter`   - if input parameters are invalid
     * - `NFErr_NotAllowed`     - if the operation is not allowed, for example attempting
     *                            to extract a non-extractable key
     * - `NFErr_Internal`       - when there is an internal error
     * - `NFErr_NotFound `      - when a key is not found by name or handle
     * - `NFErr_NotImplemented` - if a feature is not implemented
     *
     */

    virtual NFErr importKey(KeyFormat format, const DataBuffer& keyData,
                            const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType) = 0;

    /**
     * This method exports a key from the local key store. Only keys that are
     * marked extractable may be exported.
     * @param[in] keyHandle The handle of the key to export.
     * @param[in] format The desired format of the exported key data.
     * @param[out] keyData The data containing the key in the desired format
     */
    virtual NFErr exportKey(uint32_t keyHandle, KeyFormat format,
                            /*out*/DataBuffer &keyData) = 0;

    /**
     * This method reports information about a key in the local key store.
     * @param[in] keyHandle The handle of the key
     * @param[out] type The key type
     * @param[out] extractable Whether the key is marked as extractable
     * @param[out] algorithm The full details about the key algorithm
     * @param[out] usage The intended uses of the key, may be empty
     */
    virtual NFErr getKeyInfo(uint32_t keyHandle, /*out*/KeyType &type,
                             /*out*/bool &extractable, /*out*/Variant &algorithm,
                             /*out*/unsigned int &usage) const = 0;

    //---------------- Digest ------------------------------------------------//

    /** Compute the message digest of the input data.
     * @param[in] algorithm The SHA algorithm to use
     * @param[in] data The data to hash
     * @param[out] digest The result of the SHA computation
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr digest(const Variant &algorithm, const DataBuffer& data,
                         /*out*/DataBuffer& digest) = 0;

    //---------------- Encrypt / Decrypt -------------------------------------//

    /**
     * This method encrypts / decrypts input data with AES CBC, using a key in
     * the key cache indicated by the input key handle.
     * @param[in] keyHandle The handle of the desired key in the key cache.
     * @param[in] ivIn The initialization vector
     * @param[in] dataIn The data to encrypt
     * @param[in] cipherOp Which operation to perform
     * @param[out] dataOut The encrypted data
     * @return NFErr_OK if no error, error otherwise
     */
    enum CipherOp {DOENCRYPT, DODECRYPT};
    virtual NFErr aesCbc(uint32_t keyHandle, const DataBuffer& ivIn,
                         const DataBuffer& dataIn, CipherOp cipherOp,
                         /*out*/DataBuffer& dataOut) = 0;

    /**
     * This method encrypts input data with AES-GCM authenticated encryption,
     * using the key in the key cache indicated by the input key handle. The
     * input is cleartext to encrypt and additional data used when computing
     * the authentication tag. The result is the tag and the ciphertext.
     * @param[in] keyHandle The handle of the key in the key cache.
     * @param[in] ivIn The initialization vector
     * @param[in] dataIn The data to encrypt
     * @param[in] aadIn Additional authenticated data
     * @param[out] tagOut The computed authentication tag
     * @param[out] dataOut The encrypted data
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr aesGcmEncrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                const DataBuffer& dataIn, const DataBuffer& aadIn,
                                /*out*/DataBuffer& tagOut,
                                /*out*/DataBuffer& dataOut) = 0;

    /**
     * This method decrypts input data with AES-GCM authenticated decryption,
     * using a key in the key cache indicated by the input key handle. The input
     * ciphertext is decrypted and an authentication tag is computed with it
     * plus the additional authentication data. The operation is a success only
     * if the computed tag matches the input tag, and only then is the cleartext
     * returned.
     * @param[in] keyHandle The handle of the key in the key cache.
     * @param[in] ivIn The initialization vector
     * @param[in] dataIn The data to decrypt
     * @param[in] aadIn Additional authenticated data
     * @param[in] tagIn Authentication tag
     * @param[out] dataOut The decrypted data
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr aesGcmDecrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                const DataBuffer& dataIn, const DataBuffer& aadIn,
                                const DataBuffer& tagIn,
                                /*out*/DataBuffer& dataOut) = 0;

    /**
     * This method encrypts input data with RSAES-PKCS1-v1_5, using a key in the
     * key cache indicated by the input key handle. The key must be an RSA key.
     * @param[in] keyHandle The handle of the desired RSA key in the key cache.
     * @param[in] dataIn The data to encrypt
     * @param[in] cipherOp Which operation to perform
     * @param[out] dataOut The encrypted data
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr rsaCrypt(uint32_t keyHandle, const DataBuffer& dataIn,
                           CipherOp cipherOp, /*out*/DataBuffer& dataOut) = 0;

    //---------------- HMAC --------------------------------------------------//

    /**
     * This method computes the HMAC of the input data, using the specified
     * SHA inner hash and using the key indicated by the provided key handle.
     * @param[in] keyHandle The handle of the key to use when computing the HMAC
     * @param[in] shaAlgo The inner hash algorithm to use
     * @param[in] data The data to HMAC
     * @param[out] hmac The result of the HMAC operation
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr hmac(uint32_t keyHandle, const Variant& algorithm,
                           const DataBuffer& data,
                           /*out*/DataBuffer& hmac) = 0;


    /**
     * This method verifies the HMAC of the input data, using the specified
     * SHA inner hash and using the key indicated by the provided key handle.
     * @param[in] keyHandle The handle of the key to use when computing the HMAC
     * @param[in] shaAlgo The inner hash algorithm to use
     * @param[in] data The data to HMAC
     * @param[in] signature The computed signature
     * @param[out] isVerified True if the computed signature of the data matched
     * the provided signature, otherwise false
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr hmacVerify(uint32_t keyHandle, const Variant& algorithm,
                           const DataBuffer& data,
                           const DataBuffer& signature,
                           /*out*/bool& isVerified) = 0;


    //---------------- RSA ---------------------------------------------------//

    /**
     * This method computes an RSA public/private key pair
     * @param[in] algorithm The full details about the key gen algorithm, including
     *     the public exponent and modulus length.
     * @param[in] extractable Whether or not the raw key material may be exported
     * @param[in] keyUsage The allowed usages of the keys
     * @param[out] pubKeyHandle The handle of the generated public key in the key map
     * @param[out] privKeyHandle The handle of the generated private key in the key map
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr rsaKeyGen(const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& pubKeyHandle,
                            /*out*/uint32_t& privKeyHandle) = 0;

    /**
     * This method computes the RSASSA-PKCS1-v1_5 signature of a block of data,
     * using the specified key and inner hash.
     * @param[in] keyHandle The handle of the key to use
     * @param[in] algorithm The inner message digest algorithm to use
     * @param[in] data The data over which to compute the signature
     * @param[out] sig The computed signature
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr rsaSign(uint32_t keyHandle, const Variant &algorithm,
                          const DataBuffer& data, /*out*/DataBuffer& sig) = 0;

    /**
     * This method computes the RSASSA-PKCS1-v1_5 signature of a block of data,
     * using the specified key and inner hash, and compares it to the provided
     * signature.
     * @param[in] keyHandle The handle of the key to use
     * @param[in] algorithm The inner message digest algorithm to use
     * @param[in] data The data over which to compute the signature
     * @param[in] sig The data signature
     * @param[out] isVerified True if the computed signature of the data matched
     * the provided signature, otherwise false
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr rsaVerify(uint32_t keyHandle, const Variant &algorithm,
                            const DataBuffer& data, const DataBuffer& sig,
                            /*out*/bool& isVerified) = 0;

    //---------------- Diffie-Hellman ----------------------------------------//

    /**
     * This method computes Diffie-Hellman public/private key pair
     * @param[in] algVar The full details about the key gen algorithm, including
     *     the prime and generator values
     * @param[in] extractable Whether or not the raw key material may be exported
     * @param[in] keyUsage The allowed usages of the keys
     * @param[out] pubKeyHandle The handle of the generated public key in the key map
     * @param[out] privKeyHandle The handle of the generated private key in the key map
     * @return NFErr_OK if no error
     */
    virtual NFErr dhKeyGen(const Variant& algVar, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& pubKeyHandle,
                           /*out*/uint32_t& privKeyHandle) = 0;

    /**
     * This method computes a shared private key using a baseKey produced by
     * dhKeyGen() plus the public key from the remote peer who has previously
     * obtained the public baseKey.
     * @param[in] baseKeyHandle The handle of the key that started the DH
     *   exchange, produced by a call to dhKeyGen
     * @param[in] peerPublicKeyData The raw public key received from the remote
     *   peer
     * @param[in] derivedAlgObj The full details about the algorithm to be
     *   associated with the derived key
     * @param[in] extractable Whether or not the raw key material of the derived
     *   key may be exported
     * @param[in] keyUsage The allowed usages of the derived key
     * @param[out] keyHandle The handle of the derived key in the key map
     * @return NFErr_OK, if no error
     */
    virtual NFErr dhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
                           const Variant& derivedAlgObj, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& keyHandle) = 0;

   /**
     * This method computes shared Netflix private keys using a baseKey produced
     * by dhKeyGen(), the public key from the remote peer who has previously
     * obtained the public baseKey, and a special derivation key Kd. This method
     * produces three keys in the key store using a Netflix-proprietary key
     * derivation algorithm: a session encryption key, a sesion HMAC key, and a
     * wrapping key. The algorithm, extractable, and usage fields of the
     * produced keys are pre-determined and set automatically.
     * @param[in] baseKeyHandle The handle of the key that started the DH
     *   exchange, produced by a call to dhKeyGen
     * @param[in] peerPublicKeyData The raw public key received from the remote
     *   peer
     * @param[in] deriveKeyHandle The caller supplied derivation key Kd
     * @param[out] encryptionKeyHandle The handle of the derived session
     *   encryption key in the key map
     * @param[out] hmacKeyHandle The handle of the derived session hmac key in
     *   the key map
     * @param[out] wrappingKeyHandle The handle of the derived wrapping key in
     *   the key map
     * @return NFErr_OK, if no error
     */
    virtual NFErr nflxDhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
            uint32_t derivationKeyHandle,
            /*out*/uint32_t& encryptionKeyHandle,
            /*out*/uint32_t& hmacKeyHandle,
            /*out*/uint32_t& wrappingKeyHandle) = 0;

    //---------------- Symmnetric Key Generation -----------------------------//
    /** Generate a symmetric key
     * This method generates a single random key and places it in the key store.
     * @param[in] algVar The full details about the key generation algorithm.
     * @param[in] extractable Whether the key should be marked as extractable
     * @param[in] usage The intended uses of the key
     * @param[out] jeyHandle The handle of the resulting key in the key store.
     */
    virtual NFErr symKeyGen(const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t &keyHandle) = 0;

    //---------------- Key Wrapping-------------------------------------------//

    /**
     * This method unwraps a JWE-wrapped key as described in
     * draft-ietf-jose-json-web-encryption-08. This input JWE may be in either
     * the JSON or Compact Serialization format. The result is the unwrapped key
     * in the key store, which is referred to by the output keyHandle when later
     * performing other crypto operations. This method will report an error if
     * the unwrap fails for any reason, including failing the integrity check.
     * @param[in] jweData The base64-encoded wrapped key in JWE-JS or JWE-CS
     * format. Specifically, a wrapped key contains the following components
     *       Base64Url-encoded JWE Header,
     *       Base64Url-encoded JWE Encrypted Content Master Key (CMK),
     *       Base64Url-encoded JWE Initialization Vector,
     *       Base64Url-encoded JWE Ciphertext (the actual target key)
     *       Base64Url-encoded JWE Integrity Value,
     *     Encoding used in this string is URL-SAFE base64 UTF8 as mandated by
     *     the JWE spec. Note that this differs from the standard base64
     *     encoding used by the rest of this API.
     * @param[in] wrappingKeyHandle Handle of the key in the keystore with
     *     which to decrypt the CMK. This will be typically the RSA private key
     *     corresponding to the public key that encrypted the CMK.
     * @param[in] algVar In case the unwrapped JDK does not have the 'alg'
     *     field inside it, use this value; otherwise ignore
     * @param[in] extractable In case the unwrapped JWK does not have the
     *     'extractable' field inside it, use this value; otherwise the
     *     unwrapped key will have its extractable value set to a logical OR
     *     this and the extractable value inside the JWK.
     * @param[in] keyUsage In case the unwrapped JDK does not have the 'use'
     *     field inside it, use this value; otherwise ignore
     * @param[out] keyHandle The handle of the unwrapped key in the keystore.
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr unwrapJwe(const DataBuffer& jweData, uint32_t wrappingKeyHandle,
                            const Variant& algVar, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle) = 0;

    /**
     * This method wraps an existing key in the keystore according to the rules
     * in draft-ietf-jose-json-web-encryption-08, using an existing wrapping key
     * also in the keystore. The result is a base-64 encoded JWE-JS string.
     * @param[in] toBeWrappedKeyHandle The handle of the key to be wrapped
     * @param[in] wrappingKeyHandle The handle of the key to use in during the
     *     key wrap process.
     * @param[in] wrappingAlgoObj The encryption algorithm to be applied to the
     *    Content Master Key (CMK) during the wrapping process. This must be
     *    consistent with the algorithm associated with wrappingKeyHandle.
     *    Only RSA-OAEP and AES-KW algorithms are supported.
     * @param[in] wrappingEnc The algorithm to apply cleartext data. For AES-KW,
     *    only A128GCM is supported, while RSA-OAEP supports only A128GCM and
     *    A256GCM.
     * @param[out] wrappedKeyJcs The base64-encoded wrapped key in the specific
     *    format described in JWE-JS spec
     */
    enum JweEncMethod {A128GCM, A256GCM};
    virtual NFErr wrapJwe(uint32_t toBeWrappedKeyHandle, uint32_t wrappingKeyHandle,
                          const Variant& wrappingAlgoObj, JweEncMethod jweEncMethod,
                          /*out*/DataBuffer& wrappedKeyJcs) = 0;


    /**
     * This method unwraps a key wrapped with AES according to RFC3394. The key will
     * always be non-extractable.
     * @param[in] data The data to unwrap
     * @param[in] algVar The algorithm that will be assumed for the unwrapped key.
     * @param[in]  wrappingKeyHandle The handle of the key to use to unwrap.
     * @param[in] keyUsage Usage for the unwrapped key.
     * @param[out] keyHandle The handle of the unwrapped key.
     */
    virtual NFErr aesUnwrap(const DataBuffer & data, const Variant & algVar,
                            uint32_t wrappingKeyHandle,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle) = 0;

    /**
     * This method computes the ECC signature of a block of data,
     * using the specified key and inner hash.
     * @param[in] keyHandle The handle of the key to use
     * @param[in] algorithm The inner message digest algorithm to use
     * @param[in] data The data over which to compute the signature
     * @param[out] sig The computed signature
     * @return NFErr_OK if no error, error otherwise
     */
    virtual NFErr eccSign(uint32_t keyHandle, const Variant &algorithm,
                          const DataBuffer& data, /*out*/DataBuffer& sig) = 0;

    //---------------- NON Web Crypto APIs------------------------------------//

    /** Add an existing key as a named key
     * @param[in] name The name for the key.
     * @param[in] keyHandle The handle to associate with this name.
     */
    virtual NFErr addNamedKey(const std::string & name, uint32_t keyHandle) = 0;

    /** Get the handle to a key by name.
     * @param[in] name The name to look for.
     * @param[out] keyHandle A handle to the key.
     * @return NFErr_NotFound if a key by this name is not found.
     * @see http://www.w3.org/TR/webcrypto-key-discovery/
     */
    virtual NFErr getNamedKey(const std::string & name,
                               /*out*/ uint32_t & keyHandle) = 0;

    /** Flush the specified keys to secure store so that they can
     * be retrieved by their handle at a later time - beyond the end of the
     * current process. This should retain all the key attributes, handle,
     * algorithm data and key material.
     * @param[in] keyHandles A vector of key handles. If this vector is
     *    empty, all persisted keys should be deleted during this call.
    */
    virtual NFErr persistKeys(const std::vector<uint32_t> & keyHandles) = 0;

    /** Remove all keys that are not named or persisted from the key store
     * @param[out] keyHandles A set of key handles that were removed.
    */

    virtual NFErr clearKeys(/*out*/std::set<uint32_t> & keyHandles) = 0;

    /** Removes the key from the key store
     * @param[in] keyHandle The handle of the key to remove
    */

    virtual NFErr deleteKey(uint32_t keyHandle) = 0;

    static inline unsigned int convertKeyUsage(const Variant &vusage);
    static inline Variant convertKeyUsage(unsigned int usage);
};

//////////
//
// utilities
//
//////////
inline unsigned int IWebCrypto::convertKeyUsage(const Variant &vusage)
{
    unsigned int ret = 0;
    switch (vusage.type()) {
    case Variant::Type_Array: {
        const int size = vusage.size();
        for (int i=0; i<size; ++i)
            ret |= vusage[i].value<int>();
        break; }
    case Variant::Type_Integer:
    case Variant::Type_Double:
        ret |= vusage.value<int>();
        break;
    default:
        assert(0);
        break;
    }
    return ret;
}

inline Variant IWebCrypto::convertKeyUsage(unsigned int usage)
{
    Variant ret;
    ret.resizeArray(0);
    enum IWebCrypto::KeyUsage usages[] = {
        IWebCrypto::ENCRYPT,
        IWebCrypto::DECRYPT,
        IWebCrypto::SIGN,
        IWebCrypto::VERIFY,
        IWebCrypto::DERIVE,
        IWebCrypto::WRAP,
        IWebCrypto::UNWRAP
    };
    int count = 0;
    for (size_t i=0; i<sizeof(usages) / sizeof(usages[0]); ++i) {
        if (usage & usages[i]) {
            ret[count++] = usages[i];
        }
    }
    return ret;
}


} // namespace netflix::crypto

#endif // __IWEBCRYPTO_H__
