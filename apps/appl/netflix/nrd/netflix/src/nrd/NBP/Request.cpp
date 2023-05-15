/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Request.h"
#include <nrd/NrdApplication.h>

using namespace netflix;

Request::Request(const Variant &data, uint32_t flags)
    : mVariant(data), mProperty(-1), mMethod(-1), mFlags(flags)
{
    shared_ptr<NfObject> obj = nrdApp()->findObject(mVariant.mapValue<std::string>("object"));
    mObject = obj;
    assert(mVariant.isStringMap() || mVariant.isNull());
    mType = determineType();
    if (obj) {
        mFlags |= obj->requestFlags(*this);
    }
}
