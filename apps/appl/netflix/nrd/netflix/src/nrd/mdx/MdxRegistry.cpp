/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "MdxRegistry.h"

using namespace netflix::mdx;

shared_ptr<IMdx> MdxRegistry::sMdx;

void MdxRegistry::setMdx(shared_ptr<IMdx> mdx)
{
    sMdx = mdx;
}

shared_ptr<IMdx> MdxRegistry::getMdx()
{
    return sMdx;
}
