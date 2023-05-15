/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "IMediaFile.h"

NRDP_DEFINE_OBJECTCOUNT(IMediaFile);

using namespace netflix::ase;

IMediaFile::~IMediaFile()
{
    NRDP_OBJECTCOUNT_DEREF(IMediaFile, this);
}

