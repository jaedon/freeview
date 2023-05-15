/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef WeakPtrSet_h
#define WeakPtrSet_h

#include <set>
#include <nrdbase/tr1.h>

namespace netflix {
template <typename T>
class WeakPtrSet : public
#ifdef NRDP_REQUIRE_OWNERLESS
std::set<weak_ptr<T>, std::owner_less<weak_ptr<T> > >
#else
std::set<weak_ptr<T> >
#endif
{
public:
#ifdef NRDP_REQUIRE_OWNERLESS
    typedef std::set<weak_ptr<T>, std::owner_less<weak_ptr<T> > > Base;
#else
    typedef std::set<weak_ptr<T> > Base;
#endif

    inline size_t lock(std::vector<shared_ptr<T> > &listeners)
    {
        if(const size_t size = Base::size()) {
            listeners.reserve(size);
            typename WeakPtrSet<T>::iterator it = Base::begin();
            const typename WeakPtrSet<T>::iterator end = Base::end();

            size_t ret = 0;
            while (it != end) {
                if (shared_ptr<T> locked = it->lock()) {
                    listeners.push_back(locked);
                    ++ret;
                    ++it;
                } else {
                    Base::erase(it++);
                }
            }
            return ret;
        }
        return 0;
    }
};
}

#endif
