/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file tr1.h C++ Technical Report 1 abstraction header.
 *
 * This file provides an abstraction to the C++ TR1 extensions.
 */
#ifndef TR1_H
#define TR1_H

#include <memory>
#include <math.h>
#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__) // c++11
#include <cmath>
#define NRDP_CPP11
#define NRDP_REQUIRE_OWNERLESS
// non-apple clangs have a different versioning scheme than apple clangs
#endif
#ifdef NRDP_CPP11
using std::isnormal;
using std::isnan;
using std::isinf;
using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::enable_shared_from_this;
#elif defined(_LIBCPP_VERSION)
#define NRDP_REQUIRE_OWNERLESS
using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::enable_shared_from_this;
#elif defined(__CWCC__) && defined(RVL)
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::static_pointer_cast;
using std::tr1::enable_shared_from_this;
/* GCC 4 supplies std::tr1, but SN does not */
#elif __GNUC__ >= 4 && !defined(__SNC__)
#include <tr1/memory>
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::static_pointer_cast;
using std::tr1::enable_shared_from_this;
#elif defined(__EDG__)
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
namespace std {
namespace tr1 {
using boost::shared_ptr;
using boost::weak_ptr;
using boost::static_pointer_cast;
using boost::enable_shared_from_this;
}
}
using boost::shared_ptr;
using boost::weak_ptr;
using boost::static_pointer_cast;
using boost::enable_shared_from_this;
#elif defined(_WIN32)
#include <memory>
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::static_pointer_cast;
using std::tr1::enable_shared_from_this;
namespace std {
template<class _Ty1, class _Ty2>
bool operator<(const weak_ptr<_Ty1>& _W1, const weak_ptr<_Ty2>& _W2) {
    return (_W1.owner_before(_W2));
}
}
#if !defined(_XBOX_ONE)
template<class _Ty1, class _Ty2>
bool operator<(const weak_ptr<_Ty1>& _W1, const weak_ptr<_Ty2>& _W2) {
    return (_W1.owner_before(_W2));
}
#endif
#else
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
namespace std {
namespace tr1 {
using boost::shared_ptr;
using boost::weak_ptr;
using boost::static_pointer_cast;
using boost::enable_shared_from_this;
}
}
using boost::shared_ptr;
using boost::weak_ptr;
using boost::static_pointer_cast;
using boost::enable_shared_from_this;
#endif

#endif

