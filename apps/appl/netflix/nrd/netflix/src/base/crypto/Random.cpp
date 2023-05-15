/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <openssl/rand.h>

#include <nrdbase/tr1.h>
#include <nrdbase/OpenSSLException.h>

#include "Random.h"
using namespace netflix;

unsigned char Random::NON_RANDOM_SEED[] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
};

bool Random::init_ = false;

std::vector<unsigned char> Random::next(size_t num_bytes)
{
    std::vector<unsigned char> val(num_bytes);
    next(&val[0], num_bytes);
    return val;
}

void Random::next(unsigned char * buffer, size_t num_bytes)
{
    if(!init_) {
        OPENSSLEXCEPTION_MSG("Random must be initialized before use");
    }
    if(!(RAND_bytes(buffer, num_bytes))) {
        OPENSSLEXCEPTION_MSG("RAND_bytes");
    }
}
