/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef __DIGESTER_H__
#define __DIGESTER_H__

#include <openssl/evp.h>
#include <vector>

#include <nrdbase/tr1.h>
#include <nrdbase/DataBuffer.h>

#include "DigestAlgo.h"

namespace netflix {

/**
 * @class Digester Digester.h
 * @brief Digester for hashing data using SHA1, etc.
 */
class Digester
{
public:
    /**
     * Create a digester using a hashing algorithm
     *
     * @param[in] algo The hash algorithm to use
     */
    Digester(shared_ptr<const DigestAlgo> algo);
    ~Digester();

    /**
     * Initialize the digester
     */
    void init();

    /**
     * Update the accumulated hash with the new data
     *
     * @param[in] data new data to add to hash
     * @param[in] sz size of new data
     */
    void update(const std::vector<unsigned char> &data) { update(&data[0], data.size()); }
    void update(const std::string &data) { update(reinterpret_cast<const unsigned char *>(data.data()), data.size()); }
    void update(const DataBuffer &buffer) { update(buffer.data(), buffer.size()); }
    void update(const unsigned char *data, size_t sz);

    /**
     * Get the final hash of the data
     *
     * @return the hash of the accumulated data
     */
    inline std::vector<unsigned char> final() { std::vector<unsigned char> vec; final(vec); return vec; }
    int final(DataBuffer &buffer);
    int final(std::vector<unsigned char> &vec);


    /**
     * Gets the hashing algortihm
     *
     * @return the digester's hashing algorithm
     */
    shared_ptr<const DigestAlgo> algo() const { return algo_; }

private:
    const shared_ptr<const DigestAlgo> algo_;
    EVP_MD_CTX ctx_;
};

} // namespace netflix::crypto

#endif // __DIGESTER_H__
