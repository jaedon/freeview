/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "DigestAlgo.h"

using namespace netflix;

shared_ptr<const DigestAlgo> DigestAlgo::SHA1()
{
    shared_ptr<const DigestAlgo> da(new DigestAlgo("SHA1", NID_sha1, EVP_sha1()));
    return da;
}

shared_ptr<const DigestAlgo> DigestAlgo::SHA224()
{
    shared_ptr<const DigestAlgo> da(new DigestAlgo("SHA224", NID_sha224, EVP_sha224()));
    return da;
}

shared_ptr<const DigestAlgo> DigestAlgo::SHA256()
{
    shared_ptr<const DigestAlgo> da(new DigestAlgo("SHA256", NID_sha256, EVP_sha256()));
    return da;
}

shared_ptr<const DigestAlgo> DigestAlgo::SHA384()
{
    shared_ptr<const DigestAlgo> da(new DigestAlgo("SHA384", NID_sha384, EVP_sha384()));
    return da;
}

shared_ptr<const DigestAlgo> DigestAlgo::SHA512()
{
    shared_ptr<const DigestAlgo> da(new DigestAlgo("SHA512", NID_sha512, EVP_sha512()));
    return da;
}

DigestAlgo::DigestAlgo(const std::string name, int nid, const EVP_MD *evp_md)
    : name_(name), nid_(nid), evp_md_(evp_md)
{}

