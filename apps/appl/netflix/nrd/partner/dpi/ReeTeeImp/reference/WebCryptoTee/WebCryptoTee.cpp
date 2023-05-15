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

#include "WebCryptoTee.h"
#include <nrd/AppLog.h>
#include <nrdbase/ScopedMutex.h>

#include <nrd/NrdApplication.h>

#include <nrdbase/Base64.h>
#include <nrdbase/DigestAlgo.h>
#include <nrdbase/HMAC.h>

using namespace netflix;
using namespace netflix::device;

extern DataBuffer ncf_kav;

Variant WebCryptoTee::Key::toJSON() const
{
    Variant result;
    result["key"] = key.toBase64String();
    result["type"] = type;
    result["extractable"] = extractable;
    result["algorithm"] = algVar;
    result["keyUsage"] = keyUsage;

    switch (variantToAlgorithm(algVar)) {
    case RSASSA_PKCS1_V1_5:
    case RSAES_PKCS1_V1_5:
    case RSA_OAEP:
        if (pRsaContext) {
            DataBuffer buf;
            if (type == PRIVATE) {
                if (!pRsaContext->getPrivatePkcs1(buf))
                    buf.clear();
            } else if (type == PUBLIC) {
                if (!pRsaContext->getPublicPkcs1(buf))
                    buf.clear();
            }
            if (!buf.isEmpty())
                result["key"] = buf.toBase64String();
        }
        break;
    default:
        break;
    }
    return result;
}

//////////
//
// WebCryptoTee
//
//////////

WebCryptoTee::WebCryptoTee(ISystem::AuthenticationType authType,
                           shared_ptr<TeeApiMgmt> teeMgmtPtr,
                           shared_ptr<TeeApiCrypto> teeCryptoPtr):
    mutex_(WEBCRYPTOTEE_MUTEX, "WebCryptoTeeMutex"),
    teeMgmtPtr_(teeMgmtPtr),
    teeCryptoPtr_(teeCryptoPtr)
{
    // restore persisted keys from TEE.
    teeRestorePersistedKeys();

    NRDP_UNUSED(authType);

    Variant kavAlg;
    kavAlg["name"] = "RSASSA-PKCS1-v1_5";
    const unsigned int kavUsage = VERIFY;
    KeyType keyType;
    uint32_t abkpReeHandle;

    NFErr err=reeImportKey(SPKI, ncf_kav, kavAlg, true, kavUsage, "ABKP", /*out*/abkpReeHandle, /*out*/keyType);
    if (!err.ok()) {
        Log::error(TRACE_CRYPTO) << "Failed to import appboot public key : " << err.toString();
    }
    uint32_t keyHandle = allocateKeyHandle();
    keyMap_[keyHandle] = KeySpec(KeySpec::SOURCE_REE, abkpReeHandle);
}

WebCryptoTee::~WebCryptoTee()
{
}

NFErr WebCryptoTee::reeImportKey(KeyFormat keyFormat, const DataBuffer& key,
                                 const Variant& algVar, bool extractable,
                                 unsigned int keyUsage, std::string name,
                                 /*out*/uint32_t& out_reeKeyHandle, /*out*/KeyType& out_keyType)
{
    ScopedMutex lock(mutex_);

    const Algorithm algorithm = variantToAlgorithm(algVar);
    if (algorithm!=RSASSA_PKCS1_V1_5){
        NTRACE(TRACE_CRYPTO, "WebCryptoTee::reeImportKey().  algorithm != RSASSA_PKCS1_V1_5");
        return NFErr_BadParameter;
    }

    if (key.empty())
        return NFErr_BadParameter;

    // shouldn't get JWK
    if (keyFormat == JWK){
        NTRACE(TRACE_CRYPTO, "WebCryptoTee::reeImportKey().  import JWK format is not supported in REE");
        return NFErr_BadParameter;
    }

    // Compare to code in WebCrypto::importKey -- jjb 11/10/14
    if (keyUsage!=0 && keyUsage!=SIGN && keyUsage!=VERIFY)
    {
        NTRACE(TRACE_CRYPTO,
               "WebCryptoTee::importKey: ERROR: inconsistent algorithm vs usage");
        return NFErr_BadParameter;
    }

    KeyType keyType;
    shared_ptr<RsaContext> pRsaContext(new RsaContext());
    switch (keyFormat){
        case SPKI:{
            // initialize the RSA context with the public SPKI-formatted key
            if (!pRsaContext->setPublicSpki(key)) {
                return NFErr_MalformedData; // cipher error
            }

            // SPKI is always public
            keyType = PUBLIC;

            // Since this is a public key, it should be forced to be
            // extractable.
            extractable = true;
            break;
        }
        case PKCS8:{
            // initialize the RSA context with the private PKCS#8-formatted key
            if (!pRsaContext->setPrivatePkcs8(key))
                return NFErr_MalformedData; // cipher error

            // PKCS8 is always private
            keyType = PRIVATE;
            break;
        }
        default:{
            NTRACE(TRACE_CRYPTO, "WebCryptoTee::reeImportKey: UNSUPPORTED_KEY_ENCODING");
            return NFErr_MalformedData; // UNSUPPORTED_KEY_ENCODING;
        }
    }
    // Finally, make a new Key object containing the extracted key and add it to
    // the key store, indexed by (output) keyHandle.
    // Note that extractable and keyUsage are copied through without processing.
    uint32_t reeKeyHandle = allocateReeKeyHandle();
    reeKeyMap_[reeKeyHandle] = Key(key, pRsaContext, keyType, extractable, algVar, keyUsage, name);

    NTRACEIF(!key.empty(), TRACE_CRYPTO, "reeImportKey: vuc=%s, handle=%d, type=%d", key.toHex().c_str(), reeKeyHandle, keyType);

    out_reeKeyHandle = reeKeyHandle;
    out_keyType = keyType;

    return NFErr_OK;
}

NFErr WebCryptoTee::importKey(KeyFormat format, const DataBuffer& keyData,
                              const Variant &algorithm, bool extractable,
                              unsigned int keyUsage,
                              /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType)
{
    ScopedMutex lock(mutex_);

    switch (format) {
        default:
            {
                /* all other key formats imported into REE */
                NTRACE(TRACE_CRYPTO, "REE: importing RAW Key");
                uint32_t reeHandle = kInvalidKeyHandle;
                NFErr ret = reeImportKey(format, keyData, algorithm, extractable, keyUsage, "", reeHandle, keyType);
                if (ret != NFErr_OK) {
                    Log::error(TRACE_CRYPTO) << "Failed to import key into REE, got error " << ret.toString();
                    return ret;
                }
                keyHandle = allocateKeyHandle();
                keyMap_[keyHandle] = KeySpec(KeySpec::SOURCE_REE, reeHandle);
                NTRACE(TRACE_CRYPTO, "REE import key was successful");
            }
            break;

        case RAW:
            {
                /* RAW keys get imported into TEE */
                NTRACE(TRACE_CRYPTO, "TEE: importing Key");
                uint32_t teeHandle = kInvalidKeyHandle;
                NFErr ret = teeCryptoPtr_->importKey (format, keyData, variantToAlgorithm(algorithm), extractable, keyUsage, teeHandle, keyType);
                if (ret != NFErr_OK) {
                    Log::error(TRACE_CRYPTO) << "Could not import key into TEE, got error " << ret.toString();
                    return ret;
                }
                keyHandle = allocateKeyHandle();
                keyMap_[keyHandle] = KeySpec(KeySpec::SOURCE_TEE, teeHandle);
                NTRACE(TRACE_CRYPTO, "TEE import key was successful");
            }
            break;
    }

    return NFErr_OK;
}

NFErr WebCryptoTee::exportKey(uint32_t keyHandle, KeyFormat format,
                              /*out*/DataBuffer &keyData)
{
    ScopedMutex lock(mutex_);

    const KeySpec * keySpec = getKeySpec(keyHandle);
    if (!keySpec)
        return NFErr_BadParameter;

    switch(keySpec->source){
    case KeySpec::SOURCE_INVALID:
            return NFErr_MalformedData;

    case KeySpec::SOURCE_REE:{
        if (!hasReeKey(keySpec->handle)) {
            Log::error(TRACE_CRYPTO) << "WebCryptoTee::exportKey(): key doesn't exist";
            return NFErr_MalformedData;
        }

        switch (format) {
            case SPKI:
                NTRACE(TRACE_CRYPTO, "REE: exporting public Key handle %d", keySpec->handle);
                if (!reeKeyMap_[keySpec->handle].pRsaContext->getPublicSpki(keyData)){
                    Log::error(TRACE_CRYPTO) << "WebCryptoTee::exportKey(): failed to get the public key";
                    return NFErr_Bad;
                }
                break;
            default:
                Log::error(TRACE_CRYPTO) << "WebCryptoTee::exportKey(): unsupported format";
                return NFErr_MalformedData;
        }

        return NFErr_OK;

    }
    case KeySpec::SOURCE_TEE:{
        NTRACE(TRACE_CRYPTO, "TEE: exporting Key");
        NFErr ret = teeCryptoPtr_->exportKey(keySpec->handle, format, keyData);
        if (ret != NFErr_OK){
            keyData.clear();
            Log::error(TRACE_CRYPTO) << "Could not export key from TEE, got error " << ret.toString();
            return ret;
        }
        NTRACE(TRACE_CRYPTO, "TEE export key was successful");
        break;
    }
    }


    return NFErr_OK;
}

NFErr WebCryptoTee::getKeyInfo(uint32_t keyHandle, /*out*/KeyType &type,
                               /*out*/bool &extractable, /*out*/Variant &algorithm,
                               /*out*/unsigned int &usage)const
{
    ScopedMutex lock(mutex_);

    NTRACE(TRACE_CRYPTO, "WebCryptoTee: getKeyInfo on handle %d", keyHandle);
    const KeySpec * keySpec = getKeySpec(keyHandle);
    if (!keySpec)
        return NFErr_BadParameter;

    switch(keySpec->source){
    case KeySpec::SOURCE_INVALID:
        return NFErr_MalformedData;

    case KeySpec::SOURCE_REE: {
        if (!hasReeKey(keySpec->handle))
            return NFErr_BadParameter;
        NTRACE(TRACE_CRYPTO, "WebCryptoTee::getKeyInfo(): getKeyInfo from REE on handle %d", keySpec->handle);
        // STL map operator[] is intolerant to const args...
        const Key key = const_cast<WebCryptoTee *>(this)->reeKeyMap_[keySpec->handle];
        type = key.type;
        extractable = key.extractable;
        algorithm = key.algVar;
        usage = key.keyUsage;
        break; }
    case KeySpec::SOURCE_TEE: {
        NTRACE(TRACE_CRYPTO, "TEE: getting Key info, handle %d", keyHandle);
        Algorithm algo = INVALID_ALGORITHM;
        NFErr ret = teeCryptoPtr_->getKeyInfo (keySpec->handle, type, extractable, algo, usage);
        if (ret != NFErr_OK){
            Log::error(TRACE_CRYPTO) << "Could not get key info from TEE, got error " << ret.toString();
            return ret;
        }
        algorithm.clear();
        algorithm["name"] = algorithmToString(algo);
        NTRACE(TRACE_CRYPTO, "TEE get key info was successful");
        break; }
    }

    return NFErr_OK;
}

NFErr WebCryptoTee::digest(const Variant &algorithm, const DataBuffer& data,
                           /*out*/DataBuffer& digest)
{
    NRDP_UNUSED(algorithm);
    NRDP_UNUSED(data);
    NRDP_UNUSED(digest);
    Log::error(TRACE_CRYPTO) << "digest() not implemented.";
    return NFErr_NotImplemented;
}


NFErr WebCryptoTee::aesCbc(uint32_t keyHandle, const DataBuffer& ivIn,
                           const DataBuffer& dataIn, CipherOp cipherOp,
                           /*out*/DataBuffer& dataOut)
{
    ScopedMutex lock(mutex_);

    const KeySpec * keySpec = getKeySpec(keyHandle);
    if (!keySpec)
        return NFErr_BadParameter;

    switch(keySpec->source)
    {
        case KeySpec::SOURCE_INVALID:
            return NFErr_MalformedData;

        case KeySpec::SOURCE_REE:
            Log::error(TRACE_CRYPTO) << "Asked to do aesCBC with REE key " << keySpec->handle;
            return NFErr_BadParameter;

        case KeySpec::SOURCE_TEE:
        {
            NFErr ret = teeCryptoPtr_->aescbc(keySpec->handle, cipherOp, ivIn, dataIn, dataOut);
            if (ret != NFErr_OK)
            {
                dataOut.clear();
                Log::error(TRACE_CRYPTO) << "TEE aes-cbc cipher op returned error " << ret.toString();
                return ret;
            }
            NTRACE(TRACE_CRYPTO, "TEE aes-cbc cipher op was successful");
            break;
        }
    }
    return NFErr_OK;
}


NFErr WebCryptoTee::aesGcmEncrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                  const DataBuffer& dataIn, const DataBuffer& aadIn,
                                  /*out*/DataBuffer& tagOut,
                                  /*out*/DataBuffer& dataOut)
{
    NRDP_UNUSED(keyHandle);
    NRDP_UNUSED(ivIn);
    NRDP_UNUSED(dataIn);
    NRDP_UNUSED(aadIn);
    NRDP_UNUSED(tagOut);
    NRDP_UNUSED(dataOut);
    Log::error(TRACE_CRYPTO) << "aesGcmEncrypt() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::aesGcmDecrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                  const DataBuffer& dataIn, const DataBuffer& aadIn,
                                  const DataBuffer& tagIn,
                                  /*out*/DataBuffer& dataOut)
{
    NRDP_UNUSED(keyHandle);
    NRDP_UNUSED(ivIn);
    NRDP_UNUSED(dataIn);
    NRDP_UNUSED(aadIn);
    NRDP_UNUSED(tagIn);
    NRDP_UNUSED(dataOut);
    Log::error(TRACE_CRYPTO) << "aesGcmDecrypt() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::rsaCrypt(uint32_t keyHandle, const DataBuffer& dataIn,
                             CipherOp cipherOp, /*out*/DataBuffer& dataOut)
{
    NRDP_UNUSED(keyHandle);
    NRDP_UNUSED(dataIn);
    NRDP_UNUSED(cipherOp);
    NRDP_UNUSED(dataOut);
    Log::error(TRACE_CRYPTO) << "rsaCrypt() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::hmac(uint32_t keyHandle, const Variant& algorithm,
           const DataBuffer& data,
           /*out*/DataBuffer& hmac)
{
    ScopedMutex lock(mutex_);
    const KeySpec * keySpec = getKeySpec(keyHandle);
    if (!keySpec)
        return NFErr_BadParameter;

    switch(keySpec->source)
    {
        case KeySpec::SOURCE_INVALID:
            return NFErr_MalformedData;

        case KeySpec::SOURCE_REE:
            Log::error(TRACE_CRYPTO) << "Asked to do hmac with REE key " << keySpec->handle;
            return NFErr_Bad;

        case KeySpec::SOURCE_TEE:
        {
            NTRACE(TRACE_CRYPTO, "TEE: hmac");
            const Algorithm shaAlgo = variantToHashAlgorithm(algorithm);
            NFErr ret = teeCryptoPtr_->hmac(keySpec->handle, shaAlgo, data, hmac);
            if (ret != NFErr_OK){
                hmac.clear();
                Log::error(TRACE_CRYPTO) << "TEE hmac returned error " << ret.toString();
                return ret;
            }
            NTRACE(TRACE_CRYPTO, "TEE hmac was successful");
            break;
        }
    }
    return NFErr_OK;
}

NFErr WebCryptoTee::hmacVerify(uint32_t keyHandle, const Variant& algorithm,
                       const DataBuffer& data,
                       const DataBuffer& signature,
                       /*out*/bool& isVerified)
{
    isVerified = false;
    ScopedMutex lock(mutex_);
    const KeySpec * keySpec = getKeySpec(keyHandle);
    if (!keySpec)
        return NFErr_BadParameter;

    switch(keySpec->source)
    {
        case KeySpec::SOURCE_INVALID:
            return NFErr_MalformedData;

        case KeySpec::SOURCE_REE:
            Log::error(TRACE_CRYPTO) << "Asked to do hmac verify with REE key " << keySpec->handle;
            return NFErr_Bad;

        case KeySpec::SOURCE_TEE:
        {
            NTRACE(TRACE_CRYPTO, "TEE: hmacVerify");
            const Algorithm shaAlgo = variantToHashAlgorithm(algorithm);
            NFErr ret = teeCryptoPtr_->hmacVerify (keySpec->handle, shaAlgo, data, signature, isVerified);
            if (ret != NFErr_OK){
                Log::error(TRACE_CRYPTO) << "TEE hmacVerify returned error " << ret.toString();
                isVerified = false;
                return ret;
            }
            NTRACE(TRACE_CRYPTO, "TEE hmacVerify was successful");
            break;
        }
    }
    return NFErr_OK;
}

NFErr WebCryptoTee::rsaKeyGen(const Variant &algorithmVariant, bool extractable,
                unsigned int keyUsage,
                /*out*/uint32_t& pubKeyHandle,
                /*out*/uint32_t& privKeyHandle)
{
    NRDP_UNUSED(algorithmVariant);
    NRDP_UNUSED(extractable);
    NRDP_UNUSED(keyUsage);
    NRDP_UNUSED(pubKeyHandle);
    NRDP_UNUSED(privKeyHandle);
    Log::error(TRACE_CRYPTO) << "rsaKeyGen() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::rsaSign(uint32_t keyHandle, const Variant &algorithm,
    const DataBuffer &data, /*out*/DataBuffer& sig)
{
    NRDP_UNUSED(keyHandle);
    NRDP_UNUSED(algorithm);
    NRDP_UNUSED(data);
    NRDP_UNUSED(sig);
    Log::error(TRACE_CRYPTO) << "rsaSign() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::rsaVerify(uint32_t keyHandle, const Variant &algorithm,
                              const DataBuffer& data, const DataBuffer& sig,
                              /*out*/bool& isVerified)
{
    ScopedMutex lock(mutex_);

    const KeySpec * keySpec = getKeySpec(keyHandle);
    if (!keySpec)
        return NFErr_BadParameter;

    switch(keySpec->source)
    {
        case KeySpec::SOURCE_INVALID:
            return NFErr_MalformedData;

        case KeySpec::SOURCE_REE: {
            if (!hasReeKey(keySpec->handle))
                return NFErr_BadParameter;

            // verify the provided key is permitted this usage
            if (!reeIsUsageAllowed(keySpec->handle, VERIFY)){
                NTRACE(TRACE_CRYPTO, "WebCryptoTee::rsaVerify: operation disallowed by keyUsage");
                return NFErr_BadParameter;
            }

            // verify the provided key is intended for this algorithm
            if (!reeIsKeyAlgMatch(keySpec->handle, RSASSA_PKCS1_V1_5)){
                NTRACE(TRACE_CRYPTO,
                       "WebCryptoTee::rsaVerify: operation incompatible with key algorithm");
                return NFErr_BadParameter;
            }

            // determine the algorithm
            Algorithm theAlgorithm = variantToHashAlgorithm(algorithm);
            if (!isHashAlgorithm(theAlgorithm)){
                NTRACE(TRACE_CRYPTO, "rsaVerify: bad algorithm %d", theAlgorithm);
                return NFErr_BadParameter;
            }
            isVerified = reeKeyMap_[keySpec->handle].pRsaContext->publicVerify(data, algorithmToShaAlgo(theAlgorithm), sig);
            break; }
        case KeySpec::SOURCE_TEE:
            Log::error(TRACE_CRYPTO) << "Asked to do rsaVerify with TEE key " << keySpec->handle;
            return NFErr_Bad;
    }
    return NFErr_OK;
}

NFErr WebCryptoTee::dhKeyGen(const Variant& algVar, bool extractable,
                             unsigned int keyUsage,
                             /*out*/uint32_t& pubKeyHandle,
                             /*out*/uint32_t& privKeyHandle)
{
    ScopedMutex lock(mutex_);
    NFErr err = NFErr_Bad;

    if (extractable)
    {  // do DH in REE
        Log::error(TRACE_CRYPTO) << "WebCryptoTee::dhDerive() does not support operation with extractable key.";
    }
    else
    { // do DH in TEE
        uint32_t teePublicKeyHandle = kInvalidKeyHandle;
        uint32_t teePrivateKeyHandle = kInvalidKeyHandle;
        // get prime from algorithm object
        if (!algVar.contains("params") && !algVar["params"].contains("prime"))
        {
            NTRACE(TRACE_CRYPTO, "WebCryptoTee::dhKeyGen: algorithm params missing prime");
            return NFErr_BadParameter;
        }
        const DataBuffer primeBuf = algVar["params"].mapValue<DataBuffer>("prime");
        NTRACE(TRACE_CRYPTO, "\tprime: %s", primeBuf.toBase64().c_str());
        if (primeBuf.empty())
        {
            NTRACE(TRACE_CRYPTO, "TeeApiCrypto::dhKeyGen: prime is empty");
            return NFErr_BadParameter;
        }
        // get generator from algorithm object
        if (!algVar.contains("params") && !algVar["params"].contains("generator"))
        {
            NTRACE(TRACE_CRYPTO, "WebCryptoTee::dhKeyGen: algorithm params missing generator");
            return NFErr_BadParameter;
        }
        const DataBuffer generatorBuf = algVar["params"].mapValue<DataBuffer>("generator");
        NTRACE(TRACE_CRYPTO, "\tgenerator: %s", generatorBuf.toBase64().c_str());
        if (generatorBuf.empty())
        {
            NTRACE(TRACE_CRYPTO, "WebCryptoTee::dhKeyGen: generator is empty");
            return NFErr_BadParameter;
        }

        err = teeCryptoPtr_->dhKeyGen(generatorBuf, primeBuf,
                keyUsage, teePublicKeyHandle, teePrivateKeyHandle);

        if (err == NFErr_OK)
        {
            pubKeyHandle = allocateKeyHandle();
            privKeyHandle = allocateKeyHandle();
            keyMap_[pubKeyHandle] = KeySpec(KeySpec::SOURCE_TEE, teePublicKeyHandle);
            keyMap_[privKeyHandle] = KeySpec(KeySpec::SOURCE_TEE, teePrivateKeyHandle);
        }
    }

    return err;
}

NFErr WebCryptoTee::dhDerive(uint32_t keyHandle, const DataBuffer& peerPublicKeyData,
                             const Variant& derivedAlgObj, bool extractable,
                             unsigned int keyUsage,
                             /*out*/uint32_t& outKeyHandle)
{
    NRDP_UNUSED(keyHandle);
    NRDP_UNUSED(peerPublicKeyData);
    NRDP_UNUSED(derivedAlgObj);
    NRDP_UNUSED(extractable);
    NRDP_UNUSED(keyUsage);
    NRDP_UNUSED(outKeyHandle);
    Log::error(TRACE_CRYPTO) << "dhDerive() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::nflxDhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
        uint32_t derivationKeyHandle,
        /*out*/uint32_t& encryptionKeyHandle,
        /*out*/uint32_t& hmacKeyHandle,
        /*out*/uint32_t& wrappingKeyHandle)
{
    ScopedMutex lock(mutex_);
    NFErr err = NFErr_Bad;
    const KeySpec *baseKeySpec = getKeySpec(baseKeyHandle);
    const KeySpec *derivationKeySpec = getKeySpec(derivationKeyHandle);

    if (!baseKeySpec or !derivationKeySpec)
        return NFErr_BadParameter;

    if (baseKeySpec->source != derivationKeySpec->source)
        return NFErr_BadParameter;

    switch(baseKeySpec->source)
    {
        case KeySpec::SOURCE_INVALID:
        return NFErr_MalformedData;
        case KeySpec::SOURCE_REE:{
        Log::error(TRACE_CRYPTO) << "WebCryptoTee::nflxDhDerive(): does not support option is REE ";
        return NFErr_MalformedData;
        }
        case KeySpec::SOURCE_TEE:{
        uint32_t teeEncryptionKeyHandle = kInvalidKeyHandle;
        uint32_t teeHmacKeyHandle = kInvalidKeyHandle;
        uint32_t teeWrappingKeyHandle = kInvalidKeyHandle;
        err = teeCryptoPtr_->nflxDhDerive(baseKeySpec->handle, peerPublicKeyData, derivationKeySpec->handle,
                                                    teeEncryptionKeyHandle, teeHmacKeyHandle, teeWrappingKeyHandle);
        if (err == NFErr_OK){
            encryptionKeyHandle = allocateKeyHandle();
            hmacKeyHandle = allocateKeyHandle();
            wrappingKeyHandle = allocateKeyHandle();
            keyMap_[encryptionKeyHandle]=KeySpec(KeySpec::SOURCE_TEE, teeEncryptionKeyHandle);
            keyMap_[hmacKeyHandle]=KeySpec(KeySpec::SOURCE_TEE, teeHmacKeyHandle);
            keyMap_[wrappingKeyHandle]=KeySpec(KeySpec::SOURCE_TEE, teeWrappingKeyHandle);
        }
        return err;
        }
    }
    return NFErr_OK;
}

NFErr WebCryptoTee::symKeyGen(const Variant &algorithm, bool extractable,
                unsigned int keyUsage,
                /*out*/uint32_t &keyHandle)
{
    NRDP_UNUSED(algorithm);
    NRDP_UNUSED(extractable);
    NRDP_UNUSED(keyUsage);
    NRDP_UNUSED(keyHandle);
    Log::error(TRACE_CRYPTO) << "symKeyGen() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::unwrapJwe(const DataBuffer& jweData, uint32_t wrappingKeyHandle,
                const Variant& algVar, bool extractable,
                unsigned int keyUsage,
                /*out*/uint32_t& keyHandle)
{
    ScopedMutex lock(mutex_);
    NRDP_UNUSED(jweData);
    NRDP_UNUSED(wrappingKeyHandle);
    NRDP_UNUSED(algVar);
    NRDP_UNUSED(extractable);
    NRDP_UNUSED(keyUsage);
    NRDP_UNUSED(keyHandle);
    Log::error(TRACE_CRYPTO) << "WebCryptoTee::unwrapJwe() not implemented.";
    return NFErr_NotImplemented;
}


NFErr WebCryptoTee::wrapJwe(uint32_t toBeWrappedKeyHandle, uint32_t wrappingKeyHandle,
              const Variant& wrappingAlgoObj, JweEncMethod jweEncMethod,
              /*out*/DataBuffer& wrappedKeyJcs)
{
    NRDP_UNUSED(toBeWrappedKeyHandle);
    NRDP_UNUSED(wrappingKeyHandle);
    NRDP_UNUSED(wrappingAlgoObj);
    NRDP_UNUSED(jweEncMethod);
    NRDP_UNUSED(wrappedKeyJcs);
    Log::error(TRACE_CRYPTO) << "wrapJwe() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::aesUnwrap(const DataBuffer & data, const Variant & algVar,
                uint32_t keyHandle,
                unsigned int keyUsage,
                /*out*/uint32_t& outKeyHandle)
{
    ScopedMutex lock(mutex_);
    NRDP_UNUSED(data);
    NRDP_UNUSED(algVar);
    NRDP_UNUSED(keyHandle);
    NRDP_UNUSED(keyUsage);
    NRDP_UNUSED(outKeyHandle);
    Log::error(TRACE_CRYPTO) << "WebCryptoTee::aesUnwrap() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::eccSign(uint32_t keyHandle, const Variant &algorithm,
    const DataBuffer &data, /*out*/DataBuffer& sig)
{
    NRDP_UNUSED(keyHandle);
    NRDP_UNUSED(algorithm);
    NRDP_UNUSED(data);
    NRDP_UNUSED(sig);
    Log::error(TRACE_CRYPTO) << "eccSign() not implemented.";
    return NFErr_NotImplemented;
}

NFErr WebCryptoTee::addNamedKey(const std::string & name, uint32_t keyHandle)
{
    NRDP_UNUSED(name);
    NRDP_UNUSED(keyHandle);
    Log::error(TRACE_CRYPTO) << "addNamedKey() not implemented.";
    return NFErr_NotImplemented;
}

uint32_t WebCryptoTee::findKeyByOtherHandle(KeySpec::Source source, uint32_t handle) const
{
    for (KeyMap::const_iterator it = keyMap_.begin(); it != keyMap_.end(); ++it )
        if (it->second.source == source && it->second.handle == handle)
            return it->first;
    return kInvalidKeyHandle;
}

NFErr WebCryptoTee::getNamedKey(const std::string & name,
                                /*out*/ uint32_t & keyHandle)
{
    ScopedMutex lock(mutex_);
    NTRACE(TRACE_CRYPTO, "WebCryptoTee: getNamedKey %s", name.c_str());

    // try to get the key from REE first, if not there, try to get it from TEE
    uint32_t reeHandle = kInvalidKeyHandle;
    bool foundInRee = false;
    for (ReeKeyMap::const_iterator it = reeKeyMap_.begin(); it != reeKeyMap_.end(); it++){
        NTRACE(TRACE_CRYPTO, "WebCryptoTee: considering key named \"%s\"", it->second.name.c_str());
        if (it->second.name==name){
            reeHandle=it->first;
            foundInRee=true;
            break;
        }
    }

    if (foundInRee) {
        keyHandle = findKeyByOtherHandle(KeySpec::SOURCE_REE, reeHandle);

        /* if this instance was not aware of the named key, make it aware now! */
        if (keyHandle == kInvalidKeyHandle)
        {
            keyHandle = allocateKeyHandle();
            keyMap_[keyHandle] = KeySpec(KeySpec::SOURCE_REE, reeHandle);
            NTRACE(TRACE_CRYPTO, "WebCryptoTee::getNamedKey: allocating handle %u for REE named key %s", keyHandle, name.c_str());
        }
        return NFErr_OK;
    }

    /* named key was not found in REE; looking for it in TEE */
    uint32_t teeHandle = kInvalidKeyHandle;
    NFErr ret = teeCryptoPtr_->getNamedKey(name, teeHandle);
    if (ret != NFErr_OK) {
        Log::error(TRACE_CRYPTO) << "TEE getNamedKey returned error " << ret.toString();
        return ret;
    }
    keyHandle = findKeyByOtherHandle(KeySpec::SOURCE_TEE, teeHandle);

    if (keyHandle == kInvalidKeyHandle) {
        keyHandle = allocateKeyHandle();
        keyMap_[keyHandle] = KeySpec(KeySpec::SOURCE_TEE, teeHandle);
        NTRACE(TRACE_CRYPTO, "WebCryptoTee::getNamedKey: allocating handle %u for TEE named key %s", keyHandle, name.c_str());
    }
    return NFErr_OK;
}


static const std::string TeePersistedKeysStorageKey("WCTEE/keys");
NFErr WebCryptoTee::teePersistKeys(const std::vector<uint32_t> & keyHandles)
{
    ScopedMutex lock(mutex_);
    teePersistentKeys_.clear();
    // find keys
    Variant keys;
    for(std::vector<uint32_t>::const_iterator it = keyHandles.begin(); it != keyHandles.end(); ++it)
    {
        const uint32_t handle(*it);
        KeyMap::const_iterator k = keyMap_.find(handle);
        if (k == keyMap_.end() || k->second.source!=KeySpec::SOURCE_TEE)
        {
            Log::trace(TRACE_CRYPTO, "WebCryptoTee::teePersistKeys: invalid key handle");
            return NFErr_BadParameter;
        }
        // send the key to TEE for packaging
        DataBuffer keyData;
        NTRACE(TRACE_CRYPTO, "teePersistKeys: master handle %d, source %d, tee handle %d", handle, k->second.source, k->second.handle);
        NFErr status = teeCryptoPtr_->exportSealedKey(k->second.handle, keyData);
        if (status != NFErr_OK)
        {
            NTRACE(TRACE_CRYPTO, "TEE exportSealedKey failed %s\n", status.toString().c_str());
            return status;
        }
        Variant key;
        key["handle"] = handle;
        key["keyData"] = keyData;
        NTRACE(TRACE_CRYPTO, "teePersistKeys: key value for handle %d is %s\n", handle, keyData.toString().c_str());
        keys.push_back(key);
        teePersistentKeys_.insert(handle);
    }
    NrdApplication * app = nrdApp();

    std::string value = keys.empty() ? std::string() : keys.toJSON();

    app->writeSystemConfiguration(TeePersistedKeysStorageKey, value);

    return NFErr_OK;
}

NFErr WebCryptoTee::persistKeys(const std::vector<uint32_t> & keyHandles)
{
    ScopedMutex lock(mutex_);

    // If the array is empty, this is a special case - we have to call both the
    // REE and the TEE with an empty array
    if (keyHandles.empty())
    {
        NTRACE(TRACE_CRYPTO, "WebCryptoTee::persistKeys(): key vector empty");
        return NFErr_OK;
    }

    // Otherwise, we sort the handles into REE and TEE handles
    std::vector<uint32_t> reeHandles;
    std::vector<uint32_t> teeHandles;
    for (std::vector<uint32_t>::const_iterator it = keyHandles.begin();it != keyHandles.end(); ++it)
    {
        const KeySpec * keySpec = getKeySpec(*it);
        switch(keySpec->source) {
            case KeySpec::SOURCE_INVALID:
                return NFErr_MalformedData;
            case KeySpec::SOURCE_REE:
                reeHandles.push_back(keySpec->handle);
                break;
            case KeySpec::SOURCE_TEE:
                teeHandles.push_back(*it);
                break;
        }
    }
    if (!reeHandles.empty()) {
        Log::error(TRACE_CRYPTO) << "WebCryptoTee::persistKeys(): can not persist REE side keys";
        return NFErr_MalformedData;
    }

    if (!teeHandles.empty()) {
        NFErr teeErr = teePersistKeys(teeHandles);
        if (!teeErr.ok())
            return teeErr;
    }

    // TODO: Now, we also have to save our map of KeySpecs for these handles

    return NFErr_OK;
}

NFErr WebCryptoTee::clearKeys(std::set<uint32_t> & keyHandles)
{
    ScopedMutex lock(mutex_);
    KeyHandleSet keysToDelete;
    // figure out which keys to delete
    for (KeyMap::iterator it = keyMap_.begin(); it != keyMap_.end();it++){
        if (!(KeySpec::SOURCE_REE == it->second.source && reeKeyMap_[it->second.handle].name != "") /* the key is not a REE named key */
                && teePersistentKeys_.find(it->first)==teePersistentKeys_.end()                     /* and it's not a TEE persistent key */
                && reePersistentKeys_.find(it->second.handle)==reePersistentKeys_.end()){           /* and it's not a REE persistent key */
            keysToDelete.insert(it->first);
        }
    }
    // iterate through keysToDelete and delete the keys
    for (KeyHandleSet::iterator it=keysToDelete.begin(); it!=keysToDelete.end(); it++){
        NFErr err = this->deleteKey(*it);
        if (err != NFErr_OK){
            Log::error(TRACE_CRYPTO) << "WebCryptoTee::clearKeys(): calling this->deleteKey() returned " << err.toString();
        }
        // insert the key in output keyHandles
        keyHandles.insert(*it);
    }

    return NFErr_OK;
}

NFErr WebCryptoTee::deleteKey(uint32_t keyHandle)
{
    ScopedMutex lock(mutex_);

    const KeySpec * keySpec = getKeySpec(keyHandle);
    if (!keySpec){
        Log::error(TRACE_CRYPTO) << "WebCryptoTee::deleteKey(): keySpec not found for master handle: " << keyHandle;
        return NFErr_BadParameter;
    }

    NTRACE(TRACE_CRYPTO, "WebCryptoTee::deleteKeys(): deleting key: master handle %d, source %d, handle %d", keyHandle, keySpec->source, keySpec->handle);

    NFErr err;
    switch(keySpec->source){
    case KeySpec::SOURCE_INVALID:
        return NFErr_MalformedData;

    case KeySpec::SOURCE_REE: {
        const uint32_t specHandle = keySpec->handle;
        if ("" != reeKeyMap_[specHandle].name)
            Log::warn(TRACE_CRYPTO) << "Deleting REE named key \"%s\"" << reeKeyMap_[specHandle].name.c_str();
        if (reeKeyMap_.erase(specHandle)!=1){
            Log::error(TRACE_CRYPTO) << "WebCryptoTee::deleteKey(): deleting a REE key failed";
            return NFErr_MalformedData;
        }
        if (keyMap_.erase(keyHandle)!=1){
            Log::error(TRACE_CRYPTO) << "WebCryptoTee::deleteKey(): deleting a REE key failed.  master handle doesn't exist.";
            return NFErr_MalformedData;
        };
        reePersistentKeys_.erase(specHandle);
        break; }
    case KeySpec::SOURCE_TEE:
        NFErr ret = teeCryptoPtr_->deleteKey(keySpec->handle);
        if (ret != NFErr_OK){
            Log::error(TRACE_CRYPTO) << "WebCryptoTee::deleteKey(): TEE delete key returned error " << ret.toString();
            return ret;
        }
        NTRACE(TRACE_CRYPTO, "WebCryptoTee::deleteKey(): TEE delete key was successful");
        keyMap_.erase(keyHandle);
        teePersistentKeys_.erase(keyHandle);
        break;
    }
    return NFErr_OK;
}

//////////
//
// END - WebCryptoTee
//
//////////

uint32_t WebCryptoTee::allocateKeyHandle()
{
    ScopedMutex lock(mutex_);

    static uint32_t nextHandle = kStartKeyHandle;
    const KeyMap::const_iterator end(keyMap_.end());
    while (keyMap_.find(nextHandle) != end)
        ++nextHandle;
    return nextHandle++;
}

const WebCryptoTee::KeySpec * WebCryptoTee::getKeySpec(uint32_t keyHandle) const
{
    ScopedMutex lock(mutex_);
    KeyMap::const_iterator it = keyMap_.find(keyHandle);
    if (it == keyMap_.end())
        return NULL;
    return &it->second;
}

void WebCryptoTee::teeRestorePersistedKeys()
{
    ScopedMutex lock(mutex_);
    teePersistentKeys_.clear();
    NrdApplication * app = nrdApp();
    if (!app)
    {
        Log::warn(TRACE_CRYPTO, "Not restoring persisted keys, no app");
        return;
    }
    const std::string json = app->readSystemConfiguration(TeePersistedKeysStorageKey);
    if (json.empty())
        return;

    Variant keyArray = Variant::fromJSON(json);
    if (!keyArray.isArray())
    {
        Log::warn(TRACE_CRYPTO, "Failed to parse persisted keys as JSON");
        return;
    }
    const std::vector<Variant> keys(keyArray.array());
    for (Variant::ArrayIterator it = keys.begin(); it != keys.end(); ++it)
    {
        if (uint32_t handle = (*it)["handle"].integer())
        {
            DataBuffer keyData((*it)["keyData"].string());
            NTRACE(TRACE_CRYPTO, "teeRestor: handle %d key: %s", handle, keyData.toString().c_str());
            //send it to TEE to get key values
            nfCryptoKeyHandle_t teeKeyHandle;
            nfKeyType_t keyType;
            nfCryptoAlgorithm_t algorithm;
            nfKeyUsageFlagsWord_t 	keyUsageFlags;
            uint32_t keyBitCount;
            NFErr status = teeCryptoPtr_->importSealedKey(keyData, teeKeyHandle, keyType, algorithm,
                                                        keyUsageFlags, keyBitCount);
            if (status != NFErr_OK)
            {
                NTRACE(TRACE_CRYPTO, "TEE importSealedKey failed %s", status.toString().c_str());
                return;
            }
            //map it to the keyMap_
            keyMap_[handle] = KeySpec(KeySpec::SOURCE_TEE, teeKeyHandle);
            teePersistentKeys_.insert(handle);
        }
    }
}

//////////
//
// ree-specific functionality
//
//////////

bool WebCryptoTee::hasReeKey(uint32_t keyHandle) const
{
    ScopedMutex lock(mutex_);
    return reeKeyMap_.count(keyHandle);
}

uint32_t WebCryptoTee::allocateReeKeyHandle()
{
    ScopedMutex lock(mutex_);

    static uint32_t nextHandle = kStartKeyHandle;
    const ReeKeyMap::const_iterator end(reeKeyMap_.end());
    while (reeKeyMap_.find(nextHandle) != end)
        ++nextHandle;
    return nextHandle++;
}

bool WebCryptoTee::reeIsUsageAllowed(uint32_t reeKeyHandle, KeyUsage keyUsage)
{
    assert(hasReeKey(reeKeyHandle));
    const Key key = reeKeyMap_[reeKeyHandle];
    return !key.keyUsage || (key.keyUsage & keyUsage);   // empty keyUsage means all allowed
}

bool WebCryptoTee::reeIsKeyAlgMatch(uint32_t reeKeyHandle, Algorithm algorithm)
{
    assert(hasReeKey(reeKeyHandle));
    const Algorithm keyAlgorithm = variantToAlgorithm(reeKeyMap_[reeKeyHandle].algVar);
    return keyAlgorithm == algorithm;
}

//////////
//
// algorithm utilities
//
//////////

bool WebCryptoTee::isAlgorithmCompatibleWithUsage(Algorithm algorithm, unsigned int keyUsage)
{
    // empty usage means default usage, so always ok
    if (!keyUsage)
        return true;

    unsigned int allowed;
    switch (algorithm) {
    case HMAC:
    case RSASSA_PKCS1_V1_5:
        allowed = SIGN|VERIFY;
        break;
    case AES_CBC:
        allowed = ENCRYPT|DECRYPT|UNWRAP;
        break;
    case RSAES_PKCS1_V1_5:
    case AES_GCM:
    case AES_CTR:
        allowed = ENCRYPT|DECRYPT;
        break;
    case RSA_OAEP:
        allowed = ENCRYPT|DECRYPT|UNWRAP|WRAP;
        break;
    case AES_KW:
        allowed = WRAP|UNWRAP;
        break;
    default:
        return false;
    }
    return !(keyUsage & ~(allowed));
}

IWebCrypto::Algorithm WebCryptoTee::variantToAlgorithm(const Variant &algVar)
{
    bool success = true;
    std::string algNameStr;
    if (algVar.isString())
    {
        algNameStr = algVar.string();
    }
    else if (algVar.isStringMap())
    {
        algNameStr = algVar.mapValue<std::string>("name", &success);
        if (!success)
        {
            NTRACE(TRACE_CRYPTO,
                   "variantToAlgorithm: unable to extract name from algorithm object");
        }
    }
    return stringToAlgorithm(algNameStr);
}

IWebCrypto::Algorithm WebCryptoTee::stringToAlgorithm(const std::string& algorithmStr)
{
    if (strCaseCmp(algorithmStr, "HMAC") == 0)
        return HMAC;
    else if (strCaseCmp(algorithmStr, "AES-CBC") == 0)
        return AES_CBC;
    else if (strCaseCmp(algorithmStr, "AES-GCM") == 0)
        return AES_GCM;
    else if (strCaseCmp(algorithmStr, "AES-CTR") == 0)
        return AES_CTR;
    else if (strCaseCmp(algorithmStr, "RSAES-PKCS1-v1_5") == 0)
        return RSAES_PKCS1_V1_5;
    else if (strCaseCmp(algorithmStr, "RSASSA-PKCS1-v1_5") == 0)
        return RSASSA_PKCS1_V1_5;
    else if (strCaseCmp(algorithmStr, "RSA-OAEP") == 0)
        return RSA_OAEP;
    else if (strCaseCmp(algorithmStr, "SHA-1") == 0)
        return SHA1;
    else if (strCaseCmp(algorithmStr, "SHA-224") == 0)
        return SHA224;
    else if (strCaseCmp(algorithmStr, "SHA-256") == 0)
        return SHA256;
    else if (strCaseCmp(algorithmStr, "SHA-384") == 0)
        return SHA384;
    else if (strCaseCmp(algorithmStr, "SHA-512") == 0)
        return SHA512;
    else if (strCaseCmp(algorithmStr, "DH") == 0)
        return DH;
    else if (strCaseCmp(algorithmStr, "NFLX-DH") == 0)
        return NFLX_DH;
    else if (strCaseCmp(algorithmStr, "AES-KW") == 0)
        return AES_KW;
    else
        return INVALID_ALGORITHM;
}

IWebCrypto::Algorithm WebCryptoTee::variantToHashAlgorithm(const Variant &algoVar)
{
    Variant hashVar = algoVar["params"]["hash"];
    return variantToAlgorithm(hashVar);
}

RsaContext::ShaAlgo WebCryptoTee::algorithmToShaAlgo(Algorithm shaAlgo)
{
    switch (shaAlgo)
    {
    case SHA1:   return RsaContext::SHA1;
    case SHA224: return RsaContext::SHA224;
    case SHA256: return RsaContext::SHA256;
    case SHA384: return RsaContext::SHA384;
    case SHA512: return RsaContext::SHA512;
    default:
        assert(false);
        break;
    }
    return RsaContext::SHA256;  // to keep compiler happy
}

std::string WebCryptoTee::algorithmToString(Algorithm algorithm)
{
    switch (algorithm)
    {
    case HMAC: return "HMAC";
    case AES_CBC: return "AES-CBC";
    case AES_GCM: return "AES-GCM";
    case AES_CTR: return "AES-CTR";
    case RSAES_PKCS1_V1_5: return "RSAES-PKCS1-v1_5";
    case RSASSA_PKCS1_V1_5: return "RSASSA-PKCS1-v1_5";
    case RSA_OAEP: return "RSA-OAEP";
    case SHA1: return "SHA-1";
    case SHA224: return "SHA-224";
    case SHA256: return "SHA-256";
    case SHA384: return "SHA-384";
    case SHA512: return "SHA-512";
    case AES_KW: return "AES-KW";
    case DH: return "DH";
    case NFLX_DH: return "NFLX-DH";
    case SYSTEM: return "SYSTEM";
    default:
    case INVALID_ALGORITHM: return "";
    }
}

bool WebCryptoTee::isHashAlgorithm(Algorithm algorithm)
{
    return (algorithm == SHA1   ||
            algorithm == SHA224 ||
            algorithm == SHA256 ||
            algorithm == SHA384 ||
            algorithm == SHA512);
}

