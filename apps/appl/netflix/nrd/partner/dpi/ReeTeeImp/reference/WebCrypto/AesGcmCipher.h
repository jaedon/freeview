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

#ifndef __AESGCMCIPHER_H__
#define __AESGCMCIPHER_H__

#include <nrdbase/DataBuffer.h>
#include <openssl/bn.h>

namespace netflix {

class AesGcmCipher
{
public:
    AesGcmCipher(const DataBuffer& key,
                 const DataBuffer& iv);
    ~AesGcmCipher();

    bool encrypt(const DataBuffer& clearText,
                 const DataBuffer& aad,
                 DataBuffer& cipherText,
                 DataBuffer& mac);
    bool decrypt(const DataBuffer& cipherText,
                 const DataBuffer& aad,
                 const DataBuffer& mac,
                 DataBuffer& clearText);
    enum KeyLength { KL128=16, KL192=24, KL256=32 };

private:
    const EVP_CIPHER* getCipher() const;

private:
    const DataBuffer key_;
    const DataBuffer iv_;
    const KeyLength keyLength_;
};

} // namespace netflix::crypto

#endif // __AESGCMCIPHER_H__
