/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef WeakPtrMap_h
#define WeakPtrMap_h

#include <map>
#include <nrdbase/tr1.h>

namespace netflix {
template <typename Key, typename T>
class WeakPtrMap : public
#ifdef NRDP_REQUIRE_OWNERLESS
std::map<weak_ptr<Key>, T, std::owner_less<weak_ptr<Key> > >
#else
std::map<weak_ptr<Key>, T>
#endif
{
public:
#ifdef NRDP_REQUIRE_OWNERLESS
    typedef std::map<weak_ptr<Key>, T, std::owner_less<weak_ptr<Key> > > Base;
#else
    typedef std::map<weak_ptr<Key>, T> Base;
#endif

    size_t lockKeys(std::vector<shared_ptr<T> > &listeners)
    {
        listeners.reserve(Base::size());
        typename WeakPtrMap<Key, T>::iterator it = Base::begin();
        const typename WeakPtrMap<Key, T>::iterator end = Base::end();

        size_t ret = 0;
        while (it != end) {
            if (shared_ptr<Key> locked = it->first.lock()) {
                listeners.push_back(locked);
                ++ret;
                ++it;
            } else {
                Base::erase(it++);
            }
        }
        return ret;
    }
};
}

#endif
