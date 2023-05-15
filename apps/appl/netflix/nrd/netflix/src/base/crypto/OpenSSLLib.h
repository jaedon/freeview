/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file OpenSSLLib.h OpenSSL initialization and cleanup.
 *
 * This class provides methods to manage OpenSSL. If you manage
 * OpenSSL separately, you must still use this class to notify the NRD
 * library of the OpenSSL state.
 */
#ifndef OPENSSLLIB_H
#define OPENSSLLIB_H

#include <openssl/asn1.h>
#include <openssl/evp.h>
#include <string>
#include <vector>

#include <nrdbase/NFErr.h>

namespace netflix {

/**
 * @class OpenSSLLib OpenSSLLib.hpp
 * @brief OpenSSL library (libcrypto) initialization and cleanup
 *        routines.
 *
 * This class must be used to either initialize and cleanup the
 * OpenSSL library or to notify the NRD library if OpenSSL is
 * initialized or no longer initialized.
 *
 * To use this library to manage OpenSSL, use one of the
 * init(const unsigned char*, uint32_t) or
 * init(const std::vector<unsigned char>) methods and the cleanup()
 * method.
 *
 * If you manage OpenSSL separately, call setInit(bool) to set the
 * OpenSSL initialization state.
 */
class OpenSSLLib {
public:
    /** Minimum size of random seed needed to initialize OpenSSL. */
    static const size_t MIN_SEED_LEN = 512 / 8;

    /**
     * Initialize all components of the OpenSSL library. This calls
     * init_crypto() followed by init_ssl().
     *
     * @param[in] random seed for RNG, must be >= MIN_SEED_LEN in
     *            size.
     * @param[in] length of random seed.
     * @return NFErr_OK if successful;
     *         NFErr_BadParameter if the seed is too short;
     *         NFErr_InvalidStateTransition_OpenSSLLib if already initialized.
     * @sa OpenSSL::init(const std::vector<unsigned char>)
     */
    static NFErr init(const unsigned char* random, uint32_t length);

    /**
     * Initialize OpenSSL. This calls init_crypto() followed by
     * init_ssl().
     *
     * @param[in] random seed for RNG, must be >= MIN_SEED_LEN in
     *            size.
     * @return NFErr_OK if successful;
     *         NFErr_BadParameter if the seed is too short;
     *         NFErr_InvalidStateTransition_OpenSSLLib if already initialized.
     * @sa OpenSSLLib::init(const unsigned char* const, uint32)
     */
    static NFErr init(const DataBuffer &random);

    /**
     * Initialize the crypto portion (libcrypto) of the OpenSSL
     * library.
     *
     * @param[in] random seed for RNG, must be >= MIN_SEED_LEN in
     *            size.
     * @param[in] length of random seed.
     * @return NFErr_OK if successful;
     *         NFErr_BadParameter if the seed is too short;
     *         NFErr_InvalidStateTransition_OpenSSLLib if already initialized.
     */
    static NFErr init_crypto(const unsigned char* random, uint32_t length);

    /**
     * Initialize the crypto portion (libcrypto) of the OpenSSL
     * library.
     *
     * @param[in] random seed for RNG, must be >= MIN_SEED_LEN in
     *            size.
     * @return NFErr_OK if successful;
     *         NFErr_BadParameter if the seed is too short;
     *         NFErr_InvalidStateTransition_OpenSSLLib if already initialized.
     */
    static NFErr init_crypto(const std::vector<unsigned char> random);

    /**
     * Initialize the SSL portion (libssl) of the OpenSSL library.
     *
     * Note: init_crypto() must be called before init_ssl().
     *
     * @return NFErr_InvalidStateTransition_OpenSSLLib if called more than once;
     *         NFErr_Uninitialized if init_crypto() has not been
     *         called.
     */
    static NFErr init_ssl();

    /**
     * Cleanup OpenSSL when shutting down. This calls cleanup_ssl()
     * followed by cleanup_crypto().
     */
    static void cleanup();

    /**
     * Clean up the SSL portion (libssl) of the OpenSSL library.
     */
    static void cleanup_ssl();

    /**
     * Clean up the crypto portion (libcrypto) of the OpenSSL library.
     *
     * Note: cleanup_ssl() must be called before cleanup_crypto().
     *
     * @return NFErr_InvalidStateTransition_OpenSSLLib if cleanup_ssl() has not
     *         been called.
     */
    static NFErr cleanup_crypto();

    /**
     * Add some additional entropy to the RNG.
     *
     * @param[in] random data for RNG.
     * @param[in] length of data.
     * @param[in] entropy lower-bound of data entropy (see RFC
     *            1750). This is equal to the data length if the data
     *            is pulled from /dev/random.
     */
    static void add_entropy(const unsigned char* random, uint32_t length,
                            double entropy);

    /**
     * Add some additional entropy to the RNG.
     *
     * @param[in] random data for RNG.
     * @param[in] entropy lower-bound of data entropy (see RFC
     *            1750). This is equal to the data length if the data
     *            is pulled from /dev/random.
     */
    static void add_entropy(std::vector<unsigned char> random,
                            double entropy);

    /**
     * Returns a private data index where the remote hostname can be
     * stored. The value can be set using SSL_set_ex_data() and
     * retrieved using SSL_get_ex_data().
     *
     * @return a private data index for remote hostname storage.
     */
    static int hostnameIndex() { return hostnameIndex_; }

    static int hmacSha1(const std::string &key, const std::string &message, unsigned char *md);

    enum CertType {
        CertType_X509,
        CertType_PrivateKey
    };
    static NFErr certificatesFromPem(const void *data, int len, std::vector<void*> *certificates, CertType type=CertType_X509);
    static void freeCertificate(void *certificate, CertType type=CertType_X509);

    /**
     * Initializes the random number generator using the provided
     * seed.
     *
     * @param[in] random seed for RNG, must be >= MIN_SEED_LEN in
     *            size.
     * @param[in] length of random seed.
     * @return NFErr_OK if successful;
     *         NFErr_BadParameter if the random seed is too short.
     */
    static NFErr random_init(const unsigned char* random, uint32_t length);

private:
    /**
     * Initializes OpenSSL for use with threads.
     *
     * @return NFErr_InvalidStateTransition_OpenSSLLib if called more than once.
     */
    static NFErr thread_init();

    /**
     * Cleans up thread callbacks and mutexes.
     */
    static void thread_cleanup();

    static bool init_ssl_;     //!< True if libssl initialized.
    static bool init_crypto_;  //!< True if libcrypto initialized.
    static int hostnameIndex_; //!< Private data storage index.
};

} // namespace netflix::crypto

#endif
