/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <ostream>
#include <sstream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/asn1.h>
#include <openssl/rand.h>
#include <openssl/objects.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Log.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/Thread.h>

#include "OpenSSLLib.h"

#include <nrdbase/Random.h>

using namespace netflix;

// note: no thread safety on setting init() / shutdown()
bool OpenSSLLib::init_ssl_ = false;
bool OpenSSLLib::init_crypto_ = false;
int OpenSSLLib::hostnameIndex_ = -1;

#ifndef NRDP_NO_SSL_THREAD_CALLBACKS
/* thread safety mutexes */
static Mutex **mutex_array = NULL;

/* thread safety callbacks */
static unsigned long id_func();
static void lock_func(int mode, int n, const char * file, int line);
#endif


NFErr OpenSSLLib::init(const unsigned char* random, uint32_t length)
{
    NFErr err = init_crypto(random, length);
    if (err != NFErr_OK)
        return err;
    return init_ssl();
}

NFErr OpenSSLLib::init(const DataBuffer &random)
{
    return OpenSSLLib::init(random.data(), random.size());
}

NFErr OpenSSLLib::init_crypto(const std::vector<unsigned char> random)
{
    return init_crypto(&random[0], random.size());
}

NFErr OpenSSLLib::init_crypto(const unsigned char* random, uint32_t length)
{
    if (init_crypto_)
    {
        Log::warn(TRACE_CRYPTO,
               "OpenSSLLib::init_crypto() called more than once");
        return NFErr_InvalidStateTransition_OpenSSLLib;
    }

    // init multi-thread support
    NFErr err = thread_init();
    if (err != NFErr_OK)
        return err;

    // load error strings for libcrypto
    ERR_load_crypto_strings();

    if (random) {
        // init RNG
        err = random_init(random, length);
        if (err != NFErr_OK)
            return err;
    }

    // add all algorithms
    // TODO: consider only loading needed algos
#ifndef NRDP_NO_SSL_INIT
    OpenSSL_add_all_algorithms();
#endif

    // Set init flag and return success.
    init_crypto_ = true;
    return NFErr_OK;
}

NFErr OpenSSLLib::init_ssl()
{
    if (init_ssl_)
    {
        Log::warn(TRACE_CRYPTO, "OpenSSLLib::init_ssl() called more than once");
        return NFErr_InvalidStateTransition_OpenSSLLib;
    }
    if (!init_crypto_)
    {
        Log::warn(TRACE_CRYPTO, "OpenSSLLib::init_ssl() called without first "
               "calling OpenSSLLib::init_crypto()");
        return NFErr_Uninitialized;
    }

    // load error strings for libssl
    SSL_load_error_strings();

#ifndef NRDP_NO_SSL_INIT
    // init SSL library
    SSL_library_init();
#endif

    // grab some private storage for hostname verification
    OpenSSLLib::hostnameIndex_ = SSL_get_ex_new_index(0, NULL, NULL, NULL, NULL);

    // Set init flag and return success.
    init_ssl_ = true;
    return NFErr_OK;
}

void OpenSSLLib::cleanup()
{
    cleanup_ssl();
    cleanup_crypto();
}

void OpenSSLLib::cleanup_ssl()
{
    init_ssl_ = false;
    sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
}

NFErr OpenSSLLib::cleanup_crypto()
{
    if (init_ssl_)
    {
        Log::warn(TRACE_CRYPTO, "OpenSSLLib::cleanup_crypto() called without "
               "first calling OpenSSLLib::cleanup_ssl()");
        return NFErr_InvalidStateTransition_OpenSSLLib;
    }

    thread_cleanup();
    // delete ciphers & hash algos from openssl
    EVP_cleanup();
    // delete debug strings
    ERR_free_strings();
    init_crypto_ = false;
    return NFErr_OK;
}

void OpenSSLLib::add_entropy(const unsigned char* random, uint32_t length,
                             double entropy)
{
    RAND_add(random, length, entropy);
}

void OpenSSLLib::add_entropy(std::vector<unsigned char> random, double entropy)
{
    OpenSSLLib::add_entropy(&random[0], random.size(), entropy);
}

int OpenSSLLib::hmacSha1(const std::string &key, const std::string &message, unsigned char *md)
{
    unsigned int count = 0;
    (void)HMAC(EVP_sha1(), reinterpret_cast<const void *>(key.data()), key.size(),
               reinterpret_cast<const unsigned char*>(message.data()), message.size(),
               md, &count);
    return count;
}

NFErr OpenSSLLib::certificatesFromPem(const void *pem, int length, std::vector<void*> *certificates, CertType type)
{
    if (!pem || length <= 0 || !certificates)
        return NFErr_OK;

    BIO *in = BIO_new_mem_buf(const_cast<void*>(pem), length);
    if (!in)
        return NFErr_SSLFailure;

    do {
        void *x = 0;
        if(type == CertType_X509)
            x = PEM_read_bio_X509(in, 0, 0, 0);
        else if(type == CertType_PrivateKey)
            x = PEM_read_bio_PrivateKey(in, 0, 0, 0);
        if (!x)
            break;
        certificates->push_back(x);
    } while (true);

    BIO_free(in);
    return NFErr_OK;
}

void OpenSSLLib::freeCertificate(void *cert, CertType type)
{
    if(type == CertType_X509)
        X509_free(reinterpret_cast<X509*>(cert));
    else if(type == CertType_PrivateKey)
        EVP_PKEY_free(reinterpret_cast<EVP_PKEY*>(cert));
}

NFErr OpenSSLLib::random_init(const unsigned char* random, uint32_t length)
{
    if (length < MIN_SEED_LEN)
    {
        Log::warn(TRACE_CRYPTO, "random seed size (%d) < %zu",
               length, MIN_SEED_LEN);
        return NFErr_BadParameter;
    }
    RAND_seed(random, length);
    // also set Random.init_ so that users of that class can test for
    // RNG initialization
    Random::init_ = true;
    return NFErr_OK;
}

NFErr OpenSSLLib::thread_init()
{
#ifndef NRDP_NO_SSL_THREAD_CALLBACKS
    if (mutex_array)
    {
        Log::warn(TRACE_CRYPTO, "thread_init() called more than once");
        return NFErr_InvalidStateTransition_OpenSSLLib;
    }

    mutex_array = (Mutex **)malloc(
        CRYPTO_num_locks() * sizeof(Mutex *));
    for (int i=0; i<CRYPTO_num_locks(); ++i)
        mutex_array[i] = new Mutex(OPENSSLLIB_MUTEX, "OpenSSLLib");

    /* set threading callbacks */
    CRYPTO_set_id_callback(id_func);
    CRYPTO_set_locking_callback(lock_func);
#endif
    return NFErr_OK;
}

void OpenSSLLib::thread_cleanup()
{
#ifndef NRDP_NO_SSL_THREAD_CALLBACKS
    if (mutex_array)
    {
        /* unset callbacks */
        CRYPTO_set_id_callback(NULL);
        CRYPTO_set_locking_callback(NULL);

        /* delete mutex_array */
        for (int i=0; i<CRYPTO_num_locks(); ++i)
            delete mutex_array[i];
        free(mutex_array);

        mutex_array = NULL;
    }
#endif
}

#ifndef NRDP_NO_SSL_THREAD_CALLBACKS
static unsigned long id_func(void)
{
    return ((unsigned long)Thread::CurrentThreadPtr());
}

static void lock_func(int mode, int n, const char *, int)
{
    if (!mutex_array)
    {
        Log::warn(TRACE_CRYPTO, "uninitialized mutex array");
        return;
    }

    /* check CRYPTO_LOCK flag */
    if (mode & CRYPTO_LOCK)
        mutex_array[n]->lock();
    else
        mutex_array[n]->unlock();
}
#endif
