/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright nd other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "TrustStore.h"
#include <nrdbase/Digester.h>

using namespace netflix;

TrustStore::TrustStore(const DataBuffer &truststore)
{
    if (BIO *in = BIO_new_mem_buf(const_cast<unsigned char*>(truststore.data()), truststore.size())) {
        while (true) {
            X509 *x509 = PEM_read_bio_X509(in, 0, 0, 0);
            if (!x509)
                break;
            x509s.push_back(x509);
        }
        BIO_free(in);
    }

    Digester digester(DigestAlgo::SHA256());
    digester.init();
    digester.update(truststore);
    digester.final(mHash);
}

TrustStore::~TrustStore()
{
    const int count = x509s.size();
    for (int i=0; i<count; ++i)
        X509_free(x509s.at(i));
}

