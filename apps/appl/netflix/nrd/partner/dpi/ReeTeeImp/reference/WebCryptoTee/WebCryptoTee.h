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

#ifndef __WEBCRYPTOTEE_H__
#define __WEBCRYPTOTEE_H__

#include <map>
#include <nrd/IWebCrypto.h>

#include <nrdbase/Mutex.h>
#include <nrdbase/Variant.h>
#include <nrdbase/tr1.h>

#include <nrddpi/WebCrypto.h>
#include <nrddpi/TeeApiCrypto.h>
#include <nrddpi/TeeApiMgmt.h>

namespace netflix {

class WebCryptoTee : public IWebCrypto
{
public:
    WebCryptoTee(netflix::device::ISystem::AuthenticationType authType,
                 shared_ptr<TeeApiMgmt> teeMgmtPtr,
                 shared_ptr<TeeApiCrypto> teeCryptoPtr);

    virtual ~WebCryptoTee();

    virtual NFErr importKey(KeyFormat format, const DataBuffer& keyData,
                            const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType);

    virtual NFErr exportKey(uint32_t keyHandle, KeyFormat format,
                            /*out*/DataBuffer &keyData);

    virtual NFErr getKeyInfo(uint32_t keyHandle, /*out*/KeyType &type,
                             /*out*/bool &extractable, /*out*/Variant &algorithm,
                             /*out*/unsigned int &usage) const;

    virtual NFErr digest(const Variant &algorithm, const DataBuffer& data,
                         /*out*/DataBuffer& digest);

    virtual NFErr aesCbc(uint32_t keyHandle, const DataBuffer& ivIn,
                         const DataBuffer& dataIn, CipherOp cipherOp,
                         /*out*/DataBuffer& dataOut);

    virtual NFErr aesGcmEncrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                const DataBuffer& dataIn, const DataBuffer& aadIn,
                                /*out*/DataBuffer& tagOut,
                                /*out*/DataBuffer& dataOut);

    virtual NFErr aesGcmDecrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                const DataBuffer& dataIn, const DataBuffer& aadIn,
                                const DataBuffer& tagIn,
                                /*out*/DataBuffer& dataOut) ;

    virtual NFErr rsaCrypt(uint32_t keyHandle, const DataBuffer& dataIn,
                           CipherOp cipherOp, /*out*/DataBuffer& dataOut);

    virtual NFErr hmac(uint32_t keyHandle, const Variant& algorithm,
                       const DataBuffer& data,
                       /*out*/DataBuffer& hmac);

    virtual NFErr hmacVerify(uint32_t keyHandle, const Variant& algorithm,
                       const DataBuffer& data,
                       const DataBuffer& signature,
                       /*out*/bool& isVerified);

    virtual NFErr rsaKeyGen(const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& pubKeyHandle,
                            /*out*/uint32_t& privKeyHandle);

    virtual NFErr rsaSign(uint32_t keyHandle, const Variant &algorithm,
                          const DataBuffer& data, /*out*/DataBuffer& sig);

    virtual NFErr rsaVerify(uint32_t keyHandle, const Variant &algorithm,
                            const DataBuffer& data, const DataBuffer& sig,
                            /*out*/bool& isVerified);

    virtual NFErr dhKeyGen(const Variant& algVar, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& pubKeyHandle,
                           /*out*/uint32_t& privKeyHandle);

    virtual NFErr dhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
                           const Variant& derivedAlgObj, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& keyHandle);

    virtual NFErr nflxDhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
                               uint32_t derivationKeyHandle,
                               /*out*/uint32_t& encryptionKeyHandle,
                               /*out*/uint32_t& hmacKeyHandle,
                               /*out*/uint32_t& wrappingKeyHandle);

    virtual NFErr symKeyGen(const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t &keyHandle);

    virtual NFErr unwrapJwe(const DataBuffer& jweData, uint32_t wrappingKeyHandle,
                            const Variant& algVar, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle);

    virtual NFErr wrapJwe(uint32_t toBeWrappedKeyHandle, uint32_t wrappingKeyHandle,
                          const Variant& wrappingAlgoObj, JweEncMethod jweEncMethod,
                          /*out*/DataBuffer& wrappedKeyJcs);

    virtual NFErr aesUnwrap(const DataBuffer & data, const Variant & algVar,
                            uint32_t wrappingKeyHandle,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle);

    virtual NFErr eccSign(uint32_t keyHandle, const Variant &algorithm,
                          const DataBuffer& data, /*out*/DataBuffer& sig);

    virtual NFErr addNamedKey(const std::string & name, uint32_t keyHandle);

    virtual NFErr getNamedKey(const std::string & name,
                               /*out*/ uint32_t & keyHandle);

    virtual NFErr persistKeys(const std::vector<uint32_t> & keyHandles);

    virtual NFErr clearKeys(std::set<uint32_t> & keyHandles);

    virtual NFErr deleteKey(uint32_t keyHandle);


private:
    struct Key {
        Key() : type(SECRET), extractable(false) {}
        Key(const DataBuffer &key, shared_ptr<RsaContext> pRsaContext,
            KeyType type, bool extractable, const Variant &algVar,
            unsigned int keyUsage, std::string name) : key(key), pRsaContext(pRsaContext),
            type(type), extractable(extractable), algVar(algVar),
            keyUsage(keyUsage), name(name) {}
        Variant toJSON() const;

        DataBuffer key;
        shared_ptr<RsaContext> pRsaContext;
        shared_ptr<DiffieHellmanContext> pDhContext;
        KeyType type;
        bool extractable;
        Variant algVar;
        unsigned int keyUsage;
        std::string name;
    };

    struct KeySpec {
        enum Source {
            SOURCE_INVALID,
            SOURCE_REE,
            SOURCE_TEE
        };

        KeySpec() : source(SOURCE_INVALID), handle(kInvalidKeyHandle) {}
        KeySpec(Source _source, uint32_t _handle) : source(_source), handle(_handle) {}

        Source      source; // Where the key lives
        uint32_t    handle; // The handle where it lives
    };

    mutable Mutex mutex_;
    shared_ptr<TeeApiMgmt> teeMgmtPtr_;
    shared_ptr<TeeApiCrypto> teeCryptoPtr_;

    static const uint32_t kStartKeyHandle = 1;
    static const uint32_t kInvalidKeyHandle = 0;

    typedef std::map<uint32_t, KeySpec> KeyMap;
    KeyMap keyMap_;

    typedef std::map<uint32_t, Key> ReeKeyMap;
    ReeKeyMap reeKeyMap_;

    typedef std::set<uint32_t> KeyHandleSet;
    KeyHandleSet teePersistentKeys_;
    KeyHandleSet reePersistentKeys_;


    uint32_t allocateKeyHandle();
    const KeySpec *getKeySpec(uint32_t keyHandle) const;
    uint32_t findKeyByOtherHandle(KeySpec::Source source, uint32_t handle) const;

    NFErr teePersistKeys(const std::vector<uint32_t> & keyHandles);
    void  teeRestorePersistedKeys();


    /* ree-specific functionality */
    bool hasReeKey(uint32_t keyHandle) const;
    uint32_t allocateReeKeyHandle();
    bool reeIsUsageAllowed(uint32_t reeKeyHandle, KeyUsage keyUsage);
    bool reeIsKeyAlgMatch(uint32_t reeKeyHandle, Algorithm algorithm);
    NFErr reeImportKey(KeyFormat format, const DataBuffer& keyData,
                       const Variant &algorithm, bool extractable,
                       unsigned int keyUsage, std::string name,
                       /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType);



    /* algorithm utilities */
    static bool isAlgorithmCompatibleWithUsage(Algorithm algorithm, unsigned int keyUsage);
    static Algorithm variantToAlgorithm(const Variant &algVar);
    static Algorithm stringToAlgorithm(const std::string& algorithmStr);
    static Algorithm variantToHashAlgorithm(const Variant &algoVar);
    static RsaContext::ShaAlgo algorithmToShaAlgo(Algorithm shaAlgo);
    static std::string algorithmToString(Algorithm algorithm);
    static bool isHashAlgorithm(Algorithm algorithm);
};


} // namespace netflix::crypto

#endif // __WEBCRYPTOTEE_H__
