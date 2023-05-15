/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SystemDataKey.h"

#include <assert.h>

using namespace netflix;

std::string SystemDataKey::toString(Key key)
{
    switch (key)
    {
        case SYSTEM_FILE:                   return "systemFile";
        case UI_LANGUAGE:                   return "uiLanguage";
        // Don't add a default case here so we can catch anything missing
    }

    assert(0 && "Missing case in SystemDataKey::toString!");
    return "";
}

