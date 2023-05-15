/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "IDnsCache.h"

namespace netflix {


IDnsCache* IDnsCache::mInstance = NULL;

/** Set the IP connectivity manager instance. */
void IDnsCache::setInstance( IDnsCache* pDnsCache )
{
    mInstance = pDnsCache;
}

/** Destroy the IP connectivity manager instance. */
void IDnsCache::destroy()
{
    delete mInstance;

    mInstance = NULL;
}

/** Get the IP connectivity manager instance. */
IDnsCache* IDnsCache::instance()
{
    return mInstance;
}

}

