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

#ifndef __AESKEYWRAPPER_H__
#define __AESKEYWRAPPER_H__

#include <nrdbase/DataBuffer.h>

namespace netflix {

class AesKeyWrapper
{
public:
    AesKeyWrapper(
        const DataBuffer& wrappingKey,
        const DataBuffer& initVal = DataBuffer());
    ~AesKeyWrapper();

    bool wrap(const DataBuffer& keyToWrap,
              /*out*/DataBuffer& wrappedKey);
    bool unwrap(const DataBuffer& wrappedKey,
                /*out*/DataBuffer& unwrappedKey);

private:
    AesKeyWrapper();    // not implemented
    const DataBuffer wrappingKey_;
    const DataBuffer initVal_;
};

} // namespace netflix::crypto

#endif // __AESKEYWRAPPER_H__
