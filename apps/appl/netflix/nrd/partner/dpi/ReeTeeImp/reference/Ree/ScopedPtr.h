/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
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

#ifndef __SCOPEDPTR_H__
#define __SCOPEDPTR_H__

#include <cstdio>
/*
 * ScopedPtr.h
 *
 * A helper class that takes care of destroying pointer objects when they go out of scope.
 */

template <typename T, void (*destructor)(T*)>
class ScopedPtr
{
public:
	ScopedPtr() : ptr_(NULL) {}
    explicit ScopedPtr(T* ptr) : ptr_(ptr) {}
    ~ScopedPtr() {reset(NULL);}
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
    ScopedPtr(const ScopedPtr&);
    void operator=(const ScopedPtr&);
};

#endif // __SCOPEDPTR_H__
