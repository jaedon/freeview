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

#ifndef __RSACONTEXT_H__
#define __RSACONTEXT_H__

#include <stdint.h>
#include <nrdbase/DataBuffer.h>
#include <openssl/evp.h>

namespace netflix {

class RsaContext
{
public:
    enum ShaAlgo {SHA1, SHA224, SHA256, SHA384, SHA512};
    enum Padding {NONE, PKCS1, PKCS1_OAEP};

public:
    RsaContext();
    ~RsaContext();

    bool generate(uint32_t nBits, uint64_t publicExponent);

    bool setRaw(const DataBuffer &pubMod,
                const DataBuffer &pubExp,
                const DataBuffer &privMod);
    bool getPublicRaw(/*out*/DataBuffer &pubMod,
                      /*out*/DataBuffer &pubExp);

    bool setPublicPkcs1(const DataBuffer &pubKeyPkcs1Der);
    bool getPublicPkcs1(/*out*/DataBuffer &pubKeyPkcs1Der) const;

    bool setPublicSpki(const DataBuffer &pubKeySpkiDer);
    bool getPublicSpki(/*out*/DataBuffer &pubKeySpkiDer) const;

    bool setPrivatePkcs1(const DataBuffer &privKeyPkcs1Der);
    bool getPrivatePkcs1(/*out*/DataBuffer &privKeyPkcs1Der) const;

    bool setPrivatePkcs8(const DataBuffer &pkcs8);
    bool getPrivatePkcs8(/*out*/DataBuffer &pkcs8) const;

    bool publicEncrypt(const DataBuffer &n,
                       /*out*/DataBuffer &out, Padding padding = PKCS1);
    bool privateDecrypt(const DataBuffer &in,
                        /*out*/DataBuffer &out, Padding padding = PKCS1);

    bool privateSign(const DataBuffer &data, ShaAlgo shaAlgo,
                     /*out*/DataBuffer &sig);
    bool publicVerify(const DataBuffer &data, ShaAlgo shaAlgo,
                      const DataBuffer &sig);

    bool hasPublicKey() const {return hasPublicKey_;}

private:
    DataBuffer computeDigest(const DataBuffer &data, ShaAlgo shaAlgo);

private:
    RSA * pOsslRsa_;
    bool hasPrivateKey_;
    bool hasPublicKey_;
};

} // namespace netflix::crypto

#endif // __RSACONTEXT_H__
