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

#ifndef __SCOPEDOPENSSL_H__
#define __SCOPEDOPENSSL_H__

namespace netflix {

// A helper class that takes care of destroying OpenSSL objects when they go out
// of scope.
template <typename T, void (*destructor)(T*)>
class ScopedOpenSSL
{
public:
    ScopedOpenSSL() : ptr_(NULL) {}
    explicit ScopedOpenSSL(T* ptr) : ptr_(ptr) {}
    ~ScopedOpenSSL() {reset(NULL);}
    T* get() const {return ptr_;}
    T* release()
    {
        T* ptr = ptr_;
        ptr_ = NULL;
        return ptr;
    }
    void reset(T* ptr)
    {
        if (ptr != ptr_)
        {
            if (ptr_) (*destructor)(ptr_);
            ptr_ = ptr;
        }
    }
private:
    T* ptr_;
    ScopedOpenSSL(const ScopedOpenSSL&);
    void operator=(const ScopedOpenSSL&);
};

}  // namespace netflix::crypto

#endif // __SCOPEDOPENSSL_H__
