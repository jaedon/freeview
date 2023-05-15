/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef TrustStore_h
#define TrustStore_h

#include <nrdbase/tr1.h>
#include <nrdbase/DataBuffer.h>
#include <openssl/ssl.h>

namespace netflix {


class TrustStore
{
public:
    TrustStore(const DataBuffer &truststore);

    ~TrustStore();

    std::vector<X509*> x509s;

    std::string getHash() const { return mHash.toBase64String(); }

private:
    DataBuffer mHash;
};

typedef shared_ptr<TrustStore> TrustStorePtr;

}

#endif
