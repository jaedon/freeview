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

#ifndef __AESCBCCIPHER_H__
#define __AESCBCCIPHER_H__

#include <stdint.h>
#include <nrdbase/DataBuffer.h>

#include <openssl/aes.h>
#include <openssl/bn.h>

namespace netflix {

class AesCbcCipher
{
public:
    AesCbcCipher(const DataBuffer& key,
                 const DataBuffer& iv);
    ~AesCbcCipher();

    bool encrypt(const DataBuffer& clear,
                 /*out*/DataBuffer& encrypted);
    bool decrypt(const DataBuffer& encrypted,
                 /*out*/DataBuffer& clear);

    enum { BLOCKSIZE = AES_BLOCK_SIZE };
    enum KeyLength { KL128=16, KL192=24, KL256=32 };

private:
    const EVP_CIPHER* getCipher() const;

private:
    const DataBuffer key_;
    const DataBuffer iv_;
    const KeyLength keyLength_;
};

} // namespace netflix::crypto

#endif // __AESCBCCIPHER_H__
