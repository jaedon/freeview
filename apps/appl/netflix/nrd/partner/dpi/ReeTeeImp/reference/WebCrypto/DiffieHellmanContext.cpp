/*
 *  Copyright 2014 Netflix, Inc.
 *
 *     Licensed under the Apache License, Version 2.0 (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *
 */

#include "DiffieHellmanContext.h"
#include "NFBigNum.h"

#include <assert.h>
#include <algorithm>
#include <string>

#include <openssl/bn.h>
#include <openssl/dh.h>

#include <nrdbase/Log.h>

using namespace netflix;


//////////
//
// DiffieHellmanContext
//
//////////
DiffieHellmanContext::DiffieHellmanContext()
{
}

DiffieHellmanContext::~DiffieHellmanContext()
{
}

//////////
//
// init
//
//////////
bool DiffieHellmanContext::init(const DataBuffer& p,
                                const DataBuffer& g)
{
    // create a new DH
    dh_ = shared_ptr<DH>(DH_new(), DH_free);
    if (!dh_)
    {
        NTRACE(TRACE_CRYPTO,
               "DiffieHellmanContext::init: Unable to create DH using DH_new()");
        return false;
    }

    // ensure p is 1024 bits - this is a requirement to get a 128-bit key for AES
    if (p.size() != 128)
    {
        NTRACE(TRACE_CRYPTO, "DiffieHellmanContext::init: p is not 1024 bits");
        return false;
    }

    // DH_new creates a DH with p & g set to NULL; copy in our new values
    dh_->p = BN_dup(BigNum(p.data(), p.size()).getBIGNUM());
    if (!dh_->p)
    {
        NTRACE(TRACE_CRYPTO, "DiffieHellmanContext::init: Unable to duplicate DH prime "
               "using BN_dup()");
        return false;
    }
    dh_->g = BN_dup(BigNum(g.data(), g.size()).getBIGNUM());
    if (!dh_->g)
    {
        NTRACE(TRACE_CRYPTO, "DiffieHellmanContext::init: Unable to duplicate DH "
               "generator using BN_dup()");
        return false;
    }

    // check DH
    // NOTE: DH_check() does not seem to work if you set your own p and g on a
    // new DH context. My guess is that it is only appropriate after calling
    // DH_generate parameters(), which we are not doing here.
//    int errorBitMask = 0;
//    if (!DH_check(dh_.get(), &errorBitMask))
//    {
//        DLOG() << "DiffieHellmanContext::init: DH_check failed\n";
//        return false;
//    }
//    if (errorBitMask)
//    {
//        DLOG() << "DiffieHellmanContext::init: Parameters are not suitable: ";
//        if (errorBitMask & DH_CHECK_P_NOT_PRIME)
//            DLOG() << "DH_CHECK_P_NOT_PRIME ";
//        if (errorBitMask & DH_CHECK_P_NOT_SAFE_PRIME)
//            DLOG() << "DH_CHECK_P_NOT_SAFE_PRIME ";
//        if (errorBitMask & DH_UNABLE_TO_CHECK_GENERATOR)
//            DLOG() << "DH_UNABLE_TO_CHECK_GENERATOR ";
//        if (errorBitMask & DH_NOT_SUITABLE_GENERATOR)
//            DLOG() << "DH_NOT_SUITABLE_GENERATOR ";
//        DLOG() << "\n";
//        return false;
//    }

    // generate the pub/priv key pair
    if (!DH_generate_key(dh_.get()))
    {
        NTRACE(TRACE_CRYPTO, "DiffieHellmanContext::init: failed");
        return false;
    }

    return true;
}

//////////
//
// getPubKey
//
//////////
DataBuffer DiffieHellmanContext::getPubKey() const
{
    return BigNum(dh_->pub_key).encode();
}

//////////
//
// getPrivKey
//
//////////
DataBuffer DiffieHellmanContext::getPrivKey() const
{
    return BigNum(dh_->priv_key).encode();
}

//////////
//
// computeSharedSecret
//
//////////
bool DiffieHellmanContext::computeSharedSecret(
    const DataBuffer& peerPubKey)
{
    // make sure we have a pub/priv key
    if(!dh_->pub_key || !dh_->priv_key)
    {
        NTRACE(TRACE_CRYPTO,
               "DiffieHellmanContext::computeSharedSecret: missing local key");
        return false;
    }

    if (!peerPubKey.size())
    {
        NTRACE(TRACE_CRYPTO,
               "DiffieHellmanContext::computeSharedSecret: missing peer key");
        return false;
    }

    // get size needed for shared secret
    int outLen = DH_size(dh_.get());
    if(outLen != 128)
    {
        NTRACE(TRACE_CRYPTO, "DiffieHellmanContext::computeSharedSecret: shared secret "
               "(DH_size) not 128 bytes");
        return false;
    }

    // allocate and zero space for the shared secret
    sharedSecret_.resize(outLen);

    // compute the shared secret
    outLen = DH_compute_key(&sharedSecret_[0], BigNum(peerPubKey.data(), peerPubKey.size()).getBIGNUM(), dh_.get());
    if(outLen == -1 || outLen > 128)
    {
        NTRACE(TRACE_CRYPTO, "DiffieHellmanContext::computeSharedSecret: error computing "
               "shared secret");
        return false;
    }
    if (outLen != sharedSecret_.size())
    {
        sharedSecret_.resize(outLen);
    }

    return true;
}

//////////
//
// getSharedSecret
//
//////////
DataBuffer DiffieHellmanContext::getSharedSecret() const
{
    return sharedSecret_;
}
