/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "OpenSSLException.h"

#include <stdlib.h>
#include <ostream>
#include <sstream>
#include <openssl/bio.h>
#include <openssl/err.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Log.h>

using namespace netflix;

namespace {
/**
 * Returns the current OpenSSL error string.
 *
 * @return the OpenSSL error string or empty.
 */
std::string openssl_err()
{
    char *cstr = NULL;
    shared_ptr<BIO> err_bio(BIO_new(BIO_s_mem()), BIO_free);
    if (!err_bio.get())
    {
        NTRACE(TRACE_CRYPTO, "BIO_new(BIO_s_mem())");
        return "";
    }
    ERR_print_errors(err_bio.get());
    int len = BIO_get_mem_data(err_bio.get(), &cstr);
    if (len == 0 || cstr == NULL)
    {
        NTRACE(TRACE_CRYPTO, "BIO_get_mem_data(err_bio, &cstr)");
        return "";
    }
    std::string err(cstr);
    return err;
}
} // namespace anonymous

OpenSSLException::OpenSSLException(const std::string& /*msg*/)
{}

OpenSSLException::OpenSSLException(const std::string& /*msg*/,
                                   const std::string& openssl_err)
    : openssl_err_(openssl_err)
{}

void OpenSSLException::throw_message(const char* msg, const char* file,
                                     int line, const char* function)
{
    const std::string errstack = openssl_err();
    std::ostringstream ostr;
    ostr << "OpenSSL Error: " << file << ":" << line
         << ":" << function << std::endl
         << "    '" << msg << "'" << std::endl
         << "    OpenSSL Error Stack:" << std::endl
         << "        " << errstack << std::endl;
    NTRACE(TRACE_CRYPTO, "%s", ostr.str().c_str());
    fprintf(stderr, "%s", ostr.str().c_str());
    ::abort();
}
