/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef __HMAC_H__
#define __HMAC_H__

#include <openssl/hmac.h>

#include <nrdbase/Noncopyable.h>
#include <nrdbase/DigestAlgo.h>

namespace netflix {

/**
 * Keyed Hash Message Authentication Code.
 * Keyed hash of data used for message authentication.
 */
class HMAC : private Noncopyable {
public:

    /**
     * Constructor.
     * Constructs an HMAC object with the default hashing algorithm.
     * @param key the key used to compute an HMAC
     */
    template <typename T>
    HMAC(const T &key, const shared_ptr<const DigestAlgo> &algo = DigestAlgo::SHA256())
    {
        assert(algo);
        init(&key[0], key.size(), algo->evp_md());
    }

    HMAC(const unsigned char *data, size_t len,
         const shared_ptr<const DigestAlgo> &algo = DigestAlgo::SHA256())
    {
        assert(algo);
        init(data, len, algo->evp_md());
    }

    /** Destructor. */
    ~HMAC()
    {
        HMAC_cleanup(&mCtx);
    }

    /**
     * Initialize this HMAC object for use.  Each HMAC object can be reused
     * after a call to final(), if init() is called for each session.
     */
    void init()
    {
        // use of HMAC_Init_ex with key == NULL reuses previous key
        // use of HMAC_Init_ex with EVP_MD == NULL reuses previous EVP_MD
        HMAC_Init_ex(&mCtx, /*key*/NULL, /*key_len*/0, /*EVP_MD*/NULL, /*ENGINE*/NULL);
    }

    /**
     * Add more data to the HMAC calculation.
     * @param data data to add to the calculation
     * @see final()
     */
    template <typename T>
    void update(const T &data)
    {
        update(&data[0], data.size());
    }

    /**
     * Add more data to the HMAC calculation.
     * @param data data to add to the calculation
     * @len the length of the data
     */
    void update(const void *data, size_t len)
    {
        HMAC_Update(&mCtx, reinterpret_cast<const unsigned char*>(data), len);
    }

    /**
     * Complete the HMAC calculation.
     * @return the HMAC (not Base64 encoded).
     */
    template <typename T>
    T final()
    {
        T out;
        out.resize(EVP_MAX_MD_SIZE);
        unsigned int sz = 0;
        HMAC_Final(&mCtx, &out[0], &sz);
        out.resize(sz);
        return out;
    }

    template <typename T>
    T hmac(const T &t)
    {
        init();
        update(t);
        return final<T>();
    }
    template <typename T>
    T hmac(const void *data, size_t len)
    {
        init();
        update(data, len);
        return final<T>();
    }
private:
    void init(const unsigned char *data, size_t len, const EVP_MD *evp_md)
    {
        HMAC_CTX_init(&mCtx);
        HMAC_Init(&mCtx, data, len, evp_md);
    }

    HMAC_CTX mCtx;
};

} // namespace netflix

#endif
