/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 *  @file X509.h x509 wrapper.
 */
#ifndef X509_CERT_H
#define X509_CERT_H

#include <openssl/x509.h>
#include <string>

namespace netflix {

class X509Crl;

/**
 * @class X509Cert
 * @brief Wrapper around the openssl X509 class.
 */
class X509Cert
{
public:
    X509Cert(X509 *x = 0);

    /**
     *  @return the subject of the cert, may be null.
     */
    X509_NAME *getSubjectName() const;
    std::string getSubjectCommonName() const;

    /**
     *  @return the issuer of the cert, may be null.
     */
    X509_NAME *getIssuerName() const;
    std::string getIssuerCommonName() const;

    /**
     *  @return the serial number of the cert, may be null.
     */
    ASN1_INTEGER *getSerialNumber() const;
    std::string getSerialNumberHex() const;

    /** Use only for traces and logging.
     *
     * @return the not-before cert validity date
     */
    std::string getNotBeforePrettyString() const;

    /** Use only for traces and logging.
     *
     * @return the not-after cert validity date
     */
    std::string getNotAfterPrettyString() const;

#if DEBUG
    void print() const;
#endif

private:
    X509 *mX509;
};
}

#endif /* X509_CERT_H */
