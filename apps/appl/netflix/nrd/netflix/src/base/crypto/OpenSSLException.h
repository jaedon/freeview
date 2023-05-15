/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef __OPENSSLEXCEPTION_H__
#define __OPENSSLEXCEPTION_H__

#include <string>

#define OPENSSLEXCEPTION_MSG(msg) \
    OpenSSLException::throw_message(msg, __FILE__, __LINE__, __FUNCTION__)

#define OPENSSLEXCEPTION OPENSSLEXCEPTION_MSG("General OpenSSL Error")

namespace netflix {

/**
 * @class OpenSSLException OpenSSLException.hpp
 * @brief Exception thrown when an OpenSSL library error occurs.
 */
class OpenSSLException
{
public:
    /**
     * Constructs a new OpenSSL exception.
     *
     * @param[in] msg the general cause of the exception.
     */
    explicit OpenSSLException(const std::string &msg);

    /**
     * Constructs a new OpenSSL exception.
     *
     * @param[in] msg the general cause of the exception.
     * @param[in] openssl_err the specific OpenSSL failure
     *            information.
     */
    OpenSSLException(const std::string &msg, const std::string &openssl_err);

    /** Destructor. */
    virtual ~OpenSSLException() {}

    /**
     * Create and throw an OpenSSL exception containing the last
     * OpenSSL error and stack. The OpenSSL error and stack is also
     * traced.
     *
     * @param[in] msg the general cause of the exception.
     * @param[in] file the filename.
     * @param[in] line the line number.
     * @param[in] function the function name.
     * @throw OpenSSLException.
     */
    static void throw_message(const char* msg, const char* file, int line,
                              const char* function);

private:
    std::string openssl_err_; //!< OpenSSL error details.
};

} // namespace netflix::crypto

#endif // __OPENSSLEXCEPTION_H__
