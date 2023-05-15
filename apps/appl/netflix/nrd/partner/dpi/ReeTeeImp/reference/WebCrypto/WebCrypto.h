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

#ifndef __WEBCRYPTO_H__
#define __WEBCRYPTO_H__

#include <map>

#include <nrd/ISystem.h>
#include <nrd/IWebCrypto.h>

#include <nrdbase/Mutex.h>
#include <nrdbase/Variant.h>
#include <nrdbase/tr1.h>

#include "AesCbcCipher.h"
#include "AesGcmCipher.h"
#include "DiffieHellmanContext.h"
#include "RsaContext.h"
#include "AesKeyWrapper.h"


namespace netflix {

class RsaContext;
class DiffieHellmanContext;

class WebCrypto : public IWebCrypto
{
public:
    WebCrypto(netflix::device::ISystem::AuthenticationType authType);
    virtual ~WebCrypto();

    virtual NFErr init(/*const DataBuffer &prngSeed*/);

    void addEntropy(const DataBuffer& entropyBytes);

    virtual NFErr digest(const Variant &algorithm, const DataBuffer& data,
                         /*out*/DataBuffer& digest);
    virtual NFErr importKey(KeyFormat format, const DataBuffer& keyData,
                            const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType);
    virtual NFErr exportKey(uint32_t keyHandle, KeyFormat format,
                            /*out*/DataBuffer& keyData);
    virtual NFErr getKeyInfo(uint32_t keyHandle, /*out*/KeyType &type,
                             /*out*/bool &extractable, /*out*/Variant &algorithm,
                             /*out*/unsigned int &usage) const;
    virtual NFErr aesCbc(uint32_t keyHandle, const DataBuffer& ivIn,
                         const DataBuffer& dataIn, CipherOp cipherOp,
                         /*out*/DataBuffer& dataOut);
    virtual NFErr aesGcmEncrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                const DataBuffer& dataIn, const DataBuffer& aadIn,
                                /*out*/DataBuffer& tagOut, /*out*/DataBuffer& dataOut);
    virtual NFErr aesGcmDecrypt(uint32_t keyHandle, const DataBuffer& ivIn,
                                const DataBuffer& dataIn, const DataBuffer& aadIn,
                                const DataBuffer& tagIn, /*out*/DataBuffer& dataOut);
    virtual NFErr rsaCrypt(uint32_t keyHandle, const DataBuffer& dataIn,
                           CipherOp cipherOp, /*out*/DataBuffer& dataOut);
    virtual NFErr hmac(uint32_t keyHandle, const Variant &algorithm,
                       const DataBuffer& data, /*out*/DataBuffer& hmac);
    virtual NFErr hmacVerify(uint32_t keyHandle, const Variant &algorithm,
                       const DataBuffer& data, const DataBuffer& signature, /*out*/bool& isVerified);
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
    virtual NFErr unwrapJwe(const DataBuffer& jweData, uint32_t wrappingKeyHandle,
                            const Variant& algVar, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle);
    virtual NFErr wrapJwe(uint32_t toBeWrappedKeyHandle, uint32_t wrappingKeyHandle,
                          const Variant& wrappingAlgoObj, JweEncMethod jweEncMethod,
                          /*out*/DataBuffer& wrappedKeyJcs);
    virtual NFErr symKeyGen(const Variant &algorithm, bool extractable,
                            unsigned int keyUsage,
                            /*out*/uint32_t &keyHandle);
    virtual NFErr aesUnwrap(const DataBuffer &data, const Variant &algVar,
                            uint32_t wrappingKeyHandle,
                            unsigned int keyUsage,
                            /*out*/uint32_t& keyHandle);

    virtual NFErr eccSign(uint32_t keyHandle, const Variant &algorithm,
                          const DataBuffer& data, /*out*/DataBuffer& sig);

    virtual NFErr addNamedKey(const std::string &name, uint32_t keyHandle);
    virtual NFErr getNamedKey(const std::string &name,
                              /*out*/ uint32_t &keyHandle);

    virtual NFErr persistKeys(const std::vector<uint32_t> & keyHandles);
    virtual NFErr clearKeys(std::set<uint32_t> & keyHandles);

    virtual NFErr deleteKey(uint32_t keyHandle);


    // utility methods
    static IWebCrypto::Algorithm getAlgorithmFromVar(const Variant &algVar);
    static WebCrypto::Algorithm toAlgorithm(const std::string& algorithmStr);
    static std::string toString(IWebCrypto::Algorithm algorithm);
    static bool isJweJs(const DataBuffer& input);
    // end utility methods

#if defined(BUILD_DEBUG)
    int getDHKeySharedSecretLength(uint32_t keyHandle) const;
#endif

protected:
    // utility methods
    static const uint32_t kStartKeyHandle = 1;
    static const uint32_t kInvalidKeyHandle = 0;

    RsaContext::ShaAlgo xShaAlgo(WebCrypto::Algorithm shaAlgo);
    bool isAlgorithmRsa(WebCrypto::Algorithm algorithm);
    bool isAlgorithmAes(WebCrypto::Algorithm algorithm);
    bool isAlgorithmHmac(WebCrypto::Algorithm algorithm);
    bool isAlgorithmSha(WebCrypto::Algorithm algorithm);
    bool isAlgorithmDh(WebCrypto::Algorithm algorithm);

    static IWebCrypto::Algorithm getHashAlgorithmFromVar(const Variant &algoVar);
    bool reconcileAlgVsUsage(WebCrypto::Algorithm algorithm, unsigned int keyUsage);
    bool isHashSpecPresent(const Variant& algVar);
    int extractIntFromString(const std::string& str);
    // end utility methods

    uint32_t addInternalKey(const std::string & name, const Variant& algVar, unsigned int usage);

    uint32_t allocateKeyHandle();
    mutable Mutex mutex_;
    bool isInited_;

    friend class WebCryptoTee;

private:
    struct Key
    {
    public:
        DataBuffer key;
        shared_ptr<RsaContext> pRsaContext;
        shared_ptr<DiffieHellmanContext> pDhContext;
        KeyType type;
        bool extractable;
        Variant algVar;
        unsigned int keyUsage;

    public:
        Key();
        Key(const DataBuffer &key, shared_ptr<RsaContext> pRsaContext,
            KeyType kt, bool extractable, const Variant &algVar,
            unsigned int usg);
        Key(const DataBuffer &key,
            shared_ptr<DiffieHellmanContext> pDhContext, KeyType kt, bool extractable,
            const Variant& algVar, unsigned int usg);
        Key(const DataBuffer &key,
            KeyType kt, bool extractable,
            const Variant& algVar, unsigned int usg);
        Key(const Key& rhs);
        Key& operator=(const Key& rhs);
        Key(const Variant& json);

        Variant toJSON() const;
    };

private:
    bool hasKey(uint32_t keyHandle) const;
    bool isUsageAllowed(uint32_t keyHandle, KeyUsage keyUsage);
    bool isKeyAlgMatch(uint32_t keyHandle, Algorithm algorithm);

    NFErr importJwk(const DataBuffer &keyVuc, const Variant& algVar,
                    bool extractable, unsigned int keyUsage,
                    /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType);
    NFErr exportJwk(const Key& key, DataBuffer& jwkJSON);
    NFErr unwrapJwe(const std::vector<DataBuffer>& jweData, uint32_t wrappingKeyHandle,
                    const Variant& algVar, bool extractable,
                    unsigned int keyUsage, /*out*/uint32_t& keyHandle);
    NFErr aesPre(uint32_t keyHandle, KeyUsage keyUsage,
                 const DataBuffer& ivInBuf,
                 Algorithm algorithm,
                 /*out*/DataBuffer &keyBuf);

    void restorePersistedKeys();
    void importNamedKeys();

    void dumpKeys();

private:
    //    bool isInited_;
    netflix::device::ISystem::AuthenticationType authType_;

    typedef std::map<uint32_t, Key> KeyMap;
    KeyMap keyMap_;

    typedef std::map<std::string, uint32_t> NamedKeyMap;
    NamedKeyMap namedKeyMap_;

    typedef std::set<uint32_t> KeyHandleSet;
    KeyHandleSet persistentKeys_;

#if 0
    void test();
#endif
};

} // namespace netflix::crypto

#endif // __WEBCRYPTO_H__
