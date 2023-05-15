/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "Digester.h"

#include <nrdbase/OpenSSLException.h>
using namespace netflix;

Digester::Digester(shared_ptr<const DigestAlgo> algo)
    : algo_(algo)
{
    EVP_MD_CTX_init(&ctx_);
}

Digester::~Digester()
{
    EVP_MD_CTX_cleanup(&ctx_);
}

void Digester::init()
{
    if(!(EVP_DigestInit_ex(&ctx_, algo_->evp_md(), NULL))) {
        OPENSSLEXCEPTION_MSG("EVP_DigestInit_ex()");
    }
}

void Digester::update(const unsigned char *data, size_t sz)
{
    if(!(EVP_DigestUpdate(&ctx_, data, sz))) {
        OPENSSLEXCEPTION_MSG("EVP_DigestUpdate()");
    }
}

int Digester::final(std::vector<unsigned char> &buf)
{
    buf.resize(EVP_MAX_MD_SIZE);
    unsigned int buf_len = 0;
    if(!(EVP_DigestFinal_ex(&ctx_, &buf[0], &buf_len))) {
        OPENSSLEXCEPTION_MSG("EVP_DigestFinal_ex()");
    }
    buf.resize(buf_len);
    return buf_len;
}

int Digester::final(DataBuffer &buf)
{
    buf.resize(EVP_MAX_MD_SIZE);
    unsigned int buf_len = 0;
    if(!(EVP_DigestFinal_ex(&ctx_, buf.data(), &buf_len))) {
        OPENSSLEXCEPTION_MSG("EVP_DigestFinal_ex()");
    }
    buf.resize(buf_len);
    return buf_len;
}
