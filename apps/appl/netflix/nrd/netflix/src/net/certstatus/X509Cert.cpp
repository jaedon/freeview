/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "X509Cert.h"
#include <nrdbase/Time.h>

using namespace netflix;

X509Cert::X509Cert(X509 *x)
    : mX509(x)
{
}

X509_NAME *X509Cert::getSubjectName() const {
    if (!mX509)
        return NULL;
    return X509_get_subject_name(mX509);
}

std::string X509Cert::getSubjectCommonName() const {
    if (!mX509)
        return std::string();
    std::string subject;
    X509_NAME *name = this->getSubjectName();
    char *nameBuf = X509_NAME_oneline(name, NULL, 0);
    if (nameBuf) {
        subject.append(nameBuf);
        free(nameBuf);
    }
    return subject;
}

X509_NAME *X509Cert::getIssuerName() const {
    if (!mX509)
        return NULL;
    return X509_get_issuer_name(mX509);
}

std::string X509Cert::getIssuerCommonName() const {
    if (!mX509)
        return std::string();
    std::string issuer;
    X509_NAME *name = this->getIssuerName();
    char *nameBuf = X509_NAME_oneline(name, NULL, 0);
    if (nameBuf) {
        issuer.append(nameBuf);
        free(nameBuf);
    }
    return issuer;
}



ASN1_INTEGER *X509Cert::getSerialNumber() const {
    if (!mX509)
        return NULL;
    return X509_get_serialNumber(mX509);
}

std::string X509Cert::getSerialNumberHex() const {
    if (!mX509)
        return NULL;

    ASN1_INTEGER *serNum = X509_get_serialNumber(mX509);
    if (NULL == serNum)
        return "";

    BIGNUM *bn = ASN1_INTEGER_to_BN(serNum, NULL);
    if (NULL == bn)
        return "";

    char *bnStr = BN_bn2hex(bn);
    if (NULL == bnStr)
        return "";

    std::string serNumStr(bnStr);

    // Free resources.
    BN_free(bn);
    OPENSSL_free(bnStr);
    return serNumStr;
}

static void asn1UTCTimeToPrettyString(const ASN1_UTCTIME *utc, std::string& s)
{
    ASN1_UTCTIME *t = (ASN1_UTCTIME *)(utc);
    s.erase();

    if (NULL != t && 1 == ASN1_UTCTIME_check(t)) {
        BIO *mem = BIO_new(BIO_s_mem());
        if (NULL != mem) {
            ASN1_UTCTIME_print(mem, t);
            char *strPtr = NULL;
            long strLen = BIO_get_mem_data(mem, &strPtr);
            s.append(strPtr, strLen);
            BIO_free(mem);
            return;
        }
    }

    // Invalid expiration time, use current time
    time_t now = Time::serverTime().seconds();
    ASN1_UTCTIME *tmp = ASN1_UTCTIME_set(NULL, now - 1);
    s.append((char *) ASN1_STRING_data(tmp));
    M_ASN1_UTCTIME_free(tmp);
    return;
}

std::string X509Cert::getNotBeforePrettyString() const {
    std::string notBeforeString("");
    if (!mX509)
        return notBeforeString;

    const ASN1_UTCTIME *notBefore = X509_get_notBefore(mX509);
    ::asn1UTCTimeToPrettyString(notBefore, notBeforeString);
    return notBeforeString;
}

std::string X509Cert::getNotAfterPrettyString() const {
    std::string notAfterString("");
    if (!mX509)
        return notAfterString;

    const ASN1_UTCTIME *notAfter = X509_get_notAfter(mX509);
    ::asn1UTCTimeToPrettyString(notAfter, notAfterString);
    return notAfterString;
}

#if DEBUG
void X509Cert::print() const {
    if (mX509) {
        BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
        X509_print(out, mX509);
        BIO_free(out);
    }
}
#endif

