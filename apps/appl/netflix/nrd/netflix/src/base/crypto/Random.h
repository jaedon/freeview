/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Random_hpp
#define Random_hpp

#include <memory>
#include <vector>

#include <nrdbase/Noncopyable.h>
#include <nrdbase/tr1.h>

namespace netflix {
class OpenSSLLib;

/**
 * @class Random Random.hpp
 * @brief Class used to obtain random bytes.
 */
class Random : private Noncopyable {
public:
    /**
     * Non random seed suitable for use with OpenSSLLib::init() when
     * testing. Do not use in production.
     */
    static unsigned char NON_RANDOM_SEED[];

    static std::vector<unsigned char> next(size_t num_bytes);
    static void next(unsigned char * buffer, size_t num_bytes);

private:
    static bool init_;
    friend class OpenSSLLib;
};

} // namespace netflix::crypto

#endif /* Random_hpp */
